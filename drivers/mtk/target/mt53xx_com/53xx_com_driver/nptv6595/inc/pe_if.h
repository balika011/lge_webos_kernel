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
 * $RCSfile: pe_if.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file nptv_if.h
 *  NPTV video path control, include  input source select ,
 *  timing information, and video quality control
 */

#ifndef _PE_IF_H_
#define _PE_IF_H_

#include "drv_hdmi.h"
#include "util.h"
#include "drv_vga.h"
#include "drv_tvd.h"
#include "api_notify.h"
#include "u_drv_cust.h"
#include "drv_ycproc.h"
// Quality Item and Enumation
#include "pe_table.h"

#define PQ_OK                   0  ///< Success
#define PQ_NOT_OK              -1  ///< Not ok
#define PQ_NOT_SUPPORTED       -2  ///< Not support
#define PQ_PARAMETER_ERROR     -3  ///< Parameter error
#define PQ_DONT_CARE           -4  ///< Don't care
#define PQ_ERR_INV             -5  ///< Invalid argument passed
#define PQ_ERR_TIMEDOUT        -6  ///< Timeout occured
#define PQ_PARAMETER_NOCHANGE  -7  /// Paramenter no change

#define MJC_CTRL_EFCT_OFF		0x00            // MJC 1W1R (bypass)
#define MJC_CTRL_EFCT_LOW		0x40
#define MJC_CTRL_EFCT_MID		0x80
#define MJC_CTRL_EFCT_HIGH		0xff
#define MJC_CTRL_EFCT_MSK		0xff

#define EXTMJC_SW_TO_FRONT (IO32ReadFldAlign(PEUI_00, PEUI_EXTMJC_SWITCH_TO_FRONT) == 1)
#define GAMMA_IN_DRAM (IO32ReadFldAlign(PEUI_00, PEUI_INIT_GAMMA_TBL_IN_DRAM) == 1)
#define SET_GAMMA_IN_DRAM(u1OnOff) (vIO32WriteFldAlign(PEUI_00, (u1OnOff)?1:0, PEUI_INIT_GAMMA_TBL_IN_DRAM))

enum
{
    MJC_CTRL_INTP_OFF,
    MJC_CTRL_INTP_HFBK,
    MJC_CTRL_INTP_NORM
};

typedef enum
{
    VDP_ARG_MJC_EFFECT_OFF,
    VDP_ARG_MJC_EFFECT_LOW,
    VDP_ARG_MJC_EFFECT_MIDDLE,
    VDP_ARG_MJC_EFFECT_HIGH
} MJC_EFFECT_MODE;

typedef enum
{
    VDP_ARG_MJC_MODE_NORM,
    VDP_ARG_MJC_MODE_NO_DEBLUR,
    VDP_ARG_MJC_MODE_NO_FILM
} MJC_DEJUDDER_MODE;

typedef enum
{
    VDP_ARG_MJC_DEMO_ALL,
    VDP_ARG_MJC_DEMO_RIGHT,     // Enable Right, disable left
    VDP_ARG_MJC_DEMO_LEFT       // Enable left, disable right
} MJC_DEMO_MODE;


#ifdef DRV_SUPPORT_EXTMJC

EXTERN INT32 d_custom_extmjc_set_ui(UINT8  ui2_level,
                                          UINT8  ui2_mode,
                                          UINT8  ui2_demo,
                                          UINT8* pui1_msg_error);

#define vMJCSetDemoMode(A, B, C)        d_custom_extmjc_set_ui(A, B, C, NULL)

#endif

enum
{
    PEQTY_APPLY_TIME_OUTPUT_VSYNC_ISR,
    PEQTY_APPLY_TIME_INPUT_VSYNC_ISR,        
    PEQTY_APPLY_TIME_MJC_INPUT_VSYNC_ISR,        
    PEQTY_APPLY_TIME_MJC_OUTPUT_VSYNC_ISR,
};

//-----------------------------------------------------------------------------
//function Declare
//-----------------------------------------------------------------------------
/**
 * 3D NR On/Off
 *
 * @param bOnOff SV_ON/SV_OFF
 * @return NONE
 * @retval void
 */
void vApiVideo3DNROnOff(UINT8 bOnOff);

/**
 * TIIR NR On/Off
 *
 * @param bOnOff SV_ON/SV_OFF
 * @return NONE
 * @retval void
 */
void vApiVideoNRTIIROnOff(UINT8 bOnOff);

/**
 * TIIR NR YC Select
 *
 * @param bOnOff SV_ON/SV_OFF
 * @return NONE
 * @retval void
 */
void vApiVideoSRMNRYCOnOff(UINT8 bYOnOff, UINT8 bCOnOff);

/**
 * 2D NR On/Off
 *
 * @param bOnOff SV_ON/SV_OFF
 * @return NONE
 * @retval void
 */
void vApiVideo2DNROnOff(UINT8 bOnOff);

/**
 * MPEG NR On/Off
 *
 * @param bOnOff SV_ON/SV_OFF
 * @return NONE
 * @retval void
 */
void vApiVideoMPEGNROnOff(UINT8 bOnOff);

/**
 * PE initialize
 *
 * @param NONE
 * @return NONE
 * @retval void
 */
void vApiVideoPeInit(void);

/**
 * @brief Set the default value for MDY_QTY_TBL.
 * @param u1QtyIndex
 * @param u1QtyItem QUALITY_xxx
 * @param u1Value default value
 */
UINT8 bApiSetQtyDft(UINT8 u1QtyIndex, UINT32 u4UiItem, UINT16 u2Value);

/**
 * @brief Set the default value for MDY_QTY_TBL table.
 */
UINT8 bApiSetQtyTblDft(void);

/**
 * @brief Mapping from UI_min, UI_max, UI_dft, UI_cur, HW_min, HW_max, HW_dft, to HW cur.
 * @param u1VidPath SV_VP_MAIN/SV_VP_PIP
 * @param u2QtyItem VDP_ARG_xxx
 * @param pi2Min, pi2Max, pi2Dft, pucCur: UI values.
 */
UINT16 bApiQtyMapping(UINT8 u1QtyIndex, UINT16 u2QtyItem,
                     INT16 const i2Min, INT16 const i2Max,
                     INT16 const i2Dft, INT16 const i2Cur);

/**
 * @brief Parse the bCustomQtyItem[] table.
 */
UINT8 bApiParseCustomQtyItem(void);

/**
 * @brief Translate source type/timing index into customer defined index.
 * @param bSrcTypTmg SOURCE_TYPE_TIMING_RFTV ~ SOURCE_TYPE_TIMING_MAX
 *
 */
UINT8 bApiGetCustomSourceTypeTiming(UINT8 bSrcTypTmg);

/**
 * @brief Search the default value from DefaultQtyTbl or CustomQtyTbl.
 * @param wSearchIndex
 * @param bQtyIdx
 */
UINT8 bApiGetQtyDft(UINT16 wSearchItem, UINT8 bQtyIdx);

/**
 * @brief Search the min value from DefaultQtyTbl.
 * @param wSearchIndex
 */
UINT8 bApiGetQtyMin(UINT16 u2QtyItem);

/**
 * @brief Search the min value from DefaultQtyTbl.
 * @param wSearchIndex
 */
UINT8 bApiGetQtyMax(UINT16 u2QtyItem);

/**
 * @brief Register video process event, wait for apply to hardware in output v-sync.
 * @param bVdpArg VDP_ARG_xx
 * @param bPath <SV_VP_MAIN/SV_VP_PIP>
 * @param bOnOff <SV_ON/SV_OFF>
 */
void vApiRegisterVideoEvent(UINT8 bVdpArg, UINT8 bPath, UINT8 bOnOff);

/**
 * @brief Register video process event, wait for apply to hardware in specified time.
 * @param bVdpArg VDP_ARG_xx
 * @param bPath <SV_VP_MAIN/SV_VP_PIP>
 * @param bOnOff <SV_ON/SV_OFF>
 * @param bApplyTime <PEQTY_APPLY_TIME_OUTPUT_VSYNC_ISR/PEQTY_APPLY_TIME_INPUT_VSYNC_ISR/PEQTY_APPLY_TIME_MJC_INPUT_VSYNC_ISR/PEQTY_APPLY_TIME_MJC_OUTPUT_VSYNC_ISR>
 */
void vApiRegisterVideoEventByApplyTime(UINT8 bVdpArg, UINT8 bPath, UINT8 bOnOff, UINT8 bApplyTime);

/**
 * @brief Call corresponding video API in bRegisterList
 *
 */
void vApiApplyVideoEventOutputVSync(void);

/**
 * @brief Call corresponding video API in bRegisterList in MJC input Vsync isr
 *
 */
void vApiApplyVideoEventMJCInputVSync(void);

/**
 * @brief Call corresponding video API in bRegisterList
 *
 */
void vApiApplyVideoEventInputVSync(void);

/**
 * @brief Set Demo mode
 * @param bmode VDP_DEMO_ALL_OFF/VDP_DEMO_ALL_ON/VDP_DEMO_ON_LEFT/VDP_DEMO_ON_RIGHT/
 * @param u1SrcTypTmg source type and timing
 */
void vApiVideoDemoMode(UINT8 bmode, Region DemoRec, UINT8 bDEMOModules);

/**
 * @brief Verify if FlashPQ header is valid or not.
 * @return SV_TRUE/SV_FALSE
 */
UINT8 bApiFlashPqVerifyHeader(void);

void vApiFlashPqReadFlash(UINT64 u8Offset, UINT32 u4MemPtr, UINT32 u4MemLen);

/**
 * @brief Get PQ version
 * @return SV_TRUE/SV_FALSE
 */
UINT8 * bApiFlashPqGetVersion(UINT8 bType);

void vApiFlashPqSetGammaId(UINT8 bIndex);
void vApiFlashPqSetSmartPicId(UINT8 bIndex);
void vApiFlashPqSetSceId(UINT8 bIndex);
void vApiFlashPqSetOdId(UINT8 bIndex);

UINT16 vApiFlashPqGetGammaId(void);
UINT16 vApiFlashPqGetSceId(void);
UINT16 vApiFlashPqGetMinSceId(void);
UINT16 vApiFlashPqGetQtyId(void);
UINT8 vApiFlashPqGetSmartPicId(void);
UINT16 vApiFlashPqGetOdId(void);

UINT8 bApiFlashPqUpdateGamma(void);
UINT8 bApiFlashPqUpdateSCE(void);
UINT8 bApiFlashPqUpdateQtyDft(void);
UINT8 bApiFlashPqUpdateQtyMinMax(UINT8 u1VidPath);
UINT8 bApiFlashPqUpdateOD(void);

UINT32 PE_GetUiRange(UINT8 u1VidPath, PE_ARG_TYPE ucArgType, PE_UI_RANGE_T *prUiRange);
void PE_SetUiRange(UINT8 u1VdpVisType, PE_ARG_TYPE u1UiVqItem, PE_UI_RANGE_T *prUiRange);
void PE_SetAllUiVqItemDftValues(void);
void PE_SetUiMinMaxDft(void);

/**
 * VDP set video plane argument.
 *
 * @param ucVdpId specify the video plane id.
 * @param ucArg specify the color item
 * @param i2Value specify the value of the color item
 * @return VDP_SET_ERROR or VDP_SET_OK
 */
UINT32 PE_SetArg(UCHAR ucVdpId, PE_ARG_TYPE ucArg, INT16 i2Value);

/**
 * VDP query video plane argument.
 *
 * @param ucVdpId specify the video plane id.
 * @param ucArg
 * @param pi2Value
 * @return VDP_SET_ERROR or VDP_SET_OK.
 */
UINT32 PE_GetArg(UCHAR ucVdpId, PE_ARG_TYPE ucArg, INT16* pi2Value);

/**
 * @brief Interface between MW_if and NPTV api
 * @param u1VidPath SV_VP_MAIN/SV_VP_PIP
 * @param u1QtyItem VDP_ARG_xxx
 */
UINT8 bApiVideoProc(UINT8 u1VidPath, PE_ARG_TYPE u1UiItem);

/**
 * VDP set pq info interface.
 *
 * @param ucVdpId specify the video plane id.
 * @param u4PqType specify the PQ type to set
 * @param pkvPqSetInfo specify the data of the PQ type
 * @param u4Size specify the size of data
 * @return VDP_SET_ERROR or VDP_SET_OK
 */
INT32 PE_SetPqInfo(UCHAR ucVdpId, UINT32 u4PqType, const void* pkvPqSetInfo, UINT32 u4Size);

/**
 * VDP get pq info interface.
 *
 * @param ucVdpId specify the video plane id.
 * @param u4PqType specify the PQ type to get
 * @param pkvPqGetInfo specify the location for put the retrieved data
 * @param u4Size specify the size of data
 * @return VDP_SET_ERROR or VDP_SET_OK
 */
INT32 PE_GetPqInfo(UCHAR ucVdpId, UINT32 u4PqType, void* pkvPqGetInfo, UINT32 u4Size);

/**
 * @brief Auto Backlight update
 * @param aBLight auto Backlight structure
 */
void DRVCUST_AutoBacklightUpdate(AUTO_BACKLIGHT_SETS_INFO_T *aBLight);

/**
 * @brief Clarity update
 * @param u1Value -5~+5
 */
void DRVCUST_ClarityUpdate(INT8 u1Value);


/* @ Auto NR default Initial setting
 *      1: low
 *      2: Middle
 */
UINT8 DRVCUST_AutoNRDefSet(void);

/* @ Auto NR noise level implement
 * @ return noise level
 */
UINT8 DRVCUST_NoiseLevel(void);

void DRVCUST_UpdateBacklightState(UINT8 u1State, UINT16 u2Duty);
void DRVCUST_SetBacklightDuty(UINT16 u2Duty);
void DRVCUST_AutoColorTempUpdate(AUTO_COLORTEMP_SETS_INFO_T *aCTInfo);
void DRVCUST_PANEL_GAMMA_REMAP(UINT32 u4GammaSel);
void DRVCUST_SET_GAMMA(void);
void DRVCUST_CalculateSCE(UINT8 bSceLumaGain, UINT8 bSceSatGain, UINT8 bSceHueGain);
void DRVCUST_AdaptiveBacklightMode(UINT8 bMode);
void DRVCUST_AdaptiveBacklightParam(UINT8 bMode);
UINT32 DRVCUST_RGBOfstRemap(UINT32 u4Value);
UINT8 bApiGetPeUiDebugMode(void);
void vApiSetPeUiDebugMode(UINT8 bOnOff);

// Gamma Delta Customized Prototype
UINT16 DRVCUST_GetGammaVal(UINT8 bMode, UINT16 wLevel);
UINT16 DRVCUST_GetCompGammaVal(UINT8 bMode, UINT16 wLevel);
void DRVCUST_SetGammaDelta(UINT8 bMode, INT8 iDelta1, INT8 iDelta2, INT8 iDelta3, INT8 iDelta4);
void DRVCUST_SetGammaLevel(UINT16 wP1, UINT16 wP2, UINT16 wP3, UINT16 wP4);

// OD Customized Prototype
void DRVCUST_ODTableSel(UINT8 TableSel);

/*****************************************************************************************/
/*********************************      TDES     *****************************************/
/*****************************************************************************************/
/**
 * TDES On/Off
 *
 * @bPath SV_VP_MAIN / SV_VP_PIP
 * @param bOnOff SV_ON/SV_OFF
 * @return NONE
 * @retval void
 */
void vHalVideoTDESOnOff(UINT8 bPath, UINT8 bOnOff);

/*****************************************************************************************/
/*********************************  HSHARP FRONT *****************************************/
/*****************************************************************************************/
/**
 * Sharpness On/Off
 *
 * @bPath SV_VP_MAIN / SV_VP_PIP
 * @param bOnOff SV_ON/SV_OFF
 * @return NONE
 * @retval void
 */
void vHalVideoSharpnessOnOff(UINT8 bPath, UINT8 bOnOff);
void vHalVideoSharpnessHorizontalOnOff(UINT8 bPath, UINT8 bOnOff);
void vHalVideoSharpnessVerticalOnOff(UINT8 bPath, UINT8 bOnOff);


/**
 * LTI On/Off
 *
 * @bPath SV_VP_MAIN / SV_VP_PIP
 * @param bOnOff SV_ON/SV_OFF
 * @return NONE
 * @retval void
 */
void vHalVideoLTIOnOff(UINT8 u1VidPath, UINT8 bOnOff);

void vHalVideoLTIParam(UINT8 u1VidPath);

/**
 * Sharpness Control
 * u1Qty[QUALITY_SHARP_BEGIN] ~ u1Qty[QUALITY_SHARP_END] must be set first
 * vHalVideoSharpnessParam will asign each ulQty value to related driver
 *
 * @bPath SV_VP_MAIN / SV_VP_PIP
 * @return NONE
 * @retval void
 */
void vHalVideoSharpnessParam(UINT8 bPath);
void vHalVideoSharpnessParamHorizontal(UINT8 bPath);
void vHalVideoSharpnessParamVertical(UINT8 bPath);

/**
 * Load Y Level Sharpness table
 *
 * @bPath SV_VP_MAIN / SV_VP_PIP
 * @param bTbl[65]
 * @return NONE
 * @retval void
 */
void vHalVideoYLevSharpness(UINT8 bPath, const UINT8* u1YLevSharp);

/**
 * LTI Control
 * u1Qty[QUALITY_LTI_BEGIN] ~ u1Qty[QUALITY_LTI_END] must be set first
 * vHalVideoLTIParam will asign each ulQty value to related driver
 *
 * @bPath SV_VP_MAIN / SV_VP_PIP
 * @return NONE
 * @retval void
 */
void vHalVideoLTIParam(UINT8 u1VidPath);

/*****************************************************************************************/
/********************************  ADAPTIVE LUMA    **************************************/
/*****************************************************************************************/
/**
 * Adaptive luma On/Off
 * Change contrast curve according to histogram
 *
 * @bPath SV_VP_MAIN / SV_VP_PIP
 * @param bOnOff SV_ON/SV_OFF
 * @return NONE
 * @retval void
 */
void vHalVideoAdaptiveLumaOnOff(UINT8 bPath, UINT8 bOnOff);

/**
 * Load adaptive luma parameters according to current input timing
 *
 * @return NONE
 * @retval void
 */
void vHalVideoAdaptiveLumaParam(void);

/*****************************************************************************************/
/********************************  LOCAL DIMMING    **************************************/
/*****************************************************************************************/
/**
 * @brief enable/disable local dimming
 * @param bPath SV_VP_MAIN/SV_VP_PIP
 * @param bOnOff SV_ON/SV_OFF
 */
void vHalVideoLcDimOnOff(UINT8 bPath, UINT8 bOnOff);

/*****************************************************************************************/
/***********************************   CONTRAST   ****************************************/
/*****************************************************************************************/
/**
 * Adjust Video contrast
 *
 * @param bPath SV_VP_MAIN/SV_VP_PIP
 * @param bValue Contrast value (0x00~0xff, defalut:0x80)
 * @return NONE
 * @retval void
 */
void vHalVideoContrast(UINT8 bPath, UINT16 u2Value);

/*****************************************************************************************/
/**********************************   BRIGHTNESS   ***************************************/
/*****************************************************************************************/
/**
 * Adjust Video brightness
 *
 * @param bPath SV_VP_MAIN/SV_VP_PIP
 * @param bValue Brightness value (0x00~0xff, defalut:0x80)
 * @return NONE
 * @retval void
 */
void vHalVideoBrightness(UINT8 bPath, UINT8 bValue);

/*****************************************************************************************/
/**********************************   SATURATION   ***************************************/
/*****************************************************************************************/
/**
 * Adjust Video saturation
 *
 * @param bPath SV_VP_MAIN/SV_VP_PIP
 * @param bValue Saturation value (0x00~0xff, defalut:0x80)
 * @return NONE
 * @retval void
 */
void vHalVideoSaturation(UINT8 bPath, UINT16 u2Value);

/*****************************************************************************************/
/*************************************   HUE   *******************************************/
/*****************************************************************************************/
/**
 * Adjust Video Hue
 *
 * @param bPath SV_VP_MAIN/SV_VP_PIP
 * @param bValue Hue value (0x00~0x40, defalut:0x20)
 * @return NONE
 * @retval void
 */
void vHalVideoHue(UINT8 bPath, UINT8 bValue);

/*****************************************************************************************/
/*************************************   SCE   *******************************************/
/*****************************************************************************************/
/**
 * @brief Load SCE table by function index and hue index
 * @param u4FunIdx function index from 0~21
 * @param u4HueIdx hue index from 0~27
 * @param u4Value
 */
UINT32 vApiSCELoadIdx(UINT32 u4FunIdx, UINT32 u4HueIdx, UINT32 u4Value);

/**
 * SCE On/Off
 *
 * @param bPath SV_VP_MAIN/SV_VP_PIP
 * @param bOnOff SV_ON/SV_OFF
 * @return NONE
 * @retval void
 */
void vHalVideoSCEOnOff(UINT8 bPath, UINT8 bOnOff);

/**
 * select SCE table to load
 *
 * @param bPath SV_VP_MAIN/SV_VP_PIP
 * @param bValue which SCE table to load? usually use 1, 0: do not load table
 * @return NONE
 * @retval void
 */
void vHalVideoSCELoadTable(UINT8 bPath);

/**
 * @brief Set SCE pattern
 * @param bOnOff SV_ON/SV_OFF
 * @param wYdata SCE pattern Y level
 * @return NONE
 * @retval void
 */
void vHalSetSCEPat(UINT8 bOnOff, UINT16 wYdata);

/*****************************************************************************************/
/************************************   CTI  *******************************************/
/*****************************************************************************************/
/**
 * CTI On/Off
 *
 * @param bPath SV_VP_MAIN/SV_VP_PIP
 * @param bOnOff SV_ON/SV_OFF
 * @return NONE
 * @retval void
 */
void vHalVideoCTIOnOff(UINT8 bPath, UINT8 bOnOff);

/**
 * CTI parameters control
 * u1Qty[QUALITY_CTI_BEGIN] ~ u1Qty[QUALITY_CTI_END] must be set first
 *
 * @param bPath SV_VP_MAIN/SV_VP_PIP
 * @return NONE
 * @retval void
 */
void vHalVideoCTIParam(UINT8 bPath);

/*****************************************************************************************/
/*******************************   RGB GAIN  & OFFSET ************************************/
/*****************************************************************************************/
/**
 * Set RGB Gain
 *
 * @param bPath SV_VP_MAIN / SV_VP_PIP
 * @param bRGB SV_RGB_R/SV_RGB_G/SV_RGB_B
 * @param wValue 0x00~0xff
 * @return NONE
 * @retval void
 */
void vHalVideoRGBGain(UINT8 bPath, UINT8 bRGB, UINT16 wValue);

/**
 * Set RGB offset
 *
 * @param bPath SV_VP_MAIN / SV_VP_PIP
 * @param bRGB SV_RGB_R/SV_RGB_G/SV_RGB_B
 * @param wValue 0x00~0xff, 2' complement, defalut = 0x80
 * @return NONE
 * @retval void
 */
void vHalVideoRGBOffset(UINT8 bPath, UINT8 bRGB, UINT16 wValue);

/**
 * Set RGB gain/offset at the same time from quality table
 *
 * @param bPath SV_VP_MAIN/SV_VP_PIP
 * @return NONE
 * @retval void
 */
void vHalVideoRGBGainOfstFromQtyTbl(UINT8 bPath);

/**
 * Set RGB Gain at the same time
 *
 * @param bPath SV_VP_MAIN / SV_VP_PIP
 * @param u2RGain 0x00~0x3ff, default 0x200
 * @param u2GGain 0x00~0x3ff, default 0x200
 * @param u2BGain 0x00~0x3ff, default 0x200
 * @return NONE
 * @retval void
 */
void vHalVideoSetRGBGain(UINT8 bPath, UINT16 u2RGain, UINT16 u2GGain, UINT16 u2BGain);

/**
 * Set RGB Offset 1 at the same time
 *
 * @param bPath SV_VP_MAIN / SV_VP_PIP
 * @param u2ROfst 0x00~0x3ff, default 0x200
 * @param u2GOfst 0x00~0x3ff, default 0x200
 * @param u2BOfst 0x00~0x3ff, default 0x200
 * @return NONE
 * @retval void
 */
void vHalVideoSetRGBOfst1(UINT8 bPath, UINT16 u2ROfst, UINT16 u2GOfst, UINT16 u2BOfst);

/**
 * Set RGB Offset 2 at the same time
 *
 * @param bPath SV_VP_MAIN / SV_VP_PIP
 * @param u2ROfst 0x00~0x3ff, default 0x200
 * @param u2GOfst 0x00~0x3ff, default 0x200
 * @param u2BOfst 0x00~0x3ff, default 0x200
 * @return NONE
 * @retval void
 */
void vHalVideoSetRGBOfst2(UINT8 bPath, UINT16 u2ROfst, UINT16 u2GOfst, UINT16 u2BOfst);

/**
 * Set xvYCC RGB Gain at the same time
 *
 * @param bPath SV_VP_MAIN / SV_VP_PIP
 * @param u2ROfst 0x00~0x3ff, default 0x200
 * @param u2GOfst 0x00~0x3ff, default 0x200
 * @param u2BOfst 0x00~0x3ff, default 0x200
 * @return NONE
 * @retval void
 */
void vHalVideoSetXVRGBGain(UINT8 bPath, const UINT16* u2RgbGain);
void vHalVideoSetXVRGBOffset1(UINT8 bPath, const UINT16* u2RgbOffset);
void vHalVideoSetXVRGBOffset2(UINT8 bPath, const UINT16* u2RgbOffset);
void vHalVideoSetXVYCC(UINT8 bPath, UINT8 u1OnOff, UINT8 u1GammaOnOff, const UINT16* u2Matrix);

/**************************************************************************************/
/***********************************Bypass All PQ Item****************************************/
/**************************************************************************************/
void vVideoAllPQItemOnOff(UINT8 bOnOff);

/*****************************************************************************************/
/************************************  Gamma  ********************************************/
/*****************************************************************************************/
/**
 * Load Gamma table
 *
 * @param bMode SV_V_GAMMA_OFF / SV_V_GAMMA_DARK / SV_V_GAMMA_MIDDLE / SV_V_GAMMA_BRIGHT
 * @return NONE
 * @retval void
 */
void vHalVideoGammaLoadTable(UINT8 bMode);
void vApiSetStaticGamma(UINT32 u4Index);
#ifndef CC_UP8032_ATV
void vHalVideoExternalGammaInit(void);
BOOL bHalVideoVerifyGammaTable(UINT8 bMode);
void vHalSetGammaSaveDelta(DRV_CUSTOM_GAMMA_DELTA_T *pv_set_info);
#endif

/*****************************************************************************************/
/************************************  Noise Reduction  **********************************/
/*****************************************************************************************/
void vHalVideoAutoNROnOff(UINT8 bOnOff);
void vHalVideo2DNRParam(UINT8 bCurUI);
void vHalVideoMPEGNRParam(UINT8 bCurUI);
void vHalVideo3DNRParam(void);
void vHalVideoIRRParam(void);
void vHalVideoCBEParam(void);

/**
 * Enable / Disable white peak limitation feature.
 * @param bOnOff SV_ON/SV_OFF
 */
void vHalVideoWhitePeakLimit(UINT8 bOnOff);

/*****************************************************************************************/
/************************************  Adaptive Backlight  *******************************/
/*****************************************************************************************/
void vHalVideoSetAdaptiveBacklightMode(UINT8 bMode);
void vHalVideoAdaptiveBackLightParam(UINT8 bMode);
void vHalSetAdpBacklight(UINT8 u1Level, UINT8 u1Value);
UINT8 bHalGetAdpBacklight(UINT8 u1Level);
void vHalVideoExtLevel(UINT8 u1VidPath, BOOL fgIsModeChange);
void vHalOutRGBColorOnOff(UINT8 bOnOff);
void vHalOutRGBColor(UINT16 RValue, UINT16 GValue, UINT16 BValue);

/*****************************************************************************************/
/********************************** ANGULAR SCE MODEL ************************************/
/*****************************************************************************************/
void vHalSCEAngularReset(void);
void vHalSetSCEAngular(UINT8 bmodule, UINT8 bColor, UINT8 bValue);

/*****************************************************************************************/
/********************************** BLUE STRETCH  ****************************************/
/*****************************************************************************************/
void vHalVideoBlueStretchSetDefault(void);
void vHalVideoBlueStretchOnOff(UINT8 bOnOff);

#endif /* _PE_IF_H_ */

