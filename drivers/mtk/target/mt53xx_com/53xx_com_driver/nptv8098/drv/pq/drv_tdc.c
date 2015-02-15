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
 * $RCSfile: drv_tdc.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

#define _DRV_TDC_C_

#include "x_assert.h"
#include "x_os.h"

#include "general.h"
#include "hw_vdoin.h"
#include "nptv_debug.h"

#include "hw_tdc.h"
#include "hw_tvd.h"
#include "hw_nr.h"

#include "hw_sw.h"

#include "drv_tdc.h"
#include "drv_tvd.h"

#include "source_select.h"
#include "vdo_misc.h"
#include "video_def.h"

#include "hw_di_int.h"
#include "hw_di.h"
#include "hw_ycproc.h"


#ifndef  CC_UP8032_ATV
#include "fbm_drvif.h"
EXTERN UINT32 TDC_DRAM_BASE;
#else
//TDC_DRAM_BASE Should be defined by memory map
//#ifndef TDC_DRAM_BASE
//#define TDC_DRAM_BASE 0x1000000
//#endif
extern UINT32 TDC_DRAM_BASE;
#endif

#ifdef CC_MT5398
#include "hw_mjc.h"
#endif

#define SUPPORT_PQ_FINETUNE 0

#if SUPPORT_PQ_FINETUNE 
UINT8 _bK2ValueByCSS;
UINT8 _bK2ValueByWeakC;
UINT8 _bK4ChromaThByCans;
UINT8 _bPatchInPattern107;
UINT8 u1fgPatternHammock;
UINT8 _fgMovingcolorCharacter;
UINT8 _fgCornPattern;
UINT8 _fgMovingCans;

EXTERN UINT16 u2MajorCnt0;
EXTERN UINT8  u1MajorMvX0;
EXTERN UINT8  u1MajorMvY0;
EXTERN UINT16 u2TotalCnt;
EXTERN UINT8 u1IF_MAX_MOTION_C_Value;


EXTERN void vDrvDISetChromaMaxMoLevel(UINT8 u1MaxMoC);
#endif

UINT8 _bTDCEnable;
UINT8 _bTDCConf;

//PRIVATE UINT8 _fgTDCDemo;
PRIVATE UINT16 _wTDCStart;
PRIVATE UINT16 _wTDCLen;
static UINT32 preCOMB_MODE = 0xFF;

#if ADAPTIVE_2D_COMB_SHARP
UINT8 _bAdaptiveSharpGain;
#endif // #if ADAPTIVE_2D_COMB_SHARP

REGTBL_T const CODE REGTBL_COMB_COMMON[] = {
    {COMB2D_09, 0xA0A03530, 0xFFFF3FF0},
    {COMB2D_0C, 0x82F850AA, 0xFFFFFFFF},
    //to do : NON-standard mode tuning
    {C_COMB_29, 0x00000000, 0xFE000000},
    //{C_COMB_29, 0x08000000, 0xFF000000},
    {C_COMB_3A, 0x10482810, 0xFFFFFFFF},
    {C_COMB_3B, 0x00004010, 0xFFFFFFFF},
    {C_COMB_40, 0x00000000, 0xFFF00000},
    {C_COMB_42, 0x00000000, 0xFFFFFFFF},
    {REGTBL_END, 0x00000000, 0x00000000}
};

REGTBL_T const CODE REGTBL_COMB_NTSC_AV[] = {
    {COMB2D_00, 0x22110608, 0xFFFFFFFF},
    {COMB2D_01, 0x80505F06, 0xF9FFFF06},
    {COMB2D_02, 0x10002000, 0xFF00FF03},
    {COMB2D_03, 0x1E00408A, 0xFF01FFFF},
    {COMB2D_04, 0x44300608, 0xF7F0FFFF},
    {COMB2D_05, 0x6C000000, 0xFC000009},
    {COMB2D_06, 0x00001E67, 0x00FFFFFF},
    {COMB2D_07, 0x03440000, 0xFFFF00FF},
    {COMB2D_08, 0x01234444, 0xFFFFFFFF},
    {COMB2D_09, 0x45678888, 0xFFFFFFFF},
    {COMB2D_0A, 0xF0100A89, 0xFFDFFF9F},
    {COMB2D_0B, 0x00000003, 0x8000007F},
    {COMB2D_0C, 0x10092808, 0xFFFFFFFF},
    {COMB2D_0D, 0x80081010, 0x8FFFFFFF},
    {COMB2D_0E, 0x82100A22, 0xFFFFFFFF},
    {COMB2D_0F, 0x01233333, 0xFFFFFFFF},
    {C_COMB_10, 0x100AF850, 0xFFFFFFFF},
    {C_COMB_11, 0x0F000000, 0xFFFFFFFF},
    {C_COMB_12, 0x00061410, 0x48FFFFFF},
    {C_COMB_13, 0x64000114, 0xFFFFFFFF},
    {C_COMB_14, 0x0A074A36, 0xFFFFFFFF},
    {C_COMB_15, 0x0A074A36, 0xFFFFFFFF},
    {C_COMB_16, 0x007F9C00, 0xFFFFFFFF},
    {C_COMB_17, 0x06000000, 0x3FFFFFFF},
    {C_COMB_18, 0x0C0480E8, 0x0E3EDEFB},
    {C_COMB_19, 0x00110333, 0xFFFFFFFF},
    {C_COMB_1A, 0x1488C084, 0xFFFFFFFF},
    {C_COMB_1B, 0xE8414444, 0xFFFFFFFF},
    {C_COMB_1C, 0x11600823, 0xFFFFFE23},
    {C_COMB_1D, 0x04202411, 0xFFFFFFFF},
    {C_COMB_1E, 0x11111111, 0xFFFFFFFF},
    {C_COMB_1F, 0x28CC82A5, 0xFFFFFFBF},
    {C_COMB_20, 0x05404300, 0xFFFFFFFF},
    {C_COMB_21, 0x38081808, 0xFFFFFFFF},
    {C_COMB_22, 0x6096AA50, 0xFFFFFFFF},
    {C_COMB_23, 0x78806810, 0xFFFFFFFF},
    {C_COMB_24, 0x0A0B4145, 0xFFFFFFFF},
    {C_COMB_25, 0x0120FF05, 0xFFFFFFFF},
    {C_COMB_26, 0x1C00640A, 0x7FFFFFFF},
    {C_COMB_27, 0x38006000, 0xFFFFFFFF},
    {C_COMB_28, 0x0010101F, 0x01FFFFFF},
    {C_COMB_29, 0x00CDF400, 0xFE000000},
    {C_COMB_2A, 0x8A045AF4, 0x7FFFFFFF},
    {C_COMB_2B, 0x25030203, 0xFFFFFFFF},    //Enable Ckill
    {C_COMB_2C, 0xC800640F, 0xFFE2FFFF},
    {C_COMB_2D, 0x2B030038, 0xFFFFFFFF},
    {C_COMB_2E, 0x70231E05, 0xFFFFFFFF},
    {C_COMB_2F, 0x00000000, 0x9FFFFFFF},
    {COMB2D_31, 0x204022CA, 0x7FFFFFFF},
    {COMB2D_32, 0x80000820, 0xFC7FFFFF},
    {COMB2D_33, 0x11110415, 0xFFFFFFF7},
    {COMB2D_34, 0x11111456, 0xFFFFFFFF},
    {COMB2D_35, 0x02345678, 0xFFFFFFFF},
    {COMB2D_36, 0x02345678, 0xFFFFFFFF},
    {COMB2D_37, 0x01234567, 0xFFFFFFFF},
    {COMB2D_38, 0x00081414, 0x0FFFFFFF},
    {COMB2D_39, 0x00020A34, 0x7F7F7F7F},
    {C_COMB_3A, 0x00000000, 0x0000000F},
    {C_COMB_3B, 0x00002480, 0xB3FFFFFF},
    {C_COMB_3C, 0x84080800, 0xFF3F3F03},
    {C_COMB_3D, 0x00003241, 0xFFFFFFE7},
    {C_COMB_3E, 0x3F3F3F3F, 0x3F3F3F3F},
    {C_COMB_3F, 0x3228321E, 0xFFFFFFFF},
    {C_COMB_40, 0x00015C1E, 0xFFFFFFFF},
    {C_COMB_41, 0x00800A10, 0xFFFFFFFF},
    {C_COMB_42, 0x00000000, 0xFFFFFFFF},
    {C_COMB_43, 0x3E40A028, 0xFFDFFFFF},
    {C_COMB_44, 0x00800000, 0xBFFFFFFF},
    {C_COMB_45, 0x45198064, 0xFFFFFFFF},
    {C_COMB_46, 0x96001F43, 0xFFFFFFFF},
    {C_COMB_47, 0x1E0F1904, 0xFFFFFFFF},
    {C_COMB_48, 0x83001402, 0xFF00FF7F},
    {C_COMB_49, 0x60145030, 0xFFFFFFFF},
    {C_COMB_4A, 0xB9100020, 0xFFFFFFFF},
    {C_COMB_4B, 0x02345678, 0xFFFFFFFF},
    {C_COMB_4C, 0x34406110, 0x777BFFFF},
    {C_COMB_4D, 0x80007010, 0xFF00F3FF},
    {C_COMB_4E, 0x00103111, 0xFFFFFFFF},
    {C_COMB_4F, 0x10009111, 0xFFFDFFFF},
    {REGTBL_END,   0x00000000, 0x00000000}

};
REGTBL_T const CODE REGTBL_COMB_NTSC443[] = { 
    {COMB2D_00, 0x22110A08, 0xFFFFFFFF},
    {COMB2D_01, 0x80505F06, 0xF9FFFF06},
    {COMB2D_02, 0x11002000, 0xFF00FF03},
    {COMB2D_03, 0x3E00408A, 0xFF01FFFF},
    {COMB2D_04, 0x40300808, 0xF7F0FFFF},
    {COMB2D_05, 0x6C000000, 0xFC000009},
    {COMB2D_06, 0x00001E77, 0x00FFFFFF},
    {COMB2D_07, 0x03440000, 0xFFFF00FF},
    {COMB2D_08, 0x01234444, 0xFFFFFFFF},
    {COMB2D_09, 0x45678888, 0xFFFFFFFF},
    {COMB2D_0A, 0xF0900088, 0xFFDFFF9F},
    {COMB2D_0B, 0x00000003, 0x8000007F},
    {COMB2D_0C, 0x10092808, 0xFFFFFFFF},
    {COMB2D_0D, 0x00081010, 0x8FFFFFFF},
    {COMB2D_0E, 0x84100A22, 0xFFFFFFFF},
    {COMB2D_0F, 0x01234567, 0xFFFFFFFF},
    {C_COMB_10, 0x100AF850, 0xFFFFFFFF},
    {C_COMB_11, 0x08000000, 0xFFFFFFFF},
    {C_COMB_12, 0x00061410, 0x48FFFFFF},
    {C_COMB_13, 0x64000114, 0xFFFFFFFF},
    {C_COMB_14, 0x0A09052E, 0xFFFFFFFF},
    {C_COMB_15, 0x0A09052E, 0xFFFFFFFF},
    {C_COMB_16, 0x007F9C00, 0xFFFFFFFF},
    {C_COMB_17, 0x00000000, 0x3FFFFFFF},
    {C_COMB_18, 0x000800E8, 0x0E3EDEFB},
    {C_COMB_19, 0x00110333, 0xFFFFFFFF},
    {C_COMB_1A, 0x14888084, 0xFFFFFFFF},
    {C_COMB_1B, 0xE8414444, 0xFFFFFFFF},
    {C_COMB_1C, 0x11600823, 0xFFFFFE23},
    {C_COMB_1D, 0x04202411, 0xFFFFFFFF},
    {C_COMB_1E, 0x11111111, 0xFFFFFFFF},
    {C_COMB_1F, 0x280C02A5, 0xFFFFFFBF},
    {C_COMB_20, 0x00404300, 0xFFFFFFFF},
    {C_COMB_21, 0x38081808, 0xFFFFFFFF},
    {C_COMB_22, 0x6096AA50, 0xFFFFFFFF},
    {C_COMB_23, 0x78804810, 0xFFFFFFFF},
    {C_COMB_24, 0x0A013800, 0xFFFFFFFF},
    {C_COMB_25, 0x0120FF05, 0xFFFFFFFF},
    {C_COMB_26, 0x1C00640A, 0x7FFFFFFF},
    {C_COMB_27, 0x38006000, 0xFFFFFFFF},
    {C_COMB_28, 0x0010101F, 0x01FFFFFF},
    {C_COMB_29, 0x80063300, 0xFE000000},
    {C_COMB_2A, 0x0A000000, 0x7FFFFFFF},
    {C_COMB_2B, 0x00000213, 0xFFFFFFFF},
    {C_COMB_2C, 0xC800640F, 0xFFE2FFFF},
    {C_COMB_2D, 0x2F030438, 0xFFFFFFFF},
    {C_COMB_2E, 0x70231E00, 0xFFFFFFFF},
    {C_COMB_2F, 0x00000000, 0x9FFFFFFF},
    {COMB2D_31, 0x20B0000A, 0x7FFFFFFF},
    {COMB2D_32, 0x80000808, 0xFC7FFFFF},
    {COMB2D_33, 0x11110435, 0xFFFFFFF7},
    {COMB2D_34, 0x12345678, 0xFFFFFFFF},
    {COMB2D_35, 0x02345678, 0xFFFFFFFF},
    {COMB2D_36, 0x02345678, 0xFFFFFFFF},
    {COMB2D_37, 0x01234567, 0xFFFFFFFF},
    {COMB2D_38, 0x00080014, 0x0FFFFFFF},
    {COMB2D_39, 0x00020A34, 0x7F7F7F7F},
    {C_COMB_3A, 0x00000000, 0x0000000F},
    {C_COMB_3B, 0x00002480, 0xB3FFFFFF},
    {C_COMB_3C, 0x84081D01, 0xFF3F3F03},
    {C_COMB_3D, 0x00003242, 0xFFFFFFE7},
    {C_COMB_3E, 0x0820080C, 0x3F3F3F3F},
    {C_COMB_3F, 0x3238321E, 0xFFFFFFFF},
    {C_COMB_40, 0x00015C1E, 0xFFFFFFFF},
    {C_COMB_41, 0x00810A10, 0xFFFFFFFF},
    {C_COMB_42, 0x00000000, 0xFFFFFFFF},
    {C_COMB_43, 0x3E40C828, 0xFFDFFFFF},
    {C_COMB_44, 0x00800000, 0xBFFFFFFF},
    {C_COMB_45, 0x45198064, 0xFFFFFFFF},
    {C_COMB_46, 0x96071F43, 0xFFFFFFFF},
    {C_COMB_47, 0x1E0F1904, 0xFFFFFFFF},
    {C_COMB_48, 0x83000702, 0xFF00FF7F},
    {C_COMB_49, 0x60145030, 0xFFFFFFFF},
    {C_COMB_4A, 0xB9100010, 0xFFFFFFFF},
    {C_COMB_4B, 0x02345678, 0xFFFFFFFF},
    {C_COMB_4C, 0x34406110, 0x777BFFFF},
    {C_COMB_4D, 0x80007010, 0xFF00F3FF},
    {C_COMB_4E, 0x00103111, 0xFFFFFFFF},
    {C_COMB_4F, 0x10089111, 0xFFFDFFFF},
    {REGTBL_END, 0x00000000, 0x00000000},
    };
REGTBL_T const CODE REGTBL_COMB_PAL_60[] = {
    {COMB2D_00, 0x55000018, 0xFFFFFFFF},
    {COMB2D_01, 0x80969606, 0xF9FFFF06},
    {COMB2D_02, 0x10002000, 0xFF00FF03},
    {COMB2D_03, 0x0C00208A, 0xFF01FFFF},
    {COMB2D_04, 0x50500808, 0xF7F0FFFF},
    {COMB2D_05, 0x5C000000, 0xFC000009},
    {COMB2D_06, 0x00FF1E2F, 0x00FFFFFF},
    {COMB2D_07, 0x1444000A, 0xFFFF00FF},
    {COMB2D_08, 0x01234444, 0xFFFFFFFF},
    {COMB2D_09, 0x45678888, 0xFFFFFFFF},
    {COMB2D_0A, 0xF0480A99, 0xFFDFFF9F},
    {COMB2D_0B, 0x00000003, 0x8000007F},
    {COMB2D_0C, 0x10806C10, 0xFFFFFFFF},
    {COMB2D_0D, 0x80080C0A, 0x8FFFFFFF},
    {COMB2D_0E, 0x84280A08, 0xFFFFFFFF},
    {COMB2D_0F, 0x00123456, 0xFFFFFFFF},
    {C_COMB_10, 0x100AF852, 0xFFFFFFFF},
    {C_COMB_11, 0x0F400000, 0xFFFFFFFF},
    {C_COMB_12, 0x00053700, 0x48FFFFFF},
    {C_COMB_13, 0xC8020444, 0xFFFFFFFF},
    {C_COMB_14, 0x0A09073F, 0xFFFFFFFF},
    {C_COMB_15, 0x0A09073F, 0xFFFFFFFF},
    {C_COMB_16, 0xC00F9C10, 0xFFFFFFFF},
    {C_COMB_17, 0x06400000, 0x3FFFFFFF},
    {C_COMB_18, 0x0C0E8088, 0x0E3EDEFB},
    {C_COMB_19, 0x0C010444, 0xFFFFFFFF},
    {C_COMB_1A, 0x0F860078, 0xFFFFFFFF},
    {C_COMB_1B, 0x48044444, 0xFFFFFFFF},
    {C_COMB_1C, 0x1020D820, 0xFFFFFE23},
    {C_COMB_1D, 0x20202831, 0xFFFFFFFF},
    {C_COMB_1E, 0x11111111, 0xFFFFFFFF},
    {C_COMB_1F, 0x288D31AE, 0xFFFFFFBF},    //disble C3D_FIR
    {C_COMB_20, 0x00444300, 0xFFFFFFFF},
    {C_COMB_21, 0x4A101030, 0xFFFFFFFF},
    {C_COMB_22, 0x80969614, 0xFFFFFFFF},
    {C_COMB_23, 0x7008301E, 0xFFFFFFFF},
    {C_COMB_24, 0x0A08B0E2, 0xFFFFFFFF},
    {C_COMB_25, 0x01185805, 0xFFFFFFFF},
    {C_COMB_26, 0x1C00640A, 0x7FFFFFFF},
    {C_COMB_27, 0x10E08F28, 0xFFFFFFFF},
    {C_COMB_28, 0x0010101F, 0x01FFFFFF},
    {C_COMB_29, 0x08063300, 0xFE000000},
    {C_COMB_2A, 0x0A000000, 0x7FFFFFFF},
    {C_COMB_2B, 0x00000213, 0xFFFFFFFF},
    {C_COMB_2C, 0x1420640A, 0xFFE2FFFF},
    {C_COMB_2D, 0x31034000, 0xFFFFFFFF},
    {C_COMB_2E, 0x70231E05, 0xFFFFFFFF},
    {C_COMB_2F, 0x03980000, 0x9FFFFFFF},
    {COMB2D_31, 0x0A14000A, 0x7FFFFFFF},
    {COMB2D_32, 0x8C0A1020, 0xFC7FFFFF},
    {COMB2D_33, 0x03110416, 0xFFFFFFF7},
    {COMB2D_34, 0x01245678, 0xFFFFFFFF},
    {COMB2D_35, 0x02345678, 0xFFFFFFFF},
    {COMB2D_36, 0x02345678, 0xFFFFFFFF},
    {COMB2D_37, 0x01234567, 0xFFFFFFFF},
    {COMB2D_38, 0x00080014, 0x0FFFFFFF},
    {COMB2D_39, 0x00020A34, 0x7F7F7F7F},
    {C_COMB_3A, 0x00000000, 0x0000000F},
    {C_COMB_3B, 0x00002480, 0xB3FFFFFF},
    {C_COMB_3C, 0x84081D01, 0xFF3F3F03},
    {C_COMB_3D, 0x00003282, 0xFFFFFFE7},
    {C_COMB_3E, 0x0820080C, 0x3F3F3F3F},
    {C_COMB_3F, 0x0040321E, 0xFFFFFFFF},
    {C_COMB_40, 0x00007814, 0xFFFFFFFF},
    {C_COMB_41, 0x30000A20, 0xFFFFFFFF},
    {C_COMB_42, 0x00000000, 0xFFFFFFFF},
    {C_COMB_43, 0x000FC014, 0xFFDFFFFF},
    {C_COMB_44, 0x08887040, 0xBFFFFFFF},
    {C_COMB_45, 0x3E1A8064, 0xFFFFFFFF},
    {C_COMB_46, 0x96001F53, 0xFFFFFFFF},
    {C_COMB_47, 0x54280504, 0xFFFFFFFF},
    {C_COMB_48, 0x03001405, 0xFF00FF7F},
    {C_COMB_49, 0x60145353, 0xFFFFFFFF},
    {C_COMB_4A, 0x500C0010, 0xFFFFFFFF},
    {C_COMB_4B, 0x02345678, 0xFFFFFFFF},
    {C_COMB_4C, 0x0000608C, 0x777BFFFF},
    {C_COMB_4D, 0x99007110, 0xFF00F3FF},
    {C_COMB_4E, 0x60123110, 0xFFFFFFFF},
    {C_COMB_4F, 0x10654444, 0xFFFDFFFF},
    {REGTBL_END,   0x00000000, 0x00000000},
        };

REGTBL_T const CODE REGTBL_COMB_PAL_AV[] = {
    {COMB2D_00, 0x55000018, 0xFFFFFFFF},
    {COMB2D_01, 0x80969606, 0xF9FFFF06},
    {COMB2D_02, 0x10002000, 0xFF00FF03},
    {COMB2D_03, 0x0C00208A, 0xFF01FFFF},
    {COMB2D_04, 0x54500808, 0xF7F0FFFF},
    {COMB2D_05, 0x5C000000, 0xFC000009},
    {COMB2D_06, 0x00FF1E2F, 0x00FFFFFF},
    {COMB2D_07, 0x1444000A, 0xFFFF00FF},
    {COMB2D_08, 0x01234444, 0xFFFFFFFF},
    {COMB2D_09, 0x45678888, 0xFFFFFFFF},
    {COMB2D_0A, 0x70480A99, 0xFFDFFF9F},//[31]->0 for Move White Ball.
    {COMB2D_0B, 0x00000003, 0x8000007F},
    {COMB2D_0C, 0x10806C10, 0xFFFFFFFF},
    {COMB2D_0D, 0x80080C0A, 0x8FFFFFFF},
    {COMB2D_0E, 0x84280A08, 0xFFFFFFFF},
    {COMB2D_0F, 0x00123456, 0xFFFFFFFF},
    {C_COMB_10, 0x100AF852, 0xFFFFFFFF},
    {C_COMB_11, 0x0F400000, 0xFFFFFFFF},
    {C_COMB_12, 0x00053700, 0x48FFFFFF},
    {C_COMB_13, 0x82020444, 0xFFFFFFFF},
    {C_COMB_14, 0x0A0AD303, 0xFFFFFFFF},
    {C_COMB_15, 0x0A0AD303, 0xFFFFFFFF},
    {C_COMB_16, 0xC00FBC10, 0xFFFFFFFF},
    {C_COMB_17, 0x06400000, 0x3FFFFFFF},
    {C_COMB_18, 0x0C0E8088, 0x0E3EDEFB},
    {C_COMB_19, 0x0C010444, 0xFFFFFFFF},
    {C_COMB_1A, 0x0F86008A, 0xFFFFFFFF},
    {C_COMB_1B, 0x48044444, 0xFFFFFFFF},
    {C_COMB_1C, 0x1020D820, 0xFFFFFE23},
    {C_COMB_1D, 0x20202831, 0xFFFFFFFF},
    {C_COMB_1E, 0x11111111, 0xFFFFFFFF},
    {C_COMB_1F, 0x288DF5AE, 0xFFFFFFBF},    //diable C3D_FIR
    {C_COMB_20, 0x1A444300, 0xFFFFFFFF},
    {C_COMB_21, 0x4A101030, 0xFFFFFFFF},
    {C_COMB_22, 0x80969614, 0xFFFFFFFF},
    {C_COMB_23, 0x7008301E, 0xFFFFFFFF},
    {C_COMB_24, 0x0A08B0E2, 0xFFFFFFFF},
    {C_COMB_25, 0x01185805, 0xFFFFFFFF},
    {C_COMB_26, 0x1C00640A, 0x7FFFFFFF},
    {C_COMB_27, 0x10E08F28, 0xFFFFFFFF},
    {C_COMB_28, 0x0010101F, 0x01FFFFFF},
    {C_COMB_29, 0x08CDF400, 0xFE000000},
    {C_COMB_2A, 0x8A0493B3, 0x7FFFFFFF},
    {C_COMB_2B, 0x27360203, 0xFFFFFFFF},    //Enable Ckill(E-07/R-12 channel)
    {C_COMB_2C, 0x1420640A, 0xFFE2FFFF},
    {C_COMB_2D, 0x31314000, 0xFFFFFFFF},
    {C_COMB_2E, 0x70101E05, 0xFFFFFFFF},
    {C_COMB_2F, 0x03980000, 0x9FFFFFFF},
    {COMB2D_31, 0x0A14226A, 0x7FFFFFFF},
    {COMB2D_32, 0x8C031020, 0xFC7FFFFF},
    {COMB2D_33, 0x03110416, 0xFFFFFFF7},
    {COMB2D_34, 0x01245678, 0xFFFFFFFF},
    {COMB2D_35, 0x02345678, 0xFFFFFFFF},
    {COMB2D_36, 0x02345678, 0xFFFFFFFF},
    {COMB2D_37, 0x01234567, 0xFFFFFFFF},
    {COMB2D_38, 0x00081414, 0x0FFFFFFF},
    {COMB2D_39, 0x00020A34, 0x7F7F7F7F},
    {C_COMB_3A, 0x00000000, 0x0000000F},
    {C_COMB_3B, 0x00002480, 0xB3FFFFFF},
    {C_COMB_3C, 0x84081D00, 0xFF3F3F03},
    {C_COMB_3D, 0x00003282, 0xFFFFFFE7},
    {C_COMB_3E, 0x0820080C, 0x3F3F3F3F},
    {C_COMB_3F, 0x0040321E, 0xFFFFFFFF},
    {C_COMB_40, 0x00007814, 0xFFFFFFFF},
    {C_COMB_41, 0x30000A20, 0xFFFFFFFF},
    {C_COMB_42, 0x00000000, 0xFFFFFFFF},
    {C_COMB_43, 0x000F8814, 0xFFDFFFFF},
    {C_COMB_44, 0x08887040, 0xBFFFFFFF},
    {C_COMB_45, 0x3E1A8064, 0xFFFFFFFF},
    {C_COMB_46, 0x96001F53, 0xFFFFFFFF},
    {C_COMB_47, 0x54280000, 0xFFFFFFFF},//[15:0]->0 for G/M MB
    {C_COMB_48, 0x03001405, 0xFF00FF7F},
    {C_COMB_49, 0x601453D3, 0xFFFFFFFF},//Turn off Flash Flag to avoid PCB noise false alarm.
    {C_COMB_4A, 0x500C10A0, 0xFFFFFFFF},
    {C_COMB_4B, 0x02345678, 0xFFFFFFFF},
    {C_COMB_4C, 0x0000608C, 0x777BFFFF},
    {C_COMB_4D, 0x99007110, 0xFF00F3FF},
    {C_COMB_4E, 0x60123110, 0xFFFFFFFF},
    {C_COMB_4F, 0x10654444, 0xFFFDFFFF},
    {REGTBL_END,   0x00000000, 0x00000000}

};

REGTBL_T const CODE REGTBL_COMB_PAL_M_AV[] = {
    {COMB2D_00, 0x55000018, 0xFFFFFFFF},
    {COMB2D_01, 0x80969606, 0xF9FFFF06},
    {COMB2D_02, 0x10002000, 0xFF00FF03},
    {COMB2D_03, 0x0C00208A, 0xFF01FFFF},
    {COMB2D_04, 0x54500808, 0xF7F0FFFF},
    {COMB2D_05, 0x5C000000, 0xFC000009},
    {COMB2D_06, 0x00FF1E2F, 0x00FFFFFF},
    {COMB2D_07, 0x1444000A, 0xFFFF00FF},
    {COMB2D_08, 0x01234444, 0xFFFFFFFF},
    {COMB2D_09, 0x45678888, 0xFFFFFFFF},
    {COMB2D_0A, 0xF008C198, 0xFFDFFF9F},
    {COMB2D_0B, 0x00000003, 0x8000007F},
    {COMB2D_0C, 0x10806C10, 0xFFFFFFFF},
    {COMB2D_0D, 0x80080C0A, 0x8FFFFFFF},
    {COMB2D_0E, 0x84280A08, 0xFFFFFFFF},
    {COMB2D_0F, 0x00123456, 0xFFFFFFFF},
    {C_COMB_10, 0x100AF850, 0xFFFFFFFF},
    {C_COMB_11, 0x10400000, 0xFFFFFFFF},
    {C_COMB_12, 0x00053700, 0x48FFFFFF},
    {C_COMB_13, 0xC8020444, 0xFFFFFFFF},
    {C_COMB_14, 0x0A074829, 0xFFFFFFFF},
    {C_COMB_15, 0x20074829, 0xFFFFFFFF},
    {C_COMB_16, 0xC00F9C10, 0xFFFFFFFF},
    {C_COMB_17, 0x00400020, 0x3FFFFFFF},
    {C_COMB_18, 0x000A8088, 0x0E3EDEFB},
    {C_COMB_19, 0x0C010444, 0xFFFFFFFF},
    {C_COMB_1A, 0x0F860078, 0xFFFFFFFF},
    {C_COMB_1B, 0x48044444, 0xFFFFFFFF},
    {C_COMB_1C, 0x1020D820, 0xFFFFFE23},
    {C_COMB_1D, 0x20202831, 0xFFFFFFFF},
    {C_COMB_1E, 0x11111111, 0xFFFFFFFF},
    {C_COMB_1F, 0x280D319E, 0xFFFFFFBF},
    {C_COMB_20, 0x00444300, 0xFFFFFFFF},
    {C_COMB_21, 0x4A101030, 0xFFFFFFFF},
    {C_COMB_22, 0x80969614, 0xFFFFFFFF},
    {C_COMB_23, 0x7008301E, 0xFFFFFFFF},
    {C_COMB_24, 0x0A018000, 0xFFFFFFFF},
    {C_COMB_25, 0x01185805, 0xFFFFFFFF},
    {C_COMB_26, 0x1C00640A, 0x7FFFFFFF},
    {C_COMB_27, 0x10E08F28, 0xFFFFFFFF},
    {C_COMB_28, 0x0010101F, 0x01FFFFFF},
    {C_COMB_29, 0x88063300, 0xFE000000},
    {C_COMB_2A, 0x8A0482F0, 0x7FFFFFFF},
    {C_COMB_2B, 0x23000203, 0xFFFFFFFF},
    {C_COMB_2C, 0x1420640A, 0xFFE2FFFF},
    {C_COMB_2D, 0x2B004438, 0xFFFFFFFF},
    {C_COMB_2E, 0x70231E2A, 0xFFFFFFFF},
    {C_COMB_2F, 0x06400000, 0x9FFFFFFF},
    {COMB2D_31, 0x0A14000A, 0x7FFFFFFF},
    {COMB2D_32, 0x8C0A1020, 0xFC7FFFFF},
    {COMB2D_33, 0x03110416, 0xFFFFFFF7},
    {COMB2D_34, 0x01245678, 0xFFFFFFFF},
    {COMB2D_35, 0x02345678, 0xFFFFFFFF},
    {COMB2D_36, 0x02345678, 0xFFFFFFFF},
    {COMB2D_37, 0x01234567, 0xFFFFFFFF},
    {COMB2D_38, 0x00080014, 0x0FFFFFFF},
    {COMB2D_39, 0x00020A34, 0x7F7F7F7F},
    {C_COMB_3A, 0x00000000, 0x0000000F},
    {C_COMB_3B, 0x00002480, 0xB3FFFFFF},
    {C_COMB_3C, 0x84081D01, 0xFF3F3F03},
    {C_COMB_3D, 0x00003202, 0xFFFFFFE7},
    {C_COMB_3E, 0x0820080C, 0x3F3F3F3F},
    {C_COMB_3F, 0x0050321E, 0xFFFFFFFF},
    {C_COMB_40, 0x00007814, 0xFFFFFFFF},
    {C_COMB_41, 0x30000A20, 0xFFFFFFFF},
    {C_COMB_42, 0x00000000, 0xFFFFFFFF},
    {C_COMB_43, 0x000FC014, 0xFFDFFFFF},
    {C_COMB_44, 0x88887030, 0xBFFFFFFF},
    {C_COMB_45, 0x3E1A8064, 0xFFFFFFFF},
    {C_COMB_46, 0x96001E53, 0xFFFFFFFF},
    {C_COMB_47, 0x54280504, 0xFFFFFFFF},
    {C_COMB_48, 0x03005005, 0xFF00FF7F},
    {C_COMB_49, 0x60145353, 0xFFFFFFFF},
    {C_COMB_4A, 0x500C0010, 0xFFFFFFFF},
    {C_COMB_4B, 0x02345678, 0xFFFFFFFF},
    {C_COMB_4C, 0x0000608C, 0x777BFFFF},
    {C_COMB_4D, 0x81007110, 0xFF00F3FF},
    {C_COMB_4E, 0x60123110, 0xFFFFFFFF},
    {C_COMB_4F, 0x10654444, 0xFFFDFFFF},
    {REGTBL_END, 0x00000000, 0x00000000},
};

REGTBL_T const CODE REGTBL_COMB_PAL_N_AV[] = {
    {COMB2D_00, 0x55000018, 0xFFFFFFFF},
    {COMB2D_01, 0x80969606, 0xF9FFFF06},
    {COMB2D_02, 0x10002000, 0xFF00FF03},
    {COMB2D_03, 0x0C00208A, 0xFF01FFFF},
    {COMB2D_04, 0x54500808, 0xF7F0FFFF},
    {COMB2D_05, 0x5C000000, 0xFC000009},
    {COMB2D_06, 0x00FF1E2F, 0x00FFFFFF},
    {COMB2D_07, 0x1444000A, 0xFFFF00FF},
    {COMB2D_08, 0x01234444, 0xFFFFFFFF},
    {COMB2D_09, 0x45678888, 0xFFFFFFFF},
    {COMB2D_0A, 0xF008C198, 0xFFDFFF9F},
    {COMB2D_0B, 0x00000003, 0x8000007F},
    {COMB2D_0C, 0x10806C10, 0xFFFFFFFF},
    {COMB2D_0D, 0x80080C0A, 0x8FFFFFFF},
    {COMB2D_0E, 0x84280A08, 0xFFFFFFFF},
    {COMB2D_0F, 0x00123456, 0xFFFFFFFF},
    {C_COMB_10, 0x100AF850, 0xFFFFFFFF},
    {C_COMB_11, 0x10400000, 0xFFFFFFFF},
    {C_COMB_12, 0x00053700, 0x48FFFFFF},
    {C_COMB_13, 0xC8020444, 0xFFFFFFFF},
    {C_COMB_14, 0x0A08BEC9, 0xFFFFFFFF},
    {C_COMB_15, 0x2008BEC9, 0xFFFFFFFF},
    {C_COMB_16, 0xC00F9C10, 0xFFFFFFFF},
    {C_COMB_17, 0x00400020, 0x3FFFFFFF},
    {C_COMB_18, 0x000A8088, 0x0E3EDEFB},
    {C_COMB_19, 0x0C010444, 0xFFFFFFFF},
    {C_COMB_1A, 0x0F860078, 0xFFFFFFFF},
    {C_COMB_1B, 0x48044444, 0xFFFFFFFF},
    {C_COMB_1C, 0x1020D820, 0xFFFFFE23},
    {C_COMB_1D, 0x20202831, 0xFFFFFFFF},
    {C_COMB_1E, 0x11111111, 0xFFFFFFFF},
    {C_COMB_1F, 0x280D319E, 0xFFFFFFBF},
    {C_COMB_20, 0x00444300, 0xFFFFFFFF},
    {C_COMB_21, 0x4A101030, 0xFFFFFFFF},
    {C_COMB_22, 0x80969614, 0xFFFFFFFF},
    {C_COMB_23, 0x7008301E, 0xFFFFFFFF},
    {C_COMB_24, 0x0A018000, 0xFFFFFFFF},
    {C_COMB_25, 0x01185805, 0xFFFFFFFF},
    {C_COMB_26, 0x1C00640A, 0x7FFFFFFF},
    {C_COMB_27, 0x10E08F28, 0xFFFFFFFF},
    {C_COMB_28, 0x0010101F, 0x01FFFFFF},
    {C_COMB_29, 0x88063300, 0xFE000000},
    {C_COMB_2A, 0x8A04E2EA, 0x7FFFFFFF},
    {C_COMB_2B, 0x2D350203, 0xFFFFFFFF},
    {C_COMB_2C, 0x1420640A, 0xFFE2FFFF},
    {C_COMB_2D, 0x2B354438, 0xFFFFFFFF},
    {C_COMB_2E, 0x70231E2A, 0xFFFFFFFF},
    {C_COMB_2F, 0x06400000, 0x9FFFFFFF},
    {COMB2D_31, 0x0A14000A, 0x7FFFFFFF},
    {COMB2D_32, 0x8C0A1020, 0xFC7FFFFF},
    {COMB2D_33, 0x03110416, 0xFFFFFFF7},
    {COMB2D_34, 0x01245678, 0xFFFFFFFF},
    {COMB2D_35, 0x02345678, 0xFFFFFFFF},
    {COMB2D_36, 0x02345678, 0xFFFFFFFF},
    {COMB2D_37, 0x01234567, 0xFFFFFFFF},
    {COMB2D_38, 0x00080014, 0x0FFFFFFF},
    {COMB2D_39, 0x00020A34, 0x7F7F7F7F},
    {C_COMB_3A, 0x00000000, 0x0000000F},
    {C_COMB_3B, 0x00002480, 0xB3FFFFFF},
    {C_COMB_3C, 0x84081D01, 0xFF3F3F03},
    {C_COMB_3D, 0x00003202, 0xFFFFFFE7},
    {C_COMB_3E, 0x0820080C, 0x3F3F3F3F},
    {C_COMB_3F, 0x0050321E, 0xFFFFFFFF},
    {C_COMB_40, 0x00007814, 0xFFFFFFFF},
    {C_COMB_41, 0x30000A20, 0xFFFFFFFF},
    {C_COMB_42, 0x00000000, 0xFFFFFFFF},
    {C_COMB_43, 0x000FC014, 0xFFDFFFFF},
    {C_COMB_44, 0x88887030, 0xBFFFFFFF},
    {C_COMB_45, 0x3E1A8064, 0xFFFFFFFF},
    {C_COMB_46, 0x96001E53, 0xFFFFFFFF},
    {C_COMB_47, 0x54280504, 0xFFFFFFFF},
    {C_COMB_48, 0x03005005, 0xFF00FF7F},
    {C_COMB_49, 0x60145353, 0xFFFFFFFF},
    {C_COMB_4A, 0x500C0010, 0xFFFFFFFF},
    {C_COMB_4B, 0x02345678, 0xFFFFFFFF},
    {C_COMB_4C, 0x0000608C, 0x777BFFFF},
    {C_COMB_4D, 0x81007110, 0xFF00F3FF},
    {C_COMB_4E, 0x60123110, 0xFFFFFFFF},
    {C_COMB_4F, 0x10654444, 0xFFFDFFFF},
    {REGTBL_END, 0x00000000, 0x00000000}
};

REGTBL_T const CODE REGTBL_COMB_SV[] = {
    {C_COMB_2A, 0x0004A2EE, 0x80FFFFFF},
    {COMB2D_01, 0x0080836D, 0xFFFCF90B},
    {COMB2D_02, 0x11113020, 0xFFFFFFFF},
    {COMB2D_05, 0x04050AC0, 0xFFFFFFF8},
    {COMB2D_0D, 0x008047FF, 0x83FFFFFF},
    {COMB2D_35, 0x180A4B00, 0xFFFFCFFF},
    //mtk01140 20091125 enable 3D CKILL 6E4[31] at initial
    {C_COMB_29, 0x80000000, 0x80000000},        
    {REGTBL_END, 0x00000000, 0x00000000}
};


/**
 * @brief TDC Initial Routine
 *
 * TDC Module Initial Routine
 *
 * @param None
 * @return None
 * @warning
 * @todo
 *
 * @pre System Initial
 * @post TDC can start
 */
void vDrvTDCInit(void)
{
//      _fgTDCDemo = FALSE;
    _bTDCEnable = FALSE;

    vDrvLoadRegTbl(REGTBL_COMB_COMMON);
    vDrvTDCOnOff(SV_OFF);
    
#if SUPPORT_PQ_FINETUNE
    vIO32WriteFldAlign(TDC_FW_00, 1, TDC_FW_DOTCRAWL_RD);
#endif
}

/**
 * @brief TDC On/Off Routine
 * 	  //mtk01140 20090902 TDC onoff switchh control rule
 *		[condition] [TDC need to be set to]
 *	1.	[SCART mode change done, SVideo is ON(vTvd3dSVInit)]				[OFF]
 * 	2.	[dis-connect from a input with TDC enable(vTvd3dConnect)]			[OFF]
 *	3.	[Hardware init => TDC init(vDrvTDCInit)]					[OFF]
 *	4.	[Connet to SCART Svideo(vScartSvConnect)]					[OFF]
 *	5.	[FIFO error occur (bFIFOERR)]							[->OFF->ON] 
 *	6.	[before change TDC DRAM based(vDrvTDCSetDramBase), and (C_COMB_2A, EN3D) is ON]	[OFF]
 *	7.	[after change TDC DRAM based(vDrvTDCSetDramBase), and (C_COMB_2A, EN3D) is ON]	[ON]
 *	8.	[before Change setting of "TDC Bandwidth saving(vDrvTDCSaveBW)"]		[OFF]
 *	9.	[after Change setting of "TDC Bandwidth saving(vDrvTDCSaveBW)"]			[ON]
 *	10.	[before Change setting of "TDC Bandwidth mode(vDrvTDCSetDAMode)"]		[OFF]
 *	11.	[after Change setting of "TDC Bandwidth mode(vDrvTDCSetDAMode)"]		[ON]
 *	12.	[before Change setting of "TDC DRAM bit(vDrvTDCSetDramMode)"]			[OFF]
 *	13.	[after Change setting of "TDC DRAM bit(vDrvTDCSetDramMode)"]			[ON] 
 * Switch TDC On/Off
 *
 * @param bOnOff: On/Off TDC
 * @return None
 * @warning
 * @todo
 *
 * @pre
 * @post
 */
void vDrvTDCOnOff(UINT8 bOnOff)
{
    CRIT_STATE_T csState;
    
#if !SUPPORT_3DCOMB
    bOnOff = SV_OFF;
#endif // #if !SUPPORT_3DCOMB

    if (IO32ReadFldAlign(C_COMB_29, DRAMBASEADR))
    {
		csState = x_crit_start();
        if ((bOnOff == SV_ON) && fgDrvTDCEnableChk())
        {
            _bTDCEnable = TRUE;
            //turn off "force dram fifo idle"
            vIO32WriteFldAlign(C_COMB_2B, SV_OFF, FSKBACK);
            vIO32WriteFldAlign(C_COMB_2A, SV_ON, EN3D);
            preCOMB_MODE =  SV_ON;
        }
        else
        {
            _bTDCEnable = FALSE;
            //force dram fifo idle
            vIO32WriteFldAlign(C_COMB_2B, SV_ON, FSKBACK);
            vIO32WriteFldAlign(C_COMB_2A, SV_OFF, EN3D);
            preCOMB_MODE = SV_OFF;
    	}
	    x_crit_end(csState);
    }
    else
    {
        LOG(1,"Zero TDC memory address\n");
    }
} 


/**
 * @brief TDC active window size
 *
 * Set TDC active window
 *
 * @param bOnOff: Active/de-active TDC
 * @return None
 */
void vDrvTDCActive(UINT8 bOnOff)
{
    if (bOnOff)
    {
        switch (_rTvd3dStatus.bTvdMode)
        {
        case SV_CS_PAL:
            // Set active window for PAL
            vIO32WriteFldAlign(C_COMB_2A, ACTIVE_WIN_PAL_X_START, HCNT3D);       // Start pixel
            vIO32WriteFldAlign(C_COMB_2A, ACTIVE_WIN_PAL_X_LENGTH, HLEN3D);      // h-length
            vIO32WriteFldAlign(C_COMB_2B, ACTIVE_WIN_PAL_Y_START, VLINEST);      // start line
            vIO32WriteFldAlign(C_COMB_2B, ACTIVE_WIN_PAL_Y_LENGTH, VLINECNT);    // v-length
            vIO32WriteFldAlign(C_COMB_2D, ACTIVE_WIN_PAL_Y_LENGTH, VLEN3D_M);    // v-length
            break;
        case SV_CS_PAL_M:
            // Set active window for PAL
            vIO32WriteFldAlign(C_COMB_2A, ACTIVE_WIN_PAL_M_X_START, HCNT3D);     // Start pixel
            vIO32WriteFldAlign(C_COMB_2A, ACTIVE_WIN_PAL_M_X_LENGTH, HLEN3D);    // h-length
            vIO32WriteFldAlign(C_COMB_2B, ACTIVE_WIN_PAL_M_Y_START, VLINEST);    // start line
            vIO32WriteFldAlign(C_COMB_2B, ACTIVE_WIN_PAL_M_Y_LENGTH, VLINECNT);  // v-length
            vIO32WriteFldAlign(C_COMB_2D, ACTIVE_WIN_PAL_M_Y_LENGTH, VLEN3D_M);  // v-length
            break;
        case SV_CS_PAL_N:
            // Set active window for PAL
            vIO32WriteFldAlign(C_COMB_2A, ACTIVE_WIN_PAL_N_X_START, HCNT3D);     // Start pixel
            vIO32WriteFldAlign(C_COMB_2A, ACTIVE_WIN_PAL_N_X_LENGTH, HLEN3D);    // h-length
            vIO32WriteFldAlign(C_COMB_2B, ACTIVE_WIN_PAL_N_Y_START, VLINEST);    // start line
            vIO32WriteFldAlign(C_COMB_2B, ACTIVE_WIN_PAL_N_Y_LENGTH, VLINECNT);  // v-length
            vIO32WriteFldAlign(C_COMB_2D, ACTIVE_WIN_PAL_N_Y_LENGTH, VLEN3D_M);  // v-length
            break;
        case SV_CS_NTSC358:
        default:
            // Set active window for NTSC
            vIO32WriteFldAlign(C_COMB_2A, ACTIVE_WIN_NTSC_X_START, HCNT3D);      // Start pixel
            vIO32WriteFldAlign(C_COMB_2A, ACTIVE_WIN_NTSC_X_LENGTH, HLEN3D);     // h-length
            vIO32WriteFldAlign(C_COMB_2B, ACTIVE_WIN_NTSC_Y_START, VLINEST);     // start line
            vIO32WriteFldAlign(C_COMB_2B, ACTIVE_WIN_NTSC_Y_LENGTH, VLINECNT);   // v-length
            vIO32WriteFldAlign(C_COMB_2D, ACTIVE_WIN_NTSC_Y_LENGTH, VLEN3D_M);   // v-length
            break;
        }
    }
    else
    {
        vIO32WriteFldAlign(C_COMB_2A, 0, HCNT3D);        // Start pixel
        vIO32WriteFldAlign(C_COMB_2A, 0, HLEN3D);        // h-length
        vIO32WriteFldAlign(C_COMB_2B, 0, VLINEST);       // start line
        vIO32WriteFldAlign(C_COMB_2B, 0, VLINECNT);      // v-length
    }
}

/**
 * @brief TDC Check Routine for Enable or not. Internal routine for TDC Module
 *
 * @param None
 * @return BOOL: TRUE/FALSE
 * @warning
 * @todo
 */
BOOL fgDrvTDCEnableChk(void)
{
    UINT8 bTmp;

    bTmp = bHwTvdMode();
    if (fgHwTvdSVID() || ((!fgIsMainTvd3d())&&(!fgIsPipTvd3d()))||
        ((bTmp != SV_CS_NTSC358) && (bTmp != SV_CS_PAL)
         && (bTmp != SV_CS_PAL_M) && (bTmp != SV_CS_PAL_N)))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/**
 * @brief Set DRAM mode
 *
 * TDC can be 10-bit or 9-bit mode for DRAM access.
 *
 * @param b10BitMode SV_ON/SV_OFF
 */
void vDrvTDCSetDramMode(UINT8 b10BitMode)
{
    BOOL fgTDCEnabled = SV_FALSE;

    // Store last TDC on/off status.
    fgTDCEnabled = IO32ReadFldAlign(C_COMB_2A, EN3D) ? SV_ON : SV_OFF;

    vDrvTDCOnOff(SV_OFF);

    // Turn on 10-bit 3D comb.
    if (b10BitMode)
    {
        vIO32WriteFldAlign(C_COMB_2A, SV_ON, B10MODE);
    }
    // Turn on 9-bit 3D comb.
    else
    {
        vIO32WriteFldAlign(C_COMB_2A, SV_OFF, B10MODE);
    }

    // Re-enable TDC.
    if (fgTDCEnabled)
    {
        vDrvTDCOnOff(SV_ON);
    }

}

/**
 * @brief Set TDC DRAM Access Mode
 *
 * Set TDC dram access mode High BW or Low BW.
 *
 * @param bOnOff: E_TDC_HIGH_BW/E_TDC_LOW_BW
 * @return None
 * @warning Remember to Off TDC before change setting
 * @todo Confirm TVD timgen setting for phase lock.
 *
 * @pre
 * @post
 */
void vDrvTDCSetDAMode(UINT8 bMode)
{
    BOOL fgTDCEnabled = SV_FALSE;

    // Store last TDC on/off status.
    fgTDCEnabled = IO32ReadFldAlign(C_COMB_2A, EN3D) ? SV_ON : SV_OFF;

    vDrvTDCOnOff(SV_OFF);

    if (bMode == E_TDC_LOW_BW)
    {
        vIO32WriteFldAlign(C_COMB_2A, 2, COMB_OPTION);
    }
    else                        // if (bMode == E_TDC_HIGH_BW)
    {
        vIO32WriteFldAlign(C_COMB_2A, 0, COMB_OPTION);
    }


    // Re-enable TDC.
    if (fgTDCEnabled)
    {
        vDrvTDCOnOff(SV_ON);
    }

}


/*void vDrvTDCSaveBW(UINT8 bOnOff)
{
	//avoid link error from nptv_pq_cmd.c
	return;
}
*/
/**
 * @brief TDC Set Parameter
 *
 * Set TDC Parameter for different TV systems.
 *
 * @param None
 * @return None
 * @warning Remember to Off TDC before change setting
 * @todo Confirm TVD timgen setting for phase lock.
 *
 * @pre
 * @post
 */
void vDrvTDCSet(void)
{
	BOOL fgTDCEnabled = SV_FALSE;
    //AAF Setting---
    if(_rTvd3dStatus.eSourceType==SV_ST_TV)
    {
        switch (_rTvd3dStatus.bTvdMode)
        {
            case SV_CS_PAL:
            case SV_CS_PAL_N:
            case SV_CS_PAL_60:  
            case SV_CS_PAL_M:    
                vTvd3dSetAAF(4);
                break;
            case SV_CS_SECAM:
                vTvd3dSetAAF(0);
                break;
            case SV_CS_NTSC443:
            case SV_CS_NTSC358:  
                vTvd3dSetAAF(3);
            default:
                break;
        }
    }
    else
    {
        vTvd3dSetAAF(0);
    }
    //---AAF Setting
    //YCDelay
    vTvd3dSetYCDelay(0);


    if (fgHwTvdSVID())
    {
        vDrvLoadRegTbl(REGTBL_COMB_SV);
        return;
    }
    // Store last TDC on/off status.    
    fgTDCEnabled = IO32ReadFldAlign(C_COMB_2A, EN3D) ? SV_ON : SV_OFF;        
	vDrvTDCOnOff(SV_OFF);

    //force dram fifo idle
    vIO32WriteFldAlign(C_COMB_2B, SV_ON, FSKBACK);
    // turn off 3d before mode change.
    vIO32WriteFldAlign(C_COMB_2A, SV_OFF, EN3D);

    switch (_rTvd3dStatus.bTvdMode)
    {
    case SV_CS_PAL:
        vDrvLoadRegTbl(REGTBL_COMB_PAL_AV);
        vIO32WriteFldAlign(CTG_00, SV_OFF, BST_0DEG);     // Change TVD CGen color 45 degree .

        #if SUPPORT_PQ_FINETUNE
        vIO32WriteFldAlign(TDC_FW_01, 0x4000, COLOR_EDGE_TH);
        #endif
        
        break;
    case SV_CS_PAL_M:
        vDrvLoadRegTbl(REGTBL_COMB_PAL_M_AV);
        vIO32WriteFldAlign(CTG_00, SV_OFF, BST_0DEG);     // Change TVD CGen color 45 degree .
        break;
    case SV_CS_PAL_N:
        vDrvLoadRegTbl(REGTBL_COMB_PAL_N_AV);
        vIO32WriteFldAlign(CTG_00, SV_OFF, BST_0DEG);     // Change TVD CGen color 45 degree .
        break;
    case SV_CS_SECAM:
        //Enable CKill bypass Y/C seperation only under not SECAM (To reduce color fleshing).
        vIO32WriteFldAlign(C_COMB_2B, SV_OFF, ENYCCKILL);
        vIO32WriteFldAlign(CTG_00, SV_ON, BST_0DEG); 		// Nomatter
        break;
    case SV_CS_NTSC443:
        vDrvLoadRegTbl(REGTBL_COMB_NTSC443);
        vIO32WriteFldAlign(C_COMB_14, 0x09052E, PCNTH2_1);       // 1126*525=591150
        vIO32WriteFldAlign(C_COMB_15, 0x09052E, PCNTH1_1);       // 1126*525=591150
        vIO32WriteFldAlign(CTG_00, SV_ON, BST_0DEG);     // Change TVD CGen color 0 degree .
        break;
    case SV_CS_PAL_60:
        vDrvLoadRegTbl(REGTBL_COMB_PAL_60);
        vIO32WriteFldAlign(C_COMB_14, 0x09073F, PCNTH2_1);       // 1127*525+4=591679
        vIO32WriteFldAlign(C_COMB_15, 0x09073F, PCNTH1_1);       // 1127*525+4=591679
        vIO32WriteFldAlign(CTG_00, SV_OFF, BST_0DEG);     // Change TVD CGen color 45 degree .
        break;
    case SV_CS_NTSC358:
    default:
        vDrvLoadRegTbl(REGTBL_COMB_NTSC_AV);
        vIO32WriteFldAlign(CTG_00, SV_ON, BST_0DEG);     // Change TVD CGen color 0 degree .
        break;
    }
    //other PQ setting
    #if SUPPORT_PQ_FINETUNE
    switch (_rTvd3dStatus.bTvdMode)
    {
        case SV_CS_NTSC358:
            vIO32WriteFldAlign(METER_CFG_00, 0x2, METER_CC_SAT_RDC_M);
            break;
        case SV_CS_PAL:
            vIO32WriteFldAlign(METER_CFG_00, 0x3, METER_CC_SAT_RDC_M);
            break;
        default:
            vIO32WriteFldAlign(METER_CFG_00, 0xF, METER_CC_SAT_RDC_M);
            break;
    }
    #endif
    // Re-enable TDC.        
    if (fgTDCEnabled)    
    {
		vDrvTDCOnOff(SV_ON);    
    } 
}

#if ADAPTIVE_GOH
static void vAdaptiveGOH(void)
{
    // Only enable go 1D for VCR (2D) mode.
    if (IO32ReadFldAlign(STA_COMB_0C, NONVCR3D))
    {
        vIO32WriteFldAlign(COMB2D_00, SV_OFF, ENGOH);
    }
    else
    {
        vIO32WriteFldAlign(COMB2D_00, SV_ON, ENGOH);
    }
}
#endif

#if GOH_FROM_1D
static void vGOHFrom1D(void)
{
    UINT16 wHorPixelValue;
    UINT16 wHorPixelValueDefault;
    wHorPixelValue = IO32ReadFldAlign(TG_STA_05, STA_LCNT);
    switch(_rTvd3dStatus.bTvdMode)
    {
    case SV_CS_PAL_M:
    case SV_CS_NTSC358:
    case SV_CS_PAL_60:
    case SV_CS_NTSC443:
        wHorPixelValueDefault = 1716;
        break;
    default:
        wHorPixelValueDefault = 1728;
    }

    if ((wHorPixelValue >= wHorPixelValueDefault -IO32ReadFldAlign(COMB2D_0D, GOHTH))
        &&(wHorPixelValue <= wHorPixelValueDefault +IO32ReadFldAlign(COMB2D_0D, GOHTH)))
    {
        vIO32WriteFldAlign(COMB2D_05, SV_OFF, REG_PURE1D);
    }
    else
    {
        vIO32WriteFldAlign(COMB2D_05, SV_ON, REG_PURE1D);
    }
}
#endif

static void vAdaptiveCKill(void)
{
    // If VCR is trick or SECAM, turn off CKill bypass yc.
    if (((_rTvd3dStatus.fgTrick > 0)
       &&(_bSrcMainNew != (UINT8) SV_VS_TUNER1)
       &&(_bSrcSubNew != (UINT8) SV_VS_TUNER1))
       ||(_rTvd3dStatus.bTvdMode == SV_CS_SECAM))
    {
   //     vIO32WriteFldAlign(C_COMB_2B, SV_OFF, ENYCCKILL);
     //   vIO32WriteFldAlign(C_COMB_2B, SV_OFF, ENCKILL);
    }
    else
    {
     //   vIO32WriteFldAlign(C_COMB_2B, SV_ON, ENYCCKILL);
      //  vIO32WriteFldAlign(C_COMB_2B, SV_ON, ENCKILL);
    }
}

static void v3DCombVNCCheck(void)
{
    if(IO32ReadFldAlign(STA_COMB_0C, NONVCRTG)==0)
    {
        //VCR mode on (bypass 3D)
        vDrvTDCActive(SV_OFF);
    }
    else
    {
        //VCR mode off (enable 3D)
        if(fgDrvTDCEnableChk())
        {
            vDrvTDCActive(SV_ON);
        }        
    }
}

static void vAdaptive3DCombGain(void)
{
    UINT8  bTVDNoiseLevel;
    UINT32 dwTdc3dMotion;
    UINT32 dwTdc3dColorSum;
    
    bTVDNoiseLevel = bTvdSignalStrengthLevel(1);
    dwTdc3dMotion = IO32ReadFldAlign(STA_COMB_05, MOPIXCNTSTA);
    dwTdc3dColorSum = IO32ReadFldAlign(STA_COMB_08, COLORSUMSTA);
    //New 3D function, 3D PAL CVBS diff is so big, the Fluke output Colorbar with Circle, have some shake on the circle
    if ((_rTvd3dStatus.eSourceType==SV_ST_TV)&&(_rTvd3dStatus.bTvdMode == SV_CS_PAL))
    {
        if ((IO32ReadFldAlign(STA_COMB_0C, STILL_FRAME)==1)
            &&(dwTdc3dColorSum >=0x2000000)
            &&(dwTdc3dMotion>0x6000)&&(dwTdc3dMotion<0xF000)
            )
        {
            vIO32WriteFldAlign(C_COMB_1B, 0, REG_D3GAINCV);
            vIO32WriteFldAlign(C_COMB_1B, 2, REG_D3GAINY);
        }
        else
        {
			//vIO32WriteFldAlign(C_COMB_1B, 4, D3GAIN_CV);
            vIO32WriteFldAlign(C_COMB_1B, 0, REG_D3GAINY);
            
            if ((bTVDNoiseLevel >= 0x3D)&&(bTVDNoiseLevel <= 0x47))
            {
                vIO32WriteFldAlign(C_COMB_1B, 4, REG_D3GAINCV);
                vIO32WriteFldAlign(C_COMB_4A, 0x58 +((0x47 -bTVDNoiseLevel)<<4), REG_LOSTI_LUMATH);
                vIO32WriteFldAlign(C_COMB_43, 0x16 +(0x47 -bTVDNoiseLevel), REG_MOTIONTH);
            }
            else if (bTVDNoiseLevel > 0x47)
            {
                vIO32WriteFldAlign(C_COMB_1B, 4, REG_D3GAINCV);
                vIO32WriteFldAlign(C_COMB_4A, 0x50, REG_LOSTI_LUMATH);
                vIO32WriteFldAlign(C_COMB_43, 0x14, REG_MOTIONTH);
            }
            else if ((bTVDNoiseLevel == 0x3C))
            {
                vIO32WriteFldAlign(C_COMB_1B, 2, REG_D3GAINCV);
                vIO32WriteFldAlign(C_COMB_4A, 0xFF, REG_LOSTI_LUMATH);
                vIO32WriteFldAlign(C_COMB_43, 0x22, REG_MOTIONTH);
            }
            else if ((bTVDNoiseLevel <= 0x3B))
            {
                vIO32WriteFldAlign(C_COMB_1B, 1, REG_D3GAINCV);
                vIO32WriteFldAlign(C_COMB_4A, 0xFF, REG_LOSTI_LUMATH);
                vIO32WriteFldAlign(C_COMB_43, 0x22, REG_MOTIONTH);
            }
        }        
	}
}


void vTdcSetPureVSettingForFsc(void)
{
    if (bHwTvdMode()==SV_CS_NTSC358)
    {
        if (IO32ReadFldAlign(STA_COMB_0C, NONVCRTG)==0)
        {
            vIO32WriteFldAlign(COMB2D_0E, 0x40,PVCVBSVERTH);
        }
        else
        {
            vIO32WriteFldAlign(COMB2D_0E, 0x20,PVCVBSVERTH);
        }
    }
    else if (bHwTvdMode()==SV_CS_PAL)
    {
        if ((IO32ReadFldAlign(STA_COMB_0C, NONVCRTG)==0)||(_rTvd3dStatus.eSourceType==SV_ST_TV))
        {
            vIO32WriteFldAlign(COMB2D_0E, 0x60,PVCVBSVERTH);
        }
        else
        {
            //vIO32WriteFldAlign(COMB2D_0E, 0x28,PVCVBSVERTH); //do not roll back when signal is standard
        }
    }
}
void vWeakCOff(void)
{
    if (_rTvd3dStatus.eSourceType == SV_ST_TV)
    {
        vIO32WriteFldAlign(COMB2D_04,0,ENFWEAKC);
    }
}
#if SUPPORT_PQ_FINETUNE
static void vTdc3DCombGain(void)
{
    UINT8  bTVDNoiseLevel = bTvdSignalStrengthLevel(1);
    UINT32 dwTdc3dMotion = IO32ReadFldAlign(STA_COMB_05, MOPIXCNTSTA);
    UINT32 dwTdc3dColorSum = IO32ReadFldAlign(STA_COMB_08, COLORSUMSTA);
    UINT32 dwTdc3dColorEdge = IO32ReadFldAlign(STA_COMB_0B, COLOREDGESTA);
    
    //3D PAL CVBS diff is so big, the signal like Colorbar(or handover bar part) with Circle, have some shake on the circle
    if (_rTvd3dStatus.eSourceType == SV_ST_TV)
    {
        //CL = 1191801/1196469
        if(_rTvd3dStatus.bTvdMode == SV_CS_PAL)
        {   
            //CL = 1215364/1172671/1190130/1194340/1202878/1217080/
            if ((((dwTdc3dColorSum > 0x500000) && (dwTdc3dColorSum < 0x2500000))               
                &&  (dwTdc3dMotion < 0xD000))  //for LG still channle E-05/E-25/I-11/NZ-01  & combination pattern etc.
                || (((dwTdc3dColorSum > 0x2D00000) && (dwTdc3dColorSum < 0x4000000))
                && ((dwTdc3dColorEdge > 0x6D0000) && (dwTdc3dColorEdge < 0x7F0000))
                && (dwTdc3dMotion < 0x6500)))   //E-52
            {
                if (bTVDNoiseLevel >= 0x3D)
                {
                    vIO32WriteFldAlign(C_COMB_1B, 4, REG_D3GAINCV);
                    vIO32WriteFldAlign(C_COMB_4A, 0xB8, REG_LOSTI_LUMATH);
                    vIO32WriteFldAlign(C_COMB_13, 0xFF, REG_PALMOTION45TH);
                    vIO32WriteFldAlign(C_COMB_13, 0x0, REG_D3GAIN_CV_SM);
                    vIO32WriteFldAlign(MCVP_KC_2F, 0x50, PAL_C_AVG_MAX_DIFF_VALUE);
                }
                else
                {
                    vIO32WriteFldAlign(C_COMB_1B, 2, REG_D3GAINCV);
                    vIO32WriteFldAlign(C_COMB_4A, 0x50, REG_LOSTI_LUMATH);
                    vIO32WriteFldAlign(C_COMB_13, 0x82, REG_PALMOTION45TH);
                    vIO32WriteFldAlign(C_COMB_13, 0x0, REG_D3GAIN_CV_SM);
                    vIO32WriteFldAlign(MCVP_KC_2F, 0x80, PAL_C_AVG_MAX_DIFF_VALUE);
                }
            }                      
        }  
        if((_rTvd3dStatus.bTvdMode == SV_CS_PAL_M) || (_rTvd3dStatus.bTvdMode == SV_CS_PAL_N))
        {
            if (((dwTdc3dColorSum > 0x1000000) && (dwTdc3dColorSum < 0x1500000))
                && (dwTdc3dMotion < 0xE000))    //AN-10/AN-69
            {
                if (bTVDNoiseLevel >= 0x34)
                {
                    vIO32WriteFldAlign(C_COMB_1B, 4, REG_D3GAINCV);
                    vIO32WriteFldAlign(C_COMB_4A, 0x78, REG_LOSTI_LUMATH);
                    vIO32WriteFldAlign(C_COMB_13, 0xFF, REG_PALMOTION45TH);
                    vIO32WriteFldAlign(MCVP_KC_2F, 0x50, PAL_C_AVG_MAX_DIFF_VALUE);
                }
                else
                {
                    vIO32WriteFldAlign(C_COMB_1B, 0, REG_D3GAINCV);
                    vIO32WriteFldAlign(C_COMB_4A, 0x78, REG_LOSTI_LUMATH);
                    vIO32WriteFldAlign(C_COMB_13, 0xFF, REG_PALMOTION45TH);
                    vIO32WriteFldAlign(MCVP_KC_2F, 0x80, PAL_C_AVG_MAX_DIFF_VALUE);
                }
            }
            else
            {
                vIO32WriteFldAlign(C_COMB_1B, 4, REG_D3GAINCV);
                vIO32WriteFldAlign(C_COMB_4A, 0x50, REG_LOSTI_LUMATH);
                vIO32WriteFldAlign(C_COMB_13, 0xC8, REG_PALMOTION45TH);
                vIO32WriteFldAlign(MCVP_KC_2F, 0x20, PAL_C_AVG_MAX_DIFF_VALUE);
            }
        }
    }
}
static void vColorBarTransitionForFSC(void)
{
    UINT32 dwTdc3dColorSum = IO32ReadFldAlign(STA_COMB_08, COLORSUMSTA);
//    UINT32 dwTdc3dMotion = IO32ReadFldAlign(STA_COMB_05, MOPIXCNTSTA);
    UINT32 dwDIHomoSum = IO32ReadFldAlign(MCVP_FUSION_15, IF_PD_SUM_HOMO_DN); 
    
    if (_rTvd3dStatus.eSourceType == SV_ST_TV)
    {
        //for color bar change FSC from 0~80hz case
        if ((_rTvd3dStatus.bTvdMode == SV_CS_PAL_M) || (_rTvd3dStatus.bTvdMode == SV_CS_PAL_N))
        {   
            //CL = 1186240/1190106/194177/
            if (((dwTdc3dColorSum >= 0x2000000) && (dwDIHomoSum < 0xA000))
                || ((IO32ReadFldAlign(STA_COMB_0C, NONVCR3D)== 0)&& (dwDIHomoSum < 0x100))) //100% color bar
            {
                vIO32Write4B(COMB2D_34, 0x1111111);
            }
            else
            {
                vIO32Write4B(COMB2D_34, 0x1245678);
            }
        }        
    }
}

static void vTdc3DStable(void)
{
    UINT32 u4MotionPixelCnt = RegReadFldAlign(STA_COMB_05, MOPIXCNTSTA);        
    UINT32 dwTdc3dLumasum = IO32ReadFldAlign(STA_COMB_07, LUMASUMSTA);
    UINT32 dwTdc3dMBPixCnt = IO32ReadFldAlign(STA_COMB_06, MBPIXCNTSTA);
    UINT32 dwTdc3dColorEdgeSum = IO32ReadFldAlign(STA_COMB_0B, COLOREDGESTA);
    UINT32 dwTdc3dColorSum = IO32ReadFldAlign(STA_COMB_08, COLORSUMSTA);
    //UINT32 dwTdc3dLumaEdgeSum = IO32ReadFldAlign(STA_COMB_09, LUMAEDGESTA);

    if (bHwTvdMode()==SV_CS_PAL)
    {
        if (((dwTdc3dMBPixCnt>0x600)&&(dwTdc3dMBPixCnt<0x800))
            &&((dwTdc3dLumasum>0xD800000)&&(dwTdc3dLumasum<0xE800000))
            &&((dwTdc3dColorSum>0x1200000)&&(dwTdc3dColorSum<0x1400000))
            &&((dwTdc3dColorEdgeSum>0x180000)&&(dwTdc3dColorEdgeSum<0x280000))
        )//7
        {
            vRegWriteFldAlign(C_COMB_1A, MIN(0xB0, RegReadFldAlign(C_COMB_1A, REG_D3BIGTH)+0x4), REG_D3BIGTH);
        }
        else
        {
            vRegWriteFldAlign(C_COMB_1A, MAX(0x8A, RegReadFldAlign(C_COMB_1A, REG_D3BIGTH)-0x4), REG_D3BIGTH);
        }

    }
    if (bHwTvdMode()==SV_CS_NTSC358)//some still pattern have flicker
    {
        if (u4MotionPixelCnt <= 0x120)
        {
            vRegWriteFldAlign(C_COMB_22, MIN(20, 0x120 - u4MotionPixelCnt)+0xAA, REG_CVBSDIFFTH);
        }
        else
        {
            vRegWriteFldAlign(C_COMB_22, 0xAA, REG_CVBSDIFFTH);
        }
    }
}

static void vTdcRNRSetting(void)
{
    //UINT32 u4MotionPixelCnt = RegReadFldAlign(STA_COMB_05, MOPIXCNTSTA);        
    UINT32 dwTdc3dLumasum = IO32ReadFldAlign(STA_COMB_07, LUMASUMSTA);
    UINT32 dwTdc3dMBPixCnt = IO32ReadFldAlign(STA_COMB_06, MBPIXCNTSTA);
    UINT32 dwTdc3dColorEdgeSum = IO32ReadFldAlign(STA_COMB_0B, COLOREDGESTA);
    UINT32 dwTdc3dColorSum = IO32ReadFldAlign(STA_COMB_08, COLORSUMSTA);
    UINT32 dwTdc3dLumaEdgeSum = IO32ReadFldAlign(STA_COMB_09, LUMAEDGESTA);

    if (bHwTvdMode()==SV_CS_PAL)
    {
        if (((dwTdc3dMBPixCnt>0x350)&&(dwTdc3dMBPixCnt<0x480))
            &&((dwTdc3dLumasum>0xD000000)&&(dwTdc3dLumasum<0xD900000))
            &&((dwTdc3dColorSum>0x1C00000)&&(dwTdc3dColorSum<0x1E00000))
            &&((dwTdc3dLumaEdgeSum>0x100000)&&(dwTdc3dLumaEdgeSum<0x128000))
            &&((dwTdc3dColorEdgeSum>0x700000)&&(dwTdc3dColorEdgeSum<0x8C0000))
        )//46
        {
            //vIO32WriteFldAlign(NR_CTRL_00, 1, EN_RNR_TH_ADAP);
        }
        else
        {
            //vIO32WriteFldAlign(NR_CTRL_00, 0, EN_RNR_TH_ADAP);
        }
    }

    
}

static void vTdcCrossColorReduce2(void)
{
    UINT32 u4MotionPixelCnt = RegReadFldAlign(STA_COMB_05, MOPIXCNTSTA);        
    UINT32 dwTdc3dLumasum = IO32ReadFldAlign(STA_COMB_07, LUMASUMSTA);
    UINT32 dwTdc3dMBPixCnt = IO32ReadFldAlign(STA_COMB_06, MBPIXCNTSTA);
    UINT32 dwTdc3dColorEdgeSum = IO32ReadFldAlign(STA_COMB_0B, COLOREDGESTA);
    //UINT32 dwTdc3dLumaEdgeSum = IO32ReadFldAlign(STA_COMB_09, LUMAEDGESTA);
    UINT32 dwTdc3dColorSum = IO32ReadFldAlign(STA_COMB_08, COLORSUMSTA);
    UINT16 u2MjcMvZeroCnt = IO32ReadFldAlign(MJC_STA_MH_1A, MJC_STA_MV_ZERO);
    static UINT8 bPre2DGain = 0x4;
    UINT8 bTarget2DGain;
    UINT8 b3DGainC;
    UINT8 u1IsHammock = 0;
    
    if (bHwTvdMode()==SV_CS_PAL)
    {
        b3DGainC = 4;
        if((u4MotionPixelCnt<=0xE00)
            &&(u2MjcMvZeroCnt > 0x6500)
            &&((dwTdc3dLumasum>=0xF400000)&&(dwTdc3dLumasum<=0x10200000))
            //&&((dwTdc3dMBPixCnt>0x1200)&&(dwTdc3dMBPixCnt<0x1300))
            &&((dwTdc3dColorEdgeSum>1700000)&&(dwTdc3dColorEdgeSum<5800000))
            &&((dwTdc3dColorSum>5000000)&&(dwTdc3dColorSum<9200000)))//for hammock
        {
            //vRegWriteFldAlign (COMB_3D_0E, MIN(0x10, (0x2000-u4MotionPixelCnt)>>7), CB_3D_FW_2D_COST);
            //vRegWriteFldAlign(MCVP_FUSION_21, 0, IF_MAX_MOTION_C);
            u1fgPatternHammock = 1;
            vDrvDISetChromaMaxMoLevel(0);
            vRegWriteFldAlign(COMB2D_31, 0x20, VWY2CHROMATH);
            vRegWriteFldAlign(C_COMB_23, 0x8A, REG_MBMAXTH);
            
        }
        else if (_bK4ChromaThByCans == 4)
        {
            u1fgPatternHammock = 0;
            vDrvDISetChromaMaxMoLevel(0);
            vRegWriteFldAlign(COMB2D_31, 0x12, VWY2CHROMATH);
            vRegWriteFldAlign(C_COMB_23, 0x90, REG_MBMAXTH);
        }
        else if (_fgMovingcolorCharacter == 1)
        {
            u1fgPatternHammock = 0;
            vDrvDISetChromaMaxMoLevel(5);
            vRegWriteFldAlign(COMB2D_31, 0xD, VWY2CHROMATH);
            vRegWriteFldAlign(C_COMB_23, 0xA0, REG_MBMAXTH);
        }
        else if (_bPatchInPattern107 == 1)
        {
            u1fgPatternHammock = 0;
            b3DGainC = 1;
            vDrvDISetChromaMaxMoLevel(8);
            vRegWriteFldAlign(COMB2D_31, 0xD, VWY2CHROMATH);
            vRegWriteFldAlign(C_COMB_23, 0x71, REG_MBMAXTH);
            
        }
        else
        {
            //vRegWriteFldAlign (COMB_3D_0E, 0, CB_3D_FW_2D_COST);
            //vRegWriteFldAlign(MCVP_FUSION_21, 8, IF_MAX_MOTION_C);
            u1fgPatternHammock = 0;
            if(IO32ReadFldAlign(PSCAN_FWCS_02, FAVOR_CS_STATE)>=4)
            {
                vDrvDISetChromaMaxMoLevel(8 - IO32ReadFldAlign(PSCAN_FWCS_02, FAVOR_CS_STATE));
            }
            else
            {
                vDrvDISetChromaMaxMoLevel(8);
            }
            
            vRegWriteFldAlign(COMB2D_31, 0xD, VWY2CHROMATH);
            vRegWriteFldAlign(C_COMB_23, 0x71, REG_MBMAXTH);
            
        }
        
        if ((dwTdc3dColorEdgeSum>26000000)
            &&(dwTdc3dColorSum>48000000)
            )
        {
            vRegWriteFldAlign(C_COMB_4A, 0, REG_UNIFORMTH);
        }
        else
        {
            vRegWriteFldAlign(C_COMB_4A, 0x10, REG_UNIFORMTH);
        }
        vRegWriteFldAlign (C_COMB_1B, b3DGainC, REG_D3GAINC);
    }
    else if (_rTvd3dStatus.bTvdMode==SV_CS_NTSC358)
    {
        if ((u4MotionPixelCnt<=0x4800)&&(u2MjcMvZeroCnt > 0x6500)
            &&((dwTdc3dLumasum>=0xA800000)&&(dwTdc3dLumasum<=0xB000000))
            &&((dwTdc3dMBPixCnt>0x300)&&(dwTdc3dMBPixCnt<0x900))
            &&((dwTdc3dColorEdgeSum>0x1A0000)&&(dwTdc3dColorEdgeSum<0x310000))
            &&((dwTdc3dColorSum>0x500000)&&(dwTdc3dColorSum<0x700000)))//for hammock
        {
            //vRegWriteFldAlign (C_COMB_1B, 3, REG_D3GAINY);
            //vRegWriteFldAlign (C_COMB_1B, 4-MIN(((0x4800 - u4MotionPixelCnt)>>9),4), REG_D3GAINCV);
            u1IsHammock = 1;
            vRegWriteFldAlign (C_COMB_47, 8, CHROMA3D_OFFSET);
            vRegWriteFldAlign (C_COMB_47, 0xA, CHROMA3D_SLOP);
            
            bTarget2DGain = MIN(((0x4800 - u4MotionPixelCnt)>>8)+4, 0xF);

            vRegWriteFldAlign (C_COMB_1A, 0x84 - MIN((0x4800 - u4MotionPixelCnt)>>4, 0x84), REG_D3BIGTH);
            vRegWriteFldAlign (C_COMB_23, 0x78 - MIN((0x4800 - u4MotionPixelCnt)>>4, 0x78), REG_MBMAXTH);

            vRegWriteFldAlign (C_COMB_4C, 1, REG_ENCVMO);
            vRegWriteFldAlign (C_COMB_1F, 1, REG_ENSWEEPGAINCV);

            vRegWriteFldAlign (C_COMB_1B, 4, REG_D3GAINCV);
            vRegWriteFldAlign (C_COMB_1B, 8, REG_D3GAINC);
            vRegWriteFldAlign (C_COMB_1B, 4, REG_D3GAINY);
            vRegWriteFldAlign (C_COMB_1B, 4, REG_D2GAINCV);
            vRegWriteFldAlign (C_COMB_1B, 4, REG_D2GAINC);
            vRegWriteFldAlign (C_COMB_1B, 4, REG_D2GAINY);
            vRegWriteFldAlign (C_COMB_13, 4, REG_D2GAIN_CV_SM);
        }
        else 
        {
            if ((dwTdc3dColorSum>38000000)
                &&(dwTdc3dColorEdgeSum>20000000)
                &&(u4MotionPixelCnt<4000)// //resolution still color mulitburst pattern not still
                )

            {
                vRegWriteFldAlign (C_COMB_1B, 0, REG_D3GAINY);
                vRegWriteFldAlign (C_COMB_4C, 0, REG_ENCVMO);
                vRegWriteFldAlign (C_COMB_1A, 0x84, REG_D3BIGTH);
                vRegWriteFldAlign (C_COMB_23, 0x40, REG_MBMAXTH);
                vRegWriteFldAlign (C_COMB_1F, 0, REG_ENSWEEPGAINCV);
                bTarget2DGain = 0x9;
                
            }
            else
            {
                vRegWriteFldAlign (C_COMB_1B, 4, REG_D3GAINY);
                vRegWriteFldAlign (C_COMB_4C, 1, REG_ENCVMO);
                vRegWriteFldAlign (C_COMB_1A, 0x84, REG_D3BIGTH);
                vRegWriteFldAlign (C_COMB_23, 0x78, REG_MBMAXTH);
                vRegWriteFldAlign (C_COMB_1F, 1, REG_ENSWEEPGAINCV);
                bTarget2DGain = 4;
            }
            
            //vRegWriteFldAlign (C_COMB_1B, 4, REG_D3GAINCV);

            vRegWriteFldAlign (C_COMB_47, 0x1E, CHROMA3D_OFFSET);
            vRegWriteFldAlign (C_COMB_47, 0xF, CHROMA3D_SLOP);
        }
        //.................................................................
        if ((IO32ReadFldAlign(PSCAN_FWCS_02, FAVOR_CS_STATE)>=4)
            &&(u4MotionPixelCnt<=0x6000)
            &&((dwTdc3dLumasum>=0xB000000)&&(dwTdc3dLumasum<=0xC000000))
            &&((dwTdc3dMBPixCnt>0xB00)&&(dwTdc3dMBPixCnt<0xC00))
            &&((dwTdc3dColorEdgeSum>0x200000)&&(dwTdc3dColorEdgeSum<0x400000))
            &&((dwTdc3dColorSum>0xF80000)&&(dwTdc3dColorSum<0x1180000))
            )//pattern 107 need more 3d
        {
            vRegWriteFldAlign (C_COMB_1B, 2, REG_D3GAINCV);
        }
        else
        {
            vRegWriteFldAlign (C_COMB_1B, 4, REG_D3GAINCV);
        }

        if (bPre2DGain < bTarget2DGain)
        {
            bPre2DGain++;
        }
        else if (bPre2DGain > bTarget2DGain)
        {
            bPre2DGain--;
        }
        if(u1IsHammock == 0)
        {
        vRegWriteFldAlign (C_COMB_1B, bPre2DGain, REG_D2GAINY);
        vRegWriteFldAlign (C_COMB_1B, bPre2DGain, REG_D2GAINC);
        vRegWriteFldAlign (C_COMB_1B, bPre2DGain, REG_D2GAINCV);
        vRegWriteFldAlign (C_COMB_13, bPre2DGain, REG_D2GAIN_CV_SM);
        }
    }
}

// for moving can dot carwl redction
void vTdcDotCrawlReduce(void)
{
    UINT8 u1DCR_En = IO32ReadFldAlign(TDC_FW_00, TDC_FW_DOTCRAWL_RD);
    static UINT8 _u1PreDCR_En;
    static UINT8 K2THValue = 7;
    UINT8 bcs_state = IO32ReadFldAlign(PSCAN_FWCS_02, FAVOR_CS_STATE);
    UINT32 dwTdc3dColorEdgeSum = IO32ReadFldAlign(STA_COMB_0B, COLOREDGESTA);
    UINT32 dwTdc3dColorSum = IO32ReadFldAlign(STA_COMB_08, COLORSUMSTA);
    UINT32 dwTdc3dMBPixCnt = IO32ReadFldAlign(STA_COMB_06, MBPIXCNTSTA);
    UINT32 u4MotionPixelCnt = RegReadFldAlign(STA_COMB_05, MOPIXCNTSTA); 
    UINT32 dwTdc3dLumaEdgeSum = IO32ReadFldAlign(STA_COMB_09, LUMAEDGESTA);    
    UINT8 bEdgeSum = MIN(0xFF, dwTdc3dLumaEdgeSum>>14);
    UINT8 bK4_V_Gain;
    UINT8 bK1Gain = 0x8;
    UINT8 bK4_HDGAIN_HFY2 = 0x2;
    UINT8 bK2_TBTH_CVAR = 0x0;
    
    if ((bHwTvdMode()==SV_CS_NTSC358))
    {
        _fgCornPattern = 0;
        if ((bcs_state>=4)
            && ((dwTdc3dColorEdgeSum >= 2200000)&&(dwTdc3dColorEdgeSum <= 4000000))
            && ((dwTdc3dColorSum >= 11000000)&&(dwTdc3dColorSum <= 18500000))
            && ((dwTdc3dMBPixCnt >= 700)&&(dwTdc3dMBPixCnt <= 3400))//cans pattern
            )
        {
            if (u4MotionPixelCnt>50000)//cans pattern
            {
                vRegWriteFldAlign(COMB2D_31, MAX(0x28, RegReadFldAlign(COMB2D_31, VWY2CHROMATH)-0x10), VWY2CHROMATH);
                vRegWriteFldAlign(COMB2D_33, 0x4, HDGAIN_HFY2);
                vRegWriteFldAlign(C_COMB_48, 0x1, TBTH_CVAR); 
                _fgMovingCans = 1;
            }
            else //107
            {
                vRegWriteFldAlign(COMB2D_31, MAX(0x28, RegReadFldAlign(COMB2D_31, VWY2CHROMATH)-0x10), VWY2CHROMATH);
                vRegWriteFldAlign(COMB2D_33, MAX(0x2, RegReadFldAlign(COMB2D_33, HDGAIN_HFY2)-0x1), HDGAIN_HFY2);
                vRegWriteFldAlign(C_COMB_48, 3, TBTH_CVAR);            
            }
            
        }
        else if (((dwTdc3dColorEdgeSum >= 2000000)&&(dwTdc3dColorEdgeSum <= 12000000))
            && ((dwTdc3dColorSum >=12000000)&&(dwTdc3dColorSum <= 34000000))
            && ((dwTdc3dMBPixCnt >= 100)&&(dwTdc3dMBPixCnt <= 2400))// Corn and sunflower
            && (u4MotionPixelCnt>4500)
            )
        {
            vRegWriteFldAlign(C_COMB_48, 0x05, TBTH_CVAR);
            vRegWriteFldAlign(COMB2D_31, MIN(0x88, RegReadFldAlign(COMB2D_31, VWY2CHROMATH)+0x10), VWY2CHROMATH);
            vRegWriteFldAlign(COMB2D_33, MIN(0x6, RegReadFldAlign(COMB2D_33, HDGAIN_HFY2)+0x1), HDGAIN_HFY2);
            _fgCornPattern = 1;
        }
        else
        {
            vRegWriteFldAlign(C_COMB_48, 4, TBTH_CVAR);
            vRegWriteFldAlign(COMB2D_31, MAX(0x50, RegReadFldAlign(COMB2D_31, VWY2CHROMATH)-0x10), VWY2CHROMATH);
            vRegWriteFldAlign(COMB2D_33, MIN(0x6, RegReadFldAlign(COMB2D_33, HDGAIN_HFY2)+0x1), HDGAIN_HFY2);
            vRegWriteFldAlign(COMB2D_07, 0x0, ENSHARP);        
            vIO32Write4B(COMB2D_08, 0x1444000A);  
        }
    }
    else if ((bHwTvdMode()==SV_CS_PAL))//Cans pattern check
    {
        UINT32 u4SCEHueHist0 = IO32ReadFldAlign(HUE_HIST_1_0_MAIN, HUE_HIST_0);
        UINT32 u4SCEHueHist2 = IO32ReadFldAlign(HUE_HIST_3_2_MAIN, HUE_HIST_2);
        UINT32 u4SCEHueHist5 = IO32ReadFldAlign(HUE_HIST_5_4_MAIN, HUE_HIST_5);
        _bPatchInPattern107 = 0;
        _bK4ChromaThByCans =0;
        _fgMovingcolorCharacter =0;
        bK4_V_Gain = 3;
        if (IO32ReadFldAlign(PSCAN_FWCS_02, FAVOR_CS_STATE) > 4)
        {
            bEdgeSum = bEdgeSum - MIN(bEdgeSum, (RegReadFldAlign(PSCAN_FWCS_02, FAVOR_CS_STATE)-4)<<3);
        }
        
        if (u1fgPatternHammock == 1)
        {
            bK4_V_Gain = 3;
            K2THValue = 6;
        }
        else if ((u1MajorMvX0>=0)&&(u1MajorMvY0 == 0)
            &&((dwTdc3dColorEdgeSum >= 0x800000)&&(dwTdc3dColorEdgeSum <= 0x10F0000))
            && ((dwTdc3dColorSum >= 0x3000000)&&(dwTdc3dColorSum <= 0x3600000))
            && ((dwTdc3dMBPixCnt >= 0xA00)&&(dwTdc3dMBPixCnt <= 0x2000))
            && ((u4MotionPixelCnt >= 0x2B0)&&(u4MotionPixelCnt <= 0x18000))
            && (u4SCEHueHist0<=0x50)
            && ((u4SCEHueHist2<=0x1600)&&(u4SCEHueHist2>=0x1300))
            && ((u4SCEHueHist5<=0x2500)&&(u4SCEHueHist5>=0x2000))
            )//leaf pattern
        {
            
            bK4_V_Gain = 3;
            K2THValue = 3;
            _fgMovingcolorCharacter = 1;
        }
        else if ((bcs_state>=4) && (u1MajorMvX0>=1)
            && ((dwTdc3dColorEdgeSum >= 2800000)&&(dwTdc3dColorEdgeSum <= 5950000))
            && ((dwTdc3dColorSum >= 15000000)&&(dwTdc3dColorSum <= 25500000))
            && ((dwTdc3dMBPixCnt >= 500)&&(dwTdc3dMBPixCnt <= 3500))//cans
            )
        {
            K2THValue = (K2THValue>3)?(K2THValue-1):3;
            bK4_V_Gain = 3;
            _bK4ChromaThByCans = 4;
        }
        else if((bEdgeSum>0x68) && (dwTdc3dColorSum > 0x1000000) && (dwTdc3dColorSum < 0x3500000))//corn and sunflower
        {
            K2THValue = (K2THValue<8)?(K2THValue+1):8;
            bK1Gain = 0xF;
            bK4_HDGAIN_HFY2 = 0x8;
            bK2_TBTH_CVAR = 0x5; 
            _fgCornPattern = 1;            
        }
        else if ((dwTdc3dColorSum>29000000)&&(dwTdc3dColorSum<40000000)
            &&(dwTdc3dLumaEdgeSum>800000)&&(dwTdc3dLumaEdgeSum<2000000)
            &&(dwTdc3dColorEdgeSum>8200000)&&(dwTdc3dColorEdgeSum<13000000)
            &&(dwTdc3dMBPixCnt>500)&&(dwTdc3dMBPixCnt<1200)//Thailand woman
            )
        {
            K2THValue = 2;
        }
        else if ((bcs_state>=4) &&(u4MotionPixelCnt<16000)
            && ((dwTdc3dColorSum >=17000000)&&(dwTdc3dColorSum <= 29000000))
            && ((dwTdc3dColorEdgeSum >= 2100000)&&(dwTdc3dColorEdgeSum <= 4100000))
            && ((dwTdc3dMBPixCnt >= 2700)&&(dwTdc3dMBPixCnt <= 4800))//107
            )
        {
            //K2THValue = (K2THValue>1)?(K2THValue-1):1;
            K2THValue = (K2THValue<3)?(K2THValue+1):3;
            _bPatchInPattern107 =1;
        }

        else
        {
            K2THValue = (K2THValue<3)?(K2THValue+1):3;
        }
        vRegWriteFldAlign(COMB2D_32, MIN(0x8, MAX(1, _bK2ValueByCSS+K2THValue-_bK2ValueByWeakC)), TBTH_K2B);
        vRegWriteFldAlign(COMB2D_00, bK4_V_Gain, REG_HFY_VYD_GAIN);
        vRegWriteFldAlign(COMB2D_00, bK4_V_Gain, REG_HFY_VCD_GAIN);
        vRegWriteFldAlign(COMB2D_04, bK1Gain, MK1);
        vRegWriteFldAlign(COMB2D_33, bK4_HDGAIN_HFY2, HDGAIN_HFY2);
        vRegWriteFldAlign(C_COMB_48, bK2_TBTH_CVAR, TBTH_CVAR);
    }
    else if (u1DCR_En !=_u1PreDCR_En)
    {
        vRegWriteFldAlign(COMB2D_04, 0, ENMK2);
        vRegWriteFldAlign(COMB2D_04, 0, MK2);
    }
    _u1PreDCR_En = u1DCR_En;
}

void vTdcAdaptiveFW2DSoftswitch(void)
{
    //UINT32 u4MotionPixelCnt = RegReadFldAlign(STA_COMB_05, MOPIXCNTSTA);        
    UINT32 dwTdc3dColorEdgeSum = IO32ReadFldAlign(STA_COMB_0B, COLOREDGESTA);
    UINT32 dwTdc3dLumaEdgeSum = IO32ReadFldAlign(STA_COMB_09, LUMAEDGESTA);
    //UINT8 bK2THValue;
    UINT8 bK4HGain;
    UINT8 bEdgeSum;

    if (bHwTvdMode()==SV_CS_PAL)
    {
        //bEdgeSum = MIN(0xFF, MIN(dwTdc3dColorEdgeSum>>16, dwTdc3dLumaEdgeSum>>14));
        bEdgeSum = MIN(0xFF, dwTdc3dLumaEdgeSum>>14);
        if (IO32ReadFldAlign(PSCAN_FWCS_02, FAVOR_CS_STATE) > 4)
        {
            bEdgeSum = bEdgeSum - MIN(bEdgeSum, (IO32ReadFldAlign(PSCAN_FWCS_02, FAVOR_CS_STATE)-4)<<3);
        }
        /*
        if ((RegReadFldAlign(COMB_3D_15, CB_3D_CHROMA_EDGE_SUM)<0x5)&&(RegReadFldAlign(COMB_3D_15, CB_3D_MOTION_PIXEL_CNT)>=0x3000))// for pattern 152 flicker
        {
            bUsefulBandSum = bUsefulBandSum + ((0x5 - RegReadFldAlign(COMB_3D_15, CB_3D_CHROMA_EDGE_SUM))<<4);
        }
            */

        if (u1fgPatternHammock == 1)//hammock
        {
            bK4HGain = 0x5;
        }
        else if(_fgMovingcolorCharacter == 1)
        {
            bK4HGain = 0x0;
        }
        else if (bEdgeSum>0x68)//for corn and sunflower
        {
            //bK2THValue = 0xA;
            bK4HGain = 0x8;
        }
        else if (_bK4ChromaThByCans == 4)
        {
            bK4HGain = 1;
        }
        else if (dwTdc3dColorEdgeSum < 0x500000)//shop
        {
            bK4HGain = 1;
        }
        else if (bEdgeSum<0x58)
        {
            //bK2THValue = 7;
            bK4HGain = 0x0;
        }
        else
        {
            //bK2THValue = 7;///2 + ((bEdgeSum - 0x60)>>2);
            bK4HGain = (bEdgeSum - 0x58)>>2;
        }

        bK4HGain = MIN(bK4HGain, 0x8);
        vRegWriteFldAlign(COMB2D_00, bK4HGain+1, REG_HFY_HDGAIN);
        vRegWriteFldAlign(COMB2D_00, bK4HGain, REG_HFY_HYD_GAIN);
        vRegWriteFldAlign(COMB2D_00, bK4HGain, REG_HFY_HCD_GAIN);
        
    }
    
}

void CheckSPCAndWeakC(void)// Test Pattern
{
    UINT32 dwTdc3dColorEdgeSum = IO32ReadFldAlign(STA_COMB_0B, COLOREDGESTA);
    UINT32 dwTdc3dColorSum = IO32ReadFldAlign(STA_COMB_08, COLORSUMSTA);
    UINT32 u4MotionPixelCnt = RegReadFldAlign(STA_COMB_05, MOPIXCNTSTA); 
    UINT32 dwTdc3dMBPixCnt = IO32ReadFldAlign(STA_COMB_06, MBPIXCNTSTA);
    UINT32 dwColorStatus = (dwTdc3dColorEdgeSum>>14) + (dwTdc3dColorSum>>16);
    UINT8 bSpcHorSmoothTH;
    UINT8 bWeakC_Tbth_K2;
    UINT8 bWeakC_Tbth_K1V;
    UINT8 bWeakC_Tbth_K1H;
    UINT8 b3D_D3Gain_CV;
    UINT8 bK3Value;
    if (bHwTvdMode()==SV_CS_NTSC358)
    {
        if (dwTdc3dColorSum>=0x2000000)
        {
            bSpcHorSmoothTH = 0x88;
            bWeakC_Tbth_K2 = 0x7F;

            //bypass weakC
            vRegWriteFldAlign(COMB2D_0C, 1, DISCFIRINCBG);
            //K4
            //vRegWriteFldAlign(COMB2D_01, 0, REG_MHFYK);
            //vRegWriteFldAlign(COMB2D_01, 1, REG_ENMHFY);
        }
        else if (dwTdc3dColorSum<0x1C00000)
        {
            bSpcHorSmoothTH = 0x8;
            bWeakC_Tbth_K2 = 0x2;

            vRegWriteFldAlign(COMB2D_0C, 0, DISCFIRINCBG);
            //vRegWriteFldAlign(COMB2D_01, 0, REG_ENMHFY);
        }
        else
        {
            bSpcHorSmoothTH = 0x8+ ((dwTdc3dColorSum-0x1C00000)>>15);
            bWeakC_Tbth_K2 = MIN(0x7F, 0x2+ ((dwTdc3dColorSum-0x1C00000)>>15));

            vRegWriteFldAlign(COMB2D_0C, 0, DISCFIRINCBG);
            //vRegWriteFldAlign(COMB2D_01, 0, REG_ENMHFY);
        }
    
        vRegWriteFldAlign(C_COMB_20, bSpcHorSmoothTH, REG_VDGYHORSMOOTHTH);
        vRegWriteFldAlign(COMB2D_39, bWeakC_Tbth_K2, TBTH_K2);
    
    }
    else if (bHwTvdMode()==SV_CS_PAL)
    {
        bSpcHorSmoothTH = 0x10;
        bWeakC_Tbth_K2 = 0x3;
        bWeakC_Tbth_K1V = 0xA;
        bWeakC_Tbth_K1H = 0x10;
        b3D_D3Gain_CV = 0x4;
        bK3Value = 0x8;
        
        _bK2ValueByWeakC =0;
        if ((u4MotionPixelCnt<10000)||(dwTdc3dMBPixCnt>200))
        {
            //do nothing
        }
        else if (dwColorStatus >= 0x980)
        {
            b3D_D3Gain_CV = 9;
            bK3Value = 0;
            _bK2ValueByWeakC = 2;
            
        }
        else if (dwColorStatus<=0x700)
        {   
            //do nothing
        }
        else if(_fgCornPattern == 1)
        {
            bK3Value = 0xF;
        }
        else
        {
            
            b3D_D3Gain_CV = MIN(9, b3D_D3Gain_CV + ((dwColorStatus-0x700)>>7));
            bK3Value = MIN(8, (980-dwColorStatus)>>6);
        }
        //......................................................................................
        if (dwTdc3dColorSum>=0x3000000)//pattern moving char bars in leaf disc
        {
            bSpcHorSmoothTH = 0x60;
            bWeakC_Tbth_K2 = 0x53;
            bWeakC_Tbth_K1V = 0;
            bWeakC_Tbth_K1H = 0;
            
        }
        else if (dwTdc3dColorSum <=0x2800000 )
        {
            
            //do nothing
        }
        else
        {
            bSpcHorSmoothTH = bSpcHorSmoothTH + ((dwTdc3dColorSum-0x2800000)>>17);
            bWeakC_Tbth_K2 = bWeakC_Tbth_K2 + ((dwTdc3dColorSum-0x2800000)>>17);
            bWeakC_Tbth_K1V = bWeakC_Tbth_K1V - MIN((dwTdc3dColorSum-0x2800000)>>19, bWeakC_Tbth_K1V);
            bWeakC_Tbth_K1H = bWeakC_Tbth_K1H - MIN((dwTdc3dColorSum-0x2800000)>>19, bWeakC_Tbth_K1H);
            
        }

        vRegWriteFldAlign(C_COMB_20, bSpcHorSmoothTH, REG_VDGYHORSMOOTHTH);
        vRegWriteFldAlign(COMB2D_39, bWeakC_Tbth_K2, TBTH_K2);
        vRegWriteFldAlign(COMB2D_39, bWeakC_Tbth_K1V, TBTH_K1V);
        vRegWriteFldAlign(COMB2D_39, bWeakC_Tbth_K1H, TBTH_K1H);
        vRegWriteFldAlign(COMB2D_04, bK3Value, MK3);
        vRegWriteFldAlign(C_COMB_1B, b3D_D3Gain_CV, REG_D3GAINCV);
        
    }
    
}

void vTdcCCSPatch(void)
{
    //UINT16 u2Sat = IO32ReadFldAlign(GLOBAL_ADJ_02, SAT_DUMMY);
    UINT32 u4CCState = IO32ReadFldAlign(METER_INFO_17, METER_CC_STATE);
    UINT32 u4MoPix = IO32ReadFldAlign(MCVP_STATUS_0F, OLD_FRAME_MO_Q_STATUS);
    UINT32 u4Rto = 16;
    static UINT8 _u1Cnt = 0;

    u4MoPix = ((MAX(u4MoPix, 6000) - 6000) >> 10);
    u4MoPix = MIN(u4MoPix, 8);
    u4CCState = u4CCState * u4MoPix;

    if (u4CCState >= 768)
    {
        _u1Cnt = (_u1Cnt < 32) ? _u1Cnt+1 : _u1Cnt;
    }
    else
    {
        UINT8 u1Dec = ((768 - u4CCState) >> 6);
        _u1Cnt = _u1Cnt - MIN(_u1Cnt, u1Dec);
    }

    u4Rto = MIN((MAX(_u1Cnt,12) - 12), 16);
    _bK2ValueByCSS = u4Rto>>2;
    u4Rto = 16 - u4Rto;
        
    //u2Sat = (u4Rto*u2Sat) >> 4;

    if (IO32ReadFldAlign(METER_CFG_00, METER_CC_ADAP_EN))
    {
        vIO32WriteFldAlign(METER_INFO_17, u4Rto, METER_CC_RATIO);
    }
    else
    {
        vIO32WriteFldAlign(METER_INFO_17, u4Rto, METER_CC_RATIO);
    }
}

void vTdcMainMenuTransition(void)
{
    UINT32 dwTdc3dColorEdgeSum = IO32ReadFldAlign(STA_COMB_0B, COLOREDGESTA);
    UINT32 dwThresholdColorEdge = IO32ReadFldAlign(TDC_FW_01, COLOR_EDGE_TH);
    static UINT8 bPreMbVTh = 0;
    UINT8 bMbVTh =0;
    dwTdc3dColorEdgeSum = dwTdc3dColorEdgeSum>>8;
    
    if (bHwTvdMode()==SV_CS_PAL)
    {
        if (dwTdc3dColorEdgeSum < dwThresholdColorEdge)
        {
            bMbVTh = MIN(0x20,(dwThresholdColorEdge-dwTdc3dColorEdgeSum)>>6);
            //vRegWriteFldAlign(COMB2D_00, MIN(0x20,(dwThresholdColorEdge-dwTdc3dColorEdgeSum)>>6),REG_HFY_MBVTH);
        }
        else 
        {
            bMbVTh = 0;
            //vRegWriteFldAlign(COMB2D_00, 0,REG_HFY_MBVTH);
        }

        if ((RegReadFldAlign(STA_COMB_05, MOPIXCNTSTA)< 0x180)&&(bPreMbVTh < bMbVTh))
        {
            bPreMbVTh = bPreMbVTh+1;
        }
        else if(bPreMbVTh>0)
        {
            bPreMbVTh = bPreMbVTh - 1;
        }
        vRegWriteFldAlign(COMB2D_00, bPreMbVTh, REG_HFY_MBVTH);
    }
}
void vTdcSharpness(void)
{
    if(_fgMovingCans == 1)
    {
        vRegWriteFldAlign(COMB2D_07, 0x1, ENSHARP);        
        vIO32Write4B(COMB2D_08, 0x44446666); 
    }
    else
    {
        vRegWriteFldAlign(COMB2D_07, 0x0, ENSHARP);        
        vIO32Write4B(COMB2D_08, 0x1234444); 
    }
}
#endif

/**
 * @brief TDC Interrupt Service Routine
 *
 * Measure video content and adaptive tune TDC parameters.
 *
 * @param None
 * @return None
 */
void vTdc3dProc(void)
{
    UINT32 dwTdc3dLumasum;
    UINT32 dwTdc3dMBPixCnt;
    UINT32 dwTdc3dColorEdgeSum;
    UINT32 dwTdc3dLumaEdgeSum;
    UINT8  bMOTH4MB, bLSMOOTH;
    UINT8  bSweepDet, bFIFOERR;
	BOOL fgTDCEnabled = SV_FALSE;
	
    bSweepDet =  IO32ReadFldAlign(STA_COMB_0C, SWEEP_DETECTION);
    dwTdc3dLumasum = IO32ReadFldAlign(STA_COMB_07, LUMASUMSTA);
    dwTdc3dMBPixCnt = IO32ReadFldAlign(STA_COMB_06, MBPIXCNTSTA);
    dwTdc3dColorEdgeSum = IO32ReadFldAlign(STA_COMB_0B, COLOREDGESTA);
    dwTdc3dLumaEdgeSum = IO32ReadFldAlign(STA_COMB_09, LUMAEDGESTA);

    bFIFOERR = ((IO32ReadFldAlign(STA_COMB_0C, FIFO_EVERFULL))||
                 (IO32ReadFldAlign(STA_COMB_0C, FIFOEMPTY))||
                 (IO32ReadFldAlign(STA_COMB_0C, FIFOEMPTY_A))||
                 (IO32ReadFldAlign(STA_COMB_0C, FIFOEMPTY_AA)));

    //UNUSED(bTVDNoiseLevel);
    UNUSED(dwTdc3dLumaEdgeSum);
    UNUSED(dwTdc3dColorEdgeSum);
    UNUSED(dwTdc3dLumasum);
    UNUSED(dwTdc3dMBPixCnt);
    UNUSED(bMOTH4MB);
    UNUSED(bLSMOOTH);
    UNUSED(bSweepDet);  

    if (fgHwTvdSVID()||
        (!fgIsMainTvd3d() && !fgIsPipTvd3d()) ||
        (!fgIsAdaptiveFlgSet(FLG_ADAPTIVE_COMB)) )
    {
        return;
    }

    //Patch for 3dcomb dram
    if(bFIFOERR&&(_rTvd3dStatus.bSigStatus==(UINT8)SV_VDO_STABLE))
    {
        #ifndef CC_ENABLE_AUTO_CHECK_DRAM_PRIO_UNDERFLOW
		    // Re-enable TDC. 
		fgTDCEnabled = IO32ReadFldAlign(C_COMB_2A, EN3D) ? SV_ON : SV_OFF;     
        vDrvTDCOnOff(SV_OFF);
        if (fgTDCEnabled)        
        {
        	vDrvTDCOnOff(SV_ON);
        } 		
        #endif
    }

#if ADAPTIVE_2D_COMB_SHARP
/*
    if ((dwTdc3dMotion < MOTION_SMALL) ||
        (bGetSignalType(SV_VP_MAIN) == SV_ST_TV) ||
        (IO32ReadFldAlign(STA_CDET_00, NR_LEVEL) >= 4))
    {
        vIO32WriteFldAlign(COMB2D_0A, SHARP_WEAK, TDC_LC_SHARP_MID);
        vIO32WriteFldAlign(COMB2D_0A, SHARP_WEAK, TDC_LC_SHARP_HIGH);
    }
    else if (dwTdc3dMotion > MOTION_BIG)
    {
        vIO32WriteFldAlign(COMB2D_0A, SHARP_STRONG, TDC_LC_SHARP_MID);
        vIO32WriteFldAlign(COMB2D_0A, SHARP_STRONG, TDC_LC_SHARP_HIGH);
    }
    else
    {
        _bAdaptiveSharpGain =
            (dwTdc3dMotion - MOTION_SMALL) *
            (SHARP_STRONG - SHARP_WEAK) /
            (MOTION_BIG - MOTION_SMALL) +
            SHARP_WEAK;
        vIO32WriteFldAlign(COMB2D_0A, _bAdaptiveSharpGain, TDC_LC_SHARP_MID);
        vIO32WriteFldAlign(COMB2D_0A, _bAdaptiveSharpGain, TDC_LC_SHARP_HIGH);
    }
*/    
#endif // #if ADAPTIVE_2D_COMB_SHARP

#if ADAPTIVE_POST_NOTCH_FILTER
/*
    // burst un-lock => comb force to 2D and S-Video (Y=CVBS)
    // => Should disable post notch filter
    if (fgHwTvdBLock())
    {
        vIO32WriteFldAlign(COMB2D_35, SV_ON, ENPNO);
    }
    else
    {
        vIO32WriteFldAlign(COMB2D_35, SV_OFF, ENPNO);
    }
*/    
#endif // #if ADAPTIVE_POST_NOTCH_FILTER

#if ADAPTIVE_GOH
    vAdaptiveGOH();
#endif // #if ADAPTIVE_GOH

#if GOH_FROM_1D
    vGOHFrom1D();
#endif
    vAdaptiveCKill();
    v3DCombVNCCheck();
    vTdcSetPureVSettingForFsc();
    vAdaptive3DCombGain();
    vWeakCOff();
#if SUPPORT_PQ_FINETUNE
    if(_rTvd3dStatus.eSourceType==SV_ST_TV)
    {
        vTdc3DCombGain();
        vColorBarTransitionForFSC();
    }

    if((_rTvd3dStatus.eSourceType==SV_ST_AV))
    {
        vTdc3DStable();
        vTdcRNRSetting();
        vTdcCrossColorReduce2();
        vTdcDotCrawlReduce();
        vTdcAdaptiveFW2DSoftswitch();
        CheckSPCAndWeakC();
        vTdcCCSPatch();
        vTdcMainMenuTransition();
    }
#endif
}


void vDrvTDCResetCCS(void)
{
    vIO32WriteFldAlign(METER_INFO_17, 16, METER_CC_RATIO);
}



/**
 * @brief TDC Demo mode (seamless) On/Off
 *
 * Set TDC Demo mode
 *
 * @param bOnOff: SV_ON/SV_OFF
 * @return None
 * @warning Must set region param first before switch on demo mode.
 * @todo
 *
 * @pre
 * @post
 */
void vDrvTDCDemo(UINT8 bOnOff, UINT8 bRegion, UINT16 wXCenter)
{
    if (bOnOff == SV_ON)
    {
        switch (_rTvd3dStatus.bTvdMode)
        {
            case SV_CS_PAL:
                _wTDCStart = (bRegion==SV_LEFT)?ACTIVE_WIN_PAL_X_START:
                                                                   ACTIVE_WIN_DEMO_PAL_X_START;
                _wTDCLen = ACTIVE_WIN_PAL_X_LENGTH/2;                                            
            break;
            case SV_CS_PAL_M:
                _wTDCStart = (bRegion==SV_LEFT)?ACTIVE_WIN_PAL_M_X_START:
                                                                    ACTIVE_WIN_DEMO_PAL_M_X_START;
                _wTDCLen = ACTIVE_WIN_PAL_M_X_LENGTH/2;    
            break;
            case SV_CS_PAL_N:
                _wTDCStart = (bRegion==SV_LEFT)?ACTIVE_WIN_PAL_N_X_START:
                                                                   ACTIVE_WIN_DEMO_PAL_N_X_START;
                _wTDCLen = ACTIVE_WIN_PAL_N_X_LENGTH/2; 
            break;
            case SV_CS_NTSC358:
                _wTDCStart = (bRegion==SV_LEFT)?ACTIVE_WIN_NTSC_X_START:
                                                                    ACTIVE_WIN_DEMO_NTSC_X_START;
                _wTDCLen = ACTIVE_WIN_NTSC_X_LENGTH/2; 
            break;
            default://No 3D Comb.
            break;
        }
        }
        else
        {
        switch (_rTvd3dStatus.bTvdMode)
        {
            case SV_CS_PAL:
                _wTDCStart = ACTIVE_WIN_PAL_X_START;
                _wTDCLen = ACTIVE_WIN_PAL_X_LENGTH;                                            
            break;
            case SV_CS_PAL_M:
                _wTDCStart = ACTIVE_WIN_PAL_M_X_START;
                _wTDCLen = ACTIVE_WIN_PAL_M_X_LENGTH;    
            break;
            case SV_CS_PAL_N:
                _wTDCStart =ACTIVE_WIN_PAL_N_X_START;
                _wTDCLen = ACTIVE_WIN_PAL_N_X_LENGTH; 
            break;
            case SV_CS_NTSC358:
                _wTDCStart = ACTIVE_WIN_NTSC_X_START;
                _wTDCLen = ACTIVE_WIN_NTSC_X_LENGTH; 
            break;
            default://No 3D Comb.
            break;
        }
    }

    vIO32WriteFldAlign(C_COMB_2A, (UINT32) _wTDCStart, HCNT3D);
    vIO32WriteFldAlign(C_COMB_2A, (UINT32) _wTDCLen, HLEN3D);
}

/**
 * @brief TDC Set Dram Base Routine
 *
 * Set TDC Dram Base
 *
 * @param None
 * @return None
 * @warning
 * @todo
 *
 * @pre System Initial
 * @post TDC can start
 */
void vDrvTDCSetDramBase(void)
{
    BOOL fgTDCEnabled = SV_FALSE;

    // Store last TDC on/off status.
    fgTDCEnabled = IO32ReadFldAlign(C_COMB_2A, EN3D) ? SV_ON : SV_OFF;
    
    vDrvTDCOnOff(SV_OFF);

	if (TDC_DRAM_BASE & 0x40000000)
    {
		vIO32WriteFldAlign(COMB2D_38, 1, CHANNEL_B_SEL);
    }
    else
    {
		vIO32WriteFldAlign(COMB2D_38, 0, CHANNEL_B_SEL);
    }
    if (TDC_DRAM_BASE & 0x20000000)
    {
		vIO32WriteFldAlign(COMB2D_38, 1, DRAMBASEADR_MSB);
    }
    else
    {
		vIO32WriteFldAlign(COMB2D_38, 0, DRAMBASEADR_MSB);
    }
    // Update TDC Dram base.
    vIO32WriteFldAlign(C_COMB_29, TDC_DRAM_BASE >> 4, DRAMBASEADR);

    // Re-enable TDC.    
    if (fgTDCEnabled)
    {
        vDrvTDCOnOff(SV_ON);
    }

}
            
// For CLI use to check TDC active window range.
void vDrvTDCSetStart(UINT16 wStart)
{
    vIO32WriteFldAlign(C_COMB_2A, wStart, HCNT3D);
}
            
void vDrvTDCSetLength(UINT16 wLength)
{
    vIO32WriteFldAlign(C_COMB_2A, wLength, HLEN3D);
}
                        
void vDrvTDCSetTop(UINT16 wTop)
{
    vIO32WriteFldAlign(C_COMB_2B, wTop, VLINEST);
    }

void vDrvTDCSetBottom(UINT16 wBottom)
    {
    vIO32WriteFldAlign(C_COMB_2B, wBottom, VLINECNT);
    }

UINT16 wDrvTDCGetStart(void)
{
    return (UINT16) IO32ReadFldAlign(C_COMB_2A, HCNT3D);
}

UINT16 wDrvTDCGetLength(void)
{
    return (UINT16) IO32ReadFldAlign(C_COMB_2A, HLEN3D);
}

UINT16 wDrvTDCGetTop(void)
{
    return (UINT16) IO32ReadFldAlign(C_COMB_2B, VLINEST);
}

UINT16 wDrvTDCGetBottom(void)
{
    return (UINT16) IO32ReadFldAlign(C_COMB_2B, VLINECNT);
}
UINT8 u1DrvTDCChkDramOvfUdf(void)
{
    return((IO32ReadFldAlign(C_COMB_3A, FULLSTA))|
           (IO32ReadFldAlign(C_COMB_3A, EMPTYSTA)));
}
void vDrvTDCClearDramStatus(void)
{
    //Clear Status
    vIO32WriteFldMulti(C_COMB_3A,
              P_Fld(1, CLRFULL)|
              P_Fld(1, CLREMPTY));
    
    vIO32WriteFldMulti(C_COMB_3A,
              P_Fld(0, CLRFULL)|
              P_Fld(0, CLREMPTY)); 

}

/**
* @brief TDC callback function
* @param u4Arg1 VDP_1/VDP_2
* @param u4Arg2 configuration
* @param u4SramMode sram mode
*/
void vDrvTDCSrmCb(UINT32 u4Arg1, UINT32 u4Arg2, UINT32 u4PDSize, UINT32 u4SramMode)
{
    FBM_POOL_T* prPool;
    #ifndef CC_SUPPORT_RECORD_AV
    prPool = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_TDC, NULL);
	#else
	prPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_TDC);
	#endif
//    Printf("vDrvTDCSrmCb %x %x %x %x \n",u4Arg1,u4Arg2,u4PDSize,u4SramMode);
    if (prPool != NULL)
    {
        //if the setting is not for the TVD path, don't controlled by it
        //if ((fgIsMainTvd3d() && (u4Arg1 == SV_VP_PIP))||(fgIsPipTvd3d() && (u4Arg1 == SV_VP_MAIN)))
        #ifdef CC_SUPPORT_FIXEDDRAM
        if ((!(((u4Arg1 == SV_VP_MAIN)&&fgIsMainTvd3d())||((u4Arg1 == SV_VP_PIP)&&fgIsPipTvd3d())))||
        ((fgIsMainTvd3d() || fgIsPipTvd3d())&&(_rTvd3dStatus.bSigStatus==SV_VDO_STABLE)&&fgDrvTDCEnableChk()&&(preCOMB_MODE==SV_ON)))
        #else
        if (!(((u4Arg1 == SV_VP_MAIN)&&fgIsMainTvd3d())||((u4Arg1 == SV_VP_PIP)&&fgIsPipTvd3d())))
        #endif
        {
            return;
        }
        if (u4Arg2 & FBM_POOL_MODE_10BIT)
        {
            if (prPool->u4Mode & FBM_POOL_MODE_10BIT)
            {
                vDrvTDCSetDramMode(SV_ON);
            }
            else
            {
                vDrvTDCSetDramMode(SV_OFF);
            }
            TDC_DRAM_BASE = prPool->u4Addr;
            vDrvTDCSetDramBase();


		    if(fgDrvTDCEnableChk())
		    {
		    	vDrvTDCOnOff(SV_ON);
	            vDrvTDCActive(SV_ON);
			}

        }
        else
        {
            vDrvTDCOnOff(SV_OFF);
        }
    }
    else
    {
        LOG(1, "vDrvDISrmCb: Fail to get pool\n");
    }
}

