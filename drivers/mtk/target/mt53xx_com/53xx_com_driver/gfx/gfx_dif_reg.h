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
 * $RCSfile: gfx_dif_reg.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file gfx_dif_reg.h
 *  gfx driver hardware registers
 *  5371 specific
 */

/* 
    lint inhibition(s)
        46   : suppress MISRA rule 111, field type should be int
        960  : suppress MISRA rule 110, bitfields inside union
               here size of bitfield has been carefully calculated 
               to fit union size
*/

/*lint -save -e46 -e960 */

#ifndef GFX_DIF_REG_H
#define GFX_DIF_REG_H


//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

// size of gfx register space (in unit sizeof(uint32))
#define GREG_FILE_SIZE          (INT32)(GREG_LAST)

#define GFX_ENABLE              1
#define GFX_DISABLE             0

#define GFX_FIELD_MODE          1
#define GFX_FRAME_MODE          0

#define GFX_CMD_BUF_CYLIC       0
#define GFX_CMD_BUF_SINGLE      1

#define GFX_SOFT_RESET          3

#define GFX_HW_IDLE             1
#define GFX_HW_BUSY             0

#define GFX_ENGINE_FIRE         1

#define GFX_ENGINE_BURST_READ   1



//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------

// gfx hardware register enumeration
typedef enum _E_GFX_REG_T
{
    GREG_G_CONFIG           = 0x00,     // 0x4000
    GREG_G_STATUS           = 0x01,     // 0x4004
    GREG_DRAMQ_STAD         = 0x02,     // 0x4008
    GREG_DRAMQ_LEN          = 0x03,     // 0x400C
    GREG_G_MODE             = 0x04,     // 0x4010
    GREG_RECT_COLOR         = 0x05,     // 0x4014
    GREG_SRC_BSAD           = 0x06,     // 0x4018
    GREG_DST_BSAD           = 0x07,     // 0x401C
    GREG_SRC_XY             = 0x08,     // 0x4020
    GREG_DST_XY             = 0x09,     // 0x4024
    GREG_SRC_SIZE           = 0x0A,     // 0x4028
    GREG_S_OSD_WIDTH        = 0x0B,     // 0x402C
    GREG_CLIP_BR            = 0x0C,     // 0x4030
    GREG_CLIP_TL            = 0x0D,     // 0x4034
    GREG_GRAD_X_DELTA       = 0x0E,     // 0x4038
    GREG_GRAD_Y_DELTA       = 0x0F,     // 0x403C
    GREG_GRAD_XY_INC        = 0x10,     // 0x4040
    GREG_BITBLT_MODE        = 0x11,     // 0x4044
    GREG_KEY_DATA0          = 0x12,     // 0x4048
    GREG_KEY_DATA1          = 0x13,     // 0x404C
    GREG_SRCCBCR_BSAD       = 0x14,     // 0x4050
    GREG_SRCCBCR_PITCH      = 0x15,     // 0x4054
    GREG_DSTCBCR_BSAD       = 0x16,     // 0x4058
    GREG_F_COLOR            = 0x17,     // 0x405C
    GREG_B_COLOR            = 0x18,     // 0x4060
    GREG_COL_TRAN0          = 0x19,     // 0x4064
    GREG_COL_TRAN1          = 0x1A,     // 0x4068
    GREG_COL_TRAN2          = 0x1B,     // 0x406C
    GREG_COL_TRAN3          = 0x1C,     // 0x4070
    GREG_COL_TRAN4          = 0x1D,     // 0x4074
    GREG_COL_TRAN5          = 0x1E,     // 0x4078
    GREG_COL_TRAN6          = 0x1F,     // 0x407C
    GREG_COL_TRAN7          = 0x20,     // 0x4080
    GREG_STR_BLT_H          = 0x21,     // 0x4084
    GREG_STR_BLT_V          = 0x22,     // 0x4088
    GREG_STR_DST_SIZE       = 0x23,     // 0x408C
    GREG_LEGAL_START_ADDR   = 0x24,     // 0x4090
    GREG_LEGAL_END_ADDR     = 0x25,     // 0x4094
    GREG_DUMMY              = 0x26,     // 0x4098
    GREG_ALCOM_LOOP         = 0x27,     // 0x409C
    GREG_ROP                = 0x28,     // 0x40A0
    GREG_IDX2DIR            = 0x29,     // 0x40A4
    GREG_COMPRESS           = 0x2A,     // 0x40A8
    GREG_BPCOMP_CFG         = 0x2B,     // 0x40AC
    GREG_BPCOMP_AD_END      = 0x2C,     // 0x40B0
    GREG_0x40B4             = 0x2D,     // 0x40B4
    GREG_0x40B8             = 0x2E,     // 0x40B8
    GREG_0x40BC             = 0x2F,     // 0x40BC
    GREG_0x40C0             = 0x30,     // 0x40C0
    GREG_0x40C4             = 0x31,     // 0x40C4
    GREG_0x40C8             = 0x32,     // 0x40C8
    GREG_0x40CC             = 0x33,     // 0x40CC
    GREG_0x40D0             = 0x34,     // 0x40D0
    GREG_0x40D4             = 0x35,     // 0x40D4
    GREG_0x40D8             = 0x36,     // 0x40D8
    GREG_0x40DC             = 0x37,     // 0x40DC
    GREG_0x40E0             = 0x38,     // 0x40E0
    GREG_0x40E4             = 0x39,     // 0x40E4
    GREG_0x40E8             = 0x3A,     // 0x40E8
    GREG_0x40EC             = 0x3B,     // 0x40EC
    GREG_0x40F0             = 0x3C,     // 0x40F0
    GREG_0x40F4             = 0x3D,     // 0x40F4
    GREG_0x40F8             = 0x3E,     // 0x40F8
    GREG_0x40FC             = 0x3F,     // 0x40FC
    GREG_LAST               = 0x40      // 0x4100
} E_GFX_REG_T;


// gfx register name
typedef struct _MI_DIF_REG_T
{
    UINT32 u4_G_CONFIG;
    UINT32 u4_G_STATUS;
    UINT32 u4_DRAMQ_STAD;
    UINT32 u4_DRAMQ_LEN;
    UINT32 u4_G_MODE;
    UINT32 u4_RECT_COLOR;
    UINT32 u4_SRC_BSAD;
    UINT32 u4_DST_BSAD;
    UINT32 u4_SRC_XY;
    UINT32 u4_DST_XY;
    UINT32 u4_SRC_SIZE;
    UINT32 u4_S_OSD_WIDTH;
    UINT32 u4_CLIP_BR;
    UINT32 u4_CLIP_TL;
    UINT32 u4_GRAD_X_DELTA;
    UINT32 u4_GRAD_Y_DELTA;
    UINT32 u4_GRAD_XY_INC;
    UINT32 u4_BITBLT_MODE;
    UINT32 u4_KEY_DATA0;
    UINT32 u4_KEY_DATA1;
    UINT32 u4_SRCCBCR_BSA;
    UINT32 u4_SRCCBCR_PITC;
    UINT32 u4_DSTCBCR_BSA;
    UINT32 u4_F_COLOR;
    UINT32 u4_B_COLOR;
    UINT32 u4_COL_TRAN0;
    UINT32 u4_COL_TRAN1;
    UINT32 u4_COL_TRAN2;
    UINT32 u4_COL_TRAN3;
    UINT32 u4_COL_TRAN4;
    UINT32 u4_COL_TRAN5;
    UINT32 u4_COL_TRAN6;
    UINT32 u4_COL_TRAN7;
    UINT32 u4_STR_BLT_H;
    UINT32 u4_STR_BLT_V;
    UINT32 u4_STR_DST_SIZE;
    UINT32 u4_LEGAL_START_ADDR;
    UINT32 u4_LEGAL_END_ADDR;
    UINT32 u4_DUMMY;
    UINT32 u4_ALCOM_LOOP;
    UINT32 u4_ROP;
    UINT32 u4_IDX2DIR;
    UINT32 u4_COMPRESS;
    UINT32 u4_BPCOMP_CFG;
    UINT32 u4_BPCOMP_AD_END;
    UINT32 u4_Res40B4;
    UINT32 u4_Res40B8;
    UINT32 u4_Res40BC;
    UINT32 u4_Res40C0;
    UINT32 u4_Res40C4;
    UINT32 u4_Res40C8;
    UINT32 u4_Res40CC;
    UINT32 u4_Res40D0;
    UINT32 u4_Res40D4;
    UINT32 u4_Res40D8;
    UINT32 u4_Res40DC;
    UINT32 u4_Res40E0;
    UINT32 u4_Res40E4;
    UINT32 u4_Res40E8;
    UINT32 u4_Res40EC;
    UINT32 u4_Res40F0;
    UINT32 u4_Res40F4;
    UINT32 u4_Res40F8;
    UINT32 u4_Res40FC;
    UINT32 u4_LAST;

} MI_DIF_REG_T;


// gfx hw register bitfield
typedef struct _MI_DIF_FIELD_T
{
    // DWORD - G_CONFIG         (4000h)
    UINT32 fg_EN_DRAMQ          :  1;
    UINT32 fg_INT_MASK          :  1;
    UINT32 fg_POST_THRS         :  2;
    UINT32 fg_CMDFIFO_THRS      :  2;
    UINT32 fg_REQ_INTVAL        :  2;
    UINT32 fg_DRAMQ_MODE        :  1;
    UINT32 fg_SDFIFO_THRS       :  2;
    UINT32                      :  5;
    UINT32 fg_SHT_CMDQ          :  1;
    UINT32 fg_ENG_LP            :  1;
    UINT32                      :  3;
    UINT32 fg_CHECKSUM_SEL      :  1;
    UINT32                      :  2;
    UINT32 fg_REG_RST           :  2;
    UINT32 fg_G_RST_CMDQ        :  2;
    UINT32 fg_CQ_RST            :  2;
    UINT32 fg_G_RST             :  2;

    // DWORD - G_STATUS         (4004h)
    UINT32 fg_IDLE              :  1;
    #if defined(CC_MT8223)
    UINT32 fg_HWQ_LEN           :  5;
    UINT32                      :  2;
    #else
    UINT32 fg_HWQ_LEN           :  4;
    UINT32                      :  3;
    #endif
    UINT32 fg_VERSION_ID        :  8;
    UINT32 fg_CURR_Y_LINE       : 11;
    UINT32                      :  5;

    // DWORD - DRAMQ_STAD       (4008h)
    UINT32 fg_DRAMQ_BSAD        : 30;
    UINT32 fg_CYC_SIZE          :  2;

    // DWORD - DRAMQ_LEN        (400Ch)
    UINT32 fg_DRAMQ_LEN         : 18;
    UINT32                      : 14;
    
    // DWORD - G_MODE           (4010h)
    UINT32 fg_CM                :  4;
    UINT32 fg_OP_MODE           :  5;
    UINT32                      :  2;
    UINT32 fg_FIRE              :  1;
    UINT32 fg_BURST_READ        :  1;
    UINT32                      :  3;
    UINT32 fg_DSTOWN            :  1;
    UINT32 fg_SRCOWN            :  1;
    UINT32                      :  1;    
    UINT32 fg_A1555_EN            :  1;
    UINT32                      :  3;
    UINT32 fg_APP_BPCOMP        :  1;
    UINT32 fg_ALCOM_THIRD_KEY_EN : 1;
    UINT32 fg_ALCOM_SRC_KEY_EN  :  1;
    UINT32 fg_ALCOM_DIFF_CM     :  1;
    UINT32 fg_THIRD_BUF_EN      :  1;
    UINT32 fg_THIRD_CM          :  4;

    // DWORD - RECT_COLOR       (4014h)
    UINT32 fg_RECT_COLOR        : 32;

    // DWORD - SRC_BSAD         (4018h)
    #if defined(CC_MT8223)
    UINT32 fg_SRC_BSAD          : 29;
    UINT32                      :  1;
    #else
    UINT32 fg_SRC_BSAD          : 30;
    #endif
    UINT32 fg_CHAR_CM           :  2;

    // DWORD - DST_BSAD         (401Ch)
    #if defined(CC_MT8223)
    UINT32 fg_DST_BSAD          : 29;
    UINT32                      :  3;
    #else
    UINT32 fg_DST_BSAD          : 30;
    UINT32                      :  2;
    #endif

    // DWORD - SRC_XY           (4020h)
    UINT32 fg_SRCX              : 15;
    UINT32                      :  1;
    UINT32 fg_SRCY              : 11;
    UINT32                      :  5;
    
    // DWORD - DST_XY           (4024h)
    UINT32 fg_DSTX              : 15;
    UINT32                      :  1;
    UINT32 fg_DSTY              : 11;
    UINT32                      :  5;

    // DWORD - SRC_SIZE         (4028h)
    UINT32 fg_SRC_WIDTH         : 15;
    UINT32 fg_RL_DEC            :  1;
    UINT32 fg_SRC_HEIGHT        : 11;
    UINT32 fg_SRC_CM            :  4;
    UINT32 fg_SPU_RLDEC         :  1;

    // DWORD - S_OSD_WIDTH      (402Ch)
    UINT32 fg_OSD_WIDTH         : 16;
    UINT32 fg_SRC_PITCH         : 16;

    // DWORD - CLIP_BR          (4030h)
    UINT32 fg_CLIP_RIGHT        : 11;
    UINT32 fg_CLR_ENA           :  1;
    UINT32                      :  4;
    UINT32 fg_CLIP_BOT          : 11;
    UINT32 fg_CLB_ENA           :  1;
    UINT32                      :  4;

    // DWORD - CLIP_TL          (4034h)
    UINT32 fg_CLIP_LEFT         : 11;
    UINT32 fg_CLL_ENA           :  1;
    UINT32                      :  4;
    UINT32 fg_CLIP_TOP          : 11;
    UINT32 fg_CLT_ENA           :  1;
    UINT32                      :  4;

    // DWORD - GRAD_X_DELTA     (4038h)
    UINT32 fg_DELTA_X_C0        :  8;
    UINT32 fg_DELTA_X_C1        :  8;
    UINT32 fg_DELTA_X_C2        :  8;
    UINT32 fg_DELTA_X_C3        :  8;

    // DWORD - GRAD_Y_DELTA     (403Ch)
    UINT32 fg_DELTA_Y_C0        :  8;
    UINT32 fg_DELTA_Y_C1        :  8;
    UINT32 fg_DELTA_Y_C2        :  8;
    UINT32 fg_DELTA_Y_C3        :  8;

    // DWORD - GRAD_XY_INC      (4040h)
    UINT32 fg_GRAD_X_PIX_INC    : 11;
    UINT32                      :  5;
    UINT32 fg_GRAD_Y_PIX_INC    : 11;
    UINT32                      :  3;
    UINT32 fg_GRAD_MODE         :  2;

    // DWORD - BITBLT_MODE      (4044h)
    UINT32 fg_TRANS_ENA         :  1;
    UINT32 fg_KEYNOT_ENA        :  1;
    UINT32 fg_COLCHG_ENA        :  1;
    UINT32                      :  1;
    UINT32 fg_CFMT_ENA          :  1;
    UINT32 fg_KEYSDSEL          :  1;
    UINT32 fg_PRCN_OPT          :  1;
    UINT32 fg_FORCE_USE_ALPHA   :  1;
    UINT32 fg_ALPHA_VALUE       :  8;
    UINT32 fg_ALCOM_PASS        :  3;
    UINT32                      :  5;
    UINT32 fg_DSTPITCH_DEC      :  1;
    UINT32 fg_MIRROR            :  1;
    UINT32 fg_SRCPITCH_DEC      :  1;
    UINT32 fg_SRCMIRROR         :  1;
    UINT32                      :  4;

    // DWORD - KEY_DATA0        (4048h)
    UINT32 fg_COLOR_KEY_MIN     : 32;

    // DWORD - KEY_DATA1        (404Ch)
    UINT32 fg_COLOR_KEY_MAX     : 32;

    // DWORD - SRCCBCR_BSA      (4050h)
    #if defined(CC_MT8223)
    UINT32 fg_SRCCBCR_BSAD      : 29;
    UINT32                      :  1;
    #else
    UINT32 fg_SRCCBCR_BSAD      : 30;
    #endif
    UINT32 fg_YC_FMT            :  2;

    // DWORD - SRCCBCR_PITCH    (4054h)
    UINT32 fg_SRCCBCR_PITCH     : 16;
    UINT32 fg_VSTD              :  1;
    UINT32 fg_VSYS              :  1;
    UINT32 fg_VSCLIP            :  1;
    UINT32 fg_FLD_PIC           :  1;
    UINT32 fg_SWAP_MODE         :  2;
    UINT32 fg_PROJ_CODE         :  1;
    UINT32 fg_VFULL             :  1;
    UINT32                      :  8;

    // DWORD - DSTCBCR_BSAD     (4058h)
    UINT32 fg_DSTCBCR_BSAD      : 30;
    UINT32                      :  2;

    // DWORD - F_COLOR (1)      (405Ch)
    UINT32 fg_FORE_COLOR        : 32;

    // DWORD - B_COLOR (0)      (4060h)
    UINT32 fg_BACK_COLOR        : 32;

    // DWORD - COL_TRAN0        (4064h)
    UINT32 fg_COLOR_TRANS0      : 32;

    // DWORD - COL_TRAN1        (4068h)
    UINT32 fg_COLOR_TRANS1      : 32;

    // DWORD - COL_TRAN2        (406Ch)
    UINT32 fg_COLOR_TRANS2      : 32;

    // DWORD - COL_TRAN3        (4070h)
    UINT32 fg_COLOR_TRANS3      : 32;

    // DWORD - COL_TRAN4        (4074h)
    UINT32 fg_COLOR_TRANS4      : 32;

    // DWORD - COL_TRAN5        (4078h)
    UINT32 fg_COLOR_TRANS5      : 32;

    // DWORD - COL_TRAN6        (407Ch)
    UINT32 fg_COLOR_TRANS6      : 32;

    // DWORD - COL_TRAN7        (4080h)
    UINT32 fg_COLOR_TRANS7      : 32;

    // DWORD - STR_BLT_H        (4084h)
    UINT32 fg_STR_BLT_H         : 24;
    UINT32                      :  8;

    // DWORD - STR_BLT_V        (4088h)
    UINT32 fg_STR_BLT_V         : 24;
    UINT32                      :  8;

    // DWORD - STR_DST_SIZE     (408Ch)
    UINT32 fg_STR_DST_WIDTH     : 15;
    UINT32                      :  1;
    UINT32 fg_STR_DST_HEIGHT    : 11;
    UINT32                      :  5;

    // DWORD - LEGAL_START_ADDR (4090h)
    #if defined(CC_MT8223)
    UINT32 fg_LEGAL_START_ADDR  : 29;
    UINT32                      :  2;
    #else
    UINT32 fg_LEGAL_START_ADDR  : 28;
    UINT32                      :  3;
    #endif
    UINT32 fg_MEM_PROTECT_EN    :  1;
    
    // DWORD - LEGAL_END_ADDR   (4094h)
    #if defined(CC_MT8223)
    UINT32 fg_LEGAL_END_ADDR    : 29;
    UINT32                      :  3;
    #else
    UINT32 fg_LEGAL_END_ADDR    : 28;
    UINT32                      :  4;
    #endif

    // DWORD - DUMMY            (4098h)
    UINT32 fg_DUMMY             : 32;

    // DWORD - ALCOM_LOOP       (409Ch)
    UINT32 fg_ALCOM_AR          :  8;
    UINT32 fg_ALCOM_OPCODE      :  4;
    UINT32                      :  4;
    UINT32 fg_ALCOM_RECT_SRC    :  1;
    UINT32 fg_ALCOM_NORMAL      :  1;
    UINT32 fg_PREMULT_DST_W       : 1;
    UINT32 fg_PREMULT_DST_R       : 1;
    UINT32 fg_PREMULT_SRC_R       : 1;
    UINT32 fg_PREMULT_OVERFLOW_EN       : 1;
    UINT32                          : 10;

    // DWORD - ROP              (40A0h)
    UINT32 fg_ROP_OPCODE        :  4;
    UINT32                      :  4;
    UINT32 fg_ROP_NO_WR         :  1;
    UINT32 fg_ROP_CMP_FLAG      :  1;
    UINT32                      : 22;

    // DWORD - IDX2DIR          (40A4h)
    UINT32 fg_PAL_BSAD          : 30;
    UINT32 fg_MSB_LEFT          :  1;
    UINT32 fg_LN_ST_BYTE_AL     :  1;

    // DWORD - COMPRESS         (40A8h)
    UINT32 fg_A_METHOD          :  1;
    UINT32 fg_COMPRESS_LP       :  1;
    UINT32                      : 30;

    //DWORD - BPCOMP_CFG for new compression (40ACh)
    UINT32 fg_ROLL_BACK_EN      : 1;
    UINT32 fg_QUALITY_MODE      : 2;
    UINT32 fg_LINE_SEPARATE     : 1;
    UINT32                      : 28;
    
    //DWORD - BPCOMP_AD_END for new compression (40B0h)
    #if defined(CC_MT8223)
    UINT32 fg_BPCOMP_AD_END     : 29;
    UINT32                      : 3;
    #else
    UINT32                      : 4;
    UINT32 fg_BPCOMP_AD_END     : 24;
    UINT32                      : 4;
    #endif
    // DWORD - Reserved 19 Regs
    UINT32 fg_Res40B4           : 32;
    UINT32 fg_Res40B8           : 32;
    UINT32 fg_Res40BC           : 32;
    UINT32 fg_Res40C0           : 32;
    UINT32 fg_Res40C4           : 32;
    UINT32 fg_Res40C8           : 32;
    UINT32 fg_Res40CC           : 32;
    UINT32 fg_THIRD_BSAD        : 30;
    UINT32 fg_THIRD_CHAR_CM     : 2;
    UINT32 fg_THIRD_PAL_BSAD    : 30;
    UINT32 fg_THIRD_MSB_LEFT    : 1;
    UINT32 fg_THIRD_LN_ST_BYTE_AL : 1;
    UINT32 fg_THIRD_PITCH       : 16;
    UINT32                      : 16;
    UINT32 fg_ALCOM_SRC_KEY     : 32;
    UINT32 fg_ALCOM_THIRD_KEY   : 32;
    UINT32 fg_Res40E4           : 32;
    UINT32 fg_Res40E8           : 32;
    UINT32 fg_Res40EC           : 32;
    UINT32 fg_Res40F0           : 32;
    UINT32 fg_Res40F4           : 32;
    UINT32 fg_Res40F8           : 32;
    UINT32 fg_Res40FC           : 32;
    // DWORD - LAST            (4100h)
    // DWORD - LAST            (4100h)
    UINT32 fg_LAST              : 32;
} MI_DIF_FIELD_T;


// gfx hw register name/bitfield union
typedef union _MI_DIF_UNION_T
{
    MI_DIF_REG_T   rReg;
    MI_DIF_FIELD_T rField;
} MI_DIF_UNION_T;

#endif // GFX_DIF_REG_H
/*lint -restore */


