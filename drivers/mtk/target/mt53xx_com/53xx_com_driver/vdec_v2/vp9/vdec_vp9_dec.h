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
 * $RCSfile: vdec_vp8dec.h,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_vp8dec.h
 *  This header file include vp8 decode function 
 */

#ifndef VDEC_VP9_DEC_H
#define VDEC_VP9_DEC_H
#include "vdec_info_vp9.h"
#include "vdec_hal_if_vp9.h"

#define ALIGN_POWER_OF_TWO(value, n) \
        (((value) + ((1 << (n)) - 1)) & ~((1 << (n)) - 1))
        
#define MIN_TILE_WIDTH_B64 4
#define MAX_TILE_WIDTH_B64 64

#define COEF_COUNT_SAT 24
#define COEF_MAX_UPDATE_FACTOR 112
#define COEF_COUNT_SAT_KEY 24
#define COEF_MAX_UPDATE_FACTOR_KEY 112
#define COEF_COUNT_SAT_AFTER_KEY 24
#define COEF_MAX_UPDATE_FACTOR_AFTER_KEY 128

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
    
#define ROUND_POWER_OF_TWO(value, n) \
        (((value) + (1 << ((n) - 1))) >> (n))
    
#define ALIGN_POWER_OF_TWO(value, n) \
        (((value) + ((1 << (n)) - 1)) & ~((1 << (n)) - 1))
        
#define COEFF_CONTEXTS 6
#define BAND_COEFF_CONTEXTS(band) ((band) == 0 ? 3 : COEFF_CONTEXTS)
#define EOB_MODEL_TOKEN 3

#define COUNT_SAT 20
#define MAX_UPDATE_FACTOR 128

#if 1
static const vp9_coeff_probs_model default_coef_probs_4x4[PLANE_TYPES] = {
  {  // Y plane
    {  // Intra
      {  // Band 0
        { 195,  29, 183 }, {  84,  49, 136 }, {   8,  42,  71 }
      }, {  // Band 1
        {  31, 107, 169 }, {  35,  99, 159 }, {  17,  82, 140 },
        {   8,  66, 114 }, {   2,  44,  76 }, {   1,  19,  32 }
      }, {  // Band 2
        {  40, 132, 201 }, {  29, 114, 187 }, {  13,  91, 157 },
        {   7,  75, 127 }, {   3,  58,  95 }, {   1,  28,  47 }
      }, {  // Band 3
        {  69, 142, 221 }, {  42, 122, 201 }, {  15,  91, 159 },
        {   6,  67, 121 }, {   1,  42,  77 }, {   1,  17,  31 }
      }, {  // Band 4
        { 102, 148, 228 }, {  67, 117, 204 }, {  17,  82, 154 },
        {   6,  59, 114 }, {   2,  39,  75 }, {   1,  15,  29 }
      }, {  // Band 5
        { 156,  57, 233 }, { 119,  57, 212 }, {  58,  48, 163 },
        {  29,  40, 124 }, {  12,  30,  81 }, {   3,  12,  31 }
      }
    }, {  // Inter
      {  // Band 0
        { 191, 107, 226 }, { 124, 117, 204 }, {  25,  99, 155 }
      }, {  // Band 1
        {  29, 148, 210 }, {  37, 126, 194 }, {   8,  93, 157 },
        {   2,  68, 118 }, {   1,  39,  69 }, {   1,  17,  33 }
      }, {  // Band 2
        {  41, 151, 213 }, {  27, 123, 193 }, {   3,  82, 144 },
        {   1,  58, 105 }, {   1,  32,  60 }, {   1,  13,  26 }
      }, {  // Band 3
        {  59, 159, 220 }, {  23, 126, 198 }, {   4,  88, 151 },
        {   1,  66, 114 }, {   1,  38,  71 }, {   1,  18,  34 }
      }, {  // Band 4
        { 114, 136, 232 }, {  51, 114, 207 }, {  11,  83, 155 },
        {   3,  56, 105 }, {   1,  33,  65 }, {   1,  17,  34 }
      }, {  // Band 5
        { 149,  65, 234 }, { 121,  57, 215 }, {  61,  49, 166 },
        {  28,  36, 114 }, {  12,  25,  76 }, {   3,  16,  42 }
      }
    }
  }, {  // UV plane
    {  // Intra
      {  // Band 0
        { 214,  49, 220 }, { 132,  63, 188 }, {  42,  65, 137 }
      }, {  // Band 1
        {  85, 137, 221 }, { 104, 131, 216 }, {  49, 111, 192 },
        {  21,  87, 155 }, {   2,  49,  87 }, {   1,  16,  28 }
      }, {  // Band 2
        {  89, 163, 230 }, {  90, 137, 220 }, {  29, 100, 183 },
        {  10,  70, 135 }, {   2,  42,  81 }, {   1,  17,  33 }
      }, {  // Band 3
        { 108, 167, 237 }, {  55, 133, 222 }, {  15,  97, 179 },
        {   4,  72, 135 }, {   1,  45,  85 }, {   1,  19,  38 }
      }, {  // Band 4
        { 124, 146, 240 }, {  66, 124, 224 }, {  17,  88, 175 },
        {   4,  58, 122 }, {   1,  36,  75 }, {   1,  18,  37 }
      }, {  //  Band 5
        { 141,  79, 241 }, { 126,  70, 227 }, {  66,  58, 182 },
        {  30,  44, 136 }, {  12,  34,  96 }, {   2,  20,  47 }
      }
    }, {  // Inter
      {  // Band 0
        { 229,  99, 249 }, { 143, 111, 235 }, {  46, 109, 192 }
      }, {  // Band 1
        {  82, 158, 236 }, {  94, 146, 224 }, {  25, 117, 191 },
        {   9,  87, 149 }, {   3,  56,  99 }, {   1,  33,  57 }
      }, {  // Band 2
        {  83, 167, 237 }, {  68, 145, 222 }, {  10, 103, 177 },
        {   2,  72, 131 }, {   1,  41,  79 }, {   1,  20,  39 }
      }, {  // Band 3
        {  99, 167, 239 }, {  47, 141, 224 }, {  10, 104, 178 },
        {   2,  73, 133 }, {   1,  44,  85 }, {   1,  22,  47 }
      }, {  // Band 4
        { 127, 145, 243 }, {  71, 129, 228 }, {  17,  93, 177 },
        {   3,  61, 124 }, {   1,  41,  84 }, {   1,  21,  52 }
      }, {  // Band 5
        { 157,  78, 244 }, { 140,  72, 231 }, {  69,  58, 184 },
        {  31,  44, 137 }, {  14,  38, 105 }, {   8,  23,  61 }
      }
    }
  }
};

static const vp9_coeff_probs_model default_coef_probs_8x8[PLANE_TYPES] = {
  {  // Y plane
    {  // Intra
      {  // Band 0
        { 125,  34, 187 }, {  52,  41, 133 }, {   6,  31,  56 }
      }, {  // Band 1
        {  37, 109, 153 }, {  51, 102, 147 }, {  23,  87, 128 },
        {   8,  67, 101 }, {   1,  41,  63 }, {   1,  19,  29 }
      }, {  // Band 2
        {  31, 154, 185 }, {  17, 127, 175 }, {   6,  96, 145 },
        {   2,  73, 114 }, {   1,  51,  82 }, {   1,  28,  45 }
      }, {  // Band 3
        {  23, 163, 200 }, {  10, 131, 185 }, {   2,  93, 148 },
        {   1,  67, 111 }, {   1,  41,  69 }, {   1,  14,  24 }
      }, {  // Band 4
        {  29, 176, 217 }, {  12, 145, 201 }, {   3, 101, 156 },
        {   1,  69, 111 }, {   1,  39,  63 }, {   1,  14,  23 }
      }, {  // Band 5
        {  57, 192, 233 }, {  25, 154, 215 }, {   6, 109, 167 },
        {   3,  78, 118 }, {   1,  48,  69 }, {   1,  21,  29 }
      }
    }, {  // Inter
      {  // Band 0
        { 202, 105, 245 }, { 108, 106, 216 }, {  18,  90, 144 }
      }, {  // Band 1
        {  33, 172, 219 }, {  64, 149, 206 }, {  14, 117, 177 },
        {   5,  90, 141 }, {   2,  61,  95 }, {   1,  37,  57 }
      }, {  // Band 2
        {  33, 179, 220 }, {  11, 140, 198 }, {   1,  89, 148 },
        {   1,  60, 104 }, {   1,  33,  57 }, {   1,  12,  21 }
      }, {  // Band 3
        {  30, 181, 221 }, {   8, 141, 198 }, {   1,  87, 145 },
        {   1,  58, 100 }, {   1,  31,  55 }, {   1,  12,  20 }
      }, {  // Band 4
        {  32, 186, 224 }, {   7, 142, 198 }, {   1,  86, 143 },
        {   1,  58, 100 }, {   1,  31,  55 }, {   1,  12,  22 }
      }, {  // Band 5
        {  57, 192, 227 }, {  20, 143, 204 }, {   3,  96, 154 },
        {   1,  68, 112 }, {   1,  42,  69 }, {   1,  19,  32 }
      }
    }
  }, {  // UV plane
    {  // Intra
      {  // Band 0
        { 212,  35, 215 }, { 113,  47, 169 }, {  29,  48, 105 }
      }, {  // Band 1
        {  74, 129, 203 }, { 106, 120, 203 }, {  49, 107, 178 },
        {  19,  84, 144 }, {   4,  50,  84 }, {   1,  15,  25 }
      }, {  // Band 2
        {  71, 172, 217 }, {  44, 141, 209 }, {  15, 102, 173 },
        {   6,  76, 133 }, {   2,  51,  89 }, {   1,  24,  42 }
      }, {  // Band 3
        {  64, 185, 231 }, {  31, 148, 216 }, {   8, 103, 175 },
        {   3,  74, 131 }, {   1,  46,  81 }, {   1,  18,  30 }
      }, {  // Band 4
        {  65, 196, 235 }, {  25, 157, 221 }, {   5, 105, 174 },
        {   1,  67, 120 }, {   1,  38,  69 }, {   1,  15,  30 }
      }, {  // Band 5
        {  65, 204, 238 }, {  30, 156, 224 }, {   7, 107, 177 },
        {   2,  70, 124 }, {   1,  42,  73 }, {   1,  18,  34 }
      }
    }, {  // Inter
      {  // Band 0
        { 225,  86, 251 }, { 144, 104, 235 }, {  42,  99, 181 }
      }, {  // Band 1
        {  85, 175, 239 }, { 112, 165, 229 }, {  29, 136, 200 },
        {  12, 103, 162 }, {   6,  77, 123 }, {   2,  53,  84 }
      }, {  // Band 2
        {  75, 183, 239 }, {  30, 155, 221 }, {   3, 106, 171 },
        {   1,  74, 128 }, {   1,  44,  76 }, {   1,  17,  28 }
      }, {  // Band 3
        {  73, 185, 240 }, {  27, 159, 222 }, {   2, 107, 172 },
        {   1,  75, 127 }, {   1,  42,  73 }, {   1,  17,  29 }
      }, {  // Band 4
        {  62, 190, 238 }, {  21, 159, 222 }, {   2, 107, 172 },
        {   1,  72, 122 }, {   1,  40,  71 }, {   1,  18,  32 }
      }, {  // Band 5
        {  61, 199, 240 }, {  27, 161, 226 }, {   4, 113, 180 },
        {   1,  76, 129 }, {   1,  46,  80 }, {   1,  23,  41 }
      }
    }
  }
};

static const vp9_coeff_probs_model default_coef_probs_16x16[PLANE_TYPES] = {
  {  // Y plane
    {  // Intra
      {  // Band 0
        {   7,  27, 153 }, {   5,  30,  95 }, {   1,  16,  30 }
      }, {  // Band 1
        {  50,  75, 127 }, {  57,  75, 124 }, {  27,  67, 108 },
        {  10,  54,  86 }, {   1,  33,  52 }, {   1,  12,  18 }
      }, {  // Band 2
        {  43, 125, 151 }, {  26, 108, 148 }, {   7,  83, 122 },
        {   2,  59,  89 }, {   1,  38,  60 }, {   1,  17,  27 }
      }, {  // Band 3
        {  23, 144, 163 }, {  13, 112, 154 }, {   2,  75, 117 },
        {   1,  50,  81 }, {   1,  31,  51 }, {   1,  14,  23 }
      }, {  // Band 4
        {  18, 162, 185 }, {   6, 123, 171 }, {   1,  78, 125 },
        {   1,  51,  86 }, {   1,  31,  54 }, {   1,  14,  23 }
      }, {  // Band 5
        {  15, 199, 227 }, {   3, 150, 204 }, {   1,  91, 146 },
        {   1,  55,  95 }, {   1,  30,  53 }, {   1,  11,  20 }
      }
    }, {  // Inter
      {  // Band 0
        {  19,  55, 240 }, {  19,  59, 196 }, {   3,  52, 105 }
      }, {  // Band 1
        {  41, 166, 207 }, { 104, 153, 199 }, {  31, 123, 181 },
        {  14, 101, 152 }, {   5,  72, 106 }, {   1,  36,  52 }
      }, {  // Band 2
        {  35, 176, 211 }, {  12, 131, 190 }, {   2,  88, 144 },
        {   1,  60, 101 }, {   1,  36,  60 }, {   1,  16,  28 }
      }, {  // Band 3
        {  28, 183, 213 }, {   8, 134, 191 }, {   1,  86, 142 },
        {   1,  56,  96 }, {   1,  30,  53 }, {   1,  12,  20 }
      }, {  // Band 4
        {  20, 190, 215 }, {   4, 135, 192 }, {   1,  84, 139 },
        {   1,  53,  91 }, {   1,  28,  49 }, {   1,  11,  20 }
      }, {  // Band 5
        {  13, 196, 216 }, {   2, 137, 192 }, {   1,  86, 143 },
        {   1,  57,  99 }, {   1,  32,  56 }, {   1,  13,  24 }
      }
    }
  }, {  // UV plane
    {  // Intra
      {  // Band 0
        { 211,  29, 217 }, {  96,  47, 156 }, {  22,  43,  87 }
      }, {  // Band 1
        {  78, 120, 193 }, { 111, 116, 186 }, {  46, 102, 164 },
        {  15,  80, 128 }, {   2,  49,  76 }, {   1,  18,  28 }
      }, {  // Band 2
        {  71, 161, 203 }, {  42, 132, 192 }, {  10,  98, 150 },
        {   3,  69, 109 }, {   1,  44,  70 }, {   1,  18,  29 }
      }, {  // Band 3
        {  57, 186, 211 }, {  30, 140, 196 }, {   4,  93, 146 },
        {   1,  62, 102 }, {   1,  38,  65 }, {   1,  16,  27 }
      }, {  // Band 4
        {  47, 199, 217 }, {  14, 145, 196 }, {   1,  88, 142 },
        {   1,  57,  98 }, {   1,  36,  62 }, {   1,  15,  26 }
      }, {  // Band 5
        {  26, 219, 229 }, {   5, 155, 207 }, {   1,  94, 151 },
        {   1,  60, 104 }, {   1,  36,  62 }, {   1,  16,  28 }
      }
    }, {  // Inter
      {  // Band 0
        { 233,  29, 248 }, { 146,  47, 220 }, {  43,  52, 140 }
      }, {  // Band 1
        { 100, 163, 232 }, { 179, 161, 222 }, {  63, 142, 204 },
        {  37, 113, 174 }, {  26,  89, 137 }, {  18,  68,  97 }
      }, {  // Band 2
        {  85, 181, 230 }, {  32, 146, 209 }, {   7, 100, 164 },
        {   3,  71, 121 }, {   1,  45,  77 }, {   1,  18,  30 }
      }, {  // Band 3
        {  65, 187, 230 }, {  20, 148, 207 }, {   2,  97, 159 },
        {   1,  68, 116 }, {   1,  40,  70 }, {   1,  14,  29 }
      }, {  // Band 4
        {  40, 194, 227 }, {   8, 147, 204 }, {   1,  94, 155 },
        {   1,  65, 112 }, {   1,  39,  66 }, {   1,  14,  26 }
      }, {  // Band 5
        {  16, 208, 228 }, {   3, 151, 207 }, {   1,  98, 160 },
        {   1,  67, 117 }, {   1,  41,  74 }, {   1,  17,  31 }
      }
    }
  }
};

static const vp9_coeff_probs_model default_coef_probs_32x32[PLANE_TYPES] = {
  {  // Y plane
    {  // Intra
      {  // Band 0
        {  17,  38, 140 }, {   7,  34,  80 }, {   1,  17,  29 }
      }, {  // Band 1
        {  37,  75, 128 }, {  41,  76, 128 }, {  26,  66, 116 },
        {  12,  52,  94 }, {   2,  32,  55 }, {   1,  10,  16 }
      }, {  // Band 2
        {  50, 127, 154 }, {  37, 109, 152 }, {  16,  82, 121 },
        {   5,  59,  85 }, {   1,  35,  54 }, {   1,  13,  20 }
      }, {  // Band 3
        {  40, 142, 167 }, {  17, 110, 157 }, {   2,  71, 112 },
        {   1,  44,  72 }, {   1,  27,  45 }, {   1,  11,  17 }
      }, {  // Band 4
        {  30, 175, 188 }, {   9, 124, 169 }, {   1,  74, 116 },
        {   1,  48,  78 }, {   1,  30,  49 }, {   1,  11,  18 }
      }, {  // Band 5
        {  10, 222, 223 }, {   2, 150, 194 }, {   1,  83, 128 },
        {   1,  48,  79 }, {   1,  27,  45 }, {   1,  11,  17 }
      }
    }, {  // Inter
      {  // Band 0
        {  36,  41, 235 }, {  29,  36, 193 }, {  10,  27, 111 }
      }, {  // Band 1
        {  85, 165, 222 }, { 177, 162, 215 }, { 110, 135, 195 },
        {  57, 113, 168 }, {  23,  83, 120 }, {  10,  49,  61 }
      }, {  // Band 2
        {  85, 190, 223 }, {  36, 139, 200 }, {   5,  90, 146 },
        {   1,  60, 103 }, {   1,  38,  65 }, {   1,  18,  30 }
      }, {  // Band 3
        {  72, 202, 223 }, {  23, 141, 199 }, {   2,  86, 140 },
        {   1,  56,  97 }, {   1,  36,  61 }, {   1,  16,  27 }
      }, {  // Band 4
        {  55, 218, 225 }, {  13, 145, 200 }, {   1,  86, 141 },
        {   1,  57,  99 }, {   1,  35,  61 }, {   1,  13,  22 }
      }, {  // Band 5
        {  15, 235, 212 }, {   1, 132, 184 }, {   1,  84, 139 },
        {   1,  57,  97 }, {   1,  34,  56 }, {   1,  14,  23 }
      }
    }
  }, {  // UV plane
    {  // Intra
      {  // Band 0
        { 181,  21, 201 }, {  61,  37, 123 }, {  10,  38,  71 }
      }, {  // Band 1
        {  47, 106, 172 }, {  95, 104, 173 }, {  42,  93, 159 },
        {  18,  77, 131 }, {   4,  50,  81 }, {   1,  17,  23 }
      }, {  // Band 2
        {  62, 147, 199 }, {  44, 130, 189 }, {  28, 102, 154 },
        {  18,  75, 115 }, {   2,  44,  65 }, {   1,  12,  19 }
      }, {  // Band 3
        {  55, 153, 210 }, {  24, 130, 194 }, {   3,  93, 146 },
        {   1,  61,  97 }, {   1,  31,  50 }, {   1,  10,  16 }
      }, {  // Band 4
        {  49, 186, 223 }, {  17, 148, 204 }, {   1,  96, 142 },
        {   1,  53,  83 }, {   1,  26,  44 }, {   1,  11,  17 }
      }, {  // Band 5
        {  13, 217, 212 }, {   2, 136, 180 }, {   1,  78, 124 },
        {   1,  50,  83 }, {   1,  29,  49 }, {   1,  14,  23 }
      }
    }, {  // Inter
      {  // Band 0
        { 197,  13, 247 }, {  82,  17, 222 }, {  25,  17, 162 }
      }, {  // Band 1
        { 126, 186, 247 }, { 234, 191, 243 }, { 176, 177, 234 },
        { 104, 158, 220 }, {  66, 128, 186 }, {  55,  90, 137 }
      }, {  // Band 2
        { 111, 197, 242 }, {  46, 158, 219 }, {   9, 104, 171 },
        {   2,  65, 125 }, {   1,  44,  80 }, {   1,  17,  91 }
      }, {  // Band 3
        { 104, 208, 245 }, {  39, 168, 224 }, {   3, 109, 162 },
        {   1,  79, 124 }, {   1,  50, 102 }, {   1,  43, 102 }
      }, {  // Band 4
        {  84, 220, 246 }, {  31, 177, 231 }, {   2, 115, 180 },
        {   1,  79, 134 }, {   1,  55,  77 }, {   1,  60,  79 }
      }, {  // Band 5
        {  43, 243, 240 }, {   8, 180, 217 }, {   1, 115, 166 },
        {   1,  84, 121 }, {   1,  51,  67 }, {   1,  16,   6 }
      }
    }
  }
};

static const vp9_prob default_if_y_probs[BLOCK_SIZE_GROUPS][INTRA_MODES - 1] = {
  {  65,  32,  18, 144, 162, 194,  41,  51,  98 },  // block_size < 8x8
  { 132,  68,  18, 165, 217, 196,  45,  40,  78 },  // block_size < 16x16
  { 173,  80,  19, 176, 240, 193,  64,  35,  46 },  // block_size < 32x32
  { 221, 135,  38, 194, 248, 121,  96,  85,  29 }   // block_size >= 32x32
};

static const vp9_prob default_if_uv_probs[INTRA_MODES][INTRA_MODES - 1] = {
  { 120,   7,  76, 176, 208, 126,  28,  54, 103 },  // y = dc
  {  48,  12, 154, 155, 139,  90,  34, 117, 119 },  // y = v
  {  67,   6,  25, 204, 243, 158,  13,  21,  96 },  // y = h
  {  97,   5,  44, 131, 176, 139,  48,  68,  97 },  // y = d45
  {  83,   5,  42, 156, 111, 152,  26,  49, 152 },  // y = d135
  {  80,   5,  58, 178,  74,  83,  33,  62, 145 },  // y = d117
  {  86,   5,  32, 154, 192, 168,  14,  22, 163 },  // y = d153
  {  85,   5,  32, 156, 216, 148,  19,  29,  73 },  // y = d207
  {  77,   7,  64, 116, 132, 122,  37, 126, 120 },  // y = d63
  { 101,  21, 107, 181, 192, 103,  19,  67, 125 }   // y = tm
};

static const vp9_prob default_skip_probs[SKIP_CONTEXTS] = {
  192, 128, 64
};

static const vp9_prob default_switchable_interp_prob[SWITCHABLE_FILTER_CONTEXTS]
                                                    [SWITCHABLE_FILTERS - 1] = {
  { 235, 162, },
  { 36, 255, },
  { 34, 3, },
  { 149, 144, },
};

static const vp9_prob default_partition_probs[PARTITION_CONTEXTS]
                                             [PARTITION_TYPES - 1] = {
  // 8x8 -> 4x4
  { 199, 122, 141 },  // a/l both not split
  { 147,  63, 159 },  // a split, l not split
  { 148, 133, 118 },  // l split, a not split
  { 121, 104, 114 },  // a/l both split
  // 16x16 -> 8x8
  { 174,  73,  87 },  // a/l both not split
  {  92,  41,  83 },  // a split, l not split
  {  82,  99,  50 },  // l split, a not split
  {  53,  39,  39 },  // a/l both split
  // 32x32 -> 16x16
  { 177,  58,  59 },  // a/l both not split
  {  68,  26,  63 },  // a split, l not split
  {  52,  79,  25 },  // l split, a not split
  {  17,  14,  12 },  // a/l both split
  // 64x64 -> 32x32
  { 222,  34,  30 },  // a/l both not split
  {  72,  16,  44 },  // a split, l not split
  {  58,  32,  12 },  // l split, a not split
  {  10,   7,   6 },  // a/l both split
};

static const vp9_prob default_inter_mode_probs[INTER_MODE_CONTEXTS]
                                              [INTER_MODES - 1] = {
  {2,       173,   34},  // 0 = both zero mv
  {7,       145,   85},  // 1 = one zero mv + one a predicted mv
  {7,       166,   63},  // 2 = two predicted mvs
  {7,       94,    66},  // 3 = one predicted/zero and one new mv
  {8,       64,    46},  // 4 = two new mvs
  {17,      81,    31},  // 5 = one intra neighbour + x
  {25,      29,    30},  // 6 = two intra neighbours
};

static const vp9_prob default_intra_inter_p[INTRA_INTER_CONTEXTS] = {
  9, 102, 187, 225
};

static const vp9_prob default_comp_inter_p[COMP_INTER_CONTEXTS] = {
  239, 183, 119,  96,  41
};

static const vp9_prob default_comp_ref_p[REF_CONTEXTS] = {
  50, 126, 123, 221, 226
};

static const vp9_prob default_single_ref_p[REF_CONTEXTS][2] = {
  {  33,  16 },
  {  77,  74 },
  { 142, 142 },
  { 172, 170 },
  { 238, 247 }
};

static const struct tx_probs default_tx_probs = {
  { { 3, 136, 37 },
    { 5, 52,  13 } },

  { { 20, 152 },
    { 15, 101 } },

  { { 100 },
    { 66  } }
};

static const nmv_context default_nmv_context = {
  {32, 64, 96},
  { // NOLINT
    { /* vert component */ // NOLINT
      128,                                                  /* sign */
      {224, 144, 192, 168, 192, 176, 192, 198, 198, 245},   /* class */
      {216},                                                /* class0 */
      {136, 140, 148, 160, 176, 192, 224, 234, 234, 240},   /* bits */
      {{128, 128, 64}, {96, 112, 64}},                      /* class0_fp */
      {64, 96, 64},                                         /* fp */
      160,                                                  /* class0_hp bit */
      128,                                                  /* hp */
    },
    { /* hor component */ // NOLINT
      128,                                                  /* sign */
      {216, 128, 176, 160, 176, 176, 192, 198, 198, 208},   /* class */
      {208},                                                /* class0 */
      {136, 140, 148, 160, 176, 192, 224, 234, 234, 240},   /* bits */
      {{128, 128, 64}, {96, 112, 64}},                      /* class0_fp */
      {64, 96, 64},                                         /* fp */
      160,                                                  /* class0_hp bit */
      128,                                                  /* hp */
    }
  },
};

static const vp9_tree_index vp9_mv_joint_tree[TREE_SIZE(MV_JOINTS)] = {
  -MV_JOINT_ZERO, 2,
  -MV_JOINT_HNZVZ, 4,
  -MV_JOINT_HZVNZ, -MV_JOINT_HNZVNZ
};

static const vp9_tree_index vp9_mv_class_tree[TREE_SIZE(MV_CLASSES)] = {
  -MV_CLASS_0, 2,
  -MV_CLASS_1, 4,
  6, 8,
  -MV_CLASS_2, -MV_CLASS_3,
  10, 12,
  -MV_CLASS_4, -MV_CLASS_5,
  -MV_CLASS_6, 14,
  16, 18,
  -MV_CLASS_7, -MV_CLASS_8,
  -MV_CLASS_9, -MV_CLASS_10,
};

static const vp9_tree_index vp9_mv_class0_tree[TREE_SIZE(CLASS0_SIZE)] = {
  -0, -1,
};

static const vp9_tree_index vp9_mv_fp_tree[TREE_SIZE(MV_FP_SIZE)] = {
  -0, 2,
  -1, 4,
  -2, -3
};
//	void tx_counts_to_branch_counts_32x32(const unsigned int *tx_count_32x32p,
//	                                      unsigned int (*ct_32x32p)[2]) {
//	    ct_32x32p[0][0] = tx_count_32x32p[TX_4X4];
//	    ct_32x32p[0][1] = tx_count_32x32p[TX_8X8] +
//	                    tx_count_32x32p[TX_16X16] +
//	                    tx_count_32x32p[TX_32X32];
//	    ct_32x32p[1][0] = tx_count_32x32p[TX_8X8];
//	    ct_32x32p[1][1] = tx_count_32x32p[TX_16X16] +
//	                    tx_count_32x32p[TX_32X32];
//	    ct_32x32p[2][0] = tx_count_32x32p[TX_16X16];
//	    ct_32x32p[2][1] = tx_count_32x32p[TX_32X32];
//	}
//	
//	void tx_counts_to_branch_counts_16x16(const unsigned int *tx_count_16x16p,
//	                                      unsigned int (*ct_16x16p)[2]) {
//	    ct_16x16p[0][0] = tx_count_16x16p[TX_4X4];
//	    ct_16x16p[0][1] = tx_count_16x16p[TX_8X8] + tx_count_16x16p[TX_16X16];
//	    ct_16x16p[1][0] = tx_count_16x16p[TX_8X8];
//	    ct_16x16p[1][1] = tx_count_16x16p[TX_16X16];
//	}
//	
//	void tx_counts_to_branch_counts_8x8(const unsigned int *tx_count_8x8p,
//	                                    unsigned int (*ct_8x8p)[2]) {
//	    ct_8x8p[0][0] = tx_count_8x8p[TX_4X4];
//	    ct_8x8p[0][1] = tx_count_8x8p[TX_8X8];
//	}
//	
//	
//	INLINE int is_inter_mode(PREDICTION_MODE mode) {
//	  return mode >= NEARESTMV && mode <= NEWMV;
//	}
/* Array indices are identical to previously-existing INTRAMODECONTEXTNODES. */
static const vp9_tree_index vp9_intra_mode_tree[TREE_SIZE(INTRA_MODES)] = {
  -DC_PRED, 2,                      /* 0 = DC_NODE */
  -TM_PRED, 4,                      /* 1 = TM_NODE */
  -V_PRED, 6,                       /* 2 = V_NODE */
  8, 12,                            /* 3 = COM_NODE */
  -H_PRED, 10,                      /* 4 = H_NODE */
  -D135_PRED, -D117_PRED,           /* 5 = D135_NODE */
  -D45_PRED, 14,                    /* 6 = D45_NODE */
  -D63_PRED, 16,                    /* 7 = D63_NODE */
  -D153_PRED, -D207_PRED             /* 8 = D153_NODE */
};

static const vp9_tree_index vp9_inter_mode_tree[TREE_SIZE(INTER_MODES)] = {
  -INTER_OFFSET(ZEROMV), 2,
  -INTER_OFFSET(NEARESTMV), 4,
  -INTER_OFFSET(NEARMV), -INTER_OFFSET(NEWMV)
};

static const vp9_tree_index vp9_partition_tree[TREE_SIZE(PARTITION_TYPES)] = {
  -PARTITION_NONE, 2,
  -PARTITION_HORZ, 4,
  -PARTITION_VERT, -PARTITION_SPLIT
};

static const vp9_tree_index vp9_switchable_interp_tree
                         [TREE_SIZE(SWITCHABLE_FILTERS)] = {
  -EIGHTTAP, 2,
  -EIGHTTAP_SMOOTH, -EIGHTTAP_SHARP
};

static const INTERP_FILTER literal_to_filter[] = { EIGHTTAP_SMOOTH,
                                            EIGHTTAP,
                                            EIGHTTAP_SHARP,
                                            BILINEAR };

static const int seg_feature_data_signed[SEG_LVL_MAX] = { 1, 1, 0, 0 };

static const int seg_feature_data_max[SEG_LVL_MAX] = {
  MAXQ, MAX_LOOP_FILTER, 3, 0 };

static const UINT32 dc_qlookup[QINDEX_RANGE] = {
  4,       8,    8,    9,   10,   11,   12,   12,
  13,     14,   15,   16,   17,   18,   19,   19,
  20,     21,   22,   23,   24,   25,   26,   26,
  27,     28,   29,   30,   31,   32,   32,   33,
  34,     35,   36,   37,   38,   38,   39,   40,
  41,     42,   43,   43,   44,   45,   46,   47,
  48,     48,   49,   50,   51,   52,   53,   53,
  54,     55,   56,   57,   57,   58,   59,   60,
  61,     62,   62,   63,   64,   65,   66,   66,
  67,     68,   69,   70,   70,   71,   72,   73,
  74,     74,   75,   76,   77,   78,   78,   79,
  80,     81,   81,   82,   83,   84,   85,   85,
  87,     88,   90,   92,   93,   95,   96,   98,
  99,    101,  102,  104,  105,  107,  108,  110,
  111,   113,  114,  116,  117,  118,  120,  121,
  123,   125,  127,  129,  131,  134,  136,  138,
  140,   142,  144,  146,  148,  150,  152,  154,
  156,   158,  161,  164,  166,  169,  172,  174,
  177,   180,  182,  185,  187,  190,  192,  195,
  199,   202,  205,  208,  211,  214,  217,  220,
  223,   226,  230,  233,  237,  240,  243,  247,
  250,   253,  257,  261,  265,  269,  272,  276,
  280,   284,  288,  292,  296,  300,  304,  309,
  313,   317,  322,  326,  330,  335,  340,  344,
  349,   354,  359,  364,  369,  374,  379,  384,
  389,   395,  400,  406,  411,  417,  423,  429,
  435,   441,  447,  454,  461,  467,  475,  482,
  489,   497,  505,  513,  522,  530,  539,  549,
  559,   569,  579,  590,  602,  614,  626,  640,
  654,   668,  684,  700,  717,  736,  755,  775,
  796,   819,  843,  869,  896,  925,  955,  988,
  1022, 1058, 1098, 1139, 1184, 1232, 1282, 1336,
};

static const UINT32 ac_qlookup[QINDEX_RANGE] = {
  4,       8,    9,   10,   11,   12,   13,   14,
  15,     16,   17,   18,   19,   20,   21,   22,
  23,     24,   25,   26,   27,   28,   29,   30,
  31,     32,   33,   34,   35,   36,   37,   38,
  39,     40,   41,   42,   43,   44,   45,   46,
  47,     48,   49,   50,   51,   52,   53,   54,
  55,     56,   57,   58,   59,   60,   61,   62,
  63,     64,   65,   66,   67,   68,   69,   70,
  71,     72,   73,   74,   75,   76,   77,   78,
  79,     80,   81,   82,   83,   84,   85,   86,
  87,     88,   89,   90,   91,   92,   93,   94,
  95,     96,   97,   98,   99,  100,  101,  102,
  104,   106,  108,  110,  112,  114,  116,  118,
  120,   122,  124,  126,  128,  130,  132,  134,
  136,   138,  140,  142,  144,  146,  148,  150,
  152,   155,  158,  161,  164,  167,  170,  173,
  176,   179,  182,  185,  188,  191,  194,  197,
  200,   203,  207,  211,  215,  219,  223,  227,
  231,   235,  239,  243,  247,  251,  255,  260,
  265,   270,  275,  280,  285,  290,  295,  300,
  305,   311,  317,  323,  329,  335,  341,  347,
  353,   359,  366,  373,  380,  387,  394,  401,
  408,   416,  424,  432,  440,  448,  456,  465,
  474,   483,  492,  501,  510,  520,  530,  540,
  550,   560,  571,  582,  593,  604,  615,  627,
  639,   651,  663,  676,  689,  702,  715,  729,
  743,   757,  771,  786,  801,  816,  832,  848,
  864,   881,  898,  915,  933,  951,  969,  988,
  1007, 1026, 1046, 1066, 1087, 1108, 1129, 1151,
  1173, 1196, 1219, 1243, 1267, 1292, 1317, 1343,
  1369, 1396, 1423, 1451, 1479, 1508, 1537, 1567,
  1597, 1628, 1660, 1692, 1725, 1759, 1793, 1828,
};
#endif

#if 0
static INLINE int mi_cols_aligned_to_sb(int n_mis) {
  return ALIGN_POWER_OF_TWO(n_mis, MI_BLOCK_SIZE_LOG2);
}

static int get_tile_offset(int idx, int mis, int log2) {
    const int sb_cols = mi_cols_aligned_to_sb(mis) >> MI_BLOCK_SIZE_LOG2;
    const int offset = ((idx * sb_cols) >> log2) << MI_BLOCK_SIZE_LOG2;
    return MIN(offset, mis);
}

static void vp9_tile_set_row(VP9_TILE_INFO_T *pTile, const VP9_COMMON_T *prCommon, int row) {
    pTile->i4MiRowStart = get_tile_offset(row, prCommon->u4MI_rows, prCommon->rUnCompressedHeader.u4Log2TileRows);
    pTile->i4MiRowEnd = get_tile_offset(row + 1, prCommon->u4MI_rows, prCommon->rUnCompressedHeader.u4Log2TileRows);
}

static void vp9_tile_set_col(VP9_TILE_INFO_T *pTile, const VP9_COMMON_T *prCommon, int col) {
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
            VP9_ERROR("truncated packet or corrupt tile length!");
            return 0;
        }
        u4Byte1 = (UINT32)(*(pData++));
        if((u4DataStart > u4DataEnd) && (u4DataStart + 4 >= u4FifoEnd))
        {
            if((UINT32)pData == u4FifoEnd)
            {
                pData = (UCHAR*)u4FifoStart;
            }
            u4Byte2 = (UINT32)(*(pData++));
            if((UINT32)pData == u4FifoEnd)
            {
                pData = (UCHAR*)u4FifoStart;
            }
            u4Byte3 = (UINT32)(*(pData++));
            if((UINT32)pData == u4FifoEnd)
            {
                pData = (UCHAR*)u4FifoStart;
            }
            u4Byte4 = (UINT32)(*(pData++));
            if((UINT32)pData == u4FifoEnd)
            {
                pData = (UCHAR*)u4FifoStart;
            }
        }
        else
        {
            u4Byte2 = (UINT32)(*(pData++));
            u4Byte3 = (UINT32)(*(pData++));
            u4Byte4 = (UINT32)(*(pData++));
        }
        
        u4Size = u4Byte1 << 24 | u4Byte2 << 16 | u4Byte3 << 8 | u4Byte4;
        u4DoneSize += 4;
        
        if (u4Size > u4LeftSize)  
        {
            VP9_ERROR("truncated packet or corrupt tile length!");
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
#endif
UINT32 vp9_get_qindex(SEGMENTATION *seg, UINT32 segment_id, UINT32 base_qindex);
UINT32 get_tile_buffer(UINT32 u4DataStart, UINT32 u4DataEnd, UINT32 u4FifoStart, UINT32 u4FifoEnd, TileBuffer *pTileBuf);
void vp9_tile_init(VP9_TILE_INFO_T *pTile, const VP9_COMMON_T *prCommon, int row, int col);
void vp9_tile_set_row(VP9_TILE_INFO_T *pTile, const VP9_COMMON_T *prCommon, int row);
void vp9_tile_set_col(VP9_TILE_INFO_T *pTile, const VP9_COMMON_T *prCommon, int col);



BOOL fgVP9IntraOnly(VP9_COMMON_T * prCommon);
INT32 iVP9GetFreeFb(VP9_COMMON_T * prCommon);
void vVP9RefCntFb(VP9_COMMON_T* prCommon, int *idx, int new_idx, BOOL bFbmCtrl);
INT32 i4VP9SetupFrameSize(UINT32 u4CoreId, VP9_COMMON_T * prCommon);
INT32 i4VP9SetupFrameSizeWithRefs(UINT32 u4CoreId, VP9_COMMON_T * prCommon);
void vVP9_Setup_Scale_Factors_For_Frame(VP9_Scale_Factors_T *rScaleFactors,
                                       INT32 other_w, INT32 other_h,
                                       INT32 this_w, INT32 this_h);
void vVP9SetupPastIndependence(VP9_COMMON_T * prCommon);
void vVP9SetupLoopFilter(UINT32 u4CoreId, VP9_LOOP_FILTER_INFO_T* prLoopFilter);
void vVP9SetupQuantization(VP9_COMMON_T * prCommon, VP9_UNCOMPRESSED_HEADER_T* prUnCompressed, MACROBLOCKD* prMBD, UINT32 u4CoreId);
void vVP9SetupSegmentation(SEGMENTATION *seg, UINT32 u4CoreId);
void vVP9SetupTileInfo(VP9_UNCOMPRESSED_HEADER_T * prUnCompressed, UINT32 u4CoreId);
void vVP9SetupCompoundRefMode(VP9_COMMON_T *prCommon);
void vVP9_Init_Dequantizer(VP9_COMMON_T *prCommon);
void vVP9SetPicInfo(VP9_COMMON_T* prCommon, UINT32 u4CoreId);
void vVP9AdaptCoefProbs(VP9_COMMON_T *prCommon);
void vVP9AdaptModeProbs(VP9_COMMON_T *prCommon);
void vVP9AdaptMvProbs(VP9_COMMON_T *prCommon, int allow_hp);
void vVP9SwapFrameBuffers(VP9_COMMON_T* prCommon);
#endif

