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
 * $RCSfile: jdhuff.h,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/6 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: 613a084812f02f189937f566b4c7f69d $ Asa
 *
 * Description: 
 *         This file define DE_HUFFMAN structure and macro
 *---------------------------------------------------------------------------*/

#ifndef _JDHUFF_H_
#define _JDHUFF_H_

/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#include "x_common.h"
//#include "x_jpg_hw.h"
#include "jpeglib.h"
#include "jpg_hw.h"
/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#if 0
#define INPUT_BYTE(obj, out)    out = (obj)->t_input.fp_input1byte_fct(obj);
        
#define INPUT_2BYTES(obj, out)  out = (obj)->t_input.fp_input2bytes_fct(obj);
        
#define INPUT_4BYTES(obj, out)  out = (obj)->t_input.fp_input4bytes_fct(obj);

#define OUTPUT_BYTE(obj, in, pos)   (obj)->t_input.fp_output1byte_fct(obj, in, pos);

#define SKIP_BYTES(obj, num)        (obj)->t_input.fp_skipbytes_fct(obj, num);
#else
#endif
/* Derived data constructed for each Huffman table */
#define HUFF_LOOKAHEAD  6  /* # of bits of lookahead */
/*
 * Fetching the next N bits from the input stream is a time-critical operation
 * for the Huffman decoders.  We implement it with a combination of inline
 * macros and out-of-line subroutines.  Note that N (the number of bits
 * demanded at one time) never exceeds 15 for JPEG use.
 *
 * We read source bytes into get_buffer and dole out bits as needed.
 * If get_buffer already contains enough bits, they are fetched in-line
 * by the macros CHECK_BIT_BUFFER and GET_BITS.  When there aren't enough
 * bits, jpeg_vFill_bit_buffer is called; it will attempt to fill get_buffer
 * as full as possible (not just to the number of bits needed; this
 * prefetching reduces the overhead cost of calling jpeg_vFill_bit_buffer).
 * Note that jpeg_vFill_bit_buffer may return FALSE to indicate suspension.
 * On TRUE return, jpeg_fill_bit_buffer guarantees that get_buffer contains
 * at least the requested number of bits --- dummy zeroes are inserted if
 * necessary.
 */

#define BIT_BUF_SIZE  32  /* size of buffer in bits */

/* If long is > 32 bits on your machine, and shifting/masking longs is
 * reasonably fast, making bit_buf_type be long and setting BIT_BUF_SIZE
 * appropriately should be a win.  Unfortunately we can't define the size
 * with something like  #define BIT_BUF_SIZE (sizeof(bit_buf_type)*8)
 * because not all machines measure sizeof in 8-bit bytes.
 */

#define MIN_GET_BITS  (BIT_BUF_SIZE-7)

#define HUFF_DECODE(prJdecInst, jpg_obj, result, vld_buffer, bits_left, htbl, slowlabel) \
{  INT32 nb, look; \
  if (bits_left < HUFF_LOOKAHEAD) { \
    vFill_bit_buffer (prJdecInst, jpg_obj, &vld_buffer, &bits_left, 0); \
    if (bits_left < HUFF_LOOKAHEAD) { \
      nb = 1; goto slowlabel; }} \
  look = PEEK_BITS(vld_buffer, bits_left, HUFF_LOOKAHEAD); \
  if ((nb = htbl->ai2_look_nbits[look]) != 0) \
  { \
    DROP_BITS(vld_buffer, bits_left, nb); \
    result = htbl->aui1_look_sym[look]; \
  } else { \
    nb = HUFF_LOOKAHEAD+1; \
slowlabel: \
    result = i2Jpeg_huff_decode (prJdecInst, jpg_obj, &vld_buffer, &bits_left, htbl, nb); } \
}


#define HUFF_DECODE_DC(jpg_obj, result, vld_buffer, bits_left, htbl) \
{  \
    result = i2Jpeg_huff_decode (jpg_obj, &vld_buffer, &bits_left, htbl, 1);\
}
/*
 * These macros provide the in-line portion of bit fetching.
 * Use CHECK_BIT_BUFFER to ensure there are N bits in get_buffer
 * before using GET_BITS, PEEK_BITS, or DROP_BITS.
 * The variables get_buffer and bits_left are assumed to be locals,
 * but the state struct might not be (i2Jpeg_huff_decode needs this).
 *  CHECK_BIT_BUFFER(state,n,action);
 *    Ensure there are N bits in get_buffer; if suspend, take action.
 *      val = GET_BITS(n);
 *    Fetch next N bits.
 *      val = PEEK_BITS(n);
 *    Fetch next N bits without removing them from the buffer.
 *  DROP_BITS(n);
 *    Discard next N bits.
 * The value N should be a simple variable, not an expression, because it
 * is evaluated multiple times.
 */  
#define GET_BITS(vld_buffer, bits_left, min_bits) \
  (((INT16) (vld_buffer >> (bits_left -= (min_bits)))) & ((1<<(min_bits))-1))

#define PEEK_BITS(vld_buffer, bits_left, min_bits) \
  (((INT16) (vld_buffer >> (bits_left -  (min_bits)))) & ((1<<(min_bits))-1))
  

#define DROP_BITS(vld_buffer, bits_left, min_bits) \
  (bits_left -= (min_bits))

#define CHECK_BIT_BUFFER(prJdecInst, jpg_obj, vld_buffer, bits_left, nbits) \
  { if (bits_left < (nbits)) { \
        vFill_bit_buffer (prJdecInst, jpg_obj, &vld_buffer, &bits_left, nbits); }}  

typedef struct {
    /* Basic tables: (element [0] of each array is unused) */
    INT32 ai4_maxcode[18];    /* largest code of length k (-1 if none) */
    /* (maxcode[17] is a sentinel to ensure i2Jpeg_huff_decode terminates) */
    INT32 ai4_valoffset[17];    /* huffval[] offset for codes of length k */
    /* valoffset[k] = huffval[] index of 1st symbol of code length k, less
    * the smallest code of length k; so given a code of length k, the
    * corresponding symbol is huffval[code + valoffset[k]]
    */
    /* Link to public Huffman table (needed only in i2Jpeg_huff_decode) */
    HUFF_TBL_T *pub;
    /* Lookahead tables: indexed by the next HUFF_LOOKAHEAD bits of
    * the input data stream.  If the next Huffman code is no more
    * than HUFF_LOOKAHEAD bits long, we can obtain its length and
    * the corresponding symbol directly from these tables.
    */
    INT16 ai2_look_nbits[1<<HUFF_LOOKAHEAD]; /* # bits, or 0 if too long */
    UINT8 aui1_look_sym[1<<HUFF_LOOKAHEAD]; /* symbol, or unused */
} d_derived_tbl;

typedef struct {
    UINT16 ui2_EOBRUN;      /* remaining EOBs in EOBRUN */
    INT16 ai2_last_dc_val[MAX_COMPS_IN_SCAN]; /* last DC coef for each component */
} savable_state;


typedef struct _HUFF_ENTROPY_DECODER_T{
    /* This is here to share code between baseline and progressive decoders; */
    /* other modules probably should not use it */
    BOOL insufficient_data; /* set TRUE after emitting warning */
    /* These fields are loaded into local variables at start of each MCU.
    * In case of suspension, we exit WITHOUT updating them.
    */
    INT32 vld_buffer; /* current bit-extraction buffer */
    INT32 bits_left;   /* # of unused bits in it */  
    savable_state saved;    /* Other state at start of MCU */
    /* These fields are NOT loaded into local working state. */
    UINT16 restarts_to_go;  /* MCUs left in this restart interval */
    /* Pointers to derived tables (these workspaces have image lifespan) */
    d_derived_tbl dc_derived_tbls[NUM_HUFF_TBLS];
    d_derived_tbl ac_derived_tbls[NUM_HUFF_TBLS];
    d_derived_tbl derived_tbls[NUM_HUFF_TBLS];    
    /* Precalculated info set up by start_pass for use in vDecode_mcu: */
    /* Pointers to derived tables to be used for each block within an MCU */
    d_derived_tbl * dc_cur_tbls[D_MAX_BLOCKS_IN_MCU];
    d_derived_tbl * ac_cur_tbls[D_MAX_BLOCKS_IN_MCU];
    d_derived_tbl * ac_derived_tbl; /* active table during an AC scan */  
    /* Whether we care about the DC and AC coefficient values for each block */
    BOOL dc_needed[D_MAX_BLOCKS_IN_MCU];
    BOOL ac_needed[D_MAX_BLOCKS_IN_MCU];
} HUFF_ENTROPY_DECODER_T;

/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/
#endif /* _JDHUFF_H_ */


