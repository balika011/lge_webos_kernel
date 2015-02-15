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
 *-----------------------------------------------------------------------------
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: mpv_hw.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file mpv_hw.c
 *  This file contains hardware-related functions of MPV.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "x_lint.h"
#include "mpv_parser.h"
#include "mpv_decode.h"
#include "mpv_hw.h"
#include "mpv_type.h"
#include "mpv_debug.h"
#include "vdec_drvif.h"
#include "mpv_config.h"
#include "mpv_errcode.h"
LINT_EXT_HEADER_BEGIN
#include "x_ckgen.h"
#include "x_assert.h"
#include "x_os.h"
#include "x_mm_common.h"
LINT_EXT_HEADER_END


static HAL_TIME_T _rStartTime;
static HAL_TIME_T _rStopTime;
static HAL_TIME_T _rDeltaTime;
//static UINT32     u4FrameCount;    //used for print log


#ifdef MPV_SEMI_HOST
extern UCHAR *_pYaddr[MPV_MAX_ES];
extern UCHAR *_pCaddr[MPV_MAX_ES];
#endif
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define VLD_VECTOR        24
//89 = 25 +64;  #define VECTOR_VDEC_CORE1   (VECTOR_MISC2_BASE+25)  #define VECTOR_MISC2_BASE 64
#define VLD_VECTOR_CORE1  89

#define REG_BITNUM1    32
#define REG_BITNUM2    64

#define DBK_MIN_H    32

#if defined(CC_MT5363) && !defined(CC_MT5365) && !defined(CC_MT5395)
//#ifdef CC_MT5363

#define DBK_THQP0I     9
#define DBK_THQP0P     8
#define DBK_THQP0B     8
#define DBK_THQP0      12
#define DBK_THQP0I_C  9
#define DBK_THQP0P_C  8
#define DBK_THQP0B_C  8
#define DBK_THQP0_C   12

#define DBK_FTR_RATIO  4
#define DBK_QP1_Y     3
#define DBK_QP2_Y     11
#define DBK_QP3_Y     3
#define DBK_QP1_C     17
#define DBK_QP2_C     4
#define DBK_QP3_C     2
#define DBK_MB_FTR_CNT  96 //16x6
#define DBK_QP_MAX  31
#else
//for DBK
#define DBK_QP0     12
#define DBK_QP1     15
#define DBK_QP2     26
#define DBK_QP3     29
//I
#define DBK_QP0_I     9
#define DBK_QP1_I     12
#define DBK_QP2_I     23
#define DBK_QP3_I     26
//P
#define DBK_QP0_P     8
#define DBK_QP1_P     11
#define DBK_QP2_P     22
#define DBK_QP3_P     25
//B
#define DBK_QP0_B     8
#define DBK_QP1_B     11
#define DBK_QP2_B     22
#define DBK_QP3_B     25
#if 0

//for DBK test
#define DBK_QP0     2
#define DBK_QP1     4
#define DBK_QP2     6
#define DBK_QP3     8
//I
#define DBK_QP0_I     2
#define DBK_QP1_I     4
#define DBK_QP2_I     5
#define DBK_QP3_I     7
//P
#define DBK_QP0_P     2
#define DBK_QP1_P     4
#define DBK_QP2_P     5
#define DBK_QP3_P     8
//B
#define DBK_QP0_B     2
#define DBK_QP1_B     3
#define DBK_QP2_B     4
#define DBK_QP3_B     6
#endif
#endif

#define RR_RATIO_34    1

// there are two main control register for dbk
// 1. 0x200 to bypass (on/off) deblocking
// 2. 0x238 to enable filter for Y or C data.

// comes from mpv_hw.h, all about DBK
#if defined(CC_MT5363) && !defined(CC_MT5365) && !defined(CC_MT5395)
//#ifdef CC_MT5363
#define RW_MC_DBK_CTRL 0xC0
    #define DBK_EN      (0x1 << 0)
    #define DBK_2MBROW (0x1 << 1)    
    #define DBK_BW_SAVING (0x1 << 5)            
    #define DBK_DISABLE_ECO (0x1 << 6)       
    #define DBK_DISABLE_DELAY (0x1 << 7)          
    #define DBK_GROUP1 (0x1 << 9)                
#define RW_MC_DBK_PICSZ 0xC4
#define RW_MC_DBK_Y 0xC8
#define RW_MC_DBK_C 0xCC
#define RW_MC_DBK_THRSD 0xD0
#define RW_MC_DBK_QP_Y 0xD4
#define RW_MC_DBK_ADV_THRSD_Y 0xD8
#define WO_MC_DBK_DEC 0xDC
    #define DBK_START    (0x1 << 0)
#define RO_MC_DBK_STATUS 0xE0
#define RW_MC_DBK_DEBUG   0xE4
    #define OFST_EN (0x1 << 0)
    #define DEF_EN (0x1 << 1)
    #define FPF_EN (0x1 << 2)    
#define RO_MC_DBK_OFST_CNT 0xE8
#define RO_MC_DBK_DEFT_CNT 0xEC
#define RO_MC_DBK_4P_CNT 0xF0
#define RW_MC_DBK_QP_C 0xF4
#define RW_MC_DBK_ADV_THRSD_C 0xF8
#define RW_MC_DBK_INTEREST 0xFC
#elif defined(CC_MT5387) && !defined(CC_MT5363)
// nothing
#else //CC_MT5391 CC_MT5360
#define RW_MC_POSTEN 0x220
    #define POSTEN    (0x1 << 0)
    
#define RW_MC_DBK_Y 0x224
#define RW_MC_DBK_C 0x228

#define RW_MC_POST_WD 0x22C

#define RW_MC_DBK_CTRL 0x238
    #define DBKC      (0x1 << 0)
    #define DBKY    (0x1 << 1)
    
#define RW_MC_DBLK_THR    0x23C

#define RW_MC_WPOST   0x250
    #define DISABLE_MC_WRITE    (0x1 << 0)
    
#define RW_MC_HMBRNG   0x260
#define RW_MC_VMBRNG   0x264

#define RW_ECO_CTRL   0x268
	#define DBK_FLR_CNT  (0x1 << 8)
	#define DBK_FLR_NEW_MODE   (0x1 << 4)
	
#define RO_MC_DBK_4P     0x5C4

#define RO_MC_DBK_DEFT     0x608
#define RO_MC_DBK_OFST     0x60C

#define RW_MC_DBK_FLR   0x658
    #define PARTIAL     (0x1 << 0)
    #define INTERLACE    (0x1 << 1)
    #define ENGR        (0x1 << 2)    
    #define SKIPMB      (0x1 << 3)    
    #define BYPASSWRAP    (0x1 << 4)    
    #define WRAPIDX    (0x1 << 5)    
    
#define RW_MC_DBK_PARTIAL 0x65C

#define RW_MC_QPBPT  0x700
#define RW_MC_DBKATHR   0x704


#define RW_PP_CTRL      0x30
	#define DBK_STR_FLITER  (0x1 << 8)
#define RW_PP_DUMMY     0x200
    #define IS_DUMMY       (0x1 << 17)

#define RW_PP_DBK_EC    0x40 // error concealment
	#define MPEG1_DBK_BWEC  (0x1 << 12) // backward error concealment
#define RO_PP_DBK_4P    0x50
#define RO_PP_DBK_DEFT  0x54
#define RO_PP_DBK_OFST  0x58
#endif
#if defined(MPV_CRCTEST) || defined(MULTIMEDIA_AUTOTEST_SUPPORT) || defined(VDEC_CRC_TEST)
#define VDEC_MISC                     (IO_VIRT + 0x3000)
#endif 
// ~comes from mpv_hw.h, all about DBK

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define IS_MPEG1(prEsInfo)    (prEsInfo->ucMpegVer == MPEG1)

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static UINT32 _u4DramClk = 0;
//static UINT32 _u4WaitBusCycle = 0;
static HANDLE_T _hVldTimer[MPV_MAX_VLD] = {0};
static volatile UCHAR _ucTimeout[MPV_MAX_VLD] = {0};
#ifdef MPV_SEMI_HOST
#ifndef MPV_DUMP_SIM_PAT
#define MPV_DUMP_SIM_PAT
#endif
//#define MPV_TEST_DDR3 1
#define MPV_TEST_DDR3 0
#else
#define MPV_TEST_DDR3 0
#endif
#ifdef MPV_DUMP_SIM_PAT
BOOL _bVdecLogSimPat = FALSE;
#endif

#ifdef MPV_DUMP_REG
BOOL _fgDumpMCData = FALSE;
#endif

#ifdef MPV_DUAL_CORE_SUPPORT
const UINT32  MPV_DUAL_FORMAT_COMMON_SETTING[11]={
    0, 480, 960, 1232, 1368, 1436, 1556, 960, 60, 128, 162
};
const UINT32 MPV_DUAL_FORMAT_CORE1_SETTING[3] = {
    60, 128, 162
};
#endif

void VLD_TOP_WRITE32(int id, int offset, int value)
{
    _VDEC_CheckLockStatus((UCHAR)id);
    if (id == 0)
    {
#ifdef MPV_DUMP_SIM_PAT
        if(_bVdecLogSimPat)
        {
            Printf("RISCWrite(`VDEC_FULL_ADDR_TOP + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, 
                value);
        }
#endif
        VLD0_TOP_WRITE32(offset, value);
    }
    else
    {
#ifdef MPV_DUMP_SIM_PAT
            if(_bVdecLogSimPat)
            {
                Printf("RISCWrite(`VDEC_FULL_ADDR_TOP1 + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, 
                    value);
            }
#endif    
        VLD1_TOP_WRITE32(offset, value);
    }        
}

void PP_WRITE32(int id, int offset, int value)
{
    _VDEC_CheckLockStatus((UCHAR)id);
    if (id == 0)
    {
#ifdef MPV_DUMP_SIM_PAT
        if(_bVdecLogSimPat)
        {
            Printf("RISCWrite(`VDEC_FULL_ADDR_PP + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, 
                value);
        }
#endif
        PP0_WRITE32(offset, value);
    }
    else
    {
#ifdef MPV_DUMP_SIM_PAT
        if(_bVdecLogSimPat)
        {
            Printf("RISCWrite(`VDEC_FULL_ADDR_PP1 + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, 
                value);
        }
#endif    
        PP1_WRITE32(offset, value);
    }        
}

void MISC_WRITE32(int id, int offset, int value)
{
    _VDEC_CheckLockStatus((UCHAR)id);
    if (id == 0)
    {        
#ifdef MPV_DUMP_SIM_PAT
        if(_bVdecLogSimPat)
        {
            Printf("RISCWrite(`VDEC_FULL_ADDR_MISC + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, 
                value);
        }
#endif
        MISC0_WRITE32(offset, value);
    }
    else
    {
#ifdef MPV_DUMP_SIM_PAT
        if(_bVdecLogSimPat)
        {
            Printf("RISCWrite(`VDEC_FULL_ADDR_MISC1 + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, 
                value);
        }
#endif    
        MISC1_WRITE32(offset, value);
    }        
}

void VLD_WRITE32(int id, int offset, int value)
{
    _VDEC_CheckLockStatus((UCHAR)id);
    if (id == 0)
    {        
#ifdef MPV_DUMP_SIM_PAT
        if(_bVdecLogSimPat)
        {
            Printf("RISCWrite(`VDEC_FULL_ADDR_VLD + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, 
                value);
        }
#endif
        VLD0_WRITE32(offset, value);
    }
    else
    {
#ifdef MPV_DUMP_SIM_PAT
        if(_bVdecLogSimPat)
        {
            Printf("RISCWrite(`VDEC_FULL_ADDR_VLD1 + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, 
                value);
        }
#endif    
        VLD1_WRITE32(offset, value);
    }        
}
 
void MC_WRITE32(int id, int offset, int value)
{
    _VDEC_CheckLockStatus((UCHAR)id);
    if (id == 0)
    {        
#ifdef MPV_DUMP_SIM_PAT
        if(_bVdecLogSimPat)
        {
            Printf("RISCWrite(`VDEC_FULL_ADDR_MC + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, 
                value);
        }
#endif
        MC0_WRITE32(offset, value);
    }
    else
    {
#ifdef MPV_DUMP_SIM_PAT
        if(_bVdecLogSimPat)
        {
            Printf("RISCWrite(`VDEC_FULL_ADDR_MC1 + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, 
                value);
        }
#endif    
        MC1_WRITE32(offset, value);
    }        
}

UINT32 VLD_TOP_READ32(UCHAR id, UINT16 offset)
{
    _VDEC_CheckLockStatus(id);
    if (id == 0)
    {
#ifdef MPV_DUMP_SIM_PAT
        UINT32 u4Reg = VLD0_TOP_READ32(offset);
        if(_bVdecLogSimPat)
        {
            Printf("RISCRead(`VDEC_FULL_ADDR_TOP + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, u4Reg);
        }
        return u4Reg;
#else
        return VLD0_TOP_READ32(offset);
#endif
    }
    else
    {
#ifdef MPV_DUMP_SIM_PAT
            UINT32 u4Reg = VLD1_TOP_READ32(offset);
            if(_bVdecLogSimPat)
            {
                Printf("RISCRead(`VDEC_FULL_ADDR_TOP1 + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, u4Reg);
            }
            
            LOG(5, "Read VLD_TOP_%d(%d) 0x%x\n", id, offset/4, u4Reg);
            return u4Reg;
#else
        return VLD1_TOP_READ32(offset);
#endif
    }        
}

UINT32 PP_READ32(UCHAR id, UINT16 offset)
{
    _VDEC_CheckLockStatus(id);
    if (id == 0)
    {
#ifdef MPV_DUMP_SIM_PAT
        UINT32 u4Reg = PP0_READ32(offset);
        if(_bVdecLogSimPat)
        {
            Printf("RISCRead(`VDEC_FULL_ADDR_PP + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, u4Reg);
        }
        return u4Reg;
#else
        return PP0_READ32(offset);
#endif
    }
    else
    {
#ifdef MPV_DUMP_SIM_PAT
        UINT32 u4Reg = PP1_READ32(offset);
        if(_bVdecLogSimPat)
        {
            Printf("RISCRead(`VDEC_FULL_ADDR_PP1 + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, u4Reg);
        }
        return u4Reg;
#else
        return PP1_READ32(offset);
#endif
    }        
}

UINT32 MISC_READ32(UCHAR id, UINT16 offset)
{
    _VDEC_CheckLockStatus(id);
    if (id == 0)
    {
#ifdef MPV_DUMP_SIM_PAT
        UINT32 u4Reg = MISC0_READ32(offset);
        if(_bVdecLogSimPat)
        {
            Printf("RISCRead(`VDEC_FULL_ADDR_MISC + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, u4Reg);
        }
        return u4Reg;
#else
        return MISC0_READ32(offset);
#endif
    }
    else
    {
#ifdef MPV_DUMP_SIM_PAT
        UINT32 u4Reg = MISC1_READ32(offset);
        if(_bVdecLogSimPat)
        {
            Printf("RISCRead(`VDEC_FULL_ADDR_MISC1 + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, u4Reg);
        }
        return u4Reg;
#else
        return MISC1_READ32(offset);
#endif
    }        
}

UINT32 VLD_READ32(UCHAR id, UINT16 offset)
{
    _VDEC_CheckLockStatus(id);
    if (id == 0)
    {        
#ifdef MPV_DUMP_SIM_PAT
        UINT32 u4Reg = VLD0_READ32(offset);
        if(_bVdecLogSimPat)
        {
            Printf("RISCRead(`VDEC_FULL_ADDR_VLD + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, u4Reg);
        }
        return u4Reg;
#else
        return VLD0_READ32(offset);
#endif
    }
    else
    {
#ifdef MPV_DUMP_SIM_PAT
        UINT32 u4Reg = VLD1_READ32(offset);
        if(_bVdecLogSimPat)
        {
            Printf("RISCRead(`VDEC_FULL_ADDR_VLD1 + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, u4Reg);
        }
        return u4Reg;
#else
        return VLD1_READ32(offset);
#endif
    }        
}

UINT32 MC_READ32(UCHAR id, UINT16 offset)
{
    _VDEC_CheckLockStatus(id);
    if (id == 0)
    {        
#ifdef MPV_DUMP_SIM_PAT
        UINT32 u4Reg = MC0_READ32(offset);
        if(_bVdecLogSimPat)
        {
            Printf("RISCRead(`VDEC_FULL_ADDR_MC + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, u4Reg);
        }
        return u4Reg;
#else
        return MC0_READ32(offset);
#endif
    }
    else
    {
#ifdef MPV_DUMP_SIM_PAT
        UINT32 u4Reg = MC1_READ32(offset);
        if(_bVdecLogSimPat)
        {
            Printf("RISCRead(`VDEC_FULL_ADDR_MC1 + 4*%d, 32'h%x);\n", (offset & 0xFFF)>>2, u4Reg);
        }
        return u4Reg;
#else
        return MC1_READ32(offset);
#endif
    }        
}

        
//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

static VOID _VldFetchTimeout(HANDLE_T  pt_tm_handle, VOID *pv_tag)
{
    UCHAR ucMpvId;
    UINT32 u4Idx;

    HAL_GetTime(&_rStopTime);    
    
    ucMpvId = (UCHAR)(UINT32)pv_tag;    

    if (ucMpvId < MPV_MAX_VLD)
    {
        _ucTimeout[ucMpvId] = 1;
    }
    else
    {
        ASSERT(ucMpvId < MPV_MAX_VLD);
        
        for (u4Idx = 0; u4Idx < MPV_MAX_VLD; u4Idx++)
        {
            _ucTimeout[u4Idx] = 1;
        }
    }
        
    UNUSED(pt_tm_handle);

}


static BOOL _WaitVldFetchOk(UCHAR ucMpvId)
{
    #if defined(CC_MT5387) && !defined(CC_MT5363)
    #else
    UINT32 u4Cnt = 0;
	#endif

    if(ucMpvId >= MPV_MAX_VLD)
    {
        LOG(0, "ucMpvId >= MPV_MAX_VLD\n");
        ASSERT(ucMpvId < MPV_MAX_VLD);
        ucMpvId = (MPV_MAX_VLD-1);
    }
    
    _ucTimeout[ucMpvId] = 0;    

    HAL_GetTime(&_rStartTime);
    
    #if 1
    VERIFY(x_timer_start(_hVldTimer[ucMpvId], 100, X_TIMER_FLAG_ONCE,
        _VldFetchTimeout, (void*)(UINT32)ucMpvId) == OSR_OK);

#ifdef MPV_DUMP_SIM_PAT
    if(_bVdecLogSimPat)
    {
        Printf("wait(`VDEC_INI_FETCH_RDY == 1);\n");
    }
#endif

    while (_ucTimeout[ucMpvId] == 0)
    {
        if ((_MPV_VldRead32(ucMpvId, RO_VLD_FETCHOK) & VLD_FETCH_OK))
        {
            VERIFY(x_timer_stop(_hVldTimer[ucMpvId]) == OSR_OK);

            #if defined(CC_MT5387) && !defined(CC_MT5363)
            #elif defined(VDEC_IS_POST_MT5368)
            UNUSED(u4Cnt);
            #else
            while((_MPV_VldRead32(ucMpvId, RO_VLD_SRAMCTRL) & AA_FIT_TARGET_SCLK)==0)
            {
                u4Cnt++;
                if(u4Cnt >= MPV_WAIT_SRAM_THRD)
                {
                    LOG(1, "WaitSramStable Fail\n");
                    #ifdef MPV_DUMP_REG
                    if (0)
                    {
                       _MPV_DumpReg(ucMpvId);
                    }
                    #endif
                    break;
                }
            }
			#endif
            
            return (TRUE);
        }
    }
    #else

    while (!(_MPV_VldRead32(ucMpvId, RO_VLD_FETCHOK) & VLD_FETCH_OK))
    {
        u4Count++;
        if (u4Count > 0xFffffff0)
        {
            break;
        }
    }

    if (_MPV_VldRead32(ucMpvId, RO_VLD_FETCHOK) & VLD_FETCH_OK)
    {
        return TRUE;
    }
    #endif

HAL_GetDeltaTime(&_rDeltaTime, &_rStartTime, &_rStopTime);
LOG(1, "WaitVldFetch Fail %d\n", _rDeltaTime.u4Micros);

    // timeout
    return FALSE;
}


static void _VldIsr(UINT16 u2Vector)
{

    if (u2Vector == VLD_VECTOR)
    {
        if (VLD_READ32(VLD0, RW_VLD_INTP) & VLD_INTP)
        {
            _MPV_GetStopTime(VLD0);
            #if defined (CC_MT5360B) || defined (CC_MT5392B)
                VLD_WRITE32(VLD0, RW_VLD_INTP, (UINT32)VLD_OK_CLR);
			#elif defined(CC_MT5387) && !defined(CC_MT5363)
                VLD_WRITE32(VLD0, RW_VLD_INTP, (UINT32)VLD_CLR);
            #elif defined (VDEC_IS_POST_MT5368)
                VLD_WRITE32(VLD0, RW_VLD_INTP, (UINT32)VLD_OK_CLR);
                VLD_WRITE32(VLD0, RW_VLD_INTP, (UINT32)0);
                BIM_ClearIrq(VLD_VECTOR);
            #else // CC_MT5363, CC_MT5365, CC_MT5395,  defined(VDEC_IS_POST_MT5368)
                VLD_WRITE32(VLD0, RW_VLD_SSCS, (UINT32)VLD_CLR);
            #endif
            _MPV_DecTimeout(VLD0, FALSE);
            _MPV_FinDec(VLD0);
        }
        else if (VLD_READ32(VLD0, RW_VLD_INTP) & MDEC_TIMEOUT_INTP)
        {
            _MPV_GetStopTime(VLD0);
            VLD_WRITE32(VLD0, RW_VLD_INTP, (UINT32)MDEC_TIMEOUT_CLR);                                    
            #if defined (VDEC_IS_POST_MT5368)
            VLD_WRITE32(VLD0, RW_VLD_INTP, (UINT32)0);
            BIM_ClearIrq(VLD_VECTOR);
            #endif
            _MPV_DecTimeout(VLD0, TRUE);
            _MPV_FinDec(VLD0);
            LOG(1, "TIMEOUT INT!\n");
        }
        else if (VLD_READ32(VLD0, RW_VLD_INTP) & START_CODE_INT)
        {
            VLD_WRITE32(VLD0, RW_VLD_INTP, (UINT32)START_CODE_CLR);                                    
            #if defined (VDEC_IS_POST_MT5368)
            VLD_WRITE32(VLD0, RW_VLD_INTP, (UINT32)0);
            BIM_ClearIrq(VLD_VECTOR);
            #endif
            _MPV_FinSearch(VLD0);
            LOG(10, "Find Start Code!\n");
        }

    }
#if  defined (CC_MT5890)    
    else if(u2Vector == VLD_VECTOR_CORE1)        
    {
        if (VLD_READ32(VLD1, RW_VLD_INTP) & VLD_INTP)
        {
            _MPV_GetStopTime(VLD1);
            #if defined (CC_MT5360B) || defined (CC_MT5392B)
                VLD_WRITE32(VLD1, RW_VLD_INTP, (UINT32)VLD_OK_CLR);
            #elif defined(CC_MT5387) && !defined(CC_MT5363)
                VLD_WRITE32(VLD1, RW_VLD_INTP, (UINT32)VLD_CLR);
            #elif defined (VDEC_IS_POST_MT5368)
                VLD_WRITE32(VLD1, RW_VLD_INTP, (UINT32)VLD_OK_CLR);
                VLD_WRITE32(VLD1, RW_VLD_INTP, (UINT32)0);
                BIM_ClearIrq(VLD_VECTOR_CORE1);
            #else // CC_MT5363, CC_MT5365, CC_MT5395,  defined(VDEC_IS_POST_MT5368)
                VLD_WRITE32(VLD1, RW_VLD_SSCS, (UINT32)VLD_CLR);
            #endif
            _MPV_DecTimeout(VLD1, FALSE);
            _MPV_FinDec(VLD1);
        }
        else if (VLD_READ32(VLD1, RW_VLD_INTP) & MDEC_TIMEOUT_INTP)
        {
            _MPV_GetStopTime(VLD1);
            VLD_WRITE32(VLD1, RW_VLD_INTP, (UINT32)MDEC_TIMEOUT_CLR);                                    
            #if defined (VDEC_IS_POST_MT5368)
            VLD_WRITE32(VLD1, RW_VLD_INTP, (UINT32)0);
            BIM_ClearIrq(VLD_VECTOR_CORE1);
            #endif
            _MPV_DecTimeout(VLD1, TRUE);
            _MPV_FinDec(VLD1);
            LOG(1, "TIMEOUT INT!\n");
        }
        else if (VLD_READ32(VLD1, RW_VLD_INTP) & START_CODE_INT)
        {
            VLD_WRITE32(VLD1, RW_VLD_INTP, (UINT32)START_CODE_CLR);                                    
            #if defined (VDEC_IS_POST_MT5368)
            VLD_WRITE32(VLD1, RW_VLD_INTP, (UINT32)0);
            BIM_ClearIrq(VLD_VECTOR_CORE1);
            #endif
            _MPV_FinSearch(VLD1);
            LOG(10, "Find Start Code!\n");
        }
    }
#endif   
    else
    {
        LOG(0, "Error, not VLD_VECTOR or VLD_VECTOR_CORE1!\n");
    }
}



#ifdef CC_MPV_DEBLOCKING
#if defined(CC_MT5363) && !defined(CC_MT5365) && !defined(CC_MT5395)
//#ifdef CC_MT5363

static void _SetDbkPrmProc(UCHAR ucEsId, UCHAR ucMpvId, MPV_ES_INFO_T *prEsInfo, 
       const FBM_SEQ_HDR_T *prFbmSeqHdr, const FBM_PIC_HDR_T *prFbmPicHdr)
{
    UCHAR ucFRefFbId;
    UCHAR ucBRefFbId;
    UINT16 u2HSizeMB;
    UINT16 u2HSizeMBtoMC;
    UINT16 u2VSize;
    UINT32 u4DbkPicSz;
    UINT32 u4QP0_Y;
    UINT32 u4QP1_Y;
    UINT32 u4QP2_Y;
    UINT32 u4QP3_Y;    
    UINT32 u4QP0_C;
    UINT32 u4QP1_C;
    UINT32 u4QP2_C;
    UINT32 u4QP3_C;    
    UINT32 u4FtrCnt;
    UINT32 u4FtrThd;
    UINT32 u4OfstCnt;
    UINT32 u4pCnt;    
    UINT32 u4FRefOfstCnt;
    UINT32 u4FRef4pCnt;
    UINT32 u4BRefOfstCnt;
    UINT32 u4BRef4pCnt;
    UINT32 u4RegQpY;
    UINT32 u4RegQpC;
    UINT32 u4DbkRegion;
    UINT32 u4RegVal;
    UINT32 u4DbkReg = 0;
    VDEC_ES_INFO_T *prVdecEsInfo;
    MPV_INFO_T *prMpvInfo;    

    ASSERT(prEsInfo);
    ASSERT(prFbmSeqHdr);    
    ASSERT(prFbmPicHdr);
    
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prMpvInfo = _MPV_GetMpvInfo();

    // deblocking does not support 16x16 size, ccm1.mpg
    if ((prVdecEsInfo->u2HSize < DBK_MIN_H) &&
        (prVdecEsInfo->u2VSize < DBK_MIN_H))
    {
        // we don't set dbk register, so we have to use MC result
        // we will change to output MC result in _MPV_HandleDbk.
        return;
    }
    
#if 0
#ifdef CC_MT5363
#else
    if (prVdecEsInfo->u2HSize > DBK_MIN_H)
#endif
#endif
    
    {
        FBM_GetRefFrameBuffer(prVdecEsInfo->ucFbgId, &ucFRefFbId, &ucBRefFbId);
        FBM_GetFrameBufferFtrCnt(prVdecEsInfo->ucFbgId, ucFRefFbId, &u4OfstCnt, &u4pCnt);
        u4FRefOfstCnt = u4OfstCnt;
        u4FRef4pCnt = u4pCnt;

        FBM_GetFrameBufferFtrCnt(prVdecEsInfo->ucFbgId, ucBRefFbId, &u4OfstCnt, &u4pCnt);
        u4BRefOfstCnt = u4OfstCnt;
        u4BRef4pCnt = u4pCnt;

        u2HSizeMB = (prVdecEsInfo->u2OrgHSize + 15) >> 4;
        u2HSizeMBtoMC = (prVdecEsInfo->u2LineSize + 15) >> 4;        
        u2VSize =  MPV_ALIGN_MASK(prVdecEsInfo->u2OrgVSize, 0xf);
        
        u4FtrCnt = u2HSizeMB * u2VSize * 6;
        
        if (!prEsInfo->u4P11)
        {
            u4FtrThd = (u4FtrCnt * DBK_FTR_RATIO)/10;
        }
        else
        {
            u4FtrThd = (u4FtrCnt *prEsInfo->u4P12)/10;
            MC_WRITE32(ucMpvId, RW_MC_DBK_THRSD, (prEsInfo->u4P11<<16)|0x0202);        
            MC_WRITE32(ucMpvId, RW_MC_DBK_ADV_THRSD_Y, 
                (prEsInfo->u4P20<<28)|(prEsInfo->u4P19<<24)|(prEsInfo->u4P18<<20)|
                (prEsInfo->u4P17<<16)|(prEsInfo->u4P16<<12)|(prEsInfo->u4P15<<8)|
                (prEsInfo->u4P14<<4)|prEsInfo->u4P13);                        
            MC_WRITE32(ucMpvId, RW_MC_DBK_ADV_THRSD_C, 
                (prEsInfo->u4P20<<28)|(prEsInfo->u4P19<<24)|(prEsInfo->u4P18<<20)|
                (prEsInfo->u4P17<<16)|(prEsInfo->u4P16<<12)|(prEsInfo->u4P15<<8)|
                (prEsInfo->u4P14<<4)|prEsInfo->u4P13);                        
            
        }

        if (IS_FRMPIC(prFbmPicHdr))
        {
            u4DbkPicSz =  ((u2HSizeMB << 16) + ((u2VSize >> 4) << 8) + u2HSizeMBtoMC);
        }
        else
        {
            u4DbkPicSz =  ((u2HSizeMB << 16) + ((u2VSize >> 5) << 8) + u2HSizeMBtoMC);
        }


        u4DbkReg = _MPV_McRead32(ucMpvId, RW_MC_DBK_CTRL);

        if (prVdecEsInfo->ucPicType == B_TYPE)
        {
            u4DbkReg |= (/*DBK_2MBROW|*/DBK_EN);
        }
        else
        {
           u4DbkReg |= DBK_EN ;
        }

        if (prVdecEsInfo->ucPicType == B_TYPE)
        {
            u4DbkReg |= 0/*DBK_BW_SAVING*/;        
        }

        // 5382
        //u4DbkReg |= (DBK_GROUP1 | DBK_DISABLE_DELAY);
        // 5363, not GROUP1 now
        u4DbkReg |= (/*DBK_GROUP1 | */DBK_DISABLE_DELAY);
            

        LOG(7,"dbk reg 0x%x\n", u4DbkReg);        

        MC_WRITE32(ucMpvId, RW_MC_DBK_CTRL, u4DbkReg);
        
        MC_WRITE32(ucMpvId, RW_MC_DBK_PICSZ, u4DbkPicSz);
        // ilii        
        //MC_WRITE32(ucMpvId, RW_MC_DBK_CTRL, (u4DbkReg | 0x4));

        if (prEsInfo->u4P1)
        {
            if (prVdecEsInfo->ucPicType == I_TYPE)
            {
                u4QP0_Y = prEsInfo->u4P1;    
                u4QP0_C = prEsInfo->u4P1;    
            }
            else if ((prVdecEsInfo->ucPicType == P_TYPE) && 
                ((u4FRefOfstCnt + u4FRef4pCnt) >= u4FtrThd))
            {
                u4QP0_Y = prEsInfo->u4P2;        
                u4QP0_C = prEsInfo->u4P2;            
            }
            else if ((prVdecEsInfo->ucPicType == B_TYPE) && 
                ((u4FRefOfstCnt + u4FRef4pCnt + u4BRefOfstCnt + u4BRef4pCnt) >= (u4FtrThd << 1)))
            {
                u4QP0_Y = prEsInfo->u4P3;    
                u4QP0_C = prEsInfo->u4P3;            
            }
            else
            {
                u4QP0_Y = prEsInfo->u4P4;        
                u4QP0_C = prEsInfo->u4P4;        
            }

            u4QP1_Y = u4QP0_Y + prEsInfo->u4P5;
            u4QP2_Y = u4QP1_Y + prEsInfo->u4P6;
            u4QP3_Y = u4QP2_Y + prEsInfo->u4P7;

            u4QP1_C = u4QP0_C + prEsInfo->u4P8;
            u4QP2_C = u4QP1_C + prEsInfo->u4P9;
            u4QP3_C = u4QP2_C + prEsInfo->u4P10;
        }
        else
        {
            if (prVdecEsInfo->ucPicType == I_TYPE)
            {
                u4QP0_Y = DBK_THQP0I;    
                u4QP0_C = DBK_THQP0I_C;    
            }
            else if ((prVdecEsInfo->ucPicType == P_TYPE) && 
                ((u4FRefOfstCnt + u4FRef4pCnt) >= u4FtrThd))
            {
                u4QP0_Y = DBK_THQP0P;        
                u4QP0_C = DBK_THQP0P_C;            
            }
            else if ((prVdecEsInfo->ucPicType == B_TYPE) && 
                ((u4FRefOfstCnt + u4FRef4pCnt + u4BRefOfstCnt + u4BRef4pCnt) >= (u4FtrThd << 1)))
            {
                u4QP0_Y = DBK_THQP0B;    
                u4QP0_C = DBK_THQP0B_C;            
            }
            else
            {
                u4QP0_Y = DBK_THQP0;        
                u4QP0_C = DBK_THQP0_C;        
            }

            u4QP1_Y = u4QP0_Y + DBK_QP1_Y;
            u4QP2_Y = u4QP1_Y + DBK_QP2_Y;
            u4QP3_Y = u4QP2_Y + DBK_QP3_Y;

            u4QP1_C = u4QP0_C + DBK_QP1_C;
            u4QP2_C = u4QP1_C + DBK_QP2_C;
            u4QP3_C = u4QP2_C + DBK_QP3_C;
        }

        if (u4QP2_C > DBK_QP_MAX)
        {
            u4QP2_C = DBK_QP_MAX;
        }

        if (u4QP3_C > DBK_QP_MAX)
        {
            u4QP3_C = DBK_QP_MAX;
        }

        u4RegQpY = (u4QP3_Y << 24) | (u4QP2_Y << 16) | (u4QP1_Y << 8) | (u4QP0_Y);
        u4RegQpC = (u4QP3_C << 24) | (u4QP2_C << 16) | (u4QP1_C << 8) | (u4QP0_C);

        MC_WRITE32(ucMpvId, RW_MC_DBK_QP_Y, u4RegQpY);        
        MC_WRITE32(ucMpvId, RW_MC_DBK_QP_C, u4RegQpC);        

        if (((prEsInfo->u4DbkXEnd == 0) || (prEsInfo->u4DbkYEnd == 0)) &&
            ((prMpvInfo->u2DbkWidthT == 0) || (prMpvInfo->u2DbkHeightT)))
        {
            prEsInfo->u4DbkXStart= 0;
            prEsInfo->u4DbkXEnd = prVdecEsInfo->u2OrgHSize >> 4;
            prEsInfo->u4DbkYStart= 0;
            prEsInfo->u4DbkYEnd = prVdecEsInfo->u2OrgVSize >> 4;        
        }
        else if ((prMpvInfo->u2DbkWidthT != 0) && (prMpvInfo->u2DbkHeightT != 0))
        {
            prEsInfo->u4DbkXStart = ((prMpvInfo->u2DbkXStartT *prVdecEsInfo->u2HSize) / 1000) >> 4;
            prEsInfo->u4DbkYStart = ((prMpvInfo->u2DbkYStartT *prVdecEsInfo->u2VSize) / 1000) >> 4;
            prEsInfo->u4DbkXEnd = (((prMpvInfo->u2DbkXStartT + prMpvInfo->u2DbkWidthT) *prVdecEsInfo->u2HSize) / 1000) >> 4;
            prEsInfo->u4DbkYEnd = (((prMpvInfo->u2DbkYStartT + prMpvInfo->u2DbkHeightT) *prVdecEsInfo->u2VSize) / 1000) >> 4;
        }

        u4DbkRegion= ((prEsInfo->u4DbkYEnd << 24) |(prEsInfo->u4DbkYStart << 16)|
                     (prEsInfo->u4DbkXEnd << 8)|(prEsInfo->u4DbkXStart));       
                                    
        MC_WRITE32(ucMpvId, RW_MC_DBK_INTEREST, u4DbkRegion);        

        if (prMpvInfo->fgDbkEngrMode)
        {
            //MC_WRITE32(ucMpvId, RW_MC_DBK_DEBUG, ((255 << 24) |(128 << 16)|FPF_EN|DEF_EN|OFST_EN));    
            u4RegVal = _MPV_McRead32(ucMpvId, RW_MC_DBK_DEBUG);            
            MC_WRITE32(ucMpvId, RW_MC_DBK_DEBUG, (u4RegVal|FPF_EN|DEF_EN|OFST_EN));                
        }
        
        MC_WRITE32(ucMpvId, WO_MC_DBK_DEC, DBK_START);    
    }

    /*
    #ifdef MPV_SEMI_HOST
    //for golden comparison
    MC_WRITE32(ucMpvId, RW_MC_DBLK_THR, 0x20004);
    if (prFbmPicHdr->ucPicStruct != MPEG_PIC_FRAME)
    {
        MC_WRITE32(ucMpvId, RW_MC_DBK_FLR, 0x3050038);        
    }
    else
    {
        MC_WRITE32(ucMpvId, RW_MC_DBK_FLR, 0x305003a);    
    }
    MC_WRITE32(ucMpvId, RW_MC_QPBPT, (8<<24|6<<16|4<<8|2));        
    MC_WRITE32(ucMpvId, 0x704, 0x12343456);
    #endif  
    */
    

}


void  _MPV_McGetDbkCnt(UCHAR ucMpvId, UCHAR ucEsId, UINT32* pu4DbkOfstCnt, UINT32* pu4Dbk4pCnt)
{
    UINT32 u4Ofst;
    UINT32 u4Deft;
    UINT32 u44p;
    UINT32 u4Ratio;
    UINT32 u4MbCnt;
    MPV_ES_INFO_T *prEsInfo;
    VDEC_ES_INFO_T *prVdecEsInfo;
    
    prEsInfo = (MPV_ES_INFO_T*)_MpvGetHdlr(ucEsId);
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);       

    u4MbCnt = (prVdecEsInfo->u2HSize *prVdecEsInfo->u2VSize) >> 8;
        
    ASSERT(pu4DbkOfstCnt);
    ASSERT(pu4Dbk4pCnt);
    
    *pu4DbkOfstCnt = _MPV_McRead32(ucMpvId, RO_MC_DBK_OFST_CNT);
    *pu4Dbk4pCnt = _MPV_McRead32(ucMpvId, RO_MC_DBK_4P_CNT);

#if defined(VDEC_IS_POST_MT5398)
    u4Ofst = _MPV_McRead32(ucMpvId, RO_MC_DBK_OFST) - 6*u4MbCnt + 3*(prVdecEsInfo->u2HSize / 16);
#else
    u4Ofst = _MPV_McRead32(ucMpvId, RO_MC_DBK_OFST);
#endif
    u4Deft = _MPV_McRead32(ucMpvId, RO_MC_DBK_DEFT_CNT);
    u44p = _MPV_McRead32(ucMpvId, RO_MC_DBK_4P_CNT);

    u4Ratio = ((u4Ofst + u44p) * 100) / (u4MbCnt * 6*16);

    prEsInfo->u4DbkRatio = u4Ratio;

    LOG(7, "Ratio(%d%)\n", u4Ratio);    
    UNUSED(u4Deft);
}

#else // CC_MT5360 CC_MT5391 CC_MT5387(not have deblocking) CC_MT5365 CC_MT5395
// CC_MT5368 CC_MT5396,  || defined(VDEC_IS_POST_MT5368)
static void _SetDbkPrmProc(UCHAR ucEsId, UCHAR ucMpvId, MPV_ES_INFO_T *prEsInfo, 
       const FBM_SEQ_HDR_T *prFbmSeqHdr, const FBM_PIC_HDR_T *prFbmPicHdr)
{
    UINT16 u2HSizeMB;
    UINT16 u2HSizeMBtoMC;
    UINT16 u2VSizeMB;  
    UINT16 u2VSize;
    UINT32 u4DbkRegion;
    UINT32 u4RegVal;    
    UINT32 u4RegQp, u4QP0, u4QP1, u4QP2, u4QP3;
    INT32 i4RegQp;
    UINT32 u4SliceIdx; 
    UCHAR ucFRefFbId;
    UCHAR ucBRefFbId;
    UINT32 u4FtrCnt;
    UINT32 u4FtrThd;
    UINT32 u4OfstCnt;
    UINT32 u4pCnt;    
    UINT32 u4FRefOfstCnt;
    UINT32 u4FRef4pCnt;
    UINT32 u4BRefOfstCnt;
    UINT32 u4BRef4pCnt;
    UINT32 u4Tmp;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    //VDEC_INFO_T *prVdecInfo;
    MPV_INFO_T *prMpvInfo;
    
    
    if(ucMpvId >= MPV_MAX_VLD)
    {
        LOG(0, "ucMpvId >= MPV_MAX_VLD\n");
        ASSERT(ucMpvId < MPV_MAX_VLD);
        ucMpvId = (MPV_MAX_VLD-1);
    }
    
    if((prEsInfo == NULL) ||
        (prFbmSeqHdr == NULL) ||
        (prFbmPicHdr == NULL))
    {
        LOG(0, "_SetDbkPrmProc null pointer\n");
        return;
    }
    
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    //prVdecInfo = _VDEC_GetInfo();
    prMpvInfo = (MPV_INFO_T*)_MPV_GetMpvInfo();

    // deblocking does not support 16x16 size, ccm1.mpg
    if ((prVdecEsInfo->u2HSize < DBK_MIN_H) &&
        (prVdecEsInfo->u2VSize < DBK_MIN_H))
    {
        // we don't set dbk register, so we have to use MC result
        // we will change to output MC result in _MPV_HandleDbk.
        return;
    }
    
    u2HSizeMB = (UINT16)((prVdecEsInfo->u2OrgHSize + 15) >> 4);
    u2VSizeMB = (UINT16)((prVdecEsInfo->u2OrgVSize + 15) >> 4);
    u2HSizeMBtoMC = (UINT16)((prVdecEsInfo->u2LineSize + 15) >> 4); 
    // ilii test 5391
    //u2HSizeMBtoMC = (prVdecEsInfo->u2OrgHSize + 15) >> 4; 
    
    MC_WRITE32(ucMpvId, RW_MC_POSTEN, (UINT32)POSTEN);
    MC_WRITE32(ucMpvId, RW_MC_DBK_CTRL, (UINT32)(DBKY|DBKC));

    if (prFbmPicHdr->ucPicStruct != MPEG_PIC_FRAME)
    {
//Dear all,
//�󥿽T�����k�O, MC #406  bit[4] �Ф��n�]1
//�]����Lbit�O�Ψӳ]�w�@��mode�Ϊ�, ���ݭn���ܬO�i�H����ʪ�
//�u��bit[4]  �ЫO��default�� (0)

//�ר�O���}�Fpp_out_buf�άORR��, MC_406 bit[4]�Y��1����
//�|�ɭPdecoder time out

//����

//�f��
        MC_WRITE32(ucMpvId, RW_MC_DBK_FLR, (UINT32)((3 << 24) |(16 << 16)|WRAPIDX|SKIPMB));
        //MC_WRITE32(ucMpvId, RW_MC_DBK_FLR, (UINT32)((3 << 24) |(16 << 16)|WRAPIDX|BYPASSWRAP|SKIPMB));
        u2VSizeMB = (UINT16)(u2VSizeMB >> 1);
    }

    MC_WRITE32(ucMpvId, RW_MC_HMBRNG, (UINT32)(u2HSizeMB - 1));
    u4SliceIdx = (_MPV_VldRead32(ucMpvId, RO_VLD_BARL) & 0xFF);
    UNUSED(u4SliceIdx);
    #if defined(VDEC_IS_POST_MT5368)
    MC_WRITE32(ucMpvId, RW_MC_VMBRNG, (UINT32)(u2VSizeMB - 1));
    #else
    MC_WRITE32(ucMpvId, RW_MC_VMBRNG, (UINT32)(((u4SliceIdx-1) << 8)|(u2VSizeMB - 1)));
    #endif
    MC_WRITE32(ucMpvId, RW_MC_POST_WD, (UINT32)u2HSizeMBtoMC);

    //MC_WRITE32(ucMpvId, RW_MC_DBLK_THR, (2 << 16) | 4);
    //MC_WRITE32(ucMpvId, RW_MC_DBK_FLR, (3 << 24) |(16 << 16)|WRAPIDX|BYPASSWRAP|SKIPMB |INTERLACE);



    if (prEsInfo->u4P1 != 0)
    {
        u4QP0 = prEsInfo->u4P2;
        u4QP1 = prEsInfo->u4P3;
        u4QP2 = prEsInfo->u4P4;
        u4QP3 = prEsInfo->u4P5;
    }
    else
    {
        FBM_GetRefFrameBuffer(prVdecEsInfo->ucFbgId, &ucFRefFbId, &ucBRefFbId);
        FBM_GetFrameBufferFtrCnt(prVdecEsInfo->ucFbgId, ucFRefFbId, &u4OfstCnt, &u4pCnt);
        u4FRefOfstCnt = u4OfstCnt;
        u4FRef4pCnt = u4pCnt;

        FBM_GetFrameBufferFtrCnt(prVdecEsInfo->ucFbgId, ucBRefFbId, &u4OfstCnt, &u4pCnt);
        u4BRefOfstCnt = u4OfstCnt;
        u4BRef4pCnt = u4pCnt;
        
        u2VSize =  (UINT16)MPV_ALIGN_MASK((UINT32)prVdecEsInfo->u2OrgVSize, (UINT32)0xf); 
        
        u4FtrCnt = (UINT32)((UINT32)u2HSizeMB * u2VSize * 6);

        u4FtrThd = (u4FtrCnt * 4)/10; //5381
        
        if (prVdecEsInfo->ucPicType == I_TYPE)
        {
            u4QP0 = DBK_QP0_I;
            u4QP1 = DBK_QP1_I;
            u4QP2 = DBK_QP2_I;
            u4QP3 = DBK_QP3_I;   
        }
        else if ((prVdecEsInfo->ucPicType == P_TYPE) && 
        ((u4FRefOfstCnt + u4FRef4pCnt) >= u4FtrThd))
        {
            u4QP0 = DBK_QP0_P;
            u4QP1 = DBK_QP1_P;
            u4QP2 = DBK_QP2_P;
            u4QP3 = DBK_QP3_P;     
        }
        else if ((prVdecEsInfo->ucPicType == B_TYPE) && 
        ((u4FRefOfstCnt + u4FRef4pCnt + u4BRefOfstCnt + u4BRef4pCnt) >= (UINT32)((u4FtrThd << 1))))
        {
            u4QP0 = DBK_QP0_B;
            u4QP1 = DBK_QP1_B;
            u4QP2 = DBK_QP2_B;
            u4QP3 = DBK_QP3_B;          
        }
        else
        {
            u4QP0 = DBK_QP0;
            u4QP1 = DBK_QP1;
            u4QP2 = DBK_QP2;
            u4QP3 = DBK_QP3;          
        }
    }
    
    if(prVdecEsInfoKeep->i2DBKAdjust != 0)
    {
        if(prVdecEsInfoKeep->i2DBKAdjust > 0)
        {
            u4Tmp = (UINT32)(UINT16)prVdecEsInfoKeep->i2DBKAdjust;
            if((u4QP0 + u4Tmp) < MPV_DBK_LEVEL_MAX)
            {
                u4QP0 += u4Tmp;
            }
            else
            {
                u4QP0 = MPV_DBK_LEVEL_MAX;
            }
            if((u4QP1 + u4Tmp) < MPV_DBK_LEVEL_MAX)
            {
                u4QP1 += u4Tmp;
            }
            else
            {
                u4QP1 = MPV_DBK_LEVEL_MAX;
            }
            if((u4QP2 + u4Tmp) < MPV_DBK_LEVEL_MAX)
            {
                u4QP2 += u4Tmp;
            }
            else
            {
                u4QP2 = MPV_DBK_LEVEL_MAX;
            }
            if((u4QP3 + u4Tmp) < MPV_DBK_LEVEL_MAX)
            {
                u4QP3 += u4Tmp;
            }
            else
            {
                u4QP3 = MPV_DBK_LEVEL_MAX;
            }
        }
        else
        {
            i4RegQp = (INT32)u4QP0;
            i4RegQp += prVdecEsInfoKeep->i2DBKAdjust;
            if(i4RegQp <= 0)
            {
                u4QP0 = 0;
            }
            else
            {
                u4QP0 = (UINT32)i4RegQp;
            }
            i4RegQp = (INT32)u4QP1;
            i4RegQp += prVdecEsInfoKeep->i2DBKAdjust;
            if(i4RegQp <= 0)
            {
                u4QP1 = 0;
            }
            else
            {
                u4QP1 = (UINT32)i4RegQp;
            }
            i4RegQp = (INT32)u4QP2;
            i4RegQp += prVdecEsInfoKeep->i2DBKAdjust;
            if(i4RegQp <= 0)
            {
                u4QP2 = 0;
            }
            else
            {
                u4QP2 = (UINT32)i4RegQp;
            }
            i4RegQp = (INT32)u4QP3;
            i4RegQp += prVdecEsInfoKeep->i2DBKAdjust;
            if(i4RegQp <= 0)
            {
                u4QP3 = 0;
            }
            else
            {
                u4QP3 = (UINT32)i4RegQp;
            }
        }
    }
        
    u4RegQp = (UINT32)((u4QP3 << 24) | (u4QP2 << 16) | (u4QP1 << 8) | (u4QP0));
    MC_WRITE32(ucMpvId, RW_MC_QPBPT, u4RegQp);

    prEsInfo->u4QP0 = u4QP0;
    prEsInfo->u4QP1 = u4QP1;
    prEsInfo->u4QP2 = u4QP2;
    prEsInfo->u4QP3 = u4QP3;

    if (prEsInfo->u4P11 != 0)
    {
        UINT32 u4Reg;
        UINT32 u4T1, u4T2, u4T3, u4T4, u4T5, u4T6, u4T7, u4T8;

        u4T1 = prEsInfo->u4P12;
        u4T2 = prEsInfo->u4P13;
        u4T3 = prEsInfo->u4P14;
        u4T4 = prEsInfo->u4P15;
        u4T5 = prEsInfo->u4P16;
        u4T6 = prEsInfo->u4P17;
        u4T7 = prEsInfo->u4P18;
        u4T8 = prEsInfo->u4P19;        

        u4Reg= (UINT32)(((u4T1<<28)|
                (u4T2<<24)|
                (u4T3<<20)|
                (u4T4<<16)|
                (u4T5<<12)|
                (u4T6<< 8)|
                (u4T7<< 4)|
                (u4T8)));
        MC_WRITE32(ucMpvId, RW_MC_DBKATHR, u4Reg);       
    }


    if (prVdecEsInfo->ucPicType == B_TYPE)
    {
        MC_WRITE32(ucMpvId, RW_MC_WPOST, (UINT32)DISABLE_MC_WRITE);        
    }


    if ((prMpvInfo->u2DbkWidthT != 0) && (prMpvInfo->u2DbkHeightT != 0))
    {
        prEsInfo->u4DbkXStart = (((UINT32)prMpvInfo->u2DbkXStartT * (UINT32)prVdecEsInfo->u2HSize) / 1000) >> 4;
        prEsInfo->u4DbkYStart = (((UINT32)prMpvInfo->u2DbkYStartT * (UINT32)prVdecEsInfo->u2VSize) / 1000) >> 4;
        prEsInfo->u4DbkXEnd = ((((UINT32)prMpvInfo->u2DbkXStartT + (UINT32)prMpvInfo->u2DbkWidthT) * (UINT32)prVdecEsInfo->u2HSize) / 1000) >> 4;
        prEsInfo->u4DbkYEnd = ((((UINT32)prMpvInfo->u2DbkYStartT + (UINT32)prMpvInfo->u2DbkHeightT) * (UINT32)prVdecEsInfo->u2VSize) / 1000) >> 4;
    }

    if ((prEsInfo->u4DbkXEnd != 0) && (prEsInfo->u4DbkYEnd!= 0))
    {
        u4DbkRegion= (UINT32)(((prEsInfo->u4DbkYEnd << 24) |(prEsInfo->u4DbkYStart << 16)|
                      (prEsInfo->u4DbkXEnd << 8)|(prEsInfo->u4DbkXStart)));       
        MC_WRITE32(ucMpvId, RW_MC_DBK_PARTIAL, u4DbkRegion);        

        u4RegVal = _MPV_McRead32(ucMpvId, RW_MC_DBK_FLR);   
        MC_WRITE32(ucMpvId, RW_MC_DBK_FLR, (u4RegVal|PARTIAL));                        
    }                                    

    if (prMpvInfo->fgDbkEngrMode)
    {
        u4RegVal = _MPV_McRead32(ucMpvId, RW_MC_DBK_FLR);            
        MC_WRITE32(ucMpvId, RW_MC_DBK_FLR, (u4RegVal|ENGR));                
    } 
    
#ifdef CC_MT5360
#ifndef CC_MT5360B // A, ECO version.
    MC_WRITE32(ucMpvId, (351*4), 1);
#endif
#endif

#ifdef CC_MT5391
#ifndef CC_MT5392B // A, ECO version.
    MC_WRITE32(ucMpvId, (351*4), 1);
#endif
#endif

#if defined(VDEC_IS_POST_MT5368)
    u4RegVal = PP_READ32(ucMpvId, RW_PP_CTRL);
    PP_WRITE32(ucMpvId, RW_PP_CTRL, (u4RegVal|DBK_STR_FLITER));
#elif defined(CC_MT5365) || defined(CC_MT5395)
    // to adjust filter strength. Add new control bit to switch on/off.
    // Turn on this value to have the same result with previous IC.
    // Default value is 0 (turn off) because of some MPEG-4 clip result is not good.
    u4RegVal = _MPV_McRead32(ucMpvId, RW_ECO_CTRL);
    MC_WRITE32(ucMpvId, RW_ECO_CTRL, (u4RegVal|DBK_FLR_NEW_MODE));
#endif

    #if 0 // ilii question
    {
        //for test
        u4RegVal = _MPV_McRead32(ucMpvId, RW_MC_DBK_FLR);   	
        //u4RegVal = ((u4RegVal & 0xFF00FFFF) | 0x50000);
        u4RegVal = ((u4RegVal & 0xFF00FFFF) | 0xff0000);    
        MC_WRITE32(ucMpvId, RW_MC_DBK_FLR, u4RegVal);    

        u4RegVal = _MPV_McRead32(ucMpvId, 0x238);   	
        u4RegVal = ((u4RegVal & 0xFFFFFEFF) | 0x100);
        MC_WRITE32(ucMpvId, 0x238, u4RegVal);    
    }
    #endif


#ifdef MPV_SEMI_HOST
    if(prMpvInfo->fgDbkEngrMode)
    {
        // dump mode, test for ink and parameter
    }
    else
    {
        //for golden comparison    
        MC_WRITE32(ucMpvId, RW_MC_DBLK_THR, 0x20004);
        if (prFbmPicHdr->ucPicStruct != MPEG_PIC_FRAME)
        {
            MC_WRITE32(ucMpvId, RW_MC_DBK_FLR, 0x3050038);
        }
        else
        {
            MC_WRITE32(ucMpvId, RW_MC_DBK_FLR, 0x305003a);
        }
        MC_WRITE32(ucMpvId, RW_MC_QPBPT, (8<<24|6<<16|4<<8|2));
        MC_WRITE32(ucMpvId, 0x704, 0x12343456);
    }
#endif    

}

void  _MPV_McGetDbkCnt(UCHAR ucMpvId, UCHAR ucEsId, UINT32* pu4DbkOfstCnt, UINT32* pu4Dbk4pCnt)
{
    UINT32 u4Ofst;
    UINT32 u4Deft;
    UINT32 u44p;
    UINT32 u4Ratio;
    UINT32 u4MbCnt;
    FBM_SEQ_HDR_T *prFbmSeqHdr;
    MPV_ES_INFO_T *prEsInfo;
    VDEC_ES_INFO_T *prVdecEsInfo;
    
    if(ucEsId >= MPV_MAX_ES)
    {
        LOG(0, "ucEsId >= MPV_MAX_ES\n");
        ASSERT(ucEsId < MPV_MAX_ES);
        ucEsId = (MPV_MAX_ES-1);
    }
    
    if(ucMpvId >= MPV_MAX_VLD)
    {
        LOG(0, "ucMpvId >= MPV_MAX_VLD\n");
        ASSERT(ucMpvId < MPV_MAX_VLD);
        ucMpvId = (MPV_MAX_VLD-1);
    }
    
    prEsInfo = (MPV_ES_INFO_T*)_MpvGetHdlr(ucEsId);    
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);    

    if((pu4DbkOfstCnt == NULL) || (pu4Dbk4pCnt == NULL))
    {
        LOG(1, "_MPV_McGetDbkCnt null pointer\n");
        return;
    }
    
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecEsInfo->ucFbgId);
    if(prFbmSeqHdr == NULL)
    {
        LOG(1, "_MPV_McGetDbkCnt prFbmSeqHdr null\n");
        u4MbCnt = (UINT32)(120*68);
    }
    else
    {
        u4MbCnt = (UINT32)((prVdecEsInfo->u2HSize *prVdecEsInfo->u2VSize) >> 8);
    }

#if defined(VDEC_IS_POST_MT5368)
    u4Ofst = PP_READ32(ucMpvId, RO_PP_DBK_OFST);
    *pu4DbkOfstCnt = u4Ofst;
    u4Deft = PP_READ32(ucMpvId, RO_PP_DBK_DEFT);
    *pu4Dbk4pCnt = u4Deft;
    u44p = PP_READ32(ucMpvId, RO_PP_DBK_4P);
#else
    {    
        UINT32 u4Reg;
    u4Reg = _MPV_McRead32(ucMpvId, RW_ECO_CTRL);
	MC_WRITE32(ucMpvId, RW_ECO_CTRL, (UINT32)(u4Reg | DBK_FLR_CNT));

#if defined(VDEC_IS_POST_MT5398)
    u4Ofst = _MPV_McRead32(ucMpvId, RO_MC_DBK_OFST) - 6*u4MbCnt + 3*(prVdecEsInfo->u2HSize / 16);
#else
    u4Ofst = _MPV_McRead32(ucMpvId, RO_MC_DBK_OFST);
#endif
    *pu4DbkOfstCnt = u4Ofst;
    u4Deft = _MPV_McRead32(ucMpvId, RO_MC_DBK_DEFT);
    *pu4Dbk4pCnt = u4Deft;
    u44p = _MPV_McRead32(ucMpvId, RO_MC_DBK_4P);//todo...
    }
#endif


    if(u4MbCnt == 0)
    {
        u4MbCnt = 1;
        LOG(1, "u4MbCnt 0\n"); 
    }


    {
        // Block have four edge, each edge have max four count.
        // One MB have six blocks.
        // in 100 %
        FBM_PIC_HDR_T *prFbmPicHdr;
        UINT32 u4MbCntField;
        u4MbCntField = u4MbCnt;
        prFbmPicHdr = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId,
            prVdecEsInfo->ucFbId);
        if(prFbmPicHdr)
        {
            if((!IS_FRMPIC(prFbmPicHdr)) && (u4MbCnt > 1))
            {
                u4MbCntField /= 2;
            }
        }
        prVdecEsInfo->u4Dbk1PixelCnt = ((u4Ofst) * 100) / (u4MbCntField * 6*16);
        prVdecEsInfo->u4Dbk3PixelCnt = ((u44p) * 100) / (u4MbCntField * 6*16);
        prVdecEsInfo->u4Dbk2PixelCnt = ((u4Deft) * 100) / (u4MbCntField * 6*16);
        if(prVdecEsInfo->u4Dbk1PixelCnt > 100)
        {
            prVdecEsInfo->u4Dbk1PixelCnt = 100;
        }
        if(prVdecEsInfo->u4Dbk3PixelCnt > 100)
        {
            prVdecEsInfo->u4Dbk3PixelCnt = 100;
        }
        if(prVdecEsInfo->u4Dbk2PixelCnt > 100)
        {
            prVdecEsInfo->u4Dbk2PixelCnt = 100;
        }

        /*
        prVdecEsInfo->u4Dbk1PixelCnt = u4Ofst;
        prVdecEsInfo->u4Dbk3PixelCnt = u44p;
        prVdecEsInfo->u4Dbk2PixelCnt = u4Deft;
        */
    }
    
    u4Ratio = ((u4Ofst + u44p) * 100) / (u4MbCnt * 6*16);

    prEsInfo->u4DbkRatio = u4Ratio;

    LOG(6, "Ratio(%d%) (%d, %d, %d)\n", u4Ratio, u4Deft, u44p, u4Ofst);    
    UNUSED(u4Deft);
}


#endif
#endif



//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

void _MPV_GetHwVer(void)
{
    UINT32 u4VldVer;
    ////UINT32 u4McVer;
    
    u4VldVer = _MPV_VldRead32(0, RW_VLD_VER);
    //u4McVer = _MPV_McRead32(0, RW_MC_VER);
    LOG(1, "VLD Version: 0x%x\n", u4VldVer);
    //Printf( "VLD Version: %x, MC Version: %x\n", u4VldVer, u4McVer);

}


void _MPV_PllInit(void)
{

    UINT16 u2MS, u2NS, u2Counter;
    UINT8 u1Band;
    //UINT32 u4BusClk;

    _u4DramClk = BSP_GetClock(SRC_MEM_CLK, &u1Band, &u2MS, &u2NS,
        &u2Counter);

    LOG(7, "DRAM_CLK: %d\n", _u4DramClk);
    UNUSED(_u4DramClk);

    //_eVer = 
    	//BSP_GetIcVersion();    

    // move to vdec_flw.c, VDEC_Init()
    /*
#ifdef CC_LOW_POWER
    IO_REG32(CKGEN_BASE, 0x21c) = (IO_REG32(CKGEN_BASE, 0x21c) & 0xFFFFFFF8) | 0x1;
#else
    IO_REG32(CKGEN_BASE, 0x21c) = (IO_REG32(CKGEN_BASE, 0x21c) & 0xFFFFFFF8) | 0x4;
#endif

    IO_REG32(DRAM_BASE, 0x54) = (IO_REG32(DRAM_BASE, 0x54) & 0xFFFFF00F) | 0x00000FF0;
    */
#if 0
    u4BusClk = BSP_GetClock(SRC_BUS_CLK, &u1Band, &u2MS, &u2NS, &u2Counter);
    LOG(6, "BUS_CLK: %d\n", u4BusClk);

    _u4WaitBusCycle = u4BusClk >> 1;
#endif

}


void _MPV_IsrInit(UCHAR ucMpvId)
{
        // reg ISR
        if(ucMpvId == 0)
        {
    #ifdef VDEC_COMMON_ISR
            _VDEC_RegIsr(VLD_VECTOR, _VldIsr);
    #else
            x_os_isr_fct pfnOldIsr;
            if (x_reg_isr(VLD_VECTOR, _VldIsr, &pfnOldIsr) != OSR_OK)
            {
                ASSERT(0);
            }
    #endif
        }
#ifdef CC_MT5890
        else if(ucMpvId == 1)
        {
    #ifdef VDEC_COMMON_ISR
            _VDEC_RegIsr(VLD_VECTOR_CORE1, _VldIsr);
    #else
            x_os_isr_fct pfnOldIsr;
            if (x_reg_isr(VLD_VECTOR_CORE1, _VldIsr, &pfnOldIsr) != OSR_OK)
            {
                ASSERT(0);
            }
    #endif        
        }
#endif

    #if defined(MPV_SEMI_HOST) && defined(VDEC_IS_POST_MT5368)
    {
        static BOOL fgDramBusyTest = FALSE;
        static BOOL fgDramBusyInit = FALSE;
        if((!fgDramBusyInit) && (fgDramBusyTest))
        {
            fgDramBusyInit = TRUE;
            IO_WRITE32((IO_VIRT + 0x07200),  0x18,
                (UINT32)(IO_READ32((IO_VIRT + 0x07200), 0x18)) | (UINT32)(1 << 25) | (UINT32)((UINT32)1 <<31));

            IO_WRITE32((IO_VIRT + 0x07200),  0x10, 0x7200000); //-> �o�ӬO�� DRAM ���� Busy �� Buffer �p�ߤ��n�\��z���ϥΪ��ϰ�
            IO_WRITE32((IO_VIRT + 0x07200),  0x14, 0x10000); //-> �W�z buffer �� size

            IO_WRITE32((IO_VIRT + 0x7100), 0x4, ((UINT32)(IO_READ32((IO_VIRT + 0x7100), 0x4))) & 0xFFFFFFF);

            IO_WRITE32((IO_VIRT + 0x7100), 0x14, 0xFFFFFF11);
        }
    }
    #endif

}


void _MPV_IsrStop(void)
{
    #ifdef VDEC_COMMON_ISR
    _VDEC_RegIsr(VLD_VECTOR, _VDEC_FakeVldIsr);

    #if (defined (CC_MT5890))
    _VDEC_RegIsr(VLD_VECTOR_CORE1, _VDEC_FakeVldIsr);
    #endif
    #else
    x_os_isr_fct pfnOldIsr;

    // dereg ISR
    if (x_reg_isr(VLD_VECTOR, NULL, &pfnOldIsr) != OSR_OK)
    {
        ASSERT(0);
    }
        
    #if (defined (CC_MT5890))
    if (x_reg_isr(VLD_VECTOR_CORE1, NULL, &pfnOldIsr) != OSR_OK)
    {
        ASSERT(0);
    }
    #endif
    #endif
}


void _MPV_VldHWReset(UCHAR ucEsId, UCHAR ucMpvId)
{
    UINT32 u4RegVal;
    //UINT32 u4Cnt = 0;
    UINT32 u4Cnt = 0;
    u4Cnt = 50000;

#ifdef TEST_ONOFF_RUNTIME
    VDEC_INFO_T *prVdecInfo = NULL;
    prVdecInfo = _VDEC_GetInfo();
    prVdecInfo->arVdecVldInfo[0].fgSwrst = TRUE;
#endif

    if(ucMpvId >= MPV_MAX_VLD)
    {
        LOG(0, "ucMpvId >= MPV_MAX_VLD\n");
        ASSERT(ucMpvId < MPV_MAX_VLD);
        ucMpvId = (MPV_MAX_VLD-1);
    }

#ifdef MPV_DUMP_SIM_PAT
    if(_bVdecLogSimPat)
    {
        //LOG(1, "wait(`VDEC_BITS_PROC_NOP == 1);\n");
        Printf("if(`VDEC_PROCESS_FLAG == 1)  wait(`VDEC_BITS_PROC_NOP == 1);\n");
    }
#endif
#if 0
#if defined(VDEC_IS_POST_MT5368)
    if(_MPV_VldRead32(ucMpvId, RO_VLD_SRAMCTRL) & PROCESS_FLAG)
    {
#endif
    while((_MPV_VldRead32(ucMpvId, RO_VLD_SRAMCTRL) & AA_FIT_TARGET_SCLK)==0)
    {
    u4Cnt++;
    if(u4Cnt >= MPV_WAIT_SRAM_THRD)
    {
    LOG(1, "WaitSramStable Fail\n");            
#ifdef MPV_DUMP_REG
    if (0)
    {
    _MPV_DumpReg(ucMpvId);
    }
#endif
    break;
    }
    }
#if defined(VDEC_IS_POST_MT5368)
    }
#endif
#endif

    if (_MPV_VldRead32(ucMpvId, RO_VLD_SRAMCTRL) & PROCESS_FLAG)
    {
        LOG(10, "wait RP stable\n");
        while((!(_MPV_VldRead32(ucMpvId, RO_VLD_SRAMCTRL)&AA_FIT_TARGET_SCLK)) && (u4Cnt--));
    }

#if defined(VDEC_IS_POST_MT5368)
    //VLD_WRITE32(ucMpvId, WO_VLD_SRST, 0x101);
    VLD_WRITE32(ucMpvId, WO_VLD_SRST, (0x1 |(0x1<<8) |(0x1<<16)));

    _VDEC_CkgenSel(ucEsId, VDEC_FMT_MPV, 0);

#ifdef CC_DYNAMIC_POWER_ONOFF
    /* MPEG2 power down mode setting
    ref to: \\mediatek.inc\taiwan\MM\MM_DE5\VDEC_PUBLIC\5_Document_To_SA\power_down\
    MPEG2 pdn_con_spec */
#if (defined (VDEC_IS_POST_MT5882))
    if(BSP_GetIcVersion() == IC_VER_5890_AC)
    {
        MISC_WRITE32(ucMpvId, 0xC8, 0xFFFE);
    }
    else
    {
        MISC_WRITE32(ucMpvId, 0xC8, 0x1FE);    
    }

    MISC_WRITE32(ucMpvId, 0x178, 0x7F);    
#else
    MISC_WRITE32(ucMpvId, 0xC8, 0xFE);
#endif
    // MPEG2 pdn_con_module
#ifdef CC_MPV_DEBLOCKING
    {
        VDEC_ES_INFO_T *prVdecEsInfo;
        UINT32 u4DramType = DRVCUST_InitGet(eDramType);
        prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
        if ((IS_DECFLAG_SET(prVdecEsInfo, DEC_FLAG_DBK)) ||
        (u4DramType == DDR_III_x2) || MPV_TEST_DDR3 )
        {
            // on deblocking h/w
#if (defined (VDEC_IS_POST_MT5882))
            MISC_WRITE32(ucMpvId, 0xCC, 0x7F6A151D);
#elif defined(VDEC_IS_POST_MT5398)
            MISC_WRITE32(ucMpvId, 0xCC, 0x3F6A151D);
#else
            MISC_WRITE32(ucMpvId, 0xCC, 0x3FDA951D);
#endif
        }
        else
        {
#if (defined (VDEC_IS_POST_MT5882))
            MISC_WRITE32(ucMpvId, 0xCC, 0x7F6A151D);
#elif defined(VDEC_IS_POST_MT5398)
            MISC_WRITE32(ucMpvId, 0xCC, 0x3F6A15FD);
#else
            MISC_WRITE32(ucMpvId, 0xCC, 0x3FDA95FD);
#endif
        }
    }
#else
#if (defined (VDEC_IS_POST_MT5882))
    if(BSP_GetIcVersion() == IC_VER_5890_AC)
    {
        MISC_WRITE32(ucMpvId, 0xCC, 0x7F6A15DD);                
    }
    else    
    {
        MISC_WRITE32(ucMpvId, 0xCC, 0x7F6A15FD);
    }
#elif defined(VDEC_IS_POST_MT5398)
    MISC_WRITE32(ucMpvId, 0xCC, 0x3F6A15FD);
#else
    MISC_WRITE32(ucMpvId, 0xCC, 0x3FDA95FD);
#endif
#endif
#endif
#if (defined (VDEC_IS_POST_MT5882))
    if(BSP_GetIcVersion() == IC_VER_5890_AC)
    {
        MISC_WRITE32(ucMpvId, 0x178, 0xFFFFFFFB);
    }
    else
    {
        MISC_WRITE32(ucMpvId, 0x178, 0x7F);
    }
#endif
    // MPEG2 use mid clock. 5389
    // must turn on this to enable clock, so we could write register correct.
    //MISC_WRITE32(ucMpvId, 0x84, 0x1);
    _VDEC_SysClkSel(ucMpvId, VDEC_FMT_MPV, 0);

#else
    VLD_WRITE32(ucMpvId, WO_VLD_SRST, 1);
#endif
    VLD_WRITE32(ucMpvId, WO_VLD_SRST, 0);
    //0812 for cobra 5398
#ifdef VDEC_IS_POST_MT5398
    VLD_WRITE32(ucMpvId,RW_VLD_NEW_MP2_VLD_MODE,0x30307);
    u4RegVal = (UINT32)((_MPV_VldRead32(ucMpvId, RWO_VLD_VBAR)));
    u4RegVal |= 1 << 28;
    VLD_WRITE32(ucMpvId,RWO_VLD_VBAR,u4RegVal);  // partial read only    
#endif
    u4RegVal = (UINT32)((_MPV_VldRead32(ucMpvId, RW_VLD_VDOUFM) & 0xFFFFF7FF) | (0x1 << 11));
    VLD_WRITE32(ucMpvId, RW_VLD_VDOUFM, u4RegVal);        

    // u4RegVal= (_MPV_VldRead32(ucMpvId, RW_VLD_STWD) & 0xFEFFFFFF) | (0x1000000);
    // VLD_WRITE32(ucMpvId, RW_VLD_STWD, u4RegVal);  
#if defined (CC_MT5360B) || defined (CC_MT5392B) 
    u4RegVal = ((_MPV_McRead32(ucMpvId, RW_MC_PP_WRAPPER) & 0x00FFFFFF) | 
    PP_WRAPPER_Y_LINES | PP_WRAPPER_CBCR_LINES);
    MC_WRITE32(ucMpvId, RW_MC_PP_WRAPPER, u4RegVal);
#endif
}


void _MPV_VldSetVFifo(UCHAR ucMpvId, UINT32 u4Sa, UINT32 u4Ea)
{
    if(ucMpvId >= MPV_MAX_VLD)
    {
        LOG(0, "ucMpvId >= MPV_MAX_VLD\n");
        ASSERT(ucMpvId < MPV_MAX_VLD);
        ucMpvId = (MPV_MAX_VLD-1);
    }
    
    VLD_WRITE32(ucMpvId,RW_VLD_VSTART, u4Sa >> 6);
    VLD_WRITE32(ucMpvId,RW_VLD_VEND, u4Ea >> 6);

#ifdef MPV_DUMP_SIM_PAT
    if(_bVdecLogSimPat)
    {
        Printf("//RW_VLD_VSTART(0x%x)\n", u4Sa);
        Printf("//RW_VLD_VEND(0x%x)\n", u4Ea);
        Printf("// d.save.binary d:/0x%x_0x%x.bin 0x%x--0x%x \n",
            u4Sa, u4Ea, u4Sa, u4Ea);
    }
#endif

#ifdef MPV_VIRTUAL_PRS
    _MPV_VldRisCtrlWptr(ucMpvId);
    VLD_WRITE32(ucMpvId,RW_VLD_SW_WP, 0xFFFFFFFE);
    VLD_WRITE32(ucMpvId,RW_VLD_SRHSTC_END, 0xFFFFFFF);    
#endif

}


BOOL _MPV_VldInitBarrelShifter(UCHAR ucMpvId, UINT32 u4Ptr)
{

    //UINT32 u4SramWptr, u4RdPtr;

    if(ucMpvId >= MPV_MAX_VLD)
    {
        LOG(0, "ucMpvId >= MPV_MAX_VLD\n");
        ASSERT(ucMpvId < MPV_MAX_VLD);
        ucMpvId = (MPV_MAX_VLD-1);
    }
    
    LOG(9, "Rptr 0x%x\n", u4Ptr);

    u4Ptr &= ~0xF;        // 16 Byte Alignment

#ifdef VDEC_IS_POST_MT5882
    if((_MPV_VldRead32(ucMpvId,RO_VLD_SRAMCTRL) & (0x1<<15)))
        while (!(_MPV_VldRead32(ucMpvId,RO_VLD_SRAMCTRL) & 1));
#endif
   

#ifdef MPV_DUMP_SIM_PAT
    if(_bVdecLogSimPat)
    {
        //LOG(1, "wait(`VDEC_BITS_PROC_NOP == 1);\n");
        Printf("if(`VDEC_PROCESS_FLAG == 1)  wait(`VDEC_BITS_PROC_NOP == 1);\n");
    }
#endif
    #if defined(VDEC_IS_POST_MT5368)
    if(_MPV_VldRead32(ucMpvId, RO_VLD_SRAMCTRL) & PROCESS_FLAG)
    {
        UINT32 u4Cnt = 0;
        while((_MPV_VldRead32(ucMpvId, RO_VLD_SRAMCTRL) & AA_FIT_TARGET_SCLK)==0)
        {
            u4Cnt++;
            if(u4Cnt >= MPV_WAIT_SRAM_THRD)
            {
                LOG(1, "WaitSramStable Fail\n");            
                #ifdef MPV_DUMP_REG
                if (0)
                {
                   _MPV_DumpReg(ucMpvId);
                }
                #endif
                break;
            }
        }
    }
    #endif

#ifdef MPV_DUMP_SIM_PAT
    if(_bVdecLogSimPat)
    {
        Printf("//ReadPointer(0x%x)\n", u4Ptr);
    }
#endif

    // Write DRAM Start Address
    VLD_WRITE32(ucMpvId, RW_VLD_RPTR, u4Ptr);

    //reset
    VLD_WRITE32(ucMpvId, WO_VLD_SRST, (0x1 << 8));
    VLD_WRITE32(ucMpvId, WO_VLD_SRST, 0); 

    // start to fetch data
    VLD_WRITE32(ucMpvId, RW_VLD_PROC, (UINT32)VLD_INIFET);

    if (_hVldTimer[ucMpvId] == (HANDLE_T)NULL)
    {
        VERIFY(x_timer_create(&_hVldTimer[ucMpvId]) == OSR_OK);        
    }

    if(!_WaitVldFetchOk(ucMpvId))
    {
    #if 0
        u4RdPtr = _MPV_VldRead32(ucMpvId, RO_VLD_VRPTR);
        u4SramWptr = (_MPV_VldRead32(ucMpvId, RO_VLD_VBAR) >> 16) & 0x3f;
        LOG(1, "Finish Fail! RdPtr = %x, Sramaa = %x\n", u4RdPtr, u4SramWptr);
    #endif
        return(FALSE);
    }
#if 0
    u4RdPtr = _MPV_VldRead32(ucMpvId, RO_VLD_VRPTR);
    u4SramWptr = (_MPV_VldRead32(ucMpvId, RO_VLD_VBAR) >> 16) & 0x3f;
    LOG(1, "Finish Fail! RdPtr = %x, Sramaa = %x\n", u4RdPtr, u4SramWptr);
#endif
    
    VLD_WRITE32(ucMpvId, RW_VLD_PROC, (UINT32)VLD_INIBR);

    LOG(8, "0xb0: 0x%x, 0xfc: 0x%x, 0xf8:0x%x\n",
        _MPV_VldRead32(ucMpvId, RW_VLD_RPTR),
        _MPV_VldRead32(ucMpvId, RO_VLD_VRPTR),
        _MPV_VldRead32(ucMpvId, RO_VLD_VWPTR));

    return(TRUE);

}


UINT32 _MPV_VldGetBitS(UCHAR ucMpvId, UCHAR ucShiftBit)
{
    UINT32 u4RegVal;

    u4RegVal = (UINT32)(_MPV_VldRead32(ucMpvId, RO_VLD_BARL + (ucShiftBit << 2)));

    return(u4RegVal);

}


void _MPV_VldResetMat(UCHAR ucMpvId)
{

    VLD_WRITE32(ucMpvId, RW_VLD_FDEC, (UINT32)VLD_RST_QMATRIX);

}


void _MPV_VldRldQMat(UCHAR ucMpvId, BOOL fgIntra)
{
    UINT32 u4RegVal = 0;

    u4RegVal = _MPV_VldRead32(ucMpvId, RW_VLD_FDEC);

    if (fgIntra)
    {
        VLD_WRITE32(ucMpvId, RW_VLD_FDEC, (UINT32)(VLD_RELOAD_INTRA |u4RegVal));
    }
    else
    {
        VLD_WRITE32(ucMpvId, RW_VLD_FDEC, (UINT32)(VLD_RELOAD_NONINTRA |u4RegVal));
    }
}


void _MPV_VldLoadQMat(UCHAR ucMpvId, const UINT32 *pu4Addr, BOOL fgIntra)
{
    UINT32 u4Idx;
    UINT32 u4Val;

    ASSERT(pu4Addr != NULL);

    if (fgIntra)
    {
        for (u4Idx = 0; u4Idx <= 15; u4Idx++)
        {
            u4Val = (UINT32)(VLD_WRITE_QMATRIX |(u4Idx << 2));
            VLD_WRITE32(ucMpvId, RW_VLD_RISC_QMATRIX, u4Val);
            VLD_WRITE32(ucMpvId, RW_VLD_QMATRIX_RW, pu4Addr[u4Idx]);
        }

    }
    else
    {
        for (u4Idx = 0; u4Idx <= 15; u4Idx++)
        {
            u4Val = (UINT32)(VLD_WRITE_QMATRIX | VLD_NONINTRA |(u4Idx << 2));
            VLD_WRITE32(ucMpvId, RW_VLD_RISC_QMATRIX, u4Val);
            VLD_WRITE32(ucMpvId, RW_VLD_QMATRIX_RW, pu4Addr[u4Idx]);
        }
    }
    
}


void _MPV_VldReadQMat(UCHAR ucMpvId, BOOL fgIntra)
{
    UINT32 u4Idx;
    UINT32 u4Val;

    if (fgIntra)
    {
        for (u4Idx = 0; u4Idx<= 63; u4Idx +=4)
        {
            u4Val = (UINT32)(VLD_READ_QMATRIX |u4Idx);
            VLD_WRITE32(ucMpvId, RW_VLD_RISC_QMATRIX, u4Val);
            LOG(10, "%x\n", _MPV_VldRead32(ucMpvId, RW_VLD_QMATRIX_RW));
        }
    }
    else
    {
        for (u4Idx = 0; u4Idx<= 63; u4Idx +=4)
        {
            u4Val = (UINT32)(VLD_READ_QMATRIX |VLD_NONINTRA |u4Idx);
            VLD_WRITE32(ucMpvId, RW_VLD_RISC_QMATRIX, u4Val);
            LOG(10, "%x\n", _MPV_VldRead32(ucMpvId, RW_VLD_QMATRIX_RW));
        }
    }

}

/*
void _MPV_VldAutoLoadQMat(UCHAR ucMpvId, BOOL fgIntra)
{
    UINT32 u4VldDummy;

    // hardware will get 64 byte auto  
    // use u4VldDummy to prevent code being optimize by compiler
    if (fgIntra)
    {
        u4VldDummy = _MPV_VldRead32(ucMpvId, RW_VLD_TABLIM);
    }
    else
    {
        u4VldDummy = _MPV_VldRead32(ucMpvId, RW_VLD_TABLNIM);  
    }
    UNUSED(u4VldDummy);

}
*/

void _MPV_VldPower(UCHAR ucMpvId, BOOL fgOn)
{

    if(fgOn)
    {
        VLD_WRITE32(ucMpvId, RW_VLD_PWRSAVE, 0);
    }
    else
    {
        VLD_WRITE32(ucMpvId, RW_VLD_PWRSAVE, 1);
    }

}


void _MPV_VldDecPrmProc(UCHAR ucMpvId, UCHAR ucEsId, MPV_ES_INFO_T *prEsInfo,
                        const FBM_SEQ_HDR_T *prFbmSeqHdr, const FBM_PIC_HDR_T *prFbmPicHdr)
{
    UINT16 u2HSizeMB;
    #if defined(CC_MT5387) && !defined(CC_MT5363)
    #else
    UINT16 u2HSize;    
    #endif
    UINT16 u2VSize;
    UINT16 u2HSizeMBtoMC;
    UINT16 u2VSizeMB;
    UINT32 u4RegVal1;
    UINT32 u4RegVal2;
	#ifndef VDEC_IS_POST_MT5368
	UINT32 u4RegVal3;
	#endif
    UINT32 u4Mbl = 0;
    VDEC_ES_INFO_T *prVdecEsInfo;
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    ASSERT(prEsInfo != NULL);
    ASSERT(prFbmPicHdr != NULL);
    ASSERT(prFbmSeqHdr != NULL);

    u4RegVal1 = (UINT32)(((prFbmPicHdr->ucPicStruct & 0x3 )<< 30) +
                       (prFbmPicHdr->fgFrmPredFrmDct << 29) +
                       (prFbmPicHdr->fgConcMotionVec << 28) +
                       (prFbmPicHdr->fgQScaleType << 27) +
                       (prFbmPicHdr->fgTopFldFirst << 26) +
                       (prFbmPicHdr->ucFullPelFordVec << 25) +
                       (prFbmPicHdr->ucFullPelBackVec << 24) +
                       (prFbmPicHdr->ucPicCdTp << 21) +
                       (prFbmPicHdr->fgIntraVlcFmt << 20) +
                       (prFbmPicHdr->ucIntraDcPre << 18) +
                       (prFbmPicHdr->fgAltScan << 17));

    if(prEsInfo->ucMpegVer == MPEG1)
    {
        u4RegVal1 = (UINT32)(u4RegVal1 +
                           (prFbmPicHdr->ucFordFCode << 12) +
                           (prFbmPicHdr->ucFordFCode << 8) +
                           (prFbmPicHdr->ucBackFCode << 4) +
                           (prFbmPicHdr->ucBackFCode));
    }
    else
    {
        u4RegVal1 = (UINT32)(u4RegVal1 +
                           (prFbmPicHdr->ucfcode[0][0] << 12) +
                           (prFbmPicHdr->ucfcode[0][1] << 8) +
                           (prFbmPicHdr->ucfcode[1][0] << 4) +
                           (prFbmPicHdr->ucfcode[1][1]));
    }

    VLD_WRITE32(ucMpvId, RW_VLD_PARA, u4RegVal1);

    u2HSizeMB = (UINT16)((prVdecEsInfo->u2OrgHSize + 15) >> 4);
    u2HSizeMBtoMC = (UINT16)((prVdecEsInfo->u2LineSize + 15) >> 4);
    u2VSize = (UINT16)(MPV_ALIGN_MASK(prVdecEsInfo->u2OrgVSize, 0xf));
    #if defined(CC_MT5387) && !defined(CC_MT5363)
    #else
    u2HSize = (UINT16)(MPV_ALIGN_MASK(prVdecEsInfo->u2OrgHSize, 0xf));
    #endif

    if (prVdecEsInfo->ucPicType == B_TYPE)
    {
        {
            u4Mbl = 0xff;                       
        }    
    }
    else
    {
        u4Mbl = 0xff;                
    }

    if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLAG_REDUCE_REF))    
    {   
        //for golden compare, ilii
        #ifdef MPV_SEMI_HOST
        u2HSizeMBtoMC = (UINT16)(prVdecEsInfo->u2OrgHSize) >> 4;          
        #else
        u2HSizeMBtoMC = (UINT16)(prVdecEsInfo->u2LineSize) >> 4;
        #endif
    }

    #if defined(VDEC_IS_POST_MT5368)
    #if 0 // correct one? ask SJ
    u4RegVal2 = (((UINT32)IS_MPEG1(prEsInfo)) << 31) + // mpeg1
        (((UINT32)!IS_MPEG1(prEsInfo)) << 30) +        // mpeg2
        (u2VSize << 16) + 0x1FF;
    #else
    u4RegVal2 = (((UINT32)IS_MPEG1(prEsInfo)) << 31) + // mpeg1
        (((UINT32)!IS_MPEG1(prEsInfo)) << 30) +        // mpeg2
        (u2VSize << 16) + (u4Mbl << 8);
    #endif
    UNUSED(u2HSizeMB);
    #else
    u4RegVal2 = (((UINT32)IS_MPEG1(prEsInfo)) << 31) + (u2VSize << 16) + (u4Mbl << 8) +
                       (u2HSizeMB);
    #endif

    VLD_WRITE32(ucMpvId, RW_VLD_PICSZ, u4RegVal2);

    #if defined(VDEC_IS_POST_MT5368)
    MC_WRITE32(ucMpvId, RW_MC_MC_PITCH, u2HSizeMBtoMC);
    #else
    VLD_WRITE32(ucMpvId, RW_VLD_MCPICSZ, u2HSizeMBtoMC);
    #endif

    #if defined(CC_MT5387) && !defined(CC_MT5363)
    #else
    MC_WRITE32(ucMpvId, RW_MC_UMV_W, u2HSize);
    MC_WRITE32(ucMpvId, RW_MC_UMV_H, u2VSize);   
	#endif

    //raster
    //VLD_WRITE32(ucMpvId, RW_VLD_MCPICSZ, 45);
    //VLD_WRITE32(ucMpvId, RW_VLD_MCPICSZ, 0x78);
	#ifdef CC_MT5880
    if(IS_IC_5880_ES2()) //only for 5880es2. SJ for Error Concealment. xiaolei.li
    {
    	//set vld_50[15..12] = b'1010
    	VLD_WRITE32(ucMpvId, RW_VLD_MCPICSZ, (VLD_READ32(ucMpvId, RW_VLD_MCPICSZ) | (1<<15) | (1<<13)));
        VLD_WRITE32(ucMpvId, RW_VLD_MCPICSZ, (VLD_READ32(ucMpvId, RW_VLD_MCPICSZ) & (~(1<<12)) & (~(1<<14))));
    }    
	#endif
    #if defined(VDEC_IS_POST_MT5368)
    u2VSizeMB = (UINT16)((u2VSize >> 4));
    #if 0 // correct one? ask SJ
    VLD_WRITE32(ucMpvId, RW_VLD_MBROWPRM, (0x1FF));
    #else
    VLD_WRITE32(ucMpvId, RW_VLD_MBROWPRM, (u2VSizeMB & 0x3FF));
    #endif

    if (IS_FRMPIC(prFbmPicHdr))
    {
        VLD_TOP_WRITE32(ucMpvId, RW_VLD_TOP_PIC_PIX_SIZE, ((u2VSize << 16) | (u2HSize)));
    }
    else
    {
        // field
        VLD_TOP_WRITE32(ucMpvId, RW_VLD_TOP_PIC_PIX_SIZE, (((u2VSize>>1) << 16) | (u2HSize)));
    }

    #else
    u2VSizeMB = (UINT16)((u2VSize >> 4) -1);

    u4RegVal3 |= (UINT32)((u2VSizeMB & 0x000000FF) << 16);

    VLD_WRITE32(ucMpvId, RW_VLD_MBROWPRM, u4RegVal3);
    #endif

#ifdef CC_MPV_DEBLOCKING
    if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLAG_DBK))
    {
        if ((prVdecEsInfo->ucPicType != B_TYPE) || (!prEsInfo->fgBNoDbk))
        {
            _SetDbkPrmProc(ucEsId, ucMpvId, prEsInfo, prFbmSeqHdr, prFbmPicHdr);       
            prEsInfo->u4DbkCnt++;
        }
        else
        {
            // ilii change to 5 after dbk threshold ready
            LOG(6, "BNoDBK\n");
            prEsInfo->u4NoDbkCnt++;
        }
    }

#endif

}


void _MPV_VldPartDec(UCHAR ucMpvId, UCHAR ucDecFld)
{
    UINT32 u4Proc = 0;

    u4Proc |= (UINT32)VLD_B21EN;

    if(ucDecFld == MC_TOP_FLD)
    {
        u4Proc |= VLD_DECTOP;
    }
    else
    {
        u4Proc |= VLD_DECBTM;
    }

    VLD_WRITE32(ucMpvId, RW_VLD_PROC, u4Proc);
}


void _MPV_VldDec(UCHAR ucMpvId, UCHAR ucEsId, const MPV_ES_INFO_T *prEsInfo)
{
    UINT32 u4Proc = 0;
#ifdef MPV_TESTCODE    
    UINT32 u4McCtrl = 0;
#endif
    VDEC_ES_INFO_T *prVdecEsInfo;

#if defined(CC_MT5387) && !defined(CC_MT5363)
#else
    PARAM_MPV_T* prMpvCounter;
#endif
    
//#if defined (CC_MT5360B) || defined (CC_MT5392B) || defined(CC_MT5365)  || defined(CC_MT5395)
    FBM_PIC_HDR_T *prFbmPicHdr = NULL;
//#endif

    if(ucEsId >= MPV_MAX_ES)
    {
        LOG(0, "ucEsId >= MPV_MAX_ES\n");
        ASSERT(ucEsId < MPV_MAX_ES);
        ucEsId = (MPV_MAX_ES-1);
    }
    
    if(ucMpvId >= MPV_MAX_VLD)
    {
        LOG(0, "ucMpvId >= MPV_MAX_VLD\n");
        ASSERT(ucMpvId < MPV_MAX_VLD);
        ucMpvId = (MPV_MAX_VLD-1);
    }
    
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    UNUSED(prVdecEsInfo);
    UNUSED(prFbmPicHdr);
    
#if defined(CC_MT5387) && !defined(CC_MT5363)
#else
    prMpvCounter = (PARAM_MPV_T*)prVdecEsInfo->prMpvCounter;
#endif

    ASSERT(prEsInfo != NULL);
    #if defined (VDEC_IS_POST_MT5368)
    // bit 4 off is mv overflow -> do error concealment
    // bit 4 on is mv overflow -> change mv = 0 and decode
    VLD_WRITE32(ucMpvId, RW_VLD_ERRST, 0x30); // reserve <11:31> bits
    #else
    switch (prEsInfo->ucECMethod)
    {
        case MPV_EC_5381_IPB:
            VLD_WRITE32(ucMpvId, RW_VLD_ERRST, 0xff078000);
            break;

        case MPV_EC_5381_PB:
            VLD_WRITE32(ucMpvId, RW_VLD_ERRST, 0x78078800);
            break;

        case MPV_EC_1389:
            VLD_WRITE32(ucMpvId, RW_VLD_ERRST, 0x78078820);
            break;

        default:
            VLD_WRITE32(ucMpvId, RW_VLD_ERRST, 0x78078000);
            break;
    }
    #endif

    #ifdef MPV_SEMI_HOST
    {
        VDEC_INFO_T *prVdecInfo;
        prVdecInfo = _VDEC_GetInfo();
        if(prVdecInfo->fgIgnoreErr)
        {
            VLD_WRITE32(ucMpvId, RW_VLD_EC_SETTING, 0x300 |
                MC_VLD_MBX_EQ_CHECK);
        }
        else
        {
            VLD_WRITE32(ucMpvId, RW_VLD_EC_SETTING, 0x300 |
                OFF_MB_ADDR_INC | OFF_HEADER2_ERR | MC_VLD_MBX_EQ_CHECK);
            // OFF_MB_ADDR_INC | OFF_HEADER2_ERR -> cause MC to write other address
            // You should NOT use this bit in product.
            // When you test MID, you could turn this off.
        }
    }
    #else
// this is for sarnoff bitstream only
//#if defined (CC_MT5360B) || defined (CC_MT5392B) || defined (CC_MT5363)
//    VLD_WRITE32(ucMpvId, RW_VLD_EC_SETTING, 0x300 | 
//        OFF_MB_ADDR_INC | OFF_HEADER2_ERR | MC_VLD_MBX_EQ_CHECK);
//
//#else
    /*if(prVdecEsInfo->fgMMPlayback)
    {
        VLD_WRITE32(ucMpvId, RW_VLD_EC_SETTING,
            0x300 | MC_VLD_MBX_EQ_CHECK | (0x1<<4) | (0x1<<1));
    }
    else*/ // DTV
    {
        if(IS_MPEG1(prEsInfo))
        {
            // Following two bit only implement on MPEG2 format:
            // SLICE_REDECODE, INSUFICIENT_MBX
            // So we should turn off it in MPEG1 format.
            VLD_WRITE32(ucMpvId, RW_VLD_EC_SETTING,
                (UINT32)(0x300 | OFF_SLICE_REDECODE |
                OFF_INSUFICIENT_MBX | MC_VLD_MBX_EQ_CHECK));
        }
        else
        {   
            
            // SJ comment
            #if defined(CC_MT5398)
            if(BSP_GetIcVersion() == IC_VER_5398_AB || BSP_GetIcVersion() == IC_VER_5398_AC)
            {
                UINT32 u4RegVal;
                
                VLD_WRITE32(ucMpvId, RW_VLD_EC_SETTING, 
                    (UINT32)(0x300 | IS_NEW_CFG_1 |MC_VLD_MBX_EQ_CHECK));            
                u4RegVal = PP_READ32(ucMpvId, RW_PP_DUMMY);
                PP_WRITE32(ucMpvId, RW_PP_DUMMY, (u4RegVal|IS_DUMMY));
            }
            else
            #endif
            {                
                #if defined (VDEC_IS_POST_MT5368)
                VLD_WRITE32(ucMpvId, RW_VLD_EC_SETTING,
                    (UINT32)(0x300 | OFF_HEADER2_ERR | MC_VLD_MBX_EQ_CHECK));
                #else            
                VLD_WRITE32(ucMpvId, RW_VLD_EC_SETTING, (UINT32)(0x300 | MC_VLD_MBX_EQ_CHECK));
                #endif            
            }
        }
    }
//#endif
    #endif


#ifdef MPV_TESTCODE
    //for EC setting test
    switch (prEsInfo->u4EcSetting)
    {
    case 0:
        VLD_WRITE32(ucMpvId, RW_VLD_EC_SETTING, 0x300);        
        break;
    case 1:    
        VLD_WRITE32(ucMpvId, RW_VLD_EC_SETTING, 0x340);            
        break;
    case 2:
        // turn-off mv_overflow
        VLD_WRITE32(ucMpvId, RW_VLD_EC_SETTING, 0x380);            
        break;
    case 3:    
        VLD_WRITE32(ucMpvId, RW_VLD_EC_SETTING, 0x3c0);                
        break;
    case 4:            
        VLD_WRITE32(ucMpvId, RW_VLD_EC_SETTING, 0x304);                
    default:
        break;
    }

    //MC pipeline
    if (prEsInfo->fgMcPipe)
    {
        MC_WRITE32(ucMpvId, RW_MC_DPRE, 1); 
    }
    else
    {
        MC_WRITE32(ucMpvId, RW_MC_DPRE, 0);     
    }

    u4McCtrl = _MPV_McRead32(ucMpvId, RW_MC_CTRL_SIGNAL); 

    if (prEsInfo->fgMcFastAddr)
    {
        u4McCtrl |= FAST_DAC; 
    }
    else
    {
        u4McCtrl &= (~FAST_DAC); 
    }

   if (prEsInfo->fgMcNewFsm)
    {
        u4McCtrl |= NEW_FSM; 
    }
    else
    {
        u4McCtrl &= (~NEW_FSM); 
    }

    MC_WRITE32(ucMpvId, RW_MC_CTRL_SIGNAL, u4McCtrl);        
#endif

	#if defined(CC_MT5360) || defined(CC_MT5391)
    if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLAG_REDUCE_REF))    
    {
        UINT32 u4RegVal;

        u4RegVal = _MPV_McRead32(ucMpvId, RW_MC_RRCTL);
        u4RegVal &= (~RR_BYPASS);

        if (prEsInfo->u4RRatio == RR_RATIO_34)
        {
            MC_WRITE32(ucMpvId, RW_MC_RRCTL, (UINT32)(u4RegVal|RR_34_Y|RR_34_C|RR_EN_Y|RR_EN_C));  
            //MC_WRITE32(ucMpvId, RW_MC_RRCTL, 0x4c111110);  
        }
        else
        {
            MC_WRITE32(ucMpvId, RW_MC_RRCTL, (UINT32)(u4RegVal|RR_EN_Y|RR_EN_C));          
            //MC_WRITE32(ucMpvId, RW_MC_RRCTL, 0x4c010110);  
            
        }
        
        #if defined (CC_MT5360B) || defined (CC_MT5392B)        
        #ifndef MPV_SEMI_HOST
        prFbmPicHdr = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, prVdecEsInfo->ucFbId);
        if(prFbmPicHdr == NULL)
        {
        	LOG(0, "prFbmPicHdr null\n");
            ASSERT(prFbmPicHdr);
            return;
        }    
        if (IS_FRMPIC(prFbmPicHdr))
        {
        }
        else
        {
            u4RegVal = _MPV_McRead32(ucMpvId, RW_MC_RR_FIELDCTL);
            MC_WRITE32(ucMpvId, RW_MC_RR_FIELDCTL, (u4RegVal|
            RR_PP_C_FIELD_EN|RR_PP_Y_FIELD_EN|
            RR_MC_C_FIELD_EN|RR_MC_Y_FIELD_EN));
        }
        #endif

        // turn off "request connection" in RR mode.
        u4RegVal = MC_READ32(ucMpvId, 0x5E4);
        u4RegVal |= (UINT32)((1<<12));
        MC_WRITE32(ucMpvId, 0x5E4, u4RegVal);
        #endif        
    }
    else // set RR bypass
    {
        #ifdef CC_MT5360
        #ifndef CC_MT5360B // A, ECO version.
        MC_WRITE32(ucMpvId, RW_MC_RRCTL, (MC_READ32(ucMpvId, RW_MC_RRCTL) & 0xFFFFFFFE));
        MC_WRITE32(ucMpvId, RW_MC_PP_RRCTL, (MC_READ32(ucMpvId, RW_MC_PP_RRCTL) & 0xFFFFFFFE));  
        #endif
        #endif
        #ifdef CC_MT5391
        #ifndef CC_MT5392B // A, ECO version.
        MC_WRITE32(ucMpvId, RW_MC_RRCTL, (MC_READ32(ucMpvId, RW_MC_RRCTL) & 0xFFFFFFFE));
        MC_WRITE32(ucMpvId, RW_MC_PP_RRCTL, (MC_READ32(ucMpvId, RW_MC_PP_RRCTL) & 0xFFFFFFFE));  
        #endif
        #endif
    }
	#endif
    //#if defined(CC_MT5363) || defined(CC_MT5365) || defined(CC_MT5395)
    #if defined(CC_MT5363) || defined(VDEC_IS_POST_MT5365)
    //#ifdef CC_MT5363
    //ECO Control 3 Register & new modes. MC
    //0: enable the H.264 request reorder   1:disable
    {
        UINT32 u4RegVal;
        u4RegVal = MC_READ32(ucMpvId, 0x5E4);
        u4RegVal |= (UINT32)((1<<12));
        MC_WRITE32(ucMpvId, 0x5E4, u4RegVal);
    }
    #endif

    //VLD_WRITE32(ucMpvId, RW_VLD_DEC_TIMEOUT, _u4WaitBusCycle);           

#ifdef CC_MPV_DEBLOCKING
    //if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLAG_DBK))
    //{
       //MC_WRITE32(ucMpvId, RW_MC_WAITVLD, 0x1);    
    //}
#endif    

    // 5387 -> move to register 52 [1] no need to set because no Intra EC now
    #if defined (VDEC_IS_POST_MT5368)
    // do nothing
    UNUSED(prMpvCounter);
    #elif defined(CC_MT5387) && !defined(CC_MT5363)
    #else
    if (prMpvCounter->u4DecOkNs == 0)
    {
        UINT32 u4RegVal;

        u4RegVal = (_MPV_VldRead32(ucMpvId, RW_VLD_STWD) & 0xFFFEFFFF) | VLD_FIRST_PIC;
        VLD_WRITE32(ucMpvId, RW_VLD_STWD, u4RegVal);
    }
    #endif

    #if defined(CC_MT5387) && !defined(CC_MT5363)
    // 5387 -> enable burst read
    {
        UINT32 u4Reg = _MPV_McRead32(ucMpvId, 0X320);
        u4Reg &= 0xFFFFFFFE;
        MC_WRITE32(ucMpvId, 0X320, u4Reg);
    }
    #endif
    
    u4Proc |= VLD_PDHW;  // Picture start to decode
    u4Proc |= 0xFF;  // mb_end address
#if defined (VDEC_IS_POST_MT5368)
    VLD_WRITE32(ucMpvId, RW_VLD_MBHP, 0x3FF);  // mb_end address
#endif

    //for ec test
    if (prEsInfo->u4EcSetting == 5)
    {
        u4Proc |=0x2000000;
    }

    //raster
    //MC_WRITE32(ucMpvId, RW_MC_DRAM_MODE, 3);
    
    _MPV_GetStartTime(ucMpvId);


#ifdef CC_MT5360
    if (BSP_GetIcVersion() < IC_VER_5360_AD) // A version
    {
    }
    else
    {
        #ifdef CC_MT5360B // B version
        #else // ECO version
            //    VLD_WRITE32(ucMpvId, RW_VLD_EC_SETTING, 0x300 | 
            //        OFF_MB_ADDR_INC | OFF_HEADER2_ERR | MC_VLD_MBX_EQ_CHECK);
            {
                INT32 myreg;
                myreg = MC_READ32(ucMpvId, 0x5E4);
                myreg |= 1;
                MC_WRITE32(ucMpvId, 0x5E4, myreg);
            }
        #endif
    }
#endif

    #if defined(CC_MT5363) && !defined(CC_MT5365) && !defined(CC_MT5395)
    //#ifdef CC_MT5363
    // this is deblocking setting
    {
        INT32 myreg;

        /*
        if (prVdecEsInfo->ucPicType == B_TYPE)
        {
        myreg = MC_READ32(ucMpvId, RW_MC_DBK_Y);
        MC_WRITE32(ucMpvId, RW_MC_BY1, myreg);
        myreg *= 2;
        MC_WRITE32(ucMpvId, (64*4), myreg);

        myreg = MC_READ32(ucMpvId, RW_MC_DBK_C);
        MC_WRITE32(ucMpvId, RW_MC_BC1, myreg);
        myreg *= 2;
        MC_WRITE32(ucMpvId, (65*4), myreg);

        myreg = MC_READ32(ucMpvId, RW_MC_BC1);
        myreg *= 2;
        MC_WRITE32(ucMpvId, (65*4), myreg);
        }*/

        myreg = MC_READ32(ucMpvId, RW_MC_BY1);
        myreg *= 2;
        MC_WRITE32(ucMpvId, (64*4), myreg);

        myreg = MC_READ32(ucMpvId, RW_MC_BC1);
        myreg *= 2;
        MC_WRITE32(ucMpvId, (65*4), myreg);
    }

    #endif

    #if defined(CC_MT5365) || defined(VDEC_IS_POST_MT5368)
    {
        // this is PP out
        UINT32 u4DramType = DRVCUST_InitGet(eDramType);
        if((u4DramType == DDR_III_x2) | MPV_TEST_DDR3)
        {
            //DDR3 32 byte access
            UINT16 u2HSizeMB;
            UINT16 u2VSizeMB;
            UINT16 u2HSizeMBtoMC;
            UINT32 u4SliceIdx;

            {
                // Enable PP to output MC data
                MC_WRITE32(ucMpvId, RW_MC_POSTEN, (UINT32)POSTEN);
#ifdef CC_MPV_DEBLOCKING
                if((!IS_DECFLAG_SET(prVdecEsInfo, DEC_FLAG_DBK))
                   ||
                   ((IS_DECFLAG_SET(prVdecEsInfo, DEC_FLAG_DBK)) &&
                    (prVdecEsInfo->ucPicType == B_TYPE)))
#endif
                {
                    MC_WRITE32(ucMpvId, RW_MC_WPOST, (UINT32)DISABLE_MC_WRITE);
                }
                u2HSizeMB = (UINT16)((prVdecEsInfo->u2OrgHSize + 15) >> 4);
                u2VSizeMB = (UINT16)((prVdecEsInfo->u2OrgVSize + 15) >> 4);
                u2HSizeMBtoMC = (UINT16)((prVdecEsInfo->u2LineSize + 15) >> 4); 
                MC_WRITE32(ucMpvId, RW_MC_POST_WD, (UINT32)u2HSizeMBtoMC);
        
                prFbmPicHdr = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, prVdecEsInfo->ucFbId);
                if(prFbmPicHdr == NULL)
                {
                	LOG(0, "prFbmPicHdr null\n");
                    ASSERT(prFbmPicHdr);
                    return;
                }    
                if (prFbmPicHdr->ucPicStruct != MPEG_PIC_FRAME)
                {
                    u2VSizeMB = (UINT16)(u2VSizeMB >> 1);
                }

                MC_WRITE32(ucMpvId, RW_MC_HMBRNG, (UINT32)(u2HSizeMB - 1));
                u4SliceIdx = (_MPV_VldRead32(ucMpvId, RO_VLD_BARL) & 0xFF);
                UNUSED(u4SliceIdx);
                #if defined(VDEC_IS_POST_MT5368)
                MC_WRITE32(ucMpvId, RW_MC_VMBRNG, (UINT32)(u2VSizeMB - 1));
                #else
                MC_WRITE32(ucMpvId, RW_MC_VMBRNG, (UINT32)(((u4SliceIdx-1) << 8)|(u2VSizeMB - 1)));
                #endif
            }
            
            MC_WRITE32(ucMpvId, RW_MC_32B_MODE_EN1, MC_READ32(ucMpvId, RW_MC_32B_MODE_EN1) | 1);
            MC_WRITE32(ucMpvId, RW_MC_32B_MODE_EN2, MC_READ32(ucMpvId, RW_MC_32B_MODE_EN2) | 1);
            // bit0 = 0, 0: turn on pp_out_buf
            MC_WRITE32(ucMpvId, RW_MC_PP_OUT_32B_MODE_EN, 
                MC_READ32(ucMpvId, RW_MC_PP_OUT_32B_MODE_EN) & 0xFFFFFFFE);
        }
        #if defined(VDEC_IS_POST_MT5368)
        else
        {
            // PP output pp_out_buf
            // bit 0 = 1, 1: turn off pp_out_buf
            MC_WRITE32(ucMpvId, RW_MC_PP_OUT_32B_MODE_EN, 
                MC_READ32(ucMpvId, RW_MC_PP_OUT_32B_MODE_EN) | 0x1);
        }
        #endif
        //
        // +PP out
        // +PP out with pp_out_buf
        // +dbk
        // +PP out
        // MC_BASE #136 0x220 <0> = 1
        // +pp_out_buf
        // MC_BASE #525 0x834 <0> = 0
        // +dbk
        // MC_BASE #142 0x238 <1:0> = 0x3
    }
    #endif

    #if defined(VDEC_IS_POST_MT5368)
    {
        if(IS_MPEG1(prEsInfo))
        {
            PP_WRITE32(ucMpvId, RW_PP_DBK_EC,
                PP_READ32(ucMpvId, RW_PP_DBK_EC) | MPEG1_DBK_BWEC);
        }
    }
    #endif

   #ifdef MPV_DUMP_REG
    {
        static BOOL fgMPVDumpBeforeTrigger = FALSE;
        if (fgMPVDumpBeforeTrigger)
        {
            LOG(3, "fgMPVDumpBeforeTrigger\n");
            _MPV_DumpReg(ucMpvId);
            LOG(3, "~fgMPVDumpBeforeTrigger\n");
        }
        
        if(_fgDumpMCData)
        {
            MC_WRITE32(ucMpvId, RW_MC_PARA_BEHAVIOR,
                MC_READ32(ucMpvId, RW_MC_PARA_BEHAVIOR) | PARA4_BEHAVIOR);
        }
    }
   #endif

   // only for python, bit0->1
#ifdef CC_MT5880
   if (IS_IC_5860_ES2())
   {
       //vVDecWriteVLD(u4VDecID, RO_AVLD_ERR_MESSAGE, (u4VDecReadVLD(u4VDecID, RO_AVLD_ERR_MESSAGE) | 0x1));
       VLD_WRITE32(ucMpvId, RW_VLD_EC_SETTING, _MPV_VldRead32(ucMpvId, RW_VLD_EC_SETTING) | 0x1);    
   }
#endif

#ifdef MPV_DUMP_SIM_PAT
    if(_bVdecLogSimPat)
    {
        Printf("@(posedge `VDEC_INTERRUPT); // trigger decode\n");
    }
#endif


    VLD_WRITE32(ucMpvId, RW_VLD_PROC, u4Proc);

}


UINT16 _MPV_VldErrCnt(UCHAR ucMpvId)
{

    return (UINT16)((_MPV_VldRead32(ucMpvId, RO_VLD_ERRFG) >> 8) & 0xFFF);

}

UINT8 _MPV_VldLastErrRow(UCHAR ucMpvId)
{
    return (UINT8)((_MPV_VldRead32(ucMpvId, RO_VLD_ERRFG) >> 24) & 0x3F);
}


UINT32 _MPV_VldLastErrType(UCHAR ucMpvId)
{
    return (UINT32)(_MPV_VldRead32(ucMpvId, RO_VLD_ERRFG) & 0x3F);
}


UINT32 _MPV_VldErrStatus(UCHAR ucMpvId)
{
    return (UINT32)(_MPV_VldRead32(ucMpvId, RW_VLD_ERR_STATUS));
}


UINT32 _MPV_VldErrMb(UCHAR ucMpvId)
{
    return (UINT32)(_MPV_VldRead32(ucMpvId, RO_VLD_ERR_MB) & 0x1fff);
}


void _MPV_VldRisCtrlWptr(UCHAR ucMpvId)
{
    UINT32 u4RegVal;
#if defined (VDEC_IS_POST_MT5368)
    // do nothing
    UNUSED(u4RegVal);
#elif defined (CC_MT5360B) || defined (CC_MT5392B) || defined(CC_MT5365) || defined(CC_MT5395)
    u4RegVal = (_MPV_VldRead32(ucMpvId, RW_VLD_STWD) & 0xFFFCFFFF)|
                           VLD_STWD_SW_WP_EN|VLD_STWD_LATCH_WP_EN;
    VLD_WRITE32(ucMpvId, RW_VLD_STWD, u4RegVal);
#else // include 5387
    u4RegVal = (_MPV_VldRead32(ucMpvId, RW_VLD_SW_WP) & 0xFFFFFFF8)|
                           VLD_SW_WP_EN|VLD_LATCH_WP_EN;
    VLD_WRITE32(ucMpvId, RW_VLD_SW_WP, u4RegVal);
#endif
}


void _MPV_VldSetWptr(UCHAR ucMpvId, UINT32 u4VldWptr)
{
//#if defined (CC_MT5360B) || defined (CC_MT5392B) || defined(CC_MT5365) || defined(CC_MT5395)
#if defined (CC_MT5360B) || defined (CC_MT5392B) || defined(VDEC_IS_POST_MT5365)
    VLD_WRITE32(ucMpvId, RW_VLD_SW_WP, u4VldWptr);
#else // inclue 5387
    UINT32 u4RegVal;
    u4RegVal = (UINT32)((_MPV_VldRead32(ucMpvId, RW_VLD_SW_WP) & 0x7)|(u4VldWptr << 3));
    VLD_WRITE32(ucMpvId, RW_VLD_SW_WP, u4RegVal);
#endif
}


void _MPV_VldSearchStartCode(UCHAR ucMpvId)
{
    UINT32 u4Proc = 0;

    //u4Proc |= VLD_SSCBIT;
    u4Proc |= VLD_SSCBYTE;
    #if defined(VDEC_IS_POST_MT5368)
    VLD_WRITE32(ucMpvId, RW_VLD_MBHP, 0x3FF);
    #else
    u4Proc |= 0xFF;
    #endif

    VLD_WRITE32(ucMpvId, RW_VLD_PROC, u4Proc);  //�N�Ѽƶ���dwProc��,�A��Jregister

}


BOOL _MPV_VldSearchStartCodeFin(UCHAR ucMpvId)
{
    UINT32 u4RegVal;

	#if defined(CC_MT5387) && !defined(CC_MT5363)
    u4RegVal = _MPV_VldRead32(ucMpvId, R_VLD_SSCS);
	#else
    u4RegVal = _MPV_VldRead32(ucMpvId, RW_VLD_SSCS);
	#endif

    return (BOOL) u4RegVal;

}

#if 0
void _MPV_VldSearchStartCodeStop(UCHAR ucMpvId)
{
    _MPV_VldHWReset(ES 0, ucMpvId); // useless now
}
#endif

#if defined (MPV_DUAL_CORE_SUPPORT) 
void _MPV_DualFormatSetting(ucMpvId)
{
    _MPV_DualFormatCommSetting(ucMpvId);
    if(ucMpvId == 1)
    {
        _MPV_DualFormatCore1Setting();
    }
}
void _MPV_DualFormatCommSetting(UCHAR ucMpvId)
{
    UINT32 u4PPReg;
    UINT32 u4Index = 0;
    
    for(u4PPReg = 45; u4PPReg< 53; u4PPReg ++, u4Index ++)
    {
        if(ucMpvId == 0)
        {
            PP0_WRITE32( u4PPReg << 2, MPV_DUAL_FORMAT_COMMON_SETTING[u4Index]);
        }
        else if(ucMpvId == 1)
        {            
            PP1_WRITE32( u4PPReg << 2, MPV_DUAL_FORMAT_COMMON_SETTING[u4Index]);
        }
    }
    for(u4PPReg = 705; u4PPReg < 708; u4PPReg ++, u4Index ++)
    {
        if(ucMpvId == 0)
        {
            PP0_WRITE32(u4PPReg << 2,MPV_DUAL_FORMAT_COMMON_SETTING[u4Index]);            
        }
        else if(ucMpvId == 1)
        {
            PP1_WRITE32(u4PPReg << 2,MPV_DUAL_FORMAT_COMMON_SETTING[u4Index]);            
        }
    }
}

void _MPV_DualFormatCore1Setting(void)
{    
    VLD_TOP_WRITE32(VLD1, RW_VLD_TOP_SRAM_OFFSET, 0X2080800);
    PP1_WRITE32(DBK_WRAP_SRAM_START_OFFSET, 0x800);
}
#endif

void _MPV_McSetOutputBuf(UCHAR ucMpvId, UCHAR ucOutBufIdx)
{
    MC_WRITE32(ucMpvId, RW_MC_OPBUF, ucOutBufIdx);
}


void _MPV_McWriteToDigBuf(UCHAR ucMpvId, BOOL fgSwitch)
{

    if(fgSwitch)
    {
        MC_WRITE32(ucMpvId, RW_MC_WMBDIG, 1);
    }
    else
    {
        MC_WRITE32(ucMpvId, RW_MC_WMBDIG, 0);
    }
}


void _MPV_McSetDecFld(UCHAR ucMpvId, UCHAR ucDecFld, UCHAR uc2ndFldSw)
{

    if(ucDecFld == MC_TOP_FLD)
    {
        MC_WRITE32(ucMpvId, RW_MC_DECFLD, 0);
    }
    else
    {
        MC_WRITE32(ucMpvId, RW_MC_DECFLD, 1);
    }
    if(uc2ndFldSw == MC_2ND_FLD_ON)
    {
        MC_WRITE32(ucMpvId, RW_MC_2FLDMD, 1);
    }
    else
    {
        MC_WRITE32(ucMpvId, RW_MC_2FLDMD, 0);
    }

}


void _MPV_McSetFrmBufPtr(UCHAR ucEsId, UCHAR ucMpvId, UCHAR ucBufIdx, UINT32 u4PtrY,
                                                     UINT32 u4PtrC)
{
    VDEC_ES_INFO_T* prVdecEsInfo;
#ifdef MPV_DUMP_SIM_PAT
    UINT32 u4AlignVSize; // we use 64x32 block
#endif

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    UNUSED(prVdecEsInfo);
    
#ifdef MPV_DUMP_SIM_PAT
    u4AlignVSize = ((prVdecEsInfo->u2VSize + 0x3F) & (~0x3F));
#endif


    switch(ucBufIdx)
    {
        case MC_REF_BUF_1:
            MC_WRITE32(ucMpvId, RW_MC_R1Y, (u4PtrY >> 9)); // div 512
            MC_WRITE32(ucMpvId, RW_MC_R1C, (u4PtrC >> 8)); // div 256
#ifdef MPV_DUMP_SIM_PAT
            if(_bVdecLogSimPat)
            {
                CHAR szBuf[255];
                x_sprintf(szBuf, "Ref0_Y_0x%x.bin_Size_0x%x", u4PtrY, (prVdecEsInfo->u2LineSize *u4AlignVSize));
                Printf("//%s\n", szBuf);
                Printf("// d.save.binary d:/%s 0x%x++0x%x \n", szBuf, u4PtrY, (prVdecEsInfo->u2LineSize *u4AlignVSize));

                x_sprintf(szBuf, "Ref0_C_0x%x.bin_Size_0x%x", u4PtrC, (prVdecEsInfo->u2LineSize *u4AlignVSize)/2);
                Printf("//%s\n", szBuf);
                Printf("// d.save.binary d:/%s 0x%x++0x%x \n", szBuf, u4PtrC, (prVdecEsInfo->u2LineSize *u4AlignVSize)/2);
            }
#else
            LOG(11, "Buf Ref 1, y:0x%x, c:0x%x\n", u4PtrY, u4PtrC);
#endif
            break;

        case MC_REF_BUF_2:
            MC_WRITE32(ucMpvId, RW_MC_R2Y, (u4PtrY >> 9)); // div 512
            MC_WRITE32(ucMpvId, RW_MC_R2C, (u4PtrC >> 8)); // div 256
#ifdef MPV_DUMP_SIM_PAT
            if(_bVdecLogSimPat)
            {
                CHAR szBuf[255];
                x_sprintf(szBuf, "Ref1_Y_0x%x.bin_Size_0x%x", u4PtrY, (prVdecEsInfo->u2LineSize *u4AlignVSize));
                Printf("//%s\n", szBuf);
                Printf("// d.save.binary d:/%s 0x%x++0x%x \n", szBuf, u4PtrY, (prVdecEsInfo->u2LineSize *u4AlignVSize));

                x_sprintf(szBuf, "Ref1_C_0x%x.bin_Size_0x%x", u4PtrC, (prVdecEsInfo->u2LineSize *u4AlignVSize)/2);
                Printf("//%s\n", szBuf);
                Printf("// d.save.binary d:/%s 0x%x++0x%x \n", szBuf, u4PtrC, (prVdecEsInfo->u2LineSize *u4AlignVSize)/2);
            }
#else
            LOG(11, "Buf Ref 2, y:0x%x, c:0x%x\n", u4PtrY, u4PtrC);
#endif
            break;

        case MC_Prg_B_BUF_1:
        	   // current decode frame (no dbk), now dbk don't need working buffer (small size, dram mode)
            MC_WRITE32(ucMpvId, RW_MC_BY1,  (u4PtrY >> 9)); // div 512
            MC_WRITE32(ucMpvId, RW_MC_BC1,  (u4PtrC >> 8)); // div 256

#ifdef MPV_DUMP_SIM_PAT
            if(_bVdecLogSimPat)
            {
                CHAR szBuf[255];
                x_sprintf(szBuf, "Dec_Y_0x%x.bin_Size_0x%x", u4PtrY, (prVdecEsInfo->u2LineSize *u4AlignVSize));
                Printf("//%s\n", szBuf);
                Printf("// d.save.binary d:/%s 0x%x++0x%x \n", szBuf, u4PtrY, (prVdecEsInfo->u2LineSize *u4AlignVSize));

                x_sprintf(szBuf, "Dec_C_0x%x.bin_Size_0x%x", u4PtrC, (prVdecEsInfo->u2LineSize *u4AlignVSize)/2);
                Printf("//%s\n", szBuf);
                Printf("// d.save.binary d:/%s 0x%x++0x%x \n", szBuf, u4PtrC, (prVdecEsInfo->u2LineSize *u4AlignVSize)/2);
            }
#else
            LOG(11, "Buf B 1, y:0x%x, c:0x%x\n", u4PtrY, u4PtrC);
#endif
                        
            #if defined(CC_MT5365) || defined(VDEC_IS_POST_MT5368)
            {
                UINT32 u4DramType = DRVCUST_InitGet(eDramType);
                // 1. If we are doing DBK on I frame,
                // RW_MC_DBK_Y value will be overwrite at MC_DBK_BUF
                // 2. If we are doing I frame (Non deblocking),
                // both RW_MC_DBK_Y and MC_DBK_BUF are same address
                // Both MC out or PP out will be the same address
                // switch (by DDR_III_x2) will decide the output module
                if((u4DramType == DDR_III_x2) | MPV_TEST_DDR3)
                {        
                    //DDR3 32 byte access
                    MC_WRITE32(ucMpvId, RW_MC_DBK_Y,  u4PtrY >> 9);
                    MC_WRITE32(ucMpvId, RW_MC_DBK_C,  u4PtrC >> 8);
                }
            }
            #endif
            
            break;

        case MC_DBK_BUF: // current dbk decode frame (I, P, B)
            MC_WRITE32(ucMpvId, RW_MC_DBK_Y,  u4PtrY >> 9);
            MC_WRITE32(ucMpvId, RW_MC_DBK_C,  u4PtrC >> 8);
#ifdef MPV_DUMP_SIM_PAT
            if(_bVdecLogSimPat)
            {
                CHAR szBuf[255];
                x_sprintf(szBuf, "DecDbk_Y_0x%x.bin_Size_0x%x", u4PtrY, (prVdecEsInfo->u2LineSize *u4AlignVSize));
                Printf("//%s\n", szBuf);
                Printf("// d.save.binary d:/%s 0x%x++0x%x \n", szBuf, u4PtrY, (prVdecEsInfo->u2LineSize *u4AlignVSize));

                x_sprintf(szBuf, "DecDbk_C_0x%x.bin_Size_0x%x", u4PtrC, (prVdecEsInfo->u2LineSize *u4AlignVSize)/2);
                Printf("//%s\n", szBuf);
                Printf("// d.save.binary d:/%s 0x%x++0x%x \n", szBuf, u4PtrC, (prVdecEsInfo->u2LineSize *u4AlignVSize)/2);
            }
#else
            LOG(11, "Buf B Dbk 1, y:0x%x, c:0x%x\n", u4PtrY, u4PtrC);
#endif
            break;

        default:
            break;
    }
}


void _MPV_McSetBlkMode(UCHAR ucMpvId, UCHAR ucMode)
{
    // 5363 change SWAP set addr.
    //#if defined(CC_MT5363) || defined(CC_MT5365) || defined(CC_MT5395)
    #if defined(CC_MT5363) || defined(VDEC_IS_POST_MT5365)
    //#ifdef CC_MT5363
    // ToDo
    //MC_WRITE32(ucMpvId, RW_MC_ADDRSWAP, 0x4);
    //MC_WRITE32(ucMpvId, 0x768, 0x2);
    MC_WRITE32(ucMpvId, RW_MC_ADDRSWAP, ucMode);
    MC_WRITE32(ucMpvId, 0x768, ucMode);
    #else
    MC_WRITE32(ucMpvId, RW_MC_ADDRSWAP, ucMode);
    #endif
}


UINT32 _MPV_SaveReadPtr(UCHAR ucMpvId)
{
    UINT32 u4DramRptr;
    UINT32 u4SramRptr, u4SramWptr;
    UINT32 u4SramDataSz;
    UINT32 u4SramCtr;
    UINT32 u4ReadPtr;
    UINT32 u4FifoStart, u4FifoEnd;
    UINT32 u4Cnt = 0;

#ifdef MPV_DUMP_SIM_PAT
    if(_bVdecLogSimPat)
    {
        //LOG(1, "wait(`VDEC_BITS_PROC_NOP == 1);\n");
        Printf("if(`VDEC_PROCESS_FLAG == 1)  wait(`VDEC_BITS_PROC_NOP == 1);\n");
    }
#endif
    #if defined(VDEC_IS_POST_MT5368)
    if(_MPV_VldRead32(ucMpvId, RO_VLD_SRAMCTRL) & PROCESS_FLAG)
    {
    #endif
        while((_MPV_VldRead32(ucMpvId, RO_VLD_SRAMCTRL) & AA_FIT_TARGET_SCLK)==0)
        {
            u4Cnt++;
            if(u4Cnt >= MPV_WAIT_SRAM_THRD)
            {
                LOG(1, "WaitSramStable Fail\n");            
                #ifdef MPV_DUMP_REG
                if (0)
                {
                   _MPV_DumpReg(ucMpvId);
                }
                #endif
                break;
            }
        }
    #if defined(VDEC_IS_POST_MT5368)
    }
    #endif
		    
    u4DramRptr = _MPV_VldRead32(ucMpvId, RO_VLD_VRPTR);

    UNUSED(u4SramCtr);
    #if defined(VDEC_IS_POST_MT5368)
    //DramReadByteAddr = 
    // DramReadPtr (0xFC) - SramDataSz * 16 + SramCtr * 4 - BsBufLen + (sum/8)
    u4SramCtr = _MPV_VldRead32(ucMpvId, RO_VLD_VBAR);
    u4SramRptr = (UINT32)(u4SramCtr & 0xf);

    u4SramWptr = (UINT32)((u4SramCtr >> 8) & 0xf);
    if(u4SramWptr >= u4SramRptr)
    {
        u4SramDataSz = u4SramWptr - u4SramRptr;
    }
    else
    {
        u4SramDataSz = 16 - (u4SramRptr - u4SramWptr);
    }
    u4SramCtr = ((u4SramCtr >> 24) & 0x3);
    //0812
#ifdef VDEC_IS_POST_MT5398
    if((_MPV_VldRead32(ucMpvId, RWO_VLD_VBAR) & (1 << 28)))
    {
        u4ReadPtr = (u4DramRptr - (u4SramDataSz * 16) + (u4SramCtr * 4) - (5*4));
    }
    else
    {
        u4ReadPtr = (u4DramRptr - (u4SramDataSz * 16) + (u4SramCtr * 4) - (4*4));
    }
#else
    u4ReadPtr = (u4DramRptr - (u4SramDataSz * 16) + (u4SramCtr * 4) - (4*4));
#endif
    u4ReadPtr += ((_MPV_VldRead32(ucMpvId, RW_VLD_ERR_STATUS) & 0x3F)/8);
    // the calculation is correct now.
    //u4ReadPtr = ((u4ReadPtr - 4)& 0xfffffff0);
    u4ReadPtr = ((u4ReadPtr)& 0xfffffff0);
    #else
	#if defined(CC_MT5387) && !defined(CC_MT5363)
    u4SramRptr = _MPV_VldRead32(ucMpvId, RO_VLD_VBAR) & 0x3f;
    u4SramWptr = (UINT32)((_MPV_VldRead32(ucMpvId, RO_VLD_VBAR) >> 16) & 0x3f);
    
    
    if(u4SramWptr >= u4SramRptr)
    {
         u4SramDataSz = u4SramWptr - u4SramRptr;
    }
    else
    {
          u4SramDataSz = 64 - (u4SramRptr - u4SramWptr);
    }    
	#else
    #if defined(CC_MT5363) || defined(CC_MT5365) || defined(CC_MT5395)
    //#ifdef CC_MT5363
    u4SramRptr = _MPV_VldRead32(ucMpvId, RO_VLD_VBAR);
    u4SramRptr = (UINT32)((((u4SramRptr & 0xf) * 4) + ((u4SramRptr >> 24) & 0x3)));
        
    //u4SramWptr = (_MPV_VldRead32(ucMpvId, RO_VLD_VBAR) >> 16) & 0x3f;
    u4SramWptr = (_MPV_VldRead32(ucMpvId, RO_VLD_VBAR));
    u4SramWptr = (UINT32)((((u4SramWptr >> 8) & 0xf) * 4));
    #else
    u4SramRptr = _MPV_VldRead32(ucMpvId, RO_VLD_VBAR) & 0x3f;
    u4SramWptr = (UINT32)((_MPV_VldRead32(ucMpvId, RO_VLD_VBAR) >> 16) & 0x3f);
    #endif    

    //LOG(1, "aa/ab (%d/%d)\n", u4SramWptr, u4SramRptr);
    
    if (u4SramRptr == u4SramWptr)
    {
        u4SramDataSz = 64;
    }
    else if (u4SramWptr >= u4SramRptr)
    {
        u4SramDataSz = u4SramWptr - u4SramRptr;
    }
    else
    {
        u4SramDataSz = 64 - (u4SramRptr - u4SramWptr);
    }
	#endif	
	
    u4ReadPtr= (u4DramRptr - ((u4SramDataSz + 3) * 4)) & 0xfffffff0;
	#endif

    u4FifoStart = (UINT32)(_MPV_VldRead32(ucMpvId, RW_VLD_VSTART) << 6);

    if (u4ReadPtr < u4FifoStart)
    {
        u4FifoEnd = (UINT32)(_MPV_VldRead32(ucMpvId, RW_VLD_VEND) << 6);
        u4ReadPtr = u4ReadPtr + (u4FifoEnd - u4FifoStart);
    }

    return (u4ReadPtr);

}



void _MPV_DecodeTimeoutLog(UCHAR ucMpvId)
{
	static UINT32 u4Timeout = 0;
	
        LOG(1, "(%d) VLD State 0x%x, MC State 0x%x, 0x%x, 0x%x\n", u4Timeout, 
            _MPV_VldRead32(ucMpvId, RO_VLD_FSBI), _MPV_McRead32(ucMpvId, RO_MC_STATUS),
            _MPV_McRead32(ucMpvId, 0x264), _MPV_McRead32(ucMpvId, 0x5c4));
        LOG(1, " MBX: 0x%x, MBY: 0x%x CBP(0x6c):0x%x, 0x114: 0x%x\n",
            _MPV_McRead32(ucMpvId, RO_MC_MBX), _MPV_McRead32(ucMpvId, RO_MC_MBY), 
            _MPV_McRead32(ucMpvId, RO_MC_CBP), _MPV_VldRead32(ucMpvId, RW_VLD_ERR_STATUS));
        LOG(1, "0xbc: 0x%x, 0xb0: 0x%x, 0xf8: 0x%x, 0xfc: 0x%x, \n", _MPV_VldRead32(ucMpvId, 0xbc), 
	        _MPV_VldRead32(ucMpvId, RW_VLD_RPTR), _MPV_VldRead32(ucMpvId, RO_VLD_VWPTR), _MPV_VldRead32(ucMpvId, RO_VLD_VRPTR));        	
       	u4Timeout++;

    #if 0        	
        LOG(1, "IDCT Buffer Status: 0x%08x\n", _MPV_McRead32(ucMpvId, RO_MC_IDCTBUF_STATUS));

        LOG(1, " 0x%08x,  0x%08x, 0x%08x\n",   _MPV_VldRead32(ucMpvId, RO_VLD_FSBI), 
        _MPV_McRead32(ucMpvId, RO_MC_STATUS), _MPV_McRead32(ucMpvId, RO_MC_IDCTBUF_STATUS));
    #endif
}


UINT32 _MPV_VldRead32(UCHAR ucMpvId, UINT16 u2Offset)
{
#if 1
    return VLD_READ32(ucMpvId, u2Offset);
#else
    UINT32 u4Value;

    if (ucMpvId == VLD0)
    {
        u4Value = IO_READ32(VLD0_BASE, u2Offset);
    }
    else
    {
        u4Value = IO_READ32(VLD1_BASE, u2Offset);
    }

    return (u4Value);
#endif
}


UINT32 _MPV_McRead32(UCHAR ucMpvId, UINT16 u2Offset)
{
#if 1
    return MC_READ32(ucMpvId, u2Offset);
#else
    UINT32 u4Value;

    if (ucMpvId == VLD0)
    {
        u4Value = IO_READ32(MC0_BASE, u2Offset);
    }
    else
    {
        u4Value = IO_READ32(MC1_BASE, u2Offset);
    }

    return (u4Value);
#endif
}


void _MPV_SearchStartCodeInt(UCHAR ucMpvId, UINT32 u4Type)
{
    UINT32 u4SearchEnd;

    u4SearchEnd = _MPV_VldRead32(ucMpvId, RO_VLD_VWPTR);
    
    VLD_WRITE32(ucMpvId, RW_VLD_SRHSTC_END, u4SearchEnd);

#ifdef MPV_DUMP_SIM_PAT
    if(_bVdecLogSimPat)
    {
        Printf("@(posedge `VDEC_INTERRUPT); // start_code_int\n");
    }
#endif

    VLD_WRITE32(ucMpvId, RW_VLD_SRHSTC, (UINT32)(SEARCH_MODE|START_SEARCH|u4Type));
}

void _MPV_DisableSearchMode(UCHAR ucMpvId)
{
    VLD_WRITE32(ucMpvId, RW_VLD_SRHSTC, 0);
}


INT32 _MPV_DetectIntra(UCHAR ucMpvId, UCHAR ucEsId, MPV_ES_INFO_T *prEsInfo)
{
    BOOL fgAllIntra;
    UINT32 u4Ret;
    UINT32 u4Idx;
    UINT32 u4SliceIdx;
    VDEC_ES_INFO_T* prVdecEsInfo;
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    ASSERT(prEsInfo!=NULL);

    u4SliceIdx = _MPV_McRead32(ucMpvId, RO_MC_MBY);
    LOG(7, "u4SliceIdx= %d\n", u4SliceIdx);

    u4Ret = _MPV_VldRead32(ucMpvId, RW_VLD_INTRAS1);
    LOG(7, "0x274:0x%x \n", u4Ret);
    prEsInfo->u4IntraSliceReg0 |= u4Ret;

    LOG(7, "u4IntraSliceReg0= 0x%x\n", prEsInfo->u4IntraSliceReg0);

    if (u4SliceIdx >= REG_BITNUM1)
    {          
        u4Ret = _MPV_VldRead32(ucMpvId, RW_VLD_INTRAS2);        
        LOG(7, "0x278:0x%x \n", u4Ret);
        prEsInfo->u4IntraSliceReg1 |= u4Ret;
    }

    if (u4SliceIdx >= REG_BITNUM2)
    {
        u4Ret = (_MPV_VldRead32(ucMpvId, RO_VLD_ERR_MB) & 0xf0000) >> 16;
        LOG(7, " 0x25c:0x%x\n",  u4Ret);
        prEsInfo->u4IntraSliceReg2 |= u4Ret;                
    }

    if ((!prEsInfo->u4IntraSliceReg0) && (!prEsInfo->u4IntraSliceReg1) && (!prEsInfo->u4IntraSliceReg2))
    {
        prEsInfo->fgDetectIntra = FALSE;

        return (VDEC_ERR);
    }

    fgAllIntra = TRUE;

    for (u4Idx = 0; u4Idx <= u4SliceIdx; u4Idx++)
    {
        if (u4Idx < REG_BITNUM1)
        {
            if (!(BOOL)(((prEsInfo->u4IntraSliceReg0 >> u4Idx) & 0x1)))
            {
                fgAllIntra = FALSE;
                break;
            }
        }

        if (u4Idx >= REG_BITNUM1)
        {
            if (!(BOOL)(((prEsInfo->u4IntraSliceReg1 >> (u4Idx - REG_BITNUM1)) & 0x1)))
            {
                fgAllIntra = FALSE;
                break;
            }
        }

        if (u4Idx >= REG_BITNUM2)
        {
            if (!(BOOL)(((prEsInfo->u4IntraSliceReg2 >> (u4Idx - REG_BITNUM2)) & 0x1)))
            {
                fgAllIntra = FALSE;
                break;
            }
        }                                    
    }           

    if (fgAllIntra)
    {
        prVdecEsInfo->fgAllIntraSlice = TRUE;
        LOG(7, "AllIntraSlice\n");

    }
    return (VDEC_SUCCESS);
}


#ifdef MPV_DUMP_REG

void _MPV_DumpReg1(UCHAR ucMpvId)
{
    UINT32 u4RegVal;
    u4RegVal = _MPV_McRead32(ucMpvId, 0x770);
    LOG(6, "MC(0x%3x)= %8x, ", 0x770, u4RegVal);
    u4RegVal = _MPV_McRead32(ucMpvId, 0x774);
    LOG(6, "MC(0x%3x)= %8x, ", 0x774, u4RegVal);
    u4RegVal = _MPV_McRead32(ucMpvId, 0x778);
    LOG(6, "MC(0x%3x)= %8x, ", 0x778, u4RegVal);
    u4RegVal = _MPV_McRead32(ucMpvId, 0x644);
    LOG(6, "MC(0x%3x)= %8x, ", 0x644, u4RegVal);
    u4RegVal = _MPV_McRead32(ucMpvId, 0x828);
    LOG(6, "MC(0x%3x)= %8x\n", 0x828, u4RegVal);
}
    
void _MPV_DumpReg(UCHAR ucMpvId)
{
    static BOOL fgDumpReg = TRUE;
    UINT32  u4Idx;
    UINT32  u4RegVal;

    if(fgDumpReg)
    {
        for (u4Idx = 34; u4Idx <= 39; u4Idx++)
        {
            x_thread_delay(2);
            u4RegVal = _MPV_VldRead32(ucMpvId, 4 * u4Idx);
            LOG(6, "VLD Reg %4d (0x%3x)= %8x \n", u4Idx , 4 * u4Idx, u4RegVal);
        }

        for (u4Idx = 42; u4Idx <= 255; u4Idx++)
        {
            x_thread_delay(2);
            u4RegVal = _MPV_VldRead32(ucMpvId, 4 * u4Idx);
            LOG(6, "VLD Reg %4d (0x%3x)= %8x \n", u4Idx , 4 * u4Idx, u4RegVal);
        }

        for (u4Idx = 0; u4Idx <= 28; u4Idx++)
        {
            x_thread_delay(2);
            u4RegVal = VLD_TOP_READ32(ucMpvId, 4 * u4Idx);
            LOG(6, "TOP Reg %4d (0x%3x)= %8x \n", u4Idx , 4 * u4Idx, u4RegVal);
        }

        for (u4Idx = 0; u4Idx <= 530; u4Idx++)
        {
            x_thread_delay(2);
            u4RegVal = _MPV_McRead32(ucMpvId, 4 * u4Idx);
            LOG(6, " MC Reg %4d (0x%3x)= %8x \n", u4Idx, 4 * u4Idx, u4RegVal);
        }
        for (u4Idx = 608; u4Idx <= 608; u4Idx++)
        {
            x_thread_delay(2);
            u4RegVal = _MPV_McRead32(ucMpvId, 4 * u4Idx);
            LOG(6, " MC Reg %4d (0x%3x)= %8x \n", u4Idx, 4 * u4Idx, u4RegVal);
        }

        for (u4Idx = 0; u4Idx <= 24; u4Idx++)
        {
            x_thread_delay(2);
            u4RegVal = PP_READ32(ucMpvId, 4 * u4Idx);
            LOG(6, " PP Reg %4d (0x%3x)= %8x \n", u4Idx , 4 * u4Idx, u4RegVal);
        }

        LOG(6, "\n\n\n");
    }
}

extern CHAR _szGoldFilePath[256];
extern UINT32 _u4PicCount[MPV_MAX_ES];
VOID _MPV_DumpMCData(UCHAR ucEsId, UCHAR ucMpvId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    FBM_PIC_HDR_T *prFbmPicHdr;
    CHAR ucPicType;
    UCHAR ucFbId;
    
    if(ucEsId >= MPV_MAX_ES)
    {
        LOG(0, "ucEsId >= MPV_MAX_ES\n");
        ASSERT(ucEsId < MPV_MAX_ES);
        ucEsId = (MPV_MAX_ES-1);
    }
    
    if(ucMpvId >= MPV_MAX_VLD)
    {
        LOG(0, "ucMpvId >= MPV_MAX_VLD\n");
        ASSERT(ucMpvId < MPV_MAX_VLD);
        ucMpvId = (MPV_MAX_VLD-1);
    }
    
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    
#ifdef CC_MPV_DEBLOCKING
    if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLAG_DBK) && (prVdecEsInfo->ucPicType == B_TYPE))
    {
        ucFbId = prVdecEsInfo->ucDbkFbId;             
    }
    else
#endif
    {
        ucFbId = prVdecEsInfo->ucFbId; 
    }

    prFbmPicHdr = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, ucFbId);
    
    if((!prFbmPicHdr) || (!prVdecEsInfo))
    {
        LOG(0, "prFbmPicHdr or prVdecEsInfo null\n");
        return;
    }

    if (prVdecEsInfo->ucPicType == I_TYPE)
    {
        ucPicType = 'I';
    }
    else if (prVdecEsInfo->ucPicType == P_TYPE)
    {
        ucPicType = 'P';
    }
    else if (prVdecEsInfo->ucPicType == B_TYPE)
    {
        ucPicType = 'B';
    }
    else
    {
        ucPicType = 'U';
    }

    UNUSED(_fgDumpMCData);
    if(_fgDumpMCData)
    {
#ifdef MPV_SEMI_HOST     
        LOG(0, "VDEC_perf_measure:seq_name=%s,pic_idx=%d,pic_width=%d,pic_height=%d\n",
            _szGoldFilePath, _u4PicCount[ucEsId], prVdecEsInfo->u2HSize, prVdecEsInfo->u2VSize);
#endif
        LOG(0, "VDEC_perf_measure2:pic_type=%c,framepic=%d,CYCLE_DRAM=%d\n",
            ucPicType, ((IS_FRMPIC(prFbmPicHdr)) ? 1:0), MC_READ32(ucMpvId, RO_MC_DECODING_CYCLE));
        LOG(0, "VDEC_perf_measure3:MC_DLE_NUM=%d,MC_MBX =%d,MC_MBY =%d\n",
            MC_READ32(ucMpvId, RO_MC_DLE_NUM), MC_READ32(ucMpvId, RO_MC_MBX), MC_READ32(ucMpvId, RO_MC_MBY));
        LOG(0, "VDEC_perf_measure4:CYC_SYS=%d,INTRA_CNT=%d,Y_BLK_CNT=%d\n",
            MC_READ32(ucMpvId, RO_MC_CYC_SYS), MC_READ32(ucMpvId, RO_MC_INTRA_CNT), MC_READ32(ucMpvId, RO_MC_Y_BLK_CNT));
        LOG(0, "VDEC_perf_measure5:C_BLK_CNT=%d,WAIT_CNT=%d,REQ_CNT=%d\n",
            MC_READ32(ucMpvId, RO_MC_C_BLK_CNT), MC_READ32(ucMpvId, RO_MC_WAIT_CNT));
        LOG(0, "VDEC_perf_measure6:PARA4_BEHAVIOR=%d,PARA8_BEHAVIOR=%d\n",
            ((MC_READ32(ucMpvId, RW_MC_PARA_BEHAVIOR)&PARA4_BEHAVIOR)?1:0), ((MC_READ32(ucMpvId, RW_MC_PARA_BEHAVIOR)&PARA8_BEHAVIOR)?1:0));            
    }
}
#endif


void _MPV_VldMvSum(UCHAR ucMpvId, UCHAR ucEsId)
{
#if !defined(VDEC_IS_POST_MT5398)
    UINT32 u4Mv1X;
    UINT32 u4Mv2X;
    UINT32 u4Mv3X;
    UINT32 u4Mv4X;
    UINT32 u4Mv1Y;
    UINT32 u4Mv2Y;
    UINT32 u4Mv3Y;
    UINT32 u4Mv4Y;
    UINT32 u4Mv1Total;
    UINT32 u4Mv2Total;        
    UINT32 u4Mv3Total;
    UINT32 u4Mv4Total;
    UINT32 u4Avg1X = 0;
    UINT32 u4Avg2X = 0;
    UINT32 u4Avg3X = 0;
    UINT32 u4Avg4X = 0;        
    UINT32 u4Avg1Y = 0;
    UINT32 u4Avg2Y = 0;
    UINT32 u4Avg3Y = 0;
    UINT32 u4Avg4Y = 0;  
#endif
    UINT32 u4Data;
    UINT32 u4Avg;    
    UINT32 u4MbCnt;
    FBM_SEQ_HDR_T *prFbmSeqHdr;
    MPV_ES_INFO_T *prEsInfo;
    VDEC_ES_INFO_T *prVdecEsInfo;

    if(ucEsId >= MPV_MAX_ES)
    {
        LOG(0, "ucEsId >= MPV_MAX_ES\n");
        ASSERT(ucEsId < MPV_MAX_ES);
        ucEsId = (MPV_MAX_ES-1);
    }
    
    if(ucMpvId >= MPV_MAX_VLD)
    {
        LOG(0, "ucMpvId >= MPV_MAX_VLD\n");
        ASSERT(ucMpvId < MPV_MAX_VLD);
        ucMpvId = (MPV_MAX_VLD-1);
    }
    
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    
    prEsInfo = (MPV_ES_INFO_T*)_MpvGetHdlr(ucEsId);    

    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecEsInfo->ucFbgId);   
    if(prFbmSeqHdr == NULL)
    {
        LOG(3, "_MPV_VldMvSum prFbmSeqHdr null\n");
        u4MbCnt = (UINT32)(120*68);
    }
    else
    {
        u4MbCnt = (UINT32)((prVdecEsInfo->u2HSize *prVdecEsInfo->u2VSize) >> 8);
    }
    
#if !defined(VDEC_IS_POST_MT5398)
    u4Mv1X = _MPV_VldRead32(ucMpvId, RO_VLD_MV1X);
    u4Mv2X = _MPV_VldRead32(ucMpvId, RO_VLD_MV2X);
    u4Mv3X = _MPV_VldRead32(ucMpvId, RO_VLD_MV3X);
    u4Mv4X = _MPV_VldRead32(ucMpvId, RO_VLD_MV4X);
    u4Mv1Y = _MPV_VldRead32(ucMpvId, RO_VLD_MV1Y);
    u4Mv2Y = _MPV_VldRead32(ucMpvId, RO_VLD_MV2Y);
    u4Mv3Y = _MPV_VldRead32(ucMpvId, RO_VLD_MV3Y);
    u4Mv4Y = _MPV_VldRead32(ucMpvId, RO_VLD_MV4Y);        

    u4Mv1Total = (UINT32)(_MPV_VldRead32(ucMpvId, RO_VLD_MV12) & 0xFFF);
    u4Mv2Total = (UINT32)((_MPV_VldRead32(ucMpvId, RO_VLD_MV12) & 0xFFF0000) >> 16);
    u4Mv3Total = (UINT32)(_MPV_VldRead32(ucMpvId, RO_VLD_MV34) & 0xFFF);
    u4Mv4Total = (UINT32)((_MPV_VldRead32(ucMpvId, RO_VLD_MV34) & 0xFFF0000) >> 16);

    if (u4Mv1Total)
    {
        u4Avg1X = u4Mv1X/u4Mv1Total;
        u4Avg1Y = u4Mv1Y/u4Mv1Total;            
    }

    if (u4Mv2Total)
    {        
        u4Avg2X = u4Mv2X/u4Mv2Total;
        u4Avg2Y = u4Mv2Y/u4Mv2Total;            
    }

    if (u4Mv3Total)
    {                
        u4Avg3X = u4Mv3X/u4Mv3Total;
        u4Avg3Y = u4Mv3Y/u4Mv3Total;            
    }

    if (u4Mv4Total)
    {
        u4Avg4X = u4Mv4X/u4Mv4Total;                   
        u4Avg4Y = u4Mv4Y/u4Mv4Total;        
    }
#endif

    u4Data = _MPV_VldRead32(ucMpvId, RO_VLD_QP_SUM);
  
    u4Avg = u4Data/u4MbCnt;
  
    prEsInfo->u4QpAvg = u4Avg;
    
#if !defined(VDEC_IS_POST_MT5398)
    prEsInfo->u4Mv = u4Avg1X + u4Avg2X + u4Avg3X + u4Avg4X + u4Avg1Y + u4Avg2Y + u4Avg3Y + u4Avg4Y;
    prEsInfo->u4MvTotal = u4Mv1X + u4Mv2X + u4Mv3X + u4Mv4X + u4Mv1Y + u4Mv2Y + u4Mv3Y + u4Mv4Y;
   
    LOG(7, "MvX(%d/%d/%d/%d) MvY(%d/%d/%d/%d) QP(%d)",            
           u4Avg1X, u4Avg2X, u4Avg3X, u4Avg4X, u4Avg1Y, u4Avg2Y, u4Avg3Y, u4Avg4Y, u4Avg);      
#else
    LOG(7, "Es(%d) QpAvg(%u) QpSum(%u) MbCnt(%u)\n", ucEsId, prEsInfo->u4QpAvg, u4Data, u4MbCnt);
    prEsInfo->u4IntraBlockNum = MC_READ32(ucMpvId, RO_MC_INTRA_CNT);
#endif

    return ;
}

#ifdef MPV_TESTCODE
void _MPV_VldQpSum(UCHAR ucMpvId)
{
    UINT32 u4Data;

    u4Data = _MPV_VldRead32(ucMpvId, RO_VLD_QP_SUM);

    LOG(7, "QP sum: %d\n", u4Data);
}
#endif

#if 0
BOOL _MPV_ResetSearch(UCHAR ucMpvId, UINT32 u4FifoStart, UINT32 u4FifoEnd, 
                            UINT32 u4SearchStart, UINT32 u4SearchEnd)
{
    UINT32 u4ShiftBits;
    UINT32 u4RegVal;
        
    if(ucMpvId >= MPV_MAX_VLD)
    {
        LOG(1, "ucMpvId >= MPV_MAX_VLD\n");
        ASSERT(ucMpvId < MPV_MAX_VLD);
        ucMpvId = (MPV_MAX_VLD-1);
    }
    
    _MPV_VldHWReset(ES 0, ucMpvId); // useless now
    _MPV_VldSetVFifo(ucMpvId, u4FifoStart, u4FifoEnd);   
    
    u4SearchStart &= ~0xF;

    // Write DRAM Start Address
    VLD_WRITE32(ucMpvId, RW_VLD_RPTR, u4SearchStart);

    // start to fetch data
    VLD_WRITE32(ucMpvId, RW_VLD_PROC, (UINT32)VLD_INIFET);

    if (_hVldTimer[ucMpvId] == (HANDLE_T)NULL)
    {
        VERIFY(x_timer_create(&_hVldTimer[ucMpvId]) == OSR_OK);
    }

    if(!_WaitVldFetchOk(ucMpvId))
    {
        return(FALSE);
    }
    
    VLD_WRITE32(ucMpvId, RW_VLD_PROC, (UINT32)VLD_INIBR);

    u4SearchStart &= 0xF;

    while (u4SearchStart > 0)
    {
        u4ShiftBits = (UINT32)((u4SearchStart > 4) ? 32 : (u4SearchStart << 3));
        u4RegVal = _MPV_VldGetBitS(ucMpvId, (UCHAR)u4ShiftBits);
        UNUSED(u4RegVal);
        u4SearchStart -= (UINT32)((u4ShiftBits >> 3));
    }

    VLD_WRITE32(ucMpvId, RW_VLD_SRHSTC_END, u4SearchEnd);
 
    return(TRUE);   
}
#endif


#ifdef CC_MPV_DEBLOCKING
void _MPV_SetMcDbkBufAddr(UCHAR ucEsId, UCHAR ucMpvId, UCHAR ucFbgId, UCHAR ucDbkFbId, 
    const FBM_PIC_HDR_T *prFbmPicHdr, MPV_ES_INFO_T *prEsInfo)
{
    UCHAR ucFRefFbId;
    UCHAR ucBRefFbId;
    UCHAR ucDecFbId;
    UINT32 u4AddrY;
    UINT32 u4AddrC;
    VDEC_ES_INFO_T *prVdecEsInfo;
    #ifdef MPV_SEMI_HOST
    BOOL fgFBlack = FALSE;
    BOOL fgBBlack = FALSE;
    #endif
   
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
       
    if(prFbmPicHdr == NULL)
    {
    	LOG(0, "prFbmPicHdr null\n");
        ASSERT(prFbmPicHdr);
        return;
    }
    
    if(prEsInfo == NULL)
    {
        LOG(0, "prEsInfo null\n");
        ASSERT(prEsInfo);
        return;
    }
    ASSERT(ucDbkFbId != FBM_FB_ID_UNKNOWN);

    // move down
    /*
    if ((prVdecEsInfo->ucPicType != B_TYPE) || (!prEsInfo->fgBNoDbk))
    {
        FBM_GetFrameBufferAddr(ucFbgId, ucDbkFbId, &u4AddrY, &u4AddrC);
        ASSERT((u4AddrY!=0)&&(u4AddrC !=0));                    
        _MPV_McSetFrmBufPtr(ucMpvId, MC_DBK_BUF, u4AddrY, u4AddrC);

        LOG(8, "MC_DBK_BUF  Y: 0x%x, C: 0x%x\n", u4AddrY, u4AddrC);
    }
    */

    FBM_GetRefFrameBuffer(ucFbgId, &ucFRefFbId, &ucBRefFbId);

    ucDecFbId = prVdecEsInfo->ucFbId;

    ASSERT(ucDecFbId != FBM_FB_ID_UNKNOWN);
                
    switch (prFbmPicHdr->ucPicCdTp)
    {    
        case I_TYPE:
            
            if (ucFRefFbId == FBM_FB_ID_UNKNOWN)
            {
                ucFRefFbId = FBM_GetReadyFrameBuffer(ucFbgId);
                
                if (ucFRefFbId ==  FBM_FB_ID_UNKNOWN)
                {
                    ucFRefFbId = 0;
                }
                #ifdef MPV_SEMI_HOST
                fgFBlack = TRUE;
                #endif
            }

            if (ucBRefFbId == FBM_FB_ID_UNKNOWN)
            {
                ucBRefFbId = ucFRefFbId;
                #ifdef MPV_SEMI_HOST
                fgBBlack = TRUE;
                #endif
            }

            FBM_GetFrameBufferAddr(ucFbgId, ucFRefFbId, &u4AddrY, &u4AddrC);           
            _MPV_McSetFrmBufPtr(ucEsId, ucMpvId, MC_REF_BUF_1, u4AddrY, u4AddrC);

            FBM_GetFrameBufferAddr(ucFbgId, ucBRefFbId, &u4AddrY, &u4AddrC);
            _MPV_McSetFrmBufPtr(ucEsId, ucMpvId, MC_REF_BUF_2, u4AddrY, u4AddrC);
            LOG(8, "MC_REF_BUF_2  Y: 0x%x, C: 0x%x\n", u4AddrY, u4AddrC);

            FBM_GetFrameBufferAddr(ucFbgId, ucDecFbId, &u4AddrY, &u4AddrC);
            ASSERT((u4AddrY!=0)&&(u4AddrC !=0));                        
            _MPV_McSetFrmBufPtr(ucEsId, ucMpvId, MC_Prg_B_BUF_1, u4AddrY, u4AddrC);      
            LOG(8, "MC_Prg_B_BUF_1  Y: 0x%x, C: 0x%x\n", u4AddrY, u4AddrC);
            break;
            
        case P_TYPE:
            
            if (ucFRefFbId == FBM_FB_ID_UNKNOWN)
            {
                ucFRefFbId = FBM_GetReadyFrameBuffer(ucFbgId);
                
                if (ucFRefFbId ==  FBM_FB_ID_UNKNOWN)
                {
                    ucFRefFbId = 0;
                }
                #ifdef MPV_SEMI_HOST
                fgFBlack = TRUE;
                #endif
            }

            if (ucBRefFbId == FBM_FB_ID_UNKNOWN)
            {
                ucBRefFbId = ucFRefFbId;
                #ifdef MPV_SEMI_HOST
                fgBBlack = TRUE;
                #endif
            }

            FBM_GetFrameBufferAddr(ucFbgId, ucFRefFbId, &u4AddrY, &u4AddrC);           
            _MPV_McSetFrmBufPtr(ucEsId, ucMpvId, MC_REF_BUF_1, u4AddrY, u4AddrC);
            LOG(8, "MC_REF_BUF_1  Y: 0x%x, C: 0x%x\n", u4AddrY, u4AddrC);

            FBM_GetFrameBufferAddr(ucFbgId, ucBRefFbId, &u4AddrY, &u4AddrC);
            _MPV_McSetFrmBufPtr(ucEsId, ucMpvId, MC_REF_BUF_2, u4AddrY, u4AddrC);
            LOG(8, "MC_REF_BUF_2  Y: 0x%x, C: 0x%x\n", u4AddrY, u4AddrC);

            FBM_GetFrameBufferAddr(ucFbgId, ucDecFbId, &u4AddrY, &u4AddrC);        
            ASSERT((u4AddrY!=0)&&(u4AddrC !=0));                        
            _MPV_McSetFrmBufPtr(ucEsId, ucMpvId, MC_Prg_B_BUF_1, u4AddrY, u4AddrC);            
            LOG(8, "MC_Prg_B_BUF_1  Y: 0x%x, C: 0x%x\n", u4AddrY, u4AddrC);      
            break;
            
        case B_TYPE:

            if (ucFRefFbId == FBM_FB_ID_UNKNOWN)
            {
                ucFRefFbId = FBM_GetReadyFrameBuffer(ucFbgId);
                
                if (ucFRefFbId ==  FBM_FB_ID_UNKNOWN)
                {            
                    ucFRefFbId = 0;  
                }
                #ifdef MPV_SEMI_HOST
                fgFBlack = TRUE;
                #endif
            }

            if (ucBRefFbId == FBM_FB_ID_UNKNOWN)
            {
                ucBRefFbId = ucFRefFbId;
                #ifdef MPV_SEMI_HOST
                fgBBlack = TRUE;
                #endif
            }
            
            FBM_GetFrameBufferAddr(ucFbgId, ucFRefFbId, &u4AddrY, &u4AddrC);           
            _MPV_McSetFrmBufPtr(ucEsId, ucMpvId, MC_REF_BUF_1, u4AddrY, u4AddrC);
            LOG(8, "MC_REF_BUF_1  Y: 0x%x, C: 0x%x\n", u4AddrY, u4AddrC);

            FBM_GetFrameBufferAddr(ucFbgId, ucBRefFbId, &u4AddrY, &u4AddrC);
            _MPV_McSetFrmBufPtr(ucEsId, ucMpvId, MC_REF_BUF_2, u4AddrY, u4AddrC);
            LOG(8, "MC_REF_BUF_2  Y: 0x%x, C: 0x%x\n", u4AddrY, u4AddrC);
            
            if (!prEsInfo->fgBNoDbk)
            {
                FBM_GetFrameBufferAddr(ucFbgId, prVdecEsInfo->ucFbId, &u4AddrY, &u4AddrC);      
            }
            else
            {
                FBM_GetFrameBufferAddr(ucFbgId, ucDbkFbId, &u4AddrY, &u4AddrC);                  
            }
            ASSERT((u4AddrY!=0)&&(u4AddrC !=0));            
            _MPV_McSetFrmBufPtr(ucEsId, ucMpvId, MC_Prg_B_BUF_1, u4AddrY, u4AddrC);            
            LOG(8, "MC_Prg_B_BUF_1  Y: 0x%x, C: 0x%x\n", u4AddrY, u4AddrC);                
            
            break;
            
        default:            
            {
                LOG(1, "%s, %d %d\n", __FUNCTION__, __LINE__, prFbmPicHdr->ucPicCdTp);
            }
            ASSERT(0);
        break;
                    
    }              

    #ifdef MPV_SEMI_HOST
    {
        UINT32 u4ByteSize;
        VDEC_INFO_T *prVdecInfo;
        prVdecInfo = _VDEC_GetInfo();
        // we use 64x32 block
        u4ByteSize = (prVdecEsInfo->u2LineSize) * (((prVdecEsInfo->u2VSize)+0x3F)&(~0x3F));
        if(prVdecInfo->fgIgnoreErr)
        {
            UNUSED(fgFBlack);
            UNUSED(fgBBlack);
            if(fgFBlack)
            {
                x_memset((VOID*)_pYaddr[ucEsId], 0, u4ByteSize);
                x_memset((VOID*)_pCaddr[ucEsId], 0, u4ByteSize/2);
 #if defined(VDEC_IS_POST_MT5365)
                 HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)_pYaddr[ucEsId]), u4ByteSize);
                 HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)_pCaddr[ucEsId]), u4ByteSize/2);
 #else
                HalFlushInvalidateDCache();
 #endif                     
                _MPV_McSetFrmBufPtr(ucEsId, ucMpvId, MC_REF_BUF_1, (UINT32)_pYaddr[ucEsId], (UINT32)_pCaddr[ucEsId]);
            }
            if(fgBBlack)
            {
                x_memset((VOID*)_pYaddr[ucEsId], 0, u4ByteSize);
                x_memset((VOID*)_pCaddr[ucEsId], 0, u4ByteSize/2);
 #if defined(VDEC_IS_POST_MT5365)
                HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)_pYaddr[ucEsId]), u4ByteSize);
                HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)_pCaddr[ucEsId]), u4ByteSize/2);
 #else
                HalFlushInvalidateDCache();
 #endif      
                _MPV_McSetFrmBufPtr(ucEsId, ucMpvId, MC_REF_BUF_2, (UINT32)_pYaddr[ucEsId], (UINT32)_pCaddr[ucEsId]);
            }
            // frame data
            {
                x_memset((VOID*)u4AddrY, 0, u4ByteSize);
                x_memset((VOID*)u4AddrC, 0, u4ByteSize/2);
 #if defined(VDEC_IS_POST_MT5365)
                 HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)_pYaddr[ucEsId]), u4ByteSize);
                 HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)_pCaddr[ucEsId]), u4ByteSize/2);
 #else
                HalFlushInvalidateDCache();
 #endif      
            }
        }
    }
    #endif
    
    // move from top
    if ((prVdecEsInfo->ucPicType != B_TYPE) || (!prEsInfo->fgBNoDbk))
    {
        FBM_GetFrameBufferAddr(ucFbgId, ucDbkFbId, &u4AddrY, &u4AddrC);
        ASSERT((u4AddrY!=0)&&(u4AddrC !=0));                    
        _MPV_McSetFrmBufPtr(ucEsId, ucMpvId, MC_DBK_BUF, u4AddrY, u4AddrC);

        LOG(8, "MC_DBK_BUF  Y: 0x%x, C: 0x%x\n", u4AddrY, u4AddrC);
    }
    // ~move

    #ifdef MPV_SEMI_HOST
    {
        UINT32 u4ByteSize;
        VDEC_INFO_T *prVdecInfo;
        prVdecInfo = _VDEC_GetInfo();
        // we use 64x32 block
        u4ByteSize = (prVdecEsInfo->u2LineSize) * (((prVdecEsInfo->u2VSize)+0x3F)&(~0x3F));
        if(prVdecInfo->fgIgnoreErr)
        {
            // dbk frame data
            {
                x_memset((VOID*)u4AddrY, 0, u4ByteSize);
                x_memset((VOID*)u4AddrC, 0, u4ByteSize/2);
 #if defined(VDEC_IS_POST_MT5365)
                HalFlushInvalidateDCacheMultipleLine(VIRTUAL(u4AddrY), u4ByteSize);
                HalFlushInvalidateDCacheMultipleLine(VIRTUAL(u4AddrC), u4ByteSize/2);
 #else
                HalFlushInvalidateDCache();
 #endif     
            }
        }
    }
    #endif

    #ifdef MPV_MID
    {
        FBM_SEQ_HDR_T *prFbmSeqHdr;
        UINT32 u4ByteSize;
        
        prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
        u4ByteSize = (prVdecEsInfo->u2LineSize) * (((prVdecEsInfo->u2VSize)+0x1f)&(~0x1f));
        MID_SetRegion(0, MID_AGENT_MPEG1, u4AddrY, u4AddrY + u4ByteSize);
        MID_SetRegion(1, MID_AGENT_MPEG1, u4AddrC, u4AddrC + u4ByteSize);    
        MID_EnableRegionProtect(0);    
        MID_EnableRegionProtect(1);    
    }
    #endif
}

void _MPV_HandleDbk(UCHAR ucMpvId, UCHAR ucFbgId, UCHAR ucDecFbId, 
    UCHAR ucEsId, FBM_PIC_HDR_T *prFbmPicHdr, UINT32 u4ErrCnt)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    MPV_ES_INFO_T *prEsInfo;
    MPV_INFO_T* prMpvInfo;
    UINT32 u4MbCnt = 0;
    UINT32 u4QpAvgGroup = 0;

    if(prFbmPicHdr == NULL)
    {
        LOG(1, "prFbmPicHdr null\n");
        return;
    }
    
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prEsInfo = (MPV_ES_INFO_T*)_MpvGetHdlr(ucEsId);
    prMpvInfo = (MPV_INFO_T*)_MPV_GetMpvInfo();
    u4MbCnt = (prVdecEsInfo->u2HSize *prVdecEsInfo->u2VSize) >> 8;
    UNUSED(prFbmPicHdr);
#ifdef CC_MPV_DEBLOCKING
    if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLAG_DBK))
    {
        UINT32 u4DbkOfstCnt;
        UINT32 u4Dbk4pCnt;
        
        _MPV_VldMvSum(ucMpvId, ucEsId);        
        _MPV_McGetDbkCnt(ucMpvId, ucEsId, &u4DbkOfstCnt, &u4Dbk4pCnt);

        FBM_SetFrameBufferFtrCnt(ucFbgId, ucDecFbId, u4DbkOfstCnt, u4Dbk4pCnt);
        LOG(7, "u4DbkOfstCnt: %d, u4Dbk4pCnt: %d\n", u4DbkOfstCnt, u4Dbk4pCnt);
        //LOG(7, "Y_QP: 0x%x, C_QP: 0x%x\n", _MPV_McRead32(ucMpvId, RW_MC_DBK_QP_Y), _MPV_McRead32(ucMpvId, RW_MC_DBK_QP_C));

        if(prMpvInfo->fgDisableDbk)
        {
            prEsInfo->fgBNoDbk = TRUE;
            prVdecEsInfo->fgRefNoDbk = TRUE;
        }
        else if (!prEsInfo->fgDisableDbkThrsd)
        {
#if !defined(VDEC_IS_POST_MT5398)
            if ((prFbmPicHdr->ucPicCdTp == I_TYPE) || (prFbmPicHdr->ucPicCdTp == P_TYPE))
            {
                if ((((prVdecEsInfo->ucPicType == I_TYPE) && (prEsInfo->u4QpAvg < prEsInfo->u4QpAvgThrsd) && (prEsInfo->u4DbkRatio < prEsInfo->u4DbkRatioThrsd))    
                 || ((prVdecEsInfo->ucPicType == P_TYPE) && (prEsInfo->u4QpAvg < prEsInfo->u4QpAvgThrsdP) && (prEsInfo->u4DbkRatio < prEsInfo->u4DbkRatioThrsdP)
                  && (prEsInfo->u4Mv < prEsInfo->u4MvThrsd)))
                  || ((prEsInfo->u4MvTotal == 0) && (prVdecEsInfo->ucPicType == P_TYPE))
                  || ((prVdecEsInfo->u2HSize < DBK_MIN_H)
                      &&
                      (prVdecEsInfo->u2VSize < DBK_MIN_H)))
                {
                    prEsInfo->fgBNoDbk = TRUE;
                    prVdecEsInfo->fgRefNoDbk = TRUE;
                    // ilii change to 5 after dbk threshold ready
                    LOG(6, "(dbk %d, %d, %d, %d, %d)\n", prVdecEsInfo->ucPicType , prEsInfo->u4QpAvg, prEsInfo->u4DbkRatio, prEsInfo->u4Mv, prEsInfo->u4MvTotal);
                }
                else
                {
                    prEsInfo->fgBNoDbk = FALSE;
                    prVdecEsInfo->fgRefNoDbk = FALSE;                
                }
                
                #if defined(CC_MT5365) || defined(CC_MT5395)
                if((u4ErrCnt > 0) && IS_MPEG1(prEsInfo))
                {
                    // MC result will do error concealment (backword concealment).
                    // However, PP does not do it.
                    // This is temp solution, PP will also do it in backword concealment
                    // after 5396.
                    prEsInfo->fgBNoDbk = TRUE;
                    prVdecEsInfo->fgRefNoDbk = TRUE;
                }
                #endif
            }
#else
            
            if(prEsInfo->u4QpAvg <= prEsInfo->u4QP0)
              u4QpAvgGroup = 0;
            else if(prEsInfo->u4QpAvg <= prEsInfo->u4QP1)
              u4QpAvgGroup = 1;
            else if(prEsInfo->u4QpAvg <= prEsInfo->u4QP2)
              u4QpAvgGroup = 2;
            else 
              u4QpAvgGroup = 3;

            if (prFbmPicHdr->ucPicCdTp == I_TYPE)
            {
                if((u4QpAvgGroup == 0) || ( (prVdecEsInfo->u2HSize < DBK_MIN_H)&& (prVdecEsInfo->u2VSize < DBK_MIN_H)))
                {
                   //use non-DBK frame and disable DBK for next B
                    prEsInfo->fgBNoDbk = TRUE;
                    prVdecEsInfo->fgRefNoDbk = TRUE;
                }
                else
                {
                    //use DBK frame and turn on DBK for next B
                prEsInfo->fgBNoDbk = FALSE;
                prVdecEsInfo->fgRefNoDbk = FALSE;  

                }
                //update ref info
                prEsInfo->u4QpAvgPre = prEsInfo->u4QpAvg;
                prEsInfo->u4QpAvgGroupPre = u4QpAvgGroup;
            }
            else if (prFbmPicHdr->ucPicCdTp == P_TYPE)
            {
                if((u4QpAvgGroup == 0) ||
                   ((prVdecEsInfo->u2HSize < DBK_MIN_H)&& (prVdecEsInfo->u2VSize < DBK_MIN_H))||
                   ((u4QpAvgGroup > prEsInfo->u4QpAvgGroupPre) &&(prEsInfo->u4IntraBlockNum < (u4MbCnt/4))))
                {
                    //use non-DBK frame and disable DBK for next B
                    prEsInfo->fgBNoDbk = TRUE;
                    prVdecEsInfo->fgRefNoDbk = TRUE;
                }
                else
                {
                    //use DBK frame and turn on DBK for next B
                    prEsInfo->fgBNoDbk = FALSE;
                    prVdecEsInfo->fgRefNoDbk = FALSE;  
                    //update ref info
                    prEsInfo->u4QpAvgPre = prEsInfo->u4QpAvg;
                    prEsInfo->u4QpAvgGroupPre = u4QpAvgGroup;
                }
            }
#endif

#if 0
            if(prFbmPicHdr->ucPicCdTp == I_TYPE)
            {
                LOG(1, "Frm(%4d), Type( I ), fgBNoDbk(%4d), DbkRat%4d, RatPerI(%4d), u4QpAvg(%4d), AvgPerI(%4d), Num(%4d), Group(%4d), GroupPre(%4d)\n", u4FrameCount++, prVdecEsInfo->fgRefNoDbk,
                                prEsInfo->u4DbkRatio, prEsInfo->u4DbkRatioPerI, 
                                prEsInfo->u4QpAvg, prEsInfo->u4QpAvgPerI, prEsInfo->u4IntraBlockNum,
                                u4QpAvgGroup, prEsInfo->u4QpAvgGroupPre);
                }
            else if(prFbmPicHdr->ucPicCdTp == P_TYPE)
            {
                LOG(1, "Frm(%4d), Type( P ), fgBNoDbk(%4d), DbkRat%4d, RatPerI(%4d), u4QpAvg(%4d), AvgPerI(%4d), Num(%4d), Group(%4d), GroupPre(%4d)\n", u4FrameCount++, prVdecEsInfo->fgRefNoDbk,
                                prEsInfo->u4DbkRatio, prEsInfo->u4DbkRatioPerI, 
                                prEsInfo->u4QpAvg, prEsInfo->u4QpAvgPerI, prEsInfo->u4IntraBlockNum,
                                u4QpAvgGroup, prEsInfo->u4QpAvgGroupPre);
            }
            else if(prFbmPicHdr->ucPicCdTp == B_TYPE)
            {
                LOG(1, "Frm(%4d), Type( B ), fgBNoDbk(%4d), DbkRat%4d, RatPerI(%4d), u4QpAvg(%4d), AvgPerI(%4d), Num(%4d), Group(%4d), GroupPre(%4d)\n", u4FrameCount++, prEsInfo->fgBNoDbk,
                                prEsInfo->u4DbkRatio, prEsInfo->u4DbkRatioPerI, 
                                prEsInfo->u4QpAvg, prEsInfo->u4QpAvgPerI, prEsInfo->u4IntraBlockNum,
                                u4QpAvgGroup, prEsInfo->u4QpAvgGroupPre);
            }
#endif

        }
        else
        {
            prEsInfo->fgBNoDbk = FALSE;
            prVdecEsInfo->fgRefNoDbk = FALSE;        
        }
    }
#endif
    /*
    if (prVdecEsInfo->fgSeqHdr)
    {
        _MPV_VldMvSum(ucMpvId, ucEsId);
        if (prEsInfo->u4QpInterval >= MPV_QP_INTERVAL)
        {
            prFbmPicHdr->u4QpAvg = prEsInfo->u4QpAvg;//_DetectSliceQp(ucMpvId, ucEsId);
            prEsInfo->u4LastQp = prFbmPicHdr->u4QpAvg;        
            prEsInfo->u4QpInterval = 0;
            LOG(7, "QP=%d\n", prFbmPicHdr->u4QpAvg);                
        }
    }
    else
    {
        prFbmPicHdr->u4QpAvg = prEsInfo->u4LastQp;
    }

    prEsInfo->u4QpInterval++;  
    */
    return;
}
#endif
#if defined(MPV_CRCTEST) || defined(MULTIMEDIA_AUTOTEST_SUPPORT) || defined(VDEC_CRC_TEST)

VOID _MPV_Read_Crc(UCHAR ucEsId, UCHAR ucMpvId, UINT32* CurYCrC, UINT32* CurCCrc)
{
    int i,j;
	
    for(i=0;i<4;i++)
    {
        CurYCrC[i] = MISC_READ32(ucMpvId,(2+i)*4);
    }

    for(j=0;j<4;j++)
    {
        CurCCrc[j] = MISC_READ32(ucMpvId,(6+j)*4);
    }
    UNUSED(ucEsId);
}

VOID _MPV_EnableCRC(BOOL fgEn, UCHAR ucEsId, UCHAR ucMpvId)
{	  
    VDEC_ES_INFO_T *prVdecEsInfo;
    MPV_ES_INFO_T *prEsInfo;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prEsInfo = (MPV_ES_INFO_T*)_MpvGetHdlr(ucEsId);
	  
    if(fgEn)
    {
        if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLAG_DBK)) 
        {
            if ((prVdecEsInfo->ucPicType != B_TYPE) || (!prEsInfo->fgBNoDbk))
            {
                MISC_WRITE32(ucMpvId, 4, 3);
            }
            else
            {
                MISC_WRITE32(ucMpvId, 4, 1);
            }
        }
        else
        {
            MISC_WRITE32(ucMpvId, 4, 1);
        }
    }
    else
    {
        MISC_WRITE32(ucMpvId, 4, 0);
    }
}

#endif 

#ifdef VDEC_DEC_REG_QUERY
void _MPV_PrintReg(UCHAR ucMpvId)
{
    UINT32  u4Idx;
    UINT32  u4RegVal;

    for (u4Idx = 34; u4Idx <= 39; u4Idx++)
    {
        u4RegVal = _MPV_VldRead32(ucMpvId, 4 * u4Idx);
        Printf("VLD Reg %4d (0x%3x)= %8x \n", u4Idx , 4 * u4Idx, u4RegVal);
    }

    for (u4Idx = 42; u4Idx <= 255; u4Idx++)
    {
        u4RegVal = _MPV_VldRead32(ucMpvId, 4 * u4Idx);
        Printf("VLD Reg %4d (0x%3x)= %8x \n", u4Idx , 4 * u4Idx, u4RegVal);
    }

    for (u4Idx = 0; u4Idx <= 28; u4Idx++)
    {
        u4RegVal = VLD_TOP_READ32(ucMpvId, 4 * u4Idx);
        Printf("TOP Reg %4d (0x%3x)= %8x \n", u4Idx , 4 * u4Idx, u4RegVal);
    }

    for (u4Idx = 0; u4Idx <= 530; u4Idx++)
    {
        u4RegVal = _MPV_McRead32(ucMpvId, 4 * u4Idx);
        Printf(" MC Reg %4d (0x%3x)= %8x \n", u4Idx, 4 * u4Idx, u4RegVal);
    }
    for (u4Idx = 608; u4Idx <= 608; u4Idx++)
    {
        u4RegVal = _MPV_McRead32(ucMpvId, 4 * u4Idx);
        Printf(" MC Reg %4d (0x%3x)= %8x \n", u4Idx, 4 * u4Idx, u4RegVal);
    }

    for (u4Idx = 0; u4Idx <= 24; u4Idx++)
    {
        u4RegVal = PP_READ32(ucMpvId, 4 * u4Idx);
        Printf(" PP Reg %4d (0x%3x)= %8x \n", u4Idx , 4 * u4Idx, u4RegVal);
    }

    Printf("\n");

    return ;
}

void _MPV_DumpInputWnd(UCHAR ucMpvId)
{
    Printf("Input Window: 0x%X\n", _MPV_VldRead32(ucMpvId, RO_VLD_BARL));
    
    return ;
}
#endif // VDEC_DEC_REG_QUERY

