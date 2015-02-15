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

#ifndef __IMGRESZ_H
#define __IMGRESZ_H

#include "x_common.h"
#include "drv_imgresz.h"
#include "drv_config.h"
#include <mach/cache_operation.h>


// Work around for hardware destination width can not reach 4096 limit
#define IMGRESZ_HW_DEST_WIDTH_4096_LIMIT 0
#define IMGRESZ_HW_SOURCE_WIDTH_2_LIMIT 1
#define IMGRESZ_HW_SOURCE_HEIGHT_2_LIMIT 1
#define IMGRESZ_HW_TARGET_HEIGHT_2_LIMIT 0

#if !CONFIG_DRV_VERIFY_SUPPORT
#define IMGRESZ_INST_NUM        10
#else
#define IMGRESZ_INST_NUM        0
#endif
#define IMGRESZ_HW_INST_NUM     2
#define IMGRESZ_MAX_LOCK_HW_INST_NUM 1

#define IMGRESZ_MEMORY_DYNAMIC_ALLOC 1

#define IMGRESZ_SUPPORT_MAP_UNMAP    1


/*! @name Image Resizer Event Group Define */
/*! @{ */
#define IMGRESZ_EV_INITIAL          ((EV_GRP_EVENT_T) 0)
#define IMGRESZ_EV_FINISH           ((EV_GRP_EVENT_T)(1) << 0)
#define IMGRESZ_EV_DO_SCALE         ((EV_GRP_EVENT_T)(1) << 1)
#define IMGRESZ_EV_STOP_SCALE       ((EV_GRP_EVENT_T)(1) << 2)
#define IMGRESZ_EV_FINISH_SCALE     ((EV_GRP_EVENT_T)(1) << 3)
#define IMGRESZ_EV_LOCK             ((EV_GRP_EVENT_T)(1) << 4)
#define IMGRESZ_EV_UNLOCK           ((EV_GRP_EVENT_T)(1) << 5)

/*! @} */

#define IMGRESZ_INVALID64   0xFFFFFFFFFFFFFFFF
#define IMGRESZ_INVALID32   0xFFFFFFFF
#define IMGRESZ_INVALID16   0xFFFF
#define IMGRESZ_INVALID8    0xFF


#if IMGRESZ_HW_DEST_WIDTH_4096_LIMIT
typedef struct _IMGRESZ_YCBCR_TO_AYUV_WORK_AROUND
{
    BOOL                    fgYCbCrToAYUVWorkAround;
    BOOL                    fgCbTo444;
    BOOL                    fgCrTo444;
    UINT32                  u4Cb444Sa;
    UINT32                  u4Cr444Sa;    
    BOOL                    fgCbTo444Done;
    BOOL                    fgCrTo444Done;
    UINT32                  u4LineCnt;
    //IMGRESZ_INST_T          rImgreszInst;    
} IMGRESZ_YCBCR_TO_AYUV_WORK_AROUND;


typedef struct _IMGRESZ_AYUV_TO_ARGB_WORK_AROUND
{
    BOOL                    fgAYUVToARGBWorkAround;
    UINT32                  u4Step;
    UINT32                  u4TotalStep;
} IMGRESZ_AYUV_TO_ARGB_WORK_AROUND;
#endif


#if IMGRESZ_HW_SOURCE_HEIGHT_2_LIMIT
typedef struct _IMGRESZ_SORUCE_HEIGHT_2_WORK_AROUND
{
    UINT32                  u4SA1;
    UINT32                  u4SA2;
	UINT32                  u4SA3;
} IMGRESZ_SORUCE_HEIGHT_2_WORK_AROUND;
#endif

#if IMGRESZ_HW_TARGET_HEIGHT_2_LIMIT
typedef struct _IMGRESZ_TARGET_HEIGHT_2_WORK_AROUND
{
    UINT32                  u4SA1;
    UINT32                  u4SA2;
    UINT32                  u4SA3;
} IMGRESZ_TARGET_HEIGHT_2_WORK_AROUND;
#endif


typedef struct _IMGRESZ_HW_INST_T
{
    UINT16                  u2ImgReszHwId;          ///< Hw id
    UINT16                  u2ImgReszCompId;        ///< HW is working in which component
    HANDLE_T                hEventHandle;           ///< Event Handle
    BOOL                    fgThreadFinish;         ///< Thread finish    
    BOOL                    fgWaitThreadFinish;     ///< Wait thread finish    
    BOOL                    fgImgReszActive;        ///< HW is in active state
    BOOL                    fgInterlaced;           ///< Interlaced scaling mode.
    BOOL                    fgCurrTopField;         ///< Current scale top field.
    BOOL                    fgLock;                 ///< Locked
    BOOL                    fgWaitLock;             ///< Wait to be locked.
    UINT32                  u4WaitLockInstId;       ///< Wait to be locked instance id.
#if IMGRESZ_HW_DEST_WIDTH_4096_LIMIT
    IMGRESZ_YCBCR_TO_AYUV_WORK_AROUND rYCbCrToAYUVWorkAround; ///< YCbCr to AYUV work around
    IMGRESZ_AYUV_TO_ARGB_WORK_AROUND rAYUVToARGBWorkAround;   ///< AYUV to ARGB work around
#endif
#if IMGRESZ_HW_SOURCE_HEIGHT_2_LIMIT
    IMGRESZ_SORUCE_HEIGHT_2_WORK_AROUND rSourceHeight2WorkAround;  /// Source height 2 limit work around
#endif

#if IMGRESZ_HW_TARGET_HEIGHT_2_LIMIT
    IMGRESZ_TARGET_HEIGHT_2_WORK_AROUND rTargetHeight2WorkAround; /// Target height 2 limit work around
#endif
} IMGRESZ_HW_INST_T;

extern void vImgReszHwInstScaleNotify(BOOL fgForceHwId,UINT32 *pu4HwId);
extern INT32 i4ImgReszHwInstLockNotify(UINT32 u4HwInstId,BOOL fgLock);
extern INT32 i4ImgReszHwInstStopNotify(UINT32 u4HwInstId);
extern INT32 i4ImgReszHwInstGetNum(void);
extern BOOL fgImgReszHwInstIsActive(UINT32 u4HwInstId);
extern void vImgreszCopyMemory(void *pvDestination,void *pvSource,SIZE_T Length);
extern void vImgreszSetMemory(void *pvDestination,UINT8 u1Value,SIZE_T Length);


#endif // __IMGRESZ_H
