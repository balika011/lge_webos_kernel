
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

#if defined(LINUX_TURNKEY_SOLUTION) && defined(CC_ENABLE_VDECOMX)

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include <linux/cdev.h>
#include <linux/fs.h>          //For file_operations
#include "x_typedef.h"
#include "x_os.h"
#include "x_printf.h"
#include "x_assert.h"

#include "vdecomx_drv_if.h"
#include <linux/cb_data.h>
#include <linux/version.h>

#include <asm/uaccess.h>
#include "OMX_Core.h"
#include "x_kmem.h"

#include "vdec_drvif.h"
#if 1//1//defined(VDEC_YUV_CVT_IMG)
#include <linux/module.h>
#include "imgrz_if.h"
#include "imgrz_drvif.h"
#include "gfx_if.h"
#include "gfx_drvif.h"
#endif
LINT_EXT_HEADER_BEGIN
#define DEFINE_IS_LOG    VOMX_IsLog
#include "x_debug.h"
LINT_EXT_HEADER_END


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
#define USE_UNLOCK_IOCTL
#endif

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg, type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        LOG(1, "vdecomx_mod argument error\n");     \
        return -1;                                          \
    }                                                       \

#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel, type)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(type)))          \
    {                                                       \
        LOG(1, "vdecomx_mod argument error\n");      \
        return -1;                                          \
    }

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel, type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        LOG(1, "vdecomx_mod argument error\n");      \
        return -1;                                          \
    }



typedef struct _PARAMETER_VDECOMX_ALLOCATEBUFFER {           //OMX_ALLOCATEBUFFER
    OMX_ERRORTYPE               eReturnValue;
    OMX_HANDLETYPE              pHandle;
    HANDLE_T                    hPBufHeader;
    HANDLE_T                    hPBufHeaderBuf;
    OMX_U32                     nPortIndex;
    OMX_PTR                     pAppPrivate;
    OMX_U32                     nSizeBytes;
    HANDLE_T                    hPBufHeaderInternalBuf;
    OMX_BOOL                    fgUseBuf;                // use the assign address
} PARAMETER_VDECOMX_ALLOCATEBUFFER;

typedef struct _PARAMETER_VDECOMX_FREEBUFFER {           //OMX_FREEBUFFER
    OMX_ERRORTYPE               eReturnValue;
    OMX_HANDLETYPE              pHandle;
    OMX_U32                     nPortIndex;
    HANDLE_T                    hPBufHeader;
    HANDLE_T                    hPBufHeaderBuf;
} PARAMETER_VDECOMX_FREEBUFFER;

typedef struct _PARAMETER_VDECOMX_FILLBTAG {

  UINT32 nDecodeBufId;
  UINT32 nTag;
}PARAMETER_VDECOMX_FILLBTAG;


#define MAX_OUTPUT_BUFFER 64

typedef struct 
{
    UINT32 nGpuId;
    UINT32 u4BufferIndex;
}VDECOMX_FILLDONE_INFOR;

typedef struct
{
    HANDLE_T  hMemHandleY;
    HANDLE_T  hMemHandleC;
    UINT32 u4Address;
    UINT32 u4Size;
    UINT32 u4BufIndex;
}VDECOMX_KMEM_INFOR;

typedef struct
{
    UINT32  u4BufAddr;
    UINT32  u4BufSize;
    ENUM_VDEC_FMT_T eFmt;
    VOID*   pVPush;
    UINT8   uVdecId;
    BOOL    fgPlay;
    BOOL    fgOccupied;
    BOOL    fgDecErr;
    BOOL    fgSecureInput;
    BOOL    fgInFlush;
    BOOL    fgPending;
    VDECOMX_MODE_T ePlayMode;
    VDECOMX_KMEM_INFOR rKmemInfor[MAX_OUTPUT_BUFFER];
} VDECOMX_HANDLE_T;

typedef struct _VDECOMX_BUFFER_T {
    VDECOMX_HANDLE_T    *pOwner;
    HANDLE_T             hBuffer; // handle for below buffer
    void                *pBufSA;
    UINT32               u4BufSize; // althrough buffer size is included in below header, keep this for error detection
    HANDLE_T             hHeader; // handle for below structure
    UINT32               u4FilledLen; // used on empty, keep this althrough duplicated
    UINT32               u4Offset; // used on empty, keep this althrough duplicated
    OMX_BUFFERHEADERTYPE rHeader; // client will read/write this area
    OMX_TICKS            nTimeStamp;
    UINT32               u4Reserved; // for error detection
}VDECOMX_BUFFER_T;


//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------
#define VOMX_INVALID_VALUE ((UINT32)-1)
static BOOL _fgInited = FALSE;
static int _u4VdecOmxOpenCnt = 0;
static int _u4VdecOmxLogValue = 0;
#if 1//1//defined(VDEC_YUV_CVT_IMG)
static int _u4VdecOmxColorAddrY = 0;
static int _u4VdecOmxScaleAddrY = 0;
#endif
static VDECOMX_BUFFER_T _prVDOOmxInputBuf[VDECOMX_INPUT_BUF_MAX];
VDECOMX_HANDLE_T _arVdecOmx[VDECOMX_MAX_DECODER];
//    HANDLE_T _hVecOmxSema = NULL;

extern INT32 _CB_PutEvent(CB_FCT_ID_T eFctId, INT32 i4TagSize, void *pvTag);

static void _vdecomx_bufferinfor_release_kmemhandler(VDECOMX_KMEM_INFOR *pKmemInfo)
{
    UINT32 u4Index=0;
    VDECOMX_KMEM_INFOR *pKmem;

    for(u4Index=0;u4Index<MAX_OUTPUT_BUFFER;u4Index++)
    {
        pKmem=pKmemInfo+u4Index;
        if(pKmem->hMemHandleY!=VOMX_INVALID_VALUE)
        {
        
            LOG(4,"[Out] _vdecomx_bufferinfor_release_kmemhandler Id=%d,BuffIndex=0x%x,Y=0x%x\n",
                u4Index,pKmem->u4BufIndex,pKmem->hMemHandleY);
            x_kmem_destroy_handle(pKmem->hMemHandleY);
            pKmem->hMemHandleY=VOMX_INVALID_VALUE;
        }
        
        if(pKmem->hMemHandleC!=VOMX_INVALID_VALUE)
        {
            LOG(4,"[Out] _vdecomx_bufferinfor_release_kmemhandler Id=%d,BuffIndex=0x%x,C=0x%x\n",
                u4Index,pKmem->u4BufIndex,pKmem->hMemHandleC);
            x_kmem_destroy_handle(pKmem->hMemHandleC);
            pKmem->hMemHandleC=VOMX_INVALID_VALUE;
        }
        
    }
   return;
}


static UINT32 _vdecomx_bufferinfor_get_kmemhandler(
    VDECOMX_KMEM_INFOR *pKmemInfo,UINT32 u4BufferIndex,
    UINT32 u4AddressY,UINT32 u4AddressC,UINT32 u4Size)
{
    UINT32 u4Index=0;
    VDECOMX_KMEM_INFOR *pKmem;
    if(u4BufferIndex==VOMX_INVALID_VALUE)
    {
       return VOMX_INVALID_VALUE;
    }

    if(u4AddressY==0 || u4AddressC==0 || u4Size==0)
    {
       LOG(0,"_vdecomx_bufferinfor_get_kmemhandler Error:AddrY=0x%x,AddrC=0x%x,Size=0x%x\n",
        u4AddressY,u4AddressC,u4Size);
       return VOMX_INVALID_VALUE;
    }

    for(u4Index=0;u4Index<MAX_OUTPUT_BUFFER;u4Index++)
    {
       pKmem=pKmemInfo+u4Index;
       
       if(pKmem->u4BufIndex==VOMX_INVALID_VALUE)
       {
          pKmem->u4BufIndex=u4BufferIndex;
       }

       if(pKmem->u4BufIndex==u4BufferIndex)
       {
          if(pKmem->u4Address!=u4AddressY ||pKmem->u4Size!=u4Size)
          {
             if(pKmem->hMemHandleY!=VOMX_INVALID_VALUE)
             {
                x_kmem_destroy_handle(pKmem->hMemHandleY);
                pKmem->hMemHandleY=VOMX_INVALID_VALUE;
             }

             if(pKmem->hMemHandleC!=VOMX_INVALID_VALUE)
             {
                x_kmem_destroy_handle(pKmem->hMemHandleC);
                pKmem->hMemHandleC=VOMX_INVALID_VALUE;
             }
          }
          
          if(pKmem->hMemHandleY==VOMX_INVALID_VALUE)
          {
              pKmem->hMemHandleY=x_kmem_create_handle((void *)VIRTUAL(u4AddressY),u4Size);
          }

          if(pKmem->hMemHandleC==VOMX_INVALID_VALUE)
          {
             pKmem->hMemHandleC=x_kmem_create_handle((void *)VIRTUAL(u4AddressC),u4Size>>1);
          }
          LOG(5,"[Out] _vdecomx_bufferinfor_get_kmemhandler Id=%d,BufIndex=0x%x,Y=0x%x,C=0x%x\n",
            u4Index,u4BufferIndex,pKmem->hMemHandleY,pKmem->hMemHandleC);
          return u4Index;
       }
    }
    
    LOG(0,"[Out] _vdecomx_bufferinfor_get_kmemhandler can't find 0x%x\n",u4BufferIndex);
    return VOMX_INVALID_VALUE;
}

static BOOL _vdecomx_decode_done(UINT32 u4Tag, UINT32 u4FbgId, UINT32 u4FbId, BOOL fgEos)
{
    VDECOMX_HANDLE_T *prVdecOmx = (VDECOMX_HANDLE_T *)u4Tag;

    if(prVdecOmx->ePlayMode == VDECOMX_MODE_TUNNEL)
    {        
        return TRUE;
    }
    else
    {
        VDECOMX_NFY_T tNfy;
        UCHAR ucLockFbId;
        ucLockFbId=FBM_GetDisplayFB(u4FbgId);
        LOG(4,"[Out]Frame decode done[%d, %d]\n",u4FbgId,u4FbId);
        if(prVdecOmx->fgInFlush)
        {        
            LOG(3,"[Out][Flush] Fbg=0x%x,Fb=0x%x\n",u4FbgId,u4FbId);
            if(ucLockFbId != u4FbId)
                LOG(0,"[Out][Warning!] ucLockFbId %d != u4FbId %d\n", ucLockFbId, u4FbId);
        }
        else
        {
            ASSERT(ucLockFbId == u4FbId);
            x_memset(&tNfy, 0, sizeof(tNfy));
            tNfy.pvhdlr = prVdecOmx->pVPush;
            tNfy.u4Data1=(u4FbgId<<16)+u4FbId;
            tNfy.eType = VDECOMX_NFY_TYPE_FRAME_DECODED;
            _CB_PutEvent(CB_DRV_VDECOMX_NFY, sizeof(tNfy), &tNfy);
        }
        return TRUE;
    }
}


static BOOL _vdecomx_error_callback(UINT32 u4Tag, VOID* pvData)
{
    VDECOMX_HANDLE_T *prVdecOmx = (VDECOMX_HANDLE_T *)u4Tag;
    UINT32 u4ErrType = *((UINT32*)pvData);

    LOG(0, "_vdecomx_error_callback, prVdecOmx 0x%x, errType %d\n", prVdecOmx, u4ErrType);

    switch (u4ErrType)
    {
        case VDEC_DEC_DECODE_FORCE_STOP:
            {
                VDECOMX_NFY_T tNfy;
                x_memset(&tNfy, 0, sizeof(tNfy));
                tNfy.pvhdlr = prVdecOmx->pVPush;
                tNfy.eType = VDECOMX_NFY_TYPE_FORCE_STOP;
                _CB_PutEvent(CB_DRV_VDECOMX_NFY, sizeof(tNfy), &tNfy);
            }
            break;
        case VDEC_DEC_DECODE_CODEC_NOT_SUPPORT:
        case VDEC_DEC_DECODE_HD_NOT_SUPPORT:
        case VDEC_DEC_DECODE_ERROR:
        case VDEC_DEC_DECODE_CODEC_TYPE_CHG:
        case VDEC_DEC_DECODE_RES_NOT_SUPPORT:
            LOG(0, "decoder error : %d\n", u4ErrType);
            prVdecOmx->fgDecErr = TRUE;
            break;
        default:
            break;
    }

    return TRUE;
}

static void _vdecomx_buffer_init(void)
{
    UINT32 u4Index;

    for (u4Index = 0; u4Index < VDECOMX_INPUT_BUF_MAX; u4Index ++)
    {

        _prVDOOmxInputBuf[u4Index].pOwner     = NULL;
        _prVDOOmxInputBuf[u4Index].hBuffer    = NULL_HANDLE;
        _prVDOOmxInputBuf[u4Index].pBufSA     = NULL;
        _prVDOOmxInputBuf[u4Index].u4BufSize  = 0;
        _prVDOOmxInputBuf[u4Index].hHeader    = NULL_HANDLE;
        _prVDOOmxInputBuf[u4Index].u4Reserved = 0x5A5A5A5A;
    }
}


static void _vdecomx_buffer_uninit(void)
{
    UINT32 u4Index;

    for (u4Index = 0; u4Index < VDECOMX_INPUT_BUF_MAX; u4Index ++)
    {
        if (NULL_HANDLE != _prVDOOmxInputBuf[u4Index].hHeader)
        {
            x_kmem_destroy_handle(_prVDOOmxInputBuf[u4Index].hHeader);
            _prVDOOmxInputBuf[u4Index].hHeader=NULL_HANDLE;
        }
    }
}

static int _vdecomx_alloc_input_buffer(VDECOMX_HANDLE_T *prVdecOmx, UINT32 u4Size, VDECOMX_BUFFER_T **ppBufInfo)
{
    // allocate input buffer from FEEDER
    UINT32 u4Index;
    void *pBufSA  = NULL;

    HANDLE_T hMemHandle = NULL_HANDLE;
    VDECOMX_BUFFER_T *pAvailBuf = NULL;

    VDECOMX_HANDLE_T *prVDOOmxHandle = prVdecOmx;

    // STEP 0: get a free input buffer info node
    for (u4Index = 0; u4Index < VDECOMX_INPUT_BUF_MAX; u4Index ++)
    {
        if (NULL == _prVDOOmxInputBuf[u4Index].pOwner)
        {
            pAvailBuf = &(_prVDOOmxInputBuf[u4Index]);
            break;
        }
    }

    if (NULL == pAvailBuf)
    {
        // no more free nodes
        LOG(0, "_vdecomx_alloc_input_buffer, not find node\n");
        goto lblErrorReturn;
    }
    else
    {
        // initialize
        pAvailBuf->pOwner    = prVDOOmxHandle;
        pAvailBuf->hBuffer   = NULL_HANDLE;
        pAvailBuf->pBufSA    = NULL;
        pAvailBuf->u4BufSize = 0;
    }
    //VOMX_Printf("[VDO][OMX] i4VDOOmxAllocInputBuf : u4Index = 0x%x, pAvailBuf->pOwner = 0x%x, _prVDOOmxInputBuf[u4Index].pOwner = 0x%x.\n", (UINT32)u4Index, (UINT32)pAvailBuf->pOwner,(UINT32)_prVDOOmxInputBuf[u4Index].pOwner);

    if(!_VPUSH_AllocVFifo(prVDOOmxHandle->pVPush, (UINT32*)&pBufSA, u4Size, prVdecOmx->eFmt))
    {
        LOG(0, "[VDECOMX] alloc VFIFO memory return failed, %d bytes.\n", u4Size);
        goto lblErrorReturn;
    }
    
    if(pBufSA==NULL)
    {
        LOG(0, "[VDECOMX] alloc u4SecureBufferAlloc memory return failed, %d bytes.\n", u4Size);
        goto lblErrorReturn;
    }

    if(prVdecOmx->fgSecureInput)
    {
        pAvailBuf->hBuffer = (HANDLE_T)pBufSA;
    } 
    else
    {
        hMemHandle = x_kmem_create_handle(pBufSA, u4Size);
        if (NULL_HANDLE == hMemHandle)
        {
            goto lblErrorReturn;
        }
        else
        {
            pAvailBuf->hBuffer = hMemHandle;
        }
    }

    pAvailBuf->pBufSA    = pBufSA;
    pAvailBuf->u4BufSize = u4Size;
    prVdecOmx->u4BufAddr = (UINT32)pBufSA;
    prVdecOmx->u4BufSize = u4Size;
    *ppBufInfo = pAvailBuf;
    return 0;

    // error handling
lblErrorReturn:

    // resource release will be done on freeing component handle, not here
    *ppBufInfo = NULL;
    return -1;
}


static int _vdecomx_free_input_buffer(VDECOMX_HANDLE_T *prVdecOmx, HANDLE_T hHeader)
{
    UINT32 u4Index;
    VDECOMX_BUFFER_T *pBufNode = NULL;

    if ((NULL == prVdecOmx) || (NULL_HANDLE == hHeader))
    {
        LOG(0, "_vdecomx_free_input_buffer, handle is null\n");
        return -1;
    }

    for (u4Index = 0; u4Index < VDECOMX_INPUT_BUF_MAX; u4Index ++)
    {
        if (hHeader == _prVDOOmxInputBuf[u4Index].hBuffer)
        {
            pBufNode = &(_prVDOOmxInputBuf[u4Index]);
            break;
        }
    }

    if (NULL != pBufNode)
    {
        if ((pBufNode->pOwner == prVdecOmx) && (NULL != pBufNode->pBufSA))
        {            
            LOG(2, "[In] free input buf addr:0x%xx\n", pBufNode->pBufSA);
            _VPUSH_FreeVFifo(prVdecOmx->pVPush, (UINT32)pBufNode->pBufSA);
            if(!prVdecOmx->fgSecureInput)
            {
                // Todo : Free secure buffer.
//                  if(bSecureBufferFree((UINT32)pBufNode->pBufSA)==FALSE)
//                  {
//                      LOG(0, "bSecureBufferFree, ret fail\n");
//                  }
                x_kmem_destroy_handle(pBufNode->hBuffer);
            }
            // clear info
            pBufNode->pOwner    = NULL;
            pBufNode->hBuffer   = NULL_HANDLE;
            pBufNode->pBufSA    = NULL;
            pBufNode->u4BufSize = 0;
            pBufNode->hHeader   = NULL_HANDLE;
        }
    }

    return 0;
}

#if 0
int _vdecomx_copy(VDECOMX_HANDLE_T *prVdecOmx, VDECOMX_RINGBUF_T *prBitstream)
{
    UINT32 u4Size;
    if((prBitstream->rBase.u4VA == 0) ||
       (prBitstream->rBase.u4Size == 0) ||
       (prBitstream->u4Read == 0) ||
       (prBitstream->u4Write == 0))
    {
        LOG(3, "%s(L:%d) addr = 0\n", __FUNCTION__, __LINE__);
        return -1;
    }
    if(prBitstream->u4Read == prBitstream->u4Write)
    {
        // size = 0, NeedMoreFrameBuf case.
        return 0;
    }
    if((prBitstream->u4Read < prBitstream->rBase.u4VA) ||
       (prBitstream->u4Write < prBitstream->rBase.u4VA) ||
       (prBitstream->u4Read >= (prBitstream->rBase.u4VA + prBitstream->rBase.u4Size)) ||
       (prBitstream->u4Write > (prBitstream->rBase.u4VA + prBitstream->rBase.u4Size)))
    {
        LOG(3, "%s(L:%d) read/write address error\n", __FUNCTION__, __LINE__);
        return -1;
    }
    
    if(prBitstream->u4Read < prBitstream->u4Write)
    {
        u4Size = (prBitstream->u4Write - prBitstream->u4Read);
    }
    else
    {
        // TODO, actually I don't know the defination of u4PA and u4VA.
        // User assign the same value in u4PA and u4VA.
        u4Size = (prBitstream->u4Write - prBitstream->rBase.u4VA) +
            (prBitstream->rBase.u4VA + prBitstream->rBase.u4Size - prBitstream->u4Read);
    }
    if(u4Size > prVdecOmx->u4BufSize)
    {
        ASSERT(0);
        LOG(3, "%s(L:%d) u4BufSize(%d) too small(%d)\n", __FUNCTION__, __LINE__,
            prVdecOmx->u4BufSize,
            (prBitstream->u4Write - prBitstream->u4Read));
        return -1;
    }
    
    // copy the data
if(prVdecOmx->fgSecureInput==FALSE)
{
    if(prBitstream->u4Read < prBitstream->u4Write)
    {
        if (!access_ok(VERIFY_READ,
                (void __user *)prBitstream->u4Read,
                (prBitstream->u4Write - prBitstream->u4Read)))
        {
            LOG(3, "%s(L:%d) can't access\n", __FUNCTION__, __LINE__);
            return -1;
        }
        if (copy_from_user((void *)prVdecOmx->u4BufAddr,
                (void __user *)prBitstream->u4Read,
                (prBitstream->u4Write - prBitstream->u4Read)))
        {
            LOG(3, "%s(L:%d) copy_from_user error\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }
    else
    {
        if (!access_ok(VERIFY_READ,
                (void __user *)prBitstream->u4Read,
                (prBitstream->rBase.u4VA + prBitstream->rBase.u4Size - prBitstream->u4Read)))
        {
            LOG(3, "%s(L:%d) can't access\n", __FUNCTION__, __LINE__);
            return -1;
        }
        if (copy_from_user((void *)prVdecOmx->u4BufAddr,
                (void __user *)prBitstream->u4Read,
                (prBitstream->rBase.u4VA + prBitstream->rBase.u4Size - prBitstream->u4Read)))
        {
            LOG(3, "%s(L:%d) copy_from_user error\n", __FUNCTION__, __LINE__);
            return -1;
        }
        if (!access_ok(VERIFY_READ,
                (void __user *)prBitstream->rBase.u4VA,
                (prBitstream->u4Write - prBitstream->u4Read)))
        {
            LOG(3, "%s(L:%d) can't access\n", __FUNCTION__, __LINE__);
            return -1;
        }
        if (copy_from_user((void *)(prVdecOmx->u4BufAddr +
                    (prBitstream->rBase.u4VA + prBitstream->rBase.u4Size - prBitstream->u4Read)),
                (void __user *)prBitstream->rBase.u4VA,
                (prBitstream->u4Write - prBitstream->rBase.u4VA)))
        {
            LOG(3, "%s(L:%d) copy_from_user error\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }
}
    return (int)u4Size;
}
#endif

int vdecomx_open(struct inode *inode, struct file *pfile)
{
    UINT32 i;
    VDECOMX_HANDLE_T *prVdecOmx;
    if(!_fgInited)
    {
        LOG(1, "%s(L:%d) init\n", __FUNCTION__, __LINE__);
        _VPUSH_DecodeInit();
        _fgInited = TRUE;
    }

    for (i = 0; i < VDECOMX_MAX_DECODER; i++)
    {
        if (!_arVdecOmx[i].fgOccupied)
        {
            _arVdecOmx[i].fgOccupied = TRUE;
            break;
        }
    }
    if(i >= VDECOMX_MAX_DECODER)
    {
        LOG(0, "%s(L:%d) inst full\n", __FUNCTION__, __LINE__);
        return -1;
    }

    pfile->private_data = &_arVdecOmx[i];
    prVdecOmx=&_arVdecOmx[i];

    if(prVdecOmx)
    {
       for(i=0;i<MAX_OUTPUT_BUFFER;i++)
       {
          prVdecOmx->rKmemInfor[i].u4Size=0;
          prVdecOmx->rKmemInfor[i].u4Address=0;
          prVdecOmx->rKmemInfor[i].u4BufIndex=VOMX_INVALID_VALUE;
          prVdecOmx->rKmemInfor[i].hMemHandleC=VOMX_INVALID_VALUE;
          prVdecOmx->rKmemInfor[i].hMemHandleY=VOMX_INVALID_VALUE;
       }
    }

    if (0 == _u4VdecOmxOpenCnt)
    {
        _vdecomx_buffer_init();
    }

    _u4VdecOmxOpenCnt ++;

    return 0;
}

static ENUM_VDEC_FMT_T vdecomx_fmt_map(VDECOMX_VIDEO_FORMAT_T eFmtIn)
{
    ENUM_VDEC_FMT_T eFmtOut;
    switch(eFmtIn)
    {
    case VDECOMX_VIDEO_FORMAT_DIVX311:
    case VDECOMX_VIDEO_FORMAT_DIVX4:
    case VDECOMX_VIDEO_FORMAT_DIVX5:
    case VDECOMX_VIDEO_FORMAT_XVID:
    case VDECOMX_VIDEO_FORMAT_MPEG4:
    case VDECOMX_VIDEO_FORMAT_H263:
    case VDECOMX_VIDEO_FORMAT_S263:
            eFmtOut = VDEC_FMT_MP4;
            break;
    case VDECOMX_VIDEO_FORMAT_MPEG1:
    case VDECOMX_VIDEO_FORMAT_MPEG2:
            eFmtOut = VDEC_FMT_MPV;
            break;
    case VDECOMX_VIDEO_FORMAT_H264SEC:
            eFmtOut = VDEC_FMT_H264;
            break;
    case VDECOMX_VIDEO_FORMAT_H264:
    case VDECOMX_VIDEO_FORMAT_H264HP:
            eFmtOut = VDEC_FMT_H264;
            break;
    case VDECOMX_VIDEO_FORMAT_WMV9:
    case VDECOMX_VIDEO_FORMAT_VC1:
            eFmtOut = VDEC_FMT_WMV;
            break;
    case VDECOMX_VIDEO_FORMAT_REALVIDEO8:
    case VDECOMX_VIDEO_FORMAT_REALVIDEO9:
            eFmtOut = VDEC_FMT_RV;
            break;
    case VDECOMX_VIDEO_FORMAT_VP6:
            eFmtOut = VDEC_FMT_VP6;
            break;
    case VDECOMX_VIDEO_FORMAT_VP8:
            eFmtOut = VDEC_FMT_VP8;
            break;
    case VDECOMX_VIDEO_FORMAT_VP9:
            eFmtOut = VDEC_FMT_VP9;
            break;
    case VDECOMX_VIDEO_FORMAT_AVS:
            eFmtOut = VDEC_FMT_AVS;
            break;
    case VDECOMX_VIDEO_FORMAT_MJPEG:
            eFmtOut = VDEC_FMT_MJPEG;
            break;
    case VDECOMX_VIDEO_FORMAT_H265:        
            eFmtOut = VDEC_FMT_H265;        
            break;
    case VDECOMX_VIDEO_FORMAT_WMV7:
    case VDECOMX_VIDEO_FORMAT_WMV8:
    case VDECOMX_VIDEO_FORMAT_VP7:
        default:            
            eFmtOut = VDEC_FMT_MAX;
            break;
    }
    return eFmtOut;
}

int vdecomx_create(VDECOMX_HANDLE_T *prVdecOmx, unsigned long arg)
{
    PAR_VDECOMX_CREATE rCreate;
    ENUM_VDEC_FMT_T eFmt = VDEC_FMT_MAX;
    VDEC_SET_INTO_T rVdecSetInfo;
    BOOL fgLowLatency = FALSE;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, PAR_VDECOMX_CREATE);
    COPY_FROM_USER_ARG(arg, rCreate, PAR_VDECOMX_CREATE);

    prVdecOmx->pVPush = NULL;
    prVdecOmx->u4BufAddr = 0;
    prVdecOmx->fgDecErr = FALSE;
    prVdecOmx->fgSecureInput = FALSE;
    prVdecOmx->fgInFlush = FALSE;
    if(VDECOMX_VIDEO_FORMAT_H264SEC == rCreate.eVideoFormat)
        prVdecOmx->fgSecureInput = TRUE;

    eFmt = vdecomx_fmt_map(rCreate.eVideoFormat);
    LOG(2,"vdecomx_create FMT=%d\n",eFmt);

    if(eFmt == VDEC_FMT_MAX)
    {
        LOG(0, "%s(L:%d) not support format(%d)\n", __FUNCTION__, __LINE__,
            rCreate.eVideoFormat);
        return -1;
    }

    if(rCreate.uVdecId>=VDECOMX_MAX_DECODER)
    {
         LOG(0, "%s(L:%d) vdecId %d error \n", __FUNCTION__, __LINE__,rCreate.uVdecId);
         return -1;
    }

    prVdecOmx->eFmt = eFmt;
    prVdecOmx->uVdecId = rCreate.uVdecId;
    // esid always equal to vpush id
    prVdecOmx->pVPush = _VPUSH_AllocVideoDecoder(eFmt, prVdecOmx->uVdecId);
    if(!prVdecOmx->pVPush)
    {
        LOG(0, "vdec-%d is occupied, please try next another one\n", __FUNCTION__, prVdecOmx->uVdecId);
        return -1;
    }
    
    x_memset(&rVdecSetInfo, 0, sizeof(VDEC_SET_INTO_T));
    rVdecSetInfo.u4InfoMask = VDEC_PUSH_SET_INFO_PLAY_MODE;
//	    rVdecSetInfo.ucVdecId=prVdecOmx->uVdecId;
    
    if(rCreate.eMode != VDECOMX_MODE_ASYNC && rCreate.eMode != VDECOMX_MODE_TUNNEL)
    {                    
        LOG(0, "%s(L:%d)  no such mode[%d]!\n", __FUNCTION__, __LINE__, rCreate.eMode);
        goto CREATE_FAIL;
    }
    prVdecOmx->ePlayMode = rCreate.eMode;
    
    switch(rCreate.eMode)
    {
        case VDECOMX_MODE_ASYNC:
            rVdecSetInfo.eVpushPlayMode = VDEC_PUSH_MODE_ASYNC;
            break;
        case VDECOMX_MODE_TUNNEL:
            rVdecSetInfo.eVpushPlayMode = VDEC_PUSH_MODE_TUNNEL;
            break;
        default:
            rVdecSetInfo.eVpushPlayMode = VDEC_PUSH_MODE_UNKNOWN;
            break;
    }

    switch (prVdecOmx->eFmt)
    {
    case VDEC_FMT_H264:
        {
            if (rCreate.eMode == VDECOMX_MODE_TUNNEL)
            {
                fgLowLatency = TRUE;
            }
        }
        break;
    case VDEC_FMT_VP9:
        {
            fgLowLatency = TRUE;
        }
        break;
    default:
        break;
    }
    
    //low latency mode, deocde do not keep last frame.
    if(fgLowLatency)
    {
        rVdecSetInfo.u4InfoMask |= VDEC_PUSH_SET_INFO_LOW_LATENCY;
    }
        
    if(!_VPUSH_SetInfo(prVdecOmx->pVPush, &rVdecSetInfo))
    {
        LOG(0, "%s(L:%d) _VPUSH_SetInfo fail, Play mode: %d\n", __FUNCTION__, __LINE__, rVdecSetInfo.eVpushPlayMode);
        goto CREATE_FAIL;
    }

    if(prVdecOmx->fgSecureInput)
    {
        x_memset(&rVdecSetInfo, 0, sizeof(VDEC_SET_INTO_T));
        rVdecSetInfo.u4InfoMask |= VDEC_PUSH_SET_INFO_SECURE_INPUT;        
        LOG(0, "secure Play!\n");
        if(!_VPUSH_SetInfo(prVdecOmx->pVPush, &rVdecSetInfo))
        {
            LOG(0, "%s(L:%d) _VPUSH_SetInfo VDEC_PUSH_SET_INFO_FBG_FROM_INST fail\n", __FUNCTION__, __LINE__);
             goto CREATE_FAIL;
        }
    }

    {
        VDEC_PUSH_CB_T rPushCb;
        x_memset(&rPushCb, 0, sizeof(VDEC_PUSH_CB_T));
        //rPushCb.u4Mask |= VDEC_PUSH_CB_STOP_DONE;
        //rPushCb.u4StopTag = (UINT32)prVdecOmx;
        //rPushCb.pfnVdecStopDone = i4VDOOmxCompVPushStopDone;
        //rPushCb.u4Mask |= VDEC_PUSH_CB_FLUSH_DONE;
        //rPushCb.u4FlushTag = (UINT32)prVdecOmx;
        //rPushCb.pfnVdecFlushDone = i4VDOOmxCompVPushFlushDone;
        //rPushCb.u4Mask |= VDEC_PUSH_CB_PUT_DATA_DONE;
        //rPushCb.u4PutTag = (UINT32)prVdecOmx;
        //rPushCb.pfnVdecPutDataDone = i4VDOOmxCompVPushPutDone;
        if (prVdecOmx->ePlayMode != VDECOMX_MODE_TUNNEL)
        {
            rPushCb.u4Mask |= VDEC_PUSH_CB_DECODE_DONE;
            rPushCb.u4DecodeTag = (UINT32)prVdecOmx;
            rPushCb.pfnVdecDecodeDone = _vdecomx_decode_done;
        }
        rPushCb.u4Mask |= VDEC_PUSH_CB_VDEC_ERR;
        rPushCb.u4VdecErrTag = (UINT32)prVdecOmx;
        rPushCb.pfnVdecErrHandler = _vdecomx_error_callback;

        if(!_VPUSH_RegCb(prVdecOmx->pVPush, &rPushCb))
        {
            LOG(0, "%s(L:%d) _VPUSH_RegCb fail\n", __FUNCTION__, __LINE__);
            ASSERT(0);
            goto CREATE_FAIL;
        }
    }

    LOG(0,"vdecomx_create done mode=%d fmt=%d vdecId=%d\n", rCreate.eMode, eFmt, prVdecOmx->uVdecId);
    rCreate.pvhdlr = prVdecOmx->pVPush;
    rCreate.eRet = VDECOMX_MRESULT_OK;
    COPY_TO_USER_ARG(arg, rCreate, PAR_VDECOMX_CREATE);
    return 0;

CREATE_FAIL:
    prVdecOmx->eFmt = VDEC_FMT_MAX;
    if(prVdecOmx->pVPush)
    {
        _VPUSH_ReleaseVideoDecoder(prVdecOmx->pVPush);
        prVdecOmx->pVPush = NULL;
    }
    return -1;
}

int vdecomx_init(VDECOMX_HANDLE_T *prVdecOmx, unsigned long arg)
{
    PAR_VDECOMX_INIT rInit;
    VDEC_BYTES_INTO_T rBytesInfo;
    VDECOMX_RINGBUF_T rBitstream;
//	    VDECOMX_SEQINFO_T rSeqinfo;
    FBM_SEQ_HDR_T rVdecSeqInfo;
    UINT32 u4Index;
    VDECOMX_BUFFER_T *pBufNode = NULL;
    UINT32 u4Size = 0;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, PAR_VDECOMX_INIT);
    COPY_FROM_USER_ARG(arg, rInit, PAR_VDECOMX_INIT);
    USR_SPACE_ACCESS_VALIDATE_ARG(rInit.prBitstream, VDECOMX_RINGBUF_T);
    COPY_FROM_USER_ARG(rInit.prBitstream, rBitstream, VDECOMX_RINGBUF_T);
    
    x_memset(&rBytesInfo, 0, sizeof(VDEC_BYTES_INTO_T));
    x_memset(&rVdecSeqInfo, 0, sizeof(VDEC_SEQUENCE_DATA_T));

    if(!_VPUSH_Play(prVdecOmx->pVPush))
    {
        LOG(0, "%s(L:%d) _VPUSH_Play fail\n", __FUNCTION__, __LINE__);
        return -1;
    }
    prVdecOmx->fgPlay = TRUE;

    // copy ES data from the rBitstream to prVdecOmx memory address
    if( prVdecOmx->ePlayMode == VDECOMX_MODE_ASYNC || prVdecOmx->ePlayMode == VDECOMX_MODE_TUNNEL)
    {
        for (u4Index = 0; u4Index < VDECOMX_INPUT_BUF_MAX; u4Index++)
        {
            if (rBitstream.u4BufHandle == _prVDOOmxInputBuf[u4Index].hBuffer)
            {
                pBufNode = &(_prVDOOmxInputBuf[u4Index]);
                break;
            }
        }

        if(pBufNode == NULL)
        {
            LOG(0, "%s(%d), Not find input buffer header\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }



    LOG(2, "[In] Header u4Size: %d\n", rBitstream.u4Size);
    u4Size = rBitstream.u4Size;
    if(VDEC_FMT_MPV == prVdecOmx->eFmt)
    {
        if(u4Size <= (pBufNode->u4BufSize - 8))
        {
            // insert PIC start code for DMX send SEQ HDR to vdec.
            // TODO?
            UCHAR *pucPtr = (UCHAR*)(pBufNode->pBufSA + rBitstream.u4Offset);
            pucPtr[u4Size] = 0x00;
            pucPtr[u4Size+1] = 0x00;
            pucPtr[u4Size+2] = 0x01;
            pucPtr[u4Size+3] = 0x00;
            pucPtr[u4Size+4] = 0x00;
            pucPtr[u4Size+5] = 0x0F; // I frame.
            pucPtr[u4Size+6] = 0xFF;
            pucPtr[u4Size+7] = 0xF8;
            //u4TemporalRef       :    10;
            //u4PicType               :    3;
            //u4VbvDelay            :    16;
            // :    3;
            u4Size += 8;
        }
        else
        {
            LOG(3, "%s(L:%d) insert start code fail\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }

    rBytesInfo.u4BytesAddr = (UINT32)(pBufNode->pBufSA) + rBitstream.u4Offset;
    rBytesInfo.u4BytesSize = u4Size;
    rBytesInfo.u8BytesPTS = (UINT64)rBitstream.u8Timestamp;
    rBytesInfo.fgSecureBuf=prVdecOmx->fgSecureInput;
    rBytesInfo.u4SecureInputBufStartAddr = (UINT32)(pBufNode->pBufSA);
    rBytesInfo.u4SecureInputBufEndAddr = rBytesInfo.u4SecureInputBufStartAddr + pBufNode->u4BufSize;
    rBytesInfo.fgMoveBufDone=TRUE;
    
    //low latency mode
    if((prVdecOmx->ePlayMode == VDECOMX_MODE_TUNNEL) && (prVdecOmx->eFmt == VDEC_FMT_H264))
    {
        UINT64 u4Delim = 0xF00901000000;
        x_memcpy((VOID*)(VIRTUAL(rBytesInfo.u4BytesAddr + rBytesInfo.u4BytesSize)), (VOID*)(&u4Delim), 6);
        rBytesInfo.u4BytesSize += 6;
    }

    // call vpush to parse
    if(!_VPUSH_PrsSeqHdr(prVdecOmx->pVPush, &rBytesInfo, &rVdecSeqInfo))
    {
        LOG(0, "%s(L:%d) _VPUSH_PrsSeqHdr fail\n", __FUNCTION__, __LINE__);
        return -1;
    }

//info not used by omx component now
#if 0
    x_memset(&rSeqinfo, 0, sizeof(VDECOMX_SEQINFO_T));

    // use rSeqInfo information
    rSeqinfo.u4Width = rVdecSeqInfo.u2HSize;
    rSeqinfo.u4Height = rVdecSeqInfo.u2VSize;

    // TODO, fill the display one.
    rSeqinfo.u4PicWidth = rVdecSeqInfo.u2HSize;
    rSeqinfo.u4PicHeight = rVdecSeqInfo.u2VSize;
    rSeqinfo.bCropping = rVdecSeqInfo.fgCropping;
    rSeqinfo.u4CropX = rVdecSeqInfo.u4CropX;
    rSeqinfo.u4CropY = rVdecSeqInfo.u4CropY;
    rSeqinfo.u4CropWidth = rVdecSeqInfo.u4CropWidth;
    rSeqinfo.u4CropHeight = rVdecSeqInfo.u4CropHeight;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(rInit.prSeqinfo, VDECOMX_SEQINFO_T);
    COPY_TO_USER_ARG(rInit.prSeqinfo, rSeqinfo, VDECOMX_SEQINFO_T);
#endif
    rInit.eRet = VDECOMX_MRESULT_OK;
    COPY_TO_USER_ARG(arg, rInit, PAR_VDECOMX_INIT);
    if(prVdecOmx->fgPending)
    {
        if(!_VPUSH_PauseVdec(prVdecOmx->pVPush))
        {
            LOG(3, "%s(L:%d) _VPUSH_PauseVdec fail\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }
    return 0;
}

int vdecomx_deinit(VDECOMX_HANDLE_T *prVdecOmx, unsigned long arg)
{
    if(prVdecOmx->fgPlay)
    {
        if(!_VPUSH_StopSync(prVdecOmx->pVPush))
        {
            LOG(0, "%s(L:%d) _VPUSH_StopSync fail\n", __FUNCTION__, __LINE__);
            return -1;
        }
        prVdecOmx->fgPlay = FALSE;
    }
    
    return 0;
}

int vdecomx_decode(VDECOMX_HANDLE_T *prVdecOmx, unsigned long arg)
{
    PAR_VDECOMX_DECODE rDecode;
    VDEC_BYTES_INTO_T rBytesInfo;
    UINT32 u4Index = VDECOMX_INPUT_BUF_MAX;
    VDECOMX_BUFFER_T *pBufNode = NULL;
    VDECOMX_RINGBUF_T rBitstream;          // IN
    INT32 iRet = VDEC_DEC_ERROR_NONE;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, PAR_VDECOMX_DECODE);
    COPY_FROM_USER_ARG(arg, rDecode, PAR_VDECOMX_DECODE);
    
    if(rDecode.prBitstream)
    {
        USR_SPACE_ACCESS_VALIDATE_ARG(rDecode.prBitstream, VDECOMX_RINGBUF_T);
        COPY_FROM_USER_ARG(rDecode.prBitstream, rBitstream, VDECOMX_RINGBUF_T);
    }
    
    if (prVdecOmx->ePlayMode == VDECOMX_MODE_ASYNC || prVdecOmx->ePlayMode == VDECOMX_MODE_TUNNEL)
    {
        x_memset(&rBytesInfo, 0, sizeof(VDEC_BYTES_INTO_T));
        
        for (u4Index = 0; u4Index < VDECOMX_INPUT_BUF_MAX; u4Index++)
        {
            if (rBitstream.u4BufHandle == _prVDOOmxInputBuf[u4Index].hBuffer)
            {
                pBufNode = &(_prVDOOmxInputBuf[u4Index]);
                break;
            }
        }

        if(pBufNode == NULL)
        {
            LOG(0, "%s(%d), Not find input buffer header\n", __FUNCTION__, __LINE__);
            return -1;
        }

        if(rBitstream.u4Size)
        {
            // prepare rBytesInfo
            rBytesInfo.u4BytesAddr = (UINT32)pBufNode->pBufSA + rBitstream.u4Offset;
            rBytesInfo.u4BytesSize = rBitstream.u4Size;
            rBytesInfo.u8BytesPTS  = (UINT64)rBitstream.u8Timestamp;
            //low latency mode
            if((prVdecOmx->ePlayMode == VDECOMX_MODE_TUNNEL) && (prVdecOmx->eFmt == VDEC_FMT_H264))
            {
                UINT64 u4Delim = 0xF00901000000;
                x_memcpy((VOID*)(VIRTUAL(rBytesInfo.u4BytesAddr + rBytesInfo.u4BytesSize)), (VOID*)(&u4Delim), 6);
                rBytesInfo.u4BytesSize += 6;
            }
        }
        else
        {
            rBytesInfo.u4BytesAddr = (UINT32)pBufNode->pBufSA;
            rBytesInfo.u4BytesSize = 0;
            rBytesInfo.u8BytesPTS = 0;
            // EOS
            rBytesInfo.fgEos = TRUE;
        } 

        rBytesInfo.u4SecureInputBufStartAddr = (UINT32)(pBufNode->pBufSA);
        rBytesInfo.u4SecureInputBufEndAddr = rBytesInfo.u4SecureInputBufStartAddr + pBufNode->u4BufSize;
    }
    
    // call vpush to decode
    rBytesInfo.fgSecureBuf = prVdecOmx->fgSecureInput;
    LOG(2, "[In] Addr=0x%x,Size=0x%x,Pts=0x%x,Eos=%d\n", rBytesInfo.u4BytesAddr,rBytesInfo.u4BytesSize, (UINT32)rBytesInfo.u8BytesPTS, rBytesInfo.fgEos);
    rBytesInfo.fgMoveBufDone=TRUE;
    iRet = _VPUSH_PutData(prVdecOmx->pVPush, &rBytesInfo);
    if (iRet < VDEC_DEC_ERROR_NONE)
    {
        LOG(0, "%s(L:%d) _VPUSH_PutData fail\n", __FUNCTION__, __LINE__);
        rDecode.eRet = VDECOMX_MRESULT_FATAL;
        goto lbFinish;
    }else if (iRet > VDEC_DEC_ERROR_NONE) 
    {
        LOG(3, "%s(L:%d) _VPUSH_PutData too slowly\n", __FUNCTION__, __LINE__);
        rDecode.eRet = VDECOMX_MRESULT_FAIL;
        goto lbFinish;
    }


    if (prVdecOmx->fgDecErr && !rBytesInfo.fgEos)
    {
         LOG(0, "vdecomx_decode return error\n");
         rDecode.eRet = VDECOMX_MRESULT_FATAL;
         goto lbFinish;
    }

    rDecode.eRet = VDECOMX_MRESULT_OK;

    lbFinish:
    COPY_TO_USER_ARG(arg, rDecode, PAR_VDECOMX_DECODE);
    return 0;
}


extern void FBM_QueryStatus(UCHAR ucFbgId);
int vdecomx_getparam(VDECOMX_HANDLE_T *prVdecOmx, unsigned long arg)
{
    BOOL fgRet;
    PAR_VDECOMX_GETPARAM rGetparam;
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, PAR_VDECOMX_GETPARAM);
    COPY_FROM_USER_ARG(arg, rGetparam, PAR_VDECOMX_GETPARAM);

    switch (rGetparam.eType)
    {
        case VDECOMX_GET_TYPE_DISP_FRAME_BUFFER:
            {
                VDECOMX_FRAME_T rFrame;          // IN
                VDEC_DISP_FRAME_T rDispFrame;
                UINT32 u4FbIndex=(UINT32)rGetparam.pvInParam;
                UINT8 u1FbgId,u1FbId;
    
                u1FbgId=(u4FbIndex>>16)&0xff;
                u1FbId=u4FbIndex&0xff;
                
                x_memset(&rDispFrame, 0, sizeof(VDEC_DISP_FRAME_T));
                x_memset(&rFrame, 0, sizeof(VDECOMX_FRAME_T));
                fgRet = _VPUSH_GetDisplayInfor(prVdecOmx->pVPush, &rDispFrame,u1FbgId,u1FbId);                
                LOG(3,"[Out] Index=0x%x Size=0x%x Pts=%llu eos=%d\n",u4FbIndex, rDispFrame.u4BufSize, rDispFrame.u8Timestamp, rDispFrame.fgEos);

                if(!fgRet)
                {
                    // return VDECOMX_MRESULT_FAIL if no disp buffer
                    // rFrame.rFrameBuf.rBaseAddr is 0;
                    rGetparam.eRet = VDECOMX_MRESULT_FATAL;                    
                    return -1;
                }
                else
                {
                    rFrame.rFrameFormat.u4Offset = 0;
                    rFrame.rFrameFormat.u4Width = (((rDispFrame.u4Width+15)/16) * 16);
                    rFrame.rFrameFormat.u4Height = (((rDispFrame.u4Height+15)/16) * 16);
                    if((rDispFrame.fgCropping) &&
                       (rDispFrame.u4CropWidth > 0) &&
                       (rDispFrame.u4CropHeight > 0))
                    {
                        rFrame.rFrameFormat.u2CropX = (UINT16)rDispFrame.u4CropX;
                        rFrame.rFrameFormat.u2CropY = (UINT16)rDispFrame.u4CropY;                        
                        rFrame.rFrameFormat.u2CropWidth = (UINT16)rDispFrame.u4CropWidth;                        
                        rFrame.rFrameFormat.u2CropHeight = (UINT16)rDispFrame.u4CropHeight;
                    }
                    else
                    {
                        ASSERT(rDispFrame.u4Width > 0);
                        ASSERT(rDispFrame.u4Height > 0);
                        rFrame.rFrameFormat.u2CropX = 0;
                        rFrame.rFrameFormat.u2CropY = 0;                        
                        rFrame.rFrameFormat.u2CropWidth = (UINT16)rDispFrame.u4Width;                        
                        rFrame.rFrameFormat.u2CropHeight = (UINT16)rDispFrame.u4Height;
                    }                    
                    rFrame.rFrameBuf.fgCropping= rDispFrame.fgCropping;
                    rFrame.rFrameBuf.u4CropX= rFrame.rFrameFormat.u2CropX;
                    rFrame.rFrameBuf.u4CropY= rFrame.rFrameFormat.u2CropY;
                    rFrame.rFrameBuf.u4CropWidth = rFrame.rFrameFormat.u2CropWidth;
                    rFrame.rFrameBuf.u4CropHeight = rFrame.rFrameFormat.u2CropHeight;
                    rFrame.rFrameBuf.u4AddressY= rDispFrame.u4BufAddr;
                    rFrame.rFrameBuf.u4AddressC= rDispFrame.u4BufAddrC;
                    rFrame.rFrameBuf.u4Size = rDispFrame.u4BufSize;
                    rFrame.rFrameBuf.u4BufWidth = rDispFrame.u4BufWidth;
                    rFrame.rFrameBuf.u4BufHeight = rDispFrame.u4BufHeight;
                    rFrame.rFrameBuf.u4DispWidth = rDispFrame.u4Width;
                    rFrame.rFrameBuf.u4DispHeight = rDispFrame.u4Height;
                    rFrame.rFrameBuf.u4DispPitch = rDispFrame.u4Pitch;
                    rFrame.rFrameBuf.u4Timestamp = (UINT32)rDispFrame.u8Timestamp;
                    rFrame.rFrameBuf.u8Timestamp = rDispFrame.u8Timestamp;
                    rFrame.rFrameBuf.u4AspectRatioW = rDispFrame.u4ParWidth;
                    rFrame.rFrameBuf.u4AspectRatioH = rDispFrame.u4ParHeight;
                    rFrame.rFrameBuf.u4FrameBufferType = 0;
                    rFrame.rFrameBuf.u4PictureStructure = 0;
                    rFrame.rFrameBuf.u4FrameBufferStatus = 0;
                    rFrame.rFrameBuf.u4Eos = rDispFrame.fgEos;
                    
                    if(rFrame.rFrameBuf.u4Eos)
                    {
                        LOG(3, "%s(%d):  fgEos, index 0x%x\n",  __FUNCTION__, __LINE__, u4FbIndex);
                    }
                    if (rDispFrame.u4FrameBufferStatus & VDEC_FBSTSTUS_NOT_DISPLAY)
                    {
                        rFrame.rFrameBuf.u4FrameBufferStatus |= VDECOMX_FBSTATUS_NOT_DISPLAY;
                    }
                    if (rDispFrame.u4FrameBufferStatus & VDEC_FBSTSTUS_REPEAT_LAST)
                    {
                        rFrame.rFrameBuf.u4FrameBufferStatus |= VDECOMX_FBSTATUS_REPEAT_LAST;
                    }
                    if (rDispFrame.u4FrameBufferStatus & VDEC_FBSTSTUS_NOT_USED)
                    {
                        rFrame.rFrameBuf.u4FrameBufferStatus |= VDECOMX_FBSTATUS_NOT_USED;
                    }
    
                    rGetparam.eRet = VDECOMX_MRESULT_OK;
                }
                // copy to user
                USR_SPACE_ACCESS_VALIDATE_ARG(rGetparam.pvOutParam, VDECOMX_FRAME_T);
                COPY_TO_USER_ARG(rGetparam.pvOutParam, rFrame, VDECOMX_FRAME_T);
            }
            break;

        case VDECOMX_GET_TYPE_GET_SEQUENCE_INFO:
            {
                VDECOMX_SEQINFO_T rSeqinfo;
                VDEC_ES_INFO_T *prVdecEsInfo;
                VDEC_GET_INTO_T rVdecGetInfo;
                x_memset(&rVdecGetInfo, 0, sizeof(VDEC_GET_INTO_T));
                if(!_VPUSH_GetInfo(prVdecOmx->pVPush, &rVdecGetInfo))
                {
                    LOG(0, "_VPUSH_GetInfo fail\n");
                    rGetparam.eRet = VDECOMX_MRESULT_FAIL;
                }
                else
                {
                    prVdecEsInfo = _VDEC_GetEsInfo(rVdecGetInfo.ucVdecId);
                    rGetparam.eRet = VDECOMX_MRESULT_OK;
                    if (!prVdecEsInfo)
                    {
                        LOG(0, "%s(%d): FBM_GetFrameBufferSeqHdr fail\n", __FUNCTION__, __LINE__);
                        rGetparam.eRet = VDECOMX_MRESULT_FAIL;
                    }
                    else
                    {                        
                        FBM_SEQ_HDR_T *prFbmSeqHdr;
                        x_memset(&rSeqinfo, 0, sizeof(VDECOMX_SEQINFO_T));
                        prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecEsInfo->ucFbgId);
                        if(NULL != prFbmSeqHdr)
                        {
                            rSeqinfo.u4PicWidth = prFbmSeqHdr->u2HSize;
                            rSeqinfo.u4PicHeight = prFbmSeqHdr->u2VSize;
                            rSeqinfo.u4Width = prFbmSeqHdr->u2HSize;
                            rSeqinfo.u4Height = prFbmSeqHdr->u2VSize;
                            rSeqinfo.bCropping = prFbmSeqHdr->fgCropping;
                            rSeqinfo.u4CropX = prFbmSeqHdr->u4CropX;
                            rSeqinfo.u4CropY = prFbmSeqHdr->u4CropY;
                            rSeqinfo.u4CropWidth = prFbmSeqHdr->u4CropWidth;
                            rSeqinfo.u4CropHeight = prFbmSeqHdr->u4CropHeight;
                            USR_SPACE_ACCESS_VALIDATE_ARG(rGetparam.pvOutParam, VDECOMX_SEQINFO_T);
                            COPY_TO_USER_ARG(rGetparam.pvOutParam, rSeqinfo, VDECOMX_SEQINFO_T);
                            LOG(1, "%s(%d) ret (%dx%d)\n", __FUNCTION__, __LINE__, rSeqinfo.u4PicWidth, rSeqinfo.u4PicHeight);
                        }
                        else
                        {
                            rGetparam.eRet = VDECOMX_MRESULT_FAIL;
                        }
                    }
                }
            }
            break;
            
        case VDECOMX_GET_TYPE_QUERY_FEEDER_INFO:
            {
                VDEC_FEEDER_INTO_T rVdecFeederInfo;
                VDECOMX_FEEDER_INFO_T rFeederInfo;                
                x_memset(&rFeederInfo, 0, sizeof(VDECOMX_FEEDER_INFO_T));
                if(!prVdecOmx->pVPush)
                {
                    LOG(0, "%s(L:%d) vpush not inited\n", __FUNCTION__, __LINE__);
                    return -1;
                }
                
                if(!_VPUSH_GetFeederInfo(prVdecOmx->pVPush, &rVdecFeederInfo))
                {
                    LOG(0, "%s(L:%d) vpush get feeder info failed\n", __FUNCTION__, __LINE__);
                    return -1;
                }
                rFeederInfo.u4TotalSize = rVdecFeederInfo.u4TotalSize;                
                rFeederInfo.u4BufferCount = rVdecFeederInfo.u4BufferCount;                
                rFeederInfo.u4BufferSize = rVdecFeederInfo.u4BufferSize;
                // copy to user
                USR_SPACE_ACCESS_VALIDATE_ARG(rGetparam.pvOutParam, VDECOMX_FEEDER_INFO_T);
                COPY_TO_USER_ARG(rGetparam.pvOutParam, rFeederInfo, VDECOMX_FEEDER_INFO_T);
            }
             break;
             
        case VDECOMX_GET_TYPE_ALLOCATE_BUFFER:
            {
                PARAMETER_VDECOMX_ALLOCATEBUFFER OMX_AllocBuf_Param;
                VDECOMX_BUFFER_T *pBuffer = NULL; // internal buffer handle
        
                USR_SPACE_ACCESS_VALIDATE_ARG(rGetparam.pvInParam, PARAMETER_VDECOMX_ALLOCATEBUFFER);
                COPY_FROM_USER_ARG(rGetparam.pvInParam, OMX_AllocBuf_Param, PARAMETER_VDECOMX_ALLOCATEBUFFER);

                _vdecomx_alloc_input_buffer(prVdecOmx, OMX_AllocBuf_Param.nSizeBytes, &pBuffer);
                if (NULL != pBuffer)
                {
                    OMX_AllocBuf_Param.hPBufHeaderBuf = pBuffer->hBuffer;
                    LOG(2, "[In] input buf allocated: pBuffer = 0x%X, pBuffer->pOwner = 0x%X.\n", pBuffer, pBuffer->pOwner);
                }
        
                // copy to user
                USR_SPACE_ACCESS_VALIDATE_ARG(rGetparam.pvOutParam, PARAMETER_VDECOMX_ALLOCATEBUFFER);
                COPY_TO_USER_ARG(rGetparam.pvOutParam, OMX_AllocBuf_Param, PARAMETER_VDECOMX_ALLOCATEBUFFER);
            }
            break;

        case VDECOMX_GET_TYPE_GET_KMEM_HANDLER:
            {
                VDECOMX_FRAMEBUF_T rFrameBuf;
                UINT32 u4FbIndex=(UINT32)rGetparam.pvInParam;
                UINT32 u4Index=0;
                
                USR_SPACE_ACCESS_VALIDATE_ARG(rGetparam.pvOutParam, VDECOMX_FRAMEBUF_T);
                COPY_FROM_USER_ARG(rGetparam.pvOutParam, rFrameBuf, VDECOMX_FRAMEBUF_T);

                LOG(5,"vdecomx_getparam _vdecomx_bufferinfor_get_kmemhandler index=%d\n",u4FbIndex);
                u4Index=_vdecomx_bufferinfor_get_kmemhandler(prVdecOmx->rKmemInfor,\
                    u4FbIndex,rFrameBuf.u4AddressY,rFrameBuf.u4AddressC,rFrameBuf.u4Size);
                LOG(5,"vdecomx_getparam _vdecomx_bufferinfor_get_kmemhandler done\n");

                if(u4Index!=VOMX_INVALID_VALUE)
                {
                    LOG(5,"vdecomx_getparam index=%d,YHeader=0x%x,CHeader=0x%x\n",u4Index,
                        prVdecOmx->rKmemInfor[u4Index].hMemHandleY,prVdecOmx->rKmemInfor[u4Index].hMemHandleC);
                    rFrameBuf.u4MemHeaderY=prVdecOmx->rKmemInfor[u4Index].hMemHandleY;
                    rFrameBuf.u4MemHeaderC=prVdecOmx->rKmemInfor[u4Index].hMemHandleC;
                }
                else
                {
                    rFrameBuf.u4MemHeaderY=VOMX_INVALID_VALUE;
                    rFrameBuf.u4MemHeaderC=VOMX_INVALID_VALUE;
                    LOG(0,"%s(%d):GetKmemError: fbidx=%d\n",__FUNCTION__, __LINE__,u4FbIndex);
                }
                COPY_TO_USER_ARG(rGetparam.pvOutParam, rFrameBuf, VDECOMX_FRAMEBUF_T);
            }
            break;
        case VDECOMX_GET_TYPE_GET_VDEC_CNT:
            {
               UINT32 u4DecCnt;
               u4DecCnt=_VPUSH_GetDecoderCnt();
               USR_SPACE_ACCESS_VALIDATE_ARG(rGetparam.pvOutParam, UINT32);
               COPY_TO_USER_ARG(rGetparam.pvOutParam, u4DecCnt, UINT32);
            }
            break;
            
        case VDECOMX_GET_TYPE_QUERY_HW_ABILITY:
            {
#ifdef CC_VDEC_RM_SUPPORT
                ENUM_VDEC_FMT_T eFmt = VDEC_FMT_MAX;
                UINT32 u4FrmRateCod;
                VDECOMX_SET_TYPE_PICTURE_INFO_T rQueryInput;
                USR_SPACE_ACCESS_VALIDATE_ARG(rGetparam.pvInParam, VDECOMX_SET_TYPE_PICTURE_INFO_T);
                COPY_FROM_USER_ARG(rGetparam.pvInParam, rQueryInput, VDECOMX_SET_TYPE_PICTURE_INFO_T);
                u4FrmRateCod = rQueryInput.u4FrameRate * 100;
                eFmt = vdecomx_fmt_map(rQueryInput.eVideoFormat);
                if(!VDEC_VLDMngrQueryCap(eFmt, rQueryInput.u4Width, rQueryInput.u4Height, u4FrmRateCod))
                    return -1;
#endif
            }
            break;

        default:
            rGetparam.eRet = VDECOMX_MRESULT_FAIL;
            break;
    }

    COPY_TO_USER_ARG(arg, rGetparam, PAR_VDECOMX_GETPARAM);

    return 0;
}
#if 1//1//defined(VDEC_YUV_CVT_IMG)

#define VDEC_COLOR_MAX_WIDTH         (1920)
#define VDEC_COLOR_MAX_HEIGHT        (1088)
#define VDEC_COLOR_BUF_SIZE_YUV420       (VDEC_COLOR_MAX_WIDTH*VDEC_COLOR_MAX_HEIGHT*3/2)
BOOL vdecomx_colorcvt_init(VOID)
{
    LOG(3, "[VDEC] %s(L:%d) start call alloc for color cvt buffer\n",__FUNCTION__,__LINE__);
    _u4VdecOmxColorAddrY = (UINT32)BSP_AllocAlignedDmaMemory(VDEC_COLOR_BUF_SIZE_YUV420,32);  //Fix me use this function FBM_GetResizeFrameBufferAddr
    _u4VdecOmxScaleAddrY = (UINT32)BSP_AllocAlignedDmaMemory(VDEC_COLOR_BUF_SIZE_YUV420,32);
    return TRUE;
}
EXPORT_SYMBOL(vdecomx_colorcvt_init);

typedef struct
{
    UINT32 u4AddrY;
    UINT32 u4AddrC;
    UINT16 u2HSize;
    UINT16 u2VSize;
    UINT32 u2LineSize;
    UINT32 u4UfoAddrLenY;
    UINT32 u4UfoAddrLenC;
    UINT8  ucBlockType;
    UINT8  u4UfoEnable;
    UINT8  eUfoType;
    UINT8  eNeedRester;

}IMAGE_CONVERT_SRC_PARAM;

typedef struct
{
    UINT32 u4AddrY;
    UINT32 u4AddrC;
    UINT16 u2HSize;
    UINT16 u2VSize;
    UINT32 u2LineSize;
}IMAGE_CONVERT_DIST_PARAM;

static BOOL __vdecomx_do_image_convert(IMAGE_CONVERT_SRC_PARAM srcImage, IMAGE_CONVERT_DIST_PARAM distImage)
{
    UCHAR ucRzId = 0;     //Fixme Find a way to manager rzID
    RZ_VDO_SCL_PARAM_SET_T rImgrzScaleParam;    

    x_memset(&rImgrzScaleParam,0,sizeof(RZ_VDO_SCL_PARAM_SET_T));

    if (srcImage.u4UfoEnable)
    {
        rImgrzScaleParam.u4UfoEn = TRUE;
        rImgrzScaleParam.eUfoType = srcImage.eUfoType;
        rImgrzScaleParam.u4UfoYBufSZSA = srcImage.u4UfoAddrLenY;
        rImgrzScaleParam.u4UfoCBufSZSA = srcImage.u4UfoAddrLenC;
        distImage.u2HSize = distImage.u2LineSize;
        srcImage.u2HSize = srcImage.u2LineSize;
        LOG(4,"%s u4UfoEnable 10 bit case %d\n",__FUNCTION__,__LINE__);
    }

    rImgrzScaleParam.u4SrcSwap = srcImage.ucBlockType ? 0 : 6;//0 for 16x32, 6 for 64x32
    rImgrzScaleParam.u4OutSwap = 0;
    rImgrzScaleParam.u4InMode = E_RZ_VDO_OUTMD_420;
    rImgrzScaleParam.u4YSrcBase = srcImage.u4AddrY;
    rImgrzScaleParam.u4YSrcBufLen = srcImage.u2LineSize;
    rImgrzScaleParam.u4YSrcHOffset = 0;
    rImgrzScaleParam.u4YSrcVOffset = 0;
    rImgrzScaleParam.u4YSrcW = srcImage.u2HSize;
    rImgrzScaleParam.u4YSrcH = srcImage.u2VSize;
    rImgrzScaleParam.u4CSrcBase = srcImage.u4AddrC;
    rImgrzScaleParam.u4CSrcHOffset = 0;
    rImgrzScaleParam.u4CSrcVOffset = 0;
    rImgrzScaleParam.u4CSrcW = rImgrzScaleParam.u4YSrcW / 2;
    rImgrzScaleParam.u4CSrcH = rImgrzScaleParam.u4YSrcH / 2;
    rImgrzScaleParam.u4IsRsOut = srcImage.eNeedRester;
    rImgrzScaleParam.u4OutMode = E_RZ_VDO_OUTMD_420;
    rImgrzScaleParam.u4IsVdo2Osd = 0;
    rImgrzScaleParam.u4VdoCbCrSwap = 0; //0 for NV12, 1 for NV21
    rImgrzScaleParam.u4YTgBase = distImage.u4AddrY;
    rImgrzScaleParam.u4YTgCM = E_RZ_VDO_OUTMD_420;
    rImgrzScaleParam.u4YTgBufLen = distImage.u2LineSize;
    rImgrzScaleParam.u4YTgHOffset = 0;
    rImgrzScaleParam.u4YTgVOffset = 0;
    rImgrzScaleParam.u4YTgW = distImage.u2HSize;
    rImgrzScaleParam.u4YTgH = distImage.u2VSize;
    rImgrzScaleParam.u4CTgBase = distImage.u4AddrC;
    rImgrzScaleParam.u4CTgW = rImgrzScaleParam.u4YTgW / 2;
    rImgrzScaleParam.u4CTgH = rImgrzScaleParam.u4YTgH / 2;

    IMGRZ_Lock_Ex((UINT32)ucRzId);
    IMGRZ_ReInit_Ex((UINT32)ucRzId);
    IMGRZ_SetScaleOpt_Ex((UINT32)ucRzId, E_RZ_VDO_OUTMD_420);
    IMGRZ_Scale_Ex((UINT32)ucRzId, (void *)(&rImgrzScaleParam));
    IMGRZ_Flush_Ex((UINT32)ucRzId);
    IMGRZ_Wait_Ex((UINT32)ucRzId);
    IMGRZ_Unlock_Ex((UINT32)ucRzId);
    
    HalFlushInvalidateDCacheMultipleLine(((VIRTUAL)(_u4VdecOmxColorAddrY)), VDEC_COLOR_BUF_SIZE_YUV420);

    LOG(6,"%d ImgrzScale:SH:%d SV:%d SL:%d DH:%d DV%d DL%d UFOY:%d UFOC%d BT:%d UE%d UT%d NR%d \n",
         __LINE__,srcImage.u2HSize, srcImage.u2VSize, 
        srcImage.u2LineSize, distImage.u2HSize, distImage.u2VSize, distImage.u2LineSize, srcImage.u4UfoAddrLenY, srcImage.u4UfoAddrLenC, 
        srcImage.ucBlockType, srcImage.u4UfoEnable, srcImage.eUfoType, srcImage.eNeedRester);

    return 0;
}

#define COLOR_CONVERT_SOURCE_NORMAL (0)
#define COLOR_CONVERT_SOURCE_UFO (1)
#define COLOR_CONVERT_SOURCE_INVAILD_ARG (-1)
#define COLOR_CONVERT_SOURCE_CROP_ERROR (-2)

inline static INT32 __vdec_generate_srcImage(VDECOMX_SET_COLOR_CVT_T* eDrvColorCvtParam, IMAGE_CONVERT_SRC_PARAM* outImage)
{
    INT32 ret = COLOR_CONVERT_SOURCE_NORMAL;
    UCHAR ucFbgId;
    UCHAR ucFbId;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;
    FBM_PIC_HDR_T* prFbmPicHdr = NULL;
    UINT32 u4BufIndex=(UINT32)eDrvColorCvtParam->u4SrcBufIndex;

    ucFbgId = (u4BufIndex>>16)&0xff;
    ucFbId = u4BufIndex&0xff;
    FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &(outImage->u4AddrY), &(outImage->u4AddrC));
    prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
    if(NULL == prFbmPicHdr || NULL == prFbmSeqHdr)
    {
        LOG(0,"%s(%d) convert to nv12 failed, fbg or fb not exist\n",__FUNCTION__,__LINE__);
        return COLOR_CONVERT_SOURCE_INVAILD_ARG;
    }
    
    outImage->u2LineSize = prFbmSeqHdr->u2LineSize;
    outImage->u2HSize = prFbmSeqHdr->u2HSize;
    outImage->u2VSize = prFbmSeqHdr->u2VSize;
    outImage->ucBlockType = prFbmSeqHdr->ucBlockType;

    LOG(6,"L:%d H:%d V%d Line%d \n",outImage->u2LineSize, outImage->u2HSize, outImage->u2VSize,  __LINE__);

    if(prFbmPicHdr->u4PicWidth && prFbmPicHdr->u4PicHeight && prFbmPicHdr->u4PicWidthPitch)
    {
        //all codec output is 64 align now
        outImage->u2LineSize = (prFbmPicHdr->u4PicWidth + 63) & (~63);//prFbmPicHdr->u4PicWidthPitch;
        outImage->u2HSize  = prFbmPicHdr->u4PicWidth;
        outImage->u2VSize = prFbmPicHdr->u4PicHeight;
      LOG(6,"L:%d H:%d V%d Line%d \n",outImage->u2LineSize, outImage->u2HSize, outImage->u2VSize,  __LINE__);
    }

    if(prFbmPicHdr->fgCropping)
    {
        if((prFbmPicHdr->u4CropX == 0) && (prFbmPicHdr->u4CropY == 0))
        {
            outImage->u2HSize = prFbmPicHdr->u4CropWidth;
            outImage->u2VSize = prFbmPicHdr->u4CropHeight;
            LOG(6,"L:%d H:%d V%d Line%d \n",outImage->u2LineSize, outImage->u2HSize, outImage->u2VSize,  __LINE__);
        }
        else
        {
            LOG(0,"%s(%d) convert to nv12 failed, crop.x==0 or crop.y==0\n",__FUNCTION__,__LINE__);
            return COLOR_CONVERT_SOURCE_CROP_ERROR;
        }
    }
    
    outImage->eNeedRester = 1;
  //Preprocess for UFO and 10 pixel case
    if((prFbmSeqHdr->fgUfoEn) || (10 == prFbmSeqHdr->u1Depth))
    {
         UINT32 u4UfoAddrLenY;
         UINT32 u4UfoAddrLenC;
         FBM_GetFrameBufferAddrExt(ucFbgId, ucFbId, &u4UfoAddrLenY,&u4UfoAddrLenC);
         outImage->u4UfoEnable = TRUE;
         outImage->eUfoType = (10 == prFbmSeqHdr->u1Depth) ? RZ_UFO_TYPE_10BIT_COMPACT : RZ_UFO_TYPE_8BIT;
         outImage->u4UfoAddrLenY = u4UfoAddrLenY;
         outImage->u4UfoAddrLenC = u4UfoAddrLenC;
         ret = COLOR_CONVERT_SOURCE_UFO;
    }

    LOG(6,"%d outImage: Y:%d C:%d H:%d V:%d L:%d UFOY:%d UFOC%d BT:%d UE%d UT%d NR%d \n",
         __LINE__, outImage->u4AddrY,outImage->u4AddrC,outImage->u2HSize, outImage->u2VSize, 
        outImage->u2LineSize, outImage->u4UfoAddrLenY, outImage->u4UfoAddrLenC, 
        outImage->ucBlockType, outImage->u4UfoEnable, outImage->eUfoType, outImage->eNeedRester);

    return ret;
}

BOOL vdecomx_colorcvtNV12(VDECOMX_SET_COLOR_CVT_T* eDrvColorCvtParam)
{
    INT32 result = 0;
    INT32 u4VdecTemp;
    IMAGE_CONVERT_SRC_PARAM srcImage;
    IMAGE_CONVERT_DIST_PARAM distImage;
    x_memset(&srcImage,0,sizeof(IMAGE_CONVERT_SRC_PARAM));
    x_memset(&distImage,0,sizeof(IMAGE_CONVERT_DIST_PARAM));

    result = __vdec_generate_srcImage(eDrvColorCvtParam, &srcImage);

    switch (result)
    {
    case COLOR_CONVERT_SOURCE_INVAILD_ARG:
    case COLOR_CONVERT_SOURCE_CROP_ERROR:
        {
              LOG(0,"%s vdecomx_colorcvtNV12 source not ready %d\n",__FUNCTION__,__LINE__);
              return result;
        }
        break;
    case COLOR_CONVERT_SOURCE_UFO:
        {
          if(srcImage.u2LineSize != eDrvColorCvtParam->u2TGStride)
            {
                /*unpacket the 10bit pixel to 8 bit pixel*/

                IMAGE_CONVERT_DIST_PARAM ufodistImage;
                x_memset(&ufodistImage,0,sizeof(IMAGE_CONVERT_DIST_PARAM));
                srcImage.eNeedRester = 0;
				
				if (srcImage.u2HSize * srcImage.u2VSize > VDEC_COLOR_MAX_WIDTH*VDEC_COLOR_MAX_HEIGHT)
				{
					// workaround for 4K2K
					FBM_POOL_T *prFbmPool;
					
			        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_VENC);
			        if(!prFbmPool)
			        {
			            LOG(0,"%s prFbmPool null\n",__FUNCTION__);
			            return -1;
			        }
					
			        if ((prFbmPool->u4Size == 0) || (prFbmPool->u4Addr == 0))
			        {
			            LOG(0,"%s prFbmPool buffer null\n",__FUNCTION__);
			            return -1;
			        }

			    	ufodistImage.u4AddrY = prFbmPool->u4Addr;
			    	ufodistImage.u4AddrC = prFbmPool->u4Addr + srcImage.u2HSize * srcImage.u2VSize;
				}
				else
				{
	                ufodistImage.u4AddrY = _u4VdecOmxScaleAddrY;
	                ufodistImage.u4AddrC = _u4VdecOmxScaleAddrY + VDEC_COLOR_MAX_WIDTH*VDEC_COLOR_MAX_HEIGHT;
				}
				
                ufodistImage.u2HSize = srcImage.u2HSize;
                ufodistImage.u2VSize = srcImage.u2VSize;
                ufodistImage.u2LineSize = srcImage.u2LineSize;
                LOG(4,"UFO: L:%d, H:%d, V%d, %d \n",srcImage.u2LineSize,srcImage.u2HSize, srcImage.u2VSize, __LINE__);
                __vdecomx_do_image_convert(srcImage, ufodistImage);
                /*Update the srcImage buffer to do further color convert*/
                srcImage.u4UfoEnable = 0;
                srcImage.eNeedRester = 1;
                srcImage.u4AddrY = ufodistImage.u4AddrY;
                srcImage.u4AddrC = ufodistImage.u4AddrC;
            }
        }
    case COLOR_CONVERT_SOURCE_NORMAL:
        {
            distImage.u2LineSize = eDrvColorCvtParam->u2TGStride;
            distImage.u2HSize = eDrvColorCvtParam->u2HTGSize;
            distImage.u2VSize = eDrvColorCvtParam->u2VTGSize;

			if (distImage.u2HSize * distImage.u2VSize > VDEC_COLOR_MAX_WIDTH*VDEC_COLOR_MAX_HEIGHT)
			{
				// workaround for 4K2K
				FBM_POOL_T *prFbmPool;
				
				prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_VENC);
				if(!prFbmPool)
				{
					LOG(0,"%s prFbmPool null\n",__FUNCTION__);
					return -1;
				}
				
				if ((prFbmPool->u4Size == 0) || (prFbmPool->u4Addr == 0))
				{
					LOG(0,"%s prFbmPool buffer null\n",__FUNCTION__);
					return -1;
				}
			
				distImage.u4AddrY = prFbmPool->u4Addr;
				distImage.u4AddrC = prFbmPool->u4Addr + distImage.u2HSize * distImage.u2VSize;
			}
			else
			{
	            distImage.u4AddrY = _u4VdecOmxColorAddrY;
	            distImage.u4AddrC = _u4VdecOmxColorAddrY + VDEC_COLOR_MAX_WIDTH*VDEC_COLOR_MAX_HEIGHT;
			}
			
            LOG(4,"Finish L:%d, H:%d, V:%d Line:%d\n",distImage.u2LineSize, distImage.u2HSize, distImage.u2VSize, __LINE__);
            __vdecomx_do_image_convert(srcImage,distImage);



        }
        break;
    default:
        break;
    }

     u4VdecTemp = VIRTUAL(distImage.u4AddrY);

     if (copy_to_user((void __user *)eDrvColorCvtParam->u4DstAddr, (void*)(u4VdecTemp),
                           (distImage.u2LineSize * distImage.u2VSize)))
     {
          LOG(1,"%s vdecomx_mod argument error %d\n",__FUNCTION__,__LINE__);
          return -1;
     }
     u4VdecTemp = VIRTUAL(distImage.u4AddrC);
     if (copy_to_user((void __user *)(eDrvColorCvtParam->u4DstAddr + distImage.u2LineSize * distImage.u2VSize), (void*)(u4VdecTemp),
                            (distImage.u2LineSize * distImage.u2VSize / 2)))
     {
           LOG(0,"%s vdecomx_mod argument error %d\n",__FUNCTION__,__LINE__);
           return -1;
     }
    

    return 0;
}
#endif

int vdecomx_setparam(VDECOMX_HANDLE_T *prVdecOmx, unsigned long arg)
{
//    BOOL fgRet;;
//    UINT32 u4Addr;
    PAR_VDECOMX_SETPARAM rSetparam;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, PAR_VDECOMX_SETPARAM);
    COPY_FROM_USER_ARG(arg, rSetparam, PAR_VDECOMX_SETPARAM);

    rSetparam.eRet = VDECOMX_MRESULT_OK;

    switch (rSetparam.eType)
    {
        case VDECOMX_SET_TYPE_SET_DECODE_MODE:
            {
                VDEC_SET_INTO_T rVdecSetInfo;
                VDECOMX_DECODE_MODE_T eDrvDecodeMode;
                x_memset(&rVdecSetInfo, 0, sizeof(VDEC_SET_INTO_T));
                rVdecSetInfo.u4InfoMask = VDEC_PUSH_SET_INFO_DECODE_MODE;
                if(!rSetparam.pvInParam)
                {
                    LOG(0, "%s(L:%d) pvInParam null\n", __FUNCTION__, __LINE__);
                    return -1;
                }
                eDrvDecodeMode = ((VDECOMX_SET_DECODE_MODE_T*)rSetparam.pvInParam)->eDecodeMode;
                switch(eDrvDecodeMode)
                {
                    case VDECOMX_DECODE_MODE_I_ONLY:
                        rVdecSetInfo.eDecodeMode = VDEC_DECODE_MODE_I_ONLY;
                        break;
                    case VDECOMX_DECODE_MODE_B_SKIP:
                        rVdecSetInfo.eDecodeMode = VDEC_DECODE_MODE_B_SKIP;
                        break;
                    case VDECOMX_DECODE_MODE_NO_REORDER:
                        rVdecSetInfo.eDecodeMode = VDEC_DECODE_MODE_NO_REORDER;
                        break;
                    case VDECOMX_DECODE_MODE_NORMAL:
                        rVdecSetInfo.eDecodeMode = VDEC_DECODE_MODE_NORMAL;
                        break;
                    case VDECOMX_DECODE_MODE_DROPFRAME:
                    case VDECOMX_DECODE_MODE_UNKNOWN:
                        rVdecSetInfo.eDecodeMode = VDEC_DECODE_MODE_UNKNOWN;
                        break;
                }
                LOG(1,"setparam DecMode=%d\n",rVdecSetInfo.eDecodeMode);
                if(!_VPUSH_SetInfo(prVdecOmx->pVPush, &rVdecSetInfo))
                {
                    LOG(0, "%s(L:%d) _VPUSH_SetInfo fail\n", __FUNCTION__, __LINE__);
                    return -1;
                }

            }
            break;
        case VDECOMX_SET_TYPE_SET_BUFTAG:
            {
                PARAMETER_VDECOMX_FILLBTAG rBufferTag;
                USR_SPACE_ACCESS_VALIDATE_ARG(rSetparam.pvInParam, PARAMETER_VDECOMX_FILLBTAG);
                COPY_FROM_USER_ARG(rSetparam.pvInParam,rBufferTag,PARAMETER_VDECOMX_FILLBTAG);
                LOG(3,"[Out] tag Frame[%d, %d] with[0x%x]\n",(rBufferTag.nDecodeBufId>>16)&0xff, rBufferTag.nDecodeBufId&0xff, rBufferTag.nTag);
                FBM_SetDispTag((rBufferTag.nDecodeBufId>>16)&0xff, rBufferTag.nDecodeBufId&0xff, rBufferTag.nTag);
                break;
            }
        case VDECOMX_SET_TYPE_SET_DROP_TAGBUFFER:
            {
                UINT32 u4BufTag=(UINT32)rSetparam.pvInParam;
                LOG(4,"[Out] release Frame by Tag[0x%x]\n",u4BufTag);
                FBM_ReleaseBufferByDispTag(u4BufTag);
                break;
            }
        case VDECOMX_SET_TYPE_SET_DROP_UNTAGBUFFER:
            {
                UINT32 u4BufIndex=(UINT32)rSetparam.pvInParam;
                LOG(3,"[Out] release Frame[%d, %d]\n",(u4BufIndex>>16)&0xff, u4BufIndex&0xff);
                FBM_ReleaseBufferByIndex((u4BufIndex>>16)&0xff,u4BufIndex&0xff);
                break;
            }
        case VDECOMX_SET_TYPE_SET_PICTURE_INFO:
            {
                VDECOMX_SET_TYPE_PICTURE_INFO_T* pPicInfo = (VDECOMX_SET_TYPE_PICTURE_INFO_T*)(rSetparam.pvInParam);
                _VPUSH_SetPicSz(prVdecOmx->pVPush, pPicInfo->u4Width, pPicInfo->u4Height, pPicInfo->u4FrameRate);
            }
            break;
        case VDECOMX_SET_TYPE_SET_COLOR_CONVERT:
            {
                VDECOMX_SET_COLOR_CVT_T* pDrvColorCvtParam;
                if(!rSetparam.pvInParam)
                {
                    LOG(0, "%s(L:%d) pvInParam null\n", __FUNCTION__, __LINE__);
                    return -1;
                }
                pDrvColorCvtParam = (VDECOMX_SET_COLOR_CVT_T*)rSetparam.pvInParam;
                vdecomx_colorcvtNV12(pDrvColorCvtParam);
            }
            break;
            
        case VDECOMX_SET_TYPE_FREE_BUFFER:
            {
                int i4Ret = -1;
                PARAMETER_VDECOMX_FREEBUFFER OMX_FreeBuf_Param;
                
                USR_SPACE_ACCESS_VALIDATE_ARG(rSetparam.pvInParam, PARAMETER_VDECOMX_FREEBUFFER);
                COPY_FROM_USER_ARG(rSetparam.pvInParam, OMX_FreeBuf_Param, PARAMETER_VDECOMX_FREEBUFFER);

                i4Ret = _vdecomx_free_input_buffer(prVdecOmx, OMX_FreeBuf_Param.hPBufHeaderBuf);
                if (-1 == i4Ret)
                {
                    rSetparam.eRet = VDECOMX_MRESULT_FAIL;
                }
            }
            break;
        case VDECOMX_SET_TYPE_FLUSH_DEOCDER:
            {
                LOG(0,"omx flush");
                prVdecOmx->fgInFlush = TRUE;
                if(!_VPUSH_FlushSync(prVdecOmx->pVPush))
                {
                    LOG(0, "%s(L:%d) _VPUSH_FlushSync fail\n", __FUNCTION__, __LINE__);                    
                    prVdecOmx->fgInFlush = FALSE;
                    return -1;
                }                
                LOG(0,"omx flush done\n");
                prVdecOmx->fgInFlush = FALSE;
            }
            break;
        case VDECOMX_SET_TYPE_SET_SEAMLESS_INFO:
            {
                VDECOMX_SET_TYPE_SEAMLESS_INFO_T* pSeamlessInfo = (VDECOMX_SET_TYPE_SEAMLESS_INFO_T*)(rSetparam.pvInParam);
                
                if (TRUE == pSeamlessInfo->fgseamless)
                {
                    if(!_VPUSH_SetSeamlessMode(prVdecOmx->pVPush, pSeamlessInfo->u4MaxWidth, pSeamlessInfo->u4MaxHeight, pSeamlessInfo->u4ResizerId))
                    {
                        LOG(3, "%s(L:%d) _VPUSH_SetSeamlessMode fail\n", __FUNCTION__, __LINE__);
                        return -1;
                    }
                }
            }
            break;
        case VDECOMX_SET_TYPE_SET_VDP_ID:
           {
               VDECOMX_SET_TYPE_VDP_INFO_T* pVdpInfo = (VDECOMX_SET_TYPE_VDP_INFO_T*)(rSetparam.pvInParam);
               VDEC_SET_INTO_T rVdecSetInfo;
               x_memset(&rVdecSetInfo, 0, sizeof(VDEC_SET_INTO_T));
               rVdecSetInfo.ucVdpId = pVdpInfo->u4Id;                
               rVdecSetInfo.u4InfoMask = VDEC_PUSH_SET_INFO_VDP_ID;
               if(!_VPUSH_SetInfo(prVdecOmx->pVPush, &rVdecSetInfo))
               {
                   LOG(0, "%s(L:%d) _VPUSH_SetInfo fail\n", __FUNCTION__, __LINE__);
                   return -1;
               }
           }
           break;
        case VDECOMX_SET_TYPE_VDEC_PAUSE:
           {
               if(prVdecOmx->fgPlay)
               {
                   if(!_VPUSH_PauseVdec(prVdecOmx->pVPush))
                   {
                       LOG(3, "%s(L:%d) _VPUSH_PauseVdec fail\n", __FUNCTION__, __LINE__);
                       return -1;
                   }
               }
               else
               {               
                    LOG(0, "%s(L:%d) pending requested\n", __FUNCTION__, __LINE__);
                    prVdecOmx->fgPending = TRUE;
               }
           }
           break;
        case VDECOMX_SET_TYPE_VDEC_RESUME:
           {
               if(!_VPUSH_PlayVdec(prVdecOmx->pVPush))
               {
                   LOG(3, "%s(L:%d) _VPUSH_PlayVdec\n", __FUNCTION__, __LINE__);
                   return -1;
               }
               prVdecOmx->fgPlay = TRUE;               
               prVdecOmx->fgPending = FALSE;
           }
           break;
        default:
            rSetparam.eRet = VDECOMX_MRESULT_FAIL;
            break;
    }

    COPY_TO_USER_ARG(arg, rSetparam, PAR_VDECOMX_SETPARAM);

    return 0;
}

int vdecomx_release(struct inode *inode, struct file *pfile)
{
//    UINT32 i;
    VDECOMX_HANDLE_T *prVdecOmx;
    VDECOMX_BUFFER_T *pBufNode = NULL;
    UINT32 u4Index;
    #if 0
    //?????
    if(!_fgInited)
    {
        LOG(5, "%s(L:%d) init\n", __FUNCTION__, __LINE__);
        _VPUSH_DecodeInit();
        _fgInited = TRUE;
        return -1;
    }
    #endif

    if(!pfile)
    {
        LOG(0, "%s(L:%d) pfile null\n", __FUNCTION__, __LINE__);
        return -1;
    }
    prVdecOmx = (VDECOMX_HANDLE_T*)pfile->private_data;
    if(!prVdecOmx)
    {
        LOG(0, "%s(L:%d) prVdecOmx null\n", __FUNCTION__, __LINE__);
        return -1;
    }

    for (u4Index = 0; u4Index < VDECOMX_INPUT_BUF_MAX; u4Index ++)
    {
        pBufNode = &(_prVDOOmxInputBuf[u4Index]);
        if ((NULL != pBufNode) && (NULL_HANDLE != pBufNode->hBuffer))
        {
            _vdecomx_free_input_buffer(prVdecOmx, pBufNode->hBuffer);
        }
    }
    
    if(prVdecOmx->fgPlay)
    {
        if(!_VPUSH_StopSync(prVdecOmx->pVPush))
        {
            LOG(0, "%s(L:%d) _VPUSH_StopSync fail\n", __FUNCTION__, __LINE__);
            return -1;
        }
        prVdecOmx->fgPlay = FALSE;
    }

    _u4VdecOmxOpenCnt --;

    if (0 == _u4VdecOmxOpenCnt)
    {
        // do something?
        _vdecomx_buffer_uninit();
    }

    prVdecOmx->eFmt = VDEC_FMT_MAX;
    if(prVdecOmx->pVPush)
    {
        _VPUSH_ReleaseVideoDecoder(prVdecOmx->pVPush);
        prVdecOmx->pVPush = NULL;
    }
    _vdecomx_bufferinfor_release_kmemhandler(prVdecOmx->rKmemInfor);

    prVdecOmx->fgOccupied = FALSE;
    pfile->private_data = NULL;

//        VERIFY(x_sema_unlock(_hVecOmxSema) == OSR_OK);
    return 0;
}



//-----------------------------------------------------------------------------
/** vdec_ioctl.
 */
//-----------------------------------------------------------------------------
int vdecomx_ioctl(struct inode *inode,
                struct file *pfile,
                unsigned int cmd,
                 unsigned long arg)
{
    INT32  i4Ret = 0;
    VDECOMX_HANDLE_T *prVdecOmx = (VDECOMX_HANDLE_T *)pfile->private_data;
    if(!prVdecOmx)
    {
        return -1;
    }

    switch (cmd)
    {
        case VDEXOMX_CREATE:
            i4Ret = vdecomx_create(prVdecOmx, arg);
            break;
        case VDEXOMX_INIT:
            i4Ret = vdecomx_init(prVdecOmx, arg);
            break;
        case VDEXOMX_DECODE:
            i4Ret = vdecomx_decode(prVdecOmx, arg);
            break;
        case VDEXOMX_GETPARAM:
            i4Ret = vdecomx_getparam(prVdecOmx, arg);
            break;
        case VDEXOMX_SETPARAM:
            i4Ret = vdecomx_setparam(prVdecOmx, arg);
            break;
        case VDEXOMX_DEINIT:
            i4Ret = vdecomx_deinit(prVdecOmx, arg);
            break;
        case VDEXOMX_GETLOGMASK:
            USR_SPACE_ACCESS_VALIDATE_ARG(arg, UINT32);
            COPY_TO_USER_ARG(arg, _u4VdecOmxLogValue, UINT32);
            break;
        default:
            break;
    }

    return i4Ret;
}

#ifdef USE_UNLOCK_IOCTL
long vdecomx_unlocked_ioctl(struct file *pfile, unsigned int cmd, unsigned long param)
{
    return vdecomx_ioctl(0, pfile, cmd, param);
}
#endif

struct file_operations vdecomx_fops = {
    .owner              = THIS_MODULE,
    .open               = vdecomx_open,
#ifdef USE_UNLOCK_IOCTL
    .unlocked_ioctl     = vdecomx_unlocked_ioctl,
#else
    .ioctl              = vdecomx_ioctl,
#endif
    .release            = vdecomx_release,
};


#endif


