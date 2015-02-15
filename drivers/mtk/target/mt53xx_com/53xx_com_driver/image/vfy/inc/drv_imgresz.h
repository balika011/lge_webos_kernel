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

#ifndef __IMGRESZ_DRV_DRV_H
#define __IMGRESZ_DRV_DRV_H

//#include "sys_config.h"

/*-----------------------------------------------------------------------------
                    include files
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/

#define IMGRESZ_ALPHA_BLENDING 1

/// Image Resizer Scale Mode
typedef enum
{
    IMGRESZ_DRV_NONE_SCALE,             ///< Not assign scale mode yet.
    IMGRESZ_DRV_FRAME_SCALE,            ///< Frame mode, scale whole frame at once.
    IMGRESZ_DRV_PARTIAL_SCALE,          ///< Partial mode decode, scale seperate partial of one frame.
    IMGRESZ_DRV_JPEG_PIC_SCALE,         ///< Jpeg picture mode decode
    IMGRESZ_DRV_SCALE_MODE_MAX          ///< The max
} IMGRESZ_DRV_SCALE_MODE;


/// Image Resizer Scale Priority
typedef enum
{
    IMGRESZ_DRV_PRIORITY_LOW = 1,       ///< Low priority, for normal image decoder driver
    IMGRESZ_DRV_PRIORITY_MID = 3,       ///< Medium priority, for video decoder driver SD source using
    IMGRESZ_DRV_PRIORITY_HIGH = 10,     ///< High priority, for video decoder driver HD source using
    IMGRESZ_DRV_PRIORITY_MAX            ///< The max
} IMGRESZ_DRV_SCALE_PRIORITY;


/// Image Resizer State Machine
typedef enum
{
    IMGRESZ_DRV_STATE_NONE,             ///< Instance is released.
    IMGRESZ_DRV_STATE_IDLE,             ///< Instance is lock but not trigger to do scale yet,
                                        ///< or instance has finish scale, but not release yet.
    IMGRESZ_DRV_STATE_START,            ///< Instance has trigger do scale, but not be serviced yet.
    IMGRESZ_DRV_STATE_SCALING,          ///< Instance is serviced to do scale.
    IMGRESZ_DRV_STATE_WAIT_STOP,        ///< When scaling and receive stop command, state change to wait stop. If stop, state change to IDLE.
    IMGRESZ_DRV_STATE_MAX               ///< The max
} IMGRESZ_DRV_STATE;


/// Image Resizer Input(Source Buffer) Color Mode
typedef enum
{
    IMGRESZ_DRV_INPUT_COL_MD_NONE = 0,      ///< Not available.
    IMGRESZ_DRV_INPUT_COL_MD_JPG_DEF,       ///< Video mode, only use in Jpeg partial and picture mode.
    IMGRESZ_DRV_INPUT_COL_MD_420_BLK,       ///< Video mode, YUV420 block mode.
    IMGRESZ_DRV_INPUT_COL_MD_422_BLK,       ///< Video mode, YUV422 block mode.
    IMGRESZ_DRV_INPUT_COL_MD_420_RS,        ///< Video mode, YUV420 raster scan mode.
    IMGRESZ_DRV_INPUT_COL_MD_422_RS,        ///< Video mode, YUV422 raster scan mode.
    IMGRESZ_DRV_INPUT_COL_MD_AYUV,          ///< Video mode, AYUV 8888 mode.
    IMGRESZ_DRV_INPUT_COL_MD_2BPP_IDX,      ///< Graph mode, 2 bits per pixel index mode.
    IMGRESZ_DRV_INPUT_COL_MD_4BPP_IDX,      ///< Graph mode, 4 bits per pixel index mode.
    IMGRESZ_DRV_INPUT_COL_MD_8BPP_IDX,      ///< Graph mode, 8 bits per pixel index mode.
    IMGRESZ_DRV_INPUT_COL_MD_ARGB_1555,     ///< Graph mode, ARGB 1555 mode.
    IMGRESZ_DRV_INPUT_COL_MD_RGB_565,       ///< Graph mode, RGB 565 mode.
    IMGRESZ_DRV_INPUT_COL_MD_ARGB_4444,     ///< Graph mode, ARGB 4444 mode.
    IMGRESZ_DRV_INPUT_COL_MD_ARGB_8888      ///< Graph mode, ARGB 8888 mode.
} IMGRESZ_DRV_SRC_COLOR_MODE;


/// Image Resizer Output(Destination Buffer) Color Mode
typedef enum
{
    IMGRESZ_DRV_OUTPUT_COL_MD_NONE = 0,     ///< Not available.
    IMGRESZ_DRV_OUTPUT_COL_MD_420_BLK,      ///< Video mode, YUV420 block mode
    IMGRESZ_DRV_OUTPUT_COL_MD_422_BLK,      ///< Video mode, YUV422 block mode
    IMGRESZ_DRV_OUTPUT_COL_MD_420_RS,       ///< Video mode, YUV420 raster scan mode
    IMGRESZ_DRV_OUTPUT_COL_MD_422_RS,       ///< Video mode, YUV422 raster scan mode
    IMGRESZ_DRV_OUTPUT_COL_MD_AYUV,         ///< Video mode, AYUV 8888 mode.
    IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_1555,    ///< Graph mode, ARGB 1555 mode.
    IMGRESZ_DRV_OUTPUT_COL_MD_RGB_565,      ///< Graph mode, RGB 565 mode.
    IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_4444,    ///< Graph mode, ARGB 4444 mode.
    IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_8888     ///< Graph mode, ARGB 8888 mode.
} IMGRESZ_DRV_DST_COLOR_MODE;


/// Image Resizer Output(Destination Buffer) Color Mode
typedef enum
{
    IMGRESZ_DRV_BLENDING_COL_MD_SAME = 0,   ///< The same as source buffer or destination buffer
} IMGRESZ_DRV_BLD_COLOR_MODE;


/// Image Resizer Specific Component Factor, only for jpeg picture scale using
typedef struct _IMGRESZ_DRV_COMPONENT_FACTOR_T
{
    UINT32                      u4Components;       ///< The number of components in jpeg
    UINT8                       u1YCompFactorH;     ///< Horizontal factor of Y component.
    UINT8                       u1YCompFactorV;     ///< Vertical factor of Y component.
    UINT8                       u1CbCompFactorH;    ///< Horizontal factor of Cb component.
    UINT8                       u1CbCompFactorV;    ///< Vertical factor of Cb component.
    UINT8                       u1CrCompFactorH;    ///< Horizontal factor of Cr component.
    UINT8                       u1CrCompFactorV;    ///< Vertical factor of Cr component.
} IMGRESZ_DRV_COMPONENT_FACTOR_T;


/// Image Resizer Source Buffer Information
typedef struct _IMGRESZ_DRV_SRC_BUF_INFO_T
{
    IMGRESZ_DRV_SRC_COLOR_MODE  eSrcColorMode;  ///< The color mode of input source buffer.
    UINT32                      u4YBufAddr;     ///< In video mode, it means the Y buffer.
                                                ///< In graph mode, it means the graph buffer.
    UINT32                      u4CbBufAddr;    ///< Only used in video and jpeg mode.
    UINT32                      u4CrBufAddr;    ///< Only used in jpeg partial and picture mode.
    UINT32                      u4BufWidth;     ///< The buffer width (pitch).
    UINT32                      u4BufHeight;    ///< The buffer height.
    UINT32                      u4PicPosX;      ///< The picture position X.
    UINT32                      u4PicPosY;      ///< The picture position Y.
    UINT32                      u4PicWidth;     ///< The picture width.
    UINT32                      u4PicHeight;    ///< The picture height.
    BOOL                        fgInterlaced;   ///< Interlaced picture.
    BOOL                        fgTopField;     ///< Interlaced picture top field exist. (For interlaced video only)
    BOOL                        fgBottomField;  ///< Interlaced picture bottom field exist. (For interlaced video only)
    IMGRESZ_DRV_COMPONENT_FACTOR_T  rCompFactor;    ///< Used when eSrcColorMode == IMGRESZ_INPUT_COL_MD_JPG_DEF.
    UINT32                      u4ColorPalletSa; ///< Color Pallet.
    BOOL                        fgWTEnable;     ///< Wavelet transform compression.
} IMGRESZ_DRV_SRC_BUF_INFO_T;


/// Image Resizer Destination Buffer Information
typedef struct _IMGRESZ_DRV_DST_BUF_INFO_T
{
    IMGRESZ_DRV_DST_COLOR_MODE  eDstColorMode;  ///< The color mode of output destination buffer.
    UINT32                      u4YBufAddr;     ///< In video mode, it means the Y buffer.
                                                ///< In graph mode, it means the graph buffer.
    UINT32                      u4CBufAddr;     ///< Only used in video mode.
    UINT32                      u4BufWidth;     ///< The buffer width (pitch).
    UINT32                      u4BufHeight;    ///< The buffer height.
    UINT32                      u4PicPosX;      ///< The picture position X.
    UINT32                      u4PicPosY;      ///< The picture position Y.
    UINT32                      u4PicWidth;     ///< The picture width.
    UINT32                      u4PicHeight;    ///< The picture height.
    BOOL                        fgWTEnable;     ///< Wavelet transform compression.    
#if CONFIG_SYS_MEM_PHASE3    
    BOOL                        fgResetSA;      ///< For reset destination buffer sa    
#endif    
} IMGRESZ_DRV_DST_BUF_INFO_T;


/// Image Resizer Blending Buffer Information
typedef struct _IMGRESZ_DRV_BLD_BUF_INFO_T
{
    IMGRESZ_DRV_BLD_COLOR_MODE  eBldColorMode;  ///< The color mode of output destination buffer.
    UINT32                      u4YBufAddr;     ///< In video mode, it means the Y buffer.
                                                ///< In graph mode, it means the graph buffer.
    UINT32                      u4CbBufAddr;    ///< Only used in video and jpeg mode.
    UINT32                      u4CrBufAddr;    ///< Only used in jpeg partial and picture mode.
    UINT32                      u4BufWidth;     ///< The buffer width (pitch).
    UINT32                      u4BufHeight;    ///< The buffer height.
    UINT32                      u4PicPosX;      ///< The picture position X.
    UINT32                      u4PicPosY;      ///< The picture position Y.
    UINT32                      u4PicWidth;     ///< The picture width.
    UINT32                      u4PicHeight;    ///< The picture height.
    UINT8                       u1Alpha;        ///< Alpha value. Alpha * blending buffer.
    BOOL                        fgBlendBeforeResz; /// < True: Blending before resize. False: Blending after resize.
} IMGRESZ_DRV_BLD_BUF_INFO_T;


/// Image Resizer Partial Buffer Info
typedef struct _IMGRESZ_DRV_PARTIAL_INFO_T
{
    UINT32                      u4YBufAddr;     ///< In graph mode, it means the graph buffer.
    UINT32                      u4CbBufAddr;    ///< Only used in jpeg mode.
    UINT32                      u4CrBufAddr;    ///< Only used in jpeg mode.
    UINT32                      u4YBufLine;     ///< Y buffer line number (if eSrcColorMode != IMGRESZ_INPUT_COL_MD_JPG_DEF).
    UINT32                      u4CbBufLine;    ///< Cb buffer line number (if eSrcColorMode != IMGRESZ_INPUT_COL_MD_JPG_DEF).
    UINT32                      u4CrBufLine;    ///< Cr buffer line number (if eSrcColorMode != IMGRESZ_INPUT_COL_MD_JPG_DEF).
    BOOL                        fgFirstRow;     ///< The First row.
    BOOL                        fgLastRow;      ///< The Last row.
} IMGRESZ_DRV_PARTIAL_INFO_T;


/// Image Resizer Jpg Info
typedef struct _IMGRESZ_DRV_JPEG_INFO_T
{
    BOOL                        fgExistY;       ///< Exist Y component
    BOOL                        fgExistCb;      ///< Exist Cb component
    BOOL                        fgExistCr;      ///< Exist Cr component
    BOOL                        fgPreload;      ///< Progressive preload mode
    UINT32                      u4HwId;         ///< Indicate image resizer hardware ID for picture mode
} IMGRESZ_DRV_JPEG_INFO_T;

/// Imgage Resizer RM video related info
typedef struct _IMGRESZ_DRV_RM_INFO_T
{
    BOOL                        fgRPRMode;     ///< PRP mode or not for rm video
    BOOL                        fgRacingMode;  ///< Racing mode or not
} IMGRESZ_DRV_RM_INFO_T;


/// Image Resizer Do Scale Structure
typedef struct _IMGRESZ_DRV_DO_SCALE_T
{
} IMGRESZ_DRV_DO_SCALE_T;


/// Image Resizer Scaling State
typedef struct _IMGRESZ_DRV_SCALE_STATE_T
{
    IMGRESZ_DRV_STATE           eState;             ///< The Scale State
} IMGRESZ_DRV_SCALE_STATE_T;


/// Image Resizer Ticket
typedef struct _IMGRESZ_DRV_TICKET_T
{
    UINT32                      u4Ticket;           ///< The ticket
} IMGRESZ_DRV_TICKET_T;


/// Notify callback function
typedef INT32 (*IMGRESZ_DRV_NOTIFY)(INT32 i4State,void *pvPrivData);
typedef struct
{
  IMGRESZ_DRV_NOTIFY pvCallBackFunc;
  void *pvPrivData;
} IMGRESZ_DRV_NOTIFY_CB_REG_T;


/// Get a image resizer ticket.
/// You must get a ticket first and then you can control image resizer by the ticket.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.
extern INT32 i4ImgResz_Drv_GetTicket(
    IMGRESZ_DRV_TICKET_T *pImgReszTicket
);


/// Release image resizer by ticket.
/// If you do not use image resizer any more, release it so others can use it.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.
extern INT32 i4ImgResz_Drv_ReleaseTicket(
    IMGRESZ_DRV_TICKET_T *pImgReszTicket
);


/// Set the priority that using image resizer.
/// Image resizer can be used by multi-instances, so higher priority is more likely to be served.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.
extern INT32 i4ImgResz_Drv_SetPriority(
    IMGRESZ_DRV_TICKET_T *pImgReszTicket,
    IMGRESZ_DRV_SCALE_PRIORITY  eScalePriority
);


/// Set lock or unlock image resizer.
/// If you lock image resizer, it can serve only you.
/// Lock can promise high performance, but also prevent other to using it.
/// It suggests that lock should be used only for some specific case.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.
extern INT32 i4ImgResz_Drv_SetLock(
    IMGRESZ_DRV_TICKET_T *pImgReszTicket,
    BOOL fgLock
);


/// Get image resizer state.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.
extern INT32 i4ImgResz_Drv_GetState(
    IMGRESZ_DRV_TICKET_T *pImgReszTicket,
    IMGRESZ_DRV_SCALE_STATE_T *pImgReszState
);


/// Set image resizer scaling mode.
/// Set scaling mode detail in IMGRESZ_DRV_SCALE_MODE.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.
extern INT32 i4ImgResz_Drv_SetScaleMode(
    IMGRESZ_DRV_TICKET_T *pImgReszTicket,
    IMGRESZ_DRV_SCALE_MODE eScaleMode
);

/// Set image resizer luma key.
/// Set luma key information u1LumaKey and fgEnableLumaKey.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.

INT32 i4ImgResz_Drv_SetLumaKey(
    IMGRESZ_DRV_TICKET_T *pImgReszTicket,
    UINT8 u1LumaKey, 
    BOOL fgEnableLumaKey
);
    

/// Set image resizer source buffer information.
/// Set source buffer information detail in IMGRESZ_DRV_SRC_BUF_INFO_T.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.
extern INT32 i4ImgResz_Drv_SetSrcBufInfo(
    IMGRESZ_DRV_TICKET_T *pImgReszTicket,
    IMGRESZ_DRV_SRC_BUF_INFO_T *pSrcBufInfo
);


/// Set image resizer destination buffer information.
/// Set destination buffer information detail in IMGRESZ_DRV_DST_BUF_INFO_T.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.
extern INT32 i4ImgResz_Drv_SetDstBufInfo(
    IMGRESZ_DRV_TICKET_T *pImgReszTicket,
    IMGRESZ_DRV_DST_BUF_INFO_T *pDstBufInfo
);


/// Set image resizer blending buffer information.
/// Set blending buffer information detail in IMGRESZ_DRV_BLD_BUF_INFO_T.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.
extern INT32 i4ImgResz_Drv_SetBldBufInfo(
    IMGRESZ_DRV_TICKET_T *pImgReszTicket,
    IMGRESZ_DRV_BLD_BUF_INFO_T *pBldBufInfo
);


/// Set image resizer partial buffer information.
/// If scaling mode is partial mode, set the partial buffer info.
/// Set partial buffer information detail in IMGRESZ_DRV_PARTIAL_INFO_T.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.
extern INT32 i4ImgResz_Drv_SetPartialBufInfo(
    IMGRESZ_DRV_TICKET_T *pImgReszTicket,
    IMGRESZ_DRV_PARTIAL_INFO_T *pPartialBufInfo
);


/// Set image resizer jpg information.
/// For jpg file use
/// Set jpg information detail in IMGRESZ_DRV_JPEG_INFO_T.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.
extern INT32 i4ImgResz_Drv_SetJpegInfo(
    IMGRESZ_DRV_TICKET_T *pImgReszTicket,
    IMGRESZ_DRV_JPEG_INFO_T *prJpegInfo
);

/// Set image resizer rm video information.
/// For rm video use
/// Set rm video scale information detail in IMGRESZ_DRV_RM_INFO_T.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.

extern INT32 i4ImgResz_Drv_SetRmInfo(
    IMGRESZ_DRV_TICKET_T *pImgReszTicket, 
    IMGRESZ_DRV_RM_INFO_T *prRmInfo
);

/// Set 1:1 scale flag to hw
/// For venc , 8555 only
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h
extern INT32 i4ImgResz_Drv_Set_Scale1To1(
    IMGRESZ_DRV_TICKET_T *pImgReszTicket,
    BOOL fg1To1Scale
);

/// Set 1:1 scale flag to hw
/// For venc , 8555 only
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h

extern INT32 i4ImgResz_Drv_Set_Y_Only(
    IMGRESZ_DRV_TICKET_T *pImgReszTicket, 
    BOOL fgYOnly
);

/// Trigget image resizer to do scale.
/// Be sure all info is set before do scale.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.
extern INT32 i4ImgResz_Drv_DoScale(
    IMGRESZ_DRV_TICKET_T *pImgReszTicket,
    IMGRESZ_DRV_DO_SCALE_T *pDoScale
);


/// To stop scale while doing scale.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.
extern INT32 i4ImgResz_Drv_StopScale(IMGRESZ_DRV_TICKET_T *pImgReszTicket);


/// Register a callback function which will be called when scale finish.
/// You can do send event in this callback function to notify the task that trigger doing scale.
/// Do register callback function before triggering to do scale.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.
extern INT32 i4ImgResz_Drv_RegFinishNotifyCallback(IMGRESZ_DRV_TICKET_T *pImgReszTicket,IMGRESZ_DRV_NOTIFY_CB_REG_T *prNotifyCallbackReg);


/// Unregister the callback function that is registered.
/// Before release a ticket, remember to do unregister.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.
extern INT32 i4ImgResz_Drv_UnregFinishNotifyCallback(IMGRESZ_DRV_TICKET_T *pImgReszTicket,IMGRESZ_DRV_NOTIFY_CB_REG_T *prNotifyCallbackReg);


/// Init image resizer driver.
/// Call this function by system initialization function when system boot up.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.
extern INT32 i4ImgResz_Drv_Init(void);


/// Uninit image resizer driver.
/// Call this function by system uninitialization function when system shut down.
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.
extern INT32 i4ImgResz_Drv_Uninit(void);

/// Uninit image resizer driver.
/// Call this function by system uninitialization function when system shut down,for linux use
/// \return If return value < 0, it's failed. Please reference drv_imgresz_errcode.h.

extern INT32 i4ImgResz_DrvUninit(UINT32 u4Case);


extern INT32 i4ImgResz_Drv_SetAgentID(IMGRESZ_DRV_TICKET_T *pImgReszTicket, UINT32  id);

///\example imgresz_cmd.c

#endif // __IMGRESZ_DRV_DRV_H
