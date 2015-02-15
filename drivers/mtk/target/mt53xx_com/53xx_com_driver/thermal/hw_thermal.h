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

/*************************************************************************
 *
 * Filename:
 * ---------
 *   $Workfile: mtk_ts_cpu.h $
 *
 * Project:
 * --------
 *
 *
 * Description:
 * ------------
 *
 *
 * Author:
 * -------
 *
 *
 * Last changed:
 * -------------
 * $Author: p4admin $
 *
 * $Modtime: 04/03/23 7:08p $
 *
 * $Revision: #1 $
*************************************************************************/

#ifndef _HW_THERMALCTL_H_
#define _HW_THERMALCTL_H_

#define EFUSE_TS1_ADDR 0xF00086C8
#define EFUSE_TS2_ADDR 0xF00086CC

// need check
#define APMIXEDSYS_BASE 0x00000000  // need check
#define AUXADC_BASE 0x00000000  // need check
#define PERICFG_BASE 0x00000000  // need check
#define APMIXED_BASE 0x00000000  // need check


#define THERMAL_BASE 0xF0066000

/*******************************************************************************
 * AUXADC Register Definition
 ******************************************************************************/
#define AUXADC_CON0_V       (AUXADC_BASE + 0x000)	//yes, 0x11003000
#define AUXADC_CON1_V       (AUXADC_BASE + 0x004)
#define AUXADC_CON1_SET_V   (AUXADC_BASE + 0x008)
#define AUXADC_CON1_CLR_V   (AUXADC_BASE + 0x00C)
#define AUXADC_CON2_V       (AUXADC_BASE + 0x010)
#define AUXADC_DAT0_V       (AUXADC_BASE + 0x014)
#define AUXADC_DAT1_V       (AUXADC_BASE + 0x018)
#define AUXADC_DAT2_V       (AUXADC_BASE + 0x01C)
#define AUXADC_DAT3_V       (AUXADC_BASE + 0x020)
#define AUXADC_DAT4_V       (AUXADC_BASE + 0x024)
#define AUXADC_DAT5_V       (AUXADC_BASE + 0x028)
#define AUXADC_DAT6_V       (AUXADC_BASE + 0x02C)
#define AUXADC_DAT7_V       (AUXADC_BASE + 0x030)
#define AUXADC_DAT8_V       (AUXADC_BASE + 0x034)
#define AUXADC_DAT9_V       (AUXADC_BASE + 0x038)
#define AUXADC_DAT10_V       (AUXADC_BASE + 0x03C)
#define AUXADC_DAT11_V       (AUXADC_BASE + 0x040)
#define AUXADC_MISC_V       (AUXADC_BASE + 0x094)

#define AUXADC_CON0_P       (AUXADC_BASE + 0x000 - 0xE0000000)
#define AUXADC_CON1_P       (AUXADC_BASE + 0x004 - 0xE0000000)
#define AUXADC_CON1_SET_P   (AUXADC_BASE + 0x008 - 0xE0000000)
#define AUXADC_CON1_CLR_P   (AUXADC_BASE + 0x00C - 0xE0000000)
#define AUXADC_CON2_P       (AUXADC_BASE + 0x010 - 0xE0000000)
#define AUXADC_DAT0_P       (AUXADC_BASE + 0x014 - 0xE0000000)
#define AUXADC_DAT1_P       (AUXADC_BASE + 0x018 - 0xE0000000)
#define AUXADC_DAT2_P       (AUXADC_BASE + 0x01C - 0xE0000000)
#define AUXADC_DAT3_P       (AUXADC_BASE + 0x020 - 0xE0000000)
#define AUXADC_DAT4_P       (AUXADC_BASE + 0x024 - 0xE0000000)
#define AUXADC_DAT5_P       (AUXADC_BASE + 0x028 - 0xE0000000)
#define AUXADC_DAT6_P       (AUXADC_BASE + 0x02C - 0xE0000000)
#define AUXADC_DAT7_P       (AUXADC_BASE + 0x030 - 0xE0000000)
#define AUXADC_DAT8_P       (AUXADC_BASE + 0x034 - 0xE0000000)
#define AUXADC_DAT9_P       (AUXADC_BASE + 0x038 - 0xE0000000)
#define AUXADC_DAT10_P       (AUXADC_BASE + 0x03C - 0xE0000000)
#define AUXADC_DAT11_P       (AUXADC_BASE + 0x040 - 0xE0000000)

#define AUXADC_MISC_P       (AUXADC_BASE + 0x094 - 0xE0000000)

/*******************************************************************************
 * Peripheral Configuration Register Definition
 ******************************************************************************/
#define PERI_GLOBALCON_RST0 (PERICFG_BASE + 0x000) //yes, 0x10003000

/*******************************************************************************
 * APMixedSys Configuration Register Definition
 ******************************************************************************/
//#define TS_CON0_V             (APMIXED_BASE + 0x600) //yes 0x10209000
//#define TS_CON1_V             (APMIXED_BASE + 0x604)
//#define TS_CON2_V             (APMIXED_BASE + 0x608)

#define TS_CON0_P             (APMIXED_BASE + 0x600 - 0xE0000000)
#define TS_CON1_P             (APMIXED_BASE + 0x604 - 0xE0000000)
#define TS_CON2_P             (APMIXED_BASE + 0x608 - 0xE0000000)

/*******************************************************************************
 * Thermal Controller Register Definition
 ******************************************************************************/
// Thermal Controller 1
#define TEMPMONCTL0         (THERMAL_BASE + 0x000) //yes 0x1100c000
#define TEMPMONCTL1         (THERMAL_BASE + 0x004)
#define TEMPMONCTL2         (THERMAL_BASE + 0x008)
#define TEMPMONINT          (THERMAL_BASE + 0x00C)
#define TEMPMONINTSTS       (THERMAL_BASE + 0x010)
#define TEMPMONIDET0        (THERMAL_BASE + 0x014)
#define TEMPMONIDET1        (THERMAL_BASE + 0x018)
#define TEMPMONIDET2        (THERMAL_BASE + 0x01C)
#define TEMPH2NTHRE         (THERMAL_BASE + 0x024)
#define TEMPHTHRE           (THERMAL_BASE + 0x028)
#define TEMPCTHRE           (THERMAL_BASE + 0x02C)
#define TEMPOFFSETH         (THERMAL_BASE + 0x030)
#define TEMPOFFSETL         (THERMAL_BASE + 0x034)
#define TEMPMSRCTL0         (THERMAL_BASE + 0x038)
#define TEMPMSRCTL1         (THERMAL_BASE + 0x03C)
	#define TEMPMSRCTL1_MSR3IMM Fld(1,9,AC_MSKB1)//[9:9]
	#define TEMPMSRCTL1_PAUSE3 Fld(1,8,AC_MSKB1)//[8:8]
	#define TEMPMSRCTL1_MSRSTS1 Fld(1,7,AC_MSKB0)//[7:7]
	#define TEMPMSRCTL1_MSR0IMM Fld(1,6,AC_MSKB0)//[6:6]
	#define TEMPMSRCTL1_MSR1IMM Fld(1,5,AC_MSKB0)//[5:5]
	#define TEMPMSRCTL1_MSR2IMM Fld(1,4,AC_MSKB0)//[4:4]
	#define TEMPMSRCTL1_PAUSE0 Fld(1,3,AC_MSKB0)//[3:3]
	#define TEMPMSRCTL1_PAUSE1 Fld(1,2,AC_MSKB0)//[2:2]
	#define TEMPMSRCTL1_PAUSE2 Fld(1,1,AC_MSKB0)//[1:1]
	#define TEMPMSRCTL1_MSRSTS0 Fld(1,0,AC_MSKB0)//[0:0]
#define TEMPAHBPOLL         (THERMAL_BASE + 0x040)
#define TEMPAHBTO           (THERMAL_BASE + 0x044)
#define TEMPADCPNP0         (THERMAL_BASE + 0x048)
#define TEMPADCPNP1         (THERMAL_BASE + 0x04C)
#define TEMPADCPNP2         (THERMAL_BASE + 0x050)
#define TEMPADCMUX          (THERMAL_BASE + 0x054)
#define TEMPADCEXT          (THERMAL_BASE + 0x058)
#define TEMPADCEXT1         (THERMAL_BASE + 0x05C)
#define TEMPADCEN           (THERMAL_BASE + 0x060)
#define TEMPPNPMUXADDR      (THERMAL_BASE + 0x064)
#define TEMPADCMUXADDR      (THERMAL_BASE + 0x068)
#define TEMPADCEXTADDR      (THERMAL_BASE + 0x06C)
#define TEMPADCEXT1ADDR     (THERMAL_BASE + 0x070)
#define TEMPADCENADDR       (THERMAL_BASE + 0x074)
#define TEMPADCVALIDADDR    (THERMAL_BASE + 0x078)
#define TEMPADCVOLTADDR     (THERMAL_BASE + 0x07C)
#define TEMPRDCTRL          (THERMAL_BASE + 0x080)
#define TEMPADCVALIDMASK    (THERMAL_BASE + 0x084)
#define TEMPADCVOLTAGESHIFT (THERMAL_BASE + 0x088)
#define TEMPADCWRITECTRL    (THERMAL_BASE + 0x08C)
#define TEMPMSR0            (THERMAL_BASE + 0x090)
#define TEMPMSR1            (THERMAL_BASE + 0x094)
#define TEMPMSR2            (THERMAL_BASE + 0x098)
#define TEMPIMMD0           (THERMAL_BASE + 0x0A0)
#define TEMPIMMD1           (THERMAL_BASE + 0x0A4)
#define TEMPIMMD2           (THERMAL_BASE + 0x0A8)

#define TEMPMONIDET3 		(THERMAL_BASE + 0x0B0)
#define TEMPADCPNP3 		(THERMAL_BASE + 0x0B4)
#define TEMPMSR3 			(THERMAL_BASE + 0x0B8)
#define TEMPIMMD3 			(THERMAL_BASE + 0x0BC)

#define TEMPPROTCTL         (THERMAL_BASE + 0x0C0)
#define TEMPPROTTA          (THERMAL_BASE + 0x0C4)
#define TEMPPROTTB          (THERMAL_BASE + 0x0C8)
#define TEMPPROTTC          (THERMAL_BASE + 0x0CC)

#define TEMPSPARE0          (THERMAL_BASE + 0x0F0)
		#define TEMPSPARE0_INITIAL Fld(1,0,AC_MSKB0)//[0:0]
#define TEMPSPARE1          (THERMAL_BASE + 0x0F4)
#define TEMPSPARE2          (THERMAL_BASE + 0x0F8)
#define TEMPSPARE3          (THERMAL_BASE + 0x0FC)


/*******************************************************************************
 * Thermal Controller Register Mask Definition
 ******************************************************************************/
#define THERMAL_ENABLE_SEN0     0x1
#define THERMAL_ENABLE_SEN1     0x2
#define THERMAL_ENABLE_SEN2     0x4
#define THERMAL_MONCTL0_MASK    0x00000007

#define THERMAL_PUNT_MASK       0x00000FFF
#define THERMAL_FSINTVL_MASK    0x03FF0000
#define THERMAL_SPINTVL_MASK    0x000003FF
#define THERMAL_MON_INT_MASK    0x0007FFFF

#define THERMAL_MON_CINTSTS0    0x000001
#define THERMAL_MON_HINTSTS0    0x000002
#define THERMAL_MON_LOINTSTS0   0x000004
#define THERMAL_MON_HOINTSTS0   0x000008
#define THERMAL_MON_NHINTSTS0   0x000010
#define THERMAL_MON_CINTSTS1    0x000020
#define THERMAL_MON_HINTSTS1    0x000040
#define THERMAL_MON_LOINTSTS1   0x000080
#define THERMAL_MON_HOINTSTS1   0x000100
#define THERMAL_MON_NHINTSTS1   0x000200
#define THERMAL_MON_CINTSTS2    0x000400
#define THERMAL_MON_HINTSTS2    0x000800
#define THERMAL_MON_LOINTSTS2   0x001000
#define THERMAL_MON_HOINTSTS2   0x002000
#define THERMAL_MON_NHINTSTS2   0x004000
#define THERMAL_MON_TOINTSTS    0x008000
#define THERMAL_MON_IMMDINTSTS0 0x010000
#define THERMAL_MON_IMMDINTSTS1 0x020000
#define THERMAL_MON_IMMDINTSTS2 0x040000
#define THERMAL_MON_FILTINTSTS0 0x080000
#define THERMAL_MON_FILTINTSTS1 0x100000
#define THERMAL_MON_FILTINTSTS2 0x200000


#define THERMAL_tri_SPM_State0	0x20000000
#define THERMAL_tri_SPM_State1	0x40000000
#define THERMAL_tri_SPM_State2	0x80000000


#define THERMAL_MSRCTL0_MASK    0x00000007
#define THERMAL_MSRCTL1_MASK    0x00000038
#define THERMAL_MSRCTL2_MASK    0x000001C0

#endif
