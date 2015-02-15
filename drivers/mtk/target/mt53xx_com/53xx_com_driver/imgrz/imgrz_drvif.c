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
 * $RCSfile: imgrz_drvif.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
 
/** @file imgrz_drvif.c
 *  internal public interfaces of image resizer driver
 *
 */

/*
    lint inhibition(s)
        506 - Constant value Boolean [MTK Rule 6.1.3]
        534 - Ignoring return value of function
        774 - Boolean within 'right side of && within if' always
              evaluates to True [Reference: file imgrz_if.c: lines 919, 935]
        826 - Suspicious pointer-to-pointer conversion (area too small)
        831 - Loss of sign (assignment) (signed char to unsigned long)
*/


//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#include "imgrz_if.h"
#include "imgrz_drvif.h"
#include "imgrz_dif.h"
#include "imgrz_hw.h"
#include "imgrz_cmdque.h"
#include "imgrz_debug.h"
#include "imgrz_coeff.h"


//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

#define D_RZ_TIME_OUT           (5 * 1000)     // 30 secs
#define D_RZ_DEF_TMP_BUF_LEN    256             // default temp buffer length
#define D_RZ_A_BLEND_DEF_LVL    0x8             // default a_blend level
#define D_RZ_DEF_ALPHA          0xff            // default alpha
#define D_RZ_YCBCR_EXIST        0x7
#define D_RZ_JPG_TMP_BUF_SIZE   (1920 * 3)

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------


// acronym of register name in register file
#define RZREG(XREG)         _prRzRegFile->rField.XREG
#define RZWD(XREG)         _prRzRegFile->rReg.XREG
#define RZADDR(XADDR)      ((UINT32 *)&(_prRzRegFile->rField))[XADDR]

// push back relative register value in register file.
#if defined(IMGRZ_RISC_MODE)  // risc mode
#define IMGRZ_CMDENQ(XREG)        \
    (IMGRZ_RiscPushBack((UINT32)(XREG), _au4RzRegFile[(INT32)(XREG)]))
#else                       // cmdque mode
#define IMGRZ_CMDENQ(XREG)        \
    (IMGRZ_CmdQuePushBack((UINT32)(XREG), _au4RzRegFile[(INT32)(XREG)]))
#endif // #if defined(IMGRZ_RISC_MODE)


//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Static function forward declarations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------

// engine is in flush (used in middleware).
static BOOL _fgImgrzInFlush = FALSE;

//static BOOL _fgImgrzInited = FALSE;

static UINT32 _u4JpgTmpBufAddr = 0;

static UINT32 _u4JpgPreScaleCnt = 0;

static UINT32 _au4RzRegFile[IMGRZ_REG_FILE_SIZE];

static E_RZ_INOUT_TYPE_T _eRzCurInOutType=E_RZ_INOUT_TYPE_VDOMD;

static HANDLE_T _semImgrzOp = NULL_HANDLE;
static HANDLE_T _semImgrzCmdque = NULL_HANDLE;
static HANDLE_T _hImgrzCrashDetectTimer = NULL_HANDLE;

#define IMGRZ_WAITISR_MSGQ

#ifdef IMGRZ_WAITISR_MSGQ
static HANDLE_T _hWaitIsrMsgQ = NULL_HANDLE;
static HANDLE_T _hWaitIsrTimer = NULL_HANDLE;
#else
static HANDLE_T _semImgrzWaitIsr = NULL_HANDLE;
#endif

/*lint -save -e826 */
static E_RZ_DIF_UNION_T *_prRzRegFile = (E_RZ_DIF_UNION_T *)_au4RzRegFile;
/*lint -restore */

#if defined(IMGRZ_DEBUG_MODE)
    static UINT32 _u4ImgrzCrashDetection = IMGRZ_DISABLE; // IMGRZ_ENABLE when IC back
#else
    static UINT32 _u4ImgrzCrashDetection = IMGRZ_DISABLE;
#endif

//UINT32 ImgrzhwISR;
//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------


//-------------------------------------------------------------------------
/** _IMGRZ_IsFlushing
 *  check whether engine is flushing
 *  
 */
//-------------------------------------------------------------------------
BOOL _IMGRZ_IsFlushing(void)
{
    return _fgImgrzInFlush;
}


//-------------------------------------------------------------------------
/** _IMGRZ_SetFlushStatus
 *  set engine flush status
 *  
 */
//-------------------------------------------------------------------------
void _IMGRZ_SetFlushStatus(BOOL fgStatus)
{
    _fgImgrzInFlush = fgStatus;
}


//-------------------------------------------------------------------------
/** _IMGRZ_SetCrashDetection
 *  enable/disable crash detection
 *  
 */
//-------------------------------------------------------------------------
void _IMGRZ_SetCrashDetection(UINT32 u4Value)
{
    _u4ImgrzCrashDetection = u4Value;
}


//-------------------------------------------------------------------------
/** _IMGRZ_GetCrashDetection
 *  get crash detection
 *  
 */
//-------------------------------------------------------------------------
UINT32 _IMGRZ_GetCrashDetection()
{
    return _u4ImgrzCrashDetection;
}


//-------------------------------------------------------------------------
/** _IMGRZ_GetCrashDetectTimer
 *  get crash detect timer
 *  
 */
//-------------------------------------------------------------------------
HANDLE_T _IMGRZ_GetCrashDetectTimer(void)
{
    return _hImgrzCrashDetectTimer;
}


//-------------------------------------------------------------------------
/** _IMGRZ_GetScaleLine
 *  get current scale vertical line
 *  
 */
//-------------------------------------------------------------------------
UINT32 _IMGRZ_GetScaleLine(void)
{
    UINT32 u4ScaleLine, u4CurrentTotalLine;
    u4CurrentTotalLine = _IMGRZ_HwGetCurrentLine();
    u4ScaleLine = u4CurrentTotalLine - _u4JpgPreScaleCnt;
    _u4JpgPreScaleCnt = u4CurrentTotalLine;
    
    return u4ScaleLine;
}


//-------------------------------------------------------------------------
/** _IMGRZ_ReInit
 *  re-init
 *
 */
//-------------------------------------------------------------------------
void _IMGRZ_ReInit(void)
{
    //_fgImgrzInited = FALSE;
    _IMGRZ_Init();
}

//-------------------------------------------------------------------------
/** _IMGRZ_Init
 *  create semaphore
 *  reset engine & driver
 *  clear soft reg file
 *  init dif interface
 *  init cmdque
 *  set internal SRAM
 */
//-------------------------------------------------------------------------
void _IMGRZ_Init(void)
{
    /*if (_fgImgrzInited)
    {
        return;
    }*/

    if (_semImgrzOp == NULL_HANDLE)
    {
        VERIFY(x_sema_create(&_semImgrzOp, X_SEMA_TYPE_MUTEX, 
            X_SEMA_STATE_UNLOCK) == OSR_OK);
    }

    if (_semImgrzCmdque == NULL_HANDLE)
    {
        VERIFY(x_sema_create(&_semImgrzCmdque, X_SEMA_TYPE_BINARY, 
            X_SEMA_STATE_UNLOCK) == OSR_OK);
    }

#ifdef IMGRZ_WAITISR_MSGQ
     if(_hWaitIsrMsgQ == NULL_HANDLE)
     {
         VERIFY(x_msg_q_create(&_hWaitIsrMsgQ, "WaitIsr-Q", sizeof(UINT32), 1) == OSR_OK);        
         IMGRZ_UnlockWaitIsr();
     }
     if(_hWaitIsrTimer == NULL_HANDLE)
     {
         VERIFY(x_timer_create(&_hWaitIsrTimer) == OSR_OK);
     }
#else
    if (_semImgrzWaitIsr == NULL_HANDLE)
    {
        VERIFY(x_sema_create(&_semImgrzWaitIsr, X_SEMA_TYPE_BINARY, 
            X_SEMA_STATE_UNLOCK) == OSR_OK);
    }
#endif


    
    if (_hImgrzCrashDetectTimer == NULL_HANDLE)
    {
        VERIFY(x_timer_create(&_hImgrzCrashDetectTimer) == OSR_OK);
    }

    IMGRZ_Reset();

    IMGRZ_UNUSED_RET(x_memset(_au4RzRegFile, 0, sizeof(_au4RzRegFile)))

    // init dif
    IMGRZ_DifInit();

   
#if defined(IMGRZ_DEBUG_MODE)

    // init debug info
    IMGRZ_DbgInit();

#endif // #if defined(IMGRZ_DEBUG_MODE)


    // init cmdque
    IMGRZ_CmdQueInit();

    IMGRZ_DBG_REC_CUR_STATUS((INT32)E_IMGRZ_IDLE);

    //_fgImgrzInited = TRUE;
    _u4JpgPreScaleCnt = 0;
}


//-------------------------------------------------------------------------
/** _IMGRZ_Reset
 *  reset engine & driver
 */
//-------------------------------------------------------------------------
void _IMGRZ_Reset(void)
{
    IMGRZ_DifReset();
}

//-------------------------------------------------------------------------
/** _IMGRZ_Wait
 *  wait engine work complete
 */
//-------------------------------------------------------------------------
void _IMGRZ_Wait(void)
{
#ifdef IMGRZ_POLLING
    UINT32 u4Cnt = 0;

    HalFlushInvalidateDCache();

    IMGRZ_UnlockCmdque();               //polling mode needn't use semalock

    while (u4Cnt != 0xffffffff)    
    {
        if (IMGRZ_HwPolling())
        {
            break;
        }
        u4Cnt++;
    }
    if (u4Cnt == 0xffffffff)
    {
        LOG(0, "IMGRZ engine works too long\n");
    }
    
    IMGRZ_DBG_INC_DRV_INT_COUNT();      //fake interrupt
#else

  //LOG(0, "1 IMGRZ engine new..ImgrzhwISR = [%x]\n", ImgrzhwISR);
  //  IMGRZ_LockCmdque();
   IMGRZ_LockWaitIsr();

    //LOG(0, "2 IMGRZ engine new..ImgrzhwISR = [%x]\n", ImgrzhwISR);

    HalFlushInvalidateDCache();

    //LOG(0, "_IMGRZ_Wait unlock\n");

   //IMGRZ_UnlockCmdque();
    IMGRZ_UnlockWaitIsr();

#endif  
}


//-------------------------------------------------------------------------
/** _IMGRZ_MwFlush
 *  force engine to do operations (for middleware use)
 *  set in flush flag
 *  call cmdque action
 */
//-------------------------------------------------------------------------
void _IMGRZ_MwFlush(void)
{
    IMGRZ_DBG_INC_MW_FLUSH_COUNT(); // for debug use

    _IMGRZ_SetFlushStatus(TRUE);    // for middleware use


#if defined(IMGRZ_RISC_MODE)    // risc mode
    IMGRZ_HwISR();
#else                           // cmdque mode
    IMGRZ_CmdQueAction();
#endif
}


//-------------------------------------------------------------------------
/** _IMGRZ_Flush
 *  force engine to do operations (for driver use)
 *  set in flush flag
 *  call cmdque action
 */
//-------------------------------------------------------------------------
void _IMGRZ_Flush(void)
{
#if !defined(IMGRZ_RISC_MODE) // cmdque mode
    IMGRZ_CmdQueAction();
#endif
}


//-------------------------------------------------------------------------
/** _IMGRZ_Lock
 *  take the semaphore
 */
//-------------------------------------------------------------------------
void _IMGRZ_Lock(void)
{
    IMGRZ_UNUSED_RET(x_sema_lock(_semImgrzOp, X_SEMA_OPTION_WAIT))
}


//-------------------------------------------------------------------------
/** _IMGRZ_TryLock
 *  try to take the semaphore
 */
//-------------------------------------------------------------------------
void _IMGRZ_TryLock(void)
{
    IMGRZ_UNUSED_RET(x_sema_lock(_semImgrzOp, X_SEMA_OPTION_NOWAIT))
}


//-------------------------------------------------------------------------
/** _IMGRZ_Unlock
 *  release the semaphore
 */
//-------------------------------------------------------------------------
void _IMGRZ_Unlock(void)
{
    IMGRZ_UNUSED_RET(x_sema_unlock(_semImgrzOp))
}


//-------------------------------------------------------------------------
/** _IMGRZ_LockCmdque
 *  use lock_timeout to do engine crash detection
 *  crash cause 1 : engine eats invalid cmd
 */
//-------------------------------------------------------------------------
#if 1
void _IMGRZ_LockCmdque(void)
{
    //UINT32 u4Cnt, u4Bit8, u4Cnt2;
    //UINT32 u4IMGRZ_Reg[4], u4OtherAgent[4], u4OtherBase;
    if (OSR_TIMEOUT == 
        x_sema_lock_timeout(_semImgrzCmdque, D_RZ_TIME_OUT))
    {
        LOG(0, "IMGRZ engine works too long (10 secs)......\n");
#if 0
        u4IMGRZ_Reg[0] = IO_READ32(0x20020000, 0x1c4);
        u4IMGRZ_Reg[1] = IO_READ32(0x20020000, 0x1c8);
        u4IMGRZ_Reg[2] = IO_READ32(0x20020000, 0x1cc);
        u4IMGRZ_Reg[3] = IO_READ32(0x20020000, 0x1d0);
        for (u4Cnt = 0; u4Cnt < 34; u4Cnt++)
        {
            u4OtherBase = (u4Cnt << 12) | 0x20000000;
            for (u4Bit8 = 1; u4Bit8 <= 9; u4Bit8 += 2)
            {
                u4OtherAgent[0] = u4OtherBase | (u4Bit8 << 8) | 0xc4;
                u4OtherAgent[1] = u4OtherBase | (u4Bit8 << 8) | 0xc8;
                u4OtherAgent[2] = u4OtherBase | (u4Bit8 << 8) | 0xcc;
                u4OtherAgent[3] = u4OtherBase | (u4Bit8 << 8) | 0xd0;
                for (u4Cnt2 = 0; u4Cnt2 < 4; u4Cnt2++)
                {
                    if (u4IMGRZ_Reg[u4Cnt2] == IO_READ32(u4OtherAgent[u4Cnt2], 0x0))
                    {
                        LOG(0, "0x%x", u4OtherAgent[u4Cnt2]);
                    }
                }
            }        
        }        
#endif
        if (!IMGRZ_HwGetIdle())
        {
            LOG(0, "IMGRZ engine still busy ......\n");

            // recover hw engine
            //IMGRZ_DBG_RECOVER_ENGINE();
        }
    }
}
#else
void _IMGRZ_LockCmdque(void)
{
    IMGRZ_UNUSED_RET(x_sema_lock(_semImgrzCmdque, X_SEMA_OPTION_WAIT))
}
#endif


//-------------------------------------------------------------------------
/** _IMGRZ_UnlockCmdque
 *  release the semaphore
 */
//-------------------------------------------------------------------------
void _IMGRZ_UnlockCmdque(void)
{
    IMGRZ_UNUSED_RET(x_sema_unlock(_semImgrzCmdque))
}

#ifdef IMGRZ_WAITISR_MSGQ

void _IMGRZ_TimerTimeout(HANDLE_T  pt_tm_handle, VOID *pv_tag)
{
    UINT32 u4RzFin;
    INT32 i4Ret;    
       
    u4RzFin = 2;
    i4Ret = x_msg_q_send(_hWaitIsrMsgQ, (void *)&u4RzFin, sizeof(UINT32), 1);
  
    if ((i4Ret != OSR_OK)&&(i4Ret != OSR_TOO_MANY))
    {
        LOG(0, "IMGRZ engine _IMGRZ_TimerTimeout  fail ......\n");
        ASSERT(0);
    }
    
    UNUSED(pt_tm_handle);
    UNUSED(pv_tag);

}
#endif

//-------------------------------------------------------------------------
/** _IMGRZ_LockWaitIsr
 *  use lock_timeout to do engine crash detection
 *  crash cause 1 : engine eats invalid cmd
 */
//-------------------------------------------------------------------------
void _IMGRZ_LockWaitIsr(void)
{
#ifdef IMGRZ_WAITISR_MSGQ

    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    UINT32 u4WaitTime;
    UINT32 u4RzFin;
    u4WaitTime = 500;
    zMsgSize = sizeof(UINT32);
 
    VERIFY(x_timer_start(_hWaitIsrTimer, u4WaitTime, X_TIMER_FLAG_ONCE, 
            _IMGRZ_TimerTimeout, NULL)== OSR_OK);        
    VERIFY(x_msg_q_receive(&u2MsgQIdx, &u4RzFin, &zMsgSize, &_hWaitIsrMsgQ,1,
        X_MSGQ_OPTION_WAIT) == OSR_OK);
    VERIFY(x_timer_stop(_hWaitIsrTimer) == OSR_OK);    

    if(u4RzFin != 1)
    {
           LOG(0, "IMGRZ engine IMGRZ_WAITISR_MSGQ works too long ......\n");
    }
#else
        if (OSR_TIMEOUT == 
        x_sema_lock_timeout(_semImgrzWaitIsr, D_RZ_TIME_OUT))
    {
        LOG(0, "IMGRZ engine WaitIsr works too long (11 secs)......\n");
        if (!IMGRZ_HwGetIdle())
        {
            LOG(0, "IMGRZ engine still busy ......\n");
            
            // recover hw engine
            //IMGRZ_DBG_RECOVER_ENGINE();
        }
    }
#endif


}


//-------------------------------------------------------------------------
/** _IMGRZ_UnlockWaitIsr
 *  release the semaphore
 */
//-------------------------------------------------------------------------
void _IMGRZ_UnlockWaitIsr(void)
{
#ifdef IMGRZ_WAITISR_MSGQ    
    UINT32 u4RzFin;
    INT32 i4Ret;
    u4RzFin = 1;
    i4Ret = x_msg_q_send(_hWaitIsrMsgQ, (void *)&u4RzFin, sizeof(UINT32), 1);

    if ((i4Ret != OSR_OK)&&(i4Ret != OSR_TOO_MANY))
    {
        LOG(0, "IMGRZ engine IMGRZ_WAITISR_MSGQ is full ......\n");
        ASSERT(0);
    }
#else
    IMGRZ_UNUSED_RET(x_sema_unlock(_semImgrzWaitIsr))
#endif
}

//-------------------------------------------------------------------------
/** _IMGRZ_QueryHwIdle
 *  query engine status
 */
//-------------------------------------------------------------------------
INT32 _IMGRZ_QueryHwIdle(void)
{
    return IMGRZ_HwGetIdle();
}


//-------------------------------------------------------------------------
/** _IMGRZ_SetNotify
 *  set notify function.
 *  set dif notification.
 */
//-------------------------------------------------------------------------
void _IMGRZ_SetNotify(void (*pfnNotifyFunc)(UINT32))
{
    IMGRZ_DifSetNotify(pfnNotifyFunc);
}


//-------------------------------------------------------------------------
/** _IMGRZ_SetRasterScanInOut
 *  raster scan input flag      (raster/block scan)
 *  raster scan output flag     (raster/block scan)
 */
//-------------------------------------------------------------------------
void _IMGRZ_SetRasterScanInOut(BOOL fgRasterScanIn, 
    BOOL fgRasterScanOut)
{
    // fill field & cmdque
    RZREG(fg_RSIN)  = fgRasterScanIn;
    RZREG(fg_RSOUT) = fgRasterScanOut;
    IMGRZ_CMDENQ(RZ_REG_RZTYPE);
}


//-------------------------------------------------------------------------
/** _IMGRZ_SetInternalSRAM
 *  16 bpp pixel precision flag     (16/8 bpp)
 *  internal sram length            (provided by designer)
 */
//-------------------------------------------------------------------------
void _IMGRZ_SetInternalSRAM(UINT32 u4Is16Bpp, UINT32 u4LinBufLen)
{
    // fill field & cmdque
    RZREG(fg_LBUFL) = u4LinBufLen;
    RZREG(fg_EXT16) = u4Is16Bpp;
    IMGRZ_CMDENQ(RZ_REG_RZTGBUFL);
}

//-------------------------------------------------------------------------
/** _IMGRZ_SetH8TapScaleFacOft
 *  set H8Tap scaling factor & offset (y, cb, cr)
 */
//-------------------------------------------------------------------------
void _IMGRZ_SetH8TapScaleFacOft(const RZ_SCL_FAC_OFT_T *psrFacOft)
{
    IMGRZ_ASSERT(psrFacOft != NULL);
    
    // fill field
    RZREG(fg_H8TAP_FAC_Y)  = psrFacOft->u4YFactor;
    RZREG(fg_H8TAP_OFT_Y)  = psrFacOft->u4YOffset;
    RZREG(fg_H8TAP_FAC_CB) = psrFacOft->u4CbFactor;
    RZREG(fg_H8TAP_OFT_CB) = psrFacOft->u4CbOffset;
    RZREG(fg_H8TAP_FAC_CR) = psrFacOft->u4CrFactor;
    RZREG(fg_H8TAP_OFT_CR) = psrFacOft->u4CrOffset;

#ifdef CC_MT5363
    IMGRZ_CMDENQ(RZ_REG_RZH8TAPFACY);
    IMGRZ_CMDENQ(RZ_REG_RZH8TAPFACCB);
    IMGRZ_CMDENQ(RZ_REG_RZH8TAPFACCR);
    IMGRZ_CMDENQ(RZ_REG_YH8TAPO);
    IMGRZ_CMDENQ(RZ_REG_CBH8TAPO);
    IMGRZ_CMDENQ(RZ_REG_CRH8TAPO);    
#else
    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZH8TAPY);
    IMGRZ_CMDENQ(RZ_REG_RZH8TAPCB);
    IMGRZ_CMDENQ(RZ_REG_RZH8TAPCR);
#endif    
}


//-------------------------------------------------------------------------
/** _IMGRZ_SetHsaScaleFacOft
 *  set Hsa scaling factor & offset (y, cb, cr)
 */
//-------------------------------------------------------------------------
void _IMGRZ_SetHsaScaleFacOft(const RZ_SCL_FAC_OFT_T *psrFacOft)
{
    IMGRZ_ASSERT(psrFacOft != NULL);

    // fill field
    RZREG(fg_HSA_FAC_Y)  = psrFacOft->u4YFactor;
    RZREG(fg_HSA_OFT_Y)  = psrFacOft->u4YOffset;
    RZREG(fg_HSA_FAC_CB) = psrFacOft->u4CbFactor;
    RZREG(fg_HSA_OFT_CB) = psrFacOft->u4CbOffset;
    RZREG(fg_HSA_FAC_CR) = psrFacOft->u4CrFactor;
    RZREG(fg_HSA_OFT_CR) = psrFacOft->u4CrOffset;

    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZHSAY);
    IMGRZ_CMDENQ(RZ_REG_RZHSACB);
    IMGRZ_CMDENQ(RZ_REG_RZHSACR);
}


//-------------------------------------------------------------------------
/** _IMGRZ_SetH8TapFilterCoeff
 *  set H8Tap filter coefficients
 */
//-------------------------------------------------------------------------
void _IMGRZ_SetH8TapFilterCoeff(const UINT32 *psu4CoeffTbl)
{
    IMGRZ_ASSERT(psu4CoeffTbl != NULL);
    
    // fill field
    RZREG(fg_H8TAP_COEFF0)  = psu4CoeffTbl[0];
    RZREG(fg_H8TAP_COEFF1)  = psu4CoeffTbl[1];
    RZREG(fg_H8TAP_COEFF2)  = psu4CoeffTbl[2];
    RZREG(fg_H8TAP_COEFF3)  = psu4CoeffTbl[3];
    RZREG(fg_H8TAP_COEFF4)  = psu4CoeffTbl[4];
    RZREG(fg_H8TAP_COEFF5)  = psu4CoeffTbl[5];
    RZREG(fg_H8TAP_COEFF6)  = psu4CoeffTbl[6];
    RZREG(fg_H8TAP_COEFF7)  = psu4CoeffTbl[7];
    RZREG(fg_H8TAP_COEFF8)  = psu4CoeffTbl[8];
    RZREG(fg_H8TAP_COEFF9)  = psu4CoeffTbl[9];
    RZREG(fg_H8TAP_COEFF10) = psu4CoeffTbl[10];
    RZREG(fg_H8TAP_COEFF11) = psu4CoeffTbl[11];
    RZREG(fg_H8TAP_COEFF12) = psu4CoeffTbl[12];
    RZREG(fg_H8TAP_COEFF13) = psu4CoeffTbl[13];
    RZREG(fg_H8TAP_COEFF14) = psu4CoeffTbl[14];
    RZREG(fg_H8TAP_COEFF15) = psu4CoeffTbl[15];
    RZREG(fg_H8TAP_COEFF16) = psu4CoeffTbl[16];
    RZREG(fg_H8TAP_COEFF17) = psu4CoeffTbl[17];
    
    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZHCOEFF0);
    IMGRZ_CMDENQ(RZ_REG_RZHCOEFF1);
    IMGRZ_CMDENQ(RZ_REG_RZHCOEFF2);
    IMGRZ_CMDENQ(RZ_REG_RZHCOEFF3);
    IMGRZ_CMDENQ(RZ_REG_RZHCOEFF4);
    IMGRZ_CMDENQ(RZ_REG_RZHCOEFF5);
    IMGRZ_CMDENQ(RZ_REG_RZHCOEFF6);
    IMGRZ_CMDENQ(RZ_REG_RZHCOEFF7);
    IMGRZ_CMDENQ(RZ_REG_RZHCOEFF8);
    IMGRZ_CMDENQ(RZ_REG_RZHCOEFF9);
    IMGRZ_CMDENQ(RZ_REG_RZHCOEFF10);
    IMGRZ_CMDENQ(RZ_REG_RZHCOEFF11);
    IMGRZ_CMDENQ(RZ_REG_RZHCOEFF12);
    IMGRZ_CMDENQ(RZ_REG_RZHCOEFF13);
    IMGRZ_CMDENQ(RZ_REG_RZHCOEFF14);
    IMGRZ_CMDENQ(RZ_REG_RZHCOEFF15);
    IMGRZ_CMDENQ(RZ_REG_RZHCOEFF16);
    IMGRZ_CMDENQ(RZ_REG_RZHCOEFF17);
}


//-------------------------------------------------------------------------
/** _IMGRZ_SetV4TapScaleFacOft
 *  set V4Tap scaling factor & offset (y, cb, cr)
 */
//-------------------------------------------------------------------------
void _IMGRZ_SetV4TapScaleFacOft(const RZ_SCL_FAC_OFT_T *psrFacOft)
{
    IMGRZ_ASSERT(psrFacOft != NULL);
    
    // fill field
    RZREG(fg_V4TAP_FAC_Y)  = psrFacOft->u4YFactor;
    RZREG(fg_V4TAP_OFT_Y)  = psrFacOft->u4YOffset;
#ifdef CC_MT5363
    RZREG(fg_V4TAP_FAC_C) = psrFacOft->u4CbFactor;
    RZREG(fg_V4TAP_OFT_C) = psrFacOft->u4CbOffset;
    if(_eRzCurInOutType==E_RZ_INOUT_TYPE_VDORMRPR)
    {
	  IMGRZ_CMDENQ(RZ_REG_YV4TAPO);
	  IMGRZ_CMDENQ(RZ_REG_CV4TAPO);
    }
    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZYV4TAPFAC);
    IMGRZ_CMDENQ(RZ_REG_RZCV4TAPFAC);
#else
    RZREG(fg_V4TAP_FAC_CB) = psrFacOft->u4CbFactor;
    RZREG(fg_V4TAP_OFT_CB) = psrFacOft->u4CbOffset;
    RZREG(fg_V4TAP_FAC_CR) = psrFacOft->u4CrFactor;
    RZREG(fg_V4TAP_OFT_CR) = psrFacOft->u4CrOffset;

    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZV4TAPY);
    IMGRZ_CMDENQ(RZ_REG_RZV4TAPCB);
    IMGRZ_CMDENQ(RZ_REG_RZV4TAPCR);
#endif    
}


//-------------------------------------------------------------------------
/** _IMGRZ_SetVSclScaleFacOft
 *  set VScl scaling factor & offset (y, cb, cr)
 */
//-------------------------------------------------------------------------
void _IMGRZ_SetVSclScaleFacOft(const RZ_SCL_FAC_OFT_T *psrFacOft)
{
    IMGRZ_ASSERT(psrFacOft != NULL);

    // fill field
    RZREG(fg_V_FACTORY)  = psrFacOft->u4YFactor;
    RZREG(fg_VUP_Y)      = psrFacOft->u4VupY;
    RZREG(fg_V_SCLOFTY)  = psrFacOft->u4YOffset;
    RZREG(fg_V_FACTORCB) = psrFacOft->u4CbFactor;
    RZREG(fg_VUP_CB)     = psrFacOft->u4VupCb;
    RZREG(fg_V_SCLOFTCB) = psrFacOft->u4CbOffset;
    RZREG(fg_V_FACTORCR) = psrFacOft->u4CrFactor;
    RZREG(fg_VUP_CR)     = psrFacOft->u4VupCr;
    RZREG(fg_V_SCLOFTCR) = psrFacOft->u4CrOffset;
#ifdef CC_MT5363
    if(_eRzCurInOutType==E_RZ_INOUT_TYPE_VDORMRPR)
    {
      RZREG(fg_V4TAP_OFT_C) = psrFacOft->u4CbOffset;
      RZREG(fg_V4TAP_OFT_Y)  = psrFacOft->u4YOffset;
      RZREG(fg_V4TAP_FAC_Y)  = psrFacOft->u4YFactor;
      RZREG(fg_V4TAP_FAC_C) = psrFacOft->u4CbFactor;
	  IMGRZ_CMDENQ(RZ_REG_YV4TAPO);
	  IMGRZ_CMDENQ(RZ_REG_CV4TAPO);
      IMGRZ_CMDENQ(RZ_REG_RZYV4TAPFAC);
      IMGRZ_CMDENQ(RZ_REG_RZCV4TAPFAC);
    }
#endif
    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZVSCLY);
    IMGRZ_CMDENQ(RZ_REG_RZVSCLCB);
    IMGRZ_CMDENQ(RZ_REG_RZVSCLCR);
}


//-------------------------------------------------------------------------
/** _IMGRZ_SetV4TapFilterCoeff
 *  set V4Tap filter coefficients
 */
//-------------------------------------------------------------------------
void _IMGRZ_SetV4TapFilterCoeff(const UINT32 *psu4CoeffTbl)
{
    IMGRZ_ASSERT(psu4CoeffTbl != NULL);
    
    // fill field
    RZREG(fg_V4TAP_COEFF0)  = psu4CoeffTbl[0];
    RZREG(fg_V4TAP_COEFF1)  = psu4CoeffTbl[1];
    RZREG(fg_V4TAP_COEFF2)  = psu4CoeffTbl[2];
    RZREG(fg_V4TAP_COEFF3)  = psu4CoeffTbl[3];
    RZREG(fg_V4TAP_COEFF4)  = psu4CoeffTbl[4];
    RZREG(fg_V4TAP_COEFF5)  = psu4CoeffTbl[5];
    RZREG(fg_V4TAP_COEFF6)  = psu4CoeffTbl[6];
    RZREG(fg_V4TAP_COEFF7)  = psu4CoeffTbl[7];
    RZREG(fg_V4TAP_COEFF8)  = psu4CoeffTbl[8];
    
    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZVCOEFF0);
    IMGRZ_CMDENQ(RZ_REG_RZVCOEFF1);
    IMGRZ_CMDENQ(RZ_REG_RZVCOEFF2);
    IMGRZ_CMDENQ(RZ_REG_RZVCOEFF3);
    IMGRZ_CMDENQ(RZ_REG_RZVCOEFF4);
    IMGRZ_CMDENQ(RZ_REG_RZVCOEFF5);
    IMGRZ_CMDENQ(RZ_REG_RZVCOEFF6);
    IMGRZ_CMDENQ(RZ_REG_RZVCOEFF7);
    IMGRZ_CMDENQ(RZ_REG_RZVCOEFF8);
}



//-------------------------------------------------------------------------
/** _IMGRZ_SetABlendOpt
 *  set alpha blending option
 */
//-------------------------------------------------------------------------
void _IMGRZ_SetABlendOpt(UINT32 u4Level, UINT32 u4Alpha)
{
    // fill field
    RZREG(fg_A_BLEND)   = u4Level;
    RZREG(fg_USR_DEF_A) = u4Alpha;
    RZREG(fg_A_SCL_TYPE) = 0;
    RZREG(fg_A_SCL_DIST_0) = 1;
    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZOSDMODE);
}


//-------------------------------------------------------------------------
/** _IMGRZ_SetScaleOpt
 *  set inout type (vdo/jpg/osd)
 */
//-------------------------------------------------------------------------
void _IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_T eInOutType)
{
    if ((eInOutType == E_RZ_INOUT_TYPE_OSDMD) || (eInOutType == E_RZ_INOUT_TYPE_OSDUI))
    {   // osd mode
        RZREG(fg_OSDMD) = IMGRZ_ENABLE;
        RZREG(fg_JPGMD) = IMGRZ_DISABLE;
    }
    else
    {   // video/jpg mode
        RZREG(fg_OSDMD) = IMGRZ_DISABLE;

        if (eInOutType == E_RZ_INOUT_TYPE_JPGMD)
        {
            RZREG(fg_JPGMD) = IMGRZ_ENABLE;
        }
        else
        {
            RZREG(fg_JPGMD) = IMGRZ_DISABLE;
        }
    }

    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZTYPE);
    _eRzCurInOutType = eInOutType;
}


//-------------------------------------------------------------------------
/** _IMGRZ_GetInOutType
 *  get inout type (vdo/jpg/osd)
 */
//-------------------------------------------------------------------------
E_RZ_INOUT_TYPE_T _IMGRZ_GetScaleOpt(void)
{
    return _eRzCurInOutType;
}


//-------------------------------------------------------------------------
/** _IMGRZ_SwitchToV4TapFilter
 *  enable V4Tap filter (V4Tap and VScl are mutual-exclusive)
 */
//-------------------------------------------------------------------------
void _IMGRZ_SwitchToV4TapFilter(void)
{
#ifdef CC_MT5363
    UINT32 u4LBufL = 0x10;
#else
    UINT32 u4LBufL = 0x8;
#endif
    // hw limitation
    _IMGRZ_SetInternalSRAM(IMGRZ_FALSE, u4LBufL);

    // fill field & cmdque
    RZREG(fg_V4TAPMD) = IMGRZ_ENABLE;
    IMGRZ_CMDENQ(RZ_REG_RZTYPE);
}

/** _IMGRZ_SaveToSameAddr
 *  enable V4Tap filter (V4Tap and VScl are mutual-exclusive)
 */
//-------------------------------------------------------------------------
#ifdef IMGRZ_AUTO_TEST
extern INT32 RzEmuGetParam(INT32 i4ParamType,INT32 i4Param);
#endif

void _IMGRZ_SaveToSameAddr(UINT32 u4AddrRST)
{
    // fill field & cmdque
#ifdef IMGRZ_AUTO_TEST
  RZREG(fg_BST_LIM) = RzEmuGetParam(2,0);
#else
  RZREG(fg_BST_LIM) = 8;
#endif
    RZREG(fg_RSTADDR) = u4AddrRST;
    IMGRZ_CMDENQ(RZ_REG_RZMIFMODE);
}

//-------------------------------------------------------------------------
/** _IMGRZ_SwitchToVSclFilter
 *  enable VScl filter (V4Tap and VScl are mutual-exclusive)
 */
//-------------------------------------------------------------------------
void _IMGRZ_SwitchToVSclFilter(UINT32 u4SrcH, UINT32 u4TgH, UINT32 u4TgW, 
                                        E_RZ_INOUT_TYPE_T eType)
{
#ifdef CC_MT5363
    UINT32 u4LBufL = 0x10;
#else
    UINT32 u4LBufL = 0x8;
#endif

    if ((eType == E_RZ_INOUT_TYPE_JPGMD) && (u4TgW >= 8))
    {
        if (u4TgH <= u4SrcH)     // scale down
        {
#if 1//defined (CC_MT5360B) || defined (CC_MT5392B)
            while (u4TgW % (u4LBufL << 5) < 8)
            {
                u4LBufL--;
            }
#else
            u4LBufL = (u4TgW % 128 < 8)? 0x3 : 0x4; 
#endif
            _IMGRZ_SetInternalSRAM(IMGRZ_ENABLE, u4LBufL);
        }
        else
        {        
#if 1//defined (CC_MT5360B) || defined (CC_MT5392B)       
            u4LBufL *= 2;
            if (u4LBufL > IMGRZ_MAX_LBUFL)
            {
                u4LBufL = IMGRZ_MAX_LBUFL;
            }
#endif
            while (u4TgW % (u4LBufL << 5) < 8)
            {
                u4LBufL--;
            }
            _IMGRZ_SetInternalSRAM(IMGRZ_DISABLE, u4LBufL);
        }
    }
    else
{
#if 0//!defined (CC_MT5360B) && !defined (CC_MT5392B)
    u4LBufL = 0x4;
#endif
    // hw limitation
    if (u4TgH <= u4SrcH)     // scale down
    {
        _IMGRZ_SetInternalSRAM(IMGRZ_ENABLE, u4LBufL);
    }
    else
    {
            u4LBufL *= 2;
            if (u4LBufL > IMGRZ_MAX_LBUFL)
            {
                u4LBufL = IMGRZ_MAX_LBUFL;
            }
            _IMGRZ_SetInternalSRAM(IMGRZ_DISABLE, u4LBufL);
    }
    }
    
    // fill field & cmdque
    RZREG(fg_V4TAPMD) = IMGRZ_DISABLE;
    IMGRZ_CMDENQ(RZ_REG_RZTYPE);
}


//-------------------------------------------------------------------------
/** _IMGRZ_BypassHsaFilter
 *  bypass Hori-Src-Accumulator
 */
//-------------------------------------------------------------------------
void _IMGRZ_BypassHsaFilter(void)
{
    // fill field
    RZREG(fg_HSA_FAC_Y)  = D_RZ_SCALE_FAC_ONE;
    RZREG(fg_HSA_OFT_Y)  = 0;
    RZREG(fg_HSA_FAC_CB) = D_RZ_SCALE_FAC_ONE;
    RZREG(fg_HSA_OFT_CB) = 0;
    RZREG(fg_HSA_FAC_CR) = D_RZ_SCALE_FAC_ONE;
    RZREG(fg_HSA_OFT_CR) = 0;

    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZHSAY);
    IMGRZ_CMDENQ(RZ_REG_RZHSACB);
    IMGRZ_CMDENQ(RZ_REG_RZHSACR);
}


//-------------------------------------------------------------------------
/** _IMGRZ_BypassH8TapFilter
 *  bypass H8Tap filter
 */
//-------------------------------------------------------------------------
void _IMGRZ_BypassH8TapFilter(void)
{
    // fill field
    RZREG(fg_H8TAP_FAC_Y)  = D_RZ_SCALE_FAC_H8V4 * D_RZ_SCALE_OLD_TRANSFER;
    RZREG(fg_H8TAP_OFT_Y)  = 0;
    RZREG(fg_H8TAP_FAC_CB) = D_RZ_SCALE_FAC_H8V4 * D_RZ_SCALE_OLD_TRANSFER;
    RZREG(fg_H8TAP_OFT_CB) = 0;
    RZREG(fg_H8TAP_FAC_CR) = D_RZ_SCALE_FAC_H8V4 * D_RZ_SCALE_OLD_TRANSFER;
    RZREG(fg_H8TAP_OFT_CR) = 0;

#ifdef CC_MT5363
    IMGRZ_CMDENQ(RZ_REG_RZH8TAPFACY);
    IMGRZ_CMDENQ(RZ_REG_RZH8TAPFACCB);
    IMGRZ_CMDENQ(RZ_REG_RZH8TAPFACCR);
    IMGRZ_CMDENQ(RZ_REG_YH8TAPO);
    IMGRZ_CMDENQ(RZ_REG_CBH8TAPO);
    IMGRZ_CMDENQ(RZ_REG_CRH8TAPO);
#else
    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZH8TAPY);
    IMGRZ_CMDENQ(RZ_REG_RZH8TAPCB);
    IMGRZ_CMDENQ(RZ_REG_RZH8TAPCR);
#endif    
}


//-------------------------------------------------------------------------
/** _IMGRZ_BypassVSclFilter
 *  bypass Vert-scaling filter
 */
//-------------------------------------------------------------------------
void _IMGRZ_BypassVSclFilter(void)
{
    // fill field
    RZREG(fg_V_FACTORY)  = 0;
    RZREG(fg_VUP_Y)      = IMGRZ_ENABLE;
    RZREG(fg_V_SCLOFTY)  = 0;
    RZREG(fg_V_FACTORCB) = 0;
    RZREG(fg_VUP_CB)     = IMGRZ_ENABLE;
    RZREG(fg_V_SCLOFTCB) = 0;
    RZREG(fg_V_FACTORCR) = 0;
    RZREG(fg_VUP_CR)     = IMGRZ_ENABLE;
    RZREG(fg_V_SCLOFTCR) = 0;

    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZVSCLY);
    IMGRZ_CMDENQ(RZ_REG_RZVSCLCB);
    IMGRZ_CMDENQ(RZ_REG_RZVSCLCR);
}


//-------------------------------------------------------------------------
/** _IMGRZ_BypassV4TapFilter
 *  bypass V4Tap filter
 */
//-------------------------------------------------------------------------
void _IMGRZ_BypassV4TapFilter(void)
{
    // fill field
    RZREG(fg_V4TAP_FAC_Y)  = D_RZ_SCALE_FAC_H8V4 * D_RZ_SCALE_OLD_TRANSFER;
    RZREG(fg_V4TAP_OFT_Y)  = 0;
#ifdef CC_MT5363
    RZREG(fg_V4TAP_FAC_C)  = D_RZ_SCALE_FAC_H8V4 * D_RZ_SCALE_OLD_TRANSFER;
    RZREG(fg_V4TAP_OFT_C)  = 0;
    IMGRZ_CMDENQ(RZ_REG_RZYV4TAPFAC);
    IMGRZ_CMDENQ(RZ_REG_RZCV4TAPFAC);
    IMGRZ_CMDENQ(RZ_REG_YV4TAPO);
    IMGRZ_CMDENQ(RZ_REG_CV4TAPO);
#else
    RZREG(fg_V4TAP_FAC_CB) = D_RZ_SCALE_FAC_H8V4;
    RZREG(fg_V4TAP_OFT_CB) = 0;
    RZREG(fg_V4TAP_FAC_CR) = D_RZ_SCALE_FAC_H8V4;
    RZREG(fg_V4TAP_OFT_CR) = 0;
    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZV4TAPY);
    IMGRZ_CMDENQ(RZ_REG_RZV4TAPCB);
    IMGRZ_CMDENQ(RZ_REG_RZV4TAPCR);
#endif    

}


//-------------------------------------------------------------------------
/** _IMGRZ_SetGray
 */
//-------------------------------------------------------------------------
void _IMGRZ_SetGray(void)
{
    RZREG(fg_PADCBCR) = 3;
    IMGRZ_CMDENQ(RZ_REG_RZTYPE);
}


//-------------------------------------------------------------------------
/** _IMGRZ_SetPreload
 */
//-------------------------------------------------------------------------
void _IMGRZ_SetPreload(BOOL fgPreload, UINT32 u4PreloadYAddr, UINT32 u4PreloadCAddr)
{    
    RZREG(fg_PRELD) = fgPreload;
    RZREG(fg_PRELOADY) = u4PreloadYAddr >> 4;
    RZREG(fg_PRELOADC) = u4PreloadCAddr >> 4;
    IMGRZ_CMDENQ(RZ_REG_RZJPGMODE);
    IMGRZ_CMDENQ(RZ_REG_RZPRELOADY);
    IMGRZ_CMDENQ(RZ_REG_RZPRELOADC);
}

#ifdef CC_MT5363
static void _IMGRZ_RprResz(
  UINT32 u4SrcW, UINT32 u4SrcH,
  UINT32 u4TgW, UINT32 u4TgH)
{
  UINT32 qw;
  UINT32 hfactor;
  UINT32 hoffset;
  UINT32 vfactor;
  UINT32 voffset;
  
  int  uInWidth, uInHeight;
  int  uOutWidth, uOutHeight;
  int  iUxR, iUyLB;
  int  Hprime, D;
  int  m, n;
  int  ax_initial, ax_increment;
  int  iUyL_inc;
  int  uTemp;
  

  RZREG(fg_RPR) = 1;



  IMGRZ_CMDENQ(RZ_REG_RPR); 

  RZWD(u4_RZJPGMODE)=0x0001E500;
  IMGRZ_CMDENQ(RZ_REG_RZJPGMODE); 


  RZADDR(RZ_REG_RZHCOEFF0)=0x0;
  RZADDR(RZ_REG_RZHCOEFF1)=0xf0000000;
  RZADDR(RZ_REG_RZHCOEFF2)=0xe0000000;
  RZADDR(RZ_REG_RZHCOEFF3)=0xd0000000;
  RZADDR(RZ_REG_RZHCOEFF4)=0xc0000000;
  RZADDR(RZ_REG_RZHCOEFF5)=0xb0000000;
  RZADDR(RZ_REG_RZHCOEFF6)=0xa0000000;
  RZADDR(RZ_REG_RZHCOEFF7)=0x90000000;
  RZADDR(RZ_REG_RZHCOEFF8)=0x80000000;
  RZADDR(RZ_REG_RZHCOEFF9)=0x70000000;
  RZADDR(RZ_REG_RZHCOEFF10)=0x60000000;
  RZADDR(RZ_REG_RZHCOEFF11)=0x50000000;
  RZADDR(RZ_REG_RZHCOEFF12)=0x40000000;
  RZADDR(RZ_REG_RZHCOEFF13)=0x30000000;
  RZADDR(RZ_REG_RZHCOEFF14)=0x20000000;
  RZADDR(RZ_REG_RZHCOEFF15)=0x10000000;
  RZADDR(RZ_REG_RZHCOEFF16)=0x00000008;
  RZADDR(RZ_REG_RZHCOEFF17)=0x00000000;
  IMGRZ_CMDENQ(RZ_REG_RZHCOEFF0); 
  IMGRZ_CMDENQ(RZ_REG_RZHCOEFF1); 
  IMGRZ_CMDENQ(RZ_REG_RZHCOEFF2); 
  IMGRZ_CMDENQ(RZ_REG_RZHCOEFF3); 
  IMGRZ_CMDENQ(RZ_REG_RZHCOEFF4); 
  IMGRZ_CMDENQ(RZ_REG_RZHCOEFF5); 
  IMGRZ_CMDENQ(RZ_REG_RZHCOEFF6); 
  IMGRZ_CMDENQ(RZ_REG_RZHCOEFF7); 
  IMGRZ_CMDENQ(RZ_REG_RZHCOEFF8); 
  IMGRZ_CMDENQ(RZ_REG_RZHCOEFF9); 
  IMGRZ_CMDENQ(RZ_REG_RZHCOEFF10); 
  IMGRZ_CMDENQ(RZ_REG_RZHCOEFF11); 
  IMGRZ_CMDENQ(RZ_REG_RZHCOEFF12); 
  IMGRZ_CMDENQ(RZ_REG_RZHCOEFF13); 
  IMGRZ_CMDENQ(RZ_REG_RZHCOEFF14); 
  IMGRZ_CMDENQ(RZ_REG_RZHCOEFF15); 
  IMGRZ_CMDENQ(RZ_REG_RZHCOEFF16); 
  IMGRZ_CMDENQ(RZ_REG_RZHCOEFF17); 

  RZADDR(RZ_REG_RZVCOEFF0)=0xf0000000;
  RZADDR(RZ_REG_RZVCOEFF1)=0xd000e000;
  RZADDR(RZ_REG_RZVCOEFF2)=0xb000c000;
  RZADDR(RZ_REG_RZVCOEFF3)=0x9000a000;
  RZADDR(RZ_REG_RZVCOEFF4)=0x70008000;
  RZADDR(RZ_REG_RZVCOEFF5)=0x50006000;
  RZADDR(RZ_REG_RZVCOEFF6)=0x30004000;
  RZADDR(RZ_REG_RZVCOEFF7)=0x10002000;
  RZADDR(RZ_REG_RZVCOEFF8)=0x1;

  IMGRZ_CMDENQ(RZ_REG_RZVCOEFF0); 
  IMGRZ_CMDENQ(RZ_REG_RZVCOEFF1); 
  IMGRZ_CMDENQ(RZ_REG_RZVCOEFF2); 
  IMGRZ_CMDENQ(RZ_REG_RZVCOEFF3); 
  IMGRZ_CMDENQ(RZ_REG_RZVCOEFF4); 
  IMGRZ_CMDENQ(RZ_REG_RZVCOEFF5); 
  IMGRZ_CMDENQ(RZ_REG_RZVCOEFF6); 
  IMGRZ_CMDENQ(RZ_REG_RZVCOEFF7); 
  IMGRZ_CMDENQ(RZ_REG_RZVCOEFF8); 
  
  RZADDR(RZ_REG_RZOSDMODE)=0x80FF0026;
  IMGRZ_CMDENQ(RZ_REG_RZOSDMODE); 


  // -- scaling factor calculation --
  uInWidth   = u4SrcW;
  uInHeight  = u4SrcH;
  uOutWidth  = u4TgW;
  uOutHeight = u4TgH;
  
  m = 0;
  uTemp = uInWidth;
  while (uTemp > 0)
  {
    m=m+1;
    uTemp = uTemp>>1;
  }
  /* check for case when uInWidth is power of two */
  if (uInWidth == (1<<(m-1))) m=m-1;
  Hprime = 1<<m;
  D = (64*Hprime)/16;

  n = 0;
  uTemp = uInHeight;
  while (uTemp > 0)
  {
    n=n+1;
    uTemp = uTemp>>1;
  }
  /* check for case when uInHeight is power of two */
  if (uInHeight == (1<<(n-1))) n=n-1;

  /* iUxL and iUxR are independent of row, so compute once only */
  iUxR = ((((uInWidth - uOutWidth)<<1))<<(4+m));    /* numerator part */
  /* complete iUxR init by dividing by H with rounding to nearest integer, */
  /* half-integers away from 0 */
  if (iUxR >= 0)
    iUxR = (iUxR + (uOutWidth>>1))/uOutWidth;
  else
    iUxR = (iUxR - (uOutWidth>>1))/uOutWidth;

  /* initial x displacement and the x increment are independent of row */
  /* so compute once only */
  ax_initial = iUxR + (D>>1);
  ax_increment = (Hprime<<6) + (iUxR<<1);

  iUyLB = ((uInHeight - uOutHeight)<<(n+5)); /* numerator */
  /* complete iUyLB by dividing by V with rounding to nearest integer, */
  /* half-integers away from 0 */
  if (iUyLB >= 0)
    iUyLB = (iUyLB + (uOutHeight>>1))/uOutHeight;
  else
    iUyLB = (iUyLB - (uOutHeight>>1))/uOutHeight;
  iUyL_inc = (iUyLB<<1);

  hfactor = ax_increment<<(18-(m+6));
  hoffset = ax_initial<<(18-(m+6));
  vfactor = ((1<<(6+n)) + iUyL_inc) << (18-(n+6));
  voffset = (iUyLB + (1<<(1+n))) << (18-(n+6));

  RZADDR(RZ_REG_RZH8TAPFACY)=hfactor;
  RZADDR(RZ_REG_YH8TAPO)=hoffset;
  RZADDR(RZ_REG_RZYV4TAPFAC)=vfactor;
  RZADDR(RZ_REG_YV4TAPO)=voffset;
  RZADDR(RZ_REG_RZH8TAPFACCB)=hfactor;
  RZADDR(RZ_REG_CBH8TAPO)=hoffset;
  RZADDR(RZ_REG_RZCV4TAPFAC)=vfactor;
  RZADDR(RZ_REG_CV4TAPO)=voffset;

  IMGRZ_CMDENQ(RZ_REG_RZH8TAPFACY); 
  IMGRZ_CMDENQ(RZ_REG_YH8TAPO); 
  IMGRZ_CMDENQ(RZ_REG_RZYV4TAPFAC); 
  IMGRZ_CMDENQ(RZ_REG_YV4TAPO); 
  IMGRZ_CMDENQ(RZ_REG_RZH8TAPFACCB); 
  IMGRZ_CMDENQ(RZ_REG_CBH8TAPO); 
  IMGRZ_CMDENQ(RZ_REG_RZCV4TAPFAC); 
  IMGRZ_CMDENQ(RZ_REG_CV4TAPO); 


  qw = (u4SrcW << 16) | u4SrcH;
  RZWD(u4_RZSRCSZY)=qw;
  qw = ((u4SrcW >> 1) << 16) | (u4SrcH >> 1);
  RZWD(u4_RZSRCSZCB)=qw ;
  IMGRZ_CMDENQ(RZ_REG_RZSRCSZY); 
  IMGRZ_CMDENQ(RZ_REG_RZSRCSZCB); 
  qw = (u4TgW) << 16 | u4TgH;
  RZWD(u4_RZTGSZ)=qw ;
  IMGRZ_CMDENQ(RZ_REG_RZTGSZ); 


  qw = 0x10000000 | RZREG(fg_TGBUFL); 
  RZWD(u4_RZTGBUFL)=qw ;
  IMGRZ_CMDENQ(RZ_REG_RZTGBUFL); 


  RZREG(fg_INT_MASK) = IMGRZ_ENABLE;
  RZREG(fg_CKEN)     = IMGRZ_ENABLE;
  RZREG(fg_START)    = IMGRZ_ENABLE;
  IMGRZ_CMDENQ(RZ_REG_RZSTART);

}
#endif
//-------------------------------------------------------------------------
/** _IMGRZ_Scale
 */
//-------------------------------------------------------------------------
void _IMGRZ_Scale(void *pvSclParam)
{
    RZ_VDO_SCL_PARAM_SET_T *prVdoParam = NULL;
    RZ_JPG_SCL_PARAM_SET_T *prJpgParam = NULL;
    RZ_OSD_SCL_PARAM_SET_T *prOsdParam = NULL;
    UINT32 u4ForceV4Filter = 0; /*Auto select*/
    UINT8 uColorComp=0x7;
    // bypass alpha blending
    _IMGRZ_SetABlendOpt(D_RZ_A_BLEND_DEF_LVL, D_RZ_DEF_ALPHA);
            
    switch(_eRzCurInOutType)
    {
#ifdef CC_MT5363
    case E_RZ_INOUT_TYPE_VDORMRPR:
    {
        prVdoParam = (RZ_VDO_SCL_PARAM_SET_T *)pvSclParam;
        IMGRZ_ASSERT(prVdoParam != NULL);
        IMGRZ_ASSERT(prVdoParam->u4YSrcBase != NULL);
        IMGRZ_CHK_16B_ALIGN(prVdoParam->u4YSrcBase);
        IMGRZ_CHK_16B_ALIGN(prVdoParam->u4YSrcBufLen);
        IMGRZ_ASSERT(prVdoParam->u4YSrcW != 0);
        IMGRZ_ASSERT(prVdoParam->u4YSrcH != 0);
        IMGRZ_ASSERT(prVdoParam->u4YTgBase != NULL);
        IMGRZ_CHK_16B_ALIGN(prVdoParam->u4YTgBase);
        IMGRZ_CHK_16B_ALIGN(prVdoParam->u4YTgBufLen);
        IMGRZ_ASSERT(prVdoParam->u4YTgW != 0);
        IMGRZ_ASSERT(prVdoParam->u4YTgH != 0);
        IMGRZ_CHK_16B_ALIGN(prVdoParam->u4CTgBase);
        IMGRZ_ASSERT(prVdoParam->u4CTgBase != NULL);
        IMGRZ_CHK_16B_ALIGN(prVdoParam->u4CSrcBase);
        IMGRZ_ASSERT(prVdoParam->u4CSrcW != 0);
        IMGRZ_ASSERT(prVdoParam->u4CSrcH != 0);
        IMGRZ_ASSERT(prVdoParam->u4CTgW != 0);
        IMGRZ_ASSERT(prVdoParam->u4CTgH != 0);
            
        _IMGRZ_VdoSetSrcBuf(prVdoParam->u4YSrcBase, prVdoParam->u4YSrcBufLen, 
                prVdoParam->u4YSrcHOffset, prVdoParam->u4YSrcVOffset,
                prVdoParam->u4CSrcBase, 
                prVdoParam->u4CSrcHOffset, prVdoParam->u4CSrcVOffset);
        _IMGRZ_VdoSetTgBuf(prVdoParam->u4YTgBase, prVdoParam->u4YTgBufLen, 
            prVdoParam->u4YTgHOffset, prVdoParam->u4YTgVOffset, 
            prVdoParam->u4CTgBase);
		
        _IMGRZ_SetPreload(0, 0, 0);                     //clean preload setting

        _IMGRZ_SetRasterScanInOut(prVdoParam->u4IsRsIn, prVdoParam->u4IsRsOut);

        _IMGRZ_VdoSetSwap(prVdoParam->u4SrcSwap, prVdoParam->u4OutSwap);
        _IMGRZ_SaveToSameAddr(0);

        _IMGRZ_RprResz(prVdoParam->u4YSrcW,prVdoParam->u4YSrcH,prVdoParam->u4YTgW,prVdoParam->u4YTgH);
      break;
    }
#endif
    case E_RZ_INOUT_TYPE_VDOMD:
    case E_RZ_INOUT_TYPE_VDOENCODE:
    //case E_RZ_INOUT_TYPE_VDORMRPR:
        prVdoParam = (RZ_VDO_SCL_PARAM_SET_T *)pvSclParam;
        // check parameter
        IMGRZ_ASSERT(prVdoParam != NULL);
        IMGRZ_ASSERT(prVdoParam->u4YSrcBase != NULL);
        IMGRZ_CHK_16B_ALIGN(prVdoParam->u4YSrcBase);
        IMGRZ_CHK_16B_ALIGN(prVdoParam->u4YSrcBufLen);
        IMGRZ_ASSERT(prVdoParam->u4YSrcW != 0);
        IMGRZ_ASSERT(prVdoParam->u4YSrcH != 0);
        IMGRZ_ASSERT(prVdoParam->u4YTgBase != NULL);
        IMGRZ_CHK_16B_ALIGN(prVdoParam->u4YTgBase);
        IMGRZ_CHK_16B_ALIGN(prVdoParam->u4YTgBufLen);
        IMGRZ_ASSERT(prVdoParam->u4YTgW != 0);
        IMGRZ_ASSERT(prVdoParam->u4YTgH != 0);

	LOG(6, "prVdoParam->u4YSrcBase=[%x]\n", prVdoParam->u4YSrcBase);        
	LOG(6, "prVdoParam->u4YSrcBufLen=[%x]\n", prVdoParam->u4YSrcBufLen);     	
	LOG(6, "prVdoParam->u4YSrcW=[%x]\n", prVdoParam->u4YSrcW);  
	LOG(6, "prVdoParam->u4YSrcH=[%x]\n", prVdoParam->u4YSrcH);  
	LOG(6, "prVdoParam->u4YTgBase=[%x]\n", prVdoParam->u4YTgBase);  
	LOG(6, "prVdoParam->u4YTgBufLen=[%x]\n", prVdoParam->u4YTgBufLen);  
	LOG(6, "prVdoParam->u4YTgW=[%x]\n", prVdoParam->u4YTgW);  	
	LOG(6, "prVdoParam->u4YTgH=[%x]\n", prVdoParam->u4YTgH); 
        
        if(prVdoParam->u4CSrcBase==0)
        {
#if !(defined(CC_MT5363) ||defined(CC_MT5365) ||defined(CC_MT5395))
           IMGRZ_ASSERT(prVdoParam->u4CTgBase != NULL);
           IMGRZ_CHK_16B_ALIGN(prVdoParam->u4CTgBase);
           IMGRZ_ASSERT(prVdoParam->u4CTgW != 0);
           IMGRZ_ASSERT(prVdoParam->u4CTgH != 0);
           _IMGRZ_SetGray();
#endif
           uColorComp=0x4;
           prVdoParam->u4CSrcBase=prVdoParam->u4YSrcBase;
        }
        else
        {
		IMGRZ_CHK_16B_ALIGN(prVdoParam->u4CTgBase);
		IMGRZ_ASSERT(prVdoParam->u4CTgBase != NULL);
		IMGRZ_CHK_16B_ALIGN(prVdoParam->u4CSrcBase);
		IMGRZ_ASSERT(prVdoParam->u4CSrcW != 0);
		IMGRZ_ASSERT(prVdoParam->u4CSrcH != 0);
		IMGRZ_ASSERT(prVdoParam->u4CTgW != 0);
		IMGRZ_ASSERT(prVdoParam->u4CTgH != 0);
		LOG(6, "prVdoParam->u4CTgBase=[%x]\n", prVdoParam->u4CTgBase);        
		LOG(6, "prVdoParam->u4CSrcBase=[%x]\n", prVdoParam->u4CSrcBase);     	
		LOG(6, "prVdoParam->u4CSrcW=[%x]\n", prVdoParam->u4CSrcW);  
		LOG(6, "prVdoParam->u4CSrcH=[%x]\n", prVdoParam->u4CSrcH);  
		LOG(6, "prVdoParam->u4CTgW=[%x]\n", prVdoParam->u4CTgW);  
		LOG(6, "prVdoParam->u4CTgH=[%x]\n", prVdoParam->u4CTgH);            
        }
        
        _IMGRZ_SetPreload(0, 0, 0);                     //clean preload setting

        _IMGRZ_SetRasterScanInOut(prVdoParam->u4IsRsIn, prVdoParam->u4IsRsOut);

        _IMGRZ_VdoSetSwap(prVdoParam->u4SrcSwap, prVdoParam->u4OutSwap);

        _IMGRZ_SetHoriFilterOpt(
                prVdoParam->u4YSrcW, prVdoParam->u4CSrcW, prVdoParam->u4CSrcW,
                prVdoParam->u4YTgW, prVdoParam->u4CTgW, prVdoParam->u4CTgW, 
                prVdoParam->u4HFilterMode,FALSE);
            
        _IMGRZ_SetVertFilterOpt(
                prVdoParam->u4YSrcH, prVdoParam->u4CSrcH, prVdoParam->u4CSrcH, prVdoParam->u4YTgH, 
                prVdoParam->u4CTgH, prVdoParam->u4CTgH, prVdoParam->u4YTgW,
                prVdoParam->u4VFilterMode,_eRzCurInOutType);

        _IMGRZ_VdoSetSrcBuf(prVdoParam->u4YSrcBase, prVdoParam->u4YSrcBufLen, 
            prVdoParam->u4YSrcHOffset, prVdoParam->u4YSrcVOffset,
            prVdoParam->u4CSrcBase, 
            prVdoParam->u4CSrcHOffset, prVdoParam->u4CSrcVOffset);

        _IMGRZ_SaveToSameAddr(0);
        if (prVdoParam->u4IsVdo2Osd == IMGRZ_ENABLE)
        {
            // check parameter
            if (prVdoParam->u4OutMode == (UINT32)E_RZ_VDO_OUTMD_444)
            {
                IMGRZ_ASSERT(
                    prVdoParam->u4YTgCM == (UINT32)E_RZ_OSD_DIR_CM_AYCbCr8888);
            }
            else if (prVdoParam->u4OutMode == (UINT32)E_RZ_VDO_OUTMD_422)
            {
                IMGRZ_ASSERT(
                    (prVdoParam->u4YTgCM == (UINT32)E_RZ_OSD_DIR_CM_YCbYCr) || 
                    (prVdoParam->u4YTgCM == (UINT32)E_RZ_OSD_DIR_CM_CbYCrY));
            }
            else
            {
                ;   // for lint happy
            }
            
            _IMGRZ_OsdSetTgBuf(prVdoParam->u4YTgBase, 
                prVdoParam->u4YTgCM, prVdoParam->u4YTgBufLen, 
                prVdoParam->u4YTgHOffset, prVdoParam->u4YTgVOffset);
        }
        else
        {
            // check parameter
            IMGRZ_ASSERT(
                (prVdoParam->u4OutMode == (UINT32)E_RZ_VDO_OUTMD_420) ||
                (prVdoParam->u4OutMode == (UINT32)E_RZ_VDO_OUTMD_422) ||
                (prVdoParam->u4OutMode == (UINT32)E_RZ_VDO_OUTMD_444));

            _IMGRZ_VdoSetTgBuf(prVdoParam->u4YTgBase, prVdoParam->u4YTgBufLen, 
                prVdoParam->u4YTgHOffset, prVdoParam->u4YTgVOffset, 
                prVdoParam->u4CTgBase);
        }

        _IMGRZ_VdoSetOutMode(prVdoParam->u4IsVdo2Osd, prVdoParam->u4OutMode);

        //5387, need fill Y/Cb/Cr component
        _IMGRZ_JpgSetMiscParam(0, 0, 0, 0, 0, 0, uColorComp);
        
        _IMGRZ_VdoScale(prVdoParam->u4YSrcW, prVdoParam->u4YSrcH, 
            prVdoParam->u4CSrcW, prVdoParam->u4CSrcH, 
            prVdoParam->u4YTgW, prVdoParam->u4YTgH);
        break;


    case E_RZ_INOUT_TYPE_JPGMD:
        prJpgParam = (RZ_JPG_SCL_PARAM_SET_T *)pvSclParam;
        
        // check parameter
        IMGRZ_ASSERT(prJpgParam != NULL);
        IMGRZ_ASSERT(prJpgParam->u4YSrcBase1 != NULL);
        IMGRZ_ASSERT(prJpgParam->u4YSrcBase2 != NULL);
        IMGRZ_CHK_16B_ALIGN(prJpgParam->u4YSrcBase1);
        IMGRZ_CHK_16B_ALIGN(prJpgParam->u4YSrcBase2);
        IMGRZ_CHK_16B_ALIGN(prJpgParam->u4YSrcBufLen);
        IMGRZ_ASSERT(prJpgParam->u4YSrcW != 0);
        IMGRZ_ASSERT(prJpgParam->u4YSrcH != 0);
        IMGRZ_CHK_16B_ALIGN(prJpgParam->u4CbSrcBase1);
        IMGRZ_CHK_16B_ALIGN(prJpgParam->u4CbSrcBase2);
        IMGRZ_ASSERT(prJpgParam->u4CbSrcW != 0);
        IMGRZ_ASSERT(prJpgParam->u4CbSrcH != 0);
        IMGRZ_CHK_16B_ALIGN(prJpgParam->u4CrSrcBase1);
        IMGRZ_CHK_16B_ALIGN(prJpgParam->u4CrSrcBase2);
        IMGRZ_ASSERT(prJpgParam->u4CrSrcW != 0);
        IMGRZ_ASSERT(prJpgParam->u4CrSrcH != 0);
        IMGRZ_ASSERT(prJpgParam->u4YTgBase != NULL);
        IMGRZ_CHK_16B_ALIGN(prJpgParam->u4YTgBase);
        IMGRZ_CHK_16B_ALIGN(prJpgParam->u4YTgBufLen);
        IMGRZ_ASSERT(prJpgParam->u4YTgW != 0);
        IMGRZ_ASSERT(prJpgParam->u4YTgH != 0);
        IMGRZ_ASSERT(prJpgParam->u4CTgBase != NULL);
        IMGRZ_CHK_16B_ALIGN(prJpgParam->u4CTgBase);
        IMGRZ_ASSERT(prJpgParam->u4CTgW != 0);
        IMGRZ_ASSERT(prJpgParam->u4CTgH != 0);

	LOG(5, "prJpgParam->u4YSrcBase1=[%x]\n", prJpgParam->u4YSrcBase1);        
	LOG(5, "prJpgParam->u4YSrcBase2=[%x]\n", prJpgParam->u4YSrcBase2);     	
	LOG(5, "prJpgParam->u4YSrcBufLen=[%x]\n", prJpgParam->u4YSrcBufLen);  
	LOG(5, "prJpgParam->u4YSrcW=[%x]\n", prJpgParam->u4YSrcW);  
	LOG(5, "prJpgParam->u4YSrcH=[%x]\n", prJpgParam->u4YSrcH);  
	LOG(5, "prJpgParam->u4CbSrcBase1=[%x]\n", prJpgParam->u4CbSrcBase1);  
	LOG(5, "prJpgParam->u4CbSrcBase2=[%x]\n", prJpgParam->u4CbSrcBase2);  	
	LOG(5, "prJpgParam->u4CbSrcW=[%x]\n", prJpgParam->u4CbSrcW); 
	LOG(5, "prJpgParam->u4CbSrcH=[%x]\n", prJpgParam->u4CbSrcH);        
	LOG(5, "prJpgParam->u4CrSrcBase1=[%x]\n", prJpgParam->u4CrSrcBase1);  
	LOG(5, "prJpgParam->u4CrSrcBase2=[%x]\n", prJpgParam->u4CrSrcBase2);  	
	LOG(5, "prJpgParam->u4CrSrcW=[%x]\n", prJpgParam->u4CrSrcW); 
	LOG(5, "prJpgParam->u4CrSrcH=[%x]\n", prJpgParam->u4CrSrcH); 	
	LOG(5, "prJpgParam->u4YTgBase=[%x]\n", prJpgParam->u4YTgBase);     	
	LOG(5, "prJpgParam->u4YTgBufLen=[%x]\n", prJpgParam->u4YTgBufLen);  
	LOG(5, "prJpgParam->u4YTgW=[%x]\n", prJpgParam->u4YTgW);  
	LOG(5, "prJpgParam->u4YTgH=[%x]\n", prJpgParam->u4YTgH);  
	LOG(5, "prJpgParam->u4CTgBase=[%x]\n", prJpgParam->u4CTgBase);  
	LOG(5, "prJpgParam->u4CTgW=[%x]\n", prJpgParam->u4CTgW);  	
	LOG(5, "prJpgParam->u4CTgH=[%x]\n", prJpgParam->u4CTgH); 	
   
        if (prJpgParam->u4JpgTempAddr != 0)
        {
            IMGRZ_ASSERT(prJpgParam->u4JpgTempAddr != 0);
            IMGRZ_CHK_16B_ALIGN(prJpgParam->u4JpgTempAddr);        
            _IMGRZ_JpgSetTmpBuf(prJpgParam->u4JpgTempAddr);
        }    
        else
        {   //for emulation
            if (_u4JpgTmpBufAddr == 0)
            {
                // 16-byte alignment
                _u4JpgTmpBufAddr = (UINT32)BSP_AllocAlignedDmaMemory(
                                            D_RZ_JPG_TMP_BUF_SIZE, 16);
                IMGRZ_ASSERT(_u4JpgTmpBufAddr != 0);
                IMGRZ_CHK_16B_ALIGN(_u4JpgTmpBufAddr);
            }

            _IMGRZ_JpgSetTmpBuf(_u4JpgTmpBufAddr);
        }
        if (prJpgParam->fgGray)
        {
            _IMGRZ_SetGray();
        }

        if (prJpgParam->fgBlassign)
        {
            _IMGRZ_JpgEnableBLAssign(prJpgParam->u4YSrcH, prJpgParam->u4CbSrcH, prJpgParam->u4CrSrcH);
        }
                
        _IMGRZ_SetPreload(prJpgParam->fgPreload, prJpgParam->u4YTgBase, prJpgParam->u4CTgBase);
   
        _IMGRZ_SetRasterScanInOut(prJpgParam->u4IsRsIn, prJpgParam->u4IsRsOut);

        _IMGRZ_VdoSetSwap(0, prJpgParam->u4OutSwap);

        _IMGRZ_SetHoriFilterOpt(
            prJpgParam->u4YSrcW, prJpgParam->u4CbSrcW, prJpgParam->u4CrSrcW,
            prJpgParam->u4YTgW, prJpgParam->u4CTgW, prJpgParam->u4CTgW,0,FALSE);

        _IMGRZ_SetVertFilterOpt(
            prJpgParam->u4YSrcH, prJpgParam->u4CbSrcH, prJpgParam->u4CrSrcH, prJpgParam->u4YTgH,
            prJpgParam->u4CTgH, prJpgParam->u4CTgH, prJpgParam->u4YTgW, 0,_eRzCurInOutType);

        _IMGRZ_JpgSetSrcBuf(prJpgParam->u4YSrcBase1, prJpgParam->u4YSrcBase2,
            prJpgParam->u4YSrcBufLen, 
            prJpgParam->u4YSrcHOffset, prJpgParam->u4YSrcVOffset,
            prJpgParam->u4CbSrcBase1, prJpgParam->u4CbSrcBase2, 
            prJpgParam->u4CbSrcHOffset, prJpgParam->u4CbSrcVOffset,
            prJpgParam->u4CrSrcBase1, prJpgParam->u4CrSrcBase2, 
            prJpgParam->u4CrSrcHOffset, prJpgParam->u4CrSrcVOffset);

        _IMGRZ_SaveToSameAddr(prJpgParam->u4IsJpgSameaddr);
        if (prJpgParam->u4IsVdo2Osd == IMGRZ_ENABLE)
        {
            // check parameter
            if (prJpgParam->u4OutMode == (UINT32)E_RZ_VDO_OUTMD_444)
            {
                IMGRZ_ASSERT(
                    prJpgParam->u4YTgCM == (UINT32)E_RZ_OSD_DIR_CM_AYCbCr8888);
            }
            else if (prJpgParam->u4OutMode == (UINT32)E_RZ_VDO_OUTMD_422)
            {
                IMGRZ_ASSERT(
                    (prJpgParam->u4YTgCM == (UINT32)E_RZ_OSD_DIR_CM_YCbYCr) || 
                    (prJpgParam->u4YTgCM == (UINT32)E_RZ_OSD_DIR_CM_CbYCrY));
            }
            else
            {
                ;   // for lint happy
            }

            _IMGRZ_OsdSetTgBuf(prJpgParam->u4YTgBase, 
                prJpgParam->u4YTgCM, prJpgParam->u4YTgBufLen, 
                prJpgParam->u4YTgHOffset, prJpgParam->u4YTgVOffset);
        }
        else
        {
            // check parameter
            IMGRZ_ASSERT(
                (prJpgParam->u4OutMode == (UINT32)E_RZ_VDO_OUTMD_420) ||
                (prJpgParam->u4OutMode == (UINT32)E_RZ_VDO_OUTMD_422) ||
                (prJpgParam->u4OutMode == (UINT32)E_RZ_VDO_OUTMD_444));

            _IMGRZ_VdoSetTgBuf(prJpgParam->u4YTgBase, prJpgParam->u4YTgBufLen, 
                prJpgParam->u4YTgHOffset, prJpgParam->u4YTgVOffset, 
                prJpgParam->u4CTgBase);
        }

        _IMGRZ_VdoSetOutMode(prJpgParam->u4IsVdo2Osd, prJpgParam->u4OutMode);

        // set miscellaneous parameters
        _IMGRZ_JpgSetMiscParam(prJpgParam->u4IsFstBl, prJpgParam->u4IsLstBl, 
            prJpgParam->u4JpgVfacY, prJpgParam->u4JpgVfacCb, 
            prJpgParam->u4JpgVfacCr, prJpgParam->u4IsJpgRzOn, prJpgParam->u4ColorComp);
        _IMGRZ_JpgScale(prJpgParam->u4YSrcW, prJpgParam->u4YSrcH, 
            prJpgParam->u4CbSrcW, prJpgParam->u4CbSrcH, 
            prJpgParam->u4CrSrcW, prJpgParam->u4CrSrcH, 
            prJpgParam->u4YTgW, prJpgParam->u4YTgH);
        break;


    case E_RZ_INOUT_TYPE_OSDMD:
	case E_RZ_INOUT_TYPE_OSDUI:	
        prOsdParam = (RZ_OSD_SCL_PARAM_SET_T *)pvSclParam;
		if((prOsdParam->u4PartialUpdate==TRUE))
        {
            u4ForceV4Filter = 2;
        }
        
        // check parameter
        IMGRZ_ASSERT(prOsdParam != NULL);
        IMGRZ_ASSERT(prOsdParam->u4SrcBase != NULL);
        IMGRZ_CHK_16B_ALIGN(prOsdParam->u4SrcBase);
        IMGRZ_CHK_16B_ALIGN(prOsdParam->u4SrcBufLen);
        IMGRZ_ASSERT(prOsdParam->u4SrcW != 0);
        IMGRZ_ASSERT(prOsdParam->u4SrcH != 0);
        IMGRZ_ASSERT(prOsdParam->u4TgBase != NULL);
        IMGRZ_CHK_16B_ALIGN(prOsdParam->u4TgBase);
        IMGRZ_CHK_16B_ALIGN(prOsdParam->u4TgBufLen);
        IMGRZ_ASSERT(prOsdParam->u4TgW != 0);
        IMGRZ_ASSERT(prOsdParam->u4TgH != 0);

	LOG(7, "prOsdParam->u4SrcBase=[%x]\n", prOsdParam->u4SrcBase);        
	LOG(7, "prOsdParam->u4SrcBufLen=[%x]\n", prOsdParam->u4SrcBufLen);     	
	LOG(7, "prOsdParam->u4SrcW=[%x]\n", prOsdParam->u4SrcW);  
	LOG(7, "prOsdParam->u4SrcH=[%x]\n", prOsdParam->u4SrcH);  
	LOG(7, "prOsdParam->u4TgBase=[%x]\n", prOsdParam->u4TgBase);  
	LOG(7, "prOsdParam->u4TgBufLen=[%x]\n", prOsdParam->u4TgBufLen);  
	LOG(7, "prOsdParam->u4TgW=[%x]\n", prOsdParam->u4TgW);  	
	LOG(7, "prOsdParam->u4TgH=[%x]\n", prOsdParam->u4TgH); 	
	LOG(6, "prOsdParam->u4PartialUpdate=[%x]\n", prOsdParam->u4PartialUpdate);
	LOG(6, "u4ForceV4Filter=[%x]\n", u4ForceV4Filter);
	
        _IMGRZ_SetPreload(0, 0, 0);                         //clean preload setting

        _IMGRZ_SetRasterScanInOut(IMGRZ_ENABLE, IMGRZ_ENABLE);

        _IMGRZ_SetHoriFilterOpt(
            prOsdParam->u4SrcW, prOsdParam->u4SrcW, prOsdParam->u4SrcW, 
            prOsdParam->u4TgW, prOsdParam->u4TgW, prOsdParam->u4TgW, 0,prOsdParam->u4PartialUpdate);

        _IMGRZ_SetVertFilterOpt(
            prOsdParam->u4SrcH, prOsdParam->u4SrcH, prOsdParam->u4SrcH, prOsdParam->u4TgH, 
            prOsdParam->u4TgH, prOsdParam->u4TgH, prOsdParam->u4TgW, u4ForceV4Filter, _eRzCurInOutType);

        _IMGRZ_OsdSetSrcBuf(prOsdParam->u4SrcBase, prOsdParam->u4IsSrcDirCM, 
            prOsdParam->u4SrcCM, prOsdParam->u4SrcBufLen, 
            prOsdParam->u4SrcHOffset, prOsdParam->u4SrcVOffset);

        _IMGRZ_OsdSetTgBuf(prOsdParam->u4TgBase, 
            prOsdParam->u4TgCM, prOsdParam->u4TgBufLen, 
            prOsdParam->u4TgHOffset, prOsdParam->u4TgVOffset);

        _IMGRZ_SaveToSameAddr(0);
        // set miscellaneous parameters
        _IMGRZ_OsdSetMiscParam(prOsdParam->u4IsSrcDirCM, 
            prOsdParam->u4CPTDataCount, prOsdParam->pu4CPTData);
        
        #ifdef CC_MT5363
		//Resizer partial update	
		if(prOsdParam->u4PartialUpdate==TRUE)
		{
			_IMGRZ_OsdPartialUpdate(prOsdParam);
		}
        #endif
        	
        _IMGRZ_OsdScale(prOsdParam->u4SrcW, prOsdParam->u4SrcH, 
            prOsdParam->u4TgW, prOsdParam->u4TgH);

        if(prOsdParam->u4IsSrcDirCM==IMGRZ_FALSE)
        {
            _IMGRZ_OsdSetIdxTranslation(prOsdParam->u4SrcCM);
        }
        break;

    default:
        IMGRZ_ASSERT((_eRzCurInOutType == E_RZ_INOUT_TYPE_OSDMD) ||
                     (_eRzCurInOutType == E_RZ_INOUT_TYPE_JPGMD) ||
                     (_eRzCurInOutType == E_RZ_INOUT_TYPE_VDOMD) ||
                     (_eRzCurInOutType == E_RZ_INOUT_TYPE_OSDUI));
        break;
    }
}


//===================
// VDO mode
//===================

//-------------------------------------------------------------------------
/** _IMGRZ_VdoSetOutMode
 *
 */
//-------------------------------------------------------------------------
void _IMGRZ_VdoSetOutMode(UINT32 u4IsVdo2Osd, UINT32 u4VdoOutMode)
{
    // fill field & cmdque
    RZREG(fg_V2OSD) = u4IsVdo2Osd;
    RZREG(fg_OUTMD) = u4VdoOutMode;
    IMGRZ_CMDENQ(RZ_REG_RZTYPE);
}


//-------------------------------------------------------------------------
/** _IMGRZ_VdoSetSrcBuf
 *  y source buffer base address  (unit: 16-byte)
 *  y buffer length               (unit: 16-byte)
 *  y hori-offset                 (unit: pixel)
 *  y vert-offset                 (unit: pixel)
 *  cbcr source buffer base address  (unit: 16-byte)
 *  cbcr hori-offset                 (unit: pixel)
 *  cbcr vert-offset                 (unit: pixel)
 */
//-------------------------------------------------------------------------
void _IMGRZ_VdoSetSrcBuf(UINT32 u4YBaseAddr, UINT32 u4YBufLen, 
    UINT32 u4YHOffset, UINT32 u4YVOffset, 
    UINT32 u4CBaseAddr, UINT32 u4CHOffset, UINT32 u4CVOffset)
{
    // fill field
    RZREG(fg_SRCYBASE1)  = IMGRZ_DO_16B_SHRINK(u4YBaseAddr);
    RZREG(fg_SRCBUFL)    = IMGRZ_DO_16B_SHRINK(u4YBufLen);
    RZREG(fg_SRC_H_OFTY) = u4YHOffset;
    RZREG(fg_SRC_V_OFTY) = u4YVOffset;

    RZREG(fg_SRCCBBASE1)  = IMGRZ_DO_16B_SHRINK(u4CBaseAddr);
    RZREG(fg_SRC_H_OFTCB) = u4CHOffset;
    RZREG(fg_SRC_V_OFTCB) = u4CVOffset;
    
    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZSRCYB1);
    IMGRZ_CMDENQ(RZ_REG_RZSRCBUFL);
    IMGRZ_CMDENQ(RZ_REG_RZSRCOFTY);

    IMGRZ_CMDENQ(RZ_REG_RZSRCCBB1);
    IMGRZ_CMDENQ(RZ_REG_RZSRCOFTCB);
}


//-------------------------------------------------------------------------
/** _IMGRZ_VdoSetTgBuf
 *  y target buffer base address  (unit: 16-byte)
 *  y buffer length               (unit: 16-byte)
 *  y hori-offset                 (unit: pixel)
 *  y vert-offset                 (unit: pixel)
 *  cbcr target buffer base address  (unit: 16-byte)
 */
//-------------------------------------------------------------------------
void _IMGRZ_VdoSetTgBuf(UINT32 u4YBaseAddr, UINT32 u4YBufLen, 
    UINT32 u4YHOffset, UINT32 u4YVOffset,
    UINT32 u4CBaseAddr)
{
    // fill field
    RZREG(fg_TGYBASE)  = IMGRZ_DO_16B_SHRINK(u4YBaseAddr);
    RZREG(fg_TGBUFL)   = IMGRZ_DO_16B_SHRINK(u4YBufLen);
    RZREG(fg_TG_H_OFT) = u4YHOffset;
    RZREG(fg_TG_V_OFT) = u4YVOffset;

    RZREG(fg_TGCBASE)  = IMGRZ_DO_16B_SHRINK(u4CBaseAddr);
    
    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZTGYB);
    IMGRZ_CMDENQ(RZ_REG_RZTGBUFL);
    IMGRZ_CMDENQ(RZ_REG_RZTGOFT);
    IMGRZ_CMDENQ(RZ_REG_RZTGCB);
}


//-------------------------------------------------------------------------
/** _IMGRZ_VdoScale
 */
//-------------------------------------------------------------------------
void _IMGRZ_VdoScale(UINT32 u4YSrcW, UINT32 u4YSrcH, 
    UINT32 u4CSrcW, UINT32 u4CSrcH, 
    UINT32 u4YTgW, UINT32 u4YTgH)
{
    // fill field
    RZREG(fg_SRC_WIDTHY)   = u4YSrcW;
    RZREG(fg_SRC_HEIGHTY)  = u4YSrcH;
    RZREG(fg_SRC_WIDTHCB)  = u4CSrcW;
    RZREG(fg_SRC_HEIGHTCB) = u4CSrcH;
    RZREG(fg_SRC_WIDTHCR)  = u4CSrcW;
    RZREG(fg_SRC_HEIGHTCR) = u4CSrcH;

    RZREG(fg_TG_WIDTH)  = u4YTgW;
    RZREG(fg_TG_HEIGHT) = u4YTgH;

#ifdef CC_MT5363
    RZREG(fg_INT_MASK) = IMGRZ_ENABLE;
    RZREG(fg_CKEN)     = IMGRZ_ENABLE;
#endif    
    RZREG(fg_START)    = IMGRZ_ENABLE;

    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZSRCSZY);
    IMGRZ_CMDENQ(RZ_REG_RZSRCSZCB);
    IMGRZ_CMDENQ(RZ_REG_RZSRCSZCR);
    IMGRZ_CMDENQ(RZ_REG_RZTGSZ);
    IMGRZ_CMDENQ(RZ_REG_RZSTART);
}


//-------------------------------------------------------------------------
/** _IMGRZ_VdoSetSrcSwap
 */
//-------------------------------------------------------------------------
void _IMGRZ_VdoSetSwap(UINT32 u4InSwap, UINT32 u4OutSwap)
{
    RZREG(fg_ASWIN) = u4InSwap;
    RZREG(fg_ASWOUT) = u4OutSwap;
    IMGRZ_CMDENQ(RZ_REG_RZMIFMODE);
}

//===================
// JPG mode
//===================

//-------------------------------------------------------------------------
/** _IMGRZ_JpgSetTmpBuf
 *  temp buffer base address        (16-byte align)
 */
//-------------------------------------------------------------------------
void _IMGRZ_JpgSetTmpBuf(UINT32 u4BaseAddr)
{
    // fill field
    RZREG(fg_TMPBASE) = IMGRZ_DO_16B_SHRINK(u4BaseAddr);
    
    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZTMPB);
}


//-------------------------------------------------------------------------
/** _IMGRZ_JpgSetMiscParam
 *  set miscellaneous parameters
 */
//-------------------------------------------------------------------------
void _IMGRZ_JpgSetMiscParam(UINT32 u4IsFstBl, UINT32 u4IsLstBl, 
    UINT32 u4JpgVfacY, UINT32 u4JpgVfacCb, UINT32 u4JpgVfacCr,
    UINT32 u4JpgRzOn, UINT32 u4ColorComp)
{
    // fill field
    RZREG(fg_YCBCR)   = u4ColorComp;//0x7;        // y, cb, cr exist
    RZREG(fg_FST_BL)  = u4IsFstBl;
    RZREG(fg_LAST_BL) = u4IsLstBl;
    
    RZREG(fg_JPGVFAC_Y)  = u4JpgVfacY;
    RZREG(fg_JPGVFAC_CB) = u4JpgVfacCb;
    RZREG(fg_JPGVFAC_CR) = u4JpgVfacCr;

    RZREG(fg_INTFON) = u4JpgRzOn;

    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZJPGMODE);
    IMGRZ_CMDENQ(RZ_REG_RZJPGINTF);
}

void _IMGRZ_JpgEnableBLAssign(UINT32 u4SrcYHeight,UINT32 u4SrcCbHeight,UINT32 u4SrcCrHeight)
{
   
   RZREG(fg_BLASSIGN) = 1;
   RZREG(fg_LNUMY) =  u4SrcYHeight;
   RZREG(fg_LNUMCB) = u4SrcCbHeight;
   RZREG(fg_LNUMCR) = u4SrcCrHeight;
   IMGRZ_CMDENQ(RZ_REG_RZJPGMODE);
   IMGRZ_CMDENQ(RZ_REG_RZJPGVY);
   IMGRZ_CMDENQ(RZ_REG_RZJPGVCB);
   IMGRZ_CMDENQ(RZ_REG_RZJPGVCR); 
   return;
}

//-------------------------------------------------------------------------
/** _IMGRZ_JpgSetSrcBuf
 *  y source buffer base address  (unit: 16-byte)
 *  y buffer length               (unit: 16-byte)
 *  y hori-offset                 (unit: pixel)
 *  y vert-offset                 (unit: pixel)
 *  cbcr source buffer base address  (unit: 16-byte)
 *  cbcr hori-offset                 (unit: pixel)
 *  cbcr vert-offset                 (unit: pixel)
 */
//-------------------------------------------------------------------------
void _IMGRZ_JpgSetSrcBuf(UINT32 u4YBaseAddr1, UINT32 u4YBaseAddr2, 
    UINT32 u4YBufLen, 
    UINT32 u4YHOffset, UINT32 u4YVOffset, 
    UINT32 u4CbBaseAddr1, UINT32 u4CbBaseAddr2, 
    UINT32 u4CbHOffset, UINT32 u4CbVOffset,
    UINT32 u4CrBaseAddr1, UINT32 u4CrBaseAddr2, 
    UINT32 u4CrHOffset, UINT32 u4CrVOffset)
{
    // fill field
    RZREG(fg_SRCYBASE1)  = IMGRZ_DO_16B_SHRINK(u4YBaseAddr1);
    RZREG(fg_SRCYBASE2)  = IMGRZ_DO_16B_SHRINK(u4YBaseAddr2);
    RZREG(fg_SRCBUFL)    = IMGRZ_DO_16B_SHRINK(u4YBufLen);
    RZREG(fg_SRC_H_OFTY) = u4YHOffset;
    RZREG(fg_SRC_V_OFTY) = u4YVOffset;

    RZREG(fg_SRCCBBASE1)  = IMGRZ_DO_16B_SHRINK(u4CbBaseAddr1);
    RZREG(fg_SRCCBBASE2)  = IMGRZ_DO_16B_SHRINK(u4CbBaseAddr2);
    RZREG(fg_SRC_H_OFTCB) = u4CbHOffset;
    RZREG(fg_SRC_V_OFTCB) = u4CbVOffset;

    RZREG(fg_SRCCRBASE1)  = IMGRZ_DO_16B_SHRINK(u4CrBaseAddr1);
    RZREG(fg_SRCCRBASE2)  = IMGRZ_DO_16B_SHRINK(u4CrBaseAddr2);
    RZREG(fg_SRC_H_OFTCR) = u4CrHOffset;
    RZREG(fg_SRC_V_OFTCR) = u4CrVOffset;
    
    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZSRCYB1);
    IMGRZ_CMDENQ(RZ_REG_RZSRCYB2);
    IMGRZ_CMDENQ(RZ_REG_RZSRCBUFL);
    IMGRZ_CMDENQ(RZ_REG_RZSRCOFTY);

    IMGRZ_CMDENQ(RZ_REG_RZSRCCBB1);
    IMGRZ_CMDENQ(RZ_REG_RZSRCCBB2);
    IMGRZ_CMDENQ(RZ_REG_RZSRCOFTCB);

    IMGRZ_CMDENQ(RZ_REG_RZSRCCRB1);
    IMGRZ_CMDENQ(RZ_REG_RZSRCCRB2);
    IMGRZ_CMDENQ(RZ_REG_RZSRCOFTCR);
}


//-------------------------------------------------------------------------
/** _IMGRZ_JpgScale
 */
//-------------------------------------------------------------------------
void _IMGRZ_JpgScale(UINT32 u4YSrcW, UINT32 u4YSrcH, 
    UINT32 u4CbSrcW, UINT32 u4CbSrcH, 
    UINT32 u4CrSrcW, UINT32 u4CrSrcH, 
    UINT32 u4YTgW, UINT32 u4YTgH)
{
    // fill field
    RZREG(fg_SRC_WIDTHY)   = u4YSrcW;
    RZREG(fg_SRC_HEIGHTY)  = u4YSrcH;
    RZREG(fg_SRC_WIDTHCB)  = u4CbSrcW;
    RZREG(fg_SRC_HEIGHTCB) = u4CbSrcH;
    RZREG(fg_SRC_WIDTHCR)  = u4CrSrcW;
    RZREG(fg_SRC_HEIGHTCR) = u4CrSrcH;

    RZREG(fg_TG_WIDTH)  = u4YTgW;
    RZREG(fg_TG_HEIGHT) = u4YTgH;

#ifdef CC_MT5363
    RZREG(fg_INT_MASK) = IMGRZ_ENABLE;
    RZREG(fg_CKEN)     = IMGRZ_ENABLE;
#endif    
    RZREG(fg_START)    = IMGRZ_ENABLE;

    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZSRCSZY);
    IMGRZ_CMDENQ(RZ_REG_RZSRCSZCB);
    IMGRZ_CMDENQ(RZ_REG_RZSRCSZCR);
    IMGRZ_CMDENQ(RZ_REG_RZTGSZ);

    IMGRZ_CMDENQ(RZ_REG_RZSTART);
}


//===================
// OSD mode
//===================

//-------------------------------------------------------------------------
/** _IMGRZ_OsdSetMiscParam
 *  set miscellaneous parameters
 */
//-------------------------------------------------------------------------
void _IMGRZ_OsdSetMiscParam(UINT32 u4IsSrcDirCM, 
    UINT32 u4CPTDataCount, UINT32 *pu4CPTData)
{
    if (u4IsSrcDirCM == IMGRZ_TRUE) // source is direct color mode
    {
        // turn off CPT clock, switch to write clock
        RZREG(fg_INI_CPT)  = 0;
        RZREG(fg_CPT_CKEN) = 0;
        IMGRZ_CMDENQ(RZ_REG_RZOSDCTRL);
    }
    else                            // source is index color mode
    {
        UINT32 u4Idx;
        
        // turn off CPT clock, switch to write clock
        RZREG(fg_INI_CPT)  = 0;
        RZREG(fg_CPT_CKEN) = 0;
        IMGRZ_CMDENQ(RZ_REG_RZOSDCTRL);

        // turn on CPT clock
        RZREG(fg_CPT_CKEN) = 1;
        IMGRZ_CMDENQ(RZ_REG_RZOSDCTRL);

        // so far, only support 8bpp
        IMGRZ_ASSERT(pu4CPTData != NULL);

        // write CPTData to SRAM
        for (u4Idx = 0; u4Idx < u4CPTDataCount; u4Idx++)
        {
            RZREG(fg_CPT_W_ADDR) = u4Idx;
            RZREG(fg_CPT_W_DATA) = pu4CPTData[u4Idx];
            IMGRZ_CMDENQ(RZ_REG_RZCPTADDR);
            IMGRZ_CMDENQ(RZ_REG_RZCPTDATA);
        }

        // turn off CPT clock
        RZREG(fg_CPT_CKEN) = 0;
        IMGRZ_CMDENQ(RZ_REG_RZOSDCTRL);
        
        // switch to read clock
        RZREG(fg_INI_CPT)  = 1;
        IMGRZ_CMDENQ(RZ_REG_RZOSDCTRL);

        // turn on CPT clock
        RZREG(fg_CPT_CKEN) = 1;
        IMGRZ_CMDENQ(RZ_REG_RZOSDCTRL);
    }
}


//-------------------------------------------------------------------------
/** _IMGRZ_OsdSetSrcBuf
 *  source buffer base address  (unit: 16-byte)
 *  direct color mode flag      (direct/index)
 *  color mode                  (direct/index)
 *  buffer length               (unit: 16-byte)
 *  hori-offset                 (unit: pixel)
 *  vert-offset                 (unit: pixel)
 */
//-------------------------------------------------------------------------
void _IMGRZ_OsdSetSrcBuf(UINT32 u4BaseAddr, 
    UINT32 u4IsDirCM, UINT32 u4CM, UINT32 u4BufLen, 
    UINT32 u4HOffset, UINT32 u4VOffset)
{
    // fill field
    RZREG(fg_SRCYBASE1)  = IMGRZ_DO_16B_SHRINK(u4BaseAddr);
    RZREG(fg_DCMD)       = u4IsDirCM;
    RZREG(fg_INTYPE)     = u4CM;
    RZREG(fg_SRCBUFL)    = IMGRZ_DO_16B_SHRINK(u4BufLen);
    RZREG(fg_SRC_H_OFTY) = u4HOffset;
    RZREG(fg_SRC_V_OFTY) = u4VOffset;
    
    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZSRCYB1);
    IMGRZ_CMDENQ(RZ_REG_RZOSDMODE);
    IMGRZ_CMDENQ(RZ_REG_RZSRCBUFL);
    IMGRZ_CMDENQ(RZ_REG_RZSRCOFTY);
}


//-------------------------------------------------------------------------
/** _IMGRZ_OsdSetTgBuf
 *  target buffer base address  (unit: 16-byte)
 *  color mode                  (direct only)
 *  buffer length               (unit: 16-byte)
 *  hori-offset                 (unit: pixel)
 *  vert-offset                 (unit: pixel)
 */
//-------------------------------------------------------------------------
void _IMGRZ_OsdSetTgBuf(UINT32 u4BaseAddr, 
    UINT32 u4DirCM, UINT32 u4BufLen, 
    UINT32 u4HOffset, UINT32 u4VOffset)
{
    // fill field
    RZREG(fg_TGYBASE)  = IMGRZ_DO_16B_SHRINK(u4BaseAddr);
    RZREG(fg_OUTTYPE)  = u4DirCM;
    RZREG(fg_TGBUFL)   = IMGRZ_DO_16B_SHRINK(u4BufLen);
    RZREG(fg_TG_H_OFT) = u4HOffset;
    RZREG(fg_TG_V_OFT) = u4VOffset;
    
    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZTGYB);
    IMGRZ_CMDENQ(RZ_REG_RZOSDMODE);
    IMGRZ_CMDENQ(RZ_REG_RZTGBUFL);
    IMGRZ_CMDENQ(RZ_REG_RZTGOFT);
}


//-------------------------------------------------------------------------
/** _IMGRZ_OsdScale
 */
//-------------------------------------------------------------------------
void _IMGRZ_OsdScale(UINT32 u4SrcW, UINT32 u4SrcH, 
    UINT32 u4TgW, UINT32 u4TgH)
{
    // fill field
    RZREG(fg_SRC_WIDTHY)  = u4SrcW;
    RZREG(fg_SRC_HEIGHTY) = u4SrcH;
    RZREG(fg_TG_WIDTH)    = u4TgW;
    RZREG(fg_TG_HEIGHT)   = u4TgH;

#ifdef CC_MT5363
    RZREG(fg_INT_MASK) = IMGRZ_ENABLE;
    RZREG(fg_CKEN)     = IMGRZ_ENABLE;
#endif    
    RZREG(fg_START)    = IMGRZ_ENABLE;

    // fill cmdque
    IMGRZ_CMDENQ(RZ_REG_RZSRCSZY);
    IMGRZ_CMDENQ(RZ_REG_RZTGSZ);
    IMGRZ_CMDENQ(RZ_REG_RZSTART);
}


//-------------------------------------------------------------------------
/** _IMGRZ_OsdSetIdxTranslation
 */
//-------------------------------------------------------------------------
void _IMGRZ_OsdSetIdxTranslation(UINT32 u4CM)
{    
    if (u4CM == E_RZ_OSD_IDX_CM_2BPP)
    {
        RZREG(fg_COLOR_TRANS0)=0x03020100;
        IMGRZ_CMDENQ(RZ_REG_RZOSDTRAN0);
    }
    else
    {
        RZREG(fg_COLOR_TRANS0)=0x03020100;
        RZREG(fg_COLOR_TRANS1)=0x07060504;
        RZREG(fg_COLOR_TRANS2)=0x0b0a0908;
        RZREG(fg_COLOR_TRANS3)=0x0f0e0d0c;
        IMGRZ_CMDENQ(RZ_REG_RZOSDTRAN0);
        IMGRZ_CMDENQ(RZ_REG_RZOSDTRAN1);
        IMGRZ_CMDENQ(RZ_REG_RZOSDTRAN2);
        IMGRZ_CMDENQ(RZ_REG_RZOSDTRAN3);
    }
}

//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------

void _IMGRZ_Break(void)
{
    IMGRZ_HwBreak();
}

void _IMGRZ_Resume(void)
{
    IMGRZ_HwResume();
}

void _IMGRZ_OFF(void)
{
    IMGRZ_HwPowerDown();
}

//-------------------------------------------------------------------------
/** _IMGRZ_OsdScale
 */
//-------------------------------------------------------------------------
#ifdef CC_MT5363
void _IMGRZ_OsdPartialUpdate(RZ_OSD_SCL_PARAM_SET_T *prOsdParam)
{
	RZ_OSD_SCL_PARAM_SET_T *prOsdParamLocal=prOsdParam;
	UINT32 h_scl_factor, v_scl_factor, h_scl_factor_offset, v_scl_factor_offset;
	UINT32 src_scl_ptr_h, src_scl_ptr_v;

	/*
	prOsdParamLocal->u4PartialUpdate_ClipSrcx = 0;
	prOsdParamLocal->u4PartialUpdate_ClipSrcy = 0;	
	prOsdParamLocal->u4PartialUpdate_ClipSrcw = 0;
	prOsdParamLocal->u4PartialUpdate_ClipSrch = 0;	
	prOsdParamLocal->u4PartialUpdate_ClipTgx = 0;
	prOsdParamLocal->u4PartialUpdate_ClipTgy = 0;		
	prOsdParamLocal->u4PartialUpdate_ClipTgw = 233;
	prOsdParamLocal->u4PartialUpdate_ClipTgh = 171;	
	*/
	// 4-tap is necessary for vertical direction
	_IMGRZ_SwitchToV4TapFilter();
	
    h_scl_factor = IMGRZ_READ32(((RZ_REG_RZH8TAPFACY << 2) & IMGRZ_REG_MASK));
    v_scl_factor = IMGRZ_READ32(((RZ_REG_RZYV4TAPFAC << 2) & IMGRZ_REG_MASK));

	//setup source offset
    src_scl_ptr_h = prOsdParamLocal->u4PartialUpdate_ClipTgx*h_scl_factor;
    src_scl_ptr_v = prOsdParamLocal->u4PartialUpdate_ClipTgy*v_scl_factor;    

	//update source offset
	prOsdParamLocal->u4PartialUpdate_ClipSrcx = ((src_scl_ptr_h >> 18)>=3)? ((src_scl_ptr_h >> 18) - 3): (src_scl_ptr_h >> 18);
	prOsdParamLocal->u4PartialUpdate_ClipSrcy = ((src_scl_ptr_v >> 18)>=1)? ((src_scl_ptr_v >> 18) - 1): (src_scl_ptr_v >> 18);

	//update src W H
	prOsdParamLocal->u4SrcW = prOsdParamLocal->u4PartialUpdate_ClipSrcw = prOsdParamLocal->u4SrcW - prOsdParamLocal->u4PartialUpdate_ClipSrcx;
	prOsdParamLocal->u4SrcH = prOsdParamLocal->u4PartialUpdate_ClipSrch = prOsdParamLocal->u4SrcH - prOsdParamLocal->u4PartialUpdate_ClipSrcy;	

	//change to absolute coordinate
	prOsdParamLocal->u4PartialUpdate_ClipSrcx += prOsdParamLocal->u4SrcHOffset;
	prOsdParamLocal->u4PartialUpdate_ClipSrcy+= prOsdParamLocal->u4SrcVOffset;
	
	RZREG(fg_SRC_V_OFTY)  = prOsdParamLocal->u4PartialUpdate_ClipSrcy;
	RZREG(fg_SRC_H_OFTY)  = prOsdParamLocal->u4PartialUpdate_ClipSrcx;
	IMGRZ_CMDENQ(RZ_REG_RZSRCOFTY);



	//setup source scaling offset
	h_scl_factor_offset = (src_scl_ptr_h&0x3ffff)+(((src_scl_ptr_h >> 18) >= 3 ) ? 0xc0000 : (src_scl_ptr_h >> 18)*0x40000);
	RZREG(fg_H8TAP_OFT_Y)  = h_scl_factor_offset;
	IMGRZ_CMDENQ(RZ_REG_YH8TAPO);
	v_scl_factor_offset = (src_scl_ptr_v&0x3ffff)+(((src_scl_ptr_v >> 18) >= 1 ) ? 0x40000 : (src_scl_ptr_v >> 18)*0x40000);	
	RZREG(fg_V4TAP_OFT_Y)  = v_scl_factor_offset;
	IMGRZ_CMDENQ(RZ_REG_YV4TAPO);

	//setup target offset
	//RZREG(fg_TG_V_OFT)  = prOsdParamLocal->u4PartialUpdate_ClipTgy;
	RZREG(fg_TG_V_OFT)  = prOsdParamLocal->u4TgVOffset;
	RZREG(fg_TG_H_OFT)  = prOsdParamLocal->u4TgHOffset;
	IMGRZ_CMDENQ(RZ_REG_RZTGOFT);	

	//reset target W H
	prOsdParamLocal->u4TgW = prOsdParamLocal->u4PartialUpdate_ClipTgw;
	prOsdParamLocal->u4TgH = prOsdParamLocal->u4PartialUpdate_ClipTgh;	
}
#endif //#ifdef CC_MT5363

