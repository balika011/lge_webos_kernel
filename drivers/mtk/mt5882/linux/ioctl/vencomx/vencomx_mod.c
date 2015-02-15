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

#include "vencomx_mod.h"

#include <linux/cb_data.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/mm.h>

#include <asm/uaccess.h>
#include <linux/proc_fs.h>

#define DEFINE_IS_LOG	VOMX_IsLog
#include "x_debug.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
#define USE_UNLOCK_IOCTL
#endif

//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------

static int vencomx_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int vencomx_release(struct inode *inode, struct file *file)
{
    return 0;
}

//-----------------------------------------------------------------------------
/** venc_ioctl.
 */
//-----------------------------------------------------------------------------

static int vencomx_ioctl(struct inode *inode,
                struct file *file, 
                unsigned int cmd,
                 unsigned long arg)
{
    int i4Ret = VENCR_OK;

    switch (cmd)
    {
    default:
#ifndef CC_SUPPORT_VENC
        LOG(0, "vencomx_ioctl: not define CC_SUPPORT_VENC!\n");
#endif 

        LOG(1, "Unknown cmd(%d)\n", cmd);
        i4Ret = VENCR_NOT_OK;
        break;
#ifdef CC_SUPPORT_VENC
    case IO_VENC_COPYDATA:
        i4Ret=_VENCOMX_CopyData(arg);
	    break;
    case IO_VENC_INIT:
        i4Ret = _VENCOMX_Init();
        break;
    case IO_VENC_TERMINT:
        i4Ret = _VENCOMX_Termint();
        break;
    case IO_VENC_RESET:
        i4Ret = _VENCOMX_Reset(arg);
        break;
    case IO_VENC_SET:
        i4Ret = _VENCOMX_Set(arg);
        break;
    case IO_VENC_GET:
        i4Ret = _VENCOMX_Get(arg);
        break;
    case IO_VENC_OPEN:
        i4Ret = _VENCOMX_Open(arg);
        break;
    case IO_VENC_SET_PARAM:
        i4Ret = _VENCOMX_SetParam(arg);
        break;
    case IO_VENC_SET_CTRL:
        i4Ret = _VENCOMX_SetCtrl(arg);
        break;
    case IO_VENC_DRV_INIT:
        i4Ret = _VENCOMX_DrvInit(arg);
        break;
    case IO_VENC_RUN:
        i4Ret = _VENCOMX_Run(arg);
        break;
    case IO_VENC_STOP:
        i4Ret = _VENCOMX_Stop(arg);
        break;
    case IO_VENC_CLOSE:
        i4Ret = _VENCOMX_Close(arg);
        break;
    case IO_VENC_ENC_SEQHDR_SYNC:
        i4Ret = _VENCOMX_EncSeqHdrSync(arg);
        break;    
    case IO_VENC_ENC_FRAME_SYNC:
        i4Ret = _VENCOMX_EncFrameSync(arg);
        break;
    case IO_VENC_ENC_FRAME_ASYNC:
        i4Ret = _VENCOMX_EncFrameAsync(arg);
        break;
    case IO_VENC_CREATE_BUFFMGR:
        i4Ret = _VENCOMX_CreateBuffMgr(arg);
        break;
    case IO_VENC_DELETE_BUFFMGR:
        i4Ret = _VENCOMX_DeleteBuffMgr(arg);
        break;
    case IO_VENC_BUFFMGR_OBJ_GET:
        i4Ret = _VENCOMX_BUFMGR_OBJ_Get(arg);
        break;
    case IO_VENC_BUFFMGR_OBJ_FREE:
        i4Ret = _VENCOMX_BUFMGR_OBJ_Free(arg);
        break;
    case IO_VENC_FLUSH_FB:
        i4Ret = _VENCOMX_FlushFb(arg);
        break;
    case IO_VENC_INSERT_KEYFRM:
        i4Ret = _VENCOMX_InsertKeyframe(arg);
        break;
    case IO_VENC_REG_CB:
        i4Ret = _VENCOMX_RegCb(arg);
        break;
    case IO_VENC_UNLOCK_BUF:
        i4Ret = _VENCOMX_UnlockBuf(arg);
        break;
#endif // CC_SUPPORT_VENC
    }

    return i4Ret;
}


#ifdef USE_UNLOCK_IOCTL
long vencomx_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long param)
{
    return vencomx_ioctl(0, file, cmd, param);
}
#endif

struct file_operations vencomx_fops = {
    .owner              = THIS_MODULE,
    .open               = vencomx_open,
#ifdef USE_UNLOCK_IOCTL
    .unlocked_ioctl     = vencomx_unlocked_ioctl,
#else
    .ioctl              = vencomx_ioctl,
#endif
	//.mmap               = fbmomx_mmap,
    .release            = vencomx_release,
};

#endif


