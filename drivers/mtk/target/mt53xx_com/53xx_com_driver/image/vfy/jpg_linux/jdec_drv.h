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

#ifndef __JDEC_H
#define __JDEC_H

#include "x_common.h"
#include "x_img_dec.h"
#include "drv_config.h"
#include "jdec_com_common.h"
#include "jdec_com_jfif.h"
#include "jdec_hal_if.h"
#include "drv_jdec.h"
#include "drv_imgresz.h"
#include "sw/djpeg.h"
#include "drv_img_dma.h"
#include "x_debug.h"
//#include "sys_config.h"
//#include <mach/cache_operation.h>
//#include <linux/module.h>
//#include "asm/uaccess.h"

#define JDEC_LOG_PRINT  0
#if JDEC_LOG_PRINT
#define vJdecLog UTIL_Printf
#else
#define vJdecLog(...)
#endif
#define JDEC_DEBUG_LOG_SAVE 1

#define JPG_SUPPORT_DIRECT_DECODE_MODE 1

#define PERFORMANCE_TEST 0

#define SUPPORT_VDEC_PATH  1

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
#define SUPPORT_JDEC_VIDEO_MODE  1
#else
#define SUPPORT_JDEC_VIDEO_MODE  0
#endif

#if CONFIG_SYS_MEM_PHASE3
#if CONFIG_DRV_ONLY
#define JDEC_SUPPORT_BROKEN_TARGET 0
#define JDEC_DISCONTINUOUS_SOURCE  0
#else
#if CONFIG_ENABLE_IOMMU
#define JDEC_DISCONTINUOUS_SOURCE  0
#define JDEC_SUPPORT_BROKEN_TARGET 0
#else
#define JDEC_DISCONTINUOUS_SOURCE  1
#define JDEC_SUPPORT_BROKEN_TARGET 1

#endif
#endif
#else
#define JDEC_DISCONTINUOUS_SOURCE 0
#define JDEC_SUPPORT_BROKEN_TARGET 0
#endif

#define JDEC_MCU_ROW_ENHANCE 0

#define JDEC_USE_MAP_UNMAP   1

#define JDEC_CACHE_LINE_ALIGN 1
#define JDEC_REST_BEFORE_POWER_DONE 1

#define JDEC_SUPPORT_SLICED_COEF_BUFFER 0
#if JDEC_SUPPORT_SLICED_COEF_BUFFER
#define JDEC_MAX_COEF_BLOCK        (256*1024)
#endif

#define JDEC_CACHE_ENABLE 1

#define JDEC_LOG_FRM_START      (1U << 0)   // 1
#define JDEC_LOG_FRM_DECODE     (1U << 1)   // 2
#define JDEC_LOG_FRM_END        (1U << 2)   // 4
#define JDEC_LOG_FRM_STOP       (1U << 3)   // 8
#define JDEC_LOG_SCALE_STOP     (1U << 4)   // 10
#define JDEC_LOG_YUV_2_RGB1     (1U << 5)   // 20
#define JDEC_LOG_YUV_2_RGB2     (1U << 6)   // 40
#define JDEC_LOG_DEC_STOP       (1U << 7)   // 80
#define JDEC_LOG_TRNS_STOP      (1U << 8)   // 100
#define JDEC_LOG_FRM_END_END    (1U << 9)   // 200
#define JDEC_LOG_FRM_STOP_END   (1U << 10)  // 400

#define JdecCopyMemory(destination, source, length) x_memcpy((destination), (source), (length))
#define JdecZeroMemory(destination, length)         x_memset((destination), 0, (length))

#define JDEC_INVALID64      0xFFFFFFFFFFFFFFFF
#define JDEC_INVALID32      0xFFFFFFFF
#define JDEC_INVALID16      0xFFFF
#define JDEC_INVALID8       0xFF

#if !CONFIG_DRV_VERIFY_SUPPORT
#define JDEC_INST_NUM               2                   ///< the instance number
#define JDEC_PIC_INFO_CACHE_NUM     2                  ///< the picture info cache number
#else
#define JDEC_INST_NUM               0                   ///< the instance number
#define JDEC_PIC_INFO_CACHE_NUM     0                  ///< the picture info cache number

#endif

#define JDEC_DRV_FIFO_SIZE          512*1024    ///< fifo size, 512K
#define JDEC_FIFO_THRESHOLD         4*1024      ///< fifo threshold, reach, then start decode
#define JDEC_CLR_BUF_SIZE           4096*40    ///< color buffer size
#define JDEC_CLR_BUF_NUM            2
#define JDEC_COMP_NUM               3
#define JDEC_DEC_TIME_OUT_BASE      10

#define JDEC_PROGRESSIVE_MULTI_COLLECT_LIMIT    1920*1080//1024*768

#define JDEC_RD_POINTER_MATCH_COUNT1            3 // for decoding finish
#define JDEC_RD_POINTER_MATCH_COUNT2            5 // for error bitstream
/*! @name Jdec Event Group Define */
/*! @{ */
#define JDEC_DRV_EV_INITIAL         ((EV_GRP_EVENT_T) 0)
#define JDEC_DRV_EV_DECODE          ((EV_GRP_EVENT_T)(1) << 0)
#define JDEC_DRV_EV_SCALE_DONE      ((EV_GRP_EVENT_T)(1) << 1)
#define JDEC_DRV_EV_STOP            ((EV_GRP_EVENT_T)(1) << 2)
#define JDEC_DRV_EV_SCALE_ABORT     ((EV_GRP_EVENT_T)(1) << 3)
#define JDEC_DRV_EV_DEC_ABORT       ((EV_GRP_EVENT_T)(1) << 4)
#define JDEC_DRV_EV_TIME_OUT        ((EV_GRP_EVENT_T)(1) << 5)
#define JDEC_DRV_EV_DEC_TIME_OUT    ((EV_GRP_EVENT_T)(1) << 6)
#define JDEC_DRV_EV_FILLED          ((EV_GRP_EVENT_T)(1) << 7)
#define JDEC_DRV_EV_SCALE_ERR       ((EV_GRP_EVENT_T)(1) << 8)
#define JDEC_DRV_EV_DEL_THREAD      ((EV_GRP_EVENT_T)(1) << 9)
#define JDEC_DRV_EV_SLEEP           ((EV_GRP_EVENT_T)(1) << 10)
#define JDEC_DRV_EV_SCALE_READY     ((EV_GRP_EVENT_T)(1) << 11)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)
#define JDEC_DRV_EV_DEC_DONE        ((EV_GRP_EVENT_T)(1) << 12)
#define JDEC_DRV_EV_DEC_ERR         ((EV_GRP_EVENT_T)(1) << 13)
#endif
/*! @} */

#define JDEC_DRV_RTN_STOP    1
#define JDEC_DRV_RTN_OK      0
#define JDEC_DRV_RTN_FAIL    (-1)

/// Jdec State Machine
typedef enum
{
    JDEC_STATE_IDLE,
    JDEC_STATE_PRE_START,
    JDEC_STATE_START,
    JDEC_STATE_PRS_HDR,
    JDEC_STATE_INIT_HAL,
    JDEC_STATE_DECODING,
    JDEC_STATE_DEC_FINISH,
    JDEC_STATE_MAX
} JDEC_STATE;


/// Jdec Usage State
typedef enum
{
    JDEC_USAGE_NONE,
    JDEC_USAGE_NORMAL,
    JDEC_USAGE_MJPG,
    JDEC_USAGE_MAX
} JDEC_USAGE;

/// Jdec decoding state
typedef enum
{
    JDEC_DECODE_IDLE,
    JDEC_DECODE_DO_DECODE,
    JDEC_DECODE_WAIT_DECODE,
    JDEC_DECODE_DO_SCALE,
    JDEC_DECODE_CHECK_NEXT_SCAN,
    JDEC_DECODE_RE_DECODE,
    JDEC_DECODE_MAX
} JDEC_DEC_STATE;

/// Jdec Cache State
typedef enum
{
    JDEC_CACHE_INVALID,
    JDEC_CACHE_INUSING,
    JDEC_CACHE_VALID,
    JDEC_CACHE_DEC_FAIL
} JDEC_CACHE_STATE;

typedef struct _JDEC_PIC_INFO_T
{
    UINT32              u4ImgId;                ///< picture id
    JDEC_JFIF_SOS_T     rSosInfo;
    JDEC_JFIF_DQT_T     rQTblInfo;
    JDEC_JFIF_DHT_T     rHuffTblInfo;
    JDEC_JFIF_SOF_T     rSofInfo;
    UINT32              u4RestartInterval;
    UINT32              u4Ofst1stSOS;           ///< offset of 1st SOS
    UINT32              u4MaxHFactor;           ///< Max H Factor
    UINT32              u4MaxVFactor;           ///< Max V Factor

    JDEC_CACHE_STATE    eCacheState;            ///< Cache State
    INT32               i4ReplaceCount;         ///< if 0, can be replaced
} JDEC_PIC_INFO_T;

typedef struct _JDEC_FIFO_INFO_T
{
    UINT32              u4WrPoint;              ///< current fifo write point
    UINT32              u4RdPoint;              ///< current fifo read point
    UINT32              u4FifoStart;            ///< fifo start address
    UINT32              u4FifoEnd;              ///< fifo size
    UINT32              u4PreRdPoint;           ///< previous read point
    UINT32              u4FileOfst;             ///< the offset of file that has been read
    UINT32              u4ValidData;            ///< the number of valid data in fifo
    BOOL                fgEoRange;              ///< end of file
#if CONFIG_SYS_MEM_PHASE3
    HANDLE_T            hFifo;
#endif
} JDEC_FIFO_INFO_T;

#if JDEC_SUPPORT_BROKEN_TARGET
typedef struct _JDEC_BLOCK_INFO_T
{
    UINT32             u4BlkIdx;
    UINT32             u4TotalBlk;
    UINT32             u4BlkSa;
    UINT32             u4BlkOfst;
    UINT32             u4BlkSize;
    UINT32             u4LastBlkSize;
    HANDLE_T           hMem;
} JDEC_BLOCK_INFO_T;
#endif



typedef struct _JDEC_INST_T
{
    UINT16              u2CompId;               ///< the Jdec Component Id
    HANDLE_T            hJdecInst;              ///< the instance id
    HANDLE_T            hJdecEvent;             ///< the message queue id
    HANDLE_T            hJdecSema;              ///< the semapfore id
    HANDLE_T            hJdecHwDevSema;              ///< the semapfore id
    HANDLE_T            hJdecTrnsTimer;         ///< the Timer id
    INT32               i4JdecTrnsTimerStatus;  ///< 0:idle, -1:timeout, 1:run timer
    HANDLE_T            hJdecDecTimer;          ///< the Timer id
    HANDLE_T            hJdecSleepTimer;        ///< the Timer id
    INT32               i4JdecDecTimerStatus;   ///< 0:idle, -1:timeout, 1:run timer
    UINT32              u4HwInstId;             ///< the Jpeg HW decode Id
    UINT32              u4HwId;                 ///< real hw id
    JDEC_FIFO_INFO_T    rFifoInfo;              ///< fifo infomation

    #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
    UINT32              u4VJDMHandle;
    #endif

    //img frm start
    UINT32              u4ImgId;                ///< picture id
    void                *pvImgBuf;              ///< if MW has put whole source data into buffer
    BOOL                fgJdecKernImgBuf;
    UINT32              u4ImgSize;              ///< image source size
    void*               pvPicCache;             ///< jpeg picture cache
    #if UNIFORM_DRV_CALLBACK
    DRV_CB_REG_INFO_T   rNfyInfo;              ///< call back function and info
    #else
    IMG_NFY_FCT_T       pfNfyCb;                ///< the callback function of MW
    #endif
    VOID                *pvTag;                 ///< the callback function private data
    UINT32              u4ImgdmaId;
    IMGRESZ_DRV_TICKET_T    rImgReszTicket;     ///< Img resz ticket
    //--

    //img decode
    UINT32              u4FrameIndex;           ///< must be 0 in Jpeg
    UINT32              u4SrcCropPosX;          ///< source picture crop X position
    UINT32              u4SrcCropPosY;          ///< source picture crop Y position
    UINT32              u4SrcCropWidth;         ///< source picture crop width
    UINT32              u4SrcCropHeight;        ///< source picture crop height

    void                *pvDstYBuf;             ///< destination buffer point
    void                *pvDstCBuf;             ///< destination buffer point
    UINT32              u4DstCropPosX;          ///< destination picture crop X position
    UINT32              u4DstCropPosY;          ///< destination picture crop y position
    UINT32              u4DstCropWidth;         ///< destination picture crop width
    UINT32              u4DstCropHeight;        ///< destination picture crop height
    UINT32              u4DstBufPitch;          ///< destination buffer pitch
    GFX_COLORMODE_T     eDstColorMode;          ///< destination picture color mode
    IMG_ROTATE_T        eDstRotate;             ///< destination picture rotation degree
    //--

    //get decode progress
    JDEC_STATE          eJdecState;             ///< state machine
    UINT32              u4DecProgress;          ///< decode progress
    INT32               i4JdecScaleState;       ///< scale state
    UINT32              u4ScanCount;            ///< scan count
    UINT32              u4TimeOutCount;         ///< TimeOutCount
    //--

    void*               hJfifInst;              ///< jfif inst

    // decode mode
    BOOL                fgInitHal;
    JDEC_DEC_STATE      eJdecDecState;          ///< decoding state
    JDEC_HAL_DEC_MODE_T eJdecDecMode;           ///< decode mode
    UINT32              u4CurrDecodeMCUW;       ///< current decode MCU in width
    UINT32              u4CurrDecodeMCUH;       ///< current decode MCU in height
    UINT32              u4MCUInMCURowW;         ///< MCU width in a MCR ROW
    UINT32              u4MCUInMCURowH;         ///< MCU height in a MCR ROW
    UINT32              u4CurrDecodeMCURow;     ///< current decode MCU ROW
    UINT32              u4MCURowNum;            ///< MCU Row Number

    INT32               i4JdecErrorType;        ///< error handle type

    UINT32              au4ColorBufSa[JDEC_CLR_BUF_NUM][JDEC_COMP_NUM]; ///< color buffer start address
    UINT32              au4ColorBufPitch[JDEC_COMP_NUM];                ///< color buffer pitch
    UINT32              au4ColorBufSz[JDEC_COMP_NUM];                   ///< color buffer size
    BOOL                fgDefaultScale[JDEC_COMP_NUM];

    //for progressive multi-collect mode
    BOOL                fgLastScan;             ///< if last scan, true
    //for progressive enhance mode
    UINT32              au4CompScanCount[JDEC_COMP_NUM];
    //--
    BOOL                fgJdecHwScale;

    //Add by pingzhao ,for progressvie jpg, use to count how many last scan has been decode
    UINT32              u4LastScanNum;

    /*Add by pingzhao, for progressive jpg,
      use to count how many first scan has been decode*/
    UINT32              u4FirstScanNum;
    /*Add by pingzhao, for progressive jpg,
      if the image is resize to small size ,
      we can only decode the first scan for each component*/
    BOOL                fgDecodeFirstScanOnly;

    IMG_QUALITY_FACTOR_T eQuality; // for progressive jpg decode quality adjust

    //SW Instance
    IMG_JPG_OBJ_T       *ptImgJpgObj;

    // For BDP00117567
    BOOL                fgParsingHeaderWaitData;
    UINT32              u4NeedData;

    IMG_JPG_DECODE_FLAG_E eDecodeFlag;

    BOOL                fgSwDecode;


    #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)
    BOOL                fgWaitStop;
    #endif

    #if 0//(CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    UINT32              u4LastScanSA;
    #endif

    //add by mtk40029, for jpg_mod call
    BOOL                fgNoJpgCbToUSMode;
	BOOL                fgNoLockWrSemphore;
    BOOL                fgNoJpgCbUSModeScale;
    HANDLE_T            hJpgDirectDecEv;
    IMGDMA_DRV_FIFO_STATUS_T     tUsrTempFifoInfo;
    HANDLE_T            hJdecSrcfifoSema;
    BOOL                fgStartDecodeNow;
#if JDEC_SUPPORT_BROKEN_TARGET
    JDEC_BLOCK_INFO_T   rBlkInfo;
    BOOL                fgBrokenTarget;
    BOOL                fgBrokenRow;
    UINT32              u4TempAYUVBuf;
    HANDLE_T            hTempAYUVBuf;
    UINT32              u4TempLineBuf;
    HANDLE_T            hTempLineBuf;
    UINT32              u4McuRowLength;  ///< AYUV format length
    BOOL                fgNeedJumpBack;
#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    UINT32              u4ScanRd;        /// Read pointer for recorder scan end
#endif
#if SUPPORT_VDEC_PATH
    BOOL                fgVdecPath;
    VDEC_JPEG_DRV_NOTIFY_CB_REG_T  rVdecCallback;
    UINT32              u4CheckSum;
#endif

#if JDEC_USE_MAP_UNMAP
    UINT32              u4MapSA;
#endif
} JDEC_INST_T;

#endif // __JDEC_H
