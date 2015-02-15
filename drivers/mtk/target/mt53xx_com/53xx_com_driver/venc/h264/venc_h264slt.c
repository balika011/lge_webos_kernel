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
 * $RCSfile: venc_h264slt.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file venc_h264slt.c
 *  Implementation of System Level Test of H.264.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "venc_drvif.h"
#ifdef __MODEL_slt__
#include "venc_util_drvif.h"
#include "venc_h264api.h"
#include "venc_h264hw.h"
#include "venc_h264_reg.h"
#include "x_fm.h"

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


//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static function forward declarations
//-----------------------------------------------------------------------------

static INT32 _VEncCmdSltLoadFb(UINT32 u4Idx, UINT32 u4YAddr, UINT32 u4CAddr);
static VOID _VEncSltSendDataThreadAsync(VOID *pvArg);


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

VENC_BUFMGR_DECLARE(VENC_FRM, VENC_FRM_T, 10);
static BOOL _fgVEncSltPass;
static UINT32 _u4VEncSltFrmCnt;
static HANDLE_T hVEncSltDoneSema, hThread;
static VENC_BUFMGR_BASE_T *_ptVEncSltFrmBuffMgr;
static VENC_HANDLE _hVEncSltVenc;


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

static INT32 _VEncCmdSltLoadFb(UINT32 u4Idx, UINT32 u4YAddr, UINT32 u4CAddr)
{
    CHAR acFn[64];
    HANDLE_T hFp;
    UINT32 u4Read, u4FrmSz = 1280 * 720;

    //LOG(0, "%s(%d, 0x%08x, 0x%08x)\n", __FUNCTION__, u4Idx, u4YAddr, u4CAddr);

    u4YAddr = VIRTUAL(u4YAddr);
    u4CAddr = VIRTUAL(u4CAddr);

    // load luma
    x_sprintf(acFn, "//mnt//usb_0//venc_slt//Night_1280x720_150_%08d_y.bin", u4Idx);
    if (FMR_OK != x_fm_open(FM_ROOT_HANDLE, acFn, FM_READ_ONLY, 0777, 0, &hFp))
    {
        LOG(0, "Fail to open %s\n", acFn);
        return -1;
    }

    if (FMR_OK != x_fm_read(hFp, (VOID *)u4YAddr, u4FrmSz, &u4Read))
    {
        LOG(0, "x_fm_read %d=>%d NG\n", u4FrmSz, u4Read);
        return -2;
    }
    HalFlushInvalidateDCacheMultipleLine(u4YAddr, u4FrmSz);
    x_fm_close(hFp);

    // load chroma
    x_sprintf(acFn, "//mnt//usb_0//venc_slt//Night_1280x720_150_%08d_c.bin", u4Idx);
    if (FMR_OK != x_fm_open(FM_ROOT_HANDLE, acFn, FM_READ_ONLY, 0777, 0, &hFp))
    {
        LOG(0, "Fail to open %s\n", acFn);
        return -3;
    }

    u4FrmSz /= 2;
    if (FMR_OK != x_fm_read(hFp, (VOID *)u4CAddr, u4FrmSz, &u4Read))
    {
        LOG(0, "x_fm_read %d=>%d NG\n", u4FrmSz, u4Read);
        return -4;
    }
    HalFlushInvalidateDCacheMultipleLine(u4CAddr, u4FrmSz);
    x_fm_close(hFp);

    return 0;
}


static VOID _VEncSltSendDataThreadAsync(VOID *pvArg)
{
    UINT32 u4FrmIdx;
    VENC_FRM_T rFrm;
    VENC_IN_PIC_T rIn;
    INT32  i4Ret = 0;

    x_memset(&rIn, 0, sizeof(rIn));
    for (u4FrmIdx = 0; u4FrmIdx < 6; ++u4FrmIdx)
    {
        _BUFMGR_OBJ_Get(_ptVEncSltFrmBuffMgr, (VOID *)&rFrm, VENC_BUFMGR_WAIT);
        
        rIn.u4YAddr = rFrm.u4YAddr;
        rIn.u4CAddr = rFrm.u4UVAddr;

        i4Ret = _VEncCmdSltLoadFb(u4FrmIdx, rIn.u4YAddr, rIn.u4CAddr);
        if (0 != i4Ret)
        {
            LOG(0, "get enc source data fail\n");
            _fgVEncSltPass = FALSE;
            VERIFY(x_sema_unlock(hVEncSltDoneSema) == OSR_OK);
            break;
        }

        if (VENC_OK != VENC_EncFrameAsync(_hVEncSltVenc, &rIn, NULL))
        {
            LOG(0, "enc frame fail\n");
            break;
        }
    }

    UNUSED(pvArg);
    
    x_thread_exit();
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

/*
golden crc
[VENC]CRC info dump 0
[VENC]   FRAME:0xe2a93e64
[VENC] FCS_cur:0x80b4ea68
[VENC] FCS_ref:0xffffffff
[VENC]DPCM_cur:0x42eb93a1
[VENC] IME_cur:0x6d5cff6f
[VENC] IME_ref:0xffffffff
[VENC]CRC info dump 1
[VENC]   FRAME:0x8392b1de
[VENC] FCS_cur:0x5af49ce9
[VENC] FCS_ref:0x80b4ea68
[VENC]DPCM_cur:0xd08b6b0f
[VENC] IME_cur:0x7e49f8e9
[VENC] IME_ref:0xf5d2b933
[VENC]CRC info dump 2
[VENC]   FRAME:0x0238ce33
[VENC] FCS_cur:0x79c832a7
[VENC] FCS_ref:0x80b4ea68
[VENC]DPCM_cur:0x67052a7e
[VENC] IME_cur:0xd3bde7c4
[VENC] IME_ref:0xf5d2b933
[VENC]CRC info dump 3
[VENC]   FRAME:0x0e75a7ab
[VENC] FCS_cur:0xf40148a1
[VENC] FCS_ref:0x80b4ea68
[VENC]DPCM_cur:0x3e223be6
[VENC] IME_cur:0x2c3eb02d
[VENC] IME_ref:0xa13496d7
[VENC]CRC info dump 4
[VENC]   FRAME:0x2f307fbc
[VENC] FCS_cur:0x996ec468
[VENC] FCS_ref:0x16882870
[VENC]DPCM_cur:0x22418814
[VENC] IME_cur:0xaf267e4a
[VENC] IME_ref:0x6ee95004

DTV>[VENC]CRC info dump 5
[VENC]   FRAME:0xde62c5d5
[VENC] FCS_cur:0xd9a18b3e
[VENC] FCS_ref:0x16882870
[VENC]DPCM_cur:0x523c7189
[VENC] IME_cur:0x73e46658
[VENC] IME_ref:0x6ee95004
[VENC]VENCOK!!!
 */
VOID _VEncH264SltCb(VOID *pvArg, VENC_IN_PIC_T * ptIn, VENC_OUT_PIC_T * ptOut, UINT32 type)
{
    UINT32 crc_ec;
    UINT32 crc_fcs_cur;
    UINT32 crc_fcs_ref;
    UINT32 crc_dpcm;
    UINT32 crc_ime_cur;
    UINT32 crc_ime_ref;

    LOG(2, "0x%08x--0x%08x\n", ptOut->u4Addr, ptOut->u4Addr + ptOut->u4Size - 1);

    // I0 B1 B2 P3 B4 B5
    if (!_fgIsRealPic(type)) 
    {
        return;
    }

    crc_ec      = IO_VENC_H264_READ_REG(CRC_EC_BITSTREAM);
    crc_fcs_cur = IO_VENC_H264_READ_REG(CRC_FCS_CUR);
    crc_fcs_ref = IO_VENC_H264_READ_REG(CRC_FCS_REF);
    crc_dpcm    = IO_VENC_H264_READ_REG(CRC_DPCM_CMB);
    crc_ime_cur = IO_VENC_H264_READ_REG(CRC_IME_CUR);
    crc_ime_ref = IO_VENC_H264_READ_REG(CRC_IME_REF);

    LOG(1, "CRC info dump %d\n", _u4VEncSltFrmCnt);
    LOG(1, "   FRAME:0x%08x\n", crc_ec);
    LOG(1, " FCS_cur:0x%08x\n", crc_fcs_cur);
    LOG(1, " FCS_ref:0x%08x\n", crc_fcs_ref);
    LOG(1, "DPCM_cur:0x%08x\n", crc_dpcm);
    LOG(1, " IME_cur:0x%08x\n", crc_ime_cur);
    LOG(1, " IME_ref:0x%08x\n", crc_ime_ref);

    if (_u4VEncSltFrmCnt == 3) // P3
    {
        if (0x0e75a7ab != crc_ec)
        {
            _fgVEncSltPass = FALSE;
            LOG(0, "P3 CRC error: 0x%08x\n", crc_ec);
        }
    }
    else if (_u4VEncSltFrmCnt == 4) // B4
    {
        if (0x2f307fbc != crc_ec)
        {
            _fgVEncSltPass = FALSE;
            LOG(0, "B4 CRC error: 0x%08x\n", crc_ec);
        }
    }
    else if (_u4VEncSltFrmCnt == 5) // B5, end
    {
        VERIFY(x_sema_unlock(hVEncSltDoneSema) == OSR_OK);
    }
    _u4VEncSltFrmCnt++;

    UNUSED(pvArg);
    UNUSED(ptIn);
    UNUSED(ptOut);
}


UINT32 _VEncH264Slt(INT32 i4Argc, const CHAR ** szArgv)
{
    UINT32 u4Value;
    UINT32 u4Ret;

    LOG(0, "start venc slt\n");

    _fgVEncSltPass = TRUE;
    _u4VEncSltFrmCnt = 0;

    _hVEncSltVenc = VENC_Open(VENC_H264, VENC_PROFILE_SLT, FALSE);

    if (VENC_NULL_HANDLE == _hVEncSltVenc)
    {
        LOG(0, "Open venc fail!\n");
        return VENC_INV_HANDLE;
    }

    u4Value = 30;
    u4Ret = VENC_SetParam(_hVEncSltVenc, VENC_PRAM_FRAMERATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "Set parameter error!\n");
        return u4Ret;
    }

    u4Value = 1500000;
    u4Ret = VENC_SetParam(_hVEncSltVenc, VENC_PRAM_BITRATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "Set parameter error!\n");
        return u4Ret;
    }

    VENC_PRAM_RES_T res;
    res.u2Width = 1280;
    res.u2Height= 720;
    u4Ret = VENC_SetParam(_hVEncSltVenc, VENC_PRAM_RESOLUTION, (VOID*)&res);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "Set parameter error!\n");
        return u4Ret;
    }

    //PIX FORMATE
    VENC_CTRL_PIXFMT_T pix;
    pix.ePixFmt  = VENC_PIX_FMT_NV12;
    pix.u2Pitch   = 1280;
    u4Ret = VENC_SetCtrl(_hVEncSltVenc, VENC_CTRL_PIX_FMT, (VOID*)&pix);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "Set ctrl error!\n");
        return u4Ret;
    }

    //SYNC MODE
    u4Value = (UINT32)VENC_SMODE_ASYNC;
    u4Ret = VENC_SetCtrl(_hVEncSltVenc, VENC_CTRL_SYNC_MODE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "Set ctrl error!\n");
        return u4Ret;
    }

//set buffer
//Working Buffer
{
    UINT32 u4Size;
    VENC_FRM_T rFrm;
    UINT32 u4Idx, u4TmpAddr;
    FBM_POOL_T *prFbmPool;
    VENC_CTRL_BUFF_T buff;

    prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_VENC);

    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    ASSERT(prFbmPool->u4Size != 0);

    //raw data buffer
    _ptVEncSltFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_FRM); 
    ASSERT(_ptVEncSltFrmBuffMgr);

    _BUFMGR_OBJ_Reset(_ptVEncSltFrmBuffMgr);
    u4Size = 1280*736; //VENC_ALIGN_MASK(res.u2Width, 31) * VENC_ALIGN_MASK(res.u2Height, 31); //

    u4TmpAddr = VENC_ALIGN_MASK(prFbmPool->u4Addr, 63);
    for (u4Idx = 2; u4Idx < 8; u4Idx++)
    {
        rFrm.u4YAddr  = u4TmpAddr;
        u4TmpAddr    += u4Size;
        u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
        rFrm.u4UVAddr = u4TmpAddr;
        u4TmpAddr    += (u4Size >> 1);
        u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
        _BUFMGR_OBJ_Add(_ptVEncSltFrmBuffMgr, &rFrm);
    }

    buff.u4Addr  = VENC_ALIGN_MASK(u4TmpAddr, 63);
    buff.u4Size  = prFbmPool->u4Size - (u4TmpAddr - prFbmPool->u4Addr);

    u4Ret = VENC_SetCtrl(_hVEncSltVenc, VENC_CTRL_BUFF, (VOID*)&buff);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "Set ctrl buffer error!\n");
        return u4Ret;
    }
}

    VENC_CTRL_OUTCB_T rCb;
    rCb.pvData = NULL;
    rCb.cb        = _VEncH264SltCb;
    u4Ret = VENC_SetCtrl(_hVEncSltVenc, VENC_CTRL_OUT_CB, (VOID*)&rCb);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "Set ctrl error!\n");
        return u4Ret;
    }

    //Init
    u4Ret = VENC_Init(_hVEncSltVenc);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "Init fail!\n");
        return u4Ret;
    }

    //Run
    u4Ret = VENC_Run(_hVEncSltVenc);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "Run fail!\n");
        return u4Ret;
    }

    VERIFY(x_sema_create(&(hVEncSltDoneSema), X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);

    VERIFY(x_thread_create(&hThread,
        "SendDThread", VENC_STACK_SIZE, VENC_THREAD_PRIORITY,
        (x_os_thread_main_fct)_VEncSltSendDataThreadAsync, 0, NULL) == OSR_OK);

    /*setp 4 wait for encode finish*/
    VERIFY(x_sema_lock(hVEncSltDoneSema, X_SEMA_OPTION_WAIT) == OSR_OK);

    /*setp 5 release resouce*/
    VERIFY(x_sema_delete(hVEncSltDoneSema) == OSR_OK);

    LOG(0, "VENC%s!!!\n", _fgVEncSltPass? "OK" : "FAIL");

    //Stop
    u4Ret = VENC_Stop(_hVEncSltVenc);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "Stop fail!\n");
        return u4Ret;
    }

    //Close
    u4Ret = VENC_Close(_hVEncSltVenc);
    if (VENC_OK != u4Ret)
    {
        LOG(0, "Close fail!\n");
        return u4Ret;
    }

    _hVEncSltVenc = VENC_NULL_HANDLE;

    if (_ptVEncSltFrmBuffMgr)
    {
        VENC_BUFMGR_OBJ_DELETE(VENC_FRM,(VENC_BUFMGR_T(VENC_FRM) *)_ptVEncSltFrmBuffMgr);
        _ptVEncSltFrmBuffMgr = NULL;
    }

    LOG(0, "end venc slt\n");

    UNUSED(i4Argc);
    UNUSED(szArgv);

    return 0;
}
#endif


