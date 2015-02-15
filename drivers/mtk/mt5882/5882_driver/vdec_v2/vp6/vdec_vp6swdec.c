/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright(c) 2005, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: vdec_vp6.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_vp6dec.c
 *  This file contains implementation of Vp6 sofware decoder.
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "vdec_vp6com.h"
#include "vdec_vp6swdec.h"
#include "vdec_vp6tbl.h"


#ifdef VP6_DEBUG
U32 static _u4Vp6DebugConter=0;
U32 static _u4Vp6DebugCouter2=0;
#endif

extern const unsigned char KRangeToShiftTable[128];
#define UNCOMPILE_UNUSED_FUNCTION

static U32 ReadHeaderBits(FRAME_HEADER *Header, U32 BitsRequired);
static int VP6_DecodeBool(BOOL_CODER *br, int probability);
static int VP6_DecodeBool128(BOOL_CODER*br);
static U32 VP6_bitread(BOOL_CODER *br,int bits );
static void VP6_StartDecode ( BOOL_CODER *br, unsigned char *source, unsigned char *end );
static inline U32 bitread(BITREADER *br, int bits);
static inline U32 bitreadonly(BITREADER *br,U32 bits);
static inline void bitShift(BITREADER *br,int bits);
static inline U32 bitread1(BITREADER *br);
static inline int tDecodeBool128(BOOL_CODER* br);
static int tDecodeBool(int probability, BOOL_CODER* br);
static void VP6_BuildModeTree(PB_INSTANCE* pbi);
static int VP6_decodeModeDiff(PB_INSTANCE *pbi );
static void VP6_DecodeModeProbs(PB_INSTANCE *pbi );
static CODING_MODE VP6_DecodeBlockMode ( PB_INSTANCE *pbi );
static CODING_MODE VP6_DecodeMode ( PB_INSTANCE *pbi, CODING_MODE lastmode, U32 type);
static void VP6_decodeMotionVector(PB_INSTANCE *pbi,MOTION_VECTOR *mv,CODING_MODE Mode);
static BOOL VP6_decodeModeAndMotionVector ( PB_INSTANCE *pbi, U32 MBrow, U32 MBcol );
static void VP6_ConfigureMvEntropyDecoder( PB_INSTANCE *pbi, U8 FrameType);
static void VP6_FindNearestandNextNearest(PB_INSTANCE *pbi,U32 MBrow,U32 MBcol,U8 Frame,int *type);
static void VP6_init_dequantizer ( QUANTIZER *qi, U8 Vp3VersionNo );
static void  VP6_BuildScanOrder( PB_INSTANCE *pbi, U8 *ScanBands );
static void VP6_UpdateQ ( QUANTIZER *qi, U8 Vp3VersionNo );
static void InsertSorted ( sortnode *sn, int node, int *startnode );
static void VP6_BuildHuffTree ( HUFF_NODE *hn, U32 *counts, S32 values );
static void  BoolTreeToHuffCodes ( U8 *BoolTreeProbs, U32 *HuffProbs );
static void ZerosBoolTreeToHuffCodes ( U8 *BoolTreeProbs, U32 *HuffProbs );
static void VP6_ConvertDecodeBoolTrees ( PB_INSTANCE *pbi );
static inline void ResetCoeffBuffer(S16 *coeffsPtr);
static void VP6_ConfigureEntropyDecoder( PB_INSTANCE *pbi, U8 FrameType );
static inline void dequant_slow ( const S16 *dequant_coeffs, const S16 *quantized_list, S32 *DCT_block);
static inline void  dequant_slow10 ( const S16 *dequant_coeffs, const S16 *quantized_list, S32 *DCT_block );
static void IDctSlow ( S16 *InputData, S16 *QuantMatrix, S16 *OutputData );
static void IDct10 ( S16 *InputData, S16 *QuantMatrix, S16 *OutputData );
static void IDct1 ( S16 *InputData, S16 *QuantMatrix, S16 *OutputData );
static void IDCT0_and_Recon_Inter(U8 *CurrFrame, U8 *RefFrame, U32 LineStep);
static void IDCT0_and_Recon_Intra(U8 *CurrFrame, U16 *ReconDataBuffer, U32 LineStep);
static void IDCT0_and_ReconBlock_8(U8 *PredBuffer, U8 *CurrFrame, U32 LineStep);
static void IDCT1_plus_ReconBlock(U8 *PredBuffer, U8 *CurrFrame, S16 OutD, U32 LineStep);
static void IDCT1_plus_ReconInter(U8 *CurrFrame, U8 *RefFrame, S16 OutD, U32 LineStep);
static void ReconIntra_C( S16 *TmpDataBuffer, U8 *ReconPtr, U16 *ChangePtr, U32 LineStep );
static void ReconInter_C( S16 *TmpDataBuffer, U8 *ReconPtr, U8 *RefPtr, S16 *ChangePtr, U32 LineStep );
static void ReconBlock_C_8 ( U8 *SrcBlock, S16 *ReconRefPtr, U8 *DestBlock, U32 LineStep );
static void UnpackBlock_C_8 ( U8 *ReconPtr, U8 *ReconRefPtr, U32 ReconPixelsPerLine );
static U32 Var16Point ( U8 *DataPtr, S32 SourceStride );
static void FilterBlock1d_8(U8 *SrcPtr,U8 *OutputPtr,U32 SrcPixelsPerLine,U32 PixelStep,U32 OutputHeight,U32 OutputWidth,const S32 *Filter);
static void FilterBlock2dFirstPass
(
    U8 *SrcPtr,
    S32 *OutputPtr,
    U32 SrcPixelsPerLine,
    U32 PixelStep,
    U32 OutputHeight,
    U32 OutputWidth,
    const S32 *Filter
);

static void FilterBlock2dSecondPass_8
(
    S32 *SrcPtr,
    U8 *OutputPtr,
    U32 SrcPixelsPerLine,
    U32 PixelStep,
    U32 OutputHeight,
    U32 OutputWidth,
    const S32 *Filter
);

static void FilterBlock2d_8
(
    U8  *SrcPtr,
    U8 *OutputPtr,
    U32 SrcPixelsPerLine,
    const S32  *HFilter,
    const S32  *VFilter,
    S32* fData
);

static void FilterBlock1dBil_8
(
    U8    *SrcPtr,
    U8    *OutputPtr,
    U32  SrcPixelsPerLine,
    U32  PixelStep,
    U32  OutputHeight,
    U32  OutputWidth,
    const S32  *Filter
);

static void FilterBlock2dBil_FirstPass
(
    U8 *SrcPtr,
    S32 *OutputPtr,
    U32 SrcPixelsPerLine,
    U32 PixelStep,
    U32 OutputHeight,
    U32 OutputWidth,
    const S32 *Filter
);

static void FilterBlock2dBil_SecondPass_8
(
    S32 *SrcPtr,
    U8  *OutputPtr,
    U32 SrcPixelsPerLine,
    U32 PixelStep,
    U32 OutputHeight,
    U32 OutputWidth,
    const S32 *Filter
);

static void FilterBlock2dBil_8
(
    U8  *SrcPtr,
    U8 *OutputPtr,
    U32 SrcPixelsPerLine,
    const S32  *HFilter,
    const S32  *VFilter,
    S32* fData
);

static void FilterBlock_C
(
    U8 *ReconPtr1,
    U8 *ReconPtr2,
    U8 *ReconRefPtr,
    U32 PixelsPerLine,
    S32 ModX,
    S32 ModY,
    BOOL UseBicubic,
    U8 BicubicAlpha,
    CVP6Lib* vp6Lib
);

static void VP6_FilteringHoriz_12_C ( U32 QValue, U8 *Src, S32 Pitch, U8 *LimitVal_VP6 );
static void VP6_FilteringVert_12_C ( U32 QValue, U8 *Src, S32 Pitch, U8 *LimitVal_VP6 );
static void Copy12x12_C_OPT
(
    const unsigned char *src, 
    unsigned char *dest, 
    unsigned int srcstride
);

static inline void VP6_PredictFiltered
(
 PB_INSTANCE *pbi,
 U8 *SrcPtr,
 S32 mx,
 S32 my,
 U32 bp,
 U8 *LimitVal_VP6
 );

static void VP6_PredictFilteredBlock 
(
  PB_INSTANCE *pbi,
  U8 *OutputPtr,
  U32 bp,
  CVP6Lib* vp6Lib
);

static void VP6_ResetLeftContext ( PB_INSTANCE *pbi);
static void VP6_ResetAboveContext( PB_INSTANCE *pbi );
static inline int VP6_ExtractTokenN( BITREADER *br, HUFF_NODE *huffNode, U16* hlt);
static void VP6_PredictDC_MB(PB_INSTANCE *pbi);
static void ReadHuffTokensPredictA_MB(PB_INSTANCE *pbi); //, int *EobArray)
static void VP6_ReadTokensPredictA_MB(PB_INSTANCE *pbi) ;//, int *EobArray)
static inline BOOL VP6_DecodeMacroBlock ( PB_INSTANCE *pbi, U32 MBrow, U32 MBcol, CVP6Lib* vp6Lib );
static void InitHeaderBuffer ( FRAME_HEADER *Header, unsigned char *Buffer, unsigned int BufferSize );
static BOOL LoadFrameHeader ( PB_INSTANCE *pbi );
static void VP6_DecodeFrameMbs ( PB_INSTANCE *pbi, CVP6Lib* vp6Lib);
static void VP6_InitMBI(PB_INSTANCE *pbi);
//static void On2YV12_CopyFrame(YV12_BUFFER_CONFIG *srcYBC, YV12_BUFFER_CONFIG *dstYBC, int noUVChannels);
static S8  VP6_DecodeFrameToYUV (PB_INSTANCE *pbi, char *VideoBufferPtr, unsigned int ByteCount, U32 ImageWidth,U32 ImageHeight);
static void VP6_ConfigureContexts(PB_INSTANCE *pbi );
static void  On2YV12_ExtendFrameBorders (YV12_BUFFER_CONFIG *ybf, int noUVChannels);
#ifndef UNCOMPILE_UNUSED_FUNCTION
static void On2YV12_CopyFrame(YV12_BUFFER_CONFIG *srcYBC, YV12_BUFFER_CONFIG *dstYBC, int noUVChannels);
#endif

#ifndef UNCOMPILE_UNUSED_FUNCTION
static inline U32 VP6_bitread1(BOOL_CODER *br);
static int nDecodeBool(BOOL_CODER *br, int probability);
static void  VP6_BuildHuffLookupTable(HUFF_NODE *HuffTreeRoot,  U16 * HuffTable);
static void IDCTSlow_and_ReconBlock_8(S16 *InputData, S16 *QuantMatrix,U8 *PredBuffer, S16 *OutputData, U8 *CurrFrame, U32 LineStep);
static void IDct10_plus_ReconBlock_8(S16 *InputData, S16 *QuantMatrix, U8 *PredBuffer, S16 *OutputData, U8 *CurrFrame, U32 LineStep);
static void AverageBlock_C ( U8 *ReconPtr1, U8 *ReconPtr2, U16 *ReconRefPtr, U32 ReconPixelsPerLine );
static void CopyBlock_C ( unsigned char *src, unsigned char *dest, unsigned int srcstride );
#endif

#ifdef VP6_SWDEC_MTKOPTIMIZE
static U32 ReadHeaderBits ( FRAME_HEADER *Header, U32 BitsRequired)
{
    U32 pos       = Header->pos;
    U32 available = Header->bits_available;
    U32 value     = Header->value;
    U8 *Buffer    = &Header->buffer[pos];
    U32 RetVal    = 0;

    if ( available < BitsRequired )
    {
        // Need more bits from input buffer...
        RetVal = value >> (32-available);
        BitsRequired -= available;
        RetVal <<= BitsRequired;
        value  = (Buffer[0] << 24);
        value |= (Buffer[1] << 16);
        value |= (Buffer[2] <<  8);
        value |= (Buffer[3] <<  0);
        pos += 4;
        available = 32;
    }

    RetVal |= value >> (32-BitsRequired);
    Header->value          = value<<BitsRequired;
    Header->bits_available = available-BitsRequired;
    Header->pos = pos;
    return RetVal;
}

#else
static U32 ReadHeaderBits ( FRAME_HEADER *Header, U32 BitsRequired)
{
    U32 pos       = Header->pos;
    U32 available = Header->bits_available;
    U32 value     = Header->value;
    U8 *Buffer    = &Header->buffer[pos];
    U32 RetVal    = 0;

    if ( available < BitsRequired )
    {
        // Need more bits from input buffer...
        RetVal = value >> (32-available);
        BitsRequired -= available;
        RetVal <<= BitsRequired;

        //value  = (Buffer[0]<<24)+(Buffer[1]<<16)+(Buffer[2]<<8)+(Buffer[3]);
       if ((Buffer + 0) < Header->buffer_end)
          value  = (Buffer[0] << 24);
       if ((Buffer + 1) < Header->buffer_end)
          value |= (Buffer[1] << 16);
       if ((Buffer + 2) < Header->buffer_end)
          value |= (Buffer[2] <<  8);
       if ((Buffer + 3) < Header->buffer_end)
          value |= (Buffer[3] <<  0);
        pos += 4;
        available = 32;
    }

    RetVal |= value >> (32-BitsRequired);
    
    // Update data structure
    Header->value          = value<<BitsRequired;
    Header->bits_available = available-BitsRequired;
    Header->pos = pos;

    return RetVal;
}
#endif

static int VP6_DecodeBool ( BOOL_CODER *br, int probability)
{
   unsigned retVal=0;
   unsigned int range=br->range;
   unsigned int value=br->value;
   int count=br->count;

  // perform the actual decodin
  unsigned int split=1+(((range-1)*probability)>>8);
  if (value>=split<<24)
  {
     retVal=1;
     range-=split;
     value-=split<<24;
  }
  else
  {
     range=split;
  }

  if(range<0x80)
  {
     unsigned int index=KRangeToShiftTable[range];
     range<<=index;
     value<<=index;
     count-=index;
     if(count<= 0) 
     {
        const unsigned char* tempBuffer = br->buffer + br->pos;
        if (tempBuffer < br->buffer_end)
        {
           value |= ((*tempBuffer)<<(-count));
           br->pos++;
        }
        count += 8;
     }
  }

   br->range=range;
   br->value=value;
   br->count=count;
   return(retVal);
}

static int VP6_DecodeBool128(BOOL_CODER*br)
{
    unsigned retVal=0;
    unsigned int range=br->range;
    unsigned int value=br->value;
    int count=br->count;
        
    // perform the actual decoding
    unsigned int split=(range+1)>>1;
    if (value>=(split<<24))
    {
        retVal=1;
        range-=split;
        value-=(split<<24);
        }    
    else
        range=split;

    range<<=1;
    value<<=1;

    if (--count==0)
    {
        count=8;
        const unsigned char* tempBuffer = br->buffer + br->pos;
        if (tempBuffer < br->buffer_end)
        {
            value |= *tempBuffer;
            br->pos++;
        }        
    }

    br->range=range;
    br->value=value;
    br->count=count;

    return(retVal);
}

static U32 VP6_bitread ( BOOL_CODER *br, int bits )
{
    U32 z = 0;
    int bit;
    for ( bit=bits-1; bit>=0; bit-- )
    {
        z |= (VP6_DecodeBool128(br)<<bit);
    }
    return z;
}

#ifndef UNCOMPILE_UNUSED_FUNCTION
static inline U32 VP6_bitread1 ( BOOL_CODER *br ) 
{
    return (VP6_DecodeBool128(br));
}

static int nDecodeBool ( BOOL_CODER *br, int probability )
{
    unsigned retVal=0;
    unsigned int range=br->range;
    unsigned int value=br->value;
    int count=br->count;

    // perform the actual decoding
    unsigned int split=1+(((range-1)*probability)>>8);
    if (value>=split<<24)
    {
        retVal=1;
        range-=split;
        value-=split<<24;
    }
    else
        range=split;

    if (range<0x80)
    {
        unsigned int index=KRangeToShiftTable[range];
        range<<=index;
        value<<=index;
        count-=index;
        if (count<=0)
        {
            value|=(br->buffer[br->pos])<<(-count);
            br->pos++;
            count+=8;
        }
    }

    br->range=range;
    br->value=value;
    br->count=count;

    return(retVal);
}

static void  VP6_BuildHuffLookupTable(HUFF_NODE *HuffTreeRoot,  U16 * HuffTable)
{
    short *h= (short *) HuffTreeRoot;    

    int i, bits,j;
    unsigned int nodeptr;

    for(i = 0; i < (1 << HUFF_LUT_LEVELS); i++)
    {
        bits = i;        
        nodeptr=0;
        j=0;

        do
        {
            nodeptr += (bits >> j) & 1;
            j++;
            nodeptr = ((unsigned int )h[nodeptr]);
        
        } while(!(nodeptr & 1) && (j < HUFF_LUT_LEVELS));
        
        HuffTable[i] = nodeptr | (j << 13);

    }
}


static void IDCTSlow_and_ReconBlock_8(S16 *InputData, S16 *QuantMatrix,U8 *PredBuffer, S16 *OutputData, U8 *CurrFrame, U32 LineStep)
{
    int   loop;
    S32 t1, t2;
    S32 IntermediateData[64];
    S32 A, B, C, D, Ad, Bd, Cd, Dd, E, F, G, H;
    S32 Ed, Gd, Add, Bdd, Fd, Hd;

    S32 *ip = IntermediateData;
    U8 *lPredBuffer = PredBuffer;
    
    U8    *lCurrFramePtr;

    // dequantize the input
    dequant_slow ( QuantMatrix, InputData, IntermediateData );


    // Inverse DCT on the rows now
    for ( loop=0; loop<8; loop++ )
    {
        // Check for non-zero values
        if ( ip[0] | ip[1] | ip[2] | ip[3] | ip[4] | ip[5] | ip[6] | ip[7] )
        {

            t1 = (S32)(xC1S7 * ip[1]);
            t2 = (S32)(xC7S1 * ip[7]);
            t1 >>= 16;
            t2 >>= 16;
            A = t1 + t2;

            t1 = (S32)(xC7S1 * ip[1]);
            t2 = (S32)(xC1S7 * ip[7]);
            t1 >>= 16;
            t2 >>= 16;
            B = t1 - t2;

            t1 = (S32)(xC3S5 * ip[3]);
            t2 = (S32)(xC5S3 * ip[5]);
            t1 >>= 16;
            t2 >>= 16;
            C = t1 + t2;

            t1 = (S32)(xC3S5 * ip[5]);
            t2 = (S32)(xC5S3 * ip[3]);
            t1 >>= 16;
            t2 >>= 16;
            D = t1 - t2;

            t1 = (S32)(xC4S4 * (A - C));
            t1 >>= 16;
            Ad = t1;

            t1 = (S32)(xC4S4 * (B - D));
            t1 >>= 16;
            Bd = t1;

            Cd = A + C;
            Dd = B + D;

            t1 = (S32)(xC4S4 * (ip[0] + ip[4]));
            t1 >>= 16;
            E = t1;

            t1 = (S32)(xC4S4 * (ip[0] - ip[4]));
            t1 >>= 16;
            F = t1;

            t1 = (S32)(xC2S6 * ip[2]);
            t2 = (S32)(xC6S2 * ip[6]);
            t1 >>= 16;
            t2 >>= 16;
            G = t1 + t2;

            t1 = (S32)(xC6S2 * ip[2]);
            t2 = (S32)(xC2S6 * ip[6]);
            t1 >>= 16;
            t2 >>= 16;
            H = t1 - t2;

            Ed = E - G;
            Gd = E + G;

            Add = F + Ad;
            Bdd = Bd - H;

            Fd = F - Ad;
            Hd = Bd + H;

            // Final sequence of operations over-write original inputs.
            ip[0] = (S16)((Gd + Cd )  >> 0);
            ip[7] = (S16)((Gd - Cd )  >> 0);

            ip[1] = (S16)((Add + Hd ) >> 0);
            ip[2] = (S16)((Add - Hd ) >> 0);

            ip[3] = (S16)((Ed + Dd )  >> 0);
            ip[4] = (S16)((Ed - Dd )  >> 0);

            ip[5] = (S16)((Fd + Bdd ) >> 0);
            ip[6] = (S16)((Fd - Bdd ) >> 0);
        }

        ip += 8;            /* next row */
    }

    ip = IntermediateData;

    for ( loop=0; loop<8; loop++ )
    {

        // Check for non-zero values (bitwise | faster than logical ||)
        if ( ip[0 * 8] | ip[1 * 8] | ip[2 * 8] | ip[3 * 8] |
             ip[4 * 8] | ip[5 * 8] | ip[6 * 8] | ip[7 * 8] )
        {

            t1 = (S32)(xC1S7 * ip[1*8]);
            t2 = (S32)(xC7S1 * ip[7*8]);
            t1 >>= 16;
            t2 >>= 16;
            A = t1 + t2;

            t1 = (S32)(xC7S1 * ip[1*8]);
            t2 = (S32)(xC1S7 * ip[7*8]);
            t1 >>= 16;
            t2 >>= 16;
            B = t1 - t2;

            t1 = (S32)(xC3S5 * ip[3*8]);
            t2 = (S32)(xC5S3 * ip[5*8]);
            t1 >>= 16;
            t2 >>= 16;
            C = t1 + t2;

            t1 = (S32)(xC3S5 * ip[5*8]);
            t2 = (S32)(xC5S3 * ip[3*8]);
            t1 >>= 16;
            t2 >>= 16;
            D = t1 - t2;

            t1 = (S32)(xC4S4 * (A - C));
            t1 >>= 16;
            Ad = t1;

            t1 = (S32)(xC4S4 * (B - D));
            t1 >>= 16;
            Bd = t1;

            Cd = A + C;
            Dd = B + D;

            t1 = (S32)(xC4S4 * (ip[0*8] + ip[4*8]));
            t1 >>= 16;
            E = t1;

            t1 = (S32)(xC4S4 * (ip[0*8] - ip[4*8]));
            t1 >>= 16;
            F = t1;

            t1 = (S32)(xC2S6 * ip[2*8]);
            t2 = (S32)(xC6S2 * ip[6*8]);
            t1 >>= 16;
            t2 >>= 16;
            G = t1 + t2;

            t1 = (S32)(xC6S2 * ip[2*8]);
            t2 = (S32)(xC2S6 * ip[6*8]);
            t1 >>= 16;
            t2 >>= 16;
            H = t1 - t2;

            Ed = E - G;
            Gd = E + G;

            Add = F + Ad;
            Bdd = Bd - H;

            Fd = F - Ad;
            Hd = Bd + H;

            Gd += IdctAdjustBeforeShift;
            Add += IdctAdjustBeforeShift;
            Ed += IdctAdjustBeforeShift;
            Fd += IdctAdjustBeforeShift;

            // Final sequence of operations over-write original inputs.

            lCurrFramePtr = (U8 *)CurrFrame;

            lCurrFramePtr[0] = LIMIT(((S16)((Gd + Cd )  >> 4) + (U8)lPredBuffer[0]));
            
            lCurrFramePtr = CurrFrame + (LineStep);
            lCurrFramePtr[0] = LIMIT(((S16)((Add + Hd ) >> 4) + (U8)lPredBuffer[8]));


            lCurrFramePtr += LineStep;
            lCurrFramePtr[0] = LIMIT(((S16)((Add - Hd ) >> 4) + (U8)lPredBuffer[16]));


            lCurrFramePtr += LineStep;
            lCurrFramePtr[0] = LIMIT(((S16)((Ed + Dd )  >> 4) + (U8)lPredBuffer[24]));


            lCurrFramePtr += LineStep;
            lCurrFramePtr[0] = LIMIT(((S16)((Ed - Dd )  >> 4) + (U8)lPredBuffer[32]));
            

            lCurrFramePtr += LineStep;
            lCurrFramePtr[0] = LIMIT(((S16)((Fd + Bdd ) >> 4) + (U8)lPredBuffer[40]));


            lCurrFramePtr += LineStep;
            lCurrFramePtr[0] = LIMIT(((S16)((Fd - Bdd ) >> 4) + (U8)lPredBuffer[48]));


            lCurrFramePtr += LineStep;

            lCurrFramePtr[0] = LIMIT(((S16)((Gd - Cd )  >> 4) + (U8)lPredBuffer[56]));

        }
        else
        {


            lCurrFramePtr =(U8 *)CurrFrame;

            lCurrFramePtr[0]    = (U8)(lPredBuffer[0]);
            

            lCurrFramePtr += LineStep;
            lCurrFramePtr[0]    = (U8)(lPredBuffer[8]);
            

            lCurrFramePtr += LineStep;
            lCurrFramePtr[0]    = (U8)(lPredBuffer[16]);


            lCurrFramePtr += LineStep;
            lCurrFramePtr[0]    = (U8)(lPredBuffer[24]);


            lCurrFramePtr += LineStep;
            lCurrFramePtr[0]    = (U8)(lPredBuffer[32]);


            lCurrFramePtr += LineStep;
            lCurrFramePtr[0]    = (U8)(lPredBuffer[40]);


            lCurrFramePtr += LineStep;
            lCurrFramePtr[0]    = (U8)(lPredBuffer[48]);


            lCurrFramePtr += LineStep;
            lCurrFramePtr[0]    = (U8)(lPredBuffer[56]);

        }
        ip++;            // next column
        CurrFrame++;
        lPredBuffer++;
    }

}

static void IDct10_plus_ReconBlock_8(S16 *InputData, S16 *QuantMatrix, U8 *PredBuffer, S16 *OutputData, U8 *CurrFrame, U32 LineStep)
{
    int   loop;
    S32 t1, t2;
    S32 IntermediateData[64];
    S32 A, B, C, D, Ad, Bd, Cd, Dd, E, F, G, H;
    S32 Ed, Gd, Add, Bdd, Fd, Hd;

    S32 *ip = IntermediateData;

    U8 *lPredBuffer = PredBuffer;
    
    U8    *lCurrFramePtr;

    // dequantize the input
    dequant_slow10 ( QuantMatrix, InputData, IntermediateData );


    // Inverse DCT on the rows now
    for ( loop=0; loop<4; loop++ )
    {
        // Check for non-zero values
        if ( ip[0] | ip[1] | ip[2] | ip[3] )
        {
            t1 = (S32)(xC1S7 * ip[1]);
            t1 >>= 16;
            A = t1;

            t1 = (S32)(xC7S1 * ip[1]);
            t1 >>= 16;
            B = t1 ;

            t1 = (S32)(xC3S5 * ip[3]);
            t1 >>= 16;
            C = t1;

            t2 = (S32)(xC5S3 * ip[3]);
            t2 >>= 16;
            D = -t2;

            t1 = (S32)(xC4S4 * (A - C));
            t1 >>= 16;
            Ad = t1;

            t1 = (S32)(xC4S4 * (B - D));
            t1 >>= 16;
            Bd = t1;

            Cd = A + C;
            Dd = B + D;

            t1 = (S32)(xC4S4 * ip[0] );
            t1 >>= 16;
            E = t1;

            F = t1;

            t1 = (S32)(xC2S6 * ip[2]);
            t1 >>= 16;
            G = t1;

            t1 = (S32)(xC6S2 * ip[2]);
            t1 >>= 16;
            H = t1 ;

            Ed = E - G;
            Gd = E + G;

            Add = F + Ad;
            Bdd = Bd - H;

            Fd = F - Ad;
            Hd = Bd + H;

            // Final sequence of operations over-write original inputs.
            ip[0] = (S16)((Gd + Cd )   >> 0);
            ip[7] = (S16)((Gd - Cd )   >> 0);

            ip[1] = (S16)((Add + Hd )  >> 0);
            ip[2] = (S16)((Add - Hd )  >> 0);

            ip[3] = (S16)((Ed + Dd )   >> 0);
            ip[4] = (S16)((Ed - Dd )   >> 0);

            ip[5] = (S16)((Fd + Bdd )  >> 0);
            ip[6] = (S16)((Fd - Bdd )  >> 0);
        }

        ip += 8;            /* next row */
    }

    ip = IntermediateData;

    for ( loop=0; loop<8; loop++ )
    {
        // Check for non-zero values (bitwise or faster than ||)
        if ( ip[0 * 8] | ip[1 * 8] | ip[2 * 8] | ip[3 * 8] )
        {
            t1 = (S32)(xC1S7 * ip[1*8]);
            t1 >>= 16;
            A = t1 ;

            t1 = (S32)(xC7S1 * ip[1*8]);
            t1 >>= 16;
            B = t1 ;

            t1 = (S32)(xC3S5 * ip[3*8]);
            t1 >>= 16;
            C = t1 ;

            t2 = (S32)(xC5S3 * ip[3*8]);
            t2 >>= 16;
            D = - t2;

            t1 = (S32)(xC4S4 * (A - C));
            t1 >>= 16;
            Ad = t1;

            t1 = (S32)(xC4S4 * (B - D));
            t1 >>= 16;
            Bd = t1;

            Cd = A + C;
            Dd = B + D;

            t1 = (S32)(xC4S4 * ip[0*8]);
            t1 >>= 16;
            E = t1;
            F = t1;

            t1 = (S32)(xC2S6 * ip[2*8]);
            t1 >>= 16;
            G = t1;

            t1 = (S32)(xC6S2 * ip[2*8]);
            t1 >>= 16;
            H = t1;

            Ed = E - G;
            Gd = E + G;

            Add = F + Ad;
            Bdd = Bd - H;

            Fd = F - Ad;
            Hd = Bd + H;

            Gd += IdctAdjustBeforeShift;
            Add += IdctAdjustBeforeShift;
            Ed += IdctAdjustBeforeShift;
            Fd += IdctAdjustBeforeShift;

            // Final sequence of operations over-write original inputs.

            lCurrFramePtr =(U8 *)CurrFrame;

            lCurrFramePtr[0] = LIMIT(((S16)((Gd + Cd )  >> 4) + (U8)lPredBuffer[0]));
            
            lCurrFramePtr += LineStep;
            lCurrFramePtr[0] = LIMIT(((S16)((Add + Hd ) >> 4) + (U8)lPredBuffer[8]));

            lCurrFramePtr += LineStep;
            lCurrFramePtr[0] = LIMIT(((S16)((Add - Hd ) >> 4) + (U8)lPredBuffer[16]));
            
            lCurrFramePtr += LineStep;
            lCurrFramePtr[0] = LIMIT(((S16)((Ed + Dd )  >> 4) + (U8)lPredBuffer[24]));

            lCurrFramePtr += LineStep;
            lCurrFramePtr[0] = LIMIT(((S16)((Ed - Dd )  >> 4) + (U8)lPredBuffer[32]));

            lCurrFramePtr += LineStep;
            lCurrFramePtr[0] = LIMIT(((S16)((Fd + Bdd ) >> 4) + (U8)lPredBuffer[40]));

            lCurrFramePtr += LineStep;
            lCurrFramePtr[0] = LIMIT(((S16)((Fd - Bdd ) >> 4) + (U8)lPredBuffer[48]));

            lCurrFramePtr += LineStep;
            lCurrFramePtr[0] = LIMIT(((S16)((Gd - Cd )  >> 4) + (U8)lPredBuffer[56]));

        }
        else
        {
            lCurrFramePtr =(U8 *)CurrFrame;

            lCurrFramePtr[0]    = (U8)(lPredBuffer[0]);
            
            lCurrFramePtr += LineStep;
            lCurrFramePtr[0]    = (U8)(lPredBuffer[8]);
            
            lCurrFramePtr += LineStep;
            lCurrFramePtr[0]    = (U8)(lPredBuffer[16]);

            lCurrFramePtr += LineStep;
            lCurrFramePtr[0]    = (U8)(lPredBuffer[24]);

            lCurrFramePtr += LineStep;
            lCurrFramePtr[0]    = (U8)(lPredBuffer[32]);

            lCurrFramePtr += LineStep;
            lCurrFramePtr[0]    = (U8)(lPredBuffer[40]);

            lCurrFramePtr += LineStep;
            lCurrFramePtr[0]    = (U8)(lPredBuffer[48]);

            lCurrFramePtr += LineStep;
            lCurrFramePtr[0]    = (U8)(lPredBuffer[56]);
    
        }

        ip++;    // next column
        CurrFrame++;
        lPredBuffer++;
    }
}

static void AverageBlock_C ( U8 *ReconPtr1, U8 *ReconPtr2, U16 *ReconRefPtr, U32 ReconPixelsPerLine )
{

    U32 i;

    // For each block row
    for ( i=0; i<BLOCK_HEIGHT_WIDTH; i++ )
    {
        ReconRefPtr[0] = (S16)(((S32)(ReconPtr1[0]) + ((S32)ReconPtr2[0]))>>1);
        ReconRefPtr[1] = (S16)(((S32)(ReconPtr1[1]) + ((S32)ReconPtr2[1]))>>1);
        ReconRefPtr[2] = (S16)(((S32)(ReconPtr1[2]) + ((S32)ReconPtr2[2]))>>1);
        ReconRefPtr[3] = (S16)(((S32)(ReconPtr1[3]) + ((S32)ReconPtr2[3]))>>1);
        ReconRefPtr[4] = (S16)(((S32)(ReconPtr1[4]) + ((S32)ReconPtr2[4]))>>1);
        ReconRefPtr[5] = (S16)(((S32)(ReconPtr1[5]) + ((S32)ReconPtr2[5]))>>1);
        ReconRefPtr[6] = (S16)(((S32)(ReconPtr1[6]) + ((S32)ReconPtr2[6]))>>1);
        ReconRefPtr[7] = (S16)(((S32)(ReconPtr1[7]) + ((S32)ReconPtr2[7]))>>1);
        
        // Start next row
        ReconPtr1 += ReconPixelsPerLine;
        ReconPtr2 += ReconPixelsPerLine;

        ReconRefPtr += BLOCK_HEIGHT_WIDTH;
    }
}

static void CopyBlock_C ( unsigned char *src, unsigned char *dest, unsigned int srcstride )
{
    int j;
    unsigned char *s = src;
    unsigned char *d = dest;
    unsigned int stride = srcstride;

    j=8;
    do 
    {
        U32 s1, s2;
        s1 = *((U32*)s); 
        s2 = *((U32*)++s); 
        s += stride;
        *((U32*)d) = s1; 
        *((U32*)++d) = s2; 
        d += stride;
    }
    while (--j);
}

#endif

static void VP6_StartDecode ( BOOL_CODER *br, unsigned char *source, unsigned char *end )
{
    br->lowvalue = 0;
    br->range    = 255;
    br->count    = 8;
    br->buffer   = source;
    br->buffer_end = end;
    br->pos      = 0;
    //br->value    = (br->buffer[0]<<24)+(br->buffer[1]<<16)+(br->buffer[2]<<8)+(br->buffer[3]);
     if ((br->buffer + 0) < end)
         br->value  = (br->buffer[0] << 24);
     if ((br->buffer + 1) < end)
         br->value |= (br->buffer[1] << 16);
     if ((br->buffer + 2) < end)
        br->value |= (br->buffer[2] <<  8);
     if ((br->buffer + 3) < end)
         br->value |= (br->buffer[3] <<  0);
    br->pos     += 4;
}

static inline U32 bitread(BITREADER *br, int bits)
//
// OPTIMISED VERSION.
//
    {
    U32 z=0;
    int bitsinremainder=br->bitsinremainder;
    U32 remainder=br->remainder;

    remainder&=loMaskTbl_VP60[bitsinremainder];
    bits-=bitsinremainder;
    if (bits>0)
        {
        z=remainder<<bits;
        NextWord(remainder)
        bits-=32;
        }

    bitsinremainder=-bits;
    br->bitsinremainder=bitsinremainder;
    br->remainder=remainder;

    return(z | remainder >> bitsinremainder);
}


static inline U32 bitreadonly(BITREADER *br,U32 bits)
//
// OPTIMISED VERSION.
//
    {
    int x=br->bitsinremainder;
    U32 z=(1<<x)-1;
    int shiftRightBy=x-bits;

    z&=br->remainder;
    if (shiftRightBy<0)
        {
        shiftRightBy+=8;
        z<<=8;
        if (br->position < br->end)
            z|=br->position[0];
        }

    return(z>>shiftRightBy);
    }

static inline void bitShift(BITREADER *br,int bits)
//
// OPTIMISED VERSION.
//
    {
    int bitsinremainder=br->bitsinremainder-bits;

    if (bitsinremainder<0)
        {
        bitsinremainder+=32;
        NextWord(br->remainder)
        }

    br->bitsinremainder=bitsinremainder;
    }

static inline U32 bitread1(BITREADER *br)
    {
    int bitsinremainder=br->bitsinremainder;
    U32 remainder=br->remainder;

    if (bitsinremainder--==0)
        {
        NextWord(remainder)
        bitsinremainder=31;
        }

    br->bitsinremainder=bitsinremainder;
    br->remainder=remainder;

    return((remainder>>bitsinremainder)&1);
}

static inline int tDecodeBool128(BOOL_CODER* br)
 {
    int retVal=1;
    unsigned int range=br->range;
    unsigned int value=br->value;
    int count=br->count;
        
    // perform the actual decoding
    unsigned int split=(range+1)>>1;
    if (value>=(split<<24))
        {
            retVal=-1;
        range-=split;
        value-=(split<<24);
        }
    else
        range=split;

    range<<=1;
    value<<=1;

    if (--count==0)
        {
        count=8;
        if (br->buffer < br->buffer_end)
        value|=*br->buffer++;
        }

    br->range=range;
    br->value=value;
    br->count=count;

    return(retVal);
}


static int tDecodeBool(int probability, BOOL_CODER* br)
    {
    unsigned int retVal=0;
    unsigned int range=br->range;
    unsigned int value=br->value;
    int count=br->count;

    // perform the actual decoding
    unsigned int split=1+(((range-1)*probability)>>8);
    if (value>=split<<24)
    {
        retVal=1;
        range-=split;
        value-=split<<24;
    }
    else
        range=split;

    if (range<0x80)
        {
        unsigned int index=KRangeToShiftTable[range];
        range<<=index;
        value<<=index;
        count-=index;
        if (count<=0)
            {
            if (br->buffer < br->buffer_end)
                value|=(*br->buffer++)<<(-count);
            count+=8;
            }
        }

    br->range=range;
    br->value=value;
    br->count=count;

    return(retVal);
}



//************************Decode Mode**********************************//

static void VP6_BuildModeTree(PB_INSTANCE* pbi)
{
    U32 t[MAX_MODES],k,j,i;
    U8* pDest=&pbi->probMode[0][0][0];
    // set up the probabilities for each tree
    for(k=0; k<MODETYPES; k++)
        {
        U32 total=0;
        U8* pProbXmitted=&pbi->probXmitted[k][0][0];

           for(j=0; j<10; j++ )
            {
            const U32 data=100*(*pProbXmitted++);
               total+=data;
               t[j]=data;
            }

        U32 lastValue=t[0];
        total-=t[0];
        t[0]=0;

        U8* pCopySrc2=&pbi->probMode[k][1][0];
        U8* pCopySrc1=&pbi->probMode[k][0][0];

        // create a huffman tree and code array for each of our modes 
        // Note: each of the trees is minus the node give by probmodesame
        for (i=0; i<10; i++ )
            {
            // Move the total's zero "bubble" along.
            if (i>=1)
                {
                t[i-1]=lastValue;
                total+=lastValue;
                total-=t[i];
                lastValue=t[i];
                t[i]=0;
                }

            const U8 prob=*pProbXmitted++;
            pbi->probModeSame[k][i]=256-FUNC(
                prob,
                prob+*(pProbXmitted-(MAX_MODES+1)));

            // each branch is basically calculated via 
            // summing all posibilities at that branch.
            *pDest++=FUNC(t[CODE_INTER_NO_MV]+t[CODE_INTER_PLUS_MV]+t[CODE_INTER_NEAREST_MV]+t[CODE_INTER_NEAR_MV],total);

            // Check if we need to update the results of this division or
            // copy a previously calculated version.
            const U32 performWorkMask=1<<i;
            if (performWorkMask&0x01F)
                {
                *pDest++=FUNC(t[CODE_INTER_NO_MV]+t[CODE_INTER_PLUS_MV], 
                    t[CODE_INTER_NO_MV]+t[CODE_INTER_PLUS_MV]+t[CODE_INTER_NEAREST_MV]+t[CODE_INTER_NEAR_MV]);
                }
            else
                *pDest++=pCopySrc2[1];

            // Check if we need to update the results of this division or
            // copy a previously calculated version.
            if (performWorkMask&0x3E3)
                {
                *pDest++=FUNC(
                    t[CODE_INTRA]+t[CODE_INTER_FOURMV],
                    t[CODE_INTRA]+t[CODE_INTER_FOURMV]+t[CODE_USING_GOLDEN]+t[CODE_GOLDEN_MV]+t[CODE_GOLD_NEAREST_MV]+t[CODE_GOLD_NEAR_MV]);
                }
            else
                *pDest++=pCopySrc1[2];
            
            // Check if we need to update the results of this division or
            // copy a previously calculated version.
            if (performWorkMask&0x007)
                *pDest++=FUNC(t[CODE_INTER_NO_MV],t[CODE_INTER_NO_MV]+t[CODE_INTER_PLUS_MV]);
            else
               *pDest++=pCopySrc2[3];

            // Check if we need to update the results of this division or
            // copy a previously calculated version.
            if (performWorkMask&0x019)
                *pDest++=FUNC(t[CODE_INTER_NEAREST_MV],t[CODE_INTER_NEAREST_MV]+t[CODE_INTER_NEAR_MV]);
            else
                *pDest++=pCopySrc1[4];

            // Check if we need to update the results of this division or
            // copy a previously calculated version.
            if (performWorkMask&0x083)
                *pDest++=FUNC(t[CODE_INTRA],t[CODE_INTRA]+t[CODE_INTER_FOURMV]);
            else
                *pDest++=pCopySrc1[5];

            // Check if we need to update the results of this division or
            // copy a previously calculated version.
            if (performWorkMask&0x361)
                {
                *pDest++=FUNC(t[CODE_USING_GOLDEN]+t[CODE_GOLDEN_MV], 
                    t[CODE_USING_GOLDEN]+t[CODE_GOLDEN_MV]+t[CODE_GOLD_NEAREST_MV]+t[CODE_GOLD_NEAR_MV]);
                }
            else
                *pDest++=pCopySrc1[6];

            // Check if we need to update the results of this division or
            // copy a previously calculated version.
            if (performWorkMask&0x061)
                *pDest++=FUNC(t[CODE_USING_GOLDEN],t[CODE_USING_GOLDEN]+t[CODE_GOLDEN_MV]);
            else
                *pDest++=pCopySrc1[7];

            // Check if we need to update the results of this division or
            // copy a previously calculated version.
            if (performWorkMask&0x301)
                *pDest++=FUNC(t[CODE_GOLD_NEAREST_MV],t[CODE_GOLD_NEAREST_MV]+t[CODE_GOLD_NEAR_MV]);
            else
                *pDest++=pCopySrc1[8];
            }
        }
}


static int VP6_decodeModeDiff ( PB_INSTANCE *pbi )
{
    int sign;

    if ( VP6_DecodeBool(&pbi->br, 205) == 0 )
        return 0;
    
    sign = 1 + -2 * VP6_DecodeBool128(&pbi->br);
    
    if( !VP6_DecodeBool(&pbi->br,171) )
    {
        return sign<<(3-VP6_DecodeBool(    &pbi->br,83));
    }
    else
    {
        if( !VP6_DecodeBool(    &pbi->br,199) ) 
        {
            if(VP6_DecodeBool(    &pbi->br,140))
                return sign * 12;

            if(VP6_DecodeBool(    &pbi->br,125))
                return sign * 16;

            if(VP6_DecodeBool(    &pbi->br,104))
                return sign * 20;

            return sign * 24;
        }
        else 
        {
            int diff = VP6_bitread(&pbi->br,7);
            return sign * diff * 4;
        }
    }
}


static void VP6_DecodeModeProbs ( PB_INSTANCE *pbi )
{
    int i,j;

    // For each mode type (all modes available, no nearest, no near mode)
    for ( j=0; j<MODETYPES; j++ )
    {
        // determine whether we are sending a vector for this mode byte
        if ( VP6_DecodeBool( &pbi->br, PROBVECTORXMIT ) )
        {
            // figure out which vector we have encoded
            int whichVector = VP6_bitread(&pbi->br, 4);

            // adjust the vector
            for ( i=0; i<MAX_MODES; i++ )
            {
                pbi->probXmitted[j][1][i] = VP6_ModeVq[j][whichVector][i*2];
                pbi->probXmitted[j][0][i] = VP6_ModeVq[j][whichVector][i*2+1];
            }
        } 

        // decode whether updates to bring it closer to ideal 
        if ( VP6_DecodeBool( &pbi->br, PROBIDEALXMIT) )
        {
            for ( i=0; i<10; i++ )
            {
                int diff;

                // determine difference 
                diff = VP6_decodeModeDiff(pbi);
                diff += pbi->probXmitted[j][1][i];

                pbi->probXmitted[j][1][i] = ( diff<0 ? 0 : (diff>255?255:diff) );

                // determine difference 
                diff = VP6_decodeModeDiff(pbi);
                diff += pbi->probXmitted[j][0][i];

                pbi->probXmitted[j][0][i] = ( diff<0 ? 0 : (diff>255?255:diff) );

            }
        }
    }
    
    VP6_BuildModeTree(pbi);
}

static CODING_MODE VP6_DecodeBlockMode ( PB_INSTANCE *pbi )
{
    int choice = VP6_DecodeBool128(&pbi->br)<<1;
    
       choice += VP6_DecodeBool128(&pbi->br);

    switch ( choice )
    {
        case 0: return CODE_INTER_NO_MV;        // 0
        case 1: return CODE_INTER_PLUS_MV;      // 2
        case 2: return CODE_INTER_NEAREST_MV;   // 3
        case 3: return CODE_INTER_NEAR_MV;      // 4
    }
    return (CODING_MODE)0;
}   


static CODING_MODE VP6_DecodeMode ( PB_INSTANCE *pbi, CODING_MODE lastmode, U32 type )
{
    CODING_MODE    mode;

    if ( VP6_DecodeBool(&pbi->br,pbi->probModeSame[type][lastmode]) )
    {
        mode = lastmode;
    }
    else
    {   // 0
        U8 *Stats =pbi->probMode[type][lastmode]; 
        
        if ( VP6_DecodeBool(&pbi->br,Stats[0]) )
        {   // 2
            if ( VP6_DecodeBool(&pbi->br,Stats[2]) )
            {   //6
                if ( VP6_DecodeBool(&pbi->br,Stats[6]) )
                {  // 8
                    mode = (CODING_MODE)(CODE_GOLD_NEAREST_MV + VP6_DecodeBool(&pbi->br,Stats[8]));
                }
                else
                {  // 7
                    mode = (CODING_MODE)(CODE_USING_GOLDEN + VP6_DecodeBool(&pbi->br,Stats[7]));
                }
            }
            else
            {   //5
                mode = CODE_INTRA;
                if ( VP6_DecodeBool(&pbi->br,Stats[5]) )
                {
                    mode = CODE_INTER_FOURMV;
                }
            }
        }
        else
        {   // 1
            if ( VP6_DecodeBool(&pbi->br,Stats[1]) )
            {   // 4
                mode = (CODING_MODE)(CODE_INTER_NEAREST_MV + VP6_DecodeBool(&pbi->br,Stats[4]));
            }
            else
            {   // 3
                mode = (CODING_MODE)(CODE_INTER_NO_MV + 2 * VP6_DecodeBool(&pbi->br,Stats[3]));
            }
        }
    }
    return mode;
}

static void VP6_decodeMotionVector(PB_INSTANCE *pbi,MOTION_VECTOR *mv,CODING_MODE Mode)
{
    U32 i;
    S32  Vector = 0;
    S32  SignBit = 0;
    S32  MvOffsetX = 0;
    S32  MvOffsetY = 0;

    // Work out how the MV was coded so that the appropriate origin offset can be applied
    if ( Mode == CODE_INTER_PLUS_MV )
    {    
        // Normal Inter MV
        if ( pbi->mbi.NearestMvIndex < MAX_NEAREST_ADJ_INDEX )
        {
            MvOffsetX = pbi->mbi.NearestInterMVect.x;
            MvOffsetY = pbi->mbi.NearestInterMVect.y;
        }
    }
    else
    {    
        // Golden Frame MV
        if ( pbi->mbi.NearestGMvIndex < MAX_NEAREST_ADJ_INDEX )
        {
            MvOffsetX = pbi->mbi.NearestGoldMVect.x;
            MvOffsetY = pbi->mbi.NearestGoldMVect.y;
        }
    }

    for ( i = 0; i < 2; i++ )
    {
        Vector = 0;

        // Is the vector a small vector or a large vector
        if ( !VP6_DecodeBool(&pbi->br, pbi->IsMvShortProb[i]) )
        {
            // Small magnitude vector
            if ( VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][0] ) )
            {
                Vector += (1 << 2);
                if ( VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][4]) )
                {
                    Vector += (1 << 1);
                    Vector += VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][6]);
                }
                else
                {
                    Vector += VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][5]);
                }
            }
            else
            {
                if ( VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][1]) )
                {
                    Vector += (1 << 1);
                    Vector += VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][3]);
                }
                else
                {
                    Vector = VP6_DecodeBool(&pbi->br, pbi->MvShortProbs[i][2]);
                }
            }
        }
        else
        {
            // Large magnitude vector
            Vector = VP6_DecodeBool( &pbi->br, pbi->MvSizeProbs[i][0] );
            Vector += (VP6_DecodeBool( &pbi->br, pbi->MvSizeProbs[i][1] ) << 1);
            Vector += (VP6_DecodeBool( &pbi->br, pbi->MvSizeProbs[i][2] ) << 2);

            Vector += (VP6_DecodeBool( &pbi->br, pbi->MvSizeProbs[i][7] ) << 7);
            Vector += (VP6_DecodeBool( &pbi->br, pbi->MvSizeProbs[i][6] ) << 6);
            Vector += (VP6_DecodeBool( &pbi->br, pbi->MvSizeProbs[i][5] ) << 5);
            Vector += (VP6_DecodeBool( &pbi->br, pbi->MvSizeProbs[i][4] ) << 4);

            // If none of the higher order bits are set then this bit is implicit
            if ( Vector & 0xF0 )
                Vector += (VP6_DecodeBool( &pbi->br, pbi->MvSizeProbs[i][3] ) << 3);
            else
                Vector += 0x08;
        }

        // Read the sign bit if needed.
        if ( Vector != 0 )
        {
            SignBit = VP6_DecodeBool(&pbi->br, pbi->MvSignProbs[i]);

            if ( SignBit )
                Vector = -Vector;
        }

        if ( i )
            mv->y = (short)(Vector + MvOffsetY);
        else
            mv->x = (short)(Vector + MvOffsetX);
    }
}


static BOOL VP6_decodeModeAndMotionVector ( PB_INSTANCE *pbi, U32 MBrow, U32 MBcol )
{
    int type;
    int x, y;
    U32 k;
    CODING_MODE mode;   //lastmode;
    MOTION_VECTOR mv;
       BLOCK_DX_INFO * bdi;
     int QpelX, QpelY;

    VP6_FindNearestandNextNearest(pbi,MBrow,MBcol,1,&type);
    mode =     VP6_DecodeMode(pbi,pbi->LastMode,type);
         pbi->LastMode = mode; 
    //pbi->MBCols
    pbi->predictionMode[MBOffset(pbi->MBCols,MBrow,MBcol)] = mode;
    pbi->mbi.Mode = mode;
        bdi = &pbi->mbi.blockDxInfo[0];

    if ( mode == CODE_INTER_FOURMV )    
    {
         // motion vector sanity variables
         QpelX = (UMV_BORDER + (MBcol * 16)) * 4;
         QpelY = MBrow * 16 * 4;
 
        bdi[0].BlockMode = VP6_DecodeBlockMode(pbi);
        bdi[1].BlockMode = VP6_DecodeBlockMode(pbi);
        bdi[2].BlockMode = VP6_DecodeBlockMode(pbi);
        bdi[3].BlockMode = VP6_DecodeBlockMode(pbi);

        bdi[4].BlockMode = CODE_INTER_FOURMV;
        bdi[5].BlockMode = CODE_INTER_FOURMV;
        x=0;
        y=0;
        for ( k=0; k<4; k++ )
        {
             if (k == 1) {
                 QpelX += 8 * 4;
             } else if (k == 2) {
                 QpelX -= 8 * 4;
                 QpelY += 8 * 4;
             } else if (k == 3) {
                 QpelX += 8 * 4;
             }
             
            if ( bdi->BlockMode == CODE_INTER_NO_MV )
            {
                bdi->Mv.x = 0;
                bdi->Mv.y = 0;
            }
            else if( bdi->BlockMode == CODE_INTER_NEAREST_MV )
            {
                bdi->Mv.x = pbi->mbi.NearestInterMVect.x;
                bdi->Mv.y = pbi->mbi.NearestInterMVect.y;                
                x+=pbi->mbi.NearestInterMVect.x;
                y+=pbi->mbi.NearestInterMVect.y;
            }
            else if ( bdi->BlockMode == CODE_INTER_NEAR_MV )
            {
                bdi->Mv.x = pbi->mbi.NearInterMVect.x;
                bdi->Mv.y = pbi->mbi.NearInterMVect.y;                
                x+=pbi->mbi.NearInterMVect.x;
                y+=pbi->mbi.NearInterMVect.y;
            }
            else if ( bdi->BlockMode == CODE_INTER_PLUS_MV )
            {
                VP6_decodeMotionVector(pbi,&mv,CODE_INTER_PLUS_MV);
                bdi->Mv.x = mv.x;
                bdi->Mv.y = mv.y;
                x+=mv.x;
                y+=mv.y;
            }
             // now the coordinates are easier to check since the motion vectors are
             // in fractional units; the boundaries are re-computed any time the 
             // frame size changes
             if (QpelX + bdi->Mv.x < pbi->MacroblockOverscanLeftBoundary)
                 return FALSE;
             if (QpelX + (8*4) + bdi->Mv.x >= pbi->MacroblockOverscanRightBoundary)
                 return FALSE;
             if (QpelY + bdi->Mv.y < pbi->MacroblockOverscanTopBoundary)
                 return FALSE;
             if (QpelY + (8*4) + bdi->Mv.y >= pbi->MacroblockOverscanBottomBoundary)
                 return FALSE;

            bdi++;
        }
        x = (x+1+(x>=0))>>2;
        y = (y+1+(y>=0))>>2;
    
        bdi = &pbi->mbi.blockDxInfo[3];

        pbi->MBMotionVector[MBOffset(pbi->MBCols,MBrow,MBcol)].x = bdi->Mv.x;
        pbi->MBMotionVector[MBOffset(pbi->MBCols,MBrow,MBcol)].y = bdi->Mv.y;
        bdi++;
        
        bdi->Mv.x = x; 
        bdi->Mv.y = y;
        bdi++;

        bdi->Mv.x = x; 
        bdi->Mv.y = y;
    }
    else
    {
        switch ( mode )
        {
        case CODE_INTER_NEAREST_MV:
            x = pbi->mbi.NearestInterMVect.x;
            y = pbi->mbi.NearestInterMVect.y;            
            break;
        case CODE_INTER_NEAR_MV:
            x = pbi->mbi.NearInterMVect.x;
            y = pbi->mbi.NearInterMVect.y;
            break;
        case CODE_GOLD_NEAREST_MV:
            VP6_FindNearestandNextNearest(pbi, MBrow, MBcol, 2, &type);
            x = pbi->mbi.NearestGoldMVect.x;
            y = pbi->mbi.NearestGoldMVect.y;
            break;
        case CODE_GOLD_NEAR_MV:
            VP6_FindNearestandNextNearest(pbi, MBrow, MBcol, 2, &type);
            x = pbi->mbi.NearGoldMVect.x;
            y = pbi->mbi.NearGoldMVect.y;
            break;
        case CODE_INTER_PLUS_MV:
            VP6_decodeMotionVector(pbi,&mv,CODE_INTER_PLUS_MV);
            x = mv.x;
            y = mv.y;
            break;
        case CODE_GOLDEN_MV:
            VP6_FindNearestandNextNearest(pbi, MBrow, MBcol, 2, &type);
            VP6_decodeMotionVector(pbi,&mv,CODE_GOLDEN_MV);
            x = mv.x;
            y = mv.y;
            break;
        default:
            x =0;
            y =0;
            break; 
        }
        pbi->MBMotionVector[MBOffset(pbi->MBCols,MBrow,MBcol)].x = x;
        pbi->MBMotionVector[MBOffset(pbi->MBCols,MBrow,MBcol)].y = y;
    
        bdi = &pbi->mbi.blockDxInfo[0];
        for ( k=0; k<6 ; k++ )
        {
            bdi->Mv.x = x;
            bdi->Mv.y = y;
            bdi->BlockMode = mode;
            bdi++;
        }

         // all 6 fragment motion vectors are the same; validate MV for the
         // entire macroblock
         // compute the macroblock's coordinates w.r.t. the overscan macroblock
         // map expressed in fractional pixels
         QpelX = (UMV_BORDER + (MBcol * 16)) * 4;
         QpelY = MBrow * 16 * 4;
 
         // now the coordinates are easier to check since the motion vectors are
         // in fractional units; the boundaries are re-computed any time the 
         // frame size changes
         if (QpelX + x < pbi->MacroblockOverscanLeftBoundary)
             return FALSE;
         if (QpelX + (16*4) + x >= pbi->MacroblockOverscanRightBoundary)
             return FALSE;
         if (QpelY + y < pbi->MacroblockOverscanTopBoundary)
             return FALSE;
         if (QpelY + (16*4) + y >= pbi->MacroblockOverscanBottomBoundary)
             return FALSE;
       }
 
     return TRUE;
}

//*******************************Motion Vector ******************//
static void VP6_ConfigureMvEntropyDecoder( PB_INSTANCE *pbi, U8 FrameType)
{
    U32 c;
    // Read any changes to mv probabilities.
    U8* pProb=(U8*)&VP6_MvUpdateProbs[0];
    for (c=0; c<2; c++)
        {
        // Short vector probability.
        if (VP6_DecodeBool(&pbi->br,*pProb++))
            {
            U32 data=VP6_bitread(&pbi->br,PROB_UPDATE_BASELINE_COST)<<1;
            if (data==0)
                data=1;
            pbi->IsMvShortProb[c]=(U8)data;
            }

        // Sign probability.
        if (VP6_DecodeBool(&pbi->br,*pProb++))
            {
            U32 data=VP6_bitread(&pbi->br,PROB_UPDATE_BASELINE_COST)<<1;
            if (data==0)
                data=1;
            pbi->MvSignProbs[c]=(U8)data;
            }
        }

    // Short vector tree node probabilities
    c=2*7;
    U8* pDest=&pbi->MvShortProbs[0][0];
    do
        {
        if (VP6_DecodeBool(&pbi->br,*pProb++))
            {
            U32 data=VP6_bitread(&pbi->br,PROB_UPDATE_BASELINE_COST)<<1;
            if (data==0)
                data=1;
            *pDest=(U8)data;
            }
        pDest++;
        }
    while (--c>0);

    // Long vector tree node probabilities
    c=2*LONG_MV_BITS;
    pDest=&pbi->MvSizeProbs[0][0];
    do
        {
        if (VP6_DecodeBool(&pbi->br,*pProb++))
            {
            U32 data=(U8)VP6_bitread(&pbi->br,PROB_UPDATE_BASELINE_COST)<<1;
            if (data==0)
                data=1;
            *pDest=(U8)data;
            }
        pDest++;
        }
    while (--c>0);
}

static void VP6_FindNearestandNextNearest(PB_INSTANCE *pbi,U32 MBrow,U32 MBcol,U8 Frame,int *type)
{
    int i;
    U32 OffsetMB;
    U32 BaseMB = MBOffset(pbi->MBCols,MBrow,MBcol);
    S32 Nearest = 0;
    S32 NextNearest = 0;
    S32 nearestIndex;
    U32 thisMv;
    S32 typet;
    
    typet = NONEAREST_MACROBLOCK;

    // BEWARE:
    // The use of (unsigned int *) casting here is potentially dangerous 
    // and will only work if the motion vector structure consists of 
    // two 16 bit values and is 32 bit aligned.
    for ( i=0; i<12 ; i++ )
    { 
        OffsetMB = pbi->mvNearOffset[i] + BaseMB;
              U32 predMode=pbi->predictionMode[OffsetMB];
        if ( VP6_Mode2Frame[predMode] != Frame )
            continue;

        thisMv = *((unsigned int *) &pbi->MBMotionVector[OffsetMB]);

        if ( thisMv ) 
        {
    
            *((unsigned int *) &Nearest) = thisMv;
            typet = NONEAR_MACROBLOCK;
            break;
        }             
       }

    nearestIndex = i;

    for ( i=i+1; i<12; i++ )
    {
        OffsetMB = pbi->mvNearOffset[i] + BaseMB;
        U32 predMode=pbi->predictionMode[OffsetMB];
        if ( VP6_Mode2Frame[predMode] != Frame )
            continue;
        
        thisMv = *((unsigned int *) &pbi->MBMotionVector[OffsetMB]);
        if( thisMv == *((unsigned int *) &Nearest) )
            continue;
        
        if( thisMv ) 
        {
            *((unsigned int *) &NextNearest) = thisMv;
            typet = MACROBLOCK;
            break;
        }
    }
    
    if ( Frame == 1 )
    {
        *type = typet;
        pbi->mbi.NearestMvIndex = nearestIndex;
        pbi->mbi.NearestInterMVect.x  = (S16)(Nearest & 0xFFFF);
        pbi->mbi.NearestInterMVect.y  = (S16)((Nearest >> 16) & 0xFFFF);        
        pbi->mbi.NearInterMVect.x  = (S16)(NextNearest & 0xFFFF);
        pbi->mbi.NearInterMVect.y  = (S16)((NextNearest >> 16) & 0xFFFF);
    }
   else
   {
        pbi->mbi.NearestGMvIndex = nearestIndex;        
        pbi->mbi.NearestGoldMVect.x  = (S16)(Nearest & 0xFFFF);
        pbi->mbi.NearestGoldMVect.y  = (S16)((Nearest >> 16) & 0xFFFF);        
        pbi->mbi.NearGoldMVect.x  = (S16)(NextNearest & 0xFFFF);
        pbi->mbi.NearGoldMVect.y  = (S16)((NextNearest >> 16) & 0xFFFF);
   }    
}


//***********************************scan and DQ******************************//
static void VP6_init_dequantizer ( QUANTIZER *qi, U8 Vp3VersionNo )
{
///////////////////////////////////////////////////////////////
// Optimisation #29
///////////////////////////////////////////////////////////////

    int frameQIndex=qi->FrameQIndex;

    // AC: Y set-up the dequant values and then place in the zig-zag/transposed order.
    const Q_LIST_ENTRY qThresh=(Q_LIST_ENTRY)(VP6_QThreshTable[frameQIndex]<<IDCT_SCALE_FACTOR);

    // AC: UV set-up the dequant values and then place in the zig-zag/transposed order.
    const Q_LIST_ENTRY uvqThresh=(Q_LIST_ENTRY)(VP6_UvQThreshTable[frameQIndex]<<IDCT_SCALE_FACTOR);

    Q_LIST_ENTRY* pY=&qi->dequant_coeffs[0][0];
    Q_LIST_ENTRY* pUV=&qi->dequant_coeffs[1][0];

    *pY=VP6_DcQuant[frameQIndex]<<IDCT_SCALE_FACTOR; // DC: Y
    *pUV=VP6_UvDcQuant[frameQIndex]<<IDCT_SCALE_FACTOR; // DC: UV

    U32 loop=63;
    U8* pQuantIndex=(U8*)&KQuantIndex[0];

    do
        {
        U32 index=*pQuantIndex++;
        pY[index]=qThresh;
        pUV[index]=uvqThresh;
        }
    while (--loop>0);

}

static void  VP6_BuildScanOrder( PB_INSTANCE *pbi, U8 *ScanBands )
{
    U32 i;
    U8 j;
    U32 ScanOrderIndex = 1;
    U32 MaxOffset;

    // DC is fixed
    pbi->ModifiedScanOrder[0] = 0;

    // Create a scan order where within each band the coefs are in ascending order
    // (in terms of their original zig-zag positions).
    for ( i = 0; i < SCAN_ORDER_BANDS; i++ )
    {
        for ( j = 1; j < BLOCK_SIZE; j++ )
        {
            if ( ScanBands[j] == i )
            {
                pbi->ModifiedScanOrder[ScanOrderIndex] = j;
                ScanOrderIndex++;
            }
        }
    }

    // For each of the positions in the modified scan order work out the 
    // worst case EOB offset in zig zag order. This is used in selecting
    // the appropriate idct variant
    for ( i = 0; i < BLOCK_SIZE; i++ )
    {
        MaxOffset = 0;
        for ( j = 0; j <= i; j++ )
        {
            if ( pbi->ModifiedScanOrder[j] > MaxOffset )
                MaxOffset = pbi->ModifiedScanOrder[j];
        }

        pbi->EobOffsetTable[i] = (U8)MaxOffset;

        if(pbi->Vp3VersionNo > 6)
            pbi->EobOffsetTable[i] = (U8)MaxOffset+1;

    }
}

static void VP6_UpdateQ ( QUANTIZER *qi, U8 Vp3VersionNo )
{  
   if ( qi->FrameQIndex == qi->LastFrameQIndex )
        return;

    // Update the record of last Q index.
    qi->LastFrameQIndex = qi->FrameQIndex;
    // Re-initialise the q tables for forward and reverse transforms.    
    VP6_init_dequantizer ( qi, Vp3VersionNo );
}

//************************************HUF and BOOL DECODE****************************//
static void InsertSorted ( sortnode *sn, int node, int *startnode )
{
    int which = *startnode;
    int prior = *startnode;

    // find the position at which to insert the node
    while( which != -1 && sn[node].freq > sn[which].freq )
    {
        prior = which;
        which = sn[which].next;
    }

    if(which == *startnode)
    {
        *startnode = node;
        sn[node].next = which;
    }
    else
    {
        sn[prior].next = node;
        sn[node].next = which;
    }
}

static void VP6_BuildHuffTree ( HUFF_NODE *hn, U32 *counts, S32 values )
{
    int i;
    sortnode sn[256];
    int sncount=0;
    int startnode=0;

    // NOTE:
    // Create huffman tree in reverse order so that the root will always be 0
    int huffptr=values-1;

    // Set up sorted linked list of values/pointers into the huffman tree
    for ( i=0; i<values; i++ )
    {
        sn[i].value = (i << 1) | 1;
        if(counts[i] == 0) {
            counts[i] = 1;
        }
        sn[i].freq = counts[i];
        sn[i].next = -1;

    }

    sncount = values;

    // Connect above list into a linked list
    for ( i=1; i<values; i++ )
        InsertSorted ( sn, i, &startnode );

    // while there is more than one node in our linked list
    while ( sn[startnode].next != -1 )
    {
        int first = startnode;
        int second = sn[startnode].next;
        int sumfreq = sn[first].freq + sn[second].freq;

        // set-up new merged huffman node
        --huffptr;
        // setup new merged huffman node
        hn[huffptr].left = sn[first].value;
        hn[huffptr].right = sn[second].value;

        // set up new merged sort node pointing to our huffnode
        sn[sncount].value = (huffptr << 1) | 0;
        sn[sncount].freq = sumfreq;
        sn[sncount].next = -1;


        // remove the two nodes we just merged from the linked list
        startnode = sn[second].next;

        // insert the new sort node into the proper location
        InsertSorted(sn, sncount, &startnode);

        // account for new nodes
        sncount++;
    }

    return;
}


static void  BoolTreeToHuffCodes ( U8 *BoolTreeProbs, U32 *HuffProbs )
{
    U32 Prob;
    U32 Prob1;

    HuffProbs[DCT_EOB_TOKEN]       = ((U32)BoolTreeProbs[0] * (U32)BoolTreeProbs[1]) >> 8;
    HuffProbs[ZERO_TOKEN]          = ((U32)BoolTreeProbs[0] * (255 - (U32)BoolTreeProbs[1])) >> 8;

    Prob = (255 - (U32)BoolTreeProbs[0]);
    HuffProbs[ONE_TOKEN]           = (Prob * (U32)BoolTreeProbs[2]) >> 8;

    Prob = (Prob*(255 - (U32)BoolTreeProbs[2])) >> 8;
    Prob1 = (Prob * (U32)BoolTreeProbs[3]) >> 8;
    HuffProbs[TWO_TOKEN]           = (Prob1 * (U32)BoolTreeProbs[4]) >> 8; 
    Prob1 = (Prob1 * (255 - (U32)BoolTreeProbs[4])) >> 8;
    HuffProbs[THREE_TOKEN]         = (Prob1 * (U32)BoolTreeProbs[5]) >> 8;
    HuffProbs[FOUR_TOKEN]          = (Prob1 * (255 - (U32)BoolTreeProbs[5])) >> 8;

    Prob = (Prob * (255 - (U32)BoolTreeProbs[3])) >> 8;
    Prob1 = (Prob * (U32)BoolTreeProbs[6]) >> 8;
    HuffProbs[DCT_VAL_CATEGORY1]   = (Prob1 * (U32)BoolTreeProbs[7]) >> 8;
    HuffProbs[DCT_VAL_CATEGORY2]   = (Prob1 * (255 - (U32)BoolTreeProbs[7])) >> 8;
    
    Prob = (Prob * (255 - (U32)BoolTreeProbs[6])) >> 8;
    Prob1 = (Prob * (U32)BoolTreeProbs[8]) >> 8; 
    HuffProbs[DCT_VAL_CATEGORY3]   = (Prob1 * (U32)BoolTreeProbs[9]) >> 8;
    HuffProbs[DCT_VAL_CATEGORY4]   = (Prob1 * (255 - (U32)BoolTreeProbs[9])) >> 8;

    Prob = (Prob * (255 - (U32)BoolTreeProbs[8])) >> 8;
    HuffProbs[DCT_VAL_CATEGORY5]   = (Prob * (U32)BoolTreeProbs[10]) >> 8;
    HuffProbs[DCT_VAL_CATEGORY6]   = (Prob * (255 - (U32)BoolTreeProbs[10])) >> 8;
}

static void ZerosBoolTreeToHuffCodes ( U8 *BoolTreeProbs, U32 *HuffProbs )
{
    U32 Prob;

    Prob  = ((U32)BoolTreeProbs[0] * (U32)BoolTreeProbs[1]) >> 8;
    HuffProbs[0] = (Prob * (U32)BoolTreeProbs[2]) >> 8;
    HuffProbs[1] = (Prob * (U32)(255 - BoolTreeProbs[2])) >> 8;

    Prob = ((U32)BoolTreeProbs[0] * (U32)(255 - BoolTreeProbs[1])) >> 8;
    HuffProbs[2] = (Prob * (U32)BoolTreeProbs[3]) >> 8;
    HuffProbs[3] = (Prob * (U32)(255 - BoolTreeProbs[3])) >> 8;

    Prob = ((U32)(255 - BoolTreeProbs[0]) * (U32)BoolTreeProbs[4]) >> 8;
    Prob = (Prob * (U32)BoolTreeProbs[5]) >> 8;
    HuffProbs[4] = (Prob * (U32)BoolTreeProbs[6]) >> 8;
    HuffProbs[5] = (Prob * (U32)(255 - BoolTreeProbs[6])) >> 8;

    Prob = ((U32)(255 - BoolTreeProbs[0]) * (U32)BoolTreeProbs[4]) >> 8;
    Prob = (Prob * (U32)(255 - BoolTreeProbs[5])) >> 8;
    HuffProbs[6] = (Prob * (U32)BoolTreeProbs[7]) >> 8;
    HuffProbs[7] = (Prob * (U32)(255 - BoolTreeProbs[7])) >> 8;

    Prob = ((U32)(255 - BoolTreeProbs[0]) * (U32)(255 - BoolTreeProbs[4])) >> 8;
    HuffProbs[8] = Prob;
}


static void VP6_ConvertDecodeBoolTrees ( PB_INSTANCE *pbi )
{
    U32  i;
    U32    Plane;
    U32    Band;
    S32   Prec;

    // Convert bool tree node probabilities into array of token 
    // probabilities. Use these to create a set of Huffman codes
//ON2LOG("sizeof(HUFF_NODE) = %d\n", sizeof(HUFF_NODE));
    // DC
    for ( Plane = 0; Plane < 2; Plane++ )
    {
        BoolTreeToHuffCodes ( pbi->DcProbs+DCProbOffset(Plane,0), pbi->DcHuffProbs[Plane] );
        VP6_BuildHuffTree ( pbi->DcHuffTree[Plane], pbi->DcHuffProbs[Plane], MAX_ENTROPY_TOKENS );
    }
    
    // ZEROS
    for ( i = 0; i < ZRL_BANDS; i++ )
    {
        ZerosBoolTreeToHuffCodes ( pbi->ZeroRunProbs[i], pbi->ZeroHuffProbs[i] );
        VP6_BuildHuffTree ( pbi->ZeroHuffTree[i], pbi->ZeroHuffProbs[i], 9 );
    }

    // AC
    for ( Prec = 0; Prec < PREC_CASES; Prec++ )
    {
        // Baseline probabilities for each AC band.
        for ( Plane = 0; Plane < 2; Plane++ )
        {
            for ( Band = 0; Band < VP6_AC_BANDS; Band++ )
            {
                BoolTreeToHuffCodes ( pbi->AcProbs+ACProbOffset(Plane,Prec,Band,0), pbi->AcHuffProbs[Prec][Plane][Band] );
                VP6_BuildHuffTree ( pbi->AcHuffTree[Prec][Plane][Band], pbi->AcHuffProbs[Prec][Plane][Band], MAX_ENTROPY_TOKENS );
            }
        }
    }
}

static inline void ResetCoeffBuffer(S16 *coeffsPtr)
{
    unsigned long *pTemp = (unsigned long *) coeffsPtr;
    pTemp[0]=0x00000000L;
    pTemp[1]=0x00000000L;
    pTemp[2]=0x00000000L;
    pTemp[3]=0x00000000L;
    pTemp[4]=0x00000000L;
    pTemp[5]=0x00000000L;
    pTemp[8]=0x00000000L;
    pTemp[9]=0x00000000L;
    pTemp[12]=0x00000000L;
    pTemp[13]=0x00000000L;
    coeffsPtr[32] = 0;
    return;
}

static void VP6_ConfigureEntropyDecoder( PB_INSTANCE *pbi, U8 FrameType )
{
 
     U8 LastProb[MAX_ENTROPY_TOKENS]; // 12 bytes so we can write 32bit words.

    // Clear down Last Probs data structure (Don't call FlashMemSet for
    // 11 bytes :(.
    *(U32*)&LastProb[0]=0x80808080;
    *(U32*)&LastProb[4]=0x80808080;
    *(U32*)&LastProb[8]=0x80808080;

    // Read in the Baseline DC probabilities and initialise the DC
    // context for Y and then UV plane
    U32 c=2*(MAX_ENTROPY_TOKENS-1);
    U8* pUpdateProbs=(U8*)&VP6_DcUpdateProbs[0][0];
    U8* pLastProb=&LastProb[0];
    U8* pLastProbEnd=&LastProb[MAX_ENTROPY_TOKENS-1];
    U8* pDcProbIndex=(U8*)&KDcProbsIndex[0];
    do
        {
        U8* pDcProbsDest=&pbi->DcProbs[*pDcProbIndex++];
        if (DECODE_BOOL(&pbi->br,*pUpdateProbs++))
            {
            // 0 is not a legal value, clip to 1.
            U32 data=VP6_bitread(&pbi->br,PROB_UPDATE_BASELINE_COST)<<1;
            if (data==0)
                data=1;
            *pDcProbsDest=(U8)data;
            *pLastProb=(U8)data;
            }
        else if (FrameType==BASE_FRAME)
            *pDcProbsDest=*pLastProb;
        if (++pLastProb>=pLastProbEnd)
            pLastProb=&LastProb[0];
        }
    while (--c>0);

    // Set Zero run probabilities to defaults if this is a key frame
    if (FrameType==BASE_FRAME)
        FlashMemCpy(&pbi->ZeroRunProbs[0][0],&ZeroRunProbDefaults[0][0],ZRL_BANDS*ZERO_RUN_PROB_CASES*sizeof(U8));

    // If this frame contains updates to the scan order then read them
    if (DECODE_BOOL(&pbi->br,128))
    {
        U32 c=BLOCK_SIZE-1;
        U8* pScanBandUpdateProbs=(U8*)&ScanBandUpdateProbs[1];
        U8* pScanBands=&pbi->ScanBands[1];

        // Read in the AC scan bands and build the custom scan order
        do
        {
            // Has the band for this coef been updated?
            if (DECODE_BOOL(&pbi->br,*pScanBandUpdateProbs++))
                *pScanBands=(U8)VP6_bitread(&pbi->br,SCAN_BAND_UPDATE_BITS);
            pScanBands++;

        }
        while (--c>0);

        // Build the scan order
        VP6_BuildScanOrder(pbi,pbi->ScanBands);
    }
    
    // Update the Zero Run probabilities
    c=ZRL_BANDS*ZERO_RUN_PROB_CASES;
    U8* pZrlUpDateProbs=(U8*)&ZrlUpdateProbs[0][0];
    U8* pZeroRunProbs=&pbi->ZeroRunProbs[0][0];
    do
        {
        if (DECODE_BOOL(&pbi->br,*pZrlUpDateProbs++))
            {
            // Probabilities sent
            U32 data=VP6_bitread(&pbi->br,PROB_UPDATE_BASELINE_COST)<<1;
            if (data==0)
                data=1;
            *pZeroRunProbs=(U8)data;
            }
        pZeroRunProbs++;
        }
    while (--c>0);

    // Read in the Baseline AC band probabilities and initialise the
    // appropriate contexts
    // Prec=0 means last token in current block was 0: Prec=1 means
    // it was 1. Prec=2 means it was > 1
    c=PREC_CASES*2*VP6_AC_BANDS*(MAX_ENTROPY_TOKENS-1);
    U8* pAcUpdateProbs=(U8*)&VP6_AcUpdateProbs[0][0][0][0];
    U16* pAcIndex=(U16*)&KACProbIndex[0];
    pLastProb=&LastProb[0];
    do
        {
        U8* pAcProbsDest=&pbi->AcProbs[*pAcIndex++];
        if (DECODE_BOOL(&pbi->br,*pAcUpdateProbs++))
            {
            // Probabilities transmitted at reduced resolution.
            // 0 is not a legal value, clip to 1.
            U32 data=VP6_bitread(&pbi->br,PROB_UPDATE_BASELINE_COST)<<1;
            if (data==0)
                data=1;
            *pAcProbsDest=(U8)data;
            *pLastProb=(U8)data;
            }
        else if (FrameType==BASE_FRAME)
            *pAcProbsDest=*pLastProb;
        if (++pLastProb>=pLastProbEnd)
            pLastProb=&LastProb[0];
        }
    while (--c>0);

    // Create all the context specific propabilities based upon the
    // new baseline data
    VP6_ConfigureContexts(pbi);
 
}


//***********************************IDCT *****************************************//

static inline void dequant_slow ( const S16 *dequant_coeffs, const S16 *quantized_list, S32 *DCT_block)
{

    S16 a,b,c,d;

    c = *quantized_list++;
    d = *dequant_coeffs++;

    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 0] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 1] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 8] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 16] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 9] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 2] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 3] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 10] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 17] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 24] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 32] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 25] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 18] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 11] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 4] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 5] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 12] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 19] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 26] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 33] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 40] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 48] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 41] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 34] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 27] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 20] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 13] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 6] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 7] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 14] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 21] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 28] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 35] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 42] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 49] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 56] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 57] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 50] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 43] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 36] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 29] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 22] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 15] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 23] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 30] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 37] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 44] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 51] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 58] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 59] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 52] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 45] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 38] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 31] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 39] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 46] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 53] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 60] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 61] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 54] = a * b;
    a = *quantized_list++;    b = *dequant_coeffs++;    DCT_block[ 47] = c * d;
    c = *quantized_list++;    d = *dequant_coeffs++;    DCT_block[ 55] = a * b;
    a = *quantized_list;    b = *dequant_coeffs;    DCT_block[ 62] = c * d;
    DCT_block[ 63] = a * b;

}


static inline void  dequant_slow10 ( const S16 *dequant_coeffs, const S16 *quantized_list, S32 *DCT_block )
{
    FlashMemSet(DCT_block,0, 128);
    // Loop fully expanded for maximum speed
    DCT_block[ 0] = quantized_list[0]  * dequant_coeffs[0];
    DCT_block[ 1] = quantized_list[1]  * dequant_coeffs[1];
    DCT_block[ 8] = quantized_list[2]  * dequant_coeffs[2];
    DCT_block[16] = quantized_list[3]  * dequant_coeffs[3];
    DCT_block[ 9] = quantized_list[4]  * dequant_coeffs[4];
    DCT_block[ 2] = quantized_list[5]  * dequant_coeffs[5];
    DCT_block[ 3] = quantized_list[6]  * dequant_coeffs[6];
    DCT_block[10] = quantized_list[7]  * dequant_coeffs[7];
    DCT_block[17] = quantized_list[8]  * dequant_coeffs[8];
    DCT_block[24] = quantized_list[9]  * dequant_coeffs[9];
    DCT_block[32] = quantized_list[10] * dequant_coeffs[10];
}

static void IDctSlow ( S16 *InputData, S16 *QuantMatrix, S16 *OutputData )
{
    int   loop;
    S32 t1, t2;
    S32 IntermediateData[64];
    S32 A, B, C, D, Ad, Bd, Cd, Dd, E, F, G, H;
    S32 Ed, Gd, Add, Bdd, Fd, Hd;

    S32 *ip = IntermediateData;
    S16 *op = OutputData;

    // dequantize the input
    dequant_slow ( QuantMatrix, InputData, IntermediateData );

//
// OPTIMISATION #3
// ===============
// MHB (23/11/06) - This is an old trick (modern compilers automatically
// do this change silently for you if "loop" isn't used in the body of
// code) but try replacing for loop counting up with a loop counting down.
//
// e.g. for (loop=8; loop!=0; loop--)
//
// This means the code generated relies on the results in the condition
// code register instead of something like :-
//
//         add r5, r5, #1        ; (1 cycle).
//        cmp r5, #8            ; (1 cycle).
//        bne ForLoopStart    ; (3 cycles if taken, 1 otherwise).
//
// It may become :-
//
//        subs r5, r5, #1        ; (1 cycle).
//        bne ForLoopStart    ; (3 cycles if taken, 1 otherwise).
//
// Saving a single cycle per loop iteration.
//
// Change the "loop" variable to be an unsigned 32 bit value.
//
// You could also try replacing the "for" loop with a "do/while" loop and
// check which is the most efficient code produced.
//

    // Inverse DCT on the rows now
    for ( loop=0; loop<8; loop++ )
    {

//
// OPTIMISATION #4
// ===============
// MHB (23/11/06) - I think this code is just trying to save performing
// all the multiplies. However I suspect that it will produce eight
// consecutive sequences of the following ARM code :-
//
// [#1]    ldr r0, [r1, #XX]        ; (1 cycle) + 1 cycle if used next
// [#2]    orr(s) r2, r2, r0        ; (1 cycle)
// [#3] str r0, [sp, #NN]        ; (1 cycle)
// [#4]    bne JumpToForLoopBody    ; (1 if branch not taken, 3 otherwise)
//
// Notes
// =====
// 1) Instruction #3 is compiler dependent and is used to copy the data
// from the "ip" array on to the stack.
// 2) Instruction #4 is compiler dependent and is used in "if" statements
// to "bail early".
//
// At minimum the processor will waste 16 up to 32 cycles (depending on
// the compiler and also ignoring any pipeline stalls) checking for
// something that may not occur very often.
//
// The compiler generated ARM code in the body of the "if" statement
// may also contain pipeline stalls due to the data being read from
// the "ip[]" array and then a multiplication being performed on it.
//
// Recommendations
// ===============
// 1) Remove the "if" statement completely and profile the code.
// 2) Change each "xCpSq" block of code to do the following :-
//
// A=0; B=0; // Initialised if either ip[1] or ip[7] not zero..
//
// if (ip[1]|ip[7])    // Check if anything to multiply.
//        {
//        t1 = (S32)(xC1S7 * ip[1]);
//        t2 = (S32)(xC7S1 * ip[7]);
//        t1 >>= 16;
//        t2 >>= 16;
//        A = t1 + t2;
//
//        t1 = (S32)(xC7S1 * ip[1]);
//        t2 = (S32)(xC1S7 * ip[7]);
//        t1 >>= 16;
//        t2 >>= 16;
//        B = t1 - t2;
//        }
//
// *DON'T* try an "if/then/else" construct because it will generate an
// extra unwanted branch instruction which will always be executed.
//
// Profile the code and compare against the original and the one above.
//
// 3) You could also try moving the "ip[xx]" and "ip[nn]" into temporary
// variables before the "if" statement. However I strongly suspect that
// the compiler may already be doing that.
//
// 4) If neither 1, 2 or 3 provide satisfactory results then the entire
// function would need replacing with a hand optimised ARM/THUMB assembler
// version.
//
        // Check for non-zero values
        if ( ip[0] | ip[1] | ip[2] | ip[3] | ip[4] | ip[5] | ip[6] | ip[7] )
        {
//
// OPTIMISATION #5
// ===============
// MHB (23/11/06) - It is not possible to re-factor the mathematics
// to reduce the number of multiplies in this loop.
//
// Comments on precision
// =====================
// If this function were hand coded in assembler the addition of
// t1 and t2 (throughout the loop's body) could be carried out before the shift
// right 16 by making use of the carry (if any) in the condition code
// register (effectively 33 bit precision). Obviously this would increase
// the precision of the final result.
//
            t1 = (S32)(xC1S7 * ip[1]);
            t2 = (S32)(xC7S1 * ip[7]);
            t1 >>= 16;
            t2 >>= 16;
            A = t1 + t2;

            t1 = (S32)(xC7S1 * ip[1]);
            t2 = (S32)(xC1S7 * ip[7]);
            t1 >>= 16;
            t2 >>= 16;
            B = t1 - t2;

            t1 = (S32)(xC3S5 * ip[3]);
            t2 = (S32)(xC5S3 * ip[5]);
            t1 >>= 16;
            t2 >>= 16;
            C = t1 + t2;

            t1 = (S32)(xC3S5 * ip[5]);
            t2 = (S32)(xC5S3 * ip[3]);
            t1 >>= 16;
            t2 >>= 16;
            D = t1 - t2;

            t1 = (S32)(xC4S4 * (A - C));
            t1 >>= 16;
            Ad = t1;

            t1 = (S32)(xC4S4 * (B - D));
            t1 >>= 16;
            Bd = t1;

            Cd = A + C;
            Dd = B + D;

            t1 = (S32)(xC4S4 * (ip[0] + ip[4]));
            t1 >>= 16;
            E = t1;

            t1 = (S32)(xC4S4 * (ip[0] - ip[4]));
            t1 >>= 16;
            F = t1;

            t1 = (S32)(xC2S6 * ip[2]);
            t2 = (S32)(xC6S2 * ip[6]);
            t1 >>= 16;
            t2 >>= 16;
            G = t1 + t2;

            t1 = (S32)(xC6S2 * ip[2]);
            t2 = (S32)(xC2S6 * ip[6]);
            t1 >>= 16;
            t2 >>= 16;
            H = t1 - t2;

            Ed = E - G;
            Gd = E + G;

            Add = F + Ad;
            Bdd = Bd - H;

            Fd = F - Ad;
            Hd = Bd + H;

//
// OPTIMISATION #6
// ===============
// MHB (23/11/06) - Are these casts to S16 really necessary? The array
// "ip[]" contains signed 32 bit values. They are actually reducing the
// precision of the final arithmetic results. The compiler will also
// impose a 2 instruction overhead for each cast because it will perform
// the following code sequence before the data is written to the "ip[]"
// array :-
//
// [#1] mov r1, r1, LSL #16    ; Shift left to remove top 16 bits.
// [#2] mov r1, r1, ASR #16    ; Shift right 16 bits and sign extend to 32 bits.
//
// Recommendations
// ===============
// 1) The compiler will not take any notice of the ">> 0" portion of the
// code so that can safely be removed.
//
// 2) Removal of the casts will reduce the cycle count of the loop by 16
// cycles per iteration (the equivalent of 8 32x32 multiplies in ARM9TE).
//
            // Final sequence of operations over-write original inputs.
            ip[0] = (S16)((Gd + Cd )  >> 0);
            ip[7] = (S16)((Gd - Cd )  >> 0);

            ip[1] = (S16)((Add + Hd ) >> 0);
            ip[2] = (S16)((Add - Hd ) >> 0);

            ip[3] = (S16)((Ed + Dd )  >> 0);
            ip[4] = (S16)((Ed - Dd )  >> 0);

            ip[5] = (S16)((Fd + Bdd ) >> 0);
            ip[6] = (S16)((Fd - Bdd ) >> 0);
        }

        ip += 8;            /* next row */
    }

    ip = IntermediateData;

//
// OPTIMISATION #3
// ===============
// MHB (23/11/06) - Apply optimisation #3 (defined above) here.
//
    for ( loop=0; loop<8; loop++ )
    {

//
// OPTIMISATION #4
// ===============
// MHB (23/11/06) - Apply optimisation #4 (defined above) here.
//
        // Check for non-zero values (bitwise | faster than logical ||)
        if ( ip[0 * 8] | ip[1 * 8] | ip[2 * 8] | ip[3 * 8] |
             ip[4 * 8] | ip[5 * 8] | ip[6 * 8] | ip[7 * 8] )
        {
//
// OPTIMISATION #5
// ===============
// MHB (23/11/06) - Apply optimisation #5 (defined above) here.
//

            t1 = (S32)(xC1S7 * ip[1*8]);
            t2 = (S32)(xC7S1 * ip[7*8]);
            t1 >>= 16;
            t2 >>= 16;
            A = t1 + t2;

            t1 = (S32)(xC7S1 * ip[1*8]);
            t2 = (S32)(xC1S7 * ip[7*8]);
            t1 >>= 16;
            t2 >>= 16;
            B = t1 - t2;

            t1 = (S32)(xC3S5 * ip[3*8]);
            t2 = (S32)(xC5S3 * ip[5*8]);
            t1 >>= 16;
            t2 >>= 16;
            C = t1 + t2;

            t1 = (S32)(xC3S5 * ip[5*8]);
            t2 = (S32)(xC5S3 * ip[3*8]);
            t1 >>= 16;
            t2 >>= 16;
            D = t1 - t2;

            t1 = (S32)(xC4S4 * (A - C));
            t1 >>= 16;
            Ad = t1;

            t1 = (S32)(xC4S4 * (B - D));
            t1 >>= 16;
            Bd = t1;

            Cd = A + C;
            Dd = B + D;

            t1 = (S32)(xC4S4 * (ip[0*8] + ip[4*8]));
            t1 >>= 16;
            E = t1;

            t1 = (S32)(xC4S4 * (ip[0*8] - ip[4*8]));
            t1 >>= 16;
            F = t1;

            t1 = (S32)(xC2S6 * ip[2*8]);
            t2 = (S32)(xC6S2 * ip[6*8]);
            t1 >>= 16;
            t2 >>= 16;
            G = t1 + t2;

            t1 = (S32)(xC6S2 * ip[2*8]);
            t2 = (S32)(xC2S6 * ip[6*8]);
            t1 >>= 16;
            t2 >>= 16;
            H = t1 - t2;

            Ed = E - G;
            Gd = E + G;

            Add = F + Ad;
            Bdd = Bd - H;

            Fd = F - Ad;
            Hd = Bd + H;

            Gd += IdctAdjustBeforeShift;
            Add += IdctAdjustBeforeShift;
            Ed += IdctAdjustBeforeShift;
            Fd += IdctAdjustBeforeShift;

            // Final sequence of operations over-write original inputs.
            op[0*8] = (S16)((Gd + Cd )  >> 4);
            op[7*8] = (S16)((Gd - Cd )  >> 4);

            op[1*8] = (S16)((Add + Hd ) >> 4);
            op[2*8] = (S16)((Add - Hd ) >> 4);

            op[3*8] = (S16)((Ed + Dd )  >> 4);
            op[4*8] = (S16)((Ed - Dd )  >> 4);

            op[5*8] = (S16)((Fd + Bdd ) >> 4);
            op[6*8] = (S16)((Fd - Bdd ) >> 4);
        }
        else
        {
            op[0*8] = 0;
            op[7*8] = 0;
            op[1*8] = 0;
            op[2*8] = 0;
            op[3*8] = 0;
            op[4*8] = 0;
            op[5*8] = 0;
            op[6*8] = 0;
        }
        ip++;            // next column
        op++;
    }
}

/****************************************************************************
 *
  *  ROUTINE       : IDctSlow10
 *
 *  INPUTS        : S16 *InputData   : Pointer to 8x8 quantized DCT coefficients.
 *                  S16 *QuantMatrix : Pointer to 8x8 quantization matrix.
 *
 *  OUTPUTS       : S16 *OutputData  : Pointer to 8x8 block to hold output.
 *
 *  RETURNS       : void
 *
 *  FUNCTION      : Inverse quantizes and inverse DCT's input 8x8 block
 *                  with non-zero coeffs only in DC & the first 9 AC coeffs.
 *                  i.e. non-zeros ONLY in the following 10 positions:
 *
 *                          x  x  x  x  0  0  0  0
 *                          x  x  x  0  0  0  0  0
 *                          x  x  0  0  0  0  0  0
 *                          x  0  0  0  0  0  0  0
 *                          0  0  0  0  0  0  0  0
 *                          0  0  0  0  0  0  0  0
 *                          0  0  0  0  0  0  0  0
 *                          0  0  0  0  0  0  0  0
 *
 *  SPECIAL NOTES : Output data is in raster, not zig-zag, order.
 *
 ****************************************************************************/

static void IDct10 ( S16 *InputData, S16 *QuantMatrix, S16 *OutputData )
{
    int   loop;
    S32 t1, t2;
    S32 IntermediateData[64];
    S32 A, B, C, D, Ad, Bd, Cd, Dd, E, F, G, H;
    S32 Ed, Gd, Add, Bdd, Fd, Hd;

    S32 *ip = IntermediateData;
    S16 *op = OutputData;

    // dequantize the input
    dequant_slow10 ( QuantMatrix, InputData, IntermediateData );
//
// OPTIMISATION #3
// ===============
// MHB (23/11/06) - Apply optimisation #3 (defined above) here.
//

    // Inverse DCT on the rows now
    for ( loop=0; loop<4; loop++ )
    {

//
// OPTIMISATION #4
// ===============
// MHB (23/11/06) - Apply optimisation #4 (defined above) here.
//

        // Check for non-zero values
        if ( ip[0] | ip[1] | ip[2] | ip[3] )
        {
            t1 = (S32)(xC1S7 * ip[1]);
            t1 >>= 16;
            A = t1;

            t1 = (S32)(xC7S1 * ip[1]);
            t1 >>= 16;
            B = t1 ;

            t1 = (S32)(xC3S5 * ip[3]);
            t1 >>= 16;
            C = t1;

            t2 = (S32)(xC5S3 * ip[3]);
            t2 >>= 16;
            D = -t2;

            t1 = (S32)(xC4S4 * (A - C));
            t1 >>= 16;
            Ad = t1;

            t1 = (S32)(xC4S4 * (B - D));
            t1 >>= 16;
            Bd = t1;

            Cd = A + C;
            Dd = B + D;

            t1 = (S32)(xC4S4 * ip[0] );
            t1 >>= 16;
            E = t1;

            F = t1;

            t1 = (S32)(xC2S6 * ip[2]);
            t1 >>= 16;
            G = t1;

            t1 = (S32)(xC6S2 * ip[2]);
            t1 >>= 16;
            H = t1 ;

            Ed = E - G;
            Gd = E + G;

            Add = F + Ad;
            Bdd = Bd - H;

            Fd = F - Ad;
            Hd = Bd + H;

            // Final sequence of operations over-write original inputs.
            ip[0] = (S16)((Gd + Cd )   >> 0);
            ip[7] = (S16)((Gd - Cd )   >> 0);

            ip[1] = (S16)((Add + Hd )  >> 0);
            ip[2] = (S16)((Add - Hd )  >> 0);

            ip[3] = (S16)((Ed + Dd )   >> 0);
            ip[4] = (S16)((Ed - Dd )   >> 0);

            ip[5] = (S16)((Fd + Bdd )  >> 0);
            ip[6] = (S16)((Fd - Bdd )  >> 0);
        }

        ip += 8;            /* next row */
    }

    ip = IntermediateData;

//
// OPTIMISATION #3
// ===============
// MHB (23/11/06) - Apply optimisation #3 (defined above) here.
//
    for ( loop=0; loop<8; loop++ )
    {
//
// OPTIMISATION #4
// ===============
// MHB (23/11/06) - Apply optimisation #4 (defined above) here.
//

        // Check for non-zero values (bitwise or faster than ||)
        if ( ip[0 * 8] | ip[1 * 8] | ip[2 * 8] | ip[3 * 8] )
        {
//
// OPTIMISATION #5
// ===============
// MHB (23/11/06) - Apply optimisation #5 (defined above) here.
//

            t1 = (S32)(xC1S7 * ip[1*8]);
            t1 >>= 16;
            A = t1 ;

            t1 = (S32)(xC7S1 * ip[1*8]);
            t1 >>= 16;
            B = t1 ;

            t1 = (S32)(xC3S5 * ip[3*8]);
            t1 >>= 16;
            C = t1 ;

            t2 = (S32)(xC5S3 * ip[3*8]);
            t2 >>= 16;
            D = - t2;

            t1 = (S32)(xC4S4 * (A - C));
            t1 >>= 16;
            Ad = t1;

            t1 = (S32)(xC4S4 * (B - D));
            t1 >>= 16;
            Bd = t1;

            Cd = A + C;
            Dd = B + D;

            t1 = (S32)(xC4S4 * ip[0*8]);
            t1 >>= 16;
            E = t1;
            F = t1;

            t1 = (S32)(xC2S6 * ip[2*8]);
            t1 >>= 16;
            G = t1;

            t1 = (S32)(xC6S2 * ip[2*8]);
            t1 >>= 16;
            H = t1;

            Ed = E - G;
            Gd = E + G;

            Add = F + Ad;
            Bdd = Bd - H;

            Fd = F - Ad;
            Hd = Bd + H;

            Gd += IdctAdjustBeforeShift;
            Add += IdctAdjustBeforeShift;
            Ed += IdctAdjustBeforeShift;
            Fd += IdctAdjustBeforeShift;

            // Final sequence of operations over-write original inputs.
            op[0*8] = (S16)((Gd + Cd )  >> 4);
            op[7*8] = (S16)((Gd - Cd )  >> 4);

            op[1*8] = (S16)((Add + Hd ) >> 4);
            op[2*8] = (S16)((Add - Hd ) >> 4);

            op[3*8] = (S16)((Ed + Dd )  >> 4);
            op[4*8] = (S16)((Ed - Dd )  >> 4);

            op[5*8] = (S16)((Fd + Bdd ) >> 4);
            op[6*8] = (S16)((Fd - Bdd ) >> 4);
        }
        else
        {
            op[0*8] = 0;
            op[1*8] = 0;
            op[2*8] = 0;
            op[3*8] = 0;
            op[4*8] = 0;
            op[5*8] = 0;
            op[6*8] = 0;
            op[7*8] = 0;
        }

        ip++;    // next column
        op++;
    }
}

/****************************************************************************
 *
 *  ROUTINE       : IDct1
 *
 *  INPUTS        : S16 *InputData   : Pointer to 8x8 quantized DCT coefficients.
 *                  S16 *QuantMatrix : Pointer to 8x8 quantization matrix.
 *
 *  OUTPUTS       : S16 *OutputData  : Pointer to 8x8 block to hold output.
 *
 *  RETURNS       : void
 *
 *  FUNCTION      : Inverse DCT's input 8x8 block with only one non-zero
 *                  coeff in the DC position:
 *
 *                          x   0   0  0  0  0  0  0
 *                          0   0   0  0  0  0  0  0
 *                          0   0   0  0  0  0  0  0
 *                          0   0   0  0  0  0  0  0
 *                          0   0   0  0  0  0  0  0
 *                          0   0   0  0  0  0  0  0
 *                          0   0   0  0  0  0  0  0
 *                          0   0   0  0  0  0  0  0
 *
 *  SPECIAL NOTES : Output data is in raster, not zig-zag, order.
 *
 ****************************************************************************/

static void IDct1 ( S16 *InputData, S16 *QuantMatrix, S16 *OutputData )
{
    S32 loop;
    S16 OutD;

    OutD = (S16)((S32)(InputData[0]*QuantMatrix[0]+15)>>5);

//
// OPTIMISATION #3
// ===============
// MHB (23/11/06) - Apply optimisation #3 (defined above) here.
//

//
// OPTIMISATION #7
// ===============
// MHB (23/11/06) - If the pointer OutputData can always be guaranteed
// to be 4 byte aligned (which it is) then the loop could be optimised
// to perform a 32 bit write of the constant "OutD32=OutD|(OutD<<16);".
//
// 1) Replace "OutputData[loop]" with a "*pDest++" construct and compare
// the results.
//
// 2) Replace the "for" loop with a "do/while" loop construct and compare
// the results.
//
// Care must be taken with casts to ensure that OutD32 is the value
// expected.
//
// If the code performance is still not acceptable it will need to be
// replaced with a hand crafted ARM/THUMB replacement.
//

    for ( loop=0; loop<64; loop++ )
        OutputData[loop] = OutD;
}


static void IDCT0_and_Recon_Inter(U8 *CurrFrame, U8 *RefFrame,U32 LineStep)
{
#ifdef VP6_COMBUV_IN_DECODING
  U32 u4SwapLen=LineStep&0xffff;
  U32 u4CompType=(LineStep>>16);
  U32 lCnt=8; // checked
  //do
  {
      
      if(u4CompType==VP6_Y_COMPONENT)
      {
          U32 *lCurrFramePtr ,*lRefFramePtr;
          do
          {   
            lCurrFramePtr=(U32 *)CurrFrame;
            lRefFramePtr  = (U32 *)RefFrame;

            *lCurrFramePtr++ = *lRefFramePtr++;
            *(lCurrFramePtr) = *(lRefFramePtr);
            CurrFrame += u4SwapLen;
            RefFrame  += u4SwapLen;
          }while(--lCnt);
          
      }
      else if(u4CompType==VP6_U_COMPONENT)
      {
          U32 *lCurrFramePtr ,*lRefFramePtr;
          do
          {   
              lCurrFramePtr=(U32 *)CurrFrame;
              lRefFramePtr  = (U32 *)RefFrame;

              *(lCurrFramePtr)=((*lCurrFramePtr)&(VP6_V_4MASK))|((*lRefFramePtr)&VP6_U_4MASK);
              lCurrFramePtr++;
              lRefFramePtr++;
              *(lCurrFramePtr)=((*lCurrFramePtr)&(VP6_V_4MASK))|((*lRefFramePtr)&VP6_U_4MASK);
              lCurrFramePtr++;
              lRefFramePtr++;
              *(lCurrFramePtr)=((*lCurrFramePtr)&(VP6_V_4MASK))|((*lRefFramePtr)&VP6_U_4MASK);
              lCurrFramePtr++;
              lRefFramePtr++;
              *(lCurrFramePtr)=((*lCurrFramePtr)&(VP6_V_4MASK))|((*lRefFramePtr)&VP6_U_4MASK);

              CurrFrame += u4SwapLen;
              RefFrame  += u4SwapLen;
          }while(--lCnt);
      }
      else
      {
          U32 *lCurrFramePtr ,*lRefFramePtr;
          do
          {   
              lCurrFramePtr=(U32 *)CurrFrame;
              lRefFramePtr  = (U32 *)RefFrame;
              *(lCurrFramePtr)=((*lCurrFramePtr)&(VP6_U_4MASK))|((*lRefFramePtr)&VP6_V_4MASK);
              lCurrFramePtr++;
              lRefFramePtr++;
              *(lCurrFramePtr)=((*lCurrFramePtr)&(VP6_U_4MASK))|((*lRefFramePtr)&VP6_V_4MASK);
              lCurrFramePtr++;
              lRefFramePtr++;
              *(lCurrFramePtr)=((*lCurrFramePtr)&(VP6_U_4MASK))|((*lRefFramePtr)&VP6_V_4MASK);
              lCurrFramePtr++;
              lRefFramePtr++;
              *(lCurrFramePtr)=((*lCurrFramePtr)&(VP6_U_4MASK))|((*lRefFramePtr)&VP6_V_4MASK);
              CurrFrame += u4SwapLen;
              RefFrame  += u4SwapLen;
          }while(--lCnt);
      }
      
  }//while(--lCnt);
#else
  U32 lCnt;
  for(lCnt=0; lCnt<8; lCnt++)
  {
    U32 *lCurrFramePtr = (U32 *)CurrFrame;
    U32 *lRefFramePtr  = (U32 *)RefFrame;

    *(lCurrFramePtr) = *(lRefFramePtr);
    lCurrFramePtr++;
    lRefFramePtr++;
    *(lCurrFramePtr) = *(lRefFramePtr);

    CurrFrame += LineStep;
    RefFrame  += LineStep;
  }
#endif
}


static void IDCT0_and_Recon_Intra(U8 *CurrFrame, U16 *ReconDataBuffer, U32 LineStep)
{
#ifdef VP6_COMBUV_IN_DECODING
    U32 lCnt=8,u4SwapLen=LineStep&0xffff,u4CompType=LineStep>>16;
    //checked
      if(!u4CompType) // Y component
      {
        do
        {
          *((U32 *)CurrFrame)=0x80808080;
          *((U32 *)CurrFrame+1)=0x80808080;
          CurrFrame += u4SwapLen;
        }while(--lCnt);
      }
      else if(u4CompType==VP6_U_COMPONENT)
      {
         U32 u4Value;
         do
         {
           u4Value=*((U32 *)CurrFrame);
           *((U32 *)CurrFrame)=(u4Value&(VP6_V_4MASK))|0x00800080;
           u4Value=*((U32 *)CurrFrame+1);
           *((U32 *)CurrFrame+1)=(u4Value&(VP6_V_4MASK))|0x00800080; 
           u4Value=*((U32 *)CurrFrame+2);
           *((U32 *)CurrFrame+2)=(u4Value&(VP6_V_4MASK))|0x00800080; 
           u4Value=*((U32 *)CurrFrame+3);
           *((U32 *)CurrFrame+3)=(u4Value&(VP6_V_4MASK))|0x00800080;            
           CurrFrame += u4SwapLen;
         }while(--lCnt);
      }
      else
      {
         U32 u4Value;
         do
         {
           u4Value=*((U32 *)CurrFrame);
           *((U32 *)CurrFrame)=(u4Value&(VP6_U_4MASK))|0x80008000;
           u4Value=*((U32 *)CurrFrame+1);
           *((U32 *)CurrFrame+1)=(u4Value&(VP6_U_4MASK))|0x80008000;
           u4Value=*((U32 *)CurrFrame+2);
           *((U32 *)CurrFrame+2)=(u4Value&(VP6_U_4MASK))|0x80008000;
           u4Value=*((U32 *)CurrFrame+3);
           *((U32 *)CurrFrame+3)=(u4Value&(VP6_U_4MASK))|0x80008000; 
           CurrFrame += u4SwapLen;
         }while(--lCnt);
      }
#else
    U32 lCnt;
    U32 lValue = 0x80808080;
    
    for(lCnt=0; lCnt<8; lCnt++)
    {
        U32 *lCurrFramePtr = (U32 *)CurrFrame;
        *(lCurrFramePtr) = lValue;
        lCurrFramePtr++;

        *(lCurrFramePtr) = lValue;
        CurrFrame += LineStep;
    }
#endif

}


static void IDCT0_and_ReconBlock_8(U8 *PredBuffer, U8 *CurrFrame, U32 LineStep)
{
#ifdef VP6_COMBUV_IN_DECODING
  U32 lCnt=8,u4SwapLen=LineStep&0xffff,u4CompType=LineStep>>16;
  //do //checked
  {
    if(!u4CompType)//Y Component
    {
       do
       {
         *((U32 *)CurrFrame)=*((U32 *)PredBuffer);
         *((U32 *)CurrFrame+1)=*((U32 *)PredBuffer+1);
         CurrFrame += u4SwapLen;
         PredBuffer+=8;
       }while(--lCnt);
    }
    else if(u4CompType==VP6_U_COMPONENT)
    {
        U32 u4Value,u4PredValue;

        do
        {
           u4PredValue=*((U32 *)PredBuffer);
           u4Value=*((U32 *)CurrFrame);
           *((U32 *)CurrFrame)= (u4Value&VP6_V_4MASK)|(u4PredValue&0xff) | ((u4PredValue&0xff00)<<8);
           u4Value=*((U32 *)CurrFrame+1);
           *((U32 *)CurrFrame+1)= (u4Value&VP6_V_4MASK)|((u4PredValue&0xff0000)>>16)|((u4PredValue>>8)&0xff0000);
            
           u4PredValue=*((U32 *)PredBuffer+1);
           u4Value=*((U32 *)CurrFrame+2);
           *((U32 *)CurrFrame+2)= (u4Value&VP6_V_4MASK)|(u4PredValue&0xff) | ((u4PredValue&0xff00)<<8);
           u4Value=*((U32 *)CurrFrame+3);
           *((U32 *)CurrFrame+3)= (u4Value&VP6_V_4MASK)|((u4PredValue&0xff0000)>>16)|((u4PredValue>>8)&0xff0000);        
           CurrFrame += u4SwapLen;
           PredBuffer+=8;
        }while(--lCnt);
     }
    else
    {
        U32 u4Value,u4PredValue;
        do
        {
           u4PredValue=*((U32 *)PredBuffer);
           u4Value=*((U32 *)CurrFrame);
           *((U32 *)CurrFrame)= (u4Value&(VP6_U_4MASK))|((u4PredValue&0xff)<<8) | ((u4PredValue&0xff00)<<16);
           u4Value=*((U32 *)CurrFrame+1);
           *((U32 *)CurrFrame+1)= (u4Value&(VP6_U_4MASK))|((u4PredValue&0xff0000)>>8)|(u4PredValue&0xff000000);
        
           u4PredValue=*((U32 *)PredBuffer+1);
           u4Value=*((U32 *)CurrFrame+2);
           *((U32 *)CurrFrame+2)= (u4Value&(VP6_U_4MASK))|((u4PredValue&0xff)<<8) | ((u4PredValue&0xff00)<<16);
           u4Value=*((U32 *)CurrFrame+3);
           *((U32 *)CurrFrame+3)= (u4Value&(VP6_U_4MASK))|((u4PredValue&0xff0000)>>8)|(u4PredValue&0xff000000);            
           CurrFrame += u4SwapLen;
           PredBuffer+=8;
        }while(--lCnt);
    }
  }//while(--lCnt);
  
#else
  U32 lCnt;
  for(lCnt=0; lCnt<8; lCnt++)
  {
    U32 *lCurrFramePtr = (U32 *)CurrFrame;
    U32 *PredBufferPtr = (U32 *)PredBuffer;

    *(lCurrFramePtr++) = *(PredBufferPtr++);
    *(lCurrFramePtr++) = *(PredBufferPtr++);
    
    CurrFrame += LineStep;
    PredBuffer+= 8; 
  }
#endif
}


static void IDCT1_plus_ReconBlock(U8 *PredBuffer, U8 *CurrFrame, S16 OutD, U32 LineStep)
{
#ifdef VP6_COMBUV_IN_DECODING
U32 lCnt=8,u4Value=0,u4SwapLen=LineStep&0xffff,u4CompType=LineStep>>16;
S32 i4Value; //checked

//do
{ 
  if(OutD<0)
  {
     if(!u4CompType)//Y Component
     {
         U32 u4NewValue;
         do
         {
             u4Value=*((U32 *)PredBuffer);
             i4Value=(u4Value>>24)+OutD;
             u4NewValue=IDCT_CLAMP0(i4Value)<<24;
             i4Value=(0xff&(u4Value>>16))+OutD;
             u4NewValue+=IDCT_CLAMP0(i4Value)<<16;
             i4Value=(0xff&(u4Value>>8))+OutD;
             u4NewValue+=IDCT_CLAMP0(i4Value)<<8;
             i4Value=(0xff&u4Value)+OutD;
             u4NewValue+=IDCT_CLAMP0(i4Value);
             *((U32 *)CurrFrame)=u4NewValue;
             
             u4Value=*((U32 *)PredBuffer+1);
             i4Value=(u4Value>>24)+OutD;
             u4NewValue=IDCT_CLAMP0(i4Value)<<24;
             i4Value=(0xff&(u4Value>>16))+OutD;
             u4NewValue+=IDCT_CLAMP0(i4Value)<<16;
             i4Value=(0xff&(u4Value>>8))+OutD;
             u4NewValue+=IDCT_CLAMP0(i4Value)<<8;
             i4Value=(0xff&u4Value)+OutD;
             u4NewValue+=IDCT_CLAMP0(i4Value);
             *((U32 *)CurrFrame+1)=u4NewValue;

             PredBuffer+=8;
             CurrFrame+=u4SwapLen;
         }while(--lCnt);
     }
     else if(u4CompType==VP6_U_COMPONENT) 
     {
         U32 NewValue,u4TmpVal;

         do
         {
             u4Value=*((U32 *)PredBuffer);
             i4Value=(u4Value&0xff)+OutD;
             NewValue=IDCT_CLAMP0(i4Value);
             i4Value=(0xff&(u4Value>>8))+OutD;
             NewValue+=IDCT_CLAMP0(i4Value)<<16;
             u4TmpVal=*((U32 *)CurrFrame);
             *((U32 *)CurrFrame)=(u4TmpVal&(VP6_V_4MASK))|NewValue;
             
             i4Value=(0xff&(u4Value>>16))+OutD;
             NewValue=IDCT_CLAMP0(i4Value);
             i4Value=(u4Value>>24)+OutD;
             NewValue+=IDCT_CLAMP0(i4Value)<<16;
             u4TmpVal=*((U32 *)CurrFrame+1);
             *((U32 *)CurrFrame+1)=(u4TmpVal&(VP6_V_4MASK))|NewValue;
             
             u4Value=*((U32 *)PredBuffer+1);
             i4Value=(u4Value&0xff)+OutD;
             NewValue=IDCT_CLAMP0(i4Value);
             i4Value=(0xff&(u4Value>>8))+OutD;
             NewValue+=IDCT_CLAMP0(i4Value)<<16;
             u4TmpVal=*((U32 *)CurrFrame+2);
             *((U32 *)CurrFrame+2)=(u4TmpVal&(VP6_V_4MASK))|NewValue;
             
             i4Value=(0xff&(u4Value>>16))+OutD;
             NewValue=IDCT_CLAMP0(i4Value);
             i4Value=(u4Value>>24)+OutD;
             NewValue+=IDCT_CLAMP0(i4Value)<<16;
             u4TmpVal=*((U32 *)CurrFrame+3);
             *((U32 *)CurrFrame+3)=(u4TmpVal&(VP6_V_4MASK))|NewValue;

             PredBuffer+=8;
             CurrFrame+=u4SwapLen;
         }while(--lCnt);
     }
     else //V Component
     {
         U32 NewValue,u4TmpVal;

         do
         {
             u4Value=*((U32 *)PredBuffer);
             i4Value=(0xff&u4Value)+OutD;
             NewValue=IDCT_CLAMP0(i4Value)<<8;
             i4Value=(0xff&(u4Value>>8))+OutD;
             NewValue+=IDCT_CLAMP0(i4Value)<<24;
             u4TmpVal=*((U32 *)CurrFrame);
             *((U32 *)CurrFrame)=(u4TmpVal&(VP6_U_4MASK))|NewValue;
             
             i4Value=(0xff&(u4Value>>16))+OutD;
             NewValue=IDCT_CLAMP0(i4Value)<<8;
             i4Value=(u4Value>>24)+OutD;
             NewValue+=IDCT_CLAMP0(i4Value)<<24;
             u4TmpVal=*((U32 *)CurrFrame+1);
             *((U32 *)CurrFrame+1)=(u4TmpVal&VP6_U_4MASK)|NewValue;

             u4Value=*((U32 *)PredBuffer+1);
             i4Value=(0xff&u4Value)+OutD;
             NewValue=IDCT_CLAMP0(i4Value)<<8;
             i4Value=(0xff&(u4Value>>8))+OutD;
             NewValue+=IDCT_CLAMP0(i4Value)<<24;
             u4TmpVal=*((U32 *)CurrFrame+2);
             *((U32 *)CurrFrame+2)=(u4TmpVal&VP6_U_4MASK)|NewValue;
             
             i4Value=(0xff&(u4Value>>16))+OutD;
             NewValue=IDCT_CLAMP0(i4Value)<<8;
             i4Value=(u4Value>>24)+OutD;
             NewValue+=IDCT_CLAMP0(i4Value)<<24;
             u4TmpVal=*((U32 *)CurrFrame+3);
             *((U32 *)CurrFrame+3)=(u4TmpVal&VP6_U_4MASK)|NewValue;

             PredBuffer+=8;
             CurrFrame+=u4SwapLen;
         }while(--lCnt);
     }

  }   
  else
  {
      if(!u4CompType)//Y Component
      {
          U32 u4NewValue;
          do
          {
              u4Value=*((U32 *)PredBuffer);
              i4Value=(u4Value>>24)+OutD;
              u4NewValue=IDCT_CLAMP255(i4Value)<<24;
              i4Value=(0xff&(u4Value>>16))+OutD;
              u4NewValue+=IDCT_CLAMP255(i4Value)<<16;
              i4Value=(0xff&(u4Value>>8))+OutD;
              u4NewValue+=IDCT_CLAMP255(i4Value)<<8;
              i4Value=(0xff&u4Value)+OutD;
              u4NewValue+=IDCT_CLAMP255(i4Value);
              *((U32 *)CurrFrame)=u4NewValue;
              
              u4Value=*((U32 *)PredBuffer+1);
              i4Value=(u4Value>>24)+OutD;
              u4NewValue=IDCT_CLAMP255(i4Value)<<24;
              i4Value=(0xff&(u4Value>>16))+OutD;
              u4NewValue+=IDCT_CLAMP255(i4Value)<<16;
              i4Value=(0xff&(u4Value>>8))+OutD;
              u4NewValue+=IDCT_CLAMP255(i4Value)<<8;
              i4Value=(0xff&u4Value)+OutD;
              u4NewValue+=IDCT_CLAMP255(i4Value);
              *((U32 *)CurrFrame+1)=u4NewValue;

              PredBuffer+=8;
              CurrFrame+=u4SwapLen;
          }while(--lCnt);
      }
      else if(u4CompType==VP6_U_COMPONENT) 
      {
          U32 NewValue,u4TmpVal;
          do
          {
              u4Value=*((U32 *)PredBuffer);
              i4Value=(u4Value&0xff)+OutD;
              NewValue=IDCT_CLAMP255(i4Value);
              i4Value=(0xff&(u4Value>>8))+OutD;
              NewValue+=IDCT_CLAMP255(i4Value)<<16;
              u4TmpVal=*((U32 *)CurrFrame);
              *((U32 *)CurrFrame)=(u4TmpVal&(VP6_V_4MASK))|NewValue;
              
              i4Value=(0xff&(u4Value>>16))+OutD;
              NewValue=IDCT_CLAMP255(i4Value);
              i4Value=(u4Value>>24)+OutD;
              NewValue+=IDCT_CLAMP255(i4Value)<<16;
              u4TmpVal=*((U32 *)CurrFrame+1);
              *((U32 *)CurrFrame+1)=(u4TmpVal&(VP6_V_4MASK))|NewValue;
              
              u4Value=*((U32 *)PredBuffer+1);
              i4Value=(u4Value&0xff)+OutD;
              NewValue=IDCT_CLAMP255(i4Value);
              i4Value=(0xff&(u4Value>>8))+OutD;
              NewValue+=IDCT_CLAMP255(i4Value)<<16;
              u4TmpVal=*((U32 *)CurrFrame+2);
              *((U32 *)CurrFrame+2)=(u4TmpVal&(VP6_V_4MASK))|NewValue;
              
              i4Value=(0xff&(u4Value>>16))+OutD;
              NewValue=IDCT_CLAMP255(i4Value);
              i4Value=(u4Value>>24)+OutD;
              NewValue+=IDCT_CLAMP255(i4Value)<<16;
              u4TmpVal=*((U32 *)CurrFrame+3);
              *((U32 *)CurrFrame+3)=(u4TmpVal&(VP6_V_4MASK))|NewValue;

              PredBuffer+=8;
              CurrFrame+=u4SwapLen;
          }while(--lCnt);     
      }
      else //V Component
      {
          U32 NewValue,u4TmpVal;
          do
          {
              u4Value=*((U32 *)PredBuffer);
              i4Value=(0xff&u4Value)+OutD;
              NewValue=IDCT_CLAMP255(i4Value)<<8;
              i4Value=(0xff&(u4Value>>8))+OutD;
              NewValue+=IDCT_CLAMP255(i4Value)<<24;
              u4TmpVal=*((U32 *)CurrFrame);
              *((U32 *)CurrFrame)=(u4TmpVal&(VP6_U_4MASK))|NewValue;
              
              i4Value=(0xff&(u4Value>>16))+OutD;
              NewValue=IDCT_CLAMP255(i4Value)<<8;
              i4Value=(u4Value>>24)+OutD;
              NewValue+=IDCT_CLAMP255(i4Value)<<24;
              u4TmpVal=*((U32 *)CurrFrame+1);
              *((U32 *)CurrFrame+1)=(u4TmpVal&VP6_U_4MASK)|NewValue;
              
              u4Value=*((U32 *)PredBuffer+1);
              i4Value=(0xff&u4Value)+OutD;
              NewValue=IDCT_CLAMP255(i4Value)<<8;
              i4Value=(0xff&(u4Value>>8))+OutD;
              NewValue+=IDCT_CLAMP255(i4Value)<<24;
              u4TmpVal=*((U32 *)CurrFrame+2);
              *((U32 *)CurrFrame+2)=(u4TmpVal&VP6_U_4MASK)|NewValue;
              
              i4Value=(0xff&(u4Value>>16))+OutD;
              NewValue=IDCT_CLAMP255(i4Value)<<8;
              i4Value=(u4Value>>24)+OutD;
              NewValue+=IDCT_CLAMP255(i4Value)<<24;
              u4TmpVal=*((U32 *)CurrFrame+3);
              *((U32 *)CurrFrame+3)=(u4TmpVal&VP6_U_4MASK)|NewValue;
         
              PredBuffer+=8;
              CurrFrame+=u4SwapLen;
          }while(--lCnt);         
      }

  }
  
}//while(--lCnt);
#else
    U32    lCnt;
    U32 lTemp1, lTemp2, lTemp3, lTemp4, lTemp5;
    U32 *lCurrFramePtr;
    U8 *lPredBuffPtr;
    
    for(lCnt=0; lCnt<8; lCnt++)
    {
        lCurrFramePtr = (U32 *)CurrFrame;
        lPredBuffPtr  = PredBuffer;
        
        lTemp1 = LIMIT(lPredBuffPtr[0] + OutD);
        lTemp2 = LIMIT(lPredBuffPtr[1] + OutD);
        lTemp3 = LIMIT(lPredBuffPtr[2] + OutD);
        lTemp4 = LIMIT(lPredBuffPtr[3] + OutD);
        lTemp5 = ( (lTemp1) | (lTemp2 << 8) | (lTemp3 << 16) | (lTemp4 << 24) );        
        
        *(lCurrFramePtr) = lTemp5;
        lCurrFramePtr++;        
        
        lTemp1 = LIMIT(lPredBuffPtr[4] + OutD);
        lTemp2 = LIMIT(lPredBuffPtr[5] + OutD);
        lTemp3 = LIMIT(lPredBuffPtr[6] + OutD);
        lTemp4 = LIMIT(lPredBuffPtr[7] + OutD);
        lTemp5 = ( (lTemp1) | (lTemp2 << 8) | (lTemp3 << 16) | (lTemp4 << 24) );        
        *(lCurrFramePtr) = lTemp5;

        CurrFrame         += LineStep;
        PredBuffer      += 8;                
    }
#endif    
}


static void IDCT1_plus_ReconInter(U8 *CurrFrame, U8 *RefFrame, S16 OutD, U32 LineStep)
{
#ifdef VP6_COMBUV_IN_DECODING
  U32 lCnt=8,u4Value=0,u4SwapLen=LineStep&0xffff,u4CompType=LineStep>>16;
  S32 i4Value;
  // checked
  //do
  { 
    if(OutD<0)
    {
       if(!u4CompType)//Y Component
       {
           U32 NewValue;
           do
           {
               u4Value=*((U32 *)RefFrame);
               i4Value=(u4Value>>24)+OutD;
               NewValue=IDCT_CLAMP0(i4Value)<<24;
               i4Value=(0xff&(u4Value>>16))+OutD;
               NewValue+=IDCT_CLAMP0(i4Value)<<16;
               i4Value=(0xff&(u4Value>>8))+OutD;
               NewValue+=IDCT_CLAMP0(i4Value)<<8;
               i4Value=(0xff&u4Value)+OutD;
               NewValue+=IDCT_CLAMP0(i4Value);
               *((U32 *)CurrFrame)=NewValue;
               
               u4Value=*((U32 *)RefFrame+1);
               i4Value=(u4Value>>24)+OutD;
               NewValue=IDCT_CLAMP0(i4Value)<<24;
               i4Value=(0xff&(u4Value>>16))+OutD;
               NewValue+=IDCT_CLAMP0(i4Value)<<16;
               i4Value=(0xff&(u4Value>>8))+OutD;
               NewValue+=IDCT_CLAMP0(i4Value)<<8;
               i4Value=(0xff&u4Value)+OutD;
               NewValue+=IDCT_CLAMP0(i4Value);
               *((U32 *)CurrFrame+1)=NewValue;
               
              RefFrame+=u4SwapLen;
              CurrFrame+=u4SwapLen;
           }while(--lCnt);
       }
       else if(u4CompType==VP6_V_COMPONENT) // V componen t
       {
           U32 NewValue;
           do
           {
               u4Value=*((U32 *)(RefFrame));
               i4Value=(u4Value>>24)+OutD;
               NewValue=IDCT_CLAMP0(i4Value)<<24;
               i4Value=(0xff&(u4Value>>8))+OutD;
               NewValue+=IDCT_CLAMP0(i4Value)<<8;
               u4Value=*((U32 *)CurrFrame);
               *((U32 *)CurrFrame)=(u4Value&(VP6_U_4MASK))|NewValue;
               
               u4Value=*((U32 *)RefFrame+1);
               i4Value=(u4Value>>24)+OutD;
               NewValue=IDCT_CLAMP0(i4Value)<<24;
               i4Value=(0xff&(u4Value>>8))+OutD;
               NewValue+=IDCT_CLAMP0(i4Value)<<8;
               u4Value=*((U32 *)CurrFrame+1);
               *((U32 *)CurrFrame+1)=(u4Value&(VP6_U_4MASK))|NewValue;
               
               u4Value=*((U32 *)RefFrame+2);
               i4Value=(u4Value>>24)+OutD;
               NewValue=IDCT_CLAMP0(i4Value)<<24;
               i4Value=(0xff&(u4Value>>8))+OutD;
               NewValue+=IDCT_CLAMP0(i4Value)<<8;
               u4Value=*((U32 *)CurrFrame+2);
               *((U32 *)CurrFrame+2)=(u4Value&(VP6_U_4MASK))|NewValue;
               
               u4Value=*((U32 *)RefFrame+3);
               i4Value=(u4Value>>24)+OutD;
               NewValue=IDCT_CLAMP0(i4Value)<<24;
               i4Value=(0xff&(u4Value>>8))+OutD;
               NewValue+=IDCT_CLAMP0(i4Value)<<8;
               u4Value=*((U32 *)CurrFrame+3);
               *((U32 *)CurrFrame+3)=(u4Value&(VP6_U_4MASK))|NewValue;     
              RefFrame+=u4SwapLen;
              CurrFrame+=u4SwapLen;
           }while(--lCnt);
       }
       else //U Component
       {
           U32 NewValue;
           do
           {

             u4Value=*((U32 *)RefFrame);
             i4Value=(0xff&(u4Value>>16))+OutD;
             NewValue=IDCT_CLAMP0(i4Value)<<16;
             i4Value=(0xff&u4Value)+OutD;
             NewValue+=IDCT_CLAMP0(i4Value);
             u4Value=*((U32 *)CurrFrame);
             *((U32 *)CurrFrame)=(u4Value&(VP6_V_4MASK))|NewValue;

             u4Value=*((U32 *)RefFrame+1);
             i4Value=(0xff&(u4Value>>16))+OutD;
             NewValue=IDCT_CLAMP0(i4Value)<<16;
             i4Value=(0xff&u4Value)+OutD;
             NewValue+=IDCT_CLAMP0(i4Value);
             u4Value=*((U32 *)CurrFrame+1);
             *((U32 *)CurrFrame+1)=(u4Value&(VP6_V_4MASK))|NewValue;

             u4Value=*((U32 *)RefFrame+2);
             i4Value=(0xff&(u4Value>>16))+OutD;
             NewValue=IDCT_CLAMP0(i4Value)<<16;
             i4Value=(0xff&u4Value)+OutD;
             NewValue+=IDCT_CLAMP0(i4Value);
             u4Value=*((U32 *)CurrFrame+2);
             *((U32 *)CurrFrame+2)=(u4Value&(VP6_V_4MASK))|NewValue;

             u4Value=*((U32 *)RefFrame+3);
             i4Value=(0xff&(u4Value>>16))+OutD;
             NewValue=IDCT_CLAMP0(i4Value)<<16;
             i4Value=(0xff&u4Value)+OutD;
             NewValue+=IDCT_CLAMP0(i4Value);
             u4Value=*((U32 *)CurrFrame+3);
             *((U32 *)CurrFrame+3)=(u4Value&(VP6_V_4MASK))|NewValue;
             RefFrame+=u4SwapLen;
             CurrFrame+=u4SwapLen;
           }while(--lCnt);
       }

    }   
    else
    {
        if(!u4CompType)//Y Component
       {
           U32 NewValue;
           do
           {
               u4Value=*((U32 *)RefFrame);
               i4Value=(u4Value>>24)+OutD;
               NewValue=IDCT_CLAMP255(i4Value)<<24;
               i4Value=(0xff&(u4Value>>16))+OutD;
               NewValue+=IDCT_CLAMP255(i4Value)<<16;
               i4Value=(0xff&(u4Value>>8))+OutD;
               NewValue+=IDCT_CLAMP255(i4Value)<<8;
               i4Value=(0xff&u4Value)+OutD;
               NewValue+=IDCT_CLAMP255(i4Value);
               *((U32 *)CurrFrame)=NewValue;
               
               u4Value=*((U32 *)RefFrame+1);
               i4Value=(u4Value>>24)+OutD;
               NewValue=IDCT_CLAMP255(i4Value)<<24;
               i4Value=(0xff&(u4Value>>16))+OutD;
               NewValue+=IDCT_CLAMP255(i4Value)<<16;
               i4Value=(0xff&(u4Value>>8))+OutD;
               NewValue+=IDCT_CLAMP255(i4Value)<<8;
               i4Value=(0xff&u4Value)+OutD;
               NewValue+=IDCT_CLAMP255(i4Value);
               *((U32 *)CurrFrame+1)=NewValue;
               RefFrame+=u4SwapLen;
               CurrFrame+=u4SwapLen;
           }while(--lCnt);
       }
       else if(u4CompType==VP6_V_COMPONENT) // V componen t
       {
           U32 NewValue;
           do
           {
               u4Value=*((U32 *)RefFrame);
               i4Value=(u4Value>>24)+OutD;
               NewValue=IDCT_CLAMP255(i4Value)<<24;
               i4Value=(0xff&(u4Value>>8))+OutD;
               NewValue+=IDCT_CLAMP255(i4Value)<<8;
               u4Value=*((U32 *)CurrFrame);
               *((U32 *)CurrFrame)=(u4Value&(VP6_U_4MASK))|NewValue;
               
               u4Value=*((U32 *)RefFrame+1);
               i4Value=(u4Value>>24)+OutD;
               NewValue=IDCT_CLAMP255(i4Value)<<24;
               i4Value=(0xff&(u4Value>>8))+OutD;
               NewValue+=IDCT_CLAMP255(i4Value)<<8;
               u4Value=*((U32 *)CurrFrame+1);
               *((U32 *)CurrFrame+1)=(u4Value&(VP6_U_4MASK))|NewValue;
               
               u4Value=*((U32 *)RefFrame+2);
               i4Value=(u4Value>>24)+OutD;
               NewValue=IDCT_CLAMP255(i4Value)<<24;
               i4Value=(0xff&(u4Value>>8))+OutD;
               NewValue+=IDCT_CLAMP255(i4Value)<<8;
               u4Value=*((U32 *)CurrFrame+2);
               *((U32 *)CurrFrame+2)=(u4Value&(VP6_U_4MASK))|NewValue;
               
               u4Value=*((U32 *)RefFrame+3);
               i4Value=(u4Value>>24)+OutD;
               NewValue=IDCT_CLAMP255(i4Value)<<24;
               i4Value=(0xff&(u4Value>>8))+OutD;
               NewValue+=IDCT_CLAMP255(i4Value)<<8;
               u4Value=*((U32 *)CurrFrame+3);
               *((U32 *)CurrFrame+3)=(u4Value&(VP6_U_4MASK))|NewValue;  
               RefFrame+=u4SwapLen;
               CurrFrame+=u4SwapLen;
           }while(--lCnt);

       }
       else //U Component
       {
           U32 NewValue;
           do
           {
               u4Value=*((U32 *)RefFrame);
               i4Value=(0xff&(u4Value>>16))+OutD;
               NewValue=IDCT_CLAMP255(i4Value)<<16;
               i4Value=(0xff&u4Value)+OutD;
               NewValue+=IDCT_CLAMP255(i4Value);
               u4Value=*((U32 *)CurrFrame);
               *((U32 *)CurrFrame)=(u4Value&(VP6_V_4MASK))|NewValue;
               
               u4Value=*((U32 *)RefFrame+1);
               i4Value=(0xff&(u4Value>>16))+OutD;
               NewValue=IDCT_CLAMP255(i4Value)<<16;
               i4Value=(0xff&u4Value)+OutD;
               NewValue+=IDCT_CLAMP255(i4Value);
               u4Value=*((U32 *)CurrFrame+1);
               *((U32 *)CurrFrame+1)=(u4Value&(VP6_V_4MASK))|NewValue;
               
               u4Value=*((U32 *)RefFrame+2);
               i4Value=(0xff&(u4Value>>16))+OutD;
               NewValue=IDCT_CLAMP255(i4Value)<<16;
               i4Value=(0xff&u4Value)+OutD;
               NewValue+=IDCT_CLAMP255(i4Value);
               u4Value=*((U32 *)CurrFrame+2);
               *((U32 *)CurrFrame+2)=(u4Value&(VP6_V_4MASK))|NewValue;
               
               u4Value=*((U32 *)RefFrame+3);
               i4Value=(0xff&(u4Value>>16))+OutD;
               NewValue=IDCT_CLAMP255(i4Value)<<16;
               i4Value=(0xff&u4Value)+OutD;
               NewValue+=IDCT_CLAMP255(i4Value);
               u4Value=*((U32 *)CurrFrame+3);
               *((U32 *)CurrFrame+3)=(u4Value&(VP6_V_4MASK))|NewValue;
               RefFrame+=u4SwapLen;
               CurrFrame+=u4SwapLen;
           }while(--lCnt);

       }
    }
  }//while(--lCnt);

#else
  U32 lCnt;
  U32 lTemp1, lTemp2, lTemp3, lTemp4, lTemp5;
  U8 *lRefPtr;
  U32 *lCurrFramePtr;

  for(lCnt=0;lCnt<8;lCnt++)
  {
    lCurrFramePtr = (U32 *)CurrFrame;
    lRefPtr       = RefFrame;
        
    lTemp1 = LIMIT(lRefPtr[0] + OutD);
    lTemp2 = LIMIT(lRefPtr[1] + OutD);
    lTemp3 = LIMIT(lRefPtr[2] + OutD);
    lTemp4 = LIMIT(lRefPtr[3] + OutD);
    lTemp5 = ( (lTemp1) | (lTemp2 << 8) | (lTemp3 << 16) | (lTemp4 << 24) );
    
    *(lCurrFramePtr) = lTemp5;
    lCurrFramePtr++;
    
    lTemp1 = LIMIT(lRefPtr[4] + OutD);
    lTemp2 = LIMIT(lRefPtr[5] + OutD);
    lTemp3 = LIMIT(lRefPtr[6] + OutD);
    lTemp4 = LIMIT(lRefPtr[7] + OutD);
    lTemp5 = ( (lTemp1) | (lTemp2 << 8) | (lTemp3 << 16) | (lTemp4 << 24) );        
    *(lCurrFramePtr) = lTemp5;
    
    CurrFrame += LineStep;
    RefFrame  += LineStep;
  }
  
#endif
    
}


//*********************************** ReConstruct **************************//

static void ReconIntra_C( S16 *TmpDataBuffer, U8 *ReconPtr, U16 *ChangePtr, U32 LineStep )
{
#ifdef VP6_COMBUV_IN_DECODING
 U32 i=BLOCK_HEIGHT_WIDTH,u4SwapLen=LineStep&0xffff,u4CompType=LineStep>>16;
 U32 u4Value,u4Change;
 S16 i2Value;
 //Checked
 //do
 {
   if(!u4CompType)
   {
     do
     {
         u4Change=*((U32 *)ChangePtr);
         i2Value=(S16)(u4Change+0x00000080);
         u4Value=LIMIT(i2Value);
         i2Value=(S16)((u4Change>>16)+0x00000080);
         u4Value+=LIMIT(i2Value)<<8;
         
         u4Change=*((U32 *)ChangePtr+1);
         i2Value=(S16)(u4Change+0x00000080);
         u4Value+=LIMIT(i2Value)<<16;
         i2Value=(S16)((u4Change>>16)+0x00000080);
         u4Value+=LIMIT(i2Value)<<24;
         *((U32 *)ReconPtr)=u4Value;
         
         u4Change=*((U32 *)ChangePtr+2);
         i2Value=(S16)(u4Change+0x00000080);
         u4Value=LIMIT(i2Value);
         i2Value=(S16)((u4Change>>16)+0x00000080);
         u4Value+=LIMIT(i2Value)<<8;
         
         u4Change=*((U32 *)ChangePtr+3);
         i2Value=(S16)(u4Change+0x00000080);
         u4Value+=LIMIT(i2Value)<<16;
         i2Value=(S16)((u4Change>>16)+0x00000080);
         u4Value+=LIMIT(i2Value)<<24;
         *((U32 *)ReconPtr+1)=u4Value;
         ReconPtr += u4SwapLen;
         ChangePtr  += BLOCK_HEIGHT_WIDTH;
     }while(--i);
   }
   else if(u4CompType==VP6_V_COMPONENT) // V component
   {
     U32 u4TempVal;

     do
     {
         u4Change=*((U32 *)ChangePtr);
         i2Value=(S16)(u4Change+0x00000080);
         u4Value=LIMIT(i2Value)<<8;
         i2Value=(S16)((u4Change>>16)+0x00000080);
         u4Value+=LIMIT(i2Value)<<24;
         u4TempVal=*((U32 *)ReconPtr);
         *((U32 *)ReconPtr)=(u4TempVal&(VP6_U_4MASK))|u4Value;
         
         u4Change=*((U32 *)ChangePtr+1);
         i2Value=(S16)(u4Change+0x00000080);
         u4Value=LIMIT(i2Value)<<8;
         i2Value=(S16)((u4Change>>16)+0x00000080);
         u4Value+=LIMIT(i2Value)<<24;
         u4TempVal=*((U32 *)ReconPtr+1);
         *((U32 *)ReconPtr+1)=(u4TempVal&(VP6_U_4MASK))|u4Value ;
         
         u4Change=*((U32 *)ChangePtr+2);
         i2Value=(S16)(u4Change+0x00000080);
         u4Value=LIMIT(i2Value)<<8;
         i2Value=(S16)((u4Change>>16)+0x00000080);
         u4Value+=LIMIT(i2Value)<<24;
         u4TempVal=*((U32 *)ReconPtr+2);
         *((U32 *)ReconPtr+2)=(u4TempVal&(VP6_U_4MASK))|u4Value;
         
         u4Change=*((U32 *)ChangePtr+3);
         i2Value=(S16)(u4Change+0x00000080);
         u4Value=LIMIT(i2Value)<<8;
         i2Value=(S16)((u4Change>>16)+0x00000080);
         u4Value+=LIMIT(i2Value)<<24;
         u4TempVal=*((U32 *)ReconPtr+3);
         *((U32 *)ReconPtr+3)=(u4TempVal&(VP6_U_4MASK))|u4Value;
         ReconPtr += u4SwapLen;
         ChangePtr  += BLOCK_HEIGHT_WIDTH;
     }while(--i);
   }
   else  // U Component
   {
       U32 u4TempVal;
       do
       {
         u4Change=*((U32 *)ChangePtr);
         i2Value=(S16)(u4Change+0x00000080);
         u4Value=LIMIT(i2Value);
         i2Value=(S16)((u4Change>>16)+0x00000080);
         u4Value+=LIMIT(i2Value)<<16;
         u4TempVal=*((U32 *)ReconPtr);
         *((U32 *)ReconPtr)=(u4TempVal&(VP6_V_4MASK))|u4Value;
       
         u4Change=*((U32 *)ChangePtr+1);
         i2Value=(S16)(u4Change+0x00000080);
         u4Value=LIMIT(i2Value);
         i2Value=(S16)((u4Change>>16)+0x00000080);
         u4Value+=LIMIT(i2Value)<<16;
         u4TempVal=*((U32 *)ReconPtr+1);
         *((U32 *)ReconPtr+1)=(u4TempVal&(VP6_V_4MASK))|u4Value;
       
         u4Change=*((U32 *)ChangePtr+2);
         i2Value=(S16)(u4Change+0x00000080);
         u4Value=LIMIT(i2Value);
         i2Value=(S16)((u4Change>>16)+0x00000080);
         u4Value+=LIMIT(i2Value)<<16;
         u4TempVal=*((U32 *)ReconPtr+2);
         *((U32 *)ReconPtr+2)=(u4TempVal&(VP6_V_4MASK))|u4Value;
       
         u4Change=*((U32 *)ChangePtr+3);
         i2Value=(S16)(u4Change+0x00000080);
         u4Value=LIMIT(i2Value);
         i2Value=(S16)((u4Change>>16)+0x00000080);
         u4Value+=LIMIT(i2Value)<<16;
         u4TempVal=*((U32 *)ReconPtr+3);
         *((U32 *)ReconPtr+3)=(u4TempVal&(VP6_V_4MASK))|u4Value;
         ReconPtr += u4SwapLen;
         ChangePtr  += BLOCK_HEIGHT_WIDTH;
       }while(--i);
   }
 }//while(--i);
#else
  U32 i = BLOCK_HEIGHT_WIDTH;
  const U32 addValue = 0x00000080;
  do
  {
    U32* changePtrWord = (U32 *) (ChangePtr);
    U32* destinationWord = (U32 *) (ReconPtr);
    U32 a = *changePtrWord++;
    U32 b = *changePtrWord++;
    U32 c = *changePtrWord++;
    U32 d = *changePtrWord;

    S16 ah  =   (S16)((a >> 16)+ addValue);
    S16 bh  =   (S16)((b >> 16)+ addValue);
    S16 ch  =   (S16)((c >> 16)+ addValue);
    S16 dh  =   (S16)((d >> 16)+ addValue);

    S16 al  =  (S16)(a + addValue);
    S16 bl  =  (S16)(b + addValue);
    S16 cl  =  (S16)(c + addValue);
    S16 dl  =  (S16)(d + addValue);


    U32 d1 = LIMIT(al);
    d1 += (LIMIT(ah)<<8);
    d1 += (LIMIT(bl)<<16);
    d1 += (LIMIT(bh)<<24);
    U32 d2 = LIMIT(cl);
    d2 += (LIMIT(ch)<<8);
    d2 += (LIMIT(dl)<<16);
    d2 += (LIMIT(dh)<<24);
    *destinationWord++ = d1;
    *destinationWord = d2;

    ReconPtr += LineStep;
    ChangePtr  += BLOCK_HEIGHT_WIDTH;
  } while((--i)>0);
#endif
}


static void ReconInter_C( S16 *TmpDataBuffer, U8 *ReconPtr, U8 *RefPtr, S16 *ChangePtr, U32 LineStep )
{
#ifdef VP6_COMBUV_IN_DECODING
  U32 i=BLOCK_HEIGHT_WIDTH,u4SwapLen=LineStep&0xffff,u4CompType=LineStep>>16;
  U32 u4Value,u4Change,u4NewVal;
  S32 i4AddedValue; // Checked
 // do
  {
     if(!u4CompType)//Y Component
     {
       do
       {
           u4Value=*((U32 *)RefPtr);
           u4Change=*((U32 *)ChangePtr);
           i4AddedValue=(S32)(((S16)(u4Change&0xffff))+(u4Value&0xff));
           u4NewVal=LIMIT(i4AddedValue);
           i4AddedValue=(S32)(((S32)(u4Change&0Xffff0000))>>16)+((u4Value&0xff00)>>8);
           u4NewVal+=LIMIT(i4AddedValue)<<8;
           u4Change=*((U32 *)ChangePtr+1);
           i4AddedValue=(S32)((S16)(u4Change&0xffff))+((u4Value&0xff0000)>>16);
           u4NewVal+=LIMIT(i4AddedValue)<<16;
           i4AddedValue=(S32)(((S32)(u4Change))>>16)+(u4Value>>24);
           u4NewVal+=LIMIT(i4AddedValue)<<24;
           *((U32 *)ReconPtr)=u4NewVal;
           
           u4Value=*((U32 *)RefPtr+1);
           u4Change=*((U32 *)ChangePtr+2);
           i4AddedValue=(S32)(((S16)(u4Change&0xffff))+(u4Value&0xff));
           u4NewVal=LIMIT(i4AddedValue);
           i4AddedValue=(S32)(((S32)(u4Change&0Xffff0000))>>16)+((u4Value&0xff00)>>8);
           u4NewVal+=LIMIT(i4AddedValue)<<8;
           u4Change=*((U32 *)ChangePtr+3);
           i4AddedValue=(S32)((S16)(u4Change&0xffff))+((u4Value&0xff0000)>>16);
           u4NewVal+=LIMIT(i4AddedValue)<<16;
           i4AddedValue=(S32)(((S32)(u4Change))>>16)+(u4Value>>24);
           u4NewVal+=LIMIT(i4AddedValue)<<24;
           *((U32 *)ReconPtr+1)=u4NewVal;

           ChangePtr  += BLOCK_HEIGHT_WIDTH;
           RefPtr += u4SwapLen;
           ReconPtr+=u4SwapLen; 
       }while(--i);
     }
     else if(u4CompType==VP6_U_COMPONENT)
     {
         do
         {
             u4Value=*((U32 *)RefPtr);
             u4Change=*((U32 *)ChangePtr);
             i4AddedValue=(S32)((S16)(u4Change&0xffff))+(u4Value&0xff);
             u4NewVal=LIMIT(i4AddedValue);
             i4AddedValue=(S32)(((S32)(u4Change&0Xffff0000))>>16)+(0XFF&(u4Value>>16));
             u4NewVal+=LIMIT(i4AddedValue)<<16;
             u4Value=*((U32 *)ReconPtr);
             *((U32 *)ReconPtr)=(u4Value&(VP6_V_4MASK))|u4NewVal;
             
             u4Value=*((U32 *)RefPtr+1);
             u4Change=*((U32 *)ChangePtr+1);
             i4AddedValue=(S32)((S16)(u4Change&0xffff))+(u4Value&0xff);
             u4NewVal=LIMIT(i4AddedValue);
             i4AddedValue=(S32)(((S32)(u4Change&0Xffff0000))>>16)+(0XFF&(u4Value>>16));
             u4NewVal+=LIMIT(i4AddedValue)<<16;
             u4Value=*((U32 *)ReconPtr+1);
             *((U32 *)ReconPtr+1)=(u4Value&(VP6_V_4MASK))|u4NewVal;
             
             
             u4Value=*((U32 *)RefPtr+2);
             u4Change=*((U32 *)ChangePtr+2);
             i4AddedValue=(S32)((S16)(u4Change&0xffff))+(u4Value&0xff);
             u4NewVal=LIMIT(i4AddedValue);
             i4AddedValue=(S32)(((S32)(u4Change&0Xffff0000))>>16)+(0XFF&(u4Value>>16));
             u4NewVal+=LIMIT(i4AddedValue)<<16;
             u4Value=*((U32 *)ReconPtr+2);
             *((U32 *)ReconPtr+2)=(u4Value&(VP6_V_4MASK))|u4NewVal;
             
             u4Value=*((U32 *)RefPtr+3);
             u4Change=*((U32 *)ChangePtr+3);
             i4AddedValue=(S32)((S16)(u4Change&0xffff))+(u4Value&0xff);
             u4NewVal=LIMIT(i4AddedValue);
             i4AddedValue=(S32)(((S32)(u4Change&0Xffff0000))>>16)+(0XFF&(u4Value>>16));
             u4NewVal+=LIMIT(i4AddedValue)<<16;
             u4Value=*((U32 *)ReconPtr+3);
             *((U32 *)ReconPtr+3)=(u4Value&(VP6_V_4MASK))|u4NewVal;
         
             ChangePtr  += BLOCK_HEIGHT_WIDTH;
             RefPtr += u4SwapLen;
             ReconPtr+=u4SwapLen; 
         }while(--i);
     }
     else
     {
         do
         {
             u4Value=*((U32 *)RefPtr);
             u4Change=*((U32 *)ChangePtr);
             i4AddedValue=(S32)((S16)(u4Change&0xffff))+((u4Value&0xff00)>>8);
             u4NewVal=LIMIT(i4AddedValue)<<8;
             i4AddedValue=(S32)(((S32)(u4Change&0Xffff0000))>>16)+(u4Value>>24);
             u4NewVal+=LIMIT(i4AddedValue)<<24;
             u4Value=*((U32 *)ReconPtr);
             *((U32 *)ReconPtr)=(u4Value&(VP6_U_4MASK))|u4NewVal;
             
             u4Value=*((U32 *)RefPtr+1);
             u4Change=*((U32 *)ChangePtr+1);
             i4AddedValue=(S32)((S16)(u4Change&0xffff))+((u4Value&0xff00)>>8);
             u4NewVal=LIMIT(i4AddedValue)<<8;
             i4AddedValue=(S32)(((S32)(u4Change&0Xffff0000))>>16)+(u4Value>>24);
             u4NewVal+=LIMIT(i4AddedValue)<<24;
             u4Value=*((U32 *)ReconPtr+1);
             *((U32 *)ReconPtr+1)=(u4Value&(VP6_U_4MASK))|u4NewVal;
             
             u4Value=*((U32 *)RefPtr+2);
             u4Change=*((U32 *)ChangePtr+2);
             i4AddedValue=(S32)((S16)(u4Change&0xffff))+((u4Value&0xff00)>>8);
             u4NewVal=LIMIT(i4AddedValue)<<8;
             i4AddedValue=(S32)(((S32)(u4Change&0Xffff0000))>>16)+(u4Value>>24);
             u4NewVal+=LIMIT(i4AddedValue)<<24;
             u4Value=*((U32 *)ReconPtr+2);
             *((U32 *)ReconPtr+2)=(u4Value&(VP6_U_4MASK))|u4NewVal;
             
             
             u4Value=*((U32 *)RefPtr+3);
             u4Change=*((U32 *)ChangePtr+3);
             i4AddedValue=(S32)((S16)(u4Change&0xffff))+((u4Value&0xff00)>>8);
             u4NewVal=LIMIT(i4AddedValue)<<8;
             i4AddedValue=(S32)(((S32)(u4Change&0Xffff0000))>>16)+(u4Value>>24);
             u4NewVal+=LIMIT(i4AddedValue)<<24;
             u4Value=*((U32 *)ReconPtr+3);
             *((U32 *)ReconPtr+3)=(u4Value&(VP6_U_4MASK))|u4NewVal;   
         
             ChangePtr  += BLOCK_HEIGHT_WIDTH;
             RefPtr += u4SwapLen;
             ReconPtr+=u4SwapLen; 
         }while(--i);
     }
     
  }// while(--i);
#else
  U32 i = BLOCK_HEIGHT_WIDTH;
  S32 addedValue;
  U32 d1,d2;
  U32 r1,r2,c1,c2,c3,c4;
  do 
  {
        r1 = *(U32 *) RefPtr;
        r2 = *(U32 *) (RefPtr+4);
        c1 = *(U32 *) (ChangePtr);
        c2 = *(U32 *) (ChangePtr+2);
        c3 = *(U32 *) (ChangePtr+4);
        c4 = *(U32 *) (ChangePtr+6);
        
        d1 = LIMIT((S16)(((S16)(c1&0xFFFF))+(r1&0xFF)));
        addedValue = (S32)(((S32)(c1&0xFFFF0000))>>16)+((r1&0xFF00)>>8);
        d1 += LIMIT(addedValue)<<8;
        addedValue = (S32)((S16)(c2&0xFFFF))+((r1&0xFF0000)>>16);
        d1 += LIMIT(addedValue)<<16;
        addedValue = (S32)(((S32)(c2))>>16)+((r1)>>24);
        d1 += LIMIT(addedValue)<<24;

        d2 = LIMIT((S16)(((S16)(c3&0xFFFF))+(r2&0xFF)));
        addedValue = (S32)(((S32)(c3&0xFFFF0000))>>16)+((r2&0xFF00)>>8);
        d2 += LIMIT(addedValue)<<8;
        addedValue = (S32)((S16)(c4&0xFFFF))+((r2&0xFF0000)>>16);
        d2 += LIMIT(addedValue)<<16;
        addedValue = (S32)(((S32)(c4))>>16)+((r2)>>24);
        d2 += LIMIT(addedValue)<<24;

        *(U32 *) ReconPtr = d1;
        *(U32 *) (ReconPtr + 4) = d2;

        // Next row of Block
        ChangePtr  += BLOCK_HEIGHT_WIDTH;
        RefPtr += LineStep;
        ReconPtr   += LineStep; 
    } while((--i)>0);
#endif
}

static void ReconBlock_C_8 ( U8 *SrcBlock, S16 *ReconRefPtr, U8 *DestBlock, U32 LineStep )
{
#ifdef VP6_COMBUV_IN_DECODING
  U32 i=BLOCK_HEIGHT_WIDTH,u4SwapLen=LineStep&0xffff,u4CompType=LineStep>>16;
  U32 u4SrcVal,u4RefVal,u4Value;
  S32 i4Value;
  //checked
  //do
  {
     if(!u4CompType)
     {

         do
         {
             u4SrcVal=*((U32 *)SrcBlock);
             u4RefVal=*((U32 *)ReconRefPtr);
             i4Value=(u4SrcVal&0xFF)+(S16)(u4RefVal&0xFFFF);
             u4Value=LIMIT(i4Value);
             i4Value=((u4SrcVal>>8)&0xFF)+(S16)((u4RefVal>>16)&0xFFFF);
             u4Value+=LIMIT(i4Value)<<8;
             u4RefVal=*((U32 *)ReconRefPtr+1);
             i4Value=((u4SrcVal>>16)&0xFF)+(S16)(u4RefVal&0xFFFF);
             u4Value+=LIMIT(i4Value)<<16;
             i4Value=(u4SrcVal>>24)+(S16)((u4RefVal>>16)&0xFFFF);
             u4Value+=LIMIT(i4Value)<<24;
             *((U32 *)DestBlock)=u4Value;
             
             u4SrcVal=*((U32 *)SrcBlock+1);
             u4RefVal=*((U32 *)ReconRefPtr+2);
             i4Value=(u4SrcVal&0xFF)+(S16)(u4RefVal&0xFFFF);
             u4Value=LIMIT(i4Value);
             i4Value=((u4SrcVal>>8)&0xFF)+(S16)((u4RefVal>>16)&0xFFFF);
             u4Value+=LIMIT(i4Value)<<8;
             u4RefVal=*((U32 *)ReconRefPtr+3);
             i4Value=((u4SrcVal>>16)&0xFF)+(S16)(u4RefVal&0xFFFF);
             u4Value+=LIMIT(i4Value)<<16;
             i4Value=(u4SrcVal>>24)+(S16)((u4RefVal>>16)&0xFFFF);
             u4Value+=LIMIT(i4Value)<<24;
             *((U32 *)DestBlock+1)=u4Value;
             SrcBlock    += BLOCK_HEIGHT_WIDTH;
             ReconRefPtr += BLOCK_HEIGHT_WIDTH;
             DestBlock += u4SwapLen;
         }while(--i);
     }
     else if(u4CompType==VP6_U_COMPONENT) //   U  component
     {  
     
       U32 u4TempVal;
       do
       {
         u4SrcVal=*((U32 *)SrcBlock);
         u4RefVal=*((U32 *)ReconRefPtr);
         i4Value=(u4SrcVal&0xFF)+(S16)(u4RefVal&0xFFFF);
         u4Value=LIMIT(i4Value);
         i4Value=((u4SrcVal>>8)&0xFF)+(S16)((u4RefVal>>16)&0xFFFF);
         u4Value+=LIMIT(i4Value)<<16;
         u4TempVal=*((U32 *)DestBlock);
         *((U32 *)DestBlock)=(u4TempVal&(VP6_V_4MASK))|u4Value;
         
         u4RefVal=*((U32 *)ReconRefPtr+1);
         i4Value=((u4SrcVal>>16)&0xFF)+(S16)(u4RefVal&0xFFFF);
         u4Value=LIMIT(i4Value);
         i4Value=(u4SrcVal>>24)+(S16)((u4RefVal>>16)&0xFFFF);
         u4Value+=LIMIT(i4Value)<<16;
         u4TempVal=*((U32 *)DestBlock+1);
         *((U32 *)DestBlock+1)=(u4TempVal&(VP6_V_4MASK))|u4Value;

         u4SrcVal=*((U32 *)SrcBlock+1);
         u4RefVal=*((U32 *)ReconRefPtr+2);
         i4Value=(u4SrcVal&0xFF)+(S16)(u4RefVal&0xFFFF);
         u4Value=LIMIT(i4Value);
         i4Value=((u4SrcVal>>8)&0xff)+(S16)((u4RefVal>>16)&0xFFFF);
         u4Value+=LIMIT(i4Value)<<16;
         u4TempVal=*((U32 *)DestBlock+2);
         *((U32 *)DestBlock+2)=(u4TempVal&(VP6_V_4MASK))|u4Value;

         u4RefVal=*((U32 *)ReconRefPtr+3);
         i4Value=((u4SrcVal>>16)&0xFF)+(S16)(u4RefVal&0xFFFF);
         u4Value=LIMIT(i4Value);
         i4Value=(u4SrcVal>>24)+(S16)((u4RefVal>>16)&0xFFFF);
         u4Value+=LIMIT(i4Value)<<16;
         u4TempVal=*((U32 *)DestBlock+3);
         *((U32 *)DestBlock+3)=(u4TempVal&(VP6_V_4MASK))|u4Value;
         SrcBlock    += BLOCK_HEIGHT_WIDTH;
         ReconRefPtr += BLOCK_HEIGHT_WIDTH;
         DestBlock += u4SwapLen;
       }while(--i);
     }
     else // v component
     {
         U32 u4TempVal;
         do
         {
             u4SrcVal=*((U32 *)SrcBlock);
             u4RefVal=*((U32 *)ReconRefPtr);
             i4Value=(u4SrcVal&0xFF)+(S16)(u4RefVal&0xFFFF);
             u4Value=LIMIT(i4Value)<<8;
             i4Value=((u4SrcVal>>8)&0xFF)+(S16)((u4RefVal>>16)&0xFFFF);
             u4Value+=LIMIT(i4Value)<<24;
             u4TempVal=*((U32 *)DestBlock);
             *((U32 *)DestBlock)=(u4TempVal&(VP6_U_4MASK))|u4Value;
             
             u4RefVal=*((U32 *)ReconRefPtr+1);
             i4Value=((u4SrcVal>>16)&0xFF)+(S16)(u4RefVal&0xFFFF);
             u4Value=LIMIT(i4Value)<<8;
             i4Value=((u4SrcVal>>24)&0xFF)+(S16)((u4RefVal>>16)&0xFFFF);
             u4Value+=LIMIT(i4Value)<<24;
             u4TempVal=*((U32 *)DestBlock+1);
             *((U32 *)DestBlock+1)=(u4TempVal&(VP6_U_4MASK))|u4Value;
             
             u4SrcVal=*((U32 *)SrcBlock+1);
             u4RefVal=*((U32 *)ReconRefPtr+2);
             i4Value=(u4SrcVal&0xff)+(S16)(u4RefVal&0xFFFF);
             u4Value=LIMIT(i4Value)<<8;
             i4Value=((u4SrcVal>>8)&0xff)+(S16)((u4RefVal>>16)&0xFFFF);
             u4Value+=LIMIT(i4Value)<<24;
             u4TempVal=*((U32 *)DestBlock+2);
             *((U32 *)DestBlock+2)=(u4TempVal&(VP6_U_4MASK))|u4Value;
             
             u4RefVal=*((U32 *)ReconRefPtr+3);
             i4Value=((u4SrcVal>>16)&0xFF)+(S16)(u4RefVal&0xFFFF);
             u4Value=LIMIT(i4Value)<<8;
             i4Value=(u4SrcVal>>24)+(S16)((u4RefVal>>16)&0xFFFF);
             u4Value+=LIMIT(i4Value)<<24;
             u4TempVal=*((U32 *)DestBlock+3);
             *((U32 *)DestBlock+3)=(u4TempVal&(VP6_U_4MASK))|u4Value;
             SrcBlock    += BLOCK_HEIGHT_WIDTH;
             ReconRefPtr += BLOCK_HEIGHT_WIDTH;
             DestBlock += u4SwapLen;
         }while(--i);
     }
  }//while(--i);
#else
    U32 i = BLOCK_HEIGHT_WIDTH;
    do
    {
        U32* SrcBlockWord = (U32 *) SrcBlock;
        U32* ReconRefPtrWord = (U32 *) ReconRefPtr;
        U32 s1,s2,r1,r2;
        s1 = *SrcBlockWord++;
        s2 = *SrcBlockWord++;
        r1 = *ReconRefPtrWord++;
        r2 = *ReconRefPtrWord++;
        U32 output1 = LIMIT(((S16)(s1&0xFF))+((S16)(r1&0xFFFF)));
        output1 += LIMIT(((S16)(s1 >> 8&0xFF))+((S16)(r1>>16&0xFFFF)))<<8;
        output1 += LIMIT(((S16)(s1 >> 16&0xFF))+((S16)(r2&0xFFFF)))<<16;
        output1 += LIMIT(((S16)(s1 >> 24&0xFF))+((S16)(r2>>16&0xFFFF)))<<24;

        //s1 = *SrcBlockWord++;
        //s2 = *SrcBlockWord++;
        r1 = *ReconRefPtrWord++;
        r2 = *ReconRefPtrWord++;
        U32 output2 = LIMIT(((S16)(s2&0xFF))+((S16)(r1&0xFFFF)));
        output2 += LIMIT(((S16)(s2 >> 8&0xFF))+((S16)(r1>>16&0xFFFF)))<<8;
        output2 += LIMIT(((S16)(s2 >> 16&0xFF))+((S16)(r2&0xFFFF)))<<16;
        output2 += LIMIT(((S16)(s2 >> 24&0xFF))+((S16)(r2>>16&0xFFFF)))<<24;

        *(U32 *)(DestBlock) = output1;
        *(U32 *)(DestBlock+4) = output2;

        // Next row...
        SrcBlock    += BLOCK_HEIGHT_WIDTH;
        ReconRefPtr += BLOCK_HEIGHT_WIDTH;
        DestBlock += LineStep;
    } while((--i)>0);
#endif
}


//******************************block filter *******************************//

static U32 Var16Point ( U8 *DataPtr, S32 SourceStride )
{
    U32  i;
    U32  XSum=0;
    U32  XXSum=0;
    U8   *DiffPtr;

    // Loop expanded out for speed.
    DiffPtr = DataPtr;

    for ( i=0; i<BLOCK_HEIGHT_WIDTH; i+=2 )
    {
        // Examine alternate pixel locations.
        XSum += DiffPtr[0];
        XXSum += DiffPtr[0] * DiffPtr[0];
        XSum += DiffPtr[2];
        XXSum += DiffPtr[2] * DiffPtr[2];
        XSum += DiffPtr[4];
        XXSum += DiffPtr[4] * DiffPtr[4];
        XSum += DiffPtr[6];
        XXSum += DiffPtr[6] * DiffPtr[6];

        // Step to next row of block.
        DiffPtr += (SourceStride << 1);
    }

    // Compute population variance as mis-match metric.
    return (( (XXSum<<4) - XSum*XSum ) ) >> 8;
}


static void FilterBlock1d_8(U8 *SrcPtr,U8 *OutputPtr,U32 SrcPixelsPerLine,
    U32 PixelStep,U32 OutputHeight,
    U32 OutputWidth,const S32 *Filter)
{
    U32 increment = SrcPixelsPerLine-OutputWidth;
    U32 f1 = Filter[0];
    U32 f2 = Filter[1];
    U32 f3 = Filter[2];
    U32 f4 = Filter[3];
    U32 innerLoop = 0;
    U8 a,b,c,d;
block:
    innerLoop=0;
    
row:
    a = (U8)SrcPtr[-(S32)PixelStep];
    b = (U8)SrcPtr[0];
    S32 acc = (a * f1);
    c = (U8)SrcPtr[(S32)PixelStep];
    acc += (b * f2);
    d = (U8)SrcPtr[(S32)2*PixelStep];
    acc += (c * f3);
    acc += (d * f4);
    acc += (FILTER_WEIGHT >> 1);

    acc = acc>>7;
    if (acc < 0)
        acc = 0;
    if (acc > 0xFF)
        acc = 0xFF;
    OutputPtr[innerLoop] = (U8) acc;
    innerLoop ++;
    SrcPtr++;
    if (innerLoop < OutputWidth)
        goto row;
    SrcPtr += (increment);
    OutputPtr += OutputWidth;
    OutputHeight--;
    if (OutputHeight > 0)
        goto block;
}

static void FilterBlock2dFirstPass
(
    U8 *SrcPtr,
    S32 *OutputPtr,
    U32 SrcPixelsPerLine,
    U32 PixelStep,
    U32 OutputHeight,
    U32 OutputWidth,
    const S32 *Filter
)
{
    U32 increment = SrcPixelsPerLine-OutputWidth;
    U32 f1 = Filter[0];
    U32 f2 = Filter[1];
    U32 f3 = Filter[2];
    U32 f4 = Filter[3];
    U32 innerLoop = 0;
    U8 a,b,c,d;
block:
    innerLoop=0;
    
row:
    a = (U8)SrcPtr[-(S32)PixelStep];
    b = (U8)SrcPtr[0];
    S32 acc = (a * f1);
    c = (U8)SrcPtr[(S32)PixelStep];
    acc += (b * f2);
    d = (U8)SrcPtr[(S32)2*PixelStep];
    acc += (c * f3);
    acc += (d * f4);
    acc += (FILTER_WEIGHT >> 1);

    acc = acc>>7;
    if (acc < 0)
        acc = 0;
    if (acc > 0xFF)
        acc = 0xFF;
    OutputPtr[innerLoop] = (S32) acc;
    innerLoop ++;
    SrcPtr++;
    if (innerLoop < OutputWidth)
        goto row;
    SrcPtr += (increment);
    OutputPtr += OutputWidth;
    OutputHeight--;
    if (OutputHeight > 0)
        goto block;

}

static void FilterBlock2dSecondPass_8
(
    S32 *SrcPtr,
    U8 *OutputPtr,
    U32 SrcPixelsPerLine,
    U32 PixelStep,
    U32 OutputHeight,
    U32 OutputWidth,
    const S32 *Filter
)
{
    U32 increment = SrcPixelsPerLine-OutputWidth;
    U32 f1 = Filter[0];
    U32 f2 = Filter[1];
    U32 f3 = Filter[2];
    U32 f4 = Filter[3];
    U32 innerLoop = 0;
    S32 a,b,c,d;
block:
    innerLoop = 0;
    
row:
    a = (S32)SrcPtr[-(S32)PixelStep];
    b = (S32)SrcPtr[0];
    S32 acc = (a * f1);
    c = (S32)SrcPtr[(S32)PixelStep];
    acc += (b * f2);
    d = (S32)SrcPtr[(S32)2*PixelStep];
    acc += (c * f3);
    acc += (d * f4);
    acc += (FILTER_WEIGHT >> 1);

    acc = acc>>7;
    if (acc < 0)
        acc = 0;
    if (acc > 0xFF)
        acc = 0xFF;
    OutputPtr[innerLoop] = (U8) acc;
    innerLoop ++;
    SrcPtr++;
    if (innerLoop < OutputWidth)
        goto row;
    SrcPtr += (increment);
    OutputPtr += OutputWidth;
    OutputHeight--;
    if (OutputHeight > 0)
        goto block;
}


static void FilterBlock2d_8
(
    U8  *SrcPtr,
    U8 *OutputPtr,
    U32 SrcPixelsPerLine,
    const S32  *HFilter,
    const S32  *VFilter,
    S32* fData
)
{
    // First filter 1-D horizontally...

    FilterBlock2dFirstPass( SrcPtr-SrcPixelsPerLine, fData, SrcPixelsPerLine, 1, 11, 8, HFilter );
    // then filter verticaly...
    FilterBlock2dSecondPass_8 ( fData+BLOCK_HEIGHT_WIDTH, OutputPtr, BLOCK_HEIGHT_WIDTH, BLOCK_HEIGHT_WIDTH, 8, 8, VFilter );
}

static void FilterBlock1dBil_8
(
    U8    *SrcPtr,
    U8    *OutputPtr,
    U32  SrcPixelsPerLine,
    U32  PixelStep,
    U32  OutputHeight,
    U32  OutputWidth,
    const S32  *Filter
)
{
    U32 increment = SrcPixelsPerLine-OutputWidth;
    U32 f1 = Filter[0];
    U32 f2 = Filter[1];
    U32 innerLoop = 0;
    U8 a,b;
block:
    innerLoop=0;
    
row:
    a = (U8)SrcPtr[0];
    b = (U8)SrcPtr[(S32)PixelStep];
    S32 acc = (a * f1);
    acc += (b * f2);
    acc += (FILTER_WEIGHT >> 1);

    acc = acc>>7;
    OutputPtr[innerLoop] = (U8) acc;
    innerLoop ++;
    SrcPtr++;
    if (innerLoop < OutputWidth)
        goto row;
    SrcPtr += (increment);
    OutputPtr += OutputWidth;
    OutputHeight--;
    if (OutputHeight > 0)
        goto block;
}

static void FilterBlock2dBil_FirstPass
(
    U8 *SrcPtr,
    S32 *OutputPtr,
    U32 SrcPixelsPerLine,
    U32 PixelStep,
    U32 OutputHeight,
    U32 OutputWidth,
    const S32 *Filter
)
{
    U32 increment = SrcPixelsPerLine-OutputWidth;
    U32 f1 = Filter[0];
    U32 f2 = Filter[1];
    U32 innerLoop = 0;
    U8 a,b;
block:
    innerLoop=0;
row:
    a = (U8)SrcPtr[0];
    b = (U8)SrcPtr[(S32)PixelStep];
    S32 acc = (a * f1);
    acc += (b * f2);
    acc += (FILTER_WEIGHT >> 1);

    acc = acc>>7;
    OutputPtr[innerLoop] = (S32) acc;
    innerLoop ++;
    SrcPtr++;
    if (innerLoop < OutputWidth)
        goto row;
    SrcPtr += (increment);
    OutputPtr += OutputWidth;
    OutputHeight--;
    if (OutputHeight > 0)
        goto block;
}

static void FilterBlock2dBil_SecondPass_8
(
    S32 *SrcPtr,
    U8  *OutputPtr,
    U32 SrcPixelsPerLine,
    U32 PixelStep,
    U32 OutputHeight,
    U32 OutputWidth,
    const S32 *Filter
)
{
    U32 increment = SrcPixelsPerLine-OutputWidth;
    U32 f1 = Filter[0];
    U32 f2 = Filter[1];
    U32 innerLoop = 0;
    S32 a,b;
block:
    innerLoop=0;
    
row:
    a = (S32)SrcPtr[0];
    b = (S32)SrcPtr[(S32)PixelStep];
    S32 acc = (a * f1);
    acc += (b * f2);
    acc += (FILTER_WEIGHT >> 1);

    acc = acc>>7;
    OutputPtr[innerLoop] = (U8) acc;
    innerLoop ++;
    SrcPtr++;
    if (innerLoop < OutputWidth)
        goto row;
    SrcPtr += (increment);
    OutputPtr += OutputWidth;
    OutputHeight--;
    if (OutputHeight > 0)
        goto block;
}

static void FilterBlock2dBil_8
(
    U8  *SrcPtr,
    U8 *OutputPtr,
    U32 SrcPixelsPerLine,
    const S32  *HFilter,
    const S32  *VFilter,
    S32* fData
)
{
     // First filter 1-D horizontally...
    FilterBlock2dBil_FirstPass ( SrcPtr, fData, SrcPixelsPerLine, 1, 9, 8, HFilter );
    // then 1-D vertically...
    FilterBlock2dBil_SecondPass_8 ( fData, OutputPtr, BLOCK_HEIGHT_WIDTH, BLOCK_HEIGHT_WIDTH, 8, 8, VFilter );
}

static void FilterBlock_C
(
    U8 *ReconPtr1,
    U8 *ReconPtr2,
    U8 *ReconRefPtr,
    U32 PixelsPerLine,
    S32 ModX,
    S32 ModY,
    BOOL UseBicubic,
    U8 BicubicAlpha,
    CVP6Lib* vp6Lib
)
{
    int diff;

    // ModX and ModY are the bottom three bits of the signed motion vector
    // components (in 1/8th pel units). This works out to be what we want
    // --despite the pointer swapping that goes on below.
    // For example...
    // if MV x-component is +ve then ModX = x%8.
    // if MV x-component is -ve then ModX = 8+(x%8), where X%8 is in the range -7 to -1.

    // Swap pointers to ensure that ReconPtr1 is "smaller than",
    // i.e. above, left, above-right or above-left, ReconPtr1
    diff = (int) (ReconPtr2 - ReconPtr1);

    if ( diff<0 )
    {
        // ReconPtr1>ReconPtr2, so swap...
        U8 *temp = ReconPtr1;
        ReconPtr1 = ReconPtr2;
        ReconPtr2 = temp;
        diff = (int)(ReconPtr2-ReconPtr1);
    }


    if ( diff==1 )
    {
        // Fractional pixel in horizontal only...
        if ( UseBicubic )
            FilterBlock1d_8 ( ReconPtr1, ReconRefPtr, PixelsPerLine, 1, 8, 8, BicubicFilterSet[BicubicAlpha][ModX] );
        else
            FilterBlock1dBil_8 ( ReconPtr1, ReconRefPtr, PixelsPerLine, 1, 8, 8, BilinearFilters[ModX] );

    }
    else if ( diff == (int)(PixelsPerLine) )
    {
        // Fractional pixel in vertical only...
        if ( UseBicubic )
            FilterBlock1d_8 ( ReconPtr1, ReconRefPtr, PixelsPerLine, PixelsPerLine, 8, 8, BicubicFilterSet[BicubicAlpha][ModY] );
        else
            FilterBlock1dBil_8 ( ReconPtr1, ReconRefPtr, PixelsPerLine, PixelsPerLine, 8, 8, BilinearFilters[ModY] );

    }
    else if(diff == (int)(PixelsPerLine - 1))
    {
        // ReconPtr1 is Top right...
        if ( UseBicubic )
            FilterBlock2d_8 ( ReconPtr1-1, ReconRefPtr, PixelsPerLine, BicubicFilterSet[BicubicAlpha][ModX], BicubicFilterSet[BicubicAlpha][ModY], vp6Lib->fData );
        else
            FilterBlock2dBil_8 ( ReconPtr1-1, ReconRefPtr, PixelsPerLine, BilinearFilters[ModX], BilinearFilters[ModY], vp6Lib->fData );
    }
    else if(diff == (int)(PixelsPerLine + 1) )
    {
        // ReconPtr1 is Top left...
        if ( UseBicubic )
            FilterBlock2d_8 ( ReconPtr1, ReconRefPtr, PixelsPerLine, BicubicFilterSet[BicubicAlpha][ModX], BicubicFilterSet[BicubicAlpha][ModY], vp6Lib->fData );
        else
            FilterBlock2dBil_8 ( ReconPtr1, ReconRefPtr, PixelsPerLine, BilinearFilters[ModX], BilinearFilters[ModY], vp6Lib->fData );
    }
    
}


static void VP6_FilteringHoriz_12_C ( U32 QValue, U8 *Src, S32 Pitch, U8 *LimitVal_VP6 )
{    
    S32  FiltVal;
    U32 FLimit;
    U8 *LimitTable = &LimitVal_VP6[VAL_RANGE];
    S32 lTemp;
    S32 Clamp;


    FLimit = VP6_LoopFilterLimitValues[QValue];
    Clamp = VP6_LoopFilterBoundTable[FLimit][0];

    S32  j= 12; //4 + (8 << MbMode);
    do
    {            
        // Apply 4-tap filter with rounding...
          U8 b,c,d;
          FiltVal=Src[-2]+4;
          b=Src[-1];
          c=Src[0];
        FiltVal-=b*3;
          d=Src[1];
        FiltVal+=c*3;
        FiltVal-=d;
        FiltVal=FiltVal>>3;

        lTemp = FiltVal;
        if(FiltVal < 0)
            lTemp = 0 - FiltVal;

        if(lTemp < Clamp)        
        {
            FiltVal = VP6_LoopFilterBoundTable[FLimit][FiltVal + Clamp];
        }
        else
        {
            FiltVal = 0;
        }

        Src[-1] = LimitTable[(S32)b + FiltVal];
        Src[ 0] = LimitTable[(S32)c - FiltVal];
        
        Src += Pitch;
    } while((--j) > 0);
}


static void VP6_FilteringVert_12_C ( U32 QValue, U8 *Src, S32 Pitch, U8 *LimitVal_VP6 )
{ 
     S32  FiltVal;
     U32 FLimit;
     S32 lTemp;
     S32 Clamp;

    U8 *LimitTable = &LimitVal_VP6[VAL_RANGE];

    FLimit = VP6_LoopFilterLimitValues[QValue];
    Clamp = VP6_LoopFilterBoundTable[FLimit][0];

    S32  j=12;
    do
    {            
        // Apply 4-tap filter with rounding...
        U32 a,b,c,d;
        b=Src[-Pitch];
        c=Src[0];
        FiltVal=0-(b*3);
        a=Src[-(2 * Pitch)];
        FiltVal+=c*3;
        d=Src[Pitch];
        FiltVal+=a;
        FiltVal-=d;
        FiltVal+=4;
        FiltVal=FiltVal>>3;

        lTemp = FiltVal;
        if(FiltVal < 0)
            lTemp = 0 - FiltVal;

        if(lTemp < Clamp)        
        {
            FiltVal = VP6_LoopFilterBoundTable[FLimit][FiltVal + Clamp];
        }
        else
        {
            FiltVal = 0;
        }

        Src[-Pitch] = LimitTable[(S32)b + FiltVal];
        Src[ 0] = LimitTable[(S32)c - FiltVal];
        
        Src ++;
    } while((--j) > 0);
}

static void UnpackBlock_C_8 ( U8 *ReconPtr, U8 *ReconRefPtr, U32 ReconPixelsPerLine )
{
    int i=0;
    for(; i < BLOCK_HEIGHT_WIDTH; i++ ) 
    {
        ReconRefPtr[0] = ReconPtr[0];
        ReconRefPtr[1] = ReconPtr[1];
        ReconRefPtr[2] = ReconPtr[2];
        ReconRefPtr[3] = ReconPtr[3];
        ReconRefPtr[4] = ReconPtr[4];
        ReconRefPtr[5] = ReconPtr[5];
        ReconRefPtr[6] = ReconPtr[6];
        ReconRefPtr[7] = ReconPtr[7];

        // Start next row
        ReconPtr    += ReconPixelsPerLine;
        ReconRefPtr += BLOCK_HEIGHT_WIDTH;
    }
}

static void Copy12x12_C_OPT
(
    const unsigned char *src, 
    unsigned char *dest, 
    unsigned int srcstride
)
{
#ifdef VP6_COMBUV_IN_DECODING
  U32 u4SwapLen=srcstride&0xffff,u4CompType=srcstride>>16;
  S32 i=12;
  if(!u4CompType)//Y
  {
     do
     {
         FlashMemCpy((void *)dest, (void *)src, sizeof(U32)*3);
         src += u4SwapLen;
         dest += 12;
     }while(--i);
  }
  else
  {
     U32 u4DstIndex=0;
     do
     {
        const unsigned char *pSrc=src+(u4CompType-1);
        dest[u4DstIndex++]=*pSrc;
        pSrc+=2;
        dest[u4DstIndex++]=*pSrc;
        pSrc+=2;
        dest[u4DstIndex++]=*pSrc;
        pSrc+=2;
        dest[u4DstIndex++]=*pSrc;
        pSrc+=2;

        dest[u4DstIndex++]=*pSrc;
        pSrc+=2;
        dest[u4DstIndex++]=*pSrc;
        pSrc+=2;
        dest[u4DstIndex++]=*pSrc;
        pSrc+=2;
        dest[u4DstIndex++]=*pSrc;
        pSrc+=2;

        dest[u4DstIndex++]=*pSrc;
        pSrc+=2;
        dest[u4DstIndex++]=*pSrc;
        pSrc+=2;
        dest[u4DstIndex++]=*pSrc;
        pSrc+=2;
        dest[u4DstIndex++]=*pSrc;
        pSrc+=2;

        src += u4SwapLen;
     }while(--i);
  } 
#else

    int i = 12;
    
    do 
    {
        FlashMemCpy((void *)dest, (void *)src, sizeof(U32)*3);
        src += srcstride;
        dest += 12;
    } while((--i) > 0);
#endif

}


static inline void VP6_PredictFiltered
(
 PB_INSTANCE *pbi,
 U8 *SrcPtr,
 S32 mx,
 S32 my,
 U32 bp,
 U8 *LimitVal_VP6
 )
{
    MACROBLOCK_INFO *mbi=&pbi->mbi;
    U8 *TempBuffer = pbi->LoopFilteredBlock;    

    Copy12x12_C_OPT( SrcPtr, TempBuffer, mbi->blockDxInfo[bp].CurrentReconStride);

    // apply the loop filter at the horizontal boundary we selected
    if(mx)
        VP6_FilteringHoriz_12_C(
        pbi->quantizer->FrameQIndex, 
        TempBuffer + 2 + mx, 
        12,
        LimitVal_VP6); 

    // apply the loop filter at the vertical boundary we selected
    if (my)
        VP6_FilteringVert_12_C(
        pbi->quantizer->FrameQIndex, 
        TempBuffer + ((2 + my) * 12), 
        12,
        LimitVal_VP6);

}

static void VP6_PredictFilteredBlock 
(
    PB_INSTANCE *pbi,
    U8 *OutputPtr,
    U32 bp,
    CVP6Lib* vp6Lib
) 
{

    U8 *SrcPtr;
    U8 *TempBuffer;
#if 0
    S32 TempPtr1;
    S32 TempPtr2;
#else
    U32 TempPtr1;
    U32 TempPtr2;
#endif
    S32  ModX, ModY;
    U32 IVar;
    U32 BicMvSizeLimit;
    U32 Stride;    
    S32  mVx, mVy;
    S32  BoundaryX, BoundaryY; 
    
    S32  mx = pbi->mbi.blockDxInfo[bp].Mv.x;
    S32  my = pbi->mbi.blockDxInfo[bp].Mv.y;

    U32 MvShift = pbi->mbi.blockDxInfo[bp].MvShift; //pbi->mbi.MvShift;
    U32 MvModMask = pbi->mbi.blockDxInfo[bp].MvModMask; //pbi->mbi.MvModMask;    
#ifdef VP6_COMBUV_IN_DECODING
    U32 u4Component=0,u4Factor=0;
    u4Component=pbi->mbi.blockDxInfo[bp].CurrentReconStride>>16;
    mVx = mx + (MvModMask&(mx>>31));
    mVy = my + (MvModMask&(my>>31));
    
    mVx = (mVx >> MvShift);
    mVy = (mVy >> MvShift); 
    
    // calculate block border position for x
    BoundaryX = (8 - Mod8(mVx))&7;
    
    // calculate block border position for y
    BoundaryY = (8 - Mod8(mVy))&7;
    
    // Mask off fractional pel bits.
    ModX = (mx & MvModMask);
    ModY = (my & MvModMask); 

    if(u4Component)
    {
        mVx*=2;
        //BoundaryX*=2;
        //ModX*=2;
        u4Factor=1;
    }
#else

// Calculate full pixel motion vector position 
    mVx = mx + (MvModMask&(mx>>31));
    mVy    = my + (MvModMask&(my>>31));

    mVx = (mVx >> MvShift);
    mVy = (mVy >> MvShift);    

    // calculate block border position for x
    BoundaryX = (8 - Mod8(mVx))&7;

    // calculate block border position for y
    BoundaryY = (8 - Mod8(mVy))&7;

    // Mask off fractional pel bits.
    ModX = (mx & MvModMask);
    ModY = (my & MvModMask); 
#endif
    // Which buffer are we working on?
    SrcPtr = pbi->LastFrameRecon;
    if ( VP6_Mode2Frame[pbi->mbi.Mode] == 2 ) 
    {
        SrcPtr = pbi->GoldenFrame;
    }

    // No loop filtering in simple profile
    if ( pbi->VpProfile == SIMPLE_PROFILE || (pbi->UseLoopFilter == NO_LOOP_FILTER) || (BoundaryX == 0 && BoundaryY == 0))
    {    

#ifdef VP6_COMBUV_IN_DECODING
       if(u4Component)
       {
           SrcPtr = SrcPtr + pbi->mbi.blockDxInfo[bp].thisRecon + (pbi->mbi.blockDxInfo[bp].FrameReconStride * mVy + mVx);
           Stride = pbi->mbi.blockDxInfo[bp].CurrentReconStride&0xffff;
           SrcPtr -= (Stride << 1);
           SrcPtr-= (2<<u4Factor);
           TempBuffer=pbi->LoopFilteredBlock;
           Copy12x12_C_OPT( SrcPtr, TempBuffer, pbi->mbi.blockDxInfo[bp].CurrentReconStride);
           //6_PredictFiltered( pbi, SrcPtr, BoundaryX, BoundaryY, bp, vp6Lib->LimitVal_VP6);
           Stride=12;
           TempPtr1 = TempPtr2 = 0;
           TempPtr1 = 2*12+2;      
           TempPtr2 = TempPtr1;
       }
       else
       {
           TempBuffer = SrcPtr + pbi->mbi.blockDxInfo[bp].thisRecon + (pbi->mbi.blockDxInfo[bp].FrameReconStride * mVy + mVx);
           Stride = pbi->mbi.blockDxInfo[bp].CurrentReconStride&0xffff;
           TempPtr1 = TempPtr2 = 0;
       }
#else
        // Set up a pointer into the recon buffer
       TempBuffer = SrcPtr + pbi->mbi.blockDxInfo[bp].thisRecon + (pbi->mbi.blockDxInfo[bp].FrameReconStride * mVy + mVx);
       Stride = pbi->mbi.blockDxInfo[bp].CurrentReconStride&0xffff;
       TempPtr1 = TempPtr2 = 0;
#endif
    }
    else // Loop filter the block
    {    

        // calculate offset in last frame matching motion vector
        SrcPtr = SrcPtr + pbi->mbi.blockDxInfo[bp].thisRecon + (pbi->mbi.blockDxInfo[bp].FrameReconStride * mVy + mVx);
        Stride = pbi->mbi.blockDxInfo[bp].CurrentReconStride&0xffff;
        SrcPtr -= (Stride << 1);

        // Give our selves a border of 2 extra pixel on all sides (for loop filter and half pixel moves)
#ifdef VP6_COMBUV_IN_DECODING
       SrcPtr-= (2<<u4Factor);
#else
       SrcPtr -= 2;
#endif

        VP6_PredictFiltered( pbi, SrcPtr, BoundaryX, BoundaryY, bp, vp6Lib->LimitVal_VP6);
        TempBuffer = pbi->LoopFilteredBlock;
        
        Stride = 12;
        TempPtr1 = 2*12+2;        

        TempPtr2 = TempPtr1;
    }

    // determine if we have a fractional pixel move in the x direction
    if ( ModX )
    {
        TempPtr2 += ( mx > 0 )*2 -1;        
    }

    // handle fractional pixel motion in Y
    if ( ModY )
    {
        TempPtr2 += (( my > 0 ) * 2 - 1)* Stride;
    }

    // put the results back into the real reconstruction buffer
    if ( (TempPtr1 != TempPtr2) ) 
    {
        // The FilterBlock selects a filter based upon a ModX and ModY value that are at 1/8 point 
        // precision. Because U and V are subsampled the vector is already at the right precision 
        // for U and V but for Y we have to multiply by 2.
        if ( bp < 4 )
        {
            // Filterblock expects input at 1/8 pel resolution (hence << 1 for Y)
            ModX = ModX << 1;
            ModY = ModY << 1; 

            // Select the filtering mode
            if( pbi->VpProfile == SIMPLE_PROFILE )
            {
                // Simple profile always uses bilinear filtering for speed
                FilterBlock_C( &TempBuffer[TempPtr1], &TempBuffer[TempPtr2], /*(unsigned short *)*/OutputPtr, Stride, ModX, ModY, FALSE, 14 , vp6Lib);
            }
            else if ( pbi->PredictionFilterMode == AUTO_SELECT_PM )
            {
                //  Work out the Mv size limit for selecting bicubic
                if ( pbi->PredictionFilterMvSizeThresh > 0 )
                    BicMvSizeLimit = (1 << (pbi->PredictionFilterMvSizeThresh - 1)) << 2;             // Convert to a value in 1/4 pel units
                else
                    BicMvSizeLimit = ((MAX_MV_EXTENT >> 1) + 1) << 2;                                 // Unrestricted

                // Only use bicubic on shortish vectors
                if ( ( pbi->PredictionFilterMvSizeThresh != 0 ) &&
                    ( ( (U32)abs(mx) > BicMvSizeLimit ) || 
                    ( (U32)abs(my) > BicMvSizeLimit ) ) )
                {
                    FilterBlock_C( &TempBuffer[TempPtr1], &TempBuffer[TempPtr2], /*(unsigned short *)*/ OutputPtr, Stride, ModX, ModY, FALSE, pbi->PredictionFilterAlpha, vp6Lib);
                }
                // Should we use a variance test for bicubic as well
                else if ( pbi->PredictionFilterVarThresh != 0 )
                {
                    IVar = Var16Point( &TempBuffer[TempPtr1], Stride);
                    FilterBlock_C( &TempBuffer[TempPtr1], &TempBuffer[TempPtr2], /*(unsigned short *)*/ OutputPtr, Stride, ModX, ModY, (IVar >= pbi->PredictionFilterVarThresh), pbi->PredictionFilterAlpha, vp6Lib);
                }
                else
                {
                    FilterBlock_C( &TempBuffer[TempPtr1], &TempBuffer[TempPtr2], /*(unsigned short *)*/ OutputPtr, Stride, ModX, ModY, TRUE, pbi->PredictionFilterAlpha, vp6Lib);
                }
            }
            else  
                FilterBlock_C( &TempBuffer[TempPtr1], &TempBuffer[TempPtr2], /*(unsigned short *)*/ OutputPtr, Stride, ModX, ModY, (pbi->PredictionFilterMode == BICUBIC_ONLY_PM), pbi->PredictionFilterAlpha , vp6Lib);
        }
        else
        {
            FilterBlock_C( &TempBuffer[TempPtr1], &TempBuffer[TempPtr2], /*(unsigned short *)*/ OutputPtr, Stride, ModX, ModY, FALSE, pbi->PredictionFilterAlpha , vp6Lib);
        }
    }    
    else // No fractional pels
    {
        UnpackBlock_C_8(&TempBuffer[TempPtr1], OutputPtr, Stride);
    }
}

//************************************Decode Context*******************************//

static void VP6_ResetLeftContext ( PB_INSTANCE *pbi)
{
///////////////////////////////////////////////////////////////
// Optimisation #6
///////////////////////////////////////////////////////////////

    FlashMemSet((void *) &pbi->fc.LeftY[0], 0, 4*sizeof(BLOCK_CONTEXT));
    
    pbi->fc.LeftY[0].Mode = (CODING_MODE)-1;
    pbi->fc.LeftY[1].Mode = (CODING_MODE)-1;
    pbi->fc.LeftU.Mode    = (CODING_MODE)-1;
    pbi->fc.LeftV.Mode    = (CODING_MODE)-1;
        
    pbi->fc.LeftY[0].Frame = 4;
    pbi->fc.LeftY[1].Frame = 4;
    pbi->fc.LeftU.Frame    = 4;
    pbi->fc.LeftV.Frame    = 4;
}


static void VP6_ResetAboveContext ( PB_INSTANCE *pbi )
{
    U32 i;
    BLOCK_CONTEXT* pDestY=&pbi->fc.AboveY[0];
    i=pbi->HFragments+8;
    do
        {
        pDestY->Mode=(CODING_MODE)-1;
        pDestY->Frame=4;
        pDestY->Dc=0;
        pDestY->Token=0;
        pDestY++;
        }
    while (--i);
    i=(pbi->HFragments>>1)+8;
    BLOCK_CONTEXT* pDestU=&pbi->fc.AboveU[0];
    BLOCK_CONTEXT* pDestV=&pbi->fc.AboveV[0];
    do
        {        
        pDestU->Mode=(CODING_MODE)-1;
        pDestU->Frame=4;
        pDestU->Dc=0;
        pDestU->Token=0;
        pDestU++;
        pDestV->Mode=(CODING_MODE)-1;
        pDestV->Frame=4;  
        pDestV->Dc=0;
        pDestV->Token=0;
        pDestV++;
        }
    while (--i);
    
    if(pbi->Vp3VersionNo < 6)
    {
        pbi->fc.AboveU[1].Mode = (CODING_MODE)0;
        pbi->fc.AboveU[1].Frame = 0;
        pbi->fc.AboveV[1].Mode = (CODING_MODE)0;
        pbi->fc.AboveV[1].Frame = 0;                
    }

    pbi->fc.LastDcY[0]=0;
    pbi->fc.LastDcY[1]=0;
    pbi->fc.LastDcY[2]=0;
    pbi->fc.LastDcU[0]=128;
    pbi->fc.LastDcU[1]=0;
    pbi->fc.LastDcU[2]=0;
    pbi->fc.LastDcV[0]=128;
    pbi->fc.LastDcV[1]=0;
    pbi->fc.LastDcV[2]=0;

}


static void VP6_ConfigureContexts ( PB_INSTANCE *pbi )
{
    U32 i;
    U32 Node;
    U32 Plane;
    S32  Temp;

    // Clear MMX state so floating point can work again
    //ClearSysState_C();

    // DC Node Probabilitiesg
    for ( Plane=0; Plane<2; Plane++ )
    {
        for ( i=0; i<DC_TOKEN_CONTEXTS; i++ )
        {
            // Tree Nodes
            for ( Node=0; Node<CONTEXT_NODES; Node++ )
            {
                Temp = ( ( pbi->DcProbs[DCProbOffset(Plane,Node)] * VP6_DcNodeEqs[Node][i].M + 128 ) >> 8) 
                        + VP6_DcNodeEqs[Node][i].C;    
                Temp = (Temp > 255)? 255: Temp;
                Temp = (Temp <   1)? 1  : Temp;
                
                //pbi->DcNodeContexts[Plane][i][Node] = (U8)Temp;
                *(pbi->DcNodeContexts + DcNodeOffset(Plane,i,Node)) = (U8)Temp;
            }
        }
    }
}



//************************************Decode MB****************************//

static inline int VP6_ExtractTokenN( BITREADER *br, HUFF_NODE *huffNode, U16* hlt)
{
    unsigned short torp;
    unsigned short *hn = (unsigned short *)huffNode;
    HUFF_TABLE_NODE *htptr = (HUFF_TABLE_NODE *)hlt;

    U32 x = bitreadonly(br, HUFF_LUT_LEVELS);

    bitShift(br, (htptr[x].length));
    if(htptr[x].flag)
    {
        return htptr[x].value;
    }

    // Loop searches down through tree based upon bits read from the bitstream
    // until it hits a leaf at which point we have decoded a token
    torp = htptr[x].value;
    torp = 0;
    do
    {
        torp += (U16)bitread1(br);
        torp = hn[torp];
    }
    while ( !(torp & 1));

    return torp >> 1;
}


static void VP6_PredictDC_MB(PB_INSTANCE *pbi)
{
    U8 Frame = VP6_Mode2Frame[pbi->mbi.Mode];
    Q_LIST_ENTRY *  LastDC;
    BLOCK_CONTEXT*  Above;
    BLOCK_CONTEXT *  Left;

    BLOCK_DX_INFO *bdi = pbi->mbi.blockDxInfo;
    BLOCK_DX_INFO *bdiEnd = bdi + 6;

    do
    {
        S32 Avg;
    
        LastDC = bdi->LastDc;
        Above = bdi->Above;
         Left = bdi->Left;

        Avg = LastDC[Frame];

        if(Frame == Left->Frame) 
        {
            Avg = Left->Dc;
        }
        if(Frame == Above->Frame) 
        {
            Avg = Above->Dc;
            if(Frame == Left->Frame)
            {
                Avg += Left->Dc;
                Avg += (HIGHBITDUPPED(Avg)&1);
                Avg >>= 1;

            }
        }

        bdi->coeffsPtr[0] += (short)Avg;
        LastDC[Frame] = bdi->coeffsPtr[0];
        
        Above->Dc = bdi->coeffsPtr[0];
        Above->Frame = Frame;

        Left->Dc = bdi->coeffsPtr[0];
        Left->Frame = Frame;

    } while(++bdi < bdiEnd);    
}


static void ReadHuffTokensPredictA_MB(PB_INSTANCE *pbi) //, int *EobArray)
{
    BITREADER *br = &pbi->br3;
    S32   SignBit;
    U32  Prec;

    U32   token;
    U32  blockIndex;

    U32  Plane = 0;

    S16 *CoeffData;
    MACROBLOCK_INFO *mbi = &pbi->mbi;

    U8 *MergedScanOrderPtr;

    for(blockIndex = 0; blockIndex < 6; blockIndex++)
    {
        MergedScanOrderPtr = pbi->MergedScanOrder;

        CoeffData = mbi->blockDxInfo[blockIndex].coeffsPtr;

        if(blockIndex > 3)
        {
            Plane = 1;
        }

        if ( pbi->CurrentDcRunLen[Plane] > 0 )
        {
            // DC -- run of zeros in progress
            --pbi->CurrentDcRunLen[Plane];
            Prec = 0;
        }
        else
        {
            // DC -- no current run of zeros
            token = VP6_ExtractTokenN(br, pbi->DcHuffTree[Plane], pbi->DcHuffLUT[Plane]);

            if(token == DCT_EOB_TOKEN)
                goto Finished;

            if(token == ZERO_TOKEN)
            {
                // Read zero run-length
                {
                    // Run of zeros at DC is coded as a tree
                    U32 val = 1 + bitread(br, 2);

                    if ( val == 3 )
                        val += bitread(br, 2);
                    else if ( val == 4 )
                    {
                        if ( bitread1(br) )
                            val = 11 + bitread(br, 6);
                        else
                            val = 7 + bitread(br, 2);
                    }
                    pbi->CurrentDcRunLen[Plane] = val - 1;
                }
                Prec = 0;
            }
            else
            {
                register S32 value;

                value = VP6_HuffTokenMinVal[token];

                if(token <=FOUR_TOKEN)
                {
                    SignBit = bitread1(br);
                }
                else if(token <=DCT_VAL_CATEGORY5)
                {
                    value   += bitread(br, (token-4));
                    SignBit = bitread1(br);
                }
                else
                {
                    value   += bitread(br, 11);
                    SignBit = bitread1(br);

                }
                CoeffData[0] = (Q_LIST_ENTRY)((value ^ -SignBit) + SignBit);
                Prec = (value>1)?2:1;
            }

        }
        //first AC

        MergedScanOrderPtr++;

        if ( pbi->CurrentAc1RunLen[Plane] > 0 )
        {
            // First AC in scan order -- run of EOBs in progress
            --pbi->CurrentAc1RunLen[Plane];
            goto Finished;
        }

        do
        {

            U32 Band = *(MergedScanOrderPtr + 64); //VP6_CoeffToHuffBand[EncodedCoeffs];

            token = VP6_ExtractTokenN(br, pbi->AcHuffTree[Prec][Plane][Band], pbi->AcHuffLUT[Prec][Plane][Band]);

            if(token == ZERO_TOKEN)
            {
                {
                    //U32 ZrlBand;
                    //U32 ZrlToken;
                    #define ZrlBand Band
                    #define ZrlToken token

                    // Read zero run-length
                    ZrlBand  = (MergedScanOrderPtr >= (pbi->MergedScanOrder + ZRL_BAND2));

                    ZrlToken = VP6_ExtractTokenN(br, pbi->ZeroHuffTree[ZrlBand], pbi->ZeroHuffLUT[ZrlBand]);

                    if ( ZrlToken<8 )
                        MergedScanOrderPtr += ZrlToken;             // Zero run <= 8
                    else
                        MergedScanOrderPtr += 8 + bitread(br, 6);   // Zero run > 8
                }
                Prec =0;
                MergedScanOrderPtr ++;
                continue;
            }

            if(token == DCT_EOB_TOKEN)
            {
                if ( MergedScanOrderPtr == (pbi->MergedScanOrder + 1) )
                {
                    // Read run of EOB at first AC position
                    U32 val = 1 + bitread(br, 2);

                    if ( val == 3 )
                        val += bitread(br, 2);
                    else if ( val == 4 )
                    {
                        if ( bitread1(br) )
                            val = 11 + bitread(br, 6);
                        else
                            val = 7 + bitread(br, 2);
                    }
                    pbi->CurrentAc1RunLen[Plane] = val - 1;
                }
                goto Finished;

            }

            {
                register S32 value;

                value = VP6_HuffTokenMinVal[token];

                if(token <=FOUR_TOKEN)
                {
                    SignBit = bitread1(br);
                }
                else if(token <=DCT_VAL_CATEGORY5)
                {
                    value   += bitread(br, (token-4));
                    SignBit = bitread1(br);
                }
                else
                {
                    value   += bitread(br, 11);
                    SignBit = bitread1(br);

                }

                CoeffData[*(MergedScanOrderPtr)] = (Q_LIST_ENTRY)((value ^ -SignBit) + SignBit);
                Prec = (value>1)?2:1;
                MergedScanOrderPtr ++;
            }

        } while (MergedScanOrderPtr < (pbi->MergedScanOrder + BLOCK_SIZE));

        MergedScanOrderPtr--;

    Finished:
        //EobArray[blockIndex] =  pbi->EobOffsetTable[(U32)(MergedScanOrderPtr - (pbi->MergedScanOrder))];
        mbi->blockDxInfo[blockIndex].EobPos =  pbi->EobOffsetTable[(U32)(MergedScanOrderPtr - (pbi->MergedScanOrder))];
    } //for(blockIndex = 0; blockIndex < 6; blockIndex++)
}


static void VP6_ReadTokensPredictA_MB(PB_INSTANCE *pbi) //, int *EobArray)
{
    register BOOL_CODER *br;

    S32        token;
    U8       *BaselineProbsPtr;
    U8       *ContextProbsPtr;
    U8       PrecTokenIndex;        // Preceeding token index
    S32        SignBit;

    U8       *AcProbsPtr;
    //    U32     *TransIndex = pbi->quantizer->transIndex;

    BLOCK_CONTEXT *Above;
    BLOCK_CONTEXT *Left;

    S16 *CoeffData;

    MACROBLOCK_INFO *mbi = &pbi->mbi;

    U32  blockIndex;

    U8 *MergedScanOrderPtr;

    // Select the appropriate bool code partition to use for the DCT data
    if ( pbi->MultiStream || (pbi->VpProfile == SIMPLE_PROFILE) )
        br = &pbi->br2;
    else
        br = &pbi->br;

    unsigned char* pTempBrBuffer=br->buffer;
    br->buffer+=br->pos;

    for(blockIndex = 0; blockIndex < 6; blockIndex++)
    {
        MergedScanOrderPtr = pbi->MergedScanOrder;
        CoeffData = mbi->blockDxInfo[blockIndex].coeffsPtr;

        //need array of aboves and lefts here
        Above = mbi->blockDxInfo[blockIndex].Above;
        Left = mbi->blockDxInfo[blockIndex].Left;

        if(blockIndex < 4)
        {
            AcProbsPtr = pbi->AcProbs + ACProbOffset(0,0,0,0);
            BaselineProbsPtr = pbi->DcProbs+DCProbOffset(0,0);
            //ContextProbsPtr = pbi->DcNodeContexts[0][Left->Token + Above->Token];
            ContextProbsPtr = pbi->DcNodeContexts + DcNodeOffset(0, (Left->Token + Above->Token), 0);
        }
        else
        {
            AcProbsPtr = pbi->AcProbs + ACProbOffset(1,0,0,0);
            BaselineProbsPtr = pbi->DcProbs+DCProbOffset(1,0);
            //ContextProbsPtr = pbi->DcNodeContexts[1][Left->Token + Above->Token];
            ContextProbsPtr = pbi->DcNodeContexts + DcNodeOffset(1, (Left->Token + Above->Token), 0);
        }


        // Decode the dc token -- first test to see if it is zero
        if ( !tDecodeBool(ContextProbsPtr[ZERO_CONTEXT_NODE], br ) )
        {
            // Zero is implicit for DC token
            PrecTokenIndex = 0;
            Left->Token = 0;                    // Update the above and left token contexts to indicate a zero
            Above->Token = 0;
        }
        // A non zero DC value
        else
        {

            Left->Token = 1;                    // Update the above and left token contexts to indicate non zero
            Above->Token = 1;

            // Was the value a 1
            if ( tDecodeBool(ContextProbsPtr[ONE_CONTEXT_NODE], br) )
            {
                PrecTokenIndex = 2;

                // Value token > 1
                if ( tDecodeBool(ContextProbsPtr[LOW_VAL_CONTEXT_NODE], br) )
                {
                    S32       value;

                    // High value (value category) token
                    if ( tDecodeBool(BaselineProbsPtr[HIGH_LOW_CONTEXT_NODE], br) )
                    {
                        // Cat3,Cat4 or Cat5
                        if ( tDecodeBool(BaselineProbsPtr[CAT_THREEFOUR_CONTEXT_NODE], br) )
                            token = DCT_VAL_CATEGORY5 + tDecodeBool(BaselineProbsPtr[CAT_FIVE_CONTEXT_NODE], br);
                        else
                            token = DCT_VAL_CATEGORY3 + tDecodeBool(BaselineProbsPtr[CAT_THREE_CONTEXT_NODE], br);
                    }
                    else
                    {
                        // Either Cat1 or Cat2
                        token = DCT_VAL_CATEGORY1 + tDecodeBool(BaselineProbsPtr[CAT_ONE_CONTEXT_NODE], br);
                    }

                    value = VP6_TokenExtraBits2[token].MinVal;

                    {
                        S32        BitsCount;

                        // Read the extra bits
                        BitsCount = VP6_TokenExtraBits2[token].Length;

                        do
                        {
                            value += (tDecodeBool(VP6_TokenExtraBits2[token].Probs[BitsCount], br)<<BitsCount);
                            BitsCount -- ;
                        }
                        while( BitsCount >= 0);
                    }

                    // Get the Sign Bit
                    SignBit = tDecodeBool128(br);

                    // Combine the signa and value
                    CoeffData[0] =(Q_LIST_ENTRY)((value ^ (SignBit>>1)) + ((U32)SignBit >> 31));
                }
                else
                {
                    // Low value token
                    if ( tDecodeBool(ContextProbsPtr[TWO_CONTEXT_NODE], br) )
                    {
                        // Either a 3 or a 4
                        token = THREE_TOKEN + tDecodeBool(BaselineProbsPtr[THREE_CONTEXT_NODE], br);
                    }
                    else
                    {
                        // Is it a  2
                        token = TWO_TOKEN;
                    }

                    // Get the Sign Bit and store the result in our coeff array
                    SignBit = tDecodeBool128(br);
                    CoeffData[0] =(Q_LIST_ENTRY)((token ^ (SignBit>>1)) + ((U32)SignBit >> 31));

                }
            }
            else
            {
                PrecTokenIndex = 1;

                // Get the Sign Bit
                CoeffData[0] = tDecodeBool128(br);
            }

        }
        MergedScanOrderPtr++;

        // Now loop for each AC token
        do
        {
            // calculate the context for the next token.
            BaselineProbsPtr = AcProbsPtr + ACProbOffset(0, PrecTokenIndex, *(MergedScanOrderPtr + 64), 0 );

            // Test for conditions where it is implicit that the token must be non-zero.
            if ( (MergedScanOrderPtr > (pbi->MergedScanOrder + 1)) && (PrecTokenIndex == 0) )
                token = 1;
            else
                // Explicit test for token non zero
                token = tDecodeBool(BaselineProbsPtr[ZERO_CONTEXT_NODE], br);

            // Is the token a Zero or EOB
            if (token == 0)
            {
                //U32      ZeroRunCount;
#define ZeroRunCount token

                //U8      *ZeroRunProbPtr;
#define ZeroRunProbPtr BaselineProbsPtr

                // Is it a Zero
                if ( tDecodeBool(BaselineProbsPtr[EOB_CONTEXT_NODE], br) )
                {
                    PrecTokenIndex = 0;
                }
                else                                        // EOB
                {
                    //MergedScanOrderPtr++;        //DEBUG_SHIV
                    break;                                    // Break straight out of the loop
                }

                // Select the appropriate Zero run context
                ZeroRunProbPtr = pbi->ZeroRunProbs[(MergedScanOrderPtr >= (pbi->MergedScanOrder + ZRL_BAND2)) ? 1 : 0];

                // Now decode the zero run length
                // Run lenght 1-4
                if ( !tDecodeBool(ZeroRunProbPtr[0], br ) )
                {
                    if ( !tDecodeBool(ZeroRunProbPtr[1], br ) )
                        ZeroRunCount = 1 + tDecodeBool(ZeroRunProbPtr[2], br );
                    else
                        ZeroRunCount = 3 + tDecodeBool(ZeroRunProbPtr[3], br );
                }
                // Run length 5-8
                else if ( !tDecodeBool(ZeroRunProbPtr[4], br ) )
                {
                    if ( !tDecodeBool(ZeroRunProbPtr[5] , br) )
                        ZeroRunCount = 5 + tDecodeBool(ZeroRunProbPtr[6] , br);
                    else
                        ZeroRunCount = 7 + tDecodeBool(ZeroRunProbPtr[7] , br);
                }
                // Run length > 8
                else
                {
                    ZeroRunCount = tDecodeBool(ZeroRunProbPtr[8] , br);
                    ZeroRunCount += tDecodeBool(ZeroRunProbPtr[9] , br) << 1;
                    ZeroRunCount += tDecodeBool(ZeroRunProbPtr[10] , br) << 2;
                    ZeroRunCount += tDecodeBool(ZeroRunProbPtr[11] , br) << 3;
                    ZeroRunCount += tDecodeBool(ZeroRunProbPtr[12] , br) << 4;
                    ZeroRunCount += tDecodeBool(ZeroRunProbPtr[13] , br) << 5;
                    ZeroRunCount += 9;
                }

                // Update EncodedCoeffs account for the run of zeros.
                MergedScanOrderPtr += ZeroRunCount;
            }
            // The token codes a non zero value
            else
            {
                // Was the value a 1
                if ( tDecodeBool(BaselineProbsPtr[ONE_CONTEXT_NODE], br) )
                {
                    PrecTokenIndex = 2;

                    // Value token > 1
                    if ( tDecodeBool(BaselineProbsPtr[LOW_VAL_CONTEXT_NODE], br) )
                    {
                        S32       value;

                        // High value (value category) token
                        if ( tDecodeBool(BaselineProbsPtr[HIGH_LOW_CONTEXT_NODE], br) )
                        {
                            // Cat3,Cat4 or Cat5
                            if ( tDecodeBool(BaselineProbsPtr[CAT_THREEFOUR_CONTEXT_NODE], br) )
                            {
                                token = DCT_VAL_CATEGORY5 + tDecodeBool(BaselineProbsPtr[CAT_FIVE_CONTEXT_NODE], br);
                            }
                            else
                            {
                                token = DCT_VAL_CATEGORY3 + tDecodeBool(BaselineProbsPtr[CAT_THREE_CONTEXT_NODE], br);
                            }
                        }
                        else
                        {
                            // Either Cat1 or Cat2
                            token = DCT_VAL_CATEGORY1 + tDecodeBool(BaselineProbsPtr[CAT_ONE_CONTEXT_NODE], br);
                        }

                        value = VP6_TokenExtraBits2[token].MinVal;

                        {
                            S32        BitsCount;

                            // Read the extra bits
                            BitsCount = VP6_TokenExtraBits2[token].Length;

                            do
                            {
                                value += (tDecodeBool(VP6_TokenExtraBits2[token].Probs[BitsCount], br)<<BitsCount);
                                BitsCount -- ;
                            }
                            while( BitsCount >= 0 );
                        }
                        // Get the Sign Bit
                        SignBit = tDecodeBool128(br);

                        // Combine the signa and value
                        CoeffData[*(MergedScanOrderPtr)] =(Q_LIST_ENTRY)((value ^ (SignBit>>1)) + ((U32)SignBit >> 31));
                    }
                    else
                    {
                        // Low value token
                        if ( tDecodeBool(BaselineProbsPtr[TWO_CONTEXT_NODE], br) )
                        {
                            // Either a 3 or a 4
                            token = THREE_TOKEN + tDecodeBool(BaselineProbsPtr[THREE_CONTEXT_NODE], br);
                        }
                        else
                        {
                            // Is it a  2
                            token = TWO_TOKEN;
                        }
                        // Get the Sign Bit and store the result in our coeff array
                        SignBit = tDecodeBool128(br);
                        CoeffData[*(MergedScanOrderPtr)] =(Q_LIST_ENTRY)((token ^ (SignBit>>1)) + ((U32)SignBit >> 31));
                    }
                }
                else
                {
                    PrecTokenIndex = 1;

                    // Get the Sign Bit
                    CoeffData[*(MergedScanOrderPtr)] = tDecodeBool128(br);
                }

                MergedScanOrderPtr++;
            }

        } while (MergedScanOrderPtr < (pbi->MergedScanOrder + BLOCK_SIZE));

        MergedScanOrderPtr--;            //DEBUG_SHIV

        //        EobArray[blockIndex] =  pbi->EobOffsetTable[(unsigned int)(MergedScanOrderPtr - pbi->MergedScanOrder)];
        mbi->blockDxInfo[blockIndex].EobPos =  pbi->EobOffsetTable[(unsigned int)(MergedScanOrderPtr - pbi->MergedScanOrder)];    //DEBUG_SHIV

    } //for(blockIndex = 0; blockIndex < 6; blockIndex++)

    br->pos=(unsigned int) (br->buffer - pTempBrBuffer);
    br->buffer=pTempBrBuffer;
}


static inline BOOL VP6_DecodeMacroBlock ( PB_INSTANCE *pbi, U32 MBrow, U32 MBcol, CVP6Lib* vp6Lib )
{
    U32 thisRecon;
    U32 bp;

    MACROBLOCK_INFO *mbi = &pbi->mbi;

    // Remove uv channel processing if requested    
    U32 planeCount = pbi->NoUVProcessing ? 4 : 6;

    //***********************************************************************
    // Copy the existing structures into what we have now I'll fix this next.

    pbi->mbi.Mode = CODE_INTRA;


    if(pbi->FrameType != BASE_FRAME )
    {
         // this function decodes the mode and motion vector(s), as the name
         // implies, and also validates the motion vectors; returns FALSE if
         // the MVs do not check out
         if (!VP6_decodeModeAndMotionVector ( pbi, MBrow, MBcol ))
             return FALSE;

    }

    // read tokens from the bitstream and convert to coefficients.
    if ( pbi->UseHuffman )
    {
        ReadHuffTokensPredictA_MB(pbi);
    }
    else
    {
        VP6_ReadTokensPredictA_MB(pbi);
    }

    VP6_PredictDC_MB(pbi);

    bp = 0;

#ifdef VP6_DEBUG
     if(_u4Vp6DebugConter && _u4Vp6DebugConter==MBrow-BORDER_MBS)
     {
               _u4Vp6DebugConter++;
        if(_u4Vp6DebugCouter2&& _u4Vp6DebugCouter2==(MBcol-BORDER_MBS))
        {
           _u4Vp6DebugCouter2++;
        }
     }
#endif
// Call RECON() immediately after IDCT() of each 8x8 block 
    // ZERO MOTION VECTORS CASE
    if ( pbi->mbi.Mode == CODE_INTER_NO_MV )       // Inter with no motion vector
    {
        do
        {
            U32 EOBPos = mbi->blockDxInfo[bp].EobPos;
            thisRecon = pbi->mbi.blockDxInfo[bp].thisRecon;
            if(EOBPos == 1 && mbi->blockDxInfo[bp].coeffsPtr[0] == 0)  // All coeffs of 8x8 block are 0s
            {
                IDCT0_and_Recon_Inter(&pbi->ThisFrameRecon[thisRecon], 
                                     (U8 *)&pbi->LastFrameRecon[thisRecon],
                                      pbi->mbi.blockDxInfo[bp].CurrentReconStride);

            }
            else if(EOBPos == 1)    // Only DC coeff is non-zero 
            {
                S16 OutD;
                S16    *lInputData;
                S16    *lQuantMatrix;

                lInputData         = mbi->blockDxInfo[bp].coeffsPtr;
                lQuantMatrix    =  mbi->blockDxInfo[bp].dequantPtr;    
                
                OutD = (S16)((S32)(lInputData[0]*lQuantMatrix[0]+15)>>5);
                mbi->blockDxInfo[bp].coeffsPtr[0] = 0;
                
                IDCT1_plus_ReconInter((U8 *)&pbi->ThisFrameRecon[thisRecon], 
                                        (U8 *)&pbi->LastFrameRecon[thisRecon], 
                                        OutD, 
                                        pbi->mbi.blockDxInfo[bp].CurrentReconStride
                                    );
                
            }
            else if(EOBPos <= 10) // First 10 coeffs in zigzag order are non-zero
            {
                IDct10( mbi->blockDxInfo[bp].coeffsPtr, mbi->blockDxInfo[bp].dequantPtr, pbi->ReconDataBuffer[bp] );
                ResetCoeffBuffer(mbi->blockDxInfo[bp].coeffsPtr);
                ReconInter_C((short*) pbi->TmpDataBuffer, 
                (U8 *)&pbi->ThisFrameRecon[thisRecon], 
                (U8 *)&pbi->LastFrameRecon[thisRecon], 
                            pbi->ReconDataBuffer[bp], 
                                 pbi->mbi.blockDxInfo[bp].CurrentReconStride);


            }
            else                // More than 10 coeffs are non-zero, Regular case
            {
                  IDctSlow( mbi->blockDxInfo[bp].coeffsPtr, mbi->blockDxInfo[bp].dequantPtr, pbi->ReconDataBuffer[bp] );
                  FlashMemSet(mbi->blockDxInfo[bp].coeffsPtr, 0, 64*sizeof(Q_LIST_ENTRY));
                ReconInter_C((short *) pbi->TmpDataBuffer, 
                            (U8 *)&pbi->ThisFrameRecon[thisRecon], 
                            (U8 *)&pbi->LastFrameRecon[thisRecon], 
                            pbi->ReconDataBuffer[bp], 
                            pbi->mbi.blockDxInfo[bp].CurrentReconStride);
            }

        } while(++bp < planeCount);
        }
    // NON-ZERO MOTION VECTORS CASE
    else if ( VP6_ModeUsesMC[pbi->mbi.Mode] )          // The mode uses a motion vector.
    {
        do
        {
            U32 EOBPos = mbi->blockDxInfo[bp].EobPos;
            thisRecon = pbi->mbi.blockDxInfo[bp].thisRecon;
            if(EOBPos == 1 && mbi->blockDxInfo[bp].coeffsPtr[0] == 0)  // All coeffs of 8x8 block are 0s
            {
                VP6_PredictFilteredBlock( pbi, pbi->TmpDataBuffer,bp,vp6Lib);
                IDCT0_and_ReconBlock_8( pbi->TmpDataBuffer, 
                                      (U8 *)&pbi->ThisFrameRecon[thisRecon],
                                      pbi->mbi.blockDxInfo[bp].CurrentReconStride
                                     );
            
            }
            else if(EOBPos == 1)    // Only DC coeff is non-zero 
            {
                S16 OutD;
                S16    *lInputData;
                S16    *lQuantMatrix;
                VP6_PredictFilteredBlock( pbi, pbi->TmpDataBuffer,bp,vp6Lib);
                
                lInputData         = mbi->blockDxInfo[bp].coeffsPtr;
                lQuantMatrix    =  mbi->blockDxInfo[bp].dequantPtr;    
                
                OutD = (S16)((S32)(lInputData[0]*lQuantMatrix[0]+15)>>5);
                mbi->blockDxInfo[bp].coeffsPtr[0] = 0;
                
                IDCT1_plus_ReconBlock(    
                                        pbi->TmpDataBuffer,
                                        (U8 *)&pbi->ThisFrameRecon[thisRecon], 
                                        OutD, 
                                        pbi->mbi.blockDxInfo[bp].CurrentReconStride
                                    );
            }

            else if(EOBPos <= 10) // First 10 coeffs in zigzag order are non-zero
            {
                VP6_PredictFilteredBlock( pbi, pbi->TmpDataBuffer,bp,vp6Lib);

                IDct10( mbi->blockDxInfo[bp].coeffsPtr, mbi->blockDxInfo[bp].dequantPtr, pbi->ReconDataBuffer[bp] );
                ReconBlock_C_8( pbi->TmpDataBuffer, pbi->ReconDataBuffer[bp], (U8 *)&pbi->ThisFrameRecon[thisRecon], pbi->mbi.blockDxInfo[bp].CurrentReconStride);
                ResetCoeffBuffer(mbi->blockDxInfo[bp].coeffsPtr);
            }

            else                // More than 10 coeffs are non-zero, Regular case
            {
                VP6_PredictFilteredBlock( pbi, pbi->TmpDataBuffer,bp,vp6Lib);
                IDctSlow( mbi->blockDxInfo[bp].coeffsPtr, mbi->blockDxInfo[bp].dequantPtr, pbi->ReconDataBuffer[bp] );
                ReconBlock_C_8( pbi->TmpDataBuffer, pbi->ReconDataBuffer[bp], (U8 *)&pbi->ThisFrameRecon[thisRecon], pbi->mbi.blockDxInfo[bp].CurrentReconStride);

                FlashMemSet(mbi->blockDxInfo[bp].coeffsPtr, 0, 64*sizeof(Q_LIST_ENTRY));
            }

        } while(++bp < planeCount);
    }
    
    // GOLDEN REFERENCE FRAME CASE
    else if ( pbi->mbi.Mode == CODE_USING_GOLDEN )     // Golden frame with motion vector
    {
        do
        {
            U32 EOBPos = mbi->blockDxInfo[bp].EobPos;
            thisRecon = pbi->mbi.blockDxInfo[bp].thisRecon;
            if(EOBPos == 1 && mbi->blockDxInfo[bp].coeffsPtr[0] == 0)  // All coeffs of 8x8 block are 0s
            {
                
                IDCT0_and_Recon_Inter(&pbi->ThisFrameRecon[thisRecon], 
                                        (U8 *)&pbi->GoldenFrame[thisRecon],
                                        pbi->mbi.blockDxInfo[bp].CurrentReconStride
                                      );
    
            }
            else if(EOBPos == 1)    // Only DC coeff is non-zero 
            {
                S16 OutD;
                
                S16    *lInputData;
                S16    *lQuantMatrix;

                lInputData         = mbi->blockDxInfo[bp].coeffsPtr;
                lQuantMatrix    =  mbi->blockDxInfo[bp].dequantPtr;    
                
                OutD = (S16)((S32)(lInputData[0]*lQuantMatrix[0]+15)>>5);
                mbi->blockDxInfo[bp].coeffsPtr[0] = 0;
                
                IDCT1_plus_ReconInter((U8 *)&pbi->ThisFrameRecon[thisRecon], 
                                    (U8 *)&pbi->GoldenFrame[thisRecon], 
                                       OutD, 
                                    pbi->mbi.blockDxInfo[bp].CurrentReconStride );

            }
            else if(EOBPos <= 10) // First 10 coeffs in zigzag order are non-zero
            {

                IDct10( mbi->blockDxInfo[bp].coeffsPtr, mbi->blockDxInfo[bp].dequantPtr, pbi->ReconDataBuffer[bp] );
                ResetCoeffBuffer(mbi->blockDxInfo[bp].coeffsPtr);
                ReconInter_C( (short *)pbi->TmpDataBuffer, 
                            (U8 *)&pbi->ThisFrameRecon[thisRecon], 
                            (U8 *)&pbi->GoldenFrame[thisRecon], 
                            pbi->ReconDataBuffer[bp], 
                            pbi->mbi.blockDxInfo[bp].CurrentReconStride );

            }
            else                // More than 10 coeffs are non-zero, Regular case
            {
                  IDctSlow( mbi->blockDxInfo[bp].coeffsPtr, mbi->blockDxInfo[bp].dequantPtr, pbi->ReconDataBuffer[bp] );
                  FlashMemSet(mbi->blockDxInfo[bp].coeffsPtr, 0, 64*sizeof(Q_LIST_ENTRY));

                ReconInter_C((short *) pbi->TmpDataBuffer, 
                            (U8 *)&pbi->ThisFrameRecon[thisRecon], 
                            (U8 *)&pbi->GoldenFrame[thisRecon], 
                            pbi->ReconDataBuffer[bp], 
                            pbi->mbi.blockDxInfo[bp].CurrentReconStride );

            }
        } while(++bp < planeCount);
    }
    // Intra Picture Recon; dont change now
    else                                            // Simple Intra coding
    {
        do
        {
            U32 EOBPos = mbi->blockDxInfo[bp].EobPos;
            thisRecon = pbi->mbi.blockDxInfo[bp].thisRecon;
            if(EOBPos == 1 && mbi->blockDxInfo[bp].coeffsPtr[0] == 0)  // All coeffs of 8x8 block are 0s
            {
                FlashMemSet(pbi->ReconDataBuffer[bp], 0, 64*sizeof(Q_LIST_ENTRY));

                IDCT0_and_Recon_Intra( (U8 *)&pbi->ThisFrameRecon[thisRecon],
                                        (U16 *)pbi->ReconDataBuffer[bp],
                                        pbi->mbi.blockDxInfo[bp].CurrentReconStride
                                      );
            }
            else if(EOBPos == 1)    // Only DC coeff is non-zero 
            {
                IDct1( mbi->blockDxInfo[bp].coeffsPtr, mbi->blockDxInfo[bp].dequantPtr, pbi->ReconDataBuffer[bp] );
                mbi->blockDxInfo[bp].coeffsPtr[0] = 0;
                ReconIntra_C((short *) pbi->TmpDataBuffer,
                            (U8 *)&pbi->ThisFrameRecon[thisRecon], 
                            (U16 *)pbi->ReconDataBuffer[bp], 
                            pbi->mbi.blockDxInfo[bp].CurrentReconStride);
                                          
            }
            else if(EOBPos <= 10) // First 10 coeffs in zigzag order are non-zero
            {
                IDct10( mbi->blockDxInfo[bp].coeffsPtr, mbi->blockDxInfo[bp].dequantPtr, pbi->ReconDataBuffer[bp] );
                ReconIntra_C((short *) pbi->TmpDataBuffer, 
                            (U8 *)&pbi->ThisFrameRecon[thisRecon], 
                            (U16 *)pbi->ReconDataBuffer[bp], 
                            pbi->mbi.blockDxInfo[bp].CurrentReconStride);
                                               
                ResetCoeffBuffer(mbi->blockDxInfo[bp].coeffsPtr);
            }
            else                // More than 10 coeffs are non-zero, Regular case
            {
                IDctSlow( mbi->blockDxInfo[bp].coeffsPtr, mbi->blockDxInfo[bp].dequantPtr, pbi->ReconDataBuffer[bp] );
                FlashMemSet(mbi->blockDxInfo[bp].coeffsPtr, 0, 64*sizeof(Q_LIST_ENTRY));
                ReconIntra_C((short *) pbi->TmpDataBuffer, 
                            (U8 *)&pbi->ThisFrameRecon[thisRecon], 
                            (U16 *)pbi->ReconDataBuffer[bp], 
                            pbi->mbi.blockDxInfo[bp].CurrentReconStride);
            }
        } while(++bp < planeCount);
    }

    return TRUE;
}

//**************************************Header********************************//
#ifdef VP6_SWDEC_MTKOPTIMIZE
static void InitHeaderBuffer ( FRAME_HEADER *Header, unsigned char *Buffer, unsigned int BufferSize )
{
  Header->buffer = Buffer;
  Header->buffer_size = BufferSize;
  Header->buffer_end = Header->buffer + Header->buffer_size;
  Header->value  = (Header->buffer[0] << 24);
  Header->value |= (Header->buffer[1] << 16);
  Header->value |= (Header->buffer[2] <<  8);
  Header->value |= (Header->buffer[3] <<  0);
  Header->bits_available = 32;
  Header->pos = 4;
}
#else

static void InitHeaderBuffer ( FRAME_HEADER *Header, unsigned char *Buffer, unsigned int BufferSize )
{
  Header->buffer = Buffer;
  Header->buffer_size = BufferSize;
  Header->buffer_end = Header->buffer + Header->buffer_size;
    //Header->value  = (Buffer[0]<<24)+(Buffer[1]<<16)+(Buffer[2]<<8)+Buffer[3];
  if((Header->buffer + 0) < Header->buffer_end)
     Header->value  = (Header->buffer[0] << 24);
  if ((Header->buffer + 1) < Header->buffer_end)
     Header->value |= (Header->buffer[1] << 16);
  if ((Header->buffer + 2) < Header->buffer_end)
     Header->value |= (Header->buffer[2] <<  8);
  if ((Header->buffer + 3) < Header->buffer_end)
     Header->value |= (Header->buffer[3] <<  0);
  Header->bits_available = 32;
  Header->pos = 4;
}
#endif
static BOOL LoadFrameHeader ( PB_INSTANCE *pbi )
{
    U8  DctQMask;
    FRAME_HEADER *Header = &pbi->Header;
    BOOL   RetVal = TRUE;

    // Is the frame and inter frame or a key frame
    pbi->FrameType = (U8)ReadHeaderBits(Header, 1);

    // Quality (Q) index
    DctQMask = (U8)ReadHeaderBits(Header, 6);

    // Are we using two BOOL coder data streams/partitions
    pbi->MultiStream = (U8)ReadHeaderBits(Header, 1);

    // If the frame was a base frame then read the frame dimensions and build a bitmap structure. 
    if ( (pbi->FrameType == BASE_FRAME) )
    {
        // Read the frame dimensions bytes (0,0 indicates vp31 or later)
        pbi->Vp3VersionNo = (U8)ReadHeaderBits(Header,   5 );
        pbi->VpProfile = (U8)ReadHeaderBits(Header,   2 );

        if(pbi->Vp3VersionNo > CURRENT_DECODE_VERSION)
        {
            RetVal = FALSE;
            return RetVal;
        }

        // reserved
        if( (U8)ReadHeaderBits(Header, 1) != 0)
        {
            //this is an old test bitstream and is not supported
            return FALSE;
        }        

        // Start the first bool decoder (modes, mv, probs and some flags)
        // The offset depends on whether or not we are using multiple bool code streams
        if ( pbi->MultiStream || (pbi->VpProfile == SIMPLE_PROFILE) )
        {
            VP6_StartDecode(&pbi->br, ((unsigned char*)(Header->buffer + 4)), (unsigned char*)Header->buffer_end);

            // Read the buffer offset for the second bool decoder buffer if it is being used
            pbi->Buff2Offset = (U32)ReadHeaderBits(Header, 16);
        }
        else
        {
            VP6_StartDecode(&pbi->br, ((unsigned char*)(Header->buffer + 2)), (unsigned char*)Header->buffer_end);
        }

        {             
             U32 VFragments = 2 * ((U8)VP6_bitread( &pbi->br,   8 ));             
             U32 HFragments = 2 * ((U8)VP6_bitread( &pbi->br,   8 ));              

             // Flash Player: Output fragment sizes are unsupported
             VP6_bitread( &pbi->br, 8 );             
             VP6_bitread( &pbi->br, 8 );              

             // Flash Player: Scaling mode is unsupported
             VP6_bitread( &pbi->br, 2 );

             // we have a new input size
             if( VFragments != pbi->VFragments || HFragments != pbi->HFragments )
             {
                 // Validate the combination of height and width.                 
                 pbi->Configuration.VideoFrameWidth = HFragments*8;                 
                 pbi->Configuration.VideoFrameHeight = VFragments*8;

                 // enforce the requirement of non-zero dimensions
                  if ((pbi->Configuration.VideoFrameWidth == 0) ||
                      (pbi->Configuration.VideoFrameHeight == 0))
                      return FALSE;

                
                if (!pbi->psrCb(pbi->i4DecId,VP6_INSTANCE_CB_RESET_FRAME,pbi->Configuration.VideoFrameWidth,pbi->Configuration.VideoFrameHeight,0))
                     return FALSE;
             }
        }         

        // Unless in SIMPLE_PROFILE read the the filter strategy for fractional pels
        if ( pbi->VpProfile != SIMPLE_PROFILE )
        {
            // Find out if selective bicubic filtering should be used for motion prediction.
            if ( (BOOL)VP6_DecodeBool(&pbi->br, 128) )
            {
                pbi->PredictionFilterMode = AUTO_SELECT_PM;

                // Read in the variance threshold to be used
                pbi->PredictionFilterVarThresh = ((U32)VP6_bitread( &pbi->br, 5) << ((pbi->Vp3VersionNo > 7) ? 0 : 5) );

                // Read the bicubic vector length limit (0 actually means ignore vector length)
                pbi->PredictionFilterMvSizeThresh = (U8)VP6_bitread( &pbi->br, 3);
            }
            else
            {
                if ( (BOOL)VP6_DecodeBool(&pbi->br, 128) )
                    pbi->PredictionFilterMode = BICUBIC_ONLY_PM;
                else
                    pbi->PredictionFilterMode = BILINEAR_ONLY_PM;
            }

            if ( pbi->Vp3VersionNo > 7 )
                pbi->PredictionFilterAlpha = (U8)VP6_bitread( &pbi->br, 4);
            else
                pbi->PredictionFilterAlpha = 16;    // VP61 backwards compatibility
        }
    }
    // Non key frame specific stuff
    else
    {
        // No base frame decoded yet, this stream seems 
        // to have started with inter frames
        if ( pbi->HFragments == 0 || pbi->VFragments == 0 ) {
            return FALSE;
        }
    
        // Start the first bool decoder (modes, mv, probs and some flags)
        // The offset depends on whether or not we are using multiple bool code streams
        if ( pbi->MultiStream || (pbi->VpProfile == SIMPLE_PROFILE) )
        {
            VP6_StartDecode(&pbi->br, ((unsigned char*)(Header->buffer + 3)), (unsigned char*)Header->buffer_end);
            
            // Read the buffer offset for the second bool decoder buffer if it is being used
            pbi->Buff2Offset = (U32)ReadHeaderBits(Header, 16);
        }
        else
            VP6_StartDecode(&pbi->br, ((unsigned char*)(Header->buffer + 1)), (unsigned char*)Header->buffer_end);

        // Find out if the golden frame should be refreshed this frame - use bool decoder
        pbi->RefreshGoldenFrame = (BOOL)VP6_DecodeBool(&pbi->br, 128);

        if ( pbi->VpProfile != SIMPLE_PROFILE )
        {
            // Determine if loop filtering is on and if so what type should be used
            pbi->UseLoopFilter = VP6_DecodeBool(&pbi->br, 128);
            if ( pbi->UseLoopFilter )
            {
                pbi->UseLoopFilter = (pbi->UseLoopFilter << 1) | VP6_DecodeBool(&pbi->br, 128);
            }

            if ( pbi->Vp3VersionNo > 7 )
            {
                // Are the prediction characteristics being updated this frame
                if ( VP6_DecodeBool(&pbi->br, 128) )
                {
                    // Find out if selective bicubic filtering should be used for motion prediction.
                    if ( (BOOL)VP6_DecodeBool(&pbi->br, 128) )
                    {
                        pbi->PredictionFilterMode = AUTO_SELECT_PM;

                        // Read in the variance threshold to be used
                        pbi->PredictionFilterVarThresh = (U32)VP6_bitread( &pbi->br, 5);

                        // Read the bicubic vector length limit (0 actually means ignore vector length)
                        pbi->PredictionFilterMvSizeThresh = (U8)VP6_bitread( &pbi->br, 3);
                    }
                    else
                    {
                        if ( (BOOL)VP6_DecodeBool(&pbi->br, 128) )
                            pbi->PredictionFilterMode = BICUBIC_ONLY_PM;
                        else
                            pbi->PredictionFilterMode = BILINEAR_ONLY_PM;
                    }

                    pbi->PredictionFilterAlpha = (U8)VP6_bitread( &pbi->br, 4 );
                }
            }
            else
                pbi->PredictionFilterAlpha = 16;    // VP61 backwards compatibility
        }
    }

    // All frames (Key & Inter frames)

    // Should this frame use huffman for the dct data
    pbi->UseHuffman = (BOOL)VP6_DecodeBool(&pbi->br, 128);

    // Set this frame quality value from Q Index
    pbi->quantizer->FrameQIndex = DctQMask;

    VP6_UpdateQ(pbi->quantizer, pbi->Vp3VersionNo );  

     // compute the motion vector limits, expressed on a quarter-pel fractional map
     // account for 2 pixels on each side of a predicted block
     pbi->MacroblockOverscanLeftBoundary = 2 * 4;
     pbi->MacroblockOverscanRightBoundary = (BORDER_MBS * 2 + pbi->MBCols) * 16 * 4 - 2 * 4;
     pbi->MacroblockOverscanTopBoundary = 2 * 4;
     pbi->MacroblockOverscanBottomBoundary = pbi->MBRows * 16 * 4 - 2 * 4;
 
    return RetVal;                    
}

#ifdef VP6_ONLYY_TO_FB
static void VP6_Decode_CombineUV(U8 *InputUBuf,U8 *InputVBuf,U8 *outputBuf,U32 Height,U32 UVPitch)
{
  INT32 LineIndex=0,ColIndex=UVPitch,OutPutPos=0,InputPos=0;
  
  for(;LineIndex<Height;LineIndex++)
  {
     do
     {
        outputBuf[outputBuf++]=InputUBuf[InputPos];
        outputBuf[outputBuf++]=InputVBuf[InputPos++];
     }while(--ColIndex);
     ColIndex=UVPitch;
  }
  return;
}
#endif

static void VP6_DecodeFrameMbs ( PB_INSTANCE *pbi, CVP6Lib* vp6Lib)
{
#ifdef VP6_COMBUV_IN_DECODING
   // YV12_BUFFER_CONFIG *pCurFrameCfg=NULL;
   //U32 u4UVBoderOffset=(pbi->Configuration.UVStride*BORDER_MBS)>>1;
#endif
    if (pbi == NULL)
        return;
#ifdef VP6_COMBUV_IN_DECODING
    //pCurFrameCfg=pbi->thisFrameYV12Config;
#endif
    //U32 blockIndex;

    if(pbi->FrameType != BASE_FRAME )
    {
        VP6_DecodeModeProbs(pbi);
        VP6_ConfigureMvEntropyDecoder( pbi, pbi->FrameType );
        pbi->LastMode = CODE_INTER_NO_MV;
    }
    else
    {
        FlashMemCpy ( pbi->probXmitted,VP6_BaselineXmittedProbs,sizeof(pbi->probXmitted));
        FlashMemCpy ( pbi->IsMvShortProb, DefaultIsShortProbs, sizeof(pbi->IsMvShortProb) );
        FlashMemCpy ( pbi->MvShortProbs, DefaultMvShortProbs, sizeof(pbi->MvShortProbs) );
        FlashMemCpy ( pbi->MvSignProbs, DefaultSignProbs, sizeof(pbi->MvSignProbs) );
        FlashMemCpy ( pbi->MvSizeProbs, DefaultMvLongProbs, sizeof(pbi->MvSizeProbs) );
        FlashMemSet ( pbi->predictionMode,1,sizeof(char)*pbi->MacroBlocks );
        // Set up default scan order banding
        FlashMemCpy( pbi->ScanBands, DefaultScanBands, sizeof(pbi->ScanBands) );
        // Build the scan order
        VP6_BuildScanOrder( pbi, pbi->ScanBands );
    }

    
    VP6_ConfigureEntropyDecoder( pbi, pbi->FrameType ); 

    {
///////////////////////////////////////////////////////////////
// Optimisation #21
///////////////////////////////////////////////////////////////

        FlashMemCpy(&pbi->MergedScanOrder[0],&pbi->ModifiedScanOrder[0],64);

        // Create Huffman codes for tokens based on tree probabilities
        if ( pbi->UseHuffman )
        {
            VP6_ConvertDecodeBoolTrees ( pbi );

///////////////////////////////////////////////////////////////
// Optimisation #22
///////////////////////////////////////////////////////////////

            FlashMemCpy(&pbi->MergedScanOrder[64],&VP6_CoeffToHuffBand[0],65);

            // Reset Dc zero & Ac EOB run counters
            pbi->CurrentDcRunLen[0]  = 0;
            pbi->CurrentDcRunLen[1]  = 0;
            pbi->CurrentAc1RunLen[0] = 0;
            pbi->CurrentAc1RunLen[1] = 0;
        }
        else
        {
            FlashMemCpy(&pbi->MergedScanOrder[64],&VP6_CoeffToBand[0],65);
        }
    }

    // since we are on a new frame reset the above contexts 
    //VP6_D_ResetAboveContext(pbi);
    VP6_ResetAboveContext(pbi);
    
    {
        U32 MBrow;
        U32 MBRows = pbi->MBRows; 
        U32 MBCols = pbi->MBCols;

        MBCols -= BORDER_MBS;
        MBRows -= BORDER_MBS;

        // for each row of macroblocks 
        MBrow=BORDER_MBS;
        
        do
        {
           MACROBLOCK_INFO *mbi = &pbi->mbi;
           U32 MBcol;

            //VP6_D_ResetLeftContext(pbi);
           VP6_ResetLeftContext(pbi);

            // for each macroblock within a row of macroblocks

            mbi->blockDxInfo[0].Above = &pbi->fc.AboveY[BORDER_MBS*2];
            mbi->blockDxInfo[1].Above = &pbi->fc.AboveY[BORDER_MBS*2+1];
            mbi->blockDxInfo[2].Above = &pbi->fc.AboveY[BORDER_MBS*2];
            mbi->blockDxInfo[3].Above = &pbi->fc.AboveY[BORDER_MBS*2+1];
            mbi->blockDxInfo[4].Above = &pbi->fc.AboveU[BORDER_MBS];
            mbi->blockDxInfo[5].Above = &pbi->fc.AboveV[BORDER_MBS];
#ifdef VP6_COMBUV_IN_DECODING
            mbi->blockDxInfo[0].thisRecon=((MBrow * pbi->Configuration.YStride) << 4) + (BORDER_MBS * 16);
            mbi->blockDxInfo[1].thisRecon = mbi->blockDxInfo[0].thisRecon + 8;
            mbi->blockDxInfo[2].thisRecon = mbi->blockDxInfo[0].thisRecon + (pbi->Configuration.YStride << 3);
            mbi->blockDxInfo[3].thisRecon = mbi->blockDxInfo[1].thisRecon + (pbi->Configuration.YStride << 3);
            
            mbi->blockDxInfo[4].thisRecon = pbi->UVDataOffset+ ((MBrow * pbi->Configuration.UVStride)<< 3) + (BORDER_MBS * 16);
            mbi->blockDxInfo[5].thisRecon = mbi->blockDxInfo[4].thisRecon;
#else
            mbi->blockDxInfo[0].thisRecon = pbi->ReconYDataOffset + ((MBrow * pbi->Configuration.YStride) << 4) + (BORDER_MBS * 16);
            mbi->blockDxInfo[1].thisRecon = mbi->blockDxInfo[0].thisRecon + 8;
            mbi->blockDxInfo[2].thisRecon = mbi->blockDxInfo[0].thisRecon + (pbi->Configuration.YStride << 3);
            mbi->blockDxInfo[3].thisRecon = mbi->blockDxInfo[1].thisRecon + (pbi->Configuration.YStride << 3);

            mbi->blockDxInfo[4].thisRecon = pbi->ReconUDataOffset + ((MBrow * pbi->Configuration.UVStride) << 3) + (BORDER_MBS * 8);
            mbi->blockDxInfo[5].thisRecon = pbi->ReconVDataOffset + ((MBrow * pbi->Configuration.UVStride) << 3) + (BORDER_MBS * 8);
#endif
            MBcol=BORDER_MBS;
            //HalFlushInvalidateDCache();
            do
            {
                 // Decode the macroblock; if anything about the MB is bad (like ridiculous motion
                 // vectors), bail on the entire frame decode
                 if ( !VP6_DecodeMacroBlock(pbi, MBrow, MBcol, vp6Lib) )
                     return;


                mbi->blockDxInfo[0].Above += 2;
                mbi->blockDxInfo[1].Above += 2;
                mbi->blockDxInfo[2].Above += 2;
                mbi->blockDxInfo[3].Above += 2;
                mbi->blockDxInfo[4].Above += 1;
                mbi->blockDxInfo[5].Above += 1;

                mbi->blockDxInfo[0].thisRecon += 16;
                mbi->blockDxInfo[1].thisRecon += 16;
                mbi->blockDxInfo[2].thisRecon += 16;
                mbi->blockDxInfo[3].thisRecon += 16;
#ifdef VP6_COMBUV_IN_DECODING
                mbi->blockDxInfo[4].thisRecon += 16;
                mbi->blockDxInfo[5].thisRecon += 16;
#else
                mbi->blockDxInfo[4].thisRecon += 8;
                mbi->blockDxInfo[5].thisRecon += 8;
#endif


            } while(++MBcol < MBCols);


        } while(++MBrow < MBRows);
    }
}

//******************************Decoded Process**************************************/

static void  On2YV12_ExtendFrameBorders (YV12_BUFFER_CONFIG *ybf, int noUVChannels)
{
    const U32 width=ybf->YWidth;
    U32 height=ybf->YHeight;
    const U32 stride=ybf->YStride;
    U32 border=ybf->border;

    U8* pLeftSrc=(U8*)ybf->YBuffer;
    U8* pLeftDest=pLeftSrc-border;
    U8* pRightSrc=pLeftSrc+width-1;
    U8* pRightDest=pRightSrc+1;

    U32 uvHeight=height>>1;
#ifdef VP6_COMBUV_IN_DECODING
    const U32 uvWidth=width+(width&0x1);
    U32 uvBorder=border;
#else
    const U32 uvWidth=width>>1;
    U32 uvBorder=border>>1;
#endif
    U8* pTopSrc=pLeftDest;
    U8* pBottomSrc=pLeftDest+(height*stride)-stride;

    // Copy the left and right most columns out 
    for ( ; height--; )
        {
        FlashMemSet(pLeftDest,pLeftSrc[0],border);
        FlashMemSet(pRightDest,pRightSrc[0],border);
        pLeftSrc+=stride;
        pRightSrc+=stride;
        pLeftDest+=stride;
        pRightDest+=stride;
        }

    // Now copy the top and bottom source lines into each line of the
    // respective borders
    U8* pTopDest=pTopSrc-(border*stride);
    U8* pBottomDest=pBottomSrc+stride;

    for ( ; border--; )
        {
        FlashMemCpy(pTopDest,pTopSrc,stride);
        FlashMemCpy(pBottomDest,pBottomSrc,stride);
        pTopDest+=stride;
        pBottomDest+=stride;
        }

#ifdef VP6_COMBUV_IN_DECODING
    if(!noUVChannels)
    {
       const U32 uvStride=ybf->UVStride;
       U8* pLeftUVSrc=(U8*)ybf->UVBuffer;
       U8* pLeftUVDest=pLeftUVSrc-uvBorder;
       U8* pRightUVSrc=pLeftUVSrc+uvWidth-2;
       U8* pRightUVDest=pRightUVSrc+2;
       U32 Vborder=uvHeight;
       U32 Hborder=Vborder;
       do
       {
         U16 u2LeftSrcData=(U16)(*((U16 *)pLeftUVSrc));
         U16 u2RightSrcData=(U16)(*((U16 *)pRightUVSrc));
         U16 *pu2LDestPos=(U16 *)pLeftUVDest;
         U16 *pu2RDestPos=(U16 *)pRightUVDest;
         Hborder=uvBorder>>1;
         do
         {
            *pu2LDestPos++=u2LeftSrcData;
            *pu2RDestPos++=u2RightSrcData;
         }while(--Hborder);

         pLeftUVSrc+=uvStride;
         pRightUVSrc+=uvStride;
         pLeftUVDest+=uvStride;
         pRightUVDest+=uvStride;
       }while(--Vborder);

       Vborder=uvBorder>>1;
       U8* pTopUVSrc=(U8*)(ybf->UVBuffer-uvBorder);
       U8* pBottomUVSrc=pTopUVSrc+(uvHeight*uvStride)-uvStride;
       U8* pTopUVDest=pTopUVSrc-(Vborder*uvStride);
       U8* pBottomUVDest=pBottomUVSrc+uvStride;
       do
       {
          FlashMemCpy(pTopUVDest,pTopUVSrc,uvStride);
          FlashMemCpy(pBottomUVDest,pBottomUVSrc,uvStride);
          pTopUVDest+=uvStride;
          pBottomUVDest+=uvStride;
       }while(--Vborder);
    }
#else

    if (!noUVChannels)
        {
        // copy the left and right most columns out 
        const U32 uvStride=ybf->UVStride;
        U8* pLeftUSrc=(U8*)ybf->UBuffer;
        U8* pLeftVSrc=(U8*)ybf->VBuffer;
        U8* pLeftUDest=pLeftUSrc-uvBorder;
        U8* pLeftVDest=pLeftVSrc-uvBorder;
        U8* pRightUSrc=pLeftUSrc+uvWidth-1;
        U8* pRightVSrc=pLeftVSrc+uvWidth-1;
        U8* pRightUDest=pRightUSrc+1;
        U8* pRightVDest=pRightVSrc+1;
        U32 tH=uvHeight;
        for ( ; uvHeight--; )
            {
            // U plane first.
            FlashMemSet(pLeftUDest,pLeftUSrc[0],uvBorder);
            FlashMemSet(pRightUDest,pRightUSrc[0],uvBorder);
            pLeftUSrc+=uvStride;
            pRightUSrc+=uvStride;
            pLeftUDest+=uvStride;
            pRightUDest+=uvStride;

            // V plane next.
            FlashMemSet(pLeftVDest,pLeftVSrc[0],uvBorder);
            FlashMemSet(pRightVDest,pRightVSrc[0],uvBorder);
            pLeftVSrc+=uvStride;
            pRightVSrc+=uvStride;
            pLeftVDest+=uvStride;
            pRightVDest+=uvStride;
            }

        // Now copy the top and bottom source lines into each line of the
        // respective borders
        U8* pTopUSrc=(U8*)(ybf->UBuffer-uvBorder);
        U8* pBottomUSrc=pTopUSrc+(tH*uvStride)-uvStride;
        U8* pTopUDest=pTopUSrc-(uvBorder*uvStride);
        U8* pBottomUDest=pBottomUSrc+uvStride;
        U8* pTopVSrc=(U8*)(ybf->VBuffer-uvBorder);
        U8* pBottomVSrc=pTopVSrc+(tH*uvStride)-uvStride;
        U8* pTopVDest=pTopVSrc-(uvBorder*uvStride);
        U8* pBottomVDest=pBottomVSrc+uvStride;
        for ( ; uvBorder--; )
            {
            FlashMemCpy(pTopUDest,pTopUSrc,uvStride);
            FlashMemCpy(pBottomUDest,pBottomUSrc,uvStride);
            pTopUDest+=uvStride;
            pBottomUDest+=uvStride;
            FlashMemCpy(pTopVDest,pTopVSrc,uvStride);
            FlashMemCpy(pBottomVDest,pBottomVSrc,uvStride);
            pTopVDest+=uvStride;
            pBottomVDest+=uvStride;
            }
     }
#endif
}

#ifndef UNCOMPILE_UNUSED_FUNCTION
static void On2YV12_CopyFrame(YV12_BUFFER_CONFIG *srcYBC, YV12_BUFFER_CONFIG *dstYBC, int noUVChannels)
{

    const U32 width=srcYBC->YWidth;
    U32 height=srcYBC->YHeight;
    const U32 stride=dstYBC->YStride;
    U8* pSrc=(U8*)srcYBC->YBuffer;
    U8* pDest=(U8*)dstYBC->YBuffer;
    for ( ; height--; )
        {
        FlashMemCpy(pDest,pSrc,width);
        pSrc+=stride;
        pDest+=stride;
        }

    if (!noUVChannels)
        {
        const U32 width=srcYBC->UVWidth;
        U32 height=srcYBC->UVHeight;
        const U32 stride=srcYBC->UVStride;
        U8* pDestU=(U8*)dstYBC->UBuffer;
        U8* pDestV=(U8*)dstYBC->VBuffer;
        U8* pSrcU=(U8*)srcYBC->UBuffer;
        U8* pSrcV=(U8*)srcYBC->VBuffer;
          for ( ; height--; )
            {
            FlashMemCpy(pDestU,pSrcU,width);
            FlashMemCpy(pDestV,pSrcV,width);
            pSrcU+=stride;
            pSrcV+=stride;
            pDestU+=stride;
            pDestV+=stride;
            }
        }

    On2YV12_ExtendFrameBorders(dstYBC,noUVChannels);
}

#endif
static void VP6_InitMBI(PB_INSTANCE *pbi)
{
    pbi->mbi.blockDxInfo[0].ZeroRunProbsBasePtr = 
    pbi->mbi.blockDxInfo[1].ZeroRunProbsBasePtr = 
    pbi->mbi.blockDxInfo[2].ZeroRunProbsBasePtr = 
    pbi->mbi.blockDxInfo[3].ZeroRunProbsBasePtr = 
    pbi->mbi.blockDxInfo[4].ZeroRunProbsBasePtr = 
    pbi->mbi.blockDxInfo[5].ZeroRunProbsBasePtr = (U8 *)pbi->ZeroRunProbs;

    pbi->mbi.blockDxInfo[0].AcProbsBasePtr = 
    pbi->mbi.blockDxInfo[1].AcProbsBasePtr = 
    pbi->mbi.blockDxInfo[2].AcProbsBasePtr = 
    pbi->mbi.blockDxInfo[3].AcProbsBasePtr = pbi->AcProbs + ACProbOffset(0,0,0,0);
    pbi->mbi.blockDxInfo[4].AcProbsBasePtr = 
    pbi->mbi.blockDxInfo[5].AcProbsBasePtr = pbi->AcProbs + ACProbOffset(1,0,0,0);

    pbi->mbi.blockDxInfo[0].DcProbsBasePtr = 
    pbi->mbi.blockDxInfo[1].DcProbsBasePtr = 
    pbi->mbi.blockDxInfo[2].DcProbsBasePtr = 
    pbi->mbi.blockDxInfo[3].DcProbsBasePtr = pbi->DcProbs + DCProbOffset(0,0);
    pbi->mbi.blockDxInfo[4].DcProbsBasePtr = 
    pbi->mbi.blockDxInfo[5].DcProbsBasePtr = pbi->DcProbs + DCProbOffset(1,0);

    pbi->mbi.blockDxInfo[0].DcNodeContextsBasePtr = 
    pbi->mbi.blockDxInfo[1].DcNodeContextsBasePtr = 
    pbi->mbi.blockDxInfo[2].DcNodeContextsBasePtr = 
    pbi->mbi.blockDxInfo[3].DcNodeContextsBasePtr = pbi->DcNodeContexts + DcNodeOffset(0,0,0);
    pbi->mbi.blockDxInfo[4].DcNodeContextsBasePtr = 
    pbi->mbi.blockDxInfo[5].DcNodeContextsBasePtr = pbi->DcNodeContexts + DcNodeOffset(1,0,0);

    pbi->mbi.blockDxInfo[0].dequantPtr = pbi->quantizer->dequant_coeffs[VP6_QTableSelect[0]];
    pbi->mbi.blockDxInfo[1].dequantPtr = pbi->quantizer->dequant_coeffs[VP6_QTableSelect[1]];
    pbi->mbi.blockDxInfo[2].dequantPtr = pbi->quantizer->dequant_coeffs[VP6_QTableSelect[2]];
    pbi->mbi.blockDxInfo[3].dequantPtr = pbi->quantizer->dequant_coeffs[VP6_QTableSelect[3]];
    pbi->mbi.blockDxInfo[4].dequantPtr = pbi->quantizer->dequant_coeffs[VP6_QTableSelect[4]];
    pbi->mbi.blockDxInfo[5].dequantPtr = pbi->quantizer->dequant_coeffs[VP6_QTableSelect[5]];

    pbi->mbi.blockDxInfo[0].LastDc = 
    pbi->mbi.blockDxInfo[1].LastDc = 
    pbi->mbi.blockDxInfo[2].LastDc = 
    pbi->mbi.blockDxInfo[3].LastDc = pbi->fc.LastDcY;
    pbi->mbi.blockDxInfo[4].LastDc = pbi->fc.LastDcU;
    pbi->mbi.blockDxInfo[5].LastDc = pbi->fc.LastDcV;

    pbi->mbi.blockDxInfo[0].Left = &pbi->fc.LeftY[0];
    pbi->mbi.blockDxInfo[1].Left = &pbi->fc.LeftY[0];
    pbi->mbi.blockDxInfo[2].Left = &pbi->fc.LeftY[1];
    pbi->mbi.blockDxInfo[3].Left = &pbi->fc.LeftY[1];
    pbi->mbi.blockDxInfo[4].Left = &pbi->fc.LeftU;
    pbi->mbi.blockDxInfo[5].Left = &pbi->fc.LeftV;

    pbi->mbi.blockDxInfo[0].MvShift =
    pbi->mbi.blockDxInfo[1].MvShift =
    pbi->mbi.blockDxInfo[2].MvShift =
    pbi->mbi.blockDxInfo[3].MvShift = Y_MVSHIFT;
    pbi->mbi.blockDxInfo[4].MvShift =
    pbi->mbi.blockDxInfo[5].MvShift = UV_MVSHIFT;

    pbi->mbi.blockDxInfo[0].MvModMask =
    pbi->mbi.blockDxInfo[1].MvModMask =
    pbi->mbi.blockDxInfo[2].MvModMask =
    pbi->mbi.blockDxInfo[3].MvModMask = Y_MVMODMASK;
    pbi->mbi.blockDxInfo[4].MvModMask =
    pbi->mbi.blockDxInfo[5].MvModMask = UV_MVMODMASK;

#ifdef VP6_COMBUV_IN_DECODING
    pbi->mbi.blockDxInfo[0].CurrentReconStride =
    pbi->mbi.blockDxInfo[1].CurrentReconStride =
    pbi->mbi.blockDxInfo[2].CurrentReconStride =
    pbi->mbi.blockDxInfo[3].CurrentReconStride = (VP6_Y_COMPONENT<<16)|pbi->Configuration.YStride;
    pbi->mbi.blockDxInfo[4].CurrentReconStride = (VP6_U_COMPONENT<<16)|pbi->Configuration.UVStride;
    pbi->mbi.blockDxInfo[5].CurrentReconStride = (VP6_V_COMPONENT<<16)|pbi->Configuration.UVStride;
#else
#ifdef VP6_COMB_FUNCTION_VERIFY
    pbi->mbi.blockDxInfo[0].CurrentReconStride =
    pbi->mbi.blockDxInfo[1].CurrentReconStride =
    pbi->mbi.blockDxInfo[2].CurrentReconStride =
    pbi->mbi.blockDxInfo[3].CurrentReconStride = (VP6_Y_COMPONENT<<16)|pbi->Configuration.YStride;
    pbi->mbi.blockDxInfo[4].CurrentReconStride = (VP6_U_COMPONENT<<16)|pbi->Configuration.UVStride;
    pbi->mbi.blockDxInfo[5].CurrentReconStride = (VP6_V_COMPONENT<<16)|pbi->Configuration.UVStride;
#else
    pbi->mbi.blockDxInfo[0].CurrentReconStride =
    pbi->mbi.blockDxInfo[1].CurrentReconStride =
    pbi->mbi.blockDxInfo[2].CurrentReconStride =
    pbi->mbi.blockDxInfo[3].CurrentReconStride = pbi->Configuration.YStride;
    pbi->mbi.blockDxInfo[4].CurrentReconStride = pbi->Configuration.UVStride;
    pbi->mbi.blockDxInfo[5].CurrentReconStride = pbi->Configuration.UVStride;
#endif
#endif

    pbi->mbi.blockDxInfo[0].FrameReconStride =
    pbi->mbi.blockDxInfo[1].FrameReconStride =
    pbi->mbi.blockDxInfo[2].FrameReconStride =
    pbi->mbi.blockDxInfo[3].FrameReconStride = pbi->Configuration.YStride;
    pbi->mbi.blockDxInfo[4].FrameReconStride =
    pbi->mbi.blockDxInfo[5].FrameReconStride = pbi->Configuration.UVStride;

    // Default clear data area down to 0s
    FlashMemSet(pbi->mbi.coeffsBasePtr, 0, 6*64*sizeof(Q_LIST_ENTRY));

    //______ compressor only ______
    pbi->mbi.blockDxInfo[0].FrameSourceStride =
    pbi->mbi.blockDxInfo[1].FrameSourceStride =
    pbi->mbi.blockDxInfo[2].FrameSourceStride =
    pbi->mbi.blockDxInfo[3].FrameSourceStride = pbi->Configuration.VideoFrameWidth;
    pbi->mbi.blockDxInfo[4].FrameSourceStride =
    pbi->mbi.blockDxInfo[5].FrameSourceStride = pbi->Configuration.VideoFrameWidth/2;

    pbi->mbi.blockDxInfo[0].CurrentSourceStride =
    pbi->mbi.blockDxInfo[1].CurrentSourceStride =
    pbi->mbi.blockDxInfo[2].CurrentSourceStride =
    pbi->mbi.blockDxInfo[3].CurrentSourceStride = pbi->Configuration.VideoFrameWidth;
    pbi->mbi.blockDxInfo[4].CurrentSourceStride =
    pbi->mbi.blockDxInfo[5].CurrentSourceStride = pbi->Configuration.VideoFrameWidth/2;

    pbi->mbi.blockDxInfo[0].Plane =
    pbi->mbi.blockDxInfo[1].Plane =
    pbi->mbi.blockDxInfo[2].Plane =
    pbi->mbi.blockDxInfo[3].Plane = 0;
    pbi->mbi.blockDxInfo[4].Plane =
    pbi->mbi.blockDxInfo[5].Plane = 1;
    //______ compressor only ______

}


static S8  VP6_DecodeFrameToYUV (PB_INSTANCE *pbi, char *VideoBufferPtr, unsigned int ByteCount, U32 ImageWidth,U32 ImageHeight)
{

  pbi->CurrentFrameSize = ByteCount;
     // initialize br3 for good measure (otherwise it will remain
    // uninitialized which proves dangerous for fuzzed files)
  pbi->br3.bitsinremainder = 0;
  pbi->br3.remainder = 0;
  pbi->br3.position = (unsigned char *)VideoBufferPtr;
  pbi->br3.end = ((unsigned char *)VideoBufferPtr) + ByteCount;

    //  Start the second boolean decoder
  if(pbi->MultiStream || (pbi->VpProfile == SIMPLE_PROFILE) )
  {
    pbi->mbi.br = &pbi->br2;
    if(pbi->UseHuffman )
    {
        // Initialise BITREADER for second bitstream partition
        pbi->br3.bitsinremainder = 0;
        pbi->br3.remainder = 0;
        pbi->br3.position = ((unsigned char*)VideoBufferPtr)+pbi->Buff2Offset;
    }
    else
    {
        VP6_StartDecode(&pbi->br2,((unsigned char*)VideoBufferPtr)+pbi->Buff2Offset, ((unsigned char*)VideoBufferPtr)+ByteCount);
    }
  }
  else
  {
     pbi->mbi.br = &pbi->br;
  }

    
    // decode and reconstruct frame
  VP6_DecodeFrameMbs(pbi, &pbi->vp6Lib);
    
#if 0
/*
    // switch pointers so lastframe recon is this frame
    {
        unsigned char *tmp;
        tmp = pbi->LastFrameRecon;
        pbi->LastFrameRecon = pbi->ThisFrameRecon;
        pbi->ThisFrameRecon = tmp;
    }
    //switch configurations
    {
        YV12_BUFFER_CONFIG *tmp;
        tmp = pbi->lastFrameYV12Config;
        pbi->lastFrameYV12Config = pbi->thisFrameYV12Config;
        pbi->thisFrameYV12Config = tmp;
    }
*/
    // update the border 
    On2YV12_ExtendFrameBorders(pbi->lastFrameYV12Config, pbi->NoUVProcessing);

    // Update the golden frame buffer
    if( (pbi->FrameType == BASE_FRAME) || pbi->RefreshGoldenFrame )
        //memcpy(pbi->GoldenFrame, pbi->LastFrameRecon, pbi->ReconYPlaneSize + 2* pbi->ReconUVPlaneSize); 
        On2YV12_CopyFrame(pbi->lastFrameYV12Config, pbi->goldenFrameYV12Config, pbi->NoUVProcessing);
#endif
///////////////////////////////////////////////////////////////
// Optimisation #50
///////////////////////////////////////////////////////////////

    if(pbi->FrameType == BASE_FRAME )
        pbi->AvgFrameQIndex = pbi->quantizer->FrameQIndex;
    else
        pbi->AvgFrameQIndex = (2 + 3 * pbi->AvgFrameQIndex + pbi->quantizer->FrameQIndex) / 4 ;

    return 0;
}


BOOL _VDEC_Vp6DecSetCb(U32 u4Instance,U32 u4CbPtr)
{
  PB_INSTANCE *pDecInstance;
  if(!u4Instance || !u4CbPtr)
  {
    return FALSE;
  }

  pDecInstance=(PB_INSTANCE *)u4Instance;
  pDecInstance->psrCb=(VP6_INSTANCE_CB)u4CbPtr;
  return TRUE;
}

BOOL _VDEC_Vp6SwSetReturnParam(U32 u4Instance,void *Param1Addr,void *Param2Addr,void *Param3Addr,void *Param4Addr)
{
  PB_INSTANCE *pDecInstance;
  if(!u4Instance)
  {
    return FALSE;
  }
  
  pDecInstance=(PB_INSTANCE *)u4Instance;
  pDecInstance->pfgRefreshGd=(BOOL *)Param1Addr;
  pDecInstance->pfgIsKeyFrame=(BOOL *)Param3Addr;
  pDecInstance->pu4ReadPos=(U32 *)Param2Addr;
  return TRUE;
}

BOOL _VDEC_VP6SwCreateInstance(U32 *u4Instance,S32 i4DecId,VP6_INSTANCE_CB psrCb)
{
   PB_INSTANCE *pNewInstance;
   if(!u4Instance ||!psrCb)
   {
     return FALSE;
   }
   
   if(psrCb)
   {
      if(!psrCb(i4DecId,VP6_INSTANCE_CB_MALLOC,(S32)(&pNewInstance),sizeof(PB_INSTANCE),32))
      {
        return FALSE;
      }
#ifdef VP6_DEBUG
      psrCb(i4DecId,VP6_INSTANCE_CB_DBGPARAM,(U32)(&_u4Vp6DebugConter),(U32)(&_u4Vp6DebugCouter2),0);
#endif
   }
   
   if(!pNewInstance)
   {
     return FALSE;
   }
   
   x_memset(pNewInstance,0,sizeof(PB_INSTANCE));
   pNewInstance->i4DecId=i4DecId;
   pNewInstance->psrCb=psrCb;
   *u4Instance=(U32)pNewInstance;
   return TRUE;
}

BOOL _VDEC_Vp6DecReleaseInstance(U32 u4Instance)
{
  PB_INSTANCE *pNewInstance=(PB_INSTANCE *)u4Instance;
  if(!u4Instance)
  {
    return FALSE;
  }
  pNewInstance->psrCb(pNewInstance->i4DecId,VP6_INSTANCE_CB_FREE,u4Instance,0,0);
  return TRUE;
}

BOOL _VDEC_Vp6DecGetPicInfo(U32 u4Instance,U32 *pu4Yaddr,U32 *puCaddr,U32 *puPitch,U32 *puYDataOffset,U32 *puCDataOffset)
{
  PB_INSTANCE *pNewInstance=(PB_INSTANCE *)u4Instance;
  YV12_BUFFER_CONFIG *prConfig;
  if(!pNewInstance)
  {
     return FALSE;
  }

  prConfig=pNewInstance->lastFrameYV12Config;
  if(puPitch)
  {
     *puPitch=pNewInstance->Configuration.YStride;
  }

  if(puYDataOffset)
  {
    *puYDataOffset=pNewInstance->Configuration.YStride*UMV_BORDER+UMV_BORDER;
  }

  if(puCDataOffset)
  {
    *puCDataOffset=pNewInstance->Configuration.UVStride*(UMV_BORDER>>1)+UMV_BORDER;
  }

  if(prConfig)
  {
    if(pu4Yaddr)
    {
        *pu4Yaddr=(U32)prConfig->YBuffer;
    }

    if(puCaddr)
    {
#ifdef VP6_COMBUV_IN_DECODING
        *puCaddr=(U32)prConfig->UVBuffer;
#else
        *puCaddr=(U32)prConfig->UBuffer;
#endif
    }
  }
 return TRUE;
}

S32 _VDEC_Vp6DecDecodeFrame(U32 u4Instance,U8 *puFrameData,U32 u4DataLen)
{
  PB_INSTANCE *pNewInstance=(PB_INSTANCE *)u4Instance;
  VP6_DecodeFrameToYUV(pNewInstance,(char *)puFrameData,u4DataLen,pNewInstance->OutputWidth,pNewInstance->OutputHeight);

  if(pNewInstance->UseHuffman)
  {
     *pNewInstance->pu4ReadPos=(U32)(pNewInstance->br3.position);
  }
  else
  {
     *pNewInstance->pu4ReadPos=(U32)(pNewInstance->br3.position);
     *pNewInstance->pu4ReadPos=(U32)(pNewInstance->mbi.br->buffer+pNewInstance->mbi.br->pos-20);
  }
  return 0;
}

BOOL  _Vdec_VP6SwGetHeaderInfo( U32 u4Instance,U8 *pBitstream,U32 u4Len )
{ 
    PB_INSTANCE *pbi=(PB_INSTANCE *)u4Instance;
    BOOL RetVal = TRUE; 
    InitHeaderBuffer(&pbi->Header,pBitstream,u4Len);
    // Load the frame header (including the frame size).     
    RetVal=LoadFrameHeader(pbi);
    *pbi->pfgIsKeyFrame=!pbi->FrameType;
    *pbi->pfgRefreshGd=pbi->RefreshGoldenFrame;
    return RetVal;
}

BOOL _VDEC_Vp6DecUpdateFrameBuf(U32 u4Instance,U32 u4FrameType,U32 u4YAddr,U32 u4CAddr)
{
    PB_INSTANCE *pbi=(PB_INSTANCE *)u4Instance;
    YV12_BUFFER_CONFIG *pFrmCfg=NULL;
#ifndef VP6_USE_IMGRZ
    switch(u4FrameType)
    {
       case VP6_UPDATE_FRAME_CUR:
       {
          U32 index=0;
          for(;index<3;index++)
          {
            if(!pbi->rFrameYV12Config[index].used)
            { 
               pFrmCfg=&pbi->rFrameYV12Config[index];
               break;
            }
          }
          
          if(index==3)
          {
            return FALSE;
          }
          pbi->thisFrameYV12Config=pFrmCfg;
#ifdef VP6_COMBUV_IN_DECODING
          pFrmCfg->YBuffer= (char *)u4YAddr+UMV_BORDER*pFrmCfg->YStride+UMV_BORDER;
          pFrmCfg->UVBuffer=(char *)u4CAddr+(UMV_BORDER>>1)*pFrmCfg->YStride+UMV_BORDER;
          pbi->UVDataOffset=u4CAddr-u4YAddr;
#else
          int YPlaneSize   = pFrmCfg->YStride*(pFrmCfg->YHeight+2*UMV_BORDER);
          int UVPlaneSize  = pFrmCfg->UVStride*(pFrmCfg->UVHeight+UMV_BORDER);
          pFrmCfg->YBuffer = (char *)(u4YAddr+ UMV_BORDER * pFrmCfg->YStride + UMV_BORDER);

          pFrmCfg->UBuffer = (char *)(u4YAddr+ YPlaneSize + UMV_BORDER / 2  * pFrmCfg->UVStride + UMV_BORDER / 2);
          pFrmCfg->VBuffer = (char *)(u4YAddr+ YPlaneSize + UVPlaneSize + UMV_BORDER / 2  * pFrmCfg->UVStride + UMV_BORDER / 2);
#endif
          pbi->ThisFrameRecon=(YUV_BUFFER_ENTRY *)u4YAddr;
          pFrmCfg->used=1;
          break;
       }

       case VP6_UPDATE_FRAME_LAST:
       {  
          if(pbi->lastFrameYV12Config && pbi->lastFrameYV12Config!=pbi->goldenFrameYV12Config)
          {
            pbi->lastFrameYV12Config->used=0;
          }
          pbi->lastFrameYV12Config=pbi->thisFrameYV12Config;
          pbi->LastFrameRecon=pbi->ThisFrameRecon;
          On2YV12_ExtendFrameBorders(pbi->lastFrameYV12Config, pbi->NoUVProcessing);
          break;
       }
       case VP6_UPDATE_FRAME_GOLDEN:
       {
          if(pbi->goldenFrameYV12Config&&pbi->lastFrameYV12Config!=pbi->goldenFrameYV12Config)
          {
             pbi->goldenFrameYV12Config->used=0;
          }
          pbi->goldenFrameYV12Config=pbi->thisFrameYV12Config;
          pbi->GoldenFrame=pbi->ThisFrameRecon;
          break;
       }
       case VP6_UPDATE_SET_INIT:
       {
         pbi->UVDataOffset=u4CAddr-u4YAddr;
         break;
       }
       default:
          break;
    }
#else
     switch(u4FrameType)
     {
       case VP6_UPDATE_FRAME_CUR:
       {
          U32 index=0;
          for(;index<3;index++)
          {
            if(!pbi->rFrameYV12Config[index].used)
            { 
               pFrmCfg=&pbi->rFrameYV12Config[index];
               break;
            }
          }
          
          if(index==3)
          {
            return FALSE;
          }
          
          pbi->ThisFrameRecon=(YUV_BUFFER_ENTRY *)(pFrmCfg->BaseBuffer);

          pbi->thisFrameYV12Config=pFrmCfg;
          pFrmCfg->used=1;
          break;
       }
       case VP6_UPDATE_FRAME_LAST:
       {
             VP6_IMGRZ_PARAM rResizeParam;
          if(pbi->lastFrameYV12Config && pbi->lastFrameYV12Config!=pbi->goldenFrameYV12Config)
          {
            pbi->lastFrameYV12Config->used=0;
          }
          pbi->lastFrameYV12Config=pbi->thisFrameYV12Config;
          pbi->LastFrameRecon=pbi->ThisFrameRecon;
          pFrmCfg=pbi->lastFrameYV12Config;
          On2YV12_ExtendFrameBorders(pFrmCfg, pbi->NoUVProcessing);
          rResizeParam.u4InY=(U32)pFrmCfg->YBuffer;
          rResizeParam.u4InCb=(U32)pFrmCfg->UBuffer;
          rResizeParam.u4InCr=(U32)pFrmCfg->VBuffer;
          rResizeParam.u4OutY=u4YAddr;
          rResizeParam.u4OutC=u4CAddr;
          rResizeParam.u4RZBuf=pbi->u4RzWorkBuf;
          pbi->psrCb(pbi->i4DecId,VP6_INSTANCE_CB_RESIZE,(U32)&rResizeParam,NULL,NULL);
          break;
       }
       case VP6_UPDATE_FRAME_GOLDEN:
       {
          if(pbi->goldenFrameYV12Config&&pbi->lastFrameYV12Config!=pbi->goldenFrameYV12Config)
          {
             pbi->goldenFrameYV12Config->used=0;
          }
          pbi->goldenFrameYV12Config=pbi->thisFrameYV12Config;
          pbi->GoldenFrame=pbi->ThisFrameRecon;
          break;
       }
       case VP6_UPDATE_SET_BUFFER:
        {
            U32 index=0;
            U32 UVPlaneSize  = 0;
            U32 YPlaneSize=0;
            for(;index<3;index++)
            {
              if(!pbi->rFrameYV12Config[index].YBuffer)
              { 
                 pFrmCfg=&pbi->rFrameYV12Config[index];
                 break;
              }
            }   
            if(index==3)
            {
              return FALSE;
            }
            
            YPlaneSize     = pFrmCfg->YStride*(pFrmCfg->YHeight+2*UMV_BORDER);
            UVPlaneSize=pFrmCfg->UVStride*(pFrmCfg->UVHeight+UMV_BORDER);
            pFrmCfg->YBuffer = (char *)(u4YAddr+ UMV_BORDER * pFrmCfg->YStride + UMV_BORDER);
            pFrmCfg->UBuffer = (char *)(u4YAddr+ YPlaneSize + UMV_BORDER / 2    * pFrmCfg->UVStride + UMV_BORDER / 2);
            pFrmCfg->VBuffer = (char *)(u4YAddr+ YPlaneSize + UVPlaneSize + UMV_BORDER / 2  * pFrmCfg->UVStride + UMV_BORDER / 2);
            pFrmCfg->BaseBuffer=(char *)u4YAddr;
            break;
        }

        case VP6_UPDATE_SET_INIT:
        {
            pbi->UVDataOffset=u4CAddr-u4YAddr;
            break;
        }
       default:
           break;
     }
#endif

    return TRUE;
}

BOOL _VDEC_Vp6DecUpdateWorkingBuffer(U32 u4Instance,U8 *puWorkBuf)
{
    PB_INSTANCE *pDecInstance;
    S32 i=0,i4Range=0;
    U32 u4MaxHFragments=0,u4MaxMacroBlocks=0,u4Temp=0;
    U8 *pcWorkPtr;
    YV12_BUFFER_CONFIG *pFrmCfg=NULL;
    if(!u4Instance || !puWorkBuf)
    {
       return FALSE;
    }
    
    pDecInstance=(PB_INSTANCE*)u4Instance;
    pDecInstance->Configuration.HFragPixels=8;
    pDecInstance->Configuration.VFragPixels=8;
    
    pDecInstance->YPlaneSize = pDecInstance->Configuration.VideoFrameWidth * pDecInstance->Configuration.VideoFrameHeight; 
    pDecInstance->UVPlaneSize = ( (pDecInstance->Configuration.VideoFrameWidth / 2 + 15) & (~15) ) * pDecInstance->Configuration.VideoFrameHeight;  
    
    pDecInstance->HFragments = pDecInstance->Configuration.VideoFrameWidth / pDecInstance->Configuration.HFragPixels;
    pDecInstance->VFragments = pDecInstance->Configuration.VideoFrameHeight / pDecInstance->Configuration.VFragPixels;
    pDecInstance->UnitFragments = ((pDecInstance->VFragments * pDecInstance->HFragments)*3)/2;
    pDecInstance->YPlaneFragments = pDecInstance->HFragments * pDecInstance->VFragments;
    pDecInstance->UVPlaneFragments = pDecInstance->YPlaneFragments / 4;

    pDecInstance->Configuration.YStride = (pDecInstance->Configuration.VideoFrameWidth + STRIDE_EXTRA);
#ifdef VP6_COMBUV_IN_DECODING
    pDecInstance->Configuration.UVStride =pDecInstance->Configuration.YStride;
#else
    pDecInstance->YDataOffset = 0;
    pDecInstance->UDataOffset = pDecInstance->YPlaneSize;
    pDecInstance->VDataOffset = pDecInstance->YPlaneSize + pDecInstance->UVPlaneSize;
#ifdef VP6_USE_IMGRZ
    pDecInstance->Configuration.UVStride = pDecInstance->Configuration.YStride;
    
#else
    pDecInstance->Configuration.UVStride = ((pDecInstance->Configuration.VideoFrameWidth + STRIDE_EXTRA) / 2 + 15)&(~15);
#endif
    pDecInstance->ReconYDataOffset = 0;
    pDecInstance->ReconUDataOffset = pDecInstance->Configuration.YStride * (pDecInstance->Configuration.VideoFrameHeight + STRIDE_EXTRA)+(UMV_BORDER>>1);
    pDecInstance->ReconVDataOffset = pDecInstance->ReconUDataOffset  + pDecInstance->Configuration.UVStride * ( (pDecInstance->Configuration.VideoFrameHeight + STRIDE_EXTRA ) / 2);
#endif
    // Image dimensions in Macro-Blocks
    pDecInstance->MBRows  = (2*BORDER_MBS)+(pDecInstance->Configuration.VideoFrameHeight/16)  + ( pDecInstance->Configuration.VideoFrameHeight%16 ? 1 : 0 );
    pDecInstance->MBCols  = (2*BORDER_MBS)+(pDecInstance->Configuration.VideoFrameWidth/16)  + ( pDecInstance->Configuration.VideoFrameWidth%16 ? 1 : 0 );
    pDecInstance->MacroBlocks = pDecInstance->MBRows * pDecInstance->MBCols;

    pcWorkPtr=puWorkBuf;
    BufAlign32(pcWorkPtr);
    pDecInstance->ReconDataBuffer[0]=(S16 *)pcWorkPtr;
    pDecInstance->ReconDataBuffer[1] = pDecInstance->ReconDataBuffer[0] + 64;
    pDecInstance->ReconDataBuffer[2] = pDecInstance->ReconDataBuffer[1] + 64;
    pDecInstance->ReconDataBuffer[3] = pDecInstance->ReconDataBuffer[2] + 64;
    pDecInstance->ReconDataBuffer[4] = pDecInstance->ReconDataBuffer[3] + 64;
    pDecInstance->ReconDataBuffer[5] = pDecInstance->ReconDataBuffer[4] + 64;
    pcWorkPtr+=768;
    pDecInstance->TmpDataBuffer=(U8 *)pcWorkPtr;
    pcWorkPtr+=128;
    pDecInstance->LoopFilteredBlock=(U8 *)pcWorkPtr;
    pcWorkPtr+=256;
    pDecInstance->TmpReconBuffer=(S16 *)pcWorkPtr; 
    pcWorkPtr+=128;
    
    for(i=0; i<Q_TABLE_SIZE; i++ )
    {
      pDecInstance->vp6Lib.VP6_DCQuantScaleP[i]=VP6_DcQuant[i]>>1 + 2;
    }
    
    pDecInstance->vp6Lib.VP6_DCQuantScaleV2=pDecInstance->vp6Lib.VP6_DCQuantScaleUV=pDecInstance->vp6Lib.VP6_DCQuantScaleV1=pDecInstance->vp6Lib.VP6_DCQuantScaleP;
    
    i4Range=VAL_RANGE*3;
    for ( i=0; i<i4Range; i++ ) 
    {       
        pDecInstance->vp6Lib.LimitVal_VP6[i] = Clamp255(i-VAL_RANGE);
    }
    
    pDecInstance->vp6Lib.mTemp=1;

    u4Temp=pDecInstance->MBCols;
    for(i=0;i<12;i++ )
    {
        pDecInstance->mvNearOffset[i] = MBOffset(u4Temp,NearMacroBlocks[i].Row, NearMacroBlocks[i].Col);
    }

    pDecInstance->tquantizer.dequant_coeffs[0]=(Q_LIST_ENTRY * )pcWorkPtr;
    pDecInstance->tquantizer.dequant_coeffs[1]=(Q_LIST_ENTRY * )(pcWorkPtr+128);
    pcWorkPtr+=256;
    pDecInstance->tquantizer.LastFrameQIndex=0xFFFFFFFF;
    pDecInstance->quantizer=&pDecInstance->tquantizer;
    pDecInstance->mbi.coeffsBasePtr = (Q_LIST_ENTRY *) pcWorkPtr;
    pcWorkPtr+=768;
    pDecInstance->mbi.blockDxInfo[0].coeffsPtr = pDecInstance->mbi.coeffsBasePtr;
    pDecInstance->mbi.blockDxInfo[1].coeffsPtr = pDecInstance->mbi.blockDxInfo[0].coeffsPtr + 64;
    pDecInstance->mbi.blockDxInfo[2].coeffsPtr = pDecInstance->mbi.blockDxInfo[1].coeffsPtr + 64;
    pDecInstance->mbi.blockDxInfo[3].coeffsPtr = pDecInstance->mbi.blockDxInfo[2].coeffsPtr + 64;
    pDecInstance->mbi.blockDxInfo[4].coeffsPtr = pDecInstance->mbi.blockDxInfo[3].coeffsPtr + 64;
    pDecInstance->mbi.blockDxInfo[5].coeffsPtr = pDecInstance->mbi.blockDxInfo[4].coeffsPtr + 64;

    u4MaxHFragments=pDecInstance->HFragments;
    u4MaxMacroBlocks=pDecInstance->MacroBlocks;
    
    u4Temp=u4MaxHFragments*sizeof(BLOCK_CONTEXT);
    pDecInstance->fc.AboveY=(BLOCK_CONTEXT *)pcWorkPtr;
    pcWorkPtr+=((8+u4MaxHFragments)*sizeof(BLOCK_CONTEXT));; //(8+u4MaxHFragments)*sizeof(BLOCK_CONTEXT)
    BufAlign32(pcWorkPtr);
    pDecInstance->fc.AboveU=(BLOCK_CONTEXT *)pcWorkPtr;
    pcWorkPtr+=((8+u4MaxHFragments/2)*sizeof(BLOCK_CONTEXT));
    BufAlign32(pcWorkPtr);
    pDecInstance->fc.AboveV=(BLOCK_CONTEXT *)pcWorkPtr;
    pcWorkPtr+=((8+u4MaxHFragments/2)*sizeof(BLOCK_CONTEXT));
    BufAlign32(pcWorkPtr);
    pDecInstance->predictionMode=(char *)pcWorkPtr;
    pcWorkPtr+=u4MaxMacroBlocks;
    BufAlign32(pcWorkPtr);
    pDecInstance->MBMotionVector=(MOTION_VECTOR *)pcWorkPtr;
    pcWorkPtr+=sizeof(MOTION_VECTOR)*u4MaxMacroBlocks;
    BufAlign32(pcWorkPtr);
    
    pFrmCfg=pDecInstance->rFrameYV12Config;
    pFrmCfg->YWidth=pDecInstance->Configuration.VideoFrameWidth;
    pFrmCfg->YHeight=pDecInstance->Configuration.VideoFrameHeight;
    pFrmCfg->YBuffer=NULL;
#ifdef VP6_COMBUV_IN_DECODING
    pFrmCfg->UVWidth=pFrmCfg->YWidth;
#else
    pFrmCfg->UVWidth=pFrmCfg->YWidth>>1;
#endif

    pFrmCfg->UVHeight=pFrmCfg->YHeight>>1;
    pFrmCfg->YStride=pDecInstance->Configuration.VideoFrameWidth+2*UMV_BORDER;
    pFrmCfg->YStride=(pFrmCfg->YStride+0xf)&(~0xf);
#ifdef VP6_COMBUV_IN_DECODING
    pFrmCfg->UVStride=pFrmCfg->YStride;
#else
#ifdef VP6_USE_IMGRZ
    pFrmCfg->UVStride=pFrmCfg->YStride;
#else
    pFrmCfg->UVStride=pFrmCfg->UVWidth+UMV_BORDER;
    pFrmCfg->UVStride=(pFrmCfg->UVStride+0xf)&(~0xf);

#endif
#endif
    pFrmCfg->border=UMV_BORDER;
    pFrmCfg->used=0;
    FlashMemCpy((void *)(&pDecInstance->rFrameYV12Config[1]),pFrmCfg,sizeof(YV12_BUFFER_CONFIG));
    FlashMemCpy((void *)(&pDecInstance->rFrameYV12Config[2]),pFrmCfg,sizeof(YV12_BUFFER_CONFIG));
#ifdef VP6_USE_IMGRZ
   pFrmCfg=pDecInstance->rFrameYV12Config;
   i=3;
   do
   {
      int YPlaneSize = pFrmCfg->YStride*(pFrmCfg->YHeight+2*UMV_BORDER);
      int UVPlaneSize = pFrmCfg->UVStride*(pFrmCfg->UVHeight+UMV_BORDER);
      int UVOffset=(UMV_BORDER/2)  * pFrmCfg->UVStride + UMV_BORDER;
      pFrmCfg->BaseBuffer=(char *)pcWorkPtr;
      pFrmCfg->YBuffer = (char *)(pcWorkPtr+ UMV_BORDER * pFrmCfg->YStride + UMV_BORDER);
      pFrmCfg->UBuffer = (char *)(pcWorkPtr + YPlaneSize + UVOffset);
      pFrmCfg->VBuffer = (char *)(pcWorkPtr+ YPlaneSize + UVPlaneSize + UVOffset);
      pcWorkPtr+=(YPlaneSize+UVPlaneSize*2 +pFrmCfg->YStride);
      pFrmCfg++;
      BufAlign32(pcWorkPtr);
   }while(--i);

   pDecInstance->u4RzWorkBuf=(U32)pcWorkPtr;
   pcWorkPtr+=IMGRZ_WORKING_BUF;
   BufAlign32(pcWorkPtr);
#endif

    pDecInstance->pcWrokPtr=pcWorkPtr;
    VP6_InitMBI(pDecInstance);
    return TRUE;
}


