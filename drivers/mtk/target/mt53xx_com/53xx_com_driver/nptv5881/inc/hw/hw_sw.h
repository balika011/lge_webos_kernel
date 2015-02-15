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

#ifndef _HW_SW_H_
#define _HW_SW_H_

#include "hw_vdoin.h"

// DRAM Based Address stored in 0xF0022700
#define SW_REG_BASE_ADDR        (VDOIN_BASE+0x2700)

//================================================
// SW Register Macro
//================================================
extern UINT32 SW_DRAM_BASE;
extern UINT32 SW_DRAM_BASE_PHY;

#define u4SWReg(reg16)	(SW_DRAM_BASE + reg16)
#define fgIsSWRegInit() ((void *)SW_DRAM_BASE != NULL)

//=============================================================
// Addr : 0x0000 - 0x0100
// Name : Adaptive Control
//=============================================================
#define ADAPTIVE_REG u4SWReg(0x0000)
    #define ADAPTIVE_CONTROL    Fld(12, 12, AC_MSKW21) //23:12
    #define ADAPTIVE_COMB       Fld( 1, 12, AC_MSKB1) 
    #define ADAPTIVE_NR         Fld( 1, 13, AC_MSKB1) 
    #define ADAPTIVE_GETINFO    Fld( 1, 16, AC_MSKB2)     
    #define ADAPTIVE_LUMA       Fld( 1, 17, AC_MSKB2) 
    #define ADAPTIVE_BACKLIGHT  Fld( 1, 18, AC_MSKB2) 
    #define ADAPTIVE_SHARPNESS  Fld( 1, 19, AC_MSKB2) 
    #define ADAPTIVE_MJC        Fld( 1, 20, AC_MSKB2) 
    #define ADAPTIVE_DB         Fld( 1, 21, AC_MSKB2) 

    #define FLG_ADAPTIVE_COMB           0x00001000
    #define FLG_ADAPTIVE_NR             0x00002000
    #define FLG_ADAPTIVE_INFO           0x00010000    
    #define FLG_ADAPTIVE_LUMA           0x00020000
    #define FLG_ADAPTIVE_BACKLIGHT      0x00040000
    #define FLG_ADAPTIVE_SHARPNESS      0x00080000    
    #define FLG_ADAPTIVE_MJC            0x00100000 
    #define FLG_ADAPTIVE_DB             0x00200000 
    #define FLG_ADAPTIVE_ALL            0x00FFF000

#define vDrvAdaptiveFlgInit()       (vIO32WriteFldAlign(ADAPTIVE_REG, 0xFBF, ADAPTIVE_CONTROL))
#define vDrvAdaptivePSCANFlgInit()  (vIO32WriteFldAlign(ADAPTIVE_PSCAN_REG, 0x1FF, ADAPTIVE_PSCAN))
#define fgIsAdaptiveFlgSet(flag)  (flag & u4IO32Read4B(ADAPTIVE_REG))


//=============================================================
// Addr : 0x0000 - 0x0100
// Name : System Information and Debug
//=============================================================
#define PEUI_00     u4SWReg(0x0010)
    #define PEUI_DEBUG_EN               Fld(1, 0, AC_MSKB0) 
    #define PEUI_IN_CSC_MODE_CHANGE_M   Fld(1, 1, AC_MSKB0) 
    #define PEUI_IN_CSC_MODE_CHANGE_P   Fld(1, 2, AC_MSKB0)
    #define PEUI_XVYCC_UI_M             Fld(1, 3, AC_MSKB0)
    #define PEUI_XVYCC_UI_P             Fld(1, 4, AC_MSKB0)
    #define PEUI_OS_CSC_MODE_CHANGE_M	Fld(1, 5, AC_MSKB0)
    #define PEUI_OS_CSC_MODE_CHANGE_P	Fld(1, 6, AC_MSKB0)	
    #define PEUI_EXTMJC_SWITCH_TO_FRONT Fld(1, 8, AC_MSKB1)
    #define PEUI_INIT_GAMMA_TBL_IN_DRAM Fld(1, 9, AC_MSKB1)
    #define PEUI_EDGE_ENHANCE_EN		Fld(4, 10, AC_MSKB1)
    #define PEUI_CUST_TIMING            Fld(8, 16, AC_FULLB2) //23:16
    #define PEUI_RESET_MAIN             Fld(1,24,AC_MSKB3)//[24:24]
    #define PEUI_RESET_PIP              Fld(1,25,AC_MSKB3)//[25:25]

    #define vDrvSetPEUIResetFlg(bPath, bOnOff) (vIO32WriteFldAlign(PEUI_00, bOnOff, \
                                                ((bPath==SV_VP_MAIN)?PEUI_RESET_MAIN:\
                                                PEUI_RESET_PIP)))
    #define fgGetPEUIResetFlg(bPath) (IO32ReadFldAlign(PEUI_00,\
                                        ((bPath==SV_VP_MAIN)?PEUI_RESET_MAIN:\
                                                             PEUI_RESET_PIP)))

#define FLASH_PQ_00     u4SWReg(0x0020)
    #define FALSH_PQ_LOG_CFG                Fld(8, 0, AC_FULLB0)
    #define FLASH_PQ_READY                  Fld(1, 31, AC_MSKB3)

#define SCE_REG_00     u4SWReg(0x0030)
    #define SCE_FINDCOLOR_FUN_IDX  Fld(8, 16, AC_FULLB2) //23:16
    #define SCE_HUE_INTERVAL       Fld(4, 12, AC_MSKB1) // 15:12
    #define SCE_BRI_INTERVAL       Fld(4, 8, AC_MSKB1) // 11:8
    #define SCE_FINDCOLOR_ONOFF    Fld(1, 5, AC_MSKB0) // 5
    #define SCE_FINDCOLOR_PHASE    Fld(5, 0, AC_MSKB0) // 4:0        

#define MATRIX_00     u4SWReg(0x0050)
    #define SCALERRGB_PIP       Fld(1, 19, AC_MSKB2) // 19
    #define SCALERRGB_MAIN      Fld(1, 18, AC_MSKB2) // 18
    #define IN_CSC_BLACK_LVL_P  Fld(1, 17, AC_MSKB2) // 17
    #define IN_CSC_BLACK_LVL_M  Fld(1, 16, AC_MSKB2) // 16
    #define MATRIX_HD_709       Fld(1, 15, AC_MSKB1) //15
    #define MATRIX_PED_EN       Fld(1, 14, AC_MSKB1) //14
    #define FORCE_LOAD_MATRIX   Fld(1, 13, AC_MSKB1) //13
    #define MATRIX_PQ_ADJ       Fld(1, 12, AC_MSKB1) //12
    #define SCALER444_PIP       Fld(1, 11, AC_MSKB1) //11
    #define SCALER444_MAIN      Fld(1, 10, AC_MSKB1) //10
    #define MATRIXIDX_PIP       Fld(5, 5, AC_MSKW10) //9:5
    #define MATRIXIDX_MAIN      Fld(5, 0, AC_MSKB0) //4:0
    
#define MATRIX_01     u4SWReg(0x0054)
    #define IN_Y_GAIN    Fld(10, 0, AC_MSKW10) // 9:0
    #define IN_Y_OFST    Fld(10, 10, AC_MSKW21) // 19:10
    #define IN_C_GAIN    Fld(10, 20, AC_MSKW32) // 29:20
    #define MATRIX_OS_EXTLVL    Fld(1, 30, AC_MSKB3) // 30

#define MATRIX_02     u4SWReg(0x0058)
    #define COLOR_FILTER_R_GAIN    Fld(10, 0, AC_MSKW10) // 9:0
    #define COLOR_FILTER_G_GAIN    Fld(10, 10, AC_MSKW21) // 19:10
    #define COLOR_FILTER_B_GAIN    Fld(10, 20, AC_MSKW32) // 29:20

#define MATRIX_03     u4SWReg(0x005C)
    #define YCBCR444_HDMI      Fld(1, 0, AC_MSKB0) 
    #define YCBCR444_VGA       Fld(1, 1, AC_MSKB0) 

#define MATRIX_04     u4SWReg(0x0060)
    #define M_GAIN_00      Fld(10, 0, AC_MSKW10) // 9:0
    #define M_GAIN_01      Fld(10, 10, AC_MSKW21) // 19:10
    #define M_GAIN_02      Fld(10, 20, AC_MSKW32) // 29:20
    #define M_GAIN_UPDATE  Fld(1, 30, AC_MSKB3) // 30
#define MATRIX_05     u4SWReg(0x0064)
    #define M_GAIN_10      Fld(10, 0, AC_MSKW10) // 9:0
    #define M_GAIN_11      Fld(10, 10, AC_MSKW21) // 19:10
    #define M_GAIN_12      Fld(10, 20, AC_MSKW32) // 29:20
#define MATRIX_06     u4SWReg(0x0068)
    #define M_GAIN_20      Fld(10, 0, AC_MSKW10) // 9:0
    #define M_GAIN_21      Fld(10, 10, AC_MSKW21) // 19:10
    #define M_GAIN_22      Fld(10, 20, AC_MSKW32) // 29:20

//=============================================================
// Addr : 0x0100 - 0x09FF
// Name : Meter Info
//=============================================================
#define METER_OFFSET_LR     0x80 

#define METER_INFO_00    u4SWReg(0x0100)
    #define METER_APL_VAL         Fld(8, 0, AC_FULLB0) //7:0
    #define METER_ASL_VAL         Fld(8, 8, AC_FULLB1) //15:8
    #define METER_LUMA_MIN        Fld(8, 16, AC_FULLB2) //23:16
    #define METER_LUMA_MAX        Fld(8, 24, AC_FULLB3) //31:24    
#define METER_INFO_01    u4SWReg(0x0104)
    #define METER_LUMA_SUM        Fld(30, 0, AC_MSKDW) //29:0
    #define METER_LUMA_LBOX       Fld(1, 30, AC_MSKB3) //30:30
#define METER_INFO_02    u4SWReg(0x0108)
    #define METER_LUMA_PXL_CNT    Fld(30, 0, AC_MSKDW) //29:0        
#define METER_INFO_03    u4SWReg(0x010C)
    #define METER_HUE_HIST_01     Fld(16, 16, AC_FULLW32) 
    #define METER_HUE_HIST_00     Fld(16,  0, AC_FULLW10) 
#define METER_INFO_04    u4SWReg(0x0110)
    #define METER_HUE_HIST_03     Fld(16, 16, AC_FULLW32) 
    #define METER_HUE_HIST_02     Fld(16,  0, AC_FULLW10) 
#define METER_INFO_05    u4SWReg(0x0114)
    #define METER_HUE_HIST_05     Fld(16, 16, AC_FULLW32) 
    #define METER_HUE_HIST_04     Fld(16,  0, AC_FULLW10) 
#define METER_INFO_06    u4SWReg(0x0118)
    #define METER_HUE_HIST_07     Fld(16, 16, AC_FULLW32) 
    #define METER_HUE_HIST_06     Fld(16,  0, AC_FULLW10) 
#define METER_INFO_07    u4SWReg(0x011C)
    #define METER_SAT_HIST_01     Fld(16, 16, AC_FULLW32) 
    #define METER_SAT_HIST_00     Fld(16,  0, AC_FULLW10) 
#define METER_INFO_08    u4SWReg(0x0120)
    #define METER_SAT_HIST_03     Fld(16, 16, AC_FULLW32) 
    #define METER_SAT_HIST_02     Fld(16,  0, AC_FULLW10) 
#define METER_INFO_09    u4SWReg(0x0124)
    #define METER_SAT_HIST_05     Fld(16, 16, AC_FULLW32) 
    #define METER_SAT_HIST_04     Fld(16,  0, AC_FULLW10) 
#define METER_INFO_0A    u4SWReg(0x0128)
    #define METER_SAT_HIST_07     Fld(16, 16, AC_FULLW32) 
    #define METER_SAT_HIST_06     Fld(16,  0, AC_FULLW10)     
#define METER_INFO_0B    u4SWReg(0x012C)
    #define METER_TWOD_WIN_00     Fld(22,0,AC_MSKDW)
#define METER_INFO_0C    u4SWReg(0x0130)
    #define METER_TWOD_WIN_01     Fld(22,0,AC_MSKDW)
#define METER_INFO_0D    u4SWReg(0x0134)
    #define METER_TWOD_WIN_02     Fld(22,0,AC_MSKDW)
#define METER_INFO_0E    u4SWReg(0x0138)
    #define METER_SAT_SUM     Fld(22,0,AC_MSKDW)
    
#define LUMA_HIST_START     u4SWReg(0x013C)
    #define METER_LUMA_HIST_PCNT1   Fld(16, 16, AC_FULLW32) 
    #define METER_LUMA_HIST_PCNT0   Fld(16, 0, AC_FULLW10) 
#define LUMA_HIST_END       u4SWReg(0x017C)
    #define METER_LUMA_HIST_PCNT33   Fld(16, 0, AC_FULLW10) 
// 3D Right side information
// 0x0180 ~ 0x01FF 
#define METER_DEBUG         u4SWReg(0x0220)
    #define METER_DEBUG_INDEX         		Fld(16, 0,  AC_MSKW10)  //15:0
    #define METER_DEBUG_COUNT         		Fld(8,  16, AC_FULLB2)  //23:16    
    #define METER_LUMA_LBOX                 Fld(1, 30, AC_MSKB3)    //30:30
    #define METER_LUMA_LR                   Fld(1, 31, AC_MSKB3) //31:31

#define SW_MODULE_MASK_EN  u4SWReg(0x07F0)
#define SW_MODULE_TIMER_BASE u4SWReg(0x0800)
//    #define SW_TIMER_FOR_OUT_VSYNC 0x800
//    #define SW_TIMER_FOR_MLOOP0 0x804
//    #define SW_TIMER_FOR_MLOOP1 0x808
//    #define SW_TIMER_FOR_TEST 0x80C

    
//=============================================================
// Addr : 0x1000 - 0x1FFF
// Name : Adaptive Luma
//=============================================================
//Adaptive Luma version 1
#define ADAP_LUMA_00    u4SWReg(0x1044)
    #define SWREG_BWS_BLACK_GAIN            Fld(8, 0, AC_FULLB0) //7:0
    #define SWREG_BWS_BLACK_OFFSET          Fld(8, 8, AC_FULLB1) //15:8
    #define SWREG_BWS_BLACK_RATIO           Fld(8, 16, AC_FULLB2) //23:16
    #define SWREG_BWS_BLACK_LIMIT           Fld(8, 24, AC_FULLB3) //31:24
#define ADAP_LUMA_01    u4SWReg(0x1048)
    #define SWREG_BWS_WHITE_GAIN            Fld(8, 0, AC_FULLB0) //7:0
    #define SWREG_BWS_WHITE_OFFSET          Fld(8, 8, AC_FULLB1) //15:8
    #define SWREG_BWS_WHITE_RATIO           Fld(8, 16, AC_FULLB2) //23:16
    #define SWREG_BWS_WHITE_LIMIT           Fld(8, 24, AC_FULLB3) //31:24
#define ADAP_LUMA_02    u4SWReg(0x104C)
    #define SWREG_DYNBS_PXL_THD1            Fld(8, 0, AC_FULLB0) //7:0
    #define SWREG_DYNBS_PXL_THD2            Fld(8, 8, AC_FULLB1) //15:8
    #define SWREG_DYNBS_DIST_THDL           Fld(8, 16, AC_FULLB2) //23:16
    #define SWREG_DYNBS_DIST_THDH           Fld(8, 24, AC_FULLB3) //31:24
#define ADAP_LUMA_03    u4SWReg(0x1050)
    #define SWREG_AL_APL                    Fld(8, 0, AC_FULLB0) //7:0
    #define SWREG_AL_GAIN                   Fld(8, 8, AC_FULLB1) //15:8
    #define SWREG_AL_OFFSET                 Fld(8, 16, AC_FULLB2) //23:16
    #define SWREG_AL_LIMIT                  Fld(8, 24, AC_FULLB3) //31:24
#define ADAP_LUMA_04    u4SWReg(0x1054)
    #define SWREG_BWS_ON_1_OFF_0            Fld(1,  0,  AC_MSKB0)   // 0
    #define SWREG_BWS_BLACK_ON_1_OFF_0      Fld(1,  1,  AC_MSKB0)   // 1
    #define SWREG_BWS_WHITE_ON_1_OFF_0      Fld(1,  2,  AC_MSKB0)   // 2
    #define SWREG_DYNBS_ONOFF               Fld(1,  3,  AC_MSKB0)   // 3
    #define SWREG_BWS_BLACK_LEVEL           Fld(4,  5,  AC_MSKW10)  //8:5
    #define SWREG_BWS_WHITE_LEVEL           Fld(4,  9,  AC_MSKB1)   //12:9
    #define SWREG_DYNBS_RANGE               Fld(4,  13, AC_MSKW21)  //16:13
    #define SWREG_BS_STRENTH_CTRL           Fld(1,  17, AC_MSKB2)   //17
    #define SWREG_AL_ON_1_OFF_0             Fld(1,  18, AC_MSKB2)   //18
    #define SWREG_COLOR_BS_ON_1_OFF_0 Fld(1, 19, AC_MSKB2) //19    
#define ADAP_LUMA_05    u4SWReg(0x1058)
    #define SWREG_DYNBS_LMT_BTM             Fld(8, 0, AC_FULLB0) //7:0
    #define SWREG_DYNBS_LMT_TOP             Fld(8, 8, AC_FULLB1) //15:8
    #define SWREG_DYNBS_GAIN_BTM            Fld(8, 16, AC_FULLB2) //23:16
    #define SWREG_DYNBS_GAIN_TOP            Fld(8, 24, AC_FULLB3) //31:24
#define ADAP_LUMA_06    u4SWReg(0x105C)
    #define SWREG_DYNBS_OFST_BTM            Fld(8, 0, AC_FULLB0) //7:0
    #define SWREG_DYNBS_OFST_TOP            Fld(8, 8, AC_FULLB1) //15:8
    #define SWREG_AL_DIST                   Fld(8, 16, AC_FULLB2) //23:16
    #define SWREG_AL_DRK_PXL                Fld(8, 24, AC_FULLB3) //31:24 
//Common register
#define ADAP_LUMA_08    u4SWReg(0x1064)
    #define ADL_DEBUG_INDEX         		Fld(16, 0,  AC_MSKW10)  //15:0
    #define ADL_DEBUG_COUNT         		Fld(8,  16, AC_FULLB2)  //23:16    
    #define Y_GAMMA_MID_SEL                 Fld(4,  24, AC_MSKB3)   //27:24    
    #define Y_GAMMA_MID_SEL_SKIN            Fld(4,  28, AC_MSKB3)   //31:28    
#define ADAP_LUMA_09    u4SWReg(0x1068)
    #define ADL_UI_YGAMMA                   Fld(8, 0, AC_FULLB0) //7:0
	#define ADL_UPDATE_COUNT                Fld(8, 8, AC_FULLB1) //15:8     
    #define ADL_ADAP_YGAMMA_GAIN            Fld(8,  16, AC_FULLB2)   //23:16	
    #define ADL_ADAP_YGAMMA_TH              Fld(8,  24, AC_FULLB3)   //31:24
#define ADAP_LUMA_0A    u4SWReg(0x106C)
	#define ADL_SECOND_CURVE_WEIGHT			Fld(8,  0,  AC_FULLB0)  //7:0
	#define ADL_SECOND_LUMA_CONF_UP_SLOPE	Fld(4,  8, AC_MSKB1)   //11:8
	#define ADL_SECOND_LUMA_CONF_DOWN_SLOPE	Fld(4,  12, AC_MSKB1)   //15:12	
	#define ADL_SECOND_LUMA_EN				Fld(1,  24, AC_MSKB3)   //24
    #define ADL_SECOND_LUMA_BLEND_METHOD    Fld(2,  25, AC_MSKB3)   //26:25    
    #define ADL_SECOND_LUMA_BLEND_ADAP      Fld(1,  27, AC_MSKB3)   //27
#define ADAP_LUMA_0C    u4SWReg(0x1074)
    #define Y_GAMMA_DECAY_L                 Fld(8,  0,  AC_FULLB0)   //7:0   
    #define Y_GAMMA_DECAY_H                 Fld(8,  8,  AC_FULLB1)   //15:8  
    #define Y_BRIGHT                        Fld(8,  16, AC_FULLB2)   //23:16
    #define Y_GAMMA_SEL                     Fld(8,  24, AC_FULLB3)   //31:24    
#define ADAP_LUMA_0D    u4SWReg(0x1078)
    #define SWREG_BWS_KEEP_MID_X            Fld(8, 0, AC_FULLB0) //7:0
    #define SWREG_BWS_KEEP_MID_Y            Fld(10, 8, AC_MSKW21) //17:8
    #define SWREG_BWS_KEEP_MID_ON_OFF       Fld(1, 18, AC_MSKB2) // 18
    #define ADL_IIR_STRENTH                 Fld(8,  24, AC_FULLB3)  //31:24  

//Adaptive Luma version 2
#define ADAP_LUMA2_00    u4SWReg(0x1080)
    #define ADL_MID_AC_GAIN    				Fld(8,  0,  AC_FULLB0)  //7:0
	#define ADL_MID_DC_GAIN  				Fld(8,  8,  AC_FULLB1)  //15:8
    #define ADAP_BITPLUS_EN                 Fld(1,  29, AC_MSKB3)   //30:30
    #define ADL_3D_LUMA_EN                  Fld(1,  30, AC_MSKB3)   //30:30
	#define ADL_VERSION 					Fld(1,  31, AC_MSKB3)   //31:31
#define ADAP_LUMA2_01    u4SWReg(0x1084)
	#define ADL_BWS_BLACK_DC_GAIN_RATIO		Fld(8,  0,  AC_FULLB0)  //7:0
	#define ADL_BWS_BLACK_AC_GAIN_RATIO		Fld(8,  8,  AC_FULLB1)  //15:8
	#define ADL_BWS_BLACK_RATIO 			Fld(8,  16, AC_FULLB2)  //23:16
	#define ADL_BWS_BLACK_LEVEL 			Fld(4,  24, AC_MSKB3)   //27:24
#define ADAP_LUMA2_02    u4SWReg(0x1088)
	#define ADL_BWS_BLACK_OFT_TH_L 			Fld(8,  0,  AC_FULLB0)  //7:0
	#define ADL_BWS_BLACK_OFT_TH_H			Fld(8,  8,  AC_FULLB1)  //15:8
	#define ADL_BWS_BLACK_OFFSET_L			Fld(8,  16, AC_FULLB2)  //23:16
	#define ADL_BWS_BLACK_OFFSET_H			Fld(8,  24, AC_FULLB3)  //31:24
#define ADAP_LUMA2_03    u4SWReg(0x108C)
	#define ADL_BWS_WHITE_DC_GAIN_RATIO		Fld(8,  0,  AC_FULLB0)  //7:0
	#define ADL_BWS_WHITE_AC_GAIN_RATIO		Fld(8,  8,  AC_FULLB1)  //15:8
	#define ADL_BWS_WHITE_RATIO 			Fld(8,  16, AC_FULLB2)  //23:16
	#define ADL_BWS_WHITE_LEVEL 			Fld(4,  24, AC_MSKB3)   //27:24
#define ADAP_LUMA2_04    u4SWReg(0x1090)
	#define ADL_BWS_WHITE_OFT_TH_L			Fld(8,  0,  AC_FULLB0)  //7:0
	#define ADL_BWS_WHITE_OFT_TH_H			Fld(8,  8,  AC_FULLB1)  //15:8
	#define ADL_BWS_WHITE_OFFSET_L			Fld(8,  16, AC_FULLB2)  //23:16
	#define ADL_BWS_WHITE_OFFSET_H			Fld(8,  24, AC_FULLB3)  //31:24
#define ADAP_LUMA2_05    u4SWReg(0x1094)
	#define ADL_WEIGHT1 					Fld(8,  0,  AC_FULLB0)  //7:0
	#define ADL_WEIGHT2 					Fld(8,  8,  AC_FULLB1)  //15:8
	#define ADL_WEIGHT3 					Fld(8,  16, AC_FULLB2)  //23:16
	#define ADL_ON_1_OFF_0 					Fld(1,  24, AC_MSKB3)   //24
	#define ADL_BWS_BLACK_ON_1_OFF_0 		Fld(1,  25, AC_MSKB3)   //25
	#define ADL_BWS_WHITE_ON_1_OFF_0 		Fld(1,  26, AC_MSKB3)   //26
	#define ADL_BWS_BLACK_STRONG_OFFSET_EN	Fld(1,  27, AC_MSKB3)   //27
	#define ADL_BWS_WHITE_STRONG_OFFSET_EN	Fld(1,  28, AC_MSKB3)   //28
	#define ADL_BWS_BLACK_ADPT_LMT_EN		Fld(1,  29, AC_MSKB3)   //29
	#define ADL_BWS_WHITE_ADPT_LMT_EN		Fld(1,  30, AC_MSKB3)   //30	
	#define ADL_FORCE_SCENE_CHANGE			Fld(1,  31, AC_MSKB3)   //31
#define ADAP_LUMA2_06    u4SWReg(0x1098)
	#define ADL_BLACK_PERCENTAGE			Fld(8,  0,  AC_FULLB0)  //7:0
	#define ADL_BLACK_INDEX 				Fld(8,  8,  AC_FULLB1)  //15:8
	#define ADL_WHITE_PERCENTAGE			Fld(8,  16, AC_FULLB2)  //23:16
	#define ADL_WHITE_INDEX 				Fld(8,  24, AC_FULLB3)  //31:24
#define ADAP_LUMA2_07    u4SWReg(0x109C)
	#define ADL_INCPAT_CNT_TH				Fld(4,  0,  AC_MSKB0)   //3:0
	#define ADL_APL_DIFF_TH 				Fld(4,  4,  AC_MSKB0)   //7:4
	#define ADL_APL_DBG_COUNT 				Fld(8,  8,  AC_FULLB1)  //15:8
	#define ADL_CG_PAT_PROT_EN 				Fld(1,  16, AC_MSKB2)   //16:16
    #define ADL_CG_PAT_PROT_GAIN            Fld(3,  17, AC_MSKB2)   //19:17
    #define ADL_CG_PAT_PROT_TH              Fld(8,  24, AC_FULLB3)  //31:24

// FleshTone ADL 
#define ADAP_LUMA_0E    u4SWReg(0x10A0)
    #define BS_COLOR_KR Fld(8, 0, AC_FULLB0) //7:0
    #define BS_COLOR_KG Fld(8, 8, AC_FULLB1) //15:8
    #define BS_COLOR_KB Fld(8, 16, AC_FULLB2) //23:16
#define ADAP_LUMA_0F    u4SWReg(0x10A4)
    #define BS_ASL_REMAP_R_LO Fld(8, 0, AC_FULLB0) //7:0
    #define BS_ASL_REMAP_R_HI Fld(8, 8, AC_FULLB1) //15:8
    #define BS_ASL_REMAP_G_LO Fld(8, 16, AC_FULLB2) //23:16
    #define BS_ASL_REMAP_G_HI Fld(8, 24, AC_FULLB3) //31:24     
#define ADAP_LUMA_10    u4SWReg(0x10A8)
    #define BS_ASL_REMAP_B_LO Fld(8, 0, AC_FULLB0) //7:0
    #define BS_ASL_REMAP_B_HI Fld(8, 8, AC_FULLB1) //15:8
    #define BS_ASL_REMAP_W_DECAY Fld(8, 16, AC_FULLB2) //15:8
    
#define ADAP_LUMA_11    u4SWReg(0x10AC)
    #define ASL_VALUE Fld(16, 16, AC_FULLW32) //31:16  
    #define BS_COLOR_DBG_05 Fld(1, 5, AC_MSKB0) //5    
    #define BS_COLOR_DBG_04 Fld(1, 4, AC_MSKB0) // 4
    #define BS_COLOR_DBG_03 Fld(1, 3, AC_MSKB0) // 3
    #define BS_COLOR_DBG_02 Fld(1, 2, AC_MSKB0) // 2
    #define BS_COLOR_DBG_01 Fld(1, 1, AC_MSKB0) // 1
    #define BS_COLOR_DBG_00 Fld(1, 0, AC_MSKB0) //0
// OPC
#define OPC_01  u4SWReg(0x1100)
    #define OPC_NORM_HIST_SUM Fld(16, 0, AC_FULLW10) //15:0
    #define OPC_MAX_CONTENT Fld(8, 16, AC_FULLB2) //23:16
    #define OPC_ON_OFF Fld(1, 24, AC_MSKB3) //24
    #define OPC_CONTROL_ON_OFF Fld(1, 25, AC_MSKB3) //25    
#define OPC_02  u4SWReg(0x1104)
    #define OPC_NORM_HIST_00 Fld(16, 0, AC_FULLW10) //15:0
    #define OPC_NORM_HIST_01 Fld(16, 16, AC_FULLW32) //31:16    
#define OPC_03  u4SWReg(0x1108)
    #define OPC_NORM_HIST_02 Fld(16, 0, AC_FULLW10) //15:0
    #define OPC_NORM_HIST_03 Fld(16, 16, AC_FULLW32) //31:16  
#define OPC_04  u4SWReg(0x110C)
    #define OPC_NORM_HIST_04 Fld(16, 0, AC_FULLW10) //15:0
    #define OPC_NORM_HIST_05 Fld(16, 16, AC_FULLW32) //31:16  
#define OPC_05  u4SWReg(0x1110)
    #define OPC_NORM_HIST_06 Fld(16, 0, AC_FULLW10) //15:0
    #define OPC_NORM_HIST_07 Fld(16, 16, AC_FULLW32) //31:16  
#define OPC_06  u4SWReg(0x1114)
    #define OPC_NORM_HIST_08 Fld(16, 0, AC_FULLW10) //15:0
    #define OPC_NORM_HIST_09 Fld(16, 16, AC_FULLW32) //31:16  
#define OPC_07  u4SWReg(0x1118)
    #define OPC_NORM_HIST_0A Fld(16, 0, AC_FULLW10) //15:0
    #define OPC_NORM_HIST_0B Fld(16, 16, AC_FULLW32) //31:16  
#define OPC_08  u4SWReg(0x111C)
    #define OPC_NORM_HIST_0C Fld(16, 0, AC_FULLW10) //15:0
    #define OPC_NORM_HIST_0D Fld(16, 16, AC_FULLW32) //31:16  
#define OPC_09  u4SWReg(0x1120)
    #define OPC_NORM_HIST_0E Fld(16, 0, AC_FULLW10) //15:0
    #define OPC_NORM_HIST_0F Fld(16, 16, AC_FULLW32) //31:16  
#define OPC_0A  u4SWReg(0x1124)
    #define OPC_MAX_CONTENT_TH Fld(16, 0, AC_FULLW10) //15:0
    #define OPC_SEARCHED_PWM Fld(16, 16, AC_FULLW32) //31:16  
#define OPC_0B  u4SWReg(0x1128)
    #define OPC_SW_G2 Fld(8, 24, AC_FULLB3) //31:24
    #define OPC_SW_G1 Fld(8, 16, AC_FULLB2) //23:16
    #define OPC_SW_P2 Fld(8, 8, AC_FULLB1) //15:8
    #define OPC_SW_P1 Fld(8, 0, AC_FULLB0) //7:0
#define OPC_0C  u4SWReg(0x112C)
    #define OPC_SW_PWM Fld(23  ,16 ,AC_MSKB2) //23:16    
    #define OPC_SW_G_SLOPE Fld(11, 0, AC_MSKW10) //10:0
#define OPC_0D  u4SWReg(0x1130)
    #define TOTAL_DIFF_THD Fld(32, 0, AC_FULLDW) //31:0
#define OPC_0E  u4SWReg(0x1134)
    #define SCENE_CHANGE_MODE Fld(8, 16, AC_FULLB2) //23:16
    #define RUN_QUE_SIZE Fld(8, 8, AC_FULLB1) //15:8
    #define PWM_TABLE_IDX Fld(8, 0, AC_FULLB0) //7:0
    
#ifdef CUST_LCCT_DRV
//  Bright Plus
#define BRIGHT_PLUS_00  u4SWReg(0x1200)
    #define BRIGHT_PLUS_OUTPUT_GAIN Fld(8, 8, AC_FULLB1) //15:8
    #define BRIGHT_PLUS_CTRL Fld(1, 0, AC_MSKB0) //0  
#define BRIGHT_PLUS_01  u4SWReg(0x1204)
    #define BRIGHT_PLUS_G2 Fld(8, 24, AC_FULLB3) //31:24
    #define BRIGHT_PLUS_G1 Fld(8, 16, AC_FULLB2) //23:16 
    #define BRIGHT_PLUS_P2 Fld(8, 8, AC_FULLB1) //15:8
    #define BRIGHT_PLUS_P1 Fld(8, 0, AC_FULLB0) //7:0  
#define BRIGHT_PLUS_02  u4SWReg(0x1208)
	#define BRIGHT_PLUS_STEPDNCNT Fld(8, 24, AC_FULLB3) //31:24
    #define BRIGHT_PLUS_STEPUPCNT Fld(8, 16, AC_FULLB2) //23:16 
    #define BRIGHT_PLUS_STEPDN Fld(8, 8, AC_FULLB1) //15:8
    #define BRIGHT_PLUS_STEPUP Fld(8, 0, AC_FULLB0) //7:0  
#endif

#define ADAP_LUMA2_08    u4SWReg(0x1138)    // MJC moving box patch of mt5396 changelsit:903459 ,checked to cd lee ,it also do in Cobra
    #define ADL_BLACK_PERCENTAGE_2			Fld(8,  0,  AC_FULLB0)  //7:0
    #define ADL_WHITE_PERCENTAGE_2			Fld(8,  16, AC_FULLB2)  //23:16
    
// SRAM Stress Test
#define PQSLT_01     u4SWReg(0x1300)
    #define PQSLT_EN Fld(1, 0, AC_MSKB0) //0
    #define PQSLT_DONE Fld(1, 1, AC_MSKB0) // 1
    #define PQSLT_RESULT Fld(1, 2, AC_MSKB0) // 2     
    #define PQSLT_MODULE_SEL Fld(8, 8, AC_FULLB1) //15:8
    #define PQSLT_PATTERN_SEL Fld(8, 16, AC_FULLB2) //23:16

#define PQSLT_02     u4SWReg(0x1304)    
    #define PQSLT_ROUND Fld(32, 0, AC_FULLDW) //31:0

// Local Dimming
#define ADAP_LCDIM_00  u4SWReg(0x1400)
    #define ADAP_LD_CUR_CTRL Fld(1, 0, AC_MSKB0) //0
    #define ADAP_LD_SENS_CTRL Fld(1, 1, AC_MSKB0) // 1
    #define ADAP_LD_CURVE_FREEZE Fld(1, 2, AC_MSKB0) // 2        
    #define ADAP_LCDIM_MOTION_PENALTY_EN Fld(1, 4, AC_MSKB0) // 4
    #define ADAP_LCDIM_BLACK_PENALTY_EN Fld(1, 9, AC_MSKB1) //9
    #define ADAP_LCDIM_FORCE_EN Fld(1, 10, AC_MSKB1) //10

#define ADAP_LCDIM_01  u4SWReg(0x1404)
    #define LCDIM_QUE_SIZE_AVE Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_QUE_SIZE_BLACK Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_BLACK_THD Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_APL_THD Fld(8, 0, AC_FULLB1) //7:0   

#define ADAP_LCDIM_02  u4SWReg(0x1408)    
    #define LCDIM_NR_MOTION_THD Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_NR_MOTION_CONF Fld(8, 0, AC_FULLB0) //7:0

#define ADAP_LCDIM_03  u4SWReg(0x140C)
    #define LCDIM_AVE_W_MIN Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_AVE_W_MAX Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_CUR_W_MIN Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_CUR_W_MAX Fld(8, 0, AC_FULLB0) //7:0

#define ADAP_LCDIM_04  u4SWReg(0x1410)   
    #define LCDIM_BLACK_CONFIDENCE Fld(8, 0, AC_FULLB0) //7:0
    #define LCDIM_MOTION_PENALTY_STEP Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_BLACK_PENALTY_STEP Fld(8, 24, AC_FULLB3) //31:24     

#define ADAP_LCDIM_SPI_SEP u4SWReg(0x1414)

#define ADAP_LCDIM_SPI u4SWReg(0x1418) 	//mtk70763 2010-8-12
    #define SPI_AUTO_VSYNC_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define LCDIM_DUTY Fld(8, 8, AC_FULLB1) //15:8

#define SPI_SCANNING_DELAY_0 u4SWReg(0x1420)
    #define SPI_SCANNING_DELAY_00 Fld(16, 0, AC_FULLW10) //15:0
    #define SPI_SCANNING_DELAY_01 Fld(16, 16, AC_FULLW32) //31:16  
#define SPI_SCANNING_DELAY_1 u4SWReg(0x1424)
    #define SPI_SCANNING_DELAY_02 Fld(16, 0, AC_FULLW10) //15:0
    #define SPI_SCANNING_DELAY_03 Fld(16, 16, AC_FULLW32) //31:16  
#define SPI_SCANNING_DELAY_2 u4SWReg(0x1428)
    #define SPI_SCANNING_DELAY_04 Fld(16, 0, AC_FULLW10) //15:0
    
#define LCDIM_SW_CTRL_PT_DFT_0  u4SWReg(0x1500)   
    #define LCDIM_SW_CTRL_PT_DFT_03 Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_CTRL_PT_DFT_02 Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_CTRL_PT_DFT_01 Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_CTRL_PT_DFT_00 Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_CTRL_PT_DFT_1  u4SWReg(0x1504)   
    #define LCDIM_SW_CTRL_PT_DFT_07 Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_CTRL_PT_DFT_06 Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_CTRL_PT_DFT_05 Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_CTRL_PT_DFT_04 Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_CTRL_PT_DFT_2  u4SWReg(0x1508)   
    #define LCDIM_SW_CTRL_PT_DFT_11 Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_CTRL_PT_DFT_10 Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_CTRL_PT_DFT_09 Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_CTRL_PT_DFT_08 Fld(8, 0, AC_FULLB0) //7:0    
#define LCDIM_SW_CTRL_PT_DFT_3  u4SWReg(0x150C)   
    #define LCDIM_SW_CTRL_PT_DFT_15 Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_CTRL_PT_DFT_14 Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_CTRL_PT_DFT_13 Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_CTRL_PT_DFT_12 Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_CTRL_PT_DFT_4  u4SWReg(0x1510)
    #define LCDIM_SW_CTRL_PT_DFT_19 Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_CTRL_PT_DFT_18 Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_CTRL_PT_DFT_17 Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_CTRL_PT_DFT_16 Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_CTRL_PT_DFT_5  u4SWReg(0x1514)
    #define LCDIM_SW_CTRL_PT_DFT_23 Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_CTRL_PT_DFT_22 Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_CTRL_PT_DFT_21 Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_CTRL_PT_DFT_20 Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_CTRL_PT_DFT_6  u4SWReg(0x1518)
    #define LCDIM_SW_CTRL_PT_DFT_27 Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_CTRL_PT_DFT_26 Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_CTRL_PT_DFT_25 Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_CTRL_PT_DFT_24 Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_CTRL_PT_DFT_7  u4SWReg(0x151C)
    #define LCDIM_SW_CTRL_PT_DFT_31 Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_CTRL_PT_DFT_30 Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_CTRL_PT_DFT_29 Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_CTRL_PT_DFT_28 Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_CTRL_PT_DFT_8  u4SWReg(0x1520)
    #define LCDIM_SW_CTRL_PT_DFT_32 Fld(8, 0, AC_FULLB0) //7:0

#define LCDIM_SW_CTRL_PT_STR_0  u4SWReg(0x1528)   
    #define LCDIM_SW_CTRL_PT_STR_03 Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_CTRL_PT_STR_02 Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_CTRL_PT_STR_01 Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_CTRL_PT_STR_00 Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_CTRL_PT_STR_1  u4SWReg(0x152C)   
    #define LCDIM_SW_CTRL_PT_STR_07 Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_CTRL_PT_STR_06 Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_CTRL_PT_STR_05 Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_CTRL_PT_STR_04 Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_CTRL_PT_STR_2  u4SWReg(0x1530)   
    #define LCDIM_SW_CTRL_PT_STR_11 Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_CTRL_PT_STR_10 Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_CTRL_PT_STR_09 Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_CTRL_PT_STR_08 Fld(8, 0, AC_FULLB0) //7:0        
#define LCDIM_SW_CTRL_PT_STR_3  u4SWReg(0x1534)   
    #define LCDIM_SW_CTRL_PT_STR_15 Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_CTRL_PT_STR_14 Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_CTRL_PT_STR_13 Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_CTRL_PT_STR_12 Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_CTRL_PT_STR_4  u4SWReg(0x1538)
    #define LCDIM_SW_CTRL_PT_STR_19 Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_CTRL_PT_STR_18 Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_CTRL_PT_STR_17 Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_CTRL_PT_STR_16 Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_CTRL_PT_STR_5  u4SWReg(0x153C)
    #define LCDIM_SW_CTRL_PT_STR_23 Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_CTRL_PT_STR_22 Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_CTRL_PT_STR_21 Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_CTRL_PT_STR_20 Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_CTRL_PT_STR_6  u4SWReg(0x1540)
    #define LCDIM_SW_CTRL_PT_STR_27 Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_CTRL_PT_STR_26 Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_CTRL_PT_STR_25 Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_CTRL_PT_STR_24 Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_CTRL_PT_STR_7  u4SWReg(0x1544)
    #define LCDIM_SW_CTRL_PT_STR_31 Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_CTRL_PT_STR_30 Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_CTRL_PT_STR_29 Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_CTRL_PT_STR_28 Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_CTRL_PT_STR_8  u4SWReg(0x1548)
    #define LCDIM_SW_CTRL_PT_STR_32 Fld(8, 0, AC_FULLB0) //7:0

#define LCDIM_DEMO_SETTING_0  u4SWReg(0x1550)
    #define LCDIM_DEMO_EN Fld(1, 0, AC_MSKB0) //0
    #define LCDIM_DEBUG_EN Fld(1, 1, AC_MSKB0) //1    
    #define LCDIM_SCG_DEBUG_EN Fld(1, 2, AC_MSKB0) //1    
    #define LCDIM_BLKDIM_SCG_DEBUG_EN Fld(1, 3, AC_MSKB0) //1        
    #define LCDIM_FORCE_SCG Fld(1, 4, AC_MSKB0) //1            
    #define LCDIM_MAXINFO_SCG_DEBUG_EN Fld(1, 5, AC_MSKB0) //1        
    #define LCDIM_MAXINFO_SCG_EN Fld(1, 6, AC_MSKB0) //1        
    #define LCDIM_MAXHIST_SCG_DEBUG_EN Fld(1, 7, AC_MSKB0) //1        
    #define LCDIM_MAXHIST_SCG_EN Fld(1, 8, AC_MSKB1) //1        
    #define LCDIM_SPEED_CTRL_SCG_EN         Fld(1, 9, AC_MSKB1) //1        
    #define LCDIM_UPDATE_DS_CURVE           Fld(1, 10, AC_MSKB1) //1        
    #define LCDIM_GET_DS_CURVE              Fld(1, 11, AC_MSKB1) //1        
    
#define LCDIM_DEBUG_DIM_0_3  u4SWReg(0x1554)
    #define LCDIM_DEBUG_DIM_3   Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_DEBUG_DIM_2   Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_DEBUG_DIM_1   Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_DEBUG_DIM_0   Fld(8, 0, AC_FULLB0) //7:0
    
#define LCDIM_DEBUG_DIM_4_7  u4SWReg(0x1558)
    #define LCDIM_DEBUG_DIM_7   Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_DEBUG_DIM_6   Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_DEBUG_DIM_5   Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_DEBUG_DIM_4   Fld(8, 0, AC_FULLB0) //7:0
    
#define LCDIM_DEBUG_DIM_8_11  u4SWReg(0x155C)
    #define LCDIM_DEBUG_DIM_11   Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_DEBUG_DIM_10   Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_DEBUG_DIM_9   Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_DEBUG_DIM_8   Fld(8, 0, AC_FULLB0) //7:0
    
#define LCDIM_DEBUG_DIM_12_15  u4SWReg(0x1560)
    #define LCDIM_DEBUG_DIM_15   Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_DEBUG_DIM_14   Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_DEBUG_DIM_13   Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_DEBUG_DIM_12   Fld(8, 0, AC_FULLB0) //7:0
    
#define LCDIM_SCENE_CHANGE0  u4SWReg(0x1564)
    #define LCDIM_SCG_RGB_MAX_DIFF_THD Fld(16, 0, AC_FULLW10) //15:0
    #define LCDIM_SCG_RGB_TOTAL_DIFF_THD Fld(16, 16, AC_FULLW32) //31:16  
#define LCDIM_SCENE_CHANGE1  u4SWReg(0x1568)
    #define LCDIM_SCG_RGB_AVG_DIFF_THD Fld(16, 0, AC_FULLW10) //15:0
    #define LCDIM_SCG_RGB_AVG_DIFF_THD2     Fld(16, 16, AC_FULLW32) //31:16
#define LCDIM_SCENE_CHANGE2  u4SWReg(0x156C)
    #define LCDIM_SCG_RGB_MAX_DIFF_THD2     Fld(16, 0, AC_FULLW10) //15:0
    #define LCDIM_SCG_RGB_TOTAL_DIFF_THD2   Fld(16, 16, AC_FULLW32) //31:16      
#define LCDIM_BLKDIM_SCG  u4SWReg(0x1570)
    #define LCDIM_SCG_BLKDIM_DIFF_THD Fld(16, 0, AC_FULLW10) //15:0
    #define LCDIM_SCG_BLKDIM_TOTAL_DIFF_THD Fld(16, 16, AC_FULLW32) //31:16  
#define LCDIM_MAXINFO_SCG  u4SWReg(0x1574)
    #define LCDIM_SCG_MAXHIST_DIFF_THD      Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SCG_MAXHIST_TOTAL_DIFF_THD Fld(8, 24, AC_FULLB3) //31:24  

#define ADAP_LCDIM_SPEED_CTRL1  u4SWReg(0x1584)  

#define ADAP_LCDIM_SPEED_CTRL2  u4SWReg(0x1588)
    #define LCDIM_SPCTRL_MAXRGB_STEP_HIGH   Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SPCTRL_MAXRGB_STEP_MID_UP Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SPCTRL_MAXRGB_STEP_MID_DN Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SPCTRL_MAXRGB_STEP_LOW    Fld(8, 0, AC_FULLB0) //7:0

#define ADAP_LCDIM_SPEED_CTRL3  u4SWReg(0x158C)
    #define LCDIM_SPCTRL_MOTION_PENALTY     Fld(8, 24, AC_FULLB3) //31:24

    #define LCDIM_SPCTRL_STATIC_STEP        Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SPCTRL_CONFIDENCE         Fld(8, 0, AC_FULLB0) //7:0

#define ADAP_LCDIM_SPEED_CTRL4  u4SWReg(0x1590)
    #define LCDIM_SPCTRL_CONF_TH3           Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SPCTRL_CONF_TH2           Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SPCTRL_CONF_TH1           Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SPCTRL_CONF_TH0           Fld(8, 0, AC_FULLB0) //7:0

#define ADAP_LCDIM_SPEED_CTRL5  u4SWReg(0x1594)
    #define LCDIM_SPCTRL_MOTION_STR         Fld(8, 24, AC_FULLB3) //31:24

#define ADAP_LCDIM_SPEED_CTRL6  u4SWReg(0x1598)
    #define LCDIM_MAX_UP_DIM_STEP_HIGH      Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_MAX_UP_DIM_STEP_MID_UP    Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_MAX_UP_DIM_STEP_MID_DN    Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_MAX_UP_DIM_STEP_LOW       Fld(8, 0, AC_FULLB0) //7:0

#define ADAP_LCDIM_SPEED_CTRL7  u4SWReg(0x159C)
    #define LCDIM_MAX_DN_DIM_STEP_HIGH      Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_MAX_DN_DIM_STEP_MID_UP    Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_MAX_DN_DIM_STEP_MID_DN    Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_MAX_DN_DIM_STEP_LOW       Fld(8, 0, AC_FULLB0) //7:0 

#define ADAP_LCDIM_SPEED_CTRL8  u4SWReg(0x15A0)
    #define LCDIM_DY_DIM_SLOPE_HIGH      Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_DY_DIM_SLOPE_MID_UP    Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_DY_DIM_SLOPE_MID_DN    Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_DY_DIM_SLOPE_LOW       Fld(8, 0, AC_FULLB0) //7:0        

#define LCDIM_SW_DS_CTRL_PT_0_3  u4SWReg(0x15C0)   
    #define LCDIM_SW_DS_CTRL_PT_03         Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_DS_CTRL_PT_02         Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_DS_CTRL_PT_01         Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_DS_CTRL_PT_00         Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_DS_CTRL_PT_4_7  u4SWReg(0x15C4)
    #define LCDIM_SW_DS_CTRL_PT_07         Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_DS_CTRL_PT_06         Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_DS_CTRL_PT_05         Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_DS_CTRL_PT_04         Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_DS_CTRL_PT_8_11  u4SWReg(0x15C8)   
    #define LCDIM_SW_DS_CTRL_PT_11         Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_DS_CTRL_PT_10         Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_DS_CTRL_PT_09         Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_DS_CTRL_PT_08         Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_DS_CTRL_PT_12_15  u4SWReg(0x15CC)
    #define LCDIM_SW_DS_CTRL_PT_15         Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_DS_CTRL_PT_14         Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_DS_CTRL_PT_13         Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_DS_CTRL_PT_12         Fld(8, 0, AC_FULLB0) //7:0    
#define LCDIM_SW_DS_CTRL_PT_16_19  u4SWReg(0x15D0)
    #define LCDIM_SW_DS_CTRL_PT_19         Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_DS_CTRL_PT_18         Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_DS_CTRL_PT_17         Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_DS_CTRL_PT_16         Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_DS_CTRL_PT_20_23  u4SWReg(0x15D4)
    #define LCDIM_SW_DS_CTRL_PT_23         Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_DS_CTRL_PT_22         Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_DS_CTRL_PT_21         Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_DS_CTRL_PT_20         Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_DS_CTRL_PT_24_27  u4SWReg(0x15D8)   
    #define LCDIM_SW_DS_CTRL_PT_27         Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_DS_CTRL_PT_26         Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_DS_CTRL_PT_25         Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_DS_CTRL_PT_24         Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_DS_CTRL_PT_28_31  u4SWReg(0x15DC)   
    #define LCDIM_SW_DS_CTRL_PT_31         Fld(8, 24, AC_FULLB3) //31:24
    #define LCDIM_SW_DS_CTRL_PT_30         Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_SW_DS_CTRL_PT_29         Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_SW_DS_CTRL_PT_28         Fld(8, 0, AC_FULLB0) //7:0
#define LCDIM_SW_DS_CTRL_PT_32_32  u4SWReg(0x15E0)
    #define LCDIM_SW_DS_CTRL_PT_32         Fld(8, 0, AC_FULLB0) //7:0

#define LCDIM_00     u4SWReg(0x1600)
    #define W_MIN_MUTE         Fld(8, 0, AC_FULLB0) //7:0
    #define MUTE_DELAY         Fld(8, 0, AC_FULLB1) //15:8

#define LCDIM_01    u4SWReg(0x1604)
    #define LCDIM_FILTER_TYPE               Fld(8, 0, AC_FULLB0) //7:0
    #define LCDIM_FILTER_INC_ONLY           Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_FILTER_C0                 Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_FILTER_C1                 Fld(8, 24, AC_FULLB3) //31:24    
    
#define LCDIM_02    u4SWReg(0x1608)
    #define LCDIM_FILTER_LOG                Fld(8, 0, AC_FULLB0) //7:0
    #define LCDIM_SPI_INV                   Fld(8, 8, AC_FULLB1) //15:8
    #define LCDIM_UPDATE_VSYNC              Fld(8, 16, AC_FULLB2) //23:16
    #define LCDIM_VSYNC_FREQ                Fld(8, 24, AC_FULLB3) //31:24    

//=============================================================
// Addr : 0x2000 - 0x2FFF
// Name : Color and Sharpness
//=============================================================
#define ADAPTIVE_SCE_REG u4SWReg(0x2000)
    #define ADAPTIVE_SCE_L Fld(8, 0, AC_FULLB0)
    #define ADAPTIVE_SCE_S Fld(8, 8, AC_FULLB1)
    #define ADAPTIVE_SCE_H Fld(8, 16, AC_FULLB2)
	#define ADAPTIVE_FORCE_SHPSCENECHG Fld(1, 30, AC_MSKB3)
    #define ADAPTIVE_SCE_FORCE_LOAD Fld(1, 31, AC_MSKB3)

#define ADAPTIVE_SKIN_REG u4SWReg(0x2004)
    #define ADAPTIVE_SKIN_L Fld(8, 0, AC_FULLB0)
    #define ADAPTIVE_SKIN_S Fld(8, 8, AC_FULLB1)
    #define ADAPTIVE_SKIN_H Fld(8, 16, AC_FULLB2)

#define ADAPTIVE_RGB_GAMMA_REG u4SWReg(0x2010)
    #define ADAPTIVE_RGB_GAMMA_SEL Fld(6, 0, AC_MSKB0)
    #define ADAPTIVE_RGB_GAMMA_FORCE_LOAD Fld(31, 0, AC_MSKB3)

#define ADAPTIVE_RGB_GAMMA_REG_01 u4SWReg(0x2014)
    #define ADAPTIVE_RGB_GAMMA_DECAY_UPTH Fld(8, 0, AC_FULLB0)
	#define ADAPTIVE_RGB_GAMMA_DECAY_LOWTH Fld(8, 8, AC_FULLB1)
	#define ADAPTIVE_RGB_GAMMA_BLENING_EN Fld(8, 16, AC_FULLB2)

#define SIXCOLOR_REG_00 u4SWReg(0x2020)
    #define SIXCOLOR_Y_M Fld(8, 0, AC_FULLB0)
    #define SIXCOLOR_Y_R Fld(8, 8, AC_FULLB1)
    #define SIXCOLOR_Y_Y Fld(8, 16, AC_FULLB2)
    #define SIXCOLOR_Y_G Fld(8, 24, AC_FULLB3)

#define SIXCOLOR_REG_01 u4SWReg(0x2024)
    #define SIXCOLOR_Y_C Fld(8, 0, AC_FULLB0)
    #define SIXCOLOR_Y_B Fld(8, 8, AC_FULLB1)
    #define SIXCOLOR_S_M Fld(8, 16, AC_FULLB2)
    #define SIXCOLOR_S_R Fld(8, 24, AC_FULLB3)

#define SIXCOLOR_REG_02 u4SWReg(0x2028)
    #define SIXCOLOR_S_Y Fld(8, 0, AC_FULLB0)
    #define SIXCOLOR_S_G Fld(8, 8, AC_FULLB1)
    #define SIXCOLOR_S_C Fld(8, 16, AC_FULLB2)
    #define SIXCOLOR_S_B Fld(8, 24, AC_FULLB3)

#define SIXCOLOR_REG_03 u4SWReg(0x202C)
    #define SIXCOLOR_H_M Fld(8, 0, AC_FULLB0)
    #define SIXCOLOR_H_R Fld(8, 8, AC_FULLB1)
    #define SIXCOLOR_H_Y Fld(8, 16, AC_FULLB2)
    #define SIXCOLOR_H_G Fld(8, 24, AC_FULLB3)
    
#define SIXCOLOR_REG_04 u4SWReg(0x2030)
    #define SIXCOLOR_H_C Fld(8, 0, AC_FULLB0)
    #define SIXCOLOR_H_B Fld(8, 8, AC_FULLB1)

#define TDPROC_YLEV_00 u4SWReg(0x2100)
    #define TDS_YLEV_ADAP_ENA Fld(1, 0, AC_MSKB0)       
    #define TDS_YLEV_ADL_ENA Fld(1, 1, AC_MSKB0)        
    #define TDS_YLEV_APL_ENA Fld(1, 2, AC_MSKB0)
    #define TDS_YLEV_APL_ZERO Fld(4, 8, AC_MSKB1)
    #define TDS_YLEV_APL_THR Fld(4, 12, AC_MSKB1)       
    #define TDS_YLEV_ADL_GAIN Fld(8, 24, AC_FULLB3)                
#define TDPROC_YLEV_01 u4SWReg(0x2104)
    #define TDS_YLEV_P1 Fld(4, 0, AC_MSKB0)
    #define TDS_YLEV_P2 Fld(4, 4, AC_MSKB0)
    #define TDS_YLEV_P3 Fld(4, 8, AC_MSKB1)
    #define TDS_YLEV_P4 Fld(4, 12, AC_MSKB1)
#define TDPROC_YLEV_02 u4SWReg(0x2108)
    #define TDS_YLEV_G1 Fld(8, 0, AC_FULLB0)
    #define TDS_YLEV_G2 Fld(8, 8, AC_FULLB1)
    #define TDS_YLEV_G3 Fld(8, 16, AC_FULLB2)
    #define TDS_YLEV_G4 Fld(8, 24, AC_FULLB3)

#define VI_SHP_LIMIT_00        0x2110
#define TDSHP_LIMIT_00 u4SWReg(VI_SHP_LIMIT_00)          
    #define TDSHARP_LIMIT_H1 Fld(8, 0, AC_FULLB0) 
    #define TDSHARP_LIMIT_H2 Fld(8, 8, AC_FULLB1) 
    #define TDSHARP_LIMIT_H3 Fld(8, 16, AC_FULLB2)
    #define TDSHARP_LIMIT_D1 Fld(8, 24, AC_FULLB3) 

#define VI_SHP_LIMIT_01        0x2114
#define TDSHP_LIMIT_01 u4SWReg(VI_SHP_LIMIT_01)       
    #define TDSHARP_LIMIT_D2 Fld(8, 0, AC_FULLB0)
    #define TDSHARP_LIMIT_D3 Fld(8, 8, AC_FULLB1)
    #define TDSHARP_LIMIT_V1 Fld(8, 16, AC_FULLB2)
    #define TDSHARP_LIMIT_V2 Fld(8, 24, AC_FULLB3) 

#define VI_SHP_LIMIT_02        0x2118
#define TDSHP_LIMIT_02 u4SWReg(VI_SHP_LIMIT_02)       
    #define LTI_LIMIT_LTI_H1 Fld(8, 0, AC_FULLB0)
    #define LTI_LIMIT_LTI_H2 Fld(8, 8, AC_FULLB1)
    #define LTI_LIMIT_LTI_V Fld(8, 16, AC_FULLB2)
    #define TDSHARP_LIMIT Fld(8, 24, AC_FULLB3)    

#define VI_SHP_LIMIT_03        0x211C
#define TDSHP_LIMIT_03 u4SWReg(VI_SHP_LIMIT_03)          
    #define TDSHARP_BOUND_H1 Fld(8, 0, AC_FULLB0) 
    #define TDSHARP_BOUND_H2 Fld(8, 8, AC_FULLB1) 
    #define TDSHARP_BOUND_H3 Fld(8, 16, AC_FULLB2)
    #define TDSHARP_BOUND_D1 Fld(8, 24, AC_FULLB3) 

#define VI_SHP_LIMIT_04        0x2120
#define TDSHP_LIMIT_04 u4SWReg(VI_SHP_LIMIT_04) 
    #define TDSHARP_BOUND_D2 Fld(8, 0, AC_FULLB0)
    #define TDSHARP_BOUND_D3 Fld(8, 8, AC_FULLB1)
    #define TDSHARP_BOUND_V1 Fld(8, 16, AC_FULLB2)
    #define TDSHARP_BOUND_V2 Fld(8, 24, AC_FULLB3) 
    
#define VI_SHP_LIMIT_05        0x2124
#define TDSHP_LIMIT_05 u4SWReg(VI_SHP_LIMIT_05)       
    #define LTI_BOUND_LTI_H1 Fld(8, 0, AC_FULLB0)
    #define LTI_BOUND_LTI_H2 Fld(8, 8, AC_FULLB1)
    #define LTI_BOUND_LTI_V Fld(8, 16, AC_FULLB2)
    #define TDSHARP_BOUND Fld(8, 24, AC_FULLB3)      

#define VI_SHP_LIMIT_06        0x2128
#define TDSHP_LIMIT_06 u4SWReg(VI_SHP_LIMIT_06)       
    #define TDSHARP_LIMIT_V3 Fld(8, 0, AC_FULLB0)
    #define TDSHARP_BOUND_V3 Fld(8, 8, AC_FULLB1)

#define TDSHP_GLOBAL u4SWReg(0x2130)
 	    #define TDS_GLOBAL_GAIN Fld(8, 0, AC_FULLB0)
		#define TDS_WRT_PROTECT Fld(1, 8, AC_MSKB1)
		
#define MOADAP_SHARP_00 u4SWReg(0x2134)
    #define MO_SHP_EN Fld(1, 31, AC_MSKB3)
    #define MO_SHP_YLUT_GAIN Fld(4, 20, AC_MSKB2)       
    #define MO_SHP_XLUT_GAIN Fld(4, 16, AC_MSKB2)       
    #define MO_SHP_YLEVEL Fld(4, 12, AC_MSKB1)       
    #define MO_SHP_XLEVEL Fld(4, 8, AC_MSKB1)            
    #define MO_SHP_LEVEL Fld(4, 0, AC_MSKB0)    
		
#define BNR_SHARP_00 u4SWReg(0x2138)
    #define BNR_SHP_EN Fld(1, 31, AC_MSKB3)
    #define BNR_SHP_LEVEL Fld(5, 0, AC_MSKB0)  

#define TDSHPALL_ADAP_GAIN_00 u4SWReg(0x213C)       
    #define TDSHARPALL_GAIN_HD Fld(8, 0, AC_FULLB0)
    #define TDSHARPALL_GAIN_SD Fld(8, 8, AC_FULLB1)
    #define TDSHARPALL_GAIN Fld(8, 16, AC_FULLB2)

#define GLOBAL_ADJ_00 u4SWReg(0x2200)
#define GLOBAL_ADJ_01 u4SWReg(0x2204)
    #define HUE_DUMMY Fld(8, 8, AC_FULLB1) //15:8
    #define BRIGHT_DUMMY Fld(8, 0, AC_FULLB0) //7:0
#define GLOBAL_ADJ_02 u4SWReg(0x2208)
#define GLOBAL_ADJ_03 u4SWReg(0x220C)
    #define SAT_DUMMY Fld(10, 10, AC_MSKW21) //19:10
    #define CONT_DUMMY Fld(10, 0, AC_MSKW10) //9:0

#define SCE_SW_LOAD u4SWReg(0x2400)
    #define SCE_SW_LOAD_TRIG Fld(1, 31, AC_MSKB3)           //31
    #define SCE_SW_LOAD_FUNC_IDX Fld(8, 16, AC_FULLB2)      //23:16
    #define SCE_SW_LOAD_HUE_IDX Fld(8, 8, AC_FULLB1)        //15:8
    #define SCE_SW_LOAD_VALUE Fld(8, 0, AC_FULLB0)          //7:0

//=============================================================
// Addr : 0x2500 - 0x3000
// Name : SHARPNESS
//=============================================================

#define VI_SHP_GAIN_01			          0x2518
#define TDSHP_GAIN_01			  u4SWReg(VI_SHP_GAIN_01)
	#define TDSHP_GAIN_H1 			Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_GAIN_H2 			Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_GAIN_H3			Fld(8, 16, AC_FULLB2) //23:16	
	#define TDSHP_GAIN_D1			Fld(8, 24, AC_FULLB3) //31:24		
	
#define VI_SHP_GAIN_02                    0x251C
#define TDSHP_GAIN_02			  u4SWReg(VI_SHP_GAIN_02)
	#define TDSHP_GAIN_D2 			Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_GAIN_D3 			Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_GAIN_V1			Fld(8, 16, AC_FULLB2) //23:16	
	#define TDSHP_GAIN_V2			Fld(8, 24, AC_FULLB3) //31:24	
	
#define VI_SHP_GAIN_03                    0x2520
#define TDSHP_GAIN_03		      u4SWReg(VI_SHP_GAIN_03)
	#define TDSHP_GAIN_V3 			Fld(8, 0, AC_FULLB0) //7:0	
	
#define VI_SHP_LIMIT_RATIO_01                     0x2524
#define TDSHP_LIMIT_RATIO_01			  u4SWReg(VI_SHP_LIMIT_RATIO_01)
	#define TDSHP_LIMIT_RATIO_H1 			Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_LIMIT_RATIO_H2 			Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_LIMIT_RATIO_H3			Fld(8, 16, AC_FULLB2) //23:16	
	#define TDSHP_LIMIT_RATIO_D1			Fld(8, 24, AC_FULLB3) //31:24		
	
#define VI_SHP_LIMIT_RATIO_02                     0x2528
#define TDSHP_LIMIT_RATIO_02			  u4SWReg(VI_SHP_LIMIT_RATIO_02)
	#define TDSHP_LIMIT_RATIO_D2 			Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_LIMIT_RATIO_D3 			Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_LIMIT_RATIO_V1			Fld(8, 16, AC_FULLB2) //23:16	
	#define TDSHP_LIMIT_RATIO_V2			Fld(8, 24, AC_FULLB3) //31:24	

#define VI_SHP_LIMIT_RATIO_03                     0x252C	
#define TDSHP_LIMIT_RATIO_03	  		  u4SWReg(VI_SHP_LIMIT_RATIO_03)
	#define TDSHP_LIMIT_RATIO_V3 			Fld(8, 0, AC_FULLB0) //7:0	
	
#define VI_SHP_CORING_RATIO_01			          0x2530
#define TDSHP_CORING_RATIO_01			  u4SWReg(VI_SHP_CORING_RATIO_01)
	#define TDSHP_CORING_RATIO_H1 			Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_CORING_RATIO_H2 			Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_CORING_RATIO_H3			Fld(8, 16, AC_FULLB2) //23:16	
	#define TDSHP_CORING_RATIO_D1			Fld(8, 24, AC_FULLB3) //31:24		
		
#define VI_SHP_CORING_RATIO_02			          0x2534
#define TDSHP_CORING_RATIO_02			  u4SWReg(VI_SHP_CORING_RATIO_02)
	#define TDSHP_CORING_RATIO_D2 			Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_CORING_RATIO_D3 			Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_CORING_RATIO_V1			Fld(8, 16, AC_FULLB2) //23:16	
	#define TDSHP_CORING_RATIO_V2			Fld(8, 24, AC_FULLB3) //31:24	

#define VI_SHP_CORING_RATIO_03                    0x2538
#define TDSHP_CORING_RATIO_03	  		  u4SWReg(0x2538)
	#define TDSHP_CORING_RATIO_V3 			Fld(8, 0, AC_FULLB0) //7:0	

#define VI_SHP_CORE_01                    0x2554  
#define TDSHP_CORE_01			  u4SWReg(VI_SHP_CORE_01)
	#define TDSHP_CORE_H1 			Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_CORE_H2 			Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_CORE_H3			Fld(8, 16, AC_FULLB2) //23:16	
	#define TDSHP_CORE_D1			Fld(8, 24, AC_FULLB3) //31:24		

#define VI_SHP_CORE_02				      0x2558
#define TDSHP_CORE_02			  u4SWReg(VI_SHP_CORE_02)
	#define TDSHP_CORE_D2 			Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_CORE_D3 			Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_CORE_V1			Fld(8, 16, AC_FULLB2) //23:16	
	#define TDSHP_CORE_V2			Fld(8, 24, AC_FULLB3) //31:24	

#define VI_SHP_CORE_03					  0x255C
#define TDSHP_CORE_03	  		  u4SWReg(VI_SHP_CORE_03)
	#define TDSHP_CORE_V3 			Fld(8, 0, AC_FULLB0) //7:0	

#define VI_SHP_CORETH_01	              0x2560
#define TDSHP_CORETH_01			  u4SWReg(VI_SHP_CORETH_01)
	#define TDSHP_CORETH_H1 			Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_CORETH_H2 			Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_CORETH_H3			Fld(8, 16, AC_FULLB2) //23:16	
	#define TDSHP_CORETH_D1			Fld(8, 24, AC_FULLB3) //31:24		

#define VI_SHP_CORETH_02				  0x2564
#define TDSHP_CORETH_02			  u4SWReg(VI_SHP_CORETH_02)
	#define TDSHP_CORETH_D2 			Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_CORETH_D3 			Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_CORETH_V1			Fld(8, 16, AC_FULLB2) //23:16	
	#define TDSHP_CORETH_V2			Fld(8, 24, AC_FULLB3) //31:24	
						
#define VI_SHP_CORETH_03                  0x2568
#define TDSHP_CORETH_03	  		  u4SWReg(VI_SHP_CORETH_03)
	#define TDSHP_CORETH_V3 			Fld(8, 0, AC_FULLB0) //7:0	

//LTI
#define VI_SHP_GAIN_04                    0x256C
#define TDSHP_GAIN_04                   u4SWReg(VI_SHP_GAIN_04)
    #define TDSHP_GAIN_LTI_H1 			    Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_GAIN_LTI_H2 			    Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_GAIN_LTI_V			    Fld(8, 16, AC_FULLB2) //23:16

#define VI_SHP_CORING_RATIO_04            0x2570
#define TDSHP_CORING_RATIO_04           u4SWReg(VI_SHP_CORING_RATIO_04)
    #define TDSHP_CORE_RATIO_LTI_H1 	    Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_CORE_RATIO_LTI_H2 		Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_CORE_RATIO_LTI_V		Fld(8, 16, AC_FULLB2) //23:16    

#define VI_SHP_CORE_04                    0x2574
#define TDSHP_CORE_04			        u4SWReg(VI_SHP_CORE_04)
	#define TDSHP_CORE_LTI_H1 			    Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_CORE_LTI_H2 			    Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_CORE_LTI_V			    Fld(8, 16, AC_FULLB2) //23:16	

#define VI_SHP_CORETH_04                  0x2578
#define TDSHP_CORETH_04                 u4SWReg(VI_SHP_CORETH_04)
    #define TDSHP_CORETH_LTI_H1 			Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_CORETH_LTI_H2 			Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_CORETH_LTI_V			    Fld(8, 16, AC_FULLB2) //23:16	

#define VI_SHP_LIMIT_RATIO_04             0x257C
#define TDSHP_LIMIT_RATIO_04			u4SWReg(VI_SHP_LIMIT_RATIO_04)
	#define TDSHP_LIMIT_RATIO_LTI_H1 	    Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_LIMIT_RATIO_LTI_H2 		Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_LIMIT_RATIO_LTI_V			Fld(8, 16, AC_FULLB2) //23:16	

//PBC
    //Gain
#define VI_SHP_GAIN_05                          0x2580
#define TDSHP_GAIN_05                   u4SWReg(VI_SHP_GAIN_05)
    #define TDSHP_GAIN_PBC1_H1 			    Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_GAIN_PBC1_H2 			    Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_GAIN_PBC1_H3			    Fld(8, 16, AC_FULLB2) //23:16
	#define TDSHP_GAIN_PBC1_V1			    Fld(8, 24, AC_FULLB3) //31:24	

#define VI_SHP_GAIN_06                          0x2584
#define TDSHP_GAIN_06                   u4SWReg(VI_SHP_GAIN_06)
    #define TDSHP_GAIN_PBC1_V2			    Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_GAIN_PBC1_V3 			    Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_GAIN_PBC1_D1			    Fld(8, 16, AC_FULLB2) //23:16
	#define TDSHP_GAIN_PBC1_D2			    Fld(8, 24, AC_FULLB3) //31:24	

#define VI_SHP_GAIN_07                          0x2588
#define TDSHP_GAIN_07                   u4SWReg(VI_SHP_GAIN_07)
    #define TDSHP_GAIN_PBC1_D3			    Fld(8, 0, AC_FULLB0) //7:0
    #define TDSHP_GAIN_PBC2_H1 			    Fld(8, 8, AC_FULLB1) //15:8

    //Coring_ratio	
#define VI_SHP_CORING_RATIO_05                          0X259C
#define TDSHP_CORING_RATIO_05                   u4SWReg(VI_SHP_CORING_RATIO_05)
    #define TDSHP_CORE_RATIO_PBC1_H1 			    Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_CORE_RATIO_PBC1_H2 			    Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_CORE_RATIO_PBC1_H3			    Fld(8, 16, AC_FULLB2) //23:16
	#define TDSHP_CORE_RATIO_PBC1_V1			    Fld(8, 24, AC_FULLB3) //31:24	
        
#define VI_SHP_CORING_RATIO_06                          0X25A0
#define TDSHP_CORING_RATIO_06                   u4SWReg(VI_SHP_CORING_RATIO_06)
    #define TDSHP_CORE_RATIO_PBC1_V2			    Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_CORE_RATIO_PBC1_V3 			    Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_CORE_RATIO_PBC1_D1			    Fld(8, 16, AC_FULLB2) //23:16
	#define TDSHP_CORE_RATIO_PBC1_D2			    Fld(8, 24, AC_FULLB3) //31:24	
        
#define VI_SHP_CORING_RATIO_07                          0X25A4
#define TDSHP_CORING_RATIO_07                   u4SWReg(VI_SHP_CORING_RATIO_07)
    #define TDSHP_CORE_RATIO_PBC1_D3			    Fld(8, 0, AC_FULLB0) //7:0

    //coring_zero
#define VI_SHP_CORE_05                          0x25B8
#define TDSHP_CORE_05                   u4SWReg(VI_SHP_CORE_05)
    #define TDSHP_CORE_PBC1_H1 			    Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_CORE_PBC1_H2 			    Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_CORE_PBC1_H3			    Fld(8, 16, AC_FULLB2) //23:16
	#define TDSHP_CORE_PBC1_V1			    Fld(8, 24, AC_FULLB3) //31:24	
        
#define VI_SHP_CORE_06                          0x25BC
#define TDSHP_CORE_06                   u4SWReg(VI_SHP_CORE_06)
    #define TDSHP_CORE_PBC1_V2			    Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_CORE_PBC1_V3 			    Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_CORE_PBC1_D1			    Fld(8, 16, AC_FULLB2) //23:16
	#define TDSHP_CORE_PBC1_D2			    Fld(8, 24, AC_FULLB3) //31:24	
        
#define VI_SHP_CORE_07                          0x25C0
#define TDSHP_CORE_07                   u4SWReg(VI_SHP_CORE_07)
    #define TDSHP_CORE_PBC1_D3			    Fld(8, 0, AC_FULLB0) //7:0

    //coring_th
#define VI_SHP_CORETH_05                          0x25D4
#define TDSHP_CORETH_05                   u4SWReg(VI_SHP_CORETH_05)
    #define TDSHP_CORETH_PBC1_H1 			    Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_CORETH_PBC1_H2 			    Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_CORETH_PBC1_H3			    Fld(8, 16, AC_FULLB2) //23:16
	#define TDSHP_CORETH_PBC1_V1			    Fld(8, 24, AC_FULLB3) //31:24	
        
#define VI_SHP_CORETH_06                          0x25D8
#define TDSHP_CORETH_06                   u4SWReg(VI_SHP_CORETH_06)
    #define TDSHP_CORETH_PBC1_V2			    Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_CORETH_PBC1_V3 			    Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_CORETH_PBC1_D1			    Fld(8, 16, AC_FULLB2) //23:16
	#define TDSHP_CORETH_PBC1_D2			    Fld(8, 24, AC_FULLB3) //31:24	
        
#define VI_SHP_CORETH_07                          0x25DC
#define TDSHP_CORETH_07                   u4SWReg(VI_SHP_CORETH_07)
    #define TDSHP_CORETH_PBC1_D3			    Fld(8, 0, AC_FULLB0) //7:0
	
    //Limit_ratio
#define VI_SHP_LIMIT_RATIO_05                          0x25F0
#define TDSHP_LIMIT_RATIO_05                   u4SWReg(VI_SHP_LIMIT_RATIO_05)
    #define TDSHP_LIMIT_RATIO_PBC1_H1 			    Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_LIMIT_RATIO_PBC1_H2 			    Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_LIMIT_RATIO_PBC1_H3			    Fld(8, 16, AC_FULLB2) //23:16
	#define TDSHP_LIMIT_RATIO_PBC1_V1			    Fld(8, 24, AC_FULLB3) //31:24	
        
#define VI_SHP_LIMIT_RATIO_06                          0x25F4
#define TDSHP_LIMIT_RATIO_06                   u4SWReg(VI_SHP_LIMIT_RATIO_06)
    #define TDSHP_LIMIT_RATIO_PBC1_V2			    Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_LIMIT_RATIO_PBC1_V3 			    Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_LIMIT_RATIO_PBC1_D1			    Fld(8, 16, AC_FULLB2) //23:16
	#define TDSHP_LIMIT_RATIO_PBC1_D2			    Fld(8, 24, AC_FULLB3) //31:24	
        
#define VI_SHP_LIMIT_RATIO_07                          0x25F8
#define TDSHP_LIMIT_RATIO_07                   u4SWReg(VI_SHP_LIMIT_RATIO_07)
    #define TDSHP_LIMIT_RATIO_PBC1_D3			    Fld(8, 0, AC_FULLB0) //7:0

    //Limit
#define VI_SHP_LIMIT_05_2                        0x260C
#define TDSHP_LIMIT_05_2                 u4SWReg(VI_SHP_LIMIT_05_2)
    #define TDSHP_LIMIT_PBC1_H1 			Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_LIMIT_PBC1_H2 			Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_LIMIT_PBC1_H3			    Fld(8, 16, AC_FULLB2) //23:16
	#define TDSHP_LIMIT_PBC1_V1			    Fld(8, 24, AC_FULLB3) //31:24	
    
#define VI_SHP_LIMIT_06_2                        0x2610   
#define TDSHP_LIMIT_06_2                 u4SWReg(VI_SHP_LIMIT_06_2)
    #define TDSHP_LIMIT_PBC1_V2			    Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_LIMIT_PBC1_V3 			Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_LIMIT_PBC1_D1			    Fld(8, 16, AC_FULLB2) //23:16
	#define TDSHP_LIMIT_PBC1_D2			    Fld(8, 24, AC_FULLB3) //31:24	
    
#define VI_SHP_LIMIT_07                          0x2614
#define TDSHP_LIMIT_07                   u4SWReg(VI_SHP_LIMIT_07)
    #define TDSHP_LIMIT_PBC1_D3			    Fld(8, 0, AC_FULLB0) //7:0

    //Limit_bound
#define VI_SHP_BOUND_05                          0x2628
#define TDSHP_BOUND_05                   u4SWReg(VI_SHP_BOUND_05)
    #define TDSHP_BOUND_PBC1_H1 			Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_BOUND_PBC1_H2 			Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_BOUND_PBC1_H3			    Fld(8, 16, AC_FULLB2) //23:16
	#define TDSHP_BOUND_PBC1_V1			    Fld(8, 24, AC_FULLB3) //31:24	
                
#define VI_SHP_BOUND_06                          0x262C
#define TDSHP_BOUND_06                   u4SWReg(VI_SHP_BOUND_06)
    #define TDSHP_BOUND_PBC1_V2			    Fld(8, 0, AC_FULLB0) //7:0	
	#define TDSHP_BOUND_PBC1_V3 			Fld(8, 8, AC_FULLB1) //15:8
	#define TDSHP_BOUND_PBC1_D1			    Fld(8, 16, AC_FULLB2) //23:16
	#define TDSHP_BOUND_PBC1_D2			    Fld(8, 24, AC_FULLB3) //31:24	
                
#define VI_SHP_BOUND_07                          0x2630
#define TDSHP_BOUND_07                   u4SWReg(VI_SHP_BOUND_07)
    #define TDSHP_BOUND_PBC1_D3			    Fld(8, 0, AC_FULLB0) //7:0

#define TDSHP_PBCNR_00                   u4SWReg(0x2644)
	#define DIFF_BOTTOM 			Fld(8, 0, AC_FULLB0) //7:0
	#define DIFF_UP 			Fld(8, 8, AC_FULLB1) //15:8
	#define AREA_BOTTOM 			Fld(8, 16, AC_FULLB2) //23:16
	#define AREA_UP 			Fld(8, 24, AC_FULLB3) //31:24	

#define TDSHP_PBCNR_01                   u4SWReg(0x2648)
	#define CONF_TH 			Fld(8, 0, AC_FULLB0) //7:0
	#define PBCNR_ONOFF     Fld(1,  8, AC_MSKDW) //8:8

#define SHARPNESS_3D_00     u4SWReg(0x2700) 
	#define SHARPNESS_3D_MODE 	        Fld(8, 0, AC_FULLB0) //7:0	

#define SHARPNESS_3D_01     u4SWReg(0x2704)                                                      
	#define SHARPNESS_FS_HGAIN 	        Fld(8, 0, AC_FULLB0) //7:0		     
	#define SHARPNESS_FS_DGAIN          Fld(8, 8, AC_FULLB1) //15:8		 
	#define SHARPNESS_FS_VGAIN          Fld(8, 16, AC_FULLB2) //23:16			
                                                                                                 
#define SHARPNESS_3D_02     u4SWReg(0x2708)                                                      
	#define SHARPNESS_TTD_HGAIN 	Fld(8, 0, AC_FULLB0) //7:0		  
	#define SHARPNESS_TTD_DGAIN         Fld(8, 8, AC_FULLB1) //15:8		     
	#define SHARPNESS_TTD_VGAIN         Fld(8, 16, AC_FULLB2) //23:16	      
                                                                                                 
#define SHARPNESS_3D_03     u4SWReg(0x270C)                                       			
	#define SHARPNESS_SBS_HGAIN 	Fld(8, 0, AC_FULLB0) //7:0	
	#define SHARPNESS_SBS_DGAIN         Fld(8, 8, AC_FULLB1) //15:8	 
	#define SHARPNESS_SBS_VGAIN         Fld(8, 16, AC_FULLB2) //23:16  
	                                                                                         
#define SHARPNESS_3D_04     u4SWReg(0x2710)                                       	         
	#define SHARPNESS_TB_HGAIN 		Fld(8, 0, AC_FULLB0) //7:0
	#define SHARPNESS_TB_DGAIN          Fld(8, 8, AC_FULLB1) //15:8
	#define SHARPNESS_TB_VGAIN          Fld(8, 16, AC_FULLB2) //23:16
	                                                                                         
#define SHARPNESS_3D_05     u4SWReg(0x2714)                                       	         
	#define SHARPNESS_SS_HGAIN 		Fld(8, 0, AC_FULLB0) //7:0    
	#define SHARPNESS_SS_DGAIN          Fld(8, 8, AC_FULLB1) //15:8     
	#define SHARPNESS_SS_VGAIN          Fld(8, 16, AC_FULLB2) //23:16       
                                                                                                 
#define SHARPNESS_3D_06     u4SWReg(0x2718)                                       	         
	#define SHARPNESS_RD_HGAIN 		Fld(8, 0, AC_FULLB0) //7:0         
	#define SHARPNESS_RD_DGAIN          Fld(8, 8, AC_FULLB1) //15:8         
	#define SHARPNESS_RD_VGAIN          Fld(8, 16, AC_FULLB2) //23:16         
	                                                                                         
#define SHARPNESS_3D_07     u4SWReg(0x271C)                                       	         
	#define SHARPNESS_LI_HGAIN 		Fld(8, 0, AC_FULLB0) //7:0
	#define SHARPNESS_LI_DGAIN          Fld(8, 8, AC_FULLB1) //15:8
	#define SHARPNESS_LI_VGAIN          Fld(8, 16, AC_FULLB2) //23:16 
                                                                                                 
#define SHARPNESS_3D_08     u4SWReg(0x2720)                                       	         
	#define SHARPNESS_CB_HGAIN 		Fld(8, 0, AC_FULLB0) //7:0
	#define SHARPNESS_CB_DGAIN          Fld(8, 8, AC_FULLB1) //15:8
	#define SHARPNESS_CB_VGAIN          Fld(8, 16, AC_FULLB2) //23:16 
	                                                                                         
#define SHARPNESS_3D_09     u4SWReg(0x2724)                                       	         
	#define SHARPNESS_DA_HGAIN 		Fld(8, 0, AC_FULLB0) //7:0
	#define SHARPNESS_DA_DGAIN          Fld(8, 8, AC_FULLB1) //15:8
	#define SHARPNESS_DA_VGAIN          Fld(8, 16, AC_FULLB2) //23:16 

#define SHARPNESS_3D_0A     u4SWReg(0x2728)                                                      
	#define SHARPNESS_FS_HCLIP 	        Fld(8, 0, AC_FULLB0) //7:0		     
	#define SHARPNESS_FS_DCLIP          Fld(8, 8, AC_FULLB1) //15:8		 
	#define SHARPNESS_FS_VCLIP          Fld(8, 16, AC_FULLB2) //23:16			
																									 
#define SHARPNESS_3D_0B     u4SWReg(0x272C)                                                      
	#define SHARPNESS_TTD_HCLIP 			Fld(8, 0, AC_FULLB0) //7:0		  
	#define SHARPNESS_TTD_DCLIP         Fld(8, 8, AC_FULLB1) //15:8		     
	#define SHARPNESS_TTD_VCLIP         Fld(8, 16, AC_FULLB2) //23:16	      
																									 
#define SHARPNESS_3D_0C     u4SWReg(0x2730)                                       			
	#define SHARPNESS_SBS_HCLIP		 	Fld(8, 0, AC_FULLB0) //7:0	
	#define SHARPNESS_SBS_DCLIP         Fld(8, 8, AC_FULLB1) //15:8	 
	#define SHARPNESS_SBS_VCLIP         Fld(8, 16, AC_FULLB2) //23:16  
																								 
#define SHARPNESS_3D_0D     u4SWReg(0x2734)                                       	         
	#define SHARPNESS_TB_HCLIP			Fld(8, 0, AC_FULLB0) //7:0
	#define SHARPNESS_TB_DCLIP          Fld(8, 8, AC_FULLB1) //15:8
	#define SHARPNESS_TB_VCLIP          Fld(8, 16, AC_FULLB2) //23:16
																								 
#define SHARPNESS_3D_0E     u4SWReg(0x2738)                                       	         
	#define SHARPNESS_SS_HCLIP 		Fld(8, 0, AC_FULLB0) //7:0    
	#define SHARPNESS_SS_DCLIP         Fld(8, 8, AC_FULLB1) //15:8     
	#define SHARPNESS_SS_VCLIP         Fld(8, 16, AC_FULLB2) //23:16       
																									 
#define SHARPNESS_3D_0F     u4SWReg(0x273C)                                       	         
	#define SHARPNESS_RD_HCLIP 		Fld(8, 0, AC_FULLB0) //7:0         
	#define SHARPNESS_RD_DCLIP         Fld(8, 8, AC_FULLB1) //15:8         
	#define SHARPNESS_RD_VCLIP         Fld(8, 16, AC_FULLB2) //23:16         
																								 
#define SHARPNESS_3D_10     u4SWReg(0x2740)                                       	         
	#define SHARPNESS_LI_HCLIP 		Fld(8, 0, AC_FULLB0) //7:0
	#define SHARPNESS_LI_DCLIP          Fld(8, 8, AC_FULLB1) //15:8
	#define SHARPNESS_LI_VCLIP         Fld(8, 16, AC_FULLB2) //23:16 
																									 
#define SHARPNESS_3D_11     u4SWReg(0x2744)                                       	         
	#define SHARPNESS_CB_HCLIP 		Fld(8, 0, AC_FULLB0) //7:0
	#define SHARPNESS_CB_DCLIP          Fld(8, 8, AC_FULLB1) //15:8
	#define SHARPNESS_CB_VCLIP        Fld(8, 16, AC_FULLB2) //23:16 
																								 
#define SHARPNESS_3D_12     u4SWReg(0x2748)                                       	         
	#define SHARPNESS_DA_HCLIP 		Fld(8, 0, AC_FULLB0) //7:0
	#define SHARPNESS_DA_DCLIP         Fld(8, 8, AC_FULLB1) //15:8
	#define SHARPNESS_DA_VCLIP         Fld(8, 16, AC_FULLB2) //23:16 
	
#define SHARPNESS_3D_13     u4SWReg(0x274C)                                       	         
	#define SHARPNESS_3D_HGAIN 		Fld(8, 0, AC_FULLB0) //7:0
	#define SHARPNESS_3D_DGAIN           Fld(8, 8, AC_FULLB1) //15:8
	#define SHARPNESS_3D_VGAIN           Fld(8, 16, AC_FULLB2) //23:16 

#define SHARPNESS_3D_14     u4SWReg(0x2750)                                       	         
	#define SHARPNESS_3D_HCLIP 		Fld(8, 0, AC_FULLB0) //7:0
	#define SHARPNESS_3D_DCLIP          Fld(8, 8, AC_FULLB1) //15:8
	#define SHARPNESS_3D_VCLIP        Fld(8, 16, AC_FULLB2) //23:16 

#define TDS_ADAP_00 u4SWReg(0x2800)
    #define TDS_ADAP_EN Fld(1, 0, AC_MSKB0)
    #define TDS_ADAP_GMV_EN Fld(1, 1, AC_MSKB0) 
#define TDS_ADAP_01 u4SWReg(0x2804)
    #define TDS_ADAP_GMV_MIN          Fld(8, 0, AC_FULLB0) //7:0
    #define TDS_ADAP_GMV_MAX          Fld(8, 8, AC_FULLB1) //15:8
    #define TDS_ADAP_GAIN_MIN         Fld(8, 16, AC_FULLB2) //23:16
    #define TDS_ADAP_GAIN_MAX         Fld(8, 24, AC_FULLB3) //31:24
#define TDS_ADAP_02 u4SWReg(0x2808)
    #define TDS_ADAP_GMV_CNT_TH     Fld(8, 0, AC_FULLB0) //7:0
	
//=============================================================
// Addr : 0x3000 - 0x3FFF
// Name : PSCAN
//=============================================================
#define ADAPTIVE_PSCAN_REG  u4SWReg(0x3000)
    #define ADAPTIVE_PSCAN          Fld(16, 0, AC_MSKW10) 
    #define ADAPTIVE_PSCAN_MAIN     Fld( 1, 0, AC_MSKB0)
    #define ADAPTIVE_PSCAN_SUB      Fld( 1, 1, AC_MSKB0)
    #define ADAPTIVE_PSCAN_QTY      Fld( 1, 2, AC_MSKB0)
    #define ADAPTIVE_QTY_ISR        Fld( 1, 3, AC_MSKB0)
    #define ADAPTIVE_QTY_MLOOP      Fld( 1, 4, AC_MSKB0)  
	#define ADAPTIVE_MODECHG_LOG_M  Fld( 1, 9, AC_MSKB1)
	#define ADAPTIVE_MODECHG_LOG_S  Fld( 1, 10,AC_MSKB1)
	#define ADAPTIVE_DI_ISR_EX_EN  Fld( 1, 11,AC_MSKB1)
	#define PDINFO_MSK_AT_FS_OUTPUT Fld( 3, 26,AC_MSKB3)
	#define PDINFO_DLY_AT_PRO_DISP  Fld( 3, 29,AC_MSKB3)

#define VPREDOWN_INIT_PHASE  u4SWReg(0x3004)
    #define INIT_PHASE_T          Fld( 16, 0, AC_FULLW10) 
    #define INIT_PHASE_B          Fld( 16, 16, AC_FULLW32)

#define PSCAN_MISC_00       u4SWReg(0x3010)
    #define BANDWIDTH_PER_PXL_1     Fld(8, 0, AC_FULLB0)
    #define BANDWIDTH_PER_PXL_2     Fld(8, 8, AC_FULLB1)
    #define MISC_02                 Fld(8, 16, AC_FULLB2)
    #define MISC_03                 Fld(8, 24, AC_FULLB3)

#define PSCAN_MISC_01       u4SWReg(0x3014)
    #define MDDI_DEBUG_CNT     Fld(8, 0, AC_FULLB0)

#define PSCAN_FWFILM_00     u4SWReg(0x3030)
    #define FLD_MAX                 Fld(8, 0, AC_FULLB0)
    #define FLD_MIN                 Fld(8, 8, AC_FULLB1)
    #define FLD_GAIN1               Fld(8, 16, AC_FULLB2)
    #define FLD_GAIN2               Fld(8, 24, AC_FULLB3)

#define PSCAN_FWFILM_01     u4SWReg(0x3034)
    #define FLD_0                   Fld(8, 0, AC_FULLB0)
    #define FLD_1                   Fld(8, 8, AC_FULLB1)
    #define FLD_2                   Fld(8, 16, AC_FULLB2)
    #define FLD_3                   Fld(8, 24, AC_FULLB3)

#define PSCAN_FWFILM_02     u4SWReg(0x3038)
    #define FRM_MAX                 Fld(8, 0, AC_FULLB0)
    #define FRM_MIN                 Fld(8, 8, AC_FULLB1)
    #define FRM_GAIN1               Fld(8, 16, AC_FULLB2)
    #define FRM_GAIN2               Fld(8, 24, AC_FULLB3)

#define PSCAN_FWFILM_03     u4SWReg(0x303C)
    #define FRM_0                   Fld(8, 0, AC_FULLB0)
    #define FRM_1                   Fld(8, 8, AC_FULLB1)
    #define FRM_2                   Fld(8, 16, AC_FULLB2)
    #define FRM_3                   Fld(8, 24, AC_FULLB3)

#define PSCAN_FWFILM_04     u4SWReg(0x3040)
    #define HOMO_GAIN               Fld(8, 0, AC_FULLB0)
    #define FWFILM_1                Fld(8, 8, AC_FULLB1)
    #define FWFILM_2                Fld(8, 16, AC_FULLB2)
    #define FWFILM_3                Fld(8, 24, AC_FULLB3)

#define PSCAN_FWFILM_05     u4SWReg(0x3044)
    #define BLK_STILL               Fld(8, 0, AC_FULLB0)
    #define BLK_MOTION              Fld(8, 8, AC_FULLB1)
    #define BLK_2                   Fld(8, 16, AC_FULLB2)
    #define BLK_3                   Fld(8, 24, AC_FULLB3)

#define PSCAN_FWFILM_06     u4SWReg(0x3048)
    #define FILED_WEAVE_EN          Fld(1, 0, AC_MSKB0)
    #define DET_24TO50_EN          Fld(1, 1, AC_MSKB0)
    
#define PSCAN_FWCS_00       u4SWReg(0x3050)
    #define TOTAL_BLK               Fld(16, 0, AC_MSKW10) 
    #define GMV_EDGE_RATIO          Fld(8, 16, AC_FULLB2)
    #define Y4_RATIO                Fld(8, 24, AC_FULLB3)

#define PSCAN_FWCS_01       u4SWReg(0x3054)
    #define REDUCE_RATIO            Fld(8, 0, AC_FULLB0)
    #define NON_ZERO_CNT            Fld(8, 8, AC_FULLB1)
    #define ZERO_RATIO              Fld(8, 16, AC_FULLB2)
    #define ZERO_THLD               Fld(8, 24, AC_FULLB3)    

#define PSCAN_FWCS_02       u4SWReg(0x3058)
    #define HIST_ZERO_CNT       Fld(8, 0, AC_FULLB0)
    #define FAVOR_CS_STATE      Fld(8, 8, AC_FULLB1)
    #define FAVOR_CS_EN         Fld(1, 16, AC_MSKB2)
    #define NOISE_REDUCE_EN     Fld(1, 17, AC_MSKB2)
    
#define PSCAN_FWCS_03       u4SWReg(0x305C)
    #define HIST_DIFF_RATIO     Fld(8, 0, AC_FULLB0)
    #define COUNT_MAX_BND       Fld(4, 8, AC_MSKB1)    
    #define COUNT_EXE_THD       Fld(4, 12, AC_MSKB1)
    
#define PSCAN_FWCS_04       u4SWReg(0x3060)
    #define REDUCE_NOISE_TH     Fld(8, 0, AC_FULLB0)
    #define REDUCE_STEP         Fld(4, 8, AC_MSKB1)    

    
#define PSCAN_FW_ADAPTIVE_FILM_00       u4SWReg(0x3080)
    #define FR_MIN              Fld(16, 0, AC_FULLW10)
    #define EG_MIN              Fld(16, 16, AC_FULLW32)
 
#define PSCAN_FW_ADAPTIVE_FILM_01       u4SWReg(0x3084)
    #define FR_RATE             Fld(4, 0, AC_MSKB0)
    #define EG_RATE_32          Fld(4, 4, AC_MSKB0)
    #define EG_RATE_22          Fld(4, 8, AC_MSKB1)    
    
#define PSCAN_FW_ADAPTIVE_FILM_02       u4SWReg(0x3088)
    #define FR_MAX              Fld(16, 0, AC_FULLW10)

#define PSCAN_FW_ADAPTIVE_FILM_03       u4SWReg(0x308C)
    #define SUBTITLE_RATE       Fld(8, 0, AC_FULLB0)

#define PSCAN_FW_ADAPTIVE_FILM_04       u4SWReg(0x3090)

#define PSCAN_FW_ADAPTIVE_FILM_05       u4SWReg(0x3094)
    #define EG_MAX_START_32     Fld(16, 0, AC_FULLW10)
    #define EG_MAX_END_32       Fld(16, 16, AC_FULLW32)
    
#define PSCAN_FW_ADAPTIVE_FILM_06       u4SWReg(0x3098)
    #define EG_MAX_END_BIT                 Fld(8, 0, AC_MSKB0)

#define PSCAN_FW_ADAPTIVE_FILM_07       u4SWReg(0x309C)
    #define EG_MAX_START_22                Fld(16, 0, AC_FULLW10)
    #define EG_MAX_END_22                  Fld(16, 16, AC_FULLW32)
        
#define PSCAN_FW_ADAPTIVE_FILM_08       u4SWReg(0x30A0)
    #define FR_MO_FILTER_L                 Fld(8, 0, AC_FULLB0)
    #define FR_MO_FILTER_H                 Fld(8, 8, AC_FULLB1)
    #define FR_MO_LARGE_BIT                Fld(8, 16, AC_FULLB2)

#define PSCAN_FW_ADAPTIVE_FILM_09       u4SWReg(0x30A4)
    #define EG_MO_FILTER_L                  Fld(8, 0, AC_FULLB0)
    #define EG_MO_FILTER_H                  Fld(8, 8, AC_FULLB1)
    #define EG_MO_LARGE_BIT                 Fld(8, 16, AC_FULLB2)
    
#define PSCAN_FW_ADAPTIVE_FILM_0A       u4SWReg(0x30A8)
    #define STA_FIELD_SEQ                   Fld(32, 0, AC_FULLDW)

#define PSCAN_FW_ADAPTIVE_FILM_0B       u4SWReg(0x30AC)
    #define STA_FRAME_SEQ                   Fld(32, 0, AC_FULLDW)
   
#define PSCAN_FW_ADAPTIVE_FILM_0C       u4SWReg(0x30B0)
    #define SMALL_MO_THLD                   Fld(32, 0, AC_FULLDW)

   
#define PSCAN_FW_ADAPTIVE_DAR_00       u4SWReg(0x30C0)
    #define DAR_V_MAX_MIN                   Fld(8, 0, AC_FULLB0)
    #define DAR_V_MAX_MAX                   Fld(8, 8, AC_FULLB1)
    #define DAR_OFF_IN_FILM                 Fld(1, 16, AC_MSKB2) 

#define PSCAN_FW_ADAPTIVE_DAR_01       u4SWReg(0x30C4)
    #define DAR_V_MOTION_PXL_CORING         Fld(16, 0, AC_FULLW10)
    #define DAR_V_MOTION_SUM_CORING         Fld(16, 16, AC_FULLW32)

#define PSCAN_FW_ADAPTIVE_DAR_02       u4SWReg(0x30C8)
    #define DAR_V_MOTION_PXL_SHIFT          Fld(4, 0, AC_MSKB0)
    #define DAR_V_MOTION_SUM_SHIFT          Fld(4, 4, AC_MSKB0)
    #define DAR_V_FILTER_STR_DELAY_SHIFT    Fld(4, 8, AC_MSKB1)

#define PSCAN_FW_ADAPTIVE_DAR_03       u4SWReg(0x30CC)
    #define DAR_V_TBL_IN_FOR_NOISE          Fld(8, 0, AC_FULLB0)
    #define DAR_V_TBL_OUT_FOR_NOISE         Fld(8, 8, AC_FULLB1)

#define PSCAN_FW_ADAPTIVE_IF_00       u4SWReg(0x30D0)
    #define MOVING_CZP_BOB_EN       Fld(1, 0, AC_MSKB0)    
    #define FLD_INV_EN              Fld(1, 1, AC_MSKB0)
    #define CZP_AD_EN               Fld(1, 2, AC_MSKB0)

#define PSCAN_FW_ADAPTIVE_IF_01       u4SWReg(0x30D4)
    #define FLD_INV_IN_THD      Fld(4, 0, AC_MSKB0)
    #define FLD_INV_OUT_THD     Fld(4, 4, AC_MSKB0)
    #define FLD_INV_MAX_THD     Fld(4, 8, AC_MSKB1)    
    #define CZP_AD_MIN_THD      Fld(4, 12, AC_MSKB1)

//=============================================================
// Addr : 0x3800 - 0x3FFF
// Name : TDC
//=============================================================

 #define TDC_FW_00   u4SWReg(0x3800)
    #define TDC_FW_CROSS_COLOR_RD   Fld(1, 0, AC_MSKB0)
    #define TDC_FW_CROSS_COLOR_RD_BY_DI   Fld(1, 1, AC_MSKB0)
    #define TDC_FW_DOTCRAWL_RD   Fld(1, 2, AC_MSKB0)   
    
//=============================================================
// Addr : 0x4000 - 0x4FFF
// Name : TDTV
//=============================================================
    
#define TDTV_FW_00              u4SWReg(0x4000)
    #define BL_MLOOP            Fld(1, 0, AC_MSKB0)
    #define SCE_3D_GAIN_00         Fld(8,  8, AC_FULLB1) 
    #define SCE_3D_GAIN_01         Fld(8,  16, AC_FULLB2) 

#define TDNAVI_FW_00            u4SWReg(0x4400)
    #define TDNAVI_RST_LOG         Fld(1, 0, AC_MSKB0)
    #define TDNAVI_SBS_LOG         Fld(1, 1, AC_MSKB0)
    #define TDNAVI_TAB_LOG         Fld(1, 2, AC_MSKB0)
	#define TDNAVI_SW_LOG		   Fld(1, 3, AC_MSKB0)
    
    #define TDNAVI_DEBUG		   Fld(1, 4, AC_MSKB0) //could control  g_u1TdNaviAdaptOnOff    
    

#define TDNAVI_FW_01            u4SWReg(0x4404)
#define TDNAVI_FW_02            u4SWReg(0x4408)
#define TDNAVI_FW_03            u4SWReg(0x440C) 
#define TDNAVI_FW_04            u4SWReg(0x4410)
#define TDNAVI_FW_05            u4SWReg(0x4414)
#define TDNAVI_FW_06            u4SWReg(0x4418)


#define TDNAVI_FW_16            u4SWReg(0x4440)
    #define FRM_DET_3D_HI_TH          Fld(8,   0, AC_FULLB0)
    #define FRM_DET_3D_LO_TH          Fld(8,   8, AC_FULLB1) 
    #define FRM_DET_2D_HI_TH          Fld(8,  16, AC_FULLB2)
    #define FRM_DET_2D_LO_TH          Fld(8,  24, AC_FULLB3) 

#define TDNAVI_FW_17            u4SWReg(0x4444)
    #define VLD_DIV_INVLD_THR         Fld(8,   0, AC_FULLB0) // [7:0]
    #define STATUS_PRINT_EN      Fld(1,  31, AC_MSKB3) // [31:31]
    
#define TDNAVI_FW_18            u4SWReg(0x4448)
    #define IN_3D_SIMILARITY_RATIO    Fld(8,   0, AC_FULLB0) // [7:0]
    #define IN_3D_SIMILAR_THR         Fld(5,   8, AC_MSKB1) // [12:8]
    #define IN_3D_DECISION_TYPE       Fld(2,  14, AC_MSKB1) // [15:14]
    #define OUT_3D_SIMILARITY_RATIO   Fld(8,  16, AC_FULLB2) // [23:16]
    #define OUT_3D_SIMILAR_THR        Fld(5,  24, AC_MSKB3) // [28:24]
    #define OUT_3D_DECISION_TYPE      Fld(2,  30, AC_MSKB3) // [31:30]

#define TDNAVI_FW_19            u4SWReg(0x444C)
    #define SBS_TAB_LUT_0          Fld(8,   0, AC_FULLB0)
    #define SBS_TAB_LUT_1          Fld(8,   8, AC_FULLB1) 
    #define SBS_TAB_LUT_2          Fld(8,  16, AC_FULLB2)
    #define SBS_TAB_LUT_3          Fld(8,  24, AC_FULLB3) 

#define TDNAVI_FW_20            u4SWReg(0x4450)
    #define SBS_TAB_LUT_4          Fld(8,   0, AC_FULLB0)
    #define SBS_TAB_LUT_5          Fld(8,   8, AC_FULLB1) 
    #define SBS_TAB_LUT_6          Fld(8,  16, AC_FULLB2)
    #define SBS_TAB_LUT_7          Fld(8,  24, AC_FULLB3) 

#define TDNAVI_FW_21            u4SWReg(0x4454)
    #define SBS_TAB_LUT_8          Fld(8,   0, AC_FULLB0)

#define TDNAVI_STATUS_00            u4SWReg(0x4480)
    #define LI_CNT                    Fld(16,   0, AC_FULLW10)
    #define LI_FCT                    Fld(9,   16, AC_MSKW32)
#define TDNAVI_STATUS_01            u4SWReg(0x4484)
    #define CI_CNT                    Fld(16,   0, AC_FULLW10)
    #define CI_FCT                    Fld(9,   16, AC_MSKW32)    
#define TDNAVI_STATUS_02            u4SWReg(0x4488)
    #define CB_CNT                    Fld(16,   0, AC_FULLW10)
    #define CB_FCT                    Fld(9,   16, AC_MSKW32)    
    
#define TDNAVI_STATUS_16            u4SWReg(0x44C0)
    #define STA_FW_DET_TYPE           Fld(4,   0, AC_MSKB0)  // [3:0]
    #define STA_TOO_FEW         	  Fld(8,   8, AC_FULLB1) // [15:8]
    #define STA_AMBIGUO               Fld(9,  16, AC_MSKW32) // [24:16]

#define TDNAVI_STATUS_17            u4SWReg(0x44C4)
    #define STA_SBS_RATE              Fld(9,   0, AC_MSKW10) // [8:0]
    #define STA_TAB_RATE              Fld(9,  16, AC_MSKW32) // [24:16]

#define TDNAVI_STATUS_18            u4SWReg(0x44C8)
    #define STA_NEIGHBOR              Fld(9,   0, AC_MSKW10) // [8:0]
    
// TTD  :  0x4800
#define TTD_FW_REG_00          u4SWReg(0x4800)    
    #define TTD_FW_EN               Fld( 1, 0, AC_MSKB0)
    #define TTD_FW_HIST_EN          Fld( 1, 1, AC_MSKB0)
    #define TTD_FW_FORCE_SC         Fld( 1, 2, AC_MSKB0)
    #define TTD_FW_ADAP_VERT_BASE   Fld( 1, 3, AC_MSKB0)
    #define TTD_FW_ADAP_GM_EN       Fld( 1, 4, AC_MSKB0)
    #define TTD_FW_FORCE_SC_CNT     Fld( 8, 8, AC_FULLB1) 
    #define TTD_FW_IMGSAFE_DBG_EN   Fld( 1, 26, AC_MSKB3)
    #define TTD_FW_UI_DBG_EN        Fld( 1, 27, AC_MSKB3)
    #define TTD_FW_INV_DBG_EN       Fld( 1, 28, AC_MSKB3)
    #define TTD_FW_HIST_DBG_EN      Fld( 1, 29, AC_MSKB3)
    #define TTD_INVERSE_FW_RESULT   Fld( 1, 30, AC_MSKB3)
    #define TTD_FW_IS_INVERSE       Fld( 1, 31, AC_MSKB3)

#define TTD_FW_REG_01           u4SWReg(0x4804)    
    #define TTD_PRE_SHIFT_AVG_H     Fld(16, 0, AC_FULLW10) // 15:0    
    #define TTD_SHIFT_AVG_H         Fld(16, 16, AC_FULLW32) // 23:16  

#define TTD_FW_REG_02           u4SWReg(0x4808)    
    #define TTD_SMTH_SEL            Fld(4, 0, AC_MSKB0) 
    #define TTD_TMPR_SEL            Fld(4, 4, AC_MSKB0) 
    #define TTD_ZERO_SEL            Fld(4, 8, AC_MSKB1) 
    #define TTD_RAND_SEL            Fld(4, 12, AC_MSKB1)  
    #define TTD_ORD_QUAD_SEL        Fld(4, 16, AC_MSKB2)    
    #define TTD_VAR_SEL             Fld(4, 20, AC_MSKB2)
    #define TTD_VAR_COR_TH          Fld(4, 24, AC_MSKB3)    
        
#define TTD_FW_REG_03           u4SWReg(0x480C)    
    #define TTD_AVG_THLD            Fld(8,  0, AC_FULLB0) 
    #define TTD_HIST_THLD           Fld(8,  8, AC_FULLB1) 
    #define TTD_DIVER_THLD_H        Fld(8, 16, AC_FULLB2) 
    #define TTD_DIVER_THLD_L        Fld(8, 24, AC_FULLB3) 

#define TTD_FW_REG_04           u4SWReg(0x4810)    
    #define TTD_DE_ARRAY_W          Fld(16, 0, AC_FULLW10) // 15:0    
    #define TTD_DE_ARRAY_H          Fld(16, 16, AC_FULLW32) // 23:16  
    
#define TTD_FW_REG_05           u4SWReg(0x4814)    
    #define TTD_Y_SUBSAMPLE         Fld(8, 0, AC_FULLB0) 
    #define TTD_STAB_CNT            Fld(8, 8, AC_FULLB1) 
    #define TTD_SR_H                Fld(8, 16, AC_FULLB2) // 23:16
    #define TTD_STEP_H              Fld(4, 24, AC_MSKB3) // 27:24
    #define TTD_OBME_NUM            Fld(4, 28, AC_MSKB3) // 31:28
        
#define TTD_FW_REG_06           u4SWReg(0x4818)    
    #define TTD_MB_SIZE             Fld(4, 0, AC_MSKB0) 
    #define TTD_CAND_NUM            Fld(4, 4, AC_MSKB0) 
    #define TTD_VAR_EN_TH           Fld(16, 8, AC_FULLW21) // 15:8

#define TTD_FW_REG_07           u4SWReg(0x481C)    
    #define TTD_CUR_CONVG           Fld(16, 0, AC_FULLW10) // 15:0  
    #define TTD_FG_WEIGHT           Fld(4, 16, AC_MSKB2)
    #define TTD_BG_WEIGHT           Fld(4, 20, AC_MSKB2)
	#define AMW_FOCAL_PLEN 			Fld(8, 24, AC_FULLB3)
    
#define TTD_FW_REG_08           u4SWReg(0x4820)    
    #define LSV_AUTO_GAIN	        Fld(8,  0, AC_FULLB0) 
    #define LSV_CONVG_PLEN	        Fld(8,  8, AC_FULLB1) 
    #define LSV_CID_GAIN	   		Fld(8, 16, AC_FULLB2) 
    #define LSV_CID_APL_AVG	    	Fld(8, 24, AC_FULLB3) 

#define TTD_FW_REG_09           u4SWReg(0x4824)    
    #define TTD_CUR_AVG             Fld(8,  0, AC_FULLB0) 
    #define TTD_MAX_FIR             Fld(8,  8, AC_FULLB1) 
    #define TTD_MAX_SEC             Fld(8, 16, AC_FULLB2) 
    #define TTD_DIVER               Fld(8, 24, AC_FULLB3) 

#define TTD_FW_REG_10           u4SWReg(0x4828)    
    #define D2D_CONVG              Fld(8,  0, AC_FULLB0) 
    #define D2D_RATIO              Fld(8,  8, AC_FULLB1) 
    #define D2D_GAIN               Fld(8, 16, AC_FULLB2) 

#define TTD_FW_REG_11           u4SWReg(0x482c)    
    #define TTD_LOCAL_GAIN         Fld(8,  0, AC_FULLB0) 
    #define TTD_GLOBAL_GAIN        Fld(8,  8, AC_FULLB1) 
    #define TTD_VARIANCE           Fld(8, 16, AC_FULLB2) 
    #define TTD_HUE_VARIANCE       Fld(8, 24, AC_FULLB3)     

#define TTD_FW_REG_12           u4SWReg(0x4830) 
    #define TTD_LOCAL_MAX        Fld(8,  0, AC_FULLB0) 
    #define TTD_LOCAL_MIN        Fld(8,  8, AC_FULLB1)
    #define TTD_GLOBAL_MAX       Fld(8, 16, AC_FULLB2) 
    #define TTD_GLOBAL_MIN       Fld(8, 24, AC_FULLB3) 
    
#define TTD_FW_REG_13           u4SWReg(0x4834)    
    #define TTD_APL_SCN_CHG_TH    Fld(8,  0, AC_FULLB0) 
    #define TTD_DIVER_SCN_CHG_TH  Fld(8,  8, AC_FULLB1) 
    #define TTD_SCN_CHG_CNT_THR   Fld(8, 16, AC_FULLB2) 
    #define TTD_VERT_GAIN         Fld(8, 24, AC_FULLB3) 

#define TTD_FW_REG_14           u4SWReg(0x4838)    
    #define TTD_PRE_SHIFT_AVG_H     Fld(16, 0, AC_FULLW10) // 15:0    
    #define TTD_SHIFT_AVG_H         Fld(16, 16, AC_FULLW32) // 23:16  

#define TTD_FW_REG_15           u4SWReg(0x483C)    
    #define TTD_HORI_SM_WEIGHT          Fld(16, 0, AC_FULLW10) // 15:0    
    #define TTD_VERT_SM_WEIGHT          Fld(16, 16, AC_FULLW32) // 23:16  

#define TTD_FW_REG_16           u4SWReg(0x4840)    
    #define TTD_IIR_NORMAL          Fld(4, 0, AC_MSKB0) 
    #define TTD_IIR_FREEZE          Fld(4, 4, AC_MSKB0) 
    #define TTD_TMPR_SEL_FREEZE     Fld(4, 8, AC_MSKB1) 
    #define TTD_RAND_SEL_FREEZE     Fld(4, 12, AC_MSKB1)  
    #define TTD_DEBUG_CNT           Fld(8, 16, AC_FULLB2)

#define TTD_FW_REG_17           u4SWReg(0x4844)    
    #define TTD_RGN_MAX          Fld(16, 0, AC_FULLW10) // 15:0    
    #define TTD_TFU_GAIN         Fld(8, 16, AC_FULLB2) 
    #define TTD_IMGSAFETY_SCALE  Fld(8, 24, AC_FULLB3) 

#define SW_TTD_REG_00          u4SWReg(0x4A00)  
    #define SW_LSV_EN              Fld( 1, 0,  AC_MSKB0)//[0:0] 
    #define SW_LSV_CONVG_GAIN      Fld( 6, 1,  AC_MSKB0)//[6:1]
    #define SW_LSV_ENH_GAIN        Fld( 5, 8,  AC_MSKB1)//[12:8]
    #define SW_LSV_HEIGHT_DEC      Fld( 4, 16, AC_MSKB2)  
    #define SW_LSV_WIDTH_DEC       Fld( 4, 20, AC_MSKB2) 
    #define SW_LSV_DELAY_TIMES     Fld( 4, 24, AC_MSKB3) 

#define SW_TDDC_REG_00          u4SWReg(0x4B00)  
    #define TDDC_FW_ENABLE          Fld(1, 0, AC_MSKB0)    
    #define TDDC_MV_PATCH           Fld(1, 1, AC_MSKB0)     
    #define TDDC_NOT_CLEAN_ISR      Fld(1, 5, AC_MSKB0) 
    #define TDDC_MANUAL_GAIN        Fld(1, 6, AC_MSKB0)
    #define TDDC_TRIGGER_LOOP       Fld(4, 8, AC_MSKB1)  
    #define TDDC_NAVI_LOG           Fld(1, 12, AC_MSKB1)      
    #define TDDC_MV_INK_INDEX       Fld(16, 16, AC_FULLW32)

#define SW_TDDC_REG_01          u4SWReg(0x4B04)  
    #define TDDC_DEBUG_INDEX        Fld(4, 0, AC_MSKB0)       
    #define TDDC_STILL_DET          Fld(1, 4, AC_MSKB0) 
    #define TDDC_LBOX_DET           Fld(1, 5, AC_MSKB0)  
    #define TDDC_FS_DET             Fld(1, 6, AC_MSKB0)  
    #define TDDC_DC_CAL             Fld(1, 7, AC_MSKB0)    
    #define TDDC_DEBUG_LOG          Fld(1, 8, AC_MSKB1)
    #define TDDC_STATUS_LOG         Fld(1, 9, AC_MSKB1)     
    #define TDDC_TIME_LOG           Fld(1, 10, AC_MSKB1)
	#define TDDC_FS_DET_LOG         Fld(1, 11, AC_MSKB1)     
    #define TDDC_FS_SRC_INV         Fld(1, 12, AC_MSKB1)     
    #define TDDC_FS_SRC_AUTO        Fld(1, 13, AC_MSKB1) 
    #define TDDC_TRIGGER_CHK        Fld(1, 14, AC_MSKB1) 
    #define TDDC_ERR_LOG            Fld(1, 15, AC_MSKB1) 
    #define TDDC_DV_INK             Fld(8, 16, AC_FULLB2) 
    
#define SW_TDDC_REG_02          u4SWReg(0x4B08)  
    #define TDDC_LOCAL_GAIN_UI      Fld(8,  0, AC_FULLB0)     
    #define TDDC_GLOBAL_GAIN_UI     Fld(8,  8, AC_FULLB1)
    #define TDDC_GLOBAL_SFT_UI      Fld(8, 16, AC_FULLB2) 

#define SW_TDDC_REG_03          u4SWReg(0x4B0C)      
    #define TDDC_OCC_THR_LOW        Fld(8,  8, AC_FULLB1)
    #define TDDC_OCC_THR_RANGE      Fld(16, 16, AC_FULLW32)

#define SW_TDDC_REG_04          u4SWReg(0x4B10)  
    #define TDDC_FB_GAIN        	Fld(8,  0, AC_FULLB0)     
    #define TDDC_FB_GAIN_IIR    	Fld(8,  8, AC_FULLB1)
    #define TDDC_NEW_IIR_EN         Fld(1, 16, AC_MSKDW)
    #define TDDC_GLOBAL_IIR_EN      Fld(1, 17, AC_MSKDW)

#define SW_TDDC_REG_05          u4SWReg(0x4B14)  
    #define TDDC_OCC_CNT_L        	Fld(16,  0, AC_FULLW10)     
    #define TDDC_OCC_CNT_R	    	Fld(16, 16, AC_FULLW32)

#define SW_TDDC_REG_06          u4SWReg(0x4B18)  
	#define TDDC_OCC_DV_ERROR		Fld(8,  0, AC_FULLB0)	 
	#define TDDC_OCC_LR_ERROR		Fld(8,	8, AC_FULLB1)
	#define TDDC_OCC_ALL_GLOBAL     Fld(1, 16, AC_MSKDW)
	
#define SW_TDDC_REG_07          u4SWReg(0x4B1C)  
	#define TDDC_FG_WEIGHT			Fld(8,  0, AC_FULLB0)	 
	#define TDDC_BG_WEIGHT			Fld(8,	8, AC_FULLB1)
	#define TDDC_ZDV_PLAN			Fld(16, 16, AC_FULLW32)
    
#define SW_TDDC_REG_08          u4SWReg(0x4B20)  
    #define TOTAL_SHIFT_H           Fld(16,  0, AC_FULLW10)     
    #define FINAL_SHIFT_H           Fld(16,  16, AC_FULLW32)     
    
#define SW_TDDC_REG_09          u4SWReg(0x4B24)  
    #define FSNAVI_FS_THR           Fld(8,  0, AC_FULLB0) 
    #define FSNAVI_FS_GAIN          Fld(8,  8, AC_FULLB1) 
    #define FSNAVI_2D_THR           Fld(8, 16, AC_FULLB2) 
    #define FSNAVI_FS_FCR_THR       Fld(8, 24, AC_FULLB3)     
    
#define SW_TDDC_REG_0A          u4SWReg(0x4B28)  
    #define FSNAVI_2D_FCR_HI_THR    Fld(8,  0, AC_FULLB0) 
    #define FSNAVI_2D_FCR_LO_THR    Fld(8,  8, AC_FULLB1) 
    #define FSNAVI_2D_FCR_SLP_ST    Fld(8, 16, AC_FULLB2) 
    #define FSNAVI_2D_FCR_SLP       Fld(8, 24, AC_FULLB3)     
    
#define SW_TDDC_REG_0B          u4SWReg(0x4B2C)  
    #define FSNAVI_IIR_TH           Fld(8,  0, AC_FULLB0) 
    #define FSNAVI_FS_IIR_RATE      Fld(8,  8, AC_FULLB1) 
    #define FSNAVI_2D_IIR_RATE      Fld(8, 16, AC_FULLB2) 

#define SW_TDDC_REG_0C          u4SWReg(0x4B30)  
    #define STATUS_SEQ              Fld(32, 0, AC_FULLDW)
    
//=============================================================
// Addr : 0x5000 - 0x5FFF
// Name : MJC
//=============================================================
#define MJC_FBCK_00         u4SWReg(0x5000)
    #define FBCK_TRANSITION_MODE    Fld(1, 31, AC_MSKB3) // 31:31
    #define FBCK_MODE               Fld(1, 30, AC_MSKB3) // 30:30
    #define FBCK_SW_EN              Fld(1, 29, AC_MSKB3) // 29
    #define FBCK_OCC_FBK_LVL_MAX    Fld(5, 24, AC_MSKDW) // 28:24
    #define FBCK_CTRL_VDO           Fld(8, 16, AC_FULLB2) // 23:16
    #define FBCK_CTRL               Fld(8, 8, AC_FULLB1) // 15:8
    #define FBCK_DEBUG              Fld(1, 7, AC_MSKB0) // 7
    #define FBCK_LEVEL              Fld(7, 0, AC_MSKB0) // 6:0
#define MJC_FBCK_01         u4SWReg(0x5004)
    #define FBCK_55_MODE            Fld(1, 31, AC_MSKB3) // 31
    #define FBCK_NORM_FORCE_DIV     Fld(1, 30, AC_MSKB3) // 30
    #define CUST_QT_READ            Fld(1, 29, AC_MSKB3) // 29
    #define CUST_QT_RELOAD          Fld(1, 28, AC_MSKB3) // 28
    #define CUST_ADDR               Fld(12, 16, AC_MSKW32) // 27:16
    #define CUST_QT_TBL_IDX         Fld(3, 13, AC_MSKB1) //15:13
    #define CUST_MSB                Fld(5, 8, AC_MSKB1) // 12:8
    #define CUST_TGT_TBL_IDX        Fld(1, 7, AC_MSKB0) // 7
    #define CUST_TGT_MAP_READ       Fld(1, 6, AC_MSKB0) // 6
    #define CUST_TGT_MAP_WRITE      Fld(1, 5, AC_MSKB0) // 5
    #define CUST_LSB                Fld(5, 0, AC_MSKB0) // 4:0
#define MJC_FBCK_02         u4SWReg(0x5008)
    #define CUST_QT_LVL_1           Fld(16, 16, AC_FULLW32) // 31:16
    #define CUST_QT_LVL_0           Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_03         u4SWReg(0x500C)
    #define CUST_QT_LVL_3           Fld(16, 16, AC_FULLW32) // 31:16
    #define CUST_QT_LVL_2           Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_04         u4SWReg(0x5010)
    #define CUST_QT_LVL_5           Fld(16, 16, AC_FULLW32) // 31:16
    #define CUST_QT_LVL_4           Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_05         u4SWReg(0x5014)
    #define CUST_QT_LVL_7           Fld(16, 16, AC_FULLW32) // 31:16
    #define CUST_QT_LVL_6           Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_06         u4SWReg(0x5018)
    #define CUST_QT_LVL_9           Fld(16, 16, AC_FULLW32) // 31:16
    #define CUST_QT_LVL_8           Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_07         u4SWReg(0x501C)
    #define CUST_QT_LVL_B           Fld(16, 16, AC_FULLW32) // 31:16
    #define CUST_QT_LVL_A           Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_08         u4SWReg(0x5020)
    #define CUST_QT_LVL_D           Fld(16, 16, AC_FULLW32) // 31:16
    #define CUST_QT_LVL_C           Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_09         u4SWReg(0x5024)
    #define CUST_QT_LVL_F           Fld(16, 16, AC_FULLW32) // 31:16
    #define CUST_QT_LVL_E           Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_10         u4SWReg(0x5028)
    #define STA_FBCK_TRIBALL_LEVEL  Fld(8, 24, AC_FULLB3) // 31:24
    #define STA_FBCK_TGT_LEVEL      Fld(8, 16, AC_FULLB2) // 23:16
    #define STA_FBCK_RMP_LEVEL      Fld(8, 8, AC_FULLB1) // 15:8
    #define STA_FBCK_LEVEL          Fld(8, 0, AC_FULLB0) // 7:0
#define MJC_FBCK_11         u4SWReg(0x502C)
    #define MVDS_32_00              Fld(5, 0, AC_MSKB0)  // 4:0
    #define MVDS_32_01              Fld(5, 5, AC_MSKW10)  // 9:5
    #define MVDS_32_02              Fld(5, 10, AC_MSKB1)  // 14:10
    #define MVDS_32_03              Fld(5, 15, AC_MSKW21)  // 19:15
    #define MVDS_32_04              Fld(5, 20, AC_MSKW32)  // 24:20
    #define MVDS_32_05              Fld(5, 25, AC_MSKB3)  // 29:25
    #define MVDS_APPLY_DELAY        Fld(2, 30, AC_MSKB3)  // 31:30
#define MJC_FBCK_12         u4SWReg(0x5030)
    #define MVDS_32_10              Fld(5, 0, AC_MSKB0)  // 4:0
    #define MVDS_32_11              Fld(5, 5, AC_MSKW10)  // 9:5
    #define MVDS_32_12              Fld(5, 10, AC_MSKB1)  // 14:10
    #define MVDS_32_13              Fld(5, 15, AC_MSKW21)  // 19:15
    #define CUST_TGT_TBL_TYPE   Fld(1, 21, AC_MSKB2) // 21
    #define DEJUDDER_DBG            Fld(1, 22, AC_MSKB2) // 22
    #define DEBLUR_DBG            Fld(1, 23, AC_MSKB2) // 23
    #define DEJUDR_LVL               Fld(4, 24, AC_MSKB3) // 27:24
    #define DEBLUR_LVL               Fld(4, 28, AC_MSKB3) // 31:28
#define MJC_FBCK_13         u4SWReg(0x5034)
    #define OCC_ADAP_OFF            Fld(1, 31, AC_MSKB3) // 31
    #define OCC_ADAP_OFF_FBCK_TH    Fld(6, 24, AC_MSKB3) // 29:24
    #define FBCK_MTHD_22            Fld(2, 22, AC_MSKB2) // 23:22
    #define FBCK_THR1_2             Fld(6, 16, AC_MSKB2) // 21:16
    #define FBCK_THR2               Fld(6, 10, AC_MSKB1) // 15:10
    #define FBCK_THR1               Fld(6, 4, AC_MSKW10) // 9:4
    #define FBCK_THR_TRIG           Fld(1, 3, AC_MSKB0)  // 3
    #define FBCK_MTHD_32            Fld(3, 0, AC_MSKB0)  // 2:0
			
#define MJC_FBCK_14         u4SWReg(0x5038)
    #define TGT_LVL_4               Fld(8, 24, AC_FULLB3) // 31:24
    #define TGT_LVL_3               Fld(8, 16, AC_FULLB2) // 23:16
    #define TGT_LVL_2               Fld(8, 8, AC_FULLB1) // 15:8
    #define TGT_LVL_1               Fld(8, 0, AC_FULLB0) // 7:0
#define MJC_FBCK_15         u4SWReg(0x503C)
    #define CNT_DELTA_4             Fld(4, 28, AC_MSKB3) // 31:28
    #define CNT_DELTA_3             Fld(4, 24, AC_MSKB3) // 27:24
    #define CNT_DELTA_2             Fld(4, 20, AC_MSKB2) // 23:20
    #define CNT_DELTA_1             Fld(4, 16, AC_MSKB2) // 19:16
    #define CNT_DELTA_0             Fld(4, 12, AC_MSKB1) // 15:12
    #define FILT_MODE               Fld(2, 8, AC_MSKB1)  // 9:8
    #define TGT_LVL_0               Fld(8, 0, AC_FULLB0) // 7:0
#define MJC_FBCK_16         u4SWReg(0x5054)
    #define CNT_TH_HI_0             Fld(16, 16, AC_FULLW32) // 31:16
    #define CNT_TH_LO_0             Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_17         u4SWReg(0x5058)
    #define CNT_TH_HI_1             Fld(16, 16, AC_FULLW32) // 31:16
    #define CNT_TH_LO_1             Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_18         u4SWReg(0x505C)
    #define CNT_TH_HI_2             Fld(16, 16, AC_FULLW32) // 31:16
    #define CNT_TH_LO_2             Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_19         u4SWReg(0x5060)
    #define CNT_TH_HI_3             Fld(16, 16, AC_FULLW32) // 31:16
    #define CNT_TH_LO_3             Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_20         u4SWReg(0x5064)
    #define CNT_TH_HI_4             Fld(16, 16, AC_FULLW32) // 31:16
    #define CNT_TH_LO_4             Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_21         u4SWReg(0x5068)
    #define VAL_TH_HI_0             Fld(16, 16, AC_FULLW32) // 31:16
    #define VAL_TH_LO_0             Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_22         u4SWReg(0x506C)
    #define VAL_TH_HI_1             Fld(16, 16, AC_FULLW32) // 31:16
    #define VAL_TH_LO_1             Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_23         u4SWReg(0x5070)
    #define VAL_TH_HI_2             Fld(16, 16, AC_FULLW32) // 31:16
    #define VAL_TH_LO_2             Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_24         u4SWReg(0x5074)
    #define VAL_TH_HI_3             Fld(16, 16, AC_FULLW32) // 31:16
    #define VAL_TH_LO_3             Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_25         u4SWReg(0x5078)
    #define VAL_TH_HI_4             Fld(16, 16, AC_FULLW32) // 31:16
    #define VAL_TH_LO_4             Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_26         u4SWReg(0x507C)
    #define CNT_TH_MAX              Fld(16, 16, AC_FULLW32) // 31:16
    #define SCALE_FACT              Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_27         u4SWReg(0x5080)
    #define STA_FILT_VAL            Fld(16, 16, AC_FULLW32) // 31:16
    #define STA_NOFILT_VAL          Fld(16, 0, AC_FULLW10) // 15:0
		
#define MJC_FBCK_28         u4SWReg(0x5084)
    #define STA_FBK_NRM_VSI         Fld(16, 16, AC_FULLW32) // 31:16
    #define STA_FBK_BDR_VSI         Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_29         u4SWReg(0x5088)
    #define STA_FBK_MVERR           Fld(16, 16, AC_FULLW32) // 31:16
    #define STA_FBK_BAD_MOTCNT      Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_FBCK_30         u4SWReg(0x508C)
    #define STA_FBK_OSDERR           Fld(16, 16, AC_FULLW32) // 31:16
		
#define MJC_FBCK_31         u4SWReg(0x5090)
    #define STA_FBK_BEC             Fld(16, 16, AC_FULLW32) // 31:16
    #define STA_FBK_CUST            Fld(16, 0, AC_FULLW10) // 15:0
		
#define MJC_FBCK_32         u4SWReg(0x5180)
    #define CUST_TGT_LVL_3          Fld(8, 24, AC_FULLB3) // 31:24
    #define CUST_TGT_LVL_2          Fld(8, 16, AC_FULLB2) // 23:16
    #define CUST_TGT_LVL_1          Fld(8, 8, AC_FULLB1) // 15:8
    #define CUST_TGT_LVL_0          Fld(8, 0, AC_FULLB0) // 7:0
#define MJC_FBCK_33         u4SWReg(0x5184)
    #define CUST_TGT_LVL_7          Fld(8, 24, AC_FULLB3) // 31:24
    #define CUST_TGT_LVL_6          Fld(8, 16, AC_FULLB2) // 23:16
    #define CUST_TGT_LVL_5          Fld(8, 8, AC_FULLB1) // 15:8
    #define CUST_TGT_LVL_4          Fld(8, 0, AC_FULLB0) // 7:0
#define MJC_FBCK_34         u4SWReg(0x5188)
    #define CUST_TGT_LVL_B          Fld(8, 24, AC_FULLB3) // 31:24
    #define CUST_TGT_LVL_A          Fld(8, 16, AC_FULLB2) // 23:16
    #define CUST_TGT_LVL_9          Fld(8, 8, AC_FULLB1) // 15:8
    #define CUST_TGT_LVL_8          Fld(8, 0, AC_FULLB0) // 7:0
#define MJC_FBCK_35         u4SWReg(0x518C)
    #define CUST_TGT_LVL_F          Fld(8, 24, AC_FULLB3) // 31:24
    #define CUST_TGT_LVL_E          Fld(8, 16, AC_FULLB2) // 23:16
    #define CUST_TGT_LVL_D          Fld(8, 8, AC_FULLB1) // 15:8
    #define CUST_TGT_LVL_C          Fld(8, 0, AC_FULLB0) // 7:0
		
#define MJC_FBCK_36         u4SWReg(0x5094)
    #define ERFB_INC_OFST           Fld(4, 28, AC_MSKB3) // 31:28
    #define ERFB_TRIG_WRITE         Fld(1, 27, AC_MSKB3) // 27:27
    #define ERFB_TRIG_READ          Fld(1, 26, AC_MSKB3) // 26:26
    #define ERFB_INC_TH             Fld(10, 16, AC_MSKW32) // 25:16
    #define ERFB_DEC_TH             Fld(10, 0, AC_MSKW10) // 9:0
		
#define MJC_FBCK_37         u4SWReg(0x5098)
    #define FBCK_240_THR0           Fld(6, 22, AC_MSKW32) // 27:22
    #define FBCK_240_THR1_2         Fld(6, 16, AC_MSKB2) // 21:16
    #define FBCK_240_THR2           Fld(6, 10, AC_MSKB1) // 15:10
    #define FBCK_240_THR1           Fld(6, 4, AC_MSKW10) // 9:4
    #define FBCK_240_MTHD_32        Fld(3, 0, AC_MSKB0)  // 2:0
		
#define MJC_FBCK_38         u4SWReg(0x509C)
    #define ERFB_DEC_TH_SLOPE       Fld(8, 24, AC_MSKB3) // 31:24
    #define ERFB_DEC_TH_CURVE_EN    Fld(1, 23, AC_MSKB2) // 23:23
    #define ERFB_DEC_TH_MAX         Fld(10, 10, AC_MSKW21)// 19:10
    #define ERFB_DEC_TH_MIN         Fld(10, 0, AC_MSKW10) // 9:0
		
#define MJC_FBCK_39         u4SWReg(0x50A0)
    #define FBCK_VLRG_SPEED         Fld(4, 16, AC_MSKDW) // 19:16
    #define VLRG_WDV_TH        Fld(6, 8, AC_MSKDW) // 13:8 
    #define FBCK_VLRG_TH         Fld(6, 0, AC_MSKDW) // 5:0		
    #define FBCK_MV_CLIP_MVX_THR   Fld(8, 24, AC_MSKDW) //31:24	
    #define FBCK_MV_CLIP_DBG       Fld(1, 6, AC_MSKDW) // 6:6
    #define FBCK_MV_CLIP_EN        Fld(1, 7, AC_MSKDW) //7:7
		
#define MJC_FBCK_40     u4SWReg(0x50A4)
    #define DIR_MEG_SAD_ERR_TH  Fld(8, 24, AC_MSKDW) // 31:24
    #define DIR_MEG_VSI_ERR_TH  Fld(8, 16, AC_MSKDW) // 23:16
    #define DIR_MOC_CNT_SUM_TH  Fld(16, 0, AC_MSKDW) // 15:0
    
#define MJC_FBCK_41     u4SWReg(0x50A8)
    #define DIR_MV_DSC_EN   Fld(1, 31, AC_MSKDW) // 31
    #define DIR_STILL_OFF_EN   Fld(1, 30, AC_MSKDW) // 30
    #define DIR_SR_OFF_EN    Fld(1, 29, AC_MSKDW) // 29
    #define DIR_OFF_V_BIN_TH_OSMP        Fld(4, 24, AC_MSKDW) // 27:24
    #define DIR_OFF_V_BIN_TH        Fld(4, 20, AC_MSKDW) // 23:20    
    #define DIR_MOV_ERR_SFT Fld(4, 16, AC_MSKDW) // 19:16
    #define DIR_MOV_EDG_CNT_TH  Fld(16, 8, AC_MSKDW) // 15:8
    #define DIR_FBCK_CNT_TH  Fld(8, 0, AC_MSKDW) // 7:0

#define MJC_FBCK_42     u4SWReg(0x50AC)
    #define MJC_FBCK_CLIP   Fld(8, 0, AC_MSKDW) // 7:0
    #define MJC_FBCK_GAIN   Fld(8, 8, AC_MSKDW) // 15:8
    #define MJC_FBCK_GMV_UPBND   Fld(4, 16, AC_MSKDW) // 19:16

#define MJC_FBCK_43     u4SWReg(0x50B0)
    #define MIRR_OFF_LES_SEL_TH     Fld(16, 0, AC_MSKDW) // 15:0
    #define DIR_STILL_OFF_CNT_TH     Fld(16, 16, AC_MSKDW) // 31:16

#define MJC_FBCK_44         u4SWReg(0x50B4)
    #define STA_FBK_OCCERR          Fld(16, 16, AC_FULLW32) // 31:16
    #define STA_FBK_OCCAREA         Fld(16, 0, AC_FULLW10) // 15:0

#define MJC_FBCK_45         u4SWReg(0x50B8)
    #define STA_FBK_OSDCNT         Fld(16, 16, AC_FULLW32) // 31:16
    #define STA_FBK_MVX64_CNT      Fld(16, 0, AC_FULLW10) // 15:0

#define MJC_FBCK_46     u4SWReg(0x50BC)
    #define DIR_OFF_MAJOR_V_TH     Fld(7, 0, AC_MSKDW) // 6:0	
    #define DIR_OFF_HIST_V_TH     Fld(8, 8, AC_MSKDW) // 15:8
    #define FORCE_JUDR_LVL          Fld(8, 16, AC_MSKDW) // 23:16
    #define FORCE_BLUR_LVL          Fld(8, 24, AC_MSKDW) // 31:24	
    
#define MJC_TRIBALL_00      u4SWReg(0x5040)
    #define TRIBALL_FBCK_SPEED      Fld(8, 24, AC_FULLB3) // 31:24
    #define TRIBALL_EN              Fld(1, 20, AC_MSKB2) // 20
    #define TRIBALL_INC_SEL         Fld(4, 16, AC_MSKB2) // 19:16
    #define TRIBALL_FRM_CNT_THR     Fld(8, 8, AC_FULLB1) // 15:8
    #define TRIBALL_UPPER_BOUND     Fld(8, 0, AC_FULLB0) // 7:0
#define MJC_TRIBALL_01      u4SWReg(0x5044)
    #define TRIBALL_MVY_THR         Fld(8, 24, AC_FULLB3) // 31:24
    #define TRIBALL_MVX_THR         Fld(8, 16, AC_FULLB2) // 23:16
    #define TRIBALL_MVCNT_THR       Fld(16, 0, AC_FULLW10) // 15:0
#define MJC_TRIBALL_02      u4SWReg(0x5048)
    #define TRIBALL_FRM_CNT_THR_LO  Fld(8, 0, AC_FULLB0) // 7:0
#define MJC_BADEDIT_00      u4SWReg(0x504C)
    #define BADEDIT_EN              Fld(1, 20, AC_MSKB2) // 20
    #define BADEDIT_UNSTABLE_CNT    Fld(4, 16, AC_MSKB2) // 19:16
    #define BADEDIT_EXIT_TH         Fld(8, 8, AC_FULLB1) // 15:8
    #define BADEDIT_ENTER_TH        Fld(8, 0, AC_FULLB0) // 7:0
#define MJC_ADPTOC_00       u4SWReg(0x5050)
    #define ADPTOC_EN               Fld(1, 17, AC_MSKB2) // 17
    #define ADPTOC_OCCERR_CHK_EN    Fld(1, 16, AC_MSKB2) // 16
    #define ADPTOC_OCCERR_RATIO     Fld(5, 8, AC_MSKB1) // 12:8
    #define ADPTOC_DIFFY_RATIO      Fld(4, 4, AC_MSKB0) // 7:4
    #define ADPTOC_DIFFX_RATIO      Fld(4, 0, AC_MSKB0) // 3:0

#define MJC_MISC_00                 u4SWReg(0x5100)
    #define MJC_FLMR_CHG_TRIG               Fld( 1, 30, AC_MSKDW) // [30:30]
    #define MJC_PR_MODE_CHG_OFF             Fld( 1, 29, AC_MSKDW) // [29:29]
    #define MJC_SRM_CB_MUTE                 Fld( 1, 28, AC_MSKDW) // [28:28]
    #define MJC_SRM_CB_TRIG                 Fld( 2, 26, AC_MSKDW) // [27:26]
    #define MJC_MODE_CHG_OFF                Fld( 1, 25, AC_MSKDW) // [25:25]
    #define MODE_CHG_MUTE_COUNT             Fld( 8, 16, AC_MSKDW) // [23:16]
    #define MJC_MUTE_COUNT                  Fld( 8,  8, AC_MSKDW) // [15: 8]
    #define MJC_CORE_CLK_SW                 Fld( 2,  6, AC_MSKDW) // [ 7: 6]
    #define MJC_IS_44PULLDOWN               Fld( 1,  5, AC_MSKDW) // [ 5: 5]
    #define MJC_PROG_ME_MODE                Fld( 2,  4, AC_MSKDW) // [ 4: 4]
    #define FORCE_MJC_MUTE_COUNT            Fld( 1,  3, AC_MSKDW) // [ 3: 3]
    #define MJC_MUTE_OFF                    Fld( 1,  2, AC_MSKDW) // [ 2: 2]
    #define SRM_CB_OFF                      Fld( 1,  1, AC_MSKDW) // [ 1: 1]
    #define OUT_YC2YUV_EN                   Fld( 1,  0, AC_MSKDW) // [ 0: 0]
#define MJC_MISC_01                 u4SWReg(0x5104)
    #define MJC_PD_SHIFT_UPDATE_DISABLE     Fld( 1, 23, AC_MSKDW) // [23:23]    
    #define MJC_AUTO_PD_SHIFT_DESABLE       Fld( 1, 22, AC_MSKDW) // [22:22]
    #define MJC_VDO_ME2ME3                  Fld( 1, 21, AC_MSKDW) // [21:21]
    #define MJC_FW_MVEXP_EN                 Fld( 1, 20, AC_MSKDW) // [20:20]
    #define MJC_NEW32_OFF_TRIG              Fld( 1, 19, AC_MSKDW) // [19:19]
    #define MJC_FLG_PROG_FRM_RATE           Fld( 1, 18, AC_MSKDW) // [18:19]
    #define MJC_FLG_DB_RATE                 Fld( 1, 17, AC_MSKDW) // [17:17]
    #define MJC_FLG_NEW32_FBCK              Fld( 1, 16, AC_MSKDW) // [16:16]
    #define MJC_CTRL_PARAM                  Fld( 8,  8, AC_MSKDW) // [15: 8]
    #define VIDEO_TIMING                    Fld( 8,  0, AC_MSKDW) // [ 7: 0]
#define MJC_SW_ME_00                u4SWReg(0x5200)
    #define MJC_LBOX_ME_SA_GMVTH            Fld( 8, 24, AC_MSKDW) // [31:24]
    #define MJC_BND_HLR_MAX_V               Fld( 8, 16, AC_MSKDW) // [23:16]
    #define MJC_BND_HLR_MAX_H               Fld( 8,  8, AC_MSKDW) // [15: 8]
    #define MJC_ME_LBOX_CHG_OFF             Fld( 1,  7, AC_MSKDW) // [ 7: 7]
    #define MJC_ME_SCNCHG_VDO_OFF           Fld( 1,  6, AC_MSKDW) // [ 6: 6]
    #define MJC_ME_TMPR_PNLTY_64_PATCH      Fld( 1,  5, AC_MSKDW) // [ 5: 5]
    #define MJC_ATV4LINE_EN                 Fld( 1,  4, AC_MSKDW) // [ 4: 4]    
    #define MJC_ME_EXD_GMV_MODE             Fld( 1,  3, AC_MSKDW) // [ 3: 3]
    #define MJC_ME_EXD_SR_PATCH             Fld( 1,  2, AC_MSKDW) // [ 2: 2]
    #define MJC_BND_HLR_ADAP_EN             Fld( 1,  1, AC_MSKDW) // [ 1: 1]
    #define MJC_LBOX_ME_SA_MODE             Fld( 1,  0, AC_MSKDW) // [ 0: 0]
#define MJC_SW_ME_01                u4SWReg(0x5204)
    #define MJC_ME_EXD_SR_GMVY_TH           Fld( 8,  0, AC_MSKDW) // [ 7: 0]
    #define MJC_ATV4LINE_SMOOTH_TH          Fld( 8,  8, AC_MSKDW) // [15: 8]
    #define MJC_ATV4LINE_ZERO_TH            Fld( 8, 16, AC_MSKDW) // [23:16]
    #define MJC_ATV4LINE_EDGE_TH            Fld( 8, 24, AC_MSKDW) // [31:24]
#define MJC_SW_ME_02                u4SWReg(0x5208)
    #define MJC_ATV4LINE_APL_TH             Fld( 8,  0, AC_MSKDW) // [ 7: 0]
    #define MJC_ATV4LINE_FRAME_COUNT_TH     Fld( 8,  8, AC_MSKDW) // [15: 8]
    #define MJC_ATV4LINE_FRAME_COUNT_TH_LOW Fld( 8, 16, AC_MSKDW) // [23:16]
    #define MJC_ME_PPC_DAR_TH_DFT           Fld( 8, 24, AC_MSKDW) // [31:24]
#define MJC_SW_ME_03                u4SWReg(0x520C)
    #define MJC_ATV4LINE_SWAPL              Fld( 8, 24, AC_MSKDW) // [31:24]
    #define MJC_ATV4LINE_FLAG               Fld( 1, 16, AC_MSKDW) // [16:16]
    #define MJC_ATV4LINE_LBOX_BLOCKSUM      Fld(16,  0, AC_MSKDW) // [15: 0]
#define MJC_SW_ME_04                u4SWReg(0x5210)
    #define MJC_AVG_LES                     Fld(16,  0, AC_MSKDW) // [15: 0]
    #define MJC_AVG_MINVAR                  Fld(16, 16, AC_MSKDW) // [31:16]
#define MJC_SW_ME_05                u4SWReg(0x5214)
    #define MJC_ADAP_BEC_VAR_SFT            Fld( 4, 28, AC_MSKDW) // [31:28]
    #define MJC_ADAP_BEC_SAD_TH_MAX         Fld(12, 16, AC_MSKDW) // [27:16]
    #define MJC_ADAP_BEC_VAR_GAIN           Fld( 4, 12, AC_MSKDW) // [15:12]
    #define MJC_ADAP_BEC_SAD_TH_MIN         Fld(12,  0, AC_MSKDW) // [11: 0]
#define MJC_SW_ME_06                u4SWReg(0x5218)
    #define MJC_STA_GMV_X                   Fld( 8, 24, AC_MSKDW) // [31:24]
    #define MJC_STA_GMV_Y                   Fld( 8, 16, AC_MSKDW) // [23:16]
    #define MJC_STA_MAJ_X                   Fld( 8,  8, AC_MSKDW) // [15: 8]
    #define MJC_STA_MAJ_Y                   Fld( 8,  0, AC_MSKDW) // [ 7: 0]
#define MJC_SW_ME_07                u4SWReg(0x521C)
    #define MJC_ME_LBOX_CHG_CNT_UP          Fld( 8, 24, AC_MSKDW) // [31:24]
    #define MJC_ME_LBOX_CHG_CNT_HI          Fld( 8, 16, AC_MSKDW) // [23:16]
    #define MJC_ME_LBOX_CHG_CNT_LO          Fld( 8,  8, AC_MSKDW) // [15: 8]
    #define MJC_ME_LBOX_CHG_DIF_TH          Fld( 8,  0, AC_MSKDW) // [ 7: 0]
#define MJC_SW_ME_08                u4SWReg(0x5220)
    #define STA_MJC_AVG_ZEROSAD             Fld(16,  0, AC_MSKDW) // [15: 0]
    #define STA_MJC_ME_STA_TOTALBLK         Fld(16, 16, AC_MSKDW) // [31:16]
#define MJC_SW_ME_09                u4SWReg(0x5224)
    #define MJC_ME_STILL_CLR_MV 	        Fld( 1,  0, AC_MSKDW) // [ 0: 0]
    #define MJC_ME_STILL_CLR_MV_FB	        Fld( 1,  1, AC_MSKDW) // [ 1: 1]
    #define MJC_ME2ME3_PATCH_EN     Fld(1, 2, AC_MSKB0) // 2:2
#define MJC_SW_ME_10                u4SWReg(0x5228)
    #define STA_MJC_IS_MOVLBOX              Fld( 1,  0, AC_MSKDW) // [ 0: 0]
    
#define MJC_SW_MVMC_00              u4SWReg(0x5280)
    #define MJC_PAD_GAIN_INC                Fld( 4, 28, AC_MSKDW) // [31:28]
    #define MJC_PAD_RATIO_DEC               Fld( 4, 24, AC_MSKDW) // [27:24]
    #define MJC_PAD_CNT_MAX_THR             Fld( 8, 16, AC_MSKDW) // [23:16]
    #define MJC_PAD_CNT_HI_THR              Fld( 6, 10, AC_MSKDW) // [15:10]
    #define MJC_PAD_CNT_LO_THR              Fld( 6,  4, AC_MSKDW) // [ 9: 4]
    #define MJC_PAD_MVY_EN                  Fld( 1,  1, AC_MSKDW) // [ 1: 1]
    #define MJC_PAD_FORCE_ON                Fld( 1,  0, AC_MSKDW) // [ 0: 0]
#define MJC_SW_MVMC_01              u4SWReg(0x5284)
    #define MJC_PAD_MVY_THR                 Fld( 8, 24, AC_MSKDW) // [31:24]
    #define MJC_VSI_GAIN_Y                  Fld(8, 16, AC_MSKDW)    // 23:16
    #define MJC_VSI_GAIN_Y_EN               Fld(1, 14, AC_MSKDW)    // 14:14
    #define MJC_VSI_GAIN_Y_TH               Fld(6, 8, AC_MSKDW)     // 13:8
#define MJC_FW_SCH_00               u4SWReg(0x5300)
    #define MJC_RO_FRM_SCH_SW_PD_INFO       Fld( 8, 24, AC_MSKDW) // [31:24]
    #define MJC_RO_FRM_SCH_HW_DET_FILM_TYPE Fld( 4, 12, AC_MSKDW) // [15:12]
    #define MJC_RO_FRM_SCH_SW_DET_FILM_TYPE Fld( 4,  8, AC_MSKDW) // [11: 8]
    #define MJC_RO_FRM_SCH_SW_FILM_TYPE     Fld( 4,  4, AC_MSKDW) // [ 7: 4]
    #define MJC_RO_FRM_SCH_SW_FORCE_VDO     Fld( 1,  3, AC_MSKDW) // [ 3: 3]
    #define MJC_RO_FRM_SCH_SW_SWITCH_LOCK   Fld( 1,  2, AC_MSKDW) // [ 2: 2]
    #define MJC_RO_FRM_SCH_SW_TRIG_EN       Fld( 2,  0, AC_MSKDW) // [ 3: 2]
    #define MJC_RO_FRM_SCH_SW_EN_TRIG       Fld( 1,  1, AC_MSKDW) // [ 1: 1]
    #define MJC_RO_FRM_SCH_SW_EN_CURR       Fld( 1,  0, AC_MSKDW) // [ 0: 0]
#define MJC_FW_SCH_01               u4SWReg(0x5304)
    #define MJC_FW_FRM_SCH_LOG              Fld( 1, 31, AC_MSKDW) // [31:31]
    #define MJC_FW_FRM_SCH_DYSW             Fld( 2, 29, AC_MSKDW) // [30:29]
    #define MJC_FW_FRM_SCH_AUTO_DET         Fld( 5, 24, AC_MSKDW) // [28:24]
    #define MJC_FW_FRM_SCH_3D_MODE          Fld( 1, 23, AC_MSKDW) // [23:23]
    #define MJC_FW_FRM_SCH_WRT_PTR_CTRL     Fld( 1, 22, AC_MSKDW) // [22:22]
    #define MJC_FW_FRM_SCH_LOG_SW           Fld( 1, 21, AC_MSKDW) // [21:21]
    #define MJC_FW_FRM_SCH_LOG_HW           Fld( 1, 20, AC_MSKDW) // [20:20]
    #define MJC_FW_FRM_SCH_TST_DLY          Fld( 4, 16, AC_MSKDW) // [19:16]
    #define MJC_FW_FRM_SCH_PD_INFO_DLY      Fld( 4, 12, AC_MSKDW) // [15:12]
    #define MJC_FW_FRM_SCH_IN_DLY           Fld( 4,  8, AC_MSKDW) // [11: 8]
    #define MJC_FW_FRM_SCH_1X               Fld( 1,  7, AC_MSKDW) // [ 7: 7]
    #define MJC_FW_FRM_SCH_DBG_LOG          Fld( 1,  6, AC_MSKDW) // [ 6: 6]
    #define MJC_FW_FRM_SCH_SWITCH           Fld( 2,  4, AC_MSKDW) // [ 5: 4]
    #define MJC_FW_FRM_SCH_BWRDC_EN         Fld( 1,  3, AC_MSKDW) // [ 3: 3]
    #define MJC_FW_FRM_SCH_CTRL_IDX         Fld( 1,  2, AC_MSKDW) // [ 2: 2]
    #define MJC_FW_FRM_SCH_CTRL_EN          Fld( 1,  1, AC_MSKDW) // [ 1: 1]
    #define MJC_FW_FRM_SCH_ISR_EN           Fld( 1,  0, AC_MSKDW) // [ 0: 0]
#define MJC_FW_SCH_02               u4SWReg(0x5308)
    #define MJC_FW_FRM_SW_SHIFT32           Fld( 3, 17, AC_MSKDW) // [19: 17]
    #define MJC_FW_FRM_SW_SHIFT22           Fld( 1, 16, AC_MSKDW) // [16: 16]
    #define MJC_FW_FRM_SCH_PD_R             Fld( 8,  8, AC_MSKDW) // [15: 8]
    #define MJC_FW_FRM_SCH_PD_L             Fld( 8,  0, AC_MSKDW) // [ 7: 0]
#define MJC_FW_SCH_03               u4SWReg(0x530C)
    #define MJC_UNICADENCE_EN                  Fld( 1,  0, AC_MSKDW) // [ 0: 0]
    #define MJC_UNICADENCE_DOUBLE              Fld( 1,  1, AC_MSKDW) // [ 1: 1]
    #define MJC_UNICADENCE_TTD                 Fld( 1,  2, AC_MSKDW) // [ 2: 2]
    #define MJC_DFI_FOECE_EN                   Fld( 1,  4, AC_MSKDW)// [4:4]
    #define MJC_DFI_EN    Fld( 1, 5, AC_MSKDW) // [5:5]
    #define MJC_DFI_HWFW_SW_EN  Fld( 1, 6, AC_MSKDW) // [6:6]
    #define MJC_DFI_MULTI_CADENCE_EN  Fld( 1, 7, AC_MSKDW) // [7:7]
    #define MJC_DFI_FILM_DLY Fld( 4,  8, AC_MSKDW) // [11: 8]
    #define MJC_DFI_PRE_ENTER_SFT Fld( 4, 12, AC_MSKDW) // [15:12]
    #define MJC_DFI_RESCAN_TH   Fld( 4, 16, AC_MSKDW) // [19:16]
    #define MJC_FRM_SCH_DBG_EN  Fld( 1, 20, AC_MSKDW) // [20:20]
    #define MJC_FRM_SCH_FWHW_SWITCH  Fld( 1, 21, AC_MSKDW) // [21:21]
    #define MJC_FRM_SCH_FWHW_SWITCH_AUTO  Fld( 1, 22, AC_MSKDW) // [22:22]
    #define MJC_FRM_SCH_FWDFI_SWITCH_AUTO  Fld( 1, 23, AC_MSKDW) // [23:23]
    #define MJC_FRM_FWHW_SFT    Fld( 4, 24, AC_MSKDW) // [27:24]
    #define MJC_60TO100_EN 				    Fld( 1,  28,AC_MSKDW) // [28: 28]
    #define MJC_CINEMA_TYPE 			Fld( 3,  29,AC_MSKDW) // [31: 29]
#define MJC_FW_SCH_04               u4SWReg(0x5310)
    #define MJC_UNICADENCE_TBL_IDX             Fld( 8,  0, AC_MSKDW) // [ 7: 0]
    #define MJC_UNICADENCE_WRT_PTR             Fld( 8,  8, AC_MSKDW) // [15: 8]
    #define MJC_UNICADENCE_LR3D_OUTPUT         Fld( 1, 16, AC_MSKDW) // [16:16]
    #define MJC_UNICADENCE_FILM                Fld( 1, 17, AC_MSKDW) // [17:17]
    #define MJC_UNICADENCE_BUF_IDX             Fld( 4, 20, AC_MSKDW) // [20:23]
    #define MJC_UNICADENCE_CHA_CLK_FLG         Fld( 1, 24, AC_MSKDW) // [24:24]
    #define MJC_UNICADENCE_CHA_CLK_TTDFLG      Fld( 1, 25, AC_MSKDW) // [25:25]
    #define MJC_UNICADENCE_FILM_TYPE           Fld( 3, 26, AC_MSKDW) // [28:26]
#define MJC_FW_SCH_05               u4SWReg(0x5314)
    #define MJC_FW_SCH_BUF_NUM              Fld( 4, 24, AC_MSKDW) // [27:24]
    #define MJC_FW_SCH_LRSEL_OFF            Fld( 1, 18, AC_MSKDW) // [18:18]
    #define MJC_FW_SCH_RESCAN_OFF           Fld( 1, 17, AC_MSKDW) // [17:17]
    #define MJC_FW_SCH_PREBI_OFF            Fld( 1, 16, AC_MSKDW) // [16:16]
    #define MJC_FW_SCH_BWRDC_OFF            Fld( 1, 15, AC_MSKDW) // [15:15]
    #define MJC_FW_SCH_PAIR_FIRST_OFF       Fld( 1, 14, AC_MSKDW) // [14:14]
    #define MJC_FW_SCH_NEXT_RPTR_DLY        Fld( 4, 10, AC_MSKDW) // [13:10]
    #define MJC_FW_SCH_CURR_RPTR_DLY        Fld( 4,  6, AC_MSKDW) // [ 9: 6]
    #define MJC_FW_SCH_PREV_RPTR_DLY        Fld( 4,  2, AC_MSKDW) // [ 5: 2]
    #define MJC_FW_SCH_OSMP_MODE            Fld( 1,  1, AC_MSKDW) // [ 1: 1]
    #define MJC_FW_SCH_RPTR_MSB             Fld( 1,  0, AC_MSKDW) // [ 0: 0]
#define MJC_FW_SCH_06               u4SWReg(0x5320)    
    #define MJC_DFI_EXIT_PD_INFO             Fld( 4, 0, AC_MSKDW) // [3:0]
    #define MJC_DFI_ENTER_PD_INFO           Fld( 4, 4, AC_MSKDW) // [7:4]
    #define MJC_DFI_CNT                             Fld( 8, 8, AC_MSKDW)  // [15:8]
    #define MJC_DFI_SWITCH_PERIOD             Fld( 8, 16, AC_MSKDW)  // [23:16]
    #define MJC_DFI_SW_FRM             Fld( 1, 24, AC_MSKDW)  // [24:24]
    #define MJC_DFI_HW_PRE_SW_EN        Fld( 1, 25, AC_MSKDW)  // [25:25]
    #define MJC_DFI_HW_PRE_CNT        Fld( 4, 26, AC_MSKDW)  // [29:26]
#define MJC_FW_SCH_07               u4SWReg(0x5324)      
    #define STA_DFI_ENTER                       Fld( 1, 31, AC_MSKDW)  // [31:31]
    #define STA_DFI_EXIT_VDO_CNT          Fld( 8, 0, AC_MSKDW)  // [7:0]
#define MJC_FW_SCH_08               u4SWReg(0x5318)
    #define MJC_UNICADENCE_TRIGER_CNT       Fld( 8, 24, AC_MSKDW)// [31:24]

    

#define CSS_CTRL_00                 u4SWReg(0x5800)
    #define CSS_BOUND_UPPER                 Fld( 8, 24, AC_MSKDW) // [31:24]
    #define CSS_BOUND_LOWER                 Fld( 8, 16, AC_MSKDW) // [23:16]
    #define CSS_FILTER_TAP                  Fld( 8,  8, AC_MSKDW) // [15: 8]
    #define CSS_USE_MAJOR_MV                Fld( 1,  3, AC_MSKDW) // [ 3: 3]
    #define CSS_DIRECT_COMP                 Fld( 1,  2, AC_MSKDW) // [ 2: 2]
    #define CSS_UPDATE_GMV_TAB_AT_NUM1      Fld( 1,  1, AC_MSKDW) // [ 1: 1]
    #define CSS_UPDATE_AT_NUM1              Fld( 1,  0, AC_MSKDW) // [ 0: 0]
	

//=============================================================
// Addr : 0x6000 - 0x6FFF
// Name : DBC
//=============================================================
#define DBC_00 u4SWReg(0x6000)
    #define DBC_ENA Fld(1,0,AC_MSKB0)
    #define DBC_PS_ENA Fld(1,1,AC_MSKB0)
    #define DBC_Debug_ENA Fld(1,2,AC_MSKB0)
    #define DBC_DyanBl_Debug_ENA Fld(1,3,AC_MSKB0)
    #define DBC_Backlight_Ctrl Fld(1,4,AC_MSKB0)
    #define DBC_SCANPWM_Debug_ENA Fld(1,5,AC_MSKB0)
    #define DBC_SCANPWM_Flag Fld(1,6,AC_MSKB0)
    #define DBC_BL_MIDDLE Fld(8,8,AC_MSKB1)
	#define DBC_SCANPWM_PHASE Fld(10,16,AC_FULLW32) 
    
#define DBC_01 u4SWReg(0x6004)
    #define DBC_BL_TOP Fld(8, 0, AC_FULLB0)
    #define DBC_BL_BOTTOM Fld(8, 8, AC_FULLB1)
    #define DBC_BL_PWM_FRQ Fld(8, 16, AC_FULLB2)
    #define DBC_BL_SETUDEN Fld(8, 24, AC_FULLB3)
		
#define DBC_02 u4SWReg(0x6008)
    #define DBC_BL_PSH Fld(8, 0, AC_FULLB0)
    #define DBC_BL_CRI_TEMP_L Fld(8, 8, AC_FULLB1)
    #define DBC_BL_CRI_TEMP_H Fld(8, 16, AC_FULLB2)
	
#define DBC_03 u4SWReg(0x600C)
    #define DBC_APL_LOW_START Fld(8, 0, AC_FULLB0)
    #define DBC_BL_B_LIMIT Fld(8, 8, AC_FULLB1)
    #define DBC_APL_LOW_LIMIT Fld(8, 16, AC_FULLB2)
    #define DBC_TRIGGER_UP Fld(8, 24, AC_FULLB3)
	
#define DBC_04 u4SWReg(0x6010)
    #define DBC_TRIGGER_DOWN Fld(8, 0, AC_FULLB0)
    #define DBC_BOTTOM_LIMIT Fld(8, 8, AC_FULLB1)
    #define DBC_PS_TIME Fld(8, 16, AC_FULLB2)
    #define DBC_PS_LEVEL Fld(8, 24, AC_FULLB3)
		
#define DBC_05 u4SWReg(0x6014)
    #define DBC_PS_APL_TH Fld(8, 0, AC_FULLB0)
    #define DBC_PS_ATN_DN Fld(8, 8, AC_FULLB1)
    #define DBC_PS_ATN_UP Fld(8, 16, AC_FULLB2)
    #define DBC_PS_TM_EXIT Fld(8, 24, AC_FULLB3)
		
#define DBC_06 u4SWReg(0x6018)
    #define DBC_BL_CRI_PWM_FRQ Fld(8, 0, AC_FULLW10)
    #define DBC_BL_CRI_DUTY Fld(8, 8, AC_FULLB1)
	#define DBC_EEI_DARK_TH Fld(16, 16, AC_FULLW32)
#define DBC_07 u4SWReg(0x601C)
    #define DBC_wBasic_blight Fld(16, 0, AC_FULLW10)
    #define DBC_Dynamic_Blight_mode Fld(16, 16, AC_FULLW32)
		
#define DBC_08 u4SWReg(0x6020)
    #define DBC_wPRE_TriggerMODE Fld(16, 0, AC_FULLW10)
    #define DBC_bAPL Fld(16, 16, AC_FULLW32)
	
#define DBC_09 u4SWReg(0x6024)
    #define DBC_wBL_level Fld(8, 0, AC_FULLB0)
    #define DBC_wPower_saving_sta Fld(3, 8, AC_MSKB1)
    #define DBC_wLight_sensor_sta Fld(1, 11, AC_MSKB1)
	#define DBC_EEI_WHITE_TH Fld(16, 16, AC_FULLW32)
	
#define DBC_0A u4SWReg(0x6028)
    #define DBC_wDimmerOut Fld(16, 0, AC_FULLW10)
    #define DBC_Final_BL_Limit Fld(8, 16, AC_FULLB2)
	#define DBC_EEI_ON Fld(1, 24, AC_MSKB3)
#define DBC_0B u4SWReg(0x602C)
    #define DBC_TCONTemp Fld(32, 0, AC_FULLDW)
		
#define DBC_0C u4SWReg(0x6030)
    #define DBC_PS_HIS_START Fld(8, 0, AC_FULLB0)
    #define DBC_PS_TriggerMODE Fld(8, 8, AC_FULLB1)
    #define DBC_PS_STA Fld(8, 16, AC_FULLB2)
    #define DBC_Signal_STA Fld(8, 24, AC_FULLB3)
		
#define DBC_0D u4SWReg(0x6034)
    #define DBC_PS_InCount Fld(16, 0, AC_FULLW10)
    #define DBC_PS_OutCount Fld(16, 16, AC_FULLW32)       
		
#define DBC_0E u4SWReg(0x6038)
    #define Panel_min_dim Fld(8, 0, AC_FULLB0)
    #define LightSensorValue Fld(8, 8, AC_FULLB1)
    #define LightSensorFactor Fld(16, 16, AC_FULLW32)       
		
#define DBC_0F u4SWReg(0x603C)
    #define LightSensorADC Fld(8, 0, AC_FULLB0)
    #define LightSensorSpeed Fld(8, 8, AC_FULLB1)
    #define ThermalSensorSpeed Fld(8, 16, AC_FULLB2)
    #define DBC_EEI_STEP Fld(8, 24, AC_FULLB3)		
#define DBC_10 u4SWReg(0x6040)
    #define M_TEMP_TH_LOW Fld(8, 0, AC_FULLB0)
    #define M_TEMP_TH_HIGH Fld(8, 8, AC_FULLB1)
    #define M_TEMP_BL_BOTTOM Fld(8, 16, AC_FULLB2)
    #define M_TEMP_BL_TGT Fld(8, 24, AC_FULLB3)
		
#define DBC_11 u4SWReg(0x6044)
    #define M_TEMP_BL_LIMIT Fld(8, 0, AC_FULLB0)
    #define M_NORMAL_BRIGHT_CONT Fld(8, 8, AC_FULLB1)
    #define M_TEMP_BL_CNT Fld(8, 16, AC_FULLB2)
    #define M_TEMP_BL_CNTTH Fld(8, 24, AC_FULLB3)
		
#define DBC_12 u4SWReg(0x6048)
    #define M_TEMP_BL_INC Fld(8, 0, AC_FULLB0)
    #define M_TEMP_SENSOR_VALUE Fld(8, 8, AC_FULLB1)
    #define M_TEMP_SENSOR_AVGVALUE Fld(8, 16, AC_FULLB2)
		
#define DBC_13 u4SWReg(0x604C)
    #define M_EMG_STA Fld(8, 0, AC_FULLB0)
    #define M_TEMP_SENSOR_ENA Fld(1,8,AC_MSKB1)
    #define M_UI_BACKLIGHT_STA Fld(1,9,AC_MSKB1)
    #define M_UI_POWERLEVEL_ENA Fld(1,10,AC_MSKB1)
    #define M_TEMP_DL_CNT Fld(8, 16, AC_FULLB2)
    #define DBC_EEI_INCREASE Fld(8, 24, AC_FULLB3)
#define DBC_14 u4SWReg(0x6050)
    #define EEI_DARK_Count Fld(16, 0, AC_FULLW10)
    #define EEI_WHITE_Count Fld(16, 16, AC_FULLW32)  
#define DBC_15 u4SWReg(0x6054)
    #define DBC_EEI_INCREASE_RF Fld(8, 0, AC_FULLB0)
    #define DBC_EEI_INCREASE_YP Fld(8, 8, AC_FULLB1)
    #define DBC_EEI_DECREASE_NORMAL Fld(8, 16, AC_FULLB2)
	#define DBC_EEI_INCREASE_NORMAL Fld(8, 24, AC_FULLB3)
#define DBC_16 u4SWReg(0x6058)
    #define DBC_EEI_INCREASE_DTV Fld(8, 0, AC_FULLB0)
#define DBC_17 u4SWReg(0x605C)
    #define DBC_CRI_DARK_TH Fld(16, 0, AC_FULLW10)
	#define DBC_CRI_DARK_COUNT Fld(16, 16, AC_FULLW32)
#define DBC_18 u4SWReg(0x6060)
    #define DBC_OSD_FLAG Fld(1,0,AC_MSKB0)	
	//=============================================================
	// Addr : 0x6F00
	// Name :Customer gamma
	//=============================================================
#define CUST_GAMMA_01 u4SWReg(0x6F00)
    #define CUST_GAMMA_PBM_ON Fld(1,0,AC_MSKB0) //0

//=============================================================
// Addr : 0x7000 - 0x7FFF
// Name : NR / Deblocking
//=============================================================
#define NR_BASIC_MODE_00 u4SWReg(0x7000)
	#define NR_BASIC_MODE_ENABLE Fld(1,0,AC_MSKB0) //0
	
#define NR_BASIC_MODE_01 u4SWReg(0x7004)
	#define NR_MOS_AREA_STRENGTH Fld(6,24,AC_MSKB2) //29:24
	#define NR_EDGE_AREA_STRENGTH Fld(6,16, AC_MSKW21) //21:16
	#define NR_MESS_AREA_STRENGTH Fld(6, 8, AC_MSKW10) //13:8
	#define NR_SM_AREA_STRENGTH Fld(6, 0, AC_MSKB0) //5:0
	
#define NR_BASIC_MODE_02 u4SWReg(0x7008)
	#define NR_PREFILTER_BLENDING Fld(4,0,AC_MSKB0) //3:0

#define NR_BASIC_MODE_03 u4SWReg(0x700C)
	#define NR_ColorDis_EN Fld(1,0,AC_MSKB0) //0	
	#define NR_ROUNDING_ON_PERIOD Fld(8,8,AC_MSKB0) //15:8
	#define NR_ROUNDING_OFF_PERIOD Fld(8,16,AC_MSKB0) //23:16

//for 5365/5395 Noise meter
#define NM_STATUS_00    u4SWReg(0x7014)
    #define NM_NOISE_LEVEL              Fld(16, 16, AC_FULLW32) // 31:16
    #define NM_FINAL_CONFIDENCE         Fld(16, 0, AC_FULLW10) // 15:0
#define NM_STATUS_01    u4SWReg(0x7018)
    #define NM_MOPXL_CONFIDENCE         Fld(16, 16, AC_FULLW32) // 31:16
    #define NM_DSRATIO_CONFIDENCE       Fld(16, 0, AC_FULLW10) // 15:0  
#define NM_STATUS_03    u4SWReg(0x7020)    
    #define NM_DSRATIO                  Fld(16,16,AC_FULLW32) //31:16
    #define NM_MOPXLRATIO               Fld(8,8,AC_FULLB1)  //15:8
    #define NM_DSSTABLE                 Fld(1,2,AC_MSKB0)   //2
    #define NM_MOPXLSTABLE              Fld(1,1,AC_MSKB0)   //1    
#define NM_STATUS_05    u4SWReg(0x7028)    
    #define NM_NDSRATIO                 Fld(16,16,AC_FULLW32) //31:16
    #define NM_APLW                     Fld(8,8,AC_FULLB1)  //15:8    
#define NM_LUT_00       u4SWReg(0x7030)    
    #define NM_NL1_UPBOUND              Fld(16, 16, AC_FULLW32) // 31:16
    #define NM_NL0_UPBOUND              Fld(16, 0, AC_FULLW10) // 15:0
#define NM_LUT_01       u4SWReg(0x7034)    
    #define NM_NL3_UPBOUND              Fld(16, 16, AC_FULLW32) // 31:16
    #define NM_NL2_UPBOUND              Fld(16, 0, AC_FULLW10) // 15:0
#define NM_LUT_02       u4SWReg(0x7038)    
    #define NM_NL5_UPBOUND              Fld(16, 16, AC_FULLW32) // 31:16
    #define NM_NL4_UPBOUND              Fld(16, 0, AC_FULLW10) // 15:0    
#define NM_LUT_03       u4SWReg(0x703C)    
    #define NM_NL7_UPBOUND              Fld(16, 16, AC_FULLW32) // 31:16
    #define NM_NL6_UPBOUND              Fld(16, 0, AC_FULLW10) // 15:0    
    
#define NM_LUT_04       u4SWReg(0x7040)    
    #define NM_MOPLX_LUT_XTURN1         Fld(16, 16, AC_FULLW32) // 31:16
    #define NM_MOPLX_LUT_XTURN2         Fld(16, 0, AC_FULLW10) // 15:0    
#define NM_LUT_05       u4SWReg(0x7044)    
    #define NM_MOPLX_LUT_YSTART         Fld(16, 16, AC_FULLW32) // 31:16
    #define NM_MOPLX_LUT_YEND           Fld(16, 0, AC_FULLW10) // 15:0    
#define NM_LUT_06       u4SWReg(0x7048)    
    #define NM_DSR_LUT_XTURN1           Fld(16, 16, AC_FULLW32) // 31:16
    #define NM_DSR_LUT_XTURN2           Fld(16, 0, AC_FULLW10) // 15:0    
#define NM_LUT_07       u4SWReg(0x704C)    
    #define NM_DSR_LUT_YSTART           Fld(16, 16, AC_FULLW32) // 31:16
    #define NM_DSR_LUT_YEND             Fld(16, 0, AC_FULLW10) // 15:0       
#define NM_LUT_08       u4SWReg(0x7050)    
    #define NM_APLW_LUT_XTURN1         Fld(16, 16, AC_FULLW32) // 31:16
    #define NM_APLW_LUT_XTURN2         Fld(16, 0, AC_FULLW10) // 15:0    
#define NM_LUT_09       u4SWReg(0x7054)    
    #define NM_APLW_LUT_YSTART         Fld(16, 16, AC_FULLW32) // 31:16
    #define NM_APLW_LUT_YEND           Fld(16, 0, AC_FULLW10) // 15:0    

    
#define NM_CFG_00       u4SWReg(0x7070)    
    #define NM_DS_NORM_BASE             Fld(8,8,AC_FULLB1)  //15:8
    #define NM_DS_NORM_EN               Fld(1,3,AC_MSKB0)   //3
    #define NM_TRIGGER                  Fld(1,0,AC_MSKB0) // 0    
        
//=============================================================
// Addr : 0x7100 - 0x75FF
// Name : AUTO NR 
//================================

#define AUTO_NR_OFFSET_PER_LEVEL 0x100

#define NR_AUTO_GBL_00_LV0 u4SWReg(0x7100)
#define NR_AUTO_GBL_00_LV1 u4SWReg(0x7100+AUTO_NR_OFFSET_PER_LEVEL*1)
#define NR_AUTO_GBL_00_LV2 u4SWReg(0x7100+AUTO_NR_OFFSET_PER_LEVEL*2)
#define NR_AUTO_GBL_00_LV3 u4SWReg(0x7100+AUTO_NR_OFFSET_PER_LEVEL*3)
#define NR_AUTO_GBL_00_LV4 u4SWReg(0x7100+AUTO_NR_OFFSET_PER_LEVEL*4)    
#define NR_AUTO_GBL_00_LV5 u4SWReg(0x7100+AUTO_NR_OFFSET_PER_LEVEL*5)    
#define NR_AUTO_GBL_00_LV6 u4SWReg(0x7100+AUTO_NR_OFFSET_PER_LEVEL*6)    
#define NR_AUTO_GBL_00_LV7 u4SWReg(0x7100+AUTO_NR_OFFSET_PER_LEVEL*7)    
//    #define NR_AUTO_FORCE_DEF       Fld(1,0,AC_MSKB0) //Removed
    #define NR_AUTO_DEF_TBL_TH      Fld(4,4,AC_MSKB0) 
    #define NR_AUTO_REDUCE_SAT      Fld(8,8,AC_MSKB1)   
    #define NR_AUTO_HSHARP_CORING   Fld(8,16,AC_MSKB2)   
    #define NR_AUTO_MOEDGE_TBL_TH   Fld(4,24,AC_MSKB3) 
    
#define NR_AUTO_LCSHARP_0 u4SWReg(0x7104)
#define NR_AUTO_LCSHARP_1 u4SWReg(0x7104+AUTO_NR_OFFSET_PER_LEVEL*1)    
#define NR_AUTO_LCSHARP_2 u4SWReg(0x7104+AUTO_NR_OFFSET_PER_LEVEL*2)    
#define NR_AUTO_LCSHARP_3 u4SWReg(0x7104+AUTO_NR_OFFSET_PER_LEVEL*3)    
#define NR_AUTO_LCSHARP_4 u4SWReg(0x7104+AUTO_NR_OFFSET_PER_LEVEL*4)
#define NR_AUTO_LCSHARP_5 u4SWReg(0x7104+AUTO_NR_OFFSET_PER_LEVEL*5)
#define NR_AUTO_LCSHARP_6 u4SWReg(0x7104+AUTO_NR_OFFSET_PER_LEVEL*6)
#define NR_AUTO_LCSHARP_7 u4SWReg(0x7104+AUTO_NR_OFFSET_PER_LEVEL*7)

//    #define NR_AUTO_REDUCE_LCSHARP_LOW Fld(8,0,AC_MSKB0) //0    Removed
    #define NR_AUTO_REDUCE_LCSHARP_MID Fld(8,8,AC_MSKB1) //0    
    #define NR_AUTO_REDUCE_LCSHARP_HIGH Fld(8,16,AC_MSKB2) //0    
           
#define NR_AUTO_DEF_TBL_LV0 u4SWReg(0x7108)
#define NR_AUTO_DEF_TBL_LV1 u4SWReg(0x7108+AUTO_NR_OFFSET_PER_LEVEL*1)
#define NR_AUTO_DEF_TBL_LV2 u4SWReg(0x7108+AUTO_NR_OFFSET_PER_LEVEL*2)
#define NR_AUTO_DEF_TBL_LV3 u4SWReg(0x7108+AUTO_NR_OFFSET_PER_LEVEL*3)
#define NR_AUTO_DEF_TBL_LV4 u4SWReg(0x7108+AUTO_NR_OFFSET_PER_LEVEL*4)
#define NR_AUTO_DEF_TBL_LV5 u4SWReg(0x7108+AUTO_NR_OFFSET_PER_LEVEL*5)
#define NR_AUTO_DEF_TBL_LV6 u4SWReg(0x7108+AUTO_NR_OFFSET_PER_LEVEL*6)
#define NR_AUTO_DEF_TBL_LV7 u4SWReg(0x7108+AUTO_NR_OFFSET_PER_LEVEL*7)
    #define NR_AUTO_TBL0 Fld(4,0,AC_MSKB0)   
    #define NR_AUTO_TBL1 Fld(4,4,AC_MSKB0)   
    #define NR_AUTO_TBL2 Fld(4,8,AC_MSKB1)   
    #define NR_AUTO_TBL3 Fld(4,12,AC_MSKB1)   
    #define NR_AUTO_TBL4 Fld(4,16,AC_MSKB2)    
    #define NR_AUTO_TBL5 Fld(4,20,AC_MSKB2)    
    #define NR_AUTO_TBL6 Fld(4,24,AC_MSKB3)    
    #define NR_AUTO_TBL7 Fld(4,28,AC_MSKB3)

#define NR_AUTO_MO_EDGE_TBL_LV0 u4SWReg(0x710C)
#define NR_AUTO_MO_EDGE_TBL_LV1 u4SWReg(0x710C+AUTO_NR_OFFSET_PER_LEVEL*1)
#define NR_AUTO_MO_EDGE_TBL_LV2 u4SWReg(0x710C+AUTO_NR_OFFSET_PER_LEVEL*2)
#define NR_AUTO_MO_EDGE_TBL_LV3 u4SWReg(0x710C+AUTO_NR_OFFSET_PER_LEVEL*3)
#define NR_AUTO_MO_EDGE_TBL_LV4 u4SWReg(0x710C+AUTO_NR_OFFSET_PER_LEVEL*4)
#define NR_AUTO_MO_EDGE_TBL_LV5 u4SWReg(0x710C+AUTO_NR_OFFSET_PER_LEVEL*5)
#define NR_AUTO_MO_EDGE_TBL_LV6 u4SWReg(0x710C+AUTO_NR_OFFSET_PER_LEVEL*6)
#define NR_AUTO_MO_EDGE_TBL_LV7 u4SWReg(0x710C+AUTO_NR_OFFSET_PER_LEVEL*7)
    #define NR_AUTO_TBL0 Fld(4,0,AC_MSKB0)   
    #define NR_AUTO_TBL1 Fld(4,4,AC_MSKB0)   
    #define NR_AUTO_TBL2 Fld(4,8,AC_MSKB1)   
    #define NR_AUTO_TBL3 Fld(4,12,AC_MSKB1)   
    #define NR_AUTO_TBL4 Fld(4,16,AC_MSKB2)    
    #define NR_AUTO_TBL5 Fld(4,20,AC_MSKB2)    
    #define NR_AUTO_TBL6 Fld(4,24,AC_MSKB3)    
    #define NR_AUTO_TBL7 Fld(4,28,AC_MSKB3)

#define NR_AUTO_NR_STA  u4SWReg(0x7900)	
    #define NR_AUTO_FORCE_LV0 Fld(1,31,AC_MSKB3)
    #define NR_AUTO_METER_EN Fld(1,30,AC_MSKB3)       
    #define NR_AUTO_CUR_LV Fld(4,0,AC_MSKB0)
            
#define NR_AUTO_CONTROL u4SWReg(0x7904)
    #define NR_AUTO_STRENGTH Fld(8, 0, AC_FULLB0) //7:0  




//=============================================================
// Addr : 0x7A00 - 0x7AFF
// Name : Auto NR and Motion NR (version 1)
//================================

#define NR_AUTO_00    u4SWReg(0x7A00)
    #define NR_BLUR_LVL       Fld(8,24,AC_MSKDW) //31
    #define TNR_THR_GAIN      Fld(8,16,AC_MSKDW) //31
    #define SNR_ADP_MAX       Fld(6,8,AC_MSKDW) //31
    #define SNR_ADP_MIN       Fld(6,0,AC_MSKDW) //31    
#define NR_AUTO_01    u4SWReg(0x7A04)
    #define NR_ACDC_STEP      Fld(8,16,AC_MSKDW) //31
    #define NR_MOTION_TH      Fld(8,8,AC_MSKDW) //31
    #define NR_STILL_TH       Fld(8,0,AC_MSKDW) //31  
#define NR_AUTO_02    u4SWReg(0x7A08) 
    #define SNR_ADP_MAX_NL_TH Fld(5,0,AC_MSKDW) //4:0
    #define SNR_GAIN_SLOPE    Fld(5,8,AC_MSKDW) //12:8
    #define TNR_THR_SLOPE     Fld(8,16,AC_MSKDW) //23:16
    #define SNR_ADP_MIN_NL_TH Fld(5,24,AC_MSKDW) //28:24
#define NR_AUTO_03    u4SWReg(0x7A0C)
    #define LPF_LEVEL_MIN     Fld(8,0,AC_MSKDW) //4:0
    #define LPF_LEVEL_MAX     Fld(8,8,AC_MSKDW) //4:0
    #define LPF_GAIN          Fld(8,16,AC_MSKDW) //4:0
#define NR_AUTO_04    u4SWReg(0x7A10)
    #define BNR_GAIN          Fld(8,8,AC_MSKDW)//[15:8]
    #define BNR_STILL_GAIN    Fld(8,16,AC_MSKDW)//[23:16]
    #define BNR_GOOD_ALIAS_DIFF_TH Fld(8,24,AC_MSKDW)//[31:24]
#define NR_AUTO_05    u4SWReg(0x7A14)
    #define u1BNRCnt          Fld(8,0,AC_MSKDW)//[7:0]
    #define u1BNR_tar_gain    Fld(8,8,AC_MSKDW)//[15:8]
    #define BNR_STILL_TH      Fld(16,16,AC_MSKDW)//[31:16]
#define NR_AUTO_06    u4SWReg(0x7A18)
    #define TNR_GAIN_MIN_NL   Fld(6,0,AC_MSKDW)//[5:0]
    #define TNR_GAIN_MAX_NL   Fld(6,8,AC_MSKDW)//[13:8]
    #define BNR_HDMI_SEL      Fld(1,15,AC_MSKDW)//[15:15]
    #define BNR_ALIAS_CNT_TH  Fld(8,16,AC_MSKDW)//[24:16]  
    
#define NR_MO_00     u4SWReg(0x7A50)
    #define MONR_MOTION_LEVEL_TH1 Fld(8,0,AC_MSKDW)//[7:0]
    #define MONR_MOTION_LEVEL_TH2 Fld(8,8,AC_MSKDW)//[15:8]
    #define MOMNR_EDGE_TH_MO  Fld(4,24,AC_MSKDW)//[27:24]
    #define MOMNR_EDGE_TH_ST  Fld(4,28,AC_MSKDW)//[31:28]
#define NR_MO_01     u4SWReg(0x7A54)
    #define MONR_CORING_MO    Fld(8,0,AC_MSKDW)//[7:0]
    #define MONR_CORING_ST    Fld(8,8,AC_MSKDW)//[15:8]
    #define MONR_IIR_FACTOR   Fld(8,16,AC_MSKDW)//[23:16]
    #define MONR_TH8_INC_MAX  Fld(8,24,AC_MSKDW)//[31:24]
#define NR_MO_02     u4SWReg(0x7A58)
    #define MONR_TH8             Fld(8,0,AC_MSKDW)//[7:0]
    #define MONR_HSHARP_EN       Fld(1,15,AC_MSKDW)//[15:15]
    #define MONR_SUBWEI_DEC_MAX  Fld(6,24,AC_MSKDW)//[29:24]
    #define MONR_HSHARP_BY_MO_EN Fld(1,30,AC_MSKDW)//[30:30]
    #define MOTNR_BY_NL_EN       Fld(1,31,AC_MSKDW)//[31:31]
#define NR_MO_03    u4SWReg(0x7A5C)
    #define MONR_HSHARP_TH_DIFF  Fld(4,0,AC_MSKDW)//[3:0]
    
#define NR_MO_STA_00 u4SWReg(0x7C00)
    #define STA_FINAL_STR        Fld(5,8,AC_MSKDW)//[13:8]
#define NR_MO_STA_01 u4SWReg(0x7C04)
    #define STA_MONR_HOMOSUM     Fld(24,0,AC_MSKDW)//[23:0]
    #define STA_MO_IDX           Fld(5,24,AC_MSKDW)//[28:24] 
    #define STA_BNR_STILL        Fld(1,31,AC_MSKDW)//[31] 



#if 0
#define SNRSM_CTRL_00 u4SWReg(0x7A00)
#define SNRSM_ALMOST_TOTAL_CNT Fld(8, 24, AC_FULLB3) //31:24
#define SNRSM_SIG_LVL Fld(8, 16, AC_FULLB3) //23:16
	#define SNRSM_CTRL_00_ENABLE Fld(1, 0, AC_MSKB0) //0:0
	#define SNRSM_CTRL_00_ENABLE_MSK (1<<0)
	#define SNRSM_CTRL_00_UPDATE_STA_SWREG Fld(1, 1, AC_MSKB0) //1:1
	#define SNRSM_CTRL_00_UPDATE_STA_SWREG_MSK (1<<1)
	#define SNRSM_CTRL_00_TRIG_APPLY_SWREG Fld(1, 2, AC_MSKB0) //2:2
	#define SNRSM_CTRL_00_TRIG_APPLY_SWREG_MSK (1<<2)
#define SNRSM_STA_00 u4SWReg(0x7A20)
	#define SNRSM_STA_DIFF_CM0 Fld(8, 24, AC_FULLB3) //31:24
	#define SNRSM_STA_CM00 Fld(8, 16, AC_FULLB2) //23:16
	#define SNRSM_STA_CM01 Fld(8, 8, AC_FULLB1) //15:8
	#define SNRSM_STA_CM02 Fld(8, 0, AC_FULLB0) //7:0
#define SNRSM_STA_01 (SNRSM_STA_00+4*1)
	#define SNRSM_STA_DIFF_CM1 Fld(8, 24, AC_FULLB3) //31:24
	#define SNRSM_STA_CM10 Fld(8, 16, AC_FULLB2) //23:16
	#define SNRSM_STA_CM11 Fld(8, 8, AC_FULLB1) //15:8
	#define SNRSM_STA_CM12 Fld(8, 0, AC_FULLB0) //7:0
#define SNRSM_STA_02 (SNRSM_STA_00+4*2)
	#define SNRSM_STA_DIFF_CM2 Fld(8, 24, AC_FULLB3) //31:24
	#define SNRSM_STA_CM20 Fld(8, 16, AC_FULLB2) //23:16
	#define SNRSM_STA_CM21 Fld(8, 8, AC_FULLB1) //15:8
	#define SNRSM_STA_CM22 Fld(8, 0, AC_FULLB0) //7:0
#define SNRSM_STA_03 (SNRSM_STA_00+4*3)
	#define SNRSM_STA_VALCM00 Fld(8, 16, AC_FULLB2) //23:16
	#define SNRSM_STA_VALCM01 Fld(8, 8, AC_FULLB1) //15:8
	#define SNRSM_STA_VALCM02 Fld(8, 0, AC_FULLB0) //7:0
#define SNRSM_STA_04 (SNRSM_STA_00+4*4)
	#define SNRSM_STA_VALCM10 Fld(8, 16, AC_FULLB2) //23:16
	#define SNRSM_STA_VALCM11 Fld(8, 8, AC_FULLB1) //15:8
	#define SNRSM_STA_VALCM12 Fld(8, 0, AC_FULLB0) //7:0
#define SNRSM_STA_05 (SNRSM_STA_00+4*5)
	#define SNRSM_STA_VALCM20 Fld(8, 16, AC_FULLB2) //23:16
	#define SNRSM_STA_VALCM21 Fld(8, 8, AC_FULLB1) //15:8
	#define SNRSM_STA_VALCM22 Fld(8, 0, AC_FULLB0) //7:0
#define SNRSM_STA_06 (SNRSM_STA_00+4*6)
	#define SNRSM_STA_LEFTBIN Fld(8, 24, AC_FULLB3) //31:24
	#define SNRSM_STA_IDXMAX0 Fld(8, 16, AC_FULLB2) //23:16
	#define SNRSM_STA_IDXMAX1 Fld(8, 8, AC_FULLB1) //15:8
	#define SNRSM_STA_IDXMAX2 Fld(8, 0, AC_FULLB0) //7:0
#define SNRSM_STA_07 (SNRSM_STA_00+4*7)
	#define SNRSM_STA_RIGHTBIN Fld(8, 24, AC_FULLB3) //31:24
	#define SNRSM_STA_IDXMIN0 Fld(8, 16, AC_FULLB2) //23:16
	#define SNRSM_STA_IDXMIN1 Fld(8, 8, AC_FULLB1) //15:8
	#define SNRSM_STA_IDXMIN2 Fld(8, 0, AC_FULLB0) //7:0
#define SNRSM_STA_08 (SNRSM_STA_00+4*8)
	#define SNRSM_STA_DM0 Fld(8, 16, AC_FULLB2) //23:16
	#define SNRSM_STA_DM1 Fld(8, 8, AC_FULLB1) //15:8
	#define SNRSM_STA_DM2 Fld(8, 0, AC_FULLB0) //7:0
#define SNRSM_STA_09 (SNRSM_STA_00+4*9)
	#define SNRSM_STA_FILL_H_0 Fld(8, 16, AC_FULLB2) //23:16
	#define SNRSM_STA_SPARSE_H_0 Fld(8, 8, AC_FULLB1) //15:8
	#define SNRSM_STA_SIG_H_0 Fld(8, 0, AC_FULLB0) //7:0
#define SNRSM_STA_0A (SNRSM_STA_00+4*10)
	#define SNRSM_STA_FILL_H_1 Fld(8, 16, AC_FULLB2) //23:16
	#define SNRSM_STA_SPARSE_H_1 Fld(8, 8, AC_FULLB1) //15:8
	#define SNRSM_STA_SIG_H_1 Fld(8, 0, AC_FULLB0) //7:0
#define SNRSM_STA_0B (SNRSM_STA_00+4*11)
	#define SNRSM_STA_FILL_H_2 Fld(8, 16, AC_FULLB2) //23:16
	#define SNRSM_STA_SPARSE_H_2 Fld(8, 8, AC_FULLB1) //15:8
	#define SNRSM_STA_SIG_H_2 Fld(8, 0, AC_FULLB0) //7:0
#define SNRSM_STA_0C (SNRSM_STA_00+4*12)
	#define SNRSM_STA_PATDET Fld(32, 0, AC_FULLDW) //31:0
#define SNRSM_STA_0D (SNRSM_STA_00+4*13)
    #define SNRSM_STA_DET4 Fld(8, 24, AC_FULLB3) //31:24
	#define SNRSM_STA_DET3 Fld(8, 16, AC_FULLB2) //23:16
	#define SNRSM_STA_DET2 Fld(8, 8, AC_FULLB1) //15:8
	#define SNRSM_STA_DET1 Fld(8, 0, AC_FULLB0) //7:0
#define SNRSM_STA_0E (SNRSM_STA_00+4*14)
    #define SNRSM_STA_DET8 Fld(8, 24, AC_FULLB3) //31:24
	#define SNRSM_STA_DET7 Fld(8, 16, AC_FULLB2) //23:16
	#define SNRSM_STA_DET6 Fld(8, 8, AC_FULLB1) //15:8
	#define SNRSM_STA_DET5 Fld(8, 0, AC_FULLB0) //7:0
#define SNRSM_STA_0F (SNRSM_STA_00+4*15)
	#define SNRSM_STA_DET9 Fld(8, 0, AC_FULLB0) //7:0
#endif

//=============================================================
// Addr : 0x8000 - 0x8FFF
// Name : NR / Deblocking
//=============================================================
#define NR_CTRL_00  u4SWReg(0x8000)
    #define EN_READ_NXNR_SRAM   Fld(1,  3, AC_MSKDW) //3:3
    #define EN_SLOW_ERFB        Fld(1,  4, AC_MSKDW) //4:4
    #define EN_USE_CENTROID     Fld(1,  5, AC_MSKDW) //5:5
    #define EN_GROUP_HIST       Fld(1,  6, AC_MSKDW) //6:6
    #define EN_SUPPORT_HBLK     Fld(1,  7, AC_MSKDW) //7:7
    #define EN_TRIG_COPY_LS     Fld(1,  8, AC_MSKDW) //8:8
    #define EN_BNM_ISR_MOADJ    Fld(1,  9, AC_MSKDW) //9:9
    #define EN_CLEAN_DF_DCR     Fld(1, 11, AC_MSKDW) //11:11
    #define EN_PATCH_8BIT       Fld(1, 14, AC_MSKDW) //14:14
    #define EN_BLK_READ_RIGHT   Fld(1, 15, AC_MSKDW) //15:15
    
    #define EN_NOISELEVEL       Fld(1, 16, AC_MSKDW) //16:16
    #define EN_AUTO_XNR         Fld(1, 17, AC_MSKDW) //17:17
    #define EN_ADAP_WEIGHT      Fld(1, 18, AC_MSKDW) //18:18
    #define EN_ADAP_SNR_THM     Fld(1, 19, AC_MSKDW) //19:19
    #define EN_ADAP_SNR_BLK     Fld(1, 20, AC_MSKDW) //20:20
    #define EN_RNR_TH_ADAP      Fld(1, 21, AC_MSKDW) //21:21
    #define EN_BNR_RNR_ADAP     Fld(1, 22, AC_MSKDW) //22:22 
    #define EN_MOTION_ANR       Fld(1, 23, AC_MSKDW) //23:23

    #define EN_COPY_LINE_SUM    Fld(1, 25, AC_MSKDW) //25:25
    #define EN_TEST_AUTO_BLK    Fld(1, 26, AC_MSKDW) //26:26
    #define EN_USE_OLD_ADAPSNR  Fld(1, 28, AC_MSKDW) //28:28
    #define EN_LOG_ANR          Fld(1, 29, AC_MSKDW) //29:29
    #define EN_LOG_SCNCHG       Fld(1, 30, AC_MSKDW) //30:30
    #define EN_ADAP_NR          Fld(1, 31, AC_MSKDW) //31:31
    
#define NR_CTRL_01     u4SWReg(0x8004)
    #define NR_ME_IS_FHDP       Fld(1,  0, AC_MSKDW) //0:0
    #define EN_FORCE_MA_SC      Fld(1,  1, AC_MSKDW) //1:1
    #define ANR_SEL             Fld(2,  8, AC_MSKDW) //8:8
#define NR_CTRL_02     u4SWReg(0x8008)
    #define EN_NOISELEVEL_SW    Fld(1, 16, AC_MSKDW) //16:16
    #define EN_ADAP_WEIGHT_SW   Fld(1, 18, AC_MSKDW) //18:18
    #define EN_ADAP_SNR_THM_SW  Fld(1, 19, AC_MSKDW) //19:19
    #define EN_RNR_TH_ADAP_SW   Fld(1, 21, AC_MSKDW) //21:21
    #define EN_MOTION_ANR_SW    Fld(1, 23, AC_MSKDW) //23:23

#define NR_NM_00    u4SWReg(0x8100)
    #define HIST_TEMP_IIR_SUBSAMPLE Fld(8, 24, AC_MSKDW) //31:24
    #define HIST_TEMP_IIR_WEI       Fld(4, 20, AC_MSKDW) //23:20
    #define HIST_DBG_MODE           Fld(2, 18, AC_MSKDW) //19:18
    #define PEAK_BIN_OPT            Fld(1, 17, AC_MSKDW) //17:17
    #define FORCE_MV_APPROVE        Fld(1, 16, AC_MSKDW) //16:16
    #define SNR_THM_IIR_WEI         Fld(6, 10, AC_MSKDW) //15:10
    #define FORCE_NOISE_LVL_EN      Fld(1, 7, AC_MSKDW) //7:7
    #define FORCE_NOISE_LVL         Fld(6, 0, AC_MSKDW) //5:0
#define NR_NM_01    u4SWReg(0x8104)
#define NR_NM_02    u4SWReg(0x8108)
#define NR_NM_03    u4SWReg(0x810C)
    #define SNR_THM_GAIN                Fld(8, 0, AC_MSKDW) //7:0
    #define MAX_NOISE_LVL               Fld(6, 8, AC_MSKDW) //13:8
    #define USE_RIGHTBIN_POSSIBLE       Fld(1, 14, AC_MSKDW) //14:14
    #define UPDATE_NL_SC                Fld(1, 15, AC_MSKDW) //15:15
    #define RST_SC_HIST_IIR             Fld(1, 16, AC_MSKDW) //16:16
    #define RST_SC_NL                   Fld(1, 17, AC_MSKDW) //17:17
    #define RST_SC_ERFB                 Fld(1, 18, AC_MSKDW) //18:18
    #define FAVOR_MCNR_MODE             Fld(1, 19, AC_MSKDW) //19:19
    #define FORCE_DI_FAVOR_CS_LVL_EN    Fld(1, 25, AC_MSKDW)//25:25
    #define ERFB_MODE                   Fld(1, 26, AC_MSKDW) //26:26
    #define FORCE_DI_FAVOR_CS_LVL       Fld(4, 28, AC_MSKDW)//31:28
#define NR_NM_04    u4SWReg(0x8110)
    #define CLEAN_DF_LVL_PDSUM_DIFF_TH  Fld(12,0,AC_MSKDW)//[11:0]
    #define CLEAN_DF_LVL_ENTER_TH       Fld(4,12,AC_MSKDW)//[15:12]
    #define SNR_THM_OFST_TOOL           Fld(8,18,AC_MSKDW) //[25:18]
    #define SNR_THM_OFST                Fld(10,16,AC_MSKDW) //[25:16]
#define NR_NM_05    u4SWReg(0x8114)
    #define SNR_THM_GAIN_A  Fld(12, 0, AC_MSKDW) //[11:0]
    #define SNR_THM_GAIN_B  Fld(20, 12, AC_MSKDW) //[31:12]
#define NR_NM_06    u4SWReg(0x8118)
    #define SNR_THM_OFST_C  Fld(24, 0, AC_MSKDW) //[23:0]
    #define NM_EXT_NL_MODE  Fld(3, 24, AC_MSKDW) //26:24
    #define NM_SC_NL_MODE   Fld(2, 28, AC_MSKDW) //29:28
    #define NM_SC_Q_NL_MODE Fld(2, 30, AC_MSKDW) //31:30
#define NR_NM_07    u4SWReg(0x811C)
    #define NM_SC_HIST_SKIP    Fld(6, 10, AC_MSKDW)//15:10
    #define SNR_THM_SEL        Fld(4, 4, AC_MSKDW)//7:4
#define NR_NM_08    u4SWReg(0x8120)
#define NR_NM_09    u4SWReg(0x8124)
#define NR_NM_0A    u4SWReg(0x8128)
#define NR_NM_0B    u4SWReg(0x812C)
#define NR_NM_0C    u4SWReg(0x8130)
    #define CLEAN_DF_LVL_PDSUM_TH  Fld(16,0,AC_MSKDW)//[15:0]
    #define NM_SCNCHG_DET_TH_MIN   Fld(16,16,AC_MSKDW)//31:16
#define NR_NM_0D    u4SWReg(0x8134)
    #define NM_ERFB_INC                 Fld(16, 0, AC_MSKDW) // 15:0
    #define NM_ERFB_DEC                 Fld(16, 16, AC_MSKDW) // 31:16
#define NR_NM_0E    u4SWReg(0x8138)
    #define NM_SCNCHG_DET_TH_MAX   Fld(24,0,AC_MSKDW) //23:0
    #define NM_SCNCHG_DET_TH_GAIN  Fld(4,24,AC_MSKDW) //27:24
    #define NM_ERFB_INC_OFST       Fld(3,28,AC_MSKDW) //30:28
    #define NM_SCNCHG_DET_EN       Fld(1,31,AC_MSKDW) //31
#define NR_NM_0F    u4SWReg(0x813C)
    #define NM_ANTI_NL_GAIN_THD_LO    Fld(6,0,AC_MSKDW)
    #define NM_ANTI_NL_GAIN_THD_HI    Fld(6,6,AC_MSKDW)
    #define NM_ANTI_NL_GAIN_MAX       Fld(6,12,AC_MSKDW)
    #define NM_ANTI_NL_GAIN_EN        Fld(1,31,AC_MSKDW)
    #define NM_ERFB_SUBSAMPLE         Fld(7,24,AC_MSKDW)
    #define NM_TDF_STA_IIR            Fld(4,20,AC_MSKDW)
    #define NM_ADJ_BY_FATTAIL         Fld(1,18,AC_MSKDW)
    #define NM_STABLIZER_IIR_EN       Fld(1,19,AC_MSKDW)
#define NR_NM_10    u4SWReg(0x8140)
    #define NM_STABLIZER_IIR          Fld(8,0,AC_MSKDW)
    #define NM_ADJ_BY_MOFBK           Fld(1,13,AC_MSKDW)
    #define NM_ADJ_BY_MOFBK_OFST      Fld(5,8,AC_MSKDW)
    #define NM_SCNCHG_DET_FORCE_OFF   Fld(1,14,AC_MSKDW)
#define NR_NM_DBG_00 u4SWReg(0x8180)
    #define NM_DBG_SEL_0 Fld(4,0,AC_MSKDW)
    #define NM_DBG_SEL_1 Fld(4,4,AC_MSKDW)
    #define NM_DBG_SEL_2 Fld(4,8,AC_MSKDW)
    #define NM_DBG_SEL_3 Fld(4,12,AC_MSKDW)

#define NR_BK_METER_00 u4SWReg(0x81C0)
    #define NR_BK_GOOD_THD            Fld(3,0,AC_MSKDW)//2:0
    #define NR_BK_LOCAL_MIN_RTO       Fld(4,4,AC_MSKDW)//7:4
    #define NR_BK_LOCAL_MIN_THD       Fld(8,8,AC_MSKDW)//15:8
    #define NR_BK_GOOD_AVG_MIN_DEN    Fld(8,16,AC_MSKDW)//23:16
    #define NR_BK_FNL_IIR             Fld(8,24,AC_MSKDW)//31:24
#define NR_BK_METER_01 u4SWReg(0x81C4)
    #define NR_BK_NL_MAX_STR          Fld(16,16,AC_MSKDW)//31:16
    #define NR_BK_NL_GAIN             Fld(6,0,AC_MSKDW)//5:0
#define NR_BK_METER_02 u4SWReg(0x81C8)
    #define NR_BK_THR                 Fld(14,0,AC_MSKDW)

#define NR_PQ_00    u4SWReg(0x8200)
    #define NR_TBL_MA_0 Fld(6,26,AC_MSKDW)//[31:26]
    #define NR_TBL_MA_1 Fld(6,20,AC_MSKDW)//[25:20]
    #define NR_TBL_MA_2 Fld(6,14,AC_MSKDW)//[19:14]
    #define NR_TBL_MA_3 Fld(6,8,AC_MSKDW)//[13:8]
    #define NR_TBL_MA_4 Fld(6,2,AC_MSKDW)//[7:2]
#define NR_PQ_01    u4SWReg(0x8204)	
    #define NR_TBL_MA_5 Fld(6,26,AC_MSKDW)//[31:26]
    #define NR_TBL_MA_6 Fld(6,20,AC_MSKDW)//[25:20]
    #define NR_TBL_MA_7 Fld(6,14,AC_MSKDW)//[19:14]
    #define NR_TBL_MC_0 Fld(6,8,AC_MSKDW)//[13:8]
    #define NR_TBL_MC_1 Fld(6,2,AC_MSKDW)//[7:2]
#define NR_PQ_02    u4SWReg(0x8208)	
	#define NR_TBL_MC_2 Fld(6,26,AC_MSKDW)//[31:26]
	#define NR_TBL_MC_3 Fld(6,20,AC_MSKDW)//[25:20]
    #define NR_TBL_MC_4 Fld(6,14,AC_MSKDW)//[19:14]
    #define NR_TBL_MC_5 Fld(6,8,AC_MSKDW)//[13:8]
    #define NR_TBL_MC_6 Fld(6,2,AC_MSKDW)//[7:2]    
#define NR_PQ_03    u4SWReg(0x820C)
    #define NR_TBL_MC_7 Fld(6,26,AC_MSKDW)//[31:26]
    #define NR_TBL_MA_E_0 Fld(6,20,AC_MSKDW)//[25:20]
    #define NR_TBL_MA_E_1 Fld(6,14,AC_MSKDW)//[19:14]
    #define NR_TBL_MA_E_2 Fld(6,8,AC_MSKDW)//[13:8]
    #define NR_TBL_MA_E_3 Fld(6,2,AC_MSKDW)//[7:2] 
#define NR_PQ_04    u4SWReg(0x8210)    
    #define NR_TBL_MA_E_4 Fld(6,26,AC_MSKDW)//[31:26]
    #define NR_TBL_MA_E_5 Fld(6,20,AC_MSKDW)//[25:20]
    #define NR_TBL_MA_E_6 Fld(6,14,AC_MSKDW)//[19:14]
    #define NR_TBL_MA_E_7 Fld(6,8,AC_MSKDW)//[13:8]
#define NR_PQ_05    u4SWReg(0x8214)    
    #define NR_IIR_MAX_WEI      Fld(5,16, AC_MSKDW)//[20:16]
    #define NR_IIR_MAX_WEI_E    Fld(5, 8, AC_MSKDW)//[12:8]
#define NR_PQ_06     u4SWReg(0x8218)
    #define SNR_MAX_RTO_LVL_4   Fld(6,24,AC_MSKDW)//29:24
    #define SNR_MAX_RTO_LVL_3   Fld(6,18,AC_MSKDW)//23:18
    #define SNR_MAX_RTO_LVL_2   Fld(6,12,AC_MSKDW)//17:12
    #define SNR_MAX_RTO_LVL_1   Fld(6,6,AC_MSKDW)//11:6
    #define SNR_MAX_RTO_LVL_0   Fld(6,0,AC_MSKDW)//5:0
#define NR_PQ_07     u4SWReg(0x821C)
    #define NR_TBL_MA_7_CDL Fld(5,27,AC_MSKDW)//[31:27]
    #define NR_TBL_MA_6_CDL Fld(5,22,AC_MSKDW)//[26:22]
    #define NR_TBL_MA_5_CDL Fld(5,17,AC_MSKDW)//[21:17]
    #define NR_TBL_MA_4_CDL Fld(5,12,AC_MSKDW)//[16:12]
    #define MCNR_TBL_TH_CDL Fld(8,0,AC_MSKDW)//7:0
#define NR_PQ_08     u4SWReg(0x8220)
    #define NR_MOVINGTXR_SUB_WEI_BASE_DFT           Fld(6, 0, AC_MSKDW) // 5:0
    #define NR_MOVINGTXR_SUB_WEI_NL_NAG_GAIN_DFT    Fld(2, 6, AC_MSKDW) // 7:6
    #define NR_SNR_MANUAL_AUTO_RATIO                Fld(6,24, AC_MSKDW)//[29:24]
    #define NR_SNR_AUTO_TH_GENERAL_STR              Fld(4,16, AC_MSKDW)//[19:16]
#define NR_PQ_09     u4SWReg(0x8224)
    #define NR_TBL_THD8_HI Fld(8,16,AC_MSKDW)//[23:16]
    #define NR_TBL_THD8 Fld(8,0,AC_MSKDW)//[7:0]
    #define NR_SNR_GAIN Fld(6,8,AC_MSKDW)//[13:8]
#define NR_PQ_0A     u4SWReg(0x8228)
    #define NR_DotCrawl_TH Fld(8,8,AC_MSKDW)//[15:8]
    #define NR_DotCrawl_Level Fld(4,4,AC_MSKDW)//[7:4]
    #define NR_DNR_WA_EN Fld(1,0,AC_MSKDW)//[0]
#define NR_PQ_0B     u4SWReg(0x822C)
	#define SNR_MAX_RTO_LVL_9	Fld(6,24,AC_MSKDW)//29:24
	#define SNR_MAX_RTO_LVL_8	Fld(6,18,AC_MSKDW)//23:18
	#define SNR_MAX_RTO_LVL_7	Fld(6,12,AC_MSKDW)//17:12
	#define SNR_MAX_RTO_LVL_6	Fld(6,6,AC_MSKDW)//11:6
	#define SNR_MAX_RTO_LVL_5	Fld(6,0,AC_MSKDW)//5:0
#define NR_PQ_0C    u4SWReg(0x8230)
    #define NR_TBL_C_0 Fld(6,26,AC_MSKDW)//[31:26]
    #define NR_TBL_C_1 Fld(6,20,AC_MSKDW)//[25:20]
    #define NR_TBL_C_2 Fld(6,14,AC_MSKDW)//[19:14]
    #define NR_TBL_C_3 Fld(6,8,AC_MSKDW)//[13:8]
    #define NR_TBL_C_4 Fld(6,2,AC_MSKDW)//[7:2]
#define NR_PQ_0D    u4SWReg(0x8234)
    #define NR_TBL_C_5 Fld(6,26,AC_MSKDW)//[31:26]
    #define NR_TBL_C_6 Fld(6,20,AC_MSKDW)//[25:20]
    #define NR_TBL_C_7 Fld(6,14,AC_MSKDW)//[19:14]
#define NR_PQ_0E    u4SWReg(0x8238)
    #define NR_TBL_THD_4 Fld(8,24,AC_MSKDW)//[31:24]
    #define NR_TBL_THD_3 Fld(8,16,AC_MSKDW)//[23:16]
    #define NR_TBL_THD_2 Fld(8,8, AC_MSKDW)//[15:8]
    #define NR_TBL_THD_1 Fld(8,0, AC_MSKDW)//[7:0]
#define NR_PQ_0F    u4SWReg(0x823C)
    #define NR_TBL_THD_8 Fld(8,24,AC_MSKDW)//[31:24]
    #define NR_TBL_THD_7 Fld(8,16,AC_MSKDW)//[23:16]
    #define NR_TBL_THD_6 Fld(8,8, AC_MSKDW)//[15:8]
    #define NR_TBL_THD_5 Fld(8,0, AC_MSKDW)//[7:0]
#define NR_PQ_10    u4SWReg(0x8240)
    #define NR_TBL_THD_E_4 Fld(8,24,AC_MSKDW)//[31:24]
    #define NR_TBL_THD_E_3 Fld(8,16,AC_MSKDW)//[23:16]
    #define NR_TBL_THD_E_2 Fld(8,8, AC_MSKDW)//[15:8]
    #define NR_TBL_THD_E_1 Fld(8,0, AC_MSKDW)//[7:0]
#define NR_PQ_11    u4SWReg(0x8244)
    #define NR_TBL_THD_E_8 Fld(8,24,AC_MSKDW)//[31:24]
    #define NR_TBL_THD_E_7 Fld(8,16,AC_MSKDW)//[23:16]
    #define NR_TBL_THD_E_6 Fld(8,8, AC_MSKDW)//[15:8]
    #define NR_TBL_THD_E_5 Fld(8,0, AC_MSKDW)//[7:0]
#define NR_PQ_12    u4SWReg(0x8248)
    #define NR_NL_THD_MIN       Fld(6,  0, AC_MSKDW)//[5:0]
    #define NR_NL_THD_MAX       Fld(6,  8, AC_MSKDW)//[13:8]
    #define SNR_NL_THD_MIN      Fld(6, 16, AC_MSKDW)//[21:16]
    #define SNR_NL_THD_MAX      Fld(6, 24, AC_MSKDW)//[29:24]
#define NR_PQ_13    u4SWReg(0x824C)
    #define BNR_RNR_TH       Fld(8,0,AC_MSKDW)
    #define RNR_MAX_TH       Fld(8,8,AC_MSKDW)
#define NR_PQ_14    u4SWReg(0x8250)
    #define SNR_NL_GAIN_MIN      Fld(6, 0, AC_MSKDW)//[5:0]
    #define SNR_NL_GAIN_MAX      Fld(6, 6, AC_MSKDW)//[11:6]
#define NR_PQ_15    u4SWReg(0x8254)
    #define NR_TBL_C_E_0 Fld(6,26,AC_MSKDW)//[31:26]
    #define NR_TBL_C_E_1 Fld(6,20,AC_MSKDW)//[25:20]
    #define NR_TBL_C_E_2 Fld(6,14,AC_MSKDW)//[19:14]
    #define NR_TBL_C_E_3 Fld(6,8,AC_MSKDW)//[13:8]
    #define NR_TBL_C_E_4 Fld(6,2,AC_MSKDW)//[7:2]
#define NR_PQ_16    u4SWReg(0x8258)
    #define NR_TBL_C_E_5 Fld(6,26,AC_MSKDW)//[31:26]
    #define NR_TBL_C_E_6 Fld(6,20,AC_MSKDW)//[25:20]
    #define NR_TBL_C_E_7 Fld(6,14,AC_MSKDW)//[19:14]
#define NR_PQ_17    u4SWReg(0x825C)  //NR_PQ_15
    #define RNR_AGG_THM_GAIN    Fld(8, 0, AC_MSKDW)//[7:0]
    #define RNR_AGG_THM_OFST    Fld(10, 8, AC_MSKDW)//[17:8]



#define NR_FWBNR_00 u4SWReg(0x8300)
    #define NR_FWBNR_VGRIDMINLINESUMTH Fld(11,21,AC_MSKDW) //[31:21]
    #define NR_FWBNR_VGRIDACTIVETH     Fld(4,17,AC_MSKDW) //[20:17]
    #define NR_FWBNR_VGRIDINACTIVETH   Fld(4,13,AC_MSKDW)  //[16:13]
    #define NR_FWBNR_USE_HW_MINLS_TH   Fld(1,4,AC_MSKDW) // [4:4]
    #define NR_FWBNR_CG_PATCH0         Fld(1,3,AC_MSKDW) // [3:3]
    #define NR_FWBNR_LOC_WRITE         Fld(1,2,AC_MSKDW) // [2:2]
    #define NR_FWBNR_READ_RIGHT        Fld(1,1,AC_MSKDW) // [1:1]
    #define NR_FWBNR_VBLK_IGNORE_NW    Fld(1,0,AC_MSKDW) // [0:0]
#define NR_FWBNR_01 u4SWReg(0x8304)
    #define NR_FWBNR_VBLKLEVELACTIVETH  Fld(10,20,AC_MSKDW) //[29:20]
    #define NR_FWBNR_VBLKLEVELCONFIRMTH Fld(10,10,AC_MSKDW) //[19:10]
    #define NR_FWBNR_VBLKLEVELLEAKAGE   Fld(10,0,AC_MSKDW)  //[9:0]
#define NR_FWBNR_02 u4SWReg(0x8308)
    #define NR_FWBNR_VBLKINSIDEOUTSIDERATIO  Fld(5,27,AC_MSKDW) //[31:27]
    #define NR_FWBNR_VBLKACTIVETRIALTH       Fld(8,19,AC_MSKDW) //[26:19]
    #define NR_FWBNR_VBLKGAUGEINCTH          Fld(4,15,AC_MSKDW)  //[18:15]
    #define NR_FWBNR_VBLKWINDOWTRIALCOUNTTH  Fld(4,11,AC_MSKDW) //[14:11]
    #define NR_FWBNR_VBLKSCORETH             Fld(10,1,AC_MSKDW) //[10:1]
	
#define NR_FWBNR_03 u4SWReg(0x8310)
    #define NR_FWBNR_HGRIDMINLINESUMTH Fld(11,21,AC_MSKDW) //[31:21]
    #define NR_FWBNR_HGRIDACTIVETH     Fld(4,17,AC_MSKDW) //[20:17]
    #define NR_FWBNR_HGRIDINACTIVETH   Fld(4,13,AC_MSKDW)  //[16:13]	
    #define NR_FWBNR_HBLKINSIDEOUTSIDERATIO  Fld(5,8,AC_MSKDW) //[12:8]
    #define NR_FWBNR_HBLKACTIVETRIALTH		 Fld(8,0,AC_MSKDW) //[7:0]
#define NR_FWBNR_04 u4SWReg(0x8314)
    #define NR_FWBNR_HBLKLEVELACTIVETH  Fld(10,20,AC_MSKDW) //[29:20]
    #define NR_FWBNR_HBLKLEVELCONFIRMTH Fld(10,10,AC_MSKDW) //[19:10]
    #define NR_FWBNR_HBLKLEVELLEAKAGE   Fld(10,0,AC_MSKDW)  //[9:0]

#define NR_NMSTA_00 u4SWReg(0x8500)
    #define SADHIST00 Fld(16,16,AC_MSKDW)//[31:16]
    #define SADHIST01 Fld(16,0,AC_MSKDW)//[15:0]
#define NR_NMSTA_01 u4SWReg(0x8504)
    #define SADHIST02 Fld(16,16,AC_MSKDW)//[31:16]
    #define SADHIST03 Fld(16,0,AC_MSKDW)//[15:0]
#define NR_NMSTA_02 u4SWReg(0x8508)
    #define SADHIST04 Fld(16,16,AC_MSKDW)//[31:16]
    #define SADHIST05 Fld(16,0,AC_MSKDW)//[15:0]
#define NR_NMSTA_03 u4SWReg(0x850C)
    #define SADHIST06 Fld(16,16,AC_MSKDW)//[31:16]
    #define SADHIST07 Fld(16,0,AC_MSKDW)//[15:0]
#define NR_NMSTA_04 u4SWReg(0x8510)
    #define SADHIST08 Fld(16,16,AC_MSKDW)//[31:16]
    #define SADHIST09 Fld(16,0,AC_MSKDW)//[15:0]
#define NR_NMSTA_05 u4SWReg(0x8514)
    #define SADHIST10 Fld(16,16,AC_MSKDW)//[31:16]
    #define SADHIST11 Fld(16,0,AC_MSKDW)//[15:0]
#define NR_NMSTA_06 u4SWReg(0x8518)
    #define SADHIST12 Fld(16,16,AC_MSKDW)//[31:16]
    #define SADHIST13 Fld(16,0,AC_MSKDW)//[15:0]
#define NR_NMSTA_07 u4SWReg(0x851C)
    #define SADHIST14 Fld(16,16,AC_MSKDW)//[31:16]
    #define SADHIST15 Fld(16,0,AC_MSKDW)//[15:0]
#define NR_NMSTA_08 u4SWReg(0x8520)
    #define SADHIST16 Fld(16,16,AC_MSKDW)//[31:16]
    #define SADHIST17 Fld(16,0,AC_MSKDW)//[15:0]
#define NR_NMSTA_09 u4SWReg(0x8524)
    #define SADHIST18 Fld(16,16,AC_MSKDW)//[31:16]
    #define SADHIST19 Fld(16,0,AC_MSKDW)//[15:0]
#define NR_NMSTA_0A u4SWReg(0x8528)
    #define SADHIST20 Fld(16,16,AC_MSKDW)//[31:16]
    #define SADHIST21 Fld(16,0,AC_MSKDW)//[15:0]
#define NR_NMSTA_0B u4SWReg(0x852C)
    #define SADHIST22 Fld(16,16,AC_MSKDW)//[31:16]
    #define SADHIST23 Fld(16,0,AC_MSKDW)//[15:0]
#define NR_NMSTA_0C u4SWReg(0x8530)
    #define SADHIST24 Fld(16,16,AC_MSKDW)//[31:16]
    #define SADHIST_CNT Fld(16,0,AC_MSKDW)//[15:0]

#define NR_NMSTA_0D u4SWReg(0x8534)
    #define NR_NM_STA_INK   Fld(32, 0, AC_FULLDW)//31:0
    #define NM_STA_MV_APPROVE       Fld(1, 31, AC_MSKDW)//31
    #define NM_STA_RPRSNT           Fld(1, 30, AC_MSKDW)//30
    #define NM_STA_NOT_FATTAIL      Fld(1, 29, AC_MSKDW)//29
    #define NM_STA_MONO             Fld(1, 28, AC_MSKDW)//28
    #define NM_STA_STILL_TEX        Fld(1, 27, AC_MSKDW)//27
    #define NM_STA_LOW_SEC          Fld(1, 23, AC_MSKDW)//23
    #define NM_STA_LESS_SLOW_SLOP   Fld(1, 22, AC_MSKDW)//22
    #define NM_STA_NOT_LOW_ZERO     Fld(1, 21, AC_MSKDW)//21
    #define NM_STA_LESS_STEEP_SLOP  Fld(1, 15, AC_MSKDW)//15
    #define NM_STA_SYMMETRIC        Fld(1, 14, AC_MSKDW)//14
    #define NM_STA_LOW_ONE          Fld(1, 13, AC_MSKDW)//13
    #define NM_STA_HIGH_ZERO        Fld(1, 12, AC_MSKDW)//12
    #define NM_STA_LVL0             Fld(1, 10, AC_MSKDW)//10
    #define NM_STA_LVL1             Fld(1, 9, AC_MSKDW)//9
    #define NM_STA_VALID            Fld(1, 8, AC_MSKDW)//8
#define NR_NMSTA_0E u4SWReg(0x8538)
    #define NR_NM_STA_NL        Fld(6, 0, AC_MSKDW)//5:0
    #define NR_NM_STA_NLVD      Fld(1, 6, AC_MSKDW)//6
    #define NR_NM_STA_CNTD      Fld(6, 8, AC_MSKDW)//13:8
    #define NR_NM_STA_PEAKMO    Fld(6, 14, AC_MSKDW)//19:14
    #define NR_NM_STA_PEAK      Fld(4, 20, AC_MSKDW)//23:20
    #define NR_NM_STA_MAXPEAK   Fld(4, 24, AC_MSKDW)//27:24
    #define NR_NM_STA_FIRPEAK   Fld(4, 28, AC_MSKDW)//31:28
#define NR_NMSTA_0F u4SWReg(0x853C)
    #define NR_NM_HOMO_SCNCHG         Fld(1,31,AC_MSKDW)//31
    #define NR_NM_STA_TERMINATE       Fld(5,25,AC_MSKDW)//29:25
    #define NR_NM_STA_RTROUGH         Fld(5,20,AC_MSKDW)//24:20
    #define NR_NM_STA_NL_SCNCHG_SORT  Fld(6,14,AC_MSKDW)//19:14
    #define NR_NM_STA_NL_SCNCHG       Fld(6,8,AC_MSKDW)//13:8
    #define NR_NM_STA_NR_CLEAN_DF_LVL Fld(4,0,AC_MSKDW)//3:0
    #define NR_NM_STA_DI_FAVOR_CS_LVL Fld(4,4,AC_MSKDW)//7:4
#define NR_NMSTA_10 u4SWReg(0x8540)
    #define NR_NM_HOMO_SCNHG_TH       Fld(24,0,AC_MSKDW)//23:0
#define NR_NMSTA_11 u4SWReg(0x8544)
    #define NR_PROC_START  Fld(13, 0, AC_MSKDW)
    #define NR_PROC_END    Fld(13, 16, AC_MSKDW)
    #define NR_PROC_DIRTY  Fld(1, 31, AC_MSKDW)
#define NR_NMSTA_12 u4SWReg(0x8548)
    #define NR_VSYNC_PROC_START  Fld(13, 0, AC_MSKDW)
    #define NR_VSYNC_PROC_END    Fld(13, 16, AC_MSKDW)
#define NR_NMSTA_13 u4SWReg(0x854C)
    #define NR_NM_TOTAL_HIST_CNT Fld(20, 0, AC_MSKDW)
    #define NR_NM_INVLD_RTO      Fld(8, 24, AC_MSKDW)
#define NR_NMSTA_14 u4SWReg(0x8550)
    #define NR_NM_STA_NL_TDF_RAW    Fld(6, 24, AC_MSKDW)
    #define NR_NM_STA_IIR_TMPRL_SUM Fld(18,0,AC_MSKDW)
#define NR_NMSTA_15 u4SWReg(0x8554)
    #define NR_NM_STA_IIR_MOPIX Fld(22,0,AC_MSKDW)
#define NR_NMSTA_16 u4SWReg(0x8558)
    #define NR_NM_STA_IIR_TMPRL_CNT Fld(16,0,AC_MSKDW)
    #define NR_NM_STA_PEAKBIN_VAL   Fld(16,16,AC_MSKDW)
#define NR_NMSTA_17 u4SWReg(0x855C)
    #define NR_NM_STA_TAIL_SUM      Fld(22,0,AC_MSKDW)
    #define NR_NM_STA_FATTAIL_RTO   Fld(8,22,AC_MSKDW)
#define NR_NMSTA_18 u4SWReg(0x8560)
    #define NR_NM_STA_MOFBK_RTO     Fld(5,0,AC_MSKDW)

  
#define NR_HBLK_START u4SWReg(0x8600)
#define NR_HBLK_END u4SWReg(0x8E6C)

//=============================================================
// Addr : 0x9000 - 0x9FFF
// Name : Block Status
//=============================================================
#define NR_VBLK_START u4SWReg(0x9000)
#define NR_VBLK_END u4SWReg(0x9EFC)

#define NR_BLK_STA_00 u4SWReg(0x9F00)
    #define NR_HBLK_SUM         Fld(32, 0, AC_FULLDW)
#define NR_BLK_STA_01 u4SWReg(0x9F04)
    #define NR_VBLK_SUM         Fld(32, 0, AC_FULLDW)  
#define NR_BLK_STA_02 u4SWReg(0x9F08)
    #define NR_BLK_STA_HBLK_DATA_SIZE Fld(10,0,AC_MSKDW)
    #define NR_BLK_STA_VBLK_DATA_SIZE Fld(10,10,AC_MSKDW)
#define NR_BLK_STA_03 u4SWReg(0x9F0C)
    #define NR_BLK_READ_START Fld(13, 0, AC_MSKDW)
    #define NR_BLK_READ_END   Fld(13, 16, AC_MSKDW)
    #define NR_BLK_POS_WRITE_OK Fld(1, 29, AC_MSKDW)
    #define NR_BLK_READ_VHOLD Fld(1, 30, AC_MSKDW)
    #define NR_BLK_READ_FAIL  Fld(1, 31, AC_MSKDW)
#define NR_BLK_STA_04 u4SWReg(0x9F10)
    #define NR_BLK_SZ8_BIN0   Fld(32,0,AC_FULLDW)
#define NR_BLK_STA_05 u4SWReg(0x9F14)
    #define NR_BLK_SZ8_BIN1   Fld(32,0,AC_FULLDW)
#define NR_BLK_STA_06 u4SWReg(0x9F18)
    #define NR_BLK_SZ8_BIN2   Fld(32,0,AC_FULLDW)
#define NR_BLK_STA_07 u4SWReg(0x9F1C)
    #define NR_BLK_SZ8_BIN3   Fld(32,0,AC_FULLDW)
#define NR_BLK_STA_08 u4SWReg(0x9F20)
    #define NR_BLK_SZ8_BIN4   Fld(32,0,AC_FULLDW)
#define NR_BLK_STA_09 u4SWReg(0x9F24)
    #define NR_BLK_SZ8_BIN5   Fld(32,0,AC_FULLDW)
#define NR_BLK_STA_0A u4SWReg(0x9F28)
    #define NR_BLK_SZ8_BIN6   Fld(32,0,AC_FULLDW)
#define NR_BLK_STA_0B u4SWReg(0x9F2C)
    #define NR_BLK_SZ8_BIN7   Fld(32,0,AC_FULLDW)
#define NR_BLK_STA_0C u4SWReg(0x9F30)
    #define NR_BLK_BIN0_PHS   Fld(4,0,AC_MSKDW)
    #define NR_BLK_BIN1_PHS   Fld(4,4,AC_MSKDW)
    #define NR_BLK_BIN2_PHS   Fld(4,8,AC_MSKDW)
    #define NR_BLK_BIN3_PHS   Fld(4,12,AC_MSKDW)
    #define NR_BLK_BIN4_PHS   Fld(4,16,AC_MSKDW)
    #define NR_BLK_BIN5_PHS   Fld(4,20,AC_MSKDW)
    #define NR_BLK_BIN6_PHS   Fld(4,24,AC_MSKDW)
    #define NR_BLK_BIN7_PHS   Fld(4,28,AC_MSKDW)    
#define NR_BLK_STA_0D u4SWReg(0x9F34)
    #define NR_BLK_VRTO_0     Fld(8,24,AC_MSKDW)
    #define NR_BLK_VRTO_1     Fld(8,16,AC_MSKDW)
    #define NR_BLK_VRTO_2     Fld(8,8,AC_MSKDW)
    #define NR_BLK_VRTO_3     Fld(8,0,AC_MSKDW)
#define NR_BLK_STA_0E u4SWReg(0x9F38)
    #define NR_BLK_ALIAS_CNT    Fld(8,0,AC_MSKDW)
    #define NR_BLK_GOOD_BLK_CNT Fld(8,8,AC_MSKDW)
    #define NR_BLK_GOOD_STR     Fld(16,16,AC_MSKDW)
#define NR_BLK_STA_0F u4SWReg(0x9F3C)
    #define NR_BLK_FNL_STR      Fld(16,0,AC_MSKDW)
#define NR_BLK_STA_10 u4SWReg(0x9F40)
    #define NR_BNR_PROC_START Fld(13, 0, AC_MSKDW)
    #define NR_BNR_PROC_END   Fld(13, 16, AC_MSKDW)
#define NR_BLK_STA_11 u4SWReg(0x9F44)
    #define NR_STA_HBLK_PIX_CNT Fld(16,16,AC_MSKDW)//[31:16]
    #define NR_STA_VBLK_PIX_CNT Fld(16,0,AC_MSKDW)//[15:0]
#define NR_BLK_STA_12 u4SWReg(0x9F48)
    #define NR_STA_HBLK_AVGLS   Fld(8,8,AC_MSKDW)//[15:8]
    #define NR_STA_VBLK_AVGLS   Fld(8,0,AC_MSKDW)//[7:0]

//=============================================================
// Addr : 0xA000 - 0xAFFF
// Name : Scaler / Panel Related
//=============================================================
#define MMAPP_00  u4SWReg(0xA000)
    #define APP_TYPE Fld(8, 24, AC_FULLB3) //31:24
#define MMAPP_01  u4SWReg(0xA004)
    #define APP_SRC_WIDTH Fld(16, 0, AC_MSKW10) //15:0
    #define APP_SRC_HEIGHT Fld(16, 16, AC_MSKW32) //31:16
#define MMAPP_02  u4SWReg(0xA008)
    #define APP_OUT_WIDTH Fld(16, 0, AC_MSKW10) //15:0
    #define APP_OUT_HEIGHT Fld(16, 16, AC_MSKW32) //31:16
#define MMAPP_03  u4SWReg(0xA00C)
    #define APP_BITRATE Fld(32, 0, AC_MSKDW) //31:0
#define MMAPP_04  u4SWReg(0xA010)
    #define APP_CHANGE_NOTIFY Fld(1, 0, AC_MSKB0) //0

// VDP main    
#define VDP_XDATA_MAIN_00 u4SWReg(0xA200)
    #define MAIN_CHG_AUTO Fld(1, 31, AC_MSKB3) // 31
    #define MAIN_CHG_TRIGGER Fld(1, 30, AC_MSKB3) // 30
    #define MAIN_COLOR_BIT_SEL Fld(2, 28, AC_MSKB3) // 29:28    
    #define MAIN_COLOR_FMT_SEL Fld(2, 26, AC_MSKB3) // 27:26
    #define MAIN_DISPMODE_SEL Fld(3, 23, AC_MSKW32) // 25:23
    #define MAIN_121MAP Fld(1, 22, AC_MSKB2) // 22
    #define MAIN_INFINITE Fld(1, 21, AC_MSKB2) // 21
#define VDP_XDATA_MAIN_01 u4SWReg(0xA204)
    #define MAIN_SRC_REGION_WIDTH Fld(14, 14, AC_MSKDW) //27:14
    #define MAIN_SRC_REGION_XOFST Fld(14, 0, AC_MSKDW) //13:0
#define VDP_XDATA_MAIN_02 u4SWReg(0xA208)
    #define MAIN_SRC_REGION_HEIGHT Fld(14, 14, AC_MSKDW) //27:14
    #define MAIN_SRC_REGION_YOFST Fld(14, 0, AC_MSKDW) //13:0
#define VDP_XDATA_MAIN_03 u4SWReg(0xA20c)
    #define MAIN_OUT_REGION_WIDTH Fld(14, 14, AC_MSKDW) //27:14
    #define MAIN_OUT_REGION_XOFST Fld(14, 0, AC_MSKDW) //13:0
#define VDP_XDATA_MAIN_04 u4SWReg(0xA210)
    #define MAIN_OUT_REGION_HEIGHT Fld(14, 14, AC_MSKDW) //27:14
    #define MAIN_OUT_REGION_YOFST Fld(14, 0, AC_MSKDW) //13:0
#define VDP_XDATA_MAIN_05 u4SWReg(0xA214)
    #define MAIN_OVERSCAN_TOP Fld(14, 14, AC_MSKDW) //27:14
    #define MAIN_OVERSCAN_BOTTOM Fld(14, 0, AC_MSKDW) //13:0
#define VDP_XDATA_MAIN_06 u4SWReg(0xA218)
    #define MAIN_OVERSCAN_LEFT Fld(14, 14, AC_MSKDW) //27:14
    #define MAIN_OVERSCAN_RIGHT Fld(14, 0, AC_MSKDW) //13:0
#define VDP_XDATA_MAIN_07 u4SWReg(0xA21C)
    #define MAIN_NEW_NONL_ENABLE Fld(1, 31, AC_MSKB3) // 31 
    #define MAIN_NEW_NONL_SRC_MIDDLE Fld(14, 14, AC_MSKDW) //27:14
    #define MAIN_NEW_NONL_OUT_MIDDLE Fld(14, 0, AC_MSKDW) //13:0
//FIXME: by Lucky
#define VDP_XDATA_MAIN_08 u4SWReg(0xA220)
    #define MAIN_OVERSCAN_TOP_TEST Fld(14, 14, AC_MSKDW)      //27:14
    #define MAIN_OVERSCAN_BOTTOM_TEST Fld(14, 0, AC_MSKDW)    //13:0
#define VDP_XDATA_MAIN_09 u4SWReg(0xA224)
    #define MAIN_OVERSCAN_LEFT_TEST Fld(14, 14, AC_MSKDW)     //27:14
    #define MAIN_OVERSCAN_RIGHT_TEST Fld(14, 0, AC_MSKDW)     //13:0
#define VDP_XDATA_MAIN_0A u4SWReg(0xA228)
    #define MAIN_OUT_REGION_WIDTH_TEST Fld(14, 14, AC_MSKDW)  //27:14
    #define MAIN_OUT_REGION_XOFST_TEST Fld(14, 0, AC_MSKDW)   //13:0
#define VDP_XDATA_MAIN_0B u4SWReg(0xA22C)
    #define MAIN_OUT_REGION_HEIGHT_TEST Fld(14, 14, AC_MSKDW) //27:14
    #define MAIN_OUT_REGION_YOFST_TEST Fld(14, 0, AC_MSKDW)   //13:0
#define VDP_XDATA_MAIN_0C u4SWReg(0xA230)
    #define MAIN_SRC_REGION_WIDTH_TEST Fld(14, 14, AC_MSKDW)  //27:14
    #define MAIN_SRC_REGION_XOFST_TEST Fld(14, 0, AC_MSKDW)   //13:0
#define VDP_XDATA_MAIN_0D u4SWReg(0xA234)
    #define MAIN_SRC_REGION_HEIGHT_TEST Fld(14, 14, AC_MSKDW) //27:14
    #define MAIN_SRC_REGION_YOFST_TEST Fld(14, 0, AC_MSKDW)   //13:0
#define VDP_XDATA_MAIN_0E u4SWReg(0xA238)
    #define MAIN_TEST_TIME Fld(14, 14, AC_MSKDW)       //27:14
    #define MAIN_TEST_STEP Fld(14, 0, AC_MSKDW)        //13:0
    #define MAIN_TEST_RAND Fld(1, 28, AC_MSKB3)        //28
    #define MAIN_TEST_PAT1 Fld(1, 29, AC_MSKB3)        //29
    #define MAIN_TEST_PAT2 Fld(1, 30, AC_MSKB3)        //30
#define VDP_XDATA_MAIN_0F u4SWReg(0xA23C)
    #define MAIN_TEST_RANDOM Fld(32, 0, AC_FULLDW)       //27:14

// VDP sub    
#define VDP_XDATA_SUB_00 u4SWReg(0xA800)
    #define SUB_CHG_AUTO Fld(1, 31, AC_MSKB3) // 31
    #define SUB_CHG_TRIGGER Fld(1, 30, AC_MSKB3) // 30
    #define SUB_COLOR_BIT_SEL Fld(2, 28, AC_MSKB3) // 29:28    
    #define SUB_COLOR_FMT_SEL Fld(2, 26, AC_MSKB3) // 27:26
    #define SUB_DISPMODE_SEL Fld(2, 23, AC_MSKW32) // 25:23
    #define SUB_121MAP Fld(1, 22, AC_MSKB2) // 22    
#define VDP_XDATA_SUB_01 u4SWReg(0xA804)
    #define SUB_SRC_REGION_WIDTH Fld(14, 14, AC_MSKDW) //27:14
    #define SUB_SRC_REGION_XOFST Fld(14, 0, AC_MSKDW) //13:0
#define VDP_XDATA_SUB_02 u4SWReg(0xA808)
    #define SUB_SRC_REGION_HEIGHT Fld(14, 14, AC_MSKDW) //27:14
    #define SUB_SRC_REGION_YOFST Fld(14, 0, AC_MSKDW) //13:0
#define VDP_XDATA_SUB_03 u4SWReg(0xA80c)
    #define SUB_OUT_REGION_WIDTH Fld(14, 14, AC_MSKDW) //27:14
    #define SUB_OUT_REGION_XOFST Fld(14, 0, AC_MSKDW) //13:0
#define VDP_XDATA_SUB_04 u4SWReg(0xA810)
    #define SUB_OUT_REGION_HEIGHT Fld(14, 14, AC_MSKDW) //27:14
    #define SUB_OUT_REGION_YOFST Fld(14, 0, AC_MSKDW) //13:0
#define VDP_XDATA_SUB_05 u4SWReg(0xA814)
    #define SUB_OVERSCAN_TOP Fld(14, 14, AC_MSKDW) //27:14
    #define SUB_OVERSCAN_BOTTOM Fld(14, 0, AC_MSKDW) //13:0
#define VDP_XDATA_SUB_06 u4SWReg(0xA818)
    #define SUB_OVERSCAN_LEFT Fld(14, 14, AC_MSKDW) //27:14
    #define SUB_OVERSCAN_RIGHT Fld(14, 0, AC_MSKDW) //13:0

//FIXME: by Lucky
#define VDP_XDATA_SUB_08 u4SWReg(0xA820)
    #define SUB_OVERSCAN_TOP_TEST Fld(14, 14, AC_MSKDW)      //27:14
    #define SUB_OVERSCAN_BOTTOM_TEST Fld(14, 0, AC_MSKDW)    //13:0
#define VDP_XDATA_SUB_09 u4SWReg(0xA824)
    #define SUB_OVERSCAN_LEFT_TEST Fld(14, 14, AC_MSKDW)     //27:14
    #define SUB_OVERSCAN_RIGHT_TEST Fld(14, 0, AC_MSKDW)     //13:0
#define VDP_XDATA_SUB_0A u4SWReg(0xA828)
    #define SUB_OUT_REGION_WIDTH_TEST Fld(14, 14, AC_MSKDW)  //27:14
    #define SUB_OUT_REGION_XOFST_TEST Fld(14, 0, AC_MSKDW)   //13:0
#define VDP_XDATA_SUB_0B u4SWReg(0xA82C)
    #define SUB_OUT_REGION_HEIGHT_TEST Fld(14, 14, AC_MSKDW) //27:14
    #define SUB_OUT_REGION_YOFST_TEST Fld(14, 0, AC_MSKDW)   //13:0
#define VDP_XDATA_SUB_0C u4SWReg(0xA830)
    #define SUB_SRC_REGION_WIDTH_TEST Fld(14, 14, AC_MSKDW)  //27:14
    #define SUB_SRC_REGION_XOFST_TEST Fld(14, 0, AC_MSKDW)   //13:0
#define VDP_XDATA_SUB_0D u4SWReg(0xA834)
    #define SUB_SRC_REGION_HEIGHT_TEST Fld(14, 14, AC_MSKDW) //27:14
    #define SUB_SRC_REGION_YOFST_TEST Fld(14, 0, AC_MSKDW)   //13:0
#define VDP_XDATA_SUB_0E u4SWReg(0xA838)
    #define SUB_TEST_TIME Fld(14, 14, AC_MSKDW)       //27:14
    #define SUB_TEST_STEP Fld(14, 0, AC_MSKDW)        //13:0
    #define SUB_TEST_RAND Fld(1, 28, AC_MSKB3)        //28
    #define SUB_TEST_PAT1 Fld(1, 29, AC_MSKB3)        //29
    #define SUB_TEST_PAT2 Fld(1, 30, AC_MSKB3)        //30
#define VDP_SCPQ_01 u4SWReg(0xA83C)
	#define SCPQ_CUST_UPCOEFF Fld(1, 0, AC_MSKB0) //0	
//=============================================================
// Addr : 0xB000 - 0xBFFF
// Name : Demo Mode
//=============================================================

#define DEMO_00    u4SWReg(0xB000)
    #define DEMOMODE_HSTART                 Fld(12, 0, AC_MSKW10)
    #define DEMOMODE_VSTART                 Fld(12, 16, AC_MSKW32)
    #define DEMOMODE_ENABLE                 Fld( 1, 31, AC_MSKB3)
    #define MOVINGEMO_ENABLE                Fld( 1, 30, AC_MSKB3)
    #define SPLIT_ONOFF                     Fld( 1, 29, AC_MSKB3)
    #define REGION_LEFT_RIGHT               Fld( 1, 28, AC_MSKB3)
#define DEMO_01    u4SWReg(0xB004)    
    #define DEMOMODE_WIDTH                Fld(12, 0, AC_MSKW10)
    #define DEMOMODE_HEIGHT               Fld(12, 16, AC_MSKW32)
#define DEMO_02    u4SWReg(0xB008)    
    #define DEMO_MODULES               Fld(8, 0, AC_FULLB0)

    #define DEMO_TDC_EN 		Fld(1,0,AC_MSKB0) // 0
    #define DEMO_MDDI_EN 	Fld(1,1,AC_MSKB0) // 1
    #define DEMO_HSHARP_EN 	Fld(1,2,AC_MSKB0) // 2
    #define DEMO_2DNR_EN 	Fld(1,3,AC_MSKB0) // 3
    #define DEMO_3DNR_EN 	Fld(1,4,AC_MSKB0) // 4
    #define DEMO_TDSHARP_EN Fld(1,5,AC_MSKB0) // 5
    #define DEMO_COLOR_EN 	Fld(1,6,AC_MSKB0) // 6
    #define DEMO_ECTI_EN 	Fld(1,7,AC_MSKB0) // 7
//=============================================================
// Addr : 0xC000 - 0xCFFF
// Name : Scaler
//=============================================================
#define SCALER_DUMP_00 u4SWReg(0xC000)
    #define SCALER_DUMP_EN Fld(1, 0, AC_MSKB0)		// default is 0
#define SCALER_COEF_MAIN_H_IN u4SWReg(0xC004)
#define SCALER_COEF_MAIN_H_OUT u4SWReg(0xC008)
#define SCALER_COEF_MAIN_V_IN u4SWReg(0xC00C)
#define SCALER_COEF_MAIN_V_OUT u4SWReg(0xC010)



//=============================================================
// Addr : 0xD000 - 0xDFFF
// Name : Tool
//=============================================================
#define TOOL_UART_MODE u4SWReg(0xD000)
    #define TOOL_UART_ENTER_T_MODE 		Fld(1, 0, AC_MSKB0)		// default is 0
	#define TOOL_UART_SUPPORT_MODE_CHG  Fld(1, 1, AC_MSKB0)		// default is 1
    #define TOOL_SUPPORT_CUST_NAME      Fld(1, 2, AC_MSKB0)		// default is 1
    #define TOOL_SUPPORT_MEMMAP         Fld(1, 3, AC_MSKB0)		// default is 1

#define TOOL_CUST_NAME u4SWReg(0xD010)

//=============================================================
// Addr : 0xF000 - 0xFFFF
// Name : Audio 
//=============================================================
//CC_AUD_NCSTOOL_SUPPORT
#define AUD_TOOL_CTRL_00    u4SWReg(0xf000)
    //Query
    #define AUD_TOOL_CTRL_TRIG_QUERY_DEC1       Fld(1,0,AC_MSKB0)
    #define AUD_TOOL_CTRL_TRIG_QUERY_DEC2       Fld(1,1,AC_MSKB0)
    #define AUD_TOOL_CTRL_TRIG_QUERY_DEC3       Fld(1,2,AC_MSKB0)
    #define AUD_TOOL_CTRL_TRIG_QUERY_DEC4       Fld(1,3,AC_MSKB0)
    #define AUD_TOOL_CTRL_TRIG_QUERY_DSP        Fld(1,4,AC_MSKB0)
    #define AUD_TOOL_CTRL_TRIG_QUERY_VC         Fld(1,5,AC_MSKB0)
    #define AUD_TOOL_CTRL_TRIG_QUERY_CVC        Fld(1,6,AC_MSKB0)
    //Tone
    #define AUD_TOOL_CTRL_TRIG_TONE_PLAY        Fld(1,8,AC_MSKB1)
    #define AUD_TOOL_CTRL_TRIG_TONE_STOP        Fld(1,9,AC_MSKB1)
    #define AUD_TOOL_CTRL_TRIG_TONE_RESTORE     Fld(1,10,AC_MSKB1)
    #define AUD_TOOL_CTRL_TRIG_TONE_SELECT      Fld(4,11,AC_MSKB1)
    //Bypass
    #define AUD_TOOL_CTRL_ALL_BYPASS            Fld(1,16,AC_MSKB2)
    #define AUD_TOOL_CTRL_EQ_ENABLE             Fld(1,17,AC_MSKB2)
    #define AUD_TOOL_CTRL_BASS_ENABLE           Fld(1,18,AC_MSKB2)
    #define AUD_TOOL_CTRL_TREBLE_ENABLE         Fld(1,19,AC_MSKB2)    
    #define AUD_TOOL_CTRL_LIMITER_ENABLE        Fld(1,20,AC_MSKB2)
    #define AUD_TOOL_CTRL_AVC_ENABLE            Fld(1,21,AC_MSKB2)
    #define AUD_TOOL_CTRL_VS_ENABLE             Fld(1,22,AC_MSKB2)
    #define AUD_TOOL_CTRL_BASSMNG_ENABLE        Fld(1,23,AC_MSKB2)
	#define AUD_TOOL_CTRL_VBASS_ENABLE          Fld(1,24,AC_MSKB2)
	#define AUD_TOOL_CTRL_SPKHEIGHT_ENABLE      Fld(1,25,AC_MSKB2)

#define AUD_TOOL_CTRL_01    u4SWReg(0xf004)
    //Read Trigger Bit
    #define AUD_TOOL_CTRL_TRIG_READ_ALL         Fld(1,0,AC_MSKB0)
    #define AUD_TOOL_CTRL_TRIG_READ_BYPASS      Fld(1,1,AC_MSKB0)
    #define AUD_TOOL_CTRL_TRIG_READ_EQ          Fld(1,2,AC_MSKB0)
    #define AUD_TOOL_CTRL_TRIG_READ_BASS        Fld(1,3,AC_MSKB0)
    #define AUD_TOOL_CTRL_TRIG_READ_TREBLE      Fld(1,4,AC_MSKB0)
    #define AUD_TOOL_CTRL_TRIG_READ_LIMITER     Fld(1,5,AC_MSKB0)
    #define AUD_TOOL_CTRL_TRIG_READ_AVC         Fld(1,6,AC_MSKB0)
    #define AUD_TOOL_CTRL_TRIG_READ_VS          Fld(1,7,AC_MSKB0)
    #define AUD_TOOL_CTRL_TRIG_READ_VOLUME_TBL  Fld(1,8,AC_MSKB1)
    #define AUD_TOOL_CTRL_TRIG_READ_SV          Fld(1,9,AC_MSKB1)
    #define AUD_TOOL_CTRL_TRIG_READ_CV          Fld(1,10,AC_MSKB1)
    #define AUD_TOOL_CTRL_TRIG_READ_EXGAIN      Fld(1,11,AC_MSKB1)
    #define AUD_TOOL_CTRL_TRIG_READ_CHDELAY     Fld(1,12,AC_MSKB1)
    #define AUD_TOOL_CTRL_TRIG_READ_BASSMNG     Fld(1,13,AC_MSKB1)
    #define AUD_TOOL_CTRL_TRIG_READ_VBASS       Fld(1,14,AC_MSKB1)
	#define AUD_TOOL_CTRL_TRIG_READ_SPKHEIGHT   Fld(1,15,AC_MSKB1)

    //Write Trigger Bit
    #define AUD_TOOL_CTRL_TRIG_WRITE_ALL        Fld(1,16,AC_MSKB2)
    #define AUD_TOOL_CTRL_TRIG_WRITE_BYPASS     Fld(1,17,AC_MSKB2)
    #define AUD_TOOL_CTRL_TRIG_WRITE_EQ         Fld(1,18,AC_MSKB2)
    #define AUD_TOOL_CTRL_TRIG_WRITE_BASS       Fld(1,19,AC_MSKB2)
    #define AUD_TOOL_CTRL_TRIG_WRITE_TREBLE     Fld(1,20,AC_MSKB2)
    #define AUD_TOOL_CTRL_TRIG_WRITE_LIMITER    Fld(1,21,AC_MSKB2)
    #define AUD_TOOL_CTRL_TRIG_WRITE_AVC        Fld(1,22,AC_MSKB2)
    #define AUD_TOOL_CTRL_TRIG_WRITE_VS         Fld(1,23,AC_MSKB2)
    #define AUD_TOOL_CTRL_TRIG_WRITE_VOLUME_TBL Fld(1,24,AC_MSKB3)
    #define AUD_TOOL_CTRL_TRIG_WRITE_SV         Fld(1,25,AC_MSKB3)
    #define AUD_TOOL_CTRL_TRIG_WRITE_CV         Fld(1,26,AC_MSKB3)
    #define AUD_TOOL_CTRL_TRIG_WRITE_EXGAIN     Fld(1,27,AC_MSKB3)
    #define AUD_TOOL_CTRL_TRIG_WRITE_CHDELAY    Fld(1,28,AC_MSKB3)
    #define AUD_TOOL_CTRL_TRIG_WRITE_BASSMNG    Fld(1,29,AC_MSKB3)
	#define AUD_TOOL_CTRL_TRIG_WRITE_VBASS      Fld(1,30,AC_MSKB3)
	#define AUD_TOOL_CTRL_TRIG_WRITE_SPKHEIGHT  Fld(1,31,AC_MSKB3)
    
#define AUD_TOOL_VOL_TBL        u4SWReg(0xf008)
//aud.uop.vcq, aud.uop.svt
//~101 element, next is f19C
#define AUD_TOOL_VOL_MASTER     u4SWReg(0xf19c)
    
//aud.uop.sv, aud.uop.svq
#define AUD_TOOL_SRC_GAIN_OTHERS        u4SWReg(0xf1a0)
    #define AUD_TOOL_SRC_GAIN_FLD       Fld(8,0,AC_FULLB0)
#define AUD_TOOL_SRC_GAIN_DTUNER        u4SWReg(0xf1a4)
#define AUD_TOOL_SRC_GAIN_ATUNER        u4SWReg(0xf1a8)
#define AUD_TOOL_SRC_GAIN_SPDIF         u4SWReg(0xf1ac)
#define AUD_TOOL_SRC_GAIN_LINEIN        u4SWReg(0xf1b0)
#define AUD_TOOL_SRC_GAIN_HDMI          u4SWReg(0xf1b4)
#define AUD_TOOL_SRC_GAIN_MEMORY        u4SWReg(0xf1b8)
#define AUD_TOOL_SRC_GAIN_BUFAGT        u4SWReg(0xf1bc)
#define AUD_TOOL_SRC_GAIN_FEEDER        u4SWReg(0xf1c0)
#define AUD_TOOL_SRC_GAIN_MM            u4SWReg(0xf1c4)
    
//aud.dsp.sh 2, aud.dsp.sh 4
#define AUD_TOOL_CH_VOL_C               u4SWReg(0xf1c8)
#define AUD_TOOL_CH_VOL_L               u4SWReg(0xf1cc)
#define AUD_TOOL_CH_VOL_R               u4SWReg(0xf1d0)
#define AUD_TOOL_CH_VOL_LS              u4SWReg(0xf1d4)
#define AUD_TOOL_CH_VOL_RS              u4SWReg(0xf1d8)
#define AUD_TOOL_CH_VOL_CH7             u4SWReg(0xf1dc)
#define AUD_TOOL_CH_VOL_CH8             u4SWReg(0xf1e0)
#define AUD_TOOL_CH_VOL_SUB             u4SWReg(0xf1e4)
#define AUD_TOOL_CH_VOL_LFE             u4SWReg(0xf1e8)
#define AUD_TOOL_CH_VOL_CH9             u4SWReg(0xf1ec)
#define AUD_TOOL_CH_VOL_CH10            u4SWReg(0xf1f0)
    
//aud.uop.eq.set, aud.uop.eq.q
#define AUD_TOOL_EQ_BAND1               u4SWReg(0xf1f4)
    #define AUD_TOOL_EQ_BAND_FLD        Fld(8,0,AC_FULLB0)
#define AUD_TOOL_EQ_BAND2               u4SWReg(0xf1f8)
#define AUD_TOOL_EQ_BAND3               u4SWReg(0xf1fc)
#define AUD_TOOL_EQ_BAND4               u4SWReg(0xf200)
#define AUD_TOOL_EQ_BAND5               u4SWReg(0xf204)
#define AUD_TOOL_EQ_BAND6               u4SWReg(0xf208)
#define AUD_TOOL_EQ_BAND7               u4SWReg(0xf20c)
   
//aud.uop.sbass.bb
//aud.uop.sbass.cb
//aud.uop.sbass.q
#define AUD_TOOL_BASS                   u4SWReg(0xf210)
    #define AUD_TOOL_SBASS_FLD          Fld(8,0,AC_FULLB0)
#define AUD_TOOL_TREBLE                 u4SWReg(0xf214)
    
//aud.uop.limiter.thre
#define AUD_TOOL_LIMITER                u4SWReg(0xf218)
    #define AUD_TOOL_LIMITER_MODE       Fld(8,0,AC_FULLB0)
    #define AUD_TOOL_LIMITER_THRES      Fld(24,8,AC_MSKDW)
    
//aud.uop.a 1, aud.uop.a 3, aud.uop.a 5, aud.uop.a 6
#define AUD_TOOL_AVC_TARGET_LVL         u4SWReg(0xf21c) //signed int
    #define AUD_TOOL_AVC_FLD            Fld(16,0,AC_FULLW10)
#define AUD_TOOL_AVC_MAX_GAIN_UP        u4SWReg(0xf220)
#define AUD_TOOL_AVC_ADJUST_RATE        u4SWReg(0xf224)
#define AUD_TOOL_AVC_UI_ADJUST_RATE     u4SWReg(0xf228)
    
//aud.uop.vscfg 0~6
#define AUD_TOOL_VS_CLARITY             u4SWReg(0xf22c)
#define AUD_TOOL_VS_WIDTH               u4SWReg(0xf230)
#define AUD_TOOL_VS_LRGAIN              u4SWReg(0xf234)
#define AUD_TOOL_VS_CROSSTALK           u4SWReg(0xf238)
#define AUD_TOOL_VS_OUTPUTGAIN          u4SWReg(0xf23c)
#define AUD_TOOL_VS_BASSGAIN            u4SWReg(0xf240)
#define AUD_TOOL_VS_FO                  u4SWReg(0xf244)

//aud.uop.cvg
#define AUD_TOOL_CH_VOL_AUXL            u4SWReg(0xf248)
#define AUD_TOOL_CH_VOL_AUXR            u4SWReg(0xf24c)
#define AUD_TOOL_CH_VOL_AUXLFE          u4SWReg(0xf250)

#define AUD_TOOL_EXTRA_GAIN_00          u4SWReg(0xf254)
    #define AUD_TOOL_EXTRA_GAIN_L       Fld(8,0,AC_FULLB0)
    #define AUD_TOOL_EXTRA_GAIN_R       Fld(8,8,AC_FULLB1)
    #define AUD_TOOL_EXTRA_GAIN_LS      Fld(8,16,AC_FULLB2)
    #define AUD_TOOL_EXTRA_GAIN_RS      Fld(8,24,AC_FULLB3)
    
#define AUD_TOOL_EXTRA_GAIN_01          u4SWReg(0xf258)
    #define AUD_TOOL_EXTRA_GAIN_C       Fld(8,0,AC_FULLB0)
    #define AUD_TOOL_EXTRA_GAIN_SUB     Fld(8,8,AC_FULLB1)
    #define AUD_TOOL_EXTRA_GAIN_CH7     Fld(8,16,AC_FULLB2)
    #define AUD_TOOL_EXTRA_GAIN_CH8     Fld(8,24,AC_FULLB3)

#define AUD_TOOL_EXTRA_GAIN_02          u4SWReg(0xf25c)
    #define AUD_TOOL_EXTRA_GAIN_CH9     Fld(8,0,AC_FULLB0)
    #define AUD_TOOL_EXTRA_GAIN_CH10    Fld(8,8,AC_FULLB1)
    #define AUD_TOOL_EXTRA_GAIN_AUXL    Fld(8,16,AC_FULLB2)
    #define AUD_TOOL_EXTRA_GAIN_AUXR    Fld(8,24,AC_FULLB3)

//aud.uop.spkuop.delaycfg
#define AUD_TOOL_CHDELAY_00           u4SWReg(0xf260)
    #define AUD_TOOL_CHDELAY_L          Fld(8,0,AC_FULLB0)
    #define AUD_TOOL_CHDELAY_R          Fld(8,8,AC_FULLB1)
    #define AUD_TOOL_CHDELAY_LS         Fld(8,16,AC_FULLB2)
    #define AUD_TOOL_CHDELAY_RS         Fld(8,24,AC_FULLB3)

#define AUD_TOOL_CHDELAY_01           u4SWReg(0xf264)
    #define AUD_TOOL_CHDELAY_C          Fld(8,0,AC_FULLB0)
    #define AUD_TOOL_CHDELAY_SUB        Fld(8,8,AC_FULLB1)
    #define AUD_TOOL_CHDELAY_CH7        Fld(8,16,AC_FULLB2)
    #define AUD_TOOL_CHDELAY_CH8        Fld(8,24,AC_FULLB3)

#define AUD_TOOL_CHDELAY_02           u4SWReg(0xf268)
    #define AUD_TOOL_CHDELAY_CH9        Fld(8,0,AC_FULLB0)
    #define AUD_TOOL_CHDELAY_CH10       Fld(8,8,AC_FULLB1)

#define AUD_TOOL_BASS_MANAGEMENT        u4SWReg(0xf26C)
    #define AUD_TOOL_LR_SPKTYPE_LARGE       Fld(1,0,AC_MSKB0)
    #define AUD_TOOL_SLR_SPKTYPE_LARGE      Fld(1,1,AC_MSKB0)
    #define AUD_TOOL_CENTER_SPKTYPE_LARGE   Fld(1,2,AC_MSKB0)
    #define AUD_TOOL_CH910_SPKTYPE_LARGE    Fld(1,3,AC_MSKB0)
    #define AUD_TOOL_SUBWOOFER_ON           Fld(1,4,AC_MSKB0)
    #define AUD_TOOL_BASS_MNG_CUTOFF_FREQ   Fld(16,16,AC_MSKW32)

#define AUD_TOOL_VBASS_BASSGAIN        u4SWReg(0xf270)
		
#define AUD_TOOL_VBASS_OUTPUTGAIN      u4SWReg(0xf274)
			
#define AUD_TOOL_VBASS_CUTOFF_FREQ     u4SWReg(0xf278)
	
#define AUD_TOOL_SPKHEIGHT_BOOSTGAIN   u4SWReg(0xf27C)




//CC_AUD_NCSTOOL_SUPPORT

#endif
