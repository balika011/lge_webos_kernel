/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * $RCSfile: io_mtvdec.c,v $
 *---------------------------------------------------------------------------*/

/** @file mtvdec.c
 *  This file contains implementation of exported APIs of Video Decoder (VDEC) module,
 *  which inclued MPEG 1/2/4, H.264(AVC), and VC-1.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "mtvdec.h"
#include "mtvdec_mwif.h"
#include "mtpvr.h"
#include "vdec_if.h"
#include "vdec_drvif.h"
#include "mpv_if.h"
#include "vdp_if.h"
#include "dmx_if.h"
#include "stc_drvif.h"
#include "cb_data.h"
#include "cb_low.h"
#include "x_os.h"
#include "x_assert.h"
#include <linux/mtal_ioctl.h>
#ifdef CC_BOOT_VDEC_LOGO
#include "eepdef.h"
#include "eeprom_if.h"
#include "api_eep.h"
#endif
#include "gfx_drvif.h"
#include "x_hal_arm.h"
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
#include "tz_if.h"
#endif

#include <linux/fs.h>           //three include file for dump feeder data
#include <asm/uaccess.h>
#include <linux/param.h>
#include "x_fm.h"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define MTDECODE_CANVAS_SIZE (8*1024*1024)
#ifndef CC_KR_3DDTV
#define CC_KR_3DDTV
#endif

//#define DUMP_YUV_FROM_KERNAL

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// External functions
//-----------------------------------------------------------------------------
extern VOID _MTPVR_ResetMpgInfo(UCHAR ucPvrId);
extern BOOL _MTPVR_QueryPlayMode(UCHAR ucPvrId);
extern MT_RESULT_T _MTPVR_ResetBufferEx(UCHAR ucPvrId);



//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define MAX_USERDATA_TMPSIZE       256
// Maximum Elementary Stream number
//#define VDEC_MAX_ES              2
// Use MsgQ timeout
//#define IFRAME_MSGQ_TIMEOUT
#define IFRAME_TIMEOUT              5000

#define MTVDEC_TIMEOUT    0
#define MTVDEC_FINISH     1
#define MTVDEC_ERROR      2
#define HAL_VIDEO_ES_INPUT_SUPPORT
//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
#ifdef MTVDEC_VIDEO_ES_INPUT_SUPPORT
typedef struct 
{
    UCHAR  ucEsId;
    UINT8  u1DmxId;
    UINT8  u1VideoPidx;
    
    UINT32 u4InputAddr;         //  The buffer start vritual address for user to put data
    UINT32 u4InputSize;         //  The buffer size prepared for user use
    UINT32 u4RingHandleSize;    // The ring handle buffer size
    UINT32 u4InputWp;           // The offset of current WP
} MTVDEC_INPUT_INFO_T;
#endif // MTVDEC_VIDEO_ES_INPUT_SUPPORT

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define MTVDEC_PRINT_INFO(rVdecInfo)                       \
    do{\
        if(IS_MTVDEC_LOG_ENABLE)\
        {                                        \
            printf("rVdecInfo.u2Height= %d\n",rVdecInfo.u2Height);\
            printf("rVdecInfo.u2Width= %d\n",rVdecInfo.u2Width);\
            printf("rVdecInfo.i2DHS= %d\n",rVdecInfo.i2DHS);\
            printf("rVdecInfo.i2DVS= %d\n",rVdecInfo.i2DVS);\
            printf("rVdecInfo.u2FrmRate= %d\n",rVdecInfo.u2FrmRate);\
            printf("rVdecInfo.eAspectRatio= %d\n",rVdecInfo.eAspectRatio);\
            printf("rVdecInfo.u4BitRate= %d\n",(int)rVdecInfo.u4BitRate);\
            printf("rVdecInfo.ucAfd= %d\n",rVdecInfo.ucAfd);\
            printf("rVdecInfo.fgProgressiveSeq = %d\n",rVdecInfo.fgProgressiveSeq );\
         }\
    }while(0)

#define COPY_FROM_USER(ArgUsr, ArgKernel, ArgSize)                  \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,           \
                       ArgSize))                                    \
    {                                                               \
        printk("mtal_ioctl_mtvdec argument error\n");               \
        return MTR_ERR_INV;                                         \
    }

#define COPY_TO_USER(ArgUsr, ArgKernel,ArgSize)                         \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,                 \
                       ArgSize))                                        \
    {                                                                   \
        printk("mtal_ioctl_mtvdec argument error\n");                   \
        return MTR_ERR_INV;                                             \
    }

#define USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(arg, size)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   size))              \
    {                                                       \
        printk("mtal_ioctl_mtvdec argument error\n");     \
        return -1;                                          \
    }                                                       \

#define COPY_FROM_USER_ARG_SIZE(ArgUsr, ArgKernel, size)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_mtvdec argument error\n");      \
        return -1;                                          \
    }

#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel, type)    \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(type)))                               \
    {                                                       \
        printk("mtal_ioctl_mtvdec argument error\n");       \
        return -1;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtvdec argument error\n");     \
        return -1;                                          \
    }                                                       \

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtvdec argument error\n");      \
        return -1;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtvdec argument error\n");     \
        return -1;                                          \
    }                                                       \

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtvdec argument error\n");      \
        return -1;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_4ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_4ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtvdec argument error\n");     \
        return -1;                                          \
    }                                                       \

#define COPY_FROM_USER_4ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_4ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtvdec argument error\n");      \
        return -1;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_5ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_5ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtvdec argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \
    
#define COPY_FROM_USER_5ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_5ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtvdec argument error\n");      \
        return MTR_ERR_INV;                                          \
    }     \

// 20110801
#define USR_SPACE_ACCESS_VALIDATE_6ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_6ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtvdec argument error\n");     \
        return -1;                                          \
    }                                                       \

#define COPY_FROM_USER_6ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_6ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtvdec argument error\n");      \
        return -1;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_8ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_8ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtvdec argument error\n");     \
        return -1;                                          \
    }                                                       \

#define COPY_FROM_USER_8ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_8ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtvdec argument error\n");      \
        return -1;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        printk("mtal_ioctl_mtvdec argument error\n");     \
        return MTR_ERR_INV;                                          \
    }

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_mtvdec argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define COPY_TO_USER_ARG_SIZE(ArgUsr, ArgKernel,size)              \
        if (copy_to_user((void __user *)ArgUsr, ArgKernel,   \
                           size))          \
        {                                                       \
            printk("mtal_ioctl_mtdmx argument error\n");      \
            return MTR_ERR_INV;                                          \
        } \

#define COPY_TO_USER_6ARG(arg_user, arg_kernel) \
				COPY_TO_USER_ARG(arg_user, arg_kernel, MTAL_IOCTL_6ARG_T)

#define MTVDEC_NOTIFY_IFRAME        (1<<0)
#define MTVDEC_NOTIFY_THUMBNAIL     (1<<1)
#define MTVDEC_NOTIFY_DECODE_IFRAME (1<<2)
#define MTVDEC_NOTIFY_MW_IF         (1<<3)
#define CLI_CMD_LEN  256

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
// TODO, use ucEsId on _au1UserDataTmpBuf
static UINT8    _au1UserDataTmpBuf[VDEC_MAX_ES][MAX_USERDATA_TMPSIZE];
static BOOL     _afgPlayingIframe[VDEC_MAX_ES] = {0};
UINT32   _u4NotifyMask = 0;
static BOOL _fgNotifyLocking = 0;
static CRIT_STATE_T _rNotifyLock;
static VDP_THUMBNAIL_INFO_T _arDecodeIFrameDstInfo[VDEC_MAX_ES];

// for multi-process API lock
static HANDLE_T _hAPISema = 0;

#ifdef DUMP_YUV_FROM_KERNAL
static int inum = 0;
#endif

#ifdef IFRAME_MSGQ_TIMEOUT
static HANDLE_T _ahMtvdecTimer[VDEC_MAX_ES] = {0};
static HANDLE_T _ahMtvdecMsgQ[VDEC_MAX_ES] = {0};
#else
static UINT32 _au4IFrameResult[VDEC_MAX_ES] = {0};
static HANDLE_T _ahIframeSema[VDEC_MAX_ES] = {0};
#endif

static UCHAR* _apucCcBuf[VDEC_MAX_ES] = {0};
static UINT32 _au4CcBufSize[VDEC_MAX_ES] = {0};
static UCHAR* _apucIFrameBuf[VDEC_MAX_ES] = {0};
static UINT32 _au4IFrameBufSize[VDEC_MAX_ES] = {0};

UCHAR _aucMTVDECEsId[VDEC_MAX_ES] = {0};
static HANDLE_T _ahMTVDECVdecCtrlSema[VDEC_MAX_ES] = {0};

#ifdef ENABLE_MULTIMEDIA
extern MT_RESULT_T _MTPVR_SetSpeedExt(UCHAR ucPvrId, INT32 i4Speed, UINT32 u4StepCmd);
extern void _MTPVR_VDEC_Nfy(
    void*               pvNfyTag,
    VDEC_DEC_COND_T      eNfyCond,
    UINT32              u4Data1,
    UINT32              u4Data2);
#endif
#ifdef TIME_SHIFT_SUPPORT
extern BOOL _MTDREC_IsLivePidEnable(VOID);
#endif
extern INT32 CLI_Parser(const CHAR *szCmdBuf);

static BOOL _fgVdecPlayed[VDEC_MAX_ES] = {0};
#ifdef CC_BOOT_VDEC_LOGO
static BOOL _fgVdecBootLogoRead = FALSE;
static MPV_VDEC_LOGO_CTRL_UNION_T _rVdecBootLogo;
static UINT32 _u4Valid = 0;
static BOOL _fgVdecBootLogoAddrRead = FALSE;
static UINT8   _ucDataType;
static UINT32  _u4DataSize;
static UINT32  _u4DataAddr;
#endif

#ifdef MTVDEC_VIDEO_ES_INPUT_SUPPORT
static MTVDEC_INPUT_INFO_T _rVdecInputInfo;
static FBM_POOL_T *_prDmxFbmPool = NULL;
#endif // MTVDEC_VIDEO_ES_INPUT_SUPPORT

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

static void LockVdecSemaphoreMtal(void)
{
    VERIFY(x_sema_lock(_hAPISema, X_SEMA_OPTION_WAIT) == OSR_OK);
}

static void UnLockVdecSemaphoreMtal(void)
{
    VERIFY(x_sema_unlock(_hAPISema) == OSR_OK);
}

VDEC_3D_TYPE_T _MTVDEC_3D_TypeRevMap(VDEC_3D_TYPE_T eType)
{
	MTVDEC_3D_TYPE_T eMtType=MTVDEC_3D_NONE;

    switch (eType)
    {
    default:
    case VDEC_3D_MAX:
        eMtType = MTVDEC_3D_MAX;
        break;
    case VDEC_3D_NONE:
        eMtType = MTVDEC_3D_NONE;
        break;
    case VDEC_3D_SBS_LF:
        eMtType = MTVDEC_3D_SBS_LF;
        break; //side by side, left image first
    case VDEC_3D_SBS_RF:
        eMtType = MTVDEC_3D_SBS_RF;
        break; //side by side, right image first
    case VDEC_3D_TB_LF:
        eMtType = MTVDEC_3D_TB_LF;
        break;   //top bottom, left image first
    case VDEC_3D_TB_RF:
        eMtType = MTVDEC_3D_TB_RF;
        break;   //top bottom, right image first
    case VDEC_3D_DUAL_STREAM:
        eMtType = MTVDEC_3D_DUAL_STREAM;
        break;
    case VDEC_3D_MULTI_STREAM:
        eMtType = MTVDEC_3D_MULTI_STREAM;
        break;
    case VDEC_3D_MVC:
        eMtType = MTVDEC_3D_MVC;
        break;
    case VDEC_3D_REALD_DUAL_STREAM:
        eMtType = MTVDEC_3D_REALD_DUAL_STREAM;
        break;
    case VDEC_3D_CHECKER_BOARD:
        eMtType = MTVDEC_3D_CHECKER_BOARD;
        break;
    }
    return eMtType;
}


VDEC_3D_TYPE_T _MTVDEC_3D_TypeMap(MTVDEC_3D_TYPE_T eMtType)
{
    VDEC_3D_TYPE_T eType = VDEC_3D_NONE;

    switch (eMtType)
    {
    default:
    case MTVDEC_3D_MAX:
        eType = VDEC_3D_MAX;
        break;
    case MTVDEC_3D_NONE:
        eType = VDEC_3D_NONE;
        break;
    case MTVDEC_3D_SBS_LF:
        eType = VDEC_3D_SBS_LF;
        break; //side by side, left image first
    case MTVDEC_3D_SBS_RF:
        eType = VDEC_3D_SBS_RF;
        break; //side by side, right image first
    case MTVDEC_3D_TB_LF:
        eType = VDEC_3D_TB_LF;
        break;   //top bottom, left image first
    case MTVDEC_3D_TB_RF:
        eType = VDEC_3D_TB_RF;
        break;   //top bottom, right image first
    case MTVDEC_3D_DUAL_STREAM:
        eType = VDEC_3D_DUAL_STREAM;
        break;
    case MTVDEC_3D_MULTI_STREAM:
        eType = VDEC_3D_MULTI_STREAM;
        break;
    case MTVDEC_3D_MVC:
        eType = VDEC_3D_MVC;
        break;
    case MTVDEC_3D_REALD_DUAL_STREAM:
        eType = VDEC_3D_REALD_DUAL_STREAM;
        break;
    case MTVDEC_3D_CHECKER_BOARD:
        eType = VDEC_3D_CHECKER_BOARD;
        break;
    }

    return eType;
}


VDEC_MM_PARAM_T _MTVDEC_MM_ParamMap(MTVDEC_MM_PARAM_T eMtType)
{
    VDEC_MM_PARAM_T eType = VDEC_MM_MAX;

    switch (eMtType)
    {
    default:
    case MTVDEC_MM_MAX:
        eType = VDEC_MM_MAX;
        break;
    case MTVDEC_MM_PLAY_SPEED:
        eType = VDEC_MM_PLAY_SPEED;
        break;
    case MTVDEC_MM_FRAME_RATE:
        eType = VDEC_MM_FRAME_RATE;
        break;
    case MTVDEC_MM_CONTAINER_TYPE:
        eType = VDEC_MM_CONTAINER_TYPE;
        break;
    case MTVDEC_MM_SEQ_INFO:
        eType = VDEC_MM_SEQ_INFO;
        break;
    case MTVDEC_MM_SRC_TYPE:
        eType = VDEC_MM_SRC_TYPE;
        break;
    case MTVDEC_MM_TPT_SEQ_INFO:
        eType = VDEC_MM_TPT_SEQ_INFO;
        break;
    case MTVDEC_MM_RV_RPR_INFO:
        eType = VDEC_MM_RV_RPR_INFO;
        break;
    case MTVDEC_MM_3D_INFO:
        eType = VDEC_MM_3D_INFO;
        break;
    case MTVDEC_MM_3D_TRICK_TYPE:
        eType = VDEC_MM_3D_TRICK_TYPE;
        break;
    case MTVDEC_MM_DIVX4_SIGNAL:
        eType = VDEC_MM_DIVX4_SIGNAL;
        break;
    case MTVDEC_MM_XVID_SIGNAL:
        eType = VDEC_MM_XVID_SIGNAL;
        break;
    }

    return eType;
}

ENUM_VDEC_FMT_T _MTVDEC_FmtMap(MTVDEC_FMT_T eVdecFmt)
{
    ENUM_VDEC_FMT_T eFmt;

    switch(eVdecFmt)
    {
    case MTVDEC_FMT_MPEG1_MPEG2:
        eFmt = VDEC_FMT_MPV;
        break;
    case MTVDEC_FMT_MPEG4:
        eFmt = VDEC_FMT_MP4;
        break;
    case MTVDEC_FMT_H264:
        eFmt = VDEC_FMT_H264;
        break;
    case MTVDEC_FMT_VC1:
        eFmt = VDEC_FMT_WMV;
        break;
    case MTVDEC_FMT_RV:
        eFmt = VDEC_FMT_RV;
        break;
    case MTVDEC_FMT_MJPEG:
        eFmt = VDEC_FMT_MJPEG;
        break;
    case MTVDEC_FMT_AVS:
        eFmt = VDEC_FMT_AVS;
        break;
    case MTVDEC_FMT_VP6:
        eFmt = VDEC_FMT_VP6;
        break;
    case MTVDEC_FMT_VP8:
        eFmt = VDEC_FMT_VP8;
        break;
    case MTVDEC_FMT_RAW:
        eFmt = VDEC_FMT_RAW;
        break;
    case MTVDEC_FMT_OGG:
        eFmt = VDEC_FMT_OGG;
        break;
    case MTVDEC_FMT_H265:
        eFmt = VDEC_FMT_H265;
        break;
    default:
        eFmt = VDEC_FMT_MAX;
        break;
    }

    return eFmt;
}


void _MTVDEC_Notify_Lock(void)
{
    _rNotifyLock = x_crit_start();

    ASSERT(!_fgNotifyLocking);
    _fgNotifyLocking = TRUE;

    UNUSED(_fgNotifyLocking);          // Make Lint happy
}
void _MTVDEC_Notify_Unlock(void)
{
    ASSERT(_fgNotifyLocking);
    _fgNotifyLocking = FALSE;

    x_crit_end(_rNotifyLock);

    UNUSED(_fgNotifyLocking);          // Make Lint happy
}

#ifdef IFRAME_MSGQ_TIMEOUT
static void _MTVDEC_FlushDecMsgQ()
{
    UINT16 u2Cnt;
    UINT16 u2MsgQIdx;
    UINT32 u4DecFin;
    SIZE_T zMsgSize;

    zMsgSize = sizeof(INT32);

    while (x_msg_q_receive(&u2MsgQIdx, &u4DecFin, &zMsgSize,
        &_ahMtvdecMsgQ[ucEsId], 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
    {
    }

    VERIFY(x_msg_q_num_msgs(_ahMtvdecMsgQ[ucEsId], &u2Cnt) == OSR_OK);
    ASSERT(u2Cnt== 0);
}


void _MTVDEC_TimerTimeout(HANDLE_T  pt_tm_handle, VOID *pv_tag)
{
    UINT32 u4DecFin;
    INT32 i4Ret;

    if(_ahMtvdecMsgQ[ucEsId] == 0)
    {
        // MM playback detect PS case, i.e. MPV_Init() is not called.
        return;
    }

    u4DecFin = MTVDEC_TIMEOUT;
    i4Ret = x_msg_q_send(_ahMtvdecMsgQ[ucEsId], (void *)&u4DecFin, sizeof(UINT32), 255);

    if ((i4Ret != OSR_OK)&&(i4Ret != OSR_TOO_MANY))
    {
        ASSERT(0);
    }

    UNUSED(pt_tm_handle);
    UNUSED(pv_tag);
}
#endif

static void _MTVDEC_CC_Nfy(DGUSERDATA_CB_T* prDecCcData)
{
    _CB_PutEvent(CB_MTAL_VDEC_USRDATA_TRIGGER, sizeof(DGUSERDATA_CB_T), (void*)prDecCcData);

}

static void _MTVDEC_RmNotify(UCHAR ucEsId, MTVDEC_RM_COND_T eCond, void *pvData)
{
    MTVDEC_RM_CB_T rPara;

    MTVDEC_PRINT("_MTPVR_RmNotify ucEsId(%d) ucCond(%d).\n", ucEsId, eCond);
    
    rPara.u1EsId  = ucEsId;
    rPara.eCond   = eCond;

    switch (eCond)
    {
        case MTVDEC_RM_ALLOC_CFT:
            if (pvData != NULL)
            {
                x_memcpy((void *)rPara.u.aucCftEsId, pvData, sizeof(rPara.u.aucCftEsId));
            }
            break;
        case MTVDEC_RM_IMGRZ_REQ:
            if (pvData != NULL)
            {
                rPara.u.ucReqMoreNum = *(UCHAR *)pvData;
            }
            break;
        case MTVDEC_RM_ALLOC_OK:
        case MTVDEC_RM_ALLOC_NG: 
        default:
            break;
    }

    _CB_PutEvent(CB_MTAL_VDEC_RM_NFY, sizeof(MTVDEC_RM_CB_T), (void*)&rPara);
}

static void _MTVDEC_ERR_Nfy(DECERR_CB_T* prDecErr)
{
    _CB_PutEvent(CB_MTAL_VDEC_DECERR_TRIGGER, sizeof(DECERR_CB_T), (void*)prDecErr);

}

static void _MTVDEC_FRAME_Nfy(FRAME_CB_T* prFame)
{
    _CB_PutEvent(CB_MTAL_FRAME_TRIGGER, sizeof(FRAME_CB_T), (void*)prFame);
}

static void _MTVDEC_PICINFO_Nfy(PICINFO_CB_T* prPicInfo)
{
    prPicInfo->info3D=_MTVDEC_3D_TypeRevMap(prPicInfo->info3D);
    _CB_PutEvent(CB_MTAL_VDEC_PICINFO_NFY, sizeof(PICINFO_CB_T), (void*)prPicInfo);
}

static void _MTVDEC_STATUS_INT_Nfy(DEC_STATUS_CB_T* prStatus)
{
    _CB_PutEvent(CB_MTAL_VDEC_STATUS_TRIGGER, sizeof(DEC_STATUS_CB_T), (void*)prStatus);
}


static void _MTVDEC_VDEC_Iframe_Nfy(
    void*               pvNfyTag,
    VDEC_DEC_COND_T      eNfyCond,
    UINT32              u4Data1,
    UINT32              u4Data2)
{
    UCHAR ucEsId;
    #ifdef IFRAME_MSGQ_TIMEOUT
    UINT32 u4DecFin = 0;
    INT32 i4Ret;
    #endif

    if(pvNfyTag > 0)
    {
        ucEsId = *((UCHAR*)pvNfyTag);
    }
    else
    {
        MTVDEC_PRINT("pvNfyTag = 0\n");
        ASSERT(0);
        ucEsId = 0;
    }
    if(ucEsId >= VDEC_MAX_ES)
    {
        MTVDEC_PRINT("ucEsId(%d) > VDEC_MAX_ES\n");
        ASSERT(0);
        ucEsId = 0;
    }

    MTVDEC_PRINT("_MTVDEC_VDEC_Nfy\n");

    if((eNfyCond == VDEC_COND_I_FRAME_SET_BUFF_DONE) ||
        (eNfyCond == VDEC_COND_I_FRAME_DONE))
    {
        MTVDEC_PRINT(" - Vdec job done\n");
        #ifdef IFRAME_MSGQ_TIMEOUT
        u4DecFin = MTVDEC_FINISH;
        i4Ret = x_msg_q_send(_ahMtvdecMsgQ[ucEsId], (void *)&u4DecFin, sizeof(UINT32), 255);

        if ((i4Ret != OSR_OK)&&(i4Ret != OSR_TOO_MANY))
        {
            MTVDEC_PRINT(" - Vdec job done OSR_TOO_MANY\n");
            //ASSERT(0);
        }
        #else
        _au4IFrameResult[ucEsId] = MTVDEC_FINISH;
        VERIFY(x_sema_unlock(_ahIframeSema[ucEsId]) == OSR_OK);
        #endif
    }
    else if(eNfyCond == VDEC_COND_ERROR)
    {
        MTVDEC_PRINT(" - Vdec job error\n");
        #ifdef IFRAME_MSGQ_TIMEOUT
        u4DecFin = MTVDEC_ERROR;
        i4Ret = x_msg_q_send(_ahMtvdecMsgQ[ucEsId], (void *)&u4DecFin, sizeof(UINT32), 255);

        if ((i4Ret != OSR_OK)&&(i4Ret != OSR_TOO_MANY))
        {
            MTVDEC_PRINT(" - Vdec job error OSR_TOO_MANY\n");
            //ASSERT(0);
        }
        #else
        _au4IFrameResult[ucEsId] = MTVDEC_ERROR;
        VERIFY(x_sema_unlock(_ahIframeSema[ucEsId]) == OSR_OK);
        #endif
    }
}

static void _MTVDEC_VDEC_Decode_Iframe_Nfy(
    void*               pvNfyTag,
    VDEC_DEC_COND_T      eNfyCond,
    UINT32              u4Data1,
    UINT32              u4Data2)
{
    UCHAR ucEsId;
    #ifdef IFRAME_MSGQ_TIMEOUT
    UINT32 u4DecFin = 0;
    INT32 i4Ret;
    #endif

    if(pvNfyTag > 0)
    {
        ucEsId = *((UCHAR*)pvNfyTag);
    }
    else
    {
        MTVDEC_PRINT("pvNfyTag = 0\n");
        ASSERT(0);
        ucEsId = 0;
    }
    if(ucEsId >= VDEC_MAX_ES)
    {
        MTVDEC_PRINT("ucEsId(%d) > VDEC_MAX_ES\n");
        ASSERT(0);
        ucEsId = 0;
    }

    MTVDEC_PRINT("_MTVDEC_VDEC_Nfy\n");

    if((eNfyCond == VDEC_COND_I_FRAME_SET_BUFF_DONE) ||
        (eNfyCond == VDEC_COND_I_FRAME_DONE || eNfyCond == VDEC_COND_THUMBNAIL_DONE))
    {
        MTVDEC_PRINT(" - Vdec job done\n");
        #ifdef IFRAME_MSGQ_TIMEOUT
        u4DecFin = MTVDEC_FINISH;
        i4Ret = x_msg_q_send(_ahMtvdecMsgQ[ucEsId], (void *)&u4DecFin, sizeof(UINT32), 255);

        if ((i4Ret != OSR_OK)&&(i4Ret != OSR_TOO_MANY))
        {
            MTVDEC_PRINT(" - Vdec job done OSR_TOO_MANY\n");
            //ASSERT(0);
        }
        #else
        _au4IFrameResult[ucEsId] = MTVDEC_FINISH;
        VERIFY(x_sema_unlock(_ahIframeSema[ucEsId]) == OSR_OK);
        #endif
    }
    else if(eNfyCond == VDEC_COND_ERROR)
    {
        MTVDEC_PRINT(" - Vdec job error\n");
        #ifdef IFRAME_MSGQ_TIMEOUT
        u4DecFin = MTVDEC_ERROR;
        i4Ret = x_msg_q_send(_ahMtvdecMsgQ[ucEsId], (void *)&u4DecFin, sizeof(UINT32), 255);

        if ((i4Ret != OSR_OK)&&(i4Ret != OSR_TOO_MANY))
        {
            MTVDEC_PRINT(" - Vdec job error OSR_TOO_MANY\n");
            //ASSERT(0);
        }
        #else
        _au4IFrameResult[ucEsId] = MTVDEC_ERROR;
        VERIFY(x_sema_unlock(_ahIframeSema[ucEsId]) == OSR_OK);
        #endif
    }
}

#if 0
void _MTVDEC_VDEC_Decode_Thumb_Nfy(
    UINT32 u4Arg1,
    UINT32 u4Arg2,
    UINT32 u4Arg3,
    UINT32 u4Arg4)
{
    // TODO, actually this function is removed.
    UCHAR ucEsId = 0;

#ifdef IFRAME_MSGQ_TIMEOUT
    UINT32 u4DecFin = 0;
#endif
    if(u4Arg2 == VDEC_COND_THUMBNAIL_DONE)
    {
        MTVDEC_PRINT(" - Vdec job done\n");
        _au4IFrameResult[ucEsId] = MTVDEC_FINISH;
#ifdef IFRAME_MSGQ_TIMEOUT
        u4DecFin = MTVDEC_FINISH;
        VERIFY(x_msg_q_send(_ahMtvdecMsgQ[ucEsId], (void *)&u4DecFin, sizeof(UINT32), 1) == OSR_OK);
#else
        VERIFY(x_sema_unlock(_ahIframeSema[ucEsId]) == OSR_OK);
#endif
    }
    else
    {
        _au4IFrameResult[ucEsId] = MTVDEC_ERROR;
#ifdef IFRAME_MSGQ_TIMEOUT
        u4DecFin = MTVDEC_ERROR;
        VERIFY(x_msg_q_send(_ahMtvdecMsgQ[ucEsId], (void *)&u4DecFin, sizeof(UINT32), 1) == OSR_OK);
#else
        VERIFY (x_sema_unlock(_ahIframeSema[ucEsId]) == OSR_OK);
#endif
    }
}
#endif

static void _MTVDEC_GENERAL_Nfy(
    void*               pvNfyTag,
    VDEC_DEC_COND_T      eNfyCond,
    UINT32              u4Data1,
    UINT32              u4Data2
)
{
    DEC_GENERAL_CB_T rGeneral;
    x_memset(&rGeneral, 0, sizeof(DEC_GENERAL_CB_T));
    rGeneral.u4Tag = (*(UCHAR*)pvNfyTag);
    rGeneral.u4Condition = (UINT32)eNfyCond;
    rGeneral.u4Data1 = (UINT32)u4Data1;
    rGeneral.u4Data2 = (UINT32)u4Data2;
    MTVDEC_PRINT("Put Event(%d %d %d)\n", (UINT32)eNfyCond, u4Data1, u4Data2);
    _CB_PutEvent(CB_MTAL_VDEC_GENERAL_TRIGGER, sizeof(DEC_GENERAL_CB_T), (void*)&rGeneral);
}

static void _MTVDEC_CC2_Nfy(
    void*               pvNfyTag,
    UINT32              u4NfyCond,
    UINT32              u4Data1
)
{
    DEC_CC_CB_T rCc;
    x_memset(&rCc, 0, sizeof(DEC_CC_CB_T));
    rCc.u4Tag = (UINT32)pvNfyTag;
    rCc.u4Condition = (UINT32)u4NfyCond;
    rCc.u4Data1 = (UINT32)u4Data1;
    _CB_PutEvent(CB_MTAL_VDEC_CC_TRIGGER, sizeof(DEC_CC_CB_T), (void*)&rCc);
}

#ifdef MULTIMEDIA_AUTOTEST_SUPPORT
static void _MTVDEC_AUTOTEST_Nfy(
    void*               pvNfyTag,
    VDEC_DEC_COND_T      eNfyCond,
    UINT32              u4Data1,
    UINT32              u4Data2
)
{
    DEC_AUTOTEST_CB_T rAutotest;
    x_memset(&rAutotest, 0, sizeof(DEC_AUTOTEST_CB_T));
    rAutotest.u4Tag = (UINT32)pvNfyTag;
    rAutotest.u4Condition = (UINT32)eNfyCond;
    rAutotest.u4Data1 = (UINT32)u4Data1;
    rAutotest.u4Data2 = (UINT32)u4Data2;
    _CB_PutEvent(CB_MTAL_VDEC_AUTOTEST_TRIGGER, sizeof(DEC_AUTOTEST_CB_T), (void*)&rAutotest);
}
#endif

void _MTVDEC_VDEC_Nfy(
    void*               pvNfyTag,
    VDEC_DEC_COND_T      eNfyCond,
    UINT32              u4Data1,
    UINT32              u4Data2)
{
    UCHAR ucEsId = VDEC_MAX_ES;
    if(pvNfyTag > 0)
    {
        ucEsId = *((UCHAR*)pvNfyTag);
    }
    else
    {
        MTVDEC_PRINT("pvNfyTag = 0\n");
    }
    if (eNfyCond == VDEC_COND_CTRL_DONE)
    {
        if((ucEsId < VDEC_MAX_ES) && (_ahMTVDECVdecCtrlSema[ucEsId]))
        {
            VERIFY (x_sema_unlock(_ahMTVDECVdecCtrlSema[ucEsId]) == OSR_OK);
        }
        else
        {
            MTVDEC_PRINT("ucEsId(%d) >= VDEC_MAX_ES\n", ucEsId);
        }
    }

    MTVDEC_PRINT("Callback Notify = %d\n",_u4NotifyMask);

    if(_u4NotifyMask & MTVDEC_NOTIFY_IFRAME)
    {
        _MTVDEC_VDEC_Iframe_Nfy(pvNfyTag, eNfyCond, u4Data1, u4Data2);
    }
    if(_u4NotifyMask & MTVDEC_NOTIFY_THUMBNAIL)
    {
        #ifdef ENABLE_MULTIMEDIA
        _MTPVR_VDEC_Nfy(pvNfyTag, eNfyCond, u4Data1, u4Data2);
        #endif
    }
    if(_u4NotifyMask & MTVDEC_NOTIFY_DECODE_IFRAME)
    {
         _MTVDEC_VDEC_Decode_Iframe_Nfy(pvNfyTag, eNfyCond, u4Data1, u4Data2);
    }
    if(_u4NotifyMask & MTVDEC_NOTIFY_MW_IF)
    {
        _MTVDEC_GENERAL_Nfy(pvNfyTag, eNfyCond, u4Data1, u4Data2);
    }
}

static MT_RESULT_T _MTVDEC_Init(void)
{
    UINT32 i;
    VDEC_DEC_NFY_INFO_T rNfyInfo[VDEC_MAX_ES];
#ifdef IFRAME_MSGQ_TIMEOUT
    CHAR szBuf[16];
#endif
    MTVDEC_PRINT(" - void\n");
    //add driver implementation code here

    for(i=0;i<VDEC_MAX_ES;i++)
    {
#ifdef IFRAME_MSGQ_TIMEOUT
        if(_ahMtvdecMsgQ[i] == 0)
    {
            x_snprintf(szBuf, sizeof(szBuf), "MTVQ%d", i);
            VERIFY(x_msg_q_create(&_ahMtvdecMsgQ[i], szBuf, sizeof(UINT32), 5) == OSR_OK);
    }
        if(_ahMtvdecTimer[i] == 0)
    {
            VERIFY(x_timer_create(&_ahMtvdecTimer[i]) == OSR_OK);
    }
#else
        if(_ahIframeSema[i] == 0)
    {
            VERIFY(x_sema_create(&_ahIframeSema[i], X_SEMA_TYPE_BINARY,
            X_SEMA_STATE_LOCK) == OSR_OK);
    }
#endif
        if(_ahMTVDECVdecCtrlSema[i] == 0)
        {
            VERIFY(x_sema_create(&_ahMTVDECVdecCtrlSema[i],
                             X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
        }
        _aucMTVDECEsId[i] = i;
    }

    // for multi-process API lock
    if(_hAPISema == 0)
    {
        VERIFY(x_sema_create(&_hAPISema, X_SEMA_TYPE_MUTEX,
            X_SEMA_STATE_UNLOCK) == OSR_OK);
    }

    VDEC_Init();
    VDEC_RegRmCb((PFN_VDEC_RM_CB)_MTVDEC_RmNotify);

    for(i = 0; i < VDEC_MAX_ES; i++)
    {
        rNfyInfo[i].pvTag = &_aucMTVDECEsId[i];
        rNfyInfo[i].pfDecNfy = _MTVDEC_VDEC_Nfy;
        VDEC_SetDecNfy(_aucMTVDECEsId[i], &rNfyInfo[i]);
    }

    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_Termint(void)
{
    LockVdecSemaphoreMtal();

    MTVDEC_PRINT(" - void\n");
    //add driver implementation code here
    VDEC_Termint();

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_Reset(void)
{
    LockVdecSemaphoreMtal();

    MTVDEC_PRINT(" - void\n");
    //add driver implementation code here

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_Play(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    MTVDEC_FMT_T eMtVdecFmt;
    ENUM_VDEC_FMT_T eVdecFmt;
    VDEC_DEC_NFY_INFO_T rNfyInfo;
    UCHAR ucEsId;
    BOOL fgRet;
    #ifdef CC_KR_3DDTV
    UINT8 u1Idx;
    DMX_PID_T rPid;
    #endif

    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    eMtVdecFmt = (MTVDEC_FMT_T)rArg.ai4Arg[1];

    MTVDEC_PRINT(" - ucEsId = %d, eMtVdecFmt = %d\n",
        (int)ucEsId,
        (int)eMtVdecFmt);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }

    #if 0//need it?
    // set to normal speed
    VDP_SetSpeed(B2R_1, STC_SPEED_TYPE_FORWARD_1X);
    VDP_SetFBNoTimeout(B2R_1, FALSE);
    VDP_SetStepForward(B2R_1, VDP_PLAY_NORMAL);
    VDP_SetPauseMm(B2R_1, FALSE);
    VDP_SetTrickMode(B2R_1,VDP_PLAY_NORMAL);
    #endif

    eVdecFmt = _MTVDEC_FmtMap(eMtVdecFmt);
    if (VDEC_FMT_MPV == eVdecFmt)
    {
        //DMX_SetVideoType(DMX_VIDEO_MPEG);

        #if 1//#ifdef CC_KR_3DDTV
        for (u1Idx = 0; u1Idx < DMX_NUM_PID_INDEX; u1Idx++)
        {
            // Get PID info
            if (DMX_GetPid(u1Idx, DMX_PID_FLAG_BUFFER, &rPid))
            {
                if((rPid.u1ChannelId == ucEsId) && (rPid.ePidType == DMX_PID_TYPE_ES_VIDEO))
                {
                    DMX_SetVideoTypeByPidx(u1Idx, DMX_VIDEO_MPEG);
                    break;
                }
            }
        }
        #endif
    }
    else if (VDEC_FMT_H264 == eVdecFmt)
    {
        //DMX_SetVideoType(DMX_VIDEO_H264);

        #if 1//#ifdef CC_KR_3DDTV
        for (u1Idx = 0; u1Idx < DMX_NUM_PID_INDEX; u1Idx++)
        {
            // Get PID info
            if (DMX_GetPid(u1Idx, DMX_PID_FLAG_BUFFER, &rPid))
            {
                if((rPid.u1ChannelId == ucEsId) && (rPid.ePidType == DMX_PID_TYPE_ES_VIDEO))
                {
                    DMX_SetVideoTypeByPidx(u1Idx, DMX_VIDEO_H264);
                    break;
                }
            }
        }
        #endif
    }
    else if (VDEC_FMT_AVS == eVdecFmt)
    {
        //DMX_SetVideoType(DMX_VIDEO_AVS);

        #if 1//#ifdef CC_KR_3DDTV
        for (u1Idx = 0; u1Idx < DMX_NUM_PID_INDEX; u1Idx++)
        {
            // Get PID info
            if (DMX_GetPid(u1Idx, DMX_PID_FLAG_BUFFER, &rPid))
            {
                if((rPid.u1ChannelId == ucEsId) && (rPid.ePidType == DMX_PID_TYPE_ES_VIDEO))
                {
                    DMX_SetVideoTypeByPidx(u1Idx, DMX_VIDEO_AVS);
                    break;
                }
            }
        }
        #endif
    }
	else if (VDEC_FMT_H265 == eVdecFmt)
    {
        //DMX_SetVideoType(DMX_VIDEO_AVS);

        #if 1//#ifdef CC_KR_3DDTV
        for (u1Idx = 0; u1Idx < DMX_NUM_PID_INDEX; u1Idx++)
        {
            // Get PID info
            if (DMX_GetPid(u1Idx, DMX_PID_FLAG_BUFFER, &rPid))
            {
                if((rPid.u1ChannelId == ucEsId) && (rPid.ePidType == DMX_PID_TYPE_ES_VIDEO))
                {
                    DMX_SetVideoTypeByPidx(u1Idx, DMX_VIDEO_H265);
                    break;
                }
            }
        }
        #endif
    }
	
    if(_fgVdecPlayed[ucEsId] == FALSE)
    {
        rNfyInfo.pvTag = &_aucMTVDECEsId[ucEsId];
        rNfyInfo.pfDecNfy = _MTVDEC_VDEC_Nfy;
        VDEC_SetDecNfy(ucEsId, &rNfyInfo);
    }
    _fgVdecPlayed[ucEsId] = TRUE;

    // here is hard code to for test
    // u can open here to see the result.
    // and follow the setting.
    #if 0
    if(_MTPVR_QueryPlayMode(0) == FALSE)
    {
        // this is _MTPVR_ResetBuffer
        _MTPVR_ResetBufferEx(0);
        // this is _MTVDEC_SetFlushEsData
        MPV_FlushEsmQ(0, FALSE);
    }
    #endif

    fgRet = VDEC_Play(ucEsId, eVdecFmt);

    #ifdef CC_KR_3DDTV_HARDCODE
    if(ucEsId == ES1)
    {
        VDEC_SetMMParam(0, VDEC_MM_3D_INFO, VDEC_3D_DUAL_STREAM, 0, 0);
        VDEC_SetMMParam(1, VDEC_MM_3D_INFO, VDEC_3D_DUAL_STREAM, 0, 0);
    }
    #endif

    if (fgRet)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_OK;
    }
    else
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
}


static MT_RESULT_T _MTVDEC_PlayIFrame(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T    rArg;
    MTVDEC_FMT_T eVdecFmt;
    UCHAR ucEsId;
    //BOOL fgRet;
    UCHAR *pucData;
    UINT32 u4Size;
    INT32 i4Ret;
    MPV_DEC_I_FRAME_BUFF_T rMpvBuf;
    VDEC_DEC_NFY_INFO_T rNfyInfo;
    FBM_POOL_T *prFbmPool;

    #ifdef IFRAME_MSGQ_TIMEOUT
    UINT16 u2MsgQIdx;
    UINT32 u4DecFin;
    SIZE_T zMsgSize = sizeof(INT32);
    #endif

    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    eVdecFmt = (MTVDEC_FMT_T)rArg.ai4Arg[1];
    pucData = (UCHAR*)rArg.ai4Arg[2];
    u4Size = (UINT32)rArg.ai4Arg[3];

    MTVDEC_PRINT(" - ucEsId = %d, eVdecFmt = %d\n",
        (int)ucEsId,
        (int)eVdecFmt);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        MTVDEC_PRINT(" - ucEsId = %d, _MTVDEC_PlayIFrame fail\n",(int)ucEsId);
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    #if 0//need it?
    // set to normal speed
    VDP_SetSpeed(B2R_1, STC_SPEED_TYPE_FORWARD_1X);
    VDP_SetFBNoTimeout(B2R_1, FALSE);
    VDP_SetStepForward(B2R_1, VDP_PLAY_NORMAL);
    VDP_SetPauseMm(B2R_1, FALSE);
    VDP_SetTrickMode(B2R_1,VDP_PLAY_NORMAL);
    #endif

    _afgPlayingIframe[ucEsId] = TRUE;

    rNfyInfo.pvTag = &_aucMTVDECEsId[ucEsId];
    rNfyInfo.pfDecNfy = _MTVDEC_VDEC_Nfy;

    _MTVDEC_Notify_Lock();
    _u4NotifyMask |= MTVDEC_NOTIFY_IFRAME;
    _MTVDEC_Notify_Unlock();
    VDEC_SetDecNfy(ucEsId, &rNfyInfo);

    if(eVdecFmt == MTVDEC_FMT_MPEG1_MPEG2)
    {
        VDEC_Play_I_Frame(ucEsId, VDEC_FMT_MPV);
    }
    else if(eVdecFmt == MTVDEC_FMT_H264)
    {
        VDEC_Play_I_Frame(ucEsId, VDEC_FMT_H264);
    }
    else
    {
        MTVDEC_PRINT(" - eVdecFmt = %d, not support\n",(int)eVdecFmt);
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }

    //MPV_AllocIFrameBuf();
    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_FEEDER);
    rMpvBuf.pucIFrameBufAddr = (UCHAR*)(prFbmPool->u4Addr + pucData);
    rMpvBuf.u4Size = u4Size;

    // set write pointer
    VDEC_SetWptr(ucEsId, (UINT32)(rMpvBuf.pucIFrameBufAddr + rMpvBuf.u4Size));

    // clear lock
    #ifdef IFRAME_MSGQ_TIMEOUT
    _MTVDEC_FlushDecMsgQ();
    #else
    _au4IFrameResult[ucEsId] = MTVDEC_TIMEOUT;
    x_sema_lock(_ahIframeSema[ucEsId], X_SEMA_OPTION_NOWAIT);
    #endif

    // parsing
    MTVDEC_PRINT(" - MPV_SetIFrameBuf\n");
    i4Ret = MPV_SetIFrameBuf(ucEsId, &rMpvBuf);

    if(i4Ret != MPV_DRV_OK)
    {
        MTVDEC_PRINT(" - ucEsId = %d, MPV_SetIFrameBuf fail, 0x%x 0x%x 0x%x 0x%x\n",
            (int)ucEsId,
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+1))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+2))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+3))));

        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }

    MTVDEC_PRINT(" - Wait Vdec parsing\n");

    #ifdef IFRAME_MSGQ_TIMEOUT
    VERIFY(x_timer_start(_ahMtvdecTimer[ucEsId], IFRAME_TIMEOUT, X_TIMER_FLAG_ONCE,
            _MTVDEC_TimerTimeout, (void*)(UINT32)0) == OSR_OK);
    VERIFY(x_msg_q_receive(&u2MsgQIdx, &u4DecFin, &zMsgSize,
        &(_ahMtvdecMsgQ[ucEsId]), 1, X_MSGQ_OPTION_WAIT) == OSR_OK);
    VERIFY(x_timer_stop(_ahMtvdecTimer[ucEsId]) == OSR_OK);

    if (u4DecFin != MTVDEC_FINISH)
    {
        MTVDEC_PRINT(" - Wait Vdec parsing error, 0x%x 0x%x 0x%x 0x%x\n",
            (int)u4DecFin,
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+1))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+2))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+3))));

        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
    #else
    if(x_sema_lock_timeout(_ahIframeSema[ucEsId], IFRAME_TIMEOUT) != OSR_OK)
    {
        MTVDEC_PRINT(" - Wait Vdec parsing timeout, 0x%x 0x%x 0x%x 0x%x\n",
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+1))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+2))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+3))));

        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
    if(_au4IFrameResult[ucEsId] != MTVDEC_FINISH)
    {
        MTVDEC_PRINT(" - Wait Vdec parsing error %d, 0x%x 0x%x 0x%x 0x%x\n",
            (int)_au4IFrameResult[ucEsId],
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+1))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+2))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+3))));

        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
    #endif

    if(!_afgPlayingIframe[ucEsId])
    {
        MTVDEC_PRINT(" - _afgPlayingIframe[%d] 1 FALSE\n", ucEsId);

        UnLockVdecSemaphoreMtal();
        return MTR_DONT_CARE;
    }

    // Todo: get resolution and set backend

    // decoding
    MTVDEC_PRINT(" - MPV_SetIFrameDec\n");
    i4Ret = MPV_SetIFrameDec(ucEsId, &rMpvBuf);

    if(i4Ret != MPV_DRV_OK)
    {
        MTVDEC_PRINT(" - ucEsId = %d, MPV_SetIFrameDec fail\n",(int)ucEsId);

        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }

    MTVDEC_PRINT(" - Wait Vdec decoding\n");
    #ifdef IFRAME_MSGQ_TIMEOUT
    VERIFY(x_timer_start(_ahMtvdecTimer[ucEsId], IFRAME_TIMEOUT, X_TIMER_FLAG_ONCE,
            _MTVDEC_TimerTimeout, (void*)(UINT32)0) == OSR_OK);
    VERIFY(x_msg_q_receive(&u2MsgQIdx, &u4DecFin, &zMsgSize,
        &(_ahMtvdecMsgQ[ucEsId]), 1, X_MSGQ_OPTION_WAIT) == OSR_OK);
    VERIFY(x_timer_stop(_ahMtvdecTimer[ucEsId]) == OSR_OK);

    if (u4DecFin != MTVDEC_FINISH)
    {
        MTVDEC_PRINT(" - Wait Vdec parsing error %d\n", u4DecFin);

        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
    #else
    if(x_sema_lock_timeout(_ahIframeSema[ucEsId], IFRAME_TIMEOUT) != OSR_OK)
    {
        MTVDEC_PRINT(" - Wait Vdec decoding timeout\n");

        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
    if(_au4IFrameResult[ucEsId] != MTVDEC_FINISH)
    {
        MTVDEC_PRINT(" - Wait Vdec parsing error %d\n", (int)_au4IFrameResult[ucEsId]);

        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
    #endif

    if(!_afgPlayingIframe[ucEsId])
    {
        MTVDEC_PRINT(" - _afgPlayingIframe[%d] 2 FALSE\n", ucEsId);

        UnLockVdecSemaphoreMtal();
        return MTR_DONT_CARE;
    }

    //MPV_FreeIFrameBuf();
    MTVDEC_PRINT(" - _MTVDEC_PlayIFrame return\n");

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_QureyPoolInfo(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T   rArg;
    UCHAR ucPoolType;
    //UCHAR ucPoolKernelType;
    UCHAR ucEsId;
    UINT32 u4PoolAddr;
    UINT32 u4PoolSize;
    UINT32* pu4Addr;
    UINT32* pu4Size;
    FBM_POOL_T *prFbmPool;

    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    // parameters translation
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    ucPoolType = (UCHAR)rArg.ai4Arg[1];
    pu4Addr = (UINT32*)rArg.ai4Arg[2];
    pu4Size = (UINT32*)rArg.ai4Arg[3];

    if(ucEsId >= VDEC_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    // type translation
    switch(ucPoolType)
    {
        case USR_FBM_POOL_TYPE_PVR2:
            prFbmPool = FBM_GetPoolInfo(FBM_POOL_TYPE_PVR2);
            break;
        case USR_FBM_POOL_TYPE_MPEG:
            prFbmPool = FBM_GetPoolInfo(FBM_POOL_TYPE_MPEG);
            break;
        case USR_FBM_POOL_TYPE_DMX:
            prFbmPool = FBM_GetPoolInfo(FBM_POOL_TYPE_DMX);
            break;
        case USR_FBM_POOL_TYPE_FEEDER:
            prFbmPool = FBM_GetPoolInfo(FBM_POOL_TYPE_FEEDER);
            break;
        default:
            // should not happened..
            prFbmPool = NULL;
            break;
    }
    #if 0
    prFbmPool = FBM_GetPoolInfo(ucPoolKernelType);
    #endif

    ASSERT(prFbmPool != (FBM_POOL_T *)NULL);
    ASSERT(prFbmPool->u4Addr != 0);

    u4PoolAddr = prFbmPool->u4Addr;
    u4PoolSize = prFbmPool->u4Size;


    #if 0
    #if defined(CC_FBM_INT_TWO_FBP)
    #endif
    #if defined(CC_FBM_TWO_FBP_SHARED_WITH_DFB)
    #endif
    #endif

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Addr,UINT32);
    COPY_TO_USER_ARG(pu4Addr,u4PoolAddr,UINT32);
    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Size,UINT32);
    COPY_TO_USER_ARG(pu4Size,u4PoolSize,UINT32);

    UnLockVdecSemaphoreMtal();

    return MTR_OK;
}

static MT_RESULT_T _MTVDEC_DecodeIFrame(unsigned long arg)
{
    MTAL_IOCTL_6ARG_T    rArg;
    INT32 i4Ret;

    UCHAR ucEsId;
    MTVDEC_FMT_T eVdecFmt;
    VDEC_DEC_NFY_INFO_T rNfyInfo;
    UCHAR *pucData;
    UINT32 u4Size;
    UINT32 *pu4DestSize;
    UINT32 u4DestSize;
    UINT32 *pu4Width;

    UCHAR *pDstBuffer;
    UINT32 u4Width;
    UINT32 u4Height;
    UINT32 u4Pitch;
    UINT32 u4Line, u4Hpos;
    VDEC_HDR_INFO_T rHdrInfo;

    // fb buffer
    MPV_DEC_I_FRAME_BUFF_T rMpvBuf;
    FBM_POOL_T *prFbmPool_input;
    FBM_POOL_T *prFbmPool_output;
    UINT32 u4OutputFbgAddr;
    UINT32 u4InputFbgAddr;

    #ifdef IFRAME_MSGQ_TIMEOUT
    UINT16 u2MsgQIdx;
    UINT32 u4DecFin;
    SIZE_T zMsgSize = sizeof(INT32);
    #endif

    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_6ARG(arg);
    COPY_FROM_USER_6ARG(arg, rArg);

    // parameters translation
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    eVdecFmt = (MTVDEC_FMT_T)rArg.ai4Arg[1];
    pucData = (UCHAR*)rArg.ai4Arg[2];
    u4Size = (UINT32)rArg.ai4Arg[3];
    pu4DestSize = (UINT32*)rArg.ai4Arg[4];
    pu4Width = (UINT32*)rArg.ai4Arg[5];

    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }

    rNfyInfo.pvTag = &_aucMTVDECEsId[ucEsId];
    rNfyInfo.pfDecNfy = _MTVDEC_VDEC_Nfy;

    // set notify bit mask
    // use VDEC_SetDecNfy to notify result
    //VDEC_RegEventNotifyFunc(_MTVDEC_VDEC_Decode_Thumb_Nfy);
    _MTVDEC_Notify_Lock();
    _u4NotifyMask |= MTVDEC_NOTIFY_DECODE_IFRAME;
    _MTVDEC_Notify_Unlock();
    VDEC_SetDecNfy(ucEsId, &rNfyInfo);

    // disable display,
    VDEC_PlayNotDisplay(ucEsId,TRUE);

    if(eVdecFmt == MTVDEC_FMT_MPEG1_MPEG2)
    {
        VDEC_Play_I_Frame(ucEsId, VDEC_FMT_MPV);
    }
    else if(eVdecFmt == MTVDEC_FMT_H264)
    {
        VDEC_Play_I_Frame(ucEsId, VDEC_FMT_H264);
    }
    else
    {
        MTVDEC_PRINT(" - eVdecFmt = %d, not support\n",(int)eVdecFmt);
        MTVDEC_PRINT(" - eVdecFmt = %d, not support\n");
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }

    // input buffer
    prFbmPool_input = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_FEEDER);

    // output buffer
    prFbmPool_output = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG);

    u4InputFbgAddr = (UINT32)((prFbmPool_input->u4Addr)) ;
    rMpvBuf.pucIFrameBufAddr = (UCHAR*)(u4InputFbgAddr);
    rMpvBuf.u4Size = u4Size;

    // set write pointer
    VDEC_SetWptr(ucEsId, (UINT32)(rMpvBuf.pucIFrameBufAddr + rMpvBuf.u4Size));

    // clear lock
    #ifdef IFRAME_MSGQ_TIMEOUT
    _MTVDEC_FlushDecMsgQ();
    #else
    _au4IFrameResult[ucEsId] = MTVDEC_TIMEOUT;
    x_sema_lock(_ahIframeSema[ucEsId], X_SEMA_OPTION_NOWAIT);
    #endif

    // parsing
    MTVDEC_PRINT(" - MPV_SetIFrameBuf\n");
    i4Ret = MPV_SetIFrameBuf(ucEsId, &rMpvBuf);

    if(i4Ret != MPV_DRV_OK)
    {
        MTVDEC_PRINT(" - ucEsId = %d, MPV_SetIFrameBuf fail, 0x%x 0x%x 0x%x 0x%x\n",
            (int)ucEsId,
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+1))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+2))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+3))));

        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }

    MTVDEC_PRINT(" - Wait Vdec parsing\n");

    #ifdef IFRAME_MSGQ_TIMEOUT
    VERIFY(x_timer_start(_ahMtvdecTimer[ucEsId], IFRAME_TIMEOUT, X_TIMER_FLAG_ONCE,
            _MTVDEC_TimerTimeout, (void*)(UINT32)0) == OSR_OK);
    VERIFY(x_msg_q_receive(&u2MsgQIdx, &u4DecFin, &zMsgSize,
        &(_ahMtvdecMsgQ[ucEsId]), 1, X_MSGQ_OPTION_WAIT) == OSR_OK);
    VERIFY(x_timer_stop(_ahMtvdecTimer[ucEsId]) == OSR_OK);

    if (u4DecFin != MTVDEC_FINISH)
    {
        MTVDEC_PRINT(" - Wait Vdec parsing error, 0x%x 0x%x 0x%x 0x%x\n",
            (int)u4DecFin,
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+1))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+2))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+3))));

        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
    #else

    if(x_sema_lock_timeout(_ahIframeSema[ucEsId], IFRAME_TIMEOUT) != OSR_OK)
    {
         MTVDEC_PRINT(" - Wait Vdec parsing timeout, 0x%x 0x%x 0x%x 0x%x\n",
             (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr))),
             (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+1))),
             (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+2))),
             (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+3))));

         UnLockVdecSemaphoreMtal();
         return MTR_NOT_OK;
    }
    if(_au4IFrameResult[ucEsId] != MTVDEC_FINISH)
    {
        MTVDEC_PRINT(" - !!Wait Vdec parsing error %d, 0x%x 0x%x 0x%x 0x%x\n",
            (int)_au4IFrameResult[ucEsId],
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+1))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+2))),
            (int)(*(UCHAR*)VIRTUAL((UINT32)(rMpvBuf.pucIFrameBufAddr+3))));

        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
    #endif

    // decoding
    MTVDEC_PRINT("MPV_SetIFrameDec\n");

    i4Ret = MPV_SetIFrameDec(ucEsId, &rMpvBuf);

    if(i4Ret != MPV_DRV_OK)
    {
        MTVDEC_PRINT(" - ucEsId = %d, MPV_SetIFrameDec fail\n",(int)ucEsId);
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }

    MTVDEC_PRINT("Wait Vdec decoding\n");

    #ifdef IFRAME_MSGQ_TIMEOUT

    VERIFY(x_timer_start(_ahMtvdecTimer[ucEsId], IFRAME_TIMEOUT, X_TIMER_FLAG_ONCE,
            _MTVDEC_TimerTimeout, (void*)(UINT32)0) == OSR_OK);
    VERIFY(x_msg_q_receive(&u2MsgQIdx, &u4DecFin, &zMsgSize,
        &(_ahMtvdecMsgQ[ucEsId]), 1, X_MSGQ_OPTION_WAIT) == OSR_OK);
    VERIFY(x_timer_stop(_ahMtvdecTimer[ucEsId]) == OSR_OK);

    if (u4DecFin != MTVDEC_FINISH)
    {
        MTVDEC_PRINT(" - Wait Vdec decoding error %d\n", u4DecFin);
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
    #else

    if(x_sema_lock_timeout(_ahIframeSema[ucEsId], IFRAME_TIMEOUT) != OSR_OK)
    {
        MTVDEC_PRINT(" - Wait Vdec decoding timeout\n");
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }

    if(_au4IFrameResult[ucEsId] != MTVDEC_FINISH)
    {
        MTVDEC_PRINT(" - Wait Vdec decoding error %d\n", (int)_au4IFrameResult[ucEsId]);
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
    #endif

    // get seq header infomation
    VDEC_QueryInfo(ucEsId, &rHdrInfo);
    u4Pitch = rHdrInfo.u2Width;
    u4Width = rHdrInfo.u2Width;
    u4Height = rHdrInfo.u2Height;

    u4OutputFbgAddr = (UINT32)(PAGE_ALIGN(prFbmPool_output->u4Addr) +((prFbmPool_output->u4Size) >> 1));

    MTVDEC_PRINT("ADDR of CanvasBuffer ---> %d \n",u4OutputFbgAddr + DECODER_OUTPUT_OFFSET);

    if((u4Pitch*u4Height*4) > MTDECODE_CANVAS_SIZE)
    {
        MTVDEC_PRINT("%s buffer size not enought, u4Pitch(%d) u4Height(%d)\n",
            __FUNCTION__, u4Pitch, u4Height);

        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }

    {
        x_memset((VOID*)VIRTUAL((u4OutputFbgAddr + DECODER_OUTPUT_OFFSET)),
            0, MTDECODE_CANVAS_SIZE);
        x_memset(&_arDecodeIFrameDstInfo[ucEsId], 0, sizeof(VDP_THUMBNAIL_INFO_T));
        _arDecodeIFrameDstInfo[ucEsId].eCanvasColormode = VDP_COLORMODE_ARGB_D8888;
        _arDecodeIFrameDstInfo[ucEsId].pucCanvasBuffer = (UCHAR*)(u4OutputFbgAddr + DECODER_OUTPUT_OFFSET);
        _arDecodeIFrameDstInfo[ucEsId].u4CanvasHeight = u4Height;
        _arDecodeIFrameDstInfo[ucEsId].u4CanvasPitch = u4Pitch * 4;
        _arDecodeIFrameDstInfo[ucEsId].u4CanvasWidth = u4Width * 4;
        _arDecodeIFrameDstInfo[ucEsId].u4ThumbnailHeight = u4Height;
        _arDecodeIFrameDstInfo[ucEsId].u4ThumbnailWidth = u4Width;
        _arDecodeIFrameDstInfo[ucEsId].u4ThumbnailX = 0; // offset
        _arDecodeIFrameDstInfo[ucEsId].u4ThumbnailY = 0;

        // clean thumbanil
        pDstBuffer = _arDecodeIFrameDstInfo[ucEsId].pucCanvasBuffer;

        for(u4Line=0;u4Line<_arDecodeIFrameDstInfo[ucEsId].u4ThumbnailHeight;u4Line++)
        {
            for(u4Hpos=0;u4Hpos<_arDecodeIFrameDstInfo[ucEsId].u4ThumbnailWidth;u4Hpos++)
            {
                *(((UINT32 *)((VOID*)VIRTUAL((UINT32)pDstBuffer)+0))+u4Hpos)=0xff000000;
            }
            pDstBuffer+=_arDecodeIFrameDstInfo[ucEsId].u4CanvasPitch;
        }
        if(x_sema_lock(_ahIframeSema[ucEsId], X_SEMA_OPTION_NOWAIT) != OSR_WOULD_BLOCK)
        {
            MTVDEC_PRINT(" - Wait Vdec thumbnail error \n" );
        }


        VDEC_GetVideoThumbnail3(ucEsId, 0, FALSE,
            &_arDecodeIFrameDstInfo[ucEsId], FALSE);
    }

    MTVDEC_PRINT(" - Wait VDEC_GetVideoThumbnail\n");

    #ifdef IFRAME_MSGQ_TIMEOUT

    VERIFY(x_timer_start(_ahMtvdecTimer[ucEsId], IFRAME_TIMEOUT, X_TIMER_FLAG_ONCE,
            _MTVDEC_TimerTimeout, (void*)(UINT32)0) == OSR_OK);
    VERIFY(x_msg_q_receive(&u2MsgQIdx, &u4DecFin, &zMsgSize,
        &(_ahMtvdecMsgQ[ucEsId]), 1, X_MSGQ_OPTION_WAIT) == OSR_OK);
    VERIFY(x_timer_stop(_ahMtvdecTimer[ucEsId]) == OSR_OK);

    if (u4DecFin != MTVDEC_FINISH)
    {
        MTVDEC_PRINT(" - Wait Vdec thumbnail error %d\n", u4DecFin);

        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
    #else

    if(x_sema_lock_timeout(_ahIframeSema[ucEsId], IFRAME_TIMEOUT) != OSR_OK)
    {
        MTVDEC_PRINT(" - Wait Vdec thumbnail timeout\n");
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }

    if(_au4IFrameResult[ucEsId] != MTVDEC_FINISH)
    {
        MTVDEC_PRINT(" - Wait Vdec thumbnail error %d\n", (int)_au4IFrameResult[ucEsId]);
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
    #endif

    // fill feedback
    u4DestSize = u4Width * u4Height * 4;

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4DestSize,UINT32);
    COPY_TO_USER_ARG(pu4DestSize,u4DestSize,UINT32);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Width,UINT32);
    COPY_TO_USER_ARG(pu4Width,u4Width,UINT32);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

static MT_RESULT_T _MTVDEC_Pause(unsigned long arg)
{
    UCHAR ucEsId;
    ucEsId = (UCHAR)arg;

    LockVdecSemaphoreMtal();

    MTVDEC_PRINT(" - ucEsId = %d\n",(int)ucEsId);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    VDEC_Pause(ucEsId);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

MT_RESULT_T _MTVDEC_StopEx(UCHAR ucEsId)
{
    INT32 i4Ret;
    VDEC_DEC_NFY_INFO_T rNfyInfo;

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    _fgVdecPlayed[ucEsId] = FALSE;

    // clear lock
#ifndef MPV_MW_LINUX
    x_sema_lock(_ahMTVDECVdecCtrlSema[ucEsId], X_SEMA_OPTION_NOWAIT);
#endif

    // prevent callback being hooked to others
    rNfyInfo.pvTag = &_aucMTVDECEsId[ucEsId];
    rNfyInfo.pfDecNfy = _MTVDEC_VDEC_Nfy;
    VDEC_SetDecNfy(ucEsId, &rNfyInfo);

    VDEC_Stop(ucEsId);

    if(_afgPlayingIframe[ucEsId])
    {
        _afgPlayingIframe[ucEsId] = FALSE;
        #ifdef IFRAME_MSGQ_TIMEOUT
        _MTVDEC_TimerTimeout(0, 0);
        #else
        x_sema_unlock(_ahIframeSema[ucEsId]);
        #endif
    }

    // wait ctrl done
#ifndef MPV_MW_LINUX
    i4Ret = x_sema_lock_timeout(_ahMTVDECVdecCtrlSema[ucEsId], 5000);
    if (i4Ret != OSR_OK)
    {
        MTVDEC_PRINT("_ahMTVDECVdecCtrlSema timeout\n");
    }
#endif

    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_Stop(unsigned long arg)
{
    UCHAR ucEsId;
    ucEsId = (UCHAR)arg;

    LockVdecSemaphoreMtal();
    MTVDEC_PRINT(" - ucEsId = %d\n",(int)ucEsId);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }

    if(_MTVDEC_StopEx(ucEsId) == MTR_OK)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_OK;
    }
    else
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
}

static MT_RESULT_T _MTVDEC_GetInfo(unsigned long arg)
{
    MTVDEC_HDR_INFO_T*   prVdecInfo;
    MTAL_IOCTL_2ARG_T    rArg;
    VDEC_HDR_INFO_T rHdrInfo;

    UCHAR ucEsId;
    MTVDEC_HDR_INFO_T    rVdecInfo;

    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId =(UCHAR)rArg.ai4Arg[0];
    prVdecInfo = (MTVDEC_HDR_INFO_T*)rArg.ai4Arg[1];
    x_memset(&rVdecInfo, 0, sizeof(MTVDEC_HDR_INFO_T));

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }

    if (VDEC_QueryInfo(ucEsId, &rHdrInfo) == FALSE)
    {
        USR_SPACE_ACCESS_VALIDATE_ARG(prVdecInfo, MTVDEC_HDR_INFO_T );
        COPY_TO_USER_ARG(prVdecInfo, rVdecInfo, MTVDEC_HDR_INFO_T );
        UnLockVdecSemaphoreMtal();
        return MTR_OK;
    }

    rVdecInfo.fgHdrInvalid = rHdrInfo.fgHdrInvalid;
    rVdecInfo.fgConstrainedParameterFlag = rHdrInfo.fgContrained;
    rVdecInfo.fgLowDelay = rHdrInfo.fgLowDelay;
    rVdecInfo.fgMPEG2 = rHdrInfo.fgMPEG2;
    rVdecInfo.fgSeqDispExternValid = rHdrInfo.fgSeqDispExt;
    rVdecInfo.fgColorDescription = rHdrInfo.fgColorDescrip;
    rVdecInfo.fgPixelAspectRatio = rHdrInfo.fgPixelAspectRatio;
    rVdecInfo.ucColorPrimaries = rHdrInfo.ucColorPrimaries;
    rVdecInfo.ucTransferCharacteristics = rHdrInfo.ucTransferCharacteristics;
    rVdecInfo.ucMatrixCoefficients = rHdrInfo.ucMatrixCoefficients;
    rVdecInfo.ucProfileLevelIndication = rHdrInfo.ucProfileId;
    rVdecInfo.u4VbvBufSize = rHdrInfo.u4VbvSizeExt;
    rVdecInfo.u4ChromaFormat = rHdrInfo.u4ChromaFmt;

	if(rHdrInfo.u2Width == 1920 && rHdrInfo.u2Height == 1088)
	{
    	rVdecInfo.u2Height = 1080;
	}
	else
	{
		rVdecInfo.u2Height = rHdrInfo.u2Height;
	}
    rVdecInfo.u2Width = rHdrInfo.u2Width;
    rVdecInfo.u2FrmRate = rHdrInfo.u2FrmRate;
    rVdecInfo.eAspectRatio = rHdrInfo.eAspectRatio;
    rVdecInfo.u4BitRate = rHdrInfo.u4BitRate;
    rVdecInfo.ucAfd = rHdrInfo.ucAfd;
    rVdecInfo.fgProgressiveSeq = rHdrInfo.fgProgressiveSeq;
    rVdecInfo.ucVideoFmt = rHdrInfo.ucVideoFmt;
    rVdecInfo.fgNoOverScan = rHdrInfo.fgNoOverScan;

    rVdecInfo.u4H264IdcInfo = rHdrInfo.u4H264IdcInfo;

    rVdecInfo.fgCropping = rHdrInfo.fgCropping;
    rVdecInfo.u4CropX = rHdrInfo.u4CropX;
    rVdecInfo.u4CropY = rHdrInfo.u4CropY;
    rVdecInfo.u4CropWidth = rHdrInfo.u4CropWidth;
    rVdecInfo.u4CropHeight = rHdrInfo.u4CropHeight;

    rVdecInfo.eProfile = rHdrInfo.eProfile;
    rVdecInfo.eLevel = rHdrInfo.eLevel;
    rVdecInfo.u4DropFrameCnt = rHdrInfo.u4DropFrameCnt;
    // par,dar
    switch(rHdrInfo.eAspectRatio)
    {
        case VDEC_ASPECT_RATIO_4_3: // DAR
        {
            rVdecInfo.u4DarWidth = 4;
            rVdecInfo.u4DarHeight = 3;
            rVdecInfo.u4ParWidth = 4 * rVdecInfo.u2Height;
            rVdecInfo.u4ParHeight = 3 * rVdecInfo.u2Width;
        }
            break;
        case VDEC_ASPECT_RATIO_16_9: // DAR
        {
            rVdecInfo.u4DarWidth = 16;
            rVdecInfo.u4DarHeight = 9;
            rVdecInfo.u4ParWidth = 16 * rVdecInfo.u2Height;
            rVdecInfo.u4ParHeight = 9 * rVdecInfo.u2Width;
        }
            break;
        case VDEC_ASPECT_RATIO_221_1: // DAR, avoid floating point calculation, 100 times the w & h
        {
            rVdecInfo.u4DarWidth = 221;
            rVdecInfo.u4DarHeight = 100;
            rVdecInfo.u4ParWidth = (UINT32)(221 * (UINT32)rVdecInfo.u2Height);
            rVdecInfo.u4ParHeight = (UINT32)((100 * (UINT32)rVdecInfo.u2Width));
        }
            break;
        case VDEC_ASPECT_RATIO_10_11: // PAR
        {
            rVdecInfo.u4DarWidth = 10*rVdecInfo.u2Width;
            rVdecInfo.u4DarHeight = 11*rVdecInfo.u2Height;
            rVdecInfo.u4ParWidth = 10;
            rVdecInfo.u4ParHeight = 11;
        }
            break;
        case VDEC_ASPECT_RATIO_40_33: // PAR
        {
            rVdecInfo.u4DarWidth = 40*rVdecInfo.u2Width;
            rVdecInfo.u4DarHeight = 33*rVdecInfo.u2Height;
            rVdecInfo.u4ParWidth = 40;
            rVdecInfo.u4ParHeight = 33;
        }
            break;
        case VDEC_ASPECT_RATIO_16_11: // PAR
        {
            rVdecInfo.u4DarWidth = 16*rVdecInfo.u2Width;
            rVdecInfo.u4DarHeight = 11*rVdecInfo.u2Height;
            rVdecInfo.u4ParWidth = 16;
            rVdecInfo.u4ParHeight = 11;
        }
            break;
        case VDEC_ASPECT_RATIO_12_11: // PAR
        {
            rVdecInfo.u4DarWidth = 12*rVdecInfo.u2Width;
            rVdecInfo.u4DarHeight = 11*rVdecInfo.u2Height;
            rVdecInfo.u4ParWidth = 12;
            rVdecInfo.u4ParHeight = 11;
        }
            break;
        case VDEC_ASPECT_RATIO_3_2: // PAR
        {
            rVdecInfo.u4DarWidth = 3*rVdecInfo.u2Width;
            rVdecInfo.u4DarHeight = 2*rVdecInfo.u2Height;
            rVdecInfo.u4ParWidth = 3;
            rVdecInfo.u4ParHeight = 2;
        }
            break;
        case VDEC_ASPECT_RATIO_1_1: // PAR
        {
            rVdecInfo.u4DarWidth = 1*rVdecInfo.u2Width;
            rVdecInfo.u4DarHeight = 1*rVdecInfo.u2Height;
            rVdecInfo.u4ParWidth = 1;
            rVdecInfo.u4ParHeight = 1;
        }
            break;
        case VDEC_ASPECT_RATIO_TRANSMIT: // DAR = PAR*SAR
        {
            if(rHdrInfo.fgPixelAspectRatio)
            {
                rVdecInfo.u4DarWidth = rHdrInfo.u4PixelAspectRatioWidth*rHdrInfo.u2Width;
                rVdecInfo.u4DarHeight = rHdrInfo.u4PixelAspectRatioHeight*rHdrInfo.u2Height;  
                rVdecInfo.u4ParWidth = rHdrInfo.u4PixelAspectRatioWidth; 
                rVdecInfo.u4ParHeight = rHdrInfo.u4PixelAspectRatioHeight;
            }
            else // this is DAR in u4PixelAspectRatioWidth
            {
                rVdecInfo.u4DarWidth = rHdrInfo.u4PixelAspectRatioWidth;
                rVdecInfo.u4DarHeight = rHdrInfo.u4PixelAspectRatioHeight;
                rVdecInfo.u4ParWidth = (rHdrInfo.u2Width ? rHdrInfo.u4PixelAspectRatioWidth/rHdrInfo.u2Width : rHdrInfo.u4PixelAspectRatioWidth);
                rVdecInfo.u4ParHeight = (rHdrInfo.u2Height ? rHdrInfo.u4PixelAspectRatioHeight/rHdrInfo.u2Height : rHdrInfo.u4PixelAspectRatioHeight);
            }
        }
            break;
        default:
        {
            rVdecInfo.u4DarWidth = 1*rVdecInfo.u2Width;
            rVdecInfo.u4DarHeight = 1*rVdecInfo.u2Height;
            rVdecInfo.u4ParWidth = 1;
            rVdecInfo.u4ParHeight = 1;
        }
            break;
    }

    if (rHdrInfo.u2DHS == 0)
    {
        rVdecInfo.i2DHS = -1;
    }
    else
    {
        rVdecInfo.i2DHS = (INT16)rHdrInfo.u2DHS;
    }

    if (rHdrInfo.u2DVS == 0)
    {
        rVdecInfo.i2DVS = -1;
    }
    else
    {
        rVdecInfo.i2DVS = (INT16)rHdrInfo.u2DVS;
    }

    //for sony seamless play
    rVdecInfo.u2SourceHeight = rHdrInfo.u2SourceHeight;
    rVdecInfo.u2SourceWidth  = rHdrInfo.u2SourceWidth;
    rVdecInfo.eSrcASP        = rHdrInfo.eSourceAspectRatio;
    rVdecInfo.u4SourcePixelAspectRatioHeight = rHdrInfo.u4SourcePixelAspectRatioHeight;
    rVdecInfo.u4SourcePixelAspectRatioWidth  = rHdrInfo.u4SourcePixelAspectRatioWidth;
    rVdecInfo.fgResized = rHdrInfo.fgResized;

    USR_SPACE_ACCESS_VALIDATE_ARG(prVdecInfo, MTVDEC_HDR_INFO_T );
    COPY_TO_USER_ARG(prVdecInfo, rVdecInfo, MTVDEC_HDR_INFO_T );

    #if 0
    MTVDEC_PRINT(" - ucEsId = %d\n",(int)ucEsId);
    MTVDEC_PRINT_INFO(rVdecInfo);
    #endif
    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

static MT_RESULT_T _MTVDEC_GetDelayTime(unsigned long arg)
{    
    MTVDEC_HDR_INFO_T*   prVdecInfo;
    MTAL_IOCTL_2ARG_T    rArg;
    VDEC_HDR_INFO_T rHdrInfo;

    UCHAR ucEsId;
    MTVDEC_HDR_INFO_T    rVdecInfo;

    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId =(UCHAR)rArg.ai4Arg[0];
    prVdecInfo = (MTVDEC_HDR_INFO_T*)rArg.ai4Arg[1];
    x_memset(&rVdecInfo, 0, sizeof(MTVDEC_HDR_INFO_T));
    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }

    if (VDEC_GetDelayTime(ucEsId, &rHdrInfo) == FALSE)
    {
        USR_SPACE_ACCESS_VALIDATE_ARG(prVdecInfo, MTVDEC_HDR_INFO_T );
        COPY_TO_USER_ARG(prVdecInfo, rVdecInfo, MTVDEC_HDR_INFO_T );
        UnLockVdecSemaphoreMtal();
        return MTR_OK;
    }
    
    rVdecInfo.u4DelayTime = rHdrInfo.u4DelayTime;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prVdecInfo, MTVDEC_HDR_INFO_T );
    COPY_TO_USER_ARG(prVdecInfo, rVdecInfo, MTVDEC_HDR_INFO_T );
    
    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

static MT_RESULT_T _MTVDEC_GetWFDErrInfo(unsigned long arg)
{
    MTVDEC_WFD_DECODE_ERROR_T* prWFDErrInfo;
    MTAL_IOCTL_2ARG_T rArg;
    VDEC_WFD_DECODE_ERROR_T rVdecWFDErrInfo;

    UCHAR ucEsId;
    MTVDEC_WFD_DECODE_ERROR_T rWFDErrInfo;

    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId =(UCHAR)rArg.ai4Arg[0];
    prWFDErrInfo = (MTVDEC_WFD_DECODE_ERROR_T*)rArg.ai4Arg[1];
    x_memset(&rWFDErrInfo, 0, sizeof(MTVDEC_WFD_DECODE_ERROR_T));

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }

    if (VDEC_QueryWFDErrInfo(ucEsId, &rVdecWFDErrInfo) == FALSE)
    {
        USR_SPACE_ACCESS_VALIDATE_ARG(prWFDErrInfo, MTVDEC_WFD_DECODE_ERROR_T );
        COPY_TO_USER_ARG(prWFDErrInfo, rWFDErrInfo, MTVDEC_WFD_DECODE_ERROR_T );
        UnLockVdecSemaphoreMtal();
        return MTR_OK;
    }
    rWFDErrInfo.i4LackSeqHeaderCnt = rVdecWFDErrInfo.i4LackSeqHeaderCnt;
    rWFDErrInfo.i4OutOfSpecCnt = rVdecWFDErrInfo.i4OutOfSpecCnt;
    rWFDErrInfo.i4TimeoutCnt = rVdecWFDErrInfo.i4TimeoutCnt;

    USR_SPACE_ACCESS_VALIDATE_ARG(prWFDErrInfo, MTVDEC_WFD_DECODE_ERROR_T );
    COPY_TO_USER_ARG(prWFDErrInfo, rWFDErrInfo, MTVDEC_WFD_DECODE_ERROR_T );

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

static MT_RESULT_T _MTVDEC_ResetWFDInfo(unsigned long arg)
{
    UCHAR ucEsId;
    ucEsId = (UCHAR)arg;

    LockVdecSemaphoreMtal();

    MTVDEC_PRINT(" - ucEsId = %d\n",(int)ucEsId);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    VDEC_ResetWFDInfo(ucEsId);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

static MT_RESULT_T _MTVDEC_GetDbkInfo(unsigned long arg)
{
    MTVDEC_DBK_INFO_T*   prDbkInfo;
    MTAL_IOCTL_2ARG_T    rArg;
    VDEC_HDR_INFO_T rHdrInfo;

    UCHAR ucEsId;
    MTVDEC_DBK_INFO_T    rDbkInfo;

    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId =(UCHAR)rArg.ai4Arg[0];
    prDbkInfo = (MTVDEC_DBK_INFO_T*)rArg.ai4Arg[1];
    x_memset(&rDbkInfo, 0, sizeof(MTVDEC_DBK_INFO_T));

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }

    VDEC_QueryInfo(ucEsId, &rHdrInfo);

    rDbkInfo.fgDeblocking = rHdrInfo.fgDeblocking;
    rDbkInfo.u4Dbk1PixelCnt = rHdrInfo.u4Dbk1PixelCnt;
    rDbkInfo.u4Dbk2PixelCnt = rHdrInfo.u4Dbk2PixelCnt;
    rDbkInfo.u4Dbk3PixelCnt = rHdrInfo.u4Dbk3PixelCnt;

    USR_SPACE_ACCESS_VALIDATE_ARG(prDbkInfo, MTVDEC_DBK_INFO_T );
    COPY_TO_USER_ARG(prDbkInfo, rDbkInfo, MTVDEC_DBK_INFO_T );

    MTVDEC_PRINT(" - ucEsId = %d\n",(int)ucEsId);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

static MT_RESULT_T _MTVDEC_GetDirectVdecInfo(unsigned long arg)
{
    MT_VDEC_DIRECT_VideoInfo*   prVDECInfo;
    MTAL_IOCTL_2ARG_T    rArg;
    VDEC_HDR_INFO_T rHdrInfo;

    UCHAR ucEsId;
    MT_VDEC_DIRECT_VideoInfo    rVDECInfo;

    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId =(UCHAR)rArg.ai4Arg[0];
    prVDECInfo = (MT_VDEC_DIRECT_VideoInfo*)rArg.ai4Arg[1];
    x_memset(&rVDECInfo, 0, sizeof(MT_VDEC_DIRECT_VideoInfo));
	x_memset(&rHdrInfo, 0, sizeof(VDEC_HDR_INFO_T));
    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }

    VDEC_QueryInfo(ucEsId, &rHdrInfo);

	rVDECInfo.scan_type = rHdrInfo.fgProgressiveFrm;
	rVDECInfo.framerate_num = rHdrInfo.u2FrmRate; 
	rVDECInfo.framerate_den = 100;  
    rVDECInfo.structure_size = sizeof(MT_VDEC_DIRECT_VideoInfo);
    if(rHdrInfo.u2SourceWidth == 0 || rHdrInfo.u2SourceHeight == 0)
    {
		rVDECInfo.width = rHdrInfo.u2OrgWidth;
		rVDECInfo.height = rHdrInfo.u2OrgHeight;
	}
	else
	{
		rVDECInfo.width  = rHdrInfo.u2SourceWidth;
		rVDECInfo.height = rHdrInfo.u2SourceHeight;
	}
	//cal par,dar
    switch(rHdrInfo.eAspectRatio)
    {
        case VDEC_ASPECT_RATIO_4_3: // DAR
        	{
	            rVDECInfo.par_w = 4 * rVDECInfo.height;
	            rVDECInfo.par_h = 3 * rVDECInfo.width;
        	}
            break;
        case VDEC_ASPECT_RATIO_16_9: // DAR
        	{
            	rVDECInfo.par_w = 16 * rVDECInfo.height;
            	rVDECInfo.par_h = 9 * rVDECInfo.width;
        	}
            break;
        case VDEC_ASPECT_RATIO_221_1: // DAR, avoid floating point calculation, 100 times the w & h
	        {
	            rVDECInfo.par_w = (UINT32)(221 * (UINT32)rVDECInfo.height);
	            rVDECInfo.par_h = (UINT32)((100 * (UINT32)rVDECInfo.width));
	        }
            break;
        case VDEC_ASPECT_RATIO_10_11: // PAR
	        {
	            rVDECInfo.par_w = 10;
	            rVDECInfo.par_h = 11;
	        }
            break;
        case VDEC_ASPECT_RATIO_40_33: // PAR
	        {
	            rVDECInfo.par_w = 40;
	            rVDECInfo.par_h = 33;
	        }
            break;
        case VDEC_ASPECT_RATIO_16_11: // PAR
	        {
	            rVDECInfo.par_w = 16;
	            rVDECInfo.par_h = 11;
	        }
            break;
        case VDEC_ASPECT_RATIO_12_11: // PAR
	        {
	            rVDECInfo.par_w = 12;
	            rVDECInfo.par_h = 11;
	        }
            break;
        case VDEC_ASPECT_RATIO_3_2: // PAR
	        {
	            rVDECInfo.par_w = 3;
	            rVDECInfo.par_h = 2;
	        }
            break;
        case VDEC_ASPECT_RATIO_1_1: // PAR
	        {
	            rVDECInfo.par_w = 1;
	            rVDECInfo.par_h = 1;
	        }
            break;
        case VDEC_ASPECT_RATIO_TRANSMIT: // DAR = PAR*SAR
	        {
	            if(rHdrInfo.fgPixelAspectRatio)
	            {
	                rVDECInfo.par_w = rHdrInfo.u4PixelAspectRatioWidth; 
	                rVDECInfo.par_h = rHdrInfo.u4PixelAspectRatioHeight;
	            }
	            else // this is DAR in u4PixelAspectRatioWidth
	            {
	                rVDECInfo.par_w = (rHdrInfo.u2Width ? rHdrInfo.u4PixelAspectRatioWidth/rHdrInfo.u2Width : rHdrInfo.u4PixelAspectRatioWidth);
	                rVDECInfo.par_h = (rHdrInfo.u2Height ? rHdrInfo.u4PixelAspectRatioHeight/rHdrInfo.u2Height : rHdrInfo.u4PixelAspectRatioHeight);
	            }
	        }
            break;
        default:
	        {
	            rVDECInfo.par_w = 1;
	            rVDECInfo.par_h = 1;
	        }
            break;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prVDECInfo, MT_VDEC_DIRECT_VideoInfo );
    COPY_TO_USER_ARG(prVDECInfo, rVDECInfo, MT_VDEC_DIRECT_VideoInfo );

    MTVDEC_PRINT(" - ucEsId = %d\n",(int)ucEsId);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_SetAvSynMode(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UCHAR ucEsId;
    UCHAR ucStcId;
    MTAV_SYNC_MODE_T eMode;

    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    ucEsId = (UCHAR)rArg.ai4Arg[0];
    eMode = (MTAV_SYNC_MODE_T)rArg.ai4Arg[1];
    ucStcId = (UCHAR)rArg.ai4Arg[2];
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    MTVDEC_PRINT(" - ucEsId = %d, eMode = %d\n",
        (int)ucEsId,
        (int)eMode);


    //add driver implementation code here
    if(eMode == MTAV_SYNC_FREE_RUN)
    {
        STC_SetSTCSyncMode(ucStcId, AV_SYNC_MODE_NONE);
    }
    else if(eMode == MTAV_SYNC_SLAVE)
    {
        STC_SetSTCSyncMode(ucStcId, AV_SYNC_MODE_SYSTEM_MASTER);
    }
    else if(eMode == MTAV_SYNC_MASTER)
    {
        // TODO: check why AV_SYNC_MODE_AUDIO_MASTER
        STC_SetSTCSyncMode(ucStcId, AV_SYNC_MODE_AUDIO_MASTER);
    }
    else if(eMode == MTAV_SYNC_SLAVE_MM)
    {
        STC_SetSTCSyncMode(ucStcId, AV_SYNC_MODE_AUDIO_MASTER);
    }

    VDEC_SyncStc(ucEsId, eMode, ucStcId);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_GetStc(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UCHAR ucEsId;
    UINT64 *pu8Stc;
    UINT64  u8Stc;
    STC_CLOCK rStcClk;

    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    pu8Stc = (UINT64 *)rArg.ai4Arg[1];

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    if (STC_GetSrc(0/*u2PcrId*/, &rStcClk) == STC_VALID)
    {
        u8Stc = (UINT64/*PTS_T*/)(rStcClk.u4Base);
        // TODO, incorrect use hard code 0.
    }
    else
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(pu8Stc,UINT64);
    COPY_TO_USER_ARG(pu8Stc,u8Stc,UINT64);

    MTVDEC_PRINT(" - ucEsId = %d, u8Stc = 0x%08x08%x\n",
        (int)ucEsId,(unsigned int)(u8Stc>>32),(unsigned int)u8Stc);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_GetDecStatus(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    MTVDEC_DEC_STATUS_T*   prDecStatus;
    MTVDEC_DEC_STATUS_T   rDecStatus;
    UCHAR ucEsId;
    UCHAR ucFbgId;

    BOOL fgLock;                        ///<video decoder is lock.
    BOOL fgTopFldFirst;                 ///<top field first
    BOOL fgRepFirstFld;                 ///<repeat first field
    BOOL fgProgressiveFrm;              ///<progressive frame
    BOOL fgDecError;                    ///<frame decode normally or not
    BOOL fgDisplayStatus;               ///<frame dipslaying normally or not
    UCHAR ucPicType;                    ///<picture type decoded ok by video decoder.
    UCHAR ucPicStruture;                ///<picture struct
    UCHAR ucTemporalRef;               ///<temporal reference
    UINT32 u4ReceiveNs;                 ///<picture count received by video decoder.
    UINT32 u4DecOkNs;                   ///<picture count decoded ok by video decoder.

    CRIT_STATE_T rCritState;
    static DRV_DBASE_T* prDbase = NULL;
    PARAM_MPV_T* prMpv = NULL;
    static PARAM_MPV_T arMpvKeep[VDEC_MAX_ES];
    PARAM_MPV_T* prMpvKeep;
    VDEC_GNL_INFO_T rGnlInfo;

    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    prDecStatus = (MTVDEC_DEC_STATUS_T*)rArg.ai4Arg[1];
    x_memset(&rDecStatus, 0, sizeof(MTVDEC_DEC_STATUS_T));
    x_memset(&rGnlInfo, 0, sizeof(VDEC_GNL_INFO_T));

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }

    VDEC_GetDecStatus(ucEsId, &fgLock, &fgTopFldFirst
        , &fgRepFirstFld, &fgProgressiveFrm, &ucPicType
        , &ucPicStruture, &ucTemporalRef, &u4ReceiveNs
        , &u4DecOkNs);
    VDEC_GetDecErrInfo(ucEsId, &fgDecError, &fgDisplayStatus,&rDecStatus.u4ErrMbCnt);

    ucFbgId=FBM_GetFbgByEs(ucEsId);
    if(ucFbgId != FBM_FBG_ID_UNKNOWN)
    {
        rDecStatus.fgPtsMached = FBM_GetPtsSync(ucFbgId);
    }
    else
    {
        rDecStatus.fgPtsMached = FALSE;
    }
    
    rDecStatus.fgLock = fgLock;
    rDecStatus.fgDecodeError = fgDecError;
    rDecStatus.fgDisplayStatus = fgDisplayStatus;

    if(fgLock)
    {
        MTVDEC_PRINT("_MTVDEC_GetDecStatus locked\n");
        rDecStatus.fgIsDisplaying = MPV_IsDisplay(ucEsId);
        rDecStatus.fgTopFldFirst = fgTopFldFirst;
        rDecStatus.fgRepFirstFld = fgRepFirstFld;
        rDecStatus.fgProgressiveFrm = fgProgressiveFrm;
        rDecStatus.ucPicType = ucPicType;
        rDecStatus.ucPicStruture = ucPicStruture;
        rDecStatus.ucTemporalRef = ucTemporalRef;
        rDecStatus.u4ReceiveNs = u4ReceiveNs;
        rDecStatus.u4DecOkNs = u4DecOkNs;
        rDecStatus.u4CurPTS = VDP_GetPts(VDP_Es2Vdp(ucEsId));
        if(VDEC_GetGnlInfo(ucEsId, &rGnlInfo))
        {
            rDecStatus.u4ESBufDataSize = rGnlInfo.u4ESBufDataSize;
            rDecStatus.u4DisplayQNum = rGnlInfo.u4DisplayQNum;
            rDecStatus.u4DecodingPTS = rGnlInfo.u4DecodingPTS;
        }
    }
    else
    {
        Printf("_MTVDEC_GetDecStatus unlocked\n");
    }

    {
        if(prDbase == NULL)
        {
            prDbase = DBS_Lock(&rCritState);
            ASSERT(prDbase);
            VERIFY(DBS_Unlock(prDbase, rCritState));
        }

        rDecStatus.u4Status = MTVDEC_STATUS_NORMAL;
        prMpv = (PARAM_MPV_T*)&prDbase->rMpv;
        prMpv += ucEsId;
        prMpvKeep = &(arMpvKeep[ucEsId]);

        if((prMpv->u4RetrieveNs > 90) &&
           (prMpv->u4RetrieveNs > prMpvKeep->u4RetrieveNs) &&
           (prMpv->u4DecOkNs > 0))
        {
            if((prMpv->u4EsmSkipNs - prMpvKeep->u4EsmSkipNs) &&
               ((((prMpv->u4EsmSkipNs - prMpvKeep->u4EsmSkipNs) * 30) // 2 skips in 2 second
                 / (prMpv->u4RetrieveNs - prMpvKeep->u4RetrieveNs)) > 0))
            {
                rDecStatus.u4Status |= MTVDEC_STATUS_SKIP;
            }
            if((prMpv->u4VpsrDropNs - prMpvKeep->u4VpsrDropNs) &&
               ((((prMpv->u4VpsrDropNs - prMpvKeep->u4VpsrDropNs) * 30) // 2 drops in 2 second
                / (prMpv->u4RetrieveNs - prMpvKeep->u4RetrieveNs)) > 0))
            {
                rDecStatus.u4Status |= MTVDEC_STATUS_DROP;
            }
            if((prMpv->u4TimeOutNs - prMpvKeep->u4TimeOutNs) &&
               ((((prMpv->u4TimeOutNs - prMpvKeep->u4TimeOutNs) * 60) // 1 timeout in 2 second
                / (prMpv->u4RetrieveNs - prMpvKeep->u4RetrieveNs)) > 0))
            {
                rDecStatus.u4Status |= MTVDEC_STATUS_TIMEOUT;
            }
        }

        if((prMpv->u4RetrieveNs - prMpvKeep->u4RetrieveNs) > 60)
        {
            x_memcpy(prMpvKeep, prMpv, sizeof(PARAM_MPV_T));
        }
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prDecStatus, MTVDEC_DEC_STATUS_T);
    COPY_TO_USER_ARG(prDecStatus,rDecStatus,MTVDEC_DEC_STATUS_T);

    #if 0
    MTVDEC_PRINT(" - ucEsId = %d, fgLock = %d, fgTopFldFirst = %d\
        , fgRepFirstFld = %d, fgProgressiveFrm = %d, ucPicType = %d\
        , ucPicStruture = %d, ucTemporalRef = %d, u4ReceiveNs = %d\
        , u4DecOkNs = %d"\
        , (int)ucEsId, (int)fgLock, (int)fgTopFldFirst, (int)fgRepFirstFld\
        , (int)fgProgressiveFrm, (int)ucPicType, (int)ucPicStruture\
        , (int)ucTemporalRef, (int)u4ReceiveNs, (int)u4DecOkNs);
    #endif

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}


//TODO: for Multiple instance support
static MT_RESULT_T _MTVDEC_RegCallBack(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T    rArg;
	MTVDEC_CB_TYPE_T eCbType;
    LockVdecSemaphoreMtal();
    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

	eCbType=rArg.ai4Arg[0];
	if(eCbType==MTVDEC_CB_TYPE_ERR)
	{
	    UINT32 u4Duration;
		MTVDEC_PRINT(" - _MTVDEC_RegCallBack MTVDEC_CB_TYPE_ERR\n");
		u4Duration = rArg.ai4Arg[2];
		VDEC_RegDecErrCb((PFN_VDEC_DECERR_CB)_MTVDEC_ERR_Nfy, u4Duration);
	}
	else if(eCbType==MTVDEC_CB_TYPE_STATUS)
	{
		MTVDEC_PRINT(" - _MTVDEC_RegCallBack MTVDEC_CB_TYPE_STATUS\n");
		VDEC_RegStatusCb((PFN_VDEC_STATUS_CB)_MTVDEC_STATUS_INT_Nfy);
	}
	else if(eCbType==MTVDEC_CB_TYPE_FRAME)
	{
		MTVDEC_PRINT(" - _MTVDEC_RegCallBack MTVDEC_CB_TYPE_FRAME\n");
		VDEC_RegPicTypeCb((PFN_VDEC_PIC_TYPE_CB)_MTVDEC_FRAME_Nfy);
	}
	else if(eCbType==MTVDEC_CB_TYPE_PICINFO)
	{
		MTVDEC_PRINT(" - _MTVDEC_RegCallBack MTVDEC_CB_TYPE_PICINFO\n");
		VDEC_RegPicInfoCb((PFN_VDEC_PICINFO_CB)_MTVDEC_PICINFO_Nfy);
	}
	else if(eCbType==MTVDEC_CB_TYPE_USERDATA)
	{
		MTVDEC_PRINT(" - _MTVDEC_RegCallBack MTVDEC_CB_TYPE_USERDATA\n");
		MPV_RegUserDataCb((PFN_MPV_USERDATA_CB)_MTVDEC_CC_Nfy);
	}

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

//TODO: for Multiple instance support
static MT_RESULT_T _MTVDEC_RegDecErrCb(unsigned long arg)
{
    UINT32 u4Duration;
    LockVdecSemaphoreMtal();

    u4Duration = (UINT32)arg;

    VDEC_RegDecErrCb((PFN_VDEC_DECERR_CB)_MTVDEC_ERR_Nfy, u4Duration);

    MTVDEC_PRINT(" - pfDecErrCb = 0x%x, u4Duration = %d ms\n",(int)_MTVDEC_ERR_Nfy, (int)u4Duration);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

//TODO: for Multiple instance support
static MT_RESULT_T _MTVDEC_FrameCb(void)
{
    LockVdecSemaphoreMtal();

    VDEC_RegPicTypeCb((PFN_VDEC_PIC_TYPE_CB)_MTVDEC_FRAME_Nfy);
    //VDP_RegCbFunc(VDP_CB_FUNC_Get_PIC_TYPE,(UINT32)_MTVDEC_PIC_TYPE_Nfy,0,0);

    MTVDEC_PRINT(" - pfPicTypeCb = 0x%x\n",(int)_MTVDEC_FRAME_Nfy);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_StatusIntCb(void)
{
    LockVdecSemaphoreMtal();
    VDEC_RegStatusCb((PFN_VDEC_STATUS_CB)_MTVDEC_STATUS_INT_Nfy);

    MTVDEC_PRINT(" - pfStatusCb = 0x%x\n",(int)_MTVDEC_STATUS_INT_Nfy);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

static MT_RESULT_T _MTVDEC_RegGeneralCb(unsigned long arg)
{
    UCHAR ucEsId;
    MTAL_IOCTL_2ARG_T    rArg;
    MTVDEC_PFN_GENERAL_CB prGeneralCb;
    VDEC_DEC_NFY_INFO_T rNfyInfo;

    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    prGeneralCb = (MTVDEC_PFN_GENERAL_CB)rArg.ai4Arg[1];

    x_memset(&rNfyInfo, 0, sizeof(VDEC_DEC_NFY_INFO_T));

    rNfyInfo.pvTag = (void*)(UINT32)ucEsId;
    if(prGeneralCb)
    {
        rNfyInfo.pfDecNfy = _MTVDEC_GENERAL_Nfy;
    }
    else
    {
        rNfyInfo.pfDecNfy = (PFN_VDEC_DEC_NFY_FCT)(void*)(NULL);
    }

    _MTVDEC_Notify_Lock();
    _u4NotifyMask |= MTVDEC_NOTIFY_MW_IF;
    _MTVDEC_Notify_Unlock();

#if 0
    // MTVDEC_NOTIFY_MW_IF was set, use _MTVDEC_VDEC_Nfy call it
    VDEC_SetDecNfy(ucEsId, &rNfyInfo);
#endif

    MTVDEC_PRINT(" - prGeneralCb = 0x%x\n",(int)prGeneralCb);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

static MT_RESULT_T _MTVDEC_SetFrameToUser(unsigned long arg)
{
    UCHAR ucEsId;
    MTAL_IOCTL_2ARG_T    rArg;
    BOOL fgFrameToUser;

    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    fgFrameToUser = (BOOL)rArg.ai4Arg[1];

    VDEC_SetFrameToUser(ucEsId, fgFrameToUser);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

#if 0
static MT_RESULT_T _MTVDEC_GetDecFifo(unsigned long arg)
{
    UCHAR ucEsId;
    UINT32 *pu4FifoAddr;
    UINT32 *pu4FifoSize;
    MTAL_IOCTL_3ARG_T    rArg;
    UINT32 u4FifoAddr;
    UINT32 u4FifoSize;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    pu4FifoAddr = (UINT32 *)rArg.ai4Arg[1];
    pu4FifoSize = (UINT32 *)rArg.ai4Arg[2];
    MTVDEC_PRINT(" - ucEsId = %d, pu4FifoAddr = %d, pu4FifoSize = %d\n",
        ucEsId,
        pu4FifoAddr,
        pu4FifoSize);

    //add driver implementation code here


    USR_SPACE_ACCESS_VALIDATE_ARG(pu4FifoAddr,UINT32);
    COPY_TO_USER_ARG(pu4FifoAddr,u4FifoAddr,UINT32);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4FifoSize,UINT32);
    COPY_TO_USER_ARG(pu4FifoSize,u4FifoSize,UINT32);

    return MTR_OK;
}
#endif

//TODO: for Multiple instance support
static MT_RESULT_T _MTVDEC_RegUserDataCb(unsigned long arg)
{
    MTVDEC_PFN_USERDATA_CB pfUserDataCb;

    LockVdecSemaphoreMtal();
    pfUserDataCb = (MTVDEC_PFN_USERDATA_CB)arg;
    MTVDEC_PRINT(" - pfUserDataCb = 0x%x\n",(int)pfUserDataCb);

    //add driver implementation code here
    MPV_RegUserDataCb((PFN_MPV_USERDATA_CB)_MTVDEC_CC_Nfy);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_StartUserData(unsigned long arg)
{
    UCHAR ucEsId;

    LockVdecSemaphoreMtal();
    ucEsId = (UCHAR)arg;

    MTVDEC_PRINT(" - ucEsId = %d\n",(int)ucEsId);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    MPV_StartUserData(ucEsId);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_StopUserData(unsigned long arg)
{
    UCHAR ucEsId;

    LockVdecSemaphoreMtal();
    ucEsId = (UCHAR)arg;
    MTVDEC_PRINT(" - ucEsId = %d\n",(int)ucEsId);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    MPV_StopUserData(ucEsId);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

static MT_RESULT_T _MTVDEC_GetUserData(unsigned long arg)
{
    UCHAR *pucUserDataTmpBuf;
    UINT32 u4Rptr;
    UINT32 u4DataLen;
    UCHAR ucEsId;
    MTAL_IOCTL_4ARG_T    rArg;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    pucUserDataTmpBuf = (UCHAR *)rArg.ai4Arg[1];
    u4Rptr = (UINT32)rArg.ai4Arg[2];
    u4DataLen = (UINT32)rArg.ai4Arg[3];
    MTVDEC_PRINT(" - ucEsId = %d, pucUserDataTmpBuf = %d, u4Rptr = %d, u4DataLen = %d\n",
        (int)ucEsId,
        (int)pucUserDataTmpBuf,
        (int)u4Rptr,
        (int)u4DataLen);

    if ((ucEsId>=VDEC_MAX_ES) || (MAX_USERDATA_TMPSIZE<u4DataLen))
    {
        MTVDEC_PRINT("invalid es id %d\n", ucEsId);
        return MTR_ERR_INV;
    }

    //add driver implementation code here
    if(!MPV_GetUserData(ucEsId, _au1UserDataTmpBuf[ucEsId], u4Rptr, u4DataLen))
    {
        UnLockVdecSemaphoreMtal();
        return -1;
    }
#if 0
    if((u4DataLen > 16)&& (ucEsId == 0))
    {
        INT32 i = 0;
        MTVDEC_PRINT(" - %d ", ucEsId);
        while(i < u4DataLen)
        {
            MTVDEC_PRINT("0x%x ", *((UINT32*)(_au1UserDataTmpBuf[ucEsId] + i)));
            i+=4;
        }
        MTVDEC_PRINT("\n");
    }
#endif
    if (copy_to_user((void __user *)pucUserDataTmpBuf, (void*)_au1UserDataTmpBuf[ucEsId], u4DataLen))
    {
        MTVDEC_PRINT("%s copy_to_user error\n", __FUNCTION__);
        UnLockVdecSemaphoreMtal();
        return MTR_ERR_INV;
    }

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}
#ifdef CC_ITIMING_ALWAYS
static MT_RESULT_T _MTVDEC_SetFixFHDDisplay(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UCHAR ucEsId;
    BOOL fgInterlace;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    fgInterlace = (BOOL)rArg.ai4Arg[1];

    MTVDEC_PRINT(" - ucEsId = %d, fgEnable = %d\n",
        (int)ucEsId,
        (int)fgInterlace);

    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }

    VDEC_SetFixFHDDisplay(ucEsId, fgInterlace);
    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}
#endif
static MT_RESULT_T _MTVDEC_SetSpeed(unsigned long arg)
{
    UCHAR ucEsId;
    MTVDEC_DEC_SPEED_T eSpeed;
    MTAL_IOCTL_2ARG_T    rArg;

    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucEsId = (UCHAR)rArg.ai4Arg[0];
    eSpeed = (MTVDEC_DEC_SPEED_T)rArg.ai4Arg[1];
    MTVDEC_PRINT(" - eSpeed = %d\n",
        (int)eSpeed);

    if(ucEsId == ES0) //assume ES0 map to B2R_1
    {
        MTVDEC_PRINT(" - eSpeed(%d)\n", (UINT32)eSpeed);
        switch(eSpeed)
        {
            case MTVDEC_DEC_SPEED_PAUSE:
                #ifdef TIME_SHIFT_SUPPORT
                if(_MTDREC_IsLivePidEnable())
                {
                    VDP_FrcSetFreeze(B2R_1, TRUE);
                }
                else
                #endif
                {
                    VDP_SetFBNoTimeout(B2R_1, TRUE);
                    if(VDP_SetPauseMm(B2R_1, TRUE) != VDP_SET_OK)
                    {
                        MTVDEC_PRINT(" - pause mm fail\n");
                    }
                }
                break;
            case MTVDEC_DEC_SPEED_SLOW:
                if(VDP_SetSpeed(B2R_1, STC_SPEED_TYPE_FORWARD_1_DIV_2X) != VDP_SET_OK)
                {
                    MTVDEC_PRINT(" - 1/2x fail\n");
                }
                VDP_SetFBNoTimeout(B2R_1, TRUE);
                VDP_FrcSetFreeze(B2R_1, FALSE);
                if(VDP_SetPauseMm(B2R_1, FALSE) != VDP_SET_OK)
                {
                    MTVDEC_PRINT(" - pause mm fail\n");
                }
                break;
            case MTVDEC_DEC_SPEED_NORMAL:
                if(VDP_SetSpeed(B2R_1, STC_SPEED_TYPE_FORWARD_1X) != VDP_SET_OK)
                {
                    MTVDEC_PRINT(" - 1x fail\n");
                }
                VDP_SetFBNoTimeout(B2R_1, FALSE);
                VDP_FrcSetFreeze(B2R_1, FALSE);
                if(VDP_SetPauseMm(B2R_1, FALSE) != VDP_SET_OK)
                {
                    MTVDEC_PRINT(" - pause mm fail\n");
                }
                break;
            default:
                MTVDEC_PRINT(" - invalid speed\n");
                UnLockVdecSemaphoreMtal();
                return MTR_ERR_INV;
        }
    }
    else
    {
        MTVDEC_PRINT(" - invalid ucEsId(%d)\n", ucEsId);
        UnLockVdecSemaphoreMtal();
        return MTR_ERR_INV;
    }

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

static MT_RESULT_T _MTVDEC_SetDecodeMode(unsigned long arg)
{
    UCHAR ucEsId;
    MTVDEC_DEC_MODE_T eMode;
    MTAL_IOCTL_2ARG_T    rArg;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucEsId = (UCHAR)rArg.ai4Arg[0];
    eMode = (MTVDEC_DEC_MODE_T)rArg.ai4Arg[1];
    MTVDEC_PRINT(" - eMode = %d\n",
        (int)eMode);

    if(ES0==ucEsId || ES1==ucEsId)
    {
        switch(eMode)
        {
            case MTVDEC_DEC_ALL:
                VDEC_SetMMParam(ucEsId, VDEC_MM_PLAY_SPEED,
                    (UINT32)STC_SPEED_TYPE_FORWARD_1X, 0, 0);
                VDEC_SetMMParam(ucEsId, VDEC_MM_PVR_MODE, (UINT32)TRUE, 0, 0);
                break;
            case MTVDEC_DEC_IP:
                VDEC_SetMMParam(ucEsId, VDEC_MM_PLAY_SPEED,
                    (UINT32)STC_SPEED_TYPE_FORWARD_2X, 0, 0);
                VDEC_SetMMParam(ucEsId, VDEC_MM_PVR_MODE, (UINT32)TRUE, 1, 0);
                break;
            case MTVDEC_DEC_I:
                VDEC_SetMMParam(ucEsId, VDEC_MM_PLAY_SPEED,
                    (UINT32)STC_SPEED_TYPE_FORWARD_4X, 0, 0);
                VDEC_SetMMParam(ucEsId, VDEC_MM_PVR_MODE, (UINT32)TRUE, 2, 0);
                break;
            default:
                MTVDEC_PRINT(" - invalid mode\n");
                UnLockVdecSemaphoreMtal();
                return MTR_ERR_INV;
        }
    }
    else
    {
        MTVDEC_PRINT(" - invalid ucEsId(%d)\n", ucEsId);
        UnLockVdecSemaphoreMtal();
        return MTR_ERR_INV;
    }

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}


#ifdef CC_53XX_SWDMX_V2
static VOID _MTPVR_StepFinNotify(UINT32 u4VdpId, UINT32 u4PTS, UINT32 u4Ret, UINT32 u4AttachedId)
#else
static VOID _MTPVR_StepFinNotify(UINT32 u4VdpId, UINT32 u4PTS, UINT32 u4Ret)
#endif
{
    STEPFIN_CB_T rPara;

    UNUSED(u4VdpId);
    //UNUSED(u4PTS);
    MTVDEC_PRINT("_MTPVR_StepFinNotify, _MTPVR_SetSpeed set to 0\n");
    // block mode
    //VERIFY (x_sema_unlock(_hVDPMWStepMutex) == OSR_OK);
    // notify mw that we finish step.

    rPara.u4Pts = u4PTS;

    _CB_PutEvent(CB_MTAL_STEP_FIN_TRIGGER, sizeof(STEPFIN_CB_T), (void*)&rPara);
    // PVR0 0, speed 0
#ifdef ENABLE_MULTIMEDIA
    _MTPVR_SetSpeedExt(0, 0, 2); // step done
 #endif
}

static MT_RESULT_T _MTVDEC_SetFrameAdvance(unsigned long arg)
{
    UCHAR ucEsId;
    BOOL fgSetFrameAdvance;
    MTAL_IOCTL_2ARG_T    rArg;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucEsId = (UCHAR)rArg.ai4Arg[0];
    fgSetFrameAdvance = (BOOL)rArg.ai4Arg[1];
    MTVDEC_PRINT(" - fgSetFrameAdvance = %d\n",
        (int)fgSetFrameAdvance);

    if(ucEsId == ES0) //assume ES0 map to B2R_1
    {
        if(fgSetFrameAdvance)
        {
            #ifdef ENABLE_MULTIMEDIA
            _MTPVR_SetSpeedExt(0, 0, 4); // notify the vdp unlock
            #endif
            VDP_SetSpeed(B2R_1, STC_SPEED_TYPE_FORWARD_1X);

            /* Pause VDP and just display 1 frame */
            VDP_SetFBNoTimeout(B2R_1, TRUE);
            VDP_SetPauseMm(B2R_1, TRUE);
            VDP_SetStepForward(B2R_1, VDP_PLAY_STEP_FORWARD);

            /* VDP will play one frame and then Pause if VDP_PLAY_STEP_FORWARD is set */
            VDP_FrcSetFreeze(B2R_1, FALSE);
            VDP_SetPauseMm(B2R_1, FALSE);

            VDP_SetCbAttachedSrcId(B2R_1, ucEsId);
            VDP_RegCbFunc(VDP_CB_FUNC_MM_STEP_FIN_IND, (UINT32)_MTPVR_StepFinNotify, 0, 0);
            #ifdef ENABLE_MULTIMEDIA
            _MTPVR_SetSpeedExt(0, 0, 1); // step start, move some data from PVR
            #endif
        }
        else
        {
            #ifdef ENABLE_MULTIMEDIA
            _MTPVR_SetSpeedExt(0, 0, 3); // step end
            #endif

            VDP_SetSpeed(B2R_1, STC_SPEED_TYPE_FORWARD_1X);

            VDP_SetFBNoTimeout(B2R_1, FALSE);
            VDP_SetStepForward(B2R_1, VDP_PLAY_NORMAL);
            VDP_FrcSetFreeze(B2R_1, FALSE);
            VDP_SetPauseMm(B2R_1, FALSE);

            VDP_SetTrickMode(B2R_1,VDP_PLAY_NORMAL);

            VDP_SetCbAttachedSrcId(B2R_1, ucEsId);
            VDP_RegCbFunc(VDP_CB_FUNC_MM_STEP_FIN_IND, (UINT32)0, 0, 0);
        }
    }
    else
    {
        MTVDEC_PRINT(" - invalid ucEsId(%d)\n", ucEsId);
        UnLockVdecSemaphoreMtal();
        return MTR_ERR_INV;
    }

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

static MT_RESULT_T _MTVDEC_SetFlushEsData(unsigned long arg)
{
    UCHAR ucEsId;
    LockVdecSemaphoreMtal();
    ucEsId = (UCHAR)arg;
    MTVDEC_PRINT(" - ucEsId = %d\n",(int)ucEsId);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }

    MPV_FlushEsmQ(ucEsId, FALSE);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

static MT_RESULT_T _MTVDEC_SetIPTVMode(unsigned long arg)
{
    UCHAR ucEsId;
    BOOL fgIPTVMode;
    MTAL_IOCTL_2ARG_T    rArg;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucEsId = (UCHAR)rArg.ai4Arg[0];
    fgIPTVMode = (BOOL)rArg.ai4Arg[1];
    MTVDEC_PRINT(" - fgIPTVMode = %d\n", fgIPTVMode);

    VDEC_SetParam(ucEsId, 17, (UINT32)fgIPTVMode, 0, 0);

    UnLockVdecSemaphoreMtal();

    return MTR_OK;
}

BOOL _MTVDEC_QueryVdecStatus(UCHAR ucEsId)
{
    if(ucEsId >= VDEC_MAX_ES)
    {
        return FALSE;
    }
    return _fgVdecPlayed[ucEsId];
}


static MT_RESULT_T _MTVDEC_GetThumbnail(unsigned long arg)
{
    MTAL_IOCTL_6ARG_T    rArg;

    UCHAR ucEsId;
    BOOL bIsOnePass;
    UINT16 u2CompId;
    MTVDEC_THUMBNAIL_INFO_T* prDstInfo;
    MTVDEC_CAPTURE_INTO_T* prSrcInfo;
    BOOL fgKeepRation;
    MTVDEC_THUMBNAIL_INFO_T rDstInfo;
    MTVDEC_CAPTURE_INTO_T rSrcInfo;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_6ARG(arg);
    COPY_FROM_USER_6ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    u2CompId = (UINT16)rArg.ai4Arg[1];
    bIsOnePass = (UINT32)rArg.ai4Arg[2];
    prDstInfo = (MTVDEC_THUMBNAIL_INFO_T*)rArg.ai4Arg[3];
    prSrcInfo = (MTVDEC_CAPTURE_INTO_T*)rArg.ai4Arg[4];
    fgKeepRation = (BOOL)rArg.ai4Arg[5];

    USR_SPACE_ACCESS_VALIDATE_ARG(prDstInfo, MTVDEC_THUMBNAIL_INFO_T);
    COPY_FROM_USER_ARG(prDstInfo, rDstInfo, MTVDEC_THUMBNAIL_INFO_T);
    USR_SPACE_ACCESS_VALIDATE_ARG(prSrcInfo, MTVDEC_CAPTURE_INTO_T);
    COPY_FROM_USER_ARG(prSrcInfo, rSrcInfo, MTVDEC_CAPTURE_INTO_T);

    MTVDEC_PRINT(" - _MTVDEC_GetThumbnail\n");
    if(GFX_Check_DirectFBMem((UINT32)rDstInfo.pucCanvasBuffer))
    {
        MTVDEC_PRINT("start first time flush\n");
        HalIOMMUInvalidateDCacheMultipleLine((UINT32)rDstInfo.pucCanvasBuffer,(UINT32)rDstInfo.pucCanvasBuffer,
            (rDstInfo.u4ThumbnailHeight*rDstInfo.u4CanvasPitch));
    }

    if(!VDEC_GetVideoThumbnail(ucEsId, u2CompId, bIsOnePass, (VDP_THUMBNAIL_INFO_T *)&rDstInfo, (VDP_CAPTURE_INTO_T *)&rSrcInfo, fgKeepRation))
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
    UnLockVdecSemaphoreMtal();

    return MTR_OK;
}



static MT_RESULT_T _MTVDEC_Play2(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    MTVDEC_FMT_T eMtVdecFmt;
    ENUM_VDEC_FMT_T eVdecFmt;
    VDEC_DEC_NFY_INFO_T rNfyInfo;
    MTVDEC_PLAYBACK_INTO_T *prPlayInfo;
    MTVDEC_PLAYBACK_INTO_T rPlayInfo;
    UCHAR ucEsId;
    BOOL fgRet = TRUE;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    eMtVdecFmt = (MTVDEC_FMT_T)rArg.ai4Arg[1];
    prPlayInfo = (MTVDEC_PLAYBACK_INTO_T*)rArg.ai4Arg[2];

    USR_SPACE_ACCESS_VALIDATE_ARG(prPlayInfo, MTVDEC_PLAYBACK_INTO_T);
    COPY_FROM_USER_ARG(prPlayInfo, rPlayInfo, MTVDEC_PLAYBACK_INTO_T);

    MTVDEC_PRINT(" - ucEsId = %d, eMtVdecFmt = %d\n",
        (int)ucEsId,
        (int)eMtVdecFmt);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    
    if (VDEC_NotifyStop(ucEsId))
    {
        MTVDEC_PRINT("VDEC_NotifyStop!!!!!!\n");
    }

    eVdecFmt = _MTVDEC_FmtMap(eMtVdecFmt);

    if(rPlayInfo.fgIFrame)
    {
        VDEC_Play_I_Frame(ucEsId, eVdecFmt);
    }
    else
    {
        if(rPlayInfo.fgMMPlayback)
        {
            VDEC_PlayMM(ucEsId, TRUE);
        }
        else
        {
            VDEC_PlayMM(ucEsId, FALSE);
        }
        if(_fgVdecPlayed[ucEsId] == FALSE)
        {
            rNfyInfo.pvTag = &_aucMTVDECEsId[ucEsId];
            rNfyInfo.pfDecNfy = _MTVDEC_VDEC_Nfy;
            VDEC_SetDecNfy(ucEsId, &rNfyInfo);
        }
        _fgVdecPlayed[ucEsId] = TRUE;
        fgRet = VDEC_Play(ucEsId, eVdecFmt);
    }

    if (fgRet)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_OK;
    }
    else
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
}


#if 0 // TODO: not used yet??
static MT_RESULT_T _MTVDEC_SetInfo(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    MTVDEC_SETINFO_T *prSetInfo;
    MTVDEC_SETINFO_T rSetInfo;
    UCHAR ucEsId;
    BOOL fgRet;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    prSetInfo = (MTVDEC_SETINFO_T*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prSetInfo, MTVDEC_SETINFO_T);
    COPY_FROM_USER_ARG(prSetInfo, rSetInfo, MTVDEC_SETINFO_T);

    MTVDEC_PRINT(" - _MTVDEC_SetInfo, ucEsId = %d, u4InfoMask = %d\n",
        (int)ucEsId,
        (int)rSetInfo.u4InfoMask);

    fgRet = TRUE;
    if(rSetInfo.u4InfoMask | MTVDEC_SETINFO_SUPERFREEZE)
    {
        fgRet = VDEC_SetSuperFreeze(ucEsId, rSetInfo.fgSuperFreeze);
    }

    if (fgRet)
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}
#endif


static MT_RESULT_T _MTVDEC_SetMMParam(unsigned long arg)
{
    MTAL_IOCTL_6ARG_T    rArg;
    UCHAR ucEsId;
//    BOOL fgRet;
    UINT32 u4Type;
    UINT32 u4SetType;
    UINT32 u4Param1;
    UINT32 u4Param2;
    UINT32 u4Param3;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_6ARG(arg);
    COPY_FROM_USER_6ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    u4Type = (UINT32)rArg.ai4Arg[1];
    u4Param1 = (UINT32)rArg.ai4Arg[2];
    u4Param2 = (UINT32)rArg.ai4Arg[3];
    u4Param3 = (UINT32)rArg.ai4Arg[4];

    if(u4Type == MTVDEC_MM_SEQ_INFO)
    {
        VDEC_SEQUENCE_DATA_T rSeqData;
        if(sizeof(VDEC_SEQUENCE_DATA_T) != u4Param2)
        {
            MTVDEC_PRINT(" - _MTVDEC_SetMMParam, MTVDEC_MM_SEQ_INFO size incorrect\n");
            UnLockVdecSemaphoreMtal();
            return MTR_NOT_OK;
        }
        x_memset(&rSeqData, 0, sizeof(VDEC_SEQUENCE_DATA_T));
        USR_SPACE_ACCESS_VALIDATE_ARG((VDEC_SEQUENCE_DATA_T*)u4Param1, VDEC_SEQUENCE_DATA_T);
        COPY_FROM_USER_ARG((VDEC_SEQUENCE_DATA_T*)u4Param1, rSeqData, VDEC_SEQUENCE_DATA_T);
        VDEC_SetMMParam(ucEsId, VDEC_MM_SEQ_INFO,
            (UINT32)&rSeqData, sizeof(VDEC_SEQUENCE_DATA_T), 0);
        UnLockVdecSemaphoreMtal();
        return MTR_OK;
    }
    else if(u4Type == MTVDEC_MM_TPT_SEQ_INFO)
    {
        VDEC_SEQUENCE_DATA_T rSeqData;
        if(sizeof(VDEC_SEQUENCE_DATA_T) != u4Param2)
        {
            MTVDEC_PRINT(" - _MTVDEC_SetMMParam, MTVDEC_MM_SEQ_INFO size incorrect\n");
            UnLockVdecSemaphoreMtal();
            return MTR_NOT_OK;
        }
        x_memset(&rSeqData, 0, sizeof(VDEC_SEQUENCE_DATA_T));
        USR_SPACE_ACCESS_VALIDATE_ARG((VDEC_SEQUENCE_DATA_T*)u4Param1, VDEC_SEQUENCE_DATA_T);
        COPY_FROM_USER_ARG((VDEC_SEQUENCE_DATA_T*)u4Param1, rSeqData, VDEC_SEQUENCE_DATA_T);
        VDEC_SetMMParam(ucEsId, VDEC_MM_TPT_SEQ_INFO,
            (UINT32)&rSeqData, sizeof(VDEC_SEQUENCE_DATA_T), 0);
        UnLockVdecSemaphoreMtal();
        return MTR_OK;
    }
    else if(u4Type == MTVDEC_MM_3D_INFO)
    {
        VDEC_SetMMParam(ucEsId, VDEC_MM_3D_INFO, _MTVDEC_3D_TypeMap(u4Param1), 0, 0);
        UnLockVdecSemaphoreMtal();
        return MTR_OK;
    }
    else if (u4Type == MTVDEC_MM_SEAMLESS_MODE)
    {
        VDEC_SetMMParam(ucEsId, VDEC_MM_SET_SEAMLESS_TARGET,u4Param1,0,0);
        MTVDEC_PRINT(" - _MTVDEC_SetMMParam, MTVDEC_MM_SEAMLESS_MODE to be:%d\n", u4Param1);
        UnLockVdecSemaphoreMtal();
        return MTR_OK;
    }
	else if (u4Type == MTVDEC_MM_HAL_PARAMETER)
	{
        VDEC_SetMMParam(ucEsId, VDEC_MM_HAL_PARAMETER,u4Param1,u4Param2,0);
        MTVDEC_PRINT(" - _MTVDEC_SetMMParam, MTVDEC_MM_HAL_PARAMETER to be:%d\n", u4Param1,u4Param2);
        UnLockVdecSemaphoreMtal();
        return MTR_OK;
	}


    u4SetType = _MTVDEC_MM_ParamMap(u4Type);
    if (VDEC_MM_MAX == u4SetType)
    {
        MTVDEC_PRINT(" - unknow set type %d\n", (int)u4Type);
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }

    VDEC_SetMMParam(ucEsId, u4SetType, u4Param1, u4Param2, u4Param3);
    UnLockVdecSemaphoreMtal();

    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_ClrMMParam(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UCHAR ucEsId;
//    BOOL fgRet;
    UINT32 u4Type;
//    UINT32 u4Param1;
//    UINT32 u4Param2;
//    UINT32 u4Param3;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    u4Type = (UCHAR)rArg.ai4Arg[1];

    VDEC_ClrMMParam(ucEsId, u4Type);
    UnLockVdecSemaphoreMtal();

    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_ChkCodecCap(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UCHAR ucEsId;
    BOOL fgRet;
    MTVDEC_FMT_T eVdecFmt;
    ENUM_VDEC_FMT_T eFmt;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    eVdecFmt = (MTVDEC_FMT_T)rArg.ai4Arg[1];

    eFmt = _MTVDEC_FmtMap(eVdecFmt);
    if(eFmt == VDEC_FMT_MAX)
    {
        MTVDEC_PRINT(" - _MTVDEC_ChkCodecCap, format unknown\n");
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
    fgRet = VDEC_ChkCodecCap(ucEsId, eFmt);

    if (fgRet)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_OK;
    }
    else
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
}


static MT_RESULT_T _MTVDEC_PlayCc(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    MTVDEC_PFN_CC_CB pCcCb;
    MPV_CC_NFY_INFO_T rCcNfy;
    UCHAR ucEsId;
//    BOOL fgRet;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    pCcCb = (MTVDEC_PFN_CC_CB)rArg.ai4Arg[1];

    MTVDEC_PRINT(" - ucEsId = %d, pCcCb = %d\n",
        (int)ucEsId,
        (int)pCcCb);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    x_memset(&rCcNfy, 0, sizeof(MPV_CC_NFY_INFO_T));
    rCcNfy.pvTag = (void*)(UINT32)ucEsId;
    if(pCcCb)
    {
        rCcNfy.pfCcNfyFct = (PFN_CC_NFY_FCT)_MTVDEC_CC2_Nfy;
    }
    else
    {
        rCcNfy.pfCcNfyFct = (PFN_CC_NFY_FCT)(void*)(NULL);
    }

    MPV_SetCcNfyFct(ucEsId, &rCcNfy);
    MPV_PlayCc(ucEsId);
    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_StopCc(unsigned long arg)
{
    UCHAR ucEsId;
    LockVdecSemaphoreMtal();
    ucEsId = (UCHAR)arg;
    MTVDEC_PRINT(" - ucEsId = %d\n",(int)ucEsId);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    MPV_StopCc(ucEsId);
    UnLockVdecSemaphoreMtal();

    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_AllocCcBuf(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT32 u4CcBufSize;
    UCHAR ucEsId;
    UINT32 u4Ret;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    u4CcBufSize = (UINT32)rArg.ai4Arg[1];

    MTVDEC_PRINT(" - ucEsId = %d, u4CcBufSize = %d\n",
        (int)ucEsId,
        (int)u4CcBufSize);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }

    if (_apucCcBuf[ucEsId] == NULL)
    {
        _apucCcBuf[ucEsId] = (UCHAR*)x_mem_alloc(u4CcBufSize);
    }
    else
    {
        _apucCcBuf[ucEsId] =
            (UCHAR*)x_mem_realloc((void*)(_apucCcBuf[ucEsId]), u4CcBufSize);
    }
    if(!_apucCcBuf[ucEsId])
    {
        MTVDEC_PRINT(" - ucEsId = %d, _apucCcBuf[ucEsId] = NULL\n",
            (int)ucEsId);
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
    _au4CcBufSize[ucEsId] = u4CcBufSize;

    u4Ret = MPV_AllocCcBuf(ucEsId, u4CcBufSize);

    if (u4Ret == 0)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_OK;
    }
    else
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
}


static MT_RESULT_T _MTVDEC_FreeCcBuf(unsigned long arg)
{
    UCHAR ucEsId;
    UINT32 u4Ret;
    LockVdecSemaphoreMtal();
    ucEsId = (UCHAR)arg;
    MTVDEC_PRINT(" - ucEsId = %d\n",(int)ucEsId);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    u4Ret = MPV_FreeCcBuf(ucEsId);

    if (u4Ret == 0)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_OK;
    }
    else
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
}


static MT_RESULT_T _MTVDEC_FlushCcBuf(unsigned long arg)
{
    UCHAR ucEsId;
    UINT32 u4Ret;
    LockVdecSemaphoreMtal();
    ucEsId = (UCHAR)arg;
    MTVDEC_PRINT(" - ucEsId = %d\n",(int)ucEsId);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    u4Ret = MPV_FlushCcBuf(ucEsId);

    if (u4Ret == 0)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_OK;
    }
    else
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
}


MT_RESULT_T _MTVDEC_GetCc(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    MTVDEC_CC_DATA_T* prCcData;
    MPV_CC_DATA rCcData;
    UCHAR ucEsId;
    UINT32 u4Ret;
    UCHAR* pucOrgBuf;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    prCcData = (MTVDEC_CC_DATA_T*)rArg.ai4Arg[1];

    MTVDEC_PRINT(" - ucEsId = %d, prCcData = 0x%x\n",
        (int)ucEsId,
        (int)prCcData);

    ASSERT(sizeof(MTVDEC_CC_DATA_T) == sizeof(MPV_CC_DATA));

    USR_SPACE_ACCESS_VALIDATE_ARG(prCcData, MTVDEC_CC_DATA_T);
    COPY_FROM_USER_ARG(prCcData, rCcData, MTVDEC_CC_DATA_T);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    if(rCcData.u4Size > _au4CcBufSize[ucEsId])
    {
        MTVDEC_PRINT(" - ucEsId = %d, u4Size(%d) > _au4CcBufSize[ucEsId](%d)\n",
            (int)rCcData.u4Size,
            (int)_au4CcBufSize[ucEsId]);
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }

    pucOrgBuf = rCcData.pucCcData;
    rCcData.pucCcData = _apucCcBuf[ucEsId];
    u4Ret = MPV_GetCc(ucEsId, &rCcData);
    rCcData.pucCcData = pucOrgBuf;

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pucOrgBuf, rCcData.u4Size);
    COPY_TO_USER_ARG_SIZE(pucOrgBuf,_apucCcBuf[ucEsId],rCcData.u4Size);

    USR_SPACE_ACCESS_VALIDATE_ARG(prCcData, MTVDEC_CC_DATA_T);
    COPY_TO_USER_ARG(prCcData,rCcData,MTVDEC_CC_DATA_T);

    if (u4Ret == 0)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_OK;
    }
    else
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }
}


MT_RESULT_T _MTVDEC_GetDbkInfo2(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    MTVDEC_DBK_INFO2_T* prDbkInfo;
    MTVDEC_DBK_INFO2_T rDbkInfo;
    UCHAR ucEsId;
//    UINT32 u4Ret;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    prDbkInfo = (MTVDEC_DBK_INFO2_T*)rArg.ai4Arg[1];

    MTVDEC_PRINT(" - ucEsId = %d, prDbkInfo = 0x%x\n",
        (int)ucEsId,
        (int)prDbkInfo);

    x_memset(&rDbkInfo, 0, sizeof(MTVDEC_DBK_INFO2_T));

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    MPV_GetDbkInfo(ucEsId,
        &rDbkInfo.fgDeblocking,
        &rDbkInfo.u2X,
        &rDbkInfo.u2Y,
        &rDbkInfo.u2W,
        &rDbkInfo.u2H);

    USR_SPACE_ACCESS_VALIDATE_ARG(prDbkInfo, MTVDEC_DBK_INFO2_T);
    COPY_TO_USER_ARG(prDbkInfo,rDbkInfo,MTVDEC_DBK_INFO2_T);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}


MT_RESULT_T _MTVDEC_SetDbkInfo2(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    MTVDEC_DBK_INFO2_T* prDbkInfo;
    MTVDEC_DBK_INFO2_T rDbkInfo;
    UCHAR ucEsId;
//    UINT32 u4Ret;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    prDbkInfo = (MTVDEC_DBK_INFO2_T*)rArg.ai4Arg[1];

    MTVDEC_PRINT(" - ucEsId = %d, prDbkInfo = 0x%x\n",
        (int)ucEsId,
        (int)prDbkInfo);

    USR_SPACE_ACCESS_VALIDATE_ARG(prDbkInfo, MTVDEC_DBK_INFO2_T);
    COPY_FROM_USER_ARG(prDbkInfo, rDbkInfo, MTVDEC_DBK_INFO2_T);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    MPV_SetDbkInfo(ucEsId,
        rDbkInfo.fgDeblocking,
        rDbkInfo.u2X,
        rDbkInfo.u2Y,
        rDbkInfo.u2W,
        rDbkInfo.u2H);
    UnLockVdecSemaphoreMtal();

    return MTR_OK;
}


MT_RESULT_T _MTVDEC_AllocIFrameBuf(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    MTVDEC_DEC_I_FRAME_BUFF_T* prIFrameBuf;
    MTVDEC_DEC_I_FRAME_BUFF_T rIFrameBuf;
    UCHAR ucEsId;
//    UINT32 u4Ret;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    prIFrameBuf = (MTVDEC_DEC_I_FRAME_BUFF_T*)rArg.ai4Arg[1];

    MTVDEC_PRINT(" - ucEsId = %d, prDbkInfo = 0x%x\n",
        (int)ucEsId,
        (int)prIFrameBuf);

    USR_SPACE_ACCESS_VALIDATE_ARG(prIFrameBuf, MTVDEC_DEC_I_FRAME_BUFF_T);
    COPY_FROM_USER_ARG(prIFrameBuf, rIFrameBuf, MTVDEC_DEC_I_FRAME_BUFF_T);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    ASSERT(sizeof(MPV_DEC_I_FRAME_BUFF_T) == sizeof(MTVDEC_DEC_I_FRAME_BUFF_T));

    if(MPV_AllocIFrameBuf(ucEsId, (MPV_DEC_I_FRAME_BUFF_T*)&rIFrameBuf) != MPV_DRV_OK)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }

    _apucIFrameBuf[ucEsId] = rIFrameBuf.pucIFrameBufAddr;
    _au4IFrameBufSize[ucEsId] = rIFrameBuf.u4Size;

    USR_SPACE_ACCESS_VALIDATE_ARG(prIFrameBuf, MTVDEC_DEC_I_FRAME_BUFF_T);
    COPY_TO_USER_ARG(prIFrameBuf,rIFrameBuf,MTVDEC_DEC_I_FRAME_BUFF_T);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_FreeIFrameBuf(unsigned long arg)
{
    UCHAR ucEsId;
    LockVdecSemaphoreMtal();
    ucEsId = (UCHAR)arg;
    MTVDEC_PRINT(" - ucEsId = %d\n",(int)ucEsId);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }

    _apucIFrameBuf[ucEsId] = 0;
    _au4IFrameBufSize[ucEsId] = 0;

    if(MPV_FreeIFrameBuf(ucEsId) != MPV_DRV_OK)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}


MT_RESULT_T _MTVDEC_SetIFrameBuf(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    MTVDEC_DEC_I_FRAME_BUFF_T* prIFrameBuf;
    MTVDEC_DEC_I_FRAME_BUFF_T rIFrameBuf;
    UCHAR ucEsId;
//    UINT32 u4Ret;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    prIFrameBuf = (MTVDEC_DEC_I_FRAME_BUFF_T*)rArg.ai4Arg[1];

    MTVDEC_PRINT(" - ucEsId = %d, prDbkInfo = 0x%x\n",
        (int)ucEsId,
        (int)prIFrameBuf);

    USR_SPACE_ACCESS_VALIDATE_ARG(prIFrameBuf, MTVDEC_DEC_I_FRAME_BUFF_T);
    COPY_FROM_USER_ARG(prIFrameBuf, rIFrameBuf, MTVDEC_DEC_I_FRAME_BUFF_T);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        MTVDEC_PRINT(" - ucEsId = %d > VDEC_MAX_ES\n",
            (int)ucEsId);
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    ASSERT(sizeof(MPV_DEC_I_FRAME_BUFF_T) == sizeof(MTVDEC_DEC_I_FRAME_BUFF_T));

    // user mode use 0-base address
    if(!_apucIFrameBuf[ucEsId])
    {
        MTVDEC_PRINT(" - _apucIFrameBuf[ucEsId] 0 error\n");
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    if(((UINT32)rIFrameBuf.pucIFrameBufAddr) > _au4IFrameBufSize[ucEsId])
    {
        MTVDEC_PRINT(" - pucIFrameBufAddr too big\n");
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    if(((UINT32)rIFrameBuf.u4Size) > _au4IFrameBufSize[ucEsId])
    {
        MTVDEC_PRINT(" - u4Size too big\n");
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    rIFrameBuf.pucIFrameBufAddr += (UINT32)_apucIFrameBuf[ucEsId];

    if(MPV_SetIFrameBuf(ucEsId, (MPV_DEC_I_FRAME_BUFF_T*)&rIFrameBuf) != MPV_DRV_OK)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prIFrameBuf, MTVDEC_DEC_I_FRAME_BUFF_T);
    COPY_TO_USER_ARG(prIFrameBuf,rIFrameBuf,MTVDEC_DEC_I_FRAME_BUFF_T);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}


MT_RESULT_T _MTVDEC_SetIFrameDec(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    MTVDEC_DEC_I_FRAME_BUFF_T* prIFrameBuf;
    MTVDEC_DEC_I_FRAME_BUFF_T rIFrameBuf;
    UCHAR ucEsId;
//    UINT32 u4Ret;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    prIFrameBuf = (MTVDEC_DEC_I_FRAME_BUFF_T*)rArg.ai4Arg[1];

    MTVDEC_PRINT(" - ucEsId = %d, prDbkInfo = 0x%x\n",
        (int)ucEsId,
        (int)prIFrameBuf);

    USR_SPACE_ACCESS_VALIDATE_ARG(prIFrameBuf, MTVDEC_DEC_I_FRAME_BUFF_T);
    COPY_FROM_USER_ARG(prIFrameBuf, rIFrameBuf, MTVDEC_DEC_I_FRAME_BUFF_T);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    ASSERT(sizeof(MPV_DEC_I_FRAME_BUFF_T) == sizeof(MTVDEC_DEC_I_FRAME_BUFF_T));

    // user mode use 0-base address
    if(!_apucIFrameBuf[ucEsId])
    {
        MTVDEC_PRINT(" - _apucIFrameBuf[ucEsId] 0 error\n");
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    if(((UINT32)rIFrameBuf.pucIFrameBufAddr) > _au4IFrameBufSize[ucEsId])
    {
        MTVDEC_PRINT(" - pucIFrameBufAddr too big. Addr(%d) BufSize(0x%X)\n", 
                       (UINT32)rIFrameBuf.pucIFrameBufAddr, _au4IFrameBufSize[ucEsId]);
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    if(((UINT32)rIFrameBuf.u4Size) > _au4IFrameBufSize[ucEsId])
    {
        MTVDEC_PRINT(" - u4Size too big\n");
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    rIFrameBuf.pucIFrameBufAddr += (UINT32)_apucIFrameBuf[ucEsId];

    if(MPV_SetIFrameDec(ucEsId, (MPV_DEC_I_FRAME_BUFF_T*)&rIFrameBuf) != MPV_DRV_OK)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prIFrameBuf, MTVDEC_DEC_I_FRAME_BUFF_T);
    COPY_TO_USER_ARG(prIFrameBuf,rIFrameBuf,MTVDEC_DEC_I_FRAME_BUFF_T);
    UnLockVdecSemaphoreMtal();

    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_RegAutotestCb(unsigned long arg)
{
#ifdef MULTIMEDIA_AUTOTEST_SUPPORT
    UCHAR ucEsId;
    MTAL_IOCTL_2ARG_T    rArg;
    MTVDEC_PFN_AUTOTEST_CB prAutotestCb;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    prAutotestCb = (MTVDEC_PFN_AUTOTEST_CB)rArg.ai4Arg[1];

    if(prAutotestCb)
    {
        VDEC_SetAutoTestDecNfy(ucEsId, _MTVDEC_AUTOTEST_Nfy, ucEsId);
    }
    else
    {
        VDEC_SetAutoTestDecNfy(ucEsId, NULL, ucEsId);
    }

    MTVDEC_PRINT(" - prAutotestCb = 0x%x\n",(int)prAutotestCb);
    UnLockVdecSemaphoreMtal();
#endif

    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_SetCrcGolden(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UCHAR ucEsId;
    UINT32 u4CrcGolden;
//    BOOL fgRet;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    u4CrcGolden = (UINT32)rArg.ai4Arg[1];

    MTVDEC_PRINT(" - ucEsId = %d, u4CrcGolden = %d\n",
        (int)ucEsId,
        (int)u4CrcGolden);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
#ifdef MULTIMEDIA_AUTOTEST_SUPPORT
    VDEC_SetCrcGolden(ucEsId, u4CrcGolden);
    UnLockVdecSemaphoreMtal();
    return MTR_OK;
#else
    UnLockVdecSemaphoreMtal();
    return MTR_NOT_OK;
#endif
}


static MT_RESULT_T _MTVDEC_SetAutoTestCurFileDir(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UCHAR ucEsId;
    UCHAR* pucAutoPlayCurFileDir;
    UCHAR ucAutoPlayCurFileDir[512];
    UINT32 u4Size;
//    BOOL fgRet;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    pucAutoPlayCurFileDir = (UCHAR*)rArg.ai4Arg[1];
    u4Size = (UINT32)rArg.ai4Arg[2];

    MTVDEC_PRINT(" - ucEsId = %d, u4Size = %d\n",
        (int)ucEsId,
        (int)u4Size);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }

    if(u4Size > 512)
    {
        u4Size = 512;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pucAutoPlayCurFileDir, u4Size);
    COPY_FROM_USER_ARG_SIZE(pucAutoPlayCurFileDir, ucAutoPlayCurFileDir[0], u4Size);
#if defined(MULTIMEDIA_AUTOTEST_SUPPORT) && defined(VDEC_TIME_PROFILE)
    VDEC_SetAutoTestCurFileDir(ucEsId, ucAutoPlayCurFileDir);
    UnLockVdecSemaphoreMtal();

    return MTR_OK;
#else
    UnLockVdecSemaphoreMtal();
    return MTR_NOT_OK;
#endif
}


static MT_RESULT_T _MTVDEC_SetAutoTestCurFileName(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T    rArg;
    UCHAR ucEsId;
    UCHAR* pucAutoPlayCurFileName;
    UCHAR ucAutoPlayCurFileName[256];
    UINT32 u4Size;
//    BOOL fgRet;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    pucAutoPlayCurFileName = (UCHAR*)rArg.ai4Arg[1];
    u4Size = (UINT32)rArg.ai4Arg[2];

    MTVDEC_PRINT(" - ucEsId = %d, u4Size = %d\n",
        (int)ucEsId,
        (int)u4Size);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    if(u4Size > 256)
    {
        u4Size = 256;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(pucAutoPlayCurFileName, u4Size);
    COPY_FROM_USER_ARG_SIZE(pucAutoPlayCurFileName, ucAutoPlayCurFileName[0], u4Size);
#if defined(MULTIMEDIA_AUTOTEST_SUPPORT) && defined(VDEC_TIME_PROFILE)
    VDEC_SetAutoTestCurFileName(ucEsId, ucAutoPlayCurFileName);
    UnLockVdecSemaphoreMtal();

    return MTR_OK;
#else
    UnLockVdecSemaphoreMtal();
    return MTR_NOT_OK;
#endif
}



static MT_RESULT_T _MTVDEC_ChkDivXBonding(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UCHAR ucEsId;
    BOOL *pfgValue;
    BOOL fgValue;
//    BOOL fgRet;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    pfgValue = (BOOL *)rArg.ai4Arg[1];

    MTVDEC_PRINT(" - ucEsId = %d\n");

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }
    UNUSED(ucEsId);
    fgValue = VDEC_ChkDivXBonding();

    USR_SPACE_ACCESS_VALIDATE_ARG(pfgValue, BOOL);
    COPY_TO_USER_ARG(pfgValue,fgValue,BOOL);
    UnLockVdecSemaphoreMtal();

    return MTR_OK;
}
static MT_RESULT_T _MTVDEC_RmSet(unsigned long arg)
{
#ifdef CC_VDEC_RM_SUPPORT
    MTAL_IOCTL_4ARG_T   rArg;
    UCHAR ucEsId;
    MTVDEC_SET_T eVdecSetType;
    UINT16 zSetInfoSize;
    
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    eVdecSetType = (MTVDEC_SET_T)rArg.ai4Arg[1];
    
    zSetInfoSize = (UINT16)rArg.ai4Arg[3]; 
    
    MTVDEC_PRINT(" - ucEsId = %d, eVdecSetType(%d).\n",
        (int)ucEsId,
        (int)eVdecSetType);

    switch (eVdecSetType)
    {
        case MTVDEC_SET_INST_PRIORITY: // input: UINT32 * 
            {
                UINT32 u4Priority = 0;
                USR_SPACE_ACCESS_VALIDATE_ARG(rArg.ai4Arg[2], UINT32);
                COPY_FROM_USER_ARG(rArg.ai4Arg[2], u4Priority, UINT32);
                VDEC_RmSetPriority(ucEsId, u4Priority);   
            }
            break;
        case MTVDEC_SET_INST_KICKDONE: // input: BOOL *
            {
                BOOL fgSuccess = 0;
                USR_SPACE_ACCESS_VALIDATE_ARG(rArg.ai4Arg[2], BOOL);
                COPY_FROM_USER_ARG(rArg.ai4Arg[2], fgSuccess, BOOL);
                VDEC_RmSetKickDone(ucEsId, fgSuccess);   
            }
            break;
        case MTVDEC_SET_IS_VDP_RENDER: // input: BOOL *
            {
                BOOL fgIsVDPRender = 0;
                USR_SPACE_ACCESS_VALIDATE_ARG(rArg.ai4Arg[2], BOOL);
                COPY_FROM_USER_ARG(rArg.ai4Arg[2], fgIsVDPRender, BOOL);
                VDEC_RmSetIsVdpRender(ucEsId, fgIsVDPRender);   
            }
            break;
        #ifdef CC_INPUTSOURCE_VIDEO_ON_TEXTURE
        case MTVDEC_SET_TEXTURE_MODE:
            {
                VDEC_RmSetTextureMode(ucEsId);   
            }
            break;
        #endif
        case MTVDEC_SET_IMGRZ_ID:
            {
                UCHAR ucImgrzIdMsk = (UCHAR)rArg.ai4Arg[2];
                VDEC_RmSetImgrzIdMsk(ucEsId, ucImgrzIdMsk);   
            }
            break;
        case MTVDEC_SET_ADD_IMGRZ_ID:
            {
                UCHAR ucImgrzIdMsk = 0; //(UCHAR)rArg.ai4Arg[2];
                VDEC_RmGetImgrzIdMsk(ucEsId, &ucImgrzIdMsk);
                ucImgrzIdMsk = ucImgrzIdMsk | (UCHAR)rArg.ai4Arg[2];
                VDEC_RmSetImgrzIdMsk(ucEsId, ucImgrzIdMsk);   
            }
            break;
        default:
            break;
    }

    UnLockVdecSemaphoreMtal();
#endif    
    return MTR_OK;
}


static MT_RESULT_T _MTVDEC_SetABInfo(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T   rArg;
    MTVDEC_AB_INFO_T*   prABInfo;
    MTVDEC_AB_INFO_T    rAbInfo;
    UCHAR ucEsId;
//    BOOL fgRet;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    prABInfo = (MTVDEC_AB_INFO_T*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prABInfo, MTVDEC_AB_INFO_T);
    COPY_FROM_USER_ARG(prABInfo, rAbInfo, MTVDEC_AB_INFO_T);

    MTVDEC_PRINT(" - ucEsId = %d\n");

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }

    VDEC_SetABInfo(ucEsId, (VDEC_AB_INTO_T *)&rAbInfo);
    UnLockVdecSemaphoreMtal();

    return MTR_OK;
}

static MT_RESULT_T _MTVDEC_SetSrcId(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UCHAR ucEsId;
    UINT8 u1SwdmxInstId;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    u1SwdmxInstId = (UINT8)rArg.ai4Arg[1];

    MTVDEC_PRINT(" - ucEsId = %d, u1SwdmxInstId = %d\n",
    (int)ucEsId,
    (int)u1SwdmxInstId);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    VDEC_SetSrcId(ucEsId, u1SwdmxInstId);
    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

static MT_RESULT_T _MTVDEC_PlayMM(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UCHAR ucEsId;
    BOOL fgPlayMM;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    fgPlayMM = (BOOL)rArg.ai4Arg[1];

    MTVDEC_PRINT(" - ucEsId = %d, fgPlayMM = %d\n",
    (int)ucEsId,
    (int)fgPlayMM);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    VDEC_PlayMM(ucEsId, fgPlayMM);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

static MT_RESULT_T _MTVDEC_PlayNotDisplay(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UCHAR ucEsId;
    BOOL fgNotDisplay;
    LockVdecSemaphoreMtal();

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    fgNotDisplay = (BOOL)rArg.ai4Arg[1];

    MTVDEC_PRINT(" - ucEsId = %d, fgNotDisplay = %d\n",
    (int)ucEsId,
    (int)fgNotDisplay);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }

    VDEC_PlayNotDisplay(ucEsId, fgNotDisplay);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

static MT_RESULT_T _MTVDEC_RePlayVdec(unsigned long arg)
{
    UCHAR ucEsId;
    LockVdecSemaphoreMtal();
    ucEsId = (UCHAR)arg;
    MTVDEC_PRINT(" - ucEsId = %d\n",(int)ucEsId);

    //add driver implementation code here
    if(ucEsId >= VDEC_MAX_ES)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_SUPPORTED;
    }
    VDEC_RePlayVdec(ucEsId);

    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}

#ifdef CC_BOOT_VDEC_LOGO
static MT_RESULT_T _MTVDEC_GetBootLogo(unsigned long arg)
{
    BOOL fgLogoOn;
    UINT8 u1Valid;
    MTVDEC_BOOT_LOGO_GET_T  rLogoInfo;
    MTVDEC_BOOT_LOGO_GET_T* prLogoInfo;
    LockVdecSemaphoreMtal();

    prLogoInfo = (MTVDEC_BOOT_LOGO_GET_T*)arg;

    ASSERT(prLogoInfo != NULL);
    x_memset(&rLogoInfo, 0, sizeof(MTVDEC_BOOT_LOGO_GET_T));

#if 0
    ASSERT(sizeof(MTVDEC_CC_DATA_T) == sizeof(MPV_CC_DATA));
    USR_SPACE_ACCESS_VALIDATE_ARG(prLogoInfo, MTVDEC_BOOT_LOGO_GET_T);
    COPY_FROM_USER_ARG(prLogoInfo, rLogoInfo, MTVDEC_BOOT_LOGO_GET_T);
    fgLogoOn = MTDRVCUST_OptGet(eDrvLogoVdecEnable);
#endif

    fgLogoOn = DRVCUST_OptGet(eDrvLogoVdecEnable);

    if(!fgLogoOn)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }

    if(!_fgVdecBootLogoRead)
    {
        _fgVdecBootLogoRead = TRUE;
        _rVdecBootLogo.aucReg[0] = bApiEepromReadByte(EEP_BOOT_VDEC_LOGO_CTRL);
        u1Valid = bApiEepromReadByte(EEP_BOOT_VDEC_LOGO_VALID_2);
        _u4Valid = (UINT32)u1Valid;
        _u4Valid <<= 8;
        u1Valid = bApiEepromReadByte(EEP_BOOT_VDEC_LOGO_VALID_1);
        _u4Valid += (UINT32)u1Valid;
    }

    rLogoInfo.fgDefault = (BOOL)_rVdecBootLogo.rField.ucDefault;
    rLogoInfo.fgDefault_exist = (BOOL)DRVCUST_OptGet(eDrvLogoVdecDefault);
    rLogoInfo.fgOnoff = (BOOL)_rVdecBootLogo.rField.ucEnable;
    rLogoInfo.u1Data_Select = _rVdecBootLogo.rField.ucSelect;
    rLogoInfo.u4Valid = _u4Valid;
    rLogoInfo.u1_Slot_Num = (UINT8)DRVCUST_OptGet(eDrvLogoVdecFlashSlotNum);
    rLogoInfo.u4Flash_Addr = (UINT32)DRVCUST_OptGet(eDrvLogoVdecFlashAddr);
    rLogoInfo.u4Flash_Size = (UINT32)DRVCUST_OptGet(eDrvLogoVdecFlashSize);

    USR_SPACE_ACCESS_VALIDATE_ARG(prLogoInfo, MTVDEC_BOOT_LOGO_GET_T);
    COPY_TO_USER_ARG(prLogoInfo, rLogoInfo, MTVDEC_BOOT_LOGO_GET_T);
    UnLockVdecSemaphoreMtal();

    return MTR_OK;
}
#endif

#ifdef CC_BOOT_VDEC_LOGO
static MT_RESULT_T _MTVDEC_GetBootLogo_Addr(unsigned long arg)
{
    BOOL fgLogoOn;
    MTVDEC_BOOT_LOGO_ADDR_GET_T  rLogoAddr;
    MTVDEC_BOOT_LOGO_ADDR_GET_T* prLogoAddr;
    LockVdecSemaphoreMtal();

    prLogoAddr = (MTVDEC_BOOT_LOGO_ADDR_GET_T*)arg;

    ASSERT(prLogoAddr != NULL);
    x_memset(&rLogoAddr, 0, sizeof(MTVDEC_BOOT_LOGO_ADDR_GET_T));

    fgLogoOn = DRVCUST_OptGet(eDrvLogoVdecEnable);
    if(!fgLogoOn)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }

    if(!_fgVdecBootLogoAddrRead)
    {
        _fgVdecBootLogoAddrRead = TRUE;
        _ucDataType = bApiEepromReadByte(EEP_BOOT_VDEC_LOGO_DATA_TYPE );

        _u4DataSize = (UINT32)bApiEepromReadByte(EEP_BOOT_VDEC_LOGO_DATA_SIZE );
        _u4DataSize <<= 8;
        _u4DataSize += (UINT32)bApiEepromReadByte(EEP_BOOT_VDEC_LOGO_DATA_SIZE + 1);
        _u4DataSize <<= 8;
        _u4DataSize += (UINT32)bApiEepromReadByte(EEP_BOOT_VDEC_LOGO_DATA_SIZE + 2);
        _u4DataSize <<= 8;
        _u4DataSize += (UINT32)bApiEepromReadByte(EEP_BOOT_VDEC_LOGO_DATA_SIZE + 3);

        _u4DataAddr = (UINT32)bApiEepromReadByte(EEP_BOOT_VDEC_LOGO_DATA_ADDR);
        _u4DataAddr <<= 8;
        _u4DataAddr += (UINT32)bApiEepromReadByte(EEP_BOOT_VDEC_LOGO_DATA_ADDR + 1);
        _u4DataAddr <<= 8;
        _u4DataAddr += (UINT32)bApiEepromReadByte(EEP_BOOT_VDEC_LOGO_DATA_ADDR + 2);
        _u4DataAddr <<= 8;
        _u4DataAddr += (UINT32)bApiEepromReadByte(EEP_BOOT_VDEC_LOGO_DATA_ADDR + 3);
    }

    rLogoAddr.u4Data_Addr = _u4DataAddr;
    rLogoAddr.u4Data_Size = _u4DataSize;
    rLogoAddr.u1Data_Type = _ucDataType;

    USR_SPACE_ACCESS_VALIDATE_ARG(prLogoAddr, MTVDEC_BOOT_LOGO_ADDR_GET_T);
    COPY_TO_USER_ARG(prLogoAddr,rLogoAddr, MTVDEC_BOOT_LOGO_ADDR_GET_T);
    UnLockVdecSemaphoreMtal();

    return MTR_OK;
}
#endif

#ifdef CC_BOOT_VDEC_LOGO
static MT_RESULT_T _MTVDEC_SetBootLogo(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    BOOL fgLogoOn;
    BOOL fgStore;
    UINT8 u1Valid1 = 0;
    UINT8 u1Valid2 = 0;
    MTVDEC_BOOT_LOGO_SET_T* prLogoInfo;
    MTVDEC_BOOT_LOGO_SET_T rLogoInfo;

    LockVdecSemaphoreMtal();
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prLogoInfo = (MTVDEC_BOOT_LOGO_SET_T*)rArg.ai4Arg[0];
    fgStore = (BOOL)rArg.ai4Arg[1];

    ASSERT(prLogoInfo != NULL);

    fgLogoOn = DRVCUST_OptGet(eDrvLogoVdecEnable);
    if(!fgLogoOn)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prLogoInfo, MTVDEC_BOOT_LOGO_SET_T);
    COPY_FROM_USER_ARG(prLogoInfo, rLogoInfo, MTVDEC_BOOT_LOGO_SET_T);

    _rVdecBootLogo.rField.ucDefault = (UCHAR)rLogoInfo.fgDefault;
    _rVdecBootLogo.rField.ucEnable = (UCHAR)rLogoInfo.fgOnoff;
    _rVdecBootLogo.rField.ucSelect= (UCHAR)rLogoInfo.u1Data_Select;
    _u4Valid = rLogoInfo.u4Valid;

    if(fgStore)
    {
        _fgVdecBootLogoRead = TRUE;
        fgApiEepromWriteByte(EEP_BOOT_VDEC_LOGO_CTRL, _rVdecBootLogo.aucReg[0]);
        u1Valid1 = (UINT8)(_u4Valid & 0xFF);
        u1Valid2 = (UINT8)((_u4Valid >> 8) & 0xFF);
        fgApiEepromWriteByte(EEP_BOOT_VDEC_LOGO_VALID_1, u1Valid1);
        fgApiEepromWriteByte(EEP_BOOT_VDEC_LOGO_VALID_2, u1Valid2);
    }
    UnLockVdecSemaphoreMtal();

    return MTR_OK;
}
#endif

#ifdef CC_BOOT_VDEC_LOGO
static MT_RESULT_T _MTVDEC_SetBootLogo_Addr(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    BOOL fgLogoOn;
    BOOL fgStore;
    UINT32 u4Tmp;
    UCHAR ucDataType;
    MTVDEC_BOOT_LOGO_ADDR_SET_T* prLogoAddr;
    MTVDEC_BOOT_LOGO_ADDR_SET_T rLogoAddr;

    LockVdecSemaphoreMtal();
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    prLogoAddr = (MTVDEC_BOOT_LOGO_ADDR_SET_T*)rArg.ai4Arg[0];
    ucDataType = (BOOL)rArg.ai4Arg[1];
    fgStore = (BOOL)rArg.ai4Arg[2];

    ASSERT(prLogoAddr != NULL);

    fgLogoOn = DRVCUST_OptGet(eDrvLogoVdecEnable);
    if(!fgLogoOn)
    {
        UnLockVdecSemaphoreMtal();
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prLogoAddr, MTVDEC_BOOT_LOGO_ADDR_SET_T);
    COPY_FROM_USER_ARG(prLogoAddr, rLogoAddr, MTVDEC_BOOT_LOGO_ADDR_SET_T);

    _u4DataAddr = rLogoAddr.u4Data_Addr;
    _u4DataSize = rLogoAddr.u4Data_Size;
    _ucDataType = ucDataType;

    if(fgStore)
    {
        _fgVdecBootLogoAddrRead = TRUE;
        fgApiEepromWriteByte((EEP_BOOT_VDEC_LOGO_DATA_TYPE),
            _ucDataType);

        u4Tmp = _u4DataSize;
        fgApiEepromWriteByte((EEP_BOOT_VDEC_LOGO_DATA_SIZE + 3),
                                (UCHAR)(u4Tmp & 0xFF));
        u4Tmp >>= 8;
        fgApiEepromWriteByte((EEP_BOOT_VDEC_LOGO_DATA_SIZE + 2),
                                (UCHAR)(u4Tmp & 0xFF));
        u4Tmp >>= 8;
        fgApiEepromWriteByte((EEP_BOOT_VDEC_LOGO_DATA_SIZE + 1),
                                (UCHAR)(u4Tmp & 0xFF));
        u4Tmp >>= 8;
        fgApiEepromWriteByte((EEP_BOOT_VDEC_LOGO_DATA_SIZE),
                                (UCHAR)(u4Tmp & 0xFF));

        u4Tmp = _u4DataAddr;
        fgApiEepromWriteByte((EEP_BOOT_VDEC_LOGO_DATA_ADDR + 3),
                                (UCHAR)(u4Tmp & 0xFF));
        u4Tmp >>= 8;
        fgApiEepromWriteByte((EEP_BOOT_VDEC_LOGO_DATA_ADDR + 2),
                                (UCHAR)(u4Tmp & 0xFF));
        u4Tmp >>= 8;
        fgApiEepromWriteByte((EEP_BOOT_VDEC_LOGO_DATA_ADDR + 1),
                                (UCHAR)(u4Tmp & 0xFF));
        u4Tmp >>= 8;
        fgApiEepromWriteByte((EEP_BOOT_VDEC_LOGO_DATA_ADDR),
                                (UCHAR)(u4Tmp & 0xFF));
    }
    UnLockVdecSemaphoreMtal();

    return MTR_OK;
}
#endif

static MT_RESULT_T _MTVDEC_SetSuperFreeze(unsigned long arg)
{
    MT_RESULT_T          eRet = MTR_OK;
    MTAL_IOCTL_2ARG_T    rArg;
    UCHAR ucEsId;
    BOOL fgSuperFreeze;

    LockVdecSemaphoreMtal();
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    fgSuperFreeze = (BOOL)rArg.ai4Arg[1];

    if (!VDEC_SetSuperFreeze(ucEsId, fgSuperFreeze))
    {
        eRet = MTR_NOT_OK;
    }

    UnLockVdecSemaphoreMtal();
    return eRet;
}

static MT_RESULT_T _MTVDEC_SetSkipMode(unsigned long arg)
{
    MT_RESULT_T          eRet = MTR_OK;
    MTAL_IOCTL_2ARG_T    rArg;
    UCHAR ucEsId;
    BOOL fgSkipMode;

    LockVdecSemaphoreMtal();
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    ucEsId = (UCHAR)rArg.ai4Arg[0];
    fgSkipMode = (BOOL)rArg.ai4Arg[1];

    if (!VDEC_SetSkipMode(ucEsId, fgSkipMode))
    {
        eRet = MTR_NOT_OK;
    }

    UnLockVdecSemaphoreMtal();
    return eRet;
}

static MT_RESULT_T _MTVDEC_GetDisplayFrameBufferAddr(unsigned long arg)
{
    MT_RESULT_T             eRet = MTR_OK;
    MTAL_IOCTL_2ARG_T       rArg;
    VDP_CAPTURE_INTO_T    rCapInfo;
    VDP_CAPTURE_INTO_T    *prCapInfo = NULL;
    UCHAR ucEsId;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    prCapInfo = (VDP_CAPTURE_INTO_T *)rArg.ai4Arg[1];
    LockVdecSemaphoreMtal();

    MTVDEC_PRINT("-ucEsId = %u\n", ucEsId);

    if (NULL == prCapInfo)
    {
        MTVDEC_PRINT("- prCapInfo=NULL, Err... ...\n");
        eRet = MTR_PARAMETER_ERROR;
        goto EXIT_PORT;
    }

    // Maybe there exist something (none in fact now) that driver need to known
    COPY_FROM_USER(prCapInfo, rCapInfo, sizeof(VDP_CAPTURE_INTO_T));
    if (!VDEC_GetDisplayFrameBufferAddr(ucEsId, &rCapInfo))
    {
        eRet = MTR_NOT_OK;
        MTVDEC_PRINT("- Get CapInfo Err... ...\n");
        goto EXIT_PORT;
    }
    COPY_TO_USER(prCapInfo, rCapInfo, sizeof(VDP_CAPTURE_INTO_T));

EXIT_PORT:
    UnLockVdecSemaphoreMtal();
    return eRet;
}

static MT_RESULT_T _MTVDEC_CtrlValidSrc(unsigned long arg)
{
    MT_RESULT_T         eRet = MTR_OK;
    UCHAR               ucEsId = 0;
    BOOL                fgFree;
    UCHAR               *pucEsId = NULL;
    MTAL_IOCTL_2ARG_T    rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    pucEsId = (UCHAR *)rArg.ai4Arg[0];
    fgFree = (BOOL)rArg.ai4Arg[1];

    if (pucEsId)
    {
        COPY_FROM_USER(pucEsId, ucEsId, sizeof(UCHAR));
        if (!fgFree)
        {
            if (!VDEC_TryLockValidSrc(&ucEsId))
            {
                eRet = MTR_NOT_OK;
            }
            COPY_TO_USER(pucEsId, ucEsId, sizeof(UCHAR));
        }
        else
        {
            if (!VDEC_UnlockSrc(ucEsId))
            {
                eRet = MTR_NOT_OK;
            }
        }
    }
    else
    {
        eRet = MTR_NOT_OK;
    }

    return eRet;
}

static MT_RESULT_T _MTVDEC_ThumbnailFlush(unsigned long arg)
{
    UCHAR ucEsId;
    VDEC_ES_INFO_T *prVdecEsInfo;

    ucEsId = (UCHAR)arg;
    MTVDEC_PRINT(" - ucEsId = %d\n",(int)ucEsId);
    if(ucEsId >= VDEC_MAX_ES)
    {
        return MTR_NOT_SUPPORTED;
    }

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    ASSERT(prVdecEsInfo != NULL);
    if(GFX_Check_DirectFBMem((UINT32)prVdecEsInfo->rCapDstInfo.pucCanvasBuffer))
    {
        MTVDEC_PRINT("start second time flush\n");
        HalIOMMUInvalidateDCacheMultipleLine((UINT32)prVdecEsInfo->rCapDstInfo.pucCanvasBuffer,(UINT32)prVdecEsInfo->rCapDstInfo.pucCanvasBuffer,
            (prVdecEsInfo->rCapDstInfo.u4ThumbnailHeight*prVdecEsInfo->rCapDstInfo.u4CanvasPitch));
    }

    return MTR_OK;
}

static MT_RESULT_T _MTVDEC_CLICommand(unsigned long arg)
{
	const CHAR strCli[CLI_CMD_LEN];
    MTAL_IOCTL_2ARG_T    rArg;
	UINT32 u4StrLen=0;
	UCHAR *puStr;
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	COPY_FROM_USER_2ARG(arg, rArg);

	puStr=(UCHAR *)rArg.ai4Arg[0];
	u4StrLen=rArg.ai4Arg[1];

	x_memset((void *)strCli,0,CLI_CMD_LEN);
    //Printf("cli len:%d\n",u4StrLen);
    if (!access_ok(VERIFY_READ, (void __user *)puStr,u4StrLen))
    {                                                     
        printk("access_ok argument error Line %d\n",__LINE__);    
        return MTR_ERR_INV;                                       
    }

    if(copy_from_user((void *)strCli,(void __user *)puStr,u4StrLen))
    {                                                   
        printk("_MTVDEC_CLICommand copy from user argument error Line %d\n",__LINE__);      
        return MTR_ERR_INV;                                         
    }

    Printf("CLI:%s\n",strCli);
	CLI_Parser(strCli);
	return MTR_OK;
	
}

static MT_RESULT_T _MTHEVC_ChgYUV2YCbCr(unsigned long arg)
{
    MT_RESULT_T         eRet = MTR_OK;
    MTVDEC_FB_INFO_T    *pFBinfo; 
    MTVDEC_FB_INFO_T    rFBinfo;
    VDEC_YUV_RESIZE_INFO_T rResizeInfo;

    pFBinfo = (MTVDEC_FB_INFO_T*)arg;
    //Resize
    COPY_FROM_USER(pFBinfo,rFBinfo,sizeof(MTVDEC_FB_INFO_T));

    x_memcpy(&rResizeInfo,(VDEC_YUV_RESIZE_INFO_T *)(&rFBinfo),sizeof(VDEC_YUV_RESIZE_INFO_T));

    VDEC_YUV2YCbCr_Resize(&rResizeInfo);
   
    return eRet;
}

#ifdef MTVDEC_VIDEO_ES_INPUT_SUPPORT
//-----------------------------------------------------------------------------
/** _MwMPEGVideoCallback
 *  Callback handler of video PES
 *
 *  @param  prPes           PES message
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static BOOL _MTVDEC_MPEGVideoCallback(const DMX_PES_MSG_T* prPes)
{
    VDEC_PES_INFO_T rMpvPes;

    ASSERT(prPes != NULL);

    x_memset((void*)&rMpvPes, 0, sizeof(VDEC_PES_INFO_T));

    switch (prPes->u4FrameType)
    {
    case PIC_TYPE_I:
        rMpvPes.ucPicType = MPV_I_TYPE;
        break;

    case PIC_TYPE_P:
        rMpvPes.ucPicType = MPV_P_TYPE;
        break;

    case PIC_TYPE_B:
        rMpvPes.ucPicType = MPV_B_TYPE;
        break;

    default:
        return TRUE;
    }

    rMpvPes.ucMpvId = prPes->u1DeviceId;
    rMpvPes.ucEsId = prPes->u1Channel;
    rMpvPes.u4FifoStart = prPes->u4BufStart;
    rMpvPes.u4FifoEnd = prPes->u4BufEnd;
    rMpvPes.u4VldReadPtr = prPes->u4FrameAddr;
    rMpvPes.u4PTS = prPes->u4Pts;
    rMpvPes.u4DTS = prPes->u4Dts;
    rMpvPes.fgDtsValid = prPes->fgPtsDts;
    rMpvPes.fgSeqHdr = prPes->fgSeqHeader;
    rMpvPes.fgGop = prPes->fgGop;
    rMpvPes.fgEos = prPes->fgEOS;

    if (prPes->fgEOS)
    {
        MTVDEC_PRINT("MPEG-2 VideoCallback EOS.\n");
    }

    return (VDEC_SendEs(&rMpvPes) == 1);
}


//-----------------------------------------------------------------------------
/** _MwH264VideoCallback
 *  Callback handler of video PES
 */
//-----------------------------------------------------------------------------
static BOOL _MTVDEC_H264VideoCallback(const DMX_PES_MSG_T* prPes)
{
    VDEC_PES_INFO_T  rMpvPes;
    UINT32 u4Addr;

    ASSERT(prPes != NULL);

    x_memset((void*)&rMpvPes, 0, sizeof(VDEC_PES_INFO_T));

    if (!H264_PIC_VALID_TYPE(prPes->u4FrameType))
    {
        return TRUE;
    }
    
    rMpvPes.ucPicType = prPes->au1PicInfo[0];
    rMpvPes.ucMpvId = prPes->u1DeviceId;
    rMpvPes.ucEsId = prPes->u1Channel;
    rMpvPes.u4FifoStart = prPes->u4BufStart;
    rMpvPes.u4FifoEnd = prPes->u4BufEnd;

    u4Addr = prPes->u4FrameAddr + 4;
    if (u4Addr >= prPes->u4BufEnd)
    {
        u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
    }
    rMpvPes.u4VldReadPtr = u4Addr;

    rMpvPes.u4PTS = prPes->u4Pts;
    rMpvPes.u4DTS = prPes->u4Dts;
    rMpvPes.fgDtsValid = prPes->fgPtsDts;
    //rMpvPes.fgSeqHdr = prPes->fgSeqHeader;
    //rMpvPes.fgGop = prPes->fgGop;

    rMpvPes.fgEos = prPes->fgEOS;
    rMpvPes.u1SliceCount = prPes->u1SliceCount;

    if (prPes->fgEOS)
    {
        MTVDEC_PRINT("h264 VideoCallback EOS.\n");
    }

    return (VDEC_SendEs((void *)&rMpvPes) == 1);
}


//-----------------------------------------------------------------------------
/** _AVSVideoCallback
 *  Callback handler of video PES
 */
//-----------------------------------------------------------------------------
static BOOL _MTVDEC_AvsVideoCallback(const DMX_PES_MSG_T* prPes)
{
    VDEC_PES_INFO_T  rMpvPes;
    UINT32 u4Addr;

    ASSERT(prPes != NULL);

    x_memset((void*)&rMpvPes, 0, sizeof(rMpvPes));

    u4Addr = (UINT32)prPes->au1PicInfo;
    rMpvPes.ucPicType = *(UINT8*)VIRTUAL(u4Addr);

    rMpvPes.ucMpvId = prPes->u1DeviceId;
    rMpvPes.ucEsId = prPes->u1Channel;
    rMpvPes.u4FifoStart = prPes->u4BufStart;
    rMpvPes.u4FifoEnd = prPes->u4BufEnd;

    rMpvPes.u4VldReadPtr = prPes->u4FrameAddr;

    rMpvPes.u4PTS = prPes->u4Pts;
    rMpvPes.u4DTS = prPes->u4Dts;
    rMpvPes.fgDtsValid = prPes->fgPtsDts;
    //rMpvPes.fgSeqHdr = prPes->fgSeqHeader;
    //rMpvPes.fgGop = prPes->fgGop;

    rMpvPes.fgEos = prPes->fgEOS;

    if (prPes->fgEOS)
    {
        MTVDEC_PRINT("AVS VideoCallback EOS.\n");
    }

    return (VDEC_SendEs((void *)&rMpvPes) == 1);
}

//-----------------------------------------------------------------------------
/** _MwDmxVideoCallback
 *  Callback handler of video PES
 *
 *  @param  prPes           PES message
 *
 *  @retval TRUE            Succeed
 *  @retval FALSE           Fail
 */
//-----------------------------------------------------------------------------
static BOOL _MTVDEC_VideoCallback(const DMX_PES_MSG_T* prPes)
{

    switch (prPes->eVideoType)
    {
    case DMX_VIDEO_MPEG:
        return _MTVDEC_MPEGVideoCallback(prPes);

    case DMX_VIDEO_H264:
        return _MTVDEC_H264VideoCallback(prPes);

    case DMX_VIDEO_AVS:
        return _MTVDEC_AvsVideoCallback(prPes);

    default:
        printk("ERROR: This part is not for video codec(%d) currently, You can try add it if need.\n",
                (int)prPes->eVideoType);
        return FALSE;
    }

    return TRUE;
}

static MT_RESULT_T _MTVDEC_AllocInput(unsigned long arg)
{
    UCHAR   ucEsId;
    UINT32  u4Flag;
    UINT32  u4VfifoAddr, u4VfifoSize;
    UINT8   *pu1DmxId, u1DmxId;
    UINT8   *pu1VideoPidx, u1VideoPidx;
    DMX_MM_T rDmxMM;
    MT_RESULT_T eRetv=MTR_OK;
    FBM_POOL_T  *prFbmPool = NULL;
    DMX_DECODER_CALLBACKS_T rDmxCallback;
    MTAL_IOCTL_3ARG_T rArg;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    LockVdecSemaphoreMtal();
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    pu1DmxId = (UINT8 *)rArg.ai4Arg[1];
    pu1VideoPidx = (UINT8 *)rArg.ai4Arg[2];
    
    if ((VDEC_MAX_ES<=ucEsId) || !pu1DmxId || !pu1VideoPidx)
    {
        eRetv = MTR_PARAMETER_ERROR;
        goto EXIT;
    }

    // Do nothing
    DMX_MUL_EnableMultipleInst(TRUE);

    // TODO:  we need to take more concern here.
    u1DmxId = DMX_MUL_GetAvailableInst((DMX_CONN_TYPE_T)((UINT32)DMX_CONN_TYPE_BUFAGENT_0 + ucEsId));
    
    if (0xFF == u1DmxId)
    {
        eRetv = MTR_NOT_OK;
        goto EXIT;
    }

    if (!DMX_MUL_SetInstType(u1DmxId , DMX_IN_PLAYBACK_ES))
    {
        MTVDEC_PRINT("DMX_MUL_SetInstTypefail\n");
        eRetv = MTR_NOT_OK;
        goto EXIT;
    }
   
    // Do nothing
    DMX_SetToDecoder(TRUE);

    // for video callback
    x_memset(&rDmxCallback, 0, sizeof(rDmxCallback));
    rDmxCallback.pfnSendVideoPes = _MTVDEC_VideoCallback;
    rDmxCallback.pfnUpdateVideoWp = VDEC_SetWptr;
    DMX_MUL_SetDecoderCallbacks(u1DmxId, &rDmxCallback);

    // get AvailablePidx of dmx
    u1VideoPidx = DMX_MUL_GetAvailablePidx(u1DmxId);
    
    if (u1VideoPidx  >= DMX_NUM_PID_INDEX)
    {
        MTVDEC_PRINT("ERROR: u1VideoPidx>=DMX_NUM_PID_INDEX\n");
        
        eRetv = MTR_NOT_OK;
        goto EXIT;
    }

    x_memset(&rDmxMM, 0, sizeof(DMX_MM_T));
    
    u4Flag = (UINT32)(DMX_MM_FLAG_TYPE
            | DMX_MM_FLAG_INSTANCE_TAG
            | DMX_MM_FLAG_VALID
            | DMX_MM_FLAG_BUF
            | DMX_MM_FLAG_SEARCH_START_CODE);

    if (!_prDmxFbmPool)
    {
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
        _prDmxFbmPool = FBM_Alloc(FBM_TYPE_TZ_DMX);
#else
        _prDmxFbmPool = FBM_Alloc(FBM_TYPE_DMX);
#endif
    }
    
    if (!_prDmxFbmPool)
    {
        MTVDEC_PRINT("Fail to get VFIFO\n");
        eRetv = MTR_NOT_OK;
        goto EXIT;
    }
    
    u4VfifoAddr = _prDmxFbmPool->u4Addr;
    u4VfifoSize = _prDmxFbmPool->u4Size;
    
    if ((0==u4VfifoAddr) || (0==u4VfifoSize))
    {
        MTVDEC_PRINT("(prFbmPool->u4Addr == 0)\n");
        eRetv = MTR_NOT_OK;
        goto EXIT;
    }
    
    rDmxMM.fgSearchStartCode = TRUE;
    rDmxMM.fgEnable = TRUE;
    rDmxMM.ePidType = DMX_PID_TYPE_ES_VIDEO;
    rDmxMM.pvInstanceTag = NULL;
    rDmxMM.fgAllocBuf = FALSE;
     
    // same with VDEC id
    rDmxMM.u4BufAddr = u4VfifoAddr;
    rDmxMM.u4BufSize = (u4VfifoSize >> 1);
    rDmxMM.u1ChannelId = ucEsId;
    DMX_MM_Set(u1VideoPidx, u4Flag, &rDmxMM);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1DmxId, UINT8);
    COPY_TO_USER_ARG(pu1DmxId, u1DmxId, UINT8);
    USR_SPACE_ACCESS_VALIDATE_ARG(pu1VideoPidx, UINT8);
    COPY_TO_USER_ARG(pu1VideoPidx, u1VideoPidx, UINT8);

#ifdef CC_USE_DDI
    prFbmPool = FBM_GetPoolInfo(FBM_POOL_TYPE_FEEDER);
#else
    prFbmPool = FBM_GetPoolInfo(FBM_POOL_TYPE_FEEDER);
#endif
    if (!prFbmPool || !prFbmPool->u4Addr || !prFbmPool->u4Size)
    {
        MTVDEC_PRINT("ERROR: Get PVR2/feeder Pool fail.\n");
        eRetv = MTR_NOT_OK;
        goto EXIT;
    }

    _rVdecInputInfo.u4RingHandleSize = 512*1024;
    if (prFbmPool->u4Size < _rVdecInputInfo.u4RingHandleSize*2)
    {
        MTVDEC_PRINT("ERROR: PVR2 Pool not avaliable.\n");
        _rVdecInputInfo.u4RingHandleSize = 0;
        eRetv = MTR_NOT_OK;
        goto EXIT;
    }
    
    _rVdecInputInfo.u4InputAddr = VIRTUAL(prFbmPool->u4Addr);
    _rVdecInputInfo.u4InputSize = prFbmPool->u4Size - _rVdecInputInfo.u4RingHandleSize;
    _rVdecInputInfo.u4InputWp = 0;
    _rVdecInputInfo.ucEsId = ucEsId;
    _rVdecInputInfo.u1DmxId = u1DmxId;
    _rVdecInputInfo.u1VideoPidx = u1VideoPidx;

EXIT:
    UnLockVdecSemaphoreMtal();
    return eRetv;
}

static MT_RESULT_T _MTVDEC_ReleaseInput(unsigned long arg)
{
    DMX_MM_T rDmxMM;
    MTAL_IOCTL_3ARG_T rArg;
    UCHAR ucEsId;
    UINT8 u1DmxId, u1VideoPidx;
    
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    LockVdecSemaphoreMtal();
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    u1DmxId = (UINT8)rArg.ai4Arg[1];
    u1VideoPidx = (UINT8)rArg.ai4Arg[2];
    
    x_memset(&rDmxMM, 0, sizeof(DMX_MM_T));
    rDmxMM.fgEnable = FALSE;

    UNUSED(DMX_MM_Set(u1VideoPidx, DMX_MM_FLAG_VALID, &rDmxMM));
    UNUSED(DMX_MM_Free(u1VideoPidx));
    UNUSED(DMX_MUL_FreePidx(u1VideoPidx));
    UNUSED(DMX_MUL_FreeInst(u1DmxId));

    FBM_Free(_prDmxFbmPool);
    _prDmxFbmPool = NULL;
    
    UnLockVdecSemaphoreMtal();
    return MTR_OK;
}
#ifndef HAL_VIDEO_ES_INPUT_SUPPORT

static MT_RESULT_T _MTVDEC_GetInputBuf(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    MT_RESULT_T  eRetv=MTR_OK;
    UCHAR ucEsId;
    UINT32 **pu4Addr, *pu4Size, *pu4TotalSize, u4Addr, u4Size, u4TotalSize;
    UINT32 u4DmxAvailSize;
    UINT16 u2QueueSize, u2MaxQueueSize;
    
    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    LockVdecSemaphoreMtal();
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    pu4Addr = (UINT32 **)rArg.ai4Arg[1];
    pu4Size = (UINT32 *)rArg.ai4Arg[2];
    pu4TotalSize = (UINT32 *)rArg.ai4Arg[3];
    
    if (!pu4Addr || !pu4Size || !pu4TotalSize)
    {
        MTVDEC_PRINT("Pointer is NULL.\n");
        eRetv = MTR_PARAMETER_ERROR;
        goto EXIT;
    }
    
    u4Addr = _rVdecInputInfo.u4InputWp;
    
    u4DmxAvailSize = DMX_MUL_GetEmptySize(
        _rVdecInputInfo.u1DmxId, DMX_PID_TYPE_ES_VIDEO, _rVdecInputInfo.u1VideoPidx);
        
    VDEC_GetQueueInfo(ucEsId, &u2QueueSize, &u2MaxQueueSize);

    if (u2QueueSize*3 > u2MaxQueueSize*2)
    {
        u4DmxAvailSize = 0;
    }

    u4Size = (u4DmxAvailSize > _rVdecInputInfo.u4RingHandleSize) ? _rVdecInputInfo.u4RingHandleSize : u4DmxAvailSize;
    u4TotalSize = _rVdecInputInfo.u4InputSize;
    
    // TODO: not take goto case in condition currently. 
    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Addr, UINT32);
    COPY_TO_USER_ARG(pu4Addr, u4Addr, UINT32);
    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Size, UINT32);
    COPY_TO_USER_ARG(pu4Size, u4Size, UINT32);
    USR_SPACE_ACCESS_VALIDATE_ARG(pu4TotalSize, UINT32);
    COPY_TO_USER_ARG(pu4TotalSize, u4TotalSize, UINT32);
    
EXIT:
    UnLockVdecSemaphoreMtal();
    return eRetv;
}

static MT_RESULT_T _MTVDEC_PutInputBuf(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    MT_RESULT_T eRetv=MTR_OK;
    BOOL fgEof;
    UCHAR ucEsId;
    UINT32 u4Addr, u4Size;
    DMX_MM_DATA_T rDmxMmData;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    LockVdecSemaphoreMtal();
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    u4Addr = (UINT32)rArg.ai4Arg[1];
    u4Size = (UINT32)rArg.ai4Arg[2];
    fgEof =  (BOOL)rArg.ai4Arg[3];

    if (u4Addr != _rVdecInputInfo.u4InputWp)
    {
        printk("Warning: InputAddr(0x%X) != InputWp(0x%X).\n", u4Addr, _rVdecInputInfo.u4InputWp);
    }

    HalFlushInvalidateDCacheMultipleLine(_rVdecInputInfo.u4InputAddr+u4Addr, u4Size);

    if (u4Addr +u4Size > _rVdecInputInfo.u4InputSize)
    {
        x_memcpy((void *)_rVdecInputInfo.u4InputAddr, 
                 (void *)(_rVdecInputInfo.u4InputAddr+_rVdecInputInfo.u4InputSize), 
                 (u4Addr+u4Size-_rVdecInputInfo.u4InputSize));
        HalFlushInvalidateDCacheMultipleLine(_rVdecInputInfo.u4InputAddr, u4Addr+u4Size-_rVdecInputInfo.u4InputSize);
    }

    x_memset(&rDmxMmData, 0, sizeof(rDmxMmData));
    rDmxMmData.u4BufStart = _rVdecInputInfo.u4InputAddr;
    rDmxMmData.u4BufEnd = _rVdecInputInfo.u4InputAddr + _rVdecInputInfo.u4InputSize;
    rDmxMmData.u4StartAddr = _rVdecInputInfo.u4InputAddr + u4Addr;
    rDmxMmData.u4FrameSize = u4Size;
    rDmxMmData.fgEOS = fgEof;

    _rVdecInputInfo.u4InputWp = (u4Addr+u4Size)%_rVdecInputInfo.u4InputSize;
    
    if (!DMX_MUL_MoveData(_rVdecInputInfo.u1DmxId, &rDmxMmData))
    {
        printk("Warning: DMX_MUL_MoveData fail.\n");
        eRetv = MTR_NOT_OK;
        goto EXIT;
    }

EXIT:
    UnLockVdecSemaphoreMtal();    
    return eRetv;
}

#else

static MT_RESULT_T _MTVDEC_GetInputBuf_Direct(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    MT_RESULT_T  eRetv=MTR_OK;
    UCHAR ucEsId;
    UINT32 **pu4Addr, *pu4Size, *pu4TotalSize, u4Addr, u4Size, u4TotalSize;
    UINT32 u4DmxAvailSize;
    UINT16 u2QueueSize, u2MaxQueueSize;
    
    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    LockVdecSemaphoreMtal();
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    pu4Addr = (UINT32 **)rArg.ai4Arg[1];
    pu4Size = (UINT32 *)rArg.ai4Arg[2];
    pu4TotalSize = (UINT32 *)rArg.ai4Arg[3];
    
    if (!pu4Addr || !pu4Size || !pu4TotalSize)
    {
        MTVDEC_PRINT("Pointer is NULL.\n");
        eRetv = MTR_PARAMETER_ERROR;
        goto EXIT;
    }
    
    u4Addr = 0;//use feeder start address.
    
    u4DmxAvailSize = DMX_MUL_GetEmptySize(_rVdecInputInfo.u1DmxId, DMX_PID_TYPE_ES_VIDEO, _rVdecInputInfo.u1VideoPidx);
        
    VDEC_GetQueueInfo(ucEsId, &u2QueueSize, &u2MaxQueueSize);

    if (u2QueueSize*3 > u2MaxQueueSize*2)
    {
        u4DmxAvailSize = 0;
    }

    u4Size = (u4DmxAvailSize > (_rVdecInputInfo.u4RingHandleSize + _rVdecInputInfo.u4InputSize)) ? 
		(_rVdecInputInfo.u4RingHandleSize + _rVdecInputInfo.u4InputSize) : u4DmxAvailSize;
    u4TotalSize = (_rVdecInputInfo.u4RingHandleSize + _rVdecInputInfo.u4InputSize);
    // TODO: not take goto case in condition currently. 
    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Addr, UINT32);
    COPY_TO_USER_ARG(pu4Addr, u4Addr, UINT32);
    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Size, UINT32);
    COPY_TO_USER_ARG(pu4Size, u4Size, UINT32);
    USR_SPACE_ACCESS_VALIDATE_ARG(pu4TotalSize, UINT32);
    COPY_TO_USER_ARG(pu4TotalSize, u4TotalSize, UINT32);
    
EXIT:
    UnLockVdecSemaphoreMtal();
    return eRetv;
}

static MT_RESULT_T _MTVDEC_PutInputBuf_Direct(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    MT_RESULT_T eRetv=MTR_OK;
    BOOL fgEof;
    UCHAR ucEsId;
    UINT32 u4Addr, u4Size;
    DMX_MM_DATA_T rDmxMmData;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    LockVdecSemaphoreMtal();
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    u4Addr = (UINT32)rArg.ai4Arg[1];
    u4Size = (UINT32)rArg.ai4Arg[2];
    fgEof =  (BOOL)rArg.ai4Arg[3];

	UNUSED(u4Addr);
    HalFlushInvalidateDCacheMultipleLine(_rVdecInputInfo.u4InputAddr, u4Size);

    x_memset(&rDmxMmData, 0, sizeof(rDmxMmData));
    rDmxMmData.u4BufStart = _rVdecInputInfo.u4InputAddr;
    rDmxMmData.u4BufEnd = _rVdecInputInfo.u4InputAddr + _rVdecInputInfo.u4InputSize +_rVdecInputInfo.u4RingHandleSize ;
    rDmxMmData.u4StartAddr = _rVdecInputInfo.u4InputAddr;
    rDmxMmData.u4FrameSize = u4Size;
    rDmxMmData.fgEOS = fgEof;

    //_rVdecInputInfo.u4InputWp = (u4Addr+u4Size)%_rVdecInputInfo.u4InputSize; needn't wp.
    if (!DMX_MUL_MoveData(_rVdecInputInfo.u1DmxId, &rDmxMmData))
    {
        printk("Warning: DMX_MUL_MoveData fail.\n");
        eRetv = MTR_NOT_OK;
    }
    UnLockVdecSemaphoreMtal();    
    return eRetv;
}
#endif

#endif // MTVDEC_VIDEO_ES_INPUT_SUPPORT

static MT_RESULT_T _MTVDEC_GetDecoderNum(unsigned long arg)
{
    MT_RESULT_T         eRet = MTR_OK;
    UINT32 u4DecNum=VDEC_MAX_ES;
    COPY_TO_USER_ARG(arg, u4DecNum,UINT32);       
    return eRet;
}

#ifdef DUMP_YUV_FROM_KERNAL
static INT32 _DumpData(CHAR *pcFileName, const INT8 *buf, size_t count )
{
    mm_segment_t old_fs;
    struct file *filep;
    size_t ret;

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    filep = filp_open(pcFileName, (O_CREAT | O_RDWR | O_APPEND), 0);
    if ( IS_ERR(filep) || NULL == filep)
    {
        set_fs( old_fs );
        printk("_DumpData, unable to open file: %s\n", pcFileName);

        return -1;
    }

    ret = filep->f_op->write(filep, buf, count, &filep->f_pos);
    printk("czg dump, count(%d)\n", (UINT32)count);
    
    filp_close( filep, 0 );
    set_fs( old_fs );
    
    return 0;
}
#endif

static MT_RESULT_T _MTVDEC_GetCurrentFrameData(unsigned long arg)
{
    MT_RESULT_T       eRet = MTR_OK;
    MTVDEC_FRAME_INTO_T *prFrameInfo;
    MTVDEC_FRAME_INTO_T  rFrameInfo;
    MTAL_IOCTL_4ARG_T rArg;
    UCHAR ucEsId;
    UCHAR ucFbgId;
    UCHAR ucFbId;
    UINT32 u4YAddr;      //frame Y Adress
    UINT32 u4CAddr;      //frame c Adress
    UINT32 u4WSizeAlign; //pu4FrameW 64 align
    UINT32 u4HSizeAlign; //pu4FrameH 32 align
    UINT32 u4Size;       //the frame size after align
    UINT32 u4FrameH;     //frame High
    UINT32 u4FrameW;     //frame width
    VDEC_HDR_INFO_T rHdrInfo;    
    FBM_POOL_T  *prFbmPool = NULL;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    ucFbgId = (UCHAR)rArg.ai4Arg[1];
    ucFbId = (UCHAR)rArg.ai4Arg[2];
    prFrameInfo = (MTVDEC_FRAME_INTO_T *)rArg.ai4Arg[3];

    LockVdecSemaphoreMtal();
    
    COPY_FROM_USER(prFrameInfo, rFrameInfo, sizeof(MTVDEC_FRAME_INTO_T));
    
    if (ucFbId != FBM_PreLookFrameBufferFromDispQ(ucFbgId))
    {
        eRet = MTR_NOT_OK;
        printk("Error! FB Id(%d) is not the next FB in DispQ.\n", ucFbId);
        goto EXIT_PORT;
    }

    if (!FBM_CheckFrameBufferStatus(ucFbgId, ucFbId, FBM_FB_STATUS_DISPLAYQ))
    {
        eRet = MTR_NOT_OK;
        printk("Error! FBM Status is not DISPLAYQ.\n");
        goto EXIT_PORT;
    }
    FBM_GetFrameBufferFromDispQ(ucFbgId);
    FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &u4YAddr, &u4CAddr);
    
    if(VDEC_QueryInfo(ucEsId, &rHdrInfo))
    {
        u4FrameW = rHdrInfo.u2OrgWidth;
        u4FrameH = rHdrInfo.u2OrgHeight;
    }
    else
    {
        eRet = MTR_NOT_OK;
        printk("Error! VDEC_QueryInfo fail.\n");
        goto EXIT_PORT;
    }

    u4WSizeAlign = (u4FrameW + FBM_B2R_H_PITCH) & (~FBM_B2R_H_PITCH);
    u4HSizeAlign = (u4FrameH + FBM_MPV_V_ALIGMENT) & (~FBM_MPV_V_ALIGMENT);

    u4Size  = (u4WSizeAlign * u4HSizeAlign)*3/2;

    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG);

    rFrameInfo.u4AddrYOft = u4YAddr - prFbmPool->u4Addr;
    rFrameInfo.u4AddrCOft = u4CAddr - prFbmPool->u4Addr;
    rFrameInfo.u4HSize = u4FrameW;
    rFrameInfo.u4VSize = u4FrameH;
    rFrameInfo.u4HSizeStride = u4WSizeAlign;
    rFrameInfo.u4VSizeStride = u4HSizeAlign;
    rFrameInfo.u4Size  = u4Size;

    COPY_TO_USER(prFrameInfo, rFrameInfo, sizeof(MTVDEC_FRAME_INTO_T));

#ifdef DUMP_YUV_FROM_KERNAL
    if((inum>=0) && (inum<10))
    {
         static CHAR ucFileName[255] = {0};
         x_sprintf(ucFileName, "%s/yuv.raw", "/usbdisk");
        _DumpData(ucFileName, (INT8*)VIRTUAL(u4YAddr), (size_t)u4WSizeAlign*u4HSizeAlign);
        _DumpData(ucFileName, (INT8*)VIRTUAL(u4CAddr), (size_t)u4WSizeAlign*u4HSizeAlign/2);

        printk("czg io W(%d), H(%d), i(%d)\n", (UINT32)u4FrameW, (UINT32)u4FrameH, (UINT32)inum);
    }
    inum++;
#endif

EXIT_PORT:
    UnLockVdecSemaphoreMtal();

    return eRet;
}

static MT_RESULT_T _MTVDEC_GettedData(unsigned long arg)
{
    MT_RESULT_T       eRet = MTR_OK;
    MTAL_IOCTL_4ARG_T rArg;
    UCHAR ucEsId;
    UCHAR ucFbgId;
    UCHAR ucFbId;
    BOOL fgGettedDate;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    ucFbgId = (UCHAR)rArg.ai4Arg[1];
    ucFbId = (UCHAR)rArg.ai4Arg[2];
    fgGettedDate = (BOOL)rArg.ai4Arg[3];

    LockVdecSemaphoreMtal();
    if(fgGettedDate)
    {
        FBM_SetFrameBufferStatus(ucFbgId, ucFbId, FBM_FB_STATUS_LOCK);
        FBM_SetFrameBufferStatus(ucFbgId, ucFbId, FBM_FB_STATUS_EMPTY);
    }
    else
    {
        eRet = MTR_NOT_OK;
        printk("_MTVDEC_GettedData fgGettedDate is false!\n");
    }

    UnLockVdecSemaphoreMtal();
    
    return eRet;
}

MT_RESULT_T _MTVDEC_GetColourPrimary(unsigned long arg)
{
    MT_RESULT_T       eRet = MTR_OK;
    MTAL_IOCTL_2ARG_T rArg;
    UCHAR ucEsId;
    UCHAR *pucColour;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    pucColour = (UCHAR *)rArg.ai4Arg[1];

    LockVdecSemaphoreMtal();

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    *pucColour = prVdecEsInfoKeep->ucColour;

    UnLockVdecSemaphoreMtal();
    
    return eRet;
}


static MT_RESULT_T _MTVDEC_SetRenderPts(unsigned long arg)
{
    MT_RESULT_T eRetv = MTR_OK;
    MTAL_IOCTL_2ARG_T rArg;

    UCHAR ucEsId;
    UINT32 u4Pts;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    LockVdecSemaphoreMtal();
    
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    u4Pts = (UINT32)rArg.ai4Arg[1];
    
    VDEC_SetRenderPts(ucEsId, u4Pts);
    MTVDEC_PRINT("- _MTVDEC_SetRenderPts ucEsId(%d), u4pts(0x%x).\n", ucEsId, u4Pts);

    UnLockVdecSemaphoreMtal(); 
    
    return eRetv;
}

static MT_RESULT_T _MTVDEC_SetRenderFromPts(unsigned long arg)
{
    MT_RESULT_T eRetv = MTR_OK;
    MTAL_IOCTL_2ARG_T rArg;

    UCHAR ucEsId;
    UINT32 u4Pts;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    LockVdecSemaphoreMtal();
    
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    u4Pts = (UINT32)rArg.ai4Arg[1];
    
    VDEC_SetRenderFromPts(ucEsId, u4Pts);
    MTVDEC_PRINT("- _MTVDEC_SetRenderFromPts ucEsId(%d), u4pts(0x%x).\n", ucEsId, u4Pts);

    UnLockVdecSemaphoreMtal(); 
    
    return eRetv;
}

static MT_RESULT_T _MTVDEC_SetRenderFromFirstPic(unsigned long arg)
{
    MT_RESULT_T eRetv = MTR_OK;
    MTAL_IOCTL_3ARG_T rArg;
    UCHAR ucEsId;
    BOOL fgRenderFromFirstPic;
    UINT64 u8LastRenderPos;
    
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    LockVdecSemaphoreMtal();
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    fgRenderFromFirstPic = (BOOL)rArg.ai4Arg[1];
    u8LastRenderPos = (UINT64)rArg.ai4Arg[2];

    VDEC_SetRenderFromFirstPic(ucEsId, fgRenderFromFirstPic, u8LastRenderPos);

    UnLockVdecSemaphoreMtal();
    
    return eRetv;

}

static MT_RESULT_T _MTVDEC_SetRenderFromPos(unsigned long arg)
{
    MT_RESULT_T eRetv = MTR_OK;
    MTAL_IOCTL_5ARG_T rArg;
    UCHAR ucEsId;
    BOOL fgRenderFromPos;
    UINT64 u8Pos;
    INT32 i4TempRef;
    UINT16 u2DecodingOrder;
    
    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

    LockVdecSemaphoreMtal();
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    fgRenderFromPos = (BOOL)rArg.ai4Arg[1];
    u8Pos = (UINT64)rArg.ai4Arg[2];
    i4TempRef = (INT32)rArg.ai4Arg[3];
    u2DecodingOrder = (UINT16)rArg.ai4Arg[4];

    VDEC_SetRenderFromPos(ucEsId, fgRenderFromPos, u8Pos, i4TempRef, u2DecodingOrder);

    UnLockVdecSemaphoreMtal();
    
    return eRetv;

}

static MT_RESULT_T _MTVDEC_SetRenderFromBackStep(unsigned long arg)
{
    MT_RESULT_T eRetv = MTR_OK;
    MTAL_IOCTL_5ARG_T rArg;
    UCHAR ucEsId;
    BOOL fgRenderBackStep;
    UINT64 u8Pos;
    INT32 i4TempRef;
    UINT16 u2DecodingOrder;
    
    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

    LockVdecSemaphoreMtal();
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    fgRenderBackStep = (BOOL)rArg.ai4Arg[1];
    u8Pos = (UINT64)rArg.ai4Arg[2];
    i4TempRef = (INT32)rArg.ai4Arg[3];
    u2DecodingOrder = (UINT16)rArg.ai4Arg[4];

    VDEC_SetRenderFromPos(ucEsId, fgRenderBackStep, u8Pos, i4TempRef, u2DecodingOrder);

    UnLockVdecSemaphoreMtal();
    
    return eRetv;

}

static MT_RESULT_T _MTVDEC_SetRenderFromPtsAutoPause(unsigned long arg)
{
    MT_RESULT_T eRetv = MTR_OK;
    MTAL_IOCTL_3ARG_T rArg;
    UCHAR ucEsId;
    UINT32 u4Pts;
    BOOL fgAutoPause;
    
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    LockVdecSemaphoreMtal();
    ucEsId = (UCHAR)rArg.ai4Arg[0];
    u4Pts = (UINT32)rArg.ai4Arg[1];
    fgAutoPause = (BOOL)rArg.ai4Arg[2];

    VDEC_SetRenderFromPtsAutoPause(ucEsId, u4Pts, fgAutoPause);

    UnLockVdecSemaphoreMtal();
    
    return eRetv;
}

static MT_RESULT_T _MTVDEC_PlayOneFrame(unsigned long arg)
{
    MT_RESULT_T eRetv = MTR_OK;
    MTAL_IOCTL_3ARG_T rArg;

    UCHAR ucVdecId;
    UCHAR ucB2rId;
    MTVDEC_FMT_T eVdecFmt;
    ENUM_VDEC_FMT_T eFmt = VDEC_FMT_MAX;
    
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    LockVdecSemaphoreMtal();
    
    ucVdecId = (UCHAR)rArg.ai4Arg[0];
    ucB2rId = (UINT32)rArg.ai4Arg[1];    
    eVdecFmt = (MTVDEC_FMT_T)rArg.ai4Arg[2];

    VDP_SetPauseMm(ucB2rId, FALSE);

    switch (eVdecFmt)
    {
    case MTVDEC_FMT_MPEG1_MPEG2:
        eFmt = VDEC_FMT_MPV;
        break;
    case MTVDEC_FMT_MPEG4:
        eFmt = VDEC_FMT_MP4;
        break;
    case MTVDEC_FMT_H264:
        eFmt = VDEC_FMT_H264;
        break;
    case MTVDEC_FMT_VC1:
        eFmt = VDEC_FMT_WMV;
        break;
    case MTVDEC_FMT_RV:
        eFmt = VDEC_FMT_RV;
        break; 
    case MTVDEC_FMT_MJPEG:
        eFmt = VDEC_FMT_MJPEG;
        break; 
    case MTVDEC_FMT_AVS:
        eFmt = VDEC_FMT_AVS;
        break;
    case MTVDEC_FMT_VP8:
        eFmt = VDEC_FMT_VP8;
        break;
    case MTVDEC_FMT_VP6:
        eFmt = VDEC_FMT_VP6;
        break;
    case MTVDEC_FMT_RAW:
        eFmt = VDEC_FMT_RAW;
        break;
    case MTVDEC_FMT_OGG:
        eFmt = VDEC_FMT_OGG;
        break;
    case MTVDEC_FMT_H265:
        eFmt = VDEC_FMT_H265;
    break;
        default:
        break;
    }
    
    if(!VDEC_PlayOneFrm(ucVdecId, eFmt))
    {
        MTVDEC_PRINT("VDEC_PlayOneFrm fail, ucVdecId(%d), eFmt(%d)\n", (int)ucVdecId, (int)eFmt);
        eRetv = MTR_NOT_OK;
    }
    
    UnLockVdecSemaphoreMtal(); 
    
    return eRetv;
}

static MT_RESULT_T _MTVDEC_GetHalParam(unsigned long arg)
{
	MTAL_IOCTL_6ARG_T	 rArg;
	UCHAR ucEsId;
//	  BOOL fgRet;
	UINT32 u4Type;
	UINT32 u4Param1;
	UINT32 u4Param2;
	UINT32 u4Param3;
	
	LockVdecSemaphoreMtal();

	USR_SPACE_ACCESS_VALIDATE_6ARG(arg);
	COPY_FROM_USER_6ARG(arg, rArg);
	ucEsId = (UCHAR)rArg.ai4Arg[0];
	u4Type = (UINT32)rArg.ai4Arg[1];
	u4Param1 = (UINT32)rArg.ai4Arg[2];
	u4Param2 = (UINT32)rArg.ai4Arg[3];
	u4Param3 = (UINT32)rArg.ai4Arg[4];

	//if(u4Type == MTVDEC_MM_SEQ_INFO)
	//else if(u4Type == MTVDEC_MM_TPT_SEQ_INFO)
	//else if(u4Type == MTVDEC_MM_3D_INFO)
	//else if (u4Type == MTVDEC_MM_SEAMLESS_MODE)
	if (u4Type == MTVDEC_MM_HAL_PARAMETER)
	{
		VDEC_GetHALParam(ucEsId, VDEC_MM_HAL_PARAMETER,u4Param1,&u4Param2,0);
		MTVDEC_PRINT(" - _MTVDEC_SetMMParam, MTVDEC_MM_HAL_PARAMETER to be:%d\n", u4Param1,u4Param2);
		UnLockVdecSemaphoreMtal();
		return MTR_OK;
	}
	rArg.ai4Arg[3] = u4Param2;
    COPY_TO_USER_6ARG(arg, rArg);
	UnLockVdecSemaphoreMtal();

	return MTR_OK;
}



int mtal_ioctl_mtvdec(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg)
{
    int i4Ret = MTR_OK;

    switch (cmd)
    {
    case MTAL_IO_VDEC_INIT:
        i4Ret = _MTVDEC_Init();
        break;
    case MTAL_IO_VDEC_TERMINT:
        i4Ret = _MTVDEC_Termint();
        break;
    case MTAL_IO_VDEC_RESET:
        i4Ret = _MTVDEC_Reset();
        break;
    case MTAL_IO_VDEC_PLAY:
        i4Ret = _MTVDEC_Play(arg);
        break;
    case MTAL_IO_VDEC_PLAY_I_FRAME:
        i4Ret = _MTVDEC_PlayIFrame(arg);
        break;
    case MTAL_IO_VDEC_DECODE_I_FRAME: // 20110801
        i4Ret = _MTVDEC_DecodeIFrame(arg);
        break;
    case MTAL_IO_VDEC_QUERY_FBM_POOL:
        i4Ret = _MTVDEC_QureyPoolInfo(arg);
        break;
    case MTAL_IO_VDEC_PAUSE:
        i4Ret = _MTVDEC_Pause(arg);
        break;
    case MTAL_IO_VDEC_STOP:
    case MTAL_IO_VDEC_STOP_I_FRAME:
        i4Ret = _MTVDEC_Stop(arg);
        break;
    case MTAL_IO_VDEC_GETRESOLUTION:
        i4Ret = _MTVDEC_GetInfo(arg);
        break;
    case MTAL_IO_VDEC_GETDBKINFO:
        i4Ret = _MTVDEC_GetDbkInfo(arg);
        break;

    case MTAL_IO_VDEC_SETAVSYNMODE:
        i4Ret = _MTVDEC_SetAvSynMode(arg);
        break;
    case MTAL_IO_VDEC_GETSTC:
        i4Ret = _MTVDEC_GetStc(arg);
        break;
    case MTAL_IO_VDEC_GETDECSTATUS:
        i4Ret = _MTVDEC_GetDecStatus(arg);
        break;
    case MTAL_IO_VDEC_REGDECERRCB:
        i4Ret = _MTVDEC_RegDecErrCb(arg);
        break;
    case MTAL_IO_VDEC_REGFRAMECB:
        i4Ret = _MTVDEC_FrameCb();
        break;
    case MTAL_IO_VDEC_REGSTATUSCB:
        i4Ret = _MTVDEC_StatusIntCb();
        break;
		
#if 0

    case MTAL_IO_VDEC_GETDECFIFO:
         i4Ret = _MTVDEC_GetDecFifo(arg);
        break;
#endif
    case MTAL_IO_VDEC_REGUSERDATACB:
        i4Ret = _MTVDEC_RegUserDataCb(arg);
        break;
    case MTAL_IO_VDEC_STARTUSERDATA:
        i4Ret = _MTVDEC_StartUserData(arg);
        break;
    case MTAL_IO_VDEC_STOPUSERDATA:
        i4Ret = _MTVDEC_StopUserData(arg);
        break;
    case MTAL_IO_VDEC_GETUSERDATA:
        i4Ret = _MTVDEC_GetUserData(arg);
        break;
    case MTAL_IO_VDEC_REGGENERALCB:
        i4Ret = _MTVDEC_RegGeneralCb(arg);
        break;
    case MTAL_IO_VDEC_SETFRAMETOUSER:
        i4Ret = _MTVDEC_SetFrameToUser(arg);
        break;
    case MTAL_IO_VDEC_GETTHUMBNAIL:
        i4Ret = _MTVDEC_GetThumbnail(arg);
        break;
    case MTAL_IO_VDEC_PLAY2:
        i4Ret = _MTVDEC_Play2(arg);
        break;
    case MTAL_IO_VDEC_SETMMPARAM:
        i4Ret = _MTVDEC_SetMMParam(arg);
        break;
    case MTAL_IO_VDEC_CLRMMPARAM:
        i4Ret = _MTVDEC_ClrMMParam(arg);
        break;
    case MTAL_IO_VDEC_CHKCODECCAP:
        i4Ret = _MTVDEC_ChkCodecCap(arg);
        break;
    case MTAL_IO_VDEC_PLAYCC:
        i4Ret = _MTVDEC_PlayCc(arg);
        break;
    case MTAL_IO_VDEC_STOPCC:
        i4Ret = _MTVDEC_StopCc(arg);
        break;
    case MTAL_IO_VDEC_ALLOCCCBUF:
        i4Ret = _MTVDEC_AllocCcBuf(arg);
        break;
    case MTAL_IO_VDEC_FREECCBUF:
        i4Ret = _MTVDEC_FreeCcBuf(arg);
        break;
    case MTAL_IO_VDEC_FLUSHCCBUF:
        i4Ret = _MTVDEC_FlushCcBuf(arg);
        break;
    case MTAL_IO_VDEC_GETCC:
        i4Ret = _MTVDEC_GetCc(arg);
        break;
    case MTAL_IO_VDEC_GETDBKINFO2:
        i4Ret = _MTVDEC_GetDbkInfo2(arg);
        break;
    case MTAL_IO_VDEC_SETDBKINFO2:
        i4Ret = _MTVDEC_SetDbkInfo2(arg);
        break;
    case MTAL_IO_VDEC_ALLOCIFRAMEBUF:
        i4Ret = _MTVDEC_AllocIFrameBuf(arg);
        break;
    case MTAL_IO_VDEC_FREEIFRAMEBUF:
        i4Ret = _MTVDEC_FreeIFrameBuf(arg);
        break;
    case MTAL_IO_VDEC_SETIFRAMEBUF:
        i4Ret = _MTVDEC_SetIFrameBuf(arg);
        break;
    case MTAL_IO_VDEC_SETIFRAMEDEC:
        i4Ret = _MTVDEC_SetIFrameDec(arg);
        break;
    case MTAL_IO_VDEC_REGAUTOTESTCB:
        i4Ret = _MTVDEC_RegAutotestCb(arg);
        break;
    case MTAL_IO_VDEC_SETCRCGOLDEN:
        i4Ret = _MTVDEC_SetCrcGolden(arg);
        break;
    case MTAL_IO_VDEC_SETAUTOTESTFILEDIR:
        i4Ret = _MTVDEC_SetAutoTestCurFileDir(arg);
        break;
    case MTAL_IO_VDEC_SETAUTOTESTFILENAME:
        i4Ret = _MTVDEC_SetAutoTestCurFileName(arg);
        break;
    case MTAL_IO_VDEC_CHKDIVXBONDING:
        i4Ret = _MTVDEC_ChkDivXBonding(arg);
        break;
    case MTAL_IO_VDEC_SETABINFO:
        i4Ret = _MTVDEC_SetABInfo(arg);
        break;
    case MTAL_IO_VDEC_SETSRCID:
        i4Ret = _MTVDEC_SetSrcId(arg);
        break;
    case MTAL_IO_VDEC_PLAYMM:
        i4Ret = _MTVDEC_PlayMM(arg);
        break;
       #ifdef CC_ITIMING_ALWAYS
    case MTAL_IO_VDEC_SETFHDDISPLAY:
        i4Ret = _MTVDEC_SetFixFHDDisplay(arg);
        break;
       #endif
    case MTAL_IO_VDEC_SETSPEED:
        i4Ret = _MTVDEC_SetSpeed(arg);
        break;
    case MTAL_IO_VDEC_SETDECODEMODE:
        i4Ret = _MTVDEC_SetDecodeMode(arg);
        break;
    case MTAL_IO_VDEC_SETFRAMEADVANCE:
        i4Ret = _MTVDEC_SetFrameAdvance(arg);
        break;
    case MTAL_IO_VDEC_SETFLUSHESDATA:
        i4Ret = _MTVDEC_SetFlushEsData(arg);
        break;
    case MTAL_IO_VDEC_SETIPTVMODE:
        i4Ret = _MTVDEC_SetIPTVMode(arg);
        break;
    case MTAL_IO_VDEC_PLAYNOTDISPLAY:
        i4Ret = _MTVDEC_PlayNotDisplay(arg);
        break;
    case MTAL_IO_VDEC_REPLAYVDEC:
        i4Ret = _MTVDEC_RePlayVdec(arg);
        break;
#ifdef CC_BOOT_VDEC_LOGO
    case MTAL_IO_VDEC_GET_LOGO:
        i4Ret = _MTVDEC_GetBootLogo(arg);
        break;
    case MTAL_IO_VDEC_GET_LOGO_ADDR:
        i4Ret = _MTVDEC_GetBootLogo_Addr(arg);
        break;
    case MTAL_IO_VDEC_SET_LOGO:
        i4Ret = _MTVDEC_SetBootLogo(arg);
        break;
    case MTAL_IO_VDEC_SET_LOGO_ADDR:
        i4Ret = _MTVDEC_SetBootLogo_Addr(arg);
    break;
#endif
    case MTAL_IO_VDEC_SET_SUPERFREEZE:
        i4Ret = _MTVDEC_SetSuperFreeze(arg);
        break;
    case MTAL_IO_VDEC_GET_DISP_FRM_ADDR:
        i4Ret = _MTVDEC_GetDisplayFrameBufferAddr(arg);
        break;
    case MTAL_IO_VDEC_THUMBNAILFLUSH:
        i4Ret = _MTVDEC_ThumbnailFlush(arg);
        break;
    case MTAL_IO_VDEC_TRY_CTRL_VALID_SRC:
        i4Ret = _MTVDEC_CtrlValidSrc(arg);
        break;
    case MTAL_IO_VDEC_SET_SKEP_MODE:
        i4Ret = _MTVDEC_SetSkipMode(arg);
        break;
    case MTAL_IO_VDEC_CLICMD:
        i4Ret = _MTVDEC_CLICommand(arg);
        break;
    case MTAL_IO_VDEC_ChgYUV2YCbCr:
        i4Ret = _MTHEVC_ChgYUV2YCbCr(arg);
        break;
    case MTAL_IO_VDEC_GET_DECNUM:
        i4Ret = _MTVDEC_GetDecoderNum(arg);
        break;
    case MTAL_IO_VDEC_RM_SET:
        i4Ret = _MTVDEC_RmSet(arg);
        break;
#ifdef MTVDEC_VIDEO_ES_INPUT_SUPPORT        
    case MTAL_IO_VDEC_ALLOC_INPUT:
        i4Ret = _MTVDEC_AllocInput(arg);
        break;
    case MTAL_IO_VDEC_RELEASE_INPUT:
        i4Ret = _MTVDEC_ReleaseInput(arg);
        break;
	#ifndef HAL_VIDEO_ES_INPUT_SUPPORT
    case MTAL_IO_VDEC_GET_INPUT_BUF:
        i4Ret = _MTVDEC_GetInputBuf(arg);
        break;
    case MTAL_IO_VDEC_PUT_INPUT_BUF:
        i4Ret = _MTVDEC_PutInputBuf(arg);
        break;
	#else 
    case MTAL_IO_VDEC_GET_INPUT_BUF:
        i4Ret = _MTVDEC_GetInputBuf_Direct(arg);
        break;
    case MTAL_IO_VDEC_PUT_INPUT_BUF:
        i4Ret = _MTVDEC_PutInputBuf_Direct(arg);
        break;
	#endif 
	
#else
    case MTAL_IO_VDEC_ALLOC_INPUT:
    case MTAL_IO_VDEC_RELEASE_INPUT:
    case MTAL_IO_VDEC_GET_INPUT_BUF:
    case MTAL_IO_VDEC_PUT_INPUT_BUF:
        printk("Error: version not match. define MTVDEC_VIDEO_ES_INPUT_SUPPORT and rebuild me.\n");
        break;
#endif // MTVDEC_VIDEO_ES_INPUT_SUPPORT
    case MTAL_IO_VDEC_GETWFDERRINFO:
        i4Ret = _MTVDEC_GetWFDErrInfo(arg);
        break;   
    case MTAL_IO_VDEC_RESET_WFD_INFO:
        i4Ret = _MTVDEC_ResetWFDInfo(arg);
        break;
    case MTAL_IO_VDEC_REGCALLBACK:
        i4Ret = _MTVDEC_RegCallBack(arg);
        break;
    case MTAL_IO_VDEC_GET_DELAYTIME:
        i4Ret = _MTVDEC_GetDelayTime(arg);
        break;
    case MTAL_IO_VDEC_GET_DISP_FRM:
        i4Ret = _MTVDEC_GetCurrentFrameData(arg);
        break;  
    case MTAL_IO_VDEC_GETTED_DATA:
        i4Ret = _MTVDEC_GettedData(arg);
        break;
    case MTAL_IO_VDEC_GET_COLOUR:
        i4Ret = _MTVDEC_GetColourPrimary(arg);
        break;
    case MTAL_IO_VDEC_SET_RENDER_PTS:
        i4Ret = _MTVDEC_SetRenderPts(arg);
        break;
    case MTAL_IO_VDEC_SET_RENDER_FROM_PTS:
        i4Ret = _MTVDEC_SetRenderFromPts(arg);
        break;
    case MTAL_IO_VDEC_SET_RENDER_FROM_FIRST_PIC:
        i4Ret = _MTVDEC_SetRenderFromFirstPic(arg);
        break;
    case MTAL_IO_VDEC_SET_RENDER_FROM_POS:
        i4Ret = _MTVDEC_SetRenderFromPos(arg);
        break;
    case MTAL_IO_VDEC_SET_RENDER_FROM_BACK_STEP:
        i4Ret = _MTVDEC_SetRenderFromBackStep(arg);
        break;
    case MTAL_IO_VDEC_SET_RENDER_FROM_PTS_AUTO_PAUSE:
        i4Ret = _MTVDEC_SetRenderFromPtsAutoPause(arg);
        break;
    case MTAL_IO_VDEC_PLAY_ONE_FRAME:
        i4Ret = _MTVDEC_PlayOneFrame(arg);
        break;
	case MTAL_IO_VDEC_GET_HAL_PARAMETER:
		i4Ret = _MTVDEC_GetHalParam(arg);
		break;
	case MTAL_IO_VDEC_GET_DIRECT_VDEC:
		i4Ret = _MTVDEC_GetDirectVdecInfo(arg);
		break;
    default:
        i4Ret = MTR_NOT_OK;
    break;
    }

    return i4Ret;
}

