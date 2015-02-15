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
#ifndef _HW_HDMI2_H_
#define _HW_HDMI2_H_
#include "x_hal_5381.h"

//---------------------------PDWNC ---------------------------//
#ifndef CC_UP8032_ATV
#define HDMI_DDC_PAD		(IO_VIRT + 0x28008)
	#define HDMI_DDC_MUX_MASK 	Fld(2, 24, AC_MSKB3) 
#endif


//---------------------------HDMI Rx PHY ---------------------------//
#define IO_HDMI2_RX_BASE 0x60000

#define HDMI2_REG_HDMIRX_CFG0 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x000))
  #define HDMI2_REG_HDMIRX_CFG0_0 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x000))
  #define HDMI2_REG_HDMIRX_CFG0_1 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x001))
  #define HDMI2_REG_HDMIRX_CFG0_2 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x002))
  #define HDMI2_REG_HDMIRX_CFG0_3 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x003))
    #define HDMI2_CFG0_RG_HDMI_CDR_RST Fld(1,31,AC_MSKB3)//[31:31]
    #define HDMI2_CFG0_RG_HDMI_CDR_STOP Fld(1,30,AC_MSKB3)//[30:30]
    #define HDMI2_CFG0_RG_HDMI_ATEST_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define HDMI2_CFG0_RG_HDMI_ANAMON Fld(3,24,AC_MSKB3)//[26:24]
    #define HDMI2_CFG0_RG_HDMI_CH0_CDRLPI Fld(7,17,AC_MSKB2)//[23:17]
    #define HDMI2_CFG0_RG_HDMI_CH1_CDRLPI Fld(7,8,AC_MSKB1)//[14:8]
    #define HDMI2_CFG0_RG_HDMI_CH2_CDRLPI Fld(7,0,AC_MSKB0)//[6:0]
#define HDMI2_REG_HDMIRX_CFG1 (IO_VIRT + (IO_HDMI2_RX_BASE  + 0x004))
  #define HDMI2_REG_HDMIRX_CFG1_0 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x004))
  #define HDMI2_REG_HDMIRX_CFG1_1 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x005))
  #define HDMI2_REG_HDMIRX_CFG1_2 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x006))
  #define HDMI2_REG_HDMIRX_CFG1_3 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x007))
    #define HDMI2_CFG1_RG_HDMI_CKDIV2_RSTB Fld(1,31,AC_MSKB3)//[31:31]
    #define HDMI2_CFG1_RG_HDMI_CKGEN_RSTB Fld(1,30,AC_MSKB3)//[30:30]
    #define HDMI2_CFG1_RG_HDMI_DES_HF_MODE Fld(1,29,AC_MSKB3)//[29:29]
    #define HDMI2_CFG1_RG_HDMI_CKDT_AEN Fld(1,28,AC_MSKB3)//[28:28]
    #define HDMI2_CFG1_RG_HDMI_CKDT_SET Fld(1,27,AC_MSKB3)//[27:27]
    #define HDMI2_CFG1_RG_HDMI_HYBIAS Fld(2,25,AC_MSKB3)//[26:25]
    #define HDMI2_CFG1_RG_HDMI_CLKBUF_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define HDMI2_CFG1_RG_HDMI_D2SBIAS Fld(2,20,AC_MSKB2)//[21:20]
    #define HDMI2_CFG1_RG_HDMI_HY Fld(2,14,AC_MSKB1)//[15:14]
    #define HDMI2_CFG1_RG_HDMI_EQBIAS Fld(2,12,AC_MSKB1)//[13:12]
    #define HDMI2_CFG1_RG_HDMI_FULL_EN Fld(1,11,AC_MSKB1)//[11:11]
    #define HDMI2_CFG1_RG_HDMI_VREF Fld(3,8,AC_MSKB1)//[10:8]
    #define HDMI2_CFG1_RG_HDMI_MHLEN Fld(1,7,AC_MSKB0)//[7:7]
    #define HDMI2_CFG1_RG_HDMI_MHLMODE Fld(1,6,AC_MSKB0)//[6:6]
    #define HDMI2_CFG1_RG_HDMI_FTMODE Fld(1,5,AC_MSKB0)//[5:5]
    #define HDMI2_CFG1_RG_HDMI_PI_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define HDMI2_CFG1_RG_HDMI_KPGAIN Fld(3,0,AC_MSKB0)//[2:0]
#define HDMI2_REG_HDMIRX_CFG2 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x008))
  #define HDMI2_REG_HDMIRX_CFG2_0 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x008))
  #define HDMI2_REG_HDMIRX_CFG2_1 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x009))
  #define HDMI2_REG_HDMIRX_CFG2_2 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x00A))
  #define HDMI2_REG_HDMIRX_CFG2_3 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x00B))
    #define HDMI2_CFG2_RG_HDMI_PLLCK_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define HDMI2_CFG2_RG_HDMI_RXAFE_EN_SW Fld(1,30,AC_MSKB3)//[30:30]
    #define HDMI2_CFG2_RG_HDMI_RXAFECH0_EN Fld(1,29,AC_MSKB3)//[29:29]
    #define HDMI2_CFG2_RG_HDMI_RXAFECH1_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define HDMI2_CFG2_RG_HDMI_RXAFECH2_EN Fld(1,27,AC_MSKB3)//[27:27]
    #define HDMI2_CFG2_RG_HDMI_TERM_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define HDMI2_CFG2_RG_HDMI_ZSEN_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define HDMI2_CFG2_RG_HDMI_TERM_MODE Fld(1,24,AC_MSKB3)//[24:24]
    #define HDMI2_CFG2_RG_HDMI_RXMODE Fld(2,22,AC_MSKB2)//[23:22]
    #define HDMI2_CFG2_RG_HDMI_EQBAND Fld(3,19,AC_MSKB2)//[21:19]
    #define HDMI2_CFG2_RG_HDMI_TEST_SEL Fld(1,18,AC_MSKB2)//[18:18]
    #define HDMI2_CFG2_RG_HDMI_EQ_GAIN Fld(2,16,AC_MSKB2)//[17:16]
    #define HDMI2_CFG2_RG_HDMI_TERM_MHL_SEL Fld(1,15,AC_MSKB1)//[15:15]
    #define HDMI2_CFG2_RG_HDMI_TERM_CM_ENB Fld(1,14,AC_MSKB1)//[14:14]
    #define HDMI2_CFG2_RG_HDMI_DEEPCLRCLK_PDB Fld(1,13,AC_MSKB1)//[13:13]
    #define HDMI2_CFG2_RG_HDMI_DEEPCLRCLK_RSTN Fld(1,12,AC_MSKB1)//[12:12]
    #define HDMI2_CFG2_RG_HDMI_FM_SEL Fld(12,0,AC_MSKW10)//[11:0]
#define HDMI2_REG_HDMIRX_CFG3 (IO_VIRT + (IO_HDMI2_RX_BASE  + 0x00C))
  #define HDMI2_REG_HDMIRX_CFG3_0 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x00C))
  #define HDMI2_REG_HDMIRX_CFG3_1 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x00D))
  #define HDMI2_REG_HDMIRX_CFG3_2 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x00E))
  #define HDMI2_REG_HDMIRX_CFG3_3 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x00F))
    #define HDMI2_CFG3_RG_HDMI_FM_SEL2 Fld(12,20,AC_MSKW32)//[31:20]
    #define HDMI2_CFG3_RG_HDMI_TESTE Fld(1,19,AC_MSKB2)//[19:19]
    #define HDMI2_CFG3_RG_HDMI_CKMON_JITSEL Fld(1,18,AC_MSKB2)//[18:18]
    #define HDMI2_CFG3_RG_HDMI_CKMON_DIV4EN Fld(1,17,AC_MSKB2)//[17:17]
    #define HDMI2_CFG3_RG_HDMI_CKMON_JITEN Fld(1,16,AC_MSKB2)//[16:16]
    #define HDMI2_CFG3_RG_HDMI_FLAG_SEL Fld(1,14,AC_MSKB1)//[14:14]
    #define HDMI2_CFG3_RG_HDMI_ABIST_EN Fld(1,13,AC_MSKB1)//[13:13]
    #define HDMI2_CFG3_RG_HDMI_ABISTCK_SEL Fld(1,12,AC_MSKB1)//[12:12]
    #define HDMI2_CFG3_RG_HDMI_CKMON_CKSET Fld(2,10,AC_MSKB1)//[11:10]
    #define HDMI2_CFG3_RG_HDMI_CH0_MONEN Fld(1,9,AC_MSKB1)//[9:9]
    #define HDMI2_CFG3_RG_HDMI_CH1_MONEN Fld(1,8,AC_MSKB1)//[8:8]
    #define HDMI2_CFG3_RG_HDMI_CH2_MONEN Fld(1,7,AC_MSKB0)//[7:7]
    #define HDMI2_CFG3_RG_HDMI_CKMON_PLLSEL Fld(1,6,AC_MSKB0)//[6:6]
    #define HDMI2_CFG3_RG_HDMI_EQ_CALEN Fld(1,5,AC_MSKB0)//[5:5]
#define HDMI2_REG_HDMIRX_CFG4 (IO_VIRT + (IO_HDMI2_RX_BASE  + 0x010))
    #define HDMI2_CFG4_RG_HDMI_REV Fld(32,0,AC_FULLDW)//[31:0]
    #define HDMI2_CFG4_RG_HDMI_GAIN_BUF Fld(1,31,AC_MSKB3) //[31:31]
    #define HDMI2_CFG4_RG_HDMI_DEBUG1_CTRL_EN Fld(1,30,AC_MSKB3) //[30:30]
    #define HDMI2_CFG4_RG_HDMI_DEEPPLL_POSTDIV2 Fld(1,29,AC_MSKB3) //[29:29]
#define HDMI2_REG_HDMIRX_CFG5 (IO_VIRT + (IO_HDMI2_RX_BASE  + 0x014))
    #define HDMI2_CFG5_RG_HDMI_PLL_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define HDMI2_CFG5_RG_HDMI_PLL_FBKDIV Fld(7,24,AC_MSKB3)//[30:24]
    #define HDMI2_CFG5_RG_HDMI_PLL_DDSFBK_EN Fld(1,23,AC_MSKB2)//[23:23]
    #define HDMI2_CFG5_RG_HDMI_PLL_RLH_EN Fld(1,22,AC_MSKB2)//[22:22]
    #define HDMI2_CFG5_RG_HDMI_PLL_FBKSEL Fld(2,20,AC_MSKB2)//[21:20]
    #define HDMI2_CFG5_RG_HDMI_PLL_PREDIV Fld(2,18,AC_MSKB2)//[19:18]
    #define HDMI2_CFG5_RG_HDMI_PLL_POSDIV Fld(2,16,AC_MSKB2)//[17:16]
    #define HDMI2_CFG5_RG_HDMI_PLL_IC Fld(4,12,AC_MSKB1)//[15:12]
    #define HDMI2_CFG5_RG_HDMI_PLL_IR Fld(4,8,AC_MSKB1)//[11:8]
    #define HDMI2_CFG5_RG_HDMI_PLL_BP Fld(4,4,AC_MSKB0)//[7:4]
    #define HDMI2_CFG5_RG_HDMI_PLL_BC Fld(2,2,AC_MSKB0)//[3:2]
    #define HDMI2_CFG5_RG_HDMI_PLL_BR Fld(2,0,AC_MSKB0)//[1:0]
#define HDMI2_REG_HDMIRX_CFG6 (IO_VIRT + (IO_HDMI2_RX_BASE  + 0x018))
    #define HDMI2_CFG6_RG_HDMI_PLL_AUTOK_LOAD Fld(1,31,AC_MSKB3)//[31:31]
    #define HDMI2_CFG6_RG_HDMI_PLL_DIVEN Fld(3,28,AC_MSKB3)//[30:28]
    #define HDMI2_CFG6_RG_HDMI_PLL_AUTOK_KS Fld(2,26,AC_MSKB3)//[27:26]
    #define HDMI2_CFG6_RG_HDMI_PLL_AUTOK_KF Fld(2,24,AC_MSKB3)//[25:24]
    #define HDMI2_CFG6_RG_HDMI_PLL_AUTOK_EN Fld(1,23,AC_MSKB2)//[23:23]
    #define HDMI2_CFG6_RG_HDMI_PLL_BAND Fld(7,16,AC_MSKB2)//[22:16]
    #define HDMI2_CFG6_RG_HDMI_PLL_MONCK_EN Fld(1,15,AC_MSKB1)//[15:15]
    #define HDMI2_CFG6_RG_HDMI_PLL_MONVC_EN Fld(1,14,AC_MSKB1)//[14:14]
    #define HDMI2_CFG6_RG_HDMI_PLL_MONREF_EN Fld(1,13,AC_MSKB1)//[13:13]
    #define HDMI2_CFG6_RG_HDMI_PLL_LVROD_EN Fld(1,12,AC_MSKB1)//[12:12]
    #define HDMI2_CFG6_RG_HDMI_PLL_BIAS_LPF_EN Fld(1,11,AC_MSKB1)//[11:11]
    #define HDMI2_CFG6_RG_HDMI_PLL_BIAS_EN Fld(1,10,AC_MSKB1)//[10:10]
    #define HDMI2_CFG6_RG_HDMI_PLL_RST_DLY Fld(2,8,AC_MSKB1)//[9:8]
    #define HDMI2_CFG6_RG_HDMI_PLL_RESERVE Fld(8,0,AC_FULLB0)//[7:0]
#define HDMI2_REG_HDMIRX_CFG7 (IO_VIRT + (IO_HDMI2_RX_BASE  + 0x01C))
    #define HDMI2_CFG7_RG_HDMI_PLL_BRA Fld(1,28,AC_MSKB3)//[28:28]
    #define HDMI2_CFG7_RG_HDMI_PLL_BCA Fld(1,24,AC_MSKB3)//[24:24]
    #define HDMI2_CFG7_RG_HDMI_PLL_DIVENA Fld(3,20,AC_MSKB2)//[22:20]
    #define HDMI2_CFG7_RG_HDMI_PLL_ICADJ Fld(2,18,AC_MSKB2)//[19:18]
    #define HDMI2_CFG7_RG_HDMI_PLL_KVSEL Fld(2,16,AC_MSKB2)//[17:16]
    #define HDMI2_CFG7_RG_HDMI_PLL_VCO_BAND Fld(5,8,AC_MSKB1)//[12:8]
    #define HDMI2_CFG7_RG_HDMI_PLL_HDMI20_MODE Fld(1,7,AC_MSKB0)//[7:7]
    #define HDMI2_CFG7_RG_HDMI_PLL_HF_MODE Fld(1,6,AC_MSKB0)//[6:6]
    #define HDMI2_CFG7_RG_HDMI_PLL_DIVCTRL Fld(2,4,AC_MSKB0)//[5:4]
    #define HDMI2_CFG7_RG_HDMI_PLL_DETEN Fld(1,2,AC_MSKB0)//[2:2]
    #define HDMI2_CFG7_RG_HDMI_PLL_DIV2_SEL Fld(1,0,AC_MSKB0)//[0:0]
#define HDMI2_REG_HDMIRX_CFG8 (IO_VIRT + (IO_HDMI2_RX_BASE  + 0x020))
    #define HDMI2_CFG8_RG_HDMI_DATA_RSTB Fld(1,29,AC_MSKB3)//[29:29]
    #define HDMI2_CFG8_RG_HDMI_EQALG_RSTB Fld(1,28,AC_MSKB3)//[28:28]
    #define HDMI2_CFG8_RG_HDMI_EQALG_LEQ_RSTB Fld(1,26,AC_MSKB3)//[26:26]
    #define HDMI2_CFG8_RG_HDMI_EQALG_LEQ_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define HDMI2_CFG8_RG_HDMI_EQALG_LEQ_INIT_SET Fld(1,24,AC_MSKB3)//[24:24]
    #define HDMI2_CFG8_RG_HDMI_EQALG_LEQ_INIT_VAL Fld(4,20,AC_MSKB2)//[23:20]
    #define HDMI2_CFG8_RG_HDMI_EQALG_LEQ_GAIN Fld(4,16,AC_MSKB2)//[19:16]
    #define HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_EN Fld(1,15,AC_MSKB1)//[15:15]
    #define HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_INCREASE_CTRL Fld(3,12,AC_MSKB1)//[14:12]
    #define HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_DECREASE_CTRL Fld(4,8,AC_MSKB1)//[11:8]
    #define HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_GAIN Fld(4,4,AC_MSKB0)//[7:4]
    #define HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_TARGET Fld(4,0,AC_MSKB0)//[3:0]
#define HDMI2_REG_HDMIRX_CFG9 (IO_VIRT + (IO_HDMI2_RX_BASE  + 0x024))
    #define HDMI2_CFG9_RG_HDMI_EQALG_LEQ_REDUCE_GAIN Fld(4,28,AC_MSKB3)//[31:28]
    #define HDMI2_CFG9_RG_HDMI_EQALG_LEQ_REDUCE_EN Fld(1,27,AC_MSKB3)//[27:27]
    #define HDMI2_CFG9_RG_HDMI_EQALG_LEQ_AVG_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define HDMI2_CFG9_RG_HDMI_EQALG_LEQ_REDUCE_THRESHOLD (fgIs5861?Fld(7,19,AC_MSKW32):Fld(7,16,AC_MSKB2))//[25:19] 5861, [22:16]5890 E3
    #define HDMI2_CFG9_RG_HDMI_EQALG_LEQ_REDUCE_TARGET (fgIs5861?Fld(7,11,AC_MSKW21):Fld(7,8,AC_MSKB1))//[17:11] 5861, [14:8]5890 E3
    #define HDMI2_CFG9_RG_HDMI_EQALG_LEQ_REDUCE_ADJUST_TOP_LIM (fgIs5861?Fld(7,3,AC_MSKW10):Fld(7,0,AC_MSKB0))//[9:3]5861,[6:0]5890 E3
#define HDMI2_REG_HDMIRX_CFG10 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x028))
    #define HDMI2_CFG10_RG_HDMI_EQALG_LEQ_REDUCE_ADJUST_STEP (fgIs5861?Fld(7,25,AC_MSKB3):Fld(7,24,AC_MSKB3))//[31:25]5861,[30:24]5890 E3
    #define HDMI2_CFG10_RG_HDMI_EQALG_LEQ_REDUCE_ADJUST_INIT_VAL (fgIs5861?Fld(7,17,AC_MSKB2):Fld(7,16,AC_MSKB2))//[23:17]5861,[22:16]5890 E3
    #define HDMI2_CFG10_RG_HDMI_EQALG_LEQ_REDUCE_ADJUST_BOT_LIM (fgIs5861?Fld(7,9,AC_MSKB1):Fld(7,8,AC_MSKB1))//[15:9]5861,[14:8]5890 E3
    #define HDMI2_CFG10_RG_HDMI_EQALG_LEQ_REDUCE_ADJUST_THRESHOLD (fgIs5861?Fld(7,1,AC_MSKB0):Fld(7,0,AC_MSKB0))//[7:1]5861,[6:0]5890 E3
#define HDMI2_REG_HDMIRX_CFG11 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x02C))
    #define HDMI2_CFG11_RG_HDMI_EQALG_LEQ_AVG_GAIN Fld(4,28,AC_MSKB3)//[31:28]
    #define HDMI2_CFG11_RG_HDMI_EQALG_LEQ_INCREASE_RSTB Fld(1,15,AC_MSKB1)//[15:15]
    #define HDMI2_CFG11_RG_HDMI_EQALG_LEQ_INCREASE_P_EN Fld(2,13,AC_MSKB1)//[14:13]
    #define HDMI2_CFG11_RG_HDMI_EQALG_LEQ_INCREASE_GAIN_P1 (fgIs5861?Fld(4,9,AC_MSKB1):Fld(4,4,AC_MSKB0))//[12:9]5861,[7:4]5890 E3
    #define HDMI2_CFG11_RG_HDMI_EQALG_LEQ_INCREASE_GAIN_P2 (fgIs5861?Fld(4,5,AC_MSKW10):Fld(4,0,AC_MSKB0))//[8:5]5861,[3:0]5890 E3
#define HDMI2_REG_HDMIRX_CFG12 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x030))
    #define HDMI2_CFG12_RG_HDMI_EQALG_LEQ_INCREASE_INIT_VAL_P1 (fgIs5861?Fld(7,25,AC_MSKB3):Fld(7,24,AC_MSKB3))//[31:25]5861,[30:24]5890 E3
    #define HDMI2_CFG12_RG_HDMI_EQALG_LEQ_INCREASE_INIT_VAL_P2 (fgIs5861?Fld(7,17,AC_MSKB2):Fld(7,16,AC_MSKB2))//[23:17]5861,[22:16]5890 E3
    #define HDMI2_CFG12_RG_HDMI_EQALG_LEQ_INCREASE_TOP_LIM_P2 (fgIs5861?Fld(7,9,AC_MSKB1):Fld(7,8,AC_MSKB1))//[15:9]5861,[14:8]5890 E3
    #define HDMI2_CFG12_RG_HDMI_EQALG_LEQ_INCREASE_BOT_LIM_P2 (fgIs5861?Fld(7,1,AC_MSKB0):Fld(6,0,AC_MSKB0))//[7:1]5861,[6:0]5890 E3
#define HDMI2_REG_HDMIRX_CFG13 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x034))
    #define HDMI2_CFG13_RG_HDMI_EQALG_LEQ_INCREASE_LOGIC1_LOGIC Fld(3,29,AC_MSKB3)//[31:29]
    #define HDMI2_CFG13_RG_HDMI_EQALG_LEQ_INCREASE_N1_LOGIC_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define HDMI2_CFG13_RG_HDMI_EQALG_LEQ_INCREASE_LOGIC2_LOGIC Fld(3,25,AC_MSKB3)//[27:25]
    #define HDMI2_CFG13_RG_HDMI_EQALG_LEQ_INCREASE_SATURATE_PROT_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define HDMI2_CFG13_RG_HDMI_EQALG_LEQ_INCREASE_ALG_SEL Fld(1,23,AC_MSKB2)//[23:23]
    #define HDMI2_CFG13_RG_HDMI_EQALG_ADJUST_EN (fgIs5861?Fld(1,22,AC_MSKB2):Fld(1,15,AC_MSKB1))//[22:22]5861,[15:15]5890 E3
    #define HDMI2_CFG13_RG_HDMI_EQALG_ADJUST_RATIO (fgIs5861?Fld(3,19,AC_MSKB2):Fld(3,12,AC_MSKB1))//[21:19]5861,[14:12]5890 E3
    #define HDMI2_CFG13_RG_HDMI_EQALG_ADJUST_TOP_LIM (fgIs5861?Fld(7,12,AC_MSKW21):Fld(7,0,AC_MSKB0))//[18:12]5861,[6:0]5890 E3
    #define HDMI2_CFG13_RG_HDMI_EQALG_ADJUST_RSTB Fld(1,11,AC_MSKB1)//[11:11]only 5861
    #define HDMI2_CFG13_RG_HDMI_EQALG_ADJUST_BOT_LIM Fld(7,4,AC_MSKW10)//[10:4]only 5861
    #define HDMI2_CFG13_RG_HDMI_REF_RSTB Fld(1,3,AC_MSKB0)//[3:3]only 5861
#define HDMI2_REG_HDMIRX_CFG14 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x038))
    #define HDMI2_CFG14_RG_HDMI_EQALG_ADJUST_RSTB Fld(1,31,AC_MSKB3)//[31:31]only 5890 E3
    #define HDMI2_CFG14_RG_HDMI_EQALG_ADJUST_BOT_LIM Fld(7,24,AC_MSKB3)//[30:24]only 5890 E3
    #define HDMI2_CFG14_RG_HDMI_REF_RSTB Fld(1,23,AC_MSKB2)//[23:23]only 5890 E3
    #define HDMI2_CFG14_RG_HDMI_DEBUG1_CNT_TARGET (fgIs5861?Fld(20,12,AC_MSKDW):Fld(20,0,AC_MSKDW))//[31:12]5861,[19:0]5890 E3
    #define HDMI2_CFG14_RG_HDMI_DEBUG1_RSTB Fld(1,11,AC_MSKB1)//[11:11]only 5861
    #define HDMI2_CFG14_RG_HDMI_DEBUG1_CNT_EN Fld(1,10,AC_MSKB1)//[10:10]only 5861
    #define HDMI2_CFG14_RG_HDMI_DEBUG1_CNT_GATING Fld(1,9,AC_MSKB1)//[9:9]only 5861
    #define HDMI2_CFG14_RG_HDMI_DEBUG1_MON_SEL Fld(1,8,AC_MSKB1)//[8:8]only 5861
#define HDMI2_REG_HDMIRX_CFG15 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x03C))
    #define HDMI2_CFG15_RG_HDMI_DEBUG1_RSTB Fld(1,31,AC_MSKB3)//[31:31]only 5890 E3
    #define HDMI2_CFG15_RG_HDMI_DEBUG1_CNT_EN Fld(1,30,AC_MSKB3)//[30:30]only 5890 E3
    #define HDMI2_CFG15_RG_HDMI_DEBUG1_CNT_GATING Fld(1,29,AC_MSKB3)//[29:29]only 5890 E3
    #define HDMI2_CFG15_RG_HDMI_DEBUG1_MON_SEL Fld(1,28,AC_MSKB3)//[28:28]only 5890 E3
    #define HDMI2_CFG15_RG_HDMI_DEBUG1_PAT_MASK (fgIs5861?Fld(10,22,AC_MSKW32):Fld(10,18,AC_MSKW32))//[31:22]5861,[27:18]5890 E3
    #define HDMI2_CFG15_RG_HDMI_DEBUG1_TARGET_PAT (fgIs5861?Fld(10,9,AC_MSKW21):Fld(10,5,AC_MSKW10))//[18:9]5861,[14:5]5890 E3
#define HDMI2_REG_HDMIRX_CFG16 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x040))
    #define HDMI2_CFG16_RG_HDMI_DEBUG1_PAR1 (fgIs5861?Fld(7,25,AC_MSKB3):Fld(7,19,AC_MSKW32))//[31:25]5861,[25:19]5890 E3
    #define HDMI2_CFG16_RG_HDMI_DEBUG1_PAR2 (fgIs5861?Fld(7,18,AC_MSKW32):Fld(7,12,AC_MSKW21))//[24:18]5861,[18:12]5890 E3
    #define HDMI2_CFG16_RG_HDMI_DEBUG2_RSTB (fgIs5861?Fld(1,17,AC_MSKB2):Fld(1,11,AC_MSKB1))//[17:17]5861,[11:11]5890 E3
    #define HDMI2_CFG16_RG_HDMI_DEBUG2_CDRLPI (fgIs5861?Fld(7,10,AC_MSKW21):Fld(7,4,AC_MSKW10))//[16:10]5861,[10:4]5890 E3
    #define HDMI2_CFG16_RG_HDMI_DEBUG2_CAL_EN (fgIs5861?Fld(1,9,AC_MSKB1):Fld(1,3,AC_MSKB0))//[9:9]5861,[3:3]5890 E3
    #define HDMI2_CFG16_RG_HDMI_DEBUG2_KPGAIN (fgIs5861?Fld(3,6,AC_MSKW10):Fld(3,0,AC_MSKB0))//[8:6]5861,[2:0]5890 E3
    #define HDMI2_CFG16_RG_HDMI_SAOSC_RSTB Fld(1,5,AC_MSKB0)//[5:5]only 5861
    #define HDMI2_CFG16_RG_HDMI_SAOSC_EN Fld(1,4,AC_MSKB0)//[4:4]only 5861
    #define HDMI2_CFG16_RG_HDMI_SAOSC_EQ_OS_LF_SEL Fld(3,1,AC_MSKB0)//[3:1]only 5861
    #define HDMI2_CFG16_RG_HDMI_EQOSC_RSTB Fld(1,0,AC_MSKB0)//[0:0]only 5861
#define HDMI2_REG_HDMIRX_CFG17 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x44))
    #define HDMI2_CFG17_RG_HDMI_SAOSC_RSTB Fld(1,31,AC_MSKB3)//[31:31]only 5890 E3
    #define HDMI2_CFG17_RG_HDMI_SAOSC_EN Fld(1,30,AC_MSKB3)//[30:30]only 5890 E3
    #define HDMI2_CFG17_RG_HDMI_SAOSC_EQ_OS_LF_SEL Fld(3,27,AC_MSKB3)//[29:27]only 5890 E3
    #define HDMI2_CFG17_RG_HDMI_EQOSC_RSTB Fld(1,26,AC_MSKB3)//[26:26]only 5890 E3
    #define HDMI2_CFG17_RG_HDMI_EQOSC_EN (fgIs5861?Fld(1,31,AC_MSKB3):Fld(1,25,AC_MSKB3))//[31:31]5861,[25:25]5890 E3
    #define HDMI2_CFG17_RG_HDMI_EQALG_DEBUG1_EVENSEL (fgIs5861?Fld(1,30,AC_MSKB3):Fld(1,24,AC_MSKB3))//[30:30]5861,[24:25]5890 E3
    #define HDMI2_CFG17_RG_HDMI_EQALG_LEQ_INCREASE_MONSEL (fgIs5861?Fld(4,25,AC_MSKB3):Fld(4,19,AC_MSKB2))//[28:25]5861,[22:19]5890 E3
    #define HDMI2_CFG17_RG_HDMI_EQALG_DEBUG1_MONSEL (fgIs5861?Fld(3,21,AC_MSKB2):Fld(3,15,AC_MSKW21))//[23:21]5861,[17:15]5890 E3
    #define HDMI2_CFG17_RG_HDMI_EQALG_DEBUG_SEL (fgIs5861?Fld(8,9,AC_MSKW21):Fld(8,3,AC_MSKW10))//[16:9]5861,[10:3]5890 E3
    #define HDMI2_CFG17_RG_HDMI_EQALG_RESERVE Fld(8,1,AC_MSKW10)//[8:1]only 5861
    #define HDMI2_CFG17_RG_HDMI_EQALG_ADJUST_RATIO_MSB Fld(1,1,AC_MSKB0)//[1:1]only 5861
    #define HDMI2_CFG17_RG_HDMI_LOGIC1_P2_CTRL Fld(1,0,AC_MSKB0)//[0:0]only 5861
#define HDMI2_REG_HDMIRX_CFG18 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x048))
    #define HDMI2_CFG18_RG_HDMI_EQALG_RESERVE Fld(8,24,AC_MSKB3)//[31:24]only 5890 E3
    #define HDMI2_CFG18_RG_HDMI_EQALG_ADJUST_RATIO_MSB Fld(1,24,AC_MSKB3)//[24:24]only 5890 E3
    #define HDMI2_CFG18_RG_HDMI_LOGIC1_P2_CTRL Fld(1,23,AC_MSKB2)//[23:23]only 5890 E3
    #define HDMI2_CFG18_RG_HDMI_LOGIC2_P2_CTRL (fgIs5861?Fld(1,31,AC_MSKB3):Fld(1,22,AC_MSKB2))//[31:31]5861,[22:22]5890 E3
    #define HDMI2_CFG18_RG_HDMI_CON_P2_HEAD (fgIs5861?Fld(2,29,AC_MSKB3):Fld(2,20,AC_MSKB2))//[30:29]5861,[21:20]5890 E3
    #define HDMI2_CFG18_RG_HDMI_CON_PWD_CTRL (fgIs5861?Fld(2,27,AC_MSKB3):Fld(2,18,AC_MSKB2))//[28:27]5861,[19:18]5890 E3
    #define HDMI2_CFG18_RG_HDMI_DEBUG1_CTRL (fgIs5861?Fld(2,25,AC_MSKB3):Fld(2,16,AC_MSKB2))//[26:25]5861,[17:16]5890 E3
    #define HDMI2_CFG18_RG_HDMI_CON_CTRL (fgIs5861?Fld(3,22,AC_MSKW32):Fld(3,13,AC_MSKB1))//[24:22]5861,[15:13]5890 E3
    #define HDMI2_CFG18_RG_HDMI_CON_PDB (fgIs5861?Fld(1,21,AC_MSKB2):Fld(1,12,AC_MSKB1))//[21:21]5861,[12:12]5890 E3
    #define HDMI2_CFG18_RG_HDMI_CON_MON (fgIs5861?Fld(6,11,AC_MSKW21):Fld(6,2,AC_MSKB0))//[16:11]5861,[7:2]5890 E3
    #define HDMI2_CFG18_RG_HDMI_DEEPPLL_EN Fld(1,0,AC_MSKB0)//[0:0]only 5861
#define HDMI2_REG_HDMIRX_CFG19 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x04C))
    #define HDMI2_CFG19_RG_HDMI_DEEPPLL_EN Fld(1,21,AC_MSKB2)//[21:21]only 5890 E3
    #define HDMI2_CFG19_RG_HDMI_DEEPPLL_POSDIV (fgIs5861?Fld(3,29,AC_MSKB3):Fld(3,18,AC_MSKB2))//[31:29]5861,[20:18]5890 E3
    #define HDMI2_CFG19_RG_HDMI_DEEPPLL_PREDIV (fgIs5861?Fld(2,27,AC_MSKB3):Fld(2,16,AC_MSKB2))//[28:27]5861,[17:16]5890 E3
    #define HDMI2_CFG19_RG_HDMI_DEEPPLL_LVROD_EN (fgIs5861?Fld(1,26,AC_MSKB3):Fld(1,15,AC_MSKB1))//[26:26]5861,[15:15]5890 E3
    #define HDMI2_CFG19_RG_HDMI_DEEPPLL_MONCK_EN (fgIs5861?Fld(1,25,AC_MSKB3):Fld(1,14,AC_MSKB1))//[25:25]5861,[14:14]5890 E3
    #define HDMI2_CFG19_RG_HDMI_DEEPPLL_MONVC_EN (fgIs5861?Fld(1,24,AC_MSKB3):Fld(1,13,AC_MSKB1))//[24:24]5861,[13:13]5890 E3
    #define HDMI2_CFG19_RG_HDMI_DEEPPLL_MONREF_EN (fgIs5861?Fld(1,23,AC_MSKB2):Fld(1,12,AC_MSKB1))//[23:23]5861,[12:12]5890 E3
    #define HDMI2_CFG19_RG_HDMI_DEEPPLL_SDM_FRA_EN (fgIs5861?Fld(1,22,AC_MSKB2):Fld(1,11,AC_MSKB1))//[22:22]5861,[11:11]5890 E3
    #define HDMI2_CFG19_RG_HDMI_DEEPPLL_SDM_PCW_CHG (fgIs5861?Fld(1,21,AC_MSKB2):Fld(1,10,AC_MSKB1))//[21:21]5861,[10:10]5890 E3
    #define HDMI2_CFG19_RG_HDMI_DEEPPLL_SDM_PCW (fgIs5861?Fld(7,14,AC_MSKW21):Fld(7,3,AC_MSKW10))//[20:14]5861,[9:3]5890 E3
    #define HDMI2_CFG19_RG_HDMI_DEEPPLL_DET_EN (fgIs5861?Fld(1,13,AC_MSKB1):Fld(1,2,AC_MSKB0))//[13:13]5861,[2:2]5890 E3
    #define HDMI2_CFG19_RG_HDMI_DEEPPLL_CKSEL_MON (fgIs5861?Fld(2,11,AC_MSKB1):Fld(2,0,AC_MSKB0))//[12:11]5861,[1:0]5890 E3
    #define HDMI2_CFG19_RG_HDMI_DEEPPLL_BS Fld(2,9,AC_MSKB1)//[10:9]only 5861
    #define HDMI2_CFG19_RG_HDMI_DEEPPLL_RESERVE Fld(8,1,AC_MSKW10)//[8:1]only 5861
#define HDMI2_REG_HDMIRX_CFG20 (IO_VIRT + (IO_HDMI2_RX_BASE + 0x050))//only 5890
    #define HDMI2_CFG20_RG_HDMI_DEEPPLL_BS Fld(2,30,AC_MSKB1)//[10:9]only 5890
    #define HDMI2_CFG20_RG_HDMI_DEEPPLL_RESERVE Fld(8,22,AC_MSKW10)//[8:1]only 5890
#define HDMI2_RGS_HDMIRX_CFG0 (IO_VIRT + (IO_HDMI2_RX_BASE + (fgIs5861?0x050:0x054)))
    #define HDMI2_CFG0_RGS_HDMI_CH0_STATUS Fld(8,24,AC_FULLB3)//[31:24]
    #define HDMI2_CFG0_RGS_HDMI_CH0_ABIST_FLAG1 Fld(1,23,AC_MSKB2)//[23:23]
    #define HDMI2_CFG0_RGS_HDMI_CH0_ABIST_FLAG2 Fld(1,22,AC_MSKB2)//[22:22]
    #define HDMI2_CFG0_RGS_HDMI_CH0_EQ_OS Fld(5,17,AC_MSKB2)//[21:17]
    #define HDMI2_CFG0_RGS_HDMI_CH0_EQ_DEBUG1_CNT_RDY_O Fld(1,16,AC_MSKB2)//[16:16]
    #define HDMI2_CFG0_RGS_HDMI_CH0_DEBUG1_STATUS Fld(10,6,AC_MSKW10)//[15:6]
#define HDMI2_RGS_HDMIRX_CFG1 (IO_VIRT + (IO_HDMI2_RX_BASE + (fgIs5861?0x054:0x058)))
    #define HDMI2_CFG1_RGS_HDMI_CH1_STATUS Fld(8,24,AC_FULLB3)//[31:24]
    #define HDMI2_CFG1_RGS_HDMI_CH1_ABIST_FLAG1 Fld(1,23,AC_MSKB2)//[23:23]
    #define HDMI2_CFG1_RGS_HDMI_CH1_ABIST_FLAG2 Fld(1,22,AC_MSKB2)//[22:22]
    #define HDMI2_CFG1_RGS_HDMI_CH1_EQ_OS Fld(5,17,AC_MSKB2)//[21:17]
    #define HDMI2_CFG1_RGS_HDMI_CH1_EQ_DEBUG1_CNT_RDY_O Fld(1,16,AC_MSKB2)//[16:16]
    #define HDMI2_CFG1_RGS_HDMI_CH1_DEBUG1_STATUS Fld(10,6,AC_MSKW10)//[15:6]
#define HDMI2_RGS_HDMIRX_CFG2 (IO_VIRT + (IO_HDMI2_RX_BASE + (fgIs5861?0x058:0x05c)))
    #define HDMI2_CFG2_RGS_HDMI_CH2_STATUS Fld(8,24,AC_FULLB3)//[31:24]
    #define HDMI2_CFG2_RGS_HDMI_CH2_ABIST_FLAG1 Fld(1,23,AC_MSKB2)//[23:23]
    #define HDMI2_CFG2_RGS_HDMI_CH2_ABIST_FLAG2 Fld(1,22,AC_MSKB2)//[22:22]
    #define HDMI2_CFG2_RGS_HDMI_CH2_EQ_OS Fld(5,17,AC_MSKB2)//[21:17]
    #define HDMI2_CFG2_RGS_HDMI_CH2_EQ_DEBUG1_CNT_RDY_O Fld(1,16,AC_MSKB2)//[16:16]
    #define HDMI2_CFG2_RGS_HDMI_CH2_DEBUG1_STATUS Fld(10,6,AC_MSKW10)//[15:6]
#define HDMI2_RGS_HDMIRX_CFG3 (IO_VIRT + (IO_HDMI2_RX_BASE + (fgIs5861?0x05c:0x060)))
    #define HDMI2_CFG3_RGS_HDMI_PLL_AUTOK_PASS Fld(1,25,AC_MSKB3)//[25:25]
    #define HDMI2_CFG3_RGS_HDMI_PLL_AUTOK_FAIL Fld(1,24,AC_MSKB3)//[24:24]
    #define HDMI2_CFG3_RGS_HDMI_PLL_AUTOK_BAND Fld(7,16,AC_MSKB2)//[22:16]
    #define HDMI2_CFG3_RGS_HDMI_CH0_LEQCTRL Fld(4,8,AC_MSKB1)//[11:8]
    #define HDMI2_CFG3_RGS_HDMI_CH1_LEQCTRL Fld(4,4,AC_MSKB0)//[7:4]
    #define HDMI2_CFG3_RGS_HDMI_CH2_LEQCTRL Fld(4,0,AC_MSKB0)//[3:0]
#define HDMI2_RGS_HDMIRX_CFG4 (IO_VIRT + (IO_HDMI2_RX_BASE + (fgIs5861?0x060:0x064)))
    #define HDMI2_CFG4_RGS_HDMI_CH0_CDRLPO Fld(7,24,AC_MSKB3)//[30:24]
    #define HDMI2_CFG4_RGS_HDMI_CH1_CDRLPO Fld(7,16,AC_MSKB2)//[22:16]
    #define HDMI2_CFG4_RGS_HDMI_CH2_CDRLPO Fld(7,8,AC_MSKB1)//[14:8]
#define HDMI2_RGS_HDMIRX_CFG5 (IO_VIRT + (IO_HDMI2_RX_BASE + (fgIs5861?0x064:0x068)))
    #define HDMI2_CFG5_RGS_HDMI_CH0_ALG_MON Fld(8,24,AC_FULLB3)//[31:24]
    #define HDMI2_CFG5_RGS_HDMI_CH1_ALG_MON Fld(8,16,AC_FULLB2)//[23:16]
    #define HDMI2_CFG5_RGS_HDMI_CH2_ALG_MON Fld(8,8,AC_FULLB1)//[15:8]



//-----------------------------HDMI---------------------------//
#ifndef CC_UP8032_ATV
#define IO_VIRT_HDMI2 (VDOIN_BASE)
#define IO_HDMI2_BASE (0xE000)
#else
#define IO_VIRT_HDMI2 (VDOIN_BASE)
#define IO_HDMI2_BASE (0x4000)
#endif
	
	//Page HDMI
#define HDMI2_SEL (IO_VIRT_HDMI+(0xc000+0xfc8))
	#define RG_HDMI2_REG_CCIR_CLK_SEL Fld(4,17,AC_MSKB2)//[17:20]
	#define RG_HDMI2_REG_MAIN_SUB_CLK Fld(1,15,AC_MSKB1)//[15]
	#define RG_HDMI2_REG_CCIR_CLK_SEL2 Fld(1,13,AC_MSKB1)//[13:13]
#define HDMI2_RX_PWD_CTRL (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x000)
    #define RG_HDMI2_REG_CORE_ISO_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_RI_PWD_ISOLATION_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_RSVD Fld(2,2,AC_MSKB0)//[3:2]
    #define RG_HDMI2_REG_TMDS_MODE_INV Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REG_DVI_RX_DIG_BYPASS Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_RSVD_1 Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_REG_MHL3CE_SEL_RX Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_TDM_TX_SRST Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_TDM_RX_SRST Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_COCSNK_SRST Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_DOCSNK_SRST Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_REG_TDM_SRST Fld(1,12,AC_MSKB1)//[12:12]
    #define RG_HDMI2_RSVD_2 Fld(3,13,AC_MSKB1)//[15:13]
    #define RG_HDMI2_REG_AUD_FIFO_RST_AUTO Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_REG_FIFO_RST Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_REG_ACR_RST Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_REG_HDCP_RST Fld(1,19,AC_MSKB2)//[19:19]
    #define RG_HDMI2_REG_DC_FIFO_RST Fld(1,20,AC_MSKB2)//[20:20]
    #define RG_HDMI2_REG_AAC_RST Fld(1,21,AC_MSKB2)//[21:21]
    #define RG_HDMI2_REG_ACR_RST_AUTO Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_REG_HDCP_RST_AUTO Fld(1,23,AC_MSKB2)//[23:23]
    #define RG_HDMI2_REG_PD_ALL Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_REG_SYNC_POL Fld(1,25,AC_MSKB3)//[25:25]
    #define RG_HDMI2_REG_SW_RST Fld(1,26,AC_MSKB3)//[26:26]
    #define RG_HDMI2_REG_SW_RST_AUTO Fld(1,27,AC_MSKB3)//[27:27]
    #define RG_HDMI2_REG_OSC_RST Fld(1,28,AC_MSKB3)//[28:28]
    #define RG_HDMI2_REG_HDCP2X_CRST_N Fld(1,29,AC_MSKB3)//[29:29]
    #define RG_HDMI2_REG_HDCP2X_ERST_N Fld(1,30,AC_MSKB3)//[30:30]
    #define RG_HDMI2_VSYNC_CAP Fld(1,31,AC_MSKB3)//[31:31]
#define HDMI2_SYS_TMDS_CH_MAP (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x004)
    #define RG_HDMI2_REG_DI_CH0_SEL Fld(2,0,AC_MSKB0)//[1:0]
    #define RG_HDMI2_REG_DI_CH1_SEL Fld(2,2,AC_MSKB0)//[3:2]
    #define RG_HDMI2_REG_DI_CH2_SEL Fld(2,4,AC_MSKB0)//[5:4]
    #define RG_HDMI2_RSVD_3 Fld(2,6,AC_MSKB0)//[7:6]
    #define RG_HDMI2_REG_DI_CH0_BSI Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_DI_CH1_BSI Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_DI_CH2_BSI Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_DI_CH0_INVT Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_REG_DI_CH1_INVT Fld(1,12,AC_MSKB1)//[12:12]
    #define RG_HDMI2_REG_DI_CH2_INVT Fld(1,13,AC_MSKB1)//[13:13]
    #define RG_HDMI2_REG_PHY_DI_DFF_EN Fld(1,14,AC_MSKB1)//[14:14]
    #define RG_HDMI2_RSVD_4 Fld(1,15,AC_MSKB1)//[15:15]
    #define RG_HDMI2_REG_FIFORDY_CLR_EN Fld(3,16,AC_MSKB2)//[18:16]
    #define RG_HDMI2_REG_HDMI_MODE_CLR_EN Fld(1,19,AC_MSKB2)//[19:19]
    #define RG_HDMI2_RSVD_5 Fld(4,20,AC_MSKB2)//[23:20]
    #define RG_HDMI2_REG_HDCP2_TRST Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_REG_HDCP2_TRST_AUTO Fld(1,25,AC_MSKB3)//[25:25]
    #define RG_HDMI2_REG_HDCP2X_ERST Fld(1,26,AC_MSKB3)//[26:26]
    #define RG_HDMI2_REG_HDCP2X_CRST Fld(1,27,AC_MSKB3)//[27:27]
    #define RG_HDMI2_REG_DACR_RST Fld(1,28,AC_MSKB3)//[28:28]
    #define RG_HDMI2_RSVD_6 Fld(2,29,AC_MSKB3)//[30:29]
    #define RG_HDMI2_REG_RXAV_LSRST Fld(1,31,AC_MSKB3)//[31:31]
#define HDMI2_RX_TMDS_CCTRL2 (IO_VIRT_HDMI2 + IO_HDMI2_BASE+ 0x008)
    #define RG_HDMI2_REG_DC_CTL Fld(4,0,AC_MSKB0)//[3:0]
    #define RG_HDMI2_REG_DC_CTL_OW Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REG_OFFSET_COEN Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_RSVD_7 Fld(2,6,AC_MSKB0)//[7:6]
    #define RG_HDMI2_RI_HDCP2X_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_RSVD_8 Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_RI_HDCP2X_INTR_MASK Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_RSVD_9 Fld(4,11,AC_MSKB1)//[14:11]
    #define RG_HDMI2_HDCP2X_MODE_SEL Fld(1,15,AC_MSKB1)//[15:15]
    #define RG_HDMI2_REG_VIDPATH_PAGE_CTRL Fld(2,16,AC_MSKB2)//[17:16]
    #define RG_HDMI2_REG_EXT_BIT_MODE Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_RSVD_10 Fld(5,19,AC_MSKB2)//[23:19]
    #define RG_HDMI2_REG_T4_CORR_THRESH_ Fld(7,24,AC_MSKB3)//[30:24]
    #define RG_HDMI2_RSVD_11 Fld(1,31,AC_MSKB3)//[31:31]
#define HDMI2_RX_T4_UNTHRES (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x00C)
    #define RG_HDMI2_REG_T4_UNCORR_THRESH_ Fld(7,0,AC_MSKB0)//[6:0]
    #define RG_HDMI2_RSVD_13 Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_HDMI_MODE_OVERWRITE Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_HDMI_MODE_SW_VALUE Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_FIX_DVIHDCP Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_RSVD_14 Fld(5,11,AC_MSKB1)//[15:11]
    #define RG_HDMI2_REG_PREAMBLE_CRI Fld(5,16,AC_MSKB2)//[20:16]
    #define RG_HDMI2_RSVD_15 Fld(3,21,AC_MSKB2)//[23:21]
    #define RG_HDMI2_REG_HDCP_CRI Fld(5,24,AC_MSKB3)//[28:24]
    #define RG_HDMI2_RSVD_16 Fld(3,29,AC_MSKB3)//[31:29]
#define HDMI2_RX_AUDP_FILT (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x010)
    #define RG_HDMI2_REG_BYP_DALIGN Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_BYP_SYNC Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_BYP_DVIFILT Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_RO_COLOUR_DEPTH Fld(4,4,AC_MSKB0)//[7:4]
    #define RG_HDMI2_REG_TST_MBIST_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_RBIST_CLK_ Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_TST_APLLCK Fld(1,12,AC_MSKB1)//[12:12]
    #define RG_HDMI2_REG_TST_PLLREF Fld(1,15,AC_MSKB1)//[15:15]
    #define RG_HDMI2_REG_TST_PCLK_EN Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_REG_BYP_VID_PATH Fld(1,20,AC_MSKB2)//[20:20]
    #define RG_HDMI2_REG_INVERT_TCLK Fld(1,21,AC_MSKB2)//[21:21]
    #define RG_HDMI2_REG_TST_CKDT Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_REG_TST_XCLK Fld(1,23,AC_MSKB2)//[23:23]
    #define RG_HDMI2_REG_PD_TOTAL Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_HDMI_FIFO_DIFF Fld(7,25,AC_MSKB3)//[31:25]
#define HDMI2_RX_PD_SYS2 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x014)
    #define RG_HDMI2_REG_PD_MCLK Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_REG_PD_PCLK Fld(1,7,AC_MSKB0)//[7:7]
#define HDMI2_RX_INTR1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x018)
    #define RG_HDMI2_REG_INTR1_STAT0 Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_INTR1_STAT1 Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_INTR1_STAT2 Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_INTR1_STAT3 Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_REG_INTR1_STAT4 Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REG_INTR1_STAT5 Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_INTR1_STAT6 Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_REG_INTR1_STAT7 Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_INTR2_STAT0 Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_INTR2_STAT1 Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_INTR2_STAT2 Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_INTR2_STAT6 Fld(1,14,AC_MSKB1)//[14:14]
    #define RG_HDMI2_REG_INTR2_STAT7 Fld(1,15,AC_MSKB1)//[15:15]
    #define RG_HDMI2_REG_INTR3_STAT0 Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_REG_INTR3_STAT1 Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_REG_INTR3_STAT2 Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_REG_INTR3_STAT3 Fld(1,19,AC_MSKB2)//[19:19]
    #define RG_HDMI2_REG_INTR3_STAT4 Fld(1,20,AC_MSKB2)//[20:20]
    #define RG_HDMI2_REG_INTR3_STAT5 Fld(1,21,AC_MSKB2)//[21:21]
    #define RG_HDMI2_REG_INTR3_STAT6 Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_REG_INTR3_STAT7 Fld(1,23,AC_MSKB2)//[23:23]
    #define RG_HDMI2_REG_INTR4_STAT0 Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_REG_INTR4_STAT1 Fld(1,25,AC_MSKB3)//[25:25]
    #define RG_HDMI2_REG_INTR4_STAT2 Fld(1,26,AC_MSKB3)//[26:26]
    #define RG_HDMI2_REG_INTR4_STAT3 Fld(1,27,AC_MSKB3)//[27:27]
    #define RG_HDMI2_REG_INTR4_STAT4 Fld(1,28,AC_MSKB3)//[28:28]
    #define RG_HDMI2_REG_INTR4_STAT5 Fld(1,29,AC_MSKB3)//[29:29]
    #define RG_HDMI2_REG_INTR4_STAT6 Fld(1,30,AC_MSKB3)//[30:30]
    #define RG_HDMI2_REG_INTR4_STAT7 Fld(1,31,AC_MSKB3)//[31:31]
#define HDMI2_RX_INTR5 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x01C)
    #define RG_HDMI2_REG_INTR5_STAT0 Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_INTR5_STAT5 Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_INTR5_STAT6 Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_REG_INTR5_STAT7 Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_INTR6_STAT1 Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_INTR6_STAT2 Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_INTR6_STAT3 Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_REG_INTR6_STAT4 Fld(1,12,AC_MSKB1)//[12:12]
    #define RG_HDMI2_REG_INTR6_STAT5 Fld(1,13,AC_MSKB1)//[13:13]
    #define RG_HDMI2_REG_INTR6_STAT6 Fld(1,14,AC_MSKB1)//[14:14]
    #define RG_HDMI2_REG_INTR6_STAT7 Fld(1,15,AC_MSKB1)//[15:15]
    #define RG_HDMI2_REG_INTR7_STAT0 Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_REG_INTR7_STAT1 Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_REG_INTR7_STAT2 Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_REG_INTR7_STAT5 Fld(1,21,AC_MSKB2)//[21:21]
    #define RG_HDMI2_REG_INTR7_STAT6 Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_REG_INTR7_STAT7 Fld(1,23,AC_MSKB2)//[23:23]
    #define RG_HDMI2_REG_INTR8_STAT0 Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_REG_INTR8_STAT3 Fld(1,27,AC_MSKB3)//[27:27]
    #define RG_HDMI2_REG_INTR8_STAT4 Fld(1,28,AC_MSKB3)//[28:28]
    #define RG_HDMI2_REG_INTR8_STAT5 Fld(1,29,AC_MSKB3)//[29:29]
    #define RG_HDMI2_REG_INTR8_STAT6 Fld(1,30,AC_MSKB3)//[30:30]
    #define RG_HDMI2_REG_INTR8_STAT7 Fld(1,31,AC_MSKB3)//[31:31]
#define HDMI2_RX_INTR9 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x020)
    #define RG_HDMI2_REG_INTR9_STAT0 Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_INTR9_STAT1 Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_INTR9_STAT2 Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_INTR9_STAT3 Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_REG_INTR9_STAT4 Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REG_INTR9_STAT5 Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_INTR9_STAT6 Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_REG_INTR9_STAT7 Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_INTR10_STAT0 Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_INTR10_STAT1 Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_INTR10_STAT2 Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_INTR10_STAT3 Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_REG_INTR11_STAT0 Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_REG_INTR11_STAT1 Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_REG_INTR11_STAT2 Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_REG_INTR11_STAT3 Fld(1,19,AC_MSKB2)//[19:19]
    #define RG_HDMI2_REG_INTR11_STAT4 Fld(1,20,AC_MSKB2)//[20:20]
    #define RG_HDMI2_REG_INTR11_STAT5 Fld(1,21,AC_MSKB2)//[21:21]
    #define RG_HDMI2_REG_INTR11_STAT6 Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_REG_INTR11_STAT7 Fld(1,23,AC_MSKB2)//[23:23]
    #define RG_HDMI2_REG_INTR12_STAT0 Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_REG_INTR12_STAT1 Fld(1,25,AC_MSKB3)//[25:25]
    #define RG_HDMI2_REG_INTR12_STAT2 Fld(1,26,AC_MSKB3)//[26:26]
    #define RG_HDMI2_REG_INTR12_STAT3 Fld(1,27,AC_MSKB3)//[27:27]
    #define RG_HDMI2_REG_INTR12_STAT4 Fld(1,28,AC_MSKB3)//[28:28]
    #define RG_HDMI2_REG_INTR12_STAT5 Fld(1,29,AC_MSKB3)//[29:29]
    #define RG_HDMI2_REG_INTR12_STAT6 Fld(1,30,AC_MSKB3)//[30:30]
#define HDMI2_RX_INTR13 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x024)
    #define RG_HDMI2_REG_INTR13_STAT0 Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_INTR13_STAT1 Fld(1,1,AC_MSKB0)//[1:1]
#define HDMI2_RX_INTR1_MASK1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x028)
    #define RG_HDMI2_REG_1_INTR1_MASK0 Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_1_INTR1_MASK1 Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_1_INTR1_MASK2 Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_1_INTR1_MASK3 Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_REG_1_INTR1_MASK4 Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REG_1_INTR1_MASK5 Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_1_INTR1_MASK6 Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_REG_1_INTR1_MASK7 Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_INTR2_MASK0 Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_INTR2_MASK1 Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_INTR2_MASK2 Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_INTR2_MASK6 Fld(1,14,AC_MSKB1)//[14:14]
    #define RG_HDMI2_REG_INTR2_MASK7 Fld(1,15,AC_MSKB1)//[15:15]
    #define RG_HDMI2_REG_INTR3_MASK0 Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_REG_INTR3_MASK1 Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_REG_INTR3_MASK2 Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_REG_INTR3_MASK3 Fld(1,19,AC_MSKB2)//[19:19]
    #define RG_HDMI2_REG_INTR3_MASK4 Fld(1,20,AC_MSKB2)//[20:20]
    #define RG_HDMI2_REG_INTR3_MASK5 Fld(1,21,AC_MSKB2)//[21:21]
    #define RG_HDMI2_REG_INTR3_MASK6 Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_REG_INTR3_MASK7 Fld(1,23,AC_MSKB2)//[23:23]
    #define RG_HDMI2_REG_INTR4_MASK0 Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_REG_INTR4_MASK1 Fld(1,25,AC_MSKB3)//[25:25]
    #define RG_HDMI2_REG_INTR4_MASK2 Fld(1,26,AC_MSKB3)//[26:26]
    #define RG_HDMI2_REG_INTR4_MASK3 Fld(1,27,AC_MSKB3)//[27:27]
    #define RG_HDMI2_REG_INTR4_MASK4 Fld(1,28,AC_MSKB3)//[28:28]
    #define RG_HDMI2_REG_INTR4_MASK5 Fld(1,29,AC_MSKB3)//[29:29]
    #define RG_HDMI2_REG_INTR4_MASK6 Fld(1,30,AC_MSKB3)//[30:30]
    #define RG_HDMI2_REG_INTR4_MASK7 Fld(1,31,AC_MSKB3)//[31:31]
#define HDMI2_RX_INTR5_MASK (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x02C)
    #define RG_HDMI2_REG_INTR5_MASK0 Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_INTR5_MASK5 Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_INTR5_MASK6 Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_REG_INTR5_MASK7 Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_INTR6_MASK1 Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_INTR6_MASK2 Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_INTR6_MASK3 Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_REG_INTR6_MASK4 Fld(1,12,AC_MSKB1)//[12:12]
    #define RG_HDMI2_REG_INTR6_MASK5 Fld(1,13,AC_MSKB1)//[13:13]
    #define RG_HDMI2_REG_INTR6_MASK6 Fld(1,14,AC_MSKB1)//[14:14]
    #define RG_HDMI2_REG_INTR6_MASK7 Fld(1,15,AC_MSKB1)//[15:15]
    #define RG_HDMI2_REG_INTR7_MASK0 Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_REG_INTR7_MASK1 Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_REG_INTR7_MASK2 Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_REG_INTR7_MASK5 Fld(1,21,AC_MSKB2)//[21:21]
    #define RG_HDMI2_REG_INTR7_MASK6 Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_REG_INTR7_MASK7 Fld(1,23,AC_MSKB2)//[23:23]
    #define RG_HDMI2_REG_INTR8_MASK0 Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_REG_INTR8_MASK3 Fld(1,27,AC_MSKB3)//[27:27]
    #define RG_HDMI2_REG_INTR8_MASK4 Fld(1,28,AC_MSKB3)//[28:28]
    #define RG_HDMI2_REG_INTR8_MASK5 Fld(1,29,AC_MSKB3)//[29:29]
    #define RG_HDMI2_REG_INTR8_MASK6 Fld(1,30,AC_MSKB3)//[30:30]
    #define RG_HDMI2_REG_INTR8_MASK7 Fld(1,31,AC_MSKB3)//[31:31]
#define HDMI2_RX_INTR9_MASK (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x030)
    #define RG_HDMI2_REG_INTR9_MASK0 Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_INTR9_MASK1 Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_INTR9_MASK2 Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_INTR9_MASK3 Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_REG_INTR9_MASK4 Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REG_INTR9_MASK5 Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_INTR9_MASK6 Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_REG_INTR9_MASK7 Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_INTR10_MASK0 Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_INTR10_MASK1 Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_INTR10_MASK2 Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_INTR10_MASK3 Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_REG_INTR11_MASK0 Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_REG_INTR11_MASK1 Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_REG_INTR11_MASK2 Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_REG_INTR11_MASK3 Fld(1,19,AC_MSKB2)//[19:19]
    #define RG_HDMI2_REG_INTR11_MASK4 Fld(1,20,AC_MSKB2)//[20:20]
    #define RG_HDMI2_REG_INTR11_MASK5 Fld(1,21,AC_MSKB2)//[21:21]
    #define RG_HDMI2_REG_INTR11_MASK6 Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_REG_INTR11_MASK7 Fld(1,23,AC_MSKB2)//[23:23]
    #define RG_HDMI2_REG_INTR12_MASK0 Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_REG_INTR12_MASK1 Fld(1,25,AC_MSKB3)//[25:25]
    #define RG_HDMI2_REG_INTR12_MASK2 Fld(1,26,AC_MSKB3)//[26:26]
    #define RG_HDMI2_REG_INTR12_MASK3 Fld(1,27,AC_MSKB3)//[27:27]
    #define RG_HDMI2_REG_INTR12_MASK4 Fld(1,28,AC_MSKB3)//[28:28]
    #define RG_HDMI2_REG_INTR12_MASK5 Fld(1,29,AC_MSKB3)//[29:29]
    #define RG_HDMI2_REG_INTR12_MASK6 Fld(1,30,AC_MSKB3)//[30:30]
#define HDMI2_RX_INTR13_MASK (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x034)
    #define RG_HDMI2_REG_INTR13_MASK0 Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_INTR13_MASK1 Fld(1,1,AC_MSKB0)//[1:1]
#define HDMI2_DPLL_CH0_ERR_CNT1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x038)
    #define RG_HDMI2_REG_DPLL_CH0_ERR_CNT7_0 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_DPLL_CH0_ERR_CNT14_8 Fld(7,24,AC_MSKB3)//[30:24]
    #define RG_HDMI2_REG_DPLL_CH0_VALID Fld(1,31,AC_MSKB3)//[31:31]
#define HDMI2_DPLL_CH1_ERR_CNT1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x03C)
    #define RG_HDMI2_REG_DPLL_CH1_ERR_CNT7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_DPLL_CH1_ERR_CNT14_8 Fld(7,8,AC_MSKB1)//[14:8]
    #define RG_HDMI2_REG_DPLL_CH1_VALID Fld(1,15,AC_MSKB1)//[15:15]
    #define RG_HDMI2_REG_DPLL_CH2_ERR_CNT7_0 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_DPLL_CH2_ERR_CNT14_8 Fld(7,24,AC_MSKB3)//[30:24]
    #define RG_HDMI2_REG_DPLL_CH2_VALID Fld(1,31,AC_MSKB3)//[31:31]
#define HDMI2_DPLL_SCDC_CTRL (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x040)
    #define RG_HDMI2_REG_DPLL_BYPASS_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_DPLL_CLK_DETECTED Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_REG_DPLL_CH_LOCKED Fld(3,4,AC_MSKB0)//[6:4]
    #define RG_HDMI2_REG_DPLL_SCRAMBLE_STAT Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_500NS_CALIB_CNT Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_1MS_IN_500NS_CNT0 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_1MS_IN_500NS_CNT1 Fld(3,24,AC_MSKB3)//[26:24]
#define HDMI2_SCDCS_20MS_IN_1MS_CNT_PWD (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x044)
    #define RG_HDMI2_REG_20MS_IN_1MS_CNT Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_1S_IN_20MS_CNT Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_CHR_ERRCNT_MAX1 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_CHR_ERRCNT_MAX2 Fld(7,24,AC_MSKB3)//[30:24]
#define HDMI2_HAPLL_REPLACE_APLL2 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x088)
    #define RG_HDMI2_HAPLL_MODULATOR_IN_PUT Fld(25,0,AC_MSKDW)//[24:0]
#define HDMI2_ACR_CONTROL1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x08C)
    #define RG_HDMI2_TT2_NDIV4_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_TT2_CTS_DIV4_CARRY_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_TT2_CTSDIV4_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_TT2_NDIV4_CARRY_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_TT2_NMUL4_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_TT2_CTSMUL4_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_PLL_COREID Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_FS_VAL_SW Fld(4,8,AC_MSKB1)//[11:8]
    #define RG_HDMI2_FM_VAL_SW Fld(2,12,AC_MSKB1)//[13:12]
    #define RG_HDMI2_POST_DIV2_ENA Fld(1,14,AC_MSKB1)//[14:14]
    #define RG_HDMI2_UPLL_VAL_SW Fld(7,16,AC_MSKB2)//[22:16]
    #define RG_HDMI2_POST_VAL_SW Fld(6,24,AC_MSKB3)//[29:24]
#define HDMI2_ACR_CONTROL2 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x090)
    #define RG_HDMI2_N_VAL_SW Fld(20,0,AC_MSKDW)//[19:0]
    #define RG_HDMI2_FS_HW_SW_SEL Fld(1,20,AC_MSKB2)//[20:20]
    #define RG_HDMI2_N_HW_SW_SEL Fld(1,21,AC_MSKB2)//[21:21]
    #define RG_HDMI2_CTS_HW_SW_SEL Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_UPLL_HW_SW_SEL Fld(1,23,AC_MSKB2)//[23:23]
    #define RG_HDMI2_POST_HW_SW_SEL Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_FM_IN_VAL_SW Fld(2,25,AC_MSKB3)//[26:25]
#define HDMI2_ACR_CONTROL3 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x094)
    #define RG_HDMI2_CTS_VAL_SW Fld(20,0,AC_MSKDW)//[19:0]
    #define RG_HDMI2_LK_WIN_SW Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_ACR_CONTROL4 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x098)
    #define RG_HDMI2_LS_THRS_SVAL Fld(20,0,AC_MSKDW)//[19:0]
    #define RG_HDMI2_LOG_WIN_ENA Fld(1,20,AC_MSKB2)//[20:20]
    #define RG_HDMI2_POLE Fld(3,21,AC_MSKB2)//[23:21]
    #define RG_HDMI2_ZERO Fld(3,24,AC_MSKB3)//[26:24]
    #define RG_HDMI2_CLIP Fld(4,27,AC_MSKB3)//[30:27]
    #define RG_HDMI2_WRAP Fld(1,31,AC_MSKB3)//[31:31]
#define HDMI2_ACR_CONTROL5 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x09C)
    #define RG_HDMI2_SDREF Fld(3,0,AC_MSKB0)//[2:0]
    #define RG_HDMI2_USOVRVAL Fld(3,3,AC_MSKB0)//[5:3]
    #define RG_HDMI2_FS32OVRVAL Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_MODOVRVAL Fld(2,8,AC_MSKB1)//[9:8]
    #define RG_HDMI2_USOVR Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_FS32OVR Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_SDINOVR Fld(1,12,AC_MSKB1)//[12:12]
    #define RG_HDMI2_MODOVR Fld(1,13,AC_MSKB1)//[13:13]
    #define RG_HDMI2_CTS_THRESH Fld(4,16,AC_MSKB2)//[19:16]
    #define RG_HDMI2_ACR_INIT_WP Fld(1,20,AC_MSKB2)//[20:20]
    #define RG_HDMI2_CTS_REUSED_AUTO_EN Fld(1,21,AC_MSKB2)//[21:21]
    #define RG_HDMI2_CTS_DROPPED_AUTO_EN Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_NCLKSEL Fld(1,23,AC_MSKB2)//[23:23]
    #define RG_HDMI2_EXT_NCLK Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_PRESDIVSELOVR Fld(1,25,AC_MSKB3)//[25:25]
    #define RG_HDMI2_PREDIVSELOVRSEL Fld(1,26,AC_MSKB3)//[26:26]
#define HDMI2_ACR_CONTROL6 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0A0)
    #define RG_HDMI2_SDINOVRVAL31_0 Fld(32,0,AC_FULLDW)//[31:0]
#define HDMI2_ACR_CONTROL7 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0A4)
    #define RG_HDMI2_SDINOVRVAL39_32 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CTS_FINE_STEP Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CTS_ROUGH_STEP Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_PTR_DIFF_HIGH_TH Fld(6,24,AC_MSKB3)//[29:24]
#define HDMI2_ACR_CONTROL8 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0A8)
    #define RG_HDMI2_CTS_STABLE_CNT_TH Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_CTS_REAL_STABLE_CNT_TH Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_ACR_CONTROL9 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0AC)
    #define RG_HDMI2_PTR_DIFF_LOW_TH Fld(4,0,AC_MSKB0)//[3:0]
    #define RG_HDMI2_CTS_HW_ADP_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_CTS_LOCK_ALG_SEL Fld(3,5,AC_MSKB0)//[7:5]
    #define RG_HDMI2_NCLK_CNT_TH Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CTS_PRE_STABLE_CNT_TH Fld(8,16,AC_FULLB2)//[23:16]
//    #define RG_HDMI2_24M_XTAL_CLK_SEL Fld(1,24,AC_MSKB3)//[24:24]
#define HDMI2_ACR_CONTROL10 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0B0)
    #define RG_HDMI2_FIFO_DIFF_HIGH_TH Fld(5,0,AC_MSKB0)//[4:0]
    #define RG_HDMI2_FIFO_DIFF_LOW_TH Fld(5,8,AC_MSKB1)//[12:8]
#define HDMI2_ACR_CONTROL11 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0B4)
    #define RG_HDMI2_REG_HADDS2_PCW_SW Fld(31,0,AC_MSKDW)//[30:0]
#define HDMI2_ACR_CONTROL12 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0B8)
    #define RG_HDMI2_FB_DIV_SW Fld(7,0,AC_MSKB0)//[6:0]
    #define RG_HDMI2_PCW_CHG_SW Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_PCW_DATA_SEL Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_FB_DIV_DATA_SEL Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_PCW_DATA_X2_SEL Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_PCW_DATA_SEL_FOR_40NM Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_PCW_DATA_DIV Fld(2,12,AC_MSKB1)//[13:12]
    #define RG_HDMI2_REG_APLL_STATUS_SEL Fld(1,14,AC_MSKB1)//[14:14]
#define HDMI2_DIG_PHY_CONTROL1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0BC)
    #define RG_HDMI2_REG_DIG_PHY_RSTB Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_VID_SPLIT_RSTB Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_HDCP_RE_AUTH_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_C_PRBS_AMOUNT Fld(16,8,AC_FULLW21)//[23:8]
    #define RG_HDMI2_C_PRBS_THCNT Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_DIG_PHY_CONTROL2 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0C0)
    #define RG_HDMI2_C_PRBS_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_C_DATA_SYNC_AUTO Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_C_CRC_START Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_C_CRC_MODE Fld(2,12,AC_MSKB1)//[13:12]
    #define RG_HDMI2_C_CRC_CLR Fld(1,14,AC_MSKB1)//[14:14]
    #define RG_HDMI2_C_PHYD_CLK_MODE Fld(4,16,AC_MSKB2)//[19:16]
    #define RG_HDMI2_C_CHANNEL_ALIGN_INCREASE Fld(1,20,AC_MSKB2)//[20:20]
    #define RG_HDMI2_REG_DEEP_STATUS Fld(2,24,AC_MSKB3)//[25:24]
    #define RG_HDMI2_REG_DEEP_SW_SEL Fld(1,27,AC_MSKB3)//[27:27]
    #define RG_HDMI2_REG_CHANNEL_MAPPING Fld(3,28,AC_MSKB3)//[30:28]
    #define RG_HDMI2_REG_LINK_LOST Fld(1,31,AC_MSKB3)//[31:31]
#define HDMI2_DIG_PHY_CONTROL3 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0C4)
    #define RG_HDMI2_REG_SCRAMBLING_ON Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_HW_SCRAMBLING_ON_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_BIST_VIDEO_OUT_SEL Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_PREDFF_CLK_INV Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_APLL_CNTL_SEL Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_REG_ANA_CNTL_SEL Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_REG_MONI_SEL Fld(4,20,AC_MSKB2)//[23:20]
    #define RG_HDMI2_REG_PRAM_8051_CNTL_SEL Fld(1,24,AC_MSKB3)//[24:24]
#define HDMI2_DIG_PHY_STATUS1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0C8)
	#define RG_HDMI2_C_CRC_OUT Fld(24,0,AC_MSKDW)//[23:0]
	#define RG_HDMI2_C_CRC_READY Fld(1,24,AC_MSKB3)//[24:24]
	#define RG_HDMI2_C_CRC_FAIL Fld(1,25,AC_MSKB3)//[25:25]
    #define RG_HDMI2_REG_DIG_PHY_STATUS1 Fld(6,26,AC_MSKB3)//[31:26]
#define HDMI2_DIG_PHY_STATUS2 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0CC)
    #define RG_HDMI2_REG_DIG_PHY_STATUS2 Fld(32,0,AC_FULLDW)//[31:0]
#define HDMI2_TOP_MISC (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0D0)
    #define RG_HDMI2_REG_DSCL_DEGLITCH_CNT Fld(4,0,AC_MSKB0)//[3:0]
    #define RG_HDMI2_REG_DSDA_DEGLITCH_CNT Fld(4,4,AC_MSKB0)//[7:4]
    #define RG_HDMI2_REG_HW_DSD_FEATURE Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_HW_HBR_FEATURE Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_REG_HW_8CH_FEATURE Fld(1,12,AC_MSKB1)//[12:12]
    #define RG_HDMI2_REG_HW_RPT_FEATURE Fld(1,13,AC_MSKB1)//[13:13]
    #define RG_HDMI2_TMDS_CLK_RATIO Fld(1,27,AC_MSKB3)//[27:27]
    #define RG_HDMI2_SCRAMBLING_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define RG_HDMI2_ACR_PLL_LOCK Fld(1,29,AC_MSKB3)//[29:29]
    #define RG_HDMI2_SCDT Fld(1,30,AC_MSKB3)//[30:30]
    #define RG_HDMI2_CKDT Fld(1,31,AC_MSKB3)//[31:31]
#define HDMI2_SPLIT_DITHER1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0D4)
	#define RG_HDMI2_REG_SPLIT_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_LEFT_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_ROUND_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_REG_EDITHER_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REG_RDITHER_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_LFSR_EN Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_REG_TABLE_EN Fld(2,8,AC_MSKB1)//[9:8]
    #define RG_HDMI2_REG_SUBPIX_EN Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_SUB_R Fld(2,11,AC_MSKB1)//[12:11]
    #define RG_HDMI2_REG_SUB_G Fld(2,13,AC_MSKB1)//[14:13]
    #define RG_HDMI2_REG_SUB_B Fld(2,15,AC_MSKW21)//[16:15]
    #define RG_HDMI2_REG_FPHASE_EN Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_REG_FPHASE_R Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_REG_FPHASE Fld(6,19,AC_MSKW32)//[24:19]
    #define RG_HDMI2_REG_FPHASE_CTRL Fld(2,25,AC_MSKB3)//[26:25]
    #define RG_HDMI2_REG_FPHASE_SEL Fld(2,27,AC_MSKB3)//[28:27]
    #define RG_HDMI2_REG_FPHASE_BIT Fld(3,29,AC_MSKB3)//[31:29]
#define HDMI2_SPLIT_DITHER2 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0D8)
    #define RG_HDMI2_REG_INK_DATA_R Fld(12,0,AC_MSKW10)//[11:0]
    #define RG_HDMI2_REG_INK_DATA_G Fld(12,16,AC_MSKW32)//[27:16]
#define HDMI2_SPLIT_DITHER3 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0DC)
    #define RG_HDMI2_REG_INK_DATA_B Fld(12,0,AC_MSKW10)//[11:0]
    #define RG_HDMI2_REG_DRMOD_R Fld(2,12,AC_MSKB1)//[13:12]
    #define RG_HDMI2_REG_DRMOD_G Fld(2,14,AC_MSKB1)//[15:14]
    #define RG_HDMI2_REG_DRMOD_B Fld(2,16,AC_MSKB2)//[17:16]
    #define RG_HDMI2_REG_TWO_CH_LR Fld(1,28,AC_MSKB3)//[28:28]
    #define RG_HDMI2_REG_LSB_OFF Fld(1,30,AC_MSKB3)//[30:30]
    #define RG_HDMI2_REG_INK Fld(1,31,AC_MSKB3)//[31:31]
#define HDMI2_SPLIT_DITHER4 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0E0)
    #define RG_HDMI2_REG_H_ACTIVE Fld(16,0,AC_FULLW10)//[15:0]
#define HDMI2_CKGEN_CONTROL (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0E4)
    #define RG_HDMI2_REG_MBIST0 Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_MBIST1 Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_MBIST2 Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_DPCLK_SEL0 Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REG_DPCLK_SEL1 Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_DPCLK2X_SEL0 Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_REG_DPCLK2X_SEL1 Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_SPLITCLK_SEL0 Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_SPLITCLK_SEL1 Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_CCLK_SEL Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_TMDS_CLK_INV Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_REG_DDC_EN Fld(1,12,AC_MSKB1)//[12:12]
    #define RG_HDMI2_REG_MCLK_SEL Fld(2,13,AC_MSKB1)//[14:13]
    #define RG_HDMI2_REG_PD_TMDS_CLK Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_REG_PD_DPCLK Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_REG_PD_DPCLK2X Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_REG_SCL_DEGLITCH_CNT Fld(4,20,AC_MSKB2)//[23:20]
    #define RG_HDMI2_REG_SDA_DEGLITCH_CNT Fld(4,24,AC_MSKB3)//[27:24]
    #define RG_HDMI2_REG_CKGEN_RST Fld(4,28,AC_MSKB3)//[31:28]
#define HDMI2_CRC_CONTROL (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0EC)
    #define RG_HDMI2_REG_CRC_OUT Fld(24,0,AC_MSKDW)//[0:23]
    #define RG_HDMI2_REG_CRC_Fail Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_REG_CRC_Ready Fld(1,25,AC_MSKB3)//[25:25]
    #define RG_HDMI2_REG_CRC_Start Fld(1,26,AC_MSKB3)//[26:26]
    #define RG_HDMI2_REG_CRC_Clr Fld(1,27,AC_MSKB3)//[27:27]
    #define RG_HDMI2_REG_CRC_Mode Fld(2,29,AC_MSKB3)//[30:29]
#define HDMI2_TMDS_CKDT1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0F4)
    #define RG_HDMI2_CLK_DIV Fld(10,0,AC_MSKW10)//[9:0]
    #define RG_HDMI2_CKDT_L Fld(10,10,AC_MSKW21)//[19:10]
    #define RG_HDMI2_CKDT_H Fld(10,20,AC_MSKW32)//[29:20]
    #define RG_HDMI2_INBAND_THR Fld(2,30,AC_MSKB3)//[31:30]
#define HDMI2_TMDS_CKDT2 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x0F8)
    #define RG_HDMI2_TMDS_CK_PERIOD Fld(8,0,AC_FULLB0)//[7:0]
#define HDMI2_PORT_SEL (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x108)
    #define RG_HDMI2_PORT_SEL Fld(32,0,AC_FULLDW)//[31:0]
#define HDMI2_SHERMAN_RST (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x3FC)
    #define RG_HDMI2_REG_SHERMAN_SW_RST Fld(1,0,AC_MSKB0)//[0:0]
#define HDMI2_RX_ECC_CTRL (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x400)
    #define RG_HDMI2_REG_CAPTURE_CNT Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_IGNORE_ECC_ERR_PKT_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_BCH_THRESH_ Fld(5,8,AC_MSKB1)//[12:8]
    #define RG_HDMI2_REG_HDMI_PKT_THRESH7_0 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_HDMI_PKT_THRESH15_8 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_T4_PKT_THRES (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x404)
    #define RG_HDMI2_REG_T4_PKT_THRESH7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_T4_PKT_THRESH15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_BCH_PKT_THRESH7_0 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_BCH_PKT_THRESH15_8 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_HDCP_THRES (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x408)
    #define RG_HDMI2_REG_HDCP_PKT_THRESH7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_HDCP_PKT_THRESH15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_RHDMI_PKT_CNT7_0 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_RHDMI_PKT_CNT15_8 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_T4_ERR (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x40C)
    #define RG_HDMI2_RHDMI_T4_ERR_CNT7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_RHDMI_T4_ERR_CNT15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_RHDMI_BCH_ERR_CNT7_0 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_RHDMI_BCH_ERR_CNT15_8 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_HDCP_ERR (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x410)
    #define RG_HDMI2_RHDMI_HDCP_ERR_CNT7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_RHDMI_HDCP_ERR_CNT15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_NEW_AVI_ONLY Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_REG_NEW_SPD_ONLY Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_REG_NEW_AUD_ONLY Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_REG_NEW_MPEG_ONLY Fld(1,19,AC_MSKB2)//[19:19]
    #define RG_HDMI2_REG_NEW_UNREC_ONLY Fld(1,20,AC_MSKB2)//[20:20]
    #define RG_HDMI2_REG_NEW_ACP_ONLY Fld(1,21,AC_MSKB2)//[21:21]
    #define RG_HDMI2_REG_NEW_VSI_ONLY Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_REG_USE_AIF4VSI Fld(1,23,AC_MSKB2)//[23:23]
    #define RG_HDMI2_REG_NEW_ISRC1_ONLY Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_REG_NEW_ISRC2_ONLY Fld(1,25,AC_MSKB3)//[25:25]
    #define RG_HDMI2_REG_NEW_GCP_ONLY Fld(1,26,AC_MSKB3)//[26:26]
#define HDMI2_DEC_AV_MUTE (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x414)
    #define RG_HDMI2_REG_VIDEO_MUTE Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_CLEAR_AV_MUTE Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_DC_HEADER Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_PHASE0_LUT Fld(2,16,AC_MSKB2)//[17:16]
    #define RG_HDMI2_REG_PHASE1_LUT Fld(2,18,AC_MSKB2)//[19:18]
    #define RG_HDMI2_REG_PHASE2_LUT Fld(2,20,AC_MSKB2)//[21:20]
    #define RG_HDMI2_REG_PHASE3_LUT Fld(2,22,AC_MSKB2)//[23:22]
    #define RG_HDMI2_HDMI_MODE_DET Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_HDMI_MODE_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define RG_HDMI2_HDMI_MUTE Fld(1,26,AC_MSKB3)//[26:26]
    #define RG_HDMI2_HDMI_LAYOUT Fld(2,27,AC_MSKB3)//[28:27]
    #define RG_HDMI2_HDMI_AUD_DSD_ON Fld(1,29,AC_MSKB3)//[29:29]
    #define RG_HDMI2_HDMI_HBRA_ON Fld(1,30,AC_MSKB3)//[30:30]
#define HDMI2_RX_AUTO_CLR_PKT1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x418)
    #define RG_HDMI2_REG_AUTOCLR_AVI_PKT Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_AUTOCLR_SPD_PKT Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_AUTOCLR_AIF_PKT Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_AUTOCLR_MPEG_PKT Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_REG_AUTOCLR_UNREC_PKT Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REG_AUTOCLR_ACP_PKT Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_AUTOCLR_VSI_PKT Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_REG_AUTOCLR_GCP_PKT Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_AUTOCLR_ISRC1_PKT Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_AUTOCLR_ISRC2_PKT Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_AUTOCLR_METADATA_PKT Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_CEA_AVI_HEADER0 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_AVI_HEADER1 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_AVIRX_LENGTH (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x41C)
    #define RG_HDMI2_CEA_AVI_LENGTH Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_AVI_CHECKSUM Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_AVI_DBYTE1 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_AVI_DBYTE2 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_AVIRX_DBYTE3 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x420)
    #define RG_HDMI2_CEA_AVI_DBYTE3 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_AVI_DBYTE4 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_AVI_DBYTE5 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_AVI_DBYTE6 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_AVIRX_DBYTE7 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x424)
    #define RG_HDMI2_CEA_AVI_DBYTE7 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_AVI_DBYTE8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_AVI_DBYTE9 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_AVI_DBYTE10 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_AVIRX_DBYTE11 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x428)
    #define RG_HDMI2_CEA_AVI_DBYTE11 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_AVI_DBYTE12 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_AVI_DBYTE13 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_AVI_DBYTE14 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_AVIRX_DBYTE15 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x42C)
    #define RG_HDMI2_CEA_AVI_DBYTE15 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_CATCH_SPECIFIC_PKT_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_UNREC_DEC Fld(8,16,AC_FULLB2)//[23:16]
#define HDMI2_SPDRX_TYPE (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x430)
    #define RG_HDMI2_CEA_SPD_HEADER7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_SPD_HEADER15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_SPD_LENGTH Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_SPD_CHECKSUM Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_SPDRX_DBYTE1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x434)
    #define RG_HDMI2_CEA_SPD_DBYTE1 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_SPD_DBYTE2 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_SPD_DBYTE3 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_SPD_DBYTE4 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_SPDRX_DBYTE5 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x438)
    #define RG_HDMI2_CEA_SPD_DBYTE5 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_SPD_DBYTE6 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_SPD_DBYTE7 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_SPD_DBYTE8 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_SPDRX_DBYTE9 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x43C)
    #define RG_HDMI2_CEA_SPD_DBYTE9 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_SPD_DBYTE10 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_SPD_DBYTE11 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_SPD_DBYTE12 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_SPDRX_DBYTE13 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x440)
    #define RG_HDMI2_CEA_SPD_DBYTE13 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_SPD_DBYTE14 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_SPD_DBYTE15 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_SPD_DBYTE16 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_SPDRX_DBYTE17 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x444)
    #define RG_HDMI2_CEA_SPD_DBYTE17 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_SPD_DBYTE18 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_SPD_DBYTE19 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_SPD_DBYTE20 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_SPDRX_DBYTE21 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x448)
    #define RG_HDMI2_CEA_SPD_DBYTE21 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_SPD_DBYTE22 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_SPD_DBYTE23 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_SPD_DBYTE24 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_SPDRX_DBYTE25 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x44C)
    #define RG_HDMI2_CEA_SPD_DBYTE25 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_SPD_DBYTE26 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_SPD_DBYTE27 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_SPD_DEC Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_AUDRX_TYPE (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x450)
    #define RG_HDMI2_CEA_AUD_HEADER7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_AUD_HEADER15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_AUD_LENGTH Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_AUD_CHECKSUM Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_AUDRX_DBYTE1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x454)
    #define RG_HDMI2_CEA_AUD_DBYTE1 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_AUD_DBYTE2 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_AUD_DBYTE3 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_AUD_DBYTE4 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_AUDRX_DBYTE5 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x458)
    #define RG_HDMI2_CEA_AUD_DBYTE5 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_AUD_DBYTE6 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_AUD_DBYTE7 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_AUD_DBYTE8 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_AUDRX_DBYTE9 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x45C)
    #define RG_HDMI2_CEA_AUD_DBYTE9 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_AUD_DBYTE10 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_AUD_DBYTE11 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_AUD_DBYTE12 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_AUDRX_DBYTE13 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x460)
    #define RG_HDMI2_CEA_AUD_DBYTE13 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_AUD_DBYTE14 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_AUD_DBYTE15 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_AUD_DBYTE16 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_AUDRX_DBYTE17 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x464)
    #define RG_HDMI2_CEA_AUD_DBYTE17 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_AUD_DBYTE18 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_AUD_DBYTE19 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_AUD_DBYTE20 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_AUDRX_DBYTE21 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x468)
    #define RG_HDMI2_CEA_AUD_DBYTE21 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_AUD_DBYTE22 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_AUD_DBYTE23 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_AUD_DBYTE24 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_AUDRX_DBYTE25 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x46C)
    #define RG_HDMI2_CEA_AUD_DBYTE25 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_AUD_DBYTE26 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_AUD_DBYTE27 Fld(8,16,AC_FULLB2)//[23:16]
#define HDMI2_MPEGRX_TYPE (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x470)
    #define RG_HDMI2_CEA_MPEG_HEADER7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_MPEG_HEADER15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_MPEG_LENGTH Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_MPEG_CHECKSUM Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_MPEGRX_DBYTE1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x474)
    #define RG_HDMI2_CEA_MPEG_DBYTE1 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_MPEG_DBYTE2 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_MPEG_DBYTE3 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_MPEG_DBYTE4 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_MPEGRX_DBYTE5 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x478)
    #define RG_HDMI2_CEA_MPEG_DBYTE5 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_MPEG_DBYTE6 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_MPEG_DBYTE7 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_MPEG_DBYTE8 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_MPEGRX_DBYTE9 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x47C)
    #define RG_HDMI2_CEA_MPEG_DBYTE9 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_MPEG_DBYTE10 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_MPEG_DBYTE11 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_MPEG_DBYTE12 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_MPEGRX_DBYTE13 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x480)
    #define RG_HDMI2_CEA_MPEG_DBYTE13 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_MPEG_DBYTE14 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_MPEG_DBYTE15 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_MPEG_DBYTE16 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_MPEGRX_DBYTE17 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x484)
    #define RG_HDMI2_CEA_MPEG_DBYTE17 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_MPEG_DBYTE18 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_MPEG_DBYTE19 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_MPEG_DBYTE20 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_MPEGRX_DBYTE21 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x488)
    #define RG_HDMI2_CEA_MPEG_DBYTE21 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_MPEG_DBYTE22 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_MPEG_DBYTE23 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_MPEG_DBYTE24 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_MPEGRX_DBYTE25 (IO_VIRT_HDMI + IO_HDMI_BASE + 0x48C)
    #define RG_HDMI2_CEA_MPEG_DBYTE25 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_MPEG_DBYTE26 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_MPEG_DBYTE27 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_MPEG_DEC Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_UNREC_BYTE1 (IO_VIRT_HDMI + IO_HDMI_BASE + 0x490)
    #define RG_HDMI2_CEA_UNREC_BYTE1 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_UNREC_BYTE2 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_UNREC_BYTE3 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_UNREC_BYTE4 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_UNREC_BYTE5 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x494)
    #define RG_HDMI2_CEA_UNREC_BYTE5 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_UNREC_BYTE6 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_UNREC_BYTE7 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_UNREC_BYTE8 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_UNREC_BYTE9 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x498)
    #define RG_HDMI2_CEA_UNREC_BYTE9 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_UNREC_BYTE10 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_UNREC_BYTE11 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_UNREC_BYTE12 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_UNREC_BYTE13 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x49C)
    #define RG_HDMI2_CEA_UNREC_BYTE13 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_UNREC_BYTE14 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_UNREC_BYTE15 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_UNREC_BYTE16 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_UNREC_BYTE17 (IO_VIRT_HDMI2 + IO_HDMI2_BASE+ 0x4A0)
    #define RG_HDMI2_CEA_UNREC_BYTE17 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_UNREC_BYTE18 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_UNREC_BYTE19 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_UNREC_BYTE20 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_UNREC_BYTE21 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4A4)
    #define RG_HDMI2_CEA_UNREC_BYTE21 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_UNREC_BYTE22 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_UNREC_BYTE23 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_UNREC_BYTE24 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_UNREC_BYTE25 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4A8)
    #define RG_HDMI2_CEA_UNREC_BYTE25 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_UNREC_BYTE26 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_UNREC_BYTE27 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_UNREC_BYTE28 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_UNREC_BYTE29 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4AC)
    #define RG_HDMI2_CEA_UNREC_BYTE29 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_UNREC_BYTE30 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_UNREC_BYTE31 Fld(8,16,AC_FULLB2)//[23:16]
#define HDMI2_CPRX_BYTE1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4B0)
    #define RG_HDMI2_REG_CP_SET_MUTE Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_CP_CLR_MUTE Fld(1,4,AC_MSKB0)//[4:4]
#define HDMI2_RX_ACP_BYTE1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4B4)
    #define RG_HDMI2_CEA_ACP_BYTE1 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_ACP_BYTE2 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_ACP_BYTE3 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_ACP_BYTE4 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_ACP_BYTE5 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4B8)
    #define RG_HDMI2_CEA_ACP_BYTE5 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_ACP_BYTE6 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_ACP_BYTE7 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_ACP_BYTE8 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_ACP_BYTE9 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4BC)
    #define RG_HDMI2_CEA_ACP_BYTE9 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_ACP_BYTE10 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_ACP_BYTE11 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_ACP_BYTE12 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_ACP_BYTE13 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4C0)
    #define RG_HDMI2_CEA_ACP_BYTE13 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_ACP_BYTE14 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_ACP_BYTE15 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_ACP_BYTE16 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_ACP_BYTE17 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4C4)
    #define RG_HDMI2_CEA_ACP_BYTE17 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_ACP_BYTE18 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_ACP_BYTE19 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_ACP_BYTE20 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_ACP_BYTE21 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4C8)
    #define RG_HDMI2_CEA_ACP_BYTE21 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_ACP_BYTE22 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_ACP_BYTE23 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_ACP_BYTE24 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_ACP_BYTE25 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4CC)
    #define RG_HDMI2_CEA_ACP_BYTE25 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_ACP_BYTE26 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_ACP_BYTE27 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_ACP_BYTE28 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_ACP_BYTE29 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4D0)
    #define RG_HDMI2_CEA_ACP_BYTE29 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_ACP_BYTE30 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_ACP_BYTE31 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_ACP_DEC Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_VSI_CTRL1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4D4)
    #define RG_HDMI2_REG_SWAP_VSI_IEEE_ID Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_AIF_CLR_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_NO_PKT_CNT Fld(3,2,AC_MSKB0)//[4:2]
    #define RG_HDMI2_REG_AIF4VSI_CMP Fld(2,5,AC_MSKB0)//[6:5]
    #define RG_HDMI2_REG_VSI_CLR_EN Fld(1,7,AC_MSKB0)//[7:7]
#define HDMI2_VSI_ID1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4D8)
    #define RG_HDMI2_REG_ALT_VSI_PB1 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_ALT_VSI_PB2 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_ALT_VSI_PB3 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_ALT_VSI_PB4 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_VSI_PKT_ID (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4DC)
    #define RG_HDMI2_REG_VSI_HEADER Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_AIF_HEADER Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_VSI_IEEE_ID_CHK_EN Fld(1,16,AC_MSKB2)//[16:16]
#define HDMI2_IF_CTRL1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4E0)
    #define RG_HDMI2_REG_AVI_CLR_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_SPD_CLR_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_MPEG_CLR_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_UNREC_CLR_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_REG_ACP_CLR_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REG_EN_CHECKSUM_CHECK Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_ISRC1_CLR_EN Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_REG_ISRC2_CLR_EN Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_GCP_CLR_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_META_DATA_PKT_CLR_EN Fld(1,9,AC_MSKB1)//[9:9]
#define HDMI2_VSIF_ID1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4E4)
    #define RG_HDMI2_REG_VSI_PB1 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_VSI_PB2 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_VSI_PB3 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_VSI_PB4 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_VSI_CTRL3 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4E8)
    #define RG_HDMI2_REG_AIF4VSI_IND_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_CEA_VSI_HEADER7_0 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_VSI_HEADER15_8 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_VSI_LENGTH Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_VSIRX_DBYTE0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4EC)
    #define RG_HDMI2_CEA_VSI_PB_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_VSI_PB_1 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_VSI_PB_2 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_VSI_PB_3 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_VSIRX_DBYTE4 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x4F0)
    #define RG_HDMI2_CEA_VSI_PB_4 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_VSI_PB_5 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_VSI_PB_6 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_VSI_PB_7 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_VSIRX_DBYTE8 (IO_VIRT_HDMI2 + IO_HDMI2_BASE+ 0x4F4)
    #define RG_HDMI2_CEA_VSI_PB_8 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_VSI_PB_9 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_VSI_PB_10 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_VSI_PB_11 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_VSIRX_DBYTE12 (IO_VIRT_HDMI2 + IO_HDMI2_BASE+ 0x4F8)
    #define RG_HDMI2_CEA_VSI_PB_12 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_VSI_PB_13 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_VSI_PB_14 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_VSI_PB_15 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_VSIRX_DBYTE16 (IO_VIRT_HDMI2 + IO_HDMI2_BASE+ 0x4FC)
    #define RG_HDMI2_CEA_VSI_PB_16 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_VSI_PB_17 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_VSI_PB_18 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_VSI_PB_19 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_VSIRX_DBYTE20 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x500)
    #define RG_HDMI2_CEA_VSI_PB_20 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_VSI_PB_21 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_VSI_PB_22 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_VSI_PB_23 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_VSIRX_DBYTE24 (IO_VIRT_HDMI + IO_HDMI_BASE + 0x504)
    #define RG_HDMI2_CEA_VSI_PB_24 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_VSI_PB_25 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_VSI_PB_26 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_VSI_PB_27 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_ISRC1_TYPE (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x508)
    #define RG_HDMI2_CEA_ISRC1_HEADER7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_ISRC1_HEADER15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_ISRC1_LENGTH Fld(8,16,AC_FULLB2)//[23:16]
#define HDMI2_RX_ISRC1_DBYTE0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x50C)
    #define RG_HDMI2_CEA_ISRC1_PB_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_ISRC1_PB_1 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_ISRC1_PB_2 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_ISRC1_PB_3 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_ISRC1_DBYTE4 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x510)
    #define RG_HDMI2_CEA_ISRC1_PB_4 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_ISRC1_PB_5 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_ISRC1_PB_6 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_ISRC1_PB_7 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_ISRC1_DBYTE8 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x514)
    #define RG_HDMI2_CEA_ISRC1_PB_8 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_ISRC1_PB_9 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_ISRC1_PB_10 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_ISRC1_PB_11 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_ISRC1_DBYTE12 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x518)
    #define RG_HDMI2_CEA_ISRC1_PB_12 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_ISRC1_PB_13 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_ISRC1_PB_14 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_ISRC1_PB_15 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_ISRC1_DEC (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x51C)
    #define RG_HDMI2_REG_ISRC1_DEC Fld(8,0,AC_FULLB0)//[7:0]
#define HDMI2_RX_ISRC2_TYPE (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x520)
    #define RG_HDMI2_CEA_ISRC2_HEADER7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_ISRC2_HEADER15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_ISRC2_LENGTH Fld(8,16,AC_FULLB2)//[23:16]
#define HDMI2_RX_ISRC2_DBYTE0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x524)
    #define RG_HDMI2_CEA_ISRC2_PB_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_ISRC2_PB_1 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_ISRC2_PB_2 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_ISRC2_PB_3 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_ISRC2_DBYTE4 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x528)
    #define RG_HDMI2_CEA_ISRC2_PB_4 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_ISRC2_PB_5 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_ISRC2_PB_6 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_ISRC2_PB_7 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_ISRC2_DBYTE8 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x52C)
    #define RG_HDMI2_CEA_ISRC2_PB_8 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_ISRC2_PB_9 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_ISRC2_PB_10 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_ISRC2_PB_11 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_ISRC2_DBYTE12 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x530)
    #define RG_HDMI2_CEA_ISRC2_PB_12 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_ISRC2_PB_13 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_ISRC2_PB_14 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_ISRC2_PB_15 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_ISRC2_DEC (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x534)
    #define RG_HDMI2_REG_ISRC2_DEC Fld(8,0,AC_FULLB0)//[7:0]
#define HDMI2_RX_GCP_TYPE (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x538)
    #define RG_HDMI2_CEA_GCP_HEADER7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_GCP_HEADER15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_GCP_LENGTH Fld(8,16,AC_FULLB2)//[23:16]
#define HDMI2_RX_GCP_DBYTE0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x53C)
    #define RG_HDMI2_CEA_GCP_PB_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_GCP_PB_1 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_GCP_PB_2 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_GCP_PB_3 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_GCP_DBYTE4 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x540)
    #define RG_HDMI2_CEA_GCP_PB_4 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_GCP_PB_5 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_GCP_PB_6 Fld(8,16,AC_FULLB2)//[23:16]
#define HDMI2_HF_VSIRX_TYPE (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x544)
    #define RG_HDMI2_CEA_HF_VSI_HEADER7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_HF_VSI_HEADER15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_HF_VSI_LENGTH Fld(8,16,AC_FULLB2)//[23:16]
#define HDMI2_HF_VSIRX_DBYTE0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x548)
    #define RG_HDMI2_CEA_HF_VSI_PB_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_HF_VSI_PB_1 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_HF_VSI_PB_2 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_HF_VSI_PB_3 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_HF_VSIRX_DBYTE4 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x54C)
    #define RG_HDMI2_CEA_HF_VSI_PB_4 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_HF_VSI_PB_5 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_HF_VSI_PB_6 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_HF_VSI_PB_7 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_HF_VSIRX_DBYTE8 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x550)
    #define RG_HDMI2_CEA_HF_VSI_PB_8 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_HF_VSI_PB_9 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_HF_VSI_PB_10 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_HF_VSI_PB_11 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_HF_VSIRX_DBYTE12 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x554)
    #define RG_HDMI2_CEA_HF_VSI_PB_12 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_HF_VSI_PB_13 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_HF_VSI_PB_14 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_HF_VSI_PB_15 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_HF_VSIRX_DBYTE16 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x558)
    #define RG_HDMI2_CEA_HF_VSI_PB_16 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_HF_VSI_PB_17 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_HF_VSI_PB_18 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_HF_VSI_PB_19 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_HF_VSIRX_DBYTE20 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x55C)
    #define RG_HDMI2_CEA_HF_VSI_PB_20 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_HF_VSI_PB_21 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_HF_VSI_PB_22 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_HF_VSI_PB_23 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_HF_VSIRX_DBYTE24 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x560)
    #define RG_HDMI2_CEA_HF_VSI_PB_24 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CEA_HF_VSI_PB_25 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CEA_HF_VSI_PB_26 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_CEA_HF_VSI_PB_27 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_HF_VSI_PKT_ID (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x564)
    #define RG_HDMI2_REG_HF_VSI_HEADER Fld(8,0,AC_FULLB0)//[7:0]
#define HDMI2_HF_VSIF_ID1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x568)
    #define RG_HDMI2_REG_HF_VSI_PB1 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_HF_VSI_PB2 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_HF_VSI_PB3 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_NEW_HF_VSI_ONLY Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_REG_SWAP_HF_VSI_IEEE_ID Fld(1,25,AC_MSKB3)//[25:25]
    #define RG_HDMI2_REG_HF_VSI_CLR_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define RG_HDMI2_REG_HF_VSI_IEEE_ID_CHK_EN Fld(1,27,AC_MSKB3)//[27:27]
#define HDMI2_METADATA_HEADER_BYTE0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x56C)
    #define RG_HDMI2_META_DATA_HEADER7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_META_DATA_HEADER15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_META_DATA_HEADER23_16 Fld(8,16,AC_FULLB2)//[23:16]
#define HDMI2_METADATA_DBYTE0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x570)
    #define RG_HDMI2_METADATA_PB_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_METADATA_PB_1 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_METADATA_PB_2 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_METADATA_PB_3 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_METADATA_DBYTE4 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x574)
    #define RG_HDMI2_METADATA_PB_4 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_METADATA_PB_5 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_METADATA_PB_6 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_METADATA_PB_7 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_METADATA_DBYTE8 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x57C)
    #define RG_HDMI2_METADATA_PB_8 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_METADATA_PB_9 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_METADATA_PB_10 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_METADATA_PB_11 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_METADATA_DBYTE12 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x580)
    #define RG_HDMI2_METADATA_PB_12 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_METADATA_PB_13 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_METADATA_PB_14 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_METADATA_PB_15 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_METADATA_DBYTE16 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x584)
    #define RG_HDMI2_METADATA_PB_16 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_METADATA_PB_17 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_METADATA_PB_18 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_METADATA_PB_19 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_METADATA_DBYTE20 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x588)
    #define RG_HDMI2_METADATA_PB_20 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_METADATA_PB_21 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_METADATA_PB_22 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_METADATA_PB_23 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_METADATA_DBYTE24 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x58C)
    #define RG_HDMI2_METADATA_PB_24 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_METADATA_PB_25 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_METADATA_PB_26 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_METADATA_PB_27 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_FEATURES2 (IO_VIRT_HDMI2+ IO_HDMI2_BASE + 0x800)
    #define RG_HDMI2_VIDEO_PATH_CORE Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_VI_MUXING Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_VI_FDET Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_VI_SYNC_ADJUST Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_VI_RATECONV Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_VI_RETIMING Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_DEC656 Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_DEGEN Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_PIXCAP_IN_2 Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_C420_C422_2 Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_C422_C444_2 Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_MULTI_CSC0 Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_PIXCAP_PRE Fld(1,12,AC_MSKB1)//[12:12]
    #define RG_HDMI2_PWLI0 Fld(1,13,AC_MSKB1)//[13:13]
    #define RG_HDMI2_CMS_MATRIX Fld(1,14,AC_MSKB1)//[14:14]
    #define RG_HDMI2_PWLI1 Fld(1,15,AC_MSKB1)//[15:15]
    #define RG_HDMI2_PWLI2 Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_PIXCAP_POST Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_MULTI_CSC1 Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_C444_C422 Fld(1,19,AC_MSKB2)//[19:19]
    #define RG_HDMI2_C422_C420 Fld(1,20,AC_MSKB2)//[20:20]
    #define RG_HDMI2_DITHER_RND_2 Fld(1,21,AC_MSKB2)//[21:21]
    #define RG_HDMI2_RANGE_CLIP_2 Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_PIXCAP_OUT_2 Fld(1,23,AC_MSKB2)//[23:23]
    #define RG_HDMI2_VO_RATECONV Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_VO_BLANK Fld(1,25,AC_MSKB3)//[25:25]
    #define RG_HDMI2_VO_EMBD_SYNC_ENC Fld(1,26,AC_MSKB3)//[26:26]
    #define RG_HDMI2_VO_MUXING Fld(1,27,AC_MSKB3)//[27:27]
#define HDMI2_BUILD_TIME1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x808)
    #define RG_HDMI2_BUILD_TIME1 Fld(32,0,AC_FULLDW)//[31:0]
#define HDMI2_SOFTWARE_RESET (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x80C)
    #define RG_HDMI2_RESET_CLK_IN Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_RESET_CLK_CORE Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_RESET_CLK_OUT Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_DATA_BITS_VALUE Fld(8,16,AC_FULLB2)//[23:16]
#define HDMI2_INPUT_MUTE (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x810)
    #define RG_HDMI2_VSYNC_DISABLE Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_HSYNC_DISABLE Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_DE_INPUT_DISABLE Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_Y_INPUT_DISABLE Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_CB_INPUT_DISABLE Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_CR_INPUT_DISABLE Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_VSYNC_INPUT_POLARITY Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_HSYNC_INPUT_POLARITY Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_DE_POLARITY Fld(1,18,AC_MSKB2)//[18:18]
#define HDMI2_INPUT_FORMAT (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x814)
    #define RG_HDMI2_DDR_ENABLE Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_DDR_POLARITY Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_DDR_MODE Fld(3,2,AC_MSKB0)//[4:2]
    #define RG_HDMI2_YC_DEMUX_ENABLE Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_YC_DEMUX_POLARITY Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_CBCR_ORDER Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_PIXEL_RATE Fld(2,8,AC_MSKB1)//[9:8]
    #define RG_HDMI2_MUX_420_ENABLE Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_MUX_CB_OR_CR Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_REVERSE_INPUT_Y Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_REVERSE_INPUT_CB Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_REVERSE_INPUT_CR Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_SELECT_INPUT_Y Fld(3,19,AC_MSKB2)//[21:19]
    #define RG_HDMI2_SELECT_INPUT_CB Fld(3,22,AC_MSKW32)//[24:22]
    #define RG_HDMI2_SELECT_INPUT_CR Fld(3,25,AC_MSKB3)//[27:25]
#define HDMI2_INPUT_MASK (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x818)
    #define RG_HDMI2_DISABLE_LSBS_INPUT_Y Fld(2,0,AC_MSKB0)//[1:0]
    #define RG_HDMI2_DISABLE_LSBS_INPUT_CB Fld(2,2,AC_MSKB0)//[3:2]
    #define RG_HDMI2_DISABLE_LSBS_INPUT_CR Fld(2,4,AC_MSKB0)//[5:4]
#define HDMI2_INPUT_SYNC_ADJUST_CONFIG (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x81C)
    #define RG_HDMI2_AUTO_DISABLE Fld(1,0,AC_MSKB0)//[0:0]
#define HDMI2_DEGEN_CONFIG (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x820)
    #define RG_HDMI2_ENABLE_DE Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_DEGEN_PIXEL_DELAY Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_DEGEN_PIXEL_COUNT_MINUS_ONE (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x824)
    #define RG_HDMI2_DEGEN_PIXEL_COUNT_MINUS_ONE Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_DEGEN_LINE_DELAY Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_DEGEN_LINE_COUNT (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x828)
    #define RG_HDMI2_DEGEN_LINE_COUNT Fld(16,0,AC_FULLW10)//[15:0]
#define HDMI2_DEC656_CONFIG (IO_VIRT_HDMI2 + IO_HDMI2_BASE+ 0x830)
    #define RG_HDMI2_ENABLE_656 Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_INVERT_FIELD_ID Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_FORCE_YC_PHASE Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_YC_PHASE_VALUE Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_SKIP_FIRST_EVEN_LINE Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_SKIP_FIRST_ODD_LINE Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_DEC656_DELAY_EAV_TO_HSYNC_ACTIVE Fld(26,6,AC_MSKDW)//[31:6]
#define HDMI2_DEC656_PULSE_WIDTH_HSYNC_MINUS_ONE (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x834)
    #define RG_HDMI2_DEC656_PULSE_WIDTH_HSYNC_MINUS_ONE Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_DEC656_VSYNC_FRONT_PORCH Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_DEC656_EAV_TO_VSYNC_DELAY_EVEN (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x838)
    #define RG_HDMI2_DEC656_EAV_TO_VSYNC_DELAY_EVEN Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_DEC656_EAV_TO_VSYNC_DELAY_ODD Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_DEC656_PULSE_WIDTH_VSYNC (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x83C)
    #define RG_HDMI2_DEC656_PULSE_WIDTH_VSYNC Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_INTERLACED_DEC656 Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_VIDEO656 Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_YC_PHASE Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_VIDEO656_ALIGNMENT Fld(3,19,AC_MSKB2)//[21:19]
#define HDMI2_DEC656_STATUS (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x840)
    #define RG_HDMI2_VSYNC_DISABLE Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_HSYNC_DISABLE Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_CSYNC_DISABLE Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_DE_OUTPUT_DISABLE Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_Y_OUTPUT_DISABLE Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_CB_OUTPUT_DISABLE Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_CR_OUTPUT_DISABLE Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_TRISTATE Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_VSYNC_POLARITY Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_HSYNC_DEC656_POLARITY Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_CSYNC_DEC656_POLARITY Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_DE_656_POLARITY Fld(1,19,AC_MSKB2)//[19:19]
    #define RG_HDMI2_SWAP_HS_CS Fld(1,20,AC_MSKB2)//[20:20]
#define HDMI2_OUTPUT_MAPPING (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x844)
    #define RG_HDMI2_REVERSE_OUTPUT_Y Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REVERSE_OUTPUT_CB Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REVERSE_OUTPUT_CR Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_SELECT_OUTPUT_Y Fld(3,3,AC_MSKB0)//[5:3]
    #define RG_HDMI2_SELECT_OUTPUT_CB Fld(3,6,AC_MSKW10)//[8:6]
    #define RG_HDMI2_SELECT_OUTPUT_CR Fld(3,9,AC_MSKB1)//[11:9]
    #define RG_HDMI2_DISABLE_LSBS_OUTPUT_Y Fld(2,16,AC_MSKB2)//[17:16]
    #define RG_HDMI2_DISABLE_LSBS_OUTPUT_CB Fld(2,18,AC_MSKB2)//[19:18]
    #define RG_HDMI2_DISABLE_LSBS_OUTPUT_CR Fld(2,20,AC_MSKB2)//[21:20]
#define HDMI2_OUTPUT_FORMAT (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x848)
    #define RG_HDMI2_DDR_ENABLE Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_DDR_POLARITY Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_DDR_MODE Fld(3,2,AC_MSKB0)//[4:2]
    #define RG_HDMI2_YC_MUX_ENABLE Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_YC_MUX_POLARITY Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_CBCR_ORDER Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_PIXEL_RATE Fld(2,8,AC_MSKB1)//[9:8]
    #define RG_HDMI2_DEMUX_420_ENABLE Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_DEMUX_CB_OR_CR Fld(1,11,AC_MSKB1)//[11:11]
#define HDMI2_OUTPUT_BLANK_CONFIG (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x850)
    #define RG_HDMI2_ENABLE_BLANKING_OVERRIDE Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_ENABLE_ACTIVE_OVERRIDE Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_CHROMA_MODE Fld(2,2,AC_MSKB0)//[3:2]
    #define RG_HDMI2_OUTPUT_BLANK_Y Fld(12,16,AC_MSKW32)//[27:16]
#define HDMI2_OUTPUT_BLANK_CB (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x854)
    #define RG_HDMI2_OUTPUT_BLANK_CB Fld(12,0,AC_MSKW10)//[11:0]
    #define RG_HDMI2_OUTPUT_BLANK_CR Fld(12,16,AC_MSKW32)//[27:16]
#define HDMI2_OUTPUT_ACTIVE_Y (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x858)
    #define RG_HDMI2_OUTPUT_ACTIVE_Y Fld(12,0,AC_MSKW10)//[11:0]
    #define RG_HDMI2_OUTPUT_ACTIVE_CB Fld(12,16,AC_MSKW32)//[27:16]
#define HDMI2_OUTPUT_ACTIVE_CR (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x85C)
    #define RG_HDMI2_OUTPUT_ACTIVE_CR Fld(12,0,AC_MSKW10)//[11:0]
#define HDMI2_FDET_CONFIG (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x880)
    #define RG_HDMI2_SYNC_POLARITY_FORCE Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_HSYNC_FORCE_POLARITY Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_VSYNC_FORCE_POLARITY Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_ENABLE_656_0 Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_HSYNC_POLARITY_DET Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_VSYNC_POLARITY_DET Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_INTERLACED Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_VIDEO656_DET Fld(1,11,AC_MSKB1)//[11:11]
#define HDMI2_FDET_FRAME_RATE_DELTA_THRESHOLD (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x884)
    #define RG_HDMI2_FDET_FRAME_RATE_DELTA_THRESHOLD Fld(24,0,AC_MSKDW)//[23:0]
#define HDMI2_FDET_FRAME_RATE (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x888)
    #define RG_HDMI2_FDET_FRAME_RATE Fld(24,0,AC_MSKDW)//[23:0]
#define HDMI2_FDET_PIXEL_COUNT (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x88C)
    #define RG_HDMI2_FDET_PIXEL_COUNT Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_FDET_LINE_COUNT Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_FDET_HSYNC_LOW_COUNT (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x890)
    #define RG_HDMI2_FDET_HSYNC_LOW_COUNT Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_FDET_HSYNC_HIGH_COUNT Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_FDET_HFRONT_COUNT (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x894)
    #define RG_HDMI2_FDET_HFRONT_COUNT Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_FDET_HBACK_COUNT Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_FDET_VSYNC_LOW_COUNT_EVEN (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x898)
    #define RG_HDMI2_FDET_VSYNC_LOW_COUNT_EVEN Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_FDET_VSYNC_HIGH_COUNT_EVEN Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_FDET_VFRONT_COUNT_EVEN (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x89C)
    #define RG_HDMI2_FDET_VFRONT_COUNT_EVEN Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_FDET_VBACK_COUNT_EVEN Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_FDET_VSYNC_LOW_COUNT_ODD (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x8A0)
    #define RG_HDMI2_FDET_VSYNC_LOW_COUNT_ODD Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_FDET_VSYNC_HIGH_COUNT_ODD Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_FDET_VFRONT_COUNT_ODD (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x8A4)
    #define RG_HDMI2_FDET_VFRONT_COUNT_ODD Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_FDET_VBACK_COUNT_ODD Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_FDET_FRAME_COUNT (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x8A8)
    #define RG_HDMI2_FDET_FRAME_COUNT Fld(16,0,AC_FULLW10)//[15:0]
#define HDMI2_EMBD_SYNC_ENC_CONFIG (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x8C0)
    #define RG_HDMI2_ENABLE_EMBD Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_FIELD_ID_POLARITY Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_INPUT_IS_RGB1 Fld(1,2,AC_MSKB0)//[2:2]
#define HDMI2_EMBD_SYNC_ENC_SECT0_CONFIG (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x8C4)
    #define RG_HDMI2_SAV Fld(3,0,AC_MSKB0)//[2:0]
    #define RG_HDMI2_EAV Fld(3,3,AC_MSKB0)//[5:3]
    #define RG_HDMI2_LINES Fld(16,6,AC_MSKDW)//[21:6]
    #define RG_HDMI2_DATA Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_LAST Fld(1,23,AC_MSKB2)//[23:23]
#define HDMI2_EMBD_SYNC_ENC_SECT1_CONFIG (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x8C8)
    #define RG_HDMI2_SAV Fld(3,0,AC_MSKB0)//[2:0]
    #define RG_HDMI2_EAV Fld(3,3,AC_MSKB0)//[5:3]
    #define RG_HDMI2_LINES Fld(16,6,AC_MSKDW)//[21:6]
    #define RG_HDMI2_DATA Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_LAST Fld(1,23,AC_MSKB2)//[23:23]
#define HDMI2_EMBD_SYNC_ENC_SECT2_CONFIG (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x8CC)
    #define RG_HDMI2_SAV Fld(3,0,AC_MSKB0)//[2:0]
    #define RG_HDMI2_EAV Fld(3,3,AC_MSKB0)//[5:3]
    #define RG_HDMI2_LINES Fld(16,6,AC_MSKDW)//[21:6]
    #define RG_HDMI2_DATA Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_LAST Fld(1,23,AC_MSKB2)//[23:23]
#define HDMI2_EMBD_SYNC_ENC_SECT3_CONFIG (IO_VIRT_HDMI + IO_HDMI_BASE + 0x8D0)
    #define RG_HDMI2_SAV Fld(3,0,AC_MSKB0)//[2:0]
    #define RG_HDMI2_EAV Fld(3,3,AC_MSKB0)//[5:3]
    #define RG_HDMI2_LINES Fld(16,6,AC_MSKDW)//[21:6]
    #define RG_HDMI2_DATA Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_LAST Fld(1,23,AC_MSKB2)//[23:23]
#define HDMI2_EMBD_SYNC_ENC_SECT4_CONFIG (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x8D4)
    #define RG_HDMI2_SAV Fld(3,0,AC_MSKB0)//[2:0]
    #define RG_HDMI2_EAV Fld(3,3,AC_MSKB0)//[5:3]
    #define RG_HDMI2_LINES Fld(16,6,AC_MSKDW)//[21:6]
    #define RG_HDMI2_DATA Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_LAST Fld(1,23,AC_MSKB2)//[23:23]
#define HDMI2_EMBD_SYNC_ENC_SECT5_CONFIG (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x8D8)
    #define RG_HDMI2_SAV Fld(3,0,AC_MSKB0)//[2:0]
    #define RG_HDMI2_EAV Fld(3,3,AC_MSKB0)//[5:3]
    #define RG_HDMI2_LINES Fld(16,6,AC_MSKDW)//[21:6]
    #define RG_HDMI2_DATA Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_LAST Fld(1,23,AC_MSKB2)//[23:23]
#define HDMI2_EMBD_SYNC_ENC_HORIZONTAL_DATA_PIXELS (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x8DC)
    #define RG_HDMI2_EMBD_SYNC_ENC_HORIZONTAL_DATA_PIXELS Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_EMBD_SYNC_ENC_HORIZONTAL_BLANKING_PIXELS Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_EMBD_SYNC_ENC_UPDATE_REQUEST (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x8E0)
    #define RG_HDMI2_EMBD_SYNC_ENC_UPDATE_REQUEST Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_READ_BANK1 Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_WRITE_BANK1 Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_UPDATE_MODE1 Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_ALL_UPDATE_BYPASS Fld(1,11,AC_MSKB1)//[11:11]
#define HDMI2_RESIGET_RW_ENABLE1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE  + 0x8E4)
    #define RG_HDMI2_FDET_FRAME_RATE_WR_EN Fld(3,0,AC_MSKB0)//[2:0]
    #define RG_HDMI2_FDET_FRAME_RATE_RD_EN Fld(3,3,AC_MSKB0)//[5:3]
    #define RG_HDMI2_FDET_PIXEL_COUNT_WR_EN Fld(2,6,AC_MSKB0)//[7:6]
    #define RG_HDMI2_FDET_PIXEL_COUNT_RD_EN Fld(2,8,AC_MSKB1)//[9:8]
    #define RG_HDMI2_FDET_LINE_COUNT_WR_EN Fld(2,10,AC_MSKB1)//[11:10]
    #define RG_HDMI2_FDET_LINE_COUNT_RD_EN Fld(2,12,AC_MSKB1)//[13:12]
    #define RG_HDMI2_FDET_HSYNC_LOW_COUNT_WR_EN Fld(2,14,AC_MSKB1)//[15:14]
    #define RG_HDMI2_FDET_HSYNC_LOW_COUNT_RD_EN Fld(2,16,AC_MSKB2)//[17:16]
    #define RG_HDMI2_FDET_HSYNC_HIGH_COUNT_WR_EN Fld(2,18,AC_MSKB2)//[19:18]
    #define RG_HDMI2_FDET_HSYNC_HIGH_COUNT_RD_EN Fld(2,20,AC_MSKB2)//[21:20]
    #define RG_HDMI2_FDET_HFRONT_COUNT_WR_EN Fld(2,22,AC_MSKB2)//[23:22]
    #define RG_HDMI2_FDET_HFRONT_COUNT_RD_EN Fld(2,24,AC_MSKB3)//[25:24]
    #define RG_HDMI2_FDET_HBACK_COUNT_WR_EN Fld(2,26,AC_MSKB3)//[27:26]
    #define RG_HDMI2_FDET_HBACK_COUNT_RD_EN Fld(2,28,AC_MSKB3)//[29:28]
    #define RG_HDMI2_FDET_VSYNC_LOW_COUNT_EVEN_WR_EN Fld(2,30,AC_MSKB3)//[31:30]
#define HDMI2_RESIGET_RW_ENABLE2 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x8E8)
    #define RG_HDMI2_FDET_VSYNC_LOW_COUNT_EVEN_RD_EN Fld(2,0,AC_MSKB0)//[1:0]
    #define RG_HDMI2_FDET_VSYNC_HIGH_COUNT_EVEN_WR_EN Fld(2,2,AC_MSKB0)//[3:2]
    #define RG_HDMI2_FDET_VSYNC_HIGH_COUNT_EVEN_RD_EN Fld(2,4,AC_MSKB0)//[5:4]
    #define RG_HDMI2_FDET_VFRONT_COUNT_EVEN_WR_EN Fld(2,6,AC_MSKB0)//[7:6]
    #define RG_HDMI2_FDET_VFRONT_COUNT_EVEN_RD_EN Fld(2,8,AC_MSKB1)//[9:8]
    #define RG_HDMI2_FDET_VBACK_COUNT_EVEN_WR_EN Fld(2,10,AC_MSKB1)//[11:10]
    #define RG_HDMI2_FDET_VBACK_COUNT_EVEN_RD_EN Fld(2,12,AC_MSKB1)//[13:12]
    #define RG_HDMI2_FDET_VSYNC_LOW_COUNT_ODD_WR_EN Fld(2,14,AC_MSKB1)//[15:14]
    #define RG_HDMI2_FDET_VSYNC_LOW_COUNT_ODD_RD_EN Fld(2,16,AC_MSKB2)//[17:16]
    #define RG_HDMI2_FDET_VSYNC_HIGH_COUNT_ODD_WR_EN Fld(2,18,AC_MSKB2)//[19:18]
    #define RG_HDMI2_FDET_VSYNC_HIGH_COUNT_ODD_RD_EN Fld(2,20,AC_MSKB2)//[21:20]
    #define RG_HDMI2_FDET_VFRONT_COUNT_ODD_WR_EN Fld(2,22,AC_MSKB2)//[23:22]
    #define RG_HDMI2_FDET_VFRONT_COUNT_ODD_RD_EN Fld(2,24,AC_MSKB3)//[25:24]
    #define RG_HDMI2_FDET_VBACK_COUNT_ODD_WR_EN Fld(2,26,AC_MSKB3)//[27:26]
    #define RG_HDMI2_FDET_VBACK_COUNT_ODD_RD_EN Fld(2,28,AC_MSKB3)//[29:28]
    #define RG_HDMI2_FDET_FRAME_COUNT_WR_EN Fld(2,30,AC_MSKB3)//[31:30]
#define HDMI2_RESIGET_RW_ENABLE3 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x8EC)
    #define RG_HDMI2_FDET_FRAME_COUNT_RD_EN Fld(2,0,AC_MSKB0)//[1:0]
    #define RG_HDMI2_EMBD_SYNC_ENC_UPDATE_REQUEST_WR_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_FDET_STATUS_WR_EN Fld(1,3,AC_MSKB0)//[3:3]
#define HDMI2_FEATURES1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x900)
    #define RG_HDMI2_CSC_CORE Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_PIXCAP_IN_1 Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_C420_C422_1 Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_C422_C444 Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_MULTI_CSC Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_C444_C422_1 Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_C422_C420_1 Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_DITHER_RND_1 Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_RANGE_CLIP_1 Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_PIXCAP_OUT_1 Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_RETIMING Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_SHARED_420 Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_DATA_BITS_VALUE Fld(8,16,AC_FULLB2)//[23:16]
#define HDMI2_BUILD_TIME2 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x904)
    #define RG_HDMI2_BUILD_TIME2 Fld(32,0,AC_FULLDW)//[31:0]
#define HDMI2_C420_C422_CONFIG (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x908)
    #define RG_HDMI2_C420_C422_ENABLE Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_BYPASS1 Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_IN_CB_OR_CR Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_IN_CB_CR_POLARITY Fld(1,3,AC_MSKB0)//[3:3]
#define HDMI2_C422_C444_CONFIG (IO_VIRT_HDMI2 + IO_HDMI2_BASE+ 0x90C)
    #define RG_HDMI2_C422_C444_ENABLE Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_USE_CB_OR_CR Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_DISABLE_FILTER Fld(1,2,AC_MSKB0)//[2:2]
#define HDMI2_PIXCAP_IN_CONFIG (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x910)
    #define RG_HDMI2_SHOW_POINT Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_AUTO_TRIGGER Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_TRIGGER Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_BUSY Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_ERROR Fld(1,17,AC_MSKB2)//[17:17]
#define HDMI2_PIXCAP_IN_PIXEL (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x914)
    #define RG_HDMI2_ PIXCAP_IN_PIXEL Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_PIXCAP_IN_LINE Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_PIXCAP_IN_Y (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x918)
    #define RG_HDMI2_PIXCAP_IN_Y Fld(12,0,AC_MSKW10)//[11:0]
    #define RG_HDMI2_PIXCAP_IN_CB Fld(12,12,AC_MSKW21)//[23:12]
#define HDMI2_PIXCAP_IN_CR (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x91C)
    #define RG_HDMI2_PIXCAP_IN_CR Fld(12,0,AC_MSKW10)//[11:0]
#define HDMI2_MULTI_CSC_CONFIG (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x920)
    #define RG_HDMI2_ENABLE Fld(2,0,AC_MSKB0)//[1:0]
    #define RG_HDMI2_OUT_STD Fld(2,2,AC_MSKB0)//[3:2]
    #define RG_HDMI2_OUT_PC Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_OUT_RGB Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_IN_STD Fld(2,6,AC_MSKB0)//[7:6]
    #define RG_HDMI2_IN_PC Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_IN_RGB Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_DITHER_ENABLE Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_MULTI_CSC_MULTCOEFFR1C1 Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_MULTI_CSC_MULTCOEFFR1C2 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x924)
    #define RG_HDMI2_MULTI_CSC_MULTCOEFFR1C2 Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_MULTI_CSC_MULTCOEFFR1C3 Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_MULTI_CSC_MULTCOEFFR2C1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x928)
    #define RG_HDMI2_MULTI_CSC_MULTCOEFFR2C1 Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_MULTI_CSC_MULTCOEFFR2C2 Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_MULTI_CSC_MULTCOEFFR2C3 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x92C)
    #define RG_HDMI2_MULTI_CSC_MULTCOEFFR2C3 Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_MULTI_CSC_MULTCOEFFR3C1 Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_MULTI_CSC_MULTCOEFFR3C2 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x930)
    #define RG_HDMI2_MULTI_CSC_MULTCOEFFR3C2 Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_MULTI_CSC_MULTCOEFFR3C3 Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_MULTI_CSC_IN_GY_OFFSET (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x934)
    #define RG_HDMI2_MULTI_CSC_IN_GY_OFFSET Fld(13,0,AC_MSKW10)//[12:0]
    #define RG_HDMI2_MULTI_CSC_IN_BCB_OFFSET Fld(13,16,AC_MSKW32)//[28:16]
#define HDMI2_MULTI_CSC_IN_RCR_OFFSET (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x938)
    #define RG_HDMI2_MULTI_CSC_IN_RCR_OFFSET Fld(13,0,AC_MSKW10)//[12:0]
    #define RG_HDMI2_MULTI_CSC_OUT_GY_OFFSET Fld(13,16,AC_MSKW32)//[28:16]
#define HDMI2_MULTI_CSC_OUT_BCB_OFFSET (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x93C)
    #define RG_HDMI2_MULTI_CSC_OUT_BCB_OFFSET Fld(13,0,AC_MSKW10)//[12:0]
    #define RG_HDMI2_MULTI_CSC_OUT_RCR_OFFSET Fld(13,16,AC_MSKW32)//[28:16]
#define HDMI2_C422_C422_CONFIG (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x940)
    #define RG_HDMI2_ENABLE_422 Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_RESERVED_1 Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_DISABLE_FILTER Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_ENABLE_DOWNSAMPLE Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_BYPASS2 Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_OUT_CB_OR_CR Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_OUT_CB_CR_POLARITY Fld(1,19,AC_MSKB2)//[19:19]
#define HDMI2_DITHER_CONFIG (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x944)
    #define RG_HDMI2_MODE Fld(2,0,AC_MSKB0)//[1:0]
    #define RG_HDMI2_RND_ENABLE Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_SPATIAL_ENABLE Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_SPATIAL_DUAL Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_INPUT_IS_RGB2 Fld(1,16,AC_MSKB2)//[16:16]
#define HDMI2_RANGE_CLIP_Y_MIN (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x948)
    #define RG_HDMI2_RANGE_CLIP_Y_MIN Fld(12,0,AC_MSKW10)//[11:0]
    #define RG_HDMI2_RANGE_CLIP_Y_MAX Fld(12,16,AC_MSKW32)//[27:16]
#define HDMI2_RANGE_CLIP_C_MIN (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x94C)
    #define RG_HDMI2_RANGE_CLIP_C_MIN Fld(12,0,AC_MSKW10)//[11:0]
    #define RG_HDMI2_RANGE_CLIP_C_MAX Fld(12,16,AC_MSKW32)//[27:16]
#define HDMI2_PIXCAP_OUT_CONFIG (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x950)
    #define RG_HDMI2_SHOW_POINT Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_AUTO_TRIGGER Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_TRIGGER Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_BUSY Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_ERROR Fld(1,17,AC_MSKB2)//[17:17]
#define HDMI2_PIXCAP_OUT_PIXEL (IO_VIRT_HDMI2+ IO_HDMI2_BASE + 0x954)
    #define RG_HDMI2_PIXCAP_OUT_PIXEL Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_PIXCAP_OUT_LINE Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_PIXCAP_OUT_Y (IO_VIRT_HDMI2 + IO_HDMI2_BASE+ 0x958)
    #define RG_HDMI2_PIXCAP_OUT_Y Fld(12,0,AC_MSKW10)//[11:0]
    #define RG_HDMI2_PIXCAP_OUT_CB Fld(12,16,AC_MSKW32)//[27:16]
#define HDMI2_PIXCAP_OUT_CR (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x95C)
    #define RG_HDMI2_PIXCAP_OUT_CR Fld(12,0,AC_MSKW10)//[11:0]
#define HDMI2_VTG_HORIZONTAL_SYNC_END (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x960)
    #define RG_HDMI2_VTG_HORIZONTAL_SYNC_END Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_VTG_HORIZONTAL_ACTIVE_VIDEO_START Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_VTG_HALFLINE (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x964)
    #define RG_HDMI2_VTG_HALFLINE Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_VTG_HORIZONTAL_ACTIVE_VIDEO_END Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_VTG_END_OF_LINE (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x968)
    #define RG_HDMI2_VTG_END_OF_LINE Fld(16,0,AC_FULLW10)//[15:0]
#define HDMI2_VTG_VERTICAL_SYNC_END (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x970)
    #define RG_HDMI2_VTG_VERTICAL_SYNC_END Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_VTG_TRIGGER_START Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_VTG_VERTICAL_ACTIVE_VIDEO_START (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x974)
    #define RG_HDMI2_VTG_VERTICAL_ACTIVE_VIDEO_START Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_VTTG_VERTICAL_ACTIVE_VIDEO_END Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_VTG_VERTICAL_END_OF_FRAME (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x978)
    #define RG_HDMI2_VTG_VERTICAL_END_OF_FRAME Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_ENABLE_VTG Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_RESYNC Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_UNLOCK Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_BLANK Fld(1,19,AC_MSKB2)//[19:19]
    #define RG_HDMI2_VTG_THRESHOLD Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_VTG_CYCLE_DELAY (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0x97C)
    #define RG_HDMI2_VTG_CYCLE_DELAY Fld(16,0,AC_FULLW10)//[15:0]
    #define RG_HDMI2_VTG_UPDATE_REQUEST Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_READ_BANK2 Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_WRITE_BANK2 Fld(1,25,AC_MSKB3)//[25:25]
    #define RG_HDMI2_UPDATE_MODE2 Fld(1,26,AC_MSKB3)//[26:26]
#define HDMI2_RX_ACR_CTRL1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC00)
    #define RG_HDMI2_REG_ACR_INIT Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_FS_HW_SW_SEL Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_CTS_REUSED_AUTO_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_N_HW_SW_SEL Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_REG_CTS_HW_SW_SEL Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REG_UPLL_HW_SW_SEL Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_POST_HW_SW_SEL Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_REG_CTS_DROPPED_AUTO_EN Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_MCLK4DSD Fld(2,8,AC_MSKB1)//[9:8]
    #define RG_HDMI2_REG_MCLK4HBRA Fld(2,10,AC_MSKB1)//[11:10]
    #define RG_HDMI2_REG_MCLK4DAC Fld(2,12,AC_MSKB1)//[13:12]
    #define RG_HDMI2_REG_VCNT_MAX Fld(2,14,AC_MSKB1)//[15:14]
    #define RG_HDMI2_REG_FS_VAL_SW Fld(6,16,AC_MSKB2)//[21:16]
    #define RG_HDMI2_REG_FM_VAL_SW Fld(2,22,AC_MSKB2)//[23:22]
    #define RG_HDMI2_REG_N_VAL_SW7_0 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_N_SVAL2 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC04)
    #define RG_HDMI2_REG_N_VAL_SW15_8 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_N_VAL_SW19_16 Fld(4,8,AC_MSKB1)//[11:8]
    #define RG_HDMI2_N_VAL_HW7_0 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_N_VAL_HW15_8 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_N_HVAL3 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC08)
    #define RG_HDMI2_N_VAL_HW19_16 Fld(4,0,AC_MSKB0)//[3:0]
    #define RG_HDMI2_REG_CTS_VAL_SW7_0 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_CTS_VAL_SW15_8 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_CTS_VAL_SW19_16 Fld(4,24,AC_MSKB3)//[27:24]
#define HDMI2_RX_CTS_HVAL1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC0C)
    #define RG_HDMI2_CTS_VAL_HW7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_CTS_VAL_HW15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_CTS_VAL_HW19_16 Fld(4,16,AC_MSKB2)//[19:16]
    #define RG_HDMI2_REG_UPLL_VAL_SW Fld(7,24,AC_MSKB3)//[30:24]
#define HDMI2_RX_UPLL_HVAL (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC10)
    #define RG_HDMI2_UPLL_VAL_HW Fld(7,0,AC_MSKB0)//[6:0]
    #define RG_HDMI2_REG_POST_VAL_SW Fld(6,8,AC_MSKB1)//[13:8]
    #define RG_HDMI2_POST_VAL_HW Fld(6,16,AC_MSKB2)//[21:16]
    #define RG_HDMI2_REG_LK_WIN_SW0 Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_REG_LK_WIN_SW Fld(4,25,AC_MSKB3)//[28:25]
#define HDMI2_RX_LK_THRS_SVAL1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC14)
    #define RG_HDMI2_REG_LK_THRS_SVAL7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_LK_THRS_SVAL15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_LK_THRS_SVAL19_16 Fld(4,16,AC_MSKB2)//[19:16]
    #define RG_HDMI2_RHDMI_AUD_SAMPLE_F Fld(4,24,AC_MSKB3)//[27:24]
    #define HDMI2_AUD_FS_44K           (0x0)
		#define HDMI2_AUD_FS_88K           (0x8)
		#define HDMI2_AUD_FS_176K          (0xc)
		#define HDMI2_AUD_FS_48K           (0x2)
		#define HDMI2_AUD_FS_96K           (0xa)
		#define HDMI2_AUD_FS_192K          (0xe)
		#define HDMI2_AUD_FS_32K           (0x3)
		#define HDMI2_AUD_FS_768K          (0x9)
		#define HDMI2_AUD_FS_UNKNOWN       (0x1) 	
    #define RG_HDMI2_REG_FS_FILTER_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define RG_HDMI2_RHDMI_AUD_SAMPLE_F_EXTN Fld(2,30,AC_MSKB3)//[31:30]
#define HDMI2_RX_ACR_CTRL3 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC18)
    #define RG_HDMI2_REG_POST_DIV2_ENA Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_LOG_WIN_ENA Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_MCLK_LOOPBACK Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_CTS_THRESH Fld(4,3,AC_MSKB0)//[6:3]
    #define RG_HDMI2_CGMS_A Fld(2,8,AC_MSKB1)//[9:8]
    #define RG_HDMI2_CGMS_A_VALIDITY Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_BIT43 Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_AUD_SAMPLE_F_COEFF Fld(4,12,AC_MSKB1)//[15:12]
    #define RG_HDMI2_INFO_IN_PCM_SIGNAL Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_AUD_CH_STAT7 Fld(7,17,AC_MSKB2)//[23:17]
    #define RG_HDMI2_REG_DSD_LAYOUT Fld(4,24,AC_MSKB3)//[27:24]
    #define RG_HDMI2_REG_DAC_LAYOUT Fld(4,28,AC_MSKB3)//[31:28]
#define HDMI2_RX_I2S_CTRL1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC1C)
    #define RG_HDMI2_REG_1ST_BIT Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_DATA_DIR Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_JUSTIFY Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_WS Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_REG_MSB Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REG_SIZE Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_CLK_EDGE Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_REG_INVALID_EN Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_PCM Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_VUCP Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_MUTE_FLAT Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_MCLK_EN Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_REG_SD0_EN Fld(1,12,AC_MSKB1)//[12:12]
    #define RG_HDMI2_REG_SD1_EN Fld(1,13,AC_MSKB1)//[13:13]
    #define RG_HDMI2_REG_SD2_EN Fld(1,14,AC_MSKB1)//[14:14]
    #define RG_HDMI2_REG_SD3_EN Fld(1,15,AC_MSKB1)//[15:15]
    #define RG_HDMI2_REG_SD0_MAP Fld(2,16,AC_MSKB2)//[17:16]
    #define RG_HDMI2_REG_SD1_MAP Fld(2,18,AC_MSKB2)//[19:18]
    #define RG_HDMI2_REG_SD2_MAP Fld(2,20,AC_MSKB2)//[21:20]
    #define RG_HDMI2_REG_SD3_MAP Fld(2,22,AC_MSKB2)//[23:22]
    #define RG_HDMI2_REG_SPDIF_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_REG_SPDIF_MODE Fld(1,25,AC_MSKB3)//[25:25]
    #define RG_HDMI2_REG_I2S_MODE Fld(1,26,AC_MSKB3)//[26:26]
    #define RG_HDMI2_REG_PASS_AUD_ERR Fld(1,27,AC_MSKB3)//[27:27]
    #define RG_HDMI2_REG_PASS_SPDIF_ERR Fld(1,28,AC_MSKB3)//[28:28]
    #define RG_HDMI2_REG_HW_MUTE_EN Fld(1,29,AC_MSKB3)//[29:29]
    #define RG_HDMI2_REG_INV_COR_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define RG_HDMI2_REG_I2S_LENGTH_EN Fld(1,31,AC_MSKB3)//[31:31]
#define HDMI2_RX_CHST1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC20)
    #define RG_HDMI2_BIT0 Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_BIT1 Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_BIT2 Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_BIT3_4_5 Fld(3,3,AC_MSKB0)//[5:3]
    #define RG_HDMI2_BIT6_7 Fld(2,6,AC_MSKB0)//[7:6]
    #define RG_HDMI2_BIT15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_AUD_SOURCE1 Fld(4,16,AC_MSKB2)//[19:16]
    #define RG_HDMI2_AUD_CH_NUM1 Fld(4,20,AC_MSKB2)//[23:20]
    #define RG_HDMI2_REG_DIV_INCR Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_SW_OW (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC24)
    #define RG_HDMI2_REG_OW_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_CS_BIT2 Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_SWAP Fld(4,4,AC_MSKB0)//[7:4]
    #define RG_HDMI2_REG_CS_BIT15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_AUD_SAMPLE_F Fld(4,16,AC_MSKB2)//[19:16]
    #define RG_HDMI2_AUD_ACCURACY Fld(2,20,AC_MSKB2)//[21:20]
    #define RG_HDMI2_AUD_SAMPLE_F_EXTN Fld(2,22,AC_MSKB2)//[23:22]
    #define RG_HDMI2_AUD_LENGTH_MAX Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_AUD_LENGTH Fld(3,25,AC_MSKB3)//[27:25]
    #define RG_HDMI2_AUD_ORG_FS Fld(4,28,AC_MSKB3)//[31:28]
#define HDMI2_RX_AUDO_MUTE (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC28)
    #define RG_HDMI2_REG_CH0_MUTE Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_CH1_MUTE Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_CH2_MUTE Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_CH3_MUTE Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_REG_I2S_LENGTH Fld(4,4,AC_MSKB0)//[7:4]
    #define RG_HDMI2_REG_AUDIO_MUTE Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_MUTE_OUT_POL Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_AAC_AUDIO_MUTE Fld(1,12,AC_MSKB1)//[12:12]
    #define RG_HDMI2_REG_SPDIF4I2S_EN Fld(1,13,AC_MSKB1)//[13:13]
    #define RG_HDMI2_REG_SPDIF4TDM_EN Fld(1,14,AC_MSKB1)//[14:14]
    #define RG_HDMI2_REG_AUDIO_TOGGLE_MODE Fld(1,23,AC_MSKB2)//[23:23]
    #define RG_HDMI2_REG_TDM_MODE Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_REG_TDM_CH_NUM Fld(2,25,AC_MSKB3)//[26:25]
    #define RG_HDMI2_REG_TDM_FS_MSB Fld(2,27,AC_MSKB3)//[28:27]
#define HDMI2_RX_TDM_CTRL2 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC2C)
    #define RG_HDMI2_REG_TDM_FS_LEN Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_VID_PCLK_CNT_MULT Fld(4,8,AC_MSKB1)//[11:8]
    #define RG_HDMI2_REG_VID_PCLK_CNT_BASE Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_VID_XCLKPCLK_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_REG_VRES_XCLK_DIFF Fld(4,28,AC_MSKB3)//[31:28]
#define HDMI2_VID_XPBASE0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC30)
    #define RG_HDMI2_REG_XP_BASE7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_XP_BASE15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_XP_BASE23_16 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_XP_THRSH Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_VID_XPCNT1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC34)
    #define RG_HDMI2_AAC_XCLK_IN_PCLK7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_AAC_XCLK_IN_PCLK15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_AAC_XCLK_IN_PCLK23_16 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_POLE Fld(3,24,AC_MSKB3)//[26:24]
    #define RG_HDMI2_REG_ZERO Fld(3,27,AC_MSKB3)//[29:27]
    #define RG_HDMI2_REG_MODOVRVAL Fld(2,30,AC_MSKB3)//[31:30]
#define HDMI2_RX_APLL_CLIP (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC38)
    #define RG_HDMI2_REG_CLIP Fld(4,0,AC_MSKB0)//[3:0]
    #define RG_HDMI2_REG_SDREF Fld(3,4,AC_MSKB0)//[6:4]
    #define RG_HDMI2_REG_WRAP Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_SDINOVRVAL Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_USOVRVAL Fld(3,16,AC_MSKB2)//[18:16]
    #define RG_HDMI2_REG_CPCTRL Fld(3,19,AC_MSKB2)//[21:19]
    #define RG_HDMI2_REG_LFCTRL Fld(2,22,AC_MSKB2)//[23:22]
    #define RG_HDMI2_REG_NCLKSEL Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_REG_FS32OVRVAL Fld(1,25,AC_MSKB3)//[25:25]
    #define RG_HDMI2_REG_PREDIVSELOVRVAL Fld(1,26,AC_MSKB3)//[26:26]
    #define RG_HDMI2_REG_USOVR Fld(1,27,AC_MSKB3)//[27:27]
    #define RG_HDMI2_REG_FS32OVR Fld(1,28,AC_MSKB3)//[28:28]
    #define RG_HDMI2_REG_SDINOVR Fld(1,29,AC_MSKB3)//[29:29]
    #define RG_HDMI2_REG_MODOVR Fld(1,30,AC_MSKB3)//[30:30]
    #define RG_HDMI2_REG_PREDIVSELOVR Fld(1,31,AC_MSKB3)//[31:31]
#define HDMI2_RX_APLL_STAT (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC3C)
    #define RG_HDMI2_ACR_DPLL_LOCK Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_NAPLL_CTL Fld(4,8,AC_MSKB1)//[11:8]
    #define RG_HDMI2_NAPLL_FBDIV7_0 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_NAPLL_FBDIV11_8 Fld(4,24,AC_MSKB3)//[27:24]
#define HDMI2_RX_NACR_STS3 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC40)
    #define RG_HDMI2_NAPLL_FRAC11_4 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_NAPLL_FRAC19_12 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_NAPLL_FRAC23_20 Fld(4,16,AC_MSKB2)//[19:16]
    #define RG_HDMI2_FREQDIFF3_0 Fld(4,20,AC_MSKB2)//[23:20]
    #define RG_HDMI2_FREQDIFF11_4 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_NACR_STS7 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC44)
    #define RG_HDMI2_FREQDIFF19_12 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_FREQDIFF27_20 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_FREQDIFF29_28 Fld(2,16,AC_MSKB2)//[17:16]
    #define RG_HDMI2_REG_APLL_REFDIV Fld(6,24,AC_MSKB3)//[29:24]
#define HDMI2_RX_APLL_POSTDIV (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC48)
    #define RG_HDMI2_REG_APLL_POSTDIV1 Fld(3,0,AC_MSKB0)//[2:0]
    #define RG_HDMI2_REG_APLL_POSTDIV2 Fld(3,4,AC_MSKB0)//[6:4]
    #define RG_HDMI2_REG_APLL_PD Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_APLL_DACPD Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_APLL_DSMPD Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_APLL_FOUTPOSTDIVPD Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_REG_APLL_FOUT4PHASEPD Fld(1,12,AC_MSKB1)//[12:12]
    #define RG_HDMI2_REG_APLL_FOUTVCOPD Fld(1,13,AC_MSKB1)//[13:13]
    #define RG_HDMI2_REG_APLL_BYPASS Fld(1,14,AC_MSKB1)//[14:14]
    #define RG_HDMI2_NAPLL_LOCK Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_REG_AVG_WINDOW Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_DACR_MCLK_CTRL (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC4C)
    #define RG_HDMI2_REG_DACR_MCLK_OUT_EN Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_DACR_REF_CLK_SEL Fld(2,8,AC_MSKB1)//[9:8]
    #define RG_HDMI2_REG_AAC_CTRL40 Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_REG_AAC_CTRL41 Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_REG_AAC_CTRL42 Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_REG_AAC_CTRL43 Fld(1,19,AC_MSKB2)//[19:19]
    #define RG_HDMI2_REG_AAC_CTRL44 Fld(1,20,AC_MSKB2)//[20:20]
    #define RG_HDMI2_REG_AAC_CTRL45 Fld(1,21,AC_MSKB2)//[21:21]
    #define RG_HDMI2_REG_AAC_CTRL46 Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_REG_AAC_CTRL47 Fld(1,23,AC_MSKB2)//[23:23]
    #define RG_HDMI2_REG_AAC_CTRL30 Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_REG_AAC_CTRL31 Fld(1,25,AC_MSKB3)//[25:25]
    #define RG_HDMI2_REG_AAC_CTRL32 Fld(1,26,AC_MSKB3)//[26:26]
    #define RG_HDMI2_REG_AAC_CTRL33 Fld(1,27,AC_MSKB3)//[27:27]
    #define RG_HDMI2_REG_AAC_CTRL34 Fld(1,28,AC_MSKB3)//[28:28]
    #define RG_HDMI2_REG_AAC_CTRL35 Fld(1,29,AC_MSKB3)//[29:29]
    #define RG_HDMI2_REG_AAC_CTRL36 Fld(1,30,AC_MSKB3)//[30:30]
    #define RG_HDMI2_REG_AAC_CTRL37 Fld(1,31,AC_MSKB3)//[31:31]
#define HDMI2_AEC2_CTRL (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC50)
    #define RG_HDMI2_REG_AAC_CTRL20 Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_AAC_CTRL21 Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_AAC_CTRL22 Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_AAC_CTRL23 Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_REG_AAC_CTRL24 Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REG_AAC_CTRL25 Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_AAC_CTRL26 Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_REG_AAC_CTRL27 Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_AAC_CTRL0 Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_AAC_CTRL1 Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_AAC_CTRL2 Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_AAC_CTRL3 Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_REG_AAC_CTRL4 Fld(1,12,AC_MSKB1)//[12:12]
    #define RG_HDMI2_REG_AAC_CTRL5 Fld(1,13,AC_MSKB1)//[13:13]
    #define RG_HDMI2_REG_AAC_CTRL6 Fld(1,14,AC_MSKB1)//[14:14]
    #define RG_HDMI2_REG_AAC_CTRL7 Fld(1,15,AC_MSKB1)//[15:15]
    #define RG_HDMI2_REG_AAC_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_REG_AAC_ALL Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_RSVD_17 Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_AAC_MUTE_STAT Fld(1,19,AC_MSKB2)//[19:19]
    #define RG_HDMI2_AUD_DIV_MIN Fld(1,20,AC_MSKB2)//[20:20]
    #define RG_HDMI2_REG_AAC_OUT_OFF_EN Fld(1,21,AC_MSKB2)//[21:21]
    #define RG_HDMI2_REG_AAC_EXP_SEL Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_REG_CTRL_ACR_EN Fld(1,23,AC_MSKB2)//[23:23]
    #define RG_HDMI2_REG_EXP_EN7_0 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_AEC_EN2 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC54)
    #define RG_HDMI2_REG_EXP_EN15_8 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_EXP_EN23_16 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_PSTOP_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_REG_PCLK_MAX Fld(7,17,AC_MSKB2)//[23:17]
    #define RG_HDMI2_REG_AAC_CSC_ERR Fld(5,24,AC_MSKB3)//[28:24]
#define HDMI2_AAC_VSC_ERR (IO_VIRT_HDMI + IO_HDMI_BASE + 0xC58)
    #define RG_HDMI2_REG_AAC_VSC_ERR Fld(4,0,AC_MSKB0)//[3:0]
    #define RG_HDMI2_REG_AAC_ASC_ERR Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_AAC_EXP_CAPT7_0 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_AAC_EXP_CAPT15_8 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_AAC_EXP_CAPT_H (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xC5C)
    #define RG_HDMI2_REG_AAC_EXP_CAPT23_16 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_ACR_GEN_CTRL0 Fld(8,8,AC_FULLB1)//[15:8]
#define HDMI2_RX_SHD_BKSV_1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xE04)
    #define RG_HDMI2_HDCP_BKSV1 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_HDCP_BKSV2 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_HDCP_BKSV3 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_HDCP_BKSV4 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_SHD_BKSV_5 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xE08)
    #define RG_HDMI2_HDCP_BKSV5 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_RI0 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_RI1 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_HDCP_AKSV1 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_SHD_AKSV2 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xE0C)
    #define RG_HDMI2_HDCP_AKSV2 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_HDCP_AKSV3 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_HDCP_AKSV4 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_HDCP_AKSV5 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_SHD_AN1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xE10)
    #define RG_HDMI2_HDCP_AN1 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_HDCP_AN2 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_HDCP_AN3 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_HDCP_AN4 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_SHD_AN5 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xE14)
    #define RG_HDMI2_HDCP_AN5 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_HDCP_AN6 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_HDCP_AN7 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_HDCP_AN8 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_BCAPS_SET (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xE18)
    #define RG_HDMI2_REG_FAST Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REG_FIFO_RDY Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_REPEATER Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_REG_HDMI_CAPABLE Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_DEV_CNT Fld(1,14,AC_MSKB1)//[14:14]
    #define RG_HDMI2_REG_DEV_EXCEED Fld(1,15,AC_MSKB1)//[15:15]
    #define RG_HDMI2_REG_DEPTH Fld(3,16,AC_MSKB2)//[18:16]
    #define RG_HDMI2_RPT_CASC_EXCEED Fld(1,19,AC_MSKB2)//[19:19]
    #define RG_HDMI2_HDMI_MODE Fld(1,20,AC_MSKB2)//[20:20]
    #define RG_HDMI2_REG_BSTATUS Fld(3,21,AC_MSKB2)//[23:21]
    #define RG_HDMI2_REG_WR_DEBUG Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_HDCP_STAT (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xE1C)
    #define RG_HDMI2_HDCP_SPHST Fld(4,0,AC_MSKB0)//[3:0]
    #define RG_HDMI2_HDCP_AUTHENTICATED Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_HDCP_DECRYPTING Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_START_ADDR Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_REP_START_ADDR Fld(2,16,AC_MSKB2)//[17:16]
    #define RG_HDMI2_REG_SHA_LENGTH7_0 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_SHA_LENGTH2 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xE20)
    #define RG_HDMI2_REG_SHA_LENGTH9_8 Fld(2,0,AC_MSKB0)//[1:0]
    #define RG_HDMI2_REG_SHA_GO_STAT Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_SHA_MODE Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_KSV_FIFO_OUT Fld(8,16,AC_FULLB2)//[23:16]
#define HDMI2_HDCP_M0_0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xE24)
    #define RG_HDMI2_HDCP_M07_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_HDCP_M015_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_HDCP_M023_16 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_HDCP_M031_24 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_HDCP_M0_4 (IO_VIRT_HDMI + IO_HDMI_BASE + 0xE28)
    #define RG_HDMI2_HDCP_M039_32 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_HDCP_M047_40 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_HDCP_M055_48 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_HDCP_M063_56 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_DS_BSTATUS1 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xE2C)
    #define RG_HDMI2_REG_DS_DEV_CNT Fld(7,0,AC_MSKB0)//[6:0]
    #define RG_HDMI2_REG_DS_DEV_EXCEED Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_DS_DEPTH Fld(3,8,AC_MSKB1)//[10:8]
    #define RG_HDMI2_REG_DS_CASC_EXCEED Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_REG_DS_HDMI_MODE Fld(1,12,AC_MSKB1)//[12:12]
    #define RG_HDMI2_REG_DS_BSTATUS Fld(3,13,AC_MSKB1)//[15:13]
#define HDMI2_RX_DS_M0_0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xE30)
    #define RG_HDMI2_REG_MO7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_MO15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_MO23_16 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_MO31_24 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_DS_M0_4 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xE34)
    #define RG_HDMI2_REG_MO39_32 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_MO47_40 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_MO55_48 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_MO63_56 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_VH0_0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xE38)
    #define RG_HDMI2_REG_V_H07_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_V_H015_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_V_H023_16 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_V_H031_24 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_VH1_0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xE3C)
    #define RG_HDMI2_REG_V_H17_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_V_H115_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_V_H123_16 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_V_H131_24 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_VH2_0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xE40)
    #define RG_HDMI2_REG_V_H27_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_V_H215_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_V_H223_16 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_V_H231_24 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_VH3_0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xE44)
    #define RG_HDMI2_REG_V_H37_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_V_H315_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_V_H323_16 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_V_H331_24 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_VH4_0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xE48)
    #define RG_HDMI2_REG_V_H47_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_V_H415_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_V_H423_16 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_V_H431_24 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_EPST (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xE4C)
    #define RG_HDMI2_REG_CMD_DONE_CLR Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_BIST_ERR_CLR Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_SDA_ERR_CLR Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_ACK_ERR_CLR Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_REG_LD_KSV_CLR Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REG_BIST1_ERR_CLR Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_BIST2_ERR_CLR Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_OTP_UNLOCKED Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_EPCM Fld(5,8,AC_MSKB1)//[12:8]
    #define RG_HDMI2_REG_LD_KSV Fld(1,13,AC_MSKB1)//[13:13]
    #define RG_HDMI2_REG_KEY_MASK Fld(16,16,AC_FULLW32)//[31:16]
#define HDMI2_HDCP2_RX_ID_0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xF00)
    #define RG_HDMI2_RO_RCV_ID7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_RO_RCV_ID15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_RO_RCV_ID23_16 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_RO_RCV_ID31_24 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_HDCP2_RX_ID_4 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xF04)
    #define RG_HDMI2_RO_RCV_ID Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_RI_REAUTH_REQ_CLR_OPTION Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_RI_RPT_READY_CLR_OPTION Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_RI_MSG_SZ_CLR_OPTION Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_RI_ECC_CHK_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_RI_ECC_CHK_MODE Fld(2,17,AC_MSKB2)//[18:17]
    #define RG_HDMI2_RI_ACCM_ERR_MANU_CLR Fld(1,19,AC_MSKB2)//[19:19]
#define HDMI2_RX_HDCP2X_ECC_CNT2CHK_0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xF08)
    #define RG_HDMI2_RI_CNT2CHK_ECC7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_RI_CNT2CHK_ECC8 Fld(1,8,AC_MSKB1)//[8:8]
#define HDMI2_RX_HDCP2X_ECC_ACCM_ERR_THR_0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xF0C)
    #define RG_HDMI2_RI_ACCM_ERR_THR7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_RI_ACCM_ERR_THR15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_RI_ACCM_ERR_THR20_16 Fld(5,16,AC_MSKB2)//[20:16]
#define HDMI2_RX_HDCP2X_ECC_FRM_ERR_THR_0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xF10)
    #define RG_HDMI2_RI_FRAME_ECC_ERR_THR7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_RI_FRAME_ECC_ERR_THR15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_RI_CONS_ECC_ERR_THR Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_RI_NO_ECC_THR Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RX_HDCP2X_ECC_GVN_FRM (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xF14)
    #define RG_HDMI2_RI_GIVEN_FRAME Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_RI_GIVEN_FRAME_ERR_THR7_0 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_RI_GIVEN_FRAME_ERR_THR15_8 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_RI_GIVEN_FRAME_ERR_THR20_16 Fld(5,24,AC_MSKB3)//[28:24]
#define HDMI2_RX_HDCP2X_INTR (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xF18)
    #define RG_HDMI2_REG_ECC_OUT_OF_SYNC_INTR Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_HDCP2X_INTR_MASK0 Fld(1,8,AC_MSKB1)//[8:8]
#define HDMI2_RX_HDCP2X_ROSC_BIST (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xF1C)
    #define RG_HDMI2_RI_HDCP2X_RINGOSC_BIST_START Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_RO_HDCP2X_RINGOSC_BIST_DONE Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_RO_HDCP2X_RINGOSC_BIST_FAIL Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_RI_HDCP2X_BIST_MODE Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_MBIST_FAIL_ROM32768X8 Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_MBIST_FINISH_ROM32768X8 Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_MBIST_FAIL_SPRAM16384X8 Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_MBIST_FINISH_SPRAM16384X8 Fld(1,19,AC_MSKB2)//[19:19]
    #define RG_HDMI2_MBIST_FAIL_SPRAM1024X32 Fld(1,20,AC_MSKB2)//[20:20]
    #define RG_HDMI2_MBIST_FINISH_SPRAM1024X32 Fld(1,21,AC_MSKB2)//[21:21]
    #define RG_HDMI2_MBIST_FAIL_SPRAM256X8 Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_MBIST_FINISH_SPRAM256X8 Fld(1,23,AC_MSKB2)//[23:23]
    #define RG_HDMI2_RI_HDCP2X_NVM_BIST_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_RI_HDCP2X_NVM_BIST_MODE Fld(4,28,AC_MSKB3)//[31:28]
#define HDMI2_RX_HDCP2X_NVM_BIST_STS (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xF20)
    #define RG_HDMI2_RO_HDCP2X_NVM_BIST_DONE Fld(4,0,AC_MSKB0)//[3:0]
    #define RG_HDMI2_RO_HDCP2X_NVM_BIST_ERR Fld(4,4,AC_MSKB0)//[7:4]
#define HDMI2_RXHDCP2X_CMD_CTRL_0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xF80)
    #define RG_HDMI2_REG_HDCP2X_HDCPTX Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_HDCP2X_RI_REPEATER Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_HDCP2X_HDMIMODE Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_HDCP2X_PRECOMPUTE Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_REG_HDCP2X_STM_ID Fld(3,4,AC_MSKB0)//[6:4]
    #define RG_HDMI2_REG_HDCP2X_REAUTH_SW Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_HDCP2X_CTL3MSK Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_HDCP2X_REAUTH_MSK Fld(4,12,AC_MSKB1)//[15:12]
    #define RG_HDMI2_REG_HDCP2X_CUPD_DONE Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_REG_HDCP2X_CUPD_START Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_REG_HDCP2X_CPVER Fld(4,20,AC_MSKB2)//[23:20]
#define HDMI2_RXHDCP2X_CMD_INTR0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xF84)
    #define RG_HDMI2_REG_INTR0_SRC0 Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_INTR0_SRC1 Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_INTR0_SRC2 Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_INTR0_SRC3 Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_REG_INTR0_SRC4 Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REG_INTR0_SRC5 Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_INTR0_SRC6 Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_REG_INTR0_SRC7 Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_INTR0_MASK0 Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_INTR0_MASK1 Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_INTR0_MASK2 Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_INTR0_MASK3 Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_REG_INTR0_MASK4 Fld(1,12,AC_MSKB1)//[12:12]
    #define RG_HDMI2_REG_INTR0_MASK5 Fld(1,13,AC_MSKB1)//[13:13]
    #define RG_HDMI2_REG_INTR0_MASK6 Fld(1,14,AC_MSKB1)//[14:14]
    #define RG_HDMI2_REG_INTR0_MASK7 Fld(1,15,AC_MSKB1)//[15:15]
    #define RG_HDMI2_REG_INTR1_SRC0 Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_REG_INTR1_SRC1 Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_REG_INTR1_SRC2 Fld(1,18,AC_MSKB2)//[18:18]
    #define RG_HDMI2_REG_INTR1_SRC3 Fld(1,19,AC_MSKB2)//[19:19]
    #define RG_HDMI2_REG_INTR1_SRC4 Fld(1,20,AC_MSKB2)//[20:20]
    #define RG_HDMI2_REG_INTR1_SRC5 Fld(1,21,AC_MSKB2)//[21:21]
    #define RG_HDMI2_REG_INTR1_SRC6 Fld(1,22,AC_MSKB2)//[22:22]
    #define RG_HDMI2_REG_INTR1_SRC7 Fld(1,23,AC_MSKB2)//[23:23]
    #define RG_HDMI2_REG_2_INTR1_MASK0 Fld(1,24,AC_MSKB3)//[24:24]
    #define RG_HDMI2_REG_2_INTR1_MASK1 Fld(1,25,AC_MSKB3)//[25:25]
    #define RG_HDMI2_REG_2_INTR1_MASK2 Fld(1,26,AC_MSKB3)//[26:26]
    #define RG_HDMI2_REG_2_INTR1_MASK3 Fld(1,27,AC_MSKB3)//[27:27]
    #define RG_HDMI2_REG_2_INTR1_MASK4 Fld(1,28,AC_MSKB3)//[28:28]
    #define RG_HDMI2_REG_2_INTR1_MASK5 Fld(1,29,AC_MSKB3)//[29:29]
    #define RG_HDMI2_REG_2_INTR1_MASK6 Fld(1,30,AC_MSKB3)//[30:30]
    #define RG_HDMI2_REG_2_INTR1_MASK7 Fld(1,31,AC_MSKB3)//[31:31]
#define HDMI2_RXHDCP2X_CMD_AUTH_STAT (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xF88)
    #define RG_HDMI2_REG_HDCP2X_AUTH_STAT Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_HDCP2X_STATE Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_HDCP2X_PRG_SEL Fld(1,16,AC_MSKB2)//[16:16]
    #define RG_HDMI2_REG_HDCP2X_MODE_SEL Fld(1,17,AC_MSKB2)//[17:17]
    #define RG_HDMI2_REG_HDCP2X_RO_REPEATER Fld(1,18,AC_MSKB2)//[18:18]
#define HDMI2_RXHDCP2X_RSVD (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xF8C)
    #define RG_HDMI2_REG_HDCP2X_CUPD_SIZE7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_HDCP2X_CUPD_SIZE15_8 Fld(8,8,AC_FULLB1)//[15:8]
#define HDMI2_RXHDCP2X_CMD_RPT_STM_CTR_0 (IO_VIRT_HDMI + IO_HDMI_BASE + 0xF90)
    #define RG_HDMI2_REG_HDCP2X_RPT_STM_CTR7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_HDCP2X_RPT_STM_CTR15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_HDCP2X_RPT_STM_CTR23_16 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_HDCP2X_RPT_STM_CTR31_24 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RXHDCP2X_CMD_TP0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xF94)
    #define RG_HDMI2_REG_HDCP2X_TP0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_HDCP2X_TP1 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_HDCP2X_TP2 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_HDCP2X_TP3 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RXHDCP2X_CMD_TP4 (IO_VIRT_HDMI2+ IO_HDMI2_BASE + 0xF98)
    #define RG_HDMI2_REG_HDCP2X_TP4 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_HDCP2X_TP5 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_HDCP2X_TP6 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_HDCP2X_TP7 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RXHDCP2X_CMD_TP8 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xF9C)
    #define RG_HDMI2_REG_HDCP2X_TP8 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_HDCP2X_TP9 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_HDCP2X_TP10 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_HDCP2X_TP11 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RXHDCP2X_CMD_TP12 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xFA0)
    #define RG_HDMI2_REG_HDCP2X_TP12 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_HDCP2X_TP13 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_HDCP2X_TP14 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_HDCP2X_TP15 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RXHDCP2X_CMD_GP_IN0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xFA4)
    #define RG_HDMI2_REG_HDCP2X_GP_IN0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_HDCP2X_GP_IN1 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_HDCP2X_GP_IN2 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_HDCP2X_GP_IN3 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RXHDCP2X_CMD_GP_OUT0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xFA8)
    #define RG_HDMI2_REG_HDCP2X_GP_OUT0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_HDCP2X_GP_OUT1 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_HDCP2X_GP_OUT2 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_HDCP2X_GP_OUT3 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RXHDCP2X_CMD_RX_ID_CORE_0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xFAC)
    #define RG_HDMI2_REG_HDCP2X_RCV_ID_CORE7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_HDCP2X_RCV_ID_CORE15_8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_HDCP2X_RCV_ID_CORE23_16 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_HDCP2X_RCV_ID_CORE31_24 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RXHDCP2X_CMD_RX_ID_CORE_4 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xFB0)
    #define RG_HDMI2_REG_HDCP2X_RCV_ID_CORE39_32 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_HDCP2X_RPT_HDCP1DEV_DSTRM Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_HDCP2X_RPT_HDCP20RPT_DSTRM Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_HDCP2X_RPT_MX_CASC_EXC Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_HDCP2X_RPT_MX_DEVS_EXC Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_REG_HDCP2X_RPT_SMNG_K Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_HDCP2X_RPT_DEPTH Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RXHDCP2X_RPT_DEVCNT (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xFB4)
    #define RG_HDMI2_REG_HDCP2X_RPT_DEVCNT Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_HDCP2X_RPT_SEQ_NUM_V_7TO0 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_HDCP2X_RPT_SEQ_NUM_V_15TO8 Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REG_HDCP2X_RPT_SEQ_NUM_V_23TO16 Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RXHDCP2X_RX_SEQ_NUM_M_0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xFB8)
    #define RG_HDMI2_REG_HDCP2X_RPT_SEQ_NUM_M_7TO0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_HDCP2X_RPT_SEQ_NUM_M_15TO8 Fld(8,8,AC_FULLB1)//[15:8]
    #define RG_HDMI2_REG_HDCP2X_RPT_SEQ_NUM_M_23TO16 Fld(8,16,AC_FULLB2)//[23:16]
#define HDMI2_RXHDCP2X_IPT_CTR_7TO0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xFBC)
    #define RG_HDMI2_REG_HDCP2X_IPT_CTR7_0 Fld(8,0,AC_FULLB0)//[7:0]
    #define RG_HDMI2_REG_HDCP2X_IPT_CTR15_8 Fld(8,8,AC_FULLB1)//[15:8]
#define HDMI2_RXHDCP2X_CMD_INTR2 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xFC0)
    #define RG_HDMI2_REG_INTR2_SRC0 Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REG_INTR2_SRC1 Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REG_INTR2_SRC2 Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REG_INTR2_SRC3 Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_REG_INTR2_SRC4 Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REG_INTR2_SRC5 Fld(1,5,AC_MSKB0)//[5:5]
    #define RG_HDMI2_REG_INTR2_SRC6 Fld(1,6,AC_MSKB0)//[6:6]
    #define RG_HDMI2_REG_INTR2_SRC7 Fld(1,7,AC_MSKB0)//[7:7]
    #define RG_HDMI2_REG_INTR2_MASK0 Fld(1,8,AC_MSKB1)//[8:8]
    #define RG_HDMI2_REG_INTR2_MASK1 Fld(1,9,AC_MSKB1)//[9:9]
    #define RG_HDMI2_REG_INTR2_MASK2 Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REG_INTR2_MASK3 Fld(1,11,AC_MSKB1)//[11:11]
    #define RG_HDMI2_REG_INTR2_MASK4 Fld(1,12,AC_MSKB1)//[12:12]
    #define RG_HDMI2_REG_INTR2_MASK5 Fld(1,13,AC_MSKB1)//[13:13]
    #define RG_HDMI2_REG_INTR2_MASK6 Fld(1,14,AC_MSKB1)//[14:14]
    #define RG_HDMI2_REG_INTR2_MASK7 Fld(1,15,AC_MSKB1)//[15:15]
#define HDMI2_RXHDCP2X_RX_CTRL_0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xFC4)
    #define RG_HDMI2_REGRX_HDCP2X_RPT_RCVID_WR Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REGRX_HDCP2X_RPT_RCVID_WR_START Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REGRX_HDCP2X_RPT_RCVID_XFER_START Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REGRX_HDCP2X_RPT_SMNG_RD Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_REGRX_HDCP2X_RPT_SMNG_RD_START Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REGRX_HDCP2X_RPT_RCVID_XFER_DONE Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REGRX_HDCP2X_RPT_SMNG_OUT Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REGRX_HDCP2X_RPT_RCVID_IN Fld(8,24,AC_FULLB3)//[31:24]
#define HDMI2_RXHDCP2X_TX_CTRL_0 (IO_VIRT_HDMI2 + IO_HDMI2_BASE + 0xFC8)
    #define RG_HDMI2_REGTX_HDCP2X_RPT_RCVID_RD Fld(1,0,AC_MSKB0)//[0:0]
    #define RG_HDMI2_REGTX_HDCP2X_RPT_RCVID_RD_START Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_HDMI2_REGTX_HDCP2X_RPT_SMNG_XFER_START Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_HDMI2_REGTX_HDCP2X_RPT_SMNG_WR Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_HDMI2_REGTX_HDCP2X_RPT_SMNG_WR_START Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_HDMI2_REGTX_HDCP2X_RPT_SMNG_XFER_DONE Fld(1,10,AC_MSKB1)//[10:10]
    #define RG_HDMI2_REGTX_HDCP2X_RPT_SMNG_IN Fld(8,16,AC_FULLB2)//[23:16]
    #define RG_HDMI2_REGTX_HDCP2X_RPT_RCVID_OUT Fld(8,24,AC_FULLB3)//[31:24]
#endif

