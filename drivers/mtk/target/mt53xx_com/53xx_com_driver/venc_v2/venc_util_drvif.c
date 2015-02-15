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
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: venc_util_drvif.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file venc_util_drvif.c
 *  Video Encoder - public interface
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "venc_drvif.h"
#include "venc_util_drvif.h"
#include "venc_debug.h"
#include "imgrz_if.h"
#include "imgrz_drvif.h"
#include "gfx_if.h"


/*
put bit function
max 32 bit
*/
void put_bits(VENC_BITS_BUFFER *p_buffer, UINT8 count, UINT32 bits)
{
    while (count >= p_buffer->bit_left)
    {
        if (count < 32)    bits = bits & ((((UINT32)1)<<count)-1);
        p_buffer->p_data[p_buffer->i_data] |= (UINT8)(bits >> (count - p_buffer->bit_left));
        count -= p_buffer->bit_left;
        p_buffer->bit_left = 8;
        p_buffer->i_data++;
        p_buffer->p_data[p_buffer->i_data] = 0;
    }

    if (count > 0) //left > count
    {
        bits = bits & ((((UINT32)1)<<count)-1);
        p_buffer->p_data[p_buffer->i_data] |= (UINT8)(bits << (p_buffer->bit_left-count));
        p_buffer->bit_left -= count;
    }
}

void inti_bits_buffer(VENC_BITS_BUFFER *p_buffer, UINT8 *p_addr)
{
    p_buffer->bit_left = 8;
    p_buffer->i_data = 0;
    p_buffer->p_data = p_addr;
    p_buffer->p_data[p_buffer->i_data] = 0;
}


void bits_align(VENC_BITS_BUFFER *p_buffer )
{
    if (p_buffer->bit_left != 8)
    {
        p_buffer->i_data ++;
        p_buffer->bit_left = 8;
        p_buffer->p_data[p_buffer->i_data] = 0;
    }
}

void bits_flush(VENC_BITS_BUFFER *p_buffer )
{
    bits_align(p_buffer);
}

//exp-golomb code from spec 9.1
UINT8 bits_ue_size(UINT32 value)
{
    UINT8 add=0;
    // max output 32 bit
    if (value > 254) {add = 16; value >>= 8;}

    if (value==0)
    {
        add +=1;
    }
    else if (value >=1 && value <= 2)
    {
        add +=3;
    }
    else if (value >=3 && value <= 6)
    {
        add +=5;
    }
    else if (value >=7 && value <= 14)
    {
        add +=7;
    }
    else if (value >=15 && value <= 30)
    {
        add +=9;
    }
    else if (value >=31 && value <= 62)
    {
        add +=11;
    }
    else if (value >=63 && value <= 126)
    {
        add +=13;
    }
    else if (value >=127 && value <= 254)
    {
        add +=15;
    }
    return add;
}

UINT8 bits_se_size(INT32 value)
{
    if ((1-value*2) < 0)
        value *= 2;
    else
        value = 1-value*2;
    return (value == 0) ? 1: bits_ue_size((UINT32)(value-1));
}


UINT32 bits_se_to_ue(INT32 value)
{
    if ((1-value*2) < 0)
        value *= 2;
    else
        value = 1-value*2;
    return (UINT32)value;
}


//-----------------------------------------------------------------------------
// resizer functions
//-----------------------------------------------------------------------------
typedef RZ_VDO_SCL_PARAM_SET_T VENC_HWREZ_MEMBERS_T;


static UINT32 _VEncHwRezInit(VENC_REZ_OBJ_T *ptObj)
{
    RZ_VDO_SCL_PARAM_SET_T *prScaleParam = (RZ_VDO_SCL_PARAM_SET_T *)ptObj->Members;

    x_memset(prScaleParam, 0, sizeof(RZ_VDO_SCL_PARAM_SET_T));

    switch (ptObj->ePixFmt)
    {
    case VENC_PIX_FMT_PSCAN:
    case VENC_PIX_FMT_NV12:
        prScaleParam->u4IsRsIn = 1;
        prScaleParam->u4SrcSwap = 0;
        break;
    case VENC_PIX_FMT_B16X32:
        prScaleParam->u4IsRsIn = 0;
        prScaleParam->u4SrcSwap = 3;
        break;
    case VENC_PIX_FMT_B64X32:
        prScaleParam->u4IsRsIn = 0;
        prScaleParam->u4SrcSwap = 6;
        break;
    default:
        ASSERT(0);
        break;
    }

    prScaleParam->u4InMode = E_RZ_VDO_OUTMD_420;
    prScaleParam->u4YSrcBufLen = ptObj->u4SrcPitch;//ALIGN_MASK(prVEncInfo->u4Width, 15);
    prScaleParam->u4YSrcHOffset = 0;
    prScaleParam->u4YSrcVOffset = 0;
    prScaleParam->u4YSrcW = ptObj->u4SrcW;//prVEncInfo->rCoarseInfo.u4Width * 4;//prVEncInfo->u4Width;
    prScaleParam->u4YSrcH = ptObj->u4SrcH;//prVEncInfo->rCoarseInfo.u4Height * 4;//prVEncInfo->u4Height;

    prScaleParam->u4CSrcHOffset = 0;
    prScaleParam->u4CSrcVOffset = 0;
    prScaleParam->u4CSrcW = (ptObj->u4SrcW) >> 1;
    prScaleParam->u4CSrcH = (ptObj->u4SrcH) >> 1;
#ifndef CC_MT5395
    prScaleParam->u4CSrcBufLen = ptObj->u4SrcPitch;//driver assert
#endif

    prScaleParam->u4IsRsOut = 1;
    prScaleParam->u4OutMode = E_RZ_VDO_OUTMD_420;       // 420|422|444 output
    prScaleParam->u4IsVdo2Osd = 0;     // output in osd format ?
    prScaleParam->u4YTgCM = E_RZ_VDO_OUTMD_420;
    prScaleParam->u4YTgBufLen = ptObj->u4TgPitch;
    prScaleParam->u4YTgHOffset = 0;
    prScaleParam->u4YTgVOffset = 0;
    prScaleParam->u4YTgW = ptObj->u4TgW;
    prScaleParam->u4YTgH = ptObj->u4TgH;

    prScaleParam->u4CTgBase = 0;       // cbcr
    prScaleParam->u4CTgW = prScaleParam->u4CSrcW;
    prScaleParam->u4CTgH = prScaleParam->u4CSrcH;
#ifndef CC_MT5395
    prScaleParam->u4CTgBufLen = ptObj->u4TgPitch;//driver assert
#endif

    prScaleParam->u4OutSwap = 0;

    return VENC_OK;
}

#ifdef CC_USE_DDI
static UINT32  _VEncHwRezResize(VENC_REZ_OBJ_T *ptObj, UINT32 u4SrcAddr, UINT32 u4TgAddr)
{
    RZ_VDO_SCL_PARAM_SET_T *prScaleParam = (RZ_VDO_SCL_PARAM_SET_T *)ptObj->Members;

    prScaleParam->u4CSrcBase = 0; // set u4CSrcBase = 0 to indicate Y only image resize
    prScaleParam->u4YSrcW = ptObj->u4SrcW;//prVEncInfo->rCoarseInfo.u4Width * 4;//prVEncInfo->u4Width;
    prScaleParam->u4YSrcH = ptObj->u4SrcH;//prVEncInfo->rCoarseInfo.u4Height * 4;//prVEncInfo->u4Height;

    prScaleParam->u4YSrcBase = u4SrcAddr;
    prScaleParam->u4YTgBase  = u4TgAddr;

    IMGRZ_Lock_Ex(E_SECND_RESIZER);
    IMGRZ_ReInit_Ex(E_SECND_RESIZER);
#ifdef CC_MT5395
    IMGRZ_SetScaleOpt_Ex(E_SECND_RESIZER, E_RZ_INOUT_TYPE_VDOENCODE);
#else
    IMGRZ_SetScaleOpt_Ex(E_SECND_RESIZER, E_RZ_INOUT_TYPE_VDOENCODE_V2);
#endif
    IMGRZ_Scale_Ex(E_SECND_RESIZER,(void *)(prScaleParam));
    IMGRZ_Flush_Ex(E_SECND_RESIZER);
    IMGRZ_Wait_Ex(E_SECND_RESIZER);
    IMGRZ_Unlock_Ex(E_SECND_RESIZER);

    return VENC_OK;
}
#else
static UINT32  _VEncHwRezResize(VENC_REZ_OBJ_T *ptObj, UINT32 u4SrcAddr, UINT32 u4TgAddr)
{
    RZ_VDO_SCL_PARAM_SET_T *prScaleParam = (RZ_VDO_SCL_PARAM_SET_T *)ptObj->Members;

    prScaleParam->u4CSrcBase = 0; // set u4CSrcBase = 0 to indicate Y only image resize
    prScaleParam->u4YSrcW = ptObj->u4SrcW;//prVEncInfo->rCoarseInfo.u4Width * 4;//prVEncInfo->u4Width;
    prScaleParam->u4YSrcH = ptObj->u4SrcH;//prVEncInfo->rCoarseInfo.u4Height * 4;//prVEncInfo->u4Height;

    prScaleParam->u4YSrcBase = u4SrcAddr;
    prScaleParam->u4YTgBase  = u4TgAddr;
	
	IMGRZ_Lock_Ex(E_FIRST_RESIZER);
	IMGRZ_ReInit_Ex(E_FIRST_RESIZER);
#ifdef CC_MT5395
	IMGRZ_SetScaleOpt_Ex(E_FIRST_RESIZER, E_RZ_INOUT_TYPE_VDOENCODE);
#else
	IMGRZ_SetScaleOpt_Ex(E_FIRST_RESIZER, E_RZ_INOUT_TYPE_VDOENCODE_V2);
#endif
	IMGRZ_Scale_Ex(E_FIRST_RESIZER, (void *)(prScaleParam));
	IMGRZ_Flush_Ex(E_FIRST_RESIZER);
	IMGRZ_Wait_Ex(E_FIRST_RESIZER);
	IMGRZ_Unlock_Ex(E_FIRST_RESIZER);


    return VENC_OK;
}
#endif

VENC_REZ_OBJ_T *VencCreateRezObject(void)
{
    VENC_REZ_OBJ_T *pObj;
    pObj = x_mem_alloc(sizeof(VENC_REZ_OBJ_T));

    if (pObj)
    {
        pObj->Init    = _VEncHwRezInit;
        pObj->Resize  = _VEncHwRezResize;
        pObj->Members = x_mem_alloc(sizeof(VENC_HWREZ_MEMBERS_T));

        if (!(pObj->Members))
        {
            x_mem_free(pObj);
            pObj = NULL;
        }
    }

    return pObj;
}


VOID VencDeleteRezObject(VENC_REZ_OBJ_T *pObj)
{
    if (pObj && pObj->Members) x_mem_free(pObj->Members);
    if (pObj) x_mem_free(pObj);
}


#ifdef VENC_EMU
//-----------------------------------------------------------------------------
// 95 sw resizer functions
//-----------------------------------------------------------------------------

static UINT32 _VEnc95SwRezInit(VENC_REZ_OBJ_T *ptObj)
{
    UNUSED(ptObj);
    return VENC_OK;
}

static UINT32  _VEnc95SwRezResize(VENC_REZ_OBJ_T *ptObj, UINT32 u4SrcAddr, UINT32 u4TgAddr)
{
    UINT8 *pbyDIP_ImageOrgY;
    UINT8 *pbyDIP_ImageOrgYs;
    UINT32 i_height, i_width, c_height, c_width, i, j;
    i_height = ptObj->u4SrcH;
    i_width  = VENC_ALIGN_MASK( ptObj->u4SrcW, 15);
    c_height = ptObj->u4TgH;
    c_width  = ptObj->u4TgW;

    // down scaling
    for (j=0; j<(c_height); j++)
    {
        for (i=0; i<(c_width); i++)
        {
            if ((4*j)<i_height)
            {
                pbyDIP_ImageOrgYs[j*c_width+i] = (BYTE)((
                                                   (((UINT32)(pbyDIP_ImageOrgY[(((j<<2)+0)*i_width)+(i<<2)+0])+
                                                     (UINT32)(pbyDIP_ImageOrgY[(((j<<2)+0)*i_width)+(i<<2)+1])+
                                                     (UINT32)(pbyDIP_ImageOrgY[(((j<<2)+0)*i_width)+(i<<2)+2])+
                                                     (UINT32)(pbyDIP_ImageOrgY[(((j<<2)+0)*i_width)+(i<<2)+3])+2)>>2)+
                                                   (((UINT32)(pbyDIP_ImageOrgY[(((j<<2)+1)*i_width)+(i<<2)+0])+
                                                     (UINT32)(pbyDIP_ImageOrgY[(((j<<2)+1)*i_width)+(i<<2)+1])+
                                                     (UINT32)(pbyDIP_ImageOrgY[(((j<<2)+1)*i_width)+(i<<2)+2])+
                                                     (UINT32)(pbyDIP_ImageOrgY[(((j<<2)+1)*i_width)+(i<<2)+3])+2)>>2)+
                                                   (((UINT32)(pbyDIP_ImageOrgY[(((j<<2)+2)*i_width)+(i<<2)+0])+
                                                     (UINT32)(pbyDIP_ImageOrgY[(((j<<2)+2)*i_width)+(i<<2)+1])+
                                                     (UINT32)(pbyDIP_ImageOrgY[(((j<<2)+2)*i_width)+(i<<2)+2])+
                                                     (UINT32)(pbyDIP_ImageOrgY[(((j<<2)+2)*i_width)+(i<<2)+3])+2)>>2)+
                                                   (((UINT32)(pbyDIP_ImageOrgY[(((j<<2)+3)*i_width)+(i<<2)+0])+
                                                     (UINT32)(pbyDIP_ImageOrgY[(((j<<2)+3)*i_width)+(i<<2)+1])+
                                                     (UINT32)(pbyDIP_ImageOrgY[(((j<<2)+3)*i_width)+(i<<2)+2])+
                                                     (UINT32)(pbyDIP_ImageOrgY[(((j<<2)+3)*i_width)+(i<<2)+3])+2)>>2)+2)>>2);
            }
            else
            {
                  pbyDIP_ImageOrgYs[j*c_width+i] = (BYTE)
                                                   (((UINT32)(pbyDIP_ImageOrgY[((i_height-1)*i_width)+(i<<2)+0])+
                                                     (UINT32)(pbyDIP_ImageOrgY[((i_height-1)*i_width)+(i<<2)+1])+
                                                     (UINT32)(pbyDIP_ImageOrgY[((i_height-1)*i_width)+(i<<2)+2])+
                                                     (UINT32)(pbyDIP_ImageOrgY[((i_height-1)*i_width)+(i<<2)+3])+2)>>2);
            }
        }
    }

    return VENC_OK;

}


VENC_REZ_OBJ_T *VencCreate95SwRezObject(void)
{
    VENC_REZ_OBJ_T *pObj;
    pObj = x_mem_alloc(sizeof(VENC_REZ_OBJ_T));

    if (pObj)
    {
        pObj->Init    = _VEnc95SwRezInit;
        pObj->Resize  = _VEnc95SwRezResize;
        pObj->Members = NULL;
    }

    return pObj;
}


VOID VencDelete95SwRezObject(VENC_REZ_OBJ_T *pObj)
{
    if (pObj) x_mem_free(pObj);
}


//-----------------------------------------------------------------------------
// 68 sw resizer functions
//-----------------------------------------------------------------------------
static UINT32 _SwB2R_GetBuff(VOID)
{
    static UINT32 u4SwB2rBuf = 0;
    if (!u4SwB2rBuf)
    {
        u4SwB2rBuf = (UINT32)BSP_AllocAlignedDmaMemory((1280 * 736), 1024);
        ASSERT(u4SwB2rBuf);
    }
    return u4SwB2rBuf;
}


// transform luma image from 64x32 block mode to raster scan
static VOID _SwB2R_64x32(VENC_REZ_OBJ_T *ptObj, UINT8 *pu1Src, UINT8 *pu1Dst)
{
    const UINT32 u4MbSize = 64 * 32;
    UINT32 u4MbRowStart, u4MbStart;
    UINT32 u4MbY, u4MbX, y;
    UINT32 u4OutH, u4OutW, u4MbRowSize;
    u4OutH      = ptObj->u4SrcH;
    u4OutW      = VENC_ALIGN_MASK(ptObj->u4SrcW, 63);
    u4MbRowSize = ptObj->u4SrcPitch << 5;

    for (u4MbRowStart = u4MbY = 0; u4MbY < u4OutH; u4MbY += 32, u4MbRowStart += u4MbRowSize)
    {
        for (u4MbX = 0, u4MbStart = u4MbRowStart; u4MbX < u4OutW; u4MbX += 64, u4MbStart += u4MbSize)
        {
            for (y = 0; y < 32; y++)
            {
                x_memcpy(&(*(char(*)[u4OutH][u4OutW])pu1Dst)[u4MbY + y][u4MbX],
                    (*(char(*)[32][64])(pu1Src + u4MbStart))[y],
                    64);
            }
        }
    }
}


// transform luma image from 16x32 block mode to raster scan
static VOID _SwB2R_16x32(VENC_REZ_OBJ_T *ptObj, UINT8 *pu1Src, UINT8 *pu1Dst)
{
    const UINT32 u4MbSize = 16 * 32;
    UINT32 u4MbRowStart, u4MbStart;
    UINT32 u4MbY, u4MbX, y;
    UINT32 u4OutH, u4OutW, u4MbRowSize;
    u4OutH      = ptObj->u4SrcH;
    u4OutW      = VENC_ALIGN_MASK(ptObj->u4SrcW, 15);
    u4MbRowSize = ptObj->u4SrcPitch << 5;

    for (u4MbRowStart = u4MbY = 0; u4MbY < u4OutH; u4MbY += 32, u4MbRowStart += u4MbRowSize)
    {
        for (u4MbX = 0, u4MbStart = u4MbRowStart; u4MbX < u4OutW; u4MbX += 16, u4MbStart += u4MbSize)
        {
            for (y = 0; y < 32; y++)
            {
                x_memcpy(&(*(char(*)[u4OutH][u4OutW])pu1Dst)[u4MbY + y][u4MbX],
                    (*(char(*)[32][16])(pu1Src + u4MbStart))[y],
                    16);
            }
        }
    }
}


static VOID _SwImgrz_68(VENC_REZ_OBJ_T *ptObj, UINT8 *pbyDIP_ImageOrgY, UINT8 *pbyDIP_ImageOrgYs)
{
    UINT32 i_height, i_width, c_height, c_width, i, j;
    i_height = ptObj->u4SrcH;
    if (ptObj->ePixFmt == VENC_PIX_FMT_B16X32)
    {
        i_width = VENC_ALIGN_MASK(ptObj->u4SrcW, 15);
    }
    else if (ptObj->ePixFmt == VENC_PIX_FMT_B64X32)
    {
        i_width = VENC_ALIGN_MASK(ptObj->u4SrcW, 63);
    }
    else
    {
        i_width = VENC_ALIGN_MASK(ptObj->u4SrcW, 15);
    }

    c_height = ptObj->u4TgH;
    c_width  = ptObj->u4TgW;

    // down scaling
    for (j=0; j<(c_height); j++)
    {
        for (i=0; i<(c_width); i++)
        {
            if ((4*j)<i_height)
            {
                //fast coarse frame generate
                pbyDIP_ImageOrgYs[j*c_width+i] =(BYTE)(((UINT32)(pbyDIP_ImageOrgY[((j<<2)+0)*i_width + (i<<2)+0])+
                                                    (UINT32)(pbyDIP_ImageOrgY[((j<<2)+0)*i_width + (i<<2)+1])+
                                                    (UINT32)(pbyDIP_ImageOrgY[((j<<2)+0)*i_width + (i<<2)+2])+
                                                    (UINT32)(pbyDIP_ImageOrgY[((j<<2)+0)*i_width + (i<<2)+3])+2)>>2);
            }
            else
            {
                pbyDIP_ImageOrgYs[j*c_width+i] = (BYTE)(((UINT32)(pbyDIP_ImageOrgY[((((i_height-1)>>2)<<2)*i_width) + (i<<2)+0])+
                                                    (UINT32)(pbyDIP_ImageOrgY[((((i_height-1)>>2)<<2)*i_width) + (i<<2)+1])+
                                                    (UINT32)(pbyDIP_ImageOrgY[((((i_height-1)>>2)<<2)*i_width) + (i<<2)+2])+
                                                    (UINT32)(pbyDIP_ImageOrgY[((((i_height-1)>>2)<<2)*i_width) + (i<<2)+3])+2)>>2) ;
            }
        }
    }
}


static UINT32 _VEnc68SwRezInit(VENC_REZ_OBJ_T *ptObj)
{
    UNUSED(ptObj);
    return VENC_OK;
}


static UINT32  _VEnc68SwRezResize(VENC_REZ_OBJ_T *ptObj, UINT32 u4SrcAddr, UINT32 u4TgAddr)
{
    if (ptObj->ePixFmt == VENC_PIX_FMT_B16X32)
    {
        UINT32 u4Buff = _SwB2R_GetBuff();
        _SwB2R_16x32(ptObj, (UINT8 *)u4SrcAddr, (UINT8 *)u4Buff);
        _SwImgrz_68 (ptObj, (UINT8 *)u4Buff, (UINT8 *)u4TgAddr);
    }
    else if (ptObj->ePixFmt == VENC_PIX_FMT_B64X32)
    {
        UINT32 u4Buff = _SwB2R_GetBuff();
        _SwB2R_64x32(ptObj, (UINT8 *)u4SrcAddr, (UINT8 *)u4Buff);
        _SwImgrz_68 (ptObj, (UINT8 *)u4Buff, (UINT8 *)u4TgAddr);
    }
    else
    {
        _SwImgrz_68 (ptObj, (UINT8 *)u4SrcAddr, (UINT8 *)u4TgAddr);
    }

    return VENC_OK;
}


VENC_REZ_OBJ_T *VencCreate68SwRezObject(void)
{
    VENC_REZ_OBJ_T *pObj;
    pObj = x_mem_alloc(sizeof(VENC_REZ_OBJ_T));

    if (pObj)
    {
        pObj->Init    = _VEnc68SwRezInit;
        pObj->Resize  = _VEnc68SwRezResize;
        pObj->Members = NULL;
    }

    return pObj;
}


VOID VencDelete68SwRezObject(VENC_REZ_OBJ_T *pObj)
{
    if (pObj) x_mem_free(pObj);
}
#endif



/*
BUFFER MANAGER
*/
#define SNs (*((volatile UINT8 *)&(pObj->u1StackNs)))

UINT32 _BUFMGR_OBJ_Add(VENC_BUFMGR_BASE_T *pObj, VOID *pvData)
{
    UINT32  u4StackAddr;
    UINT32  u4DataAddr;

    if (!pObj || !pvData)
    {
        ASSERT(pObj && pvData);
        return VENC_BUFMGR_INV_ARG;
    }

    if (pObj->u1Size >= pObj->u1MaxSize)
    {
        ASSERT(pObj->u1Size < pObj->u1MaxSize);
        return VENC_BUFMGR_FULL;
    }

    u4StackAddr   = ((UINT32)pObj + (VENC_MEMBER_OFFSET(VENC_BUFMGR_BASE_T, Free) + 4));
    //sizeof(UINT32) * MaxSize
    u4DataAddr    = u4StackAddr + ((pObj->u1MaxSize)<<2);
    u4DataAddr   += (pObj->u1Size * pObj->u1MemSize);
    x_memcpy((VOID *)u4DataAddr, pvData, pObj->u1MemSize);

    (*((UINT32 *)u4StackAddr + SNs)) = pObj->u1Size;
    SNs++;
    pObj->u1Size++;

    VERIFY(x_sema_unlock(pObj->hCntSem) == OSR_OK);

    return VENC_BUFMGR_OK;
 }

VOID _BUFMGR_OBJ_Reset(VENC_BUFMGR_BASE_T *pObj)
{
    INT32 i4Ret;

    ASSERT(pObj);
    pObj->u1Size = 0;
    SNs = 0;

    do
    {
        i4Ret =  x_sema_lock(pObj->hCntSem, X_SEMA_OPTION_NOWAIT);
    } while (i4Ret == OSR_OK);

    if (i4Ret != OSR_WOULD_BLOCK)
    {
        ASSERT(0);
    }
}

UINT32 _BUFMGR_OBJ_Get(VENC_BUFMGR_BASE_T *pObj, VOID *pvData, UINT32 u4Option)
{
    CRIT_STATE_T rState;
    UINT8   ret;
    UINT32  u4StackAddr;
    UINT32  u4DataAddr;
    UINT32  *puStack;
    INT32 i4Ret;

    if (!pObj || !pvData)
    {
        return VENC_BUFMGR_INV_ARG;
    }

    //empty
    if (!(SNs))
    {
        if (VENC_BUFMGR_NOWAIT == u4Option)
        {
            return VENC_BUFMGR_EMPTY;
        }
        else if (VENC_BUFMGR_WAIT_30ms == u4Option)
        {
            i4Ret = x_sema_lock_timeout(pObj->hCntSem, 30);
            if (OSR_OK != i4Ret)
            {
                return VENC_BUFMGR_TIMEOUT;
            }
        }
        else
        {
            //wait & lock todo
            i4Ret = x_sema_lock(pObj->hCntSem,X_SEMA_OPTION_WAIT);
            if (OSR_OK != i4Ret)
            {
                LOG(0, "lock fail %d\n", i4Ret);
                ASSERT(0);
                return VENC_BUFMGR_FAIL;
            }
        }
    }
    else
    {
        if (VENC_BUFMGR_NOWAIT == u4Option)
        {
            i4Ret = x_sema_lock(pObj->hCntSem,X_SEMA_OPTION_NOWAIT);
            if (OSR_OK != i4Ret)
            {
                return VENC_BUFMGR_EMPTY;
            }
        }
        else
        {
            i4Ret = x_sema_lock(pObj->hCntSem,X_SEMA_OPTION_WAIT);
            if (OSR_OK != i4Ret)
            {
                LOG(0, "lock fail %d\n", i4Ret);
                ASSERT(0);
                return VENC_BUFMGR_FAIL;
            }
        }
    }

    u4StackAddr   = ((UINT32)pObj + (VENC_MEMBER_OFFSET(VENC_BUFMGR_BASE_T, Free) + 4));
    puStack       = (UINT32 *)u4StackAddr;
    //sizeof(UINT32) * MaxSize
    u4DataAddr    = u4StackAddr + ((pObj->u1MaxSize)<<2);

    LOG(6, "%p get %d\n", pObj, SNs);
    rState = x_crit_start();
    ASSERT(SNs);
    SNs--;
    ret = puStack[SNs];
    x_crit_end(rState);

    u4DataAddr  += (ret * pObj->u1MemSize);
    x_memcpy(pvData, (VOID *)u4DataAddr, pObj->u1MemSize);


    return VENC_BUFMGR_OK;
}


UINT32 _BUFMGR_OBJ_Free(VENC_BUFMGR_BASE_T *pObj, VOID *pvData)
{
    UINT8 i;
    UINT32  u4StackAddr;
    UINT32  u4DataAddr;
    UINT32  *puStack;

    ASSERT(pObj && pvData);

    if (!pObj || !pvData)
    {
        return VENC_BUFMGR_INV_ARG;
    }

    u4StackAddr   = ((UINT32)pObj + (VENC_MEMBER_OFFSET(VENC_BUFMGR_BASE_T, Free) + 4));
    puStack       = (UINT32 *)u4StackAddr;
    //sizeof(UINT32) * MaxSize
    u4DataAddr    = u4StackAddr + ((pObj->u1MaxSize)<<2);

    for (i = 0; i < pObj->u1Size; i++)
    {
        if (x_memcmp((VOID *)u4DataAddr, pvData, pObj->u1MemSize) == 0)
        {
            break;
        }
        u4DataAddr += pObj->u1MemSize;
    }

    if (i < pObj->u1Size)
    {
        CRIT_STATE_T rState;
        LOG(6, "%p free %d\n", pObj, SNs);
        rState = x_crit_start();
        puStack[SNs++] =  i;

        // put unlock in crtical section for sync SNs & hCntSem
        VERIFY(x_sema_unlock(pObj->hCntSem) == OSR_OK);
        x_crit_end(rState);

        if (SNs > pObj->u1Size)
        {
            LOG(0, "%d, %d\n", SNs, pObj->u1Size);
            ASSERT(SNs <= pObj->u1Size);
        }
    }
    else
    {
        return VENC_BUFMGR_FAIL;
    }

    return VENC_BUFMGR_OK;
}

//Filter
#ifdef VENC_FRAME_CTRL_BY_DRV
BOOL _VENC_Filter_Do(VENC_FILTER_T *ptFilter, UINT32 u4Pts)
{
    BOOL fgRet = TRUE;
    ASSERT(ptFilter);

    if (ptFilter->u4NextPts == 0)
    {
        ptFilter->u4NextPts = u4Pts;
    }
    else if (ptFilter->u4NextPts > u4Pts)
    {
        fgRet = FALSE;
    }

    return fgRet;
}

VOID _VENC_Filter_Update(VENC_FILTER_T *ptFilter)
{
    ASSERT(ptFilter);
    ptFilter->u4NextPts += ptFilter->u4PtsDelta;
}


VOID _VENC_Filter_Reset(VENC_FILTER_T *ptFilter)
{
    ASSERT(ptFilter);
    ptFilter->u4NextPts  = 0;
}

VOID _VENC_Filter_Init(VENC_FILTER_T *ptFilter, UINT32 u4Delta)
{
    ASSERT(ptFilter);
    ptFilter->u4PtsDelta = u4Delta;
    ptFilter->u4NextPts  = 0;
}
#endif /*VENC_FRAME_CTRL_BY_DRV*/


// mirror / flip
VOID VencMirrorFlip(VENC_OBJECT_T *this, VENC_IN_PIC_T *ptIn, VENC_FRM_T *ptOut)
{
    GFX_Lock();
    GFX_MMU_Set_Enable(0x0);
    
    // do Luma
    GFX_SetSrc((UINT8 *)ptIn->u4YAddr, CM_RGB_CLUT8, this->u2Pitch);
    GFX_SetDst((UINT8 *)ptOut->u4YAddr, CM_RGB_CLUT8, this->u2Pitch);
    GFX_SetBltOpt(this->u4GfxConfig, 0, 0);
    GFX_BitBlt(0, 0, 0, 0, this->u2Width, this->u2Height);
    GFX_Flush();
    GFX_Wait();

    // do chroma
    GFX_SetSrc((UINT8 *)ptIn->u4CAddr, CM_ARGB4444_DIRECT16, this->u2Pitch);
    GFX_SetDst((UINT8 *)ptOut->u4UVAddr, CM_ARGB4444_DIRECT16, this->u2Pitch);
    GFX_SetBltOpt(this->u4GfxConfig, 0, 0);
    GFX_BitBlt(0, 0, 0, 0, this->u2Width / 2, this->u2Height / 2);
    GFX_Flush();
    GFX_Wait();

    GFX_Unlock();
}

#ifdef INPUT_BUF_FROM_SCALER
extern BOOL fgTriggerScaler;
extern UINT8 u1DrvScpipGetVencModule(void);
extern void u1DrvScpipH264FreeCurrentFb(UINT32 u4YAddr);
#endif

// resize source
VOID VencRezSrc(VENC_OBJECT_T *this, VENC_IN_PIC_T *ptIn)
{
    RZ_VDO_SCL_PARAM_SET_T rImgrzScaleParam;
    VENC_FRM_T rFrm;
    UINT32 u4Ret, u4YAddrBak, u4CAddrBak;

    LOG(6, "%s: rz: %d, gfx:0x%08x\n", __FUNCTION__, this->fgResizeSrc, this->u4GfxConfig);
    if (!this->fgResizeSrc && !this->u4GfxConfig)
    {
        LOG(0, "%s: no resize or gfx op!!\n", __FUNCTION__);
        ASSERT(0);
        return;
    }

    u4Ret = _BUFMGR_OBJ_Get((VENC_BUFMGR_BASE_T *)this->pvOut, (VOID *)&rFrm, VENC_BUFMGR_WAIT);
    if (VENC_BUFMGR_OK != u4Ret)
    {
        LOG(0, "%s: get buf from %p fail %d\n", __FUNCTION__, this->pvOut, u4Ret);
        ASSERT(VENC_BUFMGR_OK == u4Ret);
        return;
    }

    u4YAddrBak = ptIn->u4YAddr;
    u4CAddrBak = ptIn->u4CAddr;

    LOG(6, "%s, input Y 0x%x C 0x%x,  target Y 0x%x C 0x%x\n", __FUNCTION__, ptIn->u4YAddr, ptIn->u4CAddr, rFrm.u4YAddr, rFrm.u4UVAddr);

    if (!this->fgResizeSrc && this->u4GfxConfig)
    {
        VencMirrorFlip(this, ptIn, &rFrm);
        goto VENC_RZ_SRC_DONE;
    }

    x_memset(&rImgrzScaleParam,0,sizeof(RZ_VDO_SCL_PARAM_SET_T));

    switch (this->ePixFmt)
    {
    case VENC_PIX_FMT_PSCAN:
    case VENC_PIX_FMT_NV12:
        rImgrzScaleParam.u4IsRsIn = 1;
        rImgrzScaleParam.u4SrcSwap = 0;
        break;
    case VENC_PIX_FMT_B16X32:
        rImgrzScaleParam.u4IsRsIn = 0;
        rImgrzScaleParam.u4SrcSwap = 3;
        break;
    case VENC_PIX_FMT_B64X32:
        rImgrzScaleParam.u4IsRsIn = 0;
        rImgrzScaleParam.u4SrcSwap = 6;
        break;
    default:
        LOG(0, "%s: unknow pix fmt %d\n", __FUNCTION__, this->ePixFmt);
        ASSERT(0);
        return;
    }

    rImgrzScaleParam.u4OutSwap = 0;
    rImgrzScaleParam.u4InMode = E_RZ_VDO_OUTMD_420;
    rImgrzScaleParam.u4YSrcBase = u4YAddrBak;
    rImgrzScaleParam.u4YSrcBufLen = this->u4SrcPitch;
    rImgrzScaleParam.u4YSrcHOffset = 0;
    rImgrzScaleParam.u4YSrcVOffset = 0;
    rImgrzScaleParam.u4YSrcW = this->u4SrcWidth;
    rImgrzScaleParam.u4YSrcH = this->u4SrcHeight;
    rImgrzScaleParam.u4CSrcBase = u4CAddrBak;
    rImgrzScaleParam.u4CSrcHOffset = 0;
    rImgrzScaleParam.u4CSrcVOffset = 0;
    rImgrzScaleParam.u4CSrcW       = rImgrzScaleParam.u4YSrcW >> 1;
    rImgrzScaleParam.u4CSrcH = rImgrzScaleParam.u4YSrcH >> 1;

    rImgrzScaleParam.u4IsRsOut = 1;
    rImgrzScaleParam.u4OutMode = E_RZ_VDO_OUTMD_420;       // 420|422|444 output
    rImgrzScaleParam.u4IsVdo2Osd = 0;     // output in osd format ?
    rImgrzScaleParam.u4YTgCM = E_RZ_VDO_OUTMD_420;
    rImgrzScaleParam.u4YTgBufLen   = this->u2Pitch;
    rImgrzScaleParam.u4YTgHOffset = 0;
    rImgrzScaleParam.u4YTgVOffset = 0;
    rImgrzScaleParam.u4YTgW        = this->u2Width;
    rImgrzScaleParam.u4YTgH        = this->u2Height;
    rImgrzScaleParam.u4CTgW        = rImgrzScaleParam.u4YTgW >> 1;
    rImgrzScaleParam.u4CTgH        = rImgrzScaleParam.u4YTgH >> 1;

    if (this->u4GfxConfig)
    {
        rImgrzScaleParam.u4YTgBase     = this->u4GfxTmpY;
        rImgrzScaleParam.u4CTgBase     = this->u4GfxTmpC;
    }
    else
    {
        rImgrzScaleParam.u4YTgBase     = rFrm.u4YAddr;
        rImgrzScaleParam.u4CTgBase     = rFrm.u4UVAddr;
    }

    IMGRZ_Lock_Ex(E_FIRST_RESIZER);
    IMGRZ_ReInit_Ex(E_FIRST_RESIZER);
    IMGRZ_SetScaleOpt_Ex(E_FIRST_RESIZER, E_RZ_INOUT_TYPE_VDOMD);
    IMGRZ_Scale_Ex(E_FIRST_RESIZER, (void *)(&rImgrzScaleParam));
    IMGRZ_Flush_Ex(E_FIRST_RESIZER);
    IMGRZ_Wait_Ex(E_FIRST_RESIZER);
    IMGRZ_Unlock_Ex(E_FIRST_RESIZER);

    if (this->u4GfxConfig)
    {
        ptIn->u4YAddr = this->u4GfxTmpY;
        ptIn->u4CAddr = this->u4GfxTmpC;
        VencMirrorFlip(this, ptIn, &rFrm);
    }

VENC_RZ_SRC_DONE:
    ptIn->u4YAddr = rFrm.u4YAddr;
    ptIn->u4CAddr = rFrm.u4UVAddr;

#if 1//def INPUT_BUF_FROM_SCALER
    //To avoid define Module name again, apply number directly here
    //E_VENC_MODULE_MAIN_SCALER  (0), E_VENC_MODULE_SUB_SCALER	 (1)
    //E_VENC_MODULE_MON_SCALER	 (2), E_VENC_MODULE_SUB_DI       (3)
    //E_VENC_MODULE_MAX          (4)
    if((u1DrvScpipGetVencModule() == 1)
         || (u1DrvScpipGetVencModule() ==2))
    {
        u1DrvScpipH264FreeCurrentFb(u4YAddrBak);
    }
    else
#endif
    {
        // free input buffer
        rFrm.u4YAddr  = u4YAddrBak;
        rFrm.u4UVAddr = u4CAddrBak;
        _BUFMGR_OBJ_Free((VENC_BUFMGR_BASE_T *)this->pvIn, (VOID *)&rFrm);
    }
}


// get thumbnail
static VOID VencGenThumbnailYuv(
    VENC_OBJECT_T *this,
    VENC_IN_PIC_T *ptIn,
    UINT32 u4YTgBase,
    UINT32 u4CTgBase)
{
    RZ_VDO_SCL_PARAM_SET_T rImgrzScaleParam;

    x_memset(&rImgrzScaleParam,0,sizeof(RZ_VDO_SCL_PARAM_SET_T));

    if (ptIn->fgRzSrc)
    {
        rImgrzScaleParam.u4IsRsIn = 1;
        rImgrzScaleParam.u4SrcSwap = 0;
    }
    else
    {
        switch (this->ePixFmt)
        {
        case VENC_PIX_FMT_PSCAN:
        case VENC_PIX_FMT_NV12:
            rImgrzScaleParam.u4IsRsIn = 1;
            rImgrzScaleParam.u4SrcSwap = 0;
            break;
        case VENC_PIX_FMT_B16X32:
            rImgrzScaleParam.u4IsRsIn = 0;
            rImgrzScaleParam.u4SrcSwap = 3;
            break;
        case VENC_PIX_FMT_B64X32:
            rImgrzScaleParam.u4IsRsIn = 0;
            rImgrzScaleParam.u4SrcSwap = 6;
            break;
        default:
            ASSERT(0);
            break;
        }
    }

    rImgrzScaleParam.u4OutSwap = 0;
    rImgrzScaleParam.u4InMode = E_RZ_VDO_OUTMD_420;
    rImgrzScaleParam.u4YSrcBase = ptIn->u4YAddr;
    rImgrzScaleParam.u4YSrcBufLen = this->u2Pitch;
    rImgrzScaleParam.u4YSrcHOffset = 0;
    rImgrzScaleParam.u4YSrcVOffset = 0;
    rImgrzScaleParam.u4YSrcW = this->u2Width;
    rImgrzScaleParam.u4YSrcH = this->u2Height;
    rImgrzScaleParam.u4CSrcBase = ptIn->u4CAddr;
    rImgrzScaleParam.u4CSrcHOffset = 0;
    rImgrzScaleParam.u4CSrcVOffset = 0;
    rImgrzScaleParam.u4CSrcW       = rImgrzScaleParam.u4YSrcW >> 1;
    rImgrzScaleParam.u4CSrcH = rImgrzScaleParam.u4YSrcH >> 1;

    rImgrzScaleParam.u4IsRsOut = 1;
    rImgrzScaleParam.u4OutMode = E_RZ_VDO_OUTMD_420;       // 420|422|444 output
    rImgrzScaleParam.u4IsVdo2Osd = 0;     // output in osd format ?
    rImgrzScaleParam.u4YTgCM = E_RZ_VDO_OUTMD_420;
    rImgrzScaleParam.u4YTgBufLen   = this->u4ThumbnailPitch;
    rImgrzScaleParam.u4YTgHOffset = 0;
    rImgrzScaleParam.u4YTgVOffset = 0;
    rImgrzScaleParam.u4YTgW        = this->u4ThumbnailWidth;
    rImgrzScaleParam.u4YTgH        = this->u4ThumbnailHeight;
    rImgrzScaleParam.u4CTgW        = rImgrzScaleParam.u4YTgW >> 1;
    rImgrzScaleParam.u4CTgH        = rImgrzScaleParam.u4YTgH >> 1;
    rImgrzScaleParam.u4YTgBase     = u4YTgBase;
    rImgrzScaleParam.u4CTgBase     = u4CTgBase;

    IMGRZ_Lock_Ex(E_FIRST_RESIZER);
    IMGRZ_ReInit_Ex(E_FIRST_RESIZER);
    IMGRZ_SetScaleOpt_Ex(E_FIRST_RESIZER, E_RZ_INOUT_TYPE_VDOMD);
    IMGRZ_Scale_Ex(E_FIRST_RESIZER, (void *)(&rImgrzScaleParam));
    IMGRZ_Flush_Ex(E_FIRST_RESIZER);
    IMGRZ_Wait_Ex(E_FIRST_RESIZER);
    IMGRZ_Unlock_Ex(E_FIRST_RESIZER);
}


static VOID VencThumbnailYuv420ToARGB(VENC_OBJECT_T *this, UINT32 u4YBase, UINT32 u4CBase, UINT32 u4ARGB)
{
    UINT32 u4SrcLumaPitch = this->u4ThumbnailPitch;
    UINT32 u4SrcChromaPitch = u4SrcLumaPitch / 2;

    GFX_Lock();
    GFX_SetColCnvFmt(E_YCFMT_420LINEAR,E_SWAP_0,E_VSTD_BT709,E_VSYS_COMP);
    GFX_SetColCnvSrc((UINT8 *)u4YBase,u4SrcLumaPitch, (UINT8 *)u4CBase, u4SrcChromaPitch, 0);
    GFX_SetDst((UINT8 *)u4ARGB, CM_ARGB8888_DIRECT32, u4SrcLumaPitch * 4);
    GFX_SetAlpha((UINT32)0xff);
    GFX_ColConv(0, 0, this->u4ThumbnailWidth, this->u4ThumbnailHeight);
    GFX_Flush();
    GFX_Wait();
    GFX_Unlock();
}


BOOL VencGetThumbnail(VENC_OBJECT_T *this, VENC_IN_PIC_T *ptIn, UINT32 u4YUV420, UINT32 u4ARGB)
{
    UINT32 u4CbCrOffset = this->u4ThumbnailPitch * this->u4ThumbnailHeight;

    HalFlushInvalidateDCacheMultipleLine(u4ARGB, u4CbCrOffset);
    // step 1 resize
    VencGenThumbnailYuv(this, ptIn, u4YUV420, u4YUV420 + u4CbCrOffset);

    // step 2 yuv420 to ARGB
    VencThumbnailYuv420ToARGB(this, u4YUV420, u4YUV420 + u4CbCrOffset, u4ARGB);

    // step 3 callback
    this->pfnThumbnailCb(this->pvThumbnailTag, u4ARGB);

    return TRUE;
}


