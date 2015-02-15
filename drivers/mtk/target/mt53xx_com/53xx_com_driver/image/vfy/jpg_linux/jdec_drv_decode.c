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

#include "x_assert.h"
#include "x_os.h"
//#include "x_rtos.h"
#include "jdec_drv.h"
#include "jdec_drv_decode.h"
#include "jdec_drv_scale.h"
#include "jdec_hal_if.h"
#include "jdec_hal_errcode.h"
#include "jdec_com_jfif.h"
#include "jdec_com_common.h"
#include "drv_img_dma.h"
#include "drv_config.h"
#include "x_hal_5381.h"
#include "jdec_debug.h"
#include "sw/djpeg_sw_api.h"
#if PERFORMANCE_TEST
#include "x_timer.h"
#endif
//#include "x_hal_1176.h"
//#include "sys_config.h"
#if CONFIG_SYS_MEM_PHASE2
#include "x_mem_phase2.h"
#elif CONFIG_SYS_MEM_PHASE3
#include "x_kmem.h"
#endif


/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
UINT32 _u4JdecDecodeTimes1 = 0;
UINT32 _u4JdecDecodeTimes2 = 0;
UINT32 _u4JdecDecodeTimes = 0;
UINT32 _u4JdecScaleTimes1 = 0;
UINT32 _u4JdecScaleTimes2 = 0;
UINT32 _u4JdecScaleTimes = 0;
UINT32 _u4JdecTrnsTotalTimes = 0;
UINT32 _u4JdecDecTotalTimes = 0;
UINT32 _u4JdecScaleTotalTimes = 0;
UINT32 _u4JdecDecFailCount = 0;
UINT32 _u4_debug_loglevel =0;
#define JDEC_DEBUG_LOG_SAVE_MAX 20
#define JDEC_DEBUG_LOG_SAVE_MAX2 30
#if JDEC_DEBUG_LOG_SAVE
UINT32 _u4JdecStateBack = 0;
UINT32 _u4JdecDecStateBack = 0;
UINT32 _au4JdecDecbugFlow[JDEC_DEBUG_LOG_SAVE_MAX][JDEC_DEBUG_LOG_SAVE_MAX2];
UINT32 _u4JdecDebugFlowCount1 = 0;
UINT32 _u4JdecDebugFlowCount2 = 0;
#endif
INT32 i4JdecUpdateImgdmaRdPoint(JDEC_INST_T *prJdecInst);
/*-----------------------------------------------------------------------------
                    function/veriable extern
 ----------------------------------------------------------------------------*/
extern UINT32 _aru4FifoStart[JDEC_INST_NUM];
extern JDEC_INST_T _rJdecInst[JDEC_INST_NUM];
extern void vJdecAbort(JDEC_INST_T *prJdecInst, IMG_NFY_STATE_T eState, BOOL fgNfy);
extern void vJDECPollingFsh(UINT32 u4HwId);

#if JDEC_USE_MAP_UNMAP
extern void vJdecMapPrintf(const char *format_string, ...);
#endif

extern void vJdecRdPtrPrintf(const char *format_string, ...);
/*-----------------------------------------------------------------------------
                    code start
 ----------------------------------------------------------------------------*/

void vJdecIncLogCount1(void)
{
#if JDEC_DEBUG_LOG_SAVE
    INT32 i;
    _u4JdecDebugFlowCount1++;
    if (_u4JdecDebugFlowCount1 >= JDEC_DEBUG_LOG_SAVE_MAX)
    {
        _u4JdecDebugFlowCount1 = 0;
    }
    _u4JdecDebugFlowCount2 = 0;
    for (i=0; i<30; i++)
    {
        _au4JdecDecbugFlow[_u4JdecDebugFlowCount1][i] = 0;
    }
#endif
    return;
}

void vJdecResetLogCount2(void)
{
#if JDEC_DEBUG_LOG_SAVE
    _u4JdecDebugFlowCount2 = 0;
#endif
    return;
}

void vJdecSetLog(UINT32 u4LogVal)
{
#if JDEC_DEBUG_LOG_SAVE
    if (JDEC_DEBUG_LOG_SAVE_MAX2 > _u4JdecDebugFlowCount2)
    {
        _au4JdecDecbugFlow[_u4JdecDebugFlowCount1][_u4JdecDebugFlowCount2] = u4LogVal;
        _u4JdecDebugFlowCount2++;
    }
#endif
    return;
}

void vJdecSaveState(JDEC_INST_T *prJdecInst)
{
#if JDEC_DEBUG_LOG_SAVE
    if (prJdecInst->eJdecState != JDEC_STATE_IDLE)
    {
        _u4JdecStateBack = (UINT32)(prJdecInst->eJdecState) + (_u4JdecDebugFlowCount1 << 16);
    }
    if (prJdecInst->eJdecDecState != JDEC_DECODE_IDLE)
    {
        _u4JdecDecStateBack = (UINT32)(prJdecInst->eJdecDecState) + (_u4JdecDebugFlowCount1 << 16);
    }
#endif
    return;
}

void vJdecInitDecVal(JDEC_INST_T *prJdecInst)
{
    INT32 i4=0;
    prJdecInst->u4DecProgress = 0;
    prJdecInst->u4ScanCount = 0;
    prJdecInst->u4CurrDecodeMCUW = 0;
    prJdecInst->u4CurrDecodeMCUH = 0;
    prJdecInst->u4MCUInMCURowW = 0;
    prJdecInst->u4MCUInMCURowH = 0;
    prJdecInst->u4CurrDecodeMCURow = 0;
    prJdecInst->u4MCURowNum = 0;
    prJdecInst->i4JdecErrorType = 0;
    prJdecInst->eJdecDecState = JDEC_DECODE_IDLE;
    prJdecInst->i4JdecTrnsTimerStatus = 0;
    prJdecInst->i4JdecDecTimerStatus = 0;
    prJdecInst->u4TimeOutCount = 0;
    for (i4=0; i4<JDEC_COMP_NUM; i4++)
    {
        prJdecInst->au4CompScanCount[i4] = 0;
        prJdecInst->fgDefaultScale[i4] = FALSE;
    }
}

void vJdecInitPrsInstData(JDEC_INST_T *prJdecInst)
{
    #if JDEC_CACHE_ENABLE
    JDEC_PIC_INFO_T *prJdecPicInfo = (JDEC_PIC_INFO_T *)(prJdecInst->pvPicCache);

    prJdecPicInfo->u4ImgId = prJdecInst->u4ImgId;
    prJdecPicInfo->u4Ofst1stSOS = 0;
    //for cache using
    prJdecPicInfo->eCacheState = JDEC_CACHE_INUSING;
    //--

    #endif
    #if SUPPORT_VDEC_PATH
    if (prJdecInst->fgVdecPath)
        return;
    else
    #endif
    //--
    {
        if (prJdecInst->hJfifInst != NULL)
        {
            JFIF_FreeInstance(prJdecInst->hJfifInst);
        }
        prJdecInst->hJfifInst = JFIF_AllocInstance();
    }
}

void vJdecInitFifoVal(JDEC_INST_T *prJdecInst)
{
    JDEC_FIFO_INFO_T *prFifoInfo = &(prJdecInst->rFifoInfo);

    prFifoInfo->fgEoRange = FALSE;
}


void vJdecTrnsTimerCb(HANDLE_T hTimer, VOID *pvTag)
{
    INT32 i4Ret;
    JDEC_INST_T *prJdecInst = &(_rJdecInst[(UINT32)pvTag]);
   vJdecLog_L("[jdec]Clear TRS  rJdecInst->hJdecEvent =0x%x\n",prJdecInst->hJdecEvent);
    i4Ret = x_ev_group_set_event(prJdecInst->hJdecEvent, JDEC_DRV_EV_TIME_OUT, X_EV_OP_OR);
    VERIFY(i4Ret == OSR_OK);
}


INT32 i4JdecWaitTrnsTimeOut(JDEC_INST_T *prJdecInst)
{
    EV_GRP_EVENT_T  eJdecEvent;
    EV_GRP_EVENT_T  eJdecEventGet;
    INT32 i4Ret;
    vJdecLog_L ("Wait ---4JdecWaitTrnsTimeOut  0x%x\n", prJdecInst->hJdecEvent);
    eJdecEvent = JDEC_DRV_EV_TIME_OUT | JDEC_DRV_EV_STOP | JDEC_DRV_EV_FILLED;
    i4Ret = x_ev_group_wait_event(prJdecInst->hJdecEvent, eJdecEvent, &eJdecEventGet, X_EV_OP_OR_CONSUME);
    VERIFY(i4Ret == OSR_OK);
    vJdecLog_L ("Got ---4JdecWaitTrnsTimeOut  0x%x\n", prJdecInst->hJdecEvent);

    i4Ret = x_timer_stop(prJdecInst->hJdecTrnsTimer);
    VERIFY(OSR_OK == i4Ret);
    prJdecInst->i4JdecTrnsTimerStatus = 0;

    //clean event
    //i4Ret = x_ev_group_set_event(prJdecInst->hJdecEvent, 0x00, X_EV_OP_CLEAR);
    //VERIFY(i4Ret == OSR_OK);

    if (eJdecEventGet & JDEC_DRV_EV_TIME_OUT)
    {
        prJdecInst->i4JdecErrorType = -E_JPG_ERR_NO_DATA;
    }
    if (eJdecEventGet & JDEC_DRV_EV_STOP)
    {
        vJdecSetLog(JDEC_LOG_TRNS_STOP);
        vJdecAbort(prJdecInst, (IMG_NFY_STATE_T)IMG_NFY_STOP_DONE, FALSE);
        return JDEC_DRV_RTN_FAIL;
    }

    return JDEC_DRV_RTN_FAIL;
}

INT32 i4JdecSetTrnsTimer(JDEC_INST_T *prJdecInst, UINT32 u4StopTime)
{
    INT32 i4Ret;
    EV_GRP_EVENT_T  eJdecEvent = 0;
    EV_GRP_EVENT_T  eJdecEventGet = 0;

    //stop timer
    prJdecInst->i4JdecTrnsTimerStatus = 1;
    i4Ret = x_timer_stop(prJdecInst->hJdecTrnsTimer);
    VERIFY(OSR_OK == i4Ret);

    //clean timer event in event group
    eJdecEvent = JDEC_DRV_EV_TIME_OUT;
    i4Ret = x_ev_group_wait_event_timeout(prJdecInst->hJdecEvent, eJdecEvent, &eJdecEventGet, X_EV_OP_OR_CONSUME, 0);

    //start timer
    i4Ret = x_timer_start(prJdecInst->hJdecTrnsTimer, u4StopTime, X_TIMER_FLAG_ONCE, vJdecTrnsTimerCb, (void*)((UINT32)prJdecInst->u2CompId));
    VERIFY(OSR_OK == i4Ret);

    return i4Ret;
}


void vJdecDecTimerCb(HANDLE_T hTimer, VOID *pvTag, UINT32 u4ChId)
{
    INT32 i4Ret;
    JDEC_INST_T *prJdecInst = &(_rJdecInst[(UINT32)pvTag]);
    UTIL_Printf("[JDEC] %s line %d JDEC_DRV_EV_DEC_TIME_OUT\n", __FUNCTION__, __LINE__);
    i4Ret = x_ev_group_set_event(prJdecInst->hJdecEvent, JDEC_DRV_EV_DEC_TIME_OUT, X_EV_OP_OR);
    VERIFY(i4Ret == OSR_OK);
}

INT32 i4JdecWaitDecTimeOut(JDEC_INST_T *prJdecInst)
{
    EV_GRP_EVENT_T  eJdecEvent = 0;
    EV_GRP_EVENT_T  eJdecEventGet = 0;
    INT32 i4Ret;

    eJdecEvent = JDEC_DRV_EV_DEC_TIME_OUT | JDEC_DRV_EV_STOP;
    i4Ret = x_ev_group_wait_event_timeout(prJdecInst->hJdecEvent, eJdecEvent, &eJdecEventGet, X_EV_OP_OR_CONSUME, 2);

    if (eJdecEventGet & JDEC_DRV_EV_DEC_TIME_OUT)
    {
        i4Ret = x_timer_stop(prJdecInst->hJdecDecTimer);
        VERIFY(OSR_OK == i4Ret);
        prJdecInst->i4JdecDecTimerStatus = 0;
        prJdecInst->i4JdecErrorType = -E_JPG_ERR_DEC_ERROR;
		UTIL_Printf("[JDEC] %s line %dE_JPG_ERR_DEC_ERROR\n", __FUNCTION__, __LINE__);
    }
    if (eJdecEventGet & JDEC_DRV_EV_STOP)
    {
        vJdecSetLog(JDEC_LOG_DEC_STOP);
        i4Ret = x_timer_stop(prJdecInst->hJdecDecTimer);
        VERIFY(OSR_OK == i4Ret);
        prJdecInst->i4JdecDecTimerStatus = 0;
        vJdecAbort(prJdecInst, (IMG_NFY_STATE_T)IMG_NFY_STOP_DONE, FALSE);
        return JDEC_DRV_RTN_FAIL;
    }
    return JDEC_DRV_RTN_OK;
}

INT32 i4JdecSetDecTimer(JDEC_INST_T *prJdecInst, UINT32 u4StopTime)
{
    INT32 i4Ret;
    EV_GRP_EVENT_T  eJdecEvent = 0;
    EV_GRP_EVENT_T  eJdecEventGet = 0;

    //stop timer
    prJdecInst->i4JdecDecTimerStatus = 1;
    i4Ret = x_timer_stop(prJdecInst->hJdecDecTimer);
    VERIFY(OSR_OK == i4Ret);

    //clean timer event in event group
    eJdecEvent = JDEC_DRV_EV_DEC_TIME_OUT;
    vJdecLog_L("[JDEC]Clear  Time out rJdecInst->hJdecEvent =0x%x\n",prJdecInst->hJdecEvent);
    i4Ret = x_ev_group_wait_event_timeout(prJdecInst->hJdecEvent, eJdecEvent, &eJdecEventGet, X_EV_OP_OR_CONSUME, 0);

    //start timer
    i4Ret = x_timer_start(prJdecInst->hJdecDecTimer, u4StopTime, X_TIMER_FLAG_ONCE, (x_os_timer_cb_fct)vJdecDecTimerCb, (void*)((UINT32)prJdecInst->u2CompId));
    VERIFY(OSR_OK == i4Ret);

    return i4Ret;
}
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
INT32 i4JdecGetHalRdPointForScanSearch(JDEC_INST_T *prJdecInst)
{
    UINT32 u4RdPoint, u4RdPointBitShift;
    JDEC_FIFO_INFO_T *prFifoInfo = &(prJdecInst->rFifoInfo);
    INT32 i4Ret = S_JDEC_HAL_OK;

    i4Ret = i4JDEC_HAL_Get_Read_Pointer(prJdecInst->u4HwInstId, &u4RdPoint, &u4RdPointBitShift);

    if (i4Ret == S_JDEC_HAL_OK)
    {
        if(u4RdPointBitShift > 0)
            u4RdPoint++;

        if(u4RdPoint < prFifoInfo->u4FifoEnd)
        {
            prJdecInst->u4ScanRd = u4RdPoint;
        }
        else if (u4RdPoint == prFifoInfo->u4FifoEnd)
        {
            prJdecInst->u4ScanRd = prFifoInfo->u4FifoStart;
        }
        else
        {
            //ASSERT(0);
            return JDEC_DRV_RTN_FAIL;
        }
        //UTIL_Printf("[JDEC] Hw read point 0x%x \n",prFifoInfo->u4RdPoint);
        return JDEC_DRV_RTN_OK;
    }
    else
    {
        return JDEC_DRV_RTN_FAIL;
    }
}
#endif

INT32 i4JdecGetHalRdPoint(JDEC_INST_T *prJdecInst)
{
    UINT32 u4RdPoint, u4RdPointBitShift;
    JDEC_FIFO_INFO_T *prFifoInfo = &(prJdecInst->rFifoInfo);
    INT32 i4Ret = S_JDEC_HAL_OK;

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    u4RdPoint = u4JDEC_HAL_Get_VLD_RPtr(prJdecInst->u4HwInstId);
    u4RdPoint -= 64; //  for some hw limitation
    u4RdPointBitShift = 0;
    vJdecRdPtrPrintf("[JDEC] Update rd ptr to %x , fifo start = %x \n",u4RdPoint,prJdecInst->rFifoInfo.u4FifoStart);

#else
    i4Ret = i4JDEC_HAL_Get_Read_Pointer(prJdecInst->u4HwInstId, &u4RdPoint, &u4RdPointBitShift);
#endif
    if (i4Ret == S_JDEC_HAL_OK)
    {
        if(u4RdPointBitShift > 0)
            u4RdPoint++;

        if(u4RdPoint < prFifoInfo->u4FifoEnd)
        {
            prFifoInfo->u4RdPoint = u4RdPoint;
        }
        else if (u4RdPoint == prFifoInfo->u4FifoEnd)
        {
            prFifoInfo->u4RdPoint = prFifoInfo->u4FifoStart;
        }
        else
        {
            //ASSERT(0);
            return JDEC_DRV_RTN_FAIL;
        }
        //UTIL_Printf("[JDEC] Hw read point 0x%x \n",prFifoInfo->u4RdPoint);
    }
    else
    {
        if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
            || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif
            )
        {
            i4Ret = i4JDEC_HAL_Get_Decode_Status(prJdecInst->u4HwInstId);
            if (i4Ret == S_JDEC_HAL_OK)
            {
                // decoding finish
                x_timer_stop(prJdecInst->hJdecDecTimer);
                prJdecInst->i4JdecDecTimerStatus = 0;

                return JDEC_DRV_RTN_OK;
            }
        }
        //ASSERT(0);
        return JDEC_DRV_RTN_FAIL;
    }
    return JDEC_DRV_RTN_OK;
}

INT32 i4JdecUpdateCurrentFileOfst(JDEC_INST_T *prJdecInst, UINT32 u4SkipMoreData)
{
    UINT32 u4ValTmp;
    JDEC_FIFO_INFO_T *prFifoInfo = &(prJdecInst->rFifoInfo);

    vJdecRdPtrPrintf("[JDEC] i4JdecUpdateCurrentFileOfst start , Rd = %x, preRd = %x, file ofst = %x , image size = %x \n",
                prFifoInfo->u4RdPoint, prFifoInfo->u4PreRdPoint, prFifoInfo->u4FileOfst, prJdecInst->u4ImgSize);
    if (prFifoInfo->u4RdPoint >= prFifoInfo->u4PreRdPoint)
    {
        u4ValTmp = prFifoInfo->u4RdPoint - prFifoInfo->u4PreRdPoint;
    }
    else
    {
        // For error handle ,hw will jump back for marker found
        if ((prFifoInfo->u4PreRdPoint - prFifoInfo->u4RdPoint) == 2)
        {
            u4ValTmp = 0;
        }
        else
        {
        u4ValTmp = prFifoInfo->u4FifoEnd - prFifoInfo->u4PreRdPoint;
        u4ValTmp += prFifoInfo->u4RdPoint - prFifoInfo->u4FifoStart;
    }
    }
    prFifoInfo->u4FileOfst += u4ValTmp;
    prFifoInfo->u4PreRdPoint = prFifoInfo->u4RdPoint;
    prFifoInfo->u4FileOfst += u4SkipMoreData;

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    vJdecRdPtrPrintf("[JDEC]i4JdecUpdateCurrentFileOfst Offset update to %x \n" ,prFifoInfo->u4FileOfst);
    if (prFifoInfo->u4FileOfst > (prJdecInst->u4ImgSize + 256))
#else
    if (prFifoInfo->u4FileOfst > (prJdecInst->u4ImgSize + 160))
#endif
    {
        UTIL_Printf("[JDEC] FileOfst overange %d \n", (prFifoInfo->u4FileOfst - prJdecInst->u4ImgSize));
        return JDEC_DRV_RTN_FAIL;
    }
    else
    {
        return JDEC_DRV_RTN_OK;
    }
}

INT32 i4JdecUpdateImgdmaRdPoint(JDEC_INST_T *prJdecInst)
{
    INT32 i4Ret;
    JDEC_FIFO_INFO_T *prFifoInfo = &(prJdecInst->rFifoInfo);

    i4Ret = i4JdecUpdateCurrentFileOfst(prJdecInst, 0);
    if (i4Ret == JDEC_DRV_RTN_FAIL)
    {
        return JDEC_DRV_RTN_FAIL;
    }
    if (prFifoInfo->fgEoRange == FALSE)
    {
 #if JPG_SUPPORT_DIRECT_DECODE_MODE
        if(prJdecInst->fgNoJpgCbToUSMode)
        {
           x_sema_lock(prJdecInst->hJdecSrcfifoSema, X_SEMA_OPTION_WAIT);
           prJdecInst->tUsrTempFifoInfo.u4RdPoint = prJdecInst->rFifoInfo.u4RdPoint;
           if (prJdecInst->tUsrTempFifoInfo.u4RdPoint > prJdecInst->tUsrTempFifoInfo.u4WrPoint)
           {
               prJdecInst->tUsrTempFifoInfo.u4ValidData = prJdecInst->rFifoInfo.u4FifoEnd - prJdecInst->tUsrTempFifoInfo.u4RdPoint;
               prJdecInst->tUsrTempFifoInfo.u4ValidData += prJdecInst->tUsrTempFifoInfo.u4WrPoint - prJdecInst->rFifoInfo.u4FifoStart;
           }
           else
           {
               prJdecInst->tUsrTempFifoInfo.u4ValidData = prJdecInst->tUsrTempFifoInfo.u4WrPoint - prJdecInst->tUsrTempFifoInfo.u4RdPoint;
           }
           x_sema_unlock(prJdecInst->hJdecSrcfifoSema);
        }
        else
      #endif
        {
        i4Ret = i4ImgdmaSetRdPoint(prJdecInst->u4ImgdmaId, prFifoInfo->u4RdPoint);
        VERIFY(i4Ret == IMGDMA_OK);
    }
    }

    return JDEC_DRV_RTN_OK;
}

INT32 i4JdecChkFifoStatus(JDEC_INST_T *prJdecInst)
{
    INT32 i4Ret;
    IMGDMA_DRV_FIFO_STATUS_T rFifoStatus;
    UINT32 u4FifoThresHold = 32 *1024;//((prJdecInst->rFifoInfo.u4FifoEnd - prJdecInst->rFifoInfo.u4FifoStart) / 2);
    JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;

    //for software jpg decoding
    if (prJdecInst->ptImgJpgObj != NULL)
    {
        u4FifoThresHold = ((prJdecInst->rFifoInfo.u4FifoEnd - prJdecInst->rFifoInfo.u4FifoStart) / 2);
    }
    //need to add timeout code
    if (prJdecInst->rFifoInfo.fgEoRange)
    {
        if (prJdecInst->rFifoInfo.u4RdPoint > prJdecInst->rFifoInfo.u4WrPoint)
        {
            prJdecInst->rFifoInfo.u4ValidData = prJdecInst->rFifoInfo.u4FifoEnd - prJdecInst->rFifoInfo.u4RdPoint;
            prJdecInst->rFifoInfo.u4ValidData += prJdecInst->rFifoInfo.u4WrPoint - prJdecInst->rFifoInfo.u4FifoStart;
        }
        else
        {
            prJdecInst->rFifoInfo.u4ValidData = prJdecInst->rFifoInfo.u4WrPoint - prJdecInst->rFifoInfo.u4RdPoint;
        }
        return JDEC_DRV_RTN_OK;
    }
 #if JPG_SUPPORT_DIRECT_DECODE_MODE
    if(prJdecInst->fgNoJpgCbToUSMode)
    {
       x_memcpy((void*)(&rFifoStatus), (void*)(&prJdecInst->tUsrTempFifoInfo),sizeof(IMGDMA_DRV_FIFO_STATUS_T));
    }
    else
 #endif
    {
    //get current rd/wr point
    i4Ret = i4ImgdmaGetFifoStatus(prJdecInst->u4ImgdmaId, &rFifoStatus);
    VERIFY(i4Ret == IMGDMA_OK);
    }

    prJdecInst->rFifoInfo.fgEoRange = rFifoStatus.fgEoRange;
    prJdecInst->rFifoInfo.u4WrPoint = rFifoStatus.u4WrPoint;
    prJdecInst->rFifoInfo.u4ValidData = rFifoStatus.u4ValidData;

    if (prJdecInst->eJdecState == JDEC_STATE_DECODING)
    {
        if (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW)
        {
            u4FifoThresHold = (prJfifInst->rSOF.u2ImageWidth / 8) * 160;
        }
    }

    if (prJdecInst->fgParsingHeaderWaitData)
    {
        if (rFifoStatus.u4ValidData >= prJdecInst->u4NeedData)
        {
            prJdecInst->fgParsingHeaderWaitData = FALSE;
            prJdecInst->u4NeedData = 0;
            return JDEC_DRV_RTN_OK;
        }
        else
        {
            //i4JdecSetTrnsTimer(prJdecInst, 6000);
            i4Ret = i4JdecWaitTrnsTimeOut(prJdecInst);
            return i4Ret;
        }
    }
    else
    {
        if ((rFifoStatus.u4ValidData >= u4FifoThresHold) || (rFifoStatus.fgEoRange == TRUE))
        {
            if (prJdecInst->i4JdecTrnsTimerStatus == 1)
            {
                i4Ret = x_timer_stop(prJdecInst->hJdecTrnsTimer);
                prJdecInst->i4JdecTrnsTimerStatus = 0;
            }
            return JDEC_DRV_RTN_OK;
        }
        else
        {
            //i4JdecSetTrnsTimer(prJdecInst, 6000);
            i4Ret = i4JdecWaitTrnsTimeOut(prJdecInst);
            return i4Ret;
        }
    }

}

void vJdecUpdateWritePointer(VOID *pv_tag, VOID *pv_data)
{
    JDEC_INST_T *prJdecInst = &(_rJdecInst[(UINT32)pv_data]);
    BOOL fgSendEvent = *(BOOL *)pv_tag;

    if ((prJdecInst->eJdecDecMode != JDEC_HAL_DEC_MODE_BASELINE_PIC)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        && (prJdecInst->eJdecDecMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
        && (prJdecInst->eJdecDecMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif
        )
    {
        return;
    }
    else
    {
        if ((prJdecInst->eJdecDecState != JDEC_DECODE_WAIT_DECODE)
//#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
//            && (prJdecInst->eJdecDecState != JDEC_DECODE_DO_DECODE)
//#endif
            )
        {
            return;
        }
        if (i4JdecGetHalRdPoint(prJdecInst) == JDEC_DRV_RTN_FAIL)
        {
            prJdecInst->i4JdecErrorType = -E_JPG_ERR_HAL_GET_RD;
            return;
        }
        #if JDEC_USE_MAP_UNMAP
        if(prJdecInst->rFifoInfo.u4RdPoint > prJdecInst->rFifoInfo.u4PreRdPoint)
        {
            UINT32 u4Start, u4End;

            u4Start = prJdecInst->rFifoInfo.u4PreRdPoint/JDEC_CACHE_LINE_ALIGN*JDEC_CACHE_LINE_ALIGN;
            u4End = prJdecInst->rFifoInfo.u4RdPoint/JDEC_CACHE_LINE_ALIGN*JDEC_CACHE_LINE_ALIGN;
            vJdecMapPrintf("[JDEC] Unmap from 0x%x to 0x%x point 2\n",u4Start, u4End);
            BSP_dma_unmap_vaddr(u4Start, (u4End - u4Start),TO_DEVICE);
            
        }
        else
        {
            UINT32 u4Start, u4End;

            u4Start = prJdecInst->rFifoInfo.u4PreRdPoint/JDEC_CACHE_LINE_ALIGN*JDEC_CACHE_LINE_ALIGN;
            u4End = prJdecInst->rFifoInfo.u4RdPoint/JDEC_CACHE_LINE_ALIGN*JDEC_CACHE_LINE_ALIGN;

            vJdecMapPrintf("[JDEC] Unmap from 0x%x to 0x%x point 3\n",u4Start, prJdecInst->rFifoInfo.u4FifoEnd);
            vJdecMapPrintf("[JDEC] Unmap from 0x%x to 0x%x point 3\n",prJdecInst->rFifoInfo.u4FifoStart, u4End);
            BSP_dma_unmap_vaddr(u4Start, (prJdecInst->rFifoInfo.u4FifoEnd - u4Start), TO_DEVICE);
            BSP_dma_unmap_vaddr(prJdecInst->rFifoInfo.u4FifoStart,(u4End- prJdecInst->rFifoInfo.u4FifoStart),TO_DEVICE);
            
        }
        #endif
        // update read pointer
        {
            INT32 i4Ret;
            JDEC_FIFO_INFO_T *prFifoInfo = &(prJdecInst->rFifoInfo);

            if (prJdecInst->ptImgJpgObj == NULL)
            {
                i4Ret = i4JdecUpdateCurrentFileOfst(prJdecInst, 0);
                if (i4Ret == JDEC_DRV_RTN_FAIL)
                {
                    prJdecInst->i4JdecErrorType = -E_JPG_ERR_HAL_UPT_RD;
                    return;
                }
            }
            if (prFifoInfo->fgEoRange == FALSE)
            {
#if JPG_SUPPORT_DIRECT_DECODE_MODE
                if(prJdecInst->fgNoJpgCbToUSMode)
                {
                   if(!prJdecInst->fgNoLockWrSemphore)
                      x_sema_lock(prJdecInst->hJdecSrcfifoSema, X_SEMA_OPTION_WAIT);
                   prJdecInst->tUsrTempFifoInfo.u4RdPoint = prJdecInst->rFifoInfo.u4RdPoint;
                   if (prJdecInst->tUsrTempFifoInfo.u4RdPoint > prJdecInst->tUsrTempFifoInfo.u4WrPoint)
                   {
                       prJdecInst->tUsrTempFifoInfo.u4ValidData = prJdecInst->rFifoInfo.u4FifoEnd - prJdecInst->tUsrTempFifoInfo.u4RdPoint;
                       prJdecInst->tUsrTempFifoInfo.u4ValidData += prJdecInst->tUsrTempFifoInfo.u4WrPoint - prJdecInst->rFifoInfo.u4FifoStart;
                   }
                   else
                   {
                       prJdecInst->tUsrTempFifoInfo.u4ValidData = prJdecInst->tUsrTempFifoInfo.u4WrPoint - prJdecInst->tUsrTempFifoInfo.u4RdPoint;
                   }
		           if(!prJdecInst->fgNoLockWrSemphore)
                      x_sema_unlock(prJdecInst->hJdecSrcfifoSema);
                }
                else
#endif
                {
                    if (fgSendEvent)
                    {
                        i4Ret = i4ImgdmaSetRdPoint(prJdecInst->u4ImgdmaId, prFifoInfo->u4RdPoint);
                    }
                    else
                    {
                        i4Ret = i4ImgdmaSetRdPointOnly(prJdecInst->u4ImgdmaId, prFifoInfo->u4RdPoint);
                    }
                    VERIFY(i4Ret == IMGDMA_OK);
                }
            }
        }
        if (prJdecInst->rFifoInfo.fgEoRange)
        {
            if (prJdecInst->rFifoInfo.u4RdPoint > prJdecInst->rFifoInfo.u4WrPoint)
            {
                prJdecInst->rFifoInfo.u4ValidData = prJdecInst->rFifoInfo.u4FifoEnd - prJdecInst->rFifoInfo.u4RdPoint;
                prJdecInst->rFifoInfo.u4ValidData += prJdecInst->rFifoInfo.u4WrPoint - prJdecInst->rFifoInfo.u4FifoStart;
            }
            else
            {
                prJdecInst->rFifoInfo.u4ValidData = prJdecInst->rFifoInfo.u4WrPoint - prJdecInst->rFifoInfo.u4RdPoint;
            }
        }
        else
        {
            INT32 i4Ret;
            IMGDMA_DRV_FIFO_STATUS_T rFifoStatus;

#if JPG_SUPPORT_DIRECT_DECODE_MODE
            if(prJdecInst->fgNoJpgCbToUSMode)
            {
              if(!prJdecInst->fgNoLockWrSemphore)
                    x_sema_lock(prJdecInst->hJdecSrcfifoSema, X_SEMA_OPTION_WAIT);
                x_memcpy((void*)(&rFifoStatus), (void*)(&prJdecInst->tUsrTempFifoInfo),sizeof(IMGDMA_DRV_FIFO_STATUS_T));
					   if(!prJdecInst->fgNoLockWrSemphore)
                      x_sema_unlock(prJdecInst->hJdecSrcfifoSema);
            }
            else
#endif
            {
            //get current rd/wr point
            i4Ret = i4ImgdmaGetFifoStatus(prJdecInst->u4ImgdmaId, &rFifoStatus);
            VERIFY(i4Ret == IMGDMA_OK);
            }
            prJdecInst->rFifoInfo.fgEoRange = rFifoStatus.fgEoRange;
            prJdecInst->rFifoInfo.u4WrPoint = rFifoStatus.u4WrPoint;
            prJdecInst->rFifoInfo.u4ValidData = rFifoStatus.u4ValidData;
        }
        
#if JDEC_USE_MAP_UNMAP
        // map new copyed data
        if (prJdecInst->rFifoInfo.u4WrPoint > prJdecInst->u4MapSA)
        {
            UINT32 u4Start, u4End;

            u4Start = prJdecInst->u4MapSA/JDEC_CACHE_LINE_ALIGN*JDEC_CACHE_LINE_ALIGN;
            u4End = prJdecInst->rFifoInfo.u4WrPoint /JDEC_CACHE_LINE_ALIGN * JDEC_CACHE_LINE_ALIGN;
            vJdecMapPrintf("[JDEC]Map from 0x%x to 0x%x , point 1 \n",u4Start, u4End);
            BSP_dma_map_vaddr(u4Start, (u4End - u4Start),TO_DEVICE);
            
        }
        else if (prJdecInst->rFifoInfo.u4WrPoint < prJdecInst->u4MapSA)
        {
            UINT32 u4Start, u4End;

            u4Start = prJdecInst->u4MapSA/JDEC_CACHE_LINE_ALIGN*JDEC_CACHE_LINE_ALIGN;
            u4End = prJdecInst->rFifoInfo.u4WrPoint /JDEC_CACHE_LINE_ALIGN * JDEC_CACHE_LINE_ALIGN;

            vJdecMapPrintf("[JDEC]Map from 0x%x to 0x%x , point 2 \n",u4Start, prJdecInst->rFifoInfo.u4FifoEnd);
            vJdecMapPrintf("[JDEC]Map from 0x%x to 0x%x , point 2 \n",prJdecInst->rFifoInfo.u4FifoStart, u4End);
            BSP_dma_map_vaddr(u4Start, (prJdecInst->rFifoInfo.u4FifoEnd - u4Start),TO_DEVICE);
            BSP_dma_map_vaddr(prJdecInst->rFifoInfo.u4FifoStart,(u4End - prJdecInst->rFifoInfo.u4FifoStart),TO_DEVICE);
            
        }
        prJdecInst->u4MapSA = prJdecInst->rFifoInfo.u4WrPoint;
#endif
        if (prJdecInst->rFifoInfo.fgEoRange)
        {
		    vJdecRdPtrPrintf("Update0 HW prJdecInst->rFifoInfo.u4WrPoint =0x%x\n", prJdecInst->rFifoInfo.u4WrPoint);
            i4JDEC_HAL_Set_Write_Pointer(prJdecInst->u4HwInstId,prJdecInst->rFifoInfo.u4WrPoint + 64*4);             
        }
        else
        {            
            #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)            
            if (prJdecInst->rFifoInfo.u4WrPoint == prJdecInst->rFifoInfo.u4FifoStart)
            {
	            vJdecRdPtrPrintf("Update1 HW prJdecInst->rFifoInfo.u4FifoEnd \n");
                i4JDEC_HAL_Set_Write_Pointer(prJdecInst->u4HwInstId,(prJdecInst->rFifoInfo.u4FifoEnd - 16));
            }
            else
            #endif
            {
		        vJdecRdPtrPrintf("Update2 HW prJdecInst->rFifoInfo.u4WrPoint =0x%x\n", prJdecInst->rFifoInfo.u4WrPoint);
                i4JDEC_HAL_Set_Write_Pointer(prJdecInst->u4HwInstId,prJdecInst->rFifoInfo.u4WrPoint);               
            }            
        }            
    }
    return;
}


INT32 i4JdecStartTranData(JDEC_INST_T *prJdecInst, UINT32 u4TrsnOfst, UINT32 u4Size)
{
    INT32 i4Ret;
    IMGDMA_DRV_INIT_INFO_T rImgdmaInit;
    IMGDMA_DRV_FIFO_STATUS_T rFifoStatus;
    JDEC_FIFO_INFO_T *prFifoInfo = &(prJdecInst->rFifoInfo);
#if JPG_SUPPORT_DIRECT_DECODE_MODE
    if(prJdecInst->fgNoJpgCbToUSMode)
    {
       prJdecInst->tUsrTempFifoInfo.u4RdPoint   = prFifoInfo->u4FifoStart;
       prJdecInst->tUsrTempFifoInfo.u4WrPoint   = prFifoInfo->u4FifoStart;
       prJdecInst->tUsrTempFifoInfo.u4ValidData = 0;
       prJdecInst->tUsrTempFifoInfo.fgEoRange   = FALSE;

       prFifoInfo->u4RdPoint    = prJdecInst->tUsrTempFifoInfo.u4RdPoint;
       prFifoInfo->u4PreRdPoint = prFifoInfo->u4RdPoint;
       prFifoInfo->u4WrPoint    = prJdecInst->tUsrTempFifoInfo.u4WrPoint;
       prFifoInfo->u4ValidData  = prJdecInst->tUsrTempFifoInfo.u4ValidData;
       prFifoInfo->fgEoRange    = prJdecInst->tUsrTempFifoInfo.fgEoRange;
       return JDEC_DRV_RTN_OK;
    }
  #endif

    //reset
    i4Ret = i4ImgdmaSetReset(prJdecInst->u4ImgdmaId);
    VERIFY(i4Ret == IMGDMA_OK);
    //set fifo
    rImgdmaInit.u4ImgId = prJdecInst->u4ImgId;
    rImgdmaInit.u4FifoStart = prJdecInst->rFifoInfo.u4FifoStart;
    rImgdmaInit.u4FifoEnd = prJdecInst->rFifoInfo.u4FifoEnd;
    #if UNIFORM_DRV_CALLBACK
    rImgdmaInit.rNfyInfo.pf_cb_nfy_fct= prJdecInst->rNfyInfo.pf_cb_nfy_fct;
    rImgdmaInit.rNfyInfo.pv_nfy_tag= prJdecInst->rNfyInfo.pv_nfy_tag;
    #else
    rImgdmaInit.pfImgDecCb = prJdecInst->pfNfyCb;
    rImgdmaInit.pvImgDecTag = prJdecInst->pvTag;
    #endif
    rImgdmaInit.pfImgUpdCb = vJdecUpdateWritePointer;
#if CONFIG_SYS_MEM_PHASE3
    rImgdmaInit.hFifo = prJdecInst->rFifoInfo.hFifo;
#else
    rImgdmaInit.hFifo = NULL_HANDLE;
#endif
    i4Ret = i4ImgdmaSetFifoInfo(prJdecInst->u4ImgdmaId, rImgdmaInit);
    VERIFY(i4Ret == IMGDMA_OK);
    //set range
    i4Ret = i4ImgdmaSetRange(prJdecInst->u4ImgdmaId, u4TrsnOfst, u4Size);
    VERIFY(i4Ret == IMGDMA_OK);

#if JDEC_DISCONTINUOUS_SOURCE
    if (prJdecInst->pvImgBuf != NULL)
    {
        IMGDMA_DRV_SRC_INFO_T rSrcInfo;

        x_memset(&rSrcInfo, 0, sizeof(IMGDMA_DRV_SRC_INFO_T));

        rSrcInfo.eType = DRVT_JPG_DEC;
        rSrcInfo.fgCpy = TRUE;
        rSrcInfo.u2DrvCompId = prJdecInst->u2CompId;
        rSrcInfo.u4SrcSa = (UINT32)prJdecInst->pvImgBuf;


        i4ImgdmaSetSrcInfo(prJdecInst->u4ImgdmaId, &rSrcInfo);
    }
#endif
    //init fifo val
    vJdecInitFifoVal(prJdecInst);
    //get current rd/wr point
    i4Ret = i4ImgdmaGetFifoStatus(prJdecInst->u4ImgdmaId, &rFifoStatus);
    VERIFY(i4Ret == IMGDMA_OK);
    prFifoInfo->u4RdPoint = rFifoStatus.u4RdPoint;
    prFifoInfo->u4PreRdPoint = rFifoStatus.u4RdPoint;
    prFifoInfo->u4WrPoint = rFifoStatus.u4WrPoint;
    prFifoInfo->u4ValidData = rFifoStatus.u4ValidData;
    prFifoInfo->fgEoRange = rFifoStatus.fgEoRange;
    //start transfer data
    i4Ret = i4ImgdmaSetStart(prJdecInst->u4ImgdmaId);
    VERIFY(i4Ret == IMGDMA_OK);

    return JDEC_DRV_RTN_OK;
}

INT32 i4JdecUpdateFifoRdPoint(JDEC_INST_T *prJdecInst, UINT32 u4Advance)
{
    JDEC_FIFO_INFO_T *prFifoInfo = &(prJdecInst->rFifoInfo);

    if ((prFifoInfo->u4RdPoint + u4Advance) >= prFifoInfo->u4FifoEnd)
    {
        prFifoInfo->u4RdPoint = prFifoInfo->u4FifoStart + ((prFifoInfo->u4RdPoint + u4Advance) - prFifoInfo->u4FifoEnd);
    }
    else
    {
        prFifoInfo->u4RdPoint += u4Advance;
    }

    return JDEC_DRV_RTN_OK;
}

void vJdecCopyJfif2PicCache(JDEC_INST_T *prJdecInst)
{
    JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;
    JDEC_PIC_INFO_T *prJdecPicInfo = (JDEC_PIC_INFO_T *)prJdecInst->pvPicCache;
    JDEC_JFIF_SOF_T *prSof = &(prJfifInst->rSOF);
    INT32 i4 = 0;

    prJdecPicInfo->rSosInfo = prJfifInst->rSOS;
    prJdecPicInfo->rQTblInfo = prJfifInst->rDQT;
    prJdecPicInfo->rHuffTblInfo = prJfifInst->rDHT;
    prJdecPicInfo->rSofInfo = prJfifInst->rSOF;
    prJdecPicInfo->u4Ofst1stSOS = prJfifInst->u4BitStmSrtOfst;
    prJdecPicInfo->u4RestartInterval = prJfifInst->u2RestartInterval;

    prJdecPicInfo->u4MaxHFactor = 0;
    prJdecPicInfo->u4MaxVFactor = 0;
    if (prSof->u1NumComponents > MAX_JPEG_COMPONENT)
    {
        ASSERT(0);
    }
    for(i4=0; i4<prSof->u1NumComponents; i4++)
    {
        if (prSof->arSofComp[i4].u1HSampFactor > prJdecPicInfo->u4MaxHFactor)
        {
            prJdecPicInfo->u4MaxHFactor = prSof->arSofComp[i4].u1HSampFactor;
        }
        if (prSof->arSofComp[i4].u1VSampFactor > prJdecPicInfo->u4MaxVFactor)
        {
            prJdecPicInfo->u4MaxVFactor = prSof->arSofComp[i4].u1VSampFactor;
        }
    }
}

INT32 i4JdecPerScanSetup(JDEC_INST_T *prJdecInst)
{
    JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;
#if JDEC_CACHE_ENABLE
    JDEC_PIC_INFO_T *prJdecPicInfo = (JDEC_PIC_INFO_T *)prJdecInst->pvPicCache;
#endif
    JDEC_JFIF_SOF_T *prSof = &(prJfifInst->rSOF);
    JDEC_JFIF_SOS_T *prSos = &(prJfifInst->rSOS);
    UINT32 u4PicWidth = prSof->u2ImageWidth;
    UINT32 u4PicHeight = prSof->u2ImageHeight;
    INT32 i4 = 0;

    prJdecInst->u4ScanCount++;

    if ((prSos->u1Se == 63) && (prSos->u1Al == 0))
    {
        prJdecInst->fgLastScan = TRUE;
    }
    else
    {
        prJdecInst->fgLastScan = FALSE;
    }
    prJdecInst->u4CurrDecodeMCURow = 0;

    if (prSos->u1CompInScan > JDEC_COMP_NUM)
    {
        ASSERT(0);
    }
    for (i4=0; i4<prSos->u1CompInScan; i4++)
    {
        UINT32 u4CompIdx = prSos->au1CompIdx[i4];
        if (u4CompIdx < JDEC_COMP_NUM)
        {
            prJdecInst->au4CompScanCount[u4CompIdx] += 1;
        }
        else
        {
            ASSERT(0);
            return JDEC_DRV_RTN_FAIL;
        }
    }

    if (prSos->u1CompInScan == 1)
    {
        UINT32 u4CompIdx = prSos->au1CompIdx[0];
        if (u4CompIdx > (MAX_JPEG_COMPONENT - 1))
        {
            ASSERT(0);
        }
        //prJdecInst->u4MCUInMCURowW = (((u4PicWidth + 7) / 8) * prSof->arSofComp[u4CompIdx].u1HSampFactor + (prJdecPicInfo->u4MaxHFactor - 1)) / (prJdecPicInfo->u4MaxHFactor);
        prJdecInst->u4MCUInMCURowW = ((u4PicWidth * prSof->arSofComp[u4CompIdx].u1HSampFactor) + (prJdecPicInfo->u4MaxHFactor * 8 - 1)) / (prJdecPicInfo->u4MaxHFactor * 8);
        prJdecInst->u4MCUInMCURowH = prSof->arSofComp[u4CompIdx].u1VSampFactor;
        //prJdecInst->u4MCURowNum = (((u4PicHeight + 7) / 8) * prSof->arSofComp[u4CompIdx].u1VSampFactor + (prJdecPicInfo->u4MaxVFactor - 1)) / (prJdecPicInfo->u4MaxVFactor);
        prJdecInst->u4MCURowNum = (u4PicHeight + (prJdecPicInfo->u4MaxVFactor * 8 - 1)) / (prJdecPicInfo->u4MaxVFactor * 8);
        /*Temp solution*/
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT) ||
            (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE) ||
            (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT))
        {
            if (prSof->arSofComp[u4CompIdx].u1VSampFactor == 2)
            {
                prJdecInst->u4MCURowNum = (u4PicHeight + 7)/8;
                prJdecInst->u4MCUInMCURowH = 1;
            }
        }
#endif
    }
    else
    {
        prJdecInst->u4MCUInMCURowW = (u4PicWidth + (prJdecPicInfo->u4MaxHFactor * 8 - 1)) / (prJdecPicInfo->u4MaxHFactor * 8);
        prJdecInst->u4MCUInMCURowH = 1;
        prJdecInst->u4MCURowNum = (u4PicHeight + (prJdecPicInfo->u4MaxVFactor * 8 - 1)) / (prJdecPicInfo->u4MaxVFactor * 8);
    }
    //UTIL_Printf("[JDEC] u4MCURowNum = %d \n",prJdecInst->u4MCURowNum);

#if JDEC_SUPPORT_BROKEN_TARGET
    //prJdecInst->u4McuRowLength = prJdecInst->u4DstBufPitch * prJdecPicInfo->u4MaxVFactor * 8;

    if (prSos->u1CompInScan == 1)
    {
        UINT32 u4CompIdx = prSos->au1CompIdx[0];

        prJdecInst->u4McuRowLength = prJdecInst->u4DstBufPitch* 8;
        if ((prJdecPicInfo->u4MaxVFactor == 2) && (prSof->arSofComp[u4CompIdx].u1VSampFactor != 2))
        {
            prJdecInst->u4McuRowLength = prJdecInst->u4DstBufPitch * prJdecPicInfo->u4MaxVFactor * 8;
        }
    }
    else
    {
        prJdecInst->u4McuRowLength = prJdecInst->u4DstBufPitch * prJdecPicInfo->u4MaxVFactor * 8;
    }
    vJdecLog_L("[JDEC]DstBufPitch = 0x%x u4McuRowLength = 0x%x \n",prJdecInst->u4DstBufPitch, prJdecInst->u4McuRowLength);
#endif
    return JDEC_DRV_RTN_OK;
}

INT32 i4JdecCaluColorBuf(JDEC_INST_T *prJdecInst)
{
    JDEC_PIC_INFO_T *prJdecPicInfo = (JDEC_PIC_INFO_T *)prJdecInst->pvPicCache;
    JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;
    JDEC_JFIF_SOF_T *prSof = &(prJfifInst->rSOF);
    UINT32 u4PicWidthInMCUAlign = 0;
    UINT32 u4ValTmp;
    INT32 i4Count1, i4Count2;
#if JDEC_SUPPORT_BROKEN_TARGET
    HANDLE_T hTemp = NULL_HANDLE;
#endif
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    UINT32 u4Temp = 0;
    #endif

    u4ValTmp = prJdecPicInfo->u4MaxHFactor * 8;
    u4PicWidthInMCUAlign = ((prSof->u2ImageWidth + (u4ValTmp - 1)) / u4ValTmp) * u4ValTmp;  //MCU alignment
    u4PicWidthInMCUAlign = ((u4PicWidthInMCUAlign + 15) >> 4) << 4;     //because img resz need 16byte align

    //Y, Cb, Cr are the same because img resz only has one register to set src buffer length
    prJdecInst->au4ColorBufPitch[0] = u4PicWidthInMCUAlign;

#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560)

    prJdecInst->au4ColorBufPitch[1] = u4PicWidthInMCUAlign;
    prJdecInst->au4ColorBufPitch[2] = u4PicWidthInMCUAlign;
#else
    u4Temp = prSof->arSofComp[0].u1HSampFactor / prSof->arSofComp[1].u1HSampFactor;
    prJdecInst->au4ColorBufPitch[1] = (u4PicWidthInMCUAlign / u4Temp + 15) / 16 * 16;
    u4Temp = prSof->arSofComp[0].u1HSampFactor /prSof->arSofComp[2].u1HSampFactor;
    prJdecInst->au4ColorBufPitch[2] = (u4PicWidthInMCUAlign / u4Temp + 15) / 16 * 16;
#endif
#if 0
    prJdecInst->au4ColorBufSz[0] = u4PicWidthInMCUAlign * prSof->arSofComp[0].u1VSampFactor * 8;
    prJdecInst->au4ColorBufSz[1] = u4PicWidthInMCUAlign * prSof->arSofComp[1].u1VSampFactor * 8;
    prJdecInst->au4ColorBufSz[2] = u4PicWidthInMCUAlign * prSof->arSofComp[2].u1VSampFactor * 8;
#else
    prJdecInst->au4ColorBufSz[0] = u4PicWidthInMCUAlign * prSof->arSofComp[0].u1VSampFactor * 8;
    prJdecInst->au4ColorBufSz[1] = prJdecInst->au4ColorBufSz[0];
    prJdecInst->au4ColorBufSz[2] = prJdecInst->au4ColorBufSz[0];


#endif

    for (i4Count1=0; i4Count1<JDEC_CLR_BUF_NUM; i4Count1++)
    {
        for (i4Count2=0; i4Count2<JDEC_COMP_NUM; i4Count2++)
        {
            if (prJdecInst->au4ColorBufSa[i4Count1][i4Count2] != JDEC_INVALID32)
            {
                ASSERT(0);
                prJdecInst->i4JdecErrorType = -E_JPG_GET_CLRBUFMEM_FAIL;
                return JDEC_DRV_RTN_FAIL;
            }
            #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
            u4ValTmp = (UINT32)x_alloc_aligned_dma_mem(prJdecInst->au4ColorBufSz[0], 32);
#elif CONFIG_SYS_MEM_PHASE3
            u4ValTmp = (UINT32)x_mem_aligned_alloc(prJdecInst->au4ColorBufSz[0], 32);
#endif
            #else
            u4ValTmp = (UINT32)x_alloc_aligned_nc_mem(prJdecInst->au4ColorBufSz[0], 32);
            #endif
            if (u4ValTmp == 0)
            {
                prJdecInst->i4JdecErrorType = -E_JPG_GET_CLRBUFMEM_FAIL;
                return JDEC_DRV_RTN_FAIL;
            }
            else
            {
                prJdecInst->au4ColorBufSa[i4Count1][i4Count2] = u4ValTmp;
                vJdecLog_L("[JDEC]Color Buffer[%d][%d] SA = %x \n",i4Count1, i4Count2,u4ValTmp);
                if (i4Count2 >= prSof->u1NumComponents)
                {
                    prJdecInst->fgDefaultScale[i4Count2] = TRUE;
                    if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_MCU) ||
                        (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) ||
                        (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)||
                         (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_PIC))    
                    {
                        x_memset((void*)u4ValTmp, 0x80, prJdecInst->au4ColorBufSz[0]);
                        #if JDEC_USE_MAP_UNMAP
                        vJdecMapPrintf("[JDEC]Map color buffer from 0x%x to 0x%x , point 3 \n",u4ValTmp, (u4ValTmp + prJdecInst->au4ColorBufSz[0]));
                        BSP_dma_map_vaddr(u4ValTmp, prJdecInst->au4ColorBufSz[0], BIDIRECTIONAL);
                        #else
                        BSP_FlushDCacheRange(u4ValTmp, prJdecInst->au4ColorBufSz[0]);
                        #endif
                    }
                }
            }
        }
    }

#if JDEC_SUPPORT_BROKEN_TARGET
    if (prJdecInst->fgBrokenTarget)
    {
        // allocate AYUV buffer for mcu row data convert
        hTemp = x_kmem_block_aligned_alloc((SIZE_T)(prJdecInst->au4ColorBufSz[0] * 4), 32);
        prJdecInst->u4TempAYUVBuf = (UINT32)x_kmem_kernel_address(hTemp, 0);
        prJdecInst->hTempAYUVBuf = hTemp;
        vJdecLog_L("[JDEC] TempAYUVBuf = %x \n",prJdecInst->u4TempAYUVBuf);

        // allocate temp line buffer
        hTemp = x_kmem_block_aligned_alloc(prJdecInst->u4DstBufPitch, 32);
        prJdecInst->u4TempLineBuf = (UINT32)x_kmem_kernel_address(hTemp, 0);
        prJdecInst->hTempLineBuf = hTemp;
        vJdecLog_L("[JDEC] u4TempLineBuf = %x \n",prJdecInst->u4TempLineBuf);
    }
#endif
    return JDEC_DRV_RTN_OK;
}

INT32 i4JdecCaluOutputBuf(JDEC_INST_T *prJdecInst, JDEC_HAL_OUTPUT_INFO_T *ptOutputInfo)
{
    UINT32 u4CurrentBufId = (prJdecInst->u4CurrDecodeMCURow  % JDEC_CLR_BUF_NUM);
    JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;
    JDEC_JFIF_SOS_T *prSos = &(prJfifInst->rSOS);
    INT32 i4;
    UINT32 u4CompIdx;

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    if (prJdecInst->u4CurrDecodeMCURow > 0)
    {
        u4CurrentBufId = (prJdecInst->u4CurrDecodeMCURow + 1) % JDEC_CLR_BUF_NUM;
    }
#endif

    ptOutputInfo->u4OutYBufSa = prJdecInst->au4ColorBufSa[u4CurrentBufId][0];
    ptOutputInfo->u4OutYBufPitch = prJdecInst->au4ColorBufPitch[0];
    ptOutputInfo->u4OutCbBufSa = prJdecInst->au4ColorBufSa[u4CurrentBufId][1];
    ptOutputInfo->u4OutCbBufPitch = prJdecInst->au4ColorBufPitch[1];
    ptOutputInfo->u4OutCrBufSa = prJdecInst->au4ColorBufSa[u4CurrentBufId][2];
    ptOutputInfo->u4OutCrBufPitch = prJdecInst->au4ColorBufPitch[2];

    if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
        || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif
        )
    {
        ptOutputInfo->u4OutBank0YBufSa = prJdecInst->au4ColorBufSa[0][0];
        ptOutputInfo->u4OutBank0CbBufSa = prJdecInst->au4ColorBufSa[0][1];
        ptOutputInfo->u4OutBank0CrBufSa = prJdecInst->au4ColorBufSa[0][2];
        ptOutputInfo->u4OutBank1YBufSa = prJdecInst->au4ColorBufSa[1][0];
        ptOutputInfo->u4OutBank1CbBufSa = prJdecInst->au4ColorBufSa[1][1];
        ptOutputInfo->u4OutBank1CrBufSa = prJdecInst->au4ColorBufSa[1][2];
    }

    if (prSos->u1CompInScan > MAX_JPEG_COMPONENT)
    {
        ASSERT(0);
    }
    if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_MCU) ||
        (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) ||
        (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE))
    {
        for (i4=0; i4<prSos->u1CompInScan; i4++)
        {
            u4CompIdx = prSos->au1CompIdx[i4];
            if (u4CompIdx > (JDEC_COMP_NUM - 1))
            {
                ASSERT(0);
            }

            x_memset((void*)prJdecInst->au4ColorBufSa[u4CurrentBufId][u4CompIdx], 0x80, prJdecInst->au4ColorBufSz[u4CompIdx]);
            #if JDEC_USE_MAP_UNMAP
            vJdecMapPrintf("[JDEC]Map from 0x%x to 0x%x , point 4 \n",prJdecInst->au4ColorBufSa[u4CurrentBufId][u4CompIdx], 
                         (prJdecInst->au4ColorBufSa[u4CurrentBufId][u4CompIdx] + prJdecInst->au4ColorBufSz[u4CompIdx]));
            BSP_dma_map_vaddr(prJdecInst->au4ColorBufSa[u4CurrentBufId][u4CompIdx], prJdecInst->au4ColorBufSz[u4CompIdx], BIDIRECTIONAL);
            #else
            BSP_FlushDCacheRange((UINT32)prJdecInst->au4ColorBufSa[u4CurrentBufId][u4CompIdx],prJdecInst->au4ColorBufSz[u4CompIdx]);
            #endif
        }
    }
    return JDEC_DRV_RTN_OK;
}

INT32 i4JdecCheckNextScanMarker(JDEC_INST_T *prJdecInst)
{
    JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;
    JDEC_FIFO_INFO_T *prFifoInfo = &(prJdecInst->rFifoInfo);
    INT32 i4Ret = 0;
#if 0//(CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    UINT32 u4ValidData = 0;
#endif

    //init Jfif Instance, fifo info
    prJfifInst->u4FifoStart = prFifoInfo->u4FifoStart;
    prJfifInst->u4FifoEnd = prFifoInfo->u4FifoEnd;
    prJfifInst->fgPrsResult = FALSE;

    //parser header
    {
        #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
        if (JDEC_DRV_RTN_OK != i4JdecGetHalRdPointForScanSearch(prJdecInst))
        {
            prJdecInst->i4JdecErrorType = -E_JPG_ERR_HAL_GET_RD;
            return JDEC_DRV_RTN_FAIL;
        }
        if (prJdecInst->rFifoInfo.u4RdPoint > prJdecInst->u4ScanRd)
        {
            vJdecLog_L("[JDEC] Roll back the read pointer \n");
            prJdecInst->rFifoInfo.u4FileOfst -= (prJdecInst->rFifoInfo.u4RdPoint - prJdecInst->u4ScanRd);
            prJdecInst->rFifoInfo.u4RdPoint = prJdecInst->u4ScanRd;
            prJdecInst->rFifoInfo.u4PreRdPoint = prJdecInst->u4ScanRd;
        }


        vJdecLog_L("[JDEC] Search scan from %x \n",prJdecInst->u4ScanRd);
    if (prFifoInfo->fgEoRange)
    {
            JFIF_New(prJdecInst->hJfifInst, (void *)(prJdecInst->u4ScanRd), prFifoInfo->u4ValidData + 8);
    }
    else
    {
            JFIF_New(prJdecInst->hJfifInst, (void *)(prJdecInst->u4ScanRd), prFifoInfo->u4ValidData);
    }
        #else
        vJdecLog_L("[JDEC] Search scan from %x \n",prFifoInfo->u4RdPoint);
    if (prFifoInfo->fgEoRange)
    {
        JFIF_New(prJdecInst->hJfifInst, (void *)(prFifoInfo->u4RdPoint), prFifoInfo->u4ValidData + 8);
    }
    else
    {
        JFIF_New(prJdecInst->hJfifInst, (void *)(prFifoInfo->u4RdPoint), prFifoInfo->u4ValidData);
    }
        #endif
    }
    prFifoInfo->u4RdPoint = (UINT32)(prJfifInst->pu1CurrRead);

    if (prJfifInst->fgPrsResult)   //parse success
    {
        if (prJfifInst->fgEOI)
        {
            prJdecInst->eJdecState = JDEC_STATE_DEC_FINISH;
        }
        else
        {
            if ((prJfifInst->rSOS.u1Ah != 0) &&
                ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
                 || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
                 || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif
                )
                && (prJdecInst->eQuality == IMG_QUALITY_FACTOR_FAST))
            {
                prJdecInst->eJdecState = JDEC_STATE_DEC_FINISH;
                vJDECPollingFsh(prJdecInst->u4HwId);
            }
            else
            {
                if (i4JdecUpdateImgdmaRdPoint(prJdecInst) == JDEC_DRV_RTN_FAIL)
                {
                    prJdecInst->i4JdecErrorType = -E_JPG_ERR_PASS_END;
                    return JDEC_DRV_RTN_FAIL;
                }
                vJdecLog_L("[JDEC] New Scan, rd ptr = %x \n",prFifoInfo->u4RdPoint);
                i4JdecPerScanSetup(prJdecInst);
                i4JdecScaleSetJpegInfo(prJdecInst);
                i4JDEC_HAL_Set_HuffmanTable_Info(prJdecInst->u4HwInstId, &(prJfifInst->rDHT));
                i4JDEC_HAL_Set_Scan_Info(prJdecInst->u4HwInstId, &(prJfifInst->rSOS));
                i4Ret = i4JDEC_HAL_Set_Read_Pointer(prJdecInst->u4HwInstId, prFifoInfo->u4RdPoint);
                if (i4Ret < 0)
                    return JDEC_DRV_RTN_FAIL;
                //UTIL_Printf("[JDEC_DRV] Scan start , rd point to %x \n",prFifoInfo->u4RdPoint);
            }
        }
        return JDEC_DRV_RTN_OK;
    }
    else
    {
        return JDEC_DRV_RTN_FAIL;
    }
}

void vJdecCaluLastMCURowH(JDEC_INST_T *prJdecInst)
{
    JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;
    JDEC_JFIF_SOS_T *prSos = &(prJfifInst->rSOS);

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    if (prJdecInst->u4CurrDecodeMCURow == (prJdecInst->u4MCURowNum - 1))
    {
        if (prSos->u1CompInScan == 1)
            prJdecInst->u4MCUInMCURowH = 1;
    }

#else

    JDEC_JFIF_SOF_T *prSof = &(prJfifInst->rSOF);
    UINT32 u4CompIdx = prSos->au1CompIdx[0];


    if (u4CompIdx > (MAX_JPEG_COMPONENT - 1))
    {
        ASSERT(0);
    }
    if (prJdecInst->u4CurrDecodeMCURow == (prJdecInst->u4MCURowNum - 1))
    {
        if (prSos->u1CompInScan == 1)
        {
            UINT32 u4CompIdx = prSos->au1CompIdx[0];
            UINT32 u4PicHeight = prSof->u2ImageHeight;
            UINT32 u4PicBlkHeight = (u4PicHeight + 7) / 8;
            UINT32 u4PicLastMCURowBlkNum = u4PicBlkHeight % prSof->arSofComp[u4CompIdx].u1VSampFactor;
            if (u4PicLastMCURowBlkNum == 0)
            {
                prJdecInst->u4MCUInMCURowH = prSof->arSofComp[u4CompIdx].u1VSampFactor;
            }
            else
            {
                prJdecInst->u4MCUInMCURowH = u4PicLastMCURowBlkNum;
            }
        }
    }
#endif

}

void vJdecInitNextMCURow(JDEC_INST_T *prJdecInst)
{
    JDEC_HAL_OUTPUT_INFO_T rOutputInfo;
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    JDEC_HAL_OUTPUT_INFO_T rOutputInfo1;
    #endif

    i4JdecCaluOutputBuf(prJdecInst, &rOutputInfo);
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    i4JDEC_HAL_Set_Output_Info(prJdecInst->u4HwInstId, &rOutputInfo, &rOutputInfo1);
    #else
    i4JDEC_HAL_Set_Output_Info(prJdecInst->u4HwInstId, &rOutputInfo);
    #endif
    prJdecInst->eJdecDecState = JDEC_DECODE_DO_DECODE;
    prJdecInst->u4CurrDecodeMCUW = 0;
    prJdecInst->u4CurrDecodeMCUH = 0;

    vJdecCaluLastMCURowH(prJdecInst);
}

void vJdecReDecode(JDEC_INST_T *prJdecInst)
{
    INT32 i4Ret = 0;
    INT32 i4Count1, i4Count2;

    //reset Jpeg decode HW
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
    i4JDEC_HAL_Uninit(prJdecInst->u4HwInstId,FALSE);
#else
    i4JDEC_HAL_Uninit(prJdecInst->u4HwInstId);
#endif
    for (i4Count1=0; i4Count1<JDEC_CLR_BUF_NUM; i4Count1++)
    {
        for (i4Count2=0; i4Count2<JDEC_COMP_NUM; i4Count2++)
        {
            if (prJdecInst->au4ColorBufSa[i4Count1][i4Count2] != JDEC_INVALID32)
            {
            #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
	         x_free_aligned_dma_mem((void*)(prJdecInst->au4ColorBufSa[i4Count1][i4Count2]));
#elif CONFIG_SYS_MEM_PHASE3
	         x_mem_aligned_free((void*)(prJdecInst->au4ColorBufSa[i4Count1][i4Count2]));
#endif
	     #else
                x_free_aligned_nc_mem((void*)(prJdecInst->au4ColorBufSa[i4Count1][i4Count2]));
	     #endif
                prJdecInst->au4ColorBufSa[i4Count1][i4Count2] = JDEC_INVALID32;
            }
        }
    }

    if (prJdecInst->u2CompId > (JDEC_INST_NUM - 1))
    {
        ASSERT(0);
    }
    if (_aru4FifoStart[prJdecInst->u2CompId] != JDEC_INVALID32)
    {
#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
        x_free_aligned_dma_mem((void*)(_aru4FifoStart[prJdecInst->u2CompId]));
#elif CONFIG_SYS_MEM_PHASE3
        x_mem_aligned_free((void*)(_aru4FifoStart[prJdecInst->u2CompId]));
#endif
#else
        x_free_aligned_nc_mem((void*)(_aru4FifoStart[prJdecInst->u2CompId]));
#endif
        _aru4FifoStart[prJdecInst->u2CompId] = JDEC_INVALID32;
    }
    //reset Img resz drv
    if (prJdecInst->rImgReszTicket.u4Ticket != JDEC_INVALID32)
    {
        if (prJdecInst->eJdecDecMode != JDEC_HAL_DEC_MODE_BASELINE_PIC)
        {
        i4Ret = i4ImgResz_Drv_SetLock(&(prJdecInst->rImgReszTicket), FALSE);
        }
        i4Ret = i4ImgResz_Drv_ReleaseTicket(&(prJdecInst->rImgReszTicket));
        VERIFY(i4Ret == 0);
        prJdecInst->rImgReszTicket.u4Ticket = JDEC_INVALID32;
    }
    if (prJdecInst->rImgReszTicket.u4Ticket == JDEC_INVALID32)
    {
        i4Ret = i4ImgResz_Drv_GetTicket(&(prJdecInst->rImgReszTicket));
        VERIFY(i4Ret == 0);
    }
    //reset img dma
    i4Ret = i4ImgdmaSetReset(prJdecInst->u4ImgdmaId);
    VERIFY(i4Ret == IMGDMA_OK);

    //clean event
    vJdecLog_L("[jdec]Clear  rJdecInst->hJdecEvent =0x%x\n",prJdecInst->hJdecEvent);
    i4Ret = x_ev_group_set_event(prJdecInst->hJdecEvent, 0x00, X_EV_OP_CLEAR);
    VERIFY(i4Ret == OSR_OK);
    vJdecScaleSwitch2SW(prJdecInst, TRUE);

    prJdecInst->eJdecState = JDEC_STATE_START;
}

#if JDEC_SUPPORT_BROKEN_TARGET
void vJdecProcSliceScale(JDEC_INST_T *prJdecInst)
{
    UINT32 u4TempLine = 0;
    JPEG_JFIF_HEADER_T *prJfifInst= (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;
    // copy data for broken line

    //UTIL_Printf("[JDEC] vJdecProcSliceScale \n");
    if (prJdecInst->fgBrokenTarget && prJdecInst->fgBrokenRow)
    {
        HANDLE_T hTemp = NULL_HANDLE;
        INT32 i4Length = prJdecInst->rBlkInfo.u4BlkSize - prJdecInst->rBlkInfo.u4BlkOfst;
        UINT32 u4SA = prJdecInst->rBlkInfo.u4BlkSa + prJdecInst->rBlkInfo.u4BlkOfst;


        u4TempLine = prJdecInst->u4DstBufPitch;
        if ((prJfifInst->rSOS.u1CompInScan == 1) &&
            (prJfifInst->rSOF.arSofComp[0].u1VSampFactor == 2) &&
            (prJfifInst->rSOF.arSofComp[prJfifInst->rSOS.au1CompIdx[0]].u1VSampFactor != 2))
        {
            u4TempLine = prJdecInst->u4DstBufPitch * 2;
        }


        if ((prJdecInst->rBlkInfo.u4BlkIdx == 0) && prJdecInst->fgNeedJumpBack)
        {
            if (i4Length < 0)
            {
                i4Length = prJdecInst->rBlkInfo.u4BlkSize - (prJdecInst->rBlkInfo.u4BlkOfst - /*prJdecInst->u4DstBufPitch*/u4TempLine);
                if (i4Length < 0)
                {
                    ASSERT(0);
                }
            }
            i4Length = i4Length + /*prJdecInst->u4DstBufPitch*/u4TempLine; /*the leave space in block 0*/
            /*For block 0, if broken row, for previous mcu row, imgresz only output (mcu row height - 1) lines ,so
              we need to copy the broken row to the (blkofst - prJdecInst->u4DstBufPitch) to avoid one line miss */
            u4SA = u4SA - /*prJdecInst->u4DstBufPitch*/ u4TempLine;
        }

        x_memcpy((void *)u4SA, (void *)prJdecInst->u4TempAYUVBuf, i4Length);
        #if !JDEC_USE_MAP_UNMAP
        BSP_FlushDCacheRange(u4SA,i4Length);
        #endif
        prJdecInst->rBlkInfo.u4BlkIdx ++;
        hTemp = prJdecInst->rBlkInfo.hMem;//x_kmem_block_handle(prJdecInst->pvDstYBuf);
        prJdecInst->rBlkInfo.u4BlkSa = (UINT32)x_kmem_kernel_address(hTemp,prJdecInst->rBlkInfo.u4BlkIdx * prJdecInst->rBlkInfo.u4BlkSize);
        x_memcpy((void *)prJdecInst->rBlkInfo.u4BlkSa,
                 (void *)(prJdecInst->u4TempAYUVBuf + i4Length),
                 (prJdecInst->u4McuRowLength - i4Length));
        #if !JDEC_USE_MAP_UNMAP
        BSP_FlushDCacheRange(prJdecInst->rBlkInfo.u4BlkSa, (prJdecInst->u4McuRowLength - i4Length));
        #endif
        prJdecInst->rBlkInfo.u4BlkOfst = /*prJdecInst->rBlkInfo.u4BlkSa +*/ prJdecInst->u4McuRowLength - i4Length;
        vJdecLog_L("[JDEC] BlkOfst1 = %x \n",prJdecInst->rBlkInfo.u4BlkOfst);
    }
    else if (prJdecInst->fgBrokenTarget)
    {
        prJdecInst->rBlkInfo.u4BlkOfst += prJdecInst->u4McuRowLength;
        vJdecLog_L("[JDEC] BlkOfst2 = %x \n",prJdecInst->rBlkInfo.u4BlkOfst);
        if (prJdecInst->rBlkInfo.u4BlkOfst > prJdecInst->rBlkInfo.u4BlkSize)
        {

#if 0
            if (prJdecInst->rBlkInfo.u4BlkIdx == 0)
            {
                prJdecInst->rBlkInfo.u4BlkOfst -= prJdecInst->u4DstBufPitch;
                if (prJdecInst->rBlkInfo.u4BlkOfst > prJdecInst->rBlkInfo.u4BlkSize)
                {
                    ASSERT(0);
                }
            }
            else
#endif
            if (prJdecInst->rBlkInfo.u4BlkIdx != 0)
            {
                vJdecLog_L("[JDEC] Invalid block offset block %d block offset = %x block size = %x \n",
                            prJdecInst->rBlkInfo.u4BlkIdx, prJdecInst->rBlkInfo.u4BlkOfst,
                            prJdecInst->rBlkInfo.u4BlkSize);
                ASSERT(0);
            }
            else
            {
                vJdecLog_L("[JDEC] Block0 offset so large \n");
            }
        }
    }
    // One component scan
#if 0
    if (prJfifInst->rSOS.u1CompInScan == 1)
    {
        // For YUV420, YUV422 (V factor not eual)
        if ((prJfifInst->rSOF.arSofComp[0].u1VSampFactor != prJfifInst->rSOF.arSofComp[1].u1VSampFactor) &&
            (prJfifInst->rSOF.arSofComp[0].u1VSampFactor != prJfifInst->rSOF.arSofComp[2].u1VSampFactor))
        {
            if (prJfifInst->rSOS.au1CompIdx[0]!= 0/*prJfifInst->rSOF.arSofComp[0].u1ComponentId*/) //Cb/Cr
            {
                // Need to jump one more mcu row
                if (prJdecInst->rBlkInfo.u4BlkOfst + prJdecInst->u4McuRowLength >= prJdecInst->rBlkInfo.u4BlkSize)
                {
                    UINT32 u4Length = 0;

                    u4Length = prJdecInst->rBlkInfo.u4BlkSize - prJdecInst->rBlkInfo.u4BlkOfst;
                    prJdecInst->rBlkInfo.u4BlkIdx ++;
                    prJdecInst->rBlkInfo.u4BlkSa = (UINT32)x_kmem_kernel_address(prJdecInst->rBlkInfo.hMem,prJdecInst->rBlkInfo.u4BlkIdx * prJdecInst->rBlkInfo.u4BlkSize);
                    prJdecInst->rBlkInfo.u4BlkOfst = prJdecInst->u4McuRowLength - u4Length;

                }
                else
                {
                    prJdecInst->rBlkInfo.u4BlkOfst += prJdecInst->u4McuRowLength;
                }
                UTIL_Printf("[JDEC] One component update blkofst to %x \n",prJdecInst->rBlkInfo.u4BlkOfst);
            }
        }

}
#endif
}

#endif

void vJdecBaselineWaitScale(JDEC_INST_T *prJdecInst)
{
    INT32 i4Ret = 0;
    i4Ret = i4JdecScaleWaitFinish(prJdecInst);
    if (i4Ret != JDEC_DRV_RTN_OK)
    {
        return;
    }
    //wait scale finish
    if (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)
    {
        prJdecInst->eJdecState = JDEC_STATE_DEC_FINISH;
    }
    else if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW) ||
             (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_MCU))
    {
        if (prJdecInst->u4CurrDecodeMCURow < prJdecInst->u4MCURowNum)
        {
#if JDEC_SUPPORT_BROKEN_TARGET
            if (prJdecInst->fgBrokenTarget)
            {
                vJdecProcSliceScale(prJdecInst);
            }
#endif

#if !JDEC_MCU_ROW_ENHANCE
            vJdecInitNextMCURow(prJdecInst);
#endif
        }
        else if (prJdecInst->u4CurrDecodeMCURow == prJdecInst->u4MCURowNum)
        {
            prJdecInst->eJdecState = JDEC_STATE_DEC_FINISH;
        }
        else
        {
            ASSERT(0);
        }
    }
    else
    {
        ASSERT(0);
    }
}

void vJdecBaseLineProcDecResult(JDEC_INST_T *prJdecInst)
{
    //INT32 i4Ret = 0;
    #if JDEC_USE_MAP_UNMAP
    UINT32 u4i = 0;
    #endif
    

    prJdecInst->u4TimeOutCount = 0;
    if (prJdecInst->i4JdecDecTimerStatus == 1)
    {
        x_timer_stop(prJdecInst->hJdecDecTimer);
        prJdecInst->i4JdecDecTimerStatus = 0;
    }
    if (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_MCU)
    {
        prJdecInst->u4CurrDecodeMCUW++;
        if (prJdecInst->u4CurrDecodeMCUW < prJdecInst->u4MCUInMCURowW)
        {
            prJdecInst->eJdecDecState = JDEC_DECODE_DO_DECODE;
        }
        else if (prJdecInst->u4CurrDecodeMCUW == prJdecInst->u4MCUInMCURowW)
        {
            prJdecInst->u4CurrDecodeMCUW = 0;
            prJdecInst->u4CurrDecodeMCUH++;
            if (prJdecInst->u4CurrDecodeMCUH == prJdecInst->u4MCUInMCURowH)
            {
                prJdecInst->u4CurrDecodeMCUH = 0;
                prJdecInst->eJdecDecState = JDEC_DECODE_DO_SCALE;
                #if JDEC_USE_MAP_UNMAP
                for (u4i = 0; u4i < JDEC_COMP_NUM; u4i ++)
                {
                    vJdecMapPrintf("[JDEC] Unmap from 0x%x to 0x%x point 3\n",prJdecInst->au4ColorBufSa[prJdecInst->u4CurrDecodeMCURow %2][u4i], 
                                (prJdecInst->au4ColorBufSa[prJdecInst->u4CurrDecodeMCURow %2][u4i] + prJdecInst->au4ColorBufSz[0]));
                    BSP_dma_unmap_vaddr(prJdecInst->au4ColorBufSa[prJdecInst->u4CurrDecodeMCURow %2][u4i], prJdecInst->au4ColorBufSz[0],BIDIRECTIONAL);
                }
                #endif
            }
            else
            {
                ASSERT(0);
            }
        }
        else
        {
            ASSERT(0);
        }
    }
    else if (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW)
    {
        prJdecInst->eJdecDecState = JDEC_DECODE_DO_SCALE;
#if JDEC_MCU_ROW_ENHANCE
        if (prJdecInst->u4CurrDecodeMCURow > 0)
        {
            vJdecBaselineWaitScale(prJdecInst);
        }
#endif
    }
    else if (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)
    {
        vJdecBaselineWaitScale(prJdecInst);
    }
    else
    {
        ASSERT(0);
    }
}

#if JDEC_SUPPORT_BROKEN_TARGET

void vJdecFillCBuf(JDEC_INST_T *prJdecInst)
{
    JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;
    INT32 i4Temp = 0;
    UINT32 u4CurrentBufId = (prJdecInst->u4CurrDecodeMCURow % JDEC_CLR_BUF_NUM);

    //UTIL_Printf("[JDEC] vJdecFillCBuf \n");
    if (prJfifInst->rSOF.arSofComp[0].u1VSampFactor == 2)
    {
        if (prJfifInst->rSOF.arSofComp[1].u1VSampFactor == 1)
        {
            for (i4Temp = 7; i4Temp >= 0; i4Temp --)
            {
                //UTIL_Printf("[JDEC] Copy %x byte from 0x%x to 0x%x \n",
                //            prJdecInst->au4ColorBufPitch[1],
                //            (prJdecInst->au4ColorBufSa[u4CurrentBufId][1] + prJdecInst->au4ColorBufPitch[1] * u4Temp),
                //            (prJdecInst->au4ColorBufSa[u4CurrentBufId][1]+ prJdecInst->au4ColorBufPitch[1]* (u4Temp + 8)));
                x_memcpy((void *)(prJdecInst->au4ColorBufSa[u4CurrentBufId][1]+ prJdecInst->au4ColorBufPitch[1]* (i4Temp*2)),
                         (void *)(prJdecInst->au4ColorBufSa[u4CurrentBufId][1] + prJdecInst->au4ColorBufPitch[1] * i4Temp),
                         prJdecInst->au4ColorBufPitch[1]);
                x_memcpy((void *)(prJdecInst->au4ColorBufSa[u4CurrentBufId][1] + prJdecInst->au4ColorBufPitch[1]* (i4Temp *2 + 1)),
                         (void *)(prJdecInst->au4ColorBufSa[u4CurrentBufId][1] + prJdecInst->au4ColorBufPitch[1] * i4Temp),
                         prJdecInst->au4ColorBufPitch[1]);
            }
        }
        #if !JDEC_USE_MAP_UNMAP
        BSP_FlushDCacheRange(prJdecInst->au4ColorBufSa[u4CurrentBufId][1],prJdecInst->au4ColorBufSz[1]);
        #endif
        //UTIL_Printf("[JDEC] Cb copy done \n");

        if (prJfifInst->rSOF.arSofComp[2].u1VSampFactor == 1)
        {
            for (i4Temp = 7; i4Temp >= 0; i4Temp --)
            {
                //UTIL_Printf("[JDEC] Copy %x byte from 0x%x to 0x%x \n",
                //            prJdecInst->au4ColorBufPitch[2],
                //            (prJdecInst->au4ColorBufSa[u4CurrentBufId][2] + prJdecInst->au4ColorBufPitch[2] * u4Temp),
                //            (prJdecInst->au4ColorBufSa[u4CurrentBufId][2]+ prJdecInst->au4ColorBufPitch[2]* (u4Temp + 8)));
                x_memcpy((void *)(prJdecInst->au4ColorBufSa[u4CurrentBufId][2]+ prJdecInst->au4ColorBufPitch[2]* (i4Temp * 2)),
                         (void *)(prJdecInst->au4ColorBufSa[u4CurrentBufId][2] + prJdecInst->au4ColorBufPitch[2] * i4Temp),
                         prJdecInst->au4ColorBufPitch[2]);
                x_memcpy((void *)(prJdecInst->au4ColorBufSa[u4CurrentBufId][2] + prJdecInst->au4ColorBufPitch[2]* (i4Temp*2 + 1)),
                         (void *)(prJdecInst->au4ColorBufSa[u4CurrentBufId][2] + prJdecInst->au4ColorBufPitch[2] * i4Temp),
                         prJdecInst->au4ColorBufPitch[2]);
            }
            #if !JDEC_USE_MAP_UNMAP
            BSP_FlushDCacheRange(prJdecInst->au4ColorBufSa[u4CurrentBufId][2],prJdecInst->au4ColorBufSz[2]);
            #endif
        }
        //UTIL_Printf("[JDEC] Cr copy done \n");
    }
}

#endif // if JDEC_SUPPORT_BROKEN_TARGET

extern void vHwJdecPrintRegister(UINT32 u4HwId);

void vJdecProcBaselineDec(JDEC_INST_T *prJdecInst)
{
    INT32 i4Ret = 0;
    //UINT32 u4JdecDecTimeOut = 0;

    switch (prJdecInst->eJdecDecState)
    {
        case JDEC_DECODE_DO_DECODE:
            if (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)
            {
                EV_GRP_EVENT_T  eJdecEvent;
                EV_GRP_EVENT_T  eJdecEventGet;


                i4JdecScaleSetNextPartialBuf(prJdecInst);
                i4JdecScaleFire(prJdecInst);
                eJdecEvent = JDEC_DRV_EV_SCALE_READY;
                i4Ret = x_ev_group_wait_event_timeout(prJdecInst->hJdecEvent, eJdecEvent, &eJdecEventGet, X_EV_OP_OR_CONSUME, 1000);
                if (i4Ret != OSR_OK) // time out
                {
                    prJdecInst->i4JdecErrorType = -E_JPG_ERR_SCALE_ERROR;
                    //i4JdecScaleStop(prJdecInst);
                    return;
                }
            }
            //If the file bitstream parse finish, should move the write pointer again
            if (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)
            {
                #if 0//JDEC_USE_MAP_UNMAP
                vJdecMapPrintf("[JDEC]Map from 0x%x to 0x%x , point 5 \n",(UINT32)prJdecInst->pvDstYBuf, 
                             ((UINT32)prJdecInst->pvDstYBuf + prJdecInst->u4DstBufPitch * (prJdecInst->u4DstCropPosY + prJdecInst->u4DstCropHeight)));
                BSP_dma_map_vaddr((UINT32)prJdecInst->pvDstYBuf, prJdecInst->u4DstBufPitch * (prJdecInst->u4DstCropPosY + prJdecInst->u4DstCropHeight), FROM_DEVICE);
                
                #endif
                if (prJdecInst->rFifoInfo.fgEoRange)
                {               
                    i4JDEC_HAL_Set_Write_Pointer(prJdecInst->u4HwInstId,prJdecInst->rFifoInfo.u4WrPoint + 256);

                }
            }

            // Update write pointer again
            {
                JDEC_HAL_INPUT_INFO_T rInputInfo;

                rInputInfo.u4RdPoint = prJdecInst->rFifoInfo.u4RdPoint;
                if (prJdecInst->rFifoInfo.fgEoRange)
                {
                    rInputInfo.u4WrPoint = prJdecInst->rFifoInfo.u4WrPoint + 256;
                }
                else
                {
                    rInputInfo.u4WrPoint = prJdecInst->rFifoInfo.u4WrPoint;
                }
                rInputInfo.u4InFifoSa = prJdecInst->rFifoInfo.u4FifoStart;      //need 64bytes alignment
                rInputInfo.u4InFifoSz = prJdecInst->rFifoInfo.u4FifoEnd - prJdecInst->rFifoInfo.u4FifoStart;
                rInputInfo.u4InFifoSz = (rInputInfo.u4InFifoSz + 63) / 64 * 64; //need 64bytes alignment
                // Add by pingzhao for adjust the fifo end address 2008/7/17
                prJdecInst->rFifoInfo.u4FifoEnd = prJdecInst->rFifoInfo.u4FifoStart + rInputInfo.u4InFifoSz;

                i4JDEC_HAL_Set_Input_Info(prJdecInst->u4HwInstId, &rInputInfo);
            }
            #if JDEC_USE_MAP_UNMAP
            // for hw read
            if (prJdecInst->u4CurrDecodeMCURow == 0) // first decode
            {
                UINT32 u4Start, u4End;

                u4Start = prJdecInst->rFifoInfo.u4RdPoint / JDEC_CACHE_LINE_ALIGN * JDEC_CACHE_LINE_ALIGN;
                u4End = prJdecInst->rFifoInfo.u4WrPoint / JDEC_CACHE_LINE_ALIGN * JDEC_CACHE_LINE_ALIGN;
                
                if (prJdecInst->rFifoInfo.u4WrPoint > prJdecInst->rFifoInfo.u4RdPoint)
                {   
                    vJdecMapPrintf("[JDEC]Map from 0x%x to 0x%x , point 6 \n",u4Start, u4End);
                    BSP_dma_map_vaddr(u4Start, (u4End - u4Start), TO_DEVICE);   
                }
                else
                { 
                    vJdecMapPrintf("[JDEC]Map from 0x%x to 0x%x , point 7 \n",u4Start, prJdecInst->rFifoInfo.u4FifoEnd);
                    vJdecMapPrintf("[JDEC]Map from 0x%x to 0x%x , point 7 \n",prJdecInst->rFifoInfo.u4FifoStart, u4End);
                    BSP_dma_map_vaddr(u4Start, (prJdecInst->rFifoInfo.u4FifoEnd - u4Start),TO_DEVICE);
                    BSP_dma_map_vaddr(prJdecInst->rFifoInfo.u4FifoStart,(u4End - prJdecInst->rFifoInfo.u4FifoStart),TO_DEVICE);
                }
            }
            else // for mcu/mcu row mode, not first decode
            {
                UINT32 u4Start, u4End;

                u4Start = prJdecInst->u4MapSA / JDEC_CACHE_LINE_ALIGN * JDEC_CACHE_LINE_ALIGN;
                u4End = prJdecInst->rFifoInfo.u4WrPoint / JDEC_CACHE_LINE_ALIGN * JDEC_CACHE_LINE_ALIGN;
                
                if (prJdecInst->rFifoInfo.u4WrPoint > prJdecInst->u4MapSA)
                {
                    vJdecMapPrintf("[JDEC]Map from 0x%x to 0x%x , point 8 \n",u4Start, u4End);
                    BSP_dma_map_vaddr(u4Start, (u4End - u4Start), TO_DEVICE);
                }
                else if (prJdecInst->rFifoInfo.u4WrPoint < prJdecInst->u4MapSA)
                {
                    vJdecMapPrintf("[JDEC]Map from 0x%x to 0x%x , point 9 \n",u4Start, prJdecInst->rFifoInfo.u4FifoEnd);
                    vJdecMapPrintf("[JDEC]Map from 0x%x to 0x%x , point 9 \n",prJdecInst->rFifoInfo.u4FifoStart, u4End);
                    BSP_dma_map_vaddr(u4Start, (prJdecInst->rFifoInfo.u4FifoEnd - u4Start),TO_DEVICE);
                    BSP_dma_map_vaddr(prJdecInst->rFifoInfo.u4FifoStart,(u4End - prJdecInst->rFifoInfo.u4FifoStart),TO_DEVICE);
                }
            }
            prJdecInst->u4MapSA = prJdecInst->rFifoInfo.u4WrPoint;
            #endif
            if (i4JDEC_HAL_Decode(prJdecInst->u4HwInstId) < 0)
            {
                //i4JdecScaleStop(prJdecInst);
                prJdecInst->i4JdecErrorType = -E_JPG_ERR_DEC_ERROR;
				 UTIL_Printf("[JDEC] %s line %dE_JPG_ERR_DEC_ERROR\n", __FUNCTION__, __LINE__);
                break;
            }

            prJdecInst->eJdecDecState = JDEC_DECODE_WAIT_DECODE;
            break;
        case JDEC_DECODE_WAIT_DECODE:
            if (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_MCU)
            {
            i4Ret = i4JDEC_HAL_Get_Decode_Status(prJdecInst->u4HwInstId);
            }
            else if (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW)
            {
                UINT32 u4Temp = 0;
                EV_GRP_EVENT_T  eJdecEventGet = 0;

                while(1)
                {
                    #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
                    i4Ret = i4JDEC_HAL_Get_Decode_Status(prJdecInst->u4HwInstId);
                    if (i4Ret != S_JDEC_HAL_OK)
                    #else
                    i4Ret = x_ev_group_wait_event_timeout(prJdecInst->hJdecEvent,(JDEC_DRV_EV_DEC_DONE | JDEC_DRV_EV_DEC_ERR),&eJdecEventGet,X_EV_OP_OR_CONSUME, 20);
                    if (i4Ret != OSR_OK)
                    #endif
                    {
                        UTIL_Printf("[JDEC] Decode mcu row time out \n");
                        x_thread_delay(5);
                        u4Temp ++;
                        if (u4Temp > 400) // decode time out
                        {
                            //i4JdecScaleStop(prJdecInst);
                            prJdecInst->i4JdecErrorType = -E_JPG_ERR_DEC_ERROR;
							UTIL_Printf("[JDEC] %s line %dE_JPG_ERR_DEC_ERROR\n", __FUNCTION__, __LINE__);
                            return;
                        }
                    }
                    #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
                    else
                    {
                        break;
                    }
                    #endif

                    #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)
                    if (eJdecEventGet & JDEC_DRV_EV_DEC_ERR)
                    {
                        prJdecInst->i4JdecErrorType = E_JPG_ERR_DEC_ERROR;
						 UTIL_Printf("[JDEC] %s line %dE_JPG_ERR_DEC_ERROR\n", __FUNCTION__, __LINE__);
                        return;
                    }
                    else if (eJdecEventGet & JDEC_DRV_EV_DEC_DONE)
                    {
                        vJDECPollingFsh(prJdecInst->u4HwId);
                        break;
                    }
                    #endif
                    i4Ret = x_ev_group_wait_event_timeout(prJdecInst->hJdecEvent, JDEC_DRV_EV_STOP, &eJdecEventGet, X_EV_OP_OR_CONSUME, 0);
                    if (i4Ret == OSR_OK)
                    {
                        vJdecSetLog(JDEC_LOG_DEC_STOP);
                        vJdecAbort(prJdecInst, (IMG_NFY_STATE_T)IMG_NFY_STOP_DONE, FALSE);
                        return;
                    }
                }
            }
            else if (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)
            {
                UINT32 u4RdPointer = 0;
                UINT32 u4WrPointer = 0;
                //UINT32 u4Count = 0; // how many times the vld read pointer not change
                EV_GRP_EVENT_T  eJdecEvent;
                EV_GRP_EVENT_T  eJdecEventGet;


                u4RdPointer = prJdecInst->rFifoInfo.u4RdPoint;
                u4WrPointer = prJdecInst->rFifoInfo.u4WrPoint;
                while (1)
                {
                    if (prJdecInst->i4JdecErrorType < 0)
                    {
                        //i4JdecScaleStop(prJdecInst);
                        return;
                    }
                    #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)
                    if (prJdecInst->fgWaitStop)
                    {
                        if (i4JDEC_HAL_GET_STOP_STATUS(prJdecInst->u4HwInstId) == S_JDEC_HAL_OK)
                        {
                            prJdecInst->fgWaitStop = FALSE;
                            //JdecScaleStop(prJdecInst);
                            vJdecAbort(prJdecInst, (IMG_NFY_STATE_T)IMG_NFY_STOP_DONE, FALSE);
                            return;
                        }
                    }
                    #endif

                    #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
                    eJdecEvent = JDEC_DRV_EV_STOP|JDEC_DRV_EV_DEC_TIME_OUT;
                    #else
                    eJdecEvent = JDEC_DRV_EV_STOP|JDEC_DRV_EV_DEC_TIME_OUT|JDEC_DRV_EV_DEC_DONE|JDEC_DRV_EV_DEC_ERR;
                    #endif
                    #if SUPPORT_VDEC_PATH
                    if (prJdecInst->fgVdecPath)
                    {
                        i4Ret = x_ev_group_wait_event_timeout(prJdecInst->hJdecEvent, eJdecEvent, &eJdecEventGet, X_EV_OP_OR_CONSUME, 50);
                    }
                    else
                    #endif
                    {
                        i4Ret = x_ev_group_wait_event_timeout(prJdecInst->hJdecEvent, eJdecEvent, &eJdecEventGet, X_EV_OP_OR_CONSUME, 10);
                    }
                    if (i4Ret == OSR_OK)
                    {
                        if (eJdecEventGet & JDEC_DRV_EV_STOP)
                        {
                            #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)
                            //vJDEC_HAL_StopPicModeDec(prJdecInst->u4HwInstId);
                            //prJdecInst->fgWaitStop = TRUE;
                            vJdecAbort(prJdecInst, (IMG_NFY_STATE_T)IMG_NFY_STOP_DONE, FALSE);
                            i4JdecScaleStop(prJdecInst);
                            return;
                            #else
                            vJdecAbort(prJdecInst, (IMG_NFY_STATE_T)IMG_NFY_STOP_DONE, FALSE);
                            i4JdecScaleStop(prJdecInst);
                            return;
                            #endif

                        }
                        else if ((eJdecEventGet & JDEC_DRV_EV_DEC_TIME_OUT)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)
                            ||   (eJdecEventGet & JDEC_DRV_EV_DEC_ERR)
#endif
                                )
                        {
                            #if 0 // for debug usage
                            if (eJdecEventGet & JDEC_DRV_EV_DEC_TIME_OUT)
                            {
                                vHwJdecPrintRegister(0);
                            }
                            #endif
                            //i4JdecScaleStop(prJdecInst);
                            prJdecInst->i4JdecErrorType = -E_JPG_ERR_DEC_ERROR;
							UTIL_Printf("[JDEC] %s line %dE_JPG_ERR_DEC_ERROR\n", __FUNCTION__, __LINE__);
                            #if 0 // for debug usage
                            if (eJdecEventGet & JDEC_DRV_EV_DEC_TIME_OUT)
                            {
                                ASSERT(0);
                            }
                            #endif
                            // wether need to stop picture mode ?
                            return;
                        }
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)
                        else
                        {
                            x_timer_stop(prJdecInst->hJdecDecTimer);
                            prJdecInst->i4JdecDecTimerStatus = 0;
                            break;
                        }
#endif
                    }
#if SUPPORT_VDEC_PATH
                    else if (prJdecInst->fgVdecPath)
                    {
                        // dec time out
                        if (i4JDEC_HAL_Get_Decode_Status(prJdecInst->u4HwId) != S_JDEC_HAL_OK)
                        {
                            // decode time out
                            //vHwJdecPrintRegister(0);
                            //ASSERT(0);
                            //i4JdecScaleStop(prJdecInst);
                            prJdecInst->i4JdecErrorType = -E_JPG_ERR_DEC_ERROR;
							 UTIL_Printf("[JDEC] %s line %dE_JPG_ERR_DEC_ERROR\n", __FUNCTION__, __LINE__);
                            // wether need to stop picture mode ?
                            return;
                        }
                        else
                        {
                            // decode done
                            x_timer_stop(prJdecInst->hJdecDecTimer);
                            prJdecInst->i4JdecDecTimerStatus = 0;
                            break;
                        }
                    }
#endif
                    // For some case, when jpg is not stop ready, image library
                    // send frame end ,then start to decode another jpg, the
                    // fgWaitStop flag will abort the next jpg decoding
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)
                    if (!prJdecInst->fgWaitStop)
#endif
                    {
                        x_thread_delay(5);
                    }

                    // if parsing data finish ,should update read pointer manually
                    if (prJdecInst->rFifoInfo.fgEoRange)
                    {
                        BOOL fgValue = FALSE;

                        vJdecUpdateWritePointer((void *)(&fgValue), (void *)prJdecInst->u4HwInstId);

                        #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
                        i4Ret = i4JDEC_HAL_Get_Decode_Status(prJdecInst->u4HwInstId);
                        if (i4Ret == S_JDEC_HAL_OK)
                        {
                            // decoding finish
                            x_timer_stop(prJdecInst->hJdecDecTimer);
                            prJdecInst->i4JdecDecTimerStatus = 0;
                            break;
                        }
                        #endif
                    }



                    if (prJdecInst->rFifoInfo.u4RdPoint == u4RdPointer) // read pointer stop
                    {
                        if (prJdecInst->i4JdecDecTimerStatus == 0)
                        {
                            i4Ret = x_timer_stop(prJdecInst->hJdecDecTimer);
                            i4Ret = x_timer_start(prJdecInst->hJdecDecTimer, 5000, X_TIMER_FLAG_ONCE, (x_os_timer_cb_fct)vJdecDecTimerCb, (void*)((UINT32)prJdecInst->u2CompId));
                            VERIFY(OSR_OK == i4Ret);
                            prJdecInst->i4JdecDecTimerStatus = 1;
                        }

                        if (!(prJdecInst->rFifoInfo.fgEoRange))
                        {
                            if (prJdecInst->rFifoInfo.u4WrPoint == u4WrPointer)
                            {
                                if (!fgImgdmaIsTransing(prJdecInst->u4ImgdmaId))
                                {
									BOOL fgValue = TRUE;
                                    vJdecUpdateWritePointer((void *)(&fgValue), (void *)prJdecInst->u4HwInstId);
                                }
                            }

                            u4WrPointer = prJdecInst->rFifoInfo.u4WrPoint;
                        }
                    }
                    else
                    {
                        u4RdPointer = prJdecInst->rFifoInfo.u4RdPoint;
                        u4WrPointer = prJdecInst->rFifoInfo.u4WrPoint;
                        //u4Count = 0;
                        x_timer_stop(prJdecInst->hJdecDecTimer);
                        prJdecInst->i4JdecDecTimerStatus = 0;
                    }
                }
                vJdecBaselineWaitScale(prJdecInst);
                // should add time out
                return;
            }

            if (i4Ret == S_JDEC_HAL_OK)
            {
                if (prJdecInst->u4TimeOutCount > _u4JdecDecFailCount)
                {
                    _u4JdecDecFailCount = prJdecInst->u4TimeOutCount;
                }
                vJdecBaseLineProcDecResult(prJdecInst);
                    }
            else if (i4Ret == E_JDEC_HAL_FAIL)
            {
                prJdecInst->u4TimeOutCount++;
                if ((prJdecInst->u4TimeOutCount > 4000) || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW))
                {
                    prJdecInst->i4JdecErrorType = -E_JPG_ERR_DEC_ERROR;
					 UTIL_Printf("[JDEC] %s line %dE_JPG_ERR_DEC_ERROR\n", __FUNCTION__, __LINE__);
                }
                break;
            }
            else if (i4Ret == S_JDEC_HAL_MARKER)
            {
                if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_MCU) ||
                    (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW))
                {
                    if ((prJdecInst->u4CurrDecodeMCURow < (prJdecInst->u4MCURowNum - 1))/* ||
                        (prJdecInst->u4CurrDecodeMCUW < (prJdecInst->u4MCUInMCURowW - 1))*/)
                    {
                        prJdecInst->i4JdecErrorType = -E_JPG_ERR_MARKER_EARLY;
                        return;
                    }
                }
                vJdecBaseLineProcDecResult(prJdecInst);
            }
            else
            {
                ASSERT(0);
            }
            break;
        case JDEC_DECODE_DO_SCALE:
            //get read point from HAL
            if (i4JdecGetHalRdPoint(prJdecInst) == JDEC_DRV_RTN_FAIL)
            {
                prJdecInst->i4JdecErrorType = -E_JPG_ERR_HAL_GET_RD;
                return;
            }
            #if JDEC_USE_MAP_UNMAP
            if(prJdecInst->rFifoInfo.u4RdPoint > prJdecInst->rFifoInfo.u4PreRdPoint)
            {
                UINT32 u4Start, u4End;

                u4Start = prJdecInst->rFifoInfo.u4PreRdPoint / JDEC_CACHE_LINE_ALIGN * JDEC_CACHE_LINE_ALIGN;
                u4End = prJdecInst->rFifoInfo.u4RdPoint / JDEC_CACHE_LINE_ALIGN * JDEC_CACHE_LINE_ALIGN;
                vJdecMapPrintf("[JDEC] Unmap from 0x%x to 0x%x point 4\n",u4Start, u4End);
                BSP_dma_unmap_vaddr(u4Start, (u4End - u4Start),TO_DEVICE);
            }
            else if (prJdecInst->rFifoInfo.u4RdPoint < prJdecInst->rFifoInfo.u4PreRdPoint)
            {
                UINT32 u4Start, u4End;

                u4Start = prJdecInst->rFifoInfo.u4PreRdPoint / JDEC_CACHE_LINE_ALIGN * JDEC_CACHE_LINE_ALIGN;
                u4End = prJdecInst->rFifoInfo.u4RdPoint / JDEC_CACHE_LINE_ALIGN * JDEC_CACHE_LINE_ALIGN;

                vJdecMapPrintf("[JDEC] Unmap from 0x%x to 0x%x point 5\n",u4Start, prJdecInst->rFifoInfo.u4FifoEnd);
                vJdecMapPrintf("[JDEC] Unmap from 0x%x to 0x%x point 5\n",prJdecInst->rFifoInfo.u4FifoStart, u4End);
                BSP_dma_unmap_vaddr(u4Start, (prJdecInst->rFifoInfo.u4FifoEnd - u4Start), TO_DEVICE);
                BSP_dma_unmap_vaddr(prJdecInst->rFifoInfo.u4FifoStart,(u4End- prJdecInst->rFifoInfo.u4FifoStart),TO_DEVICE);   
            }
            #endif
            //update current read point
            if (i4JdecUpdateImgdmaRdPoint(prJdecInst) == JDEC_DRV_RTN_FAIL)
            {
                prJdecInst->i4JdecErrorType = -E_JPG_ERR_HAL_UPT_RD;
                return;
            }
#if JDEC_SUPPORT_BROKEN_TARGET
            if (prJdecInst->fgBrokenTarget)
            {

                vJdecFillCBuf(prJdecInst);
                i4JdecSetSliceDstInfo(prJdecInst);
            }
#endif
            //Trigger Image resz
            i4JdecScaleSetNextPartialBuf(prJdecInst);
            i4JdecScaleFire(prJdecInst);
            prJdecInst->u4CurrDecodeMCURow++;
#if !JDEC_MCU_ROW_ENHANCE
            vJdecBaselineWaitScale(prJdecInst);
            //cjyang[20080304], work around img resz drv, time out problem, need to mark after full ECO version
            //if (prJdecInst->eDstColorMode == GFX_COLORMDOE_YUV_420_BLK)
            {
                JDEC_PIC_INFO_T *prJdecPicInfo = (JDEC_PIC_INFO_T *)prJdecInst->pvPicCache;
                if ((prJdecPicInfo->u4MaxVFactor == 1) && (prJdecPicInfo->u4MaxHFactor == 1))
                {
                    x_thread_delay(1);
                }
            }
#else
            if (prJdecInst->u4CurrDecodeMCURow == prJdecInst->u4MCURowNum)
            {
                vJdecBaselineWaitScale(prJdecInst);
            }
            else
            {
                vJdecInitNextMCURow(prJdecInst);
            }
#endif
            //--
            break;
        case JDEC_DECODE_RE_DECODE:
            vJdecReDecode(prJdecInst);
            break;
        default:
            ASSERT(0);
            break;
    }
}


void vJdecProgressWaitScale(JDEC_INST_T *prJdecInst)
{
    INT32 i4Ret = 0;

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;
#endif

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE) ||
        (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE) ||
        (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE) ||
        ((prJdecInst->fgLastScan) && ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) ||
                                       (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT) ||
                                       (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT))))
#endif
    {
        //UTIL_Printf("[JDEC_DRV] Wait Scale finish \n");
        i4Ret = i4JdecScaleWaitFinish(prJdecInst);
        if (i4Ret != JDEC_DRV_RTN_OK)
        {
            return;
        }
    }
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE) ||
        (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT))
    {
        if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT) &&
            (!prJdecInst->fgLastScan))
        {
            prJdecInst->eJdecDecState = JDEC_DECODE_CHECK_NEXT_SCAN;
            return;
        }

        if ((prJfifInst->rSOS.u1Ss == 0) &&
            (prJfifInst->rSOS.u1Ah == 0))
        {
            prJdecInst->u4FirstScanNum = prJdecInst->u4FirstScanNum + prJfifInst->rSOS.u1CompInScan;
            if ((prJdecInst->fgDecodeFirstScanOnly) && (prJdecInst->u4FirstScanNum >= prJfifInst->rSOF.u1NumComponents))
            {
                prJdecInst->eJdecState = JDEC_STATE_DEC_FINISH;
                return;
            }
            else
            {
                prJdecInst->eJdecDecState = JDEC_DECODE_CHECK_NEXT_SCAN;
                return;
            }
        }
        i4Ret = i4JDEC_HAL_Decode_Finish(prJdecInst->u4HwInstId, prJdecInst->fgLastScan,
                                         &(prJdecInst->u4LastScanNum));
        if (S_JDEC_HAL_OK == i4Ret)
        {
            prJdecInst->eJdecState = JDEC_STATE_DEC_FINISH;
        }
        else
        {
            prJdecInst->eJdecDecState = JDEC_DECODE_CHECK_NEXT_SCAN;
        }
        return;
    }
#endif

    //wait scale finish
    if (prJdecInst->u4CurrDecodeMCURow < prJdecInst->u4MCURowNum)
    {
#if JDEC_SUPPORT_BROKEN_TARGET
            if (prJdecInst->fgBrokenTarget)
            {
                vJdecProcSliceScale(prJdecInst);
            }
#endif
#if 1//!JDEC_MCU_ROW_ENHANCE
        vJdecInitNextMCURow(prJdecInst);
#endif
    }
    else if (prJdecInst->u4CurrDecodeMCURow == prJdecInst->u4MCURowNum)
    {
        JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;

        if ((prJfifInst->rSOS.u1Ss == 0) &&
            (prJfifInst->rSOS.u1Ah == 0))
        {
            prJdecInst->u4FirstScanNum = prJdecInst->u4FirstScanNum + prJfifInst->rSOS.u1CompInScan;
            if ((prJdecInst->fgDecodeFirstScanOnly) && (prJdecInst->u4FirstScanNum >= prJfifInst->rSOF.u1NumComponents))
            {
                prJdecInst->eJdecState = JDEC_STATE_DEC_FINISH;
                vJDECPollingFsh(prJdecInst->u4HwId);
                return;
            }
            else
            {
                prJdecInst->eJdecDecState = JDEC_DECODE_CHECK_NEXT_SCAN;
                return;
            }
        }
        i4Ret = i4JDEC_HAL_Decode_Finish(prJdecInst->u4HwInstId, prJdecInst->fgLastScan,
                                         &(prJdecInst->u4LastScanNum));
        if (S_JDEC_HAL_OK == i4Ret)
        {
            prJdecInst->eJdecState = JDEC_STATE_DEC_FINISH;
            vJDECPollingFsh(prJdecInst->u4HwId);
        }
        else
        {
            prJdecInst->eJdecDecState = JDEC_DECODE_CHECK_NEXT_SCAN;
        }
    }
    else
    {
        ASSERT(0);
    }
}


void vJdecProgressProcDecResult(JDEC_INST_T *prJdecInst)
{
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT) ||
        (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE))
    {
        //prJdecInst->u4CurrDecodeMCURow++;
        prJdecInst->u4CurrDecodeMCUH++;
        prJdecInst->eJdecDecState = JDEC_DECODE_DO_SCALE;
    }
    else
#endif
    if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) ||
             (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE))
    {
    prJdecInst->u4TimeOutCount = 0;
    if (prJdecInst->i4JdecDecTimerStatus == 1)
    {
        x_timer_stop(prJdecInst->hJdecDecTimer);
        prJdecInst->i4JdecDecTimerStatus = 0;
    }
    prJdecInst->u4CurrDecodeMCUW++;
    if (prJdecInst->u4CurrDecodeMCUW < prJdecInst->u4MCUInMCURowW)
    {
        prJdecInst->eJdecDecState = JDEC_DECODE_DO_DECODE;
    }
    else if (prJdecInst->u4CurrDecodeMCUW == prJdecInst->u4MCUInMCURowW)
    {
        prJdecInst->u4CurrDecodeMCUW = 0;
        prJdecInst->u4CurrDecodeMCUH++;
        if (prJdecInst->u4CurrDecodeMCUH < prJdecInst->u4MCUInMCURowH)
        {
            prJdecInst->eJdecDecState = JDEC_DECODE_DO_DECODE;
        }
        else if (prJdecInst->u4CurrDecodeMCUH == prJdecInst->u4MCUInMCURowH)
        {
            prJdecInst->u4CurrDecodeMCUH = 0;
            prJdecInst->eJdecDecState = JDEC_DECODE_DO_SCALE;
        }
        else
        {
            ASSERT(0);
        }
    }
    else
    {
        ASSERT(0);
    }
}

}

INT32 i4JdecDecodeProgressMcuRow(JDEC_INST_T *prJdecInst)
{
    UINT32 u4x,u4y;
    INT32 i4Ret = 0;
    UINT32 u4Count = 0;

#if ((CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530))
    if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)||
        (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE))
    {
        i4JDEC_HAL_Decode(prJdecInst->u4HwInstId);
        prJdecInst->u4CurrDecodeMCUW = 0;
        prJdecInst->u4CurrDecodeMCUH = 0;
        prJdecInst->eJdecDecState = JDEC_DECODE_WAIT_DECODE;
        return JDEC_DRV_RTN_OK;
    }
#endif
    for (u4y = 0;u4y < prJdecInst->u4MCUInMCURowH; u4y++)
    {
        prJdecInst->u4CurrDecodeMCUW = 0;
        for (u4x = 0; u4x < prJdecInst->u4MCUInMCURowW; u4x ++)
        {
            // Decode one mcu row
            //i4JDEC_HAL_Decode(prJdecInst->u4HwInstId);
            if (i4JDEC_HAL_Decode(prJdecInst->u4HwInstId) < 0)
            {
                //i4JdecScaleStop(prJdecInst);
                prJdecInst->i4JdecErrorType = -E_JPG_ERR_DEC_ERROR;
				UTIL_Printf("[JDEC] %s line %dE_JPG_ERR_DEC_ERROR\n", __FUNCTION__, __LINE__);
                break;
            }
            i4Ret = i4JDEC_HAL_Get_Decode_Status(prJdecInst->u4HwInstId);
            while (i4Ret != S_JDEC_HAL_OK)
            {
                //x_thread_delay(2);
                u4Count ++;
                if (u4Count > 100000)
                {
                    // decode time out, return error code
                    prJdecInst->i4JdecErrorType = -E_JPG_ERR_DEC_ERROR;
					UTIL_Printf("[JDEC] %s line %dE_JPG_ERR_DEC_ERROR\n", __FUNCTION__, __LINE__);
                    break;
                }
                i4Ret = i4JDEC_HAL_Get_Decode_Status(prJdecInst->u4HwInstId);
            }
            if (prJdecInst->i4JdecErrorType < 0 )
                return JDEC_DRV_RTN_FAIL;
            prJdecInst->u4CurrDecodeMCUW ++;
            u4Count = 0;
        }
        prJdecInst->u4CurrDecodeMCUH ++;
    }

    prJdecInst->u4CurrDecodeMCUW = 0;
    prJdecInst->u4CurrDecodeMCUH = 0;

    if (prJdecInst->i4JdecErrorType >= 0)
    {
        prJdecInst->eJdecDecState = JDEC_DECODE_DO_SCALE;
        return JDEC_DRV_RTN_OK;
    }
    else
    {
        return JDEC_DRV_RTN_FAIL;
    }
}

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)

INT32 i4JdecStartProgressScanMode(JDEC_INST_T *prJdecInst)
{
    EV_GRP_EVENT_T  eJdecEvent;
    EV_GRP_EVENT_T  eJdecEventGet;
    INT32           i4Ret = OSR_OK;
    

    #if JDEC_USE_MAP_UNMAP
    
    // frist scan
    if (prJdecInst->u4ScanCount == 0 )
    {
        vJdecMapPrintf("[JDEC]Prog Map from 0x%x to 0x%x , point 5 \n",(UINT32)prJdecInst->pvDstYBuf, 
                     ((UINT32)prJdecInst->pvDstYBuf + prJdecInst->u4DstBufPitch * (prJdecInst->u4DstCropPosY + prJdecInst->u4DstCropHeight)));
        BSP_dma_map_vaddr((UINT32)prJdecInst->pvDstYBuf, prJdecInst->u4DstBufPitch * (prJdecInst->u4DstCropPosY + prJdecInst->u4DstCropHeight), FROM_DEVICE); 

    }
    
    {
        UINT32 u4Start, u4End;
        u4Start = prJdecInst->rFifoInfo.u4RdPoint / JDEC_CACHE_LINE_ALIGN * JDEC_CACHE_LINE_ALIGN;
        u4End = prJdecInst->rFifoInfo.u4WrPoint / JDEC_CACHE_LINE_ALIGN * JDEC_CACHE_LINE_ALIGN;
        
        if (prJdecInst->rFifoInfo.u4WrPoint > prJdecInst->rFifoInfo.u4RdPoint)
        {   
            vJdecMapPrintf("[JDEC]Map from 0x%x to 0x%x , point 6 \n",u4Start, u4End);
            BSP_dma_map_vaddr(u4Start, (u4End - u4Start), TO_DEVICE);   
        }
        else
        { 
            vJdecMapPrintf("[JDEC]Map from 0x%x to 0x%x , point 7 \n",u4Start, prJdecInst->rFifoInfo.u4FifoEnd);
            vJdecMapPrintf("[JDEC]Map from 0x%x to 0x%x , point 7 \n",prJdecInst->rFifoInfo.u4FifoStart, u4End);
            BSP_dma_map_vaddr(u4Start, (prJdecInst->rFifoInfo.u4FifoEnd - u4Start),TO_DEVICE);
            BSP_dma_map_vaddr(prJdecInst->rFifoInfo.u4FifoStart,(u4End - prJdecInst->rFifoInfo.u4FifoStart),TO_DEVICE);
        }

        prJdecInst->u4MapSA = prJdecInst->rFifoInfo.u4WrPoint;
    }
    #endif

    if ((prJdecInst->fgLastScan) || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
    {
        i4JdecScaleSetNextPartialBuf(prJdecInst);
        i4JdecScaleFire(prJdecInst);
        eJdecEvent = JDEC_DRV_EV_SCALE_READY;
        i4Ret = x_ev_group_wait_event_timeout(prJdecInst->hJdecEvent, eJdecEvent, &eJdecEventGet, X_EV_OP_OR_CONSUME, 1000);
        if (i4Ret != OSR_OK) // time out
        {
            prJdecInst->i4JdecErrorType = -E_JPG_ERR_SCALE_ERROR;
            //i4JdecScaleStop(prJdecInst);
            return JDEC_DRV_RTN_FAIL;
        }
    }
    i4Ret = i4JDEC_HAL_Decode(prJdecInst->u4HwId);

    prJdecInst->eJdecDecState = JDEC_DECODE_WAIT_DECODE;
    return JDEC_DRV_RTN_OK;
}

INT32 i4JdecWaitProgressMcuRowDecode(JDEC_INST_T *prJdecInst)
{
    EV_GRP_EVENT_T  eJdecEventGet = 0;
    UINT32 u4Temp = 0;
    INT32 i4Ret = 0;

    while (1)
    {
        i4Ret = x_ev_group_wait_event_timeout(prJdecInst->hJdecEvent,(JDEC_DRV_EV_DEC_DONE | JDEC_DRV_EV_DEC_ERR),&eJdecEventGet,X_EV_OP_OR_CONSUME, 0);
        if (i4Ret != OSR_OK)
        {
            x_thread_delay(5);
            u4Temp ++;
            if (u4Temp > 400) // decode time out
            {
                //i4JdecScaleStop(prJdecInst);
                //vHwJdecPrintRegister(0);
                prJdecInst->i4JdecErrorType = -E_JPG_ERR_DEC_ERROR;
				 UTIL_Printf("[JDEC] %s line %dE_JPG_ERR_DEC_ERROR\n", __FUNCTION__, __LINE__);
                return JDEC_DRV_RTN_FAIL;
            }
        }
        else
        {
            if (eJdecEventGet & JDEC_DRV_EV_DEC_ERR)
            {
                //vHwJdecPrintRegister(0);
                prJdecInst->i4JdecErrorType = E_JPG_ERR_DEC_ERROR;
				 UTIL_Printf("[JDEC] %s line %dE_JPG_ERR_DEC_ERROR\n", __FUNCTION__, __LINE__);
                return JDEC_DRV_RTN_FAIL;
            }
            else if (eJdecEventGet & JDEC_DRV_EV_DEC_DONE)
            {
                break;
            }
        }

        i4Ret = x_ev_group_wait_event_timeout(prJdecInst->hJdecEvent, JDEC_DRV_EV_STOP, &eJdecEventGet, X_EV_OP_OR_CONSUME, 0);
        if (i4Ret == OSR_OK)
        {
            vJdecSetLog(JDEC_LOG_DEC_STOP);
            vJdecAbort(prJdecInst, (IMG_NFY_STATE_T)IMG_NFY_STOP_DONE, FALSE);
            return JDEC_DRV_RTN_STOP;
        }

    }

    return JDEC_DRV_RTN_OK;
}

INT32 i4JdecWaitProgressScanDecode(JDEC_INST_T *prJdecInst)
{
    INT32 i4Ret = 0;
    UINT32 u4RdPointer = 0;
    UINT32 u4WrPointer = 0;
    //UINT32 u4Count = 0; // how many times the vld read pointer not change
    EV_GRP_EVENT_T  eJdecEvent;
    EV_GRP_EVENT_T  eJdecEventGet;


    u4RdPointer = prJdecInst->rFifoInfo.u4RdPoint;
    u4WrPointer = prJdecInst->rFifoInfo.u4WrPoint;
    while (1)
    {
        if (prJdecInst->i4JdecErrorType < 0)
        {
            //i4JdecScaleStop(prJdecInst);
            return JDEC_DRV_RTN_FAIL;
        }

        if (prJdecInst->fgWaitStop)
        {
            if (i4JDEC_HAL_GET_STOP_STATUS(prJdecInst->u4HwInstId) == S_JDEC_HAL_OK)
            {
                prJdecInst->fgWaitStop = FALSE;
                vJdecAbort(prJdecInst, (IMG_NFY_STATE_T)IMG_NFY_STOP_DONE, FALSE);
                i4JdecScaleStop(prJdecInst);
                return JDEC_DRV_RTN_FAIL;
            }
        }


        eJdecEvent = JDEC_DRV_EV_STOP|JDEC_DRV_EV_DEC_TIME_OUT|JDEC_DRV_EV_DEC_DONE|JDEC_DRV_EV_DEC_ERR;
        i4Ret = x_ev_group_wait_event_timeout(prJdecInst->hJdecEvent, eJdecEvent, &eJdecEventGet, X_EV_OP_OR_CONSUME, 2);
        if (i4Ret == OSR_OK)
        {
            if (eJdecEventGet & JDEC_DRV_EV_STOP)
            {
                vJDEC_HAL_StopPicModeDec(prJdecInst->u4HwInstId);
                prJdecInst->fgWaitStop = TRUE;
            }
            else if ((eJdecEventGet & JDEC_DRV_EV_DEC_TIME_OUT)||
                     (eJdecEventGet & JDEC_DRV_EV_DEC_ERR))
            {
                //i4JdecScaleStop(prJdecInst);
                prJdecInst->i4JdecErrorType = -E_JPG_ERR_DEC_ERROR;
				 UTIL_Printf("[JDEC] %s line %dE_JPG_ERR_DEC_ERROR\n", __FUNCTION__, __LINE__);
                // wether need to stop picture mode ?
                return JDEC_DRV_RTN_FAIL;
            }
            else
            {
                #if 0 // temp comment
                x_timer_stop(prJdecInst->hJdecDecTimer);
                #endif
                prJdecInst->i4JdecDecTimerStatus = 0;
                break;
            }
         }
         // For some case, when jpg is not stop ready, image library
         // send frame end ,then start to decode another jpg, the
         // fgWaitStop flag will abort the next jpg decoding
         if (!prJdecInst->fgWaitStop)
         {
             x_thread_delay(50);
         }

#if 1    // for decode error detection, need be open after test
         // if parsing data finish ,should update read pointer manually
         if (prJdecInst->rFifoInfo.fgEoRange)
         {
             BOOL fgValue = FALSE;

             vJdecUpdateWritePointer((void *)(&fgValue), (void *)prJdecInst->u4HwInstId);
         }

         if (prJdecInst->rFifoInfo.u4RdPoint == u4RdPointer) // read pointer stop
         {
             if (prJdecInst->i4JdecDecTimerStatus == 0)
             {
                 i4Ret = x_timer_stop(prJdecInst->hJdecDecTimer);
                 i4Ret = x_timer_start(prJdecInst->hJdecDecTimer, 1000, X_TIMER_FLAG_ONCE, (x_os_timer_cb_fct)vJdecDecTimerCb, (void*)((UINT32)prJdecInst->u2CompId));
                 VERIFY(OSR_OK == i4Ret);
                 prJdecInst->i4JdecDecTimerStatus = 1;
             }

             if (!(prJdecInst->rFifoInfo.fgEoRange))
             {
                 if (prJdecInst->rFifoInfo.u4WrPoint == u4WrPointer)
                 {
                     if (!fgImgdmaIsTransing(prJdecInst->u4ImgdmaId))
                     {
                         BOOL fgValue = TRUE;
                         vJdecUpdateWritePointer((void *)(&fgValue), (void *)prJdecInst->u4HwInstId);
                     }
                 }

                 u4WrPointer = prJdecInst->rFifoInfo.u4WrPoint;
             }
         }
         else
         {
             u4RdPointer = prJdecInst->rFifoInfo.u4RdPoint;
             u4WrPointer = prJdecInst->rFifoInfo.u4WrPoint;
             //u4Count = 0;
             x_timer_stop(prJdecInst->hJdecDecTimer);
             prJdecInst->i4JdecDecTimerStatus = 0;
         }
#endif
    }
    vJdecProgressWaitScale(prJdecInst);
    // should add time out
    return JDEC_DRV_RTN_OK;
}

#endif

#if JDEC_SUPPORT_BROKEN_TARGET
void vJdecCopyPreloadData(JDEC_INST_T *prJdecInst)
{
    HANDLE_T hTemp = NULL_HANDLE;
    void * pvBlkSa = NULL;

#if 0
    if (prJdecInst->fgBrokenTarget)
    {
        if ((prJdecInst->rBlkInfo.u4BlkSize - prJdecInst->rBlkInfo.u4BlkOfst) < prJdecInst->u4McuRowLength)
        {
            prJdecInst->fgBrokenRow = TRUE;
        }
    }
#endif
    vJdecLog_L("[JDEC] vJdecCopyPreloadData , u4BlkOfst = %x \n",prJdecInst->rBlkInfo.u4BlkOfst);
    if ((prJdecInst->fgBrokenRow) && (prJdecInst->fgBrokenTarget))
    {
        UINT32 u4Length = (prJdecInst->rBlkInfo.u4BlkSize - prJdecInst->rBlkInfo.u4BlkOfst);
        x_memcpy((void *)prJdecInst->u4TempAYUVBuf,
                 (void *)(prJdecInst->rBlkInfo.u4BlkSa + prJdecInst->rBlkInfo.u4BlkOfst),
                 u4Length);
        hTemp =  prJdecInst->rBlkInfo.hMem;//x_kmem_block_handle(prJdecInst->pvDstYBuf);
        pvBlkSa = x_kmem_kernel_address(hTemp, (prJdecInst->rBlkInfo.u4BlkIdx + 1) * prJdecInst->rBlkInfo.u4BlkSize);
        x_memcpy((void *)(prJdecInst->u4TempAYUVBuf + u4Length),pvBlkSa,
                 (prJdecInst->u4McuRowLength - u4Length));
        #if !JDEC_USE_MAP_UNMAP
        BSP_FlushDCacheRange(prJdecInst->u4TempAYUVBuf,prJdecInst->u4McuRowLength);
        #endif
    }
}
#endif

void vJdecProcProgressDec(JDEC_INST_T *prJdecInst)
{
    INT32 i4Ret = 0;

    switch (prJdecInst->eJdecDecState)
    {
        case JDEC_DECODE_DO_DECODE:
		vJdecLog_L("[JDEC]  JDEC_DECODE_DO_DECODE \n");
#if 1//JDEC_PROGRESSIVE_IMPROVE
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT) ||
                (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
            {
                //UTIL_Printf("[JDEC_DRV] Scan decode start \n");
                i4Ret = i4JdecStartProgressScanMode(prJdecInst);
            }
            else
#endif
            {
                i4Ret = i4JdecDecodeProgressMcuRow(prJdecInst);
                if (i4Ret != JDEC_DRV_RTN_OK)
                {
                    return;
                }
            }
#else
            i4JDEC_HAL_Decode(prJdecInst->u4HwInstId);
            prJdecInst->eJdecDecState = JDEC_DECODE_WAIT_DECODE;
            _u4JdecDecodeTimes1 = u4JdecGetSTC();
            _u4JdecDecodeTimes2 = 0;
#endif
            break;
        case JDEC_DECODE_WAIT_DECODE:
            if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) ||
                (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE))
            {
            }
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            else if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)||
                     (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE))
            {
                i4Ret = i4JdecWaitProgressMcuRowDecode(prJdecInst);
                if (i4Ret != JDEC_DRV_RTN_OK)
                            return;
                else
                {
                    vJdecProgressProcDecResult(prJdecInst);
                        }
                    }
            else
                    {
                //EV_GRP_EVENT_T  eJdecEventGet;

                //UTIL_Printf("[JDEC_DRV] Wait scan decode done \n");
                i4Ret = i4JdecWaitProgressScanDecode(prJdecInst);
                if (i4Ret != JDEC_DRV_RTN_OK)
                        return;

                i4Ret = x_timer_stop(prJdecInst->hJdecDecTimer);
#if 0
                /* Temp solution for scan mode stop*/
                    i4Ret = x_ev_group_wait_event_timeout(prJdecInst->hJdecEvent, JDEC_DRV_EV_STOP, &eJdecEventGet, X_EV_OP_OR_CONSUME, 0);
                    if (i4Ret == OSR_OK)
                    {
                    i4JdecScaleStop(prJdecInst);
                        vJdecAbort(prJdecInst, (IMG_NFY_STATE_T)IMG_NFY_STOP_DONE, FALSE);
                        return;
                    }
#endif
                //UTIL_Printf("[JDEC_DRV] scan decode done \n");
            }
#endif
            break;
        case JDEC_DECODE_DO_SCALE:
            //get read point from HAL
            if (i4JdecGetHalRdPoint(prJdecInst) == JDEC_DRV_RTN_FAIL)
            {
                prJdecInst->i4JdecErrorType = -E_JPG_ERR_HAL_GET_RD;
                return;
            }
            #if JDEC_USE_MAP_UNMAP
            if(prJdecInst->rFifoInfo.u4RdPoint > prJdecInst->rFifoInfo.u4PreRdPoint)
            {
                UINT32 u4Start, u4End;

                u4Start = prJdecInst->rFifoInfo.u4PreRdPoint/JDEC_CACHE_LINE_ALIGN *JDEC_CACHE_LINE_ALIGN;
                u4End = prJdecInst->rFifoInfo.u4RdPoint/JDEC_CACHE_LINE_ALIGN *JDEC_CACHE_LINE_ALIGN;
                vJdecMapPrintf("[JDEC] Unmap from 0x%x to 0x%x point 6\n",u4Start, u4End);
                BSP_dma_unmap_vaddr(u4Start, (u4End - u4Start),TO_DEVICE);
            }
            else
            {
                UINT32 u4Start, u4End;

                u4Start = prJdecInst->rFifoInfo.u4PreRdPoint/JDEC_CACHE_LINE_ALIGN *JDEC_CACHE_LINE_ALIGN;
                u4End = prJdecInst->rFifoInfo.u4RdPoint/JDEC_CACHE_LINE_ALIGN *JDEC_CACHE_LINE_ALIGN;

                vJdecMapPrintf("[JDEC] Unmap from 0x%x to 0x%x point 7\n",u4Start, prJdecInst->rFifoInfo.u4FifoEnd);
                vJdecMapPrintf("[JDEC] Unmap from 0x%x to 0x%x point 7\n",prJdecInst->rFifoInfo.u4FifoStart, u4End);
                BSP_dma_unmap_vaddr(u4Start, (prJdecInst->rFifoInfo.u4FifoEnd - u4Start), TO_DEVICE);
                BSP_dma_unmap_vaddr(prJdecInst->rFifoInfo.u4FifoStart,(u4End- prJdecInst->rFifoInfo.u4FifoStart),TO_DEVICE);   
            }
            #endif
            //update current read point
            if (i4JdecUpdateImgdmaRdPoint(prJdecInst) == JDEC_DRV_RTN_FAIL)
            {
                prJdecInst->i4JdecErrorType = -E_JPG_ERR_HAL_UPT_RD;
                return;
            }
            if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
                || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
#endif
                )
            {
                if (!prJdecInst->fgLastScan)
                {
                    prJdecInst->u4CurrDecodeMCURow++;
                    if (prJdecInst->u4CurrDecodeMCURow < prJdecInst->u4MCURowNum)
                    {
                        prJdecInst->eJdecDecState = JDEC_DECODE_DO_DECODE;
                        vJdecCaluLastMCURowH(prJdecInst);
                    }
                    else if (prJdecInst->u4CurrDecodeMCURow == prJdecInst->u4MCURowNum)
                    {
                        prJdecInst->eJdecDecState = JDEC_DECODE_CHECK_NEXT_SCAN;
                    }
                    else
                    {
                        ASSERT(0);
                    }
                    break;
                }
                else
                {
#if JDEC_SUPPORT_BROKEN_TARGET
                    if (prJdecInst->fgBrokenTarget)
                    {
                        vJdecFillCBuf(prJdecInst);
                        i4JdecSetSliceDstInfo(prJdecInst);
                    }
#endif
                    i4JdecScaleSetNextPartialBuf(prJdecInst);
                    i4JdecScaleFire(prJdecInst);
                    prJdecInst->u4CurrDecodeMCURow++;
#if 1//!JDEC_MCU_ROW_ENHANCE
                    vJdecProgressWaitScale(prJdecInst);
#else
                    if (prJdecInst->u4CurrDecodeMCURow == prJdecInst->u4MCURowNum)
                    {
                        vJdecProgressWaitScale(prJdecInst);
                    }
                    else
                    {
                        vJdecInitNextMCURow(prJdecInst);
                    }
#endif
                }
            }
            else if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
                     ||(prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
#endif
                    )
            {
#if JDEC_SUPPORT_BROKEN_TARGET
                if (prJdecInst->fgBrokenTarget)
                {
                    vJdecFillCBuf(prJdecInst);
                    vJdecCopyPreloadData(prJdecInst);
                    i4JdecSetSliceDstInfo(prJdecInst);
                }
#endif
                //UTIL_Printf("[JDEC] Scale start \n");
                i4JdecScaleSetNextPartialBuf(prJdecInst);
                i4JdecScaleFire(prJdecInst);
                prJdecInst->u4CurrDecodeMCURow++;
#if 1//!JDEC_MCU_ROW_ENHANCE
                vJdecProgressWaitScale(prJdecInst);
#else
                if (prJdecInst->u4CurrDecodeMCURow == prJdecInst->u4MCURowNum)
                {
                vJdecProgressWaitScale(prJdecInst);
            }
            else
            {
                    vJdecInitNextMCURow(prJdecInst);
                }
#endif
            }
            else
            {
                ASSERT(0);
            }
            break;
        case JDEC_DECODE_CHECK_NEXT_SCAN:
            //check next scan from HAL
            vJDECPollingFsh(prJdecInst->u4HwId);
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            if (((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)||
                (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
                /*&& (!prJdecInst->rFifoInfo.fgEoRange)*/)
            {
                if (i4JdecGetHalRdPoint(prJdecInst) == JDEC_DRV_RTN_FAIL)
                {
                    prJdecInst->i4JdecErrorType = -E_JPG_ERR_HAL_GET_RD;
                    return;
                }
                #if JDEC_USE_MAP_UNMAP
                // one scan is decode done, we need unmap all the valid data in fifo, for sw read
                if(prJdecInst->rFifoInfo.u4WrPoint > prJdecInst->rFifoInfo.u4PreRdPoint)
                {
                    UINT32 u4Start, u4End;

                    u4Start = prJdecInst->rFifoInfo.u4PreRdPoint / JDEC_CACHE_LINE_ALIGN * JDEC_CACHE_LINE_ALIGN;
                    u4End = prJdecInst->rFifoInfo.u4WrPoint/ JDEC_CACHE_LINE_ALIGN * JDEC_CACHE_LINE_ALIGN;
                    vJdecMapPrintf("[JDEC] Unmap from 0x%x to 0x%x point 7\n",prJdecInst->rFifoInfo.u4PreRdPoint, prJdecInst->rFifoInfo.u4WrPoint);
                    BSP_dma_unmap_vaddr(prJdecInst->rFifoInfo.u4PreRdPoint, (prJdecInst->rFifoInfo.u4WrPoint - prJdecInst->rFifoInfo.u4PreRdPoint),TO_DEVICE);
                }
                else
                {
                    UINT32 u4Start, u4End;

                    u4Start = prJdecInst->rFifoInfo.u4PreRdPoint / JDEC_CACHE_LINE_ALIGN * JDEC_CACHE_LINE_ALIGN;
                    u4End = prJdecInst->rFifoInfo.u4WrPoint/ JDEC_CACHE_LINE_ALIGN * JDEC_CACHE_LINE_ALIGN;

                    vJdecMapPrintf("[JDEC] Unmap from 0x%x to 0x%x point 8\n",u4Start, prJdecInst->rFifoInfo.u4FifoEnd);
                    vJdecMapPrintf("[JDEC] Unmap from 0x%x to 0x%x point 8\n",prJdecInst->rFifoInfo.u4FifoStart, u4End);
                    
                    BSP_dma_unmap_vaddr(u4Start, (prJdecInst->rFifoInfo.u4FifoEnd - u4Start), TO_DEVICE);
                    BSP_dma_unmap_vaddr(prJdecInst->rFifoInfo.u4FifoStart,(u4End- prJdecInst->rFifoInfo.u4FifoStart),TO_DEVICE);                }
                #endif
                //update current read point
                if (i4JdecUpdateImgdmaRdPoint(prJdecInst) == JDEC_DRV_RTN_FAIL)
                {
                    prJdecInst->i4JdecErrorType = -E_JPG_ERR_HAL_UPT_RD;
                    return;
                }
            }
#endif

#if JDEC_SUPPORT_BROKEN_TARGET
            if (prJdecInst->fgBrokenTarget)
            {
                // at start of each scan, reset the block info
                prJdecInst->rBlkInfo.u4BlkIdx = 0;
                prJdecInst->rBlkInfo.u4BlkSa = (UINT32)prJdecInst->pvDstYBuf;
                prJdecInst->rBlkInfo.u4BlkOfst = 0;
            }
#endif
            if (i4JdecChkFifoStatus(prJdecInst) == JDEC_DRV_RTN_OK)
            {
                if (i4JdecCheckNextScanMarker(prJdecInst) == JDEC_DRV_RTN_OK)
                {
                    if (prJdecInst->eJdecState != JDEC_STATE_DEC_FINISH)
                    {
                        JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;

                        if ((prJdecInst->fgDecodeFirstScanOnly) &&
                            !((prJfifInst->rSOS.u1Ss == 0) && (prJfifInst->rSOS.u1Se == 0) && (prJfifInst->rSOS.u1Ah== 0)))
                        {
                            return; // continue to search next scan
                        }

                        vJdecInitNextMCURow(prJdecInst);
                    }
                }
                else
                {
                    prJdecInst->i4JdecErrorType = -E_JPG_ERR_NO_NEXT_MARKER;
                    return;
                    //ASSERT(0);
                }
            }
            else
            {
                return;
            }
            break;
        case JDEC_DECODE_RE_DECODE:
            vJdecReDecode(prJdecInst);
            break;
        default:
            ASSERT(0);
            break;
    }
}

void vJdecStateDecode(JDEC_INST_T *prJdecInst)
{
    if (i4JdecChkFifoStatus(prJdecInst) == JDEC_DRV_RTN_OK)
    {
        switch (prJdecInst->eJdecDecMode)
        {
            case JDEC_HAL_DEC_MODE_BASELINE_PIC:
            case JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW:
            case JDEC_HAL_DEC_MODE_BASELINE_MCU:
                vJdecProcBaselineDec(prJdecInst);
                break;
            case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT:
            case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE:
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT:
            case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE:
            case JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT:
            case JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE:
#endif
                vJdecProcProgressDec(prJdecInst);
                break;
            default:
                ASSERT(0);
        }
    }
}

void vJdecSleepTimerCb(HANDLE_T hTimer, VOID *pvTag, UINT32 u4ChId)
{
    INT32 i4Ret;
    JDEC_INST_T *prJdecInst = &(_rJdecInst[(UINT32)pvTag]);

    i4Ret = x_ev_group_set_event(prJdecInst->hJdecEvent, JDEC_DRV_EV_SLEEP, X_EV_OP_OR);
    VERIFY(i4Ret == OSR_OK);

}


void vJdecStateStart(JDEC_INST_T *prJdecInst)
{
#if CONFIG_SYS_MEM_PHASE3
    HANDLE_T hTemp = NULL_HANDLE;
#endif
#if SUPPORT_VDEC_PATH
    JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;
#endif


    //for performance testing
    _u4JdecDecodeTimes = 0;
    _u4JdecScaleTimes = 0;
    _u4JdecTrnsTotalTimes = 0;
    _u4JdecDecTotalTimes = 0;
    _u4JdecScaleTotalTimes = 0;
    _u4JdecDecFailCount = 0;

    //check cache state, todo

    //init decode val
    vJdecInitDecVal(prJdecInst);
    //init data and fifo
#if !JDEC_DISCONTINUOUS_SOURCE
    if ((prJdecInst->pvImgBuf != NULL)
#if CONFIG_ENABLE_IOMMU		
	&& prJdecInst->fgJdecKernImgBuf
#endif 	
	)
    {
        prJdecInst->rFifoInfo.u4FifoStart = (UINT32)prJdecInst->pvImgBuf;
        prJdecInst->rFifoInfo.u4FifoEnd = prJdecInst->rFifoInfo.u4FifoStart + prJdecInst->u4ImgSize + 256;
        prJdecInst->rFifoInfo.u4RdPoint = prJdecInst->rFifoInfo.u4FifoStart;
        prJdecInst->rFifoInfo.u4PreRdPoint = prJdecInst->rFifoInfo.u4FifoStart;
        prJdecInst->rFifoInfo.u4WrPoint = prJdecInst->rFifoInfo.u4FifoEnd;
        prJdecInst->rFifoInfo.fgEoRange = TRUE;
        prJdecInst->rFifoInfo.u4FileOfst = 0;
        prJdecInst->rFifoInfo.u4ValidData = prJdecInst->u4ImgSize;
	#if	JDEC_IO_MMU_TEST
        //x_kmem_sync_table((void *)prJdecInst->pvImgBuf, prJdecInst->u4ImgSize);  dddd iommu
	#endif
    }
    else
#endif
#if SUPPORT_VDEC_PATH
    if ((prJdecInst->pvImgBuf != NULL) && (prJdecInst->fgVdecPath))
    {
        prJdecInst->rFifoInfo.u4FifoStart = prJfifInst->u4FifoStart;
        prJdecInst->rFifoInfo.u4FifoEnd = prJfifInst->u4FifoEnd;
        prJdecInst->rFifoInfo.u4RdPoint = (UINT32)prJdecInst->pvImgBuf;
        prJdecInst->rFifoInfo.u4PreRdPoint = (UINT32)prJdecInst->pvImgBuf;
        if (((UINT32)prJdecInst->pvImgBuf + prJdecInst->u4ImgSize) >= prJdecInst->rFifoInfo.u4FifoEnd)
        {
            prJdecInst->rFifoInfo.u4WrPoint = (UINT32)prJdecInst->pvImgBuf + \
                                              prJdecInst->u4ImgSize - \
                                              prJdecInst->rFifoInfo.u4FifoEnd + \
                                              prJdecInst->rFifoInfo.u4FifoStart;
        }
        else
        {
            prJdecInst->rFifoInfo.u4WrPoint = (UINT32)prJdecInst->pvImgBuf + prJdecInst->u4ImgSize;
        }
        prJdecInst->rFifoInfo.fgEoRange = TRUE;
        prJdecInst->rFifoInfo.u4FileOfst = 0;//prJdecInst->pvImgBuf - prJdecInst->rFifoInfo.u4FifoStart;
        prJdecInst->rFifoInfo.u4ValidData = prJdecInst->u4ImgSize;
    }
    else

#endif
    {
        if (prJdecInst->u2CompId > (JDEC_INST_NUM - 1))
        {
            ASSERT(0);
        }
        if (_aru4FifoStart[prJdecInst->u2CompId] != JDEC_INVALID32)
        {
            ASSERT(0);
            prJdecInst->i4JdecErrorType = -E_JPG_GET_FIFO_MEM_FAIL;
            return;
        }
        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
        _aru4FifoStart[prJdecInst->u2CompId] = (UINT32)x_alloc_aligned_dma_mem(JDEC_DRV_FIFO_SIZE, 128);
#elif CONFIG_SYS_MEM_PHASE3
        #if JDEC_IO_MMU_TEST
        _aru4FifoStart[prJdecInst->u2CompId] = (UINT32)x_mem_valloc(JDEC_DRV_FIFO_SIZE);
        //UTIL_Printf("[jdec]---_aru4FifoStart = [0x%x]\n",_aru4FifoStart[prJdecInst->u2CompId]);
        x_kmem_sync_table((void *)_aru4FifoStart[prJdecInst->u2CompId],JDEC_DRV_FIFO_SIZE);        
        hTemp = x_kmem_create_handle((void *)_aru4FifoStart[prJdecInst->u2CompId], JDEC_DRV_FIFO_SIZE);
        prJdecInst->rFifoInfo.hFifo = hTemp;
        #else
        //_aru4FifoStart[prJdecInst->u2CompId] = (UINT32)x_mem_aligned_alloc(JDEC_DRV_FIFO_SIZE, 128);
        hTemp = x_kmem_block_aligned_alloc(JDEC_DRV_FIFO_SIZE, 128);
        _aru4FifoStart[prJdecInst->u2CompId] = (UINT32)x_kmem_kernel_address(hTemp, 0);
        prJdecInst->rFifoInfo.hFifo = hTemp;
#endif
#endif
        #else
        _aru4FifoStart[prJdecInst->u2CompId] = (UINT32)x_alloc_aligned_nc_mem(JDEC_DRV_FIFO_SIZE, 128);
        #endif

        if (_aru4FifoStart[prJdecInst->u2CompId] == 0)
        {
            prJdecInst->i4JdecErrorType = -E_JPG_GET_FIFO_MEM_FAIL;
            _aru4FifoStart[prJdecInst->u2CompId] = JDEC_INVALID32;
            return;
        }
        //_aru4FifoStart[prJdecInst->u2CompId] = (UINT32)addr_kernel_to_user((void *)_aru4FifoStart[prJdecInst->u2CompId]);
        prJdecInst->rFifoInfo.u4FifoStart = _aru4FifoStart[prJdecInst->u2CompId];
        prJdecInst->rFifoInfo.u4FifoEnd = prJdecInst->rFifoInfo.u4FifoStart + JDEC_DRV_FIFO_SIZE;
        prJdecInst->rFifoInfo.fgEoRange = FALSE;
        prJdecInst->rFifoInfo.u4FileOfst = 0;

        vJdecLog_L("[JDEC] Fifo SA = %x \n",prJdecInst->rFifoInfo.u4FifoStart);
        //UTIL_Printf("[JDEC_DRV] Fifo start %x, fifo end %x \n",prJdecInst->rFifoInfo.u4FifoStart,prJdecInst->rFifoInfo.u4FifoEnd);
        //trigger img dma to start transfer
        i4JdecStartTranData(prJdecInst, prJdecInst->rFifoInfo.u4FileOfst, (prJdecInst->u4ImgSize - prJdecInst->rFifoInfo.u4FileOfst));
    }
    vJdecInitPrsInstData(prJdecInst);

    prJdecInst->eJdecState = JDEC_STATE_PRS_HDR;
}

BOOL fgJdecCheckHdr(JDEC_INST_T *prJdecInst)
{
	 JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;
	 
	 if((0==prJfifInst->rSOF.arSofComp[0].u1HSampFactor) ||
	 	  (0==prJfifInst->rSOF.arSofComp[0].u1VSampFactor))
	{
		 return FALSE;
	} 		
	
	return TRUE;
}

void vJdecStatePrsHdr(JDEC_INST_T *prJdecInst)
{
    INT32 i4Ret = 0;
    JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;
    JDEC_FIFO_INFO_T *prFifoInfo = &(prJdecInst->rFifoInfo);

    //check fifo status, enough?

#if SUPPORT_VDEC_PATH
    if (!prJdecInst->fgVdecPath)
    {
#endif
    if (i4JdecChkFifoStatus(prJdecInst) == JDEC_DRV_RTN_OK)
    {
        UINT32 u4PreReadPoint =0;
        if ((prFifoInfo->fgEoRange == TRUE) && (prFifoInfo->u4ValidData < 4))
        {
            prJdecInst->i4JdecErrorType = -E_JPG_ERR_PASS_END;
            return;
        }
        //init Jfif Instance, fifo info
        prJfifInst->u4FifoStart = prFifoInfo->u4FifoStart;
        prJfifInst->u4FifoEnd = prFifoInfo->u4FifoEnd;
        prJfifInst->fgPrsResult = FALSE;
        //parser header
        u4PreReadPoint =prFifoInfo->u4RdPoint;
        vJdecLog_L("A: fgEoRange =[%d] u4ValidData 0x%x update u4RdPoint 0x%x\n",prFifoInfo->fgEoRange,prFifoInfo->u4ValidData,prFifoInfo->u4RdPoint);
        i4Ret = JFIF_New(prJdecInst->hJfifInst, (void *)(prFifoInfo->u4RdPoint), prFifoInfo->u4ValidData);
        prFifoInfo->u4RdPoint = (UINT32)(prJfifInst->pu1CurrRead);
	    vJdecLog_L("B:prFifoInfo->fgEoRange'%d] u4ValidData 0x%x update u4RdPoint 0x%x\n",prFifoInfo->fgEoRange,prFifoInfo->u4ValidData,prFifoInfo->u4RdPoint);
        if((u4PreReadPoint == prFifoInfo->u4RdPoint) && (prFifoInfo->fgEoRange == TRUE))
        {
            prJdecInst->i4JdecErrorType = -E_JPG_ERR_PASS_END;
            return;

        }
	}
    else
    {
        return;
    }
#if SUPPORT_VDEC_PATH
    }
    else
    {
        prFifoInfo->u4RdPoint = (UINT32)(prJfifInst->pu1CurrRead);
    }
#endif


    if (prJfifInst->fgPrsResult)   //parse success
    {
        if (prJfifInst->u4BitStmSrtOfst != 0)
        {
            //copy jfif data into picture cache
            #if JDEC_CACHE_ENABLE
            vJdecCopyJfif2PicCache(prJdecInst);
            #endif
            if (i4JdecUpdateImgdmaRdPoint(prJdecInst) == JDEC_DRV_RTN_FAIL)
            {
                prJdecInst->i4JdecErrorType = -E_JPG_ERR_PASS_END;
                return;
            }
            if(fgJdecCheckHdr(prJdecInst))
            {
            //modify state
            prJdecInst->eJdecState = JDEC_STATE_INIT_HAL;
        }
        else
        {
            	prJdecInst->i4JdecErrorType = -E_JPG_ERR_PASS_END;
            }
        }
        else
        {
            ASSERT(0);
        }
    }
#if SUPPORT_VDEC_PATH
    else if (!prJdecInst->fgVdecPath)
#else
    else    //parse fail
#endif
    {
        if (-E_JPG_ERR_SKIP_DATA == i4Ret)
        {
            if (i4JdecUpdateCurrentFileOfst(prJdecInst, prJfifInst->u4JumpData) == JDEC_DRV_RTN_FAIL)
            {
                prJdecInst->i4JdecErrorType = -E_JPG_ERR_PASS_END;
                return;
            }
            i4JdecStartTranData(prJdecInst, prFifoInfo->u4FileOfst, (prJdecInst->u4ImgSize - prFifoInfo->u4FileOfst));
        }
        else if (-E_JPG_ERR_PASS_END == i4Ret)
        {
            prJdecInst->fgParsingHeaderWaitData = TRUE;
            prJdecInst->u4NeedData = prJfifInst->u4NeedData;
            //update current read point
            if (i4JdecUpdateImgdmaRdPoint(prJdecInst) == JDEC_DRV_RTN_FAIL)
            {
                prJdecInst->i4JdecErrorType = -E_JPG_ERR_HAL_UPT_RD;
                return;
            }
        }
        else
        {
            prJdecInst->i4JdecErrorType = i4Ret;
        }
    }
    return;
}


void vJdecStateInitHal(JDEC_INST_T *prJdecInst)
{
    JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;
    JDEC_HAL_PIC_INFO_T rJdecHalPicInfo;
    JDEC_HAL_INPUT_INFO_T rInputInfo;
    JDEC_HAL_OUTPUT_INFO_T rOutputInfo;
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    JDEC_HAL_OUTPUT_INFO_T rOutputInfo1;
#endif
    INT32 i4Ret = JDEC_DRV_RTN_OK;

    if(!(prJdecInst->fgStartDecodeNow))
    {
       x_thread_delay(10);
       return;
    }
    vJdecLog_L("fgStartDecodeNow\n");
    //check fifo status, enough?
    #if 0
    if (i4JdecChkFifoStatus(prJdecInst) != JDEC_DRV_RTN_OK)
    {
        return;
    }
    #endif
/*
    #if JDEC_USE_MAP_UNMAP
    vJdecMapPrintf("[JDEC]Map from 0x%x to 0x%x , point 10 \n",(UINT32)prJdecInst->hJfifInst, 
                ((UINT32)prJdecInst->hJfifInst + sizeof(JPEG_JFIF_HEADER_T)/JDEC_CACHE_LINE_ALIGN * JDEC_CACHE_LINE_ALIGN));
    BSP_dma_map_vaddr((UINT32)prJdecInst->hJfifInst,sizeof(JPEG_JFIF_HEADER_T)/JDEC_CACHE_LINE_ALIGN * JDEC_CACHE_LINE_ALIGN, TO_DEVICE);
    
    #else
    BSP_CleanDCacheRange((UINT32)prJdecInst->hJfifInst, sizeof(JPEG_JFIF_HEADER_T));
    #endif
*/
    i4JDEC_HAL_Init(prJdecInst->u4HwInstId, prJdecInst->u4HwId);
    prJdecInst->fgInitHal = TRUE;

   vJdecLog_L("prJfifInst->rSOF.eJpegFormat =0x%x\n",prJfifInst->rSOF.eJpegFormat);
    //check hw status, select decode mode
    switch (prJfifInst->rSOF.eJpegFormat)
    {
        case E_JPG_BASELINE:
        case E_JPG_EXTENDED_SEQUENTIAL_HUFFMAN:
#if JDEC_SUPPORT_BROKEN_TARGET
            if (prJdecInst->fgBrokenTarget)
            {
                if (i4JDEC_HAL_Set_Decoding_Mode(prJdecInst->u4HwInstId, JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW) == S_JDEC_HAL_OK)
                {
                    prJdecInst->eJdecDecMode = JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW;
                }
                else
                {
                    ASSERT(0);
                }
            }
            else
#endif
            if ((i4JDEC_HAL_Set_Decoding_Mode(prJdecInst->u4HwInstId, JDEC_HAL_DEC_MODE_BASELINE_PIC) == S_JDEC_HAL_OK)
				)//&&(prJdecInst->eDecodeFlag != IMG_JPG_DECODE_SAME_PIC))
            {
                prJdecInst->eJdecDecMode = JDEC_HAL_DEC_MODE_BASELINE_PIC;

                if ((prJfifInst->rSOF.u2ImageWidth == 1) || (prJfifInst->rSOF.u2ImageHeight == 1))
                {
                    /*For image resizer can't work with picture mode when image widht == 1 or height == 1*/
                    i4JDEC_HAL_Set_Decoding_Mode(prJdecInst->u4HwInstId,JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW);
                    prJdecInst->eJdecDecMode = JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW;
                }

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)

                if ((prJdecInst->pvImgBuf != NULL) &&
                    ((prJdecInst->u4DstCropWidth == 4096) || (prJdecInst->u4DstCropHeight == 4096)))
                {
                    /* Only for MT8520, for BDJ disc, the jpg can output to 4096 at most, but the image
                       resizer can't support width == 4096 or height ==  4096 for output in jpg picture
                       mode ,so if meet this condition, switch to mcu row mode */
                       i4JDEC_HAL_Set_Decoding_Mode(prJdecInst->u4HwInstId,JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW);
                       prJdecInst->eJdecDecMode = JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW;
                }

                if (prJfifInst->u2RestartInterval == 1)
                {
                    // Only for MT8520, restart interval == 1 will cause decode error
                    // Modify by pingzhao, 2008/7/23
                    i4JDEC_HAL_Set_Decoding_Mode(prJdecInst->u4HwInstId, JDEC_HAL_DEC_MODE_BASELINE_MCU);
                    prJdecInst->eJdecDecMode = JDEC_HAL_DEC_MODE_BASELINE_MCU;
                }
#endif

            }
            else if (i4JDEC_HAL_Set_Decoding_Mode(prJdecInst->u4HwInstId, JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW) == S_JDEC_HAL_OK)
            {
                prJdecInst->eJdecDecMode = JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW;
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
                // Only for MT8520, restart interval == 1 will cause decode error
                // Modify by pingzhao, 2008/6/11
                if (prJfifInst->u2RestartInterval == 1)
                {
                    i4JDEC_HAL_Set_Decoding_Mode(prJdecInst->u4HwInstId, JDEC_HAL_DEC_MODE_BASELINE_MCU);
                    prJdecInst->eJdecDecMode = JDEC_HAL_DEC_MODE_BASELINE_MCU;
                }
#endif
            }
            else if (i4JDEC_HAL_Set_Decoding_Mode(prJdecInst->u4HwInstId, JDEC_HAL_DEC_MODE_BASELINE_MCU) == S_JDEC_HAL_OK)
            {
                prJdecInst->eJdecDecMode = JDEC_HAL_DEC_MODE_BASELINE_MCU;
            }
            else
            {
                prJdecInst->i4JdecErrorType = -E_JPG_ERR_UNSUPPORT_SOF;
                return;
            }
            break;
        case E_JPG_PROGRESSIVE_HUFFMAN:
            if ((/*((prJfifInst->rSOF.u2ImageWidth * prJfifInst->rSOF.u2ImageHeight) < JDEC_PROGRESSIVE_MULTI_COLLECT_LIMIT) &&*/
                  (prJdecInst->eQuality == IMG_QUALITY_FACTOR_NORMAL)) ||
                (prJdecInst->pvImgBuf != NULL)) // BDJ will default use multi collect mode
            {
                // BDJ , normal quality + size < 1920*1080 progressive jpg use multi collect mode
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
#if JDEC_SUPPORT_BROKEN_TARGET
                if (prJdecInst->fgBrokenTarget)
                {
                    if (i4JDEC_HAL_Set_Decoding_Mode(prJdecInst->u4HwInstId, JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) == S_JDEC_HAL_OK)
                    {
                        prJdecInst->eJdecDecMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT;
                    }
                    else
                    {
                        ASSERT(0);
                    }
                }
                else
#endif
                if (i4JDEC_HAL_Set_Decoding_Mode(prJdecInst->u4HwInstId, JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT) == S_JDEC_HAL_OK)
                {
                    prJdecInst->eJdecDecMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT;
                }
                else
                {
                    ASSERT(0);
                }
#else
                if (i4JDEC_HAL_Set_Decoding_Mode(prJdecInst->u4HwInstId, JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) == S_JDEC_HAL_OK)
                {
                    prJdecInst->eJdecDecMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT;
                }
                else
                {
                    ASSERT(0);
                }
#endif
            }
            else
            {
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
                if (i4JDEC_HAL_Set_Decoding_Mode(prJdecInst->u4HwInstId, JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE) == S_JDEC_HAL_OK)
                {
                    prJdecInst->eJdecDecMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE;
                }
                else
                {
                    ASSERT(0);
                }
#else
                if (i4JDEC_HAL_Set_Decoding_Mode(prJdecInst->u4HwInstId, JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE) == S_JDEC_HAL_OK)
                {
                    prJdecInst->eJdecDecMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE;
                }
                else
                {
                    ASSERT(0);
                }
#endif
                if (prJdecInst->eQuality == IMG_QUALITY_FACTOR_LOW)
                {
                    prJdecInst->fgDecodeFirstScanOnly = TRUE;
                    vJdecLog_L("[Jdec] Decode one scan only \n");
                }
            }
            break;
        default:
            prJdecInst->i4JdecErrorType = -E_JPG_ERR_UNSUPPORT_SOF;
            return;
    }

#if 0//(CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)||
        (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
    {
        prJdecInst->u4LastScanSA = prJdecInst->rFifoInfo.u4RdPoint;
        //UTIL_Printf("[JDEC_DRV] u4LastScanSA = %x \n",prJdecInst->u4LastScanSA);
    }
#endif

    rJdecHalPicInfo.prSofInfo = &(prJfifInst->rSOF);
    rJdecHalPicInfo.prQTblInfo = &(prJfifInst->rDQT);
    rJdecHalPicInfo.prHuffTblInfo = &(prJfifInst->rDHT);
    rJdecHalPicInfo.u4RestartInterval = prJfifInst->u2RestartInterval;
    i4Ret = i4JDEC_HAL_Set_Pic_Info(prJdecInst->u4HwInstId, &rJdecHalPicInfo);
    if (i4Ret != JDEC_DRV_RTN_OK)
    {
        prJdecInst->i4JdecErrorType = -E_JPG_ERR_NO_MEMO;
        return;
    }

    //calculate color buffer info and Max factor
    if (i4JdecCaluColorBuf(prJdecInst) != JDEC_DRV_RTN_OK)
    {
        return;
    }

    //Init data before set Scan Info
    i4JdecPerScanSetup(prJdecInst);

#if JDEC_SUPPORT_BROKEN_TARGET
    if (prJdecInst->fgBrokenTarget)
    {
        if (prJdecInst->rBlkInfo.u4BlkSize < prJdecInst->u4McuRowLength)
        {
            prJdecInst->fgBrokenRow = TRUE;
        }
    }
#endif
    //init scale function
    i4JdecScaleInit(prJdecInst);
    i4JdecScaleSetJpegInfo(prJdecInst);

    //init hw and default setting
    i4JDEC_HAL_Set_Scan_Info(prJdecInst->u4HwInstId, &(prJfifInst->rSOS));

    rInputInfo.u4RdPoint = prJdecInst->rFifoInfo.u4RdPoint;
    if ((prJdecInst->rFifoInfo.fgEoRange)
#if !JDEC_DISCONTINUOUS_SOURCE
    #if !CONFIG_ENABLE_IOMMU
        && (prJdecInst->pvImgBuf == NULL)
     #else
         &&!prJdecInst->fgJdecKernImgBuf
#endif
#endif
        )
    {
        rInputInfo.u4WrPoint = prJdecInst->rFifoInfo.u4WrPoint + 256;
    }
    else
    {
        rInputInfo.u4WrPoint = prJdecInst->rFifoInfo.u4WrPoint;
    }
    rInputInfo.u4InFifoSa = prJdecInst->rFifoInfo.u4FifoStart;      //need 64bytes alignment
    rInputInfo.u4InFifoSz = prJdecInst->rFifoInfo.u4FifoEnd - prJdecInst->rFifoInfo.u4FifoStart;
    rInputInfo.u4InFifoSz = (rInputInfo.u4InFifoSz + 63) / 64 * 64; //need 64bytes alignment
    // Add by pingzhao for adjust the fifo end address 2008/7/17
    prJdecInst->rFifoInfo.u4FifoEnd = prJdecInst->rFifoInfo.u4FifoStart + rInputInfo.u4InFifoSz;

    i4JDEC_HAL_Set_Input_Info(prJdecInst->u4HwInstId, &rInputInfo);

    prJdecInst->eJdecState = JDEC_STATE_DECODING;
    prJdecInst->eJdecDecState = JDEC_DECODE_DO_DECODE;

    //calculate the Output buffer information
    i4JdecCaluOutputBuf(prJdecInst, &rOutputInfo);
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    rOutputInfo1.u4OutYBufSa = prJdecInst->au4ColorBufSa[1][0];
    rOutputInfo1.u4OutCbBufSa = prJdecInst->au4ColorBufSa[1][1];
    rOutputInfo1.u4OutCrBufSa = prJdecInst->au4ColorBufSa[1][2];
    i4JDEC_HAL_Set_Output_Info(prJdecInst->u4HwInstId, &rOutputInfo, &rOutputInfo1);
    #else
    i4JDEC_HAL_Set_Output_Info(prJdecInst->u4HwInstId, &rOutputInfo);
    #endif

    //set timer for sleep
    if (prJdecInst->eJdecDecMode != JDEC_HAL_DEC_MODE_BASELINE_PIC)
    {
        x_timer_start(prJdecInst->hJdecSleepTimer, 500, X_TIMER_FLAG_ONCE, (x_os_timer_cb_fct)vJdecSleepTimerCb, (void*)((UINT32)prJdecInst->u2CompId));
    }

    prJdecInst->eJdecState = JDEC_STATE_DECODING;
    vJdecLog_L("[Jdec] Decode mode=0x%x\n",prJdecInst->eJdecDecMode );
}

#if PERFORMANCE_TEST
extern HAL_TIME_T _rJdecStratTime, _rJdecEndTime, _rJdecTotalTime;
#endif


void vJdecDecode(JDEC_INST_T *prJdecInst)
{
    EV_GRP_EVENT_T  eJdecEvent;
    EV_GRP_EVENT_T  eJdecEventGet;
    INT32 i4Ret;


    switch(prJdecInst->eJdecState)
    {
        case JDEC_STATE_START:
            vJdecLog_M("[Jdec] JDEC_STATE_START \n");
            vJdecStateStart(prJdecInst);
            vJdecLog_M("[Jdec] JDEC_STATE_PRS_HDR \n");
            break;
        case JDEC_STATE_PRS_HDR:
            vJdecStatePrsHdr(prJdecInst);
            break;
        case JDEC_STATE_INIT_HAL:
            vJdecLog_M("[Jdec] JDEC_STATE_INIT_HAL \n");
            vJdecStateInitHal(prJdecInst);
            vJdecLog_M("[Jdec] JDEC_STATE_DECODING \n");
            vJdecLog_H("[Jdec] Decoding mode = %d\n", prJdecInst->eJdecDecMode);
            #if PERFORMANCE_TEST
            if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) ||
                (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE) ||
                (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW))
            {
                HAL_GetTime(&_rJdecStratTime);
            }
            #endif
            break;
        case JDEC_STATE_DECODING:
            eJdecEvent = JDEC_DRV_EV_SLEEP;
            if ((prJdecInst->eJdecDecMode != JDEC_HAL_DEC_MODE_BASELINE_PIC)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
                && (prJdecInst->eJdecDecMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
                && (prJdecInst->eJdecDecMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif
#if JDEC_SUPPORT_BROKEN_TARGET
                && (prJdecInst->pvImgBuf == NULL)
#endif
                )
            {
                i4Ret = x_ev_group_wait_event_timeout(prJdecInst->hJdecEvent, eJdecEvent, &eJdecEventGet, X_EV_OP_OR_CONSUME, 0);
                if (i4Ret == OSR_OK)
                {
                    x_thread_delay(50);
                    x_timer_start(prJdecInst->hJdecSleepTimer, 500, X_TIMER_FLAG_ONCE, (x_os_timer_cb_fct)vJdecSleepTimerCb, (void*)((UINT32)prJdecInst->u2CompId));
                }
            }
            vJdecStateDecode(prJdecInst);
            break;
        case JDEC_STATE_DEC_FINISH:

            vJdecLog_M("[Jdec] JDEC_STATE_DEC_FINISH %d\n", __LINE__);
            //UTIL_Printf("[JDEC_DRV] JDEC_STATE_DEC_FINISH, total %d scan \n",prJdecInst->u4ScanCount);
            #if PERFORMANCE_TEST
            if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT) ||
                (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE) ||
                (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
                || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
                || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
                || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
                || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif
                )
            {
                HAL_GetTime(&_rJdecEndTime);
                if (_rJdecEndTime.u4Micros < _rJdecStratTime.u4Micros)
                {
                    _rJdecTotalTime.u4Micros = 1000000 + _rJdecEndTime.u4Micros - _rJdecStratTime.u4Micros;
                    _rJdecEndTime.u4Seconds -= 1;
                }
                else
                {
                    _rJdecTotalTime.u4Micros = _rJdecEndTime.u4Micros - _rJdecStratTime.u4Micros;
                }
                _rJdecTotalTime.u4Seconds = _rJdecEndTime.u4Seconds - _rJdecStratTime.u4Seconds;
            }
            vJdecLog_H("[Jdec] Decode time = %ds %dms \n",_rJdecTotalTime.u4Seconds, _rJdecTotalTime.u4Micros);
            #endif
            if (prJdecInst->eDstColorMode == GFX_COLORMODE_ARGB_D8888)
            {
                vJdecSetLog(JDEC_LOG_YUV_2_RGB1);
#if JDEC_SUPPORT_BROKEN_TARGET
                if (prJdecInst->fgBrokenTarget)
                    i4JdecScaleSlicedYCbCr2RGB(prJdecInst);
                else
#endif
                i4JdecScaleYCbCr2RGB(prJdecInst);
                vJdecSetLog(JDEC_LOG_YUV_2_RGB2);
                #if JDEC_USE_MAP_UNMAP
                vJdecMapPrintf("[JDEC] Unmap from 0x%x to 0x%x point 9\n",(UINT32)prJdecInst->pvDstYBuf, 
                            ((UINT32)prJdecInst->pvDstYBuf + prJdecInst->u4DstBufPitch * (prJdecInst->u4DstCropPosY + prJdecInst->u4DstCropHeight)));
                BSP_dma_unmap_vaddr((UINT32)prJdecInst->pvDstYBuf, prJdecInst->u4DstBufPitch * (prJdecInst->u4DstCropPosY + prJdecInst->u4DstCropHeight), FROM_DEVICE);
                #endif
            }
            vJdecAbort(prJdecInst, IMG_NFY_FINISHED, TRUE);
            break;
        case JDEC_STATE_IDLE:
            break;
        default:
            break;
    }

    if (prJdecInst->i4JdecErrorType < 0)
    {
        vJdecAbort(prJdecInst, IMG_NFY_ERROR, TRUE);
        if ((prJdecInst->rImgReszTicket.u4Ticket != JDEC_INVALID32) && 
            (prJdecInst->eJdecState >= JDEC_STATE_INIT_HAL))
        {
            i4JdecScaleStop(prJdecInst);
        }
    }

}


void vJdecStateSwStart(JDEC_INST_T *prJdecInst)
{
    //init decode val
    vJdecInitDecVal(prJdecInst);

    vInitial_decompress(prJdecInst, (prJdecInst->ptImgJpgObj));
    return;
}

void vJdecStateSwPrsHdr(JDEC_INST_T *prJdecInst)
{
    JDEC_FIFO_INFO_T *prFifoInfo = &(prJdecInst->rFifoInfo);

    //check fifo status, enough?
    if (i4JdecChkFifoStatus(prJdecInst) == JDEC_DRV_RTN_OK)
    {
        if ((prFifoInfo->fgEoRange == TRUE) && (prFifoInfo->u4ValidData < 4))
        {
            prJdecInst->i4JdecErrorType = -E_JPG_ERR_PASS_END;
            vJdecLog_M("[JPG_SW] ERR: E_JPG_ERR_PASS_END()\n\r");
            return;
        }
        i2Consume_markers (prJdecInst, prJdecInst->ptImgJpgObj);
        if (prJdecInst->ptImgJpgObj->e_JPGErrorLevel != JPG_NO_ERR)
        {
            //UTIL_Printf("[IMG] ERR: vJdecStateSwPrsHdr(): i2Consume_markers error \n\r");
            vJdecLog_M("[JPG_SW] ERR: vJdecStateSwPrsHdr(): i2Consume_markers error \n\r");
            prJdecInst->i4JdecErrorType = -E_JPG_SW_PARSER_FAIL;
            return;
        }
        prJdecInst->eJdecState = JDEC_STATE_INIT_HAL;
    }
    else
    {
        vJdecLog_M("[JPG_SW] ERR: FIFO not ready()\n\r");

        return;
    }
}

void vJdecStateSwInitHal(JDEC_INST_T *prJdecInst)
{
    IMG_BASIC_INFO_T*   pt_info = NULL;
    //UINT32              ui4_scale_down_ratio;

    prJdecInst->ptImgJpgObj->pt_current_sel = (IMG_BASIC_INFO_T*) &(prJdecInst->ptImgJpgObj->t_primary);
    pt_info = prJdecInst->ptImgJpgObj->pt_current_sel;

    /************************************************************************/
    /* Perform decode image                                                 */
    /************************************************************************/

    if ((pt_info->ui2_image_height == 0)
        || (pt_info->ui2_image_width == 0))
    {
        prJdecInst->i4JdecErrorType = -E_JPG_SW_UNSUPPORT_SIZE;
        return;
    }

    if (((prJdecInst->u4SrcCropPosX + prJdecInst->u4SrcCropWidth) > pt_info->ui2_image_width) ||
        ((prJdecInst->u4SrcCropPosY + prJdecInst->u4SrcCropHeight) > pt_info->ui2_image_height))
    {
        prJdecInst->i4JdecErrorType = -E_JPG_SW_UNSUPPORT_SIZE;
        return;
    }


    if (pt_info->b_progressive_mode != TRUE)
    {
        /* restore compressed data begin pos*/
        vSet_InputPos(prJdecInst, prJdecInst->ptImgJpgObj, pt_info->ui4_compressed_data_begin_pos);
    }

    /************************************************************************/
    /* begin decode loop                                                    */
    /************************************************************************/
    //[CJYang Resample]
    //prJdecInst->ptImgJpgObj->ppui2_iMCU_row_start_addr = (INT16**) x_mem_alloc(MAX_COMPS_IN_SCAN * pt_info->ui4_TotalIMCURows * sizeof(INT16*));
    //prJdecInst->ptImgJpgObj->ppui2_iMCU_row_start_addr = (INT16**) x_mem_alloc(MAX_COMPS_IN_SCAN * prJdecInst->u4MCURowNum * sizeof(INT16*));
#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
	     prJdecInst->ptImgJpgObj->ppui2_iMCU_row_start_addr = (INT16**) x_alloc_aligned_dma_mem(MAX_COMPS_IN_SCAN * prJdecInst->u4MCURowNum * sizeof(INT16*), 32);
#elif CONFIG_SYS_MEM_PHASE3
	     prJdecInst->ptImgJpgObj->ppui2_iMCU_row_start_addr = (INT16**) x_mem_aligned_alloc(MAX_COMPS_IN_SCAN * prJdecInst->u4MCURowNum * sizeof(INT16*), 32);
#endif
#else
            prJdecInst->ptImgJpgObj->ppui2_iMCU_row_start_addr = (INT16**) x_alloc_aligned_nc_mem(MAX_COMPS_IN_SCAN * prJdecInst->u4MCURowNum * sizeof(INT16*), 32);
#endif
    if (prJdecInst->ptImgJpgObj->ppui2_iMCU_row_start_addr == NULL)
    {
        prJdecInst->ptImgJpgObj->e_JPGErrorLevel = JPG_FATAL_ERR;
        return;
    }

    prJdecInst->ptImgJpgObj->ui2_num_of_resampling = 0;
    /* initialize global variables */
    prJdecInst->ptImgJpgObj->ui2_output_image_lines = 0;

    pt_info->ui2_p_scan_decode_num = 0;
    pt_info->b_last_p_scan_decode = FALSE;

    //ui4_iTotalIMCURows = pt_info->ui4_TotalIMCURows;
    //pui4_iOutputIMCURow = &(prJdecInst->ptImgJpgObj->ui4_OutputIMCURow);
#if 0
    /* calculate_scaledown_level ignore FIT_D2*/
    if ((prJdecInst->u4SrcCropWidth / prJdecInst->u4DstCropWidth) < (prJdecInst->u4SrcCropHeight / prJdecInst->u4DstCropHeight))
    {
        ui4_scale_down_ratio = prJdecInst->u4SrcCropWidth / prJdecInst->u4DstCropWidth;
    }
    else
    {
        ui4_scale_down_ratio = prJdecInst->u4SrcCropHeight / prJdecInst->u4DstCropHeight;
    }
    //ui4_scale_down_ratio = MIN(t_src_dst_rect.ui4_src_w/t_src_dst_rect.ui4_dst_w, t_src_dst_rect.ui4_src_h/t_src_dst_rect.ui4_dst_h);
    if (ui4_scale_down_ratio >= 8)
    {
        pt_info->e_scaledown = FIT_D3;
    }
    else if (ui4_scale_down_ratio >= 4)
    {
        pt_info->e_scaledown = FIT_D1;
    }
    else
#endif
    {
        pt_info->e_scaledown = FIT_D0;
    }

}

void vJdecStateSwDecode(JDEC_INST_T *prJdecInst)
{
    IMG_BASIC_INFO_T*   pt_info = NULL;
    IMG_JPG_OBJ_T *pt_jpg_obj = prJdecInst->ptImgJpgObj;
    SRC_DST_RECT_T      t_src_dst_rect;

    pt_info = prJdecInst->ptImgJpgObj->pt_current_sel;
    /* fill src dst rect*/
    t_src_dst_rect.ui4_src_x = prJdecInst->u4SrcCropPosX;
    t_src_dst_rect.ui4_src_y = prJdecInst->u4SrcCropPosY;
    t_src_dst_rect.ui4_src_w = prJdecInst->u4SrcCropWidth;
    t_src_dst_rect.ui4_src_h = prJdecInst->u4SrcCropHeight;
    t_src_dst_rect.ui4_dst_x = prJdecInst->u4DstCropPosX;
    t_src_dst_rect.ui4_dst_y = prJdecInst->u4DstCropPosY;
    t_src_dst_rect.ui4_dst_w = prJdecInst->u4DstCropWidth;
    t_src_dst_rect.ui4_dst_h = prJdecInst->u4DstCropHeight;
    t_src_dst_rect.b_flip = FALSE;

    if (i4JdecChkFifoStatus(prJdecInst) == JDEC_DRV_RTN_OK)
    {
        while (pt_info->b_last_p_scan_decode != TRUE)
        {
            /* Loop to process a whole picture */
            //*pui4_iOutputIMCURow = 0;
            prJdecInst->u4CurrDecodeMCURow = 0;

            while (prJdecInst->u4CurrDecodeMCURow < prJdecInst->u4MCURowNum)
            {
                pt_jpg_obj->ui4_JPGErrorType = 0;    /* - indicates JPEG decoding error type  */
                pt_jpg_obj->e_JPGErrorLevel = JPG_NO_ERR;    /* - indicates JPEG decoding error level*/


                if (pt_info->b_progressive_mode != FALSE)
                {
                    /* restore compressed data begin pos*/
                    vSet_InputPos(prJdecInst, pt_jpg_obj, pt_info->ui4_compressed_data_begin_pos);
                }

                if ((fgDecode_jpg(prJdecInst, pt_jpg_obj, &t_src_dst_rect)) == FALSE)
                {
                    if (prJdecInst->i4JdecErrorType >= 0)
                    {
                        prJdecInst->i4JdecErrorType = -E_JPG_ERR_DEC_ERROR;
					    UTIL_Printf("[JDEC] %s line %dE_JPG_ERR_DEC_ERROR\n", __FUNCTION__, __LINE__);
                    }
                    return;
                }

                /* update progress status */
                pt_info->ui4_load_progress = (prJdecInst->u4CurrDecodeMCURow * 100) / prJdecInst->u4MCURowNum;/* progress form 0~100*/

                if ((pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
                    || (prJdecInst->u4CurrDecodeMCURow) >= prJdecInst->u4MCURowNum)
                {
                    pt_info->b_last_p_scan_decode = TRUE;
                    break;
                }
            }
            pt_info->ui2_p_scan_decode_num++;
        }    /* pt_jpg_obj->last_p_scan_decode loop */
    }
    else // wait for fifo status OK. by pingzhao @2008/11/19
    {
        return;
    }
    if (prJdecInst->ptImgJpgObj->e_JPGErrorLevel != JPG_NO_ERR)
    {
        UTIL_Printf("[JDEC] %s line %dE_JPG_ERR_DEC_ERROR\n", __FUNCTION__, __LINE__);
        prJdecInst->i4JdecErrorType = -E_JPG_ERR_DEC_ERROR;
        return;
    }
    prJdecInst->eJdecState = JDEC_STATE_DEC_FINISH;
}

void vJdecStateSwDecFinish(JDEC_INST_T *prJdecInst)
{
    IMG_JPG_OBJ_T *pt_jpg_obj = prJdecInst->ptImgJpgObj;

    //[CJYang_ColorBuf]
#if 0
    if (pt_jpg_obj->aapui1_color_buf[0][0])/* free color_base */
    {
        x_mem_free(pt_jpg_obj->aapui1_color_buf[0][0]);
        pt_jpg_obj->aapui1_color_buf[0][0] = NULL;
    }
#endif
    if (pt_jpg_obj->ppui2_iMCU_row_start_addr)
    {
#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
        x_free_aligned_dma_mem(pt_jpg_obj->ppui2_iMCU_row_start_addr);
#elif CONFIG_SYS_MEM_PHASE3
        x_mem_aligned_free(pt_jpg_obj->ppui2_iMCU_row_start_addr);
#endif
#else
        x_free_aligned_nc_mem(pt_jpg_obj->ppui2_iMCU_row_start_addr);
#endif
        //x_mem_free(pt_jpg_obj->ppui2_iMCU_row_start_addr);
        pt_jpg_obj->ppui2_iMCU_row_start_addr = NULL;
    }
    if (prJdecInst->au4ColorBufSa[0][0] != JDEC_INVALID32)
    {
#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
        x_free_aligned_dma_mem((void*)(prJdecInst->au4ColorBufSa[0][0]));
#elif CONFIG_SYS_MEM_PHASE3
        x_mem_aligned_free((void*)(prJdecInst->au4ColorBufSa[0][0]));
#endif
#else
        x_free_aligned_nc_mem((void*)(prJdecInst->au4ColorBufSa[0][0]));
#endif
        prJdecInst->au4ColorBufSa[0][0] = JDEC_INVALID32;
        prJdecInst->au4ColorBufSa[0][1] = JDEC_INVALID32;
        prJdecInst->au4ColorBufSa[0][2] = JDEC_INVALID32;
        prJdecInst->au4ColorBufSa[1][0] = JDEC_INVALID32;
        prJdecInst->au4ColorBufSa[1][1] = JDEC_INVALID32;
        prJdecInst->au4ColorBufSa[1][2] = JDEC_INVALID32;
    }
}

void vJdecSwDecode(JDEC_INST_T *prJdecInst)
{
    EV_GRP_EVENT_T  eJdecEvent;
    EV_GRP_EVENT_T  eJdecEventGet;
    INT32 i4Ret;


    switch(prJdecInst->eJdecState)
    {
        case JDEC_STATE_START:
            vJdecLog_H("[Jdec_SW] JDEC_STATE_START \n");
            vJdecStateSwStart(prJdecInst);
            vJdecStateStart(prJdecInst);

            prJdecInst->eJdecState = JDEC_STATE_PRS_HDR;
            vJdecLog_H("[Jdec_SW] JDEC_STATE_PRS_HDR \n");
            break;
        case JDEC_STATE_PRS_HDR:
            //vJdecStatePrsHdr(prJdecInst);
            vJdecStateSwPrsHdr(prJdecInst);
            break;
        case JDEC_STATE_INIT_HAL:
            //vJdecStateInitHal(prJdecInst);
            vJdecLog_H("[Jdec_SW] JDEC_STATE_INIT_HAL \n");
            vJdecStateSwInitHal(prJdecInst);
            prJdecInst->eJdecDecMode = JDEC_HAL_DEC_MODE_BASELINE_MCU;
            prJdecInst->eJdecState = JDEC_STATE_DECODING;
            vJdecLog_H("[Jdec_SW] JDEC_STATE_DECODING \n");
            break;
        case JDEC_STATE_DECODING:
            eJdecEvent = JDEC_DRV_EV_SLEEP | JDEC_DRV_EV_STOP;
            i4Ret = x_ev_group_wait_event_timeout(prJdecInst->hJdecEvent, eJdecEvent, &eJdecEventGet, X_EV_OP_OR_CONSUME, 0);
            if (i4Ret == OSR_OK)
            {
                if (eJdecEventGet & JDEC_DRV_EV_SLEEP)
                {
                    x_thread_delay(50);
                    x_timer_start(prJdecInst->hJdecSleepTimer, 500, X_TIMER_FLAG_ONCE, (x_os_timer_cb_fct)vJdecSleepTimerCb, (void*)((UINT32)prJdecInst->u2CompId));
                }
                else
                {
                    vJdecStateSwDecFinish(prJdecInst);
                    vJdecAbort(prJdecInst, IMG_NFY_FINISHED, FALSE);
                    return;
                }
            }
            vJdecStateSwDecode(prJdecInst);
            break;
        case JDEC_STATE_DEC_FINISH:
            vJdecLog_H("[Jdec_SW] JDEC_STATE_DEC_FINISH \n");
            if (prJdecInst->eDstColorMode == GFX_COLORMODE_ARGB_D8888)
            {
                i4JdecScaleYCbCr2RGB(prJdecInst);
            }
            prJdecInst->u4DecProgress = 100;
            vJdecStateSwDecFinish(prJdecInst);
            vJdecAbort(prJdecInst, IMG_NFY_FINISHED, TRUE);
            break;
        case JDEC_STATE_IDLE:
            break;
        default:
            break;
    }

    if (prJdecInst->i4JdecErrorType < 0)
    {
        vJdecAbort(prJdecInst, IMG_NFY_ERROR, TRUE);
    }
}

