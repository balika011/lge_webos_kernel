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

// === INCLUDE =============================================================================
#include "x_os.h"	
#include "sv_const.h"
#include "hw_vdoin.h"
#include "hw_nr.h"
#include "vdo_misc.h"
#include "nptv_debug.h"
#include "drv_contrast.h"
#include "drv_meter.h"
#include "pe_if.h"
#include "drv_scaler.h"

#include "drv_tdtv_drvif.h"
#include "fbm_drvif.h"
#include "drv_display.h"

// === HW INCLUDE =============================================================================
#include "hw_tdsharp.h"
#include "hw_scpip_pq.h"
#include "hw_sw.h"
#include "hw_di_int.h"

// === DEFINE =============================================================================
#define HSHARP_OFFSET           0x00    //H Sharp has no sub path
#define AbsDiff(a, b)   (((a)>(b))?((a)-(b)):((b)-(a)))
#define CLIP(val, min, max) ((val>=max) ? max : ((val<=min) ? min : val)) 

// === EXTERN FUNCTION ====================================================================
static void vDrvSharpCalcuACCurve(UINT8 bStartBand, UINT8 bEndBand);
EXTERN UINT8 bADLSceneChange;
EXTERN UINT8 bForceAllPQOff;
#ifdef CC_FLIP_MIRROR_SUPPORT
extern UINT8 u1GetFlipMirrorConfig(void);
extern UINT32 u4QueryFlipModule(UINT32 u4VdpId);
#endif

// === GLOBAL VARIABLE ====================================================================

// === EXTERN VARIABLE ====================================================================
#ifdef CUST_SOEM_DRV
static BOOL fgSharpForce = SV_FALSE;    //for resume
#endif
static BOOL fgAdapYLEVLock = SV_OFF;
//static BOOL fgAdapYLEV = SV_ON;
static UINT16 TDS_ADAP_YLEV[17];

UINT8 TDS_YLEV[17] =
{
	0x40,0x4D,0x5A,0x66,0x73,0x76,0x7A,0x7D,
	0x80,0x80,0x80,0x80,0x80,0x7D,0x7A,0x76,
	0x72  
};

UINT8 TDS_YLEV_APL[17] =
{
       0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
       0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
       0x80       
};

UINT32 TDS_YLEV_32BIT_VALUE[5];
#ifndef CC_MT5398
static UINT32 const TDS_CURVE_REGADDR[TDS_BAND_NUM]=
{
	SHARP_03, // H1
	SHARP_07, //H2
	SHARP_0B, //H3
	SHARP_0F, //V1
	SHARP_13, //V2
	SHARP_17, //V3
	SHARP_1B, //D1
	SHARP_1F, //D2
	SHARP_23, //D3
	SHARP_50, //LTI_HL
	SHARP_54, //LTI_HH
	SHARP_58, //LTI_V
	CDS_06,   //CDS1_H1
	CDS_0A,   //CDS1_H2
	CDS_0E,   //CDS1_H3
	CDS_12,   //CDS1_V1
	CDS_16,   //CDS1_V2
	CDS_1A,   //CDS1_V3
	CDS_1E,   //CDS1_D1
	CDS_22,   //CDS1_D2
	CDS_26,   //CDS1_D3
	CDS_30,   //CDS2_H1
	CDS_34,   //CDS2_H2
	CDS_38,   //CDS2_H3
	CDS_3C,   //CDS2_V1
	CDS_40,   //CDS2_V2
	CDS_44,   //CDS2_V3
	CDS_48,   //CDS2_D1
	CDS_4C,   //CDS2_D2
	CDS_50,   //CDS2_D3
	CDS_59,   //CDS3_H1
	CDS_5D,   //CDS3_H2
	CDS_61,   //CDS3_H3
	CDS_65,   //CDS3_V1
	CDS_69,   //CDS3_V2
	CDS_6D,   //CDS3_V3
	CDS_71,   //CDS3_D1
	CDS_75,   //CDS3_D2
	CDS_79    //CDS3_D3
};

static UINT32 const TDS_Curve_SW_Reg[TDS_BAND_NUM][TDS_CURVE_SW_REG_END]=
{
    //Gain                          //Coring_zero               //Coring_ratio                         //Coring_th                      //Limit                         //Limit_ratio                           //Bound
    {TDSHP_GAIN_H1,      TDSHP_CORE_H1,      TDSHP_CORING_RATIO_H1,    TDSHP_CORETH_H1,      TDSHARP_LIMIT_H1,    TDSHP_LIMIT_RATIO_H1,      TDSHARP_BOUND_H1}, 
    {TDSHP_GAIN_H2,      TDSHP_CORE_H2,      TDSHP_CORING_RATIO_H2,    TDSHP_CORETH_H2,      TDSHARP_LIMIT_H2,    TDSHP_LIMIT_RATIO_H2,      TDSHARP_BOUND_H2},
    {TDSHP_GAIN_H3,      TDSHP_CORE_H3,      TDSHP_CORING_RATIO_H3,    TDSHP_CORETH_H3,      TDSHARP_LIMIT_H3,    TDSHP_LIMIT_RATIO_H3,      TDSHARP_BOUND_H3},
    {TDSHP_GAIN_V1,      TDSHP_CORE_V1,      TDSHP_CORING_RATIO_V1,    TDSHP_CORETH_V1,      TDSHARP_LIMIT_V1,    TDSHP_LIMIT_RATIO_V1,      TDSHARP_BOUND_V1},
    {TDSHP_GAIN_V2,      TDSHP_CORE_V2,      TDSHP_CORING_RATIO_V2,    TDSHP_CORETH_V2,      TDSHARP_LIMIT_V2,    TDSHP_LIMIT_RATIO_V2,      TDSHARP_BOUND_V2},
    {TDSHP_GAIN_V3,      TDSHP_CORE_V3,      TDSHP_CORING_RATIO_V3,    TDSHP_CORETH_V3,      TDSHARP_LIMIT_V3,    TDSHP_LIMIT_RATIO_V3,      TDSHARP_BOUND_V3},
    {TDSHP_GAIN_D1,      TDSHP_CORE_D1,      TDSHP_CORING_RATIO_D1,    TDSHP_CORETH_D1,      TDSHARP_LIMIT_D1,    TDSHP_LIMIT_RATIO_D1,      TDSHARP_BOUND_D1},
    {TDSHP_GAIN_D2,      TDSHP_CORE_D2,      TDSHP_CORING_RATIO_D2,    TDSHP_CORETH_D2,      TDSHARP_LIMIT_D2,    TDSHP_LIMIT_RATIO_D2,      TDSHARP_BOUND_D2},
    {TDSHP_GAIN_D3,      TDSHP_CORE_D3,      TDSHP_CORING_RATIO_D3,    TDSHP_CORETH_D3,      TDSHARP_LIMIT_D3,    TDSHP_LIMIT_RATIO_D3,      TDSHARP_BOUND_D3},
    //LTI
    {TDSHP_GAIN_LTI_H1,  TDSHP_CORE_LTI_H1,  TDSHP_CORE_RATIO_LTI_H1,  TDSHP_CORETH_LTI_H1,  LTI_LIMIT_LTI_H1,    TDSHP_LIMIT_RATIO_LTI_H1,  LTI_BOUND_LTI_H1},
    {TDSHP_GAIN_LTI_H2,  TDSHP_CORE_LTI_H2,  TDSHP_CORE_RATIO_LTI_H2,  TDSHP_CORETH_LTI_H2,  LTI_LIMIT_LTI_H2,    TDSHP_LIMIT_RATIO_LTI_H2,  LTI_BOUND_LTI_H2},
    {TDSHP_GAIN_LTI_V,   TDSHP_CORE_LTI_V,   TDSHP_CORE_RATIO_LTI_V,   TDSHP_CORETH_LTI_V,   LTI_LIMIT_LTI_V,     TDSHP_LIMIT_RATIO_LTI_V,   LTI_BOUND_LTI_V},
    //PBC1
    {TDSHP_GAIN_PBC1_H1, TDSHP_CORE_PBC1_H1, TDSHP_CORE_RATIO_PBC1_H1, TDSHP_CORETH_PBC1_H1, TDSHP_LIMIT_PBC1_H1, TDSHP_LIMIT_RATIO_PBC1_H1, TDSHP_BOUND_PBC1_H1},
    {TDSHP_GAIN_PBC1_H2, TDSHP_CORE_PBC1_H2, TDSHP_CORE_RATIO_PBC1_H2, TDSHP_CORETH_PBC1_H2, TDSHP_LIMIT_PBC1_H2, TDSHP_LIMIT_RATIO_PBC1_H2, TDSHP_BOUND_PBC1_H2},
    {TDSHP_GAIN_PBC1_H3, TDSHP_CORE_PBC1_H3, TDSHP_CORE_RATIO_PBC1_H3, TDSHP_CORETH_PBC1_H3, TDSHP_LIMIT_PBC1_H3, TDSHP_LIMIT_RATIO_PBC1_H3, TDSHP_BOUND_PBC1_H3},
    {TDSHP_GAIN_PBC1_V1, TDSHP_CORE_PBC1_V1, TDSHP_CORE_RATIO_PBC1_V1, TDSHP_CORETH_PBC1_V1, TDSHP_LIMIT_PBC1_V1, TDSHP_LIMIT_RATIO_PBC1_V1, TDSHP_BOUND_PBC1_V1},
    {TDSHP_GAIN_PBC1_V2, TDSHP_CORE_PBC1_V2, TDSHP_CORE_RATIO_PBC1_V2, TDSHP_CORETH_PBC1_V2, TDSHP_LIMIT_PBC1_V2, TDSHP_LIMIT_RATIO_PBC1_V2, TDSHP_BOUND_PBC1_V2},
    {TDSHP_GAIN_PBC1_V3, TDSHP_CORE_PBC1_V3, TDSHP_CORE_RATIO_PBC1_V3, TDSHP_CORETH_PBC1_V3, TDSHP_LIMIT_PBC1_V3, TDSHP_LIMIT_RATIO_PBC1_V3, TDSHP_BOUND_PBC1_V3},
    {TDSHP_GAIN_PBC1_D1, TDSHP_CORE_PBC1_D1, TDSHP_CORE_RATIO_PBC1_D1, TDSHP_CORETH_PBC1_D1, TDSHP_LIMIT_PBC1_D1, TDSHP_LIMIT_RATIO_PBC1_D1, TDSHP_BOUND_PBC1_D1},
    {TDSHP_GAIN_PBC1_D2, TDSHP_CORE_PBC1_D2, TDSHP_CORE_RATIO_PBC1_D2, TDSHP_CORETH_PBC1_D2, TDSHP_LIMIT_PBC1_D2, TDSHP_LIMIT_RATIO_PBC1_D2, TDSHP_BOUND_PBC1_D2},
    {TDSHP_GAIN_PBC1_D3, TDSHP_CORE_PBC1_D3, TDSHP_CORE_RATIO_PBC1_D3, TDSHP_CORETH_PBC1_D3, TDSHP_LIMIT_PBC1_D3, TDSHP_LIMIT_RATIO_PBC1_D3, TDSHP_BOUND_PBC1_D3},
    //PBC2
    {TDSHP_GAIN_PBC2_H1, TDSHP_CORE_PBC2_H1, TDSHP_CORE_RATIO_PBC2_H1, TDSHP_CORETH_PBC2_H1, TDSHP_LIMIT_PBC2_H1, TDSHP_LIMIT_RATIO_PBC2_H1, TDSHP_BOUND_PBC2_H1},
    {TDSHP_GAIN_PBC2_H2, TDSHP_CORE_PBC2_H2, TDSHP_CORE_RATIO_PBC2_H2, TDSHP_CORETH_PBC2_H2, TDSHP_LIMIT_PBC2_H2, TDSHP_LIMIT_RATIO_PBC2_H2, TDSHP_BOUND_PBC2_H2},
    {TDSHP_GAIN_PBC2_H3, TDSHP_CORE_PBC2_H3, TDSHP_CORE_RATIO_PBC2_H3, TDSHP_CORETH_PBC2_H3, TDSHP_LIMIT_PBC2_H3, TDSHP_LIMIT_RATIO_PBC2_H3, TDSHP_BOUND_PBC2_H3},
    {TDSHP_GAIN_PBC2_V1, TDSHP_CORE_PBC2_V1, TDSHP_CORE_RATIO_PBC2_V1, TDSHP_CORETH_PBC2_V1, TDSHP_LIMIT_PBC2_V1, TDSHP_LIMIT_RATIO_PBC2_V1, TDSHP_BOUND_PBC2_V1},
    {TDSHP_GAIN_PBC2_V2, TDSHP_CORE_PBC2_V2, TDSHP_CORE_RATIO_PBC2_V2, TDSHP_CORETH_PBC2_V2, TDSHP_LIMIT_PBC2_V2, TDSHP_LIMIT_RATIO_PBC2_V2, TDSHP_BOUND_PBC2_V2},
    {TDSHP_GAIN_PBC2_V3, TDSHP_CORE_PBC2_V3, TDSHP_CORE_RATIO_PBC2_V3, TDSHP_CORETH_PBC2_V3, TDSHP_LIMIT_PBC2_V3, TDSHP_LIMIT_RATIO_PBC2_V3, TDSHP_BOUND_PBC2_V3},
    {TDSHP_GAIN_PBC2_D1, TDSHP_CORE_PBC2_D1, TDSHP_CORE_RATIO_PBC2_D1, TDSHP_CORETH_PBC2_D1, TDSHP_LIMIT_PBC2_D1, TDSHP_LIMIT_RATIO_PBC2_D1, TDSHP_BOUND_PBC2_D1},
    {TDSHP_GAIN_PBC2_D2, TDSHP_CORE_PBC2_D2, TDSHP_CORE_RATIO_PBC2_D2, TDSHP_CORETH_PBC2_D2, TDSHP_LIMIT_PBC2_D2, TDSHP_LIMIT_RATIO_PBC2_D2, TDSHP_BOUND_PBC2_D2},
    {TDSHP_GAIN_PBC2_D3, TDSHP_CORE_PBC2_D3, TDSHP_CORE_RATIO_PBC2_D3, TDSHP_CORETH_PBC2_D3, TDSHP_LIMIT_PBC2_D3, TDSHP_LIMIT_RATIO_PBC2_D3, TDSHP_BOUND_PBC2_D3},
    //PBC3
    {TDSHP_GAIN_PBC3_H1, TDSHP_CORE_PBC3_H1, TDSHP_CORE_RATIO_PBC3_H1, TDSHP_CORETH_PBC3_H1, TDSHP_LIMIT_PBC3_H1, TDSHP_LIMIT_RATIO_PBC3_H1, TDSHP_BOUND_PBC3_H1},
    {TDSHP_GAIN_PBC3_H2, TDSHP_CORE_PBC3_H2, TDSHP_CORE_RATIO_PBC3_H2, TDSHP_CORETH_PBC3_H2, TDSHP_LIMIT_PBC3_H2, TDSHP_LIMIT_RATIO_PBC3_H2, TDSHP_BOUND_PBC3_H2},
    {TDSHP_GAIN_PBC3_H3, TDSHP_CORE_PBC3_H3, TDSHP_CORE_RATIO_PBC3_H3, TDSHP_CORETH_PBC3_H3, TDSHP_LIMIT_PBC3_H3, TDSHP_LIMIT_RATIO_PBC3_H3, TDSHP_BOUND_PBC3_H3},
    {TDSHP_GAIN_PBC3_V1, TDSHP_CORE_PBC3_V1, TDSHP_CORE_RATIO_PBC3_V1, TDSHP_CORETH_PBC3_V1, TDSHP_LIMIT_PBC3_V1, TDSHP_LIMIT_RATIO_PBC3_V1, TDSHP_BOUND_PBC3_V1},
    {TDSHP_GAIN_PBC3_V2, TDSHP_CORE_PBC3_V2, TDSHP_CORE_RATIO_PBC3_V2, TDSHP_CORETH_PBC3_V2, TDSHP_LIMIT_PBC3_V2, TDSHP_LIMIT_RATIO_PBC3_V2, TDSHP_BOUND_PBC3_V2},
    {TDSHP_GAIN_PBC3_V3, TDSHP_CORE_PBC3_V3, TDSHP_CORE_RATIO_PBC3_V3, TDSHP_CORETH_PBC3_V3, TDSHP_LIMIT_PBC3_V3, TDSHP_LIMIT_RATIO_PBC3_V3, TDSHP_BOUND_PBC3_V3},
    {TDSHP_GAIN_PBC3_D1, TDSHP_CORE_PBC3_D1, TDSHP_CORE_RATIO_PBC3_D1, TDSHP_CORETH_PBC3_D1, TDSHP_LIMIT_PBC3_D1, TDSHP_LIMIT_RATIO_PBC3_D1, TDSHP_BOUND_PBC3_D1},
    {TDSHP_GAIN_PBC3_D2, TDSHP_CORE_PBC3_D2, TDSHP_CORE_RATIO_PBC3_D2, TDSHP_CORETH_PBC3_D2, TDSHP_LIMIT_PBC3_D2, TDSHP_LIMIT_RATIO_PBC3_D2, TDSHP_BOUND_PBC3_D2},
    {TDSHP_GAIN_PBC3_D3, TDSHP_CORE_PBC3_D3, TDSHP_CORE_RATIO_PBC3_D3, TDSHP_CORETH_PBC3_D3, TDSHP_LIMIT_PBC3_D3, TDSHP_LIMIT_RATIO_PBC3_D3, TDSHP_BOUND_PBC3_D3}
};

static UINT32 const TDS_Curve_SW_Reg_Base[TDS_BAND_NUM][TDS_CURVE_SW_REG_END]=
{
    //Gain                  //Coring_zero       //Coring_ratio                  //Coring_th             //Limit                //Limit_ratio                    //Bound
    {VI_SHP_GAIN_01, VI_SHP_CORE_01, VI_SHP_CORING_RATIO_01, VI_SHP_CORETH_01, VI_SHP_LIMIT_00, VI_SHP_LIMIT_RATIO_01, VI_SHP_LIMIT_03},//H1
    {VI_SHP_GAIN_01, VI_SHP_CORE_01, VI_SHP_CORING_RATIO_01, VI_SHP_CORETH_01, VI_SHP_LIMIT_00, VI_SHP_LIMIT_RATIO_01, VI_SHP_LIMIT_03},//H2
    {VI_SHP_GAIN_01, VI_SHP_CORE_01, VI_SHP_CORING_RATIO_01, VI_SHP_CORETH_01, VI_SHP_LIMIT_00, VI_SHP_LIMIT_RATIO_01, VI_SHP_LIMIT_03},//H3
    {VI_SHP_GAIN_02, VI_SHP_CORE_02, VI_SHP_CORING_RATIO_02, VI_SHP_CORETH_02, VI_SHP_LIMIT_01, VI_SHP_LIMIT_RATIO_02, VI_SHP_LIMIT_04},//V1
    {VI_SHP_GAIN_02, VI_SHP_CORE_02, VI_SHP_CORING_RATIO_02, VI_SHP_CORETH_02, VI_SHP_LIMIT_01, VI_SHP_LIMIT_RATIO_02, VI_SHP_LIMIT_04},//V2
    {VI_SHP_GAIN_03, VI_SHP_CORE_03, VI_SHP_CORING_RATIO_03, VI_SHP_CORETH_03, VI_SHP_LIMIT_06, VI_SHP_LIMIT_RATIO_03, VI_SHP_LIMIT_06},//V3
    {VI_SHP_GAIN_01, VI_SHP_CORE_01, VI_SHP_CORING_RATIO_01, VI_SHP_CORETH_01, VI_SHP_LIMIT_00, VI_SHP_LIMIT_RATIO_01, VI_SHP_LIMIT_03},//D1
    {VI_SHP_GAIN_02, VI_SHP_CORE_02, VI_SHP_CORING_RATIO_02, VI_SHP_CORETH_02, VI_SHP_LIMIT_01, VI_SHP_LIMIT_RATIO_02, VI_SHP_LIMIT_04},//D2
    {VI_SHP_GAIN_02, VI_SHP_CORE_02, VI_SHP_CORING_RATIO_02, VI_SHP_CORETH_02, VI_SHP_LIMIT_01, VI_SHP_LIMIT_RATIO_02, VI_SHP_LIMIT_04},//D3
    //LTI
    {VI_SHP_GAIN_04, VI_SHP_CORE_04, VI_SHP_CORING_RATIO_04, VI_SHP_CORETH_04, VI_SHP_LIMIT_02, VI_SHP_LIMIT_RATIO_04, VI_SHP_LIMIT_05},//LTI_H1
    {VI_SHP_GAIN_04, VI_SHP_CORE_04, VI_SHP_CORING_RATIO_04, VI_SHP_CORETH_04, VI_SHP_LIMIT_02, VI_SHP_LIMIT_RATIO_04, VI_SHP_LIMIT_05},//LTI_H1
    {VI_SHP_GAIN_04, VI_SHP_CORE_04, VI_SHP_CORING_RATIO_04, VI_SHP_CORETH_04, VI_SHP_LIMIT_02, VI_SHP_LIMIT_RATIO_04, VI_SHP_LIMIT_05},//LTI_H1
    //PBC1
    {VI_SHP_GAIN_05, VI_SHP_CORE_05, VI_SHP_CORING_RATIO_05, VI_SHP_CORETH_05, VI_SHP_LIMIT_05_2, VI_SHP_LIMIT_RATIO_05, VI_SHP_BOUND_05},//H1
    {VI_SHP_GAIN_05, VI_SHP_CORE_05, VI_SHP_CORING_RATIO_05, VI_SHP_CORETH_05, VI_SHP_LIMIT_05_2, VI_SHP_LIMIT_RATIO_05, VI_SHP_BOUND_05},//H2
    {VI_SHP_GAIN_05, VI_SHP_CORE_05, VI_SHP_CORING_RATIO_05, VI_SHP_CORETH_05, VI_SHP_LIMIT_05_2, VI_SHP_LIMIT_RATIO_05, VI_SHP_BOUND_05},//H3
    {VI_SHP_GAIN_05, VI_SHP_CORE_05, VI_SHP_CORING_RATIO_05, VI_SHP_CORETH_05, VI_SHP_LIMIT_05_2, VI_SHP_LIMIT_RATIO_05, VI_SHP_BOUND_05},//V1
    {VI_SHP_GAIN_06, VI_SHP_CORE_06, VI_SHP_CORING_RATIO_06, VI_SHP_CORETH_06, VI_SHP_LIMIT_06_2, VI_SHP_LIMIT_RATIO_06, VI_SHP_BOUND_06},//V2
    {VI_SHP_GAIN_06, VI_SHP_CORE_06, VI_SHP_CORING_RATIO_06, VI_SHP_CORETH_06, VI_SHP_LIMIT_06_2, VI_SHP_LIMIT_RATIO_06, VI_SHP_BOUND_06},//V3
    {VI_SHP_GAIN_06, VI_SHP_CORE_06, VI_SHP_CORING_RATIO_06, VI_SHP_CORETH_06, VI_SHP_LIMIT_06_2, VI_SHP_LIMIT_RATIO_06, VI_SHP_BOUND_06},//D1
    {VI_SHP_GAIN_06, VI_SHP_CORE_06, VI_SHP_CORING_RATIO_06, VI_SHP_CORETH_06, VI_SHP_LIMIT_06_2, VI_SHP_LIMIT_RATIO_06, VI_SHP_BOUND_06},//D2
    {VI_SHP_GAIN_07, VI_SHP_CORE_07, VI_SHP_CORING_RATIO_07, VI_SHP_CORETH_07, VI_SHP_LIMIT_07, VI_SHP_LIMIT_RATIO_07, VI_SHP_BOUND_07},//D3
    //PBC2
    {VI_SHP_GAIN_07, VI_SHP_CORE_07, VI_SHP_CORING_RATIO_07, VI_SHP_CORETH_07, VI_SHP_LIMIT_07, VI_SHP_LIMIT_RATIO_07, VI_SHP_BOUND_07},//H1
    {VI_SHP_GAIN_07, VI_SHP_CORE_07, VI_SHP_CORING_RATIO_07, VI_SHP_CORETH_07, VI_SHP_LIMIT_07, VI_SHP_LIMIT_RATIO_07, VI_SHP_BOUND_07},//H2
    {VI_SHP_GAIN_07, VI_SHP_CORE_07, VI_SHP_CORING_RATIO_07, VI_SHP_CORETH_07, VI_SHP_LIMIT_07, VI_SHP_LIMIT_RATIO_07, VI_SHP_BOUND_07},//H3
    {VI_SHP_GAIN_08, VI_SHP_CORE_08, VI_SHP_CORING_RATIO_08, VI_SHP_CORETH_08, VI_SHP_LIMIT_08, VI_SHP_LIMIT_RATIO_08, VI_SHP_BOUND_08},//V1
    {VI_SHP_GAIN_08, VI_SHP_CORE_08, VI_SHP_CORING_RATIO_08, VI_SHP_CORETH_08, VI_SHP_LIMIT_08, VI_SHP_LIMIT_RATIO_08, VI_SHP_BOUND_08},//V2
    {VI_SHP_GAIN_08, VI_SHP_CORE_08, VI_SHP_CORING_RATIO_08, VI_SHP_CORETH_08, VI_SHP_LIMIT_08, VI_SHP_LIMIT_RATIO_08, VI_SHP_BOUND_08},//V3
    {VI_SHP_GAIN_08, VI_SHP_CORE_08, VI_SHP_CORING_RATIO_08, VI_SHP_CORETH_08, VI_SHP_LIMIT_08, VI_SHP_LIMIT_RATIO_08, VI_SHP_BOUND_08},//D1
    {VI_SHP_GAIN_09, VI_SHP_CORE_09, VI_SHP_CORING_RATIO_09, VI_SHP_CORETH_09, VI_SHP_LIMIT_09, VI_SHP_LIMIT_RATIO_09, VI_SHP_BOUND_09},//D2
    {VI_SHP_GAIN_09, VI_SHP_CORE_09, VI_SHP_CORING_RATIO_09, VI_SHP_CORETH_09, VI_SHP_LIMIT_09, VI_SHP_LIMIT_RATIO_09, VI_SHP_BOUND_09},//D3
    //PBC3
    {VI_SHP_GAIN_09, VI_SHP_CORE_09, VI_SHP_CORING_RATIO_09, VI_SHP_CORETH_09, VI_SHP_LIMIT_09, VI_SHP_LIMIT_RATIO_09, VI_SHP_BOUND_09},//H1
    {VI_SHP_GAIN_09, VI_SHP_CORE_09, VI_SHP_CORING_RATIO_09, VI_SHP_CORETH_09, VI_SHP_LIMIT_09, VI_SHP_LIMIT_RATIO_09, VI_SHP_BOUND_09},//H2
    {VI_SHP_GAIN_0A, VI_SHP_CORE_0A, VI_SHP_CORING_RATIO_0A, VI_SHP_CORETH_0A, VI_SHP_LIMIT_0A, VI_SHP_LIMIT_RATIO_0A, VI_SHP_BOUND_0A},//H3
    {VI_SHP_GAIN_0A, VI_SHP_CORE_0A, VI_SHP_CORING_RATIO_0A, VI_SHP_CORETH_0A, VI_SHP_LIMIT_0A, VI_SHP_LIMIT_RATIO_0A, VI_SHP_BOUND_0A},//V1
    {VI_SHP_GAIN_0A, VI_SHP_CORE_0A, VI_SHP_CORING_RATIO_0A, VI_SHP_CORETH_0A, VI_SHP_LIMIT_0A, VI_SHP_LIMIT_RATIO_0A, VI_SHP_BOUND_0A},//V2
    {VI_SHP_GAIN_0A, VI_SHP_CORE_0A, VI_SHP_CORING_RATIO_0A, VI_SHP_CORETH_0A, VI_SHP_LIMIT_0A, VI_SHP_LIMIT_RATIO_0A, VI_SHP_BOUND_0A},//V3
    {VI_SHP_GAIN_0B, VI_SHP_CORE_0B, VI_SHP_CORING_RATIO_0B, VI_SHP_CORETH_0B, VI_SHP_LIMIT_0B, VI_SHP_LIMIT_RATIO_0B, VI_SHP_BOUND_0B},//D1
    {VI_SHP_GAIN_0B, VI_SHP_CORE_0B, VI_SHP_CORING_RATIO_0B, VI_SHP_CORETH_0B, VI_SHP_LIMIT_0B, VI_SHP_LIMIT_RATIO_0B, VI_SHP_BOUND_0B},//D2
    {VI_SHP_GAIN_0B, VI_SHP_CORE_0B, VI_SHP_CORING_RATIO_0B, VI_SHP_CORETH_0B, VI_SHP_LIMIT_0B, VI_SHP_LIMIT_RATIO_0B, VI_SHP_BOUND_0B},//D3    
};
#ifndef CC_MT5398
UINT32 bTDS_Curve_Points[TDS_BAND_NUM][TSD_CURVE_POINTSNUM];
UINT32 bTDS_Curve_Slopes[TDS_BAND_NUM][TSD_CURVE_SLOPESNUM];
UINT8 bTDS_UpdateFlg[TDS_BAND_NUM];
#endif

#endif

#ifdef CC_MT5398
static REGTBL_T const REGTBL_HSHARP[] =
{
	{HSHARP_00, 0x804C4440, 0xF0FFFFFF},
	{HSHARP_01, 0x004C4440, 0x80FFFFFF},
	{HSHARP_02, 0x00020202, 0x70FFFFFF},
	{HSHARP_03, 0x00FFFFFF, 0x70FFFFFF},
	{HSHARP_04, 0x00FFFFFF, 0x70FFFFFF},
	{HSHARP_05, 0xF000FFFF, 0xFFC0FFFF},
	{HSHARP_06, 0x30000808, 0xF3FFFFFF},
	{HSHARP_07, 0x00444340, 0x00FFFFFF},
	{HSHARP_08, 0x00444340, 0x00FFFFFF},
	{HSHARP_09, 0x00010101, 0x70FFFFFF},
	{HSHARP_0A, 0x00FFFFFF, 0x70FFFFFF},
	{HSHARP_0B, 0x00FFFFFF, 0x70FFFFFF},
	{HSHARP_0C, 0x00001800, 0x00001800},
	{HSHARP_0E, 0x00000000, 0xFFFFFF00},
	{HSHARP_0F, 0x00000000, 0xFFFFFF0F},
	{HSHARP_10, 0x00800000, 0x1FFF0000},
	{REGTBL_END, 0x00000000, 0x00000000},
};
#endif 	

static REGTBL_T const REGTBL_TDSHARP[] =
{
#ifdef CC_MT5398
	{        TDSHP_00, 0x80000408, 0x80000408},
	{        TDSHP_01, 0xA0010010, 0xBFFFFFFF},
	{        TDSHP_02, 0x20410110, 0x3FFFFFFF},
	{        TDSHP_03, 0xFFFF4010, 0xFFFFFFFF},
	{        TDSHP_04, 0x40000000, 0xFF000000},
	{        TDSHP_05, 0xA300E800, 0xFFFFFFFF},
	{        TDSHP_06, 0x0A500A0A, 0x0FFFFFFF},
	{        TDSHP_07, 0x00FFFA10, 0x01FFFFBF},
	{        TDSHP_08, 0x02020002, 0xFFFFFFFF},
	{        TDSHP_09, 0x02000202, 0xFFFFFFFF},
	{        TDSHP_0A, 0x02020200, 0xFFFFFF00},
	{        TDSHP_0B, 0x00000000, 0xFFFFFFFF},
	{        TDSHP_0C, 0x00000A00, 0xFFF00BE0},
	{        TDSHP_0D, 0x00000000, 0xFFFFFFFF},
	{        TDSHP_0E, 0x00000000, 0xFFFFFFFF},
	{        TDSHP_0F, 0x00000000, 0xFFFFFF00},
	{        TDSHP_10, 0x40600040, 0xFFFFFFFF},
	{        TDSHP_11, 0x60004060, 0xFFFFFFFF},
	{        TDSHP_12, 0x40604000, 0xFFFFFF00},
	{        TDSHP_13, 0x20202010, 0xFFFFFFFF},
	{        TDSHP_14, 0x2020FF20, 0xFFFFFFFF},
	{        TDSHP_15, 0x10FFFF00, 0xFFFFFF00},
	{        TDSHP_16, 0x20202010, 0xFFFFFFFF},
	{        TDSHP_17, 0x2020FF20, 0xFFFFFFFF},
	{        TDSHP_18, 0x10FFFF00, 0xFFFFFF00},
	{        TDSHP_19, 0x44000000, 0xFFFFFFFF},
	{        TDSHP_1A, 0x44400000, 0xFFF00000},
	{        TDSHP_1B, 0x0405FF05, 0xFFFFFFFF},
	{        TDSHP_1C, 0x28282828, 0xFFFFFFFF},
	{        TDSHP_1D, 0x20FFFF00, 0xFFFFFF00},
	{        TDSHP_1E, 0x0405FF28, 0xFFFFFFFF},
	{        TDSHP_1F, 0x28288028, 0xFFFFFFFF},
	{        TDSHP_20, 0x20FFFF00, 0xFFFFFF00},
	{        TDSHP_21, 0xD0FFFF00, 0xDFFFFF00},
	{        TDSHP_22, 0xD0FFFF00, 0xDFFFFF00},
	{        TDSHP_23, 0x88060600, 0xDFFFFF00},
	{        TDSHP_24, 0x8A060600, 0xDFFFFF00},
	{        TDSHP_25, 0x90FFFF00, 0xDFFFFF00},
	{        TDSHP_26, 0x90FFFF00, 0xDFFFFF00},
	{        TDSHP_27, 0x00000206, 0xFFFFFFFF},
	{        TDSHP_28, 0x1818FFFF, 0xFFFFFFFF},
	{        TDSHP_29, 0xD000FF05, 0xDFFFFF0F},
	{        TDSHP_2A, 0x404A5C5D, 0xFFFFFFFF},
	{        TDSHP_2B, 0x6568787D, 0xFFFFFFFF},
	{        TDSHP_2C, 0x807E7C79, 0xFFFFFFFF},
	{        TDSHP_2D, 0x78808070, 0xFFFFFFFF},
	{        TDSHP_2E, 0x4C0F4000, 0xFF3FC00C}, 
	{        TDSHP_2F, 0x00000004, 0x000000FF},	
	{        TDSHP_30, 0x40200001, 0x70FFF001},
	{        TDSHP_31, 0x0008DC21, 0x007FFFFF},
	{        TDSHP_32, 0x00159031, 0x007FFFFF},
	{        TDSHP_33, 0x00012BB6, 0x007FFFFF},
	{        TDSHP_34, 0x00000000, 0x007FFFFF},
	{        TDSHP_35, 0x00000000, 0x007FFFFF},
	{        TDSHP_36, 0x00000000, 0x007FFFFF},
	{        TDSHP_37, 0x00000000, 0x007FFFFF},
	{        TDSHP_38, 0x00000000, 0x007FFFFF},
#else
	{		 SHARP_00, 0x00000000, 0x00000040},
	{		 SHARP_31, 0x00050000, 0x000F0000},
#ifndef CUST_SOEM_DRV
	{		 SHARP_32, 0x00020050, 0x0003F0FF},
#else
    {        SHARP_32, 0x81450860, 0xFFFFFFFF},    
#endif
	{		 SHARP_36, 0x00020000, 0x00021F1F},
    {        SHARP_42, 0x0073F1F4, 0xFFFFFFFF},
    {        SHARP_43, 0x10000000, 0x1F000000},
    {        SHARP_44, 0x00001000, 0x00001F00},
    {        SHARP_45, 0x80FF80FF, 0xFFFFFFFF},    
    {        SHARP_46, 0x80FF80FF, 0xFFFFFFFF},  
    {        SHARP_4C, 0xA0000010, 0xFFFFFFFF},
    {        SHARP_4D, 0x20404110, 0xFFFFFFFF},
    {        SHARP_4E, 0xFFFF1010, 0xFFFFFFFF},
    {        SHARP_4F, 0x40000000, 0xFF000000},
    {        SHARP_5C, 0x80FF80FF, 0xFFFFFFFF},
    {        SHARP_5D, 0x000080FF, 0x0000DFFF},
#endif
	{        REGTBL_END, 0x00000000, 0x00000000}, 
};


#ifdef CC_MT5398   
static REGTBL_T const REGTBL_PBC[] =
{
	{     PBC_SHP1_01, 0x9010A0FF, 0x9FFFFFFF},
	{     PBC_SHP1_02, 0xA4110019, 0xFFFFFFFF},
	{     PBC_SHP1_03, 0x01003220, 0xFFF8FF3F},
	{     PBC_SHP1_04, 0x01808080, 0xFFFFFFFF},
	{     PBC_SHP1_05, 0x80808080, 0xFFFFFFFF},
	{     PBC_SHP1_06, 0x80808080, 0xFFFFFFFF},
	{     PBC_SHP1_07, 0x80808080, 0xFFFFFFFF},
	{     PBC_SHP1_08, 0x80808080, 0xFFFFFFFF},
	{     PBC_SHP1_09, 0x80808080, 0xFFFFFFFF},
	{     PBC_SHP2_01, 0x901060FF, 0x9FFFFFFF},
	{     PBC_SHP2_02, 0x6411002A, 0xFFFFFFFF},
	{     PBC_SHP2_03, 0x0108200C, 0xFFF8FF3F},
	{     PBC_SHP2_04, 0x01808080, 0xFFFFFFFF},
	{     PBC_SHP2_05, 0x80808080, 0xFFFFFFFF},
	{     PBC_SHP2_06, 0x80808080, 0xFFFFFFFF},
	{     PBC_SHP2_07, 0x80808080, 0xFFFFFFFF},
	{     PBC_SHP2_08, 0x80808080, 0xFFFFFFFF},
	{     PBC_SHP2_09, 0x80808080, 0xFFFFFFFF},
	{     PBC_SHP3_01, 0x901070FF, 0x9FFFFFFF},
	{     PBC_SHP3_02, 0xEE110030, 0xFFFFFFFF},
	{     PBC_SHP3_03, 0x0108200C, 0xFFF8FF3F},
	{     PBC_SHP3_04, 0x01808080, 0xFFFFFFFF},
	{     PBC_SHP3_05, 0x80808080, 0xFFFFFFFF},
	{     PBC_SHP3_06, 0x80808080, 0xFFFFFFFF},
	{     PBC_SHP3_07, 0x80808080, 0xFFFFFFFF},
	{     PBC_SHP3_08, 0x80808080, 0xFFFFFFFF},
	{     PBC_SHP3_09, 0x80808080, 0xFFFFFFFF},
	{     PBC_CTRL_00, 0x48008000, 0xEFFFF000},
	{         REGTBL_END, 0x00000000, 0x00000000},
};	
#else  
static REGTBL_T const REGTBL_PBC[] =
{
	//PBC1 color range initial
	{         CDS_00, 0x80800A20, 0x0000FFFF},
	{         CDS_01, 0x7D048199, 0xFFFFFFFF},
	{		  CDS_02, 0xC2CAC080, 0xFFFFF0FF},
	{		  CDS_03, 0x00C03220, 0xFFF00000},
	//PBC2 color range initial
	{         CDS_2A, 0x800C0A0A, 0x0000FFFF},
	{         CDS_2B, 0x5406000C, 0xFFFFFFFF},
	{		  CDS_2C, 0x62C85080, 0xFFFFF0FF},
	{		  CDS_2D, 0x00C82020, 0xFFFFFFFF},
	//PBC3 color range initial
	{         CDS_54, 0x800C0A0A, 0x0000FFFF},
	{         CDS_55, 0xB806000C, 0xFFFFFFFF},
	{		  CDS_56, 0xE3987080, 0xFFFFF0FF},
	{		  CDS_57, 0x00C03220, 0xFFF00000},
	{         REGTBL_END, 0x00000000, 0x00000000},
};
#endif

#ifdef CC_MT5398
static REGTBL_T const REGTBL_ECTIF[] =
{	
	{         ECTIF_00, 0x00001A10, 0xD0001A78},																																					
	{         ECTIF_01, 0x00033714, 0x07777FFF},																																					
	{         ECTIF_02, 0x1A0C0A01, 0xFFFFFF07},																																					
	{         ECTIF_03, 0x1A0C0A01, 0xFFFFFF07},																																					
	{         ECTIF_04, 0x0C080041, 0xFFFFFF77},																																					
	{         ECTIF_05, 0x02000000, 0xFF000000},
	{         REGTBL_END, 0x00000000, 0x00000000},
};
#endif

static REGTBL_T const REGTBL_ECTIR[] =
{
	{         ECTI_00, 0x00001A61, 0x10001A79},
	{         ECTI_01, 0x00003714, 0x07777FFF},
	{         ECTI_02, 0x1A0C0A05, 0xFFFFFF07},
	{         ECTI_03, 0x1A0C0A05, 0xFFFFFF07},
	{         ECTI_04, 0x40000001, 0xFFFFFF77},
	{         ECTI_05, 0x0000000B, 0x000000FF},
	{         REGTBL_END, 0x00000000, 0x00000000},
};

#ifdef CC_MT5398
static REGTBL_T const REGTBL_CBE[] = 
{
	{   CBE_00, 0x00004040, 0x0F7F7F7F},
	{   CBE_01, 0x00000416, 0x00007F7F},
	{   CBE_02, 0x2020083F, 0x00007F7F},
	{   REGTBL_END, 0x00000000, 0x00000000}
};
#endif


/*****************************************************************************************/
/*************************************   CTI   ******************************************/
/*****************************************************************************************/
/**
 * @brief CTIF on/off
 * @param bOnOff SV_ON/SV_OFF
 */

void vDrvCTIFOnOff(UINT8 bOnOff)
{
#ifdef CC_MT5398
	//CTIF Fail of 98 ES
	vIO32WriteFldAlign(ECTIF_00, bOnOff, ECTIF_ENA);
#endif
}

/**
 * @brief set main path CTIF initial parameters
 */
void vDrvCTIFInit(void)
{
#ifdef CC_MT5398
	vDrvLoadRegTbl(REGTBL_ECTIF);
#endif
}

/**
 * @brief set CTIF parameters
 * @param prCTIParam CTIF parameters structure
 */
void vDrvCTIFParam(void)
{
#ifdef CC_MT5398
    vDrvCTIFOnOff(wReadQualityTable(QUALITY_CTIF_ENA));

	vIO32WriteFldAlign(ECTIF_00, wReadQualityTable(QUALITY_CTIF_HD),
	                  ECTIF_HD);
	vIO32WriteFldAlign(ECTIF_01, wReadQualityTable(QUALITY_CTIF_LPF1),
	                  ECTIF_LPF1);
	vIO32WriteFldAlign(ECTIF_01, wReadQualityTable(QUALITY_CTIF_LPF1_SEL),
	                  ECTIF_LPF1_SEL);
	vIO32WriteFldAlign(ECTIF_00, wReadQualityTable(QUALITY_CTIF_FLPF),
	                  ECTIF_FLPF);
	vIO32WriteFldAlign(ECTIF_00, wReadQualityTable(QUALITY_CTIF_FLPF_SEL),
	                  ECTIF_FLPF_SEL);
	vIO32WriteFldAlign(ECTIF_04, wReadQualityTable(QUALITY_CTIF_FLAT_GAIN),
	                  ECTIF_FLAT_GAIN);
	vIO32WriteFldAlign(ECTIF_04, wReadQualityTable(QUALITY_CTIF_FLAT_OFST1),
	                  ECTIF_FLAT_OFST1);
	vIO32WriteFldAlign(ECTIF_04, wReadQualityTable(QUALITY_CTIF_FLAT_OFST2),
	                  ECTIF_FLAT_OFST2);
	vIO32WriteFldAlign(ECTIF_02, wReadQualityTable(QUALITY_CTIF_U_WND_SZ),
	                  ECTIF_U_WND_SZ);
	vIO32WriteFldAlign(ECTIF_03, wReadQualityTable(QUALITY_CTIF_V_WND_SZ),
	                  ECTIF_V_WND_SZ);
	vIO32WriteFldAlign(ECTIF_02, wReadQualityTable(QUALITY_CTIF_U_STB_GAIN),
	                  ECTIF_U_STB_GAIN);
	vIO32WriteFldAlign(ECTIF_03, wReadQualityTable(QUALITY_CTIF_V_STB_GAIN),
	                  ECTIF_V_STB_GAIN);
	vIO32WriteFldAlign(ECTIF_02, wReadQualityTable(QUALITY_CTIF_U_STB_OFST1),
	                  ECTIF_U_STB_OFST1);
	vIO32WriteFldAlign(ECTIF_03, wReadQualityTable(QUALITY_CTIF_V_STB_OFST1),
	                  ECTIF_V_STB_OFST1);
	vIO32WriteFldAlign(ECTIF_02, wReadQualityTable(QUALITY_CTIF_U_STB_OFST2),
	                  ECTIF_U_STB_OFST2);
	vIO32WriteFldAlign(ECTIF_03, wReadQualityTable(QUALITY_CTIF_V_STB_OFST2),
	                  ECTIF_V_STB_OFST2);
	vIO32WriteFldAlign(ECTIF_05, wReadQualityTable(QUALITY_CTIF_COR),
	                  ECTIF_COR);
#endif	                  
}

/**
 * @brief CTIR on/off
 * @param bOnOff SV_ON/SV_OFF
 */
void vDrvCTIROnOff(UINT8 bOnOff)
{
    UINT8 bModuleOnOff = bOnOff && (!bForceAllPQOff) && wReadQualityTable(QUALITY_MODULE_ONOFF_CTI);           
	vIO32WriteFldAlign(ECTI_00, bModuleOnOff, ECTI_ENA);
}

/**
 * @brief set main path CTIF initial parameters
 */
void vDrvCTIRInit(void)
{
	vDrvLoadRegTbl(REGTBL_ECTIR);
}

/**
 * @brief set CTIR parameters
 * @param prCTIParam CTIF parameters structure
 */
void vDrvCTIRParam(void)
{
    vDrvCTIROnOff(wReadQualityTable(QUALITY_CTIR_ENA));
	vIO32WriteFldAlign(ECTI_00, wReadQualityTable(QUALITY_CTIR_HD),
	                  ECTI_HD);
	vIO32WriteFldAlign(ECTI_01, wReadQualityTable(QUALITY_CTIR_LPF1),
	                  ECTI_LPF1);
	vIO32WriteFldAlign(ECTI_01, wReadQualityTable(QUALITY_CTIR_LPF1_SEL),
	                  ECTI_LPF1_SEL);
	vIO32WriteFldAlign(ECTI_00, wReadQualityTable(QUALITY_CTIR_FLPF),
	                  ECTI_FLPF);
	vIO32WriteFldAlign(ECTI_00, wReadQualityTable(QUALITY_CTIR_FLPF_SEL),
	                  ECTI_FLPF_SEL);
	vIO32WriteFldAlign(ECTI_04, wReadQualityTable(QUALITY_CTIR_FLAT_GAIN),
	                  ECTI_FLAT_GAIN);
	vIO32WriteFldAlign(ECTI_04, wReadQualityTable(QUALITY_CTIR_FLAT_OFST1),
	                  ECTI_FLAT_OFST1);
	vIO32WriteFldAlign(ECTI_04, wReadQualityTable(QUALITY_CTIR_FLAT_OFST2),
	                  ECTI_FLAT_OFST2);
	vIO32WriteFldAlign(ECTI_02, wReadQualityTable(QUALITY_CTIR_U_WND_SZ),
	                  ECTI_U_WND_SZ);
	vIO32WriteFldAlign(ECTI_03, wReadQualityTable(QUALITY_CTIR_V_WND_SZ),
	                  ECTI_V_WND_SZ);
	vIO32WriteFldAlign(ECTI_02, wReadQualityTable(QUALITY_CTIR_U_STB_GAIN),
	                  ECTI_U_STB_GAIN);
	vIO32WriteFldAlign(ECTI_03, wReadQualityTable(QUALITY_CTIR_V_STB_GAIN),
	                  ECTI_V_STB_GAIN);
	vIO32WriteFldAlign(ECTI_02, wReadQualityTable(QUALITY_CTIR_U_STB_OFST1),
	                  ECTI_U_STB_OFST1);
	vIO32WriteFldAlign(ECTI_03, wReadQualityTable(QUALITY_CTIR_V_STB_OFST1),
	                  ECTI_V_STB_OFST1);
	vIO32WriteFldAlign(ECTI_02, wReadQualityTable(QUALITY_CTIR_U_STB_OFST2),
	                  ECTI_U_STB_OFST2);
	vIO32WriteFldAlign(ECTI_03, wReadQualityTable(QUALITY_CTIR_V_STB_OFST2),
	                  ECTI_V_STB_OFST2);
	vIO32WriteFldAlign(ECTI_05, wReadQualityTable(QUALITY_CTIR_COR),
	                  ECTI_COR);
	                  
}

void vDrvHueTieOnOff(UINT8 bOnOff)
{
    vRegWriteFldAlign(ECTI_00, bOnOff, ECTI_HUE_TIE);
    #if defined(CC_MT5398) 
    vRegWriteFldAlign(ECTIF_00, bOnOff, ECTIF_HUE_TIE);
    #endif
}

#ifdef CC_MT5398
/*****************************************************************************************/
/*************************************   CBE   ******************************************/
/*****************************************************************************************/
/**
 * @brief CTIF on/off
 * @param bOnOff SV_ON/SV_OFF
 */
void vDrvCBEOnOff(UINT8 bOnOff)
{
    VRM_INFO_T in,out;
    UINT8 bLineInterleave;
	UINT8 bCBEBypass;
    UINT32   u4SCL  = u4DrvTDTVScalerModeQuery();
    E_TD_OUT eSCLOut= TD_SCL_OUT(u4SCL);
    E_TD_IN  eSCLIn = TD_SCL_IN(u4SCL);

    //scaler out line interleave or not
    bLineInterleave =((E_TD_OUT_3D_LI==eSCLOut)||
                     ((E_TD_IN_LI_P==eSCLIn)&&(E_TD_OUT_NATIVE==eSCLOut))); 

    //get fsc in & fsc out info
    VRMGetModuleVRMInfo(SV_VP_MAIN, VRM_MODULE_FSC, &in, &out);

    //Turn off this function when  front scaler out is
    //a. line interleave
    //b. width > 1920
    //c. height > 1080
    bCBEBypass = (out.u2Width > 1920  || 
                 out.u2Height > 1080 || 
                 bLineInterleave);
    vIO32WriteFldAlign(CBE_05, bCBEBypass, CBE_BYPASS);
	vIO32WriteFldAlign(CBE_00, bOnOff, CBEENABLE);
}

/**
 * @brief set main path CTIF initial parameters
 */
void vDrvCBEInit(void)
{
	vDrvLoadRegTbl(REGTBL_CBE);
}

/**
 * @brief set CBE parameters
 * @param void
 */
void vDrvCBELoadParam(void)
{
	vIO32WriteFldAlign(CBE_00,wReadQualityTable(QUALITY_CBE_WEI_IT3),CBE_W_IT3);
	vIO32WriteFldAlign(CBE_00,wReadQualityTable(QUALITY_CBE_WEI_IT5),CBE_W_IT5);
}
#endif


/*****************************************************************************************/
/*************************************   AC Transform  ***************************************/
/*****************************************************************************************/


#ifndef CC_MT5398
/**
 * @brief partialy update AC curve.
 * @param bType, bLimit, bLimit_Ratio, bBound
 * @dependency : Gain, Coring should be set before limit
 */
#define COR_RATIO_UNIT 0x10
#define GAIN_UNIT 0x20
#define LIMIT_RATIO_UNIT 0x8

static UINT8 bSetTDSharp(UINT8 bType, UINT8 bCor_Zero, UINT8 bCor_Ratio, UINT8 bCor_Th,
						 UINT8 bGain, UINT8 bLimit, UINT8 bLimit_Ratio, UINT8 bBound)
{
	TDSPoint P1, P2, P3, P4;
	INT32 wRatioS01, wRatioS12, wRatioS23, wRatioS34, wCor_Ratio, wLimit_Ratio, wGain;

	
	wCor_Ratio=(INT32)bCor_Ratio;
	wLimit_Ratio=(INT32)bLimit_Ratio;

	if(bType>=TDS_BAND_NUM)
	{	
		LOG(1, "\n\nError Type for sharpness Limit setting\n\n");
		return SV_FALSE;
	}
	LOG(2, "\n\n\nSharpness Parameters Input Type : %d\n", bType); 
	LOG(2, "bCor_Zero : %d, \n",bCor_Zero);
	LOG(2, "bCor_Ratio : %d, \n",bCor_Ratio);
	LOG(2, "bCor_Th : %d \n",bCor_Th);
	LOG(2, "bGain : %d \n",bGain);
	LOG(2, "bLimit : %d, \n",bLimit);
	LOG(2, "bLimit_Ratio : %d, \n",bLimit_Ratio);
	LOG(2, "bBound : %d, n", bBound);


	//resolution alignment with Cobra limit, bound (in 8 bit resolution)
	//bLimit=(bLimit>=128?0xFF:bLimit*2);
	//bBound=(bBound>=128?0xFF:bBound*2);

	/*Conditional check : 
	//a. bCor_Ratio, bLimit_Ratio must < unit ratio, and x gain//bCor_Ratio must < bGain
	//b. //bLimit_Ratio must < bGain
	//c. when bCor_Th < bCor_Zero, bCor_Zero works and bCor_Ratio 0
	*/
	if(bCor_Zero>=bCor_Th)
	{
		bCor_Th=bCor_Zero;
		wCor_Ratio=0;
	}
	if(bLimit>=bBound)
	{
		bLimit = bBound;
		wLimit_Ratio=0;
	}
	
	wGain=MIN(0x7F, bGain); // Max 4 times AC		
	wCor_Ratio=MIN(wCor_Ratio,COR_RATIO_UNIT);
	wLimit_Ratio=MIN(wLimit_Ratio,LIMIT_RATIO_UNIT);
	wCor_Ratio=(wGain*wCor_Ratio);
	wLimit_Ratio=(wGain*wLimit_Ratio);
	
	P1.bX = bCor_Zero;
	P1.bY = 0;
	P2.bX = bCor_Th;
	P2.bY = (((UINT16)bCor_Th-bCor_Zero)*wCor_Ratio*2+(COR_RATIO_UNIT*GAIN_UNIT))/(2*COR_RATIO_UNIT*GAIN_UNIT);
	if(P2.bY>=0xFF)
	{
		P2.bY=0xFF;
		wCor_Ratio=(0xFF*COR_RATIO_UNIT*GAIN_UNIT*2+((UINT16)bCor_Th-bCor_Zero))/(2*((UINT16)bCor_Th-bCor_Zero));
	}

	//resolution alignment with P2, P3
	//P1, P2 is 8 bit LSB in 12 bit resolution, P3, P4 is 8 bit LSB in 9 bit resolution
	if(wGain)
	{
		bLimit=MAX(bLimit, (P2.bY>>3));
		P3.bX = (UINT16)(bCor_Th>>3) + ((UINT16)(bLimit-(P2.bY>>3))*GAIN_UNIT*2+wGain)/(2*wGain);
		P3.bY = bLimit;
	}
	else
	{
		P3.bX = bLimit;
		P3.bY = P2.bY>>3;
	}
	if(wLimit_Ratio)
	{
		P4.bX = P3.bX+((bBound-bLimit)*LIMIT_RATIO_UNIT*GAIN_UNIT*2+wLimit_Ratio)/(2*wLimit_Ratio);
		P4.bY = bBound;
	}
	else
	{
		P4.bX=0xFF;
		P4.bY=P3.bY;
	}

	//Range check.
	if(P3.bX>0xFF)
	{
		P3.bX=0xFF;
		P4.bX=0xFF;
		P3.bY=(P2.bY>>3)+((UINT16)(0xFF-(bCor_Th>>3))*wGain*2+GAIN_UNIT)/(2*GAIN_UNIT);
		P4.bY=P3.bY;
		wLimit_Ratio=0;
	}
	else if(P4.bX>0xFF)
	{
		P4.bX=0xFF;
		P4.bY=P3.bY+(wLimit_Ratio*(0xFF-P3.bX)*2+(LIMIT_RATIO_UNIT*GAIN_UNIT))/(2*LIMIT_RATIO_UNIT*GAIN_UNIT);
	}

	wRatioS01 = 0;
	wRatioS12 = (wCor_Ratio*4096+COR_RATIO_UNIT*GAIN_UNIT)/(2*COR_RATIO_UNIT*GAIN_UNIT);
	wRatioS23 = (wGain*4096+GAIN_UNIT)/(2*GAIN_UNIT);
	wRatioS34 = (wLimit_Ratio*4096+LIMIT_RATIO_UNIT*GAIN_UNIT)/(2*LIMIT_RATIO_UNIT*GAIN_UNIT);

	//Protection
	vIO32WriteFldAlign(TDSHP_GLOBAL, SV_ON, TDS_WRT_PROTECT);
	
	bTDS_Curve_Points[bType][0]=(P1.bX<<24)+(P1.bY<<16)+(P2.bX<<8)+(P2.bY);
	bTDS_Curve_Points[bType][1]=(P3.bX<<24)+(P3.bY<<16)+(P4.bX<<8)+(P4.bY);
	
	bTDS_Curve_Slopes[bType][0]=(wRatioS01<<16)+wRatioS12;
	bTDS_Curve_Slopes[bType][1]=(wRatioS23<<16)+wRatioS34;

	bTDS_UpdateFlg[bType] = SV_ON;
	
	vIO32WriteFldAlign(TDSHP_GLOBAL, SV_OFF, TDS_WRT_PROTECT);
	vApiRegisterVideoEvent(PE_EVENT_SHARPNESS, SV_VP_MAIN, SV_ON);
	
	LOG(2, "Set Sharpness Parameters\n"); 
	LOG(2, "Pt1 : %d, %d, \n",P1.bX,P1.bY); 
	LOG(2, "Pt2 : %d, %d, \n",P2.bX,P2.bY); 
	LOG(2, "Pt3 : %d  %d, \n",P3.bX,P3.bY); 
	LOG(2, "Pt4 : %d  %d, \n",P4.bX,P4.bY); 
	LOG(2, "S01 : %d, \n",wRatioS01); 
	LOG(2, "S12 : %d, \n",wRatioS12); 
	LOG(2, "S23 : %d, \n",wRatioS23); 
	LOG(2, "S34 : %d, \n\n\n", wRatioS34);
 
	return SV_TRUE;
}
/**
 * @brief This function is called at VSYNC to update TDS curve.
 * @param 
 */

void vDrvLoadTDSCurve(void)
{
	UINT8 bType, bWriteRdy;
	UINT32 dwBaseAddr;
	
	bWriteRdy = (!IO32ReadFldAlign(TDSHP_GLOBAL, TDS_WRT_PROTECT));
	if(bWriteRdy)
	{
		for(bType=0; bType<TDS_BAND_NUM; bType++)
		{
			dwBaseAddr = TDS_CURVE_REGADDR[bType];
			if(bTDS_UpdateFlg[bType]==SV_ON)
			{
				vIO32Write4B(dwBaseAddr, 	   bTDS_Curve_Points[bType][0]);
				vIO32Write4B((dwBaseAddr+4),  bTDS_Curve_Points[bType][1]);	
				vIO32Write4B((dwBaseAddr+8),  bTDS_Curve_Slopes[bType][0]);
				vIO32Write4B((dwBaseAddr+0xC),bTDS_Curve_Slopes[bType][1]); 
			
				bTDS_UpdateFlg[bType]=SV_OFF;
			}
		}
	}	
}

#endif

static void vDrvSharpCalcuACCurve(UINT8 bStartBand, UINT8 bEndBand)
{
#ifndef CC_MT5398
	static UINT8 bPreSetting[TDS_BAND_NUM][TDS_CURVE_SW_REG_END];

    UINT8 bSetting[TDS_BAND_NUM][TDS_CURVE_SW_REG_END];

    UINT8 bBand, bItem, i;

    if (bStartBand > bEndBand)
    {
        LOG(2, "SharpCalc have wrong param, please check, bStartBand = %d, bEndBand = %d\n", bStartBand, bEndBand);
        return;
    }
    if (bEndBand >= TDS_ITEM_MAX)
    {
        LOG(2, "SharpCalc have wrong param, please check.bEndBand = %d is too large\n", bEndBand);
    }
    
    for (bBand=bStartBand; bBand<=bEndBand; bBand++)
    {
        for (bItem=0; bItem<TDS_CURVE_SW_REG_END; bItem++)
        {
            bSetting[bBand][bItem] = IO32ReadFldAlign(u4SWReg(TDS_Curve_SW_Reg_Base[bBand][bItem]), TDS_Curve_SW_Reg[bBand][bItem]);
        }
    }

    for (bBand=bStartBand; bBand<=bEndBand; bBand++)
    {
        for (bItem=0; bItem<TDS_CURVE_SW_REG_END; bItem++)
        {
            #ifndef CUST_SOEM_DRV
            if (bSetting[bBand][bItem] != bPreSetting[bBand][bItem])
            #else    
            if ((bSetting[bBand][bItem] != bPreSetting[bBand][bItem])||(fgSharpForce))
            #endif
            {
                bSetTDSharp(bBand, bSetting[bBand][TDS_CURVE_SW_REG_COR_ZERO], 
                                bSetting[bBand][TDS_CURVE_SW_REG_COR_RATIO], 
                                bSetting[bBand][TDS_CURVE_SW_REG_COR_TH], 
                                bSetting[bBand][TDS_CURVE_SW_REG_GAIN],
                                bSetting[bBand][TDS_CURVE_SW_REG_LIMIT], 
                                bSetting[bBand][TDS_CURVE_SW_REG_LIMIT_RATIO], 
                                bSetting[bBand][TDS_CURVE_SW_REG_LIMIT_BOUND]);
                for (i=0; i<TDS_CURVE_SW_REG_END; i++)
                {
                    bPreSetting[bBand][i] = bSetting[bBand][i];
                }
                break;
            }
        }
    }	
#endif
}

static void vDrv3DSharpParamCalcu(void)
{
	static UINT8 bPre3DGainH = 0x80;
	static UINT8 bPre3DGainD = 0x80;
	static UINT8 bPre3DGainV = 0x80;
	static UINT8 bPre3DClipH = 0x80;
	static UINT8 bPre3DClipD = 0x80;
	static UINT8 bPre3DClipV = 0x80;	
	UINT8 b3DHGain, b3DDGain, b3DVGain;
	UINT8 b3DHClip, b3DDClip, b3DVClip;
	
	vDrvGetSharp3DWeighting();
	b3DHGain = IO32ReadFldAlign(SHARPNESS_3D_13, SHARPNESS_3D_HGAIN);
	b3DDGain = IO32ReadFldAlign(SHARPNESS_3D_13, SHARPNESS_3D_DGAIN);
	b3DVGain = IO32ReadFldAlign(SHARPNESS_3D_13, SHARPNESS_3D_VGAIN);
	b3DHClip = IO32ReadFldAlign(SHARPNESS_3D_14, SHARPNESS_3D_HCLIP);
	b3DDClip = IO32ReadFldAlign(SHARPNESS_3D_14, SHARPNESS_3D_DCLIP);
	b3DVClip = IO32ReadFldAlign(SHARPNESS_3D_14, SHARPNESS_3D_VCLIP);
    #ifndef CUST_SOEM_DRV
    if( (b3DHGain!=bPre3DGainH)||
		(b3DDGain!=bPre3DGainD)||
		(b3DVGain!=bPre3DGainV))
	#else	
	if( (b3DHGain!=bPre3DGainH)||
		(b3DDGain!=bPre3DGainD)||
		(b3DVGain!=bPre3DGainV)||
		(fgSharpForce))
	#endif
	{
		vDrvPostSharpParamHorizontal();
		vDrvPostSharpParamVertical();
		vDrvPostSharpParamDiagonal();
		bPre3DGainH = b3DHGain;
		bPre3DGainD = b3DDGain;
		bPre3DGainV = b3DVGain;
	}
	if(	(bPre3DClipH!=b3DHClip)||
		(bPre3DClipD!=b3DDClip)||
		(bPre3DClipV!=b3DVClip))
	{
		//Group Clip	   
#ifdef CC_MT5398
	  
		vIO32WriteFldAlign(TDSHP_21, wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_CLIP_RATIO_H),b3DHClip)
							       , TDS_CLIP_RATIO_H);
		vIO32WriteFldAlign(TDSHP_22, wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_CLIP_RATIO_V),b3DVClip)
								   , TDS_CLIP_RATIO_V);				  
		vIO32WriteFldAlign(TDSHP_23, wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_CLIP_RATIO_D),b3DDClip)
								   , TDS_CLIP_RATIO_D);				  
#else
		vIO32WriteFldAlign(SHARP_45, wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_CLIP_RATIO_H),b3DHClip)
								   , TDS_CLIP_RATIO_LH);			  
		vIO32WriteFldAlign(SHARP_45, wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_CLIP_RATIO_D),b3DDClip)
								   , TDS_CLIP_RATIO_MD); 
		vIO32WriteFldAlign(SHARP_46, wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_CLIP_RATIO_V),b3DVClip)
								   , TDS_CLIP_RATIO_HV);
#endif
		bPre3DClipH = b3DHClip;
		bPre3DClipD = b3DDClip;
		bPre3DClipV = b3DVClip;
	}
}

inline void vDrvHCalcuACCurve(void)
{
#ifndef CC_MT5398
	vDrvSharpCalcuACCurve(TDS_H1, TDS_H3);
#endif
}

inline void vDrvVCalcuACCurve(void)
{
#ifndef CC_MT5398
    vDrvSharpCalcuACCurve(TDS_V1, TDS_V3);	
#endif
}

inline void vDrvDCalcuACCurve(void)
{
#ifndef CC_MT5398
    vDrvSharpCalcuACCurve(TDS_D1, TDS_D3);	
#endif
}

inline void vDrvLTICalcuACCurve(void)
{
    vDrvSharpCalcuACCurve(TDS_LTI_HL, TDS_LTI_V);
}

inline void vDrvPBCCalcuACCurve(void)
{
    vDrvSharpCalcuACCurve(CDS1_H1, CDS3_D3);
}

//**   Remove LTI
/*****************************************************************************************/
/*************************************   LTI  ***********************************************/
/*****************************************************************************************/
/**
 * @brief LTI on/off
 * @param bOnOff SV_ON/SV_OFF
 */

#ifdef CC_MT5398
void vDrvLTIH1Gain(UINT8 u1Gain)
{
    UINT8 bModuleOnOff = (!bForceAllPQOff) && wReadQualityTable(QUALITY_MODULE_ONOFF_LTI);                

    if(bModuleOnOff == SV_OFF)
    {
        vIO32WriteFldAlign(TDSHP_12, 0, TDS_GAIN_LTI_H1);    
    }
    else
    {
        vIO32WriteFldAlign(TDSHP_12, u1Gain, TDS_GAIN_LTI_H1); 
    }
}

void vDrvLTIH2Gain(UINT8 u1Gain)
{
    UINT8 bModuleOnOff = (!bForceAllPQOff) && wReadQualityTable(QUALITY_MODULE_ONOFF_LTI);                

    if(bModuleOnOff == SV_OFF)
    {
        vIO32WriteFldAlign(TDSHP_12, 0, TDS_GAIN_LTI_H2);    
    }
    else
    {
        vIO32WriteFldAlign(TDSHP_12, u1Gain, TDS_GAIN_LTI_H2); 
    }    
}

void vDrvLTIVGain(UINT8 u1Gain)
{ 
    UINT8 bModuleOnOff = (!bForceAllPQOff) && wReadQualityTable(QUALITY_MODULE_ONOFF_LTI);                

    if(bModuleOnOff == SV_OFF)
    {
        vIO32WriteFldAlign(TDSHP_12, 0, TDS_GAIN_LTI_V);    
    }
    else
    {
        vIO32WriteFldAlign(TDSHP_12, u1Gain, TDS_GAIN_LTI_V); 
    }    
}
#endif
 
void vDrvLTIOnOff(UINT8 bOnOff)
{
#ifdef CC_MT5398
	if(bOnOff)
	{
    	vDrvLTIH1Gain(wReadQualityTable(QUALITY_LTI_GAIN_LTI_H1));
    	vDrvLTIH2Gain(wReadQualityTable(QUALITY_LTI_GAIN_LTI_H2));
    	vDrvLTIVGain(wReadQualityTable(QUALITY_LTI_GAIN_LTI_V));    		      
	}
	else
	{
    	vDrvLTIH1Gain(0);
    	vDrvLTIH2Gain(0);
    	vDrvLTIVGain(0);    	
	}
#else
	vIO32WriteFldAlign(SHARP_00, (!bOnOff), TDS_BYPASS_LTI_V);	 
	vIO32WriteFldAlign(SHARP_00, (!bOnOff), TDS_BYPASS_LTI_H1);	 
	vIO32WriteFldAlign(SHARP_00, (!bOnOff), TDS_BYPASS_LTI_H2);		
#endif
}

/**
 * @brief set LTI parameters
 * @param prLTIParam LTi parameters structure
 */
void vDrvLTIParam(void)
{
#ifdef CC_MT5398
    vIO32WriteFldAlign(TDSHP_00, wReadQualityTable(QUALITY_LTI_FLT_LTI_H1), TDS_FLT_LTI_H1);        
    vIO32WriteFldAlign(TDSHP_0A, wReadQualityTable(QUALITY_LTI_COR_ZERO_LTI_H1), TDS_COR_ZERO_LTI_H1);   
    vIO32WriteFldAlign(TDSHP_0C, wReadQualityTable(QUALITY_LTI_COR_RATIO_LTI_H1), TDS_COR_RATIO_LTI_H1);  
    vIO32WriteFldAlign(TDSHP_0F, wReadQualityTable(QUALITY_LTI_COR_THR_LTI_H1), TDS_COR_THR_LTI_H1);    

    vDrvLTIH1Gain(wReadQualityTable(QUALITY_LTI_GAIN_LTI_H1));

    vIO32WriteFldAlign(TDSHP_LIMIT_02, wReadQualityTable(QUALITY_LTI_LIMIT_LTI_H1), LTI_LIMIT_LTI_H1);      
    vIO32WriteFldAlign(TDSHP_1A, wReadQualityTable(QUALITY_LTI_LIMIT_RATIO_LTI_H1), TDS_LIMIT_RATIO_LTI_H1);
    vIO32WriteFldAlign(TDSHP_LIMIT_05, wReadQualityTable(QUALITY_LTI_BOUND_LTI_H1), LTI_BOUND_LTI_H1);      
    vIO32WriteFldAlign(TDSHP_24, wReadQualityTable(QUALITY_LTI_CLIP_RATIO_LTI_H1), TDS_CLIP_RATIO_LTI_H1); 
    vIO32WriteFldAlign(TDSHP_24, wReadQualityTable(QUALITY_LTI_CLIP_POS_LTI_H1), TDS_CLIP_POS_LTI_H1);   
    vIO32WriteFldAlign(TDSHP_24, wReadQualityTable(QUALITY_LTI_CLIP_NEG_LTI_H1), TDS_CLIP_NEG_LTI_H1);   
    vIO32WriteFldAlign(TDSHP_00, wReadQualityTable(QUALITY_LTI_FLT_LTI_H2), TDS_FLT_LTI_H2);        
    vIO32WriteFldAlign(TDSHP_0A, wReadQualityTable(QUALITY_LTI_COR_ZERO_LTI_H2), TDS_COR_ZERO_LTI_H2);   
    vIO32WriteFldAlign(TDSHP_0C, wReadQualityTable(QUALITY_LTI_COR_RATIO_LTI_H2), TDS_COR_RATIO_LTI_H2);  
    vIO32WriteFldAlign(TDSHP_0F, wReadQualityTable(QUALITY_LTI_COR_THR_LTI_H2), TDS_COR_THR_LTI_H2);    

    vDrvLTIH2Gain(wReadQualityTable(QUALITY_LTI_GAIN_LTI_H2));

    vIO32WriteFldAlign(TDSHP_LIMIT_02, wReadQualityTable(QUALITY_LTI_LIMIT_LTI_H2), LTI_LIMIT_LTI_H2);      
    vIO32WriteFldAlign(TDSHP_1A, wReadQualityTable(QUALITY_LTI_LIMIT_RATIO_LTI_H2), TDS_LIMIT_RATIO_LTI_H2);
    vIO32WriteFldAlign(TDSHP_LIMIT_05 , wReadQualityTable(QUALITY_LTI_BOUND_LTI_H2), LTI_BOUND_LTI_H2);      
    vIO32WriteFldAlign(TDSHP_25, wReadQualityTable(QUALITY_LTI_CLIP_RATIO_LTI_H2), TDS_CLIP_RATIO_LTI_H2); 
    vIO32WriteFldAlign(TDSHP_25, wReadQualityTable(QUALITY_LTI_CLIP_POS_LTI_H2), TDS_CLIP_POS_LTI_H2);   
    vIO32WriteFldAlign(TDSHP_25, wReadQualityTable(QUALITY_LTI_CLIP_NEG_LTI_H2), TDS_CLIP_NEG_LTI_H2);   
    vIO32WriteFldAlign(TDSHP_00, wReadQualityTable(QUALITY_LTI_FLT_LTI_V), TDS_FLT_LTI_V);         
    vIO32WriteFldAlign(TDSHP_0A, wReadQualityTable(QUALITY_LTI_COR_ZERO_LTI_V), TDS_COR_ZERO_LTI_V);    
    vIO32WriteFldAlign(TDSHP_0C, wReadQualityTable(QUALITY_LTI_COR_RATIO_LTI_V), TDS_COR_RATIO_LTI_V);   
    vIO32WriteFldAlign(TDSHP_0F, wReadQualityTable(QUALITY_LTI_COR_THR_LTI_V), TDS_COR_THR_LTI_V);     

    vDrvLTIVGain(wReadQualityTable(QUALITY_LTI_GAIN_LTI_V));    

    vIO32WriteFldAlign(TDSHP_LIMIT_02 , wReadQualityTable(QUALITY_LTI_LIMIT_LTI_V), LTI_LIMIT_LTI_V);       
    vIO32WriteFldAlign(TDSHP_1A, wReadQualityTable(QUALITY_LTI_LIMIT_RATIO_LTI_V), TDS_LIMIT_RATIO_LTI_V); 
    vIO32WriteFldAlign(TDSHP_LIMIT_05, wReadQualityTable(QUALITY_LTI_BOUND_LTI_V), LTI_BOUND_LTI_V);       
    vIO32WriteFldAlign(TDSHP_26, wReadQualityTable(QUALITY_LTI_CLIP_RATIO_LTI_V), TDS_CLIP_RATIO_LTI_V);  
    vIO32WriteFldAlign(TDSHP_26, wReadQualityTable(QUALITY_LTI_CLIP_POS_LTI_V), TDS_CLIP_POS_LTI_V);    
    vIO32WriteFldAlign(TDSHP_26, wReadQualityTable(QUALITY_LTI_CLIP_NEG_LTI_V), TDS_CLIP_NEG_LTI_V);
#else

	vIO32WriteFldMulti(SHARP_00, P_Fld(wReadQualityTable(QUALITY_LTI_FLT_LTI_H1), TDS_FLT_LTI_H1)|
								 P_Fld(wReadQualityTable(QUALITY_LTI_FLT_LTI_H2), TDS_FLT_LTI_H2)|
								 P_Fld(wReadQualityTable(QUALITY_LTI_FLT_LTI_V), TDS_FLT_LTI_V));

	vIO32WriteFldMulti(SHARP_5C, P_Fld(wReadQualityTable(QUALITY_LTI_CLIP_RATIO_LTI_H1), TDS_CLIP_RATIO_LTI_H1)|
								 P_Fld(wReadQualityTable(QUALITY_LTI_CLIP_RATIO_LTI_H2), TDS_CLIP_RATIO_LTI_H2)|
								 P_Fld(wReadQualityTable(QUALITY_LTI_CLIP_POS_LTI_H1), TDS_CLIP_THR_LTI_H1)|
								 P_Fld(wReadQualityTable(QUALITY_LTI_CLIP_POS_LTI_H2), TDS_CLIP_THR_LTI_H2));

	vIO32WriteFldMulti(SHARP_46, P_Fld(wReadQualityTable(QUALITY_LTI_CLIP_RATIO_LTI_V), TDS_CLIP_RATIO_LTI_V)|
								 P_Fld(wReadQualityTable(QUALITY_LTI_CLIP_POS_LTI_V), TDS_CLIP_THR_LTI_V));
    
    //SW Proc
    vIO32WriteFldAlign(TDSHP_GAIN_04, wReadQualityTable(QUALITY_LTI_GAIN_LTI_H1), TDSHP_GAIN_LTI_H1);
    vIO32WriteFldAlign(TDSHP_GAIN_04, wReadQualityTable(QUALITY_LTI_GAIN_LTI_H2), TDSHP_GAIN_LTI_H2);
    vIO32WriteFldAlign(TDSHP_GAIN_04, wReadQualityTable(QUALITY_LTI_GAIN_LTI_V), TDSHP_GAIN_LTI_V);
    
    vIO32WriteFldAlign(TDSHP_CORE_04, wReadQualityTable(QUALITY_LTI_COR_ZERO_LTI_H1), TDSHP_CORE_LTI_H1);
    vIO32WriteFldAlign(TDSHP_CORE_04, wReadQualityTable(QUALITY_LTI_COR_ZERO_LTI_H2), TDSHP_CORE_LTI_H2);
    vIO32WriteFldAlign(TDSHP_CORE_04, wReadQualityTable(QUALITY_LTI_COR_ZERO_LTI_V), TDSHP_CORE_LTI_V);

    vIO32WriteFldAlign(TDSHP_CORING_RATIO_04, wReadQualityTable(QUALITY_LTI_COR_RATIO_LTI_H1), TDSHP_CORE_RATIO_LTI_H1);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_04, wReadQualityTable(QUALITY_LTI_COR_RATIO_LTI_H2), TDSHP_CORE_RATIO_LTI_H2);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_04, wReadQualityTable(QUALITY_LTI_COR_RATIO_LTI_V), TDSHP_CORE_RATIO_LTI_V);

    vIO32WriteFldAlign(TDSHP_CORETH_04, wReadQualityTable(QUALITY_LTI_COR_THR_LTI_H1), TDSHP_CORETH_LTI_H1);
    vIO32WriteFldAlign(TDSHP_CORETH_04, wReadQualityTable(QUALITY_LTI_COR_THR_LTI_H2), TDSHP_CORETH_LTI_H2);
    vIO32WriteFldAlign(TDSHP_CORETH_04, wReadQualityTable(QUALITY_LTI_COR_THR_LTI_V), TDSHP_CORETH_LTI_V);

    vIO32WriteFldAlign(TDSHP_LIMIT_02, wReadQualityTable(QUALITY_LTI_LIMIT_LTI_H1), LTI_LIMIT_LTI_H1);
    vIO32WriteFldAlign(TDSHP_LIMIT_02, wReadQualityTable(QUALITY_LTI_LIMIT_LTI_H2), LTI_LIMIT_LTI_H2);
    vIO32WriteFldAlign(TDSHP_LIMIT_02, wReadQualityTable(QUALITY_LTI_LIMIT_LTI_V), LTI_LIMIT_LTI_V);

    vIO32WriteFldAlign(TDSHP_LIMIT_05, wReadQualityTable(QUALITY_LTI_BOUND_LTI_H1), LTI_BOUND_LTI_H1);
    vIO32WriteFldAlign(TDSHP_LIMIT_05, wReadQualityTable(QUALITY_LTI_BOUND_LTI_H2), LTI_BOUND_LTI_H2);
    vIO32WriteFldAlign(TDSHP_LIMIT_05, wReadQualityTable(QUALITY_LTI_BOUND_LTI_V), LTI_BOUND_LTI_V);

    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_04, wReadQualityTable(QUALITY_LTI_LIMIT_RATIO_LTI_H1), TDSHP_LIMIT_RATIO_LTI_H1);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_04, wReadQualityTable(QUALITY_LTI_LIMIT_RATIO_LTI_H2), TDSHP_LIMIT_RATIO_LTI_H2);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_04, wReadQualityTable(QUALITY_LTI_LIMIT_RATIO_LTI_V), TDSHP_LIMIT_RATIO_LTI_V);
#endif
}

/*****************************************************************************************/
/***************************************  HSHARP *****************************************/
/****************************************************************************************/
/**
 * @brief enable/disable main path HSHARPF
 * @param bOnOff SV_ON/SV_OFF
 */
void vDrvPreSharpOnOff(UINT8 bPath, UINT8 bOnOff)
{   
#ifdef CC_MT5398
    UINT8 bModuleOnOff = bOnOff && (!bForceAllPQOff) && wReadQualityTable(QUALITY_MODULE_ONOFF_SHARPNESS);            
    UNUSED(bPath);        
    vIO32WriteFldAlign(HSHARP_00, bModuleOnOff, SHARP_EN);
#endif
}

/**
 * @brief set main path HSHARP initial parameters
 */
void vDrvPreSharpInit(void)
{
#ifdef CC_MT5398
    vDrvLoadRegTbl(REGTBL_HSHARP);
#endif
}

/**
 * @brief set main path HSHARPF parameters
 */
void vDrvPreSharpParam(UINT8 bPath)
{
#ifdef CC_MT5398
//  Global Gain
	vIO32WriteFldAlign(HSHARP_00 ,
	                  wReadQualityTable(QUALITY_HSHARP_HIGH), SHARP_HIGH);
	vIO32WriteFldAlign(HSHARP_00 ,
	                  wReadQualityTable(QUALITY_HSHARP_MID), SHARP_MID);
	vIO32WriteFldAlign(HSHARP_00 ,
	                  wReadQualityTable(QUALITY_HSHARP_LOW), SHARP_LOW);
	vIO32WriteFldAlign(HSHARP_01 ,
	                  wReadQualityTable(QUALITY_HSHARP_HIGH_NEG), SHARP_HIGH_NEG);
	vIO32WriteFldAlign(HSHARP_01 ,
	                  wReadQualityTable(QUALITY_HSHARP_MID_NEG), SHARP_MID_NEG);
	vIO32WriteFldAlign(HSHARP_01 ,
	                  wReadQualityTable(QUALITY_HSHARP_LOW_NEG), SHARP_LOW_NEG);

//  Global Coring
	vIO32WriteFldAlign(HSHARP_02 ,
	                  wReadQualityTable(QUALITY_HSHARP_HIGH_CORING),
	                  SHARP_HIGH_CORING);
	vIO32WriteFldAlign(HSHARP_02 ,
	                  wReadQualityTable(QUALITY_HSHARP_MID_CORING),
	                  SHARP_MID_CORING);
	vIO32WriteFldAlign(HSHARP_02 ,
	                  wReadQualityTable(QUALITY_HSHARP_LOW_CORING),
	                  SHARP_LOW_CORING);

//  Global Limit
	vIO32WriteFldAlign(HSHARP_03 ,
	                  wReadQualityTable(QUALITY_HSHARP_HIGH_LIMIT_POS),
	                  SHARP_HIGH_LIMIT_POS);
	vIO32WriteFldAlign(HSHARP_03 ,
	                  wReadQualityTable(QUALITY_HSHARP_MID_LIMIT_POS),
	                  SHARP_MID_LIMIT_POS);
	vIO32WriteFldAlign(HSHARP_03 ,
	                  wReadQualityTable(QUALITY_HSHARP_LOW_LIMIT_POS),
	                  SHARP_LOW_LIMIT_POS);
	vIO32WriteFldAlign(HSHARP_04 ,
	                  wReadQualityTable(QUALITY_HSHARP_HIGH_LIMIT_NEG),
	                  SHARP_HIGH_LIMIT_NEG);
	vIO32WriteFldAlign(HSHARP_04 ,
	                  wReadQualityTable(QUALITY_HSHARP_MID_LIMIT_NEG),
	                  SHARP_MID_LIMIT_NEG);
	vIO32WriteFldAlign(HSHARP_04 ,
	                  wReadQualityTable(QUALITY_HSHARP_LOW_LIMIT_NEG),
	                  SHARP_LOW_LIMIT_NEG);

//  Global Clip
	vIO32WriteFldAlign(HSHARP_05 ,
	                  wReadQualityTable(QUALITY_HSHARP_CLIP_POS_EN),
	                  CLIP_POS_EN);
	vIO32WriteFldAlign(HSHARP_05 ,
	                  wReadQualityTable(QUALITY_HSHARP_CLIP_NEG_EN),
	                  CLIP_NEG_EN);
	vIO32WriteFldAlign(HSHARP_05 ,
	                  wReadQualityTable(QUALITY_HSHARP_GB_CLIP_POS_EN_H),
	                  GB_CLIP_POS_EN_H);
	vIO32WriteFldAlign(HSHARP_05 ,
	                  wReadQualityTable(QUALITY_HSHARP_GB_CLIP_NEG_EN_H),
	                  GB_CLIP_NEG_EN_H);
	vIO32WriteFldAlign(HSHARP_05 ,
	                  wReadQualityTable(QUALITY_HSHARP_GB_CLIP_POS_EN_M),
	                  GB_CLIP_POS_EN_M);
	vIO32WriteFldAlign(HSHARP_05 ,
	                  wReadQualityTable(QUALITY_HSHARP_GB_CLIP_NEG_EN_M),
	                  GB_CLIP_NEG_EN_M);
	vIO32WriteFldAlign(HSHARP_05 ,
	                  wReadQualityTable(QUALITY_HSHARP_GB_CLIP_POS_EN_L),
	                  GB_CLIP_POS_EN_L);
	vIO32WriteFldAlign(HSHARP_05 ,
	                  wReadQualityTable(QUALITY_HSHARP_GB_CLIP_NEG_EN_L),
	                  GB_CLIP_NEG_EN_L);
	vIO32WriteFldAlign(HSHARP_06 ,
	                  wReadQualityTable(QUALITY_HSHARP_CLIP_FRONT_THR_HIGH),
	                  CLIP_FRONT_THR_HIGH);
	vIO32WriteFldAlign(HSHARP_06 ,
	                  wReadQualityTable(QUALITY_HSHARP_CLIP_FRONT_THR_LOW),
	                  CLIP_FRONT_THR_LOW);
	vIO32WriteFldAlign(HSHARP_05 ,
	                  wReadQualityTable(QUALITY_HSHARP_CLIP_POS),
	                  CLIP_POS_TH);
	vIO32WriteFldAlign(HSHARP_05 ,
	                  wReadQualityTable(QUALITY_HSHARP_CLIP_NEG),
	                  CLIP_NEG_TH);
	vIO32WriteFldAlign(HSHARP_10 ,
	                  wReadQualityTable(QUALITY_HSHARP_SOFT_CLIP_GAIN),
	                  SHARP_SOFT_GAIN);
	

//  Local Gain
	vIO32WriteFldAlign(HSHARP_07 ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_HIGH),
	                  LC_SHARP_HIGH);
	vIO32WriteFldAlign(HSHARP_07 ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_MID), LC_SHARP_MID);
	vIO32WriteFldAlign(HSHARP_07 ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_LOW), LC_SHARP_LOW);
	vIO32WriteFldAlign(HSHARP_08 ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_HIGH_NEG),
	                  LC_SHARP_HIGH_NEG);
	vIO32WriteFldAlign(HSHARP_08 ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_MID_NEG), LC_SHARP_MID_NEG);
	vIO32WriteFldAlign(HSHARP_08 ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_LOW_NEG), LC_SHARP_LOW_NEG);
	
//  Local Coring
	vIO32WriteFldAlign(HSHARP_09 ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_HIGH_CORING),
	                  LC_SHARP_HIGH_CORING);
	vIO32WriteFldAlign(HSHARP_09 ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_MID_CORING),
	                  LC_SHARP_MID_CORING);
	vIO32WriteFldAlign(HSHARP_09 ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_LOW_CORING),
	                  LC_SHARP_LOW_CORING);

//  Local Limit
	vIO32WriteFldAlign(HSHARP_0A,
	                  wReadQualityTable(QUALITY_HSHARP_LC_HIGH_LIMIT_POS),
	                  LC_SHARP_HIGH_LIMIT_POS);
	vIO32WriteFldAlign(HSHARP_0A ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_MID_LIMIT_POS),
	                  LC_SHARP_MID_LIMIT_POS);
	vIO32WriteFldAlign(HSHARP_0A ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_LOW_LIMIT_POS),
	                  LC_SHARP_LOW_LIMIT_POS);
	vIO32WriteFldAlign(HSHARP_0B ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_HIGH_LIMIT_NEG),
	                  LC_SHARP_HIGH_LIMIT_NEG);
	vIO32WriteFldAlign(HSHARP_0B ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_MID_LIMIT_NEG),
	                  LC_SHARP_MID_LIMIT_NEG);
	vIO32WriteFldAlign(HSHARP_0B ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_LOW_LIMIT_NEG),
	                  LC_SHARP_LOW_LIMIT_NEG);

//  Local Clip
	vIO32WriteFldAlign(HSHARP_05 ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_CLIP_POS_EN),
	                  LC_CLIP_POS_EN);
	vIO32WriteFldAlign(HSHARP_05 ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_CLIP_NEG_EN),
	                  LC_CLIP_NEG_EN);

//  Local Table Setting
	vIO32WriteFldAlign(HSHARP_06 ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_TABLE_GAIN),
	                  LC_TABLE_GAIN);
	vIO32WriteFldAlign(HSHARP_06 ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_TABLE_SEL),
	                  LC_TABLE_SEL);
	vIO32WriteFldAlign(HSHARP_06 ,
	                  wReadQualityTable(QUALITY_HSHARP_LC_TABLE_OFFSET),
	                  LC_TABLE_OFFSET);

//  FLPF
	vIO32WriteFldAlign(HSHARP_0F ,
	                  wReadQualityTable(QUALITY_HSHARP_AC_LPF_EN),
	                  SHARP_AC_EN);
	vIO32WriteFldAlign(HSHARP_10 ,
	                  wReadQualityTable(QUALITY_HSHARP_AC_LPF_COEF),
	                  SHARP_AC_COEF);
#endif
}

/*****************************************************************************************/
/************************************  TDSHARP *******************************************/
/*****************************************************************************************/
/**
 * @brief Post sharpness On/Off (Main path only)
 * @param bOnOff SV_ON/SV_OFF
 */
void vDrvPostSharpOnOff(UINT8 bOnOff)
{

	VRM_INFO_T in,out;
    UINT8 bNoVSHP; 
    UINT8 bLineInterleave;

    UINT32   u4SCL  = u4DrvTDTVScalerModeQuery();
    E_TD_OUT eSCLOut= TD_SCL_OUT(u4SCL);
    E_TD_IN  eSCLIn = TD_SCL_IN(u4SCL);


    //scaler out line interleave or not
    bLineInterleave =((E_TD_OUT_3D_LI==eSCLOut)||
                     ((E_TD_IN_LI_P==eSCLIn)&&(E_TD_OUT_NATIVE==eSCLOut))); 

    //get fsc in & fsc out info
    VRMGetModuleVRMInfo(SV_VP_MAIN, VRM_MODULE_FSC, &in, &out);
    
    vDrvLIPostSharpOnOff(bLineInterleave); 

    //get fsc in & fsc out info
    VRMGetModuleVRMInfo(SV_VP_MAIN, VRM_MODULE_FSC, &in, &out);
	
    bNoVSHP = (out.u2Width > 1920);
    vIO32WriteFldAlign(TDPROC_MISC_00, bNoVSHP, NO_VERTICAL_SHARP);	
    
#ifdef CC_MT5398
    vIO32WriteFldAlign(TDSHP_00, bOnOff, TDS_EN);
	vIO32WriteFldAlign(PBC_SHP1_01, bOnOff, PBC1_EN);
	vIO32WriteFldAlign(PBC_SHP2_01, bOnOff, PBC2_EN);
	vIO32WriteFldAlign(PBC_SHP3_01, bOnOff, PBC3_EN);
#else //Cobra & Viper
    vIO32WriteFldAlign(SHARP_00, bOnOff, TDS_EN);
	vIO32WriteFldAlign(CDS_00, bOnOff, PBC1_EN);
    vIO32WriteFldAlign(CDS_2A, bOnOff, PBC2_EN);
	vIO32WriteFldAlign(CDS_54, bOnOff, PBC3_EN);
#endif
}
/**
 * @brief Line Interleave Post sharpness On/Off (Main path only)
 * @param bOnOff SV_ON/SV_OFF
 */
void vDrvLIPostSharpOnOff(UINT8 bOnOff)
{
#ifdef CC_MT5398
	vIO32WriteFldAlign(TDSHP_2E, (bOnOff?1:0), TDS_FLT_V_3D);
#else
	vIO32WriteFldAlign(SHARP_00, (bOnOff?1:0), TDS_FLT_V_3D);
#endif
}


void vDrvPostSharpHorizontalOnOff(UINT8 bOnOff)
{
#ifdef CC_MT5398
	if(bOnOff)
	{
		vDrvPostSharpParamHorizontal();
	}
	else
	{
		vIO32WriteFldAlign(TDSHP_10, 0, TDS_GAIN_H1);       
		vIO32WriteFldAlign(TDSHP_10, 0, TDS_GAIN_H2);       
		vIO32WriteFldAlign(TDSHP_10, 0, TDS_GAIN_H3);       

		vIO32WriteFldAlign(PBC_SHP1_04, 0x80, PBC1_GAIN_H1);     
		vIO32WriteFldAlign(PBC_SHP1_04, 0x80, PBC1_GAIN_H2);     
		vIO32WriteFldAlign(PBC_SHP1_04, 0x80, PBC1_GAIN_H3); 

		vIO32WriteFldAlign(PBC_SHP2_04, 0x80, PBC2_GAIN_H1);    
		vIO32WriteFldAlign(PBC_SHP2_04, 0x80, PBC2_GAIN_H2);    
		vIO32WriteFldAlign(PBC_SHP2_04, 0x80, PBC2_GAIN_H3);    

		vIO32WriteFldAlign(PBC_SHP3_04, 0x80, PBC3_GAIN_H1);    
		vIO32WriteFldAlign(PBC_SHP3_04, 0x80, PBC3_GAIN_H2);    
		vIO32WriteFldAlign(PBC_SHP3_04, 0x80, PBC3_GAIN_H3);   	
	}
#endif
}

void vDrvPostSharpVerticalOnOff(UINT8 bOnOff)
{
#ifdef CC_MT5398
	if(bOnOff)
	{
		vDrvPostSharpParamVertical();
	}
	else
	{
		vIO32WriteFldAlign(TDSHP_11, 0, TDS_GAIN_V1);       
		vIO32WriteFldAlign(TDSHP_11, 0, TDS_GAIN_V2);

		vIO32WriteFldAlign(PBC_SHP1_04, 0x80, PBC1_GAIN_V1);      
		vIO32WriteFldAlign(PBC_SHP1_05, 0x80, PBC1_GAIN_V2);
                                                                   
		vIO32WriteFldAlign(PBC_SHP2_04, 0x80, PBC2_GAIN_V1);
		vIO32WriteFldAlign(PBC_SHP2_05, 0x80, PBC2_GAIN_V2);
                                                                   
		vIO32WriteFldAlign(PBC_SHP3_04, 0x80, PBC3_GAIN_V1);
		vIO32WriteFldAlign(PBC_SHP3_05, 0x80, PBC3_GAIN_V2);   	
	}
#endif	
}


void vDrvPostSharpDiagonalOnOff(UINT8 bOnOff)
{
#ifdef CC_MT5398
	if(bOnOff)
	{
		vDrvPostSharpParamDiagonal();
	}
	else
	{
		vIO32WriteFldAlign(TDSHP_10, 0, TDS_GAIN_D1);       
		vIO32WriteFldAlign(TDSHP_11, 0, TDS_GAIN_D2);       
		vIO32WriteFldAlign(TDSHP_11, 0, TDS_GAIN_D3);       
		                                                   
		vIO32WriteFldAlign(PBC_SHP1_05, 0x80, PBC1_GAIN_D1);
		vIO32WriteFldAlign(PBC_SHP1_05, 0x80, PBC1_GAIN_D2);
		vIO32WriteFldAlign(PBC_SHP1_05, 0x80, PBC1_GAIN_D3);
		                                                   
		vIO32WriteFldAlign(PBC_SHP2_05, 0x80, PBC2_GAIN_D1);
		vIO32WriteFldAlign(PBC_SHP2_05, 0x80, PBC2_GAIN_D2);
		vIO32WriteFldAlign(PBC_SHP2_05, 0x80, PBC2_GAIN_D3);
		                                                   
		vIO32WriteFldAlign(PBC_SHP3_05, 0x80, PBC3_GAIN_D1);
		vIO32WriteFldAlign(PBC_SHP3_05, 0x80, PBC3_GAIN_D2);
		vIO32WriteFldAlign(PBC_SHP3_05, 0x80, PBC3_GAIN_D3);	
	}
#endif	
}

/**
 * @brief Check Post sharpness parameters Constraint
 * @param
 * @dependency This function should be called by vApiVideoSharpness
 */
#define SAT_PROC_MAX 0x10
#define EDGE_DETAIL_MIN 0x10
#define EDGE_DETAIL_MAX 0x60

void vDrvTDSConstraint(void)
{
#ifndef CC_MT5398
	UINT8 bV1Gain, bV2Gain, bEdgeGain, bDetailGain;

	//|V2-V1| <=0x20
	bV1Gain = wReadQualityTable(QUALITY_TDSHARP_GAIN_V1);
	bV2Gain = wReadQualityTable(QUALITY_TDSHARP_GAIN_V2);
	if(AbsDiff(bV1Gain, bV2Gain)>0x20)
	{
		printf("\n\nSharpness V1 Gain and V2 Gain Difference too large, cause side effect !!\n\n");
	}

	//Edge-detail Gain
	bEdgeGain = wReadQualityTable(QUALITY_TDSHARP_EDGE_GAIN_H);
	bDetailGain = wReadQualityTable(QUALITY_TDSHARP_DETAIL_GAIN_H);
	if(bEdgeGain >= EDGE_DETAIL_MAX ||
	   bEdgeGain <= EDGE_DETAIL_MIN ||
	   bDetailGain >= EDGE_DETAIL_MAX ||
	   bDetailGain <=EDGE_DETAIL_MIN)
	{
		printf("\n\nSharpness H EdgeGain and DetailGain danger value !!\n\n");
		wWriteQualityTable(QUALITY_TDSHARP_EDGE_GAIN_H, CLIP(bEdgeGain, EDGE_DETAIL_MIN, EDGE_DETAIL_MAX));
		wWriteQualityTable(QUALITY_TDSHARP_DETAIL_GAIN_H, CLIP(bDetailGain, EDGE_DETAIL_MIN, EDGE_DETAIL_MAX));		
	}	
	
	bEdgeGain = wReadQualityTable(QUALITY_TDSHARP_EDGE_GAIN_D);
	bDetailGain = wReadQualityTable(QUALITY_TDSHARP_DETAIL_GAIN_D);
	if(bEdgeGain >= EDGE_DETAIL_MAX ||
	   bEdgeGain <= EDGE_DETAIL_MIN ||
	   bDetailGain >= EDGE_DETAIL_MAX ||
	   bDetailGain <=EDGE_DETAIL_MIN)
	{
		printf("\n\nSharpness D EdgeGain and DetailGain danger value !!\n\n");	
		wWriteQualityTable(QUALITY_TDSHARP_EDGE_GAIN_D, CLIP(bEdgeGain, EDGE_DETAIL_MIN, EDGE_DETAIL_MAX));
		wWriteQualityTable(QUALITY_TDSHARP_DETAIL_GAIN_D, CLIP(bDetailGain, EDGE_DETAIL_MIN, EDGE_DETAIL_MAX));
	}
	
	bEdgeGain = wReadQualityTable(QUALITY_TDSHARP_EDGE_GAIN_V);
	bDetailGain = wReadQualityTable(QUALITY_TDSHARP_DETAIL_GAIN_V);
	if(bEdgeGain >= EDGE_DETAIL_MAX ||
	   bEdgeGain <= EDGE_DETAIL_MIN ||
	   bDetailGain >= EDGE_DETAIL_MAX ||
	   bDetailGain <=EDGE_DETAIL_MIN)
	{	
		printf("\n\nSharpness V EdgeGain and DetailGain danger value !!\n\n");	
		wWriteQualityTable(QUALITY_TDSHARP_EDGE_GAIN_V, CLIP(bEdgeGain, EDGE_DETAIL_MIN, EDGE_DETAIL_MAX));
		wWriteQualityTable(QUALITY_TDSHARP_DETAIL_GAIN_V, CLIP(bDetailGain, EDGE_DETAIL_MIN, EDGE_DETAIL_MAX));	
	}

/*
	//SAT_PROC
	bSAT_PROC = wReadQualityTable(QUALITY_TDSHARPALL_SAT_PROC);
	if(bSAT_PROC > SAT_PROC_MAX)
	{
		printf("\n\nSharpness SAT_PROC Gain too large !!\n\n");
		wWriteQualityTable(QUALITY_TDSHARPALL_SAT_PROC, MIN(bSAT_PROC, SAT_PROC_MAX));
	}
*/	
#endif
}

/**
 * @brief Set Post sharpness parameters (Main path only)
 * @param
 * @dependency This function should be called by vApiVideoSharpness
 */
void vDrvPostSharpParam(void)
{
	VRM_INFO_T in,out;
	UINT8 bNoVSHP; 
    UINT8 bLineInterleave;
    UINT32   u4SCL  = u4DrvTDTVScalerModeQuery();
    E_TD_OUT eSCLOut= TD_SCL_OUT(u4SCL);
    E_TD_IN  eSCLIn = TD_SCL_IN(u4SCL);

	vDrvTDSConstraint();

    //scaler out line interleave or not
    bLineInterleave =((E_TD_OUT_3D_LI==eSCLOut)||
                     ((E_TD_IN_LI_P==eSCLIn)&&(E_TD_OUT_NATIVE==eSCLOut))); 
    //get fsc in & fsc out info
    VRMGetModuleVRMInfo(SV_VP_MAIN, VRM_MODULE_FSC, &in, &out);
    bNoVSHP = (out.u2Width > 1920);

#ifdef CC_MT5398
    // Coring Zero
    vIO32WriteFldAlign(TDSHP_08, wReadQualityTable(QUALITY_TDSHARP_COR_ZERO_H1), TDS_COR_ZERO_H1);                                                                                       
    vIO32WriteFldAlign(TDSHP_08, wReadQualityTable(QUALITY_TDSHARP_COR_ZERO_H2), TDS_COR_ZERO_H2);             
    vIO32WriteFldAlign(TDSHP_08, wReadQualityTable(QUALITY_TDSHARP_COR_ZERO_H3), TDS_COR_ZERO_H3);             
    vIO32WriteFldAlign(TDSHP_08, wReadQualityTable(QUALITY_TDSHARP_COR_ZERO_D1), TDS_COR_ZERO_D1);             
    vIO32WriteFldAlign(TDSHP_09, wReadQualityTable(QUALITY_TDSHARP_COR_ZERO_D2), TDS_COR_ZERO_D2);             
    vIO32WriteFldAlign(TDSHP_09, wReadQualityTable(QUALITY_TDSHARP_COR_ZERO_D3), TDS_COR_ZERO_D3);             
    vIO32WriteFldAlign(TDSHP_09, wReadQualityTable(QUALITY_TDSHARP_COR_ZERO_V1), TDS_COR_ZERO_V1);             
    vIO32WriteFldAlign(TDSHP_09, wReadQualityTable(QUALITY_TDSHARP_COR_ZERO_V2), TDS_COR_ZERO_V2);   
    // Coring Ratio             
    vIO32WriteFldAlign(TDSHP_0B, wReadQualityTable(QUALITY_TDSHARP_COR_RATIO_H1), TDS_COR_RATIO_H1);            
    vIO32WriteFldAlign(TDSHP_0B, wReadQualityTable(QUALITY_TDSHARP_COR_RATIO_H2), TDS_COR_RATIO_H2);            
    vIO32WriteFldAlign(TDSHP_0B, wReadQualityTable(QUALITY_TDSHARP_COR_RATIO_H3), TDS_COR_RATIO_H3);            
    vIO32WriteFldAlign(TDSHP_0B, wReadQualityTable(QUALITY_TDSHARP_COR_RATIO_D1), TDS_COR_RATIO_D1);            
    vIO32WriteFldAlign(TDSHP_0B, wReadQualityTable(QUALITY_TDSHARP_COR_RATIO_D2), TDS_COR_RATIO_D2);            
    vIO32WriteFldAlign(TDSHP_0B, wReadQualityTable(QUALITY_TDSHARP_COR_RATIO_D3), TDS_COR_RATIO_D3);            
    vIO32WriteFldAlign(TDSHP_0B, wReadQualityTable(QUALITY_TDSHARP_COR_RATIO_V1), TDS_COR_RATIO_V1);            
    vIO32WriteFldAlign(TDSHP_0B, wReadQualityTable(QUALITY_TDSHARP_COR_RATIO_V2), TDS_COR_RATIO_V2);  
    // Coring Threshold         
    vIO32WriteFldAlign(TDSHP_0D, wReadQualityTable(QUALITY_TDSHARP_COR_THR_H1), TDS_COR_THR_H1);          
    vIO32WriteFldAlign(TDSHP_0D, wReadQualityTable(QUALITY_TDSHARP_COR_THR_H2), TDS_COR_THR_H2);          
    vIO32WriteFldAlign(TDSHP_0D, wReadQualityTable(QUALITY_TDSHARP_COR_THR_H3), TDS_COR_THR_H3);          
    vIO32WriteFldAlign(TDSHP_0D, wReadQualityTable(QUALITY_TDSHARP_COR_THR_D1), TDS_COR_THR_D1);    
    vIO32WriteFldAlign(TDSHP_0E, wReadQualityTable(QUALITY_TDSHARP_COR_THR_D2), TDS_COR_THR_D2);          
    vIO32WriteFldAlign(TDSHP_0E, wReadQualityTable(QUALITY_TDSHARP_COR_THR_D3), TDS_COR_THR_D3);                
    vIO32WriteFldAlign(TDSHP_0E, wReadQualityTable(QUALITY_TDSHARP_COR_THR_V1), TDS_COR_THR_V1);          
    vIO32WriteFldAlign(TDSHP_0E, wReadQualityTable(QUALITY_TDSHARP_COR_THR_V2), TDS_COR_THR_V2); 

    // Limit
    vIO32WriteFldAlign(TDSHP_LIMIT_00, wReadQualityTable(QUALITY_TDSHARP_LIMIT_H1), TDSHARP_LIMIT_H1);
    vIO32WriteFldAlign(TDSHP_LIMIT_00, wReadQualityTable(QUALITY_TDSHARP_LIMIT_H2), TDSHARP_LIMIT_H2);
    vIO32WriteFldAlign(TDSHP_LIMIT_00, wReadQualityTable(QUALITY_TDSHARP_LIMIT_H3), TDSHARP_LIMIT_H3);
    vIO32WriteFldAlign(TDSHP_LIMIT_00, wReadQualityTable(QUALITY_TDSHARP_LIMIT_D1), TDSHARP_LIMIT_D1);
    vIO32WriteFldAlign(TDSHP_LIMIT_01, wReadQualityTable(QUALITY_TDSHARP_LIMIT_D2), TDSHARP_LIMIT_D2);
    vIO32WriteFldAlign(TDSHP_LIMIT_01, wReadQualityTable(QUALITY_TDSHARP_LIMIT_D3), TDSHARP_LIMIT_D3);
    vIO32WriteFldAlign(TDSHP_LIMIT_01, wReadQualityTable(QUALITY_TDSHARP_LIMIT_V1), TDSHARP_LIMIT_V1);
    vIO32WriteFldAlign(TDSHP_LIMIT_01, wReadQualityTable(QUALITY_TDSHARP_LIMIT_V2), TDSHARP_LIMIT_V2);

    
    //Limit ratio                   
    vIO32WriteFldAlign(TDSHP_19, wReadQualityTable(QUALITY_TDSHARP_LIMIT_RATIO_H1), TDS_LIMIT_RATIO_H1);                 
    vIO32WriteFldAlign(TDSHP_19, wReadQualityTable(QUALITY_TDSHARP_LIMIT_RATIO_H2), TDS_LIMIT_RATIO_H2);                 
    vIO32WriteFldAlign(TDSHP_19, wReadQualityTable(QUALITY_TDSHARP_LIMIT_RATIO_H3), TDS_LIMIT_RATIO_H3);                 
    vIO32WriteFldAlign(TDSHP_19, wReadQualityTable(QUALITY_TDSHARP_LIMIT_RATIO_D1), TDS_LIMIT_RATIO_D1);                 
    vIO32WriteFldAlign(TDSHP_19, wReadQualityTable(QUALITY_TDSHARP_LIMIT_RATIO_D2), TDS_LIMIT_RATIO_D2);                 
    vIO32WriteFldAlign(TDSHP_19, wReadQualityTable(QUALITY_TDSHARP_LIMIT_RATIO_D3), TDS_LIMIT_RATIO_D3);                 
    vIO32WriteFldAlign(TDSHP_19, wReadQualityTable(QUALITY_TDSHARP_LIMIT_RATIO_V1), TDS_LIMIT_RATIO_V1);                 
    vIO32WriteFldAlign(TDSHP_19, wReadQualityTable(QUALITY_TDSHARP_LIMIT_RATIO_V2), TDS_LIMIT_RATIO_V2);     
    
    //Bound                                                                                           
    vIO32WriteFldAlign(TDSHP_LIMIT_03, wReadQualityTable(QUALITY_TDSHARP_BOUND_H1), TDSHARP_BOUND_H1);
    vIO32WriteFldAlign(TDSHP_LIMIT_03, wReadQualityTable(QUALITY_TDSHARP_BOUND_H2), TDSHARP_BOUND_H2);
    vIO32WriteFldAlign(TDSHP_LIMIT_03, wReadQualityTable(QUALITY_TDSHARP_BOUND_H3), TDSHARP_BOUND_H3);
    vIO32WriteFldAlign(TDSHP_LIMIT_03, wReadQualityTable(QUALITY_TDSHARP_BOUND_D1), TDSHARP_BOUND_D1);
    vIO32WriteFldAlign(TDSHP_LIMIT_04, wReadQualityTable(QUALITY_TDSHARP_BOUND_D2), TDSHARP_BOUND_D2);
    vIO32WriteFldAlign(TDSHP_LIMIT_04, wReadQualityTable(QUALITY_TDSHARP_BOUND_D3), TDSHARP_BOUND_D3); 
    vIO32WriteFldAlign(TDSHP_LIMIT_04, wReadQualityTable(QUALITY_TDSHARP_BOUND_V1), TDSHARP_BOUND_V1);
    vIO32WriteFldAlign(TDSHP_LIMIT_04, wReadQualityTable(QUALITY_TDSHARP_BOUND_V2), TDSHARP_BOUND_V2);                                                                                          

    // Clip       
    vIO32WriteFldAlign(TDSHP_21, wReadQualityTable(QUALITY_TDSHARP_CLIP_RATIO_H), TDS_CLIP_RATIO_H);
    vIO32WriteFldAlign(TDSHP_21, wReadQualityTable(QUALITY_TDSHARP_CLIP_POS_H), TDS_CLIP_POS_H);              
    vIO32WriteFldAlign(TDSHP_21, wReadQualityTable(QUALITY_TDSHARP_CLIP_NEG_H), TDS_CLIP_NEG_H);     
    vIO32WriteFldAlign(TDSHP_22, wReadQualityTable(QUALITY_TDSHARP_CLIP_RATIO_V), TDS_CLIP_RATIO_V);                  
    vIO32WriteFldAlign(TDSHP_22, wReadQualityTable(QUALITY_TDSHARP_CLIP_POS_V), TDS_CLIP_POS_V);                    
    vIO32WriteFldAlign(TDSHP_22, wReadQualityTable(QUALITY_TDSHARP_CLIP_NEG_V), TDS_CLIP_NEG_V); 
    vIO32WriteFldAlign(TDSHP_23, wReadQualityTable(QUALITY_TDSHARP_CLIP_RATIO_D), TDS_CLIP_RATIO_D);                  
    vIO32WriteFldAlign(TDSHP_23, wReadQualityTable(QUALITY_TDSHARP_CLIP_POS_D), TDS_CLIP_POS_D);                    
    vIO32WriteFldAlign(TDSHP_23, wReadQualityTable(QUALITY_TDSHARP_CLIP_NEG_D), TDS_CLIP_NEG_D); 

    //PBC    
    vIO32WriteFldAlign(PBC_SHP1_01, wReadQualityTable(QUALITY_PBC_PBC1_EN) && (!bForceAllPQOff) && wReadQualityTable(QUALITY_MODULE_ONOFF_SHARPNESS), PBC1_EN);                                                                                                                    
    vIO32WriteFldAlign(PBC_SHP1_03, wReadQualityTable(QUALITY_PBC_PBC1_EDGE_EN), PBC1_EDGE_EN);                                                                                                          
    vIO32WriteFldAlign(PBC_SHP1_03, wReadQualityTable(QUALITY_PBC_PBC1_EDGE_SLOPE), PBC1_EDGE_SLOPE);           
    vIO32WriteFldAlign(PBC_SHP1_04, wReadQualityTable(QUALITY_PBC_PBC1_LPF_LEVEL), PBC1_LPF_LEVEL);  	    
    vIO32WriteFldAlign(PBC_SHP1_05, wReadQualityTable(QUALITY_PBC_PBC1_GAIN_H1), PBC1_GAIN_H1);     
    vIO32WriteFldAlign(PBC_SHP1_05, wReadQualityTable(QUALITY_PBC_PBC1_GAIN_H2), PBC1_GAIN_H2);     
    vIO32WriteFldAlign(PBC_SHP1_05, wReadQualityTable(QUALITY_PBC_PBC1_GAIN_H3), PBC1_GAIN_H3); 
    vIO32WriteFldAlign(PBC_SHP1_06, wReadQualityTable(QUALITY_PBC_PBC1_GAIN_D1), PBC1_GAIN_D1);      
    vIO32WriteFldAlign(PBC_SHP1_06, wReadQualityTable(QUALITY_PBC_PBC1_GAIN_D2), PBC1_GAIN_D2);      
    vIO32WriteFldAlign(PBC_SHP1_06, wReadQualityTable(QUALITY_PBC_PBC1_GAIN_D3), PBC1_GAIN_D3);
    vIO32WriteFldAlign(PBC_SHP1_05, wReadQualityTable(QUALITY_PBC_PBC1_GAIN_V1), PBC1_GAIN_V1);      
    vIO32WriteFldAlign(PBC_SHP1_06, wReadQualityTable(QUALITY_PBC_PBC1_GAIN_V2), PBC1_GAIN_V2);      	    
    vIO32WriteFldAlign(PBC_SHP1_07, wReadQualityTable(QUALITY_PBC_PBC1_CORING_H1), PBC1_CORING_H1);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP1_07, wReadQualityTable(QUALITY_PBC_PBC1_CORING_H2), PBC1_CORING_H2);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP1_07, wReadQualityTable(QUALITY_PBC_PBC1_CORING_H3), PBC1_CORING_H3);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP1_07, wReadQualityTable(QUALITY_PBC_PBC1_CORING_V1), PBC1_CORING_V1);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP1_08, wReadQualityTable(QUALITY_PBC_PBC1_CORING_V2), PBC1_CORING_V2);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP1_08, wReadQualityTable(QUALITY_PBC_PBC1_CORING_D1), PBC1_CORING_D1);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP1_08, wReadQualityTable(QUALITY_PBC_PBC1_CORING_D2), PBC1_CORING_D2);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP1_08, wReadQualityTable(QUALITY_PBC_PBC1_CORING_D3), PBC1_CORING_D3);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP1_09, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_H1), PBC1_LIMIT_H1);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP1_09, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_H2), PBC1_LIMIT_H2);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP1_09, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_H3), PBC1_LIMIT_H3);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP1_09, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_V1), PBC1_LIMIT_V1);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP1_0A, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_V2), PBC1_LIMIT_V2);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP1_0A, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_D1), PBC1_LIMIT_D1);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP1_0A, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_D2), PBC1_LIMIT_D2);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP1_0A, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_D3), PBC1_LIMIT_D3);                                                                                                        
                                                                                                                                                                                                        
    vIO32WriteFldAlign(PBC_SHP2_01, wReadQualityTable(QUALITY_PBC_PBC2_EN) && (!bForceAllPQOff) && wReadQualityTable(QUALITY_MODULE_ONOFF_SHARPNESS), PBC2_EN);                                                                                                                    
    vIO32WriteFldAlign(PBC_SHP2_03, wReadQualityTable(QUALITY_PBC_PBC2_EDGE_EN), PBC2_EDGE_EN);                                                                                                          
    vIO32WriteFldAlign(PBC_SHP2_03, wReadQualityTable(QUALITY_PBC_PBC2_EDGE_SLOPE), PBC2_EDGE_SLOPE);             
    vIO32WriteFldAlign(PBC_SHP2_04, wReadQualityTable(QUALITY_PBC_PBC2_LPF_LEVEL), PBC2_LPF_LEVEL);  
    vIO32WriteFldAlign(PBC_SHP2_05, wReadQualityTable(QUALITY_PBC_PBC2_GAIN_H1), PBC2_GAIN_H1);    
    vIO32WriteFldAlign(PBC_SHP2_05, wReadQualityTable(QUALITY_PBC_PBC2_GAIN_H2), PBC2_GAIN_H2);    
    vIO32WriteFldAlign(PBC_SHP2_05, wReadQualityTable(QUALITY_PBC_PBC2_GAIN_H3), PBC2_GAIN_H3);    
    vIO32WriteFldAlign(PBC_SHP2_06, wReadQualityTable(QUALITY_PBC_PBC2_GAIN_D1), PBC2_GAIN_D1);    
    vIO32WriteFldAlign(PBC_SHP2_06, wReadQualityTable(QUALITY_PBC_PBC2_GAIN_D2), PBC2_GAIN_D2);    
    vIO32WriteFldAlign(PBC_SHP2_06, wReadQualityTable(QUALITY_PBC_PBC2_GAIN_D3), PBC2_GAIN_D3);
    vIO32WriteFldAlign(PBC_SHP2_05, wReadQualityTable(QUALITY_PBC_PBC2_GAIN_V1), PBC2_GAIN_V1);    
    vIO32WriteFldAlign(PBC_SHP2_06, wReadQualityTable(QUALITY_PBC_PBC2_GAIN_V2), PBC2_GAIN_V2);    	    
    vIO32WriteFldAlign(PBC_SHP2_07, wReadQualityTable(QUALITY_PBC_PBC2_CORING_H1), PBC2_CORING_H1);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP2_07, wReadQualityTable(QUALITY_PBC_PBC2_CORING_H2), PBC2_CORING_H2);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP2_07, wReadQualityTable(QUALITY_PBC_PBC2_CORING_H3), PBC2_CORING_H3);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP2_07, wReadQualityTable(QUALITY_PBC_PBC2_CORING_V1), PBC2_CORING_V1);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP2_08, wReadQualityTable(QUALITY_PBC_PBC2_CORING_V2), PBC2_CORING_V2);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP2_08, wReadQualityTable(QUALITY_PBC_PBC2_CORING_D1), PBC2_CORING_D1);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP2_08, wReadQualityTable(QUALITY_PBC_PBC2_CORING_D2), PBC2_CORING_D2);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP2_08, wReadQualityTable(QUALITY_PBC_PBC2_CORING_D3), PBC2_CORING_D3);                                                                                                      
                                                                                                                                                                                                        
    vIO32WriteFldAlign(PBC_SHP2_09, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_H1), PBC2_LIMIT_H1);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP2_09, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_H2), PBC2_LIMIT_H2);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP2_09, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_H3), PBC2_LIMIT_H3);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP2_09, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_V1), PBC2_LIMIT_V1);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP2_0A, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_V2), PBC2_LIMIT_V2);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP2_0A, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_D1), PBC2_LIMIT_D1);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP2_0A, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_D2), PBC2_LIMIT_D2);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP2_0A, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_D3), PBC2_LIMIT_D3);                                                                                                        
                                                                                                                                                                                                        
    vIO32WriteFldAlign(PBC_SHP3_01, wReadQualityTable(QUALITY_PBC_PBC3_EN) && (!bForceAllPQOff) && wReadQualityTable(QUALITY_MODULE_ONOFF_SHARPNESS), PBC3_EN);                                                                                                                    
    vIO32WriteFldAlign(PBC_SHP3_03, wReadQualityTable(QUALITY_PBC_PBC3_EDGE_EN), PBC3_EDGE_EN);                                                                                                          
    vIO32WriteFldAlign(PBC_SHP3_03, wReadQualityTable(QUALITY_PBC_PBC3_EDGE_SLOPE), PBC3_EDGE_SLOPE);                             
    vIO32WriteFldAlign(PBC_SHP3_04, wReadQualityTable(QUALITY_PBC_PBC3_LPF_LEVEL), PBC3_LPF_LEVEL);  
    vIO32WriteFldAlign(PBC_SHP3_05, wReadQualityTable(QUALITY_PBC_PBC3_GAIN_H1), PBC3_GAIN_H1);    
    vIO32WriteFldAlign(PBC_SHP3_05, wReadQualityTable(QUALITY_PBC_PBC3_GAIN_H2), PBC3_GAIN_H2);    
    vIO32WriteFldAlign(PBC_SHP3_05, wReadQualityTable(QUALITY_PBC_PBC3_GAIN_H3), PBC3_GAIN_H3);  
    vIO32WriteFldAlign(PBC_SHP3_06, wReadQualityTable(QUALITY_PBC_PBC3_GAIN_D1), PBC3_GAIN_D1);    
    vIO32WriteFldAlign(PBC_SHP3_06, wReadQualityTable(QUALITY_PBC_PBC3_GAIN_D2), PBC3_GAIN_D2);    
    vIO32WriteFldAlign(PBC_SHP3_06, wReadQualityTable(QUALITY_PBC_PBC3_GAIN_D3), PBC3_GAIN_D3);   
    vIO32WriteFldAlign(PBC_SHP3_05, wReadQualityTable(QUALITY_PBC_PBC3_GAIN_V1), PBC3_GAIN_V1);    
    vIO32WriteFldAlign(PBC_SHP3_06, wReadQualityTable(QUALITY_PBC_PBC3_GAIN_V2), PBC3_GAIN_V2);  	    
    vIO32WriteFldAlign(PBC_SHP3_07, wReadQualityTable(QUALITY_PBC_PBC3_CORING_H1), PBC3_CORING_H1);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP3_07, wReadQualityTable(QUALITY_PBC_PBC3_CORING_H2), PBC3_CORING_H2);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP3_07, wReadQualityTable(QUALITY_PBC_PBC3_CORING_H3), PBC3_CORING_H3);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP3_07, wReadQualityTable(QUALITY_PBC_PBC3_CORING_V1), PBC3_CORING_V1);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP3_08, wReadQualityTable(QUALITY_PBC_PBC3_CORING_V2), PBC3_CORING_V2);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP3_08, wReadQualityTable(QUALITY_PBC_PBC3_CORING_D1), PBC3_CORING_D1);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP3_08, wReadQualityTable(QUALITY_PBC_PBC3_CORING_D2), PBC3_CORING_D2);                                                                                                      
    vIO32WriteFldAlign(PBC_SHP3_08, wReadQualityTable(QUALITY_PBC_PBC3_CORING_D3), PBC3_CORING_D3);                                                                                                      
                                                                                                                                                                                                        
    vIO32WriteFldAlign(PBC_SHP3_09, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_H1), PBC3_LIMIT_H1);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP3_09, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_H2), PBC3_LIMIT_H2);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP3_09, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_H3), PBC3_LIMIT_H3);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP3_09, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_V1), PBC3_LIMIT_V1);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP3_0A, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_V2), PBC3_LIMIT_V2);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP3_0A, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_D1), PBC3_LIMIT_D1);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP3_0A, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_D2), PBC3_LIMIT_D2);                                                                                                        
    vIO32WriteFldAlign(PBC_SHP3_0A, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_D3), PBC3_LIMIT_D3); 	
 
    // TDSHARPALL
    vIO32WriteFldAlign(TDSHP_27, wReadQualityTable(QUALITY_TDSHARPALL_COR_ZERO), TDS_COR_ZERO);	
    vIO32WriteFldAlign(TDSHP_27, wReadQualityTable(QUALITY_TDSHARPALL_COR_RATIO), TDS_COR_RATIO);      
    vIO32WriteFldAlign(TDSHP_27, wReadQualityTable(QUALITY_TDSHARPALL_COR_THR),TDS_COR_THR);        
    vIO32WriteFldAlign(TDSHP_27, wReadQualityTable(QUALITY_TDSHARPALL_GAIN), TDS_GAIN);           
    vIO32WriteFldAlign(TDSHP_27, wReadQualityTable(QUALITY_TDSHARPALL_LIMIT_RATIO), TDS_LIMIT_RATIO); 
    vIO32WriteFldAlign(TDSHP_LIMIT_02, wReadQualityTable(QUALITY_TDSHARPALL_LIMIT), TDSHARP_LIMIT);    
    vIO32WriteFldAlign(TDSHP_LIMIT_05, wReadQualityTable(QUALITY_TDSHARPALL_BOUND), TDSHARP_BOUND);            
    vIO32WriteFldAlign(TDSHP_29, wReadQualityTable(QUALITY_TDSHARPALL_CLIP_RATIO), TDS_CLIP_RATIO);     
    vIO32WriteFldAlign(TDSHP_29, wReadQualityTable(QUALITY_TDSHARPALL_CLIP_POS), TDS_CLIP_POS);       
    vIO32WriteFldAlign(TDSHP_29, wReadQualityTable(QUALITY_TDSHARPALL_CLIP_NEG), TDS_CLIP_NEG);       
    vIO32WriteFldAlign(TDSHP_07, wReadQualityTable(QUALITY_TDSHARPALL_SAT_PROC), TDS_SAT_PROC);       
    vIO32WriteFldAlign(TDSHP_29, wReadQualityTable(QUALITY_TDSHARPALL_AC_LPF_COE), TDS_AC_LPF_COE);     

	// Turn off functions which don't support PR mode.
	if(bNoVSHP || bLineInterleave)
	{
		vIO32WriteFldAlign(TDSHP_2E, SV_OFF, TDS_YLEV_EN);
		vIO32WriteFldAlign(PBC_SHP1_04, SV_OFF, PBC1_LPF_EN);
		vIO32WriteFldAlign(PBC_SHP2_04, SV_OFF, PBC2_LPF_EN);
		vIO32WriteFldAlign(PBC_SHP3_04, SV_OFF, PBC3_LPF_EN);		
	}
	else
	{
		vIO32WriteFldAlign(TDSHP_2E, wReadQualityTable(QUALITY_TDSHARPALL_YLEV_EN), TDS_YLEV_EN); 
		vIO32WriteFldAlign(PBC_SHP1_04, wReadQualityTable(QUALITY_PBC_PBC1_LPF_EN), PBC1_LPF_EN);
		vIO32WriteFldAlign(PBC_SHP2_04, wReadQualityTable(QUALITY_PBC_PBC2_LPF_EN), PBC2_LPF_EN);
		vIO32WriteFldAlign(PBC_SHP3_04, wReadQualityTable(QUALITY_PBC_PBC3_LPF_EN), PBC3_LPF_EN); 
	}

#else
	//PBC
    vIO32WriteFldAlign(CDS_00, wReadQualityTable(QUALITY_PBC_PBC1_EN) && (!bForceAllPQOff) && wReadQualityTable(QUALITY_MODULE_ONOFF_SHARPNESS), PBC1_EN);                                                                                                                   
    vIO32WriteFldAlign(CDS_03, wReadQualityTable(QUALITY_PBC_PBC1_EDGE_EN), PBC1_EDGE_EN);                                                                                                          
    vIO32WriteFldAlign(CDS_03, wReadQualityTable(QUALITY_PBC_PBC1_EDGE_SLOPE), PBC1_EDGE_SLOPE);                              
    vIO32WriteFldAlign(CDS_04, wReadQualityTable(QUALITY_PBC_PBC1_LPF_LEVEL), PBC1_LPF_LEVEL);

    vIO32WriteFldAlign(CDS_2A, wReadQualityTable(QUALITY_PBC_PBC2_EN) && (!bForceAllPQOff) && wReadQualityTable(QUALITY_MODULE_ONOFF_SHARPNESS), PBC2_EN);                                                                                                                    
    vIO32WriteFldAlign(CDS_2D, wReadQualityTable(QUALITY_PBC_PBC2_EDGE_EN), PBC2_EDGE_EN);                                                                                                          
    vIO32WriteFldAlign(CDS_2D, wReadQualityTable(QUALITY_PBC_PBC2_EDGE_SLOPE), PBC2_EDGE_SLOPE);                               
    vIO32WriteFldAlign(CDS_2E, wReadQualityTable(QUALITY_PBC_PBC2_LPF_LEVEL), PBC2_LPF_LEVEL);
	
    vIO32WriteFldAlign(CDS_54, wReadQualityTable(QUALITY_PBC_PBC3_EN) && (!bForceAllPQOff) && wReadQualityTable(QUALITY_MODULE_ONOFF_SHARPNESS), PBC3_EN);                                                                                                                    
    vIO32WriteFldAlign(CDS_57, wReadQualityTable(QUALITY_PBC_PBC3_EDGE_EN), PBC3_EDGE_EN);                                                                                                          
    vIO32WriteFldAlign(CDS_57, wReadQualityTable(QUALITY_PBC_PBC3_EDGE_SLOPE), PBC3_EDGE_SLOPE);                               
    vIO32WriteFldAlign(CDS_58, wReadQualityTable(QUALITY_PBC_PBC3_LPF_LEVEL), PBC3_LPF_LEVEL);

    //Group Clip       
    vIO32WriteFldMulti(SHARP_45, P_Fld(wReadQualityTable(QUALITY_TDSHARP_CLIP_RATIO_H), TDS_CLIP_RATIO_LH)|
    							 P_Fld(wReadQualityTable(QUALITY_TDSHARP_CLIP_POS_H), TDS_CLIP_THR_LH)|
    							 P_Fld(wReadQualityTable(QUALITY_TDSHARP_CLIP_RATIO_D), TDS_CLIP_RATIO_MD)|
    							 P_Fld(wReadQualityTable(QUALITY_TDSHARP_CLIP_POS_D), TDS_CLIP_THR_MD));              
     
	vIO32WriteFldMulti(SHARP_46, P_Fld(wReadQualityTable(QUALITY_TDSHARP_CLIP_RATIO_V), TDS_CLIP_RATIO_HV)|
								  P_Fld(wReadQualityTable(QUALITY_TDSHARP_CLIP_POS_V), TDS_CLIP_THR_HV));

	//Group Edge/Detail Gain
	vIO32WriteFldMulti(SHARP_31, P_Fld(wReadQualityTable(QUALITY_TDSHARP_DETAIL_GAIN_V), SHP_DETAIL_GAIN_HB)|     
								 P_Fld(wReadQualityTable(QUALITY_TDSHARP_EDGE_GAIN_V), SHP_EDGE_GAIN_HB));  
	vIO32WriteFldMulti(SHARP_30, P_Fld(wReadQualityTable(QUALITY_TDSHARP_DETAIL_GAIN_D), SHP_DETAIL_GAIN_MB)|     
								 P_Fld(wReadQualityTable(QUALITY_TDSHARP_EDGE_GAIN_D), SHP_EDGE_GAIN_MB)|  
								 P_Fld(wReadQualityTable(QUALITY_TDSHARP_DETAIL_GAIN_H), SHP_DETAIL_GAIN_LB)|     
								 P_Fld(wReadQualityTable(QUALITY_TDSHARP_EDGE_GAIN_H), SHP_EDGE_GAIN_LB));  
    
    // TDSHARPALL
    vIO32WriteFldAlign(TDSHP_GLOBAL, wReadQualityTable(QUALITY_TDSHARPALL_GAIN), TDS_GLOBAL_GAIN); 
    vIO32WriteFldMulti(SHARP_5D, P_Fld(wReadQualityTable(QUALITY_TDSHARPALL_CLIP_RATIO), TDS_CLIP_RATIO)|     
								 P_Fld(wReadQualityTable(QUALITY_TDSHARPALL_CLIP_POS), TDS_CLIP_THR));       
    vIO32WriteFldAlign(SHARP_36, wReadQualityTable(QUALITY_TDSHARPALL_SAT_PROC), TDS_SAT_PROC);       
    vIO32WriteFldAlign(SHARP_41, wReadQualityTable(QUALITY_TDSHARPALL_AC_LPF_COE), TDS_AC_LPF_COE);     
	// Y Level 
	if(bNoVSHP || bLineInterleave)
	{
		vIO32WriteFldAlign(SHARP_4B, SV_OFF, TDS_YLEV_EN); 
		vIO32WriteFldAlign(CDS_04, SV_OFF, PBC1_LPF_EN);
		vIO32WriteFldAlign(CDS_2E, SV_OFF, PBC2_LPF_EN);
		vIO32WriteFldAlign(CDS_58, SV_OFF, PBC3_LPF_EN);
	}
	else
	{
		vIO32WriteFldAlign(SHARP_4B, wReadQualityTable(QUALITY_TDSHARPALL_YLEV_EN), TDS_YLEV_EN);
		vIO32WriteFldAlign(CDS_04, wReadQualityTable(QUALITY_PBC_PBC1_LPF_EN), PBC1_LPF_EN);
		vIO32WriteFldAlign(CDS_2E, wReadQualityTable(QUALITY_PBC_PBC2_LPF_EN), PBC2_LPF_EN);
		vIO32WriteFldAlign(CDS_58, wReadQualityTable(QUALITY_PBC_PBC3_LPF_EN), PBC3_LPF_EN);
	}
#endif

    //Tap Adap
#ifdef CC_MT5398
	vIO32WriteFldMulti(TDSHP_07, P_Fld(wReadQualityTable(QUALITY_TDSHARP_TDS_TAP_MODE), TDS_TAP_MODE)|     
								 P_Fld(wReadQualityTable(QUALITY_TDSHARP_TDS_TAP_EN), TDS_TAP_EN)|  
								 P_Fld(wReadQualityTable(QUALITY_TDSHARP_TDS_TAP_CLIP_EN), TDS_TAP_CLIP_EN)|     
								 P_Fld(wReadQualityTable(QUALITY_TDSHARP_TDS_TAP_LIMIT), TDS_TAP_LIMIT)|
								 P_Fld(wReadQualityTable(QUALITY_TDSHARP_TDS_TAP_RATIO), TDS_TAP_RATIO)|
								 P_Fld(wReadQualityTable(QUALITY_TDSHARP_TDS_TAP_SRATIO), TDS_TAP_SRATIO));
#else
    vIO32WriteFldMulti(SHARP_42, P_Fld(wReadQualityTable(QUALITY_TDSHARP_TDS_TAP_MODE), TDS_TAP_MODE)|     
								 P_Fld(wReadQualityTable(QUALITY_TDSHARP_TDS_TAP_EN), TDS_TAP_EN)|  
								 P_Fld(wReadQualityTable(QUALITY_TDSHARP_TDS_TAP_CLIP_EN), TDS_TAP_CLIP_EN)|     
								 P_Fld(wReadQualityTable(QUALITY_TDSHARP_TDS_TAP_LIMIT), TDS_TAP_LIMIT)|
								 P_Fld(wReadQualityTable(QUALITY_TDSHARP_TDS_TAP_RATIO), TDS_TAP_RATIO)|
								 P_Fld(wReadQualityTable(QUALITY_TDSHARP_TDS_TAP_SRATIO), TDS_TAP_SRATIO));
#endif
    
    // Y Level 
    vIO32WriteFldAlign(TDPROC_YLEV_00, wReadQualityTable(QUALITY_TDSHARPALL_YLEV_ADAP_ENA), TDS_YLEV_ADAP_ENA);
    vIO32WriteFldAlign(TDPROC_YLEV_00, wReadQualityTable(QUALITY_TDSHARPALL_YLEV_ADL_ENA), TDS_YLEV_ADL_ENA);
    vIO32WriteFldAlign(TDPROC_YLEV_00, wReadQualityTable(QUALITY_TDSHARPALL_YLEV_APL_ENA), TDS_YLEV_APL_ENA);
    vIO32WriteFldAlign(TDPROC_YLEV_00, wReadQualityTable(QUALITY_TDSHARPALL_YLEV_ADL_GAIN), TDS_YLEV_ADL_GAIN);
    vIO32WriteFldAlign(TDPROC_YLEV_00, wReadQualityTable(QUALITY_TDSHARPALL_YLEV_APL_ZERO), TDS_YLEV_APL_ZERO);
    vIO32WriteFldAlign(TDPROC_YLEV_00, wReadQualityTable(QUALITY_TDSHARPALL_YLEV_APL_THR), TDS_YLEV_APL_THR);	
    vIO32WriteFldAlign(TDPROC_YLEV_01, wReadQualityTable(QUALITY_TDSHARPALL_YLEV_P1),  TDS_YLEV_P1);
    vIO32WriteFldAlign(TDPROC_YLEV_01, wReadQualityTable(QUALITY_TDSHARPALL_YLEV_P2),  TDS_YLEV_P2);
    vIO32WriteFldAlign(TDPROC_YLEV_01, wReadQualityTable(QUALITY_TDSHARPALL_YLEV_P3),  TDS_YLEV_P3);
    vIO32WriteFldAlign(TDPROC_YLEV_01, wReadQualityTable(QUALITY_TDSHARPALL_YLEV_P4),  TDS_YLEV_P4);
    vIO32WriteFldAlign(TDPROC_YLEV_02, wReadQualityTable(QUALITY_TDSHARPALL_YLEV_G1), TDS_YLEV_G1);
    vIO32WriteFldAlign(TDPROC_YLEV_02, wReadQualityTable(QUALITY_TDSHARPALL_YLEV_G2), TDS_YLEV_G2);
    vIO32WriteFldAlign(TDPROC_YLEV_02, wReadQualityTable(QUALITY_TDSHARPALL_YLEV_G3), TDS_YLEV_G3);
    vIO32WriteFldAlign(TDPROC_YLEV_02, wReadQualityTable(QUALITY_TDSHARPALL_YLEV_G4), TDS_YLEV_G4);

	
    // TDSHARP_3D

	vIO32WriteFldAlign(SHARPNESS_3D_00, u4DrvTDTV3DModeQuery(), SHARPNESS_3D_MODE); 
    vIO32WriteFldAlign(SHARPNESS_3D_01, wReadQualityTable(QUALITY_TDSHARP_3D_FS_HGAIN), SHARPNESS_FS_HGAIN);    
    vIO32WriteFldAlign(SHARPNESS_3D_01, wReadQualityTable(QUALITY_TDSHARP_3D_FS_DGAIN), SHARPNESS_FS_DGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_01, wReadQualityTable(QUALITY_TDSHARP_3D_FS_VGAIN), SHARPNESS_FS_VGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_02, wReadQualityTable(QUALITY_TDSHARP_3D_TTD_HGAIN), SHARPNESS_TTD_HGAIN);     
    vIO32WriteFldAlign(SHARPNESS_3D_02, wReadQualityTable(QUALITY_TDSHARP_3D_TTD_DGAIN), SHARPNESS_TTD_DGAIN);     
    vIO32WriteFldAlign(SHARPNESS_3D_02, wReadQualityTable(QUALITY_TDSHARP_3D_TTD_VGAIN), SHARPNESS_TTD_VGAIN);     
    vIO32WriteFldAlign(SHARPNESS_3D_03, wReadQualityTable(QUALITY_TDSHARP_3D_SBS_HGAIN), SHARPNESS_SBS_HGAIN);     
    vIO32WriteFldAlign(SHARPNESS_3D_03, wReadQualityTable(QUALITY_TDSHARP_3D_SBS_DGAIN), SHARPNESS_SBS_DGAIN);     
    vIO32WriteFldAlign(SHARPNESS_3D_03, wReadQualityTable(QUALITY_TDSHARP_3D_SBS_VGAIN), SHARPNESS_SBS_VGAIN);     
    vIO32WriteFldAlign(SHARPNESS_3D_04, wReadQualityTable(QUALITY_TDSHARP_3D_TB_HGAIN), SHARPNESS_TB_HGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_04, wReadQualityTable(QUALITY_TDSHARP_3D_TB_DGAIN), SHARPNESS_TB_DGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_04, wReadQualityTable(QUALITY_TDSHARP_3D_TB_VGAIN), SHARPNESS_TB_VGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_05, wReadQualityTable(QUALITY_TDSHARP_3D_SS_HGAIN), SHARPNESS_SS_HGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_05, wReadQualityTable(QUALITY_TDSHARP_3D_SS_DGAIN), SHARPNESS_SS_DGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_05, wReadQualityTable(QUALITY_TDSHARP_3D_SS_VGAIN), SHARPNESS_SS_VGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_06, wReadQualityTable(QUALITY_TDSHARP_3D_RD_HGAIN), SHARPNESS_RD_HGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_06, wReadQualityTable(QUALITY_TDSHARP_3D_RD_DGAIN), SHARPNESS_RD_DGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_06, wReadQualityTable(QUALITY_TDSHARP_3D_RD_VGAIN), SHARPNESS_RD_VGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_07, wReadQualityTable(QUALITY_TDSHARP_3D_LI_HGAIN), SHARPNESS_LI_HGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_07, wReadQualityTable(QUALITY_TDSHARP_3D_LI_DGAIN), SHARPNESS_LI_DGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_07, wReadQualityTable(QUALITY_TDSHARP_3D_LI_VGAIN), SHARPNESS_LI_VGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_08, wReadQualityTable(QUALITY_TDSHARP_3D_CB_HGAIN), SHARPNESS_CB_HGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_08, wReadQualityTable(QUALITY_TDSHARP_3D_CB_DGAIN), SHARPNESS_CB_DGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_08, wReadQualityTable(QUALITY_TDSHARP_3D_CB_VGAIN), SHARPNESS_CB_VGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_09, wReadQualityTable(QUALITY_TDSHARP_3D_DA_HGAIN), SHARPNESS_DA_HGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_09, wReadQualityTable(QUALITY_TDSHARP_3D_DA_DGAIN), SHARPNESS_DA_DGAIN);      
    vIO32WriteFldAlign(SHARPNESS_3D_09, wReadQualityTable(QUALITY_TDSHARP_3D_DA_VGAIN), SHARPNESS_DA_VGAIN);  

    vIO32WriteFldAlign(SHARPNESS_3D_0A, wReadQualityTable(QUALITY_TDSHARP_3D_FS_HCLIP), SHARPNESS_FS_HCLIP);    
    vIO32WriteFldAlign(SHARPNESS_3D_0A, wReadQualityTable(QUALITY_TDSHARP_3D_FS_DCLIP), SHARPNESS_FS_DCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_0A, wReadQualityTable(QUALITY_TDSHARP_3D_FS_VCLIP), SHARPNESS_FS_VCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_0B, wReadQualityTable(QUALITY_TDSHARP_3D_TTD_HCLIP), SHARPNESS_TTD_HCLIP);     
    vIO32WriteFldAlign(SHARPNESS_3D_0B, wReadQualityTable(QUALITY_TDSHARP_3D_TTD_DCLIP), SHARPNESS_TTD_DCLIP);     
    vIO32WriteFldAlign(SHARPNESS_3D_0B, wReadQualityTable(QUALITY_TDSHARP_3D_TTD_VCLIP), SHARPNESS_TTD_VCLIP);     
    vIO32WriteFldAlign(SHARPNESS_3D_0C, wReadQualityTable(QUALITY_TDSHARP_3D_SBS_HCLIP), SHARPNESS_SBS_HCLIP);     
    vIO32WriteFldAlign(SHARPNESS_3D_0C, wReadQualityTable(QUALITY_TDSHARP_3D_SBS_DCLIP), SHARPNESS_SBS_DCLIP);     
    vIO32WriteFldAlign(SHARPNESS_3D_0C, wReadQualityTable(QUALITY_TDSHARP_3D_SBS_VCLIP), SHARPNESS_SBS_VCLIP);     
    vIO32WriteFldAlign(SHARPNESS_3D_0D, wReadQualityTable(QUALITY_TDSHARP_3D_TB_HCLIP), SHARPNESS_TB_HCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_0D, wReadQualityTable(QUALITY_TDSHARP_3D_TB_DCLIP), SHARPNESS_TB_DCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_0D, wReadQualityTable(QUALITY_TDSHARP_3D_TB_VCLIP), SHARPNESS_TB_VCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_0E, wReadQualityTable(QUALITY_TDSHARP_3D_SS_HCLIP), SHARPNESS_SS_HCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_0E, wReadQualityTable(QUALITY_TDSHARP_3D_SS_DCLIP), SHARPNESS_SS_DCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_0E, wReadQualityTable(QUALITY_TDSHARP_3D_SS_VCLIP), SHARPNESS_SS_VCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_0F, wReadQualityTable(QUALITY_TDSHARP_3D_RD_HCLIP), SHARPNESS_RD_HCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_0F, wReadQualityTable(QUALITY_TDSHARP_3D_RD_DCLIP), SHARPNESS_RD_DCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_0F, wReadQualityTable(QUALITY_TDSHARP_3D_RD_VCLIP), SHARPNESS_RD_VCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_10, wReadQualityTable(QUALITY_TDSHARP_3D_LI_HCLIP), SHARPNESS_LI_HCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_10, wReadQualityTable(QUALITY_TDSHARP_3D_LI_DCLIP), SHARPNESS_LI_DCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_10, wReadQualityTable(QUALITY_TDSHARP_3D_LI_VCLIP), SHARPNESS_LI_VCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_11, wReadQualityTable(QUALITY_TDSHARP_3D_CB_HCLIP), SHARPNESS_CB_HCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_11, wReadQualityTable(QUALITY_TDSHARP_3D_CB_DCLIP), SHARPNESS_CB_DCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_11, wReadQualityTable(QUALITY_TDSHARP_3D_CB_VCLIP), SHARPNESS_CB_VCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_12, wReadQualityTable(QUALITY_TDSHARP_3D_DA_HCLIP), SHARPNESS_DA_HCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_12, wReadQualityTable(QUALITY_TDSHARP_3D_DA_DCLIP), SHARPNESS_DA_DCLIP);      
    vIO32WriteFldAlign(SHARPNESS_3D_12, wReadQualityTable(QUALITY_TDSHARP_3D_DA_VCLIP), SHARPNESS_DA_VCLIP);  

    // Mo Sharp
	vIO32WriteFldAlign(MOADAP_SHARP_00, wReadQualityTable(QUALITY_TDSHARPALL_MO_SHP_EN), MO_SHP_EN);    
	vIO32WriteFldAlign(MOADAP_SHARP_00, wReadQualityTable(QUALITY_TDSHARPALL_MO_SHP_LEVEL), MO_SHP_LEVEL);        
	vIO32WriteFldAlign(BNR_SHARP_00, wReadQualityTable(QUALITY_TDSHARPALL_BNR_SHP_EN), BNR_SHP_EN);            
	vIO32WriteFldAlign(BNR_SHARP_00, wReadQualityTable(QUALITY_TDSHARPALL_BNR_SHP_LEVEL), BNR_SHP_LEVEL);  

	//3d weight can be customized here by flash pq
	vDrvGetSharp3DWeighting();

	// Gain
	vDrvPostSharpParamHorizontal();
	vDrvPostSharpParamVertical();
	vDrvPostSharpParamDiagonal();
    vDrvPostSharpParamPBC();
	vDrvLTIParam();
}


void vDrvPostSharpParamHorizontal(void)
{
	UINT8 b3DHGain = IO32ReadFldAlign(SHARPNESS_3D_13, SHARPNESS_3D_HGAIN);

#ifdef CC_MT5398
	vIO32WriteFldAlign(TDSHP_10, 
	    wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_GAIN_H1), b3DHGain), TDS_GAIN_H1);
	vIO32WriteFldAlign(TDSHP_10, 
	    wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_GAIN_H2), b3DHGain), TDS_GAIN_H2);
	vIO32WriteFldAlign(TDSHP_10, 
	    wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_GAIN_H3), b3DHGain), TDS_GAIN_H3);
#else
	UINT8 bH1Gain = wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_GAIN_H1), b3DHGain),
		  bH2Gain = wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_GAIN_H2), b3DHGain),
		  bH3Gain = wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_GAIN_H3), b3DHGain),
		  bH1Coring_Ratio = wReadQualityTable(QUALITY_TDSHARP_COR_RATIO_H1),
		  bH2Coring_Ratio = wReadQualityTable(QUALITY_TDSHARP_COR_RATIO_H2),
		  bH3Coring_Ratio = wReadQualityTable(QUALITY_TDSHARP_COR_RATIO_H3),
		  bH1Limit_Ratio = wReadQualityTable(QUALITY_TDSHARP_LIMIT_RATIO_H1), 
		  bH2Limit_Ratio = wReadQualityTable(QUALITY_TDSHARP_LIMIT_RATIO_H2), 
		  bH3Limit_Ratio = wReadQualityTable(QUALITY_TDSHARP_LIMIT_RATIO_H3),
		  bH1Coring_Zero = wReadQualityTable(QUALITY_TDSHARP_COR_ZERO_H1),
		  bH2Coring_Zero = wReadQualityTable(QUALITY_TDSHARP_COR_ZERO_H2),
		  bH3Coring_Zero = wReadQualityTable(QUALITY_TDSHARP_COR_ZERO_H3),
		  bH1Coring_Th = wReadQualityTable(QUALITY_TDSHARP_COR_THR_H1),
		  bH2Coring_Th = wReadQualityTable(QUALITY_TDSHARP_COR_THR_H2),
		  bH3Coring_Th = wReadQualityTable(QUALITY_TDSHARP_COR_THR_H3),
		  bH1Limit = wReadQualityTable(QUALITY_TDSHARP_LIMIT_H1), 
		  bH2Limit = wReadQualityTable(QUALITY_TDSHARP_LIMIT_H2), 
		  bH3Limit = wReadQualityTable(QUALITY_TDSHARP_LIMIT_H3), 
		  bH1Bound = wReadQualityTable(QUALITY_TDSHARP_BOUND_H1), 
		  bH2Bound = wReadQualityTable(QUALITY_TDSHARP_BOUND_H2), 
		  bH3Bound = wReadQualityTable(QUALITY_TDSHARP_BOUND_H3);



	vIO32WriteFldAlign(TDSHP_GAIN_01, bH1Gain, TDSHP_GAIN_H1);
	vIO32WriteFldAlign(TDSHP_GAIN_01, bH2Gain, TDSHP_GAIN_H2);
	vIO32WriteFldAlign(TDSHP_GAIN_01, bH3Gain, TDSHP_GAIN_H3);

	vIO32WriteFldAlign(TDSHP_CORING_RATIO_01, bH1Coring_Ratio, TDSHP_CORING_RATIO_H1);
	vIO32WriteFldAlign(TDSHP_CORING_RATIO_01, bH2Coring_Ratio, TDSHP_CORING_RATIO_H2);
	vIO32WriteFldAlign(TDSHP_CORING_RATIO_01, bH3Coring_Ratio, TDSHP_CORING_RATIO_H3);
 
	vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_01, bH1Limit_Ratio, TDSHP_LIMIT_RATIO_H1); 
	vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_01, bH2Limit_Ratio, TDSHP_LIMIT_RATIO_H2); 
	vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_01, bH3Limit_Ratio, TDSHP_LIMIT_RATIO_H3); 

	vIO32WriteFldAlign(TDSHP_CORE_01, bH1Coring_Zero, TDSHP_CORE_H1);
	vIO32WriteFldAlign(TDSHP_CORE_01, bH2Coring_Zero, TDSHP_CORE_H2);
	vIO32WriteFldAlign(TDSHP_CORE_01, bH3Coring_Zero, TDSHP_CORE_H3);

	vIO32WriteFldAlign(TDSHP_CORETH_01, bH1Coring_Th, TDSHP_CORETH_H1);
	vIO32WriteFldAlign(TDSHP_CORETH_01, bH2Coring_Th, TDSHP_CORETH_H2);
	vIO32WriteFldAlign(TDSHP_CORETH_01, bH3Coring_Th, TDSHP_CORETH_H3);

	vIO32WriteFldAlign(TDSHP_LIMIT_00, bH1Limit, TDSHARP_LIMIT_H1); 
	vIO32WriteFldAlign(TDSHP_LIMIT_00, bH2Limit, TDSHARP_LIMIT_H2); 
	vIO32WriteFldAlign(TDSHP_LIMIT_00, bH3Limit, TDSHARP_LIMIT_H3); 
 
	vIO32WriteFldAlign(TDSHP_LIMIT_03, bH1Bound, TDSHARP_BOUND_H1); 
	vIO32WriteFldAlign(TDSHP_LIMIT_03, bH2Bound, TDSHARP_BOUND_H2); 
	vIO32WriteFldAlign(TDSHP_LIMIT_03, bH3Bound, TDSHARP_BOUND_H3); 
#endif
}

void vDrvPostSharpParamVertical(void)
{     
	UINT8 b3DVGain = IO32ReadFldAlign(SHARPNESS_3D_13, SHARPNESS_3D_VGAIN);

#ifdef CC_MT5398
	vIO32WriteFldAlign(TDSHP_11, 
	    wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_GAIN_V1), b3DVGain), TDS_GAIN_V1);
	vIO32WriteFldAlign(TDSHP_11, 
	    wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_GAIN_V2), b3DVGain), TDS_GAIN_V2);
#else
	UINT8   bV1Gain = wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_GAIN_V1), b3DVGain),
			bV2Gain = wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_GAIN_V2), b3DVGain),
			bV3Gain = wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_GAIN_V3), b3DVGain),
			bV1Coring_Ratio = wReadQualityTable(QUALITY_TDSHARP_COR_RATIO_V1),
			bV2Coring_Ratio = wReadQualityTable(QUALITY_TDSHARP_COR_RATIO_V2),
			bV3Coring_Ratio = wReadQualityTable(QUALITY_TDSHARP_COR_RATIO_V3),
			bV1Limit_Ratio = wReadQualityTable(QUALITY_TDSHARP_LIMIT_RATIO_V1), 
			bV2Limit_Ratio = wReadQualityTable(QUALITY_TDSHARP_LIMIT_RATIO_V2),
			bV3Limit_Ratio = wReadQualityTable(QUALITY_TDSHARP_LIMIT_RATIO_V3), 
			bV1Coring_Zero = wReadQualityTable(QUALITY_TDSHARP_COR_ZERO_V1),
			bV2Coring_Zero = wReadQualityTable(QUALITY_TDSHARP_COR_ZERO_V2),
			bV3Coring_Zero = wReadQualityTable(QUALITY_TDSHARP_COR_ZERO_V3),
			bV1Coring_Th = wReadQualityTable(QUALITY_TDSHARP_COR_THR_V1),
			bV2Coring_Th = wReadQualityTable(QUALITY_TDSHARP_COR_THR_V2),
			bV3Coring_Th = wReadQualityTable(QUALITY_TDSHARP_COR_THR_V3),
			bV1Limit = wReadQualityTable(QUALITY_TDSHARP_LIMIT_V1), 
			bV2Limit = wReadQualityTable(QUALITY_TDSHARP_LIMIT_V2), 
			bV3Limit = wReadQualityTable(QUALITY_TDSHARP_LIMIT_V3), 
			bV1Bound = wReadQualityTable(QUALITY_TDSHARP_BOUND_V1), 
			bV2Bound = wReadQualityTable(QUALITY_TDSHARP_BOUND_V2), 
			bV3Bound = wReadQualityTable(QUALITY_TDSHARP_BOUND_V3);

			vIO32WriteFldAlign(TDSHP_GAIN_02, bV1Gain, TDSHP_GAIN_V1);
			vIO32WriteFldAlign(TDSHP_GAIN_02, bV2Gain, TDSHP_GAIN_V2);
			vIO32WriteFldAlign(TDSHP_GAIN_03, bV3Gain, TDSHP_GAIN_V3);

			vIO32WriteFldAlign(TDSHP_CORING_RATIO_02, bV1Coring_Ratio, TDSHP_CORING_RATIO_V1);
			vIO32WriteFldAlign(TDSHP_CORING_RATIO_02, bV2Coring_Ratio, TDSHP_CORING_RATIO_V2);
			vIO32WriteFldAlign(TDSHP_CORING_RATIO_03, bV3Coring_Ratio, TDSHP_CORING_RATIO_V3);
		 
			vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_02, bV1Limit_Ratio, TDSHP_LIMIT_RATIO_V1); 
			vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_02, bV2Limit_Ratio, TDSHP_LIMIT_RATIO_V2); 
			vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_03, bV3Limit_Ratio, TDSHP_LIMIT_RATIO_V3); 

			vIO32WriteFldAlign(TDSHP_CORE_02, bV1Coring_Zero, TDSHP_CORE_V1);
			vIO32WriteFldAlign(TDSHP_CORE_02, bV2Coring_Zero, TDSHP_CORE_V2);
			vIO32WriteFldAlign(TDSHP_CORE_03, bV3Coring_Zero, TDSHP_CORE_V3);

			vIO32WriteFldAlign(TDSHP_CORETH_02, bV1Coring_Th, TDSHP_CORETH_V1);
			vIO32WriteFldAlign(TDSHP_CORETH_02, bV2Coring_Th, TDSHP_CORETH_V2);
			vIO32WriteFldAlign(TDSHP_CORETH_03, bV3Coring_Th, TDSHP_CORETH_V3);

			vIO32WriteFldAlign(TDSHP_LIMIT_01, bV1Limit, TDSHARP_LIMIT_V1); 
			vIO32WriteFldAlign(TDSHP_LIMIT_01, bV2Limit, TDSHARP_LIMIT_V2); 
			vIO32WriteFldAlign(TDSHP_LIMIT_06, bV3Limit, TDSHARP_LIMIT_V3); 
		 
			vIO32WriteFldAlign(TDSHP_LIMIT_04, bV1Bound, TDSHARP_BOUND_V1); 
			vIO32WriteFldAlign(TDSHP_LIMIT_04, bV2Bound, TDSHARP_BOUND_V2); 
			vIO32WriteFldAlign(TDSHP_LIMIT_06, bV3Bound, TDSHARP_BOUND_V3); 
#endif
}


void vDrvPostSharpParamDiagonal(void)
{
	UINT8 b3DDGain = IO32ReadFldAlign(SHARPNESS_3D_13, SHARPNESS_3D_DGAIN);

#ifdef CC_MT5398
	vIO32WriteFldAlign(TDSHP_10, 
	    wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_GAIN_D1), b3DDGain), TDS_GAIN_D1);
	vIO32WriteFldAlign(TDSHP_11, 
	    wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_GAIN_D2), b3DDGain), TDS_GAIN_D2);
	vIO32WriteFldAlign(TDSHP_11, 
	    wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_GAIN_D3), b3DDGain), TDS_GAIN_D3);
#else
	UINT8 bD1Gain = wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_GAIN_D1), b3DDGain),
		  bD2Gain = wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_GAIN_D2), b3DDGain),
		  bD3Gain = wDrvGetTDShp3DGain(wReadQualityTable(QUALITY_TDSHARP_GAIN_D3), b3DDGain),
		  bD1Coring_Ratio = wReadQualityTable(QUALITY_TDSHARP_COR_RATIO_D1),
		  bD2Coring_Ratio = wReadQualityTable(QUALITY_TDSHARP_COR_RATIO_D2),
		  bD3Coring_Ratio = wReadQualityTable(QUALITY_TDSHARP_COR_RATIO_D3),
		  bD1Limit_Ratio = wReadQualityTable(QUALITY_TDSHARP_LIMIT_RATIO_D1), 
		  bD2Limit_Ratio = wReadQualityTable(QUALITY_TDSHARP_LIMIT_RATIO_D2), 
		  bD3Limit_Ratio = wReadQualityTable(QUALITY_TDSHARP_LIMIT_RATIO_D3),
		  bD1Coring_Zero = wReadQualityTable(QUALITY_TDSHARP_COR_ZERO_D1),
		  bD2Coring_Zero = wReadQualityTable(QUALITY_TDSHARP_COR_ZERO_D2),
		  bD3Coring_Zero = wReadQualityTable(QUALITY_TDSHARP_COR_ZERO_D3),		  
		  bD1Coring_Th = wReadQualityTable(QUALITY_TDSHARP_COR_THR_D1),
		  bD2Coring_Th = wReadQualityTable(QUALITY_TDSHARP_COR_THR_D2),
		  bD3Coring_Th = wReadQualityTable(QUALITY_TDSHARP_COR_THR_D3),		  
		  bD1Limit = wReadQualityTable(QUALITY_TDSHARP_LIMIT_D1), 
		  bD2Limit = wReadQualityTable(QUALITY_TDSHARP_LIMIT_D2), 
		  bD3Limit = wReadQualityTable(QUALITY_TDSHARP_LIMIT_D3), 		  
		  bD1Bound = wReadQualityTable(QUALITY_TDSHARP_BOUND_D1), 
		  bD2Bound = wReadQualityTable(QUALITY_TDSHARP_BOUND_D2), 
		  bD3Bound = wReadQualityTable(QUALITY_TDSHARP_BOUND_D3);

		vIO32WriteFldAlign(TDSHP_GAIN_01, bD1Gain, TDSHP_GAIN_D1);
		vIO32WriteFldAlign(TDSHP_GAIN_02, bD2Gain, TDSHP_GAIN_D2);
		vIO32WriteFldAlign(TDSHP_GAIN_02, bD3Gain, TDSHP_GAIN_D3);
		vIO32WriteFldAlign(TDSHP_CORE_01, bD1Coring_Zero, TDSHP_CORE_D1);
		vIO32WriteFldAlign(TDSHP_CORE_02, bD2Coring_Zero, TDSHP_CORE_D2);
		vIO32WriteFldAlign(TDSHP_CORE_02, bD3Coring_Zero, TDSHP_CORE_D3);
		vIO32WriteFldAlign(TDSHP_CORING_RATIO_01, bD1Coring_Ratio, TDSHP_CORING_RATIO_D1);
		vIO32WriteFldAlign(TDSHP_CORING_RATIO_02, bD2Coring_Ratio, TDSHP_CORING_RATIO_D2);
		vIO32WriteFldAlign(TDSHP_CORING_RATIO_02, bD3Coring_Ratio, TDSHP_CORING_RATIO_D3);
		vIO32WriteFldAlign(TDSHP_CORETH_01, bD1Coring_Th, TDSHP_CORETH_D1);
		vIO32WriteFldAlign(TDSHP_CORETH_02, bD2Coring_Th, TDSHP_CORETH_D2);
		vIO32WriteFldAlign(TDSHP_CORETH_02, bD3Coring_Th, TDSHP_CORETH_D3);
		vIO32WriteFldAlign(TDSHP_LIMIT_00, bD1Limit, TDSHARP_LIMIT_D1); 
		vIO32WriteFldAlign(TDSHP_LIMIT_01, bD2Limit, TDSHARP_LIMIT_D2); 
		vIO32WriteFldAlign(TDSHP_LIMIT_01, bD3Limit, TDSHARP_LIMIT_D3); 
		vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_01, bD1Limit_Ratio, TDSHP_LIMIT_RATIO_D1); 
		vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_02, bD2Limit_Ratio, TDSHP_LIMIT_RATIO_D2); 
		vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_02, bD3Limit_Ratio, TDSHP_LIMIT_RATIO_D3); 
		vIO32WriteFldAlign(TDSHP_LIMIT_03, bD1Bound, TDSHARP_BOUND_D1); 
		vIO32WriteFldAlign(TDSHP_LIMIT_04, bD2Bound, TDSHARP_BOUND_D2); 
		vIO32WriteFldAlign(TDSHP_LIMIT_04, bD3Bound, TDSHARP_BOUND_D3);
#endif
}

void vDrvPostSharpParamPBC(void)
{
    #ifndef CC_MT5398
    vIO32WriteFldAlign(TDSHP_GAIN_05, wReadQualityTable(QUALITY_PBC_PBC1_GAIN_H1), TDSHP_GAIN_PBC1_H1);
    vIO32WriteFldAlign(TDSHP_GAIN_05, wReadQualityTable(QUALITY_PBC_PBC1_GAIN_H2), TDSHP_GAIN_PBC1_H2);
    vIO32WriteFldAlign(TDSHP_GAIN_05, wReadQualityTable(QUALITY_PBC_PBC1_GAIN_H3), TDSHP_GAIN_PBC1_H3);
    vIO32WriteFldAlign(TDSHP_GAIN_05, wReadQualityTable(QUALITY_PBC_PBC1_GAIN_V1), TDSHP_GAIN_PBC1_V1);
    vIO32WriteFldAlign(TDSHP_GAIN_06, wReadQualityTable(QUALITY_PBC_PBC1_GAIN_V2), TDSHP_GAIN_PBC1_V2);
    vIO32WriteFldAlign(TDSHP_GAIN_06, wReadQualityTable(QUALITY_PBC_PBC1_GAIN_V3), TDSHP_GAIN_PBC1_V3);
    vIO32WriteFldAlign(TDSHP_GAIN_06, wReadQualityTable(QUALITY_PBC_PBC1_GAIN_D1), TDSHP_GAIN_PBC1_D1);
    vIO32WriteFldAlign(TDSHP_GAIN_06, wReadQualityTable(QUALITY_PBC_PBC1_GAIN_D2), TDSHP_GAIN_PBC1_D2);
    vIO32WriteFldAlign(TDSHP_GAIN_07, wReadQualityTable(QUALITY_PBC_PBC1_GAIN_D3), TDSHP_GAIN_PBC1_D3);

    vIO32WriteFldAlign(TDSHP_GAIN_07, wReadQualityTable(QUALITY_PBC_PBC2_GAIN_H1), TDSHP_GAIN_PBC2_H1);
    vIO32WriteFldAlign(TDSHP_GAIN_07, wReadQualityTable(QUALITY_PBC_PBC2_GAIN_H2), TDSHP_GAIN_PBC2_H2);
    vIO32WriteFldAlign(TDSHP_GAIN_07, wReadQualityTable(QUALITY_PBC_PBC2_GAIN_H3), TDSHP_GAIN_PBC2_H3);
    vIO32WriteFldAlign(TDSHP_GAIN_08, wReadQualityTable(QUALITY_PBC_PBC2_GAIN_V1), TDSHP_GAIN_PBC2_V1);
    vIO32WriteFldAlign(TDSHP_GAIN_08, wReadQualityTable(QUALITY_PBC_PBC2_GAIN_V2), TDSHP_GAIN_PBC2_V2);
    vIO32WriteFldAlign(TDSHP_GAIN_08, wReadQualityTable(QUALITY_PBC_PBC2_GAIN_V3), TDSHP_GAIN_PBC2_V3);
    vIO32WriteFldAlign(TDSHP_GAIN_08, wReadQualityTable(QUALITY_PBC_PBC2_GAIN_D1), TDSHP_GAIN_PBC2_D1);
    vIO32WriteFldAlign(TDSHP_GAIN_09, wReadQualityTable(QUALITY_PBC_PBC2_GAIN_D2), TDSHP_GAIN_PBC2_D2);
    vIO32WriteFldAlign(TDSHP_GAIN_09, wReadQualityTable(QUALITY_PBC_PBC2_GAIN_D3), TDSHP_GAIN_PBC2_D3);

    vIO32WriteFldAlign(TDSHP_GAIN_09, wReadQualityTable(QUALITY_PBC_PBC3_GAIN_H1), TDSHP_GAIN_PBC3_H1);
    vIO32WriteFldAlign(TDSHP_GAIN_09, wReadQualityTable(QUALITY_PBC_PBC3_GAIN_H2), TDSHP_GAIN_PBC3_H2);
    vIO32WriteFldAlign(TDSHP_GAIN_0A, wReadQualityTable(QUALITY_PBC_PBC3_GAIN_H3), TDSHP_GAIN_PBC3_H3);
    vIO32WriteFldAlign(TDSHP_GAIN_0A, wReadQualityTable(QUALITY_PBC_PBC3_GAIN_V1), TDSHP_GAIN_PBC3_V1);
    vIO32WriteFldAlign(TDSHP_GAIN_0A, wReadQualityTable(QUALITY_PBC_PBC3_GAIN_V2), TDSHP_GAIN_PBC3_V2);
    vIO32WriteFldAlign(TDSHP_GAIN_0A, wReadQualityTable(QUALITY_PBC_PBC3_GAIN_V3), TDSHP_GAIN_PBC3_V3);
    vIO32WriteFldAlign(TDSHP_GAIN_0B, wReadQualityTable(QUALITY_PBC_PBC3_GAIN_D1), TDSHP_GAIN_PBC3_D1);
    vIO32WriteFldAlign(TDSHP_GAIN_0B, wReadQualityTable(QUALITY_PBC_PBC3_GAIN_D2), TDSHP_GAIN_PBC3_D2);
    vIO32WriteFldAlign(TDSHP_GAIN_0B, wReadQualityTable(QUALITY_PBC_PBC3_GAIN_D3), TDSHP_GAIN_PBC3_D3);

    //
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_05, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC1_H1);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_05, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC1_H2);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_05, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC1_H3);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_05, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC1_V1);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_06, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC1_V2);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_06, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC1_V3);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_06, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC1_D1);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_06, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC1_D2);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_07, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC1_D3);

    vIO32WriteFldAlign(TDSHP_CORING_RATIO_07, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC2_H1);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_07, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC2_H2);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_07, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC2_H3);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_08, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC2_V1);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_08, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC2_V2);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_08, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC2_V3);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_08, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC2_D1);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_09, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC2_D2);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_09, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC2_D3);

    vIO32WriteFldAlign(TDSHP_CORING_RATIO_09, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC3_H1);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_09, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC3_H2);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_0A, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC3_H3);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_0A, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC3_V1);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_0A, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC3_V2);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_0A, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC3_V3);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_0B, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC3_D1);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_0B, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC3_D2);
    vIO32WriteFldAlign(TDSHP_CORING_RATIO_0B, COR_RATIO_UNIT, TDSHP_CORE_RATIO_PBC3_D3);

    //
    vIO32WriteFldAlign(TDSHP_CORE_05, wReadQualityTable(QUALITY_PBC_PBC1_CORING_H1), TDSHP_CORE_PBC1_H1);
    vIO32WriteFldAlign(TDSHP_CORE_05, wReadQualityTable(QUALITY_PBC_PBC1_CORING_H2), TDSHP_CORE_PBC1_H2);
    vIO32WriteFldAlign(TDSHP_CORE_05, wReadQualityTable(QUALITY_PBC_PBC1_CORING_H3), TDSHP_CORE_PBC1_H3);
    vIO32WriteFldAlign(TDSHP_CORE_05, wReadQualityTable(QUALITY_PBC_PBC1_CORING_V1), TDSHP_CORE_PBC1_V1);
    vIO32WriteFldAlign(TDSHP_CORE_06, wReadQualityTable(QUALITY_PBC_PBC1_CORING_V2), TDSHP_CORE_PBC1_V2);
    vIO32WriteFldAlign(TDSHP_CORE_06, wReadQualityTable(QUALITY_PBC_PBC1_CORING_V3), TDSHP_CORE_PBC1_V3);
    vIO32WriteFldAlign(TDSHP_CORE_06, wReadQualityTable(QUALITY_PBC_PBC1_CORING_D1), TDSHP_CORE_PBC1_D1);
    vIO32WriteFldAlign(TDSHP_CORE_06, wReadQualityTable(QUALITY_PBC_PBC1_CORING_D2), TDSHP_CORE_PBC1_D2);
    vIO32WriteFldAlign(TDSHP_CORE_07, wReadQualityTable(QUALITY_PBC_PBC1_CORING_D3), TDSHP_CORE_PBC1_D3);

    vIO32WriteFldAlign(TDSHP_CORE_07, wReadQualityTable(QUALITY_PBC_PBC2_CORING_H1), TDSHP_CORE_PBC2_H1);
    vIO32WriteFldAlign(TDSHP_CORE_07, wReadQualityTable(QUALITY_PBC_PBC2_CORING_H2), TDSHP_CORE_PBC2_H2);
    vIO32WriteFldAlign(TDSHP_CORE_07, wReadQualityTable(QUALITY_PBC_PBC2_CORING_H3), TDSHP_CORE_PBC2_H3);
    vIO32WriteFldAlign(TDSHP_CORE_08, wReadQualityTable(QUALITY_PBC_PBC2_CORING_V1), TDSHP_CORE_PBC2_V1);
    vIO32WriteFldAlign(TDSHP_CORE_08, wReadQualityTable(QUALITY_PBC_PBC2_CORING_V2), TDSHP_CORE_PBC2_V2);
    vIO32WriteFldAlign(TDSHP_CORE_08, wReadQualityTable(QUALITY_PBC_PBC2_CORING_V3), TDSHP_CORE_PBC2_V3);
    vIO32WriteFldAlign(TDSHP_CORE_08, wReadQualityTable(QUALITY_PBC_PBC2_CORING_D1), TDSHP_CORE_PBC2_D1);
    vIO32WriteFldAlign(TDSHP_CORE_09, wReadQualityTable(QUALITY_PBC_PBC2_CORING_D2), TDSHP_CORE_PBC2_D2);
    vIO32WriteFldAlign(TDSHP_CORE_09, wReadQualityTable(QUALITY_PBC_PBC2_CORING_D3), TDSHP_CORE_PBC2_D3);

    vIO32WriteFldAlign(TDSHP_CORE_09, wReadQualityTable(QUALITY_PBC_PBC3_CORING_H1), TDSHP_CORE_PBC3_H1);
    vIO32WriteFldAlign(TDSHP_CORE_09, wReadQualityTable(QUALITY_PBC_PBC3_CORING_H2), TDSHP_CORE_PBC3_H2);
    vIO32WriteFldAlign(TDSHP_CORE_0A, wReadQualityTable(QUALITY_PBC_PBC3_CORING_H3), TDSHP_CORE_PBC3_H3);
    vIO32WriteFldAlign(TDSHP_CORE_0A, wReadQualityTable(QUALITY_PBC_PBC3_CORING_V1), TDSHP_CORE_PBC3_V1);
    vIO32WriteFldAlign(TDSHP_CORE_0A, wReadQualityTable(QUALITY_PBC_PBC3_CORING_V2), TDSHP_CORE_PBC3_V2);
    vIO32WriteFldAlign(TDSHP_CORE_0A, wReadQualityTable(QUALITY_PBC_PBC3_CORING_V3), TDSHP_CORE_PBC3_V3);
    vIO32WriteFldAlign(TDSHP_CORE_0B, wReadQualityTable(QUALITY_PBC_PBC3_CORING_D1), TDSHP_CORE_PBC3_D1);
    vIO32WriteFldAlign(TDSHP_CORE_0B, wReadQualityTable(QUALITY_PBC_PBC3_CORING_D2), TDSHP_CORE_PBC3_D2);
    vIO32WriteFldAlign(TDSHP_CORE_0B, wReadQualityTable(QUALITY_PBC_PBC3_CORING_D3), TDSHP_CORE_PBC3_D3);

    //
    vIO32WriteFldAlign(TDSHP_CORETH_05, 0, TDSHP_CORETH_PBC1_H1);
    vIO32WriteFldAlign(TDSHP_CORETH_05, 0, TDSHP_CORETH_PBC1_H2);
    vIO32WriteFldAlign(TDSHP_CORETH_05, 0, TDSHP_CORETH_PBC1_H3);
    vIO32WriteFldAlign(TDSHP_CORETH_05, 0, TDSHP_CORETH_PBC1_V1);
    vIO32WriteFldAlign(TDSHP_CORETH_06, 0, TDSHP_CORETH_PBC1_V2);
    vIO32WriteFldAlign(TDSHP_CORETH_06, 0, TDSHP_CORETH_PBC1_V3);
    vIO32WriteFldAlign(TDSHP_CORETH_06, 0, TDSHP_CORETH_PBC1_D1);
    vIO32WriteFldAlign(TDSHP_CORETH_06, 0, TDSHP_CORETH_PBC1_D2);
    vIO32WriteFldAlign(TDSHP_CORETH_07, 0, TDSHP_CORETH_PBC1_D3);

    vIO32WriteFldAlign(TDSHP_CORETH_07, 0, TDSHP_CORETH_PBC2_H1);
    vIO32WriteFldAlign(TDSHP_CORETH_07, 0, TDSHP_CORETH_PBC2_H2);
    vIO32WriteFldAlign(TDSHP_CORETH_07, 0, TDSHP_CORETH_PBC2_H3);
    vIO32WriteFldAlign(TDSHP_CORETH_08, 0, TDSHP_CORETH_PBC2_V1);
    vIO32WriteFldAlign(TDSHP_CORETH_08, 0, TDSHP_CORETH_PBC2_V2);
    vIO32WriteFldAlign(TDSHP_CORETH_08, 0, TDSHP_CORETH_PBC2_V3);
    vIO32WriteFldAlign(TDSHP_CORETH_08, 0, TDSHP_CORETH_PBC2_D1);
    vIO32WriteFldAlign(TDSHP_CORETH_09, 0, TDSHP_CORETH_PBC2_D2);
    vIO32WriteFldAlign(TDSHP_CORETH_09, 0, TDSHP_CORETH_PBC2_D3);

    vIO32WriteFldAlign(TDSHP_CORETH_09, 0, TDSHP_CORETH_PBC3_H1);
    vIO32WriteFldAlign(TDSHP_CORETH_09, 0, TDSHP_CORETH_PBC3_H2);
    vIO32WriteFldAlign(TDSHP_CORETH_0A, 0, TDSHP_CORETH_PBC3_H3);
    vIO32WriteFldAlign(TDSHP_CORETH_0A, 0, TDSHP_CORETH_PBC3_V1);
    vIO32WriteFldAlign(TDSHP_CORETH_0A, 0, TDSHP_CORETH_PBC3_V2);
    vIO32WriteFldAlign(TDSHP_CORETH_0A, 0, TDSHP_CORETH_PBC3_V3);
    vIO32WriteFldAlign(TDSHP_CORETH_0B, 0, TDSHP_CORETH_PBC3_D1);
    vIO32WriteFldAlign(TDSHP_CORETH_0B, 0, TDSHP_CORETH_PBC3_D2);
    vIO32WriteFldAlign(TDSHP_CORETH_0B, 0, TDSHP_CORETH_PBC3_D3);

    //
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_05, 0, TDSHP_LIMIT_RATIO_PBC1_H1);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_05, 0, TDSHP_LIMIT_RATIO_PBC1_H2);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_05, 0, TDSHP_LIMIT_RATIO_PBC1_H3);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_05, 0, TDSHP_LIMIT_RATIO_PBC1_V1);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_06, 0, TDSHP_LIMIT_RATIO_PBC1_V2);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_06, 0, TDSHP_LIMIT_RATIO_PBC1_V3);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_06, 0, TDSHP_LIMIT_RATIO_PBC1_D1);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_06, 0, TDSHP_LIMIT_RATIO_PBC1_D2);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_07, 0, TDSHP_LIMIT_RATIO_PBC1_D3);

    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_07, 0, TDSHP_LIMIT_RATIO_PBC2_H1);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_07, 0, TDSHP_LIMIT_RATIO_PBC2_H2);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_07, 0, TDSHP_LIMIT_RATIO_PBC2_H3);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_08, 0, TDSHP_LIMIT_RATIO_PBC2_V1);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_08, 0, TDSHP_LIMIT_RATIO_PBC2_V2);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_08, 0, TDSHP_LIMIT_RATIO_PBC2_V3);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_08, 0, TDSHP_LIMIT_RATIO_PBC2_D1);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_09, 0, TDSHP_LIMIT_RATIO_PBC2_D2);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_09, 0, TDSHP_LIMIT_RATIO_PBC2_D3);

    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_09, 0, TDSHP_LIMIT_RATIO_PBC3_H1);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_09, 0, TDSHP_LIMIT_RATIO_PBC3_H2);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_0A, 0, TDSHP_LIMIT_RATIO_PBC3_H3);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_0A, 0, TDSHP_LIMIT_RATIO_PBC3_V1);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_0A, 0, TDSHP_LIMIT_RATIO_PBC3_V2);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_0A, 0, TDSHP_LIMIT_RATIO_PBC3_V3);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_0B, 0, TDSHP_LIMIT_RATIO_PBC3_D1);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_0B, 0, TDSHP_LIMIT_RATIO_PBC3_D2);
    vIO32WriteFldAlign(TDSHP_LIMIT_RATIO_0B, 0, TDSHP_LIMIT_RATIO_PBC3_D3);

    //
    vIO32WriteFldAlign(TDSHP_LIMIT_05_2, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_H1), TDSHP_LIMIT_PBC1_H1);
    vIO32WriteFldAlign(TDSHP_LIMIT_05_2, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_H2), TDSHP_LIMIT_PBC1_H2);
    vIO32WriteFldAlign(TDSHP_LIMIT_05_2, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_H3), TDSHP_LIMIT_PBC1_H3);
    vIO32WriteFldAlign(TDSHP_LIMIT_05_2, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_V1), TDSHP_LIMIT_PBC1_V1);
    vIO32WriteFldAlign(TDSHP_LIMIT_06_2, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_V2), TDSHP_LIMIT_PBC1_V2);
    vIO32WriteFldAlign(TDSHP_LIMIT_06_2, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_V3), TDSHP_LIMIT_PBC1_V3);
    vIO32WriteFldAlign(TDSHP_LIMIT_06_2, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_D1), TDSHP_LIMIT_PBC1_D1);
    vIO32WriteFldAlign(TDSHP_LIMIT_06_2, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_D2), TDSHP_LIMIT_PBC1_D2);
    vIO32WriteFldAlign(TDSHP_LIMIT_07, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_D3), TDSHP_LIMIT_PBC1_D3);

    vIO32WriteFldAlign(TDSHP_LIMIT_07, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_H1), TDSHP_LIMIT_PBC2_H1);
    vIO32WriteFldAlign(TDSHP_LIMIT_07, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_H2), TDSHP_LIMIT_PBC2_H2);
    vIO32WriteFldAlign(TDSHP_LIMIT_07, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_H3), TDSHP_LIMIT_PBC2_H3);
    vIO32WriteFldAlign(TDSHP_LIMIT_08, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_V1), TDSHP_LIMIT_PBC2_V1);
    vIO32WriteFldAlign(TDSHP_LIMIT_08, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_V2), TDSHP_LIMIT_PBC2_V2);
    vIO32WriteFldAlign(TDSHP_LIMIT_08, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_V3), TDSHP_LIMIT_PBC2_V3);
    vIO32WriteFldAlign(TDSHP_LIMIT_08, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_D1), TDSHP_LIMIT_PBC2_D1);
    vIO32WriteFldAlign(TDSHP_LIMIT_09, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_D2), TDSHP_LIMIT_PBC2_D2);
    vIO32WriteFldAlign(TDSHP_LIMIT_09, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_D3), TDSHP_LIMIT_PBC2_D3);

    vIO32WriteFldAlign(TDSHP_LIMIT_09, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_H1), TDSHP_LIMIT_PBC3_H1);
    vIO32WriteFldAlign(TDSHP_LIMIT_09, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_H2), TDSHP_LIMIT_PBC3_H2);
    vIO32WriteFldAlign(TDSHP_LIMIT_0A, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_H3), TDSHP_LIMIT_PBC3_H3);
    vIO32WriteFldAlign(TDSHP_LIMIT_0A, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_V1), TDSHP_LIMIT_PBC3_V1);
    vIO32WriteFldAlign(TDSHP_LIMIT_0A, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_V2), TDSHP_LIMIT_PBC3_V2);
    vIO32WriteFldAlign(TDSHP_LIMIT_0A, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_V3), TDSHP_LIMIT_PBC3_V3);
    vIO32WriteFldAlign(TDSHP_LIMIT_0B, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_D1), TDSHP_LIMIT_PBC3_D1);
    vIO32WriteFldAlign(TDSHP_LIMIT_0B, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_D2), TDSHP_LIMIT_PBC3_D2);
    vIO32WriteFldAlign(TDSHP_LIMIT_0B, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_D3), TDSHP_LIMIT_PBC3_D3);

    //
    vIO32WriteFldAlign(TDSHP_BOUND_05, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_H1), TDSHP_BOUND_PBC1_H1);
    vIO32WriteFldAlign(TDSHP_BOUND_05, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_H2), TDSHP_BOUND_PBC1_H2);
    vIO32WriteFldAlign(TDSHP_BOUND_05, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_H3), TDSHP_BOUND_PBC1_H3);
    vIO32WriteFldAlign(TDSHP_BOUND_05, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_V1), TDSHP_BOUND_PBC1_V1);
    vIO32WriteFldAlign(TDSHP_BOUND_06, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_V2), TDSHP_BOUND_PBC1_V2);
    vIO32WriteFldAlign(TDSHP_BOUND_06, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_V3), TDSHP_BOUND_PBC1_V3);
    vIO32WriteFldAlign(TDSHP_BOUND_06, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_D1), TDSHP_BOUND_PBC1_D1);
    vIO32WriteFldAlign(TDSHP_BOUND_06, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_D2), TDSHP_BOUND_PBC1_D2);
    vIO32WriteFldAlign(TDSHP_BOUND_07, wReadQualityTable(QUALITY_PBC_PBC1_LIMIT_D3), TDSHP_BOUND_PBC1_D3);

    vIO32WriteFldAlign(TDSHP_BOUND_07, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_H1), TDSHP_BOUND_PBC2_H1);
    vIO32WriteFldAlign(TDSHP_BOUND_07, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_H2), TDSHP_BOUND_PBC2_H2);
    vIO32WriteFldAlign(TDSHP_BOUND_07, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_H3), TDSHP_BOUND_PBC2_H3);
    vIO32WriteFldAlign(TDSHP_BOUND_08, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_V1), TDSHP_BOUND_PBC2_V1);
    vIO32WriteFldAlign(TDSHP_BOUND_08, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_V2), TDSHP_BOUND_PBC2_V2);
    vIO32WriteFldAlign(TDSHP_BOUND_08, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_V3), TDSHP_BOUND_PBC2_V3);
    vIO32WriteFldAlign(TDSHP_BOUND_08, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_D1), TDSHP_BOUND_PBC2_D1);
    vIO32WriteFldAlign(TDSHP_BOUND_09, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_D2), TDSHP_BOUND_PBC2_D2);
    vIO32WriteFldAlign(TDSHP_BOUND_09, wReadQualityTable(QUALITY_PBC_PBC2_LIMIT_D3), TDSHP_BOUND_PBC2_D3);

    vIO32WriteFldAlign(TDSHP_BOUND_09, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_H1), TDSHP_BOUND_PBC3_H1);
    vIO32WriteFldAlign(TDSHP_BOUND_09, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_H2), TDSHP_BOUND_PBC3_H2);
    vIO32WriteFldAlign(TDSHP_BOUND_0A, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_H3), TDSHP_BOUND_PBC3_H3);
    vIO32WriteFldAlign(TDSHP_BOUND_0A, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_V1), TDSHP_BOUND_PBC3_V1);
    vIO32WriteFldAlign(TDSHP_BOUND_0A, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_V2), TDSHP_BOUND_PBC3_V2);
    vIO32WriteFldAlign(TDSHP_BOUND_0A, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_V3), TDSHP_BOUND_PBC3_V3);
    vIO32WriteFldAlign(TDSHP_BOUND_0B, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_D1), TDSHP_BOUND_PBC3_D1);
    vIO32WriteFldAlign(TDSHP_BOUND_0B, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_D2), TDSHP_BOUND_PBC3_D2);
    vIO32WriteFldAlign(TDSHP_BOUND_0B, wReadQualityTable(QUALITY_PBC_PBC3_LIMIT_D3), TDSHP_BOUND_PBC3_D3);
    #endif    
}

void vDrvPostSharpParamLimit(void)
{
#ifdef CC_MT5398
    vIO32WriteFldAlign(TDSHP_13, IO32ReadFldAlign(TDSHP_LIMIT_00, TDSHARP_LIMIT_H1), TDS_LIMIT_POS_H1);  
    vIO32WriteFldAlign(TDSHP_13, IO32ReadFldAlign(TDSHP_LIMIT_00, TDSHARP_LIMIT_H2), TDS_LIMIT_POS_H2);  
    vIO32WriteFldAlign(TDSHP_13, IO32ReadFldAlign(TDSHP_LIMIT_00, TDSHARP_LIMIT_H3), TDS_LIMIT_POS_H3);  
    vIO32WriteFldAlign(TDSHP_13, IO32ReadFldAlign(TDSHP_LIMIT_00, TDSHARP_LIMIT_D1), TDS_LIMIT_POS_D1);  
    vIO32WriteFldAlign(TDSHP_14, IO32ReadFldAlign(TDSHP_LIMIT_01, TDSHARP_LIMIT_D2), TDS_LIMIT_POS_D2);  
    vIO32WriteFldAlign(TDSHP_14, IO32ReadFldAlign(TDSHP_LIMIT_01, TDSHARP_LIMIT_D3), TDS_LIMIT_POS_D3);  
    vIO32WriteFldAlign(TDSHP_14, IO32ReadFldAlign(TDSHP_LIMIT_01, TDSHARP_LIMIT_V1), TDS_LIMIT_POS_V1);  
    vIO32WriteFldAlign(TDSHP_14, IO32ReadFldAlign(TDSHP_LIMIT_01, TDSHARP_LIMIT_V2), TDS_LIMIT_POS_V2); 
    vIO32WriteFldAlign(TDSHP_15, IO32ReadFldAlign(TDSHP_LIMIT_02, LTI_LIMIT_LTI_H1), TDS_LIMIT_POS_LTI_H1);  
    vIO32WriteFldAlign(TDSHP_15, IO32ReadFldAlign(TDSHP_LIMIT_02, LTI_LIMIT_LTI_H2), TDS_LIMIT_POS_LTI_H2);  
    vIO32WriteFldAlign(TDSHP_15, IO32ReadFldAlign(TDSHP_LIMIT_02, LTI_LIMIT_LTI_V), TDS_LIMIT_POS_LTI_V);  
    vIO32WriteFldAlign(TDSHP_28, IO32ReadFldAlign(TDSHP_LIMIT_02, TDSHARP_LIMIT), TDS_LIMIT_POS);   

    vIO32WriteFldAlign(TDSHP_16, IO32ReadFldAlign(TDSHP_LIMIT_00, TDSHARP_LIMIT_H1), TDS_LIMIT_NEG_H1);  
    vIO32WriteFldAlign(TDSHP_16, IO32ReadFldAlign(TDSHP_LIMIT_00, TDSHARP_LIMIT_H2), TDS_LIMIT_NEG_H2);  
    vIO32WriteFldAlign(TDSHP_16, IO32ReadFldAlign(TDSHP_LIMIT_00, TDSHARP_LIMIT_H3), TDS_LIMIT_NEG_H3);  
    vIO32WriteFldAlign(TDSHP_16, IO32ReadFldAlign(TDSHP_LIMIT_00, TDSHARP_LIMIT_D1), TDS_LIMIT_NEG_D1);  
    vIO32WriteFldAlign(TDSHP_17, IO32ReadFldAlign(TDSHP_LIMIT_01, TDSHARP_LIMIT_D2), TDS_LIMIT_NEG_D2);  
    vIO32WriteFldAlign(TDSHP_17, IO32ReadFldAlign(TDSHP_LIMIT_01, TDSHARP_LIMIT_D3), TDS_LIMIT_NEG_D3); 
    vIO32WriteFldAlign(TDSHP_17, IO32ReadFldAlign(TDSHP_LIMIT_01, TDSHARP_LIMIT_V1), TDS_LIMIT_NEG_V1);  
    vIO32WriteFldAlign(TDSHP_17, IO32ReadFldAlign(TDSHP_LIMIT_01, TDSHARP_LIMIT_V2), TDS_LIMIT_NEG_V2);  
    vIO32WriteFldAlign(TDSHP_18, IO32ReadFldAlign(TDSHP_LIMIT_02, LTI_LIMIT_LTI_H1), TDS_LIMIT_NEG_LTI_H1);  
    vIO32WriteFldAlign(TDSHP_18, IO32ReadFldAlign(TDSHP_LIMIT_02, LTI_LIMIT_LTI_H2), TDS_LIMIT_NEG_LTI_H2);  
    vIO32WriteFldAlign(TDSHP_18, IO32ReadFldAlign(TDSHP_LIMIT_02, LTI_LIMIT_LTI_V), TDS_LIMIT_NEG_LTI_V);  
    vIO32WriteFldAlign(TDSHP_28, IO32ReadFldAlign(TDSHP_LIMIT_02, TDSHARP_LIMIT), TDS_LIMIT_NEG);           	

    vIO32WriteFldAlign(TDSHP_1B, IO32ReadFldAlign(TDSHP_LIMIT_03, TDSHARP_BOUND_H1), TDS_BOUND_POS_H1);  
    vIO32WriteFldAlign(TDSHP_1B, IO32ReadFldAlign(TDSHP_LIMIT_03, TDSHARP_BOUND_H2), TDS_BOUND_POS_H2);  
    vIO32WriteFldAlign(TDSHP_1B, IO32ReadFldAlign(TDSHP_LIMIT_03, TDSHARP_BOUND_H3), TDS_BOUND_POS_H3);  
    vIO32WriteFldAlign(TDSHP_1B, IO32ReadFldAlign(TDSHP_LIMIT_03, TDSHARP_BOUND_D1), TDS_BOUND_POS_D1);  
    vIO32WriteFldAlign(TDSHP_1C, IO32ReadFldAlign(TDSHP_LIMIT_04, TDSHARP_BOUND_D2), TDS_BOUND_POS_D2);  
    vIO32WriteFldAlign(TDSHP_1C, IO32ReadFldAlign(TDSHP_LIMIT_04, TDSHARP_BOUND_D3), TDS_BOUND_POS_D3);  
    vIO32WriteFldAlign(TDSHP_1C, IO32ReadFldAlign(TDSHP_LIMIT_04, TDSHARP_BOUND_V1), TDS_BOUND_POS_V1);  
    vIO32WriteFldAlign(TDSHP_1C, IO32ReadFldAlign(TDSHP_LIMIT_04, TDSHARP_BOUND_V2), TDS_BOUND_POS_V2);  
    vIO32WriteFldAlign(TDSHP_1D, IO32ReadFldAlign(TDSHP_LIMIT_05, LTI_BOUND_LTI_H1), TDS_BOUND_POS_LTI_H1);  
    vIO32WriteFldAlign(TDSHP_1D, IO32ReadFldAlign(TDSHP_LIMIT_05, LTI_BOUND_LTI_H2), TDS_BOUND_POS_LTI_H2);  
    vIO32WriteFldAlign(TDSHP_1D, IO32ReadFldAlign(TDSHP_LIMIT_05, LTI_BOUND_LTI_V), TDS_BOUND_POS_LTI_V);  
    vIO32WriteFldAlign(TDSHP_28, IO32ReadFldAlign(TDSHP_LIMIT_05, TDSHARP_BOUND), TDS_BOUND_POS); 

    vIO32WriteFldAlign(TDSHP_1E, IO32ReadFldAlign(TDSHP_LIMIT_03, TDSHARP_BOUND_H1), TDS_BOUND_NEG_H1);  
    vIO32WriteFldAlign(TDSHP_1E, IO32ReadFldAlign(TDSHP_LIMIT_03, TDSHARP_BOUND_H2), TDS_BOUND_NEG_H2);  
    vIO32WriteFldAlign(TDSHP_1E, IO32ReadFldAlign(TDSHP_LIMIT_03, TDSHARP_BOUND_H3), TDS_BOUND_NEG_H3);  
    vIO32WriteFldAlign(TDSHP_1E, IO32ReadFldAlign(TDSHP_LIMIT_03, TDSHARP_BOUND_D1), TDS_BOUND_NEG_D1);  
    vIO32WriteFldAlign(TDSHP_1F, IO32ReadFldAlign(TDSHP_LIMIT_04, TDSHARP_BOUND_D2), TDS_BOUND_NEG_D2);  
    vIO32WriteFldAlign(TDSHP_1F, IO32ReadFldAlign(TDSHP_LIMIT_04, TDSHARP_BOUND_D3), TDS_BOUND_NEG_D3); 
    vIO32WriteFldAlign(TDSHP_1F, IO32ReadFldAlign(TDSHP_LIMIT_04, TDSHARP_BOUND_V1), TDS_BOUND_NEG_V1);  
    vIO32WriteFldAlign(TDSHP_1F, IO32ReadFldAlign(TDSHP_LIMIT_04, TDSHARP_BOUND_V2), TDS_BOUND_NEG_V2);       
    vIO32WriteFldAlign(TDSHP_20, IO32ReadFldAlign(TDSHP_LIMIT_05, LTI_BOUND_LTI_H1), TDS_BOUND_NEG_LTI_H1);  
    vIO32WriteFldAlign(TDSHP_20, IO32ReadFldAlign(TDSHP_LIMIT_05, LTI_BOUND_LTI_H2), TDS_BOUND_NEG_LTI_H2);  
    vIO32WriteFldAlign(TDSHP_20, IO32ReadFldAlign(TDSHP_LIMIT_05, LTI_BOUND_LTI_V), TDS_BOUND_NEG_LTI_V);  
    vIO32WriteFldAlign(TDSHP_28, IO32ReadFldAlign(TDSHP_LIMIT_05, TDSHARP_BOUND), TDS_BOUND_NEG);     	
#endif
}

static void vDrvPBCProc(void)
{
#ifdef CC_MT5398
	//can be removed later.
	static UINT16 wRadius1 = 0xffff;
    static UINT16 wRange1 = 0xffff;
    static UINT16 wThRange1 = 0xffff;
    static UINT16 wRadius2 = 0xffff;
    static UINT16 wRange2 = 0xffff;
    static UINT16 wThRange2 = 0xffff;
    static UINT16 wRadius3 = 0xffff;
    static UINT16 wRange3 = 0xffff;
    static UINT16 wThRange3 = 0xffff;
    
    UINT16 wRadius, wRange, wThRange;

    // update PBC 1 register
    wRadius = IO32ReadFldAlign(PBC_SHP1_02, PBC1_RADIUS_C);
    wRange = IO32ReadFldAlign(PBC_SHP1_01, PBC1_RADIUS_R);
    wThRange = IO32ReadFldAlign(PBC_SHP1_01, PBC1_THETA_R);

    if ((wRadius1 != wRadius) || (wRange1 != wRange) || (wThRange1 != wThRange))
    {
        if ((wRadius !=0 ) && (wRange != 0) && (wThRange != 0))
        {
            vIO32WriteFldAlign(PBC_SHP1_03, 4096 / wRange, PBC1_RSLOPE);
            vIO32WriteFldAlign(PBC_SHP1_02, 4096 / wThRange, PBC1_TSLOPE);
            vIO32WriteFldAlign(PBC_SHP1_01, 4096 / ((wRange > wRadius) ? wRadius : wRange), PBC1_RSLOPE_1);
        }
        wRadius1 = wRadius;
        wRange1 = wRange;
        wThRange1 = wThRange;
    }

    // update PBC 2 register
    wRadius = IO32ReadFldAlign(PBC_SHP2_02, PBC2_RADIUS_C);
    wRange = IO32ReadFldAlign(PBC_SHP2_01, PBC2_RADIUS_R);
    wThRange = IO32ReadFldAlign(PBC_SHP2_01, PBC2_THETA_R);

    if ((wRadius2 != wRadius) || (wRange2 != wRange) || (wThRange2 != wThRange))
    {
        if ((wRadius !=0 ) && (wRange != 0) && (wThRange != 0))
        {
            vIO32WriteFldAlign(PBC_SHP2_03, 4096 / wRange, PBC2_RSLOPE);
            vIO32WriteFldAlign(PBC_SHP2_02, 4096 / wThRange, PBC2_TSLOPE);
            vIO32WriteFldAlign(PBC_SHP2_01, 4096 / ((wRange > wRadius) ? wRadius : wRange), PBC2_RSLOPE_1);
        }
        wRadius2 = wRadius;
        wRange2 = wRange;
        wThRange2 = wThRange;
    }

    // update PBC 3 register
    wRadius = IO32ReadFldAlign(PBC_SHP3_02, PBC3_RADIUS_C);
    wRange = IO32ReadFldAlign(PBC_SHP3_01, PBC3_RADIUS_R);
    wThRange = IO32ReadFldAlign(PBC_SHP3_01, PBC3_THETA_R);

    if ((wRadius3 != wRadius) || (wRange3 != wRange) || (wThRange3 != wThRange))
    {
        if ((wRadius !=0 ) && (wRange != 0) && (wThRange != 0))
        {
            vIO32WriteFldAlign(PBC_SHP3_03, 4096 / wRange, PBC3_RSLOPE);
            vIO32WriteFldAlign(PBC_SHP3_02, 4096 / wThRange, PBC2_TSLOPE);
            vIO32WriteFldAlign(PBC_SHP3_01, 4096 / ((wRange > wRadius) ? wRadius : wRange), PBC3_RSLOPE_1);
        }
        wRadius3 = wRadius;
        wRange3 = wRange;
        wThRange3 = wThRange;
    }
#else
	//can be removed later.
	static UINT16 wPBC1_ThetaC1 = 0xffff;
	static UINT16 wPBC1_ThetaC2 = 0xffff;
	static UINT16 wPBC1_RadiusC1 = 0xffff;
	static UINT16 wPBC1_RadiusC2 = 0xffff;
	static UINT16 wPBC1_Theta_R = 0xffff;
	static UINT16 wPBC1_Radius_R = 0xffff;
	
	static UINT16 wPBC2_ThetaC1 = 0xffff;
	static UINT16 wPBC2_ThetaC2 = 0xffff;
	static UINT16 wPBC2_RadiusC1 = 0xffff;
	static UINT16 wPBC2_RadiusC2 = 0xffff;
	static UINT16 wPBC2_Theta_R = 0xffff;
	static UINT16 wPBC2_Radius_R = 0xffff;
	
	static UINT16 wPBC3_ThetaC1 = 0xffff;
	static UINT16 wPBC3_ThetaC2 = 0xffff;
	static UINT16 wPBC3_RadiusC1 = 0xffff;
	static UINT16 wPBC3_RadiusC2 = 0xffff;	
	static UINT16 wPBC3_Theta_R = 0xffff;
	static UINT16 wPBC3_Radius_R = 0xffff;
	
	// update PBC 1 register
	UINT16 wThetaC1, wThetaC2, wRadiusC1, wRadiusC2, wTheta_R, wRadius_R;

	wThetaC1= IO32ReadFldAlign(CDS_01,PBC1_THETA_C1);
	wThetaC2= IO32ReadFldAlign(CDS_02,PBC1_THETA_C2);
	wRadiusC1= IO32ReadFldAlign(CDS_01,PBC1_RADIUS_C1);
	wRadiusC2= IO32ReadFldAlign(CDS_02,PBC1_RADIUS_C2);
	wTheta_R= IO32ReadFldAlign(CDS_00,PBC1_THETA_R);
	wRadius_R= IO32ReadFldAlign(CDS_00,PBC1_RADIUS_R);
	#ifndef CUST_SOEM_DRV
	if ((wPBC1_ThetaC1  != wThetaC1) || 
		(wPBC1_ThetaC2  != wThetaC2) || 
		(wPBC1_RadiusC1 != wRadiusC1) ||
		(wPBC1_RadiusC2 != wRadiusC2) ||
		(wPBC1_Theta_R  != wTheta_R) ||		
		(wPBC1_Radius_R != wRadius_R))
	#else	
	if ((wPBC1_ThetaC1  != wThetaC1) || 
		(wPBC1_ThetaC2  != wThetaC2) || 
		(wPBC1_RadiusC1 != wRadiusC1) ||
		(wPBC1_RadiusC2 != wRadiusC2) ||
		(wPBC1_Theta_R  != wTheta_R) ||		
		(wPBC1_Radius_R != wRadius_R) ||
		(fgSharpForce))
	#endif
	{
		wPBC1_ThetaC1= wThetaC1;
		wPBC1_ThetaC2= wThetaC2;
		wPBC1_RadiusC1= wRadiusC1;
		wPBC1_RadiusC2= wRadiusC2;
		wPBC1_Theta_R= wTheta_R;	
		wPBC1_Radius_R= wRadius_R;
		if ((wPBC1_Theta_R !=0 ) && (wPBC1_Radius_R != 0) && (wPBC1_RadiusC1 != 0))
		{
			vIO32WriteFldAlign(CDS_03, 4096/wPBC1_Radius_R , PBC1_RSLOPE);
			vIO32WriteFldAlign(CDS_01, 4096/wPBC1_Theta_R, PBC1_TSLOPE);
			vIO32WriteFldAlign(CDS_00, 4096/MIN(wPBC1_RadiusC1, wPBC1_Radius_R), PBC1_RSLOPE_1);
		}
	}

	// update PBC 2 register
	wThetaC1= IO32ReadFldAlign(CDS_2B,PBC2_THETA_C1);
	wThetaC2= IO32ReadFldAlign(CDS_2C,PBC2_THETA_C2);
	wRadiusC1= IO32ReadFldAlign(CDS_2B,PBC2_RADIUS_C1);
	wRadiusC2= IO32ReadFldAlign(CDS_2C,PBC2_RADIUS_C2);
	wTheta_R= IO32ReadFldAlign(CDS_2A,PBC2_THETA_R);
	wRadius_R= IO32ReadFldAlign(CDS_2A,PBC2_RADIUS_R);

	if ((wPBC2_ThetaC1  != wThetaC1) || 
		(wPBC2_ThetaC2  != wThetaC2) || 
		(wPBC2_RadiusC1 != wRadiusC1) ||
		(wPBC2_RadiusC2 != wRadiusC2) ||
		(wPBC2_Theta_R  != wTheta_R) ||		
		(wPBC2_Radius_R != wRadius_R))
	{
		wPBC2_ThetaC1= wThetaC1;
		wPBC2_ThetaC2= wThetaC2;
		wPBC2_RadiusC1= wRadiusC1;
		wPBC2_RadiusC2= wRadiusC2;
		wPBC2_Theta_R= wTheta_R;	
		wPBC2_Radius_R= wRadius_R;	
		if ((wPBC2_Theta_R !=0 ) && (wPBC2_Radius_R != 0) && (wPBC2_RadiusC1 != 0))
		{
			vIO32WriteFldAlign(CDS_2D, 4096/wPBC2_Radius_R, PBC2_RSLOPE);
			vIO32WriteFldAlign(CDS_2B, 4096/wPBC2_Theta_R , PBC2_TSLOPE);
			vIO32WriteFldAlign(CDS_2A, 4096/MIN(wPBC2_RadiusC1, wPBC2_Radius_R), PBC2_RSLOPE_1);
		}
	}


	// update PBC 3 register
	wThetaC1= IO32ReadFldAlign(CDS_55,PBC3_THETA_C1);
	wThetaC2= IO32ReadFldAlign(CDS_56,PBC3_THETA_C2);
	wRadiusC1= IO32ReadFldAlign(CDS_55,PBC3_RADIUS_C1);
	wRadiusC2= IO32ReadFldAlign(CDS_56,PBC3_RADIUS_C2);
	wTheta_R= IO32ReadFldAlign(CDS_54,PBC3_THETA_R);
	wRadius_R= IO32ReadFldAlign(CDS_54,PBC3_RADIUS_R);

	if ((wPBC3_ThetaC1  != wThetaC1) || 
		(wPBC3_ThetaC2  != wThetaC2) || 
		(wPBC3_RadiusC1 != wRadiusC1) ||
		(wPBC3_RadiusC2 != wRadiusC2) ||
		(wPBC3_Theta_R  != wTheta_R) ||		
		(wPBC3_Radius_R != wRadius_R))
	{
		wPBC3_ThetaC1= wThetaC1;
		wPBC3_ThetaC2= wThetaC2;
		wPBC3_RadiusC1= wRadiusC1;
		wPBC3_RadiusC2= wRadiusC2;
		wPBC3_Theta_R= wTheta_R;	
		wPBC3_Radius_R= wRadius_R;

		if ((wPBC3_Theta_R !=0 ) && (wPBC3_Radius_R != 0) && (wPBC3_RadiusC1 != 0))
		{
			vIO32WriteFldAlign(CDS_57, 4096/wPBC3_Radius_R, PBC3_RSLOPE);
			vIO32WriteFldAlign(CDS_55, 4096/wPBC3_Theta_R , PBC3_TSLOPE);
			vIO32WriteFldAlign(CDS_54, 4096/MIN(wPBC3_RadiusC1, wPBC3_Radius_R), PBC3_RSLOPE_1);
		}
	}

#endif	
}

/*****************************************************************************************/
/**********************************   TDSHARP YLEV  ****************************************/
/*****************************************************************************************/
/**
 * @brief Y Level sharpness initial parameters
 * @param
 */
void vDrvYlevSharpInit(void)
{
    vDrvLoadYLevTable();   
#ifdef CC_MT5398
    vIO32WriteFldAlign(TDSHP_2E, 0x01, TDS_YLEV_EN);
#else
	vIO32WriteFldAlign(SHARP_4B, 0x01, TDS_YLEV_EN);
#endif	
}

/**
 * @brief Write Post sharpness Y Level Table
 * @      in VSYNC.
 * @param
 */
void vDrvLoadYLevTable(void)
{  
#ifdef CC_MT5398
	vIO32Write4B(TDSHP_2A, TDS_YLEV_32BIT_VALUE[0]);  
	vIO32Write4B(TDSHP_2B, TDS_YLEV_32BIT_VALUE[1]); 
	vIO32Write4B(TDSHP_2C, TDS_YLEV_32BIT_VALUE[2]); 
	vIO32Write4B(TDSHP_2D, TDS_YLEV_32BIT_VALUE[3]); 
	vIO32WriteFldAlign(TDSHP_2E, TDS_YLEV_32BIT_VALUE[4], TDS_YLEV_P1024); 
#else
	vIO32Write4B(SHARP_47, TDS_YLEV_32BIT_VALUE[0]);
	vIO32Write4B(SHARP_48, TDS_YLEV_32BIT_VALUE[1]);
	vIO32Write4B(SHARP_49, TDS_YLEV_32BIT_VALUE[2]);
	vIO32Write4B(SHARP_4A, TDS_YLEV_32BIT_VALUE[3]);
	vIO32WriteFldAlign(SHARP_4B, TDS_YLEV_32BIT_VALUE[4], TDS_YLEV_P1024); 
#endif		
	fgAdapYLEVLock = SV_OFF;
}

/**
 * @brief Read Post sharpness Y Level Table
 * @param
 */
BOOL bDrvReadYLevTable(void)
{
	UINT8 bIndex;
	for (bIndex = 0; bIndex < 17; bIndex++)
	{
		Printf("YLEV = %d, ", bIndex*64);
		Printf("Table = %d\n", TDS_YLEV[bIndex]);
	}
	return SV_TRUE;
}

/**
 * @brief Calculate manual gain curve with 4 control points
 * @param
 */
 #define TDS_GLOBAL_GAIN_UNIT 0x20
void vDrvCalYLevTable(void)
{
    UINT8 i, bIndex, bPoint[4], bGain[4];

    bPoint[0] = IO32ReadFldAlign(TDPROC_YLEV_01, TDS_YLEV_P1);
    bPoint[1] = IO32ReadFldAlign(TDPROC_YLEV_01, TDS_YLEV_P2);
    bPoint[2] = IO32ReadFldAlign(TDPROC_YLEV_01, TDS_YLEV_P3);
    bPoint[3] = IO32ReadFldAlign(TDPROC_YLEV_01, TDS_YLEV_P4);
    bGain[0] = IO32ReadFldAlign(TDPROC_YLEV_02, TDS_YLEV_G1);        
    bGain[1] = IO32ReadFldAlign(TDPROC_YLEV_02, TDS_YLEV_G2);        
    bGain[2] = IO32ReadFldAlign(TDPROC_YLEV_02, TDS_YLEV_G3);
    bGain[3] = IO32ReadFldAlign(TDPROC_YLEV_02, TDS_YLEV_G4);

    for (i = 0; i < 3; i ++)    // Make P1-P4 monotonic increasement
    {
        if (bPoint[i] >= bPoint[i+1])
        {
            bPoint[i] = bPoint[i+1] - 1;
        }
    }

    for (bIndex = 0; bIndex < 17; bIndex ++)
    {
        if (bIndex <= bPoint[0])
        {
            TDS_YLEV[bIndex] = bGain[0];
        }
        else if (bIndex >= bPoint[3])
        {
            TDS_YLEV[bIndex] = bGain[3];
        }
        else
        {
            for (i = 0; i <= 2; i ++)
            {
                if ((bIndex >= bPoint[i]) && ((bIndex <= bPoint[i+1])))
                {
                    TDS_YLEV[bIndex] = (UINT16)((bGain[i+1] - bGain[i]) *
                                    ((UINT16)bIndex - bPoint[i]) / (bPoint[i+1] - bPoint[i])
                                    + bGain[i]);
                    break;
                }
            }
        }
    }
}

/**
 * @brief Calculate adaptive gain curve 
 * @param
 */
void vDrvCalYLevAdlTable(void)
{ 
    UINT8 i;
    UINT16 wTmp;
    UINT8 ADL_GAIN;
      
    ADL_GAIN = IO32ReadFldAlign(TDPROC_YLEV_02, TDS_YLEV_ADL_GAIN); 
    
    // Calculate gain for even points
    for (i = 0; i < 16; i ++)
    {
        wTmp = waLumaArray[2*i + 1] - waLumaArray[2*i];

        // slope = 0x20/wTmp, Gain=((slope*0x80)-0x80)*(Adap_Gain/0x80)+0x80
        TDS_ADAP_YLEV[i ] = (wTmp == 0) ? 0xFF : 
            ((((0x1000/ wTmp) - 0x80) * ADL_GAIN)>> 7) + 0x80;
    }
    // Calculate gain for index 17
    TDS_ADAP_YLEV[16] = (2 * TDS_ADAP_YLEV[15] - TDS_ADAP_YLEV[14]);

    // Multiply adaptive gain curve by manual gain curve
    for (i = 0; i < 17; i ++)
    {
        wTmp = (TDS_ADAP_YLEV[i] * TDS_YLEV[i]) >> 7;
        TDS_YLEV[i] = (wTmp > 0x80) ? 0x80 : wTmp;
    }
}

void vDrvCalYLevAplTable(void)
{
    UINT8 bIndex;
    UINT8 bPoint[2];
    UINT16 wTmp;    	
    UINT8 bAPL = bDrvGetAPL();       
    // YLEV_APL_TBL 	
    bPoint[0] = IO32ReadFldAlign(TDPROC_YLEV_00, TDS_YLEV_APL_ZERO);
    bPoint[1] = IO32ReadFldAlign(TDPROC_YLEV_00, TDS_YLEV_APL_THR);  
    
    if ( bPoint[0]> bPoint[1])	// Make Table monotonic increasement
    {
         bPoint[0] =  bPoint[1];
    }
    
    for (bIndex = 0; bIndex < 17; bIndex ++)	// APLLUT Calculation 
    {
        if (bIndex <= bPoint[0])
        {
            TDS_YLEV_APL[bIndex] = 0x00;
        }
        else if (bIndex >= bPoint[1])
        {
            TDS_YLEV_APL[bIndex] = 0x80;
        }        
        else
        {
            TDS_YLEV_APL[bIndex] = (UINT16)(0x80 *((UINT16)bIndex - bPoint[0]) / (bPoint[1] - bPoint[0]));
        }    
    }
    	  
    for (bIndex = 0; bIndex < 17; bIndex++)	// YLEV Blending by YLEV_APL
    {
        wTmp = (TDS_YLEV[bIndex]*(0x80 - TDS_YLEV_APL[bAPL >> 4] ) + 0x80 * TDS_YLEV_APL[bAPL >> 4] ) >> 7;
        TDS_YLEV[bIndex] = (wTmp > 0x80) ? 0x80 : wTmp;   
    }
}
void vDrvGlobalGainbyYLEV(void)
{
	UINT32 Tmp1, Tmp2, Tmp3, Tmp4;
	UINT16 wGlobalGain = IO32ReadFldAlign(TDSHP_GLOBAL, TDS_GLOBAL_GAIN);
	
#ifdef CC_MT5398
	wGlobalGain=0x20;
#endif
	
    #ifdef CUST_SOEM_DRV
    // ---Begin Masoem. RF input, weak signal level, adaptive adjust Sharpness
    extern UINT8 bGetDecodeType(UINT8 bSrc);
    extern UINT8 bGetSignalType(UINT8 bPath);
    
    if((bGetDecodeType(SV_VP_MAIN) == SV_VD_TVD3D)
        &&(bGetSignalType(SV_VP_MAIN) == SV_ST_TV) 
        &&(IO32ReadFldAlign(ADAP_SHARPNESS_00, ADAP_SHARP_ENABLE)))    //ATV
    {
        UINT8 bsignal_level = bTvdSignalStrengthLevel(1);
        UINT8 bTHD_DN = IO32ReadFldAlign(ADAP_SHARPNESS_00, ADAP_SHARP_THD_DN);
        UINT8 bTHD_UP = IO32ReadFldAlign(ADAP_SHARPNESS_00, ADAP_SHARP_THD_UP);
        UINT8 bGAIN_DN = IO32ReadFldAlign(ADAP_SHARPNESS_00, ADAP_SHARP_GAIN_DN);
        UINT8 bGAIN_UP = wReadQualityTable(QUALITY_TDSHARPALL_GAIN);

        wGlobalGain = (bsignal_level >= bTHD_UP) ? bGAIN_UP :   \
                     ((bsignal_level <= bTHD_DN) ? bGAIN_DN :  \
         (bGAIN_UP - (bTHD_UP - bsignal_level)*(bGAIN_UP - bGAIN_DN )/(bTHD_UP - bTHD_DN)));

        vIO32WriteFldAlign(TDSHP_GLOBAL,wGlobalGain,TDS_GLOBAL_GAIN);
        if(IO32ReadFldAlign(ADAP_SHARPNESS_00, ADAP_SHARP_LOG))
        {
            Printf("\n[Drv_sharpness] Sharpness AllGain [0x%x] -> [0x%x]\n",
                wReadQualityTable(QUALITY_TDSHARPALL_GAIN),wGlobalGain);
        }
    }
    // ---End
    #endif

	Tmp1=MIN((wGlobalGain*TDS_YLEV[0])/TDS_GLOBAL_GAIN_UNIT, 0xFF);
	Tmp2=MIN((wGlobalGain*TDS_YLEV[1])/TDS_GLOBAL_GAIN_UNIT, 0xFF);
	Tmp3=MIN((wGlobalGain*TDS_YLEV[2])/TDS_GLOBAL_GAIN_UNIT, 0xFF);
	Tmp4=MIN((wGlobalGain*TDS_YLEV[3])/TDS_GLOBAL_GAIN_UNIT, 0xFF);
	TDS_YLEV_32BIT_VALUE[0]= (Tmp1<<24) + (Tmp2<<16) + (Tmp3<<8) + (Tmp4);
	//LOG("YLEV %d %d %d %d\n", Tmp1, Tmp2, Tmp3, Tmp4);
	Tmp1=MIN((wGlobalGain*TDS_YLEV[4])/TDS_GLOBAL_GAIN_UNIT, 0xFF);
	Tmp2=MIN((wGlobalGain*TDS_YLEV[5])/TDS_GLOBAL_GAIN_UNIT, 0xFF);
	Tmp3=MIN((wGlobalGain*TDS_YLEV[6])/TDS_GLOBAL_GAIN_UNIT, 0xFF);
	Tmp4=MIN((wGlobalGain*TDS_YLEV[7])/TDS_GLOBAL_GAIN_UNIT, 0xFF);
	TDS_YLEV_32BIT_VALUE[1]= (Tmp1<<24) + (Tmp2<<16) + (Tmp3<<8) + (Tmp4);

	Tmp1=MIN((wGlobalGain*TDS_YLEV[8])/TDS_GLOBAL_GAIN_UNIT, 0xFF);
	Tmp2=MIN((wGlobalGain*TDS_YLEV[9])/TDS_GLOBAL_GAIN_UNIT, 0xFF);
	Tmp3=MIN((wGlobalGain*TDS_YLEV[10])/TDS_GLOBAL_GAIN_UNIT, 0xFF);
	Tmp4=MIN((wGlobalGain*TDS_YLEV[11])/TDS_GLOBAL_GAIN_UNIT, 0xFF);
	TDS_YLEV_32BIT_VALUE[2]= (Tmp1<<24) + (Tmp2<<16) + (Tmp3<<8) + (Tmp4);

	Tmp1=MIN((wGlobalGain*TDS_YLEV[12])/TDS_GLOBAL_GAIN_UNIT, 0xFF);
	Tmp2=MIN((wGlobalGain*TDS_YLEV[13])/TDS_GLOBAL_GAIN_UNIT, 0xFF);
	Tmp3=MIN((wGlobalGain*TDS_YLEV[14])/TDS_GLOBAL_GAIN_UNIT, 0xFF);
	Tmp4=MIN((wGlobalGain*TDS_YLEV[15])/TDS_GLOBAL_GAIN_UNIT, 0xFF);
	TDS_YLEV_32BIT_VALUE[3]= (Tmp1<<24) + (Tmp2<<16) + (Tmp3<<8) + (Tmp4);
	
	Tmp1=MIN((wGlobalGain*TDS_YLEV[16])/TDS_GLOBAL_GAIN_UNIT, 0xFF);
	TDS_YLEV_32BIT_VALUE[4]= Tmp1; 
}

/**
 * @brief Enable/Disable adaptive Y level dependent sharpness
 * @param
 */
void vDrvAdapYLev(void)
{
    if (IO32ReadFldAlign(TDPROC_YLEV_00, TDS_YLEV_ADAP_ENA))
    {
    	if(fgAdapYLEVLock == SV_OFF)
    	{
    		fgAdapYLEVLock = SV_ON;
	        vDrvCalYLevTable();
	        if (IO32ReadFldAlign(TDPROC_YLEV_00, TDS_YLEV_ADL_ENA))
	        {
	            vDrvCalYLevAdlTable(); 
	        }

	        if (IO32ReadFldAlign(TDPROC_YLEV_00, TDS_YLEV_APL_ENA))
	        {	
	            vDrvCalYLevAplTable();        
	        }	 
    	}
    }
	vDrvGlobalGainbyYLEV();
	vApiRegisterVideoEvent(PE_EVENT_SHARPNESS, SV_VP_MAIN, SV_ON);
}

/*****************************************************************************************/
/************************************  3D SHARP ******************************************/
/*****************************************************************************************/
UINT16 wDrvGetTDShp3DGain(UINT16 wInputGain, UINT16 wWeighting)
{
    return ((wInputGain*wWeighting)>>7);
}

void vDrvGetSharp3DWeighting(void)
{
    UINT8 bMode;
	UINT8 b3DHGain, b3DDGain, b3DVGain;
	UINT8 b3DHClip, b3DDClip, b3DVClip;
    bMode = IO32ReadFldAlign(SHARPNESS_3D_00, SHARPNESS_3D_MODE);
    
    switch (bMode)
    {    
		   case E_TDTV_UI_3D_MODE_TTD:		   // 2D to 3D
			   b3DHGain = IO32ReadFldAlign(SHARPNESS_3D_02, SHARPNESS_TTD_HGAIN); 
			   b3DDGain = IO32ReadFldAlign(SHARPNESS_3D_02, SHARPNESS_TTD_DGAIN);				   
			   b3DVGain = IO32ReadFldAlign(SHARPNESS_3D_02, SHARPNESS_TTD_VGAIN); 
			   b3DHClip = IO32ReadFldAlign(SHARPNESS_3D_0B, SHARPNESS_TTD_HCLIP); 
			   b3DDClip = IO32ReadFldAlign(SHARPNESS_3D_0B, SHARPNESS_TTD_DCLIP); 
			   b3DVClip = IO32ReadFldAlign(SHARPNESS_3D_0B, SHARPNESS_TTD_VCLIP); 
			   break;				 
		   case E_TDTV_UI_3D_MODE_FS:		   //Frame Seq & Frame Packing
			   b3DHGain = IO32ReadFldAlign(SHARPNESS_3D_01, SHARPNESS_FS_HGAIN); 
			   b3DDGain = IO32ReadFldAlign(SHARPNESS_3D_01, SHARPNESS_FS_DGAIN);				  
			   b3DVGain = IO32ReadFldAlign(SHARPNESS_3D_01, SHARPNESS_FS_VGAIN);
			   b3DHClip = IO32ReadFldAlign(SHARPNESS_3D_0A, SHARPNESS_FS_HCLIP); 
			   b3DDClip = IO32ReadFldAlign(SHARPNESS_3D_0A, SHARPNESS_FS_DCLIP); 
			   b3DVClip = IO32ReadFldAlign(SHARPNESS_3D_0A, SHARPNESS_FS_VCLIP);			   
			   break;				 
		   case E_TDTV_UI_3D_MODE_TB:			   //Top and Bottom
			   b3DHGain = IO32ReadFldAlign(SHARPNESS_3D_04, SHARPNESS_TB_HGAIN); 
			   b3DDGain = IO32ReadFldAlign(SHARPNESS_3D_04, SHARPNESS_TB_DGAIN);				  
			   b3DVGain = IO32ReadFldAlign(SHARPNESS_3D_04, SHARPNESS_TB_VGAIN);
			   b3DHClip = IO32ReadFldAlign(SHARPNESS_3D_0D, SHARPNESS_TB_HCLIP); 
			   b3DDClip = IO32ReadFldAlign(SHARPNESS_3D_0D, SHARPNESS_TB_DCLIP); 
			   b3DVClip = IO32ReadFldAlign(SHARPNESS_3D_0D, SHARPNESS_TB_VCLIP);			   
			   break;
		   case E_TDTV_UI_3D_MODE_SBS:		   //Side by Side
			   b3DHGain = IO32ReadFldAlign(SHARPNESS_3D_03, SHARPNESS_SBS_HGAIN); 
			   b3DDGain = IO32ReadFldAlign(SHARPNESS_3D_03, SHARPNESS_SBS_DGAIN); 
			   b3DVGain = IO32ReadFldAlign(SHARPNESS_3D_03, SHARPNESS_SBS_VGAIN); 
			   b3DHClip = IO32ReadFldAlign(SHARPNESS_3D_0C, SHARPNESS_SBS_HCLIP); 
			   b3DDClip = IO32ReadFldAlign(SHARPNESS_3D_0C, SHARPNESS_SBS_DCLIP); 
			   b3DVClip = IO32ReadFldAlign(SHARPNESS_3D_0C, SHARPNESS_SBS_VCLIP);			   
			   break;
		   case E_TDTV_UI_3D_MODE_RD:		   //RealD
			   b3DHGain = IO32ReadFldAlign(SHARPNESS_3D_06, SHARPNESS_RD_HGAIN); 
			   b3DDGain = IO32ReadFldAlign(SHARPNESS_3D_06, SHARPNESS_RD_DGAIN);				  
			   b3DVGain = IO32ReadFldAlign(SHARPNESS_3D_06, SHARPNESS_RD_VGAIN);
			   b3DHClip = IO32ReadFldAlign(SHARPNESS_3D_0F, SHARPNESS_RD_HCLIP); 
			   b3DDClip = IO32ReadFldAlign(SHARPNESS_3D_0F, SHARPNESS_RD_DCLIP); 
			   b3DVClip = IO32ReadFldAlign(SHARPNESS_3D_0F, SHARPNESS_RD_VCLIP);			   
			   break;
		   case E_TDTV_UI_3D_MODE_SS:			   //Sensio
			   b3DHGain = IO32ReadFldAlign(SHARPNESS_3D_05, SHARPNESS_SS_HGAIN); 
			   b3DDGain = IO32ReadFldAlign(SHARPNESS_3D_05, SHARPNESS_SS_HGAIN);				  
			   b3DVGain = IO32ReadFldAlign(SHARPNESS_3D_05, SHARPNESS_SS_VGAIN); 
			   b3DHClip = IO32ReadFldAlign(SHARPNESS_3D_0E, SHARPNESS_SS_HCLIP); 
			   b3DDClip = IO32ReadFldAlign(SHARPNESS_3D_0E, SHARPNESS_SS_DCLIP); 
			   b3DVClip = IO32ReadFldAlign(SHARPNESS_3D_0E, SHARPNESS_SS_VCLIP);			   
			   break;
		case E_TDTV_UI_3D_MODE_LI:			   //Line Interleave		 
			   b3DHGain = IO32ReadFldAlign(SHARPNESS_3D_07, SHARPNESS_LI_HGAIN); 
			   b3DDGain = IO32ReadFldAlign(SHARPNESS_3D_07, SHARPNESS_LI_DGAIN);				  
			   b3DVGain = IO32ReadFldAlign(SHARPNESS_3D_07, SHARPNESS_LI_VGAIN); 
			   b3DHClip = IO32ReadFldAlign(SHARPNESS_3D_10, SHARPNESS_LI_HCLIP); 
			   b3DDClip = IO32ReadFldAlign(SHARPNESS_3D_10, SHARPNESS_LI_DCLIP); 
			   b3DVClip = IO32ReadFldAlign(SHARPNESS_3D_10, SHARPNESS_LI_VCLIP);			   
			   break;
		   case E_TDTV_UI_3D_MODE_CB:		   //Checker Board
			   b3DHGain = IO32ReadFldAlign(SHARPNESS_3D_08, SHARPNESS_CB_HGAIN); 
			   b3DDGain = IO32ReadFldAlign(SHARPNESS_3D_08, SHARPNESS_CB_DGAIN);				  
			   b3DVGain = IO32ReadFldAlign(SHARPNESS_3D_08, SHARPNESS_CB_VGAIN); 
			   b3DHClip = IO32ReadFldAlign(SHARPNESS_3D_11, SHARPNESS_CB_HCLIP); 
			   b3DDClip = IO32ReadFldAlign(SHARPNESS_3D_11, SHARPNESS_CB_DCLIP); 
			   b3DVClip = IO32ReadFldAlign(SHARPNESS_3D_11, SHARPNESS_CB_VCLIP);			   
			   break;
		   case E_TDTV_UI_3D_MODE_DA:		   //Dot Alternative
			   b3DHGain = IO32ReadFldAlign(SHARPNESS_3D_09, SHARPNESS_DA_HGAIN); 
			   b3DDGain = IO32ReadFldAlign(SHARPNESS_3D_09, SHARPNESS_DA_DGAIN);				  
			   b3DVGain = IO32ReadFldAlign(SHARPNESS_3D_09, SHARPNESS_DA_VGAIN);
			   b3DHClip = IO32ReadFldAlign(SHARPNESS_3D_12, SHARPNESS_DA_HCLIP); 
			   b3DDClip = IO32ReadFldAlign(SHARPNESS_3D_12, SHARPNESS_DA_DCLIP); 
			   b3DVClip = IO32ReadFldAlign(SHARPNESS_3D_12, SHARPNESS_DA_VCLIP);			   
			   break;
		   case E_TDTV_UI_3D_MODE_OFF:
		   default:
			   b3DHGain = 0x80; 
			   b3DDGain = 0x80; 
			   b3DVGain = 0x80; 
			   b3DHClip = 0x80; 
			   b3DDClip = 0x80; 
			   b3DVClip = 0x80;				   
			   break;
    }
	vIO32WriteFldAlign(SHARPNESS_3D_13, b3DHGain, SHARPNESS_3D_HGAIN);
	vIO32WriteFldAlign(SHARPNESS_3D_13, b3DDGain, SHARPNESS_3D_DGAIN);
	vIO32WriteFldAlign(SHARPNESS_3D_13, b3DVGain, SHARPNESS_3D_VGAIN);
	vIO32WriteFldAlign(SHARPNESS_3D_14, b3DHClip, SHARPNESS_3D_HCLIP);
	vIO32WriteFldAlign(SHARPNESS_3D_14, b3DDClip, SHARPNESS_3D_DCLIP);
	vIO32WriteFldAlign(SHARPNESS_3D_14, b3DVClip, SHARPNESS_3D_VCLIP);	
}
/*****************************************************************************************/
/**********************************   PBC MNR   ****************************************/
/*****************************************************************************************/
#ifndef CC_MT5398
#define WINDOW_SIZE 4320
#define BIN_NUM 9
#define POSITION_MAX 5
enum
{
	UP_HIST =0,
	DOWN_HIST,		
	LEFT_HIST,	
	RIGHT_HIST,
	CENTER_HIST,
	WAIT,	
	UPDATE_PBC,
	STOP,
};

UINT16 wHueHist_WinPos[POSITION_MAX][4]=
{
{0x0, 0x77F, 0x0, 0x8F},
{0x0, 0x77F, 0x3A7, 0x437},
{0x0, 0xFF, 0x0, 0x437},
{0x67F, 0x77F, 0x0, 0x437},
{0x2A0, 0x4DF, 0x12C, 0x30B}
};
UINT32 dwPBCSetting[BIN_NUM-1][6]=
{
{0x48, 0x3F8, 0x8, 0x374, 0x38C, 4},
{0x48, 0x3F8, 0x8, 0x394, 0x3A0, 4},
{0x48, 0x3F8, 0x8, 0x3A8, 0x3B0, 4},
{0x48, 0x3F8, 0x8, 0x3B8, 0x3C4, 4},
{0x48, 0x3F8, 0x8, 0x3CC, 0x3D4, 4},
{0x48, 0x3F8, 0x8, 0x3DC, 0x3E8, 4},
{0x48, 0x3F8, 0x8, 0x3F0, 0xC, 4},
{0x16, 0x2c, 0x8, 0x0, 0x3FF, 0},
};


static const UCHAR *WindowPosition[PE_ARG_NS] =
{
"Up ",
"Down ",
"Left ",
"Right ",
"Center ",
};

static UINT16 wHueHist[POSITION_MAX][BIN_NUM];

static void vDrvSetHueWinPos(UINT8 bWinPos)
{
	LOG(2, "Update Hist for %s Window\n", WindowPosition[bWinPos]);

	vIO32WriteFldMulti(SAT_HIST_CFG_MAIN_3,
	                  P_Fld(wHueHist_WinPos[bWinPos][0], COLOR_HIST_X_START) |
	                  P_Fld(wHueHist_WinPos[bWinPos][1], COLOR_HIST_X_END));
	vIO32WriteFldMulti(SAT_HIST_CFG_MAIN_4,
	                  P_Fld(wHueHist_WinPos[bWinPos][2], COLOR_HIST_Y_START) |
	                  P_Fld(wHueHist_WinPos[bWinPos][3], COLOR_HIST_Y_END));    
	

}

static UINT8 bDrvGetWeight(UINT8 bMin, UINT8 bMax, UINT8 bInput)
{
	UINT8 bAreaWeight=0;
	
	if(bInput>=bMin && bInput<=bMax && ((bMax-bMin)!=0))
	{
		bAreaWeight = ((bInput-bMin)*10)/(bMax-bMin);
	}
	else if(bInput<bMin)
	{
		bAreaWeight = 0;
	}
	else if(bInput>bMax)
	{
		bAreaWeight = 10;
	}
	else
	{
		LOG(2, "Error Happen !!! \n\n");
	}
	return bAreaWeight;
}
static void vDrvRemoveNoisebyPBC(void)
{
	UINT16 wTmp, wMaxBinCnt,wCenterBinCnt,wCenterDiff;
	UINT8 bIdxBin, bIdxPos, bMaxBinIdx, bBkgConf, bBkgArea, bAreaDiff, bAreaWeight, bAreaDiffWeight;
	UINT8 bTmpBottom, bTmpUp;

	LOG(2, "Do PBC PROC \n");
	//Find maximum position

	wMaxBinCnt=0x0;
	bMaxBinIdx=0x0;
	for(bIdxBin=0; bIdxBin<BIN_NUM; bIdxBin++)
	{
		wTmp=0x0;
		for(bIdxPos=0; bIdxPos<POSITION_MAX; bIdxPos++)
		{
			if(wTmp < wHueHist[bIdxPos][bIdxBin])
			{
				wTmp=wHueHist[bIdxPos][bIdxBin];					
			}
		}
		if(wMaxBinCnt<wTmp)
		{
			wMaxBinCnt = wTmp;
			bMaxBinIdx = bIdxBin;
		}
	}

	wCenterBinCnt= wHueHist[CENTER_HIST][bMaxBinIdx];
	wCenterDiff = ((wCenterBinCnt>wMaxBinCnt)?0:(wMaxBinCnt-wCenterBinCnt));
	LOG(2, "MaxBin : %d, MaxBinHist : %d\n", bMaxBinIdx, wMaxBinCnt);
	
	bAreaDiff = (wCenterDiff*100)/WINDOW_SIZE;
	bBkgArea = (wMaxBinCnt*100)/WINDOW_SIZE;

	bTmpBottom = IO32ReadFldAlign(TDSHP_PBCNR_00, AREA_BOTTOM);
	bTmpUp=		 IO32ReadFldAlign(TDSHP_PBCNR_00, AREA_UP);
	bAreaWeight = bDrvGetWeight(bTmpBottom, bTmpUp, bBkgArea);
	
	bTmpBottom = IO32ReadFldAlign(TDSHP_PBCNR_00, DIFF_BOTTOM);
	bTmpUp=		 IO32ReadFldAlign(TDSHP_PBCNR_00, DIFF_UP);		
	bAreaDiffWeight = bDrvGetWeight(bTmpBottom, bTmpUp, bAreaDiff);

	bBkgConf = bAreaDiffWeight*bAreaWeight;
	
	if(bBkgConf>IO32ReadFldAlign(TDSHP_PBCNR_01, CONF_TH) && bMaxBinIdx!=0)
	{
		vIO32WriteFldAlign(CDS_2B, dwPBCSetting[bMaxBinIdx-1][0], PBC2_RADIUS_C1);
		vIO32WriteFldAlign(CDS_2C, dwPBCSetting[bMaxBinIdx-1][1], PBC2_RADIUS_C2);
		vIO32WriteFldAlign(CDS_2A, dwPBCSetting[bMaxBinIdx-1][2], PBC2_RADIUS_R);			
		vIO32WriteFldAlign(CDS_2B, dwPBCSetting[bMaxBinIdx-1][3], PBC2_THETA_C1);
		vIO32WriteFldAlign(CDS_2C, dwPBCSetting[bMaxBinIdx-1][4], PBC2_THETA_C2);
		vIO32WriteFldAlign(CDS_2A, dwPBCSetting[bMaxBinIdx-1][5], PBC2_THETA_R);
		vIO32WriteFldAlign(CDS_2A, SV_ON, PBC2_EN);
	}
	else
	{
		vIO32WriteFldAlign(CDS_2A, SV_OFF, PBC2_EN);
	}

	LOG(2, "bAreaDiff : %d, bBkgArea : %d, bConfidence %d \n", bAreaDiff, bBkgArea, bBkgConf);
}
static void vDrvGetHueHistInfo(UINT8 bPos)
{
	UINT32 dwSum;

	wHueHist[bPos][0] = IO32ReadFldAlign(HUE_HIST_1_0_MAIN, HUE_HIST_0);
	wHueHist[bPos][1] = IO32ReadFldAlign(HUE_HIST_1_0_MAIN, HUE_HIST_1);
	wHueHist[bPos][2] = IO32ReadFldAlign(HUE_HIST_3_2_MAIN, HUE_HIST_2);
	wHueHist[bPos][3] = IO32ReadFldAlign(HUE_HIST_3_2_MAIN, HUE_HIST_3);
	wHueHist[bPos][4] = IO32ReadFldAlign(HUE_HIST_5_4_MAIN, HUE_HIST_4);
	wHueHist[bPos][5] = IO32ReadFldAlign(HUE_HIST_5_4_MAIN, HUE_HIST_5);
	wHueHist[bPos][6] = IO32ReadFldAlign(HUE_HIST_7_6_MAIN, HUE_HIST_6);
	wHueHist[bPos][7] = IO32ReadFldAlign(HUE_HIST_7_6_MAIN, HUE_HIST_7);
	dwSum = wHueHist[bPos][0]  
		  + wHueHist[bPos][1]
		  + wHueHist[bPos][2]
		  + wHueHist[bPos][3]
		  + wHueHist[bPos][4]
		  + wHueHist[bPos][5]
		  + wHueHist[bPos][6]
		  + wHueHist[bPos][7];
	wHueHist[bPos][8] = ((dwSum>WINDOW_SIZE)?0:(WINDOW_SIZE-dwSum)); 
	LOG(2, "Read Pos %d \n", (bPos));
	LOG(2, "Hist0 %d \n", wHueHist[bPos][0]);
	LOG(2, "Hist1 %d \n", wHueHist[bPos][1]);
	LOG(2, "Hist2 %d \n", wHueHist[bPos][2]);
	LOG(2, "Hist3 %d \n", wHueHist[bPos][3]);
	LOG(2, "Hist4 %d \n", wHueHist[bPos][4]);
	LOG(2, "Hist5 %d \n", wHueHist[bPos][5]);
	LOG(2, "Hist6 %d \n", wHueHist[bPos][6]);
	LOG(2, "Hist7 %d \n", wHueHist[bPos][7]);
	LOG(2, "Hist8 %d \n", wHueHist[bPos][8]);
	LOG(2, "\n");
}

void vDrvPBCMNRProc(void)
{
	static UINT8 bCnt=0;
	static UINT8 bHueHistWinPos = 0xFF;

	if(IO32ReadFldAlign(TDSHP_PBCNR_01, PBCNR_ONOFF))
	{
		if(bADLSceneChange || IO32ReadFldAlign(ADAPTIVE_SCE_REG, ADAPTIVE_FORCE_SHPSCENECHG))
		{
			bHueHistWinPos=UP_HIST;
			vIO32WriteFldAlign(ADAPTIVE_SCE_REG, SV_OFF, ADAPTIVE_FORCE_SHPSCENECHG);
		}
		//Need wait 4 loops for Histogram ready.
		if(bCnt%4==0)
		{
			switch(bHueHistWinPos)
			{
				case UP_HIST:
					vDrvSetHueWinPos(UP_HIST);				
					break;
				case DOWN_HIST:
					vDrvGetHueHistInfo(UP_HIST);				
					vDrvSetHueWinPos(DOWN_HIST);
					break;				
				case LEFT_HIST:
					vDrvGetHueHistInfo(DOWN_HIST);
					vDrvSetHueWinPos(LEFT_HIST);
					break;				
				case RIGHT_HIST:
					vDrvGetHueHistInfo(LEFT_HIST);
					vDrvSetHueWinPos(RIGHT_HIST);
					break;				
				case CENTER_HIST:	
					vDrvGetHueHistInfo(RIGHT_HIST);
					vDrvSetHueWinPos(CENTER_HIST);
					break;				
				case WAIT:
					vDrvGetHueHistInfo(CENTER_HIST);
					break;
				case UPDATE_PBC:
					vDrvRemoveNoisebyPBC();
					break;
				case STOP:
				default:
					break;
			}
			bHueHistWinPos =((bHueHistWinPos>=STOP)?(STOP):(bHueHistWinPos+1));
		}
		bCnt++;
		
	}
}
#endif
/*****************************************************************************************/
/**********************************   Sharpness Control   ****************************************/
/*****************************************************************************************/
void vDrvSharpProc(void)
{
    // SHARPNESS Driving
    vDrvPostSharpParamLimit();
    vDrvPBCProc();   

	//AC calculation
	vDrv3DSharpParamCalcu();
    vDrvHCalcuACCurve();
	vDrvVCalcuACCurve();
	vDrvDCalcuACCurve();
    vDrvLTICalcuACCurve();
    vDrvPBCCalcuACCurve();    
    // SHARPNESS ADAPTIVE
    vDrvAdapYLev(); 

#ifndef CC_MT5398
	vDrvPBCMNRProc();
#endif	
#ifdef CUST_SOEM_DRV
    fgSharpForce = SV_FALSE;
#endif
}

/**
 * @brief Post sharpness initial parameters
 * @param
 */
void vDrvPostSharpInit(void)
{	
	#ifndef CC_MT5398
	UINT8 i;
	for(i=0; i< TDS_BAND_NUM; i++)
	{
		bTDS_UpdateFlg[i]=0;
	}
	#endif	
    vDrvLoadRegTbl(REGTBL_TDSHARP);
    vDrvLoadRegTbl(REGTBL_PBC);    
    vDrvYlevSharpInit();
#ifndef CC_MT5880
	vIO32WriteFldAlign(TDPROC_MISC_00, SV_OFF, TDPROC_BYPASS_ALL);
#endif
	vIO32WriteFldMulti(TDSHP_PBCNR_00,
                  P_Fld(80, AREA_UP) |
                  P_Fld(50, AREA_BOTTOM) |
                  P_Fld(30, DIFF_UP) |
                  P_Fld(10, DIFF_BOTTOM));   
	vIO32WriteFldAlign(TDSHP_PBCNR_01, 32, CONF_TH);
	vIO32WriteFldAlign(TDSHP_PBCNR_01, SV_OFF, PBCNR_ONOFF);
    #ifdef CUST_SOEM_DRV
    if (PANEL_GetPanelWidth() <= 1440)  //Masoem, WXGA use by frequency
    {
        vIO32WriteFldAlign(SHARP_00, 1, SHP_COMBINE_MODE);
    }
    //Init Adaptive Sharpness for RF weak signal
    vIO32WriteFldAlign(ADAP_SHARPNESS_00, 1, ADAP_SHARP_ENABLE);
    vIO32WriteFldAlign(ADAP_SHARPNESS_00, 40, ADAP_SHARP_THD_DN);
    vIO32WriteFldAlign(ADAP_SHARPNESS_00, 55, ADAP_SHARP_THD_UP);
    vIO32WriteFldAlign(ADAP_SHARPNESS_00, 0x05, ADAP_SHARP_GAIN_DN);
    #endif
}

#ifdef CUST_SOEM_DRV
void vDrvSharpness_Resume(void)
{
    fgSharpForce = SV_TRUE;
}
#endif
/*****************************************************************************************/
/**********************************   Pattern Gen   ****************************************/
/*****************************************************************************************/
void vDrvMainSharpPatGen(UINT8 bOnOff)
{
	if (bOnOff == SV_ON)
	{
        vIO32WriteFldAlign(TDPROC_MISC_00, 1, TDPROC_PATGEN_DATA_EN);
		vIO32WriteFldAlign(TDPROC_MISC_00, 0, TDPROC_PATGEN_MODE);
		vIO32WriteFldAlign(TDPROC_MISC_00, 1, TDPROC_PATGEN_YSEL);
		vIO32WriteFldAlign(TDPROC_MISC_00, 1, TDPROC_PATGEN_USEL);
		vIO32WriteFldAlign(TDPROC_MISC_00, 1, TDPROC_PATGEN_VSEL);
	}
	else
	{
		vIO32WriteFldAlign(TDPROC_MISC_00, 0, TDPROC_PATGEN_DATA_EN);
		vIO32WriteFldAlign(TDPROC_MISC_00, 0, TDPROC_PATGEN_YSEL);
		vIO32WriteFldAlign(TDPROC_MISC_00, 0, TDPROC_PATGEN_USEL);
		vIO32WriteFldAlign(TDPROC_MISC_00, 0, TDPROC_PATGEN_VSEL);
	}
}


/*****************************************************************************************/
/**********************************   DEMO MODE   ****************************************/
/*****************************************************************************************/
void vDrvSetPostSharpDemoRange(Region rRegion)
{
    //Define demo region
    vIO32WriteFldAlign(SP_MASK_00, rRegion.wHStart, CH1_H_DMWINDOW_START);
    vIO32WriteFldAlign(SP_MASK_00, rRegion.wHEnd, CH1_H_DMWINDOW_END);// 2DSharp : start < active < End                                                                                                  
    vIO32WriteFldAlign(SP_MASK_01, rRegion.wVStart, CH1_V_DMWINDOW_START); // 2D Sharp Demo mode Voffset
    vIO32WriteFldAlign(SP_MASK_01, rRegion.wVEnd, CH1_V_DMWINDOW_END);// 2D Sharp Demo mode Voffset
}

void vDrv2DSharpDemo(UINT8 bOnOff, Region rRegion)
{
	#ifdef CC_FLIP_MIRROR_SUPPORT
	if((u4QueryFlipModule(VDP_1) & 0x7)&&(u1GetFlipMirrorConfig()& 0x2))
	{rRegion.u1OnOff = !rRegion.u1OnOff;}
	#endif
	vIO32WriteFldAlign(TDPROC_MISC_00, (bOnOff==SV_ON ? ((rRegion.u1OnOff==SV_ON)? 1:0) : 0), TDS_DEMO_SWAP);
    vIO32WriteFldAlign(TDPROC_MISC_00, (bOnOff==SV_ON?1:0), TDS_DEMO_ENA);  
}

void vDrvECTIDemo(UINT8 bOnOff, Region rRegion)
{
	#ifdef CC_FLIP_MIRROR_SUPPORT
	if((u4QueryFlipModule(VDP_1) & 0x7)&&(u1GetFlipMirrorConfig()& 0x2))
	{rRegion.u1OnOff = !rRegion.u1OnOff;}
	#endif
    //vIO32WriteFldAlign(TDPROC_MISC_00, ((rRegion.u1OnOff==SV_ON)?0:1), ECTI_DEMO_SWAP);
    vIO32WriteFldAlign(TDPROC_MISC_00, (bOnOff==SV_ON ? ((rRegion.u1OnOff==SV_ON)? 1:0) : 0), ECTI_DEMO_SWAP);
    vIO32WriteFldAlign(TDPROC_MISC_00, (bOnOff==SV_ON?1:0), ECTI_DEMO_ENA);
}

