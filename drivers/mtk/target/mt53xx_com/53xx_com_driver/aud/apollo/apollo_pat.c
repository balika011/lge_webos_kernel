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
 * $RCSfile: dsp_pat.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dsp_pat.c
 *  Brief of file dsp_pat.c.
 *  Source file for ADSP related pattern test.
 */


//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#include "x_lint.h"
#include "x_ckgen.h"
LINT_EXT_HEADER_BEGIN
#include "x_hal_5381.h"
#include "x_hal_926.h"

#include "aud_hw.h"
#include "fbm_drvif.h"  // for DSP memory allocation
#include "drv_common.h"
#include "drvcust_if.h"

#include "x_os.h"
#include "x_assert.h"

#include "hw_ckgen.h"
#include "x_drv_cli.h"

//#include <string.h>


#ifdef CC_AUD_APOLLO_SUPPORT
#ifdef CC_AUD_APOLLO_VERIFY
#include "apollo_pat_table.h"

extern UINT32 u4GetAFIFOStart(UINT8 uDecIndex);
extern UINT32 u4GetAFIFOEnd(UINT8 uDecIndex);

enum
{
    MIXED_MEM_PAT = 1,
    ADDR_CONFLICT_PAT = 2,
    READ_AFTER_WRITE_PAT = 3,
    SRAM_DIRTY_BIT_PAT = 4,
    JTAG_LOAD_PAT = 5,
    JTAG_CONTROL_PAT = 6,
    PM_SEQUENTIAL_ACCESS_PAT = 7,
    PM_BRANCH_PAT = 8,
    CACHE_2WAY_PAT = 9,
    CACHE_4WAY_PAT = 10,
    CACHE_STATISTIC_PAT = 11,
    EXT_PM_SADR_PAT = 12,
    INST_TST_PAT = 13,
    GETBS_TST_PAT = 14,
    GETBS_CRC_PAT = 15,
    GETBS_COUNT_LDZ_PAT = 16,
    PUTBS_TST_PAT = 17,
    PC_BranchHis = 18,
    INT_RiscApollo = 19,
    INT_DspApollo = 20,
    INT_TST_PAT = 21,
    AENV_PAT = 22,
    MP3_TST_PAT = 23,
    AAC_TST_PAT = 24,
    AC3_TST_PAT = 25,
    MTK_POST_PAT = 26,
    SXX_POST_PAT = 27,
    DMA_TST_PAT = 28,
    RISC_INTF_PAT = 29,
    APOLLO_HOLD_TST_PAT = 30,
    COMPACT_SAVE_RESTORE = 31,
    RISC_DEBUGGER = 32,
    DMEM_OVERNIGHT = 33,
    PMEM_OVERNIGHT = 34,
    ADSP_INTF_OVERNIGHT = 35,
    PMEM_CACHE_DUMP = 36,
    GETBS_DMA = 37,
    TIMER_27MHZ = 38,
    LXX_POST_PAT = 39,
    MAX_TST_PAT
};

typedef struct
{
    const CHAR* szApolloPatName;
    UINT32 u4PollAddr;
} DSP_PAT_T;

DSP_PAT_T _arApolloPatType[MAX_TST_PAT] =
{
    {"NA",0},
    {"mixed_memory_tst",0x10000},
    {"addr_conflict_tst",0x10000},
    {"read_after_write_tst",0x10000},
    {"sram_dirty_bit_tst",0x10000},
    {"jtag_load_code_tst",0x10000},
    {"jtag_control_tst",0x10000},
    {"pm_sequential_access_tst",0x10000},
    {"pm_branch_tst",0x10000},
    {"2-way_cache_tst",0x10000},
    {"4-way_cache_tst",0x10000},
    {"cache_statistics_tst",0x10000},
    {"external_pm_sadr_tst",0x10000},
    {"inst_tst",0x10000},
    {"getbs_tst",0x10000},
    {"getbs_crc_tst",0x10000},
    {"getbs_count_lead_zero_tst",0x10000},
    {"putbs_tst",0x10000},
    {"pc_branch_history",0x10000},
    {"Risc <-> Apollo interrupt test",0x10000},    
    {"DSP <-> Apollo interrupt test",0x10000},  
    {"interrupt_tst",0x10000},
    {"aenv_tst",0x10000},
    {"mp3_tst",0x10000},
    {"aac_tst",0x10000},
    {"ac3_tst",0x10000},
    {"mtk_post_tst",0x10000},
    {"sxx_post_tst", 0x10000},
    {"dma_tst", 0x10000},
    {"risc_intf_tst", 0x10000},
    {"apollo_hold_tst", 0x10000},
    {"cmp_save_restore", 0x10000},
    {"risc_debugger", 0x10000},
    {"dmem_overnight", 0x10000},
    {"pmem_overnight", 0x10000},
    {"adsp_intf_overnight", 0x10000},
    {"pmem_cache_dump", 0x10000},
    {"getbs_dma", 0x10000},
    {"timer_27mhz", 0x10000},
    {"lxx_post_tst", 0x10000}
};

extern UINT32 AUD_ProcGetPc (void);
extern void DSP_FlushInvalidateDCacheFree(UINT32 u4Addr, UINT32 u4Len);
extern INT32 u4AudApolloLoadCode (UINT32 u4Base, UINT32 u4TblAddr, UINT32 u4Size);
extern void vAproc_Disable (void);
extern void vAproc_Enable (void);
extern void vAproc_2WayEnable (void);
extern void vApollo_SetDram(UINT32 u4ApolloDmAddr, UINT32 u4Value);
extern UINT32 u4Apollo_ReadDram(UINT32 u4ApolloDmAddr);
extern UINT32 u4ReadDspSram(UINT32 u4Addr);
extern void vWriteDspSram (UINT32 u4Addr, UINT32 u4Value);
extern UINT8 AUD_Apollo_Probe_SR_Chk_C10(void);
extern UINT8 AUD_Apollo_Probe_SR_Chk_C11(void);




extern UINT32 u4GetApolloWorkBufNonInit(UINT32 u4Type);
#ifndef CC_FPGA
extern BOOL MID_Reset(void);
#endif

#define APOLLO_PMEM_TEST_RESULT_DM_ADDR    (0x30000)
#define APOLLO_CACHE_TEST_LOOP_DM_ADDR     (0x30004)
#define APOLLO_PMEM_USER_BASE_ADDR         (0x30008)

#define APOLLO_CACHE_COUNT_TOLERANCE       (5)

//ADDR_CONFLICT_PAT
#define APOLLO_DMEM_ADDR_CONFLICT_PAT_RESULT (0x22000)
#define APOLLO_DMEM_ADDR_CONFLICT_TEST_RESULT (0x22004)
#define APOLLO_DMEM_ADDR_CONFLICT_PASS_RESULT (0x00000002)
#define APOLLO_DMEM_ADDR_CONFLICT_NG_RESULT (0xFFFFFFFF)

//READ_AFTER_WRITE_PAT
#define APOLLO_DMEM_READ_AFTER_WRITE_PAT_RESULT (0x20000)
#define APOLLO_DMEM_READ_AFTER_WRITE_TEST_RESULT (0x20004)
#define APOLLO_DMEM_READ_AFTER_WRITE_PASSRESULT (0x00000003)
#define APOLLO_DMEM_READ_AFTER_WRITE_NGRESULT (0xFFFFFFFF)

//SRAM_DIRTY_BIT_PAT
#define APOLLO_DMEM_SRAM_DIRTY_BIT_PAT_RESULT (0x22000)
#define APOLLO_DMEM_SRAM_DIRTY_BIT_TEST_RESULT (0x22004)
#define APOLLO_DMEM_SRAM_DIRTY_BIT_PASSRESULT (0x00000004)
#define APOLLO_DMEM_SRAM_DIRTY_BIT_NGRESULT (0xFFFFFFFF)

//INST_PAT
#define APOLLO_INST_PAT_RESULT (0x22000)
#define APOLLO_INST_PAT_RESULT_START (0x2)
#define APOLLO_INST_PAT_RESULT_PASS (0x1)
#define APOLLO_INST_PAT_RESULT_FAIL (0x99)

//INT_PAT
#define APOLLO_INT_PAT_RESULT (0x22000)
#define APOLLO_INT_PAT_RESULT_START (0x2)
#define APOLLO_INT_PAT_RESULT_PASS1 (0x10)
#define APOLLO_INT_PAT_RESULT_PASS2 (0x20)
#define APOLLO_INT_PAT_RESULT_PASS3 (0x30)
#define APOLLO_INT_PAT_RESULT_PASS4 (0x40)
#define APOLLO_INT_PAT_RESULT_PASS5 (0x50)
#define APOLLO_INT_PAT_RESULT_PASS6 (0x60)
#define APOLLO_INT_PAT_RESULT_PASS7 (0x70)
#define APOLLO_INT_PAT_RESULT_PASS8 (0x80)
#define APOLLO_INT_PAT_RESULT_FAIL (0x99)
#define APOLLO_INT_PAT_CONTROL (0x22004)
#define APOLLO_27MHz_TIMER_FPGA_TEST 0
#define APOLLO_27MHz_TIMER_IC_TEST 1

//AENV_PAT
#define APOLLO_AENV_PAT_RESULT (0x22000)
#define APOLLO_AENV_PAT_RESULT_START (0x2)
#define APOLLO_AENV_PAT_RESULT_PASS1 (0x10)
#define APOLLO_AENV_PAT_RESULT_FAIL (0x99)
#define APOLLO_ANEV_PAT_CONTROL (0x22004)
#define APOLLO_ANEV_MMR_TEST 1
#define APOLLO_ANEV_AOUT_TEST 2
#define APOLLO_ANEV_AOUT_ASRC_TEST 3
#define APOLLO_ANEV_IEC_RAW_OUT_TEST 4
#define APOLLO_ANEV_AOUT_TIMER0_TEST 5

//COMPACT_SAVE_RESTORE
#define APOLLO_DMEM_COMPACT_SAVE_RESTORE_ADD_SETTING (0x20000)
#define APOLLO_DMEM_COMPACT_SAVE_RESTORE_PAT_RESULT (0x20004)
#define APOLLO_DMEM_COMPACT_SAVE_RESTORE_TEST_RESULT (0x20008)

#define APOLLO_DMEM_COMPACT_SAVE_RESTORE_PASSRESULT (0x00000031)
#define APOLLO_DMEM_COMPACT_SAVE_RESTORE_NGRESULT (0xFFFFFFFF)

//PUTBITS_PAT
#define APOLLO_PUTBITS_PAT_RESULT_ADDR (0x20000)
#define APOLLO_PUTBITS_PAT_PHICAL_DRAM_START_ADDR (0x20004)

//PCBranchHis_PAT
#define APOLLO_PCBranchHis_PAT_RESULT_ADDR (0x20000)

#define APOLLO_DMEM_NORM_SBLK (0x24000)
#define APOLLO_DMEM_CMPT_SBLK (0x24004)
#define APOLLO_DMEM_TEST_FINISH (0x24008)
#define APOLLO_DMEM_AFIFO_SBLK (0x24000)
#define APOLLO_DMEM_AFIFO_EBLK (0x24004)

//ADDR_DMEM_PAT
#define APOLLO_DMEM_TEST1_ADDR 0x24010

#define APOLLO_DMEM_PASS_RESULT (0x1)
#define APOLLO_DMEM_NG_RESULT (0xF)

#define APOLLO_DMEM_SRAM1_CASE 8
#define APOLLO_DMEM_DRAM1_CASE 16
#define APOLLO_DMEM_DRAM2_CASE 16
#define APOLLO_DMEM_DRAM3_CASE 8
#define APOLLO_DMEM_MIX1_CASE 72
#define APOLLO_DMEM_MIX2_CASE 72
#define APOLLO_DMEM_MIX3_CASE 36
#define APOLLO_GETBS_CNT_LDZ_CASE 80
#define APOLLO_GETBS_FUNCTION_CASE 176
#define APOLLO_GETBS_CRC_CASE 5
#define APOLLO_PSRDMA_CASE 2

//RISC <-> Apollo Test PAT
#define APOLLO_RISC2APOLLO_PAT_RESULT_FINAL_ROUND1  (0x20000)//The final test result for RISC -> Apollo interrupt
#define APOLLO_APOLLO2RISC_PAT_RESULT_FINAL_ROUND1  (0x20020)//The final test result for Apollo -> Risc interrupt
#define APOLLO_RISC2APOLLO_FINISH_IDENTIFY_ROUND1   (0x20040)//0xffff: not ready, 0x8888:ready
#define APOLLO_RISC2APOLLO_PAT_RESULT_FINAL_ROUND2  (0x20060)//The final test result for RISC -> Apollo interrupt
#define APOLLO_APOLLO2RISC_PAT_RESULT_FINAL_ROUND2  (0x20080)//The final test result for Apollo -> Risc interrupt
#define APOLLO_RISC2APOLLO_FINISH_IDENTIFY_ROUND2   (0x200a0)//0xffff: not ready, 0x8888:ready

#define APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND1 (0x20300)//for test bit0, round 1
#define APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND1 (0x20340)//for test bit1, round 1
#define APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND1 (0x20380)//for test bit2, round 1
#define APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND1 (0x203c0)//for test bit3, round 1
#define APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND2 (0x20500)//for test bit0, round 2
#define APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND2 (0x20540)//for test bit1, round 2
#define APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND2 (0x20580)//for test bit2, round 2
#define APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND2 (0x205c0)//for test bit3, round 2


//DSP <-> Apollo Test PAT
#define APOLLO_DSP_APOLLO_PAT_ROUND_NUM            (0x21000)// 1:ROUND 1 , 2: Round2
#define APOLLO_DSP2APOLLO_PAT_RESULT_FINAL_ROUND1  (0x20000)//The final test result for DSP -> Apollo interrupt round 1, Apollo Dram
#define APOLLO_DSP2APOLLO_PAT_RESULT_FINAL_ROUND2  (0x20004)//The final test result for DSP -> Apollo interrupt round 2, Apollo Dram
#define APOLLO_APOLLO2DSP_PAT_RESULT_FINAL_ROUND1  (0x2001)//The final test result for Apollo -> DSP interrupt round 1, DSP sram
#define APOLLO_APOLLO2DSP_PAT_RESULT_FINAL_ROUND2  (0x2002)//The final test result for Apollo -> DSPinterrupt round 2, DSP sram

//For Test Apollo Hold
#define APOLLO_HOLD__WriteCertain_DRAM_RC2AP_IDENTIFY_2  (0x20100)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__WriteCertain_DRAM_RC2AP_IDENTIFY_3  (0x20104)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__WriteCertain_DRAM_RC2AP_IDENTIFY_4  (0x20108)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__WriteCertain_DRAM_RC2AP_IDENTIFY_5  (0x2010c)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__WriteCertain_DRAM_RC2AP_IDENTIFY_6  (0x20110)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__WriteCertain_DRAM_RC2AP_IDENTIFY_7  (0x20114)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__WriteCertain_DRAM_RC2AP_IDENTIFY_8  (0x20118)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__WriteCertain_DRAM_RC2AP_IDENTIFY_9  (0x2011c)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__WriteCertain_DRAM_RC2AP_IDENTIFY_10 (0x20120)//0xffff: not ready, 0x8888:ready, RC -> AP

#define APOLLO_HOLD__WriteDramOutside_RC2AP_IDENTIFY_1   (0x20140)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__WriteDramOutside_RC2AP_IDENTIFY_2   (0x20144)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__WriteDramOutside_RC2AP_IDENTIFY_3   (0x20148)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__WriteDramOutside_RC2AP_IDENTIFY_4   (0x2014c)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__WriteDramOutside_RC2AP_IDENTIFY_5   (0x20150)//0xffff: not ready, 0x8888:ready, RC -> AP

#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_1  (0x20160)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_2  (0x20164)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_3  (0x20168)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_4  (0x2016c)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_5  (0x20170)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_6  (0x20174)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_7  (0x20178)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_8  (0x2017c)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_9  (0x20180)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_10 (0x20184)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_11 (0x20188)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_12 (0x2018c)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_13 (0x20190)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_14 (0x20194)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_15 (0x20198)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_16 (0x2019c)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_17 (0x201a0)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_18 (0x201a4)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_19 (0x201a8)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_20 (0x201ac)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_21 (0x201b0)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_22 (0x201b4)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_23 (0x201b8)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_24 (0x201bc)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_25 (0x201c0)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_26 (0x201c4)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_27 (0x201c8)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_28 (0x201cc)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_29 (0x201d0)//0xffff: not ready, 0x8888:ready, RC -> AP
#define APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_30 (0x201d4)//0xffff: not ready, 0x8888:ready, RC -> AP

//SMSE_PAT
#define APOLLO_SMSE_PAT_RESULT (0x22000)
#define APOLLO_SMSE_PAT_RESULT_START (0x2)
#define APOLLO_SMSE_PAT_RESULT_PASS (0x1)
#define APOLLO_SMSE_PAT_RESULT_FAIL (0x99)
#define APOLLO_SMSE_TEST_FINISH (0x22004)
#define APOLLO_SMSE_TEST_FINISH_NG (0x0)
#define APOLLO_SMSE_TEST_FINISH_OK (0x1)


void vApollo_SetDram(UINT32 u4ApolloDmAddr, UINT32 u4Value)
{
   UINT32 u4DmSblk;
   
   u4DmSblk = u4ApolloGet_RegDmSblk();
   u4DmSblk += u4ApolloDmAddr;
   
   *(volatile UINT32 *)(VIRTUAL(u4DmSblk)) = u4Value;

   u4DmSblk &= 0xFFFFFFE0; //make sure addr & len is 32 byte alignment.
    HalFlushInvalidateDCacheMultipleLine(VIRTUAL(u4DmSblk), 0x20);

}

UINT32 u4Apollo_ReadDram(UINT32 u4ApolloDmAddr)
{
   UINT32 u4DmSblk;
   
   u4DmSblk = u4ApolloGet_RegDmSblk();
   u4DmSblk += u4ApolloDmAddr;

   DSP_FlushInvalidateDCacheFree(u4DmSblk, 4);

   return  *(volatile UINT32 *)(VIRTUAL(u4DmSblk));
}

void vApollo_SetDspDram(UINT32 u4DspDmAddr, UINT32 u4Value)
{
    *(UINT32 *)(VIRTUAL(u4DspDmAddr)) = (u4Value);

    u4DspDmAddr &= 0xFFFFFFE0; //make sure addr & len is 32 byte alignment.
    HalFlushInvalidateDCacheMultipleLine(VIRTUAL(u4DspDmAddr), 0x20);

}

UINT32 u4Apollo_ReadDspDram(UINT32 u4DspDmAddr)
{
   DSP_FlushInvalidateDCacheFree(u4DspDmAddr, 4);

   return  *(UINT32 *)(VIRTUAL(u4DspDmAddr));
}

void vApollo_PgSeqTst(void)
{
    Printf("\nEmu 007 -[%d] Program memory sequential acess test ....\n", __TIME__);

    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &PM_SEQUENTIAL_ACCESS_TABLE[0], PM_SEQUENTIAL_ACCESS_SIZE); 
    vAproc_Enable();

    // sleep
    x_thread_delay(1000);
                   
    // check result
    if (u4Apollo_ReadDram(APOLLO_PMEM_TEST_RESULT_DM_ADDR) == 0x12345678)
    {
        Printf("\n Verify OK !\n");
    }
    else
    {
        Printf("\n Verify Fail !\n");
    }
}

inline INT32 aabs(INT32 x)
{
    if (x >= 0)
        return x;
    else
        return -x;
}

void vApollo_CacheTst(void)
{
    UINT32 PmAccCnt[3];            
    UINT32 PmAccHitCnt[3];        
    UINT32 PmTotalAccCnt[3];      
    UINT32 PgCacheHitCnt[3];      
    UINT32 PgCacheMissCnt[3];     
    UINT32 PgCacheMissPenalty[3]; 
    UINT32 PgTotalCycle[3];       
    UINT32 StartPc              = 0x4100;
    UINT32 EndPc                = 0x8C00;
    UINT32 i,j;

    Printf("\nEmu 011 -[%d] Cache statistics hardware test ....\n", __TIME__);

    Printf("\n Monitor Cache between 0x%x-0x%x", StartPc, EndPc);

    for (j = 0; j <= 1; j++)
    {
        Printf("\n\n =============================\n\n");

        if (j == 1)
        {
            Printf("\n Part2 : 4-way cahce test \n");
        }
        else
        {
            Printf("\n Part1 : 2-way cahce test \n");
        }
            
   
        for (i = 0; i <= 2 ; i ++)
        {
            vApollo_HaltOn(); //Apollo Hold  before reset Apollo
        
            vAproc_Disable();
            u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &CACHE_STATISTIC_TABLE[0], CACHE_STATISTIC_SIZE); 
            u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1),(UINT32) &CACHE_STATISTIC_DATA[0], CACHE_STATISTIC_DATA_SIZE); 

            // set loop count 
            vApollo_SetDram(APOLLO_CACHE_TEST_LOOP_DM_ADDR, i+1);


            if (j == 1)
            {
               vAproc_Enable(); // 4-way cache
            }
            else
            {
               vAproc_2WayEnable(); // 2-way cache
            }
        
            vApolloSet_DbgMonStartPc(StartPc);
            vApolloSet_DbgMonStopPc(EndPc);
            vApollo_DbgMonCntClr();
            vApollo_DbgMonIterEn();
            vApollo_DbgMonPcEn(); 

            vApollo_HaltOff(); //release Apollo Hold

            // sleep
            x_thread_delay(1000);

            PmAccCnt[i]                = u4ApolloGet_PmAccCnt();
            PmAccHitCnt[i]             = u4ApolloGet_PmAccHitCnt();
            PmTotalAccCnt[i]           = u4ApolloGet_PmTotalAccCnt();
            PgCacheHitCnt[i]           = u4ApolloGet_PgCacheHitCnt();
            PgCacheMissCnt[i]          = u4ApolloGet_PgCacheMissCnt();
            PgCacheMissPenalty[i]      = u4ApolloGet_PgCacheMissPenalty();
            PgTotalCycle[i]            = u4ApolloGet_DbgMonTotalCycle();
                            
			
            Printf("\n\n == Run Loop %d Times ==> ", i+1);

            // check result
            if (u4Apollo_ReadDram(APOLLO_PMEM_TEST_RESULT_DM_ADDR) == 0x12345678)
            {
                Printf("Result OK !\n");
            }
            else
            {
                Printf("Result Fail !\n");
            }
            
            Printf("\n PmAccCnt           = 0x%08x", PmAccCnt[i]);
            Printf("\n PmAccHitCnt        = 0x%08x", PmAccHitCnt[i]);
            Printf("\n PmTotalAccCnt      = 0x%08x", PmTotalAccCnt[i]);
            Printf("\n PgCacheHitCnt      = 0x%08x", PgCacheHitCnt[i]);
            Printf("\n PgCacheMissCnt     = 0x%08x", PgCacheMissCnt[i]);
            Printf("\n PgCacheMissPenalty = 0x%08x", PgCacheMissPenalty[i]);
            Printf("\n PgTotalCycle       = 0x%08x", PgTotalCycle[i]);
		
        }

        // for loop count = 2, 3, PgCacheMissCnt should be equal because after loop 1 , all pg are in cache now. miss count
        // should not be added.
        Printf("\n\n Verify 1 : PgCacheMissCnt ==>");

        if ((aabs(PgCacheMissCnt[0] - PgCacheMissCnt[1]) <= APOLLO_CACHE_COUNT_TOLERANCE) &&
            (aabs(PgCacheMissCnt[1] - PgCacheMissCnt[2]) <= APOLLO_CACHE_COUNT_TOLERANCE) &&
            (PgCacheMissCnt[0] != 0))
        {
            Printf(" OK !");
        } 
        else
        {
            Printf(" FAIL !");
        }

        // The PmAccCnt difference for (loop count 2,1) should be equal to (loop count 3,2)
        Printf("\n Verify 2 : PmAccCnt       ==>");

        if ((PmAccCnt[1] - PmAccCnt[0]) == (PmAccCnt[2] - PmAccCnt[1]))
        {
            Printf(" OK !");
        }
        else
        {
            Printf(" FAIL !");
        }

        // The PgCacheHitCnt difference for loop count 3 and 2 should be equal to the PmAccCnt difference of loop count 3 and 2
        // It means for third loop, all pm access are cache hit.
        Printf("\n Verify 3 : PgCacheHitCnt  ==>");

        if (aabs((PmAccCnt[2] - PmAccCnt[1]) - (PgCacheHitCnt[2] - PgCacheHitCnt[1])) <= APOLLO_CACHE_COUNT_TOLERANCE)
        {
            Printf(" OK !");
        }
        else
        {
            Printf(" FAIL !");
        }
    }

    Printf("\n");

}

void vApollo_PgBranchTst(void)
{
    Printf("\nEmu 008 -[%d] Program memory branch acess test ....\n", __TIME__);

    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &PM_BRANCH_TABLE[0], PM_BRANCH_SIZE); 
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1),(UINT32) &PM_BRANCH_DATA[0], PM_BRANCH_DATA_SIZE); 
    vAproc_2WayEnable();

    // sleep
    x_thread_delay(1000);
                   
    // check result
    if (u4Apollo_ReadDram(APOLLO_PMEM_TEST_RESULT_DM_ADDR) == 0x12345678)
    {
        Printf("\n Verify OK !\n");
    }
    else
    {
        Printf("\n Verify Fail !\n");
    }
}

void vApollo_4WayTst(void)
{
    UINT32 PmAccCnt[3];            
    UINT32 PmAccHitCnt[3];        
    UINT32 PmTotalAccCnt[3];      
    UINT32 PgCacheHitCnt[3];      
    UINT32 PgCacheMissCnt[3];     
    UINT32 PgCacheMissPenalty[3]; 
    UINT32 PgTotalCycle[3];       
    UINT32 StartPc[3] ={0xb8, 0xe4, 0x13c};
    UINT32 EndPc [3]  ={0xe0, 0x114,0x164};
    UINT32 i;

    Printf("\nEmu 010 -[%d] 4-Way cache stress test ....\n", __TIME__);

    for (i = 0; i < 3; i++)
    {
        vApollo_HaltOn();//Apollo Hold  before reset Apollo
    
        vAproc_Disable();
        u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &CACHE_4WAY_TABLE[0], CACHE_4WAY_SIZE); 

        vAproc_Enable(); // 4-way cache

        vApolloSet_DbgMonStartPc(StartPc[i]);
        vApolloSet_DbgMonStopPc(EndPc[i]);
        vApollo_DbgMonCntClr();
        vApollo_DbgMonIterEn();
        vApollo_DbgMonPcEn(); 

        vApollo_HaltOff(); //release Apollo Hold

        // sleep
        x_thread_delay(1000);

        PmAccCnt[i]                = u4ApolloGet_PmAccCnt();
        PmAccHitCnt[i]             = u4ApolloGet_PmAccHitCnt();
        PmTotalAccCnt[i]           = u4ApolloGet_PmTotalAccCnt();
        PgCacheHitCnt[i]           = u4ApolloGet_PgCacheHitCnt();
        PgCacheMissCnt[i]          = u4ApolloGet_PgCacheMissCnt();
        PgCacheMissPenalty[i]      = u4ApolloGet_PgCacheMissPenalty();
        PgTotalCycle[i]            = u4ApolloGet_DbgMonTotalCycle();

        
        if (i == 0)
        {
             Printf("\n\n == Run Pg A, B, C, D 512 ==> ");
        }
        else if (i == 1)
        {
             Printf("\n\n == Run Pg A, B, C, D 512, E 256 ==> ");
        }
        else
        {
             Printf("\n\n == Run Pg A, B, C, F 550 ==> ");
        }
      

        // check result
        if (u4Apollo_ReadDram(APOLLO_PMEM_TEST_RESULT_DM_ADDR) == 0x12345678)
        {
            Printf("Result OK !\n");
        }
        else
        {
            Printf("Result Fail !\n");
        }


        Printf("\n PmAccCnt           = 0x%08x", PmAccCnt[i]);
        Printf("\n PmAccHitCnt        = 0x%08x", PmAccHitCnt[i]);
        Printf("\n PmTotalAccCnt      = 0x%08x", PmTotalAccCnt[i]);
        Printf("\n PgCacheHitCnt      = 0x%08x", PgCacheHitCnt[i]);
        Printf("\n PgCacheMissCnt     = 0x%08x", PgCacheMissCnt[i]);
        Printf("\n PgCacheMissPenalty = 0x%08x", PgCacheMissPenalty[i]);
        Printf("\n PgTotalCycle       = 0x%08x", PgTotalCycle[i]);

    }                           


     Printf("\n\n Verify  : PgCacheMissCnt  ==>");

     if ((PgCacheMissCnt[0] < 60) &&
         (PgCacheMissCnt[1] > 200) &&
         (PgCacheMissCnt[2] > 200))
     {
         Printf(" OK !");
     }
     else
     {
         Printf(" FAIL !");
     }

     Printf("\n");

}

void vApollo_2WayTst(void)
{
    UINT32 PmAccCnt[3];            
    UINT32 PmAccHitCnt[3];        
    UINT32 PmTotalAccCnt[3];      
    UINT32 PgCacheHitCnt[3];      
    UINT32 PgCacheMissCnt[3];     
    UINT32 PgCacheMissPenalty[3]; 
    UINT32 PgTotalCycle[3];       
    UINT32 StartPc[3] ={0x84, 0x9c, 0xd4};
    UINT32 EndPc [3]  ={0x98, 0xb8, 0xe8};
    UINT32 i;

    Printf("\nEmu 009 -[%d] 2-Way cache stress test ....\n", __TIME__);

    for (i = 0; i < 3; i++)
    {
        vApollo_HaltOn(); //Apollo Hold  before reset Apollo
    
        vAproc_Disable();
        u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &CACHE_2WAY_TABLE[0], CACHE_2WAY_SIZE); 

        vAproc_2WayEnable(); // 2-way cache

        vApolloSet_DbgMonStartPc(StartPc[i]);
        vApolloSet_DbgMonStopPc(EndPc[i]);
        vApollo_DbgMonCntClr();
        vApollo_DbgMonIterEn();
        vApollo_DbgMonPcEn(); 

        vApollo_HaltOff(); //release Apollo Hold

        // sleep
        x_thread_delay(1000);

        PmAccCnt[i]                = u4ApolloGet_PmAccCnt();
        PmAccHitCnt[i]             = u4ApolloGet_PmAccHitCnt();
        PmTotalAccCnt[i]           = u4ApolloGet_PmTotalAccCnt();
        PgCacheHitCnt[i]           = u4ApolloGet_PgCacheHitCnt();
        PgCacheMissCnt[i]          = u4ApolloGet_PgCacheMissCnt();
        PgCacheMissPenalty[i]      = u4ApolloGet_PgCacheMissPenalty();
        PgTotalCycle[i]            = u4ApolloGet_DbgMonTotalCycle();

        
        if (i == 0)
        {
             Printf("\n\n == Run Pg A, B 1K ==> ");
        }
        else if (i == 1)
        {
             Printf("\n\n == Run Pg A, B 1K, C 512 ==> ");
        }
        else
        {
             Printf("\n\n == Run Pg A 1K B 1.05K ==> ");
        }
      

        // check result
        if (u4Apollo_ReadDram(APOLLO_PMEM_TEST_RESULT_DM_ADDR) == 0x12345678)
        {
            Printf("Result OK !\n");
        }
        else
        {
            Printf("Result Fail !\n");
        }


        Printf("\n PmAccCnt           = 0x%08x", PmAccCnt[i]);
        Printf("\n PmAccHitCnt        = 0x%08x", PmAccHitCnt[i]);
        Printf("\n PmTotalAccCnt      = 0x%08x", PmTotalAccCnt[i]);
        Printf("\n PgCacheHitCnt      = 0x%08x", PgCacheHitCnt[i]);
        Printf("\n PgCacheMissCnt     = 0x%08x", PgCacheMissCnt[i]);
        Printf("\n PgCacheMissPenalty = 0x%08x", PgCacheMissPenalty[i]);
        Printf("\n PgTotalCycle       = 0x%08x", PgTotalCycle[i]);

    }                           


     Printf("\n\n Verify  : PgCacheMissCnt  ==>");

     if ((PgCacheMissCnt[0] < 20) &&
         (PgCacheMissCnt[1] > 100) &&
         (PgCacheMissCnt[2] > 100))
     {
         Printf(" OK !");
     }
     else
     {
         Printf(" FAIL !");
     }

     Printf("\n");

}

void vApollo_PmUserAddrTst(void)
{
    UINT32 u4UpExtMemSadr[3] ={0x1000, 0x10000, 0xF0000};
    UINT32 i = 1;
    int j;

    Printf("\nEmu 012 -[%d] External Pmem sadr set to different address test ....\n", __TIME__);


    for (i = 1; i <= 2; i++)  // skip case 1 0x1000 - because external memory address needs 64K alignmenet 
    {
        for (j = 0; j <= 1; j++)
        {
            if (j == 0)
            {
                Printf("\n\nCase %d-1 (MMR) : UPEXT_MEM_SADR = 0x%x ==>", i+1, u4UpExtMemSadr[i]);
            }
            else
            {
                Printf("\n\nCase %d-2 (RISC): UPEXT_MEM_SADR = 0x%x ==>", i+1, u4UpExtMemSadr[i]);
            }

            vAproc_Disable();

            switch (i)
            {
            
               case 0:
                    // load kernel program to 0x0
                    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &EXT_PM_SADR_TABLE_0_KERNEL[0], EXT_PM_SADR_SIZE_0_KERNEL); 
                    break;
               case 1:
                    // load kernel program to 0x0
                    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &EXT_PM_SADR_TABLE_1_KERNEL[0], EXT_PM_SADR_SIZE_1_KERNEL); 
                    break;
               case 2:
                    // load kernel program to 0x0
                    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &EXT_PM_SADR_TABLE_2_KERNEL[0], EXT_PM_SADR_SIZE_2_KERNEL); 
                    break;

                default:
                    break;
            }
            
            // load user program to offset 0x10000
            u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0)+0x10000,(UINT32) &EXT_PM_SADR_TABLE_USER[0], EXT_PM_SADR_SIZE_USER); 

            vApolloSet_UpExtMemSadr(u4UpExtMemSadr[i]);


            if (j == 0)
            {
                // set physical address for MMR
                vApollo_SetDram(APOLLO_PMEM_USER_BASE_ADDR, u4GetApolloWorkBufNonInit(0)+0x10000-u4UpExtMemSadr[i]);
                // User space address base control by DSP MMR
                vApolloSet_UpAddrBaseByDsp();
            }                
            else
            {
                // User space address base control by RISC
                vApolloSet_UpAddrBaseByRisc(u4GetApolloWorkBufNonInit(0)+0x10000-u4UpExtMemSadr[i]);
            }

            vAproc_2WayEnable();

            // sleep
            x_thread_delay(1000);
                           
            // check result
            if (u4Apollo_ReadDram(APOLLO_PMEM_TEST_RESULT_DM_ADDR) == 0x12345678)
            {
                Printf(" Verify OK !\n");
            }
            else
            {
                Printf(" Verify Fail !\n");
            }
        }
    }        
}

void vApollo_Putbits_test(void)
{
    Printf("\nEmu 017 -[%d] Putbits test ....\n", __TIME__);

    //initial  Apollo Dram    
	vApollo_SetDram(APOLLO_PUTBITS_PAT_RESULT_ADDR,0xffff);  //initial APOLLO_PUTBITS_PAT_RESULT_ADDR=0xffff

 
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_PUTBITS_TEST_TABLE[0], APOLLO_PUTBITS_TEST_SIZE); 
	vApollo_SetDram(APOLLO_PUTBITS_PAT_PHICAL_DRAM_START_ADDR,u4GetApolloWorkBufNonInit(1)-0x20000);//pass the phicial Dram start to putbits function
    vAproc_Enable();

    // sleep
    x_thread_delay(1000);
                   
    // check result
    if (u4Apollo_ReadDram(APOLLO_PUTBITS_PAT_RESULT_ADDR) == 0x8888)
    {
        Printf("\n Verify OK !\n");
    }
    else
    {
        Printf("\n Verify Fail !\n");
    }
}

void vApollo_PCBranchHis_test(void)
{
    Printf("\nEmu 018 -[%d] PC Branch History test ....\n", __TIME__);

    //initial  Apollo Dram    
	vApollo_SetDram(APOLLO_PCBranchHis_PAT_RESULT_ADDR,0xffff);  //initial APOLLO_PCBranchHis_PAT_RESULT_ADDR=0xffff


    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_PCBranchHis_TEST_TABLE[0], APOLLO_PCBranchHis_TEST_SIZE); 
    vAproc_Enable();

    // sleep
    x_thread_delay(1000);
                   
    // check result
    if (u4Apollo_ReadDram(APOLLO_PCBranchHis_PAT_RESULT_ADDR) == 0x8888)
    {
        Printf("\n Verify OK !\n");
    }
    else
    {
        Printf("\n Verify Fail !\n");
    }
}

UINT8 vApollo_dmem_sram1_tst(void)
{
    UINT32 i, u1CntRepeat = 0;
    UINT32 u4TestResult[APOLLO_DMEM_SRAM1_CASE];

    vApollo_SetDram(APOLLO_DMEM_NORM_SBLK, u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR);
    vApollo_SetDram(APOLLO_DMEM_CMPT_SBLK, u4GetApolloWorkBufNonInit(1));
    vApollo_SetDram(APOLLO_DMEM_TEST_FINISH, APOLLO_DMEM_NG_RESULT);
    for(i = 0; i < APOLLO_DMEM_SRAM1_CASE; i++)
        vApollo_SetDram(APOLLO_DMEM_TEST1_ADDR+i*4, APOLLO_DMEM_NG_RESULT);

    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0), (UINT32) &DMEM_SRAM1_TST_TABLE[0], DMEM_SRAM1_TST_SIZE); 
    vAproc_Enable();

    // wait for test done or timeout
    while((u4Apollo_ReadDram(APOLLO_DMEM_TEST_FINISH) != APOLLO_DMEM_PASS_RESULT)&&(u1CntRepeat<8))
    {
        x_thread_delay(500);
        u1CntRepeat++;
    }

    // check result
    for(i = 0; i < APOLLO_DMEM_SRAM1_CASE; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*4);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[DMEM] SRAM1 Test: case %d Pass \n", i+1);
        else
        {
            printf("[DMEM] SRAM1 Test: case %d Failed \n", i+1);
            printf("                       failed addr = 0x%x \n", u4TestResult[i]);
        }
    }

    for(i = 0; i < APOLLO_DMEM_SRAM1_CASE; i++)
    {
        if (u4TestResult[i] != APOLLO_DMEM_PASS_RESULT)
            return 0;
    }
    return 1;
}

UINT8 vApollo_dmem_dram1_tst(void)
{
    UINT32 i, u1CntRepeat = 0;
    UINT32 u4TestResult[APOLLO_DMEM_DRAM1_CASE];

    vApollo_SetDram(APOLLO_DMEM_NORM_SBLK, u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR);
    vApollo_SetDram(APOLLO_DMEM_CMPT_SBLK, u4GetApolloWorkBufNonInit(1));
    vApollo_SetDram(APOLLO_DMEM_TEST_FINISH, APOLLO_DMEM_NG_RESULT);
    for(i = 0; i < APOLLO_DMEM_DRAM1_CASE; i++)
        vApollo_SetDram(APOLLO_DMEM_TEST1_ADDR+i*4, APOLLO_DMEM_NG_RESULT);

    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0), (UINT32) &DMEM_DRAM1_TST_TABLE[0], DMEM_DRAM1_TST_SIZE); 
    vAproc_Enable();

    // wait for test done or timeout
    while((u4Apollo_ReadDram(APOLLO_DMEM_TEST_FINISH) != APOLLO_DMEM_PASS_RESULT)&&(u1CntRepeat<8))
    {
        x_thread_delay(500);
        u1CntRepeat++;
    }

    // check result
    for(i = 0; i < APOLLO_DMEM_DRAM1_CASE; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*4);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[DMEM] DRAM1 Test: case %d Pass \n", i+1);
        else
        {
            printf("[DMEM] DRAM1 Test: case %d Failed \n", i+1);
            printf("                       failed addr = 0x%x \n", u4TestResult[i]);
        }
    }

    for(i = 0; i < APOLLO_DMEM_DRAM1_CASE; i++)
    {
        if (u4TestResult[i] != APOLLO_DMEM_PASS_RESULT)
            return 0;
    }
    return 1;
}

UINT8 vApollo_dmem_dram2_tst(void)
{
    UINT32 i, u1CntRepeat = 0;
    UINT32 u4TestResult[APOLLO_DMEM_DRAM2_CASE];

    vApollo_SetDram(APOLLO_DMEM_NORM_SBLK, u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR);
    vApollo_SetDram(APOLLO_DMEM_CMPT_SBLK, u4GetApolloWorkBufNonInit(1));
    vApollo_SetDram(APOLLO_DMEM_TEST_FINISH, APOLLO_DMEM_NG_RESULT);
    for(i = 0; i < APOLLO_DMEM_DRAM2_CASE; i++)
        vApollo_SetDram(APOLLO_DMEM_TEST1_ADDR+i*4, APOLLO_DMEM_NG_RESULT);

    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0), (UINT32) &DMEM_DRAM2_TST_TABLE[0], DMEM_DRAM2_TST_SIZE); 
    vAproc_Enable();

    // wait for test done or timeout
    while((u4Apollo_ReadDram(APOLLO_DMEM_TEST_FINISH) != APOLLO_DMEM_PASS_RESULT)&&(u1CntRepeat<8))
    {
        x_thread_delay(500);
        u1CntRepeat++;
    }

    // check result
    for(i = 0; i < APOLLO_DMEM_DRAM2_CASE; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*4);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[DMEM] DRAM2 Test: case %d Pass \n", i+1);
        else
        {
            printf("[DMEM] DRAM2 Test: case %d Failed \n", i+1);
            printf("                       failed addr = 0x%x \n", u4TestResult[i]);
        }
    }

    for(i = 0; i < APOLLO_DMEM_DRAM2_CASE; i++)
    {
        if (u4TestResult[i] != APOLLO_DMEM_PASS_RESULT)
            return 0;
    }
    return 1;
}

UINT8 vApollo_dmem_dram3_tst(void)
{
    UINT32 i, u1CntRepeat = 0;
    UINT32 u4TestResult[APOLLO_DMEM_DRAM3_CASE];

    vApollo_SetDram(APOLLO_DMEM_NORM_SBLK, u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR);
    vApollo_SetDram(APOLLO_DMEM_CMPT_SBLK, u4GetApolloWorkBufNonInit(1));
    vApollo_SetDram(APOLLO_DMEM_TEST_FINISH, APOLLO_DMEM_NG_RESULT);
    for(i = 0; i < APOLLO_DMEM_DRAM3_CASE; i++)
        vApollo_SetDram(APOLLO_DMEM_TEST1_ADDR+i*4, APOLLO_DMEM_NG_RESULT);

    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0), (UINT32) &DMEM_DRAM3_TST_TABLE[0], DMEM_DRAM3_TST_SIZE); 
    vAproc_Enable();

    // wait for test done or timeout
    while((u4Apollo_ReadDram(APOLLO_DMEM_TEST_FINISH) != APOLLO_DMEM_PASS_RESULT)&&(u1CntRepeat<8))
    {
        x_thread_delay(500);
        u1CntRepeat++;
    }

    // check result
    for(i = 0; i < APOLLO_DMEM_DRAM3_CASE; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*4);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[DMEM] DRAM3 Test: case %d Pass \n", i+1);
        else
        {
            printf("[DMEM] DRAM3 Test: case %d Failed \n", i+1);
            printf("                       failed addr = 0x%x \n", u4TestResult[i]);
        }
    }

    for(i = 0; i < APOLLO_DMEM_DRAM3_CASE; i++)
    {
        if (u4TestResult[i] != APOLLO_DMEM_PASS_RESULT)
            return 0;
    }
    return 1;
}

UINT8 vApollo_dmem_mix1_tst(void)
{
    UINT32 i, u1CntRepeat = 0;
    UINT32 u4TestResult[APOLLO_DMEM_MIX1_CASE];

    vApollo_SetDram(APOLLO_DMEM_NORM_SBLK, u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR);
    vApollo_SetDram(APOLLO_DMEM_CMPT_SBLK, u4GetApolloWorkBufNonInit(1));
    vApollo_SetDram(APOLLO_DMEM_TEST_FINISH, APOLLO_DMEM_NG_RESULT);
    for(i = 0; i < APOLLO_DMEM_MIX1_CASE; i++)
        vApollo_SetDram(APOLLO_DMEM_TEST1_ADDR+i*4, APOLLO_DMEM_NG_RESULT);

    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0), (UINT32) &DMEM_MIX1_TST_TABLE[0], DMEM_MIX1_TST_SIZE); 
    vAproc_Enable();

    // wait for test done or timeout
    while((u4Apollo_ReadDram(APOLLO_DMEM_TEST_FINISH) != APOLLO_DMEM_PASS_RESULT)&&(u1CntRepeat<8))
    {
        x_thread_delay(500);
        u1CntRepeat++;
    }

    // check result
    for(i = 0; i < APOLLO_DMEM_MIX1_CASE; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*4);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[DMEM] Mix1 Test: case %d Pass \n", i+1);
        else
        {
            printf("[DMEM] Mix1 Test: case %d Failed \n", i+1);
            printf("                       failed addr = 0x%x \n", u4TestResult[i]);
        }
    }

    for(i = 0; i < APOLLO_DMEM_MIX1_CASE; i++)
    {
        if (u4TestResult[i] != APOLLO_DMEM_PASS_RESULT)
            return 0;
    }
    return 1;
}

UINT8 vApollo_dmem_mix2_tst(void)
{
    UINT32 i, u1CntRepeat = 0;
    UINT32 u4TestResult[APOLLO_DMEM_MIX2_CASE];

    vApollo_SetDram(APOLLO_DMEM_NORM_SBLK, u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR);
    vApollo_SetDram(APOLLO_DMEM_CMPT_SBLK, u4GetApolloWorkBufNonInit(1));
    vApollo_SetDram(APOLLO_DMEM_TEST_FINISH, APOLLO_DMEM_NG_RESULT);
    for(i = 0; i < APOLLO_DMEM_MIX2_CASE; i++)
        vApollo_SetDram(APOLLO_DMEM_TEST1_ADDR+i*4, APOLLO_DMEM_NG_RESULT);

    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0), (UINT32) &DMEM_MIX2_TST_TABLE[0], DMEM_MIX2_TST_SIZE); 
    vAproc_Enable();

    // wait for test done or timeout
    while((u4Apollo_ReadDram(APOLLO_DMEM_TEST_FINISH) != APOLLO_DMEM_PASS_RESULT)&&(u1CntRepeat<8))
    {
        x_thread_delay(500);
        u1CntRepeat++;
    }
    if (u1CntRepeat==8) printf("Mix2 Test time-out!!\n");

    // check result
    for(i = 0; i < APOLLO_DMEM_MIX2_CASE; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*8);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
        {
            u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*8+4);
            if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
                printf("[DMEM] Mix2 Test: case %d Pass \n", i+1);
            else
            {
                printf("[DMEM] Mix2 Test: case %d Failed \n", i+1);
                printf("                       failed addr = 0x%x \n", u4TestResult[i]);
            }
        }
        else
        {
            printf("[DMEM] Mix2 Test: case %d Failed \n", i+1);
            printf("                       failed addr = 0x%x \n", u4TestResult[i]);
        }
    }

    for(i = 0; i < APOLLO_DMEM_MIX2_CASE; i++)
    {
        if ((i != 5)&&(i != 15)&&(i != 25)&&(i != 33)&&(i != 41)&&(i != 49)&&(i != 57)&&(i != 67))
        {
        if (u4TestResult[i] != APOLLO_DMEM_PASS_RESULT)
            return 0;
    }
    }
    printf("[DMEM] Mix2 Test: 6, 16, 26, 34, 42, 50, 58, 68 Ignored!!\n");
    return 1;
}

UINT8 vApollo_dmem_mix3_tst(void)
{
    UINT32 i, u1CntRepeat = 0;
    UINT32 u4TestResult[APOLLO_DMEM_MIX3_CASE];

    vApollo_SetDram(APOLLO_DMEM_NORM_SBLK, u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR);
    vApollo_SetDram(APOLLO_DMEM_CMPT_SBLK, u4GetApolloWorkBufNonInit(1));
    vApollo_SetDram(APOLLO_DMEM_TEST_FINISH, APOLLO_DMEM_NG_RESULT);
    for(i = 0; i < APOLLO_DMEM_MIX3_CASE; i++)
        vApollo_SetDram(APOLLO_DMEM_TEST1_ADDR+i*4, APOLLO_DMEM_NG_RESULT);

    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0), (UINT32) &DMEM_MIX3_TST_TABLE[0], DMEM_MIX3_TST_SIZE); 
    vAproc_Enable();

    // wait for test done or timeout
    while((u4Apollo_ReadDram(APOLLO_DMEM_TEST_FINISH) != APOLLO_DMEM_PASS_RESULT)&&(u1CntRepeat<8))
    {
        x_thread_delay(500);
        u1CntRepeat++;
    }

    // check result
    for(i = 0; i < APOLLO_DMEM_MIX3_CASE; i++)
    {
        if(i<8)
        {
            u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*4);
        }
        else if(i<16)
        {
            u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+32+(i-8)*8);
            if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            {
                u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+32+(i-8)*8+4);
            }
        }
        else if(i<23)
        {
            u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+96+(i-16)*4);
        }
        else if(i<30)
        {
            u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+124+(i-23)*8);
            if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            {
                u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+124+(i-23)*8+4);
            }
        }
        else if(i<33)
        {
            u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+180+(i-30)*4);
        }
        else if(i<36)
        {
            u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+192+(i-33)*8);
            if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            {
                u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+192+(i-33)*8+4);
            }
        }
    }

    for(i = 0; i < APOLLO_DMEM_MIX3_CASE; i++)
    {
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
        {
            printf("[DMEM] Mix3 Test: case %d Pass \n", i+1);
        }
        else
        {
            printf("[DMEM] Mix3 Test: case %d Failed \n", i+1);
            printf("                       failed addr = 0x%x \n", u4TestResult[i]);
        }
    }

    for(i = 0; i < APOLLO_DMEM_MIX3_CASE; i++)
    {
        if (u4TestResult[i] != APOLLO_DMEM_PASS_RESULT)
            return 0;
    }
    return 1;
}

UINT8 vApollo_addr_conflict_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &ADDR_CONFLICT_TABLE[0], ADDR_CONFLICT_SIZE); 
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1)+0x2000,(UINT32) &ADDR_CONFLICT_DATA[0], ADDR_CONFLICT_DATA_SIZE);            	  
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    if(u4Apollo_ReadDram(APOLLO_DMEM_ADDR_CONFLICT_PAT_RESULT) ==APOLLO_DMEM_ADDR_CONFLICT_PASS_RESULT)  //get result
    {
        Printf("[ADDR_CONFLICT_PAT]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_DMEM_ADDR_CONFLICT_PAT_RESULT) ==APOLLO_DMEM_ADDR_CONFLICT_NG_RESULT)
    {
        Printf("[ADDR_CONFLICT_PAT]  NG , Result = 0x%x \n",u4Apollo_ReadDram(APOLLO_DMEM_ADDR_CONFLICT_TEST_RESULT));
        return 0;
    }
    else
    {
        Printf("[ADDR_CONFLICT_PAT]  ERR  ; The program may be dead!! \n" );
        return 0;
    }
    return 1;
}


UINT8 vApollo_read_after_write_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &READ_AFTER_WRITE_TABLE[0], READ_AFTER_WRITE_SIZE); 
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1),(UINT32) &READ_AFTER_WRITE_DATA[0], READ_AFTER_WRITE_DATA_SIZE);            	  
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    if(u4Apollo_ReadDram(APOLLO_DMEM_READ_AFTER_WRITE_PAT_RESULT) ==APOLLO_DMEM_READ_AFTER_WRITE_PASSRESULT)  //get result
    {
        Printf("[READ_AFTER_WRITE_PAT]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_DMEM_READ_AFTER_WRITE_PAT_RESULT) ==APOLLO_DMEM_READ_AFTER_WRITE_NGRESULT)
    {
        Printf("[READ_AFTER_WRITE_PAT]  NG , Result = 0x%x \n",u4Apollo_ReadDram(APOLLO_DMEM_READ_AFTER_WRITE_TEST_RESULT));
        return 0;
    }
    else
    {
        Printf("[READ_AFTER_WRITE_PAT]  ERR  ; The program may be dead!! \n" );
        return 0;
    }
    return 1;
}

UINT8 vApollo_sram_dirty_bit_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &SRAM_DIRTY_BIT_TABLE[0], SRAM_DIRTY_BIT_SIZE); 
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1)+0x2000,(UINT32) &SRAM_DIRTY_BIT_DATA[0], SRAM_DIRTY_BIT_DATA_SIZE);            	  
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    if(u4Apollo_ReadDram(APOLLO_DMEM_SRAM_DIRTY_BIT_PAT_RESULT) ==APOLLO_DMEM_SRAM_DIRTY_BIT_PASSRESULT)  //get result
    {
        Printf("[SRAM_DIRTY_BIT_PAT]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_DMEM_SRAM_DIRTY_BIT_PAT_RESULT) ==APOLLO_DMEM_SRAM_DIRTY_BIT_NGRESULT)
    {
        Printf("[SRAM_DIRTY_BIT_PAT]  NG , Result = 0x%x \n",u4Apollo_ReadDram(APOLLO_DMEM_SRAM_DIRTY_BIT_TEST_RESULT));
        return 0;
    }
    else
    {
        Printf("[SRAM_DIRTY_BIT_PAT]  ERR  ; The program may be dead!! \n" );
        return 0;
    }
    return 1;
}

UINT8 vApollo_compact_save_restore_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_CMP_SAVE_RESTORE_TABLE[0], APOLLO_CMP_SAVE_RESTORE); 
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1),(UINT32) &APOLLO_CMP_SAVE_RESTORE_DATA[0], APOLLO_CMP_SAVE_RESTORE_DATA_SIZE);
    //Set Compact start block to dram 0x20000(DSP view)
    vApollo_SetDram(APOLLO_DMEM_COMPACT_SAVE_RESTORE_ADD_SETTING, u4GetApolloWorkBufNonInit(1));
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);

    //DramResult:0x00020004
    if(u4Apollo_ReadDram(APOLLO_DMEM_COMPACT_SAVE_RESTORE_PAT_RESULT) ==APOLLO_DMEM_COMPACT_SAVE_RESTORE_PASSRESULT)  //get result
    {
        Printf("[COMPACT_SAVE_RESTORE]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_DMEM_COMPACT_SAVE_RESTORE_PAT_RESULT) ==APOLLO_DMEM_COMPACT_SAVE_RESTORE_NGRESULT)  //get result
    {
        Printf("[COMPACT_SAVE_RESTORE]  NG , Result = 0x%x \n",u4Apollo_ReadDram(APOLLO_DMEM_COMPACT_SAVE_RESTORE_TEST_RESULT));
        return 0;
    }
    else
    {
        Printf("[COMPACT_SAVE_RESTORE]  ERR  ; The program may be dead!! \n" );
        return 0;
    }

    return 1;
}

UINT8 vApollo_inst_A03_control_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INST_A03_TABLE[0], APOLLO_INST_A03_SIZE);        	  
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    if(u4Apollo_ReadDram(APOLLO_INST_PAT_RESULT) ==APOLLO_INST_PAT_RESULT_PASS)  //get result
    {
        Printf("[A03_control]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_INST_PAT_RESULT) ==APOLLO_INST_PAT_RESULT_FAIL)
    {
        Printf("[A03_control]  FAIL   \n");
        return 0;
    }
    else
    {
        Printf("[A03_control]  ERR  ; The program may be dead!! \n" );
        return 0;
    }
    return 1;
}

UINT8 vApollo_inst_A05_do_hs_ri_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INST_A05_TABLE[0], APOLLO_INST_A05_SIZE);  
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1)+0x22008,(UINT32) &APOLLO_INST_A05_TABLE[0], APOLLO_INST_A05_SIZE);   
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    if(u4Apollo_ReadDram(APOLLO_INST_PAT_RESULT) ==APOLLO_INST_PAT_RESULT_PASS)  //get result
    {
        Printf("[A05_do_hs_ri]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_INST_PAT_RESULT) ==APOLLO_INST_PAT_RESULT_FAIL)
    {
        Printf("[A05_do_hs_ri]  FAIL   \n");
        return 0;
    }
    else
    {
        Printf("[A05_do_hs_ri]  ERR  ; The program may be dead!! \n" );
        return 0;
    }
    return 1;
}


UINT8 vApollo_inst_A09_div_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INST_A09_TABLE[0], APOLLO_INST_A09_SIZE);        	  
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    if(u4Apollo_ReadDram(APOLLO_INST_PAT_RESULT) ==APOLLO_INST_PAT_RESULT_PASS)  //get result
    {
        Printf("[A09_div]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_INST_PAT_RESULT) ==APOLLO_INST_PAT_RESULT_FAIL)
    {
        Printf("[A09_div]  FAIL   \n");
        return 0;
    }
    else
    {
        Printf("[A09_div]  ERR  ; The program may be dead!! \n" );
        return 0;
    }
    return 1;
}


UINT8 vApollo_inst_C10_apollo_mul_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INST_C10_TABLE[0], APOLLO_INST_C10_SIZE);        	  
    vAproc_Enable();
    // sleep
    x_thread_delay(2000);
    
    if(u4Apollo_ReadDram(APOLLO_INST_PAT_RESULT) ==APOLLO_INST_PAT_RESULT_PASS)  //get result
    {
        Printf("[C10_apollo_mul]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_INST_PAT_RESULT) ==APOLLO_INST_PAT_RESULT_FAIL)
    {
        Printf("[C10_apollo_mul]  FAIL   \n");
        return 0;
    }
    else
    {
        Printf("[C10_apollo_mul]  ERR  ; The program may be dead!! \n" );
        return 0;
    }
    return 1;
}


UINT8 vApollo_inst_C11_apollo_satrnd_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INST_C11_TABLE[0], APOLLO_INST_C11_SIZE);        	  
    vAproc_Enable();
    // sleep
    x_thread_delay(2000);
    
    if(u4Apollo_ReadDram(APOLLO_INST_PAT_RESULT) ==APOLLO_INST_PAT_RESULT_PASS)  //get result
    {
        Printf("[C11_apollo_satrnd]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_INST_PAT_RESULT) ==APOLLO_INST_PAT_RESULT_FAIL)
    {
        Printf("[C11_apollo_satrnd]  FAIL   \n");
        return 0;
    }
    else
    {
        Printf("[C11_apollo_satrnd]  ERR  ; The program may be dead!! \n" );
        return 0;
    }
    return 1;
}

UINT8 vApollo_inst_C12_apollo_others_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INST_C12_TABLE[0], APOLLO_INST_C12_SIZE);        	  
    vAproc_Enable();
    // sleep
    x_thread_delay(5000);
    
    if(u4Apollo_ReadDram(APOLLO_INST_PAT_RESULT) ==APOLLO_INST_PAT_RESULT_PASS)  //get result
    {
        Printf("[C12_apollo_others]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_INST_PAT_RESULT) ==APOLLO_INST_PAT_RESULT_FAIL)
    {
        Printf("[C12_apollo_others]  FAIL   \n");
        return 0;
    }
    else
    {
        Printf("[C12_apollo_others]  ERR  ; The program may be dead!! \n" );
        return 0;
    }
    return 1;
}

void vApollo_smse_tst(void)
{
    UINT32 u4CntRepeat=0;
    
    vAproc_Disable();
    vApollo_SetDram(APOLLO_SMSE_PAT_RESULT, APOLLO_SMSE_PAT_RESULT_FAIL);
    vApollo_SetDram(APOLLO_SMSE_TEST_FINISH, APOLLO_SMSE_TEST_FINISH_NG);
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_LX_POST_TABLE[0], APOLLO_LX_POST_SIZE);    
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR+0x24000,(UINT32) &DATA_APOLLO_LX_POST_TABLE[0], DATA_APOLLO_LX_POST_SIZE);
    // note that the DRAM usage for this pattern is started from 0x24000, and it should be copy to 0x24000 by RISC first.  	  
    vAproc_Enable();
    // sleep
    x_thread_delay(5000);
    while((u4Apollo_ReadDram(APOLLO_SMSE_TEST_FINISH) != APOLLO_SMSE_TEST_FINISH_OK)&&(u4CntRepeat<8))
    {
        x_thread_delay(500);
        u4CntRepeat++;
    }
    if (u4CntRepeat==8) Printf("[LXX_POST] TEST time-out \n");
    
    if(u4Apollo_ReadDram(APOLLO_SMSE_PAT_RESULT) ==APOLLO_SMSE_PAT_RESULT_PASS)  //get result
    {
        Printf("[LXX_POST]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_SMSE_PAT_RESULT) ==APOLLO_SMSE_PAT_RESULT_FAIL)
    {
        Printf("[LXX_POST]  FAIL   \n");
    }
    else
    {
        Printf("[LXX_POST]  ERR  ; The program may be dead!! \n" );
    }   
}

UINT8 vApollo_getbs_function_tst(void)
{
    UINT32 i, u1CntRepeat = 0;
    UINT32 u4TestResult[APOLLO_GETBS_FUNCTION_CASE], u4data, u4afifo_start_addr;
    UINT32 test1_curptr[16] = {0,0,0,0x1,0x1,0x2,0x3,0x4,0x5,0x6,0x8,0x9,0xb,0xd,0xf,0x11};

    vApollo_SetDram(APOLLO_DMEM_NORM_SBLK, u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR);
    vApollo_SetDram(APOLLO_DMEM_CMPT_SBLK, u4GetApolloWorkBufNonInit(1));
    vApollo_SetDram(APOLLO_DMEM_TEST_FINISH, APOLLO_DMEM_NG_RESULT);
    u4afifo_start_addr = u4GetApolloWorkBufNonInit(1) + 0x2000;
    for(i = 0; i < 1184; i++)
        vApollo_SetDram(APOLLO_DMEM_TEST1_ADDR+i*4, APOLLO_DMEM_NG_RESULT);

    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0), (UINT32) &GETBS_FUNCTION_TABLE[0], GETBS_FUNCTION_SIZE); 
    vAproc_Enable();

    // wait for test done or timeout
    while((u4Apollo_ReadDram(APOLLO_DMEM_TEST_FINISH) != APOLLO_DMEM_PASS_RESULT)&&(u1CntRepeat<8))
    {
        x_thread_delay(1000);
        u1CntRepeat++;
    }
    // wait for test case 11 (this test will cause getbits hang)
    x_thread_delay(1000);
    
    // check result
    printf("[GETBS] Function Test 1: LSB, byte aligned, normal mode, zero extension \n");
    for(i = 0; i < 16; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[GETBS] case 1-%d Pass \n", i);
        else
        {
            printf("[GETBS] case 1-%d Failed \n", i);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x4);
            if (u4data == 1)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x8);
                printf("                       Get data = 0x%x \n", u4data);
            }
            else if (u4data == 2)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0xc);
                printf("                       APOLLO_MMR_GETBS_CUR_RD_POINT = 0x%x \n", u4data);
            }
            else if (u4data == 3)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x10);
                printf("                       GETBS_BIT_READ = 0x%x \n", u4data);
            }
            else if (u4data == 4)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x14);
                printf("                       GETBS_BYTE_BIT_READ = 0x%x \n", u4data);
            }
            break;
        }
    }

    printf("[GETBS] Function Test 2: MSB, byte aligned, normal mode, zero extension= 0 \n");
    for(i = 16; i < 32; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[GETBS] case 2-%d Pass \n", i-16);
        else
        {
            printf("[GETBS] case 2-%d Failed \n", i-16);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x4);
            if (u4data == 1)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x8);
                printf("                       Get data = 0x%x \n", u4data);
            }
            else if (u4data == 2)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0xc);
                printf("                       APOLLO_MMR_GETBS_CUR_RD_POINT = 0x%x \n", u4data);
            }
            else if (u4data == 3)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x10);
                printf("                       GETBS_BIT_READ = 0x%x \n", u4data);
            }
            else if (u4data == 4)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x14);
                printf("                       GETBS_BYTE_BIT_READ = 0x%x \n", u4data);
            }
            break;
        }
    }

    printf("[GETBS] Function Test 3: LSB, byte aligned, normal mode, zero extension, SHIFT0_GETBS \n");
    for(i = 32; i < 48; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[GETBS] case 3-%d Pass \n", i-32);
        else
        {
            printf("[GETBS] case 3-%d Failed \n", i-32);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x4);
            if (u4data == 1)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x8);
                printf("                       Get data = 0x%x \n", u4data);
            }
            else if (u4data == 2)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0xc);
                printf("                       APOLLO_MMR_GETBS_CUR_RD_POINT = 0x%x \n", u4data);
            }
            else if (u4data == 3)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x10);
                printf("                       GETBS_BIT_READ = 0x%x \n", u4data);
            }
            else if (u4data == 4)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x14);
                printf("                       GETBS_BYTE_BIT_READ = 0x%x \n", u4data);
            }
            break;
        }
    }

    printf("[GETBS] Function Test 4: LSB, half aligned, normal mode, zero extension \n");
    for(i = 48; i < 64; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[GETBS] case 4-%d Pass \n", i-48);
        else
        {
            printf("[GETBS] case 4-%d Failed \n", i-48);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x4);
            if (u4data == 1)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x8);
                printf("                       Get data = 0x%x \n", u4data);
            }
            else if (u4data == 2)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0xc);
                printf("                       APOLLO_MMR_GETBS_CUR_RD_POINT = 0x%x \n", u4data);
            }
            else if (u4data == 3)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x10);
                printf("                       GETBS_BIT_READ = 0x%x \n", u4data);
            }
            else if (u4data == 4)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x14);
                printf("                       GETBS_BYTE_BIT_READ = 0x%x \n", u4data);
            }
            break;
        }
    }

    printf("[GETBS] Function Test 5: LSB, word aligned, normal mode, zero extension \n");
    for(i = 64; i < 80; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[GETBS] case 5-%d Pass \n", i-64);
        else
        {
            printf("[GETBS] case 5-%d Failed \n", i-64);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x4);
            if (u4data == 1)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x8);
                printf("                       Get data = 0x%x \n", u4data);
            }
            else if (u4data == 2)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0xc);
                printf("                       APOLLO_MMR_GETBS_CUR_RD_POINT = 0x%x \n", u4data);
            }
            else if (u4data == 3)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x10);
                printf("                       GETBS_BIT_READ = 0x%x \n", u4data);
            }
            else if (u4data == 4)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x14);
                printf("                       GETBS_BYTE_BIT_READ = 0x%x \n", u4data);
            }
            break;
        }
    }

    printf("[GETBS] Function Test 6: LSB, byte aligned, vorbis mode, zero extension \n");
    for(i = 80; i < 96; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[GETBS] case 6-%d Pass \n", i-80);
        else
        {
            printf("[GETBS] case 6-%d Failed \n", i-80);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x4);
            if (u4data == 1)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x8);
                printf("                       Get data = 0x%x \n", u4data);
            }
            else if (u4data == 2)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0xc);
                printf("                       APOLLO_MMR_GETBS_CUR_RD_POINT = 0x%x \n", u4data);
            }
            else if (u4data == 3)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x10);
                printf("                       GETBS_BIT_READ = 0x%x \n", u4data);
            }
            else if (u4data == 4)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x14);
                printf("                       GETBS_BYTE_BIT_READ = 0x%x \n", u4data);
            }
            break;
        }
    }

    printf("[GETBS] Function Test 7: LSB, byte aligned, DTSCD mode, zero extension \n");
    for(i = 96; i < 112; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[GETBS] case 7-%d Pass \n", i-96);
        else
        {
            printf("[GETBS] case 7-%d Failed \n", i-96);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x4);
            if (u4data == 1)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x8);
                printf("                       Get data = 0x%x \n", u4data);
            }
            else if (u4data == 2)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0xc);
                printf("                       APOLLO_MMR_GETBS_CUR_RD_POINT = 0x%x \n", u4data);
            }
            else if (u4data == 3)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x10);
                printf("                       GETBS_BIT_READ = 0x%x \n", u4data);
            }
            else if (u4data == 4)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x14);
                printf("                       GETBS_BYTE_BIT_READ = 0x%x \n", u4data);
            }
            break;
        }
    }

    printf("[GETBS] Function Test 8: LSB, byte aligned, normal mode, signed extension \n");
    for(i = 112; i < 128; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[GETBS] case 8-%d Pass \n", i-112);
        else
        {
            printf("[GETBS] case 8-%d Failed \n", i-112);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x4);
            if (u4data == 1)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x8);
                printf("                       Get data = 0x%x \n", u4data);
            }
            else if (u4data == 2)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0xc);
                printf("                       APOLLO_MMR_GETBS_CUR_RD_POINT = 0x%x \n", u4data);
            }
            else if (u4data == 3)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x10);
                printf("                       GETBS_BIT_READ = 0x%x \n", u4data);
            }
            else if (u4data == 4)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x14);
                printf("                       GETBS_BYTE_BIT_READ = 0x%x \n", u4data);
            }
            break;
        }
    }

    printf("[GETBS] Function Test 9: LSB, byte aligned, normal mode, zero extension, circular buffer \n");
    for(i = 128; i < 144; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[GETBS] case 9-%d Pass \n", i-128);
        else
        {
            printf("[GETBS] case 9-%d Failed \n", i-128);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x4);
            if (u4data == 1)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x8);
                printf("                       Get data = 0x%x \n", u4data);
            }
            else if (u4data == 2)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0xc);
                printf("                       APOLLO_MMR_GETBS_CUR_RD_POINT = 0x%x \n", u4data);
            }
            else if (u4data == 3)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x10);
                printf("                       GETBS_BIT_READ = 0x%x \n", u4data);
            }
            else if (u4data == 4)
            {
                u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*32+0x14);
                printf("                       GETBS_BYTE_BIT_READ = 0x%x \n", u4data);
            }
            break;
        }
    }

    printf("[GETBS] Function Test 10: don't check CHK_ABUF_WPTR \n");
    for(i = 144; i < 160; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+144*32+(i-144)*4);
        if(u4TestResult[i] == (u4afifo_start_addr + test1_curptr[i-144]))
        {
            u4TestResult[i] = APOLLO_DMEM_PASS_RESULT;
            printf("[GETBS] case 10-%d Pass \n", i-144);
        }
        else
        {
            u4TestResult[i] = APOLLO_DMEM_NG_RESULT;
            printf("[GETBS] case 10-%d Failed \n", i-144);
            break;
        }
    }

    printf("[GETBS] Function Test 11: check CHK_ABUF_WPTR \n");
    for(i = 160; i < 167; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+144*32+16*4+(i-160)*4);
        if(u4TestResult[i] == (u4afifo_start_addr + test1_curptr[i-160]))
        {
            u4TestResult[i] = APOLLO_DMEM_PASS_RESULT;
            printf("[GETBS] case 11-%d Pass \n", i-160);
        }
        else
        {
            u4TestResult[i] = APOLLO_DMEM_NG_RESULT;
            printf("[GETBS] case 11-%d Failed \n", i-160);
            break;
        }
    }
    for(i = 167; i < APOLLO_GETBS_FUNCTION_CASE; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+144*32+16*4+7*4+(i-167)*4);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[GETBS] case 11-%d Pass \n", i-167);
        else
        {
            printf("[GETBS] case 11-%d Failed \n", i-167);
            break;
        }
    }

    for(i = 0; i < APOLLO_GETBS_FUNCTION_CASE; i++)
    {
        if (u4TestResult[i] != APOLLO_DMEM_PASS_RESULT)
            return 0;
    }
    return 1;
}

UINT8 vApollo_getbs_crc_tst(void)
{
    UINT32 i, u1CntRepeat = 0;
    UINT32 u4TestResult[APOLLO_GETBS_CRC_CASE], u4data1, u4data2;

    vApollo_SetDram(APOLLO_DMEM_NORM_SBLK, u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR);
    vApollo_SetDram(APOLLO_DMEM_CMPT_SBLK, u4GetApolloWorkBufNonInit(1));
    vApollo_SetDram(APOLLO_DMEM_TEST_FINISH, APOLLO_DMEM_NG_RESULT);
    for(i = 0; i < APOLLO_GETBS_CRC_CASE*2; i++)
        vApollo_SetDram(APOLLO_DMEM_TEST1_ADDR+i*4, APOLLO_DMEM_NG_RESULT);

    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0), (UINT32) &GETBS_CRC_TABLE[0], GETBS_CRC_SIZE); 
    vAproc_Enable();

    // wait for test done or timeout
    while((u4Apollo_ReadDram(APOLLO_DMEM_TEST_FINISH) != APOLLO_DMEM_PASS_RESULT)&&(u1CntRepeat<8))
    {
        x_thread_delay(500);
        u1CntRepeat++;
    }
    
    // check result
    printf("[GETBS] CRC Test 1: AC3 \n");
    u4TestResult[0] = APOLLO_DMEM_PASS_RESULT;
    u4data1 = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR);
    u4data2 = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+4);
    if((u4data1 == APOLLO_DMEM_PASS_RESULT) && (u4data2 == APOLLO_DMEM_PASS_RESULT))
    {
        printf("[GETBS] AC3 Pass \n");
    }
    else
    {
        printf("[GETBS] AC3 Failed \n");
        printf("                    APOLLO_MMR_GETBS_CUR_RD_POINT = 0x%x \n", u4data1);
        printf("                    CRC Result = 0x%x \n", u4data2);
        u4TestResult[0] = APOLLO_DMEM_NG_RESULT;
    }

    printf("[GETBS] CRC Test 2: MPEG \n");
    u4TestResult[1] = APOLLO_DMEM_PASS_RESULT;
    u4data1 = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+8);
    u4data2 = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+0xc);
    if((u4data1 == APOLLO_DMEM_PASS_RESULT) && (u4data2 == APOLLO_DMEM_PASS_RESULT))
    {
        printf("[GETBS] MPEG Pass \n");
    }
    else
    {
        printf("[GETBS] MPEG Failed \n");
        printf("                      APOLLO_MMR_GETBS_CUR_RD_POINT = 0x%x \n", u4data1);
        printf("                      CRC Result = 0x%x \n", u4data2);
        u4TestResult[1] = APOLLO_DMEM_NG_RESULT;
    }

    printf("[GETBS] CRC Test 3: DTS \n");
    u4TestResult[2] = APOLLO_DMEM_PASS_RESULT;
    u4data1 = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+0x10);
    u4data2 = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+0x14);
    if((u4data1 == APOLLO_DMEM_PASS_RESULT) && (u4data2 == APOLLO_DMEM_PASS_RESULT))
    {
        printf("[GETBS] DTS Pass \n");
    }
    else
    {
        printf("[GETBS] DTS Failed \n");
        printf("                    APOLLO_MMR_GETBS_CUR_RD_POINT = 0x%x \n", u4data1);
        printf("                    CRC Result = 0x%x \n", u4data2);
        u4TestResult[2] = APOLLO_DMEM_NG_RESULT;
    }

    printf("[GETBS] CRC Test 4: MLP \n");
    u4TestResult[3] = APOLLO_DMEM_PASS_RESULT;
    u4data1 = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+0x18);
    u4data2 = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+0x1C);
    if((u4data1 == APOLLO_DMEM_PASS_RESULT) && (u4data2 == APOLLO_DMEM_PASS_RESULT))
    {
        printf("[GETBS] MLP Pass \n");
    }
    else
    {
        printf("[GETBS] MLP Failed \n");
        printf("                   APOLLO_MMR_GETBS_CUR_RD_POINT = 0x%x \n", u4data1);
        printf("                   CRC Result = 0x%x \n", u4data2);
        u4TestResult[3] = APOLLO_DMEM_NG_RESULT;
    }

    printf("[GETBS] CRC Test 5: write CRC result \n");
    u4TestResult[4] = APOLLO_DMEM_PASS_RESULT;
    u4data1 = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+0x20);
    u4data2 = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+0x24);
    if((u4data1 == APOLLO_DMEM_PASS_RESULT) && (u4data2 == APOLLO_DMEM_PASS_RESULT))
    {
        printf("[GETBS] write CRC result Pass \n");
    }
    else
    {
        printf("[GETBS] write CRC result Failed \n");
        printf("                   APOLLO_MMR_GETBS_CUR_RD_POINT = 0x%x \n", u4data1);
        printf("                   CRC Result = 0x%x \n", u4data2);
        u4TestResult[4] = APOLLO_DMEM_NG_RESULT;
    }

    for(i = 0; i < APOLLO_GETBS_CRC_CASE; i++)
    {
        if (u4TestResult[i] != APOLLO_DMEM_PASS_RESULT)
            return 0;
    }
    return 1;
}


UINT8 vApollo_getbs_cnt_ldz_tst(void)
{
    UINT32 i, u1CntRepeat = 0;
    UINT32 u4TestResult[APOLLO_GETBS_CNT_LDZ_CASE], u4data;

    vApollo_SetDram(APOLLO_DMEM_NORM_SBLK, u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR);
    vApollo_SetDram(APOLLO_DMEM_CMPT_SBLK, u4GetApolloWorkBufNonInit(1));
    vApollo_SetDram(APOLLO_DMEM_TEST_FINISH, APOLLO_DMEM_NG_RESULT);
    for(i = 0; i < APOLLO_GETBS_CNT_LDZ_CASE*4; i++)
        vApollo_SetDram(APOLLO_DMEM_TEST1_ADDR+i*4, APOLLO_DMEM_NG_RESULT);

    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0), (UINT32) &GETBS_CNT_LDZ_TABLE[0], GETBS_CNT_LDZ_SIZE); 
    vAproc_Enable();

    // wait for test done or timeout
    while((u4Apollo_ReadDram(APOLLO_DMEM_TEST_FINISH) != APOLLO_DMEM_PASS_RESULT)&&(u1CntRepeat<8))
    {
        x_thread_delay(500);
        u1CntRepeat++;
    }
    
    // check result
    printf("[GETBS] Count Leading Zero Test 1: LZ_SHFT_ENDONE = 0; LZ_NUM_ZSTART = 1 \n");
    for(i = 0; i < 32; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*16+12);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[GETBS] case 1-%d Pass \n", i);
        else
        {
            printf("[GETBS] case 1-%d Failed \n", i);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*16);
            printf("                       GETBS_LZ_AMOUNT = 0x%x \n", u4data);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*16+12);
            printf("                       APOLLO_MMR_GETBS_CUR_RD_POINT = 0x%x \n", u4data);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*16+8);
            printf("                       GETBS_BYTE_BIT_READ = 0x%x \n", u4data);
        }
    }

    printf("[GETBS] Count Leading Zero Test 2: LZ_SHFT_ENDONE = 0; LZ_NUM_ZSTART = 0 \n");
    for(i = 32; i < 40; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*16+12);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[GETBS] case 2-%d Pass \n", i-32);
        else
        {
            printf("[GETBS] case 2-%d Failed \n", i-32);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*16);
            printf("                       GETBS_LZ_AMOUNT = 0x%x \n", u4data);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*16+12);
            printf("                       APOLLO_MMR_GETBS_CUR_RD_POINT = 0x%x \n", u4data);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*16+8);
            printf("                       GETBS_BYTE_BIT_READ = 0x%x \n", u4data);
        }
    }

    printf("[GETBS] Count Leading Zero Test 3: LZ_SHFT_ENDONE = 1; LZ_NUM_ZSTART = 1 \n");
    for(i = 40; i < 72; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*16+12);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[GETBS] case 3-%d Pass \n", i-40);
        else
        {
            printf("[GETBS] case 3-%d Failed \n", i-40);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*16);
            printf("                       GETBS_LZ_AMOUNT = 0x%x \n", u4data);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*16+12);
            printf("                       APOLLO_MMR_GETBS_CUR_RD_POINT = 0x%x \n", u4data);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*16+8);
            printf("                       GETBS_BYTE_BIT_READ = 0x%x \n", u4data);
        }
    }

    printf("[GETBS] Count Leading Zero Test 4: LZ_SHFT_ENDONE = 1; LZ_NUM_ZSTART = 0 \n");
    for(i = 72; i < APOLLO_GETBS_CNT_LDZ_CASE; i++)
    {
        u4TestResult[i] = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*16+12);
        if(u4TestResult[i] == APOLLO_DMEM_PASS_RESULT)
            printf("[GETBS] case 4-%d Pass \n", i-72);
        else
        {
            printf("[GETBS] case 4-%d Failed \n", i-72);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*16);
            printf("                       GETBS_LZ_AMOUNT = 0x%x \n", u4data);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*16+12);
            printf("                       APOLLO_MMR_GETBS_CUR_RD_POINT = 0x%x \n", u4data);
            u4data = u4Apollo_ReadDram(APOLLO_DMEM_TEST1_ADDR+i*16+8);
            printf("                       GETBS_BYTE_BIT_READ = 0x%x \n", u4data);
        }
    }

    for(i = 0; i < APOLLO_GETBS_CNT_LDZ_CASE; i++)
    {
        if (u4TestResult[i] != APOLLO_DMEM_PASS_RESULT)
            return 0;
    }
    return 1;
}

UINT8 vApollo_psrdma_tst(void)
{
    UINT32 i, u1CntRepeat = 0;
    UINT32 u4TestResult[APOLLO_PSRDMA_CASE], u4data1, u4data2, u4data3;

    u4TestResult[0] = APOLLO_DMEM_PASS_RESULT;
    u4TestResult[1] = APOLLO_DMEM_PASS_RESULT;

    vApollo_SetDram(APOLLO_DMEM_NORM_SBLK, u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR);
    vApollo_SetDram(APOLLO_DMEM_CMPT_SBLK, u4GetApolloWorkBufNonInit(1));
    vApollo_SetDram(APOLLO_DMEM_TEST_FINISH, APOLLO_DMEM_NG_RESULT);

    u4data1 = u4GetAFIFOStart(0);
    vApollo_SetDram(APOLLO_DMEM_AFIFO_SBLK, u4data1);
    u4data2 = u4GetAFIFOEnd(0);
    vApollo_SetDram(APOLLO_DMEM_AFIFO_EBLK, u4data2);

    // set pattern to source buffer
    for(i=0;i<0x100;i++)
    {
        vApollo_SetDspDram(u4data1-0x1000+i*4, i);
    }
    for(i=0x100;i<0x200;i++)
    {
        vApollo_SetDspDram(u4data1-0x1000+i*4, (i-0x100)<<8);
    }
    for(i=0x200;i<0x300;i++)
    {
        vApollo_SetDspDram(u4data1-0x1000+i*4, (i-0x200)<<16);
    }
    for(i=0x300;i<0x380;i++)
    {
        vApollo_SetDspDram(u4data1-0x1000+i*4, (i-0x300)<<24);
    }

    for(i = 0; i < APOLLO_PSRDMA_CASE*2; i++)
        vApollo_SetDram(APOLLO_DMEM_TEST1_ADDR+i*4, APOLLO_DMEM_NG_RESULT);

    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0), (UINT32) &PARSER_DMA_TABLE[0], PARSER_DMA_SIZE); 
    vAproc_Enable();

    // wait for test done or timeout
    while((u4Apollo_ReadDram(APOLLO_DMEM_TEST_FINISH) != APOLLO_DMEM_PASS_RESULT)&&(u1CntRepeat<8))
    {
        x_thread_delay(500);
        u1CntRepeat++;
    }
    
    // check result
    // Test 1: source AFIFO start-0x1000 -> dest AFIFO start+0x1000; length 0x1000 bytes
    for(i=0;i<0x100;i++)
    {
        u4data3 = u4Apollo_ReadDspDram(u4data1+0x1000+i*4);
        if(u4data3 != i)
        {
            u4TestResult[0] = APOLLO_DMEM_NG_RESULT;
            printf("[PSRDMA] Test 1: Failed \n");
            printf("                    Failed addr = 0x%x \n", u4data1+0x1000+i*4);
            printf("                    Failed data = 0x%x \n", u4data3);
            break;
        }
    }
    for(i=0x100;i<0x200;i++)
    {
        u4data3 = u4Apollo_ReadDspDram(u4data1+0x1000+i*4);
        if(u4data3 != ((i-0x100)<<8))
        {
            u4TestResult[0] = APOLLO_DMEM_NG_RESULT;
            printf("[PSRDMA] Test 1: Failed \n");
            printf("                    Failed addr = 0x%x \n", u4data1+0x1000+i*4);
            printf("                    Failed data = 0x%x \n", u4data3);
            break;
        }
    }
    for(i=0x200;i<0x300;i++)
    {
        u4data3 = u4Apollo_ReadDspDram(u4data1+0x1000+i*4);
        if(u4data3 != ((i-0x200)<<16))
        {
            u4TestResult[0] = APOLLO_DMEM_NG_RESULT;
            printf("[PSRDMA] Test 1: Failed \n");
            printf("                    Failed addr = 0x%x \n", u4data1+0x1000+i*4);
            printf("                    Failed data = 0x%x \n", u4data3);
            break;
        }
    }
    for(i=0x300;i<0x380;i++)
    {
        u4data3 = u4Apollo_ReadDspDram(u4data1+0x1000+i*4);
        if(u4data3 != ((i-0x300)<<24))
        {
            u4TestResult[0] = APOLLO_DMEM_NG_RESULT;
            printf("[PSRDMA] Test 1: Failed \n");
            printf("                    Failed addr = 0x%x \n", u4data1+0x1000+i*4);
            printf("                    Failed data = 0x%x \n", u4data3);
            break;
        }
    }

    if(u4TestResult[0] ==APOLLO_DMEM_PASS_RESULT )
        printf("[PSRDMA] Test 1: Pass \n");

    // Test 2: source AFIFO start-0x1000 -> dest AFIFO end-0x400; length 0x1000 bytes
    for(i=0;i<0x100;i++)
    {
        u4data3 = u4Apollo_ReadDspDram(u4data2-0x400+i*4);
        if(u4data3 != i)
        {
            u4TestResult[1] = APOLLO_DMEM_NG_RESULT;
            printf("[PSRDMA] Test 2: Failed \n");
            printf("                    Failed addr = 0x%x \n", u4data2-0x400+i*4);
            printf("                    Failed data = 0x%x \n", u4data3);
            break;
        }
    }
    for(i=0x100;i<0x200;i++)
    {
        u4data3 = u4Apollo_ReadDspDram(u4data1+(i-0x100)*4);
        if(u4data3 != ((i-0x100)<<8))
        {
            u4TestResult[1] = APOLLO_DMEM_NG_RESULT;
            printf("[PSRDMA] Test 2: Failed \n");
            printf("                    Failed addr = 0x%x \n", u4data1+(i-0x100)*4);
            printf("                    Failed data = 0x%x \n", u4data3);
            break;
        }
    }
    for(i=0x200;i<0x300;i++)
    {
        u4data3 = u4Apollo_ReadDspDram(u4data1+(i-0x100)*4);
        if(u4data3 != ((i-0x200)<<16))
        {
            u4TestResult[1] = APOLLO_DMEM_NG_RESULT;
            printf("[PSRDMA] Test 2: Failed \n");
            printf("                    Failed addr = 0x%x \n", u4data1+(i-0x100)*4);
            printf("                    Failed data = 0x%x \n", u4data3);
            break;
        }
    }
    for(i=0x300;i<0x380;i++)
    {
        u4data3 = u4Apollo_ReadDspDram(u4data1+(i-0x100)*4);
        if(u4data3 != ((i-0x300)<<24))
        {
            u4TestResult[1] = APOLLO_DMEM_NG_RESULT;
            printf("[PSRDMA] Test 2: Failed \n");
            printf("                    Failed addr = 0x%x \n", u4data1+(i-0x100)*4);
            printf("                    Failed data = 0x%x \n", u4data3);
            break;
        }
    }

    if(u4TestResult[1] ==APOLLO_DMEM_PASS_RESULT )
        printf("[PSRDMA] Test 2: Pass \n");

    for(i = 0; i < APOLLO_PSRDMA_CASE; i++)
    {
        if (u4TestResult[i] != APOLLO_DMEM_PASS_RESULT)
            return 0;
    }
    return 1;
}

//RISC <-> Apollo interrupt test
//Get Apollo -> Risc interrupt bit0, data0~data15, and write to dram test result buffer
void vApollo_INT_RISC_Apollo_get_bit0Int_round1(void)
{
    UINT32  data;
	
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA0); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND1,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA1); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND1+0x4,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA2); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND1+0x8,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA3); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND1+0xc,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA4); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND1+0x10,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA5); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND1+0x14,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA6); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND1+0x18,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA7); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND1+0x1c,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA8); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND1+0x20,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA9); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND1+0x24,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA10); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND1+0x28,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA11); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND1+0x2c,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA12); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND1+0x30,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA13); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND1+0x34,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA14); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND1+0x38,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA15); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND1+0x3c,data);
}

//Get Apollo -> Risc interrupt bit1, data0~data15, and write to dram test result buffer
void vApollo_INT_RISC_Apollo_get_bit1Int_round1(void)
{
    UINT32  data;
	
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA0); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND1,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA1); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND1+0x4,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA2); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND1+0x8,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA3); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND1+0xc,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA4); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND1+0x10,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA5); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND1+0x14,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA6); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND1+0x18,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA7); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND1+0x1c,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA8); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND1+0x20,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA9); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND1+0x24,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA10); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND1+0x28,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA11); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND1+0x2c,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA12); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND1+0x30,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA13); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND1+0x34,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA14); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND1+0x38,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA15); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND1+0x3c,data);
}

//Get Apollo -> Risc interrupt bit2, data0~data15, and write to dram test result buffer
void vApollo_INT_RISC_Apollo_get_bit2Int_round1(void)
{
    UINT32  data;
	
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA0); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND1,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA1); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND1+0x4,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA2); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND1+0x8,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA3); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND1+0xc,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA4); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND1+0x10,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA5); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND1+0x14,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA6); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND1+0x18,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA7); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND1+0x1c,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA8); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND1+0x20,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA9); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND1+0x24,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA10); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND1+0x28,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA11); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND1+0x2c,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA12); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND1+0x30,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA13); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND1+0x34,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA14); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND1+0x38,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA15); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND1+0x3c,data);
}

//Get Apollo -> Risc interrupt bit3, data0~data15, and write to dram test result buffer
void vApollo_INT_RISC_Apollo_get_bit3Int_round1(void)
{
    UINT32  data;
	
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA0); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND1,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA1); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND1+0x4,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA2); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND1+0x8,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA3); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND1+0xc,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA4); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND1+0x10,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA5); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND1+0x14,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA6); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND1+0x18,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA7); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND1+0x1c,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA8); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND1+0x20,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA9); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND1+0x24,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA10); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND1+0x28,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA11); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND1+0x2c,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA12); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND1+0x30,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA13); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND1+0x34,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA14); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND1+0x38,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA15); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND1+0x3c,data);

		vApollo_SetDram(APOLLO_RISC2APOLLO_FINISH_IDENTIFY_ROUND1,0x8888);//identify to Apollo
}

void vApollo_INT_RISC_Apollo_get_bit0Int_round2(void)
{
    UINT32  data;
	
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA0); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND2,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA1); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND2+0x4,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA2); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND2+0x8,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA3); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND2+0xc,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA4); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND2+0x10,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA5); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND2+0x14,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA6); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND2+0x18,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA7); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND2+0x1c,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA8); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND2+0x20,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA9); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND2+0x24,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA10); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND2+0x28,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA11); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND2+0x2c,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA12); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND2+0x30,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA13); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND2+0x34,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA14); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND2+0x38,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA15); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_1_ROUND2+0x3c,data);
}

//Get Apollo -> Risc interrupt bit1, data0~data15, and write to dram test result buffer
void vApollo_INT_RISC_Apollo_get_bit1Int_round2(void)
{
    UINT32  data;
	
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA0); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND2,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA1); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND2+0x4,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA2); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND2+0x8,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA3); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND2+0xc,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA4); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND2+0x10,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA5); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND2+0x14,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA6); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND2+0x18,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA7); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND2+0x1c,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA8); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND2+0x20,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA9); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND2+0x24,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA10); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND2+0x28,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA11); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND2+0x2c,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA12); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND2+0x30,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA13); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND2+0x34,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA14); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND2+0x38,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA15); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_2_ROUND2+0x3c,data);
}

//Get Apollo -> Risc interrupt bit2, data0~data15, and write to dram test result buffer
void vApollo_INT_RISC_Apollo_get_bit2Int_round2(void)
{
    UINT32  data;
	
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA0); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND2,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA1); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND2+0x4,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA2); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND2+0x8,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA3); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND2+0xc,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA4); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND2+0x10,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA5); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND2+0x14,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA6); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND2+0x18,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA7); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND2+0x1c,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA8); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND2+0x20,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA9); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND2+0x24,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA10); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND2+0x28,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA11); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND2+0x2c,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA12); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND2+0x30,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA13); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND2+0x34,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA14); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND2+0x38,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA15); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_3_ROUND2+0x3c,data);
}

//Get Apollo -> Risc interrupt bit3, data0~data15, and write to dram test result buffer
void vApollo_INT_RISC_Apollo_get_bit3Int_round2(void)
{
    UINT32  data;
	
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA0); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND2,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA1); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND2+0x4,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA2); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND2+0x8,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA3); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND2+0xc,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA4); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND2+0x10,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA5); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND2+0x14,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA6); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND2+0x18,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA7); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND2+0x1c,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA8); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND2+0x20,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA9); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND2+0x24,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA10); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND2+0x28,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA11); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND2+0x2c,data);
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA12); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND2+0x30,data); 
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA13); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND2+0x34,data);
        data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA14); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND2+0x38,data); 
		data=APOLLO_READ32(APOLLO_RREG_AP2RC_EXTDATA15); 
		vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_ADDR_4_ROUND2+0x3c,data);

		vApollo_SetDram(APOLLO_RISC2APOLLO_FINISH_IDENTIFY_ROUND2,0x8888);//identify to Apollo
}




extern UINT32 _u4Apollo2RiscRountCount;

UINT8 vApollo_INT_RISC_Apollo_test(void)
{
    Printf("\nEmu 019-[%d] RISC <-> Apollo interrupt test ....\n", __TIME__);

    //initial  Apollo Dram    
	vApollo_SetDram(APOLLO_RISC2APOLLO_PAT_RESULT_FINAL_ROUND1,0xffff);  //initial APOLLO_RISC2APOLLO_PAT_RESULT_FINAL_ROUND1=0xffff
	vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_FINAL_ROUND1,0xffff);  //initial APOLLO_APOLLO2RISC_PAT_RESULT_FINAL_ROUND1=0xffff
	vApollo_SetDram(APOLLO_RISC2APOLLO_PAT_RESULT_FINAL_ROUND2,0xffff);  //initial APOLLO_RISC2APOLLO_PAT_RESULT_FINAL_ROUND2=0xffff
	vApollo_SetDram(APOLLO_APOLLO2RISC_PAT_RESULT_FINAL_ROUND2,0xffff);  //initial APOLLO_APOLLO2RISC_PAT_RESULT_FINAL_ROUND2=0xffff



    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INT_RiscApollo_TEST_TABLE[0], APOLLO_INT_RiscApollo_TEST_SIZE); 
    vAproc_Enable();

    // sleep
    x_thread_delay(1000);

    //Risc  <-> Apollo interrupt test Round 1
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_INTRVEC,0x0);         //reset APOLLO_RREG_RC2AP_INTRVEC=0x0
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA0,0x11111111); //data0
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA1,0x22222222); //data1
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA2,0x33333333); //data2
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA3,0x44444444); //data3
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA4,0x55555555); //data4
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA5,0x66666666); //data5
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA6,0x77777777); //data6
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA7,0x88888888); //data7
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA8,0x99999999); //data8
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA9,0xaaaaaaaa); //data9
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA10,0xbbbbbbbb); //data10
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA11,0xcccccccc); //data11
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA12,0xdddddddd); //data12
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA13,0xeeeeeeee); //data13
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA14,0xffffffff); //data14
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA15,0x12121212); //data15
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_INTRVEC,0xf);         //set APOLLO_RREG_RC2AP_INTRVEC=0xf, send 4 interrupt


    // sleep, waiting for Apollo get the interrupt from Risc and compare with golden buffer
    // if compare result if pass, then Apollo will send interrupt to Risc for test Apollo -> Risc interrupt
    x_thread_delay(1000);

	if ((u4Apollo_ReadDram(APOLLO_RISC2APOLLO_PAT_RESULT_FINAL_ROUND1) == 0x8888) && (u4Apollo_ReadDram(APOLLO_APOLLO2RISC_PAT_RESULT_FINAL_ROUND1) == 0x8888))
	{
        Printf("\n Verify OK for Round 1 !\n");  		 
	}	
	else
	{
	    Printf("\n Verify Fail for Round 1 !\n");   		
		return 0;
	}


	//Risc  <-> Apollo interrupt test Round 2
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_INTRVEC,0x0);         //reset APOLLO_RREG_RC2AP_INTRVEC=0x0
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA0,0x01010101); //data0
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA1,0x12121212); //data1
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA2,0x23232323); //data2
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA3,0x34343434); //data3
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA4,0x45454545); //data4
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA5,0x56565656); //data5
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA6,0x67676767); //data6
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA7,0x78787878); //data7
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA8,0x89898989); //data8
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA9,0x9a9a9a9a); //data9
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA10,0xabababab); //data10
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA11,0xbcbcbcbc); //data11
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA12,0xcdcdcdcd); //data12
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA13,0xdededede); //data13
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA14,0xefefefef); //data14
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_EXTDATA15,0xf0f0f0f0); //data15
    APOLLO_WRITE32(APOLLO_RREG_RC2AP_INTRVEC,0xf);         //set APOLLO_RREG_RC2AP_INTRVEC=0xf, send 4 interrupt


    // sleep, waiting for Apollo get the interrupt from Risc and compare with golden buffer
    // if compare result if pass, then Apollo will send interrupt to Risc for test Apollo -> Risc interrupt
    x_thread_delay(1000);

	if ((u4Apollo_ReadDram(APOLLO_RISC2APOLLO_PAT_RESULT_FINAL_ROUND2) == 0x8888) && (u4Apollo_ReadDram(APOLLO_APOLLO2RISC_PAT_RESULT_FINAL_ROUND2) == 0x8888))
	{
        Printf("\n Verify OK for Round 2 !\n");  	
		return 1;
	}	
	else
	{
	    Printf("\n Verify Fail for Round 2 !\n");   
		return 0;
	} 
	
	
		                      
}

UINT8 vApollo_INT_DSP_Apollo_test(void)
{
    Printf("\nEmu 020-[%d] DSP <-> Apollo interrupt test ....\n", __TIME__);

    //initial  Apollo Dram & DSP sram
    //set _u4Round=1
	vApollo_SetDram(APOLLO_DSP_APOLLO_PAT_ROUND_NUM,0x1);  //initial APOLLO_DSP_APOLLO_PAT_ROUND_NUM=0x1
    vApollo_SetDram(APOLLO_DSP2APOLLO_PAT_RESULT_FINAL_ROUND1,0xffff);  //initial APOLLO_DSP2APOLLO_PAT_RESULT_FINAL_ROUND1=0xffff
    vApollo_SetDram(APOLLO_DSP2APOLLO_PAT_RESULT_FINAL_ROUND2,0xffff);  //initial APOLLO_DSP2APOLLO_PAT_RESULT_FINAL_ROUND2=0xffff
    vWriteDspSram(APOLLO_APOLLO2DSP_PAT_RESULT_FINAL_ROUND1, 0xffff);   //initial APOLLO_APOLLO2DSP_PAT_RESULT_FINAL_ROUND1=0xffff
    vWriteDspSram(APOLLO_APOLLO2DSP_PAT_RESULT_FINAL_ROUND2, 0xffff);   //initial APOLLO_APOLLO2DSP_PAT_RESULT_FINAL_ROUND2=0xffff

	vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INT_DSPApollo_TEST_TABLE[0], APOLLO_INT_DSPApollo_TEST_SIZE); 
    vAproc_Enable();

    
    x_thread_delay(1000);
    //enable DSP pattern 
	CLI_Parser("aud.dsp.pat 31");

	   

    // sleep, waiting for Apollo get the interrupt from DSP and compare with golden buffer
    // if compare result if pass, then Apollo will send interrupt to DSP for test Apollo -> DSP interrupt
    x_thread_delay(1000);

	if ((u4Apollo_ReadDram(APOLLO_DSP2APOLLO_PAT_RESULT_FINAL_ROUND1) == 0x8888) && u4ReadDspSram(APOLLO_APOLLO2DSP_PAT_RESULT_FINAL_ROUND1)== 0x888800)
	{
        Printf("\n Verify OK for Round 1 !\n");  			 
	}	
	else
	{
	    Printf("\n Verify Fail for Round 1 !\n");   
		return 0;
	}

    //Check Round 2 result
	if ((u4Apollo_ReadDram(APOLLO_DSP2APOLLO_PAT_RESULT_FINAL_ROUND2) == 0x8888) && (u4ReadDspSram(APOLLO_APOLLO2DSP_PAT_RESULT_FINAL_ROUND2) == 0x888800))
	{
        Printf("\n Verify OK for Round 2 !\n");  
		return 1;
	}	
	else
	{
	    Printf("\n Verify Fail for Round 2 !\n");   
		return 0;
	} 
	
	
		                      
}

UINT8 vApollo_INT_unaligned_DM_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INT_UNALIGN_TABLE[0], APOLLO_INT_UNALIGN_SIZE);        	  
    vApollo_SetDram(APOLLO_INT_PAT_RESULT, APOLLO_INT_PAT_RESULT_FAIL); 
	
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_PASS1)  //get result
    {
        Printf("[INT : Unaligned DM test]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_FAIL)
    {
        Printf("[INT : Unaligned DM test]  FAIL   \n");
        return 0;
    }
    else
    {
        Printf("[INT : Unaligned DM test] ERR  ; The program may be dead!! \n" );
        return 0;
    }
    return 1;
}

UINT8 vApollo_INT_div_by_0_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INT_DIV0_TABLE[0], APOLLO_INT_DIV0_SIZE);        	  
    vApollo_SetDram(APOLLO_INT_PAT_RESULT, APOLLO_INT_PAT_RESULT_FAIL); 
	
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_PASS2)  //get result
    {
        Printf("[INT : DIV/0 test]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_FAIL)
    {
        Printf("[INT : DIV/0 test]  FAIL   \n");
        return 0;
    }
    else
    {
        Printf("[INT : DIV/0 test] ERR  ; The program may be dead!! \n" );
        return 0;
    }
    return 1;
}

UINT8 vApollo_INT_Stack_OV_Chk_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INT_STACK_OV_CHK_TABLE[0], APOLLO_INT_STACK_OV_CHK_SIZE);        	  
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    if((AUD_ProcGetPc() >= 0x1bc)&&(AUD_ProcGetPc() <= 0x1d0))
    {
        Printf("[INT : Stack Overflow Check test]  PASS \n");
        Printf("Apollo PC =  %x\n", AUD_ProcGetPc());
    }
    else
    {
        Printf("[INT : Stack Overflow Check test]  FAIL   \n");
        return 0;
    }
    
    return 1;
}

UINT8 vApollo_INT_Internal_Timer_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INT_INTERNAL_TIMER_TABLE[0], APOLLO_INT_INTERNAL_TIMER_SIZE);        	  
    vApollo_SetDram(APOLLO_INT_PAT_RESULT, APOLLO_INT_PAT_RESULT_FAIL); 
	
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_PASS3)  //get result
    {
        Printf("[INT : Internal Timer Test]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_FAIL)
    {
        Printf("[INT : Internal Timer Test]  FAIL   \n");
        return 0;
    }
    else
    {
        Printf("[INT : Internal Timer Test] ERR  ; The program may be dead!! \n" );
        return 0;
    }
    return 1;
}

UINT8 vApollo_INT_27MHz_Timer0_tst(void)
{

    Printf("Note that for 27MHz timer test, you shall set Apollo DM[0x22004] first. \n");
    Printf("if (Apollo DM[0x22004] ==1), real IC(APOLLO_CLK=378MHz) case is tested. \n");
    Printf("else, FPGA(APOLLO_CLK should be set to 30MHz !!!) case is tested. \n");
    Printf("Please Note that APOLLO_CLK must > 27MHz for timers to work correctly. \n");
    Printf("Don't use aud.a.pr.dmw to set 0x22004 \n");	
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INT_27MHz_TIMER0_TABLE[0], APOLLO_INT_27MHz_TIMER0_SIZE);        	  
    vApollo_SetDram(APOLLO_INT_PAT_RESULT, APOLLO_INT_PAT_RESULT_FAIL); 
	
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_PASS4)  //get result
    {
        Printf("[INT : 27MHz Timer0 Test]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_FAIL)
    {
        Printf("[INT : 27MHz Timer0 Test]  FAIL   \n");
        return 0;
    }
    else
    {
        Printf("[INT : 27MHz Timer0 Test] ERR  ; The program may be dead!! \n" );
        return 0;
    }
    return 1;
}

UINT8 vApollo_INT_27MHz_Timer1_tst(void)
{

    Printf("Note that for 27MHz timer test, you shall set Apollo DM[0x22004] first. \n");
    Printf("if (Apollo DM[0x22004] ==1), real IC(APOLLO_CLK=378MHz) case is tested. \n");
    Printf("else, FPGA(APOLLO_CLK should be set to 30MHz !!!) case is tested. \n");
    Printf("Please Note that APOLLO_CLK must > 27MHz for timers to work correctly. \n");
    Printf("Don't use aud.a.pr.dmw to set 0x22004 \n");		
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INT_27MHz_TIMER1_TABLE[0], APOLLO_INT_27MHz_TIMER1_SIZE);        	  
    vApollo_SetDram(APOLLO_INT_PAT_RESULT, APOLLO_INT_PAT_RESULT_FAIL); 

    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_PASS5)  //get result
    {
        Printf("[INT : 27MHz Timer1 Test]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_FAIL)
    {
        Printf("[INT : 27MHz Timer1 Test]  FAIL   \n");
        return 0;
    }
    else
    {
        Printf("[INT : 27MHz Timer1 Test] ERR  ; The program may be dead!! \n" );
        return 0;
    }
    return 1;
}

UINT8 vApollo_INT_27MHz_Timer2_tst(void)
{

    Printf("Note that for 27MHz timer test, you shall set Apollo DM[0x22004] first. \n");
    Printf("if (Apollo DM[0x22004] ==1), real IC(APOLLO_CLK=378MHz) case is tested. \n");
    Printf("else, FPGA(APOLLO_CLK should be set to 30MHz !!!) case is tested. \n");
    Printf("Please Note that APOLLO_CLK must > 27MHz for timers to work correctly. \n");
    Printf("Don't use aud.a.pr.dmw to set 0x22004 \n");		
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INT_27MHz_TIMER2_TABLE[0], APOLLO_INT_27MHz_TIMER2_SIZE);        	  
    vApollo_SetDram(APOLLO_INT_PAT_RESULT, APOLLO_INT_PAT_RESULT_FAIL); 
	
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_PASS6)  //get result
    {
        Printf("[INT : 27MHz Timer2 Test]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_FAIL)
    {
        Printf("[INT : 27MHz Timer2 Test]  FAIL   \n");
        return 0;
    }
    else
    {
        Printf("[INT : 27MHz Timer2 Test] ERR  ; The program may be dead!! \n" );
        return 0;
    }
    return 1;
}

UINT8 vApollo_INT_Nested_UnalignedDM_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INT_Nested_UnalignDM_TABLE[0], APOLLO_INT_Nested_UnalignDM_SIZE);        	  
    vApollo_SetDram(APOLLO_INT_PAT_RESULT, APOLLO_INT_PAT_RESULT_FAIL); 
	
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_PASS7)  //get result
    {
        Printf("[INT : Nested INT(unaligned DM access) Test]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_FAIL)
    {
        Printf("[INT : Nested INT(unaligned DM access) Test] FAIL  \n");
        return 0;
    }
    else
    {
        Printf("[INT : Nested INT(unaligned DM access) Test]  ERR  ; The program may be dead!! \n" );
        return 0;
    }
    return 1;
}

UINT8 vApollo_INT_Nested_DivZero_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INT_Nested_DivZ_TABLE[0], APOLLO_INT_Nested_DivZ_SIZE);        	  
    vApollo_SetDram(APOLLO_INT_PAT_RESULT, APOLLO_INT_PAT_RESULT_FAIL); 
	
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_PASS8)  //get result
    {
        Printf("[INT : Nested INT(Div Zero) Test]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_FAIL)
    {
        Printf("[INT : Nested INT(Div Zero) Test] FAIL  \n");
        return 0;
    }
    else
    {
        Printf("[INT : Nested INT(Div Zero) Test]  ERR  ; The program may be dead!! \n" );
        return 0;
    }
    return 1;
}

UINT8 vApollo_INT_Aout2_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INT_Aout2_TABLE[0], APOLLO_INT_Aout2_SIZE);        	  
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR+0x22010,(UINT32) &DATA_APOLLO_INT_Aout2_TABLE[0], DATA_APOLLO_INT_Aout2_SIZE);
    vApollo_SetDram(APOLLO_INT_PAT_RESULT, 0x0); 

    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_START)  //get result
    {
        Printf("[INT : Aout2 Test]  ERR  ; The program may be dead!! \n" );
        return 0;
    }
    else if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==0)
    {
        Printf("[INT : Aout2 Test] FAIL  \n");
        return 0;
    }
    else
    {
        Printf("[INT : Aout2 Test]  PASS \n");
    }
    return 1;
}

UINT8 vApollo_INT_ASRC_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INT_Asrc_TABLE[0], APOLLO_INT_Asrc_SIZE);        	  
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR+0x22010,(UINT32) &DATA_APOLLO_INT_ASRC_TABLE[0], DATA_APOLLO_INT_ASRC_SIZE);
    vApollo_SetDram(APOLLO_AENV_PAT_RESULT, u4GetApolloWorkBufNonInit(1)); 

    // note that the control variables are located at 0x14000, and it should be copy to 0x22010 by RISC first.
    // then Apollo should copy 0x22010[0x3f8] to 0x14000[0x3f8]
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_START)  //get result
    {
        Printf("[INT : ASRC Test]  ERR  ; The program may be dead!! \n" );
        return 0;
    }
    else if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==0)
    {
        Printf("[INT : ASRC Test] FAIL  \n");
        return 0;
    }
    else
    {
        Printf("[INT : ASRC Test]  PASS \n");
    }
    return 1;
}

UINT8 vApollo_Aenv_MMR_Access_tst(void)
{
    Printf("[AENV : MMR registers access Test]Initial: need to run any DSP test pattern to reset DSP for  ASRC should not setting in DSP part \n" );
    CLI_Parser("aud.dsp.pat 3");

    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_AENV_MMR_TABLE[0], APOLLO_AENV_MMR_SIZE);        	  
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    if(u4Apollo_ReadDram(APOLLO_AENV_PAT_RESULT) ==APOLLO_AENV_PAT_RESULT_PASS1)  //get result
    {
        Printf("[ANEV : MMR registers access Test]  PASS \n");
    }
    else if(u4Apollo_ReadDram(APOLLO_INT_PAT_RESULT) ==APOLLO_INT_PAT_RESULT_FAIL)
    {
        Printf("[AENV : MMR registers access Test] FAIL  \n");
        return 0;
    }
    else
    {
        Printf("[AENV : MMR registers access Test]  ERR  ; The program may be dead!! \n" );
        return 0;
    }
    return 1;
}

UINT8 vApollo_Aenv_Aout_Control_tst(void)
{

    Printf("[AENV : AOUT Test]Initial: need to run any DSP test pattern to reset DSP for  ASRC should not setting in DSP part \n" );
    Printf("[AENV : AOUT Test]Initial: need to run any DSP test pattern to reset DSP for  DSP should disalbe AOUT1/AOUT2 interrupt \n" );

    CLI_Parser("aud.dsp.pat 3");

    Printf("Note that before perform this Aout Control test, pinmux shall be set first. \n");
    Printf("The Pinmux settings for V5 PCB is as following : \n");
    Printf("w 0xf000d600 0xd5042800\n");
    Printf("w 0xf000d604 0x3f3bb94\n");
    Printf("w 0xf000d608 0x55400500\n");
    Printf("w 0xf000d60c 0x181051\n");
    Printf("w 0xf000d610 0x1000\n");
    Printf("w 0xf000d614 0x3\n");
    Printf("In addition, DSP shall disable its aout1/aout2 interrupt control, ex:\n");
    Printf("aud.dsp.w 4 0x4300000 or aud.dsp.w 4 0x300000\n");
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_AENV_AOUT_TABLE[0], APOLLO_AENV_AOUT_SIZE);        	  
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR+0x22000,(UINT32) &DATA_APOLLO_AENV_AOUT_TABLE[0], DATA_APOLLO_AENV_AOUT_SIZE);
    // note that the control variables are located at 0x14000, and it should be copy to 0x22000 by RISC first.
    // then Apollo should copy 0x22000[0x218] to 0x14000[0x218]
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    
    Printf("Now, 6-ch data is output. Please check if you can hear a 1.5kHz tone if the output clock is 48kHz.\n");
    Printf("\n In addition, if you want to check the SPDIF PCM output, set the following registers first..\n");
    Printf("aud.dsp.w 0x2a0 0\n");
    Printf("w 0xf0005030 0x0\n");
    Printf("Now, check the SPDIF output by a SPDIF receiver.You should hear a 1.5kHz tone if the output clock is 48kHz.\n");
    return 1;
}

UINT8 vApollo_Aenv_Aout_Asrc_Control_tst(void)
{

    Printf("[AENV : AOUT_ASRC Test]Initial: need to run any DSP test pattern to reset DSP for  ASRC should not setting in DSP part \n" );
    Printf("[AENV : AOUT_ASRC Test]Initial: need to run any DSP test pattern to reset DSP for  DSP should disalbe AOUT1/AOUT2 interrupt \n" );

    CLI_Parser("aud.dsp.pat 3");

    Printf("Note that before perform this Aout/Asrc Control test, pinmux shall be set first. \n");
    Printf("The Pinmux settings for V5 PCB is as following : \n");
    Printf("w 0xf000d600 0xd5042800\n");
    Printf("w 0xf000d604 0x3f3bb94\n");
    Printf("w 0xf000d608 0x55400500\n");
    Printf("w 0xf000d60c 0x181051\n");
    Printf("w 0xf000d610 0x1000\n");
    Printf("w 0xf000d614 0x3\n");
    Printf("In addition, DSP shall disable its aout1/aout2 interrupt control, ex:\n");
    Printf("aud.dsp.w 4 0x4100000 or aud.dsp.w 4 0x100000\n");
    vApollo_SetDram(APOLLO_AENV_PAT_RESULT, u4GetApolloWorkBufNonInit(1)); 
    //pass real apollo DRAM physical start address for asrc test.c
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_AENV_AOUT_ASRC_TABLE[0], APOLLO_AENV_AOUT_ASRC_SIZE);        	  
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR+0x22010,(UINT32) &DATA_APOLLO_AENV_AOUT_ASRC_TABLE[0], DATA_APOLLO_AENV_AOUT_ASRC_SIZE);
    // note that the control variables are located at 0x14000, and it should be copy to 0x22010 by RISC first.
    // then Apollo should copy 0x22010[0x3f8] to 0x14000[0x3f8]
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    Printf("Now you shall be able to hear a 1kHz tone output with fs@48kHz \n");
    
    return 1;
}

UINT8 vApollo_Aenv_IecRaw_Out_Control_tst(void)
{

    Printf("[AENV : IecRaw_OUT Test]Initial: need to run any DSP test pattern to reset DSP for  ASRC should not setting in DSP part \n" );
    Printf("[AENV : IecRaw_OUT Test]Initial: need to run any DSP test pattern to reset DSP for  DSP should disalbe AOUT1/AOUT2 interrupt \n" );

    CLI_Parser("aud.dsp.pat 3");

    Printf("Note that before performing this IEC RAW Output Control test, pinmux shall be set first. \n");
    Printf("The Pinmux settings for V5 PCB is as following : \n");
    Printf("w 0xf000d600 0xd5042800\n");
    Printf("w 0xf000d604 0x3f3bb94\n");
    Printf("w 0xf000d608 0x55400500\n");
    Printf("w 0xf000d60c 0x181051\n");
    Printf("w 0xf000d610 0x1000\n");
    Printf("w 0xf000d614 0x3\n");
    Printf("In addition, DSP shall disable its aout1/aout2 interrupt control, ex:\n");
    Printf("aud.dsp.w 4 0x4100000\n");
    vApollo_SetDram(0x22010, u4GetApolloWorkBufNonInit(1));   //DRAM start address 
    vApollo_SetDram(0x22014, 0x5000);  // byte offset(from DRAM start address) address for iec buffer
    vApollo_SetDram(0x22018, DATA_APOLLO_AC3_51SRC_TABLE_SIZE); // ac3 file size in byte
    //pass these 3 parameters for for iec apollo test code
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR+0x20000+0x5000,(UINT32) &DATA_APOLLO_AC3_51SRC_TABLE[0], DATA_APOLLO_AC3_51SRC_TABLE_SIZE);
    // load acmod2_0.ac3 to Apollo DRAM (0x20000 + 0x5000)
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_AENV_IEC_RAW_OUT_TABLE[0], APOLLO_AENV_IEC_RAW_OUT_SIZE);        	  
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR+0x22020,(UINT32) &DATA_APOLLO_AENV_IEC_RAW_OUT_TABLE[0], DATA_APOLLO_AENV_IEC_RAW_OUT_SIZE);
    // note that the control variables are located at 0x14000, and it should be copy to 0x22010 by RISC first.
    // then Apollo should copy 0x22020[0x218] to 0x14000[0x218]
    vAproc_Enable();
    // sleep
    x_thread_delay(1000);
    Printf("Now Please check the SPDIF output by a SPDIF receiver. \n");
    Printf("Dolby Digital 5.1ch shall be locked, and tones shall be heard. \n");
    
    return 1;
}

UINT8 vApollo_Aenv_Aout_Timer0_Control_tst(void)
{
    Printf("[AENV : AOUT_Timer0 Test]Initial: need to run any DSP test pattern to reset DSP for  ASRC should not setting in DSP part \n" );
    Printf("[AENV : AOUT_Timer0 Test]Initial: need to run any DSP test pattern to reset DSP for  DSP should disalbe AOUT1/AOUT2 interrupt \n" );

    CLI_Parser("aud.dsp.pat 3");

    Printf("Note that before perform this Aout/Timer0 Control test, pinmux shall be set first. \n");
    Printf("The Pinmux settings for V5 PCB is as following : \n");
    Printf("w 0xf000d600 0xd5042800\n");
    Printf("w 0xf000d604 0x3f3bb94\n");
    Printf("w 0xf000d608 0x55400500\n");
    Printf("w 0xf000d60c 0x181051\n");
    Printf("w 0xf000d610 0x1000\n");
    Printf("w 0xf000d614 0x3\n");
    Printf("In addition, DSP shall disable its aout1/aout2 interrupt control, ex:\n");
    Printf("aud.dsp.w 4 0x4300000 or aud.dsp.w 4 0x300000\n");
	vApollo_SetDram(0x2200c, 0); // initial set to 0
	vApollo_SetDram(0x22018, 0); // initial set to 0
	vApollo_SetDram(0x2201c, 0); // initial set to 0
	
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_AENV_AOUT_TMR0_TABLE[0], APOLLO_AENV_AOUT_TMR0_SIZE);        	  
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR+0x22020,(UINT32) &DATA_APOLLO_AENV_AOUT_TMR0_TABLE[0], DATA_APOLLO_AENV_AOUT_TMR0_SIZE);
    // note that the control variables are located at 0x14000, and it should be copy to 0x22020 by RISC first.
    // then Apollo should copy 0x22020[0x230] to 0x14000[0x230]
    vAproc_Enable();
    // sleep
    x_thread_delay(5000);


    Printf("Now, 6-ch data is output. Please check if you can hear a 1.5kHz tone if the output clock is 48kHz.\n");
    Printf("\n In addition, if you want to check the SPDIF PCM output, set the following registers first..\n");
    Printf("aud.dsp.w 0x2a0 0\n");
    Printf("w 0xf0005030 0x0\n");
    Printf("Now, check the SPDIF output by a SPDIF receiver.You should hear a 1.5kHz tone if the output clock is 48kHz.\n");
    Printf("In addition, please monitor DM[0x2200C]. If Aout1 interrupt is missed, this value will increase.\n");
    Printf("DM[0x22018] aout1_int_cnt, DM[0x2201C] 27MHz_timer0_int_cnt\n");

	if (u4Apollo_ReadDram(0x2200C) == 0)
    {
        Printf("\n Verify OK !\n");
        return 1;

    }
    else
    {
        Printf("\n Verify Fail !\n");
        return 0;

    }


	
    //return 1;
}

UINT8 vApollo_Probe_SR_C10_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INST_C10_TABLE[0], APOLLO_INST_C10_SIZE);        	  
    vAproc_Enable();
    // sleep
    x_thread_delay(2000);
    
    return AUD_Apollo_Probe_SR_Chk_C10();
}

UINT8 vApollo_Probe_SR_C11_tst(void)
{
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INST_C11_TABLE[0], APOLLO_INST_C11_SIZE);        	  
    vAproc_Enable();
    // sleep
    x_thread_delay(2000);

    return AUD_Apollo_Probe_SR_Chk_C11();
}


void vApollo_OntPmem(void)
{
    int fail = 0;
    int count = 1;
    int i, j, k;
    int golden_frame_ns = 5;
    int outbuf_addr = 0x2042C;// aout.map = 0x2042C
    int golden_size[5] = {AACENC_DM_GOLDEN_F1_SIZE, AACENC_DM_GOLDEN_F2_SIZE, AACENC_DM_GOLDEN_F3_SIZE, AACENC_DM_GOLDEN_F4_SIZE, AACENC_DM_GOLDEN_F5_SIZE};
    const UINT8 *golden_data[5] = {AACENC_DM_GOLDEN_F1, AACENC_DM_GOLDEN_F2, AACENC_DM_GOLDEN_F3, AACENC_DM_GOLDEN_F4, AACENC_DM_GOLDEN_F5};
    UINT32 *cur_golden;
    UINT32 fpga_result;

    Printf("\nEmu 034 -[%d] PMEM overnigth test ....\n", __TIME__);


    while (1)
    {
        //if ((count % 10) == 0)
        {
            Printf("\n    testing ... [%d]", count);
        }

        vAproc_Disable();
        u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &PM_SEQUENTIAL_ACCESS_TABLE[0], PM_SEQUENTIAL_ACCESS_SIZE); 
        vAproc_Enable();

        // sleep
        x_thread_delay(1000);
                       
        // check result
        if (u4Apollo_ReadDram(APOLLO_PMEM_TEST_RESULT_DM_ADDR) != 0x12345678)
        {
            fail = 1;
            break;
        }


        vAproc_Disable();
        u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &PM_BRANCH_TABLE[0], PM_BRANCH_SIZE); 
        u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1),(UINT32) &PM_BRANCH_DATA[0], PM_BRANCH_DATA_SIZE); 
        vAproc_2WayEnable();

        // sleep
        x_thread_delay(1000);
                       
        // check result
        if (u4Apollo_ReadDram(APOLLO_PMEM_TEST_RESULT_DM_ADDR) != 0x12345678)
        {
            fail = 2;
            break;
        }


        //  AAC Encode Test ....
        for (k = 0; k < 2; k++)
        {
            vAproc_Disable();
            
            u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &AACENC_PM_TABLE[0], AACENC_PM_SIZE); 
            u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1),(UINT32) &AACENC_DM_TABLE[0], AACENC_DM_SIZE); 

            // reset DRAM output
            for (i = 0; i < golden_frame_ns; i++)
            {
                for (j = 0; j < golden_size[i]/4; j++)
                {
                   vApollo_SetDram(outbuf_addr + (1024*i) + (j*4),0x0);                   
                }
            }

            if (k == 0)
            {
                vAproc_2WayEnable(); // 2-way cache
            }
            else
            {
                vAproc_Enable();
            }

 
            // sleep
            x_thread_delay(10000);

      
            for (i = 0; i < golden_frame_ns; i++)
            {
               cur_golden = (UINT32 *)golden_data[i];
               for (j = 0; j < golden_size[i]/4; j++)
               {
                   fpga_result = u4Apollo_ReadDram(outbuf_addr + (1024*i) + (j*4));
            
                   if (*cur_golden++ != fpga_result)                
                   {
                        Printf("\n FRAME %d CHECKING ...", i+1);
                        Printf("\n  ==> Fail @ offset 0x%x = (0x%x, 0x%x) !", j, (*--cur_golden), fpga_result);                     
                        fail = (k == 0) ? 3 : 4;
                        Printf("\nPMEM overnigth test - AAC Encode Test fail@@@@@ \n");

                        break;
                   }
                }
                if (fail !=0)
                {
                   break;  
                }
            }
            if (fail !=0)
            {
               break;  
            }

        }
        if (fail !=0)
        {
            break;  
        }
        count++;
    }

    if (1 == fail)
    {
         Printf("\nPMEM overnigth test - Sequentail Test fail \n");

    }
    else if (2 == fail)
    {
         Printf("\nPMEM overnigth test - Branch Test fail \n");
    }
    else if (3 == fail)
    {
         Printf("\nPMEM overnigth test - AAC Encode Test fail - 2 Way Cache \n");
    }
    else if (4 == fail)
    {
         Printf("\nPMEM overnigth test - AAC Encode Test fail - 4 Way Cache\n");
    }

}

#define BYTE0(data)              (data & 0xFF)
#define BYTE1(data)              ((data & 0xFF00) >> 8)
#define BYTE2(data)              ((data & 0xFF0000) >> 16)
#define BYTE3(data)              ((data & 0xFF000000) >> 24)

void vApollo_PgCacheDump(void)
{
    int i,j;
    unsigned int startEntry = 0;
    int cache2_block = 1024; // word
    int cache4_block = 512; // word
    UINT32 u4Golden0[16];  // CACHE_4WAY_SIZE is = 0x78d8 (bytes) = 8 * 0x1000 (2-way) = 16 * 0x800 (4-way)
    UINT32 u4Golden1[16];
    UINT32 u4Inst0, u4Inst1, u4Inst2, u4Inst3;
    int match0, match1, match2, match3;
    int fail = 0;

    Printf("\nEmu 036 -[%d] Program Cache Dump ....\n", __TIME__);
    
    // init _gu4ApolloMemMap[0] region to 0x0
    x_memset((VOID *)VIRTUAL(u4GetApolloWorkBufNonInit(0)), 0x0, 0x100000); //rest PM for 1M bytes


    // run emu8 program
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &CACHE_4WAY_TABLE[0], CACHE_4WAY_SIZE); 

    vAproc_2WayEnable();

    // sleep
    x_thread_delay(1000);

    vApollo_HaltOn(); //Need to Hold Apollo before doing PM cache dump

    vApolloSet_CacheDbgEn();
    APOLLO_WRITE32(APOLLO_DBG_INDEX, 0x100000);

    vApolloSet_DbgIndxAddr(startEntry);  // read from entry 0x40 = address 0x400

    for (i = 0; i < 512; i++)
    {
        match0 = 0;
        match1 = 0;
        match2 = 0;
        match3 = 0;

        u4Inst0 = u4ApolloRead_DbgIc();
        u4Inst1 = u4ApolloRead_DbgIc();
        
        u4Inst2 = u4ApolloRead_DbgIc();
        u4Inst3 = u4ApolloRead_DbgIc();
        
        Printf("\n [%03d]@[0x%03x] = ",startEntry+i, (startEntry+i)*8);
        Printf("\n                [0x%02x,    0x%02x,    0x%02x,    0x%02x]",  BYTE0(u4Inst0), BYTE1(u4Inst0), BYTE2(u4Inst0), BYTE3(u4Inst0));
        Printf("\n                [0x%02x,    0x%02x,    0x%02x,    0x%02x]",  BYTE0(u4Inst1), BYTE1(u4Inst1), BYTE2(u4Inst1), BYTE3(u4Inst1));
        Printf("\n                [0x%02x,    0x%02x,    0x%02x,    0x%02x]",  BYTE0(u4Inst2), BYTE1(u4Inst2), BYTE2(u4Inst2), BYTE3(u4Inst2));
        Printf("\n                [0x%02x,    0x%02x,    0x%02x,    0x%02x]",  BYTE0(u4Inst3), BYTE1(u4Inst3), BYTE2(u4Inst3), BYTE3(u4Inst3));        

        Printf("\n   Golden =");

        for (j = 0; j < 8; j++)
        {
            u4Golden0[j]  = *((unsigned int *)(CACHE_4WAY_TABLE) + i*2 + j*cache2_block);
            u4Golden1[j]  = *((unsigned int *)(CACHE_4WAY_TABLE) + i*2 + j*cache2_block + 1);
            Printf("\n                [0x%02x,    0x%02x,    0x%02x,    0x%02x]",  BYTE0(u4Golden0[j]), BYTE1(u4Golden0[j]), BYTE2(u4Golden0[j]), BYTE3(u4Golden0[j]));
            Printf("\n                [0x%02x,    0x%02x,    0x%02x,    0x%02x]",  BYTE0(u4Golden1[j]), BYTE1(u4Golden1[j]), BYTE2(u4Golden1[j]), BYTE3(u4Golden1[j]));

            if (u4Inst0 == u4Golden0[j])  match0 = 1;
            if (u4Inst2 == u4Golden0[j])  match2 = 1;
            if (u4Inst1 == u4Golden1[j])  match1 = 1;
            if (u4Inst3 == u4Golden1[j])  match3 = 1;
        }

        if (match0 == 0)  { Printf("\n Inst0 not match"); fail = 1; break;}
        if (match1 == 0)  { Printf("\n Inst1 not match"); fail = 1; break;}
        if (match2 == 0)  { Printf("\n Inst2 not match"); fail = 1; break;}
        if (match3 == 0)  { Printf("\n Inst3 not match"); fail = 1; break;}
    }


    if (fail == 1)
    {
         Printf("\n\n  ==> 2 Way - Verification Fail !!");

    }
    else
    {
         Printf("\n\n  ==> 2 Way - Verification OK !!");

    }
    Printf("\n");

	vApollo_HaltOff();//release Apollo Hold


    // sleep
    x_thread_delay(5000);

    fail = 0;

    // run emu8 program
    vAproc_Disable();
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &CACHE_4WAY_TABLE[0], CACHE_4WAY_SIZE); 

    vAproc_Enable();

    // sleep
    x_thread_delay(1000);


    vApollo_HaltOn(); //Need to Hold Apollo before doing PM cache dump
    
    vApolloSet_CacheDbgEn();
    APOLLO_WRITE32(APOLLO_DBG_INDEX, 0x100000);

    // First part - 4 way cache Inst0 index start from 0x400
    startEntry = 0x400;
    vApolloSet_DbgIndxAddr(startEntry);  // read from entry 0x40 = address 0x400

    for (i = 0; i < 256; i++)
    {
        match0 = 0;
        match1 = 0;
        match2 = 0;
        match3 = 0;
        
        u4Inst0 = u4ApolloRead_DbgIc();
        u4Inst1 = u4ApolloRead_DbgIc();
        u4Inst2 = u4ApolloRead_DbgIc();
        u4Inst3 = u4ApolloRead_DbgIc();

 
        Printf("\n [%03d]@[0x%03x] = ",i, (startEntry+i)*4);
        Printf("\n                [0x%02x,    0x%02x,    0x%02x,    0x%02x]",  BYTE0(u4Inst0), BYTE1(u4Inst0), BYTE2(u4Inst0), BYTE3(u4Inst0));
        Printf("\n                [0x%02x,    0x%02x,    0x%02x,    0x%02x]",  BYTE0(u4Inst1), BYTE1(u4Inst1), BYTE2(u4Inst1), BYTE3(u4Inst1));
        Printf("\n                [0x%02x,    0x%02x,    0x%02x,    0x%02x]",  BYTE0(u4Inst2), BYTE1(u4Inst2), BYTE2(u4Inst2), BYTE3(u4Inst2));
        Printf("\n                [0x%02x,    0x%02x,    0x%02x,    0x%02x]",  BYTE0(u4Inst3), BYTE1(u4Inst3), BYTE2(u4Inst3), BYTE3(u4Inst3));        

        Printf("\n   Golden =");

        for (j = 0; j < 16; j++)
        {
            u4Golden0[j]  = *((unsigned int *)(CACHE_4WAY_TABLE) + i*2 + j*cache4_block);
            Printf("\n                [0x%02x,    0x%02x,    0x%02x,    0x%02x]",  BYTE0(u4Golden0[j]), BYTE1(u4Golden0[j]), BYTE2(u4Golden0[j]), BYTE3(u4Golden0[j]));

            if (u4Inst0 == u4Golden0[j])  match0 = 1;
            if (u4Inst1 == u4Golden0[j])  match1 = 1;
            if (u4Inst2 == u4Golden0[j])  match2 = 1;
            if (u4Inst3 == u4Golden0[j])  match3 = 1;
        }

        if (match0 == 0)  { Printf("\n Inst0 not match"); fail = 1; break;}
        if (match1 == 0)  { Printf("\n Inst1 not match"); fail = 1; break;}
        if (match2 == 0)  { Printf("\n Inst2 not match"); fail = 1; break;}
        if (match3 == 0)  { Printf("\n Inst3 not match"); fail = 1; break;}
        
    }


   // Second part - 4 way cache Inst1 index start from 0x0
    startEntry = 0;
    vApolloSet_DbgIndxAddr(startEntry);  // read from entry 0x40 = address 0x400

    for (i = 0; i < 256; i++)
    {
        match0 = 0;
        match1 = 0;
        match2 = 0;
        match3 = 0;
        
        u4Inst0 = u4ApolloRead_DbgIc();
        u4Inst1 = u4ApolloRead_DbgIc();
        u4Inst2 = u4ApolloRead_DbgIc();
        u4Inst3 = u4ApolloRead_DbgIc();
  
    
        Printf("\n [%03d]@[0x%03x] = ",startEntry+i+1, (startEntry+i+1)*4);
        Printf("\n                [0x%02x,    0x%02x,    0x%02x,    0x%02x]",  BYTE0(u4Inst0), BYTE1(u4Inst0), BYTE2(u4Inst0), BYTE3(u4Inst0));
        Printf("\n                [0x%02x,    0x%02x,    0x%02x,    0x%02x]",  BYTE0(u4Inst1), BYTE1(u4Inst1), BYTE2(u4Inst1), BYTE3(u4Inst1));
        Printf("\n                [0x%02x,    0x%02x,    0x%02x,    0x%02x]",  BYTE0(u4Inst2), BYTE1(u4Inst2), BYTE2(u4Inst2), BYTE3(u4Inst2));
        Printf("\n                [0x%02x,    0x%02x,    0x%02x,    0x%02x]",  BYTE0(u4Inst3), BYTE1(u4Inst3), BYTE2(u4Inst3), BYTE3(u4Inst3));        

        Printf("\n   Golden =");

        for (j = 0; j < 16; j++)
        {
            u4Golden1[j]  = *((unsigned int *)(CACHE_4WAY_TABLE) + i*2 + 1 + j*cache4_block);
            Printf("\n                [0x%02x,    0x%02x,    0x%02x,    0x%02x]",  BYTE0(u4Golden1[j]), BYTE1(u4Golden1[j]), BYTE2(u4Golden1[j]), BYTE3(u4Golden1[j]));

            if (u4Inst0 == u4Golden1[j])  match0 = 1;
            if (u4Inst1 == u4Golden1[j])  match1 = 1;
            if (u4Inst2 == u4Golden1[j])  match2 = 1;
            if (u4Inst3 == u4Golden1[j])  match3 = 1;
        }

        if (match0 == 0)  { Printf("\n Inst0 not match"); fail = 1; break;}
        if (match1 == 0)  { Printf("\n Inst1 not match"); fail = 1; break;}
        if (match2 == 0)  { Printf("\n Inst2 not match"); fail = 1; break;}
        if (match3 == 0)  { Printf("\n Inst3 not match"); fail = 1; break;}
        
    }
    
    if (fail == 1)
    {
         Printf("\n\n  ==> 4 Way - Verification Fail !!");

    }
    else
    {
         Printf("\n\n  ==> 4 Way - Verification OK !!");

    }
    Printf("\n");
    
    vApollo_HaltOff();//release Apollo Hold
}

void vApollo_AACEncTst(void)
{
    int i, j, k;
    int golden_frame_ns = 5;
    int outbuf_addr = 0x2042C;
    int golden_size[5] = {AACENC_DM_GOLDEN_F1_SIZE, AACENC_DM_GOLDEN_F2_SIZE, AACENC_DM_GOLDEN_F3_SIZE, AACENC_DM_GOLDEN_F4_SIZE, AACENC_DM_GOLDEN_F5_SIZE};
    const UINT8 *golden_data[5] = {AACENC_DM_GOLDEN_F1, AACENC_DM_GOLDEN_F2, AACENC_DM_GOLDEN_F3, AACENC_DM_GOLDEN_F4, AACENC_DM_GOLDEN_F5};
    UINT32 *cur_golden;    
    UINT32 fpga_result;
    
    UINT32 PmAccCnt[2];            
    UINT32 PmAccHitCnt[2];        
    UINT32 PmTotalAccCnt[2];      
    UINT32 PgCacheHitCnt[2];      
    UINT32 PgCacheMissCnt[2];     
    UINT32 PgCacheMissPenalty[2]; 
    UINT32 PgTotalCycle[2];       
    UINT32 StartPc = 0x200;
    UINT32 EndPc = 0x450;
    
    Printf("\nEmu 024 -[%d] AAC Encode Test ....\n", __TIME__);

    for (k = 0; k < 2; k++)
    {
        vApollo_HaltOn(); //Apollo Hold  before reset Apollo
    
        vAproc_Disable();
        
        u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &AACENC_PM_TABLE[0], AACENC_PM_SIZE); 
        u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1),(UINT32) &AACENC_DM_TABLE[0], AACENC_DM_SIZE); 

        if (k == 0)
        {
            Printf("\n PM addr = 0x%x", u4GetApolloWorkBufNonInit(0));
            Printf("\n DM addr = 0x%x", u4GetApolloWorkBufNonInit(1));
        }

        // reset DRAM output
        for (i = 0; i < golden_frame_ns; i++)
        {
            for (j = 0; j < golden_size[i]/4; j++)
            {
                vApollo_SetDram(outbuf_addr + (1024*i) + (j*4),0x0);

            }
        }
        
        Printf("\n");

        if (k == 0)
        {
            Printf("\n == 2-Way Cache == ");
        }
        else
        {
            Printf("\n == 4-Way Cache == ");
        }


        if (k == 0)
        {
            vAproc_2WayEnable(); // 2-way cache
        }
        else
    {
            vAproc_Enable();
        }

        vApolloSet_DbgMonStartPc(StartPc);
        vApolloSet_DbgMonStopPc(EndPc);
        vApollo_DbgMonCntClr();
        vApollo_DbgMonIterEn();
        vApollo_DbgMonPcEn(); 

        vApollo_HaltOff(); //release Apollo Hold

        // sleep
        x_thread_delay(10000);

        PmAccCnt[k]                = u4ApolloGet_PmAccCnt();
        PmAccHitCnt[k]             = u4ApolloGet_PmAccHitCnt();
        PmTotalAccCnt[k]           = u4ApolloGet_PmTotalAccCnt();
        PgCacheHitCnt[k]           = u4ApolloGet_PgCacheHitCnt();
        PgCacheMissCnt[k]          = u4ApolloGet_PgCacheMissCnt();
        PgCacheMissPenalty[k]      = u4ApolloGet_PgCacheMissPenalty();
        PgTotalCycle[k]            = u4ApolloGet_DbgMonTotalCycle();
 
 
        for (i = 0; i < golden_frame_ns; i++)
        {
            Printf("\n FRAME %d CHECKING ...", i+1);
            cur_golden = (UINT32 *)golden_data[i];
            
            for (j = 0; j < golden_size[i]/4; j++)
            {
                fpga_result = u4Apollo_ReadDram(outbuf_addr + (1024*i) + (j*4));
            
                if (*cur_golden++ != fpga_result)                
                {
                    Printf(" ==> Fail @ offset 0x%x = (0x%x, 0x%x) !", j, (*--cur_golden), fpga_result);
                    return ;
                }
            }
            Printf(" ==> OK !");
        }
    }

    Printf("\n\n == Cache Performance Reuslt ==");
    Printf("\n Test Item          = 2-Way  vs 4-Way");
    Printf("\n PmAccCnt           = 0x%08x vs 0x%08x", PmAccCnt[0], PmAccCnt[1]);
    Printf("\n PmAccHitCnt        = 0x%08x vs 0x%08x", PmAccHitCnt[0], PmAccHitCnt[1]);
    Printf("\n PmTotalAccCnt      = 0x%08x vs 0x%08x", PmTotalAccCnt[0], PmTotalAccCnt[1]);
    Printf("\n PgCacheHitCnt      = 0x%08x vs 0x%08x", PgCacheHitCnt[0], PgCacheHitCnt[1]);
    Printf("\n PgCacheMissCnt     = 0x%08x vs 0x%08x", PgCacheMissCnt[0], PgCacheMissCnt[1]);
    Printf("\n PgCacheMissPenalty = 0x%08x vs 0x%08x", PgCacheMissPenalty[0], PgCacheMissPenalty[1]);
    Printf("\n PgTotalCycle       = 0x%08x vs 0x%08x", PgTotalCycle[0], PgTotalCycle[1]);

}
void vApollo_ApolloHoldTst(void)
{
    UINT32 u4UpExtMemSadr=0x10000;
	UINT32  data;
	
    
     Printf("\nEmu 030 -[%d] Apollo Hold condiction test ....\n", __TIME__);

	
     //########## Testing  PC Sequtially ###############//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x80); //enable bit7: PC Sequtially test !
    
     vAproc_Disable();  
     
	 //load kernel program 1 
     u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_HOLD_PCSequtially_Kernel_1_TEST_TABLE[0], APOLLO_HOLD_PCSequtially_Kernel_1_TEST_SIZE); 
                    
     // load kernel program 2 to offset 0xffe0
     u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0)+0xffe0,(UINT32) &APOLLO_HOLD_PCSequtially_Kernel_2_TEST_TABLE[0], APOLLO_HOLD_PCSequtially_Kernel_2_TEST_SIZE); 
     vApolloSet_UpExtMemSadr(u4UpExtMemSadr);


     // User space address base control by RISC
     // set user space address bass Dram offset 0x20000
     vApolloSet_UpAddrBaseByRisc(u4GetApolloWorkBufNonInit(0)+0x20000-u4UpExtMemSadr);
            
     vAproc_Enable();
     // sleep
     x_thread_delay(1000);

	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x81)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------PC Sequtially !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------PC Sequtially !\n");
	 }



	 //########## Testing  Cross DSP Hold ###############//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x100); //enable bit8: DSP hold then Apollo hold test
	 CLI_Parser("aud.dsp.pr.p"); //DSP Hold
	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x101)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Cross DSP Hold !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Cross DSP Hold !\n");
	 }
     CLI_Parser("aud.dsp.pr.g"); //DSP go
	
	
     //##########     Write certain physical address     ################//
     //########## (With programmable mask, only for DRAM) ###############//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x40); //enable bit6: Write certain physical address.
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDRAMH_ADR,(u4GetApolloWorkBufNonInit(1)-0x20000)+0x21000); //testing Apollo Dram address: 0x21000
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDRAMH_ADRMASK,0x7ffffff); // address masking
     
	 
	 vAproc_Disable();  
     
	 //load code
     u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_HOLD_WriteMem_TEST_TABLE[0], APOLLO_HOLD_WriteMem_TEST_SIZE); 
     vAproc_Enable();
     // sleep
     x_thread_delay(100);


     //@@@@@@@@@@@ Test 1 @@@@@@@@@@@@//
	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x41)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain physical address. ....Test 1 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain physical address. ....Test 1!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x40); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 2 @@@@@@@@@@@@//
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDRAMH_ADRMASK,0x7fffff0); // address masking
	 vApollo_SetDram(APOLLO_HOLD__WriteCertain_DRAM_RC2AP_IDENTIFY_2,0x8888);//identify to Apollo to next test

     //wait for Apollo write data....
     x_thread_delay(100);
	 
     //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x41)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain physical address. ....Test 2!\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain physical address.....Test 2 !\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x40); //Release Apollo Hold
	 
     //@@@@@@@@@@@ Test 3 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDRAMH_ADRMASK,0x7fffff0); // address masking
	 vApollo_SetDram(APOLLO_HOLD__WriteCertain_DRAM_RC2AP_IDENTIFY_3,0x8888);//identify to Apollo to next test
     //wait for Apollo write data....
     x_thread_delay(100);
	 
     //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x41)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain physical address. ....Test 3!\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain physical address.....Test 3 !\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x40); //Release Apollo Hold
	 
     //@@@@@@@@@@@ Test 4 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDRAMH_ADRMASK,0x7fffff0); // address masking
	 vApollo_SetDram(APOLLO_HOLD__WriteCertain_DRAM_RC2AP_IDENTIFY_4,0x8888);//identify to Apollo to next test
     //wait for Apollo write data....
     x_thread_delay(100);
	 
     //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x41)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain physical address. ....Test 4!\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain physical address.....Test 4 !\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x40); //Release Apollo Hold
	 
     //@@@@@@@@@@@ Test 5 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDRAMH_ADRMASK,0x7ffff0f); // address masking
	 vApollo_SetDram(APOLLO_HOLD__WriteCertain_DRAM_RC2AP_IDENTIFY_5,0x8888);//identify to Apollo to next test
     //wait for Apollo write data....
     x_thread_delay(100);
	 
     //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x41)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain physical address. ....Test 5!\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain physical address.....Test 5 !\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x40); //Release Apollo Hold
	 
     //@@@@@@@@@@@ Test 6 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDRAMH_ADRMASK,0x7ffff0f); // address masking
	 vApollo_SetDram(APOLLO_HOLD__WriteCertain_DRAM_RC2AP_IDENTIFY_6,0x8888);//identify to Apollo to next test
     //wait for Apollo write data....
     x_thread_delay(100);
	 
     //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x41)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain physical address. ....Test 6!\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain physical address.....Test 6 !\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x40); //Release Apollo Hold
	 
     //@@@@@@@@@@@ Test 7 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDRAMH_ADRMASK,0x7ffff0f); // address masking
	 vApollo_SetDram(APOLLO_HOLD__WriteCertain_DRAM_RC2AP_IDENTIFY_7,0x8888);//identify to Apollo to next test
     //wait for Apollo write data....
     x_thread_delay(100);
	 
     //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x41)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain physical address. ....Test 7!\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain physical address.....Test 7 !\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x40); //Release Apollo Hold
	 
     //@@@@@@@@@@@ Test 8 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDRAMH_ADRMASK,0x7ffff0f); // address masking
	 vApollo_SetDram(APOLLO_HOLD__WriteCertain_DRAM_RC2AP_IDENTIFY_8,0x8888);//identify to Apollo to next test
     //wait for Apollo write data....
     x_thread_delay(100);
	 
     //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x41)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain physical address. ....Test 8!\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain physical address.....Test 8 !\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x40); //Release Apollo Hold
	 
     //@@@@@@@@@@@ Test 9 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDRAMH_ADRMASK,0x7ffff0f); // address masking
	 vApollo_SetDram(APOLLO_HOLD__WriteCertain_DRAM_RC2AP_IDENTIFY_9,0x8888);//identify to Apollo to next test
     //wait for Apollo write data....
     x_thread_delay(100);
	 
     //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x41)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain physical address. ....Test 9!\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain physical address.....Test 9 !\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x40); //Release Apollo Hold
	 
     //@@@@@@@@@@@ Test 10 @@@@@@@@@@@@//
     //CMPT Write
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDRAMH_ADR,u4GetApolloWorkBufNonInit(1)+0x1000); //testing Apollo CMPT Dram address: 0x1000 (offset byte address)

     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDRAMH_ADRMASK,0x7ffff0f); // address masking
	 vApollo_SetDram(APOLLO_HOLD__WriteCertain_DRAM_RC2AP_IDENTIFY_10,0x8888);//identify to Apollo to next test
     //wait for Apollo write data....
     x_thread_delay(100);
	 
     //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x41)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain physical address. ....Test 10!\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain physical address.....Test 10 !\n");
	 }

  
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x40); //Release Apollo Hold


	 
     //##########     Write DRAM outside.     ################//    
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x20); //enable bit5: Write DRAM outside, testing   0x21000 <= Dram  <= 0x21100
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDRAMOH_ADRLB,(u4GetApolloWorkBufNonInit(1)-0x20000)+0x21000); //testing Apollo Dram Lower address: 0x21000
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDRAMOH_ADRUB,(u4GetApolloWorkBufNonInit(1)-0x20000)+0x21100); //testing Apollo Dram Upper address: 0x21100     
	 vApollo_SetDram(APOLLO_HOLD__WriteDramOutside_RC2AP_IDENTIFY_1,0x8888);//identify to Apollo to Test "Write Dram Outside"
	 //wait for Apollo write data....
     x_thread_delay(100);


	 //@@@@@@@@@@@ Test 1 @@@@@@@@@@@@//
     //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x21)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write Dram Outside. ....Test 1!\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write Dram Outside.....Test 1 !\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x20); //Release Apollo Hold


	 //@@@@@@@@@@@ Test 2 @@@@@@@@@@@@//
	 vApollo_SetDram(APOLLO_HOLD__WriteDramOutside_RC2AP_IDENTIFY_2,0x8888);//identify to Apollo to next test
     //wait for Apollo write data....
     x_thread_delay(100);

	 
     //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x21)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write Dram Outside. ....Test 2!\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify OK for Apollo Hold------Write Dram Outside.....Test 2 !\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x20); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 3 @@@@@@@@@@@@//
	 vApollo_SetDram(APOLLO_HOLD__WriteDramOutside_RC2AP_IDENTIFY_3,0x8888);//identify to Apollo to next test
     //wait for Apollo write data....
     x_thread_delay(100);

	 
     //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x21)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write Dram Outside. ....Test 3!\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify OK for Apollo Hold------Write Dram Outside.....Test 3 !\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x20); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 4 @@@@@@@@@@@@//
	 vApollo_SetDram(APOLLO_HOLD__WriteDramOutside_RC2AP_IDENTIFY_4,0x8888);//4//identify to Apollo to next test
     //wait for Apollo write data....
     x_thread_delay(100);

	 
     //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x21)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write Dram Outside. ....Test 2!\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify OK for Apollo Hold------Write Dram Outside.....Test 4 !\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x20); //Release Apollo Hold


	 //@@@@@@@@@@@ Test 5 @@@@@@@@@@@@//
	 vApollo_SetDram(APOLLO_HOLD__WriteDramOutside_RC2AP_IDENTIFY_5,0x8888);//4//identify to Apollo to next test
     //wait for Apollo write data....
     x_thread_delay(100);

	 
     //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x21)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write Dram Outside. ....Test 5!\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write Dram Outside.....Test 5 !\n");
	 }

	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x20); //Release Apollo Hold



	 //##########     Write certain data to Ceartain memory      ################//
     //##########  (Both Data and Address has programmable mask  ################//
     //##########          (For MMR, SRAM, and DRAM)             ################//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x10); //enable bit4: Certain data to Certain memory, "not care data"
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_ADR,0x22344); //testing Apollo Dram address: 0x22344
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_ADRMASK,0x7ffffff); // address masking
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_DATA,0x12345678); //data:0x12345678
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_DATAMASK,0x7fffffff); // data masking
     
	 vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_1,0x8888);//identify to Apollo to Test "Write certain data to Ceartain memory "

     // sleep
     x_thread_delay(100);


     //@@@@@@@@@@@ Test 1 @@@@@@@@@@@@//
	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x11)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 1 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 1!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x10); //Release Apollo Hold

     //@@@@@@@@@@@ Test 2 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x3010); //"32-bit care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_2,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x3011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 2 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 2!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x3010); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 3 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x3010); //"32-bit care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_3,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x3011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 3 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 3!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x3010); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 4 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x2010); //"16-bit care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_4,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x2011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 4 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 4!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x2010); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 5 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x2010); //"16-bit care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_5,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x2011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 5 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 5!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x2010); //Release Apollo Hold
    
     //@@@@@@@@@@@ Test 6 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x2010); //"16-bit care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_6,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x2011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 6 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 6!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x2010); //Release Apollo Hold	 

	 //@@@@@@@@@@@ Test 7 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //"byte care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_7,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x1011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 7 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 7!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //Release Apollo Hold	 

	 //@@@@@@@@@@@ Test 8 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //"byte care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_8,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x1011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 8 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 8!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 9 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //"byte care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_9,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x1011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 9 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 9!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 10 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //"byte care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_10,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x1011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 10 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 10!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //Release Apollo Hold

     //@@@@@@@@@@@ Test 11 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //"byte care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_11,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x1011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 11 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 11!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //Release Apollo Hold

     //@@@@@@@@@@@ Test 12 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //"byte care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_12,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x1011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 12 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 12!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //Release Apollo Hold

     //@@@@@@@@@@@ Test 13 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x10); //enable bit4: Certain data to Certain memory, "not care data"
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_ADR,0x22344); //testing Apollo Dram address: 0x22344
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_ADRMASK,0xFF00F); // address masking..... 0x22XX4
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_DATA,0x12345678); //data:0x12345678 
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_DATAMASK,0x7FFF00FF); // data masking ..... 0x1234XX78
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_13,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x11)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 13 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 13!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x10); //Release Apollo Hold
    
    //@@@@@@@@@@@ Test 14 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x3010); // "32bit care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_14,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x3011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 14 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 14!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x3010); //Release Apollo Hold

     //@@@@@@@@@@@ Test 15 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x3010); // "32bit care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_15,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x3011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 15 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 15!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x3010); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 16 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x2010); // "16bit care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_16,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x2011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 16 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 16!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x2010); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 17 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x2010); // "16bit care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_17,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x2011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 17 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 17!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x2010); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 18 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x2010); // "16bit care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_18,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x2011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 18 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 18!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x2010); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 19 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x2010); // "16bit care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_19,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x2011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 19 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 19!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x2010); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 20 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); // "byte care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_20,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x1011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 20 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 20!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 21 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); // "byte care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_21,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x1011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 21 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 21!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 22 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); // "byte care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_22,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x1011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 22 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 22!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 23 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); // "byte care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_23,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x1011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 23 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 23!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 24 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); // "byte care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_24,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x1011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 24 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 24!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 25 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); // "byte care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_25,0x8888);//identify to Apollo to Test next item

     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x1011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 25 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 25!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 26 @@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //"byte care"
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_ADR,0x22344); //testing Apollo Dram address: 0x22344
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_ADRMASK,0xFFFF0); // address masking..... 0x2234X
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_DATA,0x12345678); //data:0x12345678 
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_DATAMASK,0x7FFFFF00); // data masking ..... 0x123456XX
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_26,0x8888);//identify to Apollo to Test next item
     
     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x1011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 26 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 26!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //Release Apollo Hold

     //@@@@@@@@@@@ Test 27@@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //"byte care"
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_ADR,0x12345); //testing Apollo Sram address: 0x12345
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_ADRMASK,0xFF00F); // address masking..... 0x12XX5
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_DATA,0x12345678); //data:0x12345678 
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_DATAMASK,0x7FFFF00F); // data masking ..... 0x12345XX8
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_27,0x8888);//identify to Apollo to Test next item
     
     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x1011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 27 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 27!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //Release Apollo Hold

     //@@@@@@@@@@@ Test 28@@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //"byte care"
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_ADR,0x508); //testing Apollo Sram address: 0x12345
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_ADRMASK,0xFFFFFFFF); // address masking..... 0x508
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_DATA,0x12345678); //data:0x12345678 
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_WDMH_DATAMASK,0x7FFFF00F); // data masking ..... 0x12345XX8
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_28,0x8888);//identify to Apollo to Test next item
     
     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x1011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 28 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 28!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //Release Apollo Hold

	 //@@@@@@@@@@@ Test 29@@@@@@@@@@@@//
     APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //"byte care"
     vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_29,0x8888);//identify to Apollo to Test next item
     
     // sleep
     x_thread_delay(100);


	 //Check if the Apollo Hold !	
     data=APOLLO_READ32(APOLLO_REG_APL_RCDBG_CTRL); 
	 if (data==0x1011)
	 { 
	   //Apollo Hold!
	   Printf("\n Verify Fail for Apollo Hold------Write certain data to Ceartain memory. ....Test 29 !\n");  
	 }
	 else
	 {
	   //Apollo Non-Hold!
	   Printf("\n Verify OK for Apollo Hold------Write certain data to Ceartain memory. ....Test 29!\n");
	 }
	 
	 APOLLO_WRITE32(APOLLO_REG_APL_RCDBG_CTRL,0x1010); //Release Apollo Hold
	 
	 vApollo_SetDram(APOLLO_HOLD__CertainAddCertainData_DRAM_RC2AP_IDENTIFY_30,0x8888);//identify to Apollo to Test next item
 
		
}

void AUD_ApolloPatternTest(UINT8 u1PatId,UINT8 u1ShowHelp)
{
    UINT8 u1TestResult;
    UINT32 u4TestRound;

    if (u1PatId == 0)
    {
        UINT8 i;
        //Printf("[Pattern Revision] %s\n", PAT_REV);                
        //Printf("aud.dsp.pat [u1PatId] (0xf00005510 = 11101212 OK)\n");
        for (i=1;i<MAX_TST_PAT;i++)
        {
            Printf("  %2d - %s\n", i, _arApolloPatType[i].szApolloPatName);
        }        
        //Printf("[Note] Please stop all of the audio jobs first by aud.stop, also please stop mid (mid.r)\n");
        //Printf("       If you need to step trace from the pattern start 0, please set the bp at 0 first. (aud.dsp.pr.stb + 0 0)\n");
        //Printf("       Once you need to retrace the pattern, please reset the dsp (aud.dsp.pr.reset)\n");
    }
    else
    {
        #ifndef CC_FPGA
        MID_Reset();
        #endif
        
        switch (u1PatId)
        {
            case MIXED_MEM_PAT:         
                // DMEM SRAM1 test
                printf("\n");
            	  u1TestResult = vApollo_dmem_sram1_tst();
                if (u1TestResult == 1)
                    printf("[DMEM] SRAM1 Test: Pass \n");
                else
                    printf("[DMEM] SRAM1 Test: Failed \n");

                // DMEM DRAM1 test
                printf("\n");
            	  u1TestResult = vApollo_dmem_dram1_tst();
                if (u1TestResult == 1)
                    printf("[DMEM] DRAM1 Test: Pass \n");
                else
                    printf("[DMEM] DRAM1 Test: Failed \n");

                // DMEM DRAM2 test
                printf("\n");
            	  u1TestResult = vApollo_dmem_dram2_tst();
                if (u1TestResult == 1)
                    printf("[DMEM] DRAM2 Test: Pass \n");
                else
                    printf("[DMEM] DRAM2 Test: Failed \n");

                // DMEM DRAM3 test
                printf("\n");
            	  u1TestResult = vApollo_dmem_dram3_tst();
                if (u1TestResult == 1)
                    printf("[DMEM] DRAM3 Test: Pass \n");
                else
                    printf("[DMEM] DRAM3 Test: Failed \n");

                // DMEM Mix1 test
                printf("\n");
                u1TestResult = vApollo_dmem_mix1_tst();
                if (u1TestResult == 1)
                    printf("[DMEM] Mix1 Test: Pass \n");
                else
                    printf("[DMEM] Mix1 Test: Failed \n");

                // DMEM Mix2 test
                printf("\n");
                u1TestResult = vApollo_dmem_mix2_tst();
                if (u1TestResult == 1)
                    printf("[DMEM] Mix2 Test: Pass \n");
                else
                    printf("[DMEM] Mix2 Test: Failed \n");

                printf("\n");
                u1TestResult = vApollo_dmem_mix3_tst();
                if (u1TestResult == 1)
                    printf("[DMEM] Mix3 Test: Pass \n");
                else
                    printf("[DMEM] Mix3 Test: Failed \n");

                break;  
            case ADDR_CONFLICT_PAT :
            	  vAproc_Disable();
            	  u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &ADDR_CONFLICT_TABLE[0], ADDR_CONFLICT_SIZE); 
            	  u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1)+0x2000,(UINT32) &ADDR_CONFLICT_DATA[0], ADDR_CONFLICT_DATA_SIZE);            	  
            	  vAproc_Enable();
               // sleep
               x_thread_delay(1000);

            	  if (u1ShowHelp)
                {                
                    if(u4Apollo_ReadDram(APOLLO_DMEM_ADDR_CONFLICT_PAT_RESULT) ==APOLLO_DMEM_ADDR_CONFLICT_PASS_RESULT)  //get result
                    {
                        Printf("[ADDR_CONFLICT_PAT]  PASS \n");
                    }
                    else if(u4Apollo_ReadDram(APOLLO_DMEM_ADDR_CONFLICT_PAT_RESULT) ==APOLLO_DMEM_ADDR_CONFLICT_NG_RESULT)
                    {
                        Printf("[ADDR_CONFLICT_PAT]  NG , Result = 0x%x \n",u4Apollo_ReadDram(APOLLO_DMEM_ADDR_CONFLICT_TEST_RESULT));
                        ASSERT(0);
                    }
                    else
                    {
                        Printf("[ADDR_CONFLICT_PAT]  ERR  ; The program may be dead!! \n" );
                        ASSERT(0);
                    }
                    
                }
                break;
            case READ_AFTER_WRITE_PAT :
            	  vAproc_Disable();
            	  u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &READ_AFTER_WRITE_TABLE[0], READ_AFTER_WRITE_SIZE); 
            	  u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1),(UINT32) &READ_AFTER_WRITE_DATA[0], READ_AFTER_WRITE_DATA_SIZE);            	  
            	  vAproc_Enable();
               // sleep
               x_thread_delay(1000);
          
            	  if (u1ShowHelp)
                {
                    if(u4Apollo_ReadDram(APOLLO_DMEM_READ_AFTER_WRITE_PAT_RESULT) ==APOLLO_DMEM_READ_AFTER_WRITE_PASSRESULT)  //get result
                    {
                        Printf("[READ_AFTER_WRITE_PAT]  PASS \n");
                    }
                    else if(u4Apollo_ReadDram(APOLLO_DMEM_READ_AFTER_WRITE_PAT_RESULT) ==APOLLO_DMEM_READ_AFTER_WRITE_NGRESULT)
                    {
                        Printf("[READ_AFTER_WRITE_PAT]  NG , Result = 0x%x \n",u4Apollo_ReadDram(APOLLO_DMEM_READ_AFTER_WRITE_TEST_RESULT));
                        ASSERT(0);
                    }
                    else
                    {
                        Printf("[READ_AFTER_WRITE_PAT]  ERR  ; The program may be dead!! \n" );
                        ASSERT(0);
                    }
                    
                }
                break;
            case SRAM_DIRTY_BIT_PAT :
              vAproc_Disable();
            	  u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &SRAM_DIRTY_BIT_TABLE[0], SRAM_DIRTY_BIT_SIZE); 
            	  u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1)+0x2000,(UINT32) &SRAM_DIRTY_BIT_DATA[0], SRAM_DIRTY_BIT_DATA_SIZE);            	  
            	  vAproc_Enable();
               // sleep
               x_thread_delay(1000);

            	  if (u1ShowHelp)
                {                
                    if(u4Apollo_ReadDram(APOLLO_DMEM_SRAM_DIRTY_BIT_PAT_RESULT) ==APOLLO_DMEM_SRAM_DIRTY_BIT_PASSRESULT)  //get result
                    {
                        Printf("[SRAM_DIRTY_BIT_PAT]  PASS \n");
                    }
                    else if(u4Apollo_ReadDram(APOLLO_DMEM_SRAM_DIRTY_BIT_PAT_RESULT) ==APOLLO_DMEM_SRAM_DIRTY_BIT_NGRESULT)
                    {
                        Printf("[SRAM_DIRTY_BIT_PAT]  NG , Result = 0x%x \n",u4Apollo_ReadDram(APOLLO_DMEM_SRAM_DIRTY_BIT_TEST_RESULT));
                        ASSERT(0);
                    }
                    else
                    {
                        Printf("[SRAM_DIRTY_BIT_PAT]  ERR  ; The program may be dead!! \n" );
                        ASSERT(0);
                    }
                    
                }
                break;
            case JTAG_LOAD_PAT :
            	  Printf("To do...\n");
                break;
            case JTAG_CONTROL_PAT :
            	  Printf("[DBG_JTAG_CONTROL] Start... \n");
            	  Printf("Note: This is not an auto test. \n");
            	  Printf("Note: Please check the test items manually. \n");
            	  vAproc_Disable();
            	  u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &ADDR_DBG_JTAG_CTRL_TABLE[0], ADDR_DBG_JTAG_CTRL_SIZE); 
            	  vAproc_Enable();

                break;
            case PM_SEQUENTIAL_ACCESS_PAT :
                vApollo_PgSeqTst();
                break;
            case PM_BRANCH_PAT :
                vApollo_PgBranchTst();
                break;
            case CACHE_2WAY_PAT :
                vApollo_2WayTst();
               break;
            case CACHE_4WAY_PAT :
                vApollo_4WayTst();
                break;
            case CACHE_STATISTIC_PAT :
                vApollo_CacheTst();
                break;
         
            case EXT_PM_SADR_PAT :
                vApollo_PmUserAddrTst();
                break;
                
            case INST_TST_PAT :
#if 0            	
            	  vAproc_Disable();
            	  u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_INST_TABLE[0], APOLLO_INST_SIZE); 
            	  u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1),(UINT32) &DATA_APOLLO_INST_TABLE[0], DATA_APOLLO_INST_SIZE);
            	  vAproc_Enable();
            	  if (u1ShowHelp)
                {
                    Printf("[instruction_tst]\n");
                    Printf("  Print you result/note message here\n");
                }
            	  Printf("To be fixed...\n");
#endif
            	  //new
                // A03_control
                printf("\n");
            	  u1TestResult = vApollo_inst_A03_control_tst();
                if (u1TestResult == 1)
                    printf("[INST] A03_control Test: Pass \n");
                else
                    printf("[INST] A03_control Test: Failed \n");

                // A05_do_hs_ri
                printf("\n");
            	  u1TestResult = vApollo_inst_A05_do_hs_ri_tst();
                if (u1TestResult == 1)
                    printf("[INST]A05_do_hs_ri Test: Pass \n");
                else
                    printf("[INST] A05_do_hs_ri Test: Failed \n");

                // A09_div
                printf("\n");
            	  u1TestResult = vApollo_inst_A09_div_tst();
                if (u1TestResult == 1)
                    printf("[INST] A09_div Test: Pass \n");
                else
                    printf("[INST] A09_div Test: Failed \n");

                // C10_apollo_mul
                printf("\n");
            	  u1TestResult = vApollo_inst_C10_apollo_mul_tst();
               if (u1TestResult == 1)
                    printf("[INST] C10_apollo_mul Test: Pass \n");
                else
                    printf("[INST] C10_apollo_mul Test: Failed \n");

                // C11_apollo_satrnd
                printf("\n");
                u1TestResult = vApollo_inst_C11_apollo_satrnd_tst();
                if (u1TestResult == 1)
                    printf("[INST] C11_apollo_satrnd Test: Pass \n");
                else
                    printf("[INST] C11_apollo_satrnd Test: Failed \n");

                // C12_apollo_others
                printf("\n");
                u1TestResult = vApollo_inst_C12_apollo_others_tst();
                if (u1TestResult == 1)
                    printf("[INST] C12_apollo_others Test: Pass \n");
                else
                    printf("[INST] C12_apollo_others Test: Failed \n");
                break;
            case GETBS_TST_PAT :
                printf("\n");
                u1TestResult = vApollo_getbs_function_tst();
                if (u1TestResult == 1)
                    printf("[GETBS] Function Test: Pass \n");
                else
                    printf("[GETBS] Function Test: Failed \n");
                break;
            case GETBS_CRC_PAT :
                printf("\n");
                u1TestResult = vApollo_getbs_crc_tst();
                if (u1TestResult == 1)
                    printf("[GETBS] CRC Test: Pass \n");
                else
                    printf("[GETBS] CRC Test: Failed \n");
                break;
            case GETBS_COUNT_LDZ_PAT :
                printf("\n");
                u1TestResult = vApollo_getbs_cnt_ldz_tst();
                if (u1TestResult == 1)
                    printf("[GETBS] Count Leading Zero Test: Pass \n");
                else
                    printf("[GETBS] Count Leading Zero Test: Failed \n");

                break;
            case PUTBS_TST_PAT :
				  vApollo_Putbits_test();            	 
                break;
            case PC_BranchHis :
            	  vApollo_PCBranchHis_test();
                break;
            case INT_RiscApollo :
				  u1TestResult = vApollo_INT_RISC_Apollo_test(); //test Risc  <-> Apollo Interrupt            
                break;
            case INT_DspApollo :
            	  u1TestResult = vApollo_INT_DSP_Apollo_test(); //test DSP  <-> Apollo Interrupt  
                break;
            case INT_TST_PAT :
                // unaligned DM access INT
                 printf("\n");
            	  u1TestResult = vApollo_INT_unaligned_DM_tst();
                if (u1TestResult == 1)
                    printf("[INT] Unaligned DM Test: Pass \n");
                else
                    printf("[INT] Unaligned DM Test: Failed \n");
            	
            	  // INT_div_0
                printf("\n");
            	  u1TestResult = vApollo_INT_div_by_0_tst();
                if (u1TestResult == 1)
                    printf("[INT] DIV/0 Test: Pass \n");
                else
                    printf("[INT] DIV/0 Test: Failed \n");

                // stack OV exception
                printf("\n");
            	  u1TestResult = vApollo_INT_Stack_OV_Chk_tst();
                if (u1TestResult == 1)
                    printf("[INT] Stack Overflow Check Test: Pass \n");
                else
                    printf("[INT] Stack Overflow Check Test: Failed \n");          

                // Apollo internal timer INT
                printf("\n");
            	  u1TestResult = vApollo_INT_Internal_Timer_tst();
                if (u1TestResult == 1)
                    printf("[INT] Internal Timer Test: Pass \n");
                else
                    printf("[INT] Internal Timer Test: Failed \n");
                

                // Apollo 27MHz timer 0 INT
                printf("\n");
            	  u1TestResult = vApollo_INT_27MHz_Timer0_tst();
                if (u1TestResult == 1)
                    printf("[INT] 27M Timer0 Test: Pass \n");
                else
                    printf("[INT] 27M Timer0 Test: Failed \n");

                // Apollo 27MHz timer 1 INT
                printf("\n");
            	  u1TestResult = vApollo_INT_27MHz_Timer1_tst();
                if (u1TestResult == 1)
                    printf("[INT] 27M Timer1 Test: Pass \n");
                else
                    printf("[INT] 27M Timer1 Test: Failed \n");                

                // Apollo 27MHz timer 2 INT
                printf("\n");
            	  u1TestResult = vApollo_INT_27MHz_Timer2_tst();
                if (u1TestResult == 1)
                    printf("[INT] 27M Timer2 Test: Pass \n");
                else
                    printf("[INT] 27M Timer2 Test: Failed \n");   

                // Nested INT : unaligned DM access
                printf("\n");
            	  u1TestResult = vApollo_INT_Nested_UnalignedDM_tst();
                if (u1TestResult == 1)
                    printf("[INT] Nested Unaligned DM Int Test: Pass \n");
                else
                    printf("[INT] Nested Unaligned DM Int Test: Failed \n");   

                // Nested INT : Div Zero
                printf("\n");
            	  u1TestResult = vApollo_INT_Nested_DivZero_tst();
                if (u1TestResult == 1)
                    printf("[INT] Nested Div/0 Int Test: Pass \n");
                else
                    printf("[INT] Nested Div/0 Int Test: Failed \n");   
                
                // Aout2 INT
                printf("\n");
            	  u1TestResult = vApollo_INT_Aout2_tst();
                if (u1TestResult == 1)
                    printf("[INT] Aout2 Int Test: Pass \n");
                else
                    printf("[INT] Aout2 Int Test: Failed \n");                   

                // ASRC INT
                printf(" \n");
            	  u1TestResult = vApollo_INT_ASRC_tst();
                if (u1TestResult == 1)
                    printf("[INT] ASRC Int Test: Pass \n");
                else
                    printf("[INT] ASRC Int Test: Failed \n");         
                
                break;
            case AENV_PAT :
            	  if(u4Apollo_ReadDram(APOLLO_ANEV_PAT_CONTROL) == APOLLO_ANEV_MMR_TEST)
            	  {
                    vApollo_Aenv_MMR_Access_tst();
            	  }
            	  else if (u4Apollo_ReadDram(APOLLO_ANEV_PAT_CONTROL) == APOLLO_ANEV_AOUT_TEST)
            	  {
                    vApollo_Aenv_Aout_Control_tst();
            	  }
            	  else if (u4Apollo_ReadDram(APOLLO_ANEV_PAT_CONTROL) == APOLLO_ANEV_AOUT_ASRC_TEST)
            	  {
                    vApollo_Aenv_Aout_Asrc_Control_tst();
            	  }
            	   else if (u4Apollo_ReadDram(APOLLO_ANEV_PAT_CONTROL) == APOLLO_ANEV_IEC_RAW_OUT_TEST)
            	  {
                    vApollo_Aenv_IecRaw_Out_Control_tst();
            	  }
            	  else if (u4Apollo_ReadDram(APOLLO_ANEV_PAT_CONTROL) == APOLLO_ANEV_AOUT_TIMER0_TEST)
            	  {
                    vApollo_Aenv_Aout_Timer0_Control_tst();
            	  }
            	  else
            	  {
            	      Printf("Note that there for 4 AENV tests. Set Apollo DM[0x22004] first to verify each items. \n");
                    Printf("if (Apollo DM[0x22004] ==1), MMR registers access is tested. \n");
                    Printf("if (Apollo DM[0x22004] ==2), aout control(2 channel) is tested. \n");
                    Printf("if (Apollo DM[0x22004] ==3), aout + asrc control(2 channel) is tested. \n");
                    Printf("if (Apollo DM[0x22004] ==4), iec raw output control is tested. \n");
                    Printf("if (Apollo DM[0x22004] ==5), aout control + timer0, aout int monitor test. \n");
                    Printf("else, do nothing\n");
            	  }
                break;
            case MP3_TST_PAT :
            	  Printf("To do...\n");
                break;
            case AAC_TST_PAT :
            	vApollo_AACEncTst();
                break;
            case AC3_TST_PAT :
            	  Printf("To do...\n");
                break;
            case MTK_POST_PAT :
            	  Printf("To do...\n");
                break;
            case SXX_POST_PAT :
            	  Printf("To do...\n");
                break;
            case DMA_TST_PAT :
            	  Printf("To do...\n");
                break;
            case RISC_INTF_PAT :
            	  Printf("To do...\n");
                break;
            case APOLLO_HOLD_TST_PAT :
				  vApollo_ApolloHoldTst();            	  
                break;                
            case COMPACT_SAVE_RESTORE:
            	  vAproc_Disable();
            	  u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(0),(UINT32) &APOLLO_CMP_SAVE_RESTORE_TABLE[0], APOLLO_CMP_SAVE_RESTORE); 
            	  u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1),(UINT32) &APOLLO_CMP_SAVE_RESTORE_DATA[0], APOLLO_CMP_SAVE_RESTORE_DATA_SIZE);
            	  //Set Compact start block to dram 0x20000(DSP view)
               vApollo_SetDram(APOLLO_DMEM_COMPACT_SAVE_RESTORE_ADD_SETTING, u4GetApolloWorkBufNonInit(1));
            	  vAproc_Enable();
               // sleep
               x_thread_delay(1000);
            	  if (u1ShowHelp)
                {
                    //DramResult:0x00020004
                    if(u4Apollo_ReadDram(APOLLO_DMEM_COMPACT_SAVE_RESTORE_PAT_RESULT) ==APOLLO_DMEM_COMPACT_SAVE_RESTORE_PASSRESULT)  //get result
                    {
                        Printf("[COMPACT_SAVE_RESTORE]  PASS \n");
                    }
                    else if(u4Apollo_ReadDram(APOLLO_DMEM_COMPACT_SAVE_RESTORE_PAT_RESULT) ==APOLLO_DMEM_COMPACT_SAVE_RESTORE_NGRESULT)  //get result
                    {
                        Printf("[COMPACT_SAVE_RESTORE]  NG , Result = 0x%x \n",u4Apollo_ReadDram(APOLLO_DMEM_COMPACT_SAVE_RESTORE_TEST_RESULT));
                        ASSERT(0);
                    }
                    else
                    {
                        Printf("[COMPACT_SAVE_RESTORE]  ERR  ; The program may be dead!! \n" );
                        ASSERT(0);
                    }
                }
                break; 
            case RISC_DEBUGGER:
                Printf("This test only test the 'aud.a.pr.sr' part, for other apollo probe tests,\n");
                Printf("please refer to the emulation plan document.\n");
                u1TestResult = vApollo_Probe_SR_C10_tst();
                if (u1TestResult == 1)
                    printf("Risc debugger SR(C10) test OK \n");
                else
                    printf("Risc debugger SR(C10) test Failed \n");

                u1TestResult = vApollo_Probe_SR_C11_tst();
                if (u1TestResult == 1)
                    printf("Risc debugger SR(C11) test OK \n");
                else
                    printf("Risc debugger SR(C11) test Failed \n");
                break;                
            case DMEM_OVERNIGHT:
                u4TestRound = 0;
                while(1)
                {
                    printf("\n[DMEM Overnight] Round %d \n", u4TestRound);
                    // DMEM SRAM1 test
                    u1TestResult = vApollo_dmem_sram1_tst();
                    if (u1TestResult == 1)
                        printf("[DMEM] SRAM1 Test: Pass \n");
                    else
                    {
                        printf("[DMEM] SRAM1 Test: Failed \n");
                        break;
                    }

                    // DMEM DRAM1 test
                    printf("\n");
                    u1TestResult = vApollo_dmem_dram1_tst();
                    if (u1TestResult == 1)
                        printf("[DMEM] DRAM1 Test: Pass \n");
                    else
                    {
                        printf("[DMEM] DRAM1 Test: Failed \n");
                        break;
                    }

                    // DMEM DRAM2 test
                    printf("\n");
                    u1TestResult = vApollo_dmem_dram2_tst();
                    if (u1TestResult == 1)
                        printf("[DMEM] DRAM2 Test: Pass \n");
                    else
                    {
                        printf("[DMEM] DRAM2 Test: Failed \n");
                        break;
                    }

                    // DMEM DRAM3 test
                    printf("\n");
                    u1TestResult = vApollo_dmem_dram3_tst();
                    if (u1TestResult == 1)
                        printf("[DMEM] DRAM3 Test: Pass \n");
                    else
                    {
                        printf("[DMEM] DRAM3 Test: Failed \n");
                        break;
                    }

                    // DMEM Mix1 test
                    printf("\n");
                    u1TestResult = vApollo_dmem_mix1_tst();
                    if (u1TestResult == 1)
                        printf("[DMEM] Mix1 Test: Pass \n");
                    else
                    {
                        printf("[DMEM] Mix1 Test: Failed \n");
                        break;
                    }

                    // DMEM Mix2 test
                    printf("\n");
                    u1TestResult = vApollo_dmem_mix2_tst();
                    if (u1TestResult == 1)
                        printf("[DMEM] Mix2 Test: Pass \n");
                    else
                    {
                        printf("[DMEM] Mix2 Test: Failed \n");
                        break;
                    }

                    // DMEM Mix3 test
                    printf("\n");
                    u1TestResult = vApollo_dmem_mix3_tst();
                    if (u1TestResult == 1)
                        printf("[DMEM] Mix3 Test: Pass \n");
                    else
                    {
                        printf("[DMEM] Mix3 Test: Failed \n");
                        break;
                    }

                    printf("\n");
                    u1TestResult = vApollo_addr_conflict_tst();
                    if (u1TestResult == 1)
                        printf("[ADDR_CONFLICT_PAT] Test: Pass \n");
                    else
                    {
                        printf("[ADDR_CONFLICT_PAT] Test: Failed \n");
						break;
                    }	

                    printf("\n");
                    u1TestResult = vApollo_read_after_write_tst();
                    if (u1TestResult == 1)
                        printf("[READ_AFTER_WRITE_PAT] Test: Pass \n");
                    else
                    {
                        printf("[READ_AFTER_WRITE_PAT] Test: Failed \n");
						break;
                    }	

                    printf("\n");
                    u1TestResult = vApollo_sram_dirty_bit_tst();
                    if (u1TestResult == 1)
                        printf("[SRAM_DIRTY_BIT_PAT] Test: Pass \n");
                    else
                    {
                        printf("[SRAM_DIRTY_BIT_PAT] Test: Failed \n");
						break;
                    }	


                    printf("\n");
                    u1TestResult = vApollo_compact_save_restore_tst();
                    if (u1TestResult == 1)
                        printf("[COMPACT_SAVE_RESTORE] Test: Pass \n");
                    else                    
                    {
                        printf("[COMPACT_SAVE_RESTORE] Test: Failed \n");						
						break;
                    }	
                   

                    u4TestRound++;
                }
                break;  
           case ADSP_INTF_OVERNIGHT: //DSP <-> Apollo interrupt overnight
                u4TestRound = 0;
                while(1)
                {
                   printf("\n[DSP <-> Apollo interrupt Overnight] Round %d \n", u4TestRound);
                   u1TestResult = vApollo_INT_DSP_Apollo_test();
				   if (u1TestResult == 1)
                        printf("[DSP <-> Apollo interrupt] Test: Pass \n");
                   else
                   	{
						printf("[DSP <-> Apollo interrupt] Test: Failed \n");
						break;
                   	}
                    
				   u4TestRound++;
                }
				break;

            case PMEM_OVERNIGHT:
                vApollo_OntPmem();
                break;
                
            case PMEM_CACHE_DUMP:
                vApollo_PgCacheDump();
                break;

            case GETBS_DMA:
                printf("\n");
                u1TestResult = vApollo_psrdma_tst();
                if (u1TestResult == 1)
                    printf("[PSRDMA] Function Test: Pass \n");
                else
                    printf("[PSRDMA] Function Test: Failed \n");
                break;

            case TIMER_27MHZ:
                printf("To do...\n");
                break;
                
            case LXX_POST_PAT:     
                printf("\n");
                vApollo_smse_tst();               
                break;              

            default:
                if (u1ShowHelp)
                {                
                Printf("unsupport pattern\n");
                }
                break;
        }
    }
}





#endif
#endif
