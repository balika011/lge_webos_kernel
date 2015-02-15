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
 * $RCSfile: adsp_task.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file adsp_task.c
 *  Brief of file adsp_task.c.
 *  Source file for ADSP related control routine.
 */


//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#include "x_lint.h"
#include "x_ckgen.h"
LINT_EXT_HEADER_BEGIN
#include "x_hal_5381.h"
#include "dsp_common.h"
#include "ctrl_para.h"
#include "adsp_task.h"
#include "dsp_uop.h"
#include "dsp_shm.h"
#include "dsp_intf.h"
#include "dsp_func.h"
#include "dsp_rg_ctl.h"
#include "aud_debug.h"
#include "aud_hw.h"
#include "fbm_drvif.h"  // for DSP memory allocation
#include "drv_common.h"
#include "drv_adsp.h"
#include "drvcust_if.h"
#include "psr_drvif.h"
#include "aud_if.h"

#include "x_os.h"
#include "x_assert.h"

#include "hw_ckgen.h"

#define AUD_STATISTICS

#ifdef AUD_STATISTICS
#include "drv_dbase.h"
#include "x_timer.h"
#endif

#ifdef LINUX_TURNKEY_SOLUTION
#include <linux/module.h>
#endif

LINT_EXT_HEADER_END

//lint -e950 use __align()

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

//#define DSP_DRAM_BLOCK_TEST
#define AUD_STATISTICS

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define _ADSP_ENV_BLOCK       1
#define DSP_WORKING_BUF_ALIGN  0x100
#define MSG_QUEUE_TIME_OUT  1000   // In times of 10 ms => total 10 sec

//-----------------------------------------------------------------------------
// Imported variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Imported functions
//-----------------------------------------------------------------------------

extern BOOL fgTriggerDspErrorLogs(void);
extern BOOL fgGetDspErrorLogs(ADSP_ERROR_LOG_T* ptADSPErrLog);
extern void vDspSetSamplingRate(UINT8 u1DecId);
extern void AUD_DspClkSel(UINT8 u1ADSPclkSel);
extern UINT32 BSP_GetMemSize(void);

#ifdef CC_AUD_APOLLO_SUPPORT
extern void IniBlk6Regs (UINT32 u4ApolloDRAMCpmtStrBlk);

//added by ling
extern void vA2MCommNotifyKernelDone (void);
extern void vA2MCommKernelOpenDone (void);
extern void vA2MCommDec0OpenDone (void);
extern void vA2MCommDec1OpenDone (void);
extern void vA2MCommDec2OpenDone (void);
extern void vA2MCommDec3OpenDone (void);
extern void vA2MCommDec0CloseDone (void);
extern void vA2MCommDec1CloseDone (void);
extern void vA2MCommDec2CloseDone (void);
extern void vA2MCommDec3CloseDone (void);
extern void vA2MCommIoInitDone (void);
extern void vA2MCommAprocOpenDone (void);
extern void vA2MMixSound0UpdateRp(void);//parson mixsnd
extern void vA2MMixSound1UpdateRp(void);
extern void vA2MUploadUpdateWp(void);
extern void vA2MNotifySRC (void);
extern void vA2MNotifyAPTS0(void);
extern void vA2MNotifyAPTS1(void);
extern void vA2MNotifyAPTS2(void);
extern void vA2MNotifyHoldComplete0(void);
extern void vA2MNotifyHoldComplete1(void);
extern void vA2MNotifyHoldComplete2(void);
extern void vA2MNotifyUpdateRountine(void);
extern void AUD_Aproc_Chg_AVSyncCtrl(UINT8 u1DecID, APROC_AVSYNC_CTRL_MODE_T eMode);
extern void vApolloIRQHandler0_ISR(void);
extern void vApolloIRQHandler1_ISR(void);
extern void vApolloIRQHandler2_ISR(void);
extern void vApolloIRQHandler3_ISR(void);
extern void AUD_ApolloClkSel(UINT8 u1ApolloClkSel);
extern void vApolloReg_Write (UINT32 u4Offset, UINT32 u4Val);
#endif

#ifdef CC_AUD_APOLLO_POST_SUPPORT
extern void vA2MCommPostProcOpenDone (void);
extern void vA2MCommPostProcCloseDone (void);
#endif

#ifdef CC_AUD_ARM_SUPPORT
extern void vA2MCommNotifyKernelLog (void);
extern void vA2MCommKernelOpenDone (void);
extern void vA2MCommPostProcOpenDone (void);
extern void vA2MCommPostProcCloseDone (void);
extern void vA2MCommIoInitDone (void);

//added by ling
extern void vA2MCommNotifyKernelDone (void);
extern void vA2MCommDec0OpenDone (void);
extern void vA2MCommDec1OpenDone (void);
extern void vA2MCommDec2OpenDone (void);
extern void vA2MCommDec3OpenDone (void);
extern void vA2MCommDec0CloseDone (void);
extern void vA2MCommDec1CloseDone (void);
extern void vA2MCommDec2CloseDone (void);
extern void vA2MCommDec3CloseDone (void);
extern void vA2MCommIoInitDone (void);
extern void vA2MCommAprocOpenDone (void);
#endif

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

HANDLE_T hDspCmdQueue;
HANDLE_T hDspCmdDataQueue;  //ADSP Data Queue.
HANDLE_T _hAdspThread;
static HANDLE_T _hRdDspSramSema;
HANDLE_T _hSendDspIntSema;

#ifdef AUD_STATISTICS

static UINT32 _u4TriggerPeriod = 2;    // Seconds for Trigger Log
static UINT32 _u4LogPeriod = 3;        // Seconds per message
static PARAM_AUD_T* _prAud = NULL;

#endif  // PSR_STATISTICS

INT8 _i1DspMemBlock = -1;
UINT32 _u4DspMemBufAddr = (UINT32)NULL;
static UINT32 _u4DspMemBufSize = 0;

#if defined(CC_AUD_ARM_SUPPORT) || defined(CC_AUD_APOLLO_SUPPORT)
typedef enum {
    APROC_POSTPROC_CLOSE = 0,
    APROC_POSTPROC_OPEN
} APROC_POSTPROC_STATUS_T;

typedef enum {
    APROC_DEV_CLOSE = 0,
    APROC_DEV_OPEN
} APROC_DEV_FLAG_T;


typedef enum {
    APROC_DEV_DEC0 = 0,
    APROC_DEV_DEC1,
    APROC_DEV_DEC2,
    APROC_DEV_DEC3,
    APROC_DEV_DEC_NUM
} APROC_DEV_DEC_T;


static HANDLE_T _hAprocAccessSema;
static HANDLE_T _hAprocSema;
static HANDLE_T _ahSemaAprocComDone;

static char _gcStringBuf[AUD_APROC_CHAR_LEN] ;

static volatile UINT32 _gu4AprocPostProcControl = APROC_POSTPROC_CLOSE;
static volatile UINT32 _gu4AprocDecControl[APROC_DEV_DEC_NUM] = {APROC_POSTPROC_CLOSE, APROC_POSTPROC_CLOSE, APROC_POSTPROC_CLOSE, APROC_POSTPROC_CLOSE};
static volatile UINT32 _gu4AprocControl = APROC_DEV_CLOSE;

_AUD_APROC_COMM_SIG_T _gu4AudAprocA2MSigInfo[] = {
 {APROC_SIG_IDX_KERNEL, APROC_SIG_CMD_NOTIFY, APROC_SIG_NOTIFY_DONE, 0, (AUD_APROC_FUNC_T) &vA2MCommNotifyKernelDone},
 {APROC_SIG_IDX_KERNEL, APROC_SIG_CMD_OPEN, APROC_COMMAND_DONE, 0, (AUD_APROC_FUNC_T) &vA2MCommKernelOpenDone},
#ifdef CC_AUD_APOLLO_POST_SUPPORT 
 {APROC_SIG_IDX_POSTPROC, APROC_SIG_CMD_OPEN, APROC_COMMAND_DONE, 0, (AUD_APROC_FUNC_T) &vA2MCommPostProcOpenDone},
 {APROC_SIG_IDX_POSTPROC, APROC_SIG_CMD_CLOSE, APROC_COMMAND_DONE, 0, (AUD_APROC_FUNC_T) &vA2MCommPostProcCloseDone},
#endif 
 {APROC_SIG_IDX_DEC0, APROC_SIG_CMD_OPEN, APROC_COMMAND_DONE, 0, (AUD_APROC_FUNC_T) &vA2MCommDec0OpenDone},
 {APROC_SIG_IDX_DEC1, APROC_SIG_CMD_OPEN, APROC_COMMAND_DONE, 0, (AUD_APROC_FUNC_T) &vA2MCommDec1OpenDone},
 {APROC_SIG_IDX_DEC2, APROC_SIG_CMD_OPEN, APROC_COMMAND_DONE, 0, (AUD_APROC_FUNC_T) &vA2MCommDec2OpenDone},
 //{APROC_SIG_IDX_DEC3, APROC_SIG_CMD_OPEN, APROC_COMMAND_DONE, 0, (AUD_APROC_FUNC_T) &vA2MCommDec3OpenDone},
 {APROC_SIG_IDX_DEC0, APROC_SIG_CMD_CLOSE, APROC_COMMAND_DONE, 0, (AUD_APROC_FUNC_T) &vA2MCommDec0CloseDone},
 {APROC_SIG_IDX_DEC1, APROC_SIG_CMD_CLOSE, APROC_COMMAND_DONE, 0, (AUD_APROC_FUNC_T) &vA2MCommDec1CloseDone},
 {APROC_SIG_IDX_DEC2, APROC_SIG_CMD_CLOSE, APROC_COMMAND_DONE, 0, (AUD_APROC_FUNC_T) &vA2MCommDec2CloseDone},
 //{APROC_SIG_IDX_DEC3, APROC_SIG_CMD_CLOSE, APROC_COMMAND_DONE, 0, (AUD_APROC_FUNC_T) &vA2MCommDec3CloseDone},
 {APROC_SIG_IDX_IOBUF, APROC_SIG_CMD_INIT, APROC_COMMAND_DONE, 0, (AUD_APROC_FUNC_T) &vA2MCommIoInitDone},
 {APROC_SIG_IDX_APROC, APROC_SIG_CMD_OPEN, APROC_COMMAND_DONE, 0, (AUD_APROC_FUNC_T) &vA2MCommAprocOpenDone},
 //{APROC_SIG_IDX_MIXSND0, APROC_SIG_CMD_NOTIFY, APROC_SIG_NOTIFY_UPDATE_RP, 0, (AUD_APROC_FUNC_T) &vA2MMixSound0UpdateRp},  //parson mixsnd
 //{APROC_SIG_IDX_MIXSND1, APROC_SIG_CMD_NOTIFY, APROC_SIG_NOTIFY_UPDATE_RP, 0, (AUD_APROC_FUNC_T) &vA2MMixSound1UpdateRp},
 //{APROC_SIG_IDX_UPLOAD, APROC_SIG_CMD_NOTIFY, APROC_SIG_NOTIFY_UPDATE_WP, 0, (AUD_APROC_FUNC_T) &vA2MUploadUpdateWp}, 
 //{APROC_SIG_IDX_SRC, APROC_SIG_CMD_NOTIFY, APROC_SIG_NOTIFY_SRC, 0, (AUD_APROC_FUNC_T) &vA2MNotifySRC},
 //{APROC_SIG_IDX_POSTPROC, APROC_SIG_CMD_NOTIFY, APROC_SIG_NOTIFY_MCPU_UPDATE_ROUNTIE, 0, (AUD_APROC_FUNC_T) &vA2MNotifyUpdateRountine},
 
 
 {0, 0, 0, 0, 0}
};
#endif

#ifdef CC_AUD_APOLLO_SUPPORT
UINT32 _u4Apollo2RiscRountCount=1;
#endif

#if   defined(DSP_NEW_SCRAMBLE) 
static BOOL fgCheckSumAll = FALSE;
#endif

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define DSP_MEM_BLOCK(u4Addr)   (PHYSICAL(u4Addr)/DSP_BUF_BLOCK)

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
BOOL _fgTaskInit = FALSE;

void vSendADSPTaskMessage(UINT32 u4Msg);
void vSendADSPTaskMessageNoWait(UINT32 u4Msg);
void vADSPTaskMain_Init(void);

UINT32 _AudAlign(UINT32 u4Addr, UINT32 u4Alignment)
{
    UINT32 u4Unaligned;

    if (u4Alignment <= 1)
    {
        return u4Addr;
    }

    u4Unaligned = u4Addr % u4Alignment;
    if (u4Unaligned != 0)
    {
        u4Addr += u4Alignment - u4Unaligned;
    }

    return u4Addr;
}

//-----------------------------------------------------------------------------
//  fgDspGetDspMem
//
/** Brief: Get DSP memory address and size
 *  @param
 *  @retval
 */
//-----------------------------------------------------------------------------
BOOL fgDspGetDspMem(UINT32* pu4Addr, UINT32* pu4Size)
{
    if ((pu4Addr != NULL) && (pu4Size != NULL) && (_u4DspMemBufAddr != (UINT32)NULL))
    {
        *pu4Addr = _u4DspMemBufAddr;
        *pu4Size = _u4DspMemBufSize;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//-----------------------------------------------------------------------------
//  u4DspInternalLogicalAddress
//
/** Brief: DSP inside logical address (0 ~ 16 MB)
 *  @param
 *  @retval
 */
//-----------------------------------------------------------------------------
UINT32 u4DspInternalLogicalAddress(UINT32 u4Addr)
{
    // Check if memory is initilized
    VERIFY(_i1DspMemBlock >= 0);
    return (u4Addr & (DSP_BUF_BLOCK-1));
}


//-----------------------------------------------------------------------------
//  u4DspPhysicalAddress
//
/** Brief: Physical address (0 ~ 64 MB)
 *  @param
 *  @retval
 */
//-----------------------------------------------------------------------------
UINT32 u4DspPhysicalAddress(UINT32 u4Addr)
{
    UINT32 u4MemSize;

    // Check if memory is initialized
    VERIFY(_i1DspMemBlock >= 0);

    // Check 256 MB boundary
    u4MemSize = BSP_GetMemSize();
    VERIFY(PHYSICAL(u4Addr / u4MemSize) == 0);

    // If within 16 MB
    if ((PHYSICAL(u4Addr) / DSP_BUF_BLOCK) == 0)
    {
        return (PHYSICAL(u4Addr) + (UINT32)((_i1DspMemBlock)*DSP_BUF_BLOCK));
    }
    else if (((PHYSICAL(u4Addr)/DSP_BUF_BLOCK) > 0) &&
            ((PHYSICAL(u4Addr)/DSP_BUF_BLOCK) < 128))    // If within 16 ~ 2048 MB
    {
        return PHYSICAL(u4Addr);
    }
    else
    {
        ASSERT(0);
        LINT_SUPPRESS_NEXT_EXPRESSION(527);
        return PHYSICAL(u4Addr);
    }
}

//-----------------------------------------------------------------------------
//  u4DspVirtualAddress
//
/** Brief: Virtual address (0 ~ 64 MB | 0x30000000)
 *  @param
 *  @retval
 */
//-----------------------------------------------------------------------------
UINT32 u4DspVirtualAddress(UINT32 u4Addr)
{
    return VIRTUAL(u4DspPhysicalAddress(u4Addr));
}

#ifdef DSP_TEST_DRAM_BLOCK
__align(0x100) static UINT8 _au1AdspWorkingBufferUnAligned[DSP_WORK_BUF_SIZE + DSP_WORKING_BUF_ALIGN] ;
#endif

#if defined(CC_AUD_ARM_SUPPORT) || defined(CC_AUD_APOLLO_SUPPORT)
static UINT32 u4AudDspWorkBufBase = 0;
UINT32 u4GetDspWorkBufNonInit(void)
{
    ASSERT (u4AudDspWorkBufBase != (UINT32)NULL); // ADSP working buffer has not been not initialized

    return u4AudDspWorkBufBase;
}

UINT32 u4GetAprocMemoryMap (UINT32 u4Type)
{
    UINT32 u4Addr;

    u4Addr = _gu4AprocMemMap[u4Type];

    ASSERT (u4Addr != (UINT32)NULL); // Audio processor memory has not been not initialized

    return u4Addr;
}
#endif

#ifdef CC_AUD_APOLLO_SUPPORT
UINT32 u4GetApolloWorkBufNonInit(UINT32 u4Type)
{
    UINT32 u4Addr;
    u4Addr = _gu4ApolloMemMap[u4Type];
    ASSERT (u4Addr != (UINT32)NULL); // Audio processor memory has not been not initialized
    
    return u4Addr;
}

#endif

//-----------------------------------------------------------------------------
//  u4GetDspWorkBuf
//
/** Brief:  Allocate DSP working buffer from FB pool
 *  @param
 *  @retval
 */
//-----------------------------------------------------------------------------
UINT32 u4GetDspWorkBuf(void)
{
#ifndef DSP_TEST_DRAM_BLOCK
    FBM_POOL_T* prFbmPool;
#endif
    UINT32  u4BufAddr;
    UINT32  u4BufSize;
    UINT32  u4MemSize;

#ifndef DSP_TEST_DRAM_BLOCK
    // Get buffer from FB pool
    prFbmPool = FBM_GetPoolInfo((UINT8) FBM_POOL_TYPE_DSP);

    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != (UINT32)NULL);
    u4BufAddr= prFbmPool->u4Addr;
    u4BufSize = prFbmPool->u4Size;
    _u4DspMemBufAddr = u4BufAddr;
    _u4DspMemBufSize = u4BufSize;
#else
    UINT32 u4TmpAddr;
    u4TmpAddr = _AudAlign((UINT32)_au1AdspWorkingBufferUnAligned,0x100);
    u4BufAddr = u4TmpAddr + (64*1024*1024);
    u4BufSize = DSP_WORK_BUF_SIZE;
    _u4DspMemBufAddr = u4BufAddr;
    _u4DspMemBufSize = u4BufSize;
#endif

    // Check 256 Bytes alignment
    VERIFY(_AudAlign(VIRTUAL(u4BufAddr), 0x100) == VIRTUAL(u4BufAddr));

    // Check 64 MB boundary
    u4MemSize = BSP_GetMemSize();
    VERIFY(PHYSICAL((u4BufAddr + u4BufSize) / u4MemSize) == 0);

    // Check if start and end address are in the same 16 MB boundary
    VERIFY(DSP_MEM_BLOCK(u4BufAddr) == DSP_MEM_BLOCK(u4BufAddr + u4BufSize));

    // Setup memory block to register
    _i1DspMemBlock = (INT8)DSP_MEM_BLOCK(u4BufAddr);
    if (DSP_SetAudDramBlock((UINT8)_i1DspMemBlock))
    {
        ASSERT(0);
    }

    // Clear memory

#ifndef DSP_TEST_DRAM_BLOCK
    x_memset((VOID *)VIRTUAL(u4BufAddr), 0, prFbmPool->u4Size);
    DSP_FlushInvalidateDCacheFree(u4BufAddr, prFbmPool->u4Size);
#else
    x_memset((VOID *)VIRTUAL(u4BufAddr), 0, u4BufSize);
#endif


#if (defined(CC_AUD_ARM_SUPPORT) || defined(CC_AUD_APOLLO_SUPPORT))
    u4AudDspWorkBufBase = VIRTUAL(u4BufAddr); // init AUD DSP working buffer base
#endif

    return VIRTUAL(u4BufAddr);
}

/******************************************************************************
* Function      : u4GetAFIFOStart
* Description   : get audio FIFO start address in physical address
* Parameter     : uDecIndex: 0: first decoder,1: second decoder
* Return        : None
******************************************************************************/
UINT32 u4GetAFIFOStart(UINT8 uDecIndex)
{
#ifdef CC_MT5391_AUD_3_DECODER
    #ifdef CC_AUD_DEC3_USE_FIFO1// USE_FIFO3 : dec3 use fifo3
    if (uDecIndex == AUD_DEC_THIRD)
    {
        // Audio description uses AFIFO
        uDecIndex = AUD_DEC_MAIN;
    }
    #endif
#endif

    return _u4AFIFO[uDecIndex][0];
}

/******************************************************************************
* Function      : u4GetAFIFOEnd
* Description   : get audio FIFO end address
* Parameter     : uDecIndex: 0: first decoder,1: second decoder
* Return        : None
******************************************************************************/
UINT32 u4GetAFIFOEnd(UINT8 uDecIndex)
{
#ifdef CC_MT5391_AUD_3_DECODER
    #ifdef CC_AUD_DEC3_USE_FIFO1// USE_FIFO3 : dec3 use fifo3
    if (uDecIndex == AUD_DEC_THIRD)
    {
        // Audio description uses AFIFO
        uDecIndex = AUD_DEC_MAIN;
    }
    #endif
#endif

    return _u4AFIFO[uDecIndex][1];
}

#if defined(ALSA_PCMDEC_PATH) || defined(CC_ENABLE_AOMX) || defined(LINUX_TURNKEY_SOLUTION) && defined(CC_ENABLE_ADECOMX)
UINT32 u4GetABufPnt(UINT8 uDecIndex)
{
    UINT32 u4Reg;

    if (uDecIndex == AUD_DEC_MAIN)
    {
        u4Reg = REG_ABUF_PNT;
    }
    else if (uDecIndex == AUD_DEC_AUX)
    {
        u4Reg = REG_ABUF_PNT_DEC2;
    }
    else if (uDecIndex == AUD_DEC_THIRD)
    {
        u4Reg = REG_ABUF_PNT_DEC3;
    }
    else
    {
        u4Reg = REG_ABUF_PNT_DEC4;
    }

    AUD_WRITE32(u4Reg,0);
    return ((_u4AFIFO[uDecIndex][0]&0xff000000) | AUD_READ32(u4Reg));
}
#endif

UINT32 u4GetAWritePnt(UINT8 uDecIndex)
{
    if (uDecIndex == AUD_DEC_MAIN)
    {
        AUD_WRITE32(REG_ABUF_PNT,1);
        return ((_u4AFIFO[0][0]&0xff000000) | AUD_READ32(REG_ABUF_PNT));
    }
    else
    {
        AUD_WRITE32(REG_ABUF_PNT_DEC2,1);
        return ((_u4AFIFO[1][0]&0xff000000) | AUD_READ32(REG_ABUF_PNT_DEC2));
    }
}

extern BOOL PSR_RiscSetAudioWp(UINT8 u1DeviceId, UINT32 u4WritePointer);
#if defined(ALSA_PCMDEC_PATH) || defined(CC_ENABLE_AOMX) || defined(LINUX_TURNKEY_SOLUTION) && defined(CC_ENABLE_ADECOMX)
void vSetAWritePnt(UINT8 uDecIndex, UINT32 u4WritePointer)
{
    if (uDecIndex == AUD_DEC_MAIN)
{
    PSR_RiscSetAudioWp(AUD_DEC_MAIN, u4WritePointer);
}
    else if (uDecIndex == AUD_DEC_AUX) //AOMX2_TEST_TODO
    {
        AUD_WRITE32(REG_DMX_WRITE_PNT, u4WritePointer);
    }
    else if (uDecIndex == AUD_DEC_THIRD)
    {
        AUD_WRITE32(REG_USER0_WP, u4WritePointer);
    }
    else
    {
        AUD_WRITE32(REG_USER1_WP, u4WritePointer);
    }
}
#endif

#ifdef CC_AUD_HDMI_PARSER_2_0_SUPPORT
/******************************************************************************
* Function      : u4GetHDMIParserFIFOStart
* Description   : get HDMI parser FIFO start address in physical address
* Parameter     : void
* Return        : HDMI parser FIFO start address
******************************************************************************/
UINT32 u4GetHDMIParserFIFOStart(void)
{
    return _u4AFIFOHdmiParser[0];
}

/******************************************************************************
* Function      : u4GetHDMIParserFIFOEnd
* Description   : get HDMI parser FIFO end address in physical address
* Parameter     : void
* Return        : HDMI parser FIFO end address
******************************************************************************/
UINT32 u4GetHDMIParserFIFOEnd(void)
{
    return _u4AFIFOHdmiParser[1];
}
#endif

#ifdef CC_AUD_4_DECODER_SUPPORT
UINT32 u4GetSBCEncFIFOStart(void)
{
    return _u4SBCEncBuf[0];
}

UINT32 u4GetSBCEncFIFOEnd(void)
{
    return _u4SBCEncBuf[1];
}
UINT32 u4GetSBCEncWritePnt(void)
{
    DSP_FlushInvalidateDCache(CACHE_FLUSH_TYPE_DATA, TYPE_COMM_DRAM_IDX);
    return ((_u4SBCEncBuf[0]&0xff000000) | dReadDspCommDram(ADDR_RC2D_SBCE_WRITE_PTR_DEC4));
}
#endif

//#ifdef ENABLE_AUDIO_RECORD_SUPPORT
UINT32 u4GetUploadWritePnt(void)
{
    DSP_FlushInvalidateDCacheSmall2(ADDR_D2RC_UPLOAD_WRITE_PTR);
    return ((_u4DspCmptBuf[TYPE_COMM_DRAM_IDX] & 0xff000000) | dReadDspCommDram(ADDR_D2RC_UPLOAD_WRITE_PTR));
}

UINT32 u4GetUploadFIFOStart(void)
{
    return (_u4DspCmptBuf[TYPE_COMM_DRAM_IDX] + AUD_UPLOAD_BUFFER_BASE);
}

UINT32 u4AUD_UPLOAD_BUFFER_SIZE = AUD_UPLOAD_BUFFER_SIZE;
UINT32 u4GetUploadFIFOEnd(void)
{
    return (_u4DspCmptBuf[TYPE_COMM_DRAM_IDX] + AUD_UPLOAD_BUFFER_BASE + u4AUD_UPLOAD_BUFFER_SIZE);
}

//#endif
#ifdef CC_SUPPORT_BLUETOOTH_UPLOAD
UINT32 u4GetBluetoothFIFOStart(void)
{
    return (_u4DspCmptBuf[TYPE_COMM_DRAM_IDX] + AUD_BLUETOOTH_BUFFER_BASE);
}

UINT32 u4GetBluetoothFIFOEnd(void)
{
    return (_u4DspCmptBuf[TYPE_COMM_DRAM_IDX] + AUD_BLUETOOTH_BUFFER_BASE + AUD_BLUETOOTH_BUFFER_SIZE);
}
UINT32 u4GetBluetoothWritePnt(void)
{
    DSP_FlushInvalidateDCache(CACHE_FLUSH_TYPE_DATA, TYPE_COMM_DRAM_IDX);
    //DSP_FlushInvalidateDCacheSmall2(ADDR_RC2D_UPLOAD_WRITE_PTR);
    return ((_u4DspCmptBuf[TYPE_COMM_DRAM_IDX]&0xff000000) | dReadDspCommDram(ADDR_RC2D_BLUETOOTH_WRITE_PTR));
}
#endif

void vSetUploadReadPtr(UINT32 u4ReadPtr)
{
    WriteDspCommDram(ADDR_RC2D_UPLOAD_READ_PTR,(u4ReadPtr&0xffffff));
}

/******************************************************************************
* Function      : vAdspTaskMain
* Description   : main routine for ADSP Task
* Parameter     : None
* Return        : None
******************************************************************************/
//lint -e{818}
static void vADSPTaskMain(VOID* pvArg)
{
    UINT32 u4Msg;
    UINT16 u2MsgIdx;
    BOOL fgGetRet;

    VERIFY(pvArg == NULL);

    // Move initialzation relatrd code to vADSPTaskMainInit. To make sure the AUD_Init flow correctness.

    while (1)
    {
        SIZE_T zMsgSize = sizeof(UINT32);
        INT32  i4MsgRet;

        i4MsgRet = x_msg_q_receive(&u2MsgIdx,&u4Msg, &zMsgSize, &hDspCmdQueue, 1, X_MSGQ_OPTION_WAIT); // Light: Set queue as wait type

        VERIFY((i4MsgRet == OSR_OK) || (i4MsgRet == OSR_NO_MSG));

#ifdef ADSP_CHECK_RAM_CODE
        if (_rDspStatus[AUD_DEC_MAIN].fgDspWakeUpOk)
        {
            LOG(7, "[DSP]========= Check RAM Start =========\n");

            if (fgCheckDspData())
            {
                LOG(7, "[DSP] RAM check pass\n");
            }
            else
            {
                LOG(7, "[DSP] Error: RAM check fail\n");
            }

            LOG(7, "[DSP]========== Check RAM End ==========\n");
        }
#endif

        if (i4MsgRet == OSR_OK)
        {
            switch(u4Msg & 0xFFFF)
            {
            case ADSPTASK_MSG_INTERRUPT_DATA:
                vDspIntSvc_Data();  //ADSP Data Queue.
                break;
            case ADSPTASK_MSG_POWER_ON:
                // set to power initial state
                vDspStateInit(); // initialization for ADSP state machine
                fgGetRet = fgDspInitState (); // Light: wait for DSP wakeup in fgDspInitState
                if (fgGetRet)
                {
                    _uDspState = (UINT8)DSP_CK_INT;
                }
                else // wakeup isn't finished...
                {
                    LOG(1, "DSP wake up fail\n");
                    ASSERT(0);
                }

                LOG(9 , "ADSPTASK_MSG_POWER_ON\n");
//modify by ling                
//#ifdef CC_AUD_ARM_SUPPORT // fix me!!!!
#if 0 //waiting for APROC READY!
                // open aproc output
                // aproc output registers are switched by adsp kernel code
                // aproc output must be opened after adsp kernel ready
                if (bIsSupportAproc () == TRUE)
                {
                    while (1)
                    {
                        // wait aproc power-on
                        if (u4Aproc_IsEnable())
                        {
                            break;
                        }
                        x_thread_delay (1);
                    }
                }
                vAproc_Open ();
#endif                
                
                break;
            case ADSPTASK_MSG_POWER_OFF:
                if (_rDspStatus[AUD_DEC_MAIN].fgDspPlay)
                {
                    vDspCmd(DSP_STOP);
                }
                if (_rDspStatus[AUD_DEC_AUX].fgDspPlay)
                {
                    vDspCmd(DSP_STOP_DEC2);
                }
#ifdef CC_MT5391_AUD_3_DECODER
                if (_rDspStatus[AUD_DEC_THIRD].fgDspPlay)
                {
                    vDspCmd(DSP_STOP_DEC3);
                }
#endif
#ifdef CC_AUD_4_DECODER_SUPPORT
                if (_rDspStatus[AUD_DEC_4TH].fgDspPlay)
                {
                    vDspCmd(DSP_STOP_DEC4);
                }
#endif
                _uDspState = (UINT8)DSP_WAIT_POWER_OFF;
                LOG(9 , "ADSPTASK_MSG_POWER_OFF\n");
                break;
            case ADSPTASK_MSG_INT_SAMPLE_RATE:
                vDspSetSamplingRate(AUD_DEC_MAIN);
                vDspFlowControlNotify(D2RC_FLOW_CONTROL_SAMPLING_RATE);
                break;
            case ADSPTASK_MSG_INT_SAMPLE_RATE_DEC2:
                vDspSetSamplingRate(AUD_DEC_AUX);
                vDspFlowControlNotify(D2RC_FLOW_CONTROL_SAMPLING_RATE_DEC2);
                break;
            case ADSPTASK_MSG_INT_AOUT_ENABLE:
                vDspFlowControlNotify(D2RC_FLOW_CONTROL_AOUT_ENABLE);
                break;
            case ADSPTASK_MSG_INT_AOUT_ENABLE_DEC2:
                vDspFlowControlNotify(D2RC_FLOW_CONTROL_AOUT_ENABLE_DEC2);
                break;
            case ADSPTASK_MSG_INT_STOP:
                vDspFlowControlNotify(D2RC_FLOW_CONTROL_FLUSH_DONE);
        #ifdef CC_AUD_EFFECT_MUTE_PROTECT
                vDspEffectReInitUnMuteSurround();
                vDspEffectReInitUnMuteVBass();      
        #endif
                break;
            case ADSPTASK_MSG_INT_STOP_DEC2:
                vDspFlowControlNotify(D2RC_FLOW_CONTROL_FLUSH_DONE_DEC2);
                break;
            case ADSPTASK_MSG_INT_MODE_CHANGE:
                vDspFlowControlNotify(D2RC_FLOW_CONTROL_MODE_CHANGE);
                break;
            case ADSPTASK_MSG_INT_MODE_CHANGE_DEC2:
                vDspFlowControlNotify(D2RC_FLOW_CONTROL_MODE_CHANGE_DEC2);
                break;
            case ADSPTASK_MSG_INT_PAL_MODE:
                vDspDemodDetModeNotify(INT_DSP_PAL_DETECTED_MODE, u4Msg>>16);
                break;
            case ADSPTASK_MSG_INT_JPN_MODE:
                vDspDemodDetModeNotify(INT_DSP_JPN_DETECTED_MODE, u4Msg>>16);
                break;
            case ADSPTASK_MSG_INT_MTS_MODE:
                vDspDemodDetModeNotify(INT_DSP_MTS_DETECTED_MODE, u4Msg>>16);
                break;
            case ADSPTASK_MSG_INT_DETECTOR_NOTIFY:
                vDspTvSysDetectedNotify(AUD_DEC_MAIN, (TV_AUD_SYS_T)(u4Msg>>16));
                break;
            case ADSPTASK_MSG_INT_DETECTOR_DEC2_NOTIFY:
                vDspTvSysDetectedNotify(AUD_DEC_AUX, (TV_AUD_SYS_T)(u4Msg>>16));
                break;
            case ADSPTASK_MSG_INT_DETECTOR_DEC3_NOTIFY:
                vDspTvSysDetectedNotify(AUD_DEC_THIRD, (TV_AUD_SYS_T)(u4Msg>>16));
                break;                
            case ADSPTASK_MSG_INT_ATV_CHANGE:
                vDspTvSysChangeNotify((TV_AUD_SYS_T)(u4Msg>>16));
                break;
            case ADSPTASK_MSG_INT_ATV_HDEV_SWITCH:
                //vDspDemodDetModeNotify(INT_DSP_HDEV_AUTO_SWITCH, u4Msg>>16);
                vDspHdevModeChangeNotify(u4Msg>>16);
                break;
            case ADSPTASK_MSG_INT_FM_RADIO_DET:
                vDspFMRadioDetectionNotify((AUD_FM_RADIO_DET_T)(u4Msg>>16));
                break;
            case ADSPTASK_MSG_INT_MINER_NOTIFY:
                vDspDemodDetModeNotify(INT_DSP_MINER_NOTIFY, u4Msg>>16);
                break;
            case ADSPTASK_MSG_INT_UPDATE_EFFECT:
                vDspUpdatePostEffect();
                break;
        #ifdef CC_AUD_EFFECT_MUTE_PROTECT
            case ADSPTASK_MSG_INT_UPDATE_EFFECT_END:
                vDspEffectReInitUnMuteSurround();
                vDspEffectReInitUnMuteVBass();
                break;                   
        #endif
#ifdef CC_MT5391_AUD_3_DECODER
            case ADSPTASK_MSG_INT_SAMPLE_RATE_DEC3:
                vDspSetSamplingRate(AUD_DEC_THIRD);
                vDspFlowControlNotify(D2RC_FLOW_CONTROL_SAMPLING_RATE_DEC3);
                break;
            case ADSPTASK_MSG_INT_AOUT_ENABLE_DEC3:
                vDspFlowControlNotify(D2RC_FLOW_CONTROL_AOUT_ENABLE_DEC3);
                break;
            case ADSPTASK_MSG_INT_STOP_DEC3:
                vDspFlowControlNotify(D2RC_FLOW_CONTROL_FLUSH_DONE_DEC3);
                break;
            case ADSPTASK_MSG_INT_MODE_CHANGE_DEC3:
                vDspFlowControlNotify(D2RC_FLOW_CONTROL_MODE_CHANGE_DEC3);
                break;
#endif
            case ADSPTASK_MSG_INT_SAMPLING_RATE_CHANGE:
                vDspFlowControlNotify(D2RC_FLOW_CONTROL_SAMPLING_RATE_CHANGE);
                break;
            case ADSPTASK_MSG_INT_SAMPLING_RATE_CHANGE_DEC2:
                vDspFlowControlNotify(D2RC_FLOW_CONTROL_SAMPLING_RATE_CHANGE_DEC2);
                break;
#ifdef CC_AUD_4_DECODER_SUPPORT
            case ADSPTASK_MSG_INT_SAMPLE_RATE_DEC4:
                vDspSetSamplingRate(AUD_DEC_4TH);
                vDspFlowControlNotify(D2RC_FLOW_CONTROL_SAMPLING_RATE_DEC4);
                break;
            case ADSPTASK_MSG_INT_AOUT_ENABLE_DEC4:
                vDspFlowControlNotify(D2RC_FLOW_CONTROL_AOUT_ENABLE_DEC4);
                break;
            case ADSPTASK_MSG_INT_STOP_DEC4:
                vDspFlowControlNotify(D2RC_FLOW_CONTROL_FLUSH_DONE_DEC4);
                break;
            case ADSPTASK_MSG_INT_MODE_CHANGE_DEC4:
                vDspFlowControlNotify(D2RC_FLOW_CONTROL_MODE_CHANGE_DEC4);
                break;
#endif
            default:
                break;
            }

            vDspState(u4Msg);

        }
    }
}

#ifdef DSP_FORCE_RESET_ENABLE
/******************************************************************************
* Function      : u4ADSP_DspReset
* Description   : Reset DSP state and registers, called by DSP monitor thread.
* Parameter     : None
* Return        : None
******************************************************************************/
INT32 u4ADSP_DspReset(void) //  -- DSP Force --
{
    BOOL    fgGetRet;
    INT32  i4MsgRet = 0;

    vDspStateInit(); // initialization for ADSP state machine

    fgGetRet = fgDspInitState ();
    if (fgGetRet)
    {
        _uDspState = (UINT8)DSP_CK_INT;
    }
    else // wakeup isn't finished...
    {
        LOG(0, "DSP wake up fail\n");
        ASSERT(0);
    }

    LOG(1, "DSP ack ......\n");

    return i4MsgRet;
}
#endif

void vSendADSPTaskMessage(UINT32 u4Msg)
{
    INT32 i4Ret;
    INT32 i4MsgOverFlowCnt = 0;

    do
    {
        i4Ret = x_msg_q_send(hDspCmdQueue,&u4Msg,sizeof(UINT32),1);
        VERIFY((i4Ret == OSR_OK) || (i4Ret == OSR_TOO_MANY));
        if (i4Ret == OSR_TOO_MANY)
        {
            x_thread_delay(10);
            i4MsgOverFlowCnt ++;
            if (i4MsgOverFlowCnt >= MSG_QUEUE_TIME_OUT)
            {
                LOG(1, "Audio message queue over flow\n");
        #ifndef DEMO_BOARD                
                ASSERT(0);
        #endif
            }
        }
    } while(i4Ret != OSR_OK);
}

void vSendADSPTaskMessageNoWait(UINT32 u4Msg)
{
    INT32 i4Ret;

    i4Ret = x_msg_q_send(hDspCmdQueue,&u4Msg,sizeof(UINT32),1);
    if (i4Ret != OSR_OK)
    {
        LOG(1, "Audio message queue over flow\n");
    #ifndef DEMO_BOARD        
        ASSERT(0);
    #endif
    }
}

void vSendADSPTaskData(UINT32 u4Msg) //ADSP Data Queue.
{
    INT32 i4Ret;

    i4Ret = x_msg_q_send(hDspCmdDataQueue,&u4Msg,sizeof(UINT32),1);
    if (i4Ret != OSR_OK)
    {
        LOG(1, "DSP Data queue over flow\n");
    #ifndef DEMO_BOARD        
        ASSERT(0);
    #endif
    }
}

void vGetADSPTaskData(UINT32 *u4Msg) //ADSP Data Queue.
{
    INT32 i4Ret;
    UINT16 u2MsgIdx;
    SIZE_T zMsgSize = sizeof(UINT32);

    i4Ret = x_msg_q_receive(&u2MsgIdx, u4Msg, &zMsgSize, &hDspCmdDataQueue, 1, X_MSGQ_OPTION_WAIT);
    if (i4Ret != OSR_OK)  //Should follow "Put 1 cmd, put 1 data", so there is no case the that "No Data"
    {
        LOG(1, "DSP Data queue no data\n");
    #ifndef DEMO_BOARD        
        ASSERT(0);
    #endif
    }
}

#ifdef AUD_STATISTICS

/* Set error log period in Second */
void SetAudLogPeriod(UINT32 u4Period)
{
    if (u4Period == 0) // no log
    {
        _u4TriggerPeriod = 0;
        _u4LogPeriod = 0;
    }
    else if (u4Period == 1) // _u4TriggerPeriod at lease 1 sec
    {
        _u4TriggerPeriod = 1;
        _u4LogPeriod = 2;
    }
    else // u4Period >= 2
    {
        _u4TriggerPeriod = u4Period - 1;
        _u4LogPeriod = u4Period;
    }
}

//lint -e{727}
//lint -e{830}
//lint -e{550}
BOOL fgAudStatistics(void)
{
    static HAL_TIME_T _rStartTime;
    static HAL_TIME_T _rLastTime;
    HAL_TIME_T rCurTime = {0};
    BOOL fgRet = TRUE;
    static UINT8 uLogState = 0;
    static UINT16 u2TriggerCnt = 0;
    static UINT16 u2LogCnt = 0;
    static UINT16 u2ErrorCnt = 0;

#ifdef CC_AUD_4_DECODER_SUPPORT
    if (fgDecoderRealPlay(AUD_DEC_MAIN) || fgDecoderRealPlay(AUD_DEC_AUX) ||
        fgDecoderRealPlay(AUD_DEC_THIRD) || fgDecoderRealPlay(AUD_DEC_4TH))
#elif defined(CC_MT5391_AUD_3_DECODER)
    if (fgDecoderRealPlay(AUD_DEC_MAIN) || fgDecoderRealPlay(AUD_DEC_AUX) || fgDecoderRealPlay(AUD_DEC_THIRD))
#else
    if (fgDecoderRealPlay(AUD_DEC_MAIN) || fgDecoderRealPlay(AUD_DEC_AUX))
#endif
    {
        if ((_rStartTime.u4Seconds == 0) && (_rStartTime.u4Micros == 0))
        {
            HAL_GetTime(&_rStartTime);
            _rLastTime = _rStartTime;
        }

        HAL_GetTime(&rCurTime);
        // trigger log at 2nd second
        if ((uLogState == 0) && ( (rCurTime.u4Seconds - _rLastTime.u4Seconds) >= _u4TriggerPeriod))
        {
            fgRet = fgTriggerDspErrorLogs();
            u2TriggerCnt ++;
            if (!fgRet)
            {
                LOG(7, "fgTriggerDspErrorLogs Time out (Dsp Busy)\n");
            }
            else
            {
                uLogState = 1;         // switch to get state
            }
        }
        // get log at 3rd second
        if ((uLogState == 1) && ( (rCurTime.u4Seconds - _rLastTime.u4Seconds) >= _u4LogPeriod))
        {
            // Get DSP Previous Error Logs
            fgRet = fgGetDspErrorLogs((ADSP_ERROR_LOG_T *)_prAud);
            if (fgRet)
            {
                u2LogCnt++;            // DSP data ready
                uLogState = 0;         // switch to trigger state
                _rLastTime = rCurTime;
            }
            else
            {
                u2ErrorCnt ++;                      // DSP data not ready
                LOG(7, "fgGetDspErrorLogs Time out %d (Command Loss)\n", u2ErrorCnt);

                // If Command loss over 3 times, switch to trigger state
                if ((u2ErrorCnt % 3) == 0)
                {
                     uLogState = 0;  // switch to trigger state
                }
            }
        }
    }
    return fgRet;
}
#endif

static void vADSPIRQHandler(UINT16 u2Vector)
{
    UINT32 u4Msg;
    UINT32 u4Dsp2RiscInt;

    u4Dsp2RiscInt = AUD_READ32(REG_RISC_INT);
    if ((u4Dsp2RiscInt & DSPA2RC_INTR) != 0)
    {
        AUD_SET_BIT(REG_RISC_INT, DSPA2RC_INTR);
    }

    u4Msg = u4DspIRQSvc();
    //vSendADSPTaskMessage(u4Msg);
    vSendADSPTaskMessageNoWait(u4Msg);
    UNUSED(u2Vector);
}

#if 0   // Unused
void vADSPIRQEnable(BOOL fgEnable)
{
    BOOL u1Ret;
    // Enable ADSP interrupt
    if(fgEnable)
    {
        u1Ret = BIM_EnableIrq(VECTOR_AUDIO);
    }
    else
    {
        u1Ret = BIM_DisableIrq(VECTOR_AUDIO);
    }
    UNUSED(u1Ret);
}
#endif

void CheckSumAll(void);   

#ifdef CC_AUD_APOLLO_SUPPORT
static void vAPOLLOIRQHandler(UINT16 u2Vector)
{
#if 0  //Enable when test Apollot Test Pattern: Risc <-> Apollo Interrupt
    
     UINT32 u4Apollo2RiscInt;
     UINT32 u4Msg;
 
     LOG(7, "Enter Apollo to Risc Interrupt  .......\n");
     
     u4Apollo2RiscInt = APOLLO_READ32(APOLLO_RREG_AP2RC_INTRVEC); 
    if ((u4Apollo2RiscInt & APOLLO2RC_INT_0) != 0)
    {      
        APOLLO_WRITE32(APOLLO_RREG_AP2RC_INTRVEC,APOLLO2RC_INT_0); 
        LOG(7, "Enter Apollo to Risc Interrupt 1 .......\n");
        
        //###### The following code just for test #####
        if (_u4Apollo2RiscRountCount==1)
        {
            vApollo_INT_RISC_Apollo_get_bit0Int_round1();
        }   
        else//now, _u4Apollo2RiscRountCount=2
        {
           vApollo_INT_RISC_Apollo_get_bit0Int_round2(); 
        }
        ///////////////////////////////////////////////
    }
    else if ((u4Apollo2RiscInt & APOLLO2RC_INT_1) != 0)
    {        
        APOLLO_WRITE32(APOLLO_RREG_AP2RC_INTRVEC,APOLLO2RC_INT_1);
        LOG(7, "Enter Apollo to Risc Interrupt 2 .......\n");

        //###### The following code just for test #####
        if (_u4Apollo2RiscRountCount==1)
        {
            vApollo_INT_RISC_Apollo_get_bit1Int_round1();
        }   
        else//now, _u4Apollo2RiscRountCount=2
        {
           vApollo_INT_RISC_Apollo_get_bit1Int_round2(); 
        }
        ///////////////////////////////////////////////
    }
    else if ((u4Apollo2RiscInt & APOLLO2RC_INT_2) != 0)
    {        
        APOLLO_WRITE32(APOLLO_RREG_AP2RC_INTRVEC,APOLLO2RC_INT_2);
        LOG(7, "Enter Apollo to Risc Interrupt 3 .......\n");

        //###### The following code just for test #####
        if (_u4Apollo2RiscRountCount==1)
        {
            vApollo_INT_RISC_Apollo_get_bit2Int_round1();
        }   
        else//now, _u4Apollo2RiscRountCount=2
        {
           vApollo_INT_RISC_Apollo_get_bit2Int_round2(); 
        }
        ///////////////////////////////////////////////
    }
    else if ((u4Apollo2RiscInt & APOLLO2RC_INT_3) != 0)
    {        
        APOLLO_WRITE32(APOLLO_RREG_AP2RC_INTRVEC,APOLLO2RC_INT_3);
        LOG(7, "Enter Apollo to Risc Interrupt 4 .......\n");

        //###### The following code just for test #####
        if (_u4Apollo2RiscRountCount==1)
        {
            vApollo_INT_RISC_Apollo_get_bit3Int_round1();
            _u4Apollo2RiscRountCount++; //let _u4Apollo2RiscRountCount = 2
        }   
        else//now, _u4Apollo2RiscRountCount=2
        {
           vApollo_INT_RISC_Apollo_get_bit3Int_round2(); 
           _u4Apollo2RiscRountCount=1; //let _u4Apollo2RiscRountCount = 1
           
        }
        ///////////////////////////////////////////////
    }

    
     
     UNUSED(u4Msg);
#else
    UINT32 u4Apollo2RiscInt;

    u4Apollo2RiscInt = APOLLO_READ32(APOLLO_RREG_AP2RC_INTRVEC); 
    if ((u4Apollo2RiscInt & APOLLO2RC_INT_0) != 0)
    {      
        vApolloIRQHandler0_ISR(); 
        APOLLO_WRITE32(APOLLO_RREG_AP2RC_INTRVEC,APOLLO2RC_INT_0); //clear INT bit0 
    }
    if ((u4Apollo2RiscInt & APOLLO2RC_INT_1) != 0)
    {        
        vApolloIRQHandler1_ISR(); 
        APOLLO_WRITE32(APOLLO_RREG_AP2RC_INTRVEC,APOLLO2RC_INT_1); //clear INT bit1         
    }
    if ((u4Apollo2RiscInt & APOLLO2RC_INT_2) != 0)
    {        
        vApolloIRQHandler2_ISR(); 
        APOLLO_WRITE32(APOLLO_RREG_AP2RC_INTRVEC,APOLLO2RC_INT_2); //clear INT bit2         
    }
    if ((u4Apollo2RiscInt & APOLLO2RC_INT_3) != 0)
    {                
        vApolloIRQHandler3_ISR(); 
        APOLLO_WRITE32(APOLLO_RREG_AP2RC_INTRVEC,APOLLO2RC_INT_3); //clear INT bit3     
    }
     
#endif
}

void vAPOLLOIRQEnable(BOOL fgEnable)
{
    BOOL u1Ret;
    // Enable Apollo interrupt
    if(fgEnable)
    {
        u1Ret = BIM_EnableIrq(VECTOR_APOLLO);
        //u1Ret = BIM_EnableIrq(0x4c);
    }
    else
    {
        u1Ret = BIM_DisableIrq(VECTOR_APOLLO);
        //u1Ret = BIM_DisableIrq(0x4c);
    }
    LOG(7, "Enable Apollo to Risc Interrupt\n");
    UNUSED(u1Ret);
}
#endif
   
void vADSPTaskMain_Init(void)
{
    vADCInit();
#if defined(CC_MT5880)
    AUD_DspClkSel(4); // viper adsp runs at 450MHz
#else
    AUD_DspClkSel(1); //1: sawlesspll_d1_ck (432MHz) or 4: enetpll_d1p5_ck (450MHz)
#endif

    vIO32WriteFldAlign(CKGEN_REG_AIN_CKCFG, 0, FLD_AIN_PD);
    vIO32WriteFldAlign(CKGEN_REG_AIN_CKCFG, 0, FLD_AIN_TST);
    vIO32WriteFldAlign(CKGEN_REG_AIN_CKCFG, 1, FLD_AIN_SEL); // 1:syspll_d4_ck (432/4 = 108), 2: syspll_d6_ck (648/6=108 for 5368/96)
    
    vDspMemInit((UINT8 *)u4GetDspWorkBuf());
    vDspShareInfoInit(); // initialization for shared information
    //initial AFIFO hardware register
    vSetSPDIFInFIFO(AUD_DEC_MAIN, u4GetAFIFOStart(AUD_DEC_MAIN), u4GetAFIFOEnd(AUD_DEC_MAIN));
    #ifdef DSP_TEST_LINE2
    vSetSPDIFInFIFO(AUD_DEC_AUX, u4GetAFIFOStart(AUD_DEC_AUX), u4GetAFIFOEnd(AUD_DEC_AUX));
    #endif
    #ifdef DEC3_USE_LINEIN2
    vSetSPDIFInFIFO(AUD_DEC_THIRD, u4GetAFIFOStart(AUD_DEC_THIRD), u4GetAFIFOEnd(AUD_DEC_THIRD));
    #endif
    //initial Paser register
    VERIFY(PSR_SoftSetAudioFIFO(AUD_DEC_MAIN, u4GetAFIFOStart(AUD_DEC_MAIN), u4GetAFIFOEnd(AUD_DEC_MAIN)));
    vOplFuncInit();

    vWriteDspWORD (ADDR_RC2D_DDCO_FLAG, 0x0);
#if (!defined(CC_AUD_ARM_SUPPORT) && !defined(CC_AUD_APOLLO_POST_SUPPORT))
    vWriteDspWORD (ADDR_RC2D_APROC_CONTROL, 0x8000); // bypass post   
#endif    

#if   defined(DSP_NEW_SCRAMBLE)
    if(fgCheckSumAll != TRUE)
    {
        CheckSumAll();
        fgCheckSumAll = TRUE;
    }
    
#endif
    /* initialization for state machine*/
    vDspStateInit(); // initialization for ADSP state machine
    _uDspState = (UINT8)DSP_IDLE; // initially, we enter suspend mode

}

/******************************************************************************
* Function      : vADSPTaskInit
* Description   : initialization routine for ADSP Task
* Parameter     : None
* Return        : None
******************************************************************************/
void vADSPTaskInit(void) // first entry point of ADSP control routine
{
#ifdef AUD_STATISTICS
    {
        CRIT_STATE_T rState;
        DRV_DBASE_T* prDbase = DBS_Lock(&rState);

        ASSERT(prDbase != NULL);
        //_prAud = &(prDbase->rAud);
        VERIFY(DBS_Unlock(prDbase, rState));
    }
#endif  // PSR_STATISTICS

    vADSPTaskMain_Init();

    if (!_fgTaskInit)
    {
        x_os_isr_fct pfnOldIsr;
    #ifdef CC_AUD_APOLLO_SUPPORT
        x_os_isr_fct pfnOldIsr_apollo; 
    #endif

        // Create message queue
        VERIFY(x_msg_q_create(&hDspCmdQueue, ADSPTASK_CMD_Q_NAME, sizeof(UINT32),128) == OSR_OK);
        VERIFY(x_msg_q_create(&hDspCmdDataQueue, ADSPTASK_CMD_DATA_Q_NAME, sizeof(UINT32),1024) == OSR_OK); //ADSP Data Queue.
        // Register ISR
        VERIFY(x_reg_isr(ADSP_INT_VECTOR, vADSPIRQHandler, &pfnOldIsr) == OSR_OK);
        //vADSPIRQEnable(TRUE);


    #ifdef CC_AUD_APOLLO_SUPPORT
        // Register ISR
        VERIFY(x_reg_isr(APOLLO_INT_VECTOR, vAPOLLOIRQHandler, &pfnOldIsr_apollo) == OSR_OK);
        //vAPOLLOIRQEnable(TRUE); 
    #endif  

        // Create ADSP task
        VERIFY(x_thread_create(&_hAdspThread, ADSPTASK_NAME, ADSPTASK_STACK_SIZE, ADSPTASK_THREAD_PRIORITY,
            vADSPTaskMain, 0, NULL) == OSR_OK);

        VERIFY(x_sema_create(&_hRdDspSramSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);

        VERIFY(x_sema_create(&_hSendDspIntSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);
        _fgTaskInit = TRUE;
    }
    /*else
    {
        vADSPIRQEnable(TRUE); // remove because irq has been enabled, do not need enable twice
    }*/
}

/******************************************************************************
* Function      : fgDecoderStopped,fgDecoderRealPlay,vSendADSPCmd,vADSPPowerON,
*                 vADSPPowerOFF
* Description   : APIs for external use
* Parameter     : None
* Return        : None
******************************************************************************/
BOOL fgDecoderStopped(UINT8 u1DecId)
{
    if (u1DecId < AUD_DEC_NUM)
    {
        return (_rDspStatus[u1DecId].fgDspStop);
    }
    return (FALSE); /* default value*/
}

BOOL fgDecoderRealPlay(UINT8 u1DecId)
{
    if (u1DecId < AUD_DEC_NUM)
    {
      return(_rDspStatus[u1DecId].fgDspRealPlay);
    }
    return(FALSE); /* default value*/
}

#if 0   // Unused
BOOL fgDecoderPlaySent(UINT8 u1DecId)
{
    if (u1DecId < AUD_DEC_NUM)
    {
      return (_rDspStatus[u1DecId].fgDspPlay);
    }
    return(FALSE); /* default value*/
}
#endif
BOOL fgADSPIsIdle(void)
{
    return ( (!_rDspStatus[AUD_DEC_MAIN].fgDspWakeUp) || (!_rDspStatus[AUD_DEC_MAIN].fgDspWakeUpOk) );
}

void vADSPPowerON(void)
{
    vSendADSPTaskMessage(ADSPTASK_MSG_POWER_ON);
}
#if 0   // Unused
void vADSPPowerOFF(void)
{
    vSendADSPTaskMessage(ADSPTASK_MSG_POWER_OFF);
}
#endif
/******************************************************************************
* Function      : u4ReadShmUINT32
* Description   : read content of shared memory in UINT32
* Parameter     : u2Addr: address of shared memory
* Return        : None
******************************************************************************/
UINT32 u4ReadShmUINT32(UINT16 u2Addr)
{
    return(u4ReadDspShmDWRD(u2Addr));
}
/******************************************************************************
* Function      : u4ReadShmUINT16
* Description   : read content of shared memory in UINT16
* Parameter     : u2Addr: address of shared memory
* Return        : None
******************************************************************************/
UINT16 u2ReadShmUINT16(UINT16 u2Addr)
{
    return(u2ReadDspShmWORD(u2Addr));
}
/******************************************************************************
* Function      : u4ReadShmUINT32
* Description   : read content of shared memory in UINT8
* Parameter     : u2Addr: address of shared memory
* Return        : None
******************************************************************************/
UINT8 uReadShmUINT8(UINT16 u2Addr)
{
    return(uReadDspShmBYTE(u2Addr));
}
/******************************************************************************
* Function      : vWriteShmUINT32
* Description   : write content of shared memory in UINT32
* Parameter     : u2Addr: address of shared memory, u4Value: value in UINT32
* Return        : None
******************************************************************************/
void vWriteShmUINT32(UINT16 u2Addr,UINT32 u4Value)
{
    vWriteDspShmDWRD(u2Addr,u4Value);
}
/******************************************************************************
* Function      : vWriteShmUINT16
* Description   : write content of shared memory in UINT16
* Parameter     : u2Addr: address of shared memory, u2Value: value in UINT16
* Return        : None
******************************************************************************/
void vWriteShmUINT16(UINT16 u2Addr,UINT16 u2Value)
{
    vWriteDspShmWORD(u2Addr,u2Value);
}

/******************************************************************************
* Function      : vWriteShmUINT8
* Description   : write content of shared memory in UINT8
* Parameter     : u2Addr: address of shared memory, uValue: value in UINT8
* Return        : None
******************************************************************************/
void vWriteShmUINT8(UINT16 u2Addr,UINT32 uValue)
{
    vWriteDspShmBYTE(u2Addr,uValue);
}

#ifdef CC_AUD_DSP_SUPPORT_MIXSOUND_MEMPLAY
UINT32 _AUD_DspGetMixSndReadPtr(UINT8 u1StreamID)
{
    UINT32 u4ReadPtr;
    DSP_FlushInvalidateDCacheSmall2(ADDR_RC2D_DRAM_MIXSOUND_CLIP1_RP + 16*u1StreamID);
    u4ReadPtr = (((UINT32)_i1DspMemBlock)<<24) + dReadDspCommDram (ADDR_RC2D_DRAM_MIXSOUND_CLIP1_RP + 16*u1StreamID); //TEST
    return u4ReadPtr;
}

void _AUD_DspSetMixSndWritePtr(UINT8 u1StreamID, UINT32 u4WritePtr)
{
    u4WritePtr &= 0xffffff;
    WriteDspCommDram2 (ADDR_RC2D_DRAM_MIXSOUND_CLIP1_WP + 16*u1StreamID, u4WritePtr); //TEST
}

void _AUD_DspShowMixSndStatus(UINT8 u1StreamID)
{
    Printf("ADDR_RC2D_DRAM_MIXSOUND_BASE: 0x%04x\n", (0x8000+ADDR_RC2D_DRAM_MIXSOUND_BASE));
    Printf("ADDR_RC2D_DRAM_MIXSOUND_STATUS(%x): 0x%08x\n", ADDR_RC2D_DRAM_MIXSOUND_STATUS, dReadDspCommDram(ADDR_RC2D_DRAM_MIXSOUND_STATUS));
    Printf("  1: working, 0: idle\n");
    Printf("ADDR_RC2D_DRAM_MIXSOUND_CONFIG(%x): 0x%08x\n", ADDR_RC2D_DRAM_MIXSOUND_CONFIG,  dReadDspCommDram(ADDR_RC2D_DRAM_MIXSOUND_CONFIG));
    Printf("  bit  0: 0-> no need upsampling, 1-> 2x upsampling\n");
    Printf("  bit  1: 2-> 4x upsampling, 3-> 8x upsampling\n");
    Printf("  bit  4: source mode : 0->mono, 1->stereo\n");
    Printf("  bit  8: mixing data to left & right output\n");
    Printf("  bit  9: mixing data to surround output\n");
    Printf("  bit 10: mixing data to center output\n");
    Printf("  bit 11: mixing data to channel 7 & 8\n");
    Printf("  bit 12: mixing data to channel 9 & 10\n");
    Printf("ADDR_RC2D_DRAM_MIXSOUND_PCM_ADDR(%x): 0x%08x\n", ADDR_RC2D_DRAM_MIXSOUND_PCM_ADDR, dReadDspCommDram(ADDR_RC2D_DRAM_MIXSOUND_PCM_ADDR));
    Printf("ADDR_RC2D_DRAM_MIXSOUND_PCM_LEN(%x): 0x%08x\n", ADDR_RC2D_DRAM_MIXSOUND_PCM_LEN, dReadDspCommDram(ADDR_RC2D_DRAM_MIXSOUND_PCM_LEN));
    Printf("ADDR_RC2D_DRAM_MIXSOUND_GAIN(%x): 0x%08x\n", ADDR_RC2D_DRAM_MIXSOUND_GAIN, dReadDspCommDram(ADDR_RC2D_DRAM_MIXSOUND_GAIN));

    if (u1StreamID < MAX_AUD_MIXSND_STREAM_NUM)
    {
        Printf("ADDR_RC2D_DRAM_MIXSOUND_CLIP%d_RP(%x): 0x%08x\n", u1StreamID+1, ADDR_RC2D_DRAM_MIXSOUND_CLIP1_RP+(UINT32)u1StreamID*16, dReadDspCommDram(ADDR_RC2D_DRAM_MIXSOUND_CLIP1_RP+(UINT32)u1StreamID*16)); //TEST
        Printf("ADDR_RC2D_DRAM_MIXSOUND_CLIP%d_WP(%x): 0x%08x\n", u1StreamID+1, ADDR_RC2D_DRAM_MIXSOUND_CLIP1_WP+(UINT32)u1StreamID*16, dReadDspCommDram(ADDR_RC2D_DRAM_MIXSOUND_CLIP1_WP+(UINT32)u1StreamID*16)); //TEST
        Printf("ADDR_RC2D_DRAM_MIXSOUND_CLIP%d_SA(%x): 0x%08x\n", u1StreamID+1, ADDR_RC2D_DRAM_MIXSOUND_CLIP1_SA+(UINT32)u1StreamID*16, dReadDspCommDram(ADDR_RC2D_DRAM_MIXSOUND_CLIP1_SA+(UINT32)u1StreamID*16)); //TEST
        Printf("ADDR_RC2D_DRAM_MIXSOUND_CLIP%d_EA(%x): 0x%08x\n", u1StreamID+1, ADDR_RC2D_DRAM_MIXSOUND_CLIP1_EA+(UINT32)u1StreamID*16, dReadDspCommDram(ADDR_RC2D_DRAM_MIXSOUND_CLIP1_EA+(UINT32)u1StreamID*16)); //TESt
        Printf("ADDR_RC2D_DRAM_MIXSOUND_CLIP%d_GAIN(%x): 0x%08x\n", u1StreamID+1, ADDR_RC2D_DRAM_MIXSOUND_CLIP1_GAIN+(UINT32)u1StreamID*16, dReadDspCommDram(ADDR_RC2D_DRAM_MIXSOUND_CLIP1_GAIN+(UINT32)u1StreamID*16)); //TEST
    }
    else
    {
        Printf("u1StreamID must be less than %d\n", MAX_AUD_MIXSND_STREAM_NUM);
    }
}

UINT32 u4GetMixSndGain(UINT8 u1StreamId)
{
    return dReadDspCommDram (ADDR_RC2D_DRAM_MIXSOUND_CLIP1_GAIN + 16*u1StreamId);
}

void vSetMixSndGain(UINT8 u1StreamId,UINT32 u4Gain)
{
    WriteDspCommDram2 (ADDR_RC2D_DRAM_MIXSOUND_CLIP1_GAIN+u1StreamId*16, u4Gain);
}

#ifdef CC_AUD_MIXSND_FOR_ALSA
#ifdef ALSA_MIXSND_PATH
extern UINT32 u4GetMixSndRingFifoRP(UINT8 u1StreamId);
#endif
UINT32 u4GetMixSndReadPtr(UINT8 u1StreamId)
{
#ifdef ALSA_MIXSND_PATH
    return u4GetMixSndRingFifoRP(u1StreamId);
#else
    return _AUD_DspGetMixSndReadPtr(u1StreamId+ALSA_MIXSND_STREAM_ID);
#endif
}

#ifdef ALSA_MIXSND_PATH
extern void vSetMixSndRingFifoWP(UINT8 u1StreamId, UINT32 u4WritePtr);
#endif
void vSetMixSndWritePtr(UINT8 u1StreamId, UINT32 u4WritePtr)
{
#ifdef ALSA_MIXSND_PATH
    vSetMixSndRingFifoWP(u1StreamId, u4WritePtr);
#else
    _AUD_DspSetMixSndWritePtr(u1StreamId+ALSA_MIXSND_STREAM_ID, u4WritePtr);
#endif
}
#endif
#if defined(CC_AUD_MIXSND_FOR_ALSA) || defined(CC_ENABLE_AOMX)
UINT32 u4GetMixSndFIFOStart(UINT8 u1StreamId)
{
#ifdef ALSA_MIXSND_PATH
    return u4GetMixSoundStartByteAddr3()+u1StreamId*u4GetMixSoundBufSize3()/MAX_AUD_MIXSND_STREAM_NUM_FOR_ALSA;
#else
    UINT32 u4SA;
    u4SA = (((UINT32)_i1DspMemBlock)<<24) + dReadDspCommDram (ADDR_RC2D_DRAM_MIXSOUND_CLIP1_SA + (UINT32)16*(u1StreamId+ALSA_MIXSND_STREAM_ID));
    return u4SA;
#endif
}

UINT32 u4GetMixSndFIFOEnd(UINT8 u1StreamId)
{
#ifdef ALSA_MIXSND_PATH
    return u4GetMixSoundStartByteAddr3()+(u1StreamId+1)*u4GetMixSoundBufSize3()/MAX_AUD_MIXSND_STREAM_NUM_FOR_ALSA;
#else
    UINT32 u4EA;
    u4EA = (((UINT32)_i1DspMemBlock)<<24) + dReadDspCommDram (ADDR_RC2D_DRAM_MIXSOUND_CLIP1_EA + (UINT32)16*(u1StreamId+ALSA_MIXSND_STREAM_ID));
    return u4EA;
#endif
}
#endif
#endif

#include "dsp_data.h"
#if defined(CC_MT5881) || defined(CC_MT5399)
#include "../../fbm8199/fbm_pool_config.h"
#else
#include "../../fbm8098/fbm_pool_config.h"
#endif
extern UINT32 u4ReadDspSram(UINT32 u4Addr);
extern void AudShowAsrcBufferStatus(UINT8 u1ChMsk,BOOL fgUpdateRWP);

extern UINT32 u4GetMixSoundStartByteAddr(void);
extern UINT32 u4GetMixSoundBufSize(void);
extern UINT32 u4GetMixSoundStartByteAddr2(void);
extern UINT32 u4GetMixSoundBufSize2(void);
#ifdef ALSA_MIXSND_PATH
extern UINT32 u4GetMixSoundStartByteAddr3(void);
extern UINT32 u4GetMixSoundBufSize3(void);
#endif

void AUD_QueryDspMemory(UINT8 u1MemSpace)
{
    if (u1MemSpace == 0)
    {
        Printf("  DSP_SHARED_INFO: %x\n", DSP_SHARED_INFO_SZ);
        Printf("  DSP_CODE_SZ: %x\n", DSP_CODE_SZ);
        Printf("  DSP_DATA_SZ: %x\n", DSP_DATA_SZ);
        Printf("  DSP_AFIFO_SZ: %x\n", DSP_AFIFO_SZ);
        Printf("  DSP_AFIFO_SZ_DEC2: %x\n", DSP_AFIFO_SZ_DEC2);
        Printf("  DSP_AFIFO_SZ_DEC3: %x\n", DSP_AFIFO_SZ_DEC3);
        Printf("  DSP_AFIFO_SZ_DEC4: %x\n", DSP_AFIFO_SZ_DEC4);
        Printf("  DSP_AFIFO_HDMI_PARSER_SZ: %x\n", DSP_AFIFO_HDMI_PARSER_SZ);
        Printf("+ DSP_MIXSNDBUF_ALL_SZ: %x\n", DSP_MIXSNDBUF_ALL_SZ);
        Printf("================================================\n");
        Printf("  %x\n", FBM_DSP_SIZE);
    }

    if (u1MemSpace == 1)
    {
#ifndef DSP_SUPPORT_DUAL_DECODE
        Printf("[DSP Code Space]\n");
        Printf("  ROM : Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[0]<<8), VIRTUAL(_u4DspICacheAddr[0]<<8), DSP_ROM_SZ);
        Printf("  POS : Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[1]<<8), VIRTUAL(_u4DspICacheAddr[1]<<8), DSP_POS_RAM_SZ);
        Printf("  CRI : Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[2]<<8), VIRTUAL(_u4DspICacheAddr[2]<<8), DSP_CTR_RAM_SZ);
        Printf("  COM : Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[3]<<8), VIRTUAL(_u4DspICacheAddr[3]<<8), DSP_CMM_RAM_SZ);
        Printf("  DEC4: Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[4]<<8), VIRTUAL(_u4DspICacheAddr[4]<<8), DSP_DEC4_RAM_SZ);
        Printf("  DEC1: Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[5]<<8), VIRTUAL(_u4DspICacheAddr[5]<<8), DSP_DEC1_RAM_SZ);
        Printf("  DEC3: Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[6]<<8), VIRTUAL(_u4DspICacheAddr[6]<<8), DSP_DEC3_RAM_SZ);
        Printf("  DEC2: Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[7]<<8), VIRTUAL(_u4DspICacheAddr[7]<<8), DSP_DEC2_RAM_SZ);
#else
        Printf("[DSP Code Space 1]\n");
        Printf("  ROM : Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[0]<<8), VIRTUAL(_u4DspICacheAddr[0]<<8), DSP_ROM_SZ);
        Printf("  CMMA : Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[1]<<8), VIRTUAL(_u4DspICacheAddr[1]<<8), DSP_CMMA_RAM_SZ);
        Printf("  CRI : Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[2]<<8), VIRTUAL(_u4DspICacheAddr[2]<<8), DSP_CTR_RAM_SZ);
        Printf("  COMAX : Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[3]<<8), VIRTUAL(_u4DspICacheAddr[3]<<8), DSP_CMMAEXT_RAM_SZ);
        Printf("  DEC3: Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[4]<<8), VIRTUAL(_u4DspICacheAddr[4]<<8), DSP_DEC3_RAM_SZ);
        Printf("  DEC1: Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[5]<<8), VIRTUAL(_u4DspICacheAddr[5]<<8), DSP_DEC1_RAM_SZ);
        Printf("  DEC2: Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[6]<<8), VIRTUAL(_u4DspICacheAddr[6]<<8), DSP_DEC2_RAM_SZ);
        Printf("[DSP Code Space 2]\n");
        Printf("  ROM : Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[7]<<8), VIRTUAL(_u4DspICacheAddr[7]<<8), DSP_ROM_SZ);
        Printf("  POS : Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[8]<<8), VIRTUAL(_u4DspICacheAddr[8]<<8), DSP_POS_RAM_SZ);
        Printf("  CMMB : Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[9]<<8), VIRTUAL(_u4DspICacheAddr[9]<<8), DSP_CMMB_RAM_SZ);
        Printf("  RESER : Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[10]<<8), VIRTUAL(_u4DspICacheAddr[10]<<8), DSP_RESER_RAM_SZ);
        Printf("  ENC1 : Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[11]<<8), VIRTUAL(_u4DspICacheAddr[11]<<8), DSP_DEC4_RAM_SZ);
        Printf("  ENC2 : Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspICacheAddr[12]<<8), VIRTUAL(_u4DspICacheAddr[12]<<8), DSP_ENC2_RAM_SZ);
#endif
    }

    if (u1MemSpace == 2)
    {
        Printf("[DSP Normal Space]\n");
        Printf("  DEC1: Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspDramBuf[0]), VIRTUAL(_u4DspDramBuf[0]), DSP_DAT_DEC1_SZ);
        Printf("  DEC2: Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspDramBuf[1]), VIRTUAL(_u4DspDramBuf[1]), DSP_DAT_DEC2_SZ);
        Printf("  DEC3: Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspDramBuf[2]), VIRTUAL(_u4DspDramBuf[2]), DSP_DAT_DEC3_SZ);
        Printf("  COMM: Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspDramBuf[3]), VIRTUAL(_u4DspDramBuf[3]), DSP_DAT_CMM_NORMAL_SZ);
        Printf("  DEC4: Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspDramBuf[4]), VIRTUAL(_u4DspDramBuf[4]), DSP_DAT_DEC4_SZ);
        Printf("  MIX : Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspDramBuf[5]), VIRTUAL(_u4DspDramBuf[5]), DSP_MIXSNDBUF_ALL_SZ);
    }

    if (u1MemSpace == 3)
    {
        Printf("[DSP Compact Space]\n");
        Printf("  DEC1: Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspCmptBuf[0]), VIRTUAL(_u4DspCmptBuf[0]), DSP_DAT_DEC1_SZ);
        Printf("  DEC2: Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspCmptBuf[1]), VIRTUAL(_u4DspCmptBuf[1]), DSP_DAT_DEC2_SZ);
        Printf("  DEC3: Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspCmptBuf[2]), VIRTUAL(_u4DspCmptBuf[2]), DSP_DAT_DEC3_SZ);
        Printf("  COMM: Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspCmptBuf[3]), VIRTUAL(_u4DspCmptBuf[3]), DSP_DAT_CMM_SZ);
        Printf("  DEC4: Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspCmptBuf[4]), VIRTUAL(_u4DspCmptBuf[4]), DSP_DAT_DEC4_SZ);
        Printf("  MIX : Addr: (P)0x%08x/(V)0x%08x Size: 0x%04x\n", (_u4DspCmptBuf[5]), VIRTUAL(_u4DspCmptBuf[5]), DSP_MIXSNDBUF_ALL_SZ);
    }

    if (u1MemSpace == 4)
    {
        Printf("[AFIFO Space]\n");
        Printf("  AFIFO1: Start: (P)0x%08x/(V)0x%08x End: (P)0x%08x/(V)0x%08x\n", _u4AFIFO[0][0], VIRTUAL(_u4AFIFO[0][0]), _u4AFIFO[0][1], VIRTUAL(_u4AFIFO[0][1]));
        Printf("  AFIFO2: Start: (P)0x%08x/(V)0x%08x End: (P)0x%08x/(V)0x%08x\n", _u4AFIFO[1][0], VIRTUAL(_u4AFIFO[1][0]), _u4AFIFO[1][1], VIRTUAL(_u4AFIFO[1][1]));
        Printf("  AFIFO3: Start: (P)0x%08x/(V)0x%08x End: (P)0x%08x/(V)0x%08x\n", _u4AFIFO[2][0], VIRTUAL(_u4AFIFO[2][0]), _u4AFIFO[2][1], VIRTUAL(_u4AFIFO[2][1]));
        Printf("  AFIFO4: Start: (P)0x%08x/(V)0x%08x End: (P)0x%08x/(V)0x%08x\n", _u4AFIFO[3][0], VIRTUAL(_u4AFIFO[3][0]), _u4AFIFO[3][1], VIRTUAL(_u4AFIFO[3][1]));
    }

    if (u1MemSpace == 5)
    {
        Printf("[Internal Buffer Space]\n");
        Printf("  IntBuf1: Start (P)0x%08x/(V)0x%08x Size: 0x%08x\n", (_u4DspCmptBuf[3] + DRAM_INTERNAL_BUF*4), VIRTUAL(_u4DspCmptBuf[3] + DRAM_INTERNAL_BUF*4), DRAM_INTERNAL_BUF_SIZE*4);
        Printf("  IntBuf2: Start (P)0x%08x/(V)0x%08x Size: 0x%08x\n", (_u4DspCmptBuf[3] + DRAM_INTERNAL_BUF2*4), VIRTUAL(_u4DspCmptBuf[3] + DRAM_INTERNAL_BUF2*4), DRAM_INTERNAL_BUF2_SIZE*4);
        Printf("  IntBuf3: Start (P)0x%08x/(V)0x%08x Size: 0x%08x\n", (_u4DspCmptBuf[3] + DRAM_INTERNAL_BUF3*4), VIRTUAL(_u4DspCmptBuf[3] + DRAM_INTERNAL_BUF3*4), DRAM_INTERNAL_BUF3_SIZE*4);
        Printf("  IntBuf4: Start (P)0x%08x/(V)0x%08x Size: 0x%08x\n", (_u4DspCmptBuf[3] + DRAM_INTERNAL_BUF4*4), VIRTUAL(_u4DspCmptBuf[3] + DRAM_INTERNAL_BUF4*4), DRAM_INTERNAL_BUF4_SIZE*4);
        Printf("  IntBuf5: Start (P)0x%08x/(V)0x%08x Size: 0x%08x\n", (_u4DspCmptBuf[3] + DRAM_INTERNAL_BUF5*4), VIRTUAL(_u4DspCmptBuf[3] + DRAM_INTERNAL_BUF5*4), DRAM_INTERNAL_BUF5_SIZE*4);
        Printf("  IntBuf6: Start (P)0x%08x/(V)0x%08x Size: 0x%08x\n", (_u4DspCmptBuf[3] + DRAM_INTERNAL_BUF6*4), VIRTUAL(_u4DspCmptBuf[3] + DRAM_INTERNAL_BUF6*4), DRAM_INTERNAL_BUF6_SIZE*4);
    }

    if (u1MemSpace == 6)
    {
        Printf("[ASRC Space]\n");
        AudShowAsrcBufferStatus(0x1f,FALSE);
    }

    if (u1MemSpace == 7)
    {
        int i;
        UINT32 u4Addr;
        UINT32 u4AoutCmptPage;

        Printf("[AOUT Space]\n");

        u4AoutCmptPage = ((u4ReadDspSram(REG_DSP_AOUT_CTRL)>>12)&0x7);
        Printf("  AOUT1 Cmpt Page: %d Ch Num: %d\n", u4AoutCmptPage, (u4ReadDspSram(REG_DSP_CH_NUM)>>8));
        Printf("        CH CFG: L: %x R: %x SL: %x SR: %x C: %x LFE: %x CH7: %x CH8: %x CH9: %x CH10: %x\n",
                  ((u4ReadDspSram(REG_DSP_CH_CFG_0)>>8)&0xf),
                  ((u4ReadDspSram(REG_DSP_CH_CFG_0)>>12)&0xf),
                  ((u4ReadDspSram(REG_DSP_CH_CFG_1)>>8)&0xf),
                  ((u4ReadDspSram(REG_DSP_CH_CFG_1)>>12)&0xf),
                  ((u4ReadDspSram(REG_DSP_CH_CFG_0)>>16)&0xf),
                  ((u4ReadDspSram(REG_DSP_CH_CFG_1)>>16)&0xf),
                  ((u4ReadDspSram(REG_DSP_CH_CFG_0)>>20)&0xf),
                  ((u4ReadDspSram(REG_DSP_CH_CFG_1)>>20)&0xf),
                  ((u4ReadDspSram(REG_DSP_CH_CFG_2)>>8)&0xf),
                  ((u4ReadDspSram(REG_DSP_CH_CFG_2)>>12)&0xf));
        for (i=0;i<11;i++)
        {
            u4Addr = _u4DspCmptBuf[u4AoutCmptPage] + (u4ReadDspSram(REG_DSP_CHL_1_ADDR+i)<<2);
            Printf("        CH%d addr: (P)0x%08x/(V)0x%08x\n", i+1, u4Addr, VIRTUAL(u4Addr));
        }

        u4AoutCmptPage = ((u4ReadDspSram(REG_DSP_AOUT_CTRL_2)>>12)&0x7);
        Printf("  AOUT2 Cmpt Page: %d Ch Num: %d\n", u4AoutCmptPage, (u4ReadDspSram(REG_DSP_CH_NUM_2)>>8));
        for (i=0;i<3;i++)
        {
            u4Addr = _u4DspCmptBuf[u4AoutCmptPage] + (u4ReadDspSram(REG_DSP_CHL_1_ADDR_2+i)<<2);
            Printf("        CH%d addr: (P)0x%08x/(V)0x%08x\n", i+1, u4Addr, VIRTUAL(u4Addr));
        }
    }

    if (u1MemSpace == 8)
    {
        Printf("[Upload Buffer]\n");
        Printf("   SA: (P)0x%08x/(V)0x%08x Size: %x\n",
                u4GetUploadFIFOStart(),
                VIRTUAL(u4GetUploadFIFOStart()),
                u4GetUploadFIFOEnd() - u4GetUploadFIFOStart());
    }

    if (u1MemSpace == 9)
    {
        Printf("[MixSnd Buffer]\n");
        Printf("  MIXSND1 SA: (P)0x%08x/(V)0x%08x Size: %x\n",
                u4GetMixSoundStartByteAddr(),
                VIRTUAL(u4GetMixSoundStartByteAddr()),
                u4GetMixSoundBufSize());
        Printf("  MIXSND2 SA: (P)0x%08x/(V)0x%08x Size: %x\n",
                u4GetMixSoundStartByteAddr2(),
                VIRTUAL(u4GetMixSoundStartByteAddr2()),
                u4GetMixSoundBufSize2());
        Printf("  MIXSND3 SA: (P)0x%08x/(V)0x%08x Size: %x\n",
                u4GetMixSoundStartByteAddr3(),
                VIRTUAL(u4GetMixSoundStartByteAddr3()),
                u4GetMixSoundBufSize3());
    }
#ifdef CC_AUD_APOLLO_SUPPORT
    if (u1MemSpace == 10)
    {
        Printf("[Apollo Memory]\n");
        Printf("  Program SA: (P)0x%08x/(V)0x%08x Size: %x\n",
                u4GetApolloWorkBufNonInit(0),
                VIRTUAL(u4GetApolloWorkBufNonInit(0)),
                DSP_APROC_CODE_SZ);
        Printf("  Data SA: (P)0x%08x/(V)0x%08x Size: %x\n",
                u4GetApolloWorkBufNonInit(1),
                VIRTUAL(u4GetApolloWorkBufNonInit(1)),
                DSP_APROC_DATA_SZ);
    }
#endif    
}

void AUD_DumpDspMemory(UINT8 u1Option, UINT32 u4Size)
{
    UINT32 i;
    UINT32 u4SA;

    switch (u1Option)
    {
        case 0:
            Printf("[Dump AFIFO1]\n");
            u4SA = VIRTUAL(_u4AFIFO[0][0]);
            break;
        case 1:
            Printf("[Dump AFIFO2]\n");
            u4SA = VIRTUAL(_u4AFIFO[1][0]);
            break;
        case 2:
            Printf("[Dump AFIFO3]\n");
            u4SA = VIRTUAL(_u4AFIFO[2][0]);
            break;
        case 3:
            Printf("[Dump AFIFO4]\n");
            u4SA = VIRTUAL(_u4AFIFO[3][0]);
            break;
        case 4:
            Printf("[Dump UPLOAd]\n");
            u4SA = VIRTUAL(u4GetUploadFIFOStart());
            break;
        case 5:
            Printf("[Dump ALSA]\n");
            u4SA = VIRTUAL(u4GetMixSoundStartByteAddr3());
            break;
        case 6:
            Printf("[Dump MIXCLIP]\n");
            u4SA = VIRTUAL(u4GetMixSoundStartByteAddr3()-0x4000);
            break;

        default:
            return;
}

    for (i=0;i<u4Size;i++)
    {
        if ((i&0xf)==0)
        {
            Printf("\n%08xh: ", (u4SA+i));
        }
        Printf(" %02x", *((UINT8*)(u4SA+i)));
    }
}

void vDSPLockReadSramSema(void)
{
    VERIFY(x_sema_lock(_hRdDspSramSema, X_SEMA_OPTION_WAIT)== OSR_OK);
}

void vDSPUnlockReadSramSema(void)
{
    VERIFY(x_sema_unlock(_hRdDspSramSema)== OSR_OK);
}

void ADAC_SIFSrcSel(UINT32 isUseSIF)
{
    UINT32 u4Data;

    // Set AUDIN_CFG
    u4Data = (AUD_READ32(REG_AUDIN_CFG) & (~REG_AUDIN_CFG_MASK));
    u4Data = u4Data | APGA_GAIN_0DB | APGA_OFFSET_0V | APGA_GAIN_BY_DSP | ADCK_INV_RISING;
    AUD_WRITE32(REG_AUDIN_CFG, u4Data);

    if(isUseSIF)
    {
        ANA_WRITE32(REG_CVBS_CFG4, (AUD_READ32(REG_CVBS_CFG4)&0x00ffff7f) | 0x40000080);
        ANA_SET_BIT(REG_CVBS_CFG6, 0x20000000);
        ANA_CLR_BIT(REG_CVBS_CFG7, 0x00a0b880);
        
        u4Data = (AUD_READ32(REG_AUDIN_CFG) & (~ADC_SEL_DEMOD));
        AUD_WRITE32(REG_AUDIN_CFG, u4Data);
    }
    else //use ATD
    {
        u4Data = (AUD_READ32(REG_AUDIN_CFG)) | (ADC_SEL_DEMOD);
        AUD_WRITE32(REG_AUDIN_CFG, u4Data);
    }

}

#ifdef CC_AUD_DDI
void vSoundBarUpdate(void)
{
    vWriteDspWORD (ADDR_RC2D_IEC_CHANNEL_CFG2, ((UINT16) uReadDspShmBYTE (B_IEC_SOUNDBAR)));
}
#endif

UINT32 vGetFBMCMMSize(void)
{
    return DSP_DAT_CMM_SZ;
}

#ifdef CC_AUD_ARM_SUPPORT
/* Semaphore for Audio processor resource accessing
*/
void vAprocLockAccessSema(void)
{
    VERIFY(x_sema_lock(_hAprocAccessSema, X_SEMA_OPTION_WAIT)== OSR_OK);
}

void vAprocUnlockAccessSema(void)
{
    VERIFY(x_sema_unlock(_hAprocAccessSema)== OSR_OK);
}

/* Semaphore for Audio processor
*/
void vAprocLockSema(void)
{
    VERIFY(x_sema_lock(_hAprocSema, X_SEMA_OPTION_WAIT)== OSR_OK);
}

void vAprocUnlockSema(void)
{
    VERIFY(x_sema_unlock(_hAprocSema)== OSR_OK);
}

/* audio processor command done
*/
void vAprocCmdDone (UINT32 u4Command)
{
    LOG (5, "vAprocCmdDone: 0x%x\n", u4Command);
    
    VERIFY(x_sema_unlock(_ahSemaAprocComDone) == OSR_OK);
} 

/* audio processor wait command done
*/
void vAprocWaitCmdDone (void)
{
    INT32 i4Ret;

    LOG(5, "vAprocWaitCmdDone ...\n");

#ifndef FIX_AUD_UOP_TIMEOUT
    // Try lock to make sure the semaphore was locked
    i4Ret = x_sema_lock(_ahSemaAprocComDone, X_SEMA_OPTION_NOWAIT);
    VERIFY((i4Ret == OSR_OK) || (i4Ret == OSR_WOULD_BLOCK));
#endif

    // Real lock ..
    i4Ret = x_sema_lock_timeout(_ahSemaAprocComDone, 5000);
    VERIFY((i4Ret == OSR_OK) || (i4Ret == OSR_TIMEOUT));

    LOG(5, "vAprocWaitCmdDone: wake-up!\n");
    
    if (i4Ret == OSR_TIMEOUT)
    {
        LOG(1, "vAprocWaitCmdDone TIMEOUT\n");
        // fix me!!!! ....
    }
}


/* Add [A] to Log
*/
static CHAR *_cAudAprocLogTransfer (CHAR *cIn )
{
    UINT32 i;

    // provide prefix
    _gcStringBuf[0] = '[';
    _gcStringBuf[1] = 'A';
    _gcStringBuf[2] = ']';

    // copy stream
    for (i = 0; i < AUD_APROC_CHAR_NUM; i ++)
    {
        _gcStringBuf[i+3] = cIn[i]; 
    }   

    // provide end
    _gcStringBuf[AUD_APROC_CHAR_LEN-2] = 0x00;
    _gcStringBuf[AUD_APROC_CHAR_LEN-1] = 0x0A;

    return (CHAR *) _gcStringBuf;
}

/* Audio processor, Audio processor to Main ARM log handler
*/

// ISR handler for Audio processor Logs
static void vAprocCommLog_handler (void)
{
    INT32 i4Val0, i4Val1, i4Val2, i4Val3;
    CHAR *cLog;

    i4Val0 = u4AprocBIM_read (APROC_A2M_LOG_DATA0);
    i4Val1 = u4AprocBIM_read (APROC_A2M_LOG_DATA1);
    i4Val2 = u4AprocBIM_read (APROC_A2M_LOG_DATA2);
    i4Val3 = u4AprocBIM_read (APROC_A2M_LOG_DATA3);

    cLog= _cAudAprocLogTransfer ((CHAR *) (u4AprocBIM_read (APROC_A2M_LOG_ADDR) + 
        u4GetAprocMemoryMap(APROC_MEMMAP_WORKBUF)));

    LOG (5, cLog, i4Val0, i4Val1, i4Val2, i4Val3);
}

// ISR handler for Audio processor ISR Logs
static void vAprocCommIsrLog_handler (void)
{
    INT32 i4Val0, i4Val1, i4Val2, i4Val3;
    CHAR *cLog;

    i4Val0 = u4AprocBIM_read (APROC_A2M_ISR_LOG_DATA0);
    i4Val1 = u4AprocBIM_read (APROC_A2M_ISR_LOG_DATA1);
    i4Val2 = u4AprocBIM_read (APROC_A2M_ISR_LOG_DATA2);
    i4Val3 = u4AprocBIM_read (APROC_A2M_ISR_LOG_DATA3);

    cLog = _cAudAprocLogTransfer ((CHAR *) (u4AprocBIM_read (APROC_A2M_ISR_LOG_ADDR) + 
        u4GetAprocMemoryMap(APROC_MEMMAP_WORKBUF)));

    LOG (7, cLog, i4Val0, i4Val1, i4Val2, i4Val3);
}

void vA2MCommNotifyKernelLog (void)
{
    CHAR *cLog;

    cLog = _cAudAprocLogTransfer ((CHAR *) (u4GetAprocMemoryMap(APROC_MEMMAP_WORKBUF) + 
        u4AprocBIM_read (APROC_A2M_DATA1_REG)));

    LOG (5, cLog);
}

void vA2MCommKernelOpenDone (void)
{
    LOG (5, "====> vA2MCommKernelOpenDone\n");
}

void vA2MCommPostProcOpenDone (void)
{
    LOG (5, "====> vA2MCommPostProcOpenDone\n");
    
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_POSTPROC, APROC_SIG_CMD_OPEN));
    _gu4AprocPostProcControl = APROC_POSTPROC_OPEN;
}

void vA2MCommPostProcCloseDone (void)
{
    LOG (5, "====> vA2MCommPostProcCloseDone\n");
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_POSTPROC, APROC_SIG_CMD_CLOSE));
    _gu4AprocPostProcControl = APROC_POSTPROC_CLOSE;
}

void vA2MCommIoInitDone (void)
{
    LOG (5, "====> vA2MCommIoInitDone\n");
}

/* Audio processpr ISR functions
*/ 
static void vAprocCommSignal_handler (void)
{
    UINT32 u4Cmd, u4Idx;
    _AUD_APROC_COMM_SIG_T *eInfo = (_AUD_APROC_COMM_SIG_T *) _gu4AudAprocA2MSigInfo;
    
    u4Cmd = u4AprocBIM_read (APROC_A2M_SIG_REG);
    u4Idx = u4AudAprocSig_GetIdx(u4Cmd);
    u4Cmd = u4AudAprocSig_GetCMD(u4Cmd);

    while (1)
    {
        if ((eInfo->u4Cmd == 0) && (eInfo->u4Idx == 0))
        {
            // exit case
            break;
        }

        if ((eInfo->u4Cmd == u4Cmd) && (eInfo->u4Idx == u4Idx))
        {
            //  command hit
            if (Aproc_Sig_A2M_Is_Hit (eInfo->u4Data0, u4AprocBIM_read (APROC_A2M_DATA0_REG)) &&
                Aproc_Sig_A2M_Is_Hit (eInfo->u4Data1, u4AprocBIM_read (APROC_A2M_DATA1_REG)) )
            {
                if (eInfo->u4Func)
                {
                    eInfo->u4Func ();
                }
                break;
            }
        }
        eInfo ++;
    }
}

/* Audio processpr ISR handlers
*/ 
static void vAprocIRQHandler0_ISR(UINT16 u2Vector)
{
    vAprocCommSignal_handler ();
    
    UNUSED(u2Vector);
}

static void vAprocIRQHandler1_ISR(UINT16 u2Vector)
{
    vAprocCommLog_handler ();
    
    UNUSED(u2Vector);
}

static void vAprocIRQHandler2_ISR(UINT16 u2Vector)
{
    vAprocCommIsrLog_handler ();
     
     UNUSED(u2Vector);
}

static void vAprocIRQHandler3_ISR(UINT16 u2Vector)
{
    LOG (0, "=====> vAprocIRQHandler3_ISR\n");
    
     //....
     
     UNUSED(u2Vector);
}

/* init audio processor
*/
static void vAproc_Init (void)
{
    UINT32 u4AprocWokBuf;

    // disable Audio ARM
    vAprocBIM_write  (APROC_RST, 0);

    // reset Audio ARM
    vIO32WriteFldAlign (CKGEN_REG_ACPU_CKCFG, 1, FLD_ACPU_PD);
    vIO32WriteFldAlign (CKGEN_REG_ACPU_CKCFG, 0, FLD_ACPU_PD);

    // set Audio ARM AHB speed
    vIO32WriteFldAlign (CKGEN_REG_ACPU_CKCFG, 1, FLD_ACPU_MISC); // /2

    // set Audio ARM speed
    vIO32WriteFldAlign (CKGEN_REG_ACPU_CKCFG, 4, FLD_ACPU_SEL); // 432 MHz

    // Initial Audio ARM memory    
    vAprocMemInit ((UINT8 *) u4GetDspWorkBufNonInit());

    // setup base address
    u4AprocWokBuf = u4GetAprocMemoryMap(APROC_MEMMAP_WORKBUF_PHY);
    vAprocBIM_write (APROC_BASE, u4AprocWokBuf);
    
    // load code    
    u4AudAprocLoadCode (u4AprocWokBuf);        
}

/* start audio processor
*/
static void vAproc_start (void)
{
    vAprocBIM_write  (APROC_RST, 1);   
}

/* start audio processor
*/
static void vAproc_stop (void)
{
    vAprocBIM_write  (APROC_RST, 0);    
}


 /******************************************************************************
 * Function      : vAprocMain_Init
 * Description   : initialization Main routine for Audio processor
 * Parameter     : None
 * Return        : None
 ******************************************************************************/
// very important flag!!!
// It controls audio processor driver part enable.
// If audio processor is disable, all functions in driver related to audio proecssor must be "mute".
UINT32 _gu4AprocEnable = 0; // fix me!!!!

void vAproc_Enable(UINT32 u4Enable)
{
    if (u4Enable)
    {
        vAproc_start ();
        // enable ADSP part routine
        vWriteDspWORD (ADDR_RC2D_APROC_CONTROL, 0x0); // enable post
        _gu4AprocEnable = 1;
    }
    else
    {
        vAproc_stop ();
        vWriteDspWORD (ADDR_RC2D_APROC_CONTROL, 0x8000); // bypass post
        _gu4AprocEnable = 0;
    }
}

UINT32 u4Aproc_IsEnable(void)
{
    return _gu4AprocEnable;
}
 
 void vAprocMain_Init (void)
{
    x_os_isr_fct pfnOldIsr; 
    int i;

    // init Audio processor
    vAproc_Init ();
    
    // register ISR handler
    VERIFY(x_reg_isr(VECTOR_ARM9_0, vAprocIRQHandler0_ISR, &pfnOldIsr) == OSR_OK);
    VERIFY(x_reg_isr(VECTOR_ARM9_1, vAprocIRQHandler1_ISR, &pfnOldIsr) == OSR_OK);
    VERIFY(x_reg_isr(VECTOR_ARM9_2, vAprocIRQHandler2_ISR, &pfnOldIsr) == OSR_OK);
    VERIFY(x_reg_isr(VECTOR_ARM9_3, vAprocIRQHandler3_ISR, &pfnOldIsr) == OSR_OK);   

    // init sema
    VERIFY(x_sema_create(&_ahSemaAprocComDone, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
    VERIFY(x_sema_create(&_hAprocAccessSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);
    VERIFY(x_sema_create(&_hAprocSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);

    // init registers
    for (i = 0; i < APROC_REGMAP_SIZE; i ++)
    {
        vAprocReg_Write (i, 0x0);
    }
    
    // start Audio processor
    if (DRVCUST_OptGet(eAudioAprocEnable) == TRUE)
    {
        vAproc_start ();
        // enable ADSP part routine
        vWriteDspWORD (ADDR_RC2D_APROC_CONTROL, 0x0); // enable post
        _gu4AprocEnable = 1;
    }
    else
    {
        vWriteDspWORD (ADDR_RC2D_APROC_CONTROL, 0x8000); // bypass post
        _gu4AprocEnable = 0;
    }
}

//-----------------------------------------------------------------------------
/** vAprocPostProc_Open
 *  Audio processor open post-processing
 *
 *  @param           
 *  @retval 
 */
//-----------------------------------------------------------------------------
void vAprocPostProc_Open (void)
{
    LOG (5, "====> vAprocPostProc_Open\n");

    if (!_gu4AprocEnable)
    {
        return;
    }

    if (_gu4AprocPostProcControl == APROC_POSTPROC_OPEN)
    {
        return;
    }

    // init IO
    vAprocReg_Write (APROC_REG_IOBUF_IN_START, 
        u4GetAprocMemoryMap(APROC_MEMMAP_ADSP_CMPT)+(4*ADDR_CMPT_APROC_IO_INBUF_ADDR));
    vAprocReg_Write (APROC_REG_IOBUF_IN_END, 32);
    vAprocReg_Write (APROC_REG_IOBUF_IN_SIZE, 2 * 256 * 4);
    vAprocReg_Write (APROC_REG_IOBUF_IN_SI_BASE, 
        u4GetAprocMemoryMap(APROC_MEMMAP_ADSP_DRAM)+(4*ADDR_APROC_IO_INBUF_SI_ADDR));

    vAprocReg_Write (APROC_REG_IOBUF_OUT_START, 
        u4GetAprocMemoryMap(APROC_MEMMAP_ADSP_CMPT)+(4*ADDR_CMPT_APROC_IO_OUTBUF_ADDR));
    vAprocReg_Write (APROC_REG_IOBUF_OUT_END, 32);
    vAprocReg_Write (APROC_REG_IOBUF_OUT_SIZE, 2 * 256 * 4);
    vAprocReg_Write (APROC_REG_IOBUF_OUT_SI_BASE, 
        u4GetAprocMemoryMap(APROC_MEMMAP_ADSP_DRAM)+(4*ADDR_APROC_IO_OUTBUF_SI_ADDR));
    
    
    vAprocCmdOpen (APROC_SIG_IDX_POSTPROC);
    vAprocWaitCmdDone ();
}

//-----------------------------------------------------------------------------
/** vAprocPostProc_Close
 *  Audio processor close post-processing
 *
 *  @param           
 *  @retval 
 */
//-----------------------------------------------------------------------------
void vAprocPostProc_Close (void)
{
    if (!_gu4AprocEnable)
    {
        return;
    }

    if (_gu4AprocPostProcControl == APROC_POSTPROC_CLOSE)
    {
        return;
    }

    LOG (5, "====> vAprocPostProc_Close\n");
    vAprocCmdClose (APROC_SIG_IDX_POSTPROC);
    vAprocWaitCmdDone ();
    _gu4AprocPostProcControl = 0;
}
//added by ling
volatile UINT32 _gsu4AprocKernelDoneFlag = 0;

void vA2MCommNotifyKernelDone (void)
{
    _gsu4AprocKernelDoneFlag =  u4AprocBIM_read (APROC_A2M_DATA1_REG);
}

void vA2MCommDec0OpenDone (void)
{
    LOG (5, "====> vA2MCommDec0OpenDone\n");
    
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_DEC0, APROC_SIG_CMD_OPEN));    
}

void vA2MCommDec1OpenDone (void)
{
    LOG (5, "====> vA2MCommDec1OpenDone\n");
    
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_DEC1, APROC_SIG_CMD_OPEN));    
}

void vA2MCommDec2OpenDone (void)
{
    LOG (5, "====> vA2MCommDec2OpenDone\n");
    
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_DEC2, APROC_SIG_CMD_OPEN));    
}

void vA2MCommDec3OpenDone (void)
{
    LOG (5, "====> vA2MCommDec3OpenDone\n");
    
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_DEC3, APROC_SIG_CMD_OPEN));    
}




void vA2MCommAprocOpenDone (void)
{
    LOG (5, "====> vA2MCommAprocOpenDone\n");
    
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_APROC, APROC_SIG_CMD_OPEN));   
}


void vA2MCommDec0CloseDone (void)
{
    LOG (5, "====> vA2MCommDec0CloseDone\n");
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_DEC0, APROC_SIG_CMD_CLOSE));
    _gu4AprocDecControl[APROC_DEV_DEC0] = APROC_DEV_OPEN;
}

void vA2MCommDec1CloseDone (void)
{
    LOG (5, "====> vA2MCommDec1CloseDone\n");
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_DEC1, APROC_SIG_CMD_CLOSE));
    _gu4AprocDecControl[APROC_DEV_DEC1] = APROC_DEV_OPEN;
}

void vA2MCommDec2CloseDone (void)
{
    LOG (5, "====> vA2MCommDec2CloseDone\n");
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_DEC2, APROC_SIG_CMD_CLOSE));
    _gu4AprocDecControl[APROC_DEV_DEC2] = APROC_DEV_OPEN;
}

void vA2MCommDec3CloseDone (void)
{
    LOG (5, "====> vA2MCommDec3CloseDone\n");
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_DEC3, APROC_SIG_CMD_CLOSE));
    _gu4AprocDecControl[APROC_DEV_DEC3] = APROC_DEV_OPEN;
}

#endif

#ifdef CC_AUD_APOLLO_SUPPORT


extern const unsigned char _pbApollo_PMb[];
extern const unsigned char _pbApollo_DMb[];
extern const unsigned char _pbApollo_SMb[];

extern unsigned int _Apollo_GetPMSize(void);
extern unsigned int _Apollo_GetDMSize(void);
extern unsigned int _Apollo_GetSMSize(void);


//#define APOLLO_OS_TEST
#ifdef APOLLO_OS_TEST

#define OS_CONFIG_SINGLE_DM_SPACE

//#define OS_TEST_THREAD_NUM               (4)
#define OS_TEST_THREAD_NUM               (2) //include system thread

#define OS_MAX_THREAD_NUM                (6) //include system thread
#define OS_THREAD_NAME_SIZE              (16)
#define OS_MAX_THREAD_PRIORITY           (9)
#define OS_SYS_THREAD_PRIORITY           (OS_MAX_THREAD_PRIORITY)

#define OS_DM_EXT_SA                     (0x20000)
#define OS_DM_THREAD_SV_SIZE             (256*4)
#define OS_DM_THREAD_STACK_BAK_SIZE      (512*4)
#define OS_DM_THREAD_TEMPBUF_BAK_SIZE    (1024*12*4)

#ifdef OS_CONFIG_SINGLE_DM_SPACE          // 0x6B400(X), 0x5F400(O)
//#define OS_R2A_SYSTEM_INFO_SA            (0x20000 + OS_DM_THREAD_SV_SIZE + (OS_DM_THREAD_STACK_BAK_SIZE+OS_DM_THREAD_TEMPBUF_BAK_SIZE)*OS_MAX_THREAD_NUM)
#define OS_R2A_SYSTEM_INFO_SA            (0x20000 + OS_DM_THREAD_SV_SIZE + (OS_DM_THREAD_STACK_BAK_SIZE+OS_DM_THREAD_TEMPBUF_BAK_SIZE)*(OS_MAX_THREAD_NUM-1)+ OS_DM_THREAD_STACK_BAK_SIZE)
#else
#define OS_R2A_SYSTEM_INFO_SA            (0x20C00)
#endif

#define OS_R2A_SYSTEM_INFO_SIZE          (0x200)
#define OS_R2A_THREAD_INFO_SA            (OS_R2A_SYSTEM_INFO_SA + OS_R2A_SYSTEM_INFO_SIZE)

#define OS_TEST_TASK_DM_BLK_OFFSET       (0x20000)
#define OS_STACK_OV_SIZE                 (256*4)

typedef struct {

    /* thread info */
    UINT32      u4ThreadInfoSa;
    UINT32      u4ThreadInfoNum;

    /* thread data sram table address */
    UINT32      u4ThreadSramSa[OS_MAX_THREAD_NUM];
    UINT32      u4ThreadSramSize[OS_MAX_THREAD_NUM];  // note : size should be 8 bytes alignment for fast vector memcpy

    // extension

} OS_SYSTEM_INFO;

typedef struct {
    UINT32         u4Prio;
    char           Name[OS_THREAD_NAME_SIZE];
    UINT32         u4PmSa;
    UINT32         u4DmBlkSa;
    UINT32         u4DmBlkOffset;
    UINT32         u4WorkBufSa;
    UINT32         u4TimeSlice;  
} OS_R2A_THREADINFO;

OS_SYSTEM_INFO tSystemInfo = {
   OS_R2A_THREAD_INFO_SA, OS_TEST_THREAD_NUM
};   

OS_R2A_THREADINFO tR2AThreadInfo[OS_MAX_THREAD_NUM] =
{
    {OS_SYS_THREAD_PRIORITY,  "SYS_THREAD" ,  0x0,  0x0, 0x0, 0x0,  1},
    {       3,                "THREAD_1"   ,  0x0,  0x0, 0x0, 0x0, 1},
    {       3,                "THREAD_2"   ,  0x0,  0x0, 0x0, 0x0,  0},
    {       3,                "THREAD_3"   ,  0x0,  0x0, 0x0, 0x0,  0},
    {       3,                "THREAD_4"   ,  0x0,  0x0, 0x0, 0x0,  0},
    {       3,                "THREAD_5"   ,  0x0,  0x0, 0x0, 0x0,  0}
};
#endif

static void vAproc_Init (void)
{
    UINT32 u4AprocWokBuf;
#ifdef APOLLO_OS_TEST    
    INT32  i;
    UINT32 u4DmSblk;
#endif


    // disable Apollo
    APOLLO_WRITE32(APOLLO_REG_CFG, 0x320);

    // reset Audio ARM
    //vIO32WriteFldAlign (CKGEN_REG_ACPU_CKCFG, 1, FLD_ACPU_PD);
    //vIO32WriteFldAlign (CKGEN_REG_ACPU_CKCFG, 0, FLD_ACPU_PD);

    // set Audio ARM AHB speed
    //vIO32WriteFldAlign (CKGEN_REG_ACPU_CKCFG, 1, FLD_ACPU_MISC); // /2

    // set Audio Apollo speed
    AUD_ApolloClkSel(11); // 3: syspll_d2_ck (324MHz) or 4: tapll_d1_ck (372MHz) 
                          // 11: G3D_PLL (current 252MHz)

    // Initial Audio ARM memory    
    vApolloMemInit ((UINT8 *) u4GetDspWorkBufNonInit());

    // setup base address
    APOLLO_WRITE32(APOLLO_REG_KPMEM_SADR, 0);                        //No Program ROM/SRAM
    APOLLO_WRITE32(APOLLO_REG_UPMEM_SADR, 0xF0000000);
    u4AprocWokBuf = u4GetApolloWorkBufNonInit(0);
    APOLLO_WRITE32(APOLLO_REG_KPADDR_BASE, u4AprocWokBuf);
    APOLLO_WRITE32(APOLLO_REG_DMEM_SBLK, u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR);
    APOLLO_WRITE32(APOLLO_REG_CMPT_SBLK, u4GetApolloWorkBufNonInit(1));
    // load PM and DM    
    u4AudApolloLoadCode (u4AprocWokBuf,                   (UINT32) &_pbApollo_PMb[0], _Apollo_GetPMSize()); 
#if 1 //modify by ling   
    //u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1), (UINT32) &_pbApollo_DMb[0], _Apollo_GetDMSize());            
    //u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1)+(APOLLO_KERNEL_DATA_DRAM_SADR-APOLLO_EXT_MEM_SADR), (UINT32) &_pbApollo_DMb[0], _Apollo_GetDMSize()); //load to 0x62000~           
    u4AudApolloLoadCode (u4GetApolloWorkBufNonInit(1)+(APOLLO_SRAM_TEMP_SADR-APOLLO_EXT_MEM_SADR), (UINT32) &_pbApollo_SMb[0], _Apollo_GetSMSize()); //load to 0x5F800~ 0x5FFFF          
#endif



#ifdef APOLLO_OS_TEST
    Printf("\n --  Apollo Mini-OS releated setting --");

#ifdef OS_CONFIG_SINGLE_DM_SPACE
    for (i = 0; i < OS_TEST_THREAD_NUM; i++)
    {
        tR2AThreadInfo[i].u4DmBlkSa = u4GetApolloWorkBufNonInit(1);  
        tR2AThreadInfo[i].u4DmBlkOffset = 0;
    }
  #if 0
    // init SV region to 0x5A5A5A5A
    memset((void *)(tR2AThreadInfo[0].u4DmBlkSa), 0x5A, OS_STACK_OV_SIZE);
  #else
    for(i = 0; i < (OS_STACK_OV_SIZE/4); i++)
       vApollo_SetDram(APOLLO_EXT_MEM_SADR+i*4, 0x5A5A5A5A);

  #endif
       
#else
    // fill physical DMEM address 
    tR2AThreadInfo[0].u4DmBlkSa = u4GetApolloWorkBufNonInit(1);  
    tR2AThreadInfo[1].u4DmBlkSa = u4GetApolloWorkBufNonInit(1) + OS_TEST_TASK_DM_BLK_OFFSET;
    tR2AThreadInfo[2].u4DmBlkSa = u4GetApolloWorkBufNonInit(1) + OS_TEST_TASK_DM_BLK_OFFSET*2;
    tR2AThreadInfo[3].u4DmBlkSa = u4GetApolloWorkBufNonInit(1) + OS_TEST_TASK_DM_BLK_OFFSET*3;

    tR2AThreadInfo[0].u4DmBlkOffset = 0;
    tR2AThreadInfo[1].u4DmBlkOffset = OS_TEST_TASK_DM_BLK_OFFSET;
    tR2AThreadInfo[2].u4DmBlkOffset = OS_TEST_TASK_DM_BLK_OFFSET*2;
    tR2AThreadInfo[3].u4DmBlkOffset = OS_TEST_TASK_DM_BLK_OFFSET*3;

    // init SV region to 0x50+thread_id
    for (i = 0; i < OS_TEST_THREAD_NUM; i++)
    {
        memset((void *)(tR2AThreadInfo[i].u4DmBlkSa), 0x50+i, OS_STACK_OV_SIZE);
    }
    
#endif

    //tR2AThreadInfo[1].u4PmSa = 0x10000;
    //tR2AThreadInfo[2].u4PmSa = 0x20000;
    //tR2AThreadInfo[3].u4PmSa = 0x30000;
    
    for (i = 0; i < OS_TEST_THREAD_NUM; i++)
    {
        Printf("\n   Thread [%d] DMEM Block Physical Addr @  0x%x", i, tR2AThreadInfo[i].u4DmBlkSa);
    }
   #if 0
    // fill system info to OS_R2A_SYSTEM_INFO_SA
    memcpy((void *)(u4GetApolloWorkBufNonInit(1) + OS_R2A_SYSTEM_INFO_SA - APOLLO_EXT_MEM_SADR),
           &tSystemInfo,  sizeof(OS_SYSTEM_INFO) * OS_MAX_THREAD_NUM);

    // fill thread info to OS_R2A_THREAD_INFO_SA
    memcpy((void *)(u4GetApolloWorkBufNonInit(1) + OS_R2A_THREAD_INFO_SA - APOLLO_EXT_MEM_SADR),
           &tR2AThreadInfo[0],  sizeof(OS_R2A_THREADINFO) * OS_MAX_THREAD_NUM);
   #else
    // fill system info to OS_R2A_SYSTEM_INFO_SA
    memcpy((void *)(u4GetApolloWorkBufNonInit(1) + OS_R2A_SYSTEM_INFO_SA - APOLLO_EXT_MEM_SADR),
           &tSystemInfo,  sizeof(OS_SYSTEM_INFO) * OS_MAX_THREAD_NUM);
    for(i = 0; i < ((sizeof(OS_SYSTEM_INFO) * OS_MAX_THREAD_NUM)/4); i++)
    {
          u4DmSblk = u4GetApolloWorkBufNonInit(1)+ (OS_R2A_SYSTEM_INFO_SA - APOLLO_EXT_MEM_SADR) + (i*4);
          u4DmSblk &= 0xFFFFFFE0; //make sure addr & len is 32 byte alignment.
          HalFlushInvalidateDCacheMultipleLine(VIRTUAL(u4DmSblk), 0x20);
    }

    // fill thread info to OS_R2A_THREAD_INFO_SA
    memcpy((void *)(u4GetApolloWorkBufNonInit(1) + OS_R2A_THREAD_INFO_SA - APOLLO_EXT_MEM_SADR),
           &tR2AThreadInfo[0],  sizeof(OS_R2A_THREADINFO) * OS_MAX_THREAD_NUM);

    for(i = 0; i < ((sizeof(OS_R2A_THREADINFO) * OS_MAX_THREAD_NUM)/4); i++)
    {
          u4DmSblk = u4GetApolloWorkBufNonInit(1)+ (OS_R2A_THREAD_INFO_SA - APOLLO_EXT_MEM_SADR) + (i*4);
          u4DmSblk &= 0xFFFFFFE0; //make sure addr & len is 32 byte alignment.
          HalFlushInvalidateDCacheMultipleLine(VIRTUAL(u4DmSblk), 0x20);
    }      
   #endif
#endif
    //set DSP cmpt page 6 aligned with APOLLO_REG_CMPT_SBLK
    IniBlk6Regs(u4GetApolloWorkBufNonInit(1));
}


void vAproc_Disable (void)
{
    // disable Apollo
  #if 1  
    APOLLO_WRITE32(APOLLO_REG_CFG, 0x320);
  #else
    vApolloSet_DspApolloNOTASYNC(); 
    vApolloSet_disable();
  #endif    
}


void vAproc_Enable (void)    // enable apollo in 4-way cache
{
#if 0
    APOLLO_WRITE32(APOLLO_REG_CFG, 0x321);//flush
    APOLLO_WRITE32(APOLLO_REG_CFG, 0x10000330);//enable
#else
  #if 1
    APOLLO_WRITE32(APOLLO_REG_CFG, 0x320);//setting 4-way cache
    APOLLO_WRITE32(APOLLO_REG_CFG, 0x321);//flush
    APOLLO_WRITE32(APOLLO_REG_CFG, 0x10000330);//enable
  #else 
    vApolloSet_PgCache4Way();
    vApolloSet_FlushPMCache();
    vApolloSet_DspApolloASYNC();    
    vApolloSet_enable();
    vApolloSet_ClearFlushBit();
  #endif
#endif
}

void vAproc_2WayEnable (void) // enable apollo in 2-way cache
{
#if 0
    APOLLO_WRITE32(APOLLO_REG_CFG, 0x301);//flush
    APOLLO_WRITE32(APOLLO_REG_CFG, 0x10000310);//enable
#else
  #if 1
    APOLLO_WRITE32(APOLLO_REG_CFG, 0x300);//setting 2-way cache
    APOLLO_WRITE32(APOLLO_REG_CFG, 0x301);//flush
    APOLLO_WRITE32(APOLLO_REG_CFG, 0x10000310);//enable
  #else 
    vApolloSet_PgCache2Way();
    vApolloSet_FlushPMCache();
    vApolloSet_DspApolloASYNC();    
    vApolloSet_enable();
    vApolloSet_ClearFlushBit();
  #endif        
#endif
}



/* Semaphore for Audio processor resource accessing
*/
void vAprocLockAccessSema(void)
{
    VERIFY(x_sema_lock(_hAprocAccessSema, X_SEMA_OPTION_WAIT)== OSR_OK);
}

void vAprocUnlockAccessSema(void)
{
    VERIFY(x_sema_unlock(_hAprocAccessSema)== OSR_OK);
}

/* Semaphore for Audio processor
*/
void vAprocLockSema(void)
{
    VERIFY(x_sema_lock(_hAprocSema, X_SEMA_OPTION_WAIT)== OSR_OK);
}

void vAprocUnlockSema(void)
{
    VERIFY(x_sema_unlock(_hAprocSema)== OSR_OK);
}

/* audio processor command done
*/
void vAprocCmdDone (UINT32 u4Command)
{
    LOG (5, "vAprocCmdDone: 0x%x\n", u4Command);
    
    VERIFY(x_sema_unlock(_ahSemaAprocComDone) == OSR_OK);
} 

/* audio processor wait command done
*/
void vAprocWaitCmdDone (void)
{
    INT32 i4Ret;

    LOG(5, "vAprocWaitCmdDone ...\n");

#ifndef FIX_AUD_UOP_TIMEOUT
    // Try lock to make sure the semaphore was locked
    i4Ret = x_sema_lock(_ahSemaAprocComDone, X_SEMA_OPTION_NOWAIT);
    VERIFY((i4Ret == OSR_OK) || (i4Ret == OSR_WOULD_BLOCK));
#endif

    // Real lock ..
    i4Ret = x_sema_lock_timeout(_ahSemaAprocComDone, 5000);
    VERIFY((i4Ret == OSR_OK) || (i4Ret == OSR_TIMEOUT));

    LOG(5, "vAprocWaitCmdDone: wake-up!\n");
    
    if (i4Ret == OSR_TIMEOUT)
    {
        LOG(1, "vAprocWaitCmdDone TIMEOUT\n");
        // fix me!!!! ....
    }
}


/* Add [A] to Log
*/
static CHAR *_cAudAprocLogTransfer (CHAR *cIn )
{
    UINT32 i;

    // provide prefix
    _gcStringBuf[0] = '[';
    _gcStringBuf[1] = 'A';
    _gcStringBuf[2] = ']';

    // copy stream
    for (i = 0; i < AUD_APROC_CHAR_NUM; i ++)
    {
        _gcStringBuf[i+3] = cIn[i]; 
    }   

    // provide end
    _gcStringBuf[AUD_APROC_CHAR_LEN-2] = 0x00;
    _gcStringBuf[AUD_APROC_CHAR_LEN-1] = 0x0A;

    return (CHAR *) _gcStringBuf;
}

/* Audio processor, Audio processor to Main ARM log handler
*/

// ISR handler for Audio processor Logs
static void vAprocCommLog_handler (void)
{
    INT32 i4Val0, i4Val1, i4Val2, i4Val3;
    CHAR *cLog;

    i4Val0 = u4AprocBIM_read (APROC_A2M_LOG_DATA0);
    i4Val1 = u4AprocBIM_read (APROC_A2M_LOG_DATA1);
    i4Val2 = u4AprocBIM_read (APROC_A2M_LOG_DATA2);
    i4Val3 = u4AprocBIM_read (APROC_A2M_LOG_DATA3);

    //cLog= _cAudAprocLogTransfer ((CHAR *) (u4AprocBIM_read (APROC_A2M_LOG_ADDR) + 
    //    u4GetAprocMemoryMap(APROC_MEMMAP_WORKBUF)));
    
    cLog= _cAudAprocLogTransfer ((CHAR *) (u4AprocBIM_read (APROC_A2M_LOG_ADDR) + 
        u4GetApolloWorkBufNonInit(1)-APOLLO_EXT_MEM_SADR));

    LOG (1, cLog, i4Val0, i4Val1, i4Val2, i4Val3);
}

// ISR handler for Audio processor ISR Logs
#if 0 //This Interrupt has been changed to IsrSignal usage.
static void vAprocCommIsrLog_handler (void)
{
    INT32 i4Val0, i4Val1, i4Val2, i4Val3;
    CHAR *cLog;

    i4Val0 = u4AprocBIM_read (APROC_A2M_ISR_LOG_DATA0);
    i4Val1 = u4AprocBIM_read (APROC_A2M_ISR_LOG_DATA1);
    i4Val2 = u4AprocBIM_read (APROC_A2M_ISR_LOG_DATA2);
    i4Val3 = u4AprocBIM_read (APROC_A2M_ISR_LOG_DATA3);

    cLog = _cAudAprocLogTransfer ((CHAR *) (u4AprocBIM_read (APROC_A2M_ISR_LOG_ADDR) + 
        u4GetAprocMemoryMap(APROC_MEMMAP_WORKBUF)));

    LOG (7, cLog, i4Val0, i4Val1, i4Val2, i4Val3);
}
#endif


//added by ling
volatile UINT32 _gsu4AprocKernelDoneFlag = 0;

void vA2MCommNotifyKernelDone (void)
{
    _gsu4AprocKernelDoneFlag =  u4AprocBIM_read (APROC_A2M_DATA1_REG);
}

void vA2MCommKernelOpenDone (void)
{
    LOG (5, "====> vA2MCommKernelOpenDone\n");
}

#ifdef CC_AUD_APOLLO_POST_SUPPORT
void vA2MCommPostProcOpenDone (void)
{
    LOG (5, "====> vA2MCommPostProcOpenDone\n");
    
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_POSTPROC, APROC_SIG_CMD_OPEN));
    _gu4AprocPostProcControl = APROC_POSTPROC_OPEN;
}

void vA2MCommPostProcCloseDone (void)
{
    LOG (5, "====> vA2MCommPostProcCloseDone\n");
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_POSTPROC, APROC_SIG_CMD_CLOSE));
    _gu4AprocPostProcControl = APROC_POSTPROC_CLOSE;
}
#endif
void vA2MCommDec0OpenDone (void)
{
    LOG (5, "====> vA2MCommDec0OpenDone\n");
    
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_DEC0, APROC_SIG_CMD_OPEN));    
}

void vA2MCommDec1OpenDone (void)
{
    LOG (5, "====> vA2MCommDec1OpenDone\n");
    
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_DEC1, APROC_SIG_CMD_OPEN));    
}

void vA2MCommDec2OpenDone (void)
{
    LOG (5, "====> vA2MCommDec2OpenDone\n");
    
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_DEC2, APROC_SIG_CMD_OPEN));    
}

void vA2MCommDec3OpenDone (void)
{
    LOG (5, "====> vA2MCommDec3OpenDone\n");
    
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_DEC3, APROC_SIG_CMD_OPEN));    
}




void vA2MCommAprocOpenDone (void)
{
    LOG (5, "====> vA2MCommAprocOpenDone\n");
    
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_APROC, APROC_SIG_CMD_OPEN));   
}


void vA2MCommDec0CloseDone (void)
{
    LOG (5, "====> vA2MCommDec0CloseDone\n");
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_DEC0, APROC_SIG_CMD_CLOSE));
    _gu4AprocDecControl[APROC_DEV_DEC0] = APROC_DEV_OPEN;
}

void vA2MCommDec1CloseDone (void)
{
    LOG (5, "====> vA2MCommDec1CloseDone\n");
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_DEC1, APROC_SIG_CMD_CLOSE));
    _gu4AprocDecControl[APROC_DEV_DEC1] = APROC_DEV_OPEN;
}

void vA2MCommDec2CloseDone (void)
{
    LOG (5, "====> vA2MCommDec2CloseDone\n");
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_DEC2, APROC_SIG_CMD_CLOSE));
    _gu4AprocDecControl[APROC_DEV_DEC2] = APROC_DEV_OPEN;
}

void vA2MCommDec3CloseDone (void)
{
    LOG (5, "====> vA2MCommDec3CloseDone\n");
    vAprocCmdDone (APROC_SIGNAL (APROC_SIG_IDX_DEC3, APROC_SIG_CMD_CLOSE));
    _gu4AprocDecControl[APROC_DEV_DEC3] = APROC_DEV_OPEN;
}

#if 0
void vA2MMixSound0UpdateRp(void)//parson mixsnd
{
    UINT32 u4Rp;

    u4Rp = u4AprocBIM_read (APROC_A2M_DATA1_REG);
    LOG (9, "====> vA2MMixSoundUpdateRp  %d\n", u4Rp);    
    AUD_AprocMixSndUpdateRp(0, u4Rp);
}
void vA2MMixSound1UpdateRp(void)
{
    UINT32 u4Rp;

    u4Rp = u4AprocBIM_read (APROC_A2M_DATA1_REG);
    LOG (9, "====> vA2MMixSoundUpdateRp  %d\n", u4Rp);    
    AUD_AprocMixSndUpdateRp(1, u4Rp);
}

void vA2MUploadUpdateWp(void)
{
    UINT32 u4Wp;

    u4Wp = u4AprocBIM_read (APROC_A2M_DATA1_REG);
    LOG (9, "====> vA2MUploadUpdateWp  %d\n", u4Wp);    
    AUD_AprocUploadUpdateWp(u4Wp);
}
#endif

void vA2MCommIoInitDone (void)
{
    LOG (5, "====> vA2MCommIoInitDone\n");
}


void vA2MNotifySRC (void)
{
    vA2MNotifyAPTS0 ();
    vA2MNotifyAPTS1 ();
    vA2MNotifyAPTS2 ();
    vA2MNotifyHoldComplete0 ();
    vA2MNotifyHoldComplete1 ();
    vA2MNotifyHoldComplete2 ();
}

void vA2MNotifyUpdateRountine (void)
{
    UINT8 u1DecId;
    UINT32 u4Offset;

    u1DecId = (UINT8)u4AprocBIM_read (APROC_A2M_DATA1_REG);
    if (u1DecId > AUD_DEC_THIRD)
    {
        return;
    }    

    switch (u1DecId)
    {
    case AUD_DEC_MAIN:
        u4Offset = APROC_REG_DMIX_AMIXER0_WAIT;
        break;
    case AUD_DEC_AUX:
        u4Offset = APROC_REG_DMIX_AMIXER1_WAIT;
        break;        
    case AUD_DEC_THIRD:
        u4Offset = APROC_REG_DMIX_AMIXER2_WAIT;
        break;        
    default:
        return;
    }        
    
    // SetUpDownmix Tables.
    // Todo ..........
    LOG(7, "Dec(%d) : CR - Ch_Num from APROC is %d\n", u1DecId,
            u4AprocReg_Read(APROC_ASM_ADDR(APROC_ASM_ID_POSTPROC_4, (APROC_REG_DMIX_AMIXER0_INFO+u1DecId))) );
    //vAprocDataPort_Write (APROC_DATAPORT_DOWNMIX_TBL_BASE, pu4Value, APROC_DATAPORT_DOWNMIX_TBL_SIZE);

    // Set Done to APROC by ASM
    vAprocReg_Write (APROC_ASM_ADDR (APROC_ASM_ID_POSTPROC_4, u4Offset), 0);    
}

void vA2MNotifyAPTS0(void)
{
    UINT32 u4PTS;
    
    UNUSED(u4PTS);
    if (u4AprocReg_Read (APROC_ASM_ADDR(APROC_ASM_ID_AVSYNC_0, APROC_REG_AVSYNC_NOTIFY_APTS_SRC0)))
    {
        u4PTS = u4AprocReg_Read (APROC_ASM_ADDR(APROC_ASM_ID_AVSYNC_0, APROC_REG_APTS_SRC0));
        //AUD_Aproc_Calc_StcDiff(AUD_DEC_MAIN, u4PTS);
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_AVSYNC_0, APROC_REG_AVSYNC_NOTIFY_APTS_SRC0), 0x0);
    }
}
void vA2MNotifyAPTS1(void)
{
    UINT32 u4PTS;
    
    UNUSED(u4PTS);
    if (u4AprocReg_Read (APROC_ASM_ADDR(APROC_ASM_ID_AVSYNC_0, APROC_REG_AVSYNC_NOTIFY_APTS_SRC1)))
    {
        u4PTS = u4AprocReg_Read (APROC_ASM_ADDR(APROC_ASM_ID_AVSYNC_0, APROC_REG_APTS_SRC1));
        //AUD_Aproc_Calc_StcDiff(AUD_DEC_AUX, u4PTS);
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_AVSYNC_0, APROC_REG_AVSYNC_NOTIFY_APTS_SRC1), 0x0);
    }
}
void vA2MNotifyAPTS2(void)
{
    UINT32 u4PTS;
    
    UNUSED(u4PTS);
    if (u4AprocReg_Read (APROC_ASM_ADDR(APROC_ASM_ID_AVSYNC_0, APROC_REG_AVSYNC_NOTIFY_APTS_SRC2)))
    {
        u4PTS = u4AprocReg_Read (APROC_ASM_ADDR(APROC_ASM_ID_AVSYNC_0, APROC_REG_APTS_SRC2));
        //AUD_Aproc_Calc_StcDiff(AUD_DEC_THIRD, u4PTS);
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_AVSYNC_0, APROC_REG_AVSYNC_NOTIFY_APTS_SRC2), 0x0);
    }
}
void vA2MNotifyHoldComplete0(void)
{
    if (u4AprocReg_Read (APROC_ASM_ADDR(APROC_ASM_ID_AVSYNC_0, APROC_REG_AVSYNC_NOTIFY_HOLD_OK_SRC0)))
    {
        AUD_Aproc_Chg_AVSyncCtrl(AUD_DEC_MAIN, APROC_AVSYNC_CTRL_HOLD_COMPLETE);
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_AVSYNC_0, APROC_REG_AVSYNC_NOTIFY_HOLD_OK_SRC0), 0x0);
        LOG (8, "====> vA2MNotifyHoldComplete0\n");
    }
}
void vA2MNotifyHoldComplete1(void)
{
    if (u4AprocReg_Read (APROC_ASM_ADDR(APROC_ASM_ID_AVSYNC_0, APROC_REG_AVSYNC_NOTIFY_HOLD_OK_SRC1)))
    {
        AUD_Aproc_Chg_AVSyncCtrl(AUD_DEC_AUX, APROC_AVSYNC_CTRL_HOLD_COMPLETE);
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_AVSYNC_0, APROC_REG_AVSYNC_NOTIFY_HOLD_OK_SRC1), 0x0);
        LOG (8, "====> vA2MNotifyHoldComplete1\n");
    } 
}
void vA2MNotifyHoldComplete2(void)
{
    if (u4AprocReg_Read (APROC_ASM_ADDR(APROC_ASM_ID_AVSYNC_0, APROC_REG_AVSYNC_NOTIFY_HOLD_OK_SRC2)))
    {
        AUD_Aproc_Chg_AVSyncCtrl(AUD_DEC_THIRD, APROC_AVSYNC_CTRL_HOLD_COMPLETE);
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_AVSYNC_0, APROC_REG_AVSYNC_NOTIFY_HOLD_OK_SRC2), 0x0);
        LOG (8, "====> vA2MNotifyHoldComplete2\n");
    }  
}

/* Audio processpr ISR functions
*/ 
static void vAprocCommSignal_handler (void)
{
    UINT32 u4Cmd, u4Idx;
    _AUD_APROC_COMM_SIG_T *eInfo = (_AUD_APROC_COMM_SIG_T *) _gu4AudAprocA2MSigInfo;
    
    u4Cmd = u4AprocBIM_read (APROC_A2M_SIG_REG);
    u4Idx = u4AudAprocSig_GetIdx(u4Cmd);
    u4Cmd = u4AudAprocSig_GetCMD(u4Cmd);

    while (1)
    {
        if ((eInfo->u4Cmd == 0) && (eInfo->u4Idx == 0))
        {
            // exit case
            break;
        }

        if ((eInfo->u4Cmd == u4Cmd) && (eInfo->u4Idx == u4Idx))
        {
            //  command hit
            if (Aproc_Sig_A2M_Is_Hit (eInfo->u4Data0, u4AprocBIM_read (APROC_A2M_DATA0_REG)) &&
                Aproc_Sig_A2M_Is_Hit (eInfo->u4Data1, u4AprocBIM_read (APROC_A2M_DATA1_REG)) )
            {
                if (eInfo->u4Func)
                {
                    eInfo->u4Func ();
                }
                break;
            }
        }
        eInfo ++;
    }
}

static void vAprocISRCommSignal_handler (void)
{
    UINT32 u4Cmd, u4Idx;
    _AUD_APROC_COMM_SIG_T *eInfo = (_AUD_APROC_COMM_SIG_T *) _gu4AudAprocA2MSigInfo;
    
    u4Cmd = u4AprocBIM_read (APROC_A2M_ISR_LOG_ADDR);
    u4Idx = u4AudAprocSig_GetIdx(u4Cmd);
    u4Cmd = u4AudAprocSig_GetCMD(u4Cmd);

    while (1)
    {
        if ((eInfo->u4Cmd == 0) && (eInfo->u4Idx == 0))
        {
            // exit case
            break;
        }

        if ((eInfo->u4Cmd == u4Cmd) && (eInfo->u4Idx == u4Idx))
        {
            //  command hit
            if (Aproc_Sig_A2M_Is_Hit (eInfo->u4Data0, u4AprocBIM_read (APROC_A2M_ISR_LOG_DATA0)) &&
                Aproc_Sig_A2M_Is_Hit (eInfo->u4Data1, u4AprocBIM_read (APROC_A2M_ISR_LOG_DATA1)) )
            {
                if (eInfo->u4Func)
                {
                    eInfo->u4Func ();
                }
                break;
            }
        }
        eInfo ++;
    }
}

/* Audio processpr ISR handlers
*/ 
void vApolloIRQHandler0_ISR(void)
{
    vAprocCommSignal_handler ();
    
}

void vApolloIRQHandler1_ISR(void)
{
    vAprocCommLog_handler ();
    
}

void vApolloIRQHandler2_ISR(void)
{
    vAprocISRCommSignal_handler ();
     
}

void vApolloIRQHandler3_ISR(void)
{
    LOG (0, "=====> vAprocIRQHandler3_ISR\n");
    
}

// very important flag!!!
// It controls audio processor driver part enable.
// If audio processor is disable, all functions in driver related to audio proecssor must be "mute".
UINT32 _gu4AprocEnable = 0; // fix me!!!!

void vAprocS_Enable(UINT32 u4Enable)
{
    if (u4Enable)
    {
        vAproc_Enable ();
        // enable ADSP part routine
        vWriteDspWORD (ADDR_RC2D_APROC_CONTROL, 0x0); // enable post
        _gu4AprocEnable = 1;
    }
    else
    {
        vAproc_Disable ();
        vWriteDspWORD (ADDR_RC2D_APROC_CONTROL, 0x8000); // bypass post
        _gu4AprocEnable = 0;
    }
}

#ifdef CC_AUD_APOLLO_POST_SUPPORT
void vAprocS_EnableConfig(BOOL fgEnable)
{
    if (fgEnable)
    {
        //vAproc_Enable ();
        // enable ADSP part routine
        vWriteDspWORD (ADDR_RC2D_APROC_CONTROL, 0x0); // enable post
        _gu4AprocEnable = 1;
    }
    else
    {
        //vAproc_Disable ();
        vWriteDspWORD (ADDR_RC2D_APROC_CONTROL, 0x8000); // bypass post
        _gu4AprocEnable = 0;
    }
}
#endif

UINT32 u4Aproc_IsEnable(void)
{
    return _gu4AprocEnable;
}

UINT32 u4AprocPostProc_IsEnable(void)
{
    return _gu4AprocPostProcControl;
}

BOOL bIsSupportAproc (void)
{
    //return 1;//DRVCUST_OptGet(eAudioAprocEnable);
    return DRVCUST_OptGet(eAudioAprocEnable);
}

void vAprocMain_Init (void)
{
    //x_os_isr_fct pfnOldIsr; 
    int i;
#if 0   
    UINT32 u4Addr, u4Size;
#endif
    static BOOL fgSemaInit = FALSE;

    // init Audio processor
    vAproc_Init ();


    // init sema
    if (!fgSemaInit)
    {
        VERIFY(x_sema_create(&_ahSemaAprocComDone, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
        VERIFY(x_sema_create(&_hAprocAccessSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);
        VERIFY(x_sema_create(&_hAprocSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);
        fgSemaInit = TRUE;
    }

    // init registers
    for (i = 0; i < APROC_REGMAP_SIZE; i ++)
    {
        vApolloReg_Write (i, 0x0);
    }

     vApolloReg_Write (APROC_ASM_ADDR (APROC_ASM_ID_COMM_0, APROC_REG_CPUSIDE_BASE), u4GetApolloWorkBufNonInit(1)-0x20000);


#if 0 //waiting for APROC READY! 
    // start Audio processor
    if (bIsSupportAproc () == TRUE)
    {
        vAproc_2WayEnable();
        Printf ("##### vAproc_2WayEnable #######\n");

  #if 1
    #if defined(CC_MT5399)
      if (BSP_IsFPGA()) AUD_WRITE32(REG_TEST_CFG, 0x6000);  //change to 0x6000 by Shihhsin/Gallen for MT5399 U2 FPGA
      else AUD_WRITE32(REG_TEST_CFG, 0x4000);               // for real MT5399 IC
    #else
      AUD_WRITE32(REG_TEST_CFG, 0x2000);  //change to DRAM agent group 2
    #endif
  #endif
        
        // enable ADSP part routine
        vWriteDspWORD (ADDR_RC2D_APROC_CONTROL, 0x0); // enable post
        _gu4AprocEnable = 1;

#if 0 //enable when "APOLLO_DRIVER_READY" define is open in APOLLO
        // wait for APROC kernel OK
        while (1)
        {
            if (_gsu4AprocKernelDoneFlag)
            {
                Printf ("====> APROC version = 0x%x\n", _gsu4AprocKernelDoneFlag);
                break;
            }
            x_thread_delay (1);

        }
#endif      
    }
    else
    {
        vWriteDspWORD (ADDR_RC2D_APROC_CONTROL, 0x8000); // bypass post
        _gu4AprocEnable = 0;
    }
#else
#ifdef CC_AUD_APOLLO_POST_SUPPORT
        // enable ADSP part routine
        if (bIsSupportAproc () == TRUE)
        {
            vWriteDspWORD (ADDR_RC2D_APROC_CONTROL, 0x0); // enable post
            _gu4AprocEnable = 1;
        }    
        else
        {
            vWriteDspWORD (ADDR_RC2D_APROC_CONTROL, 0x8000); // bypass post  
            _gu4AprocEnable = 0;
        }    
#endif
        vAproc_2WayEnable();
#endif
}


#ifdef CC_AUD_APOLLO_POST_SUPPORT
void vAprocPostProc_Open (void)
{
    //LOG (0, "====> vAprocPostProc_Open, _gu4AprocEnable = %d, _gu4AprocPostProcControl = %d\n", _gu4AprocEnable, _gu4AprocPostProcControl);

    if (!_gu4AprocEnable)
    {
        return;
    }

    if (_gu4AprocPostProcControl == APROC_POSTPROC_OPEN)
    {
        return;
    }

    // init IO
    vAprocReg_Write (APROC_REG_IOBUF_IN_START, 
        u4GetAprocMemoryMap(APROC_MEMMAP_ADSP_CMPT)+ 0x20000+(4*ADDR_CMPT_APROC_IO_INBUF_ADDR));
    vAprocReg_Write (APROC_REG_IOBUF_IN_END, 2); //max 32
    vAprocReg_Write (APROC_REG_IOBUF_IN_SIZE, 2 * 256 * 4);
    vAprocReg_Write (APROC_REG_IOBUF_IN_SI_BASE, 
        u4GetAprocMemoryMap(APROC_MEMMAP_ADSP_DRAM)+ 0x20000+(4*ADDR_APROC_IO_INBUF_SI_ADDR));

    vAprocReg_Write (APROC_REG_IOBUF_OUT_START, 
        u4GetAprocMemoryMap(APROC_MEMMAP_ADSP_CMPT)+ 0x20000+(4*ADDR_CMPT_APROC_IO_OUTBUF_ADDR));
    vAprocReg_Write (APROC_REG_IOBUF_OUT_END, 2); //max 32
    vAprocReg_Write (APROC_REG_IOBUF_OUT_SIZE, 2 * 256 * 4);
    vAprocReg_Write (APROC_REG_IOBUF_OUT_SI_BASE, 
        u4GetAprocMemoryMap(APROC_MEMMAP_ADSP_DRAM)+ 0x20000+(4*ADDR_APROC_IO_OUTBUF_SI_ADDR));
    
    
    vAprocCmdOpen (APROC_SIG_IDX_POSTPROC);
    vAprocWaitCmdDone ();
}

//-----------------------------------------------------------------------------
/** vAprocPostProc_Close
 *  Audio processor close post-processing
 *
 *  @param           
 *  @retval 
 */
//-----------------------------------------------------------------------------
void vAprocPostProc_Close (void)
{
    //LOG (0, "====> vAprocPostProc_Close--1, _gu4AprocEnable = %d, _gu4AprocPostProcControl = %d \n", _gu4AprocEnable, _gu4AprocPostProcControl);
    if (!_gu4AprocEnable)
    {
        return;
    }

    if (_gu4AprocPostProcControl == APROC_POSTPROC_CLOSE)
    {
        return;
    }

    //LOG (0, "====> vAprocPostProc_Close--2\n");
    vAprocCmdClose (APROC_SIG_IDX_POSTPROC);
    vAprocWaitCmdDone ();
    _gu4AprocPostProcControl = 0;
}
#endif
//-----------------------------------------------------------------------------
/** vAproc_Open
 *  Audio processor open 
 *
 *  @param           
 *  @retval 
 */
//-----------------------------------------------------------------------------
void vAproc_Open (void)
{
    if (!_gu4AprocEnable)
    {
        return;
    }

    if (_gu4AprocControl == APROC_DEV_OPEN)
    {
        LOG (5, "vAproc_Open: already opened!\n");
        return;
    }
    
    LOG (5, "====> vAproc_Open\n");
    vAprocCmdOpen (APROC_SIG_IDX_APROC);
    vAprocWaitCmdDone ();

    _gu4AprocControl = APROC_DEV_OPEN;
}

//-----------------------------------------------------------------------------
/** vAproc_Close
 *  Audio processor close 
 *
 *  @param           
 *  @retval 
 */
//-----------------------------------------------------------------------------
void vAproc_Close (void)
{
    if (!_gu4AprocEnable)
    {
        return;
    }

    if (_gu4AprocControl == APROC_DEV_CLOSE)
    {
        LOG (5, "vAproc_Close: already closed!\n");
        return;
    }

    LOG (5, "====> vAproc_Close\n");
    vAprocCmdClose (APROC_SIG_IDX_APROC);
    vAprocWaitCmdDone ();
    
    _gu4AprocControl = APROC_DEV_CLOSE; 
}

//-----------------------------------------------------------------------------
/** vAprocPostProc_Open
 *  Audio processor open decoders
 *
 *  @param           
 *  @retval 
 */
//-----------------------------------------------------------------------------
void vAprocDec_Open (UINT32 u4DecId)
{
    UINT32 u4Idx;

    if (!_gu4AprocEnable)
    {
        return;
    }

    if (_gu4AprocDecControl[u4DecId] == APROC_DEV_OPEN)
    {
        return;
    }

    LOG (5, "====> vAprocDecc_Open %d\n", u4DecId);

    // init IO
    switch (u4DecId)
    {
    case APROC_DEV_DEC0:
        u4Idx = APROC_SIG_IDX_DEC0;    
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_IOBUF_0, APROC_REG_IOBUF_DEC0_START), 
            (u4GetAprocMemoryMap(APROC_MEMMAP_ADSP_CMPT) + APROC_ADSP_IOBUF_DEC0_START + APOLLO_EXT_MEM_SADR));
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_IOBUF_0, APROC_REG_IOBUF_DEC0_END), APROC_ADSP_IOBUF_DEC0_END);    
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_IOBUF_0, APROC_REG_IOBUF_DEC0_SIZE), APROC_ADSP_IOBUF_DEC0_SIZE);   
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_IOBUF_0, APROC_REG_IOBUF_DEC0_CH), APROC_ADSP_IOBUF_DEC0_CH);   
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_IOBUF_0, APROC_REG_IOBUF_DEC0_SI_BASE), 
            (u4GetAprocMemoryMap(APROC_MEMMAP_ADSP_DRAM) + APROC_ADSP_IOBUF_DEC0_SI_BASE + APOLLO_EXT_MEM_SADR));
        break;    
    case APROC_DEV_DEC1:
        u4Idx = APROC_SIG_IDX_DEC1;       
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_IOBUF_0, APROC_REG_IOBUF_DEC1_START), 
            (u4GetAprocMemoryMap(APROC_MEMMAP_ADSP_CMPT) + APROC_ADSP_IOBUF_DEC1_START + APOLLO_EXT_MEM_SADR));
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_IOBUF_0, APROC_REG_IOBUF_DEC1_END), APROC_ADSP_IOBUF_DEC1_END);    
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_IOBUF_0, APROC_REG_IOBUF_DEC1_SIZE), APROC_ADSP_IOBUF_DEC1_SIZE);   
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_IOBUF_0, APROC_REG_IOBUF_DEC1_CH), APROC_ADSP_IOBUF_DEC1_CH);      
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_IOBUF_0, APROC_REG_IOBUF_DEC1_SI_BASE), 
            (u4GetAprocMemoryMap(APROC_MEMMAP_ADSP_DRAM) + APROC_ADSP_IOBUF_DEC1_SI_BASE + APOLLO_EXT_MEM_SADR));
        break;
    case APROC_DEV_DEC2: 
        u4Idx = APROC_SIG_IDX_DEC2;
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_IOBUF_0, APROC_REG_IOBUF_DEC2_START), 
            (u4GetAprocMemoryMap(APROC_MEMMAP_ADSP_CMPT) + APROC_ADSP_IOBUF_DEC2_START + APOLLO_EXT_MEM_SADR));
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_IOBUF_0, APROC_REG_IOBUF_DEC2_END), APROC_ADSP_IOBUF_DEC2_END);    
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_IOBUF_0, APROC_REG_IOBUF_DEC2_SIZE), APROC_ADSP_IOBUF_DEC2_SIZE);   
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_IOBUF_0, APROC_REG_IOBUF_DEC2_CH), APROC_ADSP_IOBUF_DEC2_CH);      
        vAprocReg_Write (APROC_ASM_ADDR(APROC_ASM_ID_IOBUF_0, APROC_REG_IOBUF_DEC2_SI_BASE), 
            (u4GetAprocMemoryMap(APROC_MEMMAP_ADSP_DRAM) + APROC_ADSP_IOBUF_DEC2_SI_BASE + APOLLO_EXT_MEM_SADR));
        break;        
        
    default:
        LOG (5, "vAprocDecc_Open: wrong decoder ID = %d!\n", u4DecId);
        return;
    }
    
    vAprocCmdOpen (u4Idx);
    vAprocWaitCmdDone ();

    _gu4AprocDecControl[u4DecId] = APROC_DEV_OPEN;
}

//-----------------------------------------------------------------------------
/** vAprocDec_Close
 *  Audio processor close decoders
 *
 *  @param           
 *  @retval 
 */
//-----------------------------------------------------------------------------
void vAprocDec_Close (UINT32 u4DecId)
{
    UINT32 u4Idx;

    if (!_gu4AprocEnable)
    {
        return;
    }

    if (_gu4AprocDecControl[u4DecId] == APROC_DEV_CLOSE)
    {
        return;
    }

    LOG (5, "====> vAprocDec_Close %d\n", u4DecId);
    
    switch (u4DecId)
    {
    case APROC_DEV_DEC0:
        u4Idx = APROC_SIG_IDX_DEC0;
        break;
    case APROC_DEV_DEC1:
        u4Idx = APROC_SIG_IDX_DEC1;
        break;
    case APROC_DEV_DEC2:
        u4Idx = APROC_SIG_IDX_DEC2;
        break;
    default:
        LOG (5, "vAprocDec_Close: wrong decoder ID = %d!\n", u4DecId);
        return;  
    }
    
    vAprocCmdClose (u4Idx);
    vAprocWaitCmdDone ();

    _gu4AprocDecControl[u4DecId] = APROC_DEV_CLOSE;
}

// if ADSP is not stopped, use this to reopen APROC
void vAprocMain_Open (void)
{
    vAprocMain_Init();

//modify by ling
//#ifdef CC_AUD_ARM_SUPPORT // fix me!!!!
#if 1
    // open aproc output
    // aproc output registers are switched by adsp kernel code
    // aproc output must be opened after adsp kernel ready
    if (bIsSupportAproc () == TRUE)
    {
        while (1)
        {
            // wait aproc power-on
            if (u4Aproc_IsEnable())
            {
                break;
            }
            x_thread_delay (1);
        }
    }
    vAproc_Open ();
#endif            
}

// if ADSP is not stopped, use this to close APROC
void vAprocMain_Close (void)
{
    _gu4AprocControl = APROC_DEV_CLOSE;
    vAproc_Disable();
#if 0
    VERIFY(x_sema_delete(_ahSemaAprocComDone) == OSR_OK);
    VERIFY(x_sema_delete(_hAprocAccessSema) == OSR_OK);
    VERIFY(x_sema_delete(_hAprocSema) == OSR_OK);      
#endif
}


#endif


