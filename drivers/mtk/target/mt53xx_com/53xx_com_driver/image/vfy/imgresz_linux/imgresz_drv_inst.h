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

#ifndef __IMGRESZ_INST_H
#define __IMGRESZ_INST_H

#include "drv_imgresz.h"
#include "imgresz_drv.h"
#include "chip_ver.h"

typedef struct _IMGRESZ_INST_T
{
    UINT16                  u2ImgReszCompId;        ///< the ImgResz Component Id
    IMGRESZ_DRV_SCALE_MODE      eImgReszScaleMd;        ///< scale mode
    IMGRESZ_DRV_STATE           eImgReszState;          ///< scale state
#if IMGRESZ_MEMORY_DYNAMIC_ALLOC    
    IMGRESZ_DRV_SRC_BUF_INFO_T  *ptImgReszSrcBufInfo;     ///< source buffer information
    IMGRESZ_DRV_DST_BUF_INFO_T  *ptImgReszDstBufInfo;     ///< destination buffer information
    IMGRESZ_DRV_BLD_BUF_INFO_T  *ptImgReszBldBufInfo;     ///< blending buffer information
    IMGRESZ_DRV_PARTIAL_INFO_T  *ptImgReszPartialBufInfo; ///< Partial buffer information
    IMGRESZ_DRV_JPEG_INFO_T     *ptImgReszJpegInfo;       ///< Jpeg information
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    IMGRESZ_DRV_RM_INFO_T       *ptImgReszRmInfo;
#endif
#else
    IMGRESZ_DRV_SRC_BUF_INFO_T  tImgReszSrcBufInfo;     ///< source buffer information
    IMGRESZ_DRV_DST_BUF_INFO_T  tImgReszDstBufInfo;     ///< destination buffer information
    IMGRESZ_DRV_BLD_BUF_INFO_T  tImgReszBldBufInfo;     ///< blending buffer information
    IMGRESZ_DRV_PARTIAL_INFO_T  tImgReszPartialBufInfo; ///< Partial buffer information
    IMGRESZ_DRV_JPEG_INFO_T     tImgReszJpegInfo;       ///< Jpeg information
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    IMGRESZ_DRV_RM_INFO_T       tImgReszRmInfo;
#endif
#endif
    UINT32                  u4PrevRowBufSa1;        ///< Previous partial row buffer 1 start address
    UINT32                  u4PrevRowBufSa2;        ///< Previous partial row buffer 2 start address
    UINT32                  u4PrevRowBufSa3;        ///< Previous partial row buffer 3 start address
    //UINT32                  u4ImgReszComponents;    ///< current scale components
    //UINT32                  u4ImgReszHwHandle;      ///< ImgResz HW Handle
    UINT32                  u4ImgReszPriority;      ///< priority
    UINT32                  u4ImgReszCurrPriority;  ///< current priority (priority will increase if not serviced)
    BOOL                    fgDoLock;               ///< Ready to do lock
    BOOL                    fgLock;                 ///< Lock hardware instance.
    UINT32                  u4HwInstId;             ///< Locked hardware instance id.
    IMGRESZ_DRV_NOTIFY_CB_REG_T rNotifyCallbackReg; ///< notify callback function
    UINT32                  u4TempLineBufSa;        ///< Temporary line buffer start address.
    UINT32                  u4PartialRowCnt;        ///< Partial mode row count.
    BYTE                    abColorPallet[256][4];  ///< Color pallet
    BOOL                    fgLumaKeyEnable;        ///< Luma key enalbe or not
    UINT8                   u1LumaKey;              ///< Luma key value
    BOOL                    fg1To1Scale;            ///< 1:1 Scale for venc path  
    BOOL                    fgYSrcOnly;             ///< Only has Y component in src/dst
    BOOL                    fgUserMMUTable;
} IMGRESZ_INST_T;


extern void vImgResz_Inst_Init(void);
extern void vImgResz_Inst_Uninit(void);
extern IMGRESZ_DRV_STATE eImgResz_Inst_GetState(UINT32 u4InstId);
extern void vImgResz_Inst_SetState(UINT32 u4InstId, IMGRESZ_DRV_STATE eState);
extern INT32 i4ImgResz_Inst_SetLock(UINT32 u4InstId,BOOL fgLock);
extern void vImgResz_Inst_IncPriority(void);
extern INT32 i4ImgResz_Inst_GetInstanceObject(UINT32 u4InstId,IMGRESZ_INST_T **pprImgreszInst);
extern INT32 i4ImgResz_Inst_GetInstance(UINT32 *pu4InstId);
extern INT32 i4ImgResz_Inst_ReleaseInstance(UINT32 u4InstId);
extern void vImgResz_Inst_DispatchHw(UINT32 u4Ticket);
extern INT32 i4ImgResz_Inst_GetUnservicedInstance(IMGRESZ_HW_INST_T* ptImgReszHwInst);
extern INT32 i4ImgResz_Inst_SetServicingInstanceToUnserviced(IMGRESZ_HW_INST_T* ptImgReszHwInst);
extern void vImgResz_Inst_ReleaseServicedInstanceAndNotifyCallback(IMGRESZ_HW_INST_T* ptImgReszHwInst,BOOL fgCheckWaitStop,
                                                            INT32 i4NotifyCallbackState);
extern void vImgResz_Inst_NotifyCallback(IMGRESZ_HW_INST_T* ptImgReszHwInst,INT32 i4NotifyCallbackState);
extern INT32 i4ImgResz_Inst_GetLockInstance(IMGRESZ_HW_INST_T* ptImgReszHwInst,BOOL *pfgLock,UINT32 *pu4InstId);
extern INT32 i4ImgResz_Inst_StopResize(UINT32 u4InstId);
extern INT32 i4ImgResz_Inst_NotifyCallbackProc(UINT32 u4InstId,INT32 i4State);

#endif // __IMGRESZ_H
