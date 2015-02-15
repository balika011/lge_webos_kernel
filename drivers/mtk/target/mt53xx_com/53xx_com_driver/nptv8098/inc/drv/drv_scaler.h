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
 * $RCSfile: drv_scaler.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file drv_scaler.h
 *  Brief of file drv_scaler.h.
 *  Details of file drv_scaler.h (optional).
 */

#ifndef _DRV_SCALER_H_
#define _DRV_SCALER_H_


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "x_typedef.h"
#include "vdp_drvif.h"
#include "debug.h"
#include "feature.h"
#include "video_def.h"
#include "drv_di.h"
#include "hw_scpos.h"
#include "vdo_rm.h"

//-----------------------------------------------------------------------------
// Workaround define
//-----------------------------------------------------------------------------
#if defined(CC_MT5398) || defined(CC_MT5880)
#define SCALER_WA1_DRAM_WIDTH
#define SCALER_WA2_PR_DOWN
#endif

#define DZONE_DETECTION_EN     //to detect frc_line and output vsync overlapping.
#if defined(DZONE_DETECTION_EN)
#define FRC_LINE_ADJ    6
#endif 
#if defined(SCALER_WA1_DRAM_WIDTH)
EXTERN UINT32 u4FixedWidth[VDP_NS];
#endif

#define SUPPORT_FHD_120

//-----------------------------------------------------------------------------
// Constant define
//-----------------------------------------------------------------------------
#define CAL_HDIRECT_FACTOR               (0)
#define CAL_VDIRECT_FACTOR               (1)
#define VDP_MDDI_RESCALE_TIME               (4)
#define VDP_SCPOS_RESCALE_TIME              (7)
#define VDP_SCPOS_IMPORT_PROTECT            (3)
#define VDP_SCPOS_WEN_WAIT                  (4)
#define VDP_SCPOS_TV3D_MUTE_DELAY           (5)
#define VDP_SCPOS_DELAY_UNMUTE_TIME        (10)    
#define VDP_DYNAMICE_SCALE_IDLE             (0)
#define VDP_DYNAMICE_SCALE_SCALING          (1)

#define PD_LOG_STATUS 0x01
#define RG_LOG_STATUS 0x02
#define SC_LOG_STATUS 0x04
#define PS_LOG_STATUS 0x08
#define FT_LOG_STATUS 0x10
#define MIB_LOG_STATUS 0x20
#define MJC_LOG_STATUS 0x40
#define XXX_LOG_STATUS 0x80

#define SCPOS_BYPASS_PSCANNR    0xffffffff

#define DOWN_SCALING_MODE 0
#define UP_SCALING_MODE 1

//poclk sel value define
#define FLD_POCLK_SEL_OCLK      0
#define FLD_POCLK_SEL_OCLK_DIV2 1
#define FLD_POCLK_SEL_DPLL      2
#define FLD_POCLK_SEL_MIB_OCLK  3

//oclk_tve sel value define
#define FLD_OCLK_TVE_SEL_OCLK      0
#define FLD_OCLK_TVE_SEL_OCLK_DIV2 1
#define FLD_OCLK_TVE_SEL_DPLL      2
#define FLD_OCLK_TVE_SEL_MIB_OCLK  3
#define FLD_OCLK_TVE_SEL_TVE27_CK  4

//poclk dpll sel value define
#define FLD_POCLK_DPLL_SEL_SYSPLL_DIV2 0
#define FLD_POCLK_DPLL_SEL_TAPLL_DIV2 1
//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

#define CC_SPEED_UP_DISPMODE_TUNE 1  
#define SCPIP_TOGGLE_IMPORT_AT_INPUTV

#define DRV_DISP_OFF_MODULE_SCART          (0x0)
#define DRV_DISP_OFF_MODULE_VENC           (0x1)
#define DRV_DISP_OFF_MODULE_VSS            (0x2)
#define DRV_DISP_OFF_MODULE_MAX            (0x3)

// TODO: remove
//#define MT5360B_WA4
#ifdef MT5360B_WA4
#define MT5360B_WA4_DLY1 4 // decoder + 4 pixels
#endif

// TODO: used in other files, move?
// vsync delay constant
#define VDP_VD_DISABLE              0
#define VDP_VD_TURN_DISABLE         1
#define VDP_VD_ENABLE               2
#define VDP_VD_TURN_ENABLE          3
#define VDP_VD_DELAY_COUNT          7

#if defined(CC_MT5398)
#define SUB_HMSK_DLY_Cobra      0xDB
#define SUB_VMSK_DLY      0x3
#else    //for 5880 and 5860
#define SUB_HMSK_DLY_Viper      0x1D5
#define SUB_HMSK_DLY_Python      0x1D7
#define SUB_VMSK_DLY      0x3
#endif

#define VDP_DS_PTN_OFF 0
#define VDP_DS_PTN_AUTO 1
#define VDP_DS_PTN_MANUAL 2

#define VDP_US_PTN_OFF 0
#define VDP_US_PTN_AUTO 1
#define VDP_US_PTN_MANUAL 2

#define SCPIP_121MAP_H_POS_LEFT 0
#define SCPIP_121MAP_H_POS_MID 1
#define SCPIP_121MAP_H_POS_RIGHT 2

#define getMixedHPorch(bPath) ((getPicInfo(bPath)->wXOffset + getPicInfo(bPath)->wUIHPorch >1000)? (getPicInfo(bPath)->wXOffset + getPicInfo(bPath)->wUIHPorch - 1000): 0)
#define getMixedVPorch(bPath) ((getPicInfo(bPath)->wYOffset + getPicInfo(bPath)->wUIVPorch >1000)? (getPicInfo(bPath)->wYOffset + getPicInfo(bPath)->wUIVPorch - 1000): 0)
#define vDrvMainUpdateVBound(u2Vline) (vIO32WriteFldAlign(SCPIP_US1_14, u2Vline, US1_14_UP_VBOUND_Y1))

#define IS_COBRA()      (BSP_GetIcVersion() >= IC_VER_5398_AA && BSP_GetIcVersion() <= IC_VER_5398_AC)
#define IS_COBRA_E()    (BSP_GetIcVersion() == IC_VER_5398_AA)
#define IS_COBRA_M()    (BSP_GetIcVersion() > IC_VER_5398_AA && BSP_GetIcVersion() <= IC_VER_5398_AC)

#define IS_VIPER()      (BSP_GetIcVersion() >= IC_VER_5880_AA && BSP_GetIcVersion() <= IC_VER_5880_AC)
#define IS_VIPER_E()    (BSP_GetIcVersion() == IC_VER_5880_AA)
#define IS_VIPER_M()    (BSP_GetIcVersion() > IC_VER_5880_AA && BSP_GetIcVersion() <= IC_VER_5880_AC)

#define IS_PYTHON()     (BSP_GetIcVersion() >= IC_VER_5860_AA && BSP_GetIcVersion() <= IC_VER_5860_AC)
#define IS_PYTHON_E()   (BSP_GetIcVersion() == IC_VER_5860_AA)
#define IS_PYTHON_M()   (BSP_GetIcVersion() > IC_VER_5860_AA && BSP_GetIcVersion() <= IC_VER_5860_AC)

//Main/Sub Offset
#define getPSCANOffset(bPath) ((bPath == SV_VP_MAIN)?0:0x800)
#define getPIPOffset(bPath) ((bPath == SV_VP_MAIN)?0:0x100)
#define getDSOffset(bPath) ((bPath == SV_VP_MAIN)?0:0x100)
#define getUSOffset(bPath) ((bPath == SV_VP_MAIN)?0:0x100)
#define getDramOffset(bPath) ((bPath == SV_VP_MAIN)?0:0x100)
#define getDSPatOffset(bPath) ((bPath == SV_VP_MAIN)?0:0x10)
#define getUSPatOffset(bPath) ((bPath == SV_VP_MAIN)?0:0x04)
#define getImportOffset(bPath)((bPath == SV_VP_MAIN)?0:0x08)
#define getPIPOAOffset(bPath) ((bPath == SV_VP_MAIN)?0:0x04)

// for 5398
#define getPDSOffset(bPath) ((bPath == SV_VP_MAIN)?0:0x200)
#define getRegenOffset(bPath)((bPath == SV_VP_MAIN)?0:0x200)
#define getCtrlOffset(bPath) ((bPath == SV_VP_MAIN)?0:0x200)
#define getMskOffset(bPath) ((bPath == SV_VP_MAIN)?0:0x8)
#define getSCOffset(bPath) ((bPath == SV_VP_MAIN)? 0:0x200)

#define getScalerPath(u4VdpId) ((u4VdpId==VDP_1)?SV_VP_MAIN:SV_VP_PIP)
#define getChannel(bPath)  ((bPath==SV_VP_MAIN)?&_rMChannel:&_rPChannel)
#define getPicInfo(bPath)   ((bPath==SV_VP_MAIN)?&_rMPicInfo:&_rPPicInfo)
#define getDispPrm(bPath)   ((bPath==SV_VP_MAIN)?&_rMDispPrm:&_rPDispPrm)
#define getDispInfo(bPath)   ((bPath==SV_VP_MAIN)?&_rMDispInfo:&_rPDispInfo)
#define getScalerVDP(bPath)  ((bPath==SV_VP_MAIN)?VDP_1:VDP_2)
#define fireSCPOSModeChange(VDP_ID) ((VDP_ID==VDP_1)?vSetScposFlg(SCALER_MAIN_MODE_CHG):vSetScposFlg(SCALER_PIP_MODE_CHG))
#define getScalerMode(bPath) (u1ScpipGetDispMode(bPath))
#define getScalerInfo(bPath)    (&_arScalePrm[bPath])
#define getDramAddrInfo(u4VdpId) (&_arScalePrm[u4VdpId].rDramAddrInfo)
#define getFbmPoolInfo(u4VdpId)  (&_arScalePrm[u4VdpId].rFbmInfo)
#define getTdtvInfo(u4VdpId)    (&_arScalePrm[u4VdpId].rTdtvInfo)

#define DFT_DS_FACTOR                       (0x8000)
#define DFT_US_FACTOR                       (0x8000)
#define DFT_HPD_FACTOR                      (0x8000)
#define DFT_DS_BIT_NUM                      (15)
#define DFT_US_BIT_NUM                      (15)

/*for 5398 FPGA*/
#define DFT_FRAC_PHASE_BITS_MAIN (20)
#define DFT_FRAC_PHASE_BITS_PIP  (15)
#define DFT_FRAC_PHASE_BITS(bPath) ((bPath==SV_VP_MAIN)?DFT_FRAC_PHASE_BITS_MAIN:DFT_FRAC_PHASE_BITS_PIP)
#define DFT_UNIT_PHASE(bPath)      (1<<DFT_FRAC_PHASE_BITS(bPath))
#define DFT_FRAC_PHASE_MASK(bPath) (DFT_UNIT_PHASE(bPath)-1)

#define DFT_PDS_FACTOR      (0x8000)

//#define GET_PDS_SIZE(inSize, factor)  ((inSize*factor + (DFT_PDS_FACTOR-factor)) / DFT_PDS_FACTOR)
#define GET_PDS_SIZE(inSize, factor)  ((inSize*factor + (DFT_PDS_FACTOR>>1)) / DFT_PDS_FACTOR)

#define DIVIDE_WITH_CARRIER(dividend, divisor)      (divisor==0? ((dividend) + ((divisor) - 1)) : (((dividend) + ((divisor) - 1)) / (divisor)))
#define DIVIDE_WITH_ROUND(dividend, divisor)        (divisor==0? ((dividend) + ((divisor) >> 1)) :(((dividend) + ((divisor) >> 1)) / (divisor)))
#define RSHIFT_WITH_CARRIER(dividend, shift)        (((dividend) + ((1 << (shift)) - 1)) >> (shift))
#define RSHIFT_WITH_ROUND(dividend, shift)        (((dividend) + ((1 << (shift))/2)) >> (shift))

#define CAL_DS_FACTOR(from, to)     ((from==to || from ==0) ? DFT_DS_FACTOR : ((to << DFT_DS_BIT_NUM) / from))
#define CAL_US_FACTOR(from, to)     ((from==to || to ==0) ? DFT_US_FACTOR : ((from << DFT_US_BIT_NUM) / to))

#define ALIGN_MASK(VAL, MSK) (((VAL) + (MSK)) & (~(MSK)))
#define ALIGN_2_CEIL(val) ALIGN_MASK((val), 1)
#define ALIGN_4_CEIL(val) ALIGN_MASK((val), 3)
#define ALIGN_8_CEIL(val) ALIGN_MASK((val), 7)
#define ALIGN_16_CEIL(val) ALIGN_MASK((val), 15)
#define ALIGN_32_CEIL(val) ALIGN_MASK((val), 31)
#define ALIGN_64_CEIL(val) ALIGN_MASK((val), 63)
#define ALIGN_256_CEIL(val) ALIGN_MASK((val), 255)

#if defined(CC_MT5398)
#define SUPPORT_TV3D_OVERSCAN
#endif

#define DEF_TO_STR(a) #a

#define NPTV_REG_PRINT(a, b) Printf("%10u | %-40s\n", IO32ReadFldAlign(a, b), #b)  
#define NPTV_REG_PRINT_HEX(a, b) Printf("0x%08x | %-40s\n", IO32ReadFldAlign(a, b), #b) 
#define NPTV_REG_LOG(x, a, b) LOG(x, "%10u | %-40s\n", IO32ReadFldAlign(a, b), #b)  
#define NPTV_REG_LOG_HEX(x, a, b) LOG(x, "0x%08x | %-40s\n", IO32ReadFldAlign(a, b), #b)

#define SCPIP_WRITE_FLD(reg,value,fld) vIO32WriteFldAlign(reg, value, fld)
#define SCPIP_READ_FLD(reg,fld) IO32ReadFldAlign(reg,fld)
typedef struct
{
    UINT16 wWidth;          //width after up-scaling
    UINT16 wHeight;         //height after up-scaling
    UINT16 wLeft;           //left overscan
    UINT16 wRight;          //right overscan
    UINT16 wTop;            //top overscan
    UINT16 wBottom;         //bottom overscan
    UINT16 wClipX;          //cliping on right for MJC
    UINT16 wClipY;          //cliping on bottom for MJC
    UINT16 wTargetWidth;    //real target width
    UINT16 wTargetHeight;   //read target height
}MJC_CLIP_INFO_T;

typedef enum {
    SCPIP_PTGEN_OFF = 0,
    SCPIP_PTGEN_AUTO,
    SCPIP_PTGEN_MANUAL,
} SCPIP_PTGEN_ONOFF;

typedef enum{
    SCALER_TDTV_IN_TYPE_2D,
    SCALER_TDTV_IN_TYPE_FS,
    SCALER_TDTV_IN_TYPE_SBS,
    SCALER_TDTV_IN_TYPE_TAB,
    SCALER_TDTV_IN_TYPE_LI,
}SCALER_TDTV_IN_TYPE;

typedef enum{
    SCALER_TDTV_OUT_TYPE_2D = 0,
    SCALER_TDTV_OUT_TYPE_PR = 1,
    SCALER_TDTV_OUT_TYPE_FPR = 2,
    SCALER_TDTV_OUT_TYPE_SBS = 3,
    SCALER_TDTV_OUT_TYPE_SG = 4,
    SCALER_TDTV_OUT_TYPE_3D_TO_2D = 5,
}SCALER_TDTV_OUT_TYPE;

typedef enum{
    SCALER_TDTV_RES_POSITION_PRE_H,
    SCALER_TDTV_RES_POSITION_PRE_V,
    SCALER_TDTV_RES_POSITION_PST_H,
    SCALER_TDTV_RES_POSITION_PST_V,
}SCALER_TDTV_RES_POSITION;

typedef enum{
    SCALER_DRAM_INIT_STATE_NOT_INIT,
    SCALER_DRAM_INIT_STATE_SRM_INITED,
    SCALER_DRAM_INIT_STATE_REG_INITED,    
} SCALER_DRAM_INIT_STATE;

typedef enum
{
    SCALER_FORCE_IMPORT_OFF,
    SCALER_FORCE_IMPORT_ON,
    SCALER_FORCE_IMPORT_UNKNOWN,
}SCALER_FORCE_IMPORT;

typedef struct      //calculated Dram Address info
{
    UINT32 FrameNum;
    UINT32 HMsbPageNum;
    UINT32 VMsbPageNum;
    UINT32 HLsbPageNum;
    UINT32 VLsbPageNum;
    UINT32 MsbPageNum;
    UINT32 LsbPageNum;
    UINT32 YMsbAddr;
    UINT32 YLsbAddr;
    UINT32 UMsbAddr;
    UINT32 ULsbAddr;
    UINT32 VMsbAddr;
    UINT32 VLsbAddr;
    UINT32 LowAddr;
    UINT32 HighAddr;
}SCALER_DRAM_ADDR_T;

typedef struct      //info from SRM
{
    UINT32 u4Addr;
    UINT32 u4Width;
    UINT32 u4Height;
    UINT32 u4MddiDramWidth;
    UINT32 u4FbmSize;
    BOOL fg2FB;
    BOOL fg10bit;
    BOOL fg8bit;
    BOOL fg422;
} SCALER_FBM_POOL_T;

typedef struct
{
    SCALER_TDTV_IN_TYPE eTdtvInType;
    SCALER_TDTV_OUT_TYPE eTdtvOutType;    
    E_TDTV_RES_OUTPUT ePreH;
    E_TDTV_RES_OUTPUT ePreV;
    E_TDTV_RES_OUTPUT ePstH;
    E_TDTV_RES_OUTPUT ePstV;    
    UINT32 u4IsPRUp;
    UINT32 u4IsDisprFreerun;
}SCALER_TDTV_PRM_T;

typedef struct
{
    UINT16 u2PanelHTotal;
    UINT16 u2PanelHActive;
    UINT16 u2PanelVTotal;
    UINT16 u2PanelVActive;
    UINT32 u4DPllDividend;
    UINT32 u4DPLLDivisor;
    UINT16 u2FrameRate;
    BOOL bUpdateTGLater;                    //update FSC output timge gen later (in ISR)    
    BOOL bNeedSetData;
}SCALER_OUTTIMING_PRM_T;
typedef struct
{
    UINT32 u4NonLinearMiddlePos;
    UINT32 u4NonLinearStopPos;
    UINT32 u4NonLinearPhaseSign;
    UINT32 u4NonLinearPhaseIntg; // delta
    UINT32 u4NonLinearPhaseFrac; // delta
    UINT32 u4NonLinearDeltaPhaseIntg; // initial phase
    UINT32 u4NonLinearDeltaPhaseFrac; // initial phase

} VDP_COBRA_NEW_NONLINEAR_REG_T;

/** Brief of VDP_SCALER_PRM_T
 */
typedef struct
{
    UINT32 u4Enable;                        //enabled
    #if defined(CC_SUPPORT_4K2K) || defined(SUPPORT_FHD_120)
    UINT32 u4Bypass;                        //FSC bypass
    #endif
    //Should be removed in cobra
    UINT32 u4HpsFactor;                     //HPS factor (re-calculated when scaler mode change)
    
    UINT32 u4Forced10BitMode;               //force scaler data format to 8bit or 10 bit
    UINT32 u410BitMode;                     //scaler data format (8 or 10 bit. updated when srm set dram info)
    UINT32 u4Forced444;                     //force scaler data format to 422 or 444
    UINT32 u4Is444;                         //(not in use right now) scaler data format (422 or 444. update when srm set dram info)

    SCALER_FORCE_IMPORT eForceImport;       //force import enable to this value
    
    UINT32 u4121MapEn;                      //actual flag to show if scaler is in 1-to-1 mode or not
    UINT32 u4121Map;                        //1-to-1 flag set by vdp
    UINT16 u2121MapHPos;
    
    UINT32 u4DispMode;                      //from SRM to tell Scaler what mode should be set    
    UINT32 u4IsGameMode;                    //is game mode or not

    UINT32 u4MirrorEnable;                  //if mirror is enabled
    UINT32 u4FlipEnable;                    //if flip is enabled

    UINT32 u4Is422Mode;                     //if is 422 up mode
    UINT32 u4Is422UVMirrorMode;             //422 mode + mirror
    UINT32 u4IsScalerBobMode;               //if is scaler Bob mode
    
    UINT32 u4IsLRDataSwap;
    UINT32 u4NonlinearEn;
    UINT32 u4NonlinearCtrl;
    UINT32 u4LastNonlinearCtrl;   
    UINT32 u4NonlinearAuto;
    VDP_AUTO_NONLINEAR_PRM_T rAutoNonlinearPrm;    ///< non-linear scaling setting for auto mode
    VDP_NONLINEAR_PRM_T rNonlinearPrm;
    VDP_NONLINEAR_PRM_T rLastNonlinearPrm;
    SCALER_DRAM_INIT_STATE eDramInitState;  //current dram init state
    UINT32 u4CntToRstDramPrm;               //counter to reset dram parameter, when count to 0, enable write enable
    UINT32 u4MddiFifoMin;                   //Pscan line buffer or fifo minimum limit (Query from Pscan)
    UINT32 u4MddiFifoMax;                   //Pscan line buffer or fifo maximum limit (Query from Pscan)
    SCALER_FBM_POOL_T rFbmInfo;            //Dram info from SRM call back
    SCALER_DRAM_ADDR_T rDramAddrInfo;       //
    /* 5365/5395 new nonlinear */
    UINT32 u4NewNonlinear;
    VDP_NEW_NONLINEAR_REG_T rNewNonlinearPrm;  
	VDP_COBRA_NEW_NONLINEAR_REG_T rCobraNewNonlinearPrm;
    SCALER_TDTV_PRM_T rTdtvInfo;

    //For Cobra
    VRM_INFO_T fscIn;
    VRM_INFO_T fscOut;
    VRM_INFO_T fscTimeGen;
	UINT32 u4HDeltaPhaseIntg;	            //H Scaling factor (integer part)
	UINT32 u4HDeltaPhaseFrac;	            //H Scaling factor (fraction part)
	UINT32 u4VDeltaPhaseIntg;               //V Scaling factor (integer part)
	UINT32 u4VDeltaPhaseFrac;	            //V Scaling factor (fraction part)	
    UINT32 u4HInitPhaseSign;                //H Scaling phase  (sign)
    UINT32 u4HInitPhaseIntg;                //H Scaling phase  (integer)
    UINT32 u4HInitPhaseFrac;                //H Scaling phase  (fraction)
    UINT32 u4VInitPhaseSign;                //V Scaling phase  (sign)
    UINT32 u4VInitPhaseIntg;                //V Scaling phase  (integer)
    UINT32 u4VInitPhaseFrac;                //V Scaling phase  (fraction)
    UINT32 u4HUVInitPhaseSign;              //H UV Scaling phase for 422 mode (sign)
    UINT32 u4HUVInitPhaseIntg;              //H UV Scaling phase for 422 mode (integer)
    UINT32 u4HUVInitPhaseFrac;              //H UV Scaling phase for 422 mode (fraction)
    
    UINT32 u4VInitPhaseSignTopRight;        //V Scaling phase Top Right (sign)
    UINT32 u4VInitPhaseIntgTopRight;        //V Scaling phase Top Right  (integer)
    UINT32 u4VInitPhaseFracTopRight;        //V Scaling phase Top Right  (fraction)
    UINT32 u4VInitPhaseSignBtmLeft;         //V Scaling phase Bottom Left (sign)
    UINT32 u4VInitPhaseIntgBtmLeft;         //V Scaling phase Bottom Left (integer)
    UINT32 u4VInitPhaseFracBtmLeft;         //V Scaling phase Bottom Left (fraction)
    UINT32 u4VInitPhaseSignBtmRight;        //V Scaling phase Bottom Right (sign)
    UINT32 u4VInitPhaseIntgBtmRight;        //V Scaling phase Bottom Right (integer)
    UINT32 u4VInitPhaseFracBtmRight;        //V Scaling phase Bottom Right (fraction)
    
    UINT32 u4ForcedDramMode;                //0: auto 1:force to front mode 2: force to back mode
    UINT32 u4IsDramFrontMode;               //0: back mode 1:front mode
    
	UINT32 u4DramSRCWidth;
	UINT32 u4DramWidth;			
	UINT32 u4DramHPitch;
    UINT32 u4DramLinePitch;

	UINT32 u4DramSRCHeight;
	UINT32 u4DramHeight;
	UINT32 u4DramVPitch;

	UINT32 u4RptGenCount;		            //when to gen read pointer	
	UINT32 u4DramFrmDly;                    //Frame delay setting for import
	UINT32 u4TGImportEnable;                //for Time gen Import Enable
	
	UINT32 u4InputWidth;			        //input width(pixel based)
	UINT32 u4InputHeight;                   //input height(pixel based)
	UINT32 u4OutputX;			            //output X(pixel based)
	UINT32 u4OutputY;                       //output Y(pixel based)
	UINT32 u4OutputWidth;                   //output Width(pixel based)
	UINT32 u4OutputHeight;	                //output Height(pixel based)

	BOOL bUpdateOutXYLater;                 //update output x,y later (in ISR)
	BOOL bUpdateOutWHLater;                 //update output w,h later (in ISR)
	BOOL bOutrChanged;

	#if defined(DZONE_DETECTION_EN)
	BOOL bIsInDZone;
    UINT32 u4DZoneCenter;
	#endif
} VDP_SCALER_PRM_T;

#if defined(SCALER_WA1_DRAM_WIDTH)
typedef struct
{
    UINT32 u4StartWidth;
    UINT32 u4Is10Bit;
    UINT32 u4Is444;
}SCALER_WA1_DRAM_WIDTH_TBL_T;
#endif

EXTERN VDP_SCALER_PRM_T _arScalePrm[VDP_NS];
EXTERN UINT32 _u4ForceDispOff[2];
EXTERN UINT8  _XdataAutoScan;
EXTERN UINT8  _XdataAutoScanMode;
EXTERN UINT8  _XdataAutoScanVDP;
EXTERN UINT16 _XdataAutoScanW;
EXTERN UINT16 _XdataAutoScanH;
EXTERN UINT16 _XdataAutoScanHstep;
EXTERN UINT16 _XdataAutoScanVstep;
EXTERN RDispInfo _rTveDispInfo;
#define getTveDispInfo() &_rTveDispInfo

EXTERN UINT8 _VSS_Path;
EXTERN UINT8 _VSS_Trig;
EXTERN UINT8 _VSS_Cnt;
EXTERN UINT8 _VSS_ConnectTrigger;

/*for 5398 FPGA*/
typedef enum
{
    VIDEO_RES_MODULE_PREDS,
    VIDEO_RES_MODULE_REGEN,
    VIDEO_RES_MODULE_MIB,
    VIDEO_RES_MODULE_FSC,
    VIDEO_RES_MODULE_NS,
}VIDEO_RES_MODULE_E;

typedef struct
{
    UINT32 u4X;
    UINT32 u4Y;
    UINT32 u4W;
    UINT32 u4H;
}RECTANGLE_T;

typedef struct
{
    RECTANGLE_T inRes;      //input resolution
    RECTANGLE_T ovsnRes;    //overscaned resolution
    RECTANGLE_T outRes;     //output resolution
}VIDEO_RES_DESCRIPTION_T;

typedef struct
{
    UINT32 u4HPreDsFactor;
    UINT32 u4VPreDsFactor;
    UINT32 u4ForcedHPreDsFactor;
    UINT32 u4ForcedVPreDsFactor;
    UINT32 u4PreDsInW;
    UINT32 u4PreDsInH;
    UINT32 u4PreDsOutW;
    UINT32 u4PreDsOutH;

    UINT32 u4RegenInW;
    UINT32 u4RegenInH;
    UINT32 u4RegenOutX;
    UINT32 u4RegenOutY;
    UINT32 u4RegenOutW;
    UINT32 u4RegenOutH;
    
    //fpga only
    UINT32 u4NXNRWidth;
    UINT32 u4NXNRHeight;   
    //fpga only(end)
    
	UINT32 u4InputWidth;			//input timing(pixel based)
	UINT32 u4InputHeight;

	UINT32 u4OutputX;			//output window(pixel based)
	UINT32 u4OutputY;
	UINT32 u4OutputWidth;
	UINT32 u4OutputHeight;

	UINT32 u4HDeltaPhaseIntg;	//integer
	UINT32 u4HDeltaPhaseFrac;	//fraction
	UINT32 u4VDeltaPhaseIntg;   //integer
	UINT32 u4VDeltaPhaseFrac;	//fraction	

    UINT32 u4HInitPhaseSign;
    UINT32 u4HInitPhaseIntg;
    UINT32 u4HInitPhaseFrac;
    UINT32 u4VInitPhaseSign;
    UINT32 u4VInitPhaseIntg;
    UINT32 u4VInitPhaseFrac;
    UINT32 u4HUVInitPhaseSign;
    UINT32 u4HUVInitPhaseIntg;
    UINT32 u4HUVInitPhaseFrac;

    UINT32 u4NonLinearInTotal;
    UINT32 u4NonLinearInMiddle;
    UINT32 u4NonLinearOutTotal;
    UINT32 u4NonLinearOutMiddle;

    UINT32 u4NonLinear;
    UINT32 u4NonLinearStartPos;
    UINT32 u4NonLinearMiddlePos;
    UINT32 u4NonLinearStopPos;
    UINT32 u4NonLinearPhaseSign;
    UINT32 u4NonLinearPhaseIntg; // delta
    UINT32 u4NonLinearPhaseFrac; // delta

    UINT32 u4Mirror;
    UINT32 u4Flip;

	UINT32 u4DramSRCWidth;

	UINT32 u4DramWidth;			//dram HV
	UINT32 u4DramHeight;
	UINT32 u4DramHPitch;
	UINT32 u4DramVPitch;

    UINT32 u4ForcedDramMode;    //0: auto 1:force to front mode 2: force to back mode
    UINT32 u4IsDramFrontMode;   //0: back mode 1:front mode
    
	UINT32 u4Forced10Bit;       //0: auto 1:force to 10 bit 2: force to 8 bit
	UINT32 u4Is10Bit;           //0: 8 bit 1: 10 bit

	UINT32 u4Forced444;         //0: auto 1:force to 444 2:force to 422
	UINT32 u4Is444;             //0: 422 1:444

	UINT32 u4RptGenCount;		//when gen read pointer

	UINT32 u4Dispmode;          //current scaler dispmode
    SCALER_TDTV_IN_TYPE eRegenIn3DType;	
} SCALERINFO;

    
//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//*****************************************************************************
//  drv_scaler.c
//*****************************************************************************
void vScpipSet121Map(UCHAR ucVdpId, UCHAR ucOnOff);
UINT32 vScpipGet121Map(UCHAR ucVdpId);
void vScpipSet121MapHPos(UCHAR ucVdpId, UINT16 u2HPos);
void vScpipUpdate121MapEn(UINT32 u4VdpId);
void _VDP_Set10bitMode(UINT32 u4VdpId, UINT32 u4Mode);
void vScpipSetDynamicScaler(UCHAR ucVdpId, UCHAR ucOnOff, UINT32 u4ResponseTime);
void vScpipDelayEnable(UCHAR ucVdpId, UCHAR ucEnable, UINT32 u4DelayCount);
void vScpipSetForcedHpsFactor(UINT32 u4VdpId,UINT32 u4Factor);
void vScpipSetForcedVpsFactor(UINT32 u4VdpId,UINT32 u4Factor);
void vScpipPrintCurrentScale(UCHAR ucVdpId);
void vScpipDumpResInfo(UINT8 bPath);
void vScpipForceWriteOff(UINT32 u4VdpId, UINT8 u1OnOff); 
UINT16 _VDP_ScposGetPorch(UINT8 bPath, UINT8 bPorchType);
void _VDP_ScposSetPorch(UINT8 bPath,UINT8 bPorchType,UINT16 wValue);

void vScpipReconfigScl(UINT32 u4VdpId);
void vScpipReconfig(UINT32 ucVdpId);
void vScpipSwInit(void);
UINT32 u4ScpipGetPreScaleWidth(UINT8 bPath);
UINT32 u4ScpipGetPreScaleHeight(UINT8 bPath);
UINT8 u1ScpipIsVideoFreezable(UINT8 bPath);
UINT32 u4ScpipGetFrameDelay(UINT32 u4VdpId);
UINT8 u1DrvScpipGetFrmDelay10x(UINT32 u4VdpId);
void vScpipSetFreeze(UINT8 bPath, UINT8 bOnOff) ;
#ifdef CC_SCPOS_3DTV_SUPPORT
EXTERN void vScpipOnTVEInputVSync(void);
#endif
void vScpipOnVdoOutputVSync(void);
#if defined(SCPIP_SUPPORT_POST_SCALER)
void vScpipOnMJCInputVSync(void);
#endif

void vScpipOnDIOutputISR(void);
void vScpipOnInputVSync(UINT8 bPath);
UINT16 u2ScpipGetDispWidth(UINT8 bPath);
UINT16 u2ScpipGetDispHeight(UINT8 bPath);
void vScpipForceDispOff(UINT32 u4Path, UINT32 u4OnOff);
void vScpipForceDispOff_Ext(UINT32 u4Path,UINT32 u4DispOffModule, UINT32 u4OnOff);
void vScpipNormProc(void);
UINT32 u4ScpipGet10BitMode(UINT32 u4VdpId);
UINT32 u4ScpipGetForced10BitMode(UINT32 u4VdpId);
void  vScpipSetFrameTrackTargetVar(UINT32 target);
UINT32 u4ScpipGetFrameTrackTargetVar(void);
void vScpipFrameTrackConfig(UINT32 u4Step);
void vScpipOnVdoModeChange(UINT8 bPath);
void vScpipOnVdoModeChangeDone(UINT8 bPath);
#ifdef CC_FLIP_MIRROR_SUPPORT
EXTERN void vScpipConfigFlip(UINT8 bPath);
#endif

// Cobra
UINT8 u1ScpipIsTGUnderflow(UINT32 u4VdpId);

#if defined(SCALER_WA1_DRAM_WIDTH)
void vScpipCheckDramWidth(UINT8 bPath);
UINT32 u4ScalerGetFailWidth(UINT32 u4DramWidth, UINT32 u4Is10Bit, UINT32 u4Is444);
#endif

void vScpipSetImportEnable(UINT8 bPath, UINT32 u4OnOff);

BOOL bScpipIsNeedSetData(void);
void vScpipClearNeedSetDataFlag(void);
void vScpipUpdateReg_SETDATA(void);

void vDrvScposSuspend(void);
void vDrvScposResume(void);

//*****************************************************************************
//  drv_scaler_dram.c
//*****************************************************************************
void vScpipWriteCtrlOnOff(UCHAR ucVdpId, UCHAR ucOnOff);
void vDrvScpipSrmDispCb(UINT32 u4VdpId, UINT32 u4Mode, UINT32 u4PDSize, UINT32 u4SramMode);
void vScpipGetDramInf(UINT8 bPath,SCALER_INFO_T* inf);
UINT32 vScpipGetDramSnakeMap(UINT8 bPath);

//*****************************************************************************
//  drv_scaler_dispmode.c
//*****************************************************************************
UINT8 u1ScpipGetDispMode(UINT8 bPath);
void _VDP_DispModeEnable(UCHAR ucVdpId, UCHAR ucMode);
void vScpipSetPSCANDispmode(UINT8 bPath,UINT8 bOnOff);

#if defined(CC_MT5398)
void vScpipSetSCTimeGen(UINT16 u2HTotal,UINT16 u2HActive, UINT16 u2VTotal, UINT16 u2VActive);
#endif

//*****************************************************************************
//  drv_scaler_3d.c
//*****************************************************************************
void vScpipSetTdtvIO(UINT32 u4VdpId, SCALER_TDTV_IN_TYPE in, SCALER_TDTV_OUT_TYPE out);
SCALER_TDTV_IN_TYPE eScpipGetTdtvInType(UINT32 u4VdpId);
void vScpipSetTdtvInType(UINT32 u4VdpId, SCALER_TDTV_IN_TYPE in);
SCALER_TDTV_OUT_TYPE eScpipGetTdtvOutType(UINT32 u4VdpId);
void vScpipSetTdtvOutType(UINT32 u4VdpId, SCALER_TDTV_OUT_TYPE out);
E_TDTV_RES_OUTPUT eScpipGetTdtvResolutionType(UINT32 u4VdpId, SCALER_TDTV_RES_POSITION pos);
void vScpipSetTdtvResolutionType(UINT32 u4VdpId, SCALER_TDTV_RES_POSITION pos, E_TDTV_RES_OUTPUT type);   
#ifdef CC_ENBALE_SCPIP_DO_3D
void vScpipUpdateSrm3DResolution(UINT32 u4VdpId);
#endif
void vScpipFire3DResChange(UINT32 u4VdpId);
UINT32 u4ScpipGetTDTVPRUpFlag(UINT32 u4VdpId);
UINT8 vScpipIs3DDoneByScaler(UINT32 u4VdpId);
UINT8 vScpipIs3DDoneByMJC(void);
void vScpipSetDispROutInv(UINT32 bOnOff);
#ifdef CC_ENBALE_SCPIP_DO_3D
void vScpipUpdate3DPrm(UINT32 u4VdpId);
#endif
void vScpipSet3DReg(UINT32 u4VdpId);
void vScpipUpdateScalerBobModeFlag(UINT32 u4VdpId); 
void vScpipUpdateScalerBobModePhasePrm(UINT32 u4VdpId);
void vScpipSet3DModeReg(UINT32 u4VdpId, SCALER_TDTV_IN_TYPE inType);
void vScpipSet3DLRFreerunReg(UINT32 u4VdpId, SCALER_TDTV_IN_TYPE inType);
void vScpipUpdateDisprFreerunPrm(UINT32 u4VdpId);
//*****************************************************************************
//  drv_scaler_ptgen.c
//*****************************************************************************
void vScpipSelfInputPatternOnOff(UINT32 u4OnOff);
void vScpipSetInputPattern(UINT32 u4VdpId, UINT32 u4OnOff, UINT32 u4Width, UINT32 u4Height);
void vScpipSetOutputPattern(UINT32 u4VdpId, UINT32 u4OnOff, UINT32 u4Width, UINT32 u4Height);
#if SUPPORT_DOT_TO_DOT_PATTERN
void vScpipDotToDotPattern(UINT8 ucOnOff,UINT32 u4RGBColor);
#endif

//*****************************************************************************
//  drv_scaler_nonlinear.c
//*****************************************************************************
void vScpipSetNonLinearUpScaler(UCHAR ucVdpId, UCHAR ucOnOff);
void vScpipSetNewNonLinearOnOff(UCHAR ucVdpId, UCHAR ucOnOff);
void vScpipSetNewNonLinear(const UINT32 u4VdpId, VDP_NEW_NONLINEAR_PRM_T rNonlPrm);
void vScpipGetNewNonLinear(const UINT32 u4VdpId, VDP_NEW_NONLINEAR_PRM_T *prNonlPrm);
void vScpipSetAutoNonLinear(UINT32 u4VdpId, VDP_AUTO_NONLINEAR_PRM_T rAutoNonlinearPrm); 
void vScpipSetManualNonLinear(UINT32 u4VdpId, VDP_NONLINEAR_PRM_T rNonlinearPrm);

UINT8 vScpipCalNonLinear(UINT32 u4VdpId, UINT32 u4SrcWidth, UINT32 u4OutWidth);
void vScpipUpdateUsNonlinear(UINT32 u4VdpId,UINT32 u4SrcWidth,UINT32 u4OutWidth);
void vScpipSetUsNonlinearReg(UINT32 u4VdpId);
void vScpipCalCobraNewNonLinear( UINT32 u4VdpId,UINT32 iInputTotal,UINT32 iOutputTotal );
void vScpipChangeOldNonlinearParToNew(UINT32 u4VdpId,UINT32 u4SrcWidth, UINT32 u4OutWidth); 



//*****************************************************************************
//  drv_scpos_dump.c
//*****************************************************************************
BOOL fgForceScalerDramDump(UINT32 u4VdpId, BOOL fgOnOff);
void vScpipSetDumpCtrl(UINT8 bOnOff);

//*****************************************************************************
//  drv_scaler_hw.c
//*****************************************************************************

//API for setting POCLK register
#if defined(CC_MT5398)
void vScpipSetPOCLKSelReg(UINT8 poclkSel);
void vScpipSetPOCLKDPllSelReg(UINT8 poclkDpllSel);
void vScpipSetOclkTveSelReg(UINT8 oclkTveSel);
void vScpipSetPOCLKDividerReg(UINT32 dividend, UINT32 divisor);
#endif
void vScpipSetImportOnOff(UINT8 bPath, UINT8 bOnOff);
void vScpipSetResImportEnable(UINT8 bPath, UINT32 u4OnOff);
UINT32 u4ScpipGetBufNum(UINT32 u4VdpId);
void vScpipLinesyncModeOnOff(UINT32 u4OnOff);
void vScpipSetBufNum(UINT32 u4VdpId, UINT32 u4BufNum);
void vScpipSetDramWrite(UCHAR u4VdpId, UCHAR ucOnOff);
void vScpipSetDramRead(UCHAR u4VdpId, UCHAR ucOnOff);
UINT8 bScpipGetDramWrite(UINT32 u4VdpId);
UINT8 bScpipGetDramRead(UINT32 u4VdpId);
void vScpipWriteRelatchEn(UINT32 u4VdpId, UINT32 u4OnOff);
void vScpipSetDramLinePitch(UINT32 u4VdpId, UINT32 u4Pitch);
void vScpipDramFrameDropEn(UINT8 bPath, UINT32 u4Enable);
void vScpipSetFrameDropRatio(UINT8 bPath, UINT32 u4N, UINT32 u4M);

void vScpipHwInitOnPanelInit(UINT8 bPath);

// GFX-related function
void vScpipGfxSrcSel(UINT32 u4Sel);
void vScpipGfx422To444Enable(UINT32 u4Enable);
void vScpipGfxBypass3x3(UINT32 u4Enable);
void vScpipGfxFrameDropEn(UINT32 u4VdpId, UINT8 u4Enable, UINT8 u1DropM, UINT8 u1DropN);
UINT8 u1ScpipGfxGetFrameIdx(void);
UINT8 u1ScpipGFXGetWriteStatus(void);
void vScpipGfxSetAddrBase(UINT32 u4BaseNum, UINT32 u4Addr);
UINT32 vScpipGfxGetAddrBase(UINT32 u4BaseNum);
void vScpipGfxSetAddrLimit(UINT32 u4Hi, UINT32 u4Lo);
void vScpipGfx3DTo2DEnable(UINT32 u4VdpId, UINT32 u4Enable);
void vScpipGfxSetAlpha(UINT32 u4Alpha);
void vScpipGfxAlphaPosSel(UINT32 u4Option);
void vScpipGfxSetFrameNum(UINT32 u4Num);
UINT8 u1ScpipGfxGetFrameNum(void);
void vScpipGfxSetMode(UINT32 u4Mode);
void vScpipGfxSetMux(UINT32 u4MuxR, UINT32 u4MuxG, UINT32 u4MuxB);
void vScpipGfxWriteEn(UINT32 u4Enable);
void vScpipGfxSetWriteEnMode(UINT32 u4Mode);
void vScpipGfxWriteTrigger(UINT32 u4Trigger);

//pattern gen. related function
void vScpipSetInPtgStyle(UINT8 bPath);
void vScpipSetOutPtgStyle(UINT8 bPath);
void vPscSetInPtgStyle(void);
void vPscSetOutPtgStyle(void);
void vScpipInPtgEnable(UINT8 bPath, UINT32 u4Enable);
void vScpipSetInPtgBndSize(UINT8 bPath, UINT32 u4BndWidth, UINT32 u4BndHeight);
void vScpipInPtgBndEnable(UINT8 bPath, UINT32 u4Enable);
void vScpipOutPtgEnable(UINT8 bPath, UINT32 u4Enable);
void vScpipOutPtgBndEnable(UINT8 bPath, UINT32 u4Enable);
void vScpipSetOutPtgBndSize(UINT8 bPath, UINT32 u4BndWidth, UINT32 u4BndHeight);
void vScpipSetSelfPtgRes(UINT32 u4Hact, UINT32 u4Vact, UINT32 u4Htot, UINT32 u4Vtot);
//Frame track related function
void vScpipSetFrmTrkLines(UINT32 u4Lines);
UINT16 vScpipGetFrmTrkLines(void);
void vScpipSetFrmTrkFrames(UINT32 u4Frames);
void vScpipSetDddsTrkInv(UINT32 u4Inv);
void vScpipFrmTrkDddsEnable(UINT32 u4Enable);
void vScpipSelFrmTrkRefVsync(UINT32 u4Option);
void vScpipSetDddsTurboSpeed(UINT32 u4Speed);
void vScpipSetFrmTrkStep(UINT32 u4Step);
UINT8 bScpipGetFrmTrkStep(void);
void vScpipSetFrmTrkLockTor(UINT32 u4Lines);
void vScpipSetFrmTrkLockWin(UINT32 u4Width);
void vScpipFrmTrkInLREnable(UINT32 u4Enable);
void vScpipSelFrmTrkVsyncInEdge(UINT32 u4Option);
UINT32 u4ScpipGetInOutVDistance(void);
UINT32 u4ScpipGetFrametrackABSDistance(void);
UINT32 u4ScpipGetFrametrackVTotalStatus(void);
UINT32 u4ScpipGetFrametrackLockStatus(void);
//disp-r related function
UINT8 u1ScpipGetInDispR(void);
UINT8 u1ScpipGetOutDispR(void);

//panel time gen. related function
void vScpipSetPanelWidth(UINT32 u4Width);
void vScpipSetPanelHeight(UINT32 u4Height);
void vScpipSetPanelHtotal(UINT32 u4HTotal);
void vScpipSetPanelVtotal(UINT32 u4VTotal);
UINT32 u4ScpipGetPanelWidth(void);
UINT32 u4ScpipGetPanelHeight(void);
UINT32 u4ScpipGetPanelHtotal(void);
UINT32 u4ScpipGetPanelVtotal(void);
UINT32 u4ScpipGetInputHTotalStatus(UINT8 bPath);
UINT32 u4ScpipGetInputHActiveStatus(UINT8 bPath);
UINT32 u4ScpipGetInputVTotalStatus(UINT8 bPath);
UINT32 u4ScpipGetInputVActiveStatus(UINT8 bPath);
UINT32 u4ScpipGetOutputHActiveStatus(UINT8 bPath);
UINT32 u4ScpipGetOutputVActiveStatus(UINT8 bPath);
UINT32 u4ScpipGetOutputHTotalStatus(UINT8 bPath);
UINT32 u4ScpipGetOutputVTotalStatus(UINT8 bPath);
void vScpipSetHsyncBp(UINT32 u4BckPorch);
void vScpipSetHsyncFp(UINT32 u4FrnPorch);
void vScpipSetVsyncBp(UINT32 u4BckPorch);
void vScpipSetVsyncFp(UINT32 u4FrnPorch);
UINT32 u4ScpipGetHsyncBp(void);
UINT32 u4ScpipGetHsyncFp(void);
UINT32 u4ScpipGetVsyncBp(void);
UINT32 u4ScpipGetVsyncFp(void);
void vScpipSetData(UINT32 u4OnOff);
void vScpipSetTgImportVCnt(UINT32 u4Vcunt);
void vScpipSetTgImportOnOff(UINT32 u4OnOff);

//SCCTRL related API
UINT32 vScpipGetInputWidth(UINT8 bPath);
UINT32 vScpipGetInputHeight(UINT8 bPath);
void vScpipSetInputResolution(UINT8 bPath, UINT32 u4InputWidth, UINT32 u4InputHeight);
void vScpipSetOutputResolution(UINT8 bPath, UINT32 u4OutputWidth, UINT32 u4OutputHeight);
void vScpipSetHInitPhase(UINT8 bPath, UINT32 u4InitSign, UINT32 u4InitIntg, UINT32 u4InitFrac);
void vScpipSetHDeltaPhase(UINT8 bPath, UINT32 u4DeltaIntg, UINT32 u4DeltaFrac);
void vScpipSetHUVInitPhase(UINT8 bPath,UINT32 u4InitSign, UINT32 u4InitIntg, UINT32 u4InitFrac);
void vScpipSetVInitPhase(UINT8 bPath, UINT32 u4InitSign, UINT32 u4InitIntg, UINT32 u4InitFrac);
void vScpipSetVDeltaPhase(UINT8 bPath, UINT32 u4DeltaIntg, UINT32 u4DeltaFrac);
void vScpipBobVScalingOnOff(UINT32 u43DVUpOnOff, UINT32 u4FieldVUpOnOff, UINT32 u4EnableDiffInitPhase);
void vScpipSetBobBtmLeftInitPhase(UINT32 u4InitSign, UINT32 u4InitIntg, UINT32 u4InitFrac);
void vScpipSetBobTopRightInitPhase(UINT32 u4InitSign, UINT32 u4InitIntg, UINT32 u4InitFrac);
void vScpipSetBobBtmRightInitPhase(UINT32 u4InitSign, UINT32 u4InitIntg, UINT32 u4InitFrac);
void vScpipSetDramSrcWidth(UINT8 bPath, UINT32 u4DramWidth);
void vScpipSetDramSrcHeight(UINT8 bPath, UINT32 u4DramHeight);
void vScpipSetDramOverScanTop(UINT8 bPath, UINT16 u2OverScanTop);
void vScpipSetDramWidthHeight(UINT8 bPath, UINT32 u4DramWidth, UINT32 u4DramHeight);
void vScpipSetDramHVPitch(UINT8 bPath, UINT32 u4DramWidth, UINT32 u4DramHeight);
void vScpipSetReadPosGen(UINT8 bPath, UINT32 u4VCnt);
void vScpipSetSubMaskDelay(UINT32 u4HMaskDelay, UINT32 u4VMaskDelay);
void vScpipSetMaskStart(UINT8 bPath, UINT32 u4Hofst, UINT32 u4VOfst);
void vScpipSet422Mode(UINT8 bPath, UINT32 u4OnOff);
void vScpipReorderUV(UINT32 u4Inverse);
void vScpipSet422MirrorEnable(UINT8 bPath, UINT32 u4OnOff);
void vScpipSetDram10bFmt(UINT8 bPath, UINT32 u4OnOff);
void vScpipSetDram444Fmt(UINT8 bPath, UINT32 u4Sel444);
void vScpipEnable444To422BeforeDram(UINT8 bPath, UINT32 u4Enable);
void vScpipEnable422To444AfterDram(UINT8 bPath, UINT32 u4Enable);
void vScpipEnable422To444BeforeCTI(UINT8 bPath, UINT32 u4Enable);
void vScpipEnable444To422AfterCTI(UINT8 bPath, UINT32 u4Enable);
#if defined(CC_MT5880) || defined(CC_MT5860)
void vScpipSetNoCTI(UINT8 bPath, UINT32 u4OnOff);
#endif

#if defined(CC_SUPPORT_4K2K) || defined(SUPPORT_FHD_120)
void vScpipBypass(UINT8 bPath, UINT32 u4OnOff);
void vScpipInverseInputVDE(UINT8 bPath,UINT32 u4OnOff);
void vScpipInverseOutputVSync(UINT32 u4OnOff);
void vScpipInverseOutputHSync(UINT32 u4OnOff);
#endif

void vScpipSetTimeGenDelay(UINT8 bPath, UINT32 u4Delay);
void vScpipSetTimeGenDlyImportOnOff(UINT8 bPath, UINT32 u4OnOff);
UINT32 vScpipGetTimeGenDlyImportOnOff(UINT8 bPath);
void vScpipSetTimeGenDlyImportEnable(UINT8 bPath, UINT32 u4OnOff);

void vScpipSetFrontMode(UINT8 bPath, UINT32 u4Mode);
void vScpipSetBackMode(UINT8 bPath, UINT32 u4Mode);
void vScpipSetDramFrmDely(UINT8 bPath, UINT32 u4Delay);
void vScpipSetSC3DMode(UINT32 u4Mode);
void vScpipSetDispRFreerun(UINT32 u4OnOff);
void vScpipSetMibDispRMode(UINT32 u4Mode);
void vScpipSetDispFifoWaterLevel(UINT8 bPath, UINT32 u4Level);

//DRAM related API
void vScpipDramImportOnOff(UINT8 bPath, UINT32 u4OnOff);
void vScpipSetDramSpacing(UINT8 bPath, UINT32 u4Space);
void vScpipSetDramSnakeMap(UINT8 bPath, UINT32 u4OnOff);
void vScpipSetDramReadReqDly(UINT8 bPath, UINT32 u4Delay);
void vScpipSetDramRdreqDly(UINT8 bPath, UINT32 u4Delay);

void vScpipSetScalingCoef(UINT8 u1En);

// workabourd for dram mdoe failure under certain width
void vScpipSetDramFailWorkAround(UINT8 u1OnOff);


//for 5398 FPGA
EXTERN void vOnScalerInfoChg(UINT32 u4VdpId);
EXTERN void vScpipXdataAutoCheck(UINT32 u4VdpId, UINT32 u4Width, UINT32 u4Height, UINT32 u4Hstep, UINT32 u4Vstep);


//tve api
EXTERN void vScpipSetTveTiming(UINT32 u4Hact, UINT32 u4Htot, UINT32 u4Vact, UINT32 u4Vtot);
EXTERN void vScpipSetTvePorch(UINT32 u4Hfp, UINT32 u4Hbp, UINT32 u4Vfp, UINT32 u4vbp);
EXTERN void vScpipToTveEnable(UINT32 u4Enable);
EXTERN void vScpipTveImportEn(UINT32 u4Enable);
EXTERN void vScpipTveImportTg(UINT32 u4Enable);
EXTERN UINT32 u4ScpipGetDramRStatus(UINT8 bPath);
EXTERN UINT32 u4ScpipGetDramWStatus(UINT8 bPath);


EXTERN void vScpipSetOsdTveTiming(UINT32 u4HLen, UINT32 u4VLen);
EXTERN void vScpipInvOsdTveVsync(UINT8 uOnOff);
EXTERN void vScpipSetOsdTveVsyncSel(UINT8 uOnOff);
EXTERN void vScpipSetOsdTveVsyncDly(UINT32 u4Value);

EXTERN void vScpipSetPureEdgeBlankNum(UINT32 u4BlankNum);

BOOL bDrvScpipIsSetResDone(UINT8 u1VdpId, SET_RES_STATUS_TYPE_E eStatusType);
void vDrvScpipClrSetRes(UINT8 u1VdpId, SET_RES_STATUS_TYPE_E eStatusType);

void vScpipSetPipRstCnt(UINT8 uRstCnt);
void vScpipInOutStatusDump(UINT8 bPath);

#endif

