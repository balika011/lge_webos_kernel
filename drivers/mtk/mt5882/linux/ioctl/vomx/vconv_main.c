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

/*! \addtogroup VideoConvert
* @{
*/

/*! \file vconv_main.c
*
* $Workfile: $
*
* \par Project
*   MT8580
*
* \par Description
*   The function bodies of video Convert module
*
* \par Author
*   Qing Li
*
* \par Last changed
*   $Author: p4admin $
*   $Modtime: $
*   $Revision: #1 $
*/

#if defined(LINUX_TURNKEY_SOLUTION) && defined(CC_ENABLE_VOMX)

#include "vomx_if.h"
#include "vdec_omx_debug.h"
#include "x_typedef.h"
#include "x_os.h"
#include "x_assert.h"
#include "x_printf.h"

#include "drv_common.h"

#include <asm/page.h>
#include <asm/uaccess.h>  // for using copy_to_user()
#include <linux/module.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/semaphore.h>

#include "vconv.h"
#include "fbm_drvif.h"

#include "vdec_drvif.h"
#include "vdec_mod.h"
#include "imgrz_if.h"
#include "imgrz_drvif.h"
#include "gfx_if.h"
#include "gfx_drvif.h"
#include "vdec_if.h" 

#define VCONV_INST_NUM          VDO_COMP_MAX
#define VCONV_CMD_SIZE          2
#define VCONV_CMD_Q_SIZE        20   //may be same as vfifo buf count
#define VCONV_SRC_Q_SIZE        256
#define VCONV_INDEV_TYPE        64
#define VCONV_MAX_FB_COUNT      (6)
#ifdef CC_FBM_INT_VOMX
#define VCONV_MAX_ARGB_FB_COUNT (6)
#else
#define VCONV_MAX_ARGB_FB_COUNT (6)
#endif
#define VCONV_MAX_YUV420_FB_COUNT     6
#define VCONV_MAX_YUV420SEMI_FB_COUNT 6    //5    // sycn with VDO_OUTPUT_BUF_MAX
#define VCONV_MAX_WIDTH         (1920)
#define VCONV_MAX_HEIGHT        (1088)
#define VCONV_MAX_WIDTH_ARGB    (1920)
#define VCONV_MAX_HEIGHT_ARGB   (1088)
#define VCONV_BUF_SIZE          (1024)//VCONV_MAX_WIDTH*VCONV_MAX_HEIGHT*3/2)
#define VCONV_BUF_SIZE_FBG      (VCONV_MAX_WIDTH*VCONV_MAX_HEIGHT*3/2)
#define VCONV_BUF_SIZE_ARBG     (VCONV_MAX_WIDTH_ARGB*VCONV_MAX_HEIGHT_ARGB*4)
#define VCONV_BUF_SIZE_YUV420   (VCONV_MAX_WIDTH*VCONV_MAX_HEIGHT*3/2)
#define VCONV_BUF_SIZE_YUV420SEMI     (VCONV_MAX_WIDTH*VCONV_MAX_HEIGHT*3/2)
#define VCONV_FRMRATE_UNKNOWN   (0xffffffff) 

#define VCONV_INVALID_DW        0xffffffff
#define VCONV_INVALID_W         0xffff
#define VCONV_INVALID_B         0xff

#define VCONV_SRC_INDEV_ID      0
#define VCONV_DST_INDEV_ID      0

#define VCONV_DEFAULT_COLOR     1

typedef enum
{
    VCONV_EVENT_NONE,
    VCONV_EVENT_CMD,
    VCONV_EVENT_PAUSE,
    VCONV_EVENT_WAIT_CHANGE_FBM_BUF,
    VCONV_EVENT_WAIT_NEXT_PU
} VCONV_EVENT_T;

typedef union
{
    VCONV_FILL_FBM_BUF_PARAM_T  rFillBufParam;
} VCONV_CMD_PARAM_U;

typedef struct
{
    VCONV_CMD_TYPE_T    eCmd;
    VCONV_CMD_PARAM_U   uCmdParam;
} VCONV_CMD_T;

typedef struct
{
    UCHAR   ucFbgId;
    UCHAR   ucFbId;
    BOOL    fgEos;
} VCONV_SRC_T;

typedef struct
{
    //UINT8       u1SrcFbId;
    UINT32      u4FbAddr;   // might be user addr
    UINT32      u4InternalFbAddr;
    UINT32      u4InternalFbAddrC;
    UINT32      u4FbW;
    UINT32      u4FbH;
} VCONV_FB_INFO_T;

typedef struct
{

} VCONV_INFO_T;

typedef struct
{
    UINT32                      u4DstBufSize;
    UINT32                      u4DstFillSize;
} VCONV_FILL_T;

typedef struct
{
    UINT8                       u1FbCount;
    UINT8                       u1QFbCount;
    UINT8                       u1QCmdCount;

    VCONV_SRC_T                 rSrc;

    VCONV_MAIN_STATE_T          eMainSt;
    VCONV_SUB_STATE_T           eSubSt;

    VCONV_EVENT_T               eEventWait;

    HANDLE_T                    hCmdQ;
    HANDLE_T                    hSrcQ;
    HANDLE_T                    hSema;
    HANDLE_T                    hEvent;

    UINT32                      u4InstId;
    UINT32                      u4GfxHandle;

    UINT32                      u4DstFbgAddr;
    UINT32                      u4DstFbgSize;

    UINT32                      u4NeedAllocFbWidth;
    UINT32                      u4NeedAllocFbHeight;
    UINT32                      u4NeedAllocFbCount;

    UINT32                      u4NeedFreeFb;
    UINT32                      u4ChangeFb;

    UINT32                      u4FrameWidth;
    UINT32                      u4FrameHeight;
    UINT32                      u4Stride;

    UINT32                      u4InternalAddr;
    UINT32                      u4InternalAddrC;
    UINT32                      u4CurIdx;

    BOOL                        fgGetHandle;
    BOOL                        fgSendEos;
    BOOL                        fgUserBuf;
    BOOL                        fgAllocBuf;
    BOOL                        fgRemovePitch;

    vOmxVdecVConvCbFunc         pvCbFunc;
    void*                       pvCbPrivData;
    void*                       pvVPush;

    VCONV_COLOR_T               eColor;

    VCONV_FB_INFO_T             arFbInfo[VCONV_MAX_FB_COUNT];

    VCONV_FILL_T                rCurrFill;

    VCONV_INFO_T                rInfo;

    VCONV_FILL_FBM_BUF_PARAM_T  rFillParam;

    HANDLE_T                    hSemaThreadReady;// vconv thread was created and ready 
    UINT32                      fgFirstRun;
    struct semaphore           rConvSema;
	UINT32						u4FrameRate;
} VCONV_INST_T;

static FBM_POOL_T* _prFbmMPEG;
static FBM_POOL_T* _prFbmMPEG2;
static BOOL _bPoolInfoInited;
BOOL _bOutputFisrtCb[VCONV_INST_NUM] = {TRUE};
VCONV_INST_T    _arVConvInfo[VCONV_INST_NUM];

HANDLE_T    _ahVConvThreadHandle[VCONV_INST_NUM];
UINT32      _au4VConvThreadActive[VCONV_INST_NUM];


/***************************** declaration **********************************/

//extern static VCONV_INST_T* prGetInst(UINT32 u4InstId);
static void vVConv_ChangeSubState(VCONV_INST_T* prInst, VCONV_SUB_STATE_T eChangeSubState);

/***************************** dbg Function **********************************/

/*************************** Internal Function *******************************/

static void vVConv_CreateEventGroup(VCONV_INST_T* prInst, CHAR* szBuf)
{
    VERIFY(x_ev_group_create(&(prInst->hEvent), szBuf, 0) == OSR_OK);
}

static void vVConv_DeleteEventGroup(VCONV_INST_T* prInst)
{
    x_ev_group_delete(prInst->hEvent);
}

static void vVConv_SetEvent(VCONV_INST_T* prInst, VCONV_EVENT_T eSetEvent)
{
    if ((VCONV_EVENT_NONE != prInst->eEventWait)||
        (VCONV_EVENT_CMD == eSetEvent))
    {
        if ((VCONV_EVENT_NONE != prInst->eEventWait) && 
            (prInst->eEventWait != eSetEvent) &&
            (VCONV_EVENT_CMD != eSetEvent))
        {
            LOG(7, "[VConv] [ERR] set event %d is not wait event %d\n",
                eSetEvent, prInst->eEventWait);
        }
        else
        {
            VERIFY(OSR_OK == x_ev_group_set_event(prInst->hEvent,
                                                  eSetEvent,
                                                  X_EV_OP_OR));

            LOG(7, "[VConv] [event] set event %d\n", eSetEvent);
        }
    }
    else
    {
        LOG(7, "[VConv] [ERR] want to set event %d, but now wait for VCONV_EVENT_NONE\n",
            eSetEvent);

        //ASSERT(0);
    }
}

static void vVConv_WaitEvent(VCONV_INST_T* prInst, VCONV_EVENT_T eWaitEvent)
{
    EV_GRP_EVENT_T eGetEvent;

    if(eWaitEvent != VCONV_EVENT_NONE)
    {
        prInst->eEventWait = eWaitEvent;

        LOG(7, "[VConv] [event] start wait event %d\n", eWaitEvent);

        VERIFY(OSR_OK == x_ev_group_wait_event(prInst->hEvent,
                                     eWaitEvent,
                                     &eGetEvent,
                                     X_EV_OP_OR_CONSUME));

        LOG(7, "[VConv] [event] get event %d\n", eWaitEvent);

        prInst->eEventWait = VCONV_EVENT_NONE;
    }
}

static void vVConv_ClearEvent(VCONV_INST_T* prInst)
{
    if (VCONV_EVENT_NONE != prInst->eEventWait)
    {
        VERIFY(OSR_OK == x_ev_group_set_event(prInst->hEvent,
                                              prInst->eEventWait,
                                              X_EV_OP_OR));

        LOG(7, "[VConv] [event] set event %d for clear\n",
                    prInst->eEventWait);
    }
}

static void vVConv_CreateSema(VCONV_INST_T* prInst)
{
    VERIFY (x_sema_create(&prInst->hSema, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);
}

static void vVConv_DeleteSema(VCONV_INST_T* prInst)
{
    VERIFY (x_sema_delete(prInst->hSema) == OSR_OK);
}

/*
static void vVConv_LockSema(VCONV_INST_T* prInst)
{
    VERIFY (x_sema_lock(prInst->hSema, X_SEMA_OPTION_WAIT) == OSR_OK);
}

static void vVConv_UnlockSema(VCONV_INST_T* prInst)
{
    VERIFY (x_sema_unlock(prInst->hSema) == OSR_OK);
}
*/

static SIZE_T zVConv_GetCmdSize(VOID)
{
    return (sizeof(VCONV_CMD_T));
}

static SIZE_T zVConv_GetSrcSize(VOID)
{
    return (sizeof(VCONV_SRC_T));
}

static void vVConv_CreateCmdQ(VCONV_INST_T* prInst)
{
    CHAR szBuf[16]      = "VCONVCMDQ0";
    CHAR szSrcBuf[16]   = "VCONVSRCQ0";
    szBuf[9] += prInst->u4InstId;
    szSrcBuf[9] += prInst->u4InstId;

    VERIFY(OSR_OK == x_msg_q_create(&(prInst->hCmdQ),
                                    szBuf,
                                    zVConv_GetCmdSize(),
                                    VCONV_CMD_Q_SIZE));
    VERIFY(OSR_OK == x_msg_q_create(&(prInst->hSrcQ),
                                    szSrcBuf,
                                    zVConv_GetSrcSize(),
                                    VCONV_SRC_Q_SIZE));
}

static void vVConv_DeleteCmdQ(VCONV_INST_T* prInst)
{
    VERIFY(OSR_OK == x_msg_q_delete(prInst->hCmdQ));
    VERIFY(OSR_OK == x_msg_q_delete(prInst->hSrcQ));
}

static UINT32 u4VConv_GetCmdCountInQ(VCONV_INST_T* prInst)
{
    INT32   i4Ret;
    UINT16  u2CmdCount = 0;

    i4Ret = x_msg_q_num_msgs(prInst->hCmdQ, &u2CmdCount);

    if (i4Ret == OSR_NO_MSG)
    {
        u2CmdCount = 0;
    }

    prInst->u1QCmdCount = u2CmdCount;
    return (u2CmdCount);
}

static UINT32 u4VConv_GetSrcCountInQ(VCONV_INST_T* prInst)
{
    INT32   i4Ret;
    UINT16  u2SrcCount = 0;

    i4Ret = x_msg_q_num_msgs(prInst->hSrcQ, &u2SrcCount);

    if (i4Ret == OSR_NO_MSG)
    {
        u2SrcCount = 0;
    }

    prInst->u1QFbCount = u2SrcCount;
    return (u2SrcCount);
}

static void vVConv_SendCmd(VCONV_INST_T* prInst, VCONV_CMD_T* prCmd)
{
    INT32 i4Ret;

    i4Ret = x_msg_q_send(prInst->hCmdQ,
                         (void *)prCmd,
                         zVConv_GetCmdSize(),
                         255);

    if (i4Ret == OSR_TOO_MANY)
    {
        LOG(0, "[VConv] [ERR] send cmd %d , but cmd Q full\n", prCmd->eCmd);
        ASSERT(0);

        // add error handle here
    }
    else if (i4Ret != OSR_OK)
    {
        LOG(0, "[VConv] [ERR] send cmd %d to CmdQ 0x%x, but return 0x%x\n",
            prCmd->eCmd, prInst->hCmdQ, i4Ret);
        ASSERT(0);
    }

    /*if ((1 == u4VConv_GetCmdCountInQ(prInst)) &&
        (VCONV_MAIN_IDLE == prInst->eMainSt))*/
    {
        vVConv_SetEvent(prInst, VCONV_EVENT_CMD);
    }
    //UNUSED(u4VConv_GetCmdCountInQ(prInst));
    //UNUSED(u4VConv_GetSrcCountInQ(prInst));
    {
        /*for cli debug*/
        /* this var is defined in aomx, but android br didn't include aomx*/
        /*extern UINT32 u4AOmxDbgMsk;
        if (u4AOmxDbgMsk & 0x80000)
        {
            LOG(0, ">>>%d/%d\n", u4VConv_GetCmdCountInQ(prInst), u4VConv_GetSrcCountInQ(prInst));
        }*/
    }
}

static void vVConv_SendSrc(VCONV_INST_T* prInst, VCONV_SRC_T* prSrc)
{
    INT32 i4Ret;

    i4Ret = x_msg_q_send(prInst->hSrcQ,
                         (void *)prSrc,
                         zVConv_GetSrcSize(),
                         255);

    if (i4Ret == OSR_TOO_MANY)
    {
        LOG(0, "[VConv] [ERR] send src %d , but src Q full\n", prSrc->ucFbId);
        ASSERT(0);
    }
    else if (i4Ret != OSR_OK)
    {
        LOG(0, "[VConv] [ERR] send src %d, but return 0x%x\n",
            prSrc->ucFbId, i4Ret);
        ASSERT(0);
    }
    UNUSED(u4VConv_GetCmdCountInQ(prInst));
    UNUSED(u4VConv_GetSrcCountInQ(prInst));
}

static void vVConv_ReceiveCmd(VCONV_INST_T* prInst, VCONV_CMD_T* prCmd)
{
    INT32               i4Ret;
    UINT16              u2MsgQIdx;
    SIZE_T              zCmdSize = zVConv_GetCmdSize();

    i4Ret = x_msg_q_receive(&u2MsgQIdx,
                            prCmd,
                            &zCmdSize,
                            &(prInst->hCmdQ),
                            1,
                            X_MSGQ_OPTION_NOWAIT);

    if (i4Ret == OSR_NO_MSG)
    {
        return;
    }
    else if (i4Ret == OSR_OK)
    {
        if (zVConv_GetCmdSize() != zCmdSize)
        {
            LOG(0, "[VConv] [ERR] receive cmd size = %d , but CMD SIZE = %d\n",
                        zCmdSize, zVConv_GetCmdSize);
            ASSERT(0);
        }

        LOG(7, "[VConv] [CMD] receice cmd type %d\n", prCmd->eCmd);
    }
    else
    {
        LOG(0, "[VConv] [ERR] receice cmd return 0x%x from CmdQ 0x%x\n",
            i4Ret, prInst->hCmdQ);
        ASSERT(0);
    }
    UNUSED(u4VConv_GetCmdCountInQ(prInst));
    UNUSED(u4VConv_GetSrcCountInQ(prInst));
}

static void vVConv_ReceiveSrc(VCONV_INST_T* prInst, VCONV_SRC_T* prSrc)
{
    INT32               i4Ret;
    UINT16              u2MsgQIdx;
    SIZE_T              zSrcSize = zVConv_GetSrcSize();

    prSrc->ucFbgId = FBM_FBG_ID_UNKNOWN;
    prSrc->ucFbId = FBM_FB_ID_UNKNOWN;
    prSrc->fgEos = FALSE;

    i4Ret = x_msg_q_receive(&u2MsgQIdx,
                            prSrc,
                            &zSrcSize,
                            &(prInst->hSrcQ),
                            1,
                            X_MSGQ_OPTION_NOWAIT);

    if (i4Ret == OSR_NO_MSG)
    {
        return;
    }
    else if (i4Ret == OSR_OK)
    {
        if (zVConv_GetSrcSize() != zSrcSize)
        {
            LOG(0, "[VConv] [ERR] receive src size = %d , but CMD SIZE = %d\n",
                        zSrcSize, zVConv_GetSrcSize);
            ASSERT(0);
        }

        LOG(7, "[VConv] [CMD] receice src %d\n", prSrc->ucFbId);
    }
    else
    {
        LOG(0, "[VConv] [ERR] receice cmd return 0x%x from SrcQ 0x%x\n",
            i4Ret, prInst->hSrcQ);
        ASSERT(0);
    }

    LOG(6, "[VConv] ucFbgId(%d) ucFbId(%d) fgEos(%d)\n",
        prSrc->ucFbgId, prSrc->ucFbId, prSrc->fgEos);
    UNUSED(u4VConv_GetCmdCountInQ(prInst));
    UNUSED(u4VConv_GetSrcCountInQ(prInst));
}

static void vVConv_ClearCmdQ(VCONV_INST_T* prInst)
{
    INT32           i4Ret;
    UINT16          u2MsgQIdx;
    SIZE_T          zCmdSize = zVConv_GetCmdSize();
    UINT32          u4GetCmdCount = 0;
    VCONV_CMD_T     rCmd;
    UINT32          u4CmdCount = u4VConv_GetCmdCountInQ(prInst);

    LOG(5, "[VConv] %d count cmd in cmd Q when vVConv_ClearCmdQ\n", u4CmdCount);

    while (u4GetCmdCount != u4CmdCount)
    {
        i4Ret = x_msg_q_receive(&u2MsgQIdx,
                                &rCmd,
                                &zCmdSize,
                                &(prInst->hCmdQ),
                                1,
                                X_MSGQ_OPTION_NOWAIT);
        LOG(7, "[Vconv] x_msg_q_receive ret 0x%x when vVConv_ClearCmdQ\n", i4Ret);

        u4GetCmdCount ++;

        if (u4GetCmdCount == u4CmdCount)
        {
            if (0 != u4VConv_GetCmdCountInQ(prInst))
            {
                LOG(0, "[VConv] [ERR] cmd Q is not empty when receive cmd count %d\n",
                    u4GetCmdCount);
                ASSERT(0);
            }
        }
        if(i4Ret != OSR_OK)
        {
            break;
        }
    }
    UNUSED(u4VConv_GetCmdCountInQ(prInst));
    UNUSED(u4VConv_GetSrcCountInQ(prInst));
}

static void vVConv_ClearSrcQ(VCONV_INST_T* prInst)
{
    INT32           i4Ret;
    UINT16          u2MsgQIdx;
    SIZE_T          zSrcSize = zVConv_GetSrcSize();
    //UINT32          u4GetSrcCount = 0;
    VCONV_SRC_T     rSrc;
    UINT32          u4SrcCount = u4VConv_GetSrcCountInQ(prInst);

    LOG(5, "[VConv] %d count cmd in src Q when vVConv_ClearCmdQ\n", u4SrcCount);
    UNUSED(u4SrcCount);

    i4Ret = OSR_OK;
    while (i4Ret == OSR_OK)
    {
        i4Ret = x_msg_q_receive(&u2MsgQIdx,
                                &rSrc,
                                &zSrcSize,
                                &(prInst->hSrcQ),
                                1,
                                X_MSGQ_OPTION_NOWAIT);
        LOG(7, "[Vconv] x_msg_q_receive ret 0x%x when vVConv_ClearCmdQ\n", i4Ret);
    }
    prInst->fgSendEos = FALSE;
    UNUSED(u4VConv_GetCmdCountInQ(prInst));
    UNUSED(u4VConv_GetSrcCountInQ(prInst));
}

static void vVConv_ClearSrcQbyFbgId(VCONV_INST_T* prInst, UCHAR ucFbgId)
{
    INT32           i4Ret;
    UINT16          u2MsgQIdx;
    SIZE_T          zSrcSize = zVConv_GetSrcSize();
    //UINT32          u4GetSrcCount = 0;
    VCONV_SRC_T     rSrc;
    UINT32          u4SrcCount = u4VConv_GetSrcCountInQ(prInst);

    LOG(5, "[VConv] %d count cmd in cmd Q when vVConv_ClearCmdQ\n", u4SrcCount);
    UNUSED(u4SrcCount);

    i4Ret = OSR_OK;
    while (i4Ret == OSR_OK)
    {
        // remove the FbgId from SrcQ
        i4Ret = x_msg_q_receive(&u2MsgQIdx,
                                &rSrc,
                                &zSrcSize,
                                &(prInst->hSrcQ),
                                1,
                                X_MSGQ_OPTION_NOWAIT);

        // send the last one to SrcQ.
        if(ucFbgId != rSrc.ucFbgId)
        {
            // send to the head
            i4Ret = x_msg_q_send(prInst->hSrcQ,
                                 (void *)&rSrc,
                                 zVConv_GetSrcSize(),
                                 254);

            if (i4Ret == OSR_TOO_MANY)
            {
                LOG(0, "[VConv] [ERR] send src %d , but src Q full\n", rSrc.ucFbId);
                //ASSERT(0);
            }
            else if (i4Ret != OSR_OK)
            {
                LOG(0, "[VConv] [ERR] send src %d, but return 0x%x\n",
                    rSrc.ucFbId, i4Ret);
                //ASSERT(0);
            }
            break;
        }
        LOG(7, "[Vconv] x_msg_q_receive ret 0x%x when vVConv_ClearCmdQ\n", i4Ret);
    }
    UNUSED(u4VConv_GetCmdCountInQ(prInst));
    UNUSED(u4VConv_GetSrcCountInQ(prInst));
}

static BOOL vVConv_HandleFbgIdChange(VCONV_INST_T* prInst)
{
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prInst->rSrc.ucFbgId);
    if (!prFbmSeqHdr)
    {        
        vVConv_ClearSrcQbyFbgId(prInst, prInst->rSrc.ucFbgId);

        prInst->rSrc.ucFbgId = FBM_FBG_ID_UNKNOWN;
        prInst->rSrc.ucFbId = FBM_FB_ID_UNKNOWN;
        prInst->rSrc.fgEos = FALSE;
        return TRUE;
    }
    return FALSE;
}

static void vVConv_ClearInst(VCONV_INST_T* prInst)
{
    x_memset(prInst, 0, sizeof(VCONV_INST_T));
}

/*static void vVConv_ClearInfo(VCONV_INST_T* prInst)
{
    x_memset(&(prInst->rInfo), 0, sizeof(VCONV_INFO_T));
}*/

static VCONV_INST_T* prGetInst(UINT32 u4InstId)
{
    if (VCONV_INST_NUM <= u4InstId)
    {
        LOG(0, "[VConv] [ERR] u4InstId = %d when Get Inst\n", u4InstId);
        ASSERT(0);
        u4InstId = 0;
    }

    return (&(_arVConvInfo[u4InstId]));
}

#ifndef __HAVE_ARCH_MEMCPY
    /*
     * dest and src must be 4-bytes aligned
     * count must be multiple of 4
     */
	void *vVconv_Align_memcpy(void *dest, const void *src, size_t count)
	{
		int *tmp = dest;
		const int *s = src;
	
		count >>= 2;
	
		while (count--)
		{
			*tmp++ = *s++;
		}
		return dest;
	}
#endif

#ifndef ANDROID
static void vVconv_YUV420SemiPlaner_remove_pitch(UINT8 *dstAddr, UINT8 *addrY, UINT8 *addrC, 
                                                               UINT32 width, UINT32 height, UINT32 pitch )
{
	UINT32 u4Indx;
	
	ASSERT(addrY != NULL);
	ASSERT(addrC != NULL);
	ASSERT(dstAddr != NULL);
	
#ifndef __HAVE_ARCH_MEMCPY	
	if( ((UINT32)addrY&0x03 == 0) && ((UINT32)addrC&0x03) == 0 
		&&((UINT32)dstAddr&0x03 == 0) && (width&0x03 == 0) && (pitch&0x03 == 0))
	{
		for( u4Indx = 0; u4Indx < height; u4Indx++ )
		{
			vVconv_Align_memcpy(dstAddr+u4Indx*width, addrY+u4Indx*pitch, width);
		}
		
		for( u4Indx = 0; u4Indx < height/2; u4Indx++ )
		{
			vVconv_Align_memcpy(dstAddr+height*width+u4Indx*width, addrC+u4Indx*pitch, width);
		}
	}
	else
	{
		for( u4Indx = 0; u4Indx < height; u4Indx++ )
		{
			x_memcpy(dstAddr+u4Indx*width, addrY+u4Indx*pitch, width);
		}
		
		for( u4Indx = 0; u4Indx < height/2; u4Indx++ )
		{
			x_memcpy(dstAddr+height*width+u4Indx*width, addrC+u4Indx*pitch, width);
		}
	}
#else	
	for( u4Indx = 0; u4Indx < height; u4Indx++ )
	{
		memcpy(dstAddr+u4Indx*width, addrY+u4Indx*pitch, width);
	}
	
	for( u4Indx = 0; u4Indx < height/2; u4Indx++ )
	{
		memcpy(dstAddr+height*width+u4Indx*width, addrC+u4Indx*pitch, width);
	}
#endif
}

static void vVconv_YUV420_remove_pitch(UINT8 *dstAddr, UINT8 *srcAddr, UINT32 width, 
                                       UINT32 pitch,   UINT32 height,  UINT32 height_pitch )
{
	UINT32 u4Indx;
	
	ASSERT(srcAddr != NULL);
	ASSERT(dstAddr != NULL);
	
#ifndef __HAVE_ARCH_MEMCPY	
	if( ((UINT32)srcAddr&0x03 == 0)&&((UINT32)dstAddr&0x03 == 0) && (width&0x07 == 0) && (pitch&0x07 == 0))
	{
		for( u4Indx = 1; u4Indx < height; u4Indx++ )
		{
			vVconv_Align_memcpy(dstAddr+u4Indx*width, srcAddr+u4Indx*pitch, width);
		}
		
		for( u4Indx = 0; u4Indx < height/2; u4Indx++ )
		{
			vVconv_Align_memcpy(dstAddr+height*width+u4Indx*(width>>1), srcAddr+height_pitch*pitch+u4Indx*(pitch>>1), (width>>1));
		}
		
		for( u4Indx = 0; u4Indx < height/2; u4Indx++ )
		{
			vVconv_Align_memcpy(dstAddr+height*width*5/4+u4Indx*(width>>1), srcAddr+height_pitch*pitch*5/4+u4Indx*(pitch>>1), (width>>1));
		}
	}
	else
	{
		for( u4Indx = 1; u4Indx < height; u4Indx++ )
		{
			x_memcpy(dstAddr+u4Indx*width, srcAddr+u4Indx*pitch, width);
		}
		
		for( u4Indx = 0; u4Indx < height/2; u4Indx++ )
		{
			x_memcpy(dstAddr+height*width+u4Indx*(width>>1), srcAddr+height_pitch*pitch+u4Indx*(pitch>>1), (width>>1));
		}
		
		for( u4Indx = 0; u4Indx < height/2; u4Indx++ )
		{
			x_memcpy(dstAddr+height*width*5/4+u4Indx*(width>>1), srcAddr+height_pitch*pitch*5/4+u4Indx*(pitch>>1), (width>>1));
		}
	}
#else	
	for( u4Indx = 0; u4Indx < height; u4Indx++ )
	{
		memcpy(dstAddr+u4Indx*width, srcAddr+u4Indx*pitch, width);
	}
	
	for( u4Indx = 0; u4Indx < height/2; u4Indx++ )
	{
		memcpy(dstAddr+height*width+u4Indx*(width>>1), srcAddr+height_pitch*pitch+u4Indx*(pitch>>1), (width>>1));
	}
	
	for( u4Indx = 0; u4Indx < height/2; u4Indx++ )
	{
		memcpy(dstAddr+height*width*5/4+u4Indx*(width>>1), srcAddr+height_pitch*pitch*5/4+u4Indx*(pitch>>1), (width>>1));
	}
#endif
}
#endif

static void vVconv_ARGB8888_remove_pitch(UINT8 *dstAddr, UINT8 *srcAddr, 
	                                                 UINT32 width, UINT32 height,  UINT32 pitch )
{
	UINT32 u4Indx;

	ASSERT(srcAddr != NULL);
	ASSERT(dstAddr != NULL);
	
#ifndef __HAVE_ARCH_MEMCPY	
	if( ((UINT32)srcAddr&0x03 == 0) && ((UINT32)dstAddr&0x03 == 0) 
		&& (width&0x03 == 0) && (pitch&0x03 == 0) )
	{
		for( u4Indx = 0; u4Indx < height; u4Indx++ )
		{
			vVconv_Align_memcpy(dstAddr+u4Indx*width, srcAddr+u4Indx*pitch, width);
		}
	}
	else
	{
		for( u4Indx = 0; u4Indx < height; u4Indx++ )
		{
			x_memcpy(dstAddr+u4Indx*width, srcAddr+u4Indx*pitch, width);
		}
	}
#else	
	for( u4Indx = 0; u4Indx < height; u4Indx++ )
	{
		memcpy(dstAddr+u4Indx*width, srcAddr+u4Indx*pitch, width);
	}
#endif
}
PATH_TYPE vVConv_GetPath(VCONV_INST_T* prInst)
{
    PATH_TYPE ePathType = P_MAIN;

    if (0 == prInst->u4InstId)
    {
        ePathType = P_MAIN;
    }
    else if (1 == prInst->u4InstId)
    {
        ePathType = P_SUB;
    }
    else
    {
        LOG(0, "[VConv] [ERR] inst id %d error when get path\n",
            prInst->u4InstId);
        ASSERT(0);
    }

    return (ePathType);
}

static INT32 i4VConv_SearchFbInfo(VCONV_INST_T* prInst, UINT32 u4FbAddr, UINT32 *pu4Idx)
{
    INT32       i4Ret = VCONV_OK;
    UINT32      u4Idx = 0;

    for (u4Idx = 0; u4Idx < VCONV_MAX_FB_COUNT; u4Idx ++)
    {
        if (u4FbAddr == prInst->arFbInfo[u4Idx].u4FbAddr)
        {
            *pu4Idx = u4Idx;

            break;
        }
    }

    if (VCONV_MAX_FB_COUNT == u4Idx)
    {
        LOG(1, "[VConv] [ERR] can not find a fill FbInfo\n");

        i4Ret = VCONV_FB_ERR;
    }

    return (i4Ret);
}

BOOL vVConv_B2R(VCONV_INST_T* prInst,
    UCHAR ucFbgId, UCHAR ucFbId,
    UINT32 u4DstAddrY, UINT32 u4DstAddrC,
    BOOL fgSrcIsRs, UINT32 u4SrcSwap,
    BOOL fgDstIsRs, UINT32 u4DstSwap)
{
    UINT32 u4WorkBuf;
    UINT32 u4AddrY;
    UINT32 u4AddrC;
    VDP_THUMBNAIL_INFO_T rCapDstInfo;
    VDP_CAPTURE_INTO_T rCapSrcInfo;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;
    FBM_PIC_HDR_T* prFbmPicHdr = NULL;
    UINT16 u2HSize, u2VSize, u2LineSize;

    //physical address
    u4WorkBuf = (prInst->u4DstFbgAddr + (prInst->u1FbCount * VCONV_BUF_SIZE_ARBG));

    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
    if(!prFbmSeqHdr)
    {
        // FbgId change handle
        if (vVConv_HandleFbgIdChange(prInst))
        {
            LOG(3, "[VConv] ucFbgId change case(L%d)\n", __LINE__);
            vVConv_ChangeSubState(prInst, VCONV_SUB_GET_NEXT_PB);
            return (FALSE);
    }
        else
        {
            LOG(0, "[VConv] [ERR] null(L:%d)\n", __LINE__);
            ASSERT(0);
        }
        return TRUE;
    }

    FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &u4AddrY, &u4AddrC);

    u2LineSize = prFbmSeqHdr->u2LineSize;
    u2HSize = prFbmSeqHdr->u2HSize;
    u2VSize = prFbmSeqHdr->u2VSize;
    if(prFbmSeqHdr->fgResizeSmallPic)
    {
        prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);
        if(!prFbmPicHdr)
        {
            // FbgId change handle
            if (vVConv_HandleFbgIdChange(prInst))
            {
                LOG(3, "[VConv] ucFbgId change case(L%d)\n", __LINE__);
                vVConv_ChangeSubState(prInst, VCONV_SUB_GET_NEXT_PB);
                return (FALSE);
            }
            else
            {
                LOG(0, "[VConv] [ERR] null(L:%d)\n", __LINE__);
                ASSERT(0);
        }
            return TRUE;
        }
        u2LineSize = prFbmPicHdr->u4PicWidthPitch;
        u2HSize = prFbmPicHdr->u4PicWidth;
        u2VSize = prFbmPicHdr->u4PicHeight;
    }

    x_memset(&rCapSrcInfo, 0, sizeof(VDP_CAPTURE_INTO_T));
    x_memset(&rCapDstInfo, 0, sizeof(VDP_THUMBNAIL_INFO_T));

    rCapSrcInfo.u4HSize = u2HSize;
    rCapSrcInfo.u4VSize = u2VSize;
    rCapSrcInfo.u4MemHSize = u2LineSize;
    rCapSrcInfo.u4MemVSize = u2VSize;
    rCapSrcInfo.u4AddrY = u4AddrY;
    rCapSrcInfo.u4AddrC = u4AddrC;
    rCapSrcInfo.u4RRMode = 0;
    rCapSrcInfo.u4IsRsIn = 0;
    rCapSrcInfo.u4SrcSwap = 6;

    rCapDstInfo.u4ThumbnailWidth  = ((prFbmSeqHdr->u2HSize+0x0f)&(~0x0f));
    rCapDstInfo.u4ThumbnailHeight = prFbmSeqHdr->u2VSize;
    rCapDstInfo.pucCanvasBuffer = (UCHAR*)u4DstAddrY;
    rCapDstInfo.eCanvasColormode = VDP_COLORMODE_ARGB_D8888;
    rCapDstInfo.u4CanvasWidth = (rCapDstInfo.u4ThumbnailWidth*4);
    rCapDstInfo.u4CanvasHeight = rCapDstInfo.u4ThumbnailHeight;
    rCapDstInfo.u4CanvasPitch = (((rCapDstInfo.u4ThumbnailWidth+0x3f)&(~0x3f))*4);
    
	UNUSED(GFX_ColorConvert_YC((void*)&rCapDstInfo, u4AddrY, u4AddrC, VdecVdpClr2GfxClr(rCapDstInfo.eCanvasColormode),0xFF,u2LineSize));
	
	if( rCapDstInfo.u4ThumbnailWidth  != prFbmSeqHdr->u2HSize )
	{
		vVconv_ARGB8888_remove_pitch((UINT8 *)(VIRTUAL(u4DstAddrY)), (UINT8 *)(VIRTUAL(u4DstAddrY)), 
	                                                 (prFbmSeqHdr->u2HSize*4), rCapDstInfo.u4CanvasHeight,  (rCapDstInfo.u4ThumbnailWidth*4) );
	}

    return TRUE;
}

#if 0
#define BLOCK_W 64
#define BLOCK_H 32
BOOL vVConv_B2YUV420(VCONV_INST_T* prInst,
    UCHAR ucFbgId, UCHAR ucFbId,
    UINT32 u4DstAddrY, UINT32 u4DstAddrC,
    BOOL fgSrcIsRs, UINT32 u4SrcSwap,
    BOOL fgDstIsRs, UINT32 u4DstSwap)
{
    UINT32 u4ByteSize = 0;
    UINT32 u4Idx;
    UINT32 u4BlockSize, u4BlockNum, u4BlockPerLine;
    UINT32 u4BlockStartX, u4BlockStartY, u4BlockCoordX, u4BlockCoordY;

    UCHAR *dstptr;
    UCHAR *dstcbptr;
    UCHAR *dstcrptr;
    UINT32 u4AddrY, u4AddrC;
    FBM_SEQ_HDR_T *prSeqHdrSrc;

    prSeqHdrSrc = FBM_GetFrameBufferSeqHdr(prInst->rSrc.ucFbgId);
    if(!prSeqHdrSrc)
    {
        // FbgId change handle
        if (vVConv_HandleFbgIdChange(prInst))
        {
            LOG(3, "[VConv] ucFbgId change case(L%d)\n", __LINE__);
            vVConv_ChangeSubState(prInst, VCONV_SUB_GET_NEXT_PB);
            return (FALSE);
        }
        else
        {
            LOG(0, "[VConv] [ERR] null(L:%d)\n", __LINE__);
        ASSERT(0);
    }
        return TRUE;
    }
    FBM_GetFrameBufferAddr(prInst->rSrc.ucFbgId, prInst->rSrc.ucFbId, &u4AddrY, &u4AddrC);
    u4AddrY = VIRTUAL(u4AddrY);
    u4AddrC = VIRTUAL(u4AddrC);

    u4ByteSize = (prSeqHdrSrc->u2LineSize) * (((prSeqHdrSrc->u2VSize)+0x1f)&(~0x1f));
    dstptr = (UCHAR*)(VIRTUAL(u4DstAddrY));
    dstcbptr = dstptr + (prInst->u4Stride * prInst->u4FrameHeight);
    dstcrptr = dstcbptr + (prInst->u4Stride * prInst->u4FrameHeight / 4);

    u4BlockSize = BLOCK_W*BLOCK_H;
    u4BlockPerLine = (prSeqHdrSrc->u2LineSize / BLOCK_W);

    for (u4Idx = 0; u4Idx < u4ByteSize; u4Idx++)
    {
        u4BlockNum = u4Idx /u4BlockSize;
        u4BlockStartX = (u4BlockNum % u4BlockPerLine) * BLOCK_W;
        u4BlockStartY = (u4BlockNum / u4BlockPerLine) * BLOCK_H;
        u4BlockCoordX = (u4Idx % u4BlockSize) % BLOCK_W;
        u4BlockCoordY = (u4Idx % u4BlockSize) / BLOCK_W;

        if (((u4BlockStartX + u4BlockCoordX) >= prInst->u4FrameWidth)
            || ((u4BlockStartY + u4BlockCoordY) >= prInst->u4FrameHeight))
        {
        }
        else
        {
            dstptr[(u4BlockStartX + u4BlockCoordX) + (u4BlockStartY + u4BlockCoordY) * prInst->u4Stride]
                = *(UCHAR*)(u4AddrY + u4Idx);
        }
    }

    u4BlockSize = BLOCK_W*(BLOCK_H >> 1);
    u4BlockPerLine = (prSeqHdrSrc->u2LineSize / BLOCK_W);

    for (u4Idx = 0; u4Idx < (u4ByteSize >> 1); u4Idx++)
    {
        u4BlockNum = u4Idx /u4BlockSize;
        u4BlockStartX = (u4BlockNum % u4BlockPerLine) * BLOCK_W;
        u4BlockStartY = (u4BlockNum / u4BlockPerLine) * (BLOCK_H >> 1);
        u4BlockCoordX = (u4Idx % u4BlockSize) % BLOCK_W;
        u4BlockCoordY = (u4Idx % u4BlockSize) / BLOCK_W;

        if (((u4BlockStartX + u4BlockCoordX) >= prInst->u4FrameWidth)
                || ((u4BlockStartY + u4BlockCoordY) >= (prInst->u4FrameHeight >> 1)))
        {
        }
        else
        {
            if(((u4BlockStartX + u4BlockCoordX) & 1) == 0)
            {
                dstcbptr[((u4BlockStartX + u4BlockCoordX)/2) + (u4BlockStartY + u4BlockCoordY)*(prInst->u4Stride/2)]
                    = *(UCHAR*)(u4AddrC + u4Idx);
            }
            else
            {
                dstcrptr[((u4BlockStartX + u4BlockCoordX)/2) + (u4BlockStartY + u4BlockCoordY)*(prInst->u4Stride/2)]
                    = *(UCHAR*)(u4AddrC + u4Idx);
            }
        }
    }
    return TRUE;
}
#endif

BOOL vVConv_B2YUV420SemiPlaner(VCONV_INST_T* prInst,
    UCHAR ucFbgId, UCHAR ucFbId,
    UINT32 u4DstAddrY, UINT32 u4DstAddrC,
    BOOL fgSrcIsRs, UINT32 u4SrcSwap,
    BOOL fgDstIsRs, UINT32 u4DstSwap)
{
    UINT32 u4AddrY;
    UINT32 u4AddrC;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;
    FBM_PIC_HDR_T* prFbmPicHdr = NULL;
    RZ_VDO_SCL_PARAM_SET_T rImgrzScaleParam;
    UINT16 u2HSize, u2VSize, u2LineSize;

    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
    if(!prFbmSeqHdr)
    {
        // FbgId change handle
        if (vVConv_HandleFbgIdChange(prInst))
        {
            LOG(3, "[VConv] ucFbgId change case(L%d)\n", __LINE__);
            vVConv_ChangeSubState(prInst, VCONV_SUB_GET_NEXT_PB);
            return (FALSE);
        }
        else
        {
            LOG(0, "[VConv] [ERR] null(L:%d)\n", __LINE__);
            ASSERT(0);
        }
        return TRUE;
    }
    if( prFbmSeqHdr->u2LineSize == 0
    	|| prFbmSeqHdr->u2HSize == 0
    	|| prFbmSeqHdr->u2VSize == 0
      )
    {
    	LOG(0, "[VConv] [ERR] Format is wrong");
    	return TRUE;
    }

    FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &u4AddrY, &u4AddrC);

    u2LineSize = prFbmSeqHdr->u2LineSize;
    u2HSize = ((prFbmSeqHdr->u2HSize+0xf)&(~0x0f));
    u2VSize = prFbmSeqHdr->u2VSize;
    if(prFbmSeqHdr->fgResizeSmallPic)
    {
        prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);
        if(!prFbmPicHdr)
        {
            LOG(0, "[VConv] [ERR] prFbmPicHdr null\n");
            return TRUE;
        }
        u2LineSize = prFbmPicHdr->u4PicWidthPitch;
        u2HSize = prFbmPicHdr->u4PicWidth;
        u2VSize = prFbmPicHdr->u4PicHeight;
    }

	x_memset(&rImgrzScaleParam,0,sizeof(RZ_VDO_SCL_PARAM_SET_T));

	rImgrzScaleParam.u4SrcSwap = 6;
    rImgrzScaleParam.u4OutSwap = 0;

    rImgrzScaleParam.u4InMode = E_RZ_VDO_OUTMD_420;
    rImgrzScaleParam.u4YSrcBase = u4AddrY;
    rImgrzScaleParam.u4YSrcBufLen = u2LineSize;
    rImgrzScaleParam.u4YSrcHOffset = 0;
    rImgrzScaleParam.u4YSrcVOffset = 0;
    rImgrzScaleParam.u4YSrcW = u2HSize;
    rImgrzScaleParam.u4YSrcH = u2VSize;
    rImgrzScaleParam.u4CSrcBase = u4AddrC;
    rImgrzScaleParam.u4CSrcHOffset = 0;
    rImgrzScaleParam.u4CSrcVOffset = 0;
    rImgrzScaleParam.u4CSrcW = rImgrzScaleParam.u4YSrcW / 2;
    rImgrzScaleParam.u4CSrcH = rImgrzScaleParam.u4YSrcH / 2;

    rImgrzScaleParam.u4IsRsOut = 1;
    rImgrzScaleParam.u4OutMode = E_RZ_VDO_OUTMD_420;       // 420|422|444 output
    rImgrzScaleParam.u4IsVdo2Osd = 0;     // output in osd format ?
    rImgrzScaleParam.u4YTgBase = u4DstAddrY;
    rImgrzScaleParam.u4YTgCM = E_RZ_VDO_OUTMD_420;
    rImgrzScaleParam.u4YTgBufLen = u2HSize;//prFbmSeqHdr->u2LineSize;
    rImgrzScaleParam.u4YTgHOffset = 0;
    rImgrzScaleParam.u4YTgVOffset = 0;
    rImgrzScaleParam.u4YTgW = u2HSize;  //prFbmSeqHdr->u2HSize;
    rImgrzScaleParam.u4YTgH = u2VSize;
    //rImgrzScaleParam.u4CTgBase = u4DstAddrC;
    rImgrzScaleParam.u4CTgBase = u4DstAddrY + (u2HSize * u2VSize);
    rImgrzScaleParam.u4CTgW = rImgrzScaleParam.u4YTgW / 2;
    rImgrzScaleParam.u4CTgH = rImgrzScaleParam.u4YTgH / 2;

    IMGRZ_Lock();
    IMGRZ_ReInit();
    IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_VDOMD);
    IMGRZ_Scale((void *)(&rImgrzScaleParam));
    IMGRZ_Flush();
    IMGRZ_Wait();
    IMGRZ_Unlock();

    
#ifndef ANDROID
    if(u2HSize != prFbmSeqHdr->u2HSize)
    {
    	UINT8 *addrY, *addrC, *dstAddr;
    	//struct timeval tval0, tval1;
    	
    	addrY = (UINT8*)VIRTUAL(u4DstAddrY);
    	addrC = (UINT8*)VIRTUAL(u4DstAddrY + (u2HSize * u2VSize));
    	dstAddr = (UINT8*)VIRTUAL(u4DstAddrY);
    	
    	vVconv_YUV420SemiPlaner_remove_pitch(dstAddr, addrY, addrC, prFbmSeqHdr->u2HSize, 
    	                                     prFbmSeqHdr->u2VSize, u2HSize);
    }
#endif

    return TRUE;
}

BOOL vVConv_B2YUV420Planar(VCONV_INST_T* prInst,
    UCHAR ucFbgId, UCHAR ucFbId,
    UINT32 u4DstAddrY, UINT32 u4DstAddrC,
    BOOL fgSrcIsRs, UINT32 u4SrcSwap,
    BOOL fgDstIsRs, UINT32 u4DstSwap)
{
//		UINT32 u4CurIdx;
    UINT32 u4AddrY;
    UINT32 u4AddrC;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;
    FBM_PIC_HDR_T* prFbmPicHdr = NULL;
    UINT16 u2HSize, u2VSize, u2LineSize;
#if !defined(VOMX_NO_GFX_IOMMU) && defined(ANDROID)
    B2R_SRC_INFO_T rInternalBuf;
#endif
    
#if defined(VOMX_NO_GFX_IOMMU) || !defined(ANDROID)
    UINT16 u2AdjustHSize, u2AdjustVSize;
    RZ_VDO_SCL_PARAM_SET_T rImgrzScaleParam;
    GFX_FLTR_CBCR_T pt_cbcr;
#endif

    if(!_bPoolInfoInited)
    {
        _prFbmMPEG = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG);
        _prFbmMPEG2 = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG2);
        _bPoolInfoInited = TRUE;
    }
	
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
    if(!prFbmSeqHdr)
    {
        LOG(0, "[VConv] [ERR] prSeqHdrSrc null\n");
        return FALSE;
    }
    if( prFbmSeqHdr->u2LineSize == 0
    	|| prFbmSeqHdr->u2HSize == 0
    	|| prFbmSeqHdr->u2VSize == 0
      )
    {
    	LOG(0, "[VConv] [ERR] Format is wrong");
    	return FALSE;
    }

    FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &u4AddrY, &u4AddrC);

    u2LineSize = prFbmSeqHdr->u2LineSize;
    u2HSize = ((prFbmSeqHdr->u2HSize+0x1f)&(~0x1f));
    u2VSize = ((prFbmSeqHdr->u2VSize+0x1f)&(~0x1f));
    if(prFbmSeqHdr->fgResizeSmallPic)
    {
        prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);
        if(!prFbmPicHdr)
        {
            // FbgId change handle
            if (vVConv_HandleFbgIdChange(prInst))
            {
                LOG(3, "[VConv] ucFbgId change case(L%d)\n", __LINE__);
                vVConv_ChangeSubState(prInst, VCONV_SUB_GET_NEXT_PB);
                return (FALSE);
            }
            else
            {
                LOG(0, "[VConv] [ERR] null(L:%d)\n", __LINE__);
                ASSERT(0);
            }
            return TRUE;
        }
        u2LineSize = prFbmPicHdr->u4PicWidthPitch;
        u2HSize = prFbmPicHdr->u4PicHeight;
        u2VSize = prFbmPicHdr->u4PicWidth;
    }

#if !defined(VOMX_NO_GFX_IOMMU) && defined(ANDROID)
    x_memset(&rInternalBuf, 0, sizeof(B2R_SRC_INFO_T));
    rInternalBuf.fgNeedConv = TRUE;
    rInternalBuf.ucFbId = ucFbId;
    rInternalBuf.ucFbgId= ucFbgId;
    rInternalBuf.u4AddrY= u4AddrY;
    rInternalBuf.u4AddrC= u4AddrC;
    rInternalBuf.u2LineSize = u2LineSize;
	
    if(u4AddrY >= _prFbmMPEG->u4Addr && u4AddrY <= _prFbmMPEG->u4Addr + _prFbmMPEG->u4Size)
        rInternalBuf.ucMpegNum = 1;
    else if(u4AddrY >= _prFbmMPEG2->u4Addr && u4AddrY <= (_prFbmMPEG2->u4Addr + _prFbmMPEG2->u4Size))
        rInternalBuf.ucMpegNum = 2;
    else
        rInternalBuf.ucMpegNum = 3;
	
    if(prInst->fgRemovePitch)
    {
        // for TVQQ
        if(prFbmSeqHdr->fgCropping)
        {
           rInternalBuf.u2Width = prFbmSeqHdr->u4CropWidth;
           rInternalBuf.u2Height = prFbmSeqHdr->u4CropHeight;
        }
        else
        {
           rInternalBuf.u2Width = prFbmSeqHdr->u2HSize;
           rInternalBuf.u2Height = prFbmSeqHdr->u2VSize;
        }
    }
    else
    {
        rInternalBuf.u2Width = u2HSize;
        rInternalBuf.u2Height = u2VSize;
    }
//	#endif
    x_memcpy((VOID*)VIRTUAL(u4DstAddrY), &rInternalBuf, sizeof(B2R_SRC_INFO_T));
    prInst->rCurrFill.u4DstFillSize = sizeof(B2R_SRC_INFO_T);
#else
	x_memset(&rImgrzScaleParam,0,sizeof(RZ_VDO_SCL_PARAM_SET_T));

//		u4CurIdx = prInst->u4CurIdx;

	rImgrzScaleParam.u4SrcSwap = 6;
    rImgrzScaleParam.u4OutSwap = 0;

    rImgrzScaleParam.u4InMode = E_RZ_VDO_OUTMD_420;
    rImgrzScaleParam.u4YSrcBase = u4AddrY;
    rImgrzScaleParam.u4YSrcBufLen = u2LineSize;
    rImgrzScaleParam.u4YSrcHOffset = 0;
    rImgrzScaleParam.u4YSrcVOffset = 0;
    rImgrzScaleParam.u4YSrcW = u2HSize;
    rImgrzScaleParam.u4YSrcH = u2VSize;
    rImgrzScaleParam.u4CSrcBase = u4AddrC;
    rImgrzScaleParam.u4CSrcHOffset = 0;
    rImgrzScaleParam.u4CSrcVOffset = 0;
    rImgrzScaleParam.u4CSrcW = rImgrzScaleParam.u4YSrcW / 2;
    rImgrzScaleParam.u4CSrcH = rImgrzScaleParam.u4YSrcH / 2;

    rImgrzScaleParam.u4IsRsOut = 1;
    rImgrzScaleParam.u4OutMode = E_RZ_VDO_OUTMD_420;       // 420|422|444 output
    rImgrzScaleParam.u4IsVdo2Osd = 0;     // output in osd format ?
    rImgrzScaleParam.u4YTgBase = u4DstAddrY;
    rImgrzScaleParam.u4YTgCM = E_RZ_VDO_OUTMD_420;
    rImgrzScaleParam.u4YTgBufLen = u2HSize;//prFbmSeqHdr->u2LineSize;
    rImgrzScaleParam.u4YTgHOffset = 0;
    rImgrzScaleParam.u4YTgVOffset = 0;
    rImgrzScaleParam.u4YTgW = u2HSize;  //prFbmSeqHdr->u2HSize;
    rImgrzScaleParam.u4YTgH = u2VSize;
    rImgrzScaleParam.u4CTgBase = u4DstAddrC;
//    rImgrzScaleParam.u4CTgBase = u4DstAddrY + ((VCONV_MAX_YUV420_FB_COUNT - u4CurIdx)*VCONV_BUF_SIZE_YUV420);
    rImgrzScaleParam.u4CTgW = rImgrzScaleParam.u4YTgW / 2;
    rImgrzScaleParam.u4CTgH = rImgrzScaleParam.u4YTgH / 2;

    if(down_interruptible(&prInst->rConvSema))
    {
        return FALSE;
    }

    IMGRZ_Lock();
    IMGRZ_ReInit();
    IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_VDOMD);
    IMGRZ_Scale((void *)(&rImgrzScaleParam));
    IMGRZ_Flush();
    IMGRZ_Wait();
    IMGRZ_Unlock();

    pt_cbcr.ui4_cbcr_addr  = u4DstAddrC;
//    pt_cbcr.ui4_cbcr_addr  = u4DstAddrY + ((VCONV_MAX_YUV420_FB_COUNT - u4CurIdx)*VCONV_BUF_SIZE_YUV420);  //((u2HSize * u2VSize)*3/2);
    pt_cbcr.ui4_src_width  = u2HSize;
    pt_cbcr.ui4_src_height = u2VSize/2;
    pt_cbcr.ui4_src_pitch  = u2HSize;
    pt_cbcr.ui4_cb_addr = u4DstAddrY + (u2HSize * u2VSize);
    pt_cbcr.ui4_cr_addr = u4DstAddrY + (u2HSize * u2VSize)
                                     + ((u2HSize * u2VSize)*1/4);  
                                     
    GFX_Fltr_CbCr((GFX_FLTR_CBCR_T *)&pt_cbcr);

    u2AdjustHSize = prFbmSeqHdr->u2HSize;
	u2AdjustVSize = prFbmSeqHdr->u2VSize;
    prInst->rCurrFill.u4DstFillSize = u2HSize * u2VSize * 3 / 2;
    if(prFbmSeqHdr->fgCropping)
    {
        vVconv_YUV420_remove_pitch( (UINT8 *)VIRTUAL(u4DstAddrY), (UINT8 *)VIRTUAL(u4DstAddrY), prFbmSeqHdr->u4CropWidth,
            u2HSize, prFbmSeqHdr->u4CropHeight, u2VSize );       
        prInst->rCurrFill.u4DstFillSize = prFbmSeqHdr->u4CropWidth*prFbmSeqHdr->u4CropHeight*3/2;  
    }
    else if(u2HSize != u2AdjustHSize || u2VSize != u2AdjustVSize)   
    {
        LOG(6,"Pitch removing ...\n");
        vVconv_YUV420_remove_pitch( (UINT8 *)VIRTUAL(u4DstAddrY), (UINT8 *)VIRTUAL(u4DstAddrY), u2AdjustHSize,  
            u2HSize, u2AdjustVSize, u2VSize );              
        prInst->rCurrFill.u4DstFillSize = u2AdjustHSize * u2AdjustVSize * 3 / 2;  
    }

//	    // for TVQQ
//	    prInst->rCurrFill.u4DstFillSize = u2HSize * u2VSize * 3 / 2;
//	    if(prInst->fgRemovePitch)
//	    {        
//	        u2AdjustHSize = prFbmSeqHdr->u2HSize;
//	        u2AdjustVSize = prFbmSeqHdr->u2VSize;
//	        
//	        if(prFbmSeqHdr->fgCropping)
//	        {
//	            vVconv_YUV420_remove_pitch( VIRTUAL(u4DstAddrY), VIRTUAL(u4DstAddrY), prFbmSeqHdr->u4CropWidth,
//	                u2HSize, prFbmSeqHdr->u4CropHeight, u2VSize );       
//	            prInst->rCurrFill.u4DstFillSize = prFbmSeqHdr->u4CropWidth*prFbmSeqHdr->u4CropHeight*3/2;  
//	        }
//	        else if(u2HSize != u2AdjustHSize || u2VSize != u2AdjustVSize)   
//	        {
//	            LOG(6,"Pitch removing ...\n");
//	            vVconv_YUV420_remove_pitch( VIRTUAL(u4DstAddrY), VIRTUAL(u4DstAddrY), u2AdjustHSize,  
//	                u2HSize, u2AdjustVSize, u2VSize );              
//	            prInst->rCurrFill.u4DstFillSize = u2AdjustHSize * u2AdjustVSize * 3 / 2;  
//	        }
//	    }

#endif
    return TRUE;
}

int vVconv_Copy_Done(PARAMETER_OMX_PARAM_CPY_DONE *prUArg)
{
    PARAMETER_OMX_PARAM_CPY_DONE rKArg;
    VDO_OMX_HANDLE_T *OmxHandle;
    if(copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_PARAM_CPY_DONE)))
        return -1;
    OmxHandle = rKArg.pHandle;
    up(&(_arVConvInfo[OmxHandle->h_handle].rConvSema));
    
    return 0;
}

int vVConv_Fbm_Set_Empty(PARAMETER_OMX_PARAM_FBM_SET_EMPTY* prUArg)
{
    PARAMETER_OMX_PARAM_FBM_SET_EMPTY rKArg;
    if (copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_PARAM_FBM_SET_EMPTY)))
        return -1;
    FBM_SetFrameBufferStatus(rKArg.nFbgId, rKArg.nFbId, FBM_FB_STATUS_LOCK);
    FBM_SetFrameBufferStatus(rKArg.nFbgId, rKArg.nFbId, FBM_FB_STATUS_EMPTY);
    return 0;
}

BOOL vVConv_B2B2R(VCONV_INST_T* prInst,
    UCHAR ucFbgId, UCHAR ucFbId,
    UINT32 u4DstAddrY, UINT32 u4DstAddrC,
    BOOL fgSrcIsRs, UINT32 u4SrcSwap,
    BOOL fgDstIsRs, UINT32 u4DstSwap)
{
    FBM_INTERNAL_BUF_T rInternalBuf;

    x_memset(&rInternalBuf, 0, sizeof(FBM_INTERNAL_BUF_T));
    x_memcpy(rInternalBuf.ucBuf, FBG_INTERNAL_VOMX_BUFFER_ID, FBG_INTERNAL_VOMX_BUFFER_SIZE);
    rInternalBuf.ucFbgId = ucFbgId;
    rInternalBuf.ucFbId = ucFbId;
    x_memcpy((VOID*)VIRTUAL(u4DstAddrY), &rInternalBuf, sizeof(FBM_INTERNAL_BUF_T));
    return TRUE;
}

static void vVConv_ChangeMainState(VCONV_INST_T* prInst, VCONV_MAIN_STATE_T eChangeMainState)
{
    VCONV_MAIN_STATE_T ePrivMainState = prInst->eMainSt;

    if (ePrivMainState != eChangeMainState)
    {
        prInst->eMainSt = eChangeMainState;

        LOG(7, "[VConv] Change Main state to %d from %d\n",
                    eChangeMainState, ePrivMainState);

        vVConv_SetEvent(prInst, VCONV_EVENT_CMD);
    }
}

static void vVConv_ChangeSubState(VCONV_INST_T* prInst, VCONV_SUB_STATE_T eChangeSubState)
{
    VCONV_SUB_STATE_T ePrivSubState = prInst->eSubSt;

    if (ePrivSubState != eChangeSubState)
    {
        prInst->eSubSt = eChangeSubState;

        LOG(7, "[VConv] Change Sub state to %d from %d\n",
                    eChangeSubState, ePrivSubState);
    }
}

void vVConv_GetFrmRate(UINT32 u4Tag,UCHAR ucFrameRate)
{
	if(_arVConvInfo[u4Tag].u4FrameRate != VCONV_FRMRATE_UNKNOWN)
	{
		return;
	}
	else
	{
		if(ucFrameRate != MPEG_FRAME_RATE_UNKNOWN)
		{
			switch(ucFrameRate)
			{
				case MPEG_FRAME_RATE_24_:
					_arVConvInfo[u4Tag].u4FrameRate = 2397;
					break;
				case MPEG_FRAME_RATE_24:
					_arVConvInfo[u4Tag].u4FrameRate = 2400;
					break;
				case MPEG_FRAME_RATE_25:
					_arVConvInfo[u4Tag].u4FrameRate = 2500;
					break;
				case MPEG_FRAME_RATE_30_:
					_arVConvInfo[u4Tag].u4FrameRate = 2997;
					break;
				case MPEG_FRAME_RATE_30:
					_arVConvInfo[u4Tag].u4FrameRate = 3000;
					break;
				case MPEG_FRAME_RATE_50:
					_arVConvInfo[u4Tag].u4FrameRate = 5000;
					break;
				case MPEG_FRAME_RATE_60_:
					_arVConvInfo[u4Tag].u4FrameRate = 5997;
					break;
				case MPEG_FRAME_RATE_60:
					_arVConvInfo[u4Tag].u4FrameRate = 6000;
				case MPEG_FRAME_RATE_12:
					_arVConvInfo[u4Tag].u4FrameRate = 1200;
					break;
				case MPEG_FRAME_RATE_15:
					_arVConvInfo[u4Tag].u4FrameRate = 1500;
				case MPEG_FRAME_RATE_20:
					_arVConvInfo[u4Tag].u4FrameRate = 2000;
					break;
				default:
					_arVConvInfo[u4Tag].u4FrameRate = VCONV_FRMRATE_UNKNOWN;
					break;
			}
			LOG(6,"vVconv Get FrmRate %d\n",_arVConvInfo[u4Tag].u4FrameRate);
		}
	}
	return;
}



/*record the last frame decoded. thumbnails will be generated from the last frame.*/
static void vVDOOmxSaveLastFb(UINT8 ucOmxHandle, UINT8 ucFbgid, UINT8 ucFbid);
BOOL fgVConv_VPushDecDoneCb(UINT32 u4Tag, UINT32 u4FbgId, UINT32 u4FbId, BOOL fgEos)
{
    VCONV_INST_T*   prInst;
    VCONV_SRC_T     rSrc;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_GET_INTO_T rVdecGetInfo;
    UINT8 ucOmxHandle = (UINT8) u4Tag;
    FBM_SEQ_HDR_T *prSeqHdrSrc;

    prInst = prGetInst(u4Tag);
    if(!prInst)
    {
        LOG(1, "[VConv] prInst null\n");
        return FALSE;
    }
    x_memset(&rVdecGetInfo, 0, sizeof(VDEC_GET_INTO_T));
    if(!_VPUSH_GetInfo(prInst->pvVPush, &rVdecGetInfo))
    {
        LOG(1, "[VConv] _VPUSH_GetInfo fail\n");
        return FALSE;
    }
    prVdecEsInfo = _VDEC_GetEsInfo(rVdecGetInfo.ucVdecId);
    if(!prVdecEsInfo)
    {
        LOG(1, "[VConv] prVdecEsInfo null\n");
        return FALSE;
    }
    ASSERT(prInst->u4InstId == u4Tag);
    //if first cb, set w&h first.
	prSeqHdrSrc = FBM_GetFrameBufferSeqHdr((UCHAR)u4FbgId);
	if(prSeqHdrSrc)
	{
		vVConv_GetFrmRate(u4Tag,prSeqHdrSrc->ucFrmRatCod);
		if(_bOutputFisrtCb[u4Tag] == TRUE)
		{
			VDP_REGION_T rRegion;
			rRegion.u4X = 0;
			rRegion.u4Y =  0;
			rRegion.u4Width = 0;
			rRegion.u4Height = 0;
			VDP_SetSrcRegion(VDP_1,1,rRegion);

			_arVConvInfo[u4Tag].u4FrameHeight = prSeqHdrSrc->u2VSize;
			_arVConvInfo[u4Tag].u4FrameWidth = prSeqHdrSrc->u2HSize;

			if(prInst->pvCbFunc)
			{
			    // some caller will call executing later, but some are not.
			    //vVConv_ChangeMainState(prInst, VCONV_MAIN_PAUSE);
			    LOG(0,"Vconv_main Cb Pic Setting change\n ");
			    prInst->pvCbFunc(prInst->pvCbPrivData, prInst->u4InstId,
			        VCONV_NOTIFY_CHANGE_BUF, NULL);
			    x_thread_delay(5);
			}
			_bOutputFisrtCb[u4Tag] = FALSE;
		}
	}
    if((prVdecEsInfo->ucFbgId != u4FbgId)&&(prVdecEsInfo->ucSubFbgId != u4FbgId))
    {
        // this callback is comes from another place. I.e. io_mtb2r.c
        // do nothing
        LOG(2, "[VConv] DecDoneCb Error Fbg(%d!,%d,%d)\n",u4FbgId,prVdecEsInfo->ucFbgId,prVdecEsInfo->ucSubFbgId);
        return TRUE;
    }
    
    if(/*(prInst->eMainSt == VCONV_MAIN_NONE) ||
              (prInst->eMainSt == VCONV_MAIN_INIT) ||
            */
       (prInst->eMainSt == VCONV_MAIN_STOP) ||
       (prInst->eMainSt == VCONV_MAIN_FLUSH))
    {
        // this callback is comes from another place. I.e. io_mtb2r.c
        // do nothing
        LOG(2, "[VConv] DecDoneCb Error eMainSt = %d\n",prInst->eMainSt);
        return TRUE;
    }
    if(_prVDOOmxHandle[u4Tag].eFlushStatus != VOMX_FLUSHING_NONE)
	{
    	LOG(2, "[VConv] DecDoneCb Error drop src when FLUSH\n");
        return TRUE;
	}

    LOG(7, "[VConv] fgVConv_VPushDecDoneCb\n");

    x_memset(&rSrc, 0, sizeof(VCONV_SRC_T));
    rSrc.fgEos = fgEos;
    rSrc.ucFbgId = (UCHAR)u4FbgId;
    rSrc.ucFbId = (UCHAR)u4FbId;
    if (0 != vVDOOmxGetThumbnailMode(ucOmxHandle))
    {
        if (0 != rSrc.fgEos)
        {
            LOG(1, "[Vconv] vomx%d drop eos in thumbnail mode.fbid = %d\n", ucOmxHandle, rSrc.ucFbId);
            FBM_ClrFrameBufferPicFlag(rSrc.ucFbgId, rSrc.ucFbId,
                    (FBM_MM_EOS_FLAG | FBM_MM_PSEUDO_EOS_FLAG));
            rSrc.fgEos = 0;
            return TRUE;//drop this frame since gstreamer preroll only need on frame
        }
        else
        {
            LOG(1, "[Vconv] vdec%d in thumbnail mode last gid=%d, id=%d.\n", ucOmxHandle, rSrc.ucFbgId, rSrc.ucFbId);

            vVDOOmxSaveLastFb(ucOmxHandle, rSrc.ucFbgId, rSrc.ucFbId);
        }
    }
    vVConv_SendSrc(prInst, &rSrc);

    vVConv_SetEvent(prInst, VCONV_EVENT_WAIT_NEXT_PU);

    return (TRUE);
}

BOOL fgVConv_FbmCb(FBM_CBID eCBID, void *pvArg, void *pvPrivData)
{
    UINT32          u4InstId = VCONV_INVALID_DW;
    VCONV_INST_T*   prInst = NULL;

    LOG(7, "[VConv] fgVConv_FbmCb\n");

    if (FBM_FB_AVAIL == eCBID)
    {
        u4InstId = (UINT32)pvPrivData;

        prInst = prGetInst(u4InstId);

        vVConv_SetEvent(prInst, VCONV_EVENT_WAIT_NEXT_PU);
    }

    return (TRUE);
}

static INT32 i4VConv_AllocFb(VCONV_INST_T* prInst, BOOL fgUserBuf, UINT32* pu4BufAddr, UINT32 u4Size, BOOL fgAllocBuf)
{
    //UINT8       u1FbId = VCONV_INVALID_B;
    UINT32      u4Idx = 0;
    UINT32      u4YAddr = 0;
    UINT32      u4CAddr = 0;

    if(fgUserBuf)
    {
        prInst->fgUserBuf = TRUE;
    }
    if(fgAllocBuf)
    {
        prInst->fgAllocBuf = TRUE;
    }


    u4CAddr = 0;

    for (u4Idx = 0; u4Idx < VCONV_MAX_FB_COUNT; u4Idx ++)
    {
        if (0 == prInst->arFbInfo[u4Idx].u4FbAddr)
        {
            if(prInst->fgAllocBuf)
            {
                if(prInst->eColor == VCONV_COLOR_B2R2)
                {
                    if(prInst->u1FbCount >= VCONV_MAX_FB_COUNT)
                    {
                        LOG(0, "[VConv] [ERR] can not find a empty FbInfo unit when alloc fb\n");
                        ASSERT(0);
                        return (VCONV_FB_ERR);
                    }
                    u4YAddr = VIRTUAL((UINT32)BSP_AllocAlignedDmaMemory(VCONV_BUF_SIZE,16));
                    u4CAddr = u4YAddr + (VCONV_MAX_WIDTH*VCONV_MAX_HEIGHT);
                }
                else if(prInst->eColor == VCONV_COLOR_YUV420)
                {
#if !defined(VOMX_NO_GFX_IOMMU) && defined(ANDROID)
                    u4YAddr = VIRTUAL(prInst->u4DstFbgAddr) + u4Idx*sizeof(B2R_SRC_INFO_T);
                    u4CAddr = u4YAddr;//+ u4Idx*sizeof(B2R_SRC_INFO_T);
#else
                    u4YAddr = VIRTUAL(prInst->u4DstFbgAddr);
                    u4CAddr = u4YAddr + VCONV_BUF_SIZE_YUV420;
#endif

#if 0
                    if(prInst->u1FbCount >= VCONV_MAX_YUV420_FB_COUNT)
                    {
                        LOG(0, "[VConv] [ERR] can not find a empty FbInfo unit when alloc fb\n");
                        ASSERT(0);
                        return(VCONV_FB_ERR);
                    }

                    if(((prInst->u1FbCount + 2) * VCONV_BUF_SIZE_YUV420) > prInst->u4DstFbgSize)
                    {
                        LOG(0, "[VConv] [ERR] not enough memory\n");
                        ASSERT(0);
                    	return(VCONV_FB_ERR);
                    }
                    u4YAddr = VIRTUAL(prInst->u4DstFbgAddr + (u4Idx * VCONV_BUF_SIZE_YUV420));
                    u4CAddr = u4YAddr + (VCONV_MAX_WIDTH*VCONV_MAX_HEIGHT);
#endif
                }
                else if(prInst->eColor == VCONV_COLOR_YUV420SemiPlanar)
                {
                    if(prInst->u1FbCount >= VCONV_MAX_YUV420SEMI_FB_COUNT)
                    {
                        LOG(0, "[VConv] [ERR] can not find a empty FbInfo unit when alloc fb\n");
                        ASSERT(0);
                        return (VCONV_FB_ERR);
                    }

                    if(((prInst->u1FbCount + 1) * VCONV_BUF_SIZE_YUV420SEMI) > prInst->u4DstFbgSize)
                    {
                        LOG(0, "[VConv] [ERR] not enough memory\n");
                        ASSERT(0);
                        return (VCONV_FB_ERR);
                    }
                    u4YAddr = VIRTUAL(prInst->u4DstFbgAddr + (u4Idx * VCONV_BUF_SIZE_YUV420SEMI));
                    u4CAddr = u4YAddr + (VCONV_MAX_WIDTH*VCONV_MAX_HEIGHT);
                }
                else //if(prInst->eColor == VCONV_COLOR_ARBG8888)
                {
                    if(prInst->u1FbCount >= VCONV_MAX_ARGB_FB_COUNT)
                    {
                        LOG(0, "[VConv] [ERR] can not find a empty FbInfo unit when alloc fb\n");
                        ASSERT(0);
                        return (VCONV_FB_ERR);
                    }

                    // last one is working buffer
                    if(((prInst->u1FbCount + 1) * VCONV_BUF_SIZE_ARBG) > prInst->u4DstFbgSize)
                    {
                        LOG(0, "[VConv] [ERR] not enough memory\n");
                        ASSERT(0);
                        return (VCONV_FB_ERR);
                    }
                    u4YAddr = VIRTUAL(prInst->u4DstFbgAddr + (u4Idx * VCONV_BUF_SIZE_ARBG));
                    u4CAddr = u4YAddr + (VCONV_MAX_WIDTH_ARGB*VCONV_MAX_HEIGHT_ARGB);
                }
            }
            else
            {
                u4YAddr = *pu4BufAddr;
                u4CAddr = *pu4BufAddr + (VCONV_MAX_WIDTH*VCONV_MAX_HEIGHT);
            }

            prInst->arFbInfo[u4Idx].u4FbAddr = u4YAddr;
            prInst->arFbInfo[u4Idx].u4FbW = prInst->u4NeedAllocFbWidth;
            prInst->arFbInfo[u4Idx].u4FbH = prInst->u4NeedAllocFbHeight;
            prInst->arFbInfo[u4Idx].u4InternalFbAddr = u4YAddr;
            prInst->arFbInfo[u4Idx].u4InternalFbAddrC = u4CAddr;
            break;
        }
    }

    if (VCONV_MAX_FB_COUNT == u4Idx)
    {
        LOG(0, "[VConv] [ERR] can not find a empty FbInfo unit when alloc fb\n");
        ASSERT(0);
        return (VCONV_FB_ERR);
    }

    prInst->u1FbCount ++;

    *pu4BufAddr = u4YAddr;

    LOG(6, "[VConv] [FB] alloc FB, addr 0x%x success\n",
                    u4YAddr);

    return (VCONV_OK);
}

static INT32 i4VConv_FreeFb(VCONV_INST_T* prInst, UINT32 u4FbAddr)
{
    INT32       i4Ret = S_FBM_OK;
    UINT32      u4Idx = 0;

    i4Ret = i4VConv_SearchFbInfo(prInst, u4FbAddr, &u4Idx);
    if (VCONV_OK != i4Ret)
    {
        LOG(0, "[VConv] [ERR] can not find a fill FbInfo unit when free\n");
        ASSERT(0);
    }

    if(prInst->fgAllocBuf)
    {
        if(prInst->eColor == VCONV_COLOR_B2R2)
        {
            if(prInst->arFbInfo[u4Idx].u4InternalFbAddr)
            {
                BSP_FreeAlignedDmaMemory(PHYSICAL(prInst->arFbInfo[u4Idx].u4InternalFbAddr));
            }
        }
    }
    prInst->arFbInfo[u4Idx].u4InternalFbAddr = 0;
    prInst->arFbInfo[u4Idx].u4InternalFbAddrC = 0;

    prInst->u1FbCount --;

    if (0 == prInst->u1FbCount)
    {
        x_memset(&(prInst->arFbInfo),
                 0,
                 (VCONV_MAX_FB_COUNT * sizeof(VCONV_FB_INFO_T)));

        prInst->rSrc.ucFbgId = FBM_FBG_ID_UNKNOWN;

        if (VCONV_EVENT_WAIT_CHANGE_FBM_BUF != prInst->eEventWait)
        {
            vVConv_ChangeMainState(prInst, VCONV_MAIN_INIT);
        }
    }

    return (VCONV_OK);
}

static INT32 i4VConv_GetNextCmd(VCONV_INST_T* prInst)
{
    UINT32          u4CmdCountInQ = 0;
    VCONV_CMD_T     rCmd;

    u4CmdCountInQ = u4VConv_GetCmdCountInQ(prInst);

    if (0 < u4CmdCountInQ)
    {
        vVConv_ReceiveCmd(prInst, &rCmd);

        if (VCONV_CMD_FILLFBMBUF == rCmd.eCmd)
        {
            x_memcpy(&(prInst->rFillParam),
                     &(rCmd.uCmdParam.rFillBufParam),
                     sizeof(VCONV_FILL_FBM_BUF_PARAM_T));

            LOG(7, "[VConv] [cmd] receive cmd param, FillBuf 0x%x\n",
                prInst->rFillParam.u4FbmBufAddr);

			if ((VCONV_MAIN_FLUSH != prInst->eMainSt) &&
				(VCONV_MAIN_STOP != prInst->eMainSt))
			{			
				vVConv_ChangeMainState(prInst, VCONV_MAIN_CONVERT);
    	        vVConv_ChangeSubState(prInst, VCONV_SUB_GET_NEXT_PB);
			}

        }
        else
        {
            LOG(0, "[VConv] [ERR] receive cmd type %d\n", rCmd.eCmd);
            ASSERT(0);
        }

        return (VCONV_OK);
    }
    else
    {
        return (VCONV_NO_NEXT_CMD);
    }
}

static INT32 i4VConv_SubIdleState(VCONV_INST_T* prInst)
{
    LOG(1, "[VConv] [ERR] proc VCONV_SUB_IDLE\n");
    return (VCONV_OK);
}

static INT32 i4VConv_SubGetNextFbState(VCONV_INST_T* prInst)
{
    FBM_SEQ_HDR_T *prSeqHdrSrc;
    VCONV_SUB_STATE_T   eNextSubSt = VCONV_SUB_COMPLETE_CONVERT;

    vVConv_ReceiveSrc(prInst, &prInst->rSrc);

    if(prInst->rSrc.ucFbgId != FBM_FB_ID_UNKNOWN)
    {
        LOG(7, "[VConv] Get next FB(%d) from Q success\n", prInst->rSrc.ucFbId);
        eNextSubSt = VCONV_SUB_CHANGE_FBM_BUF;

        prSeqHdrSrc = FBM_GetFrameBufferSeqHdr(prInst->rSrc.ucFbgId);
        if (!prSeqHdrSrc)
        {
            // FbgId change handle
            if (vVConv_HandleFbgIdChange(prInst))
            {
                LOG(3, "[VConv] ucFbgId change case(L%d)\n", __LINE__);
                vVConv_ChangeSubState(prInst, VCONV_SUB_GET_NEXT_PB);
                return (VCONV_OK);
            }
            else
            {
            LOG(0, "[VConv] [ERR] prSeqHdrSrc null\n");
            ASSERT(0);
            }
            return (VCONV_NOT_OK);
        }
#ifdef ANDROID
        // when the resolution change is still smaller than original buffer size,
        // ignore this change notify.
        //if((prSeqHdrSrc->u2HSize != prInst->u4FrameWidth) ||
        //   (prSeqHdrSrc->u2VSize != prInst->u4FrameHeight))
        if((prInst->u4FrameWidth * prInst->u4FrameHeight) >
           (prSeqHdrSrc->u2LineSize * prSeqHdrSrc->u2VSize))
#else
        if((prSeqHdrSrc->u2HSize != prInst->u4FrameWidth) ||
           (prSeqHdrSrc->u2VSize != prInst->u4FrameHeight))

#endif
        {
            prInst->u4FrameWidth = prSeqHdrSrc->u2HSize;
            prInst->u4FrameHeight = prSeqHdrSrc->u2VSize;
            if(prInst->pvCbFunc)
            {
                // some caller will call executing later, but some are not.
                //vVConv_ChangeMainState(prInst, VCONV_MAIN_PAUSE);
                prInst->pvCbFunc(prInst->pvCbPrivData, prInst->u4InstId,
                    VCONV_NOTIFY_CHANGE_BUF, NULL);
                x_thread_delay(5);
            }
        }
    }
    else
    {
        eNextSubSt = VCONV_SUB_GET_NEXT_PB;
        vVConv_WaitEvent(prInst, VCONV_EVENT_WAIT_NEXT_PU);
    }

    vVConv_ChangeSubState(prInst, eNextSubSt);

    return (VCONV_OK);
}

static INT32 i4VConv_SubChangeFbState(VCONV_INST_T* prInst)
{
    INT32               i4Ret;
    FBM_SEQ_HDR_T *prSeqHdrSrc;
    UINT32 u4Idx = 0;

    //src
    if ((FBM_FBG_ID_UNKNOWN == prInst->rSrc.ucFbgId) ||
        (FBM_FB_ID_UNKNOWN == prInst->rSrc.ucFbId))
    {
        LOG(1, "[VConv] [ERR] src Fb[%d, %d] when i4VConv_SubChangeFbState\n",
            prInst->rSrc.ucFbgId, prInst->rSrc.ucFbId);
        ASSERT(0);
    }

    //Dst
    i4Ret = i4VConv_SearchFbInfo(prInst, prInst->rFillParam.u4FbmBufAddr, &u4Idx);
    if (VCONV_OK != i4Ret)
    {
        LOG(0, "[VConv] [ERR] can not find a fill FbInfo unit when get fill param\n");
        //workaround for vconv is still running. We should wait for VConv stop.
        if((prInst->eMainSt == VCONV_MAIN_IDLE) ||
           (prInst->eMainSt == VCONV_MAIN_INIT) ||
           (prInst->eMainSt == VCONV_MAIN_NONE))
        {
            return (VCONV_OK);
        }
        ASSERT(0);
    }

    prSeqHdrSrc = FBM_GetFrameBufferSeqHdr(prInst->rSrc.ucFbgId);
    if (!prSeqHdrSrc)
    {
        // FbgId change handle
        if (vVConv_HandleFbgIdChange(prInst))
        {
            LOG(3, "[VConv] ucFbgId change case(L%d)\n", __LINE__);
            vVConv_ChangeSubState(prInst, VCONV_SUB_GET_NEXT_PB);
            return (VCONV_OK);
        }
        else
        {
        LOG(0, "[VConv] [ERR] prSeqHdrSrc null\n");
        ASSERT(0);
        }
        return (VCONV_NOT_OK);
    }

    if(prInst->eColor == VCONV_COLOR_ARBG8888)
    {
        prInst->u4Stride = prSeqHdrSrc->u2HSize * 4;
        prInst->rCurrFill.u4DstBufSize =
            prSeqHdrSrc->u2LineSize * prSeqHdrSrc->u2VSize * 4;
    }
    else if(prInst->eColor == VCONV_COLOR_YUV420)
    {
        // YUV420
        prInst->u4Stride = (((prSeqHdrSrc->u2HSize + 31)>>5)<<5);
        prInst->rCurrFill.u4DstBufSize =
            prSeqHdrSrc->u2LineSize * prSeqHdrSrc->u2VSize * 3 /2;
    }
    else if(prInst->eColor == VCONV_COLOR_YUV420SemiPlanar)
    {
        // NV12
        prInst->u4Stride = prSeqHdrSrc->u2HSize;
        prInst->rCurrFill.u4DstBufSize =
            prSeqHdrSrc->u2LineSize * prSeqHdrSrc->u2VSize * 3 /2;
    }
    else if(prInst->eColor == VCONV_COLOR_B2R2)
    {
        prInst->u4Stride = prSeqHdrSrc->u2LineSize;
        prInst->rCurrFill.u4DstBufSize =
            prSeqHdrSrc->u2LineSize * prSeqHdrSrc->u2VSize * 3 /2;
    }
    else
    {
        LOG(0, "[VConv] [ERR] eColor(%d) unknown\n", prInst->eColor);
        ASSERT(0);
    }

    prInst->u4CurIdx = u4Idx;
    prInst->u4InternalAddr = prInst->arFbInfo[u4Idx].u4InternalFbAddr;
    prInst->u4InternalAddrC = prInst->arFbInfo[u4Idx].u4InternalFbAddrC;

    vVConv_ChangeSubState(prInst, VCONV_SUB_CALL_GFX_CONVERT);

    return (VCONV_OK);
}

static INT32 i4VConv_SubCallGfxState(VCONV_INST_T* prInst)
{
    VCONV_VDEC_OMX_NFY_PARAM rNfyParam;
    FBM_PIC_HDR_T *prPicHdr;
    UCHAR ucFbId;
    UINT64 u8Pts = 0;

    x_memset(&rNfyParam, 0, sizeof(VCONV_VDEC_OMX_NFY_PARAM));

    if(prInst->eColor != VCONV_COLOR_B2R2)
    {
        ucFbId = FBM_PreLookFrameBufferFromDispQ(prInst->rSrc.ucFbgId);
        if((prInst->rSrc.ucFbId != ucFbId) ||
           (prInst->rSrc.ucFbgId == FBM_FBG_ID_UNKNOWN) ||
           (prInst->rSrc.ucFbId == FBM_FB_ID_UNKNOWN))
		{		
            // FbgId change handle
            if (vVConv_HandleFbgIdChange(prInst))
            {
                LOG(3, "[VConv] ucFbgId change case(L%d)\n", __LINE__);
                vVConv_ChangeSubState(prInst, VCONV_SUB_GET_NEXT_PB);
                return (VCONV_OK);
            }
            else
            {
	            LOG(1, "[VConv] ucFbgId(%d) ucFbId(%d) != Q ucFbId(%d)\n",
	                prInst->rSrc.ucFbgId, prInst->rSrc.ucFbId, ucFbId);
                ASSERT(0);

	            // this picture is removed from decoder because there is no buffer
	            // we have to put this fill buffer cmd into cmd queue
	            prInst->rSrc.ucFbgId = FBM_FBG_ID_UNKNOWN;
	            prInst->rSrc.ucFbId = FBM_FB_ID_UNKNOWN;
	            prInst->rSrc.fgEos = FALSE;

		        vVConv_ChangeSubState(prInst, VCONV_SUB_COMPLETE_CONVERT);
		        return (VCONV_OK);
			}

		}
        ucFbId = FBM_GetFrameBufferFromDispQ(prInst->rSrc.ucFbgId);
   }

    if(FBM_ChkFrameBufferPicFlag(prInst->rSrc.ucFbgId, prInst->rSrc.ucFbId,
        (FBM_MM_EOS_FLAG | FBM_MM_PSEUDO_EOS_FLAG)))
    {
        if(!prInst->rSrc.fgEos)
        {
            LOG(1, "[VConv] ucFbgId(%d) ucFbId(%d) eos mismatch\n",
                prInst->rSrc.ucFbgId, prInst->rSrc.ucFbId);
            ASSERT(0);
        }
        prInst->fgSendEos = TRUE;
        LOG(3, "[VConv] EOS = 1\n");
    }

    prPicHdr = FBM_GetFrameBufferPicHdr(prInst->rSrc.ucFbgId, prInst->rSrc.ucFbId);
    if(prPicHdr)
    {
        #ifdef VDEC_PUSH_PTS_64_BITS
        if(prPicHdr->u8PTS >= 3600)
        {
            prPicHdr->u8PTS -= 3600;
        }
        u8Pts = prPicHdr->u8PTS;
        #else
        UINT64 u8Tmp;
        UINT64 u8Remainder;
        
        if(prPicHdr->u4PTS >= 3600)
        {
            prPicHdr->u4PTS -= 3600;
        }
        u8Tmp = (UINT64)prPicHdr->u4PTS;
        u8Pts = (UINT64)u8Div6432((u8Tmp*100), 9, &u8Remainder);
        #endif
        //u4Pts = prPicHdr->u4PTS;
    }
    else
    {
        LOG(1, "[VConv] prPicHdr null\n");
    }

    if(prInst->eColor == VCONV_COLOR_ARBG8888)
    {
        if(!vVConv_B2R(prInst, prInst->rSrc.ucFbgId, prInst->rSrc.ucFbId,
            PHYSICAL(prInst->u4InternalAddr), PHYSICAL(prInst->u4InternalAddrC),
            FALSE, 6,
            TRUE, 0))
        {
            return (VCONV_OK);
        }
        prInst->rCurrFill.u4DstFillSize = prInst->u4Stride * prInst->u4FrameHeight;

    }
    else if(prInst->eColor == VCONV_COLOR_B2R2)
    {
        if(!vVConv_B2B2R(prInst, prInst->rSrc.ucFbgId, prInst->rSrc.ucFbId,
            PHYSICAL(prInst->u4InternalAddr), PHYSICAL(prInst->u4InternalAddrC),
            FALSE, 6,
            TRUE, 0))
        {
            return (VCONV_OK);
        }
		if(prInst->rSrc.fgEos)
		{
			prInst->rCurrFill.u4DstFillSize = 0;
		}
		else
		{
			prInst->rCurrFill.u4DstFillSize = sizeof(FBM_INTERNAL_BUF_T);
		}
    }
    else if(prInst->eColor == VCONV_COLOR_YUV420)
    {
        if(!vVConv_B2YUV420Planar(prInst, prInst->rSrc.ucFbgId, prInst->rSrc.ucFbId,
            PHYSICAL(prInst->u4InternalAddr), PHYSICAL(prInst->u4InternalAddrC),
            FALSE, 6,
            TRUE, 0))
        {
            return (VCONV_OK);
        }
    }
    else if(prInst->eColor == VCONV_COLOR_YUV420SemiPlanar)
    {
        if(!vVConv_B2YUV420SemiPlaner(prInst, prInst->rSrc.ucFbgId, prInst->rSrc.ucFbId,
            PHYSICAL(prInst->u4InternalAddr), PHYSICAL(prInst->u4InternalAddrC),
            FALSE, 6,
            TRUE, 0))
        {
            return (VCONV_OK);
        }
        prInst->rCurrFill.u4DstFillSize = prInst->u4Stride * prInst->u4FrameHeight * 3 / 2;
    }
    else
    {
        LOG(0, "[VConv] [ERR] eColor(%d) unknown\n", prInst->eColor);
        ASSERT(0);
    }

    if(prInst->eColor != VCONV_COLOR_B2R2)
    {
#if defined(VOMX_NO_GFX_IOMMU) || !defined(ANDROID)
        FBM_SetFrameBufferStatus(prInst->rSrc.ucFbgId,
            prInst->rSrc.ucFbId, FBM_FB_STATUS_LOCK);
        FBM_SetFrameBufferStatus(prInst->rSrc.ucFbgId,
            prInst->rSrc.ucFbId, FBM_FB_STATUS_EMPTY);
        LOG(7, "[VConv] ucFbgId(%d) ucFbId(%d)\n",
            prInst->rSrc.ucFbgId, prInst->rSrc.ucFbId);
        prInst->rSrc.ucFbgId = FBM_FBG_ID_UNKNOWN;
        prInst->rSrc.ucFbId = FBM_FB_ID_UNKNOWN;
#endif
    }

    rNfyParam.fgEos      = prInst->rSrc.fgEos;
    rNfyParam.u4BufAddr  = prInst->u4InternalAddr;
    rNfyParam.u4BufSize  = prInst->rCurrFill.u4DstBufSize;
    rNfyParam.u4FillSize = prInst->rCurrFill.u4DstFillSize;
    rNfyParam.u8PTS      = u8Pts;

    prInst->pvCbFunc(prInst->pvCbPrivData, prInst->u4InstId, VCONV_NOTIFY_FILLBUF_DONE, &rNfyParam);

    if(rNfyParam.fgEos)
    {
        LOG(5, "[VConv] VCONV_NOTIFY_FILLBUF_DONE, EOS flag %d\n", rNfyParam.fgEos);
    }

    prInst->rSrc.ucFbgId = FBM_FBG_ID_UNKNOWN;
    prInst->rSrc.ucFbId = FBM_FB_ID_UNKNOWN;
    prInst->rSrc.fgEos = FALSE;

    vVConv_ChangeSubState(prInst, VCONV_SUB_COMPLETE_CONVERT);

    return (VCONV_OK);
}

static INT32 i4VConv_SubCompleteState(VCONV_INST_T* prInst)
{
    INT32 i4Ret;

    if (VCONV_MAIN_CONVERT == prInst->eMainSt)
    {
        i4Ret = i4VConv_GetNextCmd(prInst);
        if (VCONV_OK == i4Ret)
        {
            vVConv_ChangeSubState(prInst, VCONV_SUB_GET_NEXT_PB);
        }
        else if (VCONV_NO_NEXT_CMD == i4Ret)
        {
            vVConv_ChangeSubState(prInst, VCONV_SUB_IDLE);
        }
        else
        {
            LOG(0, "[VConv] [ERR] return 0x%x by i4VConv_GetNextCmd in i4VConv_SubCompleteState\n", i4Ret);
            ASSERT(0);
        }
    }
    else
    {
        vVConv_ChangeSubState(prInst, VCONV_SUB_IDLE);
    }

    return (VCONV_OK);
}

static INT32 i4VConv_ConvSubStateProc(VCONV_INST_T* prInst)
{
    INT32 i4Ret;

    VCONV_MAIN_STATE_T      eCurrMainSt;
    VCONV_SUB_STATE_T       eCurrSubSt;

    eCurrMainSt = prInst->eMainSt;
    eCurrSubSt = prInst->eSubSt;

    switch (prInst->eSubSt)
    {
    case VCONV_SUB_NONE:
        LOG(1, "[VConv] [ERR] proc VCONV_SUB_NONE\n");
        break;

    case VCONV_SUB_IDLE:
        i4Ret = i4VConv_SubIdleState(prInst);
        break;

    case VCONV_SUB_GET_NEXT_PB:
        i4Ret = i4VConv_SubGetNextFbState(prInst);
        break;

    case VCONV_SUB_CHANGE_FBM_BUF:
        i4Ret = i4VConv_SubChangeFbState(prInst);
        break;

    case VCONV_SUB_CALL_GFX_CONVERT:
        i4Ret = i4VConv_SubCallGfxState(prInst);
        break;

    case VCONV_SUB_COMPLETE_CONVERT:
        i4Ret = i4VConv_SubCompleteState(prInst);
        break;

    default:
        LOG(0, "[VConv] [ERR] wrong state[%d, %d]\n",
            eCurrMainSt, eCurrSubSt);
        ASSERT(0);
    }

    return (VCONV_OK);
}

static INT32 i4VConv_MainInitState(VCONV_INST_T* prInst)
{
    return (VCONV_OK);
}

static INT32 i4VConv_MainIdleState(VCONV_INST_T* prInst)
{
    i4VConv_GetNextCmd(prInst);

    return (VCONV_OK);
}

static INT32 i4VConv_MainConvertState(VCONV_INST_T* prInst)
{
    INT32 i4Ret = VCONV_OK;

    while ((VCONV_SUB_IDLE != prInst->eSubSt) &&
           (VCONV_SUB_NONE != prInst->eSubSt))
    {
        i4Ret = i4VConv_ConvSubStateProc(prInst);

        if (VCONV_OK != i4Ret)
        {
            LOG(1, "[VConv] [ERR] return 0x%x when i4VConv_MainConvertState\n",
                i4Ret);
        }

        if ((VCONV_MAIN_FLUSH == prInst->eMainSt) ||
            (VCONV_MAIN_STOP == prInst->eMainSt) ||
            ((VCONV_MAIN_PAUSE == prInst->eMainSt) && (VCONV_SUB_COMPLETE_CONVERT == prInst->eSubSt)))
        {
            LOG(5, "[VConv] exist convert when checking main state %d, SubSt %d\n",
                prInst->eMainSt, prInst->eSubSt);

            break;
        }
    }

    if ((VCONV_MAIN_FLUSH != prInst->eMainSt) &&
        (VCONV_MAIN_STOP != prInst->eMainSt))
    {
        vVConv_ChangeMainState(prInst, VCONV_MAIN_IDLE);
    }

    return (i4Ret);
}

static INT32 i4VConv_MainPauseState(VCONV_INST_T* prInst)
{
    prInst->pvCbFunc(prInst->pvCbPrivData, prInst->u4InstId, VCONV_NOTIFY_PAUSE_DONE, NULL);

    vVConv_WaitEvent(prInst, VCONV_EVENT_PAUSE);

    return (VCONV_OK);
}

static INT32 i4VConv_MainStopState(VCONV_INST_T* prInst)
{
    vVConv_ClearCmdQ(prInst);
    vVConv_ClearSrcQ(prInst);

    vVConv_ChangeMainState(prInst, VCONV_MAIN_IDLE);

    prInst->pvCbFunc(prInst->pvCbPrivData, prInst->u4InstId, VCONV_NOTIFY_STOP_DONE, NULL);

    return (VCONV_OK);
}

static INT32 i4VConv_MainFlushState(VCONV_INST_T* prInst)
{
    vVConv_ClearCmdQ(prInst);
    vVConv_ClearSrcQ(prInst);

    vVConv_ChangeMainState(prInst, VCONV_MAIN_IDLE);

    prInst->pvCbFunc(prInst->pvCbPrivData, prInst->u4InstId, VCONV_NOTIFY_FLUSH_DONE, NULL);

    return (VCONV_OK);
}

static INT32 i4VConv_MainStateProc(VCONV_INST_T* prInst)
{
    INT32                   i4Ret = S_FBM_OK;
    VCONV_MAIN_STATE_T      eCurrMainSt;
    VCONV_SUB_STATE_T       eCurrSubSt;

    LOG(7, "[VConv] Start proc Main state %d\n", prInst->eMainSt);

    eCurrMainSt = prInst->eMainSt;
    eCurrSubSt = prInst->eSubSt;

    switch (prInst->eMainSt)
    {
    case VCONV_MAIN_NONE:
        break;

    case VCONV_MAIN_INIT:
        i4Ret = i4VConv_MainInitState(prInst);
        break;

    case VCONV_MAIN_IDLE:
        i4Ret = i4VConv_MainIdleState(prInst);
        break;

    case VCONV_MAIN_CONVERT:
        i4Ret = i4VConv_MainConvertState(prInst);
        break;

    case VCONV_MAIN_PAUSE:
        i4Ret = i4VConv_MainPauseState(prInst);
        break;

    case VCONV_MAIN_STOP:
        i4Ret = i4VConv_MainStopState(prInst);
        break;

    case VCONV_MAIN_FLUSH:
        i4Ret = i4VConv_MainFlushState(prInst);
        break;

    default:
        LOG(0, "[VConv] [ERR] wrong main state %d\n", eCurrMainSt);
        ASSERT(0);
        break;
    }

    if (VCONV_OK != i4Ret)
    {
        LOG(1, "[VConv] [ERR] state [%d, %d] return error 0x%x\n",
            eCurrMainSt, eCurrSubSt, i4Ret);
    }

    if ((VCONV_MAIN_FLUSH != prInst->eMainSt) &&
        (VCONV_MAIN_STOP != prInst->eMainSt) &&
        (VCONV_MAIN_PAUSE != prInst->eMainSt) &&
        (VCONV_MAIN_CONVERT != prInst->eMainSt))
    {
        vVConv_WaitEvent(prInst, VCONV_EVENT_CMD);
    }

    return (VCONV_OK);
}

static void vVConv_MainLoop(void* pvArg)
{
    UINT32          u4InstId;
    INT32           i4Ret = OSR_OK;
    VCONV_INST_T*   prInst;

    ASSERT(pvArg != NULL);

    u4InstId = *(UINT32*)pvArg;
    prInst = prGetInst(u4InstId);

    _au4VConvThreadActive[u4InstId] = 1;

    // already done in vVConv_ClearInst
    // reassign it may cause race condition on value assign.
    //prInst->eMainSt = VCONV_MAIN_NONE;
    //prInst->eSubSt  = VCONV_SUB_NONE;
    x_sema_unlock(prInst->hSemaThreadReady);//thread is ready to run
    prInst->fgFirstRun = 0;

    LOG(5, "[VConv] inst ID %d enter vVConv_MainLoop\n", u4InstId);

    while (_au4VConvThreadActive[u4InstId])
    {
        i4VConv_MainStateProc(prInst);
    }

    LOG(5, "[VConv] inst ID %d exist vVConv_MainLoop\n", u4InstId);

    vVConv_DeleteSema(prInst);
    vVConv_DeleteEventGroup(prInst);
    vVConv_DeleteCmdQ(prInst);
    BSP_FreeAlignedDmaMemory(prInst->u4DstFbgAddr);

    x_thread_exit();

    while(i4Ret == OSR_OK)
    {
       i4Ret = x_thread_set_pri(_ahVConvThreadHandle[u4InstId],  VCONV_THREAD_PRIORITY);
       if(i4Ret == OSR_OK)
       {
          LOG(1, "[VConv] VConv Thread[%d] exist fail\n", u4InstId);
          x_thread_delay(100);
       }
    }

    LOG(3, "[VConv] VConv Thread[%d] exist success\n", u4InstId);
}


/*************************** External Interface *******************************/
INT32 _i4VConv_WaitThreadReady(UINT32 u4InstId)
{
    VCONV_INST_T*   prInst;

    if (u4InstId >= VCONV_INST_NUM)
    {
        LOG(0, "[OMX][VCONV]: _i4VConv_WaitThreadReady id error %d. \n",  u4InstId);
        return -1;
    }

    prInst = prGetInst(u4InstId);

    // only need to wait at first time
    if (prInst->fgFirstRun)
    {
        LOG(3, "[OMX][VCONV]:wait semaphore. \n");
        VERIFY(x_sema_lock(prInst->hSemaThreadReady, X_SEMA_OPTION_WAIT) == OSR_OK);
    }
    
#if defined(VOMX_NO_GFX_IOMMU) || !defined(ANDROID)
    sema_init(&(prInst->rConvSema), 1);
#endif

    LOG(3, "[OMX][VCONV]: vconv thread(%d) is ready. \n", u4InstId);

    return 0;
}
INT32 _i4VConv_DrvInit(VOID)
{    
    static BOOL fgDrvInit = FALSE;
    UINT32 u4InstId = 0;    
    VCONV_INST_T* prInst;
    
    if(fgDrvInit)
        return (VCONV_OK);    
    LOG(3, "[VConv] start call _i4VConv_DrvInit\n");

//	    #if defined(CC_FBM_INT_TWO_FBP) || defined(CC_FBM_TWO_FBP_SHARED_WITH_DFB) || defined(CC_VOMX_TV_COEXIST)
//	    FBM_POOL_T* prFbmPool;
//	    #endif

    for (u4InstId = 0; u4InstId < VCONV_INST_NUM; u4InstId ++)
    {
        CHAR szBuf[16] = "VCONV0";
        szBuf[5] += u4InstId;

        prInst = prGetInst(u4InstId);

        vVConv_ClearInst(prInst);

        prInst->u4InstId  = u4InstId;
        prInst->fgFirstRun = 1;
        VERIFY (x_sema_create(&prInst->hSemaThreadReady, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);

        vVConv_CreateSema(prInst);

        vVConv_CreateEventGroup(prInst, szBuf);

        LOG(5, "[VConv] Inst %d create event handle %d\n", u4InstId, prInst->hEvent);

        vVConv_CreateCmdQ(prInst);

        VERIFY(x_thread_create(&_ahVConvThreadHandle[u4InstId],
                                szBuf,
                                VCONV_STACK_SIZE,
                                VCONV_THREAD_PRIORITY,
                                vVConv_MainLoop,
                                sizeof(UINT32),
                                (void*)&u4InstId) == OSR_OK);
        
#if !defined(VOMX_NO_GFX_IOMMU) && defined(ANDROID)
        prInst->u4DstFbgAddr = (UINT32)BSP_AllocAlignedDmaMemory(sizeof(B2R_SRC_INFO_T)*VCONV_MAX_FB_COUNT,64);
        LOG(0, "[VConv][Mem Allocated, Addr: %x]\n",  prInst->u4DstFbgAddr);
        prInst->u4DstFbgSize = sizeof(B2R_SRC_INFO_T)*VCONV_MAX_FB_COUNT;
#else
        prInst->u4DstFbgAddr = (UINT32)BSP_AllocAlignedDmaMemory(VCONV_MAX_WIDTH*VCONV_MAX_HEIGHT*2,64);
        LOG(0, "[VConv][Mem Allocated, Addr: %x]\n",  prInst->u4DstFbgAddr);
        prInst->u4DstFbgSize = VCONV_MAX_WIDTH*VCONV_MAX_HEIGHT*2;
        sema_init(&(prInst->rConvSema), 1);
#endif



#if 0
        #if defined(CC_FBM_INT_TWO_FBP)
        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG);
        if(prFbmPool == NULL)
        {
            LOG(1, "_i4VConv_DrvInit FBM_GetPoolInfo Fail\n");
            return FALSE;
        }
        if((prFbmPool->u4Addr == 0) || (prFbmPool->u4Size == 0))
        {
            LOG(1, "_i4VConv_DrvInit prFbmPool->u4Addr/u4Size 0\n");
            return FALSE;
        }
        prInst->u4DstFbgAddr = FBM_ALIGN_MASK(prFbmPool->u4Addr + FBM_FBG_TYPE_1080HD_H264_SIZE, FBM_FMG_Y_ALIGMENT);
        prInst->u4DstFbgSize= (prFbmPool->u4Size - (FBM_FBG_TYPE_1080HD_H264_SIZE + FBM_FMG_Y_ALIGMENT));
        #elif defined(CC_FBM_TWO_FBP_SHARED_WITH_DFB)
        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG2);
        if(prFbmPool == NULL)
        {
            LOG(1, "_i4VConv_DrvInit FBM_GetPoolInfo Fail\n");
            return FALSE;
        }
        if((prFbmPool->u4Addr == 0) || (prFbmPool->u4Size == 0))
        {
            LOG(1, "_i4VConv_DrvInit prFbmPool->u4Addr/u4Size 0\n");
            return FALSE;
        }
        prInst->u4DstFbgAddr = FBM_ALIGN_MASK(prFbmPool->u4Addr, FBM_FMG_Y_ALIGMENT);
        prInst->u4DstFbgSize= (prFbmPool->u4Size - FBM_FMG_Y_ALIGMENT);
		#elif defined(CC_VOMX_TV_COEXIST)
        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG3);
        if(prFbmPool == NULL)
        {
            LOG(1, "_i4VConv_DrvInit FBM_GetPoolInfo Fail\n");
            return FALSE;
        }
        if((prFbmPool->u4Addr == 0) || (prFbmPool->u4Size == 0))
        {
            LOG(1, "_i4VConv_DrvInit prFbmPool->u4Addr/u4Size 0\n");
            return FALSE;
        }
        prInst->u4DstFbgAddr = FBM_ALIGN_MASK(prFbmPool->u4Addr, FBM_FMG_Y_ALIGMENT);
        prInst->u4DstFbgSize= (prFbmPool->u4Size - FBM_FMG_Y_ALIGMENT);
		#else
        LOG(0, "Please define CC_FBM_INT_TWO_FBP or CC_FBM_TWO_FBP_SHARED_WITH_DFB or CC_VOMX_TV_COEXIST\n");
        ASSERT(0);
        return FALSE;
        #endif
#endif
    }
    
    LOG(3, "[VConv] end call _i4VConv_DrvInit\n");
    fgDrvInit = TRUE;

    return (VCONV_OK);
}

INT32 _i4VConv_DrvUninit(VOID)
{
    UINT32 u4InstId = 0;

    VCONV_INST_T* prInst = prGetInst(u4InstId);

    LOG(3, "[VConv] start call _i4VConv_DrvUninit\n");

    for (u4InstId = 0; u4InstId < VCONV_INST_NUM; u4InstId ++)
    {
        LOG(5, "[VConv] drv uninit inst id %d\n", u4InstId);

        _au4VConvThreadActive[u4InstId] = 0;

        vVConv_SetEvent(prInst, prInst->eEventWait);
    }

    LOG(3, "[VConv] end call _i4VConv_DrvUninit\n");

    return (VCONV_OK);
}

INT32 _i4VConv_Init(VOID)
{
    UINT32          u4InstId = 0;
    VCONV_INST_T*   prInst = NULL;

    LOG(5, "[VConv] start call _i4VConv_Init\n");

    for (u4InstId = 0; u4InstId < VCONV_INST_NUM; u4InstId ++)
    {
        prInst = prGetInst(u4InstId);

        if (VCONV_MAIN_NONE != prInst->eMainSt)
        {
            continue;
        }

        LOG(6, "[VConv] start init Inst[%d]\n", u4InstId);

        prInst->rSrc.ucFbgId = FBM_FBG_ID_UNKNOWN;
        prInst->rSrc.ucFbId = FBM_FB_ID_UNKNOWN;
        prInst->rSrc.fgEos = FALSE;
        prInst->u4FrameWidth = VCONV_MAX_WIDTH;
        prInst->u4FrameHeight = VCONV_MAX_HEIGHT;
        prInst->u4Stride = VCONV_MAX_WIDTH;
		prInst->u4FrameRate = VCONV_FRMRATE_UNKNOWN;
		vVConv_ClearSrcQ(prInst);
        vVConv_ChangeMainState(prInst, VCONV_MAIN_INIT);
        break;
    }

    LOG(5, "[VConv] end call _i4VConv_Init\n");

    return (VCONV_OK);
}

INT32 _i4VConv_GetHandle (HANDLE_T * ph_Handle)
{
    UINT32 u4Idx;

    VCONV_INST_T* prInst;

    LOG(5, "[VConv] start call _i4VConv_GetHandle, ph_Handle 0x%x\n",
        ph_Handle);

    if (NULL == ph_Handle)
    {
        LOG(0, "[VConv] [ERR] _i4VConv_GetHandle param ph_Handle = NULL\n");
        ASSERT(0);
        return (VCONV_NO_HANDLE);
    }

    *ph_Handle = 0;
    for (u4Idx = 0; u4Idx < VCONV_INST_NUM; u4Idx ++)
    {
        prInst = prGetInst(u4Idx);

        if (FALSE == prInst->fgGetHandle)
        {
            prInst->fgGetHandle = TRUE;
            *ph_Handle = u4Idx;
            break;
        }
    }

    if (VCONV_INST_NUM == u4Idx)
    {
        LOG(0, "[VConv] can not find free inst when _i4VConv_GetHandle\n");
        //ASSERT(0);
        return VCONV_NO_HANDLE;
    }

    LOG(6, "[VConv] end call _i4VConv_GetHandle, return handle %d\n",
        *ph_Handle);

    return (VCONV_OK);
}

INT32 _i4VConv_FreeHandle (HANDLE_T h_Handle)
{
    UINT32 u4InstId = (UINT32)h_Handle;

    VCONV_INST_T* prInst = prGetInst(u4InstId);

    LOG(5, "[VConv] start call _i4VConv_FreeHandle, handle %d\n",
        h_Handle);

    if (VCONV_INST_NUM <= u4InstId)
    {
        LOG(1, "[VConv] [ERR] invalid inst id %d when _i4VConv_FreeHandle",
                    u4InstId);
    }

    if (TRUE == prInst->fgGetHandle)
    {
        prInst->fgGetHandle = FALSE;
    }
    else
    {
        LOG(0, "[VConv] [ERR] this inst id %d is not active\n", u4InstId);
        ASSERT(0);
    }

    vVConv_ChangeMainState(prInst, VCONV_MAIN_NONE);

    LOG(6, "[VConv] end call _i4VConv_FreeHandle, handle %d\n",
        h_Handle);
    vVConv_ClearSrcQ(prInst);

    return (VCONV_OK);
}

INT32 _i4VConv_SetNotifyFct(HANDLE_T h_Handle, vOmxVdecVConvCbFunc pvCbFunc, void *pvCbPrivData)
{
    UINT32 u4InstId = (UINT32)h_Handle;

    VCONV_INST_T* prInst = prGetInst(u4InstId);

    LOG(5, "[VConv] start call _i4VConv_SetNotifyFct, handle %d, pvCbFunc 0x%x, pvCbPrivData 0x%x\n",
            h_Handle, pvCbFunc, pvCbPrivData);

    if (NULL == pvCbFunc)
    {
        LOG(0, "[VConv] [ERR] _i4VConv_SetNotifyFct param pvCbFunc = NULL\n");
        ASSERT(0);
    }

    prInst->pvCbFunc = pvCbFunc;
    prInst->pvCbPrivData = pvCbPrivData;

    LOG(6, "[VConv] end call _i4VConv_SetNotifyFct, handle %d\n",
            h_Handle);

    return (VCONV_OK);
}

INT32 _i4VConv_AllocFbmBuf (HANDLE_T h_Handle, BOOL fgUserBuf, UINT32 u4BufSize, UINT32 u4BufCount, UINT32* pu4BufAddr)
{
    UINT32  u4Width = 1920;
    UINT32  u4Height = 1088;
    UINT32  u4InstId = (UINT32)h_Handle;

    VCONV_INST_T* prInst = prGetInst(u4InstId);

    LOG(6, "[VConv] start call _i4VConv_AllocFbmBuf, handle %d, BufSize %d, BufCount %d, pu4BufAddr 0x%x\n",
                h_Handle, u4BufSize, u4BufCount, pu4BufAddr);

    if (NULL == pu4BufAddr)
    {
        LOG(0, "[VConv] [ERR] _i4VConv_AllocFbmBuf param pu4BufAddr = NULL\n");
        ASSERT(0);
        return (VCONV_NO_HANDLE);
    }

    prInst->u4NeedAllocFbWidth = u4Width;
    prInst->u4NeedAllocFbHeight = u4Height;
    prInst->u4NeedAllocFbCount = u4BufCount;

    LOG(6, "[VConv] [IF] alloc buf W %d, H %d, Count = %d\n",
                u4Width, u4Height, u4BufCount);

    i4VConv_AllocFb(prInst, fgUserBuf, pu4BufAddr, u4BufSize, TRUE);

    if ((1 == prInst->u1FbCount) &&
        (VCONV_EVENT_WAIT_CHANGE_FBM_BUF != prInst->eEventWait))
    {
        vVConv_ChangeMainState(prInst, VCONV_MAIN_IDLE);
    }

    LOG(6, "[VConv] end call _i4VConv_AllocFbmBuf, handle %d, ret buf 0x%x\n",
                h_Handle, *pu4BufAddr);

    return (VCONV_OK);
}

INT32 _i4VConv_FreeFbmBuf (HANDLE_T h_Handle, UINT32 u4BufAddr)
{
    UINT32 u4InstId = (UINT32)h_Handle;

    VCONV_INST_T* prInst = prGetInst(u4InstId);

    LOG(6, "[VConv] start call _i4VConv_FreeFbmBuf, handle %d, BufAddr 0x%x\n",
                h_Handle, u4BufAddr);

    prInst->u4NeedFreeFb = u4BufAddr;

    LOG(6, "[VConv] [IF] free buf addr 0x%x\n",
                u4BufAddr);

    i4VConv_FreeFb(prInst, u4BufAddr);

    LOG(6, "[VConv] end call _i4VConv_FreeFbmBuf, handle %d\n",
                h_Handle);

    return (VCONV_OK);
}

INT32 _i4VConv_ChangeFbmBuf (HANDLE_T h_Handle, UINT32 u4BufAddr)
{
    UINT32 u4InstId = (UINT32)h_Handle;

    VCONV_INST_T* prInst = prGetInst(u4InstId);

    LOG(6, "[VConv] start call _i4VConv_ChangeFbmBuf, handle %d, BufAddr 0x%x\n",
                h_Handle, u4BufAddr);

    prInst->u4ChangeFb = u4BufAddr;

    LOG(6, "[VConv] [FB] change buf to 0x%x\n", u4BufAddr);

    vVConv_SetEvent(prInst, VCONV_EVENT_WAIT_CHANGE_FBM_BUF);

    LOG(6, "[VConv] end call _i4VConv_ChangeFbmBuf, handle %d\n",
                h_Handle);

    return (VCONV_OK);
}

INT32 _i4VConv_SendCmd (HANDLE_T h_Handle, VCONV_CMD_TYPE_T eCmd, VOID* pvParam)
{
    VCONV_CMD_T rCmd;
    UINT32 u4InstId = (UINT32)h_Handle;

    VCONV_INST_T* prInst = prGetInst(u4InstId);

    UINT32 u4ParamSize = 0;

    LOG(7, "[VConv] start call _i4VConv_SendCmd, handle %d, eCmd %d, pvParam 0x%x\n",
                h_Handle, eCmd, pvParam);

    if (NULL == pvParam)
    {
        LOG(0, "[VConv] [ERR] _i4VConv_SendCmd param pvParam = NULL\n");
        ASSERT(0);
        return (VCONV_NOT_OK);
    }

    if (VCONV_CMD_FILLFBMBUF == eCmd)
    {
        u4ParamSize = sizeof(VCONV_FILL_FBM_BUF_PARAM_T);
    }

    rCmd.eCmd = eCmd;
    x_memcpy(&(rCmd.uCmdParam), pvParam, u4ParamSize);

    vVConv_SendCmd(prInst, &rCmd);

    LOG(7, "[VConv] end call _i4VConv_SendCmd, handle %d\n",
                h_Handle);

    return (VCONV_OK);
}

INT32 _i4VConv_Stop (HANDLE_T h_Handle)
{
    UINT32 u4InstId = (UINT32)h_Handle;

    VCONV_INST_T* prInst = prGetInst(u4InstId);

    LOG(5, "[VConv] start call _i4VConv_Stop, handle %d\n",
                h_Handle);

	_bOutputFisrtCb[u4InstId] = TRUE;
    vVConv_ChangeMainState(prInst, VCONV_MAIN_STOP);

    vVConv_ClearEvent(prInst);

    LOG(6, "[VConv] end call _i4VConv_Stop, handle %d\n",
                h_Handle);

    return (VCONV_OK);
}

INT32 _i4VConv_Flush (HANDLE_T h_Handle)
{
    UINT32 u4InstId = (UINT32)h_Handle;

    VCONV_INST_T* prInst = prGetInst(u4InstId);

    LOG(5, "[VConv] start call _i4VConv_Flush, handle %d\n",
                h_Handle);

    vVConv_ChangeMainState(prInst, VCONV_MAIN_FLUSH);

    vVConv_ClearEvent(prInst);

    LOG(6, "[VConv] end call _i4VConv_Flush, handle %d\n",
                h_Handle);

    return (VCONV_OK);
}

INT32 _i4VConv_Finish (HANDLE_T h_Handle)
{
    UINT32 u4InstId = (UINT32)h_Handle;

    VCONV_INST_T* prInst = prGetInst(u4InstId);

    LOG(5, "[VConv] start call _i4VConv_Finish, handle %d\n",
                h_Handle);

    vVConv_ChangeMainState(prInst, VCONV_MAIN_STOP);

    vVConv_ClearEvent(prInst);

    LOG(6, "[VConv] end call _i4VConv_Finish, handle %d\n",
                h_Handle);

    return (VCONV_OK);
}

INT32 _i4VConv_Pause (HANDLE_T h_Handle)
{
    UINT32 u4InstId = (UINT32)h_Handle;

    VCONV_INST_T* prInst = prGetInst(u4InstId);

    LOG(5, "[VConv] start call _i4VConv_Pause, handle %d\n",
                h_Handle);

    vVConv_ChangeMainState(prInst, VCONV_MAIN_PAUSE);

    LOG(6, "[VConv] end call _i4VConv_Pause, handle %d\n",
                h_Handle);

    return (VCONV_OK);
}

INT32 _i4VConv_Resume (HANDLE_T h_Handle)
{
    UINT32 u4InstId = (UINT32)h_Handle;

    VCONV_INST_T* prInst = prGetInst(u4InstId);

    LOG(5, "[VConv] start call _i4VConv_Resume, handle %d\n",
                h_Handle);

    if (VCONV_MAIN_PAUSE != prInst->eMainSt)
    {
        LOG(1, "[VConv] [ERR] Set pause cmd error because current state[%d, %d] is not pause\n",
                    prInst->eMainSt, prInst->eSubSt);
    }

    if (0 < u4VConv_GetCmdCountInQ(prInst))
    {
        vVConv_ChangeMainState(prInst, VCONV_MAIN_CONVERT);
        vVConv_ChangeSubState(prInst, VCONV_SUB_GET_NEXT_PB);
    }
    else
    {
        vVConv_ChangeMainState(prInst, VCONV_MAIN_IDLE);
    }

    vVConv_SetEvent(prInst, VCONV_EVENT_PAUSE);

    LOG(6, "[VConv] end call _i4VConv_Resume, handle %d\n",
                h_Handle);

    return (VCONV_OK);
}

INT32 _i4VConv_SetParam (HANDLE_T h_Handle, VCONV_SET_PARAM_T* prParam)
{
    UINT32 u4InstId = (UINT32)h_Handle;

    VCONV_INST_T* prInst = prGetInst(u4InstId);

    LOG(5, "[VConv] start call _i4VConv_SetParam, handle %d\n",
                h_Handle);

    if (NULL == prParam)
    {
        LOG(0, "[VConv] [ERR] _i4VConv_SetParam param prParam = NULL\n");
        ASSERT(0);
        return (VCONV_NOT_OK);
    }

    if(prParam->u4PortNum == 1)
    {
        // actually gst-omx does not set output port info
        if(prParam->eVConvFormat == VCONV_COLOR_B2R2)
        {
            prInst->eColor = VCONV_COLOR_B2R2;
        }
        else if(prParam->eVConvFormat == VCONV_COLOR_ARBG8888)
        {
            prInst->eColor = VCONV_COLOR_ARBG8888;
        }
        else if(prParam->eVConvFormat == VCONV_COLOR_YUV420)
        {
            prInst->eColor = VCONV_COLOR_YUV420;
        }
        else if(prParam->eVConvFormat == VCONV_COLOR_YUV420SemiPlanar)
        {
            prInst->eColor = VCONV_COLOR_YUV420SemiPlanar;
        }
        if((prParam->u4FrameWidth > 0) &&
           (prParam->u4FrameHeight > 0))
        {
            prInst->u4FrameWidth = prParam->u4FrameWidth;
            prInst->u4FrameHeight = prParam->u4FrameHeight;
            // cal stride later
        }
    }

    LOG(6, "[VConv] end call _i4VConv_SetParam, handle %d\n",
                h_Handle);

    return (VCONV_OK);
}

INT32 _i4VConv_GetParam (HANDLE_T h_Handle, VCONV_GET_PARAM_T* prParam)
{
    UINT32 u4InstId = (UINT32)h_Handle;

    VCONV_INST_T* prInst = prGetInst(u4InstId);

    LOG(5, "[VConv] start call _i4VConv_GetParam, handle %d\n",
                h_Handle);

    if (NULL == prParam)
    {
        LOG(0, "[VConv] [ERR] _i4VConv_GetParam param prParam = NULL\n");
        ASSERT(0);
        return (VCONV_NOT_OK);
    }

    prParam->u4PortCount = 2;
    prParam->u4StartPortNumber = 0;//prInst->u4InstId * 2;
    if(prParam->u4PortNum == 0)
    {
        VDEC_VFIFO_INTO_T rVfifoInfo;
		_VPUSH_GetVFifoInfo(prInst->pvVPush,&rVfifoInfo);
        prParam->u4BufferCountActual = rVfifoInfo.u4BufferCount;
        prParam->u4BufferCountMin = rVfifoInfo.u4BufferCount;
        prParam->u4BufferSize = rVfifoInfo.u4BufferSize;
        prParam->u4FrameWidth = 0;
        prParam->u4FrameHeight = 0;
        prParam->u4Stride = 0;
        prParam->eVConvFormat = VCONV_COLOR_MAX;
    }
    else if(prParam->u4PortNum == 1)
    {
        if(prInst->eColor == VCONV_COLOR_B2R2)
        {
            prParam->u4BufferCountActual = VCONV_MAX_FB_COUNT;
            prParam->u4BufferCountMin = VCONV_MAX_FB_COUNT;
            prParam->u4BufferSize = VCONV_BUF_SIZE;
        }
        else if(prInst->eColor == VCONV_COLOR_YUV420)
        {
        	prParam->u4BufferCountActual = VCONV_MAX_YUV420_FB_COUNT;
            prParam->u4BufferCountMin = VCONV_MAX_YUV420_FB_COUNT;
            prParam->u4BufferSize = VCONV_BUF_SIZE_YUV420;
        }
        else if(prInst->eColor == VCONV_COLOR_YUV420SemiPlanar)
        {
            prParam->u4BufferCountActual = VCONV_MAX_YUV420SEMI_FB_COUNT;
            prParam->u4BufferCountMin = VCONV_MAX_YUV420SEMI_FB_COUNT;
            prParam->u4BufferSize = VCONV_BUF_SIZE_YUV420SEMI;
        }
        else //if(prInst->eColor == VCONV_COLOR_ARBG8888)
        {
            prParam->u4BufferCountActual = VCONV_MAX_ARGB_FB_COUNT;
            prParam->u4BufferCountMin = VCONV_MAX_ARGB_FB_COUNT;
            prParam->u4BufferSize = VCONV_BUF_SIZE_ARBG;
        }
        prParam->eVConvFormat = prInst->eColor;
        prParam->u4FrameWidth = prInst->u4FrameWidth;
        prParam->u4FrameHeight = prInst->u4FrameHeight;
        prParam->u4Stride = prInst->u4Stride;
		prParam->u4FrameRate = prInst->u4FrameRate;
    }
    else
    {
        LOG(5, "[VConv] unknow u4PortNum(%d)\n", (UINT32)prParam->u4PortNum);
    }

    LOG(6, "[VConv] end call _i4VConv_GetParam, handle %d\n",
                h_Handle);

    return (VCONV_OK);
}

INT32 _i4VConv_SetPitchType (HANDLE_T h_Handle, BOOL fgRemovePitch)
{
    UINT32 u4InstId = (UINT32)h_Handle;

    VCONV_INST_T* prInst = prGetInst(u4InstId);

    LOG(5, "[VConv] start call _i4VConv_SetPitchType %d, handle %d\n",fgRemovePitch,
                h_Handle);
    
    prInst->fgRemovePitch = fgRemovePitch;

    LOG(6, "[VConv] end call _i4VConv_SetPitchType, handle %d\n",
                h_Handle);

    return (VCONV_OK);
}

INT32 _i4VConv_SetVPush (HANDLE_T h_Handle, VOID* prVPush)
{
    VDEC_PUSH_CB_T rPushCb;
    UINT32 u4InstId;
    VCONV_INST_T* prInst;

    u4InstId = (UINT32)h_Handle;
    prInst = prGetInst(u4InstId);
    prInst->pvVPush = prVPush;
    x_memset(&rPushCb, 0, sizeof(VDEC_PUSH_CB_T));
    rPushCb.u4Mask |= VDEC_PUSH_CB_DECODE_DONE;
    rPushCb.u4DecodeTag = h_Handle;
    rPushCb.pfnVdecDecodeDone = fgVConv_VPushDecDoneCb;
    _VPUSH_RegCb(prVPush, &rPushCb);

    return (VCONV_OK);
}

INT32 _i4VConv_PrintStatus (HANDLE_T h_Handle)
{
    UINT32 u4Idx;
    UINT32 u4InstId = (UINT32)h_Handle;

    VCONV_INST_T* prInst = prGetInst(u4InstId);
    LOG(0, "[VConv] Inst(%d) Main: ", u4InstId);
    switch(prInst->eMainSt)
    {
        case VCONV_MAIN_NONE:
            LOG(0, "NONE ");
            break;
        case VCONV_MAIN_INIT:
            LOG(0, "INIT ");
            break;
        case VCONV_MAIN_IDLE:
            LOG(0, "IDLE ");
            break;
        case VCONV_MAIN_CONVERT:
            LOG(0, "CONVERT ");
            break;
        case VCONV_MAIN_PAUSE:
            LOG(0, "PAUSE ");
            break;
        case VCONV_MAIN_STOP:
            LOG(0, "STOP ");
            break;
        case VCONV_MAIN_FLUSH:
            LOG(0, "FLUSH ");
            break;
        default:
            LOG(0, "Unknown ");
            break;
    }
    LOG(0, "Sub: ");
    switch(prInst->eSubSt)
    {
        case VCONV_SUB_NONE:
            LOG(0, "NONE ");
            break;
        case VCONV_SUB_IDLE:
            LOG(0, "IDLE ");
            break;
        case VCONV_SUB_GET_NEXT_PB:
            LOG(0, "GET_NEXT_PB ");
            break;
        case VCONV_SUB_CHANGE_FBM_BUF:
            LOG(0, "CHANGE_FBM_BUF ");
            break;
        case VCONV_SUB_CALL_GFX_CONVERT:
            LOG(0, "CALL_GFX_CONVERT ");
            break;
        case VCONV_SUB_COMPLETE_CONVERT:
            LOG(0, "COMPLETE_CONVERT ");
            break;
        default:
            LOG(0, "Unknown ");
            break;
    }
    LOG(0, "\n");
    LOG(0, "[VConv] eColor: ");
    switch(prInst->eColor)
    {
        case VCONV_COLOR_B2R2:
            LOG(0, "B2R2 ");
            break;
        case VCONV_COLOR_ARBG8888:
            LOG(0, "ARBG8888 ");
            break;
        case VCONV_COLOR_YUV420:
            LOG(0, "YUV420 ");
            break;
        case VCONV_COLOR_YUV420SemiPlanar:
            LOG(0, "YUV420SemiPlanar ");
            break;
        default:
            LOG(0, "Unknown ");
            break;
    }
    LOG(0, "%dx%d, Stride(%d)\n", prInst->u4FrameWidth,
        prInst->u4FrameHeight, prInst->u4Stride);
    LOG(0, "[VConv] u1QFbCount(%d), u1QCmdCount(%d)\n", prInst->u1QFbCount, prInst->u1QCmdCount);
    LOG(0, "[VConv] [Fbg(%d), Fb(%d), Eos(%d)] fgSendEos(%d)\n",
            prInst->rSrc.ucFbgId, prInst->rSrc.ucFbId, prInst->rSrc.fgEos,
            prInst->fgSendEos);
    LOG(0, "[VConv] Event Wait for %d\n", prInst->eEventWait);
    LOG(0, "[VConv] Need alloc W %d, H %d, count %d, Need Free Fb Addr 0x%x, Change Fb Addr 0x%x\n",
        prInst->u4NeedAllocFbWidth, prInst->u4NeedAllocFbHeight, prInst->u4NeedAllocFbCount, prInst->u4NeedFreeFb, prInst->u4ChangeFb);

    for (u4Idx = 0; u4Idx < VCONV_MAX_FB_COUNT; u4Idx ++)
    {
        LOG(0, "[VConv] Have alloc Buf, W %d, H %d, Addr 0x%x\n",
            prInst->arFbInfo[u4Idx].u4FbW,
            prInst->arFbInfo[u4Idx].u4FbH,
            prInst->arFbInfo[u4Idx].u4FbAddr);
    }
    
    return (VCONV_OK);
}

/****************************************************************************/
/* add for thumbnails function. set thumbnail mode*/
/* set mode -> crate pipeline -> pause -> emit convert signal*/
/* muti-instance need test*/
/**************************************************************************/
#define OMX_THUMB_STATUS_LOAD (0)
#define OMX_THUMB_STATUS_Initiated (0x55)
struct ST_OMX_THUMB_STATIC_INFO
{
    atomic_t iIsThumbnailMode;
    UINT8 last_fbgid;
    UINT8 last_fbid;
    UINT8 ucStatus;
};
static struct ST_OMX_THUMB_STATIC_INFO stOmxThumbStatic[VDO_COMP_MAX];
#define OMX_THUMB_RENDER_CNT (1)
void vVDOOmxSetThumbnailMode(UINT8 ucOmxHandle, int mode)  
{
    int mode_old;

    memset(&(stOmxThumbStatic[ucOmxHandle]), 0, sizeof(struct ST_OMX_THUMB_STATIC_INFO));
    stOmxThumbStatic[ucOmxHandle].ucStatus = OMX_THUMB_STATUS_Initiated;

    mode_old = atomic_read(&stOmxThumbStatic[ucOmxHandle].iIsThumbnailMode);
    LOG(3, "[VOMX]vVDOOmxSetThumbnailMode: old_mode=%d, new_mode=%d\n", mode_old, mode);
    
    atomic_set(&stOmxThumbStatic[ucOmxHandle].iIsThumbnailMode, mode);
    if(mode&&_prVDOOmxHandle[ucOmxHandle].ucVdecId<VDEC_MAX_ES)
    {
		VDEC_SetRenderPicCount(_prVDOOmxHandle[ucOmxHandle].ucVdecId, mode, OMX_THUMB_RENDER_CNT);
    }
}
int vVDOOmxGetThumbnailMode(UINT8 ucOmxHandle) {return atomic_read(&stOmxThumbStatic[ucOmxHandle].iIsThumbnailMode) ;}

static void vVDOOmxSaveLastFb(UINT8 ucOmxHandle, UINT8 ucFbgid, UINT8 ucFbid)
{
    stOmxThumbStatic[ucOmxHandle].last_fbgid = ucFbgid;
    stOmxThumbStatic[ucOmxHandle].last_fbid = ucFbid;
}

/*return 0 if success, otherwise -1*/
INT32 i4VDOOmxCompParamSetThumbnailsMode(PARAMETER_OMX_PARAM_THUMBNAILS_MODE* prUArg)
{
    PARAMETER_OMX_PARAM_THUMBNAILS_MODE rKArg;
    OMX_VIDEO_THUMBNAIL_MODE rKMode;
    int mode;
    VDO_OMX_HANDLE_T *OmxHandle;

    if (copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_PARAM_THUMBNAILS_MODE))) return -1;
    if (copy_from_user(&rKMode, rKArg.pMode, sizeof(OMX_VIDEO_THUMBNAIL_MODE))) return -1;

    /*get vdec id from OMX*/
    OmxHandle = rKArg.pHandle;

    mode = (int)rKMode.bThumbnail;
    LOG(3, "i4VDOOmxCompParamSetThumbnailsMode vdec %d(mode %d)\n", OmxHandle->ucVdecId, mode);
   
    if(rKArg.eDir == OMX_PARAM_SET)
    {
        vVDOOmxSetThumbnailMode(OmxHandle->h_handle, mode);
    }
    else   
    { 
        mode = vVDOOmxGetThumbnailMode(OmxHandle->h_handle);
        rKMode.bThumbnail = (OMX_BOOL)mode;
    }

    rKArg.eReturnValue = OMX_ErrorNone;

    if (copy_to_user(rKArg.pMode, &rKMode, sizeof(OMX_VIDEO_THUMBNAIL_MODE))) return -1;
    if (copy_to_user(prUArg, (void *)&rKArg, sizeof(PARAMETER_OMX_PARAM_THUMBNAILS_MODE))) return -1;

    return 0;
}
/*Alex add for thumbnail function*/
#define THUMBNIAL_BACKUP_FB_ID (0)
#define THUMBNIAL_DEST_FB_ID (THUMBNIAL_BACKUP_FB_ID+1)
#define THUMBNIAL_DEST_MAX_SIZE (1920*1088*4)
#define THUMBNIAL_WORK_BUFFER_MAX_SIZE (1920*1088*4)

/*move y and c of frambuffer from fbid to THUMBNIAL_BACKUP_FB_ID*/
static void MoveSrc(UINT32 ucOmxHandle, UINT32 u4ByteSize)
{
    UINT32 u4SrcAddrY, u4SrcAddrC;
    UINT32 u4BkSrcAddrY, u4BkSrcAddrC;
    UINT8 ucLastFbg,ucLastFb;

    ucLastFbg = stOmxThumbStatic[ucOmxHandle].last_fbgid;
    ucLastFb = stOmxThumbStatic[ucOmxHandle].last_fbid;

    if (THUMBNIAL_BACKUP_FB_ID != ucLastFb)
    {
        FBM_GetFrameBufferAddr(ucLastFbg, ucLastFb, &u4SrcAddrY, &u4SrcAddrC);
        FBM_GetFrameBufferAddr(ucLastFbg, THUMBNIAL_BACKUP_FB_ID, &u4BkSrcAddrY, &u4BkSrcAddrC);
        LOG(1, "[VConv][MoveSrc] src y=%x, c = %x, bky=%x, bkc = %x\n", u4SrcAddrY, u4SrcAddrC, u4BkSrcAddrY, u4BkSrcAddrC);
        
     /*we need to flush cache before and after hw operation*/
#ifdef __KERNEL__    
        HalInvalidateDCacheMultipleLine(VIRTUAL(u4SrcAddrY), u4ByteSize);
        HalInvalidateDCacheMultipleLine(VIRTUAL(u4SrcAddrC), u4ByteSize >> 1);   
        HalInvalidateDCacheMultipleLine(VIRTUAL(u4BkSrcAddrY), u4ByteSize);   
        HalInvalidateDCacheMultipleLine(VIRTUAL(u4BkSrcAddrC), u4ByteSize >> 1);   
#else    
        HalFlushInvalidateDCache();
#endif 
        x_memcpy((void *)VIRTUAL(u4BkSrcAddrY), (void *)VIRTUAL(u4SrcAddrY), u4ByteSize);
        x_memcpy((void *)VIRTUAL(u4BkSrcAddrC), (void *)VIRTUAL(u4SrcAddrC), u4ByteSize >> 1);
#ifdef __KERNEL__    
        HalInvalidateDCacheMultipleLine(VIRTUAL(u4SrcAddrY), u4ByteSize);
        HalInvalidateDCacheMultipleLine(VIRTUAL(u4SrcAddrC), u4ByteSize >> 1);   
        HalFlushDCacheMultipleLine(VIRTUAL(u4BkSrcAddrY), u4ByteSize);   
        HalFlushDCacheMultipleLine(VIRTUAL(u4BkSrcAddrC), u4ByteSize >> 1);           
#else    
        HalFlushInvalidateDCache();
#endif

    }
}

/*return 0 if success, onterwise -1 */ 
static int vVConv_ThumbnailModeConvert(UINT8 ucOmxHandle, OMX_VIDEO_THUMBNAIL_DATA_TYPE* prKData)
{
    UINT32 u4WorkBufAddr;
    UINT32 u4SrcAddrY, u4SrcAddrC;
    UINT32 u4DstAddrY, u4DstAddrC;
    UINT32 u4ByteSize,u4BufferPitch;
    
    VDP_THUMBNAIL_INFO_T rCapDstInfo;
    VDP_CAPTURE_INTO_T rCapSrcInfo;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;
    FBM_PIC_HDR_T* prFbmPicHdr = NULL;
    UINT16 u2HSize, u2VSize, u2LineSize;
    UINT8 ucLastFb, ucLastFbg;

    ucLastFbg = stOmxThumbStatic[ucOmxHandle].last_fbgid;
    ucLastFb = stOmxThumbStatic[ucOmxHandle].last_fbid;

    /*check parameters*/
    LOG(3, "[VConv] format=%x, height=%x, width=%x, gid=%d, id=%d \n", prKData->eColorFormat, prKData->nFrameHeight, prKData->nFrameWidth, ucLastFbg, ucLastFb);
    if (OMX_COLOR_Format32bitARGB8888 != prKData->eColorFormat )//only support OMX_COLOR_Format32bitARGB8888 now
    {
        LOG(0, "[VConv] [ERR] format not support %d != %d\n", OMX_COLOR_Format32bitARGB8888, prKData->eColorFormat );
        //return -1;
    }
    u4BufferPitch=(prKData->nFrameWidth+0xf)&0xfffffff0;
    
    
    if (0 == vVDOOmxGetThumbnailMode(ucOmxHandle))//do only in thumbnail node
    {
        LOG(0, "[VConv] [ERR] Not in thumb mode\n");
        return -1;
    }
    if (stOmxThumbStatic[ucOmxHandle].ucStatus != OMX_THUMB_STATUS_Initiated)
    {
        LOG(0, "[VConv] [ERR] not initial\n");
        return -1;
    }
    if ((ucLastFbg == 0xff) 
        || (ucLastFb == 0xff))
    {
        LOG(0, "[VConv] [ERR] no frame buffer. gid=%d id=%d\n", ucLastFbg, ucLastFb);
        return -1;
    }

    /*prepare src info, move the src fb if necessery*/
    /*last_fbgid, last_fbid is set in vdec_done_cb()*/
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucLastFbg);
    if(!prFbmSeqHdr)
    {
        LOG(0, "[VConv] [ERR] prFbmSeqHdr null\n");
        return -1;
    }
    u2LineSize = prFbmSeqHdr->u2LineSize;
    u2HSize = prFbmSeqHdr->u2HSize;
    u2VSize = prFbmSeqHdr->u2VSize;
    if(prFbmSeqHdr->fgResizeSmallPic)
    {
        LOG(0, "[VConv] fgResizeSmallPic\n");
        prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucLastFbg, ucLastFb);
        if(!prFbmPicHdr)
        {
            LOG(0, "[VConv] [ERR] prFbmPicHdr null\n");
            return -1;
        }
        u2LineSize = prFbmPicHdr->u4PicWidthPitch;
        u2HSize = prFbmPicHdr->u4PicWidth;
        u2VSize = prFbmPicHdr->u4PicHeight;
    }
    u4ByteSize = (prFbmSeqHdr->u2LineSize) * (((prFbmSeqHdr->u2VSize)+0x1f)&(~0x1f));
    MoveSrc(ucLastFb, u4ByteSize);//move src data to THUMBNIAL_BACKUP_FB_ID
    LOG(0, "[VConv] src info: fbgid = %d, fbid=%d, YBufsize=%d, u2LineSize=%d, u2HSize=%d, u2VSize=%d\n", 
        ucLastFbg, ucLastFb, u4ByteSize, u2LineSize, u2HSize, u2VSize);

    
    FBM_GetFrameBufferAddr(ucLastFbg, THUMBNIAL_BACKUP_FB_ID, &u4SrcAddrY, &u4SrcAddrC);// get src address
    FBM_GetFrameBufferAddr(ucLastFbg, THUMBNIAL_DEST_FB_ID, &u4DstAddrY, &u4DstAddrC);// get dest buffer
    u4WorkBufAddr = u4DstAddrY + THUMBNIAL_DEST_MAX_SIZE;// get work buffer

    // prepare conversion data
    x_memset(&rCapSrcInfo, 0, sizeof(VDP_CAPTURE_INTO_T));
    x_memset(&rCapDstInfo, 0, sizeof(VDP_THUMBNAIL_INFO_T));

	if(prFbmSeqHdr->fgCropping&& (u2VSize>(prFbmSeqHdr->u4FrmCropTop+prFbmSeqHdr->u4FrmCropBtm)))
	{
        u4SrcAddrY+=prFbmSeqHdr->u4FrmCropTop*u2LineSize;
		u4SrcAddrC+=prFbmSeqHdr->u4FrmCropTop*u2LineSize;
		u2VSize-=prFbmSeqHdr->u4FrmCropTop;
		u2VSize-=prFbmSeqHdr->u4FrmCropBtm;
	}
	
    rCapSrcInfo.u4HSize = u2HSize;
    rCapSrcInfo.u4VSize = u2VSize;
    rCapSrcInfo.u4MemHSize = u2LineSize;
    rCapSrcInfo.u4MemVSize = u2VSize;
    rCapSrcInfo.u4AddrY = u4SrcAddrY;
    rCapSrcInfo.u4AddrC = u4SrcAddrC;
    rCapSrcInfo.u4RRMode = 0;
    //rCapSrcInfo.u4IsRsIn = 0;
    //rCapSrcInfo.u4SrcSwap = 6;
    /*for support MJPEG format*/
    if (prFbmSeqHdr->fgRasterOrder)
    {
        rCapSrcInfo.u4IsRsIn = 1;
        //rCapSrcInfo.u4OutSwap = 0;
        rCapSrcInfo.u4SrcSwap = 0;
    }
    else
    {
        rCapSrcInfo.u4IsRsIn = 0;
        //rCapSrcInfo.u4OutSwap = 6;
        rCapSrcInfo.u4SrcSwap = 6;
    }
    rCapDstInfo.u4ThumbnailWidth = prKData->nFrameWidth;//(((prKData->nFrameWidth+7)>>3)<<3);
    rCapDstInfo.u4ThumbnailHeight = prKData->nFrameHeight;
    rCapDstInfo.pucCanvasBuffer = (UCHAR*)u4DstAddrY;
    rCapDstInfo.eCanvasColormode = VDP_COLORMODE_ARGB_D8888;
    rCapDstInfo.u4CanvasWidth = (u4BufferPitch*4);
    rCapDstInfo.u4CanvasHeight = rCapDstInfo.u4ThumbnailHeight;
    rCapDstInfo.u4CanvasPitch = (u4BufferPitch*4);
    
    /*check the convert info*/
    LOG(0, "[VConv] u4SrcAddrY=0x%x, u4DstAddrY=0x%x, u4WorkBufAddr=0x%x\n", 
        u4SrcAddrY, u4DstAddrY, u4WorkBufAddr);
    
    /*we need to flush cache before and after hw operation*/
#ifdef __KERNEL__    
    HalInvalidateDCacheMultipleLine(VIRTUAL(u4DstAddrY), prKData->nFrameWidth * prKData->nFrameHeight * 4); 
    HalInvalidateDCacheMultipleLine(VIRTUAL(u4SrcAddrY), u2LineSize * u2VSize);
    HalInvalidateDCacheMultipleLine(VIRTUAL(u4SrcAddrC), u2LineSize * u2VSize >> 1);
    HalInvalidateDCacheMultipleLine(VIRTUAL(u4WorkBufAddr), u2LineSize * u2VSize * 4);
#else    
    HalFlushInvalidateDCache();
#endif
    //start to convert
    IMGRZ_SetDigest((void*)&rCapSrcInfo, (void*)&rCapDstInfo,
                    FALSE, u4WorkBufAddr);
    if (!GFX_ColorConvert((void*)&rCapDstInfo, u4WorkBufAddr,VdecVdpClr2GfxClr(rCapDstInfo.eCanvasColormode), 0xFF))
    {
        LOG(0, "[VConv] [ERR] GFX_ColorConvert fail\n");
        return -1;
    }
#ifdef __KERNEL__    
     HalInvalidateDCacheMultipleLine(VIRTUAL(u4DstAddrY), prKData->nFrameWidth * prKData->nFrameHeight * 4); 
     HalInvalidateDCacheMultipleLine(VIRTUAL(u4SrcAddrY), u2LineSize * u2VSize);
     HalInvalidateDCacheMultipleLine(VIRTUAL(u4SrcAddrC), u2LineSize * u2VSize >> 1);
     HalInvalidateDCacheMultipleLine(VIRTUAL(u4WorkBufAddr), u2LineSize * u2VSize * 4);
#else    
     HalFlushInvalidateDCache();
#endif

    //copy dest buffer to user buffer

    for(u2HSize=0;u2HSize<prKData->nFrameHeight;u2HSize++)
    {
        if (copy_to_user((prKData->buffer.pBuffer+u2HSize*prKData->nFrameWidth*4),(void *)VIRTUAL(u4DstAddrY+u2HSize*rCapDstInfo.u4CanvasPitch), 
        prKData->nFrameWidth* 4))goto __convert_copyfail;
    }
    //stOmxThumbStatic[ucVdecId].last_fbid = stOmxThumbStatic[ucVdecId].last_fbgid = -1;
    vVDOOmxSaveLastFb(ucOmxHandle, 0xff, 0xff);
    return 0;

__convert_copyfail:
    LOG(0, "[VConv] [ERR] __convert_copyfail \n");
    return -1;
}

/***************************************************************************/
/* add for thumbnails function. */
/* get thumbnail when playing*/
/* muti-instance need test*/
/**************************************************************************/
enum OMX_THUMB_STATUS
{
    OMX_THUMB_IDLE = 0,
    OMX_THUMB_WAIT,
    OMX_THUMB_OK,
    OMX_THUMB_FAIL,
    OMX_THUMB_MAX
};

struct ST_OMX_THUMB_INFO_t
{
    UINT32 u4OutputAddr;
    UINT32 u4OutputSize;
    UINT32 u4WorkingAddr;
    UINT32 u4WorkingSize;
    HANDLE_T _hSemOmxThumb;
    enum OMX_THUMB_STATUS eStatus;
    int i4InitialFlag;
};
static struct ST_OMX_THUMB_INFO_t _stThumbBufInfo[VDO_COMP_MAX];

#define OMX_MAX_OUTPUT_SIZE     (256 * 256 * 4)
#define OMX_WORK_BUF_SIZE       (256 * 256 * 4)
static BOOL i4VDOOmxThumbCb(UINT32 u4Tag, VOID* pvData);
static int GetThumbInit(UINT8 ucOmxHandle)
{
    FBM_POOL_T *prFbmPool;

    if (_stThumbBufInfo[ucOmxHandle].i4InitialFlag == 0x5a5a)
    {
        return 0;//initiated already
    }

    /* get hw buffer first */
    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PVR2);
    if(!prFbmPool)
    {
        LOG(0, "%s prFbmPool null\n",__FUNCTION__);
        ASSERT(0);
        return -1;
    }
    _stThumbBufInfo[ucOmxHandle].u4OutputSize = OMX_MAX_OUTPUT_SIZE;
    _stThumbBufInfo[ucOmxHandle].u4WorkingSize = OMX_WORK_BUF_SIZE;    
    _stThumbBufInfo[ucOmxHandle].u4OutputAddr = (prFbmPool->u4Addr);
    _stThumbBufInfo[ucOmxHandle].u4WorkingAddr = (prFbmPool->u4Addr+_stThumbBufInfo[ucOmxHandle].u4OutputSize);
    LOG(0, "omx thumbnail buf: %x(%x), %x(%x)\n",
        _stThumbBufInfo[ucOmxHandle].u4OutputAddr, _stThumbBufInfo[ucOmxHandle].u4WorkingSize, 
        _stThumbBufInfo[ucOmxHandle].u4WorkingAddr, _stThumbBufInfo[ucOmxHandle].u4WorkingSize);

    /*init semaphore, when do delete?*/
    if (x_sema_create(&(_stThumbBufInfo[ucOmxHandle]._hSemOmxThumb), X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK)
        != OSR_OK)
    {
        LOG(0, "%s create sema fail\n", __FUNCTION__);
        return -1;
    }
    _stThumbBufInfo[ucOmxHandle].eStatus = OMX_THUMB_IDLE;
    
    {
        // register cb function
        VDEC_PUSH_CB_T rPushCb;
        x_memset(&rPushCb, 0, sizeof(VDEC_PUSH_CB_T));
        
        rPushCb.u4Mask |= VDEC_PUSH_CB_THUMBNAIL;
        rPushCb.u4VdecThumbTag = (UINT32)(&_prVDOOmxHandle[ucOmxHandle]);
        rPushCb.pfnVdecThumbCb = i4VDOOmxThumbCb;

        if(!_VPUSH_RegCb(_prVDOOmxHandle[ucOmxHandle].pVdecPush, &rPushCb))
        {
            LOG(0, "_VPUSH registe thumbnail callback fail\n");
            ASSERT(0);
        }
    }
    
    _stThumbBufInfo[ucOmxHandle].i4InitialFlag = 0x5a5a;
    return 0;
}


static int _MTOMX_GetThumbnail(UINT8 ucOmxHandle, OMX_VIDEO_THUMBNAIL_DATA_TYPE* prKData)
{
    VDP_THUMBNAIL_INFO_T _rThumbDstInfo;
    UINT32 u4ThumbAlignWidth,u4LineIndex;
    INT32 iRet;

    /*check parameters*/
    LOG(0, "[VConv][_MTOMX_GetThumbnail] format=%x, height=%x, width=%x,status=%d\n", prKData->eColorFormat, prKData->nFrameHeight, prKData->nFrameWidth,prKData->nPlayState);
    if (OMX_COLOR_Format32bitARGB8888 != prKData->eColorFormat )//only support OMX_COLOR_Format32bitARGB8888 now
    {
        LOG(0, "[_MTOMX_GetThumbnail] [ERR] format not support %d != %d\n", OMX_COLOR_Format32bitARGB8888, prKData->eColorFormat );
        //return -1;
    }
    
    if((prKData->nFrameWidth == 0) || (prKData->nFrameHeight == 0))
    {
        //width should aligned by 16byte, pitch is equal to width
        LOG(0, "%s width/height error\n", __FUNCTION__);
        return -1;
    } 

    u4ThumbAlignWidth=(prKData->nFrameWidth+0xf)&0xfffffff0;
    if((u4ThumbAlignWidth * prKData->nFrameHeight * 4 > OMX_MAX_OUTPUT_SIZE))
    {
        //the output buffer and working buffer size have limitation.
        LOG(0, "%s out buffer size not enought, u4Width(%d,%d) u4Height(%d)\n", 
            __FUNCTION__, u4ThumbAlignWidth,prKData->nFrameWidth, prKData->nFrameHeight);
        return -1;
    }

    //get hw buffer
    if (GetThumbInit(ucOmxHandle)) 
    {
        LOG(0, " omx thumb hw init fail\n");
        return -1;
    }

    x_memset((VOID*)VIRTUAL(_stThumbBufInfo[ucOmxHandle].u4OutputAddr), 0, OMX_MAX_OUTPUT_SIZE);  //clear output buffer
    x_memset(&_rThumbDstInfo, 0, sizeof(VDP_THUMBNAIL_INFO_T));         //clear struct

    _rThumbDstInfo.eCanvasColormode = VDP_COLORMODE_ARGB_D8888;
    _rThumbDstInfo.pucCanvasBuffer = (UCHAR*)(_stThumbBufInfo[ucOmxHandle].u4OutputAddr);
    _rThumbDstInfo.u4CanvasHeight = prKData->nFrameHeight;
    _rThumbDstInfo.u4CanvasPitch = u4ThumbAlignWidth * 4;
    _rThumbDstInfo.u4CanvasWidth = u4ThumbAlignWidth * 4;
    _rThumbDstInfo.u4ThumbnailHeight = prKData->nFrameHeight;
    _rThumbDstInfo.u4ThumbnailWidth = prKData->nFrameWidth;
    _rThumbDstInfo.u4ThumbnailX = 0; // offset
    _rThumbDstInfo.u4ThumbnailY = 0;

    
    _stThumbBufInfo[ucOmxHandle].eStatus= OMX_THUMB_WAIT;
    if(prKData->nPlayState==3) // paused
    {
    
         LOG(2, " Start VDEC_GetVideoThumbnailDirect \n");
         if(FALSE==VDEC_GetVideoThumbnailDirect(_prVDOOmxHandle[ucOmxHandle].ucVdecId, &_rThumbDstInfo,
                _stThumbBufInfo[ucOmxHandle].u4WorkingAddr, _stThumbBufInfo[ucOmxHandle].u4WorkingSize, FALSE))
        {
             LOG(0, " VDEC_GetVideoThumbnailDirect fail \n");
        }
    }
    else
    {
    
        LOG(1, " Start VDEC_GetVideoThumbnail2 \n");
        if(FALSE == VDEC_GetVideoThumbnail2(_prVDOOmxHandle[ucOmxHandle].ucVdecId, &_rThumbDstInfo,
                _stThumbBufInfo[ucOmxHandle].u4WorkingAddr, _stThumbBufInfo[ucOmxHandle].u4WorkingSize, FALSE))
        {
            LOG(0, " VDEC_GetVideoThumbnail2 fail \n");
        }
    }


    //wait vdec get thumbnail
    iRet = x_sema_lock_timeout(_stThumbBufInfo[ucOmxHandle]._hSemOmxThumb, 5000);
    switch( iRet )
    {
        case OSR_TIMEOUT:
            LOG(0, "[VCONV]x_sema_lock_timeout timeout.\n");
            VDEC_StopVideoThumbnail2(_prVDOOmxHandle[ucOmxHandle].ucVdecId);
            _stThumbBufInfo[ucOmxHandle].eStatus = OMX_THUMB_IDLE;
            return -1;
        case OSR_OK:
            break;
        default:
            LOG(0, "[VCONV]x_sema_lock_timeout fail.\n");
            _stThumbBufInfo[ucOmxHandle].eStatus = OMX_THUMB_IDLE;
            return -1;
    }

    if (_stThumbBufInfo[ucOmxHandle].eStatus !=  OMX_THUMB_OK)
    {
        LOG(0, "[VCONV]Get thumbnail fail\n");
        _stThumbBufInfo[ucOmxHandle].eStatus = OMX_THUMB_IDLE;
        return -1;
    }
    _stThumbBufInfo[ucOmxHandle].eStatus = OMX_THUMB_IDLE;

    //copy dest buffer to user buffer
    for(u4LineIndex=0;u4LineIndex<prKData->nFrameHeight;u4LineIndex++)
    {
        if (copy_to_user(prKData->buffer.pBuffer+(u4LineIndex*prKData->nFrameWidth*4), (void *)VIRTUAL(_stThumbBufInfo[ucOmxHandle].u4OutputAddr+(u4LineIndex*_rThumbDstInfo.u4CanvasPitch)), prKData->nFrameWidth * 4))goto __omx_get_thumb_fail;
    }
    LOG(0, "[VConv] get thumbnail done\n");
    return 0;

__omx_get_thumb_fail:
    LOG(0, "[VConv] [ERR] _MTOMX_GetThumbnail \n");
    return -1;    

}

/*vdec cb function for thumbnail*/
static BOOL i4VDOOmxThumbCb(UINT32 u4Tag, VOID* pvData)
{
    VDO_OMX_HANDLE_T *pHandle = (VDO_OMX_HANDLE_T*)u4Tag;
    int i4ThumbResult = *(int *)pvData;
    
    LOG(0, "%s trace\n", __FUNCTION__);
    if (_stThumbBufInfo[pHandle->h_handle].eStatus == OMX_THUMB_WAIT)
    {
        if (i4ThumbResult == 0)//if success
        {
            LOG(0, " - Vdec thumbnail ok\n");
            _stThumbBufInfo[pHandle->h_handle].eStatus = OMX_THUMB_OK;
        }
        else
        {
            LOG(0, " - Vdec thumbnail error\n");
            _stThumbBufInfo[pHandle->h_handle].eStatus = OMX_THUMB_FAIL;
        }   
        VERIFY(x_sema_unlock(_stThumbBufInfo[pHandle->h_handle]._hSemOmxThumb) == OSR_OK);
    }

    return 0;

}

int vVConv_ThumbnailConvert(PARAMETER_OMX_PARAM_THUMBNAILS_CONVERT *prUArg)
{
    PARAMETER_OMX_PARAM_THUMBNAILS_CONVERT rKArg;
    VDO_OMX_HANDLE_T *OmxHandle;
    OMX_VIDEO_THUMBNAIL_DATA_TYPE rKData;
    int iRet;
 
    if (copy_from_user(&rKArg, prUArg, sizeof(PARAMETER_OMX_PARAM_THUMBNAILS_CONVERT))) goto __convert_copyfail;
    if (copy_from_user(&rKData, rKArg.pData, sizeof(OMX_VIDEO_THUMBNAIL_DATA_TYPE)))goto __convert_copyfail;

    rKArg.eReturnValue = OMX_ErrorNone;
    OmxHandle = rKArg.pHandle;

    if (vVDOOmxGetThumbnailMode(OmxHandle->h_handle))
    {
        LOG(0, "get thumbnail in thumbnail mode from vdec%d\n", OmxHandle->h_handle);
        iRet = vVConv_ThumbnailModeConvert(OmxHandle->h_handle, &rKData);
    }
    else
    {
        LOG(0, "get thumbnail in normal mode from vdec%d\n", OmxHandle->h_handle);
        iRet = _MTOMX_GetThumbnail(OmxHandle->h_handle, &rKData);
    }

    if (0 != iRet) goto __convert_copyfail;
    if (copy_to_user(rKArg.pData, &rKData, sizeof(OMX_VIDEO_THUMBNAIL_DATA_TYPE)))goto __convert_copyfail;
    if (copy_to_user(prUArg, &rKArg, sizeof(PARAMETER_OMX_PARAM_THUMBNAILS_CONVERT))) goto __convert_copyfail;
    return 0;

__convert_copyfail:
    LOG(0, "[VConv] [ERR] __convert_copyfail \n");
    rKArg.eReturnValue = OMX_ErrorUndefined;
    return -1;   
    
}
EXPORT_SYMBOL(_i4VConv_DrvInit);
EXPORT_SYMBOL(_i4VConv_Init);
EXPORT_SYMBOL(_i4VConv_GetHandle);
EXPORT_SYMBOL(_i4VConv_FreeHandle);
EXPORT_SYMBOL(_i4VConv_SetNotifyFct);
EXPORT_SYMBOL(_i4VConv_AllocFbmBuf);
EXPORT_SYMBOL(_i4VConv_FreeFbmBuf);
EXPORT_SYMBOL(_i4VConv_ChangeFbmBuf);
EXPORT_SYMBOL(_i4VConv_SendCmd);
EXPORT_SYMBOL(_i4VConv_Stop);
EXPORT_SYMBOL(_i4VConv_Finish);
EXPORT_SYMBOL(_i4VConv_Pause);
EXPORT_SYMBOL(_i4VConv_Resume);
EXPORT_SYMBOL(_i4VConv_GetParam);
EXPORT_SYMBOL(_i4VConv_SetVPush);
EXPORT_SYMBOL(_i4VConv_Flush);
EXPORT_SYMBOL(_i4VConv_PrintStatus);
#endif

