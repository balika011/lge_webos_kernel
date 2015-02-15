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
 * $RCSfile: vdp_frc.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/*****************************************************************************
*  Notes: (MT5351)
*  (1) Deint initialize, 20050613
*      Z(657a800) Y(657a800) X(657a800) W(657a800), Normal Mode (ucDeintEn = 1, ucDeintNextEn = 1)
*      Z(657a800) Y(657a800) X(657a800) W(65e6800), Normal Mode (ucDeintEn = 1, ucDeintNextEn = 1)
*  ->  Z(657a800) Y(657a800) X(65e6800) W(65e6800), Deintelace Mode (ucDeintEn = 1, ucDeintNextEn = 0)
*  ->  Z(657a800) Y(65e6800) X(65e6800) W(64a2800), Deinterlace Mode (ucDeintEn = 1, ucDeintNextEn = 0)
*      ucDeintNextEn
*  (2) 32 pull down bad edit detection, 20050626
*      Z Y X W   (Frame n - 1) (Check comb register)
*        Z Y X W (Frame n)
*      At frame n, if merge ZY, check YX comb, if merge YX, check XW comb
*      At frame n, if merge ZY, YX < XW comb, if merge YX, XW < YX comb
*      At frame n, if merge YX, XW comb < (XW + threshold) comb of frame n - 3
*  (3) display fail (no frame buffer) at deinterlace mode, 20050621
*      Z(65e6800) Y(65e6800) X(6f6b800) W(6f6b800), XZ(53) WY(0) YX(0) WX(0) Reg(2014)
*      Z(65e6800) Y(6f6b800) X(6f6b800) W(650e800), XZ(0) WY(57) YX(0) WX(1340) Reg(2016)
*      Z(6f6b800) Y(6f6b800) X(650e800) W(650e800), XZ(57) WY(54) YX(1218) WX(0) Reg(2014)
*      Z(6f6b800) Y(650e800) X(650e800) W(6f6b800), XZ(54) WY(68) YX(0) WX(1488) Reg(2016)
*  ->  Z(6f6b800) Y(650e800) X(650e800) W(6f6b800), XZ(68) WY(54) YX(103) WX(3815) Reg(2016)
*  ->  Z(6f6b800) Y(650e800) X(650e800) W(6f6b800), XZ(54) WY(68) YX(1488) WX(0) Reg(2016)
*  ->  Z(650e800) Y(650e800) X(64a2800) W(6f6b800), XZ(68) WY(54) YX(103) WX(3815) Reg(2014)
*      a) we should skip bad edit detection, twice
*      b) we should update both X and W together when new frame rx
*      b) we should leave 32/22 pd mode
*  (4) VDP_HalSetDeintX, 20050621
*      Z = Y, Y = X, W = X = New
*      Save WXYZ at FRC, easy for chroma ZX flip
*      Update Address after function _VdpDeintHandleCounter, which will design still field direction (YZ or YX)
*      If merge YZ, flip chroma address of Z & X
*  (5) Access Frame, 20050621
*      ucProgressiveSeqFlag, ucValid32Seq, or NOT prFrcPrm->ucForceInterlaceSrc
*  (6) Access Top, 20050621
*      According to MPEG2 top field first
*  (7) 32 Pull Down Sequence
*      1  2->2  2
*         2<-2->2  3           '<-' or '->'? '->' is better, looking forward
*            2<-2  3  3
*               2  3->3  4
*                  3<-3  4  4
*      Example:
*      XZ(44) WY(39) YX(626) WX(0) Reg(2014)
*      XZ(39) WY(28) YX(0) WX(553) Reg(2014)
*      (0) PD Mode 1 >> 0
*      Bad-Edit(2) (27, 1c, 0, 229)
*  (8) Increase 32 motion threshold from 8 to 64 for Ԥh & TS pattern, 20050628
*  (9) 32 flag detection, 20050628
*      8 >> 24, make it more difficult to enter 32 flag
*      enter 32 pull down mode in the future
*  (10) Error handle at deinterlace mode, 20050628
*      When we detect discontiuous termporal reference, we will pause
*      deinterlace for 2 field, which will reduce sawtooth when error happen
*  (11) 32 pull down bad edit detection, 20050630
*      Z Y X W   (Frame n - 1) (Check comb register)
*        Z Y X W (Frame n)
*      At frame n, if merge ZY, check YX comb, if merge YX, check XW comb (Skip)
*      At frame n, if merge ZY, YX < XW comb, if merge YX, XW < YX comb (Skip)
*      Check motion counter
*  (12) Refine AV Sync timing, STC' = STC + 1 Vsync Time, display 1 Vsync earlier
*  (13) Scene Change, 20050630
*      If Current YW motion > (Previous YW motion + Threshold)
*      Based on MT8205 motion register
*  (14) I to I without deinterlace, 20050707
*      50i to 60i or 60i to 50i, playback Top only (DC)
*  (15) Bug Fixed of (10)
*      I we pause deinterlace, a flicker will happen while changing mode
*      worst than sawtooth!
*****************************************************************************/

/*****************************************************************************
*  Notes: (MT5371)
*  (1) If multiple field change due to TFF/RFF (_VdpBaseVSyncNs), skip FRC process
*****************************************************************************/


/** @file vdp_frc.c
 *  Brief of file vdp_frc.c.
 *  Details of file vdp_frc.c (optional).
 *  Porting From DTV_X_IDTV0801 1.8.2.88
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "vdp_drvif.h"
#include "vdp_if.h"  // Trick Play at 20081001
#include "b2r_if.h"
#include "fbm_drvif.h"
#include "b2r_drvif.h"
#include "vdp_frc.h"
#include "vdp_vsync.h"
#include "vdp_hal.h"
#include "vdp_debug.h"
#include "drv_di.h"
#include "aud_drvif.h"
#include "aud_if.h"
#include "imgrz_if.h"
#include "imgrz_drvif.h"
#include "vdec_drvif.h"
#include "mute_if.h"
#include "tve_if.h"
#include "x_bim.h"
#ifdef CC_MAPLE_CUST_DRV
#include "pe_if.h"
#endif
#ifdef ENABLE_MULTIMEDIA
#include "swdmx_if.h"
#endif

#include "x_lint.h"
//LINT_EXT_HEADER_BEGIN
#ifdef __MODEL_slt__
#include "slt_if.h"
#endif
#include "gfx_if.h"
#include "gfx_drvif.h"
#include "vdo_rm.h"
#include "drv_common.h"
#include "drv_dbase.h"
#ifdef CC_SCPOS_EN
#include "drv_scpos.h"
#include "drv_video.h"
#endif
#include "x_hal_5381.h"
#include "x_os.h"
#include "x_assert.h"
#include "x_syslog.h"
#include "x_timer.h"
#ifdef TIME_MEASUREMENT
#include "x_time_msrt.h"
#include "u_time_msrt_name.h"
static BOOL fgB2REarlyDisp = FALSE;
#endif
//LINT_EXT_HEADER_END

//LINT_SUPPRESS_BRACE(717)        // Info 717: do ... while(0);
#ifdef CC_SUPPORT_FULL_3D_PHOTO
enum
{
	LEFT_TOP = 0,
	RIGHT_TOP,
	LEFT_BOTTOM,
	RIGHT_BOTTOM,
	INVALID_REGION
};
#endif

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

//#define CC_NETFLIX_SUPPORT

/// Handle 32 Flag
#define VDP_HANDLE_32FLAG

#define VDP_EARLY_DISPLAY
//CLI test for Nordig HDTV overscan and frame cropping
//#define CC_CLI_TEST

// Monitor TFF
//#define VDP_MONITOR_TFF

// Frc Test
//#define CC_FRC_TEST

// Force 32 flag error
//#define CC_FORCE_32_FLAG_ERR

// 32 flag enable/disable test
//#define VDP_TEST_32FLAG

// Work around, Interlace FRC Fail, Between B2R/SCPOS
//#define CC_VDP_NO_INTERLACE_FRC

//#define CC_VDP_FPGA

//#define CC_VDP_FORCE_FIELD_ENCODE

//#define CC_VDP_FRC_DEBUG

//#define WA_1080P_SUB_NO_FRC // 1080p at sub, no 24/30 to 60 conversion

#define CC_B2R_ENABLE_3D_LATE_NOTIFY
#define CC_B2R_ENABLE_R_VIEW_SKIP_FRAME     // Let 3D R view also drop frame (like L view case).


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#undef min
#define min(x, y) (((x) < (y)) ? (x) : (y))
#undef max
#define max(x, y) (((x) > (y)) ? (x) : (y))
#undef abs
#define abs(x) (((x) >= 0) ? (x) : -(x))
#undef abs_min
#define abs_min(x, y) ((abs(x) < abs(y)) ?  x : y)

#define VDP_STC_CLOCK                   (90000)                     // 90 KHz
#define VDP_WAIT_1ST_PTS_NS             (12)
#define VDP_WAIT_NONZERO_PTS_NS         (5)
#define VDP_ZERO_PTS_INGORE_FILTER      (4)

#ifdef MHEG5_IC_SUPPORT_CERT
/* Modify for passing DTG MHEG5 ICS Test Suite  */
#define VDP_INVALID_DRIFT		        (VDP_STC_CLOCK * 1 / 10) // 100ms
#else
#define VDP_INVALID_DRIFT		        (VDP_STC_CLOCK * 10)		 // 6 Seconds
#endif

#define VDP_EARLY_DISP_DRIFT		    (VDP_STC_CLOCK * 10)		 // 1 Seconds

#ifdef MHEG5_IC_SUPPORT_CERT
/* Modify for passing DTG MHEG5 ICS Test Suite  */
#define VDP_MM_PEND_MAX                 (VDP_STC_CLOCK * 1 / 10) // 100ms
#else
#define VDP_MM_PEND_MAX                 (VDP_STC_CLOCK * 20)     //20 Seconds
#endif

#define VDP_INVALID_ASYNC_NS            (10)
#define VDP_MAX_PTS_FILTER_LEN          (4)                         // 4 tap, Enhance for SEOUL_CH11_1_RF14
#define VDP_MAX_PTS_DRIFT               (900)                       // 10 ms, Enhance for SEOUL_CH11_1_RF14
#define VDP_KEEP_DROP_B_NS              (10)                        // Enhance for SEOUL_CH11_1_RF14
#ifndef CC_B2R_DISABLE_ONE_SECOND_FRC
#define VDP_MAX_ASYNC_REPEAT_NS         (15)
#else
#define VDP_MAX_ASYNC_REPEAT_NS         (150)                        // should not more than 125
#endif
#define VDP_MAX_DEINT_WIDTH             (720)
#define VDP_DEBUG_BUF_SIZE              (16)
#define VDP_AVSYNC_TH                   (50 * VDP_STC_CLOCK / 1000) // 50 ms
#define VDP_VSYNC_CB_THD                (20)                        // 20 VSync ~= 330 ms

// after field trail (ch48_f49_0530_signal84_V_Shake)
#define PD_32_FLAG_LOOP_SIZE            (12)                        // 0.5 second
#define PD_32_FLAG_LEAVE_LOOP_SIZE      (6)                         // 0.25 second
#define PD_32_FLAG_SEQ                  (4)

#define VDP_DISPLAY_FAIL_FREEZE         (255)

#define VDP_TFF_STABLE_COUNT            (12)

#define VDP_VIDEO_DELAY_VSYNC           (4)

#define VDP_WAIT_UNMUTE_VSYNC           (255)
#define DURATION_TO_SEND_SEEK_DONE 5

#define STC_PTS_DIFF    (7500)

#define VDP_LIP_SYNC_OK 0
#define VDP_LIP_SYNC_NG 1

//WFD related parameter
#define VDP_STATISTICS_PER_SECOND            (5)
#define VDP_MICROSECOND_PER_SECOND           (1000000)
#define VDP_RT_LATENCY_AJD_CNT               (8)
#define VDP_RT_LATENCY_TOLERATE              (2)
#define VDP_RT_LATENCY_LIMIT                 (25)
#define VDP_RT_DIV_TWO                       (2)
#define VDP_RT_INITIAL_CNT                   (1)//time = VDP_INITIAL_CNT * u4OutStcPeriod
#define VDP_RT_STATICE_CNT                   (300)//monitor packet input time interval
#define VDP_RT_LATENCY_TIME                  (40) //wfd latency time limit is 25ms ,so 1000ms / 25ms = 40
#define VDP_RT_STC_ADJ_CNT                   (5)
#define VDP_RT_MULTIPLY_THREE                (3)
#define VDP_RT_STC_ADJ                       (100)

#ifndef VDP_ADD_1_VSYNC_TIME
#define VDP_ADD_1_VSYNC_TIME
#endif

typedef struct
{
    VDP_OUT_INFO_T rOutInfo;
    UINT32 u4FrmRateMsk ;
    UINT32 u4ClkModeExp;
}VDP_OUT_MODE_T;

#define Msk(x)  (1 << (x))

//oxff means this value can't be used (or we don't use it ) in arOutInfo!
VDP_OUT_MODE_T arModeInfo[VDP_B2R_MODE_MAX] =
{
    {{0xff, VDP_B2R_MODE_480I_60,0,60,1,0xff,720,480,858,525, VDP_B2R_CLK_27_000},Msk(MPEG_FRAME_RATE_30), VDP_B2R_CLK_27_027},
    {{0xff, VDP_B2R_MODE_480I_120,0,120,1,0xff,720,480,858,525, VDP_B2R_CLK_54_000},Msk(MPEG_FRAME_RATE_30), VDP_B2R_CLK_54_054},
    {{0xff, VDP_B2R_MODE_576I_50,0,50,1,0xff,720,576,864,625, VDP_B2R_CLK_27_000},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_576I_100,0,100,1,0xff,720,576,864,625, VDP_B2R_CLK_54_000},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_1080I_48,0,48,1,0xff,1920,1080,2750,1125, VDP_B2R_CLK_148_352},Msk(MPEG_FRAME_RATE_24), VDP_B2R_CLK_148_500},
    {{0xff, VDP_B2R_MODE_1080I_50,0,50,1,0xff,1920,1080,2640,1125, VDP_B2R_CLK_148_500},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_1080I_60,0,60,1,0xff,1920,1080,2200,1125, VDP_B2R_CLK_148_352},Msk(MPEG_FRAME_RATE_24)|Msk(MPEG_FRAME_RATE_25)|Msk(MPEG_FRAME_RATE_30), VDP_B2R_CLK_148_500},
    {{0xff, VDP_B2R_MODE_1080I_100,0,100,0,0xff,1920,1080,2640,1125, VDP_B2R_CLK_148_500},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_1080I_120,0,120,0,0xff,1920,1080,2200,1125, VDP_B2R_CLK_148_352},Msk(MPEG_FRAME_RATE_30), VDP_B2R_CLK_148_500},
    {{0xff, VDP_B2R_MODE_480P_24,1,24,2,0xff,720,480,858,525, VDP_B2R_CLK_27_000},Msk(MPEG_FRAME_RATE_24), VDP_B2R_CLK_27_027},
    {{0xff, VDP_B2R_MODE_480P_30,1,30,1,0xff,720,480,858,525, VDP_B2R_CLK_27_000},Msk(MPEG_FRAME_RATE_30), VDP_B2R_CLK_27_027},
    {{0xff, VDP_B2R_MODE_480P_48,1,48,3,0xff,720,480,858,525, VDP_B2R_CLK_27_000},Msk(MPEG_FRAME_RATE_24), VDP_B2R_CLK_27_027},
    {{0xff, VDP_B2R_MODE_480P_60,1,60,0,0xff,720,480,858,525, VDP_B2R_CLK_27_000},Msk(MPEG_FRAME_RATE_24)|Msk(MPEG_FRAME_RATE_30)|Msk(MPEG_FRAME_RATE_60), VDP_B2R_CLK_27_027},
    {{0xff, VDP_B2R_MODE_480P_120,1,120,0,0xff,720,480,858,525, VDP_B2R_CLK_54_000},Msk(MPEG_FRAME_RATE_24)|Msk(MPEG_FRAME_RATE_30)|Msk(MPEG_FRAME_RATE_60), VDP_B2R_CLK_54_054},
    {{0xff, VDP_B2R_MODE_576P_25,1,25,1,0xff,720,576,864,625, VDP_B2R_CLK_27_000},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_576P_50,1,50,0,0xff,720,576,864,625, VDP_B2R_CLK_27_000},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_576P_100,1,100,0,0xff,720,576,864,625, VDP_B2R_CLK_54_000},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_720P_24,1,24,2,0xff,1280,720,1650,750, VDP_B2R_CLK_74_176},Msk(MPEG_FRAME_RATE_24), VDP_B2R_CLK_74_250},
    {{0xff, VDP_B2R_MODE_720P_25,1,25,1,0xff,1280,720,1980,750, VDP_B2R_CLK_74_250},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_720P_30,1,30,1,0xff,1280,720,1650,750, VDP_B2R_CLK_74_176},Msk(MPEG_FRAME_RATE_30), VDP_B2R_CLK_74_250},
    {{0xff, VDP_B2R_MODE_720P_48,1,48,2,0xff,1280,720,1650,750, VDP_B2R_CLK_148_352},Msk(MPEG_FRAME_RATE_24), VDP_B2R_CLK_148_500},
    {{0xff, VDP_B2R_MODE_720P_50,1,50,0,0xff,1280,720,1980,750, VDP_B2R_CLK_74_250},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_720P_60,1,60,0,0xff,1280,720,1650,750, VDP_B2R_CLK_74_176},Msk(MPEG_FRAME_RATE_24)|Msk(MPEG_FRAME_RATE_30)|Msk(MPEG_FRAME_RATE_60), VDP_B2R_CLK_74_250},
    {{0xff, VDP_B2R_MODE_720P_100,1,100,0,0xff,1280,720,1980,750, VDP_B2R_CLK_148_500},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_720P_120,1,120,0,0xff,1280,720,1650,750, VDP_B2R_CLK_148_352},Msk(MPEG_FRAME_RATE_24)|Msk(MPEG_FRAME_RATE_30)|Msk(MPEG_FRAME_RATE_60), VDP_B2R_CLK_148_500},
    {{0xff, VDP_B2R_MODE_720P_240,1,240,0,0xff,1280,720,1650,750, VDP_B2R_CLK_297_000},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_1080P_24,1,24,0,0xff,1920,1080,2750,1125, VDP_B2R_CLK_74_176},Msk(MPEG_FRAME_RATE_24), VDP_B2R_CLK_74_250},
    {{0xff, VDP_B2R_MODE_1080P_25,1,25,0,0xff,1920,1080,2640,1125, VDP_B2R_CLK_74_250},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_1080P_30,1,30,0,0xff,1920,1080,2200,1125, VDP_B2R_CLK_74_176},Msk(MPEG_FRAME_RATE_30), VDP_B2R_CLK_74_250},
    {{0xff, VDP_B2R_MODE_1080P_48,1,48,0,0xff,1920,1080,2750,1125, VDP_B2R_CLK_148_352},Msk(MPEG_FRAME_RATE_24), VDP_B2R_CLK_148_500},
    {{0xff, VDP_B2R_MODE_1080P_50,1,50,0,0xff,1920,1080,2640,1125, VDP_B2R_CLK_148_500},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_1080P_60,1,60,0,0xff,1920,1080,2200,1125, VDP_B2R_CLK_148_352},Msk(MPEG_FRAME_RATE_24)|Msk(MPEG_FRAME_RATE_30)|Msk(MPEG_FRAME_RATE_60), VDP_B2R_CLK_148_500},
    {{0xff, VDP_B2R_MODE_480P_1440W_24,1,24,2,0xff,1440,480,1716,525, VDP_B2R_CLK_54_000},Msk(MPEG_FRAME_RATE_24), VDP_B2R_CLK_54_054},
    {{0xff, VDP_B2R_MODE_480P_1440W_60,1,60,0,0xff,1440,480,1716,525, VDP_B2R_CLK_54_000},Msk(MPEG_FRAME_RATE_24)|Msk(MPEG_FRAME_RATE_30)|Msk(MPEG_FRAME_RATE_60), VDP_B2R_CLK_54_054},
    {{0xff, VDP_B2R_MODE_1440P_1280W_24,1,24,2,0xff,1280,1440,1500,1650, VDP_B2R_CLK_148_352},Msk(MPEG_FRAME_RATE_24), VDP_B2R_CLK_148_500},
    {{0xff, VDP_B2R_MODE_1440P_1280W_25,1,25,0,0xff,1280,1440,1800,1650, VDP_B2R_CLK_74_250},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_1440P_1280W_30,1,30,1,0xff,1280,1440,1500,1650, VDP_B2R_CLK_148_352},Msk(MPEG_FRAME_RATE_30), VDP_B2R_CLK_148_500},
    {{0xff, VDP_B2R_MODE_1440P_1280W_50,1,50,0,0xff,1280,1440,1800,1650, VDP_B2R_CLK_148_500},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_1440P_1280W_60,1,60,0,0xff,1280,1440,1500,1650, VDP_B2R_CLK_148_352},Msk(MPEG_FRAME_RATE_24)|Msk(MPEG_FRAME_RATE_30)|Msk(MPEG_FRAME_RATE_60), VDP_B2R_CLK_148_500},
    {{0xff, VDP_B2R_MODE_720P_1080W_24,1,24,0,0xff,1920,720,3648,848, VDP_B2R_CLK_74_176},Msk(MPEG_FRAME_RATE_24), VDP_B2R_CLK_74_250},
    {{0xff, VDP_B2R_MODE_720P_1080W_25,1,25,0,0xff,1920,720,3502,848, VDP_B2R_CLK_74_250},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_720P_1080W_30,1,30,0,0xff,1920,720,2919,848, VDP_B2R_CLK_74_176},Msk(MPEG_FRAME_RATE_30), VDP_B2R_CLK_74_250},
    {{0xff, VDP_B2R_MODE_720P_1080W_50,1,50,0,0xff,1920,720,3502,848, VDP_B2R_CLK_148_500},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_720P_1080W_60,1,60,0,0xff,1920,720,2919,848, VDP_B2R_CLK_148_352},Msk(MPEG_FRAME_RATE_24)|Msk(MPEG_FRAME_RATE_30)|Msk(MPEG_FRAME_RATE_60), VDP_B2R_CLK_148_500},
    {{0xff, VDP_B2R_MODE_1080P_ULTRAW_24,1,24,0,0xff,2560,1080,2750,1125, VDP_B2R_CLK_74_176},Msk(MPEG_FRAME_RATE_24), VDP_B2R_CLK_74_250},
    {{0xff, VDP_B2R_MODE_1080P_ULTRAW_25,1,25,0,0xff,2560,1080,2640,1125, VDP_B2R_CLK_74_250},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_1080P_ULTRAW_30,1,30,0,0xff,2560,1080,4400,1125, VDP_B2R_CLK_148_352},Msk(MPEG_FRAME_RATE_30), VDP_B2R_CLK_148_500},
    {{0xff, VDP_B2R_MODE_1080P_ULTRAW_50,1,50,0,0xff,2560,1080,2640,1125, VDP_B2R_CLK_148_500},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_1080P_12,1,12,0,0xff,1920,1080,2200,1125, VDP_B2R_CLK_29_700},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_1080P_15,1,15,0,0xff,1920,1080,2200,1125, VDP_B2R_CLK_37_125},0,VDP_B2R_CLK_UNKNOWN},
    {{0xff, VDP_B2R_MODE_1080P_20,1,20,0,0xff,1920,1080,2200,1125, VDP_B2R_CLK_49_500},0,VDP_B2R_CLK_UNKNOWN}
#ifdef CC_MT5399
    ,
	{{0xff, VDP_B2R_MODE_4K2K_15,1,15,0,0xff,3840,2160,4400,2250, VDP_B2R_CLK_148_500},0,VDP_B2R_CLK_UNKNOWN},
	{{0xff, VDP_B2R_MODE_4K1K_30,1,30,0,0xff,3840,1080,4400,1125, VDP_B2R_CLK_148_500},0,VDP_B2R_CLK_UNKNOWN},
	{{0xff, VDP_B2R_MODE_4K2K_30,1,30,0,0xff,3840,2160,4400,2250, VDP_B2R_CLK_297_000},0,VDP_B2R_CLK_UNKNOWN},
	{{0xff, VDP_B2R_MODE_4K2K_24,1,24,0,0xff,3840,2160,5500,2250, VDP_B2R_CLK_297_000},0,VDP_B2R_CLK_UNKNOWN},
	{{0xff, VDP_B2R_MODE_4K2K_25,1,25,0,0xff,3840,2160,5280,2250, VDP_B2R_CLK_297_000},0,VDP_B2R_CLK_UNKNOWN},
	{{0xff, VDP_B2R_MODE_EX_4K2K_25,1,25,0,0xff,4096,2304,5280,2250, VDP_B2R_CLK_297_000},0,VDP_B2R_CLK_UNKNOWN},
	{{0xff, VDP_B2R_MODE_EX_4K2K_30,1,30,0,0xff,4096,2304,4400,2250, VDP_B2R_CLK_297_000},0,VDP_B2R_CLK_UNKNOWN},
	{{0xff, VDP_B2R_MODE_EX_4K2K_24,1,24,0,0xff,4096,2304,5500,2250, VDP_B2R_CLK_297_000},0,VDP_B2R_CLK_UNKNOWN}
#endif
#ifdef CC_B2R_SENSIO_CONV
    ,
	{{0xff, VDP_B2R_MODE_960_1080P_48 ,1, 48,0,0xff, 960,1080,1375,1125,  VDP_B2R_CLK_74_250},0,VDP_B2R_CLK_UNKNOWN},
	{{0xff, VDP_B2R_MODE_960_720P_120 ,1,120,0,0xff, 640, 720, 825, 750,  VDP_B2R_CLK_74_250},0,VDP_B2R_CLK_UNKNOWN},
	{{0xff, VDP_B2R_MODE_960_1080I_60 ,0,120,1,0xff, 960,1080,1100,1120, VDP_B2R_CLK_148_500},0,VDP_B2R_CLK_UNKNOWN}
#endif    
};

#define FHD_HEIGHT         (1088)
#define FHD_ALIGN_VALUE    (8)
#define HEIGHT_MUST_ALIGN(u4_height) ((u4_height) == FHD_HEIGHT)

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

typedef void (*VDP_SET_ADDR_FUNC)(UCHAR ucVdpId, UINT32 u4AddrY, UINT32 u4AddrC);

/** Brief of B2R_PRM_T
 */
typedef struct _VDP_FREEZE_PRM_T
{
    UINT32 u4OnOff;
    UINT32 u4CopyField;
    UINT32 u4Recovery;
    UINT32 u4UnFreeze;
    UINT32 u4CleanDispQ;
} VDP_FREEZE_PRM_T;

/** Brief of VDP_GFX_COPY_FIELD_PRM_T
 */
typedef struct _VDP_GFX_COPY_FIELD_PRM_T
{
    UCHAR ucFbgId;
    UCHAR ucFbId;
    UINT32 u4Init;
    HANDLE_T hSemaphore;
    HANDLE_T hMutex;
} VDP_GFX_COPY_FIELD_PRM_T;

typedef struct _VDP_FRC_COPY_PRM_T
{
    B2R_PRM_T rPortPrm;
    UINT32 u4TriggerCopy;
} VDP_FRC_COPY_PRM_T;

typedef struct _VDP_LIPSYNC_OK_NG_PRM_T
{
    UCHAR ucLipSyncState; // OK or NG
    UCHAR ucNg2OkCbEn;
    UCHAR ucNg2OkCbDone;
    UINT32 u4NumFrmsOK; //Number of frames after Lip-sync is OK
    UINT32 u4OKThrd;
    UINT32 u4OKFrmCnt; // Number of Frames OK accumulated
    UCHAR ucOk2NgCbEn;
    UCHAR ucOk2NgCbDone;
    UINT32 u4NumFrmsNG; //Number of frames after Lip-sync is NG
    UINT32 u4NGThrd;
    UINT32 u4NGFrmCnt; // Number of Frames NG accumulated
} VDP_LIPSYNC_OK_NG_PRM_T;

typedef struct _VDP_B2R_CRC_PRM_T
{
    UINT32 u4DramIndex;
    UINT32 u4DramChkSum;
    UINT32 u4PixelChkSum;
    UINT32 u4PSCANCrc;
    UINT32 u4PSCANSrcCrc;
} VDP_B2R_CRC_PRM_T;

#ifdef VDP_AVS_ADJUST_STC
typedef struct _B2R_AVS_T
{
    INT64   i8Delta;
    UINT32  u4Delay;
    BOOL    fgTest;
} B2R_AVS_T;
#endif

#ifdef HANDLE_PTS_SKIP
typedef struct _B2R_AVSYNC_CTRL_T
{
    BOOL    fgAVSyncOnOff;
    UINT32  u4AVSyncSpeed;
    UINT32  u4CurCycleRepeatCnt;
} B2R_AVSYNC_CTRL_T;
#endif
#ifdef ENABLE_MULTIMEDIA
#define RT_KEEP_PTS_MAX_CNT 30
#define RT_ADJUST_THRESHOLD  3000
#define RT_VALID_DELTA_THRESHOLD  (RT_KEEP_PTS_MAX_CNT*9000)

typedef enum {
    RT_IDX_VID = 0,
    RT_IDX_AUD = 1,
    RT_IDX_NS,
} VDP_RT_IDX;

typedef struct _B2R_RT_ADJUST_T
{
    INT64   i8BufAver[RT_IDX_NS];  //maybe not necessary to be an array 
    UINT32  i4BufMin[RT_IDX_NS];
    UINT32  i4AverCount;

    UINT32  u4NoFBCnt;    //Record change FB fail Cnt;If The next coming FB's PTS is match the cnt*inFrame, we do not process the PTS Jitter.
    UCHAR   ucInfoStatus; //0--Invalid;  1- Valid; 2--more
}B2R_RT_ADJUST_T;
#endif

typedef struct _B2R_WFD_PTS_INFO_T
{
    UINT32  u4_vid_pts;
    UINT32  u4_aud_pts;
    INT32   i4_vid_diff;
    INT32   i4_aud_diff;
    UINT32  u4CurStc;
    HAL_TIME_T  rDeltaTime;
} B2R_WFD_PTS_INFO_T;

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define FRC_INC_VSYNC_NS(pFrcPrm)                                        \
          do {                                                            \
            if (pFrcPrm->u2CurrentVsyncNs < 0xFF)                        \
            { pFrcPrm->u2CurrentVsyncNs++; }                            \
          } while (0)

#define FRC_SET_VSYNC_NS(pFrcPrm, u2Ns, u2ChgNs)                        \
          do {                                                            \
            pFrcPrm->u2CurrentVsyncNs = 0;                                \
            pFrcPrm->u2ChangeFieldVsyncNs = u2ChgNs;                    \
            pFrcPrm->u2TargerVsyncNs = u2Ns;                            \
          } while (0)

#define FRC_DISP_NS(pFrcPrm)                                            \
    ((pFrcPrm->u2CurrentVsyncNs >= pFrcPrm->u2TargerVsyncNs) ? 0 : (pFrcPrm->u2TargerVsyncNs - pFrcPrm->u2CurrentVsyncNs))

#define FRC_CHANGE_FIELD_NS(pFrcPrm)                                    \
    ((pFrcPrm->u2CurrentVsyncNs >= pFrcPrm->u2ChangeFieldVsyncNs) ? 0 : (pFrcPrm->u2ChangeFieldVsyncNs - pFrcPrm->u2CurrentVsyncNs))

#define FRC_DISP_FINISH(pFrcPrm)                                        \
    (pFrcPrm->u2CurrentVsyncNs >= pFrcPrm->u2TargerVsyncNs)

#define FRC_CHANGE_FIELD(pFrcPrm)                                        \
    (pFrcPrm->u2CurrentVsyncNs == pFrcPrm->u2ChangeFieldVsyncNs)

#define FRC_MEMSET(addr, value, len)    (VERIFY(x_memset((addr), value, len) == (addr)))


#define GUESS_32_STEP(tff, rff)         (((1 - tff) << 1) + rff)

#define LOG_REL(FLAG, REL_LVL, DBG_LVL, MSG...)     \
    if (FLAG) { LOG(REL_LVL, MSG); } else { LOG(DBG_LVL, MSG); }

#define PENDING_IS_NEEDED   (prFrcPrm->ucPendingFbId != FBM_FB_ID_UNKNOWN)
#define REFETCH_IS_NEEDED   (fgDropFb && !prFrcPrm->fgDummyFrame && !fgGstPlayBack)

#define VDP2B2RID(ucVdpId)              (_prVdpConf[ucVdpId]->ucB2rId) 
//-----------------------------------------------------------------------------
// Imported variables
//-----------------------------------------------------------------------------

#ifdef CC_FAST_CHANGE_CHANNEL_LOG
EXTERN HAL_TIME_T rNvgtStartAvTime;
EXTERN HAL_TIME_T rNvgtVideoStartTime;
EXTERN HAL_TIME_T rNvgtAvSyncTime;
EXTERN UINT32 u4NvgtTimeAhead;
#endif

#ifdef VDP_AVS_ADJUST_STC
B2R_AVS_T _rB2rAvs;
#endif

#ifdef TIME_MEASUREMENT
//EXTERN UINT32 u4RecvCnt, u4RendCnt, u4DropCnt, u4ReptCnt, u4FBLateCnt;
//EXTERN UINT32 u4Rend16ms, u4Rend33ms,u4Rend48ms,u4Rend66ms, u4Rend99ms, u4Rendxms;
//EXTERN UINT32 u4Delay16ms, u4Delay33ms, u4Delay48ms,u4Delay66ms,u4Delay99ms, u4Delayxms;
//EXTERN UINT64 u8Totaltime;
UINT32 u4TimeAver=0; //ms
UINT32 u4TimeAver100=0; //ms
UINT32 u4FrameCount=0;
UINT32 u4TimeCurr=0; //ms
UINT64 u8TimeCount =0;
UINT64 u8TimeCount100 =0;
#endif

//-----------------------------------------------------------------------------
// Imported functions
//-----------------------------------------------------------------------------
extern BOOL STC_ChkStcStatusDrift(UINT8 u1StcId);

extern BOOL STC_ChkStcStatusValid(UINT8 u1StcId);

extern BOOL _VDP_FlushB2RChgFrameMsg(UCHAR ucVdpId);

//-----------------------------------------------------------------------------
// Static function forward declarations
//-----------------------------------------------------------------------------

//extern BOOL fgApiVideoStable(UINT8 u1VideoFrame); // 0: main frame, 1: sub frame for PIP/POP

#ifdef CC_B2R_GFX_COPY_FIELD
static void _VdpGfxRoutine(void* pvArg);
#endif

#ifdef CC_B2R_ENABLE_SEAMLESS_THREAD
static VOID _SeamlessRoutine(void* pvArg);
static VOID _VDP_SeamlessJob(B2R_PRM_T* prFrcPrm);
#endif

static void _VdpGfxCopyFieldInit(void);

static UINT32 _VdpGfxCopyFieldStart(UCHAR ucFbgId, UCHAR ucFbId);

static void _VdpFrmInit(UCHAR ucB2rId, UCHAR ucPort, BOOL fgCreateThread);

static void _VdpChangeFrameBuffer(B2R_PRM_T* prFrcPrm);

static BOOL _VdpGetSequenceInfo(B2R_PRM_T* prFrcPrm, BOOL fgPreChk);

static void _VdpGetNextDisp(B2R_PRM_T* prFrcPrm);

static UCHAR _VdpPreLookNextDisp(B2R_PRM_T* prFrcPrm);

static void _VdpCalculateVsyncNs(B2R_PRM_T* prFrcPrm);

static void _VdpBaseVSyncNs(B2R_PRM_T* prFrcPrm);

static void _VdpFrameRateProc(B2R_PRM_T* prFrcPrm);

static void _VdpAVSyncProc(B2R_PRM_T* prFrcPrm);

static void _VdpChk32Flag(B2R_PRM_T* prFrcPrm);

static void _VdpSetOutInfoByPmx(UCHAR ucPmxMode,VDP_OUT_INFO_T *prOutInfo);

static void _VdpCheckOutputMode(B2R_PRM_T* prFrcPrm);

static void _VdpErrNotify(UINT32 u4Arg1, UINT32 u4Arg2, UINT32 u4Arg3, UINT32 u4Arg4);

static void _VdpFrcFrameBufferReady(UCHAR ucFbgId);

static void _VdpTriggerAudOn(UCHAR ucB2rId, UINT32 u4CurPts);

static void _VDP_SetXYOffset(UCHAR ucB2rId);

static UINT32 _VDP_FrcGetStc(UCHAR ucFbgId);

static BOOL _VdpHandleEos(B2R_PRM_T* prFrcPrm);

static UCHAR _VdpFreeFrameBeforeEosHandler(B2R_PRM_T* prFrcPrm, BOOL fgMain, BOOL fgOMX, BOOL* fgEos);

static BOOL _VdpChangeFrameDirectForOMX(B2R_PRM_T* prFrcPrm, VDP_B2R_CHG_FRAME_MSG_T* prMsg);

#ifdef CC_3D_MM_DS_SUPPORT
static void _VdpGetFrameBufferForMVC3D(B2R_PRM_T* prFrcPrm);
#endif

static void _VdpGetFrameBufferForOMX(B2R_PRM_T* prFrcPrm, BOOL* pfgGstPlayBack, UCHAR ucB2rId);

static void _VdpGetFrameBufferForSkype(B2R_PRM_T* prFrcPrm);

static void _VdpGetFrameBufferForGeneral(B2R_PRM_T* prFrcPrm, BOOL* pfgGstPlayBack);

static BOOL _VdpIsFastForward(UCHAR ucB2rId);

static void _VdpSetFrameBufferStatus(B2R_PRM_T* prFrcPrm, UCHAR ucFbgId, UCHAR ucFbId, UCHAR ucFbStatus);

/* VDP Seek done notification */
void _VDPSetSeek(UCHAR ucB2rId, UCHAR ucSet, UINT32 u4Pts);
void _VDPSetABRepeat(UCHAR ucB2rId, UCHAR ucSet);
static void _VDPGetSeek(UCHAR ucB2rId, UCHAR* pucSeekMode, UINT32* pu4Pts, UCHAR* pucNotifyTrigger);

#ifdef CC_FRC_TEST
static void _VdpFrcTest(B2R_PRM_T* prFrcPrm);
#endif

#ifdef VDP_AVS_ADJUST_STC
static VOID _VDP_SetAvsyncDelta(B2R_AVS_T* prAvs, UINT32 u4Delta,BOOL bAddFlg);
static VOID _VDP_SetDeltaInit(B2R_AVS_T* prAvs);
#endif

static void _VdpHandleResolutionChange(B2R_PRM_T* prFrcPrm);
static void _VdpHandleBitRateChange(B2R_PRM_T* prFrcPrm);
#ifdef DIVX_PLUS_CER
#ifdef   CC_B2R_CHAPTER_CHG
static VDP_CHAPTER_T* _VDP_FrcGetChapterObj(VOID);
static VDP_CHAPTER_T  _arVdpChapter;
static BOOL fgFirstSetChapter = FALSE;
#endif
#endif

#ifdef CC_B2R_ENABLE_CHG_FRAME_MSG
static INT32 _VDP_ReceiveChgFrameMsg(UCHAR ucB2rId,
                                     VOID            *pv_msg,
                                     MSGQ_OPTION_T   e_option);
#endif


static void _VdpRelResource(UCHAR ucB2rId);

static void _VDPGetFrameBufferForBivl3D(B2R_PRM_T* prFrcPrm);
static UCHAR _VDPConvertFrameRateInx2FrameRate(B2R_PRM_T* prFrcPrm, UCHAR ucIndx);
static BOOL _VdpIsBivl3D(B2R_PRM_T* prFrcPrm);
static void _VdpDetermineRepeatForPushTrickMode(B2R_PRM_T* prFrcPrm, BOOL fgRepeatFbForPending, BOOL* pfgDropFb, UCHAR ucLastFbForNoFb);
static void _VdpCalcRepeatCntForPushTrickMode(B2R_PRM_T* prFrcPrm, BOOL fgLastFbIsDrop);
static void _VdpPreparePendingForNextVsync(B2R_PRM_T* prFrcPrm);
static void _VdpGetFrameBufferFromPending(B2R_PRM_T* prFrcPrm);
static void _VdpCpyPicHdr2B2r(B2R_PRM_T* prFrcPrm, FBM_PIC_HDR_T* prPicHdr);
#ifdef ENABLE_MULTIMEDIA
static UCHAR _VDP_RTFrameRateConvert(B2R_PRM_T* prFrcPrm, UCHAR ucIndex);
static INT32 _VDP_RTAVsyncProc(B2R_PRM_T* prFrcPrm);
#ifdef TIME_MEASUREMENT
VOID _B2R_RT_TMS_Statistics(B2R_PRM_T* prFrcPrm);
#endif
#endif
static INT32 _B2R_OneFbChgHandle(B2R_PRM_T* prFrcPrm);
//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static BOOL _fgEnableVideoLog = FALSE;

#ifdef CC_B2R_ENABLE_CHG_FRAME_MSG
static HANDLE_T _ahChgFrameQueue[B2R_NS] = {NULL_HANDLE, NULL_HANDLE};
static BOOL _fgEnableGstLog = FALSE;
static HANDLE_T _ahChgFrameMutex[B2R_NS] = {NULL_HANDLE, NULL_HANDLE};
#endif
static VDP_B2R_CHG_FRAME_MSG_T _arPendingChgFrmMsg[B2R_NS] = {{0}};
static BOOL _afgPendingChgFrmMsg[B2R_NS] = {FALSE, FALSE};

#ifdef CC_B2R_ENABLE_SEAMLESS_THREAD
static BOOL _fgSeamlessThreadInit = FALSE;
static HANDLE_T _hSeamlessThread = 0;
static HANDLE_T _hSeamlessSema = 0;
#endif

static HANDLE_T _hDispBufLockMutex[B2R_NS] = {NULL_HANDLE, NULL_HANDLE};    // Mutex to protect multi-thread access buf lock array

#if defined(CC_NETFLIX_SUPPORT) || defined(CC_RESIZE_SMALL_IMAGE)

static UINT32 _pu1YResizeFrame,_pu1CResizeFrame,_pu1YResizeFrame2,_pu1CResizeFrame2;
//static HAL_TIME_T _rTimeRstS, _rTimeRstE, _rTimeRstDt;
static BOOL _fgFirstFrame = TRUE;
#define IMG_RESIZE_ALIGN_SIZE 2047
#define IMG_RESIZE_ALIGN(X)  (((X) + IMG_RESIZE_ALIGN_SIZE) & (~IMG_RESIZE_ALIGN_SIZE))
#endif

static FBM_ASPECT_RATIO_T _rFbmAspectRatio;

static B2R_PRM_T    _arVdpPrm[B2R_NS];

static UINT32        _u4ReturnValue;

static STC_SPEED_TYPE_T _eSpeed[B2R_NS] = {STC_SPEED_TYPE_FORWARD_1X,STC_SPEED_TYPE_FORWARD_1X};
static BOOL _fgPauseMM[B2R_NS] = {FALSE,FALSE};

static INT32 _i4TimeShiftSpeed[B2R_NS] = {100,100};
static BOOL _fgTimeShift[B2R_NS] = {FALSE,FALSE};

static DRV_DBASE_T* _prDbase = NULL;

#ifdef VDP_HANDLE_32FLAG
static UINT32 _au4Pd32FlagCadence[2][PD_32_FLAG_SEQ] =
{
    {1, 0, 1, 0},
    {1, 0, 0, 1}
};
#endif

static VDP_CONF_T* _prVdpConf[VDP_NS];

static VDP_FREEZE_PRM_T _arVdpDtvFreeze[B2R_NS] =
{
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
};

#ifdef CC_B2R_GFX_COPY_FIELD
static VDP_GFX_COPY_FIELD_PRM_T _rVdpGfxCopyField =
{
    FBM_FBG_ID_UNKNOWN,
    FBM_FB_ID_UNKNOWN,
    0,
    0,
    0
};
#endif

/* VDP trigger mode + audio master to do trigger mode */
static VDP_TRIG_AUD_PRM_T _arVdpTrigAud[B2R_NS] = {{0}};

/* Aud out first, no need VDP to update STC */
static BOOL _arPendingForSTC[B2R_NS] = {0};

/* VDP Seek done notification */
VDP_SEEK_PRM_T arVdpSeek[B2R_NS] = {{0}};

static VDP_FRC_COPY_PRM_T _rFrcCopy;

static VDP_LIPSYNC_OK_NG_PRM_T _rVdpLipSync =
{
    VDP_LIP_SYNC_NG,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

#ifdef __MODEL_slt__
UINT32 b2rState = 0xff;
#endif

static BOOL _fgInvalidFrmRate[B2R_NS] = {FALSE,FALSE};
static BOOL _fgHandleSeqChg[B2R_NS]   = {FALSE,FALSE};
static BOOL _fgEnableWaitSTC[B2R_NS]  = {FALSE,FALSE};

static UINT16 _u2VsyncNotStableCount[B2R_NS] = {0,0};
static UCHAR _ucFrameRateUnkownCount[B2R_NS] = {0,0};

#ifdef VDP_B2R_PSCAN_CRC_FLOW
//extern UINT32 u4DramIndex;
extern UINT32 u4DRAMChkSum;
extern UINT32 u4PixelChkSum;
extern UINT32 u4PSCANCrc;
extern UINT32 u4PSCANSrcCrc;
#endif

#ifdef VDP_B2R_PSCAN_CRC_FLOW
VDP_B2R_CRC_PRM_T _rVdpB2rCrc;
static UINT32 _u4SetPts = 0xFFFFFFFF;
static BOOL _fgTriggerEnc = FALSE;
#endif

//wait NPTV stable
//#define CC_B2R_WAIT_NPTV_STABLE
#ifdef CC_B2R_WAIT_NPTV_STABLE
static BOOL _afgNPTVStable[B2R_NS] = {FALSE};

static UINT32 _au4WaitNPTVStableCount[B2R_NS] = {0};

static UINT8 _au1PlayMode[B2R_NS] = {FBM_FBG_MODE_NS};
#endif
//for timeshift
static UINT32 _u4TimeShfitChangeFrmCnt = 0;
static BOOL _fgTimeShfitThumbStartPlay = FALSE;

//for netflix
static UINT32 _u4PreviousResizeWidth = 0;
static UINT32 _u4PreviousResizeHeight = 0;
static UINT32 _u4PreviousResizeWidth2 = 0;
static UINT32 _u4PreviousResizeHeight2 = 0;

//for counting drift
static UINT32 _u4DriftCount[B2R_NS] = {0};
static UINT32 _u4MaxDriftCount[B2R_NS] = {0};
static UINT32 _u4ValidCount[B2R_NS] = {0};

//boot time profile
static BOOL _fgB2RProfile = FALSE;
static UCHAR _ucRemainTimeToSendSeekDone = 0;


static BOOL _fgRepeatFrame = FALSE;
static BOOL _fgDropFrame = FALSE;

#ifdef HANDLE_PTS_SKIP
static B2R_AVSYNC_CTRL_T _AVSyncCtrl[B2R_NS] = {{0}};
#endif

static BOOL _fgModeChangeSent = FALSE;
static UINT32 _u4LastWidth = 0;
static UINT32 _u4LastHeight = 0;

static UINT32 _u4RepeatCntForPushTrickMode = 0;
static UINT32 _u4ElapseCntForPushTrickMode = 0;
static UCHAR _ucRepeatFbForPushTrickMode = FBM_FB_ID_UNKNOWN;
static UINT32 _au4OriSrcWidth[VDP_NS] = {0};
static UINT32 _au4OriSrcHeight[VDP_NS] = {0};

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
#ifdef ENABLE_MULTIMEDIA
static VDEC_ES_INFO_T* _B2R_GetEsInfo(UCHAR ucFbgId)
{
    VDEC_ES_INFO_T* prVdecEsInfo;
    UINT8 u1DecoderSrcId = FBM_GetDecoderSrcId(ucFbgId);
    if (u1DecoderSrcId < VDEC_MAX_ES)
    {
        prVdecEsInfo = _VDEC_GetEsInfo(u1DecoderSrcId);
    }
    else
    {
        LOG(0,"u1DecoderSrcId > VDEC_MAX_ES , oops \n");
        prVdecEsInfo = _VDEC_GetEsInfo(ES0);
    }
    return prVdecEsInfo;
}
#endif

#ifdef CC_B2R_GFX_COPY_FIELD
//-----------------------------------------------------------------------------
/** Brief of _VdpGfxRoutine.
 */
//-----------------------------------------------------------------------------

static VOID _VDP_MemCpy(UINT8 *pu1TopStartAddr,
                        UINT32 u4TotalLines, UINT32 u4LineWidth)
{

    UINT8 *pu1BotStartAddr;
    UINT8 *pu1TopCurAddr;
    UINT8 *pu1BotCurAddr;
    UINT32 i;

    // bottom field start address
    pu1BotStartAddr = (UINT8 *)((UINT32)pu1TopStartAddr + u4LineWidth);

    // set top and bottom current address
    pu1TopCurAddr = pu1TopStartAddr;
    pu1BotCurAddr = pu1BotStartAddr;

    for (i = 0; i < u4TotalLines; i++)
    {
        //GFX_SetSrc(pu1TopCurAddr, (UINT32)CM_RGB_CLUT8, u4Pitch);

        //GFX_SetDst(pu1BotCurAddr, (UINT32)CM_RGB_CLUT8, u4Pitch);

        //GFX_BitBlt(0, 0, 0, 0, u4LineWidth, 1);
        x_memcpy(pu1BotCurAddr,pu1TopCurAddr,u4LineWidth);
        pu1TopCurAddr = (UINT8 *)((UINT32)pu1TopCurAddr + (2 * u4LineWidth));
        pu1BotCurAddr = (UINT8 *)((UINT32)pu1BotCurAddr + (2 * u4LineWidth));

    } // ~for
}


static void _VdpGfxRoutine(void* pvArg)
{
    UNUSED(pvArg);

    while (1)
    {
        FBM_SEQ_HDR_T* prSeqHdr;
        UINT32 u4AddrY;
        UINT32 u4AddrC;

        // wait semaphore
        VERIFY (x_sema_lock(_rVdpGfxCopyField.hSemaphore, X_SEMA_OPTION_WAIT) == OSR_OK);

        // mutex
        VERIFY (x_sema_lock(_rVdpGfxCopyField.hMutex, X_SEMA_OPTION_WAIT) == OSR_OK);

        prSeqHdr = FBM_GetFrameBufferSeqHdr(_rVdpGfxCopyField.ucFbgId);

        FBM_GetFrameBufferAddr(_rVdpGfxCopyField.ucFbgId, _rVdpGfxCopyField.ucFbId, &u4AddrY, &u4AddrC);

        if ((u4AddrY != 0) && (u4AddrC != 0))
        {
            UINT32 u4FbWidth;
            UINT32 u4TotalLine;
            UINT32 u4LineWidth;

            LOG(3, "Copy (%d, %d)\n", prSeqHdr->u2LineSize, prSeqHdr->u2VSize);

            // Y Block = 64 * 32
            u4FbWidth = prSeqHdr->u2LineSize;
            u4LineWidth = 64;
            u4FbWidth = (u4FbWidth + (u4LineWidth - 1)) & (~(u4LineWidth - 1));
            u4TotalLine = prSeqHdr->u2VSize * (u4FbWidth / u4LineWidth);

            u4TotalLine >>= 1;  // 1 Line of vdp = 1 Line of gfx

            LOG(3, "Copy Y(%d %d)\n", u4TotalLine, u4LineWidth);

            //GFX_CopyTopfieldToBotfield((UINT8 *)u4AddrY, u4TotalLine, u4LineWidth);
            _VDP_MemCpy((UINT8 *)u4AddrY, u4TotalLine, u4LineWidth);

            // C Block = (32 * 2) * 16
//            u4FbWidth = prSeqHdr->u2LineSize;
//            u4LineWidth = 64;
//            u4FbWidth = (u4FbWidth + (u4LineWidth - 1)) & (~(u4LineWidth - 1));
//            u4TotalLine = (prSeqHdr->u2VSize / 2) * (u4FbWidth / u4LineWidth);
            u4TotalLine >>= 1;  // C = Y/2

            LOG(3, "Copy C(%d %d)\n", u4TotalLine, u4LineWidth);

            //GFX_CopyTopfieldToBotfield((UINT8 *)u4AddrC, u4TotalLine, u4LineWidth);
            _VDP_MemCpy((UINT8 *)u4AddrC, u4TotalLine, u4LineWidth);

            LOG(3, "Copy Finish\n");
        }
        else
        {
            LOG(1, "[%s-%d]Frame Buffer Addr error!.\n", __func__, __LINE__);
            ASSERT(0);
        }

        // mutex
        VERIFY (x_sema_unlock(_rVdpGfxCopyField.hMutex) == OSR_OK);
    }
}

#endif

//-----------------------------------------------------------------------------
/** Brief of _VdpGfxCopyFieldInit.
 */
//-----------------------------------------------------------------------------
static void _VdpGfxCopyFieldInit(void)
{
#ifdef CC_B2R_GFX_COPY_FIELD
    if (_rVdpGfxCopyField.u4Init == 0)
    {
        HANDLE_T hThread1;

        // create semaphore
        VERIFY (x_sema_create(&_rVdpGfxCopyField.hSemaphore, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);

        // create semaphore
        VERIFY (x_sema_create(&_rVdpGfxCopyField.hMutex, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);

        // create thread, priority = higher than mpv but lower than vdp
        VERIFY (x_thread_create(&hThread1, "VDP-G", VSYNC_STACK_SIZE, (MPV_THREAD_PRIORITY - 1),
                                _VdpGfxRoutine, 0, NULL) == OSR_OK);

        _rVdpGfxCopyField.u4Init = 1;
    }
#endif
}

//-----------------------------------------------------------------------------
/** Brief of _VdpGfxCopyFieldStart.
 */
//-----------------------------------------------------------------------------
static UINT32 _VdpGfxCopyFieldStart(UCHAR ucFbgId, UCHAR ucFbId)
{
#ifdef CC_B2R_GFX_COPY_FIELD
    UCHAR ucFbNs;
    UCHAR ucFRefFbId;
    UCHAR ucBRefFbId;

    if (ucFbgId == FBM_FBG_ID_UNKNOWN)
    {
        LOG(3, "Start Fail 1\n");
        return 0;
    }

    ucFbNs = FBM_GetFrameBufferNs(ucFbgId);

    if (ucFbId >= ucFbNs)
    {
        LOG(3, "Start Fail 2\n");
        return 0;
    }

    if (FBM_CheckFrameBufferStatus(ucFbgId, ucFbId, FBM_FB_STATUS_LOCK) == 0)
    {
        LOG(3, "Start Fail 3\n");
        return 0;
    }

    FBM_GetRefFrameBuffer(ucFbgId, &ucFRefFbId, &ucBRefFbId);

    if ((ucFbId == ucFRefFbId) || (ucFbId == ucBRefFbId))
    {
        LOG(3, "Start Fail 4\n");
        return 0;
    }

    _rVdpGfxCopyField.ucFbgId = ucFbgId;
    _rVdpGfxCopyField.ucFbId = ucFbId;

    // release semaphore
    VERIFY (x_sema_unlock(_rVdpGfxCopyField.hSemaphore) == OSR_OK);

    return 1;
#else
    UNUSED(ucFbgId);
    UNUSED(ucFbId);

    return 1;
#endif
}


#ifdef CC_B2R_ENABLE_SEAMLESS_THREAD
static VOID _SeamlessRoutine(void* pvArg)
{
    UNUSED(pvArg);

    while (1)
    {
        //wait sema
        VERIFY (x_sema_lock(_hSeamlessSema, X_SEMA_OPTION_WAIT) == OSR_OK);
        _VDP_SeamlessJob(&_arVdpPrm[0]);
    }
}
#endif


#ifdef CC_3D_MM_DS_SUPPORT
static UINT8 _VDP_GetAdditionalFbgId(B2R_PRM_T* prFrcPrm)
{
    UCHAR  ucFbgId;
    UINT32 u4idx =0;

    prFrcPrm->ucSubFbgId = FBM_FBG_ID_UNKNOWN;
    for (u4idx=0 ; u4idx < MAX_ES_NS ; u4idx++)
    {
        ucFbgId = FBM_GetFbgByEs(u4idx);
        if (ucFbgId != FBM_FBG_ID_UNKNOWN && ucFbgId != prFrcPrm->ucFbgId)
        {
            prFrcPrm->ucSubFbgId = ucFbgId;
        }
    }
	if (_VdpIsBivl3D(prFrcPrm))
	{
		prFrcPrm->ucSubFbgId = prFrcPrm->ucFbgId;
	}

    if (prFrcPrm->ucSubFbgId == FBM_FBG_ID_UNKNOWN)
    {
        if (prFrcPrm->ucSubFbgIdRecorded!= FBM_FBG_ID_UNKNOWN && prFrcPrm->ucSubFbId != FBM_FB_ID_UNKNOWN)
        {
            _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucSubFbgIdRecorded, prFrcPrm->ucSubFbId, FBM_FB_STATUS_EMPTY);
        }
        if (prFrcPrm->ucSubFbgIdRecorded != FBM_FBG_ID_UNKNOWN && prFrcPrm->ucSubReleaseFbId != FBM_FB_ID_UNKNOWN)
        {
            _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucSubFbgIdRecorded, prFrcPrm->ucSubReleaseFbId, FBM_FB_STATUS_EMPTY);
        }
        if (prFrcPrm->ucSubFbgIdRecorded != FBM_FBG_ID_UNKNOWN && prFrcPrm->ucSubPendingFbId != FBM_FB_ID_UNKNOWN)
        {
            _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucSubFbgIdRecorded, prFrcPrm->ucSubPendingFbId, FBM_FB_STATUS_EMPTY);
        }
        prFrcPrm->ucSubReleaseFbId = FBM_FB_ID_UNKNOWN;
        prFrcPrm->ucSubPendingFbId = FBM_FB_ID_UNKNOWN;
        prFrcPrm->ucSubFbId = FBM_FB_ID_UNKNOWN;
    }
    return prFrcPrm->ucSubFbgId;
}

#endif
#ifdef CC_SUPPORT_NPTV_SEAMLESS 

VOID _VDP_NPTVSeamlessStatus(B2R_PRM_T* prFrcPrm, UCHAR ucMode)
{
    UCHAR ucCurrnetMode = 0;

    if ((!prFrcPrm) || (!FBM_ChkSeamlessMode(prFrcPrm->ucFbgId, SEAMLESS_BY_NPTV)))
    {
        return;
    }
    
    if (ucMode == VDP_SMLS_MODE_NONE)
    {
        LOG(3, "VDP Seamless Mode Clear: %d -> %d.\n", prFrcPrm->ucSeamlessVDPMode, 0);
        prFrcPrm->ucSeamlessVDPMode = 0;
        return;
    }
    else
    {
        ucCurrnetMode = prFrcPrm->ucSeamlessVDPMode;
    }

    switch (ucCurrnetMode)
    {
    case VDP_SMLS_MODE_NONE:
        ASSERT((ucMode == VDP_SMLS_PREPARE) || (ucMode == VDP_SMLS_PREPARE_WHILE_START_PLAY));
        #ifdef CC_SUPPORT_NPTV_SEAMLESS 
        vVRMSetEventFlg(SV_VP_MAIN, VRM_EVENT_BY_B2R);
        #endif
        ucCurrnetMode = ucMode;
        break;
    case VDP_SMLS_PREPARE:
        ASSERT(ucMode == VDP_SMLS_READY);
        ucCurrnetMode = VDP_SMLS_READY;
        break;
    case VDP_SMLS_READY:
        ASSERT(ucMode == VDP_SMLS_FRAME_COMING);
        ucCurrnetMode = VDP_SMLS_FRAME_COMING;
        break;
    case VDP_SMLS_FRAME_COMING:
        break;
    case VDP_SMLS_PREPARE_WHILE_START_PLAY:
        ASSERT(ucMode == VDP_SMLS_READY_WHILE_START_PLAY);
        ucCurrnetMode = VDP_SMLS_READY_WHILE_START_PLAY;
        break;
    case VDP_SMLS_READY_WHILE_START_PLAY:
        ASSERT(ucMode == VDP_SMLS_FRAME_COMING);
        ucCurrnetMode = VDP_SMLS_FRAME_COMING;
        break;
    } 

    LOG(3, "VDP Seamless Mode Chg: %d -> %d.\n", prFrcPrm->ucSeamlessVDPMode, ucCurrnetMode);
    prFrcPrm->ucSeamlessVDPMode = ucCurrnetMode;
}

#endif
static VOID _VDP_SeamlessJob(B2R_PRM_T* prFrcPrm)
{
    UINT32 u4AddrY      =   0x0;
    UINT32 u4AddrC      =   0x0;
#ifdef CC_3D_MM_DS_SUPPORT
    UINT32 u4AddrYSub   =   0x0;
    UINT32 u4AddrCSub   =   0x0;
#endif
    UINT32 u4YSize      =   0x0;
    UINT32 u4RYSize     =   0x0;
    UINT32 u4FbmWith    =   0x0;
    UINT32 u4FbmHeight  =   0x0;
    UINT8 u1DecoderSrcId = FBM_GetDecoderSrcId(prFrcPrm->ucFbgId);
    UCHAR ucB2rId       = prFrcPrm->ucB2rId;
    UCHAR ucVdpId = VDP_GetVdpId(ucB2rId);
#ifdef ENABLE_MULTIMEDIA
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;
#endif
    if (ucB2rId >= B2R_NS)
    {
        LOG(3,"ucB2rId >= B2R_NS \n");
        return;
    }

    FBM_GetFrameBufferAddr(prFrcPrm->ucFbgId,prFrcPrm->ucFbId, &u4AddrY, &u4AddrC);
#ifdef CC_3D_MM_DS_SUPPORT
    if (prFrcPrm->ucSubFbgId != FBM_FBG_ID_UNKNOWN && prFrcPrm->ucSubFbId != FBM_FB_ID_UNKNOWN)
    {
        FBM_GetFrameBufferAddr(prFrcPrm->ucSubFbgId, prFrcPrm->ucSubFbId, &u4AddrYSub, &u4AddrCSub);
    }
#endif

#if defined(CC_NETFLIX_SUPPORT) || defined(CC_RESIZE_SMALL_IMAGE)
    {
        FBM_PIC_HDR_T* prFbmPicHdr = NULL;
        FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;
        UINT32 u4YResizeAddr,u4CResizeAddr;
        UINT32 u4PicX, u4PicY, u4PicWidth, u4PicHeight;
        RZ_VDO_SCL_PARAM_SET_T rScaleParamV;
        FBM_POOL_T* prFbmPool;

        x_memset(&rScaleParamV,0,sizeof(RZ_VDO_SCL_PARAM_SET_T));

        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG);
        prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prFrcPrm->ucFbgId);
        prFbmPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, prFrcPrm->ucFbId);
        //LOG(1,"Pts(%x) FbId(%d)\n",prFbmPicHdr->u4PTS,prFrcPrm->ucFbId);

        if ((prFbmPicHdr != NULL) && (prFbmSeqHdr != NULL))
        {
            UINT8 u1PicType = VDEC_PIC_TYPE_I;
            HAL_TIME_T rEndTime;
            HAL_TIME_T rDeltaTime;
            HAL_TIME_T rDeltaTimeDecoder;

            HAL_GetTime(&rEndTime);

            HAL_GetDeltaTime(&rDeltaTime, &prFbmPicHdr->rTimeHwS, &rEndTime);
            HAL_GetDeltaTime(&rDeltaTimeDecoder, &prFbmPicHdr->rTimeFromDecoder, &rEndTime);
#if 0
            LOG(3,"[Source] Seconds %d Micro %d\n",prFbmPicHdr->rTimeHwS.u4Seconds,prFbmPicHdr->rTimeHwS.u4Micros);
            LOG(3,"[Target] Seconds %d Micro %d\n",rEndTime.u4Seconds,rEndTime.u4Micros);
            LOG(1,"Delta %d seconds %d micro seconds\n",rDeltaTime.u4Seconds, rDeltaTime.u4Micros);
            LOG(2,"From Decoder %d seconds %d micro seconds\n",rDeltaTimeDecoder.u4Seconds, rDeltaTimeDecoder.u4Micros);
#endif

            if (prFbmPicHdr->fgSeqHeader)
            {
                u1PicType = VDEC_PIC_TYPE_SEQ;
                _VDP_VsyncNotify(VDP_MSG_GET_PIC_TYPE_CB, ucVdpId, u1DecoderSrcId, u1PicType);
            }

            if (prFbmPicHdr->ucPicCdTp ==MPEG_PIC_TYPE_I)
            {
                u1PicType = VDEC_PIC_TYPE_I;
            }
            else if (prFbmPicHdr->ucPicCdTp ==MPEG_PIC_TYPE_P)
            {
                u1PicType = VDEC_PIC_TYPE_P;
            }
            else if (prFbmPicHdr->ucPicCdTp ==MPEG_PIC_TYPE_B)
            {
                u1PicType = VDEC_PIC_TYPE_B;
            }
            else
            {
                u1PicType = VDEC_PIC_TYPE_I;
            }

            _VDP_VsyncNotify(VDP_MSG_GET_PIC_TYPE_CB, ucVdpId,u1DecoderSrcId,u1PicType);

            if (_u4TimeShfitChangeFrmCnt >= 5)
            {
                _fgTimeShfitThumbStartPlay = TRUE;
            }
            _u4TimeShfitChangeFrmCnt++;


#if 0 // Default use new seamless mode
#if defined(CC_NETFLIX_SUPPORT)
            if (((prFbmSeqHdr->fgResizeSmallPic)
                    || (prFbmPicHdr->fgSeamlessDisp && (prFbmPool != NULL) && (prFbmPool->u4Size > (6*1024*1024))))
                    /*#ifdef CC_3D_MM_DS_SUPPORT
                    && (prFrcPrm->ucSubFbgId == FBM_FBG_ID_UNKNOWN)
                    #endif*/
               )
#else //ifdef CC_RESIZE_SMALL_IMAGE
            if (((prFbmSeqHdr->fgResizeSmallPic) || (prFbmPicHdr->fgSeamlessDisp))
                    /*#ifdef CC_3D_MM_DS_SUPPORT
                    && (prFrcPrm->ucSubFbgId == FBM_FBG_ID_UNKNOWN)
#endif*/
                )
#endif
#else
    if (prFbmSeqHdr->fgResizeSmallPic)  // Only small pic use old resize flow
#endif
            {
                UINT32 *pu4PrevResizeWidth = NULL;
                UINT32 *pu4PrevResizeHeight = NULL;
                _pu1YResizeFrame = 0;
                _pu1CResizeFrame = 0;
                _pu1YResizeFrame2 = 0;
                _pu1CResizeFrame2 = 0;

#ifdef CC_RESIZE_SMALL_IMAGE
                if (prFbmSeqHdr->fgResizeSmallPic)
                {
                    UINT32 u4AddrYTmp;
                    UINT32 u4AddrCTmp;
                    UINT32 u4HeightAlign = (((prFbmPicHdr->u4PicHeight) + 31) & (~31));
                    // Use rest memory of original frame buffer
                    FBM_GetFrameBufferAddr(prFrcPrm->ucFbgId, (UCHAR)0, &u4AddrYTmp, &u4AddrCTmp);
                    u4YSize = prFbmPicHdr->u4PicWidthPitch * u4HeightAlign;

                    if ((u4AddrYTmp > 0) && (u4AddrCTmp > 0))
                    {
                        _pu1YResizeFrame = (u4AddrYTmp + (UINT32)u4YSize);
                        _pu1CResizeFrame = (u4AddrCTmp + (UINT32)(u4YSize/2));
                    }

                    if ((u4AddrYTmp > 0) && (u4AddrCTmp > 0))
                    {
                        FBM_GetFrameBufferAddr(prFrcPrm->ucFbgId, (UCHAR)1, &u4AddrYTmp, &u4AddrCTmp);
                        _pu1YResizeFrame2 = (u4AddrYTmp + (UINT32)u4YSize);
                        _pu1CResizeFrame2 = (u4AddrCTmp + (UINT32)(u4YSize/2));
                    }
                }
#endif

#ifdef CC_NETFLIX_SUPPORT
                if (prFbmPicHdr->fgSeamlessDisp)
                {
#ifdef ENABLE_MULTIMEDIA
                    UCHAR  ucFbNS   = 0x0;

                    ucFbNS = FBM_GetFrameBufferNs(prFrcPrm->ucFbgId);
                    FBM_GetFrameBufferSize(prFrcPrm->ucFbgId,&u4FbmWith,&u4FbmHeight);
                    u4YSize = (u4FbmWith*u4FbmHeight);
                    //VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;
                    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ES0);

                    if (prVdecEsInfoKeep->eCurFMT == VDEC_FMT_RV)
                    {
                        _pu1YResizeFrame    = (prFbmPool->u4Addr +
                                               (((ucFbNS*u4YSize*3)>>1)+
                                                FBM_FBG_TYPE_PAL_Y_SIZE + FBM_FBG_TYPE_PAL_C_SIZE));
                        _pu1CResizeFrame    = _pu1YResizeFrame  + FBM_FBG_TYPE_PAL_Y_SIZE;
                        _pu1YResizeFrame2   = _pu1CResizeFrame  + FBM_FBG_TYPE_PAL_C_SIZE;
                        _pu1CResizeFrame2   = _pu1YResizeFrame2 + FBM_FBG_TYPE_PAL_Y_SIZE;
                    }
                    else
                    {
                        /*_pu1YResizeFrame    = (prFbmPool->u4Addr +
                                               (((ucFbNS*u4YSize*3)>>1)+
                                                FBM_RESIZE_RPR_BUF_JUMP_SIZE));*/
                        _pu1YResizeFrame = prFbmPool->u4Addr + FBM_GetRunningVdecMemSize(prFrcPrm->ucFbgId);
                        _pu1CResizeFrame    = _pu1YResizeFrame  + u4YSize;
                        _pu1YResizeFrame2   = _pu1CResizeFrame  + (u4YSize/2);
                        _pu1CResizeFrame2   = _pu1YResizeFrame2 + u4YSize;
                    }

#endif
                }
#endif

                if (_fgFirstFrame)
                {
                    u4YResizeAddr = (UINT32)IMG_RESIZE_ALIGN(_pu1YResizeFrame);
                    u4CResizeAddr = (UINT32)IMG_RESIZE_ALIGN(_pu1CResizeFrame);
                    pu4PrevResizeWidth = &_u4PreviousResizeWidth;
                    pu4PrevResizeHeight = &_u4PreviousResizeHeight;
                }
                else
                {
                    u4YResizeAddr = (UINT32)IMG_RESIZE_ALIGN(_pu1YResizeFrame2);
                    u4CResizeAddr = (UINT32)IMG_RESIZE_ALIGN(_pu1CResizeFrame2);
                    pu4PrevResizeWidth = &_u4PreviousResizeWidth2;
                    pu4PrevResizeHeight = &_u4PreviousResizeHeight2;
                }

                if (prFbmPicHdr->fgCropping)
                {
                    u4PicX = prFbmPicHdr->u4CropX;
                    u4PicY = prFbmPicHdr->u4CropY;
                    u4PicWidth = prFbmPicHdr->u4CropWidth;
                    u4PicHeight = prFbmPicHdr->u4CropHeight;
                }
                else
                {
                    u4PicX = 0;
                    u4PicY = 0;
                    u4PicWidth = prFbmPicHdr->u4PicWidth;
                    u4PicHeight = prFbmPicHdr->u4PicHeight;
                }

                if ((u4PicWidth > 0) && (u4PicHeight > 0) && (u4YResizeAddr) && (u4CResizeAddr))
                {
                    // FBM ready
                    UINT32 u4Xaddr=0;
                    UINT32 u4Yaddr=0;
                    //VDP_REGION_T rSrcRegion;
                    //UCHAR ucSrcFullRegion = 0;

                    if (_fgFirstFrame)
                    {
                        _fgFirstFrame = FALSE;
                    }
                    else
                    {
                        _fgFirstFrame = TRUE;
                    }
                    if (prFbmPicHdr->fgSeamlessDisp)
                    {
                        //clear target buffer
                        //if(prFbmSeqHdr->fgCleanSeamlessBuffer)

                        //if (*pu4PrevResizeWidth != prFbmSeqHdr->u4ResizeWidth || *pu4PrevResizeHeight != prFbmSeqHdr->u4ResizeHeight)
                        {
                            GFX_Memset((UINT8*)IMG_RESIZE_ALIGN(u4YResizeAddr),u4FbmWith,u4FbmHeight,0);
                            GFX_Memset((UINT8*)IMG_RESIZE_ALIGN(u4CResizeAddr),u4FbmWith,u4FbmHeight/2,0x80);
                        }
                        *pu4PrevResizeWidth = prFbmSeqHdr->u4ResizeWidth;
                        *pu4PrevResizeHeight = prFbmSeqHdr->u4ResizeHeight;

                        rScaleParamV.u4VdoCbCrSwap =0;

                        //calculate aspect ratio
                        if (prFbmSeqHdr->u4ResizeWidth < prFbmSeqHdr->u2HSize)
                        {
                            u4Xaddr = (prFbmSeqHdr->u2HSize - prFbmSeqHdr->u4ResizeWidth)/2;
                            if (u4Xaddr %2 != 0)
                            {
                                rScaleParamV.u4VdoCbCrSwap =1;
                            }
                        }
                        if (prFbmSeqHdr->u4ResizeHeight < prFbmSeqHdr->u2VSize)
                        {
                            u4Yaddr = (prFbmSeqHdr->u2VSize - prFbmSeqHdr->u4ResizeHeight)/2;
                            /*if(u4Yaddr %2 !=0)
                            {
                                u4Yaddr -= 1;
                            }*/
                        }
                    }
                    else
                    {
                        u4Yaddr = 0;
                        u4Xaddr = 0;
                    }

#if defined(CC_MT5395) || defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881) || defined(CC_MT5399)// Force raster out for VENC
                    if (prFbmSeqHdr->fgRasterOrder)
                    {
                        rScaleParamV.u4IsRsIn = 1;
                    }
                    else
                    {
                        rScaleParamV.u4IsRsIn = 0;
                    }
#else
                    rScaleParamV.u4IsRsIn = 0;
#endif
                    rScaleParamV.u4InMode = E_RZ_VDO_OUTMD_420;
                    rScaleParamV.u4YSrcBase = u4AddrY;
                    rScaleParamV.u4YSrcBufLen = prFbmPicHdr->u4PicWidthPitch;
                    rScaleParamV.u4YSrcHOffset = u4PicX;
                    rScaleParamV.u4YSrcVOffset = u4PicY;
                    rScaleParamV.u4YSrcW = u4PicWidth;
                    rScaleParamV.u4YSrcH = u4PicHeight;
                    rScaleParamV.u4CSrcBase = u4AddrC;
                    rScaleParamV.u4CSrcHOffset = u4PicX >> 1;
                    rScaleParamV.u4CSrcVOffset = u4PicY >> 1;
                    rScaleParamV.u4CSrcW = u4PicWidth >> 1;
                    rScaleParamV.u4CSrcH = u4PicHeight >> 1;
#if defined(CC_MT5395) || defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389)|| defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881) || defined(CC_MT5399)// Force raster out for VENC
                    if (prFbmSeqHdr->fgRasterOrder)
                    {
                        rScaleParamV.u4IsRsOut = 1;
                    }
                    else
                    {
                        rScaleParamV.u4IsRsOut = 0;
                    }
#else
                    rScaleParamV.u4IsRsOut = 0;
#endif
                    rScaleParamV.u4OutMode = E_RZ_VDO_OUTMD_420;
                    rScaleParamV.u4IsVdo2Osd = 0;
                    rScaleParamV.u4YTgBase = u4YResizeAddr;
                    rScaleParamV.u4YTgCM = E_RZ_VDO_OUTMD_420;
                    rScaleParamV.u4YTgHOffset = u4Xaddr;
                    rScaleParamV.u4YTgVOffset = u4Yaddr;

                    if (prFbmSeqHdr->fgResizeSmallPic)
                    {
                        if (prFbmPicHdr->u4PicWidth < FBM_IMG_RESIZE_LIMITATION)
                        {
                            rScaleParamV.u4YTgW = FBM_IMG_RESIZE_LIMITATION;
                        }
                        else if (prFbmPicHdr->u4PicWidth > FBM_IMG_RESIZE_LIMITATION_MAX)
                        {
                            rScaleParamV.u4YTgW = FBM_IMG_RESIZE_LIMITATION_MAX;
                        }
                        else
                        {
                            rScaleParamV.u4YTgW = prFbmSeqHdr->u2HSize&(0xfff0);
                        }

                        if (prFbmPicHdr->u4PicHeight < FBM_IMG_RESIZE_LIMITATION)
                        {
                            rScaleParamV.u4YTgH = FBM_IMG_RESIZE_LIMITATION;
                        }
                        else if (prFbmPicHdr->u4PicHeight > FBM_IMG_RESIZE_LIMITATION_MAX)
                        {
                            rScaleParamV.u4YTgH = FBM_IMG_RESIZE_LIMITATION_MAX;
                        }
                        else
                        {
                            rScaleParamV.u4YTgH = prFbmSeqHdr->u2VSize;
                        }
                        rScaleParamV.u4YTgBufLen= prFbmSeqHdr->u2LineSize;
                    }
                    else
                    {
                        //rScaleParamV.u4YTgW = prFbmSeqHdr->u4ResizeWidth;
                        //rScaleParamV.u4YTgH = prFbmSeqHdr->u4ResizeHeight;

                        rScaleParamV.u4YTgW = prFbmSeqHdr->u4ResizeWidth;
                        rScaleParamV.u4YTgH = prFbmSeqHdr->u4ResizeHeight;

                        rScaleParamV.u4YTgBufLen= prFbmSeqHdr->u2LineSize;
                    }

                    rScaleParamV.u4CTgBase = (UINT32)u4CResizeAddr;
                    if (rScaleParamV.u4YTgW&0x1)
                    {
                        rScaleParamV.u4YTgW--;
                    }

                    if (rScaleParamV.u4YTgH&0x1)
                    {
                        rScaleParamV.u4YTgH--;
                    }

                    rScaleParamV.u4CTgW=rScaleParamV.u4YTgW>>1;
                    rScaleParamV.u4CTgH=rScaleParamV.u4YTgH>>1;
#if defined(CC_MT5395) || defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389)|| defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881) || defined(CC_MT5399)// Force raster out for VENC
                    if (prFbmSeqHdr->fgRasterOrder)
                    {
                        rScaleParamV.u4OutSwap = 0;
                        rScaleParamV.u4SrcSwap = 0;
                    }
                    else
                    {
                        rScaleParamV.u4OutSwap = 6;
                        rScaleParamV.u4SrcSwap = 6;
                    }
#else
                    rScaleParamV.u4OutSwap = 6;
                    rScaleParamV.u4SrcSwap = 6;
#endif
                    //  check resize width&height is identical with source region.

                    /*VDP_GetSrcRegion(prFrcPrm->ucVdpId,&ucSrcFullRegion,&rSrcRegion);

                    if( ( rSrcRegion.u4Width != rScaleParamV.u4YTgW ) ||
                        ( rSrcRegion.u4Height != rScaleParamV.u4YTgH ) )
                    {
                        LOG(0,"resize warrning r(%d,%d), s(%d,%d) \n",
                            rScaleParamV.u4YTgW,
                            rScaleParamV.u4YTgH,
                            rSrcRegion.u4Width,
                            rSrcRegion.u4Height);

                        UNUSED(ucSrcFullRegion);
                        UNUSED(rSrcRegion);
                    }
                    */

                    //  check RPR resize address whether out of fbm pool or not.
                    u4RYSize = (prFbmSeqHdr->u4ResizeWidth*prFbmSeqHdr->u4ResizeHeight);
                    if ( ( _pu1CResizeFrame2 + (u4RYSize/2) ) >= (prFbmPool->u4Addr + prFbmPool->u4Size ) )
                    {
                        LOG(0,"RPR resize address out of fbm pool!!! \n");
                    }

                    IMGRZ_Lock_Ex(E_FIRST_RESIZER);
                    IMGRZ_ReInit_Ex(E_FIRST_RESIZER);
                    IMGRZ_SetScaleOpt_Ex(E_FIRST_RESIZER,E_RZ_INOUT_TYPE_VDOMD);
                    IMGRZ_Scale_Ex(E_FIRST_RESIZER,(void *)(&rScaleParamV));
                    IMGRZ_Flush_Ex(E_FIRST_RESIZER);
                    IMGRZ_Wait_Ex(E_FIRST_RESIZER);
                    IMGRZ_Unlock_Ex(E_FIRST_RESIZER);
                }

                _VDP_HalSetAddr(ucB2rId, u4YResizeAddr, u4CResizeAddr);
#ifdef CC_3D_MM_DS_SUPPORT
                if (((_eSpeed[ucB2rId] == STC_SPEED_TYPE_FORWARD_1X) || (_eSpeed[ucB2rId] != STC_SPEED_TYPE_FORWARD_1X && prFrcPrm->u1B2R3DTrickModeType == B2R_3D_TRICK_MODE_3D_VIEW))
                        && (prFrcPrm->fgB2R3DEnable)
                        && (prFrcPrm->u1B2R3DType == B2R_3D_FRAME_SEQUENTIAL))
                {
                    _VDP_HalSetRViewAddr(ucB2rId, u4AddrYSub, u4AddrCSub);
                }
                else
                {
                    _VDP_HalSetRViewAddr(ucB2rId, u4YResizeAddr, u4CResizeAddr);
                }
#endif
            }
            else
            {
                //for VP6 sw decoder
                FBM_SEQ_HDR_T* prSeqHdr = NULL;
                UINT32 u4AddrYOffset,u4AddrCOffset;

                prSeqHdr = FBM_GetFrameBufferSeqHdr(prFrcPrm->ucFbgId);
                if (prSeqHdr != NULL)
                {
                    u4AddrYOffset = prSeqHdr->u4YAddrOffset;
                    u4AddrCOffset = prSeqHdr->u4CAddrOffset;
                }
                else
                {
                    u4AddrYOffset = 0;
                    u4AddrCOffset = 0;
                }

                // Check if new seamless
                if (prFbmPicHdr != NULL && prFbmPicHdr->fgSeamlessDisp)
                {
                    FBM_GetResizeFrameBufferAddr(prFrcPrm->ucFbgId, prFbmPicHdr->ucSeamlessResizeFbid, &u4AddrY, &u4AddrC);
                }
                _VDP_HalSetAddr(ucB2rId, u4AddrY+u4AddrYOffset, u4AddrC+u4AddrCOffset);

#ifdef CC_3D_MM_DS_SUPPORT
                if (((_eSpeed[ucB2rId] == STC_SPEED_TYPE_FORWARD_1X) || (_eSpeed[ucB2rId] != STC_SPEED_TYPE_FORWARD_1X && prFrcPrm->u1B2R3DTrickModeType == B2R_3D_TRICK_MODE_3D_VIEW))
                        && (prFrcPrm->fgB2R3DEnable)
                        && (prFrcPrm->u1B2R3DType == B2R_3D_FRAME_SEQUENTIAL))
                {
                    FBM_PIC_HDR_T* prRightPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucSubFbgId, prFrcPrm->ucSubFbId);
                    if (prRightPicHdr != NULL && (prRightPicHdr->ucSeamlessResizeFbid != FBM_FB_ID_UNKNOWN))
                    {
                        FBM_GetResizeFrameBufferAddr(prFrcPrm->ucSubFbgId, prRightPicHdr->ucSeamlessResizeFbid, &u4AddrYSub, &u4AddrCSub);
                    }

                    _VDP_HalSetRViewAddr(ucB2rId, u4AddrYSub, u4AddrCSub);
                }
                else
                {
                    _VDP_HalSetRViewAddr(ucB2rId, u4AddrY+u4AddrYOffset, u4AddrC+u4AddrCOffset);
                }
#endif
            }
        }
    }
#else
    _VDP_HalSetAddr(ucB2rId, u4AddrY, u4AddrC);
#ifdef CC_3D_MM_DS_SUPPORT
    if (((_eSpeed[ucB2rId] == STC_SPEED_TYPE_FORWARD_1X) || (_eSpeed[ucB2rId] != STC_SPEED_TYPE_FORWARD_1X && prFrcPrm->u1B2R3DTrickModeType == B2R_3D_TRICK_MODE_3D_VIEW))
            && (prFrcPrm->fgB2R3DEnable)
            && (prFrcPrm->u1B2R3DType == B2R_3D_FRAME_SEQUENTIAL))
    {
        _VDP_HalSetRViewAddr(ucB2rId, u4AddrYSub, u4AddrCSub);
    }
    else
    {
        _VDP_HalSetRViewAddr(ucB2rId, u4AddrY, u4AddrC);
    }
#endif
#endif

#if 0
    /* Check whether b2r update stc or not */

    UCHAR ucAvSyncMode;
    UCHAR ucStcSrc;

    FBM_GetSyncStc(prFrcPrm->ucFbgId, &ucAvSyncMode, &ucStcSrc);

    if ( VDP_GetB2rUpdateStc() || ( AV_SYNC_FREE_RUN == ucAvSyncMode ) )
    {
        if ( prFrcPrm->ucVdpId < VDP_NS )
        {
            VDP_SetB2rStc(_VDP_GetDispingPts(prFrcPrm->ucVdpId));
            STC_SetStcValue(VDP_GetB2rStc());
        }
    }
#endif

    return;
}


//LEO_B2R_REFACTORY ,LEO,20120124
static void _VdpHandleStepForward(B2R_PRM_T* prFrcPrm,UCHAR ucB2rId)
{
    UCHAR ucVdpId = VDP_GetVdpId(ucB2rId);
    /* Step Forward at 20081030 */
    if ((VDP_HalGetStepForward(ucB2rId) == VDP_PLAY_STEP_FORWARD))
    {
        if (prFrcPrm->ucRealChangeFb == 0)
        {
            LOG(3, "STEP MM wait STC Queue 1 Frame\n");
        }
        else
        {
            LOG(3, "STEP Real change FB PAUSE\n");
            /* Play One Frame and then Pause */
            _VDP_SetPauseMM(ucB2rId, TRUE);

            if (FBM_ChkFrameBufferPicFlag(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_MM_PSEUDO_EOS_FLAG))
            {
                _VDP_VsyncNotify(VDP_MSG_MM_STEP_FIN_CB, ucVdpId, 0, VDP_SEEK_STEP_NO_DATA);
            }
            else
            {
                _VDP_VsyncNotify(VDP_MSG_MM_STEP_FIN_CB, ucVdpId, 0, VDP_SEEK_STEP_OK);
            }
        }
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VdpFrmInit.
 */
//-----------------------------------------------------------------------------
static void _VdpFrmInit(UCHAR ucB2rId, UCHAR ucPort, BOOL fgCreateThread)
{
    B2R_PRM_T rVdpPrm;
    UCHAR ucVdpId;

    UNUSED(ucPort);
    UNUSED(_ucRepeatFbForPushTrickMode);
    if (ucB2rId >= B2R_NS)
    {
        LOG(1, "[%s-%d]Input ucB2rId(%d) error!.\n", __func__, __LINE__, ucB2rId);
        ASSERT(ucB2rId < B2R_NS);
        return;
    }
    ucVdpId = VDP_GetVdpId(ucB2rId);

    // backup those we don't want to reset
    //rVdpPrm = _arVdpPrm[ucB2rId];//TBD
    x_memcpy(&rVdpPrm, &(_arVdpPrm[ucB2rId]), sizeof(B2R_PRM_T));

    _u4DriftCount[ucB2rId] = 0;
    _u4MaxDriftCount[ucB2rId] = 0;
    _u4ValidCount[ucB2rId] = 0;

    _ucFrameRateUnkownCount[ucB2rId]=0x0;

    _fgModeChangeSent = FALSE;
    _u4LastWidth = 0;
    _u4LastHeight = 0;

	_u4RepeatCntForPushTrickMode = 0;
	_u4ElapseCntForPushTrickMode = 0;
	_ucRepeatFbForPushTrickMode = FBM_FB_ID_UNKNOWN;
#ifdef VDP_AVS_ADJUST_STC
    _VDP_SetDeltaInit(&_rB2rAvs);
#endif

    FBM_RegCbFunc(FBM_CB_FUNC_FB_READY_IND, (UINT32)_VdpFrcFrameBufferReady);

    _VdpGfxCopyFieldInit();

#if 0 // temporal disable. TODO: check recording multithread problem
    // Release locked buffer before memory set
    if (_prVdpConf[ucVdpId] && (B2R_GetEs2Fbg(_prVdpConf[ucVdpId]->ucInputPort[0]) == FBM_FBG_ID_UNKNOWN))
    {
        if (_arVdpPrm[ucVdpId].ucFbgId != FBM_FBG_ID_UNKNOWN)
        {
            FBM_SetFrameBufferFlag(_arVdpPrm[ucVdpId].ucFbgId, FBM_FLAG_SEEK_MODE);	// Set seek mode (it is just avoid assertion, like vdec).
            FBM_ReleaseDispQ(_arVdpPrm[ucVdpId].ucFbgId);
            if (_arVdpPrm[ucVdpId].ucFbId != FBM_FB_ID_UNKNOWN)
            {
                if (FBM_CheckFrameBufferStatus(_arVdpPrm[ucVdpId].ucFbgId, _arVdpPrm[ucVdpId].ucFbId, FBM_FB_STATUS_LOCK))
                {
                    FBM_SetFrameBufferStatus(_arVdpPrm[ucVdpId].ucFbgId, _arVdpPrm[ucVdpId].ucFbId, FBM_FB_STATUS_EMPTY);
                }
            }
            if (_arVdpPrm[ucVdpId].ucReleaseFbId != FBM_FB_ID_UNKNOWN)
            {
                if (FBM_CheckFrameBufferStatus(_arVdpPrm[ucVdpId].ucFbgId, _arVdpPrm[ucVdpId].ucReleaseFbId, FBM_FB_STATUS_LOCK))
                {
                    FBM_SetFrameBufferStatus(_arVdpPrm[ucVdpId].ucFbgId, _arVdpPrm[ucVdpId].ucReleaseFbId, FBM_FB_STATUS_EMPTY);
                }
            }
            FBM_ClrFrameBufferFlag(_arVdpPrm[ucVdpId].ucFbgId, FBM_FLAG_SEEK_MODE);
        }
    }
#endif

#ifdef CC_3D_MM_DS_SUPPORT
    if (_arVdpPrm[ucB2rId].ucSubFbgId != FBM_FBG_ID_UNKNOWN && _arVdpPrm[ucB2rId].ucSubFbId != FBM_FB_ID_UNKNOWN)
    {
        _VdpSetFrameBufferStatus(&(_arVdpPrm[ucB2rId]), _arVdpPrm[ucB2rId].ucSubFbgId, _arVdpPrm[ucB2rId].ucSubFbId, FBM_FB_STATUS_EMPTY);
    }
    if (_arVdpPrm[ucB2rId].ucSubFbgId != FBM_FBG_ID_UNKNOWN && _arVdpPrm[ucB2rId].ucSubReleaseFbId != FBM_FB_ID_UNKNOWN)
    {
        _VdpSetFrameBufferStatus(&(_arVdpPrm[ucB2rId]), _arVdpPrm[ucB2rId].ucSubFbgId, _arVdpPrm[ucB2rId].ucSubReleaseFbId, FBM_FB_STATUS_EMPTY);
    }
    if (_arVdpPrm[ucB2rId].ucSubFbgId != FBM_FBG_ID_UNKNOWN && _arVdpPrm[ucB2rId].ucSubPendingFbId != FBM_FB_ID_UNKNOWN)
    {
        _VdpSetFrameBufferStatus(&(_arVdpPrm[ucB2rId]), _arVdpPrm[ucB2rId].ucSubFbgId, _arVdpPrm[ucB2rId].ucSubPendingFbId, FBM_FB_STATUS_EMPTY);
    }
#endif

    FRC_MEMSET((void *)&(_arVdpPrm[ucB2rId]), 0, sizeof(B2R_PRM_T));

    _arVdpPrm[ucB2rId].ucB2rId = ucB2rId;
#ifdef CC_SUPPORT_TVE
    if (fgCreateThread)
    {
        _vDrvVideoSetMute(MUTE_MODULE_DTV, ucB2rId, FOREVER_MUTE, FALSE);  
    }
    else
    {
        _vDrvVideoSetMute(MUTE_MODULE_DTV, ucVdpId, FOREVER_MUTE, FALSE);  
    }
#else
    _VDP_HalForceBg(ucB2rId, 1);
#endif

    if (ucVdpId < VDP_NS)
    {
        // Output Info, can not be init again
        _arVdpPrm[ucB2rId].ucOutFrameRate = _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate;
        _arVdpPrm[ucB2rId].ucProgressiveOut = _prVdpConf[ucVdpId]->rOutInfo.ucPrg;
    }
    if (_arVdpPrm[ucB2rId].ucOutFrameRate)
    {
        _arVdpPrm[ucB2rId].u4OutStcPeriod =
            (UINT32)(VDP_STC_CLOCK / _arVdpPrm[ucB2rId].ucOutFrameRate);
    }
    else
    {
        _arVdpPrm[ucB2rId].u4OutStcPeriod = (VDP_STC_CLOCK / 60);
    }

#ifdef CC_B2R_WAIT_NPTV_STABLE
    _au4WaitNPTVStableCount[ucB2rId] =0;
    _afgNPTVStable[ucB2rId] = FALSE;
#endif

    // default value
    _eSpeed[ucB2rId] = STC_SPEED_TYPE_FORWARD_1X;
    _u4PreviousResizeWidth = 0;
    _u4PreviousResizeHeight = 0;
    _u4PreviousResizeWidth2 = 0;
    _u4PreviousResizeHeight2 = 0;

#ifdef CC_3D_MM_DS_SUPPORT
    _arVdpPrm[ucB2rId].ucSubFbgId = FBM_FBG_ID_UNKNOWN;
    _arVdpPrm[ucB2rId].ucSubFbId = FBM_FB_ID_UNKNOWN;
    _arVdpPrm[ucB2rId].ucSubReleaseFbId = FBM_FB_ID_UNKNOWN;
    _arVdpPrm[ucB2rId].ucSubPendingFbId = FBM_FB_ID_UNKNOWN;
    _arVdpPrm[ucB2rId].ucSubFbgIdRecorded = FBM_FBG_ID_UNKNOWN;
#endif

    _arVdpPrm[ucB2rId].ucFbgId = FBM_FBG_ID_UNKNOWN;
    _arVdpPrm[ucB2rId].ucFbId = FBM_FB_ID_UNKNOWN;
    _arVdpPrm[ucB2rId].ucReleaseFbId = FBM_FB_ID_UNKNOWN;
    _arVdpPrm[ucB2rId].ucPendingFbId = FBM_FB_ID_UNKNOWN;
    _arVdpPrm[ucB2rId].ucLastFbId = FBM_FB_ID_UNKNOWN;
    _arVdpPrm[ucB2rId].fgReleaseDispQ= FALSE;

    _arVdpPrm[ucB2rId].ucLastAcsFrame = ~0;
    _arVdpPrm[ucB2rId].ucLastAcsAuto = ~0;

    _arVdpPrm[ucB2rId].u4CbSyncEn = rVdpPrm.u4CbSyncEn;
    _arVdpPrm[ucB2rId].u4CbSyncThrsd= rVdpPrm.u4CbSyncThrsd;
    _arVdpPrm[ucB2rId].u4CbSyncChkFrmCnt= rVdpPrm.u4CbSyncChkFrmCnt;

    _arVdpPrm[ucB2rId].ptRtCli = rVdpPrm.ptRtCli;
    _arVdpPrm[ucB2rId].prFbCounter = &(_prDbase->rVdp.arFbCounter[ucB2rId][0]);
    _arVdpPrm[ucB2rId].u4B2rSkipFrameNo = 0;

    _rFbmAspectRatio.u1AspectRatioType = MPEG_ASPECT_RATIO_4_3;
    _rFbmAspectRatio.u2AspectHorizSize = 0;
    _rFbmAspectRatio.u2AspectVertSize =0;

    _fgRepeatFrame = FALSE;
    _fgDropFrame = FALSE;

    FRC_MEMSET((void *)_arVdpPrm[ucB2rId].prFbCounter, 0, sizeof(PARAM_VDP_FB_COUNTER_T));

#ifdef CC_MAPLE_CUST_DRV
    _arVdpPrm[ucB2rId].pt_cust_info = rVdpPrm.pt_cust_info;
    if(!_arVdpPrm[ucB2rId].pt_cust_info)
    {
        _arVdpPrm[ucB2rId].pt_cust_info =
            (B2R_CUST_INFO_T*)x_mem_alloc(sizeof(B2R_CUST_INFO_T));
    }
#endif

#ifdef CC_B2R_ENABLE_CHG_FRAME_MSG
    _VDP_FlushB2RChgFrameMsg(ucB2rId);
#endif

    // for _VdpCheckOutputMode, if ucPmxMode != VDP_B2R_MODE_UNKNOWN, no output change
    //[DTV00129735] DTV SCART out consideration, not to reset PMX mode
    if (ucVdpId < VDP_NS)
    { 
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_UNKNOWN;
        _au4OriSrcWidth[ucVdpId] = 0;
        _au4OriSrcHeight[ucVdpId] = 0;
    }
    //LOG(1, "_prVdpConf[%d]->rOutInfo.ucPmxMode = %d;\n", ucVdpId, VDP_B2R_MODE_UNKNOWN);

#ifdef DIVX_PLUS_CER
#ifdef   CC_B2R_CHAPTER_CHG
    if (!fgFirstSetChapter)
    {
        x_memset(&_arVdpChapter,0,sizeof(VDP_CHAPTER_T));
    }
    else
    {
        fgFirstSetChapter = FALSE;
    }
#endif
#endif

    _VDP_VsyncNotify(VDP_MSG_UNMUTE_CB, (UINT32)ucVdpId, 0, 0);
}

//-----------------------------------------------------------------------------
/** Brief of _VdpChangeFrameBuffer.
 */
//-----------------------------------------------------------------------------
static void _VdpChangeFrameBuffer(B2R_PRM_T* prFrcPrm)
{
    UCHAR ucAvSyncMode;
    UCHAR ucStcSrc;
    UCHAR ucOriginalFlow = 1;
    UCHAR ucB2rId;
    UCHAR ucPlayMode;
    UCHAR ucVdpId;

    if (prFrcPrm == NULL)
    {
        return;
    }
    ucB2rId = prFrcPrm->ucB2rId;
    if (ucB2rId >= B2R_NS)
    {
        LOG(0, "ucB2rId >= B2R_NS , oops \n");
        return;
    }
    ucVdpId = VDP_GetVdpId(prFrcPrm->ucB2rId);
    if (ucVdpId >= VDP_NS)
    {
        LOG(1, "[%s-%d]VdpId(%d) error!.\n", __func__, __LINE__, ucVdpId);
        return;
    }

    if ( FBM_CheckFbg(prFrcPrm->ucFbgId) )
    {
        return;
    }

    if (prFrcPrm->ucFbgId != FBM_FBG_ID_UNKNOWN)
    {
        UCHAR ucFrcMode;
        UINT16 u2OldTargetNs;
        UCHAR u2OldChangeFieldNs;

        // release last frame buffer (next ISR)
        // if we release it here, tearing will happen since this frame buffer is still displaying
        if (FBM_GetFrameBufferNs(prFrcPrm->ucFbgId) > 1)
        {
            ASSERT(prFrcPrm->ucReleaseFbId == FBM_FB_ID_UNKNOWN);
            prFrcPrm->ucReleaseFbId = prFrcPrm->ucFbId;
#ifdef CC_3D_MM_DS_SUPPORT
            prFrcPrm->ucSubReleaseFbId = prFrcPrm->ucSubFbId;
#endif
        }

        // backup for fail
        u2OldTargetNs = prFrcPrm->u2TargetNs;
        u2OldChangeFieldNs = prFrcPrm->u2ChangeFieldNs;

        ucB2rId = prFrcPrm->ucB2rId;
#if 1
        if (_fgEnableWaitSTC[ucB2rId])
        {
#ifdef ENABLE_MULTIMEDIA
            UCHAR ucEsId;
            VDEC_ES_INFO_T *prVdecEsInfo = NULL;
            ucEsId = _prVdpConf[ucVdpId]->ucInputPort[0];
            prVdecEsInfo = (ucEsId < MAX_ES_NS)? _VDEC_GetEsInfo(ucEsId) : NULL;
#endif
            /* 20081113 MM start, STC will be updated by AUDIO but not yet updated */
            FBM_GetSyncStc(prFrcPrm->ucFbgId, &ucAvSyncMode, &ucStcSrc);
            if ((prFrcPrm->ucReady == 0) &&
                    ((ucAvSyncMode == VID_SYNC_MODE_SLAVE_MM)
#ifdef ENABLE_MULTIMEDIA
                     || (prVdecEsInfo && prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS_TIME_SHIFT)
#endif
                    ))
            {
                UCHAR ucPreLookFbId;
                FBM_PIC_HDR_T* prPreLookNextPicHdr;
                STC_CLOCK rStcClk;

                ucPreLookFbId = _VdpPreLookNextDisp(prFrcPrm);
                if (ucPreLookFbId != FBM_FB_ID_UNKNOWN)
                {
                    if (STC_GetSrc(ucStcSrc, &rStcClk) == STC_VALID)
                    {
                        UINT32 u4Delta;

                        prPreLookNextPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, ucPreLookFbId);
                        if (prPreLookNextPicHdr != NULL)
                        {
                            if ((prPreLookNextPicHdr->u4PTS) > (rStcClk.u4Base))
                            {
                                u4Delta = ((prPreLookNextPicHdr->u4PTS) - (rStcClk.u4Base));

                                if (u4Delta >= ((UINT32)0x80000000))
                                {
                                    // handle wrap around
                                    u4Delta = (0xFFFFFFFF - u4Delta);
                                }
                            }
                            else
                            {
                                u4Delta = ((rStcClk.u4Base) - (prPreLookNextPicHdr->u4PTS));

                                if (u4Delta >= ((UINT32)0x80000000))
                                {
                                    // handle wrap around
                                    u4Delta = (0xFFFFFFFF - u4Delta);
                                }
                            }
                            LOG(3, "--- PTS(%X), STC(%X), Diff(%d), OutStcPeriod(%d)\n",
                                (prPreLookNextPicHdr->u4PTS), rStcClk.u4Base,
                                u4Delta, prFrcPrm->u4OutStcPeriod);
#if 1
                            if (u4Delta > STC_PTS_DIFF)
                            {
                                ucOriginalFlow = 0;
                            }
#endif
                        }
                    }
                }
            }
            /* 20081113 MM start, STC will be updated by AUDIO but not yet updated */
        }
#endif

        if (ucOriginalFlow == 1)
        {
            if (((_arVdpDtvFreeze[ucB2rId].u4OnOff == 0) ||
                    (prFrcPrm->ucReady == 0)) &&
                    (_arVdpDtvFreeze[ucB2rId].u4Recovery == 0) &&
                    (_fgPauseMM[ucB2rId] == 0))
            {
                _VdpGetNextDisp(prFrcPrm);
            }
            else
            {
                prFrcPrm->u2TargetNs = 0;
            }
        }
        else
        {
            prFrcPrm->u2TargetNs = 0;
        }

        if ((prFrcPrm->u2TargetNs != 0) &&
                (prFrcPrm->ucFbId != FBM_FB_ID_UNKNOWN))
        {
            BOOL fgFastForward;
            UINT32 u4AddrY;
            UINT32 u4AddrC;

#if 0
// for Test, slow motion
            prFrcPrm->u2TargetNs *= 10;
            prFrcPrm->u2ChangeFieldNs *= 10;
#endif

            // update VSYNC Ns
            FRC_SET_VSYNC_NS(prFrcPrm, prFrcPrm->u2TargetNs, prFrcPrm->u2ChangeFieldNs);

            if ((prFrcPrm->ucWrongField) || (prFrcPrm->ucMultipleField))
            {
                ucFrcMode = VDP_FRC_NONE;
            }
            else
            {
                ucFrcMode = VDP_FRC_NORMAL;
            }

            prFrcPrm->ucStopAuto = 0;

            if (prFrcPrm->ucNewFbArrive != 0)
            {
                prFrcPrm->ucNewFbArrive = 0;

                // Display Fail Count -1
                if (prFrcPrm->prFbCounter)
                {
                    if (prFrcPrm->prFbCounter->u4DispFail > 0)
                    {
                        prFrcPrm->prFbCounter->u4DispFail--;
                        //LOG(3, "DispFail--\n");
                    }
                    ucFrcMode = VDP_FRC_CANCEL;
                }
            }

#ifdef CC_VDP_NO_INTERLACE_FRC
            if (
#if 0 //5387
                (_prVdpConf[prFrcPrm->ucVdpId]->ucDispMode != 0) ||
#else
                (getScalerMode(ucVdpId) != 0) ||
#endif
                (_prVdpConf[ucVdpId]->ucBobMode != 0))
            {
                ucFrcMode = VDP_FRC_NONE;
            }
#endif

            /* Step Forward at 20081030 */
            _VdpHandleStepForward(prFrcPrm ,ucB2rId); //LEO_B2R_REFACTORY

            if (ucB2rId < B2R_NS)
            {
                if (_eSpeed[ucB2rId] <= STC_SPEED_TYPE_FORWARD_1_DOT_5X)
                {
                    fgFastForward = TRUE;
                }
                else
                {
                    fgFastForward = FALSE;
                }
            }
            else
            {
                fgFastForward = TRUE;
            }
            UNUSED(fgFastForward);

            if (!arVdpSeek[ucB2rId].fgABReach)
            {
                if (FBM_ChkFrameBufferPicFlag(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_MM_PSEUDO_EOS_FLAG))
                {
                    FBM_ClrFrameBufferPicFlag(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_MM_PSEUDO_EOS_FLAG);

                    if (!prFrcPrm->rAB.fgValid)  // if in AB repeat mode, no need to notify EOS
                    {

                        _VDP_VsyncNotify(VDP_MSG_MM_CB, (UINT32)ucVdpId, VDP_COND_EOS, FBM_GetDecoderSrcId(prFrcPrm->ucFbgId));
                        LOG(3, "FBM_MM_PSEUDO_EOS_FLAG Notify\n");

                    }
                    else
                    {
                    	  MMPlaybackSpeedTestAddTimeStamp(TIME_STAMP_VDP_ABREPEAT_DONE);
                        _VDP_VsyncNotify(VDP_MSG_MM_CB, (UINT32)ucVdpId, VDP_COND_RANGE_AB_DONE, fgFastForward);
                        LOG(3, "VDP_COND_RANGE_AB_DONE Notify\n");
                    }
                }
            }

            /* Trick Play at 20081001 */
            FBM_GetSyncStc(prFrcPrm->ucFbgId, &ucAvSyncMode, &ucStcSrc);

#if 1 // Display Success should not set Frame Repeat in Trick mode
            // for CR DTV00144143
            //UCHAR ucPlayMode;
            FBM_GetPlayMode(prFrcPrm->ucFbgId,&ucPlayMode);
            if ( FBM_FBG_MM_MODE == ucPlayMode )
            {
                static UINT32 u4PrePTS = 0xFFFFFFFF;
                if (prFrcPrm->u4Pts == u4PrePTS)
                {
                    ucFrcMode = VDP_FRC_TRICK_PLAY;
                }
                u4PrePTS = prFrcPrm->u4Pts;
                if (_VDP_GetFrcStcSpeed(ucB2rId) != STC_SPEED_TYPE_FORWARD_2X && VDP_HalGetTrickMode(ucB2rId) == VDP_PLAY_TRICK)
                {
                    ucFrcMode = VDP_FRC_TRICK_PLAY;
                }
                LOG(10,"display success : Pts(%x),Fbid(%d)\n",prFrcPrm->u4Pts,prFrcPrm->ucFbId);
            }
#endif

            _VDP_HalSetFrc(ucB2rId,
                           prFrcPrm->u2TargetNs,
                           prFrcPrm->u2ChangeFieldNs,
                           ucFrcMode);

            // update frame buffer addr
            FBM_GetFrameBufferAddr(prFrcPrm->ucFbgId,prFrcPrm->ucFbId, &u4AddrY, &u4AddrC);

#ifndef CC_B2R_ENABLE_SEAMLESS_THREAD
            _VDP_SeamlessJob(prFrcPrm);
#else
            VERIFY (x_sema_unlock(_hSeamlessSema) == OSR_OK);
#endif

#ifdef __MODEL_slt__
            SLT_CurrentPts(SLT_TYPE_VDO, prFrcPrm->u4Pts);
#endif

            prFrcPrm->u4FrameRepeated = 0;
            prFrcPrm->u4FrameSuccess++;

#ifdef CC_VDP_NO_INTERLACE_FRC
            prFrcPrm->u2DisplayFailNs = 0;
#endif
        }
        else
        {
            LOG(10,"display fail : Pts(%x),Fbid(%d)\n",prFrcPrm->u4Pts,prFrcPrm->ucLastFbId);
            // Pending New FB (MPEG) at get display Q fail, except
            // 1) not ready
            // 2) freeze
            // 3) Fail Count < 2 (ST, u4FrameRepeated @W[, ݭn Pending New FB)
            // 4) Success >= 4 (sT~Ұ Pending, OI, Pɥ]tF Fail Count < 2) (prFrcPrm->u4FrameSuccess >= 4)
            // 5) ...
            if ((prFrcPrm->ucReady != 0) &&
                    (_arVdpDtvFreeze[ucB2rId].u4OnOff == 0) &&
                    (prFrcPrm->ucMMWaitSTC != 1))
            {
                if ((FBM_ChkFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_DEBLOCKING)) &&
                        (FBM_GetFrameBufferNs(prFrcPrm->ucFbgId) <= 6))
                {
                    // 5FB Deblocking
                    prFrcPrm->ucPendingNewFb = 1;
                }
                else if (FBM_ChkFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_THREE_B))
                {
                    prFrcPrm->ucPendingNewFb = 1;
                }

                prFrcPrm->ucPendingNewFb = 1;
            }

#ifdef CC_B2R_WAIT_NPTV_STABLE
            if (_au4WaitNPTVStableCount[ucB2rId] > 0)
            {
                prFrcPrm->ucPendingNewFb = 0;
            }
#endif

            if (prFrcPrm->ucNewFbArrive != 0)
            {
                prFrcPrm->ucNewFbArrive = 0;

                // possible
                // SS: againg test with power on/off.
//                ASSERT(0);  // possible?
            }
            if (_arVdpDtvFreeze[ucB2rId].u4OnOff != 0)
            {
                if (prFrcPrm->ucProgressiveSeqFlag == 0)
                {
                    if (_arVdpDtvFreeze[ucB2rId].u4CopyField == 0)
                    {
                        _arVdpDtvFreeze[ucB2rId].u4CopyField =
                            _VdpGfxCopyFieldStart(prFrcPrm->ucFbgId, prFrcPrm->ucFbId);
                    }
                }

                if (_arVdpDtvFreeze[ucB2rId].u4CleanDispQ == 0)
                {
                    FBM_ReleaseDispQ(prFrcPrm->ucFbgId);
                    _arVdpDtvFreeze[ucB2rId].u4CleanDispQ = 1;
                }
            }
            else if (_fgPauseMM[ucB2rId] != 0)
            {
                if (prFrcPrm->ucProgressiveSeqFlag == 0)
                {
                    UINT32 u4CopyField;
                    u4CopyField = _VdpGfxCopyFieldStart(prFrcPrm->ucFbgId, prFrcPrm->ucFbId);

                    if (u4CopyField == 0)
                    {
                        LOG(5, "No Gfx copy field");
                    }
                }
            }

            // Visual Quality Tuning
            // Handle display next fail
            // T B T B B B T B (Old)
            //         ^^^ Fail
            // T B T B T B T B (New)
            //         ^^^ Fail

            // restore
            prFrcPrm->ucReleaseFbId = FBM_FB_ID_UNKNOWN;
            prFrcPrm->u2TargetNs = u2OldTargetNs;
#ifdef CC_3D_MM_DS_SUPPORT
            prFrcPrm->ucSubReleaseFbId = FBM_FB_ID_UNKNOWN;
#endif

            //// TODO, system crash
#if 0
            prFrcPrm->u2ChangeFieldNs = u2OldChangeFieldNs;

            prFrcPrm->u4FrameRepeated++;

            ucFrcMode = VDP_FRC_NORMAL;

            prFrcPrm->ucStopAuto = 0;
#endif

            prFrcPrm->u4FrameSuccess = 0;
            /*if (((prFrcPrm->u4FrameRepeated < 2) &&
                    (_fgPauseMM[ucB2rId] == 0) &&
                    (prFrcPrm->ucMMWaitSTC != 1)) || fgMainMuteOn())*/

#ifdef FBM_VIRTUAL_MPEG
            if (TRUE)
#else
            if (FALSE)
#endif

            {
                // reduce change to STOP AUTO

                prFrcPrm->u2ChangeFieldNs = u2OldChangeFieldNs;

                prFrcPrm->u4FrameRepeated++;

                if (prFrcPrm->ucMultipleField)
                {
                    ucFrcMode = VDP_FRC_NONE;
                }
                else
                {
                    ucFrcMode = VDP_FRC_NORMAL;
                }

                prFrcPrm->ucStopAuto = 0;
            }
            else
            {
                /* MM Pause or MM pending for AVS-R also STOP AUTO */
                prFrcPrm->u2ChangeFieldNs = u2OldTargetNs;

                /* 2-3 pulldown sequence. Repeat on 3-field display frame
                   To avoid B2R repeat count as odd number. */
                if ((prFrcPrm->ucWrongField) || (prFrcPrm->ucMultipleField))
                {
                    if ((prFrcPrm->u2TargetNs == 3) && (prFrcPrm->ucProgressiveOut == 0))
                    {
                        prFrcPrm->u2TargetNs = 2;
                        prFrcPrm->u2ChangeFieldNs = 2;
                    }
                }

                prFrcPrm->ucMultipleField = 0;

                ucFrcMode = VDP_FRC_STOP_AUTO;

                prFrcPrm->ucStopAuto = 1;

                if (VDP_HalGetTrickMode(ucB2rId) != VDP_PLAY_TRICK)
                {
                    UINT32 u4Ret;
                    u4Ret = _VdpGfxCopyFieldStart(prFrcPrm->ucFbgId,prFrcPrm->ucFbId);
                    if (u4Ret ==0 )
                    {
                        LOG(5,"Gfx doesn't copy top field to bottom field\n");
                    }
                }
            }

            // Recovery from mpeg freeze to normal playback
            if (_arVdpDtvFreeze[ucB2rId].u4Recovery != 0)
            {
                UINT32 u4NextPts;

                _arVdpDtvFreeze[ucB2rId].u4Recovery = 0;

                u4NextPts = FBM_GetNextDispQPts(prFrcPrm->ucFbgId);

                if (u4NextPts != 0)
                {
                    FBM_GetSyncStc(prFrcPrm->ucFbgId, &ucAvSyncMode, &ucStcSrc);

#ifndef CC_VDP_FPGA
                    if (ucAvSyncMode != VID_SYNC_MODE_NONE)
                    {
                        STC_CLOCK rStcClk;

                        if (STC_GetSrc(ucStcSrc, &rStcClk) == STC_VALID)
                        {
                            INT32 i4Delta;
                            UINT32 u4Delta;

                            if (u4NextPts > rStcClk.u4Base)
                            {
                                u4Delta = (u4NextPts - rStcClk.u4Base);

                                if (u4Delta < ((UINT32)0x80000000))
                                {
                                    i4Delta = - (INT32) u4Delta;
                                }
                                else
                                {
                                    // handle wrap around
                                    u4Delta = (0xFFFFFFFF - u4Delta);
                                    i4Delta = (INT32) u4Delta;
                                }
                            }
                            else
                            {
                                u4Delta = (rStcClk.u4Base - u4NextPts);

                                if (u4Delta < ((UINT32)0x80000000))
                                {
                                    i4Delta = (INT32) u4Delta;
                                }
                                else
                                {
                                    // handle wrap around
                                    u4Delta = (0xFFFFFFFF - u4Delta);
                                    i4Delta = - (INT32) u4Delta;
                                }
                            }

                            if (i4Delta < 0)
                            {
                                // PTS slower than STC, repeat

                                UINT32 u4OutFrameStc;
                                UINT32 u4Step;

                                u4OutFrameStc = (prFrcPrm->u2TargetNs * prFrcPrm->u4OutStcPeriod);

                                if (u4OutFrameStc != 0)
                                {
                                    u4Step = (u4Delta / u4OutFrameStc);

                                    if (u4Step != 0)
                                    {
                                        prFrcPrm->u2ChangeFieldNs *= u4Step;
                                        prFrcPrm->u2TargetNs *= u4Step;

                                        LOG(3, "Freeze-R %d PTS(0x%x) STC(0x%x)\n", prFrcPrm->u2TargetNs, u4NextPts, rStcClk.u4Base);
                                    }
                                }
                            }
                            else
                            {
                                // imposible !
                            }
                        }
                    }
#endif
                }
            }

#ifdef CC_VDP_NO_INTERLACE_FRC
#if 0 //5387
            if (_prVdpConf[prFrcPrm->ucVdpId]->ucDispMode != 0)
#else
            if (getScalerMode(ucVdpId) != 0)
#endif
            {
                ucFrcMode = VDP_FRC_NONE;
            }

            // Display Mode ON & Interlace Output mode
            if ((_arVdpDtvFreeze[ucB2rId].u4OnOff == 0) &&
#if 0 //5387
                    (_prVdpConf[prFrcPrm->ucVdpId]->ucDispMode != 0) &&
#else
                    (getScalerMode(ucVdpId) != 0) &&
#endif
                    (prFrcPrm->ucProgressiveOut == 0))
            {
                if (prFrcPrm->u2DisplayFailNs != VDP_DISPLAY_FAIL_FREEZE)
                {
                    prFrcPrm->u2DisplayFailNs += prFrcPrm->u2TargetNs;

                    // Fail > 1/4 Second
                    if (prFrcPrm->u2DisplayFailNs >= (prFrcPrm->ucOutFrameRate >> 2))
                    {
                        LOG(1,"Display Fail Start Gfx Copy Field\n");

                        prFrcPrm->u2DisplayFailNs = VDP_DISPLAY_FAIL_FREEZE;

                        _rVdpGfxCopyField.ucFbgId = prFrcPrm->ucFbgId;
                        _rVdpGfxCopyField.ucFbId = prFrcPrm->ucFbId;
                        // release semaphore, use gfx to copy top field to bottom field
                        VERIFY (x_sema_unlock(_rVdpGfxCopyField.hSemaphore) == OSR_OK);
                    }
                }
            }
#endif

            // update VSYNC Ns
            FRC_SET_VSYNC_NS(prFrcPrm, prFrcPrm->u2TargetNs, prFrcPrm->u2ChangeFieldNs);

#if 0 /* display fail, no need to pause in step forward state */
            /* Step Forward at 20081030 */
            if ((VDP_HalGetStepForward() == VDP_PLAY_STEP_FORWARD))
            {
                /* Play One Frame and then Pause */
                VDP_SetPauseMm(prFrcPrm->ucVdpId, TRUE);
            }
#endif

#if 1
            /* Trick Play at 20081001 */
            FBM_GetSyncStc(prFrcPrm->ucFbgId, &ucAvSyncMode, &ucStcSrc);
            if ((VDP_HalGetTrickMode(ucB2rId)==VDP_PLAY_TRICK) && (ucAvSyncMode==VID_SYNC_MODE_NONE))
            {
#if 0
                if (prFrcPrm->ucPendingNewFb == 1)
                {
                    /* 4x up FF or 2x up FB will trigger Frame Repeat
                    Not to allow Late Notify since Frame Repeat is triggered */
                    prFrcPrm->ucPendingNewFb = 0;
                }
#endif
                //ucFrcMode = VDP_FRC_TRICK_PLAY;
            }




#endif

#if 0 // Display Success should not set Frame Repeat in Trick mode
            // for CR DTV00144143
            UCHAR ucPlayMode;
            FBM_GetPlayMode(prFrcPrm->ucFbgId,&ucPlayMode);
            if ( FBM_FBG_MM_MODE == ucPlayMode )
            {
                static UINT32 u4PrePTS = 0xFFFFFFFF;

                if (prFrcPrm->u4Pts == u4PrePTS)
                {
                    ucFrcMode = VDP_FRC_NONE;
                }

                u4PrePTS = prFrcPrm->u4Pts;

                LOG(1,"display success : Pts(%x),Fbid(%d)\n",prFrcPrm->u4Pts,prFrcPrm->ucFbId);
            }
#endif
            _VDP_HalSetFrc(ucB2rId,
                           prFrcPrm->u2TargetNs,
                           prFrcPrm->u2ChangeFieldNs,
                           ucFrcMode);

            // release current frame buffer until new buffer is ready
            prFrcPrm->ucReleaseFbId = FBM_FB_ID_UNKNOWN;
#ifdef CC_3D_MM_DS_SUPPORT
            prFrcPrm->ucSubReleaseFbId = FBM_FB_ID_UNKNOWN;
#endif
        }

        if ((prFrcPrm->ucFbId != FBM_FB_ID_UNKNOWN) && (prFrcPrm->u2TargetNs != 0))
        {
            LOG(11, "%d %d %d Ns(%d) (%d, %d) (%d %d %d)\n",
                prFrcPrm->ucB2rId, prFrcPrm->ucFbgId, prFrcPrm->ucFbId,
                prFrcPrm->u2TargetNs, prFrcPrm->u4InFrameStc, prFrcPrm->u4OutFrameStc,
                prFrcPrm->ucRff, prFrcPrm->ucTff, prFrcPrm->i4TemporalRef);

            LOG_REL(_fgEnableVideoLog, 2, 9, "B2R(%d) NF (%d %d) Ns(%d, %d) B(%d) TFF(%d) FRC(%d) R(%d)\n",
                    prFrcPrm->ucB2rId,
                    prFrcPrm->ucFbId, prFrcPrm->i4TemporalRef,
                    prFrcPrm->u2TargetNs, prFrcPrm->u2ChangeFieldNs,
                    prFrcPrm->ucBottom, prFrcPrm->ucTff,
                    ucFrcMode, prFrcPrm->ucReleaseFbId);

#ifdef CC_VDP_FRC_DEBUG
            LOG(4, "NS(%d)TR(%d)FR(%d)T/RFF(%d,%d)PTS(0x%06x)DP(%d)STC(0x%06x)D(%d)\n",
                prFrcPrm->u2TargetNs,
                prFrcPrm->i4TemporalRef,
                prFrcPrm->ucInFrameRate,
                prFrcPrm->ucTff,
                prFrcPrm->ucRff,
                prFrcPrm->u4Pts,
                (prFrcPrm->u4Pts - prFrcPrm->u4PtsBackup),
                prFrcPrm->u4Stc,
                prFrcPrm->i4PtsStcDelta);

            prFrcPrm->u4PtsBackup = prFrcPrm->u4Pts;
#endif
        #ifdef TIME_MEASUREMENT
            _B2R_RT_TMS_Statistics(prFrcPrm);
        #endif
        }
    }
}

#ifdef DIVX_PLUS_CER
#ifdef   CC_B2R_CHAPTER_CHG
VOID _VDP_FrcSetChapterEnable(VDP_CHAPTER_T*    prChapter)
{
    x_memcpy(&_arVdpChapter, prChapter, sizeof(VDP_CHAPTER_T) );
    if (_arVdpChapter.fgFirstSetChapter)
    {
        fgFirstSetChapter = TRUE;
    }
    _arVdpChapter.fgIsSetEndPts = TRUE;
    _arVdpChapter.fgPending     = FALSE;
    LOG(3, "ChapterEnable: PTS[%lld, %lld] IsFirst(%d).\n", 
        _arVdpChapter.u8StartPts, _arVdpChapter.u8EndPts, _arVdpChapter.fgFirstSetChapter);
    return;
}

static VDP_CHAPTER_T* _VDP_FrcGetChapterObj(VOID)
{
    return &_arVdpChapter;
}
#endif
#endif

#ifdef CC_MAPLE_CUST_DRV
INT32 _B2R_Get_Cust_info(UCHAR              ucB2rId,
                              B2R_CUST_INFO_T*    pt_cust_info)
{
    INT32           i4Ret    = B2R_OK;
    B2R_PRM_T*      ptVdpPrm = NULL;
    
    do
    {
        if(ucB2rId >= B2R_NS || 
            !pt_cust_info)
        {
            i4Ret = B2R_INV_ARG;
            break;
        }
        
        ptVdpPrm = &_arVdpPrm[ucB2rId];
        if(!ptVdpPrm->pt_cust_info)
        {
            i4Ret = B2R_INTERNAL_ERR;
            break;
        }

        x_memcpy(pt_cust_info,ptVdpPrm->pt_cust_info,sizeof(B2R_CUST_INFO_T));
    }while(0);

    return i4Ret;
}

INT32 _B2R_GetAspInfo(UCHAR     ucB2rId,
                           UINT8*    pu1_asp_info)
{
    INT32           i4Ret     = B2R_OK;
    UINT8           u1AspInfo = 0;
    UCHAR           ucVdpId;
    UINT32          u4DAR     = 0;
    VDP_CONF_T*     prVdpConf = NULL;
    
    do
    {
        if(ucB2rId >= B2R_NS ||
            !pu1_asp_info)
        {
            i4Ret = B2R_INV_ARG;
            break;
        }

        ucVdpId = VDP_GetVdpId(ucB2rId);
        if (ucVdpId >= VDP_NS)
        {
            i4Ret = B2R_INTERNAL_ERR;
            break;
        }

        prVdpConf = _prVdpConf[ucVdpId];
        if(!prVdpConf)
        {
            i4Ret = B2R_INTERNAL_ERR;
            break;
        }
        u1AspInfo = (UINT8)prVdpConf->u4AspectRatio;
        if ((u1AspInfo != MPEG_ASPECT_RATIO_4_3) &&
         (u1AspInfo != MPEG_ASPECT_RATIO_16_9))
        {
            if (prVdpConf->u4AspectRatioHeight)
            {
                u4DAR = 100 * prVdpConf->u4AspectRatioWidth/prVdpConf->u4AspectRatioHeight;
            }
            else
            {
                u4DAR = 100*16/9;
            }
            if (((u4DAR *3) >> 2) < 110)
            {
                u1AspInfo = MPEG_ASPECT_RATIO_4_3;
            }
            else if (((u4DAR *9) >> 4) < 110)
            {
                u1AspInfo = MPEG_ASPECT_RATIO_16_9;
            }
            else
            {
                u1AspInfo = MPEG_ASPECT_RATIO_16_9;
            }
        }
    }while(0);

    *pu1_asp_info = u1AspInfo;
    
    return B2R_OK;
}

INT32 _B2R_Cust_info_cfg(VDP_CONF_T*   prVdpConf,
                              B2R_PRM_T*    ptVdpPrm)
{
    INT32   i4Ret    = B2R_OK;

    do
    {
        if(!prVdpConf || !ptVdpPrm)
        {
            i4Ret = B2R_INV_ARG;
            break;
        }
        
        if(ptVdpPrm->pt_cust_info)
        { 
            UCHAR                   ucEsId;
            FBM_PIC_HDR_T*          prHdr        = NULL;
            VDEC_ES_INFO_T*         prVdecEsInfo = NULL;
            FBM_CODEC_RESOLUTION_T  t_res;
            

            ucEsId = prVdpConf->ucInputPort[0];
            prVdecEsInfo = (ucEsId < MAX_ES_NS)? _VDEC_GetEsInfo(ucEsId) : NULL;
            if(prVdecEsInfo)
            {
                ptVdpPrm->pt_cust_info->eMMSrcType = prVdecEsInfo->eMMSrcType;
            }

            prHdr = FBM_GetFrameBufferPicHdr(ptVdpPrm->ucFbgId, ptVdpPrm->ucFbId);
            if(prHdr)
            {
                 ptVdpPrm->pt_cust_info->eColorID = prHdr->eColorSpace;
            }

            ptVdpPrm->pt_cust_info->eCodecInfo = 
                FBM_GetCodecInfo(ptVdpPrm->ucFbgId);

            if(FBM_GetCodecResolution(ptVdpPrm->ucFbgId,&t_res))
            {
                x_memcpy(&(ptVdpPrm->pt_cust_info->rResolution),
                    &t_res,sizeof(FBM_CODEC_RESOLUTION_T));
            }
        }
    }while(0);

    return i4Ret;
}
#endif

//-----------------------------------------------------------------------------
/** Brief of _VdpGetSequenceInfo.
 * Description : To update sequence header information to B2R and VDP structures.
 * Return value :
 *   TRUE - Get sequence info succeeded.
 *   FALSE - Get sequence info failed.
 */
//-----------------------------------------------------------------------------

BOOL VdpChkSequenceInfo(UCHAR ucVdpId, BOOL fgPreChk)
{
    //LOG(1, "[%s-%d].\n", __func__, __LINE__);
    if((ucVdpId >= VDP_NS) || (VDP2B2RID(ucVdpId) >= B2R_NS))
    {
        LOG(1, "[%s-%d]Fail\n", __func__, __LINE__);
        return FALSE;
    }

    return _VdpGetSequenceInfo(&(_arVdpPrm[VDP2B2RID(ucVdpId)]), TRUE);  
}

//-----------------------------------------------------------------------------
/** Brief of _VdpGetSequenceInfo.
 * Description : To update sequence header information to B2R and VDP structures.
 * Return value :
 *   TRUE - Get sequence info succeeded.
 *   FALSE - Get sequence info failed.
 */
//-----------------------------------------------------------------------------
static BOOL _VdpGetSequenceInfo(B2R_PRM_T* prFrcPrm, BOOL fgPreChk)
{
    BOOL fgFrmRateChg = FALSE;
    BOOL fgSeqChg = FALSE;
    BOOL fg3DChg = FALSE;
    BOOL fgResChg = FALSE;
    UCHAR ucVdpId;
    UCHAR ucB2rId;
    UINT32 u4AddrMode;
    UINT32 u4SeqWidth;
    UINT32 u4SeqHeight;
    UINT32 u4SrcLineSize = 0;
    FBM_SEQ_HDR_T* prSeqHdr;
#ifdef ENABLE_MULTIMEDIA
    UCHAR ucEsId;
    VDEC_ES_INFO_T* prVdecEsInfo = NULL;
#endif

    if (prFrcPrm == NULL)
    {
        return FALSE;
    }
    //prFrcPrm->ucSeqInfoValid = 1;
    ucB2rId = prFrcPrm->ucB2rId;
    if(ucB2rId >= B2R_NS)
    {
        return FALSE;
    }
    ucVdpId = VDP_GetVdpId(ucB2rId);
    if(ucVdpId >= VDP_NS)
    {
        return FALSE;
    }
    // Prepare Input Frame Rate
    prSeqHdr = FBM_GetFrameBufferSeqHdr(prFrcPrm->ucFbgId);
    if (prSeqHdr == NULL)
    {
        LINT_SUPPRESS_BRACE(527)
        ASSERT(0);
        return FALSE;
    }

    //1 //Check change first, never save the param to vdp_conf or b2r_prm_t
    {
        if (prSeqHdr->ucFrmRatCod == MPEG_FRAME_RATE_UNKNOWN)
        {
            prSeqHdr->ucFrmRatCod = FBM_CalFrmRate(prFrcPrm->ucFbgId);
        }
        if ((prSeqHdr->ucFrmRatCod != MPEG_FRAME_RATE_UNKNOWN) && 
            (_prVdpConf[ucVdpId]->rOutInfo.ucInputFrameRate != prSeqHdr->ucFrmRatCod))
        {
            fgFrmRateChg = TRUE;
        }

#ifdef ENABLE_MULTIMEDIA
        ucEsId = _prVdpConf[ucVdpId]->ucInputPort[0];
        prVdecEsInfo = (ucEsId < MAX_ES_NS)? _VDEC_GetEsInfo(ucEsId) : NULL;

        u4SrcLineSize = prSeqHdr->u2LineSize;

        if (prVdecEsInfo && (prVdecEsInfo->eSeamlessMode & SEAMLESS_BY_NPTV))
        {
            u4SeqWidth = (UINT32) 1920;//prSeqHdr->u4ResizeWidth;
            u4SeqHeight = (UINT32)1080;// prSeqHdr->u4ResizeHeight;
            //u4SrcLineSize = 1920;
        }
        else
        if (prVdecEsInfo && prVdecEsInfo->fgSeamlessPlay) // seamless play
        {
            u4SeqWidth = (UINT32) prSeqHdr->u4ResizeWidth;
            u4SeqHeight = (UINT32) prSeqHdr->u4ResizeHeight;
        }
        else 
#endif
        if (prSeqHdr->fgCropping )
        {
            // If cropping info is provided, the correct video size is identified by cropping size
            // ** For seamless mode, cropping is handled by seamless job, so don't care here
            u4SeqWidth = prSeqHdr->u4CropWidth;
            u4SeqHeight = prSeqHdr->u4CropHeight;
        }
        else
        {
            u4SeqWidth = (UINT32) prSeqHdr->u2HSize;
            u4SeqHeight = (UINT32) prSeqHdr->u2VSize;
        }

        // because of the 420 sample method
        // progressive : the height should be a multiple of 2
        // interlace : the height should be a multiple of 4
        if (prSeqHdr->fgProgressiveSeq != 0)
        {
            u4SeqHeight &= 0xFFFFFFFE;
        }
        else
        {
            u4SeqHeight &= 0xFFFFFFFC;
        }


#ifdef CC_B2R_3D_SUPPROT
        if (!prSeqHdr->fgB2R3DEnable)
#endif
        {
            u4SeqWidth = ((u4SeqWidth > B2R_MAX_WIDTH) ? B2R_MAX_WIDTH : u4SeqWidth);
            u4SeqHeight = ((u4SeqHeight > B2R_MAX_HEIGHT) ? B2R_MAX_HEIGHT : u4SeqHeight);
        }

        if ((_prVdpConf[ucVdpId]->u4SrcLineSize != (UINT32) u4SrcLineSize) ||
            (_au4OriSrcWidth[ucVdpId]  != u4SeqWidth) ||
            (_au4OriSrcHeight[ucVdpId] != u4SeqHeight) ||
            (_prVdpConf[ucVdpId]->ucNoOverScan!= (UCHAR)prSeqHdr->fgNoOverScan))
        {
            fgSeqChg = TRUE;
        }

        if ((_u4LastWidth != (UINT32)prSeqHdr->u2OrgWidth) || (_u4LastHeight = (UINT32)prSeqHdr->u2OrgHeight))
        {
            fgResChg = TRUE;
            _u4LastWidth = (UINT32)prSeqHdr->u2OrgWidth;
            _u4LastHeight = (UINT32)prSeqHdr->u2OrgHeight;
        }

#ifdef CC_B2R_3D_SUPPROT
        if (_arVdpPrm[ucB2rId].fgB2R3DEnable != prSeqHdr->fgB2R3DEnable)   // Check 3D enable/disable changed
        {
            fg3DChg = TRUE;
        }
        else if (_arVdpPrm[ucB2rId].fgB2R3DEnable == TRUE)                 // If 3D is enabled, check 3D configuration changed
        {
            if ((_arVdpPrm[ucB2rId].u1B2R3DType != prSeqHdr->u1B2R3DType) ||
                    (_arVdpPrm[ucB2rId].fgB2RForce2D != prSeqHdr->fgB2RForce2D) ||
                    (_arVdpPrm[ucB2rId].u1B2RForce2DType != prSeqHdr->u1B2RForce2DType) ||
                    (_arVdpPrm[ucB2rId].u4B2R3DWidthOffset != prSeqHdr->u4B2R3DWidthOffset) ||
                    (_arVdpPrm[ucB2rId].u4B2R3DHeightOffset != prSeqHdr->u4B2R3DHeightOffset) /* ||
                    (_arVdpPrm[ucB2rId].u1B2R3DTrickModeType != prSeqHdr->u1B2R3DTrickModeType)*/)
            {
                fg3DChg = TRUE;
            }
        }
        
        if (prSeqHdr->fgB2R3DEnable)
        {
            if (prSeqHdr->u1B2R3DType == B2R_3D_SIDE_BY_SIDE_REAL_D || prSeqHdr->u1B2R3DType == B2R_3D_SIDE_BY_SIDE)
            {
                u4SeqWidth/=2;
            }
            else if (prSeqHdr->u1B2R3DType == B2R_3D_TOP_N_BOTTOM)
            {
                u4SeqHeight/=2;
            }
        }
#endif // CC_B2R_3D_SUPPROT
    }

    if (fgPreChk)
    {
        //Only pre-check the seq info chg or not. .
        //For VDP seamless pre check only the w,h change or not.
        if (fgResChg && !fg3DChg && !fgFrmRateChg)
        {
#ifdef CC_SUPPORT_NPTV_SEAMLESS 
            _VDP_NPTVSeamlessStatus(prFrcPrm, VDP_SMLS_PREPARE);
#endif
            LOG(1, "[%s-%d]H,V(%d-%d).Line(%d->%d) Org(%d-%d->%d-%d).\n", __func__, __LINE__, 
                prSeqHdr->u2HSize, prSeqHdr->u2VSize,
                _prVdpConf[ucVdpId]->u4SrcLineSize, (UINT32) prSeqHdr->u2LineSize,
                _u4LastWidth, _u4LastHeight, prSeqHdr->u2OrgHSize, prSeqHdr->u2OrgVSize);
                        
            return TRUE;
        }
        return FALSE;
    }
    
    _VdpHandleResolutionChange(prFrcPrm);
    if (prFrcPrm->ucSeamlessVDPMode)
    {
        
        _prVdpConf[ucVdpId]->u4SrcLineSize = prSeqHdr->u2LineSize;
        LOG(1, "Seq2Chg %d H,V(%d %d) Fr(%d) Asp(%d %d %d) 3D(%d).\n",
            prSeqHdr->u2LineSize, u4SeqWidth, u4SeqHeight, prFrcPrm->ucInFrameRate,
            prSeqHdr->ucAspRatInf, prSeqHdr->u4AspectRatioWidth, prSeqHdr->u4AspectRatioHeight, prSeqHdr->u1B2R3DType);
        
#ifdef CC_SUPPORT_NPTV_SEAMLESS 
        _VDP_NPTVSeamlessStatus(prFrcPrm, VDP_SMLS_FRAME_COMING);
#endif
        FBM_ClrFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_SEQ_CHG);
        FBM_ClrFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_3D_SEQ_CHG);
        FBM_ClrFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_SEQ_CHG_SPEEDUP);
        return TRUE;
    }

    _fgInvalidFrmRate[ucB2rId] = (prSeqHdr->ucFrmRatCod == MPEG_FRAME_RATE_UNKNOWN) ? TRUE : FALSE;
#ifdef ENABLE_MULTIMEDIA
    if(_fgInvalidFrmRate[ucB2rId] &&
        prVdecEsInfo &&
        prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_WIFI_DISPLAY)
    {
        prSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30;//no frame rate ,default set 30fps
    }

    if(prVdecEsInfo &&
        prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_WIFI_DISPLAY)
    {
        if(!prFrcPrm->t_wfd_prm.fgInitalized)
        {
            _VDP_RTFrameRateConvert(prFrcPrm, prSeqHdr->ucFrmRatCod);
        }
    }
    else
#endif
    {
        _VDPConvertFrameRateInx2FrameRate(prFrcPrm, prSeqHdr->ucFrmRatCod);
    }
    
    if (prSeqHdr->ucFrmRatCod != MPEG_FRAME_RATE_UNKNOWN)
    {
        _prVdpConf[ucVdpId]->rOutInfo.ucInputFrameRate = prSeqHdr->ucFrmRatCod;
        if (fgFrmRateChg)
        {
            _u4ReturnValue |= VDP_EVENT_SEQ_CHG;
        }
    }
    
    if (fgSeqChg || fg3DChg)
    {
        // Update Sequence Parameter to hardware
        // do not update everything, since display mode will flash
        _u4ReturnValue |= VDP_EVENT_SEQ_CHG;
    }
    else
    {
        // Optional Sequence Change
        _u4ReturnValue |= VDP_EVENT_OPT_SEQ_CHG;
    }

#ifdef CC_B2R_3D_SUPPROT
#ifdef CC_SUPPORT_FULL_3D_PHOTO
    _arVdpPrm[ucVdpId].fg4K2K = FALSE;
    if((prSeqHdr->fg4K1K) && ((prSeqHdr->u2HSize==3840)&&(prSeqHdr->u2VSize==1080)))
    {
        u4SeqWidth /= 2;
        _arVdpPrm[ucVdpId].fg4K2K=TRUE;
        _arVdpPrm[ucVdpId].u4B2R3DWidthOffset = u4SeqWidth;
        prSeqHdr->fgB2R3DEnable = TRUE;
        prSeqHdr->fgB2RForce2D = FALSE;
    }
#endif

    _arVdpPrm[ucB2rId].fgB2RForce2D = prSeqHdr->fgB2RForce2D;
    _arVdpPrm[ucB2rId].u1B2RForce2DType = prSeqHdr->u1B2RForce2DType;
    _arVdpPrm[ucB2rId].fgB2R3DEnable = prSeqHdr->fgB2R3DEnable;
    _arVdpPrm[ucB2rId].u1B2R3DType = prSeqHdr->u1B2R3DType;
    _arVdpPrm[ucB2rId].u4B2R3DWidthOffset = prSeqHdr->u4B2R3DWidthOffset;
    _arVdpPrm[ucB2rId].u4B2R3DHeightOffset = prSeqHdr->u4B2R3DHeightOffset;
    _arVdpPrm[ucB2rId].u1B2R3DTrickModeType = prSeqHdr->u1B2R3DTrickModeType;
#endif

    // General case
    // fps    : STC Period
    // 24    : 3750
    // 25    : 3600
    // 30    : 3000
    // 50    : 1800
    // 60    : 1500
    prFrcPrm->u4InStcPeriodBase = VDP_STC_CLOCK / prFrcPrm->ucInFrameRate;
    prFrcPrm->ucProgressiveSeqFlag = prSeqHdr->fgProgressiveSeq;
    prFrcPrm->ucSeqInfoValid = 1;

    // Update Sequence Parameter to Vdp_If    
    _prVdpConf[ucVdpId]->u4MpegHeight = prSeqHdr->u2OrgVSize;
    _prVdpConf[ucVdpId]->u4MpegWidth = prSeqHdr->u2OrgHSize;
    _prVdpConf[ucVdpId]->u4SrcLineSize = u4SrcLineSize;//prSeqHdr->u2LineSize;
    _prVdpConf[ucVdpId]->u4SrcWidth = u4SeqWidth;
    _prVdpConf[ucVdpId]->u4SrcHeight = u4SeqHeight;
    _prVdpConf[ucVdpId]->u4FrameRate = prFrcPrm->ucInFrameRate;
    _prVdpConf[ucVdpId]->ucColourPrimary = prSeqHdr->ucColourPrimaries;
    _prVdpConf[ucVdpId]->ucTransCharacter = prSeqHdr->ucTransCharacter;
    _prVdpConf[ucVdpId]->ucMatrixCoeff = prSeqHdr->ucMatrixCoeff;
    _prVdpConf[ucVdpId]->ucColorDescrip = prSeqHdr->fgColorDescrip;
    _prVdpConf[ucVdpId]->ucNoOverScan = prSeqHdr->fgNoOverScan;
    _prVdpConf[ucVdpId]->u4FrmCropTop = prSeqHdr->u4FrmCropTop;
    _prVdpConf[ucVdpId]->u4FrmCropBtm = (u4SeqHeight - prSeqHdr->u4FrmCropBtm);
    _prVdpConf[ucVdpId]->fgCropping = prSeqHdr->fgCropping;
    _prVdpConf[ucVdpId]->u4CropX = prSeqHdr->u4CropX;
    _prVdpConf[ucVdpId]->u4CropY = prSeqHdr->u4CropY;
    _prVdpConf[ucVdpId]->u4CropWidth = prSeqHdr->u4CropWidth;
    _prVdpConf[ucVdpId]->u4CropHeight = prSeqHdr->u4CropHeight;

#if 1 // def CC_MT5360            
    if ((_prVdpConf[ucVdpId]->u4SrcHeight == 1088) &&
            ((_prVdpConf[ucVdpId]->u4FrmCropBtm - _prVdpConf[ucVdpId]->u4FrmCropTop) == 1080))
    {
        vDrvDISetDtvClip(ucVdpId, _prVdpConf[ucVdpId]->u4FrmCropTop);
    }
#endif

    if ((_prVdpConf[ucVdpId]->u4Afd != (UINT32) prSeqHdr->ucActFmt) ||
            (_prVdpConf[ucVdpId]->u4AspectRatio != (UINT32) prSeqHdr->ucAspRatInf) ||
            ((_prVdpConf[ucVdpId]->u4AspectRatio == MPEG_ASPECT_RATIO_TRANSMIT) &&
             ((_prVdpConf[ucVdpId]->u4AspectRatioWidth != prSeqHdr->u4AspectRatioWidth) ||
              (_prVdpConf[ucVdpId]->u4AspectRatioHeight != prSeqHdr->u4AspectRatioHeight))) ||
              (_prVdpConf[ucVdpId]->u4AspectRatioIdc != prSeqHdr->u4AspectRatioIdc))
    {
        _prVdpConf[ucVdpId]->u4Afd = (UINT32) prSeqHdr->ucActFmt;
        _prVdpConf[ucVdpId]->u4AspectRatio = (UINT32) prSeqHdr->ucAspRatInf;
        _prVdpConf[ucVdpId]->u4AspectRatioWidth = prSeqHdr->u4AspectRatioWidth;
        _prVdpConf[ucVdpId]->u4AspectRatioHeight = prSeqHdr->u4AspectRatioHeight;
        _prVdpConf[ucVdpId]->u4AspectRatioIdc = prSeqHdr->u4AspectRatioIdc;
        _rFbmAspectRatio.u1AspectRatioType = prSeqHdr->ucAspRatInf;
        _rFbmAspectRatio.u2AspectHorizSize = prSeqHdr->u4AspectRatioWidth;
        _rFbmAspectRatio.u2AspectVertSize = prSeqHdr->u4AspectRatioHeight;

        if (prFrcPrm->ucReady ==1)
        {
            LOG(1, "Aspect Chg %d \n",_prVdpConf[ucVdpId]->u4AspectRatio);
            _VDP_VsyncNotify(VDP_MSG_AFD_CB, (UINT32)ucVdpId, (UINT32) prSeqHdr->ucActFmt, (UINT32) (&_rFbmAspectRatio));
        }
    }
#define BURST_READ_THRESHOLD 128
    if (prSeqHdr->fgResizeSmallPic && (u4SeqWidth < BURST_READ_THRESHOLD && u4SeqHeight < BURST_READ_THRESHOLD))
    {
        VDP_EnableBurstRead(ucB2rId, FALSE);
    }
    _au4OriSrcWidth[ucVdpId] = _prVdpConf[ucVdpId]->u4SrcWidth;
    _au4OriSrcHeight[ucVdpId] = _prVdpConf[ucVdpId]->u4SrcHeight;
    _VdpCheckOutputMode(prFrcPrm);

    if (FBM_ChkFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_SEQ_CHG_SPEEDUP))
    {
        LOG(1, "Sequence header change (speedup) \n");
        prFrcPrm->fgSeqChgSpeedup = TRUE;
    }
    else
    {
        prFrcPrm->fgSeqChgSpeedup = FALSE;
    }

    FBM_ClrFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_SEQ_CHG);
    FBM_ClrFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_3D_SEQ_CHG);
    FBM_ClrFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_SEQ_CHG_SPEEDUP);

    if (prFrcPrm->ucReady == 0)
    {
        prFrcPrm->ucHwFrameEncode = prFrcPrm->ucProgressiveFrame;
        _VDP_HalSetChromaMode(ucB2rId, prFrcPrm->ucHwFrameEncode, prFrcPrm->ucTff);
    }

    LOG(1, "SeqChg %d H,V(%d %d) Fr(%d) Asp(%d %d %d) 3D(%d).\n",
        prSeqHdr->u2LineSize, u4SeqWidth, u4SeqHeight, prFrcPrm->ucInFrameRate,
        prSeqHdr->ucAspRatInf, prSeqHdr->u4AspectRatioWidth, prSeqHdr->u4AspectRatioHeight, prSeqHdr->u1B2R3DType);

    // Force update frame encode
    prFrcPrm->ucHwFrameEncode = 255;

    // set raster mode or block mode for B2R hardware
    u4AddrMode = prSeqHdr->fgRasterOrder ? ADDR_MODE_RASTER : (prSeqHdr->ucBlockType ? ADDR_MODE_16_32 : ADDR_MODE_64_32);
    _VDP_HalSetFormat(ucB2rId,prSeqHdr->fg422Mode,u4AddrMode);

    if (prFrcPrm->fgSeqChgSpeedup)
    {
#ifdef TIME_MEASUREMENT
        TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME_DRV, "B2R Set Start Play(S)");
#endif
        _VDP_VsyncNotify(VDP_MSG_NOTIFY, (UINT32)ucVdpId, VDP_B2R_START_PLAY, 0);
        _fgModeChangeSent = TRUE;
    }

    return TRUE;
}

#ifdef CC_3D_MM_DS_SUPPORT
void _VdpGetFrameBufferForMVC3D(B2R_PRM_T* prFrcPrm)
{
    FBM_PIC_HDR_T* prLeftPicHdr = NULL;
    FBM_PIC_HDR_T* prRightPicHdr = NULL;
    BOOL fgEos = FALSE;

    // pick L/R frame and check L/R sync behavior
    if (FBM_CheckFrameBufferDispQ(prFrcPrm->ucFbgId) && FBM_CheckFrameBufferDispQ(prFrcPrm->ucSubFbgId))
    {
        UINT8 u1LeftFbId = FBM_PreLookFrameBufferFromDispQ(prFrcPrm->ucFbgId);
        UINT8 u1RightFbId = FBM_PreLookFrameBufferFromDispQ(prFrcPrm->ucSubFbgId);

        prRightPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucSubFbgId, u1RightFbId);
        prLeftPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, u1LeftFbId);

        if (prRightPicHdr != NULL && prLeftPicHdr != NULL)
        {
            if ((prRightPicHdr->u4PTS > prLeftPicHdr->u4PTS))
            {
                while ((prRightPicHdr->u4PTS > prLeftPicHdr->u4PTS))
                {
                    if (((prRightPicHdr->u4PTS - prLeftPicHdr->u4PTS) > 500) && ((prRightPicHdr->u4PTS - prLeftPicHdr->u4PTS) < 180000))
                    {
                        LOG_REL(_fgEnableVideoLog, 1, 3, "LRSync prelook: Drop L frame pts(%d) TempRef(%d) \n",prLeftPicHdr->u4PTS,prLeftPicHdr->i4TemporalRef);
                        _VdpFreeFrameBeforeEosHandler(prFrcPrm, TRUE, FALSE, &fgEos);
                        if (fgEos)
                        {
                            break;
                        }

                        if (FBM_CheckFrameBufferDispQ(prFrcPrm->ucFbgId))
                        {
                            u1LeftFbId = FBM_PreLookFrameBufferFromDispQ(prFrcPrm->ucFbgId);
                            prLeftPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, u1LeftFbId);
                            if (prLeftPicHdr == NULL)
                            {
                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                    else if ((prRightPicHdr->u4PTS - prLeftPicHdr->u4PTS) >= 180000)
                    {
                        if (FBM_CheckFrameBufferDispQ(prFrcPrm->ucFbgId) > 1)
                        {
                            LOG_REL(_fgEnableVideoLog, 1, 3, "LRSync prelook: Drop L frame pts(%d) TempRef(%d) \n",prLeftPicHdr->u4PTS,prLeftPicHdr->i4TemporalRef);
                            _VdpFreeFrameBeforeEosHandler(prFrcPrm, TRUE, FALSE, &fgEos);
                        }
                        break;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else
            {
                while ((prLeftPicHdr->u4PTS > prRightPicHdr->u4PTS))
                {
                    if (((prLeftPicHdr->u4PTS - prRightPicHdr->u4PTS) > 500) && ((prLeftPicHdr->u4PTS - prRightPicHdr->u4PTS) < 180000))
                    {
                        LOG_REL(_fgEnableVideoLog, 1, 3, "LRSync prelook: Drop R frame pts(%d) TempRef(%d) \n",prRightPicHdr->u4PTS,prRightPicHdr->i4TemporalRef);
                        _VdpFreeFrameBeforeEosHandler(prFrcPrm, FALSE, FALSE, &fgEos);
                        if (fgEos)
                        {
                            break;
                        }

                        if (FBM_CheckFrameBufferDispQ(prFrcPrm->ucSubFbgId))
                        {
                            u1RightFbId = FBM_PreLookFrameBufferFromDispQ(prFrcPrm->ucSubFbgId);
                            prRightPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucSubFbgId, u1RightFbId);
                            if (prRightPicHdr == NULL)
                            {
                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                    else if ((prLeftPicHdr->u4PTS - prRightPicHdr->u4PTS) >= 180000)
                    {
                        if (FBM_CheckFrameBufferDispQ(prFrcPrm->ucSubFbgId) > 1)
                        {
                            LOG_REL(_fgEnableVideoLog, 1, 3, "LRSync prelook: Drop R frame pts(%d) TempRef(%d) \n",prRightPicHdr->u4PTS,prRightPicHdr->i4TemporalRef);
                            _VdpFreeFrameBeforeEosHandler(prFrcPrm, FALSE, FALSE, &fgEos);
                        }
                        break;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }

    if (!fgEos)
    {
        if (FBM_CheckFrameBufferDispQ(prFrcPrm->ucFbgId) && FBM_CheckFrameBufferDispQ(prFrcPrm->ucSubFbgId))
        {
            // L/R view are both exist => check their PTS
            UCHAR ucLookFbIdL = FBM_FB_ID_UNKNOWN;
            UCHAR ucLookFbIdR = FBM_FB_ID_UNKNOWN;
            FBM_PIC_HDR_T* prLookPicHdrL = NULL;
            FBM_PIC_HDR_T* prLookPicHdrR = NULL;

            ucLookFbIdL = FBM_PreLookFrameBufferFromDispQ(prFrcPrm->ucFbgId);
            prLookPicHdrL = (ucLookFbIdL == FBM_FB_ID_UNKNOWN)? NULL : FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, ucLookFbIdL);

            ucLookFbIdR = FBM_PreLookFrameBufferFromDispQ(prFrcPrm->ucSubFbgId);
            prLookPicHdrR = (ucLookFbIdR == FBM_FB_ID_UNKNOWN)? NULL : FBM_GetFrameBufferPicHdr(prFrcPrm->ucSubFbgId, ucLookFbIdR);

            if (prLookPicHdrL != NULL &&  prLookPicHdrR != NULL)
            {
                UINT32 u4LRDelta = (prLookPicHdrL->u4PTS > prLookPicHdrR->u4PTS)?
                                   (prLookPicHdrL->u4PTS - prLookPicHdrR->u4PTS) : (prLookPicHdrR->u4PTS - prLookPicHdrL->u4PTS);
                if (u4LRDelta < 500 || u4LRDelta > 180000)
                {
                    // LR is sync, or free run => display it
                    if (!FBM_IsSeamlessFB(prFrcPrm->ucSubFbgId, ucLookFbIdR) || prLookPicHdrR->ucSeamlessResizeFbid != FBM_FB_ID_UNKNOWN)
                    {
                        prFrcPrm->ucFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
                        prFrcPrm->ucSubFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucSubFbgId);
                    }
                    else
                    {
                        prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
                        LOG_REL(_fgEnableVideoLog, 1, 3, "LRSync L/R presync but rz not started\n");
                    }
                }
                else
                {
                    // LR is not sync => ignore this time
                    LOG_REL(_fgEnableVideoLog, 1, 3, "LRSync Ignore display because LR PTS not sync: L(%d), R(%d), Delta(%d)\n",
                            prLookPicHdrL->u4PTS, prLookPicHdrR->u4PTS, u4LRDelta);
                    prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
                }
            }
            else
            {
                // should not be here...
                prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
            }
        }
        else
        {
            // one view is not exist => ignore this time
            LOG_REL(_fgEnableVideoLog, 1, 3, "LRSync Ignore display because LR one view not exist: L(%d), R(%d)\n",
                    FBM_CheckFrameBufferDispQ(prFrcPrm->ucFbgId), FBM_CheckFrameBufferDispQ(prFrcPrm->ucSubFbgId));
            prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
        }
    }
}
#endif

void _VdpGetFrameBufferForOMX(B2R_PRM_T* prFrcPrm, BOOL* pfgGstPlayBack, UCHAR ucB2rId)
{
    //UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    VDP_B2R_CHG_FRAME_MSG_T rMsg;
    BOOL fgEos = FALSE;
    *pfgGstPlayBack = TRUE;

    zMsgSize = sizeof(VDP_B2R_CHG_FRAME_MSG_T);
    x_memset(&rMsg, 0, zMsgSize);

    if (!_afgPendingChgFrmMsg[ucB2rId] &&
            //x_msg_q_receive(&u2MsgQIdx, &rMsg, &zMsgSize,&(_ahChgFrameQueue[prFrcPrm->ucVdpId]), 1, X_MSGQ_OPTION_NOWAIT) != OSR_OK
            (_VDP_ReceiveChgFrameMsg(ucB2rId, &rMsg, X_MSGQ_OPTION_NOWAIT) != OSR_OK))
    {
        prFrcPrm->ucFbId = 0xFF;
    }
    else
    {
        if (_afgPendingChgFrmMsg[ucB2rId])
        {
            x_memcpy(&rMsg, &(_arPendingChgFrmMsg[ucB2rId]), zMsgSize);
            _afgPendingChgFrmMsg[ucB2rId] = FALSE;
        }

        if ( (ucB2rId < B2R_NS) && (_eSpeed[ucB2rId] != STC_SPEED_TYPE_FORWARD_1X))
        {
            static UINT32 _u4Counter[B2R_NS] = {0, 0};
            if (_eSpeed[ucB2rId] == STC_SPEED_TYPE_FORWARD_2X)
            {
                /*if((FBM_CheckFrameBufferDispQ(prFrcPrm->ucFbgId) >= 2))
                                     {
                                         prFrcPrm->ucFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
                                         FBM_SetFrameBufferStatus(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_FB_STATUS_LOCK);
                                         FBM_SetFrameBufferStatus(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_FB_STATUS_EMPTY);
                                         prFrcPrm->ucFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
                                     }
                                     else
                                     {
                                         prFrcPrm->ucFbId = 0xFF;
                                     }*/
                if (_u4Counter[ucB2rId] % 2 ==0)
                {
                    UCHAR ucFbId = _VdpFreeFrameBeforeEosHandler(prFrcPrm, TRUE, _VdpChangeFrameDirectForOMX(prFrcPrm, &rMsg), &fgEos);
                    if (FBM_FB_ID_UNKNOWN != ucFbId)
                    {
                        _u4Counter[ucB2rId]++;
                    }
                }
                else
                {
                    if (!_VdpChangeFrameDirectForOMX(prFrcPrm, &rMsg))
                    {
                        prFrcPrm->ucFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
                    }
                    if (prFrcPrm->ucFbId != 0xff)
                    {
                        _u4Counter[ucB2rId]++;
                    }
                }
            }
            else //1.5x case
            {
                if (_u4Counter[ucB2rId] % 3 ==0)
                {
                    UCHAR ucFbId = _VdpFreeFrameBeforeEosHandler(prFrcPrm, TRUE, _VdpChangeFrameDirectForOMX(prFrcPrm, &rMsg), &fgEos);
                    if (FBM_FB_ID_UNKNOWN != ucFbId)
                    {
                        _u4Counter[ucB2rId]++;
                    }
                }
                else
                {
                    if (!_VdpChangeFrameDirectForOMX(prFrcPrm, &rMsg))
                    {
                        prFrcPrm->ucFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
                    }
                    if (prFrcPrm->ucFbId != 0xff)
                    {
                        _u4Counter[ucB2rId]++;
                    }
                }
            }
        }
        else
        {
            BOOL fgFound = TRUE;
            FBM_PIC_HDR_T* prPicHdr = NULL;

            if (VDP_B2R_MULTI_VIEW & rMsg.u4Flag)
            {
                UINT8 ucLId;
#ifdef CC_3D_MM_DS_SUPPORT
                UINT8 ucRId;
#endif

                ucLId = FBM_PreLookFrameBufferFromDispQ(prFrcPrm->ucFbgId);

#ifdef CC_3D_MM_DS_SUPPORT
                ucRId = FBM_PreLookFrameBufferFromDispQ(prFrcPrm->ucSubFbgId);
#endif

                prFrcPrm->ucFbId = 0xFF;

                //check if it need resize
                if (ucLId != FBM_FB_ID_UNKNOWN)
                {
                    prPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, ucLId);
                    if (!prPicHdr || (prPicHdr->fgSeamlessDisp && prPicHdr->ucSeamlessResizeFbid == FBM_FB_ID_UNKNOWN))
                    {
                        ucLId = FBM_FB_ID_UNKNOWN;
                    }
                }

#ifdef CC_3D_MM_DS_SUPPORT

                if (ucRId != FBM_FB_ID_UNKNOWN)
                {
                    prPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucSubFbgId, ucRId);
                    if (!prPicHdr || (prPicHdr->fgSeamlessDisp && prPicHdr->ucSeamlessResizeFbid == FBM_FB_ID_UNKNOWN))
                    {
                        ucRId = FBM_FB_ID_UNKNOWN;
                    }
                }


                if (ucLId != FBM_FB_ID_UNKNOWN &&
                        ucRId != FBM_FB_ID_UNKNOWN)
                {
                    FBM_PIC_HDR_T* prLeftPicHdr = NULL;
                    FBM_PIC_HDR_T* prRightPicHdr = NULL;
                    prFrcPrm->ucFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
                    prFrcPrm->ucSubFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucSubFbgId);
                    prRightPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucSubFbgId, prFrcPrm->ucSubFbId);
                    prLeftPicHdr  = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, prFrcPrm->ucFbId);
                    LOG(2, "[OMX] 3D fbid (%d, %d)(%d %d)\n", prFrcPrm->ucFbId, prFrcPrm->ucSubFbId, (prLeftPicHdr?prLeftPicHdr->i4TemporalRef:0xFFFFFFFF), (prRightPicHdr?prRightPicHdr->i4TemporalRef:0xFFFFFFFF));
                }
                else
                {
                    fgFound = FALSE;
                    LOG(2, "[OMX] 3D fbid (%d, %d)(%d %d)\n", ucLId, ucRId);
                }

#endif

            }
            else
            {
                if (!_VdpChangeFrameDirectForOMX(prFrcPrm, &rMsg))
                {
                    prFrcPrm->ucFbId = FBM_PreLookFrameBufferFromDispQ(prFrcPrm->ucFbgId);
                    if (prFrcPrm->ucFbId != FBM_FB_ID_UNKNOWN)
                    {
                        prPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, prFrcPrm->ucFbId);
                        if (!prPicHdr || (prPicHdr->fgSeamlessDisp && prPicHdr->ucSeamlessResizeFbid == FBM_FB_ID_UNKNOWN))
                        {
                            prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
                            fgFound = FALSE;
                        }
                    }

                    if (prFrcPrm->ucFbId != 0xFF)
                    {
                        FBM_PIC_HDR_T* prPicHdr = NULL;
                        prFrcPrm->ucFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
                        prPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, prFrcPrm->ucFbId);
                        LOG(2, "[OMX] fbid (%d, %d)\n", prFrcPrm->ucFbId, (prPicHdr?prPicHdr->i4TemporalRef:0));
                    }
                } 
                else
                {
                    if (prFrcPrm->ucFbId != FBM_FB_ID_UNKNOWN)
                    {
                        prPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, prFrcPrm->ucFbId);
                        if (!prPicHdr || (prPicHdr->fgSeamlessDisp && prPicHdr->ucSeamlessResizeFbid == FBM_FB_ID_UNKNOWN))
                        {
                            prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
                            fgFound = FALSE;
                            //LOG(2, "[OMX] FB(%d) Resize not ready.\n", prFrcPrm->ucFbId);
                        }
                    }
                }
            }                


            if (!fgFound)
            {
                prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
                x_memcpy(&(_arPendingChgFrmMsg[ucB2rId]), &rMsg, zMsgSize);
                _afgPendingChgFrmMsg[ucB2rId] = TRUE;
            }

        }
    }
}

void _VdpGetFrameBufferForSkype(B2R_PRM_T* prFrcPrm)
{
    BOOL fgEos = FALSE;
    // For skype case, always display latest frame (drop old frames)
    UCHAR ucDropFbId = FBM_FB_ID_UNKNOWN;
    while (FBM_CheckFrameBufferDispQ(prFrcPrm->ucFbgId) > 1)
    {
        ucDropFbId = _VdpFreeFrameBeforeEosHandler(prFrcPrm, TRUE, FALSE, &fgEos);
        LOG(6, "[SKYPE] DispQ(%d), drop one fb = %d\n", FBM_CheckFrameBufferDispQ(prFrcPrm->ucFbgId), ucDropFbId);
        if (ucDropFbId == FBM_FB_ID_UNKNOWN)
        {
            break;
        }
    }
    if (!fgEos)
    {
        prFrcPrm->ucFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
    }
}

void _VdpGetFrameBufferForGeneral(B2R_PRM_T* prFrcPrm, BOOL* pfgGstPlayBack)
{
    if (_fgDropFrame && (FBM_CheckFrameBufferDispQ(prFrcPrm->ucFbgId) >=2))
    {
        BOOL fgEos = FALSE;
        _VdpFreeFrameBeforeEosHandler(prFrcPrm, TRUE, FALSE, &fgEos);
        if (!fgEos)
        {
            prFrcPrm->ucFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
            _fgDropFrame = FALSE;
            LOG(1,"audio driver notfiy vdp to drop one frame for av sync\n");
        }
    }
    else if (_fgRepeatFrame)
    {
        UINT32 u4OutFrameStc;
        UNUSED(u4OutFrameStc);
        prFrcPrm->ucFbId = 0xFF;
        _fgRepeatFrame = FALSE;
        *pfgGstPlayBack = TRUE;

        if (prFrcPrm->ucProgressiveSeqFlag)
        {
            prFrcPrm->i4PtsDrift += prFrcPrm->u4OutStcPeriod;
        }
        else
        {
            prFrcPrm->i4PtsDrift += 2*prFrcPrm->u4OutStcPeriod;
        }
        LOG(1,"audio driver notfiy vdp to repeat one frame for av sync\n");
    }
    else if (_fgTimeShift[0] && !prFrcPrm->fgDoubleClock && (_i4TimeShiftSpeed[0] == 200))
    {
        BOOL fgEos = FALSE;
        // When timeshift + FF 2x, and double clock is not enabled, drop half of frames.
        if (FBM_CheckFrameBufferDispQ(prFrcPrm->ucFbgId) >= 2)
        {
            _VdpFreeFrameBeforeEosHandler(prFrcPrm, TRUE, FALSE, &fgEos);
        }
        if (!fgEos)
        {
            prFrcPrm->ucFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
        }
    }
    else
    {
        UCHAR ucLookFbId = FBM_FB_ID_UNKNOWN;
        FBM_PIC_HDR_T* prLookPicHdr = NULL;
        ucLookFbId = FBM_PreLookFrameBufferFromDispQ(prFrcPrm->ucFbgId);
        prLookPicHdr = (ucLookFbId == FBM_FB_ID_UNKNOWN)? NULL : FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, ucLookFbId);
        if (prLookPicHdr)
        {
            BOOL fgReady;
            fgReady = (prLookPicHdr->fgSeamlessDisp) ? (prLookPicHdr->ucSeamlessResizeFbid != FBM_FB_ID_UNKNOWN) : TRUE;
            if (fgReady)
            {
                prFrcPrm->ucFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
            }
            else
            {
                prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
            }
        }
        else
        {
            prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
        }
#ifdef CC_3D_MM_DS_SUPPORT

        prFrcPrm->ucSubFbId = FBM_FB_ID_UNKNOWN;

#endif
    }
}

//LEO_B2R_REFACTORY ,LEO,20120124
static void _VdpSeekFinishNtfy(B2R_PRM_T* prFrcPrm, BOOL fgDash)
{
    /* Seek Finish Notify at 20081212 */
    UCHAR ucSeekMode;
    UINT32 u4SeekPts;
    UCHAR ucSeekNotifyTrigger;
    UCHAR ucVdpId = VDP_GetVdpId(prFrcPrm->ucB2rId);

    _VDPGetSeek(prFrcPrm->ucB2rId, &ucSeekMode, &u4SeekPts, &ucSeekNotifyTrigger);
    if (_ucRemainTimeToSendSeekDone == 0 && (ucSeekMode != 0) && (ucSeekNotifyTrigger != 0) &&
      ((u4SeekPts <= prFrcPrm->u4Pts && _eSpeed[prFrcPrm->ucB2rId] <= STC_SPEED_TYPE_FORWARD_1_DOT_5X) ||
      (u4SeekPts >= prFrcPrm->u4Pts && _eSpeed[prFrcPrm->ucB2rId] >= STC_SPEED_TYPE_REWIND_1X && _eSpeed[prFrcPrm->ucB2rId] < STC_SPEED_TYPE_NS)))
    {
        if (fgDash)
        {
            _ucRemainTimeToSendSeekDone = DURATION_TO_SEND_SEEK_DONE;
        }
        else 
        {
            _ucRemainTimeToSendSeekDone = 1;
        }
    }
    if (_ucRemainTimeToSendSeekDone == 1)
    {
        LOG(3, "+++++ Seek trigger notify\n");
        /* Seek Finish notify trigger status clear */
        arVdpSeek[prFrcPrm->ucB2rId].ucNotifyTrigger = 0;
        /* Seek Finish Notification */
        if (FBM_ChkFrameBufferPicFlag(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_MM_PSEUDO_EOS_FLAG))
        {
            _VDP_VsyncNotify(VDP_MSG_MM_SEEK_FIN_CB, ucVdpId, u4SeekPts, VDP_SEEK_STEP_NO_DATA);
            //_VDP_VsyncNotify(VDP_MSG_MM_CB, (UINT32)prFrcPrm->ucVdpId, VDP_COND_EOS, FBM_GetDecoderSrcId(prFrcPrm->ucFbgId));
        }
        else
        {
            _VDP_VsyncNotify(VDP_MSG_MM_SEEK_FIN_CB, ucVdpId, u4SeekPts, VDP_SEEK_STEP_OK);
        }
    }
    if (_ucRemainTimeToSendSeekDone > 0)
    {
        _ucRemainTimeToSendSeekDone--;
    }
}

//LEO_B2R_REFACTORY ,LEO,20120124
#ifdef ENABLE_MULTIMEDIA
static void _VdpHandleABRepeat(B2R_PRM_T* prFrcPrm ,
                               UCHAR ucB2rId,
                               VDEC_ES_INFO_T* prVdecEsInfo,
                               FBM_PIC_HDR_T *prPic,
                               BOOL fgFastForward)
#else
static void _VdpHandleABRepeat(B2R_PRM_T* prFrcPrm ,
                               UCHAR ucB2rId,
                               FBM_PIC_HDR_T *prPic,
                               BOOL fgFastForward)

#endif

{
    BOOL fgCheckPts = TRUE;
    BOOL fgTempref = TRUE;
    BOOL fgABReach = FALSE;
    UINT32 ui4_stc = 0x0;
    UCHAR ucAvSyncMode;
    UCHAR ucStcSrc;
    UCHAR ucVdpId = VDP_GetVdpId(ucB2rId);

    if (ucB2rId < B2R_NS)
    {
        if ((_eSpeed[ucB2rId] >= STC_SPEED_TYPE_FORWARD_1_DIV_2X) &&
                (_eSpeed[ucB2rId] <= STC_SPEED_TYPE_FORWARD_1_DOT_5X))
        {
            fgTempref = FALSE;
        }
    }
#ifdef ENABLE_MULTIMEDIA
    if ((prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG1_DAT) ||
            (prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_PS) ||
            (prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS) ||
            (prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS_192) ||
            (prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS_ZERO_192) ||
            (prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS_ENCRYPT_192) ||
            (prVdecEsInfo->eContainerType == SWDMX_FMT_VC1_ES) ||
            (prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_VIDEO_ES) ||
            (prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG4_VIDEO_ES) ||
            (prVdecEsInfo->eContainerType == SWDMX_FMT_H264_VIDEO_ES) ||
            /*for avs video es, pts and stc value may be error, use file position*/
            (prVdecEsInfo->eContainerType == SWDMX_FMT_AVS_VIDEO_ES)
       )
    {
        fgCheckPts = FALSE;
    }
#endif
    ui4_stc = _VDP_FrcGetStc(prFrcPrm->ucFbgId);

    if (prFrcPrm->rAB.fgPosition)
    {
        if (fgFastForward)
        {
            if ( ((fgCheckPts) && (prPic->u4PTS >= prFrcPrm->rAB.rB.u8Pts) &&
                    (ui4_stc >= prFrcPrm->rAB.rB.u8Stc)) ||
                    ((!fgCheckPts) &&((prPic->u8OffsetDisp >= prFrcPrm->rAB.rB.u8OffsetDisp) &&
                                      ((fgTempref) ||
                                       (prPic->i4TemporalRef == prFrcPrm->rAB.rB.i4TemporalRef)))))
            {
                fgABReach = TRUE;
            }
        }
        else
        {
            if (((fgCheckPts) && (prPic->u4PTS <= prFrcPrm->rAB.rA.u8Pts)) ||
                    ((!fgCheckPts) &&
                     (prPic->u8OffsetDisp <= prFrcPrm->rAB.rA.u8OffsetDisp)))
            {
                fgABReach = TRUE;
            }
        }
    }
    else
    {
        if (fgFastForward)
        {
            if ( ( (UINT64)prPic->u4PTS >= prFrcPrm->rAB.rB.u8Pts ) &&
                    ((  ui4_stc >= prFrcPrm->rAB.rB.u8Stc) ||
                     ((VDP_HalGetStepForward(ucB2rId) == VDP_PLAY_STEP_FORWARD)&&
                      (prFrcPrm->rAB.rA.u8Pts != prFrcPrm->rAB.rB.u8Pts))))
            {
                fgABReach = TRUE;
            }
        }
        else
        {
            if ((UINT64)prPic->u4PTS <= prFrcPrm->rAB.rA.u8Pts)
            {
                fgABReach = TRUE;
            }
        }
    }
    if ((fgABReach) && (arVdpSeek[ucB2rId].ucABNotifyTrigger != 0))
    {

        // if AB reach, driver should send SEEK DONE / STEP DONE notify mw before AB DONE notify ,
        // It's mw flow request
        _VdpHandleStepForward(prFrcPrm ,ucB2rId);

        /* Seek Finish Notify at 20081212 */
        _VdpSeekFinishNtfy(prFrcPrm, FALSE);  // LEO_B2R_REFACTORY

        arVdpSeek[ucB2rId].ucABNotifyTrigger = 0;
        arVdpSeek[ucB2rId].fgABReach = TRUE;
        Printf("***VDP_COND_RANGE_AB_DONE Notify****");
        FBM_GetSyncStc(prFrcPrm->ucFbgId, &ucAvSyncMode, &ucStcSrc);
#ifdef CC_53XX_SWDMX_V2
        STC_StopStc(ucStcSrc);
#else
        STC_StopStc();
#endif
        MMPlaybackSpeedTestAddTimeStamp(TIME_STAMP_VDP_ABREPEAT_DONE);
        _VDP_VsyncNotify(VDP_MSG_MM_CB, (UINT32)ucVdpId, VDP_COND_RANGE_AB_DONE, fgFastForward);
        LOG(3, "VDP_COND_RANGE_AB_DONE Notify\n");
    }
}


//LEO_B2R_REFACTORY ,LEO,20120124
#if (defined( DIVX_PLUS_CER) && defined(CC_B2R_CHAPTER_CHG))
static void _VdpHandleChapter(B2R_PRM_T* prFrcPrm,UCHAR ucB2rId)
{
    UCHAR ucVdpId = VDP_GetVdpId(ucB2rId);
    VDP_CHAPTER_T* prChapter = _VDP_FrcGetChapterObj();

    LOG(10, "Set(%d) Pts[0x%8x] @[0x%llx, 0x%llx].\n", prChapter->fgIsSetEndPts, prFrcPrm->u4Pts, prChapter->u8StartPts, prChapter->u8EndPts);
    if ( prChapter->fgIsSetEndPts )
    {
        prChapter->fgPending     = FALSE;

        if (ucB2rId < B2R_NS)
        {
            LOG(10, "Line(%d): Pts[0x%8x] @[0x%llx, 0x%llx].\n", __LINE__, prFrcPrm->u4Pts, prChapter->u8StartPts, prChapter->u8EndPts);
            if ( ( _eSpeed[ucB2rId] <= STC_SPEED_TYPE_REWIND_1024X ) &&
                    ( _eSpeed[ucB2rId] >= STC_SPEED_TYPE_REWIND_1X ) )
            {
                if ( prFrcPrm->u4Pts < prChapter->u8StartPts * 90 || prFrcPrm->u4Pts == 0)
                {
                    _VDP_VsyncNotify(VDP_MSG_MM_CB, (UINT32)ucVdpId, VDP_COND_CHAP_DONE, 0);
                    LOG(3, "VDP_COND_CHAP_DONE Notify (BACKWARD)\n");
                    prChapter->fgIsSetEndPts = FALSE;
                    prChapter->fgPending     = TRUE;
                }
                else if ( prChapter->u8StartPts == 0 && prChapter->u8EndPts == (UINT64)(-1))
                {
                    LOG(10, "Line(%d): u8StartPts == 0 && u8EndPts = -1.\n", __LINE__);
                    prChapter->fgIsSetEndPts = FALSE;
                    prChapter->fgPending     = TRUE;
                }
            }
            else
            {
                if ( prFrcPrm->u4Pts >= prChapter->u8EndPts * 90 )
                {

                    //CR[DTV00348906], seek the charpter using the number key when pause status.
                    //Don't send the step done(Seek Done in MW) because of this charpter done to MW.
                    //Add the step done after charpter done when this case.
                    _VdpHandleStepForward(prFrcPrm ,ucB2rId); // LEO_B2R_REFACTORY

                    _VDP_VsyncNotify(VDP_MSG_MM_CB, (UINT32)ucVdpId, VDP_COND_CHAP_DONE, 0);
                    LOG(3, "VDP_COND_CHAP_DONE Notify (FORWARD)\n");

                    prChapter->fgIsSetEndPts = FALSE;
                    prChapter->fgPending     = TRUE;
                }
                else if ( prChapter->u8StartPts == 0 && prChapter->u8EndPts == (UINT64)(-1))
                {
                    LOG(10, "Line(%d): u8StartPts == 0 && u8EndPts = -1.\n", __LINE__);
                    prChapter->fgIsSetEndPts = FALSE;
                    prChapter->fgPending     = TRUE;
                }
            }
        }
    }


    if (prChapter->fgPending)
    {
        _VdpPreparePendingForNextVsync(prFrcPrm);
    }
}
#endif

void _VdpCpyPicHdr2B2r(B2R_PRM_T * prFrcPrm, FBM_PIC_HDR_T * prNextPicHdr)
{
    if (prFrcPrm && prNextPicHdr)
    {
        prFrcPrm->fgProgress2Interlace = prNextPicHdr->fgProgressive2Interlace;
        prFrcPrm->u4Pts = prNextPicHdr->u4PTS;
        prFrcPrm->i4TemporalRef = prNextPicHdr->i4TemporalRef;
        prFrcPrm->ucPicCdTp = prNextPicHdr->ucPicCdTp;
        prFrcPrm->ucLastProgressiveFrame = prFrcPrm->ucProgressiveFrame;
        prFrcPrm->ucProgressiveFrame = prNextPicHdr->fgProgressiveFrm;
        prFrcPrm->ucRff = prNextPicHdr->fgRepFirstFld;
        prFrcPrm->ucTff = prNextPicHdr->fgTopFldFirst;
        prFrcPrm->u4TotlaTimeOffset = prNextPicHdr->u4TotlaTimeOffset;
        prFrcPrm->u8FilePosition = prNextPicHdr->u8Offset;
        prFrcPrm->u8FilePositionI = prNextPicHdr->u8OffsetI;
        prFrcPrm->u4TickNum = prNextPicHdr->u4TickNum;
        prFrcPrm->u2DecodingOrder = prNextPicHdr->u2DecodingOrder;
        prFrcPrm->fgDummyFrame = prNextPicHdr->fgDummyFrame;
        x_memcpy(&prFrcPrm->rExtra, &prNextPicHdr->rExtra, sizeof(VDP_PIC_EXTRA_INTO_T));
    }
}
//-----------------------------------------------------------------------------
/** Brief of _VdpGetNextDisp.
 */
//-----------------------------------------------------------------------------
static void _VdpGetNextDisp(B2R_PRM_T* prFrcPrm)
{
    BOOL fgDropFb = FALSE;
	BOOL fgRepeatFbForPending = FALSE;
    BOOL fgLastFbIsDrop = FALSE;
    BOOL fgGstPlayBack;
    UCHAR ucBackupFbId;
#ifdef CC_3D_MM_DS_SUPPORT
    UCHAR ucBackupSubFbId;
#endif
    UCHAR ucAvSyncMode;
    UCHAR ucStcSrc;
    //UINT32 ui4_stc = 0x0; //move to _VdpHandleABRepeat
    UCHAR ucB2rId;
    UCHAR ucVdpId;
    UINT8 u1AppMode =0;

#ifdef CC_3D_MM_DS_SUPPORT
    UINT8 u1AppModeSub =0;
#endif
    FBM_SEQ_HDR_T* prSeqHdr = NULL;
//   UCHAR ucSeekMode;
//    UINT32 u4SeekPts;
//    UCHAR ucSeekNotifyTrigger;

#ifdef ENABLE_MULTIMEDIA
    VDEC_ES_INFO_T* prVdecEsInfo = NULL;
#endif

    if (prFrcPrm == NULL)
    {
        return;
    }
    ucB2rId = prFrcPrm->ucB2rId;
    if (ucB2rId >= B2R_NS)
    {
        LOG(0, "ucB2rId >= B2R_NS , oops \n");
        return;
    }
    ucVdpId = VDP_GetVdpId(ucB2rId);
    if (ucVdpId >= VDP_NS)
    {
        LOG(0, "ucVdpId >= VDP_NS , oops \n");
        return;
    }
#ifdef CC_HWCOMP_UT
    u1AppMode = FBM_FBG_APP_OMX_DISP;
#else
    u1AppMode = FBM_GetFbgAppMode(prFrcPrm->ucFbgId);
#endif

#ifdef ENABLE_MULTIMEDIA
   prVdecEsInfo = _B2R_GetEsInfo(prFrcPrm->ucFbgId);
    if (prVdecEsInfo == NULL)
    {
        LOG(0,"prVdecEsInfo == NULL, oops\n");
        return;
    }
#endif


    ucBackupFbId = prFrcPrm->ucFbId;
#ifdef CC_3D_MM_DS_SUPPORT
    ucBackupSubFbId = prFrcPrm->ucSubFbId;
#endif
    prFrcPrm->u2TargetNs = 0;
    prFrcPrm->ucMMWaitSTC = 0;
    prFrcPrm->ucRealChangeFb = 0;
    fgGstPlayBack = FALSE;

#if defined(CC_USE_DDI)
    if (_fgTimeShift[ucB2rId])
    {
        fgGstPlayBack = TRUE;
    }
#endif
	
    do
    {
#ifdef CC_3D_MM_DS_SUPPORT
        _VDP_GetAdditionalFbgId(prFrcPrm);
        if (prFrcPrm->ucSubFbgId != FBM_FBG_ID_UNKNOWN)
        {
            u1AppModeSub = FBM_GetFbgAppMode(prFrcPrm->ucSubFbgId);
        }
#endif

        if (PENDING_IS_NEEDED)
        {
            /* MM may pend new FB, Can not assert */
            //ASSERT(prFrcPrm->ucReady == 0);
           _VdpGetFrameBufferFromPending(prFrcPrm);
        }
        else
        {
#ifdef CC_3D_MM_DS_SUPPORT
            if (ucB2rId >= B2R_NS)
            {
                LOG(3,"WARNING ucB2rId >= B2R_NS \n");
                ucB2rId = B2R_1;
            }
            {
                FBM_SEQ_HDR_T* prSeqHdr = FBM_GetFrameBufferSeqHdr(prFrcPrm->ucFbgId);
                if (prSeqHdr)
                {
                    prFrcPrm->u1B2R3DTrickModeType = prSeqHdr->u1B2R3DTrickModeType;
                }
            }
            if (/*(prFrcPrm->ucSubFbgId != FBM_FBG_ID_UNKNOWN)*/
                /*&&*/ ((_eSpeed[ucB2rId] == STC_SPEED_TYPE_FORWARD_1X) || (_eSpeed[ucB2rId] != STC_SPEED_TYPE_FORWARD_1X && prFrcPrm->u1B2R3DTrickModeType == B2R_3D_TRICK_MODE_3D_VIEW))
                && ((u1AppMode == FBM_FBG_APP_Frame_Pack_3D) || (u1AppModeSub == FBM_FBG_APP_Frame_Pack_3D))
                /*&& (prFrcPrm->fgB2R3DEnable)
                && (prFrcPrm->u1B2R3DType == B2R_3D_FRAME_SEQUENTIAL)*/)
            {
                _VdpGetFrameBufferForMVC3D(prFrcPrm);
            }
            else
#endif
            {
#ifdef CC_B2R_ENABLE_CHG_FRAME_MSG
                if (u1AppMode == FBM_FBG_APP_OMX_DISP)
                {
                    _VdpGetFrameBufferForOMX(prFrcPrm, &fgGstPlayBack, ucB2rId);
                }
                else
#endif // CC_B2R_ENABLE_CHG_FRAME_MSG
                    if (u1AppMode == FBM_FBG_APP_SKYPE)
                    {
                        _VdpGetFrameBufferForSkype(prFrcPrm);
                    }
                    else if (_VdpIsBivl3D(prFrcPrm))
                    {
                        _VDPGetFrameBufferForBivl3D(prFrcPrm);
                    }
                    #ifdef ENABLE_MULTIMEDIA
                    else if (prVdecEsInfo && (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_WIFI_DISPLAY))
                    {
                        if(prFrcPrm->ucReady == 0)
                        {
                            _VdpGetFrameBufferForSkype(prFrcPrm);
                        }
                        else
                        {
                            _VdpGetFrameBufferForGeneral(prFrcPrm, &fgGstPlayBack);
                        }
                        if (prFrcPrm->ucFbId != FBM_FB_ID_UNKNOWN)
                        {
#ifdef TIME_MEASUREMENT
                            //u4RecvCnt++;
#endif
                        }
                    }
                    #endif
                    else
                    {
                        _VdpGetFrameBufferForGeneral(prFrcPrm, &fgGstPlayBack);
                    }
            }

            if (prFrcPrm->ucFbId != 0xFF)
            {
                prFrcPrm->ucRealChangeFb = 1;
            }
            else
            {
                _u4TimeShfitChangeFrmCnt =0;
                _fgTimeShfitThumbStartPlay = FALSE;
            }
        }
	
        if (FBM_GetFrameBufferNs(prFrcPrm->ucFbgId) == 1)
        {
            _B2R_OneFbChgHandle(prFrcPrm);
        }

        // vdp lock after release protection
        if (prFrcPrm->ucFbId != FBM_FB_ID_UNKNOWN)
        {
#ifdef CC_MAPLE_CUST_DRV
            FBM_SEQ_HDR_T *prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prFrcPrm->ucFbgId);
            if(prFbmSeqHdr)
            {
                if(prFbmSeqHdr->u4resolutionChg != prFbmSeqHdr->u4ChgPreStatus)
                {
                    DRVCUST_SendEvent(E_CUST_SIGNAL_FMT_CHANGE,SV_VP_MAIN);
                    DRVCUST_SendEvent(E_CUST_SIGNAL_FMT_CHANGE,SV_VP_PIP);
                }

                if(prFrcPrm->pt_cust_info)
                {
                    prFrcPrm->pt_cust_info->u4resolutionChg =
                        prFbmSeqHdr->u4resolutionChg;
                }
                prFbmSeqHdr->u4ChgPreStatus = prFbmSeqHdr->u4resolutionChg;//record current status
            }      
#endif
            _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_FB_STATUS_LOCK);
            if (FBM_ChkFbgCreateFromInst(prFrcPrm->ucFbgId) == FALSE)
            {
                if (FBM_CheckFrameBufferStatus(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_FB_STATUS_LOCK) == 0)
                {
                    prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
                }
            }
        }
#ifdef CC_3D_MM_DS_SUPPORT
        if (prFrcPrm->ucSubFbId != FBM_FB_ID_UNKNOWN)
        {
            _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucSubFbgId, prFrcPrm->ucSubFbId, FBM_FB_STATUS_LOCK);
            if (FBM_ChkFbgCreateFromInst(prFrcPrm->ucSubFbgId) == FALSE)
            {
                if (FBM_CheckFrameBufferStatus(prFrcPrm->ucSubFbgId, prFrcPrm->ucSubFbId, FBM_FB_STATUS_LOCK) == 0)
                {
                    prFrcPrm->ucSubFbId= FBM_FB_ID_UNKNOWN;
                }
            }
        }
#endif
        if (prFrcPrm->ucFbId != FBM_FB_ID_UNKNOWN)
        {
            UCHAR ucFrameEncode;
            FBM_PIC_HDR_T* prNextPicHdr;

            //FBM_SetFrameBufferStatus(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_FB_STATUS_LOCK);

            //for DTV00320479
#ifdef CC_3D_MM_DS_SUPPORT
            prFrcPrm->fgFastForward = !(FBM_ChkFrameBufferPicFlag(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_MM_BACKWARD_FLAG) || \
            FBM_ChkFrameBufferPicFlag(prFrcPrm->ucSubFbgId, prFrcPrm->ucSubFbId, FBM_MM_BACKWARD_FLAG));
#else
            prFrcPrm->fgFastForward = !(FBM_ChkFrameBufferPicFlag(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_MM_BACKWARD_FLAG));
#endif
            // backup picture header info
            prNextPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, prFrcPrm->ucFbId);

            if (prNextPicHdr != NULL)
            {
                FBM_PIC_HDR_T *prPic;
                //         BOOL fgABReach      = FALSE;  // move to _VdpHandleABRepeat
                BOOL fgFastForward;


                if (ucB2rId < B2R_NS)
                {
                    if (_eSpeed[ucB2rId] <= STC_SPEED_TYPE_FORWARD_1_DOT_5X)
                    {
                        fgFastForward = TRUE;
                    }
                    else
                    {
                        fgFastForward = FALSE;
                    }
                }
                else
                {
                    fgFastForward = TRUE;
                }

                if (FBM_ChkFrameBufferPicFlag(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_MM_SYNC_POINT_FLAG))
                {
                    FBM_ClrFrameBufferPicFlag(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_MM_SYNC_POINT_FLAG);
                    _VDP_VsyncNotify(VDP_MSG_MM_CB, (UINT32)ucVdpId, VDP_COND_SYNC_POINT, 0);
                }

                if (FBM_ChkFrameBufferPicFlag(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_MM_3D_CHG_FLAG))
                {
                    FBM_ClrFrameBufferPicFlag(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_MM_3D_CHG_FLAG);
                    FBM_SetFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_3D_SEQ_CHG);
                }
                _VdpHandleBitRateChange(prFrcPrm);
#if 1
                if (!arVdpSeek[ucB2rId].fgABReach)
                {

#if 0
                    if ( VDP_HalGetStepForward(ucB2rId) == VDP_PLAY_STEP_FORWARD )
                    {
                        FBM_GetSyncStc(prFrcPrm->ucFbgId, &ucAvSyncMode, &ucStcSrc);

#ifdef CC_53XX_SWDMX_V2
                        STC_SetStcValue(ucStcSrc, prFrcPrm->u4Pts);
#else
                        STC_SetStcValue(prFrcPrm->u4Pts);
#endif

                        LOG(5," Step PTS update STC(0x%x)\n",prFrcPrm->u4Pts);
                    }
#endif

                    if (_VdpHandleEos(prFrcPrm))
                    {
                        return;
                    }

                }

                prPic = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, prFrcPrm->ucFbId);
                if ((prPic) && (prFrcPrm->rAB.fgValid))
                {
#ifdef ENABLE_MULTIMEDIA
                    _VdpHandleABRepeat(prFrcPrm ,
                                       ucB2rId,
                                       prVdecEsInfo,
                                       prPic,
                                       fgFastForward);  // LEO_B2R_REFACTORY
#else
                    _VdpHandleABRepeat(prFrcPrm ,
                                       ucB2rId,
                                       prPic,
                                       fgFastForward);  // LEO_B2R_REFACTORY
#endif
                }
#endif
                // Check Temporal reference
                if (prFrcPrm->ucLastFbId != FBM_FB_ID_UNKNOWN)
                {
                    if ((prNextPicHdr->i4TemporalRef!= (1 + prFrcPrm->i4TemporalRef)) &&
                            (prNextPicHdr->i4TemporalRef != 0))
                    {
                        // [LOG] Discontinuous temporal reference
                        SYSLOG(VDP_PREFIX + 97, prFrcPrm->ucFbgId, prFrcPrm->i4TemporalRef, prNextPicHdr->i4TemporalRef );

                        LOG(10, "Fbg(%d) Temporal Ref %d >> %d\n"
                            , prFrcPrm->ucFbgId
                            , prFrcPrm->i4TemporalRef
                            , prNextPicHdr->i4TemporalRef);

                        if (prFrcPrm->prFbCounter)
                        {
                            prFrcPrm->prFbCounter->u4DispSkip++;
                        }
                    }
                }

                FBM_GetSyncStc(prFrcPrm->ucFbgId, &ucAvSyncMode, &ucStcSrc);
				_VdpCpyPicHdr2B2r(prFrcPrm, prNextPicHdr);
                prFrcPrm->ucLastRff = prFrcPrm->ucRff;
                prNextPicHdr->fgDummyFrame = 0;
                
                _prVdpConf[ucVdpId]->u4QpAvg = prNextPicHdr->u4QpAvg;

                /* VDP trigger mode + audio master to do trigger mode */
                /* For Trick to Normal or Audio language change */
#ifndef CC_B2R_WAIT_NPTV_STABLE
                if (!(VDP_HalGetStepForward(ucB2rId) == VDP_PLAY_STEP_FORWARD) && prFrcPrm->ucReady)
                {
                    /* Trigger Audio while not step forward state */
                    _VdpTriggerAudOn(ucB2rId, prFrcPrm->u4Pts);
                }
#endif

#ifdef CC_FORCE_32_FLAG_ERR
                if (prFrcPrm->ucValid32Seq)
                {
                    static UINT32 u4Force32Idx = 0;

                    if (++u4Force32Idx >= 16)
                    {
                        if (prFrcPrm->ucRff)
                        {
                            if (prFrcPrm->ucTff)
                            {
                                prFrcPrm->ucTff = 0;
                            }
                            else
                            {
                                prFrcPrm->ucTff = 1;
                            }
                        }
                    }

                    if (u4Force32Idx >= (16 + (PD_32_FLAG_LEAVE_LOOP_SIZE / 2)))
                    {
                        u4Force32Idx = 0;
                    }
                }
#endif
            }
            else
            {
                ASSERT(0);
            }

#if 1 // def CC_MT5360        
            //for CLI frame cropping test
#ifdef CC_CLI_TEST
            if ((_prVdpConf[ucVdpId]->u4MpegHeight == 1088) &&
                    ((_prVdpConf[ucVdpId]->u4FrmCropBtm - _prVdpConf[ucVdpId]->u4FrmCropTop) == 1080))
            {
                vDrvDISetDtvClip(ucVdpId, _prVdpConf[ucVdpId]->u4FrmCropTop);
            }
#endif
#endif

            //For CLI NoOverscan test
#ifdef CC_CLI_TEST
            {
                static UCHAR _ucNoOverScan = 0;

                if (_prVdpConf[ucVdpId]->ucNoOverScan != _ucNoOverScan)
                {
                    _ucNoOverScan = _prVdpConf[ucVdpId]->ucNoOverScan;
                    _u4ReturnValue |= VDP_EVENT_SEQ_CHG;
                }
            }
#endif



            // Sequence Header Change, update size, frame rate and aspect ratio
            if ((FBM_ChkFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_SEQ_CHG) ||
                    FBM_ChkFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_3D_SEQ_CHG) ||
                    FBM_ChkFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_SEQ_CHG_SPEEDUP) ||
                    (_fgInvalidFrmRate[ucB2rId]) ||
                    (!prFrcPrm->ucSeqInfoValid)) && (prFrcPrm->ucNewFbArrive == 0))
            {
                if (_VdpGetSequenceInfo(prFrcPrm, FALSE) == FALSE)
                {
                    LOG(1, "Get sequence info failed (with fb)\n");
                    return;
                }

                _fgHandleSeqChg[ucB2rId] = FALSE;
            }
            else if ((FBM_ChkFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_SEQ_CHG) ||
                      FBM_ChkFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_3D_SEQ_CHG) ||
                      FBM_ChkFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_SEQ_CHG_SPEEDUP) ||
                      (_fgInvalidFrmRate[ucB2rId]) ||
                      (!prFrcPrm->ucSeqInfoValid)) && (prFrcPrm->ucNewFbArrive == 1))
            {
                _fgHandleSeqChg[ucB2rId] = TRUE;
            }

#ifdef VDP_DEBUG_TR
            // Check Temporal reference
            if (prFrcPrm->ucLastFbId != FBM_FB_ID_UNKNOWN)
            {
                FBM_PIC_HDR_T* prLastPicHdr;

                prLastPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, prFrcPrm->ucLastFbId);

                if (prNextPicHdr != NULL)
                {
                    INT32 i4TemporalRef;

                    if (prNextPicHdr->ucPicCdTp == MPEG_PIC_TYPE_B)
                    {
                        prFrcPrm->szDebugBuf[prFrcPrm->u4DebugIdx] = 'B';
                    }
                    else if (prNextPicHdr->ucPicCdTp == MPEG_PIC_TYPE_P)
                    {
                        prFrcPrm->szDebugBuf[prFrcPrm->u4DebugIdx] = 'P';
                    }
                    else
                    {
                        prFrcPrm->szDebugBuf[prFrcPrm->u4DebugIdx] = 'I';
                    }

                    i4TemporalRef = (prNextPicHdr->i4TemporalRef & 0xF);
                    if (i4TemporalRef < 10)
                    {
                        prFrcPrm->szDebugBufTr[prFrcPrm->u4DebugIdx] = '0' + i4TemporalRef;
                    }
                    else
                    {
                        prFrcPrm->szDebugBufTr[prFrcPrm->u4DebugIdx] = 'A' + (i4TemporalRef - 10);
                    }

                    if (++prFrcPrm->u4DebugIdx >= (VDP_DEBUG_BUF_SIZE - 1))
                    {
                        prFrcPrm->u4DebugIdx = 0;
                    }
                }
            }
#endif
			//calculate repeat count for push trick mode before drop and repeat
			if (!fgDropFb)
			{
				_VdpCalcRepeatCntForPushTrickMode(prFrcPrm, fgLastFbIsDrop);
                fgLastFbIsDrop = FALSE;
			}
            // Backup Last Frame Buffer ID
            prFrcPrm->ucLastFbId = prFrcPrm->ucFbId;
			fgDropFb = FALSE;

            // Frame Rate Conversion
            _VdpCalculateVsyncNs(prFrcPrm);

            prSeqHdr = FBM_GetFrameBufferSeqHdr(prFrcPrm->ucFbgId);
            if ((prSeqHdr)&&(_fgInvalidFrmRate[ucB2rId])&&(_ucFrameRateUnkownCount[ucB2rId] <= 0x5))
            {
                if ( ( FBM_FB_ID_UNKNOWN != prFrcPrm->ucFbId) &&
                        ( MPEG_FRAME_RATE_UNKNOWN == prSeqHdr->ucFrmRatCod ) )
                {
                    // pending for unkown frame rate
                    _VdpPreparePendingForNextVsync(prFrcPrm);

                    _ucFrameRateUnkownCount[ucB2rId]++;
                    LOG(3, "pending for unkown frame rate(%d) \n",_ucFrameRateUnkownCount[ucB2rId]);
                }
            }

            // Dummy frame contain frame infomation but not to display
            if (prFrcPrm->fgDummyFrame)
            {
                // not display this frame
                prFrcPrm->u2TargetNs = 0;
                prFrcPrm->u2ChangeFieldNs = 0;
            }

            if (prFrcPrm->rAB.fgValid)
            {
                if (arVdpSeek[ucB2rId].fgABReach)
                {
                    // not display this frame
                    prFrcPrm->u2TargetNs = 0;
                    prFrcPrm->u2ChangeFieldNs = 0;
                }
            }

            if (prFrcPrm->ucFbId != FBM_FB_ID_UNKNOWN)
            {
                /* prFrcPrm->u2TargetNs should be 0 if  prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN
                   for AVS-R case */
                if (FBM_ChkFrameBufferPicFlag(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_MM_NOT_DISPLAY_FLAG))
                {
                    FBM_ClrFrameBufferPicFlag(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_MM_NOT_DISPLAY_FLAG);

                    prFrcPrm->u2TargetNs = 0;
                    LOG(7, "No display (%d %d %d)\n", ucB2rId, prFrcPrm->ucFbgId, prFrcPrm->ucFbId);
                }
#if (defined( DIVX_PLUS_CER) && defined(CC_B2R_CHAPTER_CHG))
                _VdpHandleChapter(prFrcPrm,ucB2rId);  // LEO_B2R_REFACTORY
#endif


#ifdef CC_B2R_3D_ERROR_SUPPORT
                if (!FBM_ChkFrameBufferFlag(prFrcPrm->ucFbgId,FBM_FLAG_3D_ERROR_HANDLE_DISABLE))
                {
                    if (prFrcPrm->fgB2R3DError && prFrcPrm->fgB2R3DEnable && !prFrcPrm->fgB2RForce2D)
                    {
                        prFrcPrm->u4B2R3DSuccessCount++;
                        if (prFrcPrm->u4B2R3DSuccessCount > B2R_3D_ERROR_RECOVER_COUNT)
                        {
                            FBM_SEQ_HDR_T* prSeqHdr;
                            prSeqHdr = FBM_GetFrameBufferSeqHdr(prFrcPrm->ucFbgId);
                            prSeqHdr->fgB2RForce2D = FALSE;
                            prSeqHdr->u1B2RForce2DType = B2R_FORCE_2D_L_VIEW;
                            FBM_SetFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_3D_SEQ_CHG);
                            prFrcPrm->fgB2R3DError = FALSE;
                        }
                    }
                    prFrcPrm->u4B2R3DErrorCount =0;
                }
#endif
                prFrcPrm->fgDispFail = FALSE;
            }
            else
            {
#ifdef CC_B2R_3D_ERROR_SUPPORT
                if (!prFrcPrm->fgB2R3DError && prFrcPrm->fgB2R3DEnable && !prFrcPrm->fgB2RForce2D)
                {
                    prFrcPrm->u4B2R3DErrorCount++;
                    if (prFrcPrm->u4B2R3DErrorCount > B2R_3D_ERROR_COUNT)
                    {
                        FBM_SEQ_HDR_T* prSeqHdr;
                        prSeqHdr = FBM_GetFrameBufferSeqHdr(prFrcPrm->ucFbgId);
                        prSeqHdr->fgB2RForce2D = TRUE;
                        prSeqHdr->u1B2RForce2DType = B2R_FORCE_2D_L_VIEW;
                        FBM_SetFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_3D_SEQ_CHG);
                        prFrcPrm->fgB2R3DError = TRUE;
                    }
                }
                prFrcPrm->u4B2R3DSuccessCount = 0;
#endif
                prFrcPrm->fgDispFail = TRUE;
                _u4TimeShfitChangeFrmCnt=0;
                _fgTimeShfitThumbStartPlay = FALSE;
            }

            /* Seek Finish Notify at 20081212 */
#ifdef ENABLE_MULTIMEDIA            
            _VdpSeekFinishNtfy(prFrcPrm, (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_DASH));  // LEO_B2R_REFACTORY
#else
            _VdpSeekFinishNtfy(prFrcPrm, FALSE);  // LEO_B2R_REFACTORY
#endif

#ifdef CC_B2R_WAIT_NPTV_STABLE
            //UCHAR ucPlayMode;
            //FBM_GetPlayMode(prFrcPrm->ucFbgId,&ucPlayMode);
            //if( FBM_FBG_MM_MODE == ucPlayMode )
            {
                /* VDP trigger mode + audio master to do trigger mode */
                /* For Trick to Normal or Audio language change */
                //if(!((VDP_HalGetStepForward(ucB2rId) == VDP_PLAY_STEP_FORWARD)) && prFrcPrm->ucFbId != FBM_FB_ID_UNKNOWN)
                if (!((VDP_HalGetStepForward(ucB2rId) == VDP_PLAY_STEP_FORWARD))
                        && prFrcPrm->ucFbId != FBM_FB_ID_UNKNOWN &&
                        prFrcPrm->ucReady && _afgNPTVStable[ucB2rId])

                {
                    #ifdef ENABLE_MULTIMEDIA            
                    if(prVdecEsInfo && (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_WIFI_DISPLAY))
                    {
                        //Do nothing
                    }
                    else
                    #endif
                    {
                        /* Trigger Audio while not step forward state */
                        _VdpTriggerAudOn(ucB2rId, prFrcPrm->u4Pts);
                    }
                }
            }
#endif
            // if no field change required, u2ChangeFieldNs = 0
            if (prFrcPrm->u2TargetNs < prFrcPrm->u2ChangeFieldNs)
            {
                LOG(5, "u2TargetNs > u2ChangeFieldNs\n");
            }

            // frame/field encode
            // According to suggestion of DVD Benchmark
            if (prFrcPrm->ucProgressiveFrame)
            {
                ucFrameEncode = 1;
            }
            else if (prFrcPrm->ucLastProgressiveFrame && prFrcPrm->ucLastRff)
            {
                ucFrameEncode = 1;
            }
            else
            {
                ucFrameEncode = 0;
            }

            // frame/field encode type change
            if (prFrcPrm->u2TargetNs != 0)
            {
#ifdef CC_VDP_FORCE_FIELD_ENCODE
                if (ucFrameEncode != prFrcPrm->ucHwFrameEncode)
                {
                    prFrcPrm->ucHwFrameEncode = ucFrameEncode;
                    _VDP_HalSetChromaMode(ucB2rId, 0, 0);
                }
#else
                // 5381
                // 1) frame encode & TFF stable >> frame encode
                // 2) else field encode

                if (ucFrameEncode != 0)
                {
                    if (prFrcPrm->ucLastTff == prFrcPrm->ucTff)
                    {
                        if (prFrcPrm->ucTffStableCounter < VDP_TFF_STABLE_COUNT)
                        {
                            prFrcPrm->ucTffStableCounter++;
                        }
                    }
                    else
                    {
                        prFrcPrm->ucTffStableCounter = 0;
                    }

                    if (prFrcPrm->ucTffStableCounter < VDP_TFF_STABLE_COUNT)
                    {
                        ucFrameEncode = 0;
                    }
                }

                prFrcPrm->ucLastTff = prFrcPrm->ucTff;

                if (ucFrameEncode != prFrcPrm->ucHwFrameEncode)
                {
                    prFrcPrm->ucHwFrameEncode = ucFrameEncode;
                    _VDP_HalSetChromaMode(ucB2rId, prFrcPrm->ucHwFrameEncode, prFrcPrm->ucTff);
                }
#endif
            }

            // clear log
            if (prFrcPrm->prFbCounter)
            {
                if ((prFrcPrm->ucReady == 0) &&
                        (prFrcPrm->u2TargetNs != 0))
                {
                    prFrcPrm->prFbCounter->u4DispOk = 0;
                    prFrcPrm->prFbCounter->u4DispFail = 0;
                    prFrcPrm->prFbCounter->u4DispSkip = 0;
                    prFrcPrm->prFbCounter->u4DispRepeat = 0;
                    prFrcPrm->prFbCounter->u4DispDrop = 0;

#ifdef CHANNEL_CHANGE_LOG
                    STC_CLOCK rStcClk;
                    UCHAR ucAvSyncMode;
                    UCHAR ucStcSrc;
                    HAL_TIME_T dt;

                    FBM_GetSyncStc(prFrcPrm->ucFbgId, &ucAvSyncMode, &ucStcSrc);
                    STC_GetSrc(ucStcSrc, &rStcClk);
                    HAL_GetTime(&dt);
                    LOG(0, " %u.%06u s [AV SYNC] 8 VDP 1st Video (PTS,STC)=(0x%x,0x%x)\n", dt.u4Seconds, dt.u4Micros, prFrcPrm->u4Pts, rStcClk.u4Base);
#endif
#ifdef TIME_MEASUREMENT
                    TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME_DRV, "REC_1st_VID, B2R");
                    TMS_DIFF_EX(TMS_FLAG_BOOT, TMS_COOL_BOOT_TIME, "REC_1st_VID, B2R");

#endif
                    MMPlaybackSpeedTestAddTimeStamp(TIME_STAMP_VDP_FRAME_FIRST);
                    /* boot up measurement in release version, record once.*/
                    if (!_fgB2RProfile)
                    {
                        x_os_drv_set_timestamp("REC_1st_VID, B2R");
                        _fgB2RProfile = TRUE;
                    }
                }

                prFrcPrm->prFbCounter->u4DispOk++;
            }
            else
            {
                ASSERT(0);
            }
			
            if (prFrcPrm->u2TargetNs == 0)
            {
                if (prFrcPrm->ucFbId != FBM_FB_ID_UNKNOWN)
                {
                    LOG(7, "D (%d %d %d)\n", ucB2rId, prFrcPrm->ucFbgId, prFrcPrm->ucFbId);
                    _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_FB_STATUS_EMPTY);
#ifdef CC_3D_MM_DS_SUPPORT
                    if (prFrcPrm->ucSubFbId != FBM_FB_ID_UNKNOWN)
                    {
                        LOG(7, "D (%d %d %d)\n", ucB2rId, prFrcPrm->ucSubFbgId, prFrcPrm->ucSubFbId);
                        _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucSubFbgId, prFrcPrm->ucSubFbId, FBM_FB_STATUS_EMPTY);
                    }
#endif
                    fgDropFb = TRUE;
                    fgLastFbIsDrop = TRUE;
                }
                else
                {
                    ASSERT(prFrcPrm->ucPendingFbId != FBM_FB_ID_UNKNOWN);
                    prFrcPrm->ucLastFbId = FBM_FB_ID_UNKNOWN;
					fgRepeatFbForPending = TRUE;
                }
            }

            // [LOG] VDP get display FB success
            SYSLOG(VDP_PREFIX + prFrcPrm->ucFbgId, prFrcPrm->ucFbId, prFrcPrm->prFbCounter->u4DispOk, prFrcPrm->prFbCounter->u4DispFail);
            _VdpDetermineRepeatForPushTrickMode(prFrcPrm, fgRepeatFbForPending, &fgDropFb, FBM_FB_ID_UNKNOWN);
        }
        else
        {
            UCHAR ucAvSyncMode;
            UCHAR ucStcSrc;
			fgDropFb = FALSE;
            prFrcPrm->fgDispFail = TRUE;
            if (_fgHandleSeqChg[ucB2rId] || FBM_ChkFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_SEQ_CHG_SPEEDUP))
            {
                if (_VdpGetSequenceInfo(prFrcPrm, FALSE) == FALSE)
                {
                    LOG(1, "Get sequence info failed (without fb)\n");
                    return;
                }

                _fgHandleSeqChg[ucB2rId] = FALSE;

#ifdef CC_B2R_WAIT_NPTV_STABLE
                _au4WaitNPTVStableCount[ucB2rId] = 0;
#endif
            }

#ifndef VDP_B2R_PSCAN_CRC_FLOW
            // ask mpeg decode to skip one picture due to repeat, if AV Sync NOT enable
            FBM_GetSyncStc(prFrcPrm->ucFbgId, &ucAvSyncMode, &ucStcSrc);
            if (ucAvSyncMode == VID_SYNC_MODE_NONE )
            {
//#ifdef ENABLE_MULTIMEDIA
//                if(prVdecEsInfo->eMMSrcType != SWDMX_SRC_TYPE_NETWORK_SKYPE)
//#endif
                if ((u1AppMode != FBM_FBG_APP_OMX_DISP) && (u1AppMode != FBM_FBG_APP_SKYPE))
                {
                    FBM_SetFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_SKIP_PIC);
#if (defined(CC_3D_MM_DS_SUPPORT) && defined(CC_B2R_ENABLE_R_VIEW_SKIP_FRAME))
                    if (prFrcPrm->ucSubFbgId != FBM_FBG_ID_UNKNOWN && u1AppModeSub == FBM_FBG_APP_Frame_Pack_3D)
                    {
                        FBM_SetFrameBufferFlag(prFrcPrm->ucSubFbgId, FBM_FLAG_SKIP_PIC);
                    }
#endif
                }
            }
#endif

            if (prFrcPrm->prFbCounter)
            {
                prFrcPrm->prFbCounter->u4DispFail++;
                //LOG(3, "DispFail++\n");
            }
            else
            {
                ASSERT(0);
            }

#ifdef SYSTEM_LOG
            if (prFrcPrm->ucReady)
            {
                // [LOG] VDP get display FB fail
                SYSLOG(VDP_PREFIX + 98, prFrcPrm->ucFbgId, prFrcPrm->prFbCounter->u4DispOk, prFrcPrm->prFbCounter->u4DispFail);
            }
#endif
           _VdpDetermineRepeatForPushTrickMode(prFrcPrm, FALSE, &fgDropFb, ucBackupFbId);
        }
    }
    while (REFETCH_IS_NEEDED);

    if (prFrcPrm->u2TargetNs == 0)
    {
        prFrcPrm->ucFbId = ucBackupFbId;
#ifdef CC_3D_MM_DS_SUPPORT
        prFrcPrm->ucSubFbId = ucBackupSubFbId;
#endif
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VdpCalculateVsyncNs.
 * reference 8105 vsv_sync.c (fgVsvCalculateVsyncNs)
 * Description : Calculate Ns at Normal mode and Trick mode
 *        (1) Calculate Base Vsync Ns
 *        (2) Normal mode: (A) Farme Rate Conversion (B) AV SYNC
 *        (3) Trick mode: Multiply Vsync Order (according to Trick speed)
 * VSync NS: Combine, VSync, CCNs, InverseField ?
 */
//-----------------------------------------------------------------------------
static void _VdpCalculateVsyncNs(B2R_PRM_T* prFrcPrm)
{
    if (prFrcPrm == NULL)
    {
        return;
    }

    _VdpBaseVSyncNs(prFrcPrm);

    _VdpFrameRateProc(prFrcPrm);

    _VdpAVSyncProc(prFrcPrm);

#ifdef CC_FRC_TEST
    _VdpFrcTest(prFrcPrm);
#endif
}

//-----------------------------------------------------------------------------
/** Brief of _VdpChk32Flag.
 * reference 8105 vsv_sync.c (vVsvChk32Flag)
 */
//-----------------------------------------------------------------------------
static void _VdpChk32Flag(B2R_PRM_T* prFrcPrm)
{
#ifdef VDP_HANDLE_32FLAG
    UCHAR ucLast32Seq;

    if (prFrcPrm == NULL)
    {
        return;
    }

    ucLast32Seq = prFrcPrm->ucValid32Seq;

    // check the following 32 flag sequence
    // TFF Sequence    : 1 0 0 1 1 0 0 1 ...
    // RFF Sequence    : 1 0 1 0 1 0 1 0 ...

    if (prFrcPrm->ucValid32Seq == 0)
    {
        UINT32 u4Step;

        u4Step = prFrcPrm->ucPd32SeqStep & (PD_32_FLAG_SEQ - 1);

        if ((prFrcPrm->ucRff == _au4Pd32FlagCadence[0][u4Step]) &&
                (prFrcPrm->ucTff == _au4Pd32FlagCadence[1][u4Step]))
        {
            // valid cadence

            if (++(prFrcPrm->ucPd32SeqStep) >= PD_32_FLAG_LOOP_SIZE)
            {
                prFrcPrm->ucValid32Seq = 1;
                prFrcPrm->ucRelease32Cnt = 0;
            }
        }
        else
        {
            prFrcPrm->ucPd32SeqStep = GUESS_32_STEP(prFrcPrm->ucTff, prFrcPrm->ucRff);
        }
    }
    else
    {
        UINT32 u4Step;

        u4Step = (prFrcPrm->ucPd32SeqStep++) & (PD_32_FLAG_SEQ - 1);

        if ((prFrcPrm->ucRff == _au4Pd32FlagCadence[0][u4Step]) &&
                (prFrcPrm->ucTff == _au4Pd32FlagCadence[1][u4Step]))
        {
            prFrcPrm->ucRelease32Cnt = 0;
        }
        else
        {
            // invalid cadence

//            LOG(1, "Invalid 32 (%d %d %d) (%d)\n",
//                prFrcPrm->ucPd32SeqStep,
//                prFrcPrm->ucTff,
//                prFrcPrm->ucRff,
//                prFrcPrm->ucRelease32Cnt);

            if (++(prFrcPrm->ucRelease32Cnt) >= PD_32_FLAG_LEAVE_LOOP_SIZE)
            {
                prFrcPrm->ucValid32Seq = 0;
                prFrcPrm->ucPd32SeqStep = 0;
            }
            else
            {
                // guess next cadence
                prFrcPrm->ucPd32SeqStep = GUESS_32_STEP(prFrcPrm->ucTff, prFrcPrm->ucRff);
            }
        }
    }

    if (ucLast32Seq != prFrcPrm->ucValid32Seq)
    {
        LOG(3, "32 Flag %d >> %d\n", ucLast32Seq, prFrcPrm->ucValid32Seq);
    }
#else

    // ignore 32 flag in MT5371 project

#ifdef VDP_TEST_32FLAG
    do
    {
        static UINT32 u4Test32Flag = 0;

        if (++u4Test32Flag >= 64)
        {
            u4Test32Flag = 0;

            if (prFrcPrm->ucValid32Seq == 0)
            {
                prFrcPrm->ucValid32Seq = 1;
            }
            else
            {
                prFrcPrm->ucValid32Seq = 0;
            }

            LOG(3, "32 Flag >> %d\n", prFrcPrm->ucValid32Seq);
        }
    }
    while (0);
#else
    UNUSED(prFrcPrm);
#endif

#endif
}

//-----------------------------------------------------------------------------
/** Brief of _VdpBaseVSyncNs.
 * reference 8105 vsv_sync.c (fgVsvBaseVSyncNs)
 * Description : Get Base VSync Ns according to RFF, TFF, and PScan state
 *        (1) VSyncNs: duration to display the frame
 *        (2) CCNs:0 Field numbers for Close Caption
 *        (3) CombineNs: duration to combine with previous frame when in PScan output
 */
//-----------------------------------------------------------------------------
static void _VdpBaseVSyncNs(B2R_PRM_T* prFrcPrm)
{
#ifdef VDP_MONITOR_TFF
    static UCHAR ucLastTff = 1;
#endif

    if (prFrcPrm == NULL)
    {
        return;
    }

    _VdpChk32Flag(prFrcPrm);

#ifdef VDP_MONITOR_TFF
    if ((prFrcPrm->ucValid32Seq == 0) &&
            (ucLastTff != prFrcPrm->ucTff))
    {
        LOG(1, "TFF %d >> %d (RFF %d %d)\n"
            , ucLastTff, prFrcPrm->ucTff
            , prFrcPrm->ucLastRff, prFrcPrm->ucRff);
    }
    ucLastTff = prFrcPrm->ucTff;
#endif

    prFrcPrm->u4InStcPeriod = prFrcPrm->u4InStcPeriodBase;

    prFrcPrm->ucMultipleField = 0;

    // 20080209 William: SGP, StickyVideo2.trp
    // Handle RFF, we should not consider frame rate
    if (prFrcPrm->ucProgressiveSeqFlag)        // Progressive Sequence
    {
        if (prFrcPrm->ucProgressiveFrame == 0)  // Progressive Frame
        {
            // Progressive Sequence with Interlace Frame ?! (Bad Edit?)
            if (prFrcPrm->ucRff)
            {
                // According to 13818-2, If Progressive Frame = 0, RFF shall be 0.
                LOG(3, "Warning! Progressive Frame = 0 and RFF = 1\n");
            }
        }

        // Acoording to 13818-2, top_field_first section

        if ((prFrcPrm->ucRff) && (prFrcPrm->ucTff))
        {
            // 3 Frame for PRGS_SEQ + REPEAT_1ST_Fld + TOP_FLS_FIRST
            prFrcPrm->u4InStcPeriod *= 3;

            if (!prFrcPrm->ucProgressiveOut)
            {
                // although according to spec, it should play 6 vsync
                // but we only encounter streams that will error if we play 6 vsync, but will correct if we play 3 vsync,
                // this is the stream error, but we modify our code to cover the stream error

                prFrcPrm->u4InStcPeriod /= 2;
            }

            prFrcPrm->ucMultipleField = 1;
        }
        else if (prFrcPrm->ucRff)
        {
            // 2 Frame for PRGS_SEQ + REPEAT_1ST_Fld
            prFrcPrm->u4InStcPeriod *= 2;

            if (!prFrcPrm->ucProgressiveOut)
            {
                // although according to spec, it should play 4 vsync
                // but we only encounter streams that will error if we play 4 vsync, but will correct if we play 2 vsync,
                // this is the stream error, but we modify our code to cover the stream error

                prFrcPrm->u4InStcPeriod /= 2;
            }

            prFrcPrm->ucMultipleField = 1;
        }
        else
        {
            // 1 Frame for PRGS_SEQ
        }
    }
    else                                // Interlace Sequence
    {
        // According to 13818-2, repeat_first_field section
        if (prFrcPrm->ucRff)
        {
            if (prFrcPrm->ucProgressiveFrame == 0)  // Progressive Frame
            {
                // According to 13818-2, If Progressive Frame = 0, RFF shall be 0.
                LOG(3, "Warning! Progressive Frame = 0 and RFF = 1\n");
            }

            prFrcPrm->u4InStcPeriod *= 3;
            prFrcPrm->u4InStcPeriod /= 2;

            prFrcPrm->ucMultipleField = 1;
        }
    }
}

//-----------------------------------------------------------------------------
/** Brief of _VdpFrameRateProc.
 * reference 8105 vsv_sync.c (fgVsvFrameRateProc)
 * Frame Rate Convert Procedure
 * PS_FILM_XX, first number: target frame_no, 2nd number: combine frame_no
 */
//-----------------------------------------------------------------------------
static void _VdpFrameRateProc(B2R_PRM_T* prFrcPrm)
{
    UINT32 u4InStcPeriod;
    UINT32 u4OutStcPeriod;
    UINT32 u4OutVSyncNs;
    UINT32 u4InFrameStc;
    UINT32 u4OutFrameStc;
	#ifdef CC_SUPPORT_FULL_3D_PHOTO
    FBM_SEQ_HDR_T* prSeqHdr;
	#endif
    UINT32 u4StcDiffVSyncNs;

    if (prFrcPrm == NULL)
    {
        return;
    }

    u4InStcPeriod = prFrcPrm->u4InStcPeriod;
    u4OutStcPeriod = prFrcPrm->u4OutStcPeriod;

    if (u4OutStcPeriod == 0)
    {
        return;
    }
	#ifdef CC_SUPPORT_FULL_3D_PHOTO
    prSeqHdr = FBM_GetFrameBufferSeqHdr(prFrcPrm->ucFbgId);
	#endif
    u4OutVSyncNs = u4InStcPeriod / u4OutStcPeriod;

    u4InFrameStc = prFrcPrm->u4InFrameStc + u4InStcPeriod;
    u4OutFrameStc = prFrcPrm->u4OutFrameStc + (u4OutStcPeriod * u4OutVSyncNs);

    if (u4InFrameStc >= u4OutFrameStc)    // Check to see if we need to repeat?
    {
        u4StcDiffVSyncNs = (u4InFrameStc - u4OutFrameStc) / u4OutStcPeriod;

        if (u4StcDiffVSyncNs > 0)
        {
            if ((prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_30) ||
                    (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_25))        // Interlace Source
            {
                if (prFrcPrm->ucValid32Seq)    // 32 flag sequence valid, repeat at NOT RFF
                {
                    if (!prFrcPrm->ucRff)
                    {
                        // @쪺k
                        u4OutVSyncNs += u4StcDiffVSyncNs;
                    }
                }
                else    // Progressive/Interlace Output
                {
                    // @쪺k
                    u4OutVSyncNs += u4StcDiffVSyncNs;
                }
            }
            else                                                    // Progressive Source
            {
                // @쪺k
                u4OutVSyncNs += u4StcDiffVSyncNs;
            }
        }
    }
    else                                // Check to see if we need to skip?
    {
        // It is possible
        // At 30 >> 25 FRC case, we will output 2 2 1 2 2 1 ...
        // When this case integrate will deinterlace, we add a constraint "Don't skip 1st B's field (IBBPBB...)"
        // When we change back to 30, we will have (u4InFrameStc < u4OutFrameStc)

        LOG(3, "FRC OutFrameStc(%d) > InFrameStc(%d) \n", prFrcPrm->u4OutFrameStc, prFrcPrm->u4InFrameStc);

        while ((u4OutVSyncNs) && (u4OutFrameStc > u4InFrameStc))
        {
            u4OutFrameStc -= u4OutStcPeriod;
            u4OutVSyncNs--;
        }
    }

    //LOG(3,"u4OutVSyncNs = %d u4InStcPeriod = %d u4OutStcPeriod = %d u4InFrameStc = %d u4OutFrameStc = %d\n", u4OutVSyncNs,u4InStcPeriod,u4OutStcPeriod,u4InFrameStc,u4OutFrameStc);

    if (_fgTimeShift[0])
    {
        if ((_i4TimeShiftSpeed[0] == 200) && (prFrcPrm->fgDoubleClock == TRUE))
        {
            prFrcPrm->u2TargetNs = u4OutVSyncNs / 2;
        }
        else if (_i4TimeShiftSpeed[0] == 50)
        {
            prFrcPrm->u2TargetNs = u4OutVSyncNs * 2;
        }
        else
        {
            prFrcPrm->u2TargetNs = u4OutVSyncNs;
        }
    }
    else
    {
        prFrcPrm->u2TargetNs = u4OutVSyncNs;
    }

   #ifdef CC_SUPPORT_FULL_3D_PHOTO
	  LOG(9,"prSeqHdr->u2VSize=%d,prSeqHdr->u2HSize=%d,fg4K1K=%d\n",prSeqHdr->u2VSize,prSeqHdr->u2HSize,prSeqHdr->fg4K1K);
	  if(prSeqHdr->fg4K1K)
	  prFrcPrm->u2TargetNs = 2;
   #endif
    prFrcPrm->u2ChangeFieldNs = prFrcPrm->u2TargetNs;

    // Field Procedure
    // MPEG1 (Progressive Sequence = 1) ȮɺO Progressive Source
    prFrcPrm->ucInterlace2Interlace = 0;
    if (prFrcPrm->ucProgressiveOut)        // Progressive output
    {
        // Different with MT5351
        // I2P or P2P
        prFrcPrm->u2ChangeFieldNs = 0;
    }
    else                                // interlace output
    {
        // Different with MT5351
        // P2I or I2I

        prFrcPrm->ucWrongField = 0;

        if ((prFrcPrm->ucProgressiveSeqFlag) ||
                (prFrcPrm->ucValid32Seq))
        {
            // P2I, Top field first is don't care
        }
        else
        {
            if ((prFrcPrm->ucBottom == prFrcPrm->ucTff) &&
                    (prFrcPrm->ucStopAuto == 0) &&
                    ((prFrcPrm->ucLastRff == 0) && (prFrcPrm->ucRff == 0)))
            {

                if ((((prFrcPrm->ucInFrameRate ==FBM_FRAME_RATE_30) ||
                        (prFrcPrm->ucInFrameRate ==FBM_FRAME_RATE_60)) &&
                        (prFrcPrm->ucOutFrameRate ==FBM_FRAME_RATE_60)) ||
                        (((prFrcPrm->ucInFrameRate ==FBM_FRAME_RATE_25) ||
                          (prFrcPrm->ucInFrameRate ==FBM_FRAME_RATE_50)) &&
                         (prFrcPrm->ucOutFrameRate ==FBM_FRAME_RATE_50)))
                {
                    // switch to correct field, please consider fast change channel
#ifndef FBM_VIRTUAL_MPEG

                    if ((prFrcPrm->u2TargetNs & 1) != 1)
                    {
                        prFrcPrm->u2TargetNs++;
                        prFrcPrm->u2ChangeFieldNs = 0;
                        prFrcPrm->ucWrongField = 1;
                    }

                    LOG(3, "Wrong Field, Odd(%d) Tff(%d)\n", prFrcPrm->ucBottom, prFrcPrm->ucTff);
#endif
                }
                else
                {
                    LOG(7, "No TargetNs++\n");
                }
            }
        }
        prFrcPrm->u2ChangeFieldNs /= 2;
    }

    // local STC Accumulate
    prFrcPrm->u4InFrameStc += u4InStcPeriod;
    prFrcPrm->u4OutFrameStc += (u4OutStcPeriod * u4OutVSyncNs);

    if(prFrcPrm->t_wfd_prm.fgDblClk)
    {
        prFrcPrm->u2TargetNs *= 2;
    }
    
    // prevent overflow
    if ((prFrcPrm->u4InFrameStc > 0x80000000) &&
            (prFrcPrm->u4OutFrameStc > 0x80000000))
    {
        prFrcPrm->u4InFrameStc -= 0x80000000;
        prFrcPrm->u4OutFrameStc -= 0x80000000;
    }
}

void VDP_ResetPts(UCHAR ucB2rId)
{
    if (ucB2rId >= B2R_NS)
    {
        return;
    }
    _arVdpPrm[ucB2rId].u4LastPts = 0;
}
#if defined(CC_IPTV_SUPPORT) && defined(ENABLE_MULTIMEDIA)
BOOL _VDP_IPTVSkipAvSync(B2R_PRM_T* prFrcPrm)
{
    UCHAR ucB2rId = B2R_NS;
    UCHAR ucVdpId = VDP_NS;
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;
    UCHAR ucEsId = _prVdpConf[ucVdpId]->ucInputPort[0];

    ucB2rId = prFrcPrm ? prFrcPrm->ucB2rId : B2R_NS;
    ucVdpId = (ucB2rId < B2R_NS) ? VDP_GetVdpId(prFrcPrm->ucB2rId) : VDP_NS;

    if (ucVdpId >= VDP_NS)
    {
        return FALSE;
    }
    
    prVdecEsInfo = (ucEsId < MAX_ES_NS)? _VDEC_GetEsInfo(_prVdpConf[ucVdpId]->ucInputPort[0]) : NULL;

    if (prVdecEsInfo && (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_IPTV) && 
        ((_eSpeed[ucB2rId] == STC_SPEED_TYPE_FORWARD_2X) || (_eSpeed[ucB2rId] == STC_SPEED_TYPE_REWIND_2X)))
    {
        return TRUE;
    }

    return FALSE;

}
#endif

#ifdef VDP_AVS_ADJUST_STC
static VOID _VDP_SetAvsyncDelta(B2R_AVS_T* prAvs, UINT32 u4Delta,BOOL bAddFlg)
{
    if ( u4Delta < prAvs->u4Delay )
    {
        return;
    }

    if ( bAddFlg )
    {
        prAvs->i8Delta += (u4Delta - prAvs->u4Delay);
    }
    else
    {
        prAvs->i8Delta -= (u4Delta + prAvs->u4Delay);
    }

    return;
}

static VOID _VDP_SetDeltaInit(B2R_AVS_T* prAvs)
{
    prAvs->i8Delta = 0x0;
    prAvs->u4Delay = 30000;
}

#endif


//-----------------------------------------------------------------------------
/** Brief of _VdpAVSyncProc.
 * reference 8105 vsv_sync.c (fgVsvAVSyncProc)
 * Description : AV Sync procedure
 *  when AV difference < VIDEO_SYNC_THRESHOLD, no need skip/repeat
 *  if VIDEO_SYNC_THRESHOLD < AV difference < VIDEO_SYNC_RST_THRESHOLD, skip/repeat 1 frame
 *  if VIDEO_SYNC_RST_THRESHOLD < AV difference < VIDEO_WAIT_SYNC_THRESHOLD, skip/repeat 5 frame
 *  if AV difference > VIDEO_WAIT_SYNC_THRESHOLD, stall decode
 * *********************************************************************
 */
//-----------------------------------------------------------------------------
static void _VdpAVSyncProc(B2R_PRM_T* prFrcPrm)
{
#ifndef CC_VDP_FPGA

    UCHAR ucAvSyncMode;
    UCHAR ucStcSrc;
    UCHAR ucB2rId;
    UCHAR ucVdpId;
    
    if (prFrcPrm == NULL)
    {
        return;
    }
    ucB2rId = prFrcPrm->ucB2rId;
    if (ucB2rId >= B2R_NS)
    {
        LOG(0, "ucB2rId >= B2R_NS , oops \n");
        return;
    }
    ucVdpId = VDP_GetVdpId(ucB2rId);
    if (ucVdpId >= VDP_NS)
    {
        LOG(0, "ucVdpId >= VDP_NS , oops \n");
        return;
    }

    if (prFrcPrm->u4CbPtsEn)
    {
        if ((prFrcPrm->u4Pts >= prFrcPrm->u4CbPts) ||
                ((prFrcPrm->u4CbPts - prFrcPrm->u4Pts) > 0x80000000))    // normal & warp case
        {
            _VDP_VsyncNotify(VDP_MSG_CC_CB, (UINT32)ucVdpId, prFrcPrm->u4CbPts, prFrcPrm->u4CbArg);

            prFrcPrm->u4CbPtsEn = 0;
        }
        else
        {
            // call back protection
            if (prFrcPrm->ucReady)
            {
                // invalid PTS detection
                if ((prFrcPrm->u4CbLastPts != 0) &&
                        (prFrcPrm->u4Pts < prFrcPrm->u4CbLastPts) &&
                        ((prFrcPrm->u4CbLastPts - prFrcPrm->u4Pts) < 0x80000000))
                {
                    _VDP_VsyncNotify(VDP_MSG_CC_CB, (UINT32)ucVdpId, prFrcPrm->u4CbPts, (prFrcPrm->u4CbArg | VDP_CB_ERR));

                    prFrcPrm->u4CbPtsEn = 0;

                    LOG(5, "P2(%x)\n", prFrcPrm->u4CbPts);
                }
                else
                {
                    prFrcPrm->u4CbLastPts = prFrcPrm->u4Pts;
                }
            }
        }
    }


#ifdef __MODEL_slt__
    //SLT_CurrentPts(SLT_TYPE_VDO, prFrcPrm->u4Pts);
#endif

    if (prFrcPrm->u2TargetNs == 0)
    {
        return;
    }

    //we don't perform av sync in (timeshift && speed != 1x)
    if (_fgTimeShift[ucB2rId] && (_i4TimeShiftSpeed[ucB2rId] != 100))
    {
        return;
    }

    // We can do only 1 AVS-R per second
    if (prFrcPrm->u4PtsAvSyncNotAllow > 0)
    {
        prFrcPrm->u4PtsAvSyncNotAllow--;
    }

    FBM_GetSyncStc(prFrcPrm->ucFbgId, &ucAvSyncMode, &ucStcSrc);

    if (prFrcPrm->prFbCounter == NULL)
    {
        ASSERT(prFrcPrm->prFbCounter);
        return;
    }
    
#if defined(CC_IPTV_SUPPORT) && defined(ENABLE_MULTIMEDIA)
    if (_VDP_IPTVSkipAvSync(prFrcPrm))
    {
        return;
    }
#endif
    // skip AV Sync for Late Notify
    if (prFrcPrm->ucNewFbArrive != 0 && !_fgTimeShift[ucB2rId])
    {
        BOOL fgSeamless = FALSE;
        FBM_PIC_HDR_T* prPicHdr = NULL;
        if (prFrcPrm->ucFbgId != FBM_FBG_ID_UNKNOWN && prFrcPrm->ucFbId != FBM_FB_ID_UNKNOWN)
        {
            prPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, prFrcPrm->ucFbId);
            if (prPicHdr != NULL && prPicHdr->fgSeamlessDisp)
            {
                fgSeamless = TRUE;
            }
        }
#ifdef CC_3D_MM_DS_SUPPORT
        if (prFrcPrm->ucSubFbgId != FBM_FBG_ID_UNKNOWN && prFrcPrm->ucSubFbId != FBM_FB_ID_UNKNOWN)
        {
            if (FBM_IsSeamlessFB(prFrcPrm->ucSubFbgId, prFrcPrm->ucSubFbId))
            {
                fgSeamless = TRUE;
            }
        }
#endif

        if (fgSeamless)
        {
            // Because there are only 2 resize buffer in new seamless mode, it always trigger late notify...
            // We must have avsync in this case
        }
        else
        {
            return;
        }
    }

    if (prFrcPrm->ucProgressiveSeqFlag == 0)
    {
        // if drift > 3 secs then free run
        if (_u4MaxDriftCount[ucB2rId] >= 100)
        {
            // count valid
            if (STC_ChkStcStatusValid(ucStcSrc))
            {
                ++_u4ValidCount[ucB2rId];
            }
            else
            {
                _u4ValidCount[ucB2rId] = 0;
            }

            // if valid > 5 secs then restore the av sync
            if (_u4ValidCount[ucB2rId] >= 155)
            {
                _u4MaxDriftCount[ucB2rId] = 0;
            }

            return;
        }

        // count drift
        if (STC_ChkStcStatusDrift(ucStcSrc))
        {
            ++_u4DriftCount[ucB2rId];
        }
        else
        {
            _u4DriftCount[ucB2rId] = 0;
        }

        _u4MaxDriftCount[ucB2rId] = MAX(_u4MaxDriftCount[ucB2rId], _u4DriftCount[ucB2rId]);
    }
    if (ucAvSyncMode != VID_SYNC_MODE_NONE)
    {
        STC_CLOCK rStcClk;
        if (STC_GetSrc(ucStcSrc, &rStcClk) == STC_VALID)
        {
            UINT32 u4Delta;
            INT32 i4Delta;
            INT32 i4PtsDrift;
            UINT32 u4OutFrameStc;

            UINT32 u4ZeroPtsNs;
            UINT32 u4PtsDelta;
            UCHAR ucEsId;
            VDEC_ES_INFO_T* prVdecEsInfo;

            ucEsId = _prVdpConf[ucVdpId]->ucInputPort[0];
            prVdecEsInfo = (ucEsId < MAX_ES_NS)? _VDEC_GetEsInfo(ucEsId) : NULL;

#ifdef CC_VDP_FRC_DEBUG
            prFrcPrm->u4Stc = rStcClk.u4Base;
#endif
            if(!prFrcPrm->t_wfd_prm.fgDblClk)
            {
                u4OutFrameStc = (prFrcPrm->u2TargetNs * prFrcPrm->u4OutStcPeriod);
            }
            else
            {
                u4OutFrameStc = prFrcPrm->u4OutStcPeriod;
            }

#ifdef VDP_ADD_1_VSYNC_TIME
            // The actual display time is 1 Vsync later
            rStcClk.u4Base += prFrcPrm->u4OutStcPeriod;
#endif


#ifdef CC_B2R_WAIT_NPTV_STABLE
            //wait for NPTV stable
            //UCHAR ucPlayMode;
            FBM_GetPlayMode(prFrcPrm->ucFbgId,&_au1PlayMode[ucB2rId]);
            if ( (FBM_FBG_MM_MODE == _au1PlayMode[ucB2rId]) &&
#ifdef ENABLE_MULTIMEDIA
                    (prVdecEsInfo != NULL && prVdecEsInfo->eContainerType != SWDMX_FMT_MPEG2_TS_TIME_SHIFT) &&
                    (prVdecEsInfo != NULL && prVdecEsInfo->eMMSrcType != SWDMX_SRC_TYPE_WIFI_DISPLAY) &&
#endif
                    (prFrcPrm->ucReady ==1) && !(_afgNPTVStable[ucB2rId]) && !(prFrcPrm->fgB2R3DEnable))
            {
                BOOL fgSeamlessReady = FALSE;
                #ifdef CC_SUPPORT_NPTV_SEAMLESS 
                if (FBM_ChkSeamlessMode(prFrcPrm->ucFbgId, SEAMLESS_BY_NPTV))
                {
                    fgSeamlessReady = FALSE;
                    if ((prFrcPrm->ucSeamlessVDPMode == VDP_SMLS_PREPARE_WHILE_START_PLAY) && bVRMReadyForB2R(ucVdpId))
                    {
                        _VDP_NPTVSeamlessStatus(prFrcPrm, VDP_SMLS_READY_WHILE_START_PLAY);
                        fgSeamlessReady = TRUE;
                    }
                    else if (prFrcPrm->ucSeamlessVDPMode == VDP_SMLS_READY_WHILE_START_PLAY)
                    {
                        fgSeamlessReady = TRUE;
                    }
                }
                else
                #endif
                {
                    fgSeamlessReady = TRUE;
                }

                if ((_u4DrvVideoGetMute(ucVdpId) || !fgSeamlessReady) && (prFrcPrm->ucFbId !=FBM_FB_ID_UNKNOWN))
                {
                    FBM_SetFrameBufferGlobalFlag(prFrcPrm->ucFbgId, FBM_FLAG_FB_NO_TIMEOUT);

                    _afgNPTVStable[ucB2rId] = FALSE;

                    _au4WaitNPTVStableCount[ucB2rId]++;

                    // pending for the 1st PTS
                    _VdpPreparePendingForNextVsync(prFrcPrm);

                    if (_arPendingForSTC[ucB2rId])
                    {
                        _VDP_HalSetBlack(ucB2rId, TRUE);
                    }

                    LOG(5, "VDP(%d)----Pending for wating NPTV stable %d\n",
                        ucVdpId, _au4WaitNPTVStableCount[ucB2rId]);

                    return;
                }
                else if (!_u4DrvVideoGetMute(ucVdpId) && !_afgNPTVStable[ucB2rId] && fgSeamlessReady)
                {
                    // LOG(3,"wait %d vsyncs for NPTV statlbe\n",_au4WaitNPTVStableCount[prFrcPrm->ucVdpId]);
                    if (_arPendingForSTC[ucB2rId])
                    {
                        if (rStcClk.u4Base + prFrcPrm->u4OutStcPeriod < prFrcPrm->u4Pts)
                        {
                            // pending for the 1st PTS
                            _VdpPreparePendingForNextVsync(prFrcPrm);

                            LOG(2, "%d)----Waiting for STC:x%X Frm:0x%X Prd:0x%X\n",
                                ucVdpId, rStcClk.u4Base,
                                prFrcPrm->u4Pts, prFrcPrm->u4OutStcPeriod);

                            return;
                        }

                        _VDP_HalSetBlack(ucB2rId, FALSE);
                        _arPendingForSTC[ucB2rId] = FALSE;
                    }
                    else
                    {
#ifdef CC_53XX_SWDMX_V2
                        STC_StopStc(ucStcSrc);
                        STC_SetStcValue(ucStcSrc,(UINT32)(prFrcPrm->u4Pts));
                        rStcClk.u4Base = (UINT32)(prFrcPrm->u4Pts);
                        STC_StartStc(ucStcSrc);
#else
                        STC_StopStc();
                        STC_SetStcValue((UINT32)(prFrcPrm->u4Pts));
                        rStcClk.u4Base = (UINT32)(prFrcPrm->u4Pts);
                        STC_StartStc();
#endif
                        rStcClk.u4Base = prFrcPrm->u4Pts;
                    }

                    _au4WaitNPTVStableCount[ucB2rId] =0;
                    _afgNPTVStable[ucB2rId] = TRUE;
                    #ifdef CC_SUPPORT_NPTV_SEAMLESS 
                    if (prFrcPrm->ucSeamlessVDPMode)
                    {
                        _VDP_NPTVSeamlessStatus(prFrcPrm, VDP_SMLS_FRAME_COMING);
                    }
                    #endif

                    //LOG(3,"wait %d vsyncs for NPTV statlbe\n",_au4WaitNPTVStableCount[prFrcPrm->ucVdpId]);

                    FBM_ClrFrameBufferGlobalFlag(prFrcPrm->ucFbgId, FBM_FLAG_FB_NO_TIMEOUT);
#ifdef CC_B2R_WAIT_NPTV_STABLE
                    FBM_SetFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_DISPLAYING);
#endif
                    LOG(3, "%d)----ready for waiting NPTV stable\n", ucB2rId);
                }
            }
            else if (prFrcPrm->fgB2R3DEnable
#ifdef ENABLE_MULTIMEDIA
                     || (prVdecEsInfo != NULL && prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS_TIME_SHIFT)
                     || (prVdecEsInfo != NULL && prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_WIFI_DISPLAY)
#endif
                    )
            {
                _afgNPTVStable[ucB2rId] = TRUE;
                FBM_SetFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_DISPLAYING);
            }
#endif
#ifdef ENABLE_MULTIMEDIA
            if (prVdecEsInfo && (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_WIFI_DISPLAY))
            {
                _VDP_RTAVsyncProc(prFrcPrm);
                STC_GetSrc(ucStcSrc, &rStcClk);
            }
#endif
            // Zero PTS
            u4ZeroPtsNs = prFrcPrm->u4ZeroPtsNs;
            if (prFrcPrm->u4Pts == 0)
            {
                LOG(9, "Zero PTS (%d) Delta(%d)\n", prFrcPrm->u4ZeroPtsNs, prFrcPrm->u4PtsDelta);

                prFrcPrm->u4ZeroPtsNs++;

                if (prFrcPrm->ucReady == 0)
                {
                    if ((ucAvSyncMode == VID_SYNC_MODE_SLAVE_MM)
                            || (ucAvSyncMode == VID_SYNC_MODE_MASTER) /* 2x Trick Play */)
                    {
                        // MM playback receive 1st PTS as 0, Start to Play
                        LOG(3, "MM playback receive 1st PTS as 0, Start to Play\n");
                        prFrcPrm->u4PtsAsyncNs = 0;
                    }
                    else
                    {
                        if (prFrcPrm->u4PtsAsyncNs < VDP_WAIT_NONZERO_PTS_NS)
                        {
                            // pending for the 1st PTS
                            prFrcPrm->u2TargetNs = 0;
                            prFrcPrm->u2ChangeFieldNs = 0;

                            prFrcPrm->u4InFrameStc = 0;
                            prFrcPrm->u4OutFrameStc = 0;

                            // drop this frame, wait for another frame with PTS != 0
                            /*
                                                        prFrcPrm->ucPendingFbId = prFrcPrm->ucFbId;
                                                        prFrcPrm->ucLastFbId = FBM_FB_ID_UNKNOWN;
                                                        prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
                            */
                            prFrcPrm->u4PtsAsyncNs++;
                        }
                        else
                        {
                            // STC still invalid after 12 vsync, forget it, go display!
                            LOG(3, "Force Video Start 1\n");
                            prFrcPrm->u4PtsAsyncNs = 0;
                        }
                    }

                    prFrcPrm->prFbCounter->u4SyncMode = FBM_FBG_AVSYNC_NO_PTS;
                    FBM_SetAVSyncStatus(prFrcPrm->ucFbgId, prFrcPrm->prFbCounter->u4SyncMode);
                    return;

                }
                else
                {
#if 0
                    if ((ucAvSyncMode == VID_SYNC_MODE_SLAVE_MM)
                            || (ucAvSyncMode == VID_SYNC_MODE_MASTER) /* 2x Trick Play */)
                    {
                        //Calculate PTS for no PTS picture when trick mode

                        UINT32 u4Diff;

                        if (prFrcPrm->u4LastPts == 0)
                        {
                            //prFrcPrm->u4Pts = rStcClk.u4Base;
                            // Pibben: Why use STC value at above calculation?
                            prFrcPrm->u4Pts = 0;
                        }
                        else
                        {
                            u4Diff = 0xFFFFFFFF - prFrcPrm->u4LastPts;

                            if (u4Diff >= u4OutFrameStc)
                            {
                                prFrcPrm->u4Pts = prFrcPrm->u4LastPts + u4OutFrameStc;
                            }
                            else
                            {
                                prFrcPrm->u4Pts = (u4OutFrameStc - u4Diff);
                            }
                            if (prFrcPrm->ucRealChangeFb == 0)
                            {
                                /* MM pending for last frame, no PTS accumulation */
                                prFrcPrm->u4Pts = prFrcPrm->u4LastPts;
                                LOG(9, "MM pending for last frame, no PTS accumulation\n");
                            }
                        }

                        LOG(9, "AVS-C PTS (0x%x)\n", prFrcPrm->u4Pts);
                        prFrcPrm->u4ZeroPtsNs = 0;

                        prFrcPrm->u4KeepDropB = 0;

                        // Backup last PTS
                        prFrcPrm->u4LastPts = prFrcPrm->u4Pts;

                    }
                    else if (ucAvSyncMode == VID_SYNC_MODE_SLAVE)
#endif
                    {
                        // Handle Keep Drop B
                        if (prFrcPrm->u4KeepDropB != 0)
                        {
                            LOG(3, "AVS-D VLD Zero PTS MPV Q(%d)\n", _prDbase->rMpv[0].u4EsmQCnt);

                            FBM_SetFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_SKIP_PIC_NO_PTS);
                        }

                        prFrcPrm->prFbCounter->u4SyncMode = FBM_FBG_AVSYNC_NO_PTS;
                        FBM_SetAVSyncStatus(prFrcPrm->ucFbgId, prFrcPrm->prFbCounter->u4SyncMode);
                        return;
                    }
                }
            }
            else // if current pts !=0 , calculate difference between current frame and last frame
            {
#if 0
                if (((prFrcPrm->u4CbSyncEn) && (prFrcPrm->u4CbSyncDone == 0)) ||
                        (_rVdpLipSync.ucNg2OkCbEn == 1) ||
                        (_rVdpLipSync.ucOk2NgCbEn == 1))
                {
                    UINT32 u4AVDiff;
                    //STC_CLOCK rClock;

#if 0
                    STC_GetAudioPts(&rClock);


                    if (rClock.u4Base >= prFrcPrm->u4Pts)
                    {
                        u4AVDiff = rClock.u4Base - prFrcPrm->u4Pts;
                    }
                    else
                    {
                        u4AVDiff = prFrcPrm->u4Pts - rClock.u4Base;
                    }

                    if (u4AVDiff >= 0x80000000)
                    {
                        // handle wrap around
                        u4AVDiff = (0xFFFFFFFF - u4AVDiff);

                    }

                    LOG(1," (0x%x, 0x%x, %d)\n", rClock.u4Base, prFrcPrm->u4Pts, u4AVDiff);

#else

                    if (rStcClk.u4Base >= prFrcPrm->u4Pts)
                    {
                        u4AVDiff = rStcClk.u4Base - prFrcPrm->u4Pts;
                    }
                    else
                    {
                        u4AVDiff = prFrcPrm->u4Pts - rStcClk.u4Base;
                    }

                    if (u4AVDiff >= 0x80000000)
                    {
                        // handle wrap around
                        u4AVDiff = (0xFFFFFFFF - u4AVDiff);

                    }

                    LOG(9," (0x%x, 0x%x, Diff: %d)\n", rStcClk.u4Base, prFrcPrm->u4Pts, u4AVDiff);

#endif

                    if ((prFrcPrm->u4CbSyncEn) && (prFrcPrm->u4CbSyncDone == 0))
                    {
                        if (u4AVDiff < prFrcPrm->u4CbSyncThrsd)
                        {
                            prFrcPrm->u4CbSyncFrmCnt++;

                            if (prFrcPrm->u4CbSyncFrmCnt >= prFrcPrm->u4CbSyncChkFrmCnt)
                            {
                                _VDP_VsyncNotify(VDP_MSG_LIPSYNC_CB, prFrcPrm->ucVdpId, 0, 0);
                                LOG(9, "LIP Sync callback\n");
                                prFrcPrm->u4CbSyncDone = 1;
                            }
                        }
                        else
                        {
                            prFrcPrm->u4CbSyncFrmCnt = 0;
                        }
                    }

                    if ((_rVdpLipSync.ucLipSyncState == VDP_LIP_SYNC_NG) &&
                            (_rVdpLipSync.ucNg2OkCbEn == 1))
                    {
                        /* _rVdpLipSync.u4OKThrd should equal to _rVdpLipSync.u4NGThrd? */
                        if (u4AVDiff <= _rVdpLipSync.u4OKThrd)
                        {
                            if (_rVdpLipSync.u4OKFrmCnt < _rVdpLipSync.u4NumFrmsOK)
                            {
                                _rVdpLipSync.u4OKFrmCnt++;
                            }
                            else
                            {
                                _rVdpLipSync.ucLipSyncState = VDP_LIP_SYNC_OK;
                                _rVdpLipSync.u4NGFrmCnt = 0;
                                /* Lip Sync state transition callback */
                                _VDP_VsyncNotify(VDP_MSG_LIPSYNC_NG_TO_OK_CB, prFrcPrm->ucVdpId, 0, 0);
                                LOG(0, "Lip Sync state transition to OK callback\n");
                            }
                        }
                        else
                        {
                            _rVdpLipSync.u4OKFrmCnt = 0;
                        }
                    }
                    else if ((_rVdpLipSync.ucLipSyncState == VDP_LIP_SYNC_OK) &&
                             (_rVdpLipSync.ucOk2NgCbEn == 1))
                    {
                        if (u4AVDiff > _rVdpLipSync.u4NGThrd)
                        {
                            if (_rVdpLipSync.u4NGFrmCnt < _rVdpLipSync.u4NumFrmsNG)
                            {
                                _rVdpLipSync.u4NGFrmCnt++;
                            }
                            else
                            {
                                _rVdpLipSync.ucLipSyncState = VDP_LIP_SYNC_NG;
                                _rVdpLipSync.u4OKFrmCnt = 0;
                                /* Lip Sync state transition callback */
                                _VDP_VsyncNotify(VDP_MSG_LIPSYNC_OK_TO_NG_CB, prFrcPrm->ucVdpId, 0, 0);
                                LOG(0, "Lip Sync state transition to NG callback\n");
                            }
                        }
                        else
                        {
                            _rVdpLipSync.u4NGFrmCnt = 0;
                        }
                    }

                }
#else
                //if (/*(prFrcPrm->u4CbSyncEn) && */(prFrcPrm->u4CbSyncDone == 0))
                {
                    //if (u4AVDiff < prFrcPrm->u4CbSyncThrsd)
                    {
                        //prFrcPrm->u4CbSyncFrmCnt++;

                        //if (prFrcPrm->u4CbSyncFrmCnt >= prFrcPrm->u4CbSyncChkFrmCnt)
                        {
                            _VDP_VsyncNotify(VDP_MSG_LIPSYNC_CB, ucVdpId, 0, 0);
                            //LOG(11, "LIP Sync callback\n");
                            //prFrcPrm->u4CbSyncDone = 1;
                        }
                    }
                    //else
                    //{
                    //    prFrcPrm->u4CbSyncFrmCnt = 0;
                    //}
                }
#endif

                prFrcPrm->u4ZeroPtsNs = 0;

                if (u4ZeroPtsNs != 0)
                {
                    prFrcPrm->u4KeepDropB = 0;
                }

                if (prFrcPrm->u4LastPts != 0)
                {
                    // Calculate PTS Delta
                    if (prFrcPrm->u4Pts > prFrcPrm->u4LastPts)
                    {
                        u4PtsDelta = (prFrcPrm->u4Pts - prFrcPrm->u4LastPts);
                    }
                    else
                    {
                        // handle wrap around
                        u4PtsDelta = (0xFFFFFFFF - prFrcPrm->u4LastPts);
                        u4PtsDelta += prFrcPrm->u4Pts;
                    }

                    if (prFrcPrm->u4PtsDelta != 0)
                    {
                        prFrcPrm->u4PtsDelta -= (prFrcPrm->u4PtsDelta / VDP_MAX_PTS_FILTER_LEN);
                        prFrcPrm->u4PtsDelta += (u4PtsDelta / VDP_MAX_PTS_FILTER_LEN);
                    }
                    else
                    {
                        // Filter initial
                        prFrcPrm->u4PtsDelta = u4PtsDelta;
                    }
                }

                // Backup last PTS
                prFrcPrm->u4LastPts = prFrcPrm->u4Pts;
            }

            //calcus difference between current pts and STC
            if (prFrcPrm->u4Pts > rStcClk.u4Base)
            {
                u4Delta = (prFrcPrm->u4Pts - rStcClk.u4Base);

                if (u4Delta < 0x80000000)
                {
                    i4Delta = - (INT32) u4Delta;
                }
                else
                {
                    // handle wrap around
                    u4Delta = (0xFFFFFFFF - u4Delta);
                    i4Delta = (INT32) u4Delta;

                    // [LOG] AVSYNC, V-PTS, Wrap around
                    SYSLOG(SYNC_PREFIX + 98, 20 + prFrcPrm->ucFbgId, prFrcPrm->u4Pts, rStcClk.u4Base);
                }
            }
            else
            {
                u4Delta = (rStcClk.u4Base - prFrcPrm->u4Pts);

                if (u4Delta < 0x80000000)
                {
                    i4Delta = (INT32) u4Delta;
                }
                else
                {
                    // handle wrap around
                    u4Delta = (0xFFFFFFFF - u4Delta);
                    i4Delta = - (INT32) u4Delta;

                    // [LOG] AVSYNC, V-PTS, Wrap around
                    SYSLOG(SYNC_PREFIX + 98, prFrcPrm->ucFbgId, prFrcPrm->u4Pts, rStcClk.u4Base);
                }
            }

#ifdef CC_VDP_FRC_DEBUG
            prFrcPrm->i4PtsStcDelta = i4Delta;
#endif

#ifdef CC_GET_PCR_PTS_INFO
            _prDbase->rPts.u4VideoPts = prFrcPrm->u4Pts;
            _prDbase->rPts.u4VideoPtsStcDelta = u4Delta;
            _prDbase->rPts.i4VideoPtsStcDelta = i4Delta;

            if (u4Delta >= 27000)     // 300ms
            {
                _prDbase->rPts.fgVideoSync = FALSE;
            }
            else
            {
                _prDbase->rPts.fgVideoSync = TRUE;
            }
#endif

            LOG_REL(_fgEnableVideoLog, 1, 7, "B2R(%d) PTS %x, STC(%d) %x, Delta (%d, %d), %d\n",
                    prFrcPrm->ucB2rId,
                    prFrcPrm->u4Pts, ucStcSrc, rStcClk.u4Base, i4Delta, prFrcPrm->i4PtsDrift,
                    u4OutFrameStc);

#ifdef CC_GET_PCR_PTS_INFO
            if (prFrcPrm->ucReady != 0)
            {
                HAL_TIME_T rTime;
                HAL_TIME_T rDeltaTime;

                HAL_GetTime(&rTime);
                HAL_GetDeltaTime(&rDeltaTime, &(prFrcPrm->rLastAvSyncTime), &rTime);

                // check if AV sync lost for a long time
                if ((rDeltaTime.u4Seconds >= (10 + 5)) &&
                        (u4Delta > VDP_INVALID_DRIFT))
                {
                    // try to sync video PTS to audio PTS
                    LOG(4, "V2A Delta(%d)\n", u4Delta);

                    if (prFrcPrm->u4Pts > _prDbase->rPts.u4AudioPts)
                    {
                        u4Delta = (prFrcPrm->u4Pts - _prDbase->rPts.u4AudioPts);

                        if (u4Delta < 0x80000000)
                        {
                            i4Delta = - (INT32) u4Delta;
                        }
                        else
                        {
                            // handle wrap around
                            u4Delta = (0xFFFFFFFF - u4Delta);
                            i4Delta = (INT32) u4Delta;
                        }
                    }
                    else
                    {
                        u4Delta = (_prDbase->rPts.u4AudioPts - prFrcPrm->u4Pts);

                        if (u4Delta < 0x80000000)
                        {
                            i4Delta = (INT32) u4Delta;
                        }
                        else
                        {
                            // handle wrap around
                            u4Delta = (0xFFFFFFFF - u4Delta);
                            i4Delta = - (INT32) u4Delta;
                        }
                    }
                }
            }
            else
            {
                // 1st picture after change channel

#if 1
                // solution #2
#else

                // solution #1

                if (u4Delta > VDP_INVALID_DRIFT)
                {
                    UINT32 u4WaitThreshold = 0;

                    LOG(4, "1st V-Drift A-PTS %x, WAIT(%d)\n", _prDbase->rPts.u4AudioPts, prFrcPrm->u4PtsAsyncNs);

                    switch (_prVdpConf[prFrcPrm->ucVdpId]->rOutInfo.ucPmxMode)
                    {
                    case VDP_B2R_MODE_480I:
                    case VDP_B2R_MODE_480P:
                    case VDP_B2R_MODE_576I:
                    case VDP_B2R_MODE_576P:
                        u4WaitThreshold = 30;
                        break;

                    case VDP_B2R_MODE_720P:
                        u4WaitThreshold = 24;
                        break;

                    case VDP_B2R_MODE_1080I:
                    default:
                        u4WaitThreshold = 16;
                        break;
                    }

                    if (prFrcPrm->u4PtsAsyncNs < u4WaitThreshold)
                    {
                        // pending for the 1st PTS
                        prFrcPrm->u2TargetNs = 0;
                        prFrcPrm->u2ChangeFieldNs = 0;

                        prFrcPrm->u4InFrameStc = 0;
                        prFrcPrm->u4OutFrameStc = 0;

                        prFrcPrm->ucPendingFbId = prFrcPrm->ucFbId;
                        prFrcPrm->ucLastFbId = FBM_FB_ID_UNKNOWN;
                        prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;

                        prFrcPrm->u4PtsAsyncNs++;
                    }
                }
#endif
            }
#endif

            // handle PTS drift
#ifdef CC_VDO_PTS_FILTER

            // do not handle PTS drift while Fast Channel Changing
#ifdef CC_FAST_CHANGE_CHANNEL
            if (prFrcPrm->ucFastChangeEn == 0)
            {
#endif

                // Handle Keep Drop B
                if (prFrcPrm->u4KeepDropB != 0)
                {
                    if ((i4Delta > 0) && (u4Delta > u4OutFrameStc))
                    {
                        FBM_SetFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_SKIP_PIC);
#if (defined(CC_3D_MM_DS_SUPPORT) && defined(CC_B2R_ENABLE_R_VIEW_SKIP_FRAME))
                        if (prFrcPrm->ucSubFbgId != FBM_FBG_ID_UNKNOWN && FBM_GetFbgAppMode(prFrcPrm->ucSubFbgId) == FBM_FBG_APP_Frame_Pack_3D)
                        {
                            FBM_SetFrameBufferFlag(prFrcPrm->ucSubFbgId, FBM_FLAG_SKIP_PIC);
                        }
#endif
                    }
                    else
                    {
                        prFrcPrm->u4KeepDropB = 0;
                    }
                }

#ifndef CC_B2R_DISABLE_PTS_IIR
                if ((prFrcPrm->ucReady) &&
                        (_arVdpDtvFreeze[ucB2rId].u4UnFreeze == 0) &&
                        (u4ZeroPtsNs == 0) && (ucAvSyncMode == VID_SYNC_MODE_SLAVE))
                {
                    if (prVdecEsInfo != NULL)
                    {
#ifdef ENABLE_MULTIMEDIA
                        if (!((prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS_TIME_SHIFT) && (prFrcPrm->fgReleaseDispQ))
                            && prVdecEsInfo->eMMSrcType != SWDMX_SRC_TYPE_WIFI_DISPLAY)
#endif
                        {
                            i4PtsDrift = i4Delta - prFrcPrm->i4PtsDrift;
                            prFrcPrm->i4PtsDrift += (i4PtsDrift / VDP_MAX_PTS_FILTER_LEN);

                            if ((i4PtsDrift > VDP_MAX_PTS_DRIFT) ||
                                    (i4PtsDrift < -VDP_MAX_PTS_DRIFT))
                            {
                                LOG(7, "PTS Jitter! (%d, %d, %d)\n", i4PtsDrift, i4Delta, prFrcPrm->i4PtsDrift);

                                prFrcPrm->prFbCounter->u4SyncMode = FBM_FBG_AVSYNC_PTS_JITTER;
                                FBM_SetAVSyncStatus(prFrcPrm->ucFbgId, prFrcPrm->prFbCounter->u4SyncMode);
                                if (prFrcPrm->ucWrongField && (prFrcPrm->u2TargetNs&1))
                                {
                                    prFrcPrm->i4PtsDrift += prFrcPrm->u4OutStcPeriod;
                                }

                                return;
                            }
                            else
                            {
                                UNUSED(prVdecEsInfo);
                                prFrcPrm->fgReleaseDispQ = FALSE;
                            }
                        }
                    }
                    else
                    {
                        LOG(5,"_VdpAVSyncProc : prVdecEsInfo == NULL\n");
                    }
                }
                else
#else
                UNUSED(prVdecEsInfo);
                UNUSED(i4PtsDrift);
#endif
                {
                    if (_arVdpDtvFreeze[ucB2rId].u4UnFreeze != 0)
                    {
                        prFrcPrm->u4PtsAvSyncNotAllow = 0;
                        _arVdpDtvFreeze[ucB2rId].u4UnFreeze = 0;
                    }

                    prFrcPrm->i4PtsDrift = i4Delta;
                }

#ifdef CC_FAST_CHANGE_CHANNEL
            }
#endif

#else
            UNUSED(i4PtsDrift);
#endif

            if (ucAvSyncMode == VID_SYNC_MODE_SLAVE)
            {
                if ((prFrcPrm->ucReady != 0) && (u4Delta > VDP_INVALID_DRIFT))
                {
                    // Bad Edit Program
                    prFrcPrm->u4KeepDropB = 0;
                    prFrcPrm->prFbCounter->u4SyncMode = FBM_FBG_AVSYNC_PTS_JITTER;
                    FBM_SetAVSyncStatus(prFrcPrm->ucFbgId, prFrcPrm->prFbCounter->u4SyncMode);
                    return;
                }
            }

            HAL_GetTime(&(prFrcPrm->rLastAvSyncTime));

            if (i4Delta < 0)
            {
                // PTS slower than STC, repeat
                // [LOG] AVSYNC, V-PTS slower than STC
//                SYSLOG(SYNC_PREFIX + prFrcPrm->ucFbgId, rStcClk.u4Base, u4Delta, prFrcPrm->i4TemporalRef);

                prFrcPrm->prFbCounter->u4SyncMode = FBM_FBG_AVSYNC_NORMAL;
                FBM_SetAVSyncStatus(prFrcPrm->ucFbgId, prFrcPrm->prFbCounter->u4SyncMode);

                if (u4Delta > u4OutFrameStc && ucB2rId<B2R_NS)
                {
                    UINT32 u4Step;
                    UINT32 u4TargetNs;

                    if (u4OutFrameStc != 0)
                    {
                        _arVdpPrm[ucB2rId].u4B2rSkipFrameNo = u4Delta / u4OutFrameStc;
                    }
                    else
                    {
                        _arVdpPrm[ucB2rId].u4B2rSkipFrameNo = 0;
                    }
                    prFrcPrm->prFbCounter->u4SyncMode = FBM_FBG_AVSYNC_REPEAT;
                    FBM_SetAVSyncStatus(prFrcPrm->ucFbgId, prFrcPrm->prFbCounter->u4SyncMode);

#ifndef CC_B2R_DISABLE_ONE_SECOND_FRC
                    // protection
                    if ((ucAvSyncMode == VID_SYNC_MODE_SLAVE) && (prFrcPrm->u4PtsAvSyncNotAllow))
                    {
                        return;
                    }
#endif

                    if (prFrcPrm->ucReady == 0)
                    {
#ifdef VDP_EARLY_DISPLAY

                        if (u4Delta > VDP_EARLY_DISP_DRIFT && u4Delta < VDP_INVALID_DRIFT)
                        {
                            // pending for the 1st PTS
                            _VdpPreparePendingForNextVsync(prFrcPrm);

#ifdef VDP_AVS_ADJUST_STC
                            _VDP_SetAvsyncDelta(&_rB2rAvs,u4Delta,TRUE);
#endif
                            LOG(3, "----Pending for the 1st PTS\n");


                            return;
                        }
                        else if (u4Delta > VDP_INVALID_DRIFT)
                        {
                            // Bad Edit Program
                            LOG(3, "Force Video Start 999 for bad edit bitstream\n");
                            prFrcPrm->u4PtsAsyncNs = 0;
                            return ;
                        }

                        else if (_u2VsyncNotStableCount[ucB2rId] > 0)
                        {
                            if (_u2VsyncNotStableCount[ucB2rId] == 0xFF)
                            {
                                _u2VsyncNotStableCount[ucB2rId] = prFrcPrm->u2TargetNs;
                            }

                            // pending for the 1st PTS
                            _VdpPreparePendingForNextVsync(prFrcPrm);

                            _u2VsyncNotStableCount[ucB2rId]--;
                            LOG(9, "VSYNC not stable (%d)!\n", _u2VsyncNotStableCount[ucB2rId]);
                            return;
                        }
                        _prVdpConf[ucVdpId]->ucEarlyDisp = 1;
                        LOG(3, "ucEarlyDisp VDP(%d)\n", ucVdpId);
#ifdef TIME_MEASUREMENT
                       fgB2REarlyDisp =TRUE;
#endif
                        //if (!prFrcPrm->fgSeqChgSpeedup)
                        {
#ifdef VDP_MUTE_EARLY_DISPLAY
                            _vDrvVideoSetMute(MUTE_MODULE_DTV, ucVdpId,
                                              FOREVER_MUTE, FALSE);
                            LOG(3, "DTV Mute Early Display -1 VDP(%d)\n", ucVdpId);

#ifdef CC_SUPPORT_TVE
                            if ((_prVdpConf[VDP_1]->ucB2rId == B2R_1) &&
                                    (_prVdpConf[VDP_2]->ucB2rId == B2R_1)&&
                                    (_prVdpConf[VDP_1]->ucEnable == 1) &&
                                    (_prVdpConf[VDP_2]->ucEnable == 1) )
                            {
                                _prVdpConf[1-ucVdpId]->ucEarlyDisp = 1;
                                LOG(3, "ucEarlyDisp VDP(%d)\n", (1-ucVdpId));

                                _vDrvVideoSetMute(MUTE_MODULE_DTV, (1 - ucVdpId),
                                                  FOREVER_MUTE, FALSE);
                                LOG(3, "DTV Mute Early Display -2 VDP(%d)\n", ucVdpId);
                            }
#endif
#else // VDP_MUTE_EARLY_DISPLAY
                            LOG(0, "WARNING: DTV mute early display is disabled \n");
#endif
                        }
                        u4Step = 1 + (u4Delta / u4OutFrameStc);

                        prFrcPrm->i4PtsDrift += (INT32)(u4OutFrameStc * (u4Step - 1));

                        if (u4Step > 1)
                        {
                            prFrcPrm->u2RestoreNs = prFrcPrm->u2TargetNs;
                        }

                        prFrcPrm->u2TargetNs *= u4Step;

                        if (prFrcPrm->u2TargetNs != 2)
                        {
                            prFrcPrm->u2ChangeFieldNs = 0;
                        }

                        if (prFrcPrm->ucProgressiveOut)
                        {
                            prFrcPrm->u4WaitUnmute = (prFrcPrm->u2TargetNs + VDP_VIDEO_DELAY_VSYNC) - 2;
                        }
                        else
                        {
                            prFrcPrm->u4WaitUnmute = prFrcPrm->u2TargetNs + VDP_VIDEO_DELAY_VSYNC;
                        }

                        if (prFrcPrm->u4WaitUnmute >= VDP_WAIT_UNMUTE_VSYNC)
                        {
                            prFrcPrm->u4WaitUnmute = VDP_WAIT_UNMUTE_VSYNC;
                        }

                        LOG(5, "-----Wait Unmute VSync(%d)  PTS(0x%x) \n", prFrcPrm->u4WaitUnmute, prFrcPrm->u4Pts);
#ifdef TIME_MEASUREMENT
                        //u4ReptCnt++;
#endif
                        LOG(3, "AVS-R B2R(%d) Fbg(%d) %d PTS(0x%x) STC(0x%x)\n", prFrcPrm->ucB2rId, prFrcPrm->ucFbgId, prFrcPrm->u2TargetNs, prFrcPrm->u4Pts, rStcClk.u4Base);

                        return;

#else
#ifdef CC_FAST_CHANGE_CHANNEL
                        UINT32 u4Ahead;
                        u4Ahead = STC_GetFastChangeChangeAhead(ucStcSrc);

                        if (u4Ahead > u4Delta)
                        {
                            if (prFrcPrm->ucInterlace2Interlace)
                            {
                                if (prFrcPrm->ucWrongField == 0)
                                {
                                    // Playback single field only for FRC
                                    prFrcPrm->u2TargetNs++;
                                    prFrcPrm->u2ChangeFieldNs = 0;
                                }
                            }
                            else
                            {
                                prFrcPrm->u2TargetNs++;
                            }

                            prFrcPrm->ucFastChangeEn = 1;

                            LOG(3, "Enable Fast Start PTS %x, STC %x, Delta (%d) Ahead(%d)\n", prFrcPrm->u4Pts, rStcClk.u4Base, i4Delta, u4Ahead);

#ifdef CC_FAST_CHANGE_CHANNEL_LOG
                            HAL_GetTime(&rNvgtVideoStartTime);
                            u4NvgtTimeAhead = u4Ahead;
#endif
                        }
                        else
                        {
#endif

                            if (FBM_ChkFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_ES_FIFO_FULL))
                            {
                                // ES FIFO full, can not wait anymore
                                LOG(3, "Force Video Start 2\n");
                                prFrcPrm->u4PtsAsyncNs = 0;
                            }
                            else if (u4Delta > VDP_INVALID_DRIFT)
                            {
                                // Bad Edit Program
                                LOG(3, "Force Video Start 4\n");
                                prFrcPrm->u4PtsAsyncNs = 0;
                            }
                            else
                            {
                                // pending for the 1st PTS
                                _VdpPreparePendingForNextVsync(prFrcPrm);
                            }

                            return;

#ifdef CC_FAST_CHANGE_CHANNEL
                        }
#endif
#endif
                    }

                    // ES FIFO FULL, don't repeat anymore
                    if (FBM_ChkFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_ES_FIFO_FULL))
                    {
                        return;
                    }

                    if (prFrcPrm->ucReady == 1)
                    {
#ifdef CC_FAST_CHANGE_CHANNEL
                        if (prFrcPrm->ucFastChangeEn == 1)
                        {
                            if (prFrcPrm->ucInterlace2Interlace)
                            {
                                if (prFrcPrm->ucWrongField == 0)
                                {
                                    // Playback single field only for FRC
                                    prFrcPrm->u2TargetNs++;
                                    prFrcPrm->u2ChangeFieldNs = 0;
                                }
                            }
                            else
                            {
                                prFrcPrm->u2TargetNs++;
                            }

                            LOG(3, "Fast Start PTS %x, STC %x, Delta (%d)\n", prFrcPrm->u4Pts, rStcClk.u4Base, i4Delta);
                        }
                        else
                        {
#endif
#ifdef ENABLE_MULTIMEDIA
                            UCHAR ucEsId;
                            VDEC_ES_INFO_T *prVdecEsInfo = NULL;
                            ucEsId = _prVdpConf[ucVdpId]->ucInputPort[0];
                            prVdecEsInfo = (ucEsId < MAX_ES_NS)? _VDEC_GetEsInfo(ucEsId) : NULL;
#endif
                            if ((ucAvSyncMode == VID_SYNC_MODE_SLAVE_MM)
                                    || (ucAvSyncMode == VID_SYNC_MODE_MASTER) /* Trick Play */
#ifdef ENABLE_MULTIMEDIA
                                    || (prVdecEsInfo && prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS_TIME_SHIFT)
#endif
                               )

                            {
                                if (u4Delta > VDP_MM_PEND_MAX)
                                {
                                    if ((ucAvSyncMode == VID_SYNC_MODE_MASTER)
                                        #ifdef ENABLE_MULTIMEDIA
                                        && (prVdecEsInfo == NULL || (prVdecEsInfo && (prVdecEsInfo->eMMSrcType != SWDMX_SRC_TYPE_WIFI_DISPLAY)))
                                        #endif
                                        )
                                    {
#ifdef CC_53XX_SWDMX_V2
                                        STC_SetStcValue(ucStcSrc, _VDP_GetDispingPts(ucB2rId));
#else
                                        STC_SetStcValue(_VDP_GetDispingPts(ucB2rId));
#endif
                                        LOG(3, "MM ----Force Set Stc for bad edit bitstream\n");
                                    }
                                    else
                                    {
                                        LOG(3, "MM ----Force Video Start for bad edit bitstream\n");
                                        return;
                                    }
                                }
                                else //if (u4Delta > VDP_EARLY_DISP_DRIFT)
                                {
                                    UCHAR ucSeekMode;
                                    UINT32 u4SeekPts;
                                    UCHAR ucSeekNotifyTrigger;
                                    
                                    #ifdef CC_IPTV_SUPPORT
                                    if (prVdecEsInfo && (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_IPTV))
                                    {
                                        static UINT32 u4IPTVcnt = 0;
                                        
                                        //2s<u4Delta<4s: repeat 2; u4Delta>4s: repeat 1; others: repeat 3
                                        if ((++u4IPTVcnt) < ((u4Delta >= 2*90000 && u4Delta < 4*90000) ? 2 : ((u4Delta >= 4*90000) ? 1 : 3)))
                                        {
                                            LOG(3, "MM ----not repeat for IPTV (%d)\n", i4Delta);
                                            
                                            return;
                                        }              
                                        
                                        u4IPTVcnt = 0;
                                    }
                                    #endif

                                    _VDPGetSeek(ucB2rId, &ucSeekMode, &u4SeekPts, &ucSeekNotifyTrigger);

                                    if ((ucSeekMode != 0) && (ucSeekNotifyTrigger != 0) /*&&
                                       (u4SeekPts <= prFrcPrm->u4Pts)*/)
                                    {
                                        LOG(3, "+++++MM ----Frame Repeat in Seek. force play \n");
                                        return;
                                    }
#ifdef HANDLE_PTS_SKIP
                                    // For some soruce, we my not need to always repeat frame
                                    if ( (_AVSyncCtrl[ucB2rId].fgAVSyncOnOff) &&
                                            (_AVSyncCtrl[ucB2rId].u4AVSyncSpeed > 1) )
                                    {
                                        if ( !( _AVSyncCtrl[ucB2rId].u4CurCycleRepeatCnt %
                                                _AVSyncCtrl[ucB2rId].u4AVSyncSpeed ) )
                                        {
                                            _AVSyncCtrl[ucB2rId].u4CurCycleRepeatCnt = 1;

                                            LOG(3, "AVSyncSpeed(%u) ---not repeat this time.\n",
                                                _AVSyncCtrl[ucB2rId].u4AVSyncSpeed);

                                            return ;
                                        }

                                        _AVSyncCtrl[ucB2rId].u4CurCycleRepeatCnt += 1;
                                    }
#endif
                                    // pending for the current PTS
                                    _VdpPreparePendingForNextVsync(prFrcPrm);
                                    prFrcPrm->ucMMWaitSTC = 1;
#ifdef TIME_MEASUREMENT
                                    //u4ReptCnt++;
#endif
                                    LOG(3, " MM ----Pending for current PTS\n");
                                    return;
                                }
                                //LOG(3, "MM -- Repeat Last Frame\n\r");
                            }
                            else if (ucAvSyncMode == VID_SYNC_MODE_SLAVE)
                            {
                                UINT32 u4MaxStep = VDP_MAX_ASYNC_REPEAT_NS;
                                
                                u4Step = 1 + (u4Delta / u4OutFrameStc);
                                
                                u4Step = (u4Step < u4MaxStep) ? u4Step : u4MaxStep;

                                prFrcPrm->prFbCounter->u4DispRepeat += (prFrcPrm->u2TargetNs * (u4Step - 1));

                                prFrcPrm->i4PtsDrift += (INT32)(u4OutFrameStc * (u4Step - 1));

                                if (prFrcPrm->ucWrongField && (prFrcPrm->u2TargetNs&1))
                                {
                                    prFrcPrm->i4PtsDrift += prFrcPrm->u4OutStcPeriod;
                                }

                                u4TargetNs = (prFrcPrm->u2TargetNs*u4Step);
                                if ( u4TargetNs >= 0xFF )
                                {
                                    prFrcPrm->u2TargetNs = 250;
                                    prFrcPrm->u2ChangeFieldNs =(prFrcPrm->u2TargetNs/2);
                                }
                                else
                                {
                                    prFrcPrm->u2TargetNs = (UCHAR)u4TargetNs;
                                    prFrcPrm->u2ChangeFieldNs *= u4Step;
                                }

                                // protection
#ifndef CC_DCR_TEST
                                prFrcPrm->u4PtsAvSyncNotAllow = prFrcPrm->ucInFrameRate;
#endif
                                // [LOG] AVSYNC, V-PTS Repeat
                                SYSLOG(SYNC_PREFIX + 20 + prFrcPrm->ucFbgId, prFrcPrm->prFbCounter->u4DispOk, prFrcPrm->prFbCounter->u4DispFail, prFrcPrm->u2TargetNs);

                                /* Video Frame repeat notification */
                                _VDP_VsyncNotify(VDP_MSG_REPEAT_DROP_CB, ucVdpId,
                                                 prFrcPrm->prFbCounter->u4SyncMode, 0);
#ifdef TIME_MEASUREMENT
                                //u4ReptCnt++;
#endif

                                LOG(3, "AVS-R B2R(%d) Fbg(%d) %d PTS(0x%x) STC(0x%x)\n", prFrcPrm->ucB2rId, prFrcPrm->ucFbgId, prFrcPrm->u2TargetNs, prFrcPrm->u4Pts, rStcClk.u4Base);
                            }

#ifdef CC_FAST_CHANGE_CHANNEL
                        }
#endif
                    }
                }
                else
                {
#ifdef CC_FAST_CHANGE_CHANNEL
                    if (prFrcPrm->ucFastChangeEn)
                    {
                        LOG(3, "Fast Stop 1\n");
                        prFrcPrm->ucFastChangeEn = 0;
#ifdef CC_FAST_CHANGE_CHANNEL_LOG
                        HAL_GetTime(&rNvgtAvSyncTime);
#endif
                    }
#endif
                }
            }
            else
            {
                // PTS faster than STC, drop
                // [LOG] AVSYNC, V-PTS faster than STC
//                SYSLOG(SYNC_PREFIX + 10 + prFrcPrm->ucFbgId, rStcClk.u4Base, u4Delta, prFrcPrm->i4TemporalRef);

#ifdef CC_FAST_CHANGE_CHANNEL
                if (prFrcPrm->ucFastChangeEn)
                {
                    LOG(3, "Fast Stop 2\n");
                    prFrcPrm->ucFastChangeEn = 0;
#ifdef CC_FAST_CHANGE_CHANNEL_LOG
                    HAL_GetTime(&rNvgtAvSyncTime);
#endif
                }
#endif
                prFrcPrm->prFbCounter->u4SyncMode = FBM_FBG_AVSYNC_NORMAL;
                FBM_SetAVSyncStatus(prFrcPrm->ucFbgId, prFrcPrm->prFbCounter->u4SyncMode);

                if (u4Delta > u4OutFrameStc && ucB2rId<B2R_NS)
                {
                    if(prFrcPrm->t_wfd_prm.fgDblClk
                        && prFrcPrm->u2TargetNs > 1)
                    {
                        prFrcPrm->u2TargetNs = 1;
                    }

                    if (u4OutFrameStc != 0)
                    {
                        _arVdpPrm[ucB2rId].u4B2rSkipFrameNo = u4Delta / u4OutFrameStc;
                    }
                    else
                    {
                        _arVdpPrm[ucB2rId].u4B2rSkipFrameNo = 0;
                    }
                    prFrcPrm->prFbCounter->u4SyncMode = FBM_FBG_AVSYNC_DROP;
                    FBM_SetAVSyncStatus(prFrcPrm->ucFbgId, prFrcPrm->prFbCounter->u4SyncMode);

#ifndef CC_B2R_DISABLE_ONE_SECOND_FRC
                    // protection
                    if ((ucAvSyncMode == VID_SYNC_MODE_SLAVE) && (prFrcPrm->u4PtsAvSyncNotAllow))
                    {
                        return;
                    }
#endif
#ifdef VDP_AVS_ADJUST_STC
                    if ( ( prFrcPrm->ucReady == 0 ) && (ucAvSyncMode == VID_SYNC_MODE_SLAVE) )
                    {
                        // DTV
                        if ( u4Delta > VDP_EARLY_DISP_DRIFT )
                        {
                            // pending for the 1st PTS
                            _VdpPreparePendingForNextVsync(prFrcPrm);

                            _VDP_SetAvsyncDelta(&_rB2rAvs,u4Delta,FALSE);


                            LOG(3, "----D Pending for the 1st PTS\n");
                            return;
                        }
                    }
#endif
                    if ((u4ZeroPtsNs != 0) && (u4Delta > prFrcPrm->u4PtsDelta)
                        && !prFrcPrm->t_wfd_prm.fgDblClk)
                    {
                        // ask MPV to keep drop B if NOT Enough
                        FBM_SetFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_SKIP_PIC_NO_PTS);
                        LOG(3, "AVS-D VLD Keep Drop B (Zero PTS) B2R(%d)\n", prFrcPrm->ucB2rId);
                        prFrcPrm->u4KeepDropB = 1;
                    }
                    else if (u4Delta > (u4OutFrameStc * VDP_KEEP_DROP_B_NS)
                        && !prFrcPrm->t_wfd_prm.fgDblClk)
                    {
                        // ask MPV to keep drop B if NOT Enough
                        FBM_SetFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_SKIP_PIC);
#if (defined(CC_3D_MM_DS_SUPPORT) && defined(CC_B2R_ENABLE_R_VIEW_SKIP_FRAME))
                        if (prFrcPrm->ucSubFbgId != FBM_FBG_ID_UNKNOWN && FBM_GetFbgAppMode(prFrcPrm->ucSubFbgId) == FBM_FBG_APP_Frame_Pack_3D)
                        {
                            FBM_SetFrameBufferFlag(prFrcPrm->ucSubFbgId, FBM_FLAG_SKIP_PIC);
                        }
#endif
                        LOG(3, "AVS-D VLD Keep Drop B B2R(%d)\n", prFrcPrm->ucB2rId);
                        prFrcPrm->u4KeepDropB = 1;
                    }

                    // protection
#ifndef CC_DCR_TEST
                    prFrcPrm->u4PtsAvSyncNotAllow = prFrcPrm->ucInFrameRate;
#endif

#ifdef HANDLE_PTS_SKIP
                    // For some soruce, we my not need to always drop frame
                    if ( (_AVSyncCtrl[ucB2rId].fgAVSyncOnOff) &&
                            (_AVSyncCtrl[ucB2rId].u4AVSyncSpeed > 1) )
                    {
                        if (_AVSyncCtrl[ucB2rId].u4CurCycleRepeatCnt % (_AVSyncCtrl[ucB2rId].u4AVSyncSpeed+2))
                        {
                            _AVSyncCtrl[ucB2rId].u4CurCycleRepeatCnt += 1;

                            LOG(3, "AVSyncSpeed(%u) ---not drop this time.\n",
                                _AVSyncCtrl[ucB2rId].u4AVSyncSpeed);

                            return ;
                        }

                        _AVSyncCtrl[ucB2rId].u4CurCycleRepeatCnt = 1;
                    }
#endif
                    if (FBM_ChkFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_SKIP_PIC))
                    {
                        if (!(prFrcPrm->fgB2R3DEnable && prFrcPrm->u1B2R3DType == B2R_3D_FRAME_SEQUENTIAL))
                        {
                            // ask MPV to drop if Display Queue is Empty
                            if (FBM_CheckFrameBufferDispQ(prFrcPrm->ucFbgId) != 0)
                            {
                                prFrcPrm->i4PtsDrift -= (INT32)u4OutFrameStc;

                                prFrcPrm->u2ChangeFieldNs = 0;
                                prFrcPrm->u2TargetNs = 0;

                                // [LOG] AVSYNC, V-PTS Drop
#ifdef TIME_MEASUREMENT
                                //u4DropCnt++;
#endif
                                SYSLOG(SYNC_PREFIX + 30 + prFrcPrm->ucFbgId, prFrcPrm->prFbCounter->u4DispOk, prFrcPrm->prFbCounter->u4DispFail, prFrcPrm->u2TargetNs);
                                LOG(3, "AVS-D B2R(%d) Fbg(%d) PTS(0x%x) STC(0x%x)\n", prFrcPrm->ucB2rId, prFrcPrm->ucFbgId, prFrcPrm->u4Pts, rStcClk.u4Base);
                            }
                            else
                            {
                                LOG(3, "[Warning] Can not drop at both VDP & MPV\n");
                            }
                        }
                    }
                    else if(!prFrcPrm->t_wfd_prm.fgDblClk)
                    {
                        UINT32 u4EsmQCnt;

                        u4EsmQCnt = _prDbase->rMpv[0].u4EsmQCnt;

                        if (u4EsmQCnt > 1)
                        {
                            // ask MPV to drop
                            FBM_SetFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_SKIP_PIC);
#if (defined(CC_3D_MM_DS_SUPPORT) && defined(CC_B2R_ENABLE_R_VIEW_SKIP_FRAME))
                            if (prFrcPrm->ucSubFbgId != FBM_FBG_ID_UNKNOWN && FBM_GetFbgAppMode(prFrcPrm->ucSubFbgId) == FBM_FBG_APP_Frame_Pack_3D)
                            {
                                FBM_SetFrameBufferFlag(prFrcPrm->ucSubFbgId, FBM_FLAG_SKIP_PIC);
                            }
#endif
                            LOG(3, "AVS-D VLD B2R(%d) Fbg(%d) PTS(0x%x) STC(0x%x) Q(%d)\n", prFrcPrm->ucB2rId, prFrcPrm->ucFbgId, prFrcPrm->u4Pts, rStcClk.u4Base, u4EsmQCnt);
                        }
                        else
                        {
                            if (FBM_CheckFrameBufferDispQ(prFrcPrm->ucFbgId) != 0)
                            {
                                prFrcPrm->i4PtsDrift -= (INT32)u4OutFrameStc;

                                prFrcPrm->u2ChangeFieldNs = 0;
                                prFrcPrm->u2TargetNs = 0;

                                // [LOG] AVSYNC, V-PTS Drop
#ifdef TIME_MEASUREMENT
                                //u4DropCnt++;
#endif
                                SYSLOG(SYNC_PREFIX + 30 + prFrcPrm->ucFbgId, prFrcPrm->prFbCounter->u4DispOk, prFrcPrm->prFbCounter->u4DispFail, prFrcPrm->u2TargetNs);
                                LOG(3, "AVS-D B2R(%d) Fbg(%d) PTS(0x%x) STC(0x%x)\n", prFrcPrm->ucB2rId, prFrcPrm->ucFbgId, prFrcPrm->u4Pts, rStcClk.u4Base);
                            }
                            else
                            {
                                LOG(3, "[Warning] Can not drop at VDP & MPV\n");
                            }
                        }
                    }

                    /* Video Frame drop notification */
                    /*_VDP_VsyncNotify(VDP_MSG_REPEAT_DROP_CB, prFrcPrm->ucVdpId,
                                     prFrcPrm->prFbCounter->u4SyncMode, 0);*/

                    prFrcPrm->prFbCounter->u4DispDrop++;

#if 0
                    // ask MPV to drop if Display Queue is Empty
                    if (FBM_CheckFrameBufferDispQ(prFrcPrm->ucFbgId) == 0)
                    {
                        // [LOG] AVSYNC, V-PTS Don't Drop (Display Queue Empty)
                        SYSLOG(SYNC_PREFIX + 40 + prFrcPrm->ucFbgId, prFrcPrm->prFbCounter->u4DispOk, prFrcPrm->prFbCounter->u4DispFail, prFrcPrm->u2TargetNs);

                        FBM_SetFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_SKIP_PIC);
                        LOG(3, "AVS-D VLD Vdp(%d) Fbg(%d) PTS(0x%x) STC(0x%x)\n", prFrcPrm->ucVdpId, prFrcPrm->ucFbgId, prFrcPrm->u4Pts, rStcClk.u4Base);

                        return;
                    }

                    prFrcPrm->prFbCounter->u4DispDrop++;

                    prFrcPrm->i4PtsDrift -= (INT32)u4OutFrameStc;

                    prFrcPrm->u2ChangeFieldNs = 0;
                    prFrcPrm->u2TargetNs = 0;

                    // [LOG] AVSYNC, V-PTS Drop
                    SYSLOG(SYNC_PREFIX + 30 + prFrcPrm->ucFbgId, prFrcPrm->prFbCounter->u4DispOk, prFrcPrm->prFbCounter->u4DispFail, prFrcPrm->u2TargetNs);
                    LOG(3, "AVS-D Vdp(%d) Fbg(%d) PTS(0x%x) STC(0x%x)\n", prFrcPrm->ucVdpId, prFrcPrm->ucFbgId, prFrcPrm->u4Pts, rStcClk.u4Base);
#endif
                }
                else /*u4Delta < u4OutFrameStc*/
                {
                    //only consider wrongfield case
                    if (prFrcPrm->ucWrongField && (prFrcPrm->u2TargetNs&1))
                    {
                        prFrcPrm->i4PtsDrift = i4Delta + prFrcPrm->u4OutStcPeriod;
                    }
                }
            }
        }
        else if (prFrcPrm->ucReady == 0)
        {
            if (prFrcPrm->u4PtsAsyncNs < VDP_WAIT_1ST_PTS_NS)
            {
                // pending for the 1st PTS
                _VdpPreparePendingForNextVsync(prFrcPrm);

                prFrcPrm->u4PtsAsyncNs++;
            }
            else
            {
                // STC still invalid after 12 vsync, forget it, go display!
                LOG(3, "Force Video Start 3\n");
                prFrcPrm->u4PtsAsyncNs = 0;
            }

            prFrcPrm->prFbCounter->u4SyncMode = FBM_FBG_AVSYNC_WAIT_1ST;
            FBM_SetAVSyncStatus(prFrcPrm->ucFbgId, prFrcPrm->prFbCounter->u4SyncMode);
        }
        else
        {
#ifdef CC_GET_PCR_PTS_INFO
            UINT32 u4Delta;
            INT32 i4Delta;

            if (prFrcPrm->u4Pts > rStcClk.u4Base)
            {
                u4Delta = (prFrcPrm->u4Pts - rStcClk.u4Base);

                if (u4Delta < 0x80000000)
                {
                    i4Delta = - (INT32) u4Delta;
                }
                else
                {
                    // handle wrap around
                    u4Delta = (0xFFFFFFFF - u4Delta);
                    i4Delta = (INT32) u4Delta;
                }
            }
            else
            {
                u4Delta = (rStcClk.u4Base - prFrcPrm->u4Pts);

                if (u4Delta < 0x80000000)
                {
                    i4Delta = (INT32) u4Delta;
                }
                else
                {
                    // handle wrap around
                    u4Delta = (0xFFFFFFFF - u4Delta);
                    i4Delta = - (INT32) u4Delta;
                }
            }

            _prDbase->rPts.u4VideoPts = prFrcPrm->u4Pts;
            _prDbase->rPts.u4VideoPtsStcDelta = u4Delta;
            _prDbase->rPts.i4VideoPtsStcDelta = i4Delta;

            if (u4Delta >= 27000)     // 300ms
            {
                _prDbase->rPts.fgVideoSync = FALSE;
            }
            else
            {
                _prDbase->rPts.fgVideoSync = TRUE;
            }
#endif

            prFrcPrm->prFbCounter->u4SyncMode = FBM_FBG_AVSYNC_STC_INVALID;
            FBM_SetAVSyncStatus(prFrcPrm->ucFbgId, prFrcPrm->prFbCounter->u4SyncMode);
        }
    }
    else
    {
#ifdef CC_GET_PCR_PTS_INFO
        STC_CLOCK rStcClk;

        if (STC_GetSrc(0, &rStcClk) == STC_VALID)
        {
            UINT32 u4Delta;

            if (prFrcPrm->u4Pts > rStcClk.u4Base)
            {
                u4Delta = (prFrcPrm->u4Pts - rStcClk.u4Base);
            }
            else
            {
                u4Delta = (rStcClk.u4Base - prFrcPrm->u4Pts);
            }

            _prDbase->rPts.u4VideoPts = prFrcPrm->u4Pts;
            _prDbase->rPts.u4VideoPtsStcDelta = u4Delta;
        }
#endif

        prFrcPrm->prFbCounter->u4SyncMode = FBM_FBG_AVSYNC_DISABLE;
        FBM_SetAVSyncStatus(prFrcPrm->ucFbgId, prFrcPrm->prFbCounter->u4SyncMode);
    }
#endif
}

static void _VdpSetOutInfoByPmx(UCHAR ucPmxMode,VDP_OUT_INFO_T *prOutInfo)
{
    UCHAR ucIdx = 0;
    UCHAR u1Default = 0;  // to record where 1080p_60 is!

    for (ucIdx = 0; ucIdx < VDP_B2R_MODE_MAX; ucIdx++)
    {
        if (ucPmxMode == arModeInfo[ucIdx].rOutInfo.ucPmxMode)
        {
            break;
        }
        else if (arModeInfo[ucIdx].rOutInfo.ucPmxMode == VDP_B2R_MODE_1080P_60)
        {
            u1Default = ucIdx;
        }
    }

    if (ucIdx >= VDP_B2R_MODE_MAX)   // this case should not happen!! use default 1080p config!
    {
        ucIdx = u1Default ;
        LOG(0,"Error! No pmx config found!PMXMode = %d\n",ucPmxMode);
    }

    prOutInfo->ucPrg =  arModeInfo[ucIdx].rOutInfo.ucPrg;
    prOutInfo->ucFrameRate = arModeInfo[ucIdx].rOutInfo.ucFrameRate;
    prOutInfo->u4Width = arModeInfo[ucIdx].rOutInfo.u4Width;
    prOutInfo->u4Height = arModeInfo[ucIdx].rOutInfo.u4Height;
    prOutInfo->u4TotalWidth = arModeInfo[ucIdx].rOutInfo.u4TotalWidth;
    prOutInfo->u4TotalHeight = arModeInfo[ucIdx].rOutInfo.u4TotalHeight;
    prOutInfo->ucDoubleClk = arModeInfo[ucIdx].rOutInfo.ucDoubleClk;
    prOutInfo->u4ClkMode = arModeInfo[ucIdx].rOutInfo.u4ClkMode;

    // decide clk mode other than default input frame rate.
    if (arModeInfo[ucIdx].u4FrmRateMsk &  Msk(prOutInfo->ucInputFrameRate))
        prOutInfo->u4ClkMode = arModeInfo[ucIdx].u4ClkModeExp;

// Log out the vdp output info!
    LOG(1,"pmx mode: %d\n",ucPmxMode);
    LOG(1,"arPrg:%d,ucFrameRate=%d,u4Width:%d,u4Height:%d,u4TotalWidth:%d,u4TotalHeight:%d,ucDoubleClk:%d,u4ClkMode:%d\n",
        prOutInfo->ucPrg ,
        prOutInfo->ucFrameRate ,
        prOutInfo->u4Width ,
        prOutInfo->u4Height ,
        prOutInfo->u4TotalWidth ,
        prOutInfo->u4TotalHeight ,
        prOutInfo->ucDoubleClk ,
        prOutInfo->u4ClkMode
       );
}

//-----------------------------------------------------------------------------
/** Brief of _VdpCheckOutputMode.
 */
//-----------------------------------------------------------------------------

static void _VdpCheckOutputMode(B2R_PRM_T* prFrcPrm)
{
    UCHAR ucVdpId;
    UCHAR ucPmxMode;
    UCHAR ucPlayMode;
    UCHAR ucProgressive;
    VDP_OUT_INFO_T * prOutInfo ;
    CHAR ucB2rId;

#ifdef CC_B2R_SENSIO_CONV
    VDEC_ES_INFO_T* prVdecEsInfo;
    prVdecEsInfo = _VDEC_GetEsInfo(ES0);
#endif

    if (prFrcPrm == NULL)
    {
        return;
    }
    ucB2rId = prFrcPrm->ucB2rId;
    if (ucB2rId >= B2R_NS)
    {
        LOG(0, "ucB2rId >= B2R_NS , oops \n");
        return;
    }
    ucVdpId = VDP_GetVdpId(ucB2rId);
    if (ucVdpId >= VDP_NS)
    {
        LOG(0, "ucVdpId >= VDP_NS , oops \n");
        return;
    }

    prOutInfo = &(_prVdpConf[ucVdpId]->rOutInfo);

    ucPmxMode = VDP_B2R_MODE_UNKNOWN;

    prFrcPrm->fgDoubleClock = FALSE;
#ifdef ENABLE_MULTIMEDIA
    {
        UCHAR ucEsId;
        VDEC_ES_INFO_T *prVdecEsInfo = NULL;
        ucEsId = _prVdpConf[ucVdpId]->ucInputPort[0];
        prVdecEsInfo = (ucEsId < MAX_ES_NS)? _VDEC_GetEsInfo(ucEsId) : NULL;
        if(prVdecEsInfo && prFrcPrm->ucProgressiveSeqFlag &&
          (((prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_WIFI_DISPLAY) && (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_30)) ||
          ((prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_DASH) && (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_25))))
        {
            prFrcPrm->fgDoubleClock = TRUE;
            LOG(1, "Double Clock(%d).\n", prFrcPrm->fgDoubleClock);
        }
    }
#endif

    ucProgressive = prFrcPrm->ucProgressiveSeqFlag;

    FBM_GetPlayMode(prFrcPrm->ucFbgId, &ucPlayMode);
    if ((prFrcPrm->ucProgressiveSeqFlag==0) &&
            ((prOutInfo->ucInputFrameRate == MPEG_FRAME_RATE_24_) ||
             (prOutInfo->ucInputFrameRate == MPEG_FRAME_RATE_24) ||
             (prOutInfo->ucInputFrameRate == MPEG_FRAME_RATE_50) ||
             (prOutInfo->ucInputFrameRate == MPEG_FRAME_RATE_60_) ||
             (prOutInfo->ucInputFrameRate == MPEG_FRAME_RATE_60)) &&
            (prFrcPrm->ucProgressiveFrame!= 0) &&
            (ucPlayMode == FBM_FBG_MM_MODE))
    {
        LOG(3, "Interlace Seq but Progressive Pic\n");
        ucProgressive = 1;
    }


    if (ucProgressive)
    {
        // Progressive Source
        if(prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_12)
        {
            ucPmxMode = VDP_B2R_MODE_1080P_12;
        }
        else if(prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_15 &&
            (_prVdpConf[ucVdpId]->u4SrcWidth <= 1920) &&
            (_prVdpConf[ucVdpId]->u4SrcHeight <= 1080))
        {
            ucPmxMode = VDP_B2R_MODE_1080P_15;
        }
        else if(prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_20)
        {
            ucPmxMode = VDP_B2R_MODE_1080P_20;
        }
        else if (((prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_25) ||
                (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_50)
            ) &&
                (_prVdpConf[ucVdpId]->u4SrcWidth <= 720) &&
                (_prVdpConf[ucVdpId]->u4SrcHeight <= 576))
        {
#ifndef CC_B2R_ENABLE_OUTPUT_FRC
            if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_25)
            {
                ucPmxMode = VDP_B2R_MODE_576P_25;
            }
            else
#endif
                {
                    ucPmxMode = VDP_B2R_MODE_576P_50;
                }

            if (prFrcPrm->fgDoubleClock)
            {
                if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_50)
                {
                    ucPmxMode = VDP_B2R_MODE_576P_100;
                }
                else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_25)
                {
                    ucPmxMode = VDP_B2R_MODE_576P_50;
                }
            }
#ifdef CC_B2R_3D_SUPPROT
            if (prFrcPrm->fgB2R3DEnable && !prFrcPrm->fgB2RForce2D)
            {
                if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_50)
                {
                    ucPmxMode = VDP_B2R_MODE_576P_100;
                }
                else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_25)
                {
                    ucPmxMode = VDP_B2R_MODE_576P_50;
                }
            }
#endif
        }
        else if ((_prVdpConf[ucVdpId]->u4SrcWidth <= 720) &&
                 (_prVdpConf[ucVdpId]->u4SrcHeight <= 480))
        {
#ifndef CC_B2R_ENABLE_OUTPUT_FRC
            if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_24)
            {
                ucPmxMode = VDP_B2R_MODE_480P_24;
            }
            else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_30)
            {
#ifdef SUPPORT_DISP_WEB_VIDEO_SMOOTHER
                ucPmxMode = VDP_B2R_MODE_480P_60;
#else
                ucPmxMode = VDP_B2R_MODE_480P_30;
#endif
            }
            else
#endif
                {
                    ucPmxMode = VDP_B2R_MODE_480P_60;
                }

            if (prFrcPrm->fgDoubleClock)
            {
                if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_24)
                {
                    ucPmxMode = VDP_B2R_MODE_480P_48;
                }
                else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_60)
                {
                    ucPmxMode = VDP_B2R_MODE_480P_120;
                }
                else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_30)
                {
                    ucPmxMode = VDP_B2R_MODE_480P_60;
                }
            }
#ifdef CC_B2R_3D_SUPPROT
            if (prFrcPrm->fgB2R3DEnable && !prFrcPrm->fgB2RForce2D)
            {
                if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_24)
                {
                    ucPmxMode = VDP_B2R_MODE_480P_48;
                }
                else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_60)
                {
                    ucPmxMode = VDP_B2R_MODE_480P_120;
                }
                else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_30)
                {
                    ucPmxMode = VDP_B2R_MODE_480P_60;
                }
            }
#endif
        }
#ifdef CC_MT5396
        else if ((_prVdpConf[ucVdpId]->u4SrcWidth <= 1280) &&   // 1408 = 1280 * 1.1
                 (_prVdpConf[ucVdpId]->u4SrcHeight <= 720) && (_prVdpConf[ucVdpId]->u4SrcHeight > 480))      // 792 = 720 * 1.1
#else
        else if ((_prVdpConf[ucVdpId]->u4SrcWidth <= 1280) &&   // 1408 = 1280 * 1.1
                 (_prVdpConf[ucVdpId]->u4SrcHeight <= 720))      // 792 = 720 * 1.1
#endif
        {
#ifndef CC_B2R_ENABLE_OUTPUT_FRC
            switch (prFrcPrm->ucInFrameRate)
            {
            case FBM_FRAME_RATE_24:
                ucPmxMode = VDP_B2R_MODE_720P_24;
                break;
            case FBM_FRAME_RATE_25:
                ucPmxMode = VDP_B2R_MODE_720P_25;
                break;
            case FBM_FRAME_RATE_30:
#ifdef SUPPORT_DISP_WEB_VIDEO_SMOOTHER
                ucPmxMode = VDP_B2R_MODE_720P_60;
#else
                ucPmxMode = VDP_B2R_MODE_720P_30;
#endif
                break;
            case FBM_FRAME_RATE_50:
                ucPmxMode = VDP_B2R_MODE_720P_50;
                break;
            case FBM_FRAME_RATE_120:
                ucPmxMode = VDP_B2R_MODE_720P_120;
                break;
            case FBM_FRAME_RATE_240:
                ucPmxMode = VDP_B2R_MODE_720P_240;
                break;
            case FBM_FRAME_RATE_60:
            default:
                ucPmxMode = VDP_B2R_MODE_720P_60;
                break;
            }
#else
            switch (prFrcPrm->ucInFrameRate)
            {
            case FBM_FRAME_RATE_25:
            case FBM_FRAME_RATE_50:
                ucPmxMode = VDP_B2R_MODE_720P_50;
                break;
            case FBM_FRAME_RATE_120:
                ucPmxMode = VDP_B2R_MODE_720P_120;
                break;
            case FBM_FRAME_RATE_240:
                ucPmxMode = VDP_B2R_MODE_720P_240;
                break;
            case FBM_FRAME_RATE_24:
            case FBM_FRAME_RATE_30:
            case FBM_FRAME_RATE_60:
            default:
                ucPmxMode = VDP_B2R_MODE_720P_60;
                break;
            }
#endif
            if (prFrcPrm->fgDoubleClock)
            {
                if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_60)
                {
                    ucPmxMode = VDP_B2R_MODE_720P_120;
                }
                else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_50)
                {
                    ucPmxMode = VDP_B2R_MODE_720P_100;
                }
                else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_24)
                {
                    ucPmxMode = VDP_B2R_MODE_720P_48;
                }
                else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_25)
                {
                    ucPmxMode = VDP_B2R_MODE_720P_50;
                }
                else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_30)
                {
                    ucPmxMode = VDP_B2R_MODE_720P_60;
                }
            }
#ifdef CC_B2R_3D_SUPPROT
            if (prFrcPrm->fgB2R3DEnable && !prFrcPrm->fgB2RForce2D)
            {
                if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_60)
                {
                    ucPmxMode = VDP_B2R_MODE_720P_120;
                }
                else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_50)
                {
                    ucPmxMode = VDP_B2R_MODE_720P_100;
                }
                else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_24)
                {
                    ucPmxMode = VDP_B2R_MODE_720P_48;
                }
                else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_25)
                {
                    ucPmxMode = VDP_B2R_MODE_720P_50;
                }
                else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_30)
                {
                    ucPmxMode = VDP_B2R_MODE_720P_60;
                }
            }
#ifdef CC_B2R_SENSIO_CONV
            if(prVdecEsInfo->fgSensio3D)
            {
                if((_prVdpConf[ucVdpId]->u4SrcWidth <= 960) &&   //add special 3D
                        (_prVdpConf[ucVdpId]->u4SrcHeight <= 720))
                {
                    switch (prFrcPrm->ucInFrameRate)
                    {
                        case
                            FBM_FRAME_RATE_60:
                            ucPmxMode
                            =
                            VDP_B2R_MODE_960_720P_120;
                        break;

                        default:
                        ucPmxMode
                            =
                            VDP_B2R_MODE_720P_60;
                        break;
                    }
                }
            }
#endif            
#endif

            if (_prVdpConf[ucVdpId]->u4SrcHeight > 720)
            {
                LOG(1, "Force Image Height %d >> 720\n", _prVdpConf[ucVdpId]->u4SrcHeight);
                _prVdpConf[ucVdpId]->u4SrcHeight = 720;
            }
            if (_prVdpConf[ucVdpId]->u4SrcWidth > 1280)
            {
                LOG(1, "Force Image Width %d >> 1280\n", _prVdpConf[ucVdpId]->u4SrcWidth);
                _prVdpConf[ucVdpId]->u4SrcWidth = 1280;
            }
        }
        else if ((_prVdpConf[ucVdpId]->u4SrcWidth <= 1440) &&
                 (_prVdpConf[ucVdpId]->u4SrcHeight <= 480))
        {
            switch (prFrcPrm->ucInFrameRate)
            {
            case FBM_FRAME_RATE_24:
                ucPmxMode = VDP_B2R_MODE_480P_1440W_24;
                break;
            case FBM_FRAME_RATE_25:
            case FBM_FRAME_RATE_50:
            case FBM_FRAME_RATE_30:
            case FBM_FRAME_RATE_60:
            default:
                ucPmxMode = VDP_B2R_MODE_480P_1440W_60;
                break;
            }
        }
        else if (((_prVdpConf[ucVdpId]->u4SrcWidth <= 1280) &&
                  (_prVdpConf[ucVdpId]->u4SrcHeight <= 1440)) ||
                 ((_prVdpConf[ucVdpId]->u4SrcWidth <= 1024) &&
                  (_prVdpConf[ucVdpId]->u4SrcHeight <= 1536)))
        {
            switch (prFrcPrm->ucInFrameRate)
            {
#if 1//defined(CC_MT5396) || defined(CC_MT5399) //#ifdef CC_MT5396 add for CR DTV00351047 when mm video is 1024*768(tab) => v_total too large to do MJC 3D in mt5396
            case FBM_FRAME_RATE_24:
                ucPmxMode = (_prVdpConf[ucVdpId]->u4SrcHeight <= 768)? VDP_B2R_MODE_720P_1080W_24 :
                            (_prVdpConf[ucVdpId]->u4SrcHeight <= 1088)? VDP_B2R_MODE_1080P_24 : VDP_B2R_MODE_1440P_1280W_24;
                if (prFrcPrm->fgB2R3DEnable && !prFrcPrm->fgB2RForce2D)
                {
                    if (VDP_B2R_MODE_1080P_24 == ucPmxMode)
                    {
                        ucPmxMode = VDP_B2R_MODE_1080P_48;
                    }
                }
                break;
            case FBM_FRAME_RATE_25:
                ucPmxMode = (_prVdpConf[ucVdpId]->u4SrcHeight <= 768)? VDP_B2R_MODE_720P_1080W_25 :
                            (_prVdpConf[ucVdpId]->u4SrcHeight <= 1088)? VDP_B2R_MODE_1080P_25 : VDP_B2R_MODE_1440P_1280W_25;
                if (prFrcPrm->fgB2R3DEnable && !prFrcPrm->fgB2RForce2D)
                {
                    if (VDP_B2R_MODE_1080P_25 == ucPmxMode)
                    {
                        ucPmxMode = VDP_B2R_MODE_1080P_50;
                    }
                }
                break;
            case FBM_FRAME_RATE_50:
                ucPmxMode = (_prVdpConf[ucVdpId]->u4SrcHeight <= 768)? VDP_B2R_MODE_720P_1080W_50 :
                            (_prVdpConf[ucVdpId]->u4SrcHeight <= 1088)? VDP_B2R_MODE_1080P_50 : VDP_B2R_MODE_1440P_1280W_50;
                break;
            case FBM_FRAME_RATE_30:
                ucPmxMode = (_prVdpConf[ucVdpId]->u4SrcHeight <= 768)? VDP_B2R_MODE_720P_1080W_30 :
                            (_prVdpConf[ucVdpId]->u4SrcHeight <= 1088)? VDP_B2R_MODE_1080P_30 : VDP_B2R_MODE_1440P_1280W_30;
                if (prFrcPrm->fgB2R3DEnable && !prFrcPrm->fgB2RForce2D)
                {
                    if (VDP_B2R_MODE_1080P_30 == ucPmxMode)
                    {
                        ucPmxMode = VDP_B2R_MODE_1080P_60;
                    }
                }
                break;
            case FBM_FRAME_RATE_60:
            default:
                ucPmxMode = (_prVdpConf[ucVdpId]->u4SrcHeight <= 768)? VDP_B2R_MODE_720P_1080W_60 :
                            (_prVdpConf[ucVdpId]->u4SrcHeight <= 1088)? VDP_B2R_MODE_1080P_60 : VDP_B2R_MODE_1440P_1280W_60;
                break;
#else
            case FBM_FRAME_RATE_24:
                ucPmxMode = VDP_B2R_MODE_1440P_1280W_24;
                break;
            case FBM_FRAME_RATE_25:
            case FBM_FRAME_RATE_50:
                ucPmxMode = VDP_B2R_MODE_1440P_1280W_50;
                break;
            case FBM_FRAME_RATE_30:
#ifdef CC_MT5399	 // for JPG 1024*768 timings!!!
                 ucPmxMode = VDP_B2R_MODE_1080P_30 ;
                 break;
#endif		
            case FBM_FRAME_RATE_60:
#ifdef CC_MT5399	 // for JPG 1024*768 timings!!!
                 ucPmxMode = VDP_B2R_MODE_1080P_60 ;
                break;
#endif				
            default:
                ucPmxMode = VDP_B2R_MODE_1440P_1280W_60;
                break;
#endif
            }

#ifdef CC_B2R_SENSIO_CONV
            if(prFrcPrm->fgB2R3DEnable && !prFrcPrm->fgB2RForce2D)
            {
                if(prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_24)
                {
                    ucPmxMode = VDP_B2R_MODE_1080P_48;
                }
                else if(prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_30)
                {
                    ucPmxMode = VDP_B2R_MODE_1080P_60;
                }
                else if(prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_25)
                {
                    ucPmxMode = VDP_B2R_MODE_1080P_50;
                }
            }
            LOG(1,"prVdecEsInfo->fgSensio3D=%d,->u4SrcWidth=%d,->u4SrcHeight=%d,prFrcPrm->ucInFrameRate=%d\n",prVdecEsInfo->fgSensio3D,_prVdpConf[ucVdpId]->u4SrcWidth,_prVdpConf[ucVdpId]->u4SrcHeight,prFrcPrm->ucInFrameRate);
            if(prVdecEsInfo->fgSensio3D)
            {
                if ((_prVdpConf[ucVdpId]->u4SrcWidth <= 960) &&(_prVdpConf[ucVdpId]->u4SrcHeight <= 1088))
                {
                    switch
                        (prFrcPrm->ucInFrameRate)
                        {
                            case
                                FBM_FRAME_RATE_24:
                                ucPmxMode = VDP_B2R_MODE_960_1080P_48;
                            break;

                            default:
                            ucPmxMode = VDP_B2R_MODE_1080P_24;
                            break;
                        }            
                }

                if (_prVdpConf[ucVdpId]->u4SrcHeight > 1080) 
                {
                    LOG(1, "Force Image Height %d %>> %1080\n", %_prVdpConf[ucVdpId]->u4SrcHeight); _prVdpConf[ucVdpId]->u4SrcHeight = 1080;
                }
                if (_prVdpConf[ucVdpId]->u4SrcWidth > 1920)
                {
                    LOG(1, "Force Image Width %d %>> %1920\n", %_prVdpConf[ucVdpId]->u4SrcWidth); _prVdpConf[ucVdpId]->u4SrcWidth = 1920;
                }
            }
#endif

        }

#if 1

        else if ((_prVdpConf[ucVdpId]->u4SrcWidth <= 1920) &&   //add for CR DTV00351047 when mm video is 1920*540(SBS) => v_total too large to do MJC 3D
                 (_prVdpConf[ucVdpId]->u4SrcHeight <= 720))
        {
            switch (prFrcPrm->ucInFrameRate)
            {
            case FBM_FRAME_RATE_24:
                ucPmxMode = VDP_B2R_MODE_720P_1080W_24;
                break;
            case FBM_FRAME_RATE_25:
                ucPmxMode = VDP_B2R_MODE_720P_1080W_25;
                break;
            case FBM_FRAME_RATE_50:
                ucPmxMode = VDP_B2R_MODE_720P_1080W_50;
                break;
            case FBM_FRAME_RATE_30:
                ucPmxMode = VDP_B2R_MODE_720P_1080W_30;
                break;
            case FBM_FRAME_RATE_60:
            default:
                ucPmxMode = VDP_B2R_MODE_720P_1080W_60;
                break;
            }
        }
        else if ((_prVdpConf[ucVdpId]->u4SrcWidth <= 2560) &&
                 (_prVdpConf[ucVdpId]->u4SrcWidth > 1920) &&
                 (_prVdpConf[ucVdpId]->u4SrcHeight <= 1088))
        {
            switch (prFrcPrm->ucInFrameRate)
            {
            case FBM_FRAME_RATE_24:
                ucPmxMode = VDP_B2R_MODE_1080P_ULTRAW_24 ;
                break;
            case FBM_FRAME_RATE_25:
                ucPmxMode = VDP_B2R_MODE_1080P_ULTRAW_25 ;
                break;
            case FBM_FRAME_RATE_50:
                ucPmxMode = VDP_B2R_MODE_1080P_ULTRAW_50 ;
                break;
            case FBM_FRAME_RATE_30:
                ucPmxMode = VDP_B2R_MODE_1080P_ULTRAW_30 ;
                break;
            default:
                break;
            }
        }
#endif
#ifdef CC_MT5399
     else if ((_prVdpConf[ucVdpId]->u4SrcWidth <= 3840) &&
			(_prVdpConf[ucVdpId]->u4SrcWidth >= 2560) &&
                 (_prVdpConf[ucVdpId]->u4SrcHeight <= 2160))
        {
            switch (prFrcPrm->ucInFrameRate)
            {
            case FBM_FRAME_RATE_15:  // FOR 4K2K JPEG
                ucPmxMode = VDP_B2R_MODE_4K2K_15;
                break;
            case FBM_FRAME_RATE_30:  // FOR 4K1K video
                ucPmxMode = VDP_B2R_MODE_4K2K_30;
				break;
			case FBM_FRAME_RATE_25:  // FOR 4K1K video
                ucPmxMode = VDP_B2R_MODE_4K2K_25;
				break;
			case FBM_FRAME_RATE_24:  // FOR 4K1K video
                ucPmxMode = VDP_B2R_MODE_4K2K_24;
				break;
            default:
				 ucPmxMode = VDP_B2R_MODE_4K2K_30;
                break;
            }
        }
	    else if ((_prVdpConf[ucVdpId]->u4SrcWidth <= 4096) &&
			(_prVdpConf[ucVdpId]->u4SrcWidth >= 3840) &&
                 (_prVdpConf[ucVdpId]->u4SrcHeight <= 2304))
        {
            switch (prFrcPrm->ucInFrameRate)
            {
            case FBM_FRAME_RATE_30: 
                ucPmxMode = VDP_B2R_MODE_EX_4K2K_30;
				break;
			case FBM_FRAME_RATE_25: 
                ucPmxMode = VDP_B2R_MODE_EX_4K2K_25;
				break;
			case FBM_FRAME_RATE_24: 
                ucPmxMode = VDP_B2R_MODE_EX_4K2K_24;
				break;
            default:
				 ucPmxMode = VDP_B2R_MODE_EX_4K2K_30;
                break;
            }
        }
	#endif
        else
        {
            switch (prFrcPrm->ucInFrameRate)
            {
            case FBM_FRAME_RATE_25:
#ifndef CC_B2R_ENABLE_OUTPUT_FRC
                ucPmxMode = VDP_B2R_MODE_1080P_25;
                break;
#endif
            case FBM_FRAME_RATE_50:
                ucPmxMode = VDP_B2R_MODE_1080P_50;
                break;
            case FBM_FRAME_RATE_24:
#ifndef CC_B2R_ENABLE_OUTPUT_FRC
                ucPmxMode = VDP_B2R_MODE_1080P_24;
                break;
#endif
            case FBM_FRAME_RATE_30:
#if !(defined(CC_B2R_ENABLE_OUTPUT_FRC)) && !(defined(SUPPORT_DISP_WEB_VIDEO_SMOOTHER))
                ucPmxMode = VDP_B2R_MODE_1080P_30;
                break;
#endif
               case FBM_FRAME_RATE_60:
                ucPmxMode = VDP_B2R_MODE_1080P_60;
                break;
                default:
                LOG(1,"-3>u4SrcWidth=%d,->u4SrcHeight=%d,->ucInFrameRate=%d\n",_prVdpConf[ucVdpId]->u4SrcWidth,_prVdpConf[ucVdpId]->u4SrcHeight,prFrcPrm->ucInFrameRate);
                ucPmxMode = VDP_B2R_MODE_1080P_30;
                break;
            }

            if (prFrcPrm->fgDoubleClock)
            {
                if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_24)
                {
                    ucPmxMode = VDP_B2R_MODE_1080P_48;
                }
                else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_30)
                {
                    ucPmxMode = VDP_B2R_MODE_1080P_60;
                }
                else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_25)
                {
                    ucPmxMode = VDP_B2R_MODE_1080P_50;
                }
            }

#ifdef CC_B2R_3D_SUPPROT
            if (prFrcPrm->fgB2R3DEnable && !prFrcPrm->fgB2RForce2D)
            {
                if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_24)
                {
                    ucPmxMode = VDP_B2R_MODE_1080P_48;
                }
                else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_30)
                {
                    ucPmxMode = VDP_B2R_MODE_1080P_60;
                }
                else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_25)
                {
                    ucPmxMode = VDP_B2R_MODE_1080P_50;
                }
            }
#endif

            if (_prVdpConf[ucVdpId]->u4SrcHeight > 1080)
            {
                LOG(1, "Force Image Height %d >> 1080\n", _prVdpConf[ucVdpId]->u4SrcHeight);
                _prVdpConf[ucVdpId]->u4SrcHeight = 1080;
            }
            if (_prVdpConf[ucVdpId]->u4SrcWidth > 1920)
            {
                LOG(1, "Force Image Width %d >> 1920\n", _prVdpConf[ucVdpId]->u4SrcWidth);
                _prVdpConf[ucVdpId]->u4SrcWidth = 1920;
            }

            //add for new resolution
#if 0
            if ( _prVdpConf[ucVdpId]->u4SrcWidth <1920 &&_prVdpConf[ucVdpId]->u4SrcHeight > 1080) // 1280x1440
            {
                switch (prFrcPrm->ucInFrameRate)
                {
                case FBM_FRAME_RATE_24:
                    ucPmxMode = VDP_B2R_MODE_1280p_24;
                case FBM_FRAME_RATE_25:
                case FBM_FRAME_RATE_50:
                case FBM_FRAME_RATE_30:
                case FBM_FRAME_RATE_60:
                default:
                    ucPmxMode = VDP_B2R_MODE_1280p_60;
                    break;
                }
            }
#endif
        }
    }
    else
    {
        // Interlace Source

        if (((prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_25) ||
                (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_50)) &&
                (_prVdpConf[ucVdpId]->u4SrcWidth <= 720) &&
                (_prVdpConf[ucVdpId]->u4SrcHeight <= 576))
        {
            ucPmxMode = VDP_B2R_MODE_576I_50;
            if (prFrcPrm->fgDoubleClock || (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_50))
            {
                ucPmxMode = VDP_B2R_MODE_576I_100;
            }
        }
        else if ((_prVdpConf[ucVdpId]->u4SrcWidth <= 720) &&
                 (_prVdpConf[ucVdpId]->u4SrcHeight <= 480))
        {
            ucPmxMode = VDP_B2R_MODE_480I_60;
            if (prFrcPrm->fgDoubleClock)
            {
                ucPmxMode = VDP_B2R_MODE_480I_120;
            }
        }
#ifdef CC_B2R_SENSIO_CONV
        else if ((_prVdpConf[ucVdpId]->u4SrcWidth <= 960) && (_prVdpConf[ucVdpId]->u4SrcHeight <= 1088)&&(prVdecEsInfo->fgSensio3D))
        {

            ucPmxMode = VDP_B2R_MODE_960_1080I_60;
            if (_prVdpConf[ucVdpId]->u4SrcHeight > 1080)
            {
                LOG(1, "Force Image Height %d >> 1080\n", _prVdpConf[ucVdpId]->u4SrcHeight);
                _prVdpConf[ucVdpId]->u4SrcHeight = 1080;
            }
            if (_prVdpConf[ucVdpId]->u4SrcWidth > 1920)
            {
                LOG(1, "Force Image Width %d %>> %1920\n", %_prVdpConf[ucVdpId]->u4SrcWidth);
                _prVdpConf[ucVdpId]->u4SrcWidth = 1920;
            }

        }
#endif
        else
        {
            if ((prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_25) ||
                    (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_50))
            {
                // 50 Hz
                ucPmxMode = VDP_B2R_MODE_1080I_50;
            }
            else if (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_24)
            {
                // 48 Hz
                ucPmxMode = VDP_B2R_MODE_1080I_48;
            }
            else
            {
                // 60 Hz
                ucPmxMode = VDP_B2R_MODE_1080I_60;
            }


            if (prFrcPrm->fgDoubleClock)
            {
                if ((prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_25) ||
                        (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_50))
                {
                    ucPmxMode = VDP_B2R_MODE_1080I_100;
                }
                else
                {
                    ucPmxMode = VDP_B2R_MODE_1080I_120;
                }
            }
            if (prFrcPrm->fgB2R3DEnable && !prFrcPrm->fgB2RForce2D)
            {
                if ((prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_25) ||
                        (prFrcPrm->ucInFrameRate == FBM_FRAME_RATE_50))
                {
                    //todo
                    ucPmxMode = VDP_B2R_MODE_1080I_100;
                }
                else
                {
                    ucPmxMode = VDP_B2R_MODE_1080I_120;
                }
            }
            if (_prVdpConf[ucVdpId]->u4SrcHeight > 1080)
            {
                LOG(1, "Force Image Height %d >> 1080\n", _prVdpConf[ucVdpId]->u4SrcHeight);
                _prVdpConf[ucVdpId]->u4SrcHeight = 1080;
            }
            if (prFrcPrm->u1B2R3DType == B2R_3D_SIDE_BY_SIDE_REAL_D &&  _prVdpConf[ucVdpId]->u4SrcHeight == 1080)
            {
                _prVdpConf[ucVdpId]->u4SrcHeight = 1078;
            }
            if (_prVdpConf[ucVdpId]->u4SrcWidth > 1920)
            {
                LOG(1, "Force Image Width %d >> 1920\n", _prVdpConf[ucVdpId]->u4SrcWidth);
                _prVdpConf[ucVdpId]->u4SrcWidth = 1920;
            }
        }
    }

    prFrcPrm->ucNotSupport = 0;

    // B2R Output Mode Change
    if (ucPmxMode != prOutInfo->ucPmxMode)
    {
        LOG(1, "B2R Output Change %d >> %d\n",
            prOutInfo->ucPmxMode,
            ucPmxMode);

        _u2VsyncNotStableCount[(UINT8)ucB2rId] = 0xFF;

//LEO_OUTPUTMODE_REFINE
        _VdpSetOutInfoByPmx(ucPmxMode,prOutInfo);

        prOutInfo->ucPmxMode = ucPmxMode;

        // Update vdp_frc parameter
        prFrcPrm->ucOutFrameRate = prOutInfo->ucFrameRate;
        prFrcPrm->ucProgressiveOut = prOutInfo->ucPrg;

        if (prFrcPrm->ucOutFrameRate)
        {
            prFrcPrm->u4OutStcPeriod = VDP_STC_CLOCK / prFrcPrm->ucOutFrameRate;
        }
        else
        {
            prFrcPrm->u4OutStcPeriod = (VDP_STC_CLOCK / 60);
        }

        _VDP_HalStcPeriod(ucB2rId,prFrcPrm->u4OutStcPeriod);

        // notify SCPOS
        _VDP_VsyncNotify(VDP_MSG_NOTIFY, (UINT32)ucVdpId, VDP_B2R_OUTPUT_CHG, 0);

        _u4ReturnValue |= VDP_EVENT_SEQ_CHG;
    }
}


//-----------------------------------------------------------------------------
/** Brief of _VdpErrNotify.
 */
//-----------------------------------------------------------------------------
static void _VdpErrNotify(UINT32 u4Arg1, UINT32 u4Arg2, UINT32 u4Arg3, UINT32 u4Arg4)
{
    UNUSED(u4Arg1);
    UNUSED(u4Arg2);
    UNUSED(u4Arg3);
    UNUSED(u4Arg4);
}

//-----------------------------------------------------------------------------
/** Brief of _VdpFrcFrameBufferReady.
 */
//-----------------------------------------------------------------------------
void _VdpFrcFrameBufferReady(UCHAR ucFbgId)
{
    UCHAR ucIdx;
    UCHAR ucB2rId;
    UINT32 u4CurrentLine;
    UCHAR ucCntL=0, ucCntR=0;
    UINT32 u4DelayTime = 3;
    FBM_PIC_HDR_T* prFbmPicHdr = NULL;

    // get video plane id
    ucB2rId = B2R_NS;
    for (ucIdx = 0; ucIdx < B2R_NS; ucIdx++)
    {
        if (_arVdpPrm[ucIdx].ucFbgId == ucFbgId)
        {
            while (u4DelayTime && (_arVdpPrm[ucIdx].ucPendingNewFb == 0) && (_arVdpPrm[ucIdx].ucRealChangeFb == 0))
            {
                //Still not change FB, but the PendingNewFb is not set. this means, the change FB is still running.
                x_thread_delay(1);
                u4DelayTime--;
            }
            if (_arVdpPrm[ucIdx].ucPendingNewFb != 0)
            {
                ucB2rId = ucIdx;
                ucIdx = B2R_NS;
            }
        }
    }
#if defined(CC_B2R_ENABLE_3D_LATE_NOTIFY) && defined(CC_3D_MM_DS_SUPPORT)
    if (ucB2rId == B2R_NS)
    {
        u4DelayTime = 3;
        for (ucIdx = 0; ucIdx < B2R_NS; ucIdx++)
        {
            if ((_arVdpPrm[ucIdx].ucSubFbgId != FBM_FBG_ID_UNKNOWN) && (_arVdpPrm[ucIdx].ucSubFbgId == ucFbgId))
            {
                while (u4DelayTime && (_arVdpPrm[ucIdx].ucPendingNewFb == 0) && (_arVdpPrm[ucIdx].ucRealChangeFb == 0))
                {
                    //Still not change FB, but the PendingNewFb is not set. this means, the change FB is still running.
                    x_thread_delay(1);
                    u4DelayTime--;
                }
                if (_arVdpPrm[ucIdx].ucPendingNewFb != 0)
                {
                    ucB2rId = ucIdx;
                    ucIdx = B2R_NS;
                }
            }
        }
    }
#endif

    if (ucB2rId >= B2R_NS)
    {
        return;
    }

    if (FBM_ChkFrameBufferFlag(ucFbgId, FBM_FLAG_RELEASE_FBG))
    {
        return;
    }

    _VDP_VsyncMutexLock();

    // After Semaphore Wait, ucPendingNewFb can be reset by B2R VSYNC
    ucCntL = FBM_CheckFrameBufferDispQ(_arVdpPrm[ucB2rId].ucFbgId);
#if defined(CC_3D_MM_DS_SUPPORT)
    ucCntR = FBM_CheckFrameBufferDispQ(_arVdpPrm[ucB2rId].ucSubFbgId);
#endif

    prFbmPicHdr = FBM_GetFrameBufferPicHdr(_arVdpPrm[ucB2rId].ucFbgId, ucCntL);

    if (_arVdpPrm[ucB2rId].fgB2RNoLock && prFbmPicHdr && !prFbmPicHdr->fgSeamlessDisp)
    {
        //PS: This pic hdr is not the next display FB while stage fright.
        //Just get the seamless info here.
        _VDP_VsyncMutexUnlock();
        return;
    }

#if defined(CC_B2R_ENABLE_3D_LATE_NOTIFY) && defined(CC_3D_MM_DS_SUPPORT)
    if (_arVdpPrm[ucB2rId].ucPendingNewFb == 0
            || (_arVdpPrm[ucB2rId].fgB2R3DEnable && (!((ucCntL > 0) && (ucCntR > 0)))))
#else
    if (_arVdpPrm[ucB2rId].ucPendingNewFb == 0 || _arVdpPrm[ucB2rId].fgB2R3DEnable)
#endif
    {
        if (_arVdpPrm[ucB2rId].ucPendingNewFb > 0)
        {
            LOG(1, "Pending dispQ %d/%d not ready\n", ucCntL, ucCntR);
        }
        _VDP_VsyncMutexUnlock();
        return;
    }

    // reset pending (do it once)
    _arVdpPrm[ucB2rId].ucPendingNewFb = 0;

    // check active region (can we stilll update frame buffer now? or It is too late to update)
    u4CurrentLine = _VDP_HalInsideBlankRegion(ucB2rId);

    if (u4CurrentLine == 0)
    {
        // frame rate config core
        switch (_arVdpPrm[ucB2rId].ucMode)
        {
        case VDP_MODE_DEINT:
        case VDP_MODE_NORMAL:
        case VDP_MODE_BG:            // Background Mode, do nothing

#ifdef CC_B2R_ENABLE_3D_LATE_NOTIFY
            if (_arVdpPrm[ucB2rId].fgB2R3DEnable)
            {
                LOG(2,"late notify frame in 3D case\n");
            }
#endif

            _arVdpPrm[ucB2rId].ucNewFbArrive = 1;
            _VdpChangeFrameBuffer(&_arVdpPrm[ucB2rId]);

            ASSERT(_arVdpPrm[ucB2rId].ucFbId != FBM_FB_ID_UNKNOWN);
//                LOG(2, "FBR (%d) (%d)\n", _arVdpPrm[ucVdpId].ucFbId, _arVdpPrm[ucVdpId].prFbCounter->u4DispFail);
            break;

        default:
            break;
        }
    }
    else
    {
        LOG(2, "FB late (%d)\n", u4CurrentLine);
#ifdef TIME_MEASUREMENT
        //u4FBLateCnt++;
#endif
    }

    _VDP_VsyncMutexUnlock();
}


#ifdef CC_FRC_TEST
//-----------------------------------------------------------------------------
/** Brief of _VdpFrcTest.
 */
//-----------------------------------------------------------------------------
static void _VdpFrcTest(B2R_PRM_T* prFrcPrm)
{
    static UINT32 u4FrcTest = 0;

    if (prFrcPrm->ucReady == 0)
    {
        return;
    }

    u4FrcTest++;

#if 1
    if (u4FrcTest >= 128)
    {
        prFrcPrm->u2TargetNs = 20;
        prFrcPrm->u2ChangeFieldNs = 10;

        u4FrcTest = 0;
    }
#else
    if ((u4FrcTest >= 32) && (prFrcPrm->ucFbId == 0))
    {
        HAL_TIME_T rResult;

        HAL_GetTime(&rResult);

        prFrcPrm->u2TargetNs = (rResult.u4Micros & 0xF);

        if (prFrcPrm->u2TargetNs <= 2)
        {
            prFrcPrm->u2TargetNs = 3;
        }

        prFrcPrm->u2ChangeFieldNs = (rResult.u4Seconds % prFrcPrm->u2TargetNs);

        u4FrcTest = 0;
    }
#endif
}
#endif

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** Brief of _VDP_FrcSetPtsCb.
 */
//-----------------------------------------------------------------------------
void _VDP_FrcSetPtsCb(UCHAR ucB2rId, UCHAR ucPort, UINT32 u4CbPts, UINT32 u4CbArg)
{
    UNUSED(ucPort);

    if (ucB2rId < B2R_NS)
    {
        _arVdpPrm[ucB2rId].u4CbPtsEn = 1;
        _arVdpPrm[ucB2rId].u4CbPts = u4CbPts;
        _arVdpPrm[ucB2rId].u4CbArg = u4CbArg;
        _arVdpPrm[ucB2rId].u4CbCntDown = 0;
        _arVdpPrm[ucB2rId].u4CbLastPts = _arVdpPrm[ucB2rId].u4Pts;

        return;
    }

    ASSERT(0);
}

static UINT32 _VDP_FrcGetStc(UCHAR ucFbgId)
{
    UCHAR ucAvSyncMode  = 0x0;
    UCHAR ucStcSrc      = 0x0;

    FBM_GetSyncStc(ucFbgId, &ucAvSyncMode, &ucStcSrc);

#ifdef CC_53XX_SWDMX_V2
    return STC_GetStcValue(ucStcSrc);
#else
    return STC_GetStcValue();
#endif
}


//-----------------------------------------------------------------------------
/** Brief of _VDP_FrcSetPtsCb.
 */
//-----------------------------------------------------------------------------
void _VDP_FrcSetLipSyncCb(UCHAR ucB2rId, UCHAR ucPort, UINT32 u4Thrsd, UINT32 u4FrmCnt)
{
    UNUSED(ucPort);

    if (ucB2rId < B2R_NS)
    {
        _arVdpPrm[ucB2rId].u4CbSyncEn = 1;
        _arVdpPrm[ucB2rId].u4CbSyncThrsd = (u4Thrsd * 90000);
        _arVdpPrm[ucB2rId].u4CbSyncChkFrmCnt = u4FrmCnt;

        LOG(9, "(Thrsd: %d, FrmCnt:%d\n", u4Thrsd, u4FrmCnt);
        return;
    }

    ASSERT(0);
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_FrcSetLipSyncNGOKCb.
 */
//-----------------------------------------------------------------------------
void _VDP_FrcSetLipSyncNGOKCb(UCHAR ucNgOk, UCHAR ucRemove, UINT32 u4Thrsd, UINT32 u4FrmCnt)
{
    if (ucRemove == 0)
    {
        if (ucNgOk == VDP_LIPSYNC_REG_NG_TO_OK)
        {
            _rVdpLipSync.ucNg2OkCbEn = 1;
            _rVdpLipSync.u4OKThrd = (u4Thrsd*90);
            _rVdpLipSync.u4NumFrmsOK = u4FrmCnt;
            _rVdpLipSync.u4OKFrmCnt = 0;
            _rVdpLipSync.ucLipSyncState = VDP_LIP_SYNC_NG;

            LOG(9, "Lip Sync NG to OK Thrd(%d), FrmCnt(%d)\n", u4Thrsd, u4FrmCnt);
        }
        else if (ucNgOk == VDP_LIPSYNC_REG_OK_TO_NG)
        {
            _rVdpLipSync.ucOk2NgCbEn = 1;
            _rVdpLipSync.u4NGThrd = (u4Thrsd*90);
            _rVdpLipSync.u4NumFrmsNG = u4FrmCnt;
            _rVdpLipSync.u4NGFrmCnt = 0;
            _rVdpLipSync.ucLipSyncState = VDP_LIP_SYNC_OK;

            LOG(9, "Lip Sync OK to NG Thrd(%d), FrmCnt(%d)\n", u4Thrsd, u4FrmCnt);
        }
        else
        {
            ASSERT(0);
        }
    }
    else
    {
        if (ucNgOk == VDP_LIPSYNC_REG_NG_TO_OK)
        {
            _rVdpLipSync.ucNg2OkCbEn = 0;
            LOG(9, "Remove Lip Sync NG to OK callback\n", u4Thrsd, u4FrmCnt);
        }
        else if (ucNgOk == VDP_LIPSYNC_REG_OK_TO_NG)
        {
            _rVdpLipSync.ucOk2NgCbEn = 0;
            LOG(9, "Remove Lip Sync OK to NG callback\n", u4Thrsd, u4FrmCnt);
        }
        else
        {
            ASSERT(0);
        }
        UNUSED(_rVdpLipSync);
    }
}


//-----------------------------------------------------------------------------
/** Brief of _VDP_FrcInit.
 */
//-----------------------------------------------------------------------------
void _VDP_FrcInit(void)
{
    UCHAR ucVdpId;
    UCHAR ucB2rId;
    CRIT_STATE_T _rCritState;

    _prDbase = DBS_Lock(&_rCritState);
    ASSERT(_prDbase);
    VERIFY(DBS_Unlock(_prDbase, _rCritState));

    for (ucVdpId = 0; ucVdpId < VDP_NS; ucVdpId++)
    {
        _prVdpConf[ucVdpId] = VDP_GetConf(ucVdpId);
    }
    for (ucB2rId = 0; ucB2rId < B2R_NS; ucB2rId++)
    {
        _arVdpPrm[ucB2rId].ucMode = VDP_MODE_BG;

#ifdef CC_B2R_ENABLE_CHG_FRAME_MSG
        if ((_ahChgFrameQueue[ucB2rId]) == (HANDLE_T)(NULL))
        {
            CHAR szBuf[16];
            x_snprintf(szBuf, sizeof(szBuf), "ChgFrmQ%d", ucB2rId);

            VERIFY(x_msg_q_create(&_ahChgFrameQueue[ucB2rId], szBuf, sizeof(VDP_B2R_CHG_FRAME_MSG_T), 64) == OSR_OK);
            VERIFY(x_sema_create(&_ahChgFrameMutex[ucB2rId], X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);
        }
#endif
        _afgPendingChgFrmMsg[ucB2rId] = FALSE;
        x_memset(&(_arPendingChgFrmMsg[ucB2rId]), 0, sizeof(VDP_B2R_CHG_FRAME_MSG_T));
        VERIFY(x_sema_create(&(_hDispBufLockMutex[ucB2rId]), X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);
        _VdpFrmInit(ucB2rId, 0, TRUE);
    }

    /* FRC COPY parameter initialization */
    _rFrcCopy.u4TriggerCopy = 0;
    FRC_MEMSET((void *)&(_rFrcCopy), 0, sizeof(B2R_PRM_T));

#ifdef CC_B2R_ENABLE_SEAMLESS_THREAD
    // create seamless playback thread
    if (!_fgSeamlessThreadInit)
    {
        _fgSeamlessThreadInit = TRUE;

        // create semaphore
        VERIFY(x_sema_create(&_hSeamlessSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);

        VERIFY(x_thread_create(&_hSeamlessThread, "B2R_Sealmess_Thread", VSYNC_STACK_SIZE, VSYNC_THREAD_PRIORITY,
                               _SeamlessRoutine, 0, NULL)== OSR_OK);
    }
#endif
}

static UINT32 _VDP_GetInput(UCHAR ucB2rId, UCHAR ucPort, UCHAR* pucFbgId)
{
    UCHAR ucCnt, ucIdx;
    UCHAR aucVdpIds[VDP_NS];
    UCHAR ucVdpId;
    if (ucPort >= VDP_MAX_INPORT_NS || !pucFbgId)
    {
        return VDP_SET_ERROR;
    }
    ucCnt = VDP_GetVdpIds(ucB2rId, aucVdpIds);
    for (ucIdx = 0; ucIdx < ucCnt; ucIdx++)
    {
        ucVdpId = aucVdpIds[ucIdx];
        if (_prVdpConf[ucVdpId]->ucInputPort[ucPort] < MAX_ES_NS)
        {
            *pucFbgId = FBM_GetFbgByEs(_prVdpConf[ucVdpId]->ucInputPort[ucPort]);
            return VDP_SET_OK;
        }
        else
        {
            *pucFbgId = FBM_FBG_ID_UNKNOWN;
        }
    }
    return VDP_SET_OK;
}


//-----------------------------------------------------------------------------
/** Brief of _VDP_FrcHandlerEvent.
 */
//-----------------------------------------------------------------------------
UINT32 _VDP_FrcHandlerEvent(UCHAR ucB2rId, UINT32 u4Event)
{
    B2R_PRM_T* _pFrcPrm;
    UCHAR ucFbgId = 0xFF;
    UCHAR ucOldFbgId;
    UCHAR ucVdpId = VDP_GetVdpId(ucB2rId);
    if (ucVdpId >= VDP_NS || ucB2rId >= B2R_NS)
    {
        return 0;
    }

    // Handle configuration(input/output) changes
    u4Event &= (VDP_EVENT_MASTER_CHG |
                VDP_EVENT_INPUT_CHG |
                VDP_EVENT_OUTPUT_CHG |
                VDP_EVENT_MODE_CHG |
                VDP_EVENT_ENABLE_CHG);

#ifdef CC_SUPPORT_TVE
    // Handle  Main change from DTV to other source when TV Scart out.
    if ((_prVdpConf[VDP_1]->ucB2rId == B2R_1) &&
            (_prVdpConf[VDP_2]->ucB2rId == B2R_1)&&
            (_prVdpConf[VDP_1]->ucEnable == 0) &&
            (_prVdpConf[VDP_2]->ucEnable == 1) )
    {
        //LOG(0, "_VDP_FrcHandlerEvent _VDP_CopyFrcPrm\n\r");
        _rFrcCopy.u4TriggerCopy = 1;
        x_memcpy(&_rFrcCopy,  &_arVdpPrm[VDP_1], sizeof(B2R_PRM_T));
        //_VDP_CopyFrcPrm(VDP_1);
    }
#endif

    while (u4Event)
    {
        if (u4Event & VDP_EVENT_MASTER_CHG)
        {
            u4Event &= ~VDP_EVENT_MASTER_CHG;
        }

        if (u4Event & VDP_EVENT_ENABLE_CHG)
        {
            u4Event &= ~VDP_EVENT_ENABLE_CHG;
            if (!(_prVdpConf[ucVdpId]->ucEnable))
            {
                // clean port
                _pFrcPrm = &_arVdpPrm[ucB2rId];

                // clean Fbg & VdpPrm
                if (_pFrcPrm->ucFbgId != FBM_FBG_ID_UNKNOWN)
                {

                    if ((_prVdpConf[VDP_1]->ucEnable == 0) &&
                            (_prVdpConf[VDP_2]->ucEnable == 0))
                    {
                        FBM_FlushLockFrameBuffer(_pFrcPrm->ucFbgId);
                        FBM_ClrFrameBufferFlag(_pFrcPrm->ucFbgId, FBM_FLAG_DISPLAYING);
                    }

                    //disable vpd before stop vdec  and need to release resource which is locked by B2R
                    //ignore triggerCopy == 1
                    if (_rFrcCopy.u4TriggerCopy == 0)
                    {
                        _VdpRelResource(ucB2rId);
                    }

                    _VdpFrmInit(ucB2rId, 0, FALSE);
                }

                _VDP_HalForceBg(ucB2rId, 1);
            }

            // update input information
            u4Event |= VDP_EVENT_INPUT_CHG;

            // update output information
            u4Event |= VDP_EVENT_OUTPUT_CHG;

            // update mode information
            u4Event |= VDP_EVENT_MODE_CHG;

            LOG(7, "Enable change\n");
        }

        if (u4Event & VDP_EVENT_INPUT_CHG)
        {
            u4Event &= ~VDP_EVENT_INPUT_CHG;

            VERIFY(_VDP_GetInput(ucB2rId, 0, &ucFbgId) == VDP_SET_OK);
            _pFrcPrm = &_arVdpPrm[ucB2rId];

            if (ucFbgId != _pFrcPrm->ucFbgId)
            {
                ucOldFbgId = _pFrcPrm->ucFbgId;

                // clean old fbg
                if (ucOldFbgId != FBM_FBG_ID_UNKNOWN)
                {

                    if ((_prVdpConf[VDP_1]->ucEnable == 0) &&
                            (_prVdpConf[VDP_2]->ucEnable == 0))
                    {
                        FBM_FlushLockFrameBuffer(ucFbgId);
                    }
                }

                // clean new fbg
                if (ucFbgId != FBM_FBG_ID_UNKNOWN)
                {

                    if ((_prVdpConf[VDP_1]->ucEnable == 0) &&
                            (_prVdpConf[VDP_2]->ucEnable == 0))
                    {
                        FBM_FlushLockFrameBuffer(ucFbgId);
                    }
                }

                _VdpFrmInit(ucB2rId, 0, FALSE);
                _pFrcPrm->ucFbgId = ucFbgId;

                // update output information
                u4Event |= VDP_EVENT_OUTPUT_CHG;

                // update mode information
                u4Event |= VDP_EVENT_MODE_CHG;

                LOG(7, "Input change\n");
            }
        }

        if (u4Event & VDP_EVENT_MODE_CHG)
        {
            u4Event &= ~VDP_EVENT_MODE_CHG;

#ifndef FBM_VIRTUAL_MPEG
            _arVdpPrm[ucB2rId].ucMode = _prVdpConf[ucVdpId]->ucMode;
#endif

#if 0
            FBM_FlushLockFrameBuffer(_arVdpPrm[ucVdpId].ucFbgId);

            VDP_FrcResetPort(ucVdpId, 0);

            // update output information
            u4Event |= VDP_EVENT_OUTPUT_CHG;
#endif
        }

        if (u4Event & VDP_EVENT_OUTPUT_CHG)
        {
#ifdef CC_SCPOS_EN
            u4Event &= ~VDP_EVENT_OUTPUT_CHG;
#else

            u4Event &= ~VDP_EVENT_OUTPUT_CHG;

            _arVdpPrm[ucB2rId].ucOutFrameRate = _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate;
            _arVdpPrm[ucB2rId].ucProgressiveOut = _prVdpConf[ucVdpId]->rOutInfo.ucPrg;

            if (_prVdpConf[ucVdpId]->rOutInfo.ucFrameRate)
            {
                _arVdpPrm[ucB2rId].u4OutStcPeriod = VDP_STC_CLOCK / _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate;
            }
            else
            {
                _arVdpPrm[ucB2rId].u4OutStcPeriod = (VDP_STC_CLOCK / 60);
            }

            LOG(7, "Output change (%d, %d, %d)\n", _prVdpConf[ucVdpId]->rOutInfo.ucPmxId, _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate, _prVdpConf[ucVdpId]->rOutInfo.ucPrg);
#endif
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_FrcProc.
 */
//-----------------------------------------------------------------------------
UINT32 _VDP_FrcProc(UCHAR ucB2rId, UCHAR ucBottom, UCHAR ucRightView)
{
    BOOL fg3DInterlace = FALSE;
    BOOL fg3DIChgFrm = FALSE;
    B2R_PRM_T* prFrcPrm;
    UCHAR ucVdpId;
#ifdef CC_SUPPORT_FULL_3D_PHOTO
   
   UINT32 u4AddrY, u4AddrC;
   UINT32 u4Width, u4Height, u4Pitch;
   FBM_SEQ_HDR_T* prSeqHdr;
   UCHAR ucVsyncIdx ;
#endif
#ifdef __MODEL_slt__
    //HAL_TIME_T _rTime;
    //HAL_GetTime(&_rTime);
    //Printf("1-%06u\n", _rTime.u4Micros);
    b2rState = 1;
#endif

    if (ucB2rId >= B2R_NS)
    {
        return 0;
    }
    ucVdpId = VDP_GetVdpId(ucB2rId);
    if (ucVdpId >= VDP_NS)
    {
        return 0;
    }

    VDP_HalSetB2RMirrorFlip(ucB2rId);

    // clear return value
    _u4ReturnValue = 0;

    if ((_arVdpPrm[ucB2rId].fgB2R3DEnable) && (!_arVdpPrm[ucB2rId].fgB2RForce2D)
            /*&& (_arVdpPrm[ucVdpId].u1B2R3DType == B2R_3D_FRAME_SEQUENTIAL)*/
            && _arVdpPrm[ucB2rId].ucProgressiveOut ==1)
    {
        if (ucRightView ==1)
        {
            LOG(10,"FrcProc R ");
        }
        else
        {
            LOG(10,"FrcProc L ");
        }

        LOG(10,"%d\n",_arVdpPrm[ucB2rId].u2CurrentVsyncNs);

        if ((_arVdpPrm[ucB2rId].u2CurrentVsyncNs & 0x1) == 1)
        {
#ifdef CC_SECOND_B2R_SUPPORT
            VDP_HalSetLRState(ucB2rId,FALSE);
#else
            VDP_HalSetLRState(FALSE);
#endif
        }
        else
        {
#ifdef CC_SECOND_B2R_SUPPORT
            VDP_HalSetLRState(ucB2rId,TRUE);
#else
            VDP_HalSetLRState(TRUE);
#endif
        }
    }

#if 1//def CC_B2R_3D_MVC_INTERLACE_SUPPORT
    else if (_arVdpPrm[ucB2rId].fgB2R3DEnable && !_arVdpPrm[ucB2rId].fgB2RForce2D
             && ((_arVdpPrm[ucB2rId].u1B2R3DType == B2R_3D_FRAME_SEQUENTIAL)
#ifdef CC_B2R_SENSIO_CONV
                ||(_arVdpPrm[ucVdpId].rPortPrm.u1B2R3DType == B2R_3D_SIDE_BY_SIDE)
#endif
             )&& _arVdpPrm[ucB2rId].ucProgressiveOut ==0)
    {
        fg3DInterlace = TRUE;
        if (!ucBottom && !ucRightView)
        {
            fg3DIChgFrm = TRUE;
        }
        //if(_arVdpPrm[ucVdpId].u2CurrentVsyncNs == 1 || _arVdpPrm[ucVdpId].u2CurrentVsyncNs==3 || _arVdpPrm[ucVdpId].u2CurrentVsyncNs==5)
        /*if ((_arVdpPrm[ucB2rId].u2CurrentVsyncNs & 0x1) == 1)
        {
#ifdef CC_SECOND_B2R_SUPPORT
            VDP_HalSetLRState(ucB2rId,FALSE);
#else
            VDP_HalSetLRState(FALSE);
#endif
        }
        else
        {
#ifdef CC_SECOND_B2R_SUPPORT
            VDP_HalSetLRState(ucB2rId,TRUE);
#else
            VDP_HalSetLRState(TRUE);
#endif
        }

        if (_arVdpPrm[ucB2rId].u2CurrentVsyncNs ==0)
        {
#ifdef CC_SECOND_B2R_SUPPORT
            VDP_HalSetTBState(ucB2rId,TRUE);
#else
            VDP_HalSetTBState(TRUE);
#endif
        }*/
        /*if(_arVdpPrm[ucVdpId].u2CurrentVsyncNs ==0 && _arVdpPrm[ucVdpId].ucReady == 1)
        {
            if(!((ucRightView==1) && (ucBottom==1)))
            {
                return 0;
            }
        }*/

    }
#endif
 #ifdef CC_SUPPORT_FULL_3D_PHOTO
	
 if(_arVdpPrm[ucVdpId].fg4K2K)
	{
	  
		prSeqHdr = FBM_GetFrameBufferSeqHdr(_arVdpPrm[ucVdpId].ucFbgId);
		FBM_GetFrameBufferAddr(_arVdpPrm[ucVdpId].ucFbgId,_arVdpPrm[ucVdpId].ucFbId, &u4AddrY, &u4AddrC);
		ucVsyncIdx = _arVdpPrm[ucVdpId].u2CurrentVsyncNs;
			
       if (prSeqHdr)
	   {
		u4Width = prSeqHdr->u2HSize;
		u4Height = prSeqHdr->u2VSize;
		u4Pitch = prSeqHdr->u2LineSize;
		 switch (ucVsyncIdx)
		  {
		   case LEFT_TOP:
			
			 VDP_HalSetLRState(ucVdpId,FALSE);
			_VDP_HalSetAddr(ucVdpId, u4AddrY, u4AddrC);
			 LOG(2,"-----LEFT_TOP-u4AddrY=%x,u4AddrC=%x,u4Width=%d,ucFbgId=%d,ucFbId=%d\n",u4AddrY,u4AddrC,u4Width,_arVdpPrm[ucVdpId].ucFbgId,_arVdpPrm[ucVdpId].ucFbId);
				break;
		   case RIGHT_TOP :
			
			 VDP_HalSetLRState(ucVdpId,TRUE);
			u4AddrY += u4Width / 2;
			u4AddrC += u4Width / 2;
			_VDP_HalSetRViewAddr(ucVdpId, u4AddrY, u4AddrC);
		    LOG(2,"-----RIGHT_TOP-u4AddrY=%x,u4AddrC=%x,u4Width=%d,ucFbgId=%d,ucFbId=%d\n",u4AddrY,u4AddrC,u4Width,_arVdpPrm[ucVdpId].ucFbgId,_arVdpPrm[ucVdpId].ucFbId);
				break;
		 
			default:
			   break;
		  }
	   }
			 
	}
#endif
    // Frame Rate Control Process is NOT required if NOT enable
#ifdef CC_SUPPORT_TVE
    if ((!(_prVdpConf[ucVdpId]->ucEnable)) && (!(_prVdpConf[1-ucVdpId]->ucEnable)))
    {
        return _u4ReturnValue;
    }
#else
    if (!(_prVdpConf[ucVdpId]->ucEnable))
    {
        return _u4ReturnValue;
    }
#endif  
    // backup the current field information
    _arVdpPrm[ucB2rId].ucBottom = ucBottom;
    // reset pending
    _arVdpPrm[ucB2rId].ucPendingNewFb = 0;
    prFrcPrm = &_arVdpPrm[ucB2rId];
    
    if (_prVdpConf[ucVdpId]->ucEarlyDisp == 1)
    {
        if (prFrcPrm->u4WaitUnmute != 0)
        {
            prFrcPrm->u4WaitUnmute--;
#ifdef TIME_MEASUREMENT
            if(fgB2REarlyDisp)
            {
                TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME, "B2R Wait Early display");
                fgB2REarlyDisp=FALSE;
            }
#endif        
        }
        else
        {
#ifdef TIME_MEASUREMENT
            TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME, "B2R Early display Unmute");
#endif        
            _vDrvVideoSetMute(MUTE_MODULE_DTV, ucVdpId, 0, FALSE);
            _prVdpConf[ucVdpId]->ucEarlyDisp = 0;
            MMPlaybackSpeedTestAddTimeStamp(TIME_STAMP_VDP_DISPLAY_FIRST);
            LOG(3, "DTV Unmute Early Display -1 VDP(%d)\n", ucVdpId);

#ifdef CC_SUPPORT_TVE
            /* DTV SCART out application, Main and Sub are both attached to B2R */
            /* 1 Sub is connected */
            /* 2 Sub's ucEarlyDisp is set */
            /* 3 Main is connected */
            /* In above case, sub(SCART out) is never un-mute */
            /* 20081120: While Main's ucEarlyDisp, Sub's will also be set for Mute */
            if (_prVdpConf[1-ucVdpId]->ucEarlyDisp == 1)
            {
                _vDrvVideoSetMute(MUTE_MODULE_DTV, (1 - ucVdpId), 0, FALSE);
                LOG(3, "DTV Unmute Early Display -2 VDP(%d)\n", ucVdpId);
                _prVdpConf[1-ucVdpId]->ucEarlyDisp = 0;
            }
#endif

#if 0
            {
                STC_CLOCK rStcClk;

                STC_GetSrc(0, &rStcClk);
                LOG(1, "DTV Unmute PTS(0x%x) STC(0x%x)\n", prFrcPrm->u4Pts, rStcClk.u4Base);
            }
#endif
        }
    }
    
    // FBG pending release
    if (FBM_ChkFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_RELEASE_FBG))
    {
        FBM_ReleaseDispQ(prFrcPrm->ucFbgId);

        if (prFrcPrm->ucReady != 0)
        {
            LOG(1, "FBM_FLAG_RELEASE_FBG\n");

            if (prFrcPrm->ucReleaseFbId != FBM_FB_ID_UNKNOWN)
            {
                _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucFbgId, prFrcPrm->ucReleaseFbId, FBM_FB_STATUS_EMPTY);
                prFrcPrm->ucReleaseFbId = FBM_FB_ID_UNKNOWN;
            }

            if (prFrcPrm->ucFbId != FBM_FB_ID_UNKNOWN)
            {
                _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_FB_STATUS_EMPTY);
                prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
            }

            prFrcPrm->ucReleased = 1;
            prFrcPrm->ucReady = 0;

            _VDP_HalForceBg(ucB2rId, 1);

            _VDP_VsyncNotify(VDP_MSG_NOTIFY, (UINT32)(VDP_GetVdpId(ucB2rId)), VDP_B2R_NO_SIGNAL, 0);
        }

        return _u4ReturnValue;
    }
    // frame rate config core
    switch (_arVdpPrm[ucB2rId].ucMode)
    {
    case VDP_MODE_DEINT:        // Deinterlace Mode, Temp
    case VDP_MODE_NORMAL:        // Normal Mode
    case VDP_MODE_BG:            // Background Mode, do nothing
        // reference 8105 main_vdo.c (vMVdoDispProc)
        // single input port at normal mode, master port only

        // call back protection
        if ((prFrcPrm->u4CbPtsEn) && (prFrcPrm->ucReady))
        {
            if (++(prFrcPrm->u4CbCntDown) >= VDP_VSYNC_CB_THD)
            {
                UCHAR ucVdpId = VDP_GetVdpId(ucB2rId);
                _VDP_VsyncNotify(VDP_MSG_CC_CB, (UINT32)ucVdpId, prFrcPrm->u4CbPts, (prFrcPrm->u4CbArg | VDP_CB_ERR));

                prFrcPrm->u4CbPtsEn = 0;

                LOG(5, "P1(%x)\n", prFrcPrm->u4CbPts);
            }
        }

        // check fbm flag
        if (FBM_ChkFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_CLEAR_FRC_PRM))
        {
            FBM_ClrFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_CLEAR_FRC_PRM);
            VDP_FrcResetPort(ucB2rId, 0);
        }

        // Increase Current Vsync Ns here or at ISR ?
        FRC_INC_VSYNC_NS(prFrcPrm);

        // 5381 Double Frame, Delay 1 VSync to free frame buffer
        // release last frame buffer immediately
        if (prFrcPrm->ucReleaseFbId != FBM_FB_ID_UNKNOWN)
        {
            if (prFrcPrm->ucReleased == 0)
            {
                _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucFbgId, prFrcPrm->ucReleaseFbId, FBM_FB_STATUS_EMPTY);
            }
            prFrcPrm->ucReleased = 0;

            prFrcPrm->ucReleaseFbId = FBM_FB_ID_UNKNOWN;
            
            prFrcPrm->ucRealChangeFb = 0;
        }

#ifdef CC_3D_MM_DS_SUPPORT
        if (prFrcPrm->ucSubFbgId != FBM_FBG_ID_UNKNOWN && prFrcPrm->ucSubReleaseFbId != FBM_FB_ID_UNKNOWN)
        {
            if (prFrcPrm->ucSubReleased == 0)
            {
                _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucSubFbgId, prFrcPrm->ucSubReleaseFbId, FBM_FB_STATUS_EMPTY);
            }
            prFrcPrm->ucSubReleased = 0;

            prFrcPrm->ucSubReleaseFbId = FBM_FB_ID_UNKNOWN;
        }
#endif
#ifdef CC_SUPPORT_NPTV_SEAMLESS 
        if (prFrcPrm->ucSeamlessVDPMode == VDP_SMLS_FRAME_COMING)
        {
            _VDP_NPTVSeamlessStatus(prFrcPrm, VDP_SMLS_MODE_NONE);
        }
#endif

#ifdef VDP_B2R_PSCAN_CRC_FLOW
        if ((prFrcPrm->u4Pts==_u4SetPts) &&
                (((prFrcPrm->ucProgressiveOut == 0) && (FRC_CHANGE_FIELD(prFrcPrm))) ||
                 ((prFrcPrm->ucProgressiveOut != 0) && (prFrcPrm->u2CurrentVsyncNs == 1)))
           )
        {
            _rVdpB2rCrc.u4DramChkSum = u4DRAMChkSum;
            _rVdpB2rCrc.u4PixelChkSum = u4PixelChkSum;
            _rVdpB2rCrc.u4PSCANSrcCrc = u4PSCANSrcCrc;
            _rVdpB2rCrc.u4PSCANCrc = u4PSCANCrc;

            LOG(0, "(PTS, Bd, Bp, PcSrc, Pc) = (%08X, %08X, %08X, %08X, %08X)\n", prFrcPrm->u4Pts,
                _rVdpB2rCrc.u4DramChkSum, _rVdpB2rCrc.u4PixelChkSum, _rVdpB2rCrc.u4PSCANSrcCrc, _rVdpB2rCrc.u4PSCANCrc);
        }
#endif
        if (FRC_DISP_FINISH(prFrcPrm) && (!fg3DInterlace || (fg3DInterlace && fg3DIChgFrm)))
        {
            if (prFrcPrm->u2RestoreNs != 0)
            {
                // Restore target NS after repeat done. It is prevent unexpected repeat forever.
                prFrcPrm->u2TargetNs = prFrcPrm->u2RestoreNs;
                prFrcPrm->u2RestoreNs = 0;
            }
            
            #ifdef CC_SUPPORT_NPTV_SEAMLESS 
            if ((prFrcPrm->ucSeamlessVDPMode == VDP_SMLS_PREPARE) && bVRMReadyForB2R(ucVdpId))
            {
                _VDP_NPTVSeamlessStatus(prFrcPrm, VDP_SMLS_READY);
            }
            else if ((prFrcPrm->ucSeamlessVDPMode == VDP_SMLS_PREPARE_WHILE_START_PLAY) && bVRMReadyForB2R(ucVdpId))
            {
                _VDP_NPTVSeamlessStatus(prFrcPrm, VDP_SMLS_READY_WHILE_START_PLAY);
            }
            #endif
            
            _VdpChangeFrameBuffer(prFrcPrm);
            
            #ifdef CC_SUPPORT_NPTV_SEAMLESS 
            if (prFrcPrm->ucSeamlessVDPMode == VDP_SMLS_FRAME_COMING)
            {
                _VDP_HalSetPicth(ucB2rId, _prVdpConf[ucVdpId]->u4SrcLineSize);
                _VDP_HalTriggerSetResolution(ucB2rId);
                vVRMB2RTrigger(ucVdpId);
            }
            #endif

            if (prFrcPrm->fgRenderRegionChg)
            {
                _VDP_SetXYOffset(ucB2rId);
                prFrcPrm->fgRenderRegionChg = FALSE;
            }

#ifdef VDP_B2R_PSCAN_CRC_FLOW
            if (prFrcPrm->u4Pts == _u4SetPts)
            {
                /* Calculate PSCAN CRC at specified frame */
#if !(defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881) || defined(CC_MT5399))
                // Clear PSCAN CRC
                //IO_WRITE32(PSCAN_BASE, 0x05C, (IO_READ32(PSCAN_BASE, 0x05C)&0xDFFFFFFFF)|0x20000000);
                vDrvDISetCRCClr(1);
                // Trigger PSCAN CRC
                //IO_WRITE32(PSCAN_BASE, 0x05C, (IO_READ32(PSCAN_BASE, 0x05C)&0xEFFFFFFFF)|0x10000000);
                vDrvDISetCRCTrigger(1);
                LOG(2, "Trigger PSCAN CRC\n");
#endif

            }
#endif

            if (prFrcPrm->ucReady == 0)
            {
                if ((prFrcPrm->ucFbId != FBM_FB_ID_UNKNOWN) &&
                        (prFrcPrm->ucNotSupport == 0))
                {
                    UCHAR ucAvSyncMode;
                    UCHAR ucStcSrc;
#ifdef TIME_MEASUREMENT
                    TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME_DRV, "B2R ready to play 1st Frame");
                    TMS_DIFF_EX(TMS_FLAG_BOOT, TMS_COOL_BOOT_TIME, "B2R ready to play 1st Frame");

#endif
                    _VDP_HalForceBg(ucB2rId, 0);
                    prFrcPrm->ucReady = 1;
#ifdef CC_MAPLE_CUST_DRV
                    _B2R_Cust_info_cfg(_prVdpConf[ucVdpId],prFrcPrm);
#endif
                    FBM_GetSyncStc(prFrcPrm->ucFbgId, &ucAvSyncMode, &ucStcSrc);

#ifndef CC_B2R_WAIT_NPTV_STABLE
                    FBM_SetFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_DISPLAYING);
#else
                    FBM_GetPlayMode(prFrcPrm->ucFbgId, &_au1PlayMode[ucB2rId]);
                    if (( FBM_FBG_DTV_MODE == _au1PlayMode[ucB2rId]) || 
                        (( FBM_FBG_MM_MODE == _au1PlayMode[ucB2rId]) && (ucAvSyncMode == VID_SYNC_MODE_NONE)))
                    {
                        FBM_SetFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_DISPLAYING);
                    }
                    if (( FBM_FBG_MM_MODE == _au1PlayMode[ucB2rId]) && (ucAvSyncMode == VID_SYNC_MODE_NONE))
                    {
                        FBM_ClrFrameBufferGlobalFlag(prFrcPrm->ucFbgId, FBM_FLAG_FB_NO_TIMEOUT);
                    }
#endif
                    
                    _VDP_VsyncNotify(VDP_MSG_UNMUTE_CB, (UINT32)ucVdpId, 1, 0);
                    _VDP_VsyncNotify(VDP_MSG_PLAY_DONE_CB, (UINT32)ucVdpId, 0, 0);

                    if (_arVdpDtvFreeze[ucB2rId].u4OnOff != 0)
                    {
                        FBM_SetFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_FREEZE);
                        FBM_ReleaseDispQ(prFrcPrm->ucFbgId);
                    }

#ifdef TIME_MEASURE
                    if (prFrcPrm->prFbCounter)
                    {
                        HAL_TIME_T rResult;

                        HAL_GetDeltaTime(&rResult, &(_prDbase->rDmx.rEnableTime), &(prFrcPrm->prFbCounter->rStartTime));

                        LOG(1, "Video Time Measure (%d.%06d)\n", rResult.u4Seconds, rResult.u4Micros);
                    }
#endif


                    // notify SCPOS
                    if (!(prFrcPrm->fgSeqChgSpeedup))
                    {
                        _VDP_VsyncNotify(VDP_MSG_NOTIFY, (UINT32)ucVdpId, VDP_B2R_START_PLAY, 0);
                        _fgModeChangeSent = TRUE;
                    }
#ifdef VDP_MUTE_EARLY_DISPLAY
                    if ((_prVdpConf[ucVdpId]->ucEarlyDisp == 0) && (prFrcPrm->fgSeqChgSpeedup))
#endif
                    {
#ifdef TIME_MEASUREMENT
                        TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME_DRV, "B2R UnMute +Speedup");
#endif
                        _vDrvVideoSetMute(MUTE_MODULE_DTV, ucVdpId, 0, FALSE);
                        LOG(3, "DTV Unmute Early Display + Seq Chg Speedup VDP(%d)\n", ucVdpId);
#ifdef CC_SUPPORT_TVE
                        if ((_prVdpConf[VDP_1]->ucB2rId == B2R_1) &&
                                (_prVdpConf[VDP_2]->ucB2rId == B2R_1)&&
                                (_prVdpConf[VDP_1]->ucEnable == 1) &&
                                (_prVdpConf[VDP_2]->ucEnable == 1) )
                        {
                            _vDrvVideoSetMute(MUTE_MODULE_DTV, (1 - ucVdpId), 0, FALSE);
                            LOG(3, "DTV Unmute Early Display + Seq Chg Speedup VDP(%d)\n", 1-ucVdpId);
                        }
#endif
                    }
                }
                else
                {
                    _VDP_HalForceBg(ucB2rId, 1);
                    FBM_ClrFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_DISPLAYING);
                }
            }
        }
        break;

    default:
        break;
    }

#ifdef __MODEL_slt__
//HAL_GetTime(&_rTime);
//Printf("3-%06u\n", _rTime.u4Micros);
    b2rState = 0xff;
#endif

    return _u4ReturnValue;
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_FrcProcEarly.
 */
//-----------------------------------------------------------------------------
void _VDP_FrcProcEarly(UCHAR ucVdpId, UCHAR ucBottom)
{
    B2R_PRM_T* prFrcPrm;

    UNUSED(ucBottom);

    if (ucVdpId >= VDP_NS)
    {
        ASSERT(ucVdpId < VDP_NS);
        return;
    }
    // Frame Rate Control Process is NOT required if NOT enable
    if (!(_prVdpConf[ucVdpId]->ucEnable))
    {
        return;
    }

    prFrcPrm = &_arVdpPrm[VDP2B2RID(ucVdpId)];

    // FBG pending release
    if (FBM_ChkFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_RELEASE_FBG))
    {
        if (prFrcPrm->ucReady != 0)
        {
            LOG(1, "FBM_FLAG_RELEASE_FBG\n");

            if (prFrcPrm->ucReleaseFbId != FBM_FB_ID_UNKNOWN)
            {
                _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucFbgId, prFrcPrm->ucReleaseFbId, FBM_FB_STATUS_EMPTY);
                prFrcPrm->ucReleaseFbId = FBM_FB_ID_UNKNOWN;
            }

            if (prFrcPrm->ucFbId != FBM_FB_ID_UNKNOWN)
            {
                _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_FB_STATUS_EMPTY);
                prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
            }

            prFrcPrm->ucReleased = 1;
            prFrcPrm->ucReady = 0;

            _VDP_HalForceBg(VDP2B2RID(ucVdpId), 1);

            _VDP_VsyncNotify(VDP_MSG_NOTIFY, (UINT32)ucVdpId, VDP_B2R_NO_SIGNAL, 0);
        }

        return;
    }

    // frame rate config core
    switch (_arVdpPrm[VDP2B2RID(ucVdpId)].ucMode)
    {
    case VDP_MODE_DEINT:        // Deinterlace Mode, Temp
    case VDP_MODE_NORMAL:        // Normal Mode
    case VDP_MODE_BG:            // Background Mode, do nothing
        if (FBM_ChkFrameBufferFlag(prFrcPrm->ucFbgId, FBM_FLAG_DEBLOCKING))
        {
            if (prFrcPrm->ucReleaseFbId != FBM_FB_ID_UNKNOWN)
            {
                if (prFrcPrm->ucReleased == 0)
                {
                    _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucFbgId, prFrcPrm->ucReleaseFbId, FBM_FB_STATUS_EMPTY);
                }
                prFrcPrm->ucReleased = 0;

                prFrcPrm->ucReleaseFbId = FBM_FB_ID_UNKNOWN;
            }
        }
        break;

    default:
        break;
    }
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** Brief of VDP_FrcResetPort.
 */
//-----------------------------------------------------------------------------
void VDP_FrcResetPort(UCHAR ucB2rId, UCHAR ucPort)
{
    B2R_PRM_T rVdpPrm;

    UNUSED(ucPort);

    if (ucB2rId >= B2R_NS)
    {
        return;
    }

    // backup those we don't want to reset
    // rVdpPrm = _arVdpPrm[ucB2rId];  //TBD
    x_memcpy(&rVdpPrm, &(_arVdpPrm[ucB2rId]), sizeof(B2R_PRM_T));

    _VdpFrmInit(ucB2rId, 0, FALSE);

    // restore
    _arVdpPrm[ucB2rId].ucFbgId = rVdpPrm.ucFbgId;
    _arVdpPrm[ucB2rId].ucOutFrameRate = rVdpPrm.ucOutFrameRate;
    _arVdpPrm[ucB2rId].u4OutStcPeriod = rVdpPrm.u4OutStcPeriod;
    _arVdpPrm[ucB2rId].ucProgressiveOut = rVdpPrm.ucProgressiveOut;

    _arVdpPrm[ucB2rId].prFbCounter = rVdpPrm.prFbCounter;

    _arVdpPrm[ucB2rId].u4CbPtsEn = rVdpPrm.u4CbPtsEn;
    _arVdpPrm[ucB2rId].u4CbPts = rVdpPrm.u4CbPts;
    _arVdpPrm[ucB2rId].u4CbArg = rVdpPrm.u4CbArg;

    _arVdpPrm[ucB2rId].u4CbSyncEn = rVdpPrm.u4CbSyncEn;
    _arVdpPrm[ucB2rId].u4CbSyncThrsd= rVdpPrm.u4CbSyncThrsd;
    _arVdpPrm[ucB2rId].u4CbSyncFrmCnt= rVdpPrm.u4CbSyncFrmCnt;


    // clean FBM_FLAG_DISPLAYING at next VSYNC
    FBM_ClrFrameBufferFlag(rVdpPrm.ucFbgId, FBM_FLAG_DISPLAYING);
}

//-----------------------------------------------------------------------------
/** Brief of VDP_FrcSetFreeze.
 */
//-----------------------------------------------------------------------------

void VDP_FrcSetFreeze(UCHAR ucB2rId, UINT32 u4OnOff)
{
    if (ucB2rId >= B2R_NS)
    {
        ASSERT(ucB2rId < B2R_NS);
        return;
    }

#ifdef CC_B2R_GFX_COPY_FIELD
    // mutex
    VERIFY (x_sema_lock(_rVdpGfxCopyField.hMutex, X_SEMA_OPTION_WAIT) == OSR_OK);
#endif

    if (u4OnOff != _arVdpDtvFreeze[ucB2rId].u4OnOff)
    {
        UCHAR ucFbgId;

        ucFbgId = _arVdpPrm[ucB2rId].ucFbgId;

        if (ucFbgId != FBM_FBG_ID_UNKNOWN)
        {
            if (u4OnOff != 0)
            {
                FBM_SetFrameBufferFlag(ucFbgId, FBM_FLAG_FREEZE);

//                FBM_ReleaseDispQ(ucFbgId);
            }
            else
            {
                FBM_ClrFrameBufferFlag(ucFbgId, FBM_FLAG_FREEZE);
            }
        }

        if (u4OnOff == 0)
        {
            _arVdpDtvFreeze[ucB2rId].u4Recovery = 1;
            _arVdpDtvFreeze[ucB2rId].u4UnFreeze= 1;
        }
        else
        {
            _arVdpDtvFreeze[ucB2rId].u4Recovery = 0;
            _arVdpDtvFreeze[ucB2rId].u4UnFreeze= 0;
        }

        _arVdpDtvFreeze[ucB2rId].u4CleanDispQ = 0;
        _arVdpDtvFreeze[ucB2rId].u4CopyField = 0;
        _arVdpDtvFreeze[ucB2rId].u4OnOff = u4OnOff;
    }

#ifdef CC_B2R_GFX_COPY_FIELD
    // mutex
    VERIFY (x_sema_unlock(_rVdpGfxCopyField.hMutex) == OSR_OK);
#endif
}

BOOL VDP_FrcGetUnMuteStatus(UCHAR ucB2rId)
{
    if (ucB2rId < B2R_NS)
    {
        if (_arVdpPrm[ucB2rId].ucReady != 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}

UCHAR VDP_FrcGetLockFrameBuffer(UINT8 u1B2rId)
{
    UCHAR ucLockFbId;

    _VDP_VsyncMutexLock();
    if (u1B2rId < B2R_NS)
    {
        ucLockFbId = _arVdpPrm[u1B2rId].ucFbId != FBM_FB_ID_UNKNOWN ?
                     _arVdpPrm[u1B2rId].ucFbId : _arVdpPrm[u1B2rId].ucPendingFbId;
    }
    else
    {
        ucLockFbId = FBM_FB_ID_UNKNOWN;
    }
    _VDP_VsyncMutexUnlock();
    return ucLockFbId;
}

void _VdpHandleResolutionChange(B2R_PRM_T * prFrcPrm)
{
    FBM_SEQ_HDR_T* prSeqHdr = FBM_GetFrameBufferSeqHdr(prFrcPrm->ucFbgId);
    if (prSeqHdr && (_u4LastWidth != prSeqHdr->u2OrgWidth || _u4LastHeight != prSeqHdr->u2OrgHeight))
    {
        _u4LastWidth = prSeqHdr->u2OrgWidth;
        _u4LastHeight = prSeqHdr->u2OrgHeight;
        if (_fgModeChangeSent)
        {
            UCHAR ucVdpId = VDP_GetVdpId(prFrcPrm->ucB2rId);
            _VDP_VsyncNotify(VDP_MSG_RES_CHG_CB, ucVdpId, 0, 0);
        }
    }
}

void _VdpHandleBitRateChange(B2R_PRM_T * prFrcPrm)
{
    FBM_PIC_HDR_T* prHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, prFrcPrm->ucFbId);
    if (prHdr && prHdr->fgBandwidthChanged)
    {
        UCHAR ucVdpId = VDP_GetVdpId(prFrcPrm->ucB2rId);
        _VDP_VsyncNotify(VDP_MSG_BIT_RATE_CHG_CB, ucVdpId, 0, 0);
        prHdr->fgBandwidthChanged = FALSE;
    }
}

VOID _VDP_SetCurrentTick(UCHAR ucB2rId, UINT32 u4Tick)
{
    FBM_PIC_HDR_T* prLockPicHdr;
    UCHAR ucFbgId;
    UCHAR ucLockFbId;

    ucFbgId = _arVdpPrm[ucB2rId].ucFbgId;
    ucLockFbId = _arVdpPrm[ucB2rId].ucFbId;

    if ((ucLockFbId != FBM_FB_ID_UNKNOWN) && (ucFbgId  != FBM_FBG_ID_UNKNOWN))
    {
        prLockPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucLockFbId);

        if (prLockPicHdr != NULL)
        {
            prLockPicHdr->u4TickNum = u4Tick;
        }
    }
}

UINT32 _VDP_GetCurrentPts(UINT8 u1B2rId)
{
    FBM_PIC_HDR_T* prLockPicHdr;
    UCHAR ucFbgId;
    UCHAR ucLockFbId;

    if (u1B2rId < B2R_NS)
    {
        ucFbgId = _arVdpPrm[u1B2rId].ucFbgId;
        ucLockFbId = _arVdpPrm[u1B2rId].ucFbId != FBM_FB_ID_UNKNOWN ?
                     _arVdpPrm[u1B2rId].ucFbId : _arVdpPrm[u1B2rId].ucPendingFbId;

        if ((ucLockFbId != FBM_FB_ID_UNKNOWN) && (ucFbgId  != FBM_FBG_ID_UNKNOWN))
        {
            prLockPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucLockFbId);

            if (prLockPicHdr != NULL)
            {
                return prLockPicHdr->u4PTS;
            }
            else
            {
                LOG(7,"B2R _VDP_GetCurrentPts prLockPicHdr is NULL!\n");
                return 0;
            }
        }
        else
        {
            LOG(7,"B2R _VDP_GetCurrentPts ucLockFbId %d , ucFbgId :%d!\n");
        }
    }
    else
    {
        LOG(7,"B2R _VDP_GetCurrentPts u1B2rId : %d!\n");
    }
    return 0;
}

UINT32 _VDP_GetLastPts(UINT8 u1B2rId)
{
    return _arVdpPrm[u1B2rId].u4LastPts;
}

UINT32 _VDP_GetDispingPts(UINT8 u1B2rId)
{
    return _arVdpPrm[u1B2rId].u4Pts;
}

UCHAR _VDP_GetOutFrameRate(UINT8 u1B2rId)
{
    return _arVdpPrm[u1B2rId].ucOutFrameRate;
}

UCHAR _VDP_GetFbg(UINT8 u1B2rId)
{
    if ( u1B2rId >= B2R_NS )
    {
        return FBM_FBG_ID_UNKNOWN;
    }

    return _arVdpPrm[u1B2rId].ucFbgId;
}

UINT32 _VDP_GetFilePosition(UINT8 u1B2rId,
                            INT32 *pi4TempRef, UINT16 *pu2DecodingOrder,
                            UINT64 *pu8Offset, UINT64 *pu8OffsetI)
{
    FBM_PIC_HDR_T* prLockPicHdr;
    UCHAR ucFbgId;
    UCHAR ucLockFbId;

    ucFbgId = _arVdpPrm[u1B2rId].ucFbgId;
    ucLockFbId = _arVdpPrm[u1B2rId].ucFbId != FBM_FB_ID_UNKNOWN ?
                 _arVdpPrm[u1B2rId].ucFbId : _arVdpPrm[u1B2rId].ucPendingFbId;

    if ((ucLockFbId != FBM_FB_ID_UNKNOWN) && (ucFbgId  != FBM_FBG_ID_UNKNOWN))
    {
        prLockPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucLockFbId);
        if (prLockPicHdr != NULL)
        {
            *pi4TempRef = prLockPicHdr->i4TemporalRef;
            *pu2DecodingOrder = prLockPicHdr->u2DecodingOrder;
            *pu8Offset = prLockPicHdr->u8Offset;
            *pu8OffsetI = prLockPicHdr->u8OffsetI;
            return VDP_SET_OK;
        }
        else
        {
            *pi4TempRef = 0;
            *pu2DecodingOrder = 0;
            *pu8Offset = 0;
            *pu8OffsetI = 0;
        }
    }
    else
    {
        *pi4TempRef = 0;
        *pu2DecodingOrder = 0;
        *pu8Offset = 0;
        *pu8OffsetI = 0;
    }
    return VDP_SET_ERROR;
}


UINT32 _VDP_GetPosInfo(UINT8 u1B2rId, VDP_POS_INTO_T *prPosInfo)
{
    FBM_PIC_HDR_T* prLockPicHdr;
    UCHAR ucFbgId;
    UCHAR ucLockFbId;


    if (!prPosInfo)
    {
        return VDP_SET_ERROR;
    }
    if (u1B2rId >= B2R_NS)
    {
        return VDP_SET_ERROR;
    }

    x_memset(prPosInfo, 0, sizeof(VDP_POS_INTO_T));

    ucFbgId = _arVdpPrm[u1B2rId].ucFbgId;
    ucLockFbId = _arVdpPrm[u1B2rId].ucFbId != FBM_FB_ID_UNKNOWN ?
                 _arVdpPrm[u1B2rId].ucFbId : _arVdpPrm[u1B2rId].ucPendingFbId;

    if ((ucLockFbId != FBM_FB_ID_UNKNOWN) && (ucFbgId  != FBM_FBG_ID_UNKNOWN))
    {
        prLockPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucLockFbId);
        if (prLockPicHdr != NULL)
        {
            prPosInfo->i4TemporalRef = prLockPicHdr->i4TemporalRef;
            prPosInfo->u2DecodingOrder = prLockPicHdr->u2DecodingOrder;
            prPosInfo->u8Stc    = (UINT64)_VDP_FrcGetStc(ucFbgId);
            prPosInfo->u8Offset = prLockPicHdr->u8Offset;
            prPosInfo->u8OffsetI = prLockPicHdr->u8OffsetI;
            prPosInfo->u8OffsetDisp = prLockPicHdr->u8OffsetDisp;
            prPosInfo->u8Pts = (UINT64)prLockPicHdr->u4PTS;
            prPosInfo->u8PtsI = prLockPicHdr->rExtra.u8PTSI;
            prPosInfo->u8AudPts = prLockPicHdr->rExtra.u8AudPTS;
            prPosInfo->u8AudOffset = prLockPicHdr->rExtra.u8AudOffset;
            prPosInfo->u4Timestap = prLockPicHdr->u4Timestap;

            //DTV00316878 for avi file same PTS value when AB repeat in trick mode
            if (((_eSpeed[u1B2rId] >= STC_SPEED_TYPE_FORWARD_4X) &&
                    (_eSpeed[u1B2rId] <= STC_SPEED_TYPE_FORWARD_1024X))||
                    ((_eSpeed[u1B2rId] >= STC_SPEED_TYPE_REWIND_1X) &&
                     (_eSpeed[u1B2rId] <= STC_SPEED_TYPE_REWIND_1024X)))
            {
                prPosInfo->u8Stc  = prPosInfo->u8Pts;
            }
            LOG(0,"prPosInfo->u8Stc = %llu\n", prPosInfo->u8Stc);
            return VDP_SET_OK;
        }
    }
    return VDP_SET_ERROR;
}

UINT32 _VDP_SetABInfo(UINT8 u1B2rId, VDP_AB_INTO_T *prAB)
{
    if (!prAB)
    {
        return VDP_SET_ERROR;
    }
    if (u1B2rId >= B2R_NS)
    {
        return VDP_SET_ERROR;
    }
    x_memcpy(&_arVdpPrm[u1B2rId].rAB, prAB, sizeof(VDP_AB_INTO_T));
    _VDPSetABRepeat(u1B2rId, prAB->fgValid);
    return VDP_SET_OK;
}

UCHAR _VDP_GetPendingFB(UCHAR ucB2rId, BOOL fgSubFbg)
{
    if (ucB2rId < B2R_NS)
    {
        if (!fgSubFbg)
        {
            return _arVdpPrm[ucB2rId].ucPendingFbId;
        }
        else
        {
#ifdef CC_3D_MM_DS_SUPPORT
            return _arVdpPrm[ucB2rId].ucSubPendingFbId;
#else
            return FBM_FB_ID_UNKNOWN;
#endif
        }
    }
    else
    {
        return FBM_FB_ID_UNKNOWN;
    }
}

void _VDP_CleanPendingFB(UCHAR ucB2rId, BOOL fgSubFbg)
{
    if (ucB2rId < B2R_NS)
    {
        _arVdpPrm[ucB2rId].fgReleaseDispQ = TRUE;
        if (!fgSubFbg)
        {
            _arVdpPrm[ucB2rId].ucPendingFbId = FBM_FB_ID_UNKNOWN;
        }
#ifdef CC_3D_MM_DS_SUPPORT
        else
        {
            _arVdpPrm[ucB2rId].ucSubPendingFbId = FBM_FB_ID_UNKNOWN;
        }
#endif
    }
}

void _VDP_SetReleaseDispQ(UCHAR ucB2rId, BOOL fgSubFbg)
{
    UNUSED(fgSubFbg);
    if (ucB2rId < B2R_NS)
    {
        _arVdpPrm[ucB2rId].fgReleaseDispQ = TRUE;
    }
}
#if 0
void _VDP_CopyFrcPrm(UCHAR ucB2rId)
{
    PARAM_VDP_FB_COUNTER_T* pTempFbCounter;
    UCHAR ucTempVdpId;
    UCHAR ucTempReady;

    if (ucB2rId >= B2R_NS)
    {
        return;
    }

    /* for correct dbs print message */
    pTempFbCounter = _arVdpPrm[1 - ucB2rId].prFbCounter;

    /* Should NOT be copied */
    ucTempVdpId = _arVdpPrm[1 - ucB2rId].ucVdpId;

    /* DTV SCART out, mute/unmute mechanism */
    ucTempReady = _arVdpPrm[1 - ucB2rId].ucReady;

    if (_rFrcCopy.u4TriggerCopy == 1)
    {
        x_memcpy( &_arVdpPrm[1 - ucB2rId],  &_rFrcCopy, sizeof(B2R_PRM_T));
        _rFrcCopy.u4TriggerCopy = 0;
    }
    else
    {
        x_memcpy( &_arVdpPrm[1 - ucB2rId],  &_arVdpPrm[ucB2rId], sizeof(B2R_PRM_T));
    }

    /* for correct dbs print message */
    _arVdpPrm[1 - ucB2rId].prFbCounter = pTempFbCounter;

    /* Should NOT be copied */
    _arVdpPrm[1 - ucB2rId].ucVdpId = ucTempVdpId;

    /* Should be cleared */
    _arVdpPrm[ucB2rId].ucPendingNewFb = 0; /* Check _VdpFrcFrameBufferReady */

    /* DTV SCART out, mute/unmute mechanism */
    if ((ucTempReady == 0) && (_arVdpPrm[1 - ucB2rId].ucReady == 1))
    {
        /* ready from 0 to 1 */
        _vDrvVideoSetMute(MUTE_MODULE_DTV, _arVdpPrm[1 - ucB2rId].ucVdpId, 0, FALSE);
    }
}*/
#endif 
/* VDP trigger mode + audio master to do trigger mode */
void _VDPTriggerAudReceive(UINT8 u1B2rId, UINT32 u4Pts)
{
    if (u1B2rId < B2R_NS)
    {
        _arVdpTrigAud[u1B2rId].ucTriggerReceived = 1;
        _arVdpTrigAud[u1B2rId].u4TriggerPts = u4Pts;
        x_memset((void *)&_arVdpTrigAud[u1B2rId].rTrgAudCbInfo, 0, sizeof(VDP_TRIG_AUD_ARRIVE_CB_T));
    }
}

/** Disable&Enable VDP Trigger action */
BOOL _VDPTriggerAudReceiveEx(UINT8 u1B2rId, BOOL fgEnable, UINT32 u4Pts,
    VDP_TRIG_AUD_ARRIVE_CB_T *prCbInfo)
{
    if (u1B2rId < B2R_NS)
    {
        _arVdpTrigAud[u1B2rId].ucTriggerReceived = fgEnable?1:0;
        _arVdpTrigAud[u1B2rId].u4TriggerPts = u4Pts;

        if ((NULL!=prCbInfo) && (1==_arVdpTrigAud[u1B2rId].ucTriggerReceived))
        {
            _arVdpTrigAud[u1B2rId].rTrgAudCbInfo.pfnTrigAud = prCbInfo->pfnTrigAud;
            _arVdpTrigAud[u1B2rId].rTrgAudCbInfo.pvTag = prCbInfo->pvTag;
        }
        else
        {
            x_memset((void *)&_arVdpTrigAud[u1B2rId].rTrgAudCbInfo, 0, sizeof(VDP_TRIG_AUD_ARRIVE_CB_T));
        }
    }
    else
    {
        return FALSE;
    }
    
    return TRUE;
}

/* whether Update STC when VDP start output or not */
BOOL _VDPSetPendingForStc(UINT8 u1B2rId, BOOL fgPending)
{
    if (u1B2rId < B2R_NS)
    {
        _arPendingForSTC[u1B2rId] = fgPending;
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

static void _VdpTriggerAudOn(UINT8 u1B2rId, UINT32 u4CurPts)
{
    if (u1B2rId >= B2R_NS)
    {
        ASSERT(0);
        return;
    }

    if (_arVdpTrigAud[u1B2rId].ucTriggerReceived == 1)
    {
        if (u4CurPts >= _arVdpTrigAud[u1B2rId].u4TriggerPts)
        {
            LOG(1, "VDP Trigger Aud: CurPTS(%X), SetPTS(%X)\n", u4CurPts, _arVdpTrigAud[u1B2rId].u4TriggerPts);

            // _VDP_EventNofify(VDP_B2r2Vdp(u1B2rId), (UINT32)VDP_CB_FUNC_PLAY_DONE_IND, 0, 0);

            /* AUD trigger function */
            if (NULL != _arVdpTrigAud[u1B2rId].rTrgAudCbInfo.pfnTrigAud)
            {
                ((PFN_TRIG_AUD_ARRIVE_CB)_arVdpTrigAud[u1B2rId].rTrgAudCbInfo.pfnTrigAud)(_arVdpTrigAud[u1B2rId].rTrgAudCbInfo.pvTag);
            }
            else
            {
#ifdef CC_USE_DDI
                AUD_MMAoutEnable(AUD_DEC_AUX, TRUE);
#endif
#ifdef ENABLE_MULTIMEDIA
#ifdef CC_53XX_SWDMX_V2
                if (u1B2rId < B2R_NS)
                {
                    UNUSED(SWDMX_SetInfo(_arVdpPrm[u1B2rId].u4AttachedSrcId, eSWDMX_SET_AOUT_ENABLE, 0, 0, 0));
                }
                else
                {
                    ASSERT(u1B2rId < B2R_NS);
                }
#else
                UNUSED(SWDMX_SetInfo(eSWDMX_SET_AOUT_ENABLE, 0, 0, 0));
#endif
#else
                // actually this is no meaning.
                AUD_MMAoutEnable(0, TRUE);
#endif
            }
            x_memset((void *)&_arVdpTrigAud[u1B2rId].rTrgAudCbInfo, 0, sizeof(VDP_TRIG_AUD_ARRIVE_CB_T));

            _arVdpTrigAud[u1B2rId].ucTriggerReceived = 0;
        }
    }
}

static void _VdpSetFrameBufferStatus(B2R_PRM_T* prFrcPrm, UCHAR ucFbgId, UCHAR ucFbId, UCHAR ucFbStatus)
{
    if (prFrcPrm->fgB2RNoLock && (ucFbStatus == FBM_FB_STATUS_LOCK))
    {
        //Not do lock
    }
    else
    {
        FBM_SetFrameBufferStatus(ucFbgId, ucFbId, ucFbStatus);
    }
}

UINT32 _VDP_GetDispBufLocked(UCHAR ucB2rId, UINT32 u4BufAddr, BOOL *pfgLocked)
{
    return 1;
}

/* VDP Seek done notification */
void _VDPSetSeek(UCHAR ucB2rId, UCHAR ucSet, UINT32 u4Pts)
{
    if (ucB2rId < B2R_NS)
    {
        arVdpSeek[ucB2rId].u4SeekPts = u4Pts;
        arVdpSeek[ucB2rId].ucNotifyTrigger = 1;
        arVdpSeek[ucB2rId].ucSeekMode = 1;
    }
    else
    {
        ASSERT(0);
        return;
    }
}

/* VDP AB Repeat done notification */
void _VDPSetABRepeat(UCHAR ucB2rId, UCHAR ucSet)
{
    if (ucB2rId < B2R_NS)
    {
        if ((ucSet) && (_arVdpPrm[ucB2rId].rAB.fgValid))
        {
            arVdpSeek[ucB2rId].ucABNotifyTrigger = 1;
            arVdpSeek[ucB2rId].fgABReach = FALSE;
            LOG(3, "ucABNotifyTrigger ENABLE\n");
        }
        else
        {
            arVdpSeek[ucB2rId].ucABNotifyTrigger = 0;
            arVdpSeek[ucB2rId].fgABReach = FALSE;
        }
    }
    else
    {
        ASSERT(0);
        return;
    }
}

static void _VDPGetSeek(UCHAR ucB2rId, UCHAR* pucSeekMode, UINT32* pu4Pts, UCHAR* pucNotifyTrigger)
{
    *pu4Pts = arVdpSeek[ucB2rId].u4SeekPts;
    *pucNotifyTrigger = arVdpSeek[ucB2rId].ucNotifyTrigger;
    *pucSeekMode = arVdpSeek[ucB2rId].ucSeekMode;
}

VOID _VDP_FrcStcSpeed(UINT8 u1B2rId,STC_SPEED_TYPE_T eSpeed)
{
    _arVdpPrm[u1B2rId].u4B2rSkipFrameNo = 0;
    if (u1B2rId < B2R_NS)
    {
        _eSpeed[u1B2rId] = eSpeed;
    }
    else
    {
        ASSERT(0);
    }
}

STC_SPEED_TYPE_T _VDP_GetFrcStcSpeed(UINT8 u1B2rId)
{
    if (u1B2rId < B2R_NS)
    {
        return _eSpeed[u1B2rId];
    }
    else
    {
        ASSERT(0);
        return STC_SPEED_TYPE_FORWARD_1X;
    }
}

#ifdef VDP_B2R_PSCAN_CRC_FLOW
/* B2R CRC check on PTS value */
void _VDPSetChkPTS(UINT32 u4Pts)
{
    _u4SetPts = u4Pts;

    /* Reset status */
    _rVdpB2rCrc.u4DramChkSum = 0;
    _rVdpB2rCrc.u4PixelChkSum = 0;
    _rVdpB2rCrc.u4PSCANCrc = 0;
    _rVdpB2rCrc.u4PSCANSrcCrc = 0;
    LOG(3, "Set PTS (%d)\n", _u4SetPts);
}

void _VDP_SetEncTrigger(BOOL fgEnable)
{
    _fgTriggerEnc = fgEnable;
    UNUSED(_fgTriggerEnc);
}

#endif

//-----------------------------------------------------------------------------
/** Brief of _VdpPreLookNextDisp.
 */
//-----------------------------------------------------------------------------
static UCHAR _VdpPreLookNextDisp(B2R_PRM_T* prFrcPrm)
{
    return FBM_PreLookFrameBufferFromDispQ(prFrcPrm->ucFbgId);
}

B2R_PRM_T* _VDP_GetVdpPrm(UINT8 u1B2rId)
{
    if (u1B2rId < B2R_NS)
    {
        return &(_arVdpPrm[u1B2rId]);
    }
    else
    {
        return NULL;
    }
}

VOID _VDP_SetPauseMM(UINT8 u1B2rId,BOOL fgPauseMM)
{
    if (u1B2rId < B2R_NS)
    {
        _fgPauseMM[u1B2rId] = fgPauseMM;
    }
    else
    {
        ASSERT(0);
    }
}

VOID _VDP_SetB2rNoLock(UINT8 u1B2rId, BOOL fgNoLock)
{
    if (u1B2rId < B2R_NS)
    {
        _arVdpPrm[u1B2rId].fgB2RNoLock = fgNoLock;
    }
}

VOID _VDP_Set3DInfo(UINT8 u1B2rId,UINT8 u13DType,BOOL fg3DEnable)
{
    _arVdpPrm[u1B2rId].fgB2R3DEnable = fg3DEnable;
    _arVdpPrm[u1B2rId].u1B2R3DType = u13DType;
}

BOOL _VDP_IsStartToPlay(UCHAR u1B2rId)
{
#ifdef CC_B2R_WAIT_NPTV_STABLE
    if (_arVdpPrm[u1B2rId].ucFbgId != 0xFF)
    {
        B2R_PRM_T* prFrcPrm = &_arVdpPrm[u1B2rId];//TBD
        FBM_GetPlayMode(_arVdpPrm[u1B2rId].ucFbgId,&_au1PlayMode[u1B2rId]);

        if (prFrcPrm != NULL)
        {
            UCHAR ucVdpId = VDP_GetVdpId(u1B2rId);
            if (_au1PlayMode[u1B2rId] == FBM_FBG_MM_MODE && !_afgNPTVStable[u1B2rId] && !prFrcPrm->fgB2R3DEnable && (prFrcPrm->ucReady ==1) && _u4DrvVideoGetMute(ucVdpId))
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
        else
        {
            return TRUE;
        }
    }
    else
    {
        return TRUE;
    }
#else
    return TRUE;
#endif
}
VOID _VDP_SetTimeShiftSpeed(UINT8 u1B2rId,UINT32 u4SpeedValue)
{

    if (u1B2rId < B2R_NS)
    {
        _i4TimeShiftSpeed[u1B2rId] = u4SpeedValue;
    }
    else
    {
        LOG(3,"warning u1B2rId >= B2R_NS\n");
        _i4TimeShiftSpeed[0] = u4SpeedValue;
    }
    UNUSED(_i4TimeShiftSpeed);// make lint happy
}

VOID _VDP_SetTimeShiftMode(UINT8 u1B2rId,BOOL fgEnable)
{
    if (u1B2rId < B2R_NS)
    {
        _fgTimeShift[u1B2rId] = fgEnable;
    }
    else
    {
        LOG(3,"warning u1B2rId >= B2R_NS\n");
        _i4TimeShiftSpeed[0] = fgEnable;
    }
    UNUSED(_fgTimeShift);
    UNUSED(_i4TimeShiftSpeed);
}
BOOL VDP_GetTimeShfitStartPlay(UCHAR ucB2rId)
{
    UNUSED(ucB2rId);
    if (_fgTimeShift[0])
    {
        return _fgTimeShfitThumbStartPlay;
    }
    else
    {
        return TRUE;
    }
}

UINT32 _VDP_TrickModeSkipFrameNo(UCHAR ucB2rId)
{
    return _arVdpPrm[ucB2rId].u4B2rSkipFrameNo;
}

INT32 _VDP_GetPtsStcDelta(UCHAR ucB2rId)
{
    if (ucB2rId >= B2R_NS)
    {
        return 0;
    }

    return _arVdpPrm[ucB2rId].i4PtsDrift;
}

VOID _VDP_SetRepeatFrame(UCHAR ucVdpId)
{
    if (ucVdpId >= VDP_NS)
    {
        return;
    }

    // Because of wrong frame repeat problem:
    // 1. Do not apply for timeshift.
    // 2. Do not enable during early display.
    if ((_fgTimeShift[0] == FALSE) &&
            ((_prVdpConf[ucVdpId] != NULL) && (_prVdpConf[ucVdpId]->ucEarlyDisp != 1)))
    {
        _fgRepeatFrame = TRUE;
        LOG(9, "_VDP_SetRepeatFrame !! %d\n", _fgRepeatFrame);
    }
}

VOID _VDP_SetDropFrame(UCHAR ucVdpId)
{
    if (ucVdpId >= VDP_NS)
    {
        return;
    }

    // Because of wrong frame repeat problem:
    // 1. Do not apply for timeshift.
    // 2. Do not enable during early display.
    if ((_fgTimeShift[0] == FALSE) &&
            ((_prVdpConf[ucVdpId] != NULL) && (_prVdpConf[ucVdpId]->ucEarlyDisp != 1)))
    {
        _fgDropFrame = TRUE;
        LOG(9, "_VDP_SetDropFrame !! %d\n", _fgDropFrame);
    }
}

VOID _VDP_SetEnableLog(UCHAR ucVdpId,BOOL fgEnableGstLog, BOOL fgEnableVideoLog)
{
    UNUSED(ucVdpId);
    UNUSED(fgEnableGstLog);
    UNUSED(fgEnableVideoLog);

#ifdef CC_B2R_ENABLE_CHG_FRAME_MSG
    _fgEnableGstLog = fgEnableGstLog;
#endif
    _fgEnableVideoLog = fgEnableVideoLog;
}

BOOL _VDP_GetEnableGstLog(VOID)
{
#ifdef CC_B2R_ENABLE_CHG_FRAME_MSG
    return _fgEnableGstLog;
#else
    return FALSE;
#endif
}
BOOL _VdpIsFastForward(UCHAR ucB2rId)
{
    BOOL fgFastForward;
    if (ucB2rId < B2R_NS)
    {
        if (_eSpeed[ucB2rId] <= STC_SPEED_TYPE_FORWARD_1_DOT_5X)
        {
            fgFastForward = TRUE;
        }
        else
        {
            fgFastForward = FALSE;
        }
    }
    else
    {
        fgFastForward = TRUE;
    }
    return fgFastForward;
}

BOOL _VdpChangeFrameDirectForOMX(B2R_PRM_T* prFrcPrm, VDP_B2R_CHG_FRAME_MSG_T* prMsg)
{
    if (prMsg->u4DispMode == VDP_B2R_CHG_FRAME_DIRECT)
    {
        ASSERT(prMsg->ucFbgId == prFrcPrm->ucFbgId);
        prFrcPrm->ucFbId = prMsg->ucFbId;
        LOG_REL(_fgEnableVideoLog, 2, 7, "OMX DIRECT: FbgId(%d-%d)\n", prMsg->ucFbgId, prMsg->ucFbId);
        return TRUE;
    }
    return FALSE;
}

UCHAR _VdpFreeFrameBeforeEosHandler(B2R_PRM_T * prFrcPrm, BOOL fgMain, BOOL fgOMX, BOOL* fgEos)
{
    UCHAR ucFbId = FBM_FB_ID_UNKNOWN;
    if (fgMain)
    {
        if (fgOMX)
        {
            ucFbId = prFrcPrm->ucFbId;
        }
        else
        {
            ucFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
        }
        *fgEos = FALSE;
        if (ucFbId == FBM_FB_ID_UNKNOWN)
        {
            prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
        }
        else if (FBM_ChkFrameBufferPicFlag(prFrcPrm->ucFbgId, ucFbId, FBM_MM_EOS_FLAG))//eos frame, empty it in later eos handler
        {
            prFrcPrm->ucFbId = ucFbId;
            *fgEos = TRUE;
            ucFbId = FBM_FB_ID_UNKNOWN;
        }
        else
        {
            _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucFbgId, ucFbId, FBM_FB_STATUS_LOCK);
            _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucFbgId, ucFbId, FBM_FB_STATUS_EMPTY);
            prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
        }
    }
#ifdef CC_3D_MM_DS_SUPPORT
    else
    {
        ucFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucSubFbgId);
        *fgEos = FALSE;
        if (ucFbId == FBM_FB_ID_UNKNOWN)
        {
            prFrcPrm->ucSubFbId = FBM_FB_ID_UNKNOWN;
        }
        if (FBM_ChkFrameBufferPicFlag(prFrcPrm->ucSubFbgId, ucFbId, FBM_MM_EOS_FLAG))//eos frame, empty it in later eos handler
        {
            prFrcPrm->ucSubFbId = ucFbId;
            *fgEos = TRUE;
            ucFbId = FBM_FB_ID_UNKNOWN;
        }
        else
        {
            _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucSubFbgId, ucFbId, FBM_FB_STATUS_LOCK);
            _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucSubFbgId, ucFbId, FBM_FB_STATUS_EMPTY);
            prFrcPrm->ucSubFbId = FBM_FB_ID_UNKNOWN;
        }
    }
#endif
    return ucFbId;
}

BOOL _VdpHandleEos(B2R_PRM_T* prFrcPrm)
{
    BOOL fgEos = FALSE;
    UCHAR ucFbgId = FBM_FB_ID_UNKNOWN;
    if (FBM_ChkFrameBufferPicFlag(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_MM_EOS_FLAG))
    {
        fgEos = TRUE;
        ucFbgId = prFrcPrm->ucFbgId;
        FBM_ClrFrameBufferPicFlag(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_MM_EOS_FLAG);
        _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_FB_STATUS_EMPTY);
        prFrcPrm->ucFbId = prFrcPrm->ucReleaseFbId;
        prFrcPrm->ucReleaseFbId = FBM_FB_ID_UNKNOWN;
    }
#ifdef CC_3D_MM_DS_SUPPORT
    if (FBM_ChkFrameBufferPicFlag(prFrcPrm->ucSubFbgId, prFrcPrm->ucSubFbId, FBM_MM_EOS_FLAG))
    {
        fgEos = TRUE;
        ucFbgId = prFrcPrm->ucSubFbgId;
        FBM_ClrFrameBufferPicFlag(prFrcPrm->ucSubFbgId, prFrcPrm->ucSubFbId, FBM_MM_EOS_FLAG);
        _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucSubFbgId, prFrcPrm->ucSubFbId, FBM_FB_STATUS_EMPTY);
        prFrcPrm->ucSubFbId= prFrcPrm->ucSubReleaseFbId;
        prFrcPrm->ucSubReleaseFbId = FBM_FB_ID_UNKNOWN;
    }
#endif
    if (fgEos)
    {
        UCHAR ucSeekMode, ucSeekNotifyTrigger;
        UINT32 u4SeekPts;
        UCHAR ucVdpId = VDP_GetVdpId(prFrcPrm->ucB2rId);
        if ((VDP_HalGetStepForward(prFrcPrm->ucB2rId) == VDP_PLAY_STEP_FORWARD))
        {
            /* Play One Frame and then Pause */
            _VDP_SetPauseMM(prFrcPrm->ucB2rId, TRUE);
        }

        _VDPGetSeek(prFrcPrm->ucB2rId, &ucSeekMode, &u4SeekPts, &ucSeekNotifyTrigger);
        if ((ucSeekMode != 0 && ucSeekNotifyTrigger != 0) || _ucRemainTimeToSendSeekDone > 0)
        {
            /* Seek Finish notify trigger status clear */
            arVdpSeek[prFrcPrm->ucB2rId].ucNotifyTrigger = 0;
            _ucRemainTimeToSendSeekDone = 0;
            /* Seek Finish Notification */
            _VDP_VsyncNotify(VDP_MSG_MM_SEEK_FIN_CB, ucVdpId, u4SeekPts, VDP_SEEK_STEP_OK);
        }

        if (!prFrcPrm->rAB.fgValid)	 // if in AB repeat mode, no need to notify EOS
        {
            _VDP_VsyncNotify(VDP_MSG_MM_CB, (UINT32)ucVdpId, VDP_COND_EOS, FBM_GetDecoderSrcId(ucFbgId));
            LOG(3, "FBM_MM_EOS_FLAG fbg(%d) ES(%d) Notify\n",
                ucFbgId, FBM_GetDecoderSrcId(ucFbgId));
        }
        else
        {
        	  MMPlaybackSpeedTestAddTimeStamp(TIME_STAMP_VDP_ABREPEAT_DONE);
            _VDP_VsyncNotify(VDP_MSG_MM_CB, (UINT32)ucVdpId, VDP_COND_RANGE_AB_DONE, _VdpIsFastForward(prFrcPrm->ucB2rId));
            LOG(3, "VDP_COND_RANGE_AB_DONE Notify\n");
        }
    }
    return fgEos;
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_SetXYOffset.
 */
//-----------------------------------------------------------------------------
void _VDP_SetXYOffset(UCHAR ucB2rId)
{
    B2R_PRM_T * prFrcPrm = NULL;
    if (ucB2rId >= B2R_NS)
    {
        return ;
    }

    prFrcPrm = &_arVdpPrm[ucB2rId];
    ASSERT(prFrcPrm->rRenderRegion.u4X == ((prFrcPrm->rRenderRegion.u4X + 0xF) & ~0xF)); // H_Offset must be 16-pixer unit

    _VDP_HalSetXYShift(ucB2rId, prFrcPrm->rRenderRegion.u4X, prFrcPrm->rRenderRegion.u4Y);
}
//-----------------------------------------------------------------------------
/** Brief of _VDP_HalSetRenderRegion
 ** For JPEG raster mode to set the render region.
 ** Such JPEG decode a picture with 9160 * 9160, but only show (3000, 3000, 1920, 1080).
 ** Only change X,Y can be change the Y/C start Addr. 
 ** Change W/H will mode change with black, It is not implement yet.
 */
//-----------------------------------------------------------------------------
UINT32 _VDP_HalSetRenderRegion(UCHAR ucB2rId, VDP_REGION_T* prRenderRegion)
{
    //static VDP_REGION_T rRenderRegion = {0, 0, 0, 0};
    //static BOOL fgRenderRegionChg = FALSE;
    B2R_PRM_T* prFrcPrm;
    
    if ((ucB2rId >= B2R_NS) || !prRenderRegion)
    {
        return 0;
    }
    prFrcPrm = &_arVdpPrm[ucB2rId];
    
    if ((prFrcPrm->rRenderRegion.u4Width && (prRenderRegion->u4Width != prFrcPrm->rRenderRegion.u4Width)) ||
        (prFrcPrm->rRenderRegion.u4Height && (prRenderRegion->u4Height != prFrcPrm->rRenderRegion.u4Height)))
    {
        LOG(0, "Render W/H not support changed. W/H(%d, %d)->(%d, %d)!\n ", 
            prFrcPrm->rRenderRegion.u4Width, prFrcPrm->rRenderRegion.u4Height, 
            prRenderRegion->u4Width, prRenderRegion->u4Height);
        ASSERT(0);
        return 0;
    }

    if (prFrcPrm->fgRenderRegionChg)
    {
        LOG(1, "Change too fast: the last X,Y change not process yet!\n ");
        return 0;
    }

    if ((prRenderRegion->u4X == prFrcPrm->rRenderRegion.u4X) &&
        (prRenderRegion->u4Y == prFrcPrm->rRenderRegion.u4Y))
    {
        LOG(1, "RenderXY not change: X,Y(%d, %d) -> (%d, %d)!\n ", 
            prFrcPrm->rRenderRegion.u4X, prFrcPrm->rRenderRegion.u4Y, 
            prRenderRegion->u4X, prRenderRegion->u4Y);
        return 0;
    }
    
    LOG(3, "RenderXY changed: X,Y(%d, %d) -> (%d, %d)!\n ", 
        prFrcPrm->rRenderRegion.u4X, prFrcPrm->rRenderRegion.u4Y, 
        prRenderRegion->u4X, prRenderRegion->u4Y);
    
    prFrcPrm->fgRenderRegionChg = TRUE;
    prFrcPrm->rRenderRegion.u4X = ((UINT32)prRenderRegion->u4X + 0xF) & (~0xF);
    prFrcPrm->rRenderRegion.u4Y = prRenderRegion->u4Y;

    return 1;
}


#ifdef CC_B2R_ENABLE_CHG_FRAME_MSG
static INT32 _VDP_ReceiveChgFrameMsg(UCHAR ucB2rId,
                                     VOID            *pv_msg,
                                     MSGQ_OPTION_T   e_option)
{
    INT32 iRet;
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;

    if ((ucB2rId >= B2R_NS) || !pv_msg)
    {
        return OSR_INV_ARG;
    }

    zMsgSize = sizeof(VDP_B2R_CHG_FRAME_MSG_T);

    VERIFY(x_sema_lock(_ahChgFrameMutex[ucB2rId], X_SEMA_OPTION_WAIT) == OSR_OK);
    iRet = x_msg_q_receive(&u2MsgQIdx, pv_msg, &zMsgSize, &(_ahChgFrameQueue[ucB2rId]), 1, e_option);
    VERIFY(x_sema_unlock(_ahChgFrameMutex[ucB2rId]) == OSR_OK);


    return iRet;
}

BOOL _VDP_SendB2RChgFrameMsg(VDP_B2R_CHG_FRAME_MSG_T* prMsg)
{
    VDP_B2R_CHG_FRAME_MSG_T rMsg;
    B2R_PRM_T* prVdpPrm = NULL;

    x_memset(&rMsg,0,sizeof(VDP_B2R_CHG_FRAME_MSG_T));

    rMsg.u4VdpId = prMsg->u4VdpId;
    rMsg.u4Flag = prMsg->u4Flag;

    rMsg.u4DispMode = prMsg->u4DispMode;
    rMsg.ucFbgId= prMsg->ucFbgId;
    rMsg.ucFbId = prMsg->ucFbId;

    if (rMsg.u4VdpId >= VDP_NS)
    {
        LOG(1,"u4VdpId >= VDP_NS oops\n");
        return FALSE;
    }

    prVdpPrm = (B2R_PRM_T*)_VDP_GetVdpPrm(VDP2B2RID(rMsg.u4VdpId));

    if (rMsg.u4DispMode == VDP_B2R_CHG_FRAME_DIRECT && (prVdpPrm->ucFbgId != rMsg.ucFbgId))
    {
        LOG(1, "B2R(%d) render FB(%d-%d) mismatch B2R_Fbg(%d).\n",
            VDP2B2RID(rMsg.u4VdpId), rMsg.ucFbgId, rMsg.ucFbId, prVdpPrm->ucFbgId);
        return FALSE;
    }

    LOG_REL(_fgEnableGstLog, 1, 7, "B2R(%d) render: Mode(%d) FB(%d-%d).\n",
         VDP2B2RID(rMsg.u4VdpId), rMsg.u4DispMode, rMsg.ucFbgId, rMsg.ucFbId);

    if (rMsg.u4Flag & VDP_B2R_CHG_FRAME_MSG_SYNC)
    {
        VDP_B2R_CHG_FRAME_MSG_T rPopOutMsg;
        B2R_PRM_T* prVdpPrm = NULL;
        UCHAR ucFbId = FBM_FB_ID_UNKNOWN;

        prVdpPrm = (B2R_PRM_T*)_VDP_GetVdpPrm(VDP2B2RID(rMsg.u4VdpId));
        if (prVdpPrm && (prVdpPrm->ucFbgId != FBM_FBG_ID_UNKNOWN))
        {
            while (_VDP_ReceiveChgFrameMsg(VDP2B2RID(rMsg.u4VdpId), &rPopOutMsg, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
            {
                if (rPopOutMsg.u4DispMode == VDP_B2R_CHG_FRAME_DIRECT)
                {
                    if (rPopOutMsg.ucFbId != rMsg.ucFbId)
                    {
                        LOG_REL(_fgEnableGstLog, 1, 3, "sync disp: overwrite display frame : FB(%d-%d).\n", 
                                rPopOutMsg.ucFbgId, rPopOutMsg.ucFbId);
                        _VdpSetFrameBufferStatus((prVdpPrm), rPopOutMsg.ucFbgId, rPopOutMsg.ucFbId, FBM_FB_STATUS_EMPTY);
                    }
                    continue;
                }

                ucFbId = FBM_GetFrameBufferFromDispQ(prVdpPrm->ucFbgId);

                LOG_REL(_fgEnableGstLog, 1, 3, "sync disp: drop old frames Fb(%d-%d)\n", prVdpPrm->ucFbgId, ucFbId);

                if (ucFbId != FBM_FB_ID_UNKNOWN)
                {
                    _VdpSetFrameBufferStatus((prVdpPrm), prVdpPrm->ucFbgId, ucFbId, FBM_FB_STATUS_LOCK);
                    _VdpSetFrameBufferStatus((prVdpPrm), prVdpPrm->ucFbgId, ucFbId, FBM_FB_STATUS_EMPTY);
                }
            }
        }
    }

    if (x_msg_q_send(_ahChgFrameQueue[VDP2B2RID(rMsg.u4VdpId)], (void *)(&rMsg), sizeof(VDP_B2R_CHG_FRAME_MSG_T), 0) != OSR_OK)
    {
        LOG(1,"Msg Queue full for change frame buffer Msg queue\n");
        return FALSE;
    }
    
#ifndef CC_USE_DDI
    if (rMsg.u4DispMode == VDP_B2R_CHG_FRAME_DIRECT)
    {
        FBM_TriggerSmlsByResizer(rMsg.ucFbgId, rMsg.ucFbId);
    }
#endif

    //_VdpFrcFrameBufferReady((_arVdpPrm[0]).ucFbgId);

    return TRUE;
}

BOOL _VDP_FlushB2RChgFrameMsg(UCHAR ucB2rId)
{
    //UINT16 u2MsgQIdx;
    //SIZE_T zMsgSize;
    UINT16 u2Cnt;
    VDP_B2R_CHG_FRAME_MSG_T rMsg;
    B2R_PRM_T* prVdpPrm = NULL;

    if (ucB2rId >= B2R_NS)
    {
        LOG(3,"ucB2rId >= B2R_NS oops\n");
        return FALSE;
    }
    prVdpPrm = (B2R_PRM_T*)_VDP_GetVdpPrm(ucB2rId);

    if (_ahChgFrameQueue[ucB2rId] != (HANDLE_T)(NULL) && prVdpPrm)
    {
        while (_VDP_ReceiveChgFrameMsg(ucB2rId, &rMsg, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
        {
            if (rMsg.u4DispMode == VDP_B2R_CHG_FRAME_DIRECT)
            {
                _VdpSetFrameBufferStatus(prVdpPrm, rMsg.ucFbgId, rMsg.ucFbId, FBM_FB_STATUS_EMPTY);
            }
        }
        VERIFY(x_msg_q_num_msgs(_ahChgFrameQueue[ucB2rId], &u2Cnt) == OSR_OK);
        ASSERT(u2Cnt== 0);

        _afgPendingChgFrmMsg[ucB2rId] = FALSE;
        x_memset(&(_arPendingChgFrmMsg[ucB2rId]), 0, sizeof(VDP_B2R_CHG_FRAME_MSG_T));
    }
    return TRUE;
}
#endif

//release framebuffer which is locked by B2R
static void _VdpRelResource(UCHAR ucB2rId)
{
    B2R_PRM_T * prFrcPrm = NULL;

    if (ucB2rId >= B2R_NS)
    {
        LOG(3,"ucB2rId >= B2R_NS oops\n");
        return ;
    }

    prFrcPrm = &_arVdpPrm[ucB2rId];

    if (prFrcPrm && prFrcPrm->ucFbgId != FBM_FBG_ID_UNKNOWN)
    {
        if (prFrcPrm->ucReleaseFbId !=FBM_FBG_ID_UNKNOWN)
        {
            if (FBM_ChkFbgCreateFromInst(prFrcPrm->ucFbgId) == FALSE)
            {
                if (FBM_CheckFrameBufferStatus(prFrcPrm->ucFbgId, prFrcPrm->ucReleaseFbId, FBM_FB_STATUS_LOCK))
                {
                    _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucFbgId, prFrcPrm->ucReleaseFbId, FBM_FB_STATUS_EMPTY);
                }
            }
            else
            {
                _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucFbgId, prFrcPrm->ucReleaseFbId, FBM_FB_STATUS_EMPTY);
            }
            prFrcPrm->ucReleaseFbId = FBM_FB_ID_UNKNOWN;
        }

        if (prFrcPrm->ucFbId !=FBM_FBG_ID_UNKNOWN)
        {
            if (FBM_ChkFbgCreateFromInst(prFrcPrm->ucFbgId) == FALSE)
            {
                if (FBM_CheckFrameBufferStatus(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_FB_STATUS_LOCK))
                {
                    _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_FB_STATUS_EMPTY);
                }
            }
            else
            {
                _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_FB_STATUS_EMPTY);
            }
            prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
        }
    }
}

static void _VDPGetFrameBufferForBivl3D(B2R_PRM_T* prFrcPrm)
{
	FBM_SEQ_HDR_T* prSeqHdr = FBM_GetFrameBufferSeqHdr(prFrcPrm->ucFbgId);
	if (prSeqHdr)
    {
        UCHAR ucLFbId = FBM_FB_ID_UNKNOWN;
        UCHAR ucRFbId = FBM_FB_ID_UNKNOWN;
        FBM_PIC_HDR_T* prLPicHdr = NULL;
        FBM_PIC_HDR_T* prRPicHdr = NULL;
        INT64 i8LPts, i8RPts;
        BOOL fgLRealSide;
        BOOL fgRRealSide;
        UCHAR ucIndex = prSeqHdr->ucFrmRatCod;
        UCHAR ucFrameRate = _VDPConvertFrameRateInx2FrameRate(prFrcPrm, ucIndex);
        INT32 i4Interval = VDP_STC_CLOCK / ucFrameRate / 2;
        while (TRUE)
        {
        	ucLFbId = FBM_PreLookFrameBufferFromDispQ(prFrcPrm->ucFbgId);
        	if (ucLFbId == FBM_FB_ID_UNKNOWN)//no frame available, go...
        	{
        		prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
        			LOG(3, "BIVL, no frame available, go...\n");
        		break;
        	}
        	else 
        	{
        		ucRFbId = FBM_PreLook2ndFrameBufferFromDispQ(prFrcPrm->ucFbgId);
        		if (ucRFbId == FBM_FB_ID_UNKNOWN)
        		{
        			prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
        				LOG(3, "BIVL, no 2 frames available, go...\n ");
        			break;
        		}
        	}

        	prLPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, ucLFbId);
        	prRPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, ucRFbId);
        	
        	if (prLPicHdr && prRPicHdr)
        	{
        		fgLRealSide = prLPicHdr->fgBivl3DLR;
        		fgRRealSide = prRPicHdr->fgBivl3DLR;
        		if (fgLRealSide && !fgRRealSide)//L&R
        		{
        			i8LPts = prLPicHdr->u4PTS;
        			i8RPts = prRPicHdr->u4PTS;
        			if (i8RPts - i8LPts - i4Interval <= 500 && i8RPts - i8LPts - i4Interval >= -500)//right case, and go...
        			{
        				LOG(3, "BIVL, right case, L = %d, R = %d, go...\n", ucLFbId, ucRFbId);
        				prFrcPrm->ucFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
        				FBM_SetFrameBufferStatus(prFrcPrm->ucFbgId, prFrcPrm->ucFbId, FBM_FB_STATUS_LOCK);
#ifdef CC_3D_MM_DS_SUPPORT
        				prFrcPrm->ucSubFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
        				FBM_SetFrameBufferStatus(prFrcPrm->ucFbgId, prFrcPrm->ucSubFbId, FBM_FB_STATUS_LOCK);
#endif
        				break;
        			}
        			else 
        			{
        				LOG(3, "BIVL, the difference between L and R's PTS is not legal, and go on to fetch L and R\n");
        				ucLFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
        				FBM_SetFrameBufferStatus(prFrcPrm->ucFbgId, ucLFbId, FBM_FB_STATUS_LOCK);
        				FBM_SetFrameBufferStatus(prFrcPrm->ucFbgId, ucLFbId, FBM_FB_STATUS_EMPTY);
        				ucRFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
        				FBM_SetFrameBufferStatus(prFrcPrm->ucFbgId, ucRFbId, FBM_FB_STATUS_LOCK);
        				FBM_SetFrameBufferStatus(prFrcPrm->ucFbgId, ucRFbId, FBM_FB_STATUS_EMPTY);
        				ucLFbId = ucRFbId = FBM_FB_ID_UNKNOWN;
        			}
        		}
        		else if (!fgLRealSide && !fgRRealSide)//R&R
        		{
        			LOG(3, "BIVL, wrong case, drop both frames, and go on to fetch L and R\n");
        			ucLFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
        			FBM_SetFrameBufferStatus(prFrcPrm->ucFbgId, ucLFbId, FBM_FB_STATUS_LOCK);
        			FBM_SetFrameBufferStatus(prFrcPrm->ucFbgId, ucLFbId, FBM_FB_STATUS_EMPTY);
        			ucRFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
        			FBM_SetFrameBufferStatus(prFrcPrm->ucFbgId, ucRFbId, FBM_FB_STATUS_LOCK);
        			FBM_SetFrameBufferStatus(prFrcPrm->ucFbgId, ucRFbId, FBM_FB_STATUS_EMPTY);
        			ucLFbId = ucRFbId = FBM_FB_ID_UNKNOWN;
        		}
        		else//R&L, L&L
        		{
        			LOG(3, "BIVL, wrong case, drop left frame, and go on to fetch L and R\n");
        			ucLFbId = FBM_GetFrameBufferFromDispQ(prFrcPrm->ucFbgId);
        			FBM_SetFrameBufferStatus(prFrcPrm->ucFbgId, ucLFbId, FBM_FB_STATUS_LOCK);
        			FBM_SetFrameBufferStatus(prFrcPrm->ucFbgId, ucLFbId, FBM_FB_STATUS_EMPTY);
        			ucLFbId = FBM_FB_ID_UNKNOWN;
        		}
        	}
        	else
        	{
        		ASSERT(prLPicHdr && prRPicHdr);
        	}
        }
    }
	else
	{
		prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
	}
}

static UCHAR _VDPConvertFrameRateInx2FrameRate(B2R_PRM_T* prFrcPrm, UCHAR ucIndex)
{
	switch (ucIndex)
    {
    case MPEG_FRAME_RATE_12:
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_12;
        break;

    case MPEG_FRAME_RATE_15:
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_15;
        break;

    case MPEG_FRAME_RATE_20:
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_20;
        break;

    case MPEG_FRAME_RATE_24_:
    case MPEG_FRAME_RATE_24:
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_24;
        break;

    case MPEG_FRAME_RATE_25:
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_25;
        break;

    case MPEG_FRAME_RATE_30_:
    case MPEG_FRAME_RATE_30:
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_30;
        break;

    case MPEG_FRAME_RATE_50:
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_50;
        break;
    case MPEG_FRAME_RATE_120_:
    case MPEG_FRAME_RATE_120:
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_120;
        break;
   case MPEG_FRAME_RATE_240_:
   case MPEG_FRAME_RATE_240:
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_240;
        break;

    case MPEG_FRAME_RATE_60_:
    case MPEG_FRAME_RATE_60:
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_60;
        break;

    case MPEG_FRAME_RATE_UNKNOWN:
    default:
        _VdpErrNotify(1, prFrcPrm->ucB2rId, prFrcPrm->ucFbgId, ucIndex);
        LOG(1, "Unknown Frame Rate %d\n", ucIndex);
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_30;
        break;
    }
	return prFrcPrm->ucInFrameRate;
}

static BOOL _VdpIsBivl3D(B2R_PRM_T * prFrcPrm)
{
#ifdef CC_3D_MM_DS_SUPPORT
#ifdef ENABLE_MULTIMEDIA
    VDEC_ES_INFO_T* prVdecEsInfo = _B2R_GetEsInfo(prFrcPrm->ucFbgId);
    if (prVdecEsInfo && prVdecEsInfo->e3DType == VDEC_3D_BIVL)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
#endif
#endif
    return FALSE;
}

void _VdpCalcRepeatCntForPushTrickMode(B2R_PRM_T* prFrcPrm, BOOL fgLastFbIsDrop)
{
    UINT32 u4FirstPts;
    UINT32 u4LastPts;
    UINT32 u4Speed;
    FBM_PIC_HDR_T* prFirstHdr = NULL;
    FBM_PIC_HDR_T* prLastHdr = NULL;
    prFirstHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, prFrcPrm->ucLastFbId);
    prLastHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, prFrcPrm->ucFbId);
    if (prFirstHdr && prFirstHdr->fgPushModeTrick && !fgLastFbIsDrop && prLastHdr &&
		((!_u4RepeatCntForPushTrickMode && !_u4ElapseCntForPushTrickMode) || (!_u4RepeatCntForPushTrickMode && _u4ElapseCntForPushTrickMode)))
    {
        _ucRepeatFbForPushTrickMode = prFrcPrm->ucLastFbId;
        u4FirstPts = prFirstHdr->u4PTS;
        u4LastPts = prLastHdr->u4PTS;
        u4Speed = _eSpeed[prFrcPrm->ucB2rId];
        if (u4LastPts > u4FirstPts && u4Speed >= STC_SPEED_TYPE_FORWARD_4X && u4Speed <= STC_SPEED_TYPE_FORWARD_1024X && prFrcPrm->ucInFrameRate != FBM_FRAME_RATE_UNKNOWN)
        {
            _u4RepeatCntForPushTrickMode = (u4LastPts - u4FirstPts) / (1 << (u4Speed - STC_SPEED_TYPE_FORWARD_4X + 2)) / (VDP_STC_CLOCK / prFrcPrm->ucInFrameRate)  + 1;
        }
        else if (u4LastPts < u4FirstPts && u4Speed >= STC_SPEED_TYPE_REWIND_4X && u4Speed <= STC_SPEED_TYPE_REWIND_1024X && prFrcPrm->ucInFrameRate != FBM_FRAME_RATE_UNKNOWN)
        {
            _u4RepeatCntForPushTrickMode = (u4FirstPts - u4LastPts) / (1 << (u4Speed - STC_SPEED_TYPE_REWIND_4X + 2)) / (VDP_STC_CLOCK / prFrcPrm->ucInFrameRate)  + 1;
        }
        else 
        {
            _u4RepeatCntForPushTrickMode = 1;
        }
    }
}

void _VdpGetFrameBufferFromPending(B2R_PRM_T * prFrcPrm)
{
    prFrcPrm->ucFbId = prFrcPrm->ucPendingFbId;
    prFrcPrm->ucPendingFbId = FBM_FB_ID_UNKNOWN;

#ifdef CC_3D_MM_DS_SUPPORT
    if (prFrcPrm->ucSubPendingFbId != FBM_FB_ID_UNKNOWN)
    {
        prFrcPrm->ucSubFbId = prFrcPrm->ucSubPendingFbId;
        prFrcPrm->ucSubPendingFbId = FBM_FB_ID_UNKNOWN;
    }
#endif
}

void _VdpPreparePendingForNextVsync(B2R_PRM_T * prFrcPrm)
{
    prFrcPrm->u2TargetNs = 0;
    prFrcPrm->u2ChangeFieldNs = 0;
    prFrcPrm->ucPendingFbId = prFrcPrm->ucFbId;
    prFrcPrm->ucLastFbId = FBM_FB_ID_UNKNOWN;
    prFrcPrm->ucFbId = FBM_FB_ID_UNKNOWN;
    prFrcPrm->u4InFrameStc = 0;
    prFrcPrm->u4OutFrameStc = 0;
#ifdef CC_3D_MM_DS_SUPPORT
    // If L view is pending, R view should be pending, too.
    ASSERT(prFrcPrm->ucSubPendingFbId == FBM_FB_ID_UNKNOWN);
    prFrcPrm->ucSubPendingFbId = prFrcPrm->ucSubFbId;
    prFrcPrm->ucSubFbId = FBM_FB_ID_UNKNOWN;
#endif
}

void _VdpDetermineRepeatForPushTrickMode(B2R_PRM_T * prFrcPrm, BOOL fgRepeatFbForPending, BOOL* pfgDropFb, UCHAR ucLastFbForNoFb)
{
	if (_u4RepeatCntForPushTrickMode > 0)
	{
		_u4ElapseCntForPushTrickMode++;
		LOG(3, "go on to repeat, %d(%d, %d)\n", _ucRepeatFbForPushTrickMode, _u4ElapseCntForPushTrickMode, _u4RepeatCntForPushTrickMode);
		if (_u4ElapseCntForPushTrickMode < _u4RepeatCntForPushTrickMode || fgRepeatFbForPending)
		{
            if (!fgRepeatFbForPending)
			{
                _VdpPreparePendingForNextVsync(prFrcPrm);
            }
            *pfgDropFb = FALSE;
		}
		else 
		{
			_u4ElapseCntForPushTrickMode = _u4RepeatCntForPushTrickMode = 0;
		}
	}
	else 
	{
        FBM_PIC_HDR_T* prHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, ucLastFbForNoFb);
        if (prHdr && prHdr->fgPushModeTrick)
        {
            _u4ElapseCntForPushTrickMode++;
            LOG(3, "go on to repeat, %d(%d, ?)\n", ucLastFbForNoFb, _u4ElapseCntForPushTrickMode);
        }
        else
        {
		    //normal case
        }
	}
}

//-----------------------------------------------------------------------------
/** Brief of _VDP_RTEnSuperGameMode.
** For NPTV to query current Real Time src type, need to enable super game mode or not *
** Super game mode will bypass MJC. (Bypass MJC will black with OSD!!)
 */
//-----------------------------------------------------------------------------
BOOL _VDP_RTNeedSuperGameMode(UCHAR ucVdpId)
{
    #ifdef ENABLE_MULTIMEDIA
    UCHAR ucEsId;
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;
    if(ucVdpId >= VDP_NS)
    {
        return FALSE;
    }
    
    ucEsId = _prVdpConf[ucVdpId]->ucInputPort[0];
    prVdecEsInfo = (ucEsId < MAX_ES_NS)? _VDEC_GetEsInfo(ucEsId) : NULL;
    if (prVdecEsInfo && 
        (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_WIFI_DISPLAY))
    {
        return TRUE;
    }
    #else
    UNUSED(ucVdpId);
    #endif
    return FALSE;
}

void _VDP_FinishSeekDoneBeforeEnding(UCHAR ucVdpId)
{
    UCHAR ucSeekMode, ucSeekNotifyTrigger;
    UINT32 u4SeekPts;
    UCHAR ucB2rId = VDP_GetB2rId(ucVdpId);
    if (ucB2rId >= B2R_NS)
    {
        return;
    }
    if (VDP_HalGetStepForward(ucB2rId) == VDP_PLAY_STEP_FORWARD)
    {
        _VDP_VsyncNotify(VDP_MSG_MM_STEP_FIN_CB, ucVdpId, 0, VDP_SEEK_STEP_OK);
        VDP_HalSetStepForward(ucB2rId, VDP_PLAY_NORMAL);
    }
    _VDPGetSeek(ucVdpId, &ucSeekMode, &u4SeekPts, &ucSeekNotifyTrigger);
    if ((ucSeekMode != 0 && ucSeekNotifyTrigger != 0) || (_ucRemainTimeToSendSeekDone > 0))
    {
        /* Seek Finish notify trigger status clear */
        arVdpSeek[ucB2rId].ucNotifyTrigger = 0;
        _ucRemainTimeToSendSeekDone = 0;
        /* Seek Finish Notification */
        _VDP_VsyncNotify(VDP_MSG_MM_SEEK_FIN_CB, ucVdpId, u4SeekPts, VDP_SEEK_STEP_OK);
    }
}

#ifdef ENABLE_MULTIMEDIA
/*-----------------------------------------------------------------------------
* Name: _VDP_RTFrameRateConvert
*
* Description: This API Determine Enable/Disable Double Clock from Frame Rate.
*              
* Inputs:  prFrcPrm    Specifies the struct of the B2R PRM. Different B2R PRM 
*                       refer to different B2R Id.
*          
*             ucIndex  Specifies the Frame rate code
*
* Outputs: 
*
* Function: For WiFi Display , if input frame rate less or equal than 30fps , Enable Double Clock, Reduce Target NS for A/V sync
*              Otherwise Disable Double Clock and Keep Drop Frame When A/V sync
----------------------------------------------------------------------------*/
static UCHAR _VDP_RTFrameRateConvert(B2R_PRM_T* prFrcPrm, UCHAR ucIndex)
{
	switch (ucIndex)
    {
    case MPEG_FRAME_RATE_12:
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_24;
        prFrcPrm->t_wfd_prm.fgDblClk = TRUE;
        break;

    case MPEG_FRAME_RATE_15:
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_30;
        prFrcPrm->t_wfd_prm.fgDblClk = TRUE;
        break;

    case MPEG_FRAME_RATE_20:
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_20;
        break;
        
    case MPEG_FRAME_RATE_24_:
    case MPEG_FRAME_RATE_24:
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_24;
        break;

    case MPEG_FRAME_RATE_25:
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_50;
        prFrcPrm->t_wfd_prm.fgDblClk = TRUE;
        break;

    case MPEG_FRAME_RATE_30_:
    case MPEG_FRAME_RATE_30:
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_60;
        prFrcPrm->t_wfd_prm.fgDblClk = TRUE;
        break;
        
    case MPEG_FRAME_RATE_50:
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_50;
        break;

    case MPEG_FRAME_RATE_60_:
    case MPEG_FRAME_RATE_60:
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_60;
        break;

    case MPEG_FRAME_RATE_UNKNOWN:
    default:
        _VdpErrNotify(1, prFrcPrm->ucB2rId, prFrcPrm->ucFbgId, ucIndex);
        LOG(1, "wfd not support Frame Rate %d , fgDblClk : %d\n", 
            ucIndex,prFrcPrm->t_wfd_prm.fgDblClk);
        prFrcPrm->ucInFrameRate = FBM_FRAME_RATE_60;
        break;
    }
	return prFrcPrm->ucInFrameRate;
}

/*-----------------------------------------------------------------------------
* Name: _VDP_RTPtsStcDiff
*
* Description: This API Get Audio/Video PTS info from SWDMX.
*              
* Inputs:  prFrcPrm    Specifies the struct of the B2R PRM. Different B2R PRM 
*                       refer to different B2R Id.
*          
*             pt_pts_info  Specifies the PTS info
*
* Outputs: pt_pts_info  Specifies the PTS info
*
* Function: Get SWDMX current AUDIO/VIDEO PTS and Last PTS interval time
----------------------------------------------------------------------------*/
static INT32 _VDP_RTPtsStcDiff(B2R_PRM_T*             prFrcPrm, 
                                    B2R_WFD_PTS_INFO_T*    pt_pts_info)
{
    UINT32  u4SwdmxId  = 0;
    UCHAR   ucAvSyncMode;
    UCHAR   ucStcSrc;
    INT32   i4Ret   = B2R_OK;
    SWDMX_PTS_INFO_T    t_vid_info;
    SWDMX_PTS_INFO_T    t_aud_info;

    if(!prFrcPrm
        || !pt_pts_info)
    {
        return B2R_INV_ARG;
    }

    do
    {
        u4SwdmxId = prFrcPrm->u4AttachedSrcId;
        FBM_GetSyncStc(prFrcPrm->ucFbgId, &ucAvSyncMode, &ucStcSrc);
        if(ucStcSrc >= STC_SRC_NS
            || u4SwdmxId >= SWDMX_SOURCE_MAX)
        {
            i4Ret = B2R_FAIL;
            break;
        }
        
        if(!SWDMX_GetInfo(u4SwdmxId, eSWDMX_GET_VIDEO_PTS_INFO, (UINT32*)&t_vid_info, NULL, NULL))
        {
            i4Ret = B2R_FAIL;
            break;
        }

        if(!SWDMX_GetInfo(u4SwdmxId, eSWDMX_GET_AUDIO_PTS_INFO, (UINT32*)&t_aud_info, NULL, NULL))
        {
            i4Ret = B2R_FAIL;
            break;
        }

        pt_pts_info->u4CurStc        = STC_GetStcValue(ucStcSrc);
        pt_pts_info->u4_vid_pts      = t_vid_info.u4Pts;
        pt_pts_info->i4_vid_diff = pt_pts_info->u4_vid_pts - pt_pts_info->u4CurStc;
        pt_pts_info->u4_aud_pts      = t_aud_info.u4Pts;
        pt_pts_info->i4_aud_diff     = pt_pts_info->u4_aud_pts - pt_pts_info->u4CurStc;
        
        if(t_vid_info.rDeltaTime.u4Micros > t_aud_info.rDeltaTime.u4Micros)
        {
            pt_pts_info->rDeltaTime = t_vid_info.rDeltaTime;
        }
        else
        {
            pt_pts_info->rDeltaTime = t_aud_info.rDeltaTime;
        }
        
    }while(0);

    if(B2R_OK != i4Ret)
    {
        pt_pts_info->u4CurStc        = 0;
        pt_pts_info->u4_vid_pts      = 0;
        pt_pts_info->i4_vid_diff     = 0;
        pt_pts_info->u4_aud_pts      = 0;
        pt_pts_info->i4_aud_diff     = 0;
        x_memset(&pt_pts_info->rDeltaTime,0x0,sizeof(HAL_TIME_T));
    }

    return i4Ret;
}

/*-----------------------------------------------------------------------------
* Name: _VDP_RTVarInitial
*
* Description: This API Initial Wifi Display Related parameter.
*              
* Inputs:  prFrcPrm    Specifies the struct of the B2R PRM. Different B2R PRM 
*                       refer to different B2R Id.
*          
* Outputs: 
*
* Function: Initial Wifi Display Related parameter
----------------------------------------------------------------------------*/
static inline INT32 _VDP_RTVarInitial (B2R_PRM_T*   prFrcPrm)
{
    if(!prFrcPrm)
    {
        return B2R_INV_ARG;
    }

    prFrcPrm->t_wfd_prm.u4_pkt_cnt   = 0;

    if(prFrcPrm->ptRtCli &&
        prFrcPrm->ptRtCli->u4_latency_ns)
    {
        prFrcPrm->t_wfd_prm.fgDnmcltncy  = FALSE;
        prFrcPrm->t_wfd_prm.u4_ltncy_ns  = prFrcPrm->ptRtCli->u4_latency_ns;
        LOG(1,"%s , Disable Dynamic Latency , u4_ltncy_ns : %d!\n",__FUNCTION__,
            prFrcPrm->ptRtCli->u4_latency_ns);
    }
    else
    {
        prFrcPrm->t_wfd_prm.fgDnmcltncy  = TRUE;
        prFrcPrm->t_wfd_prm.u4_ltncy_ns  = VDP_RT_INITIAL_CNT;
        LOG(1,"%s , Enable Dynamic Latency!\n",__FUNCTION__);
    }
    prFrcPrm->t_wfd_prm.u4_ltncy_max = 0;
    prFrcPrm->t_wfd_prm.fgInitalized = TRUE;
    HAL_GetTime(&(prFrcPrm->t_wfd_prm.t_start_time));

    FBM_SetWfdFrameBufferFlag(prFrcPrm->ucFbgId,FBM_WFD_FRAME_MONITOR);
    FBM_ClrWfdFrameBufferFlag(prFrcPrm->ucFbgId,FBM_WFD_FRAME_COUNTING);

    return B2R_OK;
}

/*-----------------------------------------------------------------------------
* Name: _VDP_RTAVsyncInitializing
*
* Description: This API Initial Wifi Display Playback status.
*              
* Inputs:  prFrcPrm    Specifies the struct of the B2R PRM. Different B2R PRM 
*                       refer to different B2R Id.
*
*             pt_pts_info Specifies the PTS info
*
* Outputs: 
*
* Function: Use PTS info ,Wait Video PTS and STC status stable , then Initial WFD paramerer for start to playback
----------------------------------------------------------------------------*/
static INT32 _VDP_RTAVsyncInitializing (B2R_PRM_T*            prFrcPrm,
                                             B2R_WFD_PTS_INFO_T*   pt_pts_info)
{
    UCHAR   ucAvSyncMode;
    UCHAR   ucStcSrc;
    INT32   i4Ret   = B2R_OK;

    if(!prFrcPrm)
    {
        return B2R_INV_ARG;
    }

    do
    {
        FBM_GetSyncStc(prFrcPrm->ucFbgId, &ucAvSyncMode, &ucStcSrc);
        if(ucStcSrc >= STC_SRC_NS)
        {
            i4Ret = B2R_FAIL;
            break;
        }
        
        if(pt_pts_info->i4_vid_diff >= 0
            && pt_pts_info->i4_vid_diff > prFrcPrm->u4OutStcPeriod * VDP_RT_INITIAL_CNT)
        {
            LOG(1,"%s , u4_vid_pts : 0x%x!\n",__FUNCTION__,pt_pts_info->u4_vid_pts);
            STC_SetStcValue(ucStcSrc, pt_pts_info->u4_vid_pts);
        }
        else
        {
            _VDP_RTVarInitial(prFrcPrm);
            LOG(3,"B2R RT Initialize End ......\n");
        }
    }while(0);

    return i4Ret;
}

/*-----------------------------------------------------------------------------
* Name: _VDP_RTStcCorrection
*
* Description: This API Fine tuning STC.
*              
* Inputs:  prFrcPrm    Specifies the struct of the B2R PRM. Different B2R PRM 
*                       refer to different B2R Id.
*
*             pt_pts_info Specifies the PTS info
*
* Outputs: 
*
* Function: Use PTS info ,Fine tuning STC . Keep STC and SWDMX Latest input PTS Diff in range [Latency Time/2 , Latency Time]
----------------------------------------------------------------------------*/
static INT32 _VDP_RTStcCorrection(B2R_PRM_T*            prFrcPrm,
                                          B2R_WFD_PTS_INFO_T*   pt_pts_info)
{
    UINT32  u4Delta = 0;
    UINT32  u4StcAdj;
    INT32   i4Ret = B2R_OK;
    UCHAR   ucAvSyncMode;
    UCHAR   ucStcSrc;
    
    if(!prFrcPrm)
    {
        return B2R_INV_ARG;
    }

    do
    {
        FBM_GetSyncStc(prFrcPrm->ucFbgId, &ucAvSyncMode, &ucStcSrc);
        if(ucStcSrc >= STC_SRC_NS)
        {
            i4Ret = B2R_FAIL;
            break;
        }

        u4Delta = pt_pts_info->i4_vid_diff >= 0?
            pt_pts_info->i4_vid_diff : -(pt_pts_info->i4_vid_diff);

        if(prFrcPrm->t_wfd_prm.t_stc_adj.u4_adj_cnt != 0)
        {
            LOG(1,"%s,stc adj : %d , u4_adj_cnt : %d!\n",__FUNCTION__,
                prFrcPrm->t_wfd_prm.t_stc_adj.i4_ave_diff,prFrcPrm->t_wfd_prm.t_stc_adj.u4_adj_cnt);
            STC_SetStcValue(ucStcSrc,pt_pts_info->u4CurStc + prFrcPrm->t_wfd_prm.t_stc_adj.i4_ave_diff);
            prFrcPrm->t_wfd_prm.t_stc_adj.u4_adj_cnt --;
        }

        if(prFrcPrm->t_wfd_prm.fgInitalized)
        {
            if(!prFrcPrm->t_wfd_prm.fgDnmcltncy)
            {
                prFrcPrm->t_wfd_prm.u4_pkt_cnt ++;
            }
            
            if(prFrcPrm->t_wfd_prm.u4_pkt_cnt % (VDP_RT_STC_ADJ_CNT * VDP_RT_MULTIPLY_THREE) != 0)
            {
                prFrcPrm->t_wfd_prm.t_stc_adj.i4_vid_diff +=
                    pt_pts_info->i4_vid_diff;
                prFrcPrm->t_wfd_prm.t_stc_adj.i4_aud_diff +=
                    pt_pts_info->i4_aud_diff;
                break;
            }
            else
            {

               prFrcPrm->t_wfd_prm.t_stc_adj.i4_ave_diff =
                    ((prFrcPrm->t_wfd_prm.t_stc_adj.i4_vid_diff
                    + prFrcPrm->t_wfd_prm.t_stc_adj.i4_aud_diff)/VDP_RT_DIV_TWO)/(VDP_RT_STC_ADJ_CNT * VDP_RT_MULTIPLY_THREE);

                u4Delta = prFrcPrm->t_wfd_prm.t_stc_adj.i4_ave_diff >= 0 ?
                    prFrcPrm->t_wfd_prm.t_stc_adj.i4_ave_diff : -(prFrcPrm->t_wfd_prm.t_stc_adj.i4_ave_diff);

                u4StcAdj = (prFrcPrm->u4OutStcPeriod - VDP_RT_STC_ADJ)/VDP_RT_DIV_TWO;
                LOG(3,"%s,i4_ave_diff : %d , u4StcAdj : %d!\n",__FUNCTION__,prFrcPrm->t_wfd_prm.t_stc_adj.i4_ave_diff,u4StcAdj);
                if(prFrcPrm->t_wfd_prm.t_stc_adj.i4_ave_diff >= 0)
                {
                    if(u4Delta > prFrcPrm->u4OutStcPeriod * prFrcPrm->t_wfd_prm.u4_ltncy_ns)
                    {
                        u4Delta =
                            u4Delta - prFrcPrm->u4OutStcPeriod * prFrcPrm->t_wfd_prm.u4_ltncy_ns;
                        if(u4Delta > prFrcPrm->u4OutStcPeriod)
                        {
                            prFrcPrm->t_wfd_prm.t_stc_adj.i4_ave_diff = u4Delta/VDP_RT_STC_ADJ_CNT;
                            prFrcPrm->t_wfd_prm.t_stc_adj.u4_adj_cnt = VDP_RT_STC_ADJ_CNT;
                        }
                        else
                        {
                            if(u4Delta != 0)
                            {
                                prFrcPrm->t_wfd_prm.t_stc_adj.i4_ave_diff = u4Delta;
                                prFrcPrm->t_wfd_prm.t_stc_adj.u4_adj_cnt = 1;
                            }
                            else
                            {
                                prFrcPrm->t_wfd_prm.t_stc_adj.u4_adj_cnt = 0;
                            }
                        }
                    }
                    else if(prFrcPrm->t_wfd_prm.u4_ltncy_ns > VDP_RT_LATENCY_AJD_CNT
                        && u4Delta < prFrcPrm->u4OutStcPeriod * prFrcPrm->t_wfd_prm.u4_ltncy_ns/VDP_RT_DIV_TWO)
                    {
                        u4Delta =
                            prFrcPrm->u4OutStcPeriod * prFrcPrm->t_wfd_prm.u4_ltncy_ns/VDP_RT_DIV_TWO - u4Delta;
                        if(u4Delta > prFrcPrm->u4OutStcPeriod)
                        {
                            UINT32 u4_ave_delta;

                            u4_ave_delta = u4Delta/(VDP_RT_STC_ADJ_CNT * VDP_RT_MULTIPLY_THREE);
                            if(u4_ave_delta <= u4StcAdj)
                            {
                                prFrcPrm->t_wfd_prm.t_stc_adj.i4_ave_diff = (-1) * (INT32)u4StcAdj;
                                prFrcPrm->t_wfd_prm.t_stc_adj.u4_adj_cnt = u4Delta/u4StcAdj;
                            }
                            else
                            {
                                prFrcPrm->t_wfd_prm.t_stc_adj.i4_ave_diff = (-1) * (INT32)u4_ave_delta;
                                prFrcPrm->t_wfd_prm.t_stc_adj.u4_adj_cnt = (VDP_RT_STC_ADJ_CNT * VDP_RT_MULTIPLY_THREE);
                            }
                        }
                        else
                        {
                            if(u4Delta != 0)
                            {
                                prFrcPrm->t_wfd_prm.t_stc_adj.i4_ave_diff = (-1) * (INT32)u4Delta;
                                prFrcPrm->t_wfd_prm.t_stc_adj.u4_adj_cnt = 1;
                            }
                            else
                            {
                                prFrcPrm->t_wfd_prm.t_stc_adj.u4_adj_cnt = 0;
                            }
                        }
                    }
                }
                else
                {      
                    if(u4Delta > prFrcPrm->u4OutStcPeriod)
                    {
                        UINT32 u4_ave_delta;

                        u4_ave_delta = u4Delta/(VDP_RT_STC_ADJ_CNT * VDP_RT_MULTIPLY_THREE);
                        if(u4_ave_delta <= u4StcAdj)
                        {
                            prFrcPrm->t_wfd_prm.t_stc_adj.i4_ave_diff = (-1) * (INT32)u4StcAdj;
                            prFrcPrm->t_wfd_prm.t_stc_adj.u4_adj_cnt = u4Delta/u4StcAdj;
                        }
                        else
                        {
                            prFrcPrm->t_wfd_prm.t_stc_adj.i4_ave_diff = (-1) * (INT32)u4_ave_delta;
                            prFrcPrm->t_wfd_prm.t_stc_adj.u4_adj_cnt = (VDP_RT_STC_ADJ_CNT * VDP_RT_MULTIPLY_THREE);
                        }
                    }
                    else
                    {
                        prFrcPrm->t_wfd_prm.t_stc_adj.u4_adj_cnt = 1;
                    }
                }
                prFrcPrm->t_wfd_prm.t_stc_adj.i4_vid_diff = 0;
                prFrcPrm->t_wfd_prm.t_stc_adj.i4_aud_diff = 0;
            }
        }
        
    }while(0);
    
    return i4Ret;
}

/*-----------------------------------------------------------------------------
* Name: _VDP_MM1Calculation
*
* Description: This API Adjust STC using MM1 Algorithm.
*              
* Inputs:  prFrcPrm    Specifies the struct of the B2R PRM. Different B2R PRM 
*                       refer to different B2R Id.
*
*             u4_pic_cnt  Picture Count in 200 ms
*
*             pt_pts_info  Specifies the PTS info
*
* Outputs: 
*
* Function: Use MM1 algorithm to calculation average Latency From Vdec to B2R
*               If Average Latency larger Than defined Latency , We should Adjust STC.
----------------------------------------------------------------------------*/
static INT32 _VDP_MM1Calculation(B2R_PRM_T*           prFrcPrm, 
                                       UINT32               u4_pic_cnt,
                                       B2R_WFD_PTS_INFO_T*  pt_pts_info)
{
    UINT32 u4_arrival_speed = 0;
    UINT32 u4_serve_speed = 0;
    UINT32 u4_ave_wait_time = VDP_RT_LATENCY_TIME;
    UINT32 u4CurStc     = 0;
    UINT32 u4_framerate = 0;
    UINT32 u4_frame_count = 0;
    UINT32 u4_stc_adj     = 0;
    UCHAR  ucStcSrc     = 0;
    UCHAR  ucAvSyncMode = 0;
    INT32  i4Ret   = B2R_OK;
    
    if(!prFrcPrm || prFrcPrm->ucB2rId >= B2R_NS)
    {
        LOG(1, "[%s-%d]return %d.\n", __func__, __LINE__);
        return B2R_INV_ARG;
    }

    do
    {
        FBM_GetSyncStc(prFrcPrm->ucFbgId, &ucAvSyncMode, &ucStcSrc);
        if(ucStcSrc >= STC_SRC_NS)
        {
            i4Ret = B2R_FAIL;
            break;
        }

        u4_framerate = prFrcPrm->t_wfd_prm.fgDblClk ?
                        prFrcPrm->ucInFrameRate/VDP_RT_DIV_TWO : prFrcPrm->ucInFrameRate;
        u4_frame_count = u4_framerate/VDP_STATISTICS_PER_SECOND;
        u4_arrival_speed = u4_pic_cnt * VDP_STATISTICS_PER_SECOND;
        u4_serve_speed = prFrcPrm->ucOutFrameRate;

        if(prFrcPrm->t_wfd_prm.fgDblClk
            && 0 != u4_arrival_speed)
        {
            if(u4_serve_speed >= u4_arrival_speed)
            {
                u4_ave_wait_time = u4_serve_speed * (u4_serve_speed - u4_arrival_speed)/u4_arrival_speed;
            }
            else
            {
                u4_ave_wait_time = VDP_RT_LATENCY_TIME - 1;
            }
        }

        //LOG(2,"u4_ave_wait_time : %d!\n",u4_ave_wait_time);
        if(((prFrcPrm->t_wfd_prm.fgDblClk
            && u4_ave_wait_time < VDP_RT_LATENCY_TIME)
            ||(!prFrcPrm->t_wfd_prm.fgDblClk
            && u4_pic_cnt > u4_frame_count))
            && FBM_CheckFrameBufferDispQ(prFrcPrm->ucFbgId))
        {
            u4CurStc = STC_GetStcValue(ucStcSrc);
            if(u4_pic_cnt > u4_frame_count)
            {
                u4_stc_adj = (u4_pic_cnt - u4_frame_count) * prFrcPrm->u4OutStcPeriod;
                if(u4CurStc + u4_stc_adj > pt_pts_info->u4_vid_pts - prFrcPrm->u4OutStcPeriod * prFrcPrm->t_wfd_prm.u4_ltncy_ns)
                {
                    break;
                }
                LOG(1,"%s adj STC : %d, u4_pic_cnt : %d!\n",__FUNCTION__,u4_stc_adj,u4_pic_cnt);
                STC_SetStcValue(ucStcSrc, u4CurStc + u4_stc_adj);
            }
            else
            {
                u4_stc_adj = prFrcPrm->u4OutStcPeriod;
                if(u4CurStc + u4_stc_adj > pt_pts_info->u4_vid_pts - prFrcPrm->u4OutStcPeriod * prFrcPrm->t_wfd_prm.u4_ltncy_ns)
                {
                    break;
                }
                LOG(1,"%s adj STC : %d, u4_pic_cnt : %d!\n",__FUNCTION__,u4_stc_adj,u4_pic_cnt);
                STC_SetStcValue(ucStcSrc, u4CurStc + u4_stc_adj);
            }
        }
    }while(0);
    
    return i4Ret;
}

/*-----------------------------------------------------------------------------
* Name: _VDP_RTStcAdjust
*
* Description: This API use for Adjust STC.
*              
* Inputs:  prFrcPrm    Specifies the struct of the B2R PRM. Different B2R PRM 
*                       refer to different B2R Id.
*
*             pt_pts_info  Specifies the PTS info
*
* Outputs: 
*
* Function: Frame Count Statistics 
----------------------------------------------------------------------------*/
static INT32 _VDP_RTStcAdjust(B2R_PRM_T*             prFrcPrm,
                                     B2R_WFD_PTS_INFO_T*    pt_pts_info)
{
    INT32   i4Ret   = B2R_OK;

    do
    {
        HAL_TIME_T rTime;
        HAL_TIME_T rDeltaTime;
        UINT32     u4_pic_cnt;
        UINT32     u4_time_interval = 0;
        
        if(!FBM_ChkWfdFrameBufferFlag(prFrcPrm->ucFbgId,FBM_WFD_FRAME_MONITOR))
        {
            i4Ret = B2R_INTERNAL_ERR;
            break;
        }

        HAL_GetTime(&rTime);
        HAL_GetDeltaTime(&rDeltaTime, &(prFrcPrm->t_wfd_prm.t_start_time), &rTime);
        if(rDeltaTime.u4Seconds != 0)
        {
            HAL_GetTime(&(prFrcPrm->t_wfd_prm.t_start_time));
            if(FBM_ChkWfdFrameBufferFlag(prFrcPrm->ucFbgId,FBM_WFD_FRAME_COUNTING))
            {
                FBM_ClrWfdFrameBufferFlag(prFrcPrm->ucFbgId,FBM_WFD_FRAME_COUNTING);
            }
            i4Ret = B2R_INTERNAL_ERR;
            break;
        }
        else                       
        {
            u4_time_interval = (VDP_MICROSECOND_PER_SECOND/VDP_STATISTICS_PER_SECOND);
            if(rDeltaTime.u4Micros >= u4_time_interval)
            {
                if(FBM_ChkWfdFrameBufferFlag(prFrcPrm->ucFbgId,FBM_WFD_FRAME_COUNTING))
                {
                    UINT32 u4_count = 0;

                    u4_count = rDeltaTime.u4Micros/u4_time_interval; 
                    u4_pic_cnt = FBM_WfdFrameStatistics(prFrcPrm->ucFbgId);
                    FBM_ClrWfdFrameBufferFlag(prFrcPrm->ucFbgId,FBM_WFD_FRAME_COUNTING);
                    HAL_GetTime(&(prFrcPrm->t_wfd_prm.t_start_time));
                    if(u4_count != 1)
                    {
                        break;
                    }
                    _VDP_MM1Calculation(prFrcPrm,u4_pic_cnt,pt_pts_info);
                }
            }
        }
                
    }while(0);

    return i4Ret;
}

/*-----------------------------------------------------------------------------
* Name: _VDP_RTLatencyAdjust
*
* Description: This API Dynamic Adjust Latency.
*              
* Inputs:  prFrcPrm    Specifies the struct of the B2R PRM. Different B2R PRM 
*                       refer to different B2R Id.
*
*             pt_pts_info  Specifies the PTS info
*
* Outputs: 
*
* Function: IF Double Clock Enable , we use Adjacent Packet Time Interval to adjust Latency 
----------------------------------------------------------------------------*/
static INT32 _VDP_RTLatencyAdjust(B2R_PRM_T*             prFrcPrm,
                                          B2R_WFD_PTS_INFO_T*    pt_pts_info)
{
    UINT32  u4_vsync_ns;
    INT32   i4Ret   = B2R_OK;
    
    if(!prFrcPrm
        || !pt_pts_info)
    {
        return B2R_INV_ARG;
    }

    do
    {
        u4_vsync_ns = 
            pt_pts_info->rDeltaTime.u4Micros/(prFrcPrm->u4OutStcPeriod * 10);
        if(prFrcPrm->t_wfd_prm.u4_pkt_cnt < VDP_RT_STATICE_CNT)
        {
            if(prFrcPrm->t_wfd_prm.u4_ltncy_max < u4_vsync_ns)
            {
                prFrcPrm->t_wfd_prm.u4_ltncy_max = u4_vsync_ns;
            }
            prFrcPrm->t_wfd_prm.u4_pkt_cnt ++;
            break;
        }

        if(prFrcPrm->t_wfd_prm.u4_ltncy_ns <= prFrcPrm->t_wfd_prm.u4_ltncy_max)
        {
            prFrcPrm->t_wfd_prm.u4_ltncy_max -- ;
            prFrcPrm->t_wfd_prm.u4_ltncy_ns = prFrcPrm->t_wfd_prm.u4_ltncy_max;
        }
        else
        {
            UINT32 u4_ltncy_adj = 0;
            u4_ltncy_adj =
                (prFrcPrm->t_wfd_prm.u4_ltncy_ns - prFrcPrm->t_wfd_prm.u4_ltncy_max)/VDP_RT_DIV_TWO;
            prFrcPrm->t_wfd_prm.u4_ltncy_ns -= u4_ltncy_adj;
        }
        prFrcPrm->t_wfd_prm.u4_pkt_cnt = 0;
        prFrcPrm->t_wfd_prm.u4_ltncy_max = 0;
        if(prFrcPrm->t_wfd_prm.u4_ltncy_ns > VDP_RT_LATENCY_TOLERATE)
        {
            prFrcPrm->t_wfd_prm.u4_ltncy_ns -= VDP_RT_LATENCY_TOLERATE;
        }
        if(prFrcPrm->t_wfd_prm.u4_ltncy_ns > VDP_RT_LATENCY_LIMIT)
        {
            prFrcPrm->t_wfd_prm.u4_ltncy_ns = VDP_RT_LATENCY_LIMIT;
        }
        LOG(3,"%s,B2r latency adj : %d!\n",__FUNCTION__,prFrcPrm->t_wfd_prm.u4_ltncy_ns);
    }while(0);
    return i4Ret;
}

/*-----------------------------------------------------------------------------
* Name: _VDP_RTAVsyncProc
*
* Description: This API Is Main Loop Of Wifi Display A/V Sync Procedure.
*              
* Inputs:  prFrcPrm    Specifies the struct of the B2R PRM. Different B2R PRM 
*                       refer to different B2R Id.
*
* Outputs: 
*
* Function: Main Loop Of WFD A/V Sync , Adjust STC to Keep Latency.
----------------------------------------------------------------------------*/
static INT32 _VDP_RTAVsyncProc(B2R_PRM_T* prFrcPrm)
{
    INT32   i4Ret   = B2R_OK;
    B2R_WFD_PTS_INFO_T  t_pts_info;
    
    if(!prFrcPrm)
    {
        return B2R_INV_ARG;
    }

    do
    {
        i4Ret = _VDP_RTPtsStcDiff(prFrcPrm,&t_pts_info);
        if(B2R_OK != i4Ret)
        {
            break;
        }
        
        if(!prFrcPrm->t_wfd_prm.fgInitalized)
        {
            i4Ret = _VDP_RTAVsyncInitializing(prFrcPrm,&t_pts_info);
            break;
        }

        if(prFrcPrm->t_wfd_prm.fgDnmcltncy)
        {
            i4Ret = _VDP_RTLatencyAdjust(prFrcPrm,&t_pts_info);
            if(B2R_OK != i4Ret)
            {
                break;
            }
        }
        
        i4Ret = _VDP_RTStcAdjust(prFrcPrm,&t_pts_info);
        if(B2R_OK != i4Ret)
        {
            break;
        }
        
        i4Ret = _VDP_RTStcCorrection(prFrcPrm,&t_pts_info);
        if(B2R_OK != i4Ret)
        {
            break;
        }

    }while(0);

    return i4Ret;
}

/*-----------------------------------------------------------------------------
* Name: _B2R_RTCliActive
*
* Description: This API For Disable Dynamic Adjust Latency.
*              
* Inputs:  ucB2rId    Specifies the B2R Id of the B2R PRM. Different B2R PRM 
*                       refer to different B2R Id.
*
*             ucEnable   Specifies Enable/Disable Dynamic CMD
*
* Outputs: 
*
* Function: CLI for Disable Dynamic Latency.
----------------------------------------------------------------------------*/
VOID _B2R_RTCliActive(UCHAR ucB2rId,  UCHAR ucEnable)
{
    if(ucB2rId >= B2R_NS)
    {
        LOG(0,"%s,Invalid B2rId!\n",__FUNCTION__);
    }

    do
    {
        if(ucEnable)
        {
            if(_arVdpPrm[ucB2rId].ptRtCli)
            {
                LOG(0,"%s,RT CLI Already Active!\n",__FUNCTION__);
                break;
            }

            _arVdpPrm[ucB2rId].ptRtCli =
                (VOID*)x_mem_alloc(sizeof(B2R_RT_CLI_T));
            if(!_arVdpPrm[ucB2rId].ptRtCli)
            {
                LOG(0,"%s,RT CLI memmory alloc Failed!\n",__FUNCTION__);
            }
            _arVdpPrm[ucB2rId].ptRtCli->u4_latency_ns     = VDP_RT_INITIAL_CNT;
            LOG(0,"B2R (%d), RT CLI Active Success!\n",ucB2rId);
        }
        else
        {
            if(!_arVdpPrm[ucB2rId].ptRtCli)
            {
                LOG(0,"B2R (%d), RT CLI Already Deactive!\n",ucB2rId);
                break;
            }

            x_mem_free(_arVdpPrm[ucB2rId].ptRtCli);
            LOG(0,"B2R (%d), RT CLI Deactive Success!\n",ucB2rId);
        }
    }while(0);
}

/*-----------------------------------------------------------------------------
* Name: _B2R_RTCliActive
*
* Description: This API Set Static Latency to WiFi Display Test.
*              
* Inputs:  ucB2rId    Specifies the B2R Id of the B2R PRM. Different B2R PRM 
*                       refer to different B2R Id.
*
*             u4Latency  Latency Number , 1 Latency Number = 16 ms
*
* Outputs: 
*
* Function: CLI for Set Static Latency , Should Disable Dynamic First.
----------------------------------------------------------------------------*/
VOID _B2R_SetLatency(UCHAR ucB2rId,  UINT32 u4Latency)
{
    if(ucB2rId >= B2R_NS)
    {
        LOG(0,"%s,Invalid B2rId!\n",__FUNCTION__);
    }

    if(_arVdpPrm[ucB2rId].ptRtCli)
    {
        _arVdpPrm[ucB2rId].ptRtCli->u4_latency_ns = u4Latency;
        LOG(0,"Set Static Latency ns : %d!\n",u4Latency);
    }
    else
    {
        LOG(0,"Please active wfd CLI and disable dynamic latency!\n");
    }
}

#ifdef TIME_MEASUREMENT
VOID _B2R_RT_TMS_Statistics(B2R_PRM_T* prFrcPrm)
{
    FBM_PIC_HDR_T* prPicHdr = NULL;
    struct timeval tv = {0};
    UINT64 u8CurTime = 0;
    UINT64 u8Remainder  = 0;
    static BOOL fgPatternOn = FALSE;
    HAL_TIME_T  rTimeCur, rTimeDelta;

    if(!prFrcPrm)
    {
        LOG(0,"%s , %d ,Invalid Arg!\n",__FUNCTION__,__LINE__);
        return;
    }
    
    if (prFrcPrm->ucReleaseFbId != FBM_FB_ID_UNKNOWN)
    {
#define VDP_CHK_INTERVAL(value, LowBound, HighBound)  (((value) > (LowBound)) && ((value) < (HighBound)))
        static HAL_TIME_T rLastTime = {0, 0};
        HAL_TIME_T  rCurTime = {0, 0}; //rDeltaTime.u4Micros / 1000
        HAL_TIME_T  rDeltaTime = {0, 0};;
        
        HAL_GetTime(&rCurTime);
        if (rLastTime.u4Seconds != 0)
        {
            HAL_GetDeltaTime(&rDeltaTime, &rLastTime, &rCurTime);
            if (VDP_CHK_INTERVAL(rDeltaTime.u4Micros / 1000, 10, 17))
            {
                //u4Rend16ms++;
            }
            else if (VDP_CHK_INTERVAL(rDeltaTime.u4Micros / 1000, 16, 34))
            {
                //u4Rend33ms++;
            }
            else if (VDP_CHK_INTERVAL(rDeltaTime.u4Micros / 1000, 33, 49))
            {
                //u4Rend48ms++;
            }
            else if (VDP_CHK_INTERVAL(rDeltaTime.u4Micros / 1000, 48, 67))
            {
                //u4Rend66ms++;
            }
            else if (VDP_CHK_INTERVAL(rDeltaTime.u4Micros / 1000, 66, 100))
            {
                //u4Rend99ms++;
            }
            else
            {
                //u4Rendxms++;
            }
        }
        
        //u4RendCnt++;
        HAL_GetTime(&rLastTime);
    }
        
    prPicHdr = FBM_GetFrameBufferPicHdr(prFrcPrm->ucFbgId, prFrcPrm->ucFbId);
    if(prPicHdr && prPicHdr->fgWFDTMSLogEnable)
    {
        HAL_GetTime(&rTimeCur);
        HAL_GetDeltaTime(&rTimeDelta, &prPicHdr->rTimePutDispQ, &rTimeCur);
        
        u4TimeCurr = rTimeDelta.u4Seconds*1000 + rTimeDelta.u4Micros/1000;
        u8TimeCount = u8TimeCount + u4TimeCurr;
        u4TimeAver = u8Div6432(u8TimeCount, (u4FrameCount+1), &u8Remainder);
        u4FrameCount ++;

        if(u4TimeCurr < 17)
        {
            //u4Delay16ms++;
        }
        else if(u4TimeCurr < 34)
        {
            //u4Delay33ms++;
        }
        else if(u4TimeCurr < 49)
        {
            //u4Delay48ms++;
        }
        else if(u4TimeCurr < 67)
        {
            //u4Delay66ms++;
        }
        else if(u4TimeCurr < 100)
        {
            //u4Delay99ms++;
        }
        else
        {
            //u4Delayxms++;
        }
        
        if(u4FrameCount > 300)
        {
            u8TimeCount100 += u4TimeCurr;
            u4TimeAver100 = u8Div6432(u8TimeCount100, (u4FrameCount- 300),&u8Remainder);
        }
        
        TMS_DIFF_EX(TMS_FLAG_WFD_LATENCY, "WFD_DATA:", "B2R Chg Frm");
        TMS_DIFF_EX(TMS_FLAG_WFD_LATENCY, "WFD_DATA", "B2R Chg Frm");
        LOG(4, "WFD_B2R_TMS: FbId(%d) PTS(0x%08x).\n", prFrcPrm->ucFbId, prFrcPrm->u4Pts);
        prPicHdr->fgWFDTMSLogEnable = FALSE;
        VDP_Pattern(prFrcPrm->ucB2rId, 97, 0, 0);
        fgPatternOn = TRUE;
    } 
    else if (fgPatternOn)
    {
        VDP_Pattern(prFrcPrm->ucB2rId, 98, 0, 0);
        fgPatternOn = FALSE;
    }
#ifdef __KERNEL__
    if(prPicHdr && prPicHdr->u8_vid_pts)
    {                                
        do_gettimeofday(&tv);
        
        u8CurTime = (UINT64)tv.tv_sec * 1000000;                
        u8CurTime = u8CurTime + (UINT64)tv.tv_usec;
        u8CurTime = u8Div6432(u8CurTime, 1000, &u8Remainder);
        //u8Totaltime = u8CurTime - prPicHdr->u8_push_time;
        LOG(2, "[Tao][B2R]PTS(0x%llx),Time[%llu],CurTime[%llu],CVPTS[0x%x]\n",
        prPicHdr->u8_vid_pts,
        prPicHdr->u8_push_time,
        u8CurTime,
        //u8Totaltime, 
        prFrcPrm->u4Pts);
        
        prPicHdr->u8_vid_pts = 0;
        prPicHdr->u8_push_time = 0;
    }
#endif
}
#endif
#endif

INT32 _B2R_Height_Align(UINT32*  pu4_height)
{
    INT32   i4Ret = B2R_OK;

    do
    {
        if(!pu4_height)
        {
            i4Ret = B2R_INV_ARG;
            break;
        }

        if(!HEIGHT_MUST_ALIGN(*pu4_height))
        {
            i4Ret = B2R_FAIL;
            break;
        }

        switch(*pu4_height)
        {
            case FHD_HEIGHT:
                *pu4_height = FHD_HEIGHT - FHD_ALIGN_VALUE;
                break;

            default :
                i4Ret = B2R_NOT_IMPL;
                break;
        }
    }while(0);

    return i4Ret;
}

static INT32 _B2R_OneFbChgHandle(B2R_PRM_T *prFrcPrm)
{
    UCHAR ucVdpId = 0;

    if (prFrcPrm == NULL)
    {
        LOG(0, "(%s) prFrcPrm is NULL!\n", __FUNCTION__);
        return -1;
    }

    if (FBM_GetFrameBufferStatus(prFrcPrm->ucFbgId, 0) != FBM_FB_STATUS_LOCK)
    {
        prFrcPrm->rOneFbChg.u4Count = 0;
        return -2;
    }

    prFrcPrm->rOneFbChg.u4Count++;
    ucVdpId = VDP_GetVdpId(prFrcPrm->ucB2rId);

    if (prFrcPrm->rOneFbChg.fgTrigger && (prFrcPrm->rOneFbChg.u4Count >= 3))
    {
        LOG(0, "(%s) Receive ReuseTrigger, @count(%u)\n", __FUNCTION__, prFrcPrm->rOneFbChg.u4Count);
        //VDP_SetFreezeEx(ucVdpId, 1);
		LOG(0, "(%s) Set VDP freeze, @count(%u)!\n", __FUNCTION__, prFrcPrm->rOneFbChg.u4Count);
		prFrcPrm->rOneFbChg.fgReusing = 1; 
        _VdpSetFrameBufferStatus(prFrcPrm, prFrcPrm->ucFbgId, 0, FBM_FB_STATUS_EMPTY);
        prFrcPrm->rOneFbChg.fgTrigger = 0;

        return 0;
    }
    else if (prFrcPrm->rOneFbChg.fgReusing && (prFrcPrm->rOneFbChg.u4Count >= 1))
    {
        //VDP_SetFreezeEx(ucVdpId, 0);
		LOG(0, "(%s) Set VDP unfreeze, @count(%u)!\n", __FUNCTION__, prFrcPrm->rOneFbChg.u4Count);
		prFrcPrm->rOneFbChg.fgReusing = 0;

        return 1;
    }
	UNUSED(ucVdpId);
    return -3;
}

BOOL B2R_OneFbChgTrigger(UCHAR ucVdpId)
{
    UCHAR ucB2rId = 0;
    B2R_PRM_T * prFrcPrm = NULL;
    
    if(ucVdpId >= VDP_NS)
    {
        LOG(0, "(%s) ucVdpId >= VDP_NS oops!\n", __FUNCTION__);
        return FALSE;
    }
    
    ucB2rId = VDP2B2RID(ucVdpId);
    if (ucB2rId >= B2R_NS)
    {
        LOG(0, "(%s) ucB2RId >= B2R_NS oops!\n", __FUNCTION__);
        return FALSE;
    }

    prFrcPrm = &_arVdpPrm[ucB2rId];

    if (FBM_GetFrameBufferNs(prFrcPrm->ucFbgId) != 1)
    {
        LOG(0, "(%s) There is has %u FB now!\n", __FUNCTION__,
                FBM_GetFrameBufferNs(prFrcPrm->ucFbgId));
        return TRUE;
    } 

    if (prFrcPrm->rOneFbChg.fgTrigger || prFrcPrm->rOneFbChg.fgReusing)
    {
        LOG(0, "(%s) Working(%u/%u), FbNR(%u)!\n", __FUNCTION__,
                prFrcPrm->rOneFbChg.fgTrigger,
                prFrcPrm->rOneFbChg.fgReusing,
                FBM_GetFrameBufferNs(prFrcPrm->ucFbgId));
        return FALSE;
    }

    if (FBM_GetFrameBufferStatus(prFrcPrm->ucFbgId, 0) == FBM_FB_STATUS_EMPTY)
    {
        LOG(0, "(%s) FB has be empty now!\n", __FUNCTION__);
        return TRUE;
    }
    
    prFrcPrm->rOneFbChg.fgTrigger = TRUE;
    LOG(0, "(%s) Trigger on\n", __FUNCTION__);

    return TRUE;
}

