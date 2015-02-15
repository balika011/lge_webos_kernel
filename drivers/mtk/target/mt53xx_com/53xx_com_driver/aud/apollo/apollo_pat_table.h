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
 *   @file dsp_pat_table.h
 *		
 *   @author
 *
 *   Revision: 5368_0003
 */

#include "x_common.h" 

#define PAT_REV "5368_0003"


#ifdef CC_AUD_APOLLO_SUPPORT
#ifdef CC_AUD_APOLLO_VERIFY
/*************************************************************************************/
/*                            Apollo Verification Code table                         */
/*************************************************************************************/

#define DMEM_SRAM1_TST_SIZE 1360
extern const UINT8 DMEM_SRAM1_TST_TABLE[DMEM_SRAM1_TST_SIZE];
#define DMEM_DRAM1_TST_SIZE 2400
extern const UINT8 DMEM_DRAM1_TST_TABLE[DMEM_DRAM1_TST_SIZE];
#define DMEM_DRAM2_TST_SIZE 3076
extern const UINT8 DMEM_DRAM2_TST_TABLE[DMEM_DRAM2_TST_SIZE];
#define DMEM_DRAM3_TST_SIZE 2416
extern const UINT8 DMEM_DRAM3_TST_TABLE[DMEM_DRAM3_TST_SIZE];
//#define DMEM_MIX1_TST_SIZE 20204
#define DMEM_MIX1_TST_SIZE 0x4eec
extern const UINT8 DMEM_MIX1_TST_TABLE[DMEM_MIX1_TST_SIZE];
#define DMEM_MIX2_TST_SIZE 28980
extern const UINT8 DMEM_MIX2_TST_TABLE[DMEM_MIX2_TST_SIZE];
#define DMEM_MIX3_TST_SIZE 11604
extern const UINT8 DMEM_MIX3_TST_TABLE[DMEM_MIX3_TST_SIZE];

#define ADDR_DBG_JTAG_CTRL_SIZE 600
extern const UINT8 ADDR_DBG_JTAG_CTRL_TABLE[ADDR_DBG_JTAG_CTRL_SIZE];

#define GETBS_FUNCTION_SIZE 8328
extern const UINT8 GETBS_FUNCTION_TABLE[GETBS_FUNCTION_SIZE];

#define GETBS_CRC_SIZE 5388
extern const UINT8 GETBS_CRC_TABLE[GETBS_CRC_SIZE];

#define GETBS_CNT_LDZ_SIZE 2760
extern const UINT8 GETBS_CNT_LDZ_TABLE[GETBS_CNT_LDZ_SIZE];

#define PARSER_DMA_SIZE 732
extern const UINT8 PARSER_DMA_TABLE[PARSER_DMA_SIZE];

#define ADDR_CONFLICT_SIZE 0x550
extern const UINT8 ADDR_CONFLICT_TABLE[ADDR_CONFLICT_SIZE];
#define ADDR_CONFLICT_DATA_SIZE 0x238
extern const UINT8 ADDR_CONFLICT_DATA[ADDR_CONFLICT_DATA_SIZE];
	
#define READ_AFTER_WRITE_SIZE 0x470
extern const UINT8 READ_AFTER_WRITE_TABLE[READ_AFTER_WRITE_SIZE];
#define READ_AFTER_WRITE_DATA_SIZE 0x384
extern const UINT8 READ_AFTER_WRITE_DATA[READ_AFTER_WRITE_DATA_SIZE];

#define SRAM_DIRTY_BIT_SIZE 0x344
extern const UINT8 SRAM_DIRTY_BIT_TABLE[SRAM_DIRTY_BIT_SIZE];
#define SRAM_DIRTY_BIT_DATA_SIZE 0x134
extern const UINT8 SRAM_DIRTY_BIT_DATA[SRAM_DIRTY_BIT_DATA_SIZE];
	
#define JTAG_LOAD_SIZE 148
extern const UINT8 JTAG_LOAD_TABLE[JTAG_LOAD_SIZE];
#define JTAG_CONTROL_SIZE 148
extern const UINT8 JTAG_CONTROL_TABLE[JTAG_CONTROL_SIZE];
#define PM_SEQUENTIAL_ACCESS_SIZE  (0xAF4)
extern const UINT8 PM_SEQUENTIAL_ACCESS_TABLE[PM_SEQUENTIAL_ACCESS_SIZE];

#define PM_BRANCH_SIZE (0x8F0C)
extern const UINT8 PM_BRANCH_TABLE[PM_BRANCH_SIZE];
#define PM_BRANCH_DATA_SIZE (4)
extern const UINT8 PM_BRANCH_DATA[PM_BRANCH_DATA_SIZE];


#define CACHE_2WAY_SIZE (0xB06C)
extern const UINT8 CACHE_2WAY_TABLE[CACHE_2WAY_SIZE];
#define CACHE_4WAY_SIZE (0x78d8)
extern const UINT8 CACHE_4WAY_TABLE[CACHE_4WAY_SIZE];


#define CACHE_STATISTIC_SIZE (0x8C0C)
extern const UINT8 CACHE_STATISTIC_TABLE[CACHE_STATISTIC_SIZE];
#define CACHE_STATISTIC_DATA_SIZE (4)
extern const UINT8 CACHE_STATISTIC_DATA[CACHE_STATISTIC_DATA_SIZE];


#define EXT_PM_SADR_SIZE_0_KERNEL (0x174)
extern const UINT8 EXT_PM_SADR_TABLE_0_KERNEL[EXT_PM_SADR_SIZE_0_KERNEL];

#define EXT_PM_SADR_SIZE_1_KERNEL (0x174)
extern const UINT8 EXT_PM_SADR_TABLE_1_KERNEL[EXT_PM_SADR_SIZE_1_KERNEL];

#define EXT_PM_SADR_SIZE_2_KERNEL (0x174)
extern const UINT8 EXT_PM_SADR_TABLE_2_KERNEL[EXT_PM_SADR_SIZE_2_KERNEL];

#define EXT_PM_SADR_SIZE_USER (0x3C)
extern const UINT8 EXT_PM_SADR_TABLE_USER[EXT_PM_SADR_SIZE_USER];

#define APOLLO_INST_SIZE 260
extern const UINT8 APOLLO_INST_TABLE[APOLLO_INST_SIZE];

#define APOLLO_INST_A00_SIZE 260
extern const UINT8 APOLLO_INST_A00_TABLE[APOLLO_INST_A00_SIZE];

#define APOLLO_INST_A03_SIZE 0x1218
extern const UINT8 APOLLO_INST_A03_TABLE[APOLLO_INST_A03_SIZE];

#define APOLLO_INST_A05_SIZE 0x1218
extern const UINT8 APOLLO_INST_A05_TABLE[APOLLO_INST_A05_SIZE];

#define APOLLO_INST_A09_SIZE 0x3c8
extern const UINT8 APOLLO_INST_A09_TABLE[APOLLO_INST_A09_SIZE];

#define APOLLO_INST_C10_SIZE 0x9218
extern const UINT8 APOLLO_INST_C10_TABLE[APOLLO_INST_C10_SIZE];

#define APOLLO_INST_C11_SIZE 0x9218
extern const UINT8 APOLLO_INST_C11_TABLE[APOLLO_INST_C11_SIZE];

#define APOLLO_INST_C12_SIZE 0x9214
extern const UINT8 APOLLO_INST_C12_TABLE[APOLLO_INST_C12_SIZE];

#define APOLLO_INT_UNALIGN_SIZE (0x670)
extern const UINT8 APOLLO_INT_UNALIGN_TABLE[APOLLO_INT_UNALIGN_SIZE];

#define APOLLO_INT_DIV0_SIZE (0xf0)
extern const UINT8 APOLLO_INT_DIV0_TABLE[APOLLO_INT_DIV0_SIZE];

#define APOLLO_INT_STACK_OV_CHK_SIZE (0x200)
extern const UINT8 APOLLO_INT_STACK_OV_CHK_TABLE[APOLLO_INT_STACK_OV_CHK_SIZE];

#define APOLLO_INT_INTERNAL_TIMER_SIZE (0x398)
extern const UINT8 APOLLO_INT_INTERNAL_TIMER_TABLE[APOLLO_INT_INTERNAL_TIMER_SIZE];

#define APOLLO_INT_27MHz_TIMER0_SIZE (0x530)
extern const UINT8 APOLLO_INT_27MHz_TIMER0_TABLE[APOLLO_INT_27MHz_TIMER0_SIZE];

#define APOLLO_INT_27MHz_TIMER1_SIZE (0x538)
extern const UINT8 APOLLO_INT_27MHz_TIMER1_TABLE[APOLLO_INT_27MHz_TIMER1_SIZE];

#define APOLLO_INT_27MHz_TIMER2_SIZE (0x53c)
extern const UINT8 APOLLO_INT_27MHz_TIMER2_TABLE[APOLLO_INT_27MHz_TIMER2_SIZE];

#define APOLLO_INT_Nested_UnalignDM_SIZE (0x57c)
extern const UINT8 APOLLO_INT_Nested_UnalignDM_TABLE[APOLLO_INT_Nested_UnalignDM_SIZE];

#define APOLLO_INT_Nested_DivZ_SIZE (0x518)
extern const UINT8 APOLLO_INT_Nested_DivZ_TABLE[APOLLO_INT_Nested_DivZ_SIZE];

#define APOLLO_INT_Aout2_SIZE (0xadc)
extern const UINT8 APOLLO_INT_Aout2_TABLE[APOLLO_INT_Aout2_SIZE];

#define DATA_APOLLO_INT_Aout2_SIZE (0x21c)
extern const UINT8 DATA_APOLLO_INT_Aout2_TABLE[DATA_APOLLO_INT_Aout2_SIZE];


#define APOLLO_INT_Asrc_SIZE (0x1118)
extern const UINT8 APOLLO_INT_Asrc_TABLE[APOLLO_INT_Asrc_SIZE];

#define APOLLO_AENV_MMR_SIZE (0x8a4)
extern const UINT8 APOLLO_AENV_MMR_TABLE[APOLLO_AENV_MMR_SIZE];

#define APOLLO_AENV_AOUT_SIZE (0x82C)
extern const UINT8 APOLLO_AENV_AOUT_TABLE[APOLLO_AENV_AOUT_SIZE];

#define APOLLO_AENV_AOUT_ASRC_SIZE (0xff4)
extern const UINT8 APOLLO_AENV_AOUT_ASRC_TABLE[APOLLO_AENV_AOUT_ASRC_SIZE];

#define APOLLO_AENV_IEC_RAW_OUT_SIZE (0xb3C)
extern const UINT8 APOLLO_AENV_IEC_RAW_OUT_TABLE[APOLLO_AENV_IEC_RAW_OUT_SIZE];

#define APOLLO_AENV_AOUT_TMR0_SIZE (0xd60)
extern const UINT8 APOLLO_AENV_AOUT_TMR0_TABLE[APOLLO_AENV_AOUT_TMR0_SIZE];

#define APOLLO_CMP_SAVE_RESTORE 0x888
extern const UINT8 APOLLO_CMP_SAVE_RESTORE_TABLE[APOLLO_CMP_SAVE_RESTORE] ;

#define APOLLO_CMP_SAVE_RESTORE_DATA_SIZE 0x3F4
extern const UINT8 APOLLO_CMP_SAVE_RESTORE_DATA[APOLLO_CMP_SAVE_RESTORE_DATA_SIZE] ;

#define APOLLO_PUTBITS_TEST_SIZE 0x4d1c
extern const UINT8 APOLLO_PUTBITS_TEST_TABLE[APOLLO_PUTBITS_TEST_SIZE] ;

#define APOLLO_PCBranchHis_TEST_SIZE 0x3ec
extern const UINT8 APOLLO_PCBranchHis_TEST_TABLE[APOLLO_PCBranchHis_TEST_SIZE] ;

#define APOLLO_INT_RiscApollo_TEST_SIZE 0x1314
extern const UINT8 APOLLO_INT_RiscApollo_TEST_TABLE[APOLLO_INT_RiscApollo_TEST_SIZE] ;

#define APOLLO_INT_DSPApollo_TEST_SIZE 0x8b0
extern const UINT8 APOLLO_INT_DSPApollo_TEST_TABLE[APOLLO_INT_DSPApollo_TEST_SIZE] ;

#define APOLLO_HOLD_PCSequtially_Kernel_1_TEST_SIZE (0x178)
extern const UINT8 APOLLO_HOLD_PCSequtially_Kernel_1_TEST_TABLE[APOLLO_HOLD_PCSequtially_Kernel_1_TEST_SIZE];

#define APOLLO_HOLD_PCSequtially_Kernel_2_TEST_SIZE (0x4c)
extern const UINT8 APOLLO_HOLD_PCSequtially_Kernel_2_TEST_TABLE[APOLLO_HOLD_PCSequtially_Kernel_2_TEST_SIZE];

#define APOLLO_HOLD_WriteMem_TEST_SIZE (0x102c)
extern const UINT8 APOLLO_HOLD_WriteMem_TEST_TABLE[APOLLO_HOLD_WriteMem_TEST_SIZE];

#define APOLLO_LX_POST_SIZE 0x97A0
extern const UINT8 APOLLO_LX_POST_TABLE[APOLLO_LX_POST_SIZE];

/*************************************************************************************/
/*                            Apollo Verification Data table                         */
/*************************************************************************************/
#define DATA_APOLLO_INST_SIZE 32
extern const UINT8 DATA_APOLLO_INST_TABLE[DATA_APOLLO_INST_SIZE];

#define DATA_APOLLO_INST_A05_SIZE 0x208
extern const UINT8 DATA_APOLLO_INST_A05_TABLE[DATA_APOLLO_INST_A05_SIZE];

#define DATA_APOLLO_AENV_AOUT_SIZE (0x214)
extern const UINT8 DATA_APOLLO_AENV_AOUT_TABLE[DATA_APOLLO_AENV_AOUT_SIZE];

#define DATA_APOLLO_INT_ASRC_SIZE (0x3f8)
extern const UINT8 DATA_APOLLO_INT_ASRC_TABLE[DATA_APOLLO_INT_ASRC_SIZE];

#define DATA_APOLLO_AENV_AOUT_ASRC_SIZE (0x3e8)
extern const UINT8 DATA_APOLLO_AENV_AOUT_ASRC_TABLE[DATA_APOLLO_AENV_AOUT_ASRC_SIZE];

#define DATA_APOLLO_AENV_IEC_RAW_OUT_SIZE (0x22c)
extern const UINT8 DATA_APOLLO_AENV_IEC_RAW_OUT_TABLE[DATA_APOLLO_AENV_IEC_RAW_OUT_SIZE];

#define DATA_APOLLO_AENV_AOUT_TMR0_SIZE (0x234)
extern const UINT8 DATA_APOLLO_AENV_AOUT_TMR0_TABLE[DATA_APOLLO_AENV_AOUT_TMR0_SIZE];

#define DATA_APOLLO_AC3_51SRC_TABLE_SIZE (0x88800)
extern const UINT8 DATA_APOLLO_AC3_51SRC_TABLE[DATA_APOLLO_AC3_51SRC_TABLE_SIZE];

#define AACENC_PM_SIZE (0xBF18)
#define AACENC_DM_SIZE (0xDFD4)
extern const UINT8 AACENC_PM_TABLE[AACENC_PM_SIZE];
extern const UINT8 AACENC_DM_TABLE[AACENC_DM_SIZE];

#define AACENC_DM_GOLDEN_F1_SIZE (0x155)
#define AACENC_DM_GOLDEN_F2_SIZE (0x1BD)
#define AACENC_DM_GOLDEN_F3_SIZE (0x100)
#define AACENC_DM_GOLDEN_F4_SIZE (0x143)
#define AACENC_DM_GOLDEN_F5_SIZE (0x184)

extern const UINT8 AACENC_DM_GOLDEN_F1[AACENC_DM_GOLDEN_F1_SIZE];
extern const UINT8 AACENC_DM_GOLDEN_F2[AACENC_DM_GOLDEN_F2_SIZE];
extern const UINT8 AACENC_DM_GOLDEN_F3[AACENC_DM_GOLDEN_F3_SIZE];
extern const UINT8 AACENC_DM_GOLDEN_F4[AACENC_DM_GOLDEN_F4_SIZE];
extern const UINT8 AACENC_DM_GOLDEN_F5[AACENC_DM_GOLDEN_F5_SIZE];

#define DATA_APOLLO_LX_POST_SIZE (0xb7dd8-0x24000)
extern const UINT8 DATA_APOLLO_LX_POST_TABLE[DATA_APOLLO_LX_POST_SIZE];
#endif
#endif
