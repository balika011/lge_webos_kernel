#ifndef _AENC_DDI_H_
#define _AENC_DDI_H_

#include "common.h"
#include "osa_api.h"
#include "global_configurations.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*typedef struct AENC_MSG_TYPE
{
	UINT32	channel;	// Encoder index
	UINT64	pts;		// PTS
	UINT8	*pData;		// pointer to Audio Data
	UINT32	dataLen;	// Audio Data Length
	UINT8	*pRStart;	// start pointer of buffer
	UINT8	*pREnd;		// end pointer of buffer
}AENC_MSG_TYPE_T; */

#if (PLATFORM_TYPE == LG_PLATFORM)
typedef struct AENC_MSG_TYPE
{
	UINT32	channel;	// Encoder index
	UINT64	pts;		// PTS
	UINT8	*pData; 	// pointer to Audio Data
	UINT32	dataLen;	// Audio Data Length
	UINT8	*pRStart;	// start pointer of buffer
	UINT8	*pREnd; 	// end pointer of buffer
}AENC_MSG_TYPE_T;
#else
typedef struct AENC_MSG_TYPE
{
	UINT32	channel;	// Encoder index
	UINT64	pts;		// PTS
	UINT32	*pData;		// pointer to Audio Data
	UINT32	dataLen;	// Audio Data Length
	UINT32	*pRStart;	// start pointer of buffer
	UINT32	*pREnd;		// end pointer of buffer
}AENC_MSG_TYPE_T;
#endif



typedef DTV_STATUS_T (*pfnAENCDataHandlingCB)(AENC_MSG_TYPE_T *pMsg);


/**
 * AENC index
 */
typedef enum
{
	MAIN_IDX_AENC = 0,	//Main encoder
	SUB_IDX_AENC,		//Sub encoder
}AENC_IDX_T;

/**
 * AENC input type
 */
typedef enum
{
	AENC_INPUT_ADEC0 = 0,	//ADEC main path
	AENC_INPUT_ADEC1,	//ADEC sub path
}AENC_INPUT_PATH_T;

/**
 * AENC encode format
 */
typedef enum
{
	AENC_ENCODE_MP3 = 0,	//Encode MP3 format
	AENC_ENCODE_AAC		//Encode AAC format
}AENC_ENCODING_FORMAT_T;

/**
 * AENC encode Status
 */
typedef enum
{
	AENC_STATUS_STOP = 0,	
	AENC_STATUS_PLAY,	
	AENC_STATUS_ABNORMAL,	
}AENC_STATUS_T;

/**
 * AENC encode # of channel
 */
typedef enum
{
	AENC_MONO = 0,	
	AENC_STEREO,		
}AENC_CHANNEL_T;

/**
 * AENC encode bitrate
 */
typedef enum
{
	AENC_BIT_48K = 0,	
	AENC_BIT_56K,		
	AENC_BIT_64K,		
	AENC_BIT_80K,		
	AENC_BIT_112K,		
	AENC_BIT_128K,		
	AENC_BIT_160K,		
	AENC_BIT_192K,		
	AENC_BIT_224K,		
	AENC_BIT_256K,		
	AENC_BIT_320K,		
}AENC_BITRATE_T;

typedef struct AENC_INFO
{
	//Get Info for debugging on DDI	
	AENC_STATUS_T	status;		// current ENC Status
	AENC_ENCODING_FORMAT_T	codec;	// current ENC Codec
	UINT32	errCnt;			// current ENC error counter
	UINT32	inputCnt;		// current ENC input counter - we distinguish whether or not enter data from input.
	UINT32	underflowCnt;		// current ENC underflowCnt in kernel space - we distinguish which module have problem between muxer and encdoer
	UINT32	overflowCnt;		// current ENC overflowCnt - we distinguish
	
	//Set Info - it is applied realtime, no matter stop&start		
	AENC_CHANNEL_T	channel;	// number of channel
	AENC_BITRATE_T	bitrate;	// bitrate
}AENC_INFO_T;

/**
 * AENC Initializing
 */
DTV_STATUS_T	DDI_AENC_Initialize(void);


/**
 * AENC Initializing
 */
DTV_STATUS_T	DDI_AENC_Destroy(void);


/**
 * Start Audio Encoding
 */
DTV_STATUS_T	DDI_AENC_Start(AENC_IDX_T idx, AENC_INPUT_PATH_T inPath, AENC_ENCODING_FORMAT_T aType);

/**
 * Stop Audio Encoding
 */
DTV_STATUS_T	DDI_AENC_Stop(AENC_IDX_T idx);

/**
 * Register Callback function for every frame on AENC
 */
DTV_STATUS_T    DDI_AENC_RegEncoderCallback(AENC_IDX_T idx, pfnAENCDataHandlingCB pfnCallBack);

#if (PLATFORM_TYPE == LG_PLATFORM)
/**
 * Copy Encoded Data to Destination on AENC
 */
DTV_STATUS_T    DDI_AENC_CopyData(AENC_IDX_T idx, UINT8 *pDest, UINT8 *pBufAddr,UINT32 datasize, UINT8 *pRStart, UINT8 *pREnd);

/**
 * Release Encoded Data buffer on AENC
 */
DTV_STATUS_T	DDI_AENC_ReleaseData(AENC_IDX_T idx, UINT8 *pBufAddr,UINT32 datasize);
#else
/**
 * Copy Encoded Data to Destination on AENC
 */
DTV_STATUS_T    DDI_AENC_CopyData(AENC_IDX_T idx, UINT32 *pDest, UINT32 *pBufAddr,UINT32 datasize, UINT32 *pRStart, UINT32 *pREnd);

/**
 * Release Encoded Data buffer on AENC
 */
DTV_STATUS_T	DDI_AENC_ReleaseData(AENC_IDX_T idx, UINT8 *pBufAddr,UINT16 datasize);
#endif

/**
 * Set Audio Info
 */
DTV_STATUS_T	DDI_AENC_SetInfo(AENC_IDX_T idx, AENC_INFO_T info);

/**
 * Get Audio Info
 */
DTV_STATUS_T	DDI_AENC_GetInfo(AENC_IDX_T idx, AENC_INFO_T* info);

/**
* Aenc start, stop status return
*/
BOOLEAN	_AENC_StartStopStatus(void);
void	_AENC_StartStopflag(BOOLEAN set);
UINT64 DDI_AENC_GetSTC(void);


#ifdef __cplusplus
}
#endif

#endif /* _AENC_DDI_H_ */

