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

#ifndef _HAL_IMGRESZ_IF_H_
#define _HAL_IMGRESZ_IF_H_

#include "x_typedef.h"
#include "drv_config.h"
#include "x_drv_map.h"

//#include "drv_def.h"
//#include "chip_ver.h"
//#include "sys_config.h"


#ifdef __cplusplus
extern "C" {
#endif

/*! \name Image Resizer HAL Interface
* @{
*/

#define  IMGRESZ_PERFORMANCE_TEST  0

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)
#define IMGRESZ_HAL_MT8530
#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550) //sun
#define IMGRESZ_HAL_MT8550

#if CONFIG_DRV_FPGA_BOARD
#define IMGRESZ_HAL_EMU
#endif
#endif

#define IMGRESZ_HW1_IOMMU_SUPPORT  1  //TV HW1 Support it

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) //sun
#if CONFIG_ENABLE_IOMMU
#define IMGRESZ_IO_MMU_TEST    0
#else
#define IMGRESZ_IO_MMU_TEST    0
#endif
#else
#define IMGRESZ_IO_MMU_TEST    0
#endif
//#define VECTOR_RESIZER0    12
//#define VECTOR_RESIZER1    13
#define IMGRESZ_IOMMU_SOLUTION2_SOLUTION3 0

#if CONFIG_SYS_MEM_PHASE3
#define IMGRESZ_SUPPORT_RESET_DEST_BUFFER 1
#else
#define IMGRESZ_SUPPORT_RESET_DEST_BUFFER 0
#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) //sun
#define IMGRESZ_VIDEO_MODE_SUPPORT_WEBP 0
#else
#define IMGRESZ_VIDEO_MODE_SUPPORT_WEBP 0
#endif

#define IMGRESZ_VIDEO_MODE_SUPPORT_UFO 1

typedef enum {
    IMGRESZ_HAL_RESIZE_MODE_FRAME,                  ///< Resize from whole frame to whole frame
    IMGRESZ_HAL_RESIZE_MODE_PARTIAL                 ///< Resize from partial frame to partial frame
} IMGRESZ_HAL_RESIZE_MODE_T;


typedef enum {
    IMGRESZ_HAL_RESAMPLE_METHOD_BILINEAR,           ///< Bi-linear resample method
    IMGRESZ_HAL_RESAMPLE_METHOD_4_TAP,              ///< 4-tap resample method
    IMGRESZ_HAL_RESAMPLE_METHOD_8_TAP               ///< 8-tap resample method
} IMGRESZ_HAL_RESAMPLE_METHOD_T;


typedef enum {
    IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER,     ///< Y, C buffer supporting YUV422/YUV420, Block/RasterScan mode, Address swap/No swap.
    IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_CB_CR_BUFFER, ///< Y, Cb, Cr buffer.
    IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER,   ///< Color index buffer with color pallet.
    IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER,    ///< ARGB color buffer.
    IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER     ///< AYUV color buffer.
} IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_T;


typedef enum {
    IMGRESZ_HAL_IMG_YUV_FORMAT_420,                 ///< YUV 420
    IMGRESZ_HAL_IMG_YUV_FORMAT_422,                 ///< YUV 422
    IMGRESZ_HAL_IMG_YUV_FORMAT_444                  ///< YUV 444
} IMGRESZ_HAL_IMG_YUV_FORMAT_T;


typedef enum {
    IMGRESZ_HAL_INDEX_BUFFER_FORMAT_2BPP,           ///< 2 bits per pixel index format.
    IMGRESZ_HAL_INDEX_BUFFER_FORMAT_4BPP,           ///< 4 bits per pixel index format.
    IMGRESZ_HAL_INDEX_BUFFER_FORMAT_8BPP            ///< 8 bits per pixel index format.
} IMGRESZ_HAL_INDEX_BUFFER_FORMAT_T;


typedef enum {
    IMGRESZ_HAL_ARGB_BUFFER_FORMAT_0565,            ///< Bit number of (A,R,G,B) = (0,5,6,5)
    IMGRESZ_HAL_ARGB_BUFFER_FORMAT_1555,            ///< Bit number of (A,R,G,B) = (1,5,5,5)
    IMGRESZ_HAL_ARGB_BUFFER_FORMAT_4444,            ///< Bit number of (A,R,G,B) = (4,4,4,4)
    IMGRESZ_HAL_ARGB_BUFFER_FORMAT_8888,            ///< Bit number of (A,R,G,B) = (8,8,8,8)
} IMGRESZ_HAL_ARGB_BUFFER_FORMAT_T;


typedef struct {
    UINT8 u1A;                                      ///< Alfa blanding.
    UINT8 u1R;                                      ///< Color Red.
    UINT8 u1G;                                      ///< Color Green.
    UINT8 u1B;                                      ///< Color Blue.
} IMGRESZ_HAL_ARGB_COLOR_T;

typedef enum _IMGRESZ_UFO_CFG_TYPE_
{
    IMGRESZ_UFO_CFG_TYPE_8BIT=10, //8bit non compress
    IMGRESZ_UFO_CFG_TYPE_10BIT_COMPACT,
    IMGRESZ_UFO_CFG_TYPE_10BIT_REORDER,
    IMGRESZ_UFO_CFG_TYPE_10BIT_COMPACT_UNCOMPRESS,
}IMGRESZ_UFO_CFG_TYPE;


typedef struct {
    IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_T eBufferMainFormat;      ///< Buffer main format
    BOOL fgProgressiveFrame;                                  ///< If this is progressive frame or interlace field.
    BOOL fgTopField;                                          ///< When interlace field, if this is top field or bottom field.
    IMGRESZ_HAL_IMG_YUV_FORMAT_T eYUVFormat;                  ///< (Only for Y_C_BUFFER) YUV format.
    BOOL fgBlockMode;                                         ///< (Only for Y_C_BUFFER) True for Block Mode. False for RasterScan Mode.
    BOOL fgAddrSwap;                                          ///< (Only for Y_C_BUFFER) Address swap.
    UINT32 u4HSampleFactor[3];                                ///< (Only for Y_CB_CR_BUFFER) Horizontal Sample Facotr of component 0,1,2
    UINT32 u4VSampleFactor[3];                                ///< (Only for Y_CB_CR_BUFFER) Vertical Sample Facotr of component 0,1,2
    IMGRESZ_HAL_INDEX_BUFFER_FORMAT_T eIndexBufferFormat;     ///< (Only for INDEX_BUFFER) Index Buffer Format.
    IMGRESZ_HAL_ARGB_COLOR_T *prColorPallet;                  ///< (Only for INDEX_BUFFER) Color pallet.
    IMGRESZ_HAL_ARGB_BUFFER_FORMAT_T eARGBBufferFormat;       ///< (Only for ARGB_BUFFER) ARGB Buffer Format.
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
    BOOL fgWT;                                                ///< (Only for ARGB_BUFFER,AYUV_BUFFER) WT compression                                             ///< (Only for ARGB_BUFFER,AYUV_BUFFER) WT compression
#endif
} IMGRESZ_HAL_IMG_BUF_FORMAT_T;


typedef struct {
    IMGRESZ_HAL_IMG_BUF_FORMAT_T rBufferFormat;      ///< Buffer format
    UINT32 u4BufSA1;                                 ///< Used as Y buffer start address of Y_C_BUFFER and Y_CB_CR_BUFFER, or the start address of INDEX_BUFFER, ARGB_BUFFER, and AYUV buffer.
    UINT32 u4BufSA2;                                 ///< Used as C buffer start address of Y_C BIFFER, or Cb buffer start address of Y_CB_CR_BUFFER.
    UINT32 u4BufSA3;                                 ///< Used as Cr buffer start address of Y_CB_CR_BUFFER.
    UINT32 u4BufSA1App;
    UINT32 u4BufSA2App;
    UINT32 u4BufSA3App;
    UINT32 u4BufWidth;                               ///< Buffer width
    UINT32 u4BufHeight;                              ///< Buffer height
    UINT32 u4BufWidth_Byte;
	UINT32 u4BufAddr_Byte;
	BOOL    fgPitchEn;
    UINT32 u4ImgXOff;                                ///< Image left margin X offset to buffer left margin.
    UINT32 u4ImgYOff;                                ///< Image top margin Y offset to buffer top margin.
    UINT32 u4ImgWidth;                               ///< Image width.
    UINT32 u4ImgHeight;                              ///< Image height.
    UINT8  u1Alpha;                                  ///< Image alpha value. (Only used for blending buffer)
#if IMGRESZ_VIDEO_MODE_SUPPORT_WEBP
    UINT32 u4ImgRealHeight;
#endif
#if IMGRESZ_VIDEO_MODE_SUPPORT_UFO
    UINT32 u4ImgUFO_YSzBuf;
    UINT32 u4ImgUFO_CSzBuf;
    UINT32 u4ImgUFO_YSzBufSz;
    UINT32 u4ImgUFO_YBufSz;   // C Bufsize always is half of Y
    IMGRESZ_UFO_CFG_TYPE eUfoCfgType;
#endif
} IMGRESZ_HAL_IMG_INFO_T;


typedef struct {
    BOOL fgFirstRowBuf;                              ///< If this partial row buffer is the first row buffer.
    BOOL fgLastRowBuf;                               ///< If this partial row buffer is the last row buffer.
    UINT32 u4RowBufHeight;                           ///< If this is 0, determine the height by Jpeg vertical sample factor.
    UINT32 u4CurRowBufSA1;                           ///< Current row buffer start address of component 1.
    UINT32 u4CurRowBufSA2;                           ///< Current row buffer start address of component 2.
    UINT32 u4CurRowBufSA3;                           ///< Current row buffer start address of component 3.
    UINT32 u4PrevRowBufSA1;                          ///< Previous row buffer start address of component 1 when current is not first row.
    UINT32 u4PrevRowBufSA2;                          ///< Previous row buffer start address of component 2 when current is not first row.
    UINT32 u4PrevRowBufSA3;                          ///< Previous row buffer start address of component 3 when current is not first row.
} IMGRESZ_HAL_PARTIAL_BUF_INFO_T;


typedef struct {
    BOOL fgPictureMode;                              ///< If this is picture mode.
    BOOL fgPreloadMode;                              ///< If this is progressive preload mode.
    BOOL fgYExist;                                   ///< If Y component exist
    BOOL fgCbExist;                                  ///< If Cb component exist
    BOOL fgCrExist;                                  ///< If Cr component exist
} IMGRESZ_HAL_JPEG_INFO_T;


#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
typedef struct {
    BOOL fgRPRMode;                                        ///< If this is RM mode.
    BOOL fgRPRRacingModeEnable;                 ///< If this is Tracing mode enable.
} IMGRESZ_HAL_RM_INFO_T;
#endif

typedef struct {
    UINT32 u4SrcCntY;                                ///< Source Count Y
    UINT32 u4SrcCntCb;                               ///< Source Count Cb
    UINT32 u4SrcCntCr;                               ///< Source Count Cr
    UINT32 u4VOffsetY;                               ///< Vertical offset Y
    UINT32 u4VOffsetCb;                              ///< Verfical offset Cb
    UINT32 u4VOffsetCr;                              ///< Verfical offset Cr
    UINT32 u4VNextCY;                                ///< Vertical next C Y
    UINT32 u4VNextCCb;                               ///< Verfical next C Cb
    UINT32 u4VNextCCr;                               ///< Verfical next C Cr
    } IMGRESZ_HAL_HW_STATUS_T;


/// Notify callback function
typedef INT32 (*IMGRESZ_HAL_NOTIFY)(INT32 i4State,void *pvPrivData);
typedef struct
{
  IMGRESZ_HAL_NOTIFY pvCallBackFunc;
  void *pvPrivData;
} IMGRESZ_HAL_NOTIFY_CB_REG_T;

#if IMGRESZ_VIDEO_MODE_SUPPORT_WEBP

typedef struct {
    UINT32 u4RowCnt;
    UINT32 u4RowHeight;
    UINT32 u4RatioHeightY;
    UINT32 u4RatioHeightC;
    INT32 u4BorrowYLines;
    INT32 u4BorrowCLines;
    IMGRESZ_HAL_IMG_INFO_T rPreSrcImgInfo;               /// Source image infomation.
} IMGRESZ_HAL_WEBP_PARTIAL_INFO;

#endif
typedef struct {
    IMGRESZ_HAL_RESIZE_MODE_T eResizeMode;            /// Resize mode
    IMGRESZ_HAL_RESAMPLE_METHOD_T eHResampleMethod;   /// Horizontal Resample method
    IMGRESZ_HAL_RESAMPLE_METHOD_T eVResampleMethod;   /// Vertical Resample method
    IMGRESZ_HAL_IMG_INFO_T rSrcImgInfo;               /// Source image infomation.
    IMGRESZ_HAL_IMG_INFO_T rDestImgInfo;              /// Destination image infomation.
    IMGRESZ_HAL_IMG_INFO_T rBldImgInfo;               /// Blending image infomation.
    IMGRESZ_HAL_PARTIAL_BUF_INFO_T rSrcRowBufInfo;    /// Source row buffer infomation.
    volatile BOOL fgResizeComplete;                            /// Resize complete flag.
    BOOL fgScaling;                                   /// Scaling.
    UINT32 u4TempBufSa;                               /// The start address of temporary buffer for partial mode.
    BOOL fgLumaKeyEnable;                             /// For luma key enable
    UINT8 u1LumaKey;                                  /// Luma key value
    BOOL  fgUserTable;
    IMGRESZ_HAL_JPEG_INFO_T rJpegInfo;                /// Jpeg information
#if IMGRESZ_VIDEO_MODE_SUPPORT_WEBP
    BOOL fgWebpEnable;
    IMGRESZ_HAL_WEBP_PARTIAL_INFO rWebpPartialInfo;
#endif
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    IMGRESZ_HAL_RM_INFO_T    rRMInfo;
#endif
    BOOL fgResume;                                    /// If break and resume.
//#ifdef IMGRESZ_HAL_MT8530
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)
    BOOL fgStop;                                      /// To do stop
#endif

    BOOL fgAlphaComposition;

#if IMGRESZ_SUPPORT_RESET_DEST_BUFFER
    BOOL fgResetDstBuf;                               /// reset dest buffer
#endif

    BOOL fgScale1to1;                                  /// sun new
    BOOL fgScale4to1;                                  /// sun new
    IMGRESZ_HAL_NOTIFY_CB_REG_T rNofifyCallback;      /// Notify callback function
#if CONFIG_DRV_VERIFY_SUPPORT
    BOOL   fgBurstLimit;
    UINT32 u4BurstLimit;
    BOOL   fgBurstReadDisable;
#endif
} IMGRESZ_HAL_INFO_T;

enum imgrz_data_direction {
	IMGRZ_BIDIRECTIONAL = BIDIRECTIONAL,
	IMGRZ_TO_DEVICE = TO_DEVICE,
	IMGRZ_FROM_DEVICE = FROM_DEVICE
};


/// Initialize Image Resizer HAL when boot up
/// \return If return value < 0, it's failed. Please reference imgresz_hal_errcode.h.
INT32 i4ImgResz_HAL_Boot_Init(
    void
);


/// Uninitialize Image Resizer HAL when boot down
/// \return If return value < 0, it's failed. Please reference imgresz_hal_errcode.h.
INT32 i4ImgResz_HAL_Boot_Uninit(
    void
);


/// Initialize Image Resizer HAL
/// \return If return value < 0, it's failed. Please reference imgresz_hal_errcode.h.
INT32 i4ImgResz_HAL_Init(
    UINT32 u4ImgResizerID                               ///< [IN] Image Resizer hardware ID
);


/// Uninitialize Image Resizer HAL
/// \return If return value < 0, it's failed. Please reference imgresz_hal_errcode.h.
INT32 i4ImgResz_HAL_Uninit(
    UINT32 u4ImgResizerID                               ///< [IN] Image Resizer hardware ID
);

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
INT32 i4ImgResz_Gracefully_Reset(
    UINT32 u4ImgReszID                               ///< [IN] Image Resizer hardware ID
);
#endif
/// Set Image Resizer HAL resize mode
/// \return If return value < 0, it's failed. Please reference imgresz_hal_errcode.h.
INT32 i4ImgResz_HAL_Set_Resize_Mode(
    UINT32 u4ImgResizerID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_RESIZE_MODE_T eResizeMode               ///< [IN] Resize mode
);


/// Set Image Resizer HAL resample method
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Set_Resample_Method(
    UINT32 u4ImgResizerID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_RESAMPLE_METHOD_T eHResampleMethod,     ///< [IN] Horizontal Resample method
    IMGRESZ_HAL_RESAMPLE_METHOD_T eVResampleMethod      ///< [IN] Vertical Resample method
);


/// Set Image Resizer HAL source image info.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Set_Source_Image_Info(
    UINT32 u4ImgResizerID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_IMG_INFO_T *prSrcImgInfo                ///< [IN] Source image infomation.
);


/// Set Image Resizer HAL destination image info.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Set_Destination_Image_Info(
    UINT32 u4ImgResizerID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_IMG_INFO_T *prDestImgInfo               ///< [IN] Destination image infomation.
);

#if IMGRESZ_VIDEO_MODE_SUPPORT_WEBP
INT32 i4ImgResz_HAL_Set_PreRow_Image_Info(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_IMG_INFO_T *prPreRowImgInfo,            ///< [IN] Destination image infomation.
    UINT32 u4RowHeight,
    BOOL   fgWebpEnable
);
#endif

/// Set Image Resizer HAL blending image info.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Set_Blending_Image_Info(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_IMG_INFO_T *prBldImgInfo             ///< [IN] Blending image infomation.
);


/// Set Image Resizer HAL partial mode information
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Set_Partial_Mode_Info(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_PARTIAL_BUF_INFO_T *prSrcRowBufInfo, ///< [IN] Source row buffer infomation.
    UINT32 u4TempBufSa                               ///< [IN] The start address of temporary buffer for partial mode.
                                                     ///<      The size of temp buffer is destination image width * 1 bytes.
);

#if CONFIG_SYS_MEM_PHASE3
INT32 i4ImgResz_HAL_Set_DstBuf_Reset(
    UINT32 u4ImgReszID,
    BOOL fgReset
);
#endif


/// Set Image Resizer HAL Jpeg information
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Set_Jpeg_Info(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_JPEG_INFO_T *prJpegInfo              ///< [IN] Jpeg information.
);

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)

/// Set Image Resizer HAL RM(RPR) information
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Set_RM_Info(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_RM_INFO_T *prRMInfo              ///< [IN] rm information.
);
#endif

/// Set Image Resizer HAL Luma key information
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Set_LumaKey(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    BOOL fgEnable,                                   ///< [IN] Luma key enable flag
    UINT8 u1LumaKey                                  ///< [IN] Luma key value
);
INT32 i4ImgResz_HAL_Set_IOMMUTable(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    BOOL fgUserTable                                   ///<
);

INT32 i4ImgResz_HAL_Set_Scale1to1(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    BOOL fgEnable                                   ///< [IN] scale1:1 enable flag
);

INT32 i4ImgResz_HAL_Set_Scale4to1(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    BOOL fgEnable                                   ///< [IN] scale4:1 enable flag
);


/// Image Resizer HAL do resize.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Resize(
    UINT32 u4ImgResizerID                               ///< [IN] Image Resizer hardware ID
);


//#ifdef IMGRESZ_HAL_MT8530
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)

/// Image Resizer HAL stop resize.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Stop(
    UINT32 u4ImgReszID                               ///< [IN] Image Resizer hardware ID
);
#endif


/// Image Resizer HAL Get resize status.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Get_Resize_Status(
    UINT32 u4ImgResizerID                               ///< [IN] Image Resizer hardware ID
);


/// Image Resizer HAL Get HW status.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Get_HW_Status(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_HW_STATUS_T *prHwStatus              ///< [OUT] Hardware status
);


/// Image Resizer HAL Set HW status.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Set_HW_Status(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_HW_STATUS_T *prHwStatus              ///< [IN] Hardware status
);

INT32 i4ImgResz_HAL_Set_4to1_Scale_Info(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    BOOL fgSkip            ///< [IN] Destination image infomation.
);


/// Image Resizer HAL register notify callback function.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Reg_Notify_Callback(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_NOTIFY_CB_REG_T *prNofifyCallback    ///< ]IN] Nofity callback function
);

#if CONFIG_DRV_VERIFY_SUPPORT
INT32 i4ImgResz_HAL_SetBurstLimit(
    UINT32 u4ImgReszID,
    UINT32 ui4BurstLimit
);
INT32 i4ImgResz_HAL_SetBurstReadDisable(
    UINT32 u4ImgReszID,
    BOOL fgDisable
);
#endif
UINT32 i4ImgreszCacheMap(UINT32 i4Start, UINT32 u4Len, UINT32 i4DirMode);
UINT32 i4ImgreszCacheUnMap(UINT32 i4Start, UINT32 u4Len, UINT32 i4DirMode);


INT32 i4ImgResz_HAL_SetAlphaCompostion(
    UINT32 u4ImgReszID,
    BOOL fgAlphaCom
);

#if 0

/// Image Resizer HAL unregister notify callback function.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Unreg_Notify_Callback(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_NOTIFY_CB_REG_T *prNofifyCallback    ///< ]IN] Nofity callback function
);
#endif

//
/*! @} */


#ifdef __cplusplus
}
#endif

#endif //#ifndef _HAL_VDEC_MPEG_IF_H_

