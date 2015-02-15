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

#include "x_aud_dec.h"
#include "aud_if.h"
#include "aud_omx.h"
#include "aud_hw.h"
#include <linux/time.h> //ERROR_CNT_MONITOR
#include "dsp_intf.h"
#include "dsp_common.h"

#ifndef SYS_Printf
//#define SYS_Printf Printf
#define SYS_Printf(fmt...)
#endif

/*========== LOCAL VARIABLE/FUNCTION ==========*/
static UINT32 u4BufferSA = 0;
static UINT32 u4BufferEA = 0;
static UINT32 u4BufferAP = 0;
static AUD_OMX_BUFFER_T _prAudOmxInputBuf[AUD_COMP_MAX][INPUT_BUF_MAX];
static AUD_OMX_BUFFER_T _prAudOmxOutputBuf[AUD_COMP_MAX][OUTPUT_BUF_MAX];

UINT32 u4AOmxDbgMsk = 0; //Global control variable for CLI debug
UINT16 u2LPCM_CW = 0; //Global Control variable for CLI debug
INT32 i4AOmxPtsOffset = 0; //Global Control variable for CLI debug

#ifdef AOMX_REAL_PTS
UINT32 u8AudOmxGetPts(AUD_OMX_HANDLE_T* pHandle,UINT64* prAudioPts);
#endif
INT64 i8AudOmxGetCurPts(void);

/*========== EXTERN VARIABLE/FUNCTION ==========*/
extern AUD_OMX_HANDLE_T _prAudOmxHandle[AUD_COMP_MAX];
extern BOOL fgSkipMTAL; //extern global flow control variable
extern BOOL skip_create_feed_stream; //extern global flow control variable

extern UINT32 u4GetAFIFOStart(UINT8 u1DspId, UINT8 uDecIndex);
extern UINT32 u4GetAFIFOEnd(UINT8 u1DspId, UINT8 uDecIndex);
extern UINT32 u4GetABufPnt(UINT8 u1DspId, UINT8 uDecIndex);
extern void vSetAWritePnt(UINT8 uDecIndex, UINT32 u4WritePointer);
extern void DSP_FlushInvalidateDCacheFree(UINT32 u4Addr, UINT32 u4Len);
extern UINT32 u4ReadD2RInfo_TotalBank(UINT8 u1DspId,UINT8 u1DecId);
extern BOOL AUD_IsFifoEmpty(UINT8 u1DspId, UINT8 u1DecId);
extern void AUD_MMAoutEnable(UINT8 u1DecId, BOOL fgEnable);
#ifdef AOMX_REAL_PTS
extern UINT64 u8Div6432(UINT64 u8Dividend, UINT64 u8Divider, UINT64 *pu8Remainder);
extern BOOL GST_MMFindSyncInfo(UINT8 u1DecId, UINT64* u8PTS, UINT32 u4PTSWp, UINT32* u4PTSHigh);
extern void GST_MMQueueSyncInfo(UINT8 u1DecId, UINT64 u8PTS, UINT32 u4PTSWp, BOOL fgReset, UINT32 u4PTSHigh);
extern UINT32 u4ReadD2RInfo_PtsCurPnt(UINT8 u1DspId, UINT8 u1DecId);
#endif
extern UINT32 u4ReadD2RInfo_InputUnderRun(UINT8 u1DecId);
extern UINT16 bDrvVideoGetFrameDelay(UINT8 bPath);
static UINT8 u1SpkCfg = 0; //driver status variable
extern void AUD_LineInCtrlEnable(UINT8 u1DecId, BOOL fgEnable);
#if (AUD_COMP_MAX==1)
static AV_SYNC_MODE_T prev_av_sync_mode[AUD_COMP_MAX] = {0}; //driver status variable
#else
static AV_SYNC_MODE_T prev_av_sync_mode[AUD_COMP_MAX] = {0,0}; //driver status variable
#endif
extern AV_SYNC_MODE_T AUD_GetAvSynMode(UINT8 u1DecId);
extern UINT32 u4AudGetDecErrorCnt(UINT8 u1DecId);
extern UINT32 u4AudGetDecFrameCnt(UINT8 u1DecId);
extern void _AUD_DspSetDualDecMode(BOOL fgEnable);
#ifdef CC_AUD_LINEIN_USE_DEC3 
extern void _AUD_DspSetTriOnlyDecMode(BOOL fgEnable);
#endif
//======================================================
//Macro/Define

#define BSP_InvDCacheRange(a,b) DSP_FlushInvalidateDCacheFree(a,b)
#define BSP_CleanDCacheRange(a,b) DSP_FlushInvalidateDCacheFree(a,b)
#define vAudHalSetBufWPtr(a,b) vSetAWritePnt((UINT8)(a), PHYSICAL(b))
#define u4AudHalGetBufRPtr(a) VIRTUAL(u4GetABufPnt(AUD_DSP0, (UINT8)(a)))

#define x_free_aligned_afifo_mem(a)
#define x_alloc_aligned_afifo_mem(a,b) u4AudBufferAlloc(a)

#define SWDMX_AVI_AAC_FAKE_HDR_SIZE 7

#define PTS_INTERVAL (MAX_OUTPUT_BUF_SIZE * 1000L / 192L) //(170667L) //This value is 32*1024/4/48000*1000000. If you change the upload buffer size, you also need to change this value.
#define PTS_INTERVAL_BANK256_NUM  (MAX_OUTPUT_BUF_SIZE/1024)
#define PTS_BANK256 5333     //= 256/48000*1000000

//UNDERRUN_CNT_MONITOR
#define INPUT_UNDERRUN_DISABLE_TIMER 4

//======================================================
//AOMX_DUMP
UINT32 u4AOMXDumpBufAddr = 0;
UINT32 u4AOMXDumpSize = 0;
UINT32 u4AOMXDumpBufAddrCur = 0;
UINT32 u4AOMXDumpSizeCur = 0;
UINT8 u1AOMXDumpLocation = 0; // 0: Input Buffer


//=============== AOMX Configuration Param ===================
INT32 i4AOMXAVComps = 0;
BOOL fgAOMXDumpDataEnable = FALSE;

//=======================================================
void vAudOmxCopyData2Afifo(UINT32 u4AFifoAddr, UINT32 u4Src, UINT32 u4Size)
{
	UINT32 i;
	UINT32 u4Cnt = 0;
	UINT32 u4FlushSize = 0x100000;
	
	u4Cnt = u4Size / u4FlushSize;
	if(0 != u4Cnt)
	{
		for(i = 0; i < u4Cnt; i++)
		{
			x_memcpy((void*)(u4AFifoAddr + i*u4FlushSize), (const void*)(u4Src + i*u4FlushSize), u4FlushSize);
			BSP_CleanDCacheRange((u4AFifoAddr + (i*u4FlushSize)), u4FlushSize); 
		}
	}

	x_memcpy((void*)(u4AFifoAddr + u4Cnt*u4FlushSize), (const void*)(u4Src + u4Cnt*u4FlushSize), (u4Size - u4Cnt*u4FlushSize));
	BSP_CleanDCacheRange((u4AFifoAddr + (u4Cnt*u4FlushSize)), (u4Size - u4Cnt*u4FlushSize));
}

void vAudOmxSetDumpInfo(UINT32 u4Addr, UINT32 u4Size)
{
    u4AOMXDumpBufAddr = u4Addr;
    u4AOMXDumpSize = u4Size;
    u4AOMXDumpBufAddrCur = u4Addr;
    u4AOMXDumpSizeCur = 0;
    Printf("[AUD][OMX][DBG] Set AOMX Dump Start Addr: %x Size: %x\n", u4AOMXDumpBufAddr, u4AOMXDumpSize);    
}

void vAudOmxResetDumpInfo(void)
{
    u4AOMXDumpBufAddrCur = u4AOMXDumpBufAddr;
    u4AOMXDumpSizeCur = 0;
    Printf("[AUD][OMX][DBG] Reset AOMX Dump Start Addr: %x Size: %x\n", u4AOMXDumpBufAddrCur, u4AOMXDumpSize);        
}

void vAudOmxFillDumpBuf(void* pFrom, UINT32 u4Size)
{
    if (u4AOMXDumpSizeCur < u4AOMXDumpSize)
    {
        if (u4AOMXDumpSizeCur + u4Size < u4AOMXDumpSize)
        {
            x_memcpy((void*)(u4AOMXDumpBufAddrCur), pFrom, u4Size);
            u4AOMXDumpSizeCur += u4Size;
            u4AOMXDumpBufAddrCur += u4Size;
        }
        else
        {
            x_memcpy((void*)(u4AOMXDumpBufAddrCur), pFrom, (u4AOMXDumpSize - u4AOMXDumpSizeCur));
            u4AOMXDumpSizeCur = u4AOMXDumpSize;
            u4AOMXDumpBufAddrCur = (u4AOMXDumpBufAddr + u4AOMXDumpSize);
            Printf("[AUD][OMX][DBG] AOMX Buffer Dump Finished at location %d (0: Input)\n"
                   "[AUD][OMX][DBG] Please use dump tool to dump %x to %x\n", u1AOMXDumpLocation, u4AOMXDumpBufAddr, u4AOMXDumpBufAddrCur-1);
        }
    }
}
//END
//======================================================

void vAudOmxQuery(UINT8 uDecIndex)
{
    if (uDecIndex >= AUD_COMP_MAX)
        uDecIndex = 0;
    
    Printf("== AOMX Status (%d) ==\n", uDecIndex);
#ifdef AOMX_REAL_PTS
    if (!(u4AOmxDbgMsk & 0x800) && (uDecIndex == AUD_DEC_MAIN))
    {
        Printf("(AUD AOut Path + Real-PTS)\n");    
    }
    else
#endif        
    {
        Printf("(AUD AOut Path + Fake-PTS)\n");    
    }    
    Printf("  eState: %d (1: load 2: idle 3: exec 4: pause)\n", _prAudOmxHandle[uDecIndex].eState);
    Printf("  u4AomxThreadCnt: %d\n", _prAudOmxHandle[uDecIndex].u4AomxThreadCnt);
    Printf("  u4AomxThreadState: %d\n", _prAudOmxHandle[uDecIndex].u4AomxThreadState);
    Printf("  FMT: %d\n", _prAudOmxHandle[uDecIndex].eFormat);
    Printf("  fgEmptying: %d\n",_prAudOmxHandle[uDecIndex].fgEmptying);
    Printf("  fgFilling: %d\n", _prAudOmxHandle[uDecIndex].fgFilling);
    Printf("  fgGotEOS: %d\n", _prAudOmxHandle[uDecIndex].fgGotEOS);

    Printf("  u4CmdQSendNum: %d u4CmdQRecvNum: %d\n", _prAudOmxHandle[uDecIndex].u4CmdQSendNum, _prAudOmxHandle[uDecIndex].u4CmdQRecvNum);
#ifdef AOMX_CMD_QUEUE    
    Printf("  u4MsgReadIdx: %d u4MsgWriteIdx: %d\n", _prAudOmxHandle[uDecIndex].u4MsgReadIdx, _prAudOmxHandle[uDecIndex].u4MsgWriteIdx);
#endif
    Printf("  EmptyQueueNo: %d/%d/%d\n", _prAudOmxHandle[uDecIndex].u4EmptyQueueNo, _prAudOmxHandle[uDecIndex].u4TotalEmptyQueueDoneNo, _prAudOmxHandle[uDecIndex].u4TotalEmptyQueueNo);
    Printf("  FillQueueNo: %d/%d/%d\n", _prAudOmxHandle[uDecIndex].u4FillQueueNo, _prAudOmxHandle[uDecIndex].u4TotalFillQueueDoneNo, _prAudOmxHandle[uDecIndex].u4TotalFillQueueNo);
    Printf("  u4TotalEmptySize: %d\n", _prAudOmxHandle[uDecIndex].u4TotalEmptySize);
    Printf("  u4TotalInputSize: %d\n", _prAudOmxHandle[uDecIndex].u4TotalInputSize);

    Printf("== Status Bit ==\n");
    Printf("  fgAoutEnable: %d\n", _prAudOmxHandle[uDecIndex].fgAoutEnable);
    Printf("  fgRenderTrigger: %d\n", _prAudOmxHandle[uDecIndex].fgRenderTrigger);
    Printf("  fgPauseTrigger: %d\n", _prAudOmxHandle[uDecIndex].fgPauseTrigger);    
    Printf("  fgFormatNotSupport: %d\n", _prAudOmxHandle[uDecIndex].fgFormatNotSupport);
    Printf("  fgGotValidPts: %d\n", _prAudOmxHandle[uDecIndex].fgGotValidPts);
    Printf("  fgEnableAdtsInsert: %d\n", _prAudOmxHandle[uDecIndex].fgEnableAdtsInsert);
    Printf("  fgIsAacInTs: %d\n", _prAudOmxHandle[uDecIndex].fgIsAacInTs);
    Printf("  fgIsFirstChk: %d\n", _prAudOmxHandle[uDecIndex].fgIsFirstChk);    
    Printf("  fgSkipMTAL: %d\n", fgSkipMTAL);
    Printf("  skip_create_feed_stream: %d\n", skip_create_feed_stream);

    Printf("== PTS Information ==\n");
    Printf("  nOutputTimeStampLast: %lld\n", _prAudOmxHandle[uDecIndex].nOutputTimeStampLast);
    Printf("  i8AudOmxGetCurPts: %lld\n", i8AudOmxGetCurPts()); //AOMX2_TEST_TODO
    Printf("  bDrvVideoGetFrameDelay: %d\n", bDrvVideoGetFrameDelay(uDecIndex));
    
    Printf("== AFIFO1 Status ==\n");
    Printf("  SA: 0x%08x\n", _prAudOmxHandle[uDecIndex].pFifoPntr);
    Printf("  EA: 0x%08x\n", _prAudOmxHandle[uDecIndex].pFifoPntr + _prAudOmxHandle[uDecIndex].u4FifoSize);
    Printf("  WP: 0x%08x\n", _prAudOmxHandle[uDecIndex].pFifoWptr);
    Printf("  RP: 0x%08x\n", VIRTUAL(u4GetABufPnt(AUD_DSP0, uDecIndex)));

    Printf("== Dump Status ==\n");
    Printf("Addr: 0x%08x/0x%08x\n", u4AOMXDumpBufAddrCur, u4AOMXDumpBufAddr);
    Printf("Size: %x/%x\n", u4AOMXDumpSizeCur, u4AOMXDumpSize);
}

static void vAudBufferInit(void)
{
    u4BufferSA = VIRTUAL(u4GetAFIFOStart(AUD_DSP0, 3));
    u4BufferEA = VIRTUAL(u4GetAFIFOEnd(AUD_DSP0, 3));
    u4BufferAP = u4BufferSA;
}

static UINT32 u4AudBufferAlloc(UINT32 u4Size)
{
    UINT32 u4AP;
    if (u4BufferAP + u4Size > u4BufferEA)
    {
        //ASSERT(0); //FOR_IDTV0801
    }

    u4AP = u4BufferAP;
    //SYS_Printf("[u4AudBufferAlloc] SA: 0x%08x SIZE: 0x%08x\n", u4AP, u4Size);
    u4BufferAP += u4Size;

    return u4AP;
}

void vAudOmxBufferInit(void)
{
	UINT32 u4OmxId;
    UINT32 u4Index;
    //HANDLE_T hHeader = NULL_HANDLE;

	for(u4OmxId = 0; u4OmxId < AUD_COMP_MAX; u4OmxId++)
	{
	    for (u4Index = 0; u4Index < INPUT_BUF_MAX; u4Index ++)
	    {
	        // can not do this on open ???
	        //hHeader = x_kmem_create_handle(&(_prAudOmxInputBuf[u4Index].rHeader), sizeof(OMX_BUFFERHEADERTYPE));
	        //ASSERT(NULL_HANDLE == hHeader);

	        _prAudOmxInputBuf[u4OmxId][u4Index].pOwner     = NULL;
	        _prAudOmxInputBuf[u4OmxId][u4Index].hBuffer    = NULL_HANDLE;
	        _prAudOmxInputBuf[u4OmxId][u4Index].pBufSA     = NULL;
	        _prAudOmxInputBuf[u4OmxId][u4Index].u4BufSize  = 0;
	        _prAudOmxInputBuf[u4OmxId][u4Index].hHeader    = NULL_HANDLE;
	        _prAudOmxInputBuf[u4OmxId][u4Index].u4Reserved = 0x5A5A5A5A;
	    }
	}

	for(u4OmxId = 0; u4OmxId < AUD_COMP_MAX; u4OmxId++)
	{
	    for (u4Index = 0; u4Index < OUTPUT_BUF_MAX; u4Index ++)
	    {
	        //hHeader = x_kmem_create_handle(&(_prAudOmxOutputBuf[u4Index].rHeader), sizeof(OMX_BUFFERHEADERTYPE));
	        //ASSERT(NULL_HANDLE == hHeader);

	        _prAudOmxOutputBuf[u4OmxId][u4Index].pOwner     = NULL;
	        _prAudOmxOutputBuf[u4OmxId][u4Index].hBuffer    = NULL_HANDLE;
	        _prAudOmxOutputBuf[u4OmxId][u4Index].pBufSA     = NULL;
	        _prAudOmxOutputBuf[u4OmxId][u4Index].u4BufSize  = 0;
	        _prAudOmxOutputBuf[u4OmxId][u4Index].hHeader    = NULL_HANDLE;
	        _prAudOmxOutputBuf[u4OmxId][u4Index].u4Reserved = 0xA5A5A5A5;
	    }
	}

    vAudBufferInit();
}

void vAudOmxBufferUninit(void)
{
	UINT32 u4OmxId;
    UINT32 u4Index;
	
	for(u4OmxId = 0; u4OmxId < AUD_COMP_MAX; u4OmxId++)
	{
	    for (u4Index = 0; u4Index < INPUT_BUF_MAX; u4Index ++)
	    {
	        if (NULL_HANDLE != _prAudOmxInputBuf[u4OmxId][u4Index].hHeader)
	        {
	            SYS_Printf("%s %d: x_kmem_destroy_handle\n", __FUNCTION__, __LINE__);
	            x_kmem_destroy_handle(_prAudOmxInputBuf[u4OmxId][u4Index].hHeader);
	        }
	    }
	}
	
	for(u4OmxId = 0; u4OmxId < AUD_COMP_MAX; u4OmxId++)
	{
	    for (u4Index = 0; u4Index < OUTPUT_BUF_MAX; u4Index ++)
	    {
	        if (NULL_HANDLE != _prAudOmxOutputBuf[u4OmxId][u4Index].hHeader)
	        {
	            SYS_Printf("%s %d: x_kmem_destroy_handle\n", __FUNCTION__, __LINE__);
	            x_kmem_destroy_handle(_prAudOmxOutputBuf[u4OmxId][u4Index].hHeader);
	        }
	    }
	}
}

// allocate output buffer from AFIFO (temp solution?)
// OpenMAX client may allocate output buffer more than one time
INT32 i4AudOmxAllocOutputBuf(AUD_OMX_HANDLE_T *pHandle, UINT32 u4Size, UINT32 u4Port, AUD_OMX_BUFFER_T **ppBufInfo)
{
    UINT32 u4Index;

    void *pBufSA  = NULL;

    HANDLE_T hHeader = NULL_HANDLE;
    HANDLE_T hMemHandle = NULL_HANDLE;
    AUD_OMX_BUFFER_T *pAvailBuf = NULL;

    // multi-instance protection, take semaphore
    i4AudOmxTaskLockSem();

    // STEP 0: get a free output buffer info node
    for (u4Index = 0; u4Index < OUTPUT_BUF_MAX; u4Index ++)
    {
        if (NULL == _prAudOmxOutputBuf[pHandle->h_handle][u4Index].pOwner)
        {
            pAvailBuf = &(_prAudOmxOutputBuf[pHandle->h_handle][u4Index]);
            break;
        }
    }

    if (NULL == pAvailBuf)
    {
        // no more free nodes
        goto lblErrorReturn;
    }
    else
    {
        // initialize
        pAvailBuf->pOwner    = pHandle;
        pAvailBuf->hBuffer   = NULL_HANDLE;
        pAvailBuf->pBufSA    = NULL;
        pAvailBuf->u4BufSize = 0;
    }

    SYS_Printf("[AUD][OMX(%d)] %s %d: x_kmem_create_handle\n", pHandle->h_handle, __FUNCTION__, __LINE__);
    hHeader = x_kmem_create_handle(&(pAvailBuf->rHeader), sizeof(OMX_BUFFERHEADERTYPE));

    if (NULL_HANDLE == hHeader)
    {
        goto lblErrorReturn;
    }
    else
    {
        pAvailBuf->hHeader = hHeader;
    }

    // STEP 1: allocate input buffer from AFIFO
    pBufSA = (void*)x_alloc_aligned_afifo_mem(0/*u4Size*/, 256);

    if (NULL == pBufSA)
    {
        SYS_Printf("[AUD][OMX(%d)] alloc AFIFO memory failed, %d bytes.\n", pHandle->h_handle, u4Size);
        goto lblErrorReturn;
    }
    else
    {
        pAvailBuf->pBufSA    = pBufSA;
        pAvailBuf->u4BufSize = u4Size;
    }

    SYS_Printf("[AUD][OMX(%d)] %s %d: x_kmem_create_handle\n", pHandle->h_handle, __FUNCTION__, __LINE__);
    hMemHandle = x_kmem_create_handle(pBufSA, u4Size);

    if (NULL_HANDLE == hMemHandle)
    {
        goto lblErrorReturn;
    }
    else
    {
        pAvailBuf->hBuffer = hMemHandle;
    }

    // fill buffer header
    x_memset(&(pAvailBuf->rHeader), 0x00, sizeof(OMX_BUFFERHEADERTYPE));

    pAvailBuf->rHeader.nSize             = sizeof(OMX_BUFFERHEADERTYPE);
    pAvailBuf->rHeader.nVersion.nVersion = OMX_VERSION;

    pAvailBuf->rHeader.pBuffer          = pBufSA;
    pAvailBuf->rHeader.nAllocLen        = u4Size;
    pAvailBuf->rHeader.nFilledLen       = 0;
    pAvailBuf->rHeader.nOffset          = 0;
    pAvailBuf->rHeader.nOutputPortIndex = u4Port;

    // release semaphore
    i4AudOmxTaskUnlockSem();

    *ppBufInfo = pAvailBuf;
    return AUD_OK;

    // error handling
lblErrorReturn:
    // release semaphore
    i4AudOmxTaskUnlockSem();

    // resource release will be done on freeing component handle, not here
    *ppBufInfo = NULL;
    return AUD_FAIL;
}

INT32 i4AudOmxFreeOutputBuf(AUD_OMX_HANDLE_T *pHandle, HANDLE_T hHeader)
{
    UINT32 u4Index;
    AUD_OMX_BUFFER_T *pBufNode = NULL;

    if ((NULL == pHandle) || (NULL_HANDLE == hHeader))
    {
        return AUD_OK;
    }

    // multi-instance protection, take semaphore
    i4AudOmxTaskLockSem();

    for (u4Index = 0; u4Index < OUTPUT_BUF_MAX; u4Index ++)
    {
        if (hHeader == _prAudOmxOutputBuf[pHandle->h_handle][u4Index].hHeader)
        {
            pBufNode = &(_prAudOmxOutputBuf[pHandle->h_handle][u4Index]);
            break;
        }
    }

    if (NULL != pBufNode)
    {
        if ((pBufNode->pOwner == pHandle) && (NULL != pBufNode->pBufSA))
        {
            // free memory
            x_free_aligned_afifo_mem(pBufNode->pBufSA);

            // destory the buffer handle
            SYS_Printf("[AUD][OMX(%d)] %s %d: x_kmem_destroy_handle\n", pHandle->h_handle, __FUNCTION__, __LINE__);
            x_kmem_destroy_handle(pBufNode->hBuffer);

            // destory the header handle
            SYS_Printf("[AUD][OMX(%d)] %s %d: x_kmem_destroy_handle\n", pHandle->h_handle, __FUNCTION__, __LINE__);
            x_kmem_destroy_handle(pBufNode->hHeader);

            // clear info
            pBufNode->pOwner    = NULL;
            pBufNode->hBuffer   = NULL_HANDLE;
            pBufNode->pBufSA    = NULL;
            pBufNode->u4BufSize = 0;
            pBufNode->hHeader   = NULL_HANDLE;
        }
    }

    // release semaphore
    i4AudOmxTaskUnlockSem();

    return AUD_OK;
}

INT32 i4AudOmxQueueFillBuf(AUD_OMX_HANDLE_T *pHandle, HANDLE_T hHeader)
{
    INT32 i4Ret = AUD_FAIL;
    UINT32 u4Index;
    AUD_OMX_BUFFER_T *pBufNode = NULL;

    if ((NULL == pHandle) || (NULL_HANDLE == hHeader))
    {
        return AUD_OK;
    }

    // multi-instance protection, take semaphore
    i4AudOmxTaskLockSem();

    for (u4Index = 0; u4Index < OUTPUT_BUF_MAX; u4Index ++)
    {
        if (hHeader == _prAudOmxOutputBuf[pHandle->h_handle][u4Index].hHeader)
        {
            pHandle->u4FillQueueNo++;
            pHandle->u4TotalFillQueueNo++;
            if (u4AOmxDbgMsk & 0x40)
            {
                Printf("[AUD][OMX(%d)] QueueFillBuf(%d)\n", pHandle->h_handle, pHandle->u4FillQueueNo);
            }
            pBufNode = &(_prAudOmxOutputBuf[pHandle->h_handle][u4Index]);
            break;
        }
    }

    if (NULL != pBufNode)
    {
        if (pBufNode->pOwner == pHandle)
        {
            // add to queue
            for (u4Index = 0; u4Index < OUTPUT_BUF_MAX; u4Index ++)
            {
                if (NULL == pHandle->pFillQ[u4Index])
                {
                    i4Ret = AUD_OK;
                    pHandle->fgFilling = TRUE;
                    pHandle->pFillQ[u4Index] = pBufNode;
                    break;
                }
            }

            if (AUD_OK != i4Ret)
            {
                SYS_Printf("[AUD][OMX%d] WARNING: fill buffer queue is full.\n", pHandle->h_handle);
            }

            // initialize buffer filled len
            // (The OMX_FillThisBuffer macro will send an empty buffer to an output port of a component.)
            pBufNode->u4FilledLen = 0;
            pBufNode->u4Offset    = 0;
        }
    }

    // release semaphore
    i4AudOmxTaskUnlockSem();

    return i4Ret;
}

static BOOL fgAudOmxOutputNotReady(AUD_OMX_HANDLE_T *pHandle,UINT32 *u4PtsDiff)
{
    UINT32 u4BankNum,u4BankDiff;
    BOOL fgResult = TRUE;
    *u4PtsDiff = 0;
    if (AUD_IsDSPRealPlay(AUD_DSP0, pHandle->u1DecId))    //check if Aout Enable
    {
         u4BankNum = u4ReadD2RInfo_TotalBank(AUD_DSP0, pHandle->u1DecId); //AOMX2_TEST_TODO
         if (!pHandle->fgAoutEnable || (!pHandle->fgRenderTrigger) || pHandle->fgPauseTrigger)
         {
            //send out start PTS
            fgResult = FALSE;
            if (!pHandle->fgAoutEnable)
            {
                Printf("[AUD][OMX(%d)] OMX AOUT ENABLE!!!\n", pHandle->h_handle);
            }
            pHandle->fgAoutEnable = TRUE;

         }
         else if (u4BankNum < pHandle->u4BankNumPrev) //check if Aout Reinit
         {
            //aout reinit, workaround to compesate the missing aput_bank
            //ori:   64(PTS)   -----70------96 (PTS)
            //                                 0 (reinit)
            //new: 64(PTS)   -----  0------26 (PTS)
            //we should set i4PTSBankNumPrev = -6 ( 64-70) 
            //assume we can get the last bank num = 70 => i4PTSBankNumPrev = i4PTSBankNumPrev - u4BankNumPrev
            pHandle->i4PTSBankNumPrev = pHandle->i4PTSBankNumPrev - (INT32)(pHandle->u4BankNumPrev);
            Printf("[AUD][OMX(%d)] PTS bank reset!!", pHandle->h_handle);
         }
         else if  (((INT32)u4BankNum - pHandle->i4PTSBankNumPrev >= PTS_INTERVAL_BANK256_NUM) )
         {
                fgResult = FALSE;
                u4BankDiff = (UINT32)((INT32)u4BankNum - pHandle->i4PTSBankNumPrev);
                *u4PtsDiff = u4BankDiff * PTS_BANK256;
                pHandle->i4PTSBankNumPrev = (INT32)u4BankNum;
                if (u4AOmxDbgMsk & 0x1000)
                {
                    Printf("[AUD][OMX(%d)] Bank num = %d, diff = %d\n", pHandle->h_handle,u4BankNum,u4BankDiff);
                }
         }
         pHandle->u4BankNumPrev = u4BankNum;
    }
    else
    {
        SYS_Printf("[AUD][OMX(%d)] OMX WAIT AOUT ENABLE\n", pHandle->h_handle);
    }
    return fgResult;
}

INT32 i4AudOmxDoFillBuf(AUD_OMX_HANDLE_T *pHandle)
{
    INT32 i4Ret = AUD_FAIL;
    UINT32 u4FilledSize;
    UINT32 u4Index;
    BOOL fgAllFilled = TRUE;
    AUD_OMX_BUFFER_T *pToFillBuf = NULL;
    static int retry_cnt = 0;
    UINT32 u4PtsDiff;
#ifdef AOMX_REAL_PTS    
    UINT64 u8RealPts;
#endif
    BOOL fgFifoEmpty = FALSE;
    // always left aligned
    pToFillBuf = pHandle->pFillQ[0];

    if (NULL == pToFillBuf)
    {
        // error handling
        pHandle->fgFilling = FALSE;

        return AUD_OK;
    }

    if (pHandle->fgGotEOS)
    {
        fgFifoEmpty = AUD_IsFifoEmpty(AUD_DSP0, pHandle->u1DecId); //check if still output data
        if (fgFifoEmpty)
        {
            retry_cnt++;
            if (retry_cnt < 10)
                fgFifoEmpty = FALSE;
        }
    }
    else
    {   
        retry_cnt = 0;
    }
      
    // if decoder is not ready, can not fill buffer now
    if (fgAudOmxOutputNotReady(pHandle, &u4PtsDiff) && !fgFifoEmpty && !(pHandle->fgFormatNotSupport))
    {
        SYS_Printf("[AUD][OMX(%d)] i4AudOmxDoFillBuf: decoder not ready.\n", pHandle->h_handle);
        return AUD_OK;
    }

	if (pHandle->fgFormatNotSupport)
	{
		// Avoid run too fast to hunger other thread
		x_thread_delay(10);
	}
    // multi-instance protection, take semaphore
    i4AudOmxTaskLockSem();
    u4FilledSize = MAX_OUTPUT_BUF_SIZE;
    pToFillBuf->u4Offset = u4FilledSize;
    pToFillBuf->u4FilledLen = u4FilledSize;

    if ((pToFillBuf->u4FilledLen == pToFillBuf->u4BufSize) )
    {
        // if fill is done, align queue to left and check if there is pending fill event
        for (u4Index = 0; u4Index < OUTPUT_BUF_MAX - 1; u4Index ++)
        {
            pHandle->pFillQ[u4Index] = pHandle->pFillQ[u4Index + 1];

            if (NULL != pHandle->pFillQ[u4Index])
            {
                fgAllFilled = FALSE;
            }
        }
        // clear the tail node
        pHandle->pFillQ[OUTPUT_BUF_MAX - 1] = NULL;

        if (fgAllFilled)
        {
            // fill queue is empty
            pHandle->fgFilling = FALSE;
        }

        if (pHandle->fgGotEOS && (fgFifoEmpty || pHandle->fgFormatNotSupport))
        {
            Printf("[AUD][OMX(%d)] NotifyFillDone EOS\n", pHandle->h_handle);
            pToFillBuf->rHeader.nFlags |= OMX_BUFFERFLAG_EOS;
        }
        else
        {
            pToFillBuf->rHeader.nFlags &= ~OMX_BUFFERFLAG_EOS;
        }

        //PTS
        {
            if (u4PtsDiff == 0)
            {
#ifdef AOMX_REAL_PTS
				if (!(u4AOmxDbgMsk & 0x800) && ((pHandle->u1DecId == AUD_DEC_MAIN) || (pHandle->u1DecId == AUD_DEC_AUX) || (pHandle->u1DecId == AUD_DEC_THIRD)))
                {
                    u8AudOmxGetPts(pHandle,&u8RealPts);
                    pToFillBuf->rHeader.nTimeStamp = u8RealPts;     //first aout enable
                }                
#endif                
                if (u4AOmxDbgMsk & 0x10)
                {
                    struct timeval tv;
                    do_gettimeofday(&tv);
                    Printf("[AUD][OMX(%d)] %d.%06d OUTPTS: %lld \n", pHandle->h_handle, tv.tv_sec, tv.tv_usec, pToFillBuf->rHeader.nTimeStamp);
                }
            }
            else if (0 != pToFillBuf->u4FilledLen)
            {
                pHandle->nOutputTimeStampCur += u4PtsDiff;
#ifdef AOMX_REAL_PTS                
				if(!(u4AOmxDbgMsk & 0x800) && ((pHandle->u1DecId == AUD_DEC_MAIN) || (pHandle->u1DecId == AUD_DEC_AUX) || (pHandle->u1DecId == AUD_DEC_THIRD)))
                {
                    u8AudOmxGetPts(pHandle,&u8RealPts);
                    //pToFillBuf->rHeader.nTimeStamp = u8Div6432((UINT64)u8RealPts * 100,9,NULL);       //convert 32bits -> 64bits 
                    pToFillBuf->rHeader.nTimeStamp = u8RealPts;
                    if (u4AOmxDbgMsk & 0x10)
                    {
                        struct timeval tv;
                        do_gettimeofday(&tv);
                    
                        Printf("[AUD][OMX(%d)] %d.%06d u8RealPts = %lld, fakePTS = %lld, OUTPTS = %lld \n", pHandle->h_handle,
                                tv.tv_sec, tv.tv_usec,
                                pHandle->u8LastRealPts,
                                pHandle->nOutputTimeStampCur,
                                pToFillBuf->rHeader.nTimeStamp);
                    }
					pHandle->u8LastRealPts = u8RealPts;
                }
                else
#endif                    
                {                   
                    pToFillBuf->rHeader.nTimeStamp = pHandle->nOutputTimeStampCur;
                    if (u4AOmxDbgMsk & 0x10)
                    {
                        struct timeval tv;
                        do_gettimeofday(&tv);                    
                        Printf("[AUD][OMX(%d)] %d.%06d OUTPTS: %lld, STC = %lld \n", pHandle->h_handle, tv.tv_sec, tv.tv_usec, pHandle->nOutputTimeStampCur,i8AudOmxGetCurPts());
                    }
                }
                //END
            }

            if (u4AOmxDbgMsk & 0x10)
            {
                if (pHandle->nOutputTimeStampLast != -1)
                {
                    Printf("[AUD][OMX(%d)] Diff: %lld\n", pHandle->h_handle, pToFillBuf->rHeader.nTimeStamp - pHandle->nOutputTimeStampLast);
                }
                else
                {
                    Printf("\n");
                }
            }
            pHandle->nOutputTimeStampLast = pToFillBuf->rHeader.nTimeStamp;
        }
        //END

        pToFillBuf->rHeader.nTimeStamp += i4AOmxPtsOffset;
        //END

        //If there is no input data, reset u4FilledLen to 0.
        if (pHandle->u4TotalInputSize==0)
        {
            pToFillBuf->u4FilledLen = 0;
        }

         // if fill is done, callback now
        i4AudOmxCompNotifyFillDone(pHandle, pToFillBuf);

        pHandle->u4FillQueueNo--;
        pHandle->u4TotalFillQueueDoneNo++;
        if (u4AOmxDbgMsk & 0x40)
        {
            Printf("[AUD][OMX(%d)] QueueFillBuf Done(%d)\n", pHandle->h_handle, pHandle->u4FillQueueNo);
        }
    }

    if (fgAllFilled && pHandle->fgGotEOS && (fgFifoEmpty || pHandle->fgFormatNotSupport))
    {
        // if EOS received and no output sample now, callback
        PARAMETER_OMX_CB_EVENTHANDLER rEvent;

        rEvent.eEvent     = OMX_EventBufferFlag;
        rEvent.nData1     = 1; // output port index
        rEvent.nData2     = OMX_BUFFERFLAG_EOS;
        rEvent.pEventData = NULL;

        Printf("[AUD][OMX(%d)] notify OMX_BUFFERFLAG_EOS\n", pHandle->h_handle);

        i4AudOmxCompNotifyEvent(pHandle, &rEvent);
    }

    // release semaphore
    i4AudOmxTaskUnlockSem();

    return i4Ret;
}

INT32 i4AudOmxForceAllBufFillDone(AUD_OMX_HANDLE_T *pHandle)
{
    INT32 i4Ret = AUD_FAIL;
    UINT32 u4Index;
    AUD_OMX_BUFFER_T *pToFillBuf = NULL;

    // multi-instance protection, take semaphore
    i4AudOmxTaskLockSem();

     if (u4AOmxDbgMsk & 0x40)
    {
        Printf("[AUD][OMX(%d)] i4AudOmxForceAllBufFillDone\n", pHandle->h_handle);
    }
    SYS_Printf("[AUD][OMX(%d)] i4AudOmxForceAllBufFillDone\n", pHandle->h_handle);

    for (u4Index = 0; u4Index < OUTPUT_BUF_MAX; u4Index ++)
    {
        pToFillBuf = pHandle->pFillQ[u4Index];

        if (NULL != pToFillBuf)
        {
            i4AudOmxCompNotifyFillDone(pHandle, pToFillBuf);

            pHandle->u4FillQueueNo--;
            pHandle->u4TotalFillQueueDoneNo++;
            if (u4AOmxDbgMsk & 0x40)
            {
                Printf("[AUD][OMX(%d)] QueueFillBuf Done(%d)\n", pHandle->h_handle, pHandle->u4FillQueueNo);
            }

            // clear
            pHandle->pFillQ[u4Index] = NULL;
        }
    }

    pHandle->fgFilling = FALSE;

    // release semaphore
    i4AudOmxTaskUnlockSem();

    return i4Ret;
}

INT32 i4AudOmxAllocInputBuf(AUD_OMX_HANDLE_T *pHandle, UINT32 u4Size, UINT32 u4Port, AUD_OMX_BUFFER_T **ppBufInfo)
{
    // allocate input buffer from AFIFO
    // allocate continous buffer from AFIFO for decoder

    // to maintain the continuity of allocated buffers, we always free previously allocated buffers
    // and allocate a larger one containing both previous and current buffer

    UINT32 u4Index;

    void *pBufSA  = NULL;
    //void *pFifoSA = NULL;

    HANDLE_T hHeader = NULL_HANDLE;
    HANDLE_T hMemHandle = NULL_HANDLE;
    AUD_OMX_BUFFER_T *pAvailBuf = NULL;

    // multi-instance protection, take semaphore
    i4AudOmxTaskLockSem();

    // STEP 0: get a free input buffer info node
    for (u4Index = 0; u4Index < INPUT_BUF_MAX; u4Index ++)
    {
        if (NULL == _prAudOmxInputBuf[pHandle->h_handle][u4Index].pOwner)
        {
            pAvailBuf = &(_prAudOmxInputBuf[pHandle->h_handle][u4Index]);
            break;
        }
    }

    if (NULL == pAvailBuf)
    {
        // no more free nodes
        goto lblErrorReturn;
    }
    else
    {
        // initialize
        pAvailBuf->pOwner    = pHandle;
        pAvailBuf->hBuffer   = NULL_HANDLE;
        pAvailBuf->pBufSA    = NULL;
        pAvailBuf->u4BufSize = 0;
        pAvailBuf->hHeader   = NULL_HANDLE;
    }

    SYS_Printf("[AUD][OMX(%d)] %s %d: x_kmem_create_handle\n", pHandle->h_handle, __FUNCTION__, __LINE__);
    hHeader = x_kmem_create_handle(&(pAvailBuf->rHeader), sizeof(OMX_BUFFERHEADERTYPE));

    if (NULL_HANDLE == hHeader)
    {
        goto lblErrorReturn;
    }
    else
    {
        pAvailBuf->hHeader = hHeader;
    }

    // STEP 1: allocate input buffer from AFIFO
    pBufSA = (void*)x_alloc_aligned_afifo_mem(u4Size, 256);

    if (NULL == pBufSA)
    {
        SYS_Printf("[AUD][OMX(%d)] alloc AFIFO memory failed, %d bytes.\n",pHandle->h_handle, u4Size);
        goto lblErrorReturn;
    }
    else
    {
        pAvailBuf->pBufSA    = pBufSA;
        pAvailBuf->u4BufSize = u4Size;
    }

    SYS_Printf("[AUD][OMX(%d)] %s %d: x_kmem_create_handle\n", pHandle->h_handle, __FUNCTION__, __LINE__);
    hMemHandle = x_kmem_create_handle(pBufSA, u4Size);

    if (NULL_HANDLE == hMemHandle)
    {
        goto lblErrorReturn;
    }
    else
    {
        pAvailBuf->hBuffer = hMemHandle;
    }

    // fill buffer header
    x_memset(&(pAvailBuf->rHeader), 0x00, sizeof(OMX_BUFFERHEADERTYPE));

    pAvailBuf->rHeader.nSize             = sizeof(OMX_BUFFERHEADERTYPE);
    pAvailBuf->rHeader.nVersion.nVersion = OMX_VERSION;

    pAvailBuf->rHeader.pBuffer           = pBufSA;
    pAvailBuf->rHeader.nAllocLen         = u4Size;
    pAvailBuf->rHeader.nFilledLen        = 0;
    pAvailBuf->rHeader.nOffset           = 0;
    pAvailBuf->rHeader.nOutputPortIndex  = u4Port;

    // release semaphore
    i4AudOmxTaskUnlockSem();

    *ppBufInfo = pAvailBuf;
    return AUD_OK;

    // error handling
lblErrorReturn:
    // release semaphore
    i4AudOmxTaskUnlockSem();

    // resource release will be done on freeing component handle, not here
    *ppBufInfo = NULL;
    return AUD_FAIL;
}

INT32 i4AudOmxFreeInputBuf(AUD_OMX_HANDLE_T *pHandle, HANDLE_T hHeader)
{
    UINT32 u4Index;
    BOOL fgAllFreed = TRUE;
    AUD_OMX_BUFFER_T *pBufNode = NULL;

    if ((NULL == pHandle) || (NULL_HANDLE == hHeader))
    {
        return AUD_OK;
    }

    // multi-instance protection, take semaphore
    i4AudOmxTaskLockSem();

    for (u4Index = 0; u4Index < INPUT_BUF_MAX; u4Index ++)
    {
        if (hHeader == _prAudOmxInputBuf[pHandle->h_handle][u4Index].hHeader)
        {
            pBufNode = &(_prAudOmxInputBuf[pHandle->h_handle][u4Index]);
            break;
        }
    }

    if (NULL != pBufNode)
    {
        if ((pBufNode->pOwner == pHandle) && (NULL != pBufNode->pBufSA))
        {
            // free memory
            x_free_aligned_afifo_mem(pBufNode->pBufSA);

            // destory the buffer handle
            SYS_Printf("[AUD][OMX(%d)] %s %d: x_kmem_destroy_handle\n", pHandle->h_handle, __FUNCTION__, __LINE__);
            x_kmem_destroy_handle(pBufNode->hBuffer);

            // destory the header handle
            SYS_Printf("[AUD][OMX(%d)] %s %d: x_kmem_destroy_handle\n", pHandle->h_handle, __FUNCTION__, __LINE__);
            x_kmem_destroy_handle(pBufNode->hHeader);

            // clear info
            pBufNode->pOwner    = NULL;
            pBufNode->hBuffer   = NULL_HANDLE;
            pBufNode->pBufSA    = NULL;
            pBufNode->u4BufSize = 0;
            pBufNode->hHeader   = NULL_HANDLE;
        }
    }

    // check if all input buffers have been freed, then free the AFIFO memory
    for (u4Index = 0; u4Index < INPUT_BUF_MAX; u4Index ++)
    {
        if (pHandle == _prAudOmxInputBuf[pHandle->h_handle][u4Index].pOwner)
        {
            fgAllFreed = FALSE;
            break;
        }
    }

    if (fgAllFreed)
    {
        x_free_aligned_afifo_mem(pHandle->pFifoPntr);
        pHandle->pFifoPntr = NULL;
        pHandle->u4FifoSize = 0;
    }

    // release semaphore
    i4AudOmxTaskUnlockSem();

    return AUD_OK;
}

INT32 i4AudOmxQueueEmptyBuf(AUD_OMX_HANDLE_T *pHandle, HANDLE_T hHeader)
{
    INT32 i4Ret = AUD_FAIL;
    UINT32 u4Index;
    AUD_OMX_BUFFER_T *pBufNode = NULL;

    if ((NULL == pHandle) || (NULL_HANDLE == hHeader))
    {
        return AUD_OK;
    }

    // multi-instance protection, take semaphore
    i4AudOmxTaskLockSem();

    for (u4Index = 0; u4Index < INPUT_BUF_MAX; u4Index ++)
    {
        if (hHeader == _prAudOmxInputBuf[pHandle->h_handle][u4Index].hHeader)
        {
            pBufNode = &(_prAudOmxInputBuf[pHandle->h_handle][u4Index]);
            break;
        }
    }

    if (NULL != pBufNode)
    {
        if (pBufNode->pOwner == pHandle)
        {
            // fill current empty parameters (copy from client filled memory)
            pBufNode->u4FilledLen = pBufNode->rHeader.nFilledLen;
            pBufNode->u4Offset    = pBufNode->rHeader.nOffset;
            pBufNode->nTimeStamp  = pBufNode->rHeader.nTimeStamp;
            pBufNode->fgEmptied   = FALSE;
			pBufNode->fgCodecData = FALSE;

			if (pBufNode->rHeader.nFlags & OMX_BUFFERFLAG_CODECCONFIG)
			{
				Printf("[AUD][OMX(%d)] Queue Codec Config %d !\n", pHandle->h_handle, pHandle->u4EmptyQueueNo);
				pBufNode->fgCodecData = TRUE;
				pBufNode->rHeader.nFlags &= ~OMX_BUFFERFLAG_CODECCONFIG;
			} 

            // add to queue
            for (u4Index = 0; u4Index < INPUT_BUF_MAX; u4Index ++)
            {
                if (NULL == pHandle->pEmptyQ[u4Index])
                {                    
                    i4Ret = AUD_OK;
                    pHandle->fgEmptying = TRUE;
                    pHandle->pEmptyQ[u4Index] = pBufNode;
                    pHandle->u4EmptyQueueNo++;
                    pHandle->u4TotalEmptyQueueNo++;
                    if (u4AOmxDbgMsk & 0x4)
                    {
                        Printf("[AUD][OMX(%d)] Empty Queue(%d): %x %x %lld (%02x %02x %02x %02x)\n", pHandle->h_handle, 
                                pHandle->u4EmptyQueueNo,
                                pBufNode->pBufSA, 
                                pBufNode->u4FilledLen,
                                pBufNode->nTimeStamp,
                                ((UINT8*)(pBufNode->pBufSA))[0],
                                ((UINT8*)(pBufNode->pBufSA))[1],
                                ((UINT8*)(pBufNode->pBufSA))[2],
                                ((UINT8*)(pBufNode->pBufSA))[3]
                                );
                    }
                    break;
                }
            }

            if (AUD_OK != i4Ret)
            {
                if (u4AOmxDbgMsk & 0x4)
                {
                    //Printf("[AUD][OMX(%d)] Empty Queue full(%d) !!!!\n", pHandle->h_handle, pHandle->u4EmptyQueueNo);
                }
            }
        }
    }

    // release semaphore
    i4AudOmxTaskUnlockSem();

    return i4Ret;
}

#define OMX_AFIFO_GAP_SIZE   (0x100)
UINT32  gu4FillFifoDelay = 100;
static UINT32 u4AudOmxFillFIFO(AUD_OMX_HANDLE_T *pHandle, UINT32 u4From, UINT32 u4Size)
{
    UINT32 u4FifoSA, u4FifoEA;
    UINT32 u4FifoRp, u4FifoWp;
    INT32 i4FifoFree, i4Copied = 0;
    UINT32 u4FifoFullness; //Fill Fifo Debug

    if (0 == u4Size)
    {
        return 0;
    }

    u4FifoSA = (UINT32) pHandle->pFifoPntr;
    u4FifoEA = (UINT32) pHandle->pFifoPntr + pHandle->u4FifoSize;
    u4FifoWp = (UINT32) pHandle->pFifoWptr;
    u4FifoRp = u4AudHalGetBufRPtr(pHandle->u1DecId);

	if((u4FifoRp == u4FifoWp) && (u4FifoRp != u4FifoSA))
	{
		Printf("\n%s() u4FifoRp(%x) Line:%d\n", __FUNCTION__, u4FifoRp, __LINE__);
	}

	if((u4FifoRp < u4FifoSA) || (u4FifoRp >= u4FifoEA))
	{
		Printf("\n%s() u4FifoWp(%x) u4FifoRp(%x-%d) TotalSize(%d) Line:%d\n", 
			__FUNCTION__, u4FifoWp, u4FifoRp, pHandle->u1DecId,pHandle->u4TotalInputSize,__LINE__);
		return 0;
	}
    //SYS_Printf("[AUD][OMX(%d)] u4AudOmxFillFIFO: u4FifoSA = 0x%X, u4FifoEA = 0x%X, u4FifoWp = 0x%X, u4FifoRp = 0x%X\n", pHandle->h_handle,
    //           u4FifoSA, u4FifoEA, u4FifoWp, u4FifoRp);

    //Fill Fifo Debug
    if (u4FifoWp >= u4FifoRp)
    {
        u4FifoFullness = ((u4FifoEA - u4FifoSA) - (u4FifoWp - u4FifoRp));
    }
    else
    {
        u4FifoFullness = u4FifoRp - u4FifoWp;        
    }
    //End

    if (u4FifoRp >= u4FifoWp)
    {
        // |xxxxxxxx|--------------|xxxxxxxxxxxxxx|
        // SA       Wp             Rp             EA
        //          |--u4FifoFree--|
        if (u4FifoRp == u4FifoWp)
        {
            // if Wp == Rp == SA, it is the first time write FIFO
            // else if Wp == Rp, means FIFO is pure empty
            // it should not be FIFO full case, since we allways avoid filling FIFO to full
            i4FifoFree = u4FifoEA - u4FifoSA - OMX_AFIFO_GAP_SIZE; // - 8 to avoid Wp == Rp after updated
            SYS_Printf("[AUD][OMX(%d)] u4AudOmxFillFIFO: first time write or empty.\n", pHandle->h_handle);
        }
        else
        {
            i4FifoFree = u4FifoRp - u4FifoWp - OMX_AFIFO_GAP_SIZE; // - 8 to avoid Wp == Rp after updated
        }

        if (i4FifoFree <= 0)
        {
            // copy none
            i4Copied = 0;
        }
        else if (i4FifoFree >= (INT32)u4Size)
        {
            // copy all
            i4Copied = u4Size;
        }
        else
        {
            // copy part
            i4Copied = i4FifoFree;
        }

        if (i4Copied > 0)
        {
            // copy bitstream from input buffer to FIFO
            //20110622
            if (pHandle->eFormat == AUD_FMT_COOK)
            {
                INT32 i;
                for (i=0;i<i4Copied;i++)
                {
                    switch (pHandle->u2RaFrameByteIdx&0x3)
                    {
                    case 0:
                        *((UINT8*)(u4From+i)) ^= 0x37;
                        break;
                    case 1:
                        *((UINT8*)(u4From+i)) ^= 0xc5;
                        break;
                    case 2:
                        *((UINT8*)(u4From+i)) ^= 0x11;
                        break;
                    case 3:
                        *((UINT8*)(u4From+i)) ^= 0xf2;
                        break;
                    }
                    pHandle->u2RaFrameByteIdx++;
					if (pHandle->u1RaChanNum == 2 && pHandle->u2RaCplQBits == 0) 
					{
						if (pHandle->u2RaFrameByteIdx == pHandle->u2RaFrameSizeInBytes/2)
							pHandle->u2RaFrameByteIdx = 0;
					}
					else 
					{
						if (pHandle->u2RaFrameByteIdx == pHandle->u2RaFrameSizeInBytes)
							pHandle->u2RaFrameByteIdx = 0;
					}

                }
            }
            //20110622
            #if 0
            x_memcpy((void*)u4FifoWp, (const void*)u4From, i4Copied);
            if (u1AOMXDumpLocation == 0)
            {
                vAudOmxFillDumpBuf((void*)u4From, i4Copied);
            }

            // flush cache
            BSP_CleanDCacheRange(u4FifoWp, i4Copied);
			#else

			vAudOmxCopyData2Afifo(u4FifoWp, u4From, i4Copied);
			#endif
            u4FifoWp += i4Copied;
        }
    }
    else if (u4FifoRp < u4FifoWp)
    {
        // |--------|xxxxxxxxxxxxxx|--------------|
        // SA       Rp             Wp             EA
        //                         |--u4FifoFree--|

        if (u4FifoRp > u4FifoSA)
        {
            i4FifoFree = u4FifoEA - u4FifoWp;
        }
        else
        {
            // Rp == SA, - 8 to avoid Wp == Rp after updated
            i4FifoFree = u4FifoEA - u4FifoWp - OMX_AFIFO_GAP_SIZE;
        }

        if (i4FifoFree <= 0)
        {
            // copy none
            i4Copied = 0;        
        }
        else if (i4FifoFree >= (INT32)u4Size)
        {
            // copy all
            i4Copied = u4Size;
        }
        else
        {
            // copy part
            i4Copied = i4FifoFree;
        }

        if (i4Copied > 0)
        {
            // copy bitstream from input buffer to FIFO
            //20110622
            if (pHandle->eFormat == AUD_FMT_COOK)
            {
                INT32 i;
                for (i=0;i<i4Copied;i++)
                {
                    switch (pHandle->u2RaFrameByteIdx&0x3)
                    {
                    case 0:
                        *((UINT8*)(u4From+i)) ^= 0x37;
                        break;
                    case 1:
                        *((UINT8*)(u4From+i)) ^= 0xc5;
                        break;
                    case 2:
                        *((UINT8*)(u4From+i)) ^= 0x11;
                        break;
                    case 3:
                        *((UINT8*)(u4From+i)) ^= 0xf2;
                        break;
                    }
                    pHandle->u2RaFrameByteIdx++;
					if (pHandle->u1RaChanNum == 2 && pHandle->u2RaCplQBits == 0) 
					{
						if (pHandle->u2RaFrameByteIdx == pHandle->u2RaFrameSizeInBytes/2)
							pHandle->u2RaFrameByteIdx = 0;
					}
					else 
					{
						if (pHandle->u2RaFrameByteIdx == pHandle->u2RaFrameSizeInBytes)
							pHandle->u2RaFrameByteIdx = 0;
					}

                }
            }
            //20110622
            #if 0
            x_memcpy((void*)u4FifoWp, (const void*)u4From, i4Copied);
            if (u1AOMXDumpLocation == 0)
            {
                vAudOmxFillDumpBuf((void*)u4From, i4Copied);
            }            

            // flush cache
            BSP_CleanDCacheRange(u4FifoWp, i4Copied);
			#else
			vAudOmxCopyData2Afifo(u4FifoWp, u4From, i4Copied);
			#endif
            u4FifoWp += i4Copied;
        }

        // check if need roll back, make sure Wp != EA
        if (u4FifoWp == u4FifoEA)
        {
            u4FifoWp = u4FifoSA;
        }

        // check if need copy part 2
        if ((i4Copied < u4Size) && (u4FifoRp > u4FifoSA))
        {
            INT32 i4Copied2;

            // |--------|xxxxxxxxxxxxxx|--------------|
            // SA       Rp             Wp             EA
            // |--here--|

            i4FifoFree = u4FifoRp - u4FifoSA - OMX_AFIFO_GAP_SIZE;

            if (i4FifoFree <= 0)
            {
                // copy none
                i4Copied2 = 0;
            }
            else if (i4FifoFree >= (INT32)(u4Size - i4Copied))
            {
                // copy all
                i4Copied2 = u4Size - i4Copied;
            }
            else
            {
                // copy part
                i4Copied2 = i4FifoFree;
            }

            if (i4Copied2 > 0)
            {
                // copy bitstream from input buffer to FIFO
                //20110622
                if (pHandle->eFormat == AUD_FMT_COOK)
                {
                    INT32 i;
                    for (i=0;i<i4Copied2;i++)
                    {
                        switch (pHandle->u2RaFrameByteIdx&0x3)
                        {
                        case 0:
                            *((UINT8*)(u4From + i4Copied+i)) ^= 0x37;
                            break;
                        case 1:
                            *((UINT8*)(u4From + i4Copied+i)) ^= 0xc5;
                            break;
                        case 2:
                            *((UINT8*)(u4From + i4Copied+i)) ^= 0x11;
                            break;
                        case 3:
                            *((UINT8*)(u4From + i4Copied+i)) ^= 0xf2;
                            break;
                        }
                        pHandle->u2RaFrameByteIdx++;

						if (pHandle->u1RaChanNum == 2 && pHandle->u2RaCplQBits == 0) 
						{
							if (pHandle->u2RaFrameByteIdx == pHandle->u2RaFrameSizeInBytes/2)
								pHandle->u2RaFrameByteIdx = 0;
						}
                        else 
						{
							if (pHandle->u2RaFrameByteIdx == pHandle->u2RaFrameSizeInBytes)
                            	pHandle->u2RaFrameByteIdx = 0;
                        }
                    }
                }
                //20110622
				#if 0
                x_memcpy((void*)u4FifoSA, (const void*)(u4From + i4Copied), i4Copied2);
                if (u1AOMXDumpLocation == 0)
                {
                    vAudOmxFillDumpBuf((void*)(u4From+i4Copied), i4Copied2);
                }

                // flush cache
                BSP_CleanDCacheRange(u4FifoSA, i4Copied2);
				#else
				vAudOmxCopyData2Afifo(u4FifoSA, (u4From + i4Copied), i4Copied2);
				#endif
                u4FifoWp += i4Copied2;
                i4Copied += i4Copied2;
            }
        }
    }

    // check if need roll back
    if (u4FifoWp == u4FifoEA)
    {
        u4FifoWp = u4FifoSA;
    }

	HAL_Delay_us(gu4FillFifoDelay);
    // set new Wptr
    vAudHalSetBufWPtr(pHandle->u1DecId, u4FifoWp);
    pHandle->pFifoWptr = (void *)u4FifoWp;

    //SYS_Printf("[AUD][OMX(%d)] Fill FIFO: i4Copied = %4d, Wptr = 0x%X\n", pHandle->h_handle, i4Copied, u4FifoWp);

    //Fill Fifo Debug
    if (i4Copied > u4FifoFullness)
    {
        Printf("[AUD][OMX(%d)] u4AudOmxFillFIFO Fatal Error!!!!\n", pHandle->h_handle);
    }
    //End

    pHandle->u4TotalEmptySize += i4Copied;

	if(fgAOMXDumpDataEnable)
	{
		GST_AOMX_DumpAudioData(pHandle->u1DecId, (const char *) u4From, i4Copied, FALSE);
	}

    return i4Copied;
}

INT32 i4AudOmxDoEmptyBuf(AUD_OMX_HANDLE_T *pHandle)
{
    INT32 i4Ret = AUD_FAIL;
    UINT32 u4EmptySize = 0;
    UINT32 u4FifoRp;
    UINT32 u4Index;
    BOOL fgNeedEmpty = FALSE;
    BOOL fgNeedPTS = FALSE;
    AUD_OMX_BUFFER_T *pToEmptyBuf = NULL;
#ifdef AOMX_REAL_PTS    
    GST_AUDIO_PES_T rPes;
#endif

	static UINT32 u4CurWp = 0;
	static UINT32 u4LastWp = 0;
	UINT32 u4DataSize = 0;
	static UINT32 u4TotalSize = 0;
    // check if this buffer has been copied to FIFO
    for (u4Index = 0; u4Index < INPUT_BUF_MAX - 1; u4Index ++)
    {
        pToEmptyBuf = pHandle->pEmptyQ[u4Index];

        if (NULL != pToEmptyBuf)
        {
            if (!pToEmptyBuf->fgEmptied)
            {
                fgNeedEmpty = TRUE;
                break;
            }
        }
    }

    if (!fgNeedEmpty || pHandle->fgPauseTrigger)
    {
        return AUD_OK;
    }

	if (pToEmptyBuf->fgCodecData)
	{	
		Printf("[AUD][OMX(%d)] Get Codec Data \n", pHandle->h_handle);

		if (pHandle->eFormat == AUD_FMT_VORBISDEC)
		{
			// Store CodecData
			vStoreCodecData(pHandle, pToEmptyBuf->pBufSA + pToEmptyBuf->u4Offset, pToEmptyBuf->u4FilledLen);		
		}
        u4FifoRp = u4AudHalGetBufRPtr(pHandle->u1DecId);
        u4EmptySize = pToEmptyBuf->u4FilledLen;
		pToEmptyBuf->fgCodecData = FALSE;
		goto _DO_EMPTY_BUF_END;
	}

    //PTS_VALID
    //If the PTS are all -1 (audio only case), we still need to process it. Since the aout enable needs to be triggered by GST_SendAudioPes.
    if (pToEmptyBuf->nTimeStamp != 18446744073709551)
    {
        pHandle->fgGotValidPts = TRUE;
    }
    //END

    //FORMAT_NOT_SUPPORT 
    //No decoding, drop data!!!
    if (pHandle->fgFormatNotSupport || !(pHandle->fgGotValidPts))
    {
        u4FifoRp = u4AudHalGetBufRPtr(pHandle->u1DecId);
        u4EmptySize = pToEmptyBuf->u4FilledLen;                
        goto _DO_EMPTY_BUF_END;        
    }
    //END

    //PTS
    if (pToEmptyBuf->u4Offset == 0)
    {
        if ((pToEmptyBuf->u4FilledLen) && (!(pHandle->fgGotValidPts) || (pToEmptyBuf->nTimeStamp != 18446744073709551)))
        {
            if (pHandle->nOutputTimeStampCur == -1)
            {
				u4CurWp = PHYSICAL((UINT32)pHandle->pFifoPntr);
				u4TotalSize = 0;
                pHandle->nOutputTimeStampCur = pToEmptyBuf->nTimeStamp;
                Printf("[AUD][OMX(%d)] nOutputTimeStampCur init as: %lld\n", pHandle->h_handle, pHandle->nOutputTimeStampCur);
				if(fgAOMXDumpDataEnable)
				{
					GST_AOMX_DumpAudioData(pHandle->u1DecId, 0, 0, TRUE);
				}
#ifdef AOMX_REAL_PTS                
				if (!(u4AOmxDbgMsk & 0x800) && ((pHandle->u1DecId == AUD_DEC_MAIN) || (pHandle->u1DecId == AUD_DEC_AUX) || (pHandle->u1DecId == AUD_DEC_THIRD)))
                {
					GST_StopSTC(STC_SRC_A1);
					GST_SetSTCVal(STC_SRC_A1, (u8Div6432(pHandle->nOutputTimeStampCur * 9,100,NULL)));
                    //set start PTS
                    AUD_SetStartPts(pHandle->u1DecId,(UINT32)(u8Div6432(pHandle->nOutputTimeStampCur * 9,100,NULL)));         //convert 64bits -> 32bits
                    pHandle->u8LastRealPts = pToEmptyBuf->nTimeStamp;
                }
#endif                
            }

#ifdef AOMX_REAL_PTS            
			if (!(u4AOmxDbgMsk & 0x800) && ((pHandle->u1DecId == AUD_DEC_MAIN) || (pHandle->u1DecId == AUD_DEC_AUX) || (pHandle->u1DecId == AUD_DEC_THIRD)))
			{
				rPes.u8Pts = pToEmptyBuf->nTimeStamp; 
				rPes.u4Wp = PHYSICAL((UINT32) pHandle->pFifoWptr);

				u4LastWp = u4CurWp;
				u4CurWp = rPes.u4Wp;
				//Printf("[AUD][OMX(%d)] pts1 = %llx\n", pHandle->h_handle,pToEmptyBuf->nTimeStamp);
				u4DataSize = (u4CurWp >= u4LastWp)? (u4CurWp - u4LastWp):(u4CurWp + pHandle->u4FifoSize - u4LastWp);
				u4TotalSize += u4DataSize;
			}
#endif 
            if (u4AOmxDbgMsk & 0x2000)
            {
                Printf("[OMX(%d)]RP(%x)WP(%x-%x)(%x-%x)(0x%x) STC(%lld)InPTS(%lld)", pHandle->h_handle, 
					(u4GetABufPnt(AUD_DSP0, pHandle->u1DecId)&0xffffff), (u4LastWp&0xffffff), (u4CurWp&0xffffff), u4DataSize, 
					pToEmptyBuf->u4FilledLen ,u4TotalSize, GST_GetSTCVal(STC_SRC_A1),pToEmptyBuf->nTimeStamp);
                if (pHandle->nInputTimeStampLast != -1)
                {
                    Printf(" Diff: %lld\n", pToEmptyBuf->nTimeStamp - pHandle->nInputTimeStampLast);
                }
                else
                {
                    Printf("\n");
                }
            }
            pHandle->nInputTimeStampLast = pToEmptyBuf->nTimeStamp;

#ifdef AOMX_REAL_PTS
			if (!(u4AOmxDbgMsk & 0x800) && ((pHandle->u1DecId == AUD_DEC_MAIN) || (pHandle->u1DecId == AUD_DEC_AUX) || (pHandle->u1DecId == AUD_DEC_THIRD)))
            {
				rPes.u1DecId = pHandle->u1DecId;
                if (pHandle->eFormat != AUD_FMT_AAC)
                {
                    GST_SendAudioPes(rPes);
                }
                fgNeedPTS = TRUE;    
                #if 0
                if (u4AOmxDbgMsk & 0x10)
                {
                    Printf("[AUD][OMX(%d)] ori PTS = %lld, new PTS = %d\n", pHandle->h_handle, pToEmptyBuf->nTimeStamp, rPes.u4Pts);
                }
                #endif
            }
#endif
        }
    }
    //END

    // multi-instance protection, take semaphore
    i4AudOmxTaskLockSem();

    {
        u4FifoRp = u4AudHalGetBufRPtr(pHandle->u1DecId);

        if (pHandle->fgIsFirstChk)
        {
            ////////////////////////////////////////////////////////////
            //AAC Check
            //need check ID3V2: just simple checking header
            if ((pToEmptyBuf->u4Offset == 0) &&
                (pToEmptyBuf->u4FilledLen < MAX_INPUT_BUF_SIZE) &&
                (pHandle->eFormat == AUD_FMT_AAC) &&
                !((((UINT8*)(pToEmptyBuf->pBufSA))[0] == 0xff) && ((((UINT8*)(pToEmptyBuf->pBufSA))[1] & 0xf0) == 0xf0)) &&
                !((((UINT8*)(pToEmptyBuf->pBufSA))[0] == 0x56) && ((((UINT8*)(pToEmptyBuf->pBufSA))[1] & 0xf0) == 0xe0)) && 
                !((((UINT8*)(pToEmptyBuf->pBufSA))[0] == 0x49) && (((UINT8*)(pToEmptyBuf->pBufSA))[1] == 0x44) && (((UINT8*)(pToEmptyBuf->pBufSA))[2] == 0x33))
                )
            {
                if (!(pHandle->fgIsAacInTs)) //AAC_IN_TS must have ADTS header
                {
                    pHandle->fgEnableAdtsInsert = TRUE;
                    Printf("[AUD][OMX(%d)] Enable AAC ADTS insert !!!! (%02x %02x %02x %02x)\n", pHandle->h_handle, 
                        ((UINT8*)(pToEmptyBuf->pBufSA))[0],
                        ((UINT8*)(pToEmptyBuf->pBufSA))[1],
                        ((UINT8*)(pToEmptyBuf->pBufSA))[2],
                        ((UINT8*)(pToEmptyBuf->pBufSA))[3]);
                }

				i4AudOmxStartDualDecoding(pHandle);
        		AUD_DSPCmdPlay(AUD_DSP0, pHandle->u1DecId);
            }
            //End
            ////////////////////////////////////////////////////////////

            ////////////////////////////////////////////////////////////
            //VORBIS Check
            else if ((pToEmptyBuf->u4Offset == 0) &&
                (pToEmptyBuf->u4FilledLen < MAX_INPUT_BUF_SIZE) &&
                (pHandle->eFormat == AUD_FMT_VORBISDEC) &&
                !((((UINT8*)(pToEmptyBuf->pBufSA))[0] == 0x4F) && (((UINT8*)(pToEmptyBuf->pBufSA))[1]  == 0x67) && (((UINT8*)(pToEmptyBuf->pBufSA))[2]  == 0x67) && (((UINT8*)(pToEmptyBuf->pBufSA))[3]  == 0x53)))
            {
                pHandle->u4VorbisPage = 0;
                pHandle->fgEnableAdtsInsert = TRUE;
                Printf("[AUD][OMX(%d)] Enable Vorbis OggS insert !!!!\n", pHandle->h_handle);
				
				i4AudOmxStartDualDecoding(pHandle);
        		AUD_DSPCmdPlay(AUD_DSP0, pHandle->u1DecId);

				if (pHandle->pCodecData) 
				{				
					vParseVorbisHeader(pHandle, pHandle->pCodecData, pHandle->u4CodecDataSize);
					Printf("[AUD][OMX(%d)] Restore Vorbis headers.\n", pHandle->h_handle);
				} 
				else 
				{
					Printf("[AUD][OMX(%d)] WARNING : Vorbis data NOT start with headers!\n", pHandle->h_handle);
				}			
            }
            //End
            ////////////////////////////////////////////////////////////

            ////////////////////////////////////////////////////////////
            //LPCM Check
            else if ((pToEmptyBuf->u4Offset == 0) &&
                (pToEmptyBuf->u4FilledLen < MAX_INPUT_BUF_SIZE) &&
                (pToEmptyBuf->u4FilledLen > 3) &&
                (pHandle->eFormat == AUD_FMT_LPCM))
            {
                Printf("[AUD][OMX(%d)] LPCM delay play: %02x%02x%02x\n", pHandle->h_handle,
                            ((UINT8*)(pToEmptyBuf->pBufSA))[0],
                            ((UINT8*)(pToEmptyBuf->pBufSA))[1],
                            ((UINT8*)(pToEmptyBuf->pBufSA))[2]);
                if(((UINT8*)(pToEmptyBuf->pBufSA))[0]==0) // audio/x-private-ts-lpcm
                {
                    if (u2LPCM_CW)
                    {
                        AUD_SetLPCMFmtTS(pHandle->u1DecId,u2LPCM_CW);
                    }
                    else
                    {
                        AUD_SetLPCMFmtTS(pHandle->u1DecId,
                                        ((UINT16)((((UINT8*)(pToEmptyBuf->pBufSA))[1])<<8) | (((UINT8*)(pToEmptyBuf->pBufSA))[2])));
                    }
                }
                else if(((UINT8*)(pToEmptyBuf->pBufSA))[0]==1) // audio/x-private1-lpcm
                {
                    if (u2LPCM_CW)
                    {
                        AUD_SetLPCMFmtPS(pHandle->u1DecId,u2LPCM_CW);
                    }
                    else
                    {                
                        AUD_SetLPCMFmtPS(pHandle->u1DecId,
                                        ((UINT16)((((UINT8*)(pToEmptyBuf->pBufSA))[1])<<8) | (((UINT8*)(pToEmptyBuf->pBufSA))[2])));
                    }
                }
                else if(((UINT8*)(pToEmptyBuf->pBufSA))[0]==2) // audio/x-private-ts-dvd-lpcm
                {
                    if (u2LPCM_CW)
                    {
                        AUD_SetLPCMFmtWIFI(pHandle->u1DecId,u2LPCM_CW);
                    }
                    else
                    {                
                        AUD_SetLPCMFmtWIFI(pHandle->u1DecId,
                                        ((UINT16)((((UINT8*)(pToEmptyBuf->pBufSA))[1])<<8) | (((UINT8*)(pToEmptyBuf->pBufSA))[2])));
                    }
                }
				i4AudOmxStartDualDecoding(pHandle);
				AUD_DSPCmdPlay(AUD_DSP0, pHandle->u1DecId);

                pToEmptyBuf->u4Offset += 3;
                pToEmptyBuf->u4FilledLen -= 3;
            }
            // audio/x-private-ts-lpcm
            else if ((pToEmptyBuf->u4Offset == 0) &&
                (pToEmptyBuf->u4FilledLen < MAX_INPUT_BUF_SIZE) &&
                (pToEmptyBuf->u4FilledLen > 4) &&
                (pHandle->eFormat == AUD_FMT_OMX_LPCM_TS))
            {
                Printf("[AUD][OMX(%d)] LPCM_TS delay play: %02x%02x%02x\n", pHandle->h_handle,
                            ((UINT8*)(pToEmptyBuf->pBufSA))[1],
                            ((UINT8*)(pToEmptyBuf->pBufSA))[2],
                            ((UINT8*)(pToEmptyBuf->pBufSA))[3]);
                if (u2LPCM_CW)
                {
                    AUD_SetLPCMFmtTS(pHandle->u1DecId,u2LPCM_CW);
                }
                else
                {
                    AUD_SetLPCMFmtTS(pHandle->u1DecId,
                                    ((UINT16)((((UINT8*)(pToEmptyBuf->pBufSA))[2])<<8) | (((UINT8*)(pToEmptyBuf->pBufSA))[3])));
                }                
				i4AudOmxStartDualDecoding(pHandle);
				AUD_DSPCmdPlay(AUD_DSP0, pHandle->u1DecId);
            }
            // audio/x-private1-lpcm
            else if ((pToEmptyBuf->u4Offset == 0) &&
                (pToEmptyBuf->u4FilledLen < MAX_INPUT_BUF_SIZE) &&
                (pToEmptyBuf->u4FilledLen > 5) &&
                (pHandle->eFormat == AUD_FMT_OMX_LPCM_PRIVATE1))
            {
                Printf("[AUD][OMX(%d)] LPCM_PRIVATE1 delay play: %02x%02x%02x\n", pHandle->h_handle,
                            ((UINT8*)(pToEmptyBuf->pBufSA))[2],
                            ((UINT8*)(pToEmptyBuf->pBufSA))[3],
                            ((UINT8*)(pToEmptyBuf->pBufSA))[4]); 
                if (u2LPCM_CW)
                {
                    AUD_SetLPCMFmtPS(pHandle->u1DecId,u2LPCM_CW);
                }
                else
                {                
                    AUD_SetLPCMFmtPS(pHandle->u1DecId,
                                    ((UINT16)((((UINT8*)(pToEmptyBuf->pBufSA))[3])<<8) | (((UINT8*)(pToEmptyBuf->pBufSA))[4])));
                }             
				i4AudOmxStartDualDecoding(pHandle);
				AUD_DSPCmdPlay(AUD_DSP0, pHandle->u1DecId);
            }
            // audio/x-private-ts-dvd-lpcm
            else if ((pToEmptyBuf->u4Offset == 0) &&
                (pToEmptyBuf->u4FilledLen < MAX_INPUT_BUF_SIZE) &&
                (pToEmptyBuf->u4FilledLen > 4) &&
                (pHandle->eFormat == AUD_FMT_OMX_LPCM_TSDVD))
            {
                Printf("[AUD][OMX(%d)] LPCM_TSDVD delay play: %02x%02x%02x\n", pHandle->h_handle,
                            ((UINT8*)(pToEmptyBuf->pBufSA))[1],
                            ((UINT8*)(pToEmptyBuf->pBufSA))[2],
                            ((UINT8*)(pToEmptyBuf->pBufSA))[3]); 
                if (u2LPCM_CW)
                {
                    AUD_SetLPCMFmtWIFI(pHandle->u1DecId,u2LPCM_CW);
                }
                else
                {                
                    AUD_SetLPCMFmtWIFI(pHandle->u1DecId,
                                    ((UINT16)((((UINT8*)(pToEmptyBuf->pBufSA))[2])<<8) | (((UINT8*)(pToEmptyBuf->pBufSA))[3])));
                }          
				i4AudOmxStartDualDecoding(pHandle);
				AUD_DSPCmdPlay(AUD_DSP0, pHandle->u1DecId);
            }
            //End
            ////////////////////////////////////////////////////////////

            ////////////////////////////////////////////////////////////
            //DTS Check
            else if ((pToEmptyBuf->u4Offset == 0) &&
                //(pToEmptyBuf->u4FilledLen < MAX_INPUT_BUF_SIZE) &&
                (pToEmptyBuf->u4FilledLen > 4) &&
                (pHandle->eFormat == AUD_FMT_DTS))
            {
                Printf("[AUD][OMX(%d)] DTS delay play: %02x%02x%02x%02x\n", pHandle->h_handle,
                            ((UINT8*)(pToEmptyBuf->pBufSA))[0],
                            ((UINT8*)(pToEmptyBuf->pBufSA))[1],
                            ((UINT8*)(pToEmptyBuf->pBufSA))[2],
                            ((UINT8*)(pToEmptyBuf->pBufSA))[3]);
                {
                    UINT32 i;
                    UINT8* pBuf = (UINT8*)(pToEmptyBuf->pBufSA);
                    for (i=0;i<pToEmptyBuf->u4FilledLen;i+=4,pBuf+=4)
                    {
                        if (((pBuf[0] == 0x7f) && (pBuf[1] == 0xfe) && (pBuf[2] == 0x80) && (pBuf[3] == 0x01)) ||
                           ((pBuf[0] == 0x1f) && (pBuf[1] == 0xff) && (pBuf[2] == 0xe8) && (pBuf[3] == 0x00)))
                        {
                            AUD_SetDTSInfo(DEC_BIG_ENDIAN);
                            break;
                        }
                        else if (((pBuf[0] == 0xfe) && (pBuf[1] == 0x7f) && (pBuf[2] == 0x01) && (pBuf[3] == 0x80)) ||
                               ((pBuf[0] == 0xff) && (pBuf[1] == 0x1f) && (pBuf[2] == 0x00) && (pBuf[3] == 0xe8)))
                        {
                            AUD_SetDTSInfo(DEC_LITTLE_ENDIAN);
                            break;
                        }                            
                    }
                }
				i4AudOmxStartDualDecoding(pHandle);
                AUD_DSPCmdPlay(AUD_DSP0, pHandle->u1DecId);
            }
            //End
            ////////////////////////////////////////////////////////////

            ////////////////////////////////////////////////////////////
            //MP12/MP3 Check
            else if ((pToEmptyBuf->u4Offset == 0) &&
                //(pToEmptyBuf->u4FilledLen < MAX_INPUT_BUF_SIZE) &&
                (pToEmptyBuf->u4FilledLen > 4) &&
                ((pHandle->eFormat == AUD_FMT_MPEG) || (pHandle->eFormat == AUD_FMT_MP3)))
            {
                Printf("[AUD][OMX(%d)] MPEG/MP3 delay play: %02x%02x%02x%02x\n", pHandle->h_handle,
                            ((UINT8*)(pToEmptyBuf->pBufSA))[0],
                            ((UINT8*)(pToEmptyBuf->pBufSA))[1],
                            ((UINT8*)(pToEmptyBuf->pBufSA))[2],
                            ((UINT8*)(pToEmptyBuf->pBufSA))[3]);

                if ((((UINT8*)(pToEmptyBuf->pBufSA))[0] == 0xff) &&
                    ((((UINT8*)(pToEmptyBuf->pBufSA))[1]&0xe0)==0xe0) &&
                    ((((UINT8*)(pToEmptyBuf->pBufSA))[2]&0xf0)!=0xf0) &&
                    ((((UINT8*)(pToEmptyBuf->pBufSA))[2]&0x0c)!=0x0c))
                {
                    //FFFx or FFEx
                    switch (((UINT8*)(pToEmptyBuf->pBufSA))[1]&0x6)
                    {
                        case 0x2: // 01b
                            //MP3
                            if (pHandle->eFormat == AUD_FMT_MPEG)
                            {
                                Printf("[AUD][OMX(%d)] MP12 change to MP3\n", pHandle->h_handle);
                                pHandle->eFormat = AUD_FMT_MP3;                               
#ifdef AOMX_REAL_PTS                                
                                if(!(u4AOmxDbgMsk & 0x800) && ((pHandle->u1DecId == AUD_DEC_MAIN) || (pHandle->u1DecId == AUD_DEC_AUX) || (pHandle->u1DecId == AUD_DEC_THIRD)))
                                {
                                    AUD_SetDecType(AUD_DSP0, pHandle->u1DecId, AUD_STREAM_FROM_GST, AUD_FMT_MP3);
									i4AudOmxSetDualDecoding(pHandle);
                                }
                                else
#endif                                    
                                {
                                    AUD_SetDecType(AUD_DSP0, pHandle->u1DecId, AUD_STREAM_FROM_MEMORY, AUD_FMT_MP3);
                                }
                            }                           
                            break;
                        case 0x4: // 10b
                        case 0x6: // 11b
                            //MP12
                            if (pHandle->eFormat == AUD_FMT_MP3)
                            {
                                Printf("[AUD][OMX(%d)] MP3 change to MP12\n", pHandle->h_handle);
                                pHandle->eFormat = AUD_FMT_MPEG;
#ifdef AOMX_REAL_PTS                                
								if(!(u4AOmxDbgMsk & 0x800) && ((pHandle->u1DecId == AUD_DEC_MAIN) || (pHandle->u1DecId == AUD_DEC_AUX) || (pHandle->u1DecId == AUD_DEC_THIRD)))
                                {
                                    AUD_SetDecType(AUD_DSP0, pHandle->u1DecId, AUD_STREAM_FROM_GST, AUD_FMT_MPEG);
									i4AudOmxSetDualDecoding(pHandle);
                                }
                                else
#endif
                                {
                                    AUD_SetDecType(AUD_DSP0, pHandle->u1DecId, AUD_STREAM_FROM_MEMORY, AUD_FMT_MPEG);
                                }
                            }    
                            break;                
                        default:
                            break;                            
                    }
                }
				i4AudOmxStartDualDecoding(pHandle);
                AUD_DSPCmdPlay(AUD_DSP0, pHandle->u1DecId);
            }
			else  // other format.
			{

            }
            //End
            ////////////////////////////////////////////////////////////

            pHandle->fgIsFirstChk = FALSE;
        }

        if((pToEmptyBuf->u4Offset == 0) &&
            (pToEmptyBuf->u4FilledLen < MAX_INPUT_BUF_SIZE) &&
            (pToEmptyBuf->u4FilledLen > 4) &&
            ((pHandle->eFormat == AUD_FMT_OMX_LPCM_TS) || 
            (pHandle->eFormat == AUD_FMT_OMX_LPCM_TSDVD)))
        {
            pToEmptyBuf->u4Offset += 4;
            pToEmptyBuf->u4FilledLen -= 4;
        }
        else if((pToEmptyBuf->u4Offset == 0) &&
            (pToEmptyBuf->u4FilledLen < MAX_INPUT_BUF_SIZE) &&
            (pToEmptyBuf->u4FilledLen > 5) &&
            (pHandle->eFormat == AUD_FMT_OMX_LPCM_PRIVATE1))
        {
            pToEmptyBuf->u4Offset += 5;
            pToEmptyBuf->u4FilledLen -= 5;
        }

        if (pHandle->fgEnableAdtsInsert && 
            (pToEmptyBuf->u4FilledLen != 0) && 
            (pToEmptyBuf->u4Offset == 0) && 
            (pHandle->u4AudHdrSize==0))
        {
            if ((pToEmptyBuf->u4FilledLen != MAX_INPUT_BUF_SIZE) &&
                (pHandle->eFormat == AUD_FMT_AAC) &&
                !((((UINT8*)(pToEmptyBuf->pBufSA))[0] == 0xff) && ((((UINT8*)(pToEmptyBuf->pBufSA))[1] & 0xf0) == 0xf0)))
            {
                pHandle->u1AAC_ADTS[0] = 0xff;
                pHandle->u1AAC_ADTS[1] = 0xf9;
                pHandle->u1AAC_ADTS[2] = (1<<6) | ((pHandle->u1AacSampleRateIdx<<2) & 0x3c) | ((pHandle->u1AacChNo>>2) & 0x1);
                pHandle->u1AAC_ADTS[3] = ((pHandle->u1AacChNo & 0x3)<<6) | (((pToEmptyBuf->u4FilledLen + SWDMX_AVI_AAC_FAKE_HDR_SIZE)>>11)&0x3);
                pHandle->u1AAC_ADTS[4] = (((pToEmptyBuf->u4FilledLen + SWDMX_AVI_AAC_FAKE_HDR_SIZE)>> 3) & 0xff);
                pHandle->u1AAC_ADTS[5] = (((pToEmptyBuf->u4FilledLen + SWDMX_AVI_AAC_FAKE_HDR_SIZE)<<5) & 0xE0) | ((0x7FF>>6)&0x1F);
                pHandle->u1AAC_ADTS[6] = ((0x7FF<<2)&0xFC);
                
                pHandle->u4AudHdrOffset = 0;
                pHandle->u4AudHdrSize = 7;
            }

            if ((pToEmptyBuf->u4FilledLen != MAX_INPUT_BUF_SIZE) &&
                (pHandle->eFormat == AUD_FMT_VORBISDEC) &&
                !((((UINT8*)(pToEmptyBuf->pBufSA))[0] == 0x4F) && (((UINT8*)(pToEmptyBuf->pBufSA))[1]  == 0x67) && (((UINT8*)(pToEmptyBuf->pBufSA))[2]  == 0x67) && (((UINT8*)(pToEmptyBuf->pBufSA))[3]  == 0x53)))
            {
                UINT32 u4EmptySize1;
                UINT8 u1Temp;

                ///"OggS"
                pHandle->u1VORBIS_OggS[0] = 0x4F;
                pHandle->u1VORBIS_OggS[1] = 0x67;
                pHandle->u1VORBIS_OggS[2] = 0x67;
                pHandle->u1VORBIS_OggS[3] = 0x53;
                ///"Version" must be 0x00
                pHandle->u1VORBIS_OggS[4] = 0x00;
                ///"Type": =1 continue page, =2 BOS, =4 BOE.
                pHandle->u1VORBIS_OggS[5] = 0x00;
                ///"Granule=8, S/N=4, Page no=4, CRC=4" bytes
                for(u1Temp=6;u1Temp<26;u1Temp++)
                {
                    pHandle->u1VORBIS_OggS[u1Temp]=0;
                }

                /// Set "Page" Information
                pHandle->u1VORBIS_OggS[18] = (UINT8)(pHandle->u4VorbisPage & 0xFF);
                pHandle->u1VORBIS_OggS[19] = (UINT8)((pHandle->u4VorbisPage>>8) & 0xFF);
                pHandle->u1VORBIS_OggS[20] = (UINT8)((pHandle->u4VorbisPage>>16) & 0xFF);
                pHandle->u1VORBIS_OggS[21] = (UINT8)((pHandle->u4VorbisPage>>24) & 0xFF);
                pHandle->u4VorbisPage += 1;

                if((pToEmptyBuf->u4FilledLen)<= 0xFF)
                {
                    u1Temp = 1;     /// Only one segment
                    pHandle->u1VORBIS_OggS[26] = 0x01;
                    pHandle->u1VORBIS_OggS[27] = (pToEmptyBuf->u4FilledLen);
                }
                else
                {
                    u1Temp = 1;     /// Need accumulate segments number.
                    u4EmptySize1 = (pToEmptyBuf->u4FilledLen);
                    while(u4EmptySize1 > 0xFF)
                    {
                       u4EmptySize1 -= 0xFF;
                       /// Set Segments table
                       pHandle->u1VORBIS_OggS[26+u1Temp] = 0xFF;  ///[26+1]~[26+255]
                       u1Temp ++;
                    }
                    /// "Segments Number"=1
                    pHandle->u1VORBIS_OggS[26] = u1Temp;
                    /// Last segment of "Segment Table"
                    pHandle->u1VORBIS_OggS[26+u1Temp] = u4EmptySize1;    ///Update last segment table.
                }

                pHandle->u4AudHdrOffset = 0;
                pHandle->u4AudHdrSize = (27 + u1Temp);

                #ifdef VORBIS_DBG
                if((pHandle->eFormat == AUD_FMT_VORBISDEC) && (pHandle->u4VorbisPage<10))
                {
                    Printf("@@@@@@@@@@@@@@@@@@@@@\n");
                    Printf("pHandle->eFormat = 0x%x\n", (UINT32)(pHandle->eFormat));
                    Printf("pToEmptyBuf->u4FilledLen = 0x%x\n", (UINT32)(pToEmptyBuf->u4FilledLen));
                    Printf("pToEmptyBuf->u4Offset = 0x%x\n", (UINT32)(pToEmptyBuf->u4Offset));
                    Printf("@@@@@@@@@@@@@@@@@@@@@\n");

                    Printf("OggS Head Bytes: 0x%x\n", u1Temp);  ///u1Temp = u4LeftSize1

                    /// ### Printf OggS Header and +5 Segments table
                    Printf("OggS-START !!!\n");
                    for(u1Temp=0;u1Temp<(27+5);u1Temp++)
                    {
                        Printf("0x%x, ", pHandle->u1VORBIS_OggS[u1Temp]);
                    }
                    Printf("\nOggS-END !!!\n");
                    /// ### Printf OggS Header End
                }
                #endif
            }
        }

        if (pHandle->u4AudHdrSize)
        {
            UINT32 u4SendSize;
            if (pHandle->eFormat == AUD_FMT_AAC)
            {
                u4SendSize = u4AudOmxFillFIFO(pHandle, (UINT32)(pHandle->u1AAC_ADTS)+pHandle->u4AudHdrOffset, pHandle->u4AudHdrSize);
                pHandle->u4AudHdrOffset += u4SendSize;
                pHandle->u4AudHdrSize -= u4SendSize;
            }
            else if (pHandle->eFormat == AUD_FMT_VORBISDEC)
            {
                u4SendSize = u4AudOmxFillFIFO(pHandle, (UINT32)(pHandle->u1VORBIS_OggS)+pHandle->u4AudHdrOffset, pHandle->u4AudHdrSize);
                pHandle->u4AudHdrOffset += u4SendSize;
                pHandle->u4AudHdrSize -= u4SendSize;
            }
        }
        
        if (!pHandle->u4AudHdrSize)
        {
            u4EmptySize = u4AudOmxFillFIFO(pHandle, (UINT32) pToEmptyBuf->pBufSA + pToEmptyBuf->u4Offset, pToEmptyBuf->u4FilledLen);
        }
        if ((pHandle->eFormat == AUD_FMT_AAC) && fgNeedPTS)
        {
           rPes.u4Wp = PHYSICAL((UINT32) pHandle->pFifoWptr);	
           GST_SendAudioPes(rPes);	//queue PTS after WP is move to next frame		
        }         
    }
    ASSERT(u4EmptySize <= pToEmptyBuf->u4FilledLen);

_DO_EMPTY_BUF_END:
    pToEmptyBuf->u4Offset += u4EmptySize;
    pToEmptyBuf->u4FilledLen -= u4EmptySize;
    pHandle->u4TotalInputSize += u4EmptySize;
        
    if (0 == pToEmptyBuf->u4FilledLen)
    {
        // if original buffer filled len is 0, set this flag means it has been processed (althrough
        // it does not contain valid bitstream data, it may bring some flags, e.g. EOS flag)
        pToEmptyBuf->fgEmptied = TRUE;

        // this buffer is full emptied, now save current FIFO Rptr & Wptr
        // when Rptr is over this Wptr later, this buffer is really consumed, we will notify to client
        pToEmptyBuf->u4FifoRp = u4FifoRp;
        pToEmptyBuf->u4FifoWp = (UINT32) pHandle->pFifoWptr;
        pToEmptyBuf->u4FifoFreezeCnt = 0;

        // check if contains EOS flag
        if (OMX_BUFFERFLAG_EOS & pToEmptyBuf->rHeader.nFlags)
        {
            Printf("[AUD][OMX(%d)] EOS flag received\n", pHandle->h_handle);
            pHandle->fgGotEOS = TRUE;
            pToEmptyBuf->rHeader.nFlags &= ~OMX_BUFFERFLAG_EOS;
        }
    }

    // release semaphore
    i4AudOmxTaskUnlockSem();

    return i4Ret;
}

INT32 i4AudOmxCheckBufEmptyDone(AUD_OMX_HANDLE_T *pHandle)
{
    INT32 i4Ret = AUD_FAIL;
    UINT32 u4Index;
    BOOL fgAllEmptied = TRUE;
    AUD_OMX_BUFFER_T *pToEmptyBuf = NULL;

    // always left aligned
    pToEmptyBuf = pHandle->pEmptyQ[0];

    if (NULL == pToEmptyBuf)
    {
        // error handling
        pHandle->fgEmptying = FALSE;

        return AUD_OK;
    }

    if (pToEmptyBuf->u4FilledLen > 0)
    {
        // still not full copied to AFIFO
        return AUD_OK;
    }

    // multi-instance protection, take semaphore
    i4AudOmxTaskLockSem();

    {        
        // if empty is done, align queue to left and check if there is pending empty event
        for (u4Index = 0; u4Index < INPUT_BUF_MAX - 1; u4Index ++)
        {
            pHandle->pEmptyQ[u4Index] = pHandle->pEmptyQ[u4Index + 1];

            if (NULL != pHandle->pEmptyQ[u4Index])
            {
                fgAllEmptied = FALSE;
            }
        }

        // clear the tail node
        pHandle->pEmptyQ[INPUT_BUF_MAX - 1] = NULL;

        if (fgAllEmptied)
        {
            // empty queue is empty
            pHandle->fgEmptying = FALSE;
        }

        // if empty is done, callback now
        i4AudOmxCompNotifyEmptyDone(pHandle, pToEmptyBuf);

        pHandle->u4EmptyQueueNo--;
        pHandle->u4TotalEmptyQueueDoneNo++;
        if (u4AOmxDbgMsk & 0x4)
        {
            //Printf("[AUD][OMX(%d)] Empty Queue Done(%d)\n", pHandle->h_handle, pHandle->u4EmptyQueueNo);
        }
    }

    // release semaphore
    i4AudOmxTaskUnlockSem();

    return i4Ret;
}

INT32 i4AudOmxForceAllBufEmptyDone(AUD_OMX_HANDLE_T *pHandle)
{
    INT32 i4Ret = AUD_FAIL;
    UINT32 u4Index;
    AUD_OMX_BUFFER_T *pToEmptyBuf = NULL;

    // multi-instance protection, take semaphore
    i4AudOmxTaskLockSem();

    if (u4AOmxDbgMsk & 0x40)
    {
        Printf("[AUD][OMX(%d)] i4AudOmxForceAllBufEmptyDone\n", pHandle->h_handle);
    }
    SYS_Printf("[AUD][OMX(%d)] i4AudOmxForceAllBufEmptyDone\n", pHandle->h_handle);

    for (u4Index = 0; u4Index < INPUT_BUF_MAX; u4Index ++)
    {
        pToEmptyBuf = pHandle->pEmptyQ[u4Index];

        if (NULL != pToEmptyBuf)
        {
            i4AudOmxCompNotifyEmptyDone(pHandle, pToEmptyBuf);

            pHandle->u4EmptyQueueNo--;
            pHandle->u4TotalEmptyQueueDoneNo++;
            if (u4AOmxDbgMsk & 0x4)
            {
                //Printf("[AUD][OMX(%d)] Empty Queue Done(%d)\n", pHandle->h_handle, pHandle->u4EmptyQueueNo);
            }
            
            // clear
            pHandle->pEmptyQ[u4Index] = NULL;
        }
    }

    pHandle->fgEmptying = FALSE;

    // release semaphore
    i4AudOmxTaskUnlockSem();

    return i4Ret;
}

BOOL fgAudOmxAllInputBufferFreed(AUD_OMX_HANDLE_T *pHandle)
{
    UINT32 u4Index;
    BOOL fgAllFreed = TRUE;

    // multi-instance protection, take semaphore
    i4AudOmxTaskLockSem();

    for (u4Index = 0; u4Index < INPUT_BUF_MAX; u4Index ++)
    {
        if (pHandle == _prAudOmxInputBuf[pHandle->h_handle][u4Index].pOwner)
        {
            fgAllFreed = FALSE;
            break;
        }
    }

    // release semaphore
    i4AudOmxTaskUnlockSem();

    return fgAllFreed;
}

BOOL fgAudOmxAllOutputBufferFreed(AUD_OMX_HANDLE_T *pHandle)
{
    UINT32 u4Index;
    BOOL fgAllFreed = TRUE;

    // multi-instance protection, take semaphore
    i4AudOmxTaskLockSem();

    for (u4Index = 0; u4Index < OUTPUT_BUF_MAX; u4Index ++)
    {
        if (pHandle == _prAudOmxOutputBuf[pHandle->h_handle][u4Index].pOwner)
        {
            fgAllFreed = FALSE;
            break;
        }
    }

    // release semaphore
    i4AudOmxTaskUnlockSem();

    return fgAllFreed;
}

INT32 i4AudOmxPrepareDecoding(AUD_OMX_HANDLE_T *pHandle)
{
    AUD_FMT_T eFormatDecoder = pHandle->eFormat;
	
	//To avoid audio decoder not yet stop before AUD_SetDecType
	i4AudOmxStopDualDecoding(pHandle);
	AUD_DSPCmdStop(AUD_DSP0, pHandle->u1DecId);
	
    switch (pHandle->eFormat)
    {
        default:
            break;
        case AUD_FMT_PCM:
        case AUD_FMT_OMX_MSADPCM:
        case AUD_FMT_OMX_ULAW:
        case AUD_FMT_OMX_ALAW: 
        case AUD_FMT_OMX_LPCM_TS:
        case AUD_FMT_OMX_LPCM_TSDVD: 
        case AUD_FMT_OMX_LPCM_PRIVATE1: 
            eFormatDecoder = AUD_FMT_LPCM;
            break;
        case AUD_FMT_WMA10LOSSLESS:
        case AUD_FMT_WMA10SPEECH:
            eFormatDecoder = AUD_FMT_LPCM;
            break;
    }
	
#if 0	
#if (AUD_COMP_MAX==1)
    AUD_DSPCmdStop(AUD_DEC_AUX);
#endif    
#ifdef CC_MT5391_AUD_3_DECODER
    AUD_DSPCmdStop(AUD_DEC_THIRD);
#endif
#ifdef CC_AUD_4_DECODER_SUPPORT
    AUD_DSPCmdStop(AUD_DEC_4TH);
#endif
#endif
    //End
#ifdef AOMX_REAL_PTS    
    if (!(u4AOmxDbgMsk & 0x800))
    {	
        AUD_SetDecType(AUD_DSP0, pHandle->u1DecId, AUD_STREAM_FROM_GST, eFormatDecoder);
		i4AudOmxSetDualDecoding(pHandle);
        GST_MMQueueSyncInfo(pHandle->u1DecId,0,0,TRUE,0); //Reset MM Sync info
        pHandle->u8LastRealPts = -1;
    }
    else
#endif        
    {
        AUD_SetDecType(AUD_DSP0, pHandle->u1DecId, AUD_STREAM_FROM_MEMORY, eFormatDecoder);
    }

    return 0;
}

// Allocate FIFO according to format type.
INT32 i4AudOmxAllocateAudioFifo(AUD_OMX_HANDLE_T *pHandle)
{
    pHandle->pFifoPntr  = (void*)(VIRTUAL(u4GetAFIFOStart(AUD_DSP0, pHandle->u1DecId)));
    pHandle->u4FifoSize = ((UINT32)(VIRTUAL(u4GetAFIFOEnd(AUD_DSP0, pHandle->u1DecId))) - (UINT32)(VIRTUAL(u4GetAFIFOStart(AUD_DSP0, pHandle->u1DecId))));
    SYS_Printf("[AUD][OMX(%d)] Allocated FIFO SA = 0x%X, Size = %d bytes.\n", pHandle->h_handle, (UINT32)(pHandle->pFifoPntr), pHandle->u4FifoSize);

    return 0;
}

INT32 i4AudOmxStartDecoding(AUD_OMX_HANDLE_T *pHandle, void *pFifoSA, UINT32 u4FifoSz)
{

#if 0	
#if (AUD_COMP_MAX==1)
    AUD_DSPCmdStop(AUD_DEC_AUX);
#endif
#ifdef CC_MT5391_AUD_3_DECODERs
    AUD_DSPCmdStop(AUD_DEC_THIRD);
#endif
#ifdef CC_AUD_4_DECODER_SUPPORT
    AUD_DSPCmdStop(AUD_DEC_4TH);
#endif
#endif

    //AUD_DspIECConfig(AUD_IEC_CFG_PCM, TRUE);
    //AUD_LineInCtrlEnable(pHandle->u1DecId, FALSE);
   
    prev_av_sync_mode[pHandle->h_handle] = AUD_GetAvSynMode(pHandle->u1DecId);
    AUD_SetAvSynMode(pHandle->u1DecId, AV_SYNC_AUDIO_MASTER);
	//AUD_DspSetStcId(pHandle->u1DecId, (UINT16)STC_SRC_A1);

	// Disable PCR update STC
	STC_SetCtrl(STC_SRC_A1, STC_CTRL_NOT_UPDATE_ASTC, TRUE); 

    //DDT line-mode
#if (AUD_COMP_MAX>1)
    if (pHandle->u1DecId == AUD_DEC_MAIN)
#endif
    {
        AUD_DspDdtDmxRfMode(0,0,FALSE);
    }

    //Speaker Config
#if (AUD_COMP_MAX>1)    
    if (pHandle->u1DecId == AUD_DEC_MAIN)
#endif        
    {
        u1SpkCfg = AUD_DspGetSpeakerOutputConfig(0);
        AUD_DspSpeakerOutputConfig(0, 0);
    }

    //AOUT2 mix to AOUT1 config
#if (AUD_COMP_MAX>1)
    if (pHandle->u1DecId == AUD_DEC_AUX)
    {
        //AUD_SetAout2MixAout1(0x1f);
    }
#endif

    //Decoder Interface
    //RA
    pHandle->u2RaFrameByteIdx = 0;

    pHandle->fgIsFirstChk = TRUE;
    pHandle->fgEnableAdtsInsert = FALSE;

    //PTS
    pHandle->nInputTimeStampLast = -1;
    pHandle->nOutputTimeStampCur = -1;
    pHandle->nOutputTimeStampLast = -1;    
    pHandle->u4BankNumPrev = 0;
    pHandle->i4PTSBankNumPrev = 0;

   //End    
    pHandle->fgGotValidPts = FALSE;    
    pHandle->fgAoutEnable = FALSE;
    pHandle->fgRenderTrigger = FALSE;
    pHandle->fgPauseTrigger = FALSE;    

    pHandle->u4FillQueueNo = 0;
    pHandle->u4EmptyQueueNo = 0;
    pHandle->u4TotalFillQueueDoneNo = 0;
    pHandle->u4TotalEmptyQueueDoneNo = 0;    
    pHandle->u4TotalFillQueueNo = 0;
    pHandle->u4TotalEmptyQueueNo = 0;
    pHandle->u4TotalEmptySize = 0;
    pHandle->u4TotalInputSize = 0;

    // set Wptr
    pHandle->pFifoWptr = pFifoSA;
    vAudHalSetBufWPtr(pHandle->u1DecId, (UINT32)(pHandle->pFifoPntr));

    //ERROR_CNT_MONITOR
    do_gettimeofday(&pHandle->error_cnt_timer);

    //UNDERRUN_CNT_MONITOR
    do_gettimeofday(&pHandle->underrun_cnt_timer);
    pHandle->u4PrevInputUnderrunCnt = 0;
    pHandle->u4InputUnderrunDisableTimer = INPUT_UNDERRUN_DISABLE_TIMER;
    //END

    if ((pHandle->eFormat == AUD_FMT_LPCM) ||
        (pHandle->eFormat == AUD_FMT_OMX_LPCM_TS) ||
        (pHandle->eFormat == AUD_FMT_OMX_LPCM_TSDVD) ||
        (pHandle->eFormat == AUD_FMT_OMX_LPCM_PRIVATE1))
    {
        Printf("[AUD][OMX(%d)] AUD_FMT_LPCM(%d): delay play wait 5 bytes header to do config\n", pHandle->h_handle, pHandle->eFormat);
    }
    else if (pHandle->eFormat == AUD_FMT_DTS)
    {
        Printf("[AUD][OMX(%d)] AUD_FMT_DTS: delay play wait sync word parsing for big or little endian\n", pHandle->h_handle);
    }
    else if ((pHandle->eFormat == AUD_FMT_MPEG) ||
            (pHandle->eFormat == AUD_FMT_MP3))
    {
        Printf("[AUD][OMX(%d)] AUD_FMT_MPEG/AUD_FMT_MP3: delay play wait sync word parsing\n", pHandle->h_handle);
    } 
	else if (pHandle->eFormat == AUD_FMT_VORBISDEC)
	{
        Printf("[AUD][OMX(%d)] AUD_FMT_VORBISDEC : delay play wait header parsing\n", pHandle->h_handle);
	}
    else
    {
		i4AudOmxStartDualDecoding(pHandle);
        AUD_DSPCmdPlay(AUD_DSP0, pHandle->u1DecId);
    }

    return 0;
}

INT32 i4AudOmxStopDecoding(AUD_OMX_HANDLE_T *pHandle)
{
	GST_StopSTC(STC_SRC_A1);
    AUD_DSPCmdStop(AUD_DSP0, pHandle->u1DecId);
	i4AudOmxStopDualDecoding(pHandle);
    AUD_SetAvSynMode(pHandle->u1DecId, prev_av_sync_mode[pHandle->h_handle]);

	vDestroyCodecData(pHandle);

	// Enable PCR update STC
	STC_SetCtrl(STC_SRC_A1, STC_CTRL_NOT_UPDATE_ASTC, FALSE); 

    //DDT RF-mode
#if (AUD_COMP_MAX>1)
    if (pHandle->u1DecId == AUD_DEC_MAIN)
#endif
    {
        AUD_DspDdtDmxRfMode(1,0,FALSE);
    }

    //Speaker Config
#if (AUD_COMP_MAX>1)
    if (pHandle->u1DecId == AUD_DEC_MAIN)
#endif
    {
        AUD_DspSpeakerOutputConfig(0, u1SpkCfg);
    }

    //AOUT2 mix to AOUT1 config
#if (AUD_COMP_MAX>1)
    if (pHandle->u1DecId == AUD_DEC_AUX)
    {
//        AUD_SetAout2MixAout1(0);
    }
#endif

#ifdef AOMX_REAL_PTS
    if (!(u4AOmxDbgMsk & 0x800))
    {
        GST_MMQueueSyncInfo(pHandle->u1DecId,0,0,TRUE,0); //Reset MM Sync info
        pHandle->u8LastRealPts = -1;
    }
#endif    

    //ERROR_CNT_MONITOR
    pHandle->error_cnt_timer.tv_sec = 0;
    pHandle->error_cnt_timer.tv_usec = 0;
    return 0;
}

void vAudOmxRestartDecoding(AUD_OMX_HANDLE_T *pHandle)
{
	GST_StopSTC(STC_SRC_A1);
	AUD_DSPCmdStop(AUD_DSP0, pHandle->u1DecId);
	i4AudOmxStopDualDecoding(pHandle);

    //Decoder Interface
    //RA
    pHandle->u2RaFrameByteIdx = 0;

    //PTS
#ifdef AOMX_REAL_PTS    
    if (!(u4AOmxDbgMsk & 0x800))
    {
        GST_MMQueueSyncInfo(pHandle->u1DecId,0,0,TRUE,0); //Reset MM Sync info
        pHandle->u8LastRealPts = -1;
    }
#endif    
    pHandle->nInputTimeStampLast = -1;
    pHandle->nOutputTimeStampCur = -1;
    pHandle->nOutputTimeStampLast = -1;    
    pHandle->u4BankNumPrev = 0;
    pHandle->i4PTSBankNumPrev = 0;
    //END
    
    pHandle->fgGotValidPts = FALSE;    
    pHandle->fgAoutEnable = FALSE;
    pHandle->fgRenderTrigger = FALSE;
    pHandle->fgPauseTrigger = FALSE;

    pHandle->u4TotalEmptySize = 0;
    pHandle->u4TotalInputSize = 0;

    //ERROR_CNT_MONITOR
    do_gettimeofday(&pHandle->error_cnt_timer);

    //UNDERRUN_CNT_MONITOR
    do_gettimeofday(&pHandle->underrun_cnt_timer);
    pHandle->u4PrevInputUnderrunCnt = 0;
    pHandle->u4InputUnderrunDisableTimer = INPUT_UNDERRUN_DISABLE_TIMER;
    //END

    pHandle->pFifoWptr = pHandle->pFifoPntr;
    vAudHalSetBufWPtr(pHandle->u1DecId, (UINT32)(pHandle->pFifoPntr));

	if (!pHandle->fgFormatNotSupport)
	{
		if (pHandle->eFormat == AUD_FMT_VORBISDEC)
		{
			pHandle->fgIsFirstChk = TRUE;
		}
		else
		{
			i4AudOmxStartDualDecoding(pHandle);
			AUD_DSPCmdPlay(AUD_DSP0, pHandle->u1DecId);
		}
	}
}

INT32 i4AudOmxPauseDecoding(AUD_OMX_HANDLE_T *pHandle)
{
	if(TRUE == pHandle->fgRenderTrigger)
	{
		GST_StopSTC(STC_SRC_A1);
	    AUD_DSPCmdPause(AUD_DSP0, pHandle->u1DecId);
	}
    pHandle->fgPauseTrigger = TRUE;
    return 0;
}

INT32 i4AudOmxResumeDecoding(AUD_OMX_HANDLE_T *pHandle)
{
	if(TRUE == pHandle->fgRenderTrigger)
	{
		GST_StartSTC(STC_SRC_A1);
	    AUD_DSPCmdResume(AUD_DSP0, pHandle->u1DecId);
	}
    pHandle->fgPauseTrigger = FALSE;
    return 0;
}

//======================================================
//ERROR_CNT_MONITOR
void vAudOmxCheckDecodeError(AUD_OMX_HANDLE_T *pHandle)
{
    if (((pHandle->error_cnt_timer.tv_sec != 0) || (pHandle->error_cnt_timer.tv_usec != 0)) && !(pHandle->fgFormatNotSupport) && !(pHandle->fgIsAacInTs))
    {
        int time_diff;   
        struct timeval tv;
        
        do_gettimeofday(&tv);
        time_diff = ((tv.tv_sec - pHandle->error_cnt_timer.tv_sec) * 1000000 + tv.tv_usec - pHandle->error_cnt_timer.tv_usec);
        if (time_diff >= 5000000)
        {
            BOOL fgNotify = FALSE;
            UINT32 u4ErrorCnt = u4AudGetDecErrorCnt(pHandle->u1DecId);
            UINT32 u4FrameCnt = u4AudGetDecFrameCnt(pHandle->u1DecId);

            if (u4FrameCnt > 0)
            {
                UINT32 u4ErrorRate;

                if ((pHandle->eFormat == AUD_FMT_AC3)||
                    (pHandle->eFormat == AUD_FMT_MPEG)||
                    (pHandle->eFormat == AUD_FMT_MP3)||
                    (pHandle->eFormat == AUD_FMT_WMA)||
                    (pHandle->eFormat == AUD_FMT_WMAPRO))
                {
                    u4ErrorRate = (u4ErrorCnt * 100 / (u4ErrorCnt + u4FrameCnt));
                }
                else
                {
                    u4ErrorRate = (u4ErrorCnt * 100 / u4FrameCnt);
                }
                
                if (u4ErrorRate >= 90)
                {
                    Printf("[AUD][OMX(%d)] !!!! Notify Audio Error: %d/%d !!!!\n", pHandle->h_handle, u4ErrorCnt, u4FrameCnt);
                    fgNotify = TRUE;
                }
            }                

            if ((!(u4AOmxDbgMsk & 0x200)) && fgNotify)
            {  
                PARAMETER_OMX_CB_EVENTHANDLER rEvent;
                
                rEvent.eEvent     = OMX_EventError;
                rEvent.nData1     = OMX_ErrorFormatNotDetected;
                rEvent.nData2     = 0;
                rEvent.pEventData = NULL;
                i4AudOmxCompNotifyEvent(pHandle, &rEvent);
            }            

            pHandle->error_cnt_timer.tv_sec = 0;
            pHandle->error_cnt_timer.tv_usec = 0;
        }
    }
	
	if (!pHandle->fgFormatNotSupport) 
	{
		// WMA Decoder Error
		if (((pHandle->eFormat == AUD_FMT_WMA) || (pHandle->eFormat == AUD_FMT_WMAPRO)) && (DSP_GetWmaErrorCode(pHandle->u1DecId) != 0)) 
		{
			Printf("[AUD][OMX(%d)] !!!! WMA Decoder Error !!!! \n", pHandle->h_handle);
			i4AudOmxStopDecoding(pHandle);		
			pHandle->fgFormatNotSupport= TRUE;
		}
		// Vorbis
		if ((pHandle->eFormat == AUD_FMT_VORBISDEC) && (DSP_GetVorbisErrorCode(pHandle->u1DecId) != 0)) 
		{
			Printf("[AUD][OMX(%d)] !!!! Vorbis Decoder Error !!!! \n", pHandle->h_handle);
			i4AudOmxStopDecoding(pHandle);		
			pHandle->fgFormatNotSupport= TRUE;
		}
		// APE
		if ((pHandle->eFormat == AUD_FMT_APE) && (DSP_GetAPEErrorCode() != 0)) 
		{
			Printf("[AUD][OMX(%d)] !!!! APE Decoder Error !!!! \n", pHandle->h_handle);
			i4AudOmxStopDecoding(pHandle);		
			pHandle->fgFormatNotSupport= TRUE;
		}
	}
}
//END

//======================================================
//UNDERRUN_CNT_MONITOR
extern UINT32 u4ReadDspSram(UINT32 u4Addr);
void vAudOmxCheckUnderrun(AUD_OMX_HANDLE_T *pHandle)
{
    if (((pHandle->underrun_cnt_timer.tv_sec != 0) || (pHandle->underrun_cnt_timer.tv_usec != 0)) && !(pHandle->fgFormatNotSupport) && !(pHandle->fgGotEOS))
    {
        int time_diff;   
        struct timeval tv;
        UINT32 u4InputUnderrunCnt = 0;
        
        do_gettimeofday(&tv);
        time_diff = ((tv.tv_sec - pHandle->underrun_cnt_timer.tv_sec) * 1000000 + tv.tv_usec - pHandle->underrun_cnt_timer.tv_usec);
        if (time_diff >= 1000000)
        {
            if (pHandle->u4InputUnderrunDisableTimer)
                pHandle->u4InputUnderrunDisableTimer--;
            if ((pHandle->eState == OMX_StateExecuting) && (AUD_IsFifoEmpty(AUD_DSP0, pHandle->u1DecId)) && (pHandle->u4InputUnderrunDisableTimer==0))
            {
                //Notify
                PARAMETER_OMX_CB_EVENTHANDLER rEvent;                

                rEvent.eEvent     = OMX_EventExtInputUnderrun;
                rEvent.nData1     = 0;
                rEvent.nData2     = 0;
                rEvent.pEventData = NULL;
                i4AudOmxCompNotifyEvent(pHandle, &rEvent);
                Printf("[AUD][OMX(%d)][Underrun] Notify\n", pHandle->h_handle);
            }
            pHandle->u4PrevInputUnderrunCnt = u4InputUnderrunCnt;

            pHandle->underrun_cnt_timer.tv_sec = tv.tv_sec;
            pHandle->underrun_cnt_timer.tv_usec = tv.tv_usec;
        }
    }
}
//END

//======================================================
//AOUT_ENALBE_CNT_MONITOR
void vAudOmxCheckAoutEnableNotify(AUD_OMX_HANDLE_T *pHandle)
{   
    if (AUD_IsDSPRealPlay(AUD_DSP0, pHandle->u1DecId))    //check if Aout Enable
    { 
        if (!pHandle->fgAoutEnable)
        {
            //Notify
            PARAMETER_OMX_CB_EVENTHANDLER rEvent; 

            rEvent.eEvent     = OMX_EventPortSettingsChanged;
            rEvent.nData1     = 1;
            rEvent.nData2     = 0; //output index
            rEvent.pEventData = NULL;
            i4AudOmxCompNotifyEvent(pHandle, &rEvent); 
            Printf("[AUD][OMX(%d)] OMX AOUT ENABLE Notify!!!\n", pHandle->h_handle);

            pHandle->fgAoutEnable = TRUE; 
        }
    }
    else
    {
        SYS_Printf("[AUD][OMX(%d)] OMX WAIT AOUT ENABLE\n", pHandle->h_handle);
    }
}
//END

//======================================================
//MTAL Render Trigger Entry
void vAudOmxTrigRender(UINT8 u1DecId) //AOMX2_TEST_TODO
{
    UINT8 u1OmxId = 0; 

    for(u1OmxId = 0; u1OmxId < AUD_COMP_MAX; u1OmxId++)
    {
        if ((_prAudOmxHandle[u1OmxId].fgUsed) && (_prAudOmxHandle[u1OmxId].u1DecId == u1DecId))
        {
            break;
        }
    }

    //Printf("%s: AUD_DEC%d\n", __FUNCTION__, u1DecId);
	
    if (!_prAudOmxHandle[u1OmxId].fgRenderTrigger && _prAudOmxHandle[u1OmxId].fgAoutEnable)
    {
        struct timeval tv;

        GST_StartSTC(STC_SRC_A1);
        AUD_MMAoutEnable(_prAudOmxHandle[u1OmxId].u1DecId, TRUE);
        _prAudOmxHandle[u1OmxId].fgRenderTrigger = TRUE;
        do_gettimeofday(&tv);        
        Printf("%d.%06d AUD Render Trigger! u1OmxId(%d) u1DecId(%d)\n", tv.tv_sec, tv.tv_usec, u1OmxId, _prAudOmxHandle[u1OmxId].u1DecId);

        //UNDERRUN_CNT_MONITOR
        //When render is triggered then start checking underrun
        do_gettimeofday(&_prAudOmxHandle[u1OmxId].underrun_cnt_timer);
        _prAudOmxHandle[u1OmxId].u4InputUnderrunDisableTimer = INPUT_UNDERRUN_DISABLE_TIMER;
        //END
    }
}

//======================================================
#ifdef AOMX_REAL_PTS
UINT32 u8AudOmxGetPts(AUD_OMX_HANDLE_T* pHandle,UINT64* prAudioPts)
{
    UINT32 u4PtsRp;
    UINT64 u8PTS;
    UINT32 u4PTSHigh = 0;
    BOOL fgPtsCheck;
    //estimate current PTS from PTS_CUR_PNT
    u4PtsRp = u4ReadD2RInfo_PtsCurPnt(AUD_DSP0, pHandle->u1DecId);
    fgPtsCheck = GST_MMFindSyncInfo(pHandle->u1DecId,&u8PTS,u4PtsRp,&u4PTSHigh);

	if(i4AOMXAVComps >= 0)
	{
		u8PTS = u8Div6432((UINT64)GST_GetSTCVal(STC_SRC_A1) * 100, (UINT64)9, NULL) + (UINT64)(i4AOMXAVComps*1000);
	}
	else
	{
		u8PTS = u8Div6432((UINT64)GST_GetSTCVal(STC_SRC_A1) * 100, (UINT64)9, NULL) - (UINT64)((-i4AOMXAVComps)*1000);
	}
	
    //check if we can use fake PTS
    #if 0
    if ((u8PTS == pHandle->u8LastRealPts) || !fgPtsCheck)     //PTS resolution is not high enough
    {
       *prAudioPts = pHandle->nOutputTimeStampCur;
    }
    else if ((!pHandle->fgRenderTrigger) && (u8PTS - pHandle->nOutputTimeStampCur < 100000))        //check if startPTS and current PTS diff too large (for DivX I13 seek to 15~25)
    {
       *prAudioPts = pHandle->nOutputTimeStampCur;
    }
    else
    {    
        pHandle->nOutputTimeStampCur = u8PTS;      //update fake PTS if diff too large
        *prAudioPts = u8PTS;
        pHandle->u8LastRealPts = u8PTS;
    }    
	#else
	if (u8PTS == pHandle->u8LastRealPts)     //PTS resolution is not high enough
    {
       *prAudioPts = pHandle->nOutputTimeStampCur;
    }
    //else if ((!pHandle->fgRenderTrigger) && (u8PTS - pHandle->nOutputTimeStampCur < 100000))        //check if startPTS and current PTS diff too large (for DivX I13 seek to 15~25)
    else if (!pHandle->fgRenderTrigger)        //check if startPTS and current PTS diff too large (for DivX I13 seek to 15~25)
    {
       *prAudioPts = pHandle->nOutputTimeStampCur;
    }
    else
    {    
        //pHandle->nOutputTimeStampCur = u8PTS;      //update fake PTS if diff too large
        *prAudioPts = u8PTS;
        //pHandle->u8LastRealPts = u8PTS;
    }
	#endif
    return(u4PTSHigh);
}
#endif

INT64 i8AudOmxGetCurPts(void)  //AOMX2_TEST_TODO
{
    UINT64 u8RealPts;
    //UINT32 u4BankNum;
#ifdef AOMX_REAL_PTS    
    //UINT32 u4PTSHigh;
#endif
    if ((_prAudOmxHandle[0].nOutputTimeStampLast == -1) || (!_prAudOmxHandle[0].fgRenderTrigger))
    {
        return _prAudOmxHandle[0].nOutputTimeStampCur;
    }
    else
    {
		#if 0
        u4BankNum = u4ReadD2RInfo_TotalBank(AUD_DSP0, AUD_DEC_MAIN);
#ifdef AOMX_REAL_PTS
        if (((INT32)u4BankNum >= _prAudOmxHandle[0].i4PTSBankNumPrev) || (u4AOmxDbgMsk & 0x800))
#endif
        {
            u8RealPts = (UINT64)((INT32)u4BankNum - _prAudOmxHandle[0].i4PTSBankNumPrev) * PTS_BANK256;
            return (_prAudOmxHandle[0].nOutputTimeStampCur +u8RealPts);
        }
#ifdef AOMX_REAL_PTS
        else        //should not happen... 
        {
            Printf ("Get Cur PTS error!Prev = %d, Cur=%d\n",_prAudOmxHandle[0].i4PTSBankNumPrev,u4BankNum);
            GST_MMFindSyncInfo(AUD_DEC_MAIN,&u8RealPts,u4ReadD2RInfo_PtsCurPnt(AUD_DSP0, AUD_DEC_AUX),&u4PTSHigh);
            return (u8RealPts);
        }
#endif
		#else

		if(i4AOMXAVComps >= 0)
		{
			u8RealPts = u8Div6432((UINT64)GST_GetSTCVal(STC_SRC_A1) * 100, (UINT64)9, NULL) + (UINT64)(i4AOMXAVComps*1000);
		}
		else
		{
			u8RealPts = u8Div6432((UINT64)GST_GetSTCVal(STC_SRC_A1) * 100, (UINT64)9, NULL) - (UINT64)((-i4AOMXAVComps)*1000);
		}

		return (u8RealPts);
		#endif
    }
}

//======================================================

UINT32 u4GstDbgMsk[16] = 
{
    0,    //same as u4AOmxDbgMsk
    0,    //reserve for baoyin
    0,    //Fix Audio initial delay (unit 1ms) 0: => use Auto delay, >0: => use Fix Delay
    80,  //Audio freeze threshold
    80,   //Audio faster upper bound
    300,  //Audio faster lower bound
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

void vAudOmxGetGstDbgMsk(UINT32 u4Index,UINT32* pu4DbgMsk)
{
    if (u4Index < sizeof(u4GstDbgMsk)/sizeof(UINT32))
    {
        *pu4DbgMsk = u4GstDbgMsk[u4Index];
    }
}

void vAudOmxSetGstDbgMsk(UINT32 u4Index, UINT32 u4DbgMsk)
{
    if (u4Index < sizeof(u4GstDbgMsk)/sizeof(UINT32))
    {
        u4GstDbgMsk[u4Index] = u4DbgMsk;
    }
}

extern BOOL fgOmxCliCtrl;
extern UINT8 gu1ForceDecId;
extern BOOL fgForceEncFlag;

void vAudOmxEnableRecord(UINT8 u1OmxId, BOOL fgCliEnable, UINT8 u1DecEnabled, BOOL fgEncFlag)
{
	fgOmxCliCtrl = fgCliEnable;
	gu1ForceDecId = u1DecEnabled;
	fgForceEncFlag = fgEncFlag;
}

INT32 i4AudOmxStartDualDecoding(AUD_OMX_HANDLE_T *pHandle)
{
#if defined(CC_AUD_ARM_SUPPORT) && defined(CC_AUD_ARM_RENDER)
	return AUD_OK;
#endif
	if(pHandle->fgEncFlag)
		return AUD_OK;

	if(pHandle->u1DecId == AUD_DEC_MAIN)
	{
		return AUD_OK;
	}
	else if(pHandle->u1DecId == AUD_DEC_AUX)
	{
    #ifdef CC_AUD_LINEIN_USE_DEC3 
		_AUD_DspSetTriOnlyDecMode(FALSE);
    #endif
		_AUD_DspSetDualDecMode(TRUE);
	}
	else if(pHandle->u1DecId == AUD_DEC_THIRD)
	{
		_AUD_DspSetDualDecMode(FALSE);
    #ifdef CC_AUD_LINEIN_USE_DEC3 
		_AUD_DspSetTriOnlyDecMode(TRUE);
    #endif
	}
    AUD_DspIECConfig((AUD_IEC_T)AUD_GetDspIECConfig(), TRUE);
	AUD_DSPCmdPlay(AUD_DSP0, AUD_DEC_MAIN);
	return AUD_OK;
}

INT32 i4AudOmxStopDualDecoding(AUD_OMX_HANDLE_T *pHandle)
{
#if defined(CC_AUD_ARM_SUPPORT) && defined(CC_AUD_ARM_RENDER)
	return AUD_OK;
#endif    
	if(pHandle->fgEncFlag)
		return AUD_OK;

	if(pHandle->u1DecId == AUD_DEC_MAIN)
	{
		return AUD_OK;
	}
		
	AUD_DSPCmdStop(AUD_DSP0, AUD_DEC_MAIN);
#ifdef CC_AUD_LINEIN_USE_DEC3 
	_AUD_DspSetTriOnlyDecMode(FALSE);
#endif
    _AUD_DspSetDualDecMode(FALSE);
	return AUD_OK;
}

INT32 i4AudOmxSetDualDecoding(AUD_OMX_HANDLE_T *pHandle)
{
#if defined(CC_AUD_ARM_SUPPORT) && defined(CC_AUD_ARM_RENDER)
	return AUD_OK;
#endif      
	if(pHandle->fgEncFlag)
		return AUD_OK;

	if(pHandle->u1DecId == AUD_DEC_MAIN)
	{
		return AUD_OK;
	}
		
	AUD_SetDecType(AUD_DSP0, AUD_DEC_MAIN, AUD_STREAM_FROM_GST, AUD_FMT_PCM);	
	AUD_SetAvSynMode(AUD_DEC_MAIN, AV_SYNC_FREE_RUN);
	return AUD_OK;
}	

//======================================================

//=============== AOMX Configuration Param ===================
void vAudOmxSetAVSyncComps(INT32 i4Comp)
{
	Printf ("Get Last AV sync comps: %d\n",i4AOMXAVComps);
	i4AOMXAVComps = i4Comp;
	Printf ("Set New AV sync comps: %d\n",i4AOMXAVComps);
}

void vAudOmxSetDumpData(BOOL fgEnable)
{
	fgAOMXDumpDataEnable = fgEnable;
}


//=======================================================

//=================== Vorbis Decoder ========================

void vStoreCodecData(AUD_OMX_HANDLE_T *pHandle, void *pBuf, UINT32 u4Size)
{
	if (pHandle->pCodecData)
	{
		Printf("[AUD][OMX(%d)] Store Codec Data - Already exist!\n", pHandle->h_handle);
		return;
	}

	pHandle->pCodecData = x_mem_alloc(u4Size);
	if (!pHandle->pCodecData)
	{
		Printf("[AUD][OMX(%d)] Store Codec Data - Allocate memory fail.\n", pHandle->h_handle);
		pHandle->u4CodecDataSize = 0;
		return;
	}
	x_memcpy(pHandle->pCodecData, pBuf, u4Size);
	pHandle->u4CodecDataSize = u4Size;
}

void vDestroyCodecData(AUD_OMX_HANDLE_T *pHandle)
{
	pHandle->u4CodecDataSize = 0;
	if (!pHandle->pCodecData) {
		return;
	}
	x_mem_free(pHandle->pCodecData);
}

UINT8 u1WrapOggHeader(UINT8 *u1VORBIS_OggS, UINT32 u4VorbisPage, UINT32 u4DataLen)
{
	UINT8 u1Temp;
	UINT32 u4EmptySize1;

	///"OggS"
	
	u1VORBIS_OggS[0] = 0x4F;
	u1VORBIS_OggS[1] = 0x67;
	u1VORBIS_OggS[2] = 0x67;
	u1VORBIS_OggS[3] = 0x53;
	///"Version" must be 0x00
	u1VORBIS_OggS[4] = 0x00;
	///"Type": =1 continue page, =2 BOS, =4 BOE.
	u1VORBIS_OggS[5] = 0x00;
	///"Granule=8, S/N=4, Page no=4, CRC=4" bytes
	for(u1Temp=6;u1Temp<26;u1Temp++)
	{
		u1VORBIS_OggS[u1Temp]=0;
	}
	
	/// Set "Page" Information
	u1VORBIS_OggS[18] = (UINT8)(u4VorbisPage & 0xFF);
	u1VORBIS_OggS[19] = (UINT8)((u4VorbisPage>>8) & 0xFF);
	u1VORBIS_OggS[20] = (UINT8)((u4VorbisPage>>16) & 0xFF);
	u1VORBIS_OggS[21] = (UINT8)((u4VorbisPage>>24) & 0xFF);
	
	if((u4DataLen)<= 0xFF)
	{
		u1Temp = 1; 	/// Only one segment
		u1VORBIS_OggS[26] = 0x01;
		u1VORBIS_OggS[27] = u4DataLen;
	}
	else
	{
		u1Temp = 1; 	/// Need accumulate segments number.
		u4EmptySize1 = u4DataLen;
		while(u4EmptySize1 > 0xFF)
		{
		   u4EmptySize1 -= 0xFF;
		   /// Set Segments table
		   u1VORBIS_OggS[26+u1Temp] = 0xFF;  ///[26+1]~[26+255]
		   u1Temp ++;
		}
		/// "Segments Number"=1
		u1VORBIS_OggS[26] = u1Temp;
		/// Last segment of "Segment Table"
		u1VORBIS_OggS[26+u1Temp] = u4EmptySize1;	 ///Update last segment table.
	}
	
	return 27 + u1Temp;
}

UINT32 u4FindVorbisHeader(UINT8 *pBuf, UINT32 u4BufSize, UINT8 u1Type)
{
	UINT32 i;
	for (i=0 ;i < u4BufSize - 7; i++)
	{
		if ((pBuf[i] == u1Type) && (pBuf[i+1] == 'v') && (pBuf[i+2] == 'o') && (pBuf[i+3] == 'r') 
			&& (pBuf[i+4] == 'b') && (pBuf[i+5] == 'i') && (pBuf[i+6] == 's'))
		{
			return (UINT32)&pBuf[i];
		}
	}
	return 0;
}


void vParseVorbisHeader(AUD_OMX_HANDLE_T *pHandle, void *pBuf, UINT32 u4BufSize)
{
	UINT8 u1VORBIS_OggS[27+255];
	UINT8 u1OggHeaderLen;
	//UINT8 i;
	
	UINT32 u4IdentHeader = u4FindVorbisHeader((UINT8 *)pBuf, u4BufSize, 0x01);
	UINT32 u4SetupHeader = u4FindVorbisHeader((UINT8 *)pBuf, u4BufSize, 0x03);
	UINT32 u4CommentHeader = u4FindVorbisHeader((UINT8 *)pBuf, u4BufSize, 0x05);
	
	INT32 i4IdentLen = u4SetupHeader - u4IdentHeader;
	INT32 i4SetupLen = u4CommentHeader - u4SetupHeader;
	INT32 i4CommentLen = u4BufSize - (u4CommentHeader - (UINT32)pBuf);

	Printf("Vorbis Header size : %d \n", u4BufSize);
//	for (i = 0; i< 100; i++)
//		Printf("%x ", *((UINT8 *)pBuf+i));
//	Printf("\n");
	
	if ((!u4IdentHeader) || (!u4SetupHeader) || (!u4CommentHeader)
		|| (i4IdentLen <= 0) || (i4SetupLen <= 0) || (i4CommentLen <= 0))
	{
		Printf("[AUD][OMX(%d)] Parse Vorbis Header fail!, %x %x %x %x %d %d %d\n", pHandle->h_handle,
			pBuf, u4IdentHeader, u4SetupHeader, u4CommentHeader, i4IdentLen, i4SetupLen, i4CommentLen);
		return;
	}

	if (pHandle->fgEnableAdtsInsert) 
	{
		u1OggHeaderLen = u1WrapOggHeader(u1VORBIS_OggS, pHandle->u4VorbisPage++, i4IdentLen);
		u4AudOmxFillFIFO(pHandle, (UINT32)u1VORBIS_OggS, u1OggHeaderLen);
	}
	u4AudOmxFillFIFO(pHandle, u4IdentHeader, i4IdentLen);
	Printf("Ident Header Len : %d %d\n", u1OggHeaderLen, i4IdentLen);
//	for (i = 0; i< 20; i++)
//		Printf("%x ", *((UINT8 *)u4IdentHeader+i));
//	Printf("\n");

	if (pHandle->fgEnableAdtsInsert) 
	{
		u1OggHeaderLen = u1WrapOggHeader(u1VORBIS_OggS, pHandle->u4VorbisPage++, i4SetupLen);
		u4AudOmxFillFIFO(pHandle, (UINT32)u1VORBIS_OggS, u1OggHeaderLen);
	}
	u4AudOmxFillFIFO(pHandle, u4SetupHeader, i4SetupLen);
	Printf("Setup Header Len : %d %d\n", u1OggHeaderLen, i4SetupLen);
//	for (i = 0; i< 20; i++)
//		Printf("%x ", *((UINT8 *)u4SetupHeader+i));
//	Printf("\n");


	if (pHandle->fgEnableAdtsInsert) 
	{
		u1OggHeaderLen = u1WrapOggHeader(u1VORBIS_OggS, pHandle->u4VorbisPage++, i4CommentLen);
		u4AudOmxFillFIFO(pHandle, (UINT32)u1VORBIS_OggS, u1OggHeaderLen);
	}
	u4AudOmxFillFIFO(pHandle, u4CommentHeader, i4CommentLen);
	Printf("Setup Header Len : %d %d\n", u1OggHeaderLen, i4CommentLen);
//	for (i = 0; i< 20; i++)
//		Printf("%x ", *((UINT8 *)u4CommentHeader+i));
//	Printf("\n");
	
	return;
}

//=======================================================

#endif

