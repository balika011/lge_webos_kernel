/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file pvr_common.h
 *
 *  Header file for PVR Driver common usage.
 *
 *  @author		Juyoung Jo ( juyoung.jo@lge.com)
 *  @version	1.0
 *  @date		2013-05-10
 *  @note		Additional information.
 */

#ifndef _PVR_COMMON_HEADER_H_
#define _PVR_COMMON_HEADER_H_

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "hal_common.h"
#include "hal_sdec.h"
#include "hal_vdec.h"
#include "hal_audio.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
/* We use 4 bytes timestamp data on each transport packet. */
#define USE_4BYTES_TIMESTAMP

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define NORMAL_UPLOAD_SPEED		(100)
#define UPLOAD_SPEED_INCREMENT	(20)
#define DOWNLOAD_SLEEP_PREIOD	10 // 10 ms


#define	PVR_RAW_TP_SIZE		188

/* Timestamp definition */
#ifdef USE_4BYTES_TIMESTAMP
	#define	PVR_TIMESTAMP_SIZE	4
#else
	#define	PVR_TIMESTAMP_SIZE	0
#endif

#define	PVR_TP_SIZE			(PVR_RAW_TP_SIZE + PVR_TIMESTAMP_SIZE)

#define PVR_DOWN_READY_TIMEOUT	30
#define PVR_DOWN_TIMEOUT		(PVR_DOWN_READY_TIMEOUT * 50)

#define PVR_UP_READY_TIMEOUT	150
#define PVR_UP_TIMEOUT			(PVR_UP_READY_TIMEOUT * 10)

/* It is an application's charge how to handle the situation, failure of getting upload buffer. */
#if 1
#define	PVR_UP_GET_BUFF_TIMEOUT	(1)
#define	PVR_UP_BUFF_TIMEOUT		(1)
#else
#define	PVR_UP_GET_BUFF_TIMEOUT	((PVR_UP_READY_TIMEOUT/3)*30)
#define	PVR_UP_BUFF_TIMEOUT		(PVR_UP_READY_TIMEOUT/3)
#endif

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/

/**
 * Download channel
 *
*/
typedef enum _PVR_DOWN_CH_T
{
    PVR_DOWN_CH_A = 0,
    PVR_DOWN_CH_B = 1,
	PVR_DOWN_CH_MAX = PVR_DOWN_CH_B
#define	PVR_DOWN_CH_NR	(PVR_DOWN_CH_MAX+1)
} PVR_DOWN_CH_T;

/**
 * Upload channel
 *
*/
typedef enum _PVR_UP_CH_T
{
    PVR_UP_CH_A = 0,
    PVR_UP_CH_B = 1,
	PVR_UP_CH_MAX = PVR_UP_CH_B
#define	PVR_UP_CH_NR	(PVR_UP_CH_MAX+1)
} PVR_UP_CH_T;

/**
 * Frame rate
 *
*/
typedef enum _PVR_FRAMERATE_T
{
	PVR_FR_FORBIDDEN = 0,
	PVR_FR_23_976HZ = 1,
	PVR_FR_24HZ,
	PVR_FR_25HZ,
	PVR_FR_29_97HZ,
	PVR_FR_30HZ,
	PVR_FR_50HZ,
	PVR_FR_59_94HZ,
	PVR_FR_60HZ,
#define	MAX_SUPPORTED_FR_NR (PVR_FR_60HZ)
	PVR_FR_UNKNOWN
} PVR_FRAMERATE_T;

/**
 * Stream information
 *
*/
typedef struct _PVR_STREAM_INFO_T
{
	BOOLEAN				bFoundSeqSPS;
	UINT32				bitRate;
	PVR_FRAMERATE_T		frRate;
} PVR_STREAM_INFO_T;

/**
 * PID information
 *
*/
typedef struct _PVR_PID_INFO_T
{
	UINT16			pid;
	SDEC_PID_TYPE_T	pidType;
	VDEC_CODEC_T	videoCodecType;
	HAL_AUDIO_SRC_TYPE_T audioCodecType;
} PVR_PID_INFO_T;

/**
 * Index type
 *
*/
typedef enum _PVR_INDEX_TYPE_T
{
	PVR_INDEX_TYPE_SEQ			= 0x01,
	PVR_INDEX_TYPE_I_PIC		= 0x02,
	PVR_INDEX_TYPE_SEQ_I_PIC	= 0x03,
 	PVR_INDEX_TYPE_P_PIC		= 0x04,
	PVR_INDEX_TYPE_B_PIC		= 0x08,
	PVR_INDEX_TYPE_NONE			= 0xFF
} PVR_INDEX_TYPE_T;

/**
 * Index
 *
*/
typedef struct _PVR_INDEX_T
{
	PVR_INDEX_TYPE_T	indexType;
	UINT32				byteOffset;
}PVR_INDEX_T;

/**
 * Download source
 *
*/
typedef enum _PVR_DOWN_SRC_T
{
	PVR_DOWN_SRC_DTV			= 0x00,
	PVR_DOWN_SRC_DTV_ENCODED	= 0x10,
	PVR_DOWN_SRC_ATV			= 0x20,
	PVR_DOWN_SRC_AV_1			= 0x40,
	PVR_DOWN_SRC_AV_2			= 0x41,
} PVR_DOWN_SRC_T ;

/**
 * Encoding rate
 *
*/
typedef enum
{
      MAX_BITRATE=1,
      HIGH_BITRATE,
      NORMAL_BITRATE,
      LOW_BITRATE,
      LOWEST_BITRATE
} PVR_ENCODING_BITRATE_T;

/**
 * PVR Download Index Type
 */
typedef enum _PVR_DOWN_INDEX_TYPE_T
{
	PVR_INDEX_MPEG2 = 0,
	PVR_INDEX_MPEG4,
	PVR_INDEX_H264,
	PVR_INDEX_HEVC,
	PVR_INDEX_UNKOWN
} PVR_DOWN_INDEX_TYPE_T;

/**
 * Timestamp type
 *
*/
typedef enum _PVR_TIMESTAMP_T
{
	PVR_TIMESTAMP_TYPE_NONE = 0,
	PVR_TIMESTAMP_TYPE_4BYTE
} PVR_TIMESTAMP_T;

/**
 * Encryption type
 *
*/
typedef enum _PVR_CIPHER_T
{
	PVR_CIPHER_NONE = 0,
	PVR_CIPHER_AES_ECB,
	PVR_CIPHER_AES_CBC,
	PVR_CIPHER_MAX
} PVR_CIPHER_T;

/**
* PVR Cipher Key Type
*
* @see
*/
typedef enum _PVR_CIPHER_KEY_T
{
    PVR_CIPHER_KEY_EVEN = 0,
    PVR_CIPHER_KEY_ODD
} PVR_CIPHER_KEY_T;

/**
* PVR Cipher Operation Type
*
* @see
*/
typedef enum _PVR_CIPHER_OPERATION_T
{
    PVR_CIPHER_OPERATION_ENCRYPT = 0,
    PVR_CIPHER_OPERATION_DECRYPT,
	PVR_CIPHER_OPERATION_NUM
} PVR_CIPHER_OPERATION_T;

/**
 * Cryptography Information Type
 *
*/
typedef struct _PVR_CRYPTO_TYPE_T
{
	PVR_CIPHER_T			cipherAlgorithm;
	PVR_CIPHER_KEY_T		cipherKeyType;
	PVR_CIPHER_OPERATION_T	cipherOperation;
	UINT32					keyBitLen;		/**<  Length of the key  */
	UINT8					*pIV;			/**< A possible Initialization Vector for ciphering */
	UINT8					*pCipherKey;
} PVR_CRYPTO_TYPE_T;

/**
 * Thumbnail
 *
*/
typedef struct _PVR_THUMBNAIL_T
{
	UINT8	*pBuffer;
	UINT32	xSize;
	UINT32	ySize;
	UINT32	outLength;
	UINT16	consumedTime;
} PVR_THUMBNAIL_T;

/*----------------------------------------------------------------------------------------
	Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _PVR_COMMON_HEADER_H_ */

