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
#include "x_debug.h"
#include "x_typedef.h"
#include "x_os.h"
#include "x_printf.h"
#include "x_assert.h"

#include "vdpomx_mod.h"

#include <linux/cb_data.h>
#include <linux/version.h>

#include <asm/uaccess.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
#define USE_UNLOCK_IOCTL
#endif

//#define VDPOMX_DEBUG
#define FBM_DBG_FLAG 1
#define VDP_DBG_FLAG 2

#ifdef VDPOMX_DEBUG
    unsigned int vdp_dbg;  
    #define VDPOMX_DBG(fmt...)    if(vdp_dbg & VDP_DBG_FLAG) {printk("%s:","[vdpomx_mod]");  printk(fmt);  }
#else
    #define VDPOMX_DBG(fmt...)
#endif

//extern INT32 _CB_PutEvent(CB_FCT_ID_T eFctId, INT32 i4TagSize, void *pvTag);

//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------

//static int _u4VdpOmxOpenCnt = 0;


extern INT32 _CB_PutEvent(CB_FCT_ID_T eFctId, INT32 i4TagSize, void *pvTag);

static int vdpomx_open(struct inode *inode, struct file *flip)
{
 //   VDPOMX_DBG("vdpomx_open\n");
#if 0
    if (0 == _u4VdpOmxOpenCnt)
    {
    }
    
    _u4VdpOmxOpenCnt ++;
#endif
    return 0;
}


static int vdpomx_release(struct inode *inode, struct file *flip)
{
    //(xxx *)flip->private_data;
//    VDPOMX_DBG("vdpomx_release\n");
#if 0
    _u4VdpOmxOpenCnt --;

    if (0 == _u4VdpOmxOpenCnt)
    {
    }
#endif    

    return 0;
}

//-----------------------------------------------------------------------------
/** vdec_ioctl.
 */
//-----------------------------------------------------------------------------
static int vdpomx_ioctl(struct inode *inode,
                struct file *file, 
                unsigned int cmd,
                 unsigned long arg)
{
    INT32  i4Ret = 0;
    //VDPOMX_DBG("vdpomx_ioctl\n");
    switch (cmd)
    {
        case VDPOMX_SET_OUT_REGION :
             i4Ret =   vdp_set_out_region((VDPOMX_SET_REGION_PARM_T*)arg); 
            VDPOMX_DBG("set out region\n");   
            break;
        case VDPOMX_SET_SRC_REGION :
             i4Ret =   vdp_set_src_region((VDPOMX_SET_REGION_PARM_T*)arg); 
            VDPOMX_DBG("set src region\n");   
            break;      
        case VDPOMX_SET_OUT_CHANNEL :
            i4Ret =   vdp_set_out_channel((VDPOMX_SET_CHANNEL_PARM_T*)arg); 
            VDPOMX_DBG("out channel\n");   
            break;         
        case VDPOMX_SET_DISPLAY_BUF :
            i4Ret =   vdp_set_display_buf((VDPOMX_SET_DISPLAY_BUF_PARM_T*)arg);
            VDPOMX_DBG("set display buffer\n");   
            break;        
        case VDPOMX_GET_CHANNEL_FREE :
            i4Ret =   vdp_get_channel_free((VDPOMX_GET_CHANNEL_FREE_PARM_T*)arg);
            VDPOMX_DBG("get channel free\n");   
            break;                 
        case VDPOMX_GET_BUF_LOCKED :
            i4Ret =   vdp_get_buf_locked((VDPOMX_GET_BUF_LOCKED_PARM_T*)arg);
            VDPOMX_DBG("get buffer locked\n");   
            break;
        //case VDPOMX_TUNNEL_CONNECT_AUDIO :
        //    i4Ret =   vdp_tunnel_connect_audio((VDPOMX_TUNNEL_CONNECT_AUD_PARM_T*)arg);
        //    VDPOMX_DBG("tunnel connnect audio\n");   
        //    break;
        //case VDPOMX_CONNECT_VDEC:
        //    i4Ret =   vdp_connect_vdec((VDPOMX_CONNECT_VDEC_PARM_T*)arg);
        //    VDPOMX_DBG("connnect vdec\n");   
        //    break;
        case VDPOMX_SRM_SETTVMODE :
            i4Ret =   vdp_srm_settvmode(arg);
            VDPOMX_DBG("SRM_SetTvMode arg %d\n", arg);
            break;
        default:
            break;
    }

    return i4Ret;    
}

#ifdef USE_UNLOCK_IOCTL
long vdpomx_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long param)
{
    return vdpomx_ioctl(0, file, cmd, param);
}
#endif

struct file_operations vdpomx_fops = {
    .owner              = THIS_MODULE,
    .open               = vdpomx_open,
#ifdef USE_UNLOCK_IOCTL
    .unlocked_ioctl     = vdpomx_unlocked_ioctl,
#else
    .ioctl              = vdpomx_ioctl,
#endif
    .release            = vdpomx_release,
};


#endif
//#if defined(LINUX_TURNKEY_SOLUTION) && defined(CC_ENABLE_VDECOMX)


