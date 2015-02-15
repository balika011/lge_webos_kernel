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
 * $RCSfile: venc_h264_testsuit.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file venc_h264_testsuit.c
 *  this file contains function test.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "general.h"
#include "venc_drvif.h"

#ifdef VENC_EMU

#include "venc_h264api.h"
#include "venc_raven_h264_hal.h"
#include "venc_raven_h264_reg.h"
#include "x_drv_cli.h"
#include "fbm_drvif.h"
#include "venc_util_drvif.h"
#include "x_rand.h"
#include "x_stl_lib.h"
#include "x_os.h"
#ifdef USB_SUPPORT
#include "x_fm.h"
#endif

//-----------------------------------------------------------------------------
// Configurations for test case
//-----------------------------------------------------------------------------

#define FBM_CFG   //This define must be marked in in drv build.


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


typedef struct CfgFile
{
    char InputFileName[512];
    char GoldenFileName[512];
    char MMRFileName[512];
    char CRCFileName[512];
    char OutputFileName[512]; // target bitrate/framerate
    char KeyFrameFileName[512];
    BOOL fgSaveOutputFile;
}CFGFILE;


typedef struct _H264_EMU_HOST_T
{
    UINT32      u4ErrCnt;
    UINT32      u4FrmCnt;
    UINT32      u4FrmNo;
    VENC_HANDLE hVEnc;
    VENC_BUFMGR_BASE_T * ptFrmBuffMgr;
    UINT32      u4Pitch;
    UINT32      u4Width;
    UINT32      u4Height;
    HANDLE_T    hEmuDoneSema;
    CHAR        szFilePath[512];
    CHAR *      szFilePathPtr;
    H264_CTRL_T *ptCtrl;
    BOOL        fgFirst;

    //golden part
    UINT32      out_size;
    UINT32      u4GoldenBufAddr;

    //crc part
    UINT32      crc_frp;
    UINT32      u4CRCBufAddr;

    UINT32      crc_sps;
    UINT32      crc_pps;
    UINT32      crc_ec;
    UINT32      crc_fcs_cur;
    UINT32      crc_fcs_ref;
    UINT32      crc_dpcm;
    UINT32      crc_ime_cur;
    UINT32      crc_ime_ref;
    
    //insert keyframe
    UINT32      u4KeyInfoBufAddr;
    UINT32      keyframe_frp;

    //skype test
    UINT32      u4SkypeInfoBufAddr;
    UINT32      skype_frp;

    UINT8       u1VarEnd;

    //function start
    VOID       (*pfnSetEnv)(struct _H264_EMU_HOST_T *); 
    VOID       (*pfnSendDataThread)(VOID *); 
    //function end
} H264_EMU_HOST_T;

VENC_BUFMGR_DECLARE(VENC_FRM, VENC_FRM_T, 10);

//-----------------------------------------------------------------------------
// extern functions
//-----------------------------------------------------------------------------
extern CLI_EXEC_T * getVEncCmdTbl(VOID);
extern VOID getCFGPathfile(UCHAR *pBuf, CFGFILE *ptCfgFile);
extern CHAR getCFGFileList(UCHAR *pBuf, UINT32 *addr, char *u4Val);
extern VOID getMMRfile(CHAR *pBuf, UINT32 *addr, UINT32 *u4Val);
extern BOOL _VEncLoadFile(CHAR *pcFileName, UINT32 u4Addr);
#ifdef USB_SUPPORT
extern BOOL _VEncLoadUsbFile(CHAR *pcFileName, UINT32 u4Addr);
//extern INT32 _Mount(void);
EXTERN INT32 fm_init(VOID);
EXTERN INT32 x_fm_fat_init(VOID *pv_data);
//#define FMR_OK                  ((INT32)  0)    /**<Everything is OK        */
#endif
extern VOID _VEncCmdEmuList(const CHAR *pcFileList);
extern UINT32 StrToInt(const CHAR* pszStr);
extern VOID _H264HalDumpRegs(VOID);
extern VOID _VENC_H264_HalRtResetBps(VENC_OBJECT_T *this, UINT32 u4Bps);
extern VOID _VENC_H264_HalRtResetFps(VENC_OBJECT_T *this, UINT32 u4Fps);
extern VENC_REZ_OBJ_T *VencCreate68SwRezObject(void);
extern VOID VencDelete68SwRezObject(VENC_REZ_OBJ_T *pObj);


//-----------------------------------------------------------------------------
// test case declare
//-----------------------------------------------------------------------------
static INT32 _VEncEmuTestCase1(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _VEncEmuTestCase2(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _VEncEmuTestCase3(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _VEncEmuTestCase4(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _VEncEmuTestCase5(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _VEncEmuTestCase6(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _VEncEmuTestCase7(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _VEncEmuTestCase8(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _VEncEmuTestCase9(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _VEncEmuTestCase10(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _VEncEmuTestCase11(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _VEncEmuTestCase12(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _VEncEmuTestCase13(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _VEncEmuTestCase14(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _VEncEmuTestCase15(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _VEncEmuTestCase16(INT32 i4Argc, const CHAR ** szArgv);
static INT32 _VEncEmuTestCaseDbg(INT32 i4Argc, const CHAR ** szArgv);


//-----------------------------------------------------------------------------
// test case table
//-----------------------------------------------------------------------------
static CLI_EXEC_T _arH264TestCaseTbl[] =
{
    {"t1",    NULL,  _VEncEmuTestCase1,            NULL, "Function Test High Priority",            CLI_SUPERVISOR},
    {"t2",    NULL,  _VEncEmuTestCase2,            NULL, "Function Test Mediium Priority",         CLI_SUPERVISOR},
    {"t3",    NULL,  _VEncEmuTestCase3,            NULL, "Function Test Low Priority",             CLI_SUPERVISOR},
    {"t4",    NULL,  _VEncEmuTestCase4,            NULL, "Multi-SPS test",                         CLI_SUPERVISOR},
    {"t5",    NULL,  _VEncEmuTestCase5,            NULL, "Insert Keyframe test",                   CLI_SUPERVISOR},
    {"t6",    NULL,  _VEncEmuTestCase6,            NULL, "Block Mode(16x32) Test",                 CLI_SUPERVISOR},
    {"t7",    NULL,  _VEncEmuTestCase7,            NULL, "Block Mode(64x32) Test",                 CLI_SUPERVISOR},
    {"t8",    NULL,  _VEncEmuTestCase8,            NULL, "SPS VUI Test",                           CLI_SUPERVISOR},
    {"t9",    NULL,  _VEncEmuTestCase9,            NULL, "Read/Write Pointer Test",                CLI_SUPERVISOR},
    {"t10",   NULL,  _VEncEmuTestCase10,           NULL, "DRAM DLA Parameters Test",               CLI_SUPERVISOR},
    {"t11",   NULL,  _VEncEmuTestCase11,           NULL, "Read/Write Pointer Test2",               CLI_SUPERVISOR},
    {"t12",   NULL,  _VEncEmuTestCase12,           NULL, "Runtime function Test",                  CLI_SUPERVISOR},
    {"t13",   NULL,  _VEncEmuTestCase13,           NULL, "Performance Test",                  CLI_SUPERVISOR},
    {"t14",   NULL,  _VEncEmuTestCase14,           NULL, "Boundary Test",                  CLI_SUPERVISOR},
    {"t15",   NULL,  _VEncEmuTestCase15,           NULL, "Skype Test",                  CLI_SUPERVISOR},
    {"t16",   NULL,  _VEncEmuTestCase16,           NULL, "IMGRZ Test",                  CLI_SUPERVISOR},
    {"dbg",   NULL,  _VEncEmuTestCaseDbg,          NULL, "Debug Test",                             CLI_SUPERVISOR},
    {NULL, NULL, NULL, NULL, NULL, CLI_SUPERVISOR}
};

//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------

//default settings
static BOOL _fgCompareCRCDefault      = TRUE ;
static BOOL _fgCompareGoldenDefault   = TRUE;
static BOOL _fgInserKeyframeDefault   = FALSE;
static BOOL _fgNotAssertDefault       = FALSE;

HANDLE_T _hEncFrmStart = NULL_HANDLE;
volatile BOOL _fgRpWpThdRun;

BOOL _fgCompareCRC;
BOOL _fgCompareGolden;
BOOL _fgInserKeyframe;
BOOL _fgNotAssert;
BOOL _fgVUITest;
BOOL _fgSkypeTest;


H264_EMU_HOST_T rHost;
CFGFILE _rCfgFile;

static BOOL   _fgDlaTestOn;
static UINT32 _u4DlaWrAddrThrd;
static UINT32 _u4DlaRdAddrThrd;
static UINT32 _u4DlaWrDataThrd;
static UINT32 _u4DlaRdDataThrd;
static UINT32 _u4DlaWrTimeout;
static BOOL   _fgDlaBypass;

#ifdef VENC_CYCLE
static UINT32 _u4VEncH264NumMB;
static UINT32 _u4VEncI_IDRCycleCount;
static UINT32 _u4VEncPFrmCycleCount;
static UINT32 _u4VEncBFrmCycleCount;
static UINT32 _u4VEncI_IDRCount;
static UINT32 _u4VEncPFrmCount;
static UINT32 _u4VEncBFrmCount;

static UINT32 _u4ForcePinterskip_1;
static UINT32 _u4ForcePinter16x16_1;
static UINT32 _u4ForcePinter16x08_1;
static UINT32 _u4ForcePinter08x16_1;
static UINT32 _u4ForcePinter08x08_1;
static UINT32 _u4ForceBinterdirect_1;
static UINT32 _u4ForceBinter16x16_1;
static UINT32 _u4ForceBinter16x08_1;
static UINT32 _u4ForceBinter08x16_1;
static UINT32 _u4ForceBinter08x08_1;
#endif

#ifdef VENC_DRAM_PROFILE
static UINT32 _u4ForceMbaffOn;
static UINT32 _u4ForceNumOfBFrm; 
#endif

#if 0//def USB_SUPPORT
#define MAX_LENGTH_FILE_NAME 256

static CHAR _szMountPath[MAX_LENGTH_FILE_NAME];
static CHAR _szDevPath[MAX_LENGTH_FILE_NAME];
static BOOL _fgIsFMInit = FALSE;
#endif


//-----------------------------------------------------------------------------
// function declares
//-----------------------------------------------------------------------------
static BOOL _VEncEmuCmpGolden(UINT32 u4CpSa, UINT32 u4CpSize, H264_EMU_HOST_T *ptHost);
static BOOL _VEncH264EmuCmpCRC(H264_EMU_HOST_T *ptHost, UINT32 type);
static BOOL _VEncH264EmuGetKeyframeInfo(H264_EMU_HOST_T *ptHost);


//-----------------------------------------------------------------------------
// Macro 
//-----------------------------------------------------------------------------

#define _fgVUITestFinsih(a)  (_fgVUITest && ((a)->u4FrmCnt > 1))


//-----------------------------------------------------------------------------
// static  functions
//-----------------------------------------------------------------------------
static UINT32 _GetGoldenBuffer(void)
{
    static UINT32 _golden_buffer_init = 0;
    if (_golden_buffer_init == 0)
    {
        UINT32 u4Size = 0x4000000;
        // find a space to store golden data
        _golden_buffer_init = (UINT32)BSP_AllocAlignedDmaMemory(u4Size, 1024);
        ASSERT(_golden_buffer_init);
        HalInvalidateDCacheMultipleLine(VIRTUAL(_golden_buffer_init), u4Size);
    }
    return _golden_buffer_init;
}

UINT32 _GetCRCBuffer(void)
{
    static UINT32 _crc_buffer_init = 0;
    if (_crc_buffer_init == 0)
    {
        UINT32 u4Size = 0x400000;
        // find a space to store crc data
        _crc_buffer_init = (UINT32)BSP_AllocAlignedDmaMemory(u4Size, 1024);
        ASSERT(_crc_buffer_init);
        HalInvalidateDCacheMultipleLine(VIRTUAL(_crc_buffer_init), u4Size);
    }
    return _crc_buffer_init;
}

UINT32 _GetKeyBuffer(void)
{
    static UINT32 _key_buffer_init = 0;
    if (_key_buffer_init == 0)
    {
        UINT32 u4Size = 0x72000;
        // find a space to store keyframe data
        _key_buffer_init = (UINT32)BSP_AllocAlignedDmaMemory(u4Size, 1024);
        ASSERT(_key_buffer_init);
        HalInvalidateDCacheMultipleLine(VIRTUAL(_key_buffer_init), u4Size);
    }
    return _key_buffer_init;
}

UINT32 _GetMMRBuffer(void)
{
    static UINT32 _mmr_buffer_init = 0;
    if (_mmr_buffer_init == 0)
    {
        UINT32 u4Size = 0x2000;
        // find a space to store mmr data
        _mmr_buffer_init = (UINT32)BSP_AllocAlignedDmaMemory(u4Size, 32);
        ASSERT(_mmr_buffer_init);
        HalInvalidateDCacheMultipleLine(VIRTUAL(_mmr_buffer_init), u4Size);
    }
    return _mmr_buffer_init;
}

UINT32 _GetSkypeBuffer(void)
{
    static UINT32 _skype_buffer_init = 0;
    if (_skype_buffer_init == 0)
    {
        UINT32 u4Size = 0x50000;
        // find a space to store skype data
        _skype_buffer_init = (UINT32)BSP_AllocAlignedDmaMemory(u4Size, 32);
        ASSERT(_skype_buffer_init);
        HalInvalidateDCacheMultipleLine(VIRTUAL(_skype_buffer_init), u4Size);
    }
    return _skype_buffer_init;
}

static VOID _VEncEmuLoadConfig(CHAR *pBuf, H264_MMR_PARAM_T *ptMMR)
{
    UINT32 addr, u4Val;
    INT32 k;

    addr = 0;
    // MMR
    x_sscanf (pBuf+addr, "%x", &u4Val);
    //sscanf(pBuf+addr, "%x", &u4Val);

    //---- MMR inputs, GOP setting ----//
    getMMRfile(pBuf, &addr, &ptMMR->dwNumOfIandPFrames);
    //Printf("==dwNumOfIandPFrames %x\n", ptMMR->dwNumOfIandPFrames);

    // g_dwPeriodOfIFrames
    getMMRfile(pBuf, &addr, &ptMMR->mmr_period_i_frm_10);
    //Printf("== %x\n", ptMMR->mmr_period_i_frm_10);

    // g_dwPeriodOfIDRFrames
    getMMRfile(pBuf, &addr, &ptMMR->mmr_period_idr_frm_10);
    //Printf("== %x\n", ptMMR->mmr_period_idr_frm_10);

    // g_dwNumOfBFrames
    getMMRfile(pBuf, &addr, &ptMMR->mmr_num_of_b_frm_2);
    //Printf("== %x\n", ptMMR->mmr_num_of_b_frm_2);

    // g_dwBitstreamFormat
    getMMRfile(pBuf, &addr, &ptMMR->mmr_bitstream_format_1);
    //Printf("%x\n", ptMMR->mmr_bitstream_format_1);

    // g_dwH264Profile
    getMMRfile(pBuf, &addr, &ptMMR->mmr_h264_profile_8);
    //Printf("%x\n", ptMMR->mmr_h264_profile_8);

    // g_dwH264Level
    getMMRfile(pBuf, &addr, &ptMMR->mmr_h264_level_6);
    //Printf("%x\n", ptMMR->mmr_h264_level_6);

    // g_dwDRAMPageByteNumDiv256
    getMMRfile(pBuf, &addr, &ptMMR->mmr_dram_page_size_div256_7);
    //Printf("%x\n", ptMMR->mmr_dram_page_size_div256_7);

    // g_bGenBRecFrame
    getMMRfile(pBuf, &addr, &ptMMR->mmr_gen_b_rec_frm_1);
    //Printf("%x\n", ptMMR->mmr_gen_b_rec_frm_1);

    // g_bGenCopiedBFrames
    getMMRfile(pBuf, &addr, &ptMMR->mmr_use_copy_b_1);
    //Printf("%x\n", ptMMR->mmr_use_copy_b_1);

    // g_bUseMBAFF
    getMMRfile(pBuf, &addr, &ptMMR->mmr_use_mbaff_1);
    //Printf("%x\n", ptMMR->mmr_use_mbaff_1);

    // g_bUseCABAC
    getMMRfile(pBuf, &addr, &ptMMR->mmr_use_cabac_1);
    //Printf("%x\n", ptMMR->mmr_use_cabac_1);

    //---- MMR inputs, QP ----//
    // g_dwQuantIFrames
    getMMRfile(pBuf, &addr, &ptMMR->mmr_quant_i_frm_6);
    //Printf("%x\n", ptMMR->mmr_quant_i_frm_6);

    // g_dwQuantPFrames
    getMMRfile(pBuf, &addr, &ptMMR->mmr_quant_p_frm_6);
    //Printf("%x\n", ptMMR->mmr_quant_p_frm_6);

    // g_dwQuantBFrames
    getMMRfile(pBuf, &addr, &ptMMR->mmr_quant_b_frm_6);
    //Printf("%x\n", ptMMR->mmr_quant_b_frm_6);

    //---- MMR inputs, Inter Mode Selection ----//
    // g_bPFrmInterSKIP
    getMMRfile(pBuf, &addr, &ptMMR->mmr_en_p_interskip_1);
    //Printf("%x\n", ptMMR->mmr_en_p_interskip_1);

    // g_bPFrmInter16x16
    getMMRfile(pBuf, &addr, &ptMMR->mmr_en_p_inter16x16_1);
    //Printf("%x\n", ptMMR->mmr_en_p_inter16x16_1);

    // g_bPFrmInter16x08
    getMMRfile(pBuf, &addr, &ptMMR->mmr_en_p_inter16x08_1);
    //Printf("%x\n", ptMMR->mmr_en_p_inter16x08_1);

    // g_bPFrmInter08x16
    getMMRfile(pBuf, &addr, &ptMMR->mmr_en_p_inter08x16_1);
    //Printf("%x\n", ptMMR->mmr_en_p_inter08x16_1);

    // g_bPFrmInter08x08
    getMMRfile(pBuf, &addr, &ptMMR->mmr_en_p_inter08x08_1);
    //Printf("%x\n", ptMMR->mmr_en_p_inter08x08_1);

    // g_bBFrmInterDIRECT
    getMMRfile(pBuf, &addr, &ptMMR->mmr_en_b_interdirect_1);
    //Printf("%x\n", ptMMR->mmr_en_b_interdirect_1);

    // g_bBFrmInter16x16
    getMMRfile(pBuf, &addr, &ptMMR->mmr_en_b_inter16x16_1);
    //Printf("%x\n", ptMMR->mmr_en_b_inter16x16_1);

    // g_bBFrmInter16x08
    getMMRfile(pBuf, &addr, &ptMMR->mmr_en_b_inter16x08_1);
    //Printf("%x\n", ptMMR->mmr_en_b_inter16x08_1);

    // g_bBFrmInter08x16
    getMMRfile(pBuf, &addr, &ptMMR->mmr_en_b_inter08x16_1);
    //Printf("%x\n", ptMMR->mmr_en_b_inter08x16_1);

    // g_bBFrmInter08x08
    getMMRfile(pBuf, &addr, &ptMMR->mmr_en_b_inter08x08_1);
    //Printf("%x\n", ptMMR->mmr_en_b_inter08x08_1);

    //---- MMR inputs, Searching Range, Coarse Search ----//
    // g_dwPFrameSearchRangeH_div16
    getMMRfile(pBuf, &addr, &ptMMR->mmr_p_sr_h_div16_3);
    //Printf("%x\n", ptMMR->mmr_p_sr_h_div16_3);

    // g_dwPFrameSearchRangeV_div16
    getMMRfile(pBuf, &addr, &ptMMR->mmr_p_sr_v_div16_2);
    //Printf("%x\n", ptMMR->mmr_p_sr_v_div16_2);

    // g_dwBFrameSearchRangeH_div16
    getMMRfile(pBuf, &addr, &ptMMR->mmr_b_sr_h_div16_2);
    //Printf("%x\n", ptMMR->mmr_b_sr_h_div16_2);

    // g_dwBFrameSearchRangeV_div16
    getMMRfile(pBuf, &addr, &ptMMR->mmr_b_sr_v_div16_2);
    //Printf("%x\n", ptMMR->mmr_b_sr_v_div16_2);

    // g_bCoarseSearch
    getMMRfile(pBuf, &addr, &ptMMR->mmr_use_coarse_search_1);
    //Printf("%x\n", ptMMR->mmr_use_coarse_search_1);

    // g_dwPFrameCoarseSearchRangeH_div8
    getMMRfile(pBuf, &addr, &ptMMR->mmr_p_coarse_sr_h_div8_3);
    //Printf("%x\n", ptMMR->mmr_p_coarse_sr_h_div8_3);

    //g_dwPFrameCoarseSearchRangeV_div8
    getMMRfile(pBuf, &addr, &ptMMR->mmr_p_coarse_sr_v_div8_2);
    //Printf("%x\n", ptMMR->mmr_p_coarse_sr_v_div8_2);

    // g_dwBFrameCoarseSearchRangeH_div8
    getMMRfile(pBuf, &addr, &ptMMR->mmr_b_coarse_sr_h_div8_2);
    //Printf("%x\n", ptMMR->mmr_b_coarse_sr_h_div8_2);

    // g_dwBFrameCoarseSearchRangeV_div8
    getMMRfile(pBuf, &addr, &ptMMR->mmr_b_coarse_sr_v_div8_2);
    //Printf("%x\n", ptMMR->mmr_b_coarse_sr_v_div8_2);


    //---- MMR inputs, Rate Control ----//
    // bUseRateControl
    getMMRfile(pBuf, &addr, &ptMMR->mmr_use_rc_1);
    //Printf("mmr_use_rc_1 %x\n", ptMMR->mmr_use_rc_1);

    // g_bUseCBR
    getMMRfile(pBuf, &addr, &ptMMR->mmr_rc_use_cbr_1);
    //Printf("%x\n", ptMMR->mmr_rc_use_cbr_1);

    // g_bUseInitialQP
    getMMRfile(pBuf, &addr, &ptMMR->mmr_rc_use_init_qp_1);
    //Printf("%x\n", ptMMR->mmr_rc_use_init_qp_1);

    // g_dwTargetBitrateInKbps
    getMMRfile(pBuf, &addr, &ptMMR->mmr_rc_target_bit_rate_14);
    //Printf("%x\n", ptMMR->mmr_rc_target_bit_rate_14);

#ifndef VENC_MT8580_PATTERN
    ptMMR->use_stuff_byte = ptMMR->mmr_use_rc_1;
#else
    // g_dwAdaptiveFPS
    getMMRfile(pBuf, &addr, &ptMMR->mmr_rc_adaptive_fps);
    //Printf("mmr_rc_adaptive_fps %x\n", ptMMR->mmr_rc_adaptive_fps);

    // g_dwAdaptiveTBR
    getMMRfile(pBuf, &addr, &ptMMR->mmr_rc_adaptive_tbr);
    //Printf("mmr_rc_adaptive_tbr %x\n", ptMMR->mmr_rc_adaptive_tbr);

    // g_rc_stuffing_en
    getMMRfile(pBuf, &addr, &ptMMR->use_stuff_byte);
    //Printf("use_stuff_byte %x\n", ptMMR->use_stuff_byte);
#endif

    //---- MMR inputs, fps ----//
    // g_dwfps
    getMMRfile(pBuf, &addr, &ptMMR->mmr_fps_8);
    //Printf("mmr_fps_8 %x\n", ptMMR->mmr_fps_8);

    // g_PfrmQLimiter
    getMMRfile(pBuf, &addr, &ptMMR->mmr_p_frm_q_limiter_8);
    //Printf("%x\n", ptMMR->mmr_p_frm_q_limiter_8);

    // g_BfrmQLimiter
    getMMRfile(pBuf, &addr, &ptMMR->mmr_b_frm_q_limiter_8);
    //Printf("%x\n", ptMMR->mmr_b_frm_q_limiter_8);


    //---- MMR inputs, Image Info. ----//
    // g_dwImageCropRight
    getMMRfile(pBuf, &addr, &ptMMR->mmr_image_crop_right_4);
    //Printf("%x\n", ptMMR->mmr_image_crop_right_4);

    // g_dwImageCropBottom
    getMMRfile(pBuf, &addr, &ptMMR->mmr_image_crop_bottom_5);
    //Printf("%x\n", ptMMR->mmr_image_crop_bottom_5);


    //---- MMR source scan Info. ----//
    // g_dwSourceScanType
    getMMRfile(pBuf, &addr, &ptMMR->mmr_source_scan_type_2);
    //Printf("%x\n", ptMMR->mmr_source_scan_type_2);
    
    // g_dwPitchWidthInPels
    getMMRfile(pBuf, &addr, &ptMMR->mmr_pitch_width_in_pels);
    //Printf("%x\n", ptMMR->mmr_pitch_width_in_pels);


    // g_dwImageWidth_Div16
    getMMRfile(pBuf, &addr, &ptMMR->mmr_image_width_div16_7);
    //Printf("g_dwImageWidth_Div16 %x\n", ptMMR->mmr_image_width_div16_7);

    // g_dwImageHeight_Div16
    getMMRfile(pBuf, &addr, &ptMMR->mmr_image_height_div16_6);
    //Printf("g_dwImageHeight_Div16 %x\n", ptMMR->mmr_image_height_div16_6);

    // g_dwImageMbNum
    getMMRfile(pBuf, &addr, &ptMMR->mmr_num_mb_in_frm_12);
    //Printf("g_dwImageMbNum %x\n", ptMMR->mmr_num_mb_in_frm_12);
#ifdef VENC_CYCLE
    _u4VEncH264NumMB = ptMMR->mmr_num_mb_in_frm_12;
#endif

    // g_dwCoarseImageCropBottom
    getMMRfile(pBuf, &addr, &ptMMR->mmr_coarse_image_crop_bottom_3);
    //Printf("%x\n", ptMMR->mmr_coarse_image_crop_bottom_3);

    // g_dwCoarseImageWidth_Div8
    getMMRfile(pBuf, &addr, &ptMMR->mmr_coarse_image_width_div8_6);
    //Printf("%x\n", ptMMR->mmr_coarse_image_width_div8_6);

    // g_dwCoarseImageHeight_Div8;
    getMMRfile(pBuf, &addr, &ptMMR->mmr_coarse_image_height_div8_5);
    //Printf("%x\n", ptMMR->mmr_coarse_image_height_div8_5);

    // g_dwCoarseImageBlk8Num
    getMMRfile(pBuf, &addr, &ptMMR->mmr_num_blk_in_coarse_frm_10);
    //Printf("%x\n", ptMMR->mmr_num_blk_in_coarse_frm_10);

    //---- MMR CRC setting ----//
    //g_bCRC_Check_Enable
    getMMRfile(pBuf, &addr, &ptMMR->mmr_crc_check_enable_1);
    //Printf("%x\n", ptMMR->mmr_crc_check_enable_1);


    //---- MMR inputs, Bitstream Initial Addr ----//
    // g_dwInitBitstream_DRAM_ByteAddr_Div8
    getMMRfile(pBuf, &addr, &ptMMR->mmr_ini_bitstream_dram_byte_div8_26);
    //Printf("%x\n", ptMMR->mmr_ini_bitstream_dram_byte_div8_26);

    //---- MMR inputs, Bitstream Max Size ----//
    // g_dwMaxBitstream_buffer_size
    getMMRfile(pBuf, &addr, &ptMMR->mmr_max_bitstream_byte_size_29);
    //Printf("%x\n", ptMMR->mmr_max_bitstream_byte_size_29);

    //---- MMR inputs, PIC Bitstream Max Size ----//
    // g_dwMaxPicBitstreamByteCnt
    getMMRfile(pBuf, &addr, &ptMMR->mmr_max_pic_bitstream_byte_cnt_22);
    //Printf("%x\n", ptMMR->mmr_max_pic_bitstream_byte_cnt_22);

    //---- MMR inputs, Frame Y Buffer Addr ----//
    for (k=0; k<8; k++)
    {
        // g_adwFrameBufferY_DRAM_ByteAddr_Div8[k]
        getMMRfile(pBuf, &addr, &u4Val);
        //Printf("%x\n", ptMMR->mmr_frm_buf_y_dram_byte_addr_div8_26[k]);
    }

    //---- MMR inputs, Frame UV Buffer Addr ----//
    for (k=0; k<8; k++)
    {
        // g_adwFrameBufferUV_DRAM_ByteAddr_Div8[k]
        getMMRfile(pBuf, &addr, &u4Val);
        //Printf("%x\n", ptMMR->mmr_frm_buf_uv_dram_byte_addr_div8_26[k]);
    }

    //---- MMR inputs, Upper Info MB Addr ----//
    // g_dwUpperMbInfo_DRAM_ByteAddr_Div8
    getMMRfile(pBuf, &addr, &ptMMR->mmr_upper_mb_info_dram_byte_addr_div8_26);
    //Printf("%x\n", ptMMR->mmr_upper_mb_info_dram_byte_addr_div8_26);

    //---- MMR inputs, Col Info Addr ----//
    // g_dwColZeroFlag_DRAM_ByteAddr_Div8
    getMMRfile(pBuf, &addr, &ptMMR->mmr_col_info_dram_byte_addr_div8_26);
    //Printf("%x\n", ptMMR->mmr_col_info_dram_byte_addr_div8_26);

    //---- MMR inputs, Coarse Frame Buffer Addr ----//
    for (k=0; k<7; k++)
    {
        // g_adwCoarseFrameBufferY_DRAM_ByteAddr_Div8[k]
        getMMRfile(pBuf, &addr, &u4Val);
        //Printf("%x\n", ptMMR->mmr_coarse_frm_buf_y_dram_byte_addr_div8_26[k]);
    }

    // g_dwCoarseMultiplier
    getMMRfile(pBuf, &addr, &ptMMR->mmr_coarse_multiplier_7);


    //g_EnableVUISupport
    getMMRfile(pBuf, &addr, &ptMMR->mmr_enable_vui_support_1);
    //Printf("%x\n", ptMMR->mmr_enable_vui_support_1);

    //g_VUI_aspect_ratio_info_present_flag
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_aspect_ratio_info_present_flag);
    //Printf("%x\n", ptMMR->mmr_vui_aspect_ratio_info_present_flag);
                
    //g_VUI_aspect_ratio_idc
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_aspect_ratio_idc);
    //Printf("%x\n", ptMMR->mmr_vui_aspect_ratio_idc);
                              
    //g_VUI_sar_width
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_sar_width);
    //Printf("%x\n", ptMMR->mmr_vui_sar_width);
                                     
    //g_VUI_sar_height
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_sar_height);
    //Printf("%x\n", ptMMR->mmr_vui_sar_height);
                                    
    //g_VUI_overscan_info_present_flag
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_overscan_info_present_flag);
    //Printf("%x\n", ptMMR->mmr_vui_overscan_info_present_flag);
                    
    //g_VUI_overscan_appropriate_flag
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_overscan_appropriate_flag);
    //Printf("%x\n", ptMMR->mmr_vui_overscan_appropriate_flag);
                     
    //g_VUI_video_signal_type_present_flag
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_video_signal_type_present_flag);
    //Printf("%x\n", ptMMR->mmr_vui_video_signal_type_present_flag);
                
    //g_VUI_video_format
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_video_format);
    //Printf("%x\n", ptMMR->mmr_vui_video_format);
                                  
    //g_VUI_video_full_range_flag
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_video_full_range_flag);
    //Printf("%x\n", ptMMR->mmr_vui_video_full_range_flag);
                         
    //g_VUI_colour_description_present_flag
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_colour_description_present_flag);
    //Printf("%x\n", ptMMR->mmr_vui_colour_description_present_flag);
               
    //g_VUI_colour_primaries
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_colour_primaries);
    //Printf("%x\n", ptMMR->mmr_vui_colour_primaries);
                              
    //g_VUI_transfer_characteristics
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_transfer_characteristics);
    //Printf("%x\n", ptMMR->mmr_vui_transfer_characteristics);
                      
    //g_VUI_matrix_coefficients
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_matrix_coefficients);
    //Printf("%x\n", ptMMR->mmr_vui_matrix_coefficients);
                           
    //g_VUI_chroma_location_info_present_flag
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_chroma_location_info_present_flag);
    //Printf("%x\n", ptMMR->mmr_vui_chroma_location_info_present_flag);
             
    //g_VUI_chroma_sample_loc_type_top_field
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_chroma_sample_loc_type_top_field);
    //Printf("%x\n", ptMMR->mmr_vui_chroma_sample_loc_type_top_field);
              
    //g_VUI_chroma_sample_loc_type_bottom_field
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_chroma_sample_loc_type_bottom_field);
    //Printf("%x\n", ptMMR->mmr_vui_chroma_sample_loc_type_bottom_field);
           
    //g_VUI_timing_info_present_flag
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_timing_info_present_flag);
    //Printf("%x\n", ptMMR->mmr_vui_timing_info_present_flag);
                      
    //g_VUI_num_units_in_tick
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_num_units_in_tick);
    //Printf("%x\n", ptMMR->mmr_vui_num_units_in_tick);
                             
    //g_VUI_time_scale
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_time_scale);
    //Printf("%x\n", ptMMR->mmr_vui_time_scale);
                                    
    //g_VUI_fixed_frame_rate_flag
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_fixed_frame_rate_flag);
    //Printf("%x\n", ptMMR->mmr_vui_fixed_frame_rate_flag);
                         
    //g_VUI_nal_hrd_parameters_present_flag
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_nal_hrd_parameters_present_flag);
    //Printf("%x\n", ptMMR->mmr_vui_nal_hrd_parameters_present_flag);
               
    //g_VUI_nal_bit_rate_scale
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_nal_bit_rate_scale);
    //Printf("%x\n", ptMMR->mmr_vui_nal_bit_rate_scale);
                            
    //g_VUI_nal_cpb_size_scale
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_nal_cpb_size_scale);
    //Printf("%x\n", ptMMR->mmr_vui_nal_cpb_size_scale);
                            
    //g_VUI_nal_bit_rate_value_minus1
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_nal_bit_rate_value_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_nal_bit_rate_value_minus1);
                     
    //g_VUI_nal_cpb_size_value_minus1
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_nal_cpb_size_value_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_nal_cpb_size_value_minus1);
                     
    //g_VUI_nal_vbr_cbr_flag
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_nal_vbr_cbr_flag);
    //Printf("%x\n", ptMMR->mmr_vui_nal_vbr_cbr_flag);
                              
    //g_VUI_nal_initial_cpb_removal_delay_length_minus1
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_nal_initial_cpb_removal_delay_length_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_nal_initial_cpb_removal_delay_length_minus1);

    //g_VUI_nal_cpb_removal_delay_length_minus1
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_nal_cpb_removal_delay_length_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_nal_cpb_removal_delay_length_minus1);
           
    //g_VUI_nal_dpb_output_delay_length_minus1
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_nal_dpb_output_delay_length_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_nal_dpb_output_delay_length_minus1);
            
    //g_VUI_nal_time_offset_length
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_nal_time_offset_length);
    //Printf("%x\n", ptMMR->mmr_vui_nal_time_offset_length);
                        
    //g_VUI_vcl_hrd_parameters_present_flag
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_hrd_parameters_present_flag);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_hrd_parameters_present_flag);
               
    //g_VUI_vcl_bit_rate_scale
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_bit_rate_scale);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_bit_rate_scale);
                            
    //g_VUI_vcl_cpb_size_scale
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_cpb_size_scale);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_cpb_size_scale);
                            
    //g_VUI_vcl_bit_rate_value_minus1
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_bit_rate_value_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_bit_rate_value_minus1);
                     
    //g_VUI_vcl_cpb_size_value_minus1
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_cpb_size_value_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_cpb_size_value_minus1);
                     
    //g_VUI_vcl_vbr_cbr_flag
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_vbr_cbr_flag);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_vbr_cbr_flag);
                              
    //g_VUI_vcl_initial_cpb_removal_delay_length_minus1
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_initial_cpb_removal_delay_length_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_initial_cpb_removal_delay_length_minus1);

    //g_VUI_vcl_cpb_removal_delay_length_minus1
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_cpb_removal_delay_length_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_cpb_removal_delay_length_minus1);
           
    //g_VUI_vcl_dpb_output_delay_length_minus1
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_dpb_output_delay_length_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_dpb_output_delay_length_minus1);
            
    //g_VUI_vcl_time_offset_length
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_time_offset_length);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_time_offset_length);
                        
    //g_VUI_low_delay_hrd_flag
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_low_delay_hrd_flag);
    //Printf("%x\n", ptMMR->mmr_vui_low_delay_hrd_flag);
                            
    //g_VUI_pic_struct_present_flag
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_pic_struct_present_flag);
    //Printf("%x\n", ptMMR->mmr_vui_pic_struct_present_flag);
                       
    //g_VUI_bitstream_restriction_flag
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_bitstream_restriction_flag);
    //Printf("%x\n", ptMMR->mmr_vui_bitstream_restriction_flag);
                    
    //g_VUI_motion_vectors_over_pic_boundaries_flag
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_motion_vectors_over_pic_boundaries_flag);
    //Printf("%x\n", ptMMR->mmr_vui_motion_vectors_over_pic_boundaries_flag);
       
    //g_VUI_max_bytes_per_pic_denom
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_max_bytes_per_pic_denom);
    //Printf("%x\n", ptMMR->mmr_vui_max_bytes_per_pic_denom);
                       
    //g_VUI_max_bits_per_mb_denom
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_max_bits_per_mb_denom);
    //Printf("%x\n", ptMMR->mmr_vui_max_bits_per_mb_denom);
                         
    //g_VUI_log2_max_mv_length_vertical
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_log2_max_mv_length_vertical);
    //Printf("%x\n", ptMMR->mmr_vui_log2_max_mv_length_vertical);
                   
    //g_VUI_log2_max_mv_length_horizontal
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_log2_max_mv_length_horizontal);
    //Printf("%x\n", ptMMR->mmr_vui_log2_max_mv_length_horizontal);
                 
    //g_VUI_num_reorder_frames
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_num_reorder_frames);
    //Printf("%x\n", ptMMR->mmr_vui_num_reorder_frames);
                            
    //g_VUI_max_dec_frame_buffering
    getMMRfile(pBuf, &addr, &ptMMR->mmr_vui_max_dec_frame_buffering);
    //Printf("%x\n", ptMMR->mmr_vui_max_dec_frame_buffering);


    // for 
    if (ptMMR->mmr_source_scan_type_2 == 0)
    {
        u4Val = (ptMMR->mmr_image_width_div16_7 << 4) - ptMMR->mmr_image_crop_right_4;;
        ptMMR->mmr_pitch_width_in_pels = VENC_ALIGN_MASK(u4Val,15);
    }

#ifdef VENC_CYCLE
    ptMMR->mmr_en_p_interskip_1 = _u4ForcePinterskip_1;
    ptMMR->mmr_en_p_inter16x16_1 = _u4ForcePinter16x16_1;
    ptMMR->mmr_en_p_inter16x08_1 = _u4ForcePinter16x08_1;
    ptMMR->mmr_en_p_inter08x16_1 = _u4ForcePinter08x16_1;
    ptMMR->mmr_en_p_inter08x08_1 = _u4ForcePinter08x08_1;
    ptMMR->mmr_en_b_interdirect_1 = _u4ForceBinterdirect_1;
    ptMMR->mmr_en_b_inter16x16_1 = _u4ForceBinter16x16_1;
    ptMMR->mmr_en_b_inter16x08_1 = _u4ForceBinter16x08_1;
    ptMMR->mmr_en_b_inter08x16_1 = _u4ForceBinter08x16_1;
    ptMMR->mmr_en_b_inter08x08_1 = _u4ForceBinter08x08_1;
#endif

#ifdef VENC_DRAM_PROFILE
    ptMMR->mmr_use_mbaff_1 = _u4ForceMbaffOn;
    u4Val = (ptMMR->mmr_num_of_b_frm_2 + 1) * ptMMR->dwNumOfIandPFrames;
    u4Val = u4Val / 6 * 6;
    ptMMR->dwNumOfIandPFrames = u4Val / (_u4ForceNumOfBFrm + 1); 
    ptMMR->mmr_num_of_b_frm_2 = _u4ForceNumOfBFrm; 
#endif
}

void _H264EmuSettings(VENC_OBJECT_T *this)
{
    H264_MMR_PARAM_T *pMMR;
    ASSERT(this);

    pMMR  = (H264_MMR_PARAM_T *)this->hParam;

    //set uvi
    if (pMMR->mmr_enable_vui_support_1)
    {
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_aspect_ratio_info_present_flag, pMMR->mmr_vui_aspect_ratio_info_present_flag); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_aspect_ratio_idc, pMMR->mmr_vui_aspect_ratio_idc); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_overscan_info_present_flag, pMMR->mmr_vui_overscan_info_present_flag); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_overscan_appropriate_flag, pMMR->mmr_vui_overscan_appropriate_flag); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_video_signal_type_present_flag, pMMR->mmr_vui_video_signal_type_present_flag); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_video_format, pMMR->mmr_vui_video_format); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_video_full_range_flag, pMMR->mmr_vui_video_full_range_flag); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_colour_description_present_flag, pMMR->mmr_vui_colour_description_present_flag); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_chroma_location_info_present_flag, pMMR->mmr_vui_chroma_location_info_present_flag); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_chroma_sample_loc_type_top_field, pMMR->mmr_vui_chroma_sample_loc_type_top_field); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_chroma_sample_loc_type_bottom_field, pMMR->mmr_vui_chroma_sample_loc_type_bottom_field); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_timing_info_present_flag, pMMR->mmr_vui_timing_info_present_flag); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_fixed_frame_rate_flag, pMMR->mmr_vui_fixed_frame_rate_flag); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_nal_hrd_parameters_present_flag, pMMR->mmr_vui_nal_hrd_parameters_present_flag); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_vcl_hrd_parameters_present_flag, pMMR->mmr_vui_vcl_hrd_parameters_present_flag); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_low_delay_hrd_flag, pMMR->mmr_vui_low_delay_hrd_flag); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_pic_struct_present_flag, pMMR->mmr_vui_pic_struct_present_flag); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_bitstream_restriction_flag, pMMR->mmr_vui_bitstream_restriction_flag); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_1, VUI_colour_primaries, pMMR->mmr_vui_colour_primaries);
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_1, VUI_transfer_characteristics, pMMR->mmr_vui_transfer_characteristics);
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_1, VUI_matrix_coefficients, pMMR->mmr_vui_matrix_coefficients);
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_2, VUI_sar_width, pMMR->mmr_vui_sar_width); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_2, VUI_sar_height, pMMR->mmr_vui_sar_height); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_3, VUI_num_units_in_tick, pMMR->mmr_vui_num_units_in_tick); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_4, VUI_time_scale, pMMR->mmr_vui_time_scale); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_5, VUI_nal_bit_rate_scale, pMMR->mmr_vui_nal_bit_rate_scale); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_5, VUI_nal_cpb_size_scale, pMMR->mmr_vui_nal_cpb_size_scale); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_5, VUI_nal_vbr_cbr_flag, pMMR->mmr_vui_nal_vbr_cbr_flag); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_5, VUI_nal_initial_cpb_removal_delay_length_minus1, pMMR->mmr_vui_nal_initial_cpb_removal_delay_length_minus1); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_5, VUI_nal_cpb_removal_delay_length_minus1, pMMR->mmr_vui_nal_cpb_removal_delay_length_minus1); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_5, VUI_nal_dpb_output_delay_length_minus1, pMMR->mmr_vui_nal_dpb_output_delay_length_minus1); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_5, VUI_nal_time_offset_length, pMMR->mmr_vui_nal_time_offset_length); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_6, VUI_nal_bit_rate_value_minus1, pMMR->mmr_vui_nal_bit_rate_value_minus1); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_6, VUI_nal_cpb_size_value_minus1, pMMR->mmr_vui_nal_cpb_size_value_minus1); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_7, VUI_vcl_bit_rate_scale, pMMR->mmr_vui_vcl_bit_rate_scale); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_7, VUI_vcl_cpb_size_scale, pMMR->mmr_vui_vcl_cpb_size_scale); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_7, VUI_vcl_vbr_cbr_flag, pMMR->mmr_vui_vcl_vbr_cbr_flag); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_7, VUI_vcl_initial_cpb_removal_delay_length_minus1, pMMR->mmr_vui_vcl_initial_cpb_removal_delay_length_minus1); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_7, VUI_vcl_cpb_removal_delay_length_minus1, pMMR->mmr_vui_vcl_cpb_removal_delay_length_minus1); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_7, VUI_vcl_dpb_output_delay_length_minus1, pMMR->mmr_vui_vcl_dpb_output_delay_length_minus1); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_7, VUI_vcl_time_offset_length, pMMR->mmr_vui_vcl_time_offset_length); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_8, VUI_vcl_bit_rate_value_minus1, pMMR->mmr_vui_vcl_bit_rate_value_minus1); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_8, VUI_vcl_cpb_size_value_minus1, pMMR->mmr_vui_vcl_cpb_size_value_minus1); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_9, VUI_motion_vectors_over_pic_boundaries_flag, pMMR->mmr_vui_motion_vectors_over_pic_boundaries_flag); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_9, VUI_max_bytes_per_pic_denom, pMMR->mmr_vui_max_bytes_per_pic_denom); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_9, VUI_max_bits_per_mb_denom, pMMR->mmr_vui_max_bits_per_mb_denom); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_9, VUI_log2_max_mv_length_vertical, pMMR->mmr_vui_log2_max_mv_length_vertical); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_9, VUI_log2_max_mv_length_horizontal, pMMR->mmr_vui_log2_max_mv_length_horizontal); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_9, VUI_num_reorder_frames, pMMR->mmr_vui_num_reorder_frames); 
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_9, VUI_max_dec_frame_buffering, pMMR->mmr_vui_max_dec_frame_buffering); 
    }    


    //DLA Settings
    if (_fgDlaTestOn)
    {
        if (_fgDlaBypass)
        {
            //Printf("Bypass\n");
            IO_VENC_H264_WRITE_BITS(DLA_BYPASS_MODE, Bypass_Mode, 1);
        }
        else
        {
            UINT32 u4Reg;
            //Printf("threshold(WrAddr, RdAddr, WrData, RdData) = (%d, %d, %d, %d), timeout = %d\n", 
            //    _u4DlaWrAddrThrd, _u4DlaRdAddrThrd, _u4DlaWrDataThrd, _u4DlaRdDataThrd, _u4DlaWrTimeout);
            u4Reg = (_u4DlaWrAddrThrd << 24)
                  | (_u4DlaRdAddrThrd << 16)
                  | (_u4DlaWrDataThrd << 8)
                  | _u4DlaRdDataThrd;
            IO_VENC_H264_WRITE_REG(DLA_WADDR_FIFO_THR, u4Reg);
            IO_VENC_H264_WRITE_BITS(DLA_TIME_OUT_W_THR, write_addr_fifo_time_out_threhold, _u4DlaWrTimeout);
            IO_VENC_H264_WRITE_BITS(DLA_BYPASS_MODE, Bypass_Mode, 0);
        }
    }    


}


VOID _VEncEmuInvalidateDCacheRing(UINT32 u4BufSa, UINT32 u4BufSz, UINT32 u4FlushSa, UINT32 u4FlushSz)
{
    UINT32 u4BufEa = u4BufSa + u4BufSz;
    UINT32 u4FlushEa = u4FlushSa + u4FlushSz;

    ASSERT(u4FlushSa >= u4BufSa);
    ASSERT(u4FlushSa < u4BufEa);

    if (u4FlushEa <= u4BufEa)
    {
        HalInvalidateDCacheMultipleLine(VIRTUAL(u4FlushSa), u4FlushSz);
    }
    else
    {
        UINT32 u4FlushSz2 = u4FlushEa - u4BufEa;
        HalInvalidateDCacheMultipleLine(VIRTUAL(u4BufSa), u4FlushSz2);
        HalInvalidateDCacheMultipleLine(VIRTUAL(u4FlushSa), u4FlushSz - u4FlushSz2);
    }
}

VOID _emu_hw_info(H264_EMU_HOST_T *ptHost, UINT32 type)
{
    ASSERT(ptHost);
    // CRC Part
    if (_fgIsRealPic(type))
    {
        ptHost->crc_sps     = ptHost->ptCtrl->u4SPScrc;
        ptHost->crc_pps     = ptHost->ptCtrl->u4PPScrc;
        ptHost->crc_ec      = IO_VENC_H264_READ_REG(CRC_EC_BITSTREAM);
        ptHost->crc_fcs_cur = IO_VENC_H264_READ_REG(CRC_FCS_CUR);
        ptHost->crc_fcs_ref = IO_VENC_H264_READ_REG(CRC_FCS_REF);
        ptHost->crc_dpcm    = IO_VENC_H264_READ_REG(CRC_DPCM_CMB);
        ptHost->crc_ime_cur = IO_VENC_H264_READ_REG(CRC_IME_CUR);
        ptHost->crc_ime_ref = IO_VENC_H264_READ_REG(CRC_IME_REF);
    }

}


VOID _emu_demo_cb(VOID *pvArg, VENC_IN_PIC_T * ptIn, VENC_OUT_PIC_T * ptOut, UINT32 type)
{
    H264_EMU_HOST_T *ptHost = (H264_EMU_HOST_T *)pvArg;

#ifndef VENC_LIGHT_LOG
    if (ptOut)
    {
        Printf("_emu_demo_cb out: (type, addr, size) => (%d, 0x%08x, 0x%08x)\n", type, ptOut->u4Addr, ptOut->u4Size);
    }
#endif

    if (type >= (UINT32)VENC_H264_PIC_TYPE_SPS && type <= (UINT32)VENC_H264_PIC_TYPE_B)
    {
        UINT32 compare_error = 0;
        //debug info
        _emu_hw_info(ptHost, type);
        
        if (_fgIsRealPic(type))
        {
#ifndef VENC_LIGHT_LOG
            Printf("idx: %d\n", ptHost->u4FrmCnt);
#endif
            ptHost->u4FrmCnt++;
            //encode done
        }

        do 
        {
            if (_fgVUITestFinsih(ptHost))
            {
                break;
            }

            if (_fgCompareCRC)
            {
                if (_fgIsRealPic(type))
                {
                    Printf("Compare CRC......\n");
                    if (!_VEncH264EmuCmpCRC(ptHost, type))
                    {
                        compare_error ++;
                    }
                }
            }

            if (_fgCompareGolden)
            {
                BOOL fgRet1,fgRet2;

                Printf("Compare golden......\n");

                fgRet1 = fgRet2 = TRUE;
                _VEncEmuInvalidateDCacheRing(ptHost->ptCtrl->u4BsSa, ptHost->ptCtrl->u4BsSz, ptOut->u4Addr, ptOut->u4Size);
                fgRet1 = _VEncEmuCmpGolden(ptOut->u4Addr, ptOut->u4Size, ptHost);
            
                if (_fgIsRealPic(type))
                {
                    UINT32 u4StuffBytes = _VENC_H264_HalStuffbytes();//IO_VENC_H264_READ_BITS(VIDEO_STUFF_REPORT, STUFF_BYTE);
                    if (u4StuffBytes)
                    {
                        UINT32 u4StuffSa = ptOut->u4Addr + VENC_ALIGN_MASK(ptOut->u4Size, 7);
            
                        //Printf("stuff_Sa/Sz = 0x%08x / 0x%08x\n", u4StuffSa, u4StuffBytes);
                        if (u4StuffSa >= ptHost->ptCtrl->u4BsEa)
                        {
                            u4StuffSa -= ptHost->ptCtrl->u4BsSz;
                            ASSERT(u4StuffSa < ptHost->ptCtrl->u4BsEa);
                            ASSERT(u4StuffSa >= ptHost->ptCtrl->u4BsSa);
                        }

                        _VEncEmuInvalidateDCacheRing(ptHost->ptCtrl->u4BsSa, ptHost->ptCtrl->u4BsSz, u4StuffSa, u4StuffBytes);

                        //_VENC_FlushInvalidateDCacheChkRing(prVEncInfo->rBufInfo.u4BsSa, prVEncInfo->rBufInfo.u4BsSz, u4StuffSa, u4StuffBytes);
                        fgRet2 = _VEncEmuCmpGolden(u4StuffSa, u4StuffBytes, ptHost);
                    }
                }

                if (!(fgRet1 && fgRet2))
                {
                    compare_error++;
                }
            }
        }while (0);

        if (compare_error)
        {
            ptHost->u4ErrCnt++; 
        }

#ifdef VENC_CYCLE
        if (VENC_H264_PIC_TYPE_IDR == type || VENC_H264_PIC_TYPE_I == type)
        {
            _u4VEncI_IDRCycleCount += _VENC_H264_u4EncCycleCount();
            _u4VEncI_IDRCount++;
        }
        else if (VENC_H264_PIC_TYPE_P == type)
        {
            _u4VEncPFrmCycleCount += _VENC_H264_u4EncCycleCount();
            _u4VEncPFrmCount++;
        }
        else if (VENC_H264_PIC_TYPE_B == type)
        {
            _u4VEncBFrmCycleCount += _VENC_H264_u4EncCycleCount();
            _u4VEncBFrmCount++;
        }
#endif

        if (ptHost->u4FrmCnt == ptHost->u4FrmNo)
        {
            VERIFY(x_sema_unlock(ptHost->hEmuDoneSema) == OSR_OK);
        }
    }

    if (ptIn)
    {
        VENC_FRM_T rFrm;
        rFrm.u4YAddr  = ptIn->u4YAddr;
        rFrm.u4UVAddr = ptIn->u4CAddr;
        _BUFMGR_OBJ_Free(ptHost->ptFrmBuffMgr, (VOID *)&rFrm);
#ifndef VENC_LIGHT_LOG
        Printf("_emu_demo_cb free: src(Y,UV) => (0x%08x, 0x%08x)\n", ptIn->u4YAddr, ptIn->u4CAddr);
#endif
    }
}


#if 0//def USB_SUPPORT
static INT32 _Mount(void)
{
    INT32 i4_ret;
    UINT32 ui4_part_ns;

    UINT32 ui4_cnt;

    /*  1. dev point creation */

    i4_ret = x_fm_create_dir(FM_ROOT_HANDLE, "/dev", 0666);

    if (i4_ret == FMR_OK)
    {
        printf("/dev is created.\n");
    }
    else if (i4_ret != FMR_OK)
    {
        if (i4_ret == FMR_EXIST)
        {
            printf("/dev is already existed.  That is OK.\n");
        }
        else
        {
            printf("make /dev fail\n");
            printf("return value:%d\n", i4_ret);
            goto func_exit;
        }
    }

    /*  2. mount point creation */

    i4_ret = x_fm_create_dir(FM_ROOT_HANDLE, "/mnt", 0666);

    if (i4_ret == FMR_OK)
    {
        printf("/mnt is created.\n");
    }
    else if (i4_ret != FMR_OK)
    {
        if (i4_ret == FMR_EXIST)
        {
            printf("/mnt is already existed.  That is OK.\n");
        }
        else
        {
            printf("make /mnt fail\n");
            printf("return value:%d\n", i4_ret);
            goto func_exit;
        }
    }

    /*  3. make entry for USB */

    i4_ret = x_fm_make_entry(FM_ROOT_HANDLE,
                             "/dev/usb",
                             FM_MODE_TYPE_BLK | FM_MODE_DEV_TRUE | 0660,
                             FM_DRVT_USB_MASS_STORAGE,
                             0);

    if (i4_ret == FMR_OK)
    {
        printf("/dev/usb is created.\n");
    }
    else if (i4_ret != FMR_OK)
    {
        if (i4_ret == FMR_EXIST)
        {
            printf("/dev/usb is already existed.  That is OK.\n");
        }
        else
        {
            printf("make /dev/usb fail\n");
            printf("return value:%d\n", i4_ret);
            goto func_exit;
        }
    }

    /*  4. parse drive  */

    i4_ret = x_fm_parse_drive(FM_ROOT_HANDLE,
                              "/dev/usb",
                              (VOID *) NULL);

    if (i4_ret != FMR_OK)
    {
        printf("parse drive error.");
    }

    /*  5. get partition ns */

    i4_ret = x_fm_get_part_ns(FM_ROOT_HANDLE,
                              (const CHAR *) "/dev/usb",
                              &ui4_part_ns);

    if (i4_ret != FMR_OK)
    {
        printf("x_fm_get_part_ns fails.\n");
        printf("return value:%d\n", i4_ret);
        goto func_exit;
    }

    for (ui4_cnt = 0; ui4_cnt < ui4_part_ns; ui4_cnt++)
    {
        FM_PART_INFO_T t_part_info;

        CHAR ps_dev_name[32];
        CHAR ps_mnt_name[32];

        i4_ret = x_fm_get_part_info(FM_ROOT_HANDLE,
                                    (const CHAR *) "/dev/usb",
                                    ui4_cnt,
                                    &t_part_info);

        if (i4_ret != FMR_OK)
        {
            printf("x_fm_get_part_info fails.\n");
            printf("return value:%d\n", i4_ret);
            goto func_exit;
        }

        x_strcpy((CHAR *) ps_dev_name, "/dev/");
        x_strcpy((CHAR *) ps_mnt_name, "/mnt/");

        x_strcat((CHAR *) ps_dev_name, (const CHAR *) t_part_info.ps_part_name);
        x_strcat((CHAR *) ps_mnt_name, (const CHAR *) t_part_info.ps_part_name);

        i4_ret = x_fm_create_dir(FM_ROOT_HANDLE, ps_mnt_name, 0666);

        if (i4_ret == FMR_OK)
        {
            printf("%s is created.\n", ps_mnt_name);
        }
        else if (i4_ret != FMR_OK)
        {
            if (i4_ret == FMR_EXIST)
            {
                printf("%s is already existed.  That is OK.\n", ps_mnt_name);
            }
            else
            {
                printf("make %s fail.\n", ps_mnt_name);
                printf("return value:%d\n", i4_ret);
                goto func_exit;
            }
        }

        i4_ret = x_fm_mount(FM_ROOT_HANDLE, ps_dev_name, FM_ROOT_HANDLE, ps_mnt_name);

        if (i4_ret == FMR_OK)
        {
            x_strncpy(_szMountPath, ps_mnt_name, x_strlen(ps_mnt_name));
            x_strncpy(_szDevPath, ps_dev_name, x_strlen(ps_dev_name));
            printf("%s is mounted as %s.\n", ps_dev_name, ps_mnt_name);
        }
        if (i4_ret != FMR_OK)
        {
            printf("ERR: mount %s as %s fail.\n", ps_dev_name, ps_mnt_name);
            printf("return value:%d\n", i4_ret);
            goto func_exit;
        }
    }

func_exit:

    return 0;
}
#endif


#ifdef SINGLE_FILE_SUPPORT
static VOID _VEncSwIntUV(UINT8 *pu1U, UINT8 *pu1V, UINT8 *pu1C, UINT32 u4Resolution)
{
    INT32 i, j;

    for (i = j = 0; i < u4Resolution; i++)
    {
        pu1C[j++] = pu1U[i];
        pu1C[j++] = pu1V[i];
    }
}
#endif


static VOID _SendDataThreadAsync(VOID *pvArg)
{
    UINT32 u4Res;
    UINT32 u4FrmIdx;
    H264_EMU_HOST_T *ptHost = (H264_EMU_HOST_T *)(*(UINT32 *)pvArg);
    VENC_FRM_T rFrm;
    VENC_IN_PIC_T rIn;
#ifdef SINGLE_FILE_SUPPORT
    HANDLE_T  h_file;
    UINT32 u4Request, u4Read;
    UINT8 *pu1U, *pu1V;
    CFGFILE *ptCfgFile = &_rCfgFile;
#endif

    ASSERT(ptHost);

    u4Res = ptHost->u4Height * ptHost->u4Pitch;//ptHost->u4Width;

    Printf("ENTER %s\n", __FUNCTION__);

#if 0//def USB_SUPPORT
    Printf("Init FM? %d\n", _fgIsFMInit);
    if (!_fgIsFMInit)
    {
        Printf("Init FM\n");
        if (fm_init() != FMR_OK)
        {
            Printf("\nError: Init FM fail\n");
        }
        if (x_fm_fat_init(NULL) != FMR_OK)
        {
            Printf("\nError: Init FAT fail\n");
        }

        Printf("Mount USB\n");
        VERIFY(_Mount() == 0);

        _fgIsFMInit = TRUE;
    }
#endif

#ifdef SINGLE_FILE_SUPPORT
    VERIFY(FMR_OK == x_fm_open(FM_ROOT_HANDLE, ptCfgFile->InputFileName, FM_READ_ONLY, 0777, FALSE, &h_file));
    pu1U = (UINT8 *)BSP_AllocAlignedDmaMemory(u4Res >> 2, 32);
    pu1V = (UINT8 *)BSP_AllocAlignedDmaMemory(u4Res >> 2, 32);
    ASSERT(pu1U);
    ASSERT(pu1V);
    HalInvalidateDCacheMultipleLine(VIRTUAL((UINT32)pu1U), u4Res >> 2);
    HalInvalidateDCacheMultipleLine(VIRTUAL((UINT32)pu1V), u4Res >> 2);
#endif

    x_memset(&rIn, 0, sizeof(rIn));
    for (u4FrmIdx = 0; u4FrmIdx < ptHost->u4FrmNo; ++u4FrmIdx)
    {
        _BUFMGR_OBJ_Get(ptHost->ptFrmBuffMgr, (VOID *)&rFrm, VENC_BUFMGR_WAIT);
        
        rIn.u4YAddr = rFrm.u4YAddr;
        rIn.u4CAddr = rFrm.u4UVAddr;

#ifdef SINGLE_FILE_SUPPORT
        u4Request = u4Res;
        x_fm_read(h_file, (VOID *)rIn.u4YAddr, u4Request, &u4Read);
        ASSERT(u4Request == u4Read);
#else

        x_sprintf(ptHost->szFilePathPtr, "_%08d.y", u4FrmIdx);
#ifdef USB_SUPPORT
        _VEncLoadUsbFile(ptHost->szFilePath, rIn.u4YAddr);
#else
        _VEncLoadFile(ptHost->szFilePath, rIn.u4YAddr);
#endif

#endif
        HalFlushDCacheMultipleLine(VIRTUAL(rIn.u4YAddr), u4Res);

#ifdef SINGLE_FILE_SUPPORT
        u4Request = u4Res >> 2;
        x_fm_read(h_file, (VOID *)pu1U, u4Request, &u4Read);
        ASSERT(u4Request == u4Read);
        x_fm_read(h_file, (VOID *)pu1V, u4Request, &u4Read);
        ASSERT(u4Request == u4Read);
        _VEncSwIntUV(pu1U, pu1V, (UINT8 *)rIn.u4CAddr, u4Request);
#else

        x_sprintf(ptHost->szFilePathPtr, "_%08d.c", u4FrmIdx);
#ifdef USB_SUPPORT
        _VEncLoadUsbFile(ptHost->szFilePath, rIn.u4CAddr);
#else
        _VEncLoadFile(ptHost->szFilePath, rIn.u4CAddr);
#endif

#endif
        HalFlushDCacheMultipleLine(VIRTUAL(rIn.u4CAddr), (u4Res>>1));
        
#ifndef VENC_LIGHT_LOG
        Printf("enc %d\n", u4FrmIdx);
#endif
        if (VENC_OK != VENC_EncFrameAsync(ptHost->hVEnc, &rIn, NULL))
        {
            Printf("enc frame fail\n");
            break;
        }
    }

#ifdef SINGLE_FILE_SUPPORT
    BSP_FreeAlignedDmaMemory((UINT32)pu1U);
    BSP_FreeAlignedDmaMemory((UINT32)pu1V);
    VERIFY(FMR_OK == x_fm_close(h_file));
#endif

    Printf("LEAVE %s\n", __FUNCTION__);

    x_thread_exit();
}


static VOID _SetEnvAsync(H264_EMU_HOST_T *ptHost)
{
    //callback function
    UINT32 u4Val;
    VENC_CTRL_OUTCB_T rCb;

    //SYNC MODE
    u4Val = (UINT32)VENC_SMODE_ASYNC;
    if (VENC_OK != VENC_SetCtrl(ptHost->hVEnc, VENC_CTRL_SYNC_MODE, (VOID*)&u4Val))
    {
        Printf("Set ctrl sync mode error!\n");
        return;
    }

    //Output Callback
    rCb.pvData = (VOID *)ptHost;
    rCb.cb     = _emu_demo_cb;

    if (VENC_OK != VENC_SetCtrl(ptHost->hVEnc, VENC_CTRL_OUT_CB, (VOID*)&rCb))
    {
        Printf("Set ctrl cb error!\n");
        return;
    }
}


static VOID _SendDataThreadSync(VOID *pvArg)
{
    UINT32 u4Res;
    UINT32 u4FrmIdx;
    H264_EMU_HOST_T *ptHost = (H264_EMU_HOST_T *)(*(UINT32 *)pvArg);
    VENC_FRM_T rFrm;
    VENC_IN_PIC_T rIn;
    VENC_OUT_PIC_T rOut;
    H264_EMU_OUT_PIC_T rEmuOut;

    ASSERT(ptHost);

    u4Res = ptHost->u4Height * ptHost->u4Width;

    Printf("ENTER _SendDataThreadSync\n");

    //Printf("(Width, Height) => (%d,%d)\n", ptHost->u4Width, ptHost->u4Height);
    x_memset(&rIn, 0, sizeof(rIn));
    for (u4FrmIdx = 0; u4FrmIdx < ptHost->u4FrmNo; ++u4FrmIdx)
    {
        _BUFMGR_OBJ_Get(ptHost->ptFrmBuffMgr, (VOID *)&rFrm, VENC_BUFMGR_WAIT);
        
        rIn.u4YAddr = rFrm.u4YAddr;
        rIn.u4CAddr = rFrm.u4UVAddr;

        x_sprintf(ptHost->szFilePathPtr, "_%08d.y", u4FrmIdx);
#ifdef USB_SUPPORT
        _VEncLoadUsbFile(ptHost->szFilePath, rIn.u4YAddr);
#else
        _VEncLoadFile(ptHost->szFilePath, rIn.u4YAddr);
#endif
        //Printf("Load Y:addr0x%8x, %s\n", rIn.u4YAddr, ptHost->szFilePath);
        HalFlushDCacheMultipleLine(VIRTUAL(rIn.u4YAddr), u4Res);

        x_sprintf(ptHost->szFilePathPtr, "_%08d.c", u4FrmIdx);
#ifdef USB_SUPPORT
        _VEncLoadUsbFile(ptHost->szFilePath, rIn.u4CAddr);
#else
        _VEncLoadFile(ptHost->szFilePath, rIn.u4CAddr);
#endif
        //Printf("Load C:addr0x%8x, %s\n", rIn.u4CAddr, ptHost->szFilePath);
        HalFlushDCacheMultipleLine(VIRTUAL(rIn.u4CAddr), (u4Res>>1));
        
        if (VENC_OK != VENC_EncFrameSync(ptHost->hVEnc, &rIn, (VENC_OUT_PIC_T*)&rEmuOut))
        {
            Printf("enc frame fail\n");
            break;
        }

        if ((rEmuOut.u4Type) & VENC_H264_PIC_TYPE_SPS)
        {
            rOut.u4Addr = rEmuOut.u4SPSAddr;
            rOut.u4Size = rEmuOut.u4SPSSize;
            _emu_demo_cb((VOID *)ptHost, NULL, &rOut, VENC_H264_PIC_TYPE_SPS); 
        }
        if ((rEmuOut.u4Type) & VENC_H264_PIC_TYPE_PPS)
        {
            rOut.u4Addr = rEmuOut.u4PPSAddr;
            rOut.u4Size = rEmuOut.u4PPSSize;
            _emu_demo_cb((VOID *)ptHost, NULL, &rOut, VENC_H264_PIC_TYPE_PPS); 
        }

        rOut.u4Addr = rEmuOut.u4FRMAddr;
        rOut.u4Size = rEmuOut.u4FRMSize;
        _emu_demo_cb((VOID *)ptHost, &rIn, &rOut, (rEmuOut.u4Type)&0xF0); 
        
    }

    Printf("LEAVE _SendDataThreadSync\n");

    x_thread_exit();
}


static VOID _SetEnvSync(H264_EMU_HOST_T *ptHost)
{
    UINT32 u4Val;

    ASSERT(ptHost);
    //SYNC MODE
    u4Val = (UINT32)VENC_SMODE_SYNC;
    if (VENC_OK != VENC_SetCtrl(ptHost->hVEnc, VENC_CTRL_SYNC_MODE, (VOID*)&u4Val))
    {
        Printf("Set ctrl sync mode error!\n");
        return;
    }
}


static VOID _VEncCmdEmuItem(UINT8 *pu1FPathTempBuf, CHAR *pcFListPathName)
{
    H264_MMR_PARAM_T tMMR;
    UINT32 BUF_MMR;
    UINT32 u4Value;
    VENC_PRAM_RES_T res;
    HANDLE_T hThread;
    H264_EMU_HOST_T *ptHost = &rHost;
    CFGFILE *ptCfgFile = &_rCfgFile;
    VENC_CTRL_PIXFMT_T pix;

    Printf("Loading File path\n%s...\n", pcFListPathName);
    _VEncLoadFile(pcFListPathName, (UINT32)pu1FPathTempBuf);
    getCFGPathfile(pu1FPathTempBuf, ptCfgFile);

    BUF_MMR = _GetMMRBuffer();
    _VEncLoadFile(ptCfgFile->MMRFileName, BUF_MMR);
    _VEncEmuLoadConfig((CHAR *)BUF_MMR, &tMMR);

    //OPEN VENC
    ptHost->hVEnc = VENC_Open(VENC_H264, VENC_PROFILE_EMU, FALSE);
    if (VENC_NULL_HANDLE == ptHost->hVEnc)
    {
        Printf("Open venc fail!\n");
        return;
    }

    //set emu mmr
    if (VENC_OK != VENC_SetParam(ptHost->hVEnc, H264_EMU_PARM, (VOID*)&tMMR))
    {
        Printf("Set emu mmr error!\n");
        return;
    }

    //bitrate.
    u4Value = tMMR.mmr_rc_target_bit_rate_14;  
    if (VENC_OK != VENC_SetParam(ptHost->hVEnc, VENC_PRAM_BITRATE, (VOID*)&u4Value))
    {
        Printf("Set parameter error!\n");
        return;
    }

    //30 frame per sec.
    u4Value = tMMR.mmr_fps_8;  
    if (VENC_OK != VENC_SetParam(ptHost->hVEnc, VENC_PRAM_FRAMERATE, (VOID*)&u4Value))
    {
        Printf("Set parameter error!\n");
        return;
    }

    //RESOLUTION
    res.u2Width  = (tMMR.mmr_image_width_div16_7 << 4) - tMMR.mmr_image_crop_right_4;;
    res.u2Height = (tMMR.mmr_image_height_div16_6 << 4) - tMMR.mmr_image_crop_bottom_5;;
    if (VENC_OK != VENC_SetParam(ptHost->hVEnc, VENC_PRAM_RESOLUTION, (VOID*)&res))
    {
        Printf("Set parameter error!\n");
        return;
    }
    
    //PIX FORMATE
    pix.ePixFmt  = (tMMR.mmr_source_scan_type_2 == 0) ? VENC_PIX_FMT_NV12 : ((tMMR.mmr_source_scan_type_2 == 1) ? VENC_PIX_FMT_B16X32 : VENC_PIX_FMT_B64X32);
    pix.u2Pitch  = tMMR.mmr_pitch_width_in_pels;
    if (VENC_OK != VENC_SetCtrl(ptHost->hVEnc, VENC_CTRL_PIX_FMT, (VOID*)&pix))
    {
        Printf("Set ctrl error!\n");
        return;
    }
    
    //set buffer
    //Working Buffer
    {
    UINT32 u4Size;
    VENC_FRM_T rFrm;
    UINT32 u4Idx, u4TmpAddr;
    FBM_POOL_T *prFbmPool;
    VENC_CTRL_BUFF_T buff;
#ifdef FBM_CFG
    prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_VENC);
#else
    FBM_POOL_T rFbmPool;
    rFbmPool.u4Addr = 0x1b0eb000;//;0x0a9fff00;
    rFbmPool.u4Size = 16L<<20;
    prFbmPool = &rFbmPool;
#endif
    
    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    ASSERT(prFbmPool->u4Size != 0);

    HalInvalidateDCacheMultipleLine(VIRTUAL(prFbmPool->u4Addr), prFbmPool->u4Size);

    //raw data buffer
    ptHost->ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_FRM); 
    ASSERT(ptHost->ptFrmBuffMgr);
    
    _BUFMGR_OBJ_Reset(ptHost->ptFrmBuffMgr);
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
        _BUFMGR_OBJ_Add(ptHost->ptFrmBuffMgr, &rFrm);
    }

    buff.u4Addr  = VENC_ALIGN_MASK(u4TmpAddr, 63);
    buff.u4Size  = prFbmPool->u4Size - (u4TmpAddr - prFbmPool->u4Addr);
    //Printf("Venc buffer Addr:0x%8x, Size:0x%8x.\n", buff.u4Addr, buff.u4Size);
    
    if (VENC_OK != VENC_SetCtrl(ptHost->hVEnc, VENC_CTRL_BUFF, (VOID*)&buff))
    {
        Printf("Set ctrl buffer error!\n");
        return;
    }
    }

    if (ptHost->pfnSetEnv)
    {
        ptHost->pfnSetEnv(ptHost);
    }

    if (VENC_OK != VENC_Init(ptHost->hVEnc))
    {
        Printf("init error!\n");
        return;
    }

#ifdef VENC_CYCLE
    _u4VEncI_IDRCycleCount = 0;
    _u4VEncPFrmCycleCount = 0;
    _u4VEncBFrmCycleCount = 0;
    _u4VEncI_IDRCount = 0;
    _u4VEncPFrmCount = 0;
    _u4VEncBFrmCount = 0;
#endif

#ifdef VENC_DRAM_PROFILE
    {
        VENC_OBJECT_T *pObj = NULL;
        pObj = VENC_HANDLE_TO_OBJECT(ptHost->hVEnc,VENC_OBJECT_T);
        pObj->u4DramTotalCycle = 0;
        pObj->u4FrmCnt = 0;
    }
#endif

    if (VENC_OK != VENC_Run(ptHost->hVEnc))
    {
        Printf("run error!\n");
        return;
    }

    // Send raw data to encoder
    /*step 1 init varible*/
    x_strcpy(ptHost->szFilePath, ptCfgFile->InputFileName);
    ptHost->szFilePathPtr  = ptHost->szFilePath + x_strlen(ptHost->szFilePath);
    ptHost->u4ErrCnt = 0;
    ptHost->u4FrmNo  = (tMMR.mmr_num_of_b_frm_2 + 1) * tMMR.dwNumOfIandPFrames;
    ptHost->u4FrmCnt = 0;
    ptHost->u4Pitch= pix.u2Pitch;
    ptHost->u4Width  = res.u2Width;
    ptHost->u4Height = res.u2Height;
    ptHost->ptCtrl   = (H264_CTRL_T *)((VENC_HANDLE_TO_OBJECT(ptHost->hVEnc, VENC_OBJECT_T))->hCtrl);
    ptHost->fgFirst  = TRUE;
    ptHost->out_size = 0;
    ptHost->crc_frp  = 0;
    ptHost->keyframe_frp = 0;
    ptHost->skype_frp = 0;
    VERIFY(x_sema_create(&(ptHost->hEmuDoneSema), X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);

    /*step 2 load golden pattern*/
    if (_fgCompareGolden)
    {
        ptHost->u4GoldenBufAddr   = _GetGoldenBuffer();
        Printf("loading Golden...\n");
        _VEncLoadFile(ptCfgFile->GoldenFileName, ptHost->u4GoldenBufAddr);
    }

    _fgCompareCRC = tMMR.mmr_crc_check_enable_1 ? _fgCompareCRC : 0;
    if (_fgCompareCRC)
    {
        ptHost->u4CRCBufAddr   = _GetCRCBuffer();
        Printf("loading CRC...\n");
        _VEncLoadFile(ptCfgFile->CRCFileName, ptHost->u4CRCBufAddr);
    }

    if (_fgInserKeyframe)
    {
        ptHost->u4KeyInfoBufAddr = _GetKeyBuffer();
        Printf("loading Keyframe info...\n");
        _VEncLoadFile(ptCfgFile->KeyFrameFileName, ptHost->u4KeyInfoBufAddr);
    }

    if (_fgSkypeTest)
    {
        ptHost->u4SkypeInfoBufAddr = _GetSkypeBuffer();
        Printf("loading Skype info...\n");
        _VEncLoadFile(ptCfgFile->OutputFileName, ptHost->u4SkypeInfoBufAddr);
    }

    /*step 3 create thread to send data*/
    u4Value = (UINT32)ptHost;
    VERIFY(x_thread_create(&hThread,
        "SendDThread", VENC_STACK_SIZE, VENC_THREAD_PRIORITY,
        (x_os_thread_main_fct)(ptHost->pfnSendDataThread), sizeof(UINT32), (VOID *)&u4Value) == OSR_OK);

    /*setp 4 wait for encode finish*/
    VERIFY(x_sema_lock(ptHost->hEmuDoneSema, X_SEMA_OPTION_WAIT) == OSR_OK);

    /*setp 5 release resouce*/
    VERIFY(x_sema_delete(ptHost->hEmuDoneSema) == OSR_OK);
    

    //STOP VENC
    if (VENC_OK != VENC_Stop(ptHost->hVEnc))
    {
        Printf("stop error!\n");
        return;
    }

#ifdef VENC_CYCLE
    Printf("I/IDR Frame(%d) encoding take %d cycle per MB\n",
            _u4VEncI_IDRCount,
            _u4VEncI_IDRCycleCount / (_u4VEncI_IDRCount * _u4VEncH264NumMB));
    Printf("P Frame(%d) encoding take %d cycle per MB\n",
            _u4VEncPFrmCount,
            _u4VEncPFrmCycleCount / (_u4VEncPFrmCount * _u4VEncH264NumMB));
    Printf("B Frame(%d) encoding take %d cycle per MB\n",
            _u4VEncBFrmCount,
            _u4VEncBFrmCycleCount / (_u4VEncBFrmCount * _u4VEncH264NumMB));
#endif

#ifdef VENC_DRAM_PROFILE
    {
        VENC_OBJECT_T *pObj = NULL;
        pObj = VENC_HANDLE_TO_OBJECT(ptHost->hVEnc,VENC_OBJECT_T);
        UINT32 u4Tmp = pObj->u4DramTotalCycle / ptHost->u4FrmCnt;
        Printf("Total cycle: %d\n", pObj->u4DramTotalCycle);
        Printf("Cycle per frame(%d): %d\n", ptHost->u4FrmCnt, u4Tmp);
        u4Tmp *= 30;
        Printf("Cycle per 30 frame: %d = %dMBps\n", u4Tmp, u4Tmp >> 20);
    }
#endif

    //CLOSE VENC
    VENC_BUFMGR_OBJ_DELETE(VENC_FRM, (VENC_BUFMGR_T(VENC_FRM) *)(ptHost->ptFrmBuffMgr)); 
    if (VENC_OK != VENC_Close(ptHost->hVEnc))
    {
        Printf("close error!\n");
        return;
    }

    if (ptHost->u4ErrCnt > 0)
    {
        Printf("Compare NG with error count: %d !!!\n", ptHost->u4ErrCnt);
    }
    else
    {
        Printf("Compare passed!!\n");
    }
}


static UINT32 __str2val(UINT8 *pb, UINT8 len)
{
    UINT32 ret, i;
    ret = 0;
    for (i = 0; i < len; i++)
    {
        ret <<= 4;
        if (pb[i] >= 'a' && pb[i] <= 'f')
        {
            ret |= pb[i] - 'a' + 10;
        }
        else if (pb[i] >= '0' && pb[i] <= '9')
        {
            ret |= pb[i] - '0';
        }
        else
        {
            ASSERT(0);
        }
    }
    return ret;
}


VOID _VEncH264EmuDumpBst(UINT8 *pu1Buf, UINT32 u4BufSz)
{
    UINT32 i;

    Printf("size: %d\n", u4BufSz);
    for (i = 0; i < u4BufSz; i++)
    {
        if ((i & 0xf) == 0)
        {
            Printf("\n0x%08x: ", pu1Buf + i);
        }
        Printf("%02x ", pu1Buf[i]);
    }
    Printf("\n");
}


static VOID _VEncH264EmuDumpCRC(VOID)
{
    H264_EMU_HOST_T *ptHost = &rHost;
    Printf("\n++++++++++CRC info dump++++++++++++++++++++++++ !!!\n");
    Printf("     SPS:0x%08x\n", ptHost->crc_sps);
    Printf("     PPS:0x%08x\n", ptHost->crc_pps);
    Printf("   FRAME:0x%08x\n", ptHost->crc_ec);
    Printf(" FCS_cur:0x%08x\n", ptHost->crc_fcs_cur);
    Printf(" FCS_ref:0x%08x\n", ptHost->crc_fcs_ref);
    Printf("DPCM_cur:0x%08x\n", ptHost->crc_dpcm);
    Printf(" IME_cur:0x%08x\n", ptHost->crc_ime_cur);
    Printf(" IME_ref:0x%08x\n", ptHost->crc_ime_ref);
}


//-----------------------------------------------------------------------------
// Public function : 
//-----------------------------------------------------------------------------
VOID _VEncH264LoadEmu(VOID)
{
    BOOL fgLoad = FALSE;
    CLI_EXEC_T * prCmdTbl =  getVEncCmdTbl();
    while (prCmdTbl && prCmdTbl->pszCmdStr)
    {
        if (x_strcmp("emu", prCmdTbl->pszCmdStr) == 0)
        {
            fgLoad = TRUE;
            prCmdTbl->prCmdNextLevel = _arH264TestCaseTbl;
            break;
        }
        prCmdTbl++;
    }

    if (!fgLoad)
    {
        Printf("VEnc H264, load test casese failed!\n");
    }
}


VOID *_VEncH264GetEmuItem(void)
{
    return (VOID *)_VEncCmdEmuItem;
}


BOOL _VEncEmuCmpGolden(UINT32 u4CpSa, UINT32 u4CpSize, H264_EMU_HOST_T *ptHost)
{
    BOOL fgOk = TRUE;
    UINT32 i, j, k, tmp_data, max_byte_addr;
    volatile UINT8 *bs_ptr, *g_ptr;

    k = u4CpSize;
    tmp_data = u4CpSa;
    max_byte_addr = ptHost->ptCtrl->u4BsEa;

    while (k > 0)
    {
        j = k;
        if ((tmp_data + j) > max_byte_addr)
        {
            j = max_byte_addr - tmp_data;
        }

        bs_ptr = (volatile UINT8 *)tmp_data;
        g_ptr = (volatile UINT8 *)(ptHost->u4GoldenBufAddr + ptHost->out_size);
        for (i = 0; i < j; i++)
        {
            if (bs_ptr[i] != g_ptr[i])
            {
                Printf("compare error(0x%x => 0x%x) %02x != %02x \n", i, ptHost->out_size + i, bs_ptr[i], g_ptr[i]);
                Printf("Bistream Sa/Sz = (0x%x/0x%x)\n", u4CpSa, u4CpSize);
                //_VEncH264EmuDumpBst((UINT8 *)bs_ptr, i + 32);
                _VEncH264EmuDumpCRC();
                _H264HalDumpRegs();
                ASSERT(0);
                fgOk = FALSE;
            }
        }

        if ((tmp_data + j) == max_byte_addr)
        { // bitstream wrap around
//            Printf("max_byte_addr hit!!\n");
            tmp_data = ptHost->ptCtrl->u4BsSa;
        }
        k = k-j;
        ptHost->out_size += j;
    }

    return fgOk;
}


static BOOL _VEncH264EmuCmpCRC(H264_EMU_HOST_T *ptHost, UINT32 type)
{
    UINT16 sync_size;
    UINT8 *pb;
    UINT32 crc_sps_cur;
    UINT32 crc_pps_cur;
    UINT32 crc_ec_cur;
    UINT32 crc_fcs_cur;
    UINT32 crc_fcs_ref;
    UINT32 crc_dpcm_cur;
    UINT32 crc_ime_cur;
    UINT32 error = 0;

    #define CRC_ASSERT(a) \
        do{\
            if (!(a))\
            {\
                error++;\
                _H264HalDumpRegs();\
                ASSERT(_fgNotAssert);\
            }\
        }while(0)
    
    #define CRC_SYNC_KEYWORD(_a,_n) \
        sync_size = 0; \
        pb = (UINT8 *)(ptHost->crc_frp + ptHost->u4CRCBufAddr);\
        do{ \
            if (!x_strncmp((const char *)_a, (const char *) pb, _n)) \
            { \
                break; \
            } \
            pb++; sync_size++; \
            ASSERT(sync_size < 1024); \
        }while(1);\
        ptHost->crc_frp += sync_size;


    #define CRC_SYNC_NEWLINE()  \
        sync_size = 0; \
        pb = (UINT8 *)(ptHost->crc_frp + ptHost->u4CRCBufAddr);\
        do{ \
            if (*pb == 0x0a) \
            { \
                break; \
            } \
            pb++; sync_size++; \
            ASSERT(sync_size < 1024); \
        }while(1) ;  \
        ptHost->crc_frp += sync_size + 1;



    #define CRC_READ_VALUE_HEX(a) \
        pb = (UINT8 *)(ptHost->crc_frp + ptHost->u4CRCBufAddr);\
        ptHost->crc_frp += 8; \
        a = __str2val(pb, 8);


   //compare 
    /*    
        xxxxxxxx    // FrmCnt=0
        c98430aa    // CRC_SPS_cur_result
        11d89ae5    // CRC_PPS_cur_result
        5d8d90c4    // CRC_EC_result
        207b1d07    // CRC_FCS_cur_result
        ffffffff    // CRC_FCS_ref_result
        0a9a3f4e    // CRC_DPCM_cur_result
        e5c00432    // CRC_IME_cur_result
    */
    
    //XXXXXXXXXX
    CRC_SYNC_KEYWORD("xxxxxxxx", 8);

    //PROCESS FrmCnt 
    CRC_SYNC_KEYWORD("FrmCnt=", 7);

    //PROCESS SPS
    CRC_SYNC_NEWLINE();
    CRC_READ_VALUE_HEX(crc_sps_cur);

    //PROCESS PPS
    CRC_SYNC_NEWLINE();
    CRC_READ_VALUE_HEX(crc_pps_cur);

    //PROCESS EC
    CRC_SYNC_NEWLINE();
    CRC_READ_VALUE_HEX(crc_ec_cur);

    //PROCESS FCS_cur
    CRC_SYNC_NEWLINE();
    CRC_READ_VALUE_HEX(crc_fcs_cur);

    //PROCESS FCS_ref
    CRC_SYNC_NEWLINE();
    CRC_READ_VALUE_HEX(crc_fcs_ref);

    //PROCESS DPCM
    CRC_SYNC_NEWLINE();
    CRC_READ_VALUE_HEX(crc_dpcm_cur);

    //PROCESS IME
    CRC_SYNC_NEWLINE();
    CRC_READ_VALUE_HEX(crc_ime_cur);


    Printf("++++++++++CRC Compare info (%d) +++++++++++++++ !!!\n", ptHost->u4FrmCnt);
    Printf("     SPS:(hw,ref->0x%08x, 0x%08x)\n", ptHost->crc_sps,crc_sps_cur);
    Printf("     PPS:(hw,ref->0x%08x, 0x%08x)\n", ptHost->crc_pps,crc_pps_cur);
    Printf("   FRAME:(hw,ref->0x%08x, 0x%08x)\n", ptHost->crc_ec,crc_ec_cur);
    Printf(" FCS_cur:(hw,ref->0x%08x, 0x%08x)\n", ptHost->crc_fcs_cur,crc_fcs_cur);
    Printf(" FCS_ref:(hw,ref->0x%08x, 0x%08x)\n", ptHost->crc_fcs_ref,crc_fcs_ref);
    Printf("DPCM_cur:(hw,ref->0x%08x, 0x%08x)\n", ptHost->crc_dpcm,crc_dpcm_cur);
    Printf(" IME_cur:(hw,ref->0x%08x, 0x%08x)\n", ptHost->crc_ime_cur,crc_ime_cur);
    Printf(" IME_ref:(hw,ref->0x%08x)\n", ptHost->crc_ime_ref);

    CRC_ASSERT(ptHost->crc_dpcm == crc_dpcm_cur);
    CRC_ASSERT(ptHost->crc_ime_cur == crc_ime_cur);

    if (!(ptHost->u4Height < QVGA_HEIGHT || ptHost->u4Width < QVGA_WIDTH))
    {
        UINT32 rc;
        rc = IO_VENC_H264_READ_BITS(VIDEO_HW_MODE_SEL, EN_RATECONTROL);
        // fix-qp + i, skip  
        if (!(!rc && ((type == VENC_H264_PIC_TYPE_IDR) || (type == VENC_H264_PIC_TYPE_I))))
        {
            CRC_ASSERT(ptHost->crc_fcs_cur == crc_fcs_cur);
            CRC_ASSERT(ptHost->crc_fcs_ref == crc_fcs_ref);
        }
    }

    if (VENC_H264_PIC_TYPE_IDR == type)
    {
        if (crc_sps_cur != 0xFFFFFFFF) {CRC_ASSERT(ptHost->crc_sps == crc_sps_cur);}
        if (crc_pps_cur != 0xFFFFFFFF) {CRC_ASSERT(ptHost->crc_pps == crc_pps_cur);}
    }

    CRC_ASSERT(ptHost->crc_ec == crc_ec_cur);

    return error ? FALSE : TRUE;
}


BOOL _VEncH264EmuGetKeyframeInfo(H264_EMU_HOST_T *ptHost)
{
    BOOL key_on = FALSE;
    UINT32 sync_size;
    UINT8 *pb;

    #define KEYFRAME_SYNC_NEWLINE()  \
        sync_size = 0; \
        pb = (UINT8 *)(ptHost->keyframe_frp + ptHost->u4KeyInfoBufAddr);\
        while (pb[sync_size++] != 0x0a)\
        {\
            ASSERT(sync_size < 1024); \
        }\
        ptHost->keyframe_frp += sync_size;

    #define KEYFRAME_READ_VALUE(a) \
        pb = (UINT8 *)(ptHost->keyframe_frp + ptHost->u4KeyInfoBufAddr);\
        while (*pb == 0x20) {pb++;} \
        a = (*pb == '0') ? FALSE:TRUE;       

    KEYFRAME_READ_VALUE(key_on);
    KEYFRAME_SYNC_NEWLINE();

    Printf(" Insert KeyInfo:%d\n", key_on);

    return key_on;
}


VOID _VEncH264EmuSetSkypeInfo(VENC_OBJECT_T *this)
{
    H264_EMU_HOST_T *ptHost = &rHost;
    UINT32 sync_size;
    UINT32 u4Val;
    UINT32 u4Bps, u4Fps;
    UINT8 *pb;

    #define SKYPE_SYNC_NEWLINE()  \
        sync_size = 0; \
        pb = (UINT8 *)(ptHost->skype_frp + ptHost->u4SkypeInfoBufAddr);\
        while (pb[sync_size++] != 0x0a)\
        {\
            ASSERT(sync_size < 1024); \
        }\
        ptHost->skype_frp += sync_size;

    #define SKYPE_READ_VALUE_HEX(a) \
        pb = (UINT8 *)(ptHost->skype_frp + ptHost->u4SkypeInfoBufAddr);\
        ptHost->skype_frp += 8; \
        a = __str2val(pb, 8);

    SKYPE_READ_VALUE_HEX(u4Val);
    SKYPE_SYNC_NEWLINE();

    u4Bps = u4Val >> 16;
    u4Fps = u4Val & 0xffff;

    _VENC_H264_HalRtResetBps(this, u4Bps);
    _VENC_H264_HalRtResetFps(this, u4Fps);
}


static void _VEncH264TestCaseDefaultCfg(void)
{
    _fgCompareCRC    = _fgCompareCRCDefault;
    _fgCompareGolden = _fgCompareGoldenDefault;
    _fgInserKeyframe = _fgInserKeyframeDefault;
    _fgNotAssert     = _fgNotAssertDefault;
    _fgVUITest       = FALSE;
    _fgSkypeTest     = FALSE;

    _fgDlaTestOn     = FALSE;

    _hEncFrmStart    = NULL_HANDLE;

    //
    rHost.pfnSendDataThread = _SendDataThreadAsync;
    rHost.pfnSetEnv         = _SetEnvAsync;
}


//-----------------------------------------------------------------------------
// Test Case debug : 
//-----------------------------------------------------------------------------
static INT32 _VEncEmuTestCaseDbg(INT32 i4Argc, const CHAR ** szArgv)
{
    Printf("\n");
    Printf("***************Test Case debug*******************\n");

    _VEncH264TestCaseDefaultCfg();

    //environment setting
    rHost.pfnSendDataThread = _SendDataThreadSync;
    rHost.pfnSetEnv         = _SetEnvSync;

    _VEncCmdEmuList("D:\\VENC\\debList.txt");

    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}


//-----------------------------------------------------------------------------
// Test Case 1 : 
//-----------------------------------------------------------------------------
static INT32 _VEncEmuTestCase1(INT32 i4Argc, const CHAR ** szArgv)
{
    Printf("\n");
    Printf("***************Test Case 1*******************\n");
    Printf("Function Test High Priority\n");

    _VEncH264TestCaseDefaultCfg();

    //environment setting
    _fgCompareGolden = TRUE;
    _fgCompareCRC = TRUE;

    _VEncCmdEmuList("D:\\VENC\\hp_list.txt");

    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}


//-----------------------------------------------------------------------------
// Test Case 2 : 
//-----------------------------------------------------------------------------
static INT32 _VEncEmuTestCase2(INT32 i4Argc, const CHAR ** szArgv)
{
    Printf("\n");
    Printf("***************Test Case 2*******************\n");
    Printf("Function Test Mediium Priority\n");
    _VEncH264TestCaseDefaultCfg();

    //environment setting
    _fgCompareGolden = FALSE;
    _fgCompareCRC = TRUE;

    _VEncCmdEmuList("D:\\VENC\\mp_list.txt");

    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}

//-----------------------------------------------------------------------------
// Test Case 3 : 
//-----------------------------------------------------------------------------
static INT32 _VEncEmuTestCase3(INT32 i4Argc, const CHAR ** szArgv)
{
    Printf("\n");
    Printf("***************Test Case 3*******************\n");
    Printf("Function Test Low Priority\n");

    _VEncH264TestCaseDefaultCfg();

    //environment setting
    _fgCompareGolden = FALSE;

    _VEncCmdEmuList("D:\\VENC\\lp_list.txt");

    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}

//-----------------------------------------------------------------------------
// Test Case 4 : 
//-----------------------------------------------------------------------------
static INT32 _VEncEmuTestCase4(INT32 i4Argc, const CHAR ** szArgv)
{
    Printf("\n");
    Printf("***************Test Case 4*******************\n");
    Printf("Multi-SPS test\n");

    _VEncH264TestCaseDefaultCfg();

    //environment setting

    _VEncCmdEmuList("D:\\VENC\\msps_list.txt");

    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}

//-----------------------------------------------------------------------------
// Test Case 5 : 
//-----------------------------------------------------------------------------
static VOID _SendDataThreadInsertKeyframeSync(VOID *pvArg)
{
    UINT32 u4Res;
    UINT32 u4FrmIdx;
    H264_EMU_HOST_T *ptHost = (H264_EMU_HOST_T *)(*(UINT32 *)pvArg);
    VENC_FRM_T rFrm;
    VENC_IN_PIC_T rIn;
    VENC_OUT_PIC_T rOut;
    H264_EMU_OUT_PIC_T rEmuOut;
    BOOL fgVal;
    ASSERT(ptHost);

    u4Res = ptHost->u4Height * ptHost->u4Width;

    Printf("ENTER _SendDataThreadInsertKeyframeSync\n");

    x_memset(&rIn, 0, sizeof(rIn));
    for (u4FrmIdx = 0; u4FrmIdx < ptHost->u4FrmNo; ++u4FrmIdx)
    {
        _BUFMGR_OBJ_Get(ptHost->ptFrmBuffMgr, (VOID *)&rFrm, VENC_BUFMGR_WAIT);
        
        rIn.u4YAddr = rFrm.u4YAddr;
        rIn.u4CAddr = rFrm.u4UVAddr;

        x_sprintf(ptHost->szFilePathPtr, "_%08d.y", u4FrmIdx);
#ifdef USB_SUPPORT
        _VEncLoadUsbFile(ptHost->szFilePath, rIn.u4YAddr);
#else
        _VEncLoadFile(ptHost->szFilePath, rIn.u4YAddr);
#endif
        HalFlushDCacheMultipleLine(VIRTUAL(rIn.u4YAddr), u4Res);

        x_sprintf(ptHost->szFilePathPtr, "_%08d.c", u4FrmIdx);
#ifdef USB_SUPPORT
        _VEncLoadUsbFile(ptHost->szFilePath, rIn.u4CAddr);
#else
        _VEncLoadFile(ptHost->szFilePath, rIn.u4CAddr);
#endif
        HalFlushDCacheMultipleLine(VIRTUAL(rIn.u4CAddr), (u4Res>>1));

        //Get insert keyframe info
        fgVal = _VEncH264EmuGetKeyframeInfo(ptHost);

        if (fgVal)
        {
            VENC_RT_CTRL_T rRTCtrl;
            rRTCtrl.u4Flag = VENC_RT_CTRL_FLAG_INSERT_KEYFRAME; 
            VENC_SetRtCtrlSync(ptHost->hVEnc, &rRTCtrl);
        }
        
        if (VENC_OK != VENC_EncFrameSync(ptHost->hVEnc, &rIn, (VENC_OUT_PIC_T*)&rEmuOut))
        {
            Printf("enc frame fail\n");
            break;
        }


        if ((rEmuOut.u4Type) & VENC_H264_PIC_TYPE_SPS)
        {
            rOut.u4Addr = rEmuOut.u4SPSAddr;
            rOut.u4Size = rEmuOut.u4SPSSize;
            _emu_demo_cb((VOID *)ptHost, NULL, &rOut, VENC_H264_PIC_TYPE_SPS); 
        }
        if ((rEmuOut.u4Type) & VENC_H264_PIC_TYPE_PPS)
        {
            rOut.u4Addr = rEmuOut.u4PPSAddr;
            rOut.u4Size = rEmuOut.u4PPSSize;
            _emu_demo_cb((VOID *)ptHost, NULL, &rOut, VENC_H264_PIC_TYPE_PPS); 
        }

        rOut.u4Addr = rEmuOut.u4FRMAddr;
        rOut.u4Size = rEmuOut.u4FRMSize;
        _emu_demo_cb((VOID *)ptHost, &rIn, &rOut, (rEmuOut.u4Type)&0xF0); 
        
    }

    Printf("LEAVE _SendDataThreadInsertKeyframeSync\n");

    x_thread_exit();
}


static VOID _SendDataThreadInsertKeyframeAsync(VOID *pvArg)
{
    UINT32 u4Res;
    UINT32 u4FrmIdx;
    H264_EMU_HOST_T *ptHost = (H264_EMU_HOST_T *)(*(UINT32 *)pvArg);
    VENC_FRM_T rFrm;
    VENC_IN_PIC_T rIn;
    BOOL fgVal;
    ASSERT(ptHost);

    u4Res = ptHost->u4Height * ptHost->u4Width;

    Printf("ENTER _SendDataThreadInsertKeyframeAsync\n");
    x_memset(&rIn, 0, sizeof(rIn));
    for (u4FrmIdx = 0; u4FrmIdx < ptHost->u4FrmNo; ++u4FrmIdx)
    {
        _BUFMGR_OBJ_Get(ptHost->ptFrmBuffMgr, (VOID *)&rFrm, VENC_BUFMGR_WAIT);
        
        rIn.u4YAddr = rFrm.u4YAddr;
        rIn.u4CAddr = rFrm.u4UVAddr;

        x_sprintf(ptHost->szFilePathPtr, "_%08d.y", u4FrmIdx);
#ifdef USB_SUPPORT
        _VEncLoadUsbFile(ptHost->szFilePath, rIn.u4YAddr);
#else
        _VEncLoadFile(ptHost->szFilePath, rIn.u4YAddr);
#endif
        HalFlushDCacheMultipleLine(VIRTUAL(rIn.u4YAddr), u4Res);

        x_sprintf(ptHost->szFilePathPtr, "_%08d.c", u4FrmIdx);
#ifdef USB_SUPPORT
        _VEncLoadUsbFile(ptHost->szFilePath, rIn.u4CAddr);
#else
        _VEncLoadFile(ptHost->szFilePath, rIn.u4CAddr);
#endif
        HalFlushDCacheMultipleLine(VIRTUAL(rIn.u4CAddr), (u4Res>>1));

        //Get insert keyframe info
        fgVal = _VEncH264EmuGetKeyframeInfo(ptHost);

        if (fgVal)
        {
            VENC_RT_CTRL_T rRTCtrl;
            rRTCtrl.u4Flag = VENC_RT_CTRL_FLAG_INSERT_KEYFRAME; 
            VENC_SetRtCtrlAsync(ptHost->hVEnc, &rRTCtrl);
        }
        
        if (VENC_OK != VENC_EncFrameAsync(ptHost->hVEnc, &rIn, NULL))
        {
            Printf("enc frame fail\n");
            break;
        }
        
    }

    Printf("LEAVE _SendDataThreadInsertKeyframeAsync\n");

    x_thread_exit();
}


static INT32 _VEncEmuTestCase5(INT32 i4Argc, const CHAR ** szArgv)
{
    BOOL fgTestSync = FALSE;

    if (i4Argc > 1)
    {
        fgTestSync = StrToInt(szArgv[1]);
    }

    Printf("\n");
    Printf("***************Test Case 5*******************\n");
    Printf("Insert Keyframe test\n");
    // use IPPP mode to test this function

    _VEncH264TestCaseDefaultCfg();

    //environment setting
    if (fgTestSync)
    {
        rHost.pfnSendDataThread = _SendDataThreadInsertKeyframeSync;
        rHost.pfnSetEnv         = _SetEnvSync;
    }
    else
    {
        rHost.pfnSendDataThread = _SendDataThreadInsertKeyframeAsync;
        rHost.pfnSetEnv         = _SetEnvAsync;
    }

    _fgInserKeyframe        = TRUE;
    
    _VEncCmdEmuList("D:\\VENC\\ikey_list.txt");

    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}

//-----------------------------------------------------------------------------
// Test Case 6 : 
//-----------------------------------------------------------------------------
static INT32 _VEncEmuTestCase6(INT32 i4Argc, const CHAR ** szArgv)
{
    Printf("\n");
    Printf("***************Test Case 6*******************\n");
    Printf("Block Mode(16x32) Test\n");

    _VEncH264TestCaseDefaultCfg();

    //environment setting

    _VEncCmdEmuList("D:\\VENC\\b1632_list.txt");

    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}

//-----------------------------------------------------------------------------
// Test Case 7 : 
//-----------------------------------------------------------------------------
static INT32 _VEncEmuTestCase7(INT32 i4Argc, const CHAR ** szArgv)
{
    Printf("\n");
    Printf("***************Test Case 7*******************\n");
    Printf("Block Mode(64x32) Test\n");

    _VEncH264TestCaseDefaultCfg();

    //environment setting

    _VEncCmdEmuList("D:\\VENC\\b6432_list.txt");

    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}

//-----------------------------------------------------------------------------
// Test Case 8 : 
//-----------------------------------------------------------------------------
static INT32 _VEncEmuTestCase8(INT32 i4Argc, const CHAR ** szArgv)
{
    Printf("\n");
    Printf("***************Test Case 8*******************\n");
    Printf("SPS VUI Test\n");

    _VEncH264TestCaseDefaultCfg();

    //environment setting
    _fgVUITest = TRUE;
    
    _VEncCmdEmuList("D:\\VENC\\vui_list.txt");

    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}

//-----------------------------------------------------------------------------
// Test Case 9 : 
//-----------------------------------------------------------------------------
static VOID _SendDataThreadRandom(VOID *pvArg)
{
    UINT32 u4Res;
    UINT32 u4FrmIdx;
    H264_EMU_HOST_T *ptHost = (H264_EMU_HOST_T *)(*(UINT32 *)pvArg);
    VENC_FRM_T rFrm;
    VENC_IN_PIC_T rIn;
    VENC_OUT_PIC_T rOut;
    H264_EMU_OUT_PIC_T rEmuOut;
    UINT32 u4Val, u4RandomAddr;
    ASSERT(ptHost);

    u4Res = ptHost->u4Height * ptHost->u4Width;

    Printf("ENTER _SendDataThreadRandom\n");
    x_memset(&rIn, 0, sizeof(rIn));
    for (u4FrmIdx = 0; u4FrmIdx < ptHost->u4FrmNo; ++u4FrmIdx)
    {
        _BUFMGR_OBJ_Get(ptHost->ptFrmBuffMgr, (VOID *)&rFrm, VENC_BUFMGR_WAIT);
        
        rIn.u4YAddr = rFrm.u4YAddr;
        rIn.u4CAddr = rFrm.u4UVAddr;

        x_sprintf(ptHost->szFilePathPtr, "_%08d.y", u4FrmIdx);
#ifdef USB_SUPPORT
        _VEncLoadUsbFile(ptHost->szFilePath, rIn.u4YAddr);
#else
        _VEncLoadFile(ptHost->szFilePath, rIn.u4YAddr);
#endif
        HalFlushDCacheMultipleLine(VIRTUAL(rIn.u4YAddr), u4Res);

        x_sprintf(ptHost->szFilePathPtr, "_%08d.c", u4FrmIdx);
#ifdef USB_SUPPORT
        _VEncLoadUsbFile(ptHost->szFilePath, rIn.u4CAddr);
#else
        _VEncLoadFile(ptHost->szFilePath, rIn.u4CAddr);
#endif
        HalFlushDCacheMultipleLine(VIRTUAL(rIn.u4CAddr), (u4Res>>1));

        //gen random address
        u4Val = rand() << 3; // 
        u4Val = ptHost->ptCtrl->u4BsSa + (u4Val % (ptHost->ptCtrl->u4BsSz));
        u4RandomAddr = u4Val;
        Printf("Random WP: 0x%08x\n", u4Val);
        u4Val >>= 3;
        IO_VENC_H264_WRITE_REG(BITSTREAM_BUF_DRAM_START_READ_ADDR, u4Val);
        u4Val |= (1UL << 31);
        IO_VENC_H264_WRITE_REG(UPDATE_BITSTREAM_BUF_DRAM_START_WRITE_ADDR, u4Val);
        
        if (VENC_OK != VENC_EncFrameSync(ptHost->hVEnc, &rIn, (VENC_OUT_PIC_T*)&rEmuOut))
        {
            Printf("enc frame fail\n");
            break;
        }

        ASSERT(rEmuOut.u4Addr == u4RandomAddr);

        if ((rEmuOut.u4Type) & VENC_H264_PIC_TYPE_SPS)
        {
            rOut.u4Addr = rEmuOut.u4SPSAddr;
            rOut.u4Size = rEmuOut.u4SPSSize;
            _emu_demo_cb((VOID *)ptHost, NULL, &rOut, VENC_H264_PIC_TYPE_SPS); 
        }
        if ((rEmuOut.u4Type) & VENC_H264_PIC_TYPE_PPS)
        {
            rOut.u4Addr = rEmuOut.u4PPSAddr;
            rOut.u4Size = rEmuOut.u4PPSSize;
            _emu_demo_cb((VOID *)ptHost, NULL, &rOut, VENC_H264_PIC_TYPE_PPS); 
        }

        rOut.u4Addr = rEmuOut.u4FRMAddr;
        rOut.u4Size = rEmuOut.u4FRMSize;
        _emu_demo_cb((VOID *)ptHost, &rIn, &rOut, (rEmuOut.u4Type)&0xF0); 
        
    }

    Printf("LEAVE _SendDataThreadRandom\n");

    x_thread_exit();
}


static INT32 _VEncEmuTestCase9(INT32 i4Argc, const CHAR ** szArgv)
{
    Printf("\n");
    Printf("***************Test Case 9*******************\n");
    Printf("Read/Write Pointer Test\n");

    //use sync mode test this function, all the files are IPPPP 
    
    _VEncH264TestCaseDefaultCfg();

    //environment setting
    rHost.pfnSendDataThread = _SendDataThreadSync;    //FOR COMPLIE WARNING
    rHost.pfnSendDataThread = _SendDataThreadRandom;
    rHost.pfnSetEnv         = _SetEnvSync;
    

    _VEncCmdEmuList("D:\\VENC\\random_list.txt");

    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}


//-----------------------------------------------------------------------------
// Test Case 10 : 
//-----------------------------------------------------------------------------
static INT32 _VEncEmuTestCase10(INT32 i4Argc, const CHAR ** szArgv)
{
    UINT32 u4Case;

    Printf("\n");
    Printf("***************Test Case 10*******************\n");
    Printf("DRAM DLA Parameters Test\n");

    if (i4Argc < 2)
    {
        Printf("Usage: %s case_no[1,2,3,4,5]. \n", szArgv[0]);
        Printf("example: %s 1 \n", szArgv[0]);
        return -1;
    }

    u4Case = StrToInt(szArgv[1]);

    _VEncH264TestCaseDefaultCfg();

    //environment setting
    _fgDlaTestOn = TRUE;

    switch(u4Case)
    {
    case 1:    
        Printf("1) DLA parameters:\n");
        Printf("waddr_fifo_th  = 32\n");
        Printf("raddr_fifo_thr = 4\n");
        Printf("wdata_fifo_thr = 32\n");
        Printf("rdata_fifo_thr = 16\n");
        Printf("time_out       = 4\n");
        _u4DlaWrAddrThrd = 32;
        _u4DlaWrDataThrd = 4;
        _u4DlaRdAddrThrd = 32;
        _u4DlaRdDataThrd = 16;
        _u4DlaWrTimeout  = 14;
        _fgDlaBypass     = FALSE;
        _VEncCmdEmuList("D:\\VENC\\dla_list.txt");
        break;
    case 2:
        Printf("2) DLA parameters:\n");
        Printf("waddr_fifo_th  = 64\n");
        Printf("raddr_fifo_thr = 64\n");
        Printf("wdata_fifo_thr = 64\n");
        Printf("rdata_fifo_thr = 64\n");
        Printf("time_out       = 31\n");
        _u4DlaWrAddrThrd = 64;
        _u4DlaWrDataThrd = 64;
        _u4DlaRdAddrThrd = 64;
        _u4DlaRdDataThrd = 64;
        _u4DlaWrTimeout  = 31;
        _fgDlaBypass     = FALSE;
        _VEncCmdEmuList("D:\\VENC\\dla_list.txt");
        break;
    case 3:
        Printf("3) DLA parameters:\n");
        Printf("waddr_fifo_th  = 32\n");
        Printf("raddr_fifo_thr = 32\n");
        Printf("wdata_fifo_thr = 32\n");
        Printf("rdata_fifo_thr = 32\n");
        Printf("time_out       = 16\n");
        _u4DlaWrAddrThrd = 32;
        _u4DlaWrDataThrd = 32;
        _u4DlaRdAddrThrd = 32;
        _u4DlaRdDataThrd = 32;
        _u4DlaWrTimeout  = 16;
        _fgDlaBypass     = FALSE;
        _VEncCmdEmuList("D:\\VENC\\dla_list.txt");
        break;
    case 4:
        Printf("4) DLA parameters:\n");
        Printf("waddr_fifo_th  = 1\n");
        Printf("raddr_fifo_thr = 1\n");
        Printf("wdata_fifo_thr = 1\n");
        Printf("rdata_fifo_thr = 1\n");
        Printf("time_out       = 1\n");
        _u4DlaWrAddrThrd = 1;
        _u4DlaWrDataThrd = 1;
        _u4DlaRdAddrThrd = 1;
        _u4DlaRdDataThrd = 1;
        _u4DlaWrTimeout  = 1;
        _fgDlaBypass     = FALSE;
        _VEncCmdEmuList("D:\\VENC\\dla_list.txt");
        break;

    case 5:
        Printf("5) DLA parameters:\n");
        Printf("bypass\n");
        _fgDlaBypass     = TRUE;
        _VEncCmdEmuList("D:\\VENC\\dla_list.txt");
        break;
    }        
    return 0;
}



//-----------------------------------------------------------------------------
// Test Case 11 : 
//-----------------------------------------------------------------------------
UINT8   PATTERN_DATA[] = {0,0,0,1};  
#define PATTERN_LEN   sizeof(PATTERN_DATA)/sizeof(PATTERN_DATA[0])

static VOID _CheckEncStall(VOID *pvArg)
{
    UINT32 u4Val;
    UINT8 *pu1Val;
    UINT32 i;
    Printf("ENTER _CheckEncStall\n");

    while (_fgRpWpThdRun)
    {
        //Printf("Start _CheckEncStall\n");
        VERIFY(x_sema_lock(_hEncFrmStart, X_SEMA_OPTION_WAIT) == OSR_OK);
        if (_fgRpWpThdRun && IO_VENC_H264_READ_BITS(RP_WP_STATUS, STALL))
        {
            Printf("output buffer is fullness!!!!! Consume data...\n");
            //read addr
            u4Val = IO_VENC_H264_READ_BITS(BITSTREAM_BUF_DRAM_START_READ_ADDR, BSMSRA);
            u4Val <<=3;
            pu1Val = (UINT8 *)VIRTUAL(u4Val);
            Printf("check pattern: ");
            for (i = 0; i < PATTERN_LEN; i++)
            {
                Printf(" %d", pu1Val[i]);
            }
            Printf("\n");
            ASSERT(x_memcmp((VOID *)pu1Val, (VOID *)PATTERN_DATA, PATTERN_LEN) == 0);            
            //u4Val = IO_VENC_H264_READ_BITS(UPDATE_BITSTREAM_BUF_DRAM_START_WRITE_ADDR, BSMSWA);
            u4Val = _VEnc_H264_HalGetBSCurrWriteAddr();
            u4Val >>= 3;
            IO_VENC_H264_WRITE_REG(BITSTREAM_BUF_DRAM_START_READ_ADDR, u4Val);
        }
    }

    if (_hEncFrmStart)
    {
        VERIFY(x_sema_delete(_hEncFrmStart) == OSR_OK);
        _hEncFrmStart = NULL_HANDLE;
    }

    Printf("LEAVE _CheckEncStall\n");
    
    x_thread_exit();
}

static VOID _SendDataThreadRandomRW(VOID *pvArg)
{
    //INT32 i4Ret;
    UINT32 u4Res;
    UINT32 u4FrmIdx;
    H264_EMU_HOST_T *ptHost = (H264_EMU_HOST_T *)(*(UINT32 *)pvArg);
    VENC_FRM_T rFrm;
    VENC_IN_PIC_T rIn;
    VENC_OUT_PIC_T rOut;
    H264_EMU_OUT_PIC_T rEmuOut;
    UINT32 u4Val, u4RandomAddr;
    UINT32 u4RandomRAddr;
    BOOL  fgAlwaysRandomWP;
    HANDLE_T hHandle;
    ASSERT(ptHost);

    u4Res = ptHost->u4Height * ptHost->u4Width;

    Printf("ENTER _SendDataThreadRandom\n");

    x_memset(&rIn, 0, sizeof(rIn));

    _fgRpWpThdRun = TRUE;

    if (NULL_HANDLE == _hEncFrmStart)
    {
        VERIFY(x_sema_create(&_hEncFrmStart, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
    }

    VERIFY(x_thread_create(&hHandle,
        "CheckEncStall", VENC_STACK_SIZE, VENC_THREAD_PRIORITY,
        (x_os_thread_main_fct)_CheckEncStall, 0, NULL) == OSR_OK);

    fgAlwaysRandomWP = rand()%2;

    if (fgAlwaysRandomWP)
    {
        Printf("Random WP Mode:Always \n");
    }
    else
    {
        Printf("Random WP Mode:Once\n");
    }
    //gen random address
    u4Val = rand() << 3; // 
    u4Val = ptHost->ptCtrl->u4BsSa + (u4Val % (ptHost->ptCtrl->u4BsSz));
    u4RandomAddr  = u4Val;
    Printf("Random WP: 0x%08x\n", u4Val);
    u4Val >>= 3;
    u4Val |= (1UL << 31);
    IO_VENC_H264_WRITE_REG(UPDATE_BITSTREAM_BUF_DRAM_START_WRITE_ADDR, u4Val);
    
    for (u4FrmIdx = 0; u4FrmIdx < ptHost->u4FrmNo; ++u4FrmIdx)
    {
        _BUFMGR_OBJ_Get(ptHost->ptFrmBuffMgr, (VOID *)&rFrm, VENC_BUFMGR_WAIT);
        
        rIn.u4YAddr = rFrm.u4YAddr;
        rIn.u4CAddr = rFrm.u4UVAddr;

        x_sprintf(ptHost->szFilePathPtr, "_%08d.y", u4FrmIdx);
#ifdef USB_SUPPORT
        _VEncLoadUsbFile(ptHost->szFilePath, rIn.u4YAddr);
#else
        _VEncLoadFile(ptHost->szFilePath, rIn.u4YAddr);
#endif
        HalFlushDCacheMultipleLine(VIRTUAL(rIn.u4YAddr), u4Res);

        x_sprintf(ptHost->szFilePathPtr, "_%08d.c", u4FrmIdx);
#ifdef USB_SUPPORT
        _VEncLoadUsbFile(ptHost->szFilePath, rIn.u4CAddr);
#else
        _VEncLoadFile(ptHost->szFilePath, rIn.u4CAddr);
#endif
        HalFlushDCacheMultipleLine(VIRTUAL(rIn.u4CAddr), (u4Res>>1));

            //gen random address
        if (fgAlwaysRandomWP)
        {
            u4Val = rand() << 3; // 
            u4Val = ptHost->ptCtrl->u4BsSa + (u4Val % (ptHost->ptCtrl->u4BsSz));
            u4RandomAddr  = u4Val;
            u4Val >>= 3;
            u4Val |= (1UL << 31);
            IO_VENC_H264_WRITE_REG(UPDATE_BITSTREAM_BUF_DRAM_START_WRITE_ADDR, u4Val);
        }
        Printf("WP: 0x%08x\n", u4RandomAddr);

        //read 
        u4RandomRAddr = u4RandomAddr;
        u4Val = rand() << 3; 
        u4Val %=10000;
        if (u4RandomRAddr + u4Val + VENC_ALIGN_MASK(PATTERN_LEN, 7) >= ptHost->ptCtrl->u4BsEa)
        {
            u4Val = ptHost->ptCtrl->u4BsSa + (u4Val + VENC_ALIGN_MASK(PATTERN_LEN,7)) - (ptHost->ptCtrl->u4BsEa - u4RandomRAddr);
        }
        else
        {
            u4Val += u4RandomRAddr; 
        }
        x_memcpy((VOID *)VIRTUAL(u4Val), (VOID *)PATTERN_DATA, PATTERN_LEN);
        HalFlushDCacheMultipleLine(VIRTUAL(u4Val), PATTERN_LEN);
        Printf("RP: 0x%08x\n", u4Val);
        u4Val >>= 3;
        IO_VENC_H264_WRITE_REG(BITSTREAM_BUF_DRAM_START_READ_ADDR, u4Val);

        /*i4Ret = */x_sema_lock(_hEncFrmStart, X_SEMA_OPTION_NOWAIT);
        //Printf("Lock _hEncFrmStart ret %d\n", i4Ret);
        
        if (VENC_OK != VENC_EncFrameSync(ptHost->hVEnc, &rIn, (VENC_OUT_PIC_T*)&rEmuOut))
        {
            Printf("enc frame fail\n");
            break;
        }

        if (rEmuOut.u4Addr != u4RandomAddr)
        {
            Printf("out: 0x%08x, rand: 0x%08x\n", rEmuOut.u4Addr, u4RandomAddr);
            ASSERT(rEmuOut.u4Addr == u4RandomAddr);
        }
        u4RandomAddr = _VEnc_H264_HalGetBSCurrWriteAddr();
        Printf("Curr WP: 0x%08x\n", u4RandomAddr);

        if ((rEmuOut.u4Type) & VENC_H264_PIC_TYPE_SPS)
        {
            rOut.u4Addr = rEmuOut.u4SPSAddr;
            rOut.u4Size = rEmuOut.u4SPSSize;
            _emu_demo_cb((VOID *)ptHost, NULL, &rOut, VENC_H264_PIC_TYPE_SPS); 
        }
        if ((rEmuOut.u4Type) & VENC_H264_PIC_TYPE_PPS)
        {
            rOut.u4Addr = rEmuOut.u4PPSAddr;
            rOut.u4Size = rEmuOut.u4PPSSize;
            _emu_demo_cb((VOID *)ptHost, NULL, &rOut, VENC_H264_PIC_TYPE_PPS); 
        }

        rOut.u4Addr = rEmuOut.u4FRMAddr;
        rOut.u4Size = rEmuOut.u4FRMSize;
        _emu_demo_cb((VOID *)ptHost, &rIn, &rOut, (rEmuOut.u4Type)&0xF0); 
        
    }

    _fgRpWpThdRun = FALSE;
    x_sema_unlock(_hEncFrmStart);
    Printf("LEAVE _SendDataThreadRandom\n");

    x_thread_exit();
}


static INT32 _VEncEmuTestCase11(INT32 i4Argc, const CHAR ** szArgv)
{
    Printf("\n");
    Printf("***************Test Case 11*******************\n");
    Printf("Read/Write Pointer Test\n");

    //use sync mode test this function, all the files are IPPPP 
    
    _VEncH264TestCaseDefaultCfg();

    //environment setting
    rHost.pfnSendDataThread = _SendDataThreadSync;    //FOR COMPLIE WARNING
    rHost.pfnSendDataThread = _SendDataThreadRandomRW;
    rHost.pfnSetEnv         = _SetEnvSync;

    _VEncCmdEmuList("D:\\VENC\\random_list.txt");

    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}



//-----------------------------------------------------------------------------
// Test Case 12 : 
//-----------------------------------------------------------------------------
//use IPPPP file
#define TEST_FILE_320_240  "E:\\venc\\work\\MT5368\\FPGA\\config\\0320x0240_exp12\\walk_qvga\\h264enc_QP1.cfg\0"
#define TEST_FILE_352_288  "E:\\venc\\work\\MT5368\\FPGA\\config\\0352x0288_exp12\\canoe_cif\\h264enc_QP1.cfg\0"
#define TEST_FILE_640_480  "E:\\venc\\work\\MT5368\\FPGA\\config\\0640x0480_exp12\\football_640x480\\h264enc_QP1.cfg\0"
#define TEST_FILE_720_480  "E:\\venc\\work\\MT5368\\FPGA\\config\\0720x0480_exp12\\park_interlace_720x480\\h264enc_QP1.cfg\0"

#define TEST_FILE TEST_FILE_320_240
//#define VENC_RT_TEST_SYNC 
static VOID _VEncCmdEmuItemRtParmTest(UINT8 *pu1FPathTempBuf, CHAR *pcFListPathName, H264_MMR_PARAM_T *ptMMR)
{
    UINT32 BUF_MMR;
    UINT32 u4Value;
    HANDLE_T hThread;
    H264_EMU_HOST_T *ptHost = &rHost;
    CFGFILE *ptCfgFile = &_rCfgFile;
    VENC_RT_PARM_T rRTParm;
    
    Printf("Loading File path\n%s...\n", pcFListPathName);
    _VEncLoadFile(pcFListPathName, (UINT32)pu1FPathTempBuf);
    getCFGPathfile(pu1FPathTempBuf, ptCfgFile);

    BUF_MMR = _GetMMRBuffer();
    _VEncLoadFile(ptCfgFile->MMRFileName, BUF_MMR);
    _VEncEmuLoadConfig((CHAR *)BUF_MMR, ptMMR);

    //RESOLUTION
    rRTParm.u4Flag   = VENC_RT_PARM_FLAG_RES;
    rRTParm.u2Width  = (ptMMR->mmr_image_width_div16_7 << 4) - ptMMR->mmr_image_crop_right_4;;
    rRTParm.u2Height = (ptMMR->mmr_image_height_div16_6 << 4) - ptMMR->mmr_image_crop_bottom_5;;
    rRTParm.u2Pitch  = ptMMR->mmr_pitch_width_in_pels;


    //bitrate
    rRTParm.u4Flag   |= VENC_RT_PARM_FLAG_BITRATE;
    rRTParm.u4Bitrate = ptMMR->mmr_rc_target_bit_rate_14;


    //framterate
    rRTParm.u4Flag   |= VENC_RT_PARM_FLAG_FRAMERATE;
    rRTParm.u1Framerate = ptMMR->mmr_fps_8;  

#ifdef VENC_RT_TEST_SYNC
    if (VENC_OK != VENC_SetRtParamSync(ptHost->hVEnc, &rRTParm))
    {
        Printf("Set rt parameter error!\n");
        return;
    }
#else
    if (VENC_OK != VENC_SetRtParamAsync(ptHost->hVEnc, &rRTParm))
    {
        Printf("Set rt parameter error!\n");
        return;
    }
#endif

    // Send raw data to encoder
    /*step 1 init varible*/
    x_strcpy(ptHost->szFilePath, ptCfgFile->InputFileName);
    ptHost->szFilePathPtr  = ptHost->szFilePath + x_strlen(ptHost->szFilePath);
    ptHost->u4ErrCnt = 0;
    ptHost->u4FrmNo  = (ptMMR->mmr_num_of_b_frm_2 + 1) * ptMMR->dwNumOfIandPFrames;
    ptHost->u4FrmCnt = 0;
    ptHost->u4Width  = rRTParm.u2Width;
    ptHost->u4Height = rRTParm.u2Height;
    ptHost->ptCtrl   = (H264_CTRL_T *)((VENC_HANDLE_TO_OBJECT(ptHost->hVEnc, VENC_OBJECT_T))->hCtrl);
    ptHost->fgFirst  = TRUE;
    ptHost->out_size = 0;
    ptHost->crc_frp  = 0;
    ptHost->keyframe_frp = 0;
    ptHost->skype_frp = 0;
    VERIFY(x_sema_create(&(ptHost->hEmuDoneSema), X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);

    /*step 2 load golden pattern*/
    if (_fgCompareGolden)
    {
        ptHost->u4GoldenBufAddr   = _GetGoldenBuffer();
        Printf("loading Golden...\n");
        _VEncLoadFile(ptCfgFile->GoldenFileName, ptHost->u4GoldenBufAddr);
    }

    _fgCompareCRC = ptMMR->mmr_crc_check_enable_1 ? _fgCompareCRC : 0;
    if (_fgCompareCRC)
    {
        ptHost->u4CRCBufAddr   = _GetCRCBuffer();
        Printf("loading CRC...\n");
        _VEncLoadFile(ptCfgFile->CRCFileName, ptHost->u4CRCBufAddr);
    }

    /*step 3 create thread to send data*/
    u4Value = (UINT32)ptHost;
    VERIFY(x_thread_create(&hThread,
        "SendDThread", VENC_STACK_SIZE, VENC_THREAD_PRIORITY,
        (x_os_thread_main_fct)(ptHost->pfnSendDataThread), sizeof(UINT32), (VOID *)&u4Value) == OSR_OK);

    /*setp 4 wait for encode finish*/
    VERIFY(x_sema_lock(ptHost->hEmuDoneSema, X_SEMA_OPTION_WAIT) == OSR_OK);

    /*setp 5 release resouce*/
    VERIFY(x_sema_delete(ptHost->hEmuDoneSema) == OSR_OK);
    

    if (ptHost->u4ErrCnt > 0)
    {
        Printf("Compare NG with error count: %d !!!\n", ptHost->u4ErrCnt);
    }
    else
    {
        Printf("Compare passed!!\n");
    }

}


//for buffer test
VOID _VEncRtInsertGarbageFrame(H264_EMU_HOST_T *ptHost)
{
    UINT32 u4Res;
    VENC_FRM_T rFrm;
    VENC_IN_PIC_T rIn;
    x_memset(&rIn, 0, sizeof(rIn));
    u4Res = ptHost->u4Height * ptHost->u4Width;
    _BUFMGR_OBJ_Get(ptHost->ptFrmBuffMgr, (VOID *)&rFrm, VENC_BUFMGR_WAIT);
    rIn.u4YAddr = rFrm.u4YAddr;
    rIn.u4CAddr = rFrm.u4UVAddr;

    Printf("Insert garbage frame for memory manager test\n");

    x_sprintf(ptHost->szFilePathPtr, "_%08d.y", ptHost->u4FrmNo - 1);
    _VEncLoadFile(ptHost->szFilePath, rIn.u4YAddr);
    //Printf("Load Y:addr0x%8x, %s\n", rIn.u4YAddr, ptHost->szFilePath);
    HalFlushDCacheMultipleLine(VIRTUAL(rIn.u4YAddr), u4Res);

    x_sprintf(ptHost->szFilePathPtr, "_%08d.c", ptHost->u4FrmNo - 1);
    _VEncLoadFile(ptHost->szFilePath, rIn.u4CAddr);
    //Printf("Load C:addr0x%8x, %s\n", rIn.u4CAddr, ptHost->szFilePath);
    HalFlushDCacheMultipleLine(VIRTUAL(rIn.u4CAddr), (u4Res>>1));

    if (VENC_OK != VENC_EncFrameAsync(ptHost->hVEnc, &rIn, NULL))
    {
        Printf("enc frame fail\n");
        return;
    }

}


VOID _VEncEmuLoopRtParmTest(VOID *pvArg)
{
    UINT8 *pu1FListTempBuf;
    UINT8 *pu1FPathTempBuf;
    H264_EMU_HOST_T *ptHost = &rHost;
    CFGFILE *ptCfgFile = &_rCfgFile;
    UINT32 BUF_MMR;
    H264_MMR_PARAM_T tMMR;
    VENC_PRAM_RES_T res;
    VENC_CTRL_PIXFMT_T pix;
    UINT32 u4Value;

    UINT32 u4LoopNs;

    Printf("Start to Verify Video Encoder...\n");

    pu1FListTempBuf = (UINT8 *)BSP_AllocAlignedDmaMemory(0x32000, 32);
    pu1FPathTempBuf = (UINT8 *)BSP_AllocAlignedDmaMemory(0x2800, 32);

    ASSERT(pu1FListTempBuf);
    ASSERT(pu1FPathTempBuf);

    _VEncLoadFile(TEST_FILE, (UINT32)pu1FPathTempBuf);
    getCFGPathfile(pu1FPathTempBuf, ptCfgFile);

    BUF_MMR = _GetMMRBuffer();
    _VEncLoadFile(ptCfgFile->MMRFileName, BUF_MMR);
    _VEncEmuLoadConfig((CHAR *)BUF_MMR, &tMMR);


    //OPEN VENC
    ptHost->hVEnc = VENC_Open(VENC_H264, VENC_PROFILE_EMU, FALSE);
    if (VENC_NULL_HANDLE == ptHost->hVEnc)
    {
        Printf("Open venc fail!\n");
        return;
    }

    //set emu mmr
    if (VENC_OK != VENC_SetParam(ptHost->hVEnc, H264_EMU_PARM, (VOID*)&tMMR))
    {
        Printf("Set emu mmr error!\n");
        return;
    }

    //30 frame per sec.
    u4Value = tMMR.mmr_fps_8;  
    if (VENC_OK != VENC_SetParam(ptHost->hVEnc, VENC_PRAM_FRAMERATE, (VOID*)&u4Value))
    {
        Printf("Set parameter error!\n");
        return;
    }

    //RESOLUTION
    res.u2Width  = (tMMR.mmr_image_width_div16_7 << 4) - tMMR.mmr_image_crop_right_4;;
    res.u2Height = (tMMR.mmr_image_height_div16_6 << 4) - tMMR.mmr_image_crop_bottom_5;;
    if (VENC_OK != VENC_SetParam(ptHost->hVEnc, VENC_PRAM_RESOLUTION, (VOID*)&res))
    {
        Printf("Set parameter error!\n");
        return;
    }
    
    //PIX FORMATE
    pix.ePixFmt  = (tMMR.mmr_source_scan_type_2 == 0) ? VENC_PIX_FMT_NV12 : ((tMMR.mmr_source_scan_type_2 == 1) ? VENC_PIX_FMT_B16X32 : VENC_PIX_FMT_B64X32);
    pix.u2Pitch  = tMMR.mmr_pitch_width_in_pels;
    if (VENC_OK != VENC_SetCtrl(ptHost->hVEnc, VENC_CTRL_PIX_FMT, (VOID*)&pix))
    {
        Printf("Set ctrl error!\n");
        return;
    }
    
    //set buffer
    //Working Buffer
    {
    UINT32 u4Size;
    VENC_FRM_T rFrm;
    UINT32 u4Idx, u4TmpAddr;
    FBM_POOL_T *prFbmPool;
    VENC_CTRL_BUFF_T buff;
#ifdef FBM_CFG
    prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_VENC);
#else
    FBM_POOL_T rFbmPool;
    rFbmPool.u4Addr = 0x1b0eb000;//;0x0a9fff00;
    rFbmPool.u4Size = 16L<<20;
    prFbmPool = &rFbmPool;
#endif
    
    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    ASSERT(prFbmPool->u4Size != 0);

    HalInvalidateDCacheMultipleLine(VIRTUAL(prFbmPool->u4Addr), prFbmPool->u4Size);

    //raw data buffer
    ptHost->ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_FRM); 
    ASSERT(ptHost->ptFrmBuffMgr);
    
    _BUFMGR_OBJ_Reset(ptHost->ptFrmBuffMgr);
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
        _BUFMGR_OBJ_Add(ptHost->ptFrmBuffMgr, &rFrm);
    }

    buff.u4Addr  = VENC_ALIGN_MASK(u4TmpAddr, 63);
    buff.u4Size  = prFbmPool->u4Size - (u4TmpAddr - prFbmPool->u4Addr);
    //Printf("Venc buffer Addr:0x%8x, Size:0x%8x.\n", buff.u4Addr, buff.u4Size);
    
    if (VENC_OK != VENC_SetCtrl(ptHost->hVEnc, VENC_CTRL_BUFF, (VOID*)&buff))
    {
        Printf("Set ctrl buffer error!\n");
        return;
    }
    }

    if (ptHost->pfnSetEnv)
    {
        ptHost->pfnSetEnv(ptHost);
    }

    if (VENC_OK != VENC_Init(ptHost->hVEnc))
    {
        Printf("init error!\n");
        return;
    }

    if (VENC_OK != VENC_Run(ptHost->hVEnc))
    {
        Printf("run error!\n");
        return;
    }

    u4LoopNs = 100;

    while (u4LoopNs--)
    {
        //test list
        _VEncCmdEmuItemRtParmTest(pu1FPathTempBuf, TEST_FILE_352_288, &tMMR);

        #if (1 && !defined(VENC_RT_TEST_SYNC)) 
        _VEncRtInsertGarbageFrame(ptHost);
        #endif

        _VEncCmdEmuItemRtParmTest(pu1FPathTempBuf, TEST_FILE_640_480, &tMMR);

        #if (1 && !defined(VENC_RT_TEST_SYNC)) 
        _VEncRtInsertGarbageFrame(ptHost);
        #endif

        _VEncCmdEmuItemRtParmTest(pu1FPathTempBuf, TEST_FILE_720_480, &tMMR);

        if (u4LoopNs)
        {
            #if (1 && !defined(VENC_RT_TEST_SYNC)) 
            _VEncRtInsertGarbageFrame(ptHost);
            #endif
        }

    }

    //STOP VENC
    if (VENC_OK != VENC_Stop(ptHost->hVEnc))
    {
        Printf("stop error!\n");
        return;
    }
        
    //CLOSE VENC
    VENC_BUFMGR_OBJ_DELETE(VENC_FRM, (VENC_BUFMGR_T(VENC_FRM) *)(ptHost->ptFrmBuffMgr)); 
    if (VENC_OK != VENC_Close(ptHost->hVEnc))
    {
        Printf("close error!\n");
        return;
    }

    BSP_FreeAlignedDmaMemory((UINT32)pu1FListTempBuf);
    BSP_FreeAlignedDmaMemory((UINT32)pu1FPathTempBuf);

    Printf("Verification done!\n");

    UNUSED(pvArg);
    x_thread_exit();
}

static INT32 _VEncEmuTestCase12(INT32 i4Argc, const CHAR ** szArgv)
{
    HANDLE_T hEmuThrd;
    
    Printf("\n");
    Printf("***************Test Case 11*******************\n");
    Printf("Runtime function Test\n");
    
    _VEncH264TestCaseDefaultCfg();

#ifdef VENC_RT_TEST_SYNC 
    rHost.pfnSendDataThread = _SendDataThreadSync;
    rHost.pfnSetEnv         = _SetEnvSync;
#endif

    // create emulation thread
    VERIFY(x_thread_create(&hEmuThrd,
        "Emulation Thread", VENC_STACK_SIZE, VENC_THREAD_PRIORITY,
        (x_os_thread_main_fct)_VEncEmuLoopRtParmTest, 0, NULL) == OSR_OK);

    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}


//-----------------------------------------------------------------------------
// Test Case 13 : 
//-----------------------------------------------------------------------------
static INT32 _VEncEmuTestCase13(INT32 i4Argc, const CHAR ** szArgv)
{
    UINT32 u4Case;
    Printf("\n");
    Printf("***************Test Case 13*******************\n");
    Printf("Performance Test\n");

#ifndef VENC_CYCLE
    UNUSED(i4Argc);
    UNUSED(szArgv);
    UNUSED(u4Case);
    Printf("Error: Please enable VENC_CYCLE for Performance Test\n");
#else
    if (i4Argc < 2)
    {
        Printf("Usage: %s case_no. \n", szArgv[0]);
        Printf("PME order: SKIP, 16x16, 16x08, 08x16, 08x08\n");
        Printf("case 1: PME11101\n");
        Printf("case 2: PME11001\n");
        Printf("case 3: PME11000\n");
        Printf("BME order: DIRECT, 16x16, 16x08, 08x16, 08x08\n");
        Printf("case 4: BME11101\n");
        Printf("case 5: BME11001\n");
        Printf("case 6: BME11000\n");
        return -1;
    }

    u4Case = StrToInt(szArgv[1]);
    _u4ForcePinterskip_1 = 1;
    _u4ForcePinter16x16_1 = 1;
    _u4ForcePinter16x08_1 = 1;
    _u4ForcePinter08x16_1 = 1;
    _u4ForcePinter08x08_1 = 1;
    _u4ForceBinterdirect_1 = 1;
    _u4ForceBinter16x16_1 = 1;
    _u4ForceBinter16x08_1 = 1;
    _u4ForceBinter08x16_1 = 1;
    _u4ForceBinter08x08_1 = 1;
    switch(u4Case)
    {
    case 3:
        _u4ForcePinter08x08_1 = 0;
    case 2:
        _u4ForcePinter16x08_1 = 0;
    case 1:
        _u4ForcePinter08x16_1 = 0;
        break;

    case 6:
        _u4ForceBinter08x08_1 = 0;
    case 5:
        _u4ForceBinter16x08_1 = 0;
    case 4:
        _u4ForceBinter08x16_1 = 0;
        break;
    }

    Printf("PME%d%d%d%d%d\n", 
        _u4ForcePinterskip_1, _u4ForcePinter16x16_1, _u4ForcePinter16x08_1, _u4ForcePinter08x16_1, _u4ForcePinter08x08_1);
    Printf("BME%d%d%d%d%d\n",
        _u4ForceBinterdirect_1, _u4ForceBinter16x16_1, _u4ForceBinter16x08_1, _u4ForceBinter08x16_1, _u4ForceBinter08x08_1);

    _VEncH264TestCaseDefaultCfg();

    //environment setting
    _fgCompareGolden = FALSE;
    _fgCompareCRC = FALSE;

    _VEncCmdEmuList("D:\\VENC\\cycle.txt");
#endif

    return 0;
}

//-----------------------------------------------------------------------------
// Test Case 14 : 
//-----------------------------------------------------------------------------

static INT32 _VEncEmuTestCase14(INT32 i4Argc, const CHAR ** szArgv)
{
    Printf("\n");
    Printf("***************Test Case 14*******************\n");
    Printf("Boundary Test\n");

#ifndef VENC_DRAM_PROFILE
    UNUSED(i4Argc);
    UNUSED(szArgv);
    Printf("Error: Please enable VENC_DRAM_PROFILE for Performance Test\n");
#else
    if (i4Argc < 3)
    {
        Printf("Usage: %s <mbaff_on_off> <#_of_B>\n", szArgv[0]);
        return -1;
    }

    _u4ForceMbaffOn = StrToInt(szArgv[1]);
    _u4ForceNumOfBFrm = StrToInt(szArgv[2]);

    Printf("MBAFF %s with %dB\n", _u4ForceMbaffOn? "on" : "off", _u4ForceNumOfBFrm);

    if (_u4ForceMbaffOn > 1)
    {
        Printf("Error: mbaff_on_off should be 0 or 1\n");
        return -1;
    }

    if (_u4ForceNumOfBFrm > 2)
    {
        Printf("Error: #_of_B should be 0, 1 or 2\n");
        return -2;
    }

    _VEncH264TestCaseDefaultCfg();

    //environment setting
    _fgCompareGolden = FALSE;
    _fgCompareCRC = FALSE;

    _VEncCmdEmuList("D:\\VENC\\dram.txt");
#endif

    return 0;
}

//-----------------------------------------------------------------------------
// Test Case 15 : 
//-----------------------------------------------------------------------------
static INT32 _VEncEmuTestCase15(INT32 i4Argc, const CHAR ** szArgv)
{
    Printf("\n");
    Printf("***************Test Case 15*******************\n");
    Printf("Skype Test\n");
    _VEncH264TestCaseDefaultCfg();

    //environment setting
    _fgCompareGolden = FALSE;
    _fgCompareCRC = TRUE;
    _fgSkypeTest = TRUE;

#ifndef VENC_MT8580_PATTERN
    LOG(0, "VENC_MT8580_PATTERN not define\n");
    ASSERT(0);
#endif

    _VEncCmdEmuList("D:\\VENC\\skype_list.txt");

    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}


//-----------------------------------------------------------------------------
// Test Case 16 :
//-----------------------------------------------------------------------------
UINT32 _u4YSrcAddr, _u4CoarseSw, _u4CoarseHw;
static VOID _VEncEmuImgrzComp(H264_EMU_HOST_T *ptHost, VENC_REZ_OBJ_T *ptRezObj)
{
    UINT32 u4Res, u4CoarseRes, u4YAddr, u4CoarseSw, u4CoarseHw;
    UINT32 u4FrmIdx;
    VENC_REZ_OBJ_T *ptSwRezObj, *ptHwRezObj;

    ASSERT(ptHost);

    Printf("Verify %dx%d => %dx%d ", ptHost->u4Width, ptHost->u4Height, ptRezObj->u4TgPitch, ptRezObj->u4TgH);
    u4Res = ptHost->u4Height * ptHost->u4Width;
    u4CoarseRes = ptRezObj->u4TgH * ptRezObj->u4TgPitch;
    switch (ptRezObj->ePixFmt)
    {
    default:
        printf("in unknown mode\n");
        ASSERT(0);
        break;
    case VENC_PIX_FMT_NV12:
        printf("in raster scan\n");
        break;
    case VENC_PIX_FMT_B16X32:
        printf("in 16x32 block\n");
        break;
    case VENC_PIX_FMT_B64X32:
        printf("in 64x32 block\n");
        break;
    }

    if (_u4YSrcAddr == (UINT32)NULL)
    {
        _u4YSrcAddr = BSP_AllocAlignedDmaMemory(1280*720, 2048);
    }
    u4YAddr = _u4YSrcAddr;

    if (_u4CoarseSw == (UINT32)NULL)
    {
        _u4CoarseSw = BSP_AllocAlignedDmaMemory(1280*720, 2048);
    }
    u4CoarseSw = _u4CoarseSw;

    if (_u4CoarseHw == (UINT32)NULL)
    {
        _u4CoarseHw = BSP_AllocAlignedDmaMemory(1280*720, 2048);
    }
    u4CoarseHw = _u4CoarseHw;
    {
        UINT8 *pu1Buf = (UINT8 *)VIRTUAL(u4CoarseHw);
        UINT32 i;
        for (i = 0; i < 16; i++)
        {
            pu1Buf[i] = i;
        }
    }
    HalInvalidateDCacheMultipleLine(VIRTUAL(u4YAddr), 1280*720);
    HalInvalidateDCacheMultipleLine(VIRTUAL(u4CoarseSw), 1280*720);
    HalInvalidateDCacheMultipleLine(VIRTUAL(u4CoarseHw), 1280*720);

    ASSERT(u4YAddr);
    ASSERT(u4CoarseSw);
    ASSERT(u4CoarseHw);

    ptSwRezObj = VencCreate68SwRezObject();
    ptHwRezObj = VencCreateRezObject();
    ASSERT(ptSwRezObj);
    ASSERT(ptHwRezObj);

    ptSwRezObj->ePixFmt    = ptHwRezObj->ePixFmt    = ptRezObj->ePixFmt;
    ptSwRezObj->u4SrcH     = ptHwRezObj->u4SrcH     = ptRezObj->u4SrcH;
    ptSwRezObj->u4SrcW     = ptHwRezObj->u4SrcW     = ptRezObj->u4SrcW;
    ptSwRezObj->u4SrcPitch = ptHwRezObj->u4SrcPitch = ptRezObj->u4SrcPitch;
    ptSwRezObj->u4TgH      = ptHwRezObj->u4TgH      = ptRezObj->u4TgH;
    ptSwRezObj->u4TgW      = ptHwRezObj->u4TgW      = ptRezObj->u4TgW;
    ptSwRezObj->u4TgPitch  = ptHwRezObj->u4TgPitch  = ptRezObj->u4TgPitch;

    ASSERT(ptSwRezObj->Init);
    ASSERT(ptHwRezObj->Init);
    ptSwRezObj->Init(ptSwRezObj);
    ptHwRezObj->Init(ptHwRezObj);
    for (u4FrmIdx = 0; u4FrmIdx < ptHost->u4FrmNo; ++u4FrmIdx)
    {
        x_sprintf(ptHost->szFilePathPtr, "_%08d.y", u4FrmIdx);
#ifdef USB_SUPPORT
        _VEncLoadUsbFile(ptHost->szFilePath, u4YAddr);
#else
        _VEncLoadFile(ptHost->szFilePath, u4YAddr);
#endif
        //Printf("Load Y:addr0x%8x, %s\n", u4YAddr, ptHost->szFilePath);
        HalFlushDCacheMultipleLine(VIRTUAL(u4YAddr), u4Res);

        ptSwRezObj->Resize(ptSwRezObj, u4YAddr, u4CoarseSw);
        ptHwRezObj->Resize(ptHwRezObj, u4YAddr, u4CoarseHw);
        HalInvalidateDCacheMultipleLine(VIRTUAL(u4CoarseHw), u4CoarseRes);
        ASSERT(x_memcmp((VOID *)u4CoarseSw, (VOID *)u4CoarseHw, u4CoarseRes) == 0);
    }

    VencDelete68SwRezObject(ptSwRezObj);
    VencDeleteRezObject(ptHwRezObj);

    Printf("Compare passed!!\n");
}

VOID _VEncCmdEmuImgrz(UINT8 *pu1FPathTempBuf, CHAR *pcFListPathName)
{
    VENC_REZ_OBJ_T tRezObj;
    H264_MMR_PARAM_T tMMR;
    UINT32 BUF_MMR;
    H264_EMU_HOST_T *ptHost = &rHost;
    CFGFILE *ptCfgFile = &_rCfgFile;

    Printf("Loading File path\n%s...\n", pcFListPathName);
    _VEncLoadFile(pcFListPathName, (UINT32)pu1FPathTempBuf);
    getCFGPathfile(pu1FPathTempBuf, ptCfgFile);

    BUF_MMR = _GetMMRBuffer();
    _VEncLoadFile(ptCfgFile->MMRFileName, BUF_MMR);
    _VEncEmuLoadConfig((CHAR *)BUF_MMR, &tMMR);

    x_strcpy(ptHost->szFilePath, ptCfgFile->InputFileName);
    ptHost->szFilePathPtr  = ptHost->szFilePath + x_strlen(ptHost->szFilePath);
    ptHost->u4ErrCnt = 0;
    ptHost->u4FrmNo  = (tMMR.mmr_num_of_b_frm_2 + 1) * tMMR.dwNumOfIandPFrames;
    ptHost->u4FrmCnt = 0;
    ptHost->u4Pitch= tMMR.mmr_pitch_width_in_pels;
    ptHost->u4Width  = (tMMR.mmr_image_width_div16_7 << 4) - tMMR.mmr_image_crop_right_4;
    ptHost->u4Height = (tMMR.mmr_image_height_div16_6 << 4) - tMMR.mmr_image_crop_bottom_5;

    tRezObj.ePixFmt   = (tMMR.mmr_source_scan_type_2 == 0) ? VENC_PIX_FMT_NV12 : ((tMMR.mmr_source_scan_type_2 == 1) ? VENC_PIX_FMT_B16X32 : VENC_PIX_FMT_B64X32);
    tRezObj.u4SrcH    = ptHost->u4Height;
    tRezObj.u4SrcW    = ptHost->u4Width;
    tRezObj.u4SrcPitch= ptHost->u4Pitch;
    tRezObj.u4TgH     = tMMR.mmr_coarse_image_height_div8_5 <<3;
    tRezObj.u4TgW     = tMMR.mmr_coarse_image_width_div8_6  << 3;
    tRezObj.u4TgPitch = VENC_ALIGN_MASK(tRezObj.u4TgW, 15);

    _VEncEmuImgrzComp(ptHost, &tRezObj);
}

extern VOID _VEncCmdEmuImgrzList(const CHAR *pcFileList);
static INT32 _VEncEmuTestCase16(INT32 i4Argc, const CHAR ** szArgv)
{
    Printf("\n");
    Printf("***************Test Case 16*******************\n");
    Printf("IMGRZ Test\n");

    _VEncCmdEmuImgrzList("D:\\VENC\\rz_list.txt");

    UNUSED(i4Argc);
    UNUSED(szArgv);
    return 0;
}

#endif


