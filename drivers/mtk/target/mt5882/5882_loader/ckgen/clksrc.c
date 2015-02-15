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
 * $RCSfile: clksrc.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file clksrc.c
 *  This C file implements clock source configuration.
 */

#ifndef CC_MTK_LOADER

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
#include "x_clksrc.h"
#include "hw_vdoin.h"
#include "hw_ckgen.h"
//---------------------------------------------------------------------------
// Configurations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Imported variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static function forward declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------
#ifdef CC_FCI_ENABLE

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------

//-------------------------------------------------------------------------
/** CLKSRC_InitFCIClock
 *  Initial FCI module clock and current driving capability.
 *  @param VOID
 *  @return VOID
 */
//-------------------------------------------------------------------------
VOID CLKSRC_InitFCIClock(VOID)
{
    /* Setup Initial Clock. */
    CLKSRC_ChangeMSPClock(18);
    CLKSRC_ChangeSDCClock(0);
}

//-------------------------------------------------------------------------
/** CLKSRC_ChangeMSPClock
 *  Set MS card clock.
 *  @param i4ClkMHz   set msp clock.
 *  @return  VOID
 */
//-------------------------------------------------------------------------
VOID CLKSRC_ChangeMSPClock(INT32 i4ClkMHz)
{
#if 0
    UINT32 u4MsSel;
    UINT32 u4XtalClk = GET_XTAL_CLK();
    
    switch (i4ClkMHz)
    {
/*
D260 [3:0]	MS_CK_SEL[3:0]	ms_ck clock source selection

ms_ck clock source selection
0: xtal_d2_ck
1: cpupll_d12_ck 
2: cpupll_d16_ck
3: cpupll_d24_ck 
4: tvdpll_d9_ck (60MHz)
5: tvdpll_d10_ck (54MHz)
6: tvdpll_d12_ck (45MHz)
7: tvdpll_d16_ck (33.75MHz)
8: xtal_d3_ck
9: xtal_d5_ck
10: xtal_d16_ck
11: xtal_d32_ck
12: xtal_d256_ck
13: mem_ck
14: slow_ck
15: bist_ck
*/
    case 60:
        u4MsSel = 4;    // tvdpll_d9_ck (60MHz)
        break;
    case 54:
        u4MsSel = 5;    // tvdpll_d10_ck (54MHz)
        break;
    case 45:
        u4MsSel = 6;    // tvdpll_d12_ck (45MHz)
        break;
    case 33:
        u4MsSel = 7;    // tvdpll_d16_ck (33.75MHz)
        break;
    case 27:     
        if (u4XtalClk == CLK_27MHZ)
        {
            // set bist clock: 3: bist_ck = xtal_ck (27Mhz).
            vIO32WriteFldAlign(CKGEN_MISC_CKCFG, 0, FLD_BIST_CK);
            
            u4MsSel = 15;
        }
        else
        {
            u4MsSel = 0;   // xtal_d2_ck => 27 / 30 Mhz
        }        
        break;
    case 20:
        u4MsSel = 2;    // cpupll_d16_ck = 20.25 Mhz
        break;
    case 13:
        u4MsSel = 8;
        if (u4XtalClk == CLK_27MHZ)
        {
            u4MsSel = 0;    // xtal_d2_ck => 13 Mhz
        }
        else
        {
            u4MsSel = 9;    // xtal_d5_ck => 10.8 / 12 Mhz
        }        
        break;
    case 9:
        if (u4XtalClk == CLK_27MHZ)
        {
            u4MsSel = 8;    // xtal_d3_ck => 9 Mhz
        }
        else
        {
            u4MsSel = 9;    // xtal_d5_ck => 10.8 / 12 Mhz
        }
        break;
    case 6:        
        if (u4XtalClk == CLK_27MHZ)
        {
            u4MsSel = 9;    // xtal_d5_ck => 5.4 Mhz
        }
        else
        {
            u4MsSel = 9;    // xtal_d5_ck => 10.8 / 12 Mhz
        }
        break;
    case 1:
        if (u4XtalClk == CLK_27MHZ)
        {
            // set slow clock: 3: slow_ck = xtal_ck / 16 (1.6875MHz).
            vIO32WriteFldAlign(CKGEN_MISC_CKCFG, 0, FLD_SLOW_CK);
        }
        else
        {
            // set slow clock: 3: slow_ck = xtal_ck / 32 (1.6875MHz / 1.875Mhz).
            vIO32WriteFldAlign(CKGEN_MISC_CKCFG, 1, FLD_SLOW_CK);
        }        
        u4MsSel = 14;    /* slow clock */
        break;
/*
D200[3:0] Slow Clock divider selection (xtal_ck = 60MHz)
0: slow_ck = xtal_ck / 16 (3.75MHz)
1: slow_ck = xtal_ck / 32 (1.875MHz)
2: slow_ck = xtal_ck / 64 (937.5KHz)
3: slow_ck = xtal_ck / 128 (468.75KHz)
4: slow_ck = xtal_ck / 256 (234.4KHz)
5: slow_ck = xtal_ck / 512 (117.2 KHz)
6: slow_ck = xtal_ck / 1024 (58.6KHz)
7: slow_ck = xtal_ck / 2048 (29.3KHz)
*/
    case 0:
        if (u4XtalClk == CLK_27MHZ)
        {
            // set slow clock: 3: slow_ck = xtal_ck / 64 (421.875KHz).
            vIO32WriteFldAlign(CKGEN_MISC_CKCFG, 2, FLD_SLOW_CK);
        }
        else
        {
            // set slow clock: 3: slow_ck = xtal_ck / 128 (421.875KHz / 468.75Khz).
            vIO32WriteFldAlign(CKGEN_MISC_CKCFG, 3, FLD_SLOW_CK);
        }

        u4MsSel = 14;    /* slow clock */
        break;                               /* Slow cpu clk */

    default:
        Printf("No such MSP clock setting :%d\n", i4ClkMHz);
        LINT_SUPPRESS_NEXT_EXPRESSION(527);
        return;
    }

    vIO32WriteFldAlign(CKGEN_MS_CKCFG, u4MsSel, FLD_MS_CK_SEL);        
#endif
}

//-------------------------------------------------------------------------
/** CLKSRC_ChangeSDCClock
 *  Set SD card clock.
 *  @param i4ClkMHz   set sd clock.
 *  @return  VOID
 */
//-------------------------------------------------------------------------
VOID CLKSRC_ChangeSDCClock(INT32 i4ClkMHz)
{
    UINT32 u4SdSel;
    UINT32 u4XtalClk = GET_XTAL_CLK();

    switch (i4ClkMHz)
    {
/*
D264H [3:0]	SD_CK_SEL[3:0]	sd_ck clock source selection
0: xtal_ck
1: xtal_d2_ck 
2: xtal_d3_ck 
3: xtal_d4_ck 
4: xtal_d5_ck 
5: xtal_d6_ck 
6: xtal_d128_ck 
7: xtal_d256_ck 
8: sawlesspll_d8_ck
9: sawlesspll_d9_ck
10: usbpll_d16_ck
11: sawlesspll_d16_ck
12: xtal_d512_ck
13: xtal_d5_ck
14: slow_ck
15: bist_ck
*/
    case 54:         
        u4SdSel = 8;   // tvdpll_d10_ck
        break;

    case 48: 
        u4SdSel = 9;  // tvdpll_d12_ck
        break;
    case 30:
        u4SdSel = 10;  // usbpll_d16_ck, 30M
        break;
    case 27:        
        if (u4XtalClk == CLK_27MHZ)
        {
            // set bist clock: 3: bist_ck = xtal_ck (27Mhz).
            vIO32WriteFldAlign(CKGEN_MISC_CKCFG, 0, FLD_BIST_CK);
            
            u4SdSel = 0;
        }
        else 
        {
            u4SdSel = 1;   // xtal_d2_ck => 27 / 30 Mhz
        }
        break;
    case 20: // not support
        u4SdSel = 2;    // cpupll_d15_ck => 21.6 Mhz
        break;
    case 13:
        if (u4XtalClk == CLK_27MHZ)
        {
            u4SdSel = 1;    // xtal_d2_ck => 13.5 Mhz
        }
        else
        {
            u4SdSel = 13;    // xtal_d5_ck => 10.8 / 12 Mhz
        }        
        break;
    case 9:
        if (u4XtalClk == CLK_27MHZ)
        {
            u4SdSel = 2;    // xtal_d3_ck => 9 Mhz
        }
        else
        {
            u4SdSel = 13;    // xtal_d5_ck => 10.8 / 12 Mhz
        }
        break;
    case 5:        
        if (u4XtalClk == CLK_27MHZ)
        {
            u4SdSel = 4;    // xtal_d5_ck => 5.4 Mhz
        }
        else
        {
            u4SdSel = 13;    // xtal_d5_ck => 10.8 / 12 Mhz
        }
        break;
    case 3: // not support       
        if (u4XtalClk == CLK_27MHZ)
        {
            u4SdSel = 5;    // xtal_d16_ck => 1.6875 Mhz
        }
        else
        {            
            // set slow clock: 3: slow_ck = xtal_ck / 16 (3.375MHz / 3.75Mhz).
            vIO32WriteFldAlign(CKGEN_MISC_CKCFG, 0, FLD_SLOW_CK);
            
            u4SdSel = 14;    /* slow clock */
        }
        break;
    case 1:
        if (u4XtalClk == CLK_27MHZ)
        {
            // set slow clock: 3: slow_ck = xtal_ck / 16 (1.6875MHz).
            vIO32WriteFldAlign(CKGEN_MISC_CKCFG, 0, FLD_SLOW_CK);
        }
        else
        {
            // set slow clock: 3: slow_ck = xtal_ck / 32 (1.6875MHz / 1.875Mhz).
            vIO32WriteFldAlign(CKGEN_MISC_CKCFG, 1, FLD_SLOW_CK);
        }        
        u4SdSel = 5;    /* slow clock */
        break;
/*
D200[3:0] Slow Clock divider selection (xtal_ck = 60MHz)
0: slow_ck = xtal_ck / 16 (3.75MHz)
1: slow_ck = xtal_ck / 32 (1.875MHz)
2: slow_ck = xtal_ck / 64 (937.5KHz)
3: slow_ck = xtal_ck / 128 (468.75KHz)
4: slow_ck = xtal_ck / 256 (234.4KHz)
5: slow_ck = xtal_ck / 512 (117.2 KHz)
6: slow_ck = xtal_ck / 1024 (58.6KHz)
7: slow_ck = xtal_ck / 2048 (29.3KHz)
*/
    case 0:
        if (u4XtalClk == CLK_27MHZ)
        {
            // set slow clock: 3: slow_ck = xtal_ck / 64 (421.875KHz).
            vIO32WriteFldAlign(CKGEN_MISC_CKCFG, 2, FLD_SLOW_CK);
        }
        else
        {
            // set slow clock: 3: slow_ck = xtal_ck / 128 (421.875KHz / 468.75Khz).
            vIO32WriteFldAlign(CKGEN_MISC_CKCFG, 3, FLD_SLOW_CK);
        }

        u4SdSel = 14;    /* slow clock */
        break;                               /* Slow cpu clk */

    default:
        Printf("No such SDC clock setting :%d\n", i4ClkMHz);
        // ASSERT(0);
        LINT_SUPPRESS_NEXT_EXPRESSION(527);
        return;
    }

    vIO32WriteFldAlign(CKGEN_SD_CKCFG, u4SdSel, FLD_SD_CK_SEL);
}

//-------------------------------------------------------------------------
/** CLKSRC_ChangeSMXDClock
 *  Set SMXD card clock.
 *  @param i4ClkMHz   set sd clock.
 *  @return  VOID
 */
//-------------------------------------------------------------------------
#if 0
VOID CLKSRC_ChangeSMXDClock(INT32 i4ClkMHz)
{
    UINT32 u4Val, u4SdSel;

    if (_u4XDClk == (UINT32)i4ClkMHz)
    {
        return;
    }

    switch (i4ClkMHz)
    {
    case 13:
        u4SdSel = 1;
        break; /* xtal_ck/2 */

    default:
        Printf("No such SMXD clock setting :%d\n", i4ClkMHz);
        ASSERT(0);
        LINT_SUPPRESS_NEXT_EXPRESSION(527);
        return;
    }

    u4Val = CKGEN_READ32(REG_SM_CKCFG); /* Clock select. */
    u4Val &= 0xFFFFFFFC;
    u4Val |= u4SdSel;
    CKGEN_WRITE32(REG_SM_CKCFG, u4Val); /* Clock select. */

    _u4XDClk = (UINT32)i4ClkMHz;

    // wait clock to stable.
    x_thread_delay(100);
}
#endif
#endif
//-------------------------------------------------------------------------
/** CLKSRC_ChangeGFXClock
 *  Set GFX engine clock.
 *  @param  VOID
 *  @return  VOID
 */
//-------------------------------------------------------------------------
VOID CLKSRC_ChangeGFXClock(INT32 i4ClkSrc)
{
    /* Clock select.
       [Note] (1/4)x(DRAM Clk) <= (GFX clk) <= (4)x(DRAM Clk)
    */
    //vIO32WriteFldAlign(CKGEN_GRA_CKCFG, i4ClkSrc, FLD_GRA_CK_SEL);
    
    vIO32WriteFldAlign(CKGEN_GRA2D_CKCFG, i4ClkSrc, FLD_GRA2D_CK_SEL);
}

//-------------------------------------------------------------------------
/** vCLKSRC_InitRMIIClock 
 *  Set Ether MAC RMII/MII clock. 
 *  @param  VOID *  @return  VOID 
 */
//-------------------------------------------------------------------------
VOID vCLKSRC_InitRMIIClock(INT32 isMII, INT32 speed100M)
{
    if(isMII)
    {
        //vIO32WriteFldAlign(CKGEN_MIITX_CKCFG, 0x04, FLD_MIITX_CK_SEL);
        //vIO32WriteFldAlign(CKGEN_MIIRX_CKCFG, 0x05, FLD_MIIRX_CK_SEL);
    }
    else  //RMII
    {
      if(speed100M)
      	{
         #if defined(CC_MT5399)||defined(CC_MT5882)
         // ToDo: CKGEN
         #else
         vIO32WriteFldAlign(CKGEN_RMII_CKCFG, 0x02, FLD_RMII_CK_SEL);
         #endif
         //vIO32WriteFldAlign(CKGEN_MIITX_CKCFG, 0x02, FLD_MIITX_CK_SEL);
         //vIO32WriteFldAlign(CKGEN_MIIRX_CKCFG, 0x02, FLD_MIIRX_CK_SEL);
      	}
      else //RMII 10M
      	{
         #if defined(CC_MT5399)||defined(CC_MT5882)
         // ToDo: CKGEN
         #else
         vIO32WriteFldAlign(CKGEN_RMII_CKCFG, 0x02, FLD_RMII_CK_SEL);
         #endif
         //vIO32WriteFldAlign(CKGEN_MIITX_CKCFG, 0x03, FLD_MIITX_CK_SEL);
         //vIO32WriteFldAlign(CKGEN_MIIRX_CKCFG, 0x03, FLD_MIIRX_CK_SEL);
      	}
    }
}
#endif /* CC_MTK_LOADER */
