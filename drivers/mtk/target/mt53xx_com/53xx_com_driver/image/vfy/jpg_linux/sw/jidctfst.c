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
 * $RCSfile: jidctfst.c,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/6 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: a0525f342726064669d20c0c61e7ab97 $ Asa
 *
 * Description: 
 *         This file contains fast version IDCT routine
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/

#include "x_os.h"
#include "x_common.h"
#include "jpg_config.h"
#include "jpeglib.h"
#include "djpeg_sw_api.h"
/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
/*
 * This file contains a fast, not so accurate integer implementation of the
 * inverse DCT (Discrete Cosine Transform).  This routine
 * must also perform dequantization of the input coefficients.
 *
 * A 2-D IDCT can be done by 1-D IDCT on each column followed by 1-D IDCT
 * on each row (or vice versa, but it's more convenient to emit a row at
 * a time).  Direct algorithms are also available, but they are much more
 * complex and seem not to be any faster when reduced to code.
 *
 * This implementation is based on Arai, Agui, and Nakajima's algorithm for
 * scaled DCT.  Their original paper (Trans. IEICE E-71(11):1095) is in
 * Japanese, but the algorithm is described in the Pennebaker & Mitchell
 * JPEG textbook (see REFERENCES section in file README).  The following code
 * is based directly on figure 4-8 in P&M.
 * While an 8-point DCT cannot be done in less than 11 multiplies, it is
 * possible to arrange the computation so that many of the multiplies are
 * simple scalings of the final outputs.  These multiplies can then be
 * folded into the multiplications or divisions by the JPEG quantization
 * table entries.  The AA&N method leaves only 5 multiplies and 29 adds
 * to be done in the DCT itself.
 * The primary disadvantage of this method is that with fixed-point math,
 * accuracy is lost due to imprecise representation of the scaled
 * quantization values.  The smaller the quantization table entry, the less
 * precise the scaled value, so this implementation does worse with high-
 * quality-setting files than with low-quality ones.
 */

#define range_limit2(result, x) \
{ \
  if (((x) & 0xFF00) != 0) \
  { \
   if ((x) < 0)  \
    result = 0; \
   else if ((x) > MAXSAMPLE)  \
    result = (INT8) MAXSAMPLE; \
  }else{ \
    result = (x); \
  } \
}

/* Scaling decisions are generally the same as in the LL&M algorithm;
 * see jidctint.c for more details.  However, we choose to descale
 * (right shift) multiplication products as soon as they are formed,
 * rather than carrying additional fractional bits into subsequent additions.
 * This compromises accuracy slightly, but it lets us save a few shifts.
 * More importantly, 16-bit arithmetic is then adequate (for 8-bit samples)
 * everywhere except in the multiplications proper; this saves a good deal
 * of work on 16-bit-int machines.
 *
 * The dequantized coefficients are not integers because the AA&N scaling
 * factors have been incorporated.  We represent them scaled up by PASS1_BITS,
 * so that the first and second IDCT rounds have the same input scaling.
 * For 8-bit JSAMPLEs, we choose IFAST_SCALE_BITS = PASS1_BITS so as to
 * avoid a descaling shift; this compromises accuracy rather drastically
 * for small quantization table entries, but it saves a lot of shifts.
 * For 12-bit JSAMPLEs, there's no hope of using 16x16 multiplies anyway,
 * so we use a much larger scaling factor to preserve accuracy.
 *
 * A final compromise is to represent the multiplicative constants to only
 * 8 fractional bits, rather than 13.  This saves some shifting work on some
 * machines, and may also reduce the cost of multiplication (since there
 * are fewer one-bits in the constants).
 */

#if BITS_IN_JSAMPLE == 8
#define CONST_BITS  8
#define PASS1_BITS  2
#else
#define CONST_BITS  8
#define PASS1_BITS  1   /* lose a little precision to avoid overflow */
#endif

/* Some C compilers fail to reduce "FIX(constant)" at compile time, thus
 * causing a lot of useless floating-point operations at run time.
 * To get around this we use the following pre-calculated constants.
 * If you change CONST_BITS you may want to add appropriate values.
 * (With a reasonable C compiler, you can just rely on the FIX() macro...)
 */

#if CONST_BITS == 8
#define FIX_1_082392200  ((INT32)  277)   /* FIX(1.082392200) */
#define FIX_1_414213562  ((INT32)  362)   /* FIX(1.414213562) */
#define FIX_1_847759065  ((INT32)  473)   /* FIX(1.847759065) */
#define FIX_2_613125930  ((INT32)  669)   /* FIX(2.613125930) */
#else
#define FIX_1_082392200  FIX(1.082392200)
#define FIX_1_414213562  FIX(1.414213562)
#define FIX_1_847759065  FIX(1.847759065)
#define FIX_2_613125930  FIX(2.613125930)
#endif

/* Multiply a DCTELEM variable by an INT32 constant, and immediately
 * descale to yield a DCTELEM result.
 */

#define MULTIPLY(var,const)  ((DCTELEM) DESCALE((var) * (const), CONST_BITS))


/* Dequantize a coefficient by multiplying it by the multiplier-table
 * entry; produce a DCTELEM result.  For 8-bit data a 16x16->16
 * multiplication will do.  For 12-bit data, the multiplier table is
 * declared INT32, so a 32-bit multiply will be used.
 */

#if BITS_IN_JSAMPLE == 8
#define DEQUANTIZE(coef,quantval)  (((IFAST_MULT_TYPE) (coef)) * (quantval))
#else
#define DEQUANTIZE(coef,quantval)  \
  DESCALE((coef)*(quantval), IFAST_SCALE_BITS-PASS1_BITS)
#endif


/* Like DESCALE, but applies to a DCTELEM and produces an int.
 * We assume that int right shift is unsigned if INT32 right shift is.
 */

#ifdef RIGHT_SHIFT_IS_UNSIGNED
#define ISHIFT_TEMPS  DCTELEM ishift_temp;
#if BITS_IN_JSAMPLE == 8
#define DCTELEMBITS  16   /* DCTELEM may be 16 or 32 bits */
#else
#define DCTELEMBITS  32   /* DCTELEM must be 32 bits */
#endif
#define IRIGHT_SHIFT(x,shft)  \
    ((ishift_temp = (x)) < 0 ? \
     (ishift_temp >> (shft)) | ((~((DCTELEM) 0)) << (DCTELEMBITS-(shft))) : \
     (ishift_temp >> (shft)))
#else
#define ISHIFT_TEMPS
#define IRIGHT_SHIFT(x,shft)  ((x) >> (shft))
#endif

#ifdef USE_ACCURATE_ROUNDING
#define IDESCALE(x,n)  ((INT16) IRIGHT_SHIFT((x) + (1 << ((n)-1)), n))
#else
#define IDESCALE(x,n)  ((INT16) IRIGHT_SHIFT(x, n))
#endif

#define LO_DESCALE(x,n)  RIGHT_SHIFT((x) + (ONE << ((n)-1)), n)
#define SHIFT_BIT_NUM 14-IFAST_SCALE_BITS
/*-----------------------------------------------------------------------------
                    extern data declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
const INT16 JdecAanscales[DCTSIZE2] =
{
    /* precomputed values scaled up by 14 bits */
    16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
    22725, 31521, 29692, 26722, 22725, 17855, 12299,  6270,
    21407, 29692, 27969, 25172, 21407, 16819, 11585,  5906,
    19266, 26722, 25172, 22654, 19266, 15137, 10426,  5315,
    16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
    12873, 17855, 16819, 15137, 12873, 10114,  6967,  3552,
    8867, 12299, 11585, 10426,  8867,  6967,  4799,  2446,
    4520,  6270,  5906,  5315,  4520,  3552,  2446,  1247
};
/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Name:  vInitial_fast_IDCT
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vInitial_fast_IDCT(JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj)
{
    COMPONENT_INFO_T *compptr;
    INT16 i, ci;
    UINT16 *qtbl;
    INT16 * ifmtbl;
    IMG_BASIC_INFO_T* pt_info = NULL;
    
    pt_info = pt_jpg_obj->pt_current_sel; 
    if (pt_info == NULL)
    {
        return;
    }
    
    for (ci = 0, compptr = pt_info->at_comp_info; ci < pt_info->ui2_num_components; ci++, compptr++)
    {
        /* For AA&N IDCT method, multipliers are equal to quantization
        * coefficients scaled by scalefactor[row]*scalefactor[col], where
        *   scalefactor[0] = 1
        *   scalefactor[k] = cos(k*PI/16) * sqrt(2)    for k=1..7
        * For integer operation, the multiplier table is to be scaled by
        * IFAST_SCALE_BITS.
        */
        ifmtbl = (INT16 *) compptr->ai2_dct_table;
        qtbl = compptr->aui2_quant_table;

        for (i = 0; i < DCTSIZE2; i++)
        {
            ifmtbl[i] = LO_DESCALE((INT32) (qtbl[i] * JdecAanscales[i]), SHIFT_BIT_NUM);
        }
    }
} /* initial_IDCT() function */


/*-----------------------------------------------------------------------------
 * Name:  vFirst_1D_IDCT
 *
 * Description: Perform dequantization and inverse DCT on one block of coefficients.
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vFirst_1D_IDCT(COMPONENT_INFO_T * compptr, INT16 *coef_block, INT16 *wsptr)
{
    DCTELEM tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    DCTELEM tmp10, tmp11, tmp12, tmp13;
    DCTELEM z5, z10, z11, z12, z13;
    INT16 *inptr;
    INT16 *quantptr;
    INT32 ctr;

    /* Pass 1: process columns from input, store into work array. */
    inptr = coef_block;
    quantptr = (IFAST_MULT_TYPE *) compptr->ai2_dct_table;
    for (ctr = DCTSIZE; ctr > 0; ctr--) 
    {
        /* Due to quantization, we will usually find that many of the input
         * coefficients are zero, especially the AC terms.  We can exploit this
         * by short-circuiting the IDCT calculation for any column in which all
         * the AC terms are zero.  In that case each output is equal to the
         * DC coefficient (with scale factor as needed).
         * With typical images and quantization tables, half or more of the
         * column DCT calculations can be simplified this way.
         */

        if (((inptr[DCTSIZE*1] | inptr[DCTSIZE*2] | inptr[DCTSIZE*3]) == 0) &&
        ((inptr[DCTSIZE*4] | inptr[DCTSIZE*5] | inptr[DCTSIZE*6] | inptr[DCTSIZE*7]) == 0)) 
        {
            /* AC terms all zero */
            INT16 dcval = (INT16) DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);

            wsptr[DCTSIZE*0] = dcval;
            wsptr[DCTSIZE*1] = dcval;
            wsptr[DCTSIZE*2] = dcval;
            wsptr[DCTSIZE*3] = dcval;
            wsptr[DCTSIZE*4] = dcval;
            wsptr[DCTSIZE*5] = dcval;
            wsptr[DCTSIZE*6] = dcval;
            wsptr[DCTSIZE*7] = dcval;

            inptr++;      /* advance pointers to next column */
            quantptr++;
            wsptr++;
            continue;
        }

        /* Even part */
        tmp0 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
        tmp1 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
        tmp2 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
        tmp3 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);
        tmp10 = tmp0 + tmp2;  /* phase 3 */
        tmp11 = tmp0 - tmp2;
        tmp13 = tmp1 + tmp3;  /* phases 5-3 */
        tmp12 = MULTIPLY(tmp1 - tmp3, FIX_1_414213562) - tmp13; /* 2*c4 */
        tmp0 = tmp10 + tmp13; /* phase 2 */
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        /* Odd part */
        tmp4 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
        tmp5 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
        tmp6 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
        tmp7 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);
        z13 = tmp6 + tmp5;    /* phase 6 */
        z10 = tmp6 - tmp5;
        z11 = tmp4 + tmp7;
        z12 = tmp4 - tmp7;
        tmp7 = z11 + z13;   /* phase 5 */
        tmp11 = MULTIPLY(z11 - z13, FIX_1_414213562); /* 2*c4 */

        z5 = MULTIPLY(z10 + z12, FIX_1_847759065); /* 2*c2 */
        tmp10 = MULTIPLY(z12, FIX_1_082392200) - z5; /* 2*(c2-c6) */
        tmp12 = MULTIPLY(z10, - FIX_2_613125930) + z5; /* -2*(c2+c6) */
        tmp6 = tmp12 - tmp7;  /* phase 2 */
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        wsptr[DCTSIZE*0] = (INT16) (tmp0 + tmp7);
        wsptr[DCTSIZE*7] = (INT16) (tmp0 - tmp7);
        wsptr[DCTSIZE*1] = (INT16) (tmp1 + tmp6);
        wsptr[DCTSIZE*6] = (INT16) (tmp1 - tmp6);
        wsptr[DCTSIZE*2] = (INT16) (tmp2 + tmp5);
        wsptr[DCTSIZE*5] = (INT16) (tmp2 - tmp5);
        wsptr[DCTSIZE*4] = (INT16) (tmp3 + tmp4);
        wsptr[DCTSIZE*3] = (INT16) (tmp3 - tmp4);

        inptr++;      /* advance pointers to next column */
        quantptr++;
        wsptr++;
    }
}
/*-----------------------------------------------------------------------------
 * Name:  vSecond_1D_IDCT
 *
 * Description: -
 *
 * Inputs:  -
 *
 * Outputs: -
 *
 * Returns: -
 ----------------------------------------------------------------------------*/
void vSecond_1D_IDCT(JDEC_INST_T *prJdecInst, IMG_JPG_OBJ_T* pt_jpg_obj, INT16 *wsptr, INT8 *out_block, UINT16 ui2_ci)
{
    INT32 ctr;
    INT8 *outptr;
    INT8 dcval = 0;
    DCTELEM tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    DCTELEM tmp10, tmp11, tmp12, tmp13;
    DCTELEM z5, z10, z11, z12, z13;
    UINT32 ui4_pic_width_in_buf;
    
    ui4_pic_width_in_buf = pt_jpg_obj->pt_current_sel->ui4_pic_width_in_buf[ui2_ci];
    /* Pass 2: process rows from work array, store into output array. */
    /* Note that we must descale the results by a factor of 8 == 2**3, */
    /* and also undo the PASS1_BITS scaling. */
    for (ctr = 0; ctr < DCTSIZE; ctr++) 
    {
        outptr = out_block + ctr * ui4_pic_width_in_buf;

        /* Rows of zeroes can be exploited in the same way as we did with columns.
        * However, the column calculation has created many nonzero AC terms, so
        * the simplification applies less often (typically 5% to 10% of the time).
        * On machines with very fast multiplication, it's possible that the
        * test takes more time than it's worth.  In that case this section
        * may be commented out.
        */

        if ((wsptr[1]  == 0) &&
        ((wsptr[2] | wsptr[3] | wsptr[4] | wsptr[5] | wsptr[6] | wsptr[7]) == 0)) 
        {
            /* AC terms all zero */
            range_limit2(dcval, IDESCALE(wsptr[0], PASS1_BITS+3) + CENTERSAMPLE);

            outptr[0] = dcval;
            outptr[1] = dcval;
            outptr[2] = dcval;
            outptr[3] = dcval;
            outptr[4] = dcval;
            outptr[5] = dcval;
            outptr[6] = dcval;
            outptr[7] = dcval;
            wsptr += DCTSIZE;   /* advance pointer to next row */
            continue;
        }

        /* Even part */
        tmp10 = ((DCTELEM) wsptr[0] + (DCTELEM) wsptr[4]);
        tmp11 = ((DCTELEM) wsptr[0] - (DCTELEM) wsptr[4]);
        tmp13 = ((DCTELEM) wsptr[2] + (DCTELEM) wsptr[6]);
        tmp12 = MULTIPLY((DCTELEM) wsptr[2] - (DCTELEM) wsptr[6], FIX_1_414213562) - tmp13;
        tmp0 = tmp10 + tmp13;
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        /* Odd part */
        z13 = (DCTELEM) wsptr[5] + (DCTELEM) wsptr[3];
        z10 = (DCTELEM) wsptr[5] - (DCTELEM) wsptr[3];
        z11 = (DCTELEM) wsptr[1] + (DCTELEM) wsptr[7];
        z12 = (DCTELEM) wsptr[1] - (DCTELEM) wsptr[7];
        tmp7 = z11 + z13;   /* phase 5 */
        tmp11 = MULTIPLY(z11 - z13, FIX_1_414213562); /* 2*c4 */
        z5 = MULTIPLY(z10 + z12, FIX_1_847759065); /* 2*c2 */
        tmp10 = MULTIPLY(z12, FIX_1_082392200) - z5; /* 2*(c2-c6) */
        tmp12 = MULTIPLY(z10, - FIX_2_613125930) + z5; /* -2*(c2+c6) */
        tmp6 = tmp12 - tmp7;  /* phase 2 */
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        /* Final output stage: scale down by a factor of 8 and range-limit */
        range_limit2(outptr[0], IDESCALE(tmp0 + tmp7, PASS1_BITS+3) + CENTERSAMPLE);
        range_limit2(outptr[7], IDESCALE(tmp0 - tmp7, PASS1_BITS+3) + CENTERSAMPLE);
        range_limit2(outptr[1], IDESCALE(tmp1 + tmp6, PASS1_BITS+3) + CENTERSAMPLE);
        range_limit2(outptr[6], IDESCALE(tmp1 - tmp6, PASS1_BITS+3) + CENTERSAMPLE);
        range_limit2(outptr[2], IDESCALE(tmp2 + tmp5, PASS1_BITS+3) + CENTERSAMPLE);
        range_limit2(outptr[5], IDESCALE(tmp2 - tmp5, PASS1_BITS+3) + CENTERSAMPLE);
        range_limit2(outptr[4], IDESCALE(tmp3 + tmp4, PASS1_BITS+3) + CENTERSAMPLE);
        range_limit2(outptr[3], IDESCALE(tmp3 - tmp4, PASS1_BITS+3) + CENTERSAMPLE);
        wsptr += DCTSIZE;   /* advance pointer to next row */
    }
}
