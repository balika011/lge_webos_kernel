/*
 * Copyright (c) 2010 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/** OMX_VideoExt.h - OpenMax IL version 1.1.2
 * The OMX_VideoExt header file contains extensions to the
 * definitions used by both the application and the component to
 * access video items.
 */

#ifndef OMX_VideoExt_h
#define OMX_VideoExt_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Each OMX header shall include all required header files to allow the
 * header to compile without errors.  The includes below are required
 * for this header file to compile successfully
 */
#include <OMX_Core.h>

/** NALU Formats */
typedef enum OMX_NALUFORMATSTYPE {
    OMX_NaluFormatStartCodes = 1,
    OMX_NaluFormatOneNaluPerBuffer = 2,
    OMX_NaluFormatOneByteInterleaveLength = 4,
    OMX_NaluFormatTwoByteInterleaveLength = 8,
    OMX_NaluFormatFourByteInterleaveLength = 16,
    OMX_NaluFormatCodingMax = 0x7FFFFFFF
} OMX_NALUFORMATSTYPE;


/** NAL Stream Format */
typedef struct OMX_NALSTREAMFORMATTYPE{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_NALUFORMATSTYPE eNaluFormat;
} OMX_NALSTREAMFORMATTYPE;


/** Seamless information */
typedef struct OMX_SEAMLESSFORMATTYPE{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_BOOL bFlagSeamless;
    OMX_U32 nFrameWidth;
    OMX_U32 nFrameHeight;
    OMX_S32 nStride;
} OMX_SEAMLESSFORMATTYPE;

/** 3D Video Type information */
typedef struct OMX_VIDEO_USER_DEFINED{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex; 
    OMX_U32 nUserDefined;
} OMX_VIDEO_USER_DEFINED;

/** Video Decode size information */
typedef struct OMX_VIDEO_PARAM_DECODESIZE
{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_U64 nDecodedSize; // accumulated decode size
    OMX_U64 nUndecodedSize; // undecode size
}  OMX_VIDEO_PARAM_DECODESIZE;

/** Video Decode/Display ID information */
typedef struct OMX_VIDEO_PARAM_HWID
{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_U8 nVdecId; // assign to -1 if not available
}  OMX_VIDEO_PARAM_HWID;

/** Video Decode/Display ID information */
typedef struct OMX_VIDEO_PARAM_APP_TYPE
{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_STRING cAppType;
    OMX_U32 nAppTypeLen;
}  OMX_VIDEO_PARAM_APP_TYPE;

typedef enum OMX_VIDEO_CONTAINER_METADATA {
    OMX_VIDEO_MKVCodecData = 0x01,      /** Have MKV codec data (sequence header)*/
    OMX_VIDEO_VariableFrameRate    = 0x02,   /** Variable frame rate */
    OMX_VIDEO_ContainerMetaDataMax    = 0x7FFFFFFF
} OMX_VIDEO_CONTAINER_METADATA;

/** Video Decode/Display ID information */
typedef struct OMX_VIDEO_PARAM_CONTAINER_TYPE
{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_STRING cContainerType;
    OMX_U32 nContainerTypeLen;
    OMX_VIDEO_CONTAINER_METADATA eMetadata;
} OMX_VIDEO_PARAM_CONTAINER_TYPE;

typedef enum OMX_CONFIG_ENCRYPT_TYPE{
    BUFFER_ENCRYPT_AES,
    BUFFER_ENCRYPT_SCTE52,
    BUFFER_ENCRYPT_ND,
    BUFFER_ENCRYPT_PD,
    BUFFER_ENCRYPT_AES_ENCRYPTION,
    BUFFER_ENCRYPT_INVALID
}OMX_CONFIG_ENCRYPT_TYPE;

#define OMX_IMTK_DRM_MAX_UNIT_NUM (64)
typedef enum
{
   IMTK_DRM_TYPE_MTK=0,
   IMTK_DRM_TYPE_UNKNOW
}OMX_IMTK_DRM_TYPE_T;

typedef struct
{
   OMX_U32 u4Size;
   OMX_BOOL bEncrypt;
}OMX_IMTK_DRM_UNIT_T;

typedef struct
{
   OMX_U32 u4UnitNum;
   OMX_IMTK_DRM_UNIT_T arUnits[OMX_IMTK_DRM_MAX_UNIT_NUM];
}OMX_IMTK_DRM_INFO_T;

typedef struct
{
   OMX_IMTK_DRM_TYPE_T eDrmType;
   OMX_U32 u4KeyIdx;
   OMX_IMTK_DRM_INFO_T rDrmInfo;
}OMX_IMTK_ENC_INBAND_CMD_T;

typedef struct OMX_VIDEO_ENCRYPT_INFO{
    OMX_BOOL bEncrypted;
    OMX_CONFIG_ENCRYPT_TYPE eEncryptType;
}OMX_VIDEO_ENCRYPT_INFO;


//For render=========================
typedef struct OMX_VIDEO_RENDER_INFO{
    OMX_U8 ucFbgId;
    OMX_U8 ucFbId;
    OMX_U32 u4Flag;
} OMX_VIDEO_RENDER_INFO;

typedef struct OMX_VIDEO_RENDER_REGION{
    OMX_U32 u4XOffset;
    OMX_U32 u4YOffset;
    OMX_U32 u4Width;
    OMX_U32 u4Height;
} OMX_VIDEO_RENDER_REGION;

/** Video Input buffer in secure world or not */
typedef struct OMX_VIDEO_PARAM_SECURE_INPUT_BUFFER
{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U8 nSecureInputBuffer; 
} OMX_VIDEO_PARAM_SECURE_INPUT_BUFFER;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OMX_VideoExt_h */
/* File EOF */
