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
 * $RCSfile: djpeg.h,v $
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

#ifndef __DJPEG_H__
#define __DJPEG_H__

/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#include "x_common.h"
#include "x_handle.h"
#include "x_os.h"
//#include "x_fm.h"
#include "jdhuff.h"
//#include "x_img.h"
#include "x_rm.h"

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
/************************************************************************/
/* element of object structure -- input module                          */
/************************************************************************/

//typedef IMG_INPUT_T IMG_JPG_INPUT_T;
//typedef UINT32 IMG_JPG_INPUT_T;		//[CJYang]0819 need to fix it
extern INT32 UTIL_Printf(const CHAR *format, ...);

/************************************************************************/
/* element of object structure -- header info module                    */
/************************************************************************/
typedef struct _IMG_BASIC_INFO_T
{
    /* last decode action of progressive_scan ? */
    BOOL b_last_p_scan_decode;
    BOOL b_saw_SOF; /* found SOF? */
    BOOL b_use_second_color_buf; /* use first or second color buffer to store decompressed or resampling data */
    BOOL b_has_multiple_scans; /* True if file has multiple scans */
    BOOL b_progressive_mode; /* TRUE if scan script uses progressive mode */
    BOOL b_first_SOS; /* first scan ? */
    BOOL b_big_endian;
    BOOL b_exif_exist;
    BOOL b_load_abort;

    UINT8 ui1_Ss, ui1_Se, ui1_Ah, ui1_Al; /* progressive JPEG parameters for scan */
    UINT8 ui1_next_restart_num; /* next restart number expected (0-7) */
    UINT8 ui1_max_h_samp_factor; /* largest h_samp_factor */
    UINT8 ui1_max_v_samp_factor; /* largest v_samp_factor */
    UINT8 ui1_input_scan_number;
    UINT8 ui1_comps_in_scan; /* number of components encoded in this scan */

    UINT16 ui2_p_scan_decode_num; /* We have decoded many times of decoding a progressive scan image */
    UINT16 ui2_image_width, ui2_image_height; /* define input image width and height */
    UINT16 ui2_num_components;/* number of components in this image file */
    UINT16 ui2_MCUs_per_row; /* # of MCUs across the image */
    UINT16 ui2_blocks_in_MCU; /* # of DCT blocks per MCU */
    UINT16 ui2_restart_interval; /* MCUs per restart, or 0 for no restart */
    /* MCU_membership[i] is index in cur_comp_info of component owning */
    /* i'th block in an MCU */
    UINT16 aui2_MCU_membership[D_MAX_BLOCKS_IN_MCU];
    UINT16 ui2_MCU_rows_per_iMCU_row; /* number of such rows needed */
    UINT16 aaui2_quantval[NUM_QUANT_TBLS][DCTSIZE2];/* quantization table */

    /* loading progress status from 0 to 100 */
    UINT32 ui4_load_progress;
    UINT32 ui4_image_iMCU_row_num; /* the image iMCU row number */
    //UINT32 ui4_TotalIMCURows;     //[CJYang Resample]
    UINT32 ui4_pic_width_in_buf[MAX_COMPS_IN_SCAN];/* picture width in buffer and it is the least number which is a multiples of 8 */
    UINT32 ui4_downsized_idctsize;
    UINT32 ui4_downsized_idctsize2;
    UINT32 ui4_begin_decode_iMCU_row; /* record the start decoding iMCU row number of this multiple scan JPEG file */
    /* total iMCU row number in the given DRAM workspace */
    UINT32 ui4_total_WS_iMCU_row_num;
    UINT32 ui4_compressed_data_begin_pos;
    UINT32 ui4_compressed_data_end_pos;
    UINT32 ui4_pm_data_begin_pos;       /* data begin position in the picture mode */

    COMPONENT_INFO_T* apt_cur_comp_info[MAX_COMPS_IN_SCAN];
    /* DC & AC huffman tables */
    HUFF_TBL_T            at_dc_huff_tbl[NUM_HUFF_TBLS];
    HUFF_TBL_T            at_ac_huff_tbl[NUM_HUFF_TBLS];
    COMPONENT_INFO_T      at_comp_info[MAX_COMPS_IN_SCAN];

    SCALE_DOWN_LEVEL_T  e_scaledown;
    IDCT_SIZE_ENUM_T    e_idct_size; /* perfom 8x8, 4x4, 2x2 or 1x1 IDCT operation */
    //IMG_LOAD_STATUS_T   e_load_stat;

    /* EXIF info */
    UINT32              ui4_orientation_tag_pos;
    BOOL                b_orientation_tag_exist;
    //IMG_EXIF_INFO_T     t_exif_info;

    VOID        *pv_aux_cache;
    UINT32      ui4_img_id;

    UINT16              ui2_dec_comp;
    HANDLE_T            h_queue;
    HANDLE_T            h_reentry_lock;
    HANDLE_T            *ph_decoder;
    DRV_TYPE_T          t_dec_type;
} IMG_BASIC_INFO_T;

typedef struct _IMG_THUMBNAIL_T
{
    IMG_BASIC_INFO_T t_basic;
} IMG_THUMBNAIL_T;

typedef struct _IMG_PRIMARY_JPG_T
{
    IMG_BASIC_INFO_T t_basic;
} IMG_PRIMARY_JPG_T;

typedef struct _SRC_DST_RECT_T
{
    UINT32      ui4_src_x;
    UINT32      ui4_src_y;
    UINT32      ui4_src_w;
    UINT32      ui4_src_h;
    UINT32      ui4_dst_x;
    UINT32      ui4_dst_y;
    UINT32      ui4_dst_w;
    UINT32      ui4_dst_h;

    BOOL        b_flip;
} SRC_DST_RECT_T;

//typedef void (*blit_pixel_fct) (UINT32 x, UINT32 y, UINT8 R, UINT8 G, UINT8 B, UINT32 ui4_pitch, UINT8* pui1_phase1_data);
//typedef void (*resampling_fct) (VOID* pt_jpg_obj, SRC_DST_RECT_T* pt_src_dst_rect, IMG_ROTATE_T e_ratate_type,GL_LOCK_INFO_T* pt_lock_info, UINT8* pui1_y_start_addr, UINT8* pui1_cb_start_addr, UINT8* pui1_cr_start_addr, UINT8* pui1_next_y_start_addr, UINT8* pui1_next_cb_start_addr, UINT8* pui1_next_cr_start_addr, HANDLE_T h_surf_signal);

/************************************************************************/
/* whole jpeg object structure                                          */
/************************************************************************/
typedef struct _KEEP_RESAMPLE_X_PARAMETERS
{
    UINT32 next_x_index_Y;
    UINT32 next_x_index_CbCr;
    UINT32 src_x_pos_quotient_Y;
    UINT32 src_x_pos_quotient_CbCr;
    UINT32 scale_up_src_x_pos_remainder;
} KEEP_RESAMPLE_X_PARAMETERS;

/* This structure hold JPEG object after call the function _img_jpg_eng_parse */
typedef struct _IMG_JPG_OBJ_T
{
    /* improve reading from file speed so pre-fetch a buffer of data */
    //PREFETCH_RING_BUFFER t_prefetch_ring_buf;/* directly type casting so do not move it to elsewhere*/

    BOOL b_arith_code;        /* TRUE=arithmetic coding, FALSE=Huffman */
    BOOL b_APP1Exist;
    BOOL b_saw_SOI; /* Have we see start_of_image marker yet? */
    BOOL b_ReachFileEnd;
    BOOL b_consume_markers; /* consume marker or input data ? */

    //[CJYang_ColorBuf]
    //UINT8* aapui1_color_buf[MAX_COMPS_IN_SCAN][2];/* color conversion buffer base address pointers in DRAM */
    UINT8 ui1_data_precision;

    UINT16 ui2_discarded_bytes; /* # of bytes skipped looking for a marker */
    UINT16 ui2_input_iMCU_row; /* Number of iMCU rows completed */
    UINT16 ui2_output_image_lines;/* record the number of image lines outputted to display buffer */
    UINT16 ui2_num_of_resampling;  /* number of iMCU row resampling */

    INT16 i2_unread_marker;
    INT16* apui2_buffer[MAX_COMPS_IN_SCAN];/* component coefficient buffer base address pointers in DRAM */
    INT16** ppui2_iMCU_row_start_addr; /* record the start DRAM address of each component of each iMCU row */
    INT16 ai2_Cr_r_tab[MAXJSAMPLE + 1];
    INT16 ai2_Cb_b_tab[MAXJSAMPLE + 1];

    INT32 ai4_Cr_g_tab[MAXJSAMPLE + 1];
    INT32 ai4_Cb_g_tab[MAXJSAMPLE + 1];

    UINT32 ui4_JPGFileSizeInByte;   /* jpeg file size in byte */
    UINT32 aui4_comp_buf_width_in_block[MAX_COMPS_IN_SCAN];/* component buffer width in pixel */
    //UINT32 ui4_OutputIMCURow;  /* Number of iMCU rows processed */
    UINT32 ui4_JPGReadByteNum;   /* number of byte read from VLD */
#if 0
    UINT32 ui4_display_image_width;
    UINT32 ui4_display_image_height;
    UINT32 ui4_h_pixel_ratio;
    UINT32 ui4_v_pixel_ratio;
    UINT32 ui4_org_comp_Y_image_height_sub_1;
    UINT32 ui4_org_comp_Y_image_width_sub_1;
    UINT32 ui4_v_samp_factor_X_DCTSIZE;
#endif

    UINT8* pui1_blit_fct_phase1_data; /* split blit_pixel_fct to phase 1 and phase 2, and move common data to phase 1 */
    //blit_pixel_fct pf_blit_phase2_fct; /* hook blit_pixel_fct function point */
    //resampling_fct pf_resampling_fct;/* hook resampling_fct function point */

    HUFF_ENTROPY_DECODER_T entropy;
    IMG_THUMBNAIL_T t_thumbnail;
    IMG_PRIMARY_JPG_T t_primary;
    IMG_BASIC_INFO_T* pt_current_sel;
    //JPG_HW_DECODED_CLR_BUF_INFO_T       at_hw_dbl_clr_buf_info[MAX_DOUBLE_BUFFER_NUM];

    UINT32 ui4_JPGErrorType;  /* - indicates JPEG decoding error type */
    JPGERRORLEVELENUM_T   e_JPGErrorLevel;  /* indicates JPEG decoding error level */
    KEEP_RESAMPLE_X_PARAMETERS* pt_keep_x_params;

    UINT32  ui4_prev_output_lines;
    UINT32  ui4_total_mcu_rows;

#ifdef USE_EXIF_ORIENTATION_TAG
    BOOL _fgOrientationTagExist;
    UINT16 _wExifOfst;
    UINT16 _wIDF0CntOfst;
    UINT16 _wTagCount;
    UINT32 _dwOrientation;
    UINT32 _dwOrgOrientation;
    UINT32 _dwCurOrientation;
    BOOL _fgTagSaved;
    BOOL _fgAPP1Skip;
    UINT32 _dwAPP1SkipLen;
#endif /* EXIF_ORIENTATION_TAG */

    HANDLE_T    h_decode_lock;
} IMG_JPG_OBJ_T;


#endif /* __DJPEG_H__ */



