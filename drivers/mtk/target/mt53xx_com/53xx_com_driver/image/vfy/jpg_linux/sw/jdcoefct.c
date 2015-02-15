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
 * $RCSfile: jdcoefct.c,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/9 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: 9f6744f98f989c5e56c6a93733ff4724 $ Asa
 *
 * Description: 
 *         This file contains the flow control anout de-huffman and IDCT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#include "x_os.h"
#include "x_common.h"
#include "jpg_config.h"
#include "jpeglib.h"
#include "jdhuff.h"
#include "djpeg_sw_api.h"
#include "../jdec_drv_scale.h"
#include "../jdec_debug.h"
#include "x_debug.h"

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
 * Name:  vMultiple_scan_collect_data
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vMultiple_scan_collect_data (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, UINT32 max_scan_num)
{
    /* 這個無窮迴圈只有在 Parse 過所有的 scans到達 end of image的時候才會跳出來    */
    /* 每一個 scan都會將 image 中所以的 iMCU rows跑過一遍，所以在每一個 iMCU row   */
    /* 我們必須將這次要解的 iMCU rows資料存起來就好了，其他 iMCU rows的資料則只    */
    /* Parsing 不存入 DRAM workspace                                               */
    INT16 retcode;
    UINT32 scan_num;
    register UINT32 i;
    IMG_BASIC_INFO_T* pt_info;
    register UINT16* pui2_input_iMCU_row;
    register UINT32* pui4_begin_decode_iMCU_row;
    
    pt_info = pt_jpg_obj->pt_current_sel; 
    if (pt_info == NULL)
    {
        return;
    }

    scan_num = 0;
    retcode = 0;
    pt_jpg_obj->b_ReachFileEnd = FALSE;
    pui2_input_iMCU_row = &(pt_jpg_obj->ui2_input_iMCU_row);
    pui4_begin_decode_iMCU_row = &(pt_info->ui4_begin_decode_iMCU_row);

    for (i = 0; i < BIG_LOOP; i++)
    {
        /* Absorb some more input */
        if (pt_jpg_obj->b_consume_markers == TRUE)
        {
            retcode = i2Consume_markers (prJdecInst, pt_jpg_obj);
            i4JdecScaleSetJpegInfo(prJdecInst);
        }
        else /* if (scan_num != 0)*/
        {
            if ((*pui2_input_iMCU_row < *pui4_begin_decode_iMCU_row) 
                || (*pui2_input_iMCU_row >= (*pui4_begin_decode_iMCU_row + pt_info->ui4_total_WS_iMCU_row_num) ))
            {
                vConsume_data (prJdecInst, pt_jpg_obj, FALSE);
            }
            else
            {
                vConsume_data (prJdecInst, pt_jpg_obj, TRUE);
            }

        } /*end of if (pt_info->consume_markers_flag == TRUE)*/

        if (*pui2_input_iMCU_row == pt_info->ui4_image_iMCU_row_num)
        {
            scan_num ++;
        }

        if (scan_num == max_scan_num)
            break;

        if ( (retcode == JPEG_REACHED_EOI) || (pt_jpg_obj->b_ReachFileEnd == TRUE) )
            break;


        if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
        {
#ifdef  JPG_MULTI_SCAN_TOLERATE_ERROR   /*Mars,[2002/4/25],to neglect error if some scans are already correctly decoded*/
            if(scan_num >1)
            {
                pt_jpg_obj->e_JPGErrorLevel = JPG_CORRUPT_ERR;
            }
#endif
            //UTIL_Printf("[IMG] ERR: vMultiple_scan_collect_data(): to neglect error if some scans are already correctly decoded.\n\r");
            vJdecLog_L("[JPG_SW] ERR: vMultiple_scan_collect_data(): to neglect error if some scans are already correctly decoded.\n\r");
            return;
        }
    } /* infinite for loop */

    /* Perform any dummy output passes, and set up for the final pass */
}

/*-----------------------------------------------------------------------------
 * Name:  vStart_iMCU_row
 *
 * Description: Reset within-iMCU-row counters for a new row (input side)
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vStart_iMCU_row (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    IMG_BASIC_INFO_T* pt_info = NULL;
    
    pt_info = pt_jpg_obj->pt_current_sel; 
    if (pt_info == NULL)
    {
        return;
    }
    /* In an interleaved scan, an MCU row is the same as an iMCU row.
    * In a noninterleaved scan, an iMCU row has v_samp_factor MCU rows.
    * But at the bottom of the image, process only what's left.
    */
    if (pt_info->ui1_comps_in_scan > 1)
    {
        pt_info->ui2_MCU_rows_per_iMCU_row = 1;
    }
    else
    {
        //[CJYang Resample]
        //if (pt_jpg_obj->ui2_input_iMCU_row < (pt_info->ui4_TotalIMCURows-1))
        if (pt_jpg_obj->ui2_input_iMCU_row < (prJdecInst->u4MCURowNum-1))
        {
            pt_info->ui2_MCU_rows_per_iMCU_row = pt_info->apt_cur_comp_info[0]->i2_v_samp_factor;
        }
        else
        {
            pt_info->ui2_MCU_rows_per_iMCU_row = pt_info->apt_cur_comp_info[0]->i2_last_row_height;
        }
    }
}
/*-----------------------------------------------------------------------------
 * Name:  vConsume_data
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
UINT32 _u4JdecSwChkCount = 0;
extern INT32 i4JdecChkFifoStatus(JDEC_INST_T *prJdecInst);

void vConsume_data (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, BOOL write_to_buf)
{
    UINT32 MCU_col_num; /* index of current MCU within row */
    register UINT16 blkn, ci, xindex, yindex, yoffset;
    UINT32 start_col;
    register INT16 *buffer_ptr;
    COMPONENT_INFO_T *compptr;
    INT16 *MCU_buffer[D_MAX_BLOCKS_IN_MCU];
    register UINT32 compptr_MCU_width;
    register UINT32 compptr_MCU_height;
    IMG_BASIC_INFO_T* pt_info = NULL;
    UINT32 ui4_comps_in_scan;
    UINT32 ui4_MCU_rows_per_iMCU_row;
    UINT32 ui4_MCUs_per_row;
    UINT32 ui4_iTotalIMCURows;
    register INT16** ppi2_iMCU_row_start_addr ;
    register INT16** ppi2_start_addr ;
    UINT32 ui4_comp_buf_width_in_block;
    UINT8 Ah, Ss; 

    
    pt_info = pt_jpg_obj->pt_current_sel; 
    if (pt_info == NULL)
    {
        return;
    }

    ui4_comps_in_scan = pt_info->ui1_comps_in_scan;
    ui4_MCUs_per_row = pt_info->ui2_MCUs_per_row;
    ui4_MCU_rows_per_iMCU_row = pt_info->ui2_MCU_rows_per_iMCU_row;
    //[CJYang Resample]
    //ui4_iTotalIMCURows = pt_info->ui4_TotalIMCURows;
    ui4_iTotalIMCURows = prJdecInst->u4MCURowNum;
    ppi2_iMCU_row_start_addr = pt_jpg_obj->ppui2_iMCU_row_start_addr;
    Ah = pt_info->ui1_Ah;
    Ss = pt_info->ui1_Ss;

    /* Loop to process one whole iMCU row */
    for (yoffset = 0; yoffset < ui4_MCU_rows_per_iMCU_row; yoffset++)
    {
        for (MCU_col_num = 0; MCU_col_num < ui4_MCUs_per_row; MCU_col_num++)
        {
            _u4JdecSwChkCount++;
            if (_u4JdecSwChkCount >= 100)
            {
                i4JdecChkFifoStatus(prJdecInst);
                _u4JdecSwChkCount = 0;
            }
            /* Construct list of pointers to DCT blocks belonging to this MCU */
            blkn = 0;     /* index of current DCT block within MCU */
            if (ui4_comps_in_scan > MAX_COMPS_IN_SCAN)
            {
                ASSERT(0);
            }
            for (ci = 0; ci < ui4_comps_in_scan; ci++) /* 計算這個 MCU單元中，所包含blocks在記憶體中的位置 */
            {
                /* pt_info->apt_cur_comp_info 指標會指向現在 scan中依序出現的 components                                  */
                /* 因為每一個 scan所包含的 scan 數目都不一定，而且不一定是哪一個 component                   */
                /* 所以在 parse scan 的時候，就用 pt_info->cur_comp_info依序記錄出現的 component infomation including */
                /* component_id, 槫所以在這裡取出資料的時候，就要參考現在 component 的 compoenent_id         */
                /* 找出對應的 buffer base address                                                            */
                compptr = pt_info->apt_cur_comp_info[ci];
                ppi2_start_addr = ppi2_iMCU_row_start_addr + ((compptr->i2_component_index) * ui4_iTotalIMCURows);
                start_col = MCU_col_num * compptr->i2_MCU_width;
                compptr_MCU_width = compptr->i2_MCU_width;
                compptr_MCU_height = compptr->i2_MCU_height;
                ui4_comp_buf_width_in_block = pt_jpg_obj->aui4_comp_buf_width_in_block[ci];

                for (yindex = 0; yindex < compptr_MCU_height; yindex++)
                {
                    if (pt_info->b_has_multiple_scans)
                    {
                        if (write_to_buf == FALSE)
                        {
                            buffer_ptr = *(ppi2_start_addr + (pt_jpg_obj->ui2_input_iMCU_row)) + ( (yoffset + yindex) * ui4_comp_buf_width_in_block + start_col)  * DCTSIZE2 / 16;
                        }
                        else
                        {
                            buffer_ptr = *(ppi2_start_addr + (pt_jpg_obj->ui2_input_iMCU_row)) + ((yoffset + yindex) * ui4_comp_buf_width_in_block + start_col)  * DCTSIZE2;
                        }
                    }
                    else
                    {
                        /* 在不是 multiple scan的情況，只用第一個 iMCU row buffer in DRAM workspace */
                        buffer_ptr = *(ppi2_start_addr) + ( (yoffset + yindex) * ui4_comp_buf_width_in_block + start_col)  * DCTSIZE2;
                    }

                    if (write_to_buf == FALSE)
                    {
                        for (xindex = 0; xindex < compptr_MCU_width; xindex++)
                        {
                            // For klocwork issues
                            if (blkn > (D_MAX_BLOCKS_IN_MCU-1))
                            {
                                ASSERT(0); 
                                break;
                            }
                            MCU_buffer[blkn] = buffer_ptr;
                            buffer_ptr = buffer_ptr + 4;
                            blkn = blkn + 1;
                        }
                    }
                    else
                    {
                        for (xindex = 0; xindex < compptr_MCU_width; xindex++)
                        {
                            // For klocwork issues
                            if (blkn > (D_MAX_BLOCKS_IN_MCU-1))
                            {
                                ASSERT(0); 
                                break;
                            }
                            MCU_buffer[blkn] = buffer_ptr;
                            buffer_ptr = buffer_ptr + DCTSIZE2;
                            blkn = blkn + 1;
                        }
                    }
                }
            }

            /* Select MCU decoding routine */
            if (pt_info->b_progressive_mode == TRUE)
            {
                if (Ah == 0)
                {
                    if (Ss == 0)
                        {
                            vDecode_mcu_DC_first (prJdecInst, pt_jpg_obj, MCU_buffer, write_to_buf);
                        }
                    else
                        {
                            vDecode_mcu_AC_first (prJdecInst, pt_jpg_obj, MCU_buffer, write_to_buf);
                        }
                        
                }
                else
                {
                    if (Ss == 0)
                        {
                            vDecode_mcu_DC_refine (prJdecInst, pt_jpg_obj, MCU_buffer, write_to_buf);
                        }
                    else
                        {
                            vDecode_mcu_AC_refine (prJdecInst, pt_jpg_obj, MCU_buffer, write_to_buf);
                        }
                        
                }
            }
            else
            {
                vDecode_mcu (prJdecInst, pt_jpg_obj, MCU_buffer);
            }

            if ( (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR) || (pt_jpg_obj->ui4_JPGErrorType == 26 /* Read VLD data more than file size */ ) )
            {
                //UTIL_Printf("[IMG] ERR: vConsume_data(): Read VLD data more than file size.\n\r");
                vJdecLog_L("[JPG_SW] ERR: vConsume_data(): Read VLD data more than file size.\n\r");
                return;
            }
        } /* Completed an MCU row, but perhaps not an iMCU row */
    } /* Completed the iMCU row, advance counters for next one */

    //[CJYang Resample]
    //if (++(pt_jpg_obj->ui2_input_iMCU_row) < pt_info->ui4_TotalIMCURows)
    if (++(pt_jpg_obj->ui2_input_iMCU_row) < prJdecInst->u4MCURowNum)
    {
        vStart_iMCU_row (prJdecInst, pt_jpg_obj);
        return;
    }
    /* Completed the scan */
    pt_jpg_obj->b_consume_markers = TRUE;
}

/*-----------------------------------------------------------------------------
 * Name:  vDecompress_iMCU_data
 *
 * Description: Decompress and return some data in the multi-pass case.
 *              Always attempts to emit one fully interleaved MCU row ("iMCU" row).
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vDecompress_iMCU_data (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, SRC_DST_RECT_T* pt_src_dst_rect)
{
    UINT32 MCU_col_num; /* index of current MCU within row */
    UINT16 ci, yoffset;
    UINT32 start_col;
    INT16 *block_ptr;
    INT8 *out_ptr;
    UINT32 last_iMCU_row;
    UINT16 block_rows;
    COMPONENT_INFO_T *compptr;
    INT16 workspace[DCTSIZE2];  /* buffers data between passes */
    IMG_BASIC_INFO_T* pt_info = NULL;
    UINT32 clip_width, clip_height, clip_y, clip_x;
    UINT32 block_height;
    UINT32 block_width;
    UINT32 ui4_tmp;
    UINT32 ui4_OutputIMCURow;
    UINT32 ui4_TotalIMCURows;
    UINT32 ui4_downsized_idctsize;
    UINT32 ui4_left;
    UINT32 ui4_right;
    BOOL b_use_second_color_buf;
    INT16** ppi2_iMCU_row_start_addr;
    
    
    pt_info = pt_jpg_obj->pt_current_sel; 
    if (pt_info == NULL)
    {
        return;
    }
    
    clip_height = pt_src_dst_rect->ui4_src_h;
    clip_width = pt_src_dst_rect->ui4_src_w;
    clip_y = pt_src_dst_rect->ui4_src_y;
    clip_x = pt_src_dst_rect->ui4_src_x;
    //[CJYang Resample]
    //ui4_OutputIMCURow = pt_jpg_obj->ui4_OutputIMCURow;
    ui4_OutputIMCURow = prJdecInst->u4CurrDecodeMCURow;
    //[CJYang Resample]
    //ui4_TotalIMCURows = pt_info->ui4_TotalIMCURows;
    ui4_TotalIMCURows = prJdecInst->u4MCURowNum;
    ui4_downsized_idctsize = pt_info->ui4_downsized_idctsize;
    b_use_second_color_buf = pt_info->b_use_second_color_buf;
    ppi2_iMCU_row_start_addr = pt_jpg_obj->ppui2_iMCU_row_start_addr;

    if ((clip_height) || (clip_y))
    {
        ui4_tmp = clip_y + clip_height;
        block_height = ui4_downsized_idctsize * pt_info->ui1_max_v_samp_factor;        
        if ((ui4_OutputIMCURow < (UINT32)(clip_y/block_height)) 
            || (ui4_OutputIMCURow > (UINT32)(((ui4_tmp%block_height)?(ui4_tmp/block_height + 1):(ui4_tmp/block_height)))))
        {
            return;
        }
    }
    
    last_iMCU_row = ui4_TotalIMCURows - 1;
    /* 這個 迴圈會 IDCT 完一個 iMCU row中不同 component 的資料 */
    if (pt_info->ui2_num_components > MAX_COMPS_IN_SCAN)
    {
        ASSERT(0);
    }
    for (ci = 0, compptr = pt_info->at_comp_info; ci < pt_info->ui2_num_components; ci++, compptr++)
    {
        if (ui4_OutputIMCURow < last_iMCU_row)
        {
            block_rows = compptr->i2_v_samp_factor;
        }      
        else
        {
            /* NB: can't use last_row_height here; it is input-side-dependent! */
            /* Here we IDCT an iMCU row which includes v_samp_factor MCU rows of each component */
            block_rows = (INT16) (compptr->ui2_height_in_blocks % compptr->i2_v_samp_factor);

            if (block_rows == 0)
                block_rows = compptr->i2_v_samp_factor;
        }
        
        if (ci == Y_INDEX)
        {
            block_width = ui4_downsized_idctsize;
        }
        else
        {   
            block_width = ui4_downsized_idctsize * pt_info->ui1_max_h_samp_factor; 
        }
        
        ui4_tmp = clip_x + clip_width;
        ui4_left = clip_x / block_width;
        ui4_right = ((ui4_tmp%block_width) ? (ui4_tmp/block_width+1) : (ui4_tmp/block_width));
        
        for (yoffset = 0; yoffset < block_rows; yoffset++)
        {
            for (MCU_col_num = 0; MCU_col_num <  compptr ->ui2_width_in_blocks; MCU_col_num++)
            {
                /* Unlike in vConsume_data() function, we don't have to multiply MCU_col_num by compptr->MCU_width */
                /* We simply do IDCT operation on every blocks in a MCU row                                                                    */
                start_col = MCU_col_num;
                
                if ((clip_width) || (clip_x))
                {   
                    if ((MCU_col_num < ui4_left)
                        ||(MCU_col_num > ui4_right))
                    {
                        continue;
                    }
                }

                /* 算出這個 block in this MCU 的開始 DRAM address */
                if (pt_info->b_has_multiple_scans)
                {
                    block_ptr = *(ppi2_iMCU_row_start_addr + (ci*ui4_TotalIMCURows) + ui4_OutputIMCURow) + ( ( yoffset * pt_jpg_obj->aui4_comp_buf_width_in_block[ci]) + start_col)  * DCTSIZE2;
                } 
                else
                {
                    block_ptr = *(ppi2_iMCU_row_start_addr + (ci*ui4_TotalIMCURows))  + ( ( yoffset *  pt_jpg_obj->aui4_comp_buf_width_in_block[ci]) + start_col) * DCTSIZE2;
                }
                    

                /* 算出這個 block in this color buffer 的開始 DRAM address */
                /* Color buffer address is arranged as pure raster scan mode */
                //[CJYang_ColorBuf]
                //out_ptr = (INT8 *) (pt_jpg_obj->aapui1_color_buf[ci][b_use_second_color_buf] + ( yoffset * pt_info->ui4_pic_width_in_buf[ci] * ui4_downsized_idctsize ) + ( start_col * ui4_downsized_idctsize ));
                if ((ci > (JDEC_COMP_NUM - 1))|| ((UINT8)b_use_second_color_buf > 1))
                {
                    ASSERT(0);
                }
                out_ptr = (INT8 *) (prJdecInst->au4ColorBufSa[b_use_second_color_buf][ci] + ( yoffset * pt_info->ui4_pic_width_in_buf[ci] * ui4_downsized_idctsize ) + ( start_col * ui4_downsized_idctsize ));

                /* 執行這個 block 的 IDCT operation */
                if (pt_info->e_idct_size == EIGHT_BY_EIGHT)
                {
                    vFirst_1D_IDCT(compptr, block_ptr, workspace);
                    vSecond_1D_IDCT(prJdecInst, pt_jpg_obj, workspace, out_ptr, ci);
                }
                else if (pt_info->e_idct_size == FOUR_BY_FOUR)
                {
                    vFirst_idct_4x4 (compptr, block_ptr, workspace);
                    vSecond_idct_4x4 (prJdecInst, pt_jpg_obj, workspace, out_ptr, ci);
                }
                else if (pt_info->e_idct_size == TWO_BY_TWO)
                {
                    vIdct_2x2 (prJdecInst, pt_jpg_obj, compptr, block_ptr, workspace, out_ptr, ci);
                }
                else /* ONE_BY_ONE */
                {
                    vIdct_1x1 (compptr, block_ptr, out_ptr);
                }
            } /* MCU_col_num for loop */
        } /* yoffset for loop: Completed an MCU row, go to process other MCU rows in the iMCU row */
    } /* ci for loop: We have performed IDCT on all blocks of one component in a iMCU row */
}
