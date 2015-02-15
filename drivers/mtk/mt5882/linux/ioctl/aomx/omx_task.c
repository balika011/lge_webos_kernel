/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/

#if defined(LINUX_TURNKEY_SOLUTION) && defined(CC_ENABLE_AOMX)

#include "x_typedef.h"
#include "x_os.h"
#include "x_printf.h"
#include "x_assert.h"
#include "drv_common.h"
#include "x_aud_dec.h"
#include "aud_if.h"
#include "aud_omx.h"

//======================================================

#ifndef SYS_Printf
//#define SYS_Printf Printf
#define SYS_Printf(fmt...)
#endif

#define AOMX_THREAD_PRIORITY 50

//======================================================

static void vAudOmxThread(void* pvArg)
{
    INT32 i4Ret = 0;
    AUD_OMX_MSG_T rMsg = {0};
#ifndef AOMX_CMD_QUEUE    
    SIZE_T zMsgSize = sizeof(rMsg);
    UINT16 u2MsgIdx;
#endif    

    AUD_OMX_HANDLE_T *pHandle = *((AUD_OMX_HANDLE_T **)pvArg);

    Printf("[AUD][OMX(%d)] vAudOmxThread: pHandle = 0x%X.\n", pHandle->h_handle, pHandle);
    
    pHandle->u4AomxThreadCnt = 0;
    
    while (TRUE)
    {    
        pHandle->u4AomxThreadCnt++;
        pHandle->u4AomxThreadState = 0;

    #ifdef AOMX_CMD_QUEUE
        if (pHandle->u4MsgReadIdx != pHandle->u4MsgWriteIdx)
        {
            rMsg = pHandle->rMsgQ[pHandle->u4MsgReadIdx];
            pHandle->u4MsgReadIdx = ((pHandle->u4MsgReadIdx+1)%AOMX_CMD_QUEUE_SIZE);
            i4Ret = OSR_OK;
        }
        else
        {
            x_thread_delay(1);
            i4Ret = -1;
        }
    #else
        // wait FillThisBuffer/EmptyThisBuffer/SendCommand event        
        if (1) //((pHandle->fgEmptying || pHandle->fgFilling))
        {
            i4Ret = x_msg_q_receive_timeout(&u2MsgIdx, (void *)&rMsg, &zMsgSize, &(pHandle->hCommandQ), 1, 5);
        }
        else
        {
            i4Ret = x_msg_q_receive(&u2MsgIdx, (void *)&rMsg, &zMsgSize, &(pHandle->hCommandQ), 1, X_MSGQ_OPTION_WAIT);
        }
    #endif

        pHandle->u4AomxThreadState = 1;
        
        if (OSR_OK == i4Ret)
        {
            pHandle->u4CmdQRecvNum++;
            if (AUD_OMX_CMD_TERMINATE == rMsg.eCmd)
            {
                Printf("[AUD][OMX(%d)][AUD_OMX_CMD_TERMINATE]\n", pHandle->h_handle);
                //TO_TEST
                switch (pHandle->eState)
                {
                    case OMX_StatePause:
                    case OMX_StateExecuting:
                        i4AudOmxStopDecoding(pHandle);
                        break;
                    default:
                        break;
                }
                break;
            }

            switch (rMsg.eCmd)
            {
            case AUD_OMX_CMD_SET_STATE:
                pHandle->u4AomxThreadState  = 2;
                i4AudOmxCompSetState(pHandle, rMsg.eStateTo);
                break;
            case AUD_OMX_CMD_EMPTY_BUFFER:
                pHandle->u4AomxThreadState  = 3;
                i4AudOmxQueueEmptyBuf(pHandle, rMsg.hHeader);
                break;
            case AUD_OMX_CMD_FILL_BUFFER:
                pHandle->u4AomxThreadState  = 4;
                i4AudOmxQueueFillBuf(pHandle, rMsg.hHeader);
                break;
            case AUD_OMX_CMD_FLUSH:
                pHandle->u4AomxThreadState  = 5;
                i4AudOmxCompFlush(pHandle, rMsg.fgInput, rMsg.fgOutput);
                break;
            case AUD_OMX_CMD_PORT_ENABLE:
                pHandle->u4AomxThreadState  = 6;
                i4AudOmxCompPortEnable(pHandle, rMsg.fgInput, rMsg.fgOutput);
                break;
            case AUD_OMX_CMD_PORT_DISABLE:
                pHandle->u4AomxThreadState  = 7;
                i4AudOmxCompPortDisable(pHandle, rMsg.fgInput, rMsg.fgOutput);
                break;
            case AUD_OMX_CMD_BUFFER_ALLOCATED:
            case AUD_OMX_CMD_BUFFER_FREED:
                break;                
            default:
                Printf("[AUD][OMX(%d)]Unknown Cmd: %d\n", pHandle->h_handle, rMsg.eCmd);
                break;
            }
        }

        pHandle->u4AomxThreadState  = 8;
        
        // check if state transition done
        if (pHandle->fgStateTransing) //TO_DEL
        {            
            pHandle->u4AomxThreadState  = 9;
            i4AudOmxCompCheckStateTransition(pHandle);
        }
        
        pHandle->u4AomxThreadState  = 10;

        // do empty buffer
        if (pHandle->fgEmptying)
        {
            pHandle->u4AomxThreadState  = 11;        
            i4AudOmxDoEmptyBuf(pHandle);
        }
        
        pHandle->u4AomxThreadState  = 12;

        // do fill buffer
        if (pHandle->fgFilling)
        {
            pHandle->u4AomxThreadState  = 13;
            i4AudOmxDoFillBuf(pHandle);
        }

        pHandle->u4AomxThreadState  = 14;

        vAudOmxCheckAoutEnableNotify(pHandle); 
        
        pHandle->u4AomxThreadState  = 15;

        // check if any EmptyThisBuffer has really done
        i4AudOmxCheckBufEmptyDone(pHandle);

        pHandle->u4AomxThreadState  = 16;

        //MONITOR_CNT_MONITOR
        vAudOmxCheckDecodeError(pHandle);
        //END
        
        pHandle->u4AomxThreadState  = 17;

        //UNDERRUN_CNT_MONITOR
        vAudOmxCheckUnderrun(pHandle);
        //END
    }

    pHandle->u4AomxThreadState  = 18;

#ifndef AOMX_CMD_QUEUE
    // delete message queue
    x_msg_q_delete(pHandle->hCommandQ);
    pHandle->hCommandQ = 0;
#endif

    Printf("[AUD][OMX(%d)] vAudOmxThread: pHandle = 0x%X. Task terminated.\n", pHandle->h_handle, pHandle);

    pHandle->u4AomxThreadState  = 255;

    x_thread_exit();
}

INT32 i4AudOmxTaskCreate(AUD_OMX_HANDLE_T *pHandle)
{
    INT32 ret;
    char acBuf[32];

#ifndef AOMX_CMD_QUEUE
    // create command queue
    x_msg_q_create(&(pHandle->hCommandQ), "AudOmxCmdQ", sizeof(AUD_OMX_MSG_T), 128/*MAX_INPUT_BUF_NO*2*/); //YONG
#endif

    Printf("[AUD][OMX(%d)] i4AudOmxTaskCreate: pHandle = 0x%X\n", pHandle->h_handle, pHandle);

    x_memset(acBuf, 0, 32 * sizeof(char));
    x_snprintf(acBuf, sizeof(acBuf), "AudOmxTask%d", pHandle->h_handle);
    
    // create audio OpenMAX main thread
    if ((ret = x_thread_create(&(pHandle->hMainTask), acBuf,
                    AUD_DRV_THREAD_STACK_SIZE, AOMX_THREAD_PRIORITY /*AUD_DRV_THREAD_PRIORITY*/,
                    vAudOmxThread, sizeof(void *), &pHandle)) != OSR_OK)
    {        
        Printf("!!!!!!!! AOMX Thread Create Error: %d (%d) !!!!!!!!\n", ret, pHandle->u4AomxThreadState);
    }

    return AUD_OK;
}

INT32 i4AudOmxTaskDestroy(AUD_OMX_HANDLE_T *pHandle)
{
    AUD_OMX_MSG_T rMsgTerminate;

    Printf("[AUD][OMX(%d)] i4AudOmxTaskDestroy\n", pHandle->h_handle);
    
    rMsgTerminate.eCmd = AUD_OMX_CMD_TERMINATE;
    i4AudOmxTaskSendCommand(pHandle, &rMsgTerminate);
    while (pHandle->u4AomxThreadState  != 255)
    {
        Printf("[AUD][OMX(%d)] Wait Thread Exit Cnt: %d State: %d CmdQSendNum: %d CmdQRecvNum: %d\n", 
                pHandle->h_handle,
                pHandle->u4AomxThreadCnt, pHandle->u4AomxThreadState,
                pHandle->u4CmdQSendNum, pHandle->u4CmdQRecvNum);
        x_thread_delay(10);
    }
    x_thread_delay(50);

    return AUD_OK;
}

INT32 i4AudOmxTaskSendCommand(AUD_OMX_HANDLE_T *pHandle, AUD_OMX_MSG_T *prMsg)
{
    INT32 i4Ret = 0;
	i4AudOmxCmdLockSem();

#ifdef AOMX_CMD_QUEUE
    if (((pHandle->u4MsgWriteIdx+1)%AOMX_CMD_QUEUE_SIZE) != (pHandle->u4MsgReadIdx))
    {
        pHandle->rMsgQ[pHandle->u4MsgWriteIdx] = *prMsg;
        pHandle->u4MsgWriteIdx = ((pHandle->u4MsgWriteIdx+1)%AOMX_CMD_QUEUE_SIZE);
        pHandle->u4CmdQSendNum++;
    }
#else
    if (NULL_HANDLE != pHandle->hCommandQ)
    {
        int u4Count = 100;
        while (u4Count--)
        {
            i4Ret = x_msg_q_send(pHandle->hCommandQ, prMsg, sizeof(AUD_OMX_MSG_T), 0/*not used*/);
            if (OSR_OK == i4Ret)
            {
                pHandle->u4CmdQSendNum++;
                break;
            }
            else
            {
                Printf("[AUD][OMX(%d)][FATALERROR][SendCmd#1]\n", pHandle->h_handle);
                x_thread_delay(10);
            }
        }
    }
#endif    
    else
    {
        Printf("[AUD][OMX(%d)][FATALERROR][SendCmd#2]\n", pHandle->h_handle);
    }
	i4AudOmxCmdUnlockSem();

    return i4Ret;
}

//======================================================

static HANDLE_T _hAudOmxSem = NULL_HANDLE;
static HANDLE_T _hAudOmxCmdSem = NULL_HANDLE;

INT32 i4AudOmxTaskSemInit(void)
{
    INT32 i4Ret = 0;

    // create semaphore, only one semaphore for this module
    if (NULL_HANDLE == _hAudOmxSem)
    {
        i4Ret = x_sema_create(&_hAudOmxSem, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
    }

    return i4Ret;
}

INT32 i4AudOmxTaskSemUninit(void)
{
    INT32 i4Ret = 0;

    // delete semaphore
    if (NULL_HANDLE != _hAudOmxSem)
    {
        i4Ret = x_sema_delete(_hAudOmxSem);
        _hAudOmxSem = NULL_HANDLE;
    }

    return i4Ret;
}

INT32 i4AudOmxTaskLockSem(void)
{
    INT32 i4Ret = 0;

    if (NULL_HANDLE != _hAudOmxSem)
    {
        i4Ret = x_sema_lock(_hAudOmxSem, X_SEMA_OPTION_WAIT);
    }

    return i4Ret;
}

INT32 i4AudOmxTaskUnlockSem(void)
{
    INT32 i4Ret = 0;

    if (NULL_HANDLE != _hAudOmxSem)
    {
        i4Ret = x_sema_unlock(_hAudOmxSem);
    }

    return i4Ret;
}

INT32 i4AudOmxCmdSemInit(void)
{
    INT32 i4Ret = 0;

    // create semaphore, only one semaphore for this module
    if (NULL_HANDLE == _hAudOmxCmdSem)
    {
        i4Ret = x_sema_create(&_hAudOmxCmdSem, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
    }

    return i4Ret;
}

INT32 i4AudOmxCmdSemUninit(void)
{
    INT32 i4Ret = 0;

    // delete semaphore
    if (NULL_HANDLE != _hAudOmxCmdSem)
    {
        i4Ret = x_sema_delete(_hAudOmxCmdSem);
        _hAudOmxCmdSem = NULL_HANDLE;
    }

    return i4Ret;
}

INT32 i4AudOmxCmdLockSem(void)
{
    INT32 i4Ret = 0;

    if (NULL_HANDLE != _hAudOmxCmdSem)
    {
		i4Ret = x_sema_lock(_hAudOmxCmdSem, X_SEMA_OPTION_WAIT);
    }

    return i4Ret;
}

INT32 i4AudOmxCmdUnlockSem(void)
{
    INT32 i4Ret = 0;

    if (NULL_HANDLE != _hAudOmxCmdSem)
    {
        i4Ret = x_sema_unlock(_hAudOmxCmdSem);
    }

    return i4Ret;
}

//======================================================

#endif

