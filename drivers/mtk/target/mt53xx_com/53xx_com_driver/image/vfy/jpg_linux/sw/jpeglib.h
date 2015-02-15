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
 * $RCSfile: jpeglib.h,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/7 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: b4bb2a2295c76b9e9463af179f70a7ad $ Asa
 *
 * Description: 
 *         This file define jpeg constants and structure
 *---------------------------------------------------------------------------*/

#ifndef _JPEGLIB_H_
#define _JPEGLIB_H_

/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#include "x_os.h"
#include "x_common.h"
#include "jpg_config.h"
/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/

/* Various constants determining the sizes of things.
 * All of these are specified by the JPEG standard, so don't change them
 * if you want to be compatible.
 */
#define NUM_QUANT_TBLS      4    /* Quantization tables are numbered 0..3 */
#define NUM_HUFF_TBLS       4    /* Huffman tables are numbered 0..3 */
/*#define MAX_COMPS_IN_SCAN   4 JPEG limit on # of components in one scan */
/*Mars Wen,[2002/2/5], set max # of allowable components to 3 (Y,Cr,Cb)*/
#define Y_INDEX 0
#define CB_INDEX 1
#define CR_INDEX 2
#define MAX_COMPS_IN_SCAN   4    /* JPEG limit on # of components in one scan */ // [Kate20080610] increase component number from 3 to 4 to let x_img_fopen pass; CQ BDP00112427 and CQ BDP00112423
#define MAX_SAMP_FACTOR     4    /* JPEG limit on sampling factors */
/* Unfortunately, some bozo at Adobe saw no reason to be bound by the standard;
 * the PostScript DCT filter can emit files with many more than 10 blocks/MCU.
 * If you happen to run across such a file, you can up D_MAX_BLOCKS_IN_MCU
 * to handle it.  We even let you do this from the jconfig.h file.  However,
 * we strongly discourage changing C_MAX_BLOCKS_IN_MCU; just because Adobe
 * sometimes emits noncompliant files doesn't mean you should too.
 */
#define D_MAX_BLOCKS_IN_MCU   10    /* decompressor's limit on blocks per MCU */

/* Return value is one of: */
#define JPEG_MARKER_OK  0    /* Process marker OK */
#define JPEG_RETURN_ERR 1    /* Return with error */
#define JPEG_REACHED_EOI  2    /* Reached end of image */

/*
 * Define BITS_IN_JSAMPLE as either
 *   8   for 8-bit sample values (the usual setting)
 *   12  for 12-bit sample values
 * Only 8 and 12 are legal data precisions for lossy JPEG according to the
 * JPEG standard, and the IJG code does not support anything else!
 * We do not support run-time selection of data precision, sorry.
 */

#define BITS_IN_JSAMPLE  8    /* use 8 or 12 */

/*
 * Maximum number of components (color channels) allowed in JPEG image.
 * To meet the letter of the JPEG spec, set this to 255.  However, darn
 * few applications need more than 4 channels (maybe 5 for CMYK + alpha
 * mask).  We recommend 10 as a reasonable compromise; use 4 if you are
 * really short on memory.  (Each allowed component costs a hundred or so
 * bytes of storage, whether actually used in an image or not.)
 */

#define MAX_COMPONENTS  10    /* maximum number of image components */

/* Representation of a single sample (pixel element value).
 * We frequently allocate large arrays of these, so it's important to keep
 * them small.  But if you have memory to burn and access to char or short
 * arrays is very slow on your hardware, you might want to change these.
 */

#define RIGHT_SHIFT(x,shft) ((x) >> (shft))

#if BITS_IN_JSAMPLE == 8
/* JSAMPLE should be the smallest type that will hold the values 0..255.
 * You can use a signed char by having GETJSAMPLE mask it with 0xFF.
 */

#define MAXJSAMPLE  255
#define CENTERJSAMPLE 128

#endif /* BITS_IN_JSAMPLE == 8 */


#if BITS_IN_JSAMPLE == 12
/* JSAMPLE should be the smallest type that will hold the values 0..4095.
 * On nearly all machines "short" will do nicely.
 */

#define MAXJSAMPLE  4095
#define CENTERJSAMPLE 2048

#endif /* BITS_IN_JSAMPLE == 12 */

/*
 * A forward DCT routine is given a pointer to a work area of type DCTELEM[];
 * the DCT is to be performed in-place in that buffer.  Type DCTELEM is int
 * for 8-bit samples, INT32 for 12-bit samples.  (NOTE: Floating-point DCT
 * implementations use an array of type FAST_FLOAT, instead.)
 * The DCT inputs are expected to be signed (range +-CENTERJSAMPLE).
 * The DCT outputs are returned scaled up by a factor of 8; they therefore
 * have a range of +-8K for 8-bit data, +-128K for 12-bit data.  This
 * convention improves accuracy in integer implementations and saves some
 * work in floating-point ones.
 * Quantization of the output coefficients is done by jcdctmgr.c.
 */

#if BITS_IN_JSAMPLE == 8
typedef INT16 DCTELEM;        /* 16 or 32 bits is fine */
#else
typedef INT32 DCTELEM;        /* must have 32 bits */
#endif


#if BITS_IN_JSAMPLE == 8
typedef INT16 IFAST_MULT_TYPE;    /* 16 bits is OK, use short if faster */
#define IFAST_SCALE_BITS  2    /* fractional bits in scale factors */
#else
typedef INT32 IFAST_MULT_TYPE;    /* need 32 bits for scaled quantizers */
#define IFAST_SCALE_BITS  13    /* fractional bits in scale factors */
#endif
/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/

#endif /* _JPEGLIB_H_ */

