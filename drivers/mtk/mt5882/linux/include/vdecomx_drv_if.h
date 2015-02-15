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
/*
*
* $Author: p4admin $
* $Modtime: $
* $Revision: #1 $
*
*/

// *********************************************************************
// Memo
// *********************************************************************
/*
*/

#ifndef _VDECOMX_MOD_H_
#define _VDECOMX_MOD_H_


#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 *                              Definition
 *===========================================================================*/

#define VDECOMX_LOG_ERROR    (1<<0)
#define VDECOMX_LOG_WARNING  (1<<1)
#define VDECOMX_LOG_DEBUG    (1<<2)
#define VDECOMX_LOG_INFO     (1<<3)
#define VDECOMX_LOG_VERBOS   (1<<4)

#define VDECOMX_VFIFO_SIZE (0xCFFD20)    // smaller than (FEEDER BUFFER SIZE/2) => 1 instance, each has 2 buffers
#define VDECOMX_MAX_BUFFER_SIZE (21)
#define VDECOMX_MAX_DECODER (4)
#define VDECOMX_INPUT_BUF_MAX  (20)

typedef enum {
    VDEXOMX_UNKNOWN,
    
    VDEXOMX_CREATE      = _IOWR('c', 0, long),
    VDEXOMX_INIT        = _IOWR('c', 1, long),
    VDEXOMX_DECODE      = _IOWR('c', 2, long),
    VDEXOMX_GETPARAM    = _IOWR('c', 3, long),
    VDEXOMX_SETPARAM    = _IOWR('c', 4, long),
    VDEXOMX_DEINIT      = _IOWR('c', 5, long),
    VDEXOMX_RELEASE     = _IOWR('c', 6, long),
    VDEXOMX_GETLOGMASK  = _IOWR('c', 7, long),
    VDEXOMX_COMMAND_MAX = _IOWR('c', 53, long)
} VDEXOMX_COMMAND;


typedef enum
{
    VDECOMX_GET_TYPE_QUERY_REF_POOL_SIZE,      ///< how many buffer size of the reference pool needs in driver
    VDECOMX_GET_TYPE_QUERY_DISP_POOL_SIZE,     ///< how many buffer size of the display pool needs in driver
    VDECOMX_GET_TYPE_DISP_FRAME_BUFFER,        ///< return a P_VDECOMX_FRAMEBUF_T address (especially in display order != decode order)
    VDECOMX_GET_TYPE_FREE_FRAME_BUFFER,        ///< return a frame didn't be a reference more (when buffer_mode = REF_IS_DISP_EXT, REF_INT_DISP_EXT or REF_INT_POOL_DISP_EXT)
    VDECOMX_GET_TYPE_GET_PICTURE_INFO,         ///< return a pointer address point to P_VDECOMX_PICINFO_T
    VDECOMX_GET_TYPE_GET_STATISTIC_INFO,       ///< return statistic information.
    VDECOMX_GET_TYPE_GET_FRAME_MODE,           ///< return frame mode parameter.
    VDECOMX_GET_TYPE_GET_FRAME_CROP_INFO,      ///< return frame crop information.
    VDECOMX_GET_TYPE_QUERY_REORDER_ABILITY,    ///< query if driver can re-order the decode order to display order
    VDECOMX_GET_TYPE_QUERY_DOWNSIZE_ABILITY,   ///< query if driver can downsize decoded frame
    VDECOMX_GET_TYPE_QUERY_RESIZE_ABILITY,     ///< query if driver can resize decoded frame
    VDECOMX_GET_TYPE_QUERY_DEBLOCK_ABILITY,    ///< query if driver can do deblocking
    VDECOMX_GET_TYPE_QUERY_DEINTERLACE_ABILITY,///< query if driver can do deinterlace
    VDECOMX_GET_TYPE_QUERY_DROPFRAME_ABILITY,  ///< query if driver can drop frame
    VDECOMX_GET_TYPE_GET_DECODE_STATUS_INFO,   ///< query if driver finish decode one frame but no output (main profile with B frame case.)
    VDECOMX_GET_TYPE_GET_PIXEL_FORMAT,          ///< query the driver output pixel format
    VDECOMX_GET_TYPE_GET_CPU_LOADING_INFO,       ///< query the cpu loading info from kernel driver
    VDECOMX_GET_TYPE_NEED_FRAME_BUFFER,         ///< query the decoder need more output buffer.
    VDECOMX_GET_TYPE_GET_SEQUENCE_INFO,
    VDECOMX_GET_TYPE_ALLOCATE_BUFFER,           ///< allocate buffer from feeder.
    VDECOMX_GET_TYPE_QUERY_FEEDER_INFO,          //< allocate single input buffer size from feeder.
    VDECOMX_GET_TYPE_GET_KMEM_HANDLER,          // < allocate a kmem handler for a kernal space address.    
    VDECOMX_GET_TYPE_QUERY_HW_ABILITY,                // < query if hw vdec is enough to play
    VDECOMX_GET_TYPE_GET_VDEC_CNT              =64 ///< query the max decoder cnt. 
                                                   //resource manger will use hard code 64 to get vdec cnt when call ioctl
} VDECOMX_GET_TYPE_T;

typedef enum
{
    VDECOMX_VIDEO_FORMAT_UNKNOWN_VIDEO_FORMAT  = 0,       ///< Unknown video format
    VDECOMX_VIDEO_FORMAT_DIVX311               = (1<<0),  ///< Divix 3.11
    VDECOMX_VIDEO_FORMAT_DIVX4                 = (1<<1),  ///< Divix 4
    VDECOMX_VIDEO_FORMAT_DIVX5                 = (1<<2),  ///< Divix 5
    VDECOMX_VIDEO_FORMAT_XVID                  = (1<<3),  ///< Xvid
    VDECOMX_VIDEO_FORMAT_MPEG1                 = (1<<4),  ///< MPEG-1
    VDECOMX_VIDEO_FORMAT_MPEG2                 = (1<<5),  ///< MPEG-2
    VDECOMX_VIDEO_FORMAT_MPEG4                 = (1<<6),  ///< MPEG-4
    VDECOMX_VIDEO_FORMAT_H263                  = (1<<7),  ///< H263
    VDECOMX_VIDEO_FORMAT_H264                  = (1<<8),  ///< H264
    VDECOMX_VIDEO_FORMAT_H265                  = (1<<9),  ///< H265
    VDECOMX_VIDEO_FORMAT_WMV7                  = (1<<10), ///< WMV7
    VDECOMX_VIDEO_FORMAT_WMV8                  = (1<<11), ///< WMV8
    VDECOMX_VIDEO_FORMAT_WMV9                  = (1<<12), ///< WMV9
    VDECOMX_VIDEO_FORMAT_VC1                   = (1<<13), ///< VC1
    VDECOMX_VIDEO_FORMAT_REALVIDEO8            = (1<<14), ///< RV8
    VDECOMX_VIDEO_FORMAT_REALVIDEO9            = (1<<15), ///< RV9
    VDECOMX_VIDEO_FORMAT_VP6                   = (1<<16), ///< VP6
    VDECOMX_VIDEO_FORMAT_VP7                   = (1<<17), ///< VP7
    VDECOMX_VIDEO_FORMAT_VP8                   = (1<<18), ///< VP8
    VDECOMX_VIDEO_FORMAT_AVS                   = (1<<19), ///< AVS
    VDECOMX_VIDEO_FORMAT_MJPEG                 = (1<<20), ///< Motion JPEG
    VDECOMX_VIDEO_FORMAT_S263                  = (1<<21),  ///< Sorenson Spark
    VDECOMX_VIDEO_FORMAT_H264HP                = (1<<22),
    VDECOMX_VIDEO_FORMAT_H264SEC               = (1<<23),
    VDECOMX_VIDEO_FORMAT_VP9                   = (1<<24)  //vp9
} VDECOMX_VIDEO_FORMAT_T;

typedef enum
{
    VDECOMX_DECODE_MODE_UNKNOWN = 0,       ///< Unknown
    VDECOMX_DECODE_MODE_NORMAL,            ///< decode all frames (no drop)
    VDECOMX_DECODE_MODE_I_ONLY,            ///< skip P and B frame 
    VDECOMX_DECODE_MODE_B_SKIP,            ///< skip B frame 
    VDECOMX_DECODE_MODE_DROPFRAME,         ///< display param1 frames & drop param2 frames
    VDECOMX_DECODE_MODE_NO_REORDER         ///< output display ASAP without reroder
} VDECOMX_DECODE_MODE_T;

typedef enum
{
    VDECOMX_NFY_TYPE_FORCE_STOP = 0,
    VDECOMX_NFY_TYPE_INPUT_USED,
    VDECOMX_NFY_TYPE_FRAME_DECODED,
    VDECOMX_NFY_TYPE_MAX
} VDECOMX_NFY_TYPE_T;

typedef struct _VDECOMX_NFY_T
{
    void                *pvhdlr;
    VDECOMX_NFY_TYPE_T  eType;
    unsigned int        u4Data1;
    unsigned int        u4Data2;
    unsigned int        u4Data3;
} VDECOMX_NFY_T;

typedef struct _VDECOMX_SET_DECODE_MODE_T
{
    VDECOMX_DECODE_MODE_T  eDecodeMode;       ///< one of VDECOMX_DECODE_MODE
    unsigned int            u4DisplayFrameNum; ///< 0  8  7  6  5  4  3  2  1  1  1  1  1  1  1  1 
    unsigned int            u4DropFrameNum;    ///< 0  1  1  1  1  1  1  1  1  2  3  4  5  6  7  8
} VDECOMX_SET_DECODE_MODE_T;

#if 1//1//defined(VDEC_YUV_CVT_IMG) 
typedef struct _VDECOMX_SET_COLOR_CVT_T
{
   unsigned int  u4DstAddr;   
   unsigned int u4SrcBufIndex;
   unsigned int  u4AddrY;
   unsigned int  u4AddrC;
   unsigned short u2LineSize;
   unsigned short u2HSize;
   unsigned short u2HTGSize;
   unsigned short u2VTGSize;
   unsigned short u2VSize;
   unsigned short u2TGStride;   
} VDECOMX_SET_COLOR_CVT_T;
#endif

typedef struct
{
    unsigned int   u4Width;           //Full size 16 byte align
    unsigned int   u4Height;          //Full size 16 byte align
    unsigned short  u2CropX;
    unsigned short  u2CropY;
    unsigned short  u2CropWidth;
    unsigned short  u2CropHeight;
    unsigned int   u4Offset;          //Offset of YUV buffer start address
} VDECOMX_FRAMEBUF_FORMAT_T;

typedef struct _VDECOMX_MEM_ADDR_T
{
    unsigned int    u4VA;      ///< [IN/OUT] virtual address   
    unsigned int    u4PA;       ///< [IN/OUT] physical address   
    unsigned int    u4Size;     ///< [IN/OUT] size   
} VDECOMX_MEM_ADDR_T;

typedef enum
{
    VDECOMX_FBSTATUS_NORMAL        = 0,            ///< normal type
    VDECOMX_FBSTATUS_REPEAT_LAST   = (1 << 0),     ///< 
    VDECOMX_FBSTATUS_NOT_DISPLAY   = (1 << 1),     ///< YCbCr 420 block in three planar
    VDECOMX_FBSTATUS_NOT_USED      = (1 << 2)     ///< for H.264 multi-slice
} VDECOMX_FBSTATUS_T;

typedef struct _VDECOMX_FRAMEBUF_T
{
    unsigned int u4AddressY;        ///< [IN/OUT]Buffer width
    unsigned int u4AddressC;        ///< [IN/OUT]Buffer height
    unsigned int u4Size;            ///< [OUT]Display width
    unsigned int u4BufWidth;        ///< [IN/OUT]Buffer width
    unsigned int u4BufHeight;       ///< [IN/OUT]Buffer height
    unsigned int u4DispWidth;       ///< [OUT]Display width
    unsigned int u4DispHeight;      ///< [OUT]Display width
    unsigned int u4DispPitch;       ///< [OUT]Display pitch
    unsigned int u4Timestamp;       ///< [IN/OUT]Timestamp for last decode picture
    unsigned long long u8Timestamp;
    unsigned int u4AspectRatioW;    ///< [OUT]The horizontal size of the sample aspect ratio.
    unsigned int u4AspectRatioH;    ///< [OUT]The vertical size of the sample aspect ratio.
    unsigned int u4FrameBufferType; ///< [OUT]One of VDECOMX_FBTYPE
    unsigned int u4PictureStructure;///< [OUT]One of VDECOMX_PIC_STRUCT
    unsigned int u4FrameBufferStatus;///< [OUT] One of VDECOMX_FBSTSTUS
    unsigned int u4MemHeaderY;         /// < [OUT] kmem map handler
    unsigned int u4MemHeaderC;         /// < [OUT] kmem map handler
    unsigned int u4Eos;             ///< [OUT] Eos    
    BOOL            fgCropping;
    unsigned int  u4CropX;
    unsigned int  u4CropY;
    unsigned int  u4CropWidth;
    unsigned int  u4CropHeight;
    unsigned int u4Reserved1;       ///< Reserved
    unsigned int u4Reserved2;       ///< Reserved
    unsigned int u4Reserved3;       ///< Reserved
} VDECOMX_FRAMEBUF_T;

typedef struct
{
    VDECOMX_FRAMEBUF_FORMAT_T  rFrameFormat;
    VDECOMX_FRAMEBUF_T         rFrameBuf;
} VDECOMX_FRAME_T;


// VDECOMX_SET_TYPE_SET_DECODE_MODE
typedef struct
{
    VDECOMX_FRAME_T            rFrame;
    char                       bDisplay;
    char                       bFree;
} VDECOMX_SET_TYPE_DECODE_MODE_T;

typedef struct
{    
    VDECOMX_VIDEO_FORMAT_T     eVideoFormat;
    unsigned int u4Width;
    unsigned int u4Height;
    unsigned int u4FrameRate;
} VDECOMX_SET_TYPE_PICTURE_INFO_T;

typedef struct _VDECOMX_RINGBUF_T
{
    VDECOMX_MEM_ADDR_T  rBase;      ///< [IN]Base address of ring buffer
    unsigned int    u4Read;     ///< [IN/OUT]Virtual address of read pointer.
    unsigned int    u4Write;    ///< [IN]Virtual address of write pointer.
    unsigned int u4Timestamp;   ///< store timestamp
    unsigned long long u8Timestamp;
    unsigned int    u4BufHandle;
    unsigned int    u4VAddr;     ///< [IN/OUT]Virtual address of read pointer.
    unsigned int    u4Offset;    
    unsigned int    u4Size;
    char fgEos;
} VDECOMX_RINGBUF_T;

typedef struct _VDECOMX_SEQINFO_T
{
    unsigned int u4Width;      ///< Sequence buffer width
    unsigned int u4Height;     ///< Sequence buffer height
    unsigned int u4PicWidth;   ///< Sequence display width
    unsigned int u4PicHeight;  ///< Sequence display height
    BOOL         bCropping;    
    unsigned int u4CropX;
    unsigned int u4CropY;
    unsigned int u4CropWidth;
    unsigned int u4CropHeight;
} VDECOMX_SEQINFO_T;

typedef struct
{
    BOOL                       fgseamless;         //IN
    unsigned int               u4MaxWidth;
    unsigned int               u4MaxHeight;
    unsigned int               u4ResizerId;
} VDECOMX_SET_TYPE_SEAMLESS_INFO_T;

typedef struct _VDECOMX_FEEDER_INFO_T
{
    UINT32 u4TotalSize;
    UINT32 u4BufferCount;    
    UINT32 u4BufferSize;
} VDECOMX_FEEDER_INFO_T;

typedef struct
{
    unsigned int               u4Id;
} VDECOMX_SET_TYPE_VDP_INFO_T;

typedef enum
{
    VDECOMX_SET_TYPE_USE_EXT_TIMESTAMP,        ///< =1, use timestamp in sVDECOMX_FRAMEBUF_T for the picture
    VDECOMX_SET_TYPE_SET_BUFFER_MODE,          ///< value is one of VDECOMX_BUFFER_MODE
    VDECOMX_SET_TYPE_SET_FRAME_BUFFER_TYPE,    ///< one of VDECOMX_FBTYPE, if output type is the same as decode type, buffer mode can be REF_IS_DISP
    VDECOMX_SET_TYPE_FREE_FRAME_BFFER,         ///< release buffer if DISP BUFFER is allocated from driver
    VDECOMX_SET_TYPE_SET_REF_EXT_POOL_ADDR,    ///< if use REF_EXT_POOL in SET_BUFFER_MODE
    VDECOMX_SET_TYPE_SET_DISP_EXT_POOL_ADDR,   ///< if use DISP_EXT_POOL in SET_BUFFER_MODE
    VDECOMX_SET_TYPE_SET_DECODE_MODE,          ///< set if drop frame
    VDECOMX_SET_TYPE_SET_POST_PROC,            ///< buffer mode cannot set to REF_IS_DISP when using post-processing
    VDECOMX_SET_TYPE_SET_STATISTIC_ON,         ///< enable statistic function.
    VDECOMX_SET_TYPE_SET_STATISTIC_OFF,        ///< disable statistic function.
    VDECOMX_SET_TYPE_SET_FRAME_MODE,           ///< set frame mode
    VDECOMX_SET_TYPE_SET_BUF_STATUS_FOR_SPEEDY,///< set buffer status for speedy mode
    VDECOMX_SET_TYPE_SET_LAST_DISPLAY_TIME,    ///< set the last display time
    VDECOMX_SET_TYPE_SET_CURRENT_PLAY_TIME,    ///< set the current play time
    VDECOMX_SET_TYPE_SET_CONCEAL_LEVEL,        ///< error conceal level for decoder
    VDECOMX_SET_TYPE_SET_PICTURE_INFO,         ///< pictue width/height/frame rate
#if 1//1//defined(VDEC_YUV_CVT_IMG) 
    VDECOMX_SET_TYPE_SET_COLOR_CONVERT,       ///< set for color format b2r to yu420 >
#endif
    VDECOMX_SET_TYPE_SET_OMX_TIDS,
    VDECOMX_SET_TYPE_SET_BUFTAG,
    VDECOMX_SET_TYPE_SET_DROP_TAGBUFFER,
    VDECOMX_SET_TYPE_SET_DROP_UNTAGBUFFER,
    VDECOMX_SET_TYPE_FREE_BUFFER,    
    VDECOMX_SET_TYPE_FLUSH_DEOCDER,
    VDECOMX_SET_TYPE_SET_SEAMLESS_INFO,
    VDECOMX_SET_TYPE_SET_VDP_ID,
    VDECOMX_SET_TYPE_VDEC_PAUSE,
    VDECOMX_SET_TYPE_VDEC_RESUME,
} VDECOMX_SET_TYPE_T;

typedef enum _VDECOMX_MODE_T
{
    VDECOMX_MODE_ASYNC = 0,
    VDECOMX_MODE_TUNNEL
} VDECOMX_MODE_T;

typedef enum _VDECOMX_MRESULT_T
{
    VDECOMX_MRESULT_OK = 0,           ///< OK
    VDECOMX_MRESULT_FAIL,             ///< Fail
    VDECOMX_MRESULT_FATAL,            ///< Fatal error to stop.
    VDECOMX_MRESULT_MAX = 0x0FFFFFFF
} VDECOMX_MRESULT_T;

/*=============================================================================
 *                             Function Declaration
 *===========================================================================*/

typedef struct _PAR_VDECOMX_CREATE {
    void                       *pvhdlr;            // OUT
    VDECOMX_MRESULT_T          eRet;               // OUT
    VDECOMX_VIDEO_FORMAT_T     eVideoFormat;       // IN    
    VDECOMX_MODE_T             eMode;
    UINT8 uVdecId;
} PAR_VDECOMX_CREATE;

typedef struct _PAR_VDECOMX_INIT {
    VDECOMX_MRESULT_T          eRet;               // OUT
    VDECOMX_RINGBUF_T          *prBitstream;       // IN
    VDECOMX_SEQINFO_T          *prSeqinfo;         // OUT
} PAR_VDECOMX_INIT;

typedef struct _PAR_VDECOMX_DECODE {
    VDECOMX_MRESULT_T          eRet;               // OUT
    VDECOMX_RINGBUF_T          *prBitstream;       // IN
    VDECOMX_FRAMEBUF_T         *prFramebuf;        // IN
    unsigned int               pHeader;           // IN
} PAR_VDECOMX_DECODE;

typedef struct _PAR_VDECOMX_GETPARAM {
    VDECOMX_MRESULT_T          eRet;               // OUT
    VDECOMX_GET_TYPE_T         eType;              // IN
    void                       *pvInParam;         // IN
    void                       *pvOutParam;        // OUT
} PAR_VDECOMX_GETPARAM;

typedef struct _PAR_VDECOMX_SETPARAM {
    VDECOMX_MRESULT_T          eRet;               // OUT
    VDECOMX_SET_TYPE_T         eType;              // IN
    void                       *pvInParam;         // IN
    void                       *pvOutParam;        // OUT
} PAR_VDECOMX_SETPARAM;



#ifdef __cplusplus
}
#endif

#endif //#ifndef _VDECOMX_MOD_H_




