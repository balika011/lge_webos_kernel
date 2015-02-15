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
 * $RCSfile: b2r_hw_v1.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file b2r_hw_v1.c
 *  b2r  hardware interfaces
 *  purely hardware related functions, include crash detection routines,
 */

#define __NPTV_HW_IF
//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
#include "x_hal_5381.h"
#include "x_os.h"
#include "x_util.h"
#include "b2r_hal.h"
#include "b2r_hw_v2.h"
#include "b2r_reg_v2.h"
#include "b2r_debug.h"
#ifdef __NPTV_HW_IF
#include "hw_sys.h"
#include "hw_vdoin.h"
#include "drv_di.h"
#endif
#ifdef VDP_B2R_PSCAN_CRC_FLOW
#include "hw_mjc.h"
#endif
//#include "vdp_hw.h"

//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------
#define HW_NS 2

#define HD_MAX_WIDTH      3840
#define HD_MAX_HEIGHT     2160

#define SD_MAX_WIDTH      1920
#define SD_MAX_HEIGHT     1088

#define DMX_PCR_NS 2
#define PATH_NS 2

//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------
#define IDEAL_CLOCK_FREQUENCY     27000000
#define SOURCE_CLOCK_FREQUENCY_LO 216000000  
#define SOURCE_CLOCK_FREQUENCY_HI 324000000  

#define HD_CKGEN_REG 0x228  
#define SD_CKGEN_REG 0x230  
#define HD_CKDIV_REG 0x22c  
#define SD_CKDIV_REG 0x234  


#define B2R_ISR_VEI_MASK        0x00000080
#define B2R_ISR_ADI_MASK        0x00000100
#define B2R_ISR_FI_MASK         0x00000200
#define NBLK2RS_BASE                (IO_VIRT + 0x26800)

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------
#define DIVIDE_WITH_CARRIER(dividend, divisor)      (divisor==0? ((dividend) + ((divisor) - 1)) : (((dividend) + ((divisor) - 1)) / (divisor)))


//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------
#define B2R_REG_OFFSET(id) (((id) == 1) ? B2R_SD_BASE : B2R_HD_BASE)
#define BLK2RS_BASE                (IO_VIRT + 0x26000)
#define IO_B2R_READ_BITS(RegName,FieldName)        IO32ReadFldAlign((NBLK2RS_BASE+(RegName)) ,(FieldName))   
#define IO_B2R_READ_REG(RegName)                   u4IO32Read4B((NBLK2RS_BASE+(RegName))) 
#define IO_B2R_WRITE_BITS(RegName,FieldName,value) vIO32WriteFldAlign((NBLK2RS_BASE+(RegName)) ,(value) ,(FieldName))
#define IO_B2R_WRITE_REG(RegName, value)           vIO32Write4B((NBLK2RS_BASE+(RegName)), (value))
#define IO_B2R_WRITE_DRAM_EN(RegName, value)           vIO32Write4B((BLK2RS_BASE+(RegName)), (value))
#define IO_B2R_READ_DRAM_EN(RegName)           u4IO32Read4B((BLK2RS_BASE+(RegName))) 
#define IO_B2R_WRITE_BITS_COM(RegName,FieldName,value) vIO32WriteFldAlign((BLK2RS_BASE+(RegName)) ,(value) ,(FieldName))
#ifdef VDP_B2R_PSCAN_CRC_FLOW
#define IO_MJC_READ_REG(RegName)                   u4IO32Read4B((RegName))
#endif
//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Static function forward declarations
//---------------------------------------------------------------------------
static void _B2R_HW_Read_Status(UINT8 u1Id, B2R_HAL_STATUS_T * ptStatus);
static void _B2R_HW_Set_Frc(B2R_HAL_OBJECT_T *this, UCHAR u2TargetNs, UCHAR u2ChangeFieldNs, UCHAR ucMode);

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------
static B2R_HAL_OBJECT_T _b2r_hw_v2_object[HW_NS];

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------
//Greatest Common Divisor
static UINT32 __gcd(UINT32 u4Value1, UINT32 u4Value2)
{
    UINT32 u4Mod = 0;
    UINT32 u4Tmp = 0;
    if(u4Value1 < u4Value2)
    {
        u4Tmp = u4Value2;
        u4Value2 = u4Value1;
        u4Value1 = u4Tmp;
    }
    if(u4Value2 == 0) 
    {
         return 1; 
    }
    u4Mod = u4Value1%u4Value2;
    while(u4Mod)
    {
        u4Value1 = u4Value2;
        u4Value2 = u4Mod;
        u4Mod = u4Value1%u4Value2;
    }
    return u4Value2;
}

inline static void _Hw_Enable(UINT8 u1Id)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);

    //todo power on

    IO_B2R_WRITE_BITS(B2R_MISC, DRAM_CK_EN, 1);

    // enable double register buffer
    IO_B2R_WRITE_BITS(B2R_EN + u4Offset, DBF_EN, 1);
   	IO_B2R_WRITE_DRAM_EN(B2R_MISC,0x80);
    //Enable adjustable interrupt
    IO_B2R_WRITE_BITS(B2R_EN + u4Offset, ADJI_EN, 1);
    IO_B2R_WRITE_BITS(B2R_EN + u4Offset, PATH_EN, 1);

    IO_B2R_WRITE_BITS(B2R_TRIGGER + u4Offset, SET_FIELD, 1);
    IO_B2R_WRITE_BITS(B2R_TRIGGER + u4Offset, SET_FIELD, 0);
 
    IO_B2R_WRITE_BITS(B2R_TRIGGER + u4Offset, TGEN_RST, 1);
    IO_B2R_WRITE_BITS(B2R_TRIGGER + u4Offset, TGEN_RST, 0);
 }

inline static void _Hw_Disable(UINT8 u1Id)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
//     NB2R_REG_VB1(0x34010,0x00);
    //Disable 4K Mode
    IO_B2R_WRITE_BITS(B2R_BREN, B4K_MODE, 0);    
    IO_B2R_WRITE_BITS(B2R_BREN, HD_VB1_MODE, 0);
    IO_B2R_WRITE_BITS(B2R_MODE_CTRL, HD_VS_INV, 0); 
    IO_B2R_WRITE_BITS(B2R_MODE_CTRL, HD_HS_INV, 0); 
    //Disable Smart PCR
    if (u1Id == 0)
    {
        IO_B2R_WRITE_BITS(B2R_MISC, HD_SM_PCR_EN, 0);
    }
    else
    {
        IO_B2R_WRITE_BITS(B2R_MISC, SD_SM_PCR_EN, 0);
    }

    //Disable double register buffer
    IO_B2R_WRITE_BITS(B2R_EN + u4Offset, DBF_EN, 0);

    //Disable adjustable interrupt
    IO_B2R_WRITE_BITS(B2R_EN + u4Offset, PATH_EN, 0);
    IO_B2R_WRITE_BITS(B2R_EN + u4Offset, ADJI_EN, 0);

    IO_B2R_WRITE_BITS(B2R_HV_PORCH + u4Offset, FIELD_DISP_R_CHANGE, 0);
    IO_READ32(PSCAN_BASE, 0x150) = (IO_READ32(PSCAN_BASE, 0x150) & 0xBFFFFFFF);

    IO_B2R_WRITE_BITS(B2R_TRIGGER + u4Offset, SET_FIELD, 1);
    IO_B2R_WRITE_BITS(B2R_TRIGGER + u4Offset, SET_FIELD, 0);

    // 64 FIFO
    // 32 x 16 bytes FIFO for each Y/C
    IO_B2R_WRITE_BITS(B2R_LINE_OFFSET + u4Offset, FIFO_SEL, 0);
    //todo power off
}
void B2R_HW_Enable(BOOL fgEnable)///For VB1 bypass used
{
    B2R_HAL_OBJECT_T *this;
    this = &_b2r_hw_v2_object[0];
    
    if(this->fgVB1Enable)
    {
        if(fgEnable)
        {
        _Hw_Enable(B2R_HD_BASE);
        }
        else
        {
        _Hw_Disable(B2R_HD_BASE);
        }
    }
}
inline static void _Hw_Reset(UINT8 u1Id)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);

    // DRAM Req = 32
    IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, REQR_TH, 3);

    //DRAM Burst Read
    IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, BURST_READ_EN, 1);

    // HBTD = HTBD = 0
    IO_B2R_WRITE_BITS(B2R_HV_TOTAL + u4Offset, FLD_F_DLY, 0);
    IO_B2R_WRITE_BITS(B2R_HV_TOTAL + u4Offset, FLD_R_DLY, 0);

    // FMON (Underflow Check)
    IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, FIFO_MON_EN, 1);

    // Reset: Write 1 than Write 0
    if (u1Id == 0)
    {
        //smart pcr error
        IO_B2R_WRITE_REG(B2R_HD_PCR_ERR, 0);

        //dram reset
        IO_B2R_WRITE_BITS(B2R_MISC, HD_DRAM_RST, 1);
        IO_B2R_WRITE_BITS(B2R_MISC, HD_DRAM_RST, 0);

        //rst reset
        IO_B2R_WRITE_BITS(B2R_MISC, HD_RST, 1);
        IO_B2R_WRITE_BITS(B2R_MISC, HD_RST, 0);
    }
    else
    {
        //smart pcr error
        IO_B2R_WRITE_REG(B2R_SD_PCR_ERR, 0);


        IO_B2R_WRITE_BITS(B2R_MISC, SD_DRAM_RST, 1);
        IO_B2R_WRITE_BITS(B2R_MISC, SD_DRAM_RST, 0);

        IO_B2R_WRITE_BITS(B2R_MISC, SD_RST, 1);
        IO_B2R_WRITE_BITS(B2R_MISC, SD_RST, 0);
    }
  
}

inline static void _Hw_Set_YC_Start(UINT8 u1Id, UINT32 u4YAddr, UINT32 u4CAddr)
{   
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    IO_B2R_WRITE_REG(B2R_YS + u4Offset,u4YAddr);
    IO_B2R_WRITE_REG(B2R_CS + u4Offset,u4CAddr);
}

inline static void _Hw_Get_YC_Start(UINT8 u1Id, UINT32 *pu4YAddr, UINT32 *pu4CAddr)
{   
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    *pu4YAddr = IO_B2R_READ_REG(B2R_YS + u4Offset);
    *pu4CAddr = IO_B2R_READ_REG(B2R_CS + u4Offset);
}


inline static void _Hw_Set_YC_Len(UINT8 u1Id, UINT32 u4YLen, UINT32 u4CLen)
{   
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    IO_B2R_WRITE_REG(B2R_YS_LEN + u4Offset, u4YLen);
    IO_B2R_WRITE_REG(B2R_CS_LEN + u4Offset, u4CLen);
}

inline static void _Hw_Set_YC_Start_R(UINT8 u1Id, UINT32 u4YAddr, UINT32 u4CAddr)
{   
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    IO_B2R_WRITE_REG(B2R_YS_R_VIEW + u4Offset,u4YAddr);
    IO_B2R_WRITE_REG(B2R_CS_R_VIEW + u4Offset,u4CAddr);
}

inline static void _Hw_Set_YC_Len_R(UINT8 u1Id, UINT32 u4YLen, UINT32 u4CLen)
{   
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    IO_B2R_WRITE_REG(B2R_YS_R_LEN + u4Offset, u4YLen);
    IO_B2R_WRITE_REG(B2R_CS_R_LEN + u4Offset, u4CLen);
}


inline static void _Hw_Set_ColorFormat(UINT8 u1Id, UINT32 u4ChromaFmt, UINT32 u4BitDepth)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, M422, u4ChromaFmt);
    UNUSED(u4BitDepth);
}

/*
Set dram pitch, block mode or raster mode
*/
inline static void _Hw_Set_DramFormat(UINT8 u1Id, UINT32 u4AddrMode, UINT32 u4SwapMode, UINT32 u4Pitch)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
//    IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, ADDR_MODE, u4AddrMode);[24-25 bit used only for VB1  new B2R]
  //  IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, ADDR_SWAP, u4SwapMode);

    if ((u4Pitch >> 4) <= 0xFF)
    {
        IO_B2R_WRITE_BITS(B2R_LINE_OFFSET + u4Offset, LINE_OFFSET, (u4Pitch >> 4));
        IO_B2R_WRITE_BITS(B2R_LINE_OFFSET + u4Offset, LINE_OFFSET_H, 0);
    }
    else
    {
        u4Pitch = ((u4Pitch >> 4) & 0x000000FF);
        IO_B2R_WRITE_BITS(B2R_LINE_OFFSET + u4Offset, LINE_OFFSET, u4Pitch);
        IO_B2R_WRITE_BITS(B2R_LINE_OFFSET + u4Offset, LINE_OFFSET_H, 1);
    }

}

inline static void _Hw_Set_PackFormat(UINT8 u1Id, UINT32 u4PackMode, BOOL fgUfoEn)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    if (fgUfoEn)
    {
        IO_B2R_WRITE_BITS(B2R_BREN + u4Offset, PACK_EN, 0);
    }
    else
    {
        IO_B2R_WRITE_BITS(B2R_BREN + u4Offset, PACK_EN, 1);
    }
    
    IO_B2R_WRITE_BITS(B2R_BREN + u4Offset, PACK_MODE, u4PackMode);
}

inline static void _Hw_Set_UFO(UINT8 u1Id, UINT32 u4UFO)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    IO_B2R_WRITE_BITS(B2R_UFO_CFG + u4Offset, UFO_MODE, u4UFO);
}

inline static void _Hw_Set_4KMode(UINT8 u1Id, UINT32 u44K)
{
    UNUSED(u1Id);
    IO_B2R_WRITE_BITS(B2R_BREN, B4K_MODE, u44K);
}


inline static void _Hw_Set_ActiveSize(UINT8 u1Id, UINT32 u4HActive, UINT32 u4VActive)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    IO_B2R_WRITE_BITS(B2R_ACTIVE + u4Offset, HDEW, u4HActive);
    IO_B2R_WRITE_BITS(B2R_ACTIVE + u4Offset, VDEW, u4VActive);
}

inline static void _Hw_Set_TotalSize(UINT8 u1Id, UINT32 u4HTotal, UINT32 u4VTotal)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    IO_B2R_WRITE_BITS(B2R_HV_TOTAL + u4Offset, H_TOTAL, u4HTotal);
    IO_B2R_WRITE_BITS(B2R_HV_TOTAL + u4Offset, V_TOTAL, u4VTotal);
}

inline static void _Hw_Set_HdeRatio(UINT8 u1Id, UINT32 u4Ratio)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    IO_B2R_WRITE_BITS(B2R_HDE_RATIO + u4Offset, HDE_RATIO, u4Ratio);
}


inline static void _Hw_Set_Clock(UINT8 u1Id, UINT32 *pu4Clock)
{
    UINT32 u4CKGEN_REG, u4CKDIV_REG;
    UINT32 u4CLK_N, u4CLK_M;
    UINT32 u4IDEAL_CK;
    UINT32 u4PLL_N, u4PLL_M;
    UINT32 u4Clock;
    UINT32 u4CKGEN_SEL;
    UINT32 SOURCE_CLOCK_FREQUENCY;
    ASSERT(pu4Clock != NULL);

/*
    u4CKGEN_REG = (0 == u1Id)? HD_CKGEN_REG : SD_CKGEN_REG;
    u4CKDIV_REG = (0 == u1Id)? HD_CKDIV_REG : SD_CKDIV_REG; 
    */
    u4CKGEN_REG = (0 == u1Id)? SD_CKGEN_REG : HD_CKGEN_REG;
    u4CKDIV_REG = (0 == u1Id)? SD_CKDIV_REG : HD_CKDIV_REG; 
    
    u4IDEAL_CK  = (0 == u1Id)? B2R_HD_IDEAL_CK : B2R_SD_IDEAL_CK;
    u4Clock = *pu4Clock;
    u4CKGEN_SEL = 0x1; //216Mhz as default

    switch (u4Clock)
    {
    case 27000000:
        u4CLK_M = 1;
        u4CLK_N = 8;
        u4PLL_N = 2;
        u4PLL_M = 2;
        break;
    case 27027000:
        u4CLK_M = 1001;
        u4CLK_N = 8000;
        u4PLL_N = 0x3e9;
        u4PLL_M = 0x3e8;
        break;
    case 54000000:
        u4CLK_M = 2;
        u4CLK_N = 8;
        u4PLL_N = 0x4;
        u4PLL_M = 2;
        break;
    case 54054000:
        u4CLK_M = 2002;
        u4CLK_N = 8000;
        u4PLL_N = 0x7D2;
        u4PLL_M = 0x3e8;
        break;
    case 74176000:
        u4CLK_M = 125;
        u4CLK_N = 364;
        u4PLL_N = 0x1f4;
        u4PLL_M = 0xb6;
        break;
    case 74250000:
        u4CLK_M = 11;
        u4CLK_N = 32;
        u4PLL_N = 0xb;
        u4PLL_M = 0x4;
        break;
    case 148352000:
        u4CLK_M = 125;
        u4CLK_N = 182;
        u4PLL_N = 0x1f4;
        u4PLL_M = 0x5b;
        break;
    case 148500000:
        u4CLK_M = 11;
        u4CLK_N = 16;
        u4PLL_N = 11;
        u4PLL_M = 2;
        break;
    case 296704000:
        u4CLK_M = 9272;
        u4CLK_N = 10125;
        u4PLL_N = 580;
        u4PLL_M = 52;
        u4CKGEN_SEL = 0x201; //324 Mhz
        break;
    case 297000000:
        u4CLK_M = 11;
        u4CLK_N = 12;
        u4PLL_N = 11;
        u4PLL_M = 1;
        u4CKGEN_SEL = 0x201; //324 Mhz
        break;
    default:
    {
        UINT32 i;

        SOURCE_CLOCK_FREQUENCY = (u4Clock <= SOURCE_CLOCK_FREQUENCY_LO) ? SOURCE_CLOCK_FREQUENCY_LO : SOURCE_CLOCK_FREQUENCY_HI;
        u4CKGEN_SEL = (u4Clock <= SOURCE_CLOCK_FREQUENCY_LO) ? 0x1 : 0x201; 

        for( i=1; ; i=(i<<1))
        {
            UINT32 u4GCD;
            u4Clock *= i;

            //4K2K Target Clock will exceed SOURCE_CLOCK_FREQUENCY_HI , So remove this assert
            //ASSERT(u4Clock < SOURCE_CLOCK_FREQUENCY);
            u4GCD = __gcd(SOURCE_CLOCK_FREQUENCY, u4Clock);
            u4CLK_M = u4Clock/u4GCD;
            u4CLK_N = SOURCE_CLOCK_FREQUENCY/u4GCD;

            if (u4CLK_M >= 0x10000 || u4CLK_N >= 0x10000)
            {
                continue;
            }

            ASSERT((u4CLK_M < 0x10000) && (u4CLK_N < 0x10000));

            u4GCD = __gcd(IDEAL_CLOCK_FREQUENCY, u4Clock);
            u4PLL_N = u4Clock/u4GCD;
            u4PLL_M = IDEAL_CLOCK_FREQUENCY/u4GCD;

            while ((u4PLL_N >= 0x400) || (u4PLL_M >= 0x400)) 
            {
                u4PLL_N >>= 1;
                u4PLL_M >>= 1;
            }
            ASSERT((u4PLL_N < 0x400) && (u4PLL_M < 0x400));

            break;
        }
        *pu4Clock = u4Clock;
    }
        break;
    }

    // u4Clock = total * u4CLK_M / u4CLK_N
    // u4Clock = 27M * u4PLL_N / u4PLL_M

    IO_REG32(CKGEN_BASE, u4CKGEN_REG) = u4CKGEN_SEL;
    IO_REG32(CKGEN_BASE, u4CKDIV_REG) = (IO_REG32(CKGEN_BASE, u4CKDIV_REG) & 0xFFFF0000) + u4CLK_M;            // M
    IO_REG32(CKGEN_BASE, u4CKDIV_REG) = (IO_REG32(CKGEN_BASE, u4CKDIV_REG) & 0x0000FFFF) + (u4CLK_N << 16);    // N
    IO_B2R_WRITE_BITS(u4IDEAL_CK, PLL_N, u4PLL_N);
    IO_B2R_WRITE_BITS(u4IDEAL_CK, PLL_M, u4PLL_M);

    B2R_HAL_TRACE("clock = %d, CLK(M/N = %d/%d) PLL(N/M = %d,%d)\n", u4Clock, u4CLK_M, u4CLK_N, u4PLL_N, u4PLL_M);
    
}

inline static void _Hw_Set_3D(UINT8 u1Id, B2R_HAL_3D_T *pt3d, BOOL fgProg)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    UINT32 u4Val = 0;

    IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, AUTO_HV_OFFSET, 0);
    IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, DISP_R_VIEW_MODE, 0);
    IO_B2R_WRITE_BITS(B2R_OFFSET + u4Offset, V_OFFSET, 0);
    IO_B2R_WRITE_BITS(B2R_OFFSET + u4Offset, H_OFFSET, 0);
    IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, INTERLACE_3D_EN, 0);
    IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, TWO_ADDR_EN, 0);
    IO_B2R_WRITE_BITS(B2R_HV_PORCH + u4Offset, TAG_MODE, 0);
    IO_B2R_WRITE_BITS(B2R_HV_PORCH + u4Offset, FIELD_DISP_R_CHANGE, 1);
    if (pt3d && pt3d->fg3D)
    {
        #ifdef B2R_EMU
        IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, DISP_R_VIEW_MODE, 2);
        #else
        IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, DISP_R_VIEW_MODE, 3);
        #endif
        switch(pt3d->u4Layout)
        {
        case B2R_3D_FRAME_SEQUENTIAL:
            IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, TWO_ADDR_EN, 1);
            IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, INTERLACE_3D_EN, fgProg ? 0:1);
            break; 
            
        case B2R_3D_SIDE_BY_SIDE_REAL_D:
            u4Val = (pt3d->u4Data & 0xFFFF0000) >> 16;
            IO_B2R_WRITE_BITS(B2R_HV_PORCH + u4Offset, TAG_MODE, 3);
            IO_B2R_WRITE_BITS(B2R_HV_PORCH + u4Offset, TAG_LINE_NUM, u4Val);
            break;
            
        case B2R_3D_SIDE_BY_SIDE:
            u4Val = (pt3d->u4Data & 0xFFFF);
            IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, AUTO_HV_OFFSET, 1);
            IO_B2R_WRITE_BITS(B2R_OFFSET + u4Offset, H_OFFSET, u4Val); 
            break;
            
        case B2R_3D_TOP_N_BOTTOM:
            u4Val = (pt3d->u4Data & 0xFFFF);
            IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, AUTO_HV_OFFSET, 1);
            IO_B2R_WRITE_BITS(B2R_OFFSET + u4Offset, V_OFFSET, u4Val); 
            break;
        }
    }
    
}

inline static void _Hw_Set_Mirror(UINT8 u1Id, BOOL fgEnable)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, MIRROR_EN, fgEnable);
}

inline static void _Hw_Set_Interlaced(UINT8 u1Id, BOOL fgEnable)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, OUT_I, fgEnable);
    IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, C_420I, fgEnable);

    if(fgEnable)
    {
        IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, FIELD_INV, 1);
    }
}

inline static void  _Hw_Set_InterruptDelay(UINT8 u1Id, UINT32 u4Delay)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    IO_B2R_WRITE_BITS(B2R_HDE_RATIO + u4Offset, INT_ADJ, u4Delay);
}

inline static void  _Hw_Set_DramReq(UINT8 u1Id, UINT32 u4Req)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, REQR_TH, u4Req);
}

//This HW API use to reset UFO frame in Oryx
inline static void _Hw_Set_Flip(UINT8 u1Id, BOOL fgEnable)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, FLIP_EN, fgEnable);
}

inline static void _Hw_Set_Resolution(UINT8 u1Id, BOOL fgEnable)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    IO_B2R_WRITE_BITS(B2R_ACTIVE + u4Offset, SET_RESOLUTION, fgEnable);
}

inline static void _Hw_Set_Pitch(UINT8 u1Id, UINT32 u4Pitch)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    
    if ((u4Pitch >> 4) <= 0xFF)
    {
        IO_B2R_WRITE_BITS(B2R_LINE_OFFSET + u4Offset, LINE_OFFSET, (u4Pitch >> 4));
        IO_B2R_WRITE_BITS(B2R_LINE_OFFSET + u4Offset, LINE_OFFSET_H, 0);
    }
    else
    {
        u4Pitch = ((u4Pitch >> 4) & 0x000000FF);
        IO_B2R_WRITE_BITS(B2R_LINE_OFFSET + u4Offset, LINE_OFFSET, u4Pitch);
        IO_B2R_WRITE_BITS(B2R_LINE_OFFSET + u4Offset, LINE_OFFSET_H, 1);
    }
}

inline static void _Hw_Set_Pattern(UINT8 u1Id, B2R_HAL_TP_T * prArg)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);

    if (prArg && prArg->fgEnable)
    {
        UINT32 u4Val;
        //3d bit
        if (IO_B2R_READ_BITS(B2R_MODE_CTRL + u4Offset, DISP_R_VIEW_MODE))
        {
            IO_B2R_WRITE_BITS(B2R_TP_ON + u4Offset, TP_3D, 1);
        }

        //grid
        u4Val = (UINT32)(prArg->eMode - B2R_TP_NO_GRID);
        IO_B2R_WRITE_BITS(B2R_TP_ON + u4Offset, TP_GRID_OP, u4Val);
        
        //grid paramters
        if (B2R_TP_NO_GRID != prArg->eMode)
        {
            IO_B2R_WRITE_REG(B2R_TP_GRID  + u4Offset, prArg->u4Reserve);        
            IO_B2R_WRITE_REG(B2R_TP_COLOR + u4Offset, prArg->u4Color);        

        }
        else
        {
            IO_B2R_WRITE_BITS(B2R_TP_ON + u4Offset, TP_YCBCR, prArg->u4Color);        
        }

        //enable bit
        IO_B2R_WRITE_BITS(B2R_TP_ON + u4Offset, TP_ON, 1);
        
    }
    else
    {
        //disable
        IO_B2R_WRITE_BITS(B2R_TP_ON + u4Offset, TP_ON, 0);
        IO_B2R_WRITE_BITS(B2R_TP_ON + u4Offset, TP_3D, 0);
        IO_B2R_WRITE_BITS(B2R_TP_ON + u4Offset, TP_GRID_OP, 0);
    }
}

inline static void _Hw_Set_DefaultSetting(UINT8 u1Id)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);

    if (u1Id == 0)
    {
        IO_B2R_WRITE_BITS(B2R_INT_CLR + u4Offset, HD_PCR_RSTR, 1);
    }
    else
    {
        IO_B2R_WRITE_BITS(B2R_INT_CLR + u4Offset, SD_PCR_RSTR, 1);
    }
}

inline static void _Hw_Set_SmartPcr(UINT8 u1Id, UINT8 ucDmxPcrId)
{
    if (u1Id == 0)
    {
        // select pin mux
        
        IO_B2R_WRITE_REG(B2R_HD_PCR_ERR, 0);
        IO_B2R_WRITE_BITS(B2R_MISC, HD_SM_PCR_EN, 1);
    }
    else
    {
        // select pin mux

        IO_B2R_WRITE_REG(B2R_SD_PCR_ERR, 0);
        IO_B2R_WRITE_BITS(B2R_MISC, SD_SM_PCR_EN, 1);
    }

    UNUSED(ucDmxPcrId);
}

inline static void _Hw_Set_OMux(UINT8 u1Id, UINT8 ucPath, BOOL fgScartOut)
{
#ifdef __NPTV_HW_IF
    UINT8 OMuxMapTbl[HW_NS]=
    {
        9,  /* mpeghd */
        8   /* mpegsd */
    };

    // select pin mux
    LOG(3,"_New_Hw_Set_OMux u1Id=%d,ucPath=%d,fgScartOut=%d\n",u1Id,ucPath, fgScartOut);
    if(u1Id==0)
    {
       IO_B2R_WRITE_BITS_COM(B2R_SRC_SEL,B2R_SUB_PATH_SEL,0x2);
    }
    else
    {
       IO_B2R_WRITE_BITS_COM(B2R_SRC_SEL, B2R_MAIN_PATH_SEL, 0x01);
    }

    IO_B2R_WRITE_BITS_COM(B2R_SRC_SEL, DDDS_B2R_SRC_SEL, 0x01);
    
    if (ucPath == 0)
    {
        vIO32WriteFldAlign(OMUX_00,OMuxMapTbl[u1Id],OMUX_MAIN_SEL);
        if (fgScartOut)
        {
            vIO32WriteFldAlign(OMUX_00,OMuxMapTbl[u1Id],OMUX_PIP_SEL);
        }
    }
    else
    {
        vIO32WriteFldAlign(OMUX_00,OMuxMapTbl[u1Id],OMUX_PIP_SEL);
    }

    if (u1Id == 0)
    {
        vIO32WriteFldAlign(MPEG2_MIR_00,1,MPEG1_444);
    }
    else
    {
        vIO32WriteFldAlign(MPEG1_MIR_00,1,MPEG1_444);
    }

    
#else
    UNUSED(u1Id);
    UNUSED(ucPath);
#endif

}

#if defined(CC_MT5890)
inline static void _Hw_Set_OMux_OryxE3(UINT8 u1Id, UINT8 ucPath, BOOL fgScartOut)
{
#ifdef __NPTV_HW_IF
    UINT8 OMuxMapTbl[HW_NS]=
    {
        8,  /* mpeghd */
        9   /* mpegsd */
    };

    // select pin mux
    LOG(3,"_New_Hw_Set_OMux u1Id=%d,ucPath=%d,fgScartOut=%d\n",u1Id,ucPath, fgScartOut);
    
    if(ucPath == 0)
    {
        if(u1Id == 0)
        {
            
            vIO32WriteFldAlign(B2R_DATA_MUX , 3, MAIN_PATH_SEL);
        }
        else if(u1Id == 1)
        {
            vIO32WriteFldAlign(B2R_DATA_MUX , 2, MAIN_PATH_SEL);
        }
    }
    else if(ucPath == 1)
    {
        if(u1Id==0)
        {
            vIO32WriteFldAlign(B2R_DATA_MUX , 3, SUB_PATH_SEL);
        }
        else if(u1Id==1)
        {
            vIO32WriteFldAlign(B2R_DATA_MUX , 2, SUB_PATH_SEL);
        }
    }
    
    if(u1Id == 0)
    {
        vIO32WriteFldAlign(B2R_DATA_MUX , 1, NHD_CLK_SEL);
    }
    else if(u1Id == 1)
    {
        vIO32WriteFldAlign(B2R_DATA_MUX , 0, NSD_CLK_SEL);
    }

    vIO32WriteFldAlign(B2R_DATA_MUX , 1, PATH_RST);
    
    IO_B2R_WRITE_BITS_COM(B2R_SRC_SEL, DDDS_B2R_SRC_SEL, 0x01);
    
    if (ucPath == 0)
    {
        vIO32WriteFldAlign(OMUX_00,OMuxMapTbl[ucPath],OMUX_MAIN_SEL);
        if (fgScartOut)
        {
            vIO32WriteFldAlign(OMUX_00,OMuxMapTbl[ucPath],OMUX_PIP_SEL);
        }
    }
    else
    {
        vIO32WriteFldAlign(OMUX_00,OMuxMapTbl[ucPath],OMUX_PIP_SEL);
    }

    if (ucPath == 0)
    {
        vIO32WriteFldAlign(MPEG1_MIR_00,1,MPEG1_444);
    }
    else
    {
        vIO32WriteFldAlign(MPEG2_MIR_00,1,MPEG1_444);
    }

    
#else
    UNUSED(u1Id);
    UNUSED(ucPath);
#endif

}
#endif

inline static void _Hw_Set_LRView(UINT8 u1Id, BOOL fgLeft)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);

    if(fgLeft)
    {
        IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, DISP_R_VIEW, 0);
    }
    else
    {
        IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, DISP_R_VIEW, 1);
    }
}

inline static void _Hw_Set_TB(UINT8 u1Id, BOOL fgTop)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);

    if(fgTop)
    {
        IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, FIELD_SEL, 0);
    }
    else
    {
        IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, FIELD_SEL, 1);
    }
    IO_B2R_WRITE_BITS(B2R_TRIGGER + u4Offset, SET_FIELD, 1);
}

inline static void _Hw_Set_VUpPorch(UINT8 u1Id, UINT32 u4Vporch)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);

    IO_B2R_WRITE_BITS(B2R_TP_COLOR + u4Offset, V_UP_PORCH, u4Vporch);
}

static INT32 _Hw_Oryx_Specific_Setting(B2R_HAL_OBJECT_T *this)
{
    UINT8 id;
    BOOL  fgUfoRst = TRUE;
    INT32 i4Ret = B2R_HAL_OK;

    do
    {
        if(!this)
        {
            i4Ret = B2R_HAL_INV_ARG;
            break;
        }

        id = this->u1Id;

        //UFO Reset
        _Hw_Set_Flip(id, fgUfoRst);
        if(this->tTiming.u4SrcW > SD_MAX_WIDTH)
        {
            //4Kmode will use SD Line buffer
            _Hw_Set_Flip(SD_HW_ID,fgUfoRst);
        }
    }while(0);

    return i4Ret;
}

static INT32 _Hw_Capri_Specific_Setting(B2R_HAL_OBJECT_T *this,
                                             B2R_IC_SPEC_T     *pt_ic_spec)
{
    UINT8 id;
    INT32 i4Ret = B2R_HAL_OK;
    UINT32 u4Vactive = 0;
    UINT32 u4_offset = 0;

    do
    {
        if(!this || !pt_ic_spec)
        {
            i4Ret = B2R_HAL_INV_ARG;
            break;
        }

        id = this->u1Id;
        u4Vactive = pt_ic_spec->u4Vactive;
        if(this->fgFlip &&
            !ALIGN_WITH32(u4Vactive))
        {
            u4_offset = u4Vactive % VALIGN_SIZE;
            u4_offset = VALIGN_SIZE - u4_offset;
            u4Vactive += u4_offset;
            pt_ic_spec->u4Vactive = u4Vactive;
            _Hw_Set_VUpPorch(id, u4_offset);
        }
        else
        {
            _Hw_Set_VUpPorch(id, u4_offset);
        }

        //mirror and flip
        _Hw_Set_Flip(id, this->fgFlip);
        _Hw_Set_Mirror(id, this->fgMirror);
    }while(0);

    return i4Ret;
}

static INT32 _Hw_IC_Specific_Setting(B2R_HAL_OBJECT_T *this,
                                          B2R_IC_SPEC_T    *pt_ic_spec)
{
    INT32 i4Ret = B2R_HAL_OK;
    
    do
    {
        if(!this || !pt_ic_spec)
        {
            i4Ret = B2R_HAL_INV_ARG;
            break;
        }

        if(IS_IC_5890())
        {
            i4Ret = _Hw_Oryx_Specific_Setting(this);
            break;
        }

        //For Capri Setting
        //if(IS_IC_5882())
        {
            i4Ret = _Hw_Capri_Specific_Setting(this, pt_ic_spec);
            break;
        }
    }while(0);

    return i4Ret;
}

static void _Hw_Dump_reg(UINT8 u1Id)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    UINT32 i;
    UINT32 u4Reg1, u4Reg2, u4Reg3, u4Reg4;
    #define B2R_Path_Reg_End (0x007c)
    LOG(5,"\nBase reg: 0x%08x", NBLK2RS_BASE);

    for (i = B2R_YS + u4Offset; i < (B2R_Path_Reg_End + u4Offset); i += 16)
    {
        u4Reg1 = IO_B2R_READ_REG(i);
        u4Reg2 = IO_B2R_READ_REG(i+4);
        u4Reg3 = IO_B2R_READ_REG(i+8);
        u4Reg4 = IO_B2R_READ_REG(i+12);
        LOG(5,"\n0x%08x | 0x%08x 0x%08x 0x%08x 0x%08x", i, u4Reg1, u4Reg2, u4Reg3, u4Reg4);
    }
     for (i = B2R_YS + 0x100; i < (B2R_Path_Reg_End + 0x100); i += 16)
    {
        u4Reg1 = IO_B2R_READ_REG(i);
        u4Reg2 = IO_B2R_READ_REG(i+4);
        u4Reg3 = IO_B2R_READ_REG(i+8);
        u4Reg4 = IO_B2R_READ_REG(i+12);
        LOG(5,"\n0x%08x | 0x%08x 0x%08x 0x%08x 0x%08x", i, u4Reg1, u4Reg2, u4Reg3, u4Reg4);
    }
    for (i = B2R_MISC; i < B2R_SD_IDEAL_CK; i +=16)
    {
        u4Reg1 = IO_B2R_READ_REG(i);
        u4Reg2 = IO_B2R_READ_REG(i+4);
        u4Reg3 = IO_B2R_READ_REG(i+8);
        u4Reg4 = IO_B2R_READ_REG(i+12);
        LOG(5,"\n0x%08x | 0x%08x 0x%08x 0x%08x 0x%08x", i, u4Reg1, u4Reg2, u4Reg3, u4Reg4);
    }

    LOG(5,"\n");
}

static UINT32 _B2R_HW_HdeRatio(B2R_HAL_OBJECT_T *this,
                                      UINT32           *pu4Htotal,
                                      UINT32           *pu4Vtotal)
{
    UINT8  id;
    UINT32 u4Ret = B2R_HAL_OK;
    UINT32 u4Val1 = 0;
    UINT32 u4Val2 = 0;
    UINT32 u4Val3, u4Val4;
    UINT64 u8Tmp;
    
    do 
    {
        if(!this ||
            !pu4Htotal ||
            !pu4Vtotal)
        {
            u4Ret = B2R_MK_RET(B2R_HAL_INV_ARG);
            break;
        }

        id = this->u1Id;
        
        u4Val1 = this->tTiming.u4VTotal;
        u4Val2 = this->tTiming.u4HTotal;
        //u4Val3 = (u4Val1 * u4Val2 * this->tTiming.u4Rate/this->tTiming.u4Scale); overflow
        u8Tmp  = u4Val1 * u4Val2;
        u8Tmp  = u8Tmp * this->tTiming.u4Rate;
        u8Tmp  = this->t3D.fg3D ? (u8Tmp<<1): u8Tmp;
        u8Tmp  = u8Div6432(u8Tmp, this->tTiming.u4Scale, NULL);
        u4Val3 = (UINT32)u8Tmp;
        u4Val3 = ((u4Val3 + 500)/1000)*1000; //round to 1Khz
            
        if (this->tTiming.fgProg)
        {
            u4Val1 -= 1;
        }
        else
        {
            u4Val1 >>= 1;
        }

        u4Val4 = __gcd(this->tTiming.u4Clock, u4Val3);
        u4Val3 = u4Val3/u4Val4;
        u4Val4 = this->tTiming.u4Clock /u4Val4; 
        
        u4Val2 = (u4Val2 * u4Val4 / u4Val3) - 1;
        u4Val4 = (1024 * u4Val3 / u4Val4)%1024;

        if(u4Val2/this->tTiming.u4SrcW > 2 &&
            u4Val4 > HDE_RATIO_MIN)
        {
            u4Val4 = HDE_RATIO_MIN;
        }
        
        _Hw_Set_HdeRatio(id, u4Val4);

        *pu4Htotal = u4Val2;
        *pu4Vtotal = u4Val1;
    }while(0);

    B2R_HAL_TRACE("%s, %s, code=0x%08x\n", __FUNCTION__, B2R_RET(u4Ret) == B2R_HAL_OK?"ok":"fail", u4Ret);
    
    return B2R_RET(u4Ret);
}

static INT32 _B2R_HW_PackFormatGen(B2R_HAL_OBJECT_T *this,
                                                UINT32* pu4Val1,
                                                UINT32* pu4Val2)
{
    INT32 i4Ret = B2R_HAL_OK;
    UINT32 u4Val1, u4Val2;
    
    do
    {
        if(!this || !pu4Val1 || !pu4Val2)
        {
            i4Ret = B2R_HAL_INV_ARG;
            break;
        }
            
        //Pack Mode &  UFO
        // b:110  -> 10bit compact mode
        // b:101  -> 10bit reorder mode
        // b:100  -> 10bit 16bit pack

        // b:000  -> 8bit normal pack
        // b:010 or 001  -> 8bit encode pack
        //u4Val1 = (this->tDramLayout.ePackMode == B2R_PM_COMPACT) ? 0x2 : ((this->tDramLayout.ePackMode == B2R_PM_16bit) ? 0x01 : 0x0);
        switch(this->tDramLayout.ePackMode)
        {
        case B2R_PM_COMPACT:
            u4Val1 = 0x2;
            break;
        case B2R_PM_COMPACT_TILE:
        case B2R_PM_16bit:
            u4Val1 = 0x1;
            break;
        default:
            u4Val1 = 0;
            break;
        }
        u4Val2 = 0;
        if (this->tColorFmt.u1Depth != 8)
        {
            u4Val1 = u4Val1 | 0x4;
            u4Val2 = 0x1;
        }
    }while(0);

    if(pu4Val1)
    {
        *pu4Val1 = u4Val1;
    }

    if(pu4Val2)
    {
        *pu4Val2 = u4Val2;
    }

    return i4Ret;
}

static UINT32 _B2R_HW_SetIntDelay(B2R_HAL_OBJECT_T *this)
{
    UINT8  id = 0;
    UINT32 u4Ret = B2R_HAL_OK;
    UINT32 u4Val = 0;
    INT32 i4BlankLine = 0;
    
    do 
    {
        if (!this)
        {
            u4Ret = B2R_MK_RET(B2R_HAL_INV_ARG);
            break;
        }

        id = this->u1Id;
        
        i4BlankLine = this->tTiming.u4VTotal - this->tTiming.u4SrcH; 
        if (i4BlankLine < 0)
        {
            i4BlankLine = 0;
        }

#ifdef B2R_EMU
        i4BlankLine = 0;
#endif

        // Note: Line Counter of 1st Active Line is 4
        u4Val = DIVIDE_WITH_CARRIER((i4BlankLine + 4), 8);
        u4Val = this->tTiming.fgProg ? u4Val : (u4Val>>1);

        _Hw_Set_InterruptDelay(id, u4Val);
    }while(0);

    B2R_HAL_TRACE("%s, %s, code=0x%08x\n", __FUNCTION__, B2R_RET(u4Ret) == B2R_HAL_OK?"ok":"fail", u4Ret);
    
    return B2R_RET(u4Ret);
}

static UINT32 _B2R_HW_Reset(B2R_HAL_OBJECT_T *this)
{

    UINT32 u4Ret = B2R_HAL_OK;

    do {

        if (NULL == this)
        {
            u4Ret = B2R_MK_RET(B2R_HAL_INV_ARG);
            break;
        }

        _Hw_Reset(this->u1Id);
    
    }while(0);

    B2R_HAL_TRACE("%s, %s, code=0x%08x\n", __FUNCTION__, B2R_RET(u4Ret) == B2R_HAL_OK?"ok":"fail", u4Ret);
    
    return B2R_RET(u4Ret);
}

static UINT32 _B2R_HW_Enable(B2R_HAL_OBJECT_T *this)
{

    UINT32 u4Ret = B2R_HAL_OK;

    do {

        if (NULL == this)
        {
            u4Ret = B2R_MK_RET(B2R_HAL_INV_ARG);
            break;
        }
        if(this->fgVB1Enable)
        {
          LOG(1,"_B2R_HW_Enable by video path=%d\n",this->fgVB1Enable);
            break;
        }
        //for debug
       
         IO_B2R_WRITE_BITS(B2R_INT_CLR , YC_CRC_EN, 1);
         _Hw_Enable(this->u1Id);
        _Hw_Dump_reg(this->u1Id);
#ifdef VDP_B2R_PSCAN_CRC_FLOW
        IO_B2R_WRITE_BITS(B2R_INT_CLR , YC_CRC_EN, 1);
        IO_B2R_WRITE_BITS(B2R_INT_CLR , DRAM_CHKSUM_EN, 1);
#endif
    
    }while(0);

    B2R_HAL_TRACE("%s, %s, code=0x%08x\n", __FUNCTION__, B2R_RET(u4Ret) == B2R_HAL_OK?"ok":"fail", u4Ret);
    
    return B2R_RET(u4Ret);
}

static UINT32 _B2R_HW_Disable(B2R_HAL_OBJECT_T *this)
{

    UINT32 u4Ret = B2R_HAL_OK;

    do {

        if (NULL == this)
        {
            u4Ret = B2R_MK_RET(B2R_HAL_INV_ARG);
            break;
        }
        if(this->fgVB1Enable)
        {
          LOG(1,"_B2R_HW_Disable by video path=%d\n",this->fgVB1Enable);
            break;
        }
        _Hw_Disable(this->u1Id);
    
    }while(0);

    B2R_HAL_TRACE("%s, %s, code=0x%08x\n", __FUNCTION__, B2R_RET(u4Ret) == B2R_HAL_OK?"ok":"fail", u4Ret);
    
    return B2R_RET(u4Ret);
}

UINT32 _B2R_VB1_Setting( UINT32 u4Pitch,B2R_PACK_MODE_T ePackMode,UINT8 u1Depth,UINT32 u4SrcW,UINT32 u4SrcH,UINT32 u4Y0,UINT32 u4C0,UINT32 u4YLen,UINT32 u4CLen)
{
    UINT32 u4Val1, u4Val2;
    if ((u4Pitch >> 4) <= 0xFF)
    {
        IO_B2R_WRITE_BITS(B2R_LINE_OFFSET , LINE_OFFSET, (u4Pitch >> 4));
        IO_B2R_WRITE_BITS(B2R_LINE_OFFSET , LINE_OFFSET_H, 0);
    }
    else
    {
        u4Pitch = ((u4Pitch >> 4) & 0x000000FF);
        IO_B2R_WRITE_BITS(B2R_LINE_OFFSET , LINE_OFFSET, u4Pitch);
        IO_B2R_WRITE_BITS(B2R_LINE_OFFSET , LINE_OFFSET_H, 1);
    }
     switch(ePackMode)
     {
        case B2R_PM_COMPACT:
            u4Val1 = 0x2;
            break;
        case B2R_PM_COMPACT_TILE:
        case B2R_PM_16bit:
            u4Val1 = 0x1;
            break;
        default:
            u4Val1 = 0;
            break;
     }
        u4Val2 = 0;
        if (u1Depth != 8)
        {
            u4Val1 = u4Val1 | 0x4;
            u4Val2 = 0x1;
        }
        Printf("u4Val1=%x,u4Val2=%x,ePackMode=%x\n",u4Val1,u4Val2,ePackMode);
        _Hw_Set_PackFormat(0, u4Val1, TRUE);
        _Hw_Set_UFO(0, u4Val2);
        _Hw_Set_4KMode(0, (u4SrcW > 1920));
        u4Val1 = u4SrcW >> 1;
        u4Val2 =u4SrcH ;
        _Hw_Set_ActiveSize(0, u4Val1/2, 2160);//VB1 hde>>2 lijia VB1
        _Hw_Set_YC_Start(0, u4Y0,u4C0);
        if(u4YLen!=0&&u4CLen!=0)
        {
            _Hw_Set_YC_Len(0, u4YLen,u4CLen);
        }
         IO_B2R_WRITE_BITS(B2R_MODE_CTRL , FLIP_EN, 1);
         IO_B2R_WRITE_BITS(B2R_MODE_CTRL+ 0x100, FLIP_EN, 1);
       // _Hw_Dump_reg(0);
       
    return B2R_HAL_OK;
}

#if defined(CC_MT5890)
inline static void _HW_Set_DeCtrl(UINT8 u1Id, UINT32 u4DeStart, UINT32 u4DeEnd)
{
    UINT32 u4Offset = B2R_REG_OFFSET(u1Id);
    
    IO_B2R_WRITE_BITS(B2R_DE_CTRL + u4Offset, DE_START, u4DeStart);
    IO_B2R_WRITE_BITS(B2R_DE_CTRL + u4Offset, DE_END, u4DeEnd);
}
#endif

static UINT32 _B2R_HW_Config(B2R_HAL_OBJECT_T *this)
{

    UINT32 u4Ret = B2R_HAL_OK;
    UINT8 id;
    UINT32 u4Val1, u4Val2;
    B2R_IC_SPEC_T t_ic_spec;

    do {
    
        if (NULL == this)
        {
            u4Ret = B2R_MK_RET(B2R_HAL_INV_ARG);
            break;
        }

        id = this->u1Id;

        //DRAM_LAYOUT
        //B2R_HAL_DRAM_LAYOUT_T *pt = &(this->tDramLayout);
        u4Val1 = 0x1; // force 16x32 (pt->eAddrMode == B2R_AM_BLOCK1 ? 0x1 : (pt->eAddrMode == B2R_AM_BLOCK4 ? 0x0 : 0x3)); 
        u4Val2 = 0x2; // force no swap (pt->eSwapMode == B2R_SM_NO_SWAP? 0x2 : (pt->eSwapMode == B2R_SM_SWAP1 ? 0x5 : 0x6));
        _Hw_Set_DramFormat(id, u4Val1, u4Val2, this->tDramLayout.u4Pitch);

        //COLOR Format
        u4Val1 = (this->tColorFmt.eMode == B2R_CM_420 ? 0x0 : 0x1); 
        _Hw_Set_ColorFormat(id, u4Val1, 0);

        _B2R_HW_PackFormatGen(this, &u4Val1, &u4Val2);

        if(!this->fgVB1Enable)
        {
            _Hw_Set_PackFormat(id, u4Val1, this->tColorFmt.fgUfoEn);
            _Hw_Set_UFO(id, u4Val2);

        //B2R_HAL_TRACE("Pack : 0x%x, UFO: 0x%x\n", u4Val1, u4Val2);
        //4K mode
            _Hw_Set_4KMode(id, (this->tTiming.u4SrcW > 1920));

        //3d part
           _Hw_Set_3D(id, &this->t3D, this->tTiming.fgProg);
 
        //timing part
        //@interlace or not
           _Hw_Set_Interlaced(id, (this->tTiming.fgProg == 1)? 0:1);

        //@clock
           _Hw_Set_Clock(id, &(this->tTiming.u4Clock));//VB1 clk setting is no here

        //HDE Ratio
           _B2R_HW_HdeRatio(this, &u4Val2, &u4Val1);

        //@v & h total
          _Hw_Set_TotalSize(id, u4Val2-1, u4Val1);
       
        //@h & v active
            t_ic_spec.u4Hactive = this->tTiming.u4SrcW >> 1;
            t_ic_spec.u4Vactive = this->tTiming.fgProg ? this->tTiming.u4SrcH : (this->tTiming.u4SrcH >> 1);

           _Hw_IC_Specific_Setting(this, &t_ic_spec);

           _Hw_Set_ActiveSize(id, t_ic_spec.u4Hactive, t_ic_spec.u4Vactive);
        
        // @Set Interrupt Delay according to Output Format
            _B2R_HW_SetIntDelay(this);
        
        //dram req 16 or 32
           u4Val1 = (this->tTiming.u4VTotal > 750)? 1 : 3;
           _Hw_Set_DramReq(id, u4Val1);
           }
        else
        {
            Printf("===========VB1 SETTING==============\n");
            LOG(3,"this->tDramLayout.ePackMode=%d,u4Val1=%x,u4Val2=%x\n",this->tDramLayout.ePackMode,u4Val1,u4Val2);
            _Hw_Set_PackFormat(id, u4Val1, this->tColorFmt.fgUfoEn);
            _Hw_Set_UFO(id, u4Val2);
            u4Val1 = this->tTiming.u4VTotal;
            u4Val2 = this->tTiming.u4HTotal;
       

             if((u4Val2==2200)&&(u4Val1==2250))
            {
               IO_B2R_WRITE_BITS(B2R_MODE_CTRL , FLIP_EN, 1);
               IO_B2R_WRITE_BITS(B2R_MODE_CTRL+ 0x100, FLIP_EN, 1);
            }
            if (this->tTiming.fgProg)
            {
                u4Val1 -= 1;
            }
            else
            {
                u4Val1 >>= 1;
                //u4Val3 >>= 1;
            }

#if defined(CC_MT5890)
            if(IS_IC_5890_ES3())
            {
                _HW_Set_DeCtrl(id, 27, 1947);
            }
#endif
            _Hw_Set_TotalSize(id, u4Val2-1, u4Val1);
            _Hw_Set_HdeRatio(id, 0);// vb1 TEST
           

            //@h & v active
            u4Val1 = this->tTiming.u4SrcW >> 1;
            u4Val2 = this->tTiming.fgProg ? this->tTiming.u4SrcH : (this->tTiming.u4SrcH >> 1);
            _Hw_Set_ActiveSize(id, u4Val1/2, 2160);//VB1 hde>>2 lijia VB1
             
            _Hw_Set_Interlaced(id, (this->tTiming.fgProg == 1)? 0:1);
            _Hw_Set_4KMode(id, (this->tTiming.u4SrcW > 1920));
             u4Val1 = DIVIDE_WITH_CARRIER(4, 8);
             u4Val1 = this->tTiming.fgProg ? u4Val1 : (u4Val1>>1);
             _Hw_Set_InterruptDelay(id, u4Val1);

             u4Val1 = (this->tTiming.u4VTotal > 750)? 1 : 3;
             _Hw_Set_DramReq(id, u4Val1);
            //Enable B2R hw
             IO_B2R_WRITE_BITS(B2R_INT_CLR , YC_CRC_EN, 1);
#ifdef CC_SUPPORT_UHD
             IO_B2R_WRITE_BITS(B2R_LINE_OFFSET, FIFO_SEL, 1);
#endif
          //  _Hw_Enable(this->u1Id);
            _Hw_Dump_reg(this->u1Id);
        }
        
    }while(0);

    B2R_HAL_TRACE("%s, %s, code=0x%08x\n", __FUNCTION__, B2R_RET(u4Ret) == B2R_HAL_OK?"ok":"fail", u4Ret);
    
    return B2R_RET(u4Ret);
}


static UINT32 _B2R_HW_Set(B2R_HAL_OBJECT_T *this, B2R_HAL_TYPE_T eType, VOID *pvArg)
{

    UINT32 u4Ret = B2R_HAL_OK;
    UINT8 id;

    do {

        if (NULL == this || NULL == pvArg)
        {
            u4Ret = B2R_MK_RET(B2R_HAL_INV_ARG);
            break;
        }
        
        id = this->u1Id;

        switch(eType)
        {
        case B2R_HAL_ADDR:
        {
            B2R_HAL_ADDR_T *ptArg = (B2R_HAL_ADDR_T *)pvArg;
            _Hw_Set_YC_Start(id, ptArg->u4Y0, ptArg->u4C0);
            if (this->t3D.u4Layout == B2R_3D_FRAME_SEQUENTIAL)
            {
                _Hw_Set_YC_Start_R(id, ptArg->u4Y1, ptArg->u4C1);
            }
            break;
        }

        case B2R_HAL_ADDR_LEN:
        {
            B2R_HAL_ADDR_LEN_T *ptArg = (B2R_HAL_ADDR_LEN_T *)pvArg;
            _Hw_Set_YC_Len(id, ptArg->u4YLen0, ptArg->u4CLen0);
            if (this->t3D.u4Layout == B2R_3D_FRAME_SEQUENTIAL)
            {
                _Hw_Set_YC_Len_R(id, ptArg->u4YLen1, ptArg->u4CLen1);
            }
            break;
        }
        case B2R_HAL_FRC:
        {
            B2R_HAL_FRC_ARG_T *ptArg = (B2R_HAL_FRC_ARG_T *)pvArg;
            _B2R_HAL_SetFrc(this, ptArg->u2TargetNs, ptArg->u2ChangeFieldNs, ptArg->ucMode);
            _B2R_HW_Set_Frc(this, ptArg->u2TargetNs, ptArg->u2ChangeFieldNs, ptArg->ucMode);
            break;
        }
        case B2R_HAL_LR_STATE:
        {
            BOOL fgLeft = *(BOOL *)pvArg;
            _Hw_Set_LRView(id, fgLeft);
        }
            break;
        case B2R_HAL_TB_STATE:
        {
            BOOL fgTop = *(BOOL *)pvArg;
            _Hw_Set_TB(id, fgTop);
        }
            break;

        case B2R_HAL_MIRROR_FLIP:
        {
#if defined(CC_MT5882)
            B2R_HAL_MF_T *ptArg = (B2R_HAL_MF_T *)pvArg;
            this->fgFlip   = ptArg->fgFlip;
            this->fgMirror = ptArg->fgMirror;
            _Hw_Set_Mirror(id, this->fgMirror);
            _Hw_Set_Flip(id, this->fgFlip);
#endif
        }
            break;
        case B2R_HAL_VB1:
        {
             BOOL ucBypass = *(BOOL *)pvArg;
            if(ucBypass)
            {
                this->fgVB1Enable = TRUE;
                LOG(1,"this->fgVB1Enable=%d\n",this->fgVB1Enable);
            }
            else
            {
                this->fgVB1Enable = FALSE;
                LOG(1,"this->fgVB1Disable=%d\n",this->fgVB1Enable);
            }
            break;
        }
        case B2R_HAL_OMUX:
        {
            B2R_HAL_OMUX_T *ptArg = (B2R_HAL_OMUX_T *)pvArg;
            if (ptArg->ucPath >= PATH_NS) 
            {
                u4Ret = B2R_MK_RET(B2R_HAL_INV_ARG);
                break;
            }
            
#if defined(CC_MT5890)
            if(IS_IC_5890_ES3())
            {
                _Hw_Set_OMux_OryxE3(id, ptArg->ucPath, ptArg->fgScartOut);
            }
            else
#endif
            {
                _Hw_Set_OMux(id, ptArg->ucPath, ptArg->fgScartOut);
            }
        }
            break;
        case B2R_HAL_SMART_PCR:
        {
            UCHAR ucPcrId = *(UCHAR *)pvArg;
            if (ucPcrId >= DMX_PCR_NS) 
            {
                u4Ret = B2R_MK_RET(B2R_HAL_INV_ARG);
                break;
            }
            this->ucDmxPcrId = ucPcrId;
            _Hw_Set_SmartPcr(id, ucPcrId);
        }
            break;
        case B2R_HAL_PATTERN:
        {
            B2R_HAL_TP_T *ptArg = (B2R_HAL_TP_T *)pvArg;
            _Hw_Set_Pattern(id, ptArg);
        }
            break;
        case B2R_HAL_PITCH:
        {
            UINT32 u4Pitch = *(UINT32 *)pvArg;
            _Hw_Set_Pitch(id, u4Pitch);
        }
            break;
        case B2R_HAL_SET_RESOLUTION:
        {
            BOOL fgSet = *(BOOL *)pvArg;
            _Hw_Set_Resolution(id, fgSet);
        }
            break;

        case B2R_HAL_COLOR_FMT_CHG:
        {
            UINT32 u4Val1,u4Val2;
            BOOL fgUfo = *(BOOL *)pvArg;
            
            _B2R_HW_PackFormatGen(this, &u4Val1, &u4Val2);
            _Hw_Set_PackFormat(id, u4Val1, fgUfo);
            break;
        }

        default:
            u4Ret = B2R_MK_RET(B2R_HAL_NOT_IMPL);
            break;    
        }
    
    }while(0);

    B2R_HAL_TRACE("%s, %s, arg(%d), code=0x%08x\n", __FUNCTION__, B2R_RET(u4Ret) == B2R_HAL_OK?"ok":"fail", eType, u4Ret);
    
    return B2R_RET(u4Ret);
}

static UINT32 _B2R_HW_Get(B2R_HAL_OBJECT_T *this, B2R_HAL_GET_TYPE_T eType, VOID *pvArg)
{

    UINT32 u4Ret = B2R_HAL_OK;
    UINT8 id;

    do {

        if (NULL == this || NULL == pvArg)
        {
            u4Ret = B2R_MK_RET(B2R_HAL_INV_ARG);
            break;
        }

        id = this->u1Id;

        switch(eType)
        {
        case B2R_HAL_STATUS:
        {
            B2R_HAL_STATUS_T *ptArg = (B2R_HAL_STATUS_T *)pvArg;
            _B2R_HW_Read_Status(id, ptArg);
        }
            break;
        case B2R_HAL_DISP_INFO:
        {
            B2R_HAL_DISP_INFO_T *ptArg = (B2R_HAL_DISP_INFO_T *)pvArg;
            ptArg->eAddrMode = this->tDramLayout.eAddrMode;
            ptArg->u4Pitch   = this->tDramLayout.u4Pitch;
            ptArg->u4With    = this->tTiming.u4SrcW;
            ptArg->u4Height  = this->tTiming.u4SrcH;
            _Hw_Get_YC_Start(id, &ptArg->u4YAddr, &ptArg->u4CAddr);
        }
            break;
        default:
            u4Ret = B2R_MK_RET(B2R_HAL_NOT_IMPL);
            break;    
        }
    
    }while(0);

    B2R_HAL_TRACE("%s, %s, arg(%d), code=0x%08x\n", __FUNCTION__, B2R_RET(u4Ret) == B2R_HAL_OK?"ok":"fail", eType, u4Ret);
    
    return B2R_RET(u4Ret);
}

static UINT32 _B2R_HW_Is(B2R_HAL_OBJECT_T *this, B2R_HAL_IS_TYPE_T eType, VOID *pvArg)
{

    UINT32 u4Ret = B2R_HAL_OK;
    UINT8 id;

    do {

        if (NULL == this || NULL == pvArg)
        {
            u4Ret = B2R_MK_RET(B2R_HAL_INV_ARG);
            break;
        }

        id = this->u1Id;

        switch(eType)
        {
        default:
            u4Ret = B2R_MK_RET(B2R_HAL_NOT_IMPL);
            break;    
        }
    
    }while(0);

    B2R_HAL_TRACE("%s, %s, arg(%d), code=0x%08x\n", __FUNCTION__, B2R_RET(u4Ret) == B2R_HAL_OK?"ok":"fail", eType, u4Ret);
    
    return B2R_RET(u4Ret);
}



static BOOL _B2R_HW_IsSupport(B2R_HAL_OBJECT_T *this, VOID *pvArg)
{
    UINT32 u4Ret = B2R_HAL_OK;

    do {

        if (NULL == this || NULL == pvArg)
        {
            u4Ret = B2R_MK_RET(B2R_HAL_INV_ARG);
            break;
        }

        //todo 
    
    }while(0);

    B2R_HAL_TRACE("%s, %s, arg(%d), code=0x%08x\n", __FUNCTION__, u4Ret == B2R_HAL_OK?"ok":"fail", u4Ret);
    
    return (B2R_RET(u4Ret) == B2R_HAL_OK) ? TRUE : FALSE;

}

static void _B2R_HW_Read_Status(UINT8 u1Id, B2R_HAL_STATUS_T * ptStatus)
{
    UINT32 u4Line1;
    UINT32 u4Line2;
    UINT32 u4Offset;
    B2R_HAL_OBJECT_T *this;

    if (!ptStatus)
    {
        return;
    }
    this = &_b2r_hw_v2_object[u1Id];
    if(!this)
    {
        return;
    }

    u4Offset = B2R_REG_OFFSET(u1Id);

    do
    {
        // Line Counter Register does not handle cross-clock domain
        // Read twice to assure stable
        u4Line1 = IO_B2R_READ_BITS(B2R_STA + u4Offset, VCNT);
        u4Line2 = IO_B2R_READ_BITS(B2R_STA + u4Offset, VCNT);
    } while (u4Line1 != u4Line2);

    u4Line1 = IO_B2R_READ_REG(B2R_STA + u4Offset);

    ptStatus->u4LineCnt = u4Line2;
    ptStatus->fgTop     = ((u4Line1 & 0x40) == 0) ? FALSE :TRUE ;
    ptStatus->fgLeft    = ((u4Line1 & 0x800) == 0)? TRUE :FALSE ; 

    ptStatus->u4CurrentField = (~u4Line1) & 0x00000040; 
    ptStatus->u4CurrentLR    = u4Line1 & 0x00000800; 
    ptStatus->u4UnderFlow    = u4Line1 & 0x00000008;
    ptStatus->u4Status       = u4Line1 & 0x00000380;
    ptStatus->u4CurrentOutputField  = u4Line1 & 0x00004000;
    ptStatus->u4CrC          = (u1Id == 1) ? (IO_B2R_READ_REG(B2R_SD_PIXEL_CRC)) :( IO_B2R_READ_REG(B2R_HD_PIXEL_CRC));
    ptStatus->u4DramChkSum   = IO_B2R_READ_REG(B2R_DRAM_CHKSUM);
#ifdef VDP_B2R_PSCAN_CRC_FLOW
    if((ptStatus->u4CurrentField != 0 && !this->tTiming.fgProg) ||
        this->tTiming.fgProg)
    {
        ptStatus->u4DramChkSum   = IO_B2R_READ_REG(B2R_DRAM_CHKSUM);
        ptStatus->u4PSCANSrcCrc  = u4DrvDIGetInputCRC(0);
        ptStatus->u4PSCANOutCrc  = u4DrvDIGetOutputCRC(0);
        ptStatus->u4MJCSrcCrc    = IO_MJC_READ_REG(MJC_STA_CRC_1);
        ptStatus->u4MJCOutCrc    = IO_MJC_READ_REG(MJC_STA_CRC_I);
    }
#endif

    if (u4Line1 & B2R_ISR_ADI_MASK)
    {
        ptStatus->eIsrType =  B2R_ISR_ADI;   
    }
    else if (u4Line1 & B2R_ISR_VEI_MASK)
    {
        ptStatus->eIsrType =  B2R_ISR_VEI;   
    }
    else if (u4Line1 & B2R_ISR_FI_MASK)
    {
        ptStatus->eIsrType =  B2R_ISR_FI;   
    }

    ptStatus->u4Status = u4Line1;
}

static UINT32 _B2R_HW_Do_Frc(B2R_HAL_OBJECT_T *this)
{
    B2R_HAL_FRC_T *ptFrc;
    B2R_HAL_STATUS_T * ptStatus;
    UINT32 u4Offset;
    UCHAR ucVdpId;

    if (!this)
    {
        return B2R_HAL_INV_ARG;
    }

    ptFrc    = &this->tFrc;
    ptStatus = &this->tStatus;
    u4Offset = B2R_REG_OFFSET(this->u1Id);
    ucVdpId  = 0; //TODO


    if (ptFrc->ucFrcEn == 0)
    {
        return B2R_HAL_OK;
    }
    
    if (ptFrc->u2TargetNs != 3)
    {
        _Hw_Set_TB(this->u1Id,FALSE);
    }

    //if (_rB2rFrc.ucCurrentNs == 0)
    // DTV00093447 for di has do some change, b2r should send T/P by order, or will freeze
    if (ptFrc->ucCurrentNs == 0)
    {

        if (ptFrc->eRepeatField == B2R_FRC_FIELD_MAX)
        {
            if (ptStatus->u4CurrentOutputField != 0)
            {
                ptFrc->eRepeatField = B2R_FRC_FIELD_B;
            }
            else
            {
                ptFrc->eRepeatField = B2R_FRC_FIELD_T;
            }
        }

        #ifdef VDP_FRC_AUTO_STOP_CTRL
        if(this->t3D.fg3D)
        {
            vDrvDIFrameRepeat(ucVdpId, 0);
        }
        else
        {
            if (( VDP_FRC_TRICK_PLAY == ptFrc->ucMode )
                #if !defined(CC_USE_DDI)
                //todo
                //|| (_B2R_GetStepForward(ucB2rId) == VDP_PLAY_STEP_FORWARD) 
                #endif
               )
            {
                vDrvDIFrameRepeat(ucVdpId, 0);
                vDrvDIB2RFilmModeFreeze(ucVdpId,1);
            }
            else
            {
                vDrvDIFrameRepeat(ucVdpId, 1);
                vDrvDIB2RFilmModeFreeze(ucVdpId,0);
            }
        }
        #endif
        LOG(8,"B2R FRC S(%d) \n", ptFrc->u2TargetNs);
    }
    
    #ifdef VDP_FRC_INV_FIELD_CTRL
    if ((ptFrc->u2TargetNs == 3) && (ptFrc->ucCurrentNs != ptFrc->u2TargetNs))
    {
    }
    #endif // VDP_FRC_INV_FIELD_CTRL 

    if (ptFrc->ucCurrentNs == ptFrc->u2TargetNs)
    {
        IO_B2R_WRITE_BITS(B2R_R_MAP + u4Offset, REPEAT_CNT, 0);
        IO_B2R_WRITE_BITS(B2R_R_MAP + u4Offset, NEW_EVEN_REPEAT, 0);
        IO_B2R_WRITE_BITS(B2R_R_MAP + u4Offset, REPEAT_H_SHIFT, 0);
        IO_B2R_WRITE_BITS(B2R_TRIGGER + u4Offset, SET_REPEAT_CNT, 1);

        IO_B2R_WRITE_BITS(B2R_TRIGGER + u4Offset, SET_FIELD, 0);
        vDrvDIB2RFilmModeFreeze(ucVdpId,1);
    }

    return B2R_HAL_OK;

}

static void _B2R_HW_Set_Frc(B2R_HAL_OBJECT_T *this, UCHAR u2TargetNs, UCHAR u2ChangeFieldNs, UCHAR ucMode)
{
    UINT32 u4Offset;

    if (!this)
    {
        return;
    }

    u4Offset = B2R_REG_OFFSET(this->u1Id);

    if (ucMode == B2R_FRC_NONE)
    {
        // Clean HFI bit
    }
    else if ((ucMode == B2R_FRC_NORMAL) &&
             ((u2TargetNs == 0) || (u2TargetNs == 3) ||
              ((u2TargetNs == 2) && (u2ChangeFieldNs == 1)) ||
              ((u2TargetNs == 4) && (u2ChangeFieldNs == 2))))
    {
        // Normal: Top + Bottom (Single Field)
        // Clean HFI bit
    }
    else if (ucMode == B2R_FRC_CANCEL)
    {
        //clear repeat count toogle bit
        IO_B2R_WRITE_BITS(B2R_R_MAP + u4Offset, REPEAT_CNT, 0);
        IO_B2R_WRITE_BITS(B2R_TRIGGER + u4Offset, SET_REPEAT_CNT, 1);
    }
    else
    {
        _B2R_HW_Do_Frc(this);
    }
 
}



static void _B2R_HW_V2_ISR(UINT8 u1Id, UINT16 u2Vector)
{
    UINT32 u4Offset;
    BOOL fgISR = FALSE;
    B2R_HAL_OBJECT_T *this;

    u4Offset = B2R_REG_OFFSET(u1Id);

    fgISR = (u1Id == 0) ? (IO_B2R_READ_BITS(B2R_INT_CLR, HD_INT_CLR)) : (IO_B2R_READ_BITS(B2R_INT_CLR, SD_INT_CLR));

    if (fgISR == 0)
    {
        return;
    }

    u4Offset = B2R_REG_OFFSET(u1Id);
    this = &_b2r_hw_v2_object[u1Id];

    _B2R_HW_Read_Status(u1Id, &this->tStatus);

    LOG(10, "ISR(%d) crc 0x%08x, cnt(%d), T(%d), L(%d)\n",
        u1Id,
        this->tStatus.u4CrC, this->tStatus.u4LineCnt,
        this->tStatus.fgTop, this->tStatus.fgLeft);
    
    //clear underflow bit
    IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, FIFO_MON_EN, 0);
    IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, FIFO_MON_EN, 1);

    if (this->fgEnable && this->pvHost)
    {
        B2R_HAL_COMM_T * host = (B2R_HAL_COMM_T *)this->pvHost;

        if (this->tStatus.u4Status & B2R_ISR_ADI_MASK)
        {
            // do FRC here
            this->tFrc.ucCurrentNs++;
            _B2R_HW_Do_Frc(this); 
        }
        else if (this->tStatus.u4Status & B2R_ISR_VEI_MASK)
        {
            // VEI is one-shot interrupt
            IO_B2R_WRITE_BITS(B2R_EN + u4Offset, VEI_EN, 0);
        }
        
        if (host->IsrCb)
        {
            host->IsrCb(host->pvData, (VOID *)&this->tStatus);
        }
    }

    if (u1Id == 0)
    {
#ifdef VDP_B2R_PSCAN_CRC_FLOW
        IO_B2R_WRITE_REG(B2R_INT_CLR, 0x16);
#else
        IO_B2R_WRITE_REG(B2R_INT_CLR, 0x14);
#endif
    }
    else
    {
#ifdef VDP_B2R_PSCAN_CRC_FLOW
        IO_B2R_WRITE_REG(B2R_INT_CLR, 0x26);
#else
        IO_B2R_WRITE_REG(B2R_INT_CLR, 0x24);
#endif
    }

    UNUSED(u2Vector);
}


inline static void _B2R_HW_V2_ISR_HD(UINT16 u2Vector)
{
    _B2R_HW_V2_ISR(0, u2Vector);
}

inline static void _B2R_HW_V2_ISR_SD(UINT16 u2Vector)
{
    _B2R_HW_V2_ISR(1, u2Vector);
}


//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------
//#ifdef B2R_EMU
void _B2REmu_HW_V2_Init(UINT8 u1Id)
{
    _Hw_Disable(u1Id);
}

void _B2REmu_HW_V2_ISR(UINT16 u2Vector)
{
    BOOL fgHDISR = FALSE;
    BOOL fgSDISR = FALSE;
    UINT32 u4Offset = B2R_HD_BASE;
    UINT8 u1Id = 0;
    B2R_HAL_OBJECT_T *ptObj = NULL;

    UNUSED(u2Vector);

    fgHDISR = IO_B2R_READ_BITS(B2R_INT_CLR, HD_INT_CLR);
    fgSDISR = IO_B2R_READ_BITS(B2R_INT_CLR, SD_INT_CLR);

    if (fgHDISR == 0 && fgSDISR == 0)
    {
        ASSERT(0);
        return;
    }

    while(fgHDISR || fgSDISR)
    {
        if (fgHDISR)
        {
            u1Id = 0;
        }
        else if (fgSDISR)
        {
            u1Id = 1;
        }

        u4Offset = B2R_REG_OFFSET(u1Id);
        ptObj = &_b2r_hw_v2_object[u1Id];
        _B2R_HW_Read_Status(u1Id, &ptObj->tStatus);

        LOG(10, "ISR(%d) crc 0x%08x, cnt(%d), T(%d), L(%d)\n",
            u1Id,
            ptObj->tStatus.u4CrC, ptObj->tStatus.u4LineCnt,
            ptObj->tStatus.fgTop, ptObj->tStatus.fgLeft);
        
        //clear underflow bit
        IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, FIFO_MON_EN, 0);
        IO_B2R_WRITE_BITS(B2R_MODE_CTRL + u4Offset, FIFO_MON_EN, 1);

        if (ptObj->pvHost)
        {
            B2R_HAL_COMM_T * host = (B2R_HAL_COMM_T *)ptObj->pvHost;
            if (host->IsrCb)
            {
                host->IsrCb(host->pvData, &ptObj->tStatus);
            }
        }

        if (fgHDISR)
        {
            IO_B2R_WRITE_REG(B2R_INT_CLR, 0x14);
            fgHDISR = FALSE;
        }
        else if (fgSDISR)
        {
            IO_B2R_WRITE_REG(B2R_INT_CLR, 0x24);
            fgSDISR = FALSE;
        }

    }

}
//#endif /*B2R_EMU*/

//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------
VOID B2R_HW_V2_Init(VOID)
{
    UINT32 i;
    B2R_HAL_OBJECT_T *ptO = NULL;

    for (i = 0; i < HW_NS; i++)
    {
        ptO =  &_b2r_hw_v2_object[i];
        x_snprintf((char*)(ptO->acName), 12, "B2R-V2-%s", (i == 0? "HD":"SD"));
        ptO->u1Id      = i;
        ptO->u1Used    = FALSE;
        ptO->fgEnable  = FALSE;
        ptO->pvHost    = NULL;

        ptO->Reset     = _B2R_HW_Reset;
        ptO->Enable    = _B2R_HW_Enable;
        ptO->Disable   = _B2R_HW_Disable;
        ptO->Config    = _B2R_HW_Config;
        ptO->Set       = _B2R_HW_Set;
        ptO->Get       = _B2R_HW_Get;
        ptO->Is        = _B2R_HW_Is;
        ptO->IsSupport = _B2R_HW_IsSupport;
        ptO->Isr       = (i == 0) ? _B2R_HW_V2_ISR_HD : _B2R_HW_V2_ISR_SD;

        //init hw
        _Hw_Reset(i);
        _Hw_Disable(i);
    }
}

B2R_HAL_OBJECT_T * B2R_HW_V2_Get_Object(UINT8 u1Idx)
{
    return (u1Idx < sizeof(_b2r_hw_v2_object)/sizeof(_b2r_hw_v2_object[0])) ? &_b2r_hw_v2_object[u1Idx] : NULL;
}




