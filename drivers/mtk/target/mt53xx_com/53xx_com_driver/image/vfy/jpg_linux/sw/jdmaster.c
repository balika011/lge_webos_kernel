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
/*-----------------------------------------------------------------------------
 * $RCSfile: jdmaster.c,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/DTV_X_ATSC/3 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: f426aea9721282a8f3b4c05f7cf95e24 $ Asa
 *
 * Description:
 *         This file contains the interface generic APIs of GCL layer.
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/

#include "x_os.h"
//#include "x_dbg.h"
#include "x_common.h"
#include "jpg_config.h"
#include "jpeglib.h"
#include "djpeg_sw_api.h"
//#include "x_rtos.h"
#include "../jdec_debug.h"
#include "drv_config.h"
#include "x_debug.h"

//#include "sys_config.h"

#if CONFIG_SYS_MEM_PHASE2
#include "x_mem_phase2.h"
#elif CONFIG_SYS_MEM_PHASE3
#include "x_kmem.h"
#endif

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    extern data declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Name:  vInitial_decode_buffer
 *
 * Description: Initialize master decompression control and select active modules.
 *              This is performed at the start of jpeg_start_decompress.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/

void vInitial_decode_buffer (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, SRC_DST_RECT_T* pt_src_dst_rect, IMG_ROTATE_T e_rotate, BOOL b_sw_dec)
{
    INT16 ci;
    UINT32 i;
    UINT32 j;
    COMPONENT_INFO_T *compptr;
    UINT32 previous_comp_buf_size;
    UINT8 *color_base;
    //UINT32 blocks_per_iMCU_row;
    /* record the number of MCUs in interleaved scan */
    UINT32 ilv_scan_MCUs_per_row;
    UINT32 ui4_up_scale_q_image_width;
    UINT32 ui4_tmp, ui4_left, ui4_right;
    register IMG_BASIC_INFO_T* pt_info = NULL;
    INT16** ppi2_iMCU_row_start_addr;
    UINT32 ui4_iTotalIMCURows;
    UINT32 ui4_color_buf_len = 0;
    UINT32 u4EachColorBufSz = 0;
    UINT32 u4MaxHFactor = 0;
    UINT32 u4MaxVFactor = 0;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }
#if 0
    if ((e_ratate_type == IMG_ROTATE_CLOCKWS_90_OP_TYPE)  || (e_ratate_type == IMG_ROTATE_CLOCKWS_90_OP_TYPE_WITH_FLIP) ||
        (e_ratate_type == IMG_ROTATE_CLOCKWS_270_OP_TYPE) || (e_ratate_type == IMG_ROTATE_CLOCKWS_270_OP_TYPE_WITH_FLIP))
    {
        pt_jpg_obj->ui4_display_image_width = pt_src_dst_rect->ui4_dst_h;
        pt_jpg_obj->ui4_display_image_height = pt_src_dst_rect->ui4_dst_w;
    }
    else
    {
        pt_jpg_obj->ui4_display_image_width = pt_src_dst_rect->ui4_dst_w;
        pt_jpg_obj->ui4_display_image_height = pt_src_dst_rect->ui4_dst_h;
    }

    pt_jpg_obj->ui4_h_pixel_ratio = ((pt_src_dst_rect->ui4_src_w << UP_SCALE_Q) / pt_jpg_obj->ui4_display_image_width);
    pt_jpg_obj->ui4_v_pixel_ratio = ((pt_src_dst_rect->ui4_src_h  << UP_SCALE_Q) / pt_jpg_obj->ui4_display_image_height);

    vSet_blit_fun_pt(prJdecInst, pt_jpg_obj, pt_src_dst_rect, pt_jpg_obj->ui4_display_image_width, pt_jpg_obj->ui4_display_image_height, e_ratate_type);
    vSet_resampling_fun_pt(prJdecInst, pt_jpg_obj);
#endif

    if (pt_info->ui2_p_scan_decode_num == 0)
    {
        /* record component pt_info->buffer width in pixel */
        ui4_up_scale_q_image_width = pt_info->ui2_image_width << UP_SCALE_Q;

        ilv_scan_MCUs_per_row = ((SCALE_UP_CEIL((ui4_up_scale_q_image_width)/(pt_info->ui1_max_h_samp_factor*DCTSIZE)))>>UP_SCALE_Q);

        pt_info->ui4_begin_decode_iMCU_row = 0;
        pt_info->ui4_total_WS_iMCU_row_num = 0;

        if (pt_info->e_idct_size == EIGHT_BY_EIGHT)
        {
            pt_info->ui4_downsized_idctsize = DCTSIZE;
        }
        else if (pt_info->e_idct_size == FOUR_BY_FOUR)
        {
            pt_info->ui4_downsized_idctsize = DCTSIZE / 2;
        }
        else if (pt_info->e_idct_size == TWO_BY_TWO)
        {
            pt_info->ui4_downsized_idctsize = DCTSIZE / 4;
        }
        else /* if (pt_info->e_idct_size == ONE_BY_ONE) */
        {
            pt_info->ui4_downsized_idctsize = DCTSIZE / 8;
        }

        pt_info->ui4_downsized_idctsize2 = pt_info->ui4_downsized_idctsize * pt_info->ui4_downsized_idctsize;

        /* 計算所給定的DRAM workspace 一次可解多少個 iMCU rows */
        //blocks_per_iMCU_row = 0;
#if 0
        for (ci = 0, compptr = pt_info->at_comp_info; ci < pt_info->ui2_num_components; ci++, compptr++)
        {
            //blocks_per_iMCU_row += ilv_scan_MCUs_per_row * compptr->i2_h_samp_factor * compptr->i2_v_samp_factor;
            ui4_tmp = ilv_scan_MCUs_per_row * compptr->i2_h_samp_factor;
            pt_jpg_obj->aui4_comp_buf_width_in_block[ci] = ui4_tmp;
            if (ui4_tmp & 0x1)
            { /* HW JPEG decoder pt_info->ui4_pic_width_in_buf[ci] need 16bits alignment */
                ui4_tmp++;
            }
            pt_info->ui4_pic_width_in_buf[ci] = (ui4_tmp * pt_info->ui4_downsized_idctsize);
            //[CJYang_ColorBuf]
            prJdecInst->au4ColorBufPitch[ci] = pt_info->ui4_pic_width_in_buf[ci];
            ui4_tmp = pt_info->ui4_pic_width_in_buf[ci] * pt_info->ui4_downsized_idctsize * compptr->i2_v_samp_factor;
            ui4_color_buf_len += ui4_tmp;
            if (u4EachColorBufSz < ui4_tmp)
            {
                u4EachColorBufSz = ui4_tmp;
            }
        }
#else
        for (ci = 0, compptr = pt_info->at_comp_info; ci < pt_info->ui2_num_components; ci++, compptr++)
        {
            if (u4MaxHFactor < compptr->i2_h_samp_factor)
            {
                u4MaxHFactor = compptr->i2_h_samp_factor;
            }
            if (u4MaxVFactor < compptr->i2_v_samp_factor)
            {
                u4MaxVFactor = compptr->i2_v_samp_factor;
            }
        }
        if ((u4MaxHFactor == 1)&&(u4MaxVFactor == 1))
        {
            u4MaxHFactor = 2;
            u4MaxVFactor = 2;
        }
        if (pt_info->ui2_num_components > 3) // for prJdecInst->au4ColorBufPitch[] not overflow
        {
            ASSERT(0);
        }
        for (ci = 0, compptr = pt_info->at_comp_info; ci < pt_info->ui2_num_components; ci++, compptr++)
        {
            /* HW JPEG decoder pt_info->ui4_pic_width_in_buf[ci] need 128bits alignment */
            ui4_tmp = ilv_scan_MCUs_per_row * u4MaxHFactor;
            pt_jpg_obj->aui4_comp_buf_width_in_block[ci] = ui4_tmp;

            pt_info->ui4_pic_width_in_buf[ci] = (ui4_tmp * pt_info->ui4_downsized_idctsize);
            //[CJYang_ColorBuf]
            prJdecInst->au4ColorBufPitch[ci] = pt_info->ui4_pic_width_in_buf[ci];
            ui4_tmp = pt_info->ui4_pic_width_in_buf[ci] * pt_info->ui4_downsized_idctsize * u4MaxVFactor;
            ui4_color_buf_len += ui4_tmp;
            if (u4EachColorBufSz < ui4_tmp)
            {
                u4EachColorBufSz = ui4_tmp;
            }
        }
#endif
        //[CJYang Resample]
        if (pt_info->ui2_num_components < JDEC_COMP_NUM)
        {
            ui4_color_buf_len += (u4EachColorBufSz * (JDEC_COMP_NUM - pt_info->ui2_num_components));
        }
        //--

        /* calculate how many iMCU row number can be stored in the given DRAM workspace */
        pt_info->ui4_image_iMCU_row_num = ((SCALE_UP_CEIL(((UINT32)pt_info->ui2_image_height<<UP_SCALE_Q)/((UINT32)pt_info->ui1_max_v_samp_factor * DCTSIZE)))>>UP_SCALE_Q);
        pt_info->ui4_total_WS_iMCU_row_num = pt_info->ui4_image_iMCU_row_num;

        /* determine the first decode iMCU row number of current decoding action in progressive scan image */
        /* in sequential scan image, pt_jpg_obj->begin_decode_iMCU_row always begins from 0                       */
        pt_info->ui4_begin_decode_iMCU_row = (pt_info->ui4_total_WS_iMCU_row_num) * pt_info->ui2_p_scan_decode_num;
        /* When pt_jpg_obj->begin_decode_iMCU_row + pt_jpg_obj->total_WS_iMCU_row_num > total_iMCU_row of image */
        /* We restrict pt_jpg_obj->total_WS_iMCU_row_num as the remaining undecoding iMCU rows        */

        if ((pt_info->ui4_begin_decode_iMCU_row + pt_info->ui4_total_WS_iMCU_row_num) >= pt_info->ui4_image_iMCU_row_num)
        {
            pt_info->ui4_total_WS_iMCU_row_num = (pt_info->ui4_image_iMCU_row_num - pt_info->ui4_begin_decode_iMCU_row);
            pt_info->b_last_p_scan_decode = TRUE;
        }

        compptr = pt_info->at_comp_info;
#if 0 //[CJYang Resample]
        pt_jpg_obj->ui4_org_comp_Y_image_height_sub_1 = ( (pt_src_dst_rect->ui4_src_h * compptr->i2_v_samp_factor * pt_info->ui4_downsized_idctsize)/(pt_info->ui1_max_v_samp_factor * DCTSIZE)) - 1;
        pt_jpg_obj->ui4_org_comp_Y_image_width_sub_1 = ( (pt_src_dst_rect->ui4_src_w * compptr->i2_h_samp_factor * pt_info->ui4_downsized_idctsize)/(pt_info->ui1_max_h_samp_factor * DCTSIZE)) - 1;
        pt_jpg_obj->ui4_v_samp_factor_X_DCTSIZE = compptr->i2_v_samp_factor * pt_info->ui4_downsized_idctsize;
#endif
        if (b_sw_dec == TRUE)
        {
            ppi2_iMCU_row_start_addr = pt_jpg_obj->ppui2_iMCU_row_start_addr;
            //[CJYang Resample]
            //ui4_iTotalIMCURows = pt_info->ui4_TotalIMCURows;
            ui4_iTotalIMCURows = prJdecInst->u4MCURowNum;
            ui4_left = pt_info->ui4_begin_decode_iMCU_row;
            ui4_right = pt_info->ui4_begin_decode_iMCU_row + pt_info->ui4_total_WS_iMCU_row_num;

            for (ci = 0, compptr = pt_info->at_comp_info; ci < pt_info->ui2_num_components; ci++, compptr++)
            {
                ui4_tmp = pt_jpg_obj->aui4_comp_buf_width_in_block[ci]* compptr->i2_v_samp_factor;

                if (pt_info->b_has_multiple_scans == TRUE)
                {
                    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                    pt_jpg_obj->apui2_buffer[ci] = (INT16*)x_alloc_aligned_dma_mem( pt_info->ui4_total_WS_iMCU_row_num * pt_jpg_obj->aui4_comp_buf_width_in_block[ci] * compptr->i2_v_samp_factor * DCTSIZE2 * INSIZE,32 );
#elif CONFIG_SYS_MEM_PHASE3
                    pt_jpg_obj->apui2_buffer[ci] = (INT16*)x_mem_aligned_alloc( pt_info->ui4_total_WS_iMCU_row_num * pt_jpg_obj->aui4_comp_buf_width_in_block[ci] * compptr->i2_v_samp_factor * DCTSIZE2 * INSIZE,32 );
#endif

                    #else
                    pt_jpg_obj->apui2_buffer[ci] = (INT16*)x_alloc_aligned_nc_mem( pt_info->ui4_total_WS_iMCU_row_num * pt_jpg_obj->aui4_comp_buf_width_in_block[ci] * compptr->i2_v_samp_factor * DCTSIZE2 * INSIZE,32 );
	                #endif
                    //pt_jpg_obj->apui2_buffer[ci] = x_mem_alloc( pt_info->ui4_total_WS_iMCU_row_num * pt_jpg_obj->aui4_comp_buf_width_in_block[ci] * compptr->i2_v_samp_factor * DCTSIZE2 * INSIZE );
                    if (pt_jpg_obj->apui2_buffer[ci] == NULL)
                    {
                        pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
                        //UTIL_Printf("[IMG] ERR: vInitial_decode_buffer(): x_mem_alloc error\n\r");
                        vJdecLog_L("[JPG_SW] ERR: vInitial_decode_buffer(): x_mem_alloc error\n\r");
                        return;
                    }
                    /* Zero out the buffers as requested */
                    x_memset(pt_jpg_obj->apui2_buffer[ci], 0, (pt_info->ui4_total_WS_iMCU_row_num * INSIZE * 8 + (pt_info->ui4_image_iMCU_row_num - pt_info->ui4_total_WS_iMCU_row_num) ) * pt_jpg_obj->aui4_comp_buf_width_in_block[ci] * compptr->i2_v_samp_factor * (DCTSIZE2 >>3));

                    /* 因為每一次會讀一個 iMCU row, 所以在這裡計算每一個 iMCU row的pt_info->buffer DRAM start address */
                    previous_comp_buf_size = 0;
                    for (j = 0; j < pt_info->ui4_image_iMCU_row_num; j++) /*Mars,[2002/4/25],fix bug which clears pt_info->buffer over limit*/
                    {
                        *(ppi2_iMCU_row_start_addr+(ci*ui4_iTotalIMCURows)+j) =  pt_jpg_obj->apui2_buffer[ci] + previous_comp_buf_size;

                        if ((j >= ui4_left) && (j < (ui4_right)) )
                            previous_comp_buf_size += ui4_tmp * DCTSIZE2;
                        else /* nz_buffer */
                            previous_comp_buf_size += ui4_tmp * DCTSIZE2 / 16;
                    }
                }
                else
                {
                    /* Since we only use only iMCU row pt_info->buffer to save input coefficient, each component pt_info->buffer contains only one iMCU row size pt_info->buffer */
                    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                    pt_jpg_obj->apui2_buffer[ci] = (INT16*)x_alloc_aligned_dma_mem( ui4_tmp * DCTSIZE2 * INSIZE, 32 );
#elif CONFIG_SYS_MEM_PHASE3
                    pt_jpg_obj->apui2_buffer[ci] = (INT16*)x_mem_aligned_alloc( ui4_tmp * DCTSIZE2 * INSIZE, 32 );
#endif

                    #else
                    pt_jpg_obj->apui2_buffer[ci] = (INT16*)x_alloc_aligned_nc_mem( ui4_tmp * DCTSIZE2 * INSIZE, 32 );
	                #endif
                    
                    //pt_jpg_obj->apui2_buffer[ci] = x_mem_alloc( ui4_tmp * DCTSIZE2 * INSIZE );
                    if (pt_jpg_obj->apui2_buffer[ci] == NULL)
                    {
                        //UTIL_Printf("[IMG] ERR: vInitial_decode_buffer(): x_mem_alloc error\n\r");
                        vJdecLog_L("[JPG_SW] ERR: vInitial_decode_buffer(): x_mem_alloc error\n\r");
                        pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
                        return;
                    }

                    /* 因為每一次會讀一個 iMCU row, 所以在這裡計算每一個 iMCU row的pt_info->buffer DRAM start address */
                    *(ppi2_iMCU_row_start_addr+(ci*ui4_iTotalIMCURows)) =  pt_jpg_obj->apui2_buffer[ci];
                }
            }
            
            ui4_color_buf_len = ui4_color_buf_len * 2; /* double buffer */
            //[CJYang Resample]
            //color_base = (UINT8*) x_mem_alloc(ui4_color_buf_len);
          #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
	     color_base = (UINT8*) x_alloc_aligned_dma_mem(ui4_color_buf_len, 32);
#elif CONFIG_SYS_MEM_PHASE3
	     color_base = (UINT8*) x_mem_aligned_alloc(ui4_color_buf_len, 32);
#endif

          #else
            color_base = (UINT8*) x_alloc_aligned_nc_mem(ui4_color_buf_len, 32);
	   #endif
            if (color_base == NULL)
            {
                pt_jpg_obj->e_JPGErrorLevel = JPG_FATAL_ERR;
                //UTIL_Printf("[IMG] ERR: vInitial_decode_buffer(): x_mem_alloc error\n\r");
                vJdecLog_L("[JPG_SW] ERR: vInitial_decode_buffer(): x_mem_alloc error\n\r");
                return;
            }

            //[CJYang Resample]
            //x_memset(color_base, 0, ui4_color_buf_len);
            x_memset(color_base, 0x80, ui4_color_buf_len);

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

            for (ci = 0, compptr = pt_info->at_comp_info; ci < pt_info->ui2_num_components; ci++, compptr++)
            {
                for (i = 0; i < 2; i++)
                {
                    //[CJYang_ColorBuf]
                    //pt_jpg_obj->aapui1_color_buf[ci][i] = color_base ;
                    prJdecInst->au4ColorBufSa[i][ci] = (UINT32)(color_base);                    //--
                    color_base += pt_info->ui4_pic_width_in_buf[ci] * pt_info->ui4_downsized_idctsize * compptr->i2_v_samp_factor; 
                }
				prJdecInst->au4ColorBufSz[ci]= pt_info->ui4_pic_width_in_buf[ci] * pt_info->ui4_downsized_idctsize * compptr->i2_v_samp_factor; 
            }
            //[CJYang Resample], for less then 3 components
            ui4_tmp = JDEC_COMP_NUM - pt_info->ui2_num_components;
            if (ui4_tmp > 0)
            {
                for (j=0; j<ui4_tmp; j++)
                {
                    // For klocwork issues
                    if ((pt_info->ui2_num_components + j) > (JDEC_COMP_NUM - 1))
                    {
                        ASSERT(0);
                    }
                    prJdecInst->fgDefaultScale[pt_info->ui2_num_components+j] = TRUE;
                    for (i = 0; i < 2; i++)
                    {
                        // For klocwork issues
                        if ((pt_info->ui2_num_components + j) > (JDEC_COMP_NUM - 1))
                        {
                            ASSERT(0);
                        }
                        prJdecInst->au4ColorBufSa[i][pt_info->ui2_num_components+j] = (UINT32)(color_base);
                        color_base += u4EachColorBufSz;
                    }
					prJdecInst->au4ColorBufSz[pt_info->ui2_num_components+j]=u4EachColorBufSz;
                }
            }
			UTIL_Printf("[jdec]colorbuffer 0,size =0x%x\n",prJdecInst->au4ColorBufSz[0]);
			UTIL_Printf("[jdec]colorbuffer 1,size =0x%x\n",prJdecInst->au4ColorBufSz[1]);
			UTIL_Printf("[jdec]colorbuffer 2,size =0x%x\n",prJdecInst->au4ColorBufSz[2]);				
            //--
        }

    }/* pt_jpg_obj->p_scan_decode_num == 0 */
    else
    {
        /* 在progressive scan image的其他decode actions, 只需要重新計算記錄 (non-zero mark)和這次要解係數資料的DRAM start address就可 */
        pt_info->ui4_begin_decode_iMCU_row = (pt_info->ui4_total_WS_iMCU_row_num) * pt_info->ui2_p_scan_decode_num;
        /* When pt_jpg_obj->begin_decode_iMCU_row + pt_jpg_obj->total_WS_iMCU_row_num > total_iMCU_row of image */
        /* We restrict pt_jpg_obj->total_WS_iMCU_row_num as the remaining undecoding iMCU rows        */

        if ((pt_info->ui4_begin_decode_iMCU_row + pt_info->ui4_total_WS_iMCU_row_num) >= pt_info->ui4_image_iMCU_row_num)
        {
            pt_info->ui4_total_WS_iMCU_row_num = (UINT16) (pt_info->ui4_image_iMCU_row_num - pt_info->ui4_begin_decode_iMCU_row);
            pt_info->b_last_p_scan_decode = TRUE;
        }

        if (b_sw_dec == TRUE)
        {
            ppi2_iMCU_row_start_addr = pt_jpg_obj->ppui2_iMCU_row_start_addr;
            //[CJYang Resample]
            //ui4_iTotalIMCURows = pt_info->ui4_TotalIMCURows;
            ui4_iTotalIMCURows = prJdecInst->u4MCURowNum;
            ui4_left = pt_info->ui4_begin_decode_iMCU_row;
            ui4_right = pt_info->ui4_begin_decode_iMCU_row + pt_info->ui4_total_WS_iMCU_row_num;

            /* 因為每一次會讀一個 iMCU row, 所以在這裡計算每一個 iMCU row的pt_info->buffer DRAM start address */
            // For klocwork issues
            if (pt_info->ui2_num_components > MAX_COMPS_IN_SCAN)
            {
                ASSERT(0);
            }
            for (ci = 0, compptr = pt_info->at_comp_info; ci < pt_info->ui2_num_components; ci++, compptr++)
            {
                previous_comp_buf_size = 0;
                ui4_tmp = pt_jpg_obj->aui4_comp_buf_width_in_block[ci]* compptr->i2_v_samp_factor;

                for (j = 0; j < pt_info->ui4_image_iMCU_row_num; j++)
                {
                    *(ppi2_iMCU_row_start_addr + (ci * ui4_iTotalIMCURows) + j) = pt_jpg_obj->apui2_buffer[ci] + previous_comp_buf_size;

                    if ((j >= ui4_left) && (j < ui4_right) )
                        previous_comp_buf_size += ui4_tmp  * DCTSIZE2;
                    else /* nz_buffer */
                        previous_comp_buf_size += ui4_tmp  * DCTSIZE2 / 16;
                }
                /* Clear pt_info->buffer for next decode action */
                /* Zero out the buffers as requested */
                x_memset(pt_jpg_obj->apui2_buffer[ci], 0, (pt_info->ui4_total_WS_iMCU_row_num * INSIZE * 8 + (pt_info->ui4_image_iMCU_row_num - pt_info->ui4_total_WS_iMCU_row_num) ) * pt_jpg_obj->aui4_comp_buf_width_in_block[ci] * compptr->i2_v_samp_factor * (DCTSIZE2>>3));
            }
        }
    }
}




