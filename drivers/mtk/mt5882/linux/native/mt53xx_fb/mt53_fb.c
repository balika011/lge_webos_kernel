/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
/*
 * Hardware specific code is based on MT53xx driver
 * Framebuffer framework code is based on documents on net
 *
 */

//---------------------------------------------------------------------------
// Header files
//---------------------------------------------------------------------------
// #define CC_SUPPORT_MULTIPLE_FBDEV

// Remove this force define after Zijie port back
#ifndef DELAY_LOGO_DISAPPEAR_TIME
#define DELAY_LOGO_DISAPPEAR_TIME
#endif

#define FB_GFX_PF_DLTA   0 //(GFX_Get_DbgLvl()&0x20000)

#ifdef CC_GPU_SUPPORT
#define CC_SUPPORT_MULTIPLE_FBDEV
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
#include <linux/platform_device.h>
#else
#include <linux/platform.h>
#endif
#include <asm/cacheflush.h>
//#include <asm/io.h>
// #include <asm/irq.h>
//#include <asm/pgtable.h>
//#include <asm/system.h>
//#include <asm/uaccess.h>

#ifdef CONFIG_MTRR
#include <asm/mtrr.h>
#endif

//#include "osd_if.h"
#include "osd_drvif.h"
#include "osd_hw.h"
#include "gfx_if.h"
#include "pmx_if.h"
#include "pmx_drvif.h"
#include "fbm_drvif.h"
#include "u_handle.h"
#include "mt53_accel.h"
#ifdef CC_DFB_SUPPORT_VDP_LAYER
#include "vdo_if.h"
#endif
#include "imgrz_if.h"
#include "imgrz_drvif.h"

#include "extmjc_if.h"
#include "x_linux.h"
#include "x_debug.h"
#include "drv_display.h"
#ifdef LINUX_TURNKEY_SOLUTION
#include "x_util.h"
#endif

#include "b2r_drvif.h"

#ifdef LINUX_TURNKEY_SOLUTION
#include "vdp_if.h"
#endif

#include "u_drv_cust.h"

#ifdef CC_DFB_SUPPORT_VDP_LAYER
#include "drv_tdtv_drvif.h"
#endif

#if defined(ANDROID) || defined(ANDROID_TWO_WORLDS)
#include "mt53_simple.h"
#endif

extern void fb_osd_init(void);
extern void fb_osd_wait_vsync(void);
extern void fb_osd_wait_stable_recovery(void);

/******************************************************************************************************************************
***
*** nonstandard defines
***
*******************************************************************************************************************************/

#ifdef CC_SUPPORT_DYNAMIC_ENABLE_DFB_INPUTEVENT
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/init.h>
    
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

extern UINT32 GFX_Get_DbgLvl(void);


#define MTK_USER_KEY           \
    char ucToken[64];          \
                               \
    unsigned int u4IsKey;      \
                               \
    unsigned int u4KeySymbol;  \
                               \
    unsigned int u4IsKeyPress; \


#define MTK_ROUTE          0
#define MTK_DISCARD        1

#define MTKIO_SET_DFBDISCARDKEY       _IOW('M', 0x41, UINT32)
#define MTKIO_CHECK_DFBDISCARDKEY     _IOW('M', 0x42, UINT32)
#define MTKIO_DUMP_DFBKEY             _IO('M', 0x43)


typedef struct
{
    MTK_USER_KEY
}MTK_USERKEY_T;


typedef struct
{
    MTK_USER_KEY
        
    struct list_head list;
    
}MTK_KEY_NODE_T;


typedef struct
{
    UINT32 u4IsDiscard;
    
    struct list_head tList;
    
    struct semaphore tSem;
}MTK_KEY_INFO_T;


static MTK_KEY_INFO_T _gtDFBKeyInfo;


static VOID dfb_init(VOID)
{
    _gtDFBKeyInfo.u4IsDiscard = 1;
    
    INIT_LIST_HEAD(&_gtDFBKeyInfo.tList);
    
    sema_init(&_gtDFBKeyInfo.tSem, 1);
}


static MTK_KEY_NODE_T *dfbkey_allocnode(VOID)
{
    return kmalloc(sizeof(MTK_KEY_NODE_T), GFP_KERNEL);
}


static VOID dfbkey_freenode(MTK_KEY_NODE_T *ptNode)
{
    kfree(ptNode);
}


static int dfbkey_check(MTK_USERKEY_T __user *ptUserKey)
{
    MTK_USERKEY_T tKey;
    MTK_KEY_NODE_T *ptNode;
    struct list_head *l;

    ASSERT((ptUserKey != NULL));

    if(copy_from_user(&tKey, ptUserKey, sizeof(MTK_USERKEY_T))){
        printk(KERN_WARNING "Misc_DFBKey_Add() - copy_from_user() fail\n");
        return -EFAULT;
    }

    down(&_gtDFBKeyInfo.tSem);

    if(tKey.u4IsKey == 0)
    {
        if(_gtDFBKeyInfo.u4IsDiscard)
        {
            up(&_gtDFBKeyInfo.tSem);
            
            return MTK_DISCARD;
        }        
    }
    else if(tKey.u4IsKeyPress)
    {
        if(_gtDFBKeyInfo.u4IsDiscard)
        {
            up(&_gtDFBKeyInfo.tSem);
            
            return MTK_DISCARD;
        }
        else
        {
            ptNode = dfbkey_allocnode();
         
            ASSERT((ptNode != NULL));

            strncpy(ptNode->ucToken, tKey.ucToken, sizeof(ptNode->ucToken));

            ptNode->u4KeySymbol = tKey.u4KeySymbol;

            list_add_tail(&(ptNode->list), &_gtDFBKeyInfo.tList);           
        }
    }
    else
    {
        BOOL isKeyPressExist = FALSE;
        
        list_for_each(l, &_gtDFBKeyInfo.tList)
        {
            ptNode = list_entry(l, MTK_KEY_NODE_T, list);

            if(0 == strncmp(ptNode->ucToken, tKey.ucToken, sizeof(ptNode->ucToken)) && ptNode->u4KeySymbol == tKey.u4KeySymbol)
            {           
                list_del(l);

                dfbkey_freenode(ptNode);

                isKeyPressExist = TRUE;
                
                break;
            }
        }

        if(isKeyPressExist == FALSE && _gtDFBKeyInfo.u4IsDiscard)
        {
            up(&_gtDFBKeyInfo.tSem);
            
            return MTK_DISCARD;     
        }
    }

    up(&_gtDFBKeyInfo.tSem);

    return MTK_ROUTE;
    
}
#endif
/******************************************************************************************************************************
***
***nonstandard defines
***
*******************************************************************************************************************************/



typedef enum
{
    E_FB_OSD_RGN_LEFT,
    E_FB_OSD_RGN_RIGHT,
    E_FB_OSD_RGN_MAX
} E_FB_OSD_RGN_TYPE;



#define CHECK_FB_OSD_RGN_LIST_PARA(u4_plane,u4_rgn,pt_rgn_list)  \
{                                                           \
    if(u4_plane>=OSD_PLANE_MAX_NUM)                        \
    {                                                       \
        return OSD_RET_INV_ARG;                            \
    }                                                       \
    if(u4_rgn>=E_FB_OSD_RGN_MAX)                           \
    {                                                       \
        return OSD_RET_INV_ARG;                            \
    }                                                       \
    if(NULL == pt_rgn_list)                               \
    {                                                       \
        return OSD_RET_INV_ARG;                            \
    }                                                       \
}


#define CHECK_FB_RET(RET)             \
{                                       \
    if(OSD_RET_OK != RET)              \
    {                                   \
        return OSD_RET_INV_ARG;        \
    }                                   \
}


#define FB_OSD_SET_RGN(cmd_rgn,val)                                      \
{                                                                        \
    VERIFY(OSD_RGN_Set(t_left.u4_rgn, cmd_rgn,val) == OSD_RET_OK);      \
    VERIFY(OSD_RGN_Set(t_rght.u4_rgn, cmd_rgn,val) == OSD_RET_OK);      \
}

#define FB_OSD_SET_RGN_FLIP_MIRROR(u4_plane)                                    \
{                                                                               \
    if(OSD_Base_Is_Osd_Do_Flip_Mirror(u4_plane))                               \
    {                                                                           \
        FB_OSD_SET_RGN(OSD_RGN_V_FLIP,OSD_GetCpblty(E_OSD_CPBLTY_FLIP, 0x0));   \
        FB_OSD_SET_RGN(OSD_RGN_H_MIRROR,OSD_GetCpblty(E_OSD_CPBLTY_MIRROR,0x0));\
    }                                                                           \
    else                                                                       \
    {                                                                           \
        FB_OSD_SET_RGN(OSD_RGN_V_FLIP,0x0);                                     \
        FB_OSD_SET_RGN(OSD_RGN_H_MIRROR,0x0);                                   \
    }                                                                           \
}

#define FB_OSD_IS_SET_BMP_ADDR_NORMAL(pt_left,pt_rght)                          \
       (FB_OSD_IS_DISP_STATE(pt_left,pt_rght)||                               \
        FB_OSD_IS_IDLE_INIT_STATE(pt_left,pt_rght)||                          \
        FB_OSD_IS_UNCMP_INIT_STATE(pt_left,pt_rght))

#define FB_OSD_IS_SET_BMP_ADDR_CMP(pt_left,pt_rght)                             \
       ( FB_OSD_IS_CMP_DISP_STATE(pt_left,pt_rght)||                          \
         FB_OSD_IS_CMP_STATE(pt_left,pt_rght))


#define FB_RGN_IS_DISPING_STATE(V) (FB_RGN_IS_DISP_STATE(V)||FB_RGN_IS_CMP_DISP_STATE(V))

#define FB_OSD_RGN_OBJ_TMPL(NAME)                                               \
void FB_RGN_SET_##NAME##_STATE(FB_OSD_RGN_T* pt_this)                          \
{                                                                               \
    pt_this->u4_state = (OSD_RGN_STATE_##NAME);                                \
                                                                                \
    return;                                                                    \
}                                                                               \
int FB_RGN_IS_##NAME##_STATE(FB_OSD_RGN_T* pt_this)                            \
{                                                                               \
    return ((OSD_RGN_STATE_##NAME) == pt_this->u4_state );                     \
}                                                                               \
void FB_OSD_SET_##NAME##_STATE(FB_OSD_RGN_T* pt_left,FB_OSD_RGN_T* pt_right)   \
{                                                                               \
    FB_RGN_SET_##NAME##_STATE(pt_left);                                        \
    FB_RGN_SET_##NAME##_STATE(pt_right);                                       \
    return ;                                                                   \
}                                                                               \
int FB_OSD_IS_##NAME##_STATE(FB_OSD_RGN_T* pt_left,FB_OSD_RGN_T* pt_right)     \
{                                                                               \
    if(FB_RGN_IS_##NAME##_STATE(pt_left)||                                     \
        FB_RGN_IS_##NAME##_STATE(pt_right))                                    \
    {                                                                           \
        return TRUE;                                                           \
    }                                                                           \
    return FALSE;                                                              \
}                                                                               \
int FB_OSD_IS_##NAME##_INIT_STATE(FB_OSD_RGN_T* pt_left,FB_OSD_RGN_T* pt_right)\
{                                                                               \
    if(FB_RGN_IS_##NAME##_STATE(pt_left)&&                                     \
       FB_RGN_IS_##NAME##_STATE(pt_right))                                     \
    {                                                                           \
        return TRUE;                                                           \
    }                                                                           \
    return FALSE;                                                              \
}                                                                               \



#ifdef CC_SUPPORT_MULTIPLE_FBDEV
#define IS_PANEL_PDP 0
#endif

//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

#define PITCH_ALIGNMENT_64    (0)

#define FB_DRIVER_IMPROVE

//#define FB_DRIVER_MATCH_PANEL_IMPROVE
//#define FB_SUPPORT_ARGB_ORDER
//#define POSD1_FHD



typedef struct _FB_OSD_RGN_T
{
    UINT32 u4_rgn;
    UINT32 u4_list;
    UINT32 u4_addr;
    UINT32 u4_3d_addr;
    UINT32 u4_state;
}FB_OSD_RGN_T;

typedef enum _E_FB_OSD_RGN_LIST
{
    E_FB_OSD_RGN,
    E_FB_OSD_LIST,
    E_FB_OSD_RGN_LIST_MAX
}E_FB_OSD_RGN_LIST;


#ifdef CC_SUPPORT_MULTIPLE_FBDEV
typedef struct fbdev_ipc
{
    unsigned int pid;
    unsigned short x;
    unsigned short y;
    unsigned short w;
    unsigned short h;
    unsigned int curr_back_buf;
    unsigned int src_buf;
    unsigned int src_pitch;
    unsigned char src_fmt_argb8888;
}fbdev_ipc;
#endif /* CC_SUPPORT_MULTIPLE_FBDEV */

#define HAS_DEV_FB1 (1)
#define HAS_DEV_FB2 (1)

#define ANDROID_FB_CM_ARGB8888

//#define ANDROID
#if defined(ANDROID) || defined(CC_SUPPORT_MULTIPLE_FBDEV) || defined(CC_LINUX_GPU_SUPPORT)
#include "mtkfb.h"
#endif
#define TWO_OSD_PLANE

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
#define OSD_PLANE1 OSD_PLANE_2
#define OSD_PLANE2 OSD_PLANE_3
#define OSD_SCALER1 OSD_SCALER_2
#define OSD_SCALER2 OSD_SCALER_3
#else
#define OSD_PLANE1 OSD_PLANE_1
#define OSD_PLANE2 OSD_PLANE_2
#define OSD_SCALER1 OSD_SCALER_2
#define OSD_SCALER2 OSD_SCALER_2
#endif

#define MT53XX_FB_VERSION "0.1"
#define PALETTE_SIZE 1024
#define TEXT_SIZE 1024

//#define KERNEL_DRAM_SIZE (num_physpages << PAGE_SHIFT)

// Define this option if you want OSD scaling to full screen
#define OSD_PLANE1_SCALE_TO_FULLSCREEN
#define OSD_PLANE2_SCALE_TO_FULLSCREEN

#define SIZE_PLANE_ORDER_ARRAY 4
#define MIXER_PORT_VIDEO_MAIN 0
#define MIXER_PORT_VIDEO_PIP  1
#define MIXER_PORT_OSD_1 2
#define MIXER_PORT_OSD_2 3
#define MIXER_PORT_NONE 0xFF

#ifdef LINUX_TURNKEY_SOLUTION
/* MHF Linux: CHUN */
#define OSD_BUFFER_SIZE (4 * 1024 * 1024)
#define CC_SAMSUNG_IMAGE_BUFFER_SIZE ((u32)(0xE00000 + 0x400000))

#define FB_HARD_FADING
#endif

#define TMPBUF_WIDTH  1920
#define TMPBUF_HEIGHT 1080
#define TMPBUF_BPP    4

#ifdef CC_TDTV_3D_OSD_ENABLE
extern BOOL gfgOsd_Sbs;
extern BOOL gfgOsd_Tbb;
extern UINT8 _u13DOsdShift;
#endif

#ifdef CONFIG_ANDROID_PMEM
extern void mtk_pmem_set_memory_regions(u32 start, u32 size);
#endif

#ifdef CC_SUPPORT_MULTIPLE_FBDEV
#if 1 // TODO: Required Zijie to port back to use extern variable
extern UINT32 _fgOsdEnabelUpdate;
#else
UINT32 _fgOsdEnabelUpdate = 1;
#endif

#ifdef OSD_DSC_PDP
UINT32 fb1_rgn_pdp;
UINT32 fb1_rgn_pdp_list;
UINT32 fb1_rgn_pdp_2;
UINT32 fb1_rgn_pdp_list_2;
UINT32 u4PDPFBBuffer1=0;
UINT32 u4PDPFBBuffer2=0;
static UINT32 u4SrcPdpWidth = 1280;
static UINT32 u4SrcPdpHeight = 720;
UINT32 fb2_rgn_pdp;
UINT32 fb2_rgn_pdp_list;
UINT32 fb2_rgn_pdp_2;
UINT32 fb2_rgn_pdp_list_2;
UINT32 u4FB2_PDPFBBuffer1=0;
UINT32 u4FB2_PDPFBBuffer2=0;
static UINT32 u4FB2_SrcPdpWidth = 1280;
static UINT32 u4FB2_SrcPdpHeight = 720;
#define UI_SRC_WIDTH    1280
#define UI_SRC_HEIGHT    720
#define UI_DST_WIDTH    1024
#define UI_DST_HEIGHT    768
#endif /* OSD_DSC_PDP */
#endif /* CC_SUPPORT_MULTIPLE_FBDEV */
#ifdef CC_SUPPORT_4K2K_UI
BOOL fg4k2kOsdSetting = 0; 
#endif
static BOOL _scaler_status[OSD_SCALER_MAX_NUM] = {TRUE, TRUE, TRUE};

#if defined(CC_MTK_ION_SUPPORT)
UINT32 u4CarveoutAddr = 0;//mtk70854    
UINT32 u4CarveoutSize = 0;//mtk70854
#define GPU_ION_MEM_SIZE (3*4*3840*2160)   // for hw layer usage
#endif
//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------
#ifndef LINUX_TURNKEY_SOLUTION
static int _mt53fb_soft_cursor(struct fb_info *info, struct fb_cursor *cursor);
#endif

static int _mt53fb_setcolreg(u_int regno, u_int red, u_int green, u_int blue,
        u_int trans, struct fb_info *info);

static int _mt53fb_check_var(struct fb_var_screeninfo *var,
        struct fb_info *info);

static int _mt53fb_set_par(struct fb_info *info);

static int _mt53fb_open(struct fb_info *info, int user);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
static int _mt53fb_mmap(struct fb_info *info, struct vm_area_struct *vma);

static int _mt53fb_ioctl(struct fb_info *info, unsigned int cmd,
        unsigned long arg);
#else
static int _mt53fb_mmap(struct fb_info *info, struct file *file, struct vm_area_struct *vma);

static int _mt53fb_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg, struct fb_info *info);
#endif

static void __init _mt53fb_parse_colormode(char *opt);

static void __init _mt53fb_parse_dram(char *opt);

static void __init _mt53fb_init_fbinfo(void);


#if !defined(ANDROID) || defined(ANDROID_TWO_WORLDS)
static int __init _mt53fb_probe(struct device *dev);
#endif


static int _mt53fb_set_ioctl(void* pvArg);
#ifdef CC_SUPPORT_HW_CURSOR
INT32 _mt53fb_hwcursor_setting(void *pvArg);
#endif

static int _mt53fb_init_hw(void* pvArg);

//static int _RecalculateOSD(void);

static int _mt53fb_palette_ioctl(void* pvArg);

static int _mt53fb_drawtext_ioctl(void* pvArg);

static int _mt53fb_get_imagebuffer_ioctl(void* pvArg);

#ifdef CC_FBM_TWO_FBP_SHARED_WITH_DFB
static int _mt53fb_get_imagebuffer2_ioctl(void* pvArg);
#endif
#ifdef CC_DFB_SUPPORT_VDP_LAYER
extern UINT32 VDP_SetInput(UCHAR ucVdpId, UCHAR ucEsId, UCHAR ucPort);

static int _mt53fb_vdp_set_ioctl(void* pvArg);
static int _mt53fb_get_vdpbuffer_ioctl(void* pvArg);
static int _mt53fb_get_vdp_crop_rect_ioctl(void* pvArg);
static int _mt53fb_set_vdp_crop_rect_ioctl(void* pvArg);
static INT32 _mt53fb_vdp_stretch_blit_ioctl(void* pvArg);
#endif

#ifdef CC_B2R44K2K_SUPPORT
static INT32 _mt53fb_b2r_stretch_blit_ioctl(void* pvArg);
#endif

static int _mt53fb_get_plane_order_array_ioctl(void* pvArg);

static int _mt53fb_set_plane_order_array_ioctl(void* pvArg);

static int _mt53fb_set_osd_plane_order_ioctl(void* pvArg);

static int _mt53fb_get_ioctl(void* pvArg);

//#ifdef LINUX_TURNKEY_SOLUTION /* MHF Linux - yjg 9x mmp */
unsigned int fb_convert_app_to_phy(unsigned int usr_addr);
//#endif

static int _mt53fb_get_tmp_buf_ioctl(void* pvArg);

int _mt53fb_wait_logo(void* pvArg);

int __init _mt53fb_setup(char *options);


extern INT32 mt53fb_drawline_ioctl(void * pvArg);

extern INT32 mt53fb_drawlinerop_ioctl(void * pvArg);

extern INT32 mt53fb_drawrect_ioctl(void * pvArg);

extern INT32 mt53fb_drawrectrop_ioctl(void * pvArg);

extern INT32 mt53fb_fillrect_ioctl(void * pvArg);

extern INT32 mt53fb_copyarea_ioctl(void * pvArg);

extern INT32 mt53fb_imageblit_ioctl(void * pvArg);

extern INT32 mt53fb_imageblitrop_ioctl(void * pvArg);

extern INT32 mt53fb_fillrectrop_ioctl(void * pvArg);

extern INT32 mt53fb_copyarearop_ioctl(void * pvArg);

extern INT32 _mt53fb_gfx_stretch_ioctl(void * pvArg);

extern INT32 _mt53fb_imgrz_stretch_alcom_ioctl(void * pvArg);
extern INT32 _mt53fb_vgfx_scaler_ioctl(void* pvArg);
extern INT32 _mt53fb_vgfx_ioctl(void *pvArg);

extern INT32 mt53fb_drawtext(struct fb_info *info,
        const struct mt53fb_DrawText *prDrawText);

extern INT32 mt53fb_HalFlushInvalidateDCache_ioctl(void* pvArg);
#if defined(CC_ENABLE_GRAPHIC_DUMP)
extern VOID GFX_AnalyOSD(UINT32 u4Plane);
#endif
//---------------------------------------------------------------------------
extern INT32 PDWNC_Init(void);
extern void VDEC_Init(void);
extern BOOL VdoMLInit(void);
extern void SIF_Init(void);
extern void RTC_Init(void);
extern void vApiPanelPowerSequence(BOOL fgEnable);
extern INT32 GPIO_Init(void);
extern INT16 EEPROM_Init( VOID);

#if !defined(ANDROID) || defined(ANDROID_TWO_WORLDS)
static void fb_osd_palette_init(void);
#endif


static unsigned int * fb_osd_get_palette(UINT32 u4_plane);
static unsigned int * fb_osd_get_palette_virt(UINT32 u4_plane);

static INT32 fb_osd_set_rgn_obj(UINT32 u4_plane,UINT32 u4_rgn,FB_OSD_RGN_T* pt_rgn_list);
static INT32 fb_osd_get_rgn_obj(UINT32 u4_plane,UINT32 u4_rgn,FB_OSD_RGN_T* pt_rgn_list);
static void  fb_osd_rgn_obj_init(void);
static UINT32 fb_osd_get_plane(UINT32 ui4_dfb_layer_id);
static INT32 fb_set_plane_enable(UINT32 u4_enable,UINT32 u4_plane,UINT32 u4_rgn_list);
static UINT32 fb_osd_get_idle_rgn_list(UINT32 u4_plane);
static INT32 fb_osd_set_bmp_addr(struct mt53fb_set* pt_set,UINT32 u4_plane);
static INT32 fb_gfx_compress(struct mt53fb_set* pt_set,FB_OSD_RGN_T*  pt_rgn);


void FB_RGN_SET_IDLE_STATE(FB_OSD_RGN_T* pt_this);
void FB_RGN_SET_DISP_STATE(FB_OSD_RGN_T* pt_this);
void FB_RGN_SET_CMP_DISP_STATE(FB_OSD_RGN_T* pt_this);
void FB_RGN_SET_CMP_STATE(FB_OSD_RGN_T* pt_this);
void FB_RGN_SET_UNCMP_STATE(FB_OSD_RGN_T* pt_this);
int FB_RGN_IS_IDLE_STATE(FB_OSD_RGN_T* pt_this);
int FB_RGN_IS_DISP_STATE(FB_OSD_RGN_T* pt_this);
int FB_RGN_IS_CMP_DISP_STATE(FB_OSD_RGN_T* pt_this);
int FB_RGN_IS_CMP_STATE(FB_OSD_RGN_T* pt_this);
int FB_RGN_IS_UNCMP_STATE(FB_OSD_RGN_T* pt_this);
void FB_OSD_SET_IDLE_STATE(FB_OSD_RGN_T* pt_left,FB_OSD_RGN_T* pt_right);
int FB_OSD_IS_IDLE_STATE(FB_OSD_RGN_T* pt_left,FB_OSD_RGN_T* pt_right);
int FB_OSD_IS_IDLE_INIT_STATE(FB_OSD_RGN_T* pt_left,FB_OSD_RGN_T* pt_right);
void FB_OSD_SET_DISP_STATE(FB_OSD_RGN_T* pt_left,FB_OSD_RGN_T* pt_right);
int FB_OSD_IS_DISP_STATE(FB_OSD_RGN_T* pt_left,FB_OSD_RGN_T* pt_right);
int FB_OSD_IS_DISP_INIT_STATE(FB_OSD_RGN_T* pt_left,FB_OSD_RGN_T* pt_right);
void FB_OSD_SET_CMP_DISP_STATE(FB_OSD_RGN_T* pt_left,FB_OSD_RGN_T* pt_right);
int FB_OSD_IS_CMP_DISP_STATE(FB_OSD_RGN_T* pt_left,FB_OSD_RGN_T* pt_right);
int FB_OSD_IS_CMP_DISP_INIT_STATE(FB_OSD_RGN_T* pt_left,FB_OSD_RGN_T* pt_right);
void FB_OSD_SET_CMP_STATE(FB_OSD_RGN_T* pt_left,FB_OSD_RGN_T* pt_right);
int FB_OSD_IS_CMP_STATE(FB_OSD_RGN_T* pt_left,FB_OSD_RGN_T* pt_right);
int FB_OSD_IS_CMP_INIT_STATE(FB_OSD_RGN_T* pt_left,FB_OSD_RGN_T* pt_right);
void FB_OSD_SET_UNCMP_STATE(FB_OSD_RGN_T* pt_left,FB_OSD_RGN_T* pt_right);
int FB_OSD_IS_UNCMP_STATE(FB_OSD_RGN_T* pt_left,FB_OSD_RGN_T* pt_right);
int FB_OSD_IS_UNCMP_INIT_STATE(FB_OSD_RGN_T* pt_left,FB_OSD_RGN_T* pt_right);


//---------------------------------------------------------------------------
// Global variables
//---------------------------------------------------------------------------

HANDLE_T hMt53fbMutex;

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------
#if 1 /* MHF Linux - yjg 9x mmp */
static int fb_user_virt_start = 0;
#endif
#if !defined(ANDROID) || defined(ANDROID_TWO_WORLDS)
static struct device _mt53fb_dev;
#endif


static struct mt53fb_par _cur_par;
static struct fb_info _fb_info;
static struct mt53fb_imagebuffer _rImgBufInfo;
static struct mt53fb_imagebuffer _rFbmBufInfo;
static struct mt53fb_imagebuffer _rFbmBufMpeg;

static struct mt53fb_imagebuffer _rCrsrInfo;

#ifdef CC_FBM_TWO_FBP_SHARED_WITH_DFB
static struct mt53fb_imagebuffer _rImgBufInfo2;
#endif
#ifdef CC_DFB_SUPPORT_VDP_LAYER
#define DFB_VDP_LAYER_MAX       0x5
mt53fb_vdp_crop_rect mt53fb_vdp_crop_rect_array[DFB_VDP_LAYER_MAX];
struct mt53fb_imagebuffer _rVdpBufInfo;
#endif


static UINT32 _u4HidKbLanguageModeChg = 0;

static FB_OSD_RGN_T au4_plane_rgn[OSD_PLANE_MAX_NUM][E_FB_OSD_RGN_MAX];

/* MHF Linux CHUN  */
#define NULL_REGION_IDX 0xffffffff

static struct mt53fb_set  t_fb_set[OSD_PLANE_MAX_NUM];
static UINT32 *au4_draw_lines   = NULL;
static UINT32 u4_draw_line      = 0;

#ifdef CC_ANDROID_3D_UI_SUPPORT
UINT32 _u43DUiRgn = 0xFFFFFFFF;

extern UINT32 u4UiMode;
extern UINT32 u4Plane2RegionList;
extern BOOL u4Plane2Enabel;
extern BOOL fgUpdateAttr;
extern UINT32 u4UiWidth;
extern UINT32 u4UiHeight;
extern UINT32 u4OriSc2_Enable;
extern UINT32 u4OriSc2SrcW;
extern UINT32 u4OriSc2SrcH;
extern UINT32 u4OriSc2DstW;
extern UINT32 u4OriSc2DstH;
extern UINT32 u4OriSc2Is16Bpp;

#define MAX_3D_UI_BUFFER_SIZE ((((1280 * 1080 * 4 ) - 1) >> PAGE_SHIFT << PAGE_SHIFT) + PAGE_SIZE)

#endif


static UINT32 _au4PlaneOrderArray[PMX_MAX_INPORT_NS] =
{
#if defined (ANDROID_TWO_WORLDS)
        PMX_OSD3,
        PMX_OSD1,
        PMX_OSD2,
#elif defined(ANDROID) || defined(GTV)
        PMX_OSD3,
        PMX_OSD2,
        PMX_OSD1,
#else
        PMX_OSD2,
        PMX_OSD3,
        PMX_OSD1,
#endif
        PMX_PIP,
        PMX_MAIN };


static UINT8 _au1Log2HwPlane[SIZE_PLANE_ORDER_ARRAY] =
{ PMX_MAIN, PMX_PIP, PMX_OSD2, PMX_OSD3 };

#ifndef LINUX_TURNKEY_SOLUTION
static UINT8 _au1Hw2LogPlane[PMX_MAX_INPORT_NS] =
{ MIXER_PORT_VIDEO_MAIN, MIXER_PORT_VIDEO_PIP, MIXER_PORT_NONE,
        MIXER_PORT_OSD_1, MIXER_PORT_OSD_2 };
#endif

static struct fb_ops _mt53fb_ops =
{ .owner = THIS_MODULE, .fb_open = _mt53fb_open, .fb_check_var =
        _mt53fb_check_var,
        .fb_set_par = _mt53fb_set_par, // return -EINVAL
        .fb_setcolreg = _mt53fb_setcolreg, // return 0
        //.fb_pan_display   = _mt53fb_pan_display,
        .fb_fillrect = cfb_fillrect, .fb_copyarea = cfb_copyarea,
        .fb_imageblit = cfb_imageblit, .fb_mmap = _mt53fb_mmap,
#ifdef LINUX_TURNKEY_SOLUTION
        //    .fb_cursor      = soft_cursor,
#else
        .fb_cursor = _mt53fb_soft_cursor,
#endif
        .fb_ioctl = _mt53fb_ioctl, };


static struct options
{
    char *name;
    void (*parse)(char *opt);
}_mt53fb_opt_table[] __initdata =
{
    {   "colormode", _mt53fb_parse_colormode},
    {   "dram", _mt53fb_parse_dram},
    {   NULL, NULL}
};

static unsigned int * _apu4Palette[OSD_PLANE_MAX_NUM];


static unsigned char* _pu1TextArray = NULL;

#if !defined(ANDROID) || defined(ANDROID_TWO_WORLDS)
static unsigned char* _pu1TextArrayVirt = NULL;
#endif


static volatile GFX_CMDQUE_T *_prShmCmdQue = NULL;

static volatile UINT64 *_pu8CmdQueBuf = NULL;


int osd1_src_width;
int osd1_src_height;
int osd1_output_width;
int osd1_output_height;
int osd2_src_width;
int osd2_src_height;
int osd2_output_width;
int osd2_output_height;

#ifdef CC_SUPPORT_MULTIPLE_FBDEV
typedef struct mtk_fb_win_t
{
    int pid;
    unsigned int x;
    unsigned int y;
    unsigned int w;
    unsigned int h;
    unsigned short copying;
    unsigned short swaping;
    unsigned short dirty;
}mtk_fb_win_t;
#endif /* CC_SUPPORT_MULTIPLE_FBDEV */

#if defined(ANDROID) || defined(CC_SUPPORT_MULTIPLE_FBDEV) || defined(CC_LINUX_GPU_SUPPORT)
#define CHECK_FAIL(stmt, exp_ret) \
do {\
    int _____ret = (stmt);\
    if(_____ret != (exp_ret))\
    {\
        printk("CHECK FAILED %s ret:%d\n", #stmt, _____ret);\
    }\
}while(0)

#ifndef CC_SUPPORT_MULTIPLE_FBDEV
    #define ANDROID_OSD OSD_PLANE_3 /* For Android One World */
    #define ANDROID_OSD_SCALER OSD_SCALER2
#else
#define ANDROID_OSD OSD_PLANE_1
#if defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
#define ANDROID_OSD_SCALER OSD_SCALER_1
#endif /* defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) */
#endif /* CC_SUPPORT_MULTIPLE_FBDEV */

#define ANDROID_NUMBER_OF_BUFFERS 2
#ifdef ANDROID_FB_CM_ARGB8888
#define ANDROID_BITS_PER_PIXEL 32
#else
#define ANDROID_BITS_PER_PIXEL 16
#endif
/* Modify these macros to suit the hardware */

#define ANDROID_FB_WIDTH 1280
#define ANDROID_FB_HEIGHT 720

#define ANDROID_ENABLE_COMPOSER 0
#define ANDROID_MAX_NUM_OF_WINS 2

#if !defined(ANDROID)  &&  !defined(CC_LINUX_GPU_SUPPORT)
static UINT16 _u2PanelWidth = 1920;
static UINT16 _u2PanelHeight = 1080;


static UINT8 _u13DMode = 0;
static UINT8 _u13DModeApplied = 0;
#endif


static uint width = ANDROID_FB_WIDTH;
static uint height = ANDROID_FB_HEIGHT;

static mtk_drv_fb_t * drv;
struct mtk_fb *fb;
struct mtk_fb
{
    int rotation;
    struct fb_info fb;
    u32 cmap[16];
};

static UINT32 rgn = (UINT32)NULL;
static UINT32 rgn_list = (UINT32)NULL;
static UINT32 rgn_2 = (UINT32)NULL;
static UINT32 rgn_list_2 = (UINT32)NULL;

static u32 _get_line_length(u32 w, u32 bits_per_pix)
{
    /* Aligned on 16 bytes */
#if PITCH_ALIGNMENT_64
    return (w * bits_per_pix/8 + 0x3f) & ~0x3f;
#else
    return (w * bits_per_pix/8 + 0xf) & ~0xf;
#endif
}


static inline u32 convert_bitfield(int val, struct fb_bitfield *bf)
{
    unsigned int mask = (1 << bf->length) - 1;

    return (val >> (16 - bf->length) & mask) << bf->offset;
}

/* set the software color map.  Probably doesn't need modifying. */
static int
mtk_fb_setcolreg(unsigned int regno, unsigned int red, unsigned int green,
        unsigned int blue, unsigned int transp, struct fb_info *info)
{
    struct mtk_fb *fb = container_of(info, struct mtk_fb, fb);

    if (regno < 16)
    {
        fb->cmap[regno] = convert_bitfield(transp, &fb->fb.var.transp) |
        convert_bitfield(blue, &fb->fb.var.blue) |
        convert_bitfield(green, &fb->fb.var.green) |
        convert_bitfield(red, &fb->fb.var.red);
        return 0;
    }
    else
    {
        return 1;
    }
}

/* check var to see if supported by this device.  Probably doesn't
 * need modifying.
 */
static int mtk_fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
    if((var->rotate & 1) != (info->var.rotate & 1))
    {
        if((var->xres != info->var.yres) ||
                (var->yres != info->var.xres) ||
                (var->xres_virtual != info->var.yres) ||
                (var->yres_virtual >
                        info->var.xres * ANDROID_NUMBER_OF_BUFFERS) ||
                (var->yres_virtual < info->var.xres ))
        {
            return -EINVAL;
        }
    }
    else
    {
        #ifdef CC_SUPPORT_4K2K_UI
           if( (var->yres_virtual < info->var.yres ))
     #else
        if((var->xres != info->var.xres) ||
                (var->yres != info->var.yres) ||
                (var->xres_virtual != info->var.xres) ||
                (var->yres_virtual >
                        info->var.yres * ANDROID_NUMBER_OF_BUFFERS) ||
                (var->yres_virtual < info->var.yres ))
      #endif
        {
            return -EINVAL;
        }
    }
#ifdef FB_DRIVER_IMPROVE
    if((var->xoffset != info->var.xoffset) ||
            (var->grayscale != info->var.grayscale))
    {
        return -EINVAL;
    }
#else
    if((var->xoffset != info->var.xoffset) ||
            (var->bits_per_pixel != info->var.bits_per_pixel) ||
            (var->grayscale != info->var.grayscale))
    {
        return -EINVAL;
    }
#endif
    return 0;
}

#ifdef FB_DRIVER_IMPROVE
static int mtk_fb_blank(int blank_mode, struct fb_info *info)
{
    switch (blank_mode)
    {
        case FB_BLANK_UNBLANK:
        OSD_PLA_Enable(ANDROID_OSD, TRUE);
        break;
        default:
        OSD_PLA_Enable(ANDROID_OSD, FALSE);
        break;
    }
    return 0;
}
#endif
/* Handles screen rotation if device supports it. */
static int mtk_fb_set_par(struct fb_info *info)
{
#ifdef FB_DRIVER_IMPROVE
    UINT32 u4Region = rgn;
    UINT32 u4ColorMode = OSD_CM_ARGB8888_DIRECT32;
    UINT32 u4Bpp = 32;
#endif
    struct mtk_fb *fb = container_of(info, struct mtk_fb, fb);
    if(fb->rotation != fb->fb.var.rotate)
    {
        info->fix.line_length = drv->get_line_length(info->var.xres, ANDROID_BITS_PER_PIXEL);
        fb->rotation = fb->fb.var.rotate;
    }
#ifdef CC_SUPPORT_4K2K_UI
   {
     printf("fb driver width1 =%d, height=%d, reserved[0]=%d\n", info->var.xres, info->var.yres, info->var.reserved[0]);
         {
      //  info->fix.line_length = drv->get_line_length(info->var.xres, ANDROID_BITS_PER_PIXEL);
        CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_BMP_W, info->var.xres), 0);
        CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_BMP_H, info->var.yres), 0);
        CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_DISP_W, info->var.xres), 0);
        CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_DISP_H, info->var.yres), 0);
        //CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_BMP_PITCH,  info->fix.line_length), 0);
        fg4k2kOsdSetting = TRUE;
      CHECK_FAIL(OSD_SC_Scale(ANDROID_OSD_SCALER, TRUE, info->var.xres, info->var.yres,
                       OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayHeight()), 0);
          fg4k2kOsdSetting = FALSE;
    
       //show_mem(0);
     }
  }
 #endif
#ifdef FB_DRIVER_IMPROVE
    // fb->fb.var
    if((info->var.red.length == 5)&&(info->var.green.length == 6)&&(info->var.blue.length == 5))
    {
        u4ColorMode = OSD_CM_RGB565_DIRECT16;
        u4Bpp = 16;
#ifdef FB_SUPPORT_ARGB_ORDER
        IGNORE_RET(_OSD_RGN_SetASel(u4Region, 3));
        IGNORE_RET(_OSD_RGN_SetYrSel(u4Region, info->var.red.offset/5));
        IGNORE_RET(_OSD_RGN_SetUgSel(u4Region, info->var.green.offset/5));
#endif
    }
    else if((info->var.red.length == 5)&&(info->var.green.length == 5)&&(info->var.blue.length == 5))
    {
        u4ColorMode = OSD_CM_ARGB1555_DIRECT16;
        u4Bpp = 16;
#ifdef FB_SUPPORT_ARGB_ORDER
        IGNORE_RET(_OSD_RGN_SetASel(u4Region, info->var.transp.offset/5));
        IGNORE_RET(_OSD_RGN_SetYrSel(u4Region, info->var.red.offset/5));
        IGNORE_RET(_OSD_RGN_SetUgSel(u4Region, info->var.green.offset/5));
#endif
    }
    else if((info->var.red.length == 4)&&(info->var.green.length == 4)&&(info->var.blue.length == 4))
    {
        u4ColorMode = OSD_CM_ARGB4444_DIRECT16;
        u4Bpp = 16;
#ifdef FB_SUPPORT_ARGB_ORDER
        IGNORE_RET(_OSD_RGN_SetASel(u4Region, info->var.transp.offset/4));
        IGNORE_RET(_OSD_RGN_SetYrSel(u4Region, info->var.red.offset/4));
        IGNORE_RET(_OSD_RGN_SetUgSel(u4Region, info->var.green.offset/4));
#endif
    }
    else
    {
        u4ColorMode = OSD_CM_ARGB8888_DIRECT32;
        u4Bpp = 32;
#ifdef FB_SUPPORT_ARGB_ORDER
        IGNORE_RET(_OSD_RGN_SetASel(u4Region, info->var.transp.offset/8));
        IGNORE_RET(_OSD_RGN_SetYrSel(u4Region, info->var.red.offset/8));
        IGNORE_RET(_OSD_RGN_SetUgSel(u4Region, info->var.green.offset/8));
#endif
    }
    printk("mtk_fb_set_par info->var.xres=%d, u4Bpp =%d\n", info->var.xres, u4Bpp );
    if(OSD_RGN_Set(u4Region, OSD_RGN_COLORMODE, u4ColorMode) != OSD_RET_OK)
    {
        printf("mtk_fb_set_par fail for color mode\n");
    }
#if 0
    info->fix.line_length = _get_line_length(info->var.xres, u4Bpp);
    if(OSD_RGN_Set(u4Region, OSD_RGN_BMP_PITCH, info->fix.line_length) != OSD_RET_OK)
    {
        printf("mtk_fb_set_par fail 2 for color mode\n");
    }
#endif  

#endif
    return 0;
}

/* Pan the display if device supports it. */
static int mtk_fb_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
{
    struct mtk_fb *fb __attribute__ ((unused))
    = container_of(info, struct mtk_fb, fb);

#ifdef CC_SUPPORT_MULTIPLE_FBDEV
    drv->set_base(fb->fb.fix.smem_start + var->yoffset * _get_line_length(fb->fb.var.xres, ANDROID_BITS_PER_PIXEL),
            var->yoffset);
#else
#ifdef FB_DRIVER_IMPROVE
    unsigned long u4Base = fb->fb.fix.smem_start;
    u4Base = u4Base + var->yoffset * info->fix.line_length + var->xoffset*(var->bits_per_pixel>>3);
    drv->set_base(u4Base);
#else
    drv->set_base(fb->fb.fix.smem_start + fb->fb.var.xres *
            (ANDROID_BITS_PER_PIXEL >> 3) * var->yoffset);
#endif
#endif /* CC_SUPPORT_MULTIPLE_FBDEV */
    return 0;
}

#ifdef CC_SUPPORT_MULTIPLE_FBDEV
static HANDLE_T _h_fb_mutex = 0;
static UINT32 _fb_bufs[ANDROID_NUMBER_OF_BUFFERS+1] =
{   0, 0, 0};

static UINT32 _fb_num_wins = 0;
static UINT32 _fb_front_buf = 0;
static UINT32 _fb_back_buf = 0;
static mtk_fb_win_t _fb_wins[2] =
{
    {   -1, 0, 0, 0, 0, 0, 0, 0},
    {   -1, 0, 0, 0, 0, 0, 0, 0}
};

static int _fb_find_win(mtk_fb_win_t wins[], int pid)
{
    UINT32 i;
    for(i = 0; i < ANDROID_MAX_NUM_OF_WINS; i++)
    {
        if(wins[i].pid == pid)
        {
            break;
        }
    }
    if(i >= ANDROID_MAX_NUM_OF_WINS)
    {
        return -EFAULT;
    }
    return i;
}
#endif /* CC_SUPPORT_MULTIPLE_FBDEV */

static int _mtk_fb_set_ioctl(void* pvArg)
{
    int i4Ret = 0;
    UINT32 u4Region = rgn;

    struct mt53fb_set rSet;

    if(copy_from_user(&rSet, pvArg, sizeof(struct mt53fb_set)))
    {
        printk(KERN_WARNING "_mt53fb_set_ioctl() - copy_from_user() fail\n");
        return -EFAULT;
    }

    if(rSet.mask & MT53FB_SET_MASK_OPACITY)
    {
#ifdef FB_HARD_FADING
        /*CHUN: We don't allow DFB wm change Window layer opacity. FIXME*/

        //      if(rSet.rSetting.u4OsdPlaneID != 0)        //for DTV00084581

        {
#endif
#ifdef LINUX_TURNKEY_SOLUTION
            if(OSD_PLA_SetFading(ANDROID_OSD, rSet.rSetting.u4Opacity) != OSD_RET_OK)
#else
            if(OSD_PLA_SetFading(_au1Log2HwPlane[rSet.rSetting.u4OsdPlaneID + 2] - 2, rSet.rSetting.u4Opacity) != OSD_RET_OK)
#endif
            {
                return -EINVAL;
            }
#ifdef FB_HARD_FADING
        }
#endif
    }
    if(rSet.mask & MT53FB_SET_MASK_COLORKEYEN)
    {
        if(OSD_RGN_Set(u4Region, OSD_RGN_COLOR_KEY_EN, rSet.rSetting.u4ColorKeyEn) != OSD_RET_OK ||
                (rgn_2 && OSD_RGN_Set(rgn_2, OSD_RGN_COLOR_KEY_EN, rSet.rSetting.u4ColorKeyEn) != OSD_RET_OK))
        {
            return -EINVAL;
        }
    }

    if(rSet.mask & MT53FB_SET_MASK_COLORKEY)
    {
        if(OSD_RGN_Set(u4Region, OSD_RGN_COLOR_KEY, rSet.rSetting.u4ColorKey) != OSD_RET_OK ||
                (rgn_2 && OSD_RGN_Set(rgn_2, OSD_RGN_COLOR_KEY, rSet.rSetting.u4ColorKey) != OSD_RET_OK))
        {
            return -EINVAL;
        }
    }

    return i4Ret;
}

#ifdef CC_ANDROID_3D_UI_SUPPORT
static int _mtk_fb_set_uimode(unsigned long pvArg)
{
    int i4Ret = 0;
    UINT32 u4Mode = 0;
    u4Mode = (UINT32) pvArg;
    printf("the UI mode is %d\n",u4Mode );
    OSD_PLA_SetUIMode(u4Mode, 0, 0);
    return i4Ret;
}
#endif

 typedef struct hwc_osd_info
 {
     unsigned int u4Addr;
     unsigned int u4Width;
     unsigned int u4Height;
     unsigned int u4Pitch;
     unsigned int u4UserEnable;
 }hwc_osd_info;


 static hwc_osd_info _tHWCOSDInfo = {
        .u4Addr = 0,
        .u4Width = 0,
        .u4Height = 0,
        .u4Pitch = 0,
        .u4UserEnable = 0
};

#if defined(CC_MTK_ION_SUPPORT)
//mtk70854
static UINT32 _u4HWCAddRegion[2] = {NULL_REGION_IDX, NULL_REGION_IDX};
static UINT32 _u4HWCAddRegionList[2] = {NULL_REGION_IDX, NULL_REGION_IDX};
static UINT32 _u4HWCAddFlipCnt = 0;
static UINT32 _u4HWCAddInit = 0;

static UINT32 _u4HWCAddPlane = OSD_PLANE_2;
static UINT32 _u4HWCAddBase = 0xF0000000;
UINT32 _u4HWCAddWidth = 3840;
UINT32 _u4HWCAddHeight = 2160;
static UINT32 _u4HWCAddPitch = 15360;
static UINT32 _u4HWCAddCM = OSD_CM_ARGB8888_DIRECT32;

static UINT32 _u4HWCShowAddr = 0;
static UINT32 _u4HWCShowAddr2 = 0; //BACKUP

static UINT32 _u4FHD4K2KMode = 0; //0:fhd mode 1: 4k2k mode

static UINT32 _u4HWCFBReginlist = NULL_REGION_IDX;
BOOL fgHwcEn = 0;






static int _mtk_hwc_add_init(void)
{
    int i;
    UINT32 u4RegionList, u4Region;
    
    if(_u4HWCAddInit == 0)
    {
        _u4HWCAddInit = 1;
        OSD_PLA_Enable(_u4HWCAddPlane, FALSE);

        for(i = 0; i < 2; i++)
        {
            u4RegionList =  _u4HWCAddRegionList[i];
            u4Region = _u4HWCAddRegion[i];
            
            if(u4RegionList == NULL_REGION_IDX)
            {
                VERIFY(OSD_RGN_LIST_Create(&u4RegionList) == OSD_RET_OK);
                _u4HWCAddRegionList[i] = u4RegionList;
            }   
            
            if(u4Region == NULL_REGION_IDX)
            {
                VERIFY(OSD_RGN_Create(&u4Region, _u4HWCAddWidth, _u4HWCAddHeight,
                       (void *)_u4HWCAddBase, _u4HWCAddCM, _u4HWCAddPitch,
                       0, 0, _u4HWCAddWidth, _u4HWCAddHeight) == OSD_RET_OK);

                VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_BIG_ENDIAN, 0) == OSD_RET_OK);

                VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_MIX_SEL, OSD_BM_PIXEL) == OSD_RET_OK);

                VERIFY(OSD_RGN_Insert(u4Region, u4RegionList) == OSD_RET_OK);

                _u4HWCAddRegion[i] = u4Region;          
            }       
        }
    }

    return 0;       
}

int  _mtk_fbhwc_blit_to_fb(void* pvArg)  //hwc to frambuffer directly
{
     fbdev_blit fb_blit;
     unsigned int u4HWAddr;
     UINT32 u4RegionList, u4Region;
     UINT32 u44K2KSetting = 0;
     BOOL bEnable = FALSE;
     
     if(copy_from_user((void*)&fb_blit, pvArg, sizeof(fbdev_blit)))
     {
         printk(KERN_WARNING "mtk fb blit error: Copy from user error!\n");
         return -EFAULT;
     }

    _mtk_hwc_add_init();

    u4RegionList =  _u4HWCAddRegionList[_u4HWCAddFlipCnt % 2];
    u4Region = _u4HWCAddRegion[_u4HWCAddFlipCnt % 2];

    OSD_RGN_Set(u4Region, OSD_RGN_BMP_W, fb_blit.width);
    OSD_RGN_Set(u4Region, OSD_RGN_DISP_W, fb_blit.width);
    OSD_RGN_Set(u4Region, OSD_RGN_BMP_H, fb_blit.height);   
    OSD_RGN_Set(u4Region, OSD_RGN_DISP_H, fb_blit.height);
    OSD_RGN_Set(u4Region, OSD_RGN_BMP_PITCH, fb_blit.src_pitch);
    OSD_RGN_Set(u4Region, OSD_RGN_BMP_ADDR, fb_blit.src_addr); 

    _u4HWCShowAddr = fb_blit.src_addr;
    _u4HWCShowAddr2 = fb_blit.src_addr;

    _tHWCOSDInfo.u4Addr = fb_blit.src_addr;
    _tHWCOSDInfo.u4Width = fb_blit.width;
    _tHWCOSDInfo.u4Height = fb_blit.height;
    _tHWCOSDInfo.u4Pitch = fb_blit.src_pitch;
    _tHWCOSDInfo.u4UserEnable = 1;

    _u4HWCAddWidth = fb_blit.width;
    _u4HWCAddHeight = fb_blit.height;

    //VERIFY(OSD_PLA_FlipTo(_u4HWCAddPlane, u4RegionList) == OSD_RET_OK);
    fgHwcEn = TRUE;
    _u4HWCFBReginlist = u4RegionList;

    u44K2KSetting = (fb_blit.width == 3840 && fb_blit.height == 2160) ? 1 : 0;

    OSD_PLA_GetEnable(_u4HWCAddPlane, &bEnable);
    
      if(u44K2KSetting && (OSD_BASE_GetDisplayWidth() ==1920)) //for browser full screen play in FHD
    {
       OSD_RGN_Set(u4Region, OSD_RGN_STEP_H, 0x2000);
       OSD_RGN_Set(u4Region, OSD_RGN_STEP_V, 0x2000);
    }
    else
    {
      OSD_RGN_Set(u4Region, OSD_RGN_STEP_H, 0x1000);
       OSD_RGN_Set(u4Region, OSD_RGN_STEP_V, 0x1000);
    }
     
    //if(bEnable == FALSE && (_u4FHD4K2KMode == 1 || u44K2KSetting == 0))
    if(bEnable == FALSE )
    {
       if((OSD_BASE_GetDisplayWidth() ==1920) && u44K2KSetting)
        {
          CHECK_FAIL(OSD_SC_Scale(OSD_SCALER_2, TRUE, OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayHeight(),
                         OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayHeight()), 0);
        }
       else
        {
        CHECK_FAIL(OSD_SC_Scale(OSD_SCALER_2, TRUE, fb_blit.width, fb_blit.height,
                         OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayHeight()), 0);
        }

        OSD_PLA_SetFading(_u4HWCAddPlane, 255);             
        OSD_PLA_Enable(_u4HWCAddPlane, TRUE);
    }   

#if 0
    while(1)
    {
        OSD_PLA_Get_Hw_RealBmpAddr(_u4HWCAddPlane, &u4HWAddr);  
        OSD_PLA_GetEnable(_u4HWCAddPlane, &bEnable);
        if(u4HWAddr == _u4HWCShowAddr || bEnable == FALSE)
        {
            break;
        }

        //_mtk_hwc_wait_for_vsync();
        //schedule();
        
    }   
#endif
    _u4HWCAddFlipCnt++;

    return 0;
}






int _mtk_fbhwc_compose(void* pvArg)
{
    fbdev_blit fb_blit;
 
    if(copy_from_user((void*)&fb_blit, pvArg, sizeof(fbdev_blit)))
    {
        printk(KERN_WARNING "mtk fb blit error: Copy from user error!\n");
        return -EFAULT;
    }
    
       GFX_SetSrc((UINT8 *)fb_blit.src_addr, CM_ARGB8888_DIRECT32, fb_blit.src_pitch);
    GFX_SetDst((UINT8 *)fb_blit.dst_addr, CM_ARGB8888_DIRECT32, fb_blit.dst_pitch);

    if(fb_blit.blend > 0)
    {
    GFX_ComposeLoop(fb_blit.src_x, fb_blit.src_y, fb_blit.dst_x, fb_blit.dst_y, fb_blit.width, fb_blit.height,  0xFF, E_AC_SRC_OVER,  0);
    }
    else
    {
    GFX_BitBlt(fb_blit.src_x, fb_blit.src_y, fb_blit.dst_x, fb_blit.dst_y, fb_blit.width, fb_blit.height);
    }
     
    return 0;
}

int _mtk_check_fbhwc_addr_safely(void *pvArg)
{
    BOOL bEnable = FALSE;
    unsigned int u4Addr, u4HWAddr;

    if(copy_from_user((void*)&u4Addr, pvArg, sizeof(unsigned int)))
    {
        printk(KERN_WARNING "_mtk_check_fbhwc_stable error: Copy from user error!\n");
        return -EFAULT;
    }

    OSD_PLA_GetEnable(_u4HWCAddPlane, &bEnable);

    if(bEnable == FALSE)
    {
        return 0;
    }

    while(_u4HWCShowAddr != 0 && _u4HWCShowAddr == u4Addr && bEnable)
    {
        schedule();
     OSD_PLA_GetEnable(_u4HWCAddPlane, &bEnable);
    }

    while(1)
    {
        OSD_PLA_Get_Hw_RealBmpAddr(_u4HWCAddPlane, &u4HWAddr);  
        OSD_PLA_GetEnable(_u4HWCAddPlane, &bEnable);
        if(u4HWAddr != u4Addr || bEnable == FALSE)
        {
            break;
        }

    schedule();
    }

    
    return 0;
}
#endif
static int _mtk_fb_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg)
{
    void __user *pvArg = (void __user *)arg;
    INT32 i4Ret = 0;
    switch(cmd)
    {
        case FBIO_SET:
        i4Ret = _mtk_fb_set_ioctl(pvArg);
        break;
#ifdef CC_ANDROID_3D_UI_SUPPORT
        case FBIO_SET_UIMODE:
        i4Ret = _mtk_fb_set_uimode(arg);
        break;
#endif
#if defined(CC_MTK_ION_SUPPORT)
case FBIO_FBHWC_LOCK_GFX:
        GFX_Lock(); 
       GFX_MMU_Setting(TRUE, TRUE, TRUE);           
        break;
     case FBIO_FBHWC_UNLOCK_GFX:
        GFX_MMU_Set_Enable(FALSE);           
        GFX_Unlock();       
        break;
     case FBIO_FBHWC_FLUSH_GFX:
        GFX_Flush();
        GFX_Task_Wait_Sync();     
        break;
    case FBIO_FBHWC_BLIT:
        i4Ret = _mtk_fbhwc_blit_to_fb(arg);
        break;
    case FBIO_FBHWC_COMPOSE:
        i4Ret = _mtk_fbhwc_compose(arg);
        break;
    case FBIO_CHECK_WAIT_FOR_VSYNC:
        i4Ret = _mtk_check_fbhwc_addr_safely(arg);
        break;
#endif

#ifdef CC_SUPPORT_MULTIPLE_FBDEV
        case 0x4651:
        printk("Enable POSD: %d\n", ANDROID_OSD);
        OSD_PLA_Enable(ANDROID_OSD, TRUE);
        break;
        case 0x4652:
        printk("Disable POSD: %d\n", ANDROID_OSD);
        OSD_PLA_Enable(ANDROID_OSD, FALSE);
        break;
        case 0x4650:
        {
            struct fb_var_screeninfo var; /* Current var */

            if(copy_from_user((void*)&var, pvArg, sizeof(struct fb_var_screeninfo)))
            {
                printk(KERN_WARNING "mt53fb fbio set base() error: Copy from user error!\n");
                return -EFAULT;
            }

            if(var.reserved[0])
            {
                i4Ret = OSD_RGN_Set(rgn, OSD_RGN_BMP_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(rgn, OSD_RGN_BMP_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(rgn, OSD_RGN_DISP_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(rgn, OSD_RGN_DISP_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(rgn, OSD_RGN_OUT_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(rgn, OSD_RGN_OUT_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(rgn, OSD_RGN_POS_X, 0);
                i4Ret = OSD_RGN_Set(rgn, OSD_RGN_POS_Y, 0);
                i4Ret = OSD_RGN_Set(rgn, OSD_RGN_BMP_ADDR, _fb_bufs[0]);
                i4Ret = OSD_RGN_Set(rgn_2, OSD_RGN_BMP_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(rgn_2, OSD_RGN_BMP_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(rgn_2, OSD_RGN_DISP_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(rgn_2, OSD_RGN_DISP_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(rgn_2, OSD_RGN_OUT_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(rgn_2, OSD_RGN_OUT_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(rgn_2, OSD_RGN_POS_X, 0);
                i4Ret = OSD_RGN_Set(rgn_2, OSD_RGN_POS_Y, 0);
                i4Ret = OSD_RGN_Set(rgn_2, OSD_RGN_BMP_ADDR, _fb_bufs[1]);
#ifdef CC_MT5396
                OSD_SC_Scale(OSD_SCALER_1, TRUE, var.reserved[1], var.reserved[2],
                        OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayHeight());
#endif
            }
            else
            {
#if 0
                i4Ret = OSD_RGN_Set(rgn, OSD_RGN_BMP_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(rgn, OSD_RGN_BMP_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(rgn, OSD_RGN_DISP_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(rgn, OSD_RGN_DISP_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(rgn, OSD_RGN_OUT_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(rgn, OSD_RGN_OUT_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(rgn, OSD_RGN_POS_X, var.reserved[3]);
                i4Ret = OSD_RGN_Set(rgn, OSD_RGN_POS_Y, var.reserved[4]);
                i4Ret = OSD_RGN_Set(rgn, OSD_RGN_BMP_ADDR, _fb_bufs[0]);
                i4Ret = OSD_RGN_Set(rgn_2, OSD_RGN_BMP_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(rgn_2, OSD_RGN_BMP_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(rgn_2, OSD_RGN_DISP_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(rgn_2, OSD_RGN_DISP_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(rgn_2, OSD_RGN_OUT_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(rgn_2, OSD_RGN_OUT_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(rgn_2, OSD_RGN_POS_X, var.reserved[3]);
                i4Ret = OSD_RGN_Set(rgn_2, OSD_RGN_POS_Y, var.reserved[4]);
                i4Ret = OSD_RGN_Set(rgn_2, OSD_RGN_BMP_ADDR, _fb_bufs[2]);
#ifdef CC_MT5396
                OSD_SC_Scale(OSD_SCALER_1, TRUE, var.reserved[1], var.reserved[2], OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayHeight());
#endif
#endif
            }
            break;
        }

        case 0x4653:
        {
            fbdev_blit fb_blit;
            if(copy_from_user((void*)&fb_blit, pvArg, sizeof(fbdev_blit)))
            {
                printk(KERN_WARNING "mtk fb blit error: Copy from user error!\n");
                return -EFAULT;
            }

            GFX_Lock();

            GFX_SetSrc((UINT8 *)fb_blit.src_addr, CM_ARGB8888_DIRECT32, fb_blit.src_pitch);

            GFX_SetDst((UINT8 *)fb_blit.dst_addr, CM_ARGB8888_DIRECT32, fb_blit.dst_pitch);

            GFX_SetBltOpt((UINT32)D_GFXFLAG_NONE, 0x00000000, 0xffffffff);

            GFX_SetAlpha(0);

            GFX_BitBlt(0, 0, 0, 0, fb_blit.width, fb_blit.height);

            GFX_Flush();

            GFX_Unlock();

            break;
        }

        case 0x4654:
        {
            fbdev_blit fb_blit;
            if(copy_from_user((void*)&fb_blit, pvArg, sizeof(fbdev_blit)))
            {
                printk(KERN_WARNING "mtk fb blit error: Copy from user error!\n");
                return -EFAULT;
            }

            GFX_Lock();

            GFX_SetSrc((UINT8 *)fb_blit.src_addr, CM_RGB565_DIRECT16, fb_blit.src_pitch);

            GFX_SetDst((UINT8 *)fb_blit.dst_addr, CM_ARGB8888_DIRECT32, fb_blit.dst_pitch);

            GFX_SetBltOpt((UINT32)D_GFXFLAG_NONE, 0x00000000, 0xffffffff);

            GFX_SetAlpha(0xFF);

            GFX_BitBlt(0, 0, 0, 0, fb_blit.width, fb_blit.height);

            GFX_Flush();

            GFX_Unlock();

            break;
        }

        // Register window
        case 0x4655:
        {
            INT32 i;
            fbdev_ipc fb_ipc;

            if(copy_from_user((void*)&fb_ipc, pvArg, sizeof(fbdev_ipc)))
            {
                printk(KERN_WARNING "mtk fb blit error: Copy from user error!\n");
                return -EFAULT;
            }

            VERIFY(x_sema_lock(_h_fb_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);
            if(_fb_num_wins > 2)
            {
                VERIFY(x_sema_unlock(_h_fb_mutex) == OSR_OK);
                printk("mtk fb register win error: Maximum 2 win only!\n");
                return -EFAULT;
            }

            // Search for free entry
            if((i = _fb_find_win(_fb_wins, -1)) < 0)
            {
                VERIFY(x_sema_unlock(_h_fb_mutex) == OSR_OK);
                return -EFAULT;
            }
            // Add window and register window properties
            _fb_num_wins ++;
            _fb_wins[i].pid = fb_ipc.pid;
            _fb_wins[i].x = fb_ipc.x;
            _fb_wins[i].y = fb_ipc.y;
            _fb_wins[i].w = fb_ipc.w;
            _fb_wins[i].h = fb_ipc.h;
            _fb_wins[i].dirty = 0;
            _fb_wins[i].copying = 0;
            _fb_wins[i].swaping = 0;
            VERIFY(x_sema_unlock(_h_fb_mutex) == OSR_OK);
            printk("PID[%d] fb 0x4654 ioctl: Create fb window: offset (%d, %d) size (%d x%d)!\n",
                    fb_ipc.pid, fb_ipc.x, fb_ipc.y, fb_ipc.w, fb_ipc.h);

            // Clearing of buf
            GFX_Lock();
            GFX_SetColor(0);
            if(fb_ipc.src_fmt_argb8888)
            {
                GFX_SetDst((UINT8 *)_fb_bufs[2], CM_ARGB8888_DIRECT32, _get_line_length(ANDROID_FB_WIDTH, ANDROID_BITS_PER_PIXEL));
            }
            else
            {
                GFX_SetDst((UINT8 *)_fb_bufs[2], CM_RGB565_DIRECT16, _get_line_length(ANDROID_FB_WIDTH, ANDROID_BITS_PER_PIXEL));
            }
            GFX_Fill(_fb_wins[i].x, _fb_wins[i].y, _fb_wins[i].w, _fb_wins[i].h);
            GFX_Flush();
            GFX_Task_Wait_Sync();
            GFX_Unlock();
            break;
        }

        // Copy from rendering target to framebuffer
        case 0x4656:
        {
            INT32 i;
            fbdev_ipc fb_ipc;
            if(copy_from_user((void*)&fb_ipc, pvArg, sizeof(fbdev_ipc)))
            {
                printk(KERN_WARNING "mtk fb 0x4656 ioctl error: Copy from user error!\n");
                return -EFAULT;
            }
#if ANDROID_ENABLE_COMPOSER
            VERIFY(x_sema_lock(_h_fb_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);
            // Search for entry
            if((i = _fb_find_win(_fb_wins, fb_ipc.pid)) < 0)
            {
                VERIFY(x_sema_unlock(_h_fb_mutex) == OSR_OK);
                return -EFAULT;
            }
            // Indicates this proc copying
            _fb_wins[i].copying = 1;
            fb_ipc.curr_back_buf = _fb_back_buf;

            GFX_Lock();
            if(fb_ipc.src_fmt_argb8888)
            {
                GFX_SetSrc((UINT8 *)fb_ipc.src_buf, CM_ARGB8888_DIRECT32, fb_ipc.src_pitch);
                GFX_SetAlpha(0);
            }
            else
            {
                GFX_SetSrc((UINT8 *)fb_ipc.src_buf, CM_RGB565_DIRECT16, fb_ipc.src_pitch);
                GFX_SetAlpha(0xFF);
            }
            GFX_SetDst((UINT8 *)_fb_back_buf, CM_ARGB8888_DIRECT32, _get_line_length(ANDROID_FB_WIDTH, ANDROID_BITS_PER_PIXEL));
            GFX_SetBltOpt((UINT32)D_GFXFLAG_NONE, 0x00000000, 0xffffffff);
            GFX_BitBlt(0, 0, _fb_wins[i].x, _fb_wins[i].y, _fb_wins[i].w, _fb_wins[i].h);
            GFX_Flush();
            GFX_Task_Wait_Sync();
            GFX_Unlock();

            if(copy_to_user(pvArg, (void*)&fb_ipc, sizeof(fbdev_ipc)))
            {
                printk(KERN_WARNING "mtk fb 0x4656 ioctl error: Copy to user error!\n");
                return -EFAULT;
            }

            // Indicates this proc copy is done, waiting for flipping
            _fb_wins[i].copying = 0;

            // Indicates this proc has been updated but not yet flipped
            _fb_wins[i].dirty = 1;

            VERIFY(x_sema_unlock(_h_fb_mutex) == OSR_OK);
#else
            VERIFY(x_sema_lock(_h_fb_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);
            // Search for entry
            if((i = _fb_find_win(_fb_wins, fb_ipc.pid)) < 0)
            {
                VERIFY(x_sema_unlock(_h_fb_mutex) == OSR_OK);
                return -EFAULT;
            }
            // Indicates this proc copying
            _fb_wins[i].copying = 1;
            fb_ipc.curr_back_buf = _fb_front_buf;

            GFX_Lock();
            if(fb_ipc.src_fmt_argb8888)
            {
                GFX_SetSrc((UINT8 *)fb_ipc.src_buf, CM_ARGB8888_DIRECT32, fb_ipc.src_pitch);
                GFX_SetAlpha(0);
            }
            else
            {
                GFX_SetSrc((UINT8 *)fb_ipc.src_buf, CM_RGB565_DIRECT16, fb_ipc.src_pitch);
                GFX_SetAlpha(0xFF);
            }
            GFX_SetDst((UINT8 *)_fb_front_buf, CM_ARGB8888_DIRECT32, _get_line_length(ANDROID_FB_WIDTH, ANDROID_BITS_PER_PIXEL));
            GFX_SetBltOpt((UINT32)D_GFXFLAG_NONE, 0x00000000, 0xffffffff);
            GFX_BitBlt(0, 0, _fb_wins[i].x, _fb_wins[i].y, _fb_wins[i].w, _fb_wins[i].h);
            GFX_Flush();
            GFX_Task_Wait_Sync();
            GFX_Unlock();
            VERIFY(x_sema_unlock(_h_fb_mutex) == OSR_OK);
#endif /* FB1_ENABLE_COMPOSER */
            break;
        }

        // Swapping. If necessary, copy other non-dirty area from front framebuffer to back framebuffer
        case 0x4657:
        {
#if ANDROID_ENABLE_COMPOSER
            INT32 i;
            fbdev_ipc fb_ipc;
            if(copy_from_user((void*)&fb_ipc, pvArg, sizeof(fbdev_ipc)))
            {
                printk(KERN_WARNING "mtk fb blit error: Copy from user error!\n");
                return -EFAULT;
            }

            VERIFY(x_sema_lock(_h_fb_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);
            // Search for entry
            if((i = _fb_find_win(_fb_wins, fb_ipc.pid)) < 0)
            {
                VERIFY(x_sema_unlock(_h_fb_mutex) == OSR_OK);
                return -EFAULT;
            }
            // Become non-dirty because other app has call swap
            if(_fb_wins[i].dirty == 0)
            {
                // Indicates this proc is not performed swapping
                _fb_wins[i].swaping = 0;
                _fb_wins[i].dirty = 0;

                VERIFY(x_sema_unlock(_h_fb_mutex) == OSR_OK);
                printk("PID[%d] Ignore swaping because other is doing it. Waiting for other swaping\n", _fb_wins[i].pid);
                // wait for other app's blitting complete
                do
                {
                    x_thread_delay(1);
                }while(_fb_wins[i == 0?1:0].swaping == 1);
                break;
            }

            // Marked non-dirty because we are flipping it
            _fb_wins[i].dirty = 0;

            // Indicates this proc is performed swapping
            _fb_wins[i].swaping = 1;

            if(_fb_wins[i == 0?1:0].pid != -1 && _fb_wins[i == 0?1:0].copying)
            {
                printk("PID[%d] Waiting for other blitting\n", _fb_wins[i].pid);
                // wait for other app's blitting complete
                do
                {
                    x_thread_delay(1);
                }while(_fb_wins[i == 0?1:0].copying == 1);
                _fb_wins[i == 0?1:0].dirty = 1;
            }
            // Copy from front buf to back buf because other is not dirty

            else if(_fb_wins[i == 0?1:0].pid != -1 && _fb_wins[i == 0?1:0].dirty == 0)
            {
                fbdev_blit fb_blit;
                //printk("PID[%d] Copying from front 0x%8x to back 0x%8x because not dirty\n", _fb_wins[i].pid, _fb1_front_buf, _fb1_back_buf);
                if(copy_from_user((void*)&fb_blit, pvArg, sizeof(fbdev_blit)))
                {
                    printk(KERN_WARNING "mtk fb 1 blit error: Copy from user error!\n");
                    return -EFAULT;
                }

                GFX_Lock();

                GFX_SetSrc((UINT8 *)_fb_front_buf,
                        CM_ARGB8888_DIRECT32, _get_line_length(ANDROID_FB_WIDTH, ANDROID_BITS_PER_PIXEL));

                GFX_SetDst((UINT8 *)_fb_back_buf,
                        CM_ARGB8888_DIRECT32, _get_line_length(ANDROID_FB_WIDTH, ANDROID_BITS_PER_PIXEL));

                GFX_SetBltOpt((UINT32)D_GFXFLAG_NONE, 0x00000000, 0xffffffff);

                GFX_SetAlpha(0);

                GFX_BitBlt(_fb_wins[i == 0?1:0].x, _fb_wins[i == 0?1:0].y, _fb_wins[i == 0?1:0].x, _fb_wins[i == 0?1:0].y, _fb_wins[i == 0?1:0].w, _fb_wins[i == 0?1:0].h);

                GFX_Flush();

                GFX_Task_Wait_Sync();

                GFX_Unlock();

            }

            if(_fb_front_buf == _fb_bufs[0])
            {
                // printk("Flip to 0x%8x\n", _fb1_bufs[2]);
                CHECK_FAIL(OSD_PLA_FlipTo(ANDROID_OSD, rgn_list), 0);
            }
            else if(_fb_front_buf == _fb_bufs[1])
            {
                printk("0x4657 ioctl not support in stretch=1 mode\n");
            }
            else if(_fb_front_buf == _fb_bufs[2])
            {
                // printk("Flip to 0x%8x\n", _fb1_bufs[0]);
                CHECK_FAIL(OSD_PLA_FlipTo(ANDROID_OSD, rgn_list_2), 0);
            }

            // Swap role of back & front
            if(_fb_back_buf == _fb_bufs[0])
            {
                _fb_front_buf = _fb_bufs[0];
                _fb_back_buf = _fb_bufs[2];
            }
            else
            {
                _fb_front_buf = _fb_bufs[2];
                _fb_back_buf = _fb_bufs[0];
            }

            // Indicates swaping done.
            _fb_wins[i].swaping = 0;
            _fb_wins[i == 0?1:0].dirty = 0;
            VERIFY(x_sema_unlock(_h_fb_mutex) == OSR_OK);
#else
            CHECK_FAIL(OSD_PLA_FlipTo(ANDROID_OSD, rgn_list), 0);
#endif /* FB1_ENABLE_COMPOSER */
            break;
        }

        /* Removing pid from compose list */
        case 0x4658:
        {
            INT32 i;
            fbdev_ipc fb_ipc;
            if(copy_from_user((void*)&fb_ipc, pvArg, sizeof(fbdev_ipc)))
            {
                printk(KERN_WARNING "mtk fb 1 blit error: Copy from user error!\n");
                return -EFAULT;
            }
            VERIFY(x_sema_lock(_h_fb_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);
            // Search for entry
            if((i = _fb_find_win(_fb_wins, fb_ipc.pid)) < 0)
            {
                VERIFY(x_sema_unlock(_h_fb_mutex) == OSR_OK);
                return -EFAULT;
            }

            // Reset pid
            _fb_wins[i].pid = -1;
            _fb_wins[i].copying = 0;
            _fb_wins[i].swaping = 0;
            _fb_wins[i].dirty = 0;
            _fb_num_wins=(_fb_num_wins==0?0:_fb_num_wins-1);

            VERIFY(x_sema_unlock(_h_fb_mutex) == OSR_OK);
            break;
        }

        /* Get all pids */
        case 0x4659:
        {
            INT32 pids[ANDROID_MAX_NUM_OF_WINS];

            VERIFY(x_sema_lock(_h_fb_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);
            pids[0] = _fb_wins[0].pid;
            pids[1] = _fb_wins[1].pid;
            VERIFY(x_sema_unlock(_h_fb_mutex) == OSR_OK);
            if(copy_to_user(pvArg, (void*)pids, sizeof(INT32) * ANDROID_MAX_NUM_OF_WINS))
            {
                printk(KERN_WARNING "mtk fb 0x4659 ioctl: Copy from user error!\n");
                return -EFAULT;
            }
            break;
        }
#endif /* CC_SUPPORT_MULTIPLE_FBDEV */
        case FBIO_WAITFORVSYNC:
            fb_osd_wait_vsync();
            break;
        #if defined(ANDROID)
        case FBIO_SET_SIMPLE_CMD:
            mt53simple_ioctl((UINT32)pvArg);
            break;
        #endif
    #if 1//defined(CC_MTK_ION_SUPPORT)
     case FBIO_PLA_SET:
        {
            MT53_PLA_SET set;
            BOOL bEnable;
            
            if(copy_from_user((void*)&set, pvArg, sizeof(MT53_PLA_SET)))
            {
                printk(KERN_WARNING "mtk fb 1 FBIO_PLA_SET error: Copy from user error!\n");
                return -EFAULT;
            }
            printk("set osd=%d enbale=%d\n", set.u4Plane, bEnable);
            OSD_PLA_GetEnable(set.u4Plane, &bEnable);
            
            if(bEnable != set.u4Enable)
            {       
                OSD_PLA_Enable(set.u4Plane, set.u4Enable);
                if(set.u4Plane == OSD_PLANE_2)
                {
                    _tHWCOSDInfo.u4UserEnable = set.u4Enable;
                }
            }
        }
        break;
    #endif
#ifdef CC_SUPPORT_HW_CURSOR
    case FBIOSET_HWCURSOR_SETTING:
            _mt53fb_hwcursor_setting(pvArg);
            break;            
#endif
#ifdef CC_SUPPORT_DYNAMIC_ENABLE_DFB_INPUTEVENT
        case MTKIO_SET_DFBDISCARDKEY:
            {
                down(&_gtDFBKeyInfo.tSem);
                if(copy_from_user(&_gtDFBKeyInfo.u4IsDiscard, pvArg, sizeof(UINT32))){
                    printk(KERN_WARNING "misc_ioctl() - copy_from_user() fail\n");
                    return -EFAULT;
                }
                up(&_gtDFBKeyInfo.tSem);
            }
            break;
        case MTKIO_CHECK_DFBDISCARDKEY:
            {
                return dfbkey_check(pvArg);
            }
            break;
        case MTKIO_DUMP_DFBKEY:
            {
                MTK_KEY_NODE_T *ptNode;
                struct list_head *l;            
                Printf("KEY >>>>>>>>>>>>>>>>>>>>>>>>>>>>>> \n");
                down(&_gtDFBKeyInfo.tSem);
                Printf("DFBKey Disacard = %d \n", _gtDFBKeyInfo.u4IsDiscard);
                list_for_each(l, &_gtDFBKeyInfo.tList)
                {
                    ptNode = list_entry(l, MTK_KEY_NODE_T, list);
                    Printf("Node token[%s], keysymbol[0x%x] \n", ptNode->ucToken, ptNode->u4KeySymbol);                             
                }
                up(&_gtDFBKeyInfo.tSem);
                Printf("<<<<<<<<<<<<<<<<<<<<<<< \n");
            }
            break;
#endif
        default:
        //printk(KERN_WARNING "_mt53fb_ioctl(): not support ioctl cmd %d\n", cmd);
        i4Ret = -EINVAL;
        break;
    }
    return i4Ret;
}

static struct fb_ops mtk_fb_ops =
{
    .owner = THIS_MODULE,
    .fb_check_var = mtk_fb_check_var,
    .fb_set_par = mtk_fb_set_par,
    .fb_setcolreg = mtk_fb_setcolreg,
    .fb_pan_display = mtk_fb_pan_display,
#ifdef FB_DRIVER_IMPROVE
    .fb_blank = mtk_fb_blank,
#endif

    /* These are generic software based fb functions */
    .fb_fillrect = cfb_fillrect,
    .fb_copyarea = cfb_copyarea,
    .fb_imageblit = cfb_imageblit,
    .fb_ioctl = _mtk_fb_ioctl,
};

static int mtk_fb_setup(void)
{
    int ret;

    printk("MTK FB for Android mtk_fb_probe\n");
    fb = kzalloc(sizeof(*fb), GFP_KERNEL);
    if(fb == NULL)
    {
        ret = -ENOMEM;
        goto err_fb_alloc_failed;
    }

#ifdef CC_SUPPORT_MULTIPLE_FBDEV
    if(_h_fb_mutex == 0)
    {
        x_sema_create(&_h_fb_mutex, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK);
    }
#else
    width = ((osd2_src_width==0)?width:osd2_src_width);
    height = ((osd2_src_height==0)?height:osd2_src_height);

#ifdef CC_COMBINE_4K_2K
    {
        UINT32 ui4_val = 0;
        INT32 i4_ret = 0;
        i4_ret = DRVCUST_InitQuery(eGet2KModelSupport, &ui4_val);
        printk("mt53fb: Query 2K/4K model. i4_ret=%d, ui4_val=%d\n", i4_ret, ui4_val);

        if (i4_ret == 0)
        {
            if ( (ui4_val & 0x1)==0x00)
            {
                width=1920;
                height=1080;
                printk("mt53fb: COMBINE_4K_2K  2K Model. enforce width,height = 1920x1080.\n");
            }
            else
            {
                width=3840; 
                height=2160;
                printk("mt53fb: COMBINE_4K_2K  4K Model. enforce width,height = 3840x2160.\n");
            }

        }
    }
#endif

#endif /* CC_SUPPORT_MULTIPLE_FBDEV */

    fb->fb.fbops = &mtk_fb_ops;

    /* These modes are the ones currently required by Android */

    fb->fb.flags = FBINFO_FLAG_DEFAULT;
    fb->fb.pseudo_palette = fb->cmap;
    fb->fb.fix.type = FB_TYPE_PACKED_PIXELS;
    fb->fb.fix.visual = FB_VISUAL_TRUECOLOR;
    fb->fb.fix.line_length = drv->get_line_length(width, ANDROID_BITS_PER_PIXEL);
    fb->fb.fix.accel = FB_ACCEL_NONE;
    fb->fb.fix.ypanstep = 1;

#ifdef FB_DRIVER_MATCH_PANEL_IMPROVE
    fb->fb.var.left_margin = wDrvGetHsyncFp();
    fb->fb.var.right_margin = wDrvGetHsyncBp();
    fb->fb.var.hsync_len = wDrvGetOutputHTotal()-wDrvGetOutputHActive()-wDrvGetHsyncFp()-wDrvGetHsyncBp();
    fb->fb.var.xres = wDrvGetOutputHActive();

    fb->fb.var.upper_margin = wDrvGetVsyncFp();
    fb->fb.var.lower_margin = wDrvGetVsyncBp();
    fb->fb.var.vsync_len = wDrvGetOutputVTotal()-wDrvGetOutputVActive()-wDrvGetVsyncFp()-wDrvGetVsyncBp();
    fb->fb.var.yres = wDrvGetOutputVActive();

    fb->fb.var.pixclock = KHZ2PICOS(wDrvGetOutputHTotal()*wDrvGetOutputVTotal()*vDrvGetLCDFreq()/1000);
#else
    fb->fb.var.xres = width;
    fb->fb.var.yres = height;
#endif

    fb->fb.var.xres = width;
    fb->fb.var.yres = height;
    fb->fb.var.xres_virtual = width;
    fb->fb.var.yres_virtual = height * ANDROID_NUMBER_OF_BUFFERS;
    fb->fb.var.bits_per_pixel = ANDROID_BITS_PER_PIXEL;
    fb->fb.var.activate = FB_ACTIVATE_NOW;
    fb->fb.var.height = height;
    fb->fb.var.width = width;
    fb->fb.var.vmode = FB_VMODE_NONINTERLACED;

#if defined(ANDROID_FB_CM_ARGB8888) || defined(CC_SUPPORT_MULTIPLE_FBDEV) || defined(CC_LINUX_GPU_SUPPORT)
    fb->fb.var.red.offset = 16;
    fb->fb.var.red.length = 8;
    fb->fb.var.green.offset = 8;
    fb->fb.var.green.length = 8;
    fb->fb.var.blue.offset = 0;
    fb->fb.var.blue.length = 8;
    fb->fb.var.transp.offset = 24;
    fb->fb.var.transp.length = 8;

    ret = drv->setup_buffer(
            width,
            height,
            fb->fb.var.yres_virtual,
            ANDROID_BITS_PER_PIXEL,
            MTK_DRV_FB_CM_ARGB8888,
            (void **)&fb->fb.screen_base,
            (unsigned long **)&fb->fb.fix.smem_start,
            (unsigned long **)&fb->fb.fix.smem_len);
#else
    fb->fb.var.red.offset = 11;
    fb->fb.var.red.length = 5;
    fb->fb.var.green.offset = 5;
    fb->fb.var.green.length = 6;
    fb->fb.var.blue.offset = 0;
    fb->fb.var.blue.length = 5;

    ret = drv->setup_buffer(
            width,
            height,
            fb->fb.var.yres_virtual,
            ANDROID_BITS_PER_PIXEL,
            MTK_DRV_FB_CM_RGB565,
            (void **)&fb->fb.screen_base,
            (unsigned long **)&fb->fb.fix.smem_start,
            (unsigned long **)&fb->fb.fix.smem_len);
#endif /* defined(ANDROID_FB_CM_ARGB8888) || defined(CC_SUPPORT_MULTIPLE_FBDEV) */

    if(ret)
    goto err_fb_drv_failed;

    ret = fb_set_var(&fb->fb, &fb->fb.var);
    if(ret)
    goto err_fb_set_var_failed;

    ret = register_framebuffer(&fb->fb);
    if(ret)
    goto err_register_framebuffer_failed;
    printk("MTK FB for Android probe successful\n");
    return 0;

    err_register_framebuffer_failed:
    err_fb_set_var_failed:
    err_fb_drv_failed:
    kfree(fb);
    err_fb_alloc_failed:
    printk("MTK FB for Android probe failed\n");
    return ret;
}

static void _fb_exit(void)
{
    /* TODO:  Release all resources .... */
}

static UINT32 _get_drv_cm(u32 fb_cm)
{
    switch(fb_cm)
    {
        case MTK_DRV_FB_CM_RGB565:
        return OSD_CM_RGB565_DIRECT16;
        case MTK_DRV_FB_CM_ARGB8888:
        return OSD_CM_ARGB8888_DIRECT32;
        default:
        printk("NOT SUPPORT CM\n");
    }

    return -1;
}

static int _setup_buffer(
        u32 xres,
        u32 yres,
        u32 vyres,
        u32 bits_per_pix,
        u32 cm,
        void ** p_screen_base,
        unsigned long ** p_phy_start,
        unsigned long ** p_size)
{
    UINT32 pw;
    UINT32 ph;

#ifdef CC_SUPPORT_MULTIPLE_FBDEV
    // From DFB partition offset 0
    //*p_phy_start = (BSP_GetFbmMemAddr() - DRVCUST_OptGet(eDirectFBMemSize)) & PAGE_MASK;
    // Share with DDI_GPU POSD2
#ifdef POSD1_FHD
    *p_phy_start = (unsigned long*)((fb_osd_get_fb_addr() + (1280 * 720 * 4 + 1920 * 1080 * 4) * 4) & PAGE_MASK);
#else
    *p_phy_start = (unsigned long*)((fb_osd_get_fb_addr() + (1280 * 720 * 4 + 1280 * 720 * 4) * 4) & PAGE_MASK);
#endif /* POSD1_FHD */
#else
    *p_phy_start =(unsigned long *) ((fb_osd_get_fb_addr()) & PAGE_MASK);
#endif /* CC_SUPPORT_MULTIPLE_FBDEV */

    /* We Don't need too much memory as DTV mw.*/
#ifdef CC_SUPPORT_MULTIPLE_FBDEV
    *p_size = (unsigned long*)(vyres * _get_line_length(xres, bits_per_pix));
#else
#ifdef CC_SUPPORT_4K2K_UI
    *p_size =(unsigned long *)(fb_osd_get_fb_size()  - DRVCUST_OptGet(eGfxFBPartitionSize) - GPU_ION_MEM_SIZE); //for two osd plane display android UI(Improve performance)
#else
    *p_size =(unsigned long *)(fb_osd_get_fb_size()  - DRVCUST_OptGet(eGfxFBPartitionSize));
#endif
#endif /* CC_SUPPORT_MULTIPLE_FBDEV */
    /* USE DTV-FBM convertion or use ioremap, check whether the core.c mapped
     * FB memory when system init.
     * */
    *p_screen_base = ioremap((long unsigned int)*p_phy_start, (size_t)*p_size);
    printk("mt5395 fb test get buffer :scr base:0x%x, phy start:0x%x, size %d, w=%d, h=%d\n",
            *p_screen_base,
            *p_phy_start,
            *p_size, xres, yres);

    // memset((void *)*p_screen_base, 0x00, (SIZE_T)*p_size);
    //GFX_MemsetAsync((UINT8 *)*p_screen_base, xres, vyres, 0x0);
    GFX_MemsetAsync((UINT8 *)*p_phy_start, ((GFX_DST_PITCH_MAX&0xF000)/4),  (((SIZE_T)(*p_size))/(GFX_DST_PITCH_MAX&0xF000)), 0x0);
#if defined(CC_MTK_ION_SUPPORT)
u4CarveoutAddr = (UINT32)(*p_phy_start) + (UINT32) *p_size;
u4CarveoutSize = GPU_ION_MEM_SIZE;

Printf(" mt5395 fb carvoutAddr [0x%x---0x%x] \n", u4CarveoutAddr, u4CarveoutAddr + u4CarveoutSize);
#endif   
#ifdef CC_ANDROID_3D_UI_SUPPORT
    u4UiWidth = xres;
    u4UiHeight = yres;
    OSD_RGN_Create(&_u43DUiRgn, xres, yres, *p_phy_start,
            _get_drv_cm(cm), xres * bits_per_pix / 8,
            0, 0, xres, yres);
#endif
    CHECK_FAIL(OSD_RGN_LIST_Create(&rgn_list), 0);

#ifdef CC_SUPPORT_MULTIPLE_FBDEV
    CHECK_FAIL(OSD_RGN_Create(&rgn, 16, 16,
                    *p_phy_start,
                    _get_drv_cm(cm),
                    _get_line_length(xres, ANDROID_BITS_PER_PIXEL),
                    0, 0, 16, 16), 0);
#else
    CHECK_FAIL(OSD_RGN_Create(&rgn, xres, yres,
                    *p_phy_start,
                    _get_drv_cm(cm),
                    xres * bits_per_pix / 8,
                    0, 0, xres, yres), 0);
#endif /* CC_SUPPORT_MULTIPLE_FBDEV */

    CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_BIG_ENDIAN, 0), 0);
   #if defined(CC_MT5890)
       if(!IS_IC_5861())
   	  {
	    //for T624 GPU workaround
	        IGNORE_RET(_OSD_RGN_SetASel(rgn, 3));
	        IGNORE_RET(_OSD_RGN_SetYrSel(rgn, 0));
	        IGNORE_RET(_OSD_RGN_SetUgSel(rgn, 1));
	        IGNORE_RET(_OSD_RGN_SetVbSel(rgn, 2));
   	 }
  #else
    CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_BIG_ENDIAN, 0), 0);
  #endif

    CHECK_FAIL(OSD_RGN_Insert(rgn, rgn_list), 0);

    /* TODO: Which plane?*/
    CHECK_FAIL(OSD_PLA_FlipTo(ANDROID_OSD, rgn_list), 0);

    CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_MIX_SEL, OSD_BM_PIXEL), 0);

    CHECK_FAIL(OSD_RGN_LIST_Create(&rgn_list_2), 0);

#ifdef CC_SUPPORT_MULTIPLE_FBDEV
    CHECK_FAIL(OSD_RGN_Create(&rgn_2, 16, 16,
                    *p_phy_start + ANDROID_FB_HEIGHT * _get_line_length(ANDROID_FB_WIDTH, ANDROID_BITS_PER_PIXEL),
                    _get_drv_cm(cm),
                    _get_line_length(xres, ANDROID_BITS_PER_PIXEL),
                    0, 0, 16, 16), 0);
#else
    CHECK_FAIL(OSD_RGN_Create(&rgn_2, xres, yres,
                    *p_phy_start,
                    _get_drv_cm(cm),
                    xres * bits_per_pix / 8,
                    0, 0, xres, yres), 0);
#endif /* CC_SUPPORT_MULTIPLE_FBDEV */

    CHECK_FAIL(OSD_RGN_Set(rgn_2, OSD_RGN_BIG_ENDIAN, 0), 0);

    CHECK_FAIL(OSD_RGN_Insert(rgn_2, rgn_list_2), 0);

    CHECK_FAIL(OSD_RGN_Set(rgn_2, OSD_RGN_MIX_SEL, OSD_BM_PIXEL), 0);

#ifdef CC_SUPPORT_MULTIPLE_FBDEV
    _fb_bufs[0] = (UINT32)*p_phy_start;
    _fb_bufs[1] = (UINT32)(*p_phy_start) + 1280 * 720 * 4;

#if ANDROID_ENABLE_COMPOSER
    if(_fb_bufs[2] == 0)
    {
        _fb_bufs[2] = (UINT32)BSP_AllocAlignedDmaMemory(ANDROID_FB_HEIGHT * _get_line_length(ANDROID_FB_WIDTH, ANDROID_BITS_PER_PIXEL), 4096);
        VERIFY(_fb_bufs[2] != 0);
        x_memset(VIRTUAL((void*)_fb_bufs[2]), 0, ANDROID_FB_HEIGHT * _get_line_length(ANDROID_FB_WIDTH, ANDROID_BITS_PER_PIXEL));
    }
#else
    _fb_bufs[2] = (UINT32)*p_phy_start;
#endif /* ANDROID_ENABLE_COMPOSER */

    _fb_front_buf = _fb_bufs[0];
    _fb_back_buf = _fb_bufs[2];
#endif /* CC_SUPPORT_MULTIPLE_FBDEV */

    pw = OSD_BASE_GetDisplayWidth();
    ph = OSD_BASE_GetDisplayHeight();

    /* TODO: Which scaler:*/
#ifdef CC_SUPPORT_MULTIPLE_FBDEV
    CHECK_FAIL(OSD_SC_Scale(ANDROID_OSD_SCALER, TRUE, xres, yres, OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayWidth()), 0);
#else
#ifdef CC_SUPPORT_4K2K_UI
    CHECK_FAIL(OSD_SC_Scale(ANDROID_OSD_SCALER, TRUE, (xres==3840)?1920:xres, (yres==2160)?1080:yres,
                    (osd2_output_width==0)?pw:osd2_output_width,
                    (osd2_output_height==0)?ph:osd2_output_height), 0);
#else
    CHECK_FAIL(OSD_SC_Scale(ANDROID_OSD_SCALER, TRUE, xres, yres,
                    (osd2_output_width==0)?pw:osd2_output_width,
                    (osd2_output_height==0)?ph:osd2_output_height), 0);
#endif

    /* */

    printk("osd2 src_w: %d, src_h: %d, out_w: %d, out_h: %d\n",
            xres, yres, (osd2_output_width==0)?pw:osd2_output_width, (osd2_output_height==0)?ph:osd2_output_height);

    OSD_PLA_Enable(ANDROID_OSD, TRUE);

    // removed by polar
#ifdef CONFIG_ANDROID_PMEM
    mtk_pmem_set_memory_regions((u32)((BSP_GetFbmMemAddr() - DRVCUST_OptGet(eDirectFBMemSize)) & PAGE_MASK), (u32)DRVCUST_OptGet(eDirectFBMemSize));
    printk("mt53_fb: 0x%08X, 0x%08X\n", (u32)((BSP_GetFbmMemAddr() - DRVCUST_OptGet(eDirectFBMemSize)) & PAGE_MASK), (u32)DRVCUST_OptGet(eDirectFBMemSize));
#endif
#endif /* CC_SUPPORT_MULTIPLE_FBDEV */


    return 0;
}

static int _set_base(unsigned long phy_scr_base, unsigned long offset)
{
#ifndef CC_SUPPORT_MULTIPLE_FBDEV
#ifdef CC_ANDROID_3D_UI_SUPPORT
    //UINT32 u4BmpWidth =0, u4Pitch=0, u4DispWidth =0;
    //BOOL fgStatus = 0;
    if(u4UiMode == OSD_3D_SBSHALF_PR)
    {
        if(!fgUpdateAttr)
        {
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_BMP_W, u4UiWidth), 0);
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_DISP_W, u4UiWidth), 0);
           if(u4UiHeight <= OSD_BASE_GetDisplayHeight())
            {
             CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_STEP_V, 0x2000), 0);
            }
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_BMP_PITCH, _get_line_length(u4UiWidth, 32)), 0);
            OSD_SC_GetScalerInfo(OSD_SCALER_2, &u4OriSc2_Enable,
                    &u4OriSc2SrcW, &u4OriSc2SrcH,
                    &u4OriSc2DstW, &u4OriSc2DstH,
                    &u4OriSc2Is16Bpp);
            OSD_SC_Scale(ANDROID_OSD_SCALER, TRUE, u4UiWidth/2, u4UiHeight, OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayHeight());
            OSD_SC_Scale(OSD_SCALER_2, TRUE, u4UiWidth/2, u4UiHeight, OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayHeight());

            u4Plane2RegionList = OSD_PLA_GetFirstRegionList(OSD_PLANE_2);
            OSD_PLA_GetEnable(OSD_PLANE_2, &u4Plane2Enabel);
            OSD_PLA_Set3DSwitchRegion(rgn, _u43DUiRgn);
            OSD_PLA_Enable(OSD_PLANE_2, TRUE);
            OSD_3D_SetPlaneSwitch(TRUE);
            fgUpdateAttr = 1;
        }

        OSD_PLA_Set3DSwitchRgnAddr(rgn, _u43DUiRgn, (UINT32)phy_scr_base, (UINT32)(phy_scr_base + _get_line_length(u4UiWidth/2, 32)));
    }
    else if(u4UiMode == OSD_3D_TPANDBT_PR)
    {
        if(!fgUpdateAttr)
        {
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_BMP_W, u4UiWidth), 0);
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_DISP_W, u4UiWidth), 0);
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_STEP_V, 0x1000), 0);
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_BMP_PITCH, _get_line_length(u4UiWidth, 32)), 0);
            OSD_SC_GetScalerInfo(OSD_SCALER_2, &u4OriSc2_Enable,
                    &u4OriSc2SrcW, &u4OriSc2SrcH,
                    &u4OriSc2DstW, &u4OriSc2DstH,
                    &u4OriSc2Is16Bpp);
            OSD_SC_Scale(ANDROID_OSD_SCALER, TRUE, u4UiWidth, u4UiHeight, OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayHeight());
            OSD_SC_Scale(OSD_SCALER_2, TRUE, u4UiWidth, u4UiHeight, OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayHeight());

            u4Plane2RegionList = OSD_PLA_GetFirstRegionList(OSD_PLANE_2);
            OSD_PLA_GetEnable(OSD_PLANE_2, &u4Plane2Enabel);
            OSD_PLA_Set3DSwitchRegion(rgn, _u43DUiRgn);
            OSD_PLA_Enable(OSD_PLANE_2, TRUE);
            OSD_3D_SetPlaneSwitch(TRUE);
            fgUpdateAttr = 1;
        }

        OSD_PLA_Set3DSwitchRgnAddr(rgn, _u43DUiRgn, (UINT32)phy_scr_base, (UINT32)(phy_scr_base + _get_line_length(u4UiWidth, 32)*u4UiHeight/2));
    }
    else if(u4UiMode == OSD_3D_SBSHALF_SG)
    {
        if(!fgUpdateAttr)
        {
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_BMP_W, u4UiWidth), 0);
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_DISP_W, u4UiWidth), 0);
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_STEP_V, 0x1000), 0);
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_BMP_PITCH, _get_line_length(u4UiWidth, 32)), 0);
            OSD_SC_GetScalerInfo(OSD_SCALER_2, &u4OriSc2_Enable,
                    &u4OriSc2SrcW, &u4OriSc2SrcH,
                    &u4OriSc2DstW, &u4OriSc2DstH,
                    &u4OriSc2Is16Bpp);
            OSD_SC_Scale(ANDROID_OSD_SCALER, TRUE, u4UiWidth/2, u4UiHeight, OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayHeight());
            u4Plane2RegionList = OSD_PLA_GetFirstRegionList(OSD_PLANE_2);
            OSD_PLA_GetEnable(OSD_PLANE_2, &u4Plane2Enabel);
            OSD_PLA_Set3DRegion(ANDROID_OSD, rgn,_u43DUiRgn );
            OSD_PLA_Set3DMode(ANDROID_OSD, TRUE);
            fgUpdateAttr = 1;
        }
        OSD_PLA_Set3DSwitchRgnAddr(rgn, _u43DUiRgn, (UINT32)phy_scr_base, (UINT32)(phy_scr_base + _get_line_length(u4UiWidth/2, 32)));
    }
    else if(u4UiMode == OSD_3D_TPANDBT_SG)
    {
        if(!fgUpdateAttr)
        {
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_BMP_W, u4UiWidth), 0);
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_DISP_W, u4UiWidth), 0);
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_STEP_V, 0x1000), 0);
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_BMP_PITCH, _get_line_length(u4UiWidth, 32)), 0);
            OSD_SC_GetScalerInfo(OSD_SCALER_2, &u4OriSc2_Enable,
                    &u4OriSc2SrcW, &u4OriSc2SrcH,
                    &u4OriSc2DstW, &u4OriSc2DstH,
                    &u4OriSc2Is16Bpp);
            OSD_SC_Scale(ANDROID_OSD_SCALER, TRUE, u4UiWidth, u4UiHeight/2, OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayHeight());
            u4Plane2RegionList = OSD_PLA_GetFirstRegionList(OSD_PLANE_2);
            OSD_PLA_GetEnable(OSD_PLANE_2, &u4Plane2Enabel);
            OSD_PLA_Set3DRegion(ANDROID_OSD,rgn,_u43DUiRgn );
            OSD_PLA_Set3DMode(ANDROID_OSD, TRUE);
            fgUpdateAttr = 1;
        }
        OSD_PLA_Set3DSwitchRgnAddr(rgn, _u43DUiRgn, (UINT32)phy_scr_base, (UINT32)(phy_scr_base + _get_line_length(u4UiWidth, 32)*u4UiHeight/2));
    }
    else
    {
        if(!fgUpdateAttr)
        {
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_BMP_W, u4UiWidth), 0);
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_DISP_W, u4UiWidth), 0);
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_STEP_V, 0x1000), 0);
            CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_BMP_PITCH, _get_line_length(u4UiWidth, 32)), 0);
            OSD_3D_SetPlaneSwitch(FALSE);
            OSD_PLA_Set3DMode(ANDROID_OSD, FALSE);
            OSD_SC_Scale(ANDROID_OSD_SCALER, TRUE, u4UiWidth, u4UiHeight, OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayHeight());
            fgUpdateAttr = 1;
        }
    #if defined(CC_MTK_ION_SUPPORT)
     if(fgHwcEn)
     {
        OSD_PLA_FlipTo(_u4HWCAddPlane, _u4HWCFBReginlist) ;
        //OSD_PLA_Enable(_u4HWCAddPlane, TRUE);
        fgHwcEn = 0;
     }
     #endif
        CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_BMP_ADDR, phy_scr_base), 0);
    }

#else
    CHECK_FAIL(OSD_RGN_Set(rgn, OSD_RGN_BMP_ADDR, phy_scr_base), 0);
    fb_osd_wait_vsync();
#endif
#else
    UINT32 u4BmpAddr;
    //    printk("Before 0x%8x\n", phy_scr_base);
    if(phy_scr_base == _fb_bufs[0])
    {
        //printk("Flip to 0x%8x\n", _fb_bufs[0]);
        OSD_RGN_Get((UINT32)rgn, OSD_RGN_BMP_ADDR, &u4BmpAddr);
        if((u4BmpAddr & 0x3FFFFFFF) != _fb_bufs[0])
        {
            // printk("_fb_bufs[0] being altered! Set back to 0x%8x\n", _fb_bufs[0]);
            OSD_RGN_Set((UINT32)rgn, OSD_RGN_BMP_ADDR, _fb_bufs[0]);
        }
        CHECK_FAIL(OSD_PLA_FlipTo(ANDROID_OSD, rgn_list), 0);
    }
    else if(phy_scr_base == _fb_bufs[1])
    {
        //printk("Flip to 0x%8x\n", _fb_bufs[1]);
        OSD_RGN_Get((UINT32)rgn_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
        if((u4BmpAddr & 0x3FFFFFFF) != _fb_bufs[1])
        {
            // printk("_fb_bufs[1] being altered! Set back to 0x%8x\n", _fb_bufs[1]);
            OSD_RGN_Set((UINT32)rgn_2, OSD_RGN_BMP_ADDR, _fb_bufs[1]);
        }
        CHECK_FAIL(OSD_PLA_FlipTo(ANDROID_OSD, rgn_list_2), 0);
    }
    else if(phy_scr_base == _fb_bufs[2])
    {
        //printk("Flip to 0x%8x\n", _fb_bufs[2]);
        OSD_RGN_Get((UINT32)rgn_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
        if((u4BmpAddr & 0x3FFFFFFF) != _fb_bufs[2])
        {
            // printk("_fb_bufs[2] being altered! Set back to 0x%8x\n", _fb_bufs[2]);
            OSD_RGN_Set((UINT32)rgn_2, OSD_RGN_BMP_ADDR, _fb_bufs[2]);
        }
        CHECK_FAIL(OSD_PLA_FlipTo(ANDROID_OSD, rgn_list_2), 0);
    }
#endif /* CC_SUPPORT_MULTIPLE_FBDEV */
    return 0;
}

static mtk_drv_fb_t mt5395_fb =
{
    .fb_exit = _fb_exit,
    .get_line_length = _get_line_length,
    .setup_buffer = _setup_buffer,
    .set_base = _set_base,
};

int mtk_drv_fb_init(mtk_drv_fb_t ** drv)
{
#if 0
    PMX_Init();
    OSD_Init();
    GFX_Init();

    GPIO_Init();
    EEPROM_Init();
    FBM_Init();
    LoadPanelIndex();
    PDWNC_Init();
    VDEC_Init(); //???
    VDP_Init();
    VdoMLInit();
    SIF_Init();
    VDP_SetScalerPixelBased(1);
    RTC_Init();

    vApiPanelPowerSequence(1);
    OSD_Reset();
#endif
#ifndef CC_SUPPORT_MULTIPLE_FBDEV
    OSD_PLA_Enable(ANDROID_OSD, FALSE);
#endif /* CC_SUPPORT_MULTIPLE_FBDEV */
    /*OSD_PLA_Enable(2, TRUE);*/

    *drv = &mt5395_fb;
    return 0;
}

#endif

#ifdef CC_SUPPORT_MULTIPLE_FBDEV
#ifdef HAS_DEV_FB1
#define CHECK_FAIL(stmt, exp_ret) \
do {\
    int _____ret = (stmt);\
    if(_____ret != (exp_ret))\
    {\
        printk("CHECK FAILED %s ret:%d\n", #stmt, _____ret);\
    }\
}while(0)

#define FB1_OSD OSD_PLANE_3
#define FB1_OSD_SCALER OSD_SCALER_3

#define FB1_NUMBER_OF_BUFFERS 2
#define FB1_BITS_PER_PIXEL 32
/* Modify these macros to suit the hardware */

#ifdef POSD1_FHD
#define FB1_WIDTH 1920
#define FB1_HEIGHT 1080
#else
#define FB1_WIDTH 1280
#define FB1_HEIGHT 720
#endif

#define FB1_ENABLE_COMPOSER 1
#define FB1_MAX_NUM_OF_WINS 2
#define FB1_FORCE_APPLY_REGION_SRC_DST_RECT 0

UINT32 fb1_width = FB1_WIDTH;
UINT32 fb1_height = FB1_HEIGHT;

static mtk_drv_fb_t * drv_1;
struct mtk_fb *fb_1;

static int _fb_1_set_base(unsigned long phy_scr_base, unsigned long offset);

/* set the software color map.  Probably doesn't need modifying. */
static int
mtk_fb_1_setcolreg(unsigned int regno, unsigned int red, unsigned int green,
        unsigned int blue, unsigned int transp, struct fb_info *info)
{
    struct mtk_fb *fb = container_of(info, struct mtk_fb, fb);

    if (regno < 16)
    {
        fb->cmap[regno] = convert_bitfield(transp, &fb->fb.var.transp) |
        convert_bitfield(blue, &fb->fb.var.blue) |
        convert_bitfield(green, &fb->fb.var.green) |
        convert_bitfield(red, &fb->fb.var.red);
        return 0;
    }
    else
    {
        return 1;
    }
}

/* check var to see if supported by this device.  Probably doesn't
 * need modifying.
 */
static int mtk_fb_1_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
    if((var->rotate & 1) != (info->var.rotate & 1))
    {
        if((var->xres != info->var.yres) ||
                (var->yres != info->var.xres) ||
                (var->xres_virtual != info->var.yres) ||
                (var->yres_virtual >
                        info->var.xres * FB1_NUMBER_OF_BUFFERS * 2) ||
                (var->yres_virtual < info->var.xres ))
        {
            return -EINVAL;
        }
    }
    else
    {
        if((var->xres != info->var.xres) ||
                (var->yres != info->var.yres) ||
                (var->xres_virtual != info->var.xres) ||
                (var->yres_virtual >
                        info->var.yres * FB1_NUMBER_OF_BUFFERS * 2) ||
                (var->yres_virtual < info->var.yres ))
        {
            return -EINVAL;
        }
    }
    if((var->xoffset != info->var.xoffset) ||
            (var->bits_per_pixel != info->var.bits_per_pixel) ||
            (var->grayscale != info->var.grayscale))
    {
        return -EINVAL;
    }
    return 0;
}

/* Handles screen rotation if device supports it. */
static int mtk_fb_1_set_par(struct fb_info *info)
{
    struct mtk_fb *fb_1 = container_of(info, struct mtk_fb, fb);
    if(fb->rotation != fb->fb.var.rotate)
    {
        info->fix.line_length = drv_1->get_line_length(info->var.xres, FB1_BITS_PER_PIXEL);
        fb->rotation = fb->fb.var.rotate;
    }
    UNUSED(fb_1);
    return 0;
}

/* Pan the display if device supports it. */
static int mtk_fb_1_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
{
#if 0
    struct mtk_fb *fb_1 __attribute__ ((unused))
    = container_of(info, struct mtk_fb, fb);
    drv_1->set_base(fb->fb.fix.smem_start + fb->fb.var.xres *
            4 * var->yoffset, var->yoffset);
#else
    drv_1->set_base(info->fix.smem_start + var->yoffset * _get_line_length(info->var.xres, FB1_BITS_PER_PIXEL),
            var->yoffset);
#endif
    return 0;
}

static HANDLE_T _h_fb1_mutex = 0;

UINT32 fb1_rgn;
UINT32 fb1_rgn_list;
// double buffer
UINT32 fb1_rgn_2 = 0;
UINT32 fb1_rgn_list_2 = 0;
//static BOOL _fg_fb1_flip = FALSE;
static UINT32 _fb1_bufs[FB1_NUMBER_OF_BUFFERS+2] =
{   0, 0, 0, 0};

static UINT32 _fb1_num_wins = 0;
static UINT32 _fb1_front_buf = 0;
static UINT32 _fb1_back_buf = 0;
static mtk_fb_win_t _fb1_wins[2] =
{
    {   -1, 0, 0, 0, 0, 0, 0, 0},
    {   -1, 0, 0, 0, 0, 0, 0, 0}
};

static int _fb1_find_win(mtk_fb_win_t wins[], int pid)
{
    UINT32 i;
    for(i = 0; i < FB1_MAX_NUM_OF_WINS; i++)
    {
        if(wins[i].pid == pid)
        {
            break;
        }
    }
    if(i >= FB1_MAX_NUM_OF_WINS)
    {
        return -EFAULT;
    }
    return i;
}

static int _mtk_fb_1_set_ioctl(void* pvArg)
{
    int i4Ret = 0;
    //unsigned short u2PanelWidth;
    //unsigned short u2PanelHeight;
    UINT32 u4Region = fb1_rgn;

    struct mt53fb_set rSet;

    if(copy_from_user(&rSet, pvArg, sizeof(struct mt53fb_set)))
    {
        printk(KERN_WARNING "_mt53fb_set_ioctl() - copy_from_user() fail\n");
        return -EFAULT;
    }

    if(rSet.mask & MT53FB_SET_MASK_COLORKEYEN)
    {
        if(OSD_RGN_Set(u4Region, OSD_RGN_COLOR_KEY_EN, rSet.rSetting.u4ColorKeyEn) != OSD_RET_OK)
        {
            return -EINVAL;
        }
    }

    if(rSet.mask & MT53FB_SET_MASK_COLORKEY)
    {
        if(OSD_RGN_Set(u4Region, OSD_RGN_COLOR_KEY, rSet.rSetting.u4ColorKey) != OSD_RET_OK)
        {
            return -EINVAL;
        }
    }

    return i4Ret;
}

static int _mtk_fb_1_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg)
{
    void __user *pvArg = (void __user *)arg;
    INT32 i4Ret = 0;
    switch(cmd)
    {
        case FBIO_SET:
        i4Ret = _mtk_fb_1_set_ioctl(pvArg);
        break;
        case 0x4651:
        printk("Enable POSD: %d\n", FB1_OSD);
        OSD_PLA_Enable(FB1_OSD, TRUE);
        break;
        case 0x4652:
        printk("Disable POSD: %d\n", FB1_OSD);
        OSD_PLA_Enable(FB1_OSD, FALSE);
        break;
        case 0x4650:
        {
            struct fb_var_screeninfo var; /* Current var */

            if(copy_from_user((void*)&var, pvArg, sizeof(struct fb_var_screeninfo)))
            {
                printk(KERN_WARNING "mt53fb fbio set base() error: Copy from user error!\n");
                return -EFAULT;
            }

            _u2PanelWidth = OSD_BASE_GetDisplayWidth();
            _u2PanelHeight = OSD_BASE_GetDisplayHeight();

            if (IS_PANEL_PDP)
            {
                if(_u2PanelWidth == 1024)
                _u2PanelHeight = _u2PanelHeight/2;
            }

#ifdef OSD_DSC_PDP
            u4SrcPdpWidth = _u2PanelWidth;
            u4SrcPdpHeight = _u2PanelHeight;
#endif

            if(var.reserved[0])
            {
#ifdef OSD_DSC_PDP
                // If using 1024x768 PDP panel & window size > panel size, need IMGRZ scaling down
                if((_u2PanelWidth == 1024 && (var.reserved[1] > _u2PanelWidth)) || (_u2PanelHeight == 768 && (var.reserved[2] > _u2PanelHeight)))
                {
                    u4SrcPdpWidth = var.reserved[1];
                    u4SrcPdpHeight = var.reserved[2];
                    OSD_SC_Scale(FB1_OSD_SCALER, TRUE, _u2PanelWidth, _u2PanelHeight,
                            _u2PanelWidth, _u2PanelHeight);

                    /* For supporting MTOSD_PLA_GetSrcContentRegion to get width & height */
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_W, var.reserved[1]);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_H, var.reserved[2]);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_DISP_W, var.reserved[1]);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_DISP_H, var.reserved[2]);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_OUT_W, var.reserved[1]);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_OUT_H, var.reserved[2]);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_PITCH, _get_line_length(FB1_WIDTH, FB1_BITS_PER_PIXEL));
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_W, var.reserved[1]);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_H, var.reserved[2]);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_DISP_W, var.reserved[1]);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_DISP_H, var.reserved[2]);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_OUT_W, var.reserved[1]);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_OUT_H, var.reserved[2]);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_PITCH, _get_line_length(FB1_WIDTH, FB1_BITS_PER_PIXEL));
                }
                else
#endif
                {
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_W, var.reserved[1]);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_H, var.reserved[2]);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_DISP_W, var.reserved[1]);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_DISP_H, var.reserved[2]);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_OUT_W, var.reserved[1]);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_OUT_H, var.reserved[2]);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_POS_X, 0);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_POS_Y, 0);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_PITCH, _get_line_length(FB1_WIDTH, FB1_BITS_PER_PIXEL));
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_ADDR, _fb1_bufs[0]);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_W, var.reserved[1]);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_H, var.reserved[2]);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_DISP_W, var.reserved[1]);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_DISP_H, var.reserved[2]);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_OUT_W, var.reserved[1]);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_OUT_H, var.reserved[2]);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_POS_X, 0);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_POS_Y, 0);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_PITCH, _get_line_length(FB1_WIDTH, FB1_BITS_PER_PIXEL));
                    if(var.reserved[3] == 5)
                    {
                        i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_ADDR, _fb1_bufs[2]);
                    }
                    else
                    {
                        i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_ADDR, _fb1_bufs[1]);
                    }
                    OSD_SC_Scale(FB1_OSD_SCALER, TRUE, var.reserved[1], var.reserved[2],
                            _u2PanelWidth, _u2PanelHeight);
                }
            }
            else
            {

#ifdef OSD_DSC_PDP
                if(_u2PanelWidth == 1024 && _u2PanelHeight == 768)
                {
                    if((var.reserved[1] + var.reserved[3] > _u2PanelWidth) || (var.reserved[2] + var.reserved[4] > _u2PanelHeight))
                    {
                        // If using 1024x768 PDP panel & window size > panel size, not supported because it cannot fit in panel display
                        i4Ret = -1;
                    }
                    else
                    {
                        i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_W, _u2PanelWidth);
                        i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_H, _u2PanelHeight);
                        i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_DISP_W, _u2PanelWidth);
                        i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_DISP_H, _u2PanelHeight);
                        i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_OUT_W, _u2PanelWidth);
                        i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_OUT_H, _u2PanelHeight);
                        i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_POS_X, 0);
                        i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_POS_Y, 0);
                        i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_PITCH, _get_line_length(FB1_WIDTH, FB1_BITS_PER_PIXEL));
                        i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_ADDR, _fb1_bufs[0]);

                        i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_W, _u2PanelWidth);
                        i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_H, _u2PanelHeight);
                        i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_DISP_W, _u2PanelWidth);
                        i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_DISP_H, _u2PanelHeight);
                        i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_OUT_W, _u2PanelWidth);
                        i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_OUT_H, _u2PanelHeight);
                        i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_POS_X, 0);
                        i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_POS_Y, 0);
                        i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_PITCH, _get_line_length(FB1_WIDTH, FB1_BITS_PER_PIXEL));
                        i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_ADDR, _fb1_bufs[2]);
                        OSD_SC_Scale(FB1_OSD_SCALER, TRUE, _u2PanelWidth, _u2PanelHeight,
                                _u2PanelWidth, _u2PanelHeight);
                    }
                }
                else
#endif
                {
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_W, var.xres);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_H, var.yres);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_DISP_W, var.xres);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_DISP_H, var.yres);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_OUT_W, var.xres);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_OUT_H, var.yres);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_POS_X, 0);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_POS_Y, 0);
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_PITCH, _get_line_length(FB1_WIDTH, FB1_BITS_PER_PIXEL));
                    i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_ADDR, _fb1_bufs[0]);

                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_W, var.xres);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_H, var.yres);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_DISP_W, var.xres);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_DISP_H, var.yres);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_OUT_W, var.xres);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_OUT_H, var.yres);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_POS_X, 0);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_POS_Y, 0);
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_PITCH, _get_line_length(FB1_WIDTH, FB1_BITS_PER_PIXEL));
                    i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_ADDR, _fb1_bufs[2]);
                    OSD_SC_Scale(FB1_OSD_SCALER, TRUE, var.xres, var.yres,
                            _u2PanelWidth, _u2PanelHeight);
                }
            }

            /* Disable 3D stereoscopic mode */
            _u13DMode = 0;
            OSD_PLA_Set3DMode(FB1_OSD, 0);

            break;
        }

        case 0x4653:
        {
            fbdev_blit fb_blit;
            if(copy_from_user((void*)&fb_blit, pvArg, sizeof(fbdev_blit)))
            {
                printk(KERN_WARNING "mtk fb 1 blit error: Copy from user error!\n");
                return -EFAULT;
            }

            VERIFY(x_sema_lock(_h_fb1_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);

            GFX_Lock();

            GFX_SetSrc((UINT8 *)fb_blit.src_addr, CM_ARGB8888_DIRECT32, fb_blit.src_pitch);

            GFX_SetDst((UINT8 *)fb_blit.dst_addr, CM_ARGB8888_DIRECT32, fb_blit.dst_pitch);

            GFX_SetBltOpt((UINT32)D_GFXFLAG_NONE, 0x00000000, 0xffffffff);

            GFX_SetAlpha(0);

            GFX_BitBlt(0, 0, 0, 0, fb_blit.width, fb_blit.height);

            GFX_Flush();

            GFX_Task_Wait_Sync();

            GFX_Unlock();

            VERIFY(x_sema_unlock(_h_fb1_mutex) == OSR_OK);

            break;
        }

        case 0x4654:
        {
            fbdev_blit fb_blit;
            if(copy_from_user((void*)&fb_blit, pvArg, sizeof(fbdev_blit)))
            {
                printk(KERN_WARNING "mtk fb blit error: Copy from user error!\n");
                return -EFAULT;
            }

            VERIFY(x_sema_lock(_h_fb1_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);

            GFX_Lock();

            GFX_SetSrc((UINT8 *)fb_blit.src_addr, CM_RGB565_DIRECT16, fb_blit.src_pitch);

            GFX_SetDst((UINT8 *)fb_blit.dst_addr, CM_ARGB8888_DIRECT32, fb_blit.dst_pitch);

            GFX_SetBltOpt((UINT32)D_GFXFLAG_NONE, 0x00000000, 0xffffffff);

            GFX_SetAlpha(0xFF);

            GFX_BitBlt(0, 0, 0, 0, fb_blit.width, fb_blit.height);

            GFX_Flush();

            GFX_Task_Wait_Sync();

            GFX_Unlock();

            VERIFY(x_sema_unlock(_h_fb1_mutex) == OSR_OK);

            break;
        }

        // Register window
        case 0x4655:
        {
            INT32 i;
            fbdev_ipc fb_ipc;

            if(copy_from_user((void*)&fb_ipc, pvArg, sizeof(fbdev_ipc)))
            {
                printk(KERN_WARNING "/dev/fb1 error: Copy from user error!\n");
                return -EFAULT;
            }

            VERIFY(x_sema_lock(_h_fb1_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);
            if(_fb1_num_wins > 2)
            {
                VERIFY(x_sema_unlock(_h_fb1_mutex) == OSR_OK);
                printk("/dev/fb1 register win error: Maximum 2 wins only!\n");
                return -EFAULT;
            }

            // Search for free entry
            if((i = _fb1_find_win(_fb1_wins, -1)) < 0)
            {
                VERIFY(x_sema_unlock(_h_fb1_mutex) == OSR_OK);
                return -EFAULT;
            }
            // Add window and register window properties
            _fb1_num_wins ++;
            _fb1_wins[i].pid = fb_ipc.pid;
            _fb1_wins[i].x = fb_ipc.x;
            _fb1_wins[i].y = fb_ipc.y;
            _fb1_wins[i].w = fb_ipc.w;
            _fb1_wins[i].h = fb_ipc.h;
            _fb1_wins[i].dirty = 0;
            _fb1_wins[i].copying = 0;
            _fb1_wins[i].swaping = 0;
            VERIFY(x_sema_unlock(_h_fb1_mutex) == OSR_OK);
            printk("/dev/fb1 0x4654 ioctl: Create window: offset (%d, %d) size (%d x%d)!\n",
                    fb_ipc.x, fb_ipc.y, fb_ipc.w, fb_ipc.h);

            // Clearing of buf
            GFX_Lock();
            GFX_SetColor(0);
            if(fb_ipc.src_fmt_argb8888)
            {
                GFX_SetDst((UINT8 *)_fb1_bufs[2], CM_ARGB8888_DIRECT32, _get_line_length(fb_1->fb.var.xres, FB1_BITS_PER_PIXEL));
            }
            else
            {
                GFX_SetDst((UINT8 *)_fb1_bufs[2], CM_RGB565_DIRECT16, _get_line_length(fb_1->fb.var.xres, FB1_BITS_PER_PIXEL));
            }
            GFX_Fill(_fb1_wins[i].x, _fb1_wins[i].y, _fb1_wins[i].w, _fb1_wins[i].h);
            GFX_Flush();
            GFX_Task_Wait_Sync();
            GFX_Unlock();
            break;
        }

        // Copy from rendering target to framebuffer
        case 0x4656:
        {
            INT32 i;
            fbdev_ipc fb_ipc;
            if(copy_from_user((void*)&fb_ipc, pvArg, sizeof(fbdev_ipc)))
            {
                printk(KERN_WARNING "/dev/fb1 0x4656 ioctl error: Copy from user error!\n");
                return -EFAULT;
            }
#if FB1_ENABLE_COMPOSER
            VERIFY(x_sema_lock(_h_fb1_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);
            // Search for entry
            if((i = _fb1_find_win(_fb1_wins, fb_ipc.pid)) < 0)
            {
                VERIFY(x_sema_unlock(_h_fb1_mutex) == OSR_OK);
                return -EFAULT;
            }
            // Indicates this proc copying
            _fb1_wins[i].copying = 1;
            fb_ipc.curr_back_buf = _fb1_back_buf;

            GFX_Lock();
            if(fb_ipc.src_fmt_argb8888)
            {
                GFX_SetSrc((UINT8 *)fb_ipc.src_buf, CM_ARGB8888_DIRECT32, fb_ipc.src_pitch);
                GFX_SetAlpha(0);
            }
            else
            {
                GFX_SetSrc((UINT8 *)fb_ipc.src_buf, CM_RGB565_DIRECT16, fb_ipc.src_pitch);
                GFX_SetAlpha(0xFF);
            }
            GFX_SetDst((UINT8 *)_fb1_back_buf, CM_ARGB8888_DIRECT32, _get_line_length(fb_1->fb.var.xres, FB1_BITS_PER_PIXEL));
            GFX_SetBltOpt((UINT32)D_GFXFLAG_NONE, 0x00000000, 0xffffffff);
            GFX_BitBlt(0, 0, _fb1_wins[i].x, _fb1_wins[i].y, _fb1_wins[i].w, _fb1_wins[i].h);
            GFX_Flush();
            GFX_Task_Wait_Sync();
            GFX_Unlock();

            if(copy_to_user(pvArg, (void*)&fb_ipc, sizeof(fbdev_ipc)))
            {
                printk(KERN_WARNING "/dev/fb1 0x4656 ioctl error: Copy to user error!\n");
                return -EFAULT;
            }

            // Indicates this proc copy is done, waiting for flipping
            _fb1_wins[i].copying = 0;

            // Indicates this proc has been updated but not yet flipped
            _fb1_wins[i].dirty = 1;

            VERIFY(x_sema_unlock(_h_fb1_mutex) == OSR_OK);
#else
            VERIFY(x_sema_lock(_h_fb1_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);
            // Search for entry
            if((i = _fb1_find_win(_fb1_wins, fb_ipc.pid)) < 0)
            {
                VERIFY(x_sema_unlock(_h_fb1_mutex) == OSR_OK);
                return -EFAULT;
            }
            // Indicates this proc copying
            _fb1_wins[i].copying = 1;
            fb_ipc.curr_back_buf = _fb1_front_buf;

            GFX_Lock();
            if(fb_ipc.src_fmt_argb8888)
            {
                GFX_SetSrc((UINT8 *)fb_ipc.src_buf, CM_ARGB8888_DIRECT32, fb_ipc.src_pitch);
                GFX_SetAlpha(0);
            }
            else
            {
                GFX_SetSrc((UINT8 *)fb_ipc.src_buf, CM_RGB565_DIRECT16, fb_ipc.src_pitch);
                GFX_SetAlpha(0xFF);
            }
            GFX_SetDst((UINT8 *)_fb1_front_buf, CM_ARGB8888_DIRECT32, _get_line_length(fb_1->fb.var.xres, FB1_BITS_PER_PIXEL));
            GFX_SetBltOpt((UINT32)D_GFXFLAG_NONE, 0x00000000, 0xffffffff);
            GFX_BitBlt(0, 0, _fb1_wins[i].x, _fb1_wins[i].y, _fb1_wins[i].w, _fb1_wins[i].h);
            GFX_Flush();
            GFX_Task_Wait_Sync();
            GFX_Unlock();
            VERIFY(x_sema_unlock(_h_fb1_mutex) == OSR_OK);
#endif /* FB1_ENABLE_COMPOSER */
            break;
        }

        // Swapping. If necessary, copy other non-dirty area from front framebuffer to back framebuffer
        case 0x4657:
        {
            INT32 i;
            fbdev_ipc fb_ipc;
            if(copy_from_user((void*)&fb_ipc, pvArg, sizeof(fbdev_ipc)))
            {
                printk(KERN_WARNING "/dev/fb1 0x4657 ioctl error: Copy from user error!\n");
                return -EFAULT;
            }

#if FB1_ENABLE_COMPOSER
            VERIFY(x_sema_lock(_h_fb1_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);
            // Search for entry
            if((i = _fb1_find_win(_fb1_wins, fb_ipc.pid)) < 0)
            {
                VERIFY(x_sema_unlock(_h_fb1_mutex) == OSR_OK);
                return -EFAULT;
            }
            // Become non-dirty because other app has call swap
            if(_fb1_wins[i].dirty == 0)
            {
                // Indicates this proc is not performed swapping
                _fb1_wins[i].swaping = 0;
                _fb1_wins[i].dirty = 0;

                VERIFY(x_sema_unlock(_h_fb1_mutex) == OSR_OK);
                printk("PID[%d] /dev/fb1 Ignore swaping because other is doing it. Waiting for other swaping\n", _fb1_wins[i].pid);
                // wait for other app's blitting complete
                do
                {
                    x_thread_delay(1);
                }while(_fb1_wins[i == 0?1:0].swaping == 1);
                break;
            }

            // Marked non-dirty because we are flipping it
            _fb1_wins[i].dirty = 0;

            // Indicates this proc is performed swapping
            _fb1_wins[i].swaping = 1;

            if(_fb1_wins[i == 0?1:0].pid != -1 && _fb1_wins[i == 0?1:0].copying)
            {
                printk("PID[%d] /dev/fb1 Waiting for other blitting\n", _fb1_wins[i].pid);
                // wait for other app's blitting complete
                do
                {
                    x_thread_delay(1);
                }while(_fb1_wins[i == 0?1:0].copying == 1);
                _fb1_wins[i == 0?1:0].dirty = 1;
            }
            // Copy from front buf to back buf because other is not dirty

            else if(_fb1_wins[i == 0?1:0].pid != -1 && _fb1_wins[i == 0?1:0].dirty == 0)
            {
                fbdev_blit fb_blit;
                //printk("PID[%d] Copying from front 0x%8x to back 0x%8x because not dirty\n", _fb1_wins[i].pid, _fb1_front_buf, _fb1_back_buf);
                if(copy_from_user((void*)&fb_blit, pvArg, sizeof(fbdev_blit)))
                {
                    printk(KERN_WARNING "/dev/fb1 0x4657 ioctl error: Copy from user error!\n");
                    return -EFAULT;
                }

                GFX_Lock();

                GFX_SetSrc((UINT8 *)_fb1_front_buf,
                        CM_ARGB8888_DIRECT32, _get_line_length(fb_1->fb.var.xres, FB1_BITS_PER_PIXEL));

                GFX_SetDst((UINT8 *)_fb1_back_buf,
                        CM_ARGB8888_DIRECT32, _get_line_length(fb_1->fb.var.xres, FB1_BITS_PER_PIXEL));

                GFX_SetBltOpt((UINT32)D_GFXFLAG_NONE, 0x00000000, 0xffffffff);

                GFX_SetAlpha(0);

                GFX_BitBlt(_fb1_wins[i == 0?1:0].x, _fb1_wins[i == 0?1:0].y, _fb1_wins[i == 0?1:0].x, _fb1_wins[i == 0?1:0].y, _fb1_wins[i == 0?1:0].w, _fb1_wins[i == 0?1:0].h);

                GFX_Flush();

                GFX_Task_Wait_Sync();

                GFX_Unlock();

            }

            if(_fb1_front_buf == _fb1_bufs[0])
            {
                UINT32 u4BmpAddr;
                // printk("Flip to 0x%8x\n", _fb1_bufs[2]);
                OSD_RGN_Get((UINT32)fb1_rgn_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
                if((u4BmpAddr & 0x3FFFFFFF) != _fb1_bufs[2])
                {
                    // printk("_fb1_bufs[0] being altered! Set back to 0x%8x\n", _fb1_bufs[0]);
                    OSD_RGN_Set((UINT32)fb1_rgn_2, OSD_RGN_BMP_ADDR, _fb1_bufs[2]);
                }
                CHECK_FAIL(OSD_PLA_FlipTo(FB1_OSD, fb1_rgn_list_2), 0);
            }
            else if(_fb1_front_buf == _fb1_bufs[1])
            {
                printk("/dev/fb1 0x4657 ioctl not support in stretch == 1 mode\n");
            }
            else if(_fb1_front_buf == _fb1_bufs[2])
            {
                UINT32 u4BmpAddr;
                // printk("Flip to 0x%8x\n", _fb1_bufs[2]);
                OSD_RGN_Get((UINT32)fb1_rgn, OSD_RGN_BMP_ADDR, &u4BmpAddr);
                if((u4BmpAddr & 0x3FFFFFFF) != _fb1_bufs[0])
                {
                    // printk("_fb1_bufs[0] being altered! Set back to 0x%8x\n", _fb1_bufs[0]);
                    OSD_RGN_Set((UINT32)fb1_rgn, OSD_RGN_BMP_ADDR, _fb1_bufs[0]);
                }
                CHECK_FAIL(OSD_PLA_FlipTo(FB1_OSD, fb1_rgn_list), 0);
            }

            // Swap role of back & front
            if(_fb1_back_buf == _fb1_bufs[0])
            {
                _fb1_front_buf = _fb1_bufs[0];
                _fb1_back_buf = _fb1_bufs[2];
            }
            else
            {
                _fb1_front_buf = _fb1_bufs[2];
                _fb1_back_buf = _fb1_bufs[0];
            }

            // Indicates swaping done.
            _fb1_wins[i].swaping = 0;
            _fb1_wins[i == 0?1:0].dirty = 0;
            VERIFY(x_sema_unlock(_h_fb1_mutex) == OSR_OK);
#else
            CHECK_FAIL(OSD_PLA_FlipTo(FB1_OSD, fb1_rgn_list), 0);
#endif /* FB1_ENABLE_COMPOSER */
            break;
        }

        /* Removing pid from compose list */
        case 0x4658:
        {
            INT32 i;
            fbdev_ipc fb_ipc;
            if(copy_from_user((void*)&fb_ipc, pvArg, sizeof(fbdev_ipc)))
            {
                printk(KERN_WARNING "/dev/fb1 0x4658 ioctl error: Copy from user error!\n");
                return -EFAULT;
            }
            VERIFY(x_sema_lock(_h_fb1_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);
            // Search for entry
            if((i = _fb1_find_win(_fb1_wins, fb_ipc.pid)) < 0)
            {
                VERIFY(x_sema_unlock(_h_fb1_mutex) == OSR_OK);
                return -EFAULT;
            }

            // Reset pid
            _fb1_wins[i].pid = -1;
            _fb1_wins[i].copying = 0;
            _fb1_wins[i].swaping = 0;
            _fb1_wins[i].dirty = 0;
            _fb1_num_wins=(_fb1_num_wins==0?0:_fb1_num_wins-1);

            VERIFY(x_sema_unlock(_h_fb1_mutex) == OSR_OK);
            break;
        }

        /* Get all pids */
        case 0x4659:
        {
            INT32 pids[FB1_MAX_NUM_OF_WINS];

            VERIFY(x_sema_lock(_h_fb1_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);
            pids[0] = _fb1_wins[0].pid;
            pids[1] = _fb1_wins[1].pid;
            VERIFY(x_sema_unlock(_h_fb1_mutex) == OSR_OK);
            if(copy_to_user(pvArg, (void*)pids, sizeof(INT32) * FB1_MAX_NUM_OF_WINS))
            {
                printk(KERN_WARNING "/dev/fb1 0x4659 ioctl error: Copy from user error!\n");
                return -EFAULT;
            }
            break;
        }
#if 0
        case 0x4660:
        {
            struct fb_var_screeninfo var; /* Current var */

            _u2PanelWidth = OSD_BASE_GetDisplayWidth();
            _u2PanelHeight = OSD_BASE_GetDisplayHeight();

            if (IS_PANEL_PDP)
            {
                if(_u2PanelWidth == 1024)
                _u2PanelHeight = _u2PanelHeight/2;
            }

            if(copy_from_user((void*)&var, pvArg, sizeof(struct fb_var_screeninfo)))
            {
                printk(KERN_WARNING "/dev/fb1 0x4660 ioctl error: Copy from user error!\n");
                return -EFAULT;
            }
            if(!var.reserved[0])
            {
                if((var.reserved[1] + var.reserved[3] > _u2PanelWidth) || (var.reserved[2] + var.reserved[4] > _u2PanelHeight))
                {
                    // Requested window is greater than panel size: cannot fit in panel output so return error
                    return -EINVAL;
                }
                else if(var.reserved[1] == _u2PanelWidth && var.reserved[2] == _u2PanelHeight)
                {
                    // Requested window == panel size, always use full screen mode
                    var.reserved[0] = 1;
                }
            }
            else if(var.reserved[1] > _u2PanelWidth || var.reserved[2] > _u2PanelHeight)
            {
                // Requested window is greater than panel size, so scaling down
                var.reserved[1] = _u2PanelWidth;
                var.reserved[2] = _u2PanelHeight;
            }

            if(copy_to_user(pvArg, (void*)&var, sizeof(struct fb_var_screeninfo)))
            {
                printk(KERN_WARNING "/dev/fb1 0x4660 ioctl error: Copy to user error!\n");
                return -EFAULT;
            }

            break;
        }
#endif
        case 0x4661:
        {
            struct fb_var_screeninfo var; /* Current var */

            if(copy_from_user((void*)&var, pvArg, sizeof(struct fb_var_screeninfo)))
            {
                printk(KERN_WARNING "/dev/fb1 0x4661 ioctl error: Copy from user error!\n");
                return -EFAULT;
            }

            if(var.reserved[0] == 1) // PR

            {
                // PR mode buffer 0
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_W, 1280);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_H, 1080);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_DISP_W, 1280);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_DISP_H, 1080);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_OUT_W, 1280);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_OUT_H, 1080);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_POS_X, 0);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_POS_Y, 0);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_PITCH, 1280 * 4);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_ADDR, _fb1_bufs[0]);
                // PR mode buffer 1
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_W, 1280);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_H, 1080);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_DISP_W, 1280);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_DISP_H, 1080);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_OUT_W, 1280);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_OUT_H, 1080);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_POS_X, 0);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_POS_Y, 0);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_PITCH, 1280 * 4);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_ADDR, _fb1_bufs[2]);

                OSD_SC_Scale(FB1_OSD_SCALER, TRUE, 1280, _u2PanelHeight,
                        _u2PanelWidth, _u2PanelHeight);

            }
            else if(var.reserved[0] == 2) // SG

            {
                // SG mode L frame
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_W, 1280);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_H, 720);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_DISP_W, 1280);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_DISP_H, 720);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_OUT_W, 1280);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_OUT_H, 720);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_POS_X, 0);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_POS_Y, 0);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_PITCH, 2560 * 4);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_ADDR, _fb1_bufs[0]);
                // SG mode L frame 2
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_W, 1280);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_H, 720);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_DISP_W, 1280);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_DISP_H, 720);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_OUT_W, 1280);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_OUT_H, 720);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_POS_X, 0);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_POS_Y, 0);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_PITCH, 2560 * 4);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_ADDR, _fb1_bufs[2]);
#ifdef OSD_DSC_PDP
                // SG mode R frame
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp, OSD_RGN_BMP_W, 1280);
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp, OSD_RGN_BMP_H, 720);
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp, OSD_RGN_DISP_W, 1280);
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp, OSD_RGN_DISP_H, 720);
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp, OSD_RGN_OUT_W, 1280);
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp, OSD_RGN_OUT_H, 720);
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp, OSD_RGN_POS_X, 0);
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp, OSD_RGN_POS_Y, 0);
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp, OSD_RGN_BMP_PITCH, 2560 * 4);
                // Offset 1280 pixels is 1st line of R frame
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp, OSD_RGN_BMP_ADDR, _fb1_bufs[0] + 1280 * 4);
                // SG mode R frame 2
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp_2, OSD_RGN_BMP_W, 1280);
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp_2, OSD_RGN_BMP_H, 720);
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp_2, OSD_RGN_DISP_W, 1280);
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp_2, OSD_RGN_DISP_H, 720);
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp_2, OSD_RGN_OUT_W, 1280);
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp_2, OSD_RGN_OUT_H, 720);
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp_2, OSD_RGN_POS_X, 0);
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp_2, OSD_RGN_POS_Y, 0);
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp_2, OSD_RGN_BMP_PITCH, 2560 * 4);
                // Offset 1280 pixels is 1st line of R frame 2
                i4Ret = OSD_RGN_Set(fb1_rgn_pdp_2, OSD_RGN_BMP_ADDR, _fb1_bufs[2] + 1280 * 4);
#endif /* OSD_DSC_PDP */
                OSD_SC_Scale(FB1_OSD_SCALER, TRUE, 1280, 720,
                        _u2PanelWidth, _u2PanelHeight);

            }
            else
            {
                i4Ret = -EFAULT;
            }

            _u13DMode = var.reserved[0];
            // Indicates 3D mode is not yet applied
            _u13DModeApplied = 0;
            break;
        }

        // 3D supported Flip
        case 0x4662:
        {
            struct fb_var_screeninfo var; /* Current var */

            if(copy_from_user((void*)&var, pvArg, sizeof(struct fb_var_screeninfo)))
            {
                printk(KERN_WARNING "/dev/fb1 0x4662 ioctl error: Copy from user error!\n");
                return -EFAULT;
            }

#if FB1_FORCE_APPLY_REGION_SRC_DST_RECT
            // Force to apply the region width and height upon flip
            if(var.reserved[0] != 1 && var.reserved[0] != 2)
            {
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_BMP_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_DISP_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_DISP_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_OUT_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(fb1_rgn, OSD_RGN_OUT_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BMP_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_DISP_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_DISP_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_OUT_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(fb1_rgn_2, OSD_RGN_OUT_H, var.reserved[2]);
            }

            // Check for 1024x768 PDP panel case
            if((_u2PanelWidth == 1024 && (var.reserved[1] > _u2PanelWidth)) || (_u2PanelHeight == 768 && (var.reserved[2] > _u2PanelHeight)))
            {
                u4SrcPdpWidth = var.reserved[1];
                u4SrcPdpHeight = var.reserved[2];
                OSD_SC_Scale(FB1_OSD_SCALER, TRUE, _u2PanelWidth, _u2PanelHeight,
                        _u2PanelWidth, _u2PanelHeight);

            }
#endif

            i4Ret = _fb_1_set_base(fb_1->fb.fix.smem_start + var.yoffset * _get_line_length(fb_1->fb.var.xres, FB1_BITS_PER_PIXEL),
                    var.yoffset);
            break;
        }

        // get the panel info
        case 0x4663:
        {
            int panel_info[3];

            // pdp panel flag; 1 - pdp panel, 0 - non pdp panel
            panel_info[0] =
#if defined(CC_MT5398) || defined(CC_MT5399)
                            (IS_PANEL_2D_N_3D_L12R12)
#else
                            (IS_PANEL_L12R12)
#endif
                            ? 1 : 0;
            panel_info[1] = OSD_BASE_GetDisplayWidth();  // panel width
            panel_info[2] = OSD_BASE_GetDisplayHeight(); // panel height

            // if it's PDP panel and 3d mode, the height of the panel is double, so it needs dividing by 2.
            if(IS_PANEL_L12R12)
                panel_info[2] >>= 1;

            /*
             * panel_info[0]: pdp panel flag
             * panel_info[1]: panel width
             * panel_info[2]: panel height
             */
            if(copy_to_user(pvArg, (const void *)&panel_info, sizeof(panel_info)))
			{
				printk(KERN_WARNING "/dev/fb0 0x4663 ioctl error: Copy to user error!\n");
				return -EFAULT;
			}
            break;
        }

        default:
        //printk(KERN_WARNING "_mt53fb_ioctl(): not support ioctl cmd %d\n", cmd);
        i4Ret = -EINVAL;
        break;
    }
    return i4Ret;
}

static struct fb_ops mtk_fb_1_ops =
{
    .owner = THIS_MODULE,
    .fb_check_var = mtk_fb_1_check_var,
    .fb_set_par = mtk_fb_1_set_par,
    .fb_setcolreg = mtk_fb_1_setcolreg,
    .fb_pan_display = mtk_fb_1_pan_display,

    /* These are generic software based fb functions */
    .fb_fillrect = cfb_fillrect,
    .fb_copyarea = cfb_copyarea,
    .fb_imageblit = cfb_imageblit,
    .fb_ioctl = _mtk_fb_1_ioctl,
};

static int mtk_fb_1_setup(void)
{
    int ret;
    //size_t framesize;

    printk("MTK /DEV/FB1 mtk_fb_probe\n");
    fb_1 = kzalloc(sizeof(*fb_1), GFP_KERNEL);
    if(fb_1 == NULL)
    {
        ret = -ENOMEM;
        goto err_fb_alloc_failed;
    }

    //width = ((osd2_output_width==0)?width:osd2_output_width);
    //height = ((osd2_output_height==0)?height:osd2_output_height);

    if(_h_fb1_mutex == 0)
    {
        x_sema_create(&_h_fb1_mutex, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK);
    }

    fb_1->fb.fbops = &mtk_fb_1_ops;

    /* These modes are the ones currently required by Android */

    fb_1->fb.flags = FBINFO_FLAG_DEFAULT;
    fb_1->fb.pseudo_palette = fb_1->cmap;
    fb_1->fb.fix.type = FB_TYPE_PACKED_PIXELS;
    fb_1->fb.fix.visual = FB_VISUAL_TRUECOLOR;
    fb_1->fb.fix.line_length = drv_1->get_line_length(fb1_width, FB1_BITS_PER_PIXEL);
    fb_1->fb.fix.accel = FB_ACCEL_NONE;
    fb_1->fb.fix.ypanstep = 1;

    fb_1->fb.var.xres = fb1_width;
    fb_1->fb.var.yres = fb1_height;
    fb_1->fb.var.xres_virtual = fb1_width;
    fb_1->fb.var.yres_virtual = fb1_height * FB1_NUMBER_OF_BUFFERS;
    fb_1->fb.var.bits_per_pixel = FB1_BITS_PER_PIXEL;
    fb_1->fb.var.activate = FB_ACTIVATE_NOW;
    fb_1->fb.var.height = fb1_height;
    fb_1->fb.var.width = fb1_width;

    fb_1->fb.var.red.offset = 16;
    fb_1->fb.var.red.length = 8;
    fb_1->fb.var.green.offset = 8;
    fb_1->fb.var.green.length = 8;
    fb_1->fb.var.blue.offset = 0;
    fb_1->fb.var.blue.length = 8;
    fb_1->fb.var.transp.offset = 24;
    fb_1->fb.var.transp.length = 8;

    ret = drv_1->setup_buffer(
            fb1_width,
            fb1_height,
            fb_1->fb.var.yres_virtual,
            FB1_BITS_PER_PIXEL,
            MTK_DRV_FB_CM_ARGB8888,
            (void**)&fb_1->fb.screen_base,
            (unsigned long**)&fb_1->fb.fix.smem_start,
            (unsigned long**)&fb_1->fb.fix.smem_len);

    if(ret)
    goto err_fb_drv_failed;

    ret = fb_set_var(&fb_1->fb, &fb_1->fb.var);
    if(ret)
    goto err_fb_set_var_failed;

    ret = register_framebuffer(&fb_1->fb);
    if(ret)
    goto err_register_framebuffer_failed;
    printk("/DEV/FB1 probe successful\n");
    return 0;

    err_register_framebuffer_failed:
    err_fb_set_var_failed:
    err_fb_drv_failed:
    kfree(fb_1);
    err_fb_alloc_failed:
    printk("/DEV/FB1 probe failed\n");
    return ret;
}

static void _fb_1_exit(void)
{
    /* TODO:  Release all resources .... */
}

static int _fb_1_setup_buffer(
        u32 xres,
        u32 yres,
        u32 vyres,
        u32 bits_per_pix,
        u32 cm,
        void ** p_screen_base,
        unsigned long ** p_phy_start,
        unsigned long ** p_size)
{
    // From DFB partition offset 16MB
    //*p_phy_start = (BSP_GetFbmMemAddr() - DRVCUST_OptGet(eDirectFBMemSize) + 1920 * 1080 * 4 * 2) & PAGE_MASK;
    // Share with DDI_GPU POSD1
	*p_phy_start = (unsigned long*)((fb_osd_get_fb_addr() + 1280 * 720 * 4 * 4) & PAGE_MASK);
    
    /* We Don't need too much memory as DTV mw.*/
    *p_size = (unsigned long*)(vyres * _get_line_length(xres, bits_per_pix) * 2);
    //*p_size = DRVCUST_OptGet(eDirectFBMemSize);
    /* USE DTV-FBM convertion or use ioremap, check whether the core.c mapped
     * FB memory when system init.
     * */
    *p_screen_base = ioremap((unsigned long)*p_phy_start, (size_t)*p_size);
    printk("mt5395 fb 1 get buffer :scr base:0x%x, phy start:0x%x, size %d\n",
            (unsigned int)*p_screen_base,
            (unsigned int)*p_phy_start,
            (int)*p_size);
#ifdef BOOTUP_TIME_CUT_DOWN
    memset((void*)*p_screen_base, 0x0, (int)*p_size);
#endif

    CHECK_FAIL(OSD_RGN_LIST_Create(&fb1_rgn_list), 0);

    CHECK_FAIL(OSD_RGN_Create(&fb1_rgn, 1280, 720,
                    *p_phy_start,
                    _get_drv_cm(cm),
                    _get_line_length(xres, FB1_BITS_PER_PIXEL),
                    0, 0, 1280, 720), 0);

    CHECK_FAIL(OSD_RGN_Set(fb1_rgn, OSD_RGN_BIG_ENDIAN, 0), 0);

    CHECK_FAIL(OSD_RGN_Insert(fb1_rgn, fb1_rgn_list), 0);

    /* TODO: Which plane?*/
    CHECK_FAIL(OSD_PLA_FlipTo(FB1_OSD, fb1_rgn_list), 0);

    // Modify for plane alpha
    CHECK_FAIL(OSD_RGN_Set(fb1_rgn, OSD_RGN_MIX_SEL, OSD_BM_PIXEL), 0);

    // Create 2nd region
    CHECK_FAIL(OSD_RGN_LIST_Create(&fb1_rgn_list_2), 0);

    CHECK_FAIL(OSD_RGN_Create(&fb1_rgn_2, 1280, 720,
                    *p_phy_start + FB1_HEIGHT * _get_line_length(FB1_WIDTH, FB1_BITS_PER_PIXEL),
                    _get_drv_cm(cm),
                    _get_line_length(xres, FB1_BITS_PER_PIXEL),
                    0, 0, 1280, 720), 0);

    CHECK_FAIL(OSD_RGN_Set(fb1_rgn_2, OSD_RGN_BIG_ENDIAN, 0), 0);

    CHECK_FAIL(OSD_RGN_Insert(fb1_rgn_2, fb1_rgn_list_2), 0);

    // Modify for plane alpha
    CHECK_FAIL(OSD_RGN_Set(fb1_rgn_2, OSD_RGN_MIX_SEL, OSD_BM_PIXEL), 0);

    _fb1_bufs[0] = (UINT32)*p_phy_start;
#ifdef POSD1_FHD
    _fb1_bufs[1] = (UINT32)(*p_phy_start) + 1920 * 1080 * 4;
#else
    _fb1_bufs[1] = (UINT32)(*p_phy_start) + 1280 * 720 * 4;
#endif

#if FB1_ENABLE_COMPOSER
    _fb1_bufs[2] = (UINT32)(*p_phy_start) + FB1_HEIGHT * _get_line_length(FB1_WIDTH, FB1_BITS_PER_PIXEL) * 2;
    // x_memset((void*)(VIRTUAL((UINT32)_fb1_bufs[2])), 0, FB1_HEIGHT * _get_line_length(FB1_WIDTH, FB1_BITS_PER_PIXEL));
#else
    _fb1_bufs[2] = (UINT32)*p_phy_start;
#endif

    _fb1_front_buf = _fb1_bufs[0];
    _fb1_back_buf = _fb1_bufs[2];

#ifdef OSD_DSC_PDP
    u4PDPFBBuffer1 = _fb1_bufs[1];
    u4PDPFBBuffer2 = (UINT32)(*p_phy_start) + FB1_HEIGHT * _get_line_length(FB1_WIDTH, FB1_BITS_PER_PIXEL) * 3;
    _fb1_bufs[3] = u4PDPFBBuffer2;

    CHECK_FAIL(OSD_RGN_LIST_Create(&fb1_rgn_pdp_list), 0);

    CHECK_FAIL(OSD_RGN_Create(&fb1_rgn_pdp, UI_DST_WIDTH, UI_DST_HEIGHT,
                    (void *)u4PDPFBBuffer1,
                    _get_drv_cm(cm),
                    _get_line_length(UI_DST_WIDTH, FB1_BITS_PER_PIXEL),
                    0, 0, UI_DST_WIDTH, UI_DST_HEIGHT), 0);

    CHECK_FAIL(OSD_RGN_Set(fb1_rgn_pdp, OSD_RGN_BIG_ENDIAN, 0), 0);

    CHECK_FAIL(OSD_RGN_Insert(fb1_rgn_pdp, fb1_rgn_pdp_list), 0);

    /* TODO: Which plane?*/
    CHECK_FAIL(OSD_PLA_FlipTo(FB1_OSD, fb1_rgn_pdp_list), 0);

    // Modify for plane alpha
    CHECK_FAIL(OSD_RGN_Set(fb1_rgn_pdp, OSD_RGN_MIX_SEL, OSD_BM_PIXEL), 0);

    // Create 2nd region
    CHECK_FAIL(OSD_RGN_LIST_Create(&fb1_rgn_pdp_list_2), 0);

    CHECK_FAIL(OSD_RGN_Create(&fb1_rgn_pdp_2, UI_DST_WIDTH, UI_DST_HEIGHT,
                    (void *)u4PDPFBBuffer2,
                    _get_drv_cm(cm),
                    _get_line_length(UI_DST_WIDTH, FB1_BITS_PER_PIXEL),
                    0, 0, UI_DST_WIDTH, UI_DST_HEIGHT), 0);

    CHECK_FAIL(OSD_RGN_Set(fb1_rgn_pdp_2, OSD_RGN_BIG_ENDIAN, 0), 0);

    CHECK_FAIL(OSD_RGN_Insert(fb1_rgn_pdp_2, fb1_rgn_pdp_list_2), 0);

    // Modify for plane alpha
    CHECK_FAIL(OSD_RGN_Set(fb1_rgn_pdp_2, OSD_RGN_MIX_SEL, OSD_BM_PIXEL), 0);
#endif

    /* TODO: Which scaler:*/
    CHECK_FAIL(OSD_SC_Scale(FB1_OSD_SCALER, TRUE, xres, yres, OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayWidth()), 0);

    return 0;
}

static int _fb_1_set_base(unsigned long phy_scr_base, unsigned long offset)
{
    UINT32 u4BmpAddr;
    // printk("Before 0x%8x, buf[2]: 0x%8x\n", phy_scr_base, _fb1_bufs[2]);
    if(phy_scr_base == _fb1_bufs[0])
    {
#ifdef OSD_DSC_PDP
        if(_u13DMode == 1) // PR mode

        {
            if(!_u13DModeApplied)
            {
                OSD_PLA_Set3DMode(FB1_OSD, 0);
                _u13DModeApplied = 1;
            }
            OSD_RGN_Get((UINT32)fb1_rgn, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != _fb1_bufs[0])
            {
                printk("_fb1_bufs[0] being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, _fb1_bufs[0]);
                OSD_RGN_Set((UINT32)fb1_rgn, OSD_RGN_BMP_ADDR, _fb1_bufs[0]);
            }
            CHECK_FAIL(OSD_PLA_FlipTo(FB1_OSD, fb1_rgn_list), 0);
        }
        else if(_u13DMode == 2) // SG mode

        {
            if(!_u13DModeApplied)
            {
                OSD_PLA_Set3DMode(FB1_OSD, 1);
                _u13DModeApplied = 1;
            }
            // Check if fb1_rgn is correct
            OSD_RGN_Get((UINT32)fb1_rgn, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != _fb1_bufs[0])
            {
                printk("SG R region 1 being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, _fb1_bufs[0]);
                OSD_RGN_Set((UINT32)fb1_rgn, OSD_RGN_BMP_ADDR, _fb1_bufs[0]);
            }
            OSD_RGN_Get((UINT32)fb1_rgn_pdp, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != (_fb1_bufs[0] + 1280 * 4))
            {
                printk("SG L region 1 being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, (_fb1_bufs[0] + 1280 * 4));
                OSD_RGN_Set((UINT32)fb1_rgn_pdp, OSD_RGN_BMP_ADDR, (_fb1_bufs[0] + 1280 * 4));
            }
            OSD_PLA_Set3DRegion(FB1_OSD, fb1_rgn, fb1_rgn_pdp);
        }
        else if((_u2PanelWidth == 1024 && (u4SrcPdpWidth > _u2PanelWidth)) ||
                (_u2PanelHeight == 768 && (u4SrcPdpHeight > _u2PanelHeight)))
        {
            OSD_PLA_DsBlit((UINT32)phy_scr_base, 0, 0, u4SrcPdpWidth, u4SrcPdpHeight, _get_line_length(fb_1->fb.var.xres, FB1_BITS_PER_PIXEL),
                    u4PDPFBBuffer1, 0, 0, _u2PanelWidth, _u2PanelHeight, _u2PanelWidth*4, OSD_CM_ARGB8888_DIRECT32);
            OSD_RGN_Get((UINT32)fb1_rgn_pdp, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != u4PDPFBBuffer1)
            {
                printk("u4PDPFBBuffer1 being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, u4PDPFBBuffer1);
                OSD_RGN_Set((UINT32)fb1_rgn_pdp, OSD_RGN_BMP_ADDR, u4PDPFBBuffer1);
            }
            CHECK_FAIL(OSD_PLA_FlipTo(FB1_OSD, fb1_rgn_pdp_list), 0);
        }
        else
#endif
        {
            OSD_RGN_Get((UINT32)fb1_rgn, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != _fb1_bufs[0])
            {
                printk("_fb1_bufs[0] being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, _fb1_bufs[0]);
                OSD_RGN_Set((UINT32)fb1_rgn, OSD_RGN_BMP_ADDR, _fb1_bufs[0]);
            }
            CHECK_FAIL(OSD_PLA_FlipTo(FB1_OSD, fb1_rgn_list), 0);
        }
    }
    else if(phy_scr_base == _fb1_bufs[1])
    {
#ifdef OSD_DSC_PDP
        if(_u13DMode == 1) // PR mode

        {
            if(!_u13DModeApplied)
            {
                OSD_PLA_Set3DMode(FB1_OSD, 0);
                _u13DModeApplied = 1;
            }
            OSD_RGN_Get((UINT32)fb1_rgn_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != _fb1_bufs[1])
            {
                printk("_fb1_bufs[1] being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, _fb1_bufs[1]);
                OSD_RGN_Set((UINT32)fb1_rgn_2, OSD_RGN_BMP_ADDR, _fb1_bufs[1]);
            }
            CHECK_FAIL(OSD_PLA_FlipTo(FB1_OSD, fb1_rgn_list_2), 0);
        }
        else if(_u13DMode == 2) // SG mode

        {
            if(!_u13DModeApplied)
            {
                OSD_PLA_Set3DMode(FB1_OSD, 1);
                _u13DModeApplied = 1;
            }
            // Check if fb1_rgn is correct
            OSD_RGN_Get((UINT32)fb1_rgn_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != _fb1_bufs[2])
            {
                printk("SG L region 2 being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, _fb1_bufs[2]);
                OSD_RGN_Set((UINT32)fb1_rgn_2, OSD_RGN_BMP_ADDR, _fb1_bufs[2]);
            }
            OSD_RGN_Get((UINT32)fb1_rgn_pdp_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != (_fb1_bufs[2] + 1280 * 4))
            {
                printk("SG R region 2 being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, (_fb1_bufs[2] + 1280 * 4));
                OSD_RGN_Set((UINT32)fb1_rgn_pdp_2, OSD_RGN_BMP_ADDR, (_fb1_bufs[2] + 1280 * 4));
            }
            OSD_PLA_Set3DRegion(FB1_OSD, fb1_rgn_2, fb1_rgn_pdp_2);
        }
        else if((_u2PanelWidth == 1024 && (u4SrcPdpWidth > _u2PanelWidth)) ||
                (_u2PanelHeight == 768 && (u4SrcPdpHeight > _u2PanelHeight)))
        {
            OSD_PLA_DsBlit((UINT32)phy_scr_base, 0, 0, u4SrcPdpWidth, u4SrcPdpHeight, _get_line_length(fb_1->fb.var.xres, FB1_BITS_PER_PIXEL),
                    u4PDPFBBuffer2, 0, 0, _u2PanelWidth, _u2PanelHeight, _u2PanelWidth*4, OSD_CM_ARGB8888_DIRECT32);
            OSD_RGN_Get((UINT32)fb1_rgn_pdp_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != u4PDPFBBuffer2)
            {
                printk("u4PDPFBBuffer2 being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, u4PDPFBBuffer2);
                OSD_RGN_Set((UINT32)fb1_rgn_pdp_2, OSD_RGN_BMP_ADDR, u4PDPFBBuffer2);
            }
            CHECK_FAIL(OSD_PLA_FlipTo(FB1_OSD, fb1_rgn_pdp_list_2), 0);
        }
        else
#endif
        {
            OSD_RGN_Get((UINT32)fb1_rgn_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != _fb1_bufs[1])
            {
                printk("_fb1_bufs[1] being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, _fb1_bufs[1]);
                OSD_RGN_Set((UINT32)fb1_rgn_2, OSD_RGN_BMP_ADDR, _fb1_bufs[1]);
            }
            CHECK_FAIL(OSD_PLA_FlipTo(FB1_OSD, fb1_rgn_list_2), 0);
        }
    }
    else if(phy_scr_base == _fb1_bufs[2])
    {
#ifdef OSD_DSC_PDP
        if(_u13DMode == 1) // PR mode

        {
            if(!_u13DModeApplied)
            {
                OSD_PLA_Set3DMode(FB1_OSD, 0);
                _u13DModeApplied = 1;
            }
            OSD_RGN_Get((UINT32)fb1_rgn_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != _fb1_bufs[2])
            {
                printk("_fb1_bufs[2] being altered! Set back to 0x%8x\n", _fb1_bufs[2]);
                OSD_RGN_Set((UINT32)fb1_rgn_2, OSD_RGN_BMP_ADDR, _fb1_bufs[2]);
            }
            CHECK_FAIL(OSD_PLA_FlipTo(FB1_OSD, fb1_rgn_list_2), 0);
        }
        else if(_u13DMode == 2) // SG mode

        {
            if(!_u13DModeApplied)
            {
                OSD_PLA_Set3DMode(FB1_OSD, 1);
                _u13DModeApplied = 1;
            }
            // Check if fb1_rgn is correct
            OSD_RGN_Get((UINT32)fb1_rgn_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != _fb1_bufs[2])
            {
                printk("SG L region 2 being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, _fb1_bufs[2]);
                OSD_RGN_Set((UINT32)fb1_rgn_2, OSD_RGN_BMP_ADDR, _fb1_bufs[2]);
            }
            OSD_RGN_Get((UINT32)fb1_rgn_pdp_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != (_fb1_bufs[2] + 1280 * 4))
            {
                printk("SG R region 2 being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, (_fb1_bufs[2] + 1280 * 4));
                OSD_RGN_Set((UINT32)fb1_rgn_pdp_2, OSD_RGN_BMP_ADDR, (_fb1_bufs[2] + 1280 * 4));
            }
            OSD_PLA_Set3DRegion(FB1_OSD, fb1_rgn_2, fb1_rgn_pdp_2);
        }
        else if((_u2PanelWidth == 1024 && (u4SrcPdpWidth > _u2PanelWidth)) ||
                (_u2PanelHeight == 768 && (u4SrcPdpHeight > _u2PanelHeight)))
        {
            OSD_PLA_DsBlit((UINT32)phy_scr_base, 0, 0, u4SrcPdpWidth, u4SrcPdpHeight, _get_line_length(fb_1->fb.var.xres, FB1_BITS_PER_PIXEL),
                    u4PDPFBBuffer2, 0, 0, _u2PanelWidth, _u2PanelHeight, _u2PanelWidth*4, OSD_CM_ARGB8888_DIRECT32);
            OSD_RGN_Get((UINT32)fb1_rgn_pdp_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != u4PDPFBBuffer2)
            {
                printk("u4PDPFBBuffer2 being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, u4PDPFBBuffer2);
                OSD_RGN_Set((UINT32)fb1_rgn_pdp_2, OSD_RGN_BMP_ADDR, u4PDPFBBuffer2);
            }
            CHECK_FAIL(OSD_PLA_FlipTo(FB1_OSD, fb1_rgn_pdp_list_2), 0);
        }
        else
#endif
        {
            OSD_RGN_Get((UINT32)fb1_rgn_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != _fb1_bufs[2])
            {
                printk("_fb1_bufs[2] being altered! Set back to 0x%8x\n", _fb1_bufs[2]);
                OSD_RGN_Set((UINT32)fb1_rgn_2, OSD_RGN_BMP_ADDR, _fb1_bufs[2]);
            }
            CHECK_FAIL(OSD_PLA_FlipTo(FB1_OSD, fb1_rgn_list_2), 0);
        }
    }
    // Safty guard code for imprecise out vsync isr

    {
        UINT32 u4Val;
        //       UINT32 i = 0;

#if 0
        u4Val = IO_READ32(0xF0021000, 0x324) << 4;
        //printk("After 0x%8x\n", phy_scr_base);
        if((u4Val & 0xFFFFFF) != (phy_scr_base & 0xFFFFFF) )
        {
            //printk("Read 0x%8x != set 0x%8x\n", u4Val, phy_scr_base);
            OSD_WaitVsync();
        }
#else
        while(1)
        {
            u4Val = IO_READ32(0xF0021000, 0x324) << 4;
            if((u4Val & 0xFFFFFF) != (phy_scr_base & 0xFFFFFF) )
            {
                break;
            }
        }
#endif
    }

    return 0;
}

static mtk_drv_fb_t mt5395_fb_1 =
{
    .fb_exit = _fb_1_exit,
    .get_line_length = _get_line_length,
    .setup_buffer = _fb_1_setup_buffer,
    .set_base = _fb_1_set_base,
};

int mtk_drv_fb_1_init(mtk_drv_fb_t ** drv)
{
    // OSD_PLA_Enable(FB1_OSD, TRUE);

    *drv = &mt5395_fb_1;
    return 0;
}

#endif

#ifdef HAS_DEV_FB2
#define CHECK_FAIL(stmt, exp_ret) \
do {\
    int _____ret = (stmt);\
    if(_____ret != (exp_ret))\
    {\
        printk("CHECK FAILED %s ret:%d\n", #stmt, _____ret);\
    }\
}while(0)

#define FB2_OSD OSD_PLANE_2
#define FB2_OSD_SCALER OSD_SCALER_2

#define FB2_NUMBER_OF_BUFFERS 2
//#define ANDROID_BITS_PER_PIXEL 16
#define FB2_BITS_PER_PIXEL 32
/* Modify these macros to suit the hardware */

#define FB2_WIDTH 1280
#define FB2_HEIGHT 720

#define FB2_ENABLE_COMPOSER 1
#define FB2_MAX_NUM_OF_WINS 2

UINT32 fb2_width = FB2_WIDTH;
UINT32 fb2_height = FB2_HEIGHT;

static mtk_drv_fb_t * drv_2;
struct mtk_fb *fb_2;

static UINT8 _u1FB2_3DMode = 0;
static UINT8 _u1FB2_3DModeApplied = 0;

static int _fb_2_set_base(unsigned long phy_scr_base, unsigned long offset);

/* set the software color map.  Probably doesn't need modifying. */
static int
mtk_fb_2_setcolreg(unsigned int regno, unsigned int red, unsigned int green,
        unsigned int blue, unsigned int transp, struct fb_info *info)
{
    struct mtk_fb *fb = container_of(info, struct mtk_fb, fb);

    if (regno < 16)
    {
        fb->cmap[regno] = convert_bitfield(transp, &fb->fb.var.transp) |
        convert_bitfield(blue, &fb->fb.var.blue) |
        convert_bitfield(green, &fb->fb.var.green) |
        convert_bitfield(red, &fb->fb.var.red);
        return 0;
    }
    else
    {
        return 1;
    }
}

/* check var to see if supported by this device.  Probably doesn't
 * need modifying.
 */
static int mtk_fb_2_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
    if((var->rotate & 1) != (info->var.rotate & 1))
    {
        if((var->xres != info->var.yres) ||
                (var->yres != info->var.xres) ||
                (var->xres_virtual != info->var.yres) ||
                (var->yres_virtual >
                        info->var.xres * FB2_NUMBER_OF_BUFFERS) ||
                (var->yres_virtual < info->var.xres ))
        {
            return -EINVAL;
        }
    }
    else
    {
        if((var->xres != info->var.xres) ||
                (var->yres != info->var.yres) ||
                (var->xres_virtual != info->var.xres) ||
                (var->yres_virtual >
                        info->var.yres * FB2_NUMBER_OF_BUFFERS) ||
                (var->yres_virtual < info->var.yres ))
        {
            return -EINVAL;
        }
    }
    if((var->xoffset != info->var.xoffset) ||
            (var->bits_per_pixel != info->var.bits_per_pixel) ||
            (var->grayscale != info->var.grayscale))
    {
        return -EINVAL;
    }
    return 0;
}

/* Handles screen rotation if device supports it. */
static int mtk_fb_2_set_par(struct fb_info *info)
{
    struct mtk_fb *fb = container_of(info, struct mtk_fb, fb);
    if(fb->rotation != fb->fb.var.rotate)
    {
        info->fix.line_length = drv_2->get_line_length(info->var.xres, FB2_BITS_PER_PIXEL);
        fb->rotation = fb->fb.var.rotate;
    }
    return 0;
}

/* Pan the display if device supports it. */
static int mtk_fb_2_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
{
#if 0
    struct mtk_fb *fb __attribute__ ((unused))
    = container_of(info, struct mtk_fb, fb);
    drv_2->set_base(fb->fb.fix.smem_start + fb->fb.var.xres *
            4 * var->yoffset, var->yoffset);
#else
    drv_2->set_base(info->fix.smem_start + var->yoffset * _get_line_length(info->var.xres, FB2_BITS_PER_PIXEL),
            var->yoffset);
#endif
    return 0;
}
static HANDLE_T _h_fb2_mutex = 0;

static UINT32 fb2_rgn;
static UINT32 fb2_rgn_list;
// double buffer
static UINT32 fb2_rgn_2 = 0;
static UINT32 fb2_rgn_list_2 = 0;
//static BOOL _fg_fb2_flip = FALSE;
static UINT32 _fb2_bufs[FB2_NUMBER_OF_BUFFERS+2] =
{   0, 0, 0, 0};

static UINT32 _fb2_num_wins = 0;
static UINT32 _fb2_front_buf = 0;
static UINT32 _fb2_back_buf = 0;
static mtk_fb_win_t _fb2_wins[2] =
{
    {   -1, 0, 0, 0, 0, 0, 0, 0},
    {   -1, 0, 0, 0, 0, 0, 0, 0}
};

static int _fb2_find_win(mtk_fb_win_t wins[], int pid)
{
    UINT32 i;
    for(i = 0; i < FB2_MAX_NUM_OF_WINS; i++)
    {
        if(wins[i].pid == pid)
        {
            break;
        }
    }
    if(i >= FB2_MAX_NUM_OF_WINS)
    {
        return -EFAULT;
    }
    return i;
}

static int _mtk_fb_2_set_ioctl(void* pvArg)
{
    int i4Ret = 0;
    //    unsigned short u2PanelWidth;
    //    unsigned short u2PanelHeight;
    UINT32 u4Region = fb2_rgn;

    struct mt53fb_set rSet;

    if(copy_from_user(&rSet, pvArg, sizeof(struct mt53fb_set)))
    {
        printk(KERN_WARNING "_mt53fb_set_ioctl() - copy_from_user() fail\n");
        return -EFAULT;
    }

    if(rSet.mask & MT53FB_SET_MASK_COLORKEYEN)
    {
        if(OSD_RGN_Set(u4Region, OSD_RGN_COLOR_KEY_EN, rSet.rSetting.u4ColorKeyEn) != OSD_RET_OK)
        {
            return -EINVAL;
        }
    }

    if(rSet.mask & MT53FB_SET_MASK_COLORKEY)
    {
        if(OSD_RGN_Set(u4Region, OSD_RGN_COLOR_KEY, rSet.rSetting.u4ColorKey) != OSD_RET_OK)
        {
            return -EINVAL;
        }
    }

    return i4Ret;
}

static int _mtk_fb_2_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg)
{
    void __user *pvArg = (void __user *)arg;
    INT32 i4Ret = 0;
    switch(cmd)
    {
        case FBIO_SET:
        i4Ret = _mtk_fb_2_set_ioctl(pvArg);
        break;
        case 0x4651:
        printk("Enable POSD: %d\n", FB2_OSD);
        OSD_PLA_Enable(FB2_OSD, TRUE);
        break;
        case 0x4652:
        printk("Disable POSD: %d\n", FB2_OSD);
        OSD_PLA_Enable(FB2_OSD, FALSE);
        break;
        case 0x4650:
        {
            struct fb_var_screeninfo var; /* Current var */

            if(copy_from_user((void*)&var, pvArg, sizeof(struct fb_var_screeninfo)))
            {
                printk(KERN_WARNING "mt53fb fbio set base() error: Copy from user error!\n");
                return -EFAULT;
            }

            _u2PanelWidth = OSD_BASE_GetDisplayWidth();
            _u2PanelHeight = OSD_BASE_GetDisplayHeight();
            /* Fake 1024x768
             _u2PanelWidth = 1024;
             _u2PanelHeight = 768;
             */
            if (IS_PANEL_PDP)
            {
                if(_u2PanelWidth == 1024)
                _u2PanelHeight = _u2PanelHeight/2;
            }

#ifdef OSD_DSC_PDP
            u4FB2_SrcPdpWidth = _u2PanelWidth;
            u4FB2_SrcPdpHeight = _u2PanelHeight;
#endif

            if(var.reserved[0])
            {
#ifdef OSD_DSC_PDP
                // If using 1024x768 PDP panel & window size > panel size, need IMGRZ scaling down
                if((_u2PanelWidth == 1024 && (var.reserved[1] > _u2PanelWidth)) || (_u2PanelHeight == 768 && (var.reserved[2] > _u2PanelHeight)))
                {
                    u4FB2_SrcPdpWidth = var.reserved[1];
                    u4FB2_SrcPdpHeight = var.reserved[2];
                    OSD_SC_Scale(FB2_OSD_SCALER, TRUE, _u2PanelWidth, _u2PanelHeight,
                            _u2PanelWidth, _u2PanelHeight);

                }
                else
#endif
                {
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_W, var.reserved[1]);
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_H, var.reserved[2]);
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_DISP_W, var.reserved[1]);
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_DISP_H, var.reserved[2]);
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_OUT_W, var.reserved[1]);
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_OUT_H, var.reserved[2]);
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_POS_X, 0);
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_POS_Y, 0);
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_PITCH, _get_line_length(FB2_WIDTH, FB2_BITS_PER_PIXEL));
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_ADDR, _fb2_bufs[0]);
                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_W, var.reserved[1]);
                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_H, var.reserved[2]);
                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_DISP_W, var.reserved[1]);
                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_DISP_H, var.reserved[2]);
                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_OUT_W, var.reserved[1]);
                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_OUT_H, var.reserved[2]);
                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_POS_X, 0);
                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_POS_Y, 0);
                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_PITCH, _get_line_length(FB2_WIDTH, FB2_BITS_PER_PIXEL));
                    if(var.reserved[3] == 5)
                    {
                        i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_ADDR, _fb2_bufs[2]);
                    }
                    else
                    {
                        i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_ADDR, _fb2_bufs[1]);
                    }
                    OSD_SC_Scale(FB2_OSD_SCALER, TRUE, var.reserved[1], var.reserved[2],
                            _u2PanelWidth, _u2PanelHeight);
                }
            }
            else
            {

#ifdef OSD_DSC_PDP
                if(_u2PanelWidth == 1024 && _u2PanelHeight == 768)
                {
                    if((var.reserved[1] + var.reserved[3] > _u2PanelWidth) || (var.reserved[2] + var.reserved[4] > _u2PanelHeight))
                    {
                        // If using 1024x768 PDP panel & window size > panel size, not supported because it cannot fit in panel display
                        i4Ret = -1;
                    }
                    else
                    {
                        i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_W, _u2PanelWidth);
                        i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_H, _u2PanelHeight);
                        i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_DISP_W, _u2PanelWidth);
                        i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_DISP_H, _u2PanelHeight);
                        i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_OUT_W, _u2PanelWidth);
                        i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_OUT_H, _u2PanelHeight);
                        i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_POS_X, 0);
                        i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_POS_Y, 0);
                        i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_PITCH, _get_line_length(FB2_WIDTH, FB2_BITS_PER_PIXEL));
                        i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_ADDR, _fb2_bufs[0]);

                        i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_W, _u2PanelWidth);
                        i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_H, _u2PanelHeight);
                        i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_DISP_W, _u2PanelWidth);
                        i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_DISP_H, _u2PanelHeight);
                        i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_OUT_W, _u2PanelWidth);
                        i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_OUT_H, _u2PanelHeight);
                        i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_POS_X, 0);
                        i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_POS_Y, 0);
                        i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_PITCH, _get_line_length(FB2_WIDTH, FB2_BITS_PER_PIXEL));
                        i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_ADDR, _fb2_bufs[2]);
                        OSD_SC_Scale(FB2_OSD_SCALER, TRUE, _u2PanelWidth, _u2PanelHeight,
                                _u2PanelWidth, _u2PanelHeight);
                    }
                }
                else
#endif
                {
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_W, var.xres);
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_H, var.yres);
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_DISP_W, var.xres);
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_DISP_H, var.yres);
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_OUT_W, var.xres);
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_OUT_H, var.yres);
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_POS_X, 0);
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_POS_Y, 0);
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_PITCH, _get_line_length(FB2_WIDTH, FB2_BITS_PER_PIXEL));
                    i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_ADDR, _fb2_bufs[0]);

                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_W, var.xres);
                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_H, var.yres);
                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_DISP_W, var.xres);
                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_DISP_H, var.yres);
                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_OUT_W, var.xres);
                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_OUT_H, var.yres);
                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_POS_X, 0);
                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_POS_Y, 0);
                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_PITCH, _get_line_length(FB2_WIDTH, FB2_BITS_PER_PIXEL));
                    i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_ADDR, _fb2_bufs[2]);
                    OSD_SC_Scale(FB2_OSD_SCALER, TRUE, var.xres, var.yres,
                            _u2PanelWidth, _u2PanelHeight);
                }
            }

            /* Disable 3D stereoscopic mode */
            _u1FB2_3DMode = 0;
            OSD_PLA_Set3DMode(FB2_OSD, 0);

            break;
        }

        case 0x4653:
        {
            fbdev_blit fb_blit;
            if(copy_from_user((void*)&fb_blit, pvArg, sizeof(fbdev_blit)))
            {
                printk(KERN_WARNING "mtk fb 1 blit error: Copy from user error!\n");
                return -EFAULT;
            }

            VERIFY(x_sema_lock(_h_fb2_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);

            GFX_Lock();

            GFX_SetSrc((UINT8 *)fb_blit.src_addr, CM_ARGB8888_DIRECT32, fb_blit.src_pitch);

            GFX_SetDst((UINT8 *)fb_blit.dst_addr, CM_ARGB8888_DIRECT32, fb_blit.dst_pitch);

            GFX_SetBltOpt((UINT32)D_GFXFLAG_NONE, 0x00000000, 0xffffffff);

            GFX_SetAlpha(0);

            GFX_BitBlt(0, 0, 0, 0, fb_blit.width, fb_blit.height);

            GFX_Flush();

            GFX_Task_Wait_Sync();

            GFX_Unlock();

            VERIFY(x_sema_unlock(_h_fb2_mutex) == OSR_OK);

            break;
        }

        case 0x4654:
        {
            fbdev_blit fb_blit;
            if(copy_from_user((void*)&fb_blit, pvArg, sizeof(fbdev_blit)))
            {
                printk(KERN_WARNING "mtk fb blit error: Copy from user error!\n");
                return -EFAULT;
            }

            VERIFY(x_sema_lock(_h_fb2_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);

            GFX_Lock();

            GFX_SetSrc((UINT8 *)fb_blit.src_addr, CM_RGB565_DIRECT16, fb_blit.src_pitch);

            GFX_SetDst((UINT8 *)fb_blit.dst_addr, CM_ARGB8888_DIRECT32, fb_blit.dst_pitch);

            GFX_SetBltOpt((UINT32)D_GFXFLAG_NONE, 0x00000000, 0xffffffff);

            GFX_SetAlpha(0xFF);

            GFX_BitBlt(0, 0, 0, 0, fb_blit.width, fb_blit.height);

            GFX_Flush();

            GFX_Task_Wait_Sync();

            GFX_Unlock();

            VERIFY(x_sema_unlock(_h_fb2_mutex) == OSR_OK);

            break;
        }

        // Register window
        case 0x4655:
        {
            INT32 i;
            fbdev_ipc fb_ipc;

            if(copy_from_user((void*)&fb_ipc, pvArg, sizeof(fbdev_ipc)))
            {
                printk(KERN_WARNING "/dev/fb2 error: Copy from user error!\n");
                return -EFAULT;
            }

            VERIFY(x_sema_lock(_h_fb2_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);
            if(_fb2_num_wins > 2)
            {
                VERIFY(x_sema_unlock(_h_fb2_mutex) == OSR_OK);
                printk("/dev/fb2 register win error: Maximum 2 wins only!\n");
                return -EFAULT;
            }

            // Search for free entry
            if((i = _fb2_find_win(_fb2_wins, -1)) < 0)
            {
                VERIFY(x_sema_unlock(_h_fb2_mutex) == OSR_OK);
                return -EFAULT;
            }
            // Add window and register window properties
            _fb2_num_wins ++;
            _fb2_wins[i].pid = fb_ipc.pid;
            _fb2_wins[i].x = fb_ipc.x;
            _fb2_wins[i].y = fb_ipc.y;
            _fb2_wins[i].w = fb_ipc.w;
            _fb2_wins[i].h = fb_ipc.h;
            _fb2_wins[i].dirty = 0;
            _fb2_wins[i].copying = 0;
            _fb2_wins[i].swaping = 0;
            VERIFY(x_sema_unlock(_h_fb2_mutex) == OSR_OK);
            printk("/dev/fb2 0x4654 ioctl: Create window: offset (%d, %d) size (%d x%d)!\n",
                    fb_ipc.x, fb_ipc.y, fb_ipc.w, fb_ipc.h);

            // Clearing of buf
            GFX_Lock();
            GFX_SetColor(0);
            if(fb_ipc.src_fmt_argb8888)
            {
                GFX_SetDst((UINT8 *)_fb2_bufs[2], CM_ARGB8888_DIRECT32, _get_line_length(fb_2->fb.var.xres, FB2_BITS_PER_PIXEL));
            }
            else
            {
                GFX_SetDst((UINT8 *)_fb2_bufs[2], CM_RGB565_DIRECT16, _get_line_length(fb_2->fb.var.xres, FB2_BITS_PER_PIXEL));
            }
            GFX_Fill(_fb2_wins[i].x, _fb2_wins[i].y, _fb2_wins[i].w, _fb2_wins[i].h);
            GFX_Flush();
            GFX_Task_Wait_Sync();
            GFX_Unlock();
            break;
        }

        // Copy from rendering target to framebuffer
        case 0x4656:
        {
            INT32 i;
            fbdev_ipc fb_ipc;
            if(copy_from_user((void*)&fb_ipc, pvArg, sizeof(fbdev_ipc)))
            {
                printk(KERN_WARNING "/dev/fb2 0x4656 ioctl error: Copy from user error!\n");
                return -EFAULT;
            }
#if FB2_ENABLE_COMPOSER
            VERIFY(x_sema_lock(_h_fb2_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);
            // Search for entry
            if((i = _fb2_find_win(_fb2_wins, fb_ipc.pid)) < 0)
            {
                VERIFY(x_sema_unlock(_h_fb2_mutex) == OSR_OK);
                return -EFAULT;
            }
            // Indicates this proc copying
            _fb2_wins[i].copying = 1;
            fb_ipc.curr_back_buf = _fb2_back_buf;

            GFX_Lock();
            if(fb_ipc.src_fmt_argb8888)
            {
                GFX_SetSrc((UINT8 *)fb_ipc.src_buf, CM_ARGB8888_DIRECT32, fb_ipc.src_pitch);
                GFX_SetAlpha(0);
            }
            else
            {
                GFX_SetSrc((UINT8 *)fb_ipc.src_buf, CM_RGB565_DIRECT16, fb_ipc.src_pitch);
                GFX_SetAlpha(0xFF);
            }
            GFX_SetDst((UINT8 *)_fb2_back_buf, CM_ARGB8888_DIRECT32, _get_line_length(fb_2->fb.var.xres, FB2_BITS_PER_PIXEL));
            GFX_SetBltOpt((UINT32)D_GFXFLAG_NONE, 0x00000000, 0xffffffff);
            GFX_BitBlt(0, 0, _fb2_wins[i].x, _fb2_wins[i].y, _fb2_wins[i].w, _fb2_wins[i].h);
            GFX_Flush();
            GFX_Task_Wait_Sync();
            GFX_Unlock();

            if(copy_to_user(pvArg, (void*)&fb_ipc, sizeof(fbdev_ipc)))
            {
                printk(KERN_WARNING "/dev/fb2 0x4656 ioctl error: Copy to user error!\n");
                return -EFAULT;
            }

            // Indicates this proc copy is done, waiting for flipping
            _fb2_wins[i].copying = 0;

            // Indicates this proc has been updated but not yet flipped
            _fb2_wins[i].dirty = 1;

            VERIFY(x_sema_unlock(_h_fb2_mutex) == OSR_OK);
#else
            VERIFY(x_sema_lock(_h_fb2_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);
            // Search for entry
            if((i = _fb2_find_win(_fb2_wins, fb_ipc.pid)) < 0)
            {
                VERIFY(x_sema_unlock(_h_fb2_mutex) == OSR_OK);
                return -EFAULT;
            }
            // Indicates this proc copying
            _fb2_wins[i].copying = 1;
            fb_ipc.curr_back_buf = _fb2_front_buf;

            GFX_Lock();
            if(fb_ipc.src_fmt_argb8888)
            {
                GFX_SetSrc((UINT8 *)fb_ipc.src_buf, CM_ARGB8888_DIRECT32, fb_ipc.src_pitch);
                GFX_SetAlpha(0);
            }
            else
            {
                GFX_SetSrc((UINT8 *)fb_ipc.src_buf, CM_RGB565_DIRECT16, fb_ipc.src_pitch);
                GFX_SetAlpha(0xFF);
            }
            GFX_SetDst((UINT8 *)_fb2_front_buf, CM_ARGB8888_DIRECT32, _get_line_length(fb_2->fb.var.xres, FB2_BITS_PER_PIXEL));
            GFX_SetBltOpt((UINT32)D_GFXFLAG_NONE, 0x00000000, 0xffffffff);
            GFX_BitBlt(0, 0, _fb2_wins[i].x, _fb2_wins[i].y, _fb2_wins[i].w, _fb2_wins[i].h);
            GFX_Flush();
            GFX_Task_Wait_Sync();
            GFX_Unlock();
            VERIFY(x_sema_unlock(_h_fb2_mutex) == OSR_OK);
#endif /* FB2_ENABLE_COMPOSER */
            break;
        }

        // Swapping. If necessary, copy other non-dirty area from front framebuffer to back framebuffer
        case 0x4657:
        {
            INT32 i;
            fbdev_ipc fb_ipc;
            if(copy_from_user((void*)&fb_ipc, pvArg, sizeof(fbdev_ipc)))
            {
                printk(KERN_WARNING "/dev/fb2 0x4657 ioctl error: Copy from user error!\n");
                return -EFAULT;
            }

#if FB2_ENABLE_COMPOSER
            VERIFY(x_sema_lock(_h_fb2_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);
            // Search for entry
            if((i = _fb2_find_win(_fb2_wins, fb_ipc.pid)) < 0)
            {
                VERIFY(x_sema_unlock(_h_fb2_mutex) == OSR_OK);
                return -EFAULT;
            }
            // Become non-dirty because other app has call swap
            if(_fb2_wins[i].dirty == 0)
            {
                // Indicates this proc is not performed swapping
                _fb2_wins[i].swaping = 0;
                _fb2_wins[i].dirty = 0;

                VERIFY(x_sema_unlock(_h_fb2_mutex) == OSR_OK);
                printk("PID[%d] /dev/fb2 Ignore swaping because other is doing it. Waiting for other swaping\n", _fb2_wins[i].pid);
                // wait for other app's blitting complete
                do
                {
                    x_thread_delay(1);
                }while(_fb2_wins[i == 0?1:0].swaping == 1);
                break;
            }

            // Marked non-dirty because we are flipping it
            _fb2_wins[i].dirty = 0;

            // Indicates this proc is performed swapping
            _fb2_wins[i].swaping = 1;

            if(_fb2_wins[i == 0?1:0].pid != -1 && _fb2_wins[i == 0?1:0].copying)
            {
                printk("PID[%d] /dev/fb2 Waiting for other blitting\n", _fb2_wins[i].pid);
                // wait for other app's blitting complete
                do
                {
                    x_thread_delay(1);
                }while(_fb2_wins[i == 0?1:0].copying == 1);
                _fb2_wins[i == 0?1:0].dirty = 1;
            }
            // Copy from front buf to back buf because other is not dirty

            else if(_fb2_wins[i == 0?1:0].pid != -1 && _fb2_wins[i == 0?1:0].dirty == 0)
            {
                fbdev_blit fb_blit;
                //printk("PID[%d] Copying from front 0x%8x to back 0x%8x because not dirty\n", _fb2_wins[i].pid, _fb2_front_buf, _fb2_back_buf);
                if(copy_from_user((void*)&fb_blit, pvArg, sizeof(fbdev_blit)))
                {
                    printk(KERN_WARNING "/dev/fb2 0x4657 ioctl error: Copy from user error!\n");
                    return -EFAULT;
                }

                GFX_Lock();

                GFX_SetSrc((UINT8 *)_fb2_front_buf,
                        CM_ARGB8888_DIRECT32, _get_line_length(fb_2->fb.var.xres, FB2_BITS_PER_PIXEL));

                GFX_SetDst((UINT8 *)_fb2_back_buf,
                        CM_ARGB8888_DIRECT32, _get_line_length(fb_2->fb.var.xres, FB2_BITS_PER_PIXEL));

                GFX_SetBltOpt((UINT32)D_GFXFLAG_NONE, 0x00000000, 0xffffffff);

                GFX_SetAlpha(0);

                GFX_BitBlt(_fb2_wins[i == 0?1:0].x, _fb2_wins[i == 0?1:0].y, _fb2_wins[i == 0?1:0].x, _fb2_wins[i == 0?1:0].y, _fb2_wins[i == 0?1:0].w, _fb2_wins[i == 0?1:0].h);

                GFX_Flush();

                GFX_Task_Wait_Sync();

                GFX_Unlock();

            }

            if(_fb2_front_buf == _fb2_bufs[0])
            {
                UINT32 u4BmpAddr;
                // printk("Flip to 0x%8x\n", _fb2_bufs[2]);
                OSD_RGN_Get((UINT32)fb2_rgn_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
                if((u4BmpAddr & 0x3FFFFFFF) != _fb2_bufs[2])
                {
                    // printk("_fb2_bufs[0] being altered! Set back to 0x%8x\n", _fb2_bufs[0]);
                    OSD_RGN_Set((UINT32)fb2_rgn_2, OSD_RGN_BMP_ADDR, _fb2_bufs[2]);
                }
                CHECK_FAIL(OSD_PLA_FlipTo(FB2_OSD, fb2_rgn_list_2), 0);
            }
            else if(_fb2_front_buf == _fb2_bufs[1])
            {
                printk("/dev/fb2 0x4657 ioctl not support in stretch == 1 mode\n");
            }
            else if(_fb2_front_buf == _fb2_bufs[2])
            {
                UINT32 u4BmpAddr;
                // printk("Flip to 0x%8x\n", _fb2_bufs[2]);
                OSD_RGN_Get((UINT32)fb2_rgn, OSD_RGN_BMP_ADDR, &u4BmpAddr);
                if((u4BmpAddr & 0x3FFFFFFF) != _fb2_bufs[0])
                {
                    // printk("_fb2_bufs[0] being altered! Set back to 0x%8x\n", _fb2_bufs[0]);
                    OSD_RGN_Set((UINT32)fb2_rgn, OSD_RGN_BMP_ADDR, _fb2_bufs[0]);
                }
                CHECK_FAIL(OSD_PLA_FlipTo(FB2_OSD, fb2_rgn_list), 0);
            }

            // Swap role of back & front
            if(_fb2_back_buf == _fb2_bufs[0])
            {
                _fb2_front_buf = _fb2_bufs[0];
                _fb2_back_buf = _fb2_bufs[2];
            }
            else
            {
                _fb2_front_buf = _fb2_bufs[2];
                _fb2_back_buf = _fb2_bufs[0];
            }

            // Indicates swaping done.
            _fb2_wins[i].swaping = 0;
            _fb2_wins[i == 0?1:0].dirty = 0;
            VERIFY(x_sema_unlock(_h_fb2_mutex) == OSR_OK);
#else
            CHECK_FAIL(OSD_PLA_FlipTo(FB2_OSD, fb2_rgn_list), 0);
#endif /* FB2_ENABLE_COMPOSER */
            break;
        }

        /* Removing pid from compose list */
        case 0x4658:
        {
            INT32 i;
            fbdev_ipc fb_ipc;
            if(copy_from_user((void*)&fb_ipc, pvArg, sizeof(fbdev_ipc)))
            {
                printk(KERN_WARNING "/dev/fb2 0x4658 ioctl error: Copy from user error!\n");
                return -EFAULT;
            }
            VERIFY(x_sema_lock(_h_fb2_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);
            // Search for entry
            if((i = _fb2_find_win(_fb2_wins, fb_ipc.pid)) < 0)
            {
                VERIFY(x_sema_unlock(_h_fb2_mutex) == OSR_OK);
                return -EFAULT;
            }

            // Reset pid
            _fb2_wins[i].pid = -1;
            _fb2_wins[i].copying = 0;
            _fb2_wins[i].swaping = 0;
            _fb2_wins[i].dirty = 0;
            _fb2_num_wins=(_fb2_num_wins==0?0:_fb2_num_wins-1);

            VERIFY(x_sema_unlock(_h_fb2_mutex) == OSR_OK);
            break;
        }

        /* Get all pids */
        case 0x4659:
        {
            INT32 pids[FB2_MAX_NUM_OF_WINS];

            VERIFY(x_sema_lock(_h_fb2_mutex, X_SEMA_OPTION_WAIT) == OSR_OK);
            pids[0] = _fb2_wins[0].pid;
            pids[1] = _fb2_wins[1].pid;
            VERIFY(x_sema_unlock(_h_fb2_mutex) == OSR_OK);
            if(copy_to_user(pvArg, (void*)pids, sizeof(INT32) * FB2_MAX_NUM_OF_WINS))
            {
                printk(KERN_WARNING "/dev/fb2 0x4659 ioctl error: Copy from user error!\n");
                return -EFAULT;
            }
            break;
        }
#if 0
        case 0x4660:
        {
            struct fb_var_screeninfo var; /* Current var */

            if(copy_from_user((void*)&var, pvArg, sizeof(struct fb_var_screeninfo)))
            {
                printk(KERN_WARNING "/dev/fb2 0x4660 ioctl error: Copy from user error!\n");
                return -EFAULT;
            }
            if(!var.reserved[0])
            {
                if((var.reserved[1] + var.reserved[3] > _u2PanelWidth) || (var.reserved[2] + var.reserved[4] > _u2PanelHeight))
                {
                    // Requested window is greater than panel size: cannot fit in panel output so return error
                    return -EINVAL;
                }
                else if(var.reserved[1] == _u2PanelWidth && var.reserved[2] == _u2PanelHeight)
                {
                    // Requested window == panel size, always use full screen mode
                    var.reserved[0] = 1;
                }
            }
            else if(var.reserved[1] > _u2PanelWidth || var.reserved[2] > _u2PanelHeight)
            {
                // Requested window is greater than panel size, so scaling down
                var.reserved[1] = _u2PanelWidth;
                var.reserved[2] = _u2PanelHeight;
            }

            if(copy_to_user(pvArg, (void*)&var, sizeof(struct fb_var_screeninfo)))
            {
                printk(KERN_WARNING "/dev/fb2 0x4660 ioctl error: Copy to user error!\n");
                return -EFAULT;
            }

            break;
        }
#endif
        case 0x4661:
        {
            struct fb_var_screeninfo var; /* Current var */

            if(copy_from_user((void*)&var, pvArg, sizeof(struct fb_var_screeninfo)))
            {
                printk(KERN_WARNING "/dev/fb2 0x4661 ioctl error: Copy from user error!\n");
                return -EFAULT;
            }

            if(var.reserved[0] == 1) // PR

            {
                // PR mode buffer 0
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_W, 1280);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_H, 1080);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_DISP_W, 1280);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_DISP_H, 1080);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_OUT_W, 1280);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_OUT_H, 1080);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_POS_X, 0);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_POS_Y, 0);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_PITCH, 1280 * 4);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_ADDR, _fb2_bufs[0]);
                // PR mode buffer 1
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_W, 1280);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_H, 1080);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_DISP_W, 1280);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_DISP_H, 1080);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_OUT_W, 1280);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_OUT_H, 1080);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_POS_X, 0);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_POS_Y, 0);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_PITCH, 1280 * 4);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_ADDR, _fb2_bufs[2]);

                OSD_SC_Scale(FB2_OSD_SCALER, TRUE, 1280, _u2PanelHeight,
                        _u2PanelWidth, _u2PanelHeight);

            }
            else if(var.reserved[0] == 2) // SG

            {
                // SG mode L frame
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_W, 1280);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_H, 720);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_DISP_W, 1280);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_DISP_H, 720);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_OUT_W, 1280);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_OUT_H, 720);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_POS_X, 0);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_POS_Y, 0);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_PITCH, 2560 * 4);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_ADDR, _fb2_bufs[0]);
                // SG mode L frame 2
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_W, 1280);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_H, 720);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_DISP_W, 1280);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_DISP_H, 720);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_OUT_W, 1280);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_OUT_H, 720);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_POS_X, 0);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_POS_Y, 0);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_PITCH, 2560 * 4);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_ADDR, _fb2_bufs[2]);
#ifdef OSD_DSC_PDP
                // SG mode R frame
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp, OSD_RGN_BMP_W, 1280);
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp, OSD_RGN_BMP_H, 720);
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp, OSD_RGN_DISP_W, 1280);
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp, OSD_RGN_DISP_H, 720);
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp, OSD_RGN_OUT_W, 1280);
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp, OSD_RGN_OUT_H, 720);
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp, OSD_RGN_POS_X, 0);
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp, OSD_RGN_POS_Y, 0);
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp, OSD_RGN_BMP_PITCH, 2560 * 4);
                // Offset 1280 pixels is 1st line of R frame
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp, OSD_RGN_BMP_ADDR, _fb2_bufs[0] + 1280 * 4);
                // SG mode R frame 2
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp_2, OSD_RGN_BMP_W, 1280);
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp_2, OSD_RGN_BMP_H, 720);
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp_2, OSD_RGN_DISP_W, 1280);
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp_2, OSD_RGN_DISP_H, 720);
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp_2, OSD_RGN_OUT_W, 1280);
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp_2, OSD_RGN_OUT_H, 720);
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp_2, OSD_RGN_POS_X, 0);
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp_2, OSD_RGN_POS_Y, 0);
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp_2, OSD_RGN_BMP_PITCH, 2560 * 4);
                // Offset 1280 pixels is 1st line of R frame 2
                i4Ret = OSD_RGN_Set(fb2_rgn_pdp_2, OSD_RGN_BMP_ADDR, _fb2_bufs[2] + 1280 * 4);
#endif /* OSD_DSC_PDP */
                OSD_SC_Scale(FB2_OSD_SCALER, TRUE, 1280, 720,
                        _u2PanelWidth, _u2PanelHeight);

            }
            else
            {
                i4Ret = -EFAULT;
            }

            _u1FB2_3DMode = var.reserved[0];
            // Indicates 3D mode is not yet applied
            _u1FB2_3DModeApplied = 0;
            break;
        }

        // 3D supported Flip
        case 0x4662:
        {
            struct fb_var_screeninfo var; /* Current var */

            if(copy_from_user((void*)&var, pvArg, sizeof(struct fb_var_screeninfo)))
            {
                printk(KERN_WARNING "/dev/fb1 0x4662 ioctl error: Copy from user error!\n");
                return -EFAULT;
            }

            // Force to apply the region width and height upon flip
            if(var.reserved[0] != 1 && var.reserved[0] != 2)
            {
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_BMP_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_DISP_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_DISP_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_OUT_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(fb2_rgn, OSD_RGN_OUT_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BMP_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_DISP_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_DISP_H, var.reserved[2]);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_OUT_W, var.reserved[1]);
                i4Ret = OSD_RGN_Set(fb2_rgn_2, OSD_RGN_OUT_H, var.reserved[2]);
            }

            // Check for 1024x768 PDP panel case
#ifdef OSD_DSC_PDP
            if((_u2PanelWidth == 1024 && (var.reserved[1] > _u2PanelWidth)) || (_u2PanelHeight == 768 && (var.reserved[2] > _u2PanelHeight)))
            {
                u4FB2_SrcPdpWidth = var.reserved[1];
                u4FB2_SrcPdpHeight = var.reserved[2];
                OSD_SC_Scale(FB2_OSD_SCALER, TRUE, _u2PanelWidth, _u2PanelHeight,
                        _u2PanelWidth, _u2PanelHeight);

            }
#endif /* OSD_DSC_PDP */
            i4Ret = _fb_2_set_base(fb_2->fb.fix.smem_start + var.yoffset * _get_line_length(fb_2->fb.var.xres, FB2_BITS_PER_PIXEL),
                    var.yoffset);
            break;
        }

        // get the panel info
        case 0x4663:
        {
            int panel_info[3];

            // pdp panel flag; 1 - pdp panel, 0 - non pdp panel
            panel_info[0] =
#if defined(CC_MT5398) || defined(CC_MT5399)
                            (IS_PANEL_2D_N_3D_L12R12)
#else
                            (IS_PANEL_L12R12)
#endif
                            ? 1 : 0;
            panel_info[1] = OSD_BASE_GetDisplayWidth();  // panel width
            panel_info[2] = OSD_BASE_GetDisplayHeight(); // panel height

            // if it's PDP panel and 3d mode, the height of the panel is double, so it needs dividing by 2.
            if(IS_PANEL_L12R12)
                panel_info[2] >>= 1;

            /*
             * panel_info[0]: pdp panel flag
             * panel_info[1]: panel width
             * panel_info[2]: panel height
             */
            if(copy_to_user(pvArg, (const void *)&panel_info, sizeof(panel_info)))
			{
				printk(KERN_WARNING "/dev/fb1 0x4663 ioctl error: Copy to user error!\n");
				return -EFAULT;
			}
            break;
        }

        default:
        //printk(KERN_WARNING "_mt53fb_ioctl(): not support ioctl cmd %d\n", cmd);
        i4Ret = -EINVAL;
        break;
    }
    return i4Ret;
}

static struct fb_ops mtk_fb_2_ops =
{
    .owner = THIS_MODULE,
    .fb_check_var = mtk_fb_2_check_var,
    .fb_set_par = mtk_fb_2_set_par,
    .fb_setcolreg = mtk_fb_2_setcolreg,
    .fb_pan_display = mtk_fb_2_pan_display,

    /* These are generic software based fb functions */
    .fb_fillrect = cfb_fillrect,
    .fb_copyarea = cfb_copyarea,
    .fb_imageblit = cfb_imageblit,
    .fb_ioctl = _mtk_fb_2_ioctl,
};

static int mtk_fb_2_setup(void)
{
    int ret;
    //size_t framesize;

    printk("MTK /DEV/FB0 mtk_fb_probe\n");
    fb_2 = kzalloc(sizeof(*fb_2), GFP_KERNEL);
    if(fb_2 == NULL)
    {
        ret = -ENOMEM;
        goto err_fb_alloc_failed;
    }

    if(_h_fb2_mutex == 0)
    {
        x_sema_create(&_h_fb2_mutex, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK);
    }

    //width = ((osd2_output_width==0)?width:osd2_output_width);
    //height = ((osd2_output_height==0)?height:osd2_output_height);

    fb_2->fb.fbops = &mtk_fb_2_ops;

    /* These modes are the ones currently required by Android */

    fb_2->fb.flags = FBINFO_FLAG_DEFAULT;
    fb_2->fb.pseudo_palette = fb_2->cmap;
    fb_2->fb.fix.type = FB_TYPE_PACKED_PIXELS;
    fb_2->fb.fix.visual = FB_VISUAL_TRUECOLOR;
    fb_2->fb.fix.line_length = drv_2->get_line_length(fb2_width, FB2_BITS_PER_PIXEL);
    fb_2->fb.fix.accel = FB_ACCEL_NONE;
    fb_2->fb.fix.ypanstep = 1;

    fb_2->fb.var.xres = fb2_width;
    fb_2->fb.var.yres = fb2_height;
    fb_2->fb.var.xres_virtual = fb2_width;
    fb_2->fb.var.yres_virtual = fb2_height * FB2_NUMBER_OF_BUFFERS;
    fb_2->fb.var.bits_per_pixel = FB2_BITS_PER_PIXEL;
    fb_2->fb.var.activate = FB_ACTIVATE_NOW;
    fb_2->fb.var.height = fb2_height;
    fb_2->fb.var.width = fb2_width;

    fb_2->fb.var.red.offset = 16;
    fb_2->fb.var.red.length = 8;
    fb_2->fb.var.green.offset = 8;
    fb_2->fb.var.green.length = 8;
    fb_2->fb.var.blue.offset = 0;
    fb_2->fb.var.blue.length = 8;
    fb_2->fb.var.transp.offset = 24;
    fb_2->fb.var.transp.length = 8;

    ret = drv_2->setup_buffer(
            fb2_width,
            fb2_height,
            fb_2->fb.var.yres_virtual,
            FB2_BITS_PER_PIXEL,
            MTK_DRV_FB_CM_ARGB8888,
            (void**)&fb_2->fb.screen_base,
            (unsigned long**)&fb_2->fb.fix.smem_start,
            (unsigned long**)&fb_2->fb.fix.smem_len);

    if(ret)
    goto err_fb_drv_failed;

    ret = fb_set_var(&fb_2->fb, &fb_2->fb.var);
    if(ret)
    goto err_fb_set_var_failed;

    ret = register_framebuffer(&fb_2->fb);
    if(ret)
    goto err_register_framebuffer_failed;
    printk("/DEV/FB2 probe successful\n");
    return 0;

    err_register_framebuffer_failed:
    err_fb_set_var_failed:
    err_fb_drv_failed:
    kfree(fb_2);
    err_fb_alloc_failed:
    printk("/DEV/FB2 probe failed\n");
    return ret;
}

static void _fb_2_exit(void)
{
    /* TODO:  Release all resources .... */
}

static int _fb_2_setup_buffer(
        u32 xres,
        u32 yres,
        u32 vyres,
        u32 bits_per_pix,
        u32 cm,
        void ** p_screen_base,
        unsigned long ** p_phy_start,
        unsigned long ** p_size)
{
    // From DFB partition offset 32MB
    //*p_phy_start = (BSP_GetFbmMemAddr() - DRVCUST_OptGet(eDirectFBMemSize) + 1920 * 1080 * 4 * 4) & PAGE_MASK;
    // Share with DDI_GPU POSD0
	*p_phy_start = (unsigned long*)(fb_osd_get_fb_addr() & PAGE_MASK);	

    /* We Don't need too much memory as DTV mw.*/
    *p_size = (unsigned long*)(vyres * _get_line_length(xres, bits_per_pix) * 2);
    //*p_size = DRVCUST_OptGet(eDirectFBMemSize);
    /* USE DTV-FBM convertion or use ioremap, check whether the core.c mapped
     * FB memory when system init.
     * */
    *p_screen_base = ioremap((unsigned long)*p_phy_start, (size_t)*p_size);
    printk("mt5395 fb 0 get buffer :scr base:0x%x, phy start:0x%x, size %d\n",
            (unsigned int)*p_screen_base,
            (unsigned int)*p_phy_start,
            (int)*p_size);
#ifdef BOOTUP_TIME_CUT_DOWN
    memset((void*)*p_screen_base, 0x0, (int)*p_size);
#endif

    CHECK_FAIL(OSD_RGN_LIST_Create(&fb2_rgn_list), 0);

    CHECK_FAIL(OSD_RGN_Create(&fb2_rgn, 16, 16,
                    *p_phy_start,
                    _get_drv_cm(cm),
                    _get_line_length(xres, FB2_BITS_PER_PIXEL),
                    0, 0, 16, 16), 0);

    CHECK_FAIL(OSD_RGN_Set(fb2_rgn, OSD_RGN_BIG_ENDIAN, 0), 0);

    CHECK_FAIL(OSD_RGN_Insert(fb2_rgn, fb2_rgn_list), 0);

    /* TODO: Which plane?*/
    CHECK_FAIL(OSD_PLA_FlipTo(FB2_OSD, fb2_rgn_list), 0);

    // Modify for plane alpha
    CHECK_FAIL(OSD_RGN_Set(fb2_rgn, OSD_RGN_MIX_SEL, OSD_BM_PIXEL), 0);

    CHECK_FAIL(OSD_RGN_LIST_Create(&fb2_rgn_list_2), 0);

    CHECK_FAIL(OSD_RGN_Create(&fb2_rgn_2, 16, 16,
                    *p_phy_start + FB2_HEIGHT * _get_line_length(FB2_WIDTH, FB2_BITS_PER_PIXEL),
                    _get_drv_cm(cm),
                    _get_line_length(xres, FB2_BITS_PER_PIXEL),
                    0, 0, 16, 16), 0);

    CHECK_FAIL(OSD_RGN_Set(fb2_rgn_2, OSD_RGN_BIG_ENDIAN, 0), 0);

    CHECK_FAIL(OSD_RGN_Insert(fb2_rgn_2, fb2_rgn_list_2), 0);

    // Modify for plane alpha
    CHECK_FAIL(OSD_RGN_Set(fb2_rgn_2, OSD_RGN_MIX_SEL, OSD_BM_PIXEL), 0);

    _fb2_bufs[0] = (UINT32)*p_phy_start;
    _fb2_bufs[1] = (UINT32)(*p_phy_start) + 1280 * 720 * 4;

#if FB2_ENABLE_COMPOSER
    _fb2_bufs[2] = (UINT32)(*p_phy_start) + FB2_HEIGHT * _get_line_length(FB2_WIDTH, FB2_BITS_PER_PIXEL) * 2;
    // x_memset((void*)(VIRTUAL((UINT32)_fb2_bufs[2])), 0, FB2_HEIGHT * _get_line_length(FB2_WIDTH, FB2_BITS_PER_PIXEL));
#else
    _fb2_bufs[2] = (UINT32)*p_phy_start;
#endif

    _fb2_front_buf = _fb2_bufs[0];
    _fb2_back_buf = _fb2_bufs[2];

#ifdef OSD_DSC_PDP
    u4FB2_PDPFBBuffer1 = _fb2_bufs[1];
    u4FB2_PDPFBBuffer2 = (UINT32)(*p_phy_start) + FB2_HEIGHT * _get_line_length(FB2_WIDTH, FB2_BITS_PER_PIXEL) * 3;
    _fb2_bufs[3] = u4FB2_PDPFBBuffer2;

    CHECK_FAIL(OSD_RGN_LIST_Create(&fb2_rgn_pdp_list), 0);

    CHECK_FAIL(OSD_RGN_Create(&fb2_rgn_pdp, UI_DST_WIDTH, UI_DST_HEIGHT,
                    (void *)u4FB2_PDPFBBuffer1,
                    _get_drv_cm(cm),
                    _get_line_length(UI_DST_WIDTH, FB2_BITS_PER_PIXEL),
                    0, 0, UI_DST_WIDTH, UI_DST_HEIGHT), 0);

    CHECK_FAIL(OSD_RGN_Set(fb2_rgn_pdp, OSD_RGN_BIG_ENDIAN, 0), 0);

    CHECK_FAIL(OSD_RGN_Insert(fb2_rgn_pdp, fb2_rgn_pdp_list), 0);

    /* TODO: Which plane?*/
    CHECK_FAIL(OSD_PLA_FlipTo(FB2_OSD, fb2_rgn_pdp_list), 0);

    // Modify for plane alpha
    CHECK_FAIL(OSD_RGN_Set(fb2_rgn_pdp, OSD_RGN_MIX_SEL, OSD_BM_PIXEL), 0);

    // Create 2nd region
    CHECK_FAIL(OSD_RGN_LIST_Create(&fb2_rgn_pdp_list_2), 0);

    CHECK_FAIL(OSD_RGN_Create(&fb2_rgn_pdp_2, UI_DST_WIDTH, UI_DST_HEIGHT,
                    (void *)u4FB2_PDPFBBuffer2,
                    _get_drv_cm(cm),
                    _get_line_length(UI_DST_WIDTH, FB2_BITS_PER_PIXEL),
                    0, 0, UI_DST_WIDTH, UI_DST_HEIGHT), 0);

    CHECK_FAIL(OSD_RGN_Set(fb2_rgn_pdp_2, OSD_RGN_BIG_ENDIAN, 0), 0);

    CHECK_FAIL(OSD_RGN_Insert(fb2_rgn_pdp_2, fb2_rgn_pdp_list_2), 0);

    // Modify for plane alpha
    CHECK_FAIL(OSD_RGN_Set(fb2_rgn_pdp_2, OSD_RGN_MIX_SEL, OSD_BM_PIXEL), 0);
#endif

    /* TODO: Which scaler:*/
    if(_fgOsdEnabelUpdate)
    {
        CHECK_FAIL(OSD_SC_Scale(FB2_OSD_SCALER, TRUE, xres, yres, OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayWidth()), 0);
    }
    return 0;
}

static int _fb_2_set_base(unsigned long phy_scr_base, unsigned long offset)
{
    UINT32 u4BmpAddr;
    // printk("Before 0x%8x, buf[2]: 0x%8x\n", phy_scr_base, _fb2_bufs[2]);
    if(phy_scr_base == _fb2_bufs[0])
    {
#ifdef OSD_DSC_PDP
        if(_u1FB2_3DMode == 1) // PR mode

        {
            if(!_u1FB2_3DModeApplied)
            {
                OSD_PLA_Set3DMode(FB2_OSD, 0);
                _u1FB2_3DModeApplied = 1;
            }
            OSD_RGN_Get((UINT32)fb2_rgn, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != _fb2_bufs[0])
            {
                printk("_fb2_bufs[0] being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, _fb2_bufs[0]);
                OSD_RGN_Set((UINT32)fb2_rgn, OSD_RGN_BMP_ADDR, _fb2_bufs[0]);
            }
            CHECK_FAIL(OSD_PLA_FlipTo(FB2_OSD, fb2_rgn_list), 0);
        }
        else if(_u1FB2_3DMode == 2) // SG mode

        {
            if(!_u1FB2_3DModeApplied)
            {
                OSD_PLA_Set3DMode(FB2_OSD, 1);
                _u1FB2_3DModeApplied = 1;
            }
            // Check if fb2_rgn is correct
            OSD_RGN_Get((UINT32)fb2_rgn, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != _fb2_bufs[0])
            {
                printk("SG R region 1 being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, _fb2_bufs[0]);
                OSD_RGN_Set((UINT32)fb2_rgn, OSD_RGN_BMP_ADDR, _fb2_bufs[0]);
            }
            OSD_RGN_Get((UINT32)fb2_rgn_pdp, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != (_fb2_bufs[0] + 1280 * 4))
            {
                printk("SG L region 1 being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, (_fb2_bufs[0] + 1280 * 4));
                OSD_RGN_Set((UINT32)fb2_rgn_pdp, OSD_RGN_BMP_ADDR, (_fb2_bufs[0] + 1280 * 4));
            }
            OSD_PLA_Set3DRegion(FB2_OSD, fb2_rgn, fb2_rgn_pdp);
        }
        else if((_u2PanelWidth == 1024 && (u4FB2_SrcPdpWidth > _u2PanelWidth)) ||
                (_u2PanelHeight == 768 && (u4FB2_SrcPdpHeight > _u2PanelHeight)))
        {
            OSD_PLA_DsBlit((UINT32)phy_scr_base, 0, 0, u4FB2_SrcPdpWidth, u4FB2_SrcPdpHeight, _get_line_length(fb_2->fb.var.xres, FB2_BITS_PER_PIXEL),
                    u4FB2_PDPFBBuffer1, 0, 0, _u2PanelWidth, _u2PanelHeight, _u2PanelWidth*4, OSD_CM_ARGB8888_DIRECT32);
            OSD_RGN_Get((UINT32)fb2_rgn_pdp, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != u4FB2_PDPFBBuffer1)
            {
                printk("u4PDPFBBuffer1 being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, u4FB2_PDPFBBuffer1);
                OSD_RGN_Set((UINT32)fb2_rgn_pdp, OSD_RGN_BMP_ADDR, u4FB2_PDPFBBuffer1);
            }
            CHECK_FAIL(OSD_PLA_FlipTo(FB2_OSD, fb2_rgn_pdp_list), 0);
        }
        else
#endif
        {
            OSD_RGN_Get((UINT32)fb2_rgn, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != _fb2_bufs[0])
            {
                printk("_fb2_bufs[0] being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, _fb2_bufs[0]);
                OSD_RGN_Set((UINT32)fb2_rgn, OSD_RGN_BMP_ADDR, _fb2_bufs[0]);
            }
            CHECK_FAIL(OSD_PLA_FlipTo(FB2_OSD, fb2_rgn_list), 0);
        }
    }
    else if(phy_scr_base == _fb2_bufs[1])
    {
#ifdef OSD_DSC_PDP
        if(_u1FB2_3DMode == 1) // PR mode

        {
            if(!_u1FB2_3DModeApplied)
            {
                OSD_PLA_Set3DMode(FB2_OSD, 0);
                _u1FB2_3DModeApplied = 1;
            }
            OSD_RGN_Get((UINT32)fb2_rgn_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != _fb2_bufs[1])
            {
                printk("_fb2_bufs[1] being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, _fb2_bufs[1]);
                OSD_RGN_Set((UINT32)fb2_rgn_2, OSD_RGN_BMP_ADDR, _fb2_bufs[1]);
            }
            CHECK_FAIL(OSD_PLA_FlipTo(FB2_OSD, fb2_rgn_list_2), 0);
        }
        else if(_u1FB2_3DMode == 2) // SG mode

        {
            if(!_u1FB2_3DModeApplied)
            {
                OSD_PLA_Set3DMode(FB2_OSD, 1);
                _u1FB2_3DModeApplied = 1;
            }
            // Check if fb2_rgn is correct
            OSD_RGN_Get((UINT32)fb2_rgn_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != _fb2_bufs[2])
            {
                printk("SG L region 2 being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, _fb2_bufs[2]);
                OSD_RGN_Set((UINT32)fb2_rgn_2, OSD_RGN_BMP_ADDR, _fb2_bufs[2]);
            }
            OSD_RGN_Get((UINT32)fb2_rgn_pdp_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != (_fb2_bufs[2] + 1280 * 4))
            {
                printk("SG R region 2 being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, (_fb2_bufs[2] + 1280 * 4));
                OSD_RGN_Set((UINT32)fb2_rgn_pdp_2, OSD_RGN_BMP_ADDR, (_fb2_bufs[2] + 1280 * 4));
            }
            OSD_PLA_Set3DRegion(FB2_OSD, fb2_rgn_2, fb2_rgn_pdp_2);
        }
        else if((_u2PanelWidth == 1024 && (u4FB2_SrcPdpWidth > _u2PanelWidth)) ||
                (_u2PanelHeight == 768 && (u4FB2_SrcPdpHeight > _u2PanelHeight)))
        {
            OSD_PLA_DsBlit((UINT32)phy_scr_base, 0, 0, u4FB2_SrcPdpWidth, u4FB2_SrcPdpHeight, _get_line_length(fb_2->fb.var.xres, FB2_BITS_PER_PIXEL),
                    u4FB2_PDPFBBuffer2, 0, 0, _u2PanelWidth, _u2PanelHeight, _u2PanelWidth*4, OSD_CM_ARGB8888_DIRECT32);
            OSD_RGN_Get((UINT32)fb2_rgn_pdp_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != u4FB2_PDPFBBuffer2)
            {
                printk("u4PDPFBBuffer2 being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, u4FB2_PDPFBBuffer2);
                OSD_RGN_Set((UINT32)fb2_rgn_pdp_2, OSD_RGN_BMP_ADDR, u4FB2_PDPFBBuffer2);
            }
            CHECK_FAIL(OSD_PLA_FlipTo(FB2_OSD, fb2_rgn_pdp_list_2), 0);
        }
        else
#endif
        {
            OSD_RGN_Get((UINT32)fb2_rgn_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != _fb2_bufs[1])
            {
                printk("_fb2_bufs[1] being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, _fb2_bufs[1]);
                OSD_RGN_Set((UINT32)fb2_rgn_2, OSD_RGN_BMP_ADDR, _fb2_bufs[1]);
            }
            CHECK_FAIL(OSD_PLA_FlipTo(FB2_OSD, fb2_rgn_list_2), 0);
        }
    }
    else if(phy_scr_base == _fb2_bufs[2])
    {
#ifdef OSD_DSC_PDP
        if(_u1FB2_3DMode == 1) // PR mode

        {
            if(!_u1FB2_3DModeApplied)
            {
                OSD_PLA_Set3DMode(FB2_OSD, 0);
                _u1FB2_3DModeApplied = 1;
            }
            OSD_RGN_Get((UINT32)fb2_rgn_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != _fb2_bufs[2])
            {
                printk("_fb2_bufs[2] being altered! Set back to 0x%8x\n", _fb2_bufs[2]);
                OSD_RGN_Set((UINT32)fb2_rgn_2, OSD_RGN_BMP_ADDR, _fb2_bufs[2]);
            }
            CHECK_FAIL(OSD_PLA_FlipTo(FB2_OSD, fb2_rgn_list_2), 0);
        }
        else if(_u1FB2_3DMode == 2) // SG mode

        {
            if(!_u1FB2_3DModeApplied)
            {
                OSD_PLA_Set3DMode(FB2_OSD, 1);
                _u1FB2_3DModeApplied = 1;
            }
            // Check if fb2_rgn is correct
            OSD_RGN_Get((UINT32)fb2_rgn_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != _fb2_bufs[2])
            {
                printk("SG L region 2 being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, _fb2_bufs[2]);
                OSD_RGN_Set((UINT32)fb2_rgn_2, OSD_RGN_BMP_ADDR, _fb2_bufs[2]);
            }
            OSD_RGN_Get((UINT32)fb2_rgn_pdp_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != (_fb2_bufs[2] + 1280 * 4))
            {
                printk("SG R region 2 being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, (_fb2_bufs[2] + 1280 * 4));
                OSD_RGN_Set((UINT32)fb2_rgn_pdp_2, OSD_RGN_BMP_ADDR, (_fb2_bufs[2] + 1280 * 4));
            }
            OSD_PLA_Set3DRegion(FB2_OSD, fb2_rgn_2, fb2_rgn_pdp_2);
        }
        else if((_u2PanelWidth == 1024 && (u4FB2_SrcPdpWidth > _u2PanelWidth)) ||
                (_u2PanelHeight == 768 && (u4FB2_SrcPdpHeight > _u2PanelHeight)))
        {
            OSD_PLA_DsBlit((UINT32)phy_scr_base, 0, 0, u4FB2_SrcPdpWidth, u4FB2_SrcPdpHeight, _get_line_length(fb_2->fb.var.xres, FB2_BITS_PER_PIXEL),
                    u4FB2_PDPFBBuffer2, 0, 0, _u2PanelWidth, _u2PanelHeight, _u2PanelWidth*4, OSD_CM_ARGB8888_DIRECT32);
            OSD_RGN_Get((UINT32)fb2_rgn_pdp_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != u4FB2_PDPFBBuffer2)
            {
                printk("u4PDPFBBuffer2 being altered to 0x%8x! Set back to 0x%8x\n", u4BmpAddr, u4FB2_PDPFBBuffer2);
                OSD_RGN_Set((UINT32)fb2_rgn_pdp_2, OSD_RGN_BMP_ADDR, u4FB2_PDPFBBuffer2);
            }
            CHECK_FAIL(OSD_PLA_FlipTo(FB2_OSD, fb2_rgn_pdp_list_2), 0);
        }
        else
#endif
        {
            OSD_RGN_Get((UINT32)fb2_rgn_2, OSD_RGN_BMP_ADDR, &u4BmpAddr);
            if((u4BmpAddr & 0x3FFFFFFF) != _fb2_bufs[2])
            {
                printk("_fb2_bufs[2] being altered! Set back to 0x%8x\n", _fb2_bufs[2]);
                OSD_RGN_Set((UINT32)fb2_rgn_2, OSD_RGN_BMP_ADDR, _fb2_bufs[2]);
            }
            CHECK_FAIL(OSD_PLA_FlipTo(FB2_OSD, fb2_rgn_list_2), 0);
        }
    }
    // Safty guard code for imprecise out vsync isr

    {
        UINT32 u4Val;
        //       UINT32 i = 0;
#if 0
        u4Val = IO_READ32(0xF0021000, 0x224) << 4;
        //printk("After 0x%8x\n", phy_scr_base);

        if((u4Val & 0xFFFFFF) != (phy_scr_base & 0xFFFFFF) )
        {
            printk("Read 0x%8x != set 0x%8x\n", u4Val, phy_scr_base);
            OSD_WaitVsync();
        }
#else
        while(1)
        {
            u4Val = IO_READ32(0xF0021000, 0x224) << 4;
            if((u4Val & 0xFFFFFF) != (phy_scr_base & 0xFFFFFF) )
            {
                break;
            }
        }
#endif
    }

    return 0;
}

static mtk_drv_fb_t mt5395_fb_2 =
{
    .fb_exit = _fb_2_exit,
    .get_line_length = _get_line_length,
    .setup_buffer = _fb_2_setup_buffer,
    .set_base = _fb_2_set_base,
};

int mtk_drv_fb_2_init(mtk_drv_fb_t ** drv)
{
    //OSD_PLA_Enable(FB2_OSD, TRUE);

    *drv = &mt5395_fb_2;
    return 0;
}

#endif
#endif /* CC_SUPPORT_MULTIPLE_FBDEV */


#ifdef CC_DFB_SUPPORT_VDP_LAYER
static BOOL _fgVdpInit = FALSE;
static unsigned char _ucFbgId = 0;
//static unsigned char _aucFbId[2] = {FBM_FB_ID_UNKNOWN, FBM_FB_ID_UNKNOWN};
//static unsigned int _au4FbYAddr[2] = {NULL, NULL};
//static unsigned int _au4FbCAddr[2] = {NULL, NULL};
//static unsigned char _u1FrontFbId = 0;
static E_TDTV_DECODER_INPUT _ePrevDetectedMode = E_TDTV_DECODER_INPUT_2D_P;
//static unsigned int _u4VdpWidth;
//static unsigned int _u4VdpHeight;
#ifdef CC_B2R44K2K_SUPPORT
static UINT32 _u4Is3DMode = 0;
#endif
static UINT32 _u4CropBottomOffset = 0;
static UINT32 _u4CropTopOffset = 0;
static UINT32 _u4CropLeftOffset = 0;
static UINT32 _u4CropRightOffset = 0;
//static UCHAR  _fgLastVdpEnableFg = 0;
#endif

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------

#ifdef CC_DFB_SUPPORT_VDP_LAYER
static BOOL _3DCropFuncCB(TDTV_3D_CROP_INTO_T* prCropInfo)
{
    prCropInfo->fgIsFixSrcAspectRatio = 1;
    prCropInfo->fgIsFullResolution = 1;
    prCropInfo->u4CropBottomOffset = _u4CropBottomOffset;
    prCropInfo->u4CropTopOffset = _u4CropTopOffset;
    prCropInfo->u4CropLeftOffset = _u4CropLeftOffset;
    prCropInfo->u4CropRightOffset = _u4CropRightOffset;
    return 0;
}
static BOOL _VDP_FBGInit(unsigned int u4Width, unsigned int u4Height, unsigned int u4Pitch)
{
    unsigned char ucFbgType, ucFbgId;
    FBM_SEQ_HDR_T* prFbmSeqHdr;
    VDP_REGION_T rRegion;
    FBM_CREATE_FBG_PAR_T rPar;
    
    x_sema_lock(hMt53fbMutex, X_SEMA_OPTION_WAIT);
    UNUSED(ucFbgType);
    UNUSED(ucFbgId);
    UNUSED(prFbmSeqHdr);
    if(!_fgVdpInit)
    {
        UCHAR ucFbgType, ucFbgId;
        FBM_SEQ_HDR_T* prFbmSeqHdr;

        // Set video src
        bApiVideoSetVideoSrc(0, SV_VS_DT1);
        VDP_SetSrcRegion(0, 1, rRegion);
        VDP_SetOutRegion(0, 1, rRegion);
        // VDP_GetEnable(0,&_fgLastVdpEnableFg);
        VDP_SetEnable(0, 1);
        VDP_SetMode(0, VDP_MODE_BG);
        VDP_SetInput(0, 0, 0);
        VDP_B2rSwitch(0, B2R_1);

        x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
        rPar.u1VdecId = (UINT8)0;
        rPar.u1AppMode = FBM_FBG_APP_NORMAL;

        ucFbgType = FBM_SelectGroupType(u4Width, u4Height);
        FBM_SetColorMode(FBM_CM_422);
        ucFbgId = FBM_CreateGroupExt(ucFbgType, FBM_VDEC_JPEG, u4Width, u4Height, &rPar);
        if(ucFbgId == FBM_FBG_ID_UNKNOWN)
        {
            printk("Create Fbg Fail!! input resolution %dx%d\n", u4Width, u4Height);
            x_sema_unlock(hMt53fbMutex);
            return ucFbgId;
        }
        FBM_SetPlayMode(ucFbgId, FBM_FBG_MM_MODE);
        FBM_SetSyncStc(ucFbgId, 0, 0); //none
        FBM_FbgChgNotify(ucFbgId, 0);

        if (u4Pitch == 0)
        u4Pitch = ((u4Width + 15) >> 4) << 4;

        prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
        prFbmSeqHdr->u2HSize = u4Width;
        prFbmSeqHdr->u2VSize = u4Height;
        prFbmSeqHdr->u2OrgHSize = prFbmSeqHdr->u2HSize;
        prFbmSeqHdr->u2OrgVSize = prFbmSeqHdr->u2VSize;
        prFbmSeqHdr->u2LineSize = u4Pitch;
        prFbmSeqHdr->fgProgressiveSeq = TRUE;
        prFbmSeqHdr->fgRasterOrder = TRUE;
        prFbmSeqHdr->fg422Mode = TRUE;
        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30;

        FBM_SetFrameBufferFlag(ucFbgId, FBM_FLAG_SEQ_CHG);

        // Register 3D crop CB
        fpDrvTDTVReg3DCropCB(E_TDTV_CB_3D_CROP_MM, _3DCropFuncCB);

        _ucFbgId = ucFbgId;
        _fgVdpInit = TRUE;

    }

    x_sema_unlock(hMt53fbMutex);
    return TRUE;
}

static BOOL _VDP_FBFlip(unsigned char ucFbgId,
        unsigned char* pu1SrcYAddr, unsigned char* pu1SrcCAddr, BOOL fgIs3DMode, unsigned int ui4_pitch,
        unsigned int u4CropBottomOffset, unsigned int u4CropTopOffset, unsigned int u4CropLeftOffset, unsigned int u4CropRightOffset)
{
    UINT32 u43DInOut, u2InFmt, u2OutFmt;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;

    x_sema_lock(hMt53fbMutex, X_SEMA_OPTION_WAIT);

    if(_fgVdpInit)
    {
        UCHAR ucFbId;
        UINT32 u4DstYAddr, u4DstCAddr, u4Pitch, u4TotalLine, u4W;
        E_TDTV_DECODER_INPUT eDetectedMode;
        FBM_PIC_HDR_T* prFbmPicHdr;

        //Get back frame buffer
        ucFbId = FBM_GetEmptyFrameBuffer(ucFbgId, (UINT32)500);
        if (ucFbId == (UCHAR)FBM_FB_ID_UNKNOWN)
        {
            printk("Get empty frame buffer failed.\n");
            x_sema_unlock(hMt53fbMutex);
            return FALSE;
        }

        if(MT53FB_LOG>=0x2)
        {
            printk("Get empty frame buffer id = %d.\n", ucFbId);
        }

        FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &u4DstYAddr, &u4DstCAddr);
        prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);
        if ((prFbmPicHdr == NULL) || (u4DstYAddr == 0) || (u4DstCAddr == 0))
        {
            printk("Alloc frame buffer failed.\n");
            x_sema_unlock(hMt53fbMutex);
            return FALSE;
        }
        prFbmPicHdr->ucPicStruct = (UCHAR)MPEG_PIC_FRAME;
        prFbmPicHdr->fgProgressiveFrm = TRUE;
        prFbmPicHdr->fgRepFirstFld = FALSE;
        prFbmPicHdr->fgTopFldFirst = FALSE;
        FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE); //default
        FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_READY);

        //Bit blit input pu1SrcYAddr, pu1SrcCAddr to destination u4DstYAddr, u4DstCAddr
        FBM_GetFrameBufferSize(ucFbgId, &u4Pitch, &u4TotalLine);

        u4W = OSD_BASE_GetDisplayWidth();
        u4Pitch = ui4_pitch;
        u4TotalLine = OSD_BASE_GetDisplayHeight();

        if(MT53FB_LOG>=0x2)
        {
            printk("Alloc frame buffer Yaddr = 0x%8x, Caddr = 0x%8x, pitch = %d, width = %d, height = %d\n",
                    u4DstYAddr, u4DstCAddr, u4Pitch, u4W, u4TotalLine);
        }

        //Y
        
        GFX_Lock();
        GFX_MMU_Set_Enable(0x0);
        GFX_Flush_Invalid_DCache();
        GFX_SetSrc(pu1SrcYAddr, (UINT32)CM_RGB_CLUT8, u4Pitch);
        GFX_SetDst((UINT8 *)u4DstYAddr, (UINT32)CM_RGB_CLUT8, u4Pitch);
        GFX_BitBlt(0, 0, 0, 0, u4W, u4TotalLine);
        //C
        GFX_SetSrc(pu1SrcCAddr, (UINT32)CM_RGB_CLUT8, u4Pitch);
        GFX_SetDst((UINT8 *)u4DstCAddr, (UINT32)CM_RGB_CLUT8, u4Pitch);
        GFX_BitBlt(0, 0, 0, 0, u4W, u4TotalLine);
        GFX_Flush();
        GFX_Task_Wait_Sync();
        GFX_Unlock();

        if(MT53FB_LOG>=0x2)
        {
            printk("DFB buffer Yaddr = 0x%8x, Caddr = 0x%8x, 3D mode = %d\n",
                    (UINT32)pu1SrcYAddr, (UINT32)pu1SrcCAddr, fgIs3DMode);
        }

        //Check 3D Type
        if(fgIs3DMode)
        {
            eDetectedMode = E_TDTV_DECODER_INPUT_SBS_P;
        }
        else
        {
            eDetectedMode = E_TDTV_DECODER_INPUT_2D_P;
        }
        u43DInOut = u4DrvTDTVMMModeQuery(eDetectedMode);
        if(eDetectedMode != _ePrevDetectedMode)
        {
            //            u43DInOut = u4DrvTDTVMMModeQuery(eDetectedMode);
            u2InFmt = u43DInOut & 0xFFFF;
            u2OutFmt = (u43DInOut >> 16) & 0xFFFF;
            prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);

            if(prFbmSeqHdr)
            {
                switch(u2OutFmt)
                {
                    case E_TDTV_DECODER_OUTPUT_NATIVE:
                    prFbmSeqHdr->fgB2R3DEnable = FALSE;
                    break;
                    case E_TDTV_DECODER_OUTPUT_3D_FS:
                    prFbmSeqHdr->fgB2R3DEnable = TRUE;
                    prFbmSeqHdr->fgB2RForce2D = FALSE;
                    prFbmSeqHdr->u4B2R3DWidthOffset = u4Pitch >> 1;
                    prFbmSeqHdr->u4B2R3DHeightOffset = 0;
                    prFbmSeqHdr->u1B2R3DType = B2R_3D_SIDE_BY_SIDE;
                    break;
                    default:
                    break;
                }
            }

            _ePrevDetectedMode = eDetectedMode;
            FBM_SetFrameBufferPicFlag(ucFbgId, ucFbId, FBM_MM_3D_CHG_FLAG);
        }

        //Display
        FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_DISPLAYQ);

        // Unmute B2R
        VDP_SetMode(0, VDP_MODE_NORMAL);

    }

    x_sema_unlock(hMt53fbMutex);

    return TRUE;

}

static int _mt53fb_vdp_init_ioctl(void* pvArg)
{
    struct mt53fb_vdp_init rInitData;

    if(copy_from_user( (void*)&rInitData, pvArg, sizeof(struct mt53fb_vdp_init)))
    {
        printk(KERN_WARNING "_mt53fb_vdp_init_ioctl() - copy_from_user() fail\n");
        return -EFAULT;
    }

    if(!_VDP_FBGInit(OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayHeight(), 0))
    {
        printk(KERN_WARNING "_mt53fb_vdp_init_ioctl() - fbg init fail\n");
        return -EFAULT;
    }

    return 0;
}

static int _mt53fb_vdp_flip_ioctl(void* pvArg)
{
    struct mt53fb_vdp_flip rFlipData;

    if(copy_from_user( (void*)&rFlipData, pvArg, sizeof(struct mt53fb_vdp_flip)))
    {
        printk(KERN_WARNING "_mt53fb_vdp_flip_ioctl() - copy_from_user() fail\n");
        return -EFAULT;
    }

    if(!_VDP_FBGInit(OSD_BASE_GetDisplayWidth(), OSD_BASE_GetDisplayHeight(), rFlipData.ui4_pitch))
    {
        printk(KERN_WARNING "_mt53fb_vdp_flip_ioctl() - FBG init fail\n");
        return -EFAULT;
    }
    if(!_VDP_FBFlip(_ucFbgId, rFlipData.pu1SrcYAddr, rFlipData.pu1SrcCAddr, rFlipData.fgIs3DMode, rFlipData.ui4_pitch,
                    rFlipData.u4CropBottomOffset, rFlipData.u4CropTopOffset, rFlipData.u4CropLeftOffset, rFlipData.u4CropRightOffset))
    {
        printk(KERN_WARNING "_mt53fb_vdp_flip_ioctl() - FB flip fail\n");
        return -EFAULT;
    }

    return 0;
}

static int _mt53fb_vdp_set_ioctl(void* pvArg)
{
    struct mt53fb_vdp_set rSetData;

    if(copy_from_user( (void*)&rSetData, pvArg, sizeof(struct mt53fb_vdp_set)))
    {
        printk(KERN_WARNING "_mt53fb_vdp_set_ioctl() - copy_from_user() fail\n");
        return -EFAULT;
    }

    x_sema_lock(hMt53fbMutex, X_SEMA_OPTION_WAIT);

    //layer release
    if(!rSetData.ucEnable)
    {
        VDP_SetMode(0, VDP_MODE_BG);
        FBM_ReleaseGroup(_ucFbgId);
        FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, 0);
        bApiVideoSetVideoSrc(0,  SV_VS_MAX);

        fpDrvTDTVReg3DCropCB(E_TDTV_CB_3D_CROP_MM, NULL);
        _fgVdpInit = FALSE;
        // printk(KERN_WARNING "VDP release fbg!\n");
    }

    x_sema_unlock(hMt53fbMutex);

    return 0;
}
#endif


#ifdef CC_B2R44K2K_SUPPORT

//#define DFB_4K_Print(a...) Printf("[4k2k]"a)
#if 1
#define DFB_4K_DBG(s, args...) Printf("[4k2k]"s, ## args)
#else
#define DFB_4K_Printf(s, args...)
#endif

#define B2R_DEBUG           0

#ifdef CC_SUPPORT_FULL_3D_PHOTO
#define B2R_WIDTH_4K2K           3840
#define B2R_HEIGHT_4K2K          1080
#else
#define B2R_WIDTH_4K2K           3840
#define B2R_HEIGHT_4K2K          2160
#endif
#define B2R_WIDTH_FHD       1920
#define B2R_HEIGHT_FHD      1080

#define B2R_BPP             2

static UINT32 _b2r_width  = B2R_WIDTH_FHD;
static UINT32 _b2r_height = B2R_HEIGHT_FHD;

//#undef CC_4K2K_PHOTO_TO_VDO_BUFFER

static UINT32 _b2r_use_width  = B2R_WIDTH_FHD;
static UINT32 _b2r_use_height = B2R_HEIGHT_FHD;


#define DISPLAY_AS_AUTO_3D  0x1
#define SUPPORT_3D_MODE     0x2


/* RGB <-> YCbCr conversion */
#define CLAMP(x,min,max) ((x) < (min) ? (min) : (x) > (max) ? (max) : (x))
#define YCBCR_TO_RGB( y, cb, cr, r, g, b )                            \
do {                                                                  \
     int _y  = (y)  -  16;                                            \
     int _cb = (cb) - 128;                                            \
     int _cr = (cr) - 128;                                            \
                                                                      \
     int _r = (298 * _y             + 409 * _cr + 128) >> 8;          \
     int _g = (298 * _y - 100 * _cb - 208 * _cr + 128) >> 8;          \
     int _b = (298 * _y + 516 * _cb             + 128) >> 8;          \
                                                                      \
     (r) = CLAMP( _r, 0, 255 );                                       \
     (g) = CLAMP( _g, 0, 255 );                                       \
     (b) = CLAMP( _b, 0, 255 );                                       \
} while (0)

#define PIXEL_RGB32(r,g,b)     ( (0xff << 24) | \
                                 ((r)  << 16) | \
                                 ((g)  <<  8) | \
                                  (b) )

typedef struct fbg_info
{
     unsigned char      fbg_id;
     unsigned char      cur_fbid;
     unsigned int       state;
     B2R44K2K_Buffer    buffer[B2R_BUFFER_NUM];  
}fbg_info;

static struct fbg_info           _fbg_info = {
.state = 0,
.cur_fbid = FBM_FBG_ID_UNKNOWN
}; 

static struct mt53fb_imagebuffer _rMpgBufInfo[B2R_BUFFER_NUM];

static int _mt53_fb_b2r_init(void)
{
    int i = 0;
    unsigned char fbg_id, fbg_type;
     FBM_CREATE_FBG_PAR_T rPar;
     
    x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
    
    rPar.u1VdecId = (UINT8)0;
    rPar.u1AppMode = FBM_FBG_APP_MTIMAGE;

    fbg_type = FBM_SelectGroupType(B2R_WIDTH_4K2K, B2R_HEIGHT_4K2K);
    FBM_SetColorMode(FBM_CM_422);
    
        fbg_id = FBM_CreateGroupExt(fbg_type, FBM_VDEC_JPEG, B2R_WIDTH_4K2K, B2R_HEIGHT_4K2K,&rPar);

    if(fbg_id == FBM_FBG_ID_UNKNOWN)
    {
         Printf("[%s] FBM_Create_Group %d x %d Fail \n", __FUNCTION__, B2R_WIDTH_4K2K, B2R_HEIGHT_4K2K);
         return -1;
    }
    
    for(i = 0; i < B2R_BUFFER_NUM; i++)
    {           
         FBM_GetFrameBufferAddr(fbg_id, i, &_fbg_info.buffer[i].m_YAddr, &_fbg_info.buffer[i].m_CAddr);

         _rMpgBufInfo[i].u4Size = (_fbg_info.buffer[i].m_CAddr- _fbg_info.buffer[i].m_YAddr) + B2R_WIDTH_4K2K * B2R_HEIGHT_4K2K;        
         _rMpgBufInfo[i].u4Size = (_rMpgBufInfo[i].u4Size + 0xFFF) & (~0xFFF);
         _rMpgBufInfo[i].u4PhyAddr = _fbg_info.buffer[i].m_YAddr;
        
         _fbg_info.buffer[i].m_State = 0;
         _fbg_info.buffer[i].m_Fd = i;
         _fbg_info.buffer[i].m_Size = _rMpgBufInfo[i].u4Size;


   // _fbg_info.buffer[i].m_VAddr= VIRTUAL(_rMpgBufInfo[i].u4PhyAddr);
    
         FBM_SetFrameBufferStatus(fbg_id, i, (UCHAR)FBM_FB_STATUS_EMPTY);

#if B2R_DEBUG
         Printf("[%s] %d : addr[0x%x] [0x%x] size[%d] \n", __FUNCTION__, i, _rMpgBufInfo[i].u4PhyAddr, _fbg_info.buffer[i].c_addr, _rMpgBufInfo[i].u4Size);
#endif
    }

    FBM_ReleaseGroup(fbg_id);
    
    return 0;
}

static int _mt53fb_get_b2rbuffer_ioctl(void* pvArg)
{
    if(copy_to_user(pvArg, (void*)_fbg_info.buffer, B2R_BUFFER_NUM * sizeof(struct B2R44K2K_Buffer)))
    {
        return -EFAULT;
    }
    return 0;
}


static int _mt53fb_set_3d_param(UINT32 u43DMode, UINT32 u4BottomOffset, UINT32 u4TopOffset, UINT32 u4LeftOffset, UINT32 u4RightOffset)
{
    _u4Is3DMode  = u43DMode;
    _u4CropBottomOffset = u4BottomOffset;
    _u4CropTopOffset = u4TopOffset;
    _u4CropLeftOffset = u4LeftOffset;
    _u4CropRightOffset = u4RightOffset;

    return 0;
}

//Check whether it's FHD photo in always 4k2k image mode
int fgIsPhoto3DModeSupport(void)
{
    return (_u4Is3DMode & SUPPORT_3D_MODE);
}

#if B2R_DEBUG
#include <linux/file_ops.h>
UINT32  line_buf[B2R_WIDTH_4K2K];
static int _mt53fb_dump_buffer(unsigned int y_addr, int c_offset)
{
     struct file *fd;
     unsigned char path[60];
     static int  _num = 0;
     int file_size, pitch, i, x;
     UINT64  offset = 0;
     int r, g, b;
     const unsigned char  *src8;
     const unsigned short *src16;

     unsigned char header[54]   = {
          0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0,
          54, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 32, 0,
          0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0
     };

     pitch = ((_b2r_use_width + 15) >> 4) << 4;

     file_size = _b2r_use_width * _b2r_use_height * 4 + 54;
     header[2]  = (UINT8)(file_size & 0x000000ff);
     header[3]  = (UINT8)((file_size >> 8) & 0x000000ff);
     header[4]  = (UINT8)((file_size >> 16)& 0x000000ff);
     header[5]  = (UINT8)((file_size >> 24)& 0x000000ff);

     header[18] = (UINT8)(_b2r_use_width & 0x000000ff);
     header[19] = (UINT8)((_b2r_use_width >> 8) & 0x000000ff);
     header[20] = (UINT8)((_b2r_use_width >> 16)& 0x000000ff);
     header[21] = (UINT8)((_b2r_use_width >> 24)& 0x000000ff);  

     header[22] = (UINT8)(_b2r_use_height & 0x000000ff);
     header[23] = (UINT8)((_b2r_use_height >> 8) & 0x000000ff);
     header[24] = (UINT8)((_b2r_use_height >> 16)& 0x000000ff);
     header[25] = (UINT8)((_b2r_use_height >> 24)& 0x000000ff);
    
     sprintf(path, "/mnt/usb/sda1/_b2r_buffer_%d.bmp", _num);
    
     fd = FILE_Open(path, O_EXCL | O_CREAT | O_WRONLY, 0644 );  
     if (fd <= 0)
     {
         return -1;
     }

     FILE_Write( fd, header, 54, &offset);

    for (i = (_b2r_use_height - 1); i >= 0; i--)
     {
          /* Prepare one row. */
          src8  = (unsigned char *)( y_addr + pitch * i);
          src16 = (unsigned short *) (y_addr + c_offset + 2 * pitch * i);

          for (x = 0; x < _b2r_use_width; x++) 
          {        
#ifdef WORDS_BIGENDIAN
              YCBCR_TO_RGB( src8[x], src16[x>>1] >> 8, src16[x>>1] & 0xff, r, g, b );
#else
              YCBCR_TO_RGB( src8[x], src16[x>>1] & 0xff, src16[x>>1] >> 8, r, g, b );
#endif         
              line_buf[x] = PIXEL_RGB32( r, g, b );
          }
 
          FILE_Write( fd, line_buf, _b2r_use_width * 4,  &offset); 
     }
     
     FILE_Close(fd);

     _num++;

     return 0;
}

#endif

#ifdef CC_4K2K_PHOTO_TO_VDO_BUFFER
#include "drv_mjc.h"
#include "drv_mpeg.h"
#endif

static int _mt53_fb_b2r_install(void)
{
#ifdef CC_4K2K_PHOTO_TO_VDO_BUFFER

    unsigned char fbg_type, fbg_id;
    unsigned int pitch;
    FBM_SEQ_HDR_T* fbm_seq_hdr;
    FBM_CREATE_FBG_PAR_T rPar;    
    VDP_REGION_T rRegion;
    BOOL fgIs4k2k = 0, fg4k2kChanged = 0 ;
    #ifdef CC_B2R_RM_SUPPORT
    B2R_RM_T    t_b2r_src = {0};
    #endif
    
    x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
    rPar.u1VdecId = (UINT8)0;
    rPar.u1AppMode = FBM_FBG_APP_MTIMAGE;

    if(!(_fbg_info.state & 0x1))
    {
        // Set video src
        bApiVideoSetVideoSrc(0, SV_VS_DT1);        
        VDP_SetSrcRegion(0, 1, rRegion);
        VDP_SetOutRegion(0, 1, rRegion); 
        #ifdef CC_B2R_RM_SUPPORT
        t_b2r_src.u2HActive = B2R_WIDTH_4K2K;
        t_b2r_src.u2VActive = B2R_HEIGHT_4K2K;
        t_b2r_src.u2BlockMode = 0;
        B2R_Source_Connect(t_b2r_src);
        #endif

        VDP_SetEnable(0, 1);
        VDP_SetMode(0, VDP_MODE_BG);
        VDP_SetInput(0, 0, 0);

        fbg_type = FBM_SelectGroupType(B2R_WIDTH_4K2K, B2R_HEIGHT_4K2K);
        
        FBM_SetColorMode(FBM_CM_422);
        fbg_id = FBM_CreateGroupExt(fbg_type, FBM_VDEC_JPEG, B2R_WIDTH_4K2K, B2R_HEIGHT_4K2K,&rPar);
        if(fbg_id == FBM_FBG_ID_UNKNOWN)
        {
            Printf("[%s] FBM_Create_Group %d x %d Fail \n", __FUNCTION__, B2R_WIDTH_4K2K, B2R_HEIGHT_4K2K);
            return -1;
        }
        Printf("[4k2k]FBM_SelectGroupType :%d, id %d\n ",fbg_type, fbg_id);
        
        FBM_SetPlayMode(fbg_id, FBM_FBG_MM_MODE);
        FBM_SetSyncStc(fbg_id, 0, 0); //none
        FBM_FbgChgNotify(fbg_id, 0);

        pitch = ((B2R_WIDTH_4K2K + 15) >> 4) << 4;

        fbm_seq_hdr = FBM_GetFrameBufferSeqHdr(fbg_id);
        if(fbm_seq_hdr == NULL)
        {
            return 0;
        }

        fbm_seq_hdr->u2HSize = B2R_WIDTH_FHD;
        fbm_seq_hdr->u2VSize = B2R_HEIGHT_FHD;
        fbm_seq_hdr->u2OrgHSize = fbm_seq_hdr->u2HSize;
        fbm_seq_hdr->u2OrgVSize = fbm_seq_hdr->u2VSize;
        fbm_seq_hdr->u2LineSize = pitch;
        fbm_seq_hdr->fgProgressiveSeq = TRUE;
        fbm_seq_hdr->fgRasterOrder = TRUE;
        fbm_seq_hdr->fg422Mode = TRUE;

        fbm_seq_hdr->ucFrmRatCod = MPEG_FRAME_RATE_30; 
        
        FBM_SetFrameBufferFlag(fbg_id, FBM_FLAG_SEQ_CHG);

        // Register 3D crop CB
        fpDrvTDTVReg3DCropCB(E_TDTV_CB_3D_CROP_MM, _3DCropFuncCB);

        _fbg_info.fbg_id = fbg_id;
        _fbg_info.state  |= 1;
        fg4k2kChanged = 1 ;
    }

    else if(_fbg_info.state & (1 << 1))
    {
        if(_b2r_width != _b2r_use_width)
        {
            fg4k2kChanged = 1 ;         
        }       
    
        _fbg_info.state &= ~(0x1 << 1);
    }

    if(fg4k2kChanged)
    {
        fgIs4k2k = (_b2r_width == B2R_WIDTH_4K2K) ?TRUE :FALSE ;    
        
        vDrv4K2KMJCDumpInInit(fgIs4k2k);
        vDrvMJCSetFrmFreeze(fgIs4k2k); 
        
        vMpegModeChg(0);
        vMpegModeDetDone(0);  
        _b2r_use_width = _b2r_width;
        _b2r_use_height = _b2r_height;      

    }

#else

    unsigned char fbg_type, fbg_id ;
    BOOL fgSeqHdrSet=FALSE;
    unsigned int pitch;
    FBM_SEQ_HDR_T* fbm_seq_hdr;    
    VDP_REGION_T rRegion;
    FBM_CREATE_FBG_PAR_T rPar;
    #ifdef CC_B2R_RM_SUPPORT
    B2R_RM_T    t_b2r_src = {0};
    #endif
    
    if(!(_fbg_info.state & 0x1))
    {
        x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
        rPar.u1VdecId = (UINT8)0;
        rPar.u1AppMode = FBM_FBG_APP_MTIMAGE;    
        
        // Set video src
        bApiVideoSetVideoSrc(0, SV_VS_DT1);
        VDP_SetInput(0, 0, 0);
         
        #ifdef CC_B2R_RM_SUPPORT
        x_memset(&t_b2r_src, 0, sizeof(t_b2r_src));
        t_b2r_src.u2HActive = B2R_WIDTH_4K2K;
        t_b2r_src.u2VActive = B2R_HEIGHT_4K2K;
        t_b2r_src.u2BlockMode = 0;
        t_b2r_src.ucEsId = 0;
        t_b2r_src.fg4K2k = TRUE;
        t_b2r_src.fg8Bit = TRUE;
        B2R_Source_Connect(t_b2r_src);
        #else
        VDP_B2rSwitch(0, B2R_1);
        #endif

        VDP_SetEnable(0, 1);
        VDP_SetMode(0, VDP_MODE_BG);
        VDP_SetSrcRegion(0, 1, rRegion);
        VDP_SetOutRegion(0, 1, rRegion);
        
        SRM_SetTvMode(SRM_TV_MODE_TYPE_NORMAL);
        
        fbg_type = FBM_SelectGroupType(B2R_WIDTH_4K2K, B2R_HEIGHT_4K2K);
        FBM_SetColorMode(FBM_CM_422);
        fbg_id = FBM_CreateGroupExt(fbg_type, FBM_VDEC_JPEG, B2R_WIDTH_4K2K, B2R_HEIGHT_4K2K,&rPar);
        if(fbg_id == FBM_FBG_ID_UNKNOWN)
        {
            Printf("[%s] FBM_Create_Group %d x %d Fail \n", __FUNCTION__, B2R_WIDTH_4K2K, B2R_HEIGHT_4K2K);
            return -1;
        }
        Printf("[4k2k]FBM_SelectGroupType :%d, id %d\n ",fbg_type, fbg_id);
        FBM_SetPlayMode(fbg_id, FBM_FBG_MM_MODE);
        FBM_SetSyncStc(fbg_id, 0, 0); //none
        FBM_FbgChgNotify(fbg_id, 0);

        fgSeqHdrSet = TRUE ;

        // Register 3D crop CB
        fpDrvTDTVReg3DCropCB(E_TDTV_CB_3D_CROP_MM, _3DCropFuncCB);

        _fbg_info.fbg_id = fbg_id;
        _fbg_info.state  |= 1;
    }
    else if(_fbg_info.state & (1 << 1))
    {
        fgSeqHdrSet = TRUE ;
        _fbg_info.state &= ~(0x1 << 1);
    }

    if(fgSeqHdrSet)
    {
        fbm_seq_hdr = FBM_GetFrameBufferSeqHdr(_fbg_info.fbg_id);   
        if(fbm_seq_hdr == NULL)
        {
            return 0;
        }
        fbm_seq_hdr->fgProgressiveSeq = TRUE;
        fbm_seq_hdr->fgRasterOrder = TRUE;
        fbm_seq_hdr->fg422Mode = TRUE;
        pitch = ((B2R_WIDTH_4K2K+ 15) >> 4) << 4;
        fbm_seq_hdr->u2LineSize = pitch;
        fbm_seq_hdr->u2HSize = _b2r_width;
        fbm_seq_hdr->u2VSize = _b2r_height;
        fbm_seq_hdr->u2OrgHSize = fbm_seq_hdr->u2HSize;
        fbm_seq_hdr->u2OrgVSize = fbm_seq_hdr->u2VSize;

        if(_b2r_width == B2R_WIDTH_4K2K)
        {
            fbm_seq_hdr->ucFrmRatCod = MPEG_FRAME_RATE_15;   
        }
        else
        {
            fbm_seq_hdr->ucFrmRatCod = MPEG_FRAME_RATE_30; 
        }

        _b2r_use_width = _b2r_width;
        _b2r_use_height = _b2r_height;      

        FBM_SetFrameBufferFlag(_fbg_info.fbg_id, FBM_FLAG_SEQ_CHG);  
    }

#endif    
    return 0;
}


static INT32 _mt53_fb_b2r_uninstall(void)
{
    _u4Is3DMode = 0;
    
    if(_fbg_info.state & 0x1)
    {
        VDP_SetMode(0, VDP_MODE_BG);
        FBM_ReleaseGroup(_fbg_info.fbg_id);
        FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, 0);
    
        fpDrvTDTVReg3DCropCB(E_TDTV_CB_3D_CROP_MM, NULL);
#ifdef CC_4K2K_PHOTO_TO_VDO_BUFFER
        vDrv4K2KMJCDumpInInit(FALSE);
        //vDrvMJCSetFrmFreeze(FALSE);
#endif      
        VDP_SetEnable(0, 0);
        VDP_B2rSwitch(0, B2R_NS);
        bApiVideoSetVideoSrc(0, SV_VS_MAX);
        _fbg_info.cur_fbid = FBM_FBG_ID_UNKNOWN ;
        _fbg_info.state &= ~0x1;
        _b2r_width = _b2r_use_width = B2R_WIDTH_FHD;
        _b2r_height = _b2r_use_height= B2R_HEIGHT_FHD;      
        DFB_4K_DBG("[4k2k]VDP release fbg!\n");
    }
    return 0;

}

static INT32 _mt53_fb_b2r_switch(UINT32 u4Mode)
{
    switch(u4Mode)
    {
    case 0:
        _b2r_width = B2R_WIDTH_FHD;
        _b2r_height = B2R_HEIGHT_FHD;
        break;
    case 1:
        _b2r_width = B2R_WIDTH_4K2K;
        _b2r_height = B2R_HEIGHT_4K2K;      
        break;
    default:
        break;
    }

    _fbg_info.state |= (0x1 << 1);
    
    return 0;
}

//#define CC_FLIP_COPY_MEASURE
#define B2R_FLIP_WAIT_DELAY 50
#define B2R_FLIP_MAX_RTY_CNT 10
static INT32 _mt53_fb_b2r_flip(UINT32 u4Addr)
{

    INT32 i;
    UINT32 u43DInOut, u2InFmt, u2OutFmt, u4Pitch, u4TotalLine;
    B2R44K2K_Buffer *ptBuffer = NULL;
    unsigned char ucFbgId, ucFbId, ucFbTmp;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;
    E_TDTV_DECODER_INPUT eDetectedMode;
    FBM_PIC_HDR_T* prFbmPicHdr; 
    UCHAR u1EmptyQCnt =0;    

    DFB_4K_DBG("%s 0x[%p] \n", __FUNCTION__, u4Addr);
    
    _mt53_fb_b2r_install();

    //HalFlushInvalidateDCache();

    for(i = 0; i < B2R_BUFFER_NUM; i++)
    {
         if(u4Addr == _fbg_info.buffer[i].m_YAddr)
         {
              ptBuffer = &_fbg_info.buffer[i];
              break;
         }
    }
    
    if(ptBuffer == NULL)
    {
         Printf("%s can't get fbg info \n", __FUNCTION__);
         return 0;
    }
    
    ucFbgId = _fbg_info.fbg_id;
    ucFbId  = ptBuffer->m_Fd;
    

    
#ifdef CC_4K2K_PHOTO_TO_VDO_BUFFER
    
    if(_b2r_width == B2R_WIDTH_4K2K)
    {
        static UINT8 u1FrameIdx = 0 ; 
#ifdef CC_FLIP_COPY_MEASURE   
        HAL_TIME_T hStartMeasureTime;
        HAL_TIME_T hEndMeasureTime;
        HAL_TIME_T  rDeltaTime;
#endif  

#ifdef CC_FLIP_COPY_MEASURE 
        HAL_GetTime(&hStartMeasureTime);
#endif      
        //vDrvMJCDumpInRepeat(u1FrameIdx,     (UINT8 *)ptBuffer->m_VAddr, (UINT8 *)(ptBuffer->m_VAddr+ ptBuffer->m_CAddr- ptBuffer->m_YAddr));

        //vDrvMJCDumpInRepeat(1,    (UINT8 *)ptBuffer->v_addr, (UINT8 *)(ptBuffer->v_addr + ptBuffer->c_addr - ptBuffer->y_addr));
        //vDrvMJCDumpInRepeat(2,    (UINT8 *)ptBuffer->v_addr, (UINT8 *)(ptBuffer->v_addr + ptBuffer->c_addr - ptBuffer->y_addr));
        vDrvMJCSetFrmFreezeNum(TRUE, u1FrameIdx);
        DFB_4K_DBG("[4K2K] freeze on id %d, cur fb id %d\n", u1FrameIdx, ucFbId); 

        u1FrameIdx ++ ;
        u1FrameIdx = u1FrameIdx %3;
        
#ifdef CC_FLIP_COPY_MEASURE 
        HAL_GetTime(&hEndMeasureTime);
        HAL_GetDeltaTime(&rDeltaTime, &hStartMeasureTime, &hEndMeasureTime);
        DFB_4K_DBG("[4K2K] b2r flip diff %d.%03d s\n", rDeltaTime.u4Seconds,  (rDeltaTime.u4Micros / 1000));    
#endif      
    }
#endif //CC_4K2K_PHOTO_TO_VDO_BUFFER

    //HalFlushInvalidateDCache();
    
//#if B2R_DEBUG
    //_mt53fb_dump_buffer(ptBuffer->m_VAddr, ptBuffer->m_CAddr - ptBuffer->m_YAddr);
//#endif

    FBM_GetEmptyQCount(ucFbgId,&u1EmptyQCnt);
   for (i = 0;i<u1EmptyQCnt;i++)
    {
    ucFbTmp =  FBM_GetEmptyFrameBuffer(ucFbgId,(UINT32)1500);
    //DFB_4K_DBG("ucFbTmp = %d,ucFbId = %d\n",ucFbTmp,ucFbId);

    // try to find the match fb id!
     if (ucFbTmp == ucFbId) 
        {
          break;
        }
     else
        {
         //Printf("Put FB %d back into emptyQ!\n",ucFbTmp);
         FBM_SetFrameBufferStatus(ucFbgId,ucFbTmp,(UCHAR)FBM_FB_STATUS_EMPTY);
        }
    }

    if (i == u1EmptyQCnt )  // not find!
    {
        Printf("No empty matched FB found!%d\n",ucFbId);
        ASSERT(0);
    }
#if 0
    if (_fbg_info.cur_fbid != FBM_FBG_ID_UNKNOWN)
    {
        //if (FBM_GetFrameBufferStatus(ucFbgId, _atFbgInfo.ucFbId)!= (UCHAR)FBM_FB_STATUS_EMPTY)
        //FBM_SetFrameBufferStatus(ucFbgId, _atFbgInfo.ucFbId, (UCHAR)FBM_FB_STATUS_EMPTY);
    }
#endif


    FBM_GetFrameBufferAddr(ucFbgId,  ucFbId, &ptBuffer->m_YAddr, &ptBuffer->m_CAddr);
    HalFlushInvalidateDCacheMultipleLine(ptBuffer->m_YAddr, B2R_WIDTH_4K2K * B2R_HEIGHT_4K2K);
    HalFlushInvalidateDCacheMultipleLine(ptBuffer->m_CAddr, B2R_WIDTH_4K2K * B2R_HEIGHT_4K2K);

    if(u4Addr != ptBuffer->m_YAddr)
    {
         Printf("[%s] [%x -> %x] buffer invalid \n", __FUNCTION__, ptBuffer->m_YAddr, u4Addr);
         return 0;
    }
    
    prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);
    
    if (prFbmPicHdr == NULL)
    {
         Printf("Alloc frame buffer failed.\n");
         return 0;
    }
            
    prFbmPicHdr->ucPicStruct = (UCHAR)MPEG_PIC_FRAME;
    prFbmPicHdr->fgProgressiveFrm = TRUE;
    prFbmPicHdr->fgRepFirstFld = FALSE;
    prFbmPicHdr->fgTopFldFirst = FALSE;
    
    FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE); //default  
    FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_READY);  
    FBM_GetFrameBufferSize(ucFbgId, &u4Pitch, &u4TotalLine);
    
   

    if(_u4Is3DMode & DISPLAY_AS_AUTO_3D)
    {
        #ifdef CC_SUPPORT_FULL_3D_PHOTO
        eDetectedMode = E_TDTV_DECODER_INPUT_FS_I;
        #else
        eDetectedMode = E_TDTV_DECODER_INPUT_SBS_P;
        #endif
    }
    else
    {
        eDetectedMode = E_TDTV_DECODER_INPUT_2D_P;
    }       
    
    u43DInOut = u4DrvTDTVMMModeQuery(eDetectedMode);
    
    #ifndef CC_SUPPORT_FULL_3D_PHOTO
    if(eDetectedMode != _ePrevDetectedMode)
    #endif
    {
        //u43DInOut = u4DrvTDTVMMModeQuery(eDetectedMode);
        u2InFmt = u43DInOut & 0xFFFF;
        u2OutFmt = (u43DInOut >> 16) & 0xFFFF;
        prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
        
        if(prFbmSeqHdr)
        {
            switch(u2OutFmt)
            {
                 case E_TDTV_DECODER_OUTPUT_NATIVE:
                        #ifdef CC_SUPPORT_FULL_3D_PHOTO
                        prFbmSeqHdr->fg4K1K = FALSE;
                        #else
                        prFbmSeqHdr->fgB2R3DEnable = FALSE;
                        #endif
                        break;
                 case E_TDTV_DECODER_OUTPUT_3D_FS:
                        #ifdef CC_SUPPORT_FULL_3D_PHOTO
                        prFbmSeqHdr->fg4K1K = TRUE;
                        #else
                        prFbmSeqHdr->fgB2R3DEnable = TRUE;
                        prFbmSeqHdr->fgB2RForce2D = FALSE;
                        prFbmSeqHdr->u4B2R3DWidthOffset = u4Pitch >> 1;
                        prFbmSeqHdr->u4B2R3DHeightOffset = 0;
                        prFbmSeqHdr->u1B2R3DType = B2R_3D_FRAME_SEQUENTIAL;
                        #endif
                        break;
                 default:
                        break;
            }
        }
        
        _ePrevDetectedMode = eDetectedMode;
        FBM_SetFrameBufferPicFlag(ucFbgId, ucFbId, FBM_MM_3D_CHG_FLAG);
    }
        
    //Display
    FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_DISPLAYQ);
    if (_fbg_info.cur_fbid != FBM_FBG_ID_UNKNOWN)
    {
        UCHAR ucFbStatus ;

        // set formal fb to emptyQ!
#if 0
        ucFbStatus = FBM_GetFrameBufferStatus( ucFbgId,_fbg_info.cur_fbid) ;
        Printf("[4k2k]free cur fgb/fbid %d/%d status:%d\n", ucFbgId, _fbg_info.cur_fbid, ucFbStatus);
        if (ucFbStatus != FBM_FB_STATUS_LOCK)
        {
            if (ucFbStatus != FBM_FB_STATUS_DISPLAYQ)
                Printf("FATAL Error! in _mt53_fb_b2r_flip,the fb status =%d\n",ucFbStatus);

            FBM_SetFrameBufferStatus(ucFbgId, _fbg_info.cur_fbid, (UCHAR)FBM_FB_STATUS_LOCK);
        } 
#else
        for(i=0 ; i < B2R_FLIP_MAX_RTY_CNT ; i++)
        {
            ucFbStatus = FBM_GetFrameBufferStatus( ucFbgId,_fbg_info.cur_fbid) ;
            if (ucFbStatus == FBM_FB_STATUS_EMPTY)
            {
                break;
            }           
            x_thread_delay(B2R_FLIP_WAIT_DELAY);
        }
        DFB_4K_DBG("[4k2k] flip delay time %d ms\n", (i-1)*B2R_FLIP_WAIT_DELAY);
#endif
    }   

    _fbg_info.cur_fbid = ucFbId;
        
    // Unmute B2R
    VDP_SetMode(0, VDP_MODE_NORMAL);        
    return 0;

}

#endif


#ifndef LINUX_TURNKEY_SOLUTION
static int _mt53fb_soft_cursor(struct fb_info *info, struct fb_cursor *cursor)
{
    //do nothing to disable linux frame buffer cursor appearing
    return 0;
}
#endif

static int _mt53fb_setcolreg(u_int regno, u_int red, u_int green, u_int blue,
        u_int trans, struct fb_info *info)
{
    union palette pal;

    if (regno < 16 && info->fix.visual == FB_VISUAL_DIRECTCOLOR)
    {
        u32 pseudo_val;

        pseudo_val = regno << info->var.red.offset;
        pseudo_val |= regno << info->var.green.offset;
        pseudo_val |= regno << info->var.blue.offset;

        ((u32 *) info->pseudo_palette)[regno] = pseudo_val;
    }

    pal.red = red >> 8;
    pal.green = green >> 8;
    pal.blue = blue >> 8;

    _cur_par.pal[regno] = pal;

    return 0;
}

static int _mt53fb_check_var(struct fb_var_screeninfo *var,
        struct fb_info *info)
{
    var->red.msb_right = 0;
    var->green.msb_right = 0;
    var->blue.msb_right = 0;
    var->transp.msb_right = 0;

    switch (var->bits_per_pixel)
    {
    case 1:
    case 2:
    case 4:
    case 8:
        var->red.offset = 0;
        var->red.length = var->bits_per_pixel;
        var->green = var->red;
        var->blue = var->red;
        var->transp.offset = 0;
        var->transp.length = 0;
        break;
    case 16:
        var->red.offset = 0;
        var->red.length = 4;
        var->green.offset = 4;
        var->green.length = 4;
        var->blue.offset = 8;
        var->blue.length = 4;
        var->transp.offset = 12;
        var->transp.length = 4;
        break;
    case 32:
        /*
         var->red.offset    = 0;
         var->red.length    = 8;
         var->green.offset  = 8;
         var->green.length  = 8;
         var->blue.offset   = 16;
         var->blue.length   = 8;
         var->transp.offset = 24;
         var->transp.length = 8;
         */
        var->red.offset = 16;
        var->red.length = 8;
        var->green.offset = 8;
        var->green.length = 8;
        var->blue.offset = 0;
        var->blue.length = 8;
        var->transp.offset = 24;
        var->transp.length = 8;
        break;
    default:
        //printk(KERN_WARNING "_mt53fb_check_var() - invalid var->bits_per_pixel = %d\n",
        //var->bits_per_pixel);
        return -EINVAL;
    }
    info->var.red = var->red;
    info->var.green = var->green;
    info->var.blue = var->blue;
    info->var.transp = var->transp;
    return 0;
}

static int _mt53fb_set_par(struct fb_info *info)
{
    switch (info->var.bits_per_pixel)
    {
    case 1:
        break;
    case 2:
        break;
    case 4:
        break;
    case 8:
        break;
    case 16:
        break;
    case 32:
        break;
    default:
        BUG();
        break;
    }

    info->fix.line_length = (info->var.xres * info->var.bits_per_pixel) / 8;
    return 0;
}

static int _mt53fb_open(struct fb_info *info, int user)
{
    static BOOL fgInit = FALSE;
    if (!fgInit)
    {
        x_sema_create(&hMt53fbMutex, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK);
#ifndef LINUX_TURNKEY_SOLUTION
        x_sema_lock(hMt53fbMutex, X_SEMA_OPTION_WAIT);
        GFX_Init();
        VERIFY(OSD_Reset() == 0);
        x_sema_unlock(hMt53fbMutex);
#endif
        fgInit = TRUE;

#ifndef LINUX_TURNKEY_SOLUTION
        printk("Open MT53_FB successfully\n");
#endif
    }
    else
    {
        x_sema_lock(hMt53fbMutex, X_SEMA_OPTION_WAIT);
        x_sema_unlock(hMt53fbMutex);
        printk("Re-open MT53_FB successfully\n");
    }

    return 0;
}
void mt53fb_setcmdque(void)
{

   GFX_SetExtCmdQue(_prShmCmdQue, _pu8CmdQueBuf, E_GFX_CMDQ_CFG_256KB);
    return ;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
static int _mt53fb_mmap(struct fb_info *info, struct vm_area_struct *vma)
#else
static int _mt53fb_mmap(struct fb_info *info, struct file *file, struct vm_area_struct *vma)
#endif
{
    unsigned long off, start;
    u32 len;
    unsigned long phys;
    unsigned long size = vma->vm_end - vma->vm_start;

    if (_prShmCmdQue == NULL)
    {
        UINT32 i;

        _prShmCmdQue = (GFX_CMDQUE_T *) VIRTUAL(
                BSP_AllocAlignedDmaMemory(PAGE_SIZE, PAGE_SIZE));
        _pu8CmdQueBuf = (UINT64 *) BSP_AllocAlignedDmaMemory(256 * 1024,
                PAGE_SIZE);

        GFX_SetExtCmdQue(_prShmCmdQue, _pu8CmdQueBuf, E_GFX_CMDQ_CFG_256KB);

        SetPageReserved(virt_to_page(_prShmCmdQue));

        for (i = 0; i < 256 * 1024 / PAGE_SIZE; i++)
        {
            SetPageReserved(virt_to_page(_prShmCmdQue->pu8QueTop) + i);
        }
    }

    off = vma->vm_pgoff << PAGE_SHIFT;

    start = info->fix.smem_start;
    len = PAGE_ALIGN(start & ~PAGE_MASK) + info->fix.smem_len;
    start &= PAGE_MASK;

    // printk("_mt53fb_mmap[0x%x] [0x%x] \n",off,(vma->vm_end - vma->vm_start));

    if((off == _rFbmBufInfo.u4PhyAddr)&&(size == _rFbmBufInfo.u4Size))
    {
        vma->vm_flags |= VM_IO;
        vma->vm_page_prot = __pgprot(
                    pgprot_val(vma->vm_page_prot) | L_PTE_MT_BUFFERABLE);
        phys = _rFbmBufInfo.u4PhyAddr;
        return io_remap_pfn_range(vma, vma->vm_start, phys >> PAGE_SHIFT,size, vma->vm_page_prot);
    }
    
    if((off == _rFbmBufMpeg.u4PhyAddr)&&(size == _rFbmBufMpeg.u4Size))                                                
    {                                                                          
        vma->vm_flags |= VM_IO;
        vma->vm_page_prot = __pgprot(
                    pgprot_val(vma->vm_page_prot) | L_PTE_MT_BUFFERABLE);
        phys = _rFbmBufMpeg.u4PhyAddr; 
        return io_remap_pfn_range(vma, vma->vm_start, phys >> PAGE_SHIFT,size, vma->vm_page_prot);                                                               
    }
        
    if((off == _rCrsrInfo.u4PhyAddr)&&(size == _rCrsrInfo.u4Size))
    {
        vma->vm_flags |= VM_IO;
        vma->vm_page_prot = __pgprot(pgprot_val(vma->vm_page_prot) | L_PTE_MT_BUFFERABLE);
        phys = _rCrsrInfo.u4PhyAddr;
        return io_remap_pfn_range(vma, vma->vm_start, phys >> PAGE_SHIFT,size, vma->vm_page_prot);
    }

    if((off == _rImgBufInfo.u4PhyAddr)&&(size == _rImgBufInfo.u4Size))
    {
        vma->vm_flags |= VM_IO;
        vma->vm_page_prot = __pgprot(pgprot_val(vma->vm_page_prot) | L_PTE_MT_BUFFERABLE);
        phys = _rImgBufInfo.u4PhyAddr;
        //printk("_mt53fb_mmap _rImgBufInfo[0x%x] [0x%x][0x%x] \n",off,(vma->vm_end - vma->vm_start),vma->vm_start);
        return io_remap_pfn_range(vma, vma->vm_start, phys >> PAGE_SHIFT,size, vma->vm_page_prot);
    }

    #ifdef CC_FBM_TWO_FBP_SHARED_WITH_DFB
    if((off == _rImgBufInfo2.u4PhyAddr)&&(size == _rImgBufInfo2.u4Size))
    {
        vma->vm_flags |= VM_IO;
        vma->vm_page_prot = __pgprot(pgprot_val(vma->vm_page_prot) | L_PTE_MT_BUFFERABLE);
        phys = _rImgBufInfo2.u4PhyAddr;
        //printk("_mt53fb_mmap _rImgBufInfo2[0x%x] [0x%x][0x%x] \n",off,(vma->vm_end - vma->vm_start),vma->vm_start);
        return io_remap_pfn_range(vma, vma->vm_start, phys >> PAGE_SHIFT,size, vma->vm_page_prot);
    }
    #endif

    #ifdef CC_DFB_SUPPORT_VDP_LAYER
    if((off == _rVdpBufInfo.u4PhyAddr)&&(size == _rVdpBufInfo.u4Size))
    {
        vma->vm_flags |= VM_IO;
        vma->vm_page_prot = __pgprot(pgprot_val(vma->vm_page_prot) | L_PTE_MT_BUFFERABLE);
        phys = _rVdpBufInfo.u4PhyAddr;
        //printk("_mt53fb_mmap _rVdpBufInfo[0x%x] [0x%x][0x%x] \n",off,(vma->vm_end - vma->vm_start),vma->vm_start);
        return io_remap_pfn_range(vma, vma->vm_start, phys >> PAGE_SHIFT,size, vma->vm_page_prot);
    }
    #endif

#ifdef CC_B2R44K2K_SUPPORT
    {
        int n = 0;
        for(n = 0; n < B2R_BUFFER_NUM; n++)
        {
            if((off == _rMpgBufInfo[n].u4PhyAddr)&&(size == _rMpgBufInfo[n].u4Size))
            {   
                vma->vm_flags |= VM_IO;
                vma->vm_page_prot = __pgprot(pgprot_val(vma->vm_page_prot) | L_PTE_MT_BUFFERABLE);
                phys = _rMpgBufInfo[n].u4PhyAddr; 
                return io_remap_pfn_range(vma, vma->vm_start, phys >> PAGE_SHIFT,size, vma->vm_page_prot);
            }
        }
    }
#endif

    if((off == 0x0)&&(size == info->fix.smem_len))
    {
        off += start;
        vma->vm_pgoff = off >> PAGE_SHIFT;

        //printk("_mt53fb_mmap smem_start[0x%x] [0x%x][0x%x] \n",off,(vma->vm_end - vma->vm_start),vma->vm_start);

        /* This is an IO map - tell maydump to skip this VMA */
        vma->vm_flags |= VM_IO;
        vma->vm_page_prot = __pgprot(
                pgprot_val(vma->vm_page_prot) | L_PTE_MT_BUFFERABLE);
        if (io_remap_pfn_range(vma, vma->vm_start, off >> PAGE_SHIFT,
                vma->vm_end - vma->vm_start, vma->vm_page_prot)){
            return -EAGAIN;
        }

        fb_user_virt_start = vma->vm_start;

        return 0;
    }

    if((vma->vm_end - vma->vm_start + off) > len)
    {
        if(off == len)
        {
                if (size != PAGE_SIZE)
                    return -EINVAL;

            vma->vm_flags |= VM_IO;
            vma->vm_page_prot = __pgprot(
                    pgprot_val(vma->vm_page_prot) | L_PTE_MT_BUFFERABLE);
           phys = (unsigned long) PHYSICAL((UINT32) _prShmCmdQue);

            return io_remap_pfn_range(vma, vma->vm_start, phys >> PAGE_SHIFT,
                    size, vma->vm_page_prot);
        }

        if(off == len + PAGE_SIZE)
        {
            if (size > 256 * 1024)
                return -EINVAL;

            vma->vm_flags |= VM_IO;
            vma->vm_page_prot = __pgprot(
                    pgprot_val(vma->vm_page_prot) | L_PTE_MT_BUFFERABLE);
            phys = (unsigned long) _pu8CmdQueBuf;

            return io_remap_pfn_range(vma, vma->vm_start, phys >> PAGE_SHIFT,
                    size, vma->vm_page_prot);
        }
    }

    return -EINVAL;
}

#ifdef CC_TDTV_3D_OSD_ENABLE
static int _mt53fbGetIsSBSOutput(void* pvArg)
{
    int i4Ret = 0;
    u8 rGet, data;
    OSD_BASE_Set3DStatus();
    data = gfgOsd_Sbs;
    if(copy_to_user(pvArg, (void *)&data, sizeof(data)))
    {
        printk(KERN_WARNING "_mt53fbGetIsSBSOutput() -  copy_to_user() fail\n");
        return -EFAULT;

    }
    return i4Ret;
}

static int _mt53fbGetIsTBBOutput(void* pvArg)
{
    int i4Ret = 0;

    u8 rGet, data;
    OSD_BASE_Set3DStatus();
    data = gfgOsd_Tbb; //GfxGetIsSBSOutput();
    if(copy_to_user(pvArg, (void *)&data, sizeof(data)))
    {
        printk(KERN_WARNING "_mt53fbGetIsTBBOutput() -  copy_to_user() fail\n");
        return -EFAULT;

    }
    return i4Ret;
}

static int _mt53fbGet3DOsdShiftt(void* pvArg)
{
    int i4Ret = 0;
    u8 rGet, data;
    OSD_BASE_Set3DStatus();
    data = _u13DOsdShift; //GfxGetIsSBSOutput();
    if(copy_to_user(pvArg, (void *)&data, sizeof(data)))
    {
        printk(KERN_WARNING "_mt53fbGetIsTBBOutput() -  copy_to_user() fail\n");
        return -EFAULT;

    }
    return i4Ret;
}

#endif

static int _mt53fbGetCurrentLanguage(void* pvArg)
{
    int i4Ret = 0;
    u32 data;
    //data = _u4CurrentLanguage;

    data = GetHidKbCountry();
    if(copy_to_user(pvArg, (void *)&data, sizeof(data)))
        {
            printk(KERN_WARNING "_mt53fbGetCurrentLanguage() -  copy_to_user() fail\n");
            return -EFAULT;

    }
   //printf("get _u4CurrentLanguage =%d\n", _u4CurrentLanguage);
   printf("get _u4CurrentLanguage =%d\n", data);
    return i4Ret;
}

static int _mt53fbSetCurrentLanguage(void* pvArg)
{
    int i4Ret = 0;
    u32 data;
    UINT32 _u4CurrentLanguage;

    if (copy_from_user(&data, pvArg, sizeof(data)))
    {
        printk(KERN_WARNING "_mt53fbSetCurrentLanguage() -  copy_from_user() fail\n");
        return -EFAULT;
    }
    _u4CurrentLanguage =  (UINT32)data;
    SetHidKbCountry(_u4CurrentLanguage);
    //printf("set _u4CurrentLanguage =%d\n", _u4CurrentLanguage);
    printf("set _u4CurrentLanguage =%d\n", _u4CurrentLanguage);
    return i4Ret;
}

static int _mt53fbGetLanguageModeChg(void* pvArg)
{
    int i4Ret = 0;
    u32 data;
    data = _u4HidKbLanguageModeChg;
    if(copy_to_user(pvArg, (void *)&data, sizeof(data)))
        {
            printk(KERN_WARNING "_mt53fbGetLanguageModeChg() -  copy_to_user() fail\n");
            return -EFAULT;

    }
   //printf("get _u4HidKbLanguageModeChg =%d\n", _u4HidKbLanguageModeChg);
    return i4Ret;
}

static int _mt53fbSetLanguageModeChg(void* pvArg)
{
    int i4Ret = 0;
    u32 data;
    if (copy_from_user(&data, pvArg, sizeof(data)))
    {
        printk(KERN_WARNING "_mt53fbSetLanguageModeChg() -  copy_from_user() fail\n");
        return -EFAULT;
    }
    _u4HidKbLanguageModeChg =  (UINT32)data;
    //printf("set _u4HidKbLanguageModeChg =%d\n", _u4HidKbLanguageModeChg);
    return i4Ret;
}

#ifdef CC_SUPPORT_HW_CURSOR
static UINT32 _u4HWCursorRegion[2] = {NULL_REGION_IDX, NULL_REGION_IDX};
static UINT32 _u4HWCursorRegionList[2] = {NULL_REGION_IDX, NULL_REGION_IDX};
static UINT32 _u4HWCursorFlipCnt = 0;
static UINT32 _u4HWCursorOpacity = 0;

INT32 _mt53fb_hwcursor_setting(void *pvArg)
{
    HWCursorSetting setting;
    UINT32 u4Region;
    UINT32 u4RegionList;
    BOOL bEnable;
    
    if (copy_from_user(&setting, pvArg, sizeof(HWCursorSetting)))
    {
        return -EFAULT;
    }

    u4RegionList =  _u4HWCursorRegionList[_u4HWCursorFlipCnt % 2];
    u4Region = _u4HWCursorRegion[_u4HWCursorFlipCnt % 2];   

    switch(setting.u4Op)
    {
    case HWCURSOR_SET_REGION:
        {
            if(0 == setting.u.set_region.width || 0 == setting.u.set_region.height)
            {
                OSD_PLA_Enable(OSD_PLANE_5, FALSE);
                return 0;
            }
            
            _u4HWCursorFlipCnt++;
            u4RegionList =  _u4HWCursorRegionList[_u4HWCursorFlipCnt % 2];
            u4Region = _u4HWCursorRegion[_u4HWCursorFlipCnt % 2];
            
            if(u4RegionList == NULL_REGION_IDX)
            {
                VERIFY(OSD_RGN_LIST_Create(&u4RegionList) == OSD_RET_OK);
                _u4HWCursorRegionList[_u4HWCursorFlipCnt % 2] = u4RegionList;
            }
        
            if(u4Region == NULL_REGION_IDX)
            {
                VERIFY(OSD_RGN_Create(&u4Region, setting.u.set_region.width, setting.u.set_region.height,
                      (void *)setting.u.set_region.base, OSD_CM_ARGB8888_DIRECT32, setting.u.set_region.pitch,
                       0, 0, setting.u.set_region.width, setting.u.set_region.height) == OSD_RET_OK);

                VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_BIG_ENDIAN, 0) == OSD_RET_OK);

                VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_MIX_SEL, OSD_BM_PIXEL) == OSD_RET_OK);

                VERIFY(OSD_RGN_Insert(u4Region, u4RegionList) == OSD_RET_OK);

                VERIFY(OSD_PLA_FlipTo(OSD_PLANE_5, u4RegionList) == OSD_RET_OK);

                _u4HWCursorRegion[_u4HWCursorFlipCnt % 2] = u4Region;
            }
            else
            {
                OSD_RGN_Set(u4Region, OSD_RGN_BMP_W, setting.u.set_region.width);
                OSD_RGN_Set(u4Region, OSD_RGN_DISP_W, setting.u.set_region.width);
                OSD_RGN_Set(u4Region, OSD_RGN_BMP_H, setting.u.set_region.height);  
                OSD_RGN_Set(u4Region, OSD_RGN_DISP_H, setting.u.set_region.height);
                OSD_RGN_Set(u4Region, OSD_RGN_BMP_PITCH, setting.u.set_region.pitch);
                OSD_RGN_Set(u4Region, OSD_RGN_BMP_ADDR, setting.u.set_region.base);

                VERIFY(OSD_PLA_FlipTo(OSD_PLANE_5, u4RegionList) == OSD_RET_OK);
            }
            
 #ifdef CC_SUPPORT_4K2K_UI
            if((OSD_BASE_GetDisplayWidth() == 3840) && (OSD_BASE_GetDisplayHeight() == 2160))
            {
                 if(u4Region != NULL_REGION_IDX)
                {
                  VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_STEP_H, 0x800) == OSD_RET_OK);
                  VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_STEP_V, 0x800) == OSD_RET_OK);
                  VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_OUT_W, setting.u.set_region.width * 2) == OSD_RET_OK);
                  VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_OUT_H, setting.u.set_region.height * 2) == OSD_RET_OK);
                }
            }
            else
            {
                 if(u4Region != NULL_REGION_IDX)
                {
                  VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_STEP_H, 0x1000) == OSD_RET_OK);
                  VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_STEP_V, 0x1000) == OSD_RET_OK);
                  VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_OUT_W, setting.u.set_region.width ) == OSD_RET_OK);
                  VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_OUT_H, setting.u.set_region.height ) == OSD_RET_OK);
                }
            }
#endif
            OSD_PLA_GetEnable(OSD_PLANE_5, &bEnable);

            if(bEnable == FALSE)
            {
                OSD_PLA_SetFading(OSD_PLANE_5, _u4HWCursorOpacity);
                
                OSD_PLA_Enable(OSD_PLANE_5, TRUE);
            }           

        }
        break;

    case HWCURSOR_SET_POSITION:
        {
            if(u4Region != NULL_REGION_IDX)
            {
            
                UINT32 u4Enable, u4SrcW, u4SrcH, u4DstW, u4DstH, u4Is16Bpp;             

                OSD_SC_GetScalerInfo(OSD_SCALER_3, &u4Enable, &u4SrcW, &u4SrcH, &u4DstW, &u4DstH, &u4Is16Bpp);//for android
                if(u4Enable && ( setting.viewport.width <= 1920))
                {
                    setting.u.set_position.pos_x = setting.u.set_position.pos_x * u4DstW / u4SrcW;
                    setting.u.set_position.pos_y = setting.u.set_position.pos_y * u4DstH / u4SrcH;
                }
                
 #ifdef CC_SUPPORT_4K2K_UI
                if((OSD_BASE_GetDisplayWidth() == 3840) && (OSD_BASE_GetDisplayHeight() == 2160))
                {
                     if(u4Region != NULL_REGION_IDX)
                    {
                      UINT32 u4TempWidth, u4TempHeight;
                      OSD_RGN_Get(u4Region, OSD_RGN_BMP_W, &u4TempWidth);
                      OSD_RGN_Get(u4Region, OSD_RGN_BMP_H, &u4TempHeight);
                      VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_STEP_H, 0x800) == OSD_RET_OK);
                      VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_STEP_V, 0x800) == OSD_RET_OK);
                      VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_OUT_W, u4TempWidth * 2) == OSD_RET_OK);
                      VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_OUT_H, u4TempHeight * 2) == OSD_RET_OK);
                    }
                }
                else
                {
                     if(u4Region != NULL_REGION_IDX)
                    {
                      UINT32 u4TempWidth, u4TempHeight;
                      OSD_RGN_Get(u4Region, OSD_RGN_BMP_W, &u4TempWidth);
                      OSD_RGN_Get(u4Region, OSD_RGN_BMP_H, &u4TempHeight);
                      VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_STEP_H, 0x1000) == OSD_RET_OK);
                      VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_STEP_V, 0x1000) == OSD_RET_OK);
                      VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_OUT_W, u4TempWidth) == OSD_RET_OK);
                      VERIFY(OSD_RGN_Set(u4Region, OSD_RGN_OUT_H, u4TempHeight) == OSD_RET_OK);
                    }
                }
#endif
                OSD_RGN_Set(u4Region, OSD_RGN_POS_X, setting.u.set_position.pos_x);
                OSD_RGN_Set(u4Region, OSD_RGN_POS_Y, setting.u.set_position.pos_y);
            }
        }
        break;
        
    case HWCURSOR_SET_ALPHA:
        {
            _u4HWCursorOpacity = setting.u.set_alpha.alpha;
            
            if(u4Region != NULL_REGION_IDX)
            {
                OSD_PLA_SetFading(OSD_PLANE_5, setting.u.set_alpha.alpha);
            }
        }
        break;
        
    case HWCURSOR_GET_PANELSIZE:
        {
            setting.u.get_panelsize.out_width = (UINT32) OSD_BASE_GetDisplayWidth();
            setting.u.get_panelsize.out_height = (UINT32) OSD_BASE_GetDisplayHeight();
            
            if(copy_to_user(pvArg, &setting, sizeof(HWCursorSetting)))
            {
                return -EFAULT;
            }
        }
        break;
        
    default:
        break;
    }

    return 0;
}
#endif
INT32 _mt53fb_do_mw_op(void* pvArg)
{
    mw_gl_op_t tOp;
    unsigned long phy_addr;
    UINT32  u4_plane;
    UINT32  u4_rgn_list;
    BOOL b_enable = FALSE;
    
    if (copy_from_user(&tOp, pvArg, sizeof(mw_gl_op_t)))
    {
        return -EFAULT;
    }

    switch (tOp.u4Op)
    {
    case MW_GL_OP_FILL_RECT:
        phy_addr= (unsigned int) tOp.u.fill_rect.pv_bits;
        GFX_Lock();
        
        if(GFX_Check_DirectFBMem(phy_addr))
        {
            GFX_MMU_Set_Enable(FALSE);
        }
        else
        {
            GFX_MMU_Set_Enable(TRUE);
        }
        
        GFX_Flush_Invalid_DCache();
        GFX_SetDst((UINT8 *) phy_addr, tOp.u.fill_rect.e_mode,
                tOp.u.fill_rect.ui4_pitch);
        GFX_SetColor(tOp.u.fill_rect.ui4_color);
        GFX_Fill(tOp.u.fill_rect.i4_x, tOp.u.fill_rect.i4_y,
                tOp.u.fill_rect.ui4_width, tOp.u.fill_rect.ui4_height);

        GFX_Flush();
        GFX_Task_Wait_Sync();
        GFX_Unlock();
        break;
    case MW_GL_OP_SET_PLANE:
        u4_plane    = fb_osd_get_plane(tOp.u.set_plane.i4_idx);
        if(MT53_LAYER_VDP1 == tOp.u.set_plane.i4_idx)
        {
            VDP_SetEnable(0, tOp.u.set_plane.i4_enable);
            break;
        }

        OSD_PLA_GetEnable(u4_plane, &b_enable);
        if(b_enable == ((BOOL) tOp.u.set_plane.i4_enable))
        {
            break;
        }
        if((-1) == OSD_PLA_GetFirstRegion(u4_plane))
        {
            u4_rgn_list = fb_osd_get_idle_rgn_list(u4_plane);
        }
        else
        {
            u4_rgn_list = OSD_PLA_GetFirstRegionList(u4_plane);
        }
        
        /*printk("_mt53fb_do_mw_op() MW_GL_OP_SET_PLANE[%d,%d,%d,%d]\n",
                   tOp.u.set_plane.i4_idx,u4_plane,u4_rgn_list,tOp.u.set_plane.i4_enable);*/                  
        fb_set_plane_enable(u4_plane,u4_rgn_list,tOp.u.set_plane.i4_enable);
        OSD_PLA_Enable(u4_plane,tOp.u.set_plane.i4_enable);
        break;
    case MW_GL_OP_GET_FBM_BITMAP_INFO:
        tOp.u.get_fbm_bitmap_info.ui4_size = _rImgBufInfo.u4Size;
        tOp.u.get_fbm_bitmap_info.pv_bits = (void*) _rImgBufInfo.u4PhyAddr;
        if (copy_to_user(pvArg, &tOp, sizeof(mw_gl_op_t)))
        {
            return -EFAULT;
        }
        break;
    case MW_GL_OP_ENABLE_SCALER:
        OSD_SC_Scale(   tOp.u.enable_scaler.ui4_id,
                        tOp.u.enable_scaler.ui4_enable,
                        tOp.u.enable_scaler.ui4_width,
                        tOp.u.enable_scaler.ui4_height,
                        OSD_BASE_GetDisplayWidth(),
                        OSD_BASE_GetDisplayHeight());
        
        if(tOp.u.enable_scaler.ui4_id < OSD_SCALER_MAX_NUM)
        {
            _scaler_status[tOp.u.enable_scaler.ui4_id] = (BOOL) tOp.u.enable_scaler.ui4_enable;
        }
        
        break;
#ifdef CC_B2R44K2K_SUPPORT          
    case MW_GL_OP_RELEASE_FBG:
        _mt53_fb_b2r_uninstall();       
        break;
    case MW_GL_OP_SET_B2R_4K2K:
        _mt53_fb_b2r_switch(1);
        break;
    case MW_GL_OP_SET_B2R_FHD:
        _mt53_fb_b2r_switch(0);
        break;
    case MW_GL_OP_SET_B2R_3DPARM:
        _mt53fb_set_3d_param(tOp.u.b2r_3dparm.u4Is3DMode,
                             tOp.u.b2r_3dparm.u4CropBottomOffset,
                             tOp.u.b2r_3dparm.u4CropTopOffset,
                             tOp.u.b2r_3dparm.u4CropLeftOffset,
                             tOp.u.b2r_3dparm.u4CropRightOffset);
        break;
    case MW_GL_OP_GET_B2R_INFO:
        tOp.u.b2r_info.u4YCOffset = _fbg_info.buffer[0].m_CAddr - _fbg_info.buffer[0].m_YAddr;
        if (copy_to_user(pvArg, &tOp, sizeof(mw_gl_op_t)))
        {
            return -EFAULT;
        }
        break;
#endif      
    default:
        printk("Cannot do MW OP %d\n", tOp.u4Op);

    }
    return 0;
}

static int _mt53fb_get_fbm_ioctl(void* pvArg)
{
    struct mt53fb_imagebuffer rBuf[E_OAD_BUF_MAX];
    rBuf[E_OAD_BUF_ONE].u4Size     = _rFbmBufInfo.u4Size;
    rBuf[E_OAD_BUF_ONE].u4PhyAddr  = _rFbmBufInfo.u4PhyAddr;
    rBuf[E_OAD_BUF_ONE].u4VirtAddr = _rFbmBufInfo.u4VirtAddr;

    rBuf[E_OAD_BUF_TWO].u4Size     = _rFbmBufMpeg.u4Size;
    rBuf[E_OAD_BUF_TWO].u4PhyAddr  = _rFbmBufMpeg.u4PhyAddr;
    rBuf[E_OAD_BUF_TWO].u4VirtAddr = _rFbmBufMpeg.u4VirtAddr;
    
    if(copy_to_user(pvArg, (void*)&rBuf, sizeof(struct mt53fb_imagebuffer)*E_OAD_BUF_MAX))
    {
        return -EFAULT;
    }
    return 0;
}

static int _mt53fb_get_crsr_buf_ioctl(void* pvArg)
{
    struct mt53fb_imagebuffer rBuf;
    rBuf.u4Size     = _rCrsrInfo.u4Size;
    rBuf.u4PhyAddr  = _rCrsrInfo.u4PhyAddr;
    rBuf.u4VirtAddr = _rCrsrInfo.u4VirtAddr;
    if(copy_to_user(pvArg, (void*)&rBuf, sizeof(struct mt53fb_imagebuffer)))
    {
        return -EFAULT;
    }
    return 0;
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
static int _mt53fb_ioctl(struct fb_info *info, unsigned int cmd,
        unsigned long arg)
#else
static int _mt53fb_ioctl(struct inode *inode, struct file *file, unsigned int cmd,unsigned long arg, struct fb_info *info)
#endif
{
    void __user *pvArg = (void __user *)arg;
    HAL_TIME_T  t_current,t_start,t_delta;
    INT32 i4Ret = 0;
    switch (cmd)
    {
    case FBIO_GFX_FLUSH:
        if(FB_GFX_PF_DLTA)
        {
            HAL_GetTime(&t_start);
        }
        GFX_DFB_Flush();
        GFX_Unlock();
        if(FB_GFX_PF_DLTA)
        {
            HAL_GetTime(&t_current);
            HAL_GetDeltaTime(&t_delta,&t_start,&t_current); 
            GFX_Set_Dlta(t_delta.u4Seconds,t_delta.u4Micros);
        }
        break;
    case FBIO_GFX_LOCK:
        if(FB_GFX_PF_DLTA)
        {
            HAL_GetTime(&t_start);
        }
        GFX_Lock();
        mt53fb_lock_ex_action(pvArg);
        if(FB_GFX_PF_DLTA)
        {
            HAL_GetTime(&t_current);
            HAL_GetDeltaTime(&t_delta,&t_start,&t_current); 
            GFX_Set_Dlta(t_delta.u4Seconds,t_delta.u4Micros);
        }
        break;
    case FBIO_GFX_FLUSH_DCACHE:
        if(FB_GFX_PF_DLTA)
        {
            HAL_GetTime(&t_start);
        }
        mt53fb_flush_cache_action(pvArg);
        if(FB_GFX_PF_DLTA)
        {
            HAL_GetTime(&t_current);
            HAL_GetDeltaTime(&t_delta,&t_start,&t_current); 
            GFX_Set_Dlta(t_delta.u4Seconds,t_delta.u4Micros);
        }
        break;
    case FBIO_GFX_UNLOCK:
        GFX_Unlock();
        break;
    case FBIO_GFX_WAIT:
        if(FB_GFX_PF_DLTA)
        {
            HAL_GetTime(&t_start);
        }
        GFX_Task_Wait_Sync();
        if(FB_GFX_PF_DLTA)
        {
            HAL_GetTime(&t_current);
            HAL_GetDeltaTime(&t_delta,&t_start,&t_current); 
            GFX_Set_Dlta(t_delta.u4Seconds,t_delta.u4Micros);
        }
        break;
    case FBIO_MMU_GETPGD:
        i4Ret = mt53fb_get_pgt_ioctl((unsigned long) pvArg);
        break;
    case FBIO_MMU_PINMEM:
        i4Ret = mt53fb_pin_mem_ioctl((unsigned long) pvArg);
        break;
    case FBIO_MMU_UNPINMEM:
        i4Ret = mt53fb_unpin_mem_ioctl((unsigned long) pvArg);
        break;
    case FBIO_MMU_MAPSINGLE:
        i4Ret = mt53fb_dma_map_single_ioctl((unsigned long) pvArg);
        break;
    case FBIO_MMU_UNMAPSINGLE:
        i4Ret = mt53fb_dma_unmap_single_ioctl((unsigned long) pvArg);
        break;
    case FBIO_DRAWLINE:
        i4Ret = mt53fb_drawline_ioctl(pvArg);
        break;
    case FBIO_DRAWLINEROP:
        i4Ret = mt53fb_drawlinerop_ioctl(pvArg);
        break;
    case FBIO_DRAWRECT:
        i4Ret = mt53fb_drawrect_ioctl(pvArg);
        break;
    case FBIO_DRAWRECTROP:
        i4Ret = mt53fb_drawrectrop_ioctl(pvArg);
        break;
    case FBIO_DRAWTEXT:
        i4Ret = _mt53fb_drawtext_ioctl(pvArg);
        break;
    case FBIO_RECTFILL:
        i4Ret = mt53fb_fillrect_ioctl(pvArg);
        break;
    case FBIO_HAL_FLUSH_INVALII_DATE_DCACHE:
        i4Ret = mt53fb_HalFlushInvalidateDCache_ioctl(pvArg);
        break;		
    case FBIO_COPYAREA:
        i4Ret = mt53fb_copyarea_ioctl(pvArg);
        break;
    case FBIO_IMAGEBLIT:
        i4Ret = mt53fb_imageblit_ioctl(pvArg);
        break;
    case FBIO_IMAGEBLITROP:
        i4Ret = mt53fb_imageblitrop_ioctl(pvArg);
        break;
    case FBIO_RECTFILLROP:
        i4Ret = mt53fb_fillrectrop_ioctl(pvArg);
        break;
    case FBIO_COPYAREAROP:
        i4Ret = mt53fb_copyarearop_ioctl(pvArg);
        break;
    case FBIO_SET:
        x_sema_lock(hMt53fbMutex, X_SEMA_OPTION_WAIT);
        i4Ret = _mt53fb_set_ioctl(pvArg);
        x_sema_unlock(hMt53fbMutex);
        break;
    case FBIO_GET_PLANE_ORDER_ARRAY:
        i4Ret = _mt53fb_get_plane_order_array_ioctl(pvArg);
        break;
    case FBIO_SET_PLANE_ORDER_ARRAY:
        i4Ret = _mt53fb_set_plane_order_array_ioctl(pvArg);
        break;
    case FBIO_SET_OSD_PLANE_ORDER_ARRAY:
        i4Ret = _mt53fb_set_osd_plane_order_ioctl(pvArg);
        break;
    case FBIO_PALETTE:
        i4Ret = _mt53fb_palette_ioctl(pvArg);
        break;
    case FBIO_GET:
        i4Ret = _mt53fb_get_ioctl(pvArg);
        break;
    case FBIO_GETIMAGEBUFFER:
        i4Ret = _mt53fb_get_imagebuffer_ioctl(pvArg);
        break;
#ifdef CC_FBM_TWO_FBP_SHARED_WITH_DFB
        case FBIO_GETIMAGEBUFFER2:
        i4Ret = _mt53fb_get_imagebuffer2_ioctl(pvArg);
        break;
#endif
#if FB_INIT_BY_AP
        case FBIO_INIT:
        i4Ret = _mt53fb_init_hw(pvArg);
        break;
#endif
#if FB_DBG
        case FBIO_GETTIME:
        i4Ret = _mt53fb_get_time(pvArg);
        break;
#endif
    case FBIO_MW_OP:
        _mt53fb_do_mw_op(pvArg);
        break;
        case FBIO_MW_SET_LANGUAGE:
            i4Ret =_mt53fbSetCurrentLanguage(pvArg);
            break;
        case FBIO_MW_GET_LANGUAGE:
            i4Ret =_mt53fbGetCurrentLanguage(pvArg);
            break;
        case FBIO_MW_SET_LANGUAGE_MODE_CHG:
            i4Ret =_mt53fbSetLanguageModeChg(pvArg);
            break;
        case FBIO_MW_GET_LANGUAGE_MODE_CHG:
            i4Ret =_mt53fbGetLanguageModeChg(pvArg);
            break;

    case FBIO_GFX_STRETCH:
        if(FB_GFX_PF_DLTA)
        {
            HAL_GetTime(&t_start);
        }
        i4Ret = _mt53fb_gfx_stretch_ioctl(pvArg);
        if(FB_GFX_PF_DLTA)
        {
            HAL_GetTime(&t_current);
            HAL_GetDeltaTime(&t_delta,&t_start,&t_current); 
            GFX_Set_Dlta(t_delta.u4Seconds,t_delta.u4Micros);
        }
        break;
#ifdef LINUX_TURNKEY_SOLUTION
        case FBIO_WAITLOGO:
        i4Ret = _mt53fb_wait_logo(pvArg);
        break;
#else
        case FBIO_WAITLOGO:
        i4Ret = 0;
        break;
#endif
    case FBIO_GFX_WAIT_VDP:
        GFX_Wait_Vdp();
        break;
    case FBIO_IMGRZ_STRETCH_ALCOM:
        if(FB_GFX_PF_DLTA)
        {
            HAL_GetTime(&t_start);
        }
        i4Ret = _mt53fb_imgrz_stretch_alcom_ioctl(pvArg);
        if(FB_GFX_PF_DLTA)
        {
            HAL_GetTime(&t_current);
            HAL_GetDeltaTime(&t_delta,&t_start,&t_current); 
            GFX_Set_Dlta(t_delta.u4Seconds,t_delta.u4Micros);
        }
        break;
    case FBIO_VGFX_STRETCH_ALCOM:
        i4Ret = _mt53fb_vgfx_scaler_ioctl(pvArg);
        break;
    case FBIO_VGFX_SCALER:
        i4Ret = _mt53fb_vgfx_ioctl(pvArg);
        break;
#ifdef CC_TDTV_3D_OSD_ENABLE
        case FBIO_GET_SBSOUTPUT:
        i4Ret = _mt53fbGetIsSBSOutput(pvArg);
        break;
        case FBIO_GET_TBBOUTPUT:
        i4Ret = _mt53fbGetIsTBBOutput(pvArg);
        break;
        case FBIO_GET_3DOSD_SHIFT:
        i4Ret = _mt53fbGet3DOsdShiftt(pvArg);
        break;
#endif

#ifdef CC_DFB_SUPPORT_VDP_LAYER
        case FBIO_VDP_INIT:
        i4Ret = _mt53fb_vdp_init_ioctl(pvArg);
        break;
        case FBIO_VDP_FLIP:
        i4Ret = _mt53fb_vdp_flip_ioctl(pvArg);
        break;
        case FBIO_VDP_SET:
        i4Ret = _mt53fb_vdp_set_ioctl(pvArg);
        break;
        case FBIO_VDP_ROTATE:
#ifdef CC_DFB_SUPPORT_VDP_LAYER
        i4Ret = _mt53fb_vdp_rotate_ioctl(pvArg);
#endif
        break;
        case FBIO_VDP_GETBUFFER:
        i4Ret = _mt53fb_get_vdpbuffer_ioctl(pvArg);
        break;
#ifdef CC_B2R44K2K_SUPPORT      
        case FBIO_B2R44K2K_GETBUFFER:           
        i4Ret = _mt53fb_get_b2rbuffer_ioctl(pvArg); 
        break;
        case FBIO_B2R_STRETCH_BLIT:
        i4Ret = _mt53fb_b2r_stretch_blit_ioctl(pvArg);
        break;
#endif          
        case FBIO_VDP_SET_CROP_RECT:
        i4Ret = _mt53fb_set_vdp_crop_rect_ioctl(pvArg);
        break;
        case FBIO_VDP_GET_CROP_RECT:
        i4Ret = _mt53fb_get_vdp_crop_rect_ioctl(pvArg);
        break;
        case FBIO_VDP_STRETCH_BLIT:
        i4Ret = _mt53fb_vdp_stretch_blit_ioctl(pvArg);
        break;
#endif
    case FBIO_FBM_INFO:
        i4Ret = _mt53fb_get_fbm_ioctl(pvArg);
        break;
    case FBIO_CRSR_INFO:
        i4Ret = _mt53fb_get_crsr_buf_ioctl(pvArg);
        break;
    case FBIO_WAIT_VSYNC:
        OSD_WaitVsync();
        break;
    case FBIO_GET_TMP_BUF:
        _mt53fb_get_tmp_buf_ioctl(pvArg);
        break;
    case FBIO_LOCK_TMP_BUF:
        GFX_TmpBuf_Lock();
        break;
    case FBIO_UNLOCK_TMP_BUF:
        GFX_TmpBuf_Unock();
        break;    
#ifdef CC_SUPPORT_DYNAMIC_ENABLE_DFB_INPUTEVENT     
        case MTKIO_SET_DFBDISCARDKEY:
            {
                down(&_gtDFBKeyInfo.tSem);
                if(copy_from_user(&_gtDFBKeyInfo.u4IsDiscard, pvArg, sizeof(UINT32))){
                    printk(KERN_WARNING "misc_ioctl() - copy_from_user() fail\n");
                    return -EFAULT;
                }
                up(&_gtDFBKeyInfo.tSem);
            }
            break;
        case MTKIO_CHECK_DFBDISCARDKEY:
            {
                return dfbkey_check(pvArg);
            }
            break;
        case MTKIO_DUMP_DFBKEY:
            {
                MTK_KEY_NODE_T *ptNode;
                struct list_head *l;            
                Printf("KEY >>>>>>>>>>>>>>>>>>>>>>>>>>>>>> \n");
                down(&_gtDFBKeyInfo.tSem);
                Printf("DFBKey Disacard = %d \n", _gtDFBKeyInfo.u4IsDiscard);
                list_for_each(l, &_gtDFBKeyInfo.tList)
                {
                    ptNode = list_entry(l, MTK_KEY_NODE_T, list);
                    Printf("Node token[%s], keysymbol[0x%x] \n", ptNode->ucToken, ptNode->u4KeySymbol);                             
                }
                up(&_gtDFBKeyInfo.tSem);
                Printf("<<<<<<<<<<<<<<<<<<<<<<< \n");
            }
            break;  
#endif      
    default:
        //printk(KERN_WARNING "_mt53fb_ioctl(): not support ioctl cmd %d\n", cmd);
        i4Ret = -EINVAL;
        break;
    }
    return i4Ret;
}

/* --------------------------------------------------------------------- */

static void __init _mt53fb_parse_colormode(char *opt)
{
}

static void __init _mt53fb_parse_dram(char *opt)
{
    unsigned int size;

    size = simple_strtoul(opt, &opt, 0);

    if (opt)
    {
        switch (*opt)
        {
            case 'M':
            case 'm':
            size *= 1024 * 1024;
            break;
            case 'K':
            case 'k':
            size *= 1024;
            break;
            default:
            break;
        }
    }

    _cur_par.dram_size = size;
}

static void __init _mt53fb_init_fbinfo(void)
{
    static int first = 1;

    if (!first)
    return;
    first = 0;

    _fb_info.fbops = &_mt53fb_ops;
    _fb_info.flags = FBINFO_DEFAULT;
    _fb_info.pseudo_palette = _cur_par.pseudo_palette;

    strncpy(_fb_info.fix.id, "mt53xx", sizeof(_fb_info.fix.id));
    _fb_info.fix.type = FB_TYPE_PACKED_PIXELS;
    _fb_info.fix.type_aux = 0;
    _fb_info.fix.xpanstep = 0;
    _fb_info.fix.ypanstep = 0;
    _fb_info.fix.ywrapstep = 0;
    _fb_info.fix.line_length = 0;
    _fb_info.fix.accel = FB_ACCEL_NONE;

    /*
     * setup initial parameters
     */
    memset(&_fb_info.var, 0, sizeof(_fb_info.var));

    _fb_info.var.xres = MAX_X_RES;
    _fb_info.var.yres = MAX_Y_RES;
    _fb_info.var.xres_virtual = MAX_X_RES;
    _fb_info.var.yres_virtual = MAX_Y_RES * 2;
    _fb_info.var.xoffset = 0;
    _fb_info.var.yoffset = 0;
    _fb_info.var.bits_per_pixel = 16;
#ifdef LINUX_TURNKEY_SOLUTION
    _fb_info.var.red.length = 4;
    _fb_info.var.green.length = 4;
    _fb_info.var.blue.length = 4;
    _fb_info.var.transp.length = 4;
#else
    _fb_info.var.red.length = 5;
    _fb_info.var.green.length = 6;
    _fb_info.var.blue.length = 5;
    _fb_info.var.transp.length = 0;
#endif
    _fb_info.var.nonstd = 0;
    _fb_info.var.activate = FB_ACTIVATE_NOW;
    _fb_info.var.width = MAX_X_RES;
    _fb_info.var.height = MAX_Y_RES;
    _fb_info.var.vmode = FB_VMODE_NONINTERLACED;
    _fb_info.var.accel_flags = FB_ACCELF_TEXT;

    _cur_par.dram_size = 0;
}

#if !defined(ANDROID) || defined(ANDROID_TWO_WORLDS)
static UINT32 fb_dec_get_cfg_buf_idx(UINT32 u4_plane)
{
    UINT32 u4_dec_idx = OSD_INVALID_VALUE_U32;
    switch(u4_plane)
    {
        case OSD_PLANE_1:
            u4_dec_idx = e_config_osd_dec_plane1_mem_size;
            break;
        case OSD_PLANE_2:
            u4_dec_idx = e_config_osd_dec_plane2_mem_size;
            break;
        case OSD_PLANE_3:
            u4_dec_idx = e_config_osd_dec_plane3_mem_size;
            break;
        case OSD_PLANE_4:
            u4_dec_idx = e_config_osd_dec_plane4_mem_size;
            break;
        case OSD_PLANE_5:
            u4_dec_idx = e_config_osd_dec_plane5_mem_size;
            break;
        default:
            break;
    }
    return u4_dec_idx;
}
static UINT32 fb_cmpress_init_buf(UINT32 u4_start)
{
    UINT32 u4_plane     = 0x0;
    UINT32 u4_length    = 0x0;
    UINT32 u4_dec_mem_size  = 0x0;
    FB_OSD_RGN_T        t_rgn_data;

    for(u4_plane=0x0; u4_plane < OSD_PLANE_MAX_NUM; u4_plane++)
    {
        if(OSD_GetCpblty(E_OSD_CPBLTY_CMPRSS_PLANE,u4_plane))
        {
            u4_dec_mem_size = DRVCUST_OptGet(fb_dec_get_cfg_buf_idx(u4_plane));
            fb_osd_get_rgn_obj(u4_plane,E_FB_OSD_RGN_LEFT,&t_rgn_data);
            t_rgn_data.u4_addr = u4_start;
            fb_osd_set_rgn_obj(u4_plane,E_FB_OSD_RGN_LEFT,&t_rgn_data);
            fb_osd_get_rgn_obj(u4_plane,E_FB_OSD_RGN_RIGHT,&t_rgn_data);
            t_rgn_data.u4_addr = (u4_start + (u4_dec_mem_size/2));
            fb_osd_set_rgn_obj(u4_plane,E_FB_OSD_RGN_RIGHT,&t_rgn_data);

            printf("fb_cmpress_init_buf[%d,0x%x]\n",u4_plane,u4_dec_mem_size);
            u4_length += (u4_dec_mem_size);
            u4_start += u4_length;
        }
    }

    return u4_length;
}

static UINT32 fb_3d_get_cfg_buf_idx(UINT32 u4_plane)
{
    UINT32 u4_dec_idx = OSD_INVALID_VALUE_U32;
    switch(u4_plane)
    {
        case OSD_PLANE_1:
            u4_dec_idx = e_config_osd_3d_plane1_mem_size;
            break;
        case OSD_PLANE_2:
            u4_dec_idx = e_config_osd_3d_plane2_mem_size;
            break;
        case OSD_PLANE_3:
            u4_dec_idx = e_config_osd_3d_plane3_mem_size;
            break;
        case OSD_PLANE_4:
            u4_dec_idx = e_config_osd_3d_plane4_mem_size;
            break;
        case OSD_PLANE_5:
            u4_dec_idx = e_config_osd_3d_plane5_mem_size;
            break;
        default:
            break;
    }
    return u4_dec_idx;
}
static UINT32 fb_3d_init_buf(UINT32 u4_start)
{
    UINT32 u4_plane     = 0x0;
    UINT32 u4_length    = 0x0;
    UINT32 u4_3d_mem_size   = 0x0;
    FB_OSD_RGN_T        t_rgn_data;

    for(u4_plane=0x0; u4_plane < OSD_PLANE_MAX_NUM; u4_plane++)
    {
        if(OSD_GetCpblty(E_OSD_CPBLTY_SBS_PLANE,u4_plane))
        {
            u4_3d_mem_size = DRVCUST_OptGet(fb_3d_get_cfg_buf_idx(u4_plane));
            fb_osd_get_rgn_obj(u4_plane,E_FB_OSD_RGN_LEFT,&t_rgn_data);
            t_rgn_data.u4_3d_addr = u4_start;
            fb_osd_set_rgn_obj(u4_plane,E_FB_OSD_RGN_LEFT,&t_rgn_data);
            fb_osd_get_rgn_obj(u4_plane,E_FB_OSD_RGN_RIGHT,&t_rgn_data);
            t_rgn_data.u4_3d_addr = (u4_start + (u4_3d_mem_size/2));
            fb_osd_set_rgn_obj(u4_plane,E_FB_OSD_RGN_RIGHT,&t_rgn_data);

            u4_length += (u4_3d_mem_size);
            u4_start += u4_length;
        }
    }

    return u4_length;
}

static int __init _mt53fb_probe(struct device *dev)
{
    unsigned long           size;
    char *option            = NULL;
    #ifdef CC_OSD_USE_FBM
    #if defined(CC_FBM_TWO_FBP_SHARED_WITH_DFB)  || !defined(CC_DFB_SUPPORT_VDP_LAYER)
    UINT32 u4AddrOsd1, u4AddrOsd4;
    #endif
    #endif
    UINT32 u4_cmp_buf_size  = 0x0;

    if (fb_get_options("mt53fb", &option))
    return -ENODEV;

    _mt53fb_setup(option);

    _cur_par.dev = dev;

    if (!_cur_par.dram_size)
    {
        size = MAX_SIZE;
    }
    else
    {
        size = _cur_par.dram_size;
        if (size > MAX_SIZE)
        {
            size = MAX_SIZE;
        }
    }

    size = PAGE_ALIGN(size);

    // Image buffer outside FBM's control
    // Ensure page alignment
    VERIFY(PAGE_ALIGN(fb_osd_get_fb_size() & ~PAGE_MASK) == 0);
    VERIFY(PAGE_ALIGN(DRVCUST_OptGet(eDirectFBMemSize) & ~PAGE_MASK) == 0);

    _fb_info.fix.smem_start = ((BSP_GetFbmMemAddr() - DRVCUST_OptGet(eDirectFBMemSize)) & PAGE_MASK);

    // Let smem_len excludes font-rendering buffer
    _fb_info.fix.smem_len = DRVCUST_OptGet(eDirectFBMemSize);
    printk("eDirectFBMemSize: smem_start = %x, smem_len=%x \n",
                    _fb_info.fix.smem_start,
                    _fb_info.fix.smem_len);

    GFX_SetDirectFBMem(_fb_info.fix.smem_start, _fb_info.fix.smem_len);
    /*
        GFX_MemsetAsync((UINT8 *)_fb_info.fix.smem_start,((GFX_DST_PITCH_MAX&0xF000)/4),
                    (_fb_info.fix.smem_len/(GFX_DST_PITCH_MAX&0xF000)),0x0);
        */

#ifdef CC_OSD_USE_FBM
#ifndef CC_DFB_SUPPORT_VDP_LAYER
    {
        FBM_POOL_T *prFbmPool;
        FBM_Init();
        prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_JPEG_OSD1);
        ASSERT(prFbmPool != NULL);
        ASSERT(prFbmPool->u4Addr != 0x0);
        _rImgBufInfo.u4Size = prFbmPool->u4Size;
        u4AddrOsd1 = prFbmPool->u4Addr;
        prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_JPEG_OSD2);
        ASSERT(prFbmPool != NULL);
        ASSERT(prFbmPool->u4Addr != 0x0);
        _rImgBufInfo.u4Size += prFbmPool->u4Size;
        prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_JPEG_OSD3);
        ASSERT(prFbmPool != NULL);
        ASSERT(prFbmPool->u4Addr != 0x0);
        _rImgBufInfo.u4Size += prFbmPool->u4Size;
        prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_JPEG_OSD4);
        ASSERT(prFbmPool != NULL);
        ASSERT(prFbmPool->u4Addr != 0x0);
        _rImgBufInfo.u4Size += prFbmPool->u4Size;
        u4AddrOsd4 = prFbmPool->u4Addr;

        if (u4AddrOsd1 < u4AddrOsd4)
        {
            _rImgBufInfo.u4PhyAddr = u4AddrOsd1;
        }
        else
        {
            _rImgBufInfo.u4PhyAddr = u4AddrOsd4;
        }

        _rImgBufInfo.u4VirtAddr = _rImgBufInfo.u4PhyAddr;

        printk("DFB JPEG OSD: %x, PHY:%x, SIZE: %x\n",
                _rImgBufInfo.u4VirtAddr,
                _rImgBufInfo.u4PhyAddr,
                _rImgBufInfo.u4Size);
    }
#else
    {
        FBM_POOL_T *prFbmPool;
        FBM_Init();

        prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_SCPOS_MAIN);
        ASSERT(prFbmPool != NULL);
        ASSERT(prFbmPool->u4Addr != (UINT32)NULL);
        _rImgBufInfo.u4Size = prFbmPool->u4Size;
        _rImgBufInfo.u4PhyAddr = prFbmPool->u4Addr;
        _rImgBufInfo.u4VirtAddr = _rImgBufInfo.u4PhyAddr;

        printk("DFB FROM FBM: %x, PHY:%x, SIZE: %x\n",
                _rImgBufInfo.u4VirtAddr,
                _rImgBufInfo.u4PhyAddr,
                _rImgBufInfo.u4Size);

        prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_JPG_VDP);
        ASSERT(prFbmPool != NULL);
        ASSERT(prFbmPool->u4Addr != (UINT32)NULL);
        // Handle start offset alignment
        if((prFbmPool->u4Addr & PAGE_MASK) != 0)
        {
            _rVdpBufInfo.u4PhyAddr = ((prFbmPool->u4Addr - 1) >> PAGE_SHIFT << PAGE_SHIFT) + PAGE_SIZE;
        }
        else
        {
            _rVdpBufInfo.u4PhyAddr = prFbmPool->u4Addr;
        }
        // Handle size alignment
        _rVdpBufInfo.u4Size = (prFbmPool->u4Addr + prFbmPool->u4Size - _rVdpBufInfo.u4PhyAddr) & PAGE_MASK;
        _rVdpBufInfo.u4VirtAddr = _rVdpBufInfo.u4PhyAddr;

        printk("DFB JPEG VDP: %x, PHY:%x, SIZE: %x\n",
                _rVdpBufInfo.u4VirtAddr,
                _rVdpBufInfo.u4PhyAddr,
                _rVdpBufInfo.u4Size);
    }

#endif

    {
        FBM_POOL_T *prFbmPoolFirst,*prFbmPoolEnd;
        FBM_Init();
        prFbmPoolFirst  = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_SCPOS_MAIN);
        ASSERT(prFbmPoolFirst != NULL);
        ASSERT(prFbmPoolFirst->u4Addr != (UINT32)NULL);
        prFbmPoolEnd    = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_DMX2);
        ASSERT(prFbmPoolEnd != NULL);
        ASSERT(prFbmPoolEnd->u4Addr != (UINT32)NULL);

        if((prFbmPoolFirst->u4Addr & PAGE_MASK) != 0)
        {
            _rFbmBufInfo.u4PhyAddr = (((prFbmPoolFirst->u4Addr - 1) >> PAGE_SHIFT << PAGE_SHIFT) + PAGE_SIZE);
        }
        else
        {
            _rFbmBufInfo.u4PhyAddr = prFbmPoolFirst->u4Addr;
        }
        _rFbmBufInfo.u4Size     = (prFbmPoolFirst->u4Size&PAGE_MASK);
        _rFbmBufInfo.u4VirtAddr = _rFbmBufInfo.u4PhyAddr;
        
        if((prFbmPoolEnd->u4Addr & PAGE_MASK) != 0)
        {
            _rFbmBufMpeg.u4PhyAddr = (((prFbmPoolEnd->u4Addr - 1) >> PAGE_SHIFT << PAGE_SHIFT) + PAGE_SIZE);
        }
        else
        {
            _rFbmBufMpeg.u4PhyAddr = prFbmPoolEnd->u4Addr;
        }
        _rFbmBufMpeg.u4Size     = (prFbmPoolEnd->u4Size&PAGE_MASK);
        _rFbmBufMpeg.u4VirtAddr = _rFbmBufMpeg.u4PhyAddr;
        
        printk("FBM OAD: VIR:%x, PHY:%x, SIZE: %x: VIR:%x, PHY:%x, SIZE: %x\n",
                _rFbmBufInfo.u4VirtAddr,
                _rFbmBufInfo.u4PhyAddr,
                _rFbmBufInfo.u4Size,
                _rFbmBufMpeg.u4VirtAddr,
                _rFbmBufMpeg.u4PhyAddr,
                _rFbmBufMpeg.u4Size);
    }

#ifdef CC_FBM_TWO_FBP_SHARED_WITH_DFB
    {
        FBM_POOL_T *prFbmPool;

        FBM_Init();
        prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_FEEDER2);
        ASSERT(prFbmPool != NULL);
        ASSERT(prFbmPool->u4Addr != 0x0);
        //_rImgBufInfo2.u4Size = prFbmPool->u4Size;
        u4AddrOsd1 = prFbmPool->u4Addr;
        u4AddrOsd4 = prFbmPool->u4Addr + prFbmPool->u4Size;
        prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_DMX2);
        ASSERT(prFbmPool != NULL);
        ASSERT(prFbmPool->u4Addr != 0x0);
        //_rImgBufInfo2.u4Size += prFbmPool->u4Size;
        u4AddrOsd1 = (prFbmPool->u4Addr < u4AddrOsd1)?(prFbmPool->u4Addr):u4AddrOsd1;
        u4AddrOsd4 = (prFbmPool->u4Addr + prFbmPool->u4Size > u4AddrOsd4)?(prFbmPool->u4Addr + prFbmPool->u4Size):u4AddrOsd4;
        prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_MPEG2);
        ASSERT(prFbmPool != NULL);
        ASSERT(prFbmPool->u4Addr != 0x0);
        //_rImgBufInfo2.u4Size += prFbmPool->u4Size;
        _rImgBufInfo2.u4PhyAddr = (prFbmPool->u4Addr < u4AddrOsd1)?(prFbmPool->u4Addr):u4AddrOsd1;
        u4AddrOsd4 = (prFbmPool->u4Addr + prFbmPool->u4Size > u4AddrOsd4)?(prFbmPool->u4Addr + prFbmPool->u4Size):u4AddrOsd4;

        // Round to page align
        _rImgBufInfo2.u4PhyAddr = (((_rImgBufInfo2.u4PhyAddr - 1) >> PAGE_SHIFT) << PAGE_SHIFT) + PAGE_SIZE;
        u4AddrOsd4 &= PAGE_MASK;
        _rImgBufInfo2.u4Size = u4AddrOsd4 - _rImgBufInfo2.u4PhyAddr;
        _rImgBufInfo2.u4VirtAddr = _rImgBufInfo2.u4PhyAddr;

        printk("DFB 3DMM Buffer: %x, PHY:%x, SIZE: %x\n",
                _rImgBufInfo2.u4VirtAddr,
                _rImgBufInfo2.u4PhyAddr,
                _rImgBufInfo2.u4Size);
    }
#endif /* CC_FBM_TWO_FBP_SHARED_WITH_DFB */

#else
    _rImgBufInfo.u4PhyAddr = _fb_info.fix.smem_start;

    // Real image buffer excludes font-rendering buffer
    _rImgBufInfo.u4Size = _fb_info.fix.smem_len;
#endif

    // If _fb_info.smem_start is below tha kernel's DRAM limit, FB cannot successfully initialize
    //    if(_fb_info.fix.smem_start < KERNEL_DRAM_SIZE)
    if(_fb_info.fix.smem_start < DRVCUST_OptGet(eLinuxKernelMemSize))
    {
        printk(KERN_ERR "_mt53fb_probe() fail: bad framebuffer memory layout! KM:%d\n", DRVCUST_OptGet(eLinuxKernelMemSize));
        return -ENOMEM;
    }

    _fb_info.screen_base = (void *)_fb_info.fix.smem_start;


    if (_fb_info.screen_base == NULL)
    {
        return -ENOMEM;
    }

    u4_cmp_buf_size = fb_cmpress_init_buf((UINT32)_fb_info.fix.smem_start);

    _fb_info.screen_base    += u4_cmp_buf_size;
    _fb_info.fix.smem_start += u4_cmp_buf_size;
    _fb_info.fix.smem_len   -= u4_cmp_buf_size;


    u4_cmp_buf_size  = DRVCUST_OptGet(e_config_gfx_temp_mem_size);
    GFX_SetTmpBuf(_fb_info.fix.smem_start);
    GFX_SetTmpBufSize(u4_cmp_buf_size);
    _fb_info.screen_base    += u4_cmp_buf_size;
    _fb_info.fix.smem_start += u4_cmp_buf_size;
    _fb_info.fix.smem_len   -= u4_cmp_buf_size;

    printk("TmpBuf = 0x%x, size[0x%x] \n", GFX_GetTmpBuf(),u4_cmp_buf_size);

    _rCrsrInfo.u4Size       = DRVCUST_OptGet(e_config_osd_plane5_mem_size);
    _rCrsrInfo.u4PhyAddr    = _fb_info.fix.smem_start;
    printk("OSD Crsr= 0x%x,size[0x%x]\n",_rCrsrInfo.u4PhyAddr,_rCrsrInfo.u4Size);
    _fb_info.screen_base    += _rCrsrInfo.u4Size;
    _fb_info.fix.smem_start += _rCrsrInfo.u4Size;
    _fb_info.fix.smem_len   -= _rCrsrInfo.u4Size;

    u4_cmp_buf_size = fb_3d_init_buf((UINT32)_fb_info.fix.smem_start);
    _fb_info.screen_base    += u4_cmp_buf_size;
    _fb_info.fix.smem_start += u4_cmp_buf_size;
    _fb_info.fix.smem_len   -= u4_cmp_buf_size;

#ifdef LINUX_TURNKEY_SOLUTION
    printk("DFB screen_base[0x%x], Size[0x%x]\n",
            _fb_info.fix.smem_start,
            _fb_info.fix.smem_len);
#endif

#ifdef CC_B2R44K2K_SUPPORT
    _mt53_fb_b2r_init(); 
#endif

    fb_osd_palette_init();

    _pu1TextArray = (unsigned char*)BSP_AllocAlignedDmaMemory(TEXT_SIZE,0x10);
    _pu1TextArrayVirt = (unsigned char*)VIRTUAL((UINT32)_pu1TextArray);

    if (register_framebuffer(&_fb_info) < 0)
    {
        printk(KERN_ERR "_mt53fb_probe() fail: cannot register mt53fb!\n");
        return -ENXIO;
    }

    return 0;
}
#endif


static int _mt53fb_drawtext_ioctl(void* pvArg)
{
    struct fb_info* prFb;
    struct mt53fb_DrawText rDrawText;
    struct mt53fb_DrawText* prDrawText;
    unsigned char* pu1Virt = NULL;

    prDrawText = (struct mt53fb_DrawText*) (pvArg);

    pu1Virt = (unsigned char*) VIRTUAL((UINT32) _pu1TextArray);

    prFb = mt53fb_get_fb_info();

    //printk("address : %x %x\n",pu1Virt,(prDrawText->pTextArray));

    if (copy_from_user(pu1Virt, (prDrawText->pTextArray), sizeof(TEXT_SIZE)))
    {
        //printk(KERN_WARNING "mt53fb_drawtext_ioctl() error: Copy from user error1!\n");
        return -EFAULT;
    }

    rDrawText.pTextArray = (void*) pu1Virt;

    if (copy_from_user((void*) &rDrawText, pvArg,
            sizeof(struct mt53fb_DrawText)))
    {
        //printk(KERN_WARNING "mt53fb_drawtext_ioctl() error: Copy from user error2!\n");
        return -EFAULT;
    }

    return mt53fb_drawtext(prFb, &rDrawText);

}

static int _mt53fb_palette_ioctl(void* pvArg)
{
    UINT32 ui4_id = 0x0;
    FB_OSD_RGN_T t_left,t_rght;

    if (copy_from_user(&ui4_id, pvArg, 4))
    {
        return -EFAULT;
    }

    ui4_id = fb_osd_get_plane(ui4_id);
    OSD_VERIFY_PLANE(ui4_id);

    memcpy(fb_osd_get_palette_virt(ui4_id), (pvArg + 4), PALETTE_SIZE);

    fb_osd_get_rgn_obj(ui4_id,E_FB_OSD_RGN_LEFT,&t_left);
    fb_osd_get_rgn_obj(ui4_id,E_FB_OSD_RGN_RIGHT,&t_rght);

    FB_OSD_SET_RGN(OSD_RGN_PAL_ADDR, (UINT32)fb_osd_get_palette(ui4_id));

    FB_OSD_SET_RGN(OSD_RGN_PAL_LEN,  0x2);
    FB_OSD_SET_RGN(OSD_RGN_PAL_LOAD, 0x1);

    return 0;
}


static int _mt53fb_get_ioctl(void* pvArg)
{
    //int i4Ret = 0;

    INT32 i4Region = -1;
    UINT32 u4Plane = 0, u4Width = 0, u4Height = 0;

    struct mt53fb_get rGet;

    if (copy_from_user(&rGet, pvArg, sizeof(struct mt53fb_get)))
    {
        //printk(KERN_WARNING "_mt53fb_get_ioctl() - copy_from_user() fail\n");
        return -EFAULT;
    }

    if (rGet.get_type == MT53FB_GET_PANEL_SIZE)
    {
        unsigned int data[2];
        data[0] = (UINT32) OSD_BASE_GetDisplayWidth();
        data[1] = (UINT32) OSD_BASE_GetDisplayHeight();
        if (copy_to_user((void*) rGet.get_data, data, rGet.get_size))
        {
            //printk(KERN_WARNING "_mt53fb_get_ioctl() - copy_to_user() fail\n");
            return -EFAULT;
        }
    }
    else if(MT53FB_GET_GFX_DATA == rGet.get_type)
    {
        unsigned int data[E_DFB_DBG_INFO_MAX+1];
        unsigned int u4_lp=0x0;

        for(u4_lp=0x0;u4_lp<E_DFB_DBG_INFO_MAX;u4_lp++)
        {
            data[u4_lp] = gfx_dfb_get_dbg_info(u4_lp);
        }

        if(copy_to_user((void*) rGet.get_data, data, sizeof(unsigned int)*E_DFB_DBG_INFO_MAX))
        {
            return -EFAULT;
        }
    }
    else if(MT53FB_GET_DFB_LAYER0_SIZE == rGet.get_type)
    {
        unsigned int data[2];
        u4Plane = fb_osd_get_plane(MT53_LAYER_OSD1);
        i4Region = OSD_PLA_GetFirstRegion(u4Plane);
        OSD_RGN_Get(i4Region, OSD_RGN_BMP_W, &u4Width);
        OSD_RGN_Get(i4Region, OSD_RGN_BMP_H, &u4Height);
        data[0] = u4Width;
        data[1] = u4Height;
        if (copy_to_user((void*) rGet.get_data, data, rGet.get_size))
        {
            return -EFAULT;
        }       
    }

    return 0;
}

static INT32 fb_osd_set_rgn_obj(UINT32 u4_plane,UINT32 u4_rgn,FB_OSD_RGN_T* pt_rgn_list)
{
    CHECK_FB_OSD_RGN_LIST_PARA(u4_plane,u4_rgn,pt_rgn_list);

    memcpy(&au4_plane_rgn[u4_plane][u4_rgn],pt_rgn_list,sizeof(FB_OSD_RGN_T));

    return OSD_RET_OK;
}



INT32 fb_gfx_sbs_action(UINT32 e3DMode, UINT32 u4Shift,
        UINT32 u4BmpWidth, UINT32 u4BmpHeight, UINT32 u4BmpPitch,
        UINT32 u4DstAddr, UINT32 eColorMode, UINT32 u4SrcAddr)
{
    RZ_OSD_SCL_PARAM_SET_T rSclParam;
    E_RZ_INOUT_TYPE_T eRzType;

    x_memset(&rSclParam,0,sizeof(RZ_OSD_SCL_PARAM_SET_T));

    if (e3DMode == OSD_3D_SBSHALF_PR)//OSD_3D_SBSHALF
    {
        rSclParam.u4TgW = u4BmpWidth >> 1;
        rSclParam.u4TgH = u4BmpHeight;
    }
    else if (e3DMode == OSD_3D_TPANDBT_PR) //OSD_3D_TPANDBT
    {
        rSclParam.u4TgW = u4BmpWidth;
        rSclParam.u4TgH = u4BmpHeight >> 1;
    }
    else
    {
        GFX_Lock();
        GFX_MMU_Set_Enable(0x0);
        GFX_SetSrc((UINT8 *)u4SrcAddr, eColorMode, u4BmpPitch);
        GFX_SetDst((UINT8 *)u4DstAddr, eColorMode, u4BmpPitch);
        GFX_BitBlt(0, 0, 0, 0, u4BmpWidth, u4BmpHeight);

        GFX_Flush();
        GFX_Task_Wait_Sync();
        GFX_Unlock();
        return (INT32)OSD_RET_OK;
    }

    rSclParam.u4SrcBufLen = u4BmpPitch;
    rSclParam.u4SrcW = u4BmpWidth;
    rSclParam.u4SrcH = u4BmpHeight;

    rSclParam.u4SrcBase = u4SrcAddr;
    rSclParam.u4IsSrcDirCM = 1;
    rSclParam.u4SrcCM = (eColorMode == OSD_CM_ARGB4444_DIRECT16)?
    (UINT32) E_RZ_OSD_DIR_CM_ARGB4444:(UINT32) E_RZ_OSD_DIR_CM_AYCbCr8888;
    rSclParam.u4SrcHOffset = 0;
    rSclParam.u4SrcVOffset = 0;

    rSclParam.u4TgBase = u4DstAddr;

    rSclParam.u4TgHOffset = 0;
    rSclParam.u4TgVOffset = 0;

    rSclParam.u4TgCM = rSclParam.u4SrcCM;
    rSclParam.u4TgBufLen = u4BmpPitch;

    eRzType = E_RZ_INOUT_TYPE_OSDMD;

    IMGRZ_Lock();
    IMGRZ_ReInit();
    IMGRZ_SetScaleOpt(eRzType);
    IMGRZ_Scale(&rSclParam);
    IMGRZ_Flush();
    IMGRZ_Wait();
    IMGRZ_Unlock();

    GFX_Lock();
    GFX_MMU_Set_Enable(0x0);
    GFX_SetSrc((UINT8 *)u4DstAddr, eColorMode, u4BmpPitch);

    GFX_SetDst((UINT8 *)u4DstAddr, eColorMode, u4BmpPitch);
    GFX_SetColor(0x0);
    GFX_SetBltOpt((UINT32)D_GFXFLAG_NONE, 0x00000000, 0xffffffff);

    if (e3DMode == OSD_3D_SBSHALF_PR)// OSD_3D_SBSHALF

    {
        GFX_Fill(rSclParam.u4TgW, 0, rSclParam.u4TgW, rSclParam.u4TgH);
        GFX_BitBlt((u4Shift >> 1), 0, rSclParam.u4TgW, 0, (rSclParam.u4TgW - (u4Shift >> 1)), rSclParam.u4TgH);
    }
    else //OSD_3D_TPANDBT

    {
        GFX_Fill(0, rSclParam.u4TgH, rSclParam.u4TgW, rSclParam.u4TgH);
        GFX_BitBlt(u4Shift, 0, 0, rSclParam.u4TgH, (rSclParam.u4TgW - u4Shift), rSclParam.u4TgH);
    }

    GFX_SetSrc((UINT8 *)u4DstAddr, eColorMode, u4BmpPitch);
    GFX_SetDst((UINT8 *)u4DstAddr, eColorMode, u4BmpPitch);
    GFX_BitBlt(0, 0, 0, 0, u4BmpWidth, u4BmpHeight);

    GFX_Flush();
    GFX_Task_Wait_Sync();
    GFX_Unlock();

    return (INT32)OSD_RET_OK;

}


static void fb_osd_sbs_action(FB_OSD_RGN_T*  pt_this,UINT32 u4_plane,UINT32 u4_bmp_addr)
{
    UINT32 u4ColorMode       = 0x0;
    UINT32 u4_rgn_pitch      = 0x0;
    UINT32 u4_rgn_hight      = 0x0;
    UINT32 u4_rgn_width      = 0x0;

    VERIFY(OSD_RGN_Get(pt_this->u4_rgn,OSD_RGN_COLORMODE,&u4ColorMode) == OSD_RET_OK);
    VERIFY(OSD_RGN_Get(pt_this->u4_rgn,OSD_RGN_BMP_PITCH,&u4_rgn_pitch) == OSD_RET_OK);
    VERIFY(OSD_RGN_Get(pt_this->u4_rgn,OSD_RGN_BMP_W,&u4_rgn_width) == OSD_RET_OK);
    VERIFY(OSD_RGN_Get(pt_this->u4_rgn,OSD_RGN_BMP_H,&u4_rgn_hight) == OSD_RET_OK);
    OSD_RGN_Set(pt_this->u4_rgn, OSD_RGN_BMP_ADDR,pt_this->u4_3d_addr);

    fb_gfx_sbs_action(OSD_GetCpblty(E_OSD_CPBLTY_3D_STATUS,u4_plane),0x0,
                    u4_rgn_width,u4_rgn_hight,u4_rgn_pitch,
                    pt_this->u4_3d_addr,u4ColorMode,u4_bmp_addr);

    return;
}

void fb_gfx_set_draw_line(UINT32 u4_num)
{
    u4_draw_line = u4_num;
    return;
}

UINT32 fb_gfx_get_draw_line(void)
{
    return u4_draw_line;
}

UINT32* fb_gfx_get_draw_line_data(void)
{
    if(!au4_draw_lines)
    {
        au4_draw_lines = (UINT32 *)BSP_AllocAlignedDmaMemory(32*1024,0x10);
        au4_draw_lines = (UINT32 *)VIRTUAL((UINT32)au4_draw_lines);
        au4_draw_lines[0]=10;
        au4_draw_lines[1]=10;
        au4_draw_lines[2]=200;
        au4_draw_lines[3]=10;

        au4_draw_lines[4]=200;
        au4_draw_lines[5]=10;
        au4_draw_lines[6]=200;
        au4_draw_lines[7]=200;
    }

    return au4_draw_lines;
}

void fb_gfx_draw_lines(UINT32 *pa_data,UINT32 u4_cnt,UINT32 u4_color,UINT32 u4_addr,UINT32 u4_pitch,UINT32 u4_cm)
{
    UINT32 u4_lp    =0x0;
    UINT32 u4_idx   =0x0;

    GFX_Lock();
    GFX_MMU_Set_Enable(0x0);

    GFX_SetDst((UINT8 *)u4_addr,u4_cm,u4_pitch);

    GFX_SetColor(u4_color);

    //printk("fb_gfx_draw_lines[%d,0x%x,0x%x,%d,%d]\n",u4_cnt,u4_color,u4_addr,u4_pitch,u4_cm);

    for(u4_lp=0x0;u4_lp<u4_cnt;u4_lp++)
    {
        u4_idx = 4*u4_lp;
        if(pa_data[u4_idx] == pa_data[u4_idx+2])
        {
            //printk("GFX_VLine[%d,%d,%d]\n",pa_data[u4_idx],pa_data[u4_idx+1],(pa_data[u4_idx+3]-pa_data[u4_idx+1]));
            GFX_VLine(pa_data[u4_idx],pa_data[u4_idx+1],(pa_data[u4_idx+3]-pa_data[u4_idx+1]));
        }
        else if(pa_data[u4_idx+1] == pa_data[u4_idx+3])
        {
            //printk("GFX_HLine[%d,%d,%d]\n",pa_data[u4_idx],pa_data[u4_idx+2],(pa_data[u4_idx+2]-pa_data[u4_idx]));
            GFX_HLine(pa_data[u4_idx],pa_data[u4_idx+2],(pa_data[u4_idx+2]-pa_data[u4_idx]));
        }
    }

    GFX_Flush();
    GFX_Task_Wait_Sync();
    GFX_Unlock();
}

static INT32 fb_osd_set_bmp_addr(struct mt53fb_set* pt_set,UINT32 u4_plane)
{
    INT32          i4_ret          = (INT32) OSD_RET_OK;
    UINT32         u4_update_rgn   = 0x0;
    FB_OSD_RGN_T   t_left;
    FB_OSD_RGN_T   t_rght;
    FB_OSD_RGN_T*  pt_left=&t_left;
    FB_OSD_RGN_T*  pt_rght=&t_rght;
    FB_OSD_RGN_T*  pt_cur_rgn=NULL;

    #if 0 // def CC_LINUX_GPU_SUPPORT
   if(u4_plane == OSD_PLANE_3)
    {
      printk("osd 3 is used by GPU, so dfb can not use it\n");
      return OSD_RET_OK;
    }
#endif
    fb_osd_get_rgn_obj(u4_plane,E_FB_OSD_RGN_LEFT,pt_left);
    fb_osd_get_rgn_obj(u4_plane,E_FB_OSD_RGN_RIGHT,pt_rght);
    
    OSD_PLA_WaitStable(u4_plane);

    if(FB_OSD_IS_SET_BMP_ADDR_NORMAL(pt_left,pt_rght))
    {
        if( FB_OSD_IS_IDLE_INIT_STATE(pt_left,pt_rght)||
            FB_OSD_IS_UNCMP_INIT_STATE(pt_left,pt_rght))
        {
            FB_OSD_SET_RGN(OSD_RGN_DEC_EN,0x0);
            FB_OSD_SET_RGN_FLIP_MIRROR(u4_plane);
        }

        if(FB_RGN_IS_IDLE_STATE(pt_left))
        {
            FB_RGN_SET_DISP_STATE(pt_left);
            FB_RGN_SET_IDLE_STATE(pt_rght);
            pt_cur_rgn  = pt_left;
        }
        else
        {
            FB_RGN_SET_DISP_STATE(pt_rght);
            FB_RGN_SET_IDLE_STATE(pt_left);
            pt_cur_rgn  = pt_rght;
        }

        OSD_RGN_Set(pt_cur_rgn->u4_rgn, OSD_RGN_BMP_ADDR,pt_set->rSetting.u4Base);

        if(OSD_GetCpblty(E_OSD_CPBLTY_3D_STATUS,u4_plane))
        {
            fb_osd_sbs_action(pt_cur_rgn, u4_plane,pt_set->rSetting.u4Base);
        }

        fb_set_plane_enable(u4_plane,pt_cur_rgn->u4_list,TRUE);
        u4_update_rgn = TRUE;
    }
    else if(FB_OSD_IS_SET_BMP_ADDR_CMP(pt_left,pt_rght))
    {
        if(FB_OSD_IS_CMP_INIT_STATE(pt_left,pt_rght))
        {
            FB_OSD_SET_RGN(OSD_RGN_DEC_EN,0x1);
            FB_OSD_SET_RGN_FLIP_MIRROR(u4_plane);
            OSD_RGN_Set(pt_left->u4_rgn, OSD_RGN_BMP_ADDR,pt_left->u4_addr);
            OSD_RGN_Set(pt_rght->u4_rgn, OSD_RGN_BMP_ADDR,pt_rght->u4_addr);
        }

        if(FB_RGN_IS_CMP_STATE(pt_left))
        {
            FB_RGN_SET_CMP_DISP_STATE(pt_left);
            FB_RGN_SET_CMP_STATE(pt_rght);
            pt_cur_rgn  = pt_left;
        }
        else
        {
            FB_RGN_SET_CMP_DISP_STATE(pt_rght);
            FB_RGN_SET_CMP_STATE(pt_left);
            pt_cur_rgn  = pt_rght;
        }

        if(OSD_GetCpblty(E_OSD_CPBLTY_3D_STATUS,u4_plane))
        {
            fb_osd_sbs_action(pt_cur_rgn, u4_plane,pt_set->rSetting.u4Base);
        }

        fb_gfx_compress(pt_set,pt_cur_rgn);

        fb_set_plane_enable(u4_plane,pt_cur_rgn->u4_list,TRUE);

        if(OSD_DMA_Check_Plane(u4_plane))
        {
            OSD_DMA_TransferRegion(u4_plane);
        }

        u4_update_rgn = TRUE;
    }
    else
    {
        i4_ret = (INT32) OSD_RET_INV_ARG;
    }

    if(u4_update_rgn)
    {
        fb_osd_set_rgn_obj(u4_plane,E_FB_OSD_RGN_LEFT,pt_left);
        fb_osd_set_rgn_obj(u4_plane,E_FB_OSD_RGN_RIGHT,pt_rght);
    }

    #if defined(CC_ENABLE_GRAPHIC_DUMP)
    GFX_AnalyOSD(u4_plane);
    #endif

    return i4_ret;
}

INT32 fb_osd_reupdate_bmp_addr(UINT32 u4_plane)
{
    return fb_osd_set_bmp_addr(&t_fb_set[u4_plane],u4_plane);
}

static INT32 fb_set_plane_enable(UINT32 u4_plane,UINT32 u4_rgn_list,UINT32 u4_enable)
{
    if(u4_enable)
    {
        OSD_PLA_Flip_RgnList(u4_plane, u4_rgn_list);
    }
    else
    {
        OSD_PLA_Enable(u4_plane,FALSE);
    }

    return OSD_RET_OK;
}

static UINT32 fb_osd_get_idle_rgn_list(UINT32 u4_plane)
{
    UINT32 u4_rgn_list = OSD_INVALID_VALUE_U32;
    FB_OSD_RGN_T t_left;
    FB_OSD_RGN_T t_rght;

    fb_osd_get_rgn_obj(u4_plane,E_FB_OSD_RGN_LEFT,&t_left);
    fb_osd_get_rgn_obj(u4_plane,E_FB_OSD_RGN_RIGHT,&t_rght);

    if(FB_RGN_IS_IDLE_STATE(&t_left))
    {
        u4_rgn_list = t_left.u4_list;
    }
    else if(FB_RGN_IS_IDLE_STATE(&t_rght))
    {
        u4_rgn_list = t_rght.u4_list;
    }

    return u4_rgn_list;
}

static INT32 fb_osd_get_rgn_obj(UINT32 u4_plane,UINT32 u4_rgn,FB_OSD_RGN_T* pt_rgn_list)
{
    CHECK_FB_OSD_RGN_LIST_PARA(u4_plane,u4_rgn,pt_rgn_list);

    memcpy(pt_rgn_list,&au4_plane_rgn[u4_plane][u4_rgn], sizeof(FB_OSD_RGN_T));

    return OSD_RET_OK;
}

static void fb_osd_rgn_obj_init(void)
{
    memset(&au4_plane_rgn, 0, sizeof(au4_plane_rgn));

    return ;
}

static UINT32 fb_osd_get_plane(UINT32 ui4_dfb_layer_id)
{
    UINT32 ui4_pid = OSD_INVALID_VALUE_U32;

#ifdef ANDROID_TWO_WORLDS
    switch (ui4_dfb_layer_id)
    {
    case MT53_LAYER_OSD1:
        ui4_pid = OSD_PLANE_1;
        break;
    case MT53_LAYER_OSD2:
        ui4_pid = OSD_PLANE_2;
        break;
    case MT53_LAYER_OSD3:
        ui4_pid = OSD_PLANE_3;
        break;
#ifdef OSD_PLA5_SUPPORT
    case MT53_LAYER_CURSOR:
        ui4_pid = OSD_PLANE_5;
        break;
#endif
    default:
        break;
    }
    
#else

    switch (ui4_dfb_layer_id)
    {
    case MT53_LAYER_OSD1:
        ui4_pid = OSD_PLANE_2;
        break;
    case MT53_LAYER_OSD2:
        ui4_pid = OSD_PLANE_3;
        break;
    case MT53_LAYER_OSD3:
        ui4_pid = OSD_PLANE_1;
        break;
#ifdef OSD_PLA5_SUPPORT
    case MT53_LAYER_CURSOR:
        ui4_pid = OSD_PLANE_5;
        break;
#endif
    default:
        break;
    }
#endif

    return ui4_pid;
}

UINT32 fb_osd_get_buffer_mode(UINT32 u4_plane)
{
    return t_fb_set[u4_plane].rSetting.u4BufferMode;
}

UINT32 fb_osd_get_dfb_addr(void)
{
    return GFX_GetDirectFBMem_Addr();
}

UINT32 fb_osd_get_dfb_size(void)
{
    return GFX_GetDirectFBMem_Size();
}

int mtk_fb_get_property(unsigned int* addr, unsigned int* size);
UINT32 fb_osd_get_fb_addr(void)
{
#if 1
	unsigned int fb_addr, fb_size;
	mtk_fb_get_property(&fb_addr, &fb_size);
	return fb_addr;
#else
    return (BSP_GetFbmMemAddr() - DRVCUST_OptGet(eFBMemSize) - DRVCUST_OptGet(eDirectFBMemSize));
#endif
}

UINT32 fb_osd_get_fb_size(void)
{
#if 1
	unsigned int fb_addr, fb_size;
	mtk_fb_get_property(&fb_addr, &fb_size);
	return fb_size;
#else
	return DRVCUST_OptGet(eFBMemSize);
#endif
}

static UINT32 mt53fb_get_osd_cm(UINT32 u4_cm)
{
    UINT32 u4ColorMode = OSD_CM_AYCBCR8888_DIRECT32;

    if (u4_cm == CM_ARGB8888_DIRECT32)
    {
        u4ColorMode = OSD_CM_ARGB8888_DIRECT32;
    }
    else if (u4_cm == CM_ARGB4444_DIRECT16)
    {
        u4ColorMode = OSD_CM_ARGB4444_DIRECT16;
    }
    else if (u4_cm == CM_RGB565_DIRECT16)
    {
        u4ColorMode = OSD_CM_RGB565_DIRECT16;
    }
    else if (u4_cm == CM_RGB_CLUT8)
    {
        u4ColorMode = OSD_CM_RGB_CLUT8;
    }
    else if (u4_cm == CM_AYCbCr8888_DIRECT32)
    {
        u4ColorMode = OSD_CM_AYCBCR8888_DIRECT32;
    }
    else
    {
        printk("mt53fb_get_osd_cm Fail ! \n");
    }

    return u4ColorMode;
}

static INT32 fb_gfx_compress(  struct mt53fb_set* pt_set,FB_OSD_RGN_T*  pt_rgn)
{
    UINT32 u4ColorMode          = 0x0;
    UINT32 u4_cmprss_pitch      = 0x0;
    UINT32 u4_cmprss_hight      = 0x0;
    UINT32 u4_cmprss_width      = 0x0;
    struct mt53fb_compress     rCompress;
    int i4Ret                   = 0x0;
    UINT32 u4_dma_mode          = 0x0;
    UINT32 u4_dma_adj_w         = 0x8;
    UINT32 u4_plane             = fb_osd_get_plane(pt_set->rSetting.u4OsdPlaneID);

    if(OSD_DMA_Check_Plane(u4_plane))
    {
        u4_dma_mode = TRUE;
    }

    VERIFY(OSD_RGN_Get(pt_rgn->u4_rgn,OSD_RGN_COLORMODE,&u4ColorMode) == OSD_RET_OK);
    VERIFY(OSD_RGN_Get(pt_rgn->u4_rgn,OSD_RGN_BMP_PITCH,&u4_cmprss_pitch) == OSD_RET_OK);
    VERIFY(OSD_RGN_Get(pt_rgn->u4_rgn,OSD_RGN_BMP_W,&u4_cmprss_width) == OSD_RET_OK);
    VERIFY(OSD_RGN_Get(pt_rgn->u4_rgn,OSD_RGN_BMP_H,&u4_cmprss_hight) == OSD_RET_OK);
    OSD_RGN_Set(pt_rgn->u4_rgn, OSD_RGN_BMP_ADDR,pt_rgn->u4_addr);

    rCompress.eCm       = u4ColorMode;
    rCompress.u4Src     = pt_set->rSetting.u4Base;
    rCompress.u4DstX    = 0;
    rCompress.u4DstY    = 0;
    rCompress.u4SrcX    = 0;
    rCompress.u4SrcY    = 0;
    rCompress.u4Width   = (unsigned int)(u4_cmprss_pitch/GFX_Get_Cm_Size(rCompress.eCm));
    rCompress.u4Height  = u4_cmprss_hight;
    rCompress.u4Pitch   = u4_cmprss_pitch;

    if(OSD_GetCpblty(E_OSD_CPBLTY_SBS_PLANE,u4_plane))
    {
        rCompress.u4Src = pt_rgn->u4_3d_addr;
    }
    else
    {
        rCompress.u4Src = pt_set->rSetting.u4Base;
    }

    rCompress.u4Dst = pt_rgn->u4_addr;

    rCompress.u4_blt_fg = 0x0;
    if(OSD_Base_Is_Gfx_Do_Flip_Mirror(u4_plane))
    {
        if(OSD_GetCpblty(E_OSD_CPBLTY_FLIP, 0x0))
        {
            rCompress.u4_blt_fg |= D_GFXFLAG_FLIP;
        }

        if(OSD_GetCpblty(E_OSD_CPBLTY_MIRROR,0x0))
        {
            rCompress.u4_blt_fg |= D_GFXFLAG_MIRROR;
        }
    }

    rCompress.mmu = pt_set->rSetting.mmu;
    if(u4_dma_mode)
    {
        rCompress.u4Width = ((u4_cmprss_width/2) + u4_dma_adj_w);
    }

    i4Ret = fb_gfx_cmps_blt(NULL, &rCompress);

    if(u4_dma_mode)
    {
        rCompress.u4Src = (pt_set->rSetting.u4Base +
        (((u4_cmprss_width/2) - u4_dma_adj_w)*GFX_Get_Cm_Size(rCompress.eCm)));
        rCompress.u4Dst = rCompress.u4Dst+(rCompress.u4Pitch*rCompress.u4Height/2);
        i4Ret = fb_gfx_cmps_blt(NULL, &rCompress);
    }

    return i4Ret;
}


#ifdef CC_B2R44K2K_SUPPORT
static int _mt53fb_4k2k_set(struct mt53fb_set * pt_set)
{   
    if (pt_set->mask & MT53FB_SET_MASK_BASE)
    {
        _mt53_fb_b2r_flip(pt_set->rSetting.u4Base);
    }

    return 0;
}
#endif

static int _mt53fb_set_ioctl(void* pvArg)
{
    int                 i4Ret           = 0x0;
    UINT32              u4_plane        = 0x0;
    UINT32              u4_panel_w      = 0x0;
    UINT32              u4_panel_h      = 0x0;
    UINT32              u4_scaler       = 0x0;
    UINT32              u4ColorMode     = 0x0;
    struct mt53fb_set  rSet;
    FB_OSD_RGN_T        t_left;
    FB_OSD_RGN_T        t_rght;
    OSD_SCALER_DATA_T   t_rgn_list_sc;
    OSD_SCALER_DATA_T*  pt_sc_data = NULL ;

#ifdef CC_LOADER_LOGO_LONG_TIME
    static BOOL fgFirst[] = {TRUE, TRUE, TRUE};
#endif

    if (copy_from_user(&rSet, pvArg, sizeof(struct mt53fb_set)))
    {
        printk("_mt53fb_set_ioctl() - copy_from_user() fail\n");
        return -EFAULT;
    }
#ifdef CC_B2R44K2K_SUPPORT
    if(rSet.rSetting.u4OsdPlaneID == MT53_LAYER_VDP1)
    {
        return _mt53fb_4k2k_set(&rSet);
    }   
#endif    
    
    u4_plane        = fb_osd_get_plane(rSet.rSetting.u4OsdPlaneID);
    if(OSD_INVALID_VALUE_U32 == u4_plane)
    {
        printk("_mt53fb_set_ioctl() - plane error !\n");
        return -EFAULT;
    }

#ifdef CC_LOADER_LOGO_LONG_TIME
	if(u4_plane == OSD_PLANE_1 || u4_plane == OSD_PLANE_2 || u4_plane == OSD_PLANE_3)
	{
	    if(fgFirst[u4_plane] == TRUE)
	    {
	        fgFirst[u4_plane] = FALSE;
			OSD_PLA_DisableLoaderLogo(u4_plane);
	    }
	}
#endif	

    u4_panel_w      = OSD_BASE_GetDisplayWidth();
    u4_panel_h      = OSD_BASE_GetDisplayHeight();
    u4_scaler   = OSD_PLANE_TO_SCALER(u4_plane);
    fb_osd_get_rgn_obj(u4_plane,E_FB_OSD_RGN_LEFT,&t_left);
    fb_osd_get_rgn_obj(u4_plane,E_FB_OSD_RGN_RIGHT,&t_rght);
    


    if(rSet.mask & MT53FB_SET_MASK_WAIT_VSYNC)
    {
        OSD_PLA_WaitStable(u4_plane);
    }

    if (rSet.mask & MT53FB_SET_MASK_X)
    {
        FB_OSD_SET_RGN(OSD_RGN_POS_X, rSet.rSetting.u2X);
    }

    if (rSet.mask & MT53FB_SET_MASK_Y)
    {
        FB_OSD_SET_RGN(OSD_RGN_POS_Y, rSet.rSetting.u2Y);
    }

    if(rSet.mask & MT53FB_SET_MASK_W)
    {
        FB_OSD_SET_RGN(OSD_RGN_BMP_W, rSet.rSetting.u2W);
        FB_OSD_SET_RGN(OSD_RGN_DISP_W,rSet.rSetting.u2W);
    }

    if (rSet.mask & MT53FB_SET_MASK_H)
    {
        FB_OSD_SET_RGN(OSD_RGN_BMP_H, rSet.rSetting.u2H);
        FB_OSD_SET_RGN(OSD_RGN_DISP_H,rSet.rSetting.u2H);
    }

    if (rSet.mask & MT53FB_SET_MASK_H && rSet.mask & MT53FB_SET_MASK_W)
    {
        if(u4_scaler < OSD_SCALER_MAX_NUM && _scaler_status[u4_scaler] == FALSE)
        {
            t_rgn_list_sc.u4SrcWidth    = u4_panel_w;
            t_rgn_list_sc.u4SrcHeight   = u4_panel_h;      
        }
        else
        {
            t_rgn_list_sc.u4SrcWidth    = rSet.rSetting.u2W;
            t_rgn_list_sc.u4SrcHeight   = rSet.rSetting.u2H;

        }
        t_rgn_list_sc.u4DstWidth    = u4_panel_w;
        t_rgn_list_sc.u4DstHeight   = u4_panel_h;        

        OSD_RGN_LIST_SCALER_Set(t_left.u4_list,&t_rgn_list_sc);
        OSD_RGN_LIST_SCALER_Set(t_rght.u4_list,&t_rgn_list_sc);

        OSD_SC_Scale(u4_scaler,
                        0x1,
                        t_rgn_list_sc.u4SrcWidth ,
                        t_rgn_list_sc.u4SrcHeight,
                        t_rgn_list_sc.u4DstWidth,
                        t_rgn_list_sc.u4DstHeight);
    }
    else
    {
        pt_sc_data = OSD_RGN_LIST_SCALER_Get(t_left.u4_list);
        if(pt_sc_data)
        {
            if(u4_scaler < OSD_SCALER_MAX_NUM && _scaler_status[u4_scaler] == FALSE)
            {
                t_rgn_list_sc.u4SrcWidth    = u4_panel_w;
                t_rgn_list_sc.u4SrcHeight   = u4_panel_h;      
            }
            else
            {
                t_rgn_list_sc.u4SrcWidth    = pt_sc_data->u4SrcWidth;
                t_rgn_list_sc.u4SrcHeight   = pt_sc_data->u4SrcHeight;
            
            }
            t_rgn_list_sc.u4DstWidth    = u4_panel_w;
            t_rgn_list_sc.u4DstHeight   = u4_panel_h; 

            OSD_RGN_LIST_SCALER_Set(t_left.u4_list,&t_rgn_list_sc);
        }
        pt_sc_data = OSD_RGN_LIST_SCALER_Get(t_rght.u4_list);
        if(pt_sc_data)
        {
            if(u4_scaler < OSD_SCALER_MAX_NUM && _scaler_status[u4_scaler] == FALSE)
            {
                t_rgn_list_sc.u4SrcWidth    = u4_panel_w;
                t_rgn_list_sc.u4SrcHeight   = u4_panel_h;      
            }
            else
            {
                t_rgn_list_sc.u4SrcWidth    = pt_sc_data->u4SrcWidth;
                t_rgn_list_sc.u4SrcHeight   = pt_sc_data->u4SrcHeight;
            
            }
            t_rgn_list_sc.u4DstWidth    = u4_panel_w;
            t_rgn_list_sc.u4DstHeight   = u4_panel_h; 

            OSD_RGN_LIST_SCALER_Set(t_rght.u4_list,&t_rgn_list_sc);
        }
    }

    if (rSet.mask & MT53FB_SET_MASK_CM)
    {
        u4ColorMode = mt53fb_get_osd_cm(rSet.rSetting.u1CM);
        FB_OSD_SET_RGN(OSD_RGN_COLORMODE,u4ColorMode);
    }

    if (rSet.mask & MT53FB_SET_MASK_PITCH)
    {
        FB_OSD_SET_RGN(OSD_RGN_BMP_PITCH,rSet.rSetting.u4Pitch);
    }

    if (rSet.mask & MT53FB_SET_MASK_COLORKEY)
    {
        FB_OSD_SET_RGN(OSD_RGN_COLOR_KEY,rSet.rSetting.u4ColorKey);
    }

    if (rSet.mask & MT53FB_SET_MASK_OPACITY)
    {
        VERIFY(OSD_PLA_SetFading(u4_plane,rSet.rSetting.u4Opacity)==OSD_RET_OK);
    }

    if (rSet.mask & MT53FB_SET_MASK_MIXSEL)
    {
        FB_OSD_SET_RGN(OSD_RGN_MIX_SEL,rSet.rSetting.u4MixSel);
    }

    if (rSet.mask & MT53FB_SET_MASK_COLORKEYEN)
    {
        FB_OSD_SET_RGN(OSD_RGN_COLOR_KEY_EN,rSet.rSetting.u4ColorKeyEn);
    }

    if (rSet.mask & MT53FB_SET_MASK_COLORKEY)
    {
        FB_OSD_SET_RGN(OSD_RGN_COLOR_KEY,rSet.rSetting.u4ColorKey);
    }

    if (rSet.mask & MT53FB_SET_MASK_BASE)
    {
        fb_osd_set_bmp_addr(&rSet,u4_plane);
        t_fb_set[u4_plane].rSetting.u4Base = rSet.rSetting.u4Base;
    }

    if(rSet.mask & MT53FB_SET_MASK_VISIBLE)
    {
        OSD_PLA_Enable(u4_plane,rSet.rSetting.fgVisible);
        x_memcpy(&t_fb_set[u4_plane],&rSet,sizeof(struct mt53fb_set));
    }

    return i4Ret;
}

INT32 mtfb_compress_enable(UINT32 u4_plane, UINT32 u4_enable)
{
    int            i4Ret = (INT32) OSD_RET_OK;
    FB_OSD_RGN_T   t_left;
    FB_OSD_RGN_T   t_rght;

    fb_osd_get_rgn_obj(u4_plane,E_FB_OSD_RGN_LEFT,&t_left);
    fb_osd_get_rgn_obj(u4_plane,E_FB_OSD_RGN_RIGHT,&t_rght);

    if(u4_enable)
    {
        if(FB_OSD_IS_CMP_STATE(&t_left,&t_rght))
        {
            return i4Ret;
        }

        FB_OSD_SET_CMP_STATE(&t_left,&t_rght);
    }
    else
    {
        if(FB_OSD_IS_DISP_STATE(&t_left,&t_rght))
        {
            return i4Ret;
        }

        FB_OSD_SET_UNCMP_STATE(&t_left,&t_rght);
    }

    fb_osd_set_rgn_obj(u4_plane,E_FB_OSD_RGN_LEFT,&t_left);
    fb_osd_set_rgn_obj(u4_plane,E_FB_OSD_RGN_RIGHT,&t_rght);

    return i4Ret;
}

INT32 mtfb_get_cmp_state(UINT32 u4_plane)
{
    FB_OSD_RGN_T   t_left;
    FB_OSD_RGN_T   t_rght;

    fb_osd_get_rgn_obj(u4_plane,E_FB_OSD_RGN_LEFT,&t_left);
    fb_osd_get_rgn_obj(u4_plane,E_FB_OSD_RGN_RIGHT,&t_rght);

    return (INT32)FB_OSD_IS_CMP_DISP_STATE(&t_left,&t_rght);
}

INT32 mtfb_borrow_cmp_buf(UINT32 u4_plane,UINT32 u4_plane_cmp)
{
    int            i4Ret = (INT32) OSD_RET_OK;
    FB_OSD_RGN_T   t_left;
    FB_OSD_RGN_T   t_rght;
    FB_OSD_RGN_T   t_left_cmp;
    FB_OSD_RGN_T   t_rght_cmp;

    if(!OSD_GetCpblty(E_OSD_CPBLTY_CMPRSS_PLANE,u4_plane_cmp))
    {
        return -EFAULT;
    }

    fb_osd_get_rgn_obj(u4_plane,E_FB_OSD_RGN_LEFT,&t_left);
    fb_osd_get_rgn_obj(u4_plane,E_FB_OSD_RGN_RIGHT,&t_rght);
    fb_osd_get_rgn_obj(u4_plane_cmp,E_FB_OSD_RGN_LEFT,&t_left_cmp);
    fb_osd_get_rgn_obj(u4_plane_cmp,E_FB_OSD_RGN_RIGHT,&t_rght_cmp);

    t_left.u4_addr = t_left_cmp.u4_addr;
    t_rght.u4_addr = t_rght_cmp.u4_addr;

    fb_osd_set_rgn_obj(u4_plane,E_FB_OSD_RGN_LEFT,&t_left);
    fb_osd_set_rgn_obj(u4_plane,E_FB_OSD_RGN_RIGHT,&t_rght);

    return i4Ret;
}


static int _mt53fb_get_imagebuffer_ioctl(void* pvArg)
{
    struct mt53fb_imagebuffer rBuf;
    rBuf.u4Size = _rImgBufInfo.u4Size;
    rBuf.u4PhyAddr = _rImgBufInfo.u4PhyAddr;
    rBuf.u4VirtAddr = _rImgBufInfo.u4VirtAddr;
    if (copy_to_user(pvArg, (void*) &rBuf, sizeof(struct mt53fb_imagebuffer)))
    {
        //printk(KERN_WARNING "_mt53fb_get_imagebuffer_ioctl() - copy_to_user() fail\n");
        return -EFAULT;
    }
    return 0;
}

#ifdef CC_FBM_TWO_FBP_SHARED_WITH_DFB
static int _mt53fb_get_imagebuffer2_ioctl(void* pvArg)
{
    struct mt53fb_imagebuffer rBuf;
    rBuf.u4Size = _rImgBufInfo2.u4Size;
    rBuf.u4PhyAddr = _rImgBufInfo2.u4PhyAddr;
    rBuf.u4VirtAddr = _rImgBufInfo2.u4VirtAddr;
    if(copy_to_user(pvArg, (void*)&rBuf, sizeof(struct mt53fb_imagebuffer)))
    {
        //printk(KERN_WARNING "_mt53fb_get_imagebuffer_ioctl() - copy_to_user() fail\n");
        return -EFAULT;
    }
    return 0;
}
#endif /* CC_FBM_TWO_FBP_SHARED_WITH_DFB */
#ifdef CC_DFB_SUPPORT_VDP_LAYER
static int _mt53fb_get_vdpbuffer_ioctl(void* pvArg)
{
    struct mt53fb_imagebuffer rBuf;
    rBuf.u4Size = _rVdpBufInfo.u4Size;
    rBuf.u4PhyAddr = _rVdpBufInfo.u4PhyAddr;
    rBuf.u4VirtAddr = _rVdpBufInfo.u4VirtAddr;
    if(copy_to_user(pvArg, (void*)&rBuf, sizeof(struct mt53fb_imagebuffer)))
    {
        //printk(KERN_WARNING "_mt53fb_get_imagebuffer_ioctl() - copy_to_user() fail\n");
        return -EFAULT;
    }
    return 0;
}

static int _mt53fb_get_vdp_crop_rect_ioctl(void* pvArg)
{
    mt53fb_vdp_crop_rect t_s_rect;

    if(copy_from_user(&t_s_rect, pvArg, sizeof(mt53fb_vdp_crop_rect)))
    {
        return -EFAULT;
    }

    if(t_s_rect.ui4_layer > DFB_VDP_LAYER_MAX )
    {
        return -EFAULT;
    }

    if(copy_to_user(pvArg,&mt53fb_vdp_crop_rect_array[t_s_rect.ui4_layer],
                    sizeof(mt53fb_vdp_crop_rect)))
    {
        return -EFAULT;
    }

    return 0;
}

static int _mt53fb_set_vdp_crop_rect_ioctl(void* pvArg)
{
    mt53fb_vdp_crop_rect t_s_rect;

    if(copy_from_user(&t_s_rect, pvArg, sizeof(mt53fb_vdp_crop_rect)))
    {
        return -EFAULT;
    }

    if(t_s_rect.ui4_layer < DFB_VDP_LAYER_MAX )
    {
        memcpy(&mt53fb_vdp_crop_rect_array[t_s_rect.ui4_layer], &t_s_rect, sizeof(mt53fb_vdp_crop_rect));
    }
    else
    {
        return -EFAULT;
    }

    // Set 3D crop offset
    _u4CropBottomOffset = t_s_rect.crop_bottom_offset;
    _u4CropTopOffset = t_s_rect.crop_top_offset;
    _u4CropLeftOffset = t_s_rect.crop_left_offset;
    _u4CropRightOffset = t_s_rect.crop_right_offset;

    if(MT53FB_LOG>=0x2)
    {
        printk("Crop Rect[%d,%d,%d,%d]\n",
                _u4CropLeftOffset,_u4CropTopOffset,
                _u4CropRightOffset,_u4CropBottomOffset);
    }

    return 0;
}

static INT32 _mt53fb_vdp_stretch_blit_ioctl(void* pvArg)
{
    mt53fb_vdp_stretch_blit rVdpStretchBlit;

    if(copy_from_user((void*)&rVdpStretchBlit, pvArg, sizeof(mt53fb_vdp_stretch_blit)))
    {
        printk(KERN_WARNING "_mt53fb_vdp_stretch_blit_ioctl() error: Copy from user error!\n");
        return -EFAULT;
    }

    return mt53fb_vdp_stretchblit(&rVdpStretchBlit);
}
#endif

        //printk(KERN_WARNING "_mt53fb_get_imagebuffer_ioctl() - copy_to_user() fail\n");


#ifdef CC_B2R44K2K_SUPPORT  
static INT32 _mt53fb_b2r_stretch_blit_ioctl(void* pvArg)
{
    mt53fb_b2r_stretch_blit rB2rStretchBlit;

    if(copy_from_user((void*)&rB2rStretchBlit, pvArg, sizeof(mt53fb_b2r_stretch_blit)))
    {
        printk(KERN_WARNING "_mt53fb_b2r_stretch_blit_ioctl() error: Copy from user error!\n");
        return -EFAULT;
    }

    return mt53fb_b2r_stretchblit(&rB2rStretchBlit);
}
#endif


static int _mt53fb_get_plane_order_array_ioctl(void* pvArg)
{
    UINT32 i;
#ifdef LINUX_TURNKEY_SOLUTION
    UINT32 au4PlaneOrderArray[PMX_MAX_INPORT_NS];

    for(i = 0; i < PMX_MAX_INPORT_NS; i++)
    {

        au4PlaneOrderArray[i] = _au4PlaneOrderArray[i];
    }

    if(copy_to_user(pvArg, (void*)au4PlaneOrderArray, sizeof(UINT32) * PMX_MAX_INPORT_NS))
    {
        //printk(KERN_WARNING "_mt53fb_get_plane_order_array_ioctl() - copy_to_user() fail\n");
        return -EFAULT;
    }
#else
    UINT32 au4PlaneOrderArray[SIZE_PLANE_ORDER_ARRAY];
    for (i = 0; i < SIZE_PLANE_ORDER_ARRAY; i++)
    {
        au4PlaneOrderArray[i] = _au1Hw2LogPlane[_au4PlaneOrderArray[i]];
    }
    if (copy_to_user(pvArg, (void*) au4PlaneOrderArray,
            sizeof(UINT32) * SIZE_PLANE_ORDER_ARRAY))
    {
        printk(KERN_WARNING "_mt53fb_get_plane_order_array_ioctl() - copy_to_user() fail\n");
        return -EFAULT;
    }
#endif
    return 0;
}

static int _mt53fb_set_plane_order_array_ioctl(void* pvArg)
{
    UINT32 i;
#ifdef LINUX_TURNKEY_SOLUTION
    UINT32 au4PlaneOrderArray[PMX_MAX_INPORT_NS];

    if(copy_from_user( (void*)au4PlaneOrderArray, pvArg, sizeof(UINT32) * PMX_MAX_INPORT_NS))
    {
        //printk(KERN_WARNING "_mt53fb_get_plane_order_array_ioctl() - copy_to_user() fail\n");
        return -EFAULT;
    }
    for(i = 0; i < PMX_MAX_INPORT_NS; i++)
    {
        //printk("[%d] User: %d,\n",i, au4PlaneOrderArray[i]);
        _au4PlaneOrderArray[i] = au4PlaneOrderArray[i];
    }
#else
    UINT32 au4PlaneOrderArray[SIZE_PLANE_ORDER_ARRAY];

    if (copy_from_user((void*) au4PlaneOrderArray, pvArg,
            sizeof(UINT32) * SIZE_PLANE_ORDER_ARRAY))
    {
        printk(KERN_WARNING "_mt53fb_get_plane_order_array_ioctl() - copy_to_user() fail\n");
        return -EFAULT;
    }
    for (i = 0; i < SIZE_PLANE_ORDER_ARRAY; i++)
    {
        printk("[%d] User: %d, Kernel %d\n", i, au4PlaneOrderArray[i],
                _au1Log2HwPlane[au4PlaneOrderArray[i]]);
        _au4PlaneOrderArray[i] = _au1Log2HwPlane[au4PlaneOrderArray[i]];
    }
#endif

    if (PMX_SetPlaneOrderArray(_au4PlaneOrderArray) != PMX_SET_OK)
    {
        //printk(KERN_WARNING "_mt53fb_get_plane_order_array_ioctl() - PMX_SetPlaneOrderArray() fail\n");
        return -EINVAL;
    }

    // Enable mixing
    i = IO_READ32(POST_PROC_BASE, 0x5C0);
    IO_WRITE32(POST_PROC_BASE, 0x5C0, i | 0x00020202);

    return 0;
}

static int _mt53fb_set_osd_plane_order_ioctl(void* pvArg)
{
    UINT32 i;
    UINT32 au4PlaneOrderArray[SIZE_PLANE_ORDER_ARRAY];

    _au4PlaneOrderArray[0] = 5;

    if (copy_from_user((void*) au4PlaneOrderArray, pvArg,
            sizeof(UINT32) * SIZE_PLANE_ORDER_ARRAY))
    {
        //printk(KERN_WARNING "_mt53fb_set_osd_plane_order_ioctl() - copy_to_user() fail\n");
        return -EFAULT;
    }

    for (i = 0; i < SIZE_PLANE_ORDER_ARRAY - 1; i++)
    {
        //printk("%d %d\n",au4PlaneOrderArray[i],_au1Log2HwPlane[au4PlaneOrderArray[i]]);
        _au4PlaneOrderArray[i + 1] = _au1Log2HwPlane[au4PlaneOrderArray[i]];
    }

    //printk("plane mixer order: %d %d %d %d\n",_au4PlaneOrderArray[0],_au4PlaneOrderArray[1],_au4PlaneOrderArray[2],_au4PlaneOrderArray[3]);

    if (PMX_SetPlaneOrderArray(_au4PlaneOrderArray) != PMX_SET_OK)
    {
        return -EINVAL;
    }
    return 0;
}

#ifdef LINUX_TURNKEY_SOLUTION
/* MHF Linux - CHUN for called by PMX*/
int _mt53fb_get_plane_order_HW_array(void * pvArg)
{
    //UINT32 i;

    memcpy(pvArg, (void*)_au4PlaneOrderArray, sizeof(UINT32) * PMX_MAX_INPORT_NS);
    /*
     for(i = 0; i < PMX_MAX_INPORT_NS; i++)
     {
     printk("PMX 2 DFB Get ARRAY: %d, %d \n",i, _au4PlaneOrderArray[i]);
     }
     */
    return 0;
}

int _mt53fb_set_plane_order_HW_array(void * pvArg)
{
    //UINT32 i;

    memcpy( (void*)_au4PlaneOrderArray, pvArg, sizeof(UINT32) * PMX_MAX_INPORT_NS);
    /*
     for(i = 0; i < PMX_MAX_INPORT_NS; i++)
     {
     printk("PMX 2 DFB Set ARRAY: %d, %d \n",i, _au4PlaneOrderArray[i]);

     }
     */
    if (PMX_SetPlaneOrderArray(_au4PlaneOrderArray) != PMX_SET_OK)
    {
        return -EINVAL;
    }return 0;
}
/* MHF Linux - CHUN modify end*/

EXTERN UINT8 bSI_DISPLAY_DCLK_TYPE;

UINT32 VDP_SetArg(UCHAR ucVdpId, UCHAR ucArg, INT16 i2Value);
#endif

static int fb_create_rgn_list(UINT32 u4_plane,UINT32 u4_rgn,OSD_RGN_CREATE_T* pt_rgn)
{
    FB_OSD_RGN_T t_rgn_list;

    VERIFY(fb_osd_get_rgn_obj(u4_plane,u4_rgn,&t_rgn_list)== OSD_RET_OK);

    VERIFY(OSD_RGN_LIST_Create(&t_rgn_list.u4_list) == OSD_RET_OK);

    VERIFY(OSD_RGN_Create( &t_rgn_list.u4_rgn, pt_rgn->u4BmpWidth,
                            pt_rgn->u4BmpHeight,pt_rgn->pvBitmap,
                            pt_rgn->eColorMode, pt_rgn->u4BmpPitch,
                            pt_rgn->u4DispX, pt_rgn->u4DispY,
                            pt_rgn->u4DispW, pt_rgn->u4DispH) == OSD_RET_OK);

    // VERIFY(OSD_RGN_Set(u4_rgn, OSD_RGN_PAL_ADDR,(UINT32) _apu4Palette[OSD_PLANE_2]) == 0);

    VERIFY(OSD_RGN_Set(t_rgn_list.u4_rgn, OSD_RGN_BIG_ENDIAN, 0) == OSD_RET_OK);
    VERIFY(OSD_RGN_Set(t_rgn_list.u4_rgn, OSD_RGN_H_MIRROR, pt_rgn->u4Mirror) == OSD_RET_OK);
    VERIFY(OSD_RGN_Set(t_rgn_list.u4_rgn, OSD_RGN_V_FLIP, pt_rgn->u4Flip) == OSD_RET_OK);
    VERIFY(OSD_RGN_Set(t_rgn_list.u4_rgn,(INT32)OSD_RGN_MIX_SEL,(UINT32)OSD_BM_PIXEL)==OSD_RET_OK);
    VERIFY(OSD_RGN_Set(t_rgn_list.u4_rgn, OSD_RGN_DEC_EN, pt_rgn->u4Dec) == 0);
    VERIFY(OSD_RGN_Insert(t_rgn_list.u4_rgn, t_rgn_list.u4_list) == OSD_RET_OK);
    //  VERIFY(OSD_PLA_FlipTo(OSD_PLANE2, t_rgn_list.u4_list) == OSD_RET_OK);

    VERIFY(fb_osd_set_rgn_obj(u4_plane,u4_rgn,&t_rgn_list)== OSD_RET_OK);

    return OSD_RET_OK;
}

static int _mt53fb_init_hw(void* pvArg)
{
    UINT32 u4BytesPP;
    static UINT32 u4ColorMode;
    UINT16 u2PanelW;
    UINT16 u2PanelH;
    OSD_RGN_CREATE_T t_rgn_create;
    UINT32 u4_plane;
    UINT32 u4_rgn_type;
    struct fb_var_screeninfo rFbVar;
    static int single_inited = 0;

    if(single_inited)
    {
        return 0;
    }
    else
    {
        single_inited=TRUE;
    }

    if(pvArg == NULL)
    {
        rFbVar.xres = (osd1_src_width==0)?1280:osd1_src_width;
        rFbVar.yres = (osd1_src_height==0)?720:osd1_src_height;
        rFbVar.xres_virtual = rFbVar.xres;
        rFbVar.yres_virtual = rFbVar.yres;
        rFbVar.xoffset = 0;
        rFbVar.yoffset = 0;
        rFbVar.bits_per_pixel = 32;
        rFbVar.vmode = CM_ARGB8888_DIRECT32;
    }
    else
    {
        if (copy_from_user((void*) &rFbVar, pvArg, sizeof(struct fb_var_screeninfo)))
        {
            printk(KERN_ERR "_mt53fb_init_hw() - copy_to_user() fail\n");
            VERIFY(0);
        }
    }

    if (_mt53fb_check_var(&rFbVar, &_fb_info) != 0)
    {
        printk(KERN_ERR "_mt53fb_check_var() fail\n");
        VERIFY(0);
    }

    _fb_info.var.xres = rFbVar.xres;
    _fb_info.var.yres = rFbVar.yres;
    _fb_info.var.xres_virtual = rFbVar.xres_virtual;
    _fb_info.var.yres_virtual = rFbVar.yres_virtual;
    _fb_info.var.xoffset = rFbVar.xoffset;
    _fb_info.var.yoffset = rFbVar.yoffset;
    _fb_info.var.bits_per_pixel = rFbVar.bits_per_pixel;
    _fb_info.var.vmode = rFbVar.vmode;
    _fb_info.fix.line_length = (rFbVar.xres * rFbVar.bits_per_pixel) / 8;

    u4ColorMode = OSD_CM_ARGB8888_DIRECT32;
    rFbVar.bits_per_pixel = 32;
    u4BytesPP = rFbVar.bits_per_pixel >> 3;
    u2PanelW = OSD_BASE_GetDisplayWidth();
    u2PanelH = OSD_BASE_GetDisplayHeight();

    t_rgn_create.u4BmpWidth  = rFbVar.xres;
    t_rgn_create.u4BmpHeight = rFbVar.yres;
    t_rgn_create.pvBitmap    = (void *)_fb_info.fix.smem_start;
    t_rgn_create.eColorMode  = u4ColorMode;
    t_rgn_create.u4BmpPitch  = (rFbVar.xres_virtual*(rFbVar.bits_per_pixel>>3));
    t_rgn_create.u4DispX     = 0;
    t_rgn_create.u4DispY     = 0;
    t_rgn_create.u4DispW     = rFbVar.xres;
    t_rgn_create.u4DispH     = rFbVar.yres;
    t_rgn_create.u4Flip      = 0x0;
    t_rgn_create.u4Mirror    = 0x0;
    t_rgn_create.u4Dec       = 0x0;

    for(u4_plane=0x0; u4_plane < OSD_PLANE_MAX_NUM; u4_plane++)
    {
        for(u4_rgn_type=0x0; u4_rgn_type < E_FB_OSD_RGN_MAX; u4_rgn_type++)
        {
            fb_create_rgn_list(u4_plane,u4_rgn_type,&t_rgn_create);
        }

        if(OSD_GetCpblty(E_OSD_CPBLTY_CMPRSS_PLANE,u4_plane))
        {
            mtfb_compress_enable(u4_plane,0x1);
        }
    }

    printk("End init hw for FB\n");

    return 0;
}

//---------------------------------------------------------------------------
// Public interfaces
//---------------------------------------------------------------------------
//#ifdef LINUX_TURNKEY_SOLUTION /* MHF Linux - yjg 9x mmp */
unsigned int fb_convert_app_to_phy(unsigned int usr_addr)
{
    unsigned int kn_addr;

    kn_addr = usr_addr - (unsigned int) fb_user_virt_start
            + (unsigned int) _fb_info.fix.smem_start;
    return kn_addr;
}
//#endif

struct fb_info* mt53fb_get_fb_info(void)
{
    return &_fb_info;
}

struct mt53fb_par* mt53fb_get_par(void)
{
    return &_cur_par;
}

int __init _mt53fb_setup(char *options)
{
    struct options *optp;
    char *opt;

    _mt53fb_init_fbinfo();

    if (!options || !*options)
    {
        return 0;
    }

    while ((opt = strsep(&options, ",")) != NULL)
    {
        if (!*opt)
        {
            continue;
        }

        for (optp = _mt53fb_opt_table; optp->name; optp++)
        {
            int optlen;

            optlen = strlen(optp->name);

            if (strncmp(opt, optp->name, optlen) == 0 && opt[optlen] == ':')
            {
                optp->parse(opt + optlen + 1);
                break;
            }
        }

        if (!optp->name)
        {
            //printk(KERN_ERR "_mt53fb_setup(): unknown parameter: %s\n", opt);
        }
    }
    return 0;
}


int __init _mt53fb_init(void)
{
    fb_osd_init();
#if defined(ANDROID) || defined(ANDROID_TWO_WORLDS)
    mt53simple_init();
#endif
#ifdef CC_SUPPORT_DYNAMIC_ENABLE_DFB_INPUTEVENT
    dfb_init();
#endif


#if defined(LINUX_TURNKEY_SOLUTION) &&(defined(CC_LINUX_GPU_SUPPORT) || ( !defined(CC_SUPPORT_MULTIPLE_FBDEV)))
    drv_extmjc_init();
#ifndef TWO_OSD_PLANE
    GPIO_Init();
    EEPROM_Init();
    FBM_Init();
    LoadPanelIndex();
    PDWNC_Init();
    VDEC_Init(); //???
    VDP_Init();
    VdoMLInit();
    SIF_Init();
#ifndef ANDROID
    /*comment here with ifndef ANDROID for video out issue*/
    VDP_SetScalerPixelBased(1);
#endif /* ANDROID */
    RTC_Init();

    vApiPanelPowerSequence(1);
#endif /* TWO_OSD_PLANE */
    PMX_Init();
#ifdef CC_LOADER_LOGO_LONG_TIME
    OSD_SwInit();
    GFX_Init();
    fb_osd_rgn_obj_init();
    
    VERIFY(OSD_SwReset() == 0);
#else
    OSD_Init();
    GFX_Init();
    fb_osd_rgn_obj_init();

    VERIFY(OSD_Reset() == 0);
#endif

#ifndef ANDROID
    _mt53fb_probe(&_mt53fb_dev);
    _mt53fb_init_hw(NULL);
#endif
    // No DFB in android, so we disable OSD it

#ifdef FB_HARD_FADING
    /*  CHUN: In phase 2, this is needed for that appliaction which does not specify FADING FIXME */
    OSD_PLA_SetFading(1, 0xff);
    /*OSD_PLA_SetFading(2, 0xe5);*/
#endif /* FB_HARD_FADING */
    PMX_SetPlaneOrderArray(_au4PlaneOrderArray);

#if defined(ANDROID) || defined(CC_LINUX_GPU_SUPPORT)
    {
        int ret;
        ret = mtk_drv_fb_init(&drv);
        if(ret)
        return ret;
        mtk_fb_setup();
    }
#endif /* ANDROID */

#ifdef ANDROID_TWO_WORLDS
    _mt53fb_probe(&_mt53fb_dev);
#endif
    // Set GPU clock
#ifdef CC_MT5396
    IO_WRITE32(0xF000D000, 0x314, 0xD);
    ////#elif defined(IS_IC_5860)//for python mali clock
    ////     IO_WRITE32(0xF000D000, 0x3d4, 0x1);   //Python Mali clock settng
    //IO_WRITE32MSK(0xF0000000, 0x6091c, 0x04390000, 0x837f0000); //bit17=0, bit21=1
    // IO_WRITE32MSK(0xF0000000, 0x6091c, 0x04390000, 0x837f0000);
    ////    IO_WRITE32(0xF0000000, 0x6091c, 0x49b90100);
#endif /* CC_MT5396 */
    return 0;
#else
#ifdef CC_SUPPORT_MULTIPLE_FBDEV
#ifdef HAS_DEV_FB2
#ifdef LINUX_TURNKEY_SOLUTION
    PMX_Init();
#ifndef DELAY_LOGO_DISAPPEAR_TIME
    OSD_SwInit();
#else
    OSD_Init();
#endif /* DELAY_LOGO_DISAPPEAR_TIME */
    GFX_Init();

    //drv_extmjc_init();
#ifndef DELAY_LOGO_DISAPPEAR_TIME
    VERIFY(OSD_SwReset() == 0);
#else
    VERIFY(OSD_Reset() == 0);
#endif /* DELAY_LOGO_DISAPPEAR_TIME */

#ifdef FB_HARD_FADING
    /*  CHUN: In phase 2, this is needed for that appliaction which does not specify FADING FIXME */
#ifndef DELAY_LOGO_DISAPPEAR_TIME
#else
    OSD_PLA_SetFading(1, 0xff);
#endif /* DELAY_LOGO_DISAPPEAR_TIME */
    /*OSD_PLA_SetFading(2, 0xe5);*/
#endif  /* FB_HARD_FADING */
    PMX_SetPlaneOrderArray(_au4PlaneOrderArray);
#endif /* LINUX_TURNKEY_SOLUTION */
    {
        int ret;
        ret = mtk_drv_fb_2_init(&drv_2);
        if(ret)
        return ret;
        mtk_fb_2_setup();
    }
#endif /*HAS_DEV_FB2*/

#ifdef HAS_DEV_FB1
    {
        int ret;
        ret = mtk_drv_fb_1_init(&drv_1);
        if(ret)
        return ret;
        mtk_fb_1_setup();
    }
#endif /*HAS_DEV_FB1*/

#if defined(ANDROID) || defined(CC_SUPPORT_MULTIPLE_FBDEV)
    {
        int ret;
        ret = mtk_drv_fb_init(&drv);
        if(ret)
        return ret;
        mtk_fb_setup();
    }
#endif /*ANDROID*/
    _fgOsdEnabelUpdate = 1;

    // printk("MTK framebuffer driver version: %s\n", _szFBDriverDateTag);

	(void)_mt53fb_dev;
	(void)_mt53fb_probe;
	(void)fb_osd_rgn_obj_init;
    return 0;
#else
    return _mt53fb_probe(&_mt53fb_dev);
#endif /* CC_SUPPORT_MULTIPLE_FBDEV */
#endif /* defined(LINUX_TURNKEY_SOLUTION) && !defined(CC_SUPPORT_MULTIPLE_FBDEV) */
}

#ifdef LINUX_TURNKEY_SOLUTION
#ifdef MT53XX_FB_BUILDIN
module_init(_mt53fb_init);
#endif /* MT53XX_FB_BUILDIN */

#if defined(CC_MTK_ION_SUPPORT)
unsigned int mtk_get_carveoutaddr()
{
    Printf("u4CarveoutAddr 0x%x n", u4CarveoutAddr);
    //dump_stack();
    return u4CarveoutAddr;
}


unsigned int mtk_get_carveoutsize()
{
    Printf("u4CarveoutSize 0x%x n", u4CarveoutSize);
    //dump_stack();
    return u4CarveoutSize;
}


EXPORT_SYMBOL( mtk_get_carveoutaddr);
EXPORT_SYMBOL( mtk_get_carveoutsize);

#endif
int _mt53fb_wait_logo(void* pvArg)
{
    SIZE_T szSize = 0;
    BOOL fgFinished;

    fgFinished = TRUE;
    UNUSED(szSize);

    return 0;
}
#endif /* LINUX_TURNKEY_SOLUTION */


int mtk_fb_get_property(unsigned int* addr, unsigned int* size)
{
    #if defined(CC_LINUX_GPU_SUPPORT)

    *addr = (unsigned int) _fb_info.fix.smem_start;
    *size = (unsigned int) _fb_info.fix.smem_len;
    
    #else
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
	unsigned int smem_start = num_physpages << PAGE_SHIFT;
	unsigned int smem_len   = (BSP_GetFbmMemAddr() - smem_start) & PAGE_MASK;
#else
    unsigned int smem_start = (BSP_GetFbmMemAddr() - DRVCUST_OptGet(
           eDirectFBMemSize) - DRVCUST_OptGet(eFBMemSize)) & PAGE_MASK;
    unsigned int smem_len = DRVCUST_OptGet(eDirectFBMemSize) + DRVCUST_OptGet(
           eFBMemSize);
#endif

    if (smem_start == 0 || smem_len == 0)
    {
       return -1;
    }

    *addr = (unsigned int) smem_start;
    *size = (unsigned int) smem_len;
	/*
	printk(KERN_INFO "%s: BSP_GetFbmMemAddr:0x%08x\n", __func__, BSP_GetFbmMemAddr());
	printk(KERN_INFO "%s: eDirectFBMemSize:0x%08x, eFBMemSize:0x%08x\n", __func__, DRVCUST_OptGet(eDirectFBMemSize), DRVCUST_OptGet(eFBMemSize));
	printk(KERN_INFO "%s: smem_start:0x%08x\n", __func__, smem_start);
	printk(KERN_INFO "%s: smem_len:0x%08x\n", __func__, smem_len);
	*/
    #endif
    
    return 0;
}

EXPORT_SYMBOL( mtk_fb_get_property);

#ifdef CC_SUPPORT_MULTIPLE_FBDEV
int mtk_fb_set_property(unsigned int fb_id,
        unsigned int front_addr, unsigned int back_addr, unsigned int width, unsigned int height)
{
	UINT32 u4MtalFbAddr = fb_osd_get_fb_addr() & PAGE_MASK;

    if(fb_id == 0)
    {
        if((front_addr < u4MtalFbAddr) ||
                (back_addr < u4MtalFbAddr) ||
                (front_addr + height * _get_line_length(width, FB2_BITS_PER_PIXEL) > back_addr))
        {
            printk("mtk_fb_set_property fail! fb%d, front_buffer: 0x%8x, back_buffer: 0x%8x, width: %d, height: %d\n",
                    fb_id, front_addr, back_addr, width, height);
            return -1;
        }
        fb_2->fb.fix.smem_start = front_addr;
#if 0
        fb_2->fb.var.xres = width;
        fb_2->fb.var.yres = height;
        fb_2->fb.var.xres_virtual = width;
        fb_2->fb.var.yres_virtual = height * FB2_NUMBER_OF_BUFFERS;
        fb_2->fb.fix.line_length = _get_line_length(width, FB2_BITS_PER_PIXEL);
#endif

        _fb2_bufs[0] = (UINT32)front_addr;
        _fb2_bufs[1] = (UINT32)front_addr + fb_2->fb.var.yres * fb_2->fb.fix.line_length;

    }
    else if(fb_id == 1)
    {
        if((front_addr < u4MtalFbAddr) ||
                (back_addr < u4MtalFbAddr) ||
                (front_addr + height * _get_line_length(width, FB1_BITS_PER_PIXEL) > back_addr))
        {
            printk("mtk_fb_set_property fail! fb%d, front_buffer: 0x%8x, back_buffer: 0x%8x, width: %d, height: %d\n",
                    fb_id, front_addr, back_addr, width, height);
            return -1;
        }
        fb_1->fb.fix.smem_start = front_addr;
#if 0
        fb_1->fb.var.xres = width;
        fb_1->fb.var.yres = height;
        fb_1->fb.var.xres_virtual = width;
        fb_1->fb.var.yres_virtual = height * FB1_NUMBER_OF_BUFFERS;
        fb_1->fb.fix.line_length = _get_line_length(width, FB1_BITS_PER_PIXEL);
#endif
        _fb1_bufs[0] = (UINT32)front_addr;
        _fb1_bufs[1] = (UINT32)front_addr + fb_1->fb.var.yres * fb_1->fb.fix.line_length;

    }
    else if(fb_id == 2)
    {
        if((front_addr < u4MtalFbAddr) ||
                (back_addr < u4MtalFbAddr) ||
                (front_addr + height * _get_line_length(width, ANDROID_BITS_PER_PIXEL) > back_addr))
        {
            printk("mtk_fb_set_property fail! fb%d, front_buffer: 0x%8x, back_buffer: 0x%8x, width: %d, height: %d\n",
                    fb_id, front_addr, back_addr, width, height);
            return -1;
        }
        fb->fb.fix.smem_start = front_addr;
        fb->fb.var.xres = width;
        fb->fb.var.yres = height;
        fb->fb.var.xres_virtual = width;
        fb->fb.var.yres_virtual = height * ANDROID_NUMBER_OF_BUFFERS;
        fb->fb.fix.line_length = _get_line_length(width, ANDROID_BITS_PER_PIXEL);

        _fb_bufs[0] = (UINT32)front_addr;
        _fb_bufs[1] = (UINT32)front_addr + fb->fb.var.yres * fb->fb.fix.line_length;

    }
    else
    {
        printk("mtk_fb_set_property fail! fb%d, front_buffer: 0x%8x, back_buffer: 0x%8x, width: %d, height: %d\n",
                fb_id, front_addr, back_addr, width, height);
        return -1;
    }
    printk("mtk_fb_set_property ok: fb%d, front_buffer: 0x%8x, back_buffer: 0x%8x, width: %d, height: %d\n",
            fb_id, front_addr, back_addr, width, height);
    return 0;
}

EXPORT_SYMBOL(mtk_fb_set_property);

#endif /* CC_SUPPORT_MULTIPLE_FBDEV */

#if !defined(ANDROID) || defined(ANDROID_TWO_WORLDS)
static void fb_osd_palette_init(void)
{
    UINT32 u4_plane;

    for(u4_plane=0x0;u4_plane<OSD_PLANE_MAX_NUM;u4_plane++)
    {
        _apu4Palette[u4_plane] = (UINT32 *)BSP_AllocAlignedDmaMemory(PALETTE_SIZE, 0x10);
    }

    return;
}
#endif

static unsigned int * fb_osd_get_palette(UINT32 u4_plane)
{
    return _apu4Palette[u4_plane];
}

static unsigned int * fb_osd_get_palette_virt(UINT32 u4_plane)
{
    return (unsigned int * )VIRTUAL((UINT32)_apu4Palette[u4_plane]);
}


static int _mt53fb_get_tmp_buf_ioctl(void* pvArg)
{
    struct mt53fb_imagebuffer rBuf[1];
    rBuf[0].u4Size     = GFX_GetTmpBufSize();
    rBuf[0].u4PhyAddr  = GFX_GetTmpBuf();
    rBuf[0].u4VirtAddr = 0x0;
    
    // printk("tmp_buf[0x%x,0x%x]\n",rBuf[0].u4PhyAddr,rBuf[0].u4Size);
    
    if(copy_to_user(pvArg, (void*)rBuf, sizeof(struct mt53fb_imagebuffer)))
    {
        return -EFAULT;
    }
    return 0;
}



FB_OSD_RGN_OBJ_TMPL(IDLE)
FB_OSD_RGN_OBJ_TMPL(DISP)
FB_OSD_RGN_OBJ_TMPL(CMP_DISP)
FB_OSD_RGN_OBJ_TMPL(CMP)
FB_OSD_RGN_OBJ_TMPL(UNCMP)




