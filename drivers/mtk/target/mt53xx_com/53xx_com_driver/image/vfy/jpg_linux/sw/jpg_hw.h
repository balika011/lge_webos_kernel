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
 * $RCSfile: jpg_hw.h,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/1 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: 59e343be09047b7cf9dc29a005ce42fe $ Asa
 *
 * Description: 
 *         This file exports all image library API and constant for 
 *         other module using.
 *---------------------------------------------------------------------------*/



#ifndef _JPG_HW_H_
#define _JPG_HW_H_

/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#include "x_gfx.h"
/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
/* Huffman coding tables. */
typedef struct _HUFF_TBL_T
{
  /* These two fields directly represent the contents of a JPEG DHT marker */
  UINT8 aui1_bits[17];        /* bits[k] = # of symbols with codes of */
  /* length k bits; bits[0] is unused */
  UINT8 aui1_huffval[256];        /* The symbols, in order of incr code length */
  /* This field is used only during compression.  It's initialized FALSE when
   * the table is created, and set TRUE when it's been output to the file.
   * You could suppress output of a table by setting this to TRUE.
   */
} HUFF_TBL_T;

/* Basic info about one component (color channel). */
typedef struct _COMPONENT_INFO_T
{
  /* These values are fixed over the whole image. */
  /* For compression, they must be supplied by parameter setup; */
  /* for decompression, they are read from the SOF marker. */
  INT16 i2_component_id;        /* identifier for this component (0..255) */
  INT16 i2_component_index;    /* its index in SOF or cinfo->comp_info[] */
  INT16 i2_h_samp_factor;        /* horizontal sampling factor (1..4) */
  INT16 i2_v_samp_factor;        /* vertical sampling factor (1..4) */
  INT16 i2_quant_tbl_no;        /* quantization table selector (0..3) */
  /* These values may vary between scans. */
  /* For compression, they must be supplied by parameter setup; */
  /* for decompression, they are read from the SOS marker. */
  /* The decompressor output side may not use these variables. */
  INT16 i2_dc_tbl_no;        /* DC entropy table selector (0..3) */
  INT16 i2_ac_tbl_no;        /* AC entropy table selector (0..3) */

  /* Remaining fields should be treated as private by applications. */

  /* These values are computed during compression or decompression startup: */
  /* Component's size in DCT blocks.
   * Any dummy blocks added to complete an MCU are not counted; therefore
   * these values do not depend on whether a scan is interleaved or not.
   */
  UINT16 ui2_width_in_blocks;
  UINT16 ui2_height_in_blocks;

  /* This flag is used only for decompression.  In cases where some of the
   * components will be ignored (eg grayscale output from YCbCr image),
   * we can skip most computations for the unused components.
   */
  BOOL b_component_needed;    /* do we need the value of this component? */

  /* These values are computed before starting a scan of the component. */
  /* The decompressor output side may not use these variables. */
  INT16 i2_MCU_width;        /* number of blocks per MCU, horizontally */
  INT16 i2_MCU_height;        /* number of blocks per MCU, vertically */
  INT16 i2_MCU_blocks;        /* MCU_width * MCU_height */

  INT16 i2_last_row_height;    /* # of non-dummy blocks down in last MCU */

  /* Saved quantization table for component; NULL if none yet saved.
   * See jdinput.c comments about the need for this information.
   * This field is currently used only for decompression.
   */
  UINT16 aui2_quant_table[DCTSIZE2];
  BOOL b_quant_table_saved;    /* We save quantization table yet ? */
  /* Private per-component storage for DCT or IDCT subsystem. */
  INT16 ai2_dct_table[DCTSIZE2];
} COMPONENT_INFO_T;




#endif /* _JPG_HW_H_ */

