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
#include "jdec_hal_errcode.h"
#include "drv_img_dma.h"
#include "jdec_drv_decode.h"
#include "drv_thread.h"
#include "jdec_drv_scale.h"
//#include "x_rm.h"
#include "jdec_debug.h"
//#include "x_hal_1176.h"
//#include "chip_ver.h"
#include "drv_imgresz.h"
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
#include "drv_vjdm.h"
#endif
//#include "x_cb_agent.h"
#include "drv_config.h"

//added by libpng modification
#include <linux/module.h>
#include "asm/uaccess.h"

//#include "sys_config.h"
//#include "pm.h"
#define _JDEC_DRV_CACHE_BUF_FOR_DEBG 0
#if _JDEC_DRV_CACHE_BUF_FOR_DEBG
void* puJdecDstBuf = NULL;
static UINT32 _u4Pos=0;
#endif
#include "x_kmem.h"
#if CONFIG_SYS_MEM_PHASE2
#include "x_mem_phase2.h"
#elif CONFIG_SYS_MEM_PHASE3
#include "x_kmem.h"
#endif
#include "linux/fcntl.h"
#include "linux/stat.h"
#include "linux/syscalls.h"
/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
JDEC_INST_T         _rJdecInst[JDEC_INST_NUM];
#if JDEC_CACHE_ENABLE
JDEC_PIC_INFO_T     _rJdecPicInfo[JDEC_PIC_INFO_CACHE_NUM];
#endif
static HANDLE_T     _hJdecCacheSema = NULL_HANDLE;
UINT32              _aru4FifoStart[JDEC_INST_NUM];
#if !CONFIG_DRV_VERIFY_SUPPORT
//IMG_JPG_OBJ_T       _tImgJpgObj;
#endif
BOOL                _fgJdecInit = FALSE;
BOOL _fgJdecPrintMapUnmapLog = FALSE;
BOOL _fgJdecPrintRdPtrUpdateLog = FALSE;
/*-----------------------------------------------------------------------------
                    function/variable extern
 ----------------------------------------------------------------------------*/
extern UINT32 _u4JdecTrnsTotalTimes;
extern UINT32 _u4JdecDecTotalTimes;
extern UINT32 _u4JdecScaleTotalTimes;
extern UINT32 _u4JdecDecFailCount;
extern UINT32 _u4JdecDecodeTimes;

extern void vJdecIncLogCount1(void);
extern void vJdecResetLogCount2(void);
extern void vJdecSetLog(UINT32 u4LogVal);
extern void vJdecSaveState(JDEC_INST_T *prJdecInst);

/*-----------------------------------------------------------------------------
                    Code Start
 ----------------------------------------------------------------------------*/

#if CONFIG_SUSPEND_TO_DRAM
int i4JDEC_Suspend(void *param);
int i4JDEC_Resume(void *param);
struct pm_operations jdec_pm_ops = {
  .suspend = i4JDEC_Suspend,
  .resume = i4JDEC_Resume,
};
#endif

#if JDEC_USE_MAP_UNMAP
extern BOOL _fgJdecPrintMapUnmapLog;

void vJdecMapPrintf(const char *format_string, ...)
{
  va_list ap;
  char buffer[512];
  
  if (_fgJdecPrintMapUnmapLog)
  {
    va_start (ap, format_string);
    x_vsnprintf (buffer,512, format_string, ap);
    UTIL_Printf (buffer);
    va_end (ap);
  }
}
#endif

extern BOOL _fgJdecPrintRdPtrUpdateLog;

void vJdecRdPtrPrintf(const char *format_string, ...)
{
  va_list ap;
  char buffer[512];
  
  if (_fgJdecPrintRdPtrUpdateLog)
  {
    va_start (ap, format_string);
    x_vsnprintf (buffer,512, format_string, ap);
    UTIL_Printf (buffer);
    va_end (ap);
  }
}


#if JDEC_CACHE_ENABLE
void vJdecReCalCachePriority(JDEC_PIC_INFO_T *prCacheInfo)
{
    UINT32 u4 = 0;
    //use semaphore_A prevent
    x_sema_lock(_hJdecCacheSema, X_SEMA_OPTION_WAIT);
    prCacheInfo->i4ReplaceCount = JDEC_PIC_INFO_CACHE_NUM;

    for (u4=0; u4<JDEC_PIC_INFO_CACHE_NUM; u4++)
    {
        if (_rJdecPicInfo[u4].eCacheState != JDEC_CACHE_INVALID)
        {
            _rJdecPicInfo[u4].i4ReplaceCount--;
        }
    }
    x_sema_unlock(_hJdecCacheSema);
    //--
}

void vJdecSetCacheState(JDEC_PIC_INFO_T *prCacheInfo, JDEC_CACHE_STATE eCacheState)
{
    //use semaphore_A prevent
    x_sema_lock(_hJdecCacheSema, X_SEMA_OPTION_WAIT);

    prCacheInfo->eCacheState = eCacheState;
    if (eCacheState == JDEC_CACHE_INVALID)
    {
        prCacheInfo->i4ReplaceCount = 0;
    }

    x_sema_unlock(_hJdecCacheSema);
    //--
}

UINT32 u4JdecGetCache(void)
{
    UINT32 u4 = 0;
    INT32 i4Priority = JDEC_PIC_INFO_CACHE_NUM;

    //use semaphore_A prevent
    x_sema_lock(_hJdecCacheSema, X_SEMA_OPTION_WAIT);

    for (u4=0; u4<JDEC_PIC_INFO_CACHE_NUM; u4++)
    {
        if (_rJdecPicInfo[u4].i4ReplaceCount < i4Priority)
        {
            i4Priority = _rJdecPicInfo[u4].i4ReplaceCount;
            if (_rJdecPicInfo[u4].eCacheState == JDEC_CACHE_INVALID)
            {
                break;
            }
        }
    }

    vJdecSetCacheState(&(_rJdecPicInfo[u4]), JDEC_CACHE_INUSING);
    vJdecReCalCachePriority(&(_rJdecPicInfo[u4]));
    x_sema_unlock(_hJdecCacheSema);
    //--
    return u4;
}
#endif
extern void vHwJdecPrintRegister(UINT32 u4HwId);
void vJdecAbort(JDEC_INST_T *prJdecInst, IMG_NFY_STATE_T eState, BOOL fgNfy)
{
    INT32 i4Ret;

    x_sema_lock(prJdecInst->hJdecSema, X_SEMA_OPTION_WAIT);

    if (fgNfy)
    {
        vJdecLog_L("[Jdec] prJdecInst State:%d \n", eState);
        if (eState == IMG_NFY_ERROR)
        {
           if(prJdecInst->i4JdecErrorType)
           {
         	UTIL_Printf("[Jdec] ImgId=%x, ErrCode=%d, DecMode=%d, DecState=%d\n", prJdecInst->u4ImgId, prJdecInst->i4JdecErrorType, prJdecInst->eJdecDecMode, prJdecInst->eJdecDecState);
                if(!prJdecInst->fgSwDecode) 
                vHwJdecPrintRegister(prJdecInst->u4HwInstId);
            }
        }
        else if (eState == IMG_NFY_FINISHED)
        {
            vJdecLog_L("[Jdec] ImgId=%x, TrnsTime=%d, DecTime=%d, ScaleTime=%d\n", prJdecInst->u4ImgId, _u4JdecTrnsTotalTimes, _u4JdecDecTotalTimes, _u4JdecScaleTotalTimes);
            vJdecLog_M("[Jdec] ImgId=%x, MCUWidth=%d, Time=%d, Count=%d, DecMode=%d\n", prJdecInst->u4ImgId, prJdecInst->u4MCUInMCURowW, _u4JdecDecodeTimes, _u4JdecDecFailCount, prJdecInst->eJdecDecMode);
        }
    }
    if (prJdecInst->eJdecDecMode != JDEC_HAL_DEC_MODE_BASELINE_PIC)
    {
        x_timer_stop(prJdecInst->hJdecSleepTimer);
    }

    i4Ret = x_timer_stop(prJdecInst->hJdecDecTimer);
    
    //stop data transfer
    if (prJdecInst->u4ImgdmaId != JDEC_INVALID32)
    {
        i4Ret = i4ImgdmaSetReset(prJdecInst->u4ImgdmaId);
        VERIFY(i4Ret == IMGDMA_OK);
    }
    
    //stop hal decode
    if (prJdecInst->fgInitHal == TRUE)
    {
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
        if (IMG_NFY_STOP_DONE == eState)
        {
            i4Ret = i4JDEC_HAL_Uninit(prJdecInst->u4HwInstId,TRUE);
        }
        else
        {
            i4Ret = i4JDEC_HAL_Uninit(prJdecInst->u4HwInstId,FALSE);
        }
#else
        i4Ret = i4JDEC_HAL_Uninit(prJdecInst->u4HwInstId);
#endif
        VERIFY(i4Ret == S_JDEC_HAL_OK);
        prJdecInst->fgInitHal = FALSE;
    }

#if 0
    // Add by pingzhao for free the jfif instance memory , 2009/1/9
    if (prJdecInst->hJfifInst != NULL)
    {
        JFIF_FreeInstance(prJdecInst->hJfifInst);
    }
#endif    

    //clean event
    vJdecLog_L("[jdec]Clear  rJdecInst->hJdecEvent =0x%x\n",prJdecInst->hJdecEvent);
    i4Ret = x_ev_group_set_event(prJdecInst->hJdecEvent, 0x00, X_EV_OP_CLEAR);
    VERIFY(i4Ret == OSR_OK);

    //reset state
    vJdecSaveState(prJdecInst);
    prJdecInst->eJdecState = JDEC_STATE_IDLE;
    prJdecInst->eJdecDecState = JDEC_DECODE_IDLE;

#if JPG_SUPPORT_DIRECT_DECODE_MODE
       if(prJdecInst->fgNoJpgCbToUSMode)
       {
            vJdecLog_L("[jdec]  fgNoJpgCbToUSMode\n");
          fgNfy = FALSE;
          if ((eState == IMG_NFY_ERROR) || (eState == IMG_NFY_STOP_DONE))
          {       
             i4Ret = x_ev_group_set_event(prJdecInst->hJpgDirectDecEv, (EV_GRP_EVENT_T)2, X_EV_OP_OR);
          }
          else
          {
             i4Ret = x_ev_group_set_event(prJdecInst->hJpgDirectDecEv, (EV_GRP_EVENT_T)1, X_EV_OP_OR);
          }
          VERIFY(i4Ret == OSR_OK);
       }
#endif

    //notify img lib
    if (fgNfy)
    {
#if SUPPORT_VDEC_PATH        
        if (prJdecInst->fgVdecPath)
        {
            //UTIL_Printf("[JDEC] Decode checksum = %x image size = %x \n",prJdecInst->u4CheckSum,prJdecInst->u4ImgSize);
            prJdecInst->rVdecCallback.pvCallBackFunc(prJdecInst->rVdecCallback.pvPrivData);
        }
        else
#endif   
        #if UNIFORM_DRV_CALLBACK
        {
             IMG_NFY_PARAM_T rPara;
     vJdecLog_L("[jdec]  AGENT notify\n");
             rPara.ui4_img_id = prJdecInst->u4ImgId;
             rPara.e_state = eState;
             rPara.rInfo.pv_cache = NULL;             
             vCBAgent_Cb_Nfy(prJdecInst->rNfyInfo.pf_cb_nfy_fct,
                             prJdecInst->rNfyInfo.pv_nfy_tag, 
                             (VOID*)(&rPara),sizeof(IMG_NFY_PARAM_T));
        }
        #else
        prJdecInst->pfNfyCb(prJdecInst->u4ImgId, prJdecInst->pvTag, NULL, eState);
        #endif
    }
  #if JPG_SUPPORT_DIRECT_DECODE_MODE 
    if(IMG_NFY_ERROR == eState)
    {
      prJdecInst->u4DecProgress = 200;
    }
    else
    {
      prJdecInst->u4DecProgress = 100;
    }   
    vJdecLog_L("[Jdec] set process:%d \n", prJdecInst->u4DecProgress);
   #endif 
    x_sema_unlock(prJdecInst->hJdecSema);
}


INT32 i4JdecGetProgress(UINT16 u2CompId, UINT32* pu4Progress)
{
    *pu4Progress = _rJdecInst[u2CompId].u4DecProgress;
    return RMR_OK;
}
EXPORT_SYMBOL(i4JdecGetProgress);

INT32 i4JdecFrmStart(INT16 u2CompId, IMG_FRM_START_T *prFrmStart)
{
    JDEC_INST_T *prJdecInst = &(_rJdecInst[u2CompId]);
    INT32 i4Ret;

    vJdecResetLogCount2();
    vJdecSetLog(JDEC_LOG_FRM_START);

    vJdecLog_L("[Jdec][%d] ImgId=%x, i4JdecFrmStart \n", u2CompId, prFrmStart->ui4_img_id);
    if (u2CompId == 1)
    {
        prJdecInst->fgSwDecode = TRUE;
    }
    else
    {
      i4Ret =  x_sema_lock(prJdecInst->hJdecHwDevSema, X_SEMA_OPTION_NOWAIT);
      if(i4Ret!=OSR_OK)
      {
         vJdecLog_L("[Jdec] Hardware decoder is not free\n");
         return i4Ret;
      }
    }
    prJdecInst->u4ImgId     = prFrmStart->ui4_img_id;
    prJdecInst->pvImgBuf    = prFrmStart->pv_img_buf;
    prJdecInst->fgJdecKernImgBuf = prFrmStart->fgKernelImgData;
#if !CONFIG_DRV_ONLY 
    if (prJdecInst->pvImgBuf != NULL)
    {
#if CONFIG_SYS_MEM_PHASE2
        prJdecInst->pvImgBuf = addr_user_to_kernel(prJdecInst->pvImgBuf);
#elif !JDEC_DISCONTINUOUS_SOURCE
        #if !JDEC_IO_MMU_TEST
        #if SUPPORT_VDEC_PATH        
        if (!prFrmStart->fgVdecPath)
        #endif
                 prJdecInst->pvImgBuf = x_kmem_addr_user_to_kernel(prJdecInst->pvImgBuf, prFrmStart->u4_img_position);
        #else
	     vJdecLog_L("[Jdec]  prJdecInst->pvImgBuf[0x%x], position[0x%x]\n",
                                       prJdecInst->pvImgBuf,
                                       prFrmStart->ui4_img_position
				);
    	     if(prJdecInst->fgJdecKernImgBuf) 	
             {               
    	          prJdecInst->pvImgBuf = x_kmem_kernel_address((HANDLE_T)prJdecInst->pvImgBuf, prFrmStart->ui4_img_position);
              }
              else
              {
                 prJdecInst->pvImgBuf += prFrmStart->ui4_img_position;
		 
              }
	  vJdecLog_L("[Jdec]  prJdecInst->pvImgBuf[0x%x]\n",prJdecInst->pvImgBuf);
#endif    
#endif    
    }
#endif 
    prJdecInst->u4ImgSize   = prFrmStart->ui4_img_size;
    prJdecInst->u4LastScanNum = 0;
    prJdecInst->u4FirstScanNum = 0;
    prJdecInst->fgDecodeFirstScanOnly = FALSE;
    prJdecInst->fgParsingHeaderWaitData = FALSE;
    prJdecInst->u4NeedData = 0;
    prJdecInst->u4DecProgress = 0;
    prJdecInst->eDecodeFlag = prFrmStart->e_jpg_flag;
    if (prJdecInst->u4ImgSize == 0)
    {
        prJdecInst->u4ImgSize = 50*1024*1024;
    }
#if _JDEC_DRV_CACHE_BUF_FOR_DEBG
    if(puJdecDstBuf == NULL)
    {      
       puJdecDstBuf = (void *)x_mem_valloc(2* 1024*1024);
      
    }
     UTIL_Printf("[jdec]---puJdecDstBuf = [0x%x]\n",puJdecDstBuf);
    _u4Pos =0;
    {
            int i=0;
            BYTE *prTmp =NULL;
            prTmp = (BYTE *)puJdecDstBuf;
            for(i=0; i<0x200000; i++)
            {
              prTmp[i] =0;
           }

      vJdecLog_L("clen--- testing buffer\n");
   }
#endif      
    //prJdecInst->pvPicCache  = prFrmStart->pv_aux_cache;
#if JDEC_CACHE_ENABLE    
    prJdecInst->pvPicCache  = &(_rJdecPicInfo[u2CompId]);
#endif
    #if UNIFORM_DRV_CALLBACK
    prJdecInst->rNfyInfo.pf_cb_nfy_fct = prFrmStart->pf_func;
    prJdecInst->rNfyInfo.pv_nfy_tag    = prFrmStart->pv_tag;
    #else
    prJdecInst->pfNfyCb     = prFrmStart->pf_func;
    prJdecInst->pvTag       = prFrmStart->pv_tag;
    #endif

#if SUPPORT_VDEC_PATH
    prJdecInst->fgVdecPath  = prFrmStart->fgVdecPath;
    if (prJdecInst->fgVdecPath)
    {
        prJdecInst->hJfifInst   = prFrmStart->hJfifInst;
    }
    if ((prJdecInst->fgVdecPath) && (prFrmStart->hJfifInst == NULL))
    {
        // Invalid value;
        ASSERT(0);
    }
    if (prJdecInst->fgVdecPath)
    {
        prJdecInst->rVdecCallback.pvCallBackFunc = prFrmStart->prVdecCallback->pvCallBackFunc;
        prJdecInst->rVdecCallback.pvPrivData = prFrmStart->prVdecCallback->pvPrivData;
    }
    vJdecLog_L("[Jdec] fgVdecPath =%d | JfifInst = %x  i4JdecFrmStart \n", prJdecInst->fgVdecPath,prJdecInst->hJfifInst);
#endif
    

    #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)
    prJdecInst->fgWaitStop  = FALSE;
    #endif

    if (prJdecInst->u4ImgdmaId == JDEC_INVALID32)
    {
        i4Ret = i4ImgdamGetTicket(&prJdecInst->u4ImgdmaId);
        VERIFY(i4Ret == IMGDMA_OK);
    }
    if (prJdecInst->rImgReszTicket.u4Ticket == JDEC_INVALID32)
    {
        i4Ret = i4ImgResz_Drv_GetTicket(&(prJdecInst->rImgReszTicket));
        VERIFY(i4Ret == 0);
    }
    vJdecScaleSwitch2SW(prJdecInst, FALSE);
    prJdecInst->eJdecState = JDEC_STATE_PRE_START;
    #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
    prJdecInst->u4VJDMHandle = u4VJDM_GetHandle();

    //move from vJdecStateStart for BDP220685
    //check HW resource 
    i4Ret = i4VJDM_GetResource(prJdecInst->u4VJDMHandle, 1, FALSE, 0);
    if (i4Ret != OSR_OK)
    {
        prJdecInst->i4JdecErrorType = -E_JPG_GET_NO_HW_FAIL;
        return RMR_OUT_OF_HANDLES;
    }
    #endif
    return RMR_OK;
}
EXPORT_SYMBOL(i4JdecFrmStart);

INT32 i4JdecDecode(UINT16 u2CompId, IMG_DECODE_T *prImgDec)
{
    JDEC_INST_T *prJdecInst = &(_rJdecInst[u2CompId]);
    INT32 i4Ret;

    vJdecSetLog(JDEC_LOG_FRM_DECODE);

    vJdecLog_L("[Jdec] ImgId=%x, i4JdecDecode \n", prJdecInst->u4ImgId);

    //HalFlushInvalidateDCache();
    //BSP_FlushDCacheRange();
    
    if (prJdecInst->u4ImgId != prImgDec->ui4_img_id)
    {
       vJdecLog_L("[jdec] 2 IMG ID not match \n");
        return RMR_DRV_SET_FAILED;
    }
    //stop current decode
    if (prJdecInst->eJdecState != JDEC_STATE_PRE_START)
    {
        vJdecAbort(prJdecInst, (IMG_NFY_STATE_T)0, FALSE);
    }
    prJdecInst->u4FrameIndex    = (UINT32)(prImgDec->pv_type_data);
#if JPG_SUPPORT_DIRECT_DECODE_MODE
    prJdecInst->fgNoJpgCbToUSMode = prImgDec->b_directdecodemode;
#else
    prJdecInst->fgNoJpgCbToUSMode = FALSE;
#endif
    prJdecInst->fgStartDecodeNow     = !(prJdecInst->fgNoJpgCbToUSMode); 
    prJdecInst->u4SrcCropPosX   = prImgDec->ui4_src_x;
    prJdecInst->u4SrcCropPosY   = prImgDec->ui4_src_y;
    prJdecInst->u4SrcCropWidth  = prImgDec->ui4_src_width;
    prJdecInst->u4SrcCropHeight = prImgDec->ui4_src_height;
    prJdecInst->pvDstYBuf       = prImgDec->pv_dst;
#if !CONFIG_DRV_ONLY    
    if ((prJdecInst->pvDstYBuf != NULL) &&
        (!prJdecInst->fgNoJpgCbToUSMode))
    {
#if CONFIG_SYS_MEM_PHASE2
        prJdecInst->pvDstYBuf = addr_user_to_kernel(prJdecInst->pvDstYBuf);
#elif CONFIG_SYS_MEM_PHASE3
        UTIL_Printf("[JDEC] Usr Dest buffer1 %x \n",prJdecInst->pvDstYBuf);
        #if JDEC_IO_MMU_TEST
        // convert handle to address
        #if SUPPORT_VDEC_PATH
        if (!prJdecInst->fgVdecPath)
        #endif
        {
           prJdecInst->pvDstYBuf = x_kmem_kernel_address((HANDLE_T)prJdecInst->pvDstYBuf, 0);
		   x_kmem_sync_table((void *)((UINT32)(prJdecInst->pvDstYBuf) + (UINT32)(prImgDec->ui4_dst_y * prImgDec->ui4_dst_pitch)),
                         prImgDec->ui4_dst_height * prImgDec->ui4_dst_pitch);
        }
        #else
        #if SUPPORT_VDEC_PATH
        if (!prJdecInst->fgVdecPath)
        #endif

        prJdecInst->pvDstYBuf = x_kmem_addr_user_to_kernel(prJdecInst->pvDstYBuf);
        #endif
        //UTIL_Printf("[JDEC] Krn Dest buffer2 %x \n",prJdecInst->pvDstYBuf);
#endif
    }
#endif    
    prJdecInst->pvDstCBuf       = prImgDec->pv_dst2;
#if !CONFIG_DRV_ONLY
    if((prJdecInst->pvDstCBuf != NULL)&&
        (!prJdecInst->fgNoJpgCbToUSMode))
    {
#if CONFIG_SYS_MEM_PHASE2
        prJdecInst->pvDstCBuf = addr_user_to_kernel(prJdecInst->pvDstCBuf);
#elif CONFIG_SYS_MEM_PHASE3
       #if JDEC_IO_MMU_TEST
       // convert handle to address
       #if SUPPORT_VDEC_PATH
        if (!prJdecInst->fgVdecPath)
       #endif
       prJdecInst->pvDstCBuf = x_kmem_kernel_address((HANDLE_T)prJdecInst->pvDstCBuf, 0);	   
       #else
       #if SUPPORT_VDEC_PATH
        if (!prJdecInst->fgVdecPath)
       #endif
        prJdecInst->pvDstCBuf = x_kmem_addr_user_to_kernel(prJdecInst->pvDstCBuf);
#endif
#endif
    }
#endif    
    prJdecInst->u4DstCropPosX   = prImgDec->ui4_dst_x;
    prJdecInst->u4DstCropPosY   = prImgDec->ui4_dst_y;
    prJdecInst->u4DstCropWidth  = prImgDec->ui4_dst_width;
    prJdecInst->u4DstCropHeight = prImgDec->ui4_dst_height;
    prJdecInst->u4DstBufPitch   = prImgDec->ui4_dst_pitch;
    prJdecInst->eDstColorMode   = prImgDec->e_dst_cm;
    prJdecInst->eDstRotate      = prImgDec->e_rotate;
    prJdecInst->eQuality        = prImgDec->e_quality;
    //prJdecInst->eQuality        = IMG_QUALITY_FACTOR_NORMAL;
    vJdecLog_H("[JDEC] Source %d X %d, Destination %d X %d \n",
               prJdecInst->u4SrcCropWidth,prJdecInst->u4SrcCropHeight,
               prJdecInst->u4DstCropWidth,prJdecInst->u4DstCropHeight);

#if JDEC_SUPPORT_BROKEN_TARGET
    if (!prJdecInst->fgNoJpgCbToUSMode&&
        (prJdecInst->pvImgBuf != NULL) && 
        (prJdecInst->eDstColorMode == GFX_COLORMODE_ARGB_D8888))
    {
        KMEM_BLK_INFO rBlkInfo;
        HANDLE_T hTemp = NULL_HANDLE;        
        prJdecInst->fgBrokenTarget = TRUE;        
        // get memory info 
        x_memset(&rBlkInfo, 0, sizeof(KMEM_BLK_INFO));
        hTemp = x_kmem_block_handle(prJdecInst->pvDstYBuf);
        //UTIL_Printf("[JDEC] Dst Handle = %x \n",hTemp);
        i4Ret = x_kmem_block_info(hTemp, &rBlkInfo);
        //UTIL_Printf("[JDEC] Dst buffer info Total %d  Blk size = %x last size = %x \n",
        //            rBlkInfo.n,rBlkInfo.size, rBlkInfo.last_size);

        prJdecInst->rBlkInfo.u4BlkIdx = 0;
        prJdecInst->rBlkInfo.u4BlkOfst = 0;
        prJdecInst->rBlkInfo.u4BlkSa = (UINT32)prJdecInst->pvDstYBuf;
        prJdecInst->rBlkInfo.u4BlkSize = rBlkInfo.size;
        prJdecInst->rBlkInfo.u4TotalBlk = rBlkInfo.n;
        prJdecInst->rBlkInfo.u4LastBlkSize = rBlkInfo.last_size;
        prJdecInst->rBlkInfo.hMem = hTemp;
        prJdecInst->u4TempAYUVBuf = JDEC_INVALID32;
        prJdecInst->u4TempLineBuf = JDEC_INVALID32;      
        if ((rBlkInfo.n == 1) && (rBlkInfo.last_size == 0))
        {
            prJdecInst->fgBrokenTarget = FALSE;
        }
        else if ((prJdecInst->u4SrcCropWidth != prJdecInst->u4DstCropWidth) ||
            (prJdecInst->u4SrcCropHeight != prJdecInst->u4DstCropHeight) ||
            (prJdecInst->u4DstCropPosX != 0) ||
            (prJdecInst->u4DstCropPosY != 0))
        {
            ASSERT(0);
        }
        if (prJdecInst->fgBrokenTarget)
        {
            // Print block address
            UINT32 u4Value = 0;
            UINT32 u4TempSA = 0;

            for (u4Value = 0; u4Value <= prJdecInst->rBlkInfo.u4TotalBlk ; u4Value ++)
            {
                u4TempSA = (UINT32)x_kmem_kernel_address(hTemp, u4Value * prJdecInst->rBlkInfo.u4BlkSize);
                vJdecLog_L("[JDEC] Block%d SA = 0x%x \n",u4Value, u4TempSA);
            }
            vJdecLog_L("[JDEC] Last block size = %x \n", prJdecInst->rBlkInfo.u4LastBlkSize);
        }
    }
#endif

    //send event to task, to do decode
    vJdecLog_L("[jdec] rJdecInst->hJdecEvent =0x%x\n",prJdecInst->hJdecEvent);
    i4Ret = x_ev_group_set_event(prJdecInst->hJdecEvent, JDEC_DRV_EV_DECODE, X_EV_OP_OR);
    vJdecLog_L("[jdec] Send Decode event\n");    
     if(prJdecInst->fgNoJpgCbToUSMode)
    {
         UINT32 u4Cnt=0;
         vJdecLog_L("[jdec] IOCTRL FLOW\n");    
		 
        while((prJdecInst->eJdecState != JDEC_STATE_PRS_HDR) && (u4Cnt<100))
        {
		  vJdecLog_L("[jdec] ==delay status, u4Cnt=0x%x\n",u4Cnt);  
          x_thread_delay(5);
		  u4Cnt++;
        }
     }
    vJdecLog_L("[jdec] Status OK\n");    
    VERIFY(i4Ret == OSR_OK);
    return RMR_OK;
}
EXPORT_SYMBOL(i4JdecDecode);

INT32 i4JdecSetDecodeInfo(UINT16 u2CompId, IMG_DECODE_T *prImgDec)
{
    JDEC_INST_T *prJdecInst = &(_rJdecInst[u2CompId]);
    if (prJdecInst->u4ImgId != prImgDec->ui4_img_id)
    {
        vJdecLog_L("[jdec] 2 IMG ID not match \n");
        return RMR_DRV_SET_FAILED;
    }
    prJdecInst->u4FrameIndex    = (UINT32)(prImgDec->pv_type_data);
    prJdecInst->u4SrcCropPosX   = prImgDec->ui4_src_x;
    prJdecInst->u4SrcCropPosY   = prImgDec->ui4_src_y;
    prJdecInst->u4SrcCropWidth  = prImgDec->ui4_src_width;
    prJdecInst->u4SrcCropHeight = prImgDec->ui4_src_height;
    prJdecInst->pvDstYBuf       = prImgDec->pv_dst;
    prJdecInst->u4DstCropPosX   = prImgDec->ui4_dst_x;
    prJdecInst->u4DstCropPosY   = prImgDec->ui4_dst_y;
    prJdecInst->u4DstCropWidth  = prImgDec->ui4_dst_width;
    prJdecInst->u4DstCropHeight = prImgDec->ui4_dst_height;
    prJdecInst->u4DstBufPitch   = prImgDec->ui4_dst_pitch;
    prJdecInst->eDstColorMode   = prImgDec->e_dst_cm;
    prJdecInst->eDstRotate      = prImgDec->e_rotate;
    prJdecInst->eQuality        = prImgDec->e_quality;    
    prJdecInst->fgStartDecodeNow = TRUE;
    {      
       vJdecLog_L("jdec--- PDST = 0x%08x \n", (UINT32)prJdecInst->pvDstYBuf);
       vJdecLog_L("jdec--- user buffer\n");
    }
      vJdecLog_L("[JDEC] eQuality = 0x%x \n",prJdecInst->eQuality);
    vJdecLog_L("[JDEC] eDstColorMode = 0x%x \n",prJdecInst->eDstColorMode);   
	 if(prJdecInst->fgNoJpgCbToUSMode)
    {
        vJdecLog_L("[jdec] wait IOCTRL FLOW-- JDEC_STATE_INIT_HAL\n");
        if(prJdecInst->eJdecState < JDEC_STATE_INIT_HAL)
        {
          x_thread_delay(50);
          UTIL_Printf("[jdec] Status enter header not pasring done\n");
        }
    }
    vJdecLog_L("[jdec] Status enter JDEC_STATE_INIT_HAL\n");
    return RMR_OK;

}
EXPORT_SYMBOL(i4JdecSetDecodeInfo);
INT32 i4JdecFrmEnd(UINT16 u2CompId)
{
    JDEC_INST_T *prJdecInst = &(_rJdecInst[u2CompId]);
    INT32 i4Ret;
    INT32 i4Count1, i4Count2;
    IMGRESZ_DRV_SCALE_STATE_T rState = {0};

    vJdecSetLog(JDEC_LOG_FRM_END);

    vJdecLog_L("[Jdec] ImgId=%x, i4JdecFrmEnd \n", prJdecInst->u4ImgId);
    if (prJdecInst->eJdecState != JDEC_STATE_IDLE)
    {
        i4Ret = x_ev_group_set_event(prJdecInst->hJdecEvent, JDEC_DRV_EV_STOP, X_EV_OP_OR);
        VERIFY(i4Ret == OSR_OK);
    }
    vJdecAbort(prJdecInst, (IMG_NFY_STATE_T)0, FALSE);

    x_sema_lock(prJdecInst->hJdecSema, X_SEMA_OPTION_WAIT);
    if (prJdecInst->u4ImgdmaId != JDEC_INVALID32)
    {
        i4Ret = i4ImgdmaSetRelTicket(prJdecInst->u4ImgdmaId);
        VERIFY(i4Ret == IMGDMA_OK);
        prJdecInst->u4ImgdmaId = JDEC_INVALID32;
    }
    if (prJdecInst->rImgReszTicket.u4Ticket != JDEC_INVALID32)
    {
        if ((prJdecInst->eJdecDecMode != JDEC_HAL_DEC_MODE_BASELINE_PIC)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530) 
        && (prJdecInst->eJdecDecMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
        && (prJdecInst->eJdecDecMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif      
            )
        {
        i4Ret = i4ImgResz_Drv_SetLock(&(prJdecInst->rImgReszTicket), FALSE);
        }
        //VERIFY(i4Ret >= 0);
        i4Ret = i4ImgResz_Drv_GetState(&(prJdecInst->rImgReszTicket), &rState);
        UTIL_Printf("[jdec] imgresz state[%d]\n",rState.eState);
        if (rState.eState == IMGRESZ_DRV_STATE_SCALING)
        {
            i4JdecScaleStop(prJdecInst);
        }
        i4Ret = i4ImgResz_Drv_ReleaseTicket(&(prJdecInst->rImgReszTicket));
        VERIFY(i4Ret == 0);
        prJdecInst->rImgReszTicket.u4Ticket = JDEC_INVALID32;
    }
    if (prJdecInst->hJfifInst != NULL)
    {
        #if 0//JDEC_USE_MAP_UNMAP
        vJdecMapPrintf("[JDEC] Unmap from 0x%x to 0x%x point 1\n",(UINT32)prJdecInst->hJfifInst, 
                   ((UINT32)prJdecInst->hJfifInst + sizeof(JPEG_JFIF_HEADER_T)/JDEC_CACHE_LINE_ALIGN*JDEC_CACHE_LINE_ALIGN));
        BSP_dma_unmap_vaddr((UINT32)prJdecInst->hJfifInst,sizeof(JPEG_JFIF_HEADER_T)/JDEC_CACHE_LINE_ALIGN*JDEC_CACHE_LINE_ALIGN, TO_DEVICE);
        #endif
        JFIF_FreeInstance(prJdecInst->hJfifInst);
        prJdecInst->hJfifInst = NULL;
    }

    if (prJdecInst->ptImgJpgObj == NULL)
    {
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
    }
    else
    {
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
        for (i4Count1=0; i4Count1<MAX_COMPS_IN_SCAN; i4Count1++)
        {
            if (prJdecInst->ptImgJpgObj->apui2_buffer[i4Count1] != 0)
            {
                #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                x_free_aligned_dma_mem((void*)(prJdecInst->ptImgJpgObj->apui2_buffer[i4Count1]));
#elif CONFIG_SYS_MEM_PHASE3
                x_mem_aligned_free((void*)(prJdecInst->ptImgJpgObj->apui2_buffer[i4Count1]));
#endif
                #else
                x_free_aligned_nc_mem((void*)(prJdecInst->ptImgJpgObj->apui2_buffer[i4Count1]));
                #endif
                //x_mem_free((void*)(prJdecInst->ptImgJpgObj->apui2_buffer[i4Count1]));
                prJdecInst->ptImgJpgObj->apui2_buffer[i4Count1] = NULL;
            }
        }
        x_mem_vfree(prJdecInst->ptImgJpgObj);
        prJdecInst->ptImgJpgObj = NULL;
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
        //x_mem_aligned_free((void*)(_aru4FifoStart[prJdecInst->u2CompId]));
        
        //hTemp =  x_kmem_block_handle((void *)_aru4FifoStart[prJdecInst->u2CompId]);
        #if !JDEC_IO_MMU_TEST
        vJdecLog_L("[JDEC] Free fifo handle %x addr %x \n",prJdecInst->rFifoInfo.hFifo,_aru4FifoStart[prJdecInst->u2CompId]);
        x_kmem_block_aligned_free(prJdecInst->rFifoInfo.hFifo);
        #else
        x_mem_vfree((void*)(_aru4FifoStart[prJdecInst->u2CompId]));
        x_kmem_destroy_handle(prJdecInst->rFifoInfo.hFifo);
        #endif
        prJdecInst->rFifoInfo.hFifo = NULL_HANDLE;
#endif
        #else
        x_free_aligned_nc_mem((void*)(_aru4FifoStart[prJdecInst->u2CompId]));
        #endif
        _aru4FifoStart[prJdecInst->u2CompId] = JDEC_INVALID32;
    }
    // clean the flag for next jpg decoding
    #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)
    prJdecInst->fgWaitStop = FALSE;
    #endif

    // free for slice target
#if JDEC_SUPPORT_BROKEN_TARGET
    if (prJdecInst->fgBrokenTarget)
    {
        //HANDLE_T hTemp = NULL_HANDLE;

        
        // free AYUV buffer
        if (prJdecInst->u4TempAYUVBuf != JDEC_INVALID32)
        {
            //hTemp = x_kmem_block_handle((void *)prJdecInst->u4TempAYUVBuf);
            //UTIL_Printf("[JDEC] Free u4TempAYUVBuf handle %x addr %x \n",hTemp,prJdecInst->u4TempAYUVBuf);
            x_kmem_block_aligned_free(prJdecInst->hTempAYUVBuf);
            prJdecInst->u4TempAYUVBuf = JDEC_INVALID32;
            prJdecInst->u4TempAYUVBuf = NULL_HANDLE;
        }
        
        // free temp line buffer
        if (prJdecInst->u4TempLineBuf != JDEC_INVALID32)
        {
            //hTemp = x_kmem_block_handle((void *)prJdecInst->u4TempLineBuf);
            //UTIL_Printf("[JDEC] Free u4TempLineBuf handle %x addr %x \n",hTemp,prJdecInst->u4TempLineBuf);
            x_kmem_block_aligned_free(prJdecInst->hTempLineBuf);
            prJdecInst->u4TempLineBuf = JDEC_INVALID32;
            prJdecInst->hTempLineBuf = NULL_HANDLE;
        }
        prJdecInst->fgBrokenTarget = FALSE;
        prJdecInst->fgBrokenRow = FALSE;
    }
#endif
    x_sema_unlock(prJdecInst->hJdecSema);

    #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
    // move from vJdecAbort for BDP220685
    if (prJdecInst->u4VJDMHandle != JDEC_INVALID32)
    {
        i4VJDM_RelResource(prJdecInst->u4VJDMHandle, 1);
        vVJDM_RelHandle(prJdecInst->u4VJDMHandle);
        prJdecInst->u4VJDMHandle = JDEC_INVALID32;
    }
    #endif
    if (u2CompId == 1)
    {
        prJdecInst->fgSwDecode = FALSE;
    }
    vJdecSetLog(JDEC_LOG_FRM_END_END);
    vJdecIncLogCount1();
   if(u2CompId ==0)
   {
     i4Ret =  x_sema_unlock(prJdecInst->hJdecHwDevSema);
     if(i4Ret!=OSR_OK)
     {
         vJdecLog_L("[Jdec] Hardware decoder is not free\n");
     }
     else
     {
        vJdecLog_L("[Jdec] Hardware decoder is free\n");
     }
   }
    return RMR_OK;
}

EXPORT_SYMBOL(i4JdecFrmEnd);

INT32 i4JdecStop(UINT16 u2CompId)
{
    JDEC_INST_T *prJdecInst = &(_rJdecInst[u2CompId]);
    INT32 i4Ret;

    vJdecSetLog(JDEC_LOG_FRM_STOP);

    vJdecLog_L("[Jdec] ImgId=%x, DecState=%d, i4JdecStop \n", prJdecInst->u4ImgId, prJdecInst->eJdecDecState);
    if (prJdecInst->eJdecState != JDEC_STATE_IDLE)
    {
        i4Ret = x_ev_group_set_event(prJdecInst->hJdecEvent, JDEC_DRV_EV_STOP, X_EV_OP_OR);
        VERIFY(i4Ret == OSR_OK);
        vJdecLog_L("[Jdec] i4JdecStop Set Event \n");
        //vJdecAbort(prJdecInst, (IMG_NFY_STATE_T)0, FALSE);
        vJdecAbort(prJdecInst, IMG_NFY_STOP_DONE, TRUE);
    }
    else
    {
        vJdecAbort(prJdecInst, IMG_NFY_ALREADY_STOPPED, TRUE);
    }
    vJdecLog_L("[Jdec] i4JdecStop End\n");
    vJdecSetLog(JDEC_LOG_FRM_STOP_END);
    return RMR_OK;
}
EXPORT_SYMBOL(i4JdecStop);

INT32 i4JdecFillBuf(UINT16 u2CompId, IMG_BUF_FILLED_T *prBufFilled)
{
    JDEC_INST_T *prJdecInst = &(_rJdecInst[u2CompId]);
    INT32 i4Ret,i4Ret1;

    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    // flush cache memory
    //HalFlushInvalidateDCache();
    #if !JDEC_USE_MAP_UNMAP
    BSP_FlushDCacheRange(prJdecInst->rFifoInfo.u4WrPoint, prBufFilled->ui4_filled_len);
    #endif
    #endif
    // for BDP00019427, by pingzhao, 2009/2/12
    i4Ret = i4ImgdmaSetBufFilled(prJdecInst->u4ImgdmaId, prBufFilled);

    i4Ret1 = x_ev_group_set_event(prJdecInst->hJdecEvent, JDEC_DRV_EV_FILLED, X_EV_OP_OR);
    VERIFY(i4Ret1 == OSR_OK);


    switch(i4Ret)
    {
        case IMGDMA_OK:
            break;
        case IMGDMA_DRV_FAIL:
            //VERIFY(0);
            return RMR_DRV_SET_FAILED;
        case IMGDMA_DRV_EOF:
            break;
        case IMGDMA_DRV_IO_ERR:
            prJdecInst->i4JdecErrorType = -E_JPG_ERR_IO_ERROR;
            vJdecAbort(prJdecInst, IMG_NFY_ERROR, TRUE);
            break;
    }
    return RMR_OK;
}
#define JPEG_THRESH_HOLD (64 *1024)
extern void vJdecUpdateWritePointer(VOID *pv_tag, VOID *pv_data);
INT32 i4JdecSetFifoFill(UINT16 u2CompId, UINT32 u4SrcDataAdd, UINT32 u4SrcDataLen,BOOL fgFileEnd)
{
  INT32 i4Ret = 0;
  UINT32 u4FreeDataSize =0;
  UINT32 u4CopyLength =0;
  JDEC_INST_T *prJdecInst = &(_rJdecInst[u2CompId]);
  IMGDMA_DRV_FIFO_STATUS_T*   ptUsTmpFifoInfo = &(prJdecInst->tUsrTempFifoInfo);
  JDEC_FIFO_INFO_T*            ptFifoInfo      = &(prJdecInst->rFifoInfo);
  x_sema_lock(prJdecInst->hJdecSrcfifoSema, X_SEMA_OPTION_WAIT);

  if( (prJdecInst->eJdecState == JDEC_STATE_IDLE)&&
      (prJdecInst->eJdecDecState == JDEC_DECODE_IDLE))
  {  
    vJdecLog_L("jdec_drv, driver not ready\n");
     x_sema_unlock(prJdecInst->hJdecSrcfifoSema);
    return -1;
  }  
  #if 1//added for get hal rd points.
   if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)
        ||(prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
        || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)    
        )
  {
	    extern  INT32 i4JdecGetHalRdPoint(JDEC_INST_T *prJdecInst);
        if ((prJdecInst->eJdecDecState == JDEC_DECODE_WAIT_DECODE))
        {
          if (i4JdecGetHalRdPoint(prJdecInst) == JDEC_DRV_RTN_FAIL)
          {
            prJdecInst->i4JdecErrorType = -E_JPG_ERR_HAL_GET_RD;
	    vJdecLog_L("jdec_drv, get hal read point failed\n");
	 x_sema_unlock(prJdecInst->hJdecSrcfifoSema);
     return -1;
  }
		
		  vJdecLog_L("jdec_drv, hal wr point = 0x%x\n",ptUsTmpFifoInfo->u4WrPoint);	
        }		
  }
#endif 	
 vJdecLog_L("jdec_drv, read point = 0x%x\n",prJdecInst->rFifoInfo.u4RdPoint);
 // prJdecInst->tUsrTempFifoInfo.u4RdPoint = prJdecInst->rFifoInfo.u4RdPoint;
  vJdecLog_L("jdec_drv, prJdecInst->rFifoInfo.u4FifoEnd = 0x%x\n",prJdecInst->rFifoInfo.u4FifoEnd);
  vJdecLog_L("jdec_drv, prJdecInst->rFifoInfo.u4FifoStart = 0x%x\n", prJdecInst->rFifoInfo.u4FifoStart);	
  
  if (ptUsTmpFifoInfo->u4RdPoint > ptUsTmpFifoInfo->u4WrPoint)
  {
      ptUsTmpFifoInfo->u4ValidData = prJdecInst->rFifoInfo.u4FifoEnd - ptUsTmpFifoInfo->u4RdPoint;
      ptUsTmpFifoInfo->u4ValidData += ptUsTmpFifoInfo->u4WrPoint - prJdecInst->rFifoInfo.u4FifoStart;
  }
  else
  {
      if((ptUsTmpFifoInfo->u4RdPoint == prJdecInst->rFifoInfo.u4FifoStart) && 
	     (ptUsTmpFifoInfo->u4WrPoint == prJdecInst->rFifoInfo.u4FifoEnd))
    {
	   vJdecLog_L("jdec_drv, i4SetFifoFill specai case\n");  
	   ptUsTmpFifoInfo->u4ValidData = 0;
    }
	else
    {
             ptUsTmpFifoInfo->u4ValidData =ptUsTmpFifoInfo->u4WrPoint - ptUsTmpFifoInfo->u4RdPoint;
    }	
  }
  u4FreeDataSize = JDEC_DRV_FIFO_SIZE - ptUsTmpFifoInfo->u4ValidData;
  vJdecLog_L("jdec_drv, A i4SetFifoFill u4FreeDataSize =0x%x,u4ValidData =0x%x\n",
  	                    u4FreeDataSize,
  	                    ptUsTmpFifoInfo->u4ValidData);  
  if(prJdecInst->i4JdecErrorType)
  {    
       vJdecLog_L("jdec_drv, error for Decoder meeting erro\n");
      x_sema_unlock(prJdecInst->hJdecSrcfifoSema);
     return -1;
  }
  
  if( (u4FreeDataSize <2*JPEG_THRESH_HOLD)||
       (ptUsTmpFifoInfo->u4WrPoint == 0))
    {
       vJdecLog_L("jdec_drv, FIFO not ready\n");
          x_sema_unlock(prJdecInst->hJdecSrcfifoSema);
  	  return u4SrcDataLen;
    }

    if(u4SrcDataLen >= u4FreeDataSize-JPEG_THRESH_HOLD)
    {
      vJdecLog_L("jdec_drv, partial copy\n");
      u4CopyLength = u4FreeDataSize-JPEG_THRESH_HOLD;
      i4Ret = u4SrcDataLen - u4CopyLength;
    }
 else
    {
     vJdecLog_L("jdec_drv, Full  copy \n");
     u4CopyLength = u4SrcDataLen;
     i4Ret =0;
    }
 
  prJdecInst->fgNoLockWrSemphore = TRUE;  
   if(ptUsTmpFifoInfo->u4WrPoint+ u4CopyLength <= ptFifoInfo->u4FifoEnd)
    {
      vJdecLog_L("jdec_drv, copy once\n");
      ASSERT(0==copy_from_user((void*)(ptUsTmpFifoInfo->u4WrPoint),(void*)u4SrcDataAdd,u4CopyLength));
      ptUsTmpFifoInfo->u4WrPoint += u4CopyLength;
    }
  else
  {
    UINT32 u4CopyGap1 = ptFifoInfo->u4FifoEnd -ptUsTmpFifoInfo->u4WrPoint;
     vJdecLog_L("jdec_drv, copy twice\n");
     ASSERT(0==copy_from_user((void*)(ptUsTmpFifoInfo->u4WrPoint),(void*)u4SrcDataAdd,u4CopyGap1));
      u4SrcDataAdd+=u4CopyGap1;
     u4CopyGap1 =u4CopyLength - u4CopyGap1;	 
     ASSERT(0==copy_from_user((void*)(ptFifoInfo->u4FifoStart ),(void*)u4SrcDataAdd,u4CopyGap1));    
     ptUsTmpFifoInfo->u4WrPoint = ptFifoInfo->u4FifoStart + u4CopyGap1;
  }  

 //update fifo parameters.
  {
    if (ptUsTmpFifoInfo->u4RdPoint > ptUsTmpFifoInfo->u4WrPoint)
    {
      ptUsTmpFifoInfo->u4ValidData = ptFifoInfo->u4FifoEnd - ptUsTmpFifoInfo->u4RdPoint;
      ptUsTmpFifoInfo->u4ValidData += ptUsTmpFifoInfo->u4WrPoint - ptFifoInfo->u4FifoStart;
    }
    else
    {
      ptUsTmpFifoInfo->u4ValidData = ptUsTmpFifoInfo->u4WrPoint - ptUsTmpFifoInfo->u4RdPoint;
    }        
  } 

  if(ptUsTmpFifoInfo->u4WrPoint==ptFifoInfo->u4FifoEnd)
  {
    ptUsTmpFifoInfo->u4WrPoint = ptFifoInfo->u4FifoStart;
    vJdecLog_L("jdec_drv,  --Jumpt to fifo start\n");  
  }
  vJdecLog_L("jdec_drv,  i4SetFifoFill u4ValidData =0x%x\n", ptUsTmpFifoInfo->u4ValidData);  
  {
    BOOL fgValue = FALSE;
    vJdecLog_L("jdec_drv, Update write pointer\n");
    if(fgFileEnd && (i4Ret ==0))
    {
      vJdecLog_L("jdec_drv, file end \n");
      ptUsTmpFifoInfo->fgEoRange = fgFileEnd;
    }
    vJdecLog_L(" ptUsTmpFifoInfo->u4WrPoint=0x%x\n",ptUsTmpFifoInfo->u4WrPoint);
    vJdecUpdateWritePointer((void *)(&fgValue), (void *)prJdecInst->u4HwInstId);
  }
  x_sema_unlock(prJdecInst->hJdecSrcfifoSema);
  prJdecInst->fgNoLockWrSemphore = FALSE;
  
  vJdecLog_L("[jdec] Send Fill data event 0x%x\n", prJdecInst->hJdecEvent);
  #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
  #if !JDEC_USE_MAP_UNMAP
  BSP_FlushDCacheRange(ptFifoInfo->u4FifoStart, JDEC_DRV_FIFO_SIZE);
  #endif
  #endif
  VERIFY( x_ev_group_set_event(prJdecInst->hJdecEvent, JDEC_DRV_EV_FILLED, X_EV_OP_OR) == OSR_OK );
  
  return i4Ret;
   
}
EXPORT_SYMBOL(i4JdecSetFifoFill);

BOOL _fgJdecHW = TRUE;
void vJdecMain(void *arg)
{
    UINT32          u4ValTmp = *(UINT32*)(arg);
    UINT16          u2InstId = (UINT16)u4ValTmp;
    EV_GRP_EVENT_T  eJdecEvent;
    EV_GRP_EVENT_T  eJdecEventGet;
    JDEC_INST_T     *prJdecInst = NULL;
    INT32           i4Ret;

    if (u2InstId > (JDEC_INST_NUM - 1))
    {
        ASSERT(0);
    }
    else
    {
        prJdecInst = &(_rJdecInst[u2InstId]);
    }
    while (1)
    {
        if ((prJdecInst->eJdecState == JDEC_STATE_IDLE) || (prJdecInst->eJdecState == JDEC_STATE_PRE_START))
        {
           vJdecLog_L("[jdec]   wait dec event rJdecInst->hJdecEvent =0x%x\n",prJdecInst->hJdecEvent);
            eJdecEvent = JDEC_DRV_EV_DECODE | JDEC_DRV_EV_DEL_THREAD;
            i4Ret = x_ev_group_wait_event(prJdecInst->hJdecEvent, eJdecEvent, &eJdecEventGet, X_EV_OP_OR_CONSUME);
            VERIFY(i4Ret == OSR_OK);
            vJdecLog_L("[jdec] Got dec event 0x%x\n",prJdecInst->hJdecEvent);
            x_sema_lock(prJdecInst->hJdecSema, X_SEMA_OPTION_WAIT);
            if (prJdecInst->eJdecState == JDEC_STATE_PRE_START)
            {
                vJdecLog_L("[jdec] Got dec JDEC_STATE_START\n");
                prJdecInst->eJdecState = JDEC_STATE_START;
#if !CONFIG_DRV_VERIFY_SUPPORT 
                if(prJdecInst->fgSwDecode)
                {
                    if(_rJdecInst[1].ptImgJpgObj== NULL)
                    {
                      _rJdecInst[1].ptImgJpgObj = x_mem_valloc(sizeof(IMG_JPG_OBJ_T));//&(_tImgJpgObj);
                       x_memset(_rJdecInst[1].ptImgJpgObj, 0, sizeof(IMG_JPG_OBJ_T));
                    }
                }
#endif
            }
            else
            {
                vJdecLog_L("[jdec] Got dec JDEC_STATE_IDLE\n");
                prJdecInst->eJdecState = JDEC_STATE_IDLE;
            }
            if (eJdecEventGet & JDEC_DRV_EV_DEL_THREAD)
            {
                vJdecLog_L("[jdec] Got dec JDEC_STATE_IDLE\n");
                prJdecInst->hJdecInst = JDEC_INVALID32;
                return;
            }
            x_sema_unlock(prJdecInst->hJdecSema);
        }
        else
        {
            x_sema_lock(prJdecInst->hJdecSema, X_SEMA_OPTION_WAIT);
            if ((_fgJdecHW) && (!prJdecInst->fgSwDecode))
            {
                _rJdecInst[0].ptImgJpgObj = NULL;
                if (u2InstId == 0)
                {
                    vJdecDecode(prJdecInst);
                }
                //_rJdecInst[1].ptImgJpgObj = NULL;
                
            }
            else
            {                
                //_rJdecInst[0].ptImgJpgObj = &(_tImgJpgObj);
                if (u2InstId == 1)
                {
                    vJdecSwDecode(prJdecInst);
                }
            }
            x_sema_unlock(prJdecInst->hJdecSema);
        }
    }
}

#if CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN
extern void vImgdmaRegisterJdecFillBuf(INT32 (*pi4JdecFillBuf)(UINT16, IMG_BUF_FILLED_T *));
extern void ImgDecMw_RegisterJdecFrmStart(INT32 (* pi4JdecFrmStart)(INT16, IMG_FRM_START_T *));
extern void ImgDecMw_RegisterJdecGetProgress(INT32 (* pi4JdecGetProgress)(UINT16, UINT32*));
extern void ImgDecMw_RegisterJdecDecode(INT32 (* pi4JdecDecode)(UINT16, IMG_DECODE_T *));
extern void ImgDecMw_RegisterJdecStop(INT32 (* pi4JdecStop)(UINT16));
extern void ImgDecMw_RegisterJdecFrmEnd(INT32 (* pi4JdecFrmEnd)(UINT16));
extern void Jdec_RegisterJFIF_New(INT32 (* pi4JfifNew)(JPEGHANDLE, void *, UINT32));
extern void Jdec_RegisterJFIF_AllocateInstance(JPEGHANDLE (* pi4Jfif_AllocateInstance)(void));
extern void Vdec_RegisterJdecFrmStart(INT32 (* pi4JdecFrmStart)(INT16, IMG_FRM_START_T *));
extern void Vdec_RegisterJdecDecode(INT32 (* pi4JdecDecode)(UINT16, IMG_DECODE_T *));
extern void Vdec_RegisterJdecFrmEnd(INT32 (* pi4JdecFrmEnd)(UINT16));
#endif

INT32 i4JdecInit(void)
{
    INT32               i4 = 0;
    INT32               i4Count1, i4Count2;
    INT32               i4Ret = 0;
    UINT32              u4TaskId;
    CHAR                szEvName[16] = "JdecEv0";
    
#if JPG_SUPPORT_DIRECT_DECODE_MODE    
    CHAR                szDirectEvName[16] = "JpgDirectDecEv0";
#endif  
    //UINT32              u4ValTmp;

    if (_fgJdecInit)
    {
        return RMR_OK;
    }
    //zero instance data
    JdecZeroMemory(_rJdecInst, sizeof(_rJdecInst));
#if JDEC_CACHE_ENABLE    
    JdecZeroMemory(_rJdecPicInfo, sizeof(_rJdecPicInfo));
#endif

    //init instance
    for (i4=0; i4<JDEC_INST_NUM; i4++)
    {
        //init inst value
        _rJdecInst[i4].u2CompId = (UINT16)i4;
        _rJdecInst[i4].u4ImgdmaId = JDEC_INVALID32;
        _rJdecInst[i4].u4HwInstId = 0;
        if (i4 == 0)
        {
        #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
            _rJdecInst[i4].u4HwId = 1; // use hw 1 to decode
        #else
            _rJdecInst[i4].u4HwId = 0; // 8530 only has 1 jpg hw
        #endif
        }
        else
        {
            _rJdecInst[i4].u4HwId = JDEC_INVALID32; // use hw 1 to decode
        }
        _rJdecInst[i4].eJdecState = JDEC_STATE_IDLE;
        _rJdecInst[i4].hJfifInst = NULL;
        _rJdecInst[i4].u4DecProgress = 0;
        _rJdecInst[i4].rImgReszTicket.u4Ticket = JDEC_INVALID32;
        _rJdecInst[i4].fgInitHal = FALSE;
        
        #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)        
        _rJdecInst[i4].u4VJDMHandle = JDEC_INVALID32;
        #endif

        //create fifo
#if 0
        _aru4FifoStart[i4] = (UINT32)x_alloc_aligned_nc_mem(JDEC_DRV_FIFO_SIZE, 128);
#else
        _aru4FifoStart[i4] = JDEC_INVALID32;
#endif

#if JDEC_SUPPORT_BROKEN_TARGET
        _rJdecInst[i4].u4TempAYUVBuf = JDEC_INVALID32;
        _rJdecInst[i4].u4TempLineBuf = JDEC_INVALID32;
#endif
        //create event
        szEvName[6] = (CHAR)(i4+0x30);
        i4Ret = x_ev_group_create(&_rJdecInst[i4].hJdecEvent, szEvName, JDEC_DRV_EV_INITIAL);
        VERIFY(i4Ret == OSR_OK);

        //create a semaphore
        i4Ret = x_sema_create(&_rJdecInst[i4].hJdecSema, X_SEMA_TYPE_MUTEX, 1);
        VERIFY(i4Ret == OSR_OK);

        //create a semaphore
        i4Ret = x_sema_create(&_rJdecInst[i4].hJdecHwDevSema, X_SEMA_TYPE_BINARY, 1);
        VERIFY(i4Ret == OSR_OK);        

        //create a timer handle
        i4Ret = x_timer_create(&_rJdecInst[i4].hJdecTrnsTimer);
        VERIFY(i4Ret == OSR_OK);

        //create a timer handle
        i4Ret = x_timer_create(&_rJdecInst[i4].hJdecDecTimer);
        VERIFY(i4Ret == OSR_OK);

        i4Ret = x_timer_create(&_rJdecInst[i4].hJdecSleepTimer);

#if JPG_SUPPORT_DIRECT_DECODE_MODE    
        i4Ret = x_sema_create(&_rJdecInst[i4].hJdecSrcfifoSema, X_SEMA_TYPE_BINARY, 1);
        VERIFY(i4Ret == OSR_OK);
        szDirectEvName[14] = (CHAR)(i4+0x30);
        i4Ret = x_ev_group_create(&(_rJdecInst[i4].hJpgDirectDecEv),
                                  szDirectEvName,
                                  (EV_GRP_EVENT_T)0);
        VERIFY(i4Ret == OSR_OK);
#endif

        //allocate 3 clr buffer
#if 0
        u4ValTmp = (UINT32)x_alloc_aligned_nc_mem((JDEC_CLR_BUF_SIZE * JDEC_CLR_BUF_NUM * JDEC_COMP_NUM), 128);
        for (i4Count1=0; i4Count1<JDEC_CLR_BUF_NUM; i4Count1++)
        {
            for (i4Count2=0; i4Count2<JDEC_COMP_NUM; i4Count2++)
            {
                _rJdecInst[i4].au4ColorBufSa[i4Count1][i4Count2] = u4ValTmp + 
                            i4Count1 * JDEC_CLR_BUF_SIZE * JDEC_COMP_NUM + 
                            i4Count2 * JDEC_CLR_BUF_SIZE;
            }
        }
#else
        for (i4Count1=0; i4Count1<JDEC_CLR_BUF_NUM; i4Count1++)
        {
            for (i4Count2=0; i4Count2<JDEC_COMP_NUM; i4Count2++)
            {
                _rJdecInst[i4].au4ColorBufSa[i4Count1][i4Count2] = JDEC_INVALID32;
            }
        }
#endif
    }

    
    #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)    
    fgJDEC_HAL_SetIrq();
    #endif

    //init cache instance
#if 0    
    for (i4=0; i4<JDEC_PIC_INFO_CACHE_NUM; i4++)
    {
        _rJdecPicInfo[i4].eCacheState = JDEC_CACHE_INVALID;
        _rJdecPicInfo[i4].i4ReplaceCount = 0;
        _rJdecPicInfo[i4].u4ImgId = JDEC_INVALID32;
    }
#endif    

    // create a semaphore
    i4Ret = x_sema_create(&_hJdecCacheSema, X_SEMA_TYPE_MUTEX, 1);
    VERIFY(i4Ret == OSR_OK);

    //create thread
    u4TaskId = 0;
    i4Ret = x_thread_create (&(_rJdecInst[0].hJdecInst),
                             JDEC0_THREAD_NAME,
                             JDEC_STACK_SIZE,     /* stack size */
                             JDEC_THREAD_PRIORITY,      /* priority */
                             vJdecMain,
                             sizeof(UINT32),
                             (void *)&u4TaskId);
    VERIFY(i4Ret == OSR_OK);

    //SW Jpeg Decode
    u4TaskId = 1;
    i4Ret = x_thread_create (&(_rJdecInst[1].hJdecInst),
                             JDEC1_THREAD_NAME,
                             JDEC_STACK_SIZE,     /* stack size */
                             200,      /* priority */
                             vJdecMain,
                             sizeof(UINT32),
                             (void *)&u4TaskId);
    VERIFY(i4Ret == OSR_OK);
 
    _rJdecInst[0].ptImgJpgObj = NULL;
#if !CONFIG_DRV_VERIFY_SUPPORT    
    _rJdecInst[1].ptImgJpgObj = NULL;//&(_tImgJpgObj);
#endif

    #if CONFIG_SUSPEND_TO_DRAM
    register_pm_ops(&jdec_pm_ops);
    #endif
    _fgJdecInit = TRUE;

#if CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN
    vImgdmaRegisterJdecFillBuf(i4JdecFillBuf);
    ImgDecMw_RegisterJdecFrmStart(i4JdecFrmStart);
    ImgDecMw_RegisterJdecGetProgress(i4JdecGetProgress);
    ImgDecMw_RegisterJdecDecode(i4JdecDecode);
    ImgDecMw_RegisterJdecStop(i4JdecStop);
    ImgDecMw_RegisterJdecFrmEnd(i4JdecFrmEnd);
    Jdec_RegisterJFIF_New(JFIF_New);
    Jdec_RegisterJFIF_AllocateInstance(JFIF_AllocInstance);
    Vdec_RegisterJdecFrmStart(i4JdecFrmStart);
    Vdec_RegisterJdecDecode(i4JdecDecode);
    Vdec_RegisterJdecFrmEnd(i4JdecFrmEnd);
#endif    
    

    return RMR_OK;
}

// For linux uninit
INT32 i4Jdec_Uninit(UINT32 u4Case)
{
    INT32               i4 = 0;
    INT32               i4Ret = 0;

    //delete a semaphore

    if (!_fgJdecInit)
    {
        return RMR_OK;
    }

    #if CONFIG_SUSPEND_TO_DRAM
    unregister_pm_ops(&jdec_pm_ops);
    #endif
    
    i4Ret = x_sema_delete(_hJdecCacheSema);
    VERIFY(i4Ret == OSR_OK);

    //delete thread
    i4Ret = x_ev_group_set_event(_rJdecInst[0].hJdecEvent, JDEC_DRV_EV_DEL_THREAD, X_EV_OP_OR);
    VERIFY(i4Ret == OSR_OK);
#if 1    
    i4Ret = x_ev_group_set_event(_rJdecInst[1].hJdecEvent, JDEC_DRV_EV_DEL_THREAD, X_EV_OP_OR);
    VERIFY(i4Ret == OSR_OK);
#endif

    
    #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)    
    fgJDEC_HAL_DisIrq();
    #endif
    
    while ((_rJdecInst[0].hJdecInst != JDEC_INVALID32) &&
           (_rJdecInst[1].hJdecInst != JDEC_INVALID32))
    {
        x_thread_delay(1);
    }

    //uninit instance
    for (i4=0; i4<JDEC_INST_NUM; i4++)
    {
        //delete event
        i4Ret = x_ev_group_delete(_rJdecInst[i4].hJdecEvent);
        VERIFY(i4Ret == OSR_OK);

        //delete a semaphore
        i4Ret = x_sema_delete(_rJdecInst[i4].hJdecSema);
        VERIFY(i4Ret == OSR_OK);

        //delete a semaphore
        i4Ret = x_sema_delete(_rJdecInst[i4].hJdecHwDevSema);
        VERIFY(i4Ret == OSR_OK);

        //delete a timer handle
        i4Ret = x_timer_delete(_rJdecInst[i4].hJdecTrnsTimer);
        VERIFY(i4Ret == OSR_OK);

        //delete a timer handle
        i4Ret = x_timer_delete(_rJdecInst[i4].hJdecDecTimer);
        VERIFY(i4Ret == OSR_OK);

        
        //delete a timer handle
        i4Ret = x_timer_delete(_rJdecInst[i4].hJdecSleepTimer);
        VERIFY(i4Ret == OSR_OK);
#if JPG_SUPPORT_DIRECT_DECODE_MODE    
        i4Ret = x_sema_delete(_rJdecInst[i4].hJdecSrcfifoSema);
        VERIFY(i4Ret == OSR_OK);
        i4Ret = x_ev_group_delete(_rJdecInst[i4].hJpgDirectDecEv);
        VERIFY(i4Ret == OSR_OK);
#endif
        
    }

    _fgJdecInit = FALSE;
    
    return RMR_OK;
}

INT32 i4JdecUnInit(void)
{
    i4Jdec_Uninit(0);
    return RMR_OK;
}

#if CONFIG_SUSPEND_TO_DRAM
extern void vHwJdecVLDPowerOn(UINT32 u4HwId,BOOL fgPowerOn);
int i4JDEC_Suspend(void *param)
{
    //INT32 i4Ret;
    //JDEC_INST_T *prJdecInst = &(_rJdecInst[0]);
	vHwJdecVLDPowerOn(0, FALSE);

	
    return 0;
}


int i4JDEC_Resume(void *param)
{
	vHwJdecVLDPowerOn(0, TRUE);

  return 0;
}
#endif



