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

/** @file OMX_IndexExt.h - OpenMax IL version 1.1.2
 * The OMX_IndexExt header file contains extensions to the definitions 
 * for both applications and components .
 */

#ifndef OMX_IndexExt_h
#define OMX_IndexExt_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Each OMX header shall include all required header files to allow the
 * header to compile without errors.  The includes below are required
 * for this header file to compile successfully
 */
#include <OMX_Index.h>


/** Khronos standard extension indices.

This enum lists the current Khronos extension indices to OpenMAX IL.
*/
typedef enum OMX_INDEXEXTTYPE {

    /* Component parameters and configurations */
    OMX_IndexExtComponentStartUnused = OMX_IndexKhronosExtensions + 0x00100000,
    OMX_IndexConfigCallbackRequest,                 /**< reference: OMX_CONFIG_CALLBACKREQUESTTYPE */
    OMX_IndexConfigCommitMode,                      /**< reference: OMX_CONFIG_COMMITMODETYPE */
    OMX_IndexConfigCommit,                          /**< reference: OMX_CONFIG_COMMITTYPE */

    /* Port parameters and configurations */
    OMX_IndexExtPortStartUnused = OMX_IndexKhronosExtensions + 0x00200000,

    /* Audio parameters and configurations */
    OMX_IndexExtAudioStartUnused = OMX_IndexKhronosExtensions + 0x00400000,
    //MTK Specific
    OMX_IndexParamAudioWmaPro,              /**< reference: OMX_AUDIO_PARAM_WMAPROTYPE */
    OMX_IndexParamAudioDecodeSize,
    OMX_IndexParamAudioDebugMask,           /**< reference: OMX_AUDIO_DEBUG_MASK */
    OMX_IndexParamAudioRenderTrigger,
    OMX_IndexParamAudioRenderPts,
    OMX_IndexParamAudioRenderOpen,
    OMX_IndexParamAudioRenderClose,
    OMX_IndexParamAudioRenderWrite,
    OMX_IndexParamAudioRenderQuery,
    OMX_IndexParamAudioRenderIoctl,
    OMX_IndexParamAudioDecoderId,

    /* Image parameters and configurations */
    OMX_IndexExtImageStartUnused = OMX_IndexKhronosExtensions + 0x00500000,
    OMX_IndexConfigPictureMode = OMX_IndexExtImageStartUnused + 1,          /**< reference: OMX_IMAGE_MODE */
    OMX_IndexConfigClearImageFrame = OMX_IndexExtImageStartUnused + 2,      /**< reference: OMX_IMAGE_PROCESSING */
    OMX_IndexParamImageFileSize = OMX_IndexExtImageStartUnused + 3,         /**< reference: OMX_IMAGE_USER_DEFINED */
    OMX_IndexParamExpandedSize = OMX_IndexExtImageStartUnused + 4,          /**< reference: OMX_FRAMESIZETYPE */
    OMX_IndexParamOriginalSize = OMX_IndexExtImageStartUnused + 5,          /**< reference: OMX_FRAMESIZETYPE */
    OMX_IndexParamImageFreeBuffer = OMX_IndexExtImageStartUnused + 6,       /**< reference: OMX_BUFFERHEADERTYPE */
    OMX_IndexConfigOutputDevice = OMX_IndexExtImageStartUnused + 7,          /**< reference: OMX_IMAGE_USER_DEFINED */

    /* Video parameters and configurations */
    OMX_IndexExtVideoStartUnused = OMX_IndexKhronosExtensions + 0x00600000,
    OMX_IndexParamNalStreamFormatSupported,         /**< reference: OMX_NALSTREAMFORMATTYPE */
    OMX_IndexParamNalStreamFormat,                  /**< reference: OMX_NALSTREAMFORMATTYPE */
    OMX_IndexParamNalStreamFormatSelect,            /**< reference: OMX_NALSTREAMFORMATTYPE */
    OMX_IndexParamSeamless,                         /**< reference: OMX_SEAMLESSFORMATTYPE */
    OMX_IndexParam3DVideoType,                      /**< reference: OMX_VIDEO_USER_DEFINED*/
    OMX_IndexParamHwId,                             /**< reference: OMX_VIDEO_PARAM_HWID*/
    OMX_IndexParamVideoDecodeSize,                  /**< reference: OMX_VIDEO_PARAM_DECODESIZE*/
    OMX_IndexParamVideoAppType,                     /**< reference: OMX_VIDEO_PARAM_APP_TYPE*/    
    OMX_IndexParamVideoContainerType,               /**< reference: OMX_VIDEO_PARAM_CONTAINER_TYPE*/
    OMX_IndexParamVideoEncryptType,                    /**< reference: OMX_VIDEO_ENCRYPT_INFO*/
    OMX_IndexParamVideoEncryptInbandCmd,            /**< reference: OMX_IMTK_ENC_INBAND_CMD_T*/
    OMX_IndexParamVideoRemovePitchType,             /**< reference: OMX_VIDEO_PARAM_Pitch_TYPE*/
    OMX_IndexParamRenderDisplayFB,
    OMX_IndexParamRenderFreeFB,
    OMX_IndexParamRenderSelGPType,
    OMX_IndexParamRenderReleaseFBG,
    OMX_IndexParamRenderFlushFrm,
    OMX_IndexParamRenderRegion,
    OMX_IndexParamSecureInputBuffer,
    /* Image & Video common configurations */
    OMX_IndexExtCommonStartUnused = OMX_IndexKhronosExtensions + 0x00700000,

    /* Other configurations */
    OMX_IndexExtOtherStartUnused = OMX_IndexKhronosExtensions + 0x00800000,

    /* Time configurations */
    OMX_IndexExtTimeStartUnused = OMX_IndexKhronosExtensions + 0x00900000,

    OMX_IndexExtMax = 0x7FFFFFFF
} OMX_INDEXEXTTYPE;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OMX_IndexExt_h */
/* File EOF */
