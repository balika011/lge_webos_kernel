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

#if defined(LINUX_TURNKEY_SOLUTION) && defined(CC_ENABLE_VOMX)

#include "vomx_if.h"

#include "x_typedef.h"
#include "x_os.h"
#include "x_printf.h"
#include "x_assert.h"
#include "vdec_mod.h"
#include "vdec_omx_debug.h"


//#include "drv_thread.h"
static HANDLE_T          _hMainTask = NULL_HANDLE;
static HANDLE_T          _hCommandQ = NULL_HANDLE;
static HANDLE_T    _hTaskExitMutex = NULL_HANDLE;
static BOOL _fgVOMXTaskExist = FALSE;

static void vVDOOmxThread(void* pvArg)
{
    INT32 i4Ret = 0;
    UINT16 u2MsgIdx;
    SIZE_T zMsgSize;
    VDO_OMX_MSG_T rMsg;

    VOMX_Printf("[VDO][OMX] vVDOOmxThread\n");

    while (TRUE)
    {
        // wait FillThisBuffer/EmptyThisBuffer/SendCommand event
   //     if (pHandle->fgEmptying || pHandle->fgFilling)
   //     {
  //          i4Ret = x_msg_q_receive_timeout(&u2MsgIdx, (void *)&rMsg, &zMsgSize, &(pHandle->hCommandQ), 1, 5);
  //          VOMX_Printf("[VDO][OMX] x_msg_q_receive_timeout: i4Ret = 0x%X.\n", i4Ret);
 //       }
 //       else
        {
            zMsgSize = sizeof(VDO_OMX_MSG_T);
            i4Ret = x_msg_q_receive(&u2MsgIdx, (void *)&rMsg, &zMsgSize, &_hCommandQ, 1, X_MSGQ_OPTION_WAIT);
            if(i4Ret != OSR_OK)
            {
				LOG(3,"[VDO][OMX] x_msg_q_receive: i4Ret = 0x%X.\n", i4Ret);
            }
        }

        if (OSR_OK == i4Ret)
        {
            if (VDO_OMX_CMD_TERMINATE == rMsg.eCmd)
            {
                // exiting
                LOG(5, "[VDO][OMX]receive CMD_TERMINATE\n");
                break;
            }

            switch (rMsg.eCmd)
            {
            case VDO_OMX_CMD_SET_STATE:
                i4VDOOmxCompSetState(rMsg.pHandle, rMsg.eStateTo);
                break;
            case VDO_OMX_CMD_EMPTY_BUFFER:
//                i4VDOOmxQueueEmptyBuf(pHandle, rMsg.hHeader);
                break;
            case VDO_OMX_CMD_FILL_BUFFER:
//                i4VDOOmxQueueFillBuf(pHandle, rMsg.hHeader);
                break;
            case VDO_OMX_CMD_FLUSH:
   //             i4VDOOmxCompFlush(pHandle, rMsg.fgInput, rMsg.fgOutput);
                break;
            case VDO_OMX_CMD_BUFFER_ALLOCATED:
            case VDO_OMX_CMD_BUFFER_FREED:
                break;
            case VDO_OMX_CMD_PORT_ENABLE:
                i4VDOOmxCompPortEnable(rMsg.pHandle, rMsg.fgInput, rMsg.fgOutput);
                break;
            case VDO_OMX_CMD_PORT_DISABLE:
                i4VDOOmxCompPortDisable(rMsg.pHandle, rMsg.fgInput, rMsg.fgOutput);
                break;
            default:
                break;
            }
        }

        // check if state transition done
        if (rMsg.pHandle && rMsg.pHandle->fgStateTransing)
        {
            i4VDOOmxCompCheckStateTransition(rMsg.pHandle);
        }

        // do empty buffer
//	        if (pHandle->fgEmptying)
//	        {
//	//            i4VDOOmxDoEmptyBuf(pHandle);
//	        }
//	
//	        // do fill buffer
//	        if (pHandle->fgFilling)
//	        {
//	 //           i4VDOOmxDoFillBuf(pHandle);
//	        }
    }

    // delete message queue
    x_msg_q_delete(_hCommandQ);
    _hCommandQ = NULL_HANDLE;

	// unlock mainTask thread exit semaphore
	VERIFY(x_sema_unlock(_hTaskExitMutex) == OSR_OK);

    x_thread_exit();
}


INT32 i4VDOOmxTaskCreate(void)
{
    char acBuf[32];
    static UINT16 u2TaskId = 0;
    if(_fgVOMXTaskExist)
        return VDecOMX_OK;
    // Create mainTask thread exit semaphore
	VERIFY(x_sema_create(&_hTaskExitMutex, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);

    // create command queue
    x_memset(acBuf, 0, 32 * sizeof(char));
    x_snprintf(acBuf, sizeof(acBuf), "VOMXCQ%d", u2TaskId);

	x_msg_q_create(&_hCommandQ, acBuf, sizeof(VDO_OMX_MSG_T), 128);
    
    VOMX_Printf("[VDO][OMX] i4VDOOmxTaskCreate\n");
    // create video OpenMAX main thread
    x_memset(acBuf, 0, 32 * sizeof(char));
    x_snprintf(acBuf, sizeof(acBuf), "VOMXTskThread%d", u2TaskId++);
    x_thread_create(&_hMainTask, acBuf, VDOOMX_STACK_SIZE, VDOOMX_THREAD_PRIORITY, vVDOOmxThread, sizeof(void*), NULL);
    _fgVOMXTaskExist = TRUE;

    return VDecOMX_OK;
}


INT32 i4VDOOmxTaskDestroy(void)
{
    VDO_OMX_MSG_T rMsgTerminate;
    if(!_fgVOMXTaskExist)
        return VDecOMX_OK;
    rMsgTerminate.pHandle = NULL;
    rMsgTerminate.eCmd = VDO_OMX_CMD_TERMINATE;
    i4VDOOmxTaskSendCommand(&rMsgTerminate);

	// wait for mainTask thread exit
	VERIFY(x_sema_lock_timeout(_hTaskExitMutex, 500) == OSR_OK);//timeout value change to 2000, temp solution
	
    // delete mainTask thread exit semaphore
	VERIFY(x_sema_delete(_hTaskExitMutex) == OSR_OK);
    _fgVOMXTaskExist = FALSE;

    return VDecOMX_OK;
}

INT32 i4VDOOmxTaskSendCommand(VDO_OMX_MSG_T *prMsg)
{
    INT32 i4Ret = 0;

    if (NULL_HANDLE != _hCommandQ)
    {
    	int u4Count = 1000;
		while (u4Count--)
		{
        	i4Ret = x_msg_q_send(_hCommandQ, prMsg, sizeof(VDO_OMX_MSG_T), 0);
			if (OSR_OK == i4Ret)
			{
                if(prMsg->eCmd == VDO_OMX_CMD_TERMINATE)
                {
                    LOG(5, "[VDO][OMX]send CMD_TERMINATE\n");
                }
				break;
			}
			if((u4Count % 100) == 0)
			{
                LOG(0, "[VDO][OMX]OSR_ERR(0x%x)(%d)\n", i4Ret, u4Count);
			}
			x_thread_delay(10);
		}
    }

    return i4Ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	static HANDLE_T _hVDOOmxSem = NULL_HANDLE;
//	
//	INT32 i4VDOOmxTaskSemInit(void)
//	{
//	    INT32 i4Ret = 0;
//	
//	    // create semaphore, only one semaphore for this module
//	    if (NULL_HANDLE == _hVDOOmxSem)
//	    {
//	        i4Ret = x_sema_create(&_hVDOOmxSem, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
//	    }
//	
//	    return i4Ret;
//	}
//	
//	
//	INT32 i4VDOOmxTaskSemUninit(void)
//	{
//	    INT32 i4Ret = 0;
//	
//	    // delete semaphore
//	    if (NULL_HANDLE != _hVDOOmxSem)
//	    {
//	        i4Ret = x_sema_delete(_hVDOOmxSem);
//	        _hVDOOmxSem = NULL_HANDLE;
//	    }
//	
//	    return i4Ret;
//	}


//	INT32 i4VDOOmxTaskLockSem(void)
//	{
//	    INT32 i4Ret = 0;
//	
//	    if (NULL_HANDLE != _hVDOOmxSem)
//	    {
//	        i4Ret = x_sema_lock(_hVDOOmxSem, X_SEMA_OPTION_WAIT);
//	    }
//	
//	    return i4Ret;
//	}
//	
//	
//	INT32 i4VDOOmxTaskUnlockSem(void)
//	{
//	    INT32 i4Ret = 0;
//	
//	    if (NULL_HANDLE != _hVDOOmxSem)
//	    {
//	        i4Ret = x_sema_unlock(_hVDOOmxSem);
//	    }
//	
//	    return i4Ret;
//	}

#endif

