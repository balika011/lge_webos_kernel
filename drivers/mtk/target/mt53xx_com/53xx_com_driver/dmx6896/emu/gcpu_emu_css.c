/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2007, MediaTek Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: dmx_emu_drm.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_emu_des.c
 *  Demux driver - DES/3DES emulation
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_assert.h"
#include "x_printf.h"
#include "x_rand.h"
#include "x_os.h"

#include "../gcpu.h"
#include "css_matrix.h"
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** GCPU_EMU_CSSAV
 */
//-----------------------------------------------------------------------------
INT32 GCPU_EMU_CSSAV()
{
    BYTE *pbSrcBufAddr = NULL, *pbDstBufAddr = NULL, *pbGoldenBufAddr = NULL;
    UINT32 u4SrcOffset, u4DstOffset, u4GoldenFileSize, u4BufSize;
    CSS_DK_PARAM_T rCssDkParam;
    CSS_TK_PARAM_T rCssTkParam;
    CSS_AV_PARAM_T rCssAvParam;

    rCssDkParam.au1RDK[0] = 0xfb;
    rCssDkParam.au1RDK[1] = 0x81;
    rCssDkParam.au1RDK[2] = 0x26;
    rCssDkParam.au1RDK[3] = 0x01;
    rCssDkParam.au1RDK[4] = 0xfe;

    rCssDkParam.au1EDK[0] = 0x1d;
    rCssDkParam.au1EDK[1] = 0x99;
    rCssDkParam.au1EDK[2] = 0x0a;
    rCssDkParam.au1EDK[3] = 0xd3;
    rCssDkParam.au1EDK[4] = 0x45;

    GCPU_Cmd(0, GCPU_CSS_DEC_DK, &rCssDkParam);

    rCssTkParam.au1ETK[0] = 0x28;
    rCssTkParam.au1ETK[1] = 0x01;
    rCssTkParam.au1ETK[2] = 0x88;
    rCssTkParam.au1ETK[3] = 0xa5;
    rCssTkParam.au1ETK[4] = 0x05;

    GCPU_Cmd(0, GCPU_CSS_DEC_TK, &rCssTkParam);

    u4GoldenFileSize = sizeof(au1MatrixDec);
    u4BufSize = ((u4GoldenFileSize + 2047) / 2048) * 2048; //one AV packet is 2048
    u4SrcOffset = rand() % u4BufSize;
    u4DstOffset = rand() % u4BufSize;

    pbSrcBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    pbDstBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, GCPU_FIFO_ALIGNMENT);
    if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
    {
        ASSERT(0);
    }

    _DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, au1MatrixEnc, u4GoldenFileSize);
    x_memcpy(pbGoldenBufAddr, au1MatrixDec, sizeof(au1MatrixDec));

    rCssAvParam.u4SrcStartAddr = (UINT32) pbSrcBufAddr + u4SrcOffset;
    rCssAvParam.u4SrcBufStart = (UINT32) pbSrcBufAddr;
    rCssAvParam.u4SrcBufEnd = (UINT32) pbSrcBufAddr + u4BufSize;
    rCssAvParam.u4DstStartAddr = (UINT32) pbDstBufAddr + u4DstOffset;
    rCssAvParam.u4DstBufStart = (UINT32) pbDstBufAddr;
    rCssAvParam.u4DstBufEnd = (UINT32) pbDstBufAddr + u4BufSize;
    rCssAvParam.u4DatLen  = u4GoldenFileSize;
    rCssAvParam.eMode = CSS_AV_MODE_AUTO;

    GCPU_Cmd(0, GCPU_CSS_DSC_AV, &rCssAvParam);

    if (0xFFFFFFFF != _MemCmpRing(pbGoldenBufAddr, pbGoldenBufAddr,
        pbGoldenBufAddr + u4GoldenFileSize, rCssAvParam.u4DstStartAddr,
        rCssAvParam.u4DstBufStart, rCssAvParam.u4DstBufEnd, u4GoldenFileSize))
    {
        Printf("CSS Disk/Title/AV Dec Fail \n");
        while(1);
    }
    else
    {
        Printf("CSS Disk/Title/AV Dec Pass \n");
    }

    VERIFY(BSP_FreeAlignedDmaMemory(pbSrcBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    return 0;
}

BOOL GCPU_EMU_CSSAUTHDRV()
{
    INT32 i;
    UINT8 au1Res[5];
    CSS_AUTH_PARAM_T rParam;

    rParam.au1CD[0] = 0x00;
    rParam.au1CD[1] = 0x01;
    rParam.au1CD[2] = 0x02;
    rParam.au1CD[3] = 0x03;
    rParam.au1CD[4] = 0x04;
    rParam.au1CD[5] = 0x05;
    rParam.au1CD[6] = 0x06;
    rParam.au1CD[7] = 0x07;
    rParam.au1CD[8] = 0x08;
    rParam.au1CD[9] = 0x09;

    rParam.u4ACC = 0x0;

    GCPU_Cmd(0, GCPU_CSS_AUTH_DRV, &rParam);

    //Check Result
    au1Res[0] = 0x1D;
    au1Res[1] = 0x84;
    au1Res[2] = 0x6F;
    au1Res[3] = 0xED;
    au1Res[4] = 0xE3;

    for (i=0; i<5; i++)
    {
        if (rParam.au1RD[i] != au1Res[i])
        {
            Printf("CSS Auth Drv Fail \n");
            while (1);
        }
    }
    Printf("CSS Auth Drv Pass \n");

    return 0;
}


INT32 GCPU_EMU_CSSAUTHDEC()
{
    INT32 i;
    UINT8 au1Res[5];
    CSS_AUTH_PARAM_T rParam;

    rParam.au1CD[0] = 0xea;
    rParam.au1CD[1] = 0xa2;
    rParam.au1CD[2] = 0x4e;
    rParam.au1CD[3] = 0xe4;
    rParam.au1CD[4] = 0xe4;
    rParam.au1CD[5] = 0x68;
    rParam.au1CD[6] = 0xa3;
    rParam.au1CD[7] = 0x73;
    rParam.au1CD[8] = 0x73;
    rParam.au1CD[9] = 0x5c;

    rParam.u4ACC = 0x0;

    GCPU_Cmd(0, GCPU_CSS_AUTH_DEC, &rParam);

    //Check Result
    au1Res[0] = 0x92;
    au1Res[1] = 0x20;
    au1Res[2] = 0xE7;
    au1Res[3] = 0xC4;
    au1Res[4] = 0x0F;

    for (i=0; i<5; i++)
    {
        if (rParam.au1RD[i] != au1Res[i])
        {
            Printf("CSS Auth Dec Fail \n");
            while (1);
        }
    }
    Printf("CSS Auth Dec Pass \n");

    return 0;
}


INT32 GCPU_EMU_CSSAUTHBK()
{
    INT32 i;
    UINT8 au1Res[5];
    CSS_AUTH_PARAM_T rParam;

    rParam.au1CD[0] = 0x92;
    rParam.au1CD[1] = 0x20;
    rParam.au1CD[2] = 0xe7;
    rParam.au1CD[3] = 0xc4;
    rParam.au1CD[4] = 0x0f;
    rParam.au1CD[5] = 0x1d;
    rParam.au1CD[6] = 0x84;
    rParam.au1CD[7] = 0x6f;
    rParam.au1CD[8] = 0xed;
    rParam.au1CD[9] = 0xe3;

    rParam.u4ACC = 0x0;

    GCPU_Cmd(0, GCPU_CSS_AUTH_BK, &rParam);

    //Check Result
    au1Res[0] = 0x5F;
    au1Res[1] = 0x9E;
    au1Res[2] = 0x78;
    au1Res[3] = 0x48;
    au1Res[4] = 0xEB;

    for (i=0; i<5; i++)
    {
        if (rParam.au1RD[i] != au1Res[i])
        {
            Printf("CSS Auth Bk Fail \n");
            while (1);
        }
    }
    Printf("CSS Auth Bk Pass \n");

    return 0;
}
