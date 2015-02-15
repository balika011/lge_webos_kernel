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
 * $RCSfile: jpg_config.h,v $
 * $Revision: #1 $ 1.0
 * $Date: 2015/02/15 $ 2004/11/26
 * $Author: p4admin $
 * $CCRevision: /main/DTV_X_HQ_int/8 $
 * $SWAuthor: Asa Wang $
 * $MD5HEX: 380e58a0d56037b63303610eaffe49d2 $ Asa
 *
 * Description: 
 *         This file define jpeg constants and structure
 *---------------------------------------------------------------------------*/

#ifndef _JPG_CONFIG_H_
#define _JPG_CONFIG_H_

/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#include "x_os.h"
#include "x_common.h"
/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#define ONE ((INT32) 1)
#define CONST_SCALE (ONE << CONST_BITS)

/* Convert a positive real constant to an integer scaled by CONST_SCALE.
 * Caution: some C compilers fail to reduce "FIX(constant)" at compile time,
 * thus causing a lot of useless floating-point operations at run time.
 */

#define FIX(x)  ((INT32) ((x) * CONST_SCALE + 0.5))

/* Descale and correctly round an INT32 value that's scaled by N bits.
 * We assume RIGHT_SHIFT rounds towards minus infinity, so adding
 * the fudge factor is correct for either sign of X.
 */

#define DESCALE(x,n)  RIGHT_SHIFT((x) + (ONE << ((n)-1)), n)
#define TIME_OUT_LOOP_COUNT 50000
#define HUFF_EXTEND(x,s)  ((x) < (1<<((s)-1)) ? (x) + (((-1)<<(s)) + 1) : (x))
#define DCTSIZE  8        /* The basic DCT block is 8x8 samples */
#define DCTSIZE2 64        /* DCTSIZE squared; # of elements in a block */
#define JPG_MAX_SCAN_NUM  100    /* Mars,[2002/6/25] */
#define MAX_IMG_IN_JPEG 2
/* define the input buffer storage element size is INSIZE byte size */
#define INSIZE 2
#define UP_SCALE    1024
#define UP_SCALE_Q  10        /* / UP_SCALE right shift bit number */
#define UP_SCALE_Q2 20        /* / UP_SCALE right shift bit number */
#define UP_SCALE_R  0x3FF    /* % UP_SCALE remainder mask */
#define MAXSAMPLE       255
#define CENTERSAMPLE    128
#define SCALE_UP_CEIL(x)  (((UINT32)x&UP_SCALE_R) ? ((((UINT32)x)+UP_SCALE)&0xFFFFFC00):(x))

#define BIG_LOOP 5000

/* calculate mode for DRAM address */
#define ALIGN_TO_DW 0
#define FULL 1

#define Y 0
#define Cb 1
#define Cr 2
#define ALL 3  

#define TYPE_BYTE       1
#define TYPE_ASCII      2
#define TYPE_SHORT      3
#define TYPE_LONG       4
#define TYPE_RATIONAL   5
#define TYPE_UNDEFINED  7
#define TYPE_SLONG      9
#define TYPE_SRATIONAL  10

/* Total Error Type (Here is related to variable _dwJPGErrorType). */
/* 1: Unsupported Arithmetic Decoding */
/* 2: Huffman table was not defined */
/* 3: Bogus Huffman table definition */
/* 4: Corrupt JPEG data: premature end of data segment */
/* 5: Corrupt JPEG data: bad Huffman code */
/* 6: Invalid SOS parameters for sequential JPEG */
/* 7: Bogus sampling factors */
/* 8: Too many color components */
/* 9: Sampling factors too large for interleaved scan */
/* 10: Quantization table was not defined */
/* 11: Corrupt JPEG data: found marker instead of RST */
/* 12: Invalid JPEG file structure: two SOI markers */
/* 13: Invalid JPEG file structure: two SOF markers */
/* 14: Empty JPEG image (DNL not supported) */
/* 15: Invalid JPEG file structure: SOS before SOF */
/* 16: Bogus marker length */
/* 17: Invalid component ID in SOS */
/* 18: Bogus DHT index */
/* 19: Didn't expect more than one scan */
/* 20: Bogus DQT index */
/* 21: Unknown APP0 marker (not JFIF) */
/* 22: Corrupt JPEG data: extraneous bytes before marker */
/* 23: Not a JPEG file */
/* 24: Unsupported JPEG process: invalid SOF type */
/* 25: DRAM workspace can't store even one macroblock column for rotating image */
/* 26: Read VLD data more than file size */
/* 27: DRAM workspace can't store even one iMCU row */
/* 33: Progressive JPG pciture size over limit */
/* 34: JPG HW MCU decoding error */
/* 35: JPG HW MCU shift bitstream error */

#define JPG_CORRUPT_DATA_LIMIT 100

/* Corrupt Errors: Decoding action can resume but decoded image may be corrupted */
/* Error Number:                  */
/* 2: Huffman table was not defined */
/* 3: Bogus Huffman table definition */
/* 4: Corrupt JPEG data: premature end of data segment */
/* 5: Corrupt JPEG data: bad Huffman code */
/* 6: Invalid SOS parameters for sequential JPEG */
/* 7: Bogus sampling factors */
/* 10: Quantization table was not defined */
/* 11: Corrupt JPEG data: found marker instead of RST */
/* 16: Bogus marker length */
/* 21: Unknown APP0 marker (not JFIF) */
/* 22:  Corrupt JPEG data: extraneous bytes before marker */
/*      When corrupt JPEG data not exceed JPG_CORRUPT_DATA_LIMIT, it is a corrupt error. */
/* 26: Read VLD data more than file size */

/* Fatal Errors: Decoding action must be halted */
/* Error Number:                            */
/* 1: Unsupported Arithmetic Decoding */
/* 8: Too many color components */
/* 9: Sampling factors too large for interleaved scan */
/* 12: Invalid JPEG file structure: two SOI markers */
/* 13: Invalid JPEG file structure: two SOF markers */
/* 14: Empty JPEG image (DNL not supported) */
/* 15: Invalid JPEG file structure: SOS before SOF */
/* 17: Invalid component ID in SOS */
/* 18: Bogus DHT index */
/* 19: Didn't expect more than one scan */
/* 20: Bogus DQT index */
/* 22:  Corrupt JPEG data: extraneous bytes before marker */
/*      When corrupt JPEG data exceed JPG_CORRUPT_DATA_LIMIT, it becomes a fatal error. */
/* 23: Not a JPEG file */
/* 24: Unsupported JPEG process: invalid SOF type */
/* 25: DRAM workspace can't store even one macroblock column for rotating image */
/* 27: DRAM workspace can't store even one iMCU row */
/* 28: VLD read/write pointer collision exceeds limit */
/* 29: Too many iMCU rows */
/* 30: VDO buffer input stalled time exceeds limit */
/* 31: VLD buffer Initialization stalled time exceeds limit */


/* Error Classification */
typedef enum
{
  JPG_NO_ERR,            /* no Error occur */
  JPG_CORRUPT_ERR,        /* Corrupt Errors: Decoding action can resume but decoded image may be corrupted */
  JPG_FATAL_ERR            /* Fatal Errors: Decoding action must be halted */
}JPGERRORLEVELENUM_T;


typedef enum
{
  EIGHT_BY_EIGHT,        /* 8x8 IDCT */
  FOUR_BY_FOUR,            /* 4x4 IDCT */
  TWO_BY_TWO,            /* 2x2 IDCT */
  ONE_BY_ONE            /* 1x1 IDCT */
}IDCT_SIZE_ENUM_T;

typedef enum
{
  FIT_D0,            /* Scale down JPG file to just smaller than display buffer size */
  FIT_D1,            /* Find the just fit scale factor, we further scale down by one level,
                   for example, from 8x8 to 4x4 */
  FIT_D2,            /* Find the just fit scale factor, we further scale down by two level,
                   for example, from 8x8 to 2x2 */
  FIT_D3            /* Find the just fit scale factor, we further scale down by three level,
                   for example, from 8x8 to 1x1 */
}SCALE_DOWN_LEVEL_T;
/*-----------------------------------------------------------------------------
                    functions declarations
 ----------------------------------------------------------------------------*/
#endif /* _JPG_CONFIG_H_ */


