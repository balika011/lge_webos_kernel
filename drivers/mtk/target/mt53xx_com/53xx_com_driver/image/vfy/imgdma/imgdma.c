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

#include "asm/uaccess.h"
#include "x_assert.h"
#include "x_os.h"
#include "drv_img_dma.h"
#include "imgdma.h"
#include "drv_thread.h"
#if UNIFORM_DRV_CALLBACK
#include "x_cb_agent.h"
#endif
//#include "x_rtos.h"
#include "drv_config.h"
#include <linux/module.h>

//#include "sys_config.h"
#include "x_kmem.h"

//#include "sys_config.h"

#if CONFIG_SYS_MEM_PHASE2
#include "x_mem_phase2.h"
#elif CONFIG_SYS_MEM_PHASE3
#include "x_kmem.h"
#endif

/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
static HANDLE_T     h_imgdma_sema = NULL_HANDLE;
IMGDMA_INST_T       _arImgdmaInst[IMGDMA_INST_NUM];  //for img dec drv using

BOOL                _fgImgdmaInit = FALSE;

#define IMGDMA_TRSN_UNIT        64*1024// 4*1024
#define IMGDMA_TRSN_THRSHOLD    IMGDMA_TRSN_UNIT * 2 //IMGDMA_TRSN_UNIT * 3

// for discontinuous source ,use copy_from_usr 
#define IMGDMA_COPY_UINT        64*1024
#define IMGDMA_COPY_THRSHOLD    IMGDMA_COPY_UINT * 2

extern INT32 i4JdecFillBuf(UINT16 u2CompId, IMG_BUF_FILLED_T *prBufFilled);
//extern INT32 i4GifFillBuf(UINT16 u2CompId, IMG_BUF_FILLED_T * prBufFilled);
//extern INT32 i4PngFillBuf(UINT16 u2CompId, IMG_BUF_FILLED_T * prBufFilled);
extern INT32 UTIL_Printf(const CHAR *ps_format, ...);

void vImgdmaInitInst(UINT32 u4Handle)
{
    _arImgdmaInst[u4Handle].eInstState = IMGDMA_INST_NOAVTICE;
#if UNIFORM_DRV_CALLBACK
    ImgdmaZeroMemory((void *)&_arImgdmaInst[u4Handle].rImgdecCbInfo,sizeof(DRV_CB_REG_INFO_T));
#else
    _arImgdmaInst[u4Handle].pfImgdecCb = NULL;
    _arImgdmaInst[u4Handle].pvImgdecTag = NULL;
#endif    
    _arImgdmaInst[u4Handle].u4TrsnId = IMGDMA_INVALID32;
    _arImgdmaInst[u4Handle].u4ImgId = IMGDMA_INVALID32;
    _arImgdmaInst[u4Handle].u4FifoStart = IMGDMA_INVALID32;
    _arImgdmaInst[u4Handle].u4FifoEnd = IMGDMA_INVALID32;
    _arImgdmaInst[u4Handle].u4RdPoint = IMGDMA_INVALID32;
    _arImgdmaInst[u4Handle].u4WrPoint = IMGDMA_INVALID32;
    _arImgdmaInst[u4Handle].u4RangeOfst = IMGDMA_INVALID32;
    _arImgdmaInst[u4Handle].u4RangeSize = IMGDMA_INVALID32;
    _arImgdmaInst[u4Handle].u4TrsnedSz = IMGDMA_INVALID32;
    _arImgdmaInst[u4Handle].fgTrsning = FALSE;
    _arImgdmaInst[u4Handle].u4ReqstSz = IMGDMA_INVALID32;
	_arImgdmaInst[u4Handle].fgFileEnd = FALSE;
}

void vImgdmaGenerateTrsnId(UINT32 u4Handle)
{
    UINT32 u4TrsnHandle = (u4Handle & 0xFF) << 24;
    UINT32 u4TrsnSubId = (_arImgdmaInst[u4Handle].u4TrsnId & 0xFFFFFF) + 1;

    if (u4TrsnSubId > 0xFFFFFF)
    {
        u4TrsnSubId = 0;
    }
    _arImgdmaInst[u4Handle].u4TrsnId = u4TrsnHandle + u4TrsnSubId;
}

INT32 i4ImgdamGetTicket(UINT32 *pu4Handle)
{
    INT32 i4 = 0;
    INT32 i4Ret = IMGDMA_DRV_FAIL;

    *pu4Handle = IMGDMA_INVALID32;

    x_sema_lock(h_imgdma_sema, X_SEMA_OPTION_WAIT);
    for(i4=0; i4<IMGDMA_INST_NUM; i4++)
    {
        if (_arImgdmaInst[i4].eInstState == IMGDMA_INST_NOAVTICE)
        {
            _arImgdmaInst[i4].eInstState = IMGDMA_INST_AVTICE;
            *pu4Handle = (UINT32)i4;
            i4Ret = IMGDMA_OK;
            break;
        }
    }
    x_sema_unlock(h_imgdma_sema);

    return i4Ret;
}
#if CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN
EXPORT_SYMBOL(i4ImgdamGetTicket);
#endif


INT32 i4ImgdmaSetRelTicket(UINT32 u4Handle)
{
    if (u4Handle >= IMGDMA_INST_NUM)
    {
        return IMGDMA_DRV_FAIL;
    }

    x_sema_lock(h_imgdma_sema, X_SEMA_OPTION_WAIT); 
    vImgdmaInitInst(u4Handle);
    x_sema_unlock(h_imgdma_sema);

    return IMGDMA_OK;
}
#if CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN
EXPORT_SYMBOL(i4ImgdmaSetRelTicket);
#endif


INT32 i4ImgdmaSetFifoInfo(UINT32 u4Handle, IMGDMA_DRV_INIT_INFO_T tImgdmaInitInfo)
{
    if (u4Handle >= IMGDMA_INST_NUM)
    {
        return IMGDMA_DRV_FAIL;
    }
    #if UNIFORM_DRV_CALLBACK
    ImgdmaCopyMemory(&(_arImgdmaInst[u4Handle].rImgdecCbInfo), &(tImgdmaInitInfo.rNfyInfo), sizeof(DRV_CB_REG_INFO_T));
    #else
    _arImgdmaInst[u4Handle].pfImgdecCb = tImgdmaInitInfo.pfImgDecCb;
    _arImgdmaInst[u4Handle].pvImgdecTag = tImgdmaInitInfo.pvImgDecTag;
    #endif
    _arImgdmaInst[u4Handle].u4FifoStart = tImgdmaInitInfo.u4FifoStart;
    _arImgdmaInst[u4Handle].u4FifoEnd = tImgdmaInitInfo.u4FifoEnd;
    _arImgdmaInst[u4Handle].u4ImgId = tImgdmaInitInfo.u4ImgId;
    _arImgdmaInst[u4Handle].u4RdPoint = tImgdmaInitInfo.u4FifoStart;
    _arImgdmaInst[u4Handle].u4WrPoint = tImgdmaInitInfo.u4FifoStart;
    _arImgdmaInst[u4Handle].pfImgUpdCb = tImgdmaInitInfo.pfImgUpdCb;
    _arImgdmaInst[u4Handle].hMem = tImgdmaInitInfo.hFifo;

    return IMGDMA_OK;
}
#if CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN
EXPORT_SYMBOL(i4ImgdmaSetFifoInfo);
#endif

INT32 i4ImgdmaSetSrcInfo(UINT32 u4Handle, IMGDMA_DRV_SRC_INFO_T *ptImgdmaSrcInfo)
{
    if (u4Handle >= IMGDMA_INST_NUM)
    {
        return IMGDMA_DRV_FAIL;
    }

    _arImgdmaInst[u4Handle].u4SrcSA = ptImgdmaSrcInfo->u4SrcSa;
    _arImgdmaInst[u4Handle].fgCopy = ptImgdmaSrcInfo->fgCpy;
    _arImgdmaInst[u4Handle].eType = ptImgdmaSrcInfo->eType;
    _arImgdmaInst[u4Handle].u2DrvCompId = ptImgdmaSrcInfo->u2DrvCompId;
    
    return IMGDMA_OK;
}
#if CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN
EXPORT_SYMBOL(i4ImgdmaSetSrcInfo);
#endif

INT32 i4ImgdmaSetRange(UINT32 u4Handle, UINT32 u4Offset, UINT32 u4Size)
{
    if (u4Handle >= IMGDMA_INST_NUM)
    {
        return IMGDMA_DRV_FAIL;
    }

    _arImgdmaInst[u4Handle].u4RangeOfst = u4Offset;
    _arImgdmaInst[u4Handle].u4RangeSize = u4Size;
    _arImgdmaInst[u4Handle].u4TrsnedSz = 0;
    _arImgdmaInst[u4Handle].u4ReqstSz = 0;
    _arImgdmaInst[u4Handle].fgTrsning = FALSE;
	_arImgdmaInst[u4Handle].fgFileEnd = FALSE;
    vImgdmaGenerateTrsnId(u4Handle);

    return IMGDMA_OK;
}
#if CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN
EXPORT_SYMBOL(i4ImgdmaSetRange);
#endif


INT32 i4ImgdmaSetReset(UINT32 u4Handle)
{
	//  INT32 u4Cnt =0;
    if (u4Handle >= IMGDMA_INST_NUM)
    {
        return IMGDMA_DRV_FAIL;
    }

    x_sema_lock(_arImgdmaInst[u4Handle].hSemaHandle, X_SEMA_OPTION_WAIT);
#if 0
	while(_arImgdmaInst[u4Handle].fgFillBuffer && (u4Cnt<600))
    {
    	 x_thread_delay(5);
    	 u4Cnt++;
    }
    if(u4Cnt>=600)
    {
    	UTIL_Printf("[IMGDMA]--] wait count[0x%x]\n", u4Cnt);	
    }
#endif	
    _arImgdmaInst[u4Handle].u4RdPoint = _arImgdmaInst[u4Handle].u4FifoStart;
    _arImgdmaInst[u4Handle].u4WrPoint = _arImgdmaInst[u4Handle].u4FifoStart;
    _arImgdmaInst[u4Handle].u4RangeOfst = IMGDMA_INVALID32;
    _arImgdmaInst[u4Handle].u4RangeSize = IMGDMA_INVALID32;
    _arImgdmaInst[u4Handle].u4TrsnedSz = IMGDMA_INVALID32;
    _arImgdmaInst[u4Handle].u4ReqstSz = IMGDMA_INVALID32;
    _arImgdmaInst[u4Handle].fgTrsning = FALSE;
    _arImgdmaInst[u4Handle].u4SrcSA = IMGDMA_INVALID32;
    _arImgdmaInst[u4Handle].fgCopy = FALSE;
	_arImgdmaInst[u4Handle].fgFileEnd = FALSE;
    vImgdmaGenerateTrsnId(u4Handle);
    x_sema_unlock(_arImgdmaInst[u4Handle].hSemaHandle);
    return IMGDMA_OK;
}

#if CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN
EXPORT_SYMBOL(i4ImgdmaSetReset);
#endif


INT32 i4ImgdmaSetRdPoint(UINT32 u4Handle, UINT32 u4ReadPoint)
{
    INT32 i4Ret;

    if (u4Handle >= IMGDMA_INST_NUM)
    {
        return IMGDMA_DRV_FAIL;
    }

    x_sema_lock(_arImgdmaInst[u4Handle].hSemaHandle, X_SEMA_OPTION_WAIT);
    _arImgdmaInst[u4Handle].u4RdPoint = u4ReadPoint;

    if (_arImgdmaInst[u4Handle].u4ReqstSz < _arImgdmaInst[u4Handle].u4RangeSize)
    {
        //sent a event
        i4Ret = x_ev_group_set_event(_arImgdmaInst[u4Handle].hEventHandle, IMGDMA_EV_TRSN_DATA, X_EV_OP_OR);
        VERIFY(i4Ret == OSR_OK);
    }
    x_sema_unlock(_arImgdmaInst[u4Handle].hSemaHandle);

    return IMGDMA_OK;
}

#if CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN
EXPORT_SYMBOL(i4ImgdmaSetRdPoint);
#endif


INT32 i4ImgdmaSetRdPointOnly(UINT32 u4Handle, UINT32 u4ReadPoint)
{
    if (u4Handle >= IMGDMA_INST_NUM)
    {
        return IMGDMA_DRV_FAIL;
    }

    x_sema_lock(_arImgdmaInst[u4Handle].hSemaHandle, X_SEMA_OPTION_WAIT);
    _arImgdmaInst[u4Handle].u4RdPoint = u4ReadPoint;

    x_sema_unlock(_arImgdmaInst[u4Handle].hSemaHandle);

    return IMGDMA_OK;
}
#if CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN
EXPORT_SYMBOL(i4ImgdmaSetRdPointOnly);
#endif


INT32 i4ImgdmaGetFifoStatus(UINT32 u4Handle, IMGDMA_DRV_FIFO_STATUS_T *ptFifoStatus)
{
    if (u4Handle >= IMGDMA_INST_NUM)
    {
        return IMGDMA_DRV_FAIL;
    }

    x_sema_lock(_arImgdmaInst[u4Handle].hSemaHandle, X_SEMA_OPTION_WAIT);
	ptFifoStatus->fgEoRange = FALSE;
    if ((_arImgdmaInst[u4Handle].fgTrsning == FALSE) && 
        (_arImgdmaInst[u4Handle].u4TrsnedSz >= _arImgdmaInst[u4Handle].u4RangeSize))
    {
        ptFifoStatus->fgEoRange = TRUE;
    }
	if(_arImgdmaInst[u4Handle].fgFileEnd == TRUE)
	{
	   ptFifoStatus->fgEoRange = TRUE;
    }
#if 0
    else
    {
        ptFifoStatus->fgEoRange = FALSE;
    }
#endif
    ptFifoStatus->u4RdPoint = _arImgdmaInst[u4Handle].u4RdPoint;
    ptFifoStatus->u4WrPoint = _arImgdmaInst[u4Handle].u4WrPoint;

    if (ptFifoStatus->u4RdPoint > ptFifoStatus->u4WrPoint)
    {
        ptFifoStatus->u4ValidData = _arImgdmaInst[u4Handle].u4FifoEnd - ptFifoStatus->u4RdPoint;
        ptFifoStatus->u4ValidData += ptFifoStatus->u4WrPoint - _arImgdmaInst[u4Handle].u4FifoStart;
    }
    else
    {
        ptFifoStatus->u4ValidData = ptFifoStatus->u4WrPoint - ptFifoStatus->u4RdPoint;
    }
    x_sema_unlock(_arImgdmaInst[u4Handle].hSemaHandle);

    return IMGDMA_OK;
}

#if CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN
EXPORT_SYMBOL(i4ImgdmaGetFifoStatus);
#endif


//extern int UTIL_Printf(const CHAR *format, ...);

#if CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN
#include <linux/module.h>
INT32 (*_pi4GifFillBuf)(UINT16, IMG_BUF_FILLED_T *);
void vImgdmaRegisterGifFillBuf(INT32 (*pi4GifFillBuf)(UINT16, IMG_BUF_FILLED_T *))
{
  _pi4GifFillBuf = pi4GifFillBuf;
}
EXPORT_SYMBOL(vImgdmaRegisterGifFillBuf);

INT32 (*_pi4JdecFillBuf)(UINT16, IMG_BUF_FILLED_T *);
void vImgdmaRegisterJdecFillBuf(INT32 (*pi4JdecFillBuf)(UINT16, IMG_BUF_FILLED_T *))
{
  _pi4JdecFillBuf = pi4JdecFillBuf;
}
EXPORT_SYMBOL(vImgdmaRegisterJdecFillBuf);
#endif

void vImgdmaCopyProc(UINT32 u4Handle, UINT32 u4TrsnSz)
{
    IMG_BUF_FILLED_T tBufFilled;

    
    if ((_arImgdmaInst[u4Handle].u4WrPoint + u4TrsnSz) > _arImgdmaInst[u4Handle].u4FifoEnd)
    {
        u4TrsnSz = _arImgdmaInst[u4Handle].u4FifoEnd - _arImgdmaInst[u4Handle].u4WrPoint;
    }
    if (u4TrsnSz > IMGDMA_COPY_UINT)
    {
        u4TrsnSz = IMGDMA_COPY_UINT;
    }
    //UTIL_Printf("[IMGDMA] Copy %d bytes from %x to %x \n",u4TrsnSz,(_arImgdmaInst[u4Handle].u4TrsnedSz + _arImgdmaInst[u4Handle].u4SrcSA),_arImgdmaInst[u4Handle].u4WrPoint);
#if  CONFIG_DRV_LINUX_DATA_CONSISTENCY
    if (copy_from_user((void *)_arImgdmaInst[u4Handle].u4WrPoint , (void *)(_arImgdmaInst[u4Handle].u4TrsnedSz + _arImgdmaInst[u4Handle].u4SrcSA), u4TrsnSz))
    {
        ASSERT(0);
    }
#else
    x_memcpy(_arImgdmaInst[u4Handle].u4FifoStart, _arImgdmaInst[u4Handle].u4RangeOfst + _arImgdmaInst[u4Handle].u4SrcSA, u4TrsnSz);
#endif
    _arImgdmaInst[u4Handle].u4ReqstSz += u4TrsnSz;
    _arImgdmaInst[u4Handle].fgTrsning = TRUE;
    
    x_memset(&tBufFilled, 0, sizeof(IMG_BUF_FILLED_T));
    if (_arImgdmaInst[u4Handle].u4ReqstSz >= _arImgdmaInst[u4Handle].u4RangeSize)
    {
        tBufFilled.i4_ret = IMG_MW_FILL_EOF;
    }
    else
    {
        tBufFilled.i4_ret = IMG_MW_FILL_OK;
    }
    tBufFilled.ui4_filled_len = u4TrsnSz;
    tBufFilled.ui4_trsn_id = _arImgdmaInst[u4Handle].u4TrsnId;

    switch (_arImgdmaInst[u4Handle].eType)
    {
    case DRVT_JPG_DEC:
        #if CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN
        if(_pi4JdecFillBuf != NULL)
            _pi4JdecFillBuf(_arImgdmaInst[u4Handle].u2DrvCompId, &tBufFilled);
        #else
        i4JdecFillBuf(_arImgdmaInst[u4Handle].u2DrvCompId, &tBufFilled);
        #endif
        break;
    case DRVT_PNG_DEC:
       
    case DRVT_GIF_DEC:
      
    default:
        ASSERT(0);    
    }
}

INT32 i4ImgdmaSetStart(UINT32 u4Handle)
{
    #if !UNIFORM_DRV_CALLBACK
    IMG_FILL_BUF_T  tImgDrvFillBuf;
    #endif
    UINT32              u4TrsnSz;
     
    if (u4Handle >= IMGDMA_INST_NUM)
    {
        return IMGDMA_DRV_FAIL;
    }

    u4TrsnSz = _arImgdmaInst[u4Handle].u4RangeSize - _arImgdmaInst[u4Handle].u4ReqstSz;
    if (_arImgdmaInst[u4Handle].fgCopy)
    {
#if 0        
        vImgdmaCopyProc(u4Handle,u4TrsnSz);
        return IMGDMA_OK;
#else
        if (u4TrsnSz > IMGDMA_COPY_UINT)
            u4TrsnSz = IMGDMA_COPY_UINT;
#endif
    }
    else
    {
        if ((u4TrsnSz) > IMGDMA_TRSN_UNIT)
        {
            u4TrsnSz = IMGDMA_TRSN_UNIT;
        }
    }
    _arImgdmaInst[u4Handle].fgFillBuffer = TRUE;
    #if !UNIFORM_DRV_CALLBACK
    tImgDrvFillBuf.ui4_trsn_id = _arImgdmaInst[u4Handle].u4TrsnId;
    #if CONFIG_DRV_ONLY
    tImgDrvFillBuf.pv_start_add = ((void *)(_arImgdmaInst[u4Handle].u4FifoStart));
    #else
#if CONFIG_SYS_MEM_PHASE2
    tImgDrvFillBuf.pv_start_add = addr_kernel_to_user((void *)(_arImgdmaInst[u4Handle].u4FifoStart));
#elif CONFIG_SYS_MEM_PHASE3
    if (_arImgdmaInst[u4Handle].fgCopy)
    {
        tImgDrvFillBuf.pv_start_add = _arImgdmaInst[u4Handle].u4SrcSA;
    }
    else
    {
        tImgDrvFillBuf.pv_start_add = (void *)x_kmem_block_handle((void *)(_arImgdmaInst[u4Handle].u4FifoStart));
    }
    tImgDrvFillBuf.u4Offset = 0;

#endif
    #endif
    tImgDrvFillBuf.ui4_required_len = u4TrsnSz;
    tImgDrvFillBuf.b_reset_pos = TRUE;
    tImgDrvFillBuf.ui4_position = _arImgdmaInst[u4Handle].u4RangeOfst;
    #endif

    _arImgdmaInst[u4Handle].fgTrsning = TRUE;
    #if UNIFORM_DRV_CALLBACK
    {
        IMG_NFY_PARAM_T rNfyInfo;

        ImgdmaZeroMemory(&rNfyInfo, sizeof(IMG_NFY_PARAM_T));
        rNfyInfo.e_state = IMG_NFY_FILL_BUF;
        rNfyInfo.ui4_img_id = _arImgdmaInst[u4Handle].u4ImgId;
        // For fill buffer info
        rNfyInfo.rInfo.rFillBuf.ui4_trsn_id = _arImgdmaInst[u4Handle].u4TrsnId;
#if CONFIG_SYS_MEM_PHASE2
        rNfyInfo.rInfo.rFillBuf.pv_start_add = addr_kernel_to_user((void *)(_arImgdmaInst[u4Handle].u4FifoStart));
#elif CONFIG_SYS_MEM_PHASE3
        rNfyInfo.rInfo.rFillBuf.pv_start_add = (void *)(_arImgdmaInst[u4Handle].hMem);//(void *)x_kmem_block_handle((void *)(_arImgdmaInst[u4Handle].u4FifoStart));
        rNfyInfo.rInfo.rFillBuf.u4Offset = 0;
#endif
        rNfyInfo.rInfo.rFillBuf.ui4_required_len = u4TrsnSz;
        rNfyInfo.rInfo.rFillBuf.b_reset_pos = TRUE;
        rNfyInfo.rInfo.rFillBuf.ui4_position = _arImgdmaInst[u4Handle].u4RangeOfst;

        
        vCBAgent_SyncCb_Nfy(_arImgdmaInst[u4Handle].rImgdecCbInfo.pf_cb_nfy_fct,
                        _arImgdmaInst[u4Handle].rImgdecCbInfo.pv_nfy_tag,
                        (VOID *)(&rNfyInfo),
                        sizeof(IMG_NFY_PARAM_T));
        #if PNG_CLI_CALLBACK_SUPPORT	
		rNfyInfo.rInfo.rFillBuf.pv_start_add = (void *)(_arImgdmaInst[u4Handle].u4FifoStart);
        _arImgdmaInst[u4Handle].rImgdecCbInfo.pf_cb_nfy_fct((VOID *)(&rNfyInfo),(VOID*)(_arImgdmaInst[u4Handle].rImgdecCbInfo.pv_nfy_tag) );
        #endif
    }
    #else
    _arImgdmaInst[u4Handle].pfImgdecCb(_arImgdmaInst[u4Handle].u4ImgId,
                                       _arImgdmaInst[u4Handle].pvImgdecTag,
                                       (void *)&tImgDrvFillBuf,
                                       IMG_NFY_FILL_BUF);
    #endif
    _arImgdmaInst[u4Handle].u4ReqstSz += u4TrsnSz;

    return IMGDMA_OK;
}
#if CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN
EXPORT_SYMBOL(i4ImgdmaSetStart);
#endif


INT32 i4ImgdmaSetBufFilled(UINT32 u4Handle, IMG_BUF_FILLED_T *ptBufFilledInfo)
{
    INT32 i4Ret = IMGDMA_OK;
    UINT32 u4Tmp;

    if (u4Handle >= IMGDMA_INST_NUM)
    {
        return IMGDMA_DRV_FAIL;
    }
    if (_arImgdmaInst[u4Handle].fgTrsning == FALSE)
    {
        return IMGDMA_DRV_FAIL;
    }
	  _arImgdmaInst[u4Handle].fgFillBuffer = FALSE;
    x_sema_lock(_arImgdmaInst[u4Handle].hSemaHandle, X_SEMA_OPTION_WAIT);
    if (ptBufFilledInfo->ui4_trsn_id == _arImgdmaInst[u4Handle].u4TrsnId)
    {
        _arImgdmaInst[u4Handle].fgTrsning = FALSE;
        switch (ptBufFilledInfo->i4_ret)
        {
            case IMG_MW_FILL_OK:
                _arImgdmaInst[u4Handle].u4TrsnedSz += ptBufFilledInfo->ui4_filled_len;
                u4Tmp = _arImgdmaInst[u4Handle].u4WrPoint + ptBufFilledInfo->ui4_filled_len;
                if (u4Tmp == _arImgdmaInst[u4Handle].u4FifoEnd)
                {
                    _arImgdmaInst[u4Handle].u4WrPoint = _arImgdmaInst[u4Handle].u4FifoStart;
                }
                else if (u4Tmp < _arImgdmaInst[u4Handle].u4FifoEnd)
                {
                    _arImgdmaInst[u4Handle].u4WrPoint = u4Tmp;
                }
                else
                {
                    ASSERT(0);
                }
                //sent a event
                i4Ret = x_ev_group_set_event(_arImgdmaInst[u4Handle].hEventHandle, IMGDMA_EV_TRSN_DATA, X_EV_OP_OR);
                VERIFY(i4Ret == OSR_OK);
                //Call the driver update call back function
                if (_arImgdmaInst[u4Handle].pfImgUpdCb != NULL)
                {
                    // used for jpg only
                    BOOL fgSend = FALSE;
                    UINT32 u4Id = 0;

                    
                    _arImgdmaInst[u4Handle].pfImgUpdCb((void *)(&fgSend),(void *)u4Id); // temp usage.
                }
                //UTIL_Printf("[IMGDMA] Imgdma%d fill ok ,w_ptr update to %x, trans %x bytes \n", u4Handle,_arImgdmaInst[u4Handle].u4WrPoint,_arImgdmaInst[u4Handle].u4TrsnedSz);
                break;
            case IMG_MW_FILL_EOF:
                u4Tmp = _arImgdmaInst[u4Handle].u4WrPoint + ptBufFilledInfo->ui4_filled_len;
                if (u4Tmp == _arImgdmaInst[u4Handle].u4FifoEnd)
                {
                    _arImgdmaInst[u4Handle].u4WrPoint = _arImgdmaInst[u4Handle].u4FifoStart;
                }
                else
                { 
                    _arImgdmaInst[u4Handle].u4WrPoint = u4Tmp;
                }
                _arImgdmaInst[u4Handle].u4TrsnedSz = _arImgdmaInst[u4Handle].u4RangeSize;

                //Call the driver update call back function
                if (_arImgdmaInst[u4Handle].pfImgUpdCb != NULL)
                {
                    // used for jpg only
                    BOOL fgSend = FALSE;
                    UINT32 u4Id = 0;
                    
                    _arImgdmaInst[u4Handle].pfImgUpdCb((void *)(&fgSend),(void *)u4Id); // temp usage.
                }
			    _arImgdmaInst[u4Handle].fgFileEnd = TRUE;
                //UTIL_Printf("[IMGDMA] Imgdma%d fill eof ,w_ptr update to %x ,trans %x bytes \n", u4Handle,_arImgdmaInst[u4Handle].u4WrPoint,_arImgdmaInst[u4Handle].u4TrsnedSz);
                i4Ret = IMGDMA_DRV_EOF;
                break;
            case IMG_MW_FILL_ERROR:
                _arImgdmaInst[u4Handle].u4TrsnedSz = _arImgdmaInst[u4Handle].u4RangeSize;
                i4Ret = IMGDMA_DRV_IO_ERR;
                break;
            default:
                ASSERT(0);
                i4Ret = IMGDMA_DRV_FAIL;
                break;
        }
    }
    else
    {
        i4Ret = IMGDMA_DRV_FAIL;
    }
    x_sema_unlock(_arImgdmaInst[u4Handle].hSemaHandle);

    return i4Ret;
}
#if CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN
EXPORT_SYMBOL(i4ImgdmaSetBufFilled);
#endif


BOOL fgImgdmaIsTransing(UINT32 u4Handle)
{
    if (u4Handle >= IMGDMA_INST_NUM)
    {
        return FALSE;
    }
    return _arImgdmaInst[u4Handle].fgTrsning;
}
#if CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN
EXPORT_SYMBOL(fgImgdmaIsTransing);
#endif


void vImgdmaMain(void *arg)
{
    INT32               i4Ret = 1;
    UINT32              u4Handle = *((UINT32 *)arg);
    EV_GRP_EVENT_T      eImgdmaEvent;
    EV_GRP_EVENT_T      eImgdmaEventGet;
    UINT32              u4FreeFifoSpace;
    UINT32              u4TrsnSz;
    #if !UNIFORM_DRV_CALLBACK
    IMG_FILL_BUF_T      tImgDrvFillBuf;
    #endif

    while (1)
    {
        eImgdmaEvent = IMGDMA_EV_TRSN_DATA | IMGDMA_EV_DEL_THREAD;
        i4Ret = x_ev_group_wait_event(_arImgdmaInst[u4Handle].hEventHandle, eImgdmaEvent, &eImgdmaEventGet, X_EV_OP_OR_CONSUME);
        VERIFY(i4Ret == OSR_OK);

        if (eImgdmaEventGet & IMGDMA_EV_TRSN_DATA)
        {
            //transfer data
            x_sema_lock(_arImgdmaInst[u4Handle].hSemaHandle, X_SEMA_OPTION_WAIT);
            if ((_arImgdmaInst[u4Handle].fgTrsning == FALSE) && 
                (_arImgdmaInst[u4Handle].u4ReqstSz < _arImgdmaInst[u4Handle].u4RangeSize) &&
                (_arImgdmaInst[u4Handle].u4RangeOfst != IMGDMA_INVALID32))
            {
                if (_arImgdmaInst[u4Handle].u4WrPoint > _arImgdmaInst[u4Handle].u4RdPoint)
                {
                    u4FreeFifoSpace = _arImgdmaInst[u4Handle].u4FifoEnd - _arImgdmaInst[u4Handle].u4WrPoint;
                    u4FreeFifoSpace += _arImgdmaInst[u4Handle].u4RdPoint - _arImgdmaInst[u4Handle].u4FifoStart;
                }
                else if (_arImgdmaInst[u4Handle].u4WrPoint < _arImgdmaInst[u4Handle].u4RdPoint)
                {
                    u4FreeFifoSpace = _arImgdmaInst[u4Handle].u4RdPoint - _arImgdmaInst[u4Handle].u4WrPoint;
                }
                else
                {
                    u4FreeFifoSpace = _arImgdmaInst[u4Handle].u4FifoEnd - _arImgdmaInst[u4Handle].u4FifoStart;
                }
                
                if (((!_arImgdmaInst[u4Handle].fgCopy) && (u4FreeFifoSpace >= IMGDMA_TRSN_THRSHOLD)) ||
                    (_arImgdmaInst[u4Handle].fgCopy && (u4FreeFifoSpace >= IMGDMA_COPY_THRSHOLD)))
                {
                	  _arImgdmaInst[u4Handle].fgFillBuffer = TRUE;
                    u4TrsnSz = _arImgdmaInst[u4Handle].u4RangeSize - _arImgdmaInst[u4Handle].u4ReqstSz;
                    if (_arImgdmaInst[u4Handle].fgCopy)
                    {                   
                        #if 0
                        vImgdmaCopyProc(u4Handle, u4TrsnSz);
                        #else
                        if (u4TrsnSz > IMGDMA_COPY_UINT)
                            u4TrsnSz = IMGDMA_COPY_UINT;
                        #endif
                    }
                    else
                    {
                        if ((u4TrsnSz) > IMGDMA_TRSN_UNIT)
                        {
                            u4TrsnSz = IMGDMA_TRSN_UNIT;
                        }
                    }
                    
                        if ((_arImgdmaInst[u4Handle].u4WrPoint + u4TrsnSz) > _arImgdmaInst[u4Handle].u4FifoEnd)
                        {
                            u4TrsnSz = _arImgdmaInst[u4Handle].u4FifoEnd - _arImgdmaInst[u4Handle].u4WrPoint;
                        }
                        #if !UNIFORM_DRV_CALLBACK
                        //trsn data
                        tImgDrvFillBuf.ui4_trsn_id = _arImgdmaInst[u4Handle].u4TrsnId;
                        #if CONFIG_DRV_ONLY
                        tImgDrvFillBuf.pv_start_add = ((void *)(_arImgdmaInst[u4Handle].u4WrPoint));
                        #else
#if CONFIG_SYS_MEM_PHASE2
                        tImgDrvFillBuf.pv_start_add = addr_kernel_to_user((void *)(_arImgdmaInst[u4Handle].u4WrPoint));
#elif CONFIG_SYS_MEM_PHASE3
                        tImgDrvFillBuf.pv_start_add = (void *)x_kmem_block_handle((void *)(_arImgdmaInst[u4Handle].u4WrPoint));
                         tImgDrvFillBuf.u4Offset = _arImgdmaInst[u4Handle].u4WrPoint - _arImgdmaInst[u4Handle].u4FifoStart;
#endif
                        #endif
                        tImgDrvFillBuf.ui4_required_len = u4TrsnSz;
                        tImgDrvFillBuf.b_reset_pos = FALSE;
                        tImgDrvFillBuf.ui4_position = _arImgdmaInst[u4Handle].u4RangeOfst + _arImgdmaInst[u4Handle].u4ReqstSz;
                        #endif

                        //must set fgTrsning before u4TrsnedSz
                        _arImgdmaInst[u4Handle].fgTrsning = TRUE;
                        #if UNIFORM_DRV_CALLBACK
                        {
                            IMG_NFY_PARAM_T rNfyInfo;                           

                            ImgdmaZeroMemory(&rNfyInfo, sizeof(IMG_NFY_PARAM_T));
                            rNfyInfo.e_state = IMG_NFY_FILL_BUF;
                            rNfyInfo.ui4_img_id = _arImgdmaInst[u4Handle].u4ImgId;
                            
                            // For fill buffer info
                            rNfyInfo.rInfo.rFillBuf.ui4_trsn_id = _arImgdmaInst[u4Handle].u4TrsnId;
#if CONFIG_SYS_MEM_PHASE2 
                            rNfyInfo.rInfo.rFillBuf.pv_start_add = addr_kernel_to_user((void *)(_arImgdmaInst[u4Handle].u4WrPoint));
#elif CONFIG_SYS_MEM_PHASE3
                        rNfyInfo.rInfo.rFillBuf.pv_start_add = (void *)(_arImgdmaInst[u4Handle].hMem);//(void *)x_kmem_block_handle((void *)(_arImgdmaInst[u4Handle].u4WrPoint));
                        rNfyInfo.rInfo.rFillBuf.u4Offset = _arImgdmaInst[u4Handle].u4WrPoint - _arImgdmaInst[u4Handle].u4FifoStart;
#endif
                            rNfyInfo.rInfo.rFillBuf.ui4_required_len = u4TrsnSz;
                            rNfyInfo.rInfo.rFillBuf.b_reset_pos = FALSE;
                            rNfyInfo.rInfo.rFillBuf.ui4_position = _arImgdmaInst[u4Handle].u4RangeOfst + _arImgdmaInst[u4Handle].u4ReqstSz;
                            
                            vCBAgent_SyncCb_Nfy(_arImgdmaInst[u4Handle].rImgdecCbInfo.pf_cb_nfy_fct,
                            _arImgdmaInst[u4Handle].rImgdecCbInfo.pv_nfy_tag,
                            (VOID *)(&rNfyInfo),
                            sizeof(IMG_NFY_PARAM_T));
                            #if PNG_CLI_CALLBACK_SUPPORT	
                            rNfyInfo.rInfo.rFillBuf.pv_start_add = (void *)(_arImgdmaInst[u4Handle].u4WrPoint);
                            _arImgdmaInst[u4Handle].rImgdecCbInfo.pf_cb_nfy_fct((VOID *)(&rNfyInfo),(VOID*)(_arImgdmaInst[u4Handle].rImgdecCbInfo.pv_nfy_tag) );
                            #endif
                        }
                        #else
                        _arImgdmaInst[u4Handle].pfImgdecCb(_arImgdmaInst[u4Handle].u4ImgId,
                                                           _arImgdmaInst[u4Handle].pvImgdecTag,
                                                           (void *)&tImgDrvFillBuf,
                                                           IMG_NFY_FILL_BUF);
                        #endif
                        //_arImgdmaInst[u4Handle].u4TrsnedSz += u4TrsnSz;
                        _arImgdmaInst[u4Handle].u4ReqstSz += u4TrsnSz;
                    }
                }
            x_sema_unlock(_arImgdmaInst[u4Handle].hSemaHandle);
        }
        if (eImgdmaEventGet & IMGDMA_EV_DEL_THREAD)
        {
        	_arImgdmaInst[u4Handle].fgFillBuffer = FALSE;
            _arImgdmaInst[u4Handle].hInstHandle = IMGDMA_INVALID32;
            return;
        }
    }
}

INT32 i4ImgdmaInit(void)
{
    INT32   i4Ret;
    INT32   i4;
    UINT32  u4_thread_id;
    CHAR    szEvName[16] = "ImgdmaEv0";

    if (_fgImgdmaInit)
    {
        return IMGDMA_OK;
    }

    //zero instance data
    ImgdmaZeroMemory(_arImgdmaInst, sizeof(_arImgdmaInst));
    
    for (i4=0; i4<IMGDMA_INST_NUM; i4++)
    {
        _arImgdmaInst[i4].u2CompId = (UINT16)i4;
        vImgdmaInitInst((UINT32)i4);

        //create a event
        szEvName[8] = (CHAR)(i4+0x30);
        i4Ret = x_ev_group_create(&_arImgdmaInst[i4].hEventHandle, szEvName, IMGDMA_EV_INITIAL);
        VERIFY(i4Ret == OSR_OK);

        // create a semaphore
        i4Ret = x_sema_create(&_arImgdmaInst[i4].hSemaHandle, X_SEMA_TYPE_MUTEX, 1);
        VERIFY(i4Ret == OSR_OK);
    }

    // create a semaphore
    i4Ret = x_sema_create(&h_imgdma_sema, X_SEMA_TYPE_MUTEX, 1);
    VERIFY(i4Ret == OSR_OK);

    //create a task
    u4_thread_id = 0;
    i4Ret = x_thread_create (&_arImgdmaInst[0].hInstHandle,
                              IMGDMA0_THREAD_NAME,
                              IMGDMA_STACK_SIZE,     /* stack size */
                              IMGDMA_THREAD_PRIORITY,      /* priority */
                              vImgdmaMain,
                              sizeof(UINT32),
                              (VOID*)&u4_thread_id);
    VERIFY(i4Ret == OSR_OK);
    u4_thread_id = 1;
    i4Ret = x_thread_create (&_arImgdmaInst[1].hInstHandle,
                              IMGDMA1_THREAD_NAME,
                              IMGDMA_STACK_SIZE,     /* stack size */
                              IMGDMA_THREAD_PRIORITY,      /* priority */
                              vImgdmaMain,
                              sizeof(UINT32),
                              (VOID*)&u4_thread_id);
    VERIFY(i4Ret == OSR_OK);
    u4_thread_id = 2;
    i4Ret = x_thread_create (&_arImgdmaInst[2].hInstHandle,
                              IMGDMA2_THREAD_NAME,
                              IMGDMA_STACK_SIZE,     /* stack size */
                              IMGDMA_THREAD_PRIORITY,      /* priority */
                              vImgdmaMain,
                              sizeof(UINT32),
                              (VOID*)&u4_thread_id);
    VERIFY(i4Ret == OSR_OK);
    u4_thread_id = 3;
    i4Ret = x_thread_create (&_arImgdmaInst[3].hInstHandle,
                              IMGDMA3_THREAD_NAME,
                              IMGDMA_STACK_SIZE,     /* stack size */
                              IMGDMA_THREAD_PRIORITY,      /* priority */
                              vImgdmaMain,
                              sizeof(UINT32),
                              (VOID*)&u4_thread_id);
    VERIFY(i4Ret == OSR_OK);

    _fgImgdmaInit = TRUE;
    
    return IMGDMA_OK;
}
#if CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN
EXPORT_SYMBOL(i4ImgdmaInit);
#endif


INT32 i4Imgdma_UnInit(UINT32 u4Case)
{
    INT32   i4Ret;
    INT32   i4;

    if (!_fgImgdmaInit)
    {
        return IMGDMA_OK;
    }
    //delete a task
    i4Ret = x_ev_group_set_event(_arImgdmaInst[0].hEventHandle, IMGDMA_EV_DEL_THREAD, X_EV_OP_OR);
    VERIFY(i4Ret == OSR_OK);
    i4Ret = x_ev_group_set_event(_arImgdmaInst[1].hEventHandle, IMGDMA_EV_DEL_THREAD, X_EV_OP_OR);
    VERIFY(i4Ret == OSR_OK);
    i4Ret = x_ev_group_set_event(_arImgdmaInst[2].hEventHandle, IMGDMA_EV_DEL_THREAD, X_EV_OP_OR);
    VERIFY(i4Ret == OSR_OK);
    i4Ret = x_ev_group_set_event(_arImgdmaInst[3].hEventHandle, IMGDMA_EV_DEL_THREAD, X_EV_OP_OR);
    VERIFY(i4Ret == OSR_OK);

    while ((_arImgdmaInst[0].hInstHandle != IMGDMA_INVALID32) &&
           (_arImgdmaInst[1].hInstHandle != IMGDMA_INVALID32) &&
           (_arImgdmaInst[2].hInstHandle != IMGDMA_INVALID32) &&
           (_arImgdmaInst[3].hInstHandle != IMGDMA_INVALID32))
    {
        x_thread_delay(1);
    }

    // delete a semaphore
    i4Ret = x_sema_delete(h_imgdma_sema);
    VERIFY(i4Ret == OSR_OK);

    for (i4=0; i4<IMGDMA_INST_NUM; i4++)
    {
        // delete a event
        i4Ret = x_ev_group_delete(_arImgdmaInst[i4].hEventHandle);
        VERIFY(i4Ret == OSR_OK);

        // delete a semaphore
        i4Ret = x_sema_delete(_arImgdmaInst[i4].hSemaHandle);
        VERIFY(i4Ret == OSR_OK);
    }
    
    _fgImgdmaInit = FALSE;
    
    return IMGDMA_OK;
}

INT32 i4ImgdmaUnInit(void)
{
    i4Imgdma_UnInit(0);
    return IMGDMA_OK;
}


