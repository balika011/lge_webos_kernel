/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2005, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: drv_ttd.h $
 * $Revision: #1 $
 *
 *----------------------------------------------------------------------------*/

#ifndef _DRV_TTD_H_
#define _DRV_TTD_H_

/*----------------------------------------------------------------------------*
 * Header Files
 *----------------------------------------------------------------------------*/
#include "typedef.h"
#include "video_def.h"

/*----------------------------------------------------------------------------*
 * Definitions
 *----------------------------------------------------------------------------*/
#define SUPPORT_BLACK_SINGLE_MASK 1
#define DISPARITY_SCALAR_TTD 64
#define DISPARITY_SCALAR_TDDC 0x200
#define CONVERGENCE_DEPTH_3D 192
#define DEPTH_RANGE_RATIO_3D 0
#define DISPARITY_GAIN_3D 0
#define MAX_TTD_RST_COUNTER 3 
#define DIBR_DELAYOUT 250
#define DIBR_DELAYOUT_PR 245 // MT5368 + PR = 100, MT8283 + PR = 400
#define VP_LETTER_BOX_HIST_TH 200
#define STEREO_COMFORT_FACTOR 40 // Set this value < parallax (for children = 50 mm)
#define TTD_BLACKBAR_BUF_SIZE 8

#define UI_DSPT_SCALAR       0x1
#define UI_GLOBAL_SFT        0x2
#define UI_SW_GLOBAL         0x4
#define UI_SW_LOCAL          0x8
#define UI_SW_GLOBAL_SFT     0x10
#define UI_BLACK_BAR         0x20

#define IS_ECO_IC(void)     (IS_IC_5881()&&(BSP_GetIcVersion()>IC_VER_5881_AA))

typedef enum{
    SRC_3D_TYPE_FS=0,
    SRC_3D_TYPE_SBS=1,
    SRC_3D_TYPE_TAB=2,
    SRC_3D_TYPE_LI=3,
}E_SRC_3D_TYPE;

typedef enum{
    TTD_CHECK_INV_V1 = 0,
    TTD_CHECK_INV_V2,
    TTD_CHECK_INV_MAX
}E_TTD_CHECK_INV_VER;

typedef struct
{
    UINT16 ttdInputWidth;
    UINT16 ttdInputHeight;
    UINT8 ttdFwStatus;
    UINT8 tddcFwStatus;
    UINT8 ttdConvergenceDepth;
    UINT8 ttdGlobalWeight;
    UINT8 ttdLocalWeight;
    UINT8 ttdFgLocalInverse;
    UINT32 ttdPreAPL;
    UINT32 ttdCurAPL;
    UINT32 ttdPreDiver;
    UINT32 ttdCurDiver;
    UINT32 ttdSceneChangeThr;
    UINT16 ttdHist[32];
    UINT16 ttdHueHist[8];
    INT32 max_index, hist_max, max_index_sec, hist_max_sec;
}TTD_INFO;


/*----------------------------------------------------------------------------*
 * Global/Static variables
 *----------------------------------------------------------------------------*/
EXTERN UINT8 g_u1ParameterChange;
EXTERN UINT8 g_u1SupportDepthCtrl;
EXTERN UINT8 g_u1Input3D;
EXTERN UINT8 g_u1DepthOfField;
EXTERN UINT8 g_u1ProtrudeFromScreen;
EXTERN INT32 g_i4dLimMax;
EXTERN INT32 g_i4dLimMin;

EXTERN INT32 g_i4PosLeftPrev;
EXTERN INT32 g_i4PosRightPrev;
EXTERN TTD_INFO gTTDInfo;


/*----------------------------------------------------------------------------*
 * Function Members
 *----------------------------------------------------------------------------*/
void vDrvTTDInit(void);
void vDrvTTDSetDepthControl(UINT8 u1OnOff);
void vDrvTTDSet3DInput(void);
void vDrvTTDSet2DInput(void);
void vDrvTTDSet3DDepthCtrl(UINT8 u1OnOff);
void vDrvTTDSetEnable(UINT8 bEnable);
void vDrvTTDSetBlackSingleInv(UINT8 u1OnOff);
void vDrvTTDSetQualityISR(void);
void vDrvTTDPicChgCheck(void);
void vDrvTTDProc(void);
void vDrvTTDSetFWOnOff(UINT8 bOnOff);
void vDrvTDDCSetFWOnOff(UINT8 bOnOff);
void vDrvTTDCheckRstCtrl(void);
void vDrvTTDSetStereoComfortRange(UINT8 u1Factor);
void vDrvTTDSetImageSafetyLevel(UINT8 u1Level);
void vDrvTTDSetDepthOfField(UINT8 u1Depth);
void vDrvTTDSetProtrudeFromScreen(UINT8 u1Protrude);
void vDrvTTDSetDistanceToTV(UINT8 u1Distance);
void vDrvTTDSetLREyeInverse(UINT8 u1OnOff);
void vDrvTTDSetShutterLREyeInverse(UINT8 u1OnOff);
void vDrvTTDCalculateSafetyScale(void);
void vDrvTTDUiValueUpdate(void);
void vDrvTTDSetUiProc(void);
void vDrvTTDSetConvergenceDepthUI(UINT8 depth);
void vDrvTTDSetLRSyncSwitch(UINT8 bEnable);
void vDrvTTDSetLRDataSwitch(UINT8 bEnable);
void vDrvTTDSetGlobalShift(UINT32 u4Shift);
void vDrvTTDSetV3GlobalShift(INT8 i1Shift);
void vDrvTTDSetNewGlobalShift(UINT8 u1Shift);
void vDrvTTDSetPrInfo(UINT8 u1OnOff);
void vDrvTTDSetDepthBias(UINT8 u1Value);
void vDrvTTDQualityInit(void);
void vDrvTTDSetBlackBar(UINT32 u4BlackFromTTD, UINT32 u4BlackFromGbSft, UINT32 u4BlackSingleInv);
void vDrvTTDSetBlackEn(UINT8 u1OnOff);
void vDrvTTDSetBlackNum(UINT32 u4ValueL,UINT32 u4ValueR);
void vDrvTTDSetBlackSingleEn(UINT8 u1OnOff);
void vDrvTTDSetBlackSingleNum(UINT8 u1ValueL,UINT8 u1ValueR);
void vDrvTTDLSVInit(void);
void vDrvTTDLSVProc(void);
void vDrvTTDSet3DDepthCtrl(UINT8 u1OnOff);
void vDrvTTDSuspend(void);
void vDrvTTDResume(void);
void vDrvTTDSetDisptScalar(UINT32 u4Scalar);
void vDrvTTDSetDsprtMin(UINT8 u1Value);
void vDrvTTDSetDsprtMax(UINT8 u1Value);
void vDrvTTDSetTfuBase(UINT32 u4Value);
void vDrvTTDSetGlobalModel(void);
void vDrvTTDSetBlackBarMainLoop(void);
void vDrvTTDCheckTransition(void);
void vDrvTTDSetGlobalWeight(UINT8 u1Value);
void vDrvTTDSetTfuWeight(UINT8 u1Value);
void vDrvTTDSetPQWindow(UINT16 u2Width,UINT16 u2Height);
UINT8 u1DrvTTDGet3DDepthCtrl(void);
UINT32 u4DrvGetArrayMaxValue(UINT32 u4Array[], UINT32 u4Size);
 
#endif //#ifndef _DRV_TTD_H_
