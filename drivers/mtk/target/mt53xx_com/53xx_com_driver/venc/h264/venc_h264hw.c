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
 * $RCSfile: venc_h264hw.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file venc_h264hw.c
 *  Implementation of HW setting and Interrupt Service Routine of H.264 Encoder.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_ckgen.h"
#include "venc_drvif.h"
#include "venc_h264api.h"
#include "venc_h264hw.h"
#include "venc_h264_reg.h"
#ifdef VENC_EMU
#include "venc_h264_testsuit.h"
#endif

#if defined(CC_MT5880)
#include "PD_Def.h"
#endif

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------
#ifdef VENC_EMU
#define OLD_RC_ROM_CODE
#endif

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef struct {
    UINT32 u4IntBit;
    VOID (*pfnIsr)(VOID);
} VENC_INT_T;


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define VENC_CKCFG (0x2E8)

#if defined(CC_MT5396) // clock of 96 is changed to 216MHz due to E5 SLT fail rate is high
#define VENC_CK_SEL (2) // 432/216 MHz
#elif defined (CC_MT5398) || defined(CC_MT5880) // reduce clock for power saving
#define VENC_CK_SEL (2) // 432/216 MHz
#else
#define VENC_CK_SEL (1) // 480/240 MHz
#endif

#define VENC_H264_VECTOR VECTOR_VENC
#define VENC_H264_ISR_TIMEOUT 50


#ifdef OLD_RC_ROM_CODE

#ifdef VENC_MT8580_PATTERN
#define RC_ROM_CODE_FILE "rcpu_ram_code_2011_04_12.dat"
#else
//#define RC_ROM_CODE_FILE "rcpu_ram.dat24_emu_mp_vga"
#define RC_ROM_CODE_FILE "rcpu_ram.dat24_emu"
#endif

#else
//#define RC_ROM_CODE_FILE "rcpu_ram_code_2011_04_12.dat"
#define RC_ROM_CODE_FILE "rcpu_ram_code_2012_08_22.dat"
#endif


#define _HAL_DBG(fmt...)     LOG(6, fmt)
#define _HAL_FUNC_BEGIN()    _HAL_DBG("_VENC<HAL>::func Begin:%s\n", __FUNCTION__)
#define _HAL_FUNC_END(ret)   _HAL_DBG("_VEHC<HAL>::func end:  %s, ret:%d\n", __FUNCTION__, ret)
#define _HAL_FUNC_ENTER()    _HAL_DBG("_VENC<HAL>::func Enter:%s\n", __FUNCTION__)
#define _HAL_FUNC_LEAVE()    _HAL_DBG("_VENC<HAL>::func Leave:%s\n", __FUNCTION__)


//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static function forward declarations
//-----------------------------------------------------------------------------
static VOID _H264IsrInit(VOID);
static VOID _H264IsrStop(VOID);
static VOID _H264EncIsr(VOID);
static VOID _H264RCIsr(VOID);
static VOID _H264MBBitIsr(VOID);
static VOID _H264FrmBitIsr(VOID);
static VOID _H264CSIsr(VOID);
inline VOID _H264PreLoadRc(VOID);
inline VOID _H264LoadRcDone(VOID);


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static HANDLE_T _hVEncH264Sema = NULL_HANDLE;
static HANDLE_T _hVEncRcSema   = NULL_HANDLE;


// ======== Rate Control Rom Code definition ========
static const UINT32 _u4DefaultRCRomCode[] =
{
#include RC_ROM_CODE_FILE
};

static const UINT32 _u4RCRomCodeLen = sizeof(_u4DefaultRCRomCode)/sizeof(_u4DefaultRCRomCode[0]);
#define UCODE_LENGTH_ADD8 _u4RCRomCodeLen


//ISR SETTINGS
static const VENC_INT_T _arVEncInt[] =
{
    {BITS_ENC_SPS_INT,    _H264EncIsr},
    {BITS_ENC_PPS_INT,    _H264EncIsr},
    {BITS_ENC_FRM_INT,    _H264EncIsr},
    {BITS_ENC_UPDIP_INT,  NULL},
    {BITS_RC_INT,         _H264RCIsr},
    {BITS_MB_BIT_FLAG,    _H264MBBitIsr},
    {BITS_FRAME_BIT_FLAG, _H264FrmBitIsr},
    {BITS_ENC_CS_INT,     _H264CSIsr},
    {0, NULL}
};



//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

VOID _H264HalDumpRegs(VOID)
{
    UINT32 i, u4Base = VENC_H264_BASE;

    Printf("VENC_H264 reg dump\n");

    for (i = 0; i < 0x210; i += 16)
    {
        Printf("0x%08x: %08x %08x %08x %08x\n",
            u4Base + i,
            IO_READ32(u4Base, i),
            IO_READ32(u4Base, i + 4),
            IO_READ32(u4Base, i + 8),
            IO_READ32(u4Base, i + 12)
            );
    }
}

VOID _H264HalDumpInfo(VENC_OBJECT_T *this)
{
    H264_CTRL_T *ptCtrl;
    ptCtrl = (H264_CTRL_T *)this->hCtrl;
    Printf("InitBstSa/Sz/Ea = (0x%08x / 0x%08x / 0x%08x)\n", ptCtrl->u4BsSa, ptCtrl->u4BsSz, ptCtrl->u4BsEa);
    _H264HalDumpRegs();
}

static VOID _H264IsrStop(VOID)
{
#if 0
    x_os_isr_fct pfnOldIsr;
    // dereg ISR
    VERIFY(x_reg_isr(VENC_H264_VECTOR, NULL, &pfnOldIsr) == OSR_OK);
#endif
}

static VOID _H264EncIsr(VOID)
{
    VERIFY(x_sema_unlock(_hVEncH264Sema) == OSR_OK);
    //Printf("_H264EncIsr\n");
}

static VOID _H264RCIsr(VOID)
{
    VERIFY(x_sema_unlock(_hVEncRcSema) == OSR_OK);
}

static VOID _H264MBBitIsr(VOID)
{
//    LOG(3, "H.264 Video Encoder Warning: the coded bits of one MB are larger than 3200 bits\n");
}


static VOID _H264FrmBitIsr(VOID)
{
//    LOG(3, "H.264 Video Encoder Warning: The coded bits of one frame are larger than MAX_PIC_BITSTREAM_BYTE_CNT bits\n");
}


static VOID _H264CSIsr(VOID)
{
    // only occur when Two stage Control
    LOG(1, "H.264 Video Encoder Warning: CS Interrupt\n");
}

inline static UINT32 _H264HalGetIntReg(VOID)
{
    return IO_VENC_H264_READ_REG(VIDEO_HW_INTERRUPT) & 0x00FF0000;
}


inline static VOID _H264HalClearIntBit(UINT32 u4Bit)
{
    // Clear Interrupt Bit.
    // CAUTION: DO NOT USE IO_VENC_H264_READ_BITS() TO SET INTERRUPT REGISTER SINCE IT WILL LET ALL INTERRUPT CLEAR.
    IO_VENC_H264_WRITE_REG(VIDEO_HW_INTERRUPT, u4Bit);
}

static VOID _VEncH264Isr(UINT16 u2Vector)
{
    UINT32 u4Reg, i;

    if (VENC_H264_VECTOR != u2Vector)
    {
        return;
    }

    u4Reg = _H264HalGetIntReg();

    for (i = 0; i < sizeof(_arVEncInt) / sizeof(_arVEncInt[0]); i++)
    {
        if (u4Reg & _arVEncInt[i].u4IntBit)
        {
            if (_arVEncInt[i].pfnIsr)
            {
                _arVEncInt[i].pfnIsr();
            }
            else
            {
                LOG(3, "Interrupt bit 0x%08x has no ISR\n", _arVEncInt[i].u4IntBit);
            }
            _H264HalClearIntBit(_arVEncInt[i].u4IntBit);
        }
    }

}

static BOOL _fgIsrInit = FALSE; // only register isr once
static VOID _H264IsrInit(VOID)
{
    if (!_fgIsrInit)
    {
        x_os_isr_fct pfnOldIsr;

#if 0 // x_reg_isr will call this
        BIM_EnableIrq(VENC_H264_VECTOR);
#endif

        // reg ISR
        VERIFY(x_reg_isr(VENC_H264_VECTOR, _VEncH264Isr, &pfnOldIsr) == OSR_OK);

        _fgIsrInit = TRUE;
    }
}

inline VOID _H264PreLoadRc(VOID)
{
    x_sema_lock(_hVEncRcSema, X_SEMA_OPTION_NOWAIT);
}
inline VOID _H264LoadRcDone(VOID)
{
    VERIFY(x_sema_lock(_hVEncRcSema, X_SEMA_OPTION_WAIT) == OSR_OK);
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
VOID _VENC_H264_HalPreStart(VENC_OBJECT_T *this)
{
    H264_CTRL_T *pCtrl;
    UINT32 *pu4RCA;
    UINT32 u4j, u4i;
    UINT32 u4Val;

    _HAL_FUNC_ENTER();

    ASSERT(this);

    if (NULL_HANDLE == _hVEncH264Sema)
    {
        VERIFY(x_sema_create(&_hVEncH264Sema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
    }

    if (NULL_HANDLE == _hVEncRcSema)
    {
        VERIFY(x_sema_create(&_hVEncRcSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
    }

    LOG(3, "load RC: %s\n", RC_ROM_CODE_FILE);
    pCtrl = (H264_CTRL_T *)this->hCtrl;
    pu4RCA = (UINT32 *)VIRTUAL(pCtrl->u4RomCodeAddr);
    pCtrl->u4RCUAddr = pCtrl->u4RomCodeAddr;
    pCtrl->u4RCUCodeSize = UCODE_LENGTH_ADD8;
    for (u4j = u4i = 0; (u4i >> 1) < (UCODE_LENGTH_ADD8); u4i += 2, u4j++)
    {
        u4Val = _u4DefaultRCRomCode[u4j];
        *(pu4RCA + u4i + 1) = ((u4Val & 0x000000FF) << 24)
            | ((u4Val & 0x0000FF00) << 8)
            | ((u4Val & 0x00FF0000) >> 8)
            | ((u4Val & 0xFF000000) >> 24);
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)pu4RCA, UCODE_LENGTH_ADD8 * 8);

    _HAL_FUNC_LEAVE();

}

VOID _VENC_H264_HalStart(VENC_OBJECT_T *this)
{
    H264_MMR_PARAM_T *pMMR;
    H264_CTRL_T *pCtrl;
    UINT32 u4Val;

    _HAL_FUNC_ENTER();

    ASSERT(this);

    pMMR  = (H264_MMR_PARAM_T *)this->hParam;
    pCtrl = (H264_CTRL_T *)this->hCtrl;

#if defined(CC_MT5880)
    mcSET_RISC_REG(0xE20, 0xff03);
#endif

    //reg interrupt
    _H264IsrInit();

     //HW CLOCK
#if defined(CC_MT5395) || defined(CC_MT5396) || defined(CC_MT5368)
    IO_WRITE32(CKGEN_BASE, VENC_CKCFG, VENC_CK_SEL);
#elif defined(CC_MT5398)
    vIO32Write4B(CKGEN_REG_VENC_CKCFG, VENC_CK_SEL);
#elif defined(CC_MT5880)
    vIO32Write4B(CKGEN_VENC_CKCFG, VENC_CK_SEL);
    //vIO32Write4B(0xD370, VENC_CK_SEL);
#elif defined(CC_MT5399)
    //mt5399 venc driver is in venc_v2 folder
#else
#error "ckgen is not defined"
#endif

    //HW Enable
    IO_VENC_H264_WRITE_BITS(VIDEO_HW_MODE_SEL, SW_RST, 0);
    IO_VENC_H264_WRITE_BITS(VIDEO_HW_MODE_SEL, SW_RST, 1);

    IO_VENC_H264_WRITE_BITS(VIDEO_HW_MODE_SEL, CE, 1);
    IO_VENC_H264_WRITE_BITS(VIDEO_HW_MODE_SEL, DEC_NOBSM_EN, 0);
    IO_VENC_H264_WRITE_BITS(VIDEO_HW_MODE_SEL, DM_WDONE_EN, 1);
    IO_VENC_H264_WRITE_BITS(VIDEO_HW_MODE_SEL, MPEG4, 0);

#if 1
    // Make dram behavior of cobra identical to previous IC
    // Please refer to cobra function description for details
    IO_VENC_H264_WRITE_BITS(DLA_BYPASS_MODE, Enable_last, 1);
#endif

    IO_VENC_H264_WRITE_BITS(VIDEO_IMAGE_INFO_2, insert_keyframe, 0);

    //SOURCE FORAMT
    IO_VENC_H264_WRITE_BITS(VIDEO_SOURCE_INFO, source_scan_type, pMMR->mmr_source_scan_type_2);
    u4Val = pMMR->mmr_pitch_width_in_pels >> 4;
    IO_VENC_H264_WRITE_BITS(VIDEO_SOURCE_INFO, pitch_width_div16, u4Val);


    //BUFFER
    IO_VENC_H264_WRITE_BITS(VIDEO_UPPER_INFO_MB_DRAM_ADDR, UPPER_INFO_MB_DRAM_ADDR, pCtrl->u4UpperInfoMBAddr >> 3);
    IO_VENC_H264_WRITE_BITS(VIDEO_COL_INFO_DRAM_ADDR, COL_INFO_DRAM_ADDR, pCtrl->u4ColInfoAddr >> 3);
    IO_VENC_H264_WRITE_BITS(VIDEO_CS_INFO_DRAM_ADDR, CS_INFO_DRAM_ADDR, pCtrl->u4CSInfoAddr >> 3);
    IO_VENC_H264_WRITE_BITS(VIDEO_BITSTREAM_BUF_DRAM_INI_ADDR, BITSTREAM_BUF_DRAM_INI_ADDR, (pCtrl->u4BsSa) >> 3);
    IO_VENC_H264_WRITE_BITS(VIDEO_BITSTREAM_BUF_BYTE_SIZE, BITSTREAM_BUF_BYTE_SIZE, pCtrl->u4BsSz);

    u4Val =  ((pCtrl->u4BsSa) >> 3);
    IO_VENC_H264_WRITE_REG(BITSTREAM_BUF_DRAM_START_READ_ADDR, u4Val);
    u4Val |= (1UL << 31);
    IO_VENC_H264_WRITE_REG(UPDATE_BITSTREAM_BUF_DRAM_START_WRITE_ADDR, u4Val);

#ifdef DRAM_CHANNEL_AB_TEST
    //--Reconstruct frame buffer
    IO_VENC_H264_WRITE_BITS(VIDEO_FRM_REC0_Y_DRAM_ADDR,  FRM_REC0_Y_DRAM_ADDR, ((pCtrl->u4YFrmBuffAddr[0] | 0x40000000) >> 3));
    IO_VENC_H264_WRITE_BITS(VIDEO_FRM_REC0_UV_DRAM_ADDR, FRM_REC0_UV_DRAM_ADDR,((pCtrl->u4UVFrmBuffAddr[0] | 0x40000000) >> 3));
    IO_VENC_H264_WRITE_BITS(VIDEO_FRM_REC1_Y_DRAM_ADDR,  FRM_REC1_Y_DRAM_ADDR, ((pCtrl->u4YFrmBuffAddr[1] | 0x40000000) >> 3));
    IO_VENC_H264_WRITE_BITS(VIDEO_FRM_REC1_UV_DRAM_ADDR, FRM_REC1_UV_DRAM_ADDR,((pCtrl->u4UVFrmBuffAddr[1] | 0x40000000) >> 3));
#else
    //--Reconstruct frame buffer
    IO_VENC_H264_WRITE_BITS(VIDEO_FRM_REC0_Y_DRAM_ADDR,  FRM_REC0_Y_DRAM_ADDR, ((pCtrl->u4YFrmBuffAddr[0]) >> 3));
    IO_VENC_H264_WRITE_BITS(VIDEO_FRM_REC0_UV_DRAM_ADDR, FRM_REC0_UV_DRAM_ADDR,((pCtrl->u4UVFrmBuffAddr[0]) >> 3));
    IO_VENC_H264_WRITE_BITS(VIDEO_FRM_REC1_Y_DRAM_ADDR,  FRM_REC1_Y_DRAM_ADDR, ((pCtrl->u4YFrmBuffAddr[1]) >> 3));
    IO_VENC_H264_WRITE_BITS(VIDEO_FRM_REC1_UV_DRAM_ADDR, FRM_REC1_UV_DRAM_ADDR,((pCtrl->u4UVFrmBuffAddr[1]) >> 3));
#endif

    //dram request
    IO_VENC_H264_WRITE_REG(VENC_DRAM_LARB_CFG1, 0x1000);
    IO_VENC_H264_WRITE_REG(VENC_DRAM_LARB_CFG1, 0);
    IO_VENC_H264_WRITE_REG(VENC_DRAM_LARB_CFG1, 0x1000000);
    IO_VENC_H264_WRITE_REG(VENC_DRAM_LARB_CFG0, 0x0000000f);
    IO_VENC_H264_WRITE_REG(VIDEO_SRAM_PD_CONTROL, 0x9FFB0000);
    IO_VENC_H264_WRITE_REG(VIDEO_SRAM_PD_CONTROL, 0x1FFB0000);
    IO_VENC_H264_WRITE_REG(VIDEO_SRAM_PD_CONTROL, 0x9FFB0000);

    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_0, DRAM_PAGE_SIZE, pMMR->mmr_dram_page_size_div256_7);

    //interrupt
    IO_VENC_H264_WRITE_REG(VIDEO_CODEC_INTM, 0);
    u4Val = 0//BITS_FCS_INTM, // used for TWO_STAGE_CONTROL
        | BITS_RCU_INTM
        //| BITS_ENC_UPDIP_INTM
        | BITS_ENC_FRM_INTM
        | BITS_ENC_PPS_INTM
        | BITS_ENC_SPS_INTM;
    IO_VENC_H264_WRITE_REG(VIDEO_CODEC_INTM, u4Val);


    //Enc Parameters
    IO_VENC_H264_WRITE_BITS(VIDEO_HW_MODE_SEL, MAIN_PROFILE, pMMR->mmr_h264_profile_8);
    IO_VENC_H264_WRITE_BITS(VIDEO_HW_MODE_SEL, CABAC, pMMR->mmr_use_cabac_1);
    IO_VENC_H264_WRITE_BITS(VIDEO_IMAGE_INFO_1, H264_LEVEL, pMMR->mmr_h264_level_6);

    IO_VENC_H264_WRITE_BITS(VIDEO_IMAGE_INFO_1, COARSE_IMAGE_WIDTH_DIV8, pMMR->mmr_coarse_image_width_div8_6);
    IO_VENC_H264_WRITE_BITS(VIDEO_IMAGE_INFO_1, COARSE_IMAGE_HEIGHT_DIV8, pMMR->mmr_coarse_image_height_div8_5);
    IO_VENC_H264_WRITE_BITS(VIDEO_IMAGE_INFO_0, COARSE_IMAGE_CROP_BOTTOM, pMMR->mmr_coarse_image_crop_bottom_3);

    //u4NumBlk = u4WidthDiv8 * u4HeightDiv8;
    IO_VENC_H264_WRITE_BITS(VIDEO_IMAGE_INFO_1, COARSE_IMAGE_NUM_BLK, pMMR->mmr_num_blk_in_coarse_frm_10);
    //(8192 + (u4NumBlk / 2)) / u4NumBlk
    IO_VENC_H264_WRITE_BITS(VIDEO_COARSE_MULTIPLIER, COARSE_MULTIPLIER, pMMR->mmr_coarse_multiplier_7);

    IO_VENC_H264_WRITE_BITS(VIDEO_IMAGE_INFO_0, IMAGE_WIDTH_DIV16, pMMR->mmr_image_width_div16_7);
    IO_VENC_H264_WRITE_BITS(VIDEO_IMAGE_INFO_0, IMAGE_HEIGHT_DIV16, pMMR->mmr_image_height_div16_6); //need MB pair, but assign MB number
    IO_VENC_H264_WRITE_BITS(VIDEO_IMAGE_INFO_0, IMAGE_NUM_MB, pMMR->mmr_num_mb_in_frm_12);//need MB pair.
    IO_VENC_H264_WRITE_BITS(VIDEO_IMAGE_INFO_1, IMAGE_CROP_BOTTOM, pMMR->mmr_image_crop_bottom_5);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_0, IMAGE_CROP_RIGHT, pMMR->mmr_image_crop_right_4);
    IO_VENC_H264_WRITE_BITS(VIDEO_HW_MODE_SEL, MBAFF, pMMR->mmr_use_mbaff_1);


    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_0, PERIOD_IDR_FRM, pMMR->mmr_period_idr_frm_10);
    IO_VENC_H264_WRITE_BITS(VIDEO_HW_MODE_SEL, PERIOD_I_FRM, pMMR->mmr_period_i_frm_10);
    IO_VENC_H264_WRITE_BITS(VIDEO_HW_MODE_SEL, NUM_B_FRM, pMMR->mmr_num_of_b_frm_2);
    IO_VENC_H264_WRITE_BITS(VIDEO_HW_MODE_SEL_2, STUFF_EN, pMMR->use_stuff_byte);
    //u4Val = (0x3FFFFF < prEncParam->u4MaxPicByteCnt) ? 0x3FFFFF : prEncParam->u4MaxPicByteCnt;
    IO_VENC_H264_WRITE_BITS(VIDEO_MAX_PIC_BYTECNT_ADDR, MAX_PIC_BITSTREAM_BYTE_CNT, pMMR->mmr_max_pic_bitstream_byte_cnt_22);

    //VUI settings
    IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, EnableVUISupport, pMMR->mmr_enable_vui_support_1);
    if (pMMR->mmr_enable_vui_support_1)
    {
        UINT32 u4_VUI_timing_info_present_flag = pMMR->mmr_vui_timing_info_present_flag;
        UINT32 u4_VUI_nal_hrd_parameters_present_flag = pMMR->write_pic_timing;
        //aspect_ratio_info_present_flag
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_aspect_ratio_info_present_flag, pMMR->mmr_vui_aspect_ratio_info_present_flag);
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_aspect_ratio_idc, pMMR->mmr_vui_aspect_ratio_idc);
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_2, VUI_sar_width, pMMR->mmr_vui_sar_width);
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_2, VUI_sar_height, pMMR->mmr_vui_sar_height);

        //overscan_info_present_flag
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_overscan_info_present_flag, 0);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_overscan_appropriate_flag, prEncParam->vui_overscan_appropriate_flag);

        //video_signal_type_present_flag
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_video_signal_type_present_flag, 0);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_video_format, prEncParam->vui_video_format);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_video_full_range_flag, prEncParam->vui_video_full_range_flag);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_1, VUI_colour_primaries, prEncParam->vui_colour_primaries);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_colour_description_present_flag, prEncParam->vui_colour_description_present_flag);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_1, VUI_transfer_characteristics, prEncParam->vui_transfer_characteristics);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_1, VUI_matrix_coefficients, prEncParam->vui_matrix_coefficients);

        //chroma_loc_info_present_flag
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_chroma_location_info_present_flag, 0);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_chroma_sample_loc_type_top_field, prEncParam->vui_chroma_sample_loc_type_top_field);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_chroma_sample_loc_type_bottom_field, prEncParam->vui_chroma_sample_loc_type_bottom_field);

        //timing_info_present_flag
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_timing_info_present_flag, u4_VUI_timing_info_present_flag);
        if (u4_VUI_timing_info_present_flag)
        {
            IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_3, VUI_num_units_in_tick, pMMR->mmr_vui_num_units_in_tick);
            IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_4, VUI_time_scale, pMMR->mmr_vui_time_scale);
            IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_fixed_frame_rate_flag, 0);
        }

        //nal_hrd_parameters_present_flag
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_nal_hrd_parameters_present_flag, u4_VUI_nal_hrd_parameters_present_flag);
        if (u4_VUI_nal_hrd_parameters_present_flag)
        {
            //cpb_cnt_minus1 is fix to 0 now
            IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_5, VUI_nal_bit_rate_scale, 4); //bit_rate_scale
            IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_5, VUI_nal_cpb_size_scale, 6); //cpb_size_scale
            IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_6, VUI_nal_bit_rate_value_minus1, 16405); //((14000 * 1200) >> (bit_rate_scale + 6)) - 1 // hardcode for level 3.1
            IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_6, VUI_nal_cpb_size_value_minus1, 16405); //((14000 * 1200) >> (cpb_size_scale + 4)) - 1 // hardcode for level 3.1
            IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_5, VUI_nal_vbr_cbr_flag, 0);
            IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_5, VUI_nal_initial_cpb_removal_delay_length_minus1, 0);
            IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_5, VUI_nal_cpb_removal_delay_length_minus1, 7); //cpb_removal_delay_length_minus1
            IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_5, VUI_nal_dpb_output_delay_length_minus1, 7); //dpb_output_delay_length_minus1
            IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_5, VUI_nal_time_offset_length, 0);
        }

        //vcl_hrd_parameters_present_flag
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_vcl_hrd_parameters_present_flag, 0);
        //cpb_cnt_minus1 is fix to 0 now
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_7, VUI_vcl_bit_rate_scale, prEncParam->vui_vcl_bit_rate_scale);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_7, VUI_vcl_cpb_size_scale, prEncParam->vui_vcl_cpb_size_scale);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_7, VUI_vcl_vbr_cbr_flag, prEncParam->vui_vcl_vbr_cbr_flag);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_7, VUI_vcl_initial_cpb_removal_delay_length_minus1, prEncParam->vui_vcl_initial_cpb_removal_delay_length_minus1);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_7, VUI_vcl_cpb_removal_delay_length_minus1, prEncParam->vui_vcl_cpb_removal_delay_length_minus1);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_7, VUI_vcl_dpb_output_delay_length_minus1, prEncParam->vui_vcl_dpb_output_delay_length_minus1);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_7, VUI_vcl_time_offset_length, prEncParam->vui_vcl_time_offset_length);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_8, VUI_vcl_bit_rate_value_minus1, prEncParam->vui_vcl_bit_rate_value_minus1);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_8, VUI_vcl_cpb_size_value_minus1, prEncParam->vui_vcl_cpb_size_value_minus1);

        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_low_delay_hrd_flag, 0);

        //pic_struct_present_flag
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_pic_struct_present_flag, pMMR->vui_pic_struct_present_flag);

        //bitstream_restriction_flag
        IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_bitstream_restriction_flag, 0);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_9, VUI_motion_vectors_over_pic_boundaries_flag, prEncParam->vui_motion_vectors_over_pic_boundaries_flag);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_9, VUI_max_bytes_per_pic_denom, prEncParam->vui_max_bytes_per_pic_denom);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_9, VUI_max_bits_per_mb_denom, prEncParam->vui_max_bits_per_mb_denom);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_9, VUI_log2_max_mv_length_vertical, prEncParam->vui_log2_max_mv_length_vertical);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_9, VUI_log2_max_mv_length_horizontal, prEncParam->vui_log2_max_mv_length_horizontal);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_9, VUI_num_reorder_frames, prEncParam->vui_num_reorder_frames);
        //IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_9, VUI_max_dec_frame_buffering, prEncParam->vui_max_dec_frame_buffering);
    }

    //DEBUG INFO. REG
    IO_VENC_H264_WRITE_BITS(CRC_CTRL, CRC_ENABLE_CTRL, pMMR->mmr_crc_check_enable_1);


    //ME Parameters
    IO_VENC_H264_WRITE_BITS(VIDEO_HW_MODE_SEL, COARSE_SEARCH, pMMR->mmr_use_coarse_search_1);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, P_SKIP, pMMR->mmr_en_p_interskip_1);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, P_16X16, pMMR->mmr_en_p_inter16x16_1);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, P_16X08, pMMR->mmr_en_p_inter16x08_1);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, P_08X16, pMMR->mmr_en_p_inter08x16_1);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, P_08X08, pMMR->mmr_en_p_inter08x08_1);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, B_DIRECT, pMMR->mmr_en_b_interdirect_1);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, B_16X16, pMMR->mmr_en_b_inter16x16_1);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, B_16X08, pMMR->mmr_en_b_inter16x08_1);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, B_08X16, pMMR->mmr_en_b_inter08x16_1);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, B_08X08, pMMR->mmr_en_b_inter08x08_1);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, P_SEARCH_H, pMMR->mmr_p_sr_h_div16_3);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, P_SEARCH_V, pMMR->mmr_p_sr_v_div16_2);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, B_SEARCH_H, pMMR->mmr_b_sr_h_div16_2);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, B_SEARCH_V, pMMR->mmr_b_sr_v_div16_2);


    IO_VENC_H264_WRITE_BITS(VIDEO_COARSE_MULTIPLIER, COARSE_P_SEARCH_H, pMMR->mmr_p_coarse_sr_h_div8_3);
    IO_VENC_H264_WRITE_BITS(VIDEO_COARSE_MULTIPLIER, COARSE_P_SEARCH_V, pMMR->mmr_p_coarse_sr_v_div8_2);
    IO_VENC_H264_WRITE_BITS(VIDEO_COARSE_MULTIPLIER, COARSE_B_SEARCH_H, pMMR->mmr_b_coarse_sr_h_div8_2);
    IO_VENC_H264_WRITE_BITS(VIDEO_COARSE_MULTIPLIER, COARSE_B_SEARCH_V, pMMR->mmr_b_coarse_sr_v_div8_2);


    //Rate Control Parameters
    IO_VENC_H264_WRITE_BITS(VIDEO_HW_MODE_SEL, EN_RATECONTROL, pMMR->mmr_use_rc_1);
    IO_VENC_H264_WRITE_BITS(VIDEO_HW_MODE_SEL, DUMP_CS_INFO,  pMMR->mmr_use_rc_1);
    IO_VENC_H264_WRITE_BITS(VIDEO_RATECONTROL_INFO_1, RC_CBR, pMMR->mmr_rc_use_cbr_1);

    IO_VENC_H264_WRITE_BITS(VIDEO_RATECONTROL_INFO_1, RC_TARGET_BIT_RATE, pMMR->mmr_rc_target_bit_rate_14);
    IO_VENC_H264_WRITE_BITS(VIDEO_RC_CTL, RC_FPS, this->u1Framerate);
    IO_VENC_H264_WRITE_BITS(VIDEO_RATECONTROL_INFO_1, RC_INI_QP, pMMR->mmr_rc_use_init_qp_1);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_0, QP_I_FRM, pMMR->mmr_quant_i_frm_6);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, QP_P_FRM, pMMR->mmr_quant_p_frm_6);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, QP_B_FRM, pMMR->mmr_quant_b_frm_6);
    IO_VENC_H264_WRITE_BITS(VIDEO_RC_CTL, PfrmQLimter, pMMR->mmr_p_frm_q_limiter_8);
    IO_VENC_H264_WRITE_BITS(VIDEO_RC_CTL, BfrmQLimter, pMMR->mmr_b_frm_q_limiter_8);

    IO_VENC_H264_WRITE_BITS(VIDEO_RC_CTL, AFPS, pMMR->mmr_rc_adaptive_fps);
    IO_VENC_H264_WRITE_BITS(VIDEO_RC_CTL, ATBR, pMMR->mmr_rc_adaptive_tbr);

    if (pMMR->mmr_use_rc_1 && pCtrl->u4RCUAddr!= 0)
    {
        ASSERT((pCtrl->u4RCUAddr & 0x7) == 0);
        _H264PreLoadRc();
        IO_VENC_H264_WRITE_BITS(VIDEO_RC_CODE_DRAM_ADDR, RC_CODE_DRAM_ADDR, (pCtrl->u4RCUAddr) >> 3);
        IO_VENC_H264_WRITE_BITS(VIDEO_RATECONTROL_INFO_1, RCU_CODE_LENGTH_ADD8, pCtrl->u4RCUCodeSize);
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, RCU_LD, 0);
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, RCU_LD, 1);
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, RCU_LD, 0);
        _H264LoadRcDone();
        //LOG(5, "VENC RC Rom Code Loaded\n");
    }


#ifdef VENC_EMU
    _H264EmuSettings(this);
#ifndef VENC_LIGHT_LOG
    _H264HalDumpInfo(this);
#endif
#endif

    _HAL_FUNC_LEAVE();
}


VOID _VENC_H264_HalRtReset(VENC_OBJECT_T *this)
{
    H264_MMR_PARAM_T *pMMR;

    _HAL_FUNC_ENTER();

    ASSERT(this);

    pMMR  = VENC_HANDLE_TO_OBJECT(this->hParam,H264_MMR_PARAM_T);

    //ME Parameters
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, B_16X08, pMMR->mmr_en_b_inter16x08_1);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, B_08X16, pMMR->mmr_en_b_inter08x16_1);
    IO_VENC_H264_WRITE_BITS(VIDEO_ENCODER_INFO_1, B_08X08, pMMR->mmr_en_b_inter08x08_1);

    IO_VENC_H264_WRITE_BITS(VIDEO_RATECONTROL_INFO_1, RC_TARGET_BIT_RATE, pMMR->mmr_rc_target_bit_rate_14);
    IO_VENC_H264_WRITE_BITS(VIDEO_RC_CTL, RC_FPS, this->u1Framerate);

    _HAL_FUNC_LEAVE();

}


#ifdef VENC_EMU
VOID _VENC_H264_HalRtResetBps(VENC_OBJECT_T *this, UINT32 u4Bps)
{
    H264_MMR_PARAM_T *pMMR;

    ASSERT(this);

    pMMR  = VENC_HANDLE_TO_OBJECT(this->hParam,H264_MMR_PARAM_T);

    if (pMMR->mmr_rc_target_bit_rate_14 != u4Bps)
    {
        Printf("skype set %dkbps => %dkbps\n", pMMR->mmr_rc_target_bit_rate_14, u4Bps);
        pMMR->mmr_rc_target_bit_rate_14 = u4Bps;
        IO_VENC_H264_WRITE_BITS(VIDEO_RATECONTROL_INFO_1, RC_TARGET_BIT_RATE, u4Bps);
    }
}

VOID _VENC_H264_HalRtResetFps(VENC_OBJECT_T *this, UINT32 u4Fps)
{
    ASSERT(this);

    if (this->u1Framerate != u4Fps)
    {
        Printf("skype set %dfps => %dfps\n", this->u1Framerate, u4Fps);
        this->u1Framerate = u4Fps;
        IO_VENC_H264_WRITE_BITS(VIDEO_RC_CTL, RC_FPS, u4Fps);
    }
}
#endif


VOID _VENC_H264_HalStop(VENC_OBJECT_T *this)
{
    _HAL_FUNC_ENTER();

    //DisableInt
    IO_VENC_H264_WRITE_REG(VIDEO_CODEC_INTM, 0);

    //dereg interrupt
    _H264IsrStop();

    //HW CLOCK

    _HAL_FUNC_LEAVE();
}


VOID _VENC_H264_HalSetStart(VideoCodecHalStartOpt_T eParam)
{
    switch (eParam)
    {
    default:
        LOG(0, "_VENC_H264_HalSetStart unknow\n");
        ASSERT(0);
        break;

    case VIDEOCODEC_HAL_STARTOPT_FIRST_SPS:
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, FIRST_SPS_FLAG, 1);
        break;

    case VIDEOCODEC_HAL_STARTOPT_DEBUG:
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, DEBUG_1, 1);
        break;

    case VIDEOCODEC_HAL_STARTOPT_COPY_B:
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, COPY_B, 1);
        break;

    case VIDEOCODEC_HAL_STARTOPT_ENCODE_DIP:
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, ENC_DIP, 1);
        break;

    case VIDEOCODEC_HAL_STARTOPT_ENCODE_RCULD:
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, RCU_LD, 1);
        break;

    case VIDEOCODEC_HAL_STARTOPT_ENCODE_SPS:
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, ENC_SPS, 1);
        break;

    case VIDEOCODEC_HAL_STARTOPT_ENCODE_PPS:
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, ENC_PPS, 1);
        break;

    case VIDEOCODEC_HAL_STARTOPT_ENCODE_FRAME:
    case VIDEOCODEC_HAL_STARTOPT_ENCODE_FINAL:
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, ENC_FRM, 1);
        break;
    }
}


VOID _VENC_H264_HalSetStartClear(VideoCodecHalStartOpt_T eParam)
{
    switch(eParam)
    {
    default:
        LOG(0, "_VENC_H264_HalSetStartClear unknow\n");
        ASSERT(0);
        break;

    case VIDEOCODEC_HAL_STARTOPT_FIRST_SPS:
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, FIRST_SPS_FLAG, 0);
        break;

    case VIDEOCODEC_HAL_STARTOPT_DEBUG:
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, DEBUG_1, 0);
        break;

    case VIDEOCODEC_HAL_STARTOPT_COPY_B:
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, COPY_B, 0);
        break;

    case VIDEOCODEC_HAL_STARTOPT_ENCODE_DIP:
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, ENC_DIP, 0);
        break;

    case VIDEOCODEC_HAL_STARTOPT_ENCODE_RCULD:
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, RCU_LD, 0);
        break;

    case VIDEOCODEC_HAL_STARTOPT_ENCODE_SPS:
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, ENC_SPS, 0);
        break;

    case VIDEOCODEC_HAL_STARTOPT_ENCODE_PPS:
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, ENC_PPS, 0);
        break;

    case VIDEOCODEC_HAL_STARTOPT_ENCODE_FRAME:
    case VIDEOCODEC_HAL_STARTOPT_ENCODE_FINAL:
        IO_VENC_H264_WRITE_BITS(VIDEO_CODEC_CONTROL, ENC_FRM, 0);
        break;
    }
}


VOID _VENC_H264_HalSetYUVAddr(UINT32 u4YAddr, UINT32 u4UVAddr, UINT32 u4Idx)
{
    IO_VENC_H264_WRITE_BITS(VIDEO_FRM_CURR_Y_DRAM_ADDR,  FRM_CURR_Y_DRAM_ADDR, (u4YAddr >> 3));
    IO_VENC_H264_WRITE_BITS(VIDEO_FRM_CURR_UV_DRAM_ADDR, FRM_CURR_UV_DRAM_ADDR,(u4UVAddr >> 3));
    UNUSED(u4Idx);
}

VOID _VENC_H264_HalSetYCoarseAddr(UINT32 u4Addr, UINT32 u4Idx)
{
    IO_VENC_H264_WRITE_BITS(VIDEO_COARSE_FRM_CUR_Y_DRAM_ADDR0, COARSE_FRM_CUR_Y_DRAM_ADDR,(u4Addr >> 3));
    UNUSED(u4Idx);
}

VOID _VEnc_H264_HalSetBrefCoarseAddr(UINT32 u4Coarse)
{
    IO_VENC_H264_WRITE_BITS(VIDEO_COARSE_FRM_RECB_Y_DRAM_ADDR0, COARSE_FRM_RECB_Y_DRAM_ADDR,(u4Coarse >> 3));
}

VOID _VEnc_H264_HalSetFrefCoarseAddr(UINT32 u4Coarse)
{
    IO_VENC_H264_WRITE_BITS(VIDEO_COARSE_FRM_RECF_Y_DRAM_ADDR0, COARSE_FRM_RECF_Y_DRAM_ADDR,(u4Coarse >> 3));
}

UINT32 _VENC_H264_HalGetOutputCurAddr(VOID)
{
    return IO_VENC_H264_READ_BITS(VIDEO_PIC_BITSTREAM_DRAM_BYTE_ADDR_DIV8, PIC_BITSTREAM_DRAM_BYTE_ADDR_DIV8) << 3;
}

UINT32 _VENC_H264_HalGetOutputCurSize(VOID)
{
    return IO_VENC_H264_READ_BITS(VIDEO_PIC_BITSTREAM_BYTE_CNT, PIC_BITSTREAM_BYTE_CNT);
}


VOID _VEnc_H264_HalSetBSWriteAddr(UINT32 u4Addr)
{
    u4Addr =  (1UL << 31) | (u4Addr >> 3);
    IO_VENC_H264_WRITE_REG(UPDATE_BITSTREAM_BUF_DRAM_START_WRITE_ADDR, u4Addr);
}

VOID _VEnc_H264_HalClearBSWriteEnableFlag(VOID)
{
    IO_VENC_H264_WRITE_BITS(UPDATE_BITSTREAM_BUF_DRAM_START_WRITE_ADDR, UBSMSWA, 0);
}


UINT32 _VEnc_H264_HalGetBSCurrWriteAddr(VOID)
{
    UINT32 u4Addr;
    u4Addr = IO_VENC_H264_READ_BITS(BITSTREAM_BUF_DRAM_CURRENT_WRITE_ADDR, BSMCWA);
    u4Addr <<= 3;

    return u4Addr;
}

UINT32 _VEnc_H264_HalGetBSReadAddr(VOID)
{
    UINT32 u4Addr;
    u4Addr = IO_VENC_H264_READ_BITS(BITSTREAM_BUF_DRAM_START_READ_ADDR, BSMSRA);
    u4Addr <<= 3;

    return u4Addr;
}

VOID _VEnc_H264_HalInsertKeyframeOnOff(UINT32 u4OnOff)
{
    IO_VENC_H264_WRITE_BITS(VIDEO_IMAGE_INFO_2, insert_keyframe, u4OnOff);
}

BOOL _VEnc_H264_HalIsInsertKeyframeOn(VOID)
{
    return (IO_VENC_H264_READ_BITS(VIDEO_IMAGE_INFO_2, insert_keyframe))? TRUE:FALSE;
}



VOID _VEnc_H264_HalSetBSReadAddr(UINT32 u4Addr)
{
    IO_VENC_H264_WRITE_BITS(BITSTREAM_BUF_DRAM_START_READ_ADDR, BSMSRA, u4Addr >> 3);
}

BOOL _VENC_H264_HalEncDone(VOID)
{
    #ifdef VENC_EMU
    {
    extern HANDLE_T _hEncFrmStart;
    if (_hEncFrmStart)
    {
        //Printf("notify _VENC_H264_HalEncDone\n");
        x_thread_delay(200);
        x_sema_unlock(_hEncFrmStart);
    }
    }
    #endif

    VERIFY(x_sema_lock(_hVEncH264Sema, X_SEMA_OPTION_WAIT) == OSR_OK);
    return TRUE;
}


// check dram local arb request queue empty
VOID _VENC_H264_WaitDramWriteDone(VOID)
{

}


UINT32 _VENC_H264_HalStuffbytes(VOID)
{
    return IO_VENC_H264_READ_BITS(VIDEO_STUFF_REPORT, STUFF_BYTE);
}


UINT32 _VENC_H264_HalGetECCRC(void)
{
    return IO_VENC_H264_READ_REG(CRC_EC_BITSTREAM);
}


// Frame encoding cycle count
UINT32 _VENC_H264_u4EncCycleCount(VOID)
{
    return IO_VENC_H264_READ_REG(VIDEO_CODEC_COUNTER0);
}

UINT32 _VENC_H264_u4EncUnlockH264Sema(VOID)
{
    VERIFY(x_sema_unlock(_hVEncH264Sema) == OSR_OK);

    return 0;
}

BOOL _VENC_H264_fgEncFifoFull(VOID)
{
    if (IO_VENC_H264_READ_BITS(RP_WP_STATUS, STALL) != 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

VOID _VEnc_H264_HalSetAspectRatioIdc(VENC_OBJECT_T *this)
{
    H264_MMR_PARAM_T *pMMR;

    ASSERT(this);
    pMMR  = VENC_HANDLE_TO_OBJECT(this->hParam,H264_MMR_PARAM_T);

    IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_0, VUI_aspect_ratio_idc, pMMR->mmr_vui_aspect_ratio_idc);
    IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_2, VUI_sar_width, pMMR->mmr_vui_sar_width);
    IO_VENC_H264_WRITE_BITS(VUI_PARAMETER_INFO_2, VUI_sar_height, pMMR->mmr_vui_sar_height);
}

#ifdef CC_SUPPORT_STR
void VENC_pm_str_suspend(void)
{
    VENC_HANDLE handle = VENC_NULL_HANDLE;
    UINT32 u4Ret;

    // check driver state
    VENC_FOR_EACH_HANDLE(handle)
    {
        LOG(0, "STR_WARNING: VENC(%p) is not stopped.", handle);

        //Stop
        u4Ret = VENC_Stop(handle);
        if (VENC_OK != u4Ret)
        {
            LOG(0, "Stop fail!\n");
        }

        //Close
        u4Ret = VENC_Close(handle);
        if (VENC_OK != u4Ret)
        {
            LOG(0, "Close fail!\n");
        }
    }

    VENC_CkgenOnOff(FALSE);
}


void VENC_pm_str_resume(void)
{
    VENC_CkgenOnOff(TRUE);
}
#endif

