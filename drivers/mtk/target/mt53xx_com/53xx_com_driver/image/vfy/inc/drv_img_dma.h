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


#ifndef __DRV_IMG_DMA_H
#define __DRV_IMG_DMA_H

#include "x_common.h"
#include "x_img_dec.h"
#include "x_rm_dev_types.h"


/// IMG DMA Return Value, OK
#define IMGDMA_OK                          ((INT32) 0)
#define IMGDMA_DRV_FAIL                    ((INT32) -1)
#define IMGDMA_DRV_EOF                     ((INT32) -2)
#define IMGDMA_DRV_IO_ERR                  ((INT32) -3)
#ifndef UNIFORM_DRV_CALLBACK
#define UNIFORM_DRV_CALLBACK 0
#endif
/* Call back function for update sth for image driver*/
typedef VOID (*IMG_UPD_FCT_T)
(
    //UINT32          ui4_img_id,         /* the image id which causes this notification */
    VOID            *pv_tag,              /* tag passed to the callback function */
    VOID            *pv_data);            /* data passed with this notification */


/// i4ImgDmaInit()
typedef struct _IMGDMA_DRV_INIT_INFO_T
{
    UINT32                  u4ImgId;                ///< Image Id
    #if UNIFORM_DRV_CALLBACK
    DRV_CB_REG_INFO_T       rNfyInfo;               ///< replace the pfImgDecCb & pvImgDecTag
    #else
    IMG_NFY_FCT_T           pfImgDecCb;             ///< callback function of Img Dec Drv
    VOID                    *pvImgDecTag;           ///< tag passed to the callback function   
    #endif
    UINT32                  u4FifoStart;            ///< fifo start address
    UINT32                  u4FifoEnd;              ///< fifo end address
    IMG_UPD_FCT_T           pfImgUpdCb;             ///< call back function for update sth for image driver
    HANDLE_T                hFifo;                  ///< for memory phase III usage 
} IMGDMA_DRV_INIT_INFO_T;


typedef struct _IMGDMA_DRV_FIFO_STATUS_T
{
    UINT32 u4RdPoint;
    UINT32 u4WrPoint;
    UINT32 u4ValidData;
    BOOL   fgEoRange;
} IMGDMA_DRV_FIFO_STATUS_T;

typedef struct _IMGDMA_DRV_SRC_INFO_T
{
    UINT32 u4SrcSa;
    BOOL   fgCpy;
    DRV_TYPE_T eType;
    UINT16  u2DrvCompId;
} IMGDMA_DRV_SRC_INFO_T;

INT32 i4ImgdmaInit(void);
INT32 i4ImgdmaUnInit(void);
INT32 i4Imgdma_UnInit(UINT32 u4Case);
INT32 i4ImgdamGetTicket(UINT32 *pu4Handle);     ///< get ticket
INT32 i4ImgdmaGetFifoStatus(UINT32 u4Handle, IMGDMA_DRV_FIFO_STATUS_T *ptFifoStatus);    ///< get fifo status

INT32 i4ImgdmaSetReset(UINT32 u4Handle);   ///< reset transfer info
INT32 i4ImgdmaSetFifoInfo(UINT32 u4Handle, IMGDMA_DRV_INIT_INFO_T tImgdmaInitInfo);     ///< set fifo infomation
INT32 i4ImgdmaSetRange(UINT32 u4Handle, UINT32 u4Offset, UINT32 u4Size);                ///< set range
INT32 i4ImgdmaSetStart(UINT32 u4Handle);   ///< start transfer
INT32 i4ImgdmaSetBufFilled(UINT32 u4Handle, IMG_BUF_FILLED_T *ptBufFilledInfo);         ///< receive nfy
INT32 i4ImgdmaSetRdPoint(UINT32 u4Handle, UINT32 u4ReadPoint);        ///< set read point
INT32 i4ImgdmaSetRelTicket(UINT32 u4Handle);   ///< release ticket
INT32 i4ImgdmaSetRdPointOnly(UINT32 u4Handle, UINT32 u4ReadPoint);  ///< set read pointer only
BOOL fgImgdmaIsTransing(UINT32 u4Handle);
INT32 i4ImgdmaSetSrcInfo(UINT32 u4Handle, IMGDMA_DRV_SRC_INFO_T *ptImgdmaSrcInfo);


#endif //__DRV_IMG_DMA_H
