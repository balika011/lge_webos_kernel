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
 *-----------------------------------------------------------------------------
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: vdec_vp8dec.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_vp8dec.c
 *  This source file include vp8 decode function 
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_assert.h"
#include "x_printf.h"
#include "vdec_drvif.h"
LINT_EXT_HEADER_BEGIN
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_common.h"
#include "x_typedef.h"
LINT_EXT_HEADER_END
#include "vdec_vp9_dec.h"
#if defined(CC_TRUSTZONE_SUPPORT) && (defined(CC_TVP_SUPPORT)||defined(CC_SVP_SUPPORT))
#include "tz_if.h"
#endif
//extern VP9_LAE_INFO_T _rLaeInfo[];
extern VP9_LAE_INFO_T *GetLaeInfo(void);
extern VP9_COMMON_T* prVP9GetCommon(UINT32 u4InstID);
INLINE INT32 get_msb(UINT32 n) {
  return 31 ^ __builtin_clz(n);
}
void tx_counts_to_branch_counts_32x32(const unsigned int *tx_count_32x32p,
                                      unsigned int (*ct_32x32p)[2]) {
    ct_32x32p[0][0] = tx_count_32x32p[TX_4X4];
    ct_32x32p[0][1] = tx_count_32x32p[TX_8X8] +
                    tx_count_32x32p[TX_16X16] +
                    tx_count_32x32p[TX_32X32];
    ct_32x32p[1][0] = tx_count_32x32p[TX_8X8];
    ct_32x32p[1][1] = tx_count_32x32p[TX_16X16] +
                    tx_count_32x32p[TX_32X32];
    ct_32x32p[2][0] = tx_count_32x32p[TX_16X16];
    ct_32x32p[2][1] = tx_count_32x32p[TX_32X32];
}

void tx_counts_to_branch_counts_16x16(const unsigned int *tx_count_16x16p,
                                      unsigned int (*ct_16x16p)[2]) {
    ct_16x16p[0][0] = tx_count_16x16p[TX_4X4];
    ct_16x16p[0][1] = tx_count_16x16p[TX_8X8] + tx_count_16x16p[TX_16X16];
    ct_16x16p[1][0] = tx_count_16x16p[TX_8X8];
    ct_16x16p[1][1] = tx_count_16x16p[TX_16X16];
}

void tx_counts_to_branch_counts_8x8(const unsigned int *tx_count_8x8p,
                                    unsigned int (*ct_8x8p)[2]) {
    ct_8x8p[0][0] = tx_count_8x8p[TX_4X4];
    ct_8x8p[0][1] = tx_count_8x8p[TX_8X8];
}


INLINE int is_inter_mode(PREDICTION_MODE mode) {
  return mode >= NEARESTMV && mode <= NEWMV;
}


INLINE int mi_cols_aligned_to_sb(int n_mis) {
  return ALIGN_POWER_OF_TWO(n_mis, MI_BLOCK_SIZE_LOG2);
}

int get_tile_offset(int idx, int mis, int log2) {
    const int sb_cols = mi_cols_aligned_to_sb(mis) >> MI_BLOCK_SIZE_LOG2;
    const int offset = ((idx * sb_cols) >> log2) << MI_BLOCK_SIZE_LOG2;
    return MIN(offset, mis);
}

void vp9_tile_set_row(VP9_TILE_INFO_T *pTile, const VP9_COMMON_T *prCommon, int row) {
    pTile->i4MiRowStart = get_tile_offset(row, prCommon->u4MI_rows, prCommon->rUnCompressedHeader.u4Log2TileRows);
    pTile->i4MiRowEnd = get_tile_offset(row + 1, prCommon->u4MI_rows, prCommon->rUnCompressedHeader.u4Log2TileRows);
}

void vp9_tile_set_col(VP9_TILE_INFO_T *pTile, const VP9_COMMON_T *prCommon, int col) {
    pTile->i4MiColStart = get_tile_offset(col, prCommon->u4MI_cols, prCommon->rUnCompressedHeader.u4Log2TileCols);
    pTile->i4MiColEnd = get_tile_offset(col + 1, prCommon->u4MI_cols, prCommon->rUnCompressedHeader.u4Log2TileCols);
}


void vp9_tile_init(VP9_TILE_INFO_T *pTile, const VP9_COMMON_T *prCommon, int row, int col) {
    vp9_tile_set_row(pTile, prCommon, row);
    vp9_tile_set_col(pTile, prCommon, col);
}

// Reads the next tile returning its size and adjusting '*data' accordingly
// based on 'is_last'.
UINT32 get_tile_buffer(UINT32 u4DataStart, UINT32 u4DataEnd, UINT32 u4FifoStart, UINT32 u4FifoEnd, TileBuffer *pTileBuf) 
{
    UINT32 u4Size = 0;
    UCHAR* pData = (UCHAR *)(u4DataStart);
    UINT32 u4DoneSize = 0;
    UINT32 u4LeftSize = 0;
    
    if(u4DataStart > u4DataEnd)
    {
        //ringfifo case
        u4LeftSize = u4FifoEnd - u4DataStart + u4DataEnd - u4FifoStart;
    }
    else
    {
        u4LeftSize = u4DataEnd - u4DataStart;
    }
    
    if (!pTileBuf->fgLastTile) 
    {
        UINT32 u4Byte1, u4Byte2, u4Byte3, u4Byte4;
        if(!(4 <= u4LeftSize))
        {
            VP9_ERROR("truncated packet for left too small!  [0x%x 0x%x] in fifo[0x%x 0x%x]", u4DataStart, u4DataEnd, u4FifoStart, u4FifoEnd);
            return 0;
        }
        
        {
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
            UCHAR aucData[4];
            TZ_DMX_GetPicHeader(u4DataStart, u4FifoStart, u4FifoEnd, aucData, 4);
            u4Byte1 = (UINT32)(aucData[0]);
            u4Byte2 = (UINT32)(aucData[1]);
            u4Byte3 = (UINT32)(aucData[2]);
            u4Byte4 = (UINT32)(aucData[3]);
            pData = (((UINT32)pData + 4) > u4FifoEnd ? (UCHAR *)(u4FifoStart + (UINT32)pData + 4 - u4FifoEnd) : (pData + 4));
#else
            UINT32 u4VirFifoStart = VIRTUAL(u4FifoStart);
            UINT32 u4VirFifoEnd = VIRTUAL(u4FifoEnd);
            UINT32 u4VirDataStart = VIRTUAL(u4DataStart);
            UINT32 u4VirDataEnd = VIRTUAL(u4DataEnd);
            pData = (UCHAR *)(u4VirDataStart);
            u4Byte1 = (UINT32)(*(pData++));
            if((u4VirDataStart > u4VirDataEnd) && (u4VirDataStart + 4 >= u4VirFifoEnd))
            {
                if((UINT32)pData == u4VirFifoEnd)
                {
                    pData = (UCHAR*)u4VirFifoStart;
                }
                u4Byte2 = (UINT32)(*(pData++));
                if((UINT32)pData == u4VirFifoEnd)
                {
                    pData = (UCHAR*)u4VirFifoStart;
                }
                u4Byte3 = (UINT32)(*(pData++));
                if((UINT32)pData == u4VirFifoEnd)
                {
                    pData = (UCHAR*)u4VirFifoStart;
                }
                u4Byte4 = (UINT32)(*(pData++));
                if((UINT32)pData == u4VirFifoEnd)
                {
                    pData = (UCHAR*)u4VirFifoStart;
                }
            }
            else
            {
                u4Byte2 = (UINT32)(*(pData++));
                u4Byte3 = (UINT32)(*(pData++));
                u4Byte4 = (UINT32)(*(pData++));
            }
            pData = (UCHAR*)(PHYSICAL((UINT32)pData));
#endif
        }
        u4Size = u4Byte1 << 24 | u4Byte2 << 16 | u4Byte3 << 8 | u4Byte4;
        u4DoneSize += 4;
        
        if (u4Size > u4LeftSize)
        {
            VP9_ERROR("truncated packet for tile length too big! this:0x%x vs left:0x%x", u4Size, u4LeftSize);            
            return 0;
        }
    }
    else
    {
        u4Size = u4LeftSize;
    }
    
    pTileBuf->data = (UCHAR*)pData;
    pTileBuf->size = u4Size;

    u4DoneSize += u4Size;
    return u4DoneSize;
}

void vp9_get_tile_n_bits(int mi_cols,
                         int *min_log2_tile_cols, int *max_log2_tile_cols) {
    const int sb_cols = mi_cols_aligned_to_sb(mi_cols) >> MI_BLOCK_SIZE_LOG2;
    int min_log2 = 0, max_log2 = 0;

    // max
    while ((sb_cols >> max_log2) >= MIN_TILE_WIDTH_B64)
    ++max_log2;
    --max_log2;
    if (max_log2 < 0)
    max_log2 = 0;

    // min
    while ((MAX_TILE_WIDTH_B64 << min_log2) < sb_cols)
    ++min_log2;

    ASSERT(min_log2 <= max_log2);

    *min_log2_tile_cols = min_log2;
    *max_log2_tile_cols = max_log2;
}

static INLINE vp9_prob clip_prob(int p) {
  return (p > 255) ? 255u : (p < 1) ? 1u : p;
}

// int64 is not needed for normal frame level calculations.
// However when outputting entropy stats accumulated over many frames
// or even clips we can overflow int math.
static INLINE vp9_prob get_prob(int num, int den) {
  return (den == 0) ? 128u : clip_prob((num * 256 + (den >> 1)) / den);
}

static INLINE vp9_prob get_binary_prob(int n0, int n1) {
  return get_prob(n0, n0 + n1);
}

/* This function assumes prob1 and prob2 are already within [1,255] range. */
static INLINE vp9_prob weighted_prob(int prob1, int prob2, int factor) {
  return ROUND_POWER_OF_TWO(prob1 * (256 - factor) + prob2 * factor, 8);
}

static INLINE vp9_prob merge_probs(vp9_prob pre_prob,
                                   const unsigned int ct[2],
                                   unsigned int count_sat,
                                   unsigned int max_update_factor) {
  const vp9_prob prob = get_binary_prob(ct[0], ct[1]);
  const unsigned int count = MIN(ct[0] + ct[1], count_sat);
  const unsigned int factor = max_update_factor * count / count_sat;
  return weighted_prob(pre_prob, prob, factor);
}

void adapt_coef_probs(VP9_COMMON_T * prCommon, TX_SIZE tx_size,
                             unsigned int count_sat,
                             unsigned int update_factor) {
  const FRAME_CONTEXT *pre_fc = &prCommon->frame_contexts[prCommon->rUnCompressedHeader.u4FrameContextIdx];
  vp9_coeff_probs_model *const probs = prCommon->fc.coef_probs[tx_size];
  const vp9_coeff_probs_model *const pre_probs = pre_fc->coef_probs[tx_size];
  vp9_coeff_count_model *counts = prCommon->counts.coef[tx_size];
  unsigned int (*eob_counts)[REF_TYPES][COEF_BANDS][COEFF_CONTEXTS] = (unsigned int (*)[REF_TYPES][COEF_BANDS][COEFF_CONTEXTS])prCommon->counts.eob_branch[tx_size];
  int i, j, k, l, m;

  for (i = 0; i < PLANE_TYPES; ++i)
    for (j = 0; j < REF_TYPES; ++j)
      for (k = 0; k < COEF_BANDS; ++k)
        for (l = 0; l < BAND_COEFF_CONTEXTS(k); ++l) {
          const int n0 = counts[i][j][k][l][ZERO_TOKEN];
          const int n1 = counts[i][j][k][l][ONE_TOKEN];
          const int n2 = counts[i][j][k][l][TWO_TOKEN];
          const int neob = counts[i][j][k][l][EOB_MODEL_TOKEN];
          const unsigned int branch_ct[UNCONSTRAINED_NODES][2] = {
            { neob, eob_counts[i][j][k][l] - neob },
            { n0, n1 + n2 },
            { n1, n2 }
          };
          for (m = 0; m < UNCONSTRAINED_NODES; ++m)
            probs[i][j][k][l][m] = merge_probs(pre_probs[i][j][k][l][m],
                                               branch_ct[m],
                                               count_sat, update_factor);
        }
}

int adapt_prob(vp9_prob pre_prob, const unsigned int ct[2]) {
  return merge_probs(pre_prob, ct, COUNT_SAT, MAX_UPDATE_FACTOR);
}

static unsigned int tree_merge_probs_impl(unsigned int i,
                                          const vp9_tree_index *tree,
                                          const vp9_prob *pre_probs,
                                          const unsigned int *counts,
                                          unsigned int count_sat,
                                          unsigned int max_update,
                                          vp9_prob *probs) {
  const int l = tree[i];
  const unsigned int left_count = (l <= 0)
                 ? counts[-l]
                 : tree_merge_probs_impl(l, tree, pre_probs, counts,
                                         count_sat, max_update, probs);
  const int r = tree[i + 1];
  const unsigned int right_count = (r <= 0)
                 ? counts[-r]
                 : tree_merge_probs_impl(r, tree, pre_probs, counts,
                                         count_sat, max_update, probs);
  const unsigned int ct[2] = { left_count, right_count };
  probs[i >> 1] = merge_probs(pre_probs[i >> 1], ct,
                              count_sat, max_update);
  return left_count + right_count;
}

static void vp9_tree_merge_probs(const vp9_tree_index *tree, const vp9_prob *pre_probs,
                          const unsigned int *counts, unsigned int count_sat,
                          unsigned int max_update_factor, vp9_prob *probs) {
  tree_merge_probs_impl(0, tree, pre_probs, counts, count_sat,
                        max_update_factor, probs);
}

void adapt_probs(const vp9_tree_index *tree,
                        const vp9_prob *pre_probs, const unsigned int *counts,
                        vp9_prob *probs) {
  vp9_tree_merge_probs(tree, pre_probs, counts, COUNT_SAT, MAX_UPDATE_FACTOR,
                   probs);
}

UINT32 VP9_Clamp(INT32 value, INT32 low, INT32 high)
{
    return value < low ? low : (value > high ? high : value);
}

UINT32 vp9_dc_quant(UINT32 qindex, UINT32 delta)
{
    return dc_qlookup[VP9_Clamp(qindex + delta, 0, MAXQ)];
}

UINT32 vp9_ac_quant(UINT32 qindex, UINT32 delta) 
{
    return ac_qlookup[VP9_Clamp(qindex + delta, 0, MAXQ)];
}

static UINT32 vp9_segfeature_active(SEGMENTATION *seg, UINT32 segment_id, SEG_LVL_FEATURES feature_id) 
{
    return seg->enabled && (seg->feature_mask[segment_id] & (1 << feature_id));
}

static UINT32 vp9_get_segdata(SEGMENTATION *seg, UINT32 segment_id, SEG_LVL_FEATURES feature_id) 
{
    return seg->feature_data[segment_id][feature_id];
}

UINT32 vp9_get_qindex(SEGMENTATION *seg, UINT32 segment_id, UINT32 base_qindex) 
{
    if (vp9_segfeature_active(seg, segment_id, SEG_LVL_ALT_Q)) 
    {
        INT32 data = vp9_get_segdata(seg, segment_id, SEG_LVL_ALT_Q);
        INT32 seg_qindex = seg->abs_delta == SEGMENT_ABSDATA ? data : base_qindex + data;
        return VP9_Clamp(seg_qindex, 0, MAXQ);
    }
    else 
    {
        return base_qindex;
    }
}

INT32 iMax_RRF_BLK_Cal(INT32 iScale_Fac)
{
  int cuch_max_rrf_blk_result;
  int scale_ratio_1000;
    
  //printf("scale_fac = %d\n", scale_fac);

  scale_ratio_1000 = (iScale_Fac*1000)/16384;
  
  //  max rrf blk restriction
  //  rest1 : QIU buffer 23x23 for each bank
  //  rest2 : no 3_page_row DRAM access

  if(scale_ratio_1000 <= 1000)
    cuch_max_rrf_blk_result = 4;  //16 pixels 
  else if((scale_ratio_1000 > 1000) && (scale_ratio_1000 < 1800))
    cuch_max_rrf_blk_result = 3;  // 8 pixels
  else if(scale_ratio_1000 >= 1800)
    
#if RRF_NEW_FORMULA
    cuch_max_rrf_blk_result = 3; //20140618 modify for perf
#else
    cuch_max_rrf_blk_result = 2; // 4 pixels
#endif

  //printf("scale_ratio = %d, cuch_max_rrf_blk_result = %d\n", scale_ratio_1000, cuch_max_rrf_blk_result);

  return cuch_max_rrf_blk_result;
}

static INT32 iVP9_Check_Scale_Factors(INT32 other_w, INT32 other_h, INT32 this_w, INT32 this_h) 
{
  return 2 * this_w >= other_w &&
         2 * this_h >= other_h &&
         this_w <= 16 * other_w &&
         this_h <= 16 * other_h;
}

static INT32 iGet_Fixed_Point_Scale_Factor(INT32 other_size, INT32 this_size) 
{
  // Calculate scaling factor once for each reference frame
  // and use fixed point scaling factors in decoding and encoding routines.
  // Hardware implementations can calculate scale factor in device driver
  // and use multiplication and shifting on hardware instead of division.
  return (other_size << REF_SCALE_SHIFT) / this_size;
}

static INLINE INT32 iScaled_X(INT32 val, VP9_Scale_Factors_T *sf) 
{
    return (int)(val * sf->i4X_scale_fp >> REF_SCALE_SHIFT);
}

static INLINE INT32 iScaled_Y(INT32 val, VP9_Scale_Factors_T *sf) 
{
    return (int)(val * sf->i4Y_scale_fp >> REF_SCALE_SHIFT);
}

static INLINE INT32 iVP9_Is_Scaled(VP9_Scale_Factors_T *sf)
{
    return sf->i4X_scale_fp != REF_NO_SCALE || sf->i4Y_scale_fp != REF_NO_SCALE;
}

void vVP9_Setup_Scale_Factors_For_Frame(VP9_Scale_Factors_T *rScaleFactors,
                                       INT32 other_w, INT32 other_h,
                                       INT32 this_w, INT32 this_h) 
{
    if (!iVP9_Check_Scale_Factors(other_w, other_h, this_w, this_h)) 
    {
        rScaleFactors->i4X_scale_fp = REF_INVALID_SCALE;
        rScaleFactors->i4Y_scale_fp = REF_INVALID_SCALE;
        VP9_ERROR("@@@@ REF_INVALID_SCALE @@@@@");
        return;
    }

    #if 0
    if(cuch_mc_dec_order_frame_idx == 4)
    {
      cuch_i=cuch_i;//cuch: gotoline
    }
    #endif
  
    rScaleFactors->i4X_scale_fp = iGet_Fixed_Point_Scale_Factor(other_w, this_w);
    rScaleFactors->i4Y_scale_fp = iGet_Fixed_Point_Scale_Factor(other_h, this_h);
    rScaleFactors->i4X_step_q4 = iScaled_X(16, rScaleFactors);
    rScaleFactors->i4Y_step_q4 = iScaled_Y(16, rScaleFactors);

    if((rScaleFactors->i4X_scale_fp != (1<<14))|| (rScaleFactors->i4Y_scale_fp != (1<<14)))
    {
        VP9_INFO("RRF!! scale_fp--> X:%d Y:%d", rScaleFactors->i4X_scale_fp, rScaleFactors->i4Y_scale_fp); 
    }

    if (iVP9_Is_Scaled(rScaleFactors)) 
    {
        rScaleFactors->u4Ref_Scaling_EN = 1;
    } 
    else 
    {
        rScaleFactors->u4Ref_Scaling_EN = 0;
    }
    
#if 0
  // TODO(agrange): Investigate the best choice of functions to use here
  // for EIGHTTAP_SMOOTH. Since it is not interpolating, need to choose what
  // to do at full-pel offsets. The current selection, where the filter is
  // applied in one direction only, and not at all for 0,0, seems to give the
  // best quality, but it may be worth trying an additional mode that does
  // do the filtering on full-pel.
  if (sf->x_step_q4 == 16) {
    if (sf->y_step_q4 == 16) {
      // No scaling in either direction.
      sf->predict[0][0][0] = vp9_convolve_copy;
      sf->predict[0][0][1] = vp9_convolve_avg;
      sf->predict[0][1][0] = vp9_convolve8_vert;
      sf->predict[0][1][1] = vp9_convolve8_avg_vert;
      sf->predict[1][0][0] = vp9_convolve8_horiz;
      sf->predict[1][0][1] = vp9_convolve8_avg_horiz;
    } else {
      // No scaling in x direction. Must always scale in the y direction.
      sf->predict[0][0][0] = vp9_convolve8_vert;
      sf->predict[0][0][1] = vp9_convolve8_avg_vert;
      sf->predict[0][1][0] = vp9_convolve8_vert;
      sf->predict[0][1][1] = vp9_convolve8_avg_vert;
      sf->predict[1][0][0] = vp9_convolve8;
      sf->predict[1][0][1] = vp9_convolve8_avg;
    }
  } else {
    if (sf->y_step_q4 == 16) {
      // No scaling in the y direction. Must always scale in the x direction.
      sf->predict[0][0][0] = vp9_convolve8_horiz;
      sf->predict[0][0][1] = vp9_convolve8_avg_horiz;
      sf->predict[0][1][0] = vp9_convolve8;
      sf->predict[0][1][1] = vp9_convolve8_avg;
      sf->predict[1][0][0] = vp9_convolve8_horiz;
      sf->predict[1][0][1] = vp9_convolve8_avg_horiz;
    } else {
      // Must always scale in both directions.
      sf->predict[0][0][0] = vp9_convolve8;
      sf->predict[0][0][1] = vp9_convolve8_avg;
      sf->predict[0][1][0] = vp9_convolve8;
      sf->predict[0][1][1] = vp9_convolve8_avg;
      sf->predict[1][0][0] = vp9_convolve8;
      sf->predict[1][0][1] = vp9_convolve8_avg;
    }
  }
  // 2D subpel motion always gets filtered in both directions
  sf->predict[1][1][0] = vp9_convolve8;
  sf->predict[1][1][1] = vp9_convolve8_avg;
#endif
}

void vVP9_Init_Dequantizer(VP9_COMMON_T *prCommon) 
{        
    UINT32 q;
    for (q = 0; q < QINDEX_RANGE; q++) 
    {
        prCommon->au4Y_Dequant[q][0] = vp9_dc_quant(q, 0);
        prCommon->au4Y_Dequant[q][1] = vp9_ac_quant(q, 0);
        prCommon->au4UV_Dequant[q][0] = vp9_dc_quant(q, 0);
        prCommon->au4UV_Dequant[q][1] = vp9_ac_quant(q, 0);
     }
}

void vVP9_Set_Default_LF_Deltas( VP9_LOOP_FILTER_INFO_T* lf) 
{
    lf->mode_ref_delta_enabled = 1;
    lf->mode_ref_delta_update = 1;

    lf->ref_deltas[INTRA_FRAME] = 1;
    lf->ref_deltas[LAST_FRAME] = 0;
    lf->ref_deltas[GOLDEN_FRAME] = -1;
    lf->ref_deltas[ALTREF_FRAME] = -1;

    lf->mode_deltas[0] = 0;
    lf->mode_deltas[1] = 0;
}


//---------------------------------------------------------------------------
BOOL fgVP9IntraOnly(VP9_COMMON_T * prCommon)
{
    if ((prCommon->rUnCompressedHeader.u4FrameType == KEY_FRAME || prCommon->rUnCompressedHeader.u4IntraOnly))
    {
        return TRUE;
    }
    
    return FALSE;
}

// Set decoder related registers here.
void vVP9SetPicInfo(VP9_COMMON_T* prCommon, UINT32 u4CoreId)
{
    VP9_UNCOMPRESSED_HEADER_T* prUnCompressed = &prCommon->rUnCompressedHeader;
    UINT32 u4CompoundVal;
    UINT32 u4Tmp1 = 0;    
    UINT32 u4Tmp2 = 0;
    UINT32 i4Idx;
    UINT32 i;
    INT32 aiMax_RRF_BLK_TmpX[MAX_REF_FRAMES];
    INT32 aiMax_RRF_BLK_TmpY[MAX_REF_FRAMES];
    INT32 i4Use_prev_in_find_mv_refs;

    
    for(i4Idx=0; i4Idx<=3; i4Idx++)
    {
        u4Tmp1 = u4Tmp1 + (prCommon->REF_FRAME_SIGN_BIAS[i4Idx]<<i4Idx);
    }

    for(i4Idx=0; i4Idx<=1; i4Idx++)
    {
        u4Tmp2 = u4Tmp2 + (prCommon->COMP_VAR_REF[i4Idx]<<(i4Idx*2));    
    }
 
    u4CompoundVal = (u4Tmp2<<24) + (u4Tmp1<<16) + (prCommon->COMP_FIXED_REF<<8) + (prCommon->eRefMode) ;

    vVDEC_HAL_VP9_Set_Compound_Ref(u4CoreId, u4CompoundVal);

    // MV settings

    // set prev mode info, check use previous mv enable or not
    i4Use_prev_in_find_mv_refs = prUnCompressed->u4Width == prCommon->u4LastWidth &&
                                 prUnCompressed->u4Height== prCommon->u4LastHeight &&
                                !prUnCompressed->u4IntraOnly && prCommon->u4LastShowFrame;
    
    // Special case: set fgUse_Prev_MI to FALSE when the previous mode info
    // context cannot be used.    

#if (VP9_CONFIG_CHIP_VER_CURR > VP9_CONFIG_CHIP_VER_MT5890)
    prUnCompressed->fgUse_Prev_MI = (!prUnCompressed->u4ErrResilenceMode) && (i4Use_prev_in_find_mv_refs);
#else
    prUnCompressed->fgUse_Prev_MI = prUnCompressed->fgUse_Prev_MI && i4Use_prev_in_find_mv_refs;
#endif 
    vVDEC_HAL_VP9_Set_MVP_Enable(u4CoreId, prUnCompressed->fgUse_Prev_MI);
    
    vVDEC_HAL_VP9_Set_MV_Buffer_Addr(u4CoreId, prCommon->rMVBuffer.u4BufPAddr);
    // ~MV Settings Done

    // MC Ref Buffer Settings
    if(u4CoreId != VP9_LAE_ID)
    {
        vVDEC_HAL_VP9_Set_MC_RefBuf_Addr(u4CoreId, prCommon->FRAME_REFS[0], prCommon->FRAME_REFS[1], prCommon->FRAME_REFS[2]);

        if (prUnCompressed->u4FrameType != KEY_FRAME)
        {
            vVDEC_HAL_VP9_Set_MC_Ref_Scaling_Enable(u4CoreId, prCommon->FRAME_REFS[0].rScaleFactors.u4Ref_Scaling_EN,
                                                    prCommon->FRAME_REFS[1].rScaleFactors.u4Ref_Scaling_EN,
                                                    prCommon->FRAME_REFS[2].rScaleFactors.u4Ref_Scaling_EN);
            for(i = 0; i < REFS_PER_FRAME; i++)
            {
                vVDEC_HAL_VP9_Set_MC_Set_UMV(u4CoreId,  prCommon->FRAME_REFS[i].prBuf->u4YCropWidth,
                                             prCommon->FRAME_REFS[i].prBuf->u4YCropHeight, i);
            
                if(prCommon->FRAME_REFS[i].rScaleFactors.u4Ref_Scaling_EN == 1)
                {
                    vVDEC_HAL_VP9_Set_MC_Ref_Scaling_Step(u4CoreId, prCommon->FRAME_REFS[i].rScaleFactors.i4X_step_q4,
                                                          prCommon->FRAME_REFS[i].rScaleFactors.i4Y_step_q4, i);
                    vVDEC_HAL_VP9_Set_MC_Set_Scaling_Factor(u4CoreId, prCommon->FRAME_REFS[i].rScaleFactors.i4X_scale_fp,
                                                            prCommon->FRAME_REFS[i].rScaleFactors.i4Y_scale_fp, i);
                
                    aiMax_RRF_BLK_TmpX[i] = iMax_RRF_BLK_Cal(prCommon->FRAME_REFS[i].rScaleFactors.i4X_scale_fp);
                    aiMax_RRF_BLK_TmpY[i] = iMax_RRF_BLK_Cal(prCommon->FRAME_REFS[i].rScaleFactors.i4Y_scale_fp);  
                }
                else
                {
                    aiMax_RRF_BLK_TmpX[i] = 7;
                    aiMax_RRF_BLK_TmpY[i] = 7;
                }
                vVDEC_HAL_VP9_Set_MC_Ref_Pitch(u4CoreId, prCommon->FRAME_REFS[i].prBuf->u4YCropWidth, i);
            }
            vVDEC_HAL_VP9_Set_MC_MAX_RRF_BLK_Size(u4CoreId, aiMax_RRF_BLK_TmpX, aiMax_RRF_BLK_TmpY);
        }    
        vVDEC_HAL_VP9_Set_MC_MI_COLS_ROWS(u4CoreId, prCommon->u4MI_rows, prCommon->u4MI_cols);
    }
    //~~~ MC settings

    // PP settings
    if(u4CoreId != VP9_LAE_ID)
    {
        vVDEC_HAL_VP9_Set_PP_EN(u4CoreId, 0x1);    
    
        vVDEC_HAL_VP9_Set_MC_DecodeBuf_Addr(u4CoreId, prCommon->FRAME_BUFS[prCommon->u4NewFbIdx].rBuf.rBufY.u4BufPAddr,
                                            prCommon->FRAME_BUFS[prCommon->u4NewFbIdx].rBuf.rBufC.u4BufPAddr);
    
        vVDEC_HAL_VP9_Set_PP_MB_Width(u4CoreId, prCommon->pCurrentFB->rBuf.u4YCropWidth);
        vVDEC_HAL_VP9_Set_PP_DBK_EN(u4CoreId, 3);                                                       // Turn on Y, CBCR Deblocking
        vVDEC_HAL_VP9_Set_PP_WriteByPost(u4CoreId, 1);                                                  // prevents motion compensator from writing to DRAM
        vVDEC_HAL_VP9_Set_PP_MB_LeftRightMostIdx(u4CoreId, prCommon->pCurrentFB->rBuf.u4YCropWidth);   
        vVDEC_HAL_VP9_Set_PP_MB_UpDownMostIdx(u4CoreId, prCommon->pCurrentFB->rBuf.u4YCropHeight);
        vVDEC_HAL_VP9_Set_PP_Display_Range(u4CoreId, prCommon->pCurrentFB->rBuf.u4YCropWidth, prCommon->pCurrentFB->rBuf.u4YCropHeight);
    }
    // ~PP Settins 

    // SQT settings
    if(u4CoreId != VP9_LAE_ID)
    {
        vVDEC_HAL_VP9_Set_SQT_IQ_SRAM_EN(u4CoreId, 0x1);  // enable write inverse quant table   
        vVDEC_HAL_VP9_Set_SQT_Q_Table(u4CoreId, prCommon->au4DeQuant);
        vVDEC_HAL_VP9_Set_SQT_IQ_SRAM_EN(u4CoreId, 0x0);  // disable write inverse quant table
    }
    //~~~ SQT settings

    // UFO Config
    if(prCommon->fgUFOModeEnable && u4CoreId != VP9_LAE_ID)
    {        
        UINT32 u4SizeY2LenY = prCommon->pCurrentFB->rBuf.rUFO_LEN_Y.u4BufPAddr - prCommon->pCurrentFB->rBuf.rBufY.u4BufPAddr;        
        UINT32 u4SizeC2LenC = prCommon->pCurrentFB->rBuf.rUFO_LEN_C.u4BufPAddr - prCommon->pCurrentFB->rBuf.rBufC.u4BufPAddr;
        
        vVDEC_HAL_VP9_UFO_Config(u4CoreId, prCommon->rUnCompressedHeader.u4Width,prCommon->rUnCompressedHeader.u4Height,
                                 u4SizeY2LenY, u4SizeC2LenC, 
                                 prCommon->pCurrentFB->rBuf.rUFO_LEN_Y.u4BufPAddr,  prCommon->pCurrentFB->rBuf.rUFO_LEN_C.u4BufPAddr);
    }
    // workaround
#if 1
    if(prCommon->fgReDecode && u4CoreId != VP9_LAE_ID)
    {
        vVDEC_HAL_VP9_Set_WorkaroundVLD(u4CoreId);
    }
#endif
    
}

void vVP9_Set_MB_MI(VP9_COMMON_T *prCommon, UINT32 u4Aligned_width, UINT32 u4Aligned_height) 
{
  prCommon->u4MI_cols = u4Aligned_width >> MI_SIZE_LOG2;
  prCommon->u4MI_rows = u4Aligned_height >> MI_SIZE_LOG2;
  prCommon->u4MI_stride = prCommon->u4MI_cols + MI_BLOCK_SIZE;

  prCommon->u4MB_cols = (prCommon->u4MI_cols + 1) >> 1;
  prCommon->u4MB_rows = (prCommon->u4MI_rows + 1) >> 1;
  prCommon->u4MBs = prCommon->u4MB_rows * prCommon->u4MB_cols;
}

#if 0
INT32 iVP9UpdateSeqHeader(VP9_COMMON_T * prCommon, UCHAR ucFbgId, UINT32 u4Width, UINT32 u4Height)
{
    FBM_SEQ_HDR_T* prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
    UINT32 u4fps = 0;
    prFbmSeqHdr->fgResizeSmallPic=FALSE;
    prFbmSeqHdr->u4YAddrOffset   = 0;
    prFbmSeqHdr->u4CAddrOffset   = 0;
    prFbmSeqHdr->fgRasterOrder   = 0;
    prFbmSeqHdr->fgProgressiveSeq = TRUE;
    prFbmSeqHdr->ucAspRatInf         = 1;        // 16:9
    prFbmSeqHdr->ucFrmRatCod         = 5;        // 30 fps
    prFbmSeqHdr->u2HSize             = u4Width;
    prFbmSeqHdr->u2VSize             = u4Height;
    prFbmSeqHdr->u2OrgHSize          = u4Width;
    prFbmSeqHdr->u2OrgVSize          = u4Height;
    prFbmSeqHdr->u2LineSize          = (UINT16)(VP9_ALIGN_INC(u4Width, 64));
    prFbmSeqHdr->ucBlockType = 0;// 1 for 16x32, 0 for 64x32
    prFbmSeqHdr->eSwapMode = FBM_SM_NO_SWAP;
    
    //Bit-depth info
    if(1)
    {
        prFbmSeqHdr->u1Depth = 8;
        prFbmSeqHdr->ePackMode = FBM_PM_NORMAL;
    }
    else
    {
        prFbmSeqHdr->u1Depth = 10;
        prFbmSeqHdr->ePackMode = FBM_PM_COMPACT;
    }
    
    //UFO info
    if(prCommon->fgUFOModeEnable)
    {
        prFbmSeqHdr->fgUfoEn = TRUE;
    }
    else
    {
        prFbmSeqHdr->fgUfoEn = FALSE;
    }
                
    if(prVdecEsInfo->rMMSeqInfo.e_src_asp!=VDEC_ASPECT_RATIO_NONE 
        &&prVdecEsInfo->rMMSeqInfo.e_src_asp<VDEC_ASPECT_RATIO_MAX)
    {
        prFbmSeqHdr->ucAspRatInf = prVdecEsInfo->rMMSeqInfo.e_src_asp;
    }
    else
    {
        prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_16_9;        // 16:9
    }
    
    switch (prVdecEsInfo->rMMSeqInfo.e_frame_rate)
    {
    case VDEC_SRC_FRAME_RATE_23_976:
        u4fps = 23976;
        break;
    case VDEC_SRC_FRAME_RATE_24:
        u4fps = 24000;
        break;
    case VDEC_SRC_FRAME_RATE_25:
        u4fps = 25000;
        break;
    case VDEC_SRC_FRAME_RATE_29_97:
        u4fps = 29970;
        break;
    case VDEC_SRC_FRAME_RATE_30:
        u4fps = 30000;
        break;
    case VDEC_SRC_FRAME_RATE_50:
        u4fps = 50000;
        break;
    case VDEC_SRC_FRAME_RATE_59_94:
        u4fps = 59940;
        break;
    case VDEC_SRC_FRAME_RATE_60:
        u4fps = 60000;
        break;
    default:
        u4fps = 0;
        break;
    }

    switch (u4fps)
    {
        case 22:
        case 23:
            prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24_;
            break;
        case 24:
            prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24;
            break;
        case 25:
            prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_25;
            break;
        case 30:
            prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30;
            break;
        case 29:
            prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30_;
            break;
        case 50:
            prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_50;
            break;
        case 60:
            prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60;
            break;
        case 59:
            prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60_;
            break;
        default:
            prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_UNKNOWN;
            break;
    }
    
//              FBM_SetSeqChgFbId(ucFbgId, ucFbmFbId);
    FBM_SetPlayMode(ucFbgId, FBM_FBG_MM_MODE);
    FBM_SetFrameBufferFlag(ucFbgId, FBM_FLAG_SEQ_CHG);
}
#endif

INT32 iVP9ApplyFrameSize(VP9_COMMON_T * prCommon, UINT32 u4Width, UINT32 u4Height) {
    UINT32 i;
    UINT32 u4SBWidth, u4SBHeight;    
    UINT32 PIC_SIZE_Y,PIC_SIZE_C;
    UINT32 UFO_LEN_SIZE_Y, UFO_LEN_SIZE_C;
    UINT32 MV_BUF_SIZE;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prCommon->ucEsId);
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(prCommon->ucEsId);
    VP9_COMMON_T* prMainCommon = NULL;
    VP9_UNCOMPRESSED_HEADER_T* prUnCompressHdr = &prCommon->rUnCompressedHeader;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;
    VP9_REF_CNT_BUF_T* pNewFB = NULL;
    VP9_LAE_INFO_T *_rLaeInfo = NULL;
    // actual pic 
    u4SBWidth = ((u4Width + 63) >> 6);
    u4SBHeight = ((u4Height + 63) >> 6);

    PIC_SIZE_Y =  (u4SBWidth * u4SBHeight) << (6 + 6);    
    PIC_SIZE_C = PIC_SIZE_Y >> 1;
    MV_BUF_SIZE = u4SBWidth * u4SBHeight * 36 * 16;

    if(prCommon->fgUFOModeEnable)
    {
        UFO_LEN_SIZE_Y = ((((PIC_SIZE_Y + 255)>> 8)+ 63 + (16*8)) >> 6 ) << 6;
        UFO_LEN_SIZE_C = (((UFO_LEN_SIZE_Y >> 1) + 15 + (16*8)) >> 4) << 4;
    }

    if(!prVdecEsInfo->ucDualCoreMode && (prCommon->ucFbgId == FBM_FBG_ID_UNKNOWN))
    {
        UCHAR ucFbgType = 0xFF;
        UCHAR ucFbgId = FBM_FBG_ID_UNKNOWN;            
        UINT32 u4MiscBufAddr = 0;
        FBM_CREATE_FBG_PAR_T rPar;
        ucFbgType = FBM_SelectGroupType(u4Width, u4Height);
        if(ucFbgType == FBM_FBG_TYPE_UNKNOWN)
        {
            VP9_PRINT_ERROR(prCommon->ucEsId, "select fbg type failed !");
            return VP9_FAIL;
        }
        
        //lae use main's info to create fbg
        x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
        rPar.u1VdecId = (UINT8)prCommon->ucEsId;
        rPar.fgThumbnailMode = prVdecEsInfo->u4RenderPicCnt;
        rPar.fg10Bit = (prUnCompressHdr->u4BitDepth == BITS_10) ? TRUE : FALSE;
        rPar.fgUFO = prCommon->fgUFOModeEnable;        
        if(prCommon->fgLAEThread)
        {
            prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prCommon->ucPairedEsId);
            prVdecEsInfo = _VDEC_GetEsInfo(prCommon->ucPairedEsId);
            prMainCommon = prVP9GetCommon(prCommon->ucPairedEsId);
            rPar.u1VdecId = (UINT8)prCommon->ucPairedEsId;
        }
        
        if(prVdecEsInfoKeep->fgVPush && (prVdecEsInfoKeep->eVPushPlayMode != VDEC_PUSH_MODE_TUNNEL))
        {
            rPar.u1AppMode = FBM_FBG_APP_OMX_DISP;
        }
        else
        {
            rPar.u1AppMode = FBM_FBG_APP_NORMAL;
        }

        ucFbgId =FBM_CreateGroupExt(ucFbgType, FBM_VDEC_VP9, u4Width, u4Height, &rPar);
        if(ucFbgId == FBM_FBG_ID_UNKNOWN)
        {
            VP9_PRINT_ERROR(prCommon->ucEsId, "create fbg failed !");
            return VP9_FAIL;
        }

        FBM_SetPlayMode(ucFbgId, FBM_FBG_MM_MODE);

        if(prVdecEsInfo->fgPlaybackNotDisplay || prVdecEsInfoKeep->fgVPushDecodeOnly || prVdecEsInfoKeep->fgPatternMode)
        {
            FBM_SetPlayMode(ucFbgId,FBM_FBG_NOTDISPLAY_MODE);
        }
        
        if(prVdecEsInfoKeep->fgVPush)
        {
            FBM_RegFbgCbFunc(ucFbgId, FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_VPUSH_DecodeDone, rPar.u1VdecId);
        }
        
        prCommon->ucFbgId = ucFbgId;
        prVdecEsInfo->ucFbgId = ucFbgId;
        if(prCommon->fgLAEThread)
        {
            (_VDEC_GetEsInfo(prCommon->ucEsId))->ucFbgId = ucFbgId;
        }
        FBM_SetSyncStc(ucFbgId, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId);
        FBM_FbgChgNotify(ucFbgId, prCommon->fgLAEThread ? prCommon->ucPairedEsId : prCommon->ucEsId);
        prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
        FBM_GetFrameBufferAddrMv(ucFbgId, 0, &(prCommon->rMVBuffer.u4BufPAddr));
        VERIFY(prCommon->rMVBuffer.u4BufPAddr);
        prCommon->rMVBuffer.u4BufSize = MV_BUF_SIZE;

        if(prCommon->fgLAEThread)
        {
            FBM_GetFrameBufferAddrMv(ucFbgId, 1, &(prMainCommon->rMVBuffer.u4BufPAddr));
            VERIFY(prMainCommon->rMVBuffer.u4BufPAddr > (prCommon->rMVBuffer.u4BufPAddr + MV_BUF_SIZE));
            prMainCommon->rMVBuffer.u4BufSize = MV_BUF_SIZE;
        }

        if(prCommon->fgLAEThread)
        {
            UINT32 u4BufIdx;
            _rLaeInfo = GetLaeInfo();
            for (u4BufIdx = 0; u4BufIdx < MAX_LAE_BUF_NS_PER_GROUP; u4BufIdx++)
            {
                u4MiscBufAddr =  FBM_GetCabacMemAddr(ucFbgId,u4BufIdx);
                VERIFY(u4MiscBufAddr > prMainCommon->rMVBuffer.u4BufPAddr + prMainCommon->rMVBuffer.u4BufSize);
                _rLaeInfo[u4BufIdx].rLAEBuffer.u4BufPAddr = u4MiscBufAddr;
                _rLaeInfo[u4BufIdx].rLAEBuffer.u4BufSize = VP9_LAE_BUFFER_SZ;
                _rLaeInfo[u4BufIdx].rErrorBuffer.u4BufPAddr = u4MiscBufAddr + VP9_LAE_BUFFER_SZ;
                _rLaeInfo[u4BufIdx].rErrorBuffer.u4BufSize = VP9_ERR_BUFFER_SZ;
                _rLaeInfo[u4BufIdx].ucFbgId = ucFbgId;
                if(prCommon->prVp9LaeInput == &(_rLaeInfo[u4BufIdx]))
                {
                    x_memcpy(&prCommon->rLAEBuffer, &(_rLaeInfo[u4BufIdx].rLAEBuffer), sizeof(VP9_DRAM_BUF_T));
                    x_memcpy(&prCommon->rErrorBuffer, &(_rLaeInfo[u4BufIdx].rErrorBuffer), sizeof(VP9_DRAM_BUF_T));
                }
            }
        }

        u4MiscBufAddr = FBM_GetMiscMemAddr(ucFbgId);
        if(prCommon->fgLAEThread)
        {
            _rLaeInfo = GetLaeInfo();
            VERIFY(u4MiscBufAddr > (_rLaeInfo[MAX_LAE_BUF_NS_PER_GROUP-1].rErrorBuffer.u4BufPAddr + _rLaeInfo[MAX_LAE_BUF_NS_PER_GROUP-1].rErrorBuffer.u4BufSize));
        }

#ifdef CC_VDEC_FBM_DYNAMIC_MAPPING
        FBM_WorkBufPartMap(ucFbgId, u4MiscBufAddr, FBM_WORK_BUF_VP9_MISC_TBL_SIZE);
#endif

        prCommon->rSegIdBuffer.u4BufPAddr = u4MiscBufAddr;        
        prCommon->rSegIdBuffer.u4BufVAddr = VIRTUAL(u4MiscBufAddr);
        prCommon->rSegIdBuffer.u4BufSize = VP9_SEG_ID_SZ;
        u4MiscBufAddr += VP9_SEG_ID_SZ;

        if(prCommon->fgLAEThread)
        {
            prMainCommon->rSegIdBuffer.u4BufPAddr = u4MiscBufAddr;
            prMainCommon->rSegIdBuffer.u4BufVAddr = VIRTUAL(u4MiscBufAddr);
            prMainCommon->rSegIdBuffer.u4BufSize = VP9_SEG_ID_SZ;
            u4MiscBufAddr += VP9_SEG_ID_SZ;
        }

        prCommon->rTileBuffer.u4BufPAddr = u4MiscBufAddr;        
        prCommon->rTileBuffer.u4BufVAddr = VIRTUAL(u4MiscBufAddr);
        prCommon->rTileBuffer.u4BufSize = VP9_TILE_BUFFER_SIZE;
        u4MiscBufAddr += VP9_TILE_BUFFER_SIZE;
        
        if(prCommon->fgLAEThread)
        {
            prMainCommon->rTileBuffer.u4BufPAddr = u4MiscBufAddr;
            prMainCommon->rTileBuffer.u4BufVAddr = VIRTUAL(u4MiscBufAddr);
            prMainCommon->rTileBuffer.u4BufSize = VP9_TILE_BUFFER_SIZE;
            u4MiscBufAddr += VP9_TILE_BUFFER_SIZE;
        }

        prCommon->rCountTBLBuffer.u4BufPAddr = u4MiscBufAddr;
        prCommon->rCountTBLBuffer.u4BufVAddr = VIRTUAL(u4MiscBufAddr);
        prCommon->rCountTBLBuffer.u4BufSize = VP9_COUNT_TBL_SZ;
        u4MiscBufAddr += VP9_COUNT_TBL_SZ;

        prCommon->rProbTBLBuffer.u4BufPAddr = u4MiscBufAddr;
        prCommon->rProbTBLBuffer.u4BufVAddr = VIRTUAL(u4MiscBufAddr);
        prCommon->rProbTBLBuffer.u4BufSize = VP9_PROB_TBL_SZ;
        u4MiscBufAddr += VP9_PROB_TBL_SZ;
        VP9_PRINT_DEBUG(prCommon->ucEsId, "rSegIdBuffer:0x%08X (0x%08X) Size:0x%x", prCommon->rSegIdBuffer.u4BufVAddr, PHYSICAL(prCommon->rSegIdBuffer.u4BufVAddr), prCommon->rSegIdBuffer.u4BufSize);
        VP9_PRINT_DEBUG(prCommon->ucEsId, "rTileBuffer:0x%08X (0x%08X) Size:0x%x", prCommon->rTileBuffer.u4BufVAddr, PHYSICAL(prCommon->rTileBuffer.u4BufVAddr), prCommon->rTileBuffer.u4BufSize);
        VP9_PRINT_DEBUG(prCommon->ucEsId, "rCountTBLBuffer:0x%08X (0x%08X) Size:0x%x", prCommon->rCountTBLBuffer.u4BufVAddr, PHYSICAL(prCommon->rCountTBLBuffer.u4BufVAddr), prCommon->rCountTBLBuffer.u4BufSize);
        VP9_PRINT_DEBUG(prCommon->ucEsId, "rProbTBLBuffer:0x%08X (0x%08X) Size:0x%x", prCommon->rProbTBLBuffer.u4BufVAddr, PHYSICAL(prCommon->rProbTBLBuffer.u4BufVAddr), prCommon->rProbTBLBuffer.u4BufSize);
        for (i = 0; i < FRAME_BUFFERS; ++i)
        {
            prCommon->FRAME_BUFS[i].rBuf.rBufY.u4BufSize = PIC_SIZE_Y;
            prCommon->FRAME_BUFS[i].rBuf.rBufC.u4BufSize = PIC_SIZE_C;
            prCommon->FRAME_BUFS[i].rBuf.rUFO_LEN_Y.u4BufSize = UFO_LEN_SIZE_Y;
            prCommon->FRAME_BUFS[i].rBuf.rUFO_LEN_C.u4BufSize = UFO_LEN_SIZE_C;   
        }

        //update sequence header
        if (prFbmSeqHdr != NULL && prVdecEsInfo != NULL)
        {
            UINT32 u4fps = 0;
            prFbmSeqHdr->fgResizeSmallPic=FALSE;
            prFbmSeqHdr->u4YAddrOffset   = 0;
            prFbmSeqHdr->u4CAddrOffset   = 0;
            prFbmSeqHdr->fgRasterOrder   = 0;
            prFbmSeqHdr->fgProgressiveSeq = TRUE;
            prFbmSeqHdr->ucAspRatInf         = 1;        // 16:9
            prFbmSeqHdr->ucFrmRatCod         = 5;        // 30 fps
            prFbmSeqHdr->u2HSize             = u4Width;
            prFbmSeqHdr->u2VSize             = u4Height;
            prFbmSeqHdr->u2OrgHSize          = u4Width;
            prFbmSeqHdr->u2OrgVSize          = u4Height;
            prFbmSeqHdr->u2LineSize          = (UINT16)(u4SBWidth * 64);

            prFbmSeqHdr->ucBlockType = 1;// 1 for 16x32, 0 for 64x32
            prFbmSeqHdr->eSwapMode = FBM_SM_NO_SWAP;
            
            if(prUnCompressHdr->u4BitDepth == BITS_8)
            {
                prFbmSeqHdr->u1Depth = 8;
                prVdecEsInfo->u4BitDepth = 8;
                prFbmSeqHdr->ePackMode = FBM_PM_NORMAL;
            }
            else
            {
                prFbmSeqHdr->u1Depth = 10;
                prVdecEsInfo->u4BitDepth = 10;
                prFbmSeqHdr->ePackMode = FBM_PM_COMPACT;
            }
            
            prFbmSeqHdr->fgUfoEn = prCommon->fgUFOModeEnable;
                        
            if(prVdecEsInfo->rMMSeqInfo.e_src_asp!=VDEC_ASPECT_RATIO_NONE 
                &&prVdecEsInfo->rMMSeqInfo.e_src_asp<VDEC_ASPECT_RATIO_MAX)
            {
                prFbmSeqHdr->ucAspRatInf = prVdecEsInfo->rMMSeqInfo.e_src_asp;
            }
            else
            {
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_16_9;        // 16:9
            }

            if(!prVdecEsInfo->u4FrameRate)
            {
                switch (prVdecEsInfo->rMMSeqInfo.e_frame_rate)
                {
                case VDEC_SRC_FRAME_RATE_1:
                    prVdecEsInfo->u4FrameRate = 100;
                    break;
                case VDEC_SRC_FRAME_RATE_5:
                    prVdecEsInfo->u4FrameRate = 500;
                    break;
                case VDEC_SRC_FRAME_RATE_8:
                    prVdecEsInfo->u4FrameRate = 800;
                    break;
                case VDEC_SRC_FRAME_RATE_10:
                    prVdecEsInfo->u4FrameRate = 1000;
                    break;
                case VDEC_SRC_FRAME_RATE_12:                    
                    prVdecEsInfo->u4FrameRate = 1200;
                    break;
                case VDEC_SRC_FRAME_RATE_15:
                    prVdecEsInfo->u4FrameRate = 1500;
                    break;
                case VDEC_SRC_FRAME_RATE_2:
                    prVdecEsInfo->u4FrameRate = 200;
                    break;
                case VDEC_SRC_FRAME_RATE_6:
                    prVdecEsInfo->u4FrameRate = 600;
                    break;
                case VDEC_SRC_FRAME_RATE_48:
                    prVdecEsInfo->u4FrameRate = 4800;
                    break;
                case VDEC_SRC_FRAME_RATE_70:
                    prVdecEsInfo->u4FrameRate = 7000;
                    break;
                case VDEC_SRC_FRAME_RATE_20:
                    prVdecEsInfo->u4FrameRate = 2000;
                    break;
                case VDEC_SRC_FRAME_RATE_23_976:
                    prVdecEsInfo->u4FrameRate = 2397;
                    break;
                case VDEC_SRC_FRAME_RATE_24:
                    prVdecEsInfo->u4FrameRate = 2400;
                    break;
                case VDEC_SRC_FRAME_RATE_25:
                    prVdecEsInfo->u4FrameRate = 2500;
                    break;
                case VDEC_SRC_FRAME_RATE_29_97:
                    prVdecEsInfo->u4FrameRate = 2997;
                    break;
                case VDEC_SRC_FRAME_RATE_30:
                    prVdecEsInfo->u4FrameRate = 3000;
                    break;
                case VDEC_SRC_FRAME_RATE_50:
                    prVdecEsInfo->u4FrameRate = 5000;
                    break;
                case VDEC_SRC_FRAME_RATE_59_94:
                    prVdecEsInfo->u4FrameRate = 5994;
                    break;
                case VDEC_SRC_FRAME_RATE_60:
                    prVdecEsInfo->u4FrameRate = 6000;
                    break;
                case VDEC_SRC_FRAME_RATE_120:
                    prVdecEsInfo->u4FrameRate = 12000;
                    break;
                default:
                    prVdecEsInfo->u4FrameRate = 3000;
                    break;
                }
            }

            u4fps = (prVdecEsInfo->u4FrameRate + 99) / 100;

            switch (u4fps)
            {
                case 15:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_15;
                    break;
                case 22:
                case 23:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24_;
                    break;
                case 24:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24;
                    break;
                case 25:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_25;
                    break;
                case 30:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30;
                    break;
                case 29:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30_;
                    break;
                case 50:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_50;
                    break;
                case 60:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60;
                    break;
                case 59:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60_;
                    break;
                default:
                    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24;
                    break;
            }
            prCommon->fgSeqChg = TRUE;
        }
    }
    
    prCommon->u4NewFbIdx = iVP9GetFreeFb(prCommon);
    pNewFB = prCommon->pCurrentFB = &(prCommon->FRAME_BUFS[prCommon->u4NewFbIdx]);
    if(!prCommon->fgLAEThread)
    {
        FBM_GetFrameBufferAddr(prCommon->ucFbgId, pNewFB->rBuf.u4FbId, &(pNewFB->rBuf.rBufY.u4BufPAddr), &(pNewFB->rBuf.rBufC.u4BufPAddr));
        if(prCommon->fgUFOModeEnable)
        {
            FBM_GetFrameBufferAddrExt(prCommon->ucFbgId, pNewFB->rBuf.u4FbId, &(pNewFB->rBuf.rUFO_LEN_Y.u4BufPAddr), &(pNewFB->rBuf.rUFO_LEN_C.u4BufPAddr));
        }
    }

    if (prUnCompressHdr->u4Width != u4Width || prUnCompressHdr->u4Height != u4Height)
    {
        UINT32 u4Aligned_width;
        UINT32 u4Aligned_height;
        prUnCompressHdr->u4Width = u4Width;
        prUnCompressHdr->u4Height = u4Height;
        u4Aligned_width = ALIGN_POWER_OF_TWO(prUnCompressHdr->u4Width, MI_SIZE_LOG2);
        u4Aligned_height = ALIGN_POWER_OF_TWO(prUnCompressHdr->u4Height, MI_SIZE_LOG2);
        vVP9_Set_MB_MI(prCommon, u4Aligned_width, u4Aligned_height);
        prUnCompressHdr->fgResetSeg = TRUE;
        // leon tmp
        prUnCompressHdr->u4MiCols = prCommon->u4MI_cols;
#if 0
        setup_mi(cm);
        
        // Initialize the previous frame segment map to 0.
        if (cm->last_frame_seg_map)
          vpx_memset(cm->last_frame_seg_map, 0, cm->mi_rows * cm->mi_cols);
#endif
        //vp9_update_frame_size(prCommon);
    }
    
    {
        VP9_FB_INFO_T *prFrameInfo = &(pNewFB->rBuf);
        prFrameInfo->u4YCropWidth = prUnCompressHdr->u4Width;
        prFrameInfo->u4YCropHeight = prUnCompressHdr->u4Height;
        prFrameInfo->u4YStride = VP9_ALIGN_INC(prUnCompressHdr->u4Width, 64);
    }

    return VP9_OK;
}

void vVP9SetupDisplaySize(UINT32 u4CoreId, VP9_COMMON_T * prCommon) {
    prCommon->u4DisplayWidth = prCommon->rUnCompressedHeader.u4Width;
    prCommon->u4DiaplayHeight = prCommon->rUnCompressedHeader.u4Height;
    if (u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId))
    {
        prCommon->u4DisplayWidth= u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 16) + 1;
        prCommon->u4DiaplayHeight = u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 16) + 1;
        if(prCommon->rUnCompressedHeader.u4Width != prCommon->u4DisplayWidth || prCommon->rUnCompressedHeader.u4Height != prCommon->u4DiaplayHeight)
            VP9_PRINT_INFO(prCommon->ucEsId, "frame size[%d x %d] != display size[%d x %d]", prCommon->rUnCompressedHeader.u4Width, prCommon->rUnCompressedHeader.u4Height, prCommon->u4DisplayWidth, prCommon->u4DiaplayHeight);
    }  
}

INT32 i4VP9SetupFrameSize(UINT32 u4CoreId, VP9_COMMON_T * prCommon) 
{
    UINT32 u4CurWidth = u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 16) + 1;
    UINT32 u4CurHeight = u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 16) + 1;
    if(VP9_FAIL == iVP9ApplyFrameSize(prCommon, u4CurWidth, u4CurHeight))
        return VP9_FAIL;
    vVP9SetupDisplaySize(u4CoreId, prCommon);    
    return VP9_OK;
}

INT32 i4VP9SetupFrameSizeWithRefs(UINT32 u4CoreId, VP9_COMMON_T * prCommon) {
    UINT32 u4Width, u4Height;
    UINT32 u4Found = 0, i;

    for (i = 0; i < REFS_PER_FRAME; ++i) {
        if (u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId)) {
            u4Width = prCommon->FRAME_REFS[i].prBuf->u4YCropWidth;
            u4Height = prCommon->FRAME_REFS[i].prBuf->u4YCropHeight;
            u4Found = 1;
            break;
        }
    }

    if (!u4Found)
    {
        u4Width= u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 16) + 1;
        u4Height = u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 16) + 1;
    }

    if (u4Width <= 0 || u4Height <= 0)
    {
        VP9_PRINT_ERROR(prCommon->ucEsId, "referenced frame with invalid size [%d x %d]", u4Width, u4Height);
        return VP9_FAIL;
    }
    
    if(VP9_FAIL == iVP9ApplyFrameSize(prCommon, u4Width, u4Height))
        return VP9_FAIL;

    vVP9SetupDisplaySize(u4CoreId, prCommon);
    return VP9_OK;
}

void vVP9SetupLoopFilter(UINT32 u4CoreId, VP9_LOOP_FILTER_INFO_T* prLoopFilter) 
{    
    prLoopFilter->filter_level = u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 6);
    prLoopFilter->sharpness_level = u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 3);

    // Read in loop filter deltas applied at the MB level based on mode or ref frame.
    prLoopFilter->mode_ref_delta_update = 0;
    prLoopFilter->mode_ref_delta_enabled = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);

    if(prLoopFilter->mode_ref_delta_enabled) 
    {    
       prLoopFilter->mode_ref_delta_update = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);
        
        if(prLoopFilter->mode_ref_delta_update) 
        {
            UINT32 i;

            for (i = 0; i < MAX_REF_LF_DELTAS; i++)
            {                
                if (u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId))
                {
                   prLoopFilter->ref_deltas[i] = i4VDEC_HAL_VP9_Read_Signed_Literal_Raw(u4CoreId, 6);
                }
            }

            for (i = 0; i < MAX_MODE_LF_DELTAS; i++)
            {                
                if (u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId))
                {
                   prLoopFilter->mode_deltas[i] = i4VDEC_HAL_VP9_Read_Signed_Literal_Raw(u4CoreId, 6);
                }
            }
        }
    }
}

INT32 i4VP9DecodeUnsignedMax(UINT32 u4CoreId, INT32 i4Max) 
{
    //VP9_PRINT_INFO("i4VP9DecodeUnsignedMax Shift bit =  %d", (i4Max > 0 ? get_msb(i4Max) + 1 : 0));
    // possible shift 0 bit here.
    INT32 i4Ret = u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, (i4Max > 0 ? get_msb(i4Max) + 1 : 0));
    return i4Ret > i4Max ? i4Max : i4Ret;
}


INT32 i4VP9Read_Delta_Q(UINT32 u4CoreId, INT32 *pDelta_Q) {
    INT32 u4Old = *pDelta_Q;
    *pDelta_Q = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId) ? i4VDEC_HAL_VP9_Read_Signed_Literal_Raw(u4CoreId, 4) : 0;
    return u4Old != *pDelta_Q;
}

#if 0
void vVP9SwapMiAndPrevMi(VP9_COMMON_T* prCommon) {
  // Swap indices.
    const int tmp = cm->mi_idx;
    cm->mi_idx = cm->prev_mi_idx;
    cm->prev_mi_idx = tmp;
    // Current mip will be the prev_mip for the next frame.
    cm->mip = cm->mip_array[cm->mi_idx];
    cm->prev_mip = cm->mip_array[cm->prev_mi_idx];
    cm->mi_grid_base = cm->mi_grid_base_array[cm->mi_idx];
    cm->prev_mi_grid_base = cm->mi_grid_base_array[cm->prev_mi_idx];

    // Update the upper left visible macroblock ptrs.
    cm->mi = cm->mip + cm->mi_stride + 1;
    cm->prev_mi = cm->prev_mip + cm->mi_stride + 1;
    cm->mi_grid_visible = cm->mi_grid_base + cm->mi_stride + 1;
    cm->prev_mi_grid_visible = cm->prev_mi_grid_base + cm->mi_stride + 1;
}
#endif

void vVP9SetupPastIndependence(VP9_COMMON_T * prCommon)
{
    UINT32 i;    
    VP9_UNCOMPRESSED_HEADER_T* prUnCompressed = &prCommon->rUnCompressedHeader;
    x_memset(prUnCompressed->seg.feature_data, 0, sizeof(prUnCompressed->seg.feature_data));    
    x_memset(prUnCompressed->seg.feature_mask, 0, sizeof(prUnCompressed->seg.feature_mask));
    prUnCompressed->seg.abs_delta = SEGMENT_DELTADATA;
    prUnCompressed->fgResetSeg = TRUE;

    #if 0
    if (prCommon->last_frame_seg_map)
        vpx_memset(cm->last_frame_seg_map, 0, (cm->mi_rows * cm->mi_cols));
    #endif
  
    // Reset the mode ref deltas for loop filter
    x_memset(prUnCompressed->rLoopFilter.last_ref_deltas, 0, sizeof(prUnCompressed->rLoopFilter.last_ref_deltas));    
    x_memset(prUnCompressed->rLoopFilter.last_mode_deltas, 0, sizeof(prUnCompressed->rLoopFilter.last_mode_deltas));

    vVP9_Set_Default_LF_Deltas(&prUnCompressed->rLoopFilter);
    
    // To force update of the sharpness
    prUnCompressed->rLoopFilter.last_sharpness_level = -1;

    // initialize coeff prob table to default
    x_memcpy(prCommon->fc.coef_probs[TX_4X4], default_coef_probs_4x4, sizeof(default_coef_probs_4x4));
    x_memcpy(prCommon->fc.coef_probs[TX_8X8], default_coef_probs_8x8, sizeof(default_coef_probs_8x8));
    x_memcpy(prCommon->fc.coef_probs[TX_16X16], default_coef_probs_16x16, sizeof(default_coef_probs_16x16));
    x_memcpy(prCommon->fc.coef_probs[TX_32X32], default_coef_probs_32x32, sizeof(default_coef_probs_32x32));

    //initialize mode probs to default
    x_memcpy(prCommon->fc.uv_mode_prob, default_if_uv_probs, sizeof(default_if_uv_probs));
    x_memcpy(prCommon->fc.y_mode_prob, default_if_y_probs, sizeof(default_if_y_probs));
    x_memcpy(prCommon->fc.switchable_interp_prob, default_switchable_interp_prob, sizeof(default_switchable_interp_prob));
    x_memcpy(prCommon->fc.partition_prob, default_partition_probs, sizeof(default_partition_probs));
    x_memcpy(prCommon->fc.intra_inter_prob, default_intra_inter_p, sizeof(default_intra_inter_p));
    x_memcpy(prCommon->fc.comp_inter_prob, default_comp_inter_p, sizeof(default_comp_inter_p));
    x_memcpy(prCommon->fc.comp_ref_prob, default_comp_ref_p, sizeof(default_comp_ref_p));
    x_memcpy(prCommon->fc.single_ref_prob, default_single_ref_p, sizeof(default_single_ref_p));
    prCommon->fc.tx_probs = default_tx_probs;
    x_memcpy(prCommon->fc.skip_probs, default_skip_probs, sizeof(default_skip_probs));
    x_memcpy(prCommon->fc.inter_mode_probs, default_inter_mode_probs, sizeof(default_inter_mode_probs));

    //initialize mv probs to default
    prCommon->fc.nmvc = default_nmv_context;


    if (prUnCompressed->u4FrameType == KEY_FRAME ||
        prUnCompressed->u4ErrResilenceMode || prUnCompressed->u4ResetFrameContext == 3) {
        // Reset all frame contexts.
        for (i = 0; i < FRAME_CONTEXTS; ++i)
            prCommon->frame_contexts[i] = prCommon->fc;
    } else if (prUnCompressed->u4ResetFrameContext == 2) {
        // Reset only the frame context specified in the frame header.
        prCommon->frame_contexts[prUnCompressed->u4FrameContextIdx] = prCommon->fc;
    }
    
    x_memset(prCommon->REF_FRAME_SIGN_BIAS, 0, sizeof(prCommon->REF_FRAME_SIGN_BIAS));
    prUnCompressed->u4FrameContextIdx = 0;
}

void vVP9SetupQuantization(VP9_COMMON_T * prCommon, VP9_UNCOMPRESSED_HEADER_T* prUnCompressed, MACROBLOCKD* prMBD, UINT32 u4CoreId) 
{
    INT32 u4Update = 0;    
    prUnCompressed->u4BaseQIdx = u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, QINDEX_BITS);    
    u4Update |= i4VP9Read_Delta_Q(u4CoreId, &prUnCompressed->u4Y_DC_DELTA_Q);
    u4Update |= i4VP9Read_Delta_Q(u4CoreId, &prUnCompressed->u4C_DC_DELTA_Q);
    u4Update |= i4VP9Read_Delta_Q(u4CoreId, &prUnCompressed->u4C_AC_DELTA_Q);
    
    if (u4Update)
    {
        UINT32 q;
        for (q = 0; q < QINDEX_RANGE; q++) 
        {
            prCommon->au4Y_Dequant[q][0] = vp9_dc_quant(q, prCommon->rUnCompressedHeader.u4Y_DC_DELTA_Q);
            prCommon->au4Y_Dequant[q][1] = vp9_ac_quant(q, 0);
            prCommon->au4UV_Dequant[q][0] = vp9_dc_quant(q, prCommon->rUnCompressedHeader.u4C_DC_DELTA_Q);
            prCommon->au4UV_Dequant[q][1] = vp9_ac_quant(q, prCommon->rUnCompressedHeader.u4C_AC_DELTA_Q);
        }
    }
    
    prUnCompressed->u4Lossless = prUnCompressed->u4BaseQIdx == 0 &&
                 prUnCompressed->u4Y_DC_DELTA_Q == 0 &&
                 prUnCompressed->u4C_DC_DELTA_Q == 0 &&
                 prUnCompressed->u4C_AC_DELTA_Q == 0;

    //    prMBD->itxm_add = prMBD->lossless ? vp9_iwht4x4_add : vp9_idct4x4_add;
}

void vVP9SetupSegmentation(SEGMENTATION *seg, UINT32 u4CoreId) 
{    
    int i, j;    
    seg->update_map = 0;
    seg->update_data = 0;

    seg->enabled = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);
    if (!seg->enabled)
    {    
        return;
    }    
    // Segmentation map update
    seg->update_map = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);
    if (seg->update_map) {
        for (i = 0; i < SEG_TREE_PROBS; i++)
            seg->tree_probs[i] = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId) ? u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 8)
                                                   : MAX_PROB;

        seg->temporal_update = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);
        if (seg->temporal_update) {
            for (i = 0; i < PREDICTION_PROBS; i++)
                seg->pred_probs[i] = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId) ? u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 8)
                                                     : MAX_PROB;
        } else {
            for (i = 0; i < PREDICTION_PROBS; i++)
                seg->pred_probs[i] = MAX_PROB;
        }
    }
    
    //--------------------------------
    if((!vp9_segfeature_active(seg,0,SEG_LVL_ALT_LF)) &&
       (!vp9_segfeature_active(seg,1,SEG_LVL_ALT_LF)) &&
       (!vp9_segfeature_active(seg,2,SEG_LVL_ALT_LF)) &&
       (!vp9_segfeature_active(seg,3,SEG_LVL_ALT_LF)) &&
       (!vp9_segfeature_active(seg,4,SEG_LVL_ALT_LF)) &&
       (!vp9_segfeature_active(seg,5,SEG_LVL_ALT_LF)) &&
       (!vp9_segfeature_active(seg,6,SEG_LVL_ALT_LF)) &&
       (!vp9_segfeature_active(seg,7,SEG_LVL_ALT_LF)))
    {
      seg->u4SegCtr = 0;
    }
    //--------------------------------

  // Segmentation data update
    seg->update_data = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);
    if (seg->update_data) 
    {
        seg->abs_delta = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);
        //--------------------------------
        seg->u4SegCtr  = 0;
        seg->u4SegCtr  = (seg->abs_delta & 0x1) << 8;
        seg->u4SegFeature_0_3 = 0;
        seg->u4SegFeature_4_7 = 0;
        //--------------------------------

        x_memset(seg->feature_data, 0, sizeof(seg->feature_data));    
        x_memset(seg->feature_mask, 0, sizeof(seg->feature_mask));

        for (i = 0; i < MAX_SEGMENTS; i++) 
        {
            for (j = 0; j < SEG_LVL_MAX; j++) 
            {
                INT32 data = 0;
                UINT32 feature_enabled = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);
                
                if (feature_enabled) 
                {
                    //vp9 enable segfeature
                    seg->feature_mask[i] |= 1 << j;
                    data =  i4VP9DecodeUnsignedMax(u4CoreId, seg_feature_data_max[j]);
                    if (seg_feature_data_signed[j])
                        data = ((INT32)u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId)) ? -data : data;
                }
                
                ASSERT(data <= seg_feature_data_max[j]);
                if (data < 0) 
                {
                    ASSERT(seg_feature_data_signed[j]);
                    ASSERT(-data <= seg_feature_data_max[j]);
                }
                seg->feature_data[i][j] = data;
                
                //--------------------------------
                if(j == SEG_LVL_ALT_LF)
                {
                    seg->u4SegCtr |= feature_enabled << i;
            
                    if(i < 4)
                        seg->u4SegFeature_0_3 |= ((UINT32)(data & 0xff)) << i*8;
                    else
                        seg->u4SegFeature_4_7 |= ((UINT32)(data & 0xff)) << (i-4)*8;
                }
                //--------------------------------
            }
        }
    }
}

void vVP9SetupTileInfo(VP9_UNCOMPRESSED_HEADER_T * prUnCompressed, UINT32 u4CoreId)
{
    int min_log2_tile_cols, max_log2_tile_cols, max_ones;
    vp9_get_tile_n_bits(prUnCompressed->u4MiCols, &min_log2_tile_cols, &max_log2_tile_cols);
   
    // columns
    max_ones = max_log2_tile_cols - min_log2_tile_cols;
    prUnCompressed->u4Log2TileCols = min_log2_tile_cols;
    while (max_ones-- && u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId))
        prUnCompressed->u4Log2TileCols++;

    // rows
    prUnCompressed->u4Log2TileRows = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);
    if (prUnCompressed->u4Log2TileRows)
        prUnCompressed->u4Log2TileRows += u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);
}

void vVP9SetupCompoundRefMode(VP9_COMMON_T *prCommon) 
{
    if (prCommon->REF_FRAME_SIGN_BIAS[LAST_FRAME] == prCommon->REF_FRAME_SIGN_BIAS[GOLDEN_FRAME]) 
    {
        prCommon->COMP_FIXED_REF = ALTREF_FRAME;
        prCommon->COMP_VAR_REF[0] = LAST_FRAME;
        prCommon->COMP_VAR_REF[1] = GOLDEN_FRAME;
    }
    else if (prCommon->REF_FRAME_SIGN_BIAS[LAST_FRAME] == prCommon->REF_FRAME_SIGN_BIAS[ALTREF_FRAME])
    {
        prCommon->COMP_FIXED_REF = GOLDEN_FRAME;
        prCommon->COMP_VAR_REF[0] = LAST_FRAME;
        prCommon->COMP_VAR_REF[1] = ALTREF_FRAME;
    } 
    else 
    {
        prCommon->COMP_FIXED_REF = LAST_FRAME;
        prCommon->COMP_VAR_REF[0] = GOLDEN_FRAME;
        prCommon->COMP_VAR_REF[1] = ALTREF_FRAME;
    }
}

void vVP9AdaptCoefProbs(VP9_COMMON_T *prCommon) {
    TX_SIZE t;
    unsigned int count_sat, update_factor;
    if (fgVP9IntraOnly(prCommon)) {
        update_factor = COEF_MAX_UPDATE_FACTOR_KEY;
        count_sat = COEF_COUNT_SAT_KEY;
    } else if (prCommon->u4LastFrameType == KEY_FRAME) {
        update_factor = COEF_MAX_UPDATE_FACTOR_AFTER_KEY;  /* adapt quickly */
        count_sat = COEF_COUNT_SAT_AFTER_KEY;
    } else {
        update_factor = COEF_MAX_UPDATE_FACTOR;
        count_sat = COEF_COUNT_SAT;
    }
    for (t = TX_4X4; t <= TX_32X32; t++)
        adapt_coef_probs(prCommon, t, count_sat, update_factor);
}


void vVP9AdaptModeProbs(VP9_COMMON_T *prCommon) {
  int i, j;
  FRAME_CONTEXT *fc = &prCommon->fc;
  const FRAME_CONTEXT *pre_fc = &prCommon->frame_contexts[prCommon->rUnCompressedHeader.u4FrameContextIdx];
  const FRAME_COUNTS *counts = &prCommon->counts;
  for (i = 0; i < INTRA_INTER_CONTEXTS; i++)
    fc->intra_inter_prob[i] = adapt_prob(pre_fc->intra_inter_prob[i],
                                         (unsigned int*)counts->intra_inter[i]);
  for (i = 0; i < COMP_INTER_CONTEXTS; i++)
    fc->comp_inter_prob[i] = adapt_prob(pre_fc->comp_inter_prob[i],
                                        (unsigned int*)counts->comp_inter[i]);
  for (i = 0; i < REF_CONTEXTS; i++)
    fc->comp_ref_prob[i] = adapt_prob(pre_fc->comp_ref_prob[i],
                                      (unsigned int*)counts->comp_ref[i]);
  for (i = 0; i < REF_CONTEXTS; i++)
    for (j = 0; j < 2; j++)
      fc->single_ref_prob[i][j] = adapt_prob(pre_fc->single_ref_prob[i][j],
                                             (unsigned int*)counts->single_ref[i][j]);

  for (i = 0; i < INTER_MODE_CONTEXTS; i++)
    adapt_probs(vp9_inter_mode_tree, pre_fc->inter_mode_probs[i],
                (unsigned int*)counts->inter_mode[i], fc->inter_mode_probs[i]);

  for (i = 0; i < BLOCK_SIZE_GROUPS; i++)
    adapt_probs(vp9_intra_mode_tree, pre_fc->y_mode_prob[i],
                (unsigned int*)counts->y_mode[i], fc->y_mode_prob[i]);

  for (i = 0; i < INTRA_MODES; ++i)
    adapt_probs(vp9_intra_mode_tree, pre_fc->uv_mode_prob[i],
                (unsigned int*)counts->uv_mode[i], fc->uv_mode_prob[i]);

  for (i = 0; i < PARTITION_CONTEXTS; i++)
    adapt_probs(vp9_partition_tree, pre_fc->partition_prob[i],
                (unsigned int*)counts->partition[i], fc->partition_prob[i]);

  if (prCommon->rUnCompressedHeader.eInterpFilterType == SWITCHABLE) {
    for (i = 0; i < SWITCHABLE_FILTER_CONTEXTS; i++)
      adapt_probs(vp9_switchable_interp_tree, pre_fc->switchable_interp_prob[i],
                  (unsigned int*)counts->switchable_interp[i], fc->switchable_interp_prob[i]);
  }

  if (prCommon->eTxMode == TX_MODE_SELECT) {
    int j;
    unsigned int branch_ct_8x8p[TX_SIZES - 3][2];
    unsigned int branch_ct_16x16p[TX_SIZES - 2][2];
    unsigned int branch_ct_32x32p[TX_SIZES - 1][2];

    for (i = 0; i < TX_SIZE_CONTEXTS; ++i) {
      tx_counts_to_branch_counts_8x8(counts->tx.p8x8[i], branch_ct_8x8p);
      for (j = 0; j < TX_SIZES - 3; ++j)
        fc->tx_probs.p8x8[i][j] = adapt_prob(pre_fc->tx_probs.p8x8[i][j],
                                             branch_ct_8x8p[j]);

      tx_counts_to_branch_counts_16x16(counts->tx.p16x16[i], branch_ct_16x16p);
      for (j = 0; j < TX_SIZES - 2; ++j)
        fc->tx_probs.p16x16[i][j] = adapt_prob(pre_fc->tx_probs.p16x16[i][j],
                                               branch_ct_16x16p[j]);

      tx_counts_to_branch_counts_32x32(counts->tx.p32x32[i], branch_ct_32x32p);
      for (j = 0; j < TX_SIZES - 1; ++j)
        fc->tx_probs.p32x32[i][j] = adapt_prob(pre_fc->tx_probs.p32x32[i][j],
                                               branch_ct_32x32p[j]);
    }
  }

  for (i = 0; i < SKIP_CONTEXTS; ++i)
    fc->skip_probs[i] = adapt_prob(pre_fc->skip_probs[i], (unsigned int *)counts->skip[i]);
}

void vVP9AdaptMvProbs(VP9_COMMON_T *prCommon, int allow_hp) {
  int i, j;

  nmv_context *fc = &prCommon->fc.nmvc;
  const nmv_context *pre_fc = &prCommon->frame_contexts[prCommon->rUnCompressedHeader.u4FrameContextIdx].nmvc;
  const nmv_context_counts *counts = &prCommon->counts.mv;
  adapt_probs(vp9_mv_joint_tree, pre_fc->joints, counts->joints, fc->joints);

  for (i = 0; i < 2; ++i) {
    nmv_component *comp = &fc->comps[i];
    const nmv_component *pre_comp = &pre_fc->comps[i];
    const nmv_component_counts *c = &counts->comps[i];

    comp->sign = adapt_prob(pre_comp->sign, c->sign);
    adapt_probs(vp9_mv_class_tree, pre_comp->classes, c->classes,
                comp->classes);
    adapt_probs(vp9_mv_class0_tree, pre_comp->class0, c->class0, comp->class0);

    for (j = 0; j < MV_OFFSET_BITS; ++j)
      comp->bits[j] = adapt_prob(pre_comp->bits[j], c->bits[j]);

    for (j = 0; j < CLASS0_SIZE; ++j)
      adapt_probs(vp9_mv_fp_tree, pre_comp->class0_fp[j], c->class0_fp[j],
                  comp->class0_fp[j]);

    adapt_probs(vp9_mv_fp_tree, pre_comp->fp, c->fp, comp->fp);

    if (allow_hp) {
      comp->class0_hp = adapt_prob(pre_comp->class0_hp, c->class0_hp);
      comp->hp = adapt_prob(pre_comp->hp, c->hp);
    }
  }
}

INT32 iVP9GetFreeFb(VP9_COMMON_T * prCommon) 
{
    UINT32 i;
    UINT32 u4FreeFbId = FBM_FB_ID_UNKNOWN;
    for (i = 0; i < FRAME_BUFFERS; i++)
    {
        if (prCommon->FRAME_BUFS[i].u4RefCount == 0)
          break;
    }
    
    if(i == FRAME_BUFFERS)
    {
        VP9_PRINT_ERROR(prCommon->ucEsId, "no free slot");
        return -1;
    }

    if(!prCommon->fgLAEThread)
    {
        u4FreeFbId = FBM_GetEmptyFrameBuffer(prCommon->ucFbgId, 500000);
        if(FBM_FB_ID_UNKNOWN == u4FreeFbId)
        {
            VP9_PRINT_ERROR(prCommon->ucEsId, "get empty frame failed");
            return -1;
        }
        VP9_PRINT_VERBOSE(prCommon->ucEsId, "out (%d, %d) ->decode", prCommon->ucFbgId, u4FreeFbId);
        FBM_UpdateReferenceList(prCommon->ucFbgId, u4FreeFbId, TRUE);
    }

    prCommon->FRAME_BUFS[i].u4RefCount = 1;
    prCommon->FRAME_BUFS[i].rBuf.u4FbId = u4FreeFbId;
    prCommon->FRAME_BUFS[i].rBuf.u4FbgId = prCommon->ucFbgId;
    return i;
}

void vVP9RefCntFb(VP9_COMMON_T* prCommon, int *idx, int new_idx, BOOL bFbmCtrl) {
    VP9_REF_CNT_BUF_T *bufs = prCommon->FRAME_BUFS;
    const int ref_index = *idx;

    if (ref_index >= 0 && bufs[ref_index].u4RefCount > 0)
    {
        bufs[ref_index].u4RefCount--;
        if(bFbmCtrl && (0 == bufs[ref_index].u4RefCount))
        {
            VP9_PRINT_VERBOSE(prCommon->ucEsId, "out (%d, %d) ->free", bufs[ref_index].rBuf.u4FbgId, bufs[ref_index].rBuf.u4FbId);
            FBM_UpdateReferenceList(bufs[ref_index].rBuf.u4FbgId, bufs[ref_index].rBuf.u4FbId, FALSE);
        }
    }

    if(new_idx >= 0)
    {
        *idx = new_idx;
        bufs[new_idx].u4RefCount++;
    }
}


/* If any buffer updating is signaled it should be done here. */
void vVP9SwapFrameBuffers(VP9_COMMON_T* prCommon) 
{
    VP9_UNCOMPRESSED_HEADER_T* prUnCompress = &prCommon->rUnCompressedHeader;
    INT32 ref_index = 0, mask;
    UINT32 u4PureDecodeTime;
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(prCommon->ucEsId);
    for (mask = prUnCompress->u4RefreshFrameFlags; mask; mask >>= 1)
    {
        if (mask & 1) 
        {
            //const int old_idx = prCommon->REF_FRAME_MAP[ref_index];
            vVP9RefCntFb(prCommon, (int *)(&prCommon->REF_FRAME_MAP[ref_index]), prCommon->u4NewFbIdx, !prCommon->fgLAEThread);
            
            #if 0
            if (old_idx >= 0 && prCommon->FRAME_BUFS[old_idx].u4RefCount == 0)
            {
                prCommon->release_fb_cb(cm->cb_priv, &prCommon->FRAME_BUFS[old_idx].raw_frame_buffer);
            }
            #endif
        }
        ++ref_index;
    }

    prCommon->prFrameToShow = &prCommon->FRAME_BUFS[prCommon->u4NewFbIdx].rBuf;
    --prCommon->FRAME_BUFS[prCommon->u4NewFbIdx].u4RefCount;
#ifdef LINUX_TURNKEY_SOLUTION
    u4PureDecodeTime = jiffies_to_msecs(jiffies - prCommon->u4JifMark);
#endif
    if(!prCommon->fgLAEThread)
    {
        if(prUnCompress->u4ShowFrame)
        {
            if(!prUnCompress->u4ShowExisting)
            {
                VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prCommon->ucEsId);
                VDEC_PES_INFO_T* pPesInfo = &prVdecEsInfoKeep->rPesInfo;
                FBM_PIC_HDR_T* prFbmPicHdr = FBM_GetFrameBufferPicHdr(prCommon->prFrameToShow->u4FbgId, prCommon->prFrameToShow->u4FbId);

                if(prCommon->fgSeqChg)
                {
                    FBM_SetSeqChgFbId(prCommon->prFrameToShow->u4FbgId, prCommon->prFrameToShow->u4FbId);
                    FBM_SetFrameBufferFlag(prCommon->prFrameToShow->u4FbgId, FBM_FLAG_SEQ_CHG);
                }
                
                prVdecEsInfo->u4DisplayQPicCnt++;
                prFbmPicHdr->u8PTS = pPesInfo->u8PTS;
                prFbmPicHdr->u4PTS= pPesInfo->u4PTS;
                prFbmPicHdr->u4PicWidth = prCommon->prFrameToShow->u4YCropWidth;
                prFbmPicHdr->u4PicWidthPitch = prCommon->prFrameToShow->u4YStride;
                prFbmPicHdr->u4PicHeight = prCommon->prFrameToShow->u4YCropHeight;
                prFbmPicHdr->fgCropping = FALSE;
                prFbmPicHdr->fgUfoEn = prCommon->fgUFOModeEnable;
                FBM_SetFrameBufferStatus(prCommon->prFrameToShow->u4FbgId, prCommon->prFrameToShow->u4FbId, FBM_FB_STATUS_DISPLAYQ);                
                VP9_PRINT_IO(prCommon->ucEsId, "out (%d, %d) ->dispQ, (%dx%d), pts:0x%x, cost:%dms", prCommon->prFrameToShow->u4FbgId, prCommon->prFrameToShow->u4FbId, prFbmPicHdr->u4PicWidth, prFbmPicHdr->u4PicHeight, prFbmPicHdr->u4PTS, u4PureDecodeTime);
                if(prVdecEsInfoKeep->fgPatternMode)
                {
                    UCHAR ucFbIdToShow = FBM_GetDisplayFB(prCommon->prFrameToShow->u4FbgId);
                    VERIFY(prCommon->prFrameToShow->u4FbId == ucFbIdToShow);
                    FBM_SetFrameBufferStatus(prCommon->prFrameToShow->u4FbgId, prCommon->prFrameToShow->u4FbId, FBM_FB_STATUS_EMPTY);
                }
                
                if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I))
                {
                    CLR_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
                }
            }
            else
            {
                // TODO: repeat existing frame to b2r
                VP9_PRINT_IO(prCommon->ucEsId, "out ->show existing, idx:%d, cost:%ms", prUnCompress->u4FrameToShow, u4PureDecodeTime);                
            }
        }
        else
        {
            FBM_SetFrameBufferStatus(prCommon->prFrameToShow->u4FbgId, prCommon->prFrameToShow->u4FbId, FBM_FB_STATUS_EMPTY);
        }
        
        if(0 == prCommon->FRAME_BUFS[prCommon->u4NewFbIdx].u4RefCount)
        {
            VP9_PRINT_VERBOSE(prCommon->ucEsId, "out (%d, %d) ->free", prCommon->prFrameToShow->u4FbgId, prCommon->prFrameToShow->u4FbId);
            FBM_UpdateReferenceList(prCommon->FRAME_BUFS[prCommon->u4NewFbIdx].rBuf.u4FbgId, prCommon->FRAME_BUFS[prCommon->u4NewFbIdx].rBuf.u4FbId, FALSE);
        }
    }

    if(u4PureDecodeTime > prCommon->u4MaxDecodeTime)
        prCommon->u4MaxDecodeTime = u4PureDecodeTime;
    prCommon->u4TotalDecodeTime += u4PureDecodeTime;
    prCommon->u4AvgDecodeTime = prCommon->u4TotalDecodeTime / prCommon->u4TotalDecoded;
#ifdef LINUX_TURNKEY_SOLUTION
    prCommon->u4JifMark = jiffies;
    prCommon->u4LstOutJif = jiffies;
#endif    
    prCommon->fgSeqChg = FALSE;
        // Invalidate these references until the next frame starts.
    for (ref_index = 0; ref_index < 3; ref_index++)
        prCommon->FRAME_REFS[ref_index].u4Idx = INT_MAX;
}

