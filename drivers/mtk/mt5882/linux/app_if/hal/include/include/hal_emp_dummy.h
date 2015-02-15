/*
SIC LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
Copyright(c) 1998-2010 by LG Electronics Inc.
 
This program is free software; you can redistribute it and/or 
modify it under the terms of the GNU General Public License 
version 2 as published by the Free Software Foundation.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
GNU General Public License for more details.
*/ 


/** @file hal_jpeg.c
 *
 *  JPEG Decoder HAL function.
 *
 *  @author	Bong-Rae Cho(bongrae.cho@lge.com)
 *  @version	1.0
 *  @date		2013.04.22
 *  @see		hal_jpeg.c
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include "hal_common.h"


/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define MAX_TRACK   8

#define EMP_MAX_MSG_PARAM 4
#define EMP_MAX_MSG_RET	4


/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define EMP_MACRO_LOCK()    OSA_WaitSem(_gEmpSemaId, OSA_INF_WAIT)
#define EMP_MACRO_UNLOCK()  OSA_PostSem(_gEmpSemaId)


/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
typedef enum
{
   EMP_MSG_PLAYDONE = 1,
   EMP_MSG_PLAYERR,
   EMP_MSG_PLAYSTART,   // #ifdef INCLUDE_YOUTUBE
   EMP_MSG_PLAYPAUSE,	// #ifdef INCLUDE_YOUTUBE
   EMP_MSG_PLAYRESUME,
   EMP_MSG_PLAYSEEK,
   EMP_MSG_ELAPSEDTIME,
   EMP_MSG_EOF,			// #ifdef INCLUDE_YOUTUBE
   EMP_MSG_DATAEXHAUSTED,//for Netflix only
   EMP_MSG_UNKNOWN,
   EMP_MSG_DRM_CHECK_DONE,
   EMP_MSG_FAILDRAG,
   EMP_MSG_SEEKDONE,
   EMP_MSG_SEEKFAIL,
   EMP_MSG_INTERNAL_SUBTITLE,

   EMP_MSG_END
} EMP_MSG_T;

typedef enum
{
    EMP_MEDIA_TYPE_IMAGE = 0,
    EMP_MEDIA_TYPE_UNKNOWN,
} EMP_MEDIA_TYPE_T;

typedef enum
{
    EMP_PICTURE_ENLARGEMENT_1_0X = 0,
	EMP_PICTURE_ENLARGEMENT_1_2X,
	EMP_PICTURE_ENLARGEMENT_1_4X,
	EMP_PICTURE_ENLARGEMENT_1_6X,
	EMP_PICTURE_ENLARGEMENT_1_8X,
	EMP_PICTURE_ENLARGEMENT_2_0X,
    EMP_PICTURE_ENLARGEMENT_MAX_NUM,
} EMP_PICTURE_ENLARGEMENT_RATIO_T; // Enlargement Ratio


typedef enum
{
    EMP_PICTURE_EFFECT_DIRECTION_UP = 0,
    EMP_PICTURE_EFFECT_DIRECTION_DOWN,
    EMP_PICTURE_EFFECT_DIRECTION_LEFT,
    EMP_PICTURE_EFFECT_DIRECTION_RIGHT,
    EMP_PICTURE_EFFECT_DIRECTION_NOT_SUPPORT,
} EMP_PICTURE_EFFECT_DIRECTION_T; // Slideshow Effect direction.


typedef enum
{
    EMP_METADATA_TITLE,
    EMP_METADATA_ALBUM,
    EMP_METADATA_ARTIST,
    EMP_METADATA_GENRE,
    EMP_METADATA_SAMPL_FREQ,
    EMP_METADATA_BITRATE,
    EMP_METADATA_FRAMESIZE,
    EMP_METADATA_PLAYTIME,
    EMP_METADATA_FRAMERATE,
    EMP_METADATA_CAM_MAKER,
    EMP_METADATA_CAM_MODEL,
    EMP_METADATA_CREATION_DATE,
    EMP_METADATA_HRES,
    EMP_METADATA_VRES,
    EMP_METADATA_BPP,
    EMP_METADATA_IMAGETYPE,
    EMP_METADATA_PAR_WIDTH,
    EMP_METADATA_PAR_HEIGHT,
    EMP_METADATA_VIDEO_CODEC,
    EMP_METADATA_AUDIO_CODEC,
    EMP_METADATA_ERR_CODEC_INFO,
    EMP_METADATA_INDEX_EXISTED,
	EMP_METADATA_LIVEFLAG,
    EMP_METADATA_VBR,       // added by geoffrey 20090113
    EMP_METADATA_EMP_FIFODEPTH,
	EMP_METADATA_EMP_FIFOSIZE,
    EMP_METADATA_VDEC_FIFODEPTH,
    EMP_METADATA_VDEC_FIFOSIZE,
    EMP_METADATA_JPEG_DISPLAY_W,
    EMP_METADATA_JPEG_DISPLAY_H,
    EMP_METADATA_ISDIVXPLUSHD,
    EMP_METADATA_STEREOSCOPIC, //hongcheol.eom 2010.12.15 for 3D wmv auto 3D setting
    EMP_METADATA_STEREOSCOPIC_TYPE, //hongcheol.eom 2010.12.15 for 3D wmv auto 3D setting
	EMP_METADATA_ASF_HAEDER_INFO,
    EMP_METADATA_VIDEO_CSC_FORMAT,  // TODO: to be modified
	EMP_METADATA_PLAYBACK_STATE,
} EMP_METADATA_T;


/**
 * Rotation angle
 */
typedef enum
{
    EMP_ROTATION_0,
    EMP_ROTATION_90,
    EMP_ROTATION_180,
    EMP_ROTATION_270
}  EMP_ROTATION_T;

/**
 * Pixel format
 */
typedef enum
{
    EMP_PIXELFMT_RGB888,
    EMP_PIXELFMT_RGB555,
    EMP_PIXELFMT_YUV420,
    EMP_PIXELFMT_YUV422,
    EMP_PIXELFMT_YUV444,
    EMP_PIXELFMT_ARGB8888
} EMP_PIXEL_FORMAT_T;

typedef enum
{
	EMP_PHOTO_SLIDESHOW_EFFECT_DISABLE,
	EMP_SLIDE_SHOW_EFFECT_SLIDE,
	EMP_SLIDE_SHOW_EFFECT_FADE_IN,
#if 1
    EMP_SLIDE_SHOW_EFFECT_NORMAL,//0
#if (ENABLE_PHOTO_TRANS_EFFECT_BLOCKS)
    EMP_SLIDE_SHOW_EFFECT_HORZ_BLOCKS_TOPLEFT, //01
    EMP_SLIDE_SHOW_EFFECT_HORZ_BLOCKS_TOPRIGHT,
    EMP_SLIDE_SHOW_EFFECT_HORZ_BLOCKS_BOTTOMLEFT,
    EMP_SLIDE_SHOW_EFFECT_HORZ_BLOCKS_BOTTOMRIGHT,
    EMP_SLIDE_SHOW_EFFECT_VERT_BLOCKS_TOPLEFT,
    EMP_SLIDE_SHOW_EFFECT_VERT_BLOCKS_TOPRIGHT,
    EMP_SLIDE_SHOW_EFFECT_VERT_BLOCKS_BOTTOMLEFT,
    EMP_SLIDE_SHOW_EFFECT_VERT_BLOCKS_BOTTOMRIGHT,
    EMP_SLIDE_SHOW_EFFECT_HORZ_ZIGZAG_BLOCKS_TOPLEFT,
    EMP_SLIDE_SHOW_EFFECT_HORZ_ZIGZAG_BLOCKS_TOPRIGHT,//10
    EMP_SLIDE_SHOW_EFFECT_HORZ_ZIGZAG_BLOCKS_BOTTOMLEFT,
    EMP_SLIDE_SHOW_EFFECT_HORZ_ZIGZAG_BLOCKS_BOTTOMRIGHT,
    EMP_SLIDE_SHOW_EFFECT_VERT_ZIGZAG_BLOCKS_TOPLEFT,
    EMP_SLIDE_SHOW_EFFECT_VERT_ZIGZAG_BLOCKS_TOPRIGHT,
    EMP_SLIDE_SHOW_EFFECT_VERT_ZIGZAG_BLOCKS_BOTTOMLEFT,
    EMP_SLIDE_SHOW_EFFECT_VERT_ZIGZAG_BLOCKS_BOTTOMRIGHT,
    EMP_SLIDE_SHOW_EFFECT_BLOCKS_RANDOM,//17
#endif
#if (ENABLE_PHOTO_TRANS_EFFECT_DIAGONAL_BLOCKS)
    EMP_SLIDE_SHOW_EFFECT_DIAGONAL_BLOCKS_TOPLEFT,
    EMP_SLIDE_SHOW_EFFECT_DIAGONAL_BLOCKS_TOPRIGHT,
    EMP_SLIDE_SHOW_EFFECT_DIAGONAL_BLOCKS_BOTTOMLEFT, //20
    EMP_SLIDE_SHOW_EFFECT_DIAGONAL_BLOCKS_BOTTOMRIGHT,
#endif
#if (ENABLE_PHOTO_TRANS_EFFECT_WIPE)
    EMP_SLIDE_SHOW_EFFECT_BARWIPE_LEFT2RIGHT,//22
    EMP_SLIDE_SHOW_EFFECT_BARWIPE_RIGHT2LEFT,//23
    EMP_SLIDE_SHOW_EFFECT_BARWIPE_TOP2BOTTOM,//24
    EMP_SLIDE_SHOW_EFFECT_BARWIPE_BOTTOM2TOP,//25
    EMP_SLIDE_SHOW_EFFECT_BOXWIPE_TOPLEFT,//26
    EMP_SLIDE_SHOW_EFFECT_BOXWIPE_TOPRIGHT,//27
    EMP_SLIDE_SHOW_EFFECT_BOXWIPE_BOTTOMLEFT,//28
    EMP_SLIDE_SHOW_EFFECT_BOXWIPE_BOTTOMRIGHT,//29
    EMP_SLIDE_SHOW_EFFECT_BARNDOORWIPE_HORIZONTAL_OPEN,//30
    EMP_SLIDE_SHOW_EFFECT_BARNDOORWIPE_HORIZONTAL_CLOSE,//31
    EMP_SLIDE_SHOW_EFFECT_BARNDOORWIPE_VERTICAL_OPEN,//32
    EMP_SLIDE_SHOW_EFFECT_BARNDOORWIPE_VERTICAL_CLOSE,//33
    EMP_SLIDE_SHOW_EFFECT_BOXWIPE_TOPCENTER,//34
    EMP_SLIDE_SHOW_EFFECT_BOXWIPE_RIGHTCENTER,//35
    EMP_SLIDE_SHOW_EFFECT_BOXWIPE_BOTTOMCENTER,//36
    EMP_SLIDE_SHOW_EFFECT_BOXWIPE_LEFTCENTER,//37
    EMP_SLIDE_SHOW_EFFECT_IRISWIPE_CENTER,//38
    EMP_SLIDE_SHOW_EFFECT_IRISWIPE_OUTER,//39
    EMP_SLIDE_SHOW_EFFECT_4BARWIPE_HORIZONTAL_FROM_TOP,//40
    EMP_SLIDE_SHOW_EFFECT_4BARWIPE_HORIZONTAL_FROM_BOTTOM,
    EMP_SLIDE_SHOW_EFFECT_4BARWIPE_VERTICAL_FROM_LEFT,
    EMP_SLIDE_SHOW_EFFECT_4BARWIPE_VERTICAL_FROM_RIGHT,//43
#endif
#if (ENABLE_PHOTO_TRANS_EFFECT_SLIDE)
    EMP_SLIDE_SHOW_EFFECT_BARSLIDE_LEFT2RIGHT,//44
    EMP_SLIDE_SHOW_EFFECT_BARSLIDE_RIGHT2LEFT,
    EMP_SLIDE_SHOW_EFFECT_BARSLIDE_TOP2BOTTOM,
    EMP_SLIDE_SHOW_EFFECT_BARSLIDE_BOTTOM2TOP,
    EMP_SLIDE_SHOW_EFFECT_BOXSLIDE_TOPLEFT,
    EMP_SLIDE_SHOW_EFFECT_BOXSLIDE_TOPRIGHT,
    EMP_SLIDE_SHOW_EFFECT_BOXSLIDE_BOTTOMLEFT,//50
    EMP_SLIDE_SHOW_EFFECT_BOXSLIDE_BOTTOMRIGHT,
    EMP_SLIDE_SHOW_EFFECT_BARNDOORSLIDE_HORIZONTAL_OPEN,
    EMP_SLIDE_SHOW_EFFECT_BARNDOORSLIDE_HORIZONTAL_CLOSE,
    EMP_SLIDE_SHOW_EFFECT_BARNDOORSLIDE_VERTICAL_OPEN,
    EMP_SLIDE_SHOW_EFFECT_BARNDOORSLIDE_VERTICAL_CLOSE,
    EMP_SLIDE_SHOW_EFFECT_4BARSLIDE_HORIZONTAL_FROM_TOP,
    EMP_SLIDE_SHOW_EFFECT_4BARSLIDE_HORIZONTAL_FROM_BOTTOM,
    EMP_SLIDE_SHOW_EFFECT_4BARSLIDE_VERTICAL_FROM_LEFT,
    EMP_SLIDE_SHOW_EFFECT_4BARSLIDE_VERTICAL_FROM_RIGHT,//59
#endif
#if (ENABLE_PHOTO_TRANS_EFFECT_FADE_IN)
//    EMP_SLIDE_SHOW_EFFECT_FADE_IN,
#endif
    EMP_SLIDE_SHOW_EFFECT_RANDOM,
    EMP_SLIDE_SHOW_EFFECT_CLOSE,
#endif
} EMP_PHOTO_SLIDE_SHOW_EFFECT_T;

typedef enum EMP_CONTAINER{
   	EMP_CONTAINER_UNKNOWN,   	/**< Unknown */
   	EMP_CONTAINER_AVI,       	/**< Audio Video Interleave */
   	EMP_CONTAINER_MKV,       	/**< Matroska container */
   	EMP_CONTAINER_MP4,       	/**< MP4 (MPEG-4 Part12) container */
   	EMP_CONTAINER_MPEG2,		/**< MPEG2 container */
   	EMP_CONTAINER_ASF,       	/**< Advanced Systems Format */
   	EMP_CONTAINER_FLV,       	/**< Flash video container */
   	EMP_CONTAINER_MP3,       	/**< Mp3 audio container */
   	EMP_CONTAINER_WAV,			/**< WAV audio container */
   	EMP_CONTAINER_MAX
} EMP_CONTAINER_T;


typedef struct EMP_SUBTITLE_INFO
{
	UINT32 width;
	UINT32 height;

	// mkv subtitle info (txt)
	UINT32 nTxtPosition;
	struct {
	    UINT8 *data;
	    UINT64 size;
	} font;

} EMP_SUBTITLE_INFO_T;

    
typedef struct EMP_DATA
{
    //NEXUS_FilePlayHandle empFile;
    //NEXUS_PlaybackSettings empPlaybackSettings;
    EMP_MEDIA_TYPE_T    empMediaType;

    int empPlaybackStarted;

    EMP_CONTAINER_T nContainerType; //NEXUS_TransportType transportType;
    //NEXUS_VideoCodec  videoCodec;
    //NEXUS_AudioCodec  audioCodec;
    unsigned int        pcrPid;
    unsigned int        numAudio;
    unsigned int        numVideo;
    unsigned int        AUDIO_DecodingIndex;

    unsigned int        playTime;
    unsigned int        playPosition;
    unsigned int        hWidth;
    unsigned int        vWidth;

    unsigned int        imageWidth;
    unsigned int        imageHeight;
    unsigned int        bPlayJpegDec;
    EMP_ROTATION_T      rotation;
    UINT8               bStreamObject;
    UINT64              streamFileSize;
    unsigned int        jumpPosition;
    //EMP_STREAM_PB_STATUS_T  PBstatus;
    //EMP_STREAM_PB_STATUS_T    PBstopped;

    UINT8               ch;
    UINT16              pcmSampleRate;
    UINT16              pcmBitRate;
    UINT16              pcmChannelInfo;
    UINT64              videoBitRate;   /* DLNA */
    unsigned int        bWavHeaderPadding;

    void                *pData;             // ptr to parser specific data for this media item
    UINT8               location[1024];     // complete path and file name.
    unsigned int        IndexExist;
    unsigned int        maxBitrate;
    //EMP_VIDEO_INFO_T    videoInfo[MAX_TRACK];
    //EMP_AUDIO_INFO_T    audioInfo[MAX_TRACK];

    UINT32      nSeqDataLen;
    UINT8*      pSeqData;

#ifdef SUPPORT_DIVX_HD_PLUS
    EMP_HDPLUS_TITLE_LIST_T titleList;
    EMP_HDPLUS_TITLE_T      *pCurTitleInfo;
    EMP_HDPLUS_EDITION_T    *pCurEditionInfo;
    EMP_HDPLUS_CHAPTER_T    *pCurChapterInfo;

    #if 0
    b_track_info        *cur_track_info;
    NEXUS_AspectRatio   aspect_ratio;
    unsigned int        multiplayTime;
    unsigned int        multihWidth;
    unsigned int        multivWidth;
    unsigned int        titleindex;
    UINT32              multiVideoPID;
    UINT32              multiVideoCodec;
    UINT32              multiAudioPID;
    UINT32              multiAudioCodec;
    UINT32              multiPARw;
    UINT32              multiPARh;
    #endif
#endif

    unsigned int        numSubtitle;
    EMP_SUBTITLE_INFO_T subtitleInfo[MAX_TRACK];
    //b_stitle_info     subtitleInfo;

    unsigned int        frameRate;
#ifdef INCLUDE_FLASH_BR
    UINT32              bufferMinLevel;
    UINT32              bufferMaxLevel;
    SINT64              ptsToDecode;
    BOOLEAN             pauseAtDecodeTime;
#endif
    BOOLEAN             isDrm;
#ifdef INCLUDE_VCS
    //EMP_VCS_CHANNEL_T       vcsCh;
//  GFXOSD_IMAGE_INFO_T     vcsImage_info;  // for YUV info
//  GFXOSD_OUTPUT_INFO_T    vcsOutput_info; // for YUV info
//  GFXOSD_RECT_T           inRect;         // for MJPEG info
//  GFXOSD_RECT_T           outRect;        // for MJPEG info
#endif

    UINT8               demuxLibName[64];
    UINT8               demuxPluginName[64];
} EMP_DATA_T;


typedef void* EMP_MEDIA_ITEM_T;

typedef void (*pfnEMPCallback_T)(EMP_MEDIA_ITEM_T item, EMP_MSG_T msg, UINT32 data);


typedef enum
{
   EMP_MSG_SWDEC_PLAY          = 1,
   EMP_MSG_SWDEC_EXIF_PLAY     = 2,
   EMP_MSG_SWDEC_SHOW_MP3_JPEG = 3,
   EMP_MSG_SWDEC_GET_JPEGINFO  = 4
} EMP_MSG_SWDEC_T;

typedef UINT32(*pfnEMPSwDecCallback_T)(EMP_MSG_SWDEC_T msg,
                                     const UINT32 param[EMP_MAX_MSG_PARAM],
                                     UINT32 ret[EMP_MAX_MSG_RET]);

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/




/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/





/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------
	Global Function Definitions
----------------------------------------------------------------------------------------*/    
DTV_STATUS_T DDI_EMP_Initialize( EMP_MEDIA_TYPE_T mediaType, UINT32 useJpegDecoderBuf);

DTV_STATUS_T DDI_EMP_Finalize(EMP_MEDIA_TYPE_T mediaType);  // kjoh LGE 20081118

DTV_STATUS_T DDI_EMP_CreateMediaItem(UINT8 *pMediaPath, EMP_MEDIA_ITEM_T *pItem);

DTV_STATUS_T DDI_EMP_DeleteMediaItem(EMP_MEDIA_ITEM_T pItem);

DTV_STATUS_T DDI_EMP_RegisterCallback(EMP_MEDIA_ITEM_T pItem, EMP_MEDIA_TYPE_T type, pfnEMPCallback_T pfnHandler);

DTV_STATUS_T DDI_EMP_Play(EMP_MEDIA_ITEM_T pItem);

DTV_STATUS_T DDI_EMP_Stop(EMP_MEDIA_ITEM_T pItem);

DTV_STATUS_T DDI_EMP_Rotate(EMP_MEDIA_ITEM_T pItem, EMP_ROTATION_T rotation);

DTV_STATUS_T DDI_EMP_GetThumbNail (EMP_MEDIA_ITEM_T pItem, UINT32* pJpgData, UINT32* pSize, UINT16* pWidth, UINT16* pHeight, EMP_PIXEL_FORMAT_T fpFormat);

DTV_STATUS_T DDI_EMP_GetFullThumbNail (EMP_MEDIA_ITEM_T pItem, UINT32* pJpgData, UINT32* pSize, UINT16* pWidth, UINT16* pHeight, EMP_PIXEL_FORMAT_T fpFormat);
DTV_STATUS_T DDI_EMP_GetFullThumbnail (EMP_MEDIA_ITEM_T pItem, UINT32* pJpgData, UINT32* pSize, UINT16* pWidth, UINT16* pHeight, EMP_PIXEL_FORMAT_T fpFormat, int rotate);

DTV_STATUS_T DDI_EMP_GetThumbNail_BG (EMP_MEDIA_ITEM_T pItem, UINT32* pJpgData, UINT32* pSize, UINT16* pWidth, UINT16* pHeight, EMP_PIXEL_FORMAT_T fpFormat);

DTV_STATUS_T DDI_EMP_GetMetaData(EMP_MEDIA_ITEM_T pItem, EMP_METADATA_T meta, UINT8 *buf, UINT8 bufsize);

void DDI_EMP_ClearFrameBuffer(void);

UINT8 *DDI_EMP_GetFrameBuffer(void);

DTV_STATUS_T DDI_EMP_FlushFrameBuffer(UINT8* pFrame);

DTV_STATUS_T DDI_EMP_SetEnlargement ( EMP_MEDIA_ITEM_T pItem, EMP_PICTURE_ENLARGEMENT_RATIO_T EnlarageRatio );

DTV_STATUS_T DDI_EMP_GetJpegXYWH(EMP_MEDIA_ITEM_T item, UINT16 *pImgX, UINT16 *pImgY, UINT16 *pImgW, UINT16 *pImgH);

DTV_STATUS_T DDI_EMP_RegisterSwDecCallback(pfnEMPSwDecCallback_T pfnHandler);

DTV_STATUS_T DDI_EMP_SetPictureMovePosition ( EMP_MEDIA_ITEM_T pItem, UINT16 ImgX, UINT16 ImgY);

DTV_STATUS_T DDI_EMP_SetSlideShowEffect(EMP_PHOTO_SLIDE_SHOW_EFFECT_T eType, EMP_PICTURE_EFFECT_DIRECTION_T picDirect, UINT32 timeInterval);

DTV_STATUS_T DDI_EMP_ReadySlideshowEffect(EMP_PHOTO_SLIDE_SHOW_EFFECT_T eType);

DTV_STATUS_T DDI_EMP_IsMPOFile(UINT8 *pMediaPath);

DTV_STATUS_T DDI_EMP_StopSlideShowEffect(void);

void DDI_EMP_NotifyFullScreenPictureDisplay(BOOLEAN Status);


/*----------------------------------------------------------------------------------------
	Static Function Definitions
----------------------------------------------------------------------------------------*/    

