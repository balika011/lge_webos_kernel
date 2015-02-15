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
 * $RCSfile: jdphuff.c,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/DTV_X_ATSC/2 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: e1f1b6f02364fc09c4fa1a21200359ae $ Asa
 *
 * Description:
 *         This file contains the progressive de-huffman routines,
 *         Expanded pt_jpg_obj->entropy decoder object for progressive Huffman decoding.
 *
 *         The savable_state subrecord contains fields that change within an MCU,
 *         but must not be updated permanently until we complete the MCU.
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
/*
 * Figure F.12: extend sign bit.
 * On some machines, a shift and add will be faster than a table lookup.
 */
#define HUFF_EXTEND(x,s)  ((x) < (1<<((s)-1)) ? (x) + (((-1)<<(s)) + 1) : (x))
/*-----------------------------------------------------------------------------
                    extern data declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
/*
 * jdec_natural_order[i] is the natural-order position of the i'th element
 * of zigzag order.
 *
 * When reading corrupted data, the Huffman decoders could attempt
 * to reference an entry beyond the end of this array (if the decoded
 * zero run length reaches past the end of the block).  To prevent
 * wild stores without adding an inner-loop test, we put some extra
 * "63"s after the real entries.  This will cause the extra coefficient
 * to be stored in location 63 of the block, not somewhere random.
 * The worst case would be a run-length of 15, which means we need 16
 * fake entries.
 */

const INT16 jdec_natural_order[DCTSIZE2+16] = {
  0,  1,  8, 16,  9,  2,  3, 10,
 17, 24, 32, 25, 18, 11,  4,  5,
 12, 19, 26, 33, 40, 48, 41, 34,
 27, 20, 13,  6,  7, 14, 21, 28,
 35, 42, 49, 56, 57, 50, 43, 36,
 29, 22, 15, 23, 30, 37, 44, 51,
 58, 59, 52, 45, 38, 31, 39, 46,
 53, 60, 61, 54, 47, 55, 62, 63,
 63, 63, 63, 63, 63, 63, 63, 63, /* extra entries for safety in decoder */
 63, 63, 63, 63, 63, 63, 63, 63
};
/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Name:  vStart_pass_phuff_decoder
 *
 * Description: Initialize for a Huffman-compressed scan.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vStart_pass_phuff_decoder (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    BOOL is_DC_band;
    UINT16 ci,  tbl;
    COMPONENT_INFO_T * compptr;
    register HUFF_ENTROPY_DECODER_T* pt_entropy;
    register IMG_BASIC_INFO_T* pt_info = NULL;
    volatile register UINT32 ui4_Ss, ui4_Se, ui4_Ah, ui4_Al;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    pt_entropy = (HUFF_ENTROPY_DECODER_T*)(&pt_jpg_obj->entropy);

    ui4_Ss = pt_info->ui1_Ss;
    ui4_Se = pt_info->ui1_Se;
    ui4_Ah = pt_info->ui1_Ah;
    ui4_Al = pt_info->ui1_Al;

    is_DC_band = (ui4_Ss == 0);

    /* Arguably the maximum pt_info->Al value should be less than 13 for 8-bit precision,
    * but the spec doesn't say so, and we try to be liberal about what we
    * accept.  Note: large pt_info->Al values could result in out-of-range DC
    * coefficients during early scans, leading to bizarre displays due to
    * overflows in the IDCT math.  But we won't crash.
    */

    if (pt_info->ui1_comps_in_scan > MAX_COMPS_IN_SCAN)
    {
        ASSERT(0);
    }
    for (ci = 0; ci < pt_info->ui1_comps_in_scan; ci++)
    {
        compptr = pt_info->apt_cur_comp_info[ci];
        /* Make sure requested tables are present, and compute derived tables.
        * We may build same derived table more than once, but it's not expensive.
        */
        if (is_DC_band)
        {
            if (ui4_Ah == 0)  /* DC refinement needs no table */
            {
                tbl = compptr->i2_dc_tbl_no;
                // For klocwork issues
                if (tbl > (NUM_HUFF_TBLS - 1))
                {
                    ASSERT(0);
                }
                vJpeg_make_d_derived_tbl (prJdecInst, pt_jpg_obj, is_DC_band, tbl, & pt_entropy->derived_tbls[tbl]);
            }
        }
        else
        {
            tbl = compptr->i2_ac_tbl_no;
            // For klocwork issues
            if (tbl > (NUM_HUFF_TBLS - 1))
            {
                ASSERT(0);
            }
            vJpeg_make_d_derived_tbl (prJdecInst, pt_jpg_obj, is_DC_band, tbl, & pt_entropy->derived_tbls[tbl]);
            /* remember the single active table */
            pt_entropy->ac_derived_tbl = &(pt_entropy->derived_tbls[tbl]);
        }
    /* Initialize DC predictions to 0 */
    pt_entropy->saved.ai2_last_dc_val[ci] = 0;
    }

    /* Initialize bitread state variables */
    pt_entropy->bits_left = 0;
    pt_entropy->vld_buffer = 0; /* unnecessary, but keeps Purify quiet */
    pt_entropy->insufficient_data = FALSE;

    /* Initialize private state variables */
    pt_entropy->saved.ui2_EOBRUN = 0;

    /* Initialize restart counter */
    pt_entropy->restarts_to_go = pt_info->ui2_restart_interval;
}
/*-----------------------------------------------------------------------------
 * Name:  vProcess_restart
 *
 * Description: Check for a restart marker & resynchronize decoder.
 *              Returns FALSE if must suspend.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vProcess_restart (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT32 *bits_left)
{
    INT16 ci;
    register HUFF_ENTROPY_DECODER_T* pt_entropy;
    register IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }
    pt_entropy = (HUFF_ENTROPY_DECODER_T*)(&pt_jpg_obj->entropy);

    *bits_left = 0;
    /* Advance past the RSTn marker */
    vRead_restart_marker (prJdecInst, pt_jpg_obj);

    if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
    {
        //UTIL_Printf("[IMG] ERR: vProcess_restart(): vRead_restart_marker error\n\r");
        vJdecLog_L("[JPG_SW] ERR: vProcess_restart(): vRead_restart_marker error\n\r");
        return;
    }

    if (pt_info->ui1_comps_in_scan > MAX_COMPS_IN_SCAN)
    {
        ASSERT(0);
    }
    /* Re-initialize DC predictions to 0 */
    for (ci = 0; ci < pt_info->ui1_comps_in_scan; ci++)
        pt_entropy->saved.ai2_last_dc_val[ci] = 0;
    /* Re-init EOB run count, too */
    pt_entropy->saved.ui2_EOBRUN = 0;

    /* Reset restart counter */
    pt_entropy->restarts_to_go = pt_info->ui2_restart_interval;

    /* Reset out-of-data flag, unless vRead_restart_marker left us smack up
    * against a marker.  In that case we will end up treating the next data
    * segment as empty, and we can avoid producing bogus output pixels by
    * leaving the flag set.
    */
    if (pt_jpg_obj->i2_unread_marker == 0)
        pt_entropy->insufficient_data = FALSE;
}

/*
 * Huffman MCU decoding.
 * Each of these routines decodes and returns one MCU's worth of
 * Huffman-compressed coefficients.
 * The coefficients are reordered from zigzag order into natural array order,
 * but are not dequantized.
 *
 * The i'th block of the MCU is stored into the block pointed to by
 * MCU_data[i].  WE ASSUME THIS AREA IS INITIALLY ZEROED BY THE CALLER.
 *
 * We return FALSE if data source requested suspension.  In that case no
 * changes have been made to permanent state.  (Exception: some output
 * coefficients may already have been assigned.  This is harmless for
 * spectral selection, since we'll just re-assign them on the next call.
 * Successive approximation AC refinement has to be more careful, however.)
 */
/*-----------------------------------------------------------------------------
 * Name:  vDecode_mcu_DC_first
 *
 * Description: MCU decoding for DC initial scan (either spectral selection,
 *              or first pass of successive approximation).
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vDecode_mcu_DC_first (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT16 *MCU_data[], BOOL write_to_buf)
{
    register INT32 s, r;
    INT16 blkn, ci;
    INT16 *block;
    d_derived_tbl * tbl;
    COMPONENT_INFO_T * compptr;
    INT32 vld_buffer;
    INT32 bits_left;
    register INT32 blocks_in_MCU_reg;
    register HUFF_ENTROPY_DECODER_T* pt_entropy;
    register IMG_BASIC_INFO_T* pt_info = NULL;
    UINT32 ui4_Al;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    pt_entropy = (HUFF_ENTROPY_DECODER_T*)(&pt_jpg_obj->entropy);

    /* store to pt_jpg_obj->entropy structure to register version of vld_buffer and bits_left */
    vld_buffer = pt_entropy->vld_buffer;
    bits_left = pt_entropy->bits_left;
    blocks_in_MCU_reg = pt_info->ui2_blocks_in_MCU;

    /* Process restart marker if needed; may have to suspend */
    if (pt_info->ui2_restart_interval)
    {
        if (pt_entropy->restarts_to_go == 0)
        {
            vProcess_restart (prJdecInst, pt_jpg_obj, &bits_left);
            if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
            {
                //UTIL_Printf("[IMG] ERR: vDecode_mcu_DC_first(): Process restart marker if needed; may have to suspend.\n\r");
                vJdecLog_L("[JPG_SW] ERR: vDecode_mcu_DC_first(): Process restart marker if needed; may have to suspend.\n\r");
                return;
            }
        }
    }

    /* If we've run out of data, just leave the MCU set to zeroes.
    * This way, we return uniform gray for the remainder of the segment.
    */
    if (! pt_entropy->insufficient_data)
    {
        ui4_Al = pt_info->ui1_Al;
        if (blocks_in_MCU_reg > (D_MAX_BLOCKS_IN_MCU))
        {
            ASSERT(0);
        }
        /* Outer loop handles each block in the MCU */
        for (blkn = 0; blkn < blocks_in_MCU_reg; blkn++)
        {
            block = MCU_data[blkn];
            ci = pt_info->aui2_MCU_membership[blkn];
            if (ci > (MAX_COMPS_IN_SCAN - 1))
            {
                ASSERT(0);
            }
            compptr = pt_info->apt_cur_comp_info[ci];
            tbl = &(pt_entropy->derived_tbls[compptr->i2_dc_tbl_no]);

            /* Decode a single block's worth of coefficients */

            /* Section F.2.2.1: decode the DC coefficient difference */
            HUFF_DECODE(prJdecInst, pt_jpg_obj, s, vld_buffer, bits_left, tbl, label1);

            if (s)
            {
                CHECK_BIT_BUFFER(prJdecInst, pt_jpg_obj, vld_buffer, bits_left, s);
                r = GET_BITS(vld_buffer, bits_left, s);
                s = HUFF_EXTEND(r, s);
            }

            /* Convert DC difference to actual value, update last_dc_val */
            s += pt_entropy->saved.ai2_last_dc_val[ci];
            pt_entropy->saved.ai2_last_dc_val[ci] = (INT16)s;
            /* Scale and output the coefficient (assumes jdec_natural_order[0]=0) */
            if (write_to_buf == TRUE)
            {
                *block = (INT16) (s << ui4_Al);
            }
        }
    }

    /* Account for restart interval (no-op if not using restarts) */
    pt_entropy->restarts_to_go--;

    /* restore register version of vld_buffer and bits_left to pt_jpg_obj->entropy structure */
    pt_entropy->vld_buffer = vld_buffer;
    pt_entropy->bits_left = bits_left;
}

/*-----------------------------------------------------------------------------
 * Name:  vDecode_mcu_AC_first
 *
 * Description: MCU decoding for AC initial scan (either spectral selection,
 *              or first pass of successive approximation).
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vDecode_mcu_AC_first (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT16 *MCU_data[], BOOL write_to_buf)
{
    INT32 s, r;
    register INT32 k;
    UINT32 EOBRUN;
    INT16 *block;
    d_derived_tbl * tbl;
    INT16 *test_byte_ptr;
    INT32 vld_buffer;
    INT32 bits_left;
    register INT32 Se_reg;
    register HUFF_ENTROPY_DECODER_T* pt_entropy;
    register IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    pt_entropy = (HUFF_ENTROPY_DECODER_T*)(&pt_jpg_obj->entropy);

    /* store to pt_jpg_obj->entropy structure to register version of vld_buffer and bits_left */
    vld_buffer = pt_entropy->vld_buffer;
    bits_left = pt_entropy->bits_left;
    Se_reg = pt_info->ui1_Se;

    /* Process restart marker if needed; may have to suspend */
    if (pt_info->ui2_restart_interval)
    {
        if (pt_entropy->restarts_to_go == 0)
        {
            vProcess_restart (prJdecInst, pt_jpg_obj, &bits_left);
            if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
            {
                //UTIL_Printf("[IMG] ERR: vDecode_mcu_AC_first(): Process restart marker if needed; may have to suspend.\n\r");
                vJdecLog_L("[JPG_SW] ERR: vDecode_mcu_AC_first(): Process restart marker if needed; may have to suspend.\n\r");
                return;
            }
        }
    }

    /* If we've run out of data, just leave the MCU set to zeroes.
    * This way, we return uniform gray for the remainder of the segment.
    */
    if (! pt_entropy->insufficient_data)
    {
        /* Load up working state.
        * We can avoid loading/saving bitread state if in an EOB run.
        */
        EOBRUN = pt_entropy->saved.ui2_EOBRUN; /* only part of saved state we need */
        /* There is always only one block per MCU */

        if (EOBRUN > 0)   /* if it's a band of zeroes... */
            EOBRUN--;     /* ...process it now (we do nothing) */
        else
        {
            block = MCU_data[0];
            tbl = pt_entropy->ac_derived_tbl;

            if (write_to_buf == TRUE)
            {
                for (k = pt_info->ui1_Ss; k <= Se_reg; k++)
                {
                    HUFF_DECODE(prJdecInst, pt_jpg_obj, s, vld_buffer, bits_left, tbl, label2);

                    r = s >> 4;
                    s &= 15;
                    if (s)
                    {
                        k += r;
                        CHECK_BIT_BUFFER(prJdecInst, pt_jpg_obj, vld_buffer, bits_left, s);
                        r = GET_BITS(vld_buffer, bits_left, s);
                        s = HUFF_EXTEND(r, s);
                        /* Scale and output coefficient in natural (dezigzagged) order */
                        if (k > (DCTSIZE2+15))
                        {
                            ASSERT(0);
                        }
                        *(block+jdec_natural_order[k]) = (INT16) (s << pt_info->ui1_Al);
                    }
                    else
                    {
                        if (r == 15) /* ZRL */
                        {
                            k += 15;   /* skip 15 zeroes in band */
                        }
                        else /* EOBr, run length is 2^r + appended bits */
                        {
                            EOBRUN = 1 << r;
                            if (r)  /* EOBr, r > 0 */
                            {
                                CHECK_BIT_BUFFER(prJdecInst, pt_jpg_obj, vld_buffer, bits_left, r);
                                r = GET_BITS(vld_buffer, bits_left, r);
                                EOBRUN += r;
                            }
                            EOBRUN--;   /* this band is processed at this moment */
                            break;    /* force end-of-band */
                        }
                    }
                }
            }
            else
            {
                for (k = pt_info->ui1_Ss; k <= Se_reg; k++)
                {
                    HUFF_DECODE(prJdecInst, pt_jpg_obj, s, vld_buffer, bits_left, tbl, label3);

                    r = s >> 4;
                    s &= 15;
                    if (s)
                    {
                        k += r;
                        CHECK_BIT_BUFFER(prJdecInst, pt_jpg_obj, vld_buffer, bits_left, s);

                        r = GET_BITS(vld_buffer, bits_left, s);
                        s = HUFF_EXTEND(r, s);
                        /* Scale and output coefficient in natural (dezigzagged) order */
                        test_byte_ptr = block +  (k >> 4);  /* calculate the test coefficient falling within which word by dividing raster_index by 16 */
                        *test_byte_ptr = *test_byte_ptr | ((UINT16) 1 << (k & 0xF));
                    }
                    else
                    {
                        if (r == 15) /* ZRL */
                        {
                            k += 15;   /* skip 15 zeroes in band */
                        }
                        else /* EOBr, run length is 2^r + appended bits */
                        {
                            EOBRUN = 1 << r;
                            if (r)  /* EOBr, r > 0 */
                            {
                                CHECK_BIT_BUFFER(prJdecInst, pt_jpg_obj, vld_buffer, bits_left, r);
                                r = GET_BITS(vld_buffer, bits_left, r);
                                EOBRUN += r;
                            }
                            EOBRUN--;   /* this band is processed at this moment */
                            break;    /* force end-of-band */
                        }
                    }
                }
            }
        }
        /* Completed MCU, so update state */
        pt_entropy->saved.ui2_EOBRUN = (UINT16)EOBRUN; /* only part of saved state we need */
    }
    /* Account for restart interval (no-op if not using restarts) */
    pt_entropy->restarts_to_go--;
    /* restore register version of vld_buffer and bits_left to pt_jpg_obj->entropy structure */
    pt_entropy->vld_buffer = vld_buffer;
    pt_entropy->bits_left = bits_left;
}

/*-----------------------------------------------------------------------------
 * Name:  vDecode_mcu_DC_refine
 *
 * Description: MCU decoding for DC successive approximation refinement scan.
 *              Note: we assume such scans can be multi-component, although the spec
 *              is not very clear on the point.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vDecode_mcu_DC_refine (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT16 *MCU_data[], BOOL write_to_buf)
{
    INT32 p1; /* 1 in the bit position being coded */
    register INT32 blkn;
    INT16 *block;
    INT32 vld_buffer;
    INT32 bits_left;
    register INT32 blocks_in_MCU_reg;
    register HUFF_ENTROPY_DECODER_T* pt_entropy;
    register IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }

    pt_entropy = (HUFF_ENTROPY_DECODER_T*)(&pt_jpg_obj->entropy);

    p1 = 1 << pt_info->ui1_Al; /* 1 in the bit position being coded */
    /* store to pt_jpg_obj->entropy structure to register version of vld_buffer and bits_left */
    vld_buffer = pt_entropy->vld_buffer;
    bits_left = pt_entropy->bits_left;
    blocks_in_MCU_reg = pt_info->ui2_blocks_in_MCU;

    /* Process restart marker if needed; may have to suspend */
    if (pt_info->ui2_restart_interval)
    {
        if (pt_entropy->restarts_to_go == 0)
        {
            vProcess_restart (prJdecInst, pt_jpg_obj, &bits_left);
            if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
            {
                //UTIL_Printf("[IMG] ERR: vDecode_mcu_DC_refine(): Process restart marker if needed; may have to suspend.\n\r");
                vJdecLog_L("[JPG_SW] ERR: vDecode_mcu_DC_refine(): Process restart marker if needed; may have to suspend.\n\r");
                return;
            }
        }
    }

    /* Not worth the cycles to check insufficient_data here,
    * since we will not change the data anyway if we read zeroes.
    */

    /* Outer loop handles each block in the MCU */
    if (write_to_buf == TRUE)
    {
        for (blkn = 0; blkn < blocks_in_MCU_reg; blkn++)
        {
            block = MCU_data[blkn];

            /* Encoded data is simply the next bit of the two's-complement DC value */
            CHECK_BIT_BUFFER(prJdecInst, pt_jpg_obj, vld_buffer, bits_left, 1);
            if (GET_BITS(vld_buffer, bits_left, 1))
            {
                *block = (INT16) p1 | *block;
            }
            /* Note: since we use |=, repeating the assignment later is safe */
        }
    }
    else
    {
        for (blkn = 0; blkn < blocks_in_MCU_reg; blkn++)
        {
            block = MCU_data[blkn];

            /* Encoded data is simply the next bit of the two's-complement DC value */
            CHECK_BIT_BUFFER(prJdecInst, pt_jpg_obj, vld_buffer, bits_left, 1);
            DROP_BITS(vld_buffer, bits_left, 1);
        }
    }

    /* Account for restart interval (no-op if not using restarts) */
    pt_entropy->restarts_to_go--;
    /* restore register version of vld_buffer and bits_left to pt_jpg_obj->entropy structure */
    pt_entropy->vld_buffer = vld_buffer;
    pt_entropy->bits_left = bits_left;
}

/*-----------------------------------------------------------------------------
 * Name:  vDecode_mcu_AC_refine
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vDecode_mcu_AC_refine (JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT16 *MCU_data[], BOOL write_to_buf)
{
    INT32 p1;
    INT32 m1;
    register INT32 k;
    INT32 s, r;
    UINT32 EOBRUN;
    INT16 *block;
    INT16 *thiscoef;
    INT16 thiscoef_val;
    d_derived_tbl * tbl;
    INT16 test_byte;
    INT16 *test_byte_ptr;
    INT32 vld_buffer;
    INT32 bits_left;
    register INT32 Se_reg;
    register HUFF_ENTROPY_DECODER_T* pt_entropy;
    register IMG_BASIC_INFO_T* pt_info = NULL;

    pt_info = pt_jpg_obj->pt_current_sel;
    if (pt_info == NULL)
    {
        return;
    }
    pt_entropy = (HUFF_ENTROPY_DECODER_T*)(&pt_jpg_obj->entropy);

    /* store to pt_jpg_obj->entropy structure to register version of vld_buffer and bits_left */
    vld_buffer = pt_entropy->vld_buffer;
    bits_left = pt_entropy->bits_left;
    Se_reg = pt_info->ui1_Se;

    /* Process restart marker if needed; may have to suspend */
    if (pt_info->ui2_restart_interval)
    {
        if (pt_entropy->restarts_to_go == 0)
        {
            vProcess_restart (prJdecInst, pt_jpg_obj, &bits_left);
            if (pt_jpg_obj->e_JPGErrorLevel == JPG_FATAL_ERR)
            {
                //UTIL_Printf("[IMG] ERR: vDecode_mcu_AC_refine(): Process restart marker if needed; may have to suspend.\n\r");
                vJdecLog_L("[JPG_SW] ERR: vDecode_mcu_AC_refine(): Process restart marker if needed; may have to suspend.\n\r");
                return;
            }
        }
    }


    /* If we've run out of data, don't modify the MCU.
    */
    if (! pt_entropy->insufficient_data)
    {
        /* Load up working state */
        EOBRUN = pt_entropy->saved.ui2_EOBRUN; /* only part of saved state we need */

        /* There is always only one block per MCU */
        block = MCU_data[0];
        tbl = pt_entropy->ac_derived_tbl;
        /* initialize coefficient loop counter to start of band */
        k = pt_info->ui1_Ss;

        if (EOBRUN == 0)
        {
            p1 = 1 << pt_info->ui1_Al; /* 1 in the bit position being coded */
            m1 = (-1) << pt_info->ui1_Al;  /* -1 in the bit position being coded */
            for (; k <= Se_reg; k++)
            {
                HUFF_DECODE(prJdecInst, pt_jpg_obj, s, vld_buffer, bits_left, tbl, label4);

                r = s >> 4;
                s &= 15;
                if (s) /* not EOBr code, a nonzero coefficient */
                {
                    if (s != 1)   /* size of new coef should always be 1 */
                    {
                        //UTIL_Printf("[IMG] ERR: Corrupt JPEG data: bad Huffman code\n");
                        vJdecLog_M("[JPG_SW] ERR: Corrupt JPEG data: bad Huffman code\n");
                    }


                    /* Non-zero coefficients with zero history are coded with a composite code of the form  */
                    /* HUFFCO(RRRRSSSS) + additional bit (0 bit: neg; 1 bit: pos) + correction bit (0-bit: no correction; 1-bit: one should added to the decded magnitude of the coefficient */
                    CHECK_BIT_BUFFER(prJdecInst, pt_jpg_obj, vld_buffer, bits_left, 1)
                    if (GET_BITS(vld_buffer, bits_left, 1))
                        s = p1;   /* newly nonzero coef is positive */
                    else
                        s = m1;   /* newly nonzero coef is negative */
                }
                else
                {
                    if (r != 15)
                    {
                        EOBRUN = 1 << r;  /* EOBr, run length is 2^r + appended bits */
                        if (r)
                        {
                            CHECK_BIT_BUFFER(prJdecInst, pt_jpg_obj, vld_buffer, bits_left, r)
                            r = GET_BITS(vld_buffer, bits_left, r);
                            EOBRUN += r;
                        }
                        break;    /* rest of block is handled by EOB logic */
                    }
                /* note s = 0 for processing ZRL */
                }
                /* Advance over already-nonzero coefs and r still-zero coefs,
                * appending correction bits to the nonzeroes.  A correction bit is 1
                * if the absolute value of the coefficient must be increased.
                */
                if (write_to_buf == TRUE)
                {
                    do {
                            if (k > (DCTSIZE2+15))
                            {
                                ASSERT(0);
                            }
                            thiscoef = (block + jdec_natural_order[k]);

                            if (*thiscoef != 0)  /* This coefficient has non-zero history */
                            {
                                CHECK_BIT_BUFFER(prJdecInst, pt_jpg_obj, vld_buffer, bits_left, 1)

                                /* Check if the non-zero history coefficient needs correction                   */
                                /* 0-bit means no correction                                               */
                                /* 1-bit means one shall be added to the decoded magnitude of the coefficient  */
                                if (GET_BITS(vld_buffer, bits_left, 1))
                                {
                                    if (*thiscoef >= 0)
                                        *thiscoef += (INT16)p1;
                                    else
                                        *thiscoef += (INT16)m1;
                                }
                            }
                            /* r records the number of zero coefficients that are between the current coefficient and the previously */
                            /* coded coefficient (or the start of band)                                                        */
                            /* coefficients with non-zero history are skipped over when counting the zero coefficients              */
                            else
                            {
                                if (--r < 0)
                                    break;   /* reached target zero coefficient */
                            }

                            k++;
                        } while (k <= Se_reg);    /* safe do while*/


                    if (s)
                    {
                        /* Output newly nonzero coefficient */
                        if (k > (DCTSIZE2+15))
                        {
                            ASSERT(0);
                        }
                        *(block + jdec_natural_order[k]) = (INT16) s;
                    }
                }
                else
                {
                    test_byte = *(block +  (k >> 4));  /* calculate the test coefficient falling within which word by dividing raster_index by 16 */
                    do {
                            /*                                                                        */
                            /* 直接將nonzero flag 以zig-zag scan的方式存入 DRAM, 以節省轉成raster-scan的計算 */
                            /*                                                                        */
                            if ((k & 0xF) == 0) /* div by 16, remainder == 0 */
                            {
                                test_byte = *(block +  (k >> 4));  /* calculate the test coefficient falling within which word by dividing raster_index by 16 */
                            }
                            thiscoef_val = (INT16) (test_byte & ((UINT32) 1 << (k & 0xF)));

                            if (thiscoef_val != 0)
                            {
                                CHECK_BIT_BUFFER(prJdecInst, pt_jpg_obj, vld_buffer, bits_left, 1)
                                DROP_BITS(vld_buffer, bits_left, 1);
                            }
                            else
                            {
                                if (--r < 0)
                                    break;   /* reached target zero coefficient */
                            }
                            k++;
                    } while (k <= Se_reg);  /* safe do while */

                    if (s)
                    {
                        /* Output newly nonzero coefficient */
                        test_byte_ptr = block +  (k >> 4);  /* calculate the test coefficient falling within which word by dividing raster_index by 16 */
                        *test_byte_ptr = (INT16)(*test_byte_ptr | ((UINT32) 1 << (k & 0xF)));
                    }
                }
            }
        }

        if (EOBRUN > 0)
        {
            /* Scan any remaining coefficient positions after the end-of-band
            * (the last newly nonzero coefficient, if any).  Append a correction
            * bit to each already-nonzero coefficient.  A correction bit is 1
            * if the absolute value of the coefficient must be increased.
            */
            if (write_to_buf == TRUE)
            {
                p1 = 1 << pt_info->ui1_Al;  /* 1 in the bit position being coded */
                m1 = (-1) << pt_info->ui1_Al; /* -1 in the bit position being coded */

                for (; k <= Se_reg; k++)
                {
                    if (k > (DCTSIZE2+15))
                    {
                        ASSERT(0);
                    }
                    thiscoef = (block + jdec_natural_order[k]);
                    if (*thiscoef != 0)
                    {
                        CHECK_BIT_BUFFER(prJdecInst, pt_jpg_obj, vld_buffer, bits_left, 1)
                        /* Check if the non-zero history coefficient needs correction                   */
                        /* 0-bit means no correction                                               */
                        /* 1-bit means one shall be added to the decoded magnitude of the coefficient  */
                        if (GET_BITS(vld_buffer, bits_left, 1))
                        {
                            if (*thiscoef >= 0)
                                *thiscoef += (INT16)p1;
                            else
                                *thiscoef += (INT16)m1;
                        }
                    }
                }
            }
            else
            {
                test_byte = *(block +  (k >> 4));  /* calculate the test coefficient falling within which word by dividing raster_index by 16 */

                for (; k <= Se_reg; k++)
                {
                    if ((k & 0xF) == 0) /* div by 16, remainder == 0 */
                    {
                        test_byte = *(block +  (k >> 4));  /* calculate the test coefficient falling within which word by dividing raster_index by 16 */
                    }
                    thiscoef_val = (INT16)(test_byte & ((UINT32) 1 << (k & 0xF)));
                    if (thiscoef_val != 0)
                    {
                        CHECK_BIT_BUFFER(prJdecInst, pt_jpg_obj, vld_buffer, bits_left, 1)
                        DROP_BITS(vld_buffer, bits_left, 1);
                    }
                }
            }
            /* Count one block completed in EOB run */
            EOBRUN--;
        }
        /* Completed MCU, so update state */
        pt_entropy->saved.ui2_EOBRUN = (UINT16)EOBRUN; /* only part of saved state we need */
    }
    /* Account for restart interval (no-op if not using restarts) */
    pt_entropy->restarts_to_go--;
    /* restore register version of vld_buffer and bits_left to pt_jpg_obj->entropy structure */
    pt_entropy->vld_buffer = vld_buffer;
    pt_entropy->bits_left = bits_left;
}

