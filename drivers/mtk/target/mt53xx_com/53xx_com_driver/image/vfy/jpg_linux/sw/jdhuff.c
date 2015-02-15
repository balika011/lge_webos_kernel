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
 * $RCSfile: jdhuff.c,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/DTV_X_ATSC/2 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: 2c8ebc94f95e6e4ab8690fffd431f9c5 $ Asa
 *
 * Description:
 *         This file contain de-huffman routine
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#include "x_os.h"
#include "x_common.h"
#include "jpg_config.h"
#include "jpeglib.h"
#include "djpeg_sw_api.h"
#include "jdhuff.h"
#include "../jdec_debug.h"
#include "x_debug.h"

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
                    extern data declarations
 ----------------------------------------------------------------------------*/
extern const INT16 jdec_natural_order[DCTSIZE2+16];
/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Name:  vJpeg_make_d_derived_tbl
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/

extern UINT32 u4Input4bytes (JDEC_INST_T *prJdecInst);
extern UINT16 u2Input2bytes (JDEC_INST_T *prJdecInst);
extern UINT8 u1Input1byte (JDEC_INST_T *prJdecInst);
extern BOOL fgSkipbytes (JDEC_INST_T *prJdecInst, UINT32 ui4_num_bytes);

#define INPUT_BYTE(obj, out)    out = u1Input1byte(obj);
#define INPUT_2BYTES(obj, out)  out = u2Input2bytes(obj);
#define INPUT_4BYTES(obj, out)  out = u4Input4bytes(obj);
#define SKIP_BYTES(obj, num)    fgSkipbytes(obj, num);


void vJpeg_make_d_derived_tbl (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, BOOL isDC, INT16 tblno,  d_derived_tbl * pdtbl)
{
    HUFF_TBL_T *htbl;
    register INT16 p, i, l, si, numsymbols, k;
    INT16 lookbits, ctr;
    INT8 huffsize[257];
    UINT16 huffcode[257];
    UINT16 code;
    register IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }
    x_memset((void *)huffcode, 0, 257);
    /* Note that huffsize[] and huffcode[] are filled in code-length order,
    * paralleling the order of the symbols themselves in htbl->huffval[].
    */

    /* Find the input Huffman table */

    if (tblno < 0 || tblno >= NUM_HUFF_TBLS)
    {

        //UTIL_Printf("[IMG] Huffman table 0x%02x was not defined", tblno);
        vJdecLog_L("[JPG_SW] Huffman table 0x%02x was not defined \n", tblno);

        if (pt_jpg_obj->e_JPGErrorLevel != JPG_FATAL_ERR)
        {
            pt_jpg_obj->ui4_JPGErrorType = 2;
            pt_jpg_obj->e_JPGErrorLevel = JPG_CORRUPT_ERR;
            //UTIL_Printf("[IMG] ERR: vJpeg_make_d_derived_tbl(): Huffman table 0x%02x was not defined", tblno);
            vJdecLog_L("[JPG_SW] ERR: vJpeg_make_d_derived_tbl(): Huffman table 0x%02x was not defined \n", tblno);
        }
        /* Error handler:
           set tblno to NUM_HUFF_TBLS, i.e. the least used table*/
        tblno = NUM_HUFF_TBLS;
    }

    if (tblno > (NUM_HUFF_TBLS - 1))
    {
        ASSERT(0);
    }
    htbl = isDC ? &pt_info->at_dc_huff_tbl[tblno] : &pt_info->at_ac_huff_tbl[tblno];

    pdtbl->pub = htbl;    /* fill in back link */

    /* Figure C.1: make table of Huffman code length for each symbol */
    p = 0;
    for (l = 1; l <= 16; l++)
    {
        i = (INT16) htbl->aui1_bits[l];

        if (i < 0 || p + i > 256) /* protect against table overrun */
        {
            //UTIL_Printf("[IMG] Bogus Huffman table definition\n");
            vJdecLog_L("[JPG_SW] Bogus Huffman table definition\n");

            if (pt_jpg_obj->e_JPGErrorLevel != JPG_FATAL_ERR)
            {
                pt_jpg_obj->ui4_JPGErrorType = 3;
                pt_jpg_obj->e_JPGErrorLevel = JPG_CORRUPT_ERR;
                //UTIL_Printf("[IMG] ERR: vJpeg_make_d_derived_tbl(): Bogus Huffman table definition\n\r");
                vJdecLog_L("[JPG_SW] ERR: vJpeg_make_d_derived_tbl(): Bogus Huffman table definition\n\r");
            }
            /* Error handler:
               set to nearest value*/
            if (i < 0)
                i = 0;
            else /* p + i > 256 */
                i = 255 - p;  /* p + i = 255 */
        }

        while (i--)  /* safe while */
            huffsize[p++] = (char) l;
    }
    huffsize[p] = 0;
    numsymbols = p;

    /* Figure C.2: generate the codes themselves */
    /* We also validate that the counts represent a legal Huffman code tree. */
    code = 0;
    si = huffsize[0];
    p = 0;

    for (k = 0; k < BIG_LOOP; k++)
    {
        if (p > 256)
        {
            ASSERT(0);
            break;
        }
        if (huffsize[p])
        {
            for (i = 0; i < BIG_LOOP; i++)
            {
                if (((INT16) huffsize[p]) == si)
                {
                    huffcode[p++] = code;
                    code++;
                    if (p > 256)
                    {
                        ASSERT(0);
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
            /* code is now 1 more than the last code used for codelength si; but
            * it must still fit in si bits, since no code is allowed to be all ones.
            */

            if (((INT32) code) >= (((INT32) 1) << si))
            {
                //UTIL_Printf("[IMG] Bogus Huffman table definition\n");
                vJdecLog_L("[JPG_SW] Bogus Huffman table definition\n");

                if (pt_jpg_obj->e_JPGErrorLevel != JPG_FATAL_ERR)
                {
                    pt_jpg_obj->ui4_JPGErrorType = 3;
                    pt_jpg_obj->e_JPGErrorLevel = JPG_CORRUPT_ERR;
                    //UTIL_Printf("[IMG] ERR: vJpeg_make_d_derived_tbl(): Bogus Huffman table definition\n\r");
                    vJdecLog_L("[JPG_SW] ERR: vJpeg_make_d_derived_tbl(): Bogus Huffman table definition\n\r");
                }
                /* Error handler:
                   set to 0*/
                code = 0;
            }

            code <<= 1;
            si++;
        }/* huffsize[9]*/
        else
        {
            break;
        }
    }/* BIG_LOOP*/

    /* Figure F.15: generate decoding tables for bit-sequential decoding */
    p = 0;
    for (l = 1; l <= 16; l++)
    {
        if (htbl->aui1_bits[l])
        {
            /* valoffset[l] = huffval[] index of 1st symbol of code length l,
            * minus the minimum code of length l
            */
            pdtbl->ai4_valoffset[l] = (INT32) p - (INT32) huffcode[p];
            p += htbl->aui1_bits[l];
            // For klocwork issues
            if ((p > 257) || (p < 1))
            {
                ASSERT(0);
            }
            pdtbl->ai4_maxcode[l] = huffcode[p-1]; /* maximum code of length l */
        }
        else
        {
            pdtbl->ai4_valoffset[l] = 0;     /* clear to zero if no codes of this length */
            pdtbl->ai4_maxcode[l] = -1; /* -1 if no codes of this length */
        }
    }
    pdtbl->ai4_maxcode[17] = 0xFFFFFL; /* ensures i2Jpeg_huff_decode terminates */

    /* Compute lookahead tables to speed up decoding.
    * First we set all the table entries to 0, indicating "too long";
    * then we iterate through the Huffman codes that are short enough and
    * fill in all the entries that correspond to bit sequences starting
    * with that code.
    */

    for (i = 0; i < (1<<HUFF_LOOKAHEAD); i++)
        pdtbl->ai2_look_nbits[i] = 0;

    p = 0;
    for (l = 1; l <= HUFF_LOOKAHEAD; l++)
    {
        for (i = 1; i <= (INT16) htbl->aui1_bits[l]; i++, p++)
        {
            /* l = current code's length, p = its index in huffcode[] & huffval[]. */
            /* Generate left-justified code followed by all possible bit sequences */
            lookbits = huffcode[p] << (HUFF_LOOKAHEAD-l);
            for (ctr = 1 << (HUFF_LOOKAHEAD-l); ctr > 0; ctr--)
            {
                pdtbl->ai2_look_nbits[lookbits] = l;
                pdtbl->aui1_look_sym[lookbits] = htbl->aui1_huffval[p];
                lookbits++;
            }
        }
    }

    /* Validate symbols as being reasonable.
    * For AC tables, we make no check, but accept all byte values 0..255.
    * For DC tables, we require the symbols to be in range 0..15.
    * (Tighter bounds could be applied depending on the data depth and mode,
    * but this is sufficient to ensure safe decoding.)
    */
    if (isDC)
    {
        for (i = 0; i < numsymbols; i++)
        {
            INT16 sym = htbl->aui1_huffval[i];
            if (sym < 0 || sym > 15)
            {
                //UTIL_Printf("[IMG] Bogus Huffman table definition\n");
                vJdecLog_L("[JPG_SW] Bogus Huffman table definition\n");

                if (pt_jpg_obj->e_JPGErrorLevel != JPG_FATAL_ERR)
                {
                    pt_jpg_obj->ui4_JPGErrorType = 3;
                    pt_jpg_obj->e_JPGErrorLevel = JPG_CORRUPT_ERR;
                    //UTIL_Printf("[IMG] ERR: vJpeg_make_d_derived_tbl(): Bogus Huffman table definition.\n\r");
                    vJdecLog_L("[JPG_SW] ERR: vJpeg_make_d_derived_tbl(): Bogus Huffman table definition.\n\r");
                }
                /* Error handler:
                  set to nearest value */
            if (sym < 0)
                sym = 0;
            else /* sym > 15 */
                sym = 15;
            }
        }
    }
}


/*-----------------------------------------------------------------------------
 * Name:  vFill_bit_buffer
 *
 * Description: Load up the bit buffer to a depth of at least nbits. Becasue this function be called frequently, 
 *                      directly use pt_jpg_obj->puc_prefetch_buf.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vFill_bit_buffer (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT32 *vld_buffer, INT32 *bits_left, INT32 nbits)
{
    /* Attempt to load at least MIN_GET_BITS bits into get_buffer. */
    /* (It is assumed that no request will be for more than that many bits.) */
    /* We fail to do so only if we hit a marker or are forced to suspend. */

    register INT32 vld_buffer_reg;
    register INT32 c;
    register INT32 bits_left_reg;
    register UINT32 i;

    vld_buffer_reg = *vld_buffer;
    bits_left_reg = *bits_left;

    if (pt_jpg_obj->i2_unread_marker == 0) /* cannot advance past a marker */
    {
        while (bits_left_reg < MIN_GET_BITS)  /* safe while */
        {
            /*
               The execution loop number = 348013 in Picture CD
            */
            /* Attempt to read a byte */
            INPUT_BYTE(prJdecInst, c);

            /* If it's 0xFF, check and discard stuffed zero byte */
            if (c == 0xFF)
            {
                /* Loop here to discard any padding FF's on terminating marker,
                * so that we can save a valid pt_info->unread_marker value.  NOTE: we will
                * accept multiple FF's followed by a 0 as meaning a single FF data
                * byte.  This data pattern is not valid according to the standard.
                */
                i = 0;
                do
                {
                    /*
                     The execution loop number = 746 in Picture CD
                     Attempt to read a byte */
                    INPUT_BYTE(prJdecInst, c);
                    i++;
                } while ( (c == 0xFF) && (i <= BIG_LOOP) );  /* safe while */

                if (c == 0)
                {
                    /* Found FF/00, which represents an FF data byte */
                    c = 0xFF;
                }
                else
                {
                    /* Oops, it's actually a marker indicating end of compressed data.
                    * Save the marker code for later use.
                    * Fine point: it might appear that we should save the marker into
                    * bitread working state, not straight into permanent state.  But
                    * once we have hit a marker, we cannot need to suspend within the
                    * current MCU, because we will read no more bytes from the data
                    * source.  So it is OK to update permanent state right away.
                    */
                    pt_jpg_obj->i2_unread_marker = (INT16)c;

                    /* See if we need to insert some fake zero bits. */
                    goto no_more_bytes;
                }
            }

            /* OK, load c into vld_buffer */
            vld_buffer_reg = (vld_buffer_reg << 8) | c;
            bits_left_reg += 8;
        } /* end while */
    }
    else
    {
        no_more_bytes:
        /* We get here if we've read the marker that terminates the compressed
        * data segment.  There should be enough bits in the buffer register
        * to satisfy the request; if so, no problem.
        */
        if (nbits > bits_left_reg)
        {
            /* Uh-oh.  Report corrupted data to user and stuff zeroes into
            * the data stream, so that we can produce some kind of image.
            * We use a nonvolatile flag to ensure that only one warning message
            * appears per data segment.
            */
            if (!pt_jpg_obj->entropy.insufficient_data)
            {
                //UTIL_Printf("[IMG] Corrupt JPEG data: premature end of data segment\n");
                vJdecLog_L("[JPG_SW] Corrupt JPEG data: premature end of data segment\n");

                /* Canon DSC powershot a20 010421-2122-28.jpg's thumbnail require two more bits */
                /* Error handler:
                 Fill the buffer with zero bits*/
                pt_jpg_obj->entropy.insufficient_data = TRUE;
            }
            /* Fill the buffer with zero bits */
            vld_buffer_reg <<= (MIN_GET_BITS - bits_left_reg);
            bits_left_reg = MIN_GET_BITS;
        }
    }

    *vld_buffer = vld_buffer_reg;
    *bits_left = bits_left_reg;
}
/*-----------------------------------------------------------------------------
 * Name:  i2Jpeg_huff_decode
 *
 * Description: Out-of-line code for Huffman code decoding.
 *              See jdhuff.h for info about usage.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
INT16 i2Jpeg_huff_decode (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT32 *pi4_vld_buffer, INT32 *pi4_bits_left, d_derived_tbl * htbl, INT32 min_bits)
{
    register INT32 l = min_bits;
    register INT32 code;

    /* HUFF_DECODE has determined that the code is at least min_bits */
    /* bits long, so fetch that many bits in one swoop. */
    if (*pi4_bits_left < l)
    {
        vFill_bit_buffer (prJdecInst, pt_jpg_obj, pi4_vld_buffer, pi4_bits_left, l);
    }


    code = GET_BITS(*pi4_vld_buffer, *pi4_bits_left, l);

    /* Collect the rest of the Huffman code one bit at a time. */
    /* This is per Figure F.16 in the JPEG spec. */
    while (code > htbl->ai4_maxcode[l]) /* safe while*/
    {
        code <<= 1;
        if (*pi4_bits_left < l)
        {
            vFill_bit_buffer (prJdecInst, pt_jpg_obj, pi4_vld_buffer, pi4_bits_left, l);
        }

        code |= GET_BITS(*pi4_vld_buffer, *pi4_bits_left, 1);
        l++;
    }

    /* With garbage input we may reach the sentinel value l = 17. */
    if (l > 16)
    {
        if (pt_jpg_obj->e_JPGErrorLevel != JPG_FATAL_ERR)
        {
            pt_jpg_obj->ui4_JPGErrorType = 5;
            pt_jpg_obj->e_JPGErrorLevel = JPG_CORRUPT_ERR;
            //UTIL_Printf("[IMG] ERR: i2Jpeg_huff_decode(): With garbage input we may reach the sentinel value l = 17.\n\r");
            vJdecLog_L("[JPG_SW] ERR: i2Jpeg_huff_decode(): With garbage input we may reach the sentinel value l = 17.\n\r");
        }
        /* Error handler:
           fake a zero as the safest result*/
        return 0;
    }

    return htbl->pub->aui1_huffval[ (INT16) (code + htbl->ai4_valoffset[l]) ];
}
/*-----------------------------------------------------------------------------
 * Name:  vStart_pass_huff_decoder
 *
 * Description: Initialize for a Huffman-compressed scan.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vStart_pass_huff_decoder (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    INT16 ci, blkn, dctbl, actbl;
    COMPONENT_INFO_T *compptr;
    register HUFF_ENTROPY_DECODER_T* pt_entropy;
    register IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    pt_entropy = (HUFF_ENTROPY_DECODER_T*)(&pt_jpg_obj->entropy);
    /* Check that the scan parameters pt_info->Ss, pt_info->ui1_Se, pt_info->ui1_Ah/pt_info->Al are OK for sequential JPEG.
    * This ought to be an error condition, but we make it a warning because
    * there are some baseline files out there with all zeroes in these bytes.
    */
    if (pt_info->ui1_Ss != 0 || pt_info->ui1_Se != DCTSIZE2-1 || pt_info->ui1_Ah != 0 || pt_info->ui1_Al != 0)
    {

        //UTIL_Printf("[IMG] Invalid SOS parameters for sequential JPEG");
        vJdecLog_L("[JPG_SW] Invalid SOS parameters for sequential JPEG \n");

        if (pt_jpg_obj->e_JPGErrorLevel != JPG_FATAL_ERR)
        {
            pt_jpg_obj->ui4_JPGErrorType = 6;
            pt_jpg_obj->e_JPGErrorLevel = JPG_CORRUPT_ERR;
            //UTIL_Printf("[IMG] ERR: vStart_pass_huff_decoder(): Invalid SOS parameters for sequential JPEG\n\r");
            vJdecLog_L("[JPG_SW] ERR: vStart_pass_huff_decoder(): Invalid SOS parameters for sequential JPEG\n\r");
        }
        /* Error handler:
           set to expected value*/
        pt_info->ui1_Ss = 0;
        pt_info->ui1_Se = DCTSIZE2-1;
        pt_info->ui1_Ah = 0;
        pt_info->ui1_Al = 0;
    }

    if (pt_info->ui1_comps_in_scan > MAX_COMPS_IN_SCAN)
    {
        ASSERT(0);
    }
    for (ci = 0; ci < pt_info->ui1_comps_in_scan; ci++)
    {
        compptr = pt_info->apt_cur_comp_info[ci];
        dctbl = compptr->i2_dc_tbl_no;
        actbl = compptr->i2_ac_tbl_no;
        /* Compute derived values for Huffman tables */
        /* We may do this more than once for a table, but it's not expensive */
        vJpeg_make_d_derived_tbl (prJdecInst, pt_jpg_obj, TRUE, dctbl, & pt_entropy->dc_derived_tbls[dctbl]);
        vJpeg_make_d_derived_tbl (prJdecInst, pt_jpg_obj, FALSE, actbl, & pt_entropy->ac_derived_tbls[actbl]);

        /* Initialize DC predictions to 0 */
        pt_entropy->saved.ai2_last_dc_val[ci] = 0;
    }

    if (pt_info->ui2_blocks_in_MCU > D_MAX_BLOCKS_IN_MCU)
    {
        ASSERT(0);
    }
    /* Precalculate decoding info for each block in an MCU of this scan */
    for (blkn = 0; blkn < pt_info->ui2_blocks_in_MCU; blkn++)
    {
        ci = pt_info->aui2_MCU_membership[blkn];
        // For klocwork issue
        if (ci > (MAX_COMPS_IN_SCAN - 1))
        {
            ASSERT(0);
        }
        compptr = pt_info->apt_cur_comp_info[ci];
        /* Precalculate which table to use for each block */
        pt_entropy->dc_cur_tbls[blkn] = &(pt_entropy->dc_derived_tbls[compptr->i2_dc_tbl_no]);
        pt_entropy->ac_cur_tbls[blkn] = &(pt_entropy->ac_derived_tbls[compptr->i2_ac_tbl_no]);
        /* Decide whether we really care about the coefficient values */
        if (compptr->b_component_needed)
        {
            pt_entropy->dc_needed[blkn] = TRUE;
            pt_entropy->ac_needed[blkn] = TRUE;
        }
        else
        {
            pt_entropy->dc_needed[blkn] = pt_entropy->ac_needed[blkn] = FALSE;
        }
    }
    /* Initialize bitread state variables */
    /*Mars,[2002/12/23], fix bug: picture color incorrect when multi-scan jpeg stored in baseline jpg format*/
    pt_entropy->bits_left = 0;
    pt_entropy->vld_buffer = 0; /* unnecessary, but keeps Purify quiet */
    pt_entropy->insufficient_data = FALSE;
    /* Initialize restart counter */
    pt_entropy->restarts_to_go = pt_info->ui2_restart_interval;
}


/*
 * Compute the derived values for a Huffman table.
 * This routine also performs some validation checks on the table.
 *
 * Note this is also used by jdphuff.c.
 */

/*-----------------------------------------------------------------------------
 * Name:  vCalculateAC2
 *
 * Description: Figure F.12: extend sign bit.
 *              On some machines, a shift and add will be faster than a table lookup.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vCalculateAC2 (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT32 *vld_buffer, INT32 *bits_left, d_derived_tbl * actbl, INT16 *block_mem)
{

    INT32 vld_buffer_l;
    INT32 bits_left_l;
    register INT32 s, r;
    register INT32 k;
    register INT32 vld_buffer_reg;
    register INT32 bits_left_reg;
    register INT32 nb, look;
    register INT32 actbl_look_nbits, actbl_look_sym;
    register INT32 block;
    register INT32   iMaxValidData;
    IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }
    /* Keep frequently accessed variables stored in RISC registers */
    vld_buffer_l = *vld_buffer;
    bits_left_l = *bits_left;
    actbl_look_sym = (INT32) &(actbl->aui1_look_sym[0]);
    block = (UINT32) block_mem;
    vld_buffer_reg = vld_buffer_l;
    bits_left_reg = bits_left_l;
    actbl_look_nbits = (INT32) &(actbl->ai2_look_nbits[0]);

    if (pt_info->e_idct_size == TWO_BY_TWO)
        iMaxValidData = 4;
    else if (pt_info->e_idct_size == FOUR_BY_FOUR)
        iMaxValidData = 24;
    else
        iMaxValidData = DCTSIZE2;

    /* Section F.2.2.2: decode the AC coefficients */
    /* Since zeroes are skipped, output area must be cleared beforehand */

    /*
     The execution loop number = 758404 in Picture CD
    */
    for (k = 1; k < DCTSIZE2; k++)
    {
        if (bits_left_reg < HUFF_LOOKAHEAD)
        {
            /*
             The execution loop number = 154923 in Picture CD
            */
            vld_buffer_l = vld_buffer_reg;
            bits_left_l = bits_left_reg;
            vFill_bit_buffer (prJdecInst, pt_jpg_obj, &vld_buffer_l, &bits_left_l, 0);
            vld_buffer_reg = vld_buffer_l;
            bits_left_reg = bits_left_l;
            if (bits_left_reg < HUFF_LOOKAHEAD)
            {
                nb = 1;
                goto label2;
            }
        }

        look = PEEK_BITS(vld_buffer_reg, bits_left_reg, HUFF_LOOKAHEAD);

        if ((nb = *((INT16 *)actbl_look_nbits + look)) != 0)
        {
            /*
             The execution loop number = 742762 in Picture CD
            */
            DROP_BITS(vld_buffer_reg, bits_left_reg, nb);

            s = *((UINT8 *)actbl_look_sym + look);
        }
        else
        {
            /*
             The execution loop number = 15641 in Picture CD
            */
            nb = HUFF_LOOKAHEAD+1;
            vld_buffer_l = vld_buffer_reg;
            bits_left_l = bits_left_reg;
label2:
            /*
             The execution loop number = 15642 in Picture CD
            */
            s = i2Jpeg_huff_decode (prJdecInst, pt_jpg_obj, &vld_buffer_l, &bits_left_l, actbl, nb);
            vld_buffer_reg = vld_buffer_l;
            bits_left_reg = bits_left_l;
        }

        r = s >> 4;
        s &= 15;

        if (s)
        {
            /*
             The execution loop number = 720908 in Picture CD
            */
            k += r;
            if (bits_left_reg < s)
            {
                /*
                 The execution loop number = 6245 in Picture CD
                */
                vld_buffer_l = vld_buffer_reg;
                bits_left_l = bits_left_reg;
                vFill_bit_buffer (prJdecInst, pt_jpg_obj, &vld_buffer_l, &bits_left_l, s);
                vld_buffer_reg = vld_buffer_l;
                bits_left_reg = bits_left_l;
            }

            if (k <= iMaxValidData)
            {
                r = GET_BITS(vld_buffer_reg, bits_left_reg, s);
                s = HUFF_EXTEND(r, s);
                /* s = ((r) < (1<<((s)-1)) ? (r) + (((-1)<<(s)) + 1) : (r)); */
                /* Output coefficient in natural (dezigzagged) order.
                * Note: the extra entries in jdec_natural_order[] will save us
                * if k >= DCTSIZE2, which could happen if the data is corrupted.
                */
                *((INT16 *) block + jdec_natural_order[k]) = (INT16) s;
            }
            else
            {
                DROP_BITS(vld_buffer_reg, bits_left_reg, s);
            }
        }
        else
        {
            /*
             The execution loop number = 37496 in Picture CD
            */
            if (r != 15)
                break;
            k += 15;
        }
    }
    *vld_buffer = vld_buffer_reg;
    *bits_left = bits_left_reg;
}
/*-----------------------------------------------------------------------------
 * Name:  vBypassAC2
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vBypassAC2 (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT32 *pi4_vld_buffer, INT32 *pi4_bits_left, d_derived_tbl * actbl)
{
    register INT32 s, r;
    register INT32 k;
    register INT32 nb, look;
    register INT32 actbl_look_nbits, actbl_look_sym;

    /* Keep frequently accessed variables stored in RISC registers */
    actbl_look_sym = (INT32) &(actbl->aui1_look_sym[0]);
    actbl_look_nbits = (INT32) &(actbl->ai2_look_nbits[0]);

    /* Section F.2.2.2: decode the AC coefficients */
    /* Since zeroes are skipped, output area must be cleared beforehand */

    /*
     The execution loop number = 758404 in Picture CD
    */
    for (k = 1; k < DCTSIZE2; k++)
    {
        if (*pi4_bits_left < HUFF_LOOKAHEAD)
        {
            /*
             The execution loop number = 154923 in Picture CD
            */
            vFill_bit_buffer (prJdecInst, pt_jpg_obj, pi4_vld_buffer, pi4_bits_left, 0);
            if (*pi4_bits_left < HUFF_LOOKAHEAD)
            {
                nb = 1;
                goto label2;
            }
        }

        look = PEEK_BITS(*pi4_vld_buffer, *pi4_bits_left, HUFF_LOOKAHEAD);

        if ((nb = *((INT16 *) actbl_look_nbits + look)) != 0)
        {
            /*
               The execution loop number = 742762 in Picture CD
            */
            DROP_BITS(*pi4_vld_buffer, *pi4_bits_left, nb);
            s = *((UINT8 *) actbl_look_sym + look);
        }
        else
        {
            /*
             The execution loop number = 15641 in Picture CD
            */
            nb = HUFF_LOOKAHEAD+1;
label2:
            /*
             The execution loop number = 15642 in Picture CD
            */
            s = i2Jpeg_huff_decode (prJdecInst, pt_jpg_obj, pi4_vld_buffer, pi4_bits_left, actbl, nb);
        }


        r = s >> 4;
        s &= 15;

        if (s)
        {
            /*
             The execution loop number = 720908 in Picture CD
            */
            k += r;
            if (*pi4_bits_left < s)
            {
                /*
                 The execution loop number = 6245 in Picture CD
                */
                vFill_bit_buffer (prJdecInst, pt_jpg_obj, pi4_vld_buffer, pi4_bits_left, s);
            }

            DROP_BITS(*pi4_vld_buffer, *pi4_bits_left, s);
        }
        else
        {
            /*
             The execution loop number = 37496 in Picture CD
            */
            if (r != 15)
                break;
            k += 15;
        }
    }
}

 /*-----------------------------------------------------------------------------
 * Name:  vDecode_mcu
 *
 * Description: Decode and return one MCU's worth of Huffman-compressed coefficients.
 * The coefficients are reordered from zigzag order into natural array order,
 * but are not dequantized.
 *
 * The i'th block of the MCU is stored into the block pointed to by
 * MCU_data[i].  WE ASSUME THIS AREA HAS BEEN ZEROED BY THE CALLER.
 * (Wholesale zeroing is usually a little faster than retail...)
 *
 * Returns FALSE if data source requested suspension.  In that case no
 * changes have been made to permanent state.  (Exception: some output
 * coefficients may already have been assigned.  This is harmless for
 * this module, since we'll just re-assign them on the next call.)
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vDecode_mcu (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT16 *MCU_data[])
{
    register INT16 blkn;
    INT32 vld_buffer;
    INT32 bits_left;
    INT32 nb, look;
    register INT32 s, r, ui4_block_num;
    register HUFF_ENTROPY_DECODER_T* pt_entropy;
    register IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    ui4_block_num = pt_info->ui2_blocks_in_MCU;

    pt_entropy = (HUFF_ENTROPY_DECODER_T*)(&pt_jpg_obj->entropy);

    /* store to pt_jpg_obj->entropy structure to register version of vld_buffer and bits_left */
    vld_buffer = pt_entropy->vld_buffer;
    bits_left = pt_entropy->bits_left;

    /* Process restart marker if needed; may have to suspend */
    if (pt_info->ui2_restart_interval)
    {
        if (pt_entropy->restarts_to_go == 0)
        {
            vProcess_restart (prJdecInst, pt_jpg_obj, &bits_left);
            if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
            {
                //UTIL_Printf("[IMG] ERR: vDecode_mcu(): vProcess_restart error\n\r");
                vJdecLog_L("[JPG_SW] ERR: vDecode_mcu(): vProcess_restart error\n\r");
                return;
            }
        }
    }

    /* If we've run out of data, just leave the MCU set to zeroes.
    * This way, we return uniform gray for the remainder of the segment.
    */
    if (! pt_entropy->insufficient_data)
    {
        /* Outer loop handles each block in the MCU */
        if (ui4_block_num > D_MAX_BLOCKS_IN_MCU)
        {
            ASSERT(0);
        }
        for (blkn = 0; blkn < ui4_block_num; blkn++)
        {
            d_derived_tbl * dctbl = pt_entropy->dc_cur_tbls[blkn];
            d_derived_tbl * actbl = pt_entropy->ac_cur_tbls[blkn];
            INT16 *block = MCU_data[blkn];
            /* Decode a single block's worth of coefficients */

            /* Section F.2.2.1: decode the DC coefficient difference */
            if (bits_left < HUFF_LOOKAHEAD)
            {
                vFill_bit_buffer (prJdecInst, pt_jpg_obj, &vld_buffer, &bits_left, 0);
                if (bits_left < HUFF_LOOKAHEAD)
                {
                    nb = 1;
                    goto slowlabel;
                }
            }
            look = PEEK_BITS(vld_buffer, bits_left, HUFF_LOOKAHEAD);

            if ((nb = dctbl->ai2_look_nbits[look]) != 0)
            {
                DROP_BITS(vld_buffer, bits_left, nb);
                s = dctbl->aui1_look_sym[look];
            }
            else
            {
                nb = HUFF_LOOKAHEAD + 1;
slowlabel:
                s = i2Jpeg_huff_decode (prJdecInst, pt_jpg_obj, &vld_buffer, &bits_left, dctbl, nb);
            }

            if (s)
            {
                CHECK_BIT_BUFFER(prJdecInst, pt_jpg_obj, vld_buffer, bits_left, s)
                r = GET_BITS(vld_buffer, bits_left, s);
                s = HUFF_EXTEND(r, s);
            }

            /*
             The execution loop number = 36864 in Picture CD
            */

            if (pt_entropy->dc_needed[blkn])
            {
                /* Convert DC difference to actual value, update last_dc_val */
                INT16 ci = pt_info->aui2_MCU_membership[blkn];
                if (ci > (MAX_COMPS_IN_SCAN - 1))
                {
                    ASSERT(0);
                }
                s += pt_entropy->saved.ai2_last_dc_val[ci];
                pt_entropy->saved.ai2_last_dc_val[ci] = (INT16)s;
                /* Output the DC coefficient (assumes jdec_natural_order[0] = 0) */
                *(block) = (INT16) s;
            }

            if (pt_entropy->ac_needed[blkn])
            {
                if (pt_info->e_idct_size == ONE_BY_ONE)
                    vBypassAC2 (prJdecInst, pt_jpg_obj, &vld_buffer, &bits_left, actbl);
                else
                    vCalculateAC2 (prJdecInst, pt_jpg_obj, &vld_buffer, &bits_left, actbl, block);
            }
            else
            {
                /* Section F.2.2.2: decode the AC coefficients */
                /* In this path we just discard the values */
                /*
                 The execution loop number = 0 in Picture CD
                */
                vBypassAC2 (prJdecInst, pt_jpg_obj, &vld_buffer, &bits_left, actbl);
            }
        }

    }

    /* Account for restart interval (no-op if not using restarts) */
    pt_entropy->restarts_to_go--;

    /* restore register version of vld_buffer and bits_left to pt_jpg_obj->entropy structure */
    pt_entropy->vld_buffer = vld_buffer;
    pt_entropy->bits_left = bits_left;
}


