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

/**
 * @file LVDS HW register map
 *
 * @author dragon_chen@mtk.com.tw
 */

#ifndef _HW_LVDS_H_
#define _HW_LVDS_H_

// *********************************************************************
// LVDS Register
// *********************************************************************

#ifdef CC_UP8032_ATV
#define IO_VIRT 0
#endif
#define IO_ANA_LVDS (IO_VIRT + 0x62100)
#define REG_LVDS_TX_CFG0 (IO_ANA_LVDS+0x00)
    #define RG_LVDSB_ABIST_DC_SEL_D11 Fld(2, 28, AC_MSKB3) //29:28
    #define RG_LVDSB_TEST_SW_EN_D11 Fld(1, 26, AC_MSKB3) //26
    #define RG_LVDSB_ABIST_P_SW_EN_D11 Fld(1, 25, AC_MSKB3) //25
    #define RG_LVDSB_ABIST_N_SW_EN_D11 Fld(1, 24, AC_MSKB3) //24
    #define RG_LVDSB_ABIST_ISO_D11 Fld(1, 22, AC_MSKB2) //22
    #define RG_LVDSB_BIST_EN_D11 Fld(1, 21, AC_MSKB2) //21
    #define RG_LVDSB_BIST_TOGGLE_D11 Fld(1, 20, AC_MSKB2) //20
    #define RG_LVDSB_ENEDGE_SEL_D11 Fld(1, 19, AC_MSKB2) //19
    #define RG_LVDSB_RESYNC_CLK_IQ_SEL_D11 Fld(1, 18, AC_MSKB2) //18
    #define RG_LVDSB_CKDIGFF_INV_D11 Fld(1, 17, AC_MSKB2) //17
    #define RG_LVDSB_GPO_TURBO_D11 Fld(1, 16, AC_MSKB2) //16
    #define RG_LVDSB_CML_PWR_NMOS_SEL_D11 Fld(2, 12, AC_MSKB1) //13:12
    #define RG_LVDSB_BIAS_EN_D11 Fld(1, 8, AC_MSKB1) //8
    #define RG_LVDSB_TVCM_D11 Fld(4, 4, AC_MSKB0) //7:4
    #define RG_LVDSB_EN_D11 Fld(1, 0, AC_MSKB0) //0
#define REG_LVDS_TX_CFG1 (IO_ANA_LVDS+0x04)
    #define RG_LVDSB_DRV_EN_EVEN_D11 Fld(7, 24, AC_MSKB3) //30:24
    #define RG_LVDSB_DRV_EN_ODD_D11 Fld(7, 16, AC_MSKB2) //22:16
    #define RG_LVDSB_PREEM_EN_EVEN_D11 Fld(7, 8, AC_MSKB1) //14:8
    #define RG_LVDSB_PREEM_EN_ODD_D11 Fld(7, 0, AC_MSKB0) //6:0
#define REG_LVDS_TX_CFG2 (IO_ANA_LVDS+0x08)
    #define RG_LVDSB_CLK_IQ_SEL_EVEN_D11 Fld(7, 24, AC_MSKB3) //30:24
    #define RG_LVDSB_CLK_IQ_SEL_ODD_D11 Fld(7, 16, AC_MSKB2) //22:16
    #define RG_LVDSB_ENABIST_E_EVEN_D11 Fld(7, 8, AC_MSKB1) //14:8
    #define RG_LVDSB_ENABIST_E_ODD_D11 Fld(7, 0, AC_MSKB0) //6:0
#define REG_LVDS_TX_CFG3 (IO_ANA_LVDS+0x0c)
    #define RG_LVDSB_TVO_EVEN_D11 Fld(28, 0, AC_MSKDW) //27:0
#define REG_LVDS_TX_CFG4 (IO_ANA_LVDS+0x10)
    #define RG_LVDSB_TVO_ODD_D11 Fld(28, 0, AC_MSKDW) //27:0
#define REG_LVDS_TX_CFG5 (IO_ANA_LVDS+0x14)
    #define RG_LVDSB_TVO_PRE_EVEN_D11 Fld(14, 16, AC_MSKW32) //29:16
    #define RG_LVDSB_TVO_PRE_ODD_D11 Fld(14, 0, AC_MSKW10) //13:0
#define REG_LVDS_TX_CFG6 (IO_ANA_LVDS+0x18)
    #define RG_LVDSB_R_TERM_EVEN_D11 Fld(14, 16, AC_MSKW32) //29:16
    #define RG_LVDSB_R_TERM_ODD_D11 Fld(14, 0, AC_MSKW10) //13:0
#define REG_LVDS_TX_CFG7 (IO_ANA_LVDS+0x1c)
    #define RG_LVDSB_GPO_EN_EVEN_D11 Fld(14, 16, AC_MSKW32) //29:16
    #define RG_LVDSB_GPO_EN_ODD_D11 Fld(14, 0, AC_MSKW10) //13:0
#define REG_LVDS_TX_CFG8 (IO_ANA_LVDS+0x20)
    #define RG_LVDSB_GPI_EN_EVEN_D11 Fld(14, 16, AC_MSKW32) //29:16
    #define RG_LVDSB_GPI_EN_ODD_D11 Fld(14, 0, AC_MSKW10) //13:0
#define REG_LVDS_TX_CFG9 (IO_ANA_LVDS+0x24)
    #define RG_LVDSB_PAD_SW_EN_EVEN_D11 Fld(7, 24, AC_MSKB3) //30:24
    #define RG_LVDSB_PAD_SW_EN_ODD_D11 Fld(7, 16, AC_MSKB2) //22:16
    #define RG_LVDSB_MASTER_SW_EN_EVEN_D11 Fld(7, 8, AC_MSKB1) //14:8
    #define RG_LVDSB_MASTER_SW_EN_ODD_D11 Fld(7, 0, AC_MSKB0) //6:0
#define REG_LVDS_TX_CFG10 (IO_ANA_LVDS+0x28)
    #define RG_LVDSB_SLAVE_SW_EN_EVEN_D11 Fld(7, 24, AC_MSKB3) //30:24
    #define RG_LVDSB_SLAVE_SW_EN_ODD_D11 Fld(7, 16, AC_MSKB2) //22:16
    #define RG_DIV_EN Fld(1, 15, AC_MSKB1) //15:15
    #define RG_LVDSB_REV_O Fld(8, 8, AC_FULLB1) //15:8
    #define RG_LVDSB_REV_E Fld(8, 0, AC_FULLB0) //7:0
#define RGS_LVDS_TX_CFG0 (IO_ANA_LVDS+0x2c)
    #define RGS_LVDSB_ABIST_ODD_51EDG_D11 Fld(1, 31, AC_MSKB3) //31
    #define RGS_LVDSB_ABIST_EVEN_51EDG_D11 Fld(1, 30, AC_MSKB3) //30
#endif
