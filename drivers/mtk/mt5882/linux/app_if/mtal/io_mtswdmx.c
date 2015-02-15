
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
 * $RCSfile: io_mtvdec.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file mtvdec.c
 *  This file contains implementation of exported APIs of Video Decoder (VDEC) module,
 *  which inclued MPEG 1/2/4, H.264(AVC), and VC-1.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "mtswdmx.h"
#include "mtutil.h"
#include "swdmx_if.h"
#include "swdmx_drvif.h"
#include "vdp_if.h"
#include "dmx_if.h"
#include "stc_drvif.h"
#include "cb_data.h"
#include "cb_low.h"
#include "x_os.h"
#include "x_assert.h"
#include "x_mm_common.h"
#include <linux/mtal_ioctl.h>

#include "x_debug.h"

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#ifdef CC_SKYPE_FINE_INSTANCE
#define SWDMX_MAX_ES      5
#else
#define SWDMX_MAX_ES      3
#endif

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#if 1 // NOTE: the argument order is ArgKernel, ArgUsr, size, which differs from mtutil.h !!!
#define COPY_FROM_USER_SIZE(ArgKernel, ArgUsr, size)            \
    if (copy_from_user((void *)ArgKernel,(void __user *)ArgUsr, \
                       size))                                   \
    {                                                           \
        printk("%s copy_from_user error\n", __FUNCTION__);      \
        return MTR_ERR_INV;                                     \
    }
#endif

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
// for multi-process API lock
static HANDLE_T _hAPISema = 0;

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
static void LockVdecSemaphoreMtal(void)
{
    if (_hAPISema)
    {
        VERIFY(x_sema_lock(_hAPISema, X_SEMA_OPTION_WAIT) == OSR_OK);
    }
}

static void UnLockVdecSemaphoreMtal(void)
{
    if (_hAPISema)
    {
        VERIFY(x_sema_unlock(_hAPISema) == OSR_OK);
    }
}

#if 1
typedef struct _MTSWDMX_RANGE_LIST_T
{
    MM_RANGE_INFO_T *pt_range;
    struct _MTSWDMX_RANGE_LIST_T * pt_next;
    struct _MTSWDMX_RANGE_LIST_T * pt_prev;
}   MTSWDMX_RANGE_LIST_T;

#define HDCP_KEY_LENGTH_MAX          512


static UINT8  g_tHDCPKey[HDCP_KEY_LENGTH_MAX];

static MTSWDMX_RANGE_LIST_T* g_pt_range_list[SWDMX_MAX_ES] = {0};

static VOID _MtSwDmx_add_range_list(UINT8 u1SrcId, MM_RANGE_INFO_T *pt_range)
{
    MTSWDMX_RANGE_LIST_T* pt_temp = NULL;

    if (NULL == pt_range || u1SrcId >= SWDMX_MAX_ES)
    {
        return;
    }

    pt_temp = (MTSWDMX_RANGE_LIST_T*)x_mem_alloc(sizeof(MTSWDMX_RANGE_LIST_T));
    if(!pt_temp)
    {
        LOG(1, "_MtSwDmx_add_range_list x_mem_alloc fail\n");
        ASSERT(0);
        return;
    }
    x_memset(pt_temp,0,sizeof(MTSWDMX_RANGE_LIST_T));

    pt_temp->pt_range = pt_range;

    pt_temp->pt_next = g_pt_range_list[u1SrcId];

    if(g_pt_range_list[u1SrcId] != NULL)
    {
        g_pt_range_list[u1SrcId]->pt_prev = pt_temp;
    }

    g_pt_range_list[u1SrcId] = pt_temp;

    return;
}

static VOID _MtSwDmx_recursion_free_dec_info_enc_str(VOID *pvPtr)
{
    if (pvPtr == NULL)
    {
        return;
    }

    // For now condition (pvPtr != NULL) is only possible for mkv
    //if (_arDmxMMInfo[u1SrcId].rContainerSubType.u.e_contnr_subtype == MEDIA_CONTNR_SUBTYPE_MATROSKA)
    {
        MINFO_MKV_CONTENT_ENCODING_T *ptCntEncoding = (MINFO_MKV_CONTENT_ENCODING_T *)pvPtr;
        _MtSwDmx_recursion_free_dec_info_enc_str(ptCntEncoding->pt_next);
        if (ptCntEncoding->pui1_comp_settings_data)
        {
            x_mem_free(ptCntEncoding->pui1_comp_settings_data);
        }
    }

    x_mem_free(pvPtr);
}


static VOID _MtSwDmx_recursion_free_dec_info(MM_RANGE_DECODER_T *ptDecoderInfo)
{
    if (ptDecoderInfo == NULL)
    {
        return;
    }

    _MtSwDmx_recursion_free_dec_info(ptDecoderInfo->pt_next);
    _MtSwDmx_recursion_free_dec_info_enc_str(ptDecoderInfo->pv_encoding_string);
    if (ptDecoderInfo->pui1_codec_private)
    {
        x_mem_free(ptDecoderInfo->pui1_codec_private);
    }
    x_mem_free(ptDecoderInfo);
}

static VOID _MtSwDmx_recursion_free_data(MM_RANGE_ELMT_IDXTBL_T *pt_data)
{
    if(NULL != pt_data)
    {
        /* Deal pv_next_tbl data. */
        _MtSwDmx_recursion_free_data(pt_data->pv_next_tbl);

        /* Deal pt_next_partial data. */
        _MtSwDmx_recursion_free_data(pt_data->pt_next_partial);

        /* Deal pt_pb_idx_tbl_entry data. */
        if(pt_data->u.pt_pb_idx_tbl_entry != NULL)
        {
            x_mem_free(pt_data->u.pt_pb_idx_tbl_entry);
        }

        x_mem_free(pt_data);
    }

    return;
}
#ifdef CENC_MARLIN_SUPPORT
static VOID _MtSwDmx_recursion_free_enctbl_data(MM_RANGE_ELMT_ENCTBL_T *pt_data)
{
    UINT32  ui4_i = 0;
    if(NULL != pt_data)
    {
        _MtSwDmx_recursion_free_enctbl_data(pt_data->pv_next_tbl);

        for(ui4_i = 0; ui4_i < pt_data->ui4_number_of_entry; ui4_i++)
        {
            if(pt_data->pt_sample_entry->ui4_subsample_cnt != 0)
            {
                x_mem_free((pt_data->pt_sample_entry+ui4_i)->pt_subsample_info);
            }
        }
        x_mem_free(pt_data->pt_sample_entry);
    }
    return;
}
#endif

static VOID _MtSwDmx_free_range_info(MM_RANGE_INFO_T* pt_range)
{
    if(NULL == pt_range)
    {
        return;
    }

#if SUPPORT_MULTI_INFO
    if (pt_range->pt_next_scale)
    {
        _MtSwDmx_free_range_info(pt_range->pt_next_scale);
    }
#endif

    _MtSwDmx_recursion_free_dec_info(pt_range->pt_aud_decoder_info);
    _MtSwDmx_recursion_free_dec_info(pt_range->pt_vid_decoder_info);
   if(pt_range->t_drm_info_drv.t_drm_type != DRM_TYPE_PLAYREADY_DRM)
   {
        if(pt_range->t_drm_info_drv.u.t_divx_drm_info_drv.pt_divx_drm_crypto_info != NULL)
        {
            x_mem_free(pt_range->t_drm_info_drv.u.t_divx_drm_info_drv.pt_divx_drm_crypto_info);
        }
    }

    if(pt_range->pt_track_tm_scale_lst != NULL)
    {
        x_mem_free(pt_range->pt_track_tm_scale_lst);
    }

    _MtSwDmx_recursion_free_data(pt_range->pt_idxtbl_lst);
    #ifdef CENC_MARLIN_SUPPORT
    _MtSwDmx_recursion_free_enctbl_data(pt_range->pt_enctbl_lst);
    #endif

    x_mem_free(pt_range);

    return;
}

static VOID _MtSwDmx_free_range_node(MTSWDMX_RANGE_LIST_T* pt_node)
{
    if(NULL == pt_node)
    {
        return;
    }

    _MtSwDmx_free_range_info(pt_node->pt_range);

    x_mem_free(pt_node);

    return;
}

static VOID _MtSwDmx_remove_range_list(UINT8 u1SrcId, UINT32 ui4_range_id)
{
    MTSWDMX_RANGE_LIST_T* pt_temp;

    if (u1SrcId >= SWDMX_MAX_ES)
    {
        return;
    }

    pt_temp=g_pt_range_list[u1SrcId];
    while(pt_temp != NULL)
    {
        if(pt_temp->pt_range->ui4_range_id == ui4_range_id)
        {
            if(pt_temp->pt_prev != NULL)
            {
                pt_temp->pt_prev->pt_next = pt_temp->pt_next;
            }
            else
            {
                g_pt_range_list[u1SrcId] = pt_temp->pt_next;
            }

            if(pt_temp->pt_next != NULL)
            {
                pt_temp->pt_next->pt_prev = pt_temp->pt_prev;
            }

            _MtSwDmx_free_range_node(pt_temp);

            break;
        }

        pt_temp = pt_temp->pt_next;
    }

    return;
}


static INT32 _MtSwDmx_recursion_data(MM_RANGE_ELMT_IDXTBL_T **ppt_data, MM_RANGE_ELMT_IDXTBL_T *ppt_data_user)
{
    VOID *pv_idx_tbl_entry_user = NULL;
    MM_RANGE_ELMT_IDXTBL_T *pt_next_partial_user = NULL;
    MM_RANGE_ELMT_IDXTBL_T *pt_next_tbl_user = NULL;
    UINT32 ui4_entry_size = 0;
    INT32 i4_ret = MTR_OK;

    if (ppt_data_user)
    {
        *ppt_data = x_mem_alloc(sizeof(MM_RANGE_ELMT_IDXTBL_T));
        if (*ppt_data == NULL)
        {
            return MTR_NOT_OK;
        }

        COPY_FROM_USER_SIZE(*ppt_data, ppt_data_user, sizeof(MM_RANGE_ELMT_IDXTBL_T));
        pv_idx_tbl_entry_user = (*ppt_data)->u.pv_idx_tbl_entry;
        (*ppt_data)->u.pv_idx_tbl_entry = NULL;
        pt_next_partial_user = (*ppt_data)->pt_next_partial;
        (*ppt_data)->pt_next_partial = NULL;
        pt_next_tbl_user= (MM_RANGE_ELMT_IDXTBL_T *)(*ppt_data)->pv_next_tbl;
        (*ppt_data)->pv_next_tbl = NULL;

        /* Deal pt_pb_idx_tbl_entry/pt_ts_idx_tbl_entry data. */
        if((*ppt_data)->e_tbl_type == MM_RANGE_ELEMT_IDXTBL_TYPE_TIMESHIFT)
        {
            if (pv_idx_tbl_entry_user)
            {
                (*ppt_data)->u.pt_ts_idx_tbl_entry = x_mem_calloc(1,sizeof(MM_RANGE_ELEMT_IDXTBL_TS_ENTRY_T));

                if ((*ppt_data)->u.pt_ts_idx_tbl_entry)
                {
                    COPY_FROM_USER_SIZE((*ppt_data)->u.pt_ts_idx_tbl_entry, pv_idx_tbl_entry_user,
                        sizeof(MM_RANGE_ELEMT_IDXTBL_TS_ENTRY_T));
                }
                else /* Error handling and error logs here */
                {
                    _MtSwDmx_recursion_free_data(*ppt_data);
                    *ppt_data = NULL;
                    return MTR_NOT_OK;
                }
            }
        }
        else
        {
            if((*ppt_data)->ui4_number_of_entry > 0 && pv_idx_tbl_entry_user)
            {
                if((*ppt_data)->ui4_entry_size)
                {
                    ui4_entry_size = (*ppt_data)->ui4_entry_size;
                }
                else
                {
                    ui4_entry_size = sizeof(MM_RANGE_ELEMT_IDXTBL_PB_ENTRY_T);
                }
               (*ppt_data)->u.pt_pb_idx_tbl_entry = x_mem_calloc((*ppt_data)->ui4_number_of_entry, ui4_entry_size);

                if ((*ppt_data)->u.pt_pb_idx_tbl_entry)
                {
                    COPY_FROM_USER_SIZE((*ppt_data)->u.pt_pb_idx_tbl_entry, pv_idx_tbl_entry_user,
                        ui4_entry_size * (*ppt_data)->ui4_number_of_entry);
                }
                else /* Error handling and error logs here */
                {
                    _MtSwDmx_recursion_free_data(*ppt_data);
                    *ppt_data = NULL;
                    return MTR_NOT_OK;
                }
            }
        }

        /* Deal pt_next_partial data. */
        if(MTR_OK != (i4_ret = _MtSwDmx_recursion_data(&((*ppt_data)->pt_next_partial), pt_next_partial_user)))
        {
            _MtSwDmx_recursion_free_data(*ppt_data);
            *ppt_data = NULL;
            return MTR_NOT_OK;
        }

        /* Deal pv_next_tbl data. */
        if(MTR_OK != (i4_ret = _MtSwDmx_recursion_data((MM_RANGE_ELMT_IDXTBL_T **)(&((*ppt_data)->pv_next_tbl)), pt_next_tbl_user)))
        {
            _MtSwDmx_recursion_free_data(*ppt_data);
            *ppt_data = NULL;
            return MTR_NOT_OK;
        }
    }

    return MTR_OK;

}

#ifdef CENC_MARLIN_SUPPORT
static INT32 _MtSwDmx_recursion_enctbl_data(MM_RANGE_ELMT_ENCTBL_T **ppt_data, MM_RANGE_ELMT_ENCTBL_T *pt_data_user)
{
    INT32 i4_ret  = MTR_OK;
    UINT32  ui4_i = 0;
    
    MM_RANGE_ELMT_ENCTBL_T* pt_next_tbl_user = NULL;
    MM_RANGE_ELMT_ENCTBL_T* pt_data_kernel = NULL;

    MM_RANGE_ELMET_ENCTBL_SAMPLE_ENTRY_T* pt_tbl_entry_user = NULL;

    //printk("<0>""ppt_data 0x%p, *ppt_data 0x%p, pt_data_user 0x%p.\r\n", 
            //ppt_data, *ppt_data, pt_data_user);
    if(pt_data_user)
    {
        *ppt_data = x_mem_alloc(sizeof(MM_RANGE_ELMT_ENCTBL_T));
        if (*ppt_data == NULL)
        {
            return MTR_NOT_OK;
        }

        pt_data_kernel = *ppt_data;
        
        COPY_FROM_USER_SIZE(pt_data_kernel, pt_data_user, sizeof(MM_RANGE_ELMT_ENCTBL_T));
        pt_tbl_entry_user = pt_data_user->pt_sample_entry;
        pt_next_tbl_user  = pt_data_user->pv_next_tbl;

        //printk("<0>""cenc entry number %d, pt_tbl_entry_user 0x%p, ", pt_data_user->ui4_number_of_entry, pt_tbl_entry_user);
        if(pt_data_user->ui4_number_of_entry > 0 && pt_tbl_entry_user)
        {
            pt_data_kernel->pt_sample_entry = x_mem_calloc(pt_data_user->ui4_number_of_entry, 
                                                        sizeof(MM_RANGE_ELMET_ENCTBL_SAMPLE_ENTRY_T));
            if(pt_data_kernel->pt_sample_entry)
            {
                COPY_FROM_USER_SIZE(pt_data_kernel->pt_sample_entry, pt_tbl_entry_user, 
                                        pt_data_user->ui4_number_of_entry*sizeof(MM_RANGE_ELMET_ENCTBL_SAMPLE_ENTRY_T));

                for(ui4_i=0; ui4_i < pt_data_user->ui4_number_of_entry; ui4_i++)
                {
                    if((pt_data_user->pt_sample_entry+ui4_i)->ui4_subsample_cnt > 0)
                    {
                        (pt_data_kernel->pt_sample_entry+ui4_i)->pt_subsample_info = 
                                    x_mem_calloc((pt_data_kernel->pt_sample_entry+ui4_i)->ui4_subsample_cnt, 
                                                    sizeof(MM_RANGE_ELMET_ENCTBL_SUBSAMPLE_INFO));
                        if((pt_data_kernel->pt_sample_entry+ui4_i)->pt_subsample_info)
                        {
                            COPY_FROM_USER_SIZE((pt_data_kernel->pt_sample_entry+ui4_i)->pt_subsample_info, 
                                                    (pt_data_user->pt_sample_entry+ui4_i)->pt_subsample_info, 
                                                        (pt_data_kernel->pt_sample_entry+ui4_i)->ui4_subsample_cnt*
                                                         sizeof(MM_RANGE_ELMET_ENCTBL_SUBSAMPLE_INFO));  
                        }
                        else
                        {
                            //printk("%s, line %d, allocate memory error\n", __FUNCTION__, __LINE__);
                            _MtSwDmx_recursion_free_enctbl_data(*ppt_data);
                            *ppt_data = NULL;
                            return MTR_NOT_OK;
                        }
                    }
                }
            }
            else
            {
                //printk("%s, line %d, allocate memory error\n", __FUNCTION__, __LINE__);
                _MtSwDmx_recursion_free_enctbl_data(*ppt_data);
                *ppt_data = NULL;
                return MTR_NOT_OK;
            }
            
        }

        if(ppt_data != NULL)
        {
            //printk("<0>""ppt_data 0x%p, *ppt_data->pv_nexttbl 0x%p, pt_next_tbl_user %p.\r\n", ppt_data, ((*ppt_data)->pv_next_tbl), pt_next_tbl_user);
            if(MTR_OK != (i4_ret = _MtSwDmx_recursion_enctbl_data((MM_RANGE_ELMT_ENCTBL_T **)(&((*ppt_data)->pv_next_tbl)), pt_next_tbl_user)))
            {
                //_MtSwDmx_recursion_free_data(*ppt_data);
                _MtSwDmx_recursion_free_enctbl_data(*ppt_data);
                *ppt_data = NULL;
                return MTR_NOT_OK;
            }
        }
        
        
    }
    
    return MTR_OK;
}
#endif
static INT32 _MtSwDmx_pad_data_from_usr_struct(
    MM_RANGE_INFO_T** pprDmxRange,
    const VOID* pv_set_info,
    BOOL fgCopyFromUsr)
{
#if SUPPORT_MULTI_INFO
    VOID *pt_next_scale_user = NULL;
#endif
    VOID *pt_track_tm_scale_lst_user = NULL;
    VOID *pt_divx_drm_crypto_info_user = NULL;
    VOID *pt_vid_decoder_info_user = NULL;
    VOID *pt_aud_decoder_info_user = NULL;
    MM_RANGE_ELMT_IDXTBL_T *pt_idxtbl_lst_user = NULL;
    #ifdef CENC_MARLIN_SUPPORT
    MM_RANGE_ELMT_ENCTBL_T *pt_enctbl_lst_user = NULL;
    #endif
    INT32 i4_ret = MTR_OK;

    if(pprDmxRange == NULL || pv_set_info == NULL)
    {
        return MTR_ERR_INV;
    }

    *pprDmxRange = (MM_RANGE_INFO_T*)x_mem_alloc(sizeof(MM_RANGE_INFO_T));

    if (*pprDmxRange == NULL)
    {
        return MTR_NOT_OK;
    }

    if (fgCopyFromUsr)
    {
        COPY_FROM_USER_SIZE(*pprDmxRange,(VOID *)pv_set_info,sizeof(MM_RANGE_INFO_T));
    }
    else
    {
        x_memcpy(*pprDmxRange,pv_set_info,sizeof(MM_RANGE_INFO_T));
    }
    if((*pprDmxRange)->t_drm_info_drv.t_drm_type == DRM_TYPE_PLAYREADY_DRM)
    {
        printf("_MtSwDmx_pad_data_from_usr_struct WMDrm Info: Drm type :%d,Key Size:%d\n",
            (UINT32)(*pprDmxRange)->t_drm_info_drv.u.t_playready_crypto_info.e_crypto_key_type,
            (UINT32)(*pprDmxRange)->t_drm_info_drv.u.t_playready_crypto_info.ui4_crypto_key_size);
    }

    pt_track_tm_scale_lst_user = (*pprDmxRange)->pt_track_tm_scale_lst; // TODO: it seems that pt_track_tm_scale_lst is not used by driver
    (*pprDmxRange)->pt_track_tm_scale_lst = NULL;
    if((*pprDmxRange)->t_drm_info_drv.t_drm_type != DRM_TYPE_PLAYREADY_DRM)
    {
        pt_divx_drm_crypto_info_user = (*pprDmxRange)->t_drm_info_drv.u.t_divx_drm_info_drv.pt_divx_drm_crypto_info;
        (*pprDmxRange)->t_drm_info_drv.u.t_divx_drm_info_drv.pt_divx_drm_crypto_info = NULL;
    }
    pt_vid_decoder_info_user = (*pprDmxRange)->pt_vid_decoder_info;
    (*pprDmxRange)->pt_vid_decoder_info = NULL;
    pt_aud_decoder_info_user = (*pprDmxRange)->pt_aud_decoder_info;
    (*pprDmxRange)->pt_aud_decoder_info = NULL;
    pt_idxtbl_lst_user = (*pprDmxRange)->pt_idxtbl_lst;
    (*pprDmxRange)->pt_idxtbl_lst = NULL;
#ifdef CENC_MARLIN_SUPPORT
    pt_enctbl_lst_user = (*pprDmxRange)->pt_enctbl_lst;
    (*pprDmxRange)->pt_enctbl_lst = NULL;
#endif
#if SUPPORT_MULTI_INFO
    pt_next_scale_user = (*pprDmxRange)->pt_next_scale;
    (*pprDmxRange)->pt_next_scale = NULL;
    if (pt_next_scale_user)
    {
        i4_ret = _MtSwDmx_pad_data_from_usr_struct(&(*pprDmxRange)->pt_next_scale,
            pt_next_scale_user, TRUE);
        if (MTR_OK != i4_ret)
        {
            _MtSwDmx_free_range_info(*pprDmxRange);
            *pprDmxRange = NULL;
            return i4_ret;
        }
    }
#endif

    i4_ret = _MtSwDmx_recursion_data(&((*pprDmxRange)->pt_idxtbl_lst), pt_idxtbl_lst_user);
    if (MTR_OK != i4_ret)
    {
        _MtSwDmx_free_range_info(*pprDmxRange);
        *pprDmxRange = NULL;
        return i4_ret;
    }
#ifdef CENC_MARLIN_SUPPORT
    i4_ret = _MtSwDmx_recursion_enctbl_data(&((*pprDmxRange)->pt_enctbl_lst), pt_enctbl_lst_user);
    if (MTR_OK != i4_ret)
    {
        _MtSwDmx_free_range_info(*pprDmxRange);
        *pprDmxRange = NULL;
        return i4_ret;
    }
#endif

    if ((*pprDmxRange)->ui4_track_tm_scale_num > 0 && pt_track_tm_scale_lst_user)
    {
        (*pprDmxRange)->pt_track_tm_scale_lst = x_mem_calloc((*pprDmxRange)->ui4_track_tm_scale_num,
            sizeof(MM_RANGE_TRACK_TM_SCALE_T));
        if ((*pprDmxRange)->pt_track_tm_scale_lst)
        {
            COPY_FROM_USER_SIZE((*pprDmxRange)->pt_track_tm_scale_lst, pt_track_tm_scale_lst_user,
                               sizeof(MM_RANGE_TRACK_TM_SCALE_T) * ((*pprDmxRange)->ui4_track_tm_scale_num));
        }
        else /* Error handling and error logs here */
        {
            _MtSwDmx_free_range_info(*pprDmxRange);
            *pprDmxRange = NULL;
            return MTR_NOT_OK;
        }
    }

    if (pt_divx_drm_crypto_info_user)
    {
        (*pprDmxRange)->t_drm_info_drv.u.t_divx_drm_info_drv.pt_divx_drm_crypto_info = x_mem_alloc(sizeof(DIVX_DRM_CRYPTO_INFO_T));

        if ((*pprDmxRange)->t_drm_info_drv.u.t_divx_drm_info_drv.pt_divx_drm_crypto_info)
        {
            COPY_FROM_USER_SIZE((*pprDmxRange)->t_drm_info_drv.u.t_divx_drm_info_drv.pt_divx_drm_crypto_info,
                               pt_divx_drm_crypto_info_user, sizeof(DIVX_DRM_CRYPTO_INFO_T));
        }
        else /* Error handling and error logs here */
        {
            _MtSwDmx_free_range_info(*pprDmxRange);
            *pprDmxRange = NULL;
            return MTR_NOT_OK;
        }
    }

    if (pt_vid_decoder_info_user)
    {
        MM_RANGE_DECODER_T *ptDecoderInfo;

        (*pprDmxRange)->pt_vid_decoder_info = x_mem_alloc(sizeof(MM_RANGE_DECODER_T));
        if ((*pprDmxRange)->pt_vid_decoder_info)
        {
            COPY_FROM_USER_SIZE((*pprDmxRange)->pt_vid_decoder_info,
                               pt_vid_decoder_info_user, sizeof(MM_RANGE_DECODER_T));
        }
        else /* Error handling and error logs here */
        {
            _MtSwDmx_free_range_info(*pprDmxRange);
            *pprDmxRange = NULL;
            return MTR_NOT_OK;
        }

        ptDecoderInfo = (*pprDmxRange)->pt_vid_decoder_info;
        while (ptDecoderInfo)
        {
            UINT32 u4CpyLen;
            VOID *pv_encoding_string_user = ptDecoderInfo->pv_encoding_string;
            VOID *pui1_codec_private_user = ptDecoderInfo->pui1_codec_private;
            VOID *pt_next_user = ptDecoderInfo->pt_next;
            ptDecoderInfo->pv_encoding_string = NULL;
            ptDecoderInfo->pui1_codec_private = NULL;
            ptDecoderInfo->pt_next = NULL;

            // For now condition (ptDecoderInfo != NULL) is only possible for mkv
            //if (_arDmxMMInfo[u1SrcId].rContainerSubType.u.e_contnr_subtype == MEDIA_CONTNR_SUBTYPE_MATROSKA)
            if (ptDecoderInfo->b_has_encoding && pv_encoding_string_user)
            {
                UINT32 u4Idx, u4Total;
                MINFO_MKV_CONTENT_ENCODING_T *ptCntEncoding;

                ptDecoderInfo->pv_encoding_string = x_mem_alloc(sizeof(MINFO_MKV_CONTENT_ENCODING_T));
                if (ptDecoderInfo->pv_encoding_string)
                {
                    COPY_FROM_USER_SIZE(ptDecoderInfo->pv_encoding_string,
                                       pv_encoding_string_user, sizeof(MINFO_MKV_CONTENT_ENCODING_T));
                }
                else /* Error handling and error logs here */
                {
                    _MtSwDmx_free_range_info(*pprDmxRange);
                    *pprDmxRange = NULL;
                    return MTR_NOT_OK;
                }

                u4Total = ptDecoderInfo->ui4_encoding_num;
                ptCntEncoding = (MINFO_MKV_CONTENT_ENCODING_T *)ptDecoderInfo->pv_encoding_string;
                for (u4Idx = 0; ptCntEncoding && u4Idx < u4Total; u4Idx++, ptCntEncoding = ptCntEncoding->pt_next)
                {
                    VOID *pui1_comp_settings_data_user = ptCntEncoding->pui1_comp_settings_data;
                    VOID *pt_cnt_enc_next_user = ptCntEncoding->pt_next;
                    ptCntEncoding->pui1_comp_settings_data = NULL;
                    ptCntEncoding->pt_next = NULL;

                    ASSERT(0 == (ptCntEncoding->ui8_comp_settings_length >> 32));
                    u4CpyLen = ptCntEncoding->ui8_comp_settings_length;
                    if (pui1_comp_settings_data_user && u4CpyLen)
                    {
                        ptCntEncoding->pui1_comp_settings_data = x_mem_alloc(u4CpyLen);
                        if (ptCntEncoding->pui1_comp_settings_data)
                        {
                            COPY_FROM_USER_SIZE(ptCntEncoding->pui1_comp_settings_data,
                                               pui1_comp_settings_data_user, u4CpyLen);
                        }
                        else /* Error handling and error logs here */
                        {
                            _MtSwDmx_free_range_info(*pprDmxRange);
                            *pprDmxRange = NULL;
                            return MTR_NOT_OK;
                        }
                    }

                    if (pt_cnt_enc_next_user)
                    {
                        ptCntEncoding->pt_next = x_mem_alloc(sizeof(MINFO_MKV_CONTENT_ENCODING_T));
                        if (ptCntEncoding->pt_next)
                        {
                            COPY_FROM_USER_SIZE(ptCntEncoding->pt_next,
                                               pt_cnt_enc_next_user, sizeof(MINFO_MKV_CONTENT_ENCODING_T));
                        }
                        else /* Error handling and error logs here */
                        {
                            _MtSwDmx_free_range_info(*pprDmxRange);
                            *pprDmxRange = NULL;
                            return MTR_NOT_OK;
                        }
                    }
                }
            }

            ASSERT(0 == (ptDecoderInfo->ui8_codec_private_len >> 32));
            u4CpyLen = ptDecoderInfo->ui8_codec_private_len;
            if (pui1_codec_private_user && u4CpyLen)
            {
                ptDecoderInfo->pui1_codec_private = x_mem_alloc(u4CpyLen);
                if (ptDecoderInfo->pui1_codec_private)
                {
                    COPY_FROM_USER_SIZE(ptDecoderInfo->pui1_codec_private,
                                       pui1_codec_private_user, u4CpyLen);
                }
                else /* Error handling and error logs here */
                {
                    _MtSwDmx_free_range_info(*pprDmxRange);
                    *pprDmxRange = NULL;
                    return MTR_NOT_OK;
                }
            }

            if (pt_next_user)
            {
                ptDecoderInfo->pt_next = x_mem_alloc(sizeof(MM_RANGE_DECODER_T));
                if (ptDecoderInfo->pt_next)
                {
                    COPY_FROM_USER_SIZE(ptDecoderInfo->pt_next,
                                       pt_next_user, sizeof(MM_RANGE_DECODER_T));
                }
                else /* Error handling and error logs here */
                {
                    _MtSwDmx_free_range_info(*pprDmxRange);
                    *pprDmxRange = NULL;
                    return MTR_NOT_OK;
                }
            }

            ptDecoderInfo = ptDecoderInfo->pt_next;
        }
    }

    if (pt_aud_decoder_info_user)
    {
        MM_RANGE_DECODER_T *ptDecoderInfo;

        (*pprDmxRange)->pt_aud_decoder_info = x_mem_alloc(sizeof(MM_RANGE_DECODER_T));
        if ((*pprDmxRange)->pt_aud_decoder_info)
        {
            COPY_FROM_USER_SIZE((*pprDmxRange)->pt_aud_decoder_info,
                               pt_aud_decoder_info_user, sizeof(MM_RANGE_DECODER_T));
        }
        else /* Error handling and error logs here */
        {
            _MtSwDmx_free_range_info(*pprDmxRange);
            *pprDmxRange = NULL;
            return MTR_NOT_OK;
        }

        ptDecoderInfo = (*pprDmxRange)->pt_aud_decoder_info;
        while (ptDecoderInfo)
        {
            UINT32 u4CpyLen;
            VOID *pv_encoding_string_user = ptDecoderInfo->pv_encoding_string;
            VOID *pui1_codec_private_user = ptDecoderInfo->pui1_codec_private;
            VOID *pt_next_user = ptDecoderInfo->pt_next;
            ptDecoderInfo->pv_encoding_string = NULL;
            ptDecoderInfo->pui1_codec_private = NULL;
            ptDecoderInfo->pt_next = NULL;

            // For now condition (ptDecoderInfo != NULL) is only possible for mkv
            //if (_arDmxMMInfo[u1SrcId].rContainerSubType.u.e_contnr_subtype == MEDIA_CONTNR_SUBTYPE_MATROSKA)
            if (ptDecoderInfo->b_has_encoding && pv_encoding_string_user)
            {
                UINT32 u4Idx, u4Total;
                MINFO_MKV_CONTENT_ENCODING_T *ptCntEncoding;

                ptDecoderInfo->pv_encoding_string = x_mem_alloc(sizeof(MINFO_MKV_CONTENT_ENCODING_T));
                if (ptDecoderInfo->pv_encoding_string)
                {
                    COPY_FROM_USER_SIZE(ptDecoderInfo->pv_encoding_string,
                                       pv_encoding_string_user, sizeof(MINFO_MKV_CONTENT_ENCODING_T));
                }
                else /* Error handling and error logs here */
                {
                    _MtSwDmx_free_range_info(*pprDmxRange);
                    *pprDmxRange = NULL;
                    return MTR_NOT_OK;
                }

                u4Total = ptDecoderInfo->ui4_encoding_num;
                ptCntEncoding = (MINFO_MKV_CONTENT_ENCODING_T *)ptDecoderInfo->pv_encoding_string;
                for (u4Idx = 0; ptCntEncoding && u4Idx < u4Total; u4Idx++, ptCntEncoding = ptCntEncoding->pt_next)
                {
                    VOID *pui1_comp_settings_data_user = ptCntEncoding->pui1_comp_settings_data;
                    VOID *pt_cnt_enc_next_user = ptCntEncoding->pt_next;
                    ptCntEncoding->pui1_comp_settings_data = NULL;
                    ptCntEncoding->pt_next = NULL;

                    ASSERT(0 == (ptCntEncoding->ui8_comp_settings_length >> 32));
                    u4CpyLen = ptCntEncoding->ui8_comp_settings_length;
                    if (pui1_comp_settings_data_user && u4CpyLen)
                    {
                        ptCntEncoding->pui1_comp_settings_data = x_mem_alloc(u4CpyLen);
                        if (ptCntEncoding->pui1_comp_settings_data)
                        {
                            COPY_FROM_USER_SIZE(ptCntEncoding->pui1_comp_settings_data,
                                               pui1_comp_settings_data_user, u4CpyLen);
                        }
                        else /* Error handling and error logs here */
                        {
                            _MtSwDmx_free_range_info(*pprDmxRange);
                            *pprDmxRange = NULL;
                            return MTR_NOT_OK;
                        }
                    }

                    if (pt_cnt_enc_next_user)
                    {
                        ptCntEncoding->pt_next = x_mem_alloc(sizeof(MINFO_MKV_CONTENT_ENCODING_T));
                        if (ptCntEncoding->pt_next)
                        {
                            COPY_FROM_USER_SIZE(ptCntEncoding->pt_next,
                                               pt_cnt_enc_next_user, sizeof(MINFO_MKV_CONTENT_ENCODING_T));
                        }
                        else /* Error handling and error logs here */
                        {
                            _MtSwDmx_free_range_info(*pprDmxRange);
                            *pprDmxRange = NULL;
                            return MTR_NOT_OK;
                        }
                    }
                }
            }

            ASSERT(0 == (ptDecoderInfo->ui8_codec_private_len >> 32));
            u4CpyLen = ptDecoderInfo->ui8_codec_private_len;
            if (pui1_codec_private_user && u4CpyLen)
            {
                ptDecoderInfo->pui1_codec_private = x_mem_alloc(u4CpyLen);
                if (ptDecoderInfo->pui1_codec_private)
                {
                    COPY_FROM_USER_SIZE(ptDecoderInfo->pui1_codec_private,
                                       pui1_codec_private_user, u4CpyLen);
                }
                else /* Error handling and error logs here */
                {
                    _MtSwDmx_free_range_info(*pprDmxRange);
                    *pprDmxRange = NULL;
                    return MTR_NOT_OK;
                }
            }

            if (pt_next_user)
            {
                ptDecoderInfo->pt_next = x_mem_alloc(sizeof(MM_RANGE_DECODER_T));
                if (ptDecoderInfo->pt_next)
                {
                    COPY_FROM_USER_SIZE(ptDecoderInfo->pt_next,
                                       pt_next_user, sizeof(MM_RANGE_DECODER_T));
                }
                else /* Error handling and error logs here */
                {
                    _MtSwDmx_free_range_info(*pprDmxRange);
                    *pprDmxRange = NULL;
                    return MTR_NOT_OK;
                }
            }

            ptDecoderInfo = ptDecoderInfo->pt_next;
        }
    }

    return MTR_OK;
}
#endif

static void _MTSWDMX_DmxNfy(void* pvNfyTag, SWDMX_COND_T eNfyCond, UINT32 u4Data1, UINT32 u4Data2)
{
    MTSWDMX_DMXNFY_CB_T rCb;

    x_memset(&rCb, 0, sizeof(MTSWDMX_DMXNFY_CB_T));
    rCb.eNfyCond = (UINT32)eNfyCond;
    rCb.u4Data1 = u4Data1;
    rCb.u4Data2 = u4Data2;
    _CB_PutEvent(CB_MTAL_SWDMX_DMXNFY_TRIGGER, sizeof(MTSWDMX_DMXNFY_CB_T), (void*)&rCb);
}

static MT_RESULT_T _MTSWDMX_SetDmxNfy(unsigned long arg)
{
    UCHAR ucEsId;
    SWDMX_NFY_INFO_T rNfyInfo;

    ucEsId = (UCHAR)arg;
    MTSWDMX_PRINT("_MTSWDMX_SetDmxNfy %d\n",(int)ucEsId);

    //add driver implementation code here
    if(ucEsId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    rNfyInfo.pfSwdmxNfy = _MTSWDMX_DmxNfy;
    rNfyInfo.pvTag      = 0;

    if(SWDMX_SetDmxNfy(ucEsId, &rNfyInfo) != SWDMX_DRV_OK)
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

static BOOL _MTSWDMX_ScrambleStateNfy(BOOL fgIsVideo, UINT8 u1Pidx, DMX_NOTIFY_CODE_T eCode,
    UINT32 u4Data, const void* pvNotifyTag)
{
    CB_FCT_ID_T eFctId;
    MTSWDMX_SCRAMBLE_CB_T rCb;

    eFctId = (fgIsVideo) ? CB_MTAL_SWDMX_VID_SCRAMBLENFY_TRIGGER : CB_MTAL_SWDMX_AUD_SCRAMBLENFY_TRIGGER;

    x_memset(&rCb, 0, sizeof(MTSWDMX_SCRAMBLE_CB_T));
    rCb.u1Pidx  = (UINT8)u1Pidx;
    rCb.eCode   = (UINT8)eCode;
    rCb.u4Data  = (UINT32)u4Data;
    rCb.u4Data2 = (UINT32)pvNotifyTag;
    _CB_PutEvent(eFctId, sizeof(MTSWDMX_SCRAMBLE_CB_T), (void*)&rCb);

    return TRUE;
}

static BOOL _MTSWDMX_RangeNfyEx(VOID*            pv_nfy_tag,
                                       MM_RANGE_COND_T  e_nfy_cond,
                                       UINT32           ui4_data_1,
                                       UINT32           ui4_data_2,
                                       UINT32           ui4_data_3)
{
    MTSWDMX_RANGE_EX_CB_T rCb;

    x_memset(&rCb, 0, sizeof(MTSWDMX_RANGE_EX_CB_T));

    rCb.pv_nfy_tag  = (UINT32)pv_nfy_tag;
    rCb.e_nfy_cond   = (UINT32)e_nfy_cond;
    rCb.ui4_data_1  = (UINT32)ui4_data_1;
    rCb.ui4_data_2 = (UINT32)ui4_data_2;
    rCb.ui4_data_3 = (UINT32)ui4_data_3;

    _CB_PutEvent(CB_MTAL_SWDMX_RANGE_EX_NFY, sizeof(MTSWDMX_RANGE_EX_CB_T), (void*)&rCb);

    return TRUE;
}

static BOOL _MTSWDMX_VidChgNfy(VOID*            pv_nfy_tag,
                                       UINT32           ui4_data_1,
                                       UINT32           ui4_data_2,
                                       UINT32           ui4_data_3)
{
    MTSWDMX_PID_CHG_CB_T rCb;

    x_memset(&rCb, 0, sizeof(MTSWDMX_PID_CHG_CB_T));

    rCb.pv_nfy_tag  = (UINT32)pv_nfy_tag;
    rCb.ui4_data_1  = (UINT32)ui4_data_1;
    rCb.ui4_data_2 = (UINT32)ui4_data_2;
    rCb.ui4_data_3 = (UINT32)ui4_data_3;

    _CB_PutEvent(CB_MTAL_SWDMX_VID_CHG_NFY, sizeof(MTSWDMX_PID_CHG_CB_T), (void*)&rCb);

    return TRUE;
}
static BOOL _MTSWDMX_AudChgNfy(VOID*            pv_nfy_tag,
                                       UINT32           ui4_data_1,
                                       UINT32           ui4_data_2,
                                       UINT32           ui4_data_3)
{
    MTSWDMX_PID_CHG_CB_T rCb;

    x_memset(&rCb, 0, sizeof(MTSWDMX_PID_CHG_CB_T));

    rCb.pv_nfy_tag  = (UINT32)pv_nfy_tag;
    rCb.ui4_data_1  = (UINT32)ui4_data_1;
    rCb.ui4_data_2 = (UINT32)ui4_data_2;
    rCb.ui4_data_3 = (UINT32)ui4_data_3;

    _CB_PutEvent(CB_MTAL_SWDMX_AUD_CHG_NFY, sizeof(MTSWDMX_PID_CHG_CB_T), (void*)&rCb);

    return TRUE;
}

static BOOL _MTSWDMX_VID_ScrambleStateNfy(UINT8 u1Pidx, DMX_NOTIFY_CODE_T eCode,
    UINT32 u4Data, const void* pvNotifyTag)
{
    return _MTSWDMX_ScrambleStateNfy(TRUE, u1Pidx, eCode, u4Data, pvNotifyTag);
}

static BOOL _MTSWDMX_AUD_ScrambleStateNfy(UINT8 u1Pidx, DMX_NOTIFY_CODE_T eCode,
    UINT32 u4Data, const void* pvNotifyTag)
{
    return _MTSWDMX_ScrambleStateNfy(FALSE, u1Pidx, eCode, u4Data, pvNotifyTag);
}


//==============================================
static MT_RESULT_T _MTSWDMX_Init(unsigned long arg)
{
    UNUSED(arg);

    // for multi-process API lock
    if(_hAPISema == 0)
    {
        VERIFY(x_sema_create(&_hAPISema, X_SEMA_TYPE_MUTEX, 
            X_SEMA_STATE_UNLOCK) == OSR_OK);
    }
    
    SWDMX_Init();
    return MTR_OK;
}


static UINT8 MTSWDMX_GetDecType(UINT8 u1Type)
{
    switch(u1Type)
    {
    default:
        MTSWDMX_PRINT("MTSWDMX_GetDecType unknow %d\n",(int)u1Type);
        return eSWDMX_SET_MAX;
    case MTSWDMX_SET_VIDSEQHEADER:
        u1Type = eSWDMX_SET_VIDSEQHEADER;
        break;
    case MTSWDMX_SET_VIDEOTYPE:
        u1Type = eSWDMX_SET_VIDEOTYPE;
        break;
    case MTSWDMX_SET_SUBVIDEOTYPE:
        u1Type = eSWDMX_SET_SUBVIDEOTYPE;
        break;
    case MTSWDMX_SET_VIDEO2TYPE:
        u1Type = eSWDMX_SET_VIDEO2TYPE;
        break;
    case MTSWDMX_SET_AUDIOTYPE:
        u1Type = eSWDMX_SET_AUDIOTYPE;
        break;
    case MTSWDMX_SET_SYSTEMINFO:
        u1Type = eSWDMX_SET_SYSTEMINFO;
        break;
    case MTSWDMX_ENABLE_STREAM:
        u1Type = eSWDMX_ENABLE_STREAM;
        break;
    case MTSWDMX_DISABLE_STREAM:
        u1Type = eSWDMX_DISABLE_STREAM;
        break;
    case MTSWDMX_SET_RANGECALLBACK:
        u1Type = eSWDMX_SET_RANGECALLBACK;
        break;
    case MTSWDMX_SET_RANGECALLBACKEX:
        u1Type = eSWDMX_SET_RANGECALLBACKEX;
        break;
    case MTSWDMX_SET_CHK_PSI_BUF_SIZE:
        u1Type = eSWDMX_SET_CHK_PSI_BUF_SIZE;
        break; // should be MTSWDMX_CHK_PSI_BUF_SIZE
    case MTSWDMX_TOTAL_DURATION:
        u1Type = eSWDMX_TOTAL_DURATION;
        break;
    case MTSWDMX_SET_FEEDER_TEST_FUNCTION:
        u1Type = eSWDMX_SET_FEEDER_TEST_FUNCTION;
        break;
    case MTSWDMX_SET_SUBTITLECALLBACK:
        u1Type = eSWDMX_SET_SUBTITLECALLBACK;
        break;
    case MTSWDMX_GET_CURRENT_POSITION:
        u1Type = eSWDMX_GET_CURRENT_POSITION;
        break;
    case MTSWDMX_GET_FILE_OFFSET:
        u1Type = eSWDMX_GET_FILE_OFFSET;
        break;
    case MTSWDMX_GET_IS_TRICK:
        u1Type = eSWDMX_GET_IS_TRICK;
        break; // should be MTSWDMX_GET_IS_TRICKABLE
    case MTSWDMX_CONTAINER_TYPE:
        u1Type = eSWDMX_CONTAINER_TYPE;
        break;
    case MTSWDMX_GET_INTERNAL_INFO:
        u1Type = eSWDMX_GET_INTERNAL_INFO;
        break;
    case MTSWDMX_SET_REC_BUF_RANGE:
        u1Type = eSWDMX_SET_REC_BUF_RANGE;
        break;
    case MTSWDMX_PRINT_REC_BUF_STATUS:
        u1Type = eSWDMX_PRINT_REC_BUF_STATUS;
        break;
    case MTSWDMX_PRINT_INTERNAL_STATUS:
        u1Type = eSWDMX_PRINT_INTERNAL_STATUS;
        break;
    case MTSWDMX_SET_MONITOF_LBA:
        u1Type = eSWDMX_SET_MONITOF_LBA;
        break;
    case MTSWDMX_GET_CURRENT_TICK:
        u1Type = eSWDMX_GET_CURRENT_TICK;
        break;
    case MTSWDMX_GET_CURRENT_LBA:
        u1Type = eSWDMX_GET_CURRENT_LBA;
        break;
    case MTSWDMX_SET_NETFLIX_STREAM_ID:
        u1Type = eSWDMX_SET_NETFLIX_STREAM_ID;
        break;
    case MTSWDMX_SET_NETFLIX_EOS:
        u1Type = eSWDMX_SET_NETFLIX_EOS;
        break;
    case MTSWDMX_GET_CURRENT_TIME:
        u1Type = eSWDMX_GET_CURRENT_TIME;
        break;
    case MTSWDMX_PID_INDEX_START:
        u1Type = eSWDMX_PID_INDEX_START;
        break;
    case MTSWDMX_SET_TPT_VIDSEQHEADER:
        u1Type = eSWDMX_SET_TPT_VIDSEQHEADER;
        break;    //for divx plus
    case MTSWDMX_SET_AOUT_ENABLE:
        u1Type = eSWDMX_SET_AOUT_ENABLE;
        break;
    case MTSWDMX_SET_EOS:
        u1Type = eSWDMX_SET_EOS;
        break;
    case MTSWDMX_SET_VIDEO_DECRYPT:
        u1Type = eSWDMX_SET_VIDEO_DECRYPT;
        break;
    case MTSWDMX_SET_AUDIO_DECRYPT:
        u1Type = eSWDMX_SET_AUDIO_DECRYPT;
        break;
	case MTSWDMX_SET_AUDIO2_DECRYPT:
		u1Type = eSWDMX_SET_AUDIO2_DECRYPT;
		break;
	case MTSWDMX_SET_AUDIO3_DECRYPT:
		u1Type = eSWDMX_SET_AUDIO3_DECRYPT;
		break;
    case MTSWDMX_SET_VID_PID_IDX:
        u1Type = eSWDMX_SET_VID_PID_IDX;
        break;
    case MTSWDMX_SET_AUD_PID_IDX:
        u1Type = eSWDMX_SET_AUD_PID_IDX;
        break;
    case MTSWDMX_SET_PCR_PID_IDX:
        u1Type = eSWDMX_SET_PCR_PID_IDX;
        break;
    case MTSWDMX_GET_STC_OFFSET:
        u1Type = eSWDMX_GET_STC_OFFSET;
        break;
    case MTSWDMX_SET_VDEC_ID:
        u1Type = eSWDMX_SET_VDEC_ID;
        break;  // maybe we will have vdp id
    case MTSWDMX_SET_VDEC2_ID:
        u1Type = eSWDMX_SET_VDEC2_ID;
        break;  // maybe we will have vdp id
    case MTSWDMX_SET_ADEC_ID:
        u1Type = eSWDMX_SET_ADEC_ID;
        break;
    case MTSWDMX_SET_STC_ID:
        u1Type = eSWDMX_SET_STC_ID;
        break;
    case MTSWDMX_SET_B2R_ID:
        u1Type = eSWDMX_SET_B2R_ID;
        break;
    case MTSWDMX_SET_DMX_ID:
        u1Type = eSWDMX_SET_DMX_ID;
        break;
    case MTSWDMX_SET_SRC_TYPE:
        u1Type = eSWDMX_SET_SRC_TYPE;
        break;
    case MTSWDMX_GET_INTERNAL_BUF:
        u1Type = eSWDMX_GET_INTERNAL_BUF;
        break;
    case MTSWDMX_SET_PLAYBACK_THRESHOLD:
        u1Type = eSWDMX_SET_PLAYBACK_THRESHOLD;
        break;
    case MTSWDMX_GET_PLAYBACK_THRESHOLD:
        u1Type = eSWDMX_GET_PLAYBACK_THRESHOLD;
        break;
    case MTSWDMX_GET_TIME_TO_BYTES_INFO:
        u1Type = eSWDMX_GET_TIME_TO_BYTES_INFO;
        break;
    case MTSWDMX_SET_VID_SCRAMBLE_NFY:
        u1Type = eSWDMX_SET_VID_SCRAMBLE_NFY;
        break;
    case MTSWDMX_SET_AUD_SCRAMBLE_NFY:
        u1Type = eSWDMX_SET_AUD_SCRAMBLE_NFY;
        break;
    case MTSWDMX_SET_VID_CHG_CALLBACK:
        u1Type = eSWDMX_SET_VID_CHG_CALLBACK;
        break;
    case MTSWDMX_SET_AUD_CHG_CALLBACK:
        u1Type = eSWDMX_SET_AUD_CHG_CALLBACK;
        break;        
    case MTSWDMX_SET_TYPE_VID_PID_CHG_DONE:
        u1Type = eSWDMX_TYPE_VID_PID_CHG_DONE;
        break; 
    case MTSWDMX_SET_TYPE_AUD_PID_CHG_DONE:
        u1Type = eSWDMX_TYPE_AUD_PID_CHG_DONE;
        break;  
    case MTSWDMX_SET_TYPE_VID_CODEC_CHG_DONE:
        u1Type = eSWDMX_TYPE_VID_CODEC_CHG_DONE;
        break;    
    case MTSWDMX_SET_TYPE_AUD_CODEC_CHG_DONE:
        u1Type = eSWDMX_TYPE_AUD_CODEC_CHG_DONE;
        break;         
    case MTSWDMX_SET_SYSTEM_B:
        u1Type = eSWDMX_SET_SYSTEM_B;
        break;
    case MTSWDMX_GET_AUDIO_ACTIVE_PIDX:
        u1Type = eSWDMX_GET_AUDIO_ACTIVE_PIDX;
        break;
    case MTSWDMX_GET_VIDEO_PIDX:
        u1Type = eSWDMX_GET_VIDEO_PIDX;
        break;  // get current active video pidx
    case MTSWDMX_GET_AUDIO_PIDX:
        u1Type = eSWDMX_GET_AUDIO_PIDX;
        break;  // get current active audio pidx
    case MTSWDMX_SET_BAD_INTERLEAVED_INFO:
        u1Type = eSWDMX_SET_BAD_INTERLEAVED_INFO;
        break;
    case MTSWDMX_GET_BAD_INTERLEAVED_INFO:
        u1Type = eSWDMX_GET_BAD_INTERLEAVED_INFO;
        break;
    case MTSWDMX_SET_HDMVC_INFO:
        u1Type = eSWDMX_SET_HDMVC_INFO;
        break;
    case MTSWDMX_GET_HDMVC_INFO:
        u1Type = eSWDMX_GET_HDMVC_INFO;
        break;
    case MTSWDMX_SET_DBGINFO:
        u1Type = eSWDMX_SET_DBGINFO;
        break;
    case MTSWDMX_GET_DBGINFO:
        u1Type = eSWDMX_GET_DBGINFO;
        break;
    case MTSWDMX_GET_MM_SRC_TYPE:
        u1Type = eSWDMX_GET_MM_SRC_TYPE;
        break;
    case MTSWDMX_SET_FLUSHFEEDER:
        u1Type = eSWDMX_SET_FLUSHFEEDER;
        break;          
    case MTSWDMX_SET_DURATION:
        u1Type = eSWDMX_SET_DURATION;
        break; 
    case MTSWDMX_SET_FILE_SZ_UPDATE:
        u1Type = eSWDMX_SET_FILE_SZ_UPDATE;
        break;
    case MTSWDMX_SET_TYPE_PUSH_CHANGE_AUDIO:
        u1Type = eSWDMX_SET_FILE_PUSH_CHANGE_AUDIO;
        break;
    case MTSWDMX_SET_TYPE_PCP_LEN:
        u1Type = eSWDMX_SET_TYPE_PCP_LEN;
        break;		
    }

    return u1Type;
}

static MT_RESULT_T _MTSWDMX_SetInfo(unsigned long arg)
{
    UCHAR ucEsId;
    UINT8 u1Type;
    UINT32 u4Para1, u4Para2, u4Para3;
    UINT32 u4InternalBufAddr;
    MTAL_IOCTL_5ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

    ucEsId  = (UCHAR) rArg.ai4Arg[0];
    u1Type  = (UINT8) rArg.ai4Arg[1];
    u4Para1 = (UINT32)rArg.ai4Arg[2];
    u4Para2 = (UINT32)rArg.ai4Arg[3];
    u4Para3 = (UINT32)rArg.ai4Arg[4];

    MTSWDMX_PRINT("_MTSWDMX_SetInfo %d\n",(int)ucEsId);

    //add driver implementation code here
    if(ucEsId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    if (u1Type == MTSWDMX_SET_VID_SCRAMBLE_NFY)
    {
        u4Para1 = (UINT32)_MTSWDMX_VID_ScrambleStateNfy;
    }

    if (u1Type == MTSWDMX_SET_AUD_SCRAMBLE_NFY)
    {
        u4Para1 = (UINT32)_MTSWDMX_AUD_ScrambleStateNfy;
    }

    if (u1Type == MTSWDMX_SET_RANGECALLBACKEX)
    {
        u4Para1 = (UINT32)_MTSWDMX_RangeNfyEx;
    }

    if(u1Type == MTSWDMX_SET_VIDSEQHEADER)
    {
        UINT32 u4Align = 16;
        if(_SWDMX_GetInfo(ucEsId, eSWDMX_GET_INTERNAL_BUF, &u4InternalBufAddr, &u4Para2, &u4Align))
        {
            USR_SPACE_ACCESS_VALIDATE_ARG_K(u4Para1, u4Para2);
            COPY_FROM_USER_SIZE((UINT8*)u4InternalBufAddr, u4Para1, u4Para2);

            u4Para1 = u4InternalBufAddr;
        }
        else
        {
            return MTR_NOT_OK;
        }
    }
    if(u1Type == MTSWDMX_SET_TPT_VIDSEQHEADER)
    {
        UINT32 u4Align = 16;
        if(_SWDMX_GetInfo(ucEsId, eSWDMX_GET_INTERNAL_BUF, &u4InternalBufAddr, &u4Para2, &u4Align))
        {
            USR_SPACE_ACCESS_VALIDATE_ARG_K(u4Para1, u4Para2);
            COPY_FROM_USER_SIZE((UINT8*)u4InternalBufAddr, u4Para1, u4Para2);

            u4Para1 = u4InternalBufAddr;
        }
        else
        {
            return MTR_NOT_OK;
        }
    }
    if (u1Type == MTSWDMX_SET_MM_DECRYPT)
    {
        MM_CRYPT_INFO_T *ptEncryptInfo;
        UINT8 i;
        
        ptEncryptInfo = (MM_CRYPT_INFO_T*)(u4Para1);
        if(ptEncryptInfo == NULL)
        {
            return MTR_NOT_OK;
        }
        if(ptEncryptInfo->e_mm_crypt_type == MM_CRYPT_TYPE_MLN)
        {
            SWDMX_TRACK_CRYPT_INFO_T rTrackInfo;
            rTrackInfo.eDRMType = eSWDMX_DRM_TYPE_MARLIN;
            rTrackInfo.eCryptMode = eSWDMX_CRYPT_MODE_AES_CBC;
            rTrackInfo.u4KeyLength = 16;
            
            for(i=0; i < ptEncryptInfo->u.t_ml_crypt.ui1_valid_cnt && i < MAX_MARLIN_TRACK_NUM; i++)
            {
                rTrackInfo.pu1Key = (UINT8*) ptEncryptInfo->u.t_ml_crypt.t_track_drm[i].aui1_key;
                rTrackInfo.u4TrackId = ptEncryptInfo->u.t_ml_crypt.t_track_drm[i].ui4_content_id;
                if(ptEncryptInfo->u.t_ml_crypt.t_track_drm[i].e_strm_type == ST_VIDEO)
                {
                    SWDMX_SetInfo(ucEsId, eSWDMX_SET_VIDEO_DECRYPT, (UINT32)&rTrackInfo, 0, 0);
                }
                else if(ptEncryptInfo->u.t_ml_crypt.t_track_drm[i].e_strm_type == ST_AUDIO)
                {
                    SWDMX_SetInfo(ucEsId, eSWDMX_SET_AUDIO_DECRYPT, (UINT32)&rTrackInfo, 0, 0);
                }
            }
            return MTR_OK;
        }
        else if (ptEncryptInfo->e_mm_crypt_type == MM_CRYPT_TYPE_PDCF)
        {
            SWDMX_TRACK_CRYPT_INFO_T rTrackInfo;
            rTrackInfo.eDRMType = eSWDMX_DRM_TYPE_PDCF;
            rTrackInfo.u4KeyLength = 16;
            
            for(i=0; i < ptEncryptInfo->u.t_pdcf_crypt.ui1_valid_cnt && i < MAX_MARLIN_TRACK_NUM; i++)
            {
                rTrackInfo.pu1Key = (UINT8*) ptEncryptInfo->u.t_pdcf_crypt.t_track_drm[i].aui1_key;
                rTrackInfo.u4TrackId = ptEncryptInfo->u.t_pdcf_crypt.t_track_drm[i].ui4_content_id;
                if (ptEncryptInfo->u.t_pdcf_crypt.t_track_drm[i].e_mode == MM_CRYPT_MODE_AES_CBC)
                {
                    rTrackInfo.eCryptMode = eSWDMX_CRYPT_MODE_AES_CBC;
                }
                else if (ptEncryptInfo->u.t_pdcf_crypt.t_track_drm[i].e_mode == MM_CRYPT_MODE_AES_CTR)
                {
                    rTrackInfo.eCryptMode = eSWDMX_CRYPT_MODE_AES_CTR;
                }
                else
                {
                    LOG(0, "Error! Unknown encryption mode for PDCF!\n");
                }
    
                if(ptEncryptInfo->u.t_pdcf_crypt.t_track_drm[i].e_strm_type == ST_VIDEO)
                {
                    SWDMX_SetInfo(ucEsId, eSWDMX_SET_VIDEO_DECRYPT, (UINT32)&rTrackInfo, 0, 0);
                }
                else if(ptEncryptInfo->u.t_pdcf_crypt.t_track_drm[i].e_strm_type == ST_AUDIO)
                {
                    SWDMX_SetInfo(ucEsId, eSWDMX_SET_AUDIO_DECRYPT, (UINT32)&rTrackInfo, 0, 0);
                }
            }
            return MTR_OK;        
        }
    }

    u1Type = MTSWDMX_GetDecType(u1Type);

    if (!SWDMX_SetInfo(ucEsId, u1Type, u4Para1, u4Para2, u4Para3))
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


static MT_RESULT_T _MTSWDMX_GetInfo(unsigned long arg)
{
    MTAL_IOCTL_5ARG_T rArg;
    UINT8 u1SrcId, u1Type;
    UINT32* pu4Para1;
    UINT32* pu4Para2;
    UINT32* pu4Para3;
    UINT32 u4Para1 = 0, u4Para2 = 0, u4Para3 = 0;

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);
    u1SrcId = (UINT8)rArg.ai4Arg[0];
    u1Type = (UINT8)rArg.ai4Arg[1];
    pu4Para1 = (UINT32*)rArg.ai4Arg[2];
    pu4Para2 = (UINT32*)rArg.ai4Arg[3];
    pu4Para3 = (UINT32*)rArg.ai4Arg[4];

    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }
    // Some driver take u4Para1~3 as input, too
    if (pu4Para1)
    {
        USR_SPACE_ACCESS_VALIDATE_ARG(pu4Para1, UINT32);
        COPY_FROM_USER_ARG(pu4Para1, u4Para1, UINT32);
    }
    if (pu4Para2)
    {
        USR_SPACE_ACCESS_VALIDATE_ARG(pu4Para2, UINT32);
        COPY_FROM_USER_ARG(pu4Para2, u4Para2, UINT32);
    }
    if (pu4Para3)
    {
        USR_SPACE_ACCESS_VALIDATE_ARG(pu4Para3, UINT32);
        COPY_FROM_USER_ARG(pu4Para3, u4Para3, UINT32);
    }

    u1Type = MTSWDMX_GetDecType(u1Type);
    if(SWDMX_GetInfo(u1SrcId, u1Type, &u4Para1, &u4Para2, &u4Para3))
    {
        if(pu4Para1)
        {
            USR_SPACE_ACCESS_VALIDATE_ARG(pu4Para1, UINT32);
            COPY_TO_USER_ARG(pu4Para1, u4Para1, UINT32);
        }
        if(pu4Para2)
        {
            USR_SPACE_ACCESS_VALIDATE_ARG(pu4Para2, UINT32);
            COPY_TO_USER_ARG(pu4Para2, u4Para2, UINT32);
        }
        if(pu4Para3)
        {
            USR_SPACE_ACCESS_VALIDATE_ARG(pu4Para3, UINT32);
            COPY_TO_USER_ARG(pu4Para3, u4Para3, UINT32);
        }
        return MTR_OK;
    }
    else
    {
        //for some driver return false but info available
        if(pu4Para1)
        {
            USR_SPACE_ACCESS_VALIDATE_ARG(pu4Para1, UINT32);
            COPY_TO_USER_ARG(pu4Para1, u4Para1, UINT32);
        }
        if(pu4Para2)
        {
            USR_SPACE_ACCESS_VALIDATE_ARG(pu4Para2, UINT32);
            COPY_TO_USER_ARG(pu4Para2, u4Para2, UINT32);
        }
        if(pu4Para3)
        {
            USR_SPACE_ACCESS_VALIDATE_ARG(pu4Para3, UINT32);
            COPY_TO_USER_ARG(pu4Para3, u4Para3, UINT32);
        }
        return MTR_NOT_OK;
    }
}


static MT_RESULT_T _MTSWDMX_Pause(unsigned long arg)
{
    UINT8 u1SrcId = (UINT8)arg;
    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }
    if(SWDMX_Pause(u1SrcId))
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}


static MT_RESULT_T _MTSWDMX_PlayOneFrm(unsigned long arg)
{
    UINT8 u1SrcId = (UINT8)arg;
    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }
    if(SWDMX_PlayOneFrm(u1SrcId))
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}



static MT_RESULT_T _MTSWDMX_SetContainerType(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 u1SrcId;
    ENUM_SWDMX_FMT_T eContainerType = SWDMX_FMT_UNKNOWN;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    u1SrcId = (UINT8)rArg.ai4Arg[0];
     if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }
    switch ((ENUM_MTSWDMX_FMT_T)rArg.ai4Arg[1])
    {
    default:
        return MTR_NOT_OK;
    case MTSWDMX_FMT_UNKNOWN:
        eContainerType = SWDMX_FMT_UNKNOWN;
        break;
    case MTSWDMX_FMT_AVI:
        eContainerType = SWDMX_FMT_AVI;
        break;
    case MTSWDMX_FMT_MPEG1_DAT:
        eContainerType = SWDMX_FMT_MPEG1_DAT;
        break;
    case MTSWDMX_FMT_MPEG2_PS:
        eContainerType = SWDMX_FMT_MPEG2_PS;
        break;
    case MTSWDMX_FMT_MPEG2_TS:         // 188 ts
        eContainerType = SWDMX_FMT_MPEG2_TS;
        break;
    case MTSWDMX_FMT_MPEG2_TS_192:     // 192 ts: time case
        eContainerType = SWDMX_FMT_MPEG2_TS_192;
        break;
    case MTSWDMX_FMT_MPEG2_TS_ZERO_192:// 4 byte 192 ts
        eContainerType = SWDMX_FMT_MPEG2_TS_ZERO_192;
        break;
    case MTSWDMX_FMT_MPEG2_TS_130:
        eContainerType = SWDMX_FMT_MPEG2_TS_130;
        break;
    case MTSWDMX_FMT_MPEG2_TS_134:
        eContainerType = SWDMX_FMT_MPEG2_TS_134;
        break;
    case MTSWDMX_FMT_MPEG2_TS_204:
        eContainerType = SWDMX_FMT_MPEG2_TS_204;
        break;	
    case MTSWDMX_FMT_MPEG2_TS_206:
        eContainerType = SWDMX_FMT_MPEG2_TS_206;
        break;		
    case MTSWDMX_FMT_MPEG2_TS_ENCRYPT_192:
        eContainerType = SWDMX_FMT_MPEG2_TS_ENCRYPT_192;
        break;
    case MTSWDMX_FMT_MPEG2_TS_TIME_SHIFT:// time shift 192 ts
        eContainerType = SWDMX_FMT_MPEG2_TS_TIME_SHIFT;
        break;
    case MTSWDMX_FMT_VC1_ES:
        eContainerType = SWDMX_FMT_VC1_ES;
        break;
    case MTSWDMX_FMT_MPEG2_VIDEO_ES:
        eContainerType = SWDMX_FMT_MPEG2_VIDEO_ES;
        break;
    case MTSWDMX_FMT_MPEG4_VIDEO_ES:
        eContainerType = SWDMX_FMT_MPEG4_VIDEO_ES;
        break;
    case MTSWDMX_FMT_H264_VIDEO_ES:
        eContainerType = SWDMX_FMT_H264_VIDEO_ES;
        break;
    case MTSWDMX_FMT_AVS_VIDEO_ES:
        eContainerType = SWDMX_FMT_AVS_VIDEO_ES;
        break;
    case MTSWDMX_FMT_RAW_AUDIO_ES: // last one for ES: should not change this order.
        eContainerType = SWDMX_FMT_RAW_AUDIO_ES;
        break;
    case MTSWDMX_FMT_MP3_AUDIO_ES:
        eContainerType = SWDMX_FMT_MP3_AUDIO_ES;
        break;
    case MTSWDMX_FMT_VC1_WMV:
        eContainerType = SWDMX_FMT_VC1_WMV;
        break;
    case MTSWDMX_FMT_WMA_AUDIO_ES:
        eContainerType = SWDMX_FMT_WMA_AUDIO_ES;
        break;
    case MTSWDMX_FMT_MP4:
        eContainerType = SWDMX_FMT_MP4;
        break;
    case MTSWDMX_FMT_M4A:
        eContainerType = SWDMX_FMT_M4A;
        break;
    case MTSWDMX_FMT_MKV:
        eContainerType = SWDMX_FMT_MKV;
        break;
    case MTSWDMX_FMT_RM:
        eContainerType = SWDMX_FMT_RM;
        break;
    case MTSWDMX_FMT_FLV:
        eContainerType = SWDMX_FMT_FLV;
        break;
    case MTSWDMX_FMT_CMPB:
        eContainerType = SWDMX_FMT_CMPB;
        break;
    case MTSWDMX_FMT_ES_WITH_PTS:
        eContainerType = SWDMX_FMT_ES_WITH_PTS;
        break;
    case MTSWDMX_FMT_OGM:
        eContainerType = SWDMX_FMT_OGM;
        break;
    case MTSWDMX_FMT_OGG_AUDIO_ES:
        eContainerType = SWDMX_FMT_OGG_AUDIO_ES;   
        break;
    case MTSWDMX_FMT_APE_AUDIO_ES:
        eContainerType = SWDMX_FMT_APE_AUDIO_ES;   
        break;        
    case MTSWDMX_FMT_H265_VIDEO_ES:
        eContainerType = SWDMX_FMT_H265_VIDEO_ES;
        break;
    }
    
    if(SWDMX_SetContainerType(u1SrcId, eContainerType))
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}


static MT_RESULT_T _MTSWDMX_SetSpeed(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 u1SrcId;
    INT32 i4Speed;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    u1SrcId = (UINT8)rArg.ai4Arg[0];
    i4Speed = (INT32)rArg.ai4Arg[1];
    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    if(SWDMX_SetSpeed(u1SrcId, i4Speed))
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}


static MT_RESULT_T _MTSWDMX_Play(unsigned long arg)
{
    UINT8 u1SrcId = (UINT8)arg;
    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    if(SWDMX_Play(u1SrcId))
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}


static MT_RESULT_T _MTSWDMX_Flush(unsigned long arg)
{
    UINT8 u1SrcId = (UINT8)arg;
    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    if(SWDMX_Flush(u1SrcId))
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}


static MT_RESULT_T _MTSWDMX_Stop(unsigned long arg)
{
    UINT8 u1SrcId = (UINT8)arg;

    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }
    if(SWDMX_Stop(u1SrcId))
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}


static MT_RESULT_T _MTSWDMX_LockValidSrc(unsigned long arg)
{
    UINT8 u1SrcId = (UINT8)arg;

    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }
    if(SWDMX_LockValidSrc(u1SrcId))
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }

}


static MT_RESULT_T _MTSWDMX_UnlockSrc(unsigned long arg)
{
    UINT8 u1SrcId = (UINT8)arg;
    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    SWDMX_UnlockSrc(u1SrcId);

    return MTR_OK;
}


static MT_RESULT_T _MTSWDMX_SeekTime(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT8 u1SrcId;
    UINT32 u4SeekTime;
    UINT64 u8FilePos = 0;
    UINT64* pu8FilePos;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    u1SrcId = (UINT8)rArg.ai4Arg[0];
    u4SeekTime = (UINT32)rArg.ai4Arg[1];
    pu8FilePos = (UINT64*)rArg.ai4Arg[2];
    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    if(SWDMX_SeekTime(u1SrcId, u4SeekTime, &u8FilePos))
    {
        USR_SPACE_ACCESS_VALIDATE_ARG(pu8FilePos, UINT64);
        COPY_TO_USER_ARG(pu8FilePos, u8FilePos, UINT64);
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}


static MT_RESULT_T _MTSWDMX_SeekPos(unsigned long arg)
{
    MTAL_IOCTL_5ARG_T rArg;
    UINT8 u1SrcId;
    UINT64 u8SeekPos;
    UINT64 u8SeekPosI;

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

    u1SrcId = (UINT8)rArg.ai4Arg[0];
    u8SeekPos = ((UINT64)rArg.ai4Arg[1] << 32) + (UINT64)rArg.ai4Arg[2];
    u8SeekPosI = ((UINT64)rArg.ai4Arg[3] << 32) + (UINT64)rArg.ai4Arg[4];
    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    if(SWDMX_SeekPos(u1SrcId, u8SeekPos, u8SeekPosI))
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}


static MT_RESULT_T _MTSWDMX_RenderFromPos(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 u1SrcId;
    START_POS_INFO_T rPosition;
    VOID *prPosition;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    u1SrcId = (UINT8)rArg.ai4Arg[0];
    prPosition = (VOID*)rArg.ai4Arg[1];
    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prPosition, START_POS_INFO_T);
    COPY_FROM_USER_ARG(prPosition, rPosition, START_POS_INFO_T);

    if(SWDMX_RenderFromPos(u1SrcId, &rPosition))
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}



static MT_RESULT_T _MTSWDMX_SeekOffset(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 u1SrcId;
    START_POS_INFO_T rPosition;
    VOID *prPosition;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    u1SrcId = (UINT8)rArg.ai4Arg[0];
    prPosition = (VOID*)rArg.ai4Arg[1];
    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prPosition, START_POS_INFO_T);
    COPY_FROM_USER_ARG(prPosition, rPosition, START_POS_INFO_T);

    if(SWDMX_SeekOffset(u1SrcId, &rPosition))
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}


static MT_RESULT_T _MTSWDMX_SetStrmID(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    UINT8 u1SrcId;
    UINT32 u4StrmID;
    UINT32 u4StrmSubID;
    ENUM_SWDMX_STRM_TYPE_T eStrmType;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    u1SrcId = (UINT8)rArg.ai4Arg[0];
    u4StrmID = (UINT32)rArg.ai4Arg[1];
    u4StrmSubID = (UINT32)rArg.ai4Arg[2];
    eStrmType = (ENUM_SWDMX_STRM_TYPE_T)rArg.ai4Arg[3];
    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    if(SWDMX_SetStrmID(u1SrcId, u4StrmID, u4StrmSubID, eStrmType))
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}


static MT_RESULT_T _MTSWDMX_QueryRangeInfo(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 u1SrcId;
    SWDMX_RANGE_INFO_T* prRangeInfo;
    SWDMX_RANGE_INFO_T rRangeInfo;
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    u1SrcId = (UINT8)rArg.ai4Arg[0];
    prRangeInfo = (SWDMX_RANGE_INFO_T*)rArg.ai4Arg[1];

    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    SWDMX_QueryRangeInfo(u1SrcId, &rRangeInfo);

    USR_SPACE_ACCESS_VALIDATE_ARG(prRangeInfo, SWDMX_RANGE_INFO_T);
    COPY_TO_USER_ARG(prRangeInfo, rRangeInfo, SWDMX_RANGE_INFO_T);

    return MTR_OK;
}


static MT_RESULT_T _MTSWDMX_AddRange(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT8 u1SrcId;
    VOID* pvRangeInfo;
    UINT32 u4TagID;
    MM_RANGE_INFO_T* prDmxRange;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    u1SrcId = (UINT8)rArg.ai4Arg[0];
    pvRangeInfo = (VOID*)rArg.ai4Arg[1];
    u4TagID = (UINT32)rArg.ai4Arg[2];
    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    //---recursive data copy from user mode
    if(_MtSwDmx_pad_data_from_usr_struct(&prDmxRange, (void*)pvRangeInfo, TRUE) != MTR_OK)
    {
        return MTR_NOT_OK;
    }

    _MtSwDmx_add_range_list(u1SrcId, prDmxRange);
    //---

    if(SWDMX_AddRange(u1SrcId, (VOID*)prDmxRange, u4TagID))
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}


static MT_RESULT_T _MTSWDMX_DelRange(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    UINT8 u1SrcId;
    VOID* pvRangeInfo;
    UINT32 u4TagID;
    BOOL fgDelAllRange;
    BOOL fgRet;

    MM_RANGE_INFO_T rRangeInfo;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    u1SrcId = (UINT8)rArg.ai4Arg[0];
    pvRangeInfo = (VOID*)rArg.ai4Arg[1];
    u4TagID = (UINT32)rArg.ai4Arg[2];
    fgDelAllRange = (BOOL)rArg.ai4Arg[3];
    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    //rRangeInfo useless in driver
    if(pvRangeInfo)
    {
        USR_SPACE_ACCESS_VALIDATE_ARG(pvRangeInfo, MM_RANGE_INFO_T);
        COPY_FROM_USER_ARG(pvRangeInfo, rRangeInfo, MM_RANGE_INFO_T);
    }

    fgRet = SWDMX_DelRange(u1SrcId, &rRangeInfo, u4TagID, fgDelAllRange);
    _MtSwDmx_remove_range_list(u1SrcId, u4TagID);

    if(fgRet)
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}


static MT_RESULT_T _MTSWDMX_DelRangeList(unsigned long arg)
{
    UINT8 u1SrcId = (UINT8)arg;

    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    while (g_pt_range_list[u1SrcId])
    {
        if (g_pt_range_list[u1SrcId]->pt_range)
        {
            _MtSwDmx_remove_range_list(u1SrcId, g_pt_range_list[u1SrcId]->pt_range->ui4_range_id);
        }
        else
        {
            break;
        }
    }

    return MTR_OK;
}


static MT_RESULT_T _MTSWDMX_SetPlayMode(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 u1SrcId;
    SWDMX_PLAY_MODE_T ePlayMode = SWDMX_PLAY_MODE_NORMAL;
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    u1SrcId = (UINT8)rArg.ai4Arg[0];

    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }
    
    switch ((MTSWDMX_PLAY_MODE_T)rArg.ai4Arg[1])
    {
    default:
        return MTR_NOT_OK;
    case MTSWDMX_PLAY_MODE_NORMAL:
        ePlayMode = SWDMX_PLAY_MODE_NORMAL;
        break;
    case MTSWDMX_PLAY_MODE_IFRAME:
        ePlayMode = SWDMX_PLAY_MODE_IFRAME;
        break;
    }

    if(SWDMX_SetPlayMode(u1SrcId, ePlayMode))
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}

static MT_RESULT_T _MTSWDMX_WaitForPause(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 u1SrcId;
    UINT32 u4WaitTime = 0;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    
    u1SrcId = (UINT8)rArg.ai4Arg[0];
    u4WaitTime = (UINT32)rArg.ai4Arg[1];
    if(u1SrcId >= SWDMX_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    if (SWDMX_WaitForPause(u1SrcId, u4WaitTime))
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}
static MT_RESULT_T _MTSWDMX_CodecChgDone(unsigned long arg)
{
    UINT8 u1Type;
    UINT8 u1SrcId;
    UINT32 u4Para1, u4Para2, u4Para3;
    MM_CODEC_CHG_T         rCodecChg;
    MTAL_IOCTL_5ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

    u1SrcId = (UCHAR) rArg.ai4Arg[0];
    u1Type  = (UINT8) rArg.ai4Arg[1];
    u4Para1 = (UINT32)rArg.ai4Arg[2];
    u4Para2 = (UINT32)rArg.ai4Arg[3];
    u4Para3 = (UINT32)rArg.ai4Arg[4];

    MTSWDMX_PRINT("_MTSWDMX_CodecChgDone %d\n");
    
    u1Type = MTSWDMX_GetDecType(u1Type);    
    /* MW notify NULL ,so don't need set  */
    //COPY_FROM_USER_SIZE(&rCodecChg,(VOID*)u4Para1,sizeof(MM_CODEC_CHG_T));
    
    SWDMX_CodecChgDone(u1SrcId,u1Type,(MM_CODEC_CHG_T*)&rCodecChg);
    
    
    return MTR_OK;
}

static MT_RESULT_T _MTSWDMX_PidChgDone(unsigned long arg)
{
    UINT8 u1Type;
    UINT8 u1SrcId;
    UINT32 u4Para1, u4Para2, u4Para3;
    MM_PID_CHG_T         rVidPidChg;
    MTAL_IOCTL_5ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

    u1SrcId = (UCHAR) rArg.ai4Arg[0];
    u1Type  = (UINT8) rArg.ai4Arg[1];
    u4Para1 = (UINT32)rArg.ai4Arg[2];
    u4Para2 = (UINT32)rArg.ai4Arg[3];
    u4Para3 = (UINT32)rArg.ai4Arg[4];

    MTSWDMX_PRINT("_MTSWDMX_PidChgDone %d\n");
    
    u1Type = MTSWDMX_GetDecType(u1Type);    
    COPY_FROM_USER_SIZE(&rVidPidChg,(VOID*)u4Para1,sizeof(MM_PID_CHG_T));
    
    SWDMX_PidChgDone(u1SrcId,u1Type,(MM_PID_CHG_T*)&rVidPidChg);
    
    
    return MTR_OK;
}
static MT_RESULT_T _MTSWDMX_RegPidChgCallback(unsigned long arg)
{
    UINT8 u1Type;
    UINT8 u1SrcId;
    UINT32 u4Para1, u4Para2, u4Para3;
    MTSWDEMUX_PID_CHG_NFY_INFO_T    rPidChgNfy;
    MTAL_IOCTL_5ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

    u1SrcId = (UCHAR) rArg.ai4Arg[0];
    u1Type  = (UINT8) rArg.ai4Arg[1];
    u4Para1 = (UINT32)rArg.ai4Arg[2];
    u4Para2 = (UINT32)rArg.ai4Arg[3];
    u4Para3 = (UINT32)rArg.ai4Arg[4];

    MTSWDMX_PRINT("_MTSWDMX_RegPidChgCallback %d\n");
    u1Type = MTSWDMX_GetDecType(u1Type);    
    COPY_FROM_USER_SIZE(&rPidChgNfy,(VOID*)u4Para1,sizeof(MTSWDEMUX_PID_CHG_NFY_INFO_T));

    if (u1Type == eSWDMX_SET_VID_CHG_CALLBACK)
    {
        rPidChgNfy.pf_swdemux_pid_chg_nfy = _MTSWDMX_VidChgNfy;
    }
    
    if (u1Type == eSWDMX_SET_AUD_CHG_CALLBACK)
    {
        rPidChgNfy.pf_swdemux_pid_chg_nfy = _MTSWDMX_AudChgNfy;
    }    
    
    SWDMX_RegPidChgCallback(u1SrcId,u1Type,(MTSWDEMUX_PID_CHG_NFY_INFO_T*)&rPidChgNfy);
        
    return MTR_OK;
}

static MT_RESULT_T _MTSWDMX_SetFlushFeederInfo(unsigned long arg)
{
    UINT8 u1Type;
    UINT8 u1SrcId;
    UINT32 u4Para1, u4Para2, u4Para3;
    UINT32 u4ApSeekPts; 
    MTAL_IOCTL_5ARG_T    rArg;
    
    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);
    
    u1SrcId = (UINT8) rArg.ai4Arg[0];
    u1Type  = (UINT8) rArg.ai4Arg[1];
    u4Para1 = (UINT32)rArg.ai4Arg[2];
    u4Para2 = (UINT32)rArg.ai4Arg[3];
    u4Para3 = (UINT32)rArg.ai4Arg[4];
    
    
    if (0 != u4Para1)
    {
        USR_SPACE_ACCESS_VALIDATE_ARG(u4Para1, UINT32);
        COPY_FROM_USER_ARG(u4Para1, u4ApSeekPts, UINT32);
        //Printf("_MTSWDMX_SetFlushFeederInfo u4Para1 = %d, u4ApSeekPts = %d ms\n",u4Para1,u4ApSeekPts);
        SWDMX_SetFlushFeederInfo(u1SrcId,u1Type,(VOID*)&u4ApSeekPts,u4Para2,u4Para3);
    }
    else 
    {
        //Printf("_MTSWDMX_SetFlushFeederInfo u4Para1 = 0\n");
        SWDMX_SetFlushFeederInfo(u1SrcId,u1Type,NULL,u4Para2,u4Para3);
    }
    return MTR_OK;
    
}

static MT_RESULT_T _MTSWDMX_SetGnrcStreamInfo(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    UINT8 u1SrcId;
    UINT32 u4Para1;
    MM_GNRC_STREAM_T t_gnrc_stream;
    MTAL_IOCTL_2ARG_T   rArg;
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    x_memset((void*)&t_gnrc_stream,0x0,sizeof(MM_GNRC_STREAM_T));
    
    u1SrcId = (UCHAR) rArg.ai4Arg[0];
    u4Para1 = (UINT32)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(u4Para1, MM_GNRC_STREAM_T);
    COPY_FROM_USER_ARG(u4Para1, t_gnrc_stream, MM_GNRC_STREAM_T);
    
    SWDMX_SetGnrcStreamInfo(u1SrcId,(UINT32)&t_gnrc_stream);

    return eRet;
}
static MT_RESULT_T _MTSWDMX_SetHdcpKey(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    UINT8 u1SrcId;
    UINT32 u4Para1;
    MTAL_IOCTL_2ARG_T   rArg;
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);    
    
    u1SrcId = (UCHAR) rArg.ai4Arg[0];
    u4Para1 = (UINT32)rArg.ai4Arg[1];
    
    x_memset((VOID*)&g_tHDCPKey,0x0,sizeof(g_tHDCPKey));
    
    COPY_FROM_USER_SIZE((VOID*)&g_tHDCPKey, (VOID*)u4Para1, sizeof(g_tHDCPKey));
    
    SWDMX_SetInfo(u1SrcId,eSWDMX_TYPE_SET_HDCP_KEY,(UINT32)&g_tHDCPKey,0,0);
    
    return eRet;    
}
static MT_RESULT_T _MTSWDMX_EnableHdcpKey(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    UINT8 u1SrcId;
    UINT16 u2Type;
    MTAL_IOCTL_2ARG_T   rArg;
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    
    u1SrcId = (UCHAR) rArg.ai4Arg[0];
    u2Type = (UINT32)rArg.ai4Arg[1];

    u2Type = MTSWDMX_GetDecType((UINT8)u2Type);
    
    SWDMX_EnableHdcpKey(u1SrcId,(UINT8)u2Type);
    
    return eRet;    
}


int mtal_ioctl_mtswdmx(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg)
{
    int i4Ret = MTR_OK;

    if (cmd != MTAL_IO_SWDMX_INIT)
    {
        LockVdecSemaphoreMtal();
    }
    
    switch (cmd)
    {
        case MTAL_IO_SWDMX_INIT:
            i4Ret = _MTSWDMX_Init(arg);
            break;
        case MTAL_IO_SWDMX_SETINFO:
            i4Ret = _MTSWDMX_SetInfo(arg);
            break;
        case MTAL_IO_SWDMX_GETINFO:
            i4Ret = _MTSWDMX_GetInfo(arg);
            break;
        case MTAL_IO_SWDMX_PAUSE:
            i4Ret = _MTSWDMX_Pause(arg);
            break;
        case MTAL_IO_SWDMX_SETCONTAINERTYPE:
            i4Ret = _MTSWDMX_SetContainerType(arg);
            break;
        case MTAL_IO_SWDMX_SETSPEED:
            i4Ret = _MTSWDMX_SetSpeed(arg);
            break;
		case MTAL_IO_SWDMX_PLAY_ONE_FRM:
            i4Ret = _MTSWDMX_PlayOneFrm(arg);
            break;
        case MTAL_IO_SWDMX_PLAY:
            i4Ret = _MTSWDMX_Play(arg);
            break;
        case MTAL_IO_SWDMX_FLUSH:
            i4Ret = _MTSWDMX_Flush(arg);
            break;
        case MTAL_IO_SWDMX_STOP:
            i4Ret = _MTSWDMX_Stop(arg);
            break;
        case MTAL_IO_SWDMX_LOCKVALIDSRC:
            i4Ret = _MTSWDMX_LockValidSrc(arg);
            break;
        case MTAL_IO_SWDMX_UNLOCKSRC:
            i4Ret = _MTSWDMX_UnlockSrc(arg);
            break;
        case MTAL_IO_SWDMX_SEEKTIME:
            i4Ret = _MTSWDMX_SeekTime(arg);
            break;
        case MTAL_IO_SWDMX_SEEKPOS:
            i4Ret = _MTSWDMX_SeekPos(arg);
            break;
        case MTAL_IO_SWDMX_RENDERFROMPOS:
            i4Ret = _MTSWDMX_RenderFromPos(arg);
            break;
        case MTAL_IO_SWDMX_SEEKOFFSET:
            i4Ret = _MTSWDMX_SeekOffset(arg);
            break;
        case MTAL_IO_SWDMX_SETSTRMID:
            i4Ret = _MTSWDMX_SetStrmID(arg);
            break;
        case MTAL_IO_SWDMX_QUERYRANGEINFO:
            i4Ret = _MTSWDMX_QueryRangeInfo(arg);
            break;
        case MTAL_IO_SWDMX_ADDRANGE:
            i4Ret = _MTSWDMX_AddRange(arg);
            break;
        case MTAL_IO_SWDMX_DELRANGE:
            i4Ret = _MTSWDMX_DelRange(arg);
            break;
        case MTAL_IO_SWDMX_DELRANGELIST:
            i4Ret = _MTSWDMX_DelRangeList(arg);
            break;
        case MTAL_IO_SWDMX_SETDMXNFY:
            i4Ret = _MTSWDMX_SetDmxNfy(arg);
            break;
        case MTAL_IO_SWDMX_SETPLAYMODE:
            i4Ret = _MTSWDMX_SetPlayMode(arg);
            break;
        case MTAL_IO_SWDMX_WAITFORPAUSE:
            i4Ret = _MTSWDMX_WaitForPause(arg);
            break;
        case MTAL_IO_SWDMX_PID_CHG_DONE:
            i4Ret = _MTSWDMX_PidChgDone(arg);
            break;
        case MTAL_IO_SWDMX_CODEC_CHG_DONE:
            i4Ret = _MTSWDMX_CodecChgDone(arg);
            break;
        case MTAL_IO_SWDMX_REG_PID_CHG_CB:
            i4Ret = _MTSWDMX_RegPidChgCallback(arg);
            break;
        case MTAL_IO_SWDMX_SET_FLUSHFEEDER:
            i4Ret = _MTSWDMX_SetFlushFeederInfo(arg);
            break;
        case MTAL_IO_SWDMX_GNRC_STREAM_INFO:
            i4Ret = _MTSWDMX_SetGnrcStreamInfo(arg);                    
            break;
        case MTAL_IO_SWDMX_SET_HDCPKEY:
            i4Ret = _MTSWDMX_SetHdcpKey(arg);                    
            break;        
        case MTAL_IO_SWDMX_ENABLE_HDCPKEY:
            i4Ret = _MTSWDMX_EnableHdcpKey(arg);                    
            break;
        default:
            i4Ret = MTR_NOT_OK;
            break;
    }
    
    if (cmd != MTAL_IO_SWDMX_INIT)
    {    
        UnLockVdecSemaphoreMtal();
    }
    
    return i4Ret;
}

