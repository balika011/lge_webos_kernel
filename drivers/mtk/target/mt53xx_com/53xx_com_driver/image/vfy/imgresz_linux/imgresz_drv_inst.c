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
#include "x_rtos.h"
#include "x_printf.h"
#include "x_debug.h"
#include "drv_imgresz_errcode.h"
#include "imgresz_drv.h"
#include "imgresz_drv_inst.h"
#include "drv_config.h"
#include "x_kmem.h"

#include "sys_config.h"

#if CONFIG_SYS_MEM_PHASE2
#include "x_mem_phase2.h"
#elif CONFIG_SYS_MEM_PHASE3
#include "x_kmem.h"
#endif

/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
HANDLE_T            h_imgresz_sema;
UINT32              _u4ImgreszScaleCount = 0;
UINT32              _u4ImgreszUnserviceCount = 0;
IMGRESZ_INST_T      _arImgreszInst[IMGRESZ_INST_NUM];

#define IMGRESZ_INST_LOG_NUM 256
UINT32 _u4ImgreszInstLogIdx = 0;
UINT32 _au4ImgreszInstLog[IMGRESZ_INST_LOG_NUM];


void vImgReszInstLog(UINT32 u4Event,UINT32 u4Value)
{
    _au4ImgreszInstLog[_u4ImgreszInstLogIdx] = (u4Event<<16) | u4Value;

    _u4ImgreszInstLogIdx++;
    if(_u4ImgreszInstLogIdx == IMGRESZ_INST_LOG_NUM)
        _u4ImgreszInstLogIdx = 0;
}


void vImgResz_Inst_Init(void)
{
    INT32 i4,i4Ret;

    vImgreszSetMemory(_arImgreszInst,0, sizeof(_arImgreszInst));

    for (i4=0; i4<IMGRESZ_INST_NUM; i4++)
    {
        _arImgreszInst[i4].u2ImgReszCompId = (UINT16)i4;
        _arImgreszInst[i4].eImgReszScaleMd = IMGRESZ_DRV_NONE_SCALE;
        _arImgreszInst[i4].eImgReszState = IMGRESZ_DRV_STATE_NONE;
        _arImgreszInst[i4].u4ImgReszPriority = 0;
        //_arImgreszInst[i4].u4ImgReszComponents = IMGRESZ_INVALID32;
        //_arImgreszInst[i4].u4ImgReszHwHandle = IMGRESZ_INVALID32;
#if IMGRESZ_MEMORY_DYNAMIC_ALLOC
        _arImgreszInst[i4].ptImgReszSrcBufInfo = x_mem_valloc(sizeof(IMGRESZ_DRV_SRC_BUF_INFO_T));
        if (_arImgreszInst[i4].ptImgReszSrcBufInfo == NULL)
        {
            UTIL_Printf("[IMGRESZ] Allocate source buffer info fail \n");
            ASSERT(0);
        }
        x_memset(_arImgreszInst[i4].ptImgReszSrcBufInfo, 0, sizeof(IMGRESZ_DRV_SRC_BUF_INFO_T));
        
        
        _arImgreszInst[i4].ptImgReszDstBufInfo = x_mem_valloc(sizeof(IMGRESZ_DRV_DST_BUF_INFO_T));
        if (_arImgreszInst[i4].ptImgReszDstBufInfo == NULL)
        {
            UTIL_Printf("[IMGRESZ] Allocate destination buffer info fail \n");
            ASSERT(0);
        }
        x_memset(_arImgreszInst[i4].ptImgReszDstBufInfo, 0, sizeof(IMGRESZ_DRV_DST_BUF_INFO_T));
        
        
        _arImgreszInst[i4].ptImgReszBldBufInfo = x_mem_valloc(sizeof(IMGRESZ_DRV_BLD_BUF_INFO_T));
        if (_arImgreszInst[i4].ptImgReszBldBufInfo == NULL)
        {
            UTIL_Printf("[IMGRESZ] Allocate blending buffer info fail\n");
            ASSERT(0);
        }
        x_memset(_arImgreszInst[i4].ptImgReszBldBufInfo, 0, sizeof(IMGRESZ_DRV_BLD_BUF_INFO_T));
        
        
        _arImgreszInst[i4].ptImgReszJpegInfo = x_mem_valloc(sizeof(IMGRESZ_DRV_JPEG_INFO_T));
        if (_arImgreszInst[i4].ptImgReszJpegInfo == NULL)
        {
            UTIL_Printf("[IMGRESZ] Allocate jpg info fail \n");
            ASSERT(0);          
        }
        x_memset(_arImgreszInst[i4].ptImgReszJpegInfo, 0, sizeof(IMGRESZ_DRV_JPEG_INFO_T));
        
        
        _arImgreszInst[i4].ptImgReszPartialBufInfo = x_mem_valloc(sizeof(IMGRESZ_DRV_PARTIAL_INFO_T));
        if (_arImgreszInst[i4].ptImgReszPartialBufInfo == NULL)
        {
            UTIL_Printf("[IMGRESZ] Allocate partial buffer info fail \n");
            ASSERT(0);
        }
        x_memset(_arImgreszInst[i4].ptImgReszPartialBufInfo, 0, sizeof(IMGRESZ_DRV_PARTIAL_INFO_T));
        
        
        #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        _arImgreszInst[i4].ptImgReszRmInfo = x_mem_valloc(sizeof(IMGRESZ_DRV_RM_INFO_T));
        if (_arImgreszInst[i4].ptImgReszRmInfo == NULL)
        {
            UTIL_Printf("[IMGRESZ] Allocate rm info fail \n");
            ASSERT(0);
        }
        x_memset(_arImgreszInst[i4].ptImgReszRmInfo, 0, sizeof(IMGRESZ_DRV_RM_INFO_T));
        #endif
#endif            
        
    }

    _u4ImgreszScaleCount = 0;
    _u4ImgreszUnserviceCount = 0;

    // create a semaphore
    i4Ret = x_sema_create(&h_imgresz_sema, X_SEMA_TYPE_MUTEX, 1);
    VERIFY(i4Ret == OSR_OK);
}


void vImgResz_Inst_Uninit(void)
{
    INT32 i4Ret;
    INT32 i4 = 0;
    
    // Delete semaphore
    i4Ret = x_sema_delete(h_imgresz_sema);
    VERIFY(i4Ret == OSR_OK); 

            
#if IMGRESZ_MEMORY_DYNAMIC_ALLOC    
        for (i4=0; i4<IMGRESZ_INST_NUM; i4++)
        {
            if (_arImgreszInst[i4].ptImgReszBldBufInfo != NULL)
            {
                x_mem_vfree(_arImgreszInst[i4].ptImgReszBldBufInfo);
                _arImgreszInst[i4].ptImgReszBldBufInfo = NULL;
            }
            if (_arImgreszInst[i4].ptImgReszSrcBufInfo != NULL)
            {
                x_mem_vfree(_arImgreszInst[i4].ptImgReszSrcBufInfo);
                _arImgreszInst[i4].ptImgReszSrcBufInfo = NULL;
            }
            if (_arImgreszInst[i4].ptImgReszDstBufInfo != NULL)
            {
                x_mem_vfree(_arImgreszInst[i4].ptImgReszDstBufInfo);
                _arImgreszInst[i4].ptImgReszDstBufInfo = NULL;
            }
            if (_arImgreszInst[i4].ptImgReszPartialBufInfo != NULL)
            {
                x_mem_vfree(_arImgreszInst[i4].ptImgReszPartialBufInfo);
                _arImgreszInst[i4].ptImgReszPartialBufInfo = NULL;
            }
            if (_arImgreszInst[i4].ptImgReszJpegInfo!= NULL)
            {
                x_mem_vfree(_arImgreszInst[i4].ptImgReszJpegInfo);
                _arImgreszInst[i4].ptImgReszJpegInfo = NULL;
            }
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)    
            if (_arImgreszInst[i4].ptImgReszRmInfo!= NULL)
            {
                x_mem_vfree(_arImgreszInst[i4].ptImgReszRmInfo);
                _arImgreszInst[i4].ptImgReszRmInfo = NULL;
            }
#endif  
        }
#endif    
}


IMGRESZ_DRV_STATE eImgResz_Inst_GetState(UINT32 u4InstId)
{
    return _arImgreszInst[u4InstId].eImgReszState;
}

void vImgResz_Inst_SetState(UINT32 u4InstId, IMGRESZ_DRV_STATE eState)
{
    _arImgreszInst[u4InstId].eImgReszState = eState;
}


INT32 i4ImgResz_Inst_SetLock(UINT32 u4InstId,BOOL fgLock)
{
    INT32 i4,i4Ret;
    BOOL fgDoLock = FALSE;

    if(_arImgreszInst[u4InstId].fgLock == fgLock)
        return 0;

    if(_arImgreszInst[u4InstId].u4ImgReszPriority == 0)
        return -1;

    // Get semaphore
    i4Ret = x_sema_lock(h_imgresz_sema,X_SEMA_OPTION_WAIT);
    VERIFY(i4Ret == OSR_OK);

    vImgReszInstLog(0,u4InstId);
    vImgReszInstLog(1,fgLock);

    if(fgLock)
    {
        if(_arImgreszInst[u4InstId].eImgReszScaleMd == IMGRESZ_DRV_JPEG_PIC_SCALE)
        {
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            UINT32 u4TargetHwId = _arImgreszInst[u4InstId].ptImgReszJpegInfo->u4HwId;
            #else
            UINT32 u4TargetHwId = _arImgreszInst[u4InstId].tImgReszJpegInfo.u4HwId;
            #endif
            INT32 i4Temp = 0;

            fgDoLock = TRUE;

            
            for (i4=0; i4<IMGRESZ_INST_NUM; i4++)
            {
                if(_arImgreszInst[i4].fgLock && (_arImgreszInst[i4].u4HwInstId == u4TargetHwId) &&
                   (_arImgreszInst[u4InstId].u4ImgReszPriority <= _arImgreszInst[i4].u4ImgReszPriority))
                    fgDoLock = FALSE;
                if(_arImgreszInst[i4].fgLock && (_arImgreszInst[i4].u4HwInstId == u4TargetHwId) &&
                   (_arImgreszInst[u4InstId].u4ImgReszPriority > _arImgreszInst[i4].u4ImgReszPriority))
                    i4Temp = i4;
            }

            if(fgDoLock)
            {
                _arImgreszInst[u4InstId].u4HwInstId = u4TargetHwId;
                _arImgreszInst[i4Temp].fgLock = FALSE;
            }
        } 
        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
        else if((_arImgreszInst[u4InstId].ptImgReszSrcBufInfo->fgWTEnable) ||
                (_arImgreszInst[u4InstId].ptImgReszDstBufInfo->fgWTEnable))
        #else
        else if((_arImgreszInst[u4InstId].tImgReszSrcBufInfo.fgWTEnable) ||
                (_arImgreszInst[u4InstId].tImgReszDstBufInfo.fgWTEnable))
        #endif
        {
            #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) 
            UINT32 u4TargetHwId = 0;
            #else
            UINT32 u4TargetHwId = 1;
            #endif

            fgDoLock = TRUE;
            for (i4=0; i4<IMGRESZ_INST_NUM; i4++)
            {
                if(_arImgreszInst[i4].fgLock && (_arImgreszInst[i4].u4HwInstId == u4TargetHwId) &&
                   (_arImgreszInst[u4InstId].u4ImgReszPriority <= _arImgreszInst[i4].u4ImgReszPriority))
                    fgDoLock = FALSE;
            }

            if(fgDoLock)
                _arImgreszInst[u4InstId].u4HwInstId = u4TargetHwId;
        }
		#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
		#if IMGRESZ_MEMORY_DYNAMIC_ALLOC
        else if((_arImgreszInst[u4InstId].ptImgReszSrcBufInfo->eSrcColorMode >= IMGRESZ_DRV_INPUT_COL_MD_AYUV) ||
           (_arImgreszInst[u4InstId].ptImgReszDstBufInfo->eDstColorMode >= IMGRESZ_DRV_OUTPUT_COL_MD_AYUV))
        #else
        else if((_arImgreszInst[u4InstId].tImgReszSrcBufInfo.eSrcColorMode >= IMGRESZ_DRV_INPUT_COL_MD_AYUV) ||
           (_arImgreszInst[u4InstId].tImgReszDstBufInfo.eDstColorMode >= IMGRESZ_DRV_OUTPUT_COL_MD_AYUV))
        #endif
        {
            UINT32 u4TargetHwId = 0;

            fgDoLock = TRUE;
            for (i4 = 0; i4 < IMGRESZ_INST_NUM; i4 ++)
            {
                if(_arImgreszInst[i4].fgLock && (_arImgreszInst[i4].u4HwInstId == u4TargetHwId) &&
                   (_arImgreszInst[u4InstId].u4ImgReszPriority <= _arImgreszInst[i4].u4ImgReszPriority))
                    fgDoLock = FALSE;
            }
            if(fgDoLock)
                _arImgreszInst[u4InstId].u4HwInstId = u4TargetHwId;
        }
        #endif   
        else
        {
            UINT32 u4LockedNum = 0;
    
            // Reach max lock num ?
            for (i4=0; i4<IMGRESZ_INST_NUM; i4++)
            {
                if(_arImgreszInst[i4].fgLock)
                    u4LockedNum++;
            }
    
            if(u4LockedNum==IMGRESZ_MAX_LOCK_HW_INST_NUM)
            {
                for (i4=0; i4<IMGRESZ_INST_NUM; i4++)
                {
                    if(_arImgreszInst[i4].fgLock)
                    {
                        if(_arImgreszInst[u4InstId].u4ImgReszPriority > _arImgreszInst[i4].u4ImgReszPriority)
                        {
                            vImgReszInstLog(2,i4);
    
                            // Break previous lock instance.
                            _arImgreszInst[i4].fgLock = FALSE;
                            fgDoLock = TRUE;
                            break;
                        }
                    }
                }
            } else
            {
                fgDoLock = TRUE;
            }
    
            if(fgDoLock)
            {
                // Decide locked hardware instance ID
                UINT32 u4LockedHwInst = 0;
                UINT32 u4OrderdHwInst = 0;
                UINT32 u4HwInstNum = i4ImgReszHwInstGetNum();
                INT32 i4NonlockAndNonorderdHwInstId = -1;
                INT32 i4NonlockAndNonactiveHwInstId = -1;
                INT32 i4NonlockAndActiveHwInstId = -1;
    
                for (i4=0; i4<IMGRESZ_INST_NUM; i4++)
                {
                    if(_arImgreszInst[i4].fgLock)
                        u4LockedHwInst |= (1 << _arImgreszInst[i4].u4HwInstId);
                    else if(eImgResz_Inst_GetState(i4) == IMGRESZ_DRV_STATE_START)
                        u4OrderdHwInst |= (1 << _arImgreszInst[i4].u4HwInstId);
                }
    
                for(i4=0; i4<u4HwInstNum; i4++)
                {
                    if((u4LockedHwInst & (1 << i4)) == 0)
                    {
                        if(fgImgReszHwInstIsActive(i4))
                        {
                            if(i4NonlockAndActiveHwInstId == -1)
                                i4NonlockAndActiveHwInstId = i4;
                        } else if((u4OrderdHwInst & (1 << i4)) == 0)
                        {
                            if(i4NonlockAndNonorderdHwInstId == -1)
                                i4NonlockAndNonorderdHwInstId = i4;
                        } else
                        {
                            if(i4NonlockAndNonactiveHwInstId == -1)
                                i4NonlockAndNonactiveHwInstId = i4;
                        }
                    }
                }
    
                if(i4NonlockAndNonorderdHwInstId != -1)
                {
                    vImgReszInstLog(3,i4NonlockAndNonorderdHwInstId);
    
                    _arImgreszInst[u4InstId].u4HwInstId = i4NonlockAndNonorderdHwInstId;
                } else if(i4NonlockAndNonactiveHwInstId != -1)
                {
                    vImgReszInstLog(4,i4NonlockAndNonactiveHwInstId);
    
                    _arImgreszInst[u4InstId].u4HwInstId = i4NonlockAndNonactiveHwInstId;
                } else if(i4NonlockAndActiveHwInstId != -1)
                {
                    vImgReszInstLog(5,i4NonlockAndActiveHwInstId);
    
                    _arImgreszInst[u4InstId].u4HwInstId = i4NonlockAndActiveHwInstId;
                } else
                {
                    // Why do not find any hardware instance?
                    VERIFY(FALSE);
                }
            }
        }

        if(fgDoLock)
        {
            _arImgreszInst[u4InstId].fgLock = fgLock;
            i4ImgReszHwInstLockNotify(_arImgreszInst[u4InstId].u4HwInstId,fgLock);

            for (i4=0; i4<IMGRESZ_INST_NUM; i4++)
            {
                if(_arImgreszInst[i4].fgLock && (i4 != u4InstId))
                {
                    if(_arImgreszInst[i4].u4HwInstId == _arImgreszInst[u4InstId].u4HwInstId)
                        VERIFY(FALSE);
                }
            }
        }
    } else
    {
        vImgReszInstLog(2,_arImgreszInst[u4InstId].u4HwInstId);

        // Unlock
        _arImgreszInst[u4InstId].fgLock = fgLock;
        i4ImgReszHwInstLockNotify(_arImgreszInst[u4InstId].u4HwInstId,fgLock);
    }

    // Release semaphore
    i4Ret = x_sema_unlock(h_imgresz_sema);
    VERIFY(i4Ret == OSR_OK);

    if(fgLock && !fgDoLock)
        return E_IMGRESZ_DRV_LOCK_FAIL;
    else
        return 0;
}


void vImgResz_Inst_IncPriority(void)
{
    INT32 i4 = 0;

    for (i4=0; i4<IMGRESZ_INST_NUM; i4++)
    {
        if (eImgResz_Inst_GetState(i4) == IMGRESZ_DRV_STATE_START)
        {
            _arImgreszInst[i4].u4ImgReszCurrPriority++;
        }
    }
}


INT32 i4ImgResz_Inst_GetInstanceObject(UINT32 u4InstId,IMGRESZ_INST_T **pprImgreszInst)
{
    *pprImgreszInst = &(_arImgreszInst[u4InstId]);

    return 0;
}


INT32 i4ImgResz_Inst_GetInstance(UINT32 *pu4InstId)
{
    INT32 i4Ret,i4Count = 0;

    // Get semaphore
    i4Ret = x_sema_lock(h_imgresz_sema,X_SEMA_OPTION_WAIT);
    VERIFY(i4Ret == OSR_OK);

    for (i4Count=0; i4Count<IMGRESZ_INST_NUM; i4Count++)
    {
        if (eImgResz_Inst_GetState(i4Count) == IMGRESZ_DRV_STATE_NONE)
        {
            *pu4InstId = i4Count;
            //vImgreszSetMemory(&(_arImgreszInst[*pu4InstId]),0,sizeof(IMGRESZ_INST_T));
            _arImgreszInst[*pu4InstId].u2ImgReszCompId = (UINT16)*pu4InstId;
            _arImgreszInst[*pu4InstId].eImgReszScaleMd = IMGRESZ_DRV_NONE_SCALE;
            _arImgreszInst[*pu4InstId].eImgReszState = IMGRESZ_DRV_STATE_NONE;
            _arImgreszInst[*pu4InstId].u4ImgReszPriority = 0;
            _arImgreszInst[*pu4InstId].u4ImgReszCurrPriority = 0;
#if IMGRESZ_MEMORY_DYNAMIC_ALLOC            
            _arImgreszInst[*pu4InstId].ptImgReszBldBufInfo->u1Alpha = 0xFF;
            _arImgreszInst[*pu4InstId].ptImgReszSrcBufInfo->fgWTEnable = FALSE;
            _arImgreszInst[*pu4InstId].ptImgReszDstBufInfo->fgWTEnable = FALSE;
#else
            _arImgreszInst[*pu4InstId].tImgReszSrcBufInfo.fgWTEnable = FALSE;
            _arImgreszInst[*pu4InstId].tImgReszDstBufInfo.fgWTEnable = FALSE;
           
            //_arImgreszInst[*pu4InstId].u4ImgReszHwHandle = IMGRESZ_INVALID32;
            _arImgreszInst[*pu4InstId].tImgReszBldBufInfo.u1Alpha = 0xFF;
#endif            
            _arImgreszInst[*pu4InstId].u4PrevRowBufSa1 = 0;
            _arImgreszInst[*pu4InstId].u4PrevRowBufSa2 = 0;
            _arImgreszInst[*pu4InstId].u4PrevRowBufSa3 = 0;            
            _arImgreszInst[*pu4InstId].fgLock = FALSE;
            _arImgreszInst[*pu4InstId].fgDoLock = FALSE;
            _arImgreszInst[*pu4InstId].u4HwInstId = 0;
            _arImgreszInst[*pu4InstId].rNotifyCallbackReg.pvCallBackFunc = NULL;
            _arImgreszInst[*pu4InstId].rNotifyCallbackReg.pvPrivData = NULL;
            _arImgreszInst[*pu4InstId].u4TempLineBufSa = 0;
            _arImgreszInst[*pu4InstId].u4PartialRowCnt =0;
            _arImgreszInst[*pu4InstId].fgLumaKeyEnable = FALSE;
            _arImgreszInst[*pu4InstId].u1LumaKey = 0;
            //_arImgreszInst[*pu4InstId].abColorPallet = {0};
            //x_memset(&(void *)_arImgreszInst[*pu4InstId].abColorPallet, 0, 1024);
            
            _arImgreszInst[*pu4InstId].fg1To1Scale = FALSE;
            _arImgreszInst[*pu4InstId].fgYSrcOnly = FALSE;
            vImgResz_Inst_SetState(i4Count, IMGRESZ_DRV_STATE_IDLE);
            break;
        }
    }

    // Release semaphore
    i4Ret = x_sema_unlock(h_imgresz_sema);
    VERIFY(i4Ret == OSR_OK);

    if (i4Count == IMGRESZ_INST_NUM)
        return -1;
    else
        return 0;
}


INT32 i4ImgResz_Inst_ReleaseInstance(UINT32 u4InstId)
{
    INT32 i4Ret;

    // Get semaphore
    i4Ret = x_sema_lock(h_imgresz_sema,X_SEMA_OPTION_WAIT);
    VERIFY(i4Ret == OSR_OK);

    switch (eImgResz_Inst_GetState(u4InstId))
    {
        case IMGRESZ_DRV_STATE_START:
        case IMGRESZ_DRV_STATE_IDLE:
            vImgResz_Inst_SetState(u4InstId, IMGRESZ_DRV_STATE_NONE);
            break;
        case IMGRESZ_DRV_STATE_SCALING:
            //break current scaling, use event
            UTIL_Printf("[IMGRESZ] Note! Release instance in scaling state \n");
            ASSERT(0);
            //vImgReszSetState(u4Ticket, IMGRESZ_DRV_STATE_NONE);
            break;
        default:
            ASSERT(0);
            break;
    }

    if(_arImgreszInst[u4InstId].u4TempLineBufSa != 0)
    {
        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
        x_free_aligned_dma_mem((void *)_arImgreszInst[u4InstId].u4TempLineBufSa);
#elif CONFIG_SYS_MEM_PHASE3
        x_mem_aligned_free((void *)_arImgreszInst[u4InstId].u4TempLineBufSa);
#endif
        #else
        x_free_aligned_nc_mem((void *)_arImgreszInst[u4InstId].u4TempLineBufSa);
        #endif
        _arImgreszInst[u4InstId].u4TempLineBufSa = 0;
    }
    // Release semaphore
    i4Ret = x_sema_unlock(h_imgresz_sema);
    VERIFY(i4Ret == OSR_OK);

    return 0;
}


void vImgResz_Inst_DispatchHw(UINT32 u4InstId)
{
    INT32 i4Ret = 0;

    // Get semaphore
    i4Ret = x_sema_lock(h_imgresz_sema,X_SEMA_OPTION_WAIT);
    VERIFY(i4Ret == OSR_OK);

    vImgReszInstLog(0x10,u4InstId);

    _u4ImgreszScaleCount++;
    _u4ImgreszUnserviceCount++;
    vImgResz_Inst_SetState(u4InstId, IMGRESZ_DRV_STATE_START);
    if(_arImgreszInst[u4InstId].eImgReszScaleMd == IMGRESZ_DRV_JPEG_PIC_SCALE && !_arImgreszInst[u4InstId].fgLock)
    {
        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
        _arImgreszInst[u4InstId].u4HwInstId = _arImgreszInst[u4InstId].ptImgReszJpegInfo->u4HwId;
        #else
        _arImgreszInst[u4InstId].u4HwInstId = _arImgreszInst[u4InstId].tImgReszJpegInfo.u4HwId;
        #endif
        vImgReszHwInstScaleNotify(TRUE,&(_arImgreszInst[u4InstId].u4HwInstId));
    } 
    // Add for WT
    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
    else if (_arImgreszInst[u4InstId].ptImgReszSrcBufInfo->fgWTEnable || (_arImgreszInst[u4InstId].ptImgReszDstBufInfo->fgWTEnable))
    #else
    else if (_arImgreszInst[u4InstId].tImgReszSrcBufInfo.fgWTEnable || (_arImgreszInst[u4InstId].tImgReszDstBufInfo.fgWTEnable))
    #endif
    {
        #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
        _arImgreszInst[u4InstId].u4HwInstId = 0;
        #else
        _arImgreszInst[u4InstId].u4HwInstId = 1;
        #endif
        vImgReszHwInstScaleNotify(TRUE,&(_arImgreszInst[u4InstId].u4HwInstId));
    }
    else if (_arImgreszInst[u4InstId].fgYSrcOnly)
    {
        _arImgreszInst[u4InstId].u4HwInstId = 1;
        vImgReszHwInstScaleNotify(TRUE,&(_arImgreszInst[u4InstId].u4HwInstId));
    }
    else
    {
        vImgReszHwInstScaleNotify(_arImgreszInst[u4InstId].fgLock,&(_arImgreszInst[u4InstId].u4HwInstId));
    }
	
    // Release semaphore
    i4Ret = x_sema_unlock(h_imgresz_sema);
    VERIFY(i4Ret == OSR_OK);
}


INT32 i4ImgResz_Inst_GetUnservicedInstance(IMGRESZ_HW_INST_T* ptImgReszHwInst)
{
    INT32 i4,i4Ret,i4RetValue = 0;
    INT32 i4ImgReszInstId = -1;
    UINT32 u4PriorityTmp = 0;

    // Get semaphore
    i4Ret = x_sema_lock(h_imgresz_sema,X_SEMA_OPTION_WAIT);
    VERIFY(i4Ret == OSR_OK);

    vImgReszInstLog(0x20,ptImgReszHwInst->u2ImgReszHwId);

    if(_u4ImgreszUnserviceCount>0)
    {
        vImgReszInstLog(0x21,ptImgReszHwInst->fgLock);

        if(ptImgReszHwInst->fgLock)
        {
            if (ptImgReszHwInst->u2ImgReszCompId > (IMGRESZ_INST_NUM - 1))
            {
                ASSERT(0);
            }
            if (eImgResz_Inst_GetState(ptImgReszHwInst->u2ImgReszCompId) == IMGRESZ_DRV_STATE_START)
            {
                vImgReszInstLog(0x22,ptImgReszHwInst->u2ImgReszCompId);

        _u4ImgreszUnserviceCount--;
        ptImgReszHwInst->fgImgReszActive = TRUE;
                vImgResz_Inst_SetState(ptImgReszHwInst->u2ImgReszCompId,IMGRESZ_DRV_STATE_SCALING);
            } else
            {
                vImgReszInstLog(0x23,ptImgReszHwInst->u2ImgReszCompId);

                i4RetValue = -1;
            }
        } else
        {
            if (ptImgReszHwInst->u2ImgReszHwId == 0)
            {
                // skip some instance with WT enalbe
                for (i4=0; i4<IMGRESZ_INST_NUM; i4++)
                {
                    if ((eImgResz_Inst_GetState(i4) == IMGRESZ_DRV_STATE_START) && !_arImgreszInst[i4].fgLock)
                    {
                        if ((u4PriorityTmp < _arImgreszInst[i4].u4ImgReszCurrPriority) 
                            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                            #if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560) 
                                 &&((!_arImgreszInst[i4].ptImgReszSrcBufInfo->fgWTEnable) &&
                                  (!_arImgreszInst[i4].ptImgReszDstBufInfo->fgWTEnable))
                            #else
                                 && (!_arImgreszInst[i4].fgYSrcOnly)
                            #endif
                            )
                            #else
                            #if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560) 
                                 &&((!_arImgreszInst[i4].tImgReszSrcBufInfo.fgWTEnable) 
                                  (!_arImgreszInst[i4].tImgReszDstBufInfo.fgWTEnable))
                            #endif
                            )
                            #endif
                        {
                            if((_arImgreszInst[i4].eImgReszScaleMd != IMGRESZ_DRV_JPEG_PIC_SCALE) ||
                               (_arImgreszInst[i4].u4HwInstId == ptImgReszHwInst->u2ImgReszHwId))
                            {
                                u4PriorityTmp = _arImgreszInst[i4].u4ImgReszCurrPriority;
                                i4ImgReszInstId = i4;
                            }
                        }
                    }
                }
            }
            else if (ptImgReszHwInst->u2ImgReszHwId == 1)
            {
        // Search instance to do scale
        for (i4=0; i4<IMGRESZ_INST_NUM; i4++)
        {
                if ((eImgResz_Inst_GetState(i4) == IMGRESZ_DRV_STATE_START) && !_arImgreszInst[i4].fgLock)
            {
                    if (u4PriorityTmp < _arImgreszInst[i4].u4ImgReszCurrPriority)
                {
                            if (((_arImgreszInst[i4].eImgReszScaleMd != IMGRESZ_DRV_JPEG_PIC_SCALE) ||
                       (_arImgreszInst[i4].u4HwInstId == ptImgReszHwInst->u2ImgReszHwId))
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)                                
                           #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                                && ((_arImgreszInst[i4].ptImgReszSrcBufInfo->eSrcColorMode < IMGRESZ_DRV_INPUT_COL_MD_AYUV) &&
                                    (_arImgreszInst[i4].ptImgReszDstBufInfo->eDstColorMode < IMGRESZ_DRV_OUTPUT_COL_MD_AYUV))
                           #else
                                && ((_arImgreszInst[i4].tImgReszSrcBufInfo.eSrcColorMode < IMGRESZ_DRV_INPUT_COL_MD_AYUV) &&
                                    (_arImgreszInst[i4].tImgReszDstBufInfo.eDstColorMode < IMGRESZ_DRV_OUTPUT_COL_MD_AYUV))
                           #endif
#endif                           
                                    )
                    {
                        u4PriorityTmp = _arImgreszInst[i4].u4ImgReszCurrPriority;
                        i4ImgReszInstId = i4;
                    }
                }
            }
        }
            }

        if (i4ImgReszInstId == -1)
        {
                vImgReszInstLog(0x24,ptImgReszHwInst->u2ImgReszCompId);

                // Do nothing, because the unserviced instance is locked.
                i4RetValue = -1;
        }
        else
        {
                vImgReszInstLog(0x25,i4ImgReszInstId);

                _u4ImgreszUnserviceCount--;
                ptImgReszHwInst->fgImgReszActive = TRUE;
                if (i4ImgReszInstId > (IMGRESZ_INST_NUM - 1))
                {
                    ASSERT(0);
                }
                _arImgreszInst[i4ImgReszInstId].u4ImgReszCurrPriority = _arImgreszInst[i4ImgReszInstId].u4ImgReszPriority;
            ptImgReszHwInst->u2ImgReszCompId = (UINT16)i4ImgReszInstId;
            _arImgreszInst[i4ImgReszInstId].u4HwInstId = (UINT32)ptImgReszHwInst->u2ImgReszHwId;
            vImgResz_Inst_SetState(i4ImgReszInstId,IMGRESZ_DRV_STATE_SCALING);
        }
        }
    } else
    {
        vImgReszInstLog(0x26,ptImgReszHwInst->u2ImgReszHwId);
        i4RetValue = -1;
    }

        // Release semaphore
        i4Ret = x_sema_unlock(h_imgresz_sema);
        VERIFY(i4Ret == OSR_OK);

    return i4RetValue;
}


INT32 i4ImgResz_Inst_SetServicingInstanceToUnserviced(IMGRESZ_HW_INST_T* ptImgReszHwInst)
{
    INT32 i4Ret;
    UINT32 u4InstId = ptImgReszHwInst->u2ImgReszCompId;

    // Get semaphore
    i4Ret = x_sema_lock(h_imgresz_sema,X_SEMA_OPTION_WAIT);
    VERIFY(i4Ret == OSR_OK);

    if (u4InstId > (IMGRESZ_INST_NUM - 1))
    {
        ASSERT(0);
    }
    if(eImgResz_Inst_GetState(u4InstId) == IMGRESZ_DRV_STATE_SCALING)
    {
        _u4ImgreszUnserviceCount++;
        vImgResz_Inst_SetState(u4InstId,IMGRESZ_DRV_STATE_START);
    } else
    {
        VERIFY(FALSE);
    }

    // Release semaphore
    i4Ret = x_sema_unlock(h_imgresz_sema);
    VERIFY(i4Ret == OSR_OK);

    return 0;
}


void vImgResz_Inst_ReleaseServicedInstanceAndNotifyCallback(IMGRESZ_HW_INST_T* ptImgReszHwInst,BOOL fgCheckWaitStop,
                                                            INT32 i4NotifyCallbackState)
{
    INT32 i4Ret = 0;

    // Get semaphore
    i4Ret = x_sema_lock(h_imgresz_sema,X_SEMA_OPTION_WAIT);
    VERIFY(i4Ret == OSR_OK);

    vImgReszInstLog(0x30,ptImgReszHwInst->u2ImgReszHwId);
    vImgReszInstLog(0x31,ptImgReszHwInst->fgLock);
    vImgReszInstLog(0x32,ptImgReszHwInst->u2ImgReszCompId);

    if (ptImgReszHwInst->u2ImgReszCompId > (IMGRESZ_INST_NUM - 1))
    {
        ASSERT(0);
    }
    if(fgCheckWaitStop && (eImgResz_Inst_GetState(ptImgReszHwInst->u2ImgReszCompId) == IMGRESZ_DRV_STATE_WAIT_STOP))
    {
        // When decode finish, check wait lock.
        // If wait lock, then wait lock event.
    } else
    {
    _u4ImgreszScaleCount--;
    ptImgReszHwInst->fgImgReszActive = FALSE;
    //UTIL_Printf("[IMGRESZ] vImgResz_Inst_ReleaseServicedInstanceAndNotifyCallback set inst%d state IDLE \n",ptImgReszHwInst->u2ImgReszCompId);
    vImgResz_Inst_SetState((UINT32)(ptImgReszHwInst->u2ImgReszCompId), IMGRESZ_DRV_STATE_IDLE);

    // Notify callback
    i4ImgResz_Inst_NotifyCallbackProc(ptImgReszHwInst->u2ImgReszCompId,i4NotifyCallbackState);

    if ((_u4ImgreszUnserviceCount > 0) && !ptImgReszHwInst->fgLock)
    {
        //ptImgReszHwInst->fgImgReszActive = TRUE;
        //sent a event to related HW instance
        i4Ret = x_ev_group_set_event(ptImgReszHwInst->hEventHandle, IMGRESZ_EV_DO_SCALE, X_EV_OP_OR);
        VERIFY(i4Ret == OSR_OK);
    }
    }

    // Release semaphore
    i4Ret = x_sema_unlock(h_imgresz_sema);
    VERIFY(i4Ret == OSR_OK);

    return;
}


void vImgResz_Inst_NotifyCallback(IMGRESZ_HW_INST_T* ptImgReszHwInst,INT32 i4NotifyCallbackState)
{
    INT32 i4Ret = 0;

    // Get semaphore
    i4Ret = x_sema_lock(h_imgresz_sema,X_SEMA_OPTION_WAIT);
    VERIFY(i4Ret == OSR_OK);

    vImgReszInstLog(0x60,ptImgReszHwInst->u2ImgReszHwId);
    vImgReszInstLog(0x61,ptImgReszHwInst->fgLock);
    vImgReszInstLog(0x62,ptImgReszHwInst->u2ImgReszCompId);

    // Notify callback
    if (ptImgReszHwInst->u2ImgReszCompId > ((IMGRESZ_INST_NUM - 1)))
    {
        ASSERT(0);
    }
    i4ImgResz_Inst_NotifyCallbackProc(ptImgReszHwInst->u2ImgReszCompId,i4NotifyCallbackState);

    // Release semaphore
    i4Ret = x_sema_unlock(h_imgresz_sema);
    VERIFY(i4Ret == OSR_OK);

    return;
}


INT32 i4ImgResz_Inst_GetLockInstance(IMGRESZ_HW_INST_T* ptImgReszHwInst,BOOL *pfgLock,UINT32 *pu4InstId)
{
    INT32 i4,i4Ret;

    // Get semaphore
    i4Ret = x_sema_lock(h_imgresz_sema,X_SEMA_OPTION_WAIT);
    VERIFY(i4Ret == OSR_OK);

    vImgReszInstLog(0x40,ptImgReszHwInst->u2ImgReszHwId);

    // Search lock instance
    for (i4=0; i4<IMGRESZ_INST_NUM; i4++)
    {
        if(_arImgreszInst[i4].fgLock && (_arImgreszInst[i4].u4HwInstId == ptImgReszHwInst->u2ImgReszHwId))
            break;
    }

    if(i4 == IMGRESZ_INST_NUM) // Get lock instance fail.
    {
        vImgReszInstLog(0x42,ptImgReszHwInst->u2ImgReszHwId);

        *pfgLock = FALSE;
    } else // Lock instance found.
    {
        vImgReszInstLog(0x41,i4);

        *pu4InstId = i4;
        *pfgLock = TRUE;
    }

    // Release semaphore
    i4Ret = x_sema_unlock(h_imgresz_sema);
    VERIFY(i4Ret == OSR_OK);

    return 0;
}


INT32 i4ImgResz_Inst_StopResize(UINT32 u4InstId)
{
    INT32 i4Ret;
    
    // Get semaphore
    i4Ret = x_sema_lock(h_imgresz_sema,X_SEMA_OPTION_WAIT);
    VERIFY(i4Ret == OSR_OK);

    vImgReszInstLog(0x50,u4InstId);
    vImgReszInstLog(0x51,(UINT32)eImgResz_Inst_GetState(u4InstId));
    vImgReszInstLog(0x52,_arImgreszInst[u4InstId].u4HwInstId);

    switch (eImgResz_Inst_GetState(u4InstId))
    {
        case IMGRESZ_DRV_STATE_START:
            vImgResz_Inst_SetState(u4InstId, IMGRESZ_DRV_STATE_IDLE);
            i4ImgResz_Inst_NotifyCallbackProc(u4InstId,S_IMGRESZ_DRV_RESIZE_STOP);
            break;
        case IMGRESZ_DRV_STATE_IDLE:
            i4ImgResz_Inst_NotifyCallbackProc(u4InstId,S_IMGRESZ_DRV_RESIZE_STOP);
            break;
        case IMGRESZ_DRV_STATE_SCALING:
            //UTIL_Printf("[IMGRESZ] Set inst%d to IMGRESZ_DRV_STATE_WAIT_STOP \n",u4InstId);
            vImgResz_Inst_SetState(u4InstId, IMGRESZ_DRV_STATE_WAIT_STOP);
            i4ImgReszHwInstStopNotify(_arImgreszInst[u4InstId].u4HwInstId);
            break;
         case IMGRESZ_DRV_STATE_WAIT_STOP:
         	  break;
        default:
            UTIL_Printf("[RESZ] Stop scale in %d state \n",eImgResz_Inst_GetState(u4InstId));
            ASSERT(0);
            break;
    }

    // Release semaphore
    i4Ret = x_sema_unlock(h_imgresz_sema);
    VERIFY(i4Ret == OSR_OK);

    return S_IMGRESZ_DRV_OK;
}


INT32 i4ImgResz_Inst_NotifyCallbackProc(UINT32 u4InstId,INT32 i4State)
{
    vImgReszInstLog(0x70,u4InstId);
    vImgReszInstLog(0x71,i4State);

    if(_arImgreszInst[u4InstId].rNotifyCallbackReg.pvCallBackFunc != NULL)
        _arImgreszInst[u4InstId].rNotifyCallbackReg.pvCallBackFunc(i4State,
            _arImgreszInst[u4InstId].rNotifyCallbackReg.pvPrivData);

    return 0;
}
