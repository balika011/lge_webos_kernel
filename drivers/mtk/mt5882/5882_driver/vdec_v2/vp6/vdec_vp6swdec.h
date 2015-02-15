
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
 * $RCSfile: vdec_vp6.h,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_vp6.h
 *  This header file declares exported APIs of MPV.
 */

#ifndef VDEC_VP6_SWDEC_H
#define VDEC_VP6_SWDEC_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------


typedef unsigned long   U32, *P_U32, **PP_U32;
typedef signed long     S32, *P_S32, **PP_S32;
typedef unsigned short  U16, *P_U16, **PP_U16;
typedef signed short  S16, *P_S16, **PP_S16;
typedef unsigned char U8, *P_U8, **PP_U8;
typedef signed char   S8, *P_S8, **PP_S8;
typedef unsigned char  BYTE,bool;
typedef unsigned short int  WORD;
typedef long unsigned int  DWORD;
typedef unsigned int   size_t;
typedef bool BOOL;

#if !defined(TRUE)
    #define TRUE 1
#endif 
    
#if !defined(FALSE)
    #define FALSE 0
#endif 

#if !defined(NULL)
    #define NULL    0
#endif 

typedef bool BOOL;

/************************************************************************/
/************************************************************************/
/************************************************************************/
#define VP6_SWDEC_MTKOPTIMIZE

#define VAL_RANGE               256
#define BASE_FRAME              0
#define NORMAL_FRAME            1
#define Q_TABLE_SIZE            64
#define BLOCK_HEIGHT_WIDTH      8
#define BLOCK_SIZE              (64)
#define LIMIT(x)                ( (x) < 0 ? 0: (x) > 255 ? 255 : (x) )
#define FUNC(a,b) (U8)(((255*(a))/(1+(b)))+1)
// VP6 hufman table AC bands
#define VP6_AC_BANDS            6

// Tokens                                Value        Extra Bits (range + sign)
#define ZERO_TOKEN              0        //0            Extra Bits 0+0
#define ONE_TOKEN               1        //1            Extra Bits 0+1       
#define TWO_TOKEN               2        //2            Extra Bits 0+1 
#define THREE_TOKEN             3        //3            Extra Bits 0+1
#define FOUR_TOKEN              4        //4            Extra Bits 0+1
#define DCT_VAL_CATEGORY1        5        //5-6        Extra Bits 1+1
#define DCT_VAL_CATEGORY2        6        //7-10        Extra Bits 2+1
#define DCT_VAL_CATEGORY3        7        //11-26        Extra Bits 4+1
#define DCT_VAL_CATEGORY4        8        //11-26        Extra Bits 5+1
#define DCT_VAL_CATEGORY5        9        //27-58        Extra Bits 5+1
#define DCT_VAL_CATEGORY6        10        //59+        Extra Bits 11+1    
#define DCT_EOB_TOKEN           11        //EOB        Extra Bits 0+0
#define MAX_ENTROPY_TOKENS      12 //(DCT_EOB_TOKEN + 1)  
#define ILLEGAL_TOKEN            255

#define DC_TOKEN_CONTEXTS        3 // 00, 0!0, !0!0
#define CONTEXT_NODES            5 //(MAX_ENTROPY_TOKENS-7)

#define PREC_CASES                3
#define ZERO_RUN_PROB_CASES     14 

#define DC_PROBABILITY_UPDATE_THRESH    100

#define ZERO_CONTEXT_NODE        0
#define EOB_CONTEXT_NODE        1
#define ONE_CONTEXT_NODE        2
#define LOW_VAL_CONTEXT_NODE    3
#define TWO_CONTEXT_NODE        4
#define THREE_CONTEXT_NODE        5
#define HIGH_LOW_CONTEXT_NODE    6
#define CAT_ONE_CONTEXT_NODE    7
#define CAT_THREEFOUR_CONTEXT_NODE    8
#define CAT_THREE_CONTEXT_NODE    9
#define CAT_FIVE_CONTEXT_NODE    10

#define PROB_UPDATE_BASELINE_COST    7

#define MAX_PROB                254
#define DCT_MAX_VALUE            2048

#define ZRL_BANDS                2
#define ZRL_BAND2                6

#define SCAN_ORDER_BANDS        16
#define SCAN_BAND_UPDATE_BITS   4


#define CURRENT_ENCODE_VERSION  8
#define CURRENT_DECODE_VERSION  8

#define SIMPLE_PROFILE            0
#define PROFILE_1                1
#define PROFILE_2                2
#define ADVANCED_PROFILE        3

// Loop filter options
#define NO_LOOP_FILTER            0
#define LOOP_FILTER_BASIC        2
#define LOOP_FILTER_DERING        3

#define UMV_BORDER              48
#define STRIDE_EXTRA            (96)//(UMV_BORDER<<1)
#define BORDER_MBS                (3)//(UMV_BORDER>>4)
#define HALF_BORDER   24

#define MAX_MV_EXTENT           63      //  Max search distance in half pixel increments
#define MV_ENTROPY_TOKENS       511     
#define LONG_MV_BITS            8

#define PPROC_QTHRESH           64

#define MAX_MODES               10

#define MAX_NEAREST_ADJ_INDEX    2 

#define Y_MVSHIFT                0x2 
#define UV_MVSHIFT                0x3
#define Y_MVMODMASK                0x3
#define UV_MVMODMASK            0x7

// Prediction filter modes:
// Note: when trying to use an enum here we ran into an odd compiler bug in
// the WriteFrameHeader() code. Also an enum type is implicitly an int which 
// is a bit big for something that can only have 3 values
#define BILINEAR_ONLY_PM        0
#define BICUBIC_ONLY_PM            1
#define AUTO_SELECT_PM            2

#define IDCT_SCALE_FACTOR       2       // Shift left bits to improve IDCT precision

#define MODETYPES       3
#define MODEVECTORS     16
#define PROBVECTORXMIT  174
#define PROBIDEALXMIT   254

#define HUFF_LUT_LEVELS 6

#define VP6_Y_COMPONENT 0
#define VP6_U_COMPONENT 1
#define VP6_V_COMPONENT 2
#define VP6_U_4MASK 0x00ff00ff
#define VP6_V_4MASK 0xff00ff00
#define VP6_U_2MASK 0xff00
#define VP6_V_2MASK 0x00ff
// IDCT
#define IdctAdjustBeforeShift 8
#define xC1S7 64277
#define xC2S6 60547
#define xC3S5 54491
#define xC4S4 46341
#define xC5S3 36410
#define xC6S2 25080
#define xC7S1 12785

#define IMGRZ_WORKING_BUF (768*12)
#define DECODE_BOOL(a,b) VP6_DecodeBool((a),(b))
#define DECODE_BOOL128(a) VP6_DecodeBool128((a))

#define Clamp255(x)    (unsigned char) ( (x) < 0 ? 0 : ( (x) <= 255 ? (x) : 255 ) )
#define BufAlign16(ptr) (ptr+=(16-(ptr&0f)))
#define BufAlign32(ptr) (ptr+=(32-((((U32)ptr)&0x1f))))
#define HIGHBITDUPPED(X) (((signed short) X)  >> 15)

#define DCProbOffset(A,B) \
    ( (A) * (MAX_ENTROPY_TOKENS-1) \
    + (B) )

#define ACProbOffset(A,B,C,D) \
    ( (A) * PREC_CASES * VP6_AC_BANDS * (MAX_ENTROPY_TOKENS-1) \
    + (B) * VP6_AC_BANDS * (MAX_ENTROPY_TOKENS-1) \
    + (C) * (MAX_ENTROPY_TOKENS-1) \
    + (D) ) 

#define DcNodeOffset(A,B,C) \
    ( (A) * DC_TOKEN_CONTEXTS * CONTEXT_NODES \
    + (B) * CONTEXT_NODES \
    + (C) ) 

#define MBOffset(mbcols,row,col) ( (row) * (mbcols) + (col) )


#define BitsAreBigEndian 1
#if BitsAreBigEndian
#define NextWord(a) \
    {    if (br->position < br->end) (a)=(*br->position++<<24); \
        if (br->position < br->end) (a)+=(*br->position++<<16); \
        if (br->position < br->end) (a)+=(*br->position++<<8); \
        if (br->position < br->end) (a)+=(*br->position++); \
    }

#else
#define NextWord(a) \
    {    if (br->position < br->end) (a)=(*br->position++); \
        if (br->position < br->end) (a)+=(*br->position++<<8); \
        if (br->position < br->end) (a)+=(*br->position++<<16); \
        if (br->position < br->end) (a)+=(*br->position++<<24); \
    }
#endif

#define LIMIT_POSITIVE_1(x) ((x) > 0xFF ? 0xFF : (x))
#define LIMIT_POSITIVE_2(x) ((x) > 0xFF0000 ? 0xFF0000 : (x))

#define FILTER_WEIGHT 128
#define FILTER_SHIFT  7
#define Mod8(x) ( (x) & 7 )
#define abs(x) ( (x>0) ? (x) : (-(x)) )

#define IDCT_CLAMP0(value) (value <0 ? 0 : value)
#define IDCT_CLAMP255(value) (value >255 ? 255 : value)

typedef enum
{
    CODE_INTER_NO_MV        = 0x0,      // INTER prediction, (0,0) motion vector implied.
    CODE_INTRA              = 0x1,      // INTRA i.e. no prediction.
    CODE_INTER_PLUS_MV      = 0x2,      // INTER prediction, non zero motion vector.
    CODE_INTER_NEAREST_MV   = 0x3,      // Use Last Motion vector
    CODE_INTER_NEAR_MV      = 0x4,      // Prior last motion vector
    CODE_USING_GOLDEN       = 0x5,      // 'Golden frame' prediction (no MV).
    CODE_GOLDEN_MV          = 0x6,      // 'Golden frame' prediction plus MV.
    CODE_INTER_FOURMV       = 0x7,      // Inter prediction 4MV per macro block.
    CODE_GOLD_NEAREST_MV    = 0x8,      // Use Last Motion vector
    CODE_GOLD_NEAR_MV       = 0x9,      // Prior last motion vector
    DO_NOT_CODE             = 0x10       // Fake Mode
} CODING_MODE;

#define MODETYPES       3
#define MODEVECTORS     16
#define PROBVECTORXMIT  174
#define PROBIDEALXMIT   254
      
typedef struct _modeContext
{
    U8 left;
    U8 above;
    U8 last;
} MODE_CONTEXT;

typedef struct _htorp
{
    unsigned char selector : 1;   // 1 bit selector 0->ptr, 1->token
    unsigned char value : 7;
} torp;

typedef struct _hnode
{
    torp left;
    torp right;
} HNODE;

typedef enum _MODETYPE 
{
    MACROBLOCK,
    NONEAREST_MACROBLOCK,
    NONEAR_MACROBLOCK,
    BLOCK,
} MODETYPE;

typedef struct _SORT_NODE
{
    int next;
    int freq;
    unsigned char value;
} SORT_NODE;

typedef struct _sortnode
{
    int next;
    int freq;
    unsigned short value;
} sortnode;

typedef struct
{
    U16  MinVal;
    S16   Length;
    U8   Probs[11];
} TOKENEXTRABITS;

typedef struct _tokenorptr
{
    unsigned short selector : 1;   // 1 bit selector 0->ptr, 1->token
    unsigned short value : 7;
} tokenorptr;

typedef struct _ehuffnode
{
    union
    {
        char l;
        tokenorptr left;
    } leftunion; 
    union
    {
        char r;
        tokenorptr right;
    } rightunion; 
} E_HUFF_NODE;

/* HUFF_NODE for decoder */
typedef struct _HUFF_NODE
{
    unsigned short left; // 1 bit tells whether its a pointer or value
    unsigned short right;// 1 bit tells whether its a pointer or value
} HUFF_NODE;

typedef struct HUFF_TALBE_NODE
{
    unsigned short flag     :1;      // bit 0: 1-Token, 0-Index
    unsigned short value    :5;      // value: the value of the Token or the Index to the huffman tree
    unsigned short unused   :6;      // not used for now
    unsigned short length   :4;      // Huffman code length of the token
} HUFF_TABLE_NODE;

struct BOOL_CODER
{
    unsigned int lowvalue;
    unsigned int range;
    unsigned int value;
    int count;
    unsigned int pos;
    unsigned char *buffer;     
    unsigned char *buffer_end;
    // Variables used to track bit costs without outputing to the bitstream
    unsigned int  MeasureCost;
    unsigned long BitCounter;
};
typedef struct BOOL_CODER BOOL_CODER;

typedef struct LineEq
{
    S32    M;
    S32    C;
} LINE_EQ;

typedef struct YV12_BUFFER_CONFIG
{
    int   YWidth;
    int   YHeight;
    int   YStride;
//    int   YInternalWidth;

    int   UVWidth;
    int   UVHeight;
    int   UVStride;
//    int   UVInternalWidth;

    char *YBuffer;
#ifdef VP6_COMBUV_IN_DECODING
    char *UVBuffer;
#else
    char *UBuffer;
    char *VBuffer;
#ifdef VP6_USE_IMGRZ
    char *BaseBuffer;
#endif
#endif
    int border;
    int used;
} YV12_BUFFER_CONFIG;


typedef struct VP6_POSTPROC_INSTANCE
{
    // per frame information passed in
    S32         PostProcessingLevel;    // level of post processing to perform 
    S32         FrameQIndex;            // q index value used on passed in frame

    // per block info maintained by postprocessor
    S32        *FragmentVariances;        // block's pseudo variance : allocated and filled

    // filter specific vars
    S32        *BoundingValuePtr;        // pointer to a filter     
    S32        *FiltBoundingValue;        // allocated (512 big)

    // deblocker specific vars
    S32        *DeblockValuePtr;        // pointer to a filter     
    S32        *DeblockBoundingValue;    // allocated (512 big)
    
    U32         UnitFragments;            // number of total fragments y+u+v 

} VP6_POSTPROC_INSTANCE;
typedef S16           Q_LIST_ENTRY;
typedef Q_LIST_ENTRY    Q_LIST[64];

typedef struct _DCINFO
{
    Q_LIST_ENTRY dc;
    short frame;
} DCINFO;



typedef struct CONFIG_TYPE
{
    // The size of the surface we want to draw to
    U32 VideoFrameWidth;
    U32 VideoFrameHeight;

    S32 YStride;
    S32 UVStride;

    // The number of horizontal and vertical blocks encoded
    U32 HFragPixels;
    U32 VFragPixels;

} CONFIG_TYPE;

typedef struct MOTION_VECTOR
{
    S16   x;
    S16   y;
} MOTION_VECTOR;

typedef MOTION_VECTOR COORDINATE;
typedef U8 YUV_BUFFER_ENTRY;
typedef U8 *YUV_BUFFER_ENTRY_PTR;


// YUV buffer configuration structure
typedef struct YUV_BUFFER_CONFIG
{
    int     YWidth;
    int     YHeight;
    int     YStride;

    int     UVWidth;
    int     UVHeight;
    int     UVStride;

    char *  YBuffer;
    char *  UBuffer;
    char *  VBuffer;

} YUV_BUFFER_CONFIG;


// defined so i don't have to remember which block goes where
typedef enum
{
    TOP_LEFT_Y_BLOCK        = 0,
    TOP_RIGHT_Y_BLOCK       = 1,
    BOTTOM_LEFT_Y_BLOCK     = 2,
    BOTTOM_RIGHT_Y_BLOCK    = 3,
    U_BLOCK                 = 4,
    V_BLOCK                 = 5
} BLOCK_POSITION;

// all the information gathered from a block to be used as context in the next block
typedef struct BLOCK_CONTEXT
{
    U8        Token;
    CODING_MODE  Mode;
    U16       Frame;
    Q_LIST_ENTRY Dc;
    U8        unused[3];
}  BLOCK_CONTEXT;

// all the contexts maintained for a frame
typedef struct FRAME_CONTEXT
{
    BLOCK_CONTEXT    LeftY[2];   // 1 for each block row in a macroblock
    BLOCK_CONTEXT    LeftU;
    BLOCK_CONTEXT    LeftV;

    BLOCK_CONTEXT   *AboveY;
    BLOCK_CONTEXT   *AboveU;
    BLOCK_CONTEXT   *AboveV;

    Q_LIST_ENTRY     LastDcY[4]; // 1 for each frame 
    Q_LIST_ENTRY     LastDcU[4];
    Q_LIST_ENTRY     LastDcV[4];

} FRAME_CONTEXT;

typedef struct BLOCK_DX_INFO
{
    S16 *dequantPtr;
    S16 *coeffsPtr;
    S8 *reconPtr;

    S32  MvShift;                 // motion vector shift value
    S32  MvModMask;               // motion vector mod mask

    S32  FrameReconStride;        // Stride of the frame
    S32  CurrentReconStride;      // pitch of reconstruction

    S32  CurrentSourceStride;     // pitch of source (compressor only)
    S32  FrameSourceStride;        // Stride of the frame (compressor only)
    U32 Plane;                   // plane block is from (compressor only)

    BLOCK_CONTEXT  *Above;          // above block context
    BLOCK_CONTEXT  *Left;           // left block context
    Q_LIST_ENTRY   *LastDc;         // last dc value seen

    U32 thisRecon;               // index for recon
    U32 Source;                  // index for source (compressor only)

    U32 EobPos;

    U8 *BaselineProbsPtr;
    U8 *ContextProbsPtr;

    U8  *AcProbsBasePtr; 
    U8  *DcProbsBasePtr; 
    U8  *DcNodeContextsBasePtr; 
    U8  *ZeroRunProbsBasePtr;

    CODING_MODE   BlockMode;
    MOTION_VECTOR Mv;

}BLOCK_DX_INFO;


typedef struct MACROBLOCK_INFO
{
    BOOL_CODER *br;

    BLOCK_DX_INFO blockDxInfo[6];

    CODING_MODE   Mode;             // mode macroblock coded as

    S32         NearestGMvIndex;  // Indicates how neare nearest is.
    MOTION_VECTOR NearestInterMVect;// nearest mv in last frame
    MOTION_VECTOR NearInterMVect;   // near mv in last frame
    S32         NearestMvIndex;   // Indicates how neare nearest is.
    MOTION_VECTOR NearestGoldMVect; // nearest mv in gold frame
    MOTION_VECTOR NearGoldMVect;    // near mv in gold frame

    S16 *coeffsBasePtr;

} MACROBLOCK_INFO;

// Frame Header type
typedef struct FRAME_HEADER
{
    U32 buffer_size;
    U32 value;
    S32  bits_available;
    U32 pos;
    U8 *buffer;
    U8 *buffer_end;
} FRAME_HEADER;

typedef struct _BITREADER
{
    int bitsinremainder; // # of bits still used in remainder
    U32 remainder;   // remaining bits from original long
    const unsigned char * position;  // character pointer position within data
    unsigned char *end;   // points to 1 byte past the end of the data buffer
} BITREADER;

typedef struct QUANTIZER
{
    U32 FrameQIndex;    // Quality specified as a table index 
    U32 LastFrameQIndex;    
    short round[8];
    short mult[8];
    short zbin[8];

    U32 *transIndex;                       // array to reorder zig zag to idct's ordering
    U8   quant_index[64];               // array to reorder from raster to zig zag

    // used by the dequantizer 
    Q_LIST_ENTRY * dequant_coeffs[2];       // pointer to current dequantization tables

    S32 QuantCoeffs[2][64];               // Quantizer values table
    S32 QuantRound[2][64];               // Quantizer rounding table
    S32 ZeroBinSize[2][64];               // Quantizer zero bin table
    S32 ZlrZbinCorrections[2][64];       // Zbin corrections based upon zero run length.

} QUANTIZER;

typedef struct
{
  U8 *pUBuffer;
  U8 *pVBuffer;
  U32 u4Used;
}UV_BUF;

typedef struct 
{

    U32 *VP6_DCQuantScaleV2;
    U32 *VP6_DCQuantScaleUV;
    U32 *VP6_DCQuantScaleV1;
    unsigned int CPUFrequency;      // Process Frequency
    U8 LimitVal_VP6[VAL_RANGE * 3];
    S32 decTablesAllocated;
    void *mcbpcIntraVlc;
    void *mcbpcInterVlc;
    void *cbpyVlc;
    void *mvVlc;
    void *tcoefVlc;
    S32 fData[BLOCK_HEIGHT_WIDTH*11];
    U32 VP6_DCQuantScaleP[Q_TABLE_SIZE];
    U32 mTemp;
    BOOL memAllocFailure;
}CVP6Lib;


typedef struct PB_INSTANCE
{
    MACROBLOCK_INFO  mbi;        // all the information needed for one macroblock
    FRAME_CONTEXT    fc;        // all of the context information needed for a frame
    QUANTIZER        tquantizer;
    QUANTIZER        *quantizer;

        // Should be able to delete these entries when VP5 complete
    S32      CodedBlockIndex;           
    U8      *DataOutputInPtr;          

    /* Current access points fopr input and output buffers */
    BOOL_CODER br;
    BOOL_CODER br2;
    BITREADER  br3;

    // Decoder and Frame Type Information
    U8   Vp3VersionNo;
    U8    VpProfile;

    U32  PostProcessingLevel;       /* Perform post processing */
    U32  ProcessorFrequency;       /* CPU frequency    */
    U32  CPUFree;
    U8   FrameType;       

    CONFIG_TYPE Configuration;    // frame configuration
    U32  CurrentFrameSize;

    U32  YPlaneSize;  
    U32  UVPlaneSize;  

    U32  VFragments;
    U32  HFragments;
    U32  UnitFragments;
    U32  YPlaneFragments;
    U32  UVPlaneFragments;
#ifndef VP6_COMBUV_IN_DECODING
    U32  YDataOffset;
    U32  UDataOffset;
    U32  VDataOffset;
    U32  ReconYDataOffset;
    U32  ReconUDataOffset;
    U32  ReconVDataOffset;
#endif
    U32 UVDataOffset;
    // these are for motion vector sanity checks
     S32   MacroblockOverscanLeftBoundary;
     S32   MacroblockOverscanRightBoundary;
     S32   MacroblockOverscanTopBoundary;
     S32   MacroblockOverscanBottomBoundary;

    U32  MacroBlocks;    // Number of Macro-Blocks in Y component
    U32  MBRows;            // Number of rows of MacroBlocks in a Y frame
    U32  MBCols;            // Number of cols of MacroBlocks in a Y frame

    U32    OutputWidth;
    U32    OutputHeight;
    
    // Frame Buffers 
    YUV_BUFFER_ENTRY *ThisFrameRecon;
    YUV_BUFFER_ENTRY *GoldenFrame; 
    YUV_BUFFER_ENTRY *LastFrameRecon;

    Q_LIST_ENTRY *quantized_list;  
    S16 *ReconDataBuffer[6];
    S16 *TmpReconBuffer;

    U8  *TmpDataBuffer;
    U8  *LoopFilteredBlock;
    VP6_POSTPROC_INSTANCE *    postproc;
    CODING_MODE      LastMode;      // Last Mode decoded;
    U8 DcProbs[2*(MAX_ENTROPY_TOKENS-1)];
    U8 AcProbs[2*PREC_CASES*VP6_AC_BANDS*(MAX_ENTROPY_TOKENS-1)];

    U8 DcNodeContexts[2 * DC_TOKEN_CONTEXTS * CONTEXT_NODES]; // Plane, Contexts, Node
    
    U8 ZeroRunProbs[ZRL_BANDS][ZERO_RUN_PROB_CASES];

    U8 MergedScanOrder[BLOCK_SIZE + 65];
    U8 ModifiedScanOrder[BLOCK_SIZE];
    U8 EobOffsetTable[BLOCK_SIZE];
    U8 ScanBands[BLOCK_SIZE];

    U8  PredictionFilterMode;
    U8  PredictionFilterMvSizeThresh;
    U32 PredictionFilterVarThresh;
    U8  PredictionFilterAlpha;
    
    BOOL   RefreshGoldenFrame;

    U32 AvgFrameQIndex;
    
    U32 mvNearOffset[16];
    
    char *predictionMode;
    MOTION_VECTOR *MBMotionVector;

    U8  MvSignProbs[2];
    U8  IsMvShortProb[2];
    U8  MvShortProbs[2][7];
    U8  MvSizeProbs[2][LONG_MV_BITS];

    U8 probXmitted[4][2][MAX_MODES];
    U8 probModeSame[4][MAX_MODES];
    U8 probMode[4][MAX_MODES][MAX_MODES-1]; // nearest+near,nearest only, nonearest+nonear, 10 preceding modes, 9 nodes

    U32 maxTimePerFrame;
    U32 thisDecodeTime;
    U32 avgDecodeTime;
    U32 avgPPTime[5];
    U32 avgBlitTime;

    // Does this frame use multiple data streams
    // Multistream is implicit for SIMPLE_PROFILE
    BOOL   MultiStream;

    // Huffman code tables for DC, AC & Zero Run Length
    U32 DcHuffCode[2][MAX_ENTROPY_TOKENS];
    U8  DcHuffLength[2][MAX_ENTROPY_TOKENS];
    U32 DcHuffProbs[2][MAX_ENTROPY_TOKENS];
    HUFF_NODE DcHuffTree[2][MAX_ENTROPY_TOKENS];

    U32 AcHuffCode[PREC_CASES][2][VP6_AC_BANDS][MAX_ENTROPY_TOKENS];
    U8  AcHuffLength[PREC_CASES][2][VP6_AC_BANDS][MAX_ENTROPY_TOKENS];
    U32 AcHuffProbs[PREC_CASES][2][VP6_AC_BANDS][MAX_ENTROPY_TOKENS];
    HUFF_NODE AcHuffTree[PREC_CASES][2][VP6_AC_BANDS][MAX_ENTROPY_TOKENS];

    U32 ZeroHuffCode[ZRL_BANDS][ZERO_RUN_PROB_CASES];
    U8  ZeroHuffLength[ZRL_BANDS][ZERO_RUN_PROB_CASES];
    U32 ZeroHuffProbs[ZRL_BANDS][ZERO_RUN_PROB_CASES];
    HUFF_NODE ZeroHuffTree[ZRL_BANDS][ZERO_RUN_PROB_CASES];

    /* FAST look-up-table for huffman Trees */
    U16 DcHuffLUT[2][1<<HUFF_LUT_LEVELS];
    U16 AcHuffLUT[PREC_CASES][2][VP6_AC_BANDS][1<<HUFF_LUT_LEVELS];
    U16 ZeroHuffLUT[ZRL_BANDS][1<<HUFF_LUT_LEVELS];

    // Second partition buffer details
    FRAME_HEADER Header;
    U32 Buff2Offset;

    // Note: Use of huffman codes for DCT data is only allowed 
    // when using multiple data streams / partitions
    BOOL   UseHuffman;    
#ifdef VP6_USE_IMGRZ
    U32 u4RzWorkBuf;
#endif
    BOOL   *pfgRefreshGd;
    U32  *pu4ReadPos;
    BOOL  *pfgIsKeyFrame;
    // Counters for runs of zeros at DC & EOB at first AC position in Huffman mode
    S32  CurrentDcRunLen[2];
    S32  CurrentAc1RunLen[2];
    S32 i4DecId;
    U8 *pcWrokPtr;
    VP6_INSTANCE_CB psrCb;
    // Should we do loop filtering.
    // In simple profile this is ignored and there is no loop filtering    
    U8  UseLoopFilter;
    
    // Do not process U and V channels during decode
    U8  NoUVProcessing;

    YV12_BUFFER_CONFIG rFrameYV12Config[3];
    YV12_BUFFER_CONFIG *lastFrameYV12Config;
    YV12_BUFFER_CONFIG *thisFrameYV12Config;
    YV12_BUFFER_CONFIG *goldenFrameYV12Config;
    
    CVP6Lib vp6Lib;
} PB_INSTANCE;

#endif
