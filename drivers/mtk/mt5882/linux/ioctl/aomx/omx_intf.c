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

#include "x_hal_5381.h"
#include "x_util.h"
#include "x_typedef.h"
#include "x_os.h"
#include "x_printf.h"
#include "x_assert.h"
#include "x_debug.h"
#include "x_common.h"

#include <asm/uaccess.h>
#include <linux/sched.h>

#include "x_rm_dev_types.h"
#include "x_aud_dec.h"
#include "aud_if.h"
#include "aud_omx.h"

//======================================================

#ifndef SYS_Printf
//#define SYS_Printf Printf
#define SYS_Printf(fmt...)
#endif

//======================================================

/*========== LOCAL VARIABLE/FUNCTION ==========*/
#if (AUD_COMP_MAX==1)
AUD_OMX_HANDLE_T _prAudOmxHandle[AUD_COMP_MAX] = {{0}};
#else
AUD_OMX_HANDLE_T _prAudOmxHandle[AUD_COMP_MAX] = {{0},{0}};
#endif
UINT8 bOmxSettingBypass = 0; //global control variable for CLI
BOOL fgOmxCliCtrl = FALSE;
UINT8 gu1ForceDecId = AUD_DEC_MAIN;
BOOL fgForceEncFlag = FALSE;

static void vAudOmxCompDoFreeHandle(AUD_OMX_HANDLE_T *pHandle);

/*========== EXTERN VARIABLE/FUNCTION ==========*/
extern BOOL fgSkipMTAL; //extern global flow control variable
extern BOOL skip_create_feed_stream; //extern global flow control variable
extern UINT32 u4AOmxDbgMsk;
extern UINT32 u4GetABufPnt(UINT8 uDecIndex);

//======================================================

INT32 i4AudOmxCompInit(void)
{
    UINT32 u4Index;

    fgSkipMTAL = TRUE;
    skip_create_feed_stream = TRUE;
    for (u4Index = 0;u4Index < AUD_COMP_MAX;u4Index++)
    {
        _prAudOmxHandle[u4Index].fgUsed   = FALSE;
        _prAudOmxHandle[u4Index].eState   = OMX_StateMax;
        _prAudOmxHandle[u4Index].eStateTo = OMX_StateMax;
    }
    return 0;
}

INT32 i4AudOmxCompUninit(void)
{
    UINT32 u4Index;

    for (u4Index = 0;u4Index < AUD_COMP_MAX;u4Index++)
    {
        if (_prAudOmxHandle[u4Index].fgUsed)
        {
            vAudOmxCompDoFreeHandle(&_prAudOmxHandle[u4Index]);
        }
    }
    fgSkipMTAL = FALSE;
    skip_create_feed_stream = FALSE;

    return 0;
}

INT32 i4AudOmxCompGetHandle(PARAMETER_OMX_GETHANDLE *prUArg, PARAMETER_OMX_CB *prOmxCb)
{
    INT32 i4Ret = 0;
    UINT32 u4Index;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    char cCompName[255];
    PARAMETER_OMX_GETHANDLE rKArg;
    AUD_OMX_HANDLE_T *pHandle = NULL;
    INT32 i;
    HANDLE_T h_Handle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_GETHANDLE));
    i4Ret = copy_from_user(cCompName, rKArg.cComponentName, 255);

    for (i=0;i<AUD_COMP_MAX;i++)
    {
        if (!_prAudOmxHandle[i].fgUsed)
        {
            break;
        }    
    }
    
    if (AUD_COMP_MAX == i)
    {
        rKArg.eReturnValue = OMX_ErrorInsufficientResources;
        i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_GETHANDLE));

        return i4Ret;
    }
    h_Handle = (HANDLE_T)i;

	if(!fgOmxCliCtrl)
	{
		//_prAudOmxHandle[h_Handle].u1DecId = AUD_GetGstId();
		if((_prAudOmxHandle[h_Handle].u1DecId != AUD_DEC_MAIN) && (_prAudOmxHandle[h_Handle].u1DecId != AUD_DEC_AUX) && (_prAudOmxHandle[h_Handle].u1DecId != AUD_DEC_THIRD))
		{
			_prAudOmxHandle[h_Handle].u1DecId = AUD_DEC_THIRD;
#ifdef CC_AUD_ARM_RENDER
			_prAudOmxHandle[h_Handle].u1DecId = AUD_DEC_MAIN;
#endif            
		}
		_prAudOmxHandle[h_Handle].fgEncFlag = ((0 == AUD_GetGstEncFlag()) ? FALSE : TRUE);
	}
	else
	{
		_prAudOmxHandle[h_Handle].u1DecId = gu1ForceDecId;
		_prAudOmxHandle[h_Handle].fgEncFlag = fgForceEncFlag;
	}
	
    _prAudOmxHandle[h_Handle].fgFormatNotSupport = FALSE; //ERROR_CNT_MONITOR
    _prAudOmxHandle[h_Handle].fgIsAacInTs = FALSE; //AAC_IN_TS

    // check the component name
    if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.MP3", 25))
    {
        // MP3
        _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_MP3;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.AAC", 25))
    {
        // AAC
        _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_AAC;
        _prAudOmxHandle[h_Handle].u1AacChNo = 2;
        _prAudOmxHandle[h_Handle].u1AacSampleRateIdx = 3;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.PCM", 25))
    {
      //PCM
      _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_PCM;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.MP12", 26))
    {
      // MP12
      _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_MPEG;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.AC3", 25))
    {
      // AC3
      _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_AC3;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.LPCM", 26))
    {
      // LPCM
      _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_LPCM;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.MSADPCM", 29))
    {
      // LPCM - MSADPCM
      _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_OMX_MSADPCM;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.IMAADPCM", 30))
    {
      // LPCM - IMAADPCM
      _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_OMX_IMAADPCM;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.ALAW", 26))
    {
      // LPCM - ALAW
      _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_OMX_ALAW;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.ULAW", 26))
    {
      // LPCM - ULAW
      _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_OMX_ULAW;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.DTS", 25))
    {
      // DTS
      _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_DTS;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.WMA10L", 28))
    {
      // WMA10 Lossless
      _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_WMA10LOSSLESS;
      _prAudOmxHandle[h_Handle].fgFormatNotSupport = TRUE;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.WMA10S", 28))
    {
      // WMA10 Speech
      _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_WMA10SPEECH;
      _prAudOmxHandle[h_Handle].fgFormatNotSupport = TRUE;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.WMA10", 27))
    {
      // WMA10
      AUD_WMA_CFG_T rWMAProInfo = {0};
      AUD_DspWMAProHeader(_prAudOmxHandle[h_Handle].u1DecId, &rWMAProInfo);           
      _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_WMAPRO;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.WMA", 25))
    {
      // WMA
      AUD_WMA_CFG_T rWMAInfo = {0};
      AUD_DspWMAHeader(_prAudOmxHandle[h_Handle].u1DecId, &rWMAInfo);      
      _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_WMA;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.COOK", 26))
    {
      // COOK
      _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_COOK;
      _prAudOmxHandle[h_Handle].u2RaFrameSizeInBytes = 0x80;
	  _prAudOmxHandle[h_Handle].u2RaCplQBits = 0;
	  _prAudOmxHandle[h_Handle].u1RaChanNum = 2;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.VORBIS", 28))
    {
      // VORBIS
      _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_VORBISDEC;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.AMRNB", 27))
    {
      // AMRNB
      _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_AMR;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.AMRWB", 27))
    {
      // AMRWB
      _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_AWB;
    }
    else if (0 == strncmp(cCompName, "OMX.MTK.Audio.Decoder.DRA", 25))
    {
      // DRA
      _prAudOmxHandle[h_Handle].eFormat = AUD_FMT_DRA;
    }
    else
    {
        SYS_Printf("[AUD][OMX(%d)] component not found - %s\n", h_Handle, cCompName);
        omxRet = OMX_ErrorComponentNotFound;
    }
    pHandle = &_prAudOmxHandle[h_Handle];

    if (OMX_ErrorNone == omxRet)
    {
        // initialize
        _prAudOmxHandle[h_Handle].h_handle         = h_Handle;
        _prAudOmxHandle[h_Handle].fgUsed           = TRUE;
        _prAudOmxHandle[h_Handle].eState           = OMX_StateLoaded;
        _prAudOmxHandle[h_Handle].fgStateTransing  = FALSE; //TO_DEL
        _prAudOmxHandle[h_Handle].eStateTo         = OMX_StateMax;

        _prAudOmxHandle[h_Handle].pid              = current_thread_info()->task->pid;

        _prAudOmxHandle[h_Handle].fgEmptying       = FALSE;
        _prAudOmxHandle[h_Handle].fgFilling        = FALSE;
        _prAudOmxHandle[h_Handle].fgGotEOS         = FALSE;
        _prAudOmxHandle[h_Handle].fgInputPortEnable  = OMX_TRUE;
        _prAudOmxHandle[h_Handle].fgOutputPortEnable = OMX_FALSE;

        _prAudOmxHandle[h_Handle].pAppData         = rKArg.pAppData;
        _prAudOmxHandle[h_Handle].pEventHandler    = prOmxCb->pEventHandler;
        _prAudOmxHandle[h_Handle].pEmptyBufferDone = prOmxCb->pEmptyBufferDone;
        _prAudOmxHandle[h_Handle].pFillBufferDone  = prOmxCb->pFillBufferDone;
        _prAudOmxHandle[h_Handle].pFifoPntr        = NULL;
        _prAudOmxHandle[h_Handle].u4FifoSize       = 0;
        _prAudOmxHandle[h_Handle].pFifoWptr        = NULL;
        _prAudOmxHandle[h_Handle].u4CmdQSendNum    = 0;
        _prAudOmxHandle[h_Handle].u4CmdQRecvNum    = 0;
    #ifdef AOMX_CMD_QUEUE
        _prAudOmxHandle[h_Handle].u4MsgReadIdx     = 0;
        _prAudOmxHandle[h_Handle].u4MsgWriteIdx    = 0;
    #endif       

		_prAudOmxHandle[h_Handle].pCodecData	   = NULL;
		_prAudOmxHandle[h_Handle].u4CodecDataSize  = 0; 

        for (u4Index = 0; u4Index < INPUT_BUF_MAX; u4Index ++)
        {
            _prAudOmxHandle[h_Handle].pEmptyQ[u4Index] = NULL;
        }

        for (u4Index = 0; u4Index < OUTPUT_BUF_MAX; u4Index ++)
        {
            _prAudOmxHandle[h_Handle].pFillQ[u4Index] = NULL;
        }

        // create task & command queue
        i4AudOmxTaskCreate(&_prAudOmxHandle[h_Handle]);
    }

    rKArg.pHandle = pHandle;
    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_GETHANDLE));

    return 0;
}

static void vAudOmxCompDoFreeHandle(AUD_OMX_HANDLE_T *pHandle)
{
    if (pHandle->fgUsed)
    {
        // TODO: for error handling, stop decoder in case not stopped

        // destroy task
        i4AudOmxTaskDestroy(pHandle);

        pHandle->fgUsed = FALSE;
    }
}

INT32 i4AudOmxCompFreeHandle(PARAMETER_OMX_FREEHANDLE *prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    PARAMETER_OMX_FREEHANDLE rKArg;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_FREEHANDLE));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        omxRet = OMX_ErrorNone;
        vAudOmxCompDoFreeHandle(rKArg.pHandle);
    }
    else
    {
        omxRet = OMX_ErrorUndefined;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_FREEHANDLE));

    return i4Ret;
}

INT32 i4AudOmxCompGetState(PARAMETER_OMX_GETSTATE *prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    PARAMETER_OMX_GETSTATE rKArg;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_GETSTATE));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        omxRet = OMX_ErrorNone;
        i4Ret = copy_to_user(rKArg.pState, (void *)&(_prAudOmxHandle[pHandle->h_handle].eState), sizeof(OMX_STATETYPE));
    }
    else
    {
        omxRet = OMX_ErrorUndefined;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_GETSTATE));

    return i4Ret;
}

static OMX_ERRORTYPE i4AudOmxCompSetStateFromLoaded(AUD_OMX_HANDLE_T *pHandle, OMX_STATETYPE omxToState)
{
    OMX_ERRORTYPE omxRet = OMX_ErrorUndefined;

    if (OMX_StateIdle == omxToState)
    {
        // allowed
        Printf("[AUD][OMX(%d)] Component state: Loaded -> Idle.\n", pHandle->h_handle);

        // load decoder
        i4AudOmxPrepareDecoding(pHandle);
        
        omxRet = OMX_ErrorNone;
        pHandle->eState = OMX_StateIdle;
    }
    else
    {
        Printf("[AUD][OMX(%d)][FATALERROR] Component state: Loaded -> Others!!!!!.\n", pHandle->h_handle);
    }

    return omxRet;
}

static OMX_ERRORTYPE i4AudOmxCompSetStateFromIdle(AUD_OMX_HANDLE_T *pHandle, OMX_STATETYPE omxToState)
{
    OMX_ERRORTYPE omxRet = OMX_ErrorUndefined;

    if (OMX_StateExecuting == omxToState)
    {
        // allowed
        Printf("[AUD][OMX(%d)] Component state: Idle -> Executing.\n", pHandle->h_handle);

        // error detection
        if (fgAudOmxAllInputBufferFreed(pHandle) || fgAudOmxAllOutputBufferFreed(pHandle))
        {
            SYS_Printf("[AUD][OMX(%d)] Input buffer or output buffer not allocated, pHandle = 0x%X.\n", pHandle->h_handle, pHandle);
        }

        i4AudOmxAllocateAudioFifo(pHandle);

        // start playback (before sending play command, AFIFO SA/EA must be ready)
        i4AudOmxStartDecoding(pHandle, pHandle->pFifoPntr, pHandle->u4FifoSize);

        omxRet = OMX_ErrorNone;
        pHandle->eState = OMX_StateExecuting;        
    }
    else if (OMX_StateLoaded == omxToState)
    {    
        // 3.1.1.2.2.1 OMX_StateIdle to OMX_StateLoaded
        // On a transition from OMX_StateIdle to OMX_StateLoaded, each buffer supplier shall call
        // OMX_FreeBuffer on the non-supplier port for each buffer residing at the non-supplier port.
        // If the supplier allocated the buffer, it shall free the buffer before calling OMX_FreeBuffer.
        // If the non-supplier port allocated the buffer, it shall free the buffer upon receipt of an
        // OMX_FreeBuffer call. Furthermore, a non-supplier port shall always free the buffer header
        // upon receipt of an OMX_FreeBuffer call. When all of the buffers have been removed from the
        // component, the state transition is complete; the component communicates that the initiating
        // OMX_SendCommand call has completed via a callback event.
        #if 1
        omxRet = OMX_ErrorNone;
        pHandle->eState = OMX_StateLoaded;
        #else
        //TO_DEL
        pHandle->eStateTo = OMX_StateLoaded;        
        pHandle->fgStateTransing = TRUE;
        #endif

        Printf("[AUD][OMX(%d)] Component state: Idle -> Loaded, asynchronous.\n", pHandle->h_handle);
    }
    else
    {
        Printf("[AUD][OMX(%d)][FATALERROR] Component state: Idle -> Others!!!!!.\n", pHandle->h_handle);
    }

    return omxRet;
}

static OMX_ERRORTYPE i4AudOmxCompSetStateFromExecuting(AUD_OMX_HANDLE_T *pHandle, OMX_STATETYPE omxToState)
{
    OMX_ERRORTYPE omxRet = OMX_ErrorUndefined;

    if (OMX_StateIdle == omxToState)
    {
        // allowed
        Printf("[AUD][OMX(%d)] Component state: Executing -> Idle.\n", pHandle->h_handle);

        // stop playback
        i4AudOmxStopDecoding(pHandle);

        // If the IL client requests a state transition from OMX_StateExecuting to OMX_StateIdle,
        // the component shall return all buffers to their respective suppliers and receive all buffers
        // belonging to its supplier ports before completing the transition. Any port communicating
        // with the IL client shall return any buffers it is holding via EmptyBufferDone and FillBufferDone
        // callbacks, which are used by input and output ports, respectively.
        i4AudOmxForceAllBufEmptyDone(pHandle);

        i4AudOmxForceAllBufFillDone(pHandle);

        omxRet = OMX_ErrorNone;
        pHandle->eState = OMX_StateIdle;
    }
    else if (OMX_StatePause == omxToState)
    {
        // allowed
        Printf("[AUD][OMX(%d)] Component state: Executing -> Pause.\n", pHandle->h_handle);

        // stop playback
        i4AudOmxPauseDecoding(pHandle);

        omxRet = OMX_ErrorNone;
        pHandle->eState = OMX_StatePause;
    }
    else
    {
        Printf("[AUD][OMX(%d)][FATALERROR] Component state: Executing -> Others!!!!!.\n", pHandle->h_handle);
    }    

    return omxRet;
}

static OMX_ERRORTYPE i4AudOmxCompSetStateFromPause(AUD_OMX_HANDLE_T *pHandle, OMX_STATETYPE omxToState)
{
    OMX_ERRORTYPE omxRet = OMX_ErrorUndefined;

    if (OMX_StateIdle == omxToState)
    {
        // allowed
        Printf("[AUD][OMX(%d)] Component state: Pause -> Idle.\n", pHandle->h_handle);

        // stop playback
        i4AudOmxStopDecoding(pHandle);

        // If the IL client requests a state transition from OMX_StateExecuting to OMX_StateIdle,
        // the component shall return all buffers to their respective suppliers and receive all buffers
        // belonging to its supplier ports before completing the transition. Any port communicating
        // with the IL client shall return any buffers it is holding via EmptyBufferDone and FillBufferDone
        // callbacks, which are used by input and output ports, respectively.
        i4AudOmxForceAllBufEmptyDone(pHandle);

        i4AudOmxForceAllBufFillDone(pHandle);

        omxRet = OMX_ErrorNone;
        pHandle->eState = OMX_StateIdle;
    }
    else if (OMX_StateExecuting== omxToState)
    {
        // allowed
        Printf("[AUD][OMX(%d)] Component state: Pause -> Executing.\n", pHandle->h_handle);

        // Resume
        i4AudOmxResumeDecoding(pHandle);

        omxRet = OMX_ErrorNone;
        pHandle->eState = OMX_StateExecuting;
    }
    else
    {
        Printf("[AUD][OMX(%d)][FATALERROR] Component state: Pause -> Others!!!!!.\n", pHandle->h_handle);
    }    
    
    return omxRet;
}

INT32 i4AudOmxCompSetState(AUD_OMX_HANDLE_T *pHandle, OMX_STATETYPE omxStateTo)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    OMX_STATETYPE omxStateFrom;
    PARAMETER_OMX_CB_EVENTHANDLER rEvent;

    omxStateFrom = pHandle->eState;

    if (omxStateTo == omxStateFrom)
    {
        omxRet = OMX_ErrorSameState;
    }
    else
    {
        switch (omxStateFrom)
        {
        case OMX_StateLoaded:            
            omxRet = i4AudOmxCompSetStateFromLoaded(pHandle, omxStateTo);
            break;
        case OMX_StateIdle:            
            omxRet = i4AudOmxCompSetStateFromIdle(pHandle, omxStateTo);
            break;
        case OMX_StateExecuting:            
            omxRet = i4AudOmxCompSetStateFromExecuting(pHandle, omxStateTo);
            break;
        case OMX_StatePause:            
            omxRet = i4AudOmxCompSetStateFromPause(pHandle, omxStateTo);
            break;
        default:
            omxRet = OMX_ErrorUndefined;
            break;
        }
    }

    if (pHandle->fgStateTransing) //TO_DEL
    {
        // do not callback now, need wait for transiting finished
        return i4Ret;
    }

    // callback for state changed
    if (OMX_ErrorNone == omxRet)
    {
        rEvent.eEvent     = OMX_EventCmdComplete;
        rEvent.nData1     = OMX_CommandStateSet;
        rEvent.nData2     = pHandle->eState;
        rEvent.pEventData = NULL;
    }
    else
    {
        rEvent.eEvent     = OMX_EventError;
        rEvent.nData1     = omxRet;
        rEvent.nData2     = 0;
        rEvent.pEventData = NULL;
    }

    i4AudOmxCompNotifyEvent(pHandle, &rEvent);

    return i4Ret;
}

//TO_DEL
INT32 i4AudOmxCompCheckStateTransition(AUD_OMX_HANDLE_T *pHandle)
{
    INT32 i4Ret = 0;
    BOOL fgDone = FALSE;
    OMX_STATETYPE omxStateFrom;
    OMX_STATETYPE omxStateTo;
    PARAMETER_OMX_CB_EVENTHANDLER rEvent;

    omxStateFrom = pHandle->eState;
    omxStateTo   = pHandle->eStateTo;

    if (OMX_StateLoaded == omxStateTo)
    {
        // need check if all buffers are freed
        if (fgAudOmxAllInputBufferFreed(pHandle) && fgAudOmxAllOutputBufferFreed(pHandle))
        {
            fgDone = TRUE;

            SYS_Printf("[AUD][OMX(%d)] Component state: asynchronous to Loaded done.\n", pHandle->h_handle);
        }
    }
    else if (OMX_StateIdle == omxStateTo)
    {
        // need check if buffers are allocated
        if (!fgAudOmxAllInputBufferFreed(pHandle) && !fgAudOmxAllOutputBufferFreed(pHandle))
        {
            fgDone = TRUE;

            SYS_Printf("[AUD][OMX(%d)] Component state: asynchronous to Idle done.\n", pHandle->h_handle);
        }
    }

    if (fgDone)
    {
        pHandle->eState          = pHandle->eStateTo;
        pHandle->eStateTo        = OMX_StateMax;
        pHandle->fgStateTransing = FALSE;

        // callback for state changed
        rEvent.eEvent     = OMX_EventCmdComplete;
        rEvent.nData1     = OMX_CommandStateSet;
        rEvent.nData2     = pHandle->eState;
        rEvent.pEventData = NULL;

        i4AudOmxCompNotifyEvent(pHandle, &rEvent);
    }

    return i4Ret;
}

INT32 i4AudOmxCompSetStateAsync(PARAMETER_OMX_COMMAND_STATESET *prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    PARAMETER_OMX_COMMAND_STATESET rKArg;
    AUD_OMX_MSG_T rMsgSetState;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_COMMAND_STATESET));
    
    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        // status is valid, proceed to send command to queue
        rMsgSetState.eCmd     = AUD_OMX_CMD_SET_STATE;
        rMsgSetState.eStateTo = rKArg.eStateType;

        i4Ret = i4AudOmxTaskSendCommand(rKArg.pHandle, &rMsgSetState);
    }
    else
    {
        omxRet = OMX_ErrorUndefined;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_COMMAND_STATESET));

    return i4Ret;
}

// When the IL client flushes a non-tunnelling port, that port shall return all buffers it is holding
// to the IL client using EmptyBufferDone and FillBufferDone (appropriate for an input port or an output
// port, respectively) to return the buffers.
// For each port that the component successfully flushes, the component shall send an OMX_EventCmdComplete
// event, indicating OMX_CommandFlush for nData1 and the individual port index for nData2, even if the
// flush resulted from using a value of OMX_ALL for nParam.
INT32 i4AudOmxCompFlush(AUD_OMX_HANDLE_T *pHandle, BOOL fgInput, BOOL fgOutput)
{
    INT32 i4Ret = 0;
    PARAMETER_OMX_CB_EVENTHANDLER rEvent;

    if (fgInput)
    {
        Printf("[AUD][OMX(%d)]i4AudOmxCompFlush\n", pHandle->h_handle);

        // flush all pending empty buffer request
        i4AudOmxForceAllBufEmptyDone(pHandle);

        //2011/7/28 for flush cmd
        i4AudOmxForceAllBufFillDone(pHandle);

        pHandle->fgGotEOS = FALSE;

        vAudOmxRestartDecoding(pHandle);
        //End

        // callback for flush done
        rEvent.eEvent     = OMX_EventCmdComplete;
        rEvent.nData1     = OMX_CommandFlush;
        rEvent.nData2     = 0;
        rEvent.pEventData = NULL;

        i4AudOmxCompNotifyEvent(pHandle, &rEvent);
    }

    if (fgOutput)
    {
        if (u4AOmxDbgMsk & 0x00000040)
        {
            Printf("i4AudOmxCompFlush(O)\n");
        }

        // flush all pending fill buffer request
        i4AudOmxForceAllBufFillDone(pHandle);

        // callback for flush done
        rEvent.eEvent     = OMX_EventCmdComplete;
        rEvent.nData1     = OMX_CommandFlush;
        rEvent.nData2     = 1;
        rEvent.pEventData = NULL;

        i4AudOmxCompNotifyEvent(pHandle, &rEvent);
    }

    return i4Ret;
}

// This IL client calls this command to flush one or more component ports.
// nParam specifies the index of the port to flush. If the value of nParam is OMX_ALL, the component
// shall flush all ports.
INT32 i4AudOmxCompFlushAsync(PARAMETER_OMX_COMMAND_PORT *prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    PARAMETER_OMX_COMMAND_PORT rKArg;
    AUD_OMX_MSG_T rMsgFlush;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_COMMAND_PORT));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        // status is valid, proceed to send command to queue
        rMsgFlush.eCmd     = AUD_OMX_CMD_FLUSH;
        rMsgFlush.fgInput  = (0 == rKArg.nPorts) || (OMX_ALL == rKArg.nPorts);
        rMsgFlush.fgOutput = (1 == rKArg.nPorts) || (OMX_ALL == rKArg.nPorts);

        i4Ret = i4AudOmxTaskSendCommand(rKArg.pHandle, &rMsgFlush);
    }
    else
    {
        omxRet = OMX_ErrorUndefined;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_COMMAND_PORT));

    return i4Ret;
}

INT32 i4AudOmxCompPortDisable(AUD_OMX_HANDLE_T *pHandle, BOOL fgInput, BOOL fgOutput)
{
    INT32 i4Ret = 0;
    PARAMETER_OMX_CB_EVENTHANDLER rEvent;

    if (fgInput)
    {
        // flush all pending empty buffer request
        i4AudOmxForceAllBufEmptyDone(pHandle);

        pHandle->fgInputPortEnable = OMX_FALSE; 
        // TODO: wait all buffers freed before callback command complete

        // callback for port disabling done
        rEvent.eEvent     = OMX_EventCmdComplete;
        rEvent.nData1     = OMX_CommandPortDisable;
        rEvent.nData2     = 0;
        rEvent.pEventData = NULL;

        i4AudOmxCompNotifyEvent(pHandle, &rEvent);
    }

    if (fgOutput)
    {
        // flush all pending fill buffer request
        i4AudOmxForceAllBufFillDone(pHandle);

        pHandle->fgOutputPortEnable = OMX_FALSE;
	  	
        // TODO: wait all buffers freed before callback command complete

        // callback for port disabling done
        rEvent.eEvent     = OMX_EventCmdComplete;
        rEvent.nData1     = OMX_CommandPortDisable;
        rEvent.nData2     = 1;
        rEvent.pEventData = NULL;

        i4AudOmxCompNotifyEvent(pHandle, &rEvent);
    }

    return i4Ret;
}

INT32 i4AudOmxCompPortDisableAsync(PARAMETER_OMX_COMMAND_PORT *prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    PARAMETER_OMX_COMMAND_PORT rKArg;
    AUD_OMX_MSG_T rMsgFlush;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_COMMAND_PORT));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        // status is valid, proceed to send command to queue
        rMsgFlush.eCmd     = AUD_OMX_CMD_PORT_DISABLE;
        rMsgFlush.fgInput  = (0 == rKArg.nPorts) || (OMX_ALL == rKArg.nPorts);
        rMsgFlush.fgOutput = (1 == rKArg.nPorts) || (OMX_ALL == rKArg.nPorts);

        // TODO: modify port definition bEnabled flag

        i4Ret = i4AudOmxTaskSendCommand(rKArg.pHandle, &rMsgFlush);
    }
    else
    {
        omxRet = OMX_ErrorUndefined;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_COMMAND_PORT));

    return i4Ret;
}

INT32 i4AudOmxCompPortEnable(AUD_OMX_HANDLE_T *pHandle, BOOL fgInput, BOOL fgOutput)
{
    INT32 i4Ret = 0;
    PARAMETER_OMX_CB_EVENTHANDLER rEvent;

    if (fgInput)
    {
        // TODO:
        pHandle->fgInputPortEnable = OMX_TRUE; 

        // callback for port enabling done
        rEvent.eEvent     = OMX_EventCmdComplete;
        rEvent.nData1     = OMX_CommandPortEnable;
        rEvent.nData2     = 0;
        rEvent.pEventData = NULL;

        i4AudOmxCompNotifyEvent(pHandle, &rEvent);
    }

    if (fgOutput)
    {
        // TODO:
        pHandle->fgOutputPortEnable = OMX_TRUE; 

        // callback for port enabling done
        rEvent.eEvent     = OMX_EventCmdComplete;
        rEvent.nData1     = OMX_CommandPortEnable;
        rEvent.nData2     = 1;
        rEvent.pEventData = NULL;

        i4AudOmxCompNotifyEvent(pHandle, &rEvent);
    }

    return i4Ret;
}

INT32 i4AudOmxCompPortEnableAsync(PARAMETER_OMX_COMMAND_PORT *prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    PARAMETER_OMX_COMMAND_PORT rKArg;
    AUD_OMX_MSG_T rMsgFlush;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_COMMAND_PORT));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        // status is valid, proceed to send command to queue
        rMsgFlush.eCmd     = AUD_OMX_CMD_PORT_ENABLE;
        rMsgFlush.fgInput  = (0 == rKArg.nPorts) || (OMX_ALL == rKArg.nPorts);
        rMsgFlush.fgOutput = (1 == rKArg.nPorts) || (OMX_ALL == rKArg.nPorts);

        // TODO: modify port definition bEnabled flag

        i4Ret = i4AudOmxTaskSendCommand(rKArg.pHandle, &rMsgFlush);
    }
    else
    {
        omxRet = OMX_ErrorUndefined;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_COMMAND_PORT));

    return i4Ret;
}

INT32 i4AudOmxCompAllocateBuffer(PARAMETER_OMX_ALLOCATEBUFFER *prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorUndefined;
    PARAMETER_OMX_ALLOCATEBUFFER rKArg;
    AUD_OMX_BUFFER_T *pBuffer = NULL; // internal buffer handle
    AUD_OMX_MSG_T rMsgAllocated;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_ALLOCATEBUFFER));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        if (0 == rKArg.nPortIndex)
        {
            // input port
            SYS_Printf("[AUD][OMX(%d)] To allocate input buffer, size = %d bytes\n", pHandle->h_handle, rKArg.nSizeBytes);
            i4AudOmxAllocInputBuf(rKArg.pHandle, rKArg.nSizeBytes, rKArg.nPortIndex, &pBuffer);
        }
        else if (1 == rKArg.nPortIndex)
        {
            // output port
            SYS_Printf("[AUD][OMX(%d)] To allocate output buffer, size = %d bytes\n", pHandle->h_handle, rKArg.nSizeBytes);
            i4AudOmxAllocOutputBuf(rKArg.pHandle, rKArg.nSizeBytes, rKArg.nPortIndex, &pBuffer);
        }

        if (NULL != pBuffer)
        {
            rKArg.hPBufHeaderBuf = pBuffer->hBuffer;
            rKArg.hPBufHeader    = pBuffer->hHeader;

            // fill the private data
            pBuffer->rHeader.pAppPrivate = rKArg.pAppPrivate;

            rMsgAllocated.eCmd = AUD_OMX_CMD_BUFFER_ALLOCATED;
            i4AudOmxTaskSendCommand(rKArg.pHandle, &rMsgAllocated);

            SYS_Printf("[AUD][OMX(%d)] Buffer allocated: SA = 0x%X, pBuffer->pOwner = 0x%X.\n", pHandle->h_handle, pBuffer->pBufSA, pBuffer->pOwner);

            omxRet = OMX_ErrorNone;
        }
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_ALLOCATEBUFFER));

    return i4Ret;
}

INT32 i4AudOmxCompFreeBuffer(PARAMETER_OMX_FREEBUFFER *prUArg)
{
    INT32 i4Ret = AUD_FAIL;
    OMX_ERRORTYPE omxRet = OMX_ErrorUndefined;
    PARAMETER_OMX_FREEBUFFER rKArg;
    AUD_OMX_MSG_T rMsgFreed;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_FREEBUFFER));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        if (0 == rKArg.nPortIndex)
        {
            // input port
            i4Ret = i4AudOmxFreeInputBuf(rKArg.pHandle, rKArg.hPBufHeader);
        }
        else if (1 == rKArg.nPortIndex)
        {
            // output port
            i4Ret = i4AudOmxFreeOutputBuf(rKArg.pHandle, rKArg.hPBufHeader);
        }

        if (AUD_OK == i4Ret)
        {
            // trigger main task for checking if state transition is done
            rMsgFreed.eCmd = AUD_OMX_CMD_BUFFER_FREED;
            i4AudOmxTaskSendCommand(rKArg.pHandle, &rMsgFreed);

            omxRet = OMX_ErrorNone;
        }
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_FREEBUFFER));

    return i4Ret;
}

// The OMX_EmptyThisBuffer macro will send a filled buffer to an input port of a component.
// When the buffer contains data, the value of the nFilledLen field of the buffer header will not be zero.
// If the buffer contains no data, the value of nFilledLen is 0x0. The OMX_EmptyThisBuffer macro is
// invoked to pass buffers containing data when the component is in or making a transition to the
// OMX_StateExecuting or in the OMX_StatePause state.
INT32 i4AudOmxCompEmptyBuffer(PARAMETER_OMX_EMPTYTHISBUFFER *prUArg)
{
    INT32 i4Ret = AUD_FAIL;
    OMX_ERRORTYPE omxRet = OMX_ErrorUndefined;
    PARAMETER_OMX_EMPTYTHISBUFFER rKArg;
    AUD_OMX_MSG_T rMsgEmptyBuf;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_EMPTYTHISBUFFER));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        // status is valid, proceed to send command to queue
        rMsgEmptyBuf.eCmd    = AUD_OMX_CMD_EMPTY_BUFFER;
        rMsgEmptyBuf.hHeader = rKArg.hPBufHeader;

        // this will trigger i4AudOmxQueueEmptyBuf() in main task
        i4Ret = i4AudOmxTaskSendCommand(rKArg.pHandle, &rMsgEmptyBuf);

        omxRet = OMX_ErrorNone;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_EMPTYTHISBUFFER));

    return i4Ret;
}

// The OMX_FillThisBuffer macro will send an empty buffer to an output port of a component.
// The OMX_FillThisBuffer macro is invoked to pass buffers containing no data when the component is in
// or making a transition to the OMX_StateExecuting state or is in the OMX_StatePaused state.
INT32 i4AudOmxCompFillBuffer(PARAMETER_OMX_FILLTHISBUFFER *prUArg)
{
    INT32 i4Ret = AUD_FAIL;
    OMX_ERRORTYPE omxRet = OMX_ErrorUndefined;
    PARAMETER_OMX_FILLTHISBUFFER rKArg;
    AUD_OMX_MSG_T rMsgFillBuf;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_FILLTHISBUFFER));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        // status is valid, proceed to send command to queue
        rMsgFillBuf.eCmd    = AUD_OMX_CMD_FILL_BUFFER;
        rMsgFillBuf.hHeader = rKArg.hPBufHeader;

        i4Ret = i4AudOmxTaskSendCommand(rKArg.pHandle, &rMsgFillBuf);

        omxRet = OMX_ErrorNone;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_FILLTHISBUFFER));

    return i4Ret;
}

// A component uses the EmptyBufferDone callback to pass a buffer from an input port back to the IL client.
// A component sets the nOffset and nFilledLen values of the buffer header to reflect the portion of the
// buffer it consumed; for example, nFilledLen is set equal to 0x0 if completely consumed.
// The EmptyBufferDone call is a blocking call that should return from within five milliseconds.
INT32 i4AudOmxCompNotifyEmptyDone(AUD_OMX_HANDLE_T *pHandle, AUD_OMX_BUFFER_T *pBufEmptied)
{
    PARAMETER_OMX_CB_BUFFER rCbInfo;

    pBufEmptied->rHeader.nFilledLen = pBufEmptied->u4FilledLen;

    x_memset(&rCbInfo, 0, sizeof(PARAMETER_OMX_CB_BUFFER)); //New Added
    rCbInfo.hPBufHeader = pBufEmptied->hHeader;
    rCbInfo.pPlatformPrivate = pBufEmptied->rHeader.pPlatformPrivate; //New Added

    //SYS_Printf("[AUD][OMX(%d)] i4AudOmxCompNotifyEmptyDone: pBufEmptied = 0x%X, pid = %d, pHandle->pEmptyBufferDone = 0x%X.\n", pHandle->h_handle,
    //           pBufEmptied, pHandle->pid, pHandle->pEmptyBufferDone);

    if(pHandle && pHandle->pEmptyBufferDone)
    {
        pHandle->pEmptyBufferDone(&rCbInfo, pHandle->pAppData);
    }

    return AUD_OK;
}

// A component uses the FillBufferDone callback to pass a buffer from an output port back to the IL client.
// A component sets the nOffset and nFilledLen of the buffer header to reflect the portion of the buffer
// it filled (for example, nFilledLen is equal to 0x0 if it contains no data).
// The FillBufferDone call is a blocking call that should return from within five milliseconds.
INT32 i4AudOmxCompNotifyFillDone(AUD_OMX_HANDLE_T *pHandle, AUD_OMX_BUFFER_T *pBufFilled)
{
    PARAMETER_OMX_CB_BUFFER rCbInfo;

    pBufFilled->rHeader.nFilledLen = pBufFilled->u4FilledLen;
    pBufFilled->rHeader.nOffset    = 0;

	if (pBufFilled->rHeader.nFilledLen > MAX_OUTPUT_BUF_SIZE)
	{
		pBufFilled->rHeader.nFilledLen = MAX_OUTPUT_BUF_SIZE;
	}

    x_memset(&rCbInfo, 0, sizeof(PARAMETER_OMX_CB_BUFFER)); //New Added
    rCbInfo.hPBufHeader = pBufFilled->hHeader;
    rCbInfo.pPlatformPrivate = pBufFilled->rHeader.pPlatformPrivate; //New Added

    //SYS_Printf("[AUD][OMX(%d)] i4AudOmxCompNotifyFillDone: pBufFilled = 0x%X, pid = %d, pHandle->pFillBufferDone = 0x%X.\n", pHandle->h_handle,
    //           pBufFilled, pHandle->pid, pHandle->pFillBufferDone);

    //SYS_Printf("[AUD][OMX(%d)] i4AudOmxCompNotifyFillDone: nFilledLen = %d.\n", pHandle->h_handle, pBufFilled->rHeader.nFilledLen);

    if(pHandle && pHandle->pFillBufferDone)
    {
        pHandle->pFillBufferDone(&rCbInfo, pHandle->pAppData);
    }

    return AUD_OK;
}

// A call to EventHandler is a blocking call.
INT32 i4AudOmxCompNotifyEvent(AUD_OMX_HANDLE_T *pHandle, PARAMETER_OMX_CB_EVENTHANDLER *pEventInfo)
{
    //SYS_Printf("[AUD][OMX(%d)] i4AudOmxCompNotifyEvent: pid = %d, pHandle->pEventHandler = 0x%X.\n", pHandle->h_handle,
    //           pHandle->pid, pHandle->pEventHandler);

    if(pHandle && pHandle->pEventHandler)
    {
        //SYS_Printf("[AUD][OMX(%d)] [i4AudOmxCompNotifyEvent: pHandle(0x%08x) pEventInfo(0x%08x) pAppData(0x%08x)]\n", pHandle->h_handle, pHandle, pEventInfo, pHandle->pAppData);
        pHandle->pEventHandler(pEventInfo, pHandle->pAppData);
    }

    return AUD_OK;
}

INT32 i4AudOmxCompGetParamAudioInit(PARAMETER_OMX_PORT_PARAM_TYPE *prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorUndefined;
    PARAMETER_OMX_PORT_PARAM_TYPE rKArg;
    OMX_PORT_PARAM_TYPE rPortInfo;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_PORT_PARAM_TYPE));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        i4Ret = copy_from_user(&rPortInfo, rKArg.pPortParamType, sizeof(OMX_PORT_PARAM_TYPE));

        if (OMX_PARAM_SET == rKArg.eDir)
        {
            // Set Parameter
            SYS_Printf("[AUD][OMX(%d)] set OMX_IndexParamAudioInit?\n", pHandle->h_handle);
        }
        else
        {
            // Get Parameter
            rPortInfo.nPorts = 2; // 2 ports: 0 - input, 1 - output
            rPortInfo.nStartPortNumber = 0;

            omxRet = OMX_ErrorNone;

            i4Ret = copy_to_user(rKArg.pPortParamType, (void *)&rPortInfo, sizeof(OMX_PORT_PARAM_TYPE));
        }
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_PORT_PARAM_TYPE));

    return i4Ret;
}

INT32 i4AudOmxCompParamPortDef(PARAMETER_OMX_PARAM_PORTDEFINITIONTYPE *prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    PARAMETER_OMX_PARAM_PORTDEFINITIONTYPE rKArg;
    OMX_PARAM_PORTDEFINITIONTYPE rPortDef;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_PARAM_PORTDEFINITIONTYPE));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        i4Ret = copy_from_user(&rPortDef, rKArg.pPortDefinitionType, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

        if (OMX_PARAM_SET == rKArg.eDir)
        {
            // Set Parameter
            SYS_Printf("[AUD][OMX(%d)] set OMX_IndexParamPortDefinition, not processed.\n", pHandle->h_handle);
        }
        else
        {
            // Get Parameter
            //SYS_Printf("[AUD][OMX(%d)] get OMX_IndexParamPortDefinition, part implemented.\n", pHandle->h_handle);

            if (0 == rPortDef.nPortIndex)
            {
                rPortDef.nBufferCountActual = MAX_INPUT_BUF_NO;
                rPortDef.nBufferCountMin = MAX_INPUT_BUF_NO;

                // input port
                switch (_prAudOmxHandle[pHandle->h_handle].eFormat)
                {
                case AUD_FMT_MP3:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingMP3;
                    break;
                case AUD_FMT_AAC:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingAAC;
                    break;
                case AUD_FMT_PCM:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingPCM;
                    break;
                case AUD_FMT_MPEG:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingMPEG;
                    break;
                case AUD_FMT_AC3:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingAC3;
                    break;
                case AUD_FMT_LPCM:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingADPCM;
                    break;
                case AUD_FMT_OMX_MSADPCM:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingADPCM;
                    break;
                case AUD_FMT_OMX_IMAADPCM:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingADPCM;
                    break;
                case AUD_FMT_OMX_ALAW:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingPCM;
                    break;
                case AUD_FMT_OMX_ULAW:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingPCM;
                    break;
                case AUD_FMT_DTS:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingDTS;
                    break;
                case AUD_FMT_WMA:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingWMA;
                    break;
                case AUD_FMT_WMAPRO:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingWMA;
                    break;
                case AUD_FMT_WMA10LOSSLESS:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingWMA;
                    break;
                case AUD_FMT_WMA10SPEECH:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingWMA;
                    break;
                case AUD_FMT_COOK:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingRA;
                    break;
                case AUD_FMT_VORBISDEC:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingVORBIS;
                    break;
                case AUD_FMT_AMR:
                case AUD_FMT_AWB:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingAMR;
                    break;
                case AUD_FMT_DRA:
                    rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingDRA;
                    break;
                default:
                    ASSERT(0);
                    break;
                }

                rPortDef.eDir = OMX_DirInput;
                rPortDef.nBufferSize = MAX_INPUT_BUF_SIZE;
                rPortDef.eDomain = OMX_PortDomainAudio;
                rPortDef.bEnabled = pHandle->fgInputPortEnable;
            }
            else
            {
                rPortDef.nBufferCountActual = MAX_OUTPUT_BUF_NO;
                rPortDef.nBufferCountMin = MAX_OUTPUT_BUF_NO;

                // output port
                rPortDef.format.audio.eEncoding = OMX_AUDIO_CodingPCM;
                rPortDef.eDir = OMX_DirOutput;
                rPortDef.nBufferSize = MAX_OUTPUT_BUF_SIZE;
                rPortDef.eDomain = OMX_PortDomainAudio;
                rPortDef.bEnabled = pHandle->fgOutputPortEnable; 
            }

            i4Ret = copy_to_user(rKArg.pPortDefinitionType, (void *)&rPortDef, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
        }
    }
    else
    {
        omxRet = OMX_ErrorUndefined;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_PARAM_PORTDEFINITIONTYPE));

    return i4Ret;
}

INT32 i4AudOmxCompParamMP3(PARAMETER_OMX_AUDIO_PARAM_MP3TYPE *prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    PARAMETER_OMX_AUDIO_PARAM_MP3TYPE rKArg;
    OMX_AUDIO_PARAM_MP3TYPE rMp3Param;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_MP3TYPE));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        i4Ret = copy_from_user(&rMp3Param, rKArg.pMP3Type, sizeof(OMX_AUDIO_PARAM_MP3TYPE));

        if (OMX_PARAM_SET == rKArg.eDir)
        {
            // Set Parameter
            SYS_Printf("[AUD][OMX(%d)] set MP3 parameters? port = %d (not yet implement)\n", pHandle->h_handle, rMp3Param.nPortIndex);
        }
        else
        {
            // Get Parameter
            SYS_Printf("[AUD][OMX(%d)] get MP3 parameter, port = %d (not yet implement)\n", pHandle->h_handle, rMp3Param.nPortIndex);

            i4Ret = copy_to_user(rKArg.pMP3Type, (void *)&rMp3Param, sizeof(OMX_AUDIO_PARAM_MP3TYPE));
        }
    }
    else
    {
        omxRet = OMX_ErrorUndefined;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_MP3TYPE));

    return i4Ret;
}

INT32 i4AudOmxCompParamAAC(PARAMETER_OMX_AUDIO_PARAM_AACPROFILETYPE *prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    PARAMETER_OMX_AUDIO_PARAM_AACPROFILETYPE rKArg;
    OMX_AUDIO_PARAM_AACPROFILETYPE rAacParam;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_AACPROFILETYPE));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        i4Ret = copy_from_user(&rAacParam, rKArg.pAacProfFileType, sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));

        if (OMX_PARAM_SET == rKArg.eDir)
        {
            // Set Parameter
            if (rAacParam.nChannels)
            {
                _prAudOmxHandle[pHandle->h_handle].u1AacChNo = rAacParam.nChannels;
            }
            else
            {
                _prAudOmxHandle[pHandle->h_handle].u1AacChNo = 2;
            }

            switch (rAacParam.nSampleRate)
            {
                case 96000:
                    _prAudOmxHandle[pHandle->h_handle].u1AacSampleRateIdx = 0;
                    break;
                case 88200:
                    _prAudOmxHandle[pHandle->h_handle].u1AacSampleRateIdx = 1;
                    break;
                case 64000:
                    _prAudOmxHandle[pHandle->h_handle].u1AacSampleRateIdx = 2;
                    break;
                case 48000:
                    _prAudOmxHandle[pHandle->h_handle].u1AacSampleRateIdx = 3;
                    break;
                case 44100:
                    _prAudOmxHandle[pHandle->h_handle].u1AacSampleRateIdx = 4;
                    break;
                case 32000:
                    _prAudOmxHandle[pHandle->h_handle].u1AacSampleRateIdx = 5;
                    break;
                case 24000:
                    _prAudOmxHandle[pHandle->h_handle].u1AacSampleRateIdx = 6;
                    break;
                case 22050:
                    _prAudOmxHandle[pHandle->h_handle].u1AacSampleRateIdx = 7;
                    break;
                case 16000:
                    _prAudOmxHandle[pHandle->h_handle].u1AacSampleRateIdx = 8;
                    break;
                case 12000:
                    _prAudOmxHandle[pHandle->h_handle].u1AacSampleRateIdx = 9;
                    break;
                case 11025:
                    _prAudOmxHandle[pHandle->h_handle].u1AacSampleRateIdx = 10;
                    break;
                case 8000:
                    _prAudOmxHandle[pHandle->h_handle].u1AacSampleRateIdx = 11;
                    break;
                default: //48K
                    _prAudOmxHandle[pHandle->h_handle].u1AacSampleRateIdx = 3;
                    break;
            }
            Printf("Set AAC Type (nChannels: %d/nSampleRate: %d) ChNo: %d/SampleRate: %d, Profile: %d, Frame:%d\n",
                    rAacParam.nChannels,
                    rAacParam.nSampleRate,
                    _prAudOmxHandle[pHandle->h_handle].u1AacChNo, _prAudOmxHandle[pHandle->h_handle].u1AacSampleRateIdx,
                    rAacParam.eAACProfile,
                    rAacParam.nFrameLength);
            if ((rAacParam.eAACProfile !=2) && 
                (rAacParam.eAACProfile !=5) && 
                (rAacParam.eAACProfile !=29) && 
                (rAacParam.eAACProfile !=0))        //in case there is no setting
            {
                Printf("!!!! Warning AAC profile not support !!!!\n");
                _prAudOmxHandle[pHandle->h_handle].fgFormatNotSupport = TRUE;
				vAudOmxNotifyFormatNotSupport(pHandle);
            }
            else if ((rAacParam.nFrameLength !=1024) && 
                        (rAacParam.nFrameLength !=0))       //in case there is no setting
            {
                Printf("!!!! Warning AAC 960 not support !!!!\n");
                _prAudOmxHandle[pHandle->h_handle].fgFormatNotSupport = TRUE;
				vAudOmxNotifyFormatNotSupport(pHandle);				
            }

            if (rAacParam.eAACStreamFormat == OMX_AUDIO_AACStreamFormatMP4ADTS)
            {
                Printf("!!!! Warning AAC in TS !!!!\n");
                _prAudOmxHandle[pHandle->h_handle].fgIsAacInTs = TRUE;
            }           
        }
        else
        {
            // Get Parameter
            SYS_Printf("[AUD][OMX(%d)] get AAC parameter, port = %d (not yet implement)\n", pHandle->h_handle, rAacParam.nPortIndex);

            i4Ret = copy_to_user(rKArg.pAacProfFileType, (void *)&rAacParam, sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
        }
    }
    else
    {
        omxRet = OMX_ErrorUndefined;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_AACPROFILETYPE));

    return i4Ret;
}

INT32 i4AudOmxCompParamPCM(PARAMETER_OMX_AUDIO_PARAM_PCMMODETYPE * prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    PARAMETER_OMX_AUDIO_PARAM_PCMMODETYPE rKArg;
    OMX_AUDIO_PARAM_PCMMODETYPE rPcmParam;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_PCMMODETYPE));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        i4Ret = copy_from_user(&rPcmParam, rKArg.pPcmModeType, sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));

        if (OMX_PARAM_SET == rKArg.eDir)
        {
            // Set Parameter
            Printf("[AUD][OMX(%d)] set PCM parameter: Rate:%d/CH:%d/BITS:%d/Endian:%d/Signed:%d\n", pHandle->h_handle,
                    rPcmParam.nSamplingRate, rPcmParam.nChannels, rPcmParam.nBitPerSample, rPcmParam.eEndian, rPcmParam.eNumData);

            {
                AUD_PCM_SETTING_T rPCMInfo = {0};
                AUD_LPCM_INFO_T rLPCMInfo = {0};

                //eSampleFreq
                switch (rPcmParam.nSamplingRate/1000)
                {
                    case 6:
                        rLPCMInfo.eSampleFreq = FS_6K;
                        break;                        
                    case 8:
                        rLPCMInfo.eSampleFreq = FS_8K;
                        break;
                    case 11:
                        rLPCMInfo.eSampleFreq = FS_11K;
                        break;
                    case 12:
                        rLPCMInfo.eSampleFreq = FS_12K;
                        break;
                    case 16:
                        rLPCMInfo.eSampleFreq = FS_16K;
                        break;
                    case 22:
                        rLPCMInfo.eSampleFreq = FS_22K;
                        break;
                    case 24:
                        rLPCMInfo.eSampleFreq = FS_24K;
                        break;
                    case 32:
                        rLPCMInfo.eSampleFreq = FS_32K;
                        break;
                    case 44:
                        rLPCMInfo.eSampleFreq = FS_44K;
                        break;                        
                    case 48:
                        rLPCMInfo.eSampleFreq = FS_48K;
                        break;
                    case 64:
                        rLPCMInfo.eSampleFreq = FS_64K;
                        break;
                    case 88:
                        rLPCMInfo.eSampleFreq = FS_88K;
                        break;
                    case 96:
                        rLPCMInfo.eSampleFreq = FS_96K;
                        break;
                    case 176:
                        rLPCMInfo.eSampleFreq = FS_176K;
                        break;
                    case 192:
                        rLPCMInfo.eSampleFreq = FS_192K;
                        break;
                    default:
                        rLPCMInfo.eSampleFreq = FS_48K;
                        break;
                }

                rLPCMInfo.ui2_num_ch = rPcmParam.nChannels;

                switch (_prAudOmxHandle[pHandle->h_handle].eFormat)
                {
                    case AUD_FMT_PCM:
                    case AUD_FMT_LPCM:
                        Printf("[AUD][OMX(%d)] AUD_FMT_LPCM\n", pHandle->h_handle);
                        switch (rPcmParam.ePCMMode)
                        {
                        case OMX_AUDIO_PCMModeLinearTs:
                            Printf("[AUD][OMX(%d)] OMX_AUDIO_PCMModeLinearTs\n", pHandle->h_handle); 
                            _prAudOmxHandle[pHandle->h_handle].eFormat = AUD_FMT_OMX_LPCM_TS;
                            break;
                        case OMX_AUDIO_PCMModeLinearPrivate1:
                            Printf("[AUD][OMX(%d)] OMX_AUDIO_PCMModeLinearPrivate1\n", pHandle->h_handle); 
                            _prAudOmxHandle[pHandle->h_handle].eFormat = AUD_FMT_OMX_LPCM_PRIVATE1;
                            break;
                        case OMX_AUDIO_PCMModeLinearTsDvd:
                            Printf("[AUD][OMX(%d)] OMX_AUDIO_PCMModeLinearTsDvd\n", pHandle->h_handle); 
                            _prAudOmxHandle[pHandle->h_handle].eFormat = AUD_FMT_OMX_LPCM_TSDVD;
                            break;
                        default:
                            break; 
                        }
                        rLPCMInfo.eLPcmType = 0;
                        rLPCMInfo.ui2_block_align = 0;
                        rLPCMInfo.ui2_bits_per_sample = rPcmParam.nBitPerSample;
                        rLPCMInfo.b_big_endian = (rPcmParam.eEndian == OMX_EndianBig);
                        rLPCMInfo.b_info_not_specific = 0;
                        rLPCMInfo.b_signed_data = (rPcmParam.eNumData == OMX_NumericalDataSigned);
                        break;
                    case AUD_FMT_OMX_MSADPCM:
                        Printf("[AUD][OMX(%d)] AUD_FMT_OMX_MSADPCM\n", pHandle->h_handle);
                        rLPCMInfo.eLPcmType = LPCM_TYPE_MS_ADPCM;
                        rLPCMInfo.ui2_block_align = 2048; //TODO
                        switch (rPcmParam.nSamplingRate)
                        {
                            case 24000:
                            case 22050:
                            case 16000:
                                rLPCMInfo.ui2_block_align /= 2;
                                break;
                            case 12000:
                            case 11025:
                            case 8000:
                                rLPCMInfo.ui2_block_align /= 4;
                                break;
                        }
                        if (rPcmParam.nChannels == 1)
                        {
                            rLPCMInfo.ui2_block_align /= 2;
                        }
                        rLPCMInfo.ui2_bits_per_sample = 4;
                        rLPCMInfo.b_big_endian = 1;
                        rLPCMInfo.b_info_not_specific = 0;
                        rLPCMInfo.b_signed_data = FALSE;
                        break;
                    case AUD_FMT_OMX_IMAADPCM:
                        Printf("[AUD][OMX(%d)] AUD_FMT_OMX_IMAADPCM\n", pHandle->h_handle);
                        rLPCMInfo.eLPcmType = LPCM_TYPE_IMA_ADPCM;
                        rLPCMInfo.ui2_block_align = 2048; //TODO
                        switch (rPcmParam.nSamplingRate)
                        {
                            case 24000:
                            case 22050:
                            case 16000:
                                rLPCMInfo.ui2_block_align /= 2;
                                break;
                            case 12000:
                            case 11025:
                            case 8000:
                                rLPCMInfo.ui2_block_align /= 4;
                                break;
                        }
                        if (rPcmParam.nChannels == 1)
                        {
                            rLPCMInfo.ui2_block_align /= 2;
                        }

                        rLPCMInfo.ui2_bits_per_sample = 4;
                        rLPCMInfo.b_big_endian = 1;
                        rLPCMInfo.b_info_not_specific = 0;
                        rLPCMInfo.b_signed_data = FALSE;
                        break;
                    case AUD_FMT_OMX_ALAW:
                        Printf("[AUD][OMX(%d)] AUD_FMT_OMX_ALAW\n", pHandle->h_handle);
                        rLPCMInfo.eLPcmType = LPCM_TYPE_A_LAW;
                        if (rPcmParam.nChannels==1)
                        {
                            rLPCMInfo.ui2_block_align = 1;
                        }
                        else
                        {
                            rLPCMInfo.ui2_block_align = 2;
                        }
                        rLPCMInfo.ui2_bits_per_sample = 8;
                        rLPCMInfo.b_big_endian = 1;
                        rLPCMInfo.b_info_not_specific = 0;
                        rLPCMInfo.b_signed_data = FALSE;
                        break;
                    case AUD_FMT_OMX_ULAW:
                        Printf("[AUD][OMX(%d)] AUD_FMT_OMX_ULAW\n", pHandle->h_handle);
                        rLPCMInfo.eLPcmType = LPCM_TYPE_U_LAW;
                        if (rPcmParam.nChannels==1)
                        {
                            rLPCMInfo.ui2_block_align = 1;
                        }
                        else
                        {
                            rLPCMInfo.ui2_block_align = 2;
                        }
                        rLPCMInfo.ui2_bits_per_sample = 8;
                        rLPCMInfo.b_big_endian = 1;
                        rLPCMInfo.b_info_not_specific = 0;
                        rLPCMInfo.b_signed_data = FALSE;
                        break;                   
                    default:
                        break;
                }

                //MTK G-Streamer Proprietary Interface
                if ((((rPcmParam.nVersion.nVersion>>24)&0xff) == 'M') &&
                   (((rPcmParam.nVersion.nVersion>>16)&0xff) == 'T'))
                {
                    Printf("Get MTK GST ui2_block_align interface\n");
                    rLPCMInfo.ui2_block_align = (rPcmParam.nVersion.nVersion&0xffff);
                }
                //End

                rPCMInfo.eSampleFreq = rLPCMInfo.eSampleFreq;
                if (rLPCMInfo.b_big_endian)
                {
                    rPCMInfo.ePcmDataInfo = PCM_16_BITS_BIG_ENDIAN;
                }
                else
                {
                    rPCMInfo.ePcmDataInfo = PCM_16_BITS_LITTLE_ENDIAN;
                }

                Printf("eLPcmType: %d\n", rLPCMInfo.eLPcmType);
                Printf("ui2_num_ch: %d\n", rLPCMInfo.ui2_num_ch);
                Printf("ui2_block_align: %d\n", rLPCMInfo.ui2_block_align);
                Printf("ui2_bits_per_sample: %d\n", rLPCMInfo.ui2_bits_per_sample);
                Printf("b_big_endian: %d\n", rLPCMInfo.b_big_endian);
                Printf("b_info_not_specified: %d\n", rLPCMInfo.b_info_not_specific);
                Printf("b_signed_data: %d\n", rLPCMInfo.b_signed_data);


                AUD_PcmSetting(AUD_DSP0, pHandle->u1DecId, &rPCMInfo);
                AUD_LPcmSetting(pHandle->u1DecId, &rLPCMInfo);
            }
        }
        else
        {
            // Get Parameter
            SYS_Printf("[AUD][OMX(%d)] get PCM parameter, port = %d\n", pHandle->h_handle, rPcmParam.nPortIndex);

            rPcmParam.nChannels = 2;
            rPcmParam.eNumData = OMX_NumericalDataSigned;
            rPcmParam.eEndian = OMX_EndianLittle;
            rPcmParam.bInterleaved = OMX_TRUE;
            rPcmParam.nBitPerSample = 16;
            rPcmParam.nSamplingRate = 48000;
            rPcmParam.ePCMMode = OMX_AUDIO_PCMModeLinear;

            i4Ret = copy_to_user(rKArg.pPcmModeType, (void *)&rPcmParam, sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
        }
    }
    else
    {
        omxRet = OMX_ErrorUndefined;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_PCMMODETYPE));

    return i4Ret;

}

INT32 i4AudOmxCompParamWMA(PARAMETER_OMX_AUDIO_PARAM_WMATYPE *prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    PARAMETER_OMX_AUDIO_PARAM_WMATYPE rKArg;
    OMX_AUDIO_PARAM_WMATYPE rWmaParam;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_WMATYPE));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        i4Ret = copy_from_user(&rWmaParam, rKArg.pWMAType, sizeof(OMX_AUDIO_PARAM_WMATYPE));

        if (OMX_PARAM_SET == rKArg.eDir)
        {
            if (bOmxSettingBypass == 1)
            {
                Printf("[AUD][OMX(%d)] Bypass WMA Setting\n", pHandle->h_handle);
            }
            else
            {
                AUD_WMA_CFG_T rWMAInfo = {0};

                // Set Parameter
                Printf("[AUD][OMX(%d)] set WMA parameters? port = %d (not yet implement)\n", pHandle->h_handle, rWmaParam.nPortIndex);
                Printf("  nBlockAlign: %x\n", rWmaParam.nBlockAlign);
                Printf("  nChannels: %d\n", rWmaParam.nChannels);
                Printf("  nSamplingRate: %d\n", rWmaParam.nSamplingRate);
                Printf("  nBitrate: %d\n", rWmaParam.nBitRate);
                Printf("  nEncodeOptions: %x\n", rWmaParam.nEncodeOptions);

                rWMAInfo.u2HdrSrc = 1;
                rWMAInfo.u4Pktsz = rWmaParam.nBlockAlign;
                rWMAInfo.u2Numch = rWmaParam.nChannels;
                rWMAInfo.u4Sampersec = rWmaParam.nSamplingRate;
                rWMAInfo.u4Bpersec = rWmaParam.nBitRate/8;
                rWMAInfo.u4Blocksz = rWmaParam.nBlockAlign;
                rWMAInfo.u2Encoderopt = rWmaParam.nEncodeOptions;
				/*
                if (rWMAInfo.u2Encoderopt == 0)
                {
                    Printf("  u2Encoderopt not specified use default 0xf\n");
                    rWMAInfo.u2Encoderopt = 0xf;
                }
                */
                AUD_DspWMAHeader(pHandle->u1DecId, &rWMAInfo);
            }
        }
        else
        {
            // Get Parameter
            Printf("[AUD][OMX(%d)] get WMA parameter?, port = %d (not yet implement)\n", pHandle->h_handle, rWmaParam.nPortIndex);
            i4Ret = copy_to_user(rKArg.pWMAType, (void *)&rWmaParam, sizeof(OMX_AUDIO_PARAM_WMATYPE));
        }
    }
    else
    {
        omxRet = OMX_ErrorUndefined;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_WMATYPE));

    return i4Ret;
}

INT32 i4AudOmxCompParamWMAPRO(PARAMETER_OMX_AUDIO_PARAM_WMAPROTYPE *prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    PARAMETER_OMX_AUDIO_PARAM_WMAPROTYPE rKArg;
    OMX_AUDIO_PARAM_WMAPROTYPE rWmaProParam;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_WMAPROTYPE));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        i4Ret = copy_from_user(&rWmaProParam, rKArg.pWMAPROType, sizeof(OMX_AUDIO_PARAM_WMAPROTYPE));

        if (OMX_PARAM_SET == rKArg.eDir)
        {
            if (bOmxSettingBypass == 1)
            {
                Printf("[AUD][OMX(%d)] Bypass WMAPRO Setting\n", pHandle->h_handle);
            }
            else
            {
                AUD_WMA_CFG_T rWMAPROInfo = {0};
                AUD_LPCM_INFO_T rLPCMInfo = {0};

                // Set Parameter
                Printf("[AUD][OMX(%d)] set WMAPRO parameters? port = %d (not yet implement)\n", pHandle->h_handle, rWmaProParam.nPortIndex);
                Printf("  nBlockAlign: %x\n", rWmaProParam.nBlockAlign);
                Printf("  nChannels: %d\n", rWmaProParam.nChannels);
                Printf("  nSamplingRate: %d\n", rWmaProParam.nSamplingRate);
                Printf("  nBitrate: %d\n", rWmaProParam.nBitRate);
                Printf("  nEncodeOptions: %x\n", rWmaProParam.nEncodeOptions);
                Printf("  nChannelMask: %x\n", rWmaProParam.nChannelMask);
                Printf("  nBitsPerSample: %x\n", rWmaProParam.nBitsPerSample);
                Printf("  nAdvEncodeOpt: %x\n", rWmaProParam.nAdvEncodeOpt);
                Printf("  nAdvEncodeOpt2: %x\n", rWmaProParam.nAdvEncodeOpt2);
                Printf("  u4VoiceEncodeOptions1: %x\n", rWmaProParam.nVoiceEncodeOptions1);
                Printf("  u4VoiceEncodeOptions2[0] : %x\n", rWmaProParam.nVoiceEncodeOptions2[0]);
                Printf("  u4VoiceEncodeOptions2[1] : %x\n", rWmaProParam.nVoiceEncodeOptions2[1]);

                if ((rWmaProParam.nChannels > 6) ||
                    (rWmaProParam.nSamplingRate > 48000) ||
                    ((rWmaProParam.nEncodeOptions&0x1) == 0x1) ||
                    ((rWmaProParam.nAdvEncodeOpt>>13)==1) ||
                    ((rWmaProParam.nAdvEncodeOpt>>13)==2) ||
                    ((rWmaProParam.nAdvEncodeOpt>>13)==7))
                {
                    Printf("!!!! Warning WMAPro some format not support !!!!\n");
                    _prAudOmxHandle[pHandle->h_handle].fgFormatNotSupport = TRUE;
					vAudOmxNotifyFormatNotSupport(pHandle);
				}

                rWMAPROInfo.u2HdrSrc = 1;
                rWMAPROInfo.u4Pktsz = rWmaProParam.nBlockAlign;
                rWMAPROInfo.u2Numch = rWmaProParam.nChannels;
                rWMAPROInfo.u4Sampersec = rWmaProParam.nSamplingRate;
                rWMAPROInfo.u4Bpersec = rWmaProParam.nBitRate/8;
                rWMAPROInfo.u4Blocksz = rWmaProParam.nBlockAlign;
                rWMAPROInfo.u2Encoderopt = rWmaProParam.nEncodeOptions;
                rWMAPROInfo.u4ChannelMask = rWmaProParam.nChannelMask;
                rWMAPROInfo.u2BitsPerSample = rWmaProParam.nBitsPerSample;
                rWMAPROInfo.u2AdvEncoderopt = rWmaProParam.nAdvEncodeOpt;
                rWMAPROInfo.u4AdvEncoderopt2= rWmaProParam.nAdvEncodeOpt2;
                //for WMA Voice
                rWMAPROInfo.u4VoiceEncodeOptions1= rWmaProParam.nVoiceEncodeOptions1;
                rWMAPROInfo.u4VoiceEncodeOptions2[0] = rWmaProParam.nVoiceEncodeOptions2[0];
                rWMAPROInfo.u4VoiceEncodeOptions2[1] = rWmaProParam.nVoiceEncodeOptions2[1];
                if (_prAudOmxHandle[pHandle->h_handle].eFormat == AUD_FMT_WMAPRO)
                {
                    AUD_DspWMAProHeader(pHandle->u1DecId, &rWMAPROInfo);
                }
                else if ((_prAudOmxHandle[pHandle->h_handle].eFormat == AUD_FMT_WMA10LOSSLESS) ||
                             (_prAudOmxHandle[pHandle->h_handle].eFormat == AUD_FMT_WMA10SPEECH))
                {
                    //RISC decoder, set LPCM
                    if (rWmaProParam.nSamplingRate == 44100)
                    {
                        rLPCMInfo.eSampleFreq = FS_44K;
                    }
                    else if (rWmaProParam.nSamplingRate == 48000)
                    {
                        rLPCMInfo.eSampleFreq = FS_48K;
                    }
                    else if (rWmaProParam.nSamplingRate == 88200)
                    {
                        rLPCMInfo.eSampleFreq = FS_88K;
                    }
                    else if (rWmaProParam.nSamplingRate == 96000)
                    {
                        rLPCMInfo.eSampleFreq = FS_96K;
                    }
                    else if (rWmaProParam.nSamplingRate == 8000)
                    {
                        rLPCMInfo.eSampleFreq = FS_8K;
                    }
                    else if (rWmaProParam.nSamplingRate == 11025)
                    {
                        rLPCMInfo.eSampleFreq = FS_11K;
                    }
                    else if (rWmaProParam.nSamplingRate == 16000)
                    {
                        rLPCMInfo.eSampleFreq = FS_16K;
                    }
                    else if (rWmaProParam.nSamplingRate == 22050)
                    {
                        rLPCMInfo.eSampleFreq = FS_22K;
                    }
                    else
                    {
                         omxRet = OMX_ErrorUndefined;
                    }
                    rLPCMInfo.eLPcmType = 0;
                    rLPCMInfo.ui2_num_ch = rWmaProParam.nChannels;
                    rLPCMInfo.ui2_block_align = 0;
                    rLPCMInfo.ui2_bits_per_sample = 16;
                    rLPCMInfo.b_big_endian = 0;
                    rLPCMInfo.b_info_not_specific = 0;
                    rLPCMInfo.b_signed_data = 0 ;
                    AUD_LPcmSetting(pHandle->u1DecId,&rLPCMInfo);
                }
                else
                {
                    omxRet = OMX_ErrorUndefined;
                }
            }
        }
        else
        {
            // Get Parameter
            Printf("[AUD][OMX(%d)] get WMAPRO parameter?, port = %d (not yet implement)\n", pHandle->h_handle, rWmaProParam.nPortIndex);
            i4Ret = copy_to_user(rKArg.pWMAPROType, (void *)&rWmaProParam, sizeof(OMX_AUDIO_PARAM_WMAPROTYPE));
        }
    }
    else
    {
        omxRet = OMX_ErrorUndefined;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_WMATYPE));

    return i4Ret;
}

INT32 i4AudOmxCompParamRA(PARAMETER_OMX_AUDIO_PARAM_RATYPE *prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    PARAMETER_OMX_AUDIO_PARAM_RATYPE rKArg;
    OMX_AUDIO_PARAM_RATYPE rRaParam;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_RATYPE));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        i4Ret = copy_from_user(&rRaParam, rKArg.pRAType, sizeof(OMX_AUDIO_PARAM_RATYPE));

        if (OMX_PARAM_SET == rKArg.eDir)
        {
            // Set Parameter
            if (bOmxSettingBypass == 1)
            {
                Printf("[AUD][OMX(%d)] Bypass COOK Setting\n", pHandle->h_handle);
            }
            else
            {
                AUD_COOK_CFG_T rCookCfg = {0};

                Printf("[AUD][OMX(%d)] set RA parameters? port = %d\n", pHandle->h_handle, rRaParam.nPortIndex);

                rCookCfg.u2SampPerFrame = rRaParam.nSamplePerFrame;
                rCookCfg.u2ChanNum = _prAudOmxHandle[pHandle->h_handle].u1RaChanNum = rRaParam.nChannels;
                rCookCfg.u2SampRate = rRaParam.nSamplingRate;
                rCookCfg.u2FrameSzInBytes = _prAudOmxHandle[pHandle->h_handle].u2RaFrameSizeInBytes = rRaParam.nBitsPerFrame/8;
                rCookCfg.u2RegnNum = rRaParam.nNumRegions;
                rCookCfg.u2StartRegn = rRaParam.nCouplingStartRegion;
                rCookCfg.u2CplQBits = _prAudOmxHandle[pHandle->h_handle].u2RaCplQBits = rRaParam.nCouplingQuantBits;
                Printf("u2SampPerFrame: %d\n", rCookCfg.u2SampPerFrame);
                Printf("u2ChanNum: %d\n", rCookCfg.u2ChanNum);
                Printf("u2SampRate: %d\n", rCookCfg.u2SampRate);
                Printf("u2FrameSzInBytes: %d\n", rCookCfg.u2FrameSzInBytes);
                Printf("u2RegnNum: %d\n", rCookCfg.u2RegnNum);
                Printf("u2StartRegn: %d\n", rCookCfg.u2StartRegn);
                Printf("u2CplQBits: %d\n", rCookCfg.u2CplQBits);
                AUD_DspCOOKParameter(pHandle->u1DecId, &rCookCfg);
            }
        }
        else
        {
            // Get Parameter
            Printf("[AUD][OMX(%d)] get RA parameter?, port = %d (not yet implement)\n", pHandle->h_handle, rRaParam.nPortIndex);
            i4Ret = copy_to_user(rKArg.pRAType, (void *)&rRaParam, sizeof(OMX_AUDIO_PARAM_RATYPE));
        }
    }
    else
    {
        omxRet = OMX_ErrorUndefined;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_RATYPE));

    return i4Ret;
}

INT32 i4AudOmxCompParamADPCM(PARAMETER_OMX_AUDIO_PARAM_ADPCMTYPE * prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    PARAMETER_OMX_AUDIO_PARAM_ADPCMTYPE rKArg = {0};
    OMX_AUDIO_PARAM_ADPCMTYPE rAdpcmParam = {0};
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_ADPCMTYPE));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        i4Ret = copy_from_user(&rAdpcmParam, rKArg.pADPCMType, sizeof(OMX_AUDIO_PARAM_ADPCMTYPE));

        if (OMX_PARAM_SET == rKArg.eDir)
        {
            // Set Parameter
            Printf("[AUD][OMX(%d)] set ADPCM parameter: Rate:%d/CH:%d\n", pHandle->h_handle,
                    rAdpcmParam.nSampleRate, rAdpcmParam.nChannels);

            {
                AUD_PCM_SETTING_T rPCMInfo = {0};
                AUD_LPCM_INFO_T rLPCMInfo = {0};

                //eSampleFreq
                switch (rAdpcmParam.nSampleRate/1000)
                {
                     case 48:
                         rLPCMInfo.eSampleFreq = FS_48K;
                         break;
                     case 32:
                         rLPCMInfo.eSampleFreq = FS_32K;
                         break;
                     case 44:
                         rLPCMInfo.eSampleFreq = FS_44K;
                         break;
                     case 8:
                         rLPCMInfo.eSampleFreq = FS_8K;
                         break;
                     case 11:
                         rLPCMInfo.eSampleFreq = FS_11K;
                         break;
                     case 12:
                         rLPCMInfo.eSampleFreq = FS_12K;
                         break;
                    case 16:
                         rLPCMInfo.eSampleFreq = FS_16K;
                         break;
                     case 22:
                         rLPCMInfo.eSampleFreq = FS_22K;
                         break;
                     case 24:
                         rLPCMInfo.eSampleFreq = FS_24K;
                         break;
                     default:
                         rLPCMInfo.eSampleFreq = FS_48K;
                         break;
                }

                rLPCMInfo.ui2_num_ch = rAdpcmParam.nChannels;

                switch (_prAudOmxHandle[pHandle->h_handle].eFormat)
                {
                    case AUD_FMT_LPCM:
                        Printf("[AUD][OMX(%d)] AUD_FMT_LPCM\n", pHandle->h_handle);
                        rLPCMInfo.eLPcmType = LPCM_TYPE_NORAML;
                        if (rAdpcmParam.nChannels==1)
                        {
                            rLPCMInfo.ui2_block_align = 2;
                        }
                        else
                        {
                            rLPCMInfo.ui2_block_align = 4;
                        }
                        rLPCMInfo.ui2_bits_per_sample = 16;
                        rLPCMInfo.b_big_endian = 1;
                        rLPCMInfo.b_info_not_specific = 0;
                        rLPCMInfo.b_signed_data = FALSE;
                        break;
                    case AUD_FMT_OMX_MSADPCM:
                        Printf("[AUD%d][OMX] AUD_FMT_OMX_MSADPCM\n", pHandle->h_handle);
                        rLPCMInfo.eLPcmType = LPCM_TYPE_MS_ADPCM;
                        rLPCMInfo.ui2_block_align = 2048; //TODO
                        switch (rAdpcmParam.nSampleRate)
                        {
                            case 24000:
                            case 22050:
                            case 16000:
                                rLPCMInfo.ui2_block_align /= 2;
                                break;
                            case 12000:
                            case 11025:
                            case 8000:
                                rLPCMInfo.ui2_block_align /= 4;
                                break;
                        }
                        if (rAdpcmParam.nChannels == 1)
                        {
                            rLPCMInfo.ui2_block_align /= 2;
                        }
                        rLPCMInfo.ui2_bits_per_sample = 4;
                        rLPCMInfo.b_big_endian = 1;
                        rLPCMInfo.b_info_not_specific = 0;
                        rLPCMInfo.b_signed_data = FALSE;
                        break;
                    case AUD_FMT_OMX_IMAADPCM:
                        Printf("[AUD][OMX(%d)] AUD_FMT_OMX_IMAADPCM\n", pHandle->h_handle);
                        rLPCMInfo.eLPcmType = LPCM_TYPE_IMA_ADPCM;
                        rLPCMInfo.ui2_block_align = 2048; //TODO
                        switch (rAdpcmParam.nSampleRate)
                        {
                            case 24000:
                            case 22050:
                            case 16000:
                                rLPCMInfo.ui2_block_align /= 2;
                                break;
                            case 12000:
                            case 11025:
                            case 8000:
                                rLPCMInfo.ui2_block_align /= 4;
                                break;
                        }
                        if (rAdpcmParam.nChannels == 1)
                        {
                            rLPCMInfo.ui2_block_align /= 2;
                        }
                        rLPCMInfo.ui2_bits_per_sample = 4;
                        rLPCMInfo.b_big_endian = 1;
                        rLPCMInfo.b_info_not_specific = 0;
                        rLPCMInfo.b_signed_data = FALSE;
                        break;
                    case AUD_FMT_OMX_ALAW:
                        Printf("[AUD][OMX(%d)] AUD_FMT_OMX_ALAW\n", pHandle->h_handle);
                        rLPCMInfo.eLPcmType = LPCM_TYPE_A_LAW;
                        if (rAdpcmParam.nChannels==1)
                        {
                            rLPCMInfo.ui2_block_align = 1;
                        }
                        else
                        {
                            rLPCMInfo.ui2_block_align = 2;
                        }
                        rLPCMInfo.ui2_bits_per_sample = 8;
                        rLPCMInfo.b_big_endian = 1;
                        rLPCMInfo.b_info_not_specific = 0;
                        rLPCMInfo.b_signed_data = FALSE;
                        break;
                    case AUD_FMT_OMX_ULAW:
                        Printf("[AUD][OMX(%d)] AUD_FMT_OMX_ULAW\n", pHandle->h_handle);
                        rLPCMInfo.eLPcmType = LPCM_TYPE_U_LAW;
                        if (rAdpcmParam.nChannels==1)
                        {
                            rLPCMInfo.ui2_block_align = 1;
                        }
                        else
                        {
                            rLPCMInfo.ui2_block_align = 2;
                        }
                        rLPCMInfo.ui2_bits_per_sample = 8;
                        rLPCMInfo.b_big_endian = 1;
                        rLPCMInfo.b_info_not_specific = 0;
                        rLPCMInfo.b_signed_data = FALSE;
                        break;
                    default:
                        break;
                }

                //MTK G-Streamer Proprietary Interface
                if ((((rAdpcmParam.nVersion.nVersion>>24)&0xff) == 'M') &&
                   (((rAdpcmParam.nVersion.nVersion>>16)&0xff) == 'T'))
                {
                    Printf("Get MTK GST ui2_block_align interface\n");
                    rLPCMInfo.ui2_block_align = (rAdpcmParam.nVersion.nVersion&0xffff);
                }
                //End

                rPCMInfo.eSampleFreq = rLPCMInfo.eSampleFreq;
                if (rLPCMInfo.b_big_endian)
                {
                    rPCMInfo.ePcmDataInfo = PCM_16_BITS_BIG_ENDIAN;
                }
                else
                {
                    rPCMInfo.ePcmDataInfo = PCM_16_BITS_LITTLE_ENDIAN;
                }

                Printf("eLPcmType: %d\n", rLPCMInfo.eLPcmType);
                Printf("ui2_num_ch: %d\n", rLPCMInfo.ui2_num_ch);
                Printf("ui2_block_align: %d\n", rLPCMInfo.ui2_block_align);
                Printf("ui2_bits_per_sample: %d\n", rLPCMInfo.ui2_bits_per_sample);
                Printf("b_big_endian: %d\n", rLPCMInfo.b_big_endian);
                Printf("b_info_not_specified: %d\n", rLPCMInfo.b_info_not_specific);
                Printf("b_signed_data: %d\n", rLPCMInfo.b_signed_data);

                AUD_PcmSetting(AUD_DSP0, pHandle->u1DecId, &rPCMInfo);
                AUD_LPcmSetting(pHandle->u1DecId, &rLPCMInfo);
            }
        }
        else
        {
            // Get Parameter
            Printf("[AUD][OMX(%d)] get ADPCM parameter?, port = %d (strange)\n", pHandle->h_handle, rAdpcmParam.nPortIndex);
            rAdpcmParam.nChannels = 2;
            rAdpcmParam.nSampleRate = 48000;
            rAdpcmParam.nBitsPerSample = 16;
            i4Ret = copy_to_user(rKArg.pADPCMType, (void *)&rAdpcmParam, sizeof(OMX_AUDIO_PARAM_ADPCMTYPE));
        }
    }
    else
    {
        omxRet = OMX_ErrorUndefined;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_ADPCMTYPE));

    return i4Ret;

}

INT32 i4AudOmxCompParamVORBIS(PARAMETER_OMX_AUDIO_PARAM_VORBISTYPE *prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    PARAMETER_OMX_AUDIO_PARAM_VORBISTYPE rKArg;
    OMX_AUDIO_PARAM_VORBISTYPE rVorbisParam;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_VORBISTYPE));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        i4Ret = copy_from_user(&rVorbisParam, rKArg.pVORBISType, sizeof(OMX_AUDIO_PARAM_VORBISTYPE));

        if (OMX_PARAM_SET == rKArg.eDir)
        {
            // Set Parameter
            Printf("[AUD][OMX(%d)] set VORBIS parameters port = %d (not yet implement)\n", pHandle->h_handle, rVorbisParam.nPortIndex);
        }
        else
        {
            // Get Parameter
            Printf("[AUD][OMX(%d)] get VORBIS parameter?, port = %d (not yet implement)\n", pHandle->h_handle, rVorbisParam.nPortIndex);
            i4Ret = copy_to_user(rKArg.pVORBISType, (void *)&rVorbisParam, sizeof(OMX_AUDIO_PARAM_VORBISTYPE));
        }
    }
    else
    {
        omxRet = OMX_ErrorUndefined;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_VORBISTYPE));

    return i4Ret;
}

INT32 i4AudOmxCompParamAMR(PARAMETER_OMX_AUDIO_PARAM_AMRTYPE *prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    PARAMETER_OMX_AUDIO_PARAM_AMRTYPE rKArg;
    OMX_AUDIO_PARAM_AMRTYPE rAmrParam;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_AMRTYPE));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        i4Ret = copy_from_user(&rAmrParam, rKArg.pAMRType, sizeof(OMX_AUDIO_PARAM_AMRTYPE));

        if (OMX_PARAM_SET == rKArg.eDir)
        {
            Printf("[AUD][OMX(%d)] set AMR parameters port = %d (not yet implement)\n", pHandle->h_handle, rAmrParam.nPortIndex);
        }
        else
        {
            // Get Parameter
            Printf("[AUD][OMX(%d)] get AMR parameter?, port = %d (not yet implement)\n", pHandle->h_handle, rAmrParam.nPortIndex);
            i4Ret = copy_to_user(rKArg.pAMRType, (void *)&rAmrParam, sizeof(OMX_AUDIO_PARAM_AMRTYPE));
        }
    }
    else
    {
        omxRet = OMX_ErrorUndefined;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_AMRTYPE));

    return i4Ret;
}

INT32 i4AudOmxCompParamDecodeSize(PARAMETER_OMX_AUDIO_PARAM_DECODESIZE *prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    PARAMETER_OMX_AUDIO_PARAM_DECODESIZE rKArg;
    OMX_AUDIO_PARAM_DECODESIZE rDecodeSize;

	#if (AUD_COMP_MAX == 1)
    static UINT32 u4PrevRP[AUD_COMP_MAX] = {0};
    static UINT32 u4PrevWP[AUD_COMP_MAX] = {0};
    static UINT32 u4Cnt[AUD_COMP_MAX] = {0};
	#else
	static UINT32 u4PrevRP[AUD_COMP_MAX] = {0,0};
    static UINT32 u4PrevWP[AUD_COMP_MAX] = {0,0};
    static UINT32 u4Cnt[AUD_COMP_MAX] = {0,0};
	#endif
	
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_DECODESIZE));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        i4Ret = copy_from_user(&rDecodeSize, rKArg.pDecodeSize, sizeof(OMX_AUDIO_PARAM_DECODESIZE));

        if (OMX_PARAM_SET == rKArg.eDir)
        {
            omxRet = OMX_ErrorUndefined;
        }
        else
        {
            UINT32 rp,wp,size;

            {
                UINT32 sa,ea;

                sa = (UINT32)_prAudOmxHandle[pHandle->h_handle].pFifoPntr;
                ea = (UINT32)_prAudOmxHandle[pHandle->h_handle].pFifoPntr + _prAudOmxHandle[pHandle->h_handle].u4FifoSize;
                wp = (UINT32)_prAudOmxHandle[pHandle->h_handle].pFifoWptr;
                rp = (UINT32)VIRTUAL(u4GetABufPnt(pHandle->u1DecId));

                size = ((wp >= rp) ? (wp - rp) : (ea - sa + wp - rp));
                x_thread_delay(5); //For cinemanow case on r30255, if no delay, there will not be audio render trigger, Don't know why?
            }
            
            if (((rp == u4PrevRP[pHandle->h_handle]) && (wp == u4PrevWP[pHandle->h_handle])) && (_prAudOmxHandle[pHandle->h_handle].eState == OMX_StateExecuting) && (size < 2048))
            {
                u4Cnt[pHandle->h_handle]++;
            }
            else
            {
                u4Cnt[pHandle->h_handle] = 0;
            }
            u4PrevRP[pHandle->h_handle] = rp;
            u4PrevWP[pHandle->h_handle] = wp;

            if (u4Cnt[pHandle->h_handle] >= 4)
            {
                size = 0;
                if (u4AOmxDbgMsk & 0x20)
                {                
                    Printf("[AUD][OMX(%d)][DecodeSize] decoder rp/wp not change over than 5 times. size reset to 0\n", pHandle->h_handle);
                }
            }

            if (size > _prAudOmxHandle[pHandle->h_handle].u4TotalInputSize)
                size = _prAudOmxHandle[pHandle->h_handle].u4TotalInputSize;

            if (u4AOmxDbgMsk & 0x20)
            {
                Printf("u4TotalInputSize: %d size: %d rp: %x wp: %x\n", _prAudOmxHandle[pHandle->h_handle].u4TotalInputSize, size, rp,wp);
            }

            rDecodeSize.nDecodedSize = _prAudOmxHandle[pHandle->h_handle].u4TotalInputSize - size;
            rDecodeSize.nUndecodedSize = size;
            
            // Get Parameter
            //Printf("[AUD][OMX(%d)] i4AudOmxCompParamDecodeSize, port = %d\n", pHandle->h_handle, rDecodeSize.nPortIndex);
            i4Ret = copy_to_user(rKArg.pDecodeSize, (void *)&rDecodeSize, sizeof(OMX_AUDIO_PARAM_DECODESIZE));
        }
    }
    else
    {
        omxRet = OMX_ErrorUndefined;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_DECODESIZE));

    return i4Ret;
}

INT32 i4AudOmxCompParamDecoderId(PARAMETER_OMX_AUDIO_PARAM_DECODEID *prUArg)
{
    INT32 i4Ret = 0;
    OMX_ERRORTYPE omxRet = OMX_ErrorNone;
    PARAMETER_OMX_AUDIO_PARAM_DECODEID rKArg;
    OMX_AUDIO_PARAM_DECODEID rDecodeId;
    AUD_OMX_HANDLE_T *pHandle;

    i4Ret = copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_DECODEID));

    pHandle = rKArg.pHandle;
    if (pHandle->fgUsed && (rKArg.pHandle == &_prAudOmxHandle[pHandle->h_handle]))
    {
        i4Ret = copy_from_user(&rDecodeId, rKArg.pDecoderId, sizeof(OMX_AUDIO_PARAM_DECODEID));

        if (OMX_PARAM_SET == rKArg.eDir)
        {
            pHandle->u1DecId = rDecodeId.nDecoderId;
            Printf("[AUD][OMX(%d)][DecoderId][SET] AUD_DEC%d\n", pHandle->h_handle, rDecodeId.nDecoderId); 
        }
        else
        {
            rDecodeId.nDecoderId = pHandle->u1DecId;
            i4Ret = copy_to_user(rKArg.pDecoderId, &rDecodeId, sizeof(OMX_AUDIO_PARAM_DECODEID)); 
        }
    }
    else
    {
        omxRet = OMX_ErrorUndefined;
    }

    rKArg.eReturnValue = omxRet;
    i4Ret = copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_AUDIO_PARAM_DECODEID));

    return i4Ret;
}

void vAudOmxNotifyFormatNotSupport(AUD_OMX_HANDLE_T *pHandle)
{
	PARAMETER_OMX_CB_EVENTHANDLER rEvent;
		                
	rEvent.eEvent     = OMX_EventError;
	rEvent.nData1     = OMX_ErrorFormatNotDetected;
	rEvent.nData2     = 0;
	rEvent.pEventData = NULL;
	i4AudOmxCompNotifyEvent(pHandle, &rEvent);	

	Printf("[AUD][OMX(%d)] !!!! Notify Format Not Support !!!!\n", pHandle->h_handle);
}

#endif

