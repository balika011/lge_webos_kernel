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

/** @file irt_dma_vfy.h
 *
 *
 */

#ifndef __IRT_DMA_VRF_H__
#define __IRT_DMA_VRF_H__


//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
#include "x_drv_cli.h"

//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------
#ifndef RET_OK
#define RET_OK                0
#endif
#ifndef RET_ERR_UNKNOWN
#define RET_ERR_UNKNOWN      -1
#endif

#define EV_LOOP_DEF_CNT 	1000000
#define EV_MEM_SLOT_SIZE  (4*1024*1024)

#define EV_IRT_MAX_WIDTH                   2047
#define EV_IRT_MAX_HEIGHT                  2047

#define EV_DELAY_CNT          10
#define EV_IrtDma_MEM_SLOT_SIZE  (4*1024*1024) //just support 8bpp, 1920*1080*1

typedef struct
{
    UINT32 u4Para[12];
    UINT32 u2ParaDigit;
} EV_IRT_INPUT_PARA_T;


#define EV_IRT_PARA_INPUT_1        (1<<0)
#define EV_IRT_PARA_INPUT_2        (1<<1)
#define EV_IRT_PARA_INPUT_3        (1<<2)
#define EV_IRT_PARA_INPUT_4        (1<<3)
#define EV_IRT_PARA_INPUT_5        (1<<4)
#define EV_IRT_PARA_INPUT_6        (1<<5)
#define EV_IRT_PARA_INPUT_7        (1<<6)
#define EV_IRT_PARA_INPUT_8        (1<<7)
#define EV_IRT_PARA_INPUT_9        (1<<8)
#define EV_IRT_PARA_INPUT_10       (1<<9)
#define EV_IRT_PARA_INPUT_11       (1<<10)
#define EV_IRT_PARA_INPUT_12       (1<<11)

#define IRT_DMA_EV_DECODE_PARA(n)  \
        if (NULL != pu4Para##n)\
        {\
            if (!(prPara->u2ParaDigit & EV_IRT_PARA_INPUT_##n))\
            {\
                *pu4Para##n = (UINT32)rand();\
            }\
            else\
            {\
                *pu4Para##n  = prPara->u4Para[n - 1];\
            }\
        }

typedef INT32 (*IRT_IRT_EV_EXEC_FUNC)(EV_IRT_INPUT_PARA_T *prPara);

typedef struct
{
    UINT32  *p1IrtDmaEvSrcYSa; // default: SL420
    UINT32  *p1IrtDmaEvSrcCSa; // default: SL420
    UINT32  *p1IrtDmaEvMB420SrcYSa; // MB420: mb:16x32
    UINT32  *p1IrtDmaEvMB420SrcCSa; //
    UINT32  *p1IrtDmaEv5351SrcYSa; // 5351 mode, mb:64x32
    UINT32  *p1IrtDmaEv5351SrcCSa; //
    UINT32  *p1IrtDmaEvHwDstYSa; // SL420
    UINT32  *p1IrtDmaEvHwDstCSa; // SL420
    UINT32  *p1IrtDmaEvSwDstYSa; // SL420
    UINT32  *p1IrtDmaEvSwDstCSa; // SL420
    UINT32  *p1IrtDmaEvSwDstTempSa;
    UINT32  *p1IrtDmaEvHwSrcYSa; // pointer to p1IrtDmaEvSrcYSa or p1IrtDmaEvMB420SrcYSa or p1IrtDmaEv5351SrcYSa.
    UINT32  *p1IrtDmaEvHwSrcCSa;
} IRT_VFY_MEM_T;

extern IRT_VFY_MEM_T g_rIrtVfyMem;

#define EV_IRTDMA_SRC_Y_SA         g_rIrtVfyMem.p1IrtDmaEvSrcYSa
#define EV_IRTDMA_SRC_C_SA         g_rIrtVfyMem.p1IrtDmaEvSrcCSa
#define EV_IRTDMA_MB420_SRC_Y_SA   g_rIrtVfyMem.p1IrtDmaEvMB420SrcYSa
#define EV_IRTDMA_MB420_SRC_C_SA   g_rIrtVfyMem.p1IrtDmaEvMB420SrcCSa
#define EV_IRTDMA_5351_SRC_Y_SA    g_rIrtVfyMem.p1IrtDmaEv5351SrcYSa
#define EV_IRTDMA_5351_SRC_C_SA    g_rIrtVfyMem.p1IrtDmaEv5351SrcCSa
#define EV_IRTDMA_HW_DST_Y_SA      g_rIrtVfyMem.p1IrtDmaEvHwDstYSa
#define EV_IRTDMA_HW_DST_C_SA      g_rIrtVfyMem.p1IrtDmaEvHwDstCSa
#define EV_IRTDMA_SW_DST_Y_SA      g_rIrtVfyMem.p1IrtDmaEvSwDstYSa
#define EV_IRTDMA_SW_DST_C_SA      g_rIrtVfyMem.p1IrtDmaEvSwDstCSa
#define EV_IRTDMA_SW_DST_TEMP_SA   g_rIrtVfyMem.p1IrtDmaEvSwDstTempSa
#define EV_IRTDMA_HW_SRC_Y_SA      g_rIrtVfyMem.p1IrtDmaEvHwSrcYSa
#define EV_IRTDMA_HW_SRC_C_SA      g_rIrtVfyMem.p1IrtDmaEvHwSrcCSa


typedef struct
{
    BOOL fg5351Mode;
    UCHAR uc5351ModeSel;
    BOOL fgScanLineMode;
    UINT32 u4SrcWidthAlign;
    UINT32 u4SrcHeightAlign;
} IRT_VFY_SRC_MODE_T;

extern IRT_VFY_SRC_MODE_T   g_rIrtSrcMode;

#define EV_IRTDMA_W_ALIGN   g_rIrtSrcMode.u4SrcWidthAlign
#define EV_IRTDMA_H_ALIGN   g_rIrtSrcMode.u4SrcHeightAlign

//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------

//extern CLI_EXEC_T* GetIrtDmaCmdTbl(void);


#endif // __IRT_DMA_VRF_H__

