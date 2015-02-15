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
 * Copyright (c) 2005, MediaTek Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *---------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: imgrz_hw.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file imgrz_hw.c
 *  image resizer hardware interfaces
 *  purely hardware related functions, include crash detection routines,
 */


//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
#include "x_bim.h"
#include "imgrz_if.h"
#include "imgrz_drvif.h"
#include "imgrz_dif.h"
#include "imgrz_hw.h"
#include "imgrz_cmdque.h"
#include "imgrz_debug.h"

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
/*
UINT32 mtk_debug_resizer=0;
	 UINT32 IMGRZ_READ32(u4Which, UINT32 u4Offset)
	{
		UINT32 u4Reg;
		u4Reg = IO_READ32(IMGRZ_ADDR, (u4Offset));
		//Printf("  - 0x%08x = VLD0_READ32(4*%d)\n", u4Reg, (u4Offset/4));
//		Printf("r 0x%08x 1\n", (VLD0_BASE + (u4Offset - 0x400)));
		return u4Reg;
	}

	void IMGRZ_WRITE32(u4Which, UINT32 u4Offset, UINT32 u4Value)
	{
		if(mtk_debug_resizer==1)
		Printf("  - IMGRZ_WRITE32(u4Which, 0x%x, 0x%x)\n", (u4Offset), u4Value);
		IO_WRITE32(IMGRZ_ADDR, (u4Offset), u4Value);
//		Printf("w 0x%08x 0x%08x\n", (VLD0_BASE + (u4Offset - 0x400)), u4Value);
	}
*/
//---------------------------------------------------------------------------
// Static function forward declarations
//---------------------------------------------------------------------------
typedef struct _E_IMGRZ_TEMP_DATA
{
    UINT32 u4SrcCntY;
    UINT32 u4SrcCntCb;
    UINT32 u4SrcCntCr;
    UINT32 u4VWrOftY;
    UINT32 u4VWrOftCb;
    UINT32 u4VWrOftCr;
    UINT32 u4VNextCoefY;
    UINT32 u4VNextCoefCb;
    UINT32 u4VNextCoefCr;
} E_IMGRZ_TEMP_DATA;

E_IMGRZ_TEMP_DATA rTempData[IMGRZ_NUM];
#define _rTempData (rTempData[u4Which])

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------
#if (defined(IMGRZ_HW_3NM_RZ))
    static INT32 i4RzSetupISR[IMGRZ_NUM] = {IMGRZ_DISABLE,IMGRZ_DISABLE, IMGRZ_DISABLE};
#elif (defined(IMGRZ_HW_2ND_RZ))
    static INT32 i4RzSetupISR[IMGRZ_NUM] = {IMGRZ_DISABLE,IMGRZ_DISABLE};
#else
    static INT32 i4RzSetupISR[IMGRZ_NUM] = {IMGRZ_DISABLE};
#endif

#define _i4RzSetupISR   (i4RzSetupISR[u4Which])


const UINT32 ImgrzBase[IMGRZ_NUM] = {
    IMGRZ_ADDR
#ifdef IMGRZ_HW_2ND_RZ
    ,IMGRZ_ADDR2
#endif
#ifdef IMGRZ_HW_3NM_RZ
    ,IMGRZ_ADDR3
#endif
};

#ifdef IMGRZ_HW_IO_MMU
const UINT32 ImgrzMMuBase[IMGRZ_NUM] = {
    IMGRZ_MMU_BASE,
#ifdef IMGRZ_HW_2ND_RZ
    IMGRZ_MMU_BASE_2,
#endif
#ifdef IMGRZ_HW_3NM_RZ
    IMGRZ_MMU_BASE_3
#endif
};

#endif
//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------

//-------------------------------------------------------------------------
/** _ImgrzHwRealIsr
 *  interrupt service routine
 *  clear irq, calculate time, trigger hw_isr interface
 */
//-------------------------------------------------------------------------
static void _ImgrzHwRealIsr(UINT16 u2Vector)
{
    UINT32 u4Which;
    UNUSED(u2Vector);               // for lint happy

   if ((u2Vector != VECTOR_IMGRZ)
#ifdef IMGRZ_HW_2ND_RZ
            &&(u2Vector != VECTOR_IMGRZ2)
#endif
#ifdef IMGRZ_HW_3NM_RZ
            &&(u2Vector != VECTOR_IMGRZ3)
#endif
       )
    {
        Printf("RZ u2Vector false[%x] \n", u2Vector);
        ASSERT(0);
    }

    u4Which = E_FIRST_RESIZER;
#ifdef IMGRZ_HW_2ND_RZ
    if(VECTOR_IMGRZ2 == u2Vector )
    {
        u4Which = E_SECND_RESIZER;
    }
#endif
#ifdef IMGRZ_HW_3NM_RZ
    else if(VECTOR_IMGRZ3 == u2Vector)
    {
        u4Which = E_THIRD_RESIZER;
    }
#endif

    //LOG(0, "Imgrz ISR 0x%x hw %d \n", u2Vector, u4Which);

    if (BIM_ClearIrq((UINT32)u2Vector))
    {
        // stop crash detection timer ?
        if ((_IMGRZ_GetCrashDetection(u4Which) == IMGRZ_ENABLE)  &&
            (_IMGRZ_GetScaleOpt(u4Which) != E_RZ_INOUT_TYPE_JPGMD))
        {
            VERIFY(x_timer_stop(_IMGRZ_GetCrashDetectTimer(u4Which)) == OSR_OK);
        }

        IMGRZ_DBG_REC_CUR_STATUS(u4Which, (INT32)E_IMGRZ_IDLE);

    #if !defined(IMGRZ_RISC_MODE)     // cmdque mode
        IMGRZ_HwISR(u4Which);
    #endif

        // unlock cmdque resource
        // ImgrzhwISR++;
        //LOG(0, "_ImgrzHwRealIsr\n");
        _IMGRZ_UnlockWaitIsr(u4Which);
      //  _IMGRZ_UnlockCmdque(E_FIRST_RESIZER);

        IMGRZ_DBG_INC_DRV_INT_COUNT(u4Which);    // for debug use
        IMGRZ_CMDQUE_INC_INT_COUNT(u4Which);     // for debug use
    }

}

//---------------------------------------------------------------------------
// Inter-file functions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------


//-------------------------------------------------------------------------
/** IMGRZ_HwInit
 *  init hardware
 *  set control register address to default
 *  set default isr
 *  do hw reset
 */
//-------------------------------------------------------------------------
void IMGRZ_HwInit(UINT32 u4Which)
{
    x_os_isr_fct pfnOldIsr;

    // setup ISR
    if (_i4RzSetupISR == IMGRZ_DISABLE)
    {
        if(u4Which == E_FIRST_RESIZER)
        {
            VERIFY(x_reg_isr((UINT16)VECTOR_IMGRZ, _ImgrzHwRealIsr, &pfnOldIsr) == OSR_OK);
        }
    	#ifdef IMGRZ_HW_2ND_RZ
        else if(u4Which == E_SECND_RESIZER)
        {
            VERIFY(x_reg_isr((UINT16)VECTOR_IMGRZ2, _ImgrzHwRealIsr, &pfnOldIsr) == OSR_OK);
        }
    	#endif
    	#ifdef IMGRZ_HW_3NM_RZ
        else if(u4Which == E_THIRD_RESIZER)
        {
            VERIFY(x_reg_isr((UINT16)VECTOR_IMGRZ3, _ImgrzHwRealIsr, &pfnOldIsr) == OSR_OK);
        }
    	#endif
        _i4RzSetupISR = IMGRZ_ENABLE;
    }
}


#if 0
//-------------------------------------------------------------------------
/** IMGRZ_HwGetRegBase
 *  get hw control register base
 */
//-------------------------------------------------------------------------
void IMGRZ_HwGetRegBase(UINT32 u4Which, UINT32 **ppu4RegBase)
{
    IMGRZ_ASSERT(ppu4RegBase != NULL);
    IMGRZ_ASSERT(*ppu4RegBase != NULL);

    *ppu4RegBase = (UINT32 *)ImgrzBase[u4Which];
    		//(IMGRZ_ADDR+IMGRZ_BASE_OFFSET*u4Which);
}
#endif

#if !defined(IMGRZ_RISC_MODE)
//-------------------------------------------------------------------------
/** IMGRZ_HwISR
 *  hw isr
 *  reset crash detection vars
 *  set engine to idle
 *  notify if notification was set
 */
//-------------------------------------------------------------------------
void IMGRZ_HwISR(UINT32 u4Which)
{
    // call call-back function
    if (pfnIMGRZ_DifFinNotify != NULL)
    {
        if (_IMGRZ_IsFlushing(u4Which))  // if MW uses flush (IMGRZ_MwFlush)
        {
            _IMGRZ_SetFlushStatus(u4Which, FALSE);

            // notify to middleware
            pfnIMGRZ_DifFinNotify((UINT32)E_RZ_OK);

            IMGRZ_DBG_INC_MW_INT_COUNT(u4Which);  // for debug use
        }
    }
}
#endif

//-------------------------------------------------------------------------
/** IMGRZ_HwReset
 *  hw reset
 *  trigger hw reset
 *  start clock
 *  enable interrupt
 *  set register to default
 */
//-------------------------------------------------------------------------
void IMGRZ_HwReset(UINT32 u4Which)
{
    volatile UINT32 u4Value, u4Value2;
    volatile UINT32 u4RegOffset;
    UINT32  u4ImgRzClk = IMGRZ_CLK_MEM;

    UINT32 u4ResHi = (UINT32)0x3;
    UINT32 u4ResLo = (UINT32)0x0;

#ifndef IMGRZ_RISC_MODE
    //BC
    u4RegOffset = ((RZ_REG_DRAMQ_CFG << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4Which, u4RegOffset, 0x0);
#endif

#if defined(CC_MT5399)
    if (BSP_GetIcVersion() == IC_VER_5399_AB) // 5399 ES2
    {
        u4ImgRzClk = 12; // 400MHZ
    }
#elif defined(CC_MT5882)
    u4ImgRzClk = 12;
#elif defined(CC_MT5890)
    u4ImgRzClk = 3; //480MHZ
#endif

#ifdef RZ_CLK_FROM_IMGRZ_CKCFG
    //IMGRZ_HwSetEngClkSrc(u4Which, (INT32)E_IMGRZ_CLK_SYSPLL_D2);
    #ifdef IMGRZ_HW_V5
    IO_WRITE32(CKGEN_BASE, 0x3c4, u4ImgRzClk); // 324 MHZ     //CC_MT5398
    #else
    IO_WRITE32(CKGEN_BASE, 0x344, IMGRZ_CLK_SAWLESSPLL_D1P5); // 288 MHZ  //CC_MT5396, CC_MT5368,CC_MT5389
    #endif
#else

#if defined(CC_MT5387) && !defined(CC_MT5363)
    IMGRZ_HwSetEngClkSrc(u4Which, (INT32)E_IMGRZ_CLK_SYSPLL_D1P5);
#elif defined(JPG_CLK_FROM_GRAPHIC)
    IO_WRITE32(CKGEN_BASE, 0x230, 0x1); // 240 MHZ
#else
    // do clock selection (270 MHz)
    //IMGRZ_HwSetEngClkSrc(u4Which, (INT32)E_IMGRZ_CLK_TVDPLL_D2);
    //5363, imgrz clock uses vdec_mc, 216MHz
    IO_WRITE32(CKGEN_BASE, 0x228, 0x3);
#endif

#endif

    // enable engine & dram clock (set non-zero value)
//#ifdef CC_MT5387//defined (CC_MT5360B) || defined (CC_MT5392B) || defined (CC_MT5387)
#if defined(CC_MT5387) && !defined(CC_MT5363)
    u4RegOffset = ((RZ_CONFIG_REG << 2) & IMGRZ_REG_MASK);
#else
    u4RegOffset = ((RZ_REG_RZSTART << 2) & IMGRZ_REG_MASK);
#endif
    IMGRZ_WRITE32(u4Which, u4RegOffset, RZREG_MSK_CKEN );    // fg_CKEN = 0x1;

    // reset engine :
    // 1. fg_SW_RSTB = 0x3
    // 2. fg_SW_RSTB = 0x0
//#ifdef CC_MT5387//defined (CC_MT5360B) || defined (CC_MT5392B) || defined (CC_MT5387)
#if defined(CC_MT5387) && !defined(CC_MT5363)
    u4RegOffset = ((RZ_CONFIG_REG << 2) & IMGRZ_REG_MASK);
#else
    u4RegOffset = ((RZ_REG_RZSTART << 2) & IMGRZ_REG_MASK);
#endif
    u4Value = IMGRZ_READ32(u4Which, u4RegOffset);

    u4Value = (u4Value & (~(RZREG_MSK_SW_RSTB)));
    u4Value = ((u4Value) | (u4ResHi << RZREG_SHT_SW_RSTB));
    IMGRZ_WRITE32(u4Which, u4RegOffset, u4Value);

    u4Value = (u4Value & (~(RZREG_MSK_SW_RSTB)));
    u4Value = ((u4Value) | (u4ResLo << RZREG_SHT_SW_RSTB));
    IMGRZ_WRITE32(u4Which, u4RegOffset, u4Value);

    /*reset control register*/
#if defined(CC_MT5387) && !defined(CC_MT5363)
    u4RegOffset = ((RZ_CONFIG_REG << 2) & IMGRZ_REG_MASK);
#else
    u4RegOffset = ((RZ_REG_RZSTART << 2) & IMGRZ_REG_MASK);
#endif
    IMGRZ_WRITE32(u4Which, u4RegOffset, RZREG_MSK_REGRST);
#if 0//!defined (CC_MT5387) && !defined (CC_MT5363)
    // reset cmdque :
    // 1.  fg_SW_CMDQ_RST_B = 0x3
    // 2.  fg_SW_CMDQ_RST_B = 0x0
    u4RegOffset = ((RZ_REG_DRAMQ_CFG << 2) & IMGRZ_REG_MASK);
    u4Value = IMGRZ_READ32(u4Which, u4RegOffset);

    u4Value = (u4Value & (~(RZREG_MSK_SW_CMDQ_RST_B)));
    u4Value = ((u4Value) | (u4ResHi << RZREG_SHT_SW_CMDQ_RST_B));
    IMGRZ_WRITE32(u4Which, u4RegOffset, u4Value);

    u4Value = (u4Value & (~(RZREG_MSK_SW_CMDQ_RST_B)));
    u4Value = ((u4Value) | (u4ResLo << RZREG_SHT_SW_CMDQ_RST_B));
    IMGRZ_WRITE32(u4Which, u4RegOffset, u4Value);
#endif
    // wait hw reset ready
    for ( ; ; )
    {
//#ifdef CC_MT5387//defined (CC_MT5360B) || defined (CC_MT5392B) || defined (CC_MT5387)
#if defined(CC_MT5387) && !defined(CC_MT5363)
        u4RegOffset = ((RZ_CONFIG_REG << 2) & IMGRZ_REG_MASK);
#else
        u4RegOffset = ((RZ_REG_RZSTART << 2) & IMGRZ_REG_MASK);
#endif
        u4Value = IMGRZ_READ32(u4Which, u4RegOffset);

#if 0//!defined (CC_MT5387) && !defined (CC_MT5363)
        u4RegOffset = ((RZ_REG_DRAMQ_CFG << 2) & IMGRZ_REG_MASK);
        u4Value2 = IMGRZ_READ32(u4Which, u4RegOffset);
#else
        u4Value2 = IMGRZ_CMDQ_RST_READY;
#endif

        if (((u4Value  & RZREG_MSK_SW_RSTB)       == IMGRZ_ENG_RST_READY) &&
            ((u4Value2 & RZREG_MSK_SW_CMDQ_RST_B) == IMGRZ_CMDQ_RST_READY))
        {
            break;
        }
    }

#if 0//!defined (CC_MT5387) && !defined (CC_MT5363)
    // set CMDQ threshold
    u4RegOffset = ((RZ_REG_DRAMQ_CFG << 2) & IMGRZ_REG_MASK);
    u4Value = IMGRZ_READ32(u4Which, u4RegOffset);
    u4Value = (u4Value) & (~(RZREG_MSK_CMDQ_THRS));
    u4Value = (u4Value) | (0x1 << RZREG_SHT_CMDQ_THRS);
    IMGRZ_WRITE32(u4Which, u4RegOffset, u4Value);
#endif

    // enable interrupt : set fg_INT_MASK to 0x1
//#ifdef CC_MT5387//defined (CC_MT5360B) || defined (CC_MT5392B) || defined (CC_MT5387)
#if defined(CC_MT5387) && !defined(CC_MT5363)
    u4RegOffset = ((RZ_CONFIG_REG << 2) & IMGRZ_REG_MASK);
#else
    u4RegOffset = ((RZ_REG_RZSTART << 2) & IMGRZ_REG_MASK);
#endif

#ifndef IMGRZ_POLLING
    u4Value = IMGRZ_READ32(u4Which, u4RegOffset);
    u4Value = (u4Value | RZREG_MSK_INT_MASK);

	#ifdef IMGRZ_HW_V4
	#ifdef IMGRZ_HW_IO_MMU
	  	u4Value = u4Value |(1<<22);		//enable MMU gating clock
	#endif
	#endif

    IMGRZ_WRITE32(u4Which, u4RegOffset, u4Value);
#endif
}


//-------------------------------------------------------------------------
/** IMGRZ_HwGetIdle
 *  get hardware idle status
 */
//-------------------------------------------------------------------------
INT32 IMGRZ_HwGetIdle(UINT32 u4Which)
{
    volatile UINT32 u4Value;
    volatile UINT32 u4RegOffset;

    u4RegOffset = ((RZ_REG_RZDONE << 2) & IMGRZ_REG_MASK);
    u4Value = IMGRZ_READ32(u4Which, u4RegOffset);
    u4Value = ((u4Value & RZREG_MSK_IDLE) >> RZREG_SHT_IDLE);

    return ((INT32)(u4Value));
}

#if 0
//-------------------------------------------------------------------------
/** IMGRZ_HwAction
 *  start hardware engine
 *  used in interrupt mode only
 *  in command queue mode, use command queue action
 */
//-------------------------------------------------------------------------
void IMGRZ_HwAction(UINT32 u4Which)
{
    return;
}


//-------------------------------------------------------------------------
/** IMGRZ_HwSetRiscMode
 *  disable command que mode
 */
//-------------------------------------------------------------------------
void IMGRZ_HwSetRiscMode(UINT32 u4Which)
{
#if 0//!defined (CC_MT5387) && !defined (CC_MT5363)
    volatile UINT32 u4Value;
    volatile UINT32 u4RegOffset;

    u4RegOffset = ((RZ_REG_DRAMQ_CFG << 2) & IMGRZ_REG_MASK);
    u4Value = IMGRZ_READ32(u4Which, u4RegOffset);
    u4Value = ((u4Value) & (~RZREG_MSK_EN_DRAMQ));
    IMGRZ_WRITE32(u4Which, u4RegOffset, u4Value);
#endif
}


//-------------------------------------------------------------------------
/** IMGRZ_HwSetCmdQMode
 *  enable command que mode
 */
//-------------------------------------------------------------------------
void IMGRZ_HwSetCmdQMode(UINT32 u4Which)
{
//#if 0//!defined (CC_MT5387) && !defined (CC_MT5363)
    volatile UINT32 u4Value;
    volatile UINT32 u4RegOffset;

    u4RegOffset = ((RZ_REG_DRAMQ_CFG << 2) & IMGRZ_REG_MASK);
    u4Value = IMGRZ_READ32(u4Which, u4RegOffset);
    u4Value = ((u4Value) | (RZREG_MSK_EN_DRAMQ));
    IMGRZ_WRITE32(u4Which, u4RegOffset, u4Value);

}
#endif

#if ((defined(CC_MT5387) && !defined(CC_MT5363)) ||defined(IMGRZ_DEBUG_MODE))
//-------------------------------------------------------------------------
/** IMGRZ_HwSetEngClkSrc
 *  set engine clock source
 */
//-------------------------------------------------------------------------
void IMGRZ_HwSetEngClkSrc(UINT32 u4Which, INT32 i4ClkSrc)
{
    // GRA_CK : 0x2000D21C (mt539x)
    CLKSRC_ChangeGFXClock(i4ClkSrc);

    IMGRZ_DBG_REC_ENG_CLK_SRC(u4Which, i4ClkSrc);
}
#endif

#if 0
//-------------------------------------------------------------------------
/** _IMGRZ_HwSetSRAM
 *  set engine internal SRAM
 */
//-------------------------------------------------------------------------
void _IMGRZ_HwSetSRAM(UINT32 u4Pitch, UINT32 u4Is16BppPixPrec)
{
}
#endif

//-------------------------------------------------------------------------
/** _IMGRZ_HwGetCurrentLine
 *  get current scale line
 */
//-------------------------------------------------------------------------
UINT32 _IMGRZ_HwGetCurrentLine(UINT32 u4Which)
{
    UINT32 u4Ret;
    volatile UINT32 u4Value;
    volatile UINT32 u4RegOffset;
#ifdef CC_MT5363
    u4RegOffset = ((RZ_REG_RZLINECNT << 2) & IMGRZ_REG_MASK);
    u4Value = IMGRZ_READ32(u4Which, u4RegOffset);
    u4Ret = (UINT32)((u4Value >> 16) & 0x3FFF);
#else
    u4RegOffset = ((RZ_REG_RZSTMON << 2) & IMGRZ_REG_MASK);
    u4Value = IMGRZ_READ32(u4Which, u4RegOffset);
    u4Ret = (UINT32)((u4Value >> 16) & 0xFFF);
#endif
    return u4Ret;
}

#ifdef IMGRZ_POLLING
INT32 IMGRZ_HwPolling(UINT32 u4Which)
{
    volatile UINT32 u4Value;
    volatile UINT32 u4RegOffset;

    u4RegOffset = ((RZ_REG_RZDONE << 2) & IMGRZ_REG_MASK);
    u4Value = IMGRZ_READ32(u4Which, u4RegOffset);
    u4Value = u4Value & 0x1;

    return ((INT32)(u4Value));
}
#endif
#if IMGRZ_UFO_EN
UINT32 IMGRZ_UFO_Cfg(UINT32 u4Which,
		     BOOL fgEnable,
		     RZ_UFO_TYPE eUfoType)
{
    UINT32 u4Value = 0;
    volatile UINT32 u4RegOffset;

    if(E_SECND_RESIZER == u4Which)
    {
    	LOG(0, "Imgrz 1 don't have ufo\n");
	return 0;
    }

    u4RegOffset = ((RZ_REG_UFO_CFG << 2) & IMGRZ_REG_MASK);
    u4Value =  IMGRZ_READ32(u4Which, u4RegOffset);

    if(fgEnable)
	{
		u4Value  |= IMG_RESZ_UFO_UFO_EN; //enable UFO
		if(eUfoType == RZ_UFO_TYPE_8BIT)
		{
			u4Value  &= ~(0x3<<13);
		}
		else if(eUfoType == RZ_UFO_TYPE_10BIT_COMPACT)
		{
			u4Value |= IMG_RESZ_UFO_UFO_CFG_10BIT_COMPACT;
		}
		else if(eUfoType == RZ_UFO_TYPE_10BIT_REORDER)
		{
			u4Value |= IMG_RESZ_UFO_UFO_CFG_10BIT_REORDER;
		}

		IMGRZ_WRITE32(u4Which, u4RegOffset, u4Value);
	        LOG(5, "ufo cfg:0x%x\n", u4Value);
	}
    else
    {
    	u4Value = 0;
	IMGRZ_WRITE32(u4Which, u4RegOffset, u4Value);
    }
    return 0;
}

UINT32 IMGRZ_UFO_SZBuf(UINT32 u4Which,
		    UINT32 u4YBufSZSA, UINT32 u4CBufSZSA)
{
    volatile UINT32 u4RegOffset;
    if(E_SECND_RESIZER == u4Which) //0, 2 is ok
        return 0;

    u4RegOffset = ((RZ_REG_UFO_YBUFSZ_SA << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4Which, u4RegOffset, u4YBufSZSA >> 4);
    u4RegOffset = ((RZ_REG_UFO_CBUFSZ_SA << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4Which, u4RegOffset, u4CBufSZSA >> 4);
    LOG(5, "UFO write Y szBuf:0x%x, C szBuf:0x%x\n", u4YBufSZSA, u4CBufSZSA);
    LOG(5, "Register Y szBuf:0x%x, C szBuf:0x%x\n", IMGRZ_READ32(u4Which, 0x234), IMGRZ_READ32(u4Which, 0x238));
    return 0;
}

UINT32 IMGRZ_UFO_SetNewPitch(UINT32 u4Which, UINT32 u4UfoPitch)
{	
    volatile UINT32 u4RegOffset;

	u4RegOffset = ((RZ_REG_LAST<<2)&IMGRZ_REG_MASK);
	IMGRZ_WRITE32(u4Which, u4RegOffset, u4UfoPitch);	
	return 0;
}
#endif
void IMGRZ_HwBreak(UINT32 u4Which)
{
    //volatile UINT32 u4Value;
    volatile UINT32 u4RegOffset;

    u4RegOffset = ((RZ_REG_SRCCNTY << 2) & IMGRZ_REG_MASK);
    _rTempData.u4SrcCntY = IMGRZ_READ32(u4Which, u4RegOffset);
    u4RegOffset = ((RZ_REG_SRCCNTCB << 2) & IMGRZ_REG_MASK);
    _rTempData.u4SrcCntCb = IMGRZ_READ32(u4Which, u4RegOffset);
    u4RegOffset = ((RZ_REG_SRCCNTCR << 2) & IMGRZ_REG_MASK);
    _rTempData.u4SrcCntCr = IMGRZ_READ32(u4Which, u4RegOffset);
    u4RegOffset = ((RZ_REG_VWROFTY << 2) & IMGRZ_REG_MASK);
    _rTempData.u4VWrOftY = IMGRZ_READ32(u4Which, u4RegOffset);
    u4RegOffset = ((RZ_REG_VWROFTCB << 2) & IMGRZ_REG_MASK);
    _rTempData.u4VWrOftCb = IMGRZ_READ32(u4Which, u4RegOffset);
    u4RegOffset = ((RZ_REG_VWROFTCR << 2) & IMGRZ_REG_MASK);
    _rTempData.u4VWrOftCr = IMGRZ_READ32(u4Which, u4RegOffset);
    u4RegOffset = ((RZ_REG_VNEXTCOEFY << 2) & IMGRZ_REG_MASK);
    _rTempData.u4VNextCoefY = IMGRZ_READ32(u4Which, u4RegOffset);
    u4RegOffset = ((RZ_REG_VNEXTCOEFCB << 2) & IMGRZ_REG_MASK);
    _rTempData.u4VNextCoefCb = IMGRZ_READ32(u4Which, u4RegOffset);
    u4RegOffset = ((RZ_REG_VNEXTCOEFCR << 2) & IMGRZ_REG_MASK);
    _rTempData.u4VNextCoefCr = IMGRZ_READ32(u4Which, u4RegOffset);
}

void IMGRZ_HwResume(UINT32 u4Which)
{
    volatile UINT32 u4Value;
    volatile UINT32 u4RegOffset;

    IMGRZ_HwReset(u4Which);

    u4RegOffset = ((RZ_REG_SRCCNTY << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4Which, u4RegOffset, _rTempData.u4SrcCntY);
    u4RegOffset = ((RZ_REG_SRCCNTCB << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4Which, u4RegOffset, _rTempData.u4SrcCntCb);
    u4RegOffset = ((RZ_REG_SRCCNTCR << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4Which, u4RegOffset, _rTempData.u4SrcCntCr);
    u4RegOffset = ((RZ_REG_VWROFTY << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4Which, u4RegOffset, _rTempData.u4VWrOftY);
    u4RegOffset = ((RZ_REG_VWROFTCB << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4Which, u4RegOffset, _rTempData.u4VWrOftCb);
    u4RegOffset = ((RZ_REG_VWROFTCR << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4Which, u4RegOffset, _rTempData.u4VWrOftCr);
    u4RegOffset = ((RZ_REG_VNEXTCOEFY << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4Which, u4RegOffset, _rTempData.u4VNextCoefY);
    u4RegOffset = ((RZ_REG_VNEXTCOEFCB << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4Which, u4RegOffset, _rTempData.u4VNextCoefCb);
    u4RegOffset = ((RZ_REG_VNEXTCOEFCR << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4Which, u4RegOffset, _rTempData.u4VNextCoefCr);

    u4RegOffset = ((RZ_REG_RESUME << 2) & IMGRZ_REG_MASK);
    u4Value = 0x1;
    IMGRZ_WRITE32(u4Which, u4RegOffset, u4Value);
}

void IMGRZ_HwPowerDown(UINT32 u4Which)
{
    //volatile UINT32 u4Value;
    volatile UINT32 u4RegOffset;

    u4RegOffset = ((RZ_CONFIG_REG << 2) & IMGRZ_REG_MASK);
    IMGRZ_WRITE32(u4Which, u4RegOffset, 0);
}

#ifdef IMGRZ_HW_IO_MMU
static BOOL _fgIOMMUISRInited[IMGRZ_NUM] = {FALSE};
#define fgIOMMUISRInited (_fgIOMMUISRInited[u4Which])
void IMGRZ_IOMMU_IrqHandle(UINT16 u2Vector)
{
    UINT32 u4Which = E_FIRST_RESIZER;

    if ((u2Vector != VECTOR_MMU_IMGRZ)
#ifdef IMGRZ_HW_2ND_RZ
        &&(u2Vector != VECTOR_MMU_IMGRZ2)
#endif
#ifdef IMGRZ_HW_3NM_RZ
        &&(u2Vector != VECTOR_MMU_IMGRZ3)
#endif
       )
    {
        Printf("RZ IOMMU u2Vector false[%x] \n", u2Vector);
        ASSERT(0);
    }

       u4Which = E_FIRST_RESIZER;
#ifdef IMGRZ_HW_2ND_RZ
    if(u2Vector == VECTOR_MMU_IMGRZ2)
    {
        u4Which = E_SECND_RESIZER;
    }
#endif
#ifdef IMGRZ_HW_3NM_RZ
    if(VECTOR_MMU_IMGRZ3 == u2Vector)
    {
        u4Which = E_THIRD_RESIZER;
    }
#endif /*IMGRZ_HW_3NM_RZ*/

    if (fgIOMMUISRInited == 0)
    {
        //return;
        Printf("RZ IOMMU fgIOMMUISRInited=FALSE \n");
        ASSERT(0);
    }
 
    LOG(0, "RZ IOMMU page fault occurs,hw %d status =%x\n",
        u4Which,
    	IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG8));
    //clear IOMMU status bit
    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG4, IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG4) | 0x5);
    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG4, IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG4) & ~(0x5));

    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG4, IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG4) & ~(0x80000));//for show va while iommu assert

#if 1
     {
          UINT32 ui4_tmp0 = 0, ui4_tmp1 = 0;
            /*pa*/
          ui4_tmp0 = IMGRZ_MMU_READ32(u4Which,RZ_REG_RW_IOMMU_CFG4);
          //LOG(0,"CFG4= 0x%08X\n", ui4_tmp0);
          ui4_tmp0 &= ~(0xF<<8) ;
          ui4_tmp0 |= (0x8<<8) ;
          IMGRZ_MMU_WRITE32(u4Which,RZ_REG_RW_IOMMU_CFG4,  ui4_tmp0);
          ui4_tmp1 = IMGRZ_MMU_READ32(u4Which,RZ_REG_RW_IOMMU_CFG8);
          LOG(0,"CFG4= 0x%08X,pa,CFG8=0x%08X\n", ui4_tmp0, ui4_tmp1);
          /*va*/
          ui4_tmp0 = IMGRZ_MMU_READ32(u4Which,RZ_REG_RW_IOMMU_CFG4);
          ui4_tmp0 &= ~(0xF<<8) ;
          ui4_tmp0 |= (0x9<<8) ;
          IMGRZ_MMU_WRITE32(u4Which,RZ_REG_RW_IOMMU_CFG4, ui4_tmp0);
          ui4_tmp1 = IMGRZ_MMU_READ32(u4Which,RZ_REG_RW_IOMMU_CFG8);
          LOG(0,"CFG4= 0x%08X,va,CFG8=0x%08X\n", ui4_tmp0, ui4_tmp1);
          #define RESZ_READ(a) IMGRZ_READ32(u4Which,(a))
          LOG(0,"Src 0x%08X 0x%08X 0x%08X L 0x%08X \n",
                      RESZ_READ(0x1c), RESZ_READ(0x24), RESZ_READ(0x2C), RESZ_READ(0x10));
          LOG(0,"Dst 0x%08X 0x%08X L 0x%08X Temp 0x%08X\n",
                      RESZ_READ(0x34), RESZ_READ(0x38), RESZ_READ(0x18),
                      RESZ_READ(0x94));
     }
#endif

    //clear BIM status bit
    VERIFY(BIM_ClearIrq(u2Vector));
}



void IMGRZ_HW_MMU_Enable(UINT32 u4Which, BOOL bEnable)
{
	UINT32 u4RegVal = 0;
    //enable or disalbe mmu
    u4RegVal = 0xFE|bEnable;
    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG0, u4RegVal);
}

void IMGRZ_HW_Cfg_MMU(UINT32 u4Which, UINT32 MMU_cfg, UINT32 MMU_table_addr)
{
    x_os_isr_fct pfnOldIsr;
    if(0 == MMU_cfg)
    {
        return;
    }

	ASSERT(MMU_table_addr != 0);

    //IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG4, 0x0020010A);
    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG4, 0x0010010A);		//bit31 means self fire

    //enable mmu
    //u4RegVal = 0xFE|(0x1<<0);
    //IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG0, u4RegVal);

    if((MMU_cfg & 0x1) || (MMU_cfg & (0x1<<16)))
    {
        IMGRZ_HW_MMU_Enable(u4Which, TRUE);
     }
    else
    {
        return;
    }
    LOG(2, "RZ_REG_RW_IOMMU_CFG1 MMU_table_addr is 0x%x \n",HalGetMMUTableAddress());

	if(MMU_table_addr == 0)
    {
	    LOG(2, "RZ_REG_RW_IOMMU_CFG1 MMU_table_addr is NULL \n");
    	IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG1, HalGetMMUTableAddress());
    }
    else
    {
    	LOG(2, "RZ_REG_RW_IOMMU_CFG1  =%x, new table= %x\n", IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG1), MMU_table_addr);
		IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG1, MMU_table_addr);
    }

    //enable imgrz agent-2 ID for progressive preload
    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG3, 0x00000015);
    //IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG3, 0x00000014);

	  LOG(2, "RZ_REG_RW_IOMMU_CFG3 =%x \n", IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG3));


    HalFlushInvalidateDCacheMultipleLine(VIRTUAL(MMU_table_addr), 2048*4);

    //enable imgrz agent-2 ID for progressive preload
    //IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG3, 0x00000015);
    
    if (fgIOMMUISRInited == 0)
    {
        fgIOMMUISRInited = 1;
        if(E_FIRST_RESIZER == u4Which)
        {
            BIM_ClearIrq(VECTOR_MMU_IMGRZ);
            if (x_reg_isr(VECTOR_MMU_IMGRZ, IMGRZ_IOMMU_IrqHandle, &pfnOldIsr) != OSR_OK)
            {
                LOG(0, "RZ #1 IOMMU x_reg_isr create error \n");
                ASSERT(0);
            }
        }
#ifdef IMGRZ_HW_2ND_RZ
        else if(E_SECND_RESIZER == u4Which)
        {
            BIM_ClearIrq(VECTOR_MMU_IMGRZ2);
            if (x_reg_isr(VECTOR_MMU_IMGRZ2, IMGRZ_IOMMU_IrqHandle, &pfnOldIsr) != OSR_OK)
            {
                LOG(0, "RZ #2 IOMMU x_reg_isr create error \n");
                ASSERT(0);
            }
        }
#endif /*IMGRZ_HW_2ND_RZ*/
#ifdef IMGRZ_HW_3NM_RZ
        else if(E_THIRD_RESIZER == u4Which)
        {
            BIM_ClearIrq(VECTOR_MMU_IMGRZ3);
            if (x_reg_isr(VECTOR_MMU_IMGRZ3, IMGRZ_IOMMU_IrqHandle, &pfnOldIsr) != OSR_OK)
            {
                LOG(0, "RZ #2 IOMMU x_reg_isr create error \n");
                ASSERT(0);
            }
        }
#endif /*IMGRZ_HW_3NM_RZ*/
    }

    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG2, MMU_cfg | 0x00120010);
    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG4, IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG4) | 0x80000000);



    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG4, IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG4) | 0x80800);

    while(RZ_MSK_MMU_SFIRE_RST&IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG8))		//wait (self_fire status)
    {
        // x_thread_delay(1);
    }

	LOG(2, "@@@IOMMU 1 [%x][%x][%x][%x] \n", IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG0),
			IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG1),
			IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG2),
			IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG3));
	LOG(2, "@@@IOMMU 2 [%x][%x][%x][%x] \n", IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG4),
			IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG8),
			IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG9),
			IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFGA));

}

void  vIOMMU_OverRead_Protection(UINT32 u4Which, UINT32 ui4_Agent,
        UINT32 ui4_StartAddr0,
        UINT32 ui4_Buff_sz0,
        UINT32 ui4_StartAddr1,
        UINT32 ui4_Buff_sz1)
{

    //method 0
    /*
       IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG9, ((ui4_StartAddr0+ui4_Buff_sz0)&0xfffff000) | 0x1);
       IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFGA, ((ui4_StartAddr1+ui4_Buff_sz1)&0xfffff000) | 0x3);
       IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFGB, 0x21);
       */
    //method 1

 /* //TV original Err , here it is from BD.
    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG9, ((ui4_StartAddr0+ui4_Buff_sz0)&0xfffff000) | 0x1);
    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFGA, ((ui4_StartAddr1+ui4_Buff_sz1)&0xfffff000) | 0x3);
  */

    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG9, ((ui4_StartAddr0+ui4_Buff_sz0-1)&0xFFFFF000)|(0x0<<1)|0x1);
    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFGC, (ui4_StartAddr0&0xfffff000));

    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFGA, ((ui4_StartAddr1+ui4_Buff_sz1-1)&0xFFFFF000)|(0x1<<1)|0x1);
    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFGD, (ui4_StartAddr1&0xfffff000));
    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFGB, 0x10021);

}

#endif /*IMGRZ_HW_IO_MMU*/

#ifdef IMGRZ_HW_V4

#ifdef IMGRZ_HW_IO_MMU_AUTO_TEST
//#define VECTOR_IMGRZ_IOMMU 60

extern void HalSet4KPageTable(UINT32 u4PhyAddr, UINT32 u4VirtAdr, UINT32* pu4PageTableAddress);
extern void HalSet1MSectionTable(UINT32 u4PhyAddr, UINT32 u4VirtAdr);
#define IMGRZ_READ_AGENT 0
#define IMGRZ_WRITE_AGENT 1

extern UINT32 HalGetMMUTableAddress(void);
UINT32 IMGRZGetSystemMMUTableAddress(void)
{
#ifdef __KERNEL__
    return HalCpuGetPhyPgd();
#else
    return HalGetMMUTableAddress();
#endif
}

void IMGRZ_SetMMU(UINT32 u4Which)
{
    UINT32 u4RegVal = 0;
#if defined(IMGRZ_HW_IO_MMU_W_VDOJPG_MODE)||defined(IMGRZ_HW_IO_MMU_RW_OSD_MODE)
    UINT32 i = 0;
#endif

    x_os_isr_fct pfnOldIsr;

    //==================================================================
    //test 1M page
    /*
       for(i=0;i<16;i++)
       {
       HalSet1MSectionTable(0xb700000+i*0x200000, 0xa700000+i*0x100000);
       }*/

    /*
       for(i=0;i<16;i++)
       {
       HalSet1MSectionTable(0xb000000+i*0x100000, 0xa000000+i*0x100000);
       HalSet1MSectionTable(0xa000000+i*0x100000, 0xb000000+i*0x100000);
       HalSet1MSectionTable(0xf000000+i*0x100000, 0xe000000+i*0x100000);
       HalSet1MSectionTable(0xe000000+i*0x100000, 0xf000000+i*0x100000);
       }
       */
    //==================================================================
    //test 4K page
    /*
       x_memset((void*)0xa000000, 0, 0x400);
       x_memset((void*)0xa000400, 0, 0x400);


       for(i = 0; i < 0x4000; i+=0x2000)
    //for(i = 0; i < 0x100000; i+=0x2000)
    {
    HalSet4KPageTable(0xb801000 + i, 0xa800000 + i, (UINT32*)0xa000000); //phy=184M, virt=168M
    HalSet4KPageTable(0xb800000 + i, 0xa801000 + i, (UINT32*)0xa000000); //phy=184M, virt=168M

    HalSet4KPageTable(0xc001000 + i, 0xb000000 + i, (UINT32*)0xa000400); //phy=192M, virt=176M
    HalSet4KPageTable(0xc000000 + i, 0xb001000 + i, (UINT32*)0xa000400); //phy=192M, virt=176M
    }
    */

    //make page fault case
    /*
       x_memset((void*)0xa000000, 0, 0x400);
       for(i = 0; i < 0x100000; i+=0x2000)
       {
       HalSet4KPageTable(0xb801000 + i, 0xa800000 + i, (UINT32*)0xa000000); //phy=184M, virt=168M
       HalSet4KPageTable(0xb800000 + i, 0xa801000 + i, (UINT32*)0xa000000); //phy=184M, virt=168M
       }

       x_memset((void*)0xa000400, 0, 0x400);
       for(i = 0; i < 0x10000; i+=0x1000)
       {
       HalSet4KPageTable(0xc000000 + i, 0xb000000 + i, (UINT32*)0xa000400); //phy=184M, virt=168M
       }
       */

    //==================================================================

    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG4, 0x0020010A);

    //enable mmu
    u4RegVal = 0xFE|(0x1<<0);
    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG0, u4RegVal);
    u4RegVal = IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG0);
    printf("[IMGRESZ_VFY] MMU CFG0 is %x . \n",u4RegVal);

    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG1, IMGRZGetSystemMMUTableAddress());
    u4RegVal = IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG1);
    printf("[IMGRESZ_VFY] MMU CFG1 is %x . \n",u4RegVal);

#ifdef IMGRZ_HW_IO_MMU_W_VDOJPG_MODE
    //write output: 0x1f600800, 0x1fb00000
    for(i=0;i<16;i++)
    {
        HalSet1MSectionTable(0x1f000000+i*0x100000, 0x1e000000+i*0x100000);
        HalSet1MSectionTable(0x1e000000+i*0x100000, 0x1f000000+i*0x100000);
    }

    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG2, 0x00130010);	//turn off MMU read => only write for osd, vdo and jpg mode.
#endif

#ifdef IMGRZ_HW_IO_MMU_RW_OSD_MODE
    //read input: 0xa7fc000, 0xabfc000, write output: 0xf600800, 0xfb00000
    for(i=0;i<16;i++)
    {
        HalSet1MSectionTable(0xb000000+i*0x100000, 0xa000000+i*0x100000);
        HalSet1MSectionTable(0xa000000+i*0x100000, 0xb000000+i*0x100000);
        HalSet1MSectionTable(0xf000000+i*0x100000, 0xe000000+i*0x100000);
        HalSet1MSectionTable(0xe000000+i*0x100000, 0xf000000+i*0x100000);
    }

#ifdef IMGRZ_HW_A_COMPOSITION
    //change addr between 0xc000000 and 0xd000000
    for(i=0;i<16;i++)
    {
        HalSet1MSectionTable(IMGRZ_HW_A_BLENDING_PRELOAD_ADDR+i*0x100000, (IMGRZ_HW_A_BLENDING_PRELOAD_ADDR+0x1000000)+i*0x100000);
        HalSet1MSectionTable((IMGRZ_HW_A_BLENDING_PRELOAD_ADDR+0x1000000)+i*0x100000, IMGRZ_HW_A_BLENDING_PRELOAD_ADDR+i*0x100000);
    }
#endif


    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG2, 0x00130011);	//turn on MMU read&write =>read, write	for osd mode.
#endif

    HalFlushInvalidateDCacheMultipleLine(IMGRZGetSystemMMUTableAddress(), 2048*4);

    u4RegVal = IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG2);
    printf("[IMGRESZ_VFY] MMU CFG2 is %x . \n",u4RegVal);

    //enable imgrz agent-2 ID for progressive preload
    IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG3, 0x00000015);
    //IMGRZ_MMU_WRITE32(u4Which, RZ_REG_RW_IOMMU_CFG3, 0x00000014);

    u4RegVal = IMGRZ_MMU_READ32(u4Which, RZ_REG_RW_IOMMU_CFG3);
    printf("[IMGRESZ_VFY] MMU CFG3 is %x . \n",u4RegVal);

    if (fgIOMMUISRInited == 0)
    {
        fgIOMMUISRInited = 1;
        if(u4Which == E_FIRST_RESIZER)
        {
            //i4GCPU_HAL_ClrIntr(GCPU_INT_MASK);
            BIM_ClearIrq(VECTOR_MMU_IMGRZ);

            if (x_reg_isr(VECTOR_MMU_IMGRZ, IMGRZ_IOMMU_IrqHandle, &pfnOldIsr) != OSR_OK)
            {
                Printf("RZ IOMMU x_reg_isr create error \n");
                ASSERT(0);
            }
        }
#ifdef IMGRZ_HW_2ND_RZ
        else if(u4Which == E_SECND_RESIZER)
        {
            BIM_ClearIrq(VECTOR_MMU_IMGRZ2);
            if (x_reg_isr(VECTOR_MMU_IMGRZ2, IMGRZ_IOMMU_IrqHandle, &pfnOldIsr) != OSR_OK)
            {
                Printf("RZ IOMMU x_reg_isr 2 create error \n");
                ASSERT(0);
            }
        }
#endif
#ifdef IMGRZ_HW_3NM_RZ
        else if(u4Which == E_THIRD_RESIZER)
        {
            BIM_ClearIrq(VECTOR_MMU_IMGRZ3);
            if (x_reg_isr(VECTOR_MMU_IMGRZ3, IMGRZ_IOMMU_IrqHandle, &pfnOldIsr) != OSR_OK)
            {
                Printf("RZ IOMMU x_reg_isr 2 create error \n");
                ASSERT(0);
            }
        }
#endif

        //vIOMMU_OverRead_Protection(IMGRZ_WRITE_AGENT, 0xa7fc000, 253008, 0xaffc000, 0x1000);
    }
}
#endif /*IMGRZ_AUTO_TEST*/

#endif /*IMGRZ_HW_IO_MMU_AUTO_TEST*/



