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

#ifndef _VENCOMX_DRV_IF_H_
#define _VENCOMX_DRV_IF_H_

#include "x_typedef.h"
#include <linux/ioctl.h>
#include <linux/fs.h>

#define VENCOMX_LOG_ERROR    (1<<0)
#define VENCOMX_LOG_WARNING  (1<<1)
#define VENCOMX_LOG_DEBUG    (1<<2)
#define VENCOMX_LOG_INFO     (1<<3)
#define VENCOMX_LOG_VERBOS   (1<<4)

typedef struct
{
    INT32 ai4Arg[2];
} VENCOMX_IOCTL_2ARG_T;

/// MTAL_IOCTL_4ARG_T: Send 3 Arguments between user/kernel mode (ioctl interface)
typedef struct
{
    INT32 ai4Arg[3];
} VENCOMX_IOCTL_3ARG_T;

/// MTAL_IOCTL_4ARG_T: Send 4 Arguments between user/kernel mode (ioctl interface)
typedef struct
{
    INT32 ai4Arg[4];

} VENCOMX_IOCTL_4ARG_T;

typedef struct
{
    INT32 ai4Arg[8];
} VENCOMX_IOCTL_8ARG_T;


#define VENCOMX_DEVICE "/dev/vencomx"
#define VENCOMX_IOC_MAGIC    'k'

#define IO_VENC_INIT                   _IOW(VENCOMX_IOC_MAGIC,  0, INT32)
#define IO_VENC_TERMINT                _IOW(VENCOMX_IOC_MAGIC,  1, INT32)
#define IO_VENC_RESET                  _IOW(VENCOMX_IOC_MAGIC,  2, INT32)
#define IO_VENC_SET                    _IOW(VENCOMX_IOC_MAGIC,  3, VENCOMX_IOCTL_4ARG_T)
#define IO_VENC_GET                    _IOW(VENCOMX_IOC_MAGIC,  4, VENCOMX_IOCTL_4ARG_T)
#define IO_VENC_OPEN                   _IOW(VENCOMX_IOC_MAGIC,  5, VENCOMX_IOCTL_4ARG_T)
#define IO_VENC_SET_PARAM              _IOW(VENCOMX_IOC_MAGIC,  6, VENCOMX_IOCTL_3ARG_T)
#define IO_VENC_SET_CTRL               _IOW(VENCOMX_IOC_MAGIC,  7, VENCOMX_IOCTL_3ARG_T)
#define IO_VENC_DRV_INIT               _IOW(VENCOMX_IOC_MAGIC,  8, INT32)
#define IO_VENC_RUN                    _IOW(VENCOMX_IOC_MAGIC,  9, INT32)
#define IO_VENC_STOP                   _IOW(VENCOMX_IOC_MAGIC, 10, INT32)
#define IO_VENC_CLOSE                  _IOW(VENCOMX_IOC_MAGIC, 11, INT32)
#define IO_VENC_ENC_FRAME_SYNC         _IOW(VENCOMX_IOC_MAGIC, 12, VENCOMX_IOCTL_3ARG_T)
#define IO_VENC_ENC_FRAME_ASYNC        _IOW(VENCOMX_IOC_MAGIC, 13, VENCOMX_IOCTL_2ARG_T)
#define IO_VENC_CREATE_BUFFMGR         _IOW(VENCOMX_IOC_MAGIC, 14, VENCOMX_IOCTL_2ARG_T)
#define IO_VENC_DELETE_BUFFMGR         _IOW(VENCOMX_IOC_MAGIC, 15, VENCOMX_IOCTL_2ARG_T)
#define IO_VENC_BUFFMGR_OBJ_GET        _IOW(VENCOMX_IOC_MAGIC, 16, VENCOMX_IOCTL_3ARG_T)
#define IO_VENC_BUFFMGR_OBJ_FREE       _IOW(VENCOMX_IOC_MAGIC, 17, VENCOMX_IOCTL_2ARG_T)
#define IO_VENC_FLUSH_FB               _IOW(VENCOMX_IOC_MAGIC, 18, VENCOMX_IOCTL_3ARG_T)
#define IO_VENC_INSERT_KEYFRM          _IOW(VENCOMX_IOC_MAGIC, 19, INT32)
#define IO_VENC_REG_CB                 _IOW(VENCOMX_IOC_MAGIC, 20, INT32)
#define IO_VENC_UNLOCK_BUF             _IOW(VENCOMX_IOC_MAGIC, 21, VENCOMX_IOCTL_3ARG_T)
#define IO_VENC_ENC_SEQHDR_SYNC        _IOW(VENCOMX_IOC_MAGIC, 22, VENCOMX_IOCTL_2ARG_T)
#define IO_VENC_COPYDATA               _IOW(VENCOMX_IOC_MAGIC, 25, VENCOMX_IOCTL_8ARG_T)

#define VENCOMX_NULL_HANDLE  ((VOID*)0 )          ///< NULL value of VENCOMX_HANDLE

typedef VOID * VENCOMX_HANDLE;                    ///< VENCOMX handle

// for buffer manager
#define VENCOMX_BUFMGR_WAIT        1              ///< blocking wait buffer
#define VENCOMX_BUFMGR_NOWAIT      2              ///< non-blocking wait buffer

typedef enum
{
    VENCR_BUFFER_FULL           =  5,
    VENCR_CARD_READY            =  4,
    VENCR_POD_CI_ACTIVE         =  3,
    VENCR_DA_HIGH               =  2,
    VENCR_CARD_DETECTED         =  1,
    VENCR_OK                    =  0,   ///< Success
    VENCR_NOT_OK                = -1,   ///< Not ok
    VENCR_NOT_SUPPORTED         = -2,   ///< Not support
    VENCR_PARAMETER_ERROR       = -3,   ///< Parameter error
    VENCR_DONT_CARE             = -4,   ///< Don't care
    VENCR_ERR_INV               = -5,   ///< Invalid argument passed
    VENCR_ERR_TIMEDOUT          = -6,    ///< Timeout occured
    VENCR_PARAMETER_NOCHANGE    = -7,  /// Paramenter no change
    VENCR_TVE_SET_VBI_OVERFLOW = -8, ///TVE buffer overflow
} VENCOMX_RESULT_T;

typedef enum
{
    VENCOMX_SRC_TYPE_UNKNOWN = 0,                 ///< source unknown
    VENCOMX_SRC_TYPE_VDEC,                        ///< source vdec
    VENCOMX_SRC_TYPE_VDP                          ///< source vdp
} VENCOMX_SRC_TYPE_T;

typedef enum _VENCOMX_FMT_TYPE_T
{
    VENCOMX_FMT_UNKNOWN = 0,
    VENCOMX_FMT_H264
    /* Others */
} VENCOMX_FMT_TYPE_T;

typedef enum _VENCOMX_COMP_ID_T
{
    VENCOMX_COMP_1,                               ///< COMP_1
    VENCOMX_COMP_NS                               ///< COMP_NS
} VENCOMX_COMP_ID_T;

typedef enum _VENCOMX_H264_PIC_TYPE
{
    VENCOMX_H264_PIC_TYPE_SPS = 0x01,             ///< h.264 pic type sps
    VENCOMX_H264_PIC_TYPE_PPS = 0x02,             ///< h.264 pic type pps
    VENCOMX_H264_PIC_TYPE_SEI = 0x04,             ///< h.264 pic type sei
    VENCOMX_H264_PIC_TYPE_IDR = 0x10,             ///< h.264 pic type IDR
    VENCOMX_H264_PIC_TYPE_I   = 0x20,             ///< h.264 pic type I
    VENCOMX_H264_PIC_TYPE_P   = 0x30,             ///< h.264 pic type P
    VENCOMX_H264_PIC_TYPE_B   = 0x40,             ///< h.264 pic type B
    VENCOMX_H264_PIC_TYPE_MAX                     ///< h.264 pic type Unknown
} VENCOMX_H264_PIC_TYPE;

typedef enum _VENCOMX_NFY_COND_T
{
    VENCOMX_COND_ERROR    = -2,                   ///< Error
    VENCOMX_COND_OVERFLOW = -1,                   ///< Buffer Overflow
    VENCOMX_COND_DATA     =  0,                   ///< Buffer Data Available
    VENCOMX_COND_THUMBNAIL =  1,                  ///< Thumbnail callback
} VENCOMX_NFY_COND_T;

/** vencomx_nfy_fct
 * prototype of notify function
 */
typedef BOOL (*vencomx_nfy_fct)(VOID*              pvTag,
                               VENCOMX_NFY_COND_T  eNfyCond,
                               UINT32             u4Data );


/** VENCOMX_NFY_INFO_T
 * notify info
 */
typedef struct _MTVENC_NFY_INFO_T
{
    VOID*               pvTag;                   ///< tag of notify function
    vencomx_nfy_fct      pfNfy;                   ///< pointer of notify function
} VENCOMX_NFY_INFO_T;

typedef struct _VENCOMX_OUT_PIC_T
{
    VENCOMX_H264_PIC_TYPE eType;                  ///< output picture type
    UINT32               u4Pts;                  ///< output pts
    UINT32               u4Addr;                 ///< output address
    UINT32               u4Size;                 ///< output size
    UINT32               u4RStart;               ///< output buffer start
    UINT32               u4REnd;                 ///< output buffer end
} VENCOMX_OUT_PIC_T;

typedef enum VENCOMX_STATUS_T
{
    VENCOMX_STATUS_UNINIT,
    VENCOMX_STATUS_STOP,
    VENCOMX_STATUS_WAITSIGNAL,
    VENCOMX_STATUS_START, 
    VENCOMX_STATUS_RUN
} VENCOMX_STATUS_T;

typedef struct _VENCOMX_CB_T
{
    VENCOMX_NFY_COND_T eCond;                     ///< notify condition
    union
    {
        VENCOMX_OUT_PIC_T tOut;                   ///< output picture
        UINT32 u4ThumbnailPhyAddr;               ///< thumbnail addresss
    }u;                                          ///< union alias
    VENCOMX_STATUS_T tStatus;
} VENCOMX_CB_T;

typedef struct _VENCOMX_FRAME_RESOLUTION_T
{
    UINT32              u4Width;      /* Resolution: Width */
    UINT32              u4Height;     /* Resolution: Height */
} VENCOMX_FRAME_RESOLUTION_T;

typedef struct _VENCOMX_COMMON_INFO_T
{
    VENCOMX_FRAME_RESOLUTION_T  tResolution;      ///< Resolution
    UINT32                     u4Fps;            ///< Frame Rate
    UINT32                     u4Bps;            ///< Bitrate
} VENCOMX_COMMON_INFO_T;

typedef struct _VENCOMX_H264_INFO_T
{
    VENCOMX_COMMON_INFO_T  tCmmInfo;              ///< common info
/*    MTVENC_PROFILE_T      e_profile;
    MTVENC_LEVEL_T        e_level;*/
} VENCOMX_H264_INFO_T;

typedef struct _VENCOMX_SETTINGS_T
{
    VENCOMX_SRC_TYPE_T       eSrcType;            ///< Source Type
    VENCOMX_FMT_TYPE_T       eEncFmt;             ///< Encode Format Type
    union
    {
        VENCOMX_H264_INFO_T  tFmtH264;            ///< fmt h.264
    }uFmt;                                       ///< Encoding Setting according to eEncFmt
} VENCOMX_SETTINGS_T;

typedef enum _VENCOMX_AP_TYPE_T
{
    VENCOMX_AP_TYPE_UNKNOWN = 0,
    VENCOMX_AP_TYPE_SKYPE,
    VENCOMX_AP_TYPE_ATVPVR,
    VENCOMX_AP_TYPE_BGPVR,
    VENCOMX_AP_TYPE_MSI, // multi-screen interaction
    VENCOMX_AP_TYPE_QQ,
    VENCOMX_AP_TYPE_CUST
} VENCOMX_AP_TYPE_T;

typedef enum _VENCOMX_SET_TYPE_T
{
    VENCOMX_SET_TYPE_CONNECT_SRC,                 ///< set connect
    VENCOMX_SET_TYPE_DISCONNECT_SRC,              ///< set disconnect
    VENCOMX_SET_TYPE_CTRL,                        ///< set ctrl
    VENCOMX_SET_TYPE_SETTINGS,                    ///< set settings
    VENCOMX_SET_TYPE_FRAME_RESOLUTION,            ///< set resolution
    VENCOMX_SET_TYPE_FRAME_RATE,                  ///< set framerate
    VENCOMX_SET_TYPE_BITRATE,                     ///< set bitrate
    /*VENCOMX_SET_TYPE_PROFILE,                   ///< set profile
    VENCOMX_SET_TYPE_LEVEL,*/                     ///< set level
    VENCOMX_SET_TYPE_APP_TYPE,                    ///< set ap type
    VENCOMX_SET_TYPE_RESTART,                     ///< set restart
    VENCOMX_SET_TYPE_INSERT_KEYFRAME,             ///< set insert keyframe
    VENCOMX_SET_TYPE_ENCODE_OSD,                  ///< set encode osd
    VENCOMX_SET_TYPE_MSI_OSTG,                    ///< set encode Output Stage
} VENCOMX_SET_TYPE_T;

typedef enum
{
    VENCOMX_PROFILE_BASE = 0,
    VENCOMX_PROFILE_MAIN,
    VENCOMX_PROFILE_EXT,
    VENCOMX_PROFILE_HIGH
    /* Others */
} VENCOMX_PROFILE_T;

typedef enum
{
    /* H264 */
    VENCOMX_PORFILELEVEL_30 = 0,
    VENCOMX_PORFILELEVEL_31,
    VENCOMX_PORFILELEVEL_40,
    VENCOMX_PORFILELEVEL_41
    /* Others */
} VENCOMX_LEVEL_T;

typedef struct _VENCOMX_PROFILE_LEVEL_T
{
    VENCOMX_PROFILE_T      e_profile;
    VENCOMX_LEVEL_T        e_level;
} VENCOMX_PROFILE_LEVEL_T;

typedef struct _VENCOMX_OUTVIDEO_INFO_T
{
    VENCOMX_AP_TYPE_T eProfile;
    VENCOMX_FMT_TYPE_T encFmt; 
    BOOL                   fgInterlace;     /**< interlace or progress mode */
    BOOL                   fgAddAud;
    VENCOMX_COMMON_INFO_T tCommonInfo; /*Resolution, Frame rate,bit rate*/
    VENCOMX_SRC_TYPE_T srcType;
    VENCOMX_PROFILE_LEVEL_T tProfileLevel;
}VENCOMX_OUTVIDEO_INFO_T;

typedef enum _VENCOMX_GET_TYPE_T
{
    VENCOMX_GET_TYPE_CTRL,
	VENCOMX_GET_TYPE_VENCBUF,
    VENCOMX_GET_TYPE_SIGNALREADY,
    VENCOMX_GET_TYPE_FRAMERATE,
    VENCOMX_GET_TYPE_WIDTH,
    VENCOMX_GET_TYPE_HEIGHT,
    VENCOMX_GET_TYPE_NUM
} VENCOMX_GET_TYPE_T;

typedef enum _VENCOMX_PRAM_T
{
    VENCOMX_PRAM_FRAMERATE,                       ///< parameter framerate
    VENCOMX_PRAM_BITRATE,                         ///< parameter bitrate
    VENCOMX_PRAM_RESOLUTION,                      ///< parameter resolution
    VENCOMX_PRAM_PROFILE_LEVEL,
    VENCOMX_PRAM_MAX                              ///< parameter unknown
} VENCOMX_PRAM_T;

typedef enum _VENCOMX_SMODE_T
{
    VENCOMX_SMODE_SYNC,                           /// sync mode sync, only sync api is work.
    VENCOMX_SMODE_ASYNC,                          /// sync mode async, only async api is work.
    VENCOMX_SMODE_DUAL,                           /// sync mode dual, both sync api and async api are work.
    VENCOMX_SMODE_MAX                             /// sync mode unknown
} VENCOMX_SMODE_T;

typedef enum _VENCOMX_PIX_FMT_T
{
    VENCOMX_PIX_FMT_NV12,                         ///< pixel format NV12, 420 with two planes, the Y plane is first. ex.emulation
    VENCOMX_PIX_FMT_MAX                           ///< pixel format unknown
} VENCOMX_PIX_FMT_T;

typedef enum _VENCOMX_CTRL_T
{
    VENCOMX_CTRL_SYNC_MODE,                       ///< ctrl sync mode
    VENCOMX_CTRL_PIX_FMT,                         ///< ctrl pixel format
    VENCOMX_CTRL_GET_THUMBNAIL,                   ///< ctrl get thumbnail
    VENCOMX_CTRL_MAX                              ///< ctrl unknown
} VENCOMX_CTRL_T;

typedef struct _VENCOMX_CTRL_PIXFMT_T
{
    VENCOMX_PIX_FMT_T ePixFmt;                    ///< pixel format
    UINT32           u4Pitch;                    ///< pitch
} VENCOMX_CTRL_PIXFMT_T;

typedef struct
{
    UINT32 u4Width;                              ///< thumbnail width
    UINT32 u4Height;                             ///< thumbnail height
} VENCOMX_CTRL_THUMBNAIL_T;

typedef struct _VENCOMX_IN_PIC_T
{
    UINT32 u4YAddr;                              ///< input luma address
    UINT32 u4CAddr;                              ///< input chroma address
    UINT32 u4Pts;                                ///< input pts
} VENCOMX_IN_PIC_T;

typedef struct _VENCOMX_ABILITY_T
{
    UINT32 u4MaxWidth;                           ///< Max width
    UINT32 u4MaxHeight;                          ///< Max height
    UINT32 u4Ability320;                         ///< Max # of frame to encode 320x240     
    UINT32 u4MaxFps;                             ///< Max framerate
} VENCOMX_ABILITY_T;

#endif //#ifndef _VENCOMX_DRV_IF_H_




