/*

	VENC DDI header. V. 0.90

*/

#ifndef VENC_DDI_H
#define VENC_DDI_H

#include <osa_api.h>
#ifndef WIN32_SIM // Modified by dilly97: fix include
#include "common.h"
#else
#include "common_win32.h"
#endif
    
#include "video_ddi.h"
    
#ifdef __cplusplus
    extern "C"
    {
#endif

typedef struct VENC_MSG_TYPE
{
	UINT32	channel;	// Port Id
	UINT8	pictype;	// Picture Type IDC:0 I:1 P:2 B:4, 0xf:thumbnail // name change at 0.90
	UINT64	pts;		// PTS
	UINT8	*pData;		// pointer to Video Data
	UINT32	dataLen;	// Video Data Length
	UINT8	*pRStart;	// start pointer of buffer
	UINT8	*pREnd;		// end pointer of buffer
}VENC_MSG_TYPE_T;

typedef DTV_STATUS_T (*pfnVENCDataHandlingCB)(VENC_MSG_TYPE_T *pMsg);

/**
 * venc input type 
   Chnaged at 0.90
 */
typedef enum
{
	VENC_INPUT_MAIN = 0,	//Main Video source
	VENC_INPUT_SUB		//Sub Video source
}VENC_INPUT_TYPE_T;

/**
 * venc encode format
 */
typedef enum
{
	VENC_CODEC_H264 = 0,	//Encode H264 format
	VENC_CODEC_MPEG2	//Encode MPEG2 format
}VENC_CODEC_T;

/**
 * venc encode frame rate
 */
typedef enum
{
	VENC_FRAMERATE_AUTO,	// Use optimal frame rate
	VENC_FRAMERATE_ASIS,	// Use original framerate
	VENC_FRAMERATE_60P,	//60Hz progressive (reserved not used)
	VENC_FRAMERATE_60I,	//60Hz interlaced (reserved not used)
	VENC_FRAMERATE_30P,	//30Hz progressive
	VENC_FRAMERATE_30I,	//30Hz interlaced (reserved not used)
	VENC_FRAMERATE_25P,	//25Hz progressive (reserved not used)
	VENC_FRAMERATE_25I,	//25Hz interlaced (reserved not used)
	VENC_FRAMERATE_24P,	//24Hz progressive (reserved not used)
	VENC_FRAMERATE_24I,	//24Hz interlaced (reserved not used) 
	VENC_FRAMERATE_INVALID
}VENC_FRAMERATE_T;

/**
 * venc target quality
   change name at 0.90
 */
typedef enum
{
	VENC_QUALITY_HIGH = 0,
	VENC_QUALITY_MID,
	VENC_QUALITY_LOW,
	VENC_QUALITY_ULTRAHIGH,
	VENC_QUALITY_ULTRALOW
}VENC_QUALITY_T;

/**
 * venc profile
   newly added at 0.90
 */
typedef enum
{
	VENC_PROFILE_BASE = 0,
	VENC_PROFILE_MAIN,
	VENC_PROFILE_EXT,
	VENC_PROFILE_HIGH
}VENC_PROFILE_T;

/**
 * venc get information type
   newly added at 0.90
 */
typedef enum
{
	VENC_INFO_FRAMERATE = 0,
        VENC_INFO_WIDTH,
        VENC_INFO_HEIGHT,
        VENC_INFO_NUM
}VENC_INFO_T;
/**
 * venc profile level
   newly added at 0.90
 */
typedef enum
{
	VENC_PROFILELEVEL_30 = 0,
	VENC_PROFILELEVEL_31,
	VENC_PROFILELEVEL_40,
	VENC_PROFILELEVEL_41
}VENC_PROFILELEVEL_T;

/**
 * VENC Initializing
 */
DTV_STATUS_T	DDI_VENC_Initialize(void);

/**
 * VENC Finalizing
 */
DTV_STATUS_T    DDI_VENC_Destroy(void);

/**
 * Start Video Encoding
 */
DTV_STATUS_T	DDI_VENC_Start(UINT8 port);

/**
 * Select Video source
 */
DTV_STATUS_T	DDI_VENC_SetInput(UINT8 port, VENC_INPUT_TYPE_T input);

/**
 * Set output format
   Parameter changed at 0.90
 */
DTV_STATUS_T	DDI_VENC_SetOutput(UINT8 port, VENC_CODEC_T codec, VENC_FRAMERATE_T frame_rate, UINT32 width, UINT32 height, UINT32 bitrate, VENC_PROFILE_T profile, VENC_PROFILELEVEL_T level);

/**
 * Register Callback function for every frame
 */
DTV_STATUS_T	DDI_VENC_RegEncoderCallback(UINT8 port, pfnVENCDataHandlingCB pfnCallBack);

/**
 * Copy Encoded Data to Destination
 */
DTV_STATUS_T	DDI_VENC_CopyData(UINT8 port, UINT8 *pDest, UINT8 *pBufAddr,UINT32 datasize, UINT8 *pRStart, UINT8 *pREnd);

/**
 * Release Encoded Data buffer
 */
DTV_STATUS_T	DDI_VENC_ReleaseData(UINT8 port, UINT8 *pBufAddr,UINT32 datasize);

/**
 * Get Thumbnail Data 
   Added at 0.85
 */
DTV_STATUS_T	DDI_VENC_GetThumbnail(UINT8 port, UINT32 width, UINT32 height, UINT8 *pBufAddr, UINT32 datasize);

UINT32 DDI_VENC_GetInfo(UINT8 port, VENC_INFO_T info);
/**
 * Stop Video Encoding
 */
DTV_STATUS_T	DDI_VENC_Stop(UINT8 port);
#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif
