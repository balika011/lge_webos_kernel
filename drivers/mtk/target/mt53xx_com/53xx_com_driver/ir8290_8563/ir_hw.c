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
 * $RCSfile: ir_hw.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file ir_hw.c
 *  ir_hw.c provides IRRX/IRTX register access functions.
 */
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "ir_if.h"
#include "ir_debug.h"
#include "x_assert.h"
#include "x_bim.h"
#include "x_ckgen.h"
#include "x_pdwnc.h"
#include "x_debug.h"
#if defined(CC_CONFIG_IRRX_COMMON_DRIVER_DTV)  /*dtv head file*/
#include "x_hal_arm.h"
#include "x_lint.h"
#include "x_printf.h"
#endif/*CC_CONFIG_IRRX_COMMON_DRIVER_DTV*/
#if defined(CONFIG_IRRX_COMMON_DRIVER_BD)  /*bd head file*/
#include "x_os.h"
#include "x_lint.h"
#include "drv_ir.h"
#include "pdwnc_isr.h"
#include "chip_ver.h"
#include "sys_config.h"
#include "x_os.h"
#include <linux/interrupt.h>
#include <linux/string.h>
#include "drv_config.h"
#include "ir_regs.h"

/////*****here is common function for BD and DTV********/
static INT32 _IRHW_RxSetIsr(BOOL fgSet);
static void _IrRxHwIsr(UINT16 u2VctId);
static void _IrRxHwBusyClr(void);

INT32 IRHW_RxSetCallback(PFN_IRRXCB_T pfnCallback, PFN_IRRXCB_T *ppfnOld);
void IRHW_RxWrConf(INT32 i4Config, INT32 i4SaPeriod, INT32 i4Threshold);
INT32 IRHW_RxStop(void);
void IRHW_RxRdConf(INT32 * pi4Config, INT32 * pi4SaPeriod,  INT32 * pi4Threshold);
INT32 IRHW_RxSetCallback(PFN_IRRXCB_T pfnCallback, PFN_IRRXCB_T *ppfnOld);

//---------------------------------------------
static BOOL _fgIREnable = TRUE;

#endif/*CC_CONFIG_IRRX_COMMON_DRIVER_DTV*/

#if defined(CC_CONFIG_IRRX_COMMON_DRIVER_DTV) 

#ifdef CC_MTK_LOADER
#define IRRX_NON_OSAI    //'IRRX_NON_OSAI' define is used only for DTV system.
#endif /* CC_MTK_LOADER */

#define IRRX_CLK_3MHZ_DIV_1    0   // IRRX clock set in 24(3)MHZ
#define IRRX_CLK_3MHZ_DIV_2	   1   // IRRX clock set in 24(3)MHZ/2
#define IRRX_CLK_3MHZ_DIV_4	   2   // IRRX clock set in 24(3)MHZ/4
#define IRRX_CLK_3MHZ_DIV_8	   3   // IRRX clock set in 24(3)MHZ/8
#define IRRX_CLK_3MHZ_DIV_16   4   // IRRX clock set in 24(3)MHZ/16
#define IRRX_CLK_3MHZ_DIV_32   5   // IRRX clock set in 24(3)MHZ/32
#define IRRX_CLK_3MHZ_DIV_64   6   // IRRX clock set in 24(3)MHZ/64
#define IRRX_CLK_3MHZ_DIV_128  7   // IRRX clock set in 24(3)MHZ/128
#define IRRX_CLK_3MHZ_DIV_256  8   // IRRX clock set in 24(3)MHZ/256
#define IRRX_CLK_3MHZ_DIV_511  9   // IRRX clock set in 24(3)MHZ/511

#define IRRX_CLR_BSY_IR		0x01
#define MAX_IRRX_DATA       (4)

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static UINT32 u4IrPulseTimeUnit = 50; //use default value 50 us

#endif

#ifndef IRRX_NON_OSAI
static PFN_IRRXCB_T _pfnIrRxCbFunc = NULL;
#endif

//-----------------------------------------------------------------------------
/*
** _IrRxHwBusyClr() to clear IRRX h/w buffer then it can receive next message.
*/
//-----------------------------------------------------------------------------
static void _IrRxHwBusyClr(void)
{
#if defined(CC_CONFIG_IRRX_COMMON_DRIVER_DTV) 
    vIO32WriteFldAlign(PDWNC_IRCLR, IRRX_CLR_BSY_IR, FLD_IRCLR);
#endif

#if defined(CONFIG_IRRX_COMMON_DRIVER_BD)
	#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8563)
	IR_WRITE32(IRRX_IRCLR, (IR_READ32(IRRX_IRCLR) | IRCLR));
	IR_WRITE32(IRRX_INTCLR, (IR_READ32(IRRX_INTCLR) | IR_INTCLR));   	
	BIM_ClearIrq(VECTOR_INT_P_IR);	
	#endif
#endif
}

#ifndef IRRX_NON_OSAI
//-----------------------------------------------------------------------------
/*
 * _IrRxHwIsr() the interrupt of IRRX from PDWNC interrupt ISR.
 * @param u2VctId It must be PDWNC_INTNO_IRRX.
 */
//-----------------------------------------------------------------------------
static void _IrRxHwIsr(UINT16 u2VctId)
{
#if defined(CC_CONFIG_IRRX_COMMON_DRIVER_DTV) 
    UINT32 u4Info;
    UINT32 au4IrRxData[MAX_IRRX_DATA];
	UINT32 i, u4PulsePeriod;
    UNUSED(u2VctId);

    /* read IRRX IRQ data. */
    u4Info = u4IO32Read4B(PDWNC_IRCNT);
    au4IrRxData[0] = u4IO32Read4B(PDWNC_IRDATA0);
    au4IrRxData[1] = u4IO32Read4B(PDWNC_IRDATA1);

    LOG(7, "PWD:0x%08x RxIsr Info:0x%08x 0x%08x%08x\n",u4IO32Read4B(PDWNC_INTSTA), u4Info, au4IrRxData[1], au4IrRxData[0]);
	LOG(9, "IR period(us):\n");
	for(i = 0; i < 67; )
	{
		u4PulsePeriod = u4IO32Read4B(PDWNC_CHKDATA0 + i);
		LOG(9, "%4d, %4d, %4d, %4d,",  (u4PulsePeriod & 0xff) * u4IrPulseTimeUnit, ((u4PulsePeriod >> 8 )& 0xff) * u4IrPulseTimeUnit,((u4PulsePeriod >> 16 )& 0xff) * u4IrPulseTimeUnit, ((u4PulsePeriod >> 24 )& 0xff) * u4IrPulseTimeUnit);
		i+=4;
		u4PulsePeriod = u4IO32Read4B(PDWNC_CHKDATA0 + i);
		LOG(9, " %4d, %4d, %4d, %4d\n",  (u4PulsePeriod & 0xff) * u4IrPulseTimeUnit, ((u4PulsePeriod >> 8 )& 0xff) * u4IrPulseTimeUnit,((u4PulsePeriod >> 16 )& 0xff) * u4IrPulseTimeUnit, ((u4PulsePeriod >> 24 )& 0xff) * u4IrPulseTimeUnit);
		i+=4;
	}
	
    if ((u4Info != 0) && (_pfnIrRxCbFunc != NULL))
    {
        _pfnIrRxCbFunc(u4Info, (UINT8 *)au4IrRxData);
    }

    /* clear interrupt. */
    VERIFY(PDWNC_ClearInt(PDWNC_INTNO_IRRX));

    /* Do BIM clear IR busy bit, after interrupt clear */
    _IrRxHwBusyClr();
#endif

#if defined(CONFIG_IRRX_COMMON_DRIVER_BD)
   	UINT32 _u4Info;
    UINT32 _au4IrRxData[MAX_IRRX_DATA];
   	UNUSED(u2VctId);
    /* read IRRX IRQ data. */
	
    _u4Info = IR_READ32(IRRX_COUNT_HIGH_REG);
    _au4IrRxData[0] = IR_READ32(IRRX_COUNT_MID_REG);
    _au4IrRxData[1] = IR_READ32(IRRX_COUNT_LOW_REG);

    if((0 != _au4IrRxData[0]) || (0 != _au4IrRxData[1]))
    {
	    LOG(10, "RxIsr Info:0x%08x data: 0x%08x%08x\n", _u4Info, _au4IrRxData[1],
        _au4IrRxData[0]);
    }
	
    if ((_fgIREnable == TRUE) && (_u4Info != 0) && (_pfnIrRxCbFunc != NULL))
    {
        _pfnIrRxCbFunc(_u4Info, (UINT8 *) _au4IrRxData);
    }

    /* clear IR busy bit. */
    _IrRxHwBusyClr();
#endif
}
#endif

//-----------------------------------------------------------------------------
/*
 *  _IRHW_RxSetIsr() to enable/disable interrupt of IRRX
 *  IRRX interrupt is a sub-interrupt of PDWNC interrupt, the enable/disable \n
 *  function is to call PDWNC function to set the PDWNC interrupt enable \n
 *  register.
 *  @param fgSet a boolean value, true to enable ISR, false to disable.
 *  @retval 0 is operating successfully, otherwise failed.
 */
//-----------------------------------------------------------------------------
static INT32 _IRHW_RxSetIsr(BOOL fgSet)
{
#if defined(CC_CONFIG_IRRX_COMMON_DRIVER_DTV) 
	#ifndef IRRX_NON_OSAI
	INT32 i4Val;
	void (* pfnOldIsr)(UINT16);

	if (fgSet)
	{   /* enable irq. */

	  i4Val = PDWNC_RegIsr(PDWNC_INTNO_IRRX, _IrRxHwIsr, &pfnOldIsr);
	  if (i4Val != PDWNC_OK)
	  {
		  return IR_FAIL;
	  }
	  VERIFY(PDWNC_ClearInt(PDWNC_INTNO_IRRX));
	}

	else
	{   /* disable irq. */

	  i4Val = PDWNC_RegIsr(PDWNC_INTNO_IRRX, NULL, &pfnOldIsr);
	  if (i4Val != OSR_OK)
	  {
		  return IR_FAIL;
	  }
	}
	#endif /* IRRX_NON_OSAI */ 
#endif

#if defined(CONFIG_IRRX_COMMON_DRIVER_BD)
	#if(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8563)
	x_os_isr_fct pfnOldIsr;
	#endif   
	if (fgSet)
	{							/* enable irq. */ 
		#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8563) 
		if(x_reg_isr_ex(VECTOR_INT_P_IR, _IrRxHwIsr, &pfnOldIsr, X_ISR_FLAG_SAMPLE_RANDOM) != OSR_OK) 
		{
		return IR_FAIL;
		}
		#endif	
		_IrRxHwBusyClr();     
	}
	else
	{	
		#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8563) 
		IR_WRITE32(IRRX_INTEN, (IR_READ32(IRRX_INTEN) & (~IR_INTEN)));
		if(x_reg_isr(VECTOR_INT_P_IR, NULL, &pfnOldIsr) != OSR_OK)	     		
		{
			return IR_FAIL;
		}
		#endif	
	}
#endif
   return IR_SUCC;
}

//-----------------------------------------------------------------------------
/*
 * IRHW_Stop() to stop IRRX ISR
 *  @retval 0 is operating successfully, otherwise failed.
 */
//-----------------------------------------------------------------------------
INT32 IRHW_RxStop(void)
{
    INT32 i4Ret;

    i4Ret = _IRHW_RxSetIsr((BOOL) FALSE); 
    return i4Ret;
}


#ifndef IRRX_NON_OSAI
//-----------------------------------------------------------------------------
/*
 * IRHW_RxSetCallback() to register interrupt callback routine.
 *  @param pfnCallback the new register callback routine
 *  @param ppfnOld pointer to the old callback routine
 *  @retval 0 is operating successfully, otherwise failed.
 */
//-----------------------------------------------------------------------------
INT32 IRHW_RxSetCallback(PFN_IRRXCB_T pfnCallback, PFN_IRRXCB_T * ppfnOld)
{
	if (ppfnOld != NULL)
	{
	*ppfnOld = _pfnIrRxCbFunc;
	}
    _pfnIrRxCbFunc = pfnCallback;
    return IR_SUCC;
}
#endif /* IRRX_NON_OSAI */


//-----------------------------------------------------------------------------
/*
 *  IRHW_RxRdConf() to read IRRX configration
 *  @param pi4Config to read PDWNC_IRCFGH
 *  @param pi4SaPeriod to read PDWNC_IRCFGL
 *  @param pi4Threshold to read PDWNC_IRTHD
 */
//-----------------------------------------------------------------------------
void IRHW_RxRdConf(INT32 * pi4Config, INT32 * pi4SaPeriod,  INT32 * pi4Threshold)
{
#if defined(CC_CONFIG_IRRX_COMMON_DRIVER_DTV) 
	if ((pi4Config == NULL) || (pi4SaPeriod==NULL) || (pi4Threshold==NULL))
		{
			ASSERT(0);
			LINT_SUPPRESS_BRACE(527);
			return;
		}
		*pi4Config =	(INT32)u4IO32Read4B(PDWNC_IRCFGH);
		*pi4SaPeriod =	(INT32)u4IO32Read4B(PDWNC_IRCFGL);
		*pi4Threshold = (INT32)u4IO32Read4B(PDWNC_IRTHD);

#endif
#if defined(CONFIG_IRRX_COMMON_DRIVER_BD)
    if ((pi4Config == NULL) || (pi4SaPeriod == NULL) || (pi4Threshold == NULL))
    {
        ASSERT(0);
        LINT_SUPPRESS_BRACE(527);
        return;
    }
    *pi4Config = (INT32) IR_READ32(IRRX_CONFIG_HIGH_REG);
    *pi4SaPeriod = (INT32) IR_READ32(IRRX_CONFIG_LOW_REG);
    *pi4Threshold = (INT32) IR_READ32(IRRX_THRESHOLD_REG);
#endif
}

//-----------------------------------------------------------------------------
/*
 * IRHW_RxWrConf() to set IRRX configration
 *  @param pi4Config to read PDWNC_IRCFGH
 *  @param pi4SaPeriod to read PDWNC_IRCFGL
 *  @param pi4Threshold to read PDWNC_IRTHD
 */
//-----------------------------------------------------------------------------
void IRHW_RxWrConf(INT32 i4Config, INT32 i4SaPeriod, INT32 i4Threshold)
{ 
#if defined(CC_CONFIG_IRRX_COMMON_DRIVER_DTV) 
    /* Set IRRX clock to 3Mhz / 64 */
    vIO32WriteFldAlign(PDWNC_IRCKSEL, IRRX_CLK_3MHZ_DIV_64, FLD_IRCLKSEL);
    /* Config IRRX registers */
    vIO32Write4B(PDWNC_IRCFGH, (UINT32)i4Config);
    vIO32Write4B(PDWNC_IRCFGL, (UINT32)i4SaPeriod);
    vIO32Write4B(PDWNC_IRTHD, (UINT32)i4Threshold);
	//for ir HW check pulse test
	if(IO32ReadFldAlign(PDWNC_PDMODE, FLD_DEEP_SLEEP))// 3Mhz
	{
		vIO32WriteFldAlign(PDWNC_IRCFGL, u4IrPulseTimeUnit * 3, FLD_CHK_SAPERIOD);
	}
	else
	{		
		vIO32WriteFldAlign(PDWNC_IRCFGL, u4IrPulseTimeUnit * 24, FLD_CHK_SAPERIOD);		
	}
    vIO32WriteFldAlign(PDWNC_IRCFGH, 1, FLD_CHK_EN);
#endif

#if defined(CONFIG_IRRX_COMMON_DRIVER_BD)
/* Config IRRX registers */
   IR_WRITE32(IRRX_CONFIG_HIGH_REG, (UINT32) i4Config);
   IR_WRITE32(IRRX_CONFIG_LOW_REG, (UINT32) i4SaPeriod);
   IR_WRITE32(IRRX_THRESHOLD_REG, (UINT32) i4Threshold);
#endif
}




//-----------------------------------------------------------------------------
/*
 * IRHW_Init() to set IRRX configration and start IRRX ISR
 *  @param pi4Config to read PDWNC_IRCFGH
 *  @param pi4SaPeriod to read PDWNC_IRCFGL
 *  @param pi4Threshold to read PDWNC_IRTHD
 *  @retval 0 is operating successfully, otherwise failed.
 */
//-----------------------------------------------------------------------------
INT32 IRHW_RxInit(INT32 i4Config, INT32 i4SaPeriod, INT32 i4Threshold)
{
	INT32 i4Ret;
#if defined(CC_CONFIG_IRRX_COMMON_DRIVER_DTV) 
	#ifndef CC_UBOOT
	UINT32 u4State;
	#endif /* CC_UBOOT */
	INT32 i;

	IRHW_RxWrConf(i4Config, i4SaPeriod, i4Threshold);
	i4Ret = _IRHW_RxSetIsr((BOOL)TRUE);
	LOG(0,"u4IrPulseTimeUnit is %d us\n", u4IrPulseTimeUnit);
	#ifndef CC_UBOOT
	u4State = HalCriticalStart();
	#endif /* CC_UBOOT */

	// wait 1ms for Busy Clr operation finish.
	for (i=0; i<30000; i++) { }
	_IrRxHwBusyClr();
	// wait 1ms for Busy Clr operation finish.
	for (i=0; i<30000; i++) { }
	#ifndef CC_UBOOT
	HalCriticalEnd(u4State);
	#endif /* CC_UBOOT */	
#endif

#if defined(CONFIG_IRRX_COMMON_DRIVER_BD)
	#if (CONFIG_ARM2_EJECT)
	BIM_GETHWSemaphore(HSMPHE_SINFO4, 0);  
	BIM_WRITE32(REG_RW_SINFO4_REG, (BIM_READ32(REG_RW_SINFO4_REG) | FB_ARM1_IRRX_INIT));
	BIM_ReleaseHWSemaphore(HSMPHE_SINFO4);
	#endif
	
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8563)
	if((IR_READ32(IRRX_PDSTAT) & IR_IR_WAK) != 0)
	{
	    LOG(5, "[Wake up] IR wake up!!\n");
	    IR_WRITE32(IRRX_PDSTCLR, 0x01); 
	}
	IR_WRITE32(IRRX_IREXP_EN, 0);	
	IR_WRITE32(IRRX_WAKEN, (IR_READ32(IRRX_WAKEN) & (~IR_WAKEN))); 
	IRHW_RxWrConf(i4Config, i4SaPeriod, i4Threshold);
	IR_WRITE32(IRRX_CLKPDN, (IR_READ32(IRRX_CLKPDN) & (~IRRXPD)));
#if (IRRX_USE_27M)   
	//set to 27M 
	IR_WRITE32(IRRX_UP_CFG, (IR_READ32(IRRX_UP_CFG) | FAST_CK_EN));
	IR_WRITE32(IRRX_IRCKSEL, (IR_READ32(IRRX_IRCKSEL) & (~IRCLKSEL_MASK)) | CLK_SEL_IR_DIV_1_256);
#else
	IR_WRITE32(IRRX_UP_CFG, (IR_READ32(IRRX_UP_CFG) & (~FAST_CK_EN)));
	
	#if (IRRX_RC_PROTOCOL == IRRX_RC_RC5)	  
	IR_WRITE32(IRRX_IRCKSEL, (IR_READ32(IRRX_IRCKSEL) & (~IRCLKSEL_MASK)) | CLK_SEL_IR_DIV_1_16);	
	#else	  
	IR_WRITE32(IRRX_IRCKSEL, (IR_READ32(IRRX_IRCKSEL) & (~IRCLKSEL_MASK)) | CLK_SEL_IR_DIV_1_8);    
	#endif
#endif
	i4Ret = _IRHW_RxSetIsr((BOOL) TRUE);
	//enable IR_int
	IR_WRITE32(IRRX_INTEN, (IR_READ32(IRRX_INTEN) | IR_INTEN));
#endif

#endif

    return i4Ret;
}

//-----------------------------------------------------------------------------
// Add by ji.wang for BD code
//-----------------------------------------------------------------------------
#if defined(CONFIG_IRRX_COMMON_DRIVER_BD)
#if CONFIG_SUSPEND_TO_DRAM
void IR_RxEnable(BOOL fgEnable)
{
    if(fgEnable)
    {  
        IR_WRITE32(IRRX_CLKPDN, (IR_READ32(IRRX_CLKPDN) & (~IRRXPD)));
        IR_WRITE32(IRRX_CONFIG_HIGH_REG, (IR_READ32(IRRX_CONFIG_HIGH_REG) | IRRX_CH_HWIR));
 	    IR_WRITE32(IRRX_INTEN, (IR_READ32(IRRX_INTEN) | IR_INTEN));
    }
    else
    {
        IR_WRITE32(IRRX_INTEN, (IR_READ32(IRRX_INTEN) & (~IR_INTEN)));
        IR_WRITE32(IRRX_CONFIG_HIGH_REG, (IR_READ32(IRRX_CONFIG_HIGH_REG) & (~IRRX_CH_HWIR)));
        IR_WRITE32(IRRX_CLKPDN, (IR_READ32(IRRX_CLKPDN) | IRRXPD));   
    }
}
#endif


INT32 i4IrHWUninit(void)
{
    INT32 i4Ret;
    i4Ret = IRHW_RxStop();


#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8563)
    IR_WRITE32(IRRX_CONFIG_HIGH_REG, (IR_READ32(IRRX_CONFIG_HIGH_REG) | IRRX_CH_DISPD));
#endif

    IR_WRITE32(IRRX_CLKPDN, (IR_READ32(IRRX_CLKPDN) | IRRXPD));

#if (CONFIG_ARM2_EJECT)
    BIM_GETHWSemaphore(HSMPHE_SINFO4, 0);
  #if(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8563)
    BIM_WRITE32(REG_RW_SINFO4_REG, (BIM_READ32(REG_RW_SINFO4_REG) & (~FB_ARM1_IRRX_INIT)));
  #endif
    BIM_ReleaseHWSemaphore(HSMPHE_SINFO4);
#endif
  
    return i4Ret;
}


/*! \fn void IRHW_SetEnable(BOOL fgEnable)
    */
void IRHW_SetEnable(BOOL fgEnable)
{
    _fgIREnable = fgEnable;
}


#if (CONFIG_DRV_LINUX && CONFIG_ARM2_EJECT)
extern INT32 i4IdeCliTrayOpenClose(void);

#if (CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN)
#include <linux/module.h>
INT32 (*_pi4IdeCliTrayOpenClose)(void);
void IRHW_Set_IdeCliTrayOpenClose(INT32 (*pi4IdeCliTrayOpenClose)(void))
{
  _pi4IdeCliTrayOpenClose = pi4IdeCliTrayOpenClose;
}
EXPORT_SYMBOL(IRHW_Set_IdeCliTrayOpenClose);
#endif

/*! \fn void IRHW_FastejectKeyNotify(void)
    */


void IRHW_FastejectKeyNotify(void)
{

    if(!(BIM_READ32(REG_RW_SINFO4_REG) & FB_ARM1_SATA_INIT))
    {
        BIM_GETHWSemaphore(HSMPHE_SINFO4, 0);
   
        BIM_WRITE32(REG_RW_SINFO4_REG, (BIM_READ32(REG_RW_SINFO4_REG) | FB_ARM1_EJECT_KEY));
         BIM_ReleaseHWSemaphore(HSMPHE_SINFO4);

        LOG(0, "[IRRX]%s arm2 eject\n", __FUNCTION__);
    }
    else //if(/*misc not init*/)
    {
        //call sata func
#if (CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN)
	        if(_pi4IdeCliTrayOpenClose != NULL)
                _pi4IdeCliTrayOpenClose();
#else
        i4IdeCliTrayOpenClose();
#endif            
        LOG(0, "[IRRX]%s arm1 eject\n", __FUNCTION__);
    }
}

#endif


#endif



