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

#ifndef _AUD_OMX_H_
#define _AUD_OMX_H_

#include "OMX_Core.h"
#include "OMX_Component.h"
#include "OMX_CoreExt.h"

#include "openmaxil_ioctl.h"

#define AUD_COMP_MAX 2 // 2 is only tested by UT, also many DSP context swtich conflicts need to be solved (MP3 + AAC NG)

#define AOMX_REAL_PTS   //20111117 koro
#define AOMX_CMD_QUEUE  //20120110 daniel
  #define AOMX_CMD_QUEUE_SIZE 128

//The current input buf limitation is 8K*8 = 64KB
#define MAX_INPUT_BUF_SIZE 8192
#define MAX_INPUT_BUF_NO 8
//The current output buf limitation is 32K*4 = 128KB
#define MAX_OUTPUT_BUF_SIZE 8192
#define MAX_OUTPUT_BUF_NO 8

//#define INPUT_BUF_MAX  (MAX_INPUT_BUF_NO * AUD_COMP_MAX)
//#define OUTPUT_BUF_MAX (MAX_OUTPUT_BUF_NO * AUD_COMP_MAX)
#define INPUT_BUF_MAX  (MAX_INPUT_BUF_NO)
#define OUTPUT_BUF_MAX (MAX_OUTPUT_BUF_NO)


typedef struct _AUD_OMX_BUFFER_T AUD_OMX_BUFFER_T;

typedef enum {
    AUD_OMX_CMD_SET_STATE,
    AUD_OMX_CMD_EMPTY_BUFFER,
    AUD_OMX_CMD_FILL_BUFFER,
    AUD_OMX_CMD_FLUSH,
    AUD_OMX_CMD_PORT_ENABLE,
    AUD_OMX_CMD_PORT_DISABLE,
    AUD_OMX_CMD_BUFFER_ALLOCATED,
    AUD_OMX_CMD_BUFFER_FREED,
    AUD_OMX_CMD_TERMINATE,
    AUD_OMX_CMD_MAX = 0x7FFFFFFF,
} AUD_OMX_CMD_T;

typedef struct {
    AUD_OMX_CMD_T     eCmd;
    OMX_STATETYPE     eStateTo; // for AUD_OMX_CMD_SET_STATE
    HANDLE_T          hHeader;  // for AUD_OMX_CMD_EMPTY_BUFFER & AUD_OMX_CMD_FILL_BUFFER
    BOOL              fgInput;  // for AUD_OMX_CMD_FLUSH
    BOOL              fgOutput; // for AUD_OMX_CMD_FLUSH
} AUD_OMX_MSG_T;

typedef struct {
    HANDLE_T          h_handle;
	UINT8             u1DecId;
	BOOL              fgEncFlag;
    BOOL              fgUsed;
    BOOL              fgEmptying;
    BOOL              fgFilling;
    BOOL              fgStateTransing; //TO_DEL
    BOOL              fgGotEOS;
    UINT32            pid;
    HANDLE_T          hMainTask;
#ifndef AOMX_CMD_QUEUE
    HANDLE_T          hCommandQ;
#endif
    AUD_OMX_BUFFER_T *pEmptyQ[INPUT_BUF_MAX]; // always left aligned, -> 11OO0000
    AUD_OMX_BUFFER_T *pFillQ[OUTPUT_BUF_MAX];
    AUD_FMT_T         eFormat;
    OMX_STATETYPE     eState;
    OMX_STATETYPE     eStateTo;
    OMX_PTR           pAppData;
    MTVDEC_PFN_OMX_IL_CB  pEventHandler;
    MTVDEC_PFN_OMX_IL_CB  pEmptyBufferDone;
    MTVDEC_PFN_OMX_IL_CB  pFillBufferDone;
    void             *pFifoPntr;  // AFIFO SA for decoder
    UINT32            u4FifoSize; // AFIFO length for decoder
    void             *pFifoWptr;  // AFIFO Wptr for decoder

    ////////////////////////////////////////
    UINT32 u4AomxThreadCnt;
    UINT32 u4AomxThreadState;
    
    UINT32 u4CmdQSendNum;
    UINT32 u4CmdQRecvNum;
#ifdef AOMX_CMD_QUEUE
    UINT32 u4MsgReadIdx;
    UINT32 u4MsgWriteIdx;
    AUD_OMX_MSG_T rMsgQ[128];
#endif

    UINT32 u4FillQueueNo;
    UINT32 u4EmptyQueueNo;
    UINT32 u4TotalFillQueueDoneNo;
    UINT32 u4TotalEmptyQueueDoneNo;
    UINT32 u4TotalFillQueueNo;
    UINT32 u4TotalEmptyQueueNo;
    UINT32 u4TotalEmptySize;
    UINT32 u4TotalInputSize;

    ////////////////////////////////////////
    // PTS Information
    OMX_TICKS nInputTimeStampLast; // = -1;
    OMX_TICKS nOutputTimeStampCur; // = -1;
    OMX_TICKS nOutputTimeStampLast; // = -1;
    UINT32 u4BankNumPrev; // = 0;   //previous aput_bank
    INT32 i4PTSBankNumPrev; // = 0;    //aput_bank of previous fake PTS refresh
#ifdef AOMX_REAL_PTS                    
    UINT64 u8LastRealPts; // = -1;
#endif    

    ////////////////////////////////////////
    // Status Bit    
    BOOL fgAoutEnable;
    BOOL fgRenderTrigger;
    BOOL fgPauseTrigger;    
    BOOL fgFormatNotSupport;
    BOOL fgIsAacInTs;
    BOOL fgIsFirstChk;
    BOOL fgEnableAdtsInsert;
    BOOL fgGotValidPts;
    OMX_BOOL fgInputPortEnable;
    OMX_BOOL fgOutputPortEnable;

    ////////////////////////////////////////
    // Decoder Interface
    //Header Insertion
    UINT32 u4AudHdrOffset;
    UINT32 u4AudHdrSize;

    //AAC
    UINT8 u1AacChNo; //Default: 2
    UINT8 u1AacSampleRateIdx; //Default: 3 //48K
    UINT8 u1AAC_ADTS[7+1]; // 1 for alignment

    //RA    
    UINT16 u2RaFrameByteIdx; //Default: 0
    UINT16 u2RaFrameSizeInBytes; //Default: 0x80
    UINT16 u2RaCplQBits;		// Default: 0
    UINT8  u1RaChanNum; 		// Default: 2

    //VORBIS
    UINT8 u1VORBIS_OggS[27+255];    ///[255] for Segment Table.
    UINT32 u4VorbisPage;    

	//Codec Data
	void 	*pCodecData;
	UINT32	u4CodecDataSize;

    ////////////////////////////////////////
    //ERROR_CNT_MONITOR
    struct timeval error_cnt_timer; //default {0}

    ////////////////////////////////////////
    //UNDERRUN_CNT_MONITOR
    struct timeval underrun_cnt_timer;
    UINT32 u4PrevInputUnderrunCnt; //default 0;
    UINT32 u4InputUnderrunDisableTimer; //default INPUT_UNDERRUN_DISABLE_TIMER;    
} AUD_OMX_HANDLE_T;

struct _AUD_OMX_BUFFER_T {
    AUD_OMX_HANDLE_T    *pOwner;
    HANDLE_T             hBuffer; // handle for below buffer
    void                *pBufSA;
    UINT32               u4BufSize;
    UINT32               u4FifoWp; // corresponding Wptr of AFIFO
    UINT32               u4FifoRp; // corresponding Rptr of AFIFO when finish copying this buffer to AFIFO
    UINT32               u4FifoFreezeCnt; // FIFO freeze (Rptr not changed) count
    HANDLE_T             hHeader; // handle for below header structure
    UINT32               u4FilledLen; // used on empty, for counting down filled length, not duplicate with header
    UINT32               u4Offset;    // used on empty, for counting down filled length, not duplicate with header
    BOOL                 fgEmptied;   // TRUE: this buffer has been emptied (if orginial size = 0, means has been processed)
                                      // FALSE: has not been processed (just queued)
    BOOL				 fgCodecData; // TRUE if this buffer is codec data
    OMX_BUFFERHEADERTYPE rHeader; // client will read/write this area
    OMX_TICKS            nTimeStamp;    
    UINT32               u4Reserved; // for error detection
};

INT32 i4AudOmxTaskCreate(AUD_OMX_HANDLE_T *pHandle);
INT32 i4AudOmxTaskDestroy(AUD_OMX_HANDLE_T *pHandle);

INT32 i4AudOmxTaskSendCommand(AUD_OMX_HANDLE_T *pHandle, AUD_OMX_MSG_T *prMsg);

INT32 i4AudOmxTaskSemInit(void);
INT32 i4AudOmxTaskSemUninit(void);
INT32 i4AudOmxTaskLockSem(void);
INT32 i4AudOmxTaskUnlockSem(void);

INT32 i4AudOmxCmdSemInit(void);
INT32 i4AudOmxCmdSemUninit(void);
INT32 i4AudOmxCmdLockSem(void);
INT32 i4AudOmxCmdUnlockSem(void);

INT32 i4AudOmxCompInit(void);
INT32 i4AudOmxCompUninit(void);
INT32 i4AudOmxCompGetHandle(PARAMETER_OMX_GETHANDLE *prUArg, PARAMETER_OMX_CB *prOmxCb);
INT32 i4AudOmxCompFreeHandle(PARAMETER_OMX_FREEHANDLE *prUArg);

INT32 i4AudOmxCompGetState(PARAMETER_OMX_GETSTATE *prUArg);
INT32 i4AudOmxCompSetStateAsync(PARAMETER_OMX_COMMAND_STATESET *prUArg);
INT32 i4AudOmxCompFlushAsync(PARAMETER_OMX_COMMAND_PORT *prUArg);

INT32 i4AudOmxCompCheckStateTransition(AUD_OMX_HANDLE_T *pHandle);
INT32 i4AudOmxCompSetState(AUD_OMX_HANDLE_T *pHandle, OMX_STATETYPE omxStateTo);
INT32 i4AudOmxCompFlush(AUD_OMX_HANDLE_T *pHandle, BOOL fgInput, BOOL fgOutput);

INT32 i4AudOmxCompGetParamAudioInit(PARAMETER_OMX_PORT_PARAM_TYPE *prUArg);
INT32 i4AudOmxCompParamPortDef(PARAMETER_OMX_PARAM_PORTDEFINITIONTYPE *prUArg);

INT32 i4AudOmxCompParamMP3(PARAMETER_OMX_AUDIO_PARAM_MP3TYPE *prUArg);
INT32 i4AudOmxCompParamAAC(PARAMETER_OMX_AUDIO_PARAM_AACPROFILETYPE *prUArg);
INT32 i4AudOmxCompParamPCM(PARAMETER_OMX_AUDIO_PARAM_PCMMODETYPE * prUArg);
INT32 i4AudOmxCompParamWMA(PARAMETER_OMX_AUDIO_PARAM_WMATYPE *prUArg);
INT32 i4AudOmxCompParamWMAPRO(PARAMETER_OMX_AUDIO_PARAM_WMAPROTYPE *prUArg);
INT32 i4AudOmxCompParamRA(PARAMETER_OMX_AUDIO_PARAM_RATYPE *prUArg);
INT32 i4AudOmxCompParamADPCM(PARAMETER_OMX_AUDIO_PARAM_ADPCMTYPE *prUArg);
INT32 i4AudOmxCompParamVORBIS(PARAMETER_OMX_AUDIO_PARAM_VORBISTYPE *prUArg);
INT32 i4AudOmxCompParamAMR(PARAMETER_OMX_AUDIO_PARAM_AMRTYPE *prUArg);
INT32 i4AudOmxCompParamDecodeSize(PARAMETER_OMX_AUDIO_PARAM_DECODESIZE *prUArg);
INT32 i4AudOmxCompParamDecoderId(PARAMETER_OMX_AUDIO_PARAM_DECODEID *prUArg); 
INT32 i4AudOmxCompAllocateBuffer(PARAMETER_OMX_ALLOCATEBUFFER *parg);
INT32 i4AudOmxCompFreeBuffer(PARAMETER_OMX_FREEBUFFER *prUArg);

INT32 i4AudOmxCompEmptyBuffer(PARAMETER_OMX_EMPTYTHISBUFFER *prUArg);
INT32 i4AudOmxCompFillBuffer(PARAMETER_OMX_FILLTHISBUFFER *prUArg);

INT32 i4AudOmxCompNotifyEmptyDone(AUD_OMX_HANDLE_T *pHandle, AUD_OMX_BUFFER_T *pBufEmptied);
INT32 i4AudOmxCompNotifyFillDone(AUD_OMX_HANDLE_T *pHandle, AUD_OMX_BUFFER_T *pBufFilled);
INT32 i4AudOmxCompNotifyEvent(AUD_OMX_HANDLE_T *pHandle, PARAMETER_OMX_CB_EVENTHANDLER *pEventInfo);

INT32 i4AudOmxCompPortDisable(AUD_OMX_HANDLE_T *pHandle, BOOL fgInput, BOOL fgOutput);
INT32 i4AudOmxCompPortDisableAsync(PARAMETER_OMX_COMMAND_PORT *prUArg);
INT32 i4AudOmxCompPortEnable(AUD_OMX_HANDLE_T *pHandle, BOOL fgInput, BOOL fgOutput);
INT32 i4AudOmxCompPortEnableAsync(PARAMETER_OMX_COMMAND_PORT *prUArg);

INT32 i4AudOmxAllocateAudioFifo(AUD_OMX_HANDLE_T *pHandle);

INT32 i4AudOmxPrepareDecoding(AUD_OMX_HANDLE_T *pHandle);
INT32 i4AudOmxStartDecoding(AUD_OMX_HANDLE_T *pHandle, void *pFifoSA, UINT32 u4FifoSz);
INT32 i4AudOmxStopDecoding(AUD_OMX_HANDLE_T *pHandle);

void vAudOmxBufferInit(void);
void vAudOmxBufferUninit(void);

INT32 i4AudOmxAllocOutputBuf(AUD_OMX_HANDLE_T *pHandle, UINT32 u4Size, UINT32 u4Port, AUD_OMX_BUFFER_T **ppBufInfo);
INT32 i4AudOmxFreeOutputBuf(AUD_OMX_HANDLE_T *pHandle, HANDLE_T hHeader);
INT32 i4AudOmxAllocInputBuf(AUD_OMX_HANDLE_T *pHandle, UINT32 u4Size, UINT32 u4Port, AUD_OMX_BUFFER_T **ppBufInfo);
INT32 i4AudOmxFreeInputBuf(AUD_OMX_HANDLE_T *pHandle, HANDLE_T hHeader);

INT32 i4AudOmxQueueFillBuf(AUD_OMX_HANDLE_T *pHandle, HANDLE_T hHeader);
INT32 i4AudOmxQueueEmptyBuf(AUD_OMX_HANDLE_T *pHandle, HANDLE_T hHeader);

INT32 i4AudOmxDoEmptyBuf(AUD_OMX_HANDLE_T *pHandle);
INT32 i4AudOmxDoFillBuf(AUD_OMX_HANDLE_T *pHandle);

INT32 i4AudOmxCheckBufEmptyDone(AUD_OMX_HANDLE_T *pHandle);

INT32 i4AudOmxForceAllBufEmptyDone(AUD_OMX_HANDLE_T *pHandle);
INT32 i4AudOmxForceAllBufFillDone(AUD_OMX_HANDLE_T *pHandle);

BOOL fgAudOmxAllInputBufferFreed(AUD_OMX_HANDLE_T *pHandle);
BOOL fgAudOmxAllOutputBufferFreed(AUD_OMX_HANDLE_T *pHandle);
void vAudOmxRestartDecoding(AUD_OMX_HANDLE_T *pHandle);
INT32 i4AudOmxPauseDecoding(AUD_OMX_HANDLE_T *pHandle);
INT32 i4AudOmxResumeDecoding(AUD_OMX_HANDLE_T *pHandle);

void vAudOmxCheckDecodeError(AUD_OMX_HANDLE_T *pHandle);
void vAudOmxCheckUnderrun(AUD_OMX_HANDLE_T *pHandle);
void vAudOmxCheckAoutEnableNotify(AUD_OMX_HANDLE_T *pHandle);

INT32 i4AudOmxStartDualDecoding(AUD_OMX_HANDLE_T *pHandle);
INT32 i4AudOmxStopDualDecoding(AUD_OMX_HANDLE_T *pHandle);
INT32 i4AudOmxSetDualDecoding(AUD_OMX_HANDLE_T *pHandle);

// Vorbis Header
UINT8 u1WrapOggHeader(UINT8 *u1VORBIS_OggS, UINT32 u4VorbisPage, UINT32 u4DataLen);
UINT32 u4FindVorbisHeader(UINT8 *pBuf, UINT32 u4BufSize, UINT8 u1Type);
void vParseVorbisHeader(AUD_OMX_HANDLE_T *pHandle, void *pBuf, UINT32 u4BufSize);
void vStoreCodecData(AUD_OMX_HANDLE_T *pHandle, void *pBuf, UINT32 u4Size);
void vDestroyCodecData(AUD_OMX_HANDLE_T *pHandle);

// Format Not Support Notify
void vAudOmxNotifyFormatNotSupport(AUD_OMX_HANDLE_T *pHandle);

#endif // _AUD_OMX_H_


