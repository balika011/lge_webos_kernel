/******************************************************************************
 *   LCD TV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 2011 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/


/** @file hal_jpeg.h
 *
 *  HW JPEG Abstraction Layer header
 *
 *  @note This file is a prototype to provide each chip vender (MTK, MStar, SIC).
 *  @date 2014.06.09.
 **/
#ifndef _HAL_JPEG_H_
#define _HAL_JPEG_H_
     
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This enumeration describes the JPEG API return types.
 **/
typedef enum {
    HAL_JPEG_STATE_OK = 0, /* from OK */
    HAL_JPEG_STATE_ERROR,  /* from ERROR */
    HAL_JPEG_STATE_INVALID_PARAM,  /* from PARAMETER ERROR */
    HAL_JPEG_STATE_NOT_AVAILABLE,  /* from NOT ENOUGH RESOURCE */
    HAL_JPEG_STATE_NOT_CALLABLE,   /* from NOT SUPPORTED */
    HAL_JPEG_STATE_ERR_LENGTH, /* from NOT PERMITTED */
    HAL_JPEG_STATE_TIMEOUT,    /* from TIMEOUT */
    HAL_JPEG_STATE_OBJ_DELETED,    /* from OBJECT DELETED */
} HAL_JPEG_STATE_T;

/**
 * @brief This enumeration describes the supported pixel formats.
 **/
typedef enum {
    HAL_JPEG_PIXEL_FORMAT_NONE = 0, /* none of these */
    HAL_JPEG_PIXEL_FORMAT_PALETTE,    /* palette color type */        
    HAL_JPEG_PIXEL_FORMAT_GRAYSCALE,    /* 8bit gray scale */
    HAL_JPEG_PIXEL_FORMAT_RGB,  /* 24bit RGB */
    HAL_JPEG_PIXEL_FORMAT_BGR,  /* 24bit BGR */
    HAL_JPEG_PIXEL_FORMAT_ARGB, /* 32bit ARGB */
    HAL_JPEG_PIXEL_FORMAT_ABGR, /* 32bit ABGR */
    HAL_JPEG_PIXEL_FORMAT_YUV444P,  /* planar format with each U/V values plane (YYYY UUUU VVVV) */
    HAL_JPEG_PIXEL_FORMAT_YUV444I,  /* planar format with interleaved U/V values (YYYY UVUVUVUV) */
    HAL_JPEG_PIXEL_FORMAT_YUV422P,  /* semi-planar format with each U/V values plane (2x1 subsampling ; YYYY UU VV) */
    HAL_JPEG_PIXEL_FORMAT_YUV422I,  /* semi-planar format with interleaved U/V values (2x1 subsampling ; YYYY UVUV) */
    HAL_JPEG_PIXEL_FORMAT_YUV422YUYV, /* interleaved YUV values (Y0U0Y0V0Y1U1Y1V1) for MStar Chip Vender */
    HAL_JPEG_PIXEL_FORMAT_YUV420P,  /* semi-planar format with each U/V values plane (2x2 subsampling ; YYYYYYYY UU VV) */
    HAL_JPEG_PIXEL_FORMAT_YUV420I,  /* semi-planar format with interleaved U/V values (2x2 subsampling ; YYYYYYYY UVUV) */
    HAL_JPEG_PIXEL_FORMAT_YUV400,   /* 8bit Y plane without U/V values */
    HAL_JPEG_PIXEL_FORMAT_YUV224P,  /* semi-planar format with each U/V values plane, 1 Ysamples has 2 U/V samples to horizontal (Y0Y1 U0U0U1U1V0V0V1V1) */
    HAL_JPEG_PIXEL_FORMAT_YUV224I,  /* semi-planar format with interleaved U/V values (Y0Y1 U0V0U0V0U1V1U1V1) */
    HAL_JPEG_PIXEL_FORMAT_ALPHAGRAYSCALE,    /* gray scale with alpha */
    HAL_JPEG_PIXEL_FORMAT_MAX,  /* maximum number of HAL_JPEG_PIXEL_FORMAT */
} HAL_JPEG_PIXEL_FORMAT;

/**
 * @brief This enumeration describes down-sampling factor using HW decoder.
 **/
typedef enum {
    HAL_JPEG_DECODE_OPTION_SCALING_NONE = 0,    /* none of these */
    HAL_JPEG_DECODE_OPTION_SCALING_1_1, /* 1/1 */
    HAL_JPEG_DECODE_OPTION_SCALING_1_2, /* 1/2 */
    HAL_JPEG_DECODE_OPTION_SCALING_1_4, /* 1/4 */
    HAL_JPEG_DECODE_OPTION_SCALING_1_8, /* 1/8 */
    HAL_JPEG_DECODE_OPTION_SCALING_MAX, /* maximum number of HAL_JPEG_DECODE_OPTION_SCALING */
} HAL_JPEG_DECODE_OPTION_SCALING;

/**
 * @brief This structure describes a rectangle specified by a point and a dimension.
 **/
typedef struct {
    unsigned int x; /* x coordinate of its top-left point */
    unsigned int y; /* y coordinate of its top-left point */
    unsigned int w; /* width of it */
    unsigned int h; /* height of it */
} HAL_JPEG_RECT_T;

/**
 * @brief This structure describes a image information to decode or encode.
 **/
typedef struct {
    unsigned char *buf; /* buffer pointer of decoded raw data */
    unsigned char *ofs_y;   /* offset of Y component */
    unsigned char *ofs_uv;  /* offset of UV components */
    unsigned int len_buf;   /* buffer length of decoded raw data */
    unsigned int stride;    /* stride size of decoded raw data */
    HAL_JPEG_RECT_T rect; /* image data rectangular */
    HAL_JPEG_PIXEL_FORMAT pixel_format;   /* pixel format of image */
} HAL_JPEG_IMAGE_T;

/**
 * @brief This struct describes option to decode image.
 **/
typedef struct {
    HAL_JPEG_DECODE_OPTION_SCALING scale;   /* decoding scale option */
} HAL_JPEG_DECODE_OPTION_T;

/**
 * @brief This struct describes option to encode image.
 **/
typedef struct {
    unsigned int quality;   /* encoding quality */
} HAL_JPEG_ENCODE_OPTION_T;

/**
 * @brief Open the HW JPEG resources and set a memory to deal with decoded bit data.
 * @return if success HAL_JPEG_STATE_OK, else one of error in HAL_JPEG_STATE_T.
 **/
HAL_JPEG_STATE_T HAL_JPEG_Open(void);

/**
 * @brief Decompress bit data according to the input option.
 * @param   src [in]    source image to be decompressing by HW decoder
 * @param   dst [in]    decoded destination image
 * @param   option [in]    decoding option
 * @return if success HAL_JPEG_STATE_OK, else one of error in HAL_JPEG_STATE_T.
 **/
HAL_JPEG_STATE_T HAL_JPEG_Decode(HAL_JPEG_IMAGE_T *src, HAL_JPEG_IMAGE_T *dst, HAL_JPEG_DECODE_OPTION_T *option);

/**
 * @brief Open the HW JPEG resources and set a memory to deal with decoded bit data.
 * @param   src [in]    source image to be compressing by encoder
 * @param   dst [in]    encoded destination image
 * @param   option [in]    encoding option
 * @return if success HAL_JPEG_STATE_OK, else one of error in HAL_JPEG_STATE_T.
 **/
HAL_JPEG_STATE_T HAL_JPEG_Encode(HAL_JPEG_IMAGE_T *src, HAL_JPEG_IMAGE_T *dst, HAL_JPEG_ENCODE_OPTION_T *option);

/**
 * @brief Close the HW JPEG resources and release the memory to deal with decoded bit data.
 * @return if success HAL_JPEG_STATE_OK, else one of error in HAL_JPEG_STATE_T.
 **/
HAL_JPEG_STATE_T HAL_JPEG_Close(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _HAL_JPEG_H_ */
