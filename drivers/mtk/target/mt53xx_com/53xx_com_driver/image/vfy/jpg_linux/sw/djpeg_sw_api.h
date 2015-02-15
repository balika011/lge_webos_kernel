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
 * $RCSfile: djpeg_sw_api.h,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/DTV_X_ATSC/1 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: 3e5231eccd67656ec82fdf5453830eab $ Asa
 *
 * Description:
 *         This file define jpeg constants and structure
 *---------------------------------------------------------------------------*/

#ifndef __DJPEG_SW_API_H__
#define __DJPEG_SW_API_H__

/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#include "../jdec_drv.h"

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/
void vInitial_decode_buffer (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, SRC_DST_RECT_T* pt_src_dst_rect, IMG_ROTATE_T e_rotate, BOOL b_sw_dec);
void vStart_input_pass (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj);
void vDecompress_iMCU_data (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, SRC_DST_RECT_T* pt_src_dst_rect);
#if 0
void bilinear_YCBCR_resampling(IMG_JPG_OBJ_T* pt_jpg_obj, SRC_DST_RECT_T* pt_src_dst_rect, IMG_ROTATE_OP_TYPE_T e_ratate_type,GL_LOCK_INFO_T* pt_lock_info, UINT8* pui1_y_start_addr, UINT8* pui1_cb_start_addr, UINT8* pui1_cr_start_addr, UINT8* pui1_next_y_start_addr, UINT8* pui1_next_cb_start_addr, UINT8* pui1_next_cr_start_addr, HANDLE_T h_surf_signal);
void bilinear_Gray_resampling(IMG_JPG_OBJ_T* pt_jpg_obj, SRC_DST_RECT_T* pt_src_dst_rect, IMG_ROTATE_OP_TYPE_T e_ratate_type,GL_LOCK_INFO_T* pt_lock_info, UINT8* pui1_y_start_addr, UINT8* pui1_cb_start_addr, UINT8* pui1_cr_start_addr, UINT8* pui1_next_y_start_addr, UINT8* pui1_next_cb_start_addr, UINT8* pui1_next_cr_start_addr, HANDLE_T h_surf_signal);
void nearest_YCBCR_resampling(IMG_JPG_OBJ_T* pt_jpg_obj, SRC_DST_RECT_T* pt_src_dst_rect, IMG_ROTATE_OP_TYPE_T e_ratate_type,GL_LOCK_INFO_T* pt_lock_info, UINT8* pui1_y_start_addr, UINT8* pui1_cb_start_addr, UINT8* pui1_cr_start_addr, UINT8* pui1_next_y_start_addr, UINT8* pui1_next_cb_start_addr, UINT8* pui1_next_cr_start_addr, HANDLE_T h_surf_signal);
void nearest_Gray_resampling(IMG_JPG_OBJ_T* pt_jpg_obj, SRC_DST_RECT_T* pt_src_dst_rect, IMG_ROTATE_OP_TYPE_T e_ratate_type,GL_LOCK_INFO_T* pt_lock_info, UINT8* pui1_y_start_addr, UINT8* pui1_cb_start_addr, UINT8* pui1_cr_start_addr, UINT8* pui1_next_y_start_addr, UINT8* pui1_next_cb_start_addr, UINT8* pui1_next_cr_start_addr, HANDLE_T h_surf_signal);
#endif
void vMultiple_scan_collect_data (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, UINT32 max_scan_num);
INT16 i2Consume_markers (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj);
void vDecode_mcu_DC_first (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT16 *MCU_data[], BOOL write_to_buf);
void vDecode_mcu_AC_first (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT16 *MCU_data[], BOOL write_to_buf);
void vDecode_mcu_DC_refine (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT16 *MCU_data[], BOOL write_to_buf);
void vDecode_mcu_AC_refine (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT16 *MCU_data[], BOOL write_to_buf);
void vDecode_mcu (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT16 *MCU_data[]);
void vInitial_decompress(JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj);
BOOL fgDecode_jpg(JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, SRC_DST_RECT_T* pt_src_dst_rect);
void vFirst_1D_IDCT(COMPONENT_INFO_T * compptr, INT16 *coef_block, INT16 *wsptr);
void vSecond_1D_IDCT(JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT16 *wsptr, INT8 *out_block, UINT16 ui2_ci);
void vFirst_idct_4x4 (COMPONENT_INFO_T * compptr, INT16 *coef_block, INT16 *wsptr);
void vSecond_idct_4x4 (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT16 *wsptr, INT8 *out_block, UINT16 ui2_ci);
void vIdct_2x2 (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, COMPONENT_INFO_T * compptr, INT16 *coef_block, INT16 *wsptr, INT8 *out_block, UINT16 ui2_ci);
void vIdct_1x1 (COMPONENT_INFO_T * compptr, INT16 *coef_block, INT8 *out_block);
void vConsume_data (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, BOOL write_to_buf);
UINT32 u4BYTERearrange(JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, UINT32 wType, UINT32 dwValue);
void vPer_scan_setup (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj);
void vLatch_quant_tables (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj);
void vSkip_marker (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj);
void vNext_marker (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj);
void vFirst_marker (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj);
void vProcess_restart (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT32 *bits_left);
void vStart_pass_phuff_decoder (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj);
void vJpeg_make_d_derived_tbl (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, BOOL isDC, INT16 tblno,  d_derived_tbl * pdtbl);
INT16 i2Jpeg_huff_decode (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT32 *vld_buffer, INT32 *bits_left, d_derived_tbl * htbl, INT32 min_bits);
void vFill_bit_buffer (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT32 *vld_buffer, INT32 *bits_left, INT32 nbits);
void vRead_restart_marker (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj);
void vFirst_SOS_setup (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj);
void vStart_pass_huff_decoder (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj);
void vStart_iMCU_row (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj);
UINT32 u4Input4bytes (JDEC_INST_T *prJdecInst);
UINT16 u2Input2bytes (JDEC_INST_T *prJdecInst);
UINT8 u1Input1byte (JDEC_INST_T *prJdecInst);
BOOL fgSkipbytes (JDEC_INST_T *prJdecInst, UINT32 ui4_num_bytes);
void vBuild_ycc_rgb_table (INT16 * Cr_r_tab, INT16 * Cb_b_tab, INT32 * Cr_g_tab, INT32 * Cb_g_tab);
void vSet_InputPos (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, UINT32 ui4_pos);
//void vDump_dst_surface(CHAR* ps_outfile_name, GL_LOCK_INFO_T* pt_lock_info);
//void vSet_blit_fun_pt(JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, SRC_DST_RECT_T* pt_src_dst_rect, UINT32 display_image_width, UINT32 display_image_height, IMG_ROTATE_OP_TYPE_T e_ratate_type);
//void vSet_resampling_fun_pt(JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj);

#endif /* __DJPEG_SW_API_H__ */



