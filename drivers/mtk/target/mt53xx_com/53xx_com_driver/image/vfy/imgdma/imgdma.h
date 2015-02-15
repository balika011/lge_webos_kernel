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

#ifndef __IMGDMA_H
#define __IMGDMA_H


#include "x_common.h"
#include "drv_img_dma.h"
#define UNIFORM_DRV_CALLBACK 0
/// driver callback function prototype
typedef VOID (*x_drv_cb_nfy_fct)(VOID* pv_cb_info,    ///< callback information structure, define by each driver module
                                    VOID* pv_nfy_tag     ///< notify tag, this should be the same with pv_nty_tag in DRV_CB_REG_INFO_T
                                   );

typedef struct _DRV_CB_REG_INFO_T
{
  x_drv_cb_nfy_fct pf_cb_nfy_fct;    ///< callback function pointer
  VOID*            pv_nfy_tag;       ///< notify tag, this will be the parameter of callback function when driver callback
}DRV_CB_REG_INFO_T;

#define IMGDMA_INVALID64    0xFFFFFFFFFFFFFFFF
#define IMGDMA_INVALID32    0xFFFFFFFF
#define IMGDMA_INVALID16    0xFFFF
#define IMGDMA_INVALID8     0xFF


//void *memcpy(void * _Dst, const void * _Src, SIZE_T _Size);
//void *memset(void * _Dst, int _Val, SIZE_T _Size);

#define ImgdmaCopyMemory(destination, source, length) x_memcpy((destination), (source), (length))
#define ImgdmaZeroMemory(destination, length) x_memset((destination), 0, (length))

#define IMGDMA_INST_NUM        4

/*! @name Image Resizer Event Group Define */
/*! @{ */
#define IMGDMA_EV_INITIAL       ((EV_GRP_EVENT_T) 0)
#define IMGDMA_EV_TRSN_DATA     ((EV_GRP_EVENT_T)(1) << 0)
#define IMGDMA_EV_DEL_THREAD    ((EV_GRP_EVENT_T)(1) << 1)
/*! @} */

/* instance states */
typedef enum
{
    IMGDMA_INST_NOAVTICE,
    IMGDMA_INST_AVTICE,
} IMGDMA_INST_STATE_T;

typedef struct _IMGDMA_INST_T
{
    UINT16                  u2CompId;                ///< the IMGDMA Component Id
    IMGDMA_INST_STATE_T     eInstState;
    HANDLE_T                hEventHandle;
    HANDLE_T                hInstHandle;
    HANDLE_T                hSemaHandle;
    volatile   BOOL                     fgFillBuffer;
#if UNIFORM_DRV_CALLBACK
    DRV_CB_REG_INFO_T       rImgdecCbInfo;
#else
    IMG_NFY_FCT_T           pfImgdecCb;
    void*                   pvImgdecTag;
#endif    
    UINT32                  u4TrsnId;
    UINT32                  u4ImgId;

    UINT32                  u4FifoStart;
    UINT32                  u4FifoEnd;
    UINT32                  u4RdPoint;
    UINT32                  u4WrPoint;
    HANDLE_T                hMem;    

    UINT32                  u4RangeOfst;
    UINT32                  u4RangeSize;
    UINT32                  u4TrsnedSz;
    BOOL                    fgTrsning;

    UINT32                  u4SrcSA;
    BOOL                    fgCopy;
    DRV_TYPE_T              eType;
    UINT32                  u2DrvCompId;
    // Add by ping zhao
    UINT32                  u4ReqstSz;
    IMG_UPD_FCT_T           pfImgUpdCb;
	BOOL                    fgFileEnd;
} IMGDMA_INST_T;

#endif // __IMGDMA_H
