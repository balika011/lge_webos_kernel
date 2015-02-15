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
/*-----------------------------------------------------------------------------
 * $RCSfile: io_mtrm.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file io_mtrm.c
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#if 0 /* should be moved to a specific header  */
#include "u_common.h"
typedef struct
{
    CHAR                              s_name[32];
    UINT16                            ui2_level;
    CHAR                              s_comment[16];
    INT32                             i4_ret;
    UINT32                            ui4_flag;                        
    UINT32                            ui4_tms;
    UINT16                            ui2_common_level;
}PARAM_TMS;

#endif 

#include "mtrm.h"
#include <linux/mtal_ioctl.h>

/* #include "u_cust_def.h" */ /* for VID_QA_MIN_MAX_DFT_T */
#include "drv_comp_id.h"
#include "bagt_mw.h" /* MHF Linux - Basara */

#ifdef CC_NAND_ENABLE /* controlled in sys_build */
	#include "nand_if.h" /* for NANDPART_BulkWrite(...) */
#endif

#include "x_time_msrt.h"
#include "x_handle.h"
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------


// Macro definitions
//-----------------------------------------------------------------------------

/* !!NOTES: move to mw_if2 as Necleus, may not need in here. leave these case here for the time being */
/* 
#define IOCTL_AGT_REQ_ACK_FIXED: lianming
#define IOCTL_D_CUSTOM_AUD_GET_FIXED: lianming
#define IOCTL_D_CUSTOM_AUD_SET_FIXED: lianming
#define IOCTL_OSD_SET_ORDER_FIXED: lianming
#define IOCTL_D_CUSTOM_AUD_GET_MIN_MAX_FIXED: liaming
#define IOCTL_SET_AGT_REQ_ACK_FNT_FIXED: Joshua
*/

/* TODO: to add x_drv_init(..) in kernel space 
#define IOCTL_X_DRV_INIT_FIXED
*/

#ifdef IOCTL_SET_AGT_REQ_ACK_FNT_FIXED /* var: g_pf_buf_agt_req_act_fct defined in project_x/target/mt5395/mw_if2/bagt_mw.c */

#else /* non-g_pf_buf_agt_req_act_fct_FIXED */

	buf_agt_reg_act_fct g_pf_buf_agt_req_act_fct   = NULL;

#endif /* g_pf_buf_agt_req_act_fct_FIXED */

/* original rm_dev.c: start */
MODULE_LICENSE("GPL");

extern void back_kernel_reg(void);
extern void resume_kernel_reg(void);
extern int i4_cb_ret;
extern wait_queue_head_t t_wq_cb;
extern int i4_flag_cb;

#if 1
#define DEBUG
#endif
#ifdef DEBUG
    #define DBG(fmt...)    printk("%s %d:",__FILE__,(unsigned int)__LINE__);  printk(fmt)
#else
    #define DBG(fmt...)
#endif
#define ARRAY_NAME_SIZE  (16)
#define ADPT_KPAGE_SIZE (4 *1024)

/* static int rm_dev_open_count = 0 ; */
unsigned int user_virt_start = 0;
unsigned int kernel_virt_start = 0;
/* static int shmid; */
//static PARAM_PF_NFY *p_nfy_param;
VOID*   pv_usr_regs;

/* static VOID* pv_rm_mc_set_info; */
/* static UINT32 pv_rm_mc_info_len; */


/* static UINT32 open_share_buffer_addr; */
/* static HANDLE_T h_sema_fct_lib; */

unsigned int ttx_user_virt_start[TVD_COMP_NS];
unsigned int ttx_kernel_virt_start[TVD_COMP_NS];

/*fct lib with id*/
typedef struct _RM_FCT_LIB
{
    UINT32 ui4_id;
    VOID *pv_fct;
    struct _RM_FCT_LIB *p_next;
} RM_FCT_LIB;

/* Function Library */
/* static RM_FCT_LIB rm_fct_lib_header; */

typedef struct _handle_kernel_filter
{
    HANDLE_T       h_handle;
    HANDLE_TYPE_T  e_type;
    UINT32         pv_tag;
    BOOL           b_valid;
}handle_kernel_filter;

static UINT32 _get_idx(UINT32 ui4_size);

/* For del_fct async */
#define RM_FREE_MSG_RETRY_COUNT  10
#define RM_FREE_MSG_RETRY_DELAY  ((UINT32) 100)
/* static HANDLE_T h_rm_free_msg_q = NULL_HANDLE; */
/* static HANDLE_T h_thread_rm_free_fct = NULL_HANDLE; */
extern VOID del_fct(UINT32 ui4_id);
/* thread related functions -- ./driver/target/linux-2.6.18/os/thread_linux_drv.c */
extern INT32 ioctl_thread_end(void);
/* thread related functions -- ./driver/ko/mt5398/linux/app_if/mtal/time_msrt/time_msrt.c*/
extern void _time_msrt_set_tms_sel( UINT32 e_tms );
extern UINT32 _time_msrt_get_tms_sel( VOID );




typedef VOID (*rm_adpt_prc_fct) (HANDLE_T  h_obj);

typedef struct _RM_ADPT_MSG_T
{
    rm_adpt_prc_fct  prc;

    HANDLE_T  h_obj;
}RM_ADPT_MSG_T;


/* from rm_dev.c */
unsigned int addr_user_to_kernel(unsigned int addr)
{
    unsigned int ret_addr = 0;

    if ((user_virt_start != 0) && (kernel_virt_start != 0) && (addr > 0))
    {
       if (addr >= user_virt_start)
       {
           ret_addr = (kernel_virt_start + (addr - user_virt_start));
       }
       else 
       {
#ifdef DEBUG
           assert(0);
#endif           
       }
    }

    return ret_addr;
}

/* original rm_dev.c: end */

/* MTAL_PORTING: start */
/* TODO: not in original rm_dev.c: which header should be included? */
extern unsigned int fb_convert_app_to_phy(unsigned int u);

extern void pmx_mw_get_zorder(UCHAR ucPmxId, UINT32 u4Orders[], size_t *ptCount);
extern void pmx_mw_set_zorder(UCHAR ucPmxId, UINT32 u4Orders[]);

/* for mtal_io_mtrm usage: start: code from mtal */
#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtpmx argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtpmx argument error\n");      \
        return MTR_ERR_INV;                                          \
    }
/* for mtal_io_mtrm usage: end: code from mtal */

/* MTAL_PORTING: end */
#if 0 /* not used */
static VOID _make_pla_mxr_addr_to_kernel(DRV_CUSTOM_T* pSrc)
{
    DRV_CUSTOM_T *pTmp;
    DISP_QA_TBL_T* pQaTbl;
    pTmp = pSrc;
    while (!IS_DRV_CUSTOM_END(pTmp))
    {
        pQaTbl = (DISP_QA_TBL_T*)pTmp->pv_arg;
        if(pQaTbl == NULL)
        {
            printk("_make_pla_mxr_addr_to_kernel error.\n");
            return;
        }
        while (pQaTbl->e_src_type != DRVT_UNKNOWN)
        {
            pQaTbl->pt_min_max_dft_lst = (VID_QA_MIN_MAX_DFT_T*)addr_user_to_kernel((UINT32)pQaTbl->pt_min_max_dft_lst);
            pQaTbl++;
        }
        pTmp++;
    }
}

static VOID _make_vid_plane_addr_to_kernel(DRV_CUSTOM_T* pSrc)
{
    DRV_CUSTOM_T *pTmp;
    DRV_CUSTOM_VID_PLA_XLAT_DISP_FMT_EX_INFO_T *pVidPla;
    DISP_QA_TBL_T* pQaTbl;
    pTmp = pSrc;
    while (!IS_DRV_CUSTOM_END(pTmp))
    {
        if (pTmp->e_custom_type == DRV_CUSTOM_TYPE_VID_PLA_XLAT_DISP_FMT_EX)
        {
            pVidPla = (DRV_CUSTOM_VID_PLA_XLAT_DISP_FMT_EX_INFO_T *)pTmp->pv_arg;
            /*no need to convert*/
        }
        else if (pTmp->e_custom_type == DRV_CUSTOM_TYPE_VID_PLA_VID_QA_MIN_MAX_DFT)
        {
            pQaTbl = (DISP_QA_TBL_T*)pTmp->pv_arg;
            if(pQaTbl == NULL)
            {
                printk("_make_vid_plane_addr_to_kernel error.\n");
                return;
            }
            while (pQaTbl->e_src_type != DRVT_UNKNOWN)
            {
                pQaTbl->pt_min_max_dft_lst = (VID_QA_MIN_MAX_DFT_T*)addr_user_to_kernel((UINT32)pQaTbl->pt_min_max_dft_lst);
                pQaTbl++;
            }
        }
        pTmp++;
    }
}

static VOID _make_post_reg_addr_to_kernel(DRV_CUSTOM_T* pSrc)
{
    DRV_CUSTOM_T *pTmp;
    DRV_REG_GRP_NAME_T * pGrp;
    pTmp = pSrc;
    while (!IS_DRV_CUSTOM_END(pTmp))
    {
        pGrp = (DRV_REG_GRP_NAME_T *)pTmp->pv_arg;
        if(pGrp == NULL)
        {
            printk("_make_post_reg_addr_to_kernel error.\n");
            return;
        }
        pGrp->pt_drv_ids = (DRV_ID_T*)addr_user_to_kernel((UINT32)pGrp->pt_drv_ids);
        pTmp++;
    }
}

static VOID _make_combi_reg_addr_to_kernel(DRV_CUSTOM_T* pSrc)
{
    DRV_CUSTOM_T *pTmp;
        DRV_CUSTOM_TYPE_AVC_COMBI_REG_T* pCombiReg;
        pTmp = pSrc;
    while (!IS_DRV_CUSTOM_END(pTmp))
    {
        pCombiReg = (DRV_CUSTOM_TYPE_AVC_COMBI_REG_T*)pTmp->pv_arg;
        if(pCombiReg == NULL)
        {
            printk("_make_combi_reg_addr_to_kernel error.\n");
            return;
        }
        pCombiReg->pt_connection_ids = (DRV_ID_T*)addr_user_to_kernel((UINT32)pCombiReg->pt_connection_ids);
        pTmp++;
    }
/*        return pTmp; */
}

static VOID _make_drv_reg_addr_to_kernel(DRV_CUSTOM_T* pSrc)
{
    DRV_CUSTOM_T *pTmp ;
    DRV_REG_T *pReg;
    pTmp = pSrc;
      
    while (!IS_DRV_CUSTOM_END(pTmp))
    {
        pReg = (DRV_REG_T *)pTmp->pv_arg;
        if(pReg == NULL)
        {
            printk("_make_drv_reg_addr_to_kernel error.\n");
            return;
        }
        pReg->pt_connection_ids = (DRV_ID_T*)addr_user_to_kernel((UINT32)pReg->pt_connection_ids);
        pTmp++;
    }
}

static VOID _make_config_addr_to_kernel(GEN_CONFIG_T*  pt_config)
{
    DRV_CUSTOM_DIR_T *p_custom_config;
    DRV_CUSTOM_T *pt_custom;
    
    if(pt_config == NULL)
    {
        return;
    }
    
    pt_config->pv_config = addr_user_to_kernel(pt_config->pv_config);
    p_custom_config = pt_config->pv_config;
    while (!IS_DRV_CUSTOM_DIR_END(p_custom_config))
    {
        p_custom_config->pt_custom = addr_user_to_kernel(p_custom_config->pt_custom);
        pt_custom = (DRV_CUSTOM_T *)p_custom_config->pt_custom;
        p_custom_config->pt_custom->pv_arg = addr_user_to_kernel(pt_custom->pv_arg);
        if (p_custom_config->e_first_type == DRVT_AVC_COMP_VIDEO)
        {
            _make_drv_reg_addr_to_kernel(p_custom_config->pt_custom);
        }
        if (p_custom_config->e_first_type == DRVT_AVC_COMBI)
        {
            _make_combi_reg_addr_to_kernel(p_custom_config->pt_custom);
        }
        if (p_custom_config->e_first_type == DRVT_UNKNOWN)
        {
            _make_post_reg_addr_to_kernel(p_custom_config->pt_custom);
        }
        if (p_custom_config->e_first_type == DRVT_VID_PLANE)
        {
            _make_vid_plane_addr_to_kernel(p_custom_config->pt_custom);
        }
        if (p_custom_config->e_first_type == DRVT_PLA_MXR)
        {
            _make_pla_mxr_addr_to_kernel(p_custom_config->pt_custom);
        }
        p_custom_config++;
    }

}
#endif /* #if 0 */
static UINT32 
_get_idx(UINT32 ui4_size)
{
    register unsigned rn;
    register unsigned rd;
    /*
     * Value like 100000 is to be 011111.
     * */
    rn = ui4_size - 1;

    __asm__ __volatile__ ("clz %0,%1":"=r"(rd):"r"(rn));

    return (32 - rd);
}

/* from ori: dtv_linux\dtv_driver_adpt\common\adaption\adpt_res_mngr\rm_dev.c */
int rm_dev_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long param)
{
    switch(cmd)
    {    
        case IOCTRL_MALLOC_SHM:
            {
                int i;
                UINT32 ui4_size = 0;
                UINT32 ui4_num = 0;
                UINT32 ui4_page_num = 0;
                
                if (copy_from_user((VOID*)&ui4_size, (void __user *)param,
                               sizeof(UINT32)))
                {
                    /* return -EFAULT; */
					return MTR_NOT_OK;
                }
                if(ui4_size == 0) 
                {
                    /* return -EFAULT; */
					return MTR_NOT_OK;
                }
                if((ui4_size % ADPT_KPAGE_SIZE) != 0)
                {
                    /* return -EFAULT; */ /* the size should be 4*1024 numbers */
					return MTR_NOT_OK;
                }
                ui4_page_num = ui4_size / ADPT_KPAGE_SIZE;
                ui4_num = _get_idx(ui4_page_num);
                kernel_virt_start  = __get_free_pages(GFP_KERNEL,ui4_num); /* 2^2 */
                for (i = 0; i < ui4_page_num; i++) /* 4 pages */
                    SetPageReserved(virt_to_page((char *)kernel_virt_start+i*ADPT_KPAGE_SIZE));
                if(copy_to_user((void __user *)param, (VOID*) &kernel_virt_start,sizeof(unsigned long)))
                {
                    /* return -EFAULT; */
					return MTR_NOT_OK;
                }
            }
            break;

        case IOCTRL_MALLOC_SHM2:
            {
                int i;
                UINT32 ui4_size = 0;
                UINT32 ui4_num = 0;
                UINT32 ui4_page_num = 0;
                UINT32 ui4_kernel_virt_start;

                if (copy_from_user((VOID*)&ui4_size, (void __user *)param,
                               sizeof(UINT32)))
                {
                    /* return -EFAULT; */
					return MTR_NOT_OK;
                }
                if(ui4_size == 0) 
                {
                    /* return -EFAULT; */
					return MTR_NOT_OK;
                }
                if ((ui4_size % ADPT_KPAGE_SIZE) != 0)
                {
                    /* return -EFAULT; */ /* the size should be 4*1024 numbers */
					return MTR_NOT_OK;
                }
                ui4_page_num = ui4_size / ADPT_KPAGE_SIZE;
                ui4_num = _get_idx(ui4_page_num);
                printk("\n [SHM alloc]{order = %u, ui4_page_num =%u}\n", ui4_num, ui4_page_num);
                ui4_kernel_virt_start  = __get_free_pages(GFP_KERNEL,ui4_num); /* 2^2 */
                for (i = 0; i < ui4_page_num; i++) /* 4 pages */
                    SetPageReserved(virt_to_page((char *)ui4_kernel_virt_start + i * ADPT_KPAGE_SIZE));
                if(copy_to_user((void __user*)param, (VOID*)&ui4_kernel_virt_start,sizeof(unsigned long)))
                {
                    /* return -EFAULT; */
					return MTR_NOT_OK;
                }
            }
            break;

        case IOCTRL_FREE_SHM2:
            {
                int i;
                UINT64 ui8_addr_size;
                UINT32 ui4_addr = 0;
                UINT32 ui4_size = 0;
                UINT32 ui4_num = 0;
                UINT32 ui4_page_num = 0;
                
                if (copy_from_user((VOID*)&ui8_addr_size, (void __user *)param,
                               sizeof(UINT64)))
                {
                    /* return -EFAULT; */
					return MTR_NOT_OK;
                }

                ui4_addr = ui8_addr_size>>32;
                ui4_addr += PAGE_OFFSET;
                ui4_size = ui8_addr_size&0xFFFFFFFF;
        
                if (ui4_size == 0) 
                {
                    /* return -EFAULT; */
					return MTR_NOT_OK;
                }
        
                if((ui4_size % ADPT_KPAGE_SIZE) != 0)
                {
                    /* return -EFAULT; */ /* the size should be 4*1024 numbers */
					return MTR_NOT_OK;
                }
                ui4_page_num = ui4_size / ADPT_KPAGE_SIZE;
                
        
                for (i = 0; i < ui4_page_num; i++) /* 4 pages */
                    ClearPageReserved(virt_to_page((char *)ui4_addr + i * ADPT_KPAGE_SIZE));
        
                ui4_num = _get_idx(ui4_page_num);
                printk("\n [SHM free]{order=%u, ui4_page_num =%u}\n",ui4_num, ui4_page_num);
                free_pages(ui4_addr,ui4_num); /* 2^2 */
            }
        
            break;

        case IOCTL_HANDLE_GET_INFO:
            {
                handle_kernel_filter t_filter;
                HANDLE_TYPE_T  e_type;
                UINT32         pv_tag;
                
                if (copy_from_user((VOID*)&t_filter, (void __user *)param,
                               sizeof(handle_kernel_filter)))
                {
                    return -EFAULT;
                }
                
                if(x_handle_get_info(t_filter.h_handle, &e_type, (VOID*)&pv_tag) < 0)
                {
                     return -EFAULT;
                }
                
                t_filter.e_type = e_type;
                t_filter.pv_tag = pv_tag;
                
                if (copy_to_user((void __user *)param, (VOID*) &t_filter,
                                sizeof(handle_kernel_filter)))
                {
                    return -EFAULT;
                }
                
            }   
            break;
        case IOCTL_HANDLE_GET_TAG:
            {
                handle_kernel_filter t_filter;
                UINT32         pv_tag;
                
                if (copy_from_user((VOID*)&t_filter, (void __user *)param,
                               sizeof(handle_kernel_filter)))
                {
                    return -EFAULT;
                }
                
                if(x_handle_get_tag(t_filter.h_handle, (VOID*)&pv_tag) < 0)
                {
                     return -EFAULT;
                }
                
                t_filter.pv_tag = pv_tag;
                
                if (copy_to_user((void __user *)param, (VOID*)&t_filter,
                                sizeof(handle_kernel_filter)))
                {
                    return -EFAULT;
                }
                
            }   
            break;
        case IOCTL_HANDLE_GET_TYPE:
            {
                handle_kernel_filter t_filter;
                HANDLE_TYPE_T  e_type;
                
                if (copy_from_user((VOID*)&t_filter, (void __user *)param,
                               sizeof(handle_kernel_filter)))
                {
                    return -EFAULT;
                }
                
                if(x_handle_get_type(t_filter.h_handle, &e_type) < 0)
                {
                     return -EFAULT;
                }
                
                t_filter.e_type = e_type;
                
                if (copy_to_user((void __user *)param, (VOID*)&t_filter,
                                sizeof(handle_kernel_filter)))
                {
                    return -EFAULT;
                }
                
            }   
            break;
        case IOCTL_HANDLE_SET_TAG:
            {
                handle_kernel_filter t_filter;
                
                if (copy_from_user((VOID*)&t_filter, (void __user *)param,
                               sizeof(handle_kernel_filter)))
                {
                    return -EFAULT;
                }
                
                if(x_handle_set_tag(t_filter.h_handle, (VOID*)t_filter.pv_tag) < 0)
                {
                    return -EFAULT;
                }
            }   
            break;
        case IOCTL_HANDLE_VALID:
            {
                handle_kernel_filter t_filter;
                

                
                if (copy_from_user((VOID*)&t_filter, (void __user *)param,
                               sizeof(handle_kernel_filter)))
                {
                    return -EFAULT;
                }
                
                t_filter.b_valid = x_handle_valid(t_filter.h_handle);
                
                if (copy_to_user((void __user *)param, (VOID*)&t_filter,
                                sizeof(handle_kernel_filter)))
                {
                    return -EFAULT;
                }
                
            }   
            break;

        case IOCTRL_SET_USER_VIRT_START:
            user_virt_start = param;
            break;
        case IOCTL_TMS_INIT:
            {
                _time_msrt_init(0);
            }
            break;
			
        case IOCTL_TMS_BEGIN:
            {
                PARAM_TMS* pt_tms_param;
                pt_tms_param = (PARAM_TMS*)addr_user_to_kernel((UINT32)param);
				//copy_from_user((VOID*)pt_tms_param, (void __user *)param,sizeof(PARAM_TMS));
                pt_tms_param->i4_ret = _time_msrt_begin(pt_tms_param->s_name,pt_tms_param->ui2_level,pt_tms_param->s_comment);
                //copy_to_user(param,pt_tms_param,sizeof(PARAM_TMS));
            }
            break;
        case IOCTL_TMS_END:
            {
                PARAM_TMS* pt_tms_param;
                pt_tms_param = (PARAM_TMS*)addr_user_to_kernel((UINT32)param);
				//copy_from_user((VOID*)pt_tms_param, (void __user*)param,sizeof(PARAM_TMS));
                pt_tms_param->i4_ret = _time_msrt_end(pt_tms_param->s_name,pt_tms_param->ui2_level,pt_tms_param->s_comment);
                //copy_to_user(param,pt_tms_param,sizeof(PARAM_TMS));
            }
            break;
        case IOCTL_TMS_DIFF:
            {
                PARAM_TMS* pt_tms_param;
                pt_tms_param = (PARAM_TMS*)addr_user_to_kernel((UINT32)param);
				//copy_from_user((VOID*)pt_tms_param, (void __user *)param,sizeof(PARAM_TMS));
                pt_tms_param->i4_ret = _time_msrt_diff(pt_tms_param->s_name,pt_tms_param->ui2_level,pt_tms_param->s_comment);
                //copy_to_user(param,pt_tms_param,sizeof(PARAM_TMS));
            }
            break;
        case IOCTL_TMS_SPOT:
            {
                PARAM_TMS* pt_tms_param;
                pt_tms_param = (PARAM_TMS*)addr_user_to_kernel((UINT32)param);
				//copy_from_user((VOID*)pt_tms_param, (void __user *)param,sizeof(PARAM_TMS));
                pt_tms_param->i4_ret = _time_msrt_spot(pt_tms_param->s_name,pt_tms_param->ui2_level,pt_tms_param->s_comment);
                //copy_to_user(param,pt_tms_param,sizeof(PARAM_TMS));
            }
            break;
        case IOCTL_TMS_DUMP:
            {
                PARAM_TMS* pt_tms_param;
                pt_tms_param = (PARAM_TMS*)addr_user_to_kernel((UINT32)param);
				//copy_from_user((VOID*)pt_tms_param, (void __user *)param,sizeof(PARAM_TMS));
                _time_msrt_dump(pt_tms_param->s_name);
            }
            break;
        case IOCTL_TMS_DUMP_DETAIL:
            {
                PARAM_TMS* pt_tms_param;
                pt_tms_param = (PARAM_TMS*)addr_user_to_kernel((UINT32)param);
				//copy_from_user((VOID*)pt_tms_param, (void __user *)param,sizeof(PARAM_TMS));
                _time_msrt_dump_detail(pt_tms_param->s_name);
            }
            break;
        case IOCTL_TMS_CLEAN:
            {
                PARAM_TMS* pt_tms_param;
                pt_tms_param = (PARAM_TMS*)addr_user_to_kernel((UINT32)param);
				//copy_from_user((VOID*)pt_tms_param, (void __user *)param,sizeof(PARAM_TMS));
                _time_msrt_clean(pt_tms_param->s_name);
            }
            break;
        case IOCTL_TMS_GET_LEVEL_FLAG:
            {
                PARAM_TMS* pt_tms_param;
                pt_tms_param = (PARAM_TMS*)addr_user_to_kernel((UINT32)param);
                pt_tms_param->ui2_level = _time_msrt_get_level(pt_tms_param->ui2_level);
            }
            break;
        case IOCTL_TMS_GET_COMMON_LEVEL:
            {
                PARAM_TMS* pt_tms_param;
                pt_tms_param = (PARAM_TMS*)addr_user_to_kernel((UINT32)param);
                pt_tms_param->ui2_common_level = _time_msrt_get_common_tms_level();   
            }
            break;
        case IOCTL_TMS_SET_COMMON_LEVEL:
            {
                PARAM_TMS* pt_tms_param;
                pt_tms_param = (PARAM_TMS*)addr_user_to_kernel((UINT32)param);
                _time_msrt_set_common_tms_level(pt_tms_param->ui2_common_level);   
            }
            break;
        case IOCTL_TMS_SET_FLAG:
            {
                PARAM_TMS* pt_tms_param;
                pt_tms_param = (PARAM_TMS*)addr_user_to_kernel((UINT32)param);
                _time_msrt_set_flag(pt_tms_param->ui4_flag);
            }
            break;
        case IOCTL_TMS_GET_FLAG:
            {
                PARAM_TMS* pt_tms_param;
                pt_tms_param = (PARAM_TMS*)addr_user_to_kernel((UINT32)param);
				//copy_from_user(((VOID*)pt_tms_param, (void __user *)param,sizeof(PARAM_TMS));
                pt_tms_param->ui4_flag  = _time_msrt_get_flag();
                //copy_to_user(param,pt_tms_param,sizeof(PARAM_TMS));
            }
            break;
        case IOCTL_TMS_SET_TMS_SEL:
            {
                PARAM_TMS* pt_tms_param;
                pt_tms_param = (PARAM_TMS*)addr_user_to_kernel((UINT32)param);
                _time_msrt_set_tms_sel(pt_tms_param->ui4_tms);
            }
            break;
        case IOCTL_TMS_GET_TMS_SEL:
            {
                PARAM_TMS* pt_tms_param;
                pt_tms_param = (PARAM_TMS*)addr_user_to_kernel((UINT32)param);
				//copy_from_user((VOID*)pt_tms_param, (void __user *)param,sizeof(PARAM_TMS));
                pt_tms_param->ui4_tms  = _time_msrt_get_tms_sel();
                //copy_to_user(param,pt_tms_param,sizeof(PARAM_TMS));
            }
            break; 
        default:
            break;

    }
/*    return 0; */
	return MTR_OK;
}


/* from ori: dtv_linux\dtv_driver_adpt\common\adaption\adpt_direct_api\ioctl_direct_call.c */
#include "x_os.h"
extern VOID x_polling_tick(VOID);

int rm_dev_ioctl_direct_call(struct inode *inode, 
	struct file *file, unsigned int cmd, unsigned long param)
{
	int i4_tmp_ret = 0;
    switch(cmd)
    {
    	/* --- IOCTL_TEST_COMMAND --- */
		case IOCTL_TEST_COMMAND:
			{
				D_TEST_COMMAND_STRUCT_T t_test_command_param;
				i4_tmp_ret = copy_from_user((VOID*)&t_test_command_param, (void __user *)param, sizeof(D_TEST_COMMAND_STRUCT_T));
				/* TODO: process "t_test_command_param" here*/
			}
			break;
			
        case IOCTL_GET_DRV_TICK:
            x_polling_tick();
            break;
        case IOCTL_GET_TICK_PERIOD:
            {
                UINT32 tick;
                tick = x_os_drv_get_tick_period();
                i4_tmp_ret = copy_to_user((void __user*) param, (VOID*)&tick,sizeof(UINT32));
            }
            break;
        case IOCTL_GET_FINE_TICK:
            {
                UINT32 tick;
                tick = x_os_drv_get_fine_tick();
                i4_tmp_ret = copy_to_user((void __user*)param, (VOID*)&tick,sizeof(UINT32));
            }
            break;
        case IOCTRL_HW_TICK:
            {
                unsigned long *tick_addr = (unsigned long *)0xf0008074;
                unsigned long cur_tick;
                cur_tick = *tick_addr;
                i4_tmp_ret = copy_to_user((void __user *)param, (VOID*)&cur_tick,sizeof(unsigned long));
            }
            break;
        case IOCTL_RESET_GAMMA:
            {
                *((UINT32 *)0xf0025054) = 0x00249065;
                
                break;
            }
        case IOCTL_TEST_SURF:
            {
                /*char * p = fb_convert_app_to_phy(param);
                  
                memset((char *)p + 4, 0xff, 1024 * 1024);*/
                /*
                D_PLANE_SET_ORDER_T t_order_info;
                UINT32 au4PlaneOrder[5];
                size_t tLen;
                UINT32 u4OldOrder = 0;
                    

                copy_from_user((VOID*)&t_order_info, (void __user *)param, sizeof(D_PLANE_SET_ORDER_T));
                
                pmx_mw_get_zorder((UCHAR)0, au4PlaneOrder, &tLen);
                t_order_info.u4_order = au4PlaneOrder[t_order_info.u4_idx];
                
                copy_to_user(param, &t_order_info, sizeof(D_PLANE_SET_ORDER_T));
                */
            }
            break;
        case IOCTL_OSD_GET_ORDER:
            {
                char * p = (char *)fb_convert_app_to_phy(param);
                  
                memset(p, 0xff, 1024 * 1024);
                /*
                D_PLANE_SET_ORDER_T t_order_info;
                UINT32 au4PlaneOrder[5];
                size_t tLen;
                UINT32 u4OldOrder = 0;
                    

                copy_from_user((VOID*)&t_order_info,  (void __user *)param, sizeof(D_PLANE_SET_ORDER_T));
                
                pmx_mw_get_zorder((UCHAR)0, au4PlaneOrder, &tLen);
                t_order_info.u4_order = au4PlaneOrder[t_order_info.u4_idx];
                
                copy_to_user(param, &t_order_info, sizeof(D_PLANE_SET_ORDER_T));
                */
            }
            break;


#ifdef IOCTL_OSD_SET_ORDER_FIXED
        case IOCTL_OSD_SET_ORDER:
            {
                D_PLANE_SET_ORDER_T t_order_info;
                UINT32 au4PlaneOrder[5];
                size_t tLen;
                /*UINT32 u4Order = rSet.rSetting.u4Order;*/
                UINT32 u4OldOrder = 0;
                UINT32 i;

                i4_tmp_ret = copy_from_user((VOID*)&t_order_info, (void __user *)param, sizeof(D_PLANE_SET_ORDER_T));

                pmx_mw_get_zorder((UCHAR)0, au4PlaneOrder, &tLen);
                u4OldOrder = au4PlaneOrder[t_order_info.u4_idx];
                
                if (t_order_info.u4_order > u4OldOrder)
                {
                    for (i = 0; i < tLen; i++)
                    {
                        if (au4PlaneOrder[i] > u4OldOrder && au4PlaneOrder[i] <=  t_order_info.u4_order)
                        {
                            au4PlaneOrder[i]--;
                        }
                    }    
                }
                else
                {
                    for (i = 0; i < tLen; i++)
                    {
                        if (au4PlaneOrder[i] >=  t_order_info.u4_order && au4PlaneOrder[i] < u4OldOrder)
                        {
                            au4PlaneOrder[i]++;
                        }
                    }  
                }    

                au4PlaneOrder[t_order_info.u4_idx] = t_order_info.u4_order;
                pmx_mw_set_zorder((UCHAR)0, au4PlaneOrder);
            }
            break;
#endif /* IOCTL_OSD_SET_ORDER_FIXED */

        case IOCTL_THREAD_END:

            ioctl_thread_end();                  
            break;

#ifdef IOCTL_D_CUSTOM_AUD_SET_FIXED
        case IOCTL_D_CUSTOM_AUD_SET:
            {
                /* MHF Linux - yjg */
                D_CUSTOM_AUD_SET_T *pt_d_custom_aud_set;
                pt_d_custom_aud_set = (D_CUSTOM_AUD_SET_T *)addr_user_to_kernel(param);
                /*pv_set_type = (VOID *)addr_user_to_kernel(pt_d_custom_aud_set->pv_set_type);*/
                pt_d_custom_aud_set->i4_ret = d_custom_aud_set(pt_d_custom_aud_set->e_aud_type,
                            pt_d_custom_aud_set->pv_set_type,
                            pt_d_custom_aud_set->z_size,
                            pt_d_custom_aud_set->b_store);
            }
            break;

#endif /* IOCTL_D_CUSTOM_AUD_SET_FIXED */	

#ifdef IOCTL_D_CUSTOM_AUD_GET_FIXED
        case IOCTL_D_CUSTOM_AUD_GET:
            {

#if 0 /* MHF Linux - yjg */
                D_CUSTOM_AUD_GET_T *pt_d_custom_aud_get;
                VOID* pv_get_type;
                SIZE_T* pz_size;
                pt_d_custom_aud_get = (D_CUSTOM_AUD_GET_T *)addr_user_to_kernel(param);
                pv_get_type = (VOID*)addr_user_to_kernel(pt_d_custom_aud_get->pv_get_type);
                pz_size = (SIZE_T*)addr_user_to_kernel(pt_d_custom_aud_get->pz_size);
                pt_d_custom_aud_get->i4_ret = d_custom_aud_get(
                            pt_d_custom_aud_get->e_aud_type,
                            pv_get_type,
                            pz_size);
#else
             
                D_CUSTOM_AUD_GET_T *pt_d_custom_aud_get;
                VOID* pv_get_type;
                SIZE_T size;
                SIZE_T size_output;
                
                pt_d_custom_aud_get = (D_CUSTOM_AUD_GET_T *)addr_user_to_kernel(param);
                copy_from_user((VOID*)&size, (void __user *)pt_d_custom_aud_get->pz_size,sizeof(SIZE_T));
                pv_get_type = x_mem_alloc(size);
                copy_from_user((VOID*)pv_get_type, (void __user *)pt_d_custom_aud_get->pv_get_type,size);
                size_output = size;

                pt_d_custom_aud_get->i4_ret = d_custom_aud_get(
                            pt_d_custom_aud_get->e_aud_type,
                            pv_get_type,
                            &size_output);

                copy_to_user((void __user *)pt_d_custom_aud_get->pv_get_type, (VOID *)pv_get_type,size);
                copy_to_user((void __user *)pt_d_custom_aud_get->pz_size, (VOID *)&size_output,sizeof(SIZE_T));
                x_mem_free(pv_get_type);
#endif /* MHF Linux end - yjg */              
            }
            break;
#endif /* IOCTL_D_CUSTOM_AUD_GET_FIXED */


#ifdef IOCTL_D_CUSTOM_AUD_GET_MIN_MAX_FIXED
        case IOCTL_D_CUSTOM_AUD_GET_MIN_MAX:
            {               
#if 0 /* MHF Linux - yjg */
                D_CUSTOM_AUD_GET_MIN_MAX_T *pt_d_custom_aud_get_min_max;
                INT16* pui2_min;
                INT16* pui2_max;
                pt_d_custom_aud_get_min_max = (D_CUSTOM_AUD_GET_MIN_MAX_T *)addr_user_to_kernel(param);
                pui2_min = (INT16*)addr_user_to_kernel(pt_d_custom_aud_get_min_max->pui2_min);
                pui2_max = (INT16*)addr_user_to_kernel(pt_d_custom_aud_get_min_max->pui2_max);
                pt_d_custom_aud_get_min_max->i4_ret = d_custom_aud_get_min_max(
                            pt_d_custom_aud_get_min_max->e_aud_type,
                            pui2_min,
                            pui2_max);
#else
                D_CUSTOM_AUD_GET_MIN_MAX_T *pt_d_custom_aud_get_min_max;
                INT16 ui2_min;
                INT16 ui2_max;

                pt_d_custom_aud_get_min_max = (D_CUSTOM_AUD_GET_MIN_MAX_T *)addr_user_to_kernel(param);
                copy_from_user((VOID*)&ui2_min, (void __user *)pt_d_custom_aud_get_min_max->pui2_min,sizeof(INT16));
                copy_from_user((VOID*)&ui2_max, (void __user *)pt_d_custom_aud_get_min_max->pui2_max,sizeof(INT16));

                pt_d_custom_aud_get_min_max->i4_ret = d_custom_aud_get_min_max(
                            pt_d_custom_aud_get_min_max->e_aud_type,
                            &ui2_min,
                            &ui2_max);

                i4_tmp_ret = copy_to_user((void __user *)pt_d_custom_aud_get_min_max->pui2_min, (VOID *)&ui2_min,sizeof(INT16));
                i4_tmp_ret = copy_to_user((void __user *)pt_d_custom_aud_get_min_max->pui2_max, (VOID *)&ui2_max,sizeof(INT16));
#endif /* MHF Linux end - yjg */
            }
            break;

#endif /* IOCTL_D_CUSTOM_AUD_GET_MIN_MAX_FIXED */			
/*
        case IOCTL_D_CUSTOM_TUNER_SET:
            {
                D_CUSTOM_TUNER_SET_T *pt_d_custom_tuner_set;
                VOID* pv_set_info;
                pt_d_custom_tuner_set = (D_CUSTOM_TUNER_SET_T *)addr_user_to_kernel(param);
                pv_set_info = (VOID*)addr_user_to_kernel(pt_d_custom_tuner_set->pv_set_info);
                pt_d_custom_tuner_set->i4_ret = d_custom_tuner_set(pt_d_custom_tuner_set->e_tuner_type,
                            pt_d_custom_tuner_set->e_tuner_set_type,
                            pv_set_info,
                            pt_d_custom_tuner_set->z_size);
            }
            break;
        case IOCTL_D_CUSTOM_TUNER_GET:
            {
                D_CUSTOM_TUNER_GET_T *pt_d_custom_tuner_get;
                VOID* pv_get_info;
                SIZE_T* pz_size;
                pt_d_custom_tuner_get = (D_CUSTOM_TUNER_GET_T *)addr_user_to_kernel(param);
                pv_get_info = (VOID*)addr_user_to_kernel(pt_d_custom_tuner_get->pv_get_info);
                pz_size = (SIZE_T*)addr_user_to_kernel(pt_d_custom_tuner_get->pz_size);
                pt_d_custom_tuner_get->i4_ret = d_custom_tuner_get(pt_d_custom_tuner_get->e_tuner_type,
                            pt_d_custom_tuner_get->e_tuner_get_type,
                            pv_get_info,
                            pz_size);
            }
            break;
*/
        case IOCTL_VDP_SETOUTREGION:
            #if 0
            {
                MW_VDP_SETOUTREGION_T *pt_VDP_SetOutRegion;
                pt_VDP_SetOutRegion = (MW_VDP_SETOUTREGION_T *)addr_user_to_kernel((UINT32)param);
                pt_VDP_SetOutRegion->ui4_ret = VDP_SetOutRegion(pt_VDP_SetOutRegion->ucVdpId,
                            pt_VDP_SetOutRegion->ucOutFullRegion,
                            pt_VDP_SetOutRegion->rOutRegion);
            }
            #endif
            break;

        case IOCTL_VDP_CheckDisplayMode:
            #if 0
            {
                UINT32 ui4_ret;
                ui4_ret = MW_VDP_CheckDisplayMode();
                copy_to_user(param, &ui4_ret, sizeof(UINT32));
            }
            #endif
            break;

#ifdef IOCTL_SET_AGT_REQ_ACK_FNT_FIXED			
        /* MHF Linux - Basara */
        case IOCTL_SET_AGT_REQ_ACK_FNT:
            {
                g_pf_buf_agt_req_act_fct = param;
            }
            break;
#endif /* IOCTL_SET_AGT_REQ_ACK_FNT_FIXED */
#ifdef IOCTL_AGT_REQ_ACK_FIXED
        case IOCTL_AGT_REQ_ACK:
            {
                D_AGT_REQ_ACK_T * pt_d_agt_reg_ack;
                BUF_AGT_DATA_BLK_T* pt_buf_agt_data_blk;

                pt_d_agt_reg_ack = (D_AGT_REQ_ACK_T *)addr_user_to_kernel((UINT32)param);

                pt_buf_agt_data_blk = x_mem_alloc(sizeof(BUF_AGT_DATA_BLK_T));

                i4_tmp_ret = copy_from_user((VOID*)pt_buf_agt_data_blk, (void __user *)pt_d_agt_reg_ack->pt_data_blk,sizeof(BUF_AGT_DATA_BLK_T));

                pt_d_agt_reg_ack->b_ret = buf_agt_req_ack_fct(
                        pt_d_agt_reg_ack->ui2_buf_agt_id,
                        pt_d_agt_reg_ack->e_ack_cond,
                        pt_buf_agt_data_blk);

                x_mem_free(pt_buf_agt_data_blk);
            }
            break;
#endif /* IOCTL_AGT_REQ_ACK_FIXED */

        case IOCTL_GET_FBM_SIZE:
            return DRVCUST_OptGet(eFbmMemSize);

        case IOCTL_GET_FBM_PHY_ADDR:
            return BSP_GetFbmMemAddr();


#ifdef CC_NAND_ENABLE /* controlled in sys_build */
        case IOCTL_PART_BULK_WRITE:
        {
            UINT64  ui8_offset = 0;
            D_PART_BULK_WRITE_T t_write_info;
            
            i4_tmp_ret = copy_from_user((VOID*)&t_write_info, (void __user*)param, sizeof(D_PART_BULK_WRITE_T));
            ui8_offset = ((((UINT64)t_write_info.ui4_part_id) << 32) | t_write_info.ui4_offset);

            /* NAND flash Bulk Write */
            return NANDPART_BulkWrite(ui8_offset, t_write_info.ui1_value, t_write_info.ui4_size);
        }
#endif

        default:
            break;
    }

/*    return 0; */
	return MTR_OK;
}


int mtal_ioctl_mtrm(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg)
{
	MTAL_IOCTL_2ARG_T rArg;

    UINT32 ui4_real_cmd = 0;
    UINT32 u4_real_arg = 0;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg,rArg);

    ui4_real_cmd = (UINT32)rArg.ai4Arg[0];
    u4_real_arg = (UINT32)(UINT32*)rArg.ai4Arg[1];
	
	if(ui4_real_cmd >= (unsigned int)IOCTL_ADPT_START && ui4_real_cmd < (unsigned int)IOCTL_ADPT_END)
	{
		/* return _CB_Ioctl_Adpt(inode, file, ui4_real_cmd, u4_real_arg); */
		/* this part is not needed in MTAL layer */
		return MTR_NOT_OK;
	}
	else if(ui4_real_cmd >= (unsigned int)IOCTL_RM_START && ui4_real_cmd < (unsigned int)IOCTL_RM_END)
	{
		return rm_dev_ioctl(inode, file, ui4_real_cmd, u4_real_arg);
	}
    else
    {
		return rm_dev_ioctl_direct_call(inode, file, ui4_real_cmd, u4_real_arg);
    }

}


