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
 * $RCSfile: pdwnc_gpio.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file pdwnc_gpio.c
 *  pdwnc_gpio.c The PDWNC (Power DoWN Controller) GPIO functions
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "x_pinmux.h"
#include "x_pdwnc.h"
#include "x_bim.h"
#include "x_assert.h"
#include "x_os.h"
#include "x_ckgen.h"
#include "x_timer.h"
#include "x_hal_arm.h"
#include "x_gpio.h"
#include "drvcust_if.h"
#include "pdwnc_config.h"
#include "pdwnc_debug.h"
#include "pdwnc_hw.h"
#include "pdwnc_t8032.h"
#include "pdwnc_srvad.h"
#include "pdwnc_if.h"
#include "sif_if.h"
#ifndef CC_MTK_LOADER
#ifdef __KERNEL__
#include <linux/module.h>
#ifdef CONFIG_MT53XX_NATIVE_GPIO
#include <mach/mtk_gpio.h>
#else
#include "cb_data.h"
#include "mtperipheral.h"
#endif
#endif
#endif

#undef PDWNC_WRITE32
#undef PDWNC_READ32

//-----------------------------------------------------------------------------
// Configuration
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

//#define PDWNC_GPIO_INT0_BASE 0
//#define PDWNC_GPIO_INT_GROUP_0_NUM 8
//#define PDWNC_GPIO_INT1_BASE 16
//#define PDWNC_GPIO_INT_GROUP_1_NUM 4
//#define PDWNC_GPIO_INT2_BASE 26
//#define PDWNC_GPIO_INT_GROUP_2_NUM 2
//#define PDWNC_GPIO_INT3_BASE 32
//#define PDWNC_GPIO_INT_GROUP_3_NUM 6

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

#ifndef CC_MTK_LOADER
// PDWNC module sub-interrupt routine list.
static PFN_GPIO_CALLBACK _afnPDWNCGpioCallback[TOTAL_PDWNC_GPIO_NUM];

static const INT8 _ai1PdwncInt2Gpio[TOTAL_PDWNC_GPIO_INT_NUM] =
{
   	0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  //0~9
   10, 11, 12, 13, 14, 15, 16, 17, 18, 19,  //10~19
   20, 30, 31, 32, 33, 34, 35, 36, 37, 38,  //20~29
   39, 40, 48, 54, 55, 56, 57, 58, 60, 61,  //30~49
   62, 63, 64, 65, 66, 67, 68, 69, 70, 71,  //40~49
   72, 73, 74, 								//50~52
};

#endif

static const INT8 _ai1PdwncGpio2Int[TOTAL_PDWNC_GPIO_NUM] =
{
	  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, //0~9
	 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, //10~19
	 20, -1, -1, -1, -1, -1, -1, -1, -1, -1, //20~29
	 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, //30~39
	 31, -1, -1, -1, -1, -1, -1, -1, 32, -1, //40~49
	 -1, -1, -1, -1, 33, 34, 35, 36, 37, -1, //50~59
	 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, //60~69
	 48, 49, 50, 51, 52,                     //70~74
   
};

static const INT8 _ai1PdwncGpio2Wak[TOTAL_PDWNC_GPIO_NUM] =
{
	  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, //0~9    , OPCTRL 0~7
	 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, //10~19  , OPCTRL 8~20
	 20, -1, -1, -1, -1, -1, -1, -1, -1, -1, //20~29  
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //30~39  
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //40~49
	 -1, -1, -1, -1, 21, 22, 23, 24, 25, -1, //50~59  , LED_PWM 0~3, MHL_SENCE
	 -1, -1, 26, 27, 28, 29, 30, 31, 32, 33, //60~69  , TCON 0~12
	 34, 35, 36, 37, 38,                     //70~74
};


#if 0//!defined(CC_MTK_LOADER) || (defined(__KERNEL__) && !defined(CONFIG_MT53XX_NATIVE_GPIO))
static const INT8 _ai1PdwncInt2Callback[TOTAL_PDWNC_GPIO_INT_NUM] =
{
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  //0~9
   10, 11, 12, 13, 14, 15, 16, 17, 18, 19,  //10~19
   20, 30, 31, 32, 33, 34, 35, 36, 37, 38,  //20~29
   39, 40, 48, 54, 55, 56, 57, 58, 60, 61,  //30~49
   62, 63, 64, 65, 66, 67, 68, 69, 70, 71,  //40~49
   72, 73, 74, 								//50~52
};
#endif

#if defined(__KERNEL__) && !defined(CONFIG_MT53XX_NATIVE_GPIO)
static UINT32 _au4PdwncGpioCallBackArg[TOTAL_PDWNC_GPIO_NUM];
static UINT8 _au4PdwncCallBackFromLinuxAP[TOTAL_PDWNC_GPIO_NUM];
#endif

//-----------------------------------------------------------------------------
// Prototypes 
//-----------------------------------------------------------------------------
//INT32 PDWNC_GpioEnable(INT32 i4Gpio, INT32 *pfgSet);
//INT32 PDWNC_GpioOutput(INT32 i4Gpio, INT32 *pfgSet);
//INT32 PDWNC_GpioInput(INT32 i4Gpio);
#ifndef CC_MTK_LOADER
INT32 PDWNC_GpioIntrq(INT32 i4Gpio, INT32 *pfgSet);
INT32 PDWNC_GpioQuery(INT32 i4Gpio, INT32 *pi4Intr, INT32 *pi4Fall, INT32 *pi4Rise);
INT32 PDWNC_GpioReg(INT32 i4Gpio, GPIO_TYPE eType, PFN_GPIO_CALLBACK pfnCallback);
INT32 PDWNC_T8032GpioReg(INT32 i4Gpio, GPIO_TYPE eType, PFN_GPIO_CALLBACK pfnCallback);
#endif


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
#ifndef CC_MTK_LOADER

#if defined(__KERNEL__) && !defined(CONFIG_MT53XX_NATIVE_GPIO)

extern INT32 _CB_PutEvent(CB_FCT_ID_T eFctId, INT32 i4TagSize, void *pvTag);

#endif

//-----------------------------------------------------------------------------
/** _PdwncGpioIsr() is the interrupt routine for GPIO pins of PDWNC module.
 *  @param u2IntIndex is the gpio number in PDWNC module to trigger interrupt.
 */
//-----------------------------------------------------------------------------
void _PdwncGpioIsr(UINT16 u2IntIndex)
{
#if !defined(CONFIG_MT53XX_NATIVE_GPIO)
    BOOL fgStatus;
    //UNUSED(_ai1PdwncInt2Gpio);
    if (((u2IntIndex < TOTAL_PDWNC_GPIO_INT_NUM)) && (_afnPDWNCGpioCallback[_ai1PdwncInt2Gpio[u2IntIndex]]!=NULL))
    {
        fgStatus = PDWNC_GpioInput((INT32)_ai1PdwncInt2Gpio[u2IntIndex]);
		#ifdef __KERNEL__					 
		if(!_au4PdwncCallBackFromLinuxAP[_ai1PdwncInt2Gpio[u2IntIndex]])
		{
			_afnPDWNCGpioCallback[_ai1PdwncInt2Gpio[u2IntIndex]]((INT32)(_ai1PdwncInt2Gpio[u2IntIndex] + OPCTRL(0)), fgStatus);
		}
		else
		{
			MTGPIO_CB_T rGpioCb;
			rGpioCb.i4Gpio = (INT32)(_ai1PdwncInt2Gpio[u2IntIndex] + OPCTRL(0));
			rGpioCb.fgStatus = fgStatus;
			rGpioCb.u4Pfn = (UINT32)_afnPDWNCGpioCallback[_ai1PdwncInt2Gpio[u2IntIndex]];
			rGpioCb.u4Arg = _au4PdwncGpioCallBackArg[_ai1PdwncInt2Gpio[u2IntIndex]];
			//printf("_PdwncGpioIsr: gpio_%d, fgStatus: %d\n", rGpioCb.i4Gpio, rGpioCb.fgStatus);
			_CB_PutEvent(CB_MTAL_GPIO_TRIGGER, sizeof(MTGPIO_CB_T), &rGpioCb);
		}
		#else
		_afnPDWNCGpioCallback[_ai1PdwncInt2Gpio[u2IntIndex]]((INT32)(_ai1PdwncInt2Gpio[u2IntIndex] + OPCTRL(0)), fgStatus); 
		#endif	
    }
#else
    //UNUSED(_ai1PdwncInt2Gpio);
	if (_ai1PdwncGpio2Int[_ai1PdwncInt2Gpio[u2IntIndex]] != -1)
	{
		mtk_pdwnc_gpio_isr_func(u2IntIndex);
	}
#endif/* CONFIG_MT53XX_NATIVE_GPIO */

    // Still need to clear this non-hook interrupt.
    vIO32Write4B((u2IntIndex <= 31)? PDWNC_EXT_INTCLR_0 : PDWNC_EXT_INTCLR_1,  _PINT(u2IntIndex&0x1F));
}
#endif

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** _PDWNC_InitGpio(). Initialize the interrupt routine for GPIO irq.
 *  @param void
 */
//-----------------------------------------------------------------------------

void PDWNC_GPIO_Suspend(void)
{

}

void PDWNC_GPIO_Resume(void)
{	
    vIO32Write4B(PDWNC_ARM_INTEN, u4IO32Read4B(PDWNC_ARM_INTEN) & (~(REG_RW_PDWNC_INTEN_GPIO_MASK)) );
	//vIO32Write4B(PDWNC_INTCLR, REG_RW_PDWNC_INTEN_GPIO_MASK); //david: just clean gpio int.
	vIO32Write4B(PDWNC_EXINT2ED0, 0);
	vIO32Write4B(PDWNC_EXINTLEV0, 0);
	vIO32Write4B(PDWNC_EXINTPOL0, 0);
	vIO32Write4B(PDWNC_EXINT2ED1, 0);
	vIO32Write4B(PDWNC_EXINTLEV1, 0);
	vIO32Write4B(PDWNC_EXINTPOL1, 0);
	vIO32Write4B(PDWNC_ARM_INTEN_0, 0);
	vIO32Write4B(PDWNC_ARM_INTEN_1, 0);
	vIO32Write4B(PDWNC_EXT_INTCLR_0, 0xFFFFFFFF); //just clean gpio int.
	vIO32Write4B(PDWNC_EXT_INTCLR_1, 0x001FFFFF); //just clean gpio int.
   
}

#if defined(__KERNEL__) && !defined(CONFIG_MT53XX_NATIVE_GPIO)
INT32 PDWNC_GpioSetIntExtParam(INT32 i4Gpio, UINT32 u4Arg)
{
    if(_ai1PdwncGpio2Int[i4Gpio] != -1)
    {
    	_au4PdwncCallBackFromLinuxAP[_ai1PdwncGpio2Int[i4Gpio]] = 1;
        _au4PdwncGpioCallBackArg[_ai1PdwncGpio2Int[i4Gpio]] = u4Arg;
        return 0;
    }
    else
    {
        return -1;        
    }
}

INT32 PDWNC_GpioGetIntExtParam(INT32 i4Gpio, UINT32 *pu4Arg)
{
    if(_ai1PdwncGpio2Int[i4Gpio] != -1)
    {
        *pu4Arg = _au4PdwncGpioCallBackArg[_ai1PdwncGpio2Int[i4Gpio]];
        return 0;
    }
    else
    {
        return -1;        
    }
}
#endif

#ifndef CC_MTK_LOADER
//-----------------------------------------------------------------------------
/** _PDWNC_InitGpio(). Initialize the interrupt routine for GPIO irq.
 *  @param void
 */
//-----------------------------------------------------------------------------
INT32 _PDWNC_InitGpio(void)
{
    //INT32 i4Val;
    //void (* pfnOldIsr)(UINT16);
	
#if !defined(CONFIG_MT53XX_NATIVE_GPIO)
	UINT32 i;
    // Clear callback function array.
    for (i=0; i<TOTAL_PDWNC_GPIO_INT_NUM;i++)
    {
        _afnPDWNCGpioCallback[i] = NULL;
		
		#ifdef __KERNEL__  
        _au4PdwncGpioCallBackArg[i] = 0;
        _au4PdwncCallBackFromLinuxAP[i] = 0;
		#endif
    }
    // Initialize interupt register.
    vIO32Write4B(PDWNC_ARM_INTEN, u4IO32Read4B(PDWNC_ARM_INTEN) & (~(REG_RW_PDWNC_INTEN_GPIO_MASK)) );
	vIO32Write4B(PDWNC_EXINT2ED0, 0);
	vIO32Write4B(PDWNC_EXINTLEV0, 0);
	vIO32Write4B(PDWNC_EXINTPOL0, 0);
	vIO32Write4B(PDWNC_EXINT2ED1, 0);
	vIO32Write4B(PDWNC_EXINTLEV1, 0);
	vIO32Write4B(PDWNC_EXINTPOL1, 0);
	vIO32Write4B(PDWNC_EXT_INTCLR_0, 0xFFFFFFFF); //just clean gpio int.
	vIO32Write4B(PDWNC_EXT_INTCLR_1, 0x001FFFFF); //just clean gpio int.
	
#endif	
    return 0;
}
#endif


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

INT32 PDWNC_GpioRangeCheck(INT32 i4Gpio)
{
	if ((i4Gpio >= OPCTRL(0)) && (i4Gpio <= (TOTAL_PDWNC_GPIO_NUM + OPCTRL(0))))
	{
		return 1;
	}
	else
	{	
		return 0;
	}	
}
//-----------------------------------------------------------------------------
/** PDWNC_GpioEnable() The GPIO input/output mode setting functions. It will
 *  check the i4Gpio and set to related register bit as 0 or 1.  In this 
 *  function, 0 is input mode and 1 is output mode.
 *  @param i4Gpio the gpio number to set or read.
 *  @param pfgSet A integer pointer.  If it's NULL, this function will return
 *  the current mode of gpio number (0 or 1).  If it's not NULL, it must
 *  reference to a integer.  If the integer is 0, this function will set the
 *  mode of the gpio number as input mode, otherwise set as output mode.
 *  @retval If pfgSet is NULL, it return 0 or 1 (0 is input mode, 1 is output
 *          mode.)  Otherwise, return (*pfgSet).
 */
//-----------------------------------------------------------------------------
INT32 PDWNC_GpioEnable(INT32 i4Gpio, INT32 *pfgSet)
{
    UINT32 u4Val;
    INT32 i4Idx;
#ifndef CC_MTK_LOADER
	CRIT_STATE_T rCrit;
#endif
    if ((i4Gpio < 0) || (i4Gpio >= TOTAL_PDWNC_GPIO_NUM))
    {
        return -1;
    }
    // Translate GPIO number to pad setting register offset
    i4Idx = i4Gpio; 
	
#ifndef CC_MTK_LOADER
	rCrit = x_crit_start();
#endif /* CC_MTK_LOADER */

    if (i4Gpio <= 31)
    {
        u4Val = u4IO32Read4B(PDWNC_GPIOEN0);
    }
    else if((i4Gpio>=32) && (i4Gpio<=63))
    {
        u4Val = u4IO32Read4B(PDWNC_GPIOEN1);
    }
	else
	{
		u4Val = u4IO32Read4B(PDWNC_GPIOEN2);
	}

	i4Idx &= 0x1f;
    if (pfgSet == NULL)
    {
#ifndef CC_MTK_LOADER
        x_crit_end(rCrit);         
#endif /* CC_MTK_LOADER */
        return ((u4Val & (1U << i4Idx)) ? 1 : 0);
    }
    u4Val = (*pfgSet) ?
            (u4Val | (1U << i4Idx)) :
            (u4Val & ~(1U << i4Idx));
				
    if (i4Gpio <= 31)
    {
        vIO32Write4B(PDWNC_GPIOEN0, u4Val);
    }
    else if((i4Gpio>=32) && (i4Gpio<=63))
    {
        vIO32Write4B(PDWNC_GPIOEN1, u4Val);
    }
	else
	{
		vIO32Write4B(PDWNC_GPIOEN2, u4Val);
	}


#ifndef CC_MTK_LOADER
        x_crit_end(rCrit);         
#endif /* CC_MTK_LOADER */ 
    return (*pfgSet);
}

//-----------------------------------------------------------------------------
/** PDWNC_GpioOutput() The GPIO output value setting functions. It will check
 *  the i4Gpio and set to related register bit as 0 or 1.
 *  @param i4Gpio the gpio number to set or read.
 *  @param pfgSet A integer pointer.  If it's NULL, this function will return
 *          the bit of gpio number (0 or 1).  If it's not NULL, it must 
 *          reference to a integer.  If the integer is 0, this function 
 *          will set the bit of the gpio number as 0, otherwise set as 1.
 *  @retval If pfgSet is NULL, it return 0 or 1 (the bit value of the gpio
 *          number of output mode.  Otherwise, return (*pfgSet).
 */
//-----------------------------------------------------------------------------
INT32 PDWNC_GpioOutput(INT32 i4Gpio, INT32 *pfgSet)
{
    UINT32 u4Val,u4Val1;
    INT32 i4Idx,i4Val;
#ifndef CC_MTK_LOADER
        CRIT_STATE_T rCrit;
#endif 
    if ((i4Gpio < 0) || (i4Gpio >= TOTAL_PDWNC_GPIO_NUM))
    {
        return -1;
    }

    // Translate GPIO number to pad setting register offset
    i4Idx = i4Gpio;	
	
#ifndef CC_MTK_LOADER
        rCrit = x_crit_start();
#endif /* CC_MTK_LOADER */

	BSP_PinGpioSel(i4Gpio +  OPCTRL(0));
	
	i4Idx &= 0x1f;
	
    if (pfgSet == NULL)	//NULL: for query gpio status, must be GPIO output , but not change pin level
    {
#ifndef CC_MTK_LOADER
        x_crit_end(rCrit);         
#endif /* CC_MTK_LOADER */
		//if input mode, get input value and set the value for output valeu ,then change to be output
		if(PDWNC_GpioEnable(i4Gpio, NULL) == 0)
		{
			//get pin level		
		    if (i4Gpio <= 31)
		    {
		        u4Val1 = IO32ReadFldAlign(PDWNC_GPIOIN0, FLD_PD_GPIO_IN0);		
		    }
		    else if((i4Gpio>=32) && (i4Gpio<=63)) 
		    {
		        u4Val1 = IO32ReadFldAlign(PDWNC_GPIOIN1, FLD_PD_GPIO_IN1);
		    }
			else
			{
				u4Val1 = IO32ReadFldAlign(PDWNC_GPIOIN2, FLD_PD_GPIO_IN2);
			}
			//get current out register setting
			if (i4Gpio <= 31)
			{
				u4Val = u4IO32Read4B(PDWNC_GPIOOUT0);
			}
			else if((i4Gpio>=32) && (i4Gpio<=63))
			{
				u4Val = u4IO32Read4B(PDWNC_GPIOOUT1);
			}
			else
			{
	
				u4Val = u4IO32Read4B(PDWNC_GPIOOUT2);
			}
			u4Val = (u4Val1 & (1U << i4Idx)) ?
						(u4Val | (1U << i4Idx)) :
						(u4Val & ~(1U << i4Idx));
			if (i4Gpio <= 31)
			{
				vIO32Write4B(PDWNC_GPIOOUT0, u4Val);
			}
			else if((i4Gpio>=32) && (i4Gpio<=63))
			{
				vIO32Write4B(PDWNC_GPIOOUT1, u4Val);
			}
			else
			{
				vIO32Write4B(PDWNC_GPIOOUT2, u4Val);
			}
			// Set the output mode.
			i4Val = 1;
			VERIFY(1 == PDWNC_GpioEnable(i4Gpio, &i4Val));

		}
		
	//get out value
		if (i4Gpio <= 31)
		{
			u4Val = u4IO32Read4B(PDWNC_GPIOOUT0);
		}
		else if((i4Gpio>=32) && (i4Gpio<=63))
		{
			u4Val = u4IO32Read4B(PDWNC_GPIOOUT1);
		}
		else
		{
			u4Val = u4IO32Read4B(PDWNC_GPIOOUT2);
		}

        return ((u4Val & (1U << i4Idx)) ? 1 : 0);
    }

//get out value
	if (i4Gpio <= 31)
	{
		u4Val = u4IO32Read4B(PDWNC_GPIOOUT0);
	}
	else if((i4Gpio>=32) && (i4Gpio<=63))
	{
		u4Val = u4IO32Read4B(PDWNC_GPIOOUT1);
	}
	else
	{
		u4Val = u4IO32Read4B(PDWNC_GPIOOUT2);
	}

    u4Val = (*pfgSet) ?
                (u4Val | (1U << i4Idx)) :
                (u4Val & ~(1U << i4Idx));
				
    if (i4Gpio <= 31)
    {
        vIO32Write4B(PDWNC_GPIOOUT0, u4Val);
    }
    else if((i4Gpio>=32) && (i4Gpio<=63))
    {
        vIO32Write4B(PDWNC_GPIOOUT1, u4Val);
    }
	else
	{
		vIO32Write4B(PDWNC_GPIOOUT2, u4Val);
	}

#ifndef CC_MTK_LOADER
        x_crit_end(rCrit);         
#endif /* CC_MTK_LOADER */

    // Set the output mode.
    i4Val = 1;
    VERIFY(1 == PDWNC_GpioEnable(i4Gpio, &i4Val));
 
    return (*pfgSet);
}

//-----------------------------------------------------------------------------
/** PDWNC_GpioInput()  The GPIO input reading functions. It will check the 
 *  i4Gpio and read related register bit to return.
 *  @param i4Gpio the gpio number to read.
 *  @retval 0 or 1.  (GPIO input value.)
 */
//-----------------------------------------------------------------------------
INT32 PDWNC_GpioInput(INT32 i4Gpio)
{
    UINT32 u4Val;
    INT32 i4Idx = 0;

    if ((i4Gpio < 0) || (i4Gpio >= TOTAL_PDWNC_GPIO_NUM))
    {
        return -1;
    }
    // Translate GPIO number to pad setting register offset
    i4Idx = i4Gpio;    
//    _PdwncSetGpioPin(i4Gpio);
    BSP_PinGpioSel(i4Gpio +  OPCTRL(0));

	i4Idx &= 0x1f;
    if (i4Gpio <= 31)
    {
        u4Val = IO32ReadFldAlign(PDWNC_GPIOIN0, FLD_PD_GPIO_IN0);		
		return ((u4Val & (1U << i4Idx)) ? 1U : 0);	
    }
    else if((i4Gpio>=32) && (i4Gpio<=63))
    {
        u4Val = IO32ReadFldAlign(PDWNC_GPIOIN1, FLD_PD_GPIO_IN1);
		return ((u4Val & (1U << i4Idx)) ? 1U : 0);	
    }
	else
	{
		u4Val = IO32ReadFldAlign(PDWNC_GPIOIN2, FLD_PD_GPIO_IN2);
		return ((u4Val & (1U << i4Idx)) ? 1U : 0);
	}
}

INT32 PDWNC_GpioStatus(INT32 i4Gpio, INT32* pi4Pinmux, INT32* pi4Enable, INT32* pi4Output, INT32* pi4Input)
{
    UINT32 u4Val;
    INT32 i4Idx = 0;


    if ((i4Gpio < 0) || (i4Gpio >= TOTAL_PDWNC_GPIO_NUM))
    {
        return -1;
    }
    
    *pi4Pinmux = BSP_PinGpioGet(i4Gpio +  OPCTRL(0), &u4Val);

    // Translate GPIO number to pad setting register offset
    i4Idx = i4Gpio;
	i4Idx &= 0x1f;

    if (i4Gpio <= 31)
    {
        u4Val = u4IO32Read4B(PDWNC_GPIOEN0);
    }
    else if((i4Gpio>=32) && (i4Gpio<=63))
    {
        u4Val = u4IO32Read4B(PDWNC_GPIOEN1);
    }
	else
	{
		u4Val = u4IO32Read4B(PDWNC_GPIOEN2);
	}	
	
    *pi4Enable = ((u4Val & (1U << i4Idx)) ? 1 : 0);

    if (i4Gpio <= 31)
    {
        u4Val = u4IO32Read4B(PDWNC_GPIOOUT0);
    }
    else if((i4Gpio>=32) && (i4Gpio<=63))
    {
        u4Val = u4IO32Read4B(PDWNC_GPIOOUT1);
    }
	else
	{
	
		u4Val = u4IO32Read4B(PDWNC_GPIOOUT2);
	}
    *pi4Output = ((u4Val & (1U << i4Idx)) ? 1 : 0);

    if (i4Gpio <= 31)
    {
        u4Val = IO32ReadFldAlign(PDWNC_GPIOIN0, FLD_PD_GPIO_IN0);		
    }
    else if((i4Gpio>=32) && (i4Gpio<=63))
    {
        u4Val = IO32ReadFldAlign(PDWNC_GPIOIN1, FLD_PD_GPIO_IN1);
    }
	else
	{
		u4Val = IO32ReadFldAlign(PDWNC_GPIOIN2, FLD_PD_GPIO_IN2);
	}
    *pi4Input = ((u4Val & (1U << i4Idx)) ? 1U : 0);
	
    return 0;
}


#ifndef CC_MTK_LOADER
//-----------------------------------------------------------------------------
/** PDWNC_GpioIntrq() The GPIO interrupt enable setting functions. It will
 *  check the i4Gpio and set to related register bit as 0 or 1.  In this 
 *  function, 0 is interrupt disable mode and 1 is interrupt enable mode.
 *  @param i4Gpio the gpio number to set or read.
 *  @param pfgSet A integer pointer.  If it's NULL, this function will return
 *  the current setting of gpio number (0 or 1).  If it's not NULL, it must
 *  reference to a integer.  If the integer is 0, this function will set the
 *  mode of the gpio number as interrupt disable mode, otherwise set as
 *  interrupt enable mode.
 *  @retval If pfgSet is NULL, it return 0 or 1 (0 is interrupt disable mode,
 *          1 is interrupt enable mode.)  Otherwise, return (*pfgSet).
 */
//-----------------------------------------------------------------------------
INT32 PDWNC_GpioIntrq(INT32 i4Gpio, INT32 *pfgSet)
{
    UINT32 u4Val;
    INT32 i4Int;
    UNUSED(_ai1PdwncGpio2Int);
    if ((i4Gpio < 0) || (i4Gpio >= TOTAL_PDWNC_GPIO_NUM))
    {
        return -1;
    }
    if((i4Int = _ai1PdwncGpio2Int[i4Gpio]) < 0)
    {
        return -1;    
    }
	if(i4Int <= 31)
	{
		u4Val = u4IO32Read4B(PDWNC_ARM_INTEN_0);
	}
	else
	{
		u4Val = u4IO32Read4B(PDWNC_ARM_INTEN_1);
	}
    if (pfgSet == NULL)
    {
        return ((u4Val & (1U << (i4Int&0x1F))) ? 1 : 0);
    }
    u4Val = (*pfgSet) ?
            (u4Val | OPCTRL_INTEN(i4Int&0x1F)) :
            (u4Val & ~ OPCTRL_INTEN(i4Int&0x1F));
	if(i4Int <= 31)
	{
		vIO32Write4B(PDWNC_EXT_INTCLR_0, OPCTRL_INTEN(i4Int)); //just clean gpio int.
		vIO32Write4B(PDWNC_ARM_INTEN_0, u4Val);
	}
	else
	{
		vIO32Write4B(PDWNC_EXT_INTCLR_1, OPCTRL_INTEN(i4Int&0x1F)); //just clean gpio int.
		vIO32Write4B(PDWNC_ARM_INTEN_1, u4Val);
	}

    return (*pfgSet);
}

//-----------------------------------------------------------------------------
/* PDWNC_GpioQuery() to query PDWNC Gpio settings.
 *  @param i4Gpio should be between 0~7.
 *  @param pi4Intr to get interrupt enable/disable status.
 *  @param pi4Fall to get falling callback enable/disable status.
 *  @param pi4Rise to get rising callback enable/disable status.
 *  @retval return 0 if successful, otherwise return -1.
 */
//-----------------------------------------------------------------------------
INT32 PDWNC_GpioQuery(INT32 i4Gpio, INT32 *pi4Intr, INT32 *pi4Fall, INT32 *pi4Rise)
{
    return 0;
}

//-----------------------------------------------------------------------------
/* PDWNC_GpioReg() to setup the PDWNC Gpio interrupt callback function, type,
 *      and state.
 *  @param i4Gpio should be between 0~7.
 *  @param eType should be one of enum GPIO_TYPE.
 *  @param pfnCallback the callback function.
 *  @retval 0 successful, -1 failed.
 */
//-----------------------------------------------------------------------------

INT32 PDWNC_T8032GpioReg(INT32 i4Gpio, GPIO_TYPE eType, PFN_GPIO_CALLBACK pfnCallback)
{
	UINT32 u4Exint2ed,u4Exintlev,u4Exintpol;
    UINT32 u4Val,i4Idx;
    INT32 i4Int;

	if(i4Gpio >= 200)
		i4Gpio -= 200;
    if ((i4Gpio < 0) || (i4Gpio >= TOTAL_PDWNC_GPIO_NUM))
    {
        return -1;
    }

    if((i4Int = _ai1PdwncGpio2Int[i4Gpio]) < 0)    
    {
        return -1;        
    }
	u4Exint2ed = u4IO32Read4B(((i4Int <= 31)? PDWNC_EXINT2ED0 : PDWNC_EXINT2ED1));
	u4Exintlev = u4IO32Read4B(((i4Int <= 31)? PDWNC_EXINTLEV0 : PDWNC_EXINTLEV1));
	u4Exintpol = u4IO32Read4B(((i4Int <= 31)? PDWNC_EXINTPOL0 : PDWNC_EXINTPOL1));
	i4Idx = i4Int & 0x1f;
     /* Set the register and callback function. */
    switch(eType)
    {
        case GPIO_TYPE_INTR_FALL:
            u4Exint2ed &= ~(1U << i4Idx);  // Disable double edge trigger.
            u4Exintlev &= ~(1U << i4Idx);  // Set Edge trigger.
            u4Exintpol &= ~(1U << i4Idx);  // Set Falling Edge interrupt.
            break;
            
        case GPIO_TYPE_INTR_RISE:            
            u4Exint2ed &= ~(1U << i4Idx);  // Disable double edge trigger.
            u4Exintlev &= ~(1U << i4Idx);  // Set Edge trigger.
            u4Exintpol |= (1U << i4Idx);   // Set rasing Edge interrupt.
            break;               
            
        case GPIO_TYPE_INTR_BOTH:            
            u4Exint2ed |= (1U << i4Idx);   // enable double edge trigger.
            u4Exintlev &= ~(1U << i4Idx);  // Set Edge trigger.
            break;         
        case GPIO_TYPE_INTR_LEVEL_LOW:            
            u4Exint2ed &= ~(1U << i4Idx); // Disable double edge trigger.
            u4Exintlev |= (1U << i4Idx);  // Set Level trigger.
            u4Exintpol &= ~(1U << i4Idx); // Set level low interrupt.
            break;  
        case GPIO_TYPE_INTR_LEVEL_HIGH:            
            u4Exint2ed &= ~(1U << i4Idx); // Disable double edge trigger.
            u4Exintlev |= (1U << i4Idx);  // Set level trigger.
            u4Exintpol |= (1U << i4Idx);  // Set level high interrupt.
            break;         
        default:
			u4Val = u4IO32Read4B((i4Int <= 31)?PDWNC_T8032_EXT_INTEN_0:PDWNC_T8032_EXT_INTEN_1);
			u4Val = (u4Val & ~ OPCTRL_INTEN(i4Int&0x1F)); 	   // Disable interrupt.
			vIO32Write4B((i4Int <= 31)?PDWNC_T8032_EXT_INTEN_0:PDWNC_T8032_EXT_INTEN_1, u4Val);
			return 0;
    }
	vIO32Write4B(((i4Int <= 31)? PDWNC_EXINT2ED0 : PDWNC_EXINT2ED1), u4Exint2ed);
	vIO32Write4B(((i4Int <= 31)? PDWNC_EXINTLEV0 : PDWNC_EXINTLEV1), u4Exintlev);
	vIO32Write4B(((i4Int <= 31)? PDWNC_EXINTPOL0 : PDWNC_EXINTPOL1), u4Exintpol);
	
	BSP_PinGpioSel(i4Gpio + OPCTRL(0));//pinmux 

	u4Val = u4IO32Read4B((i4Int <= 31)?PDWNC_T8032_EXT_INTEN_0:PDWNC_T8032_EXT_INTEN_1);
	u4Val = (u4Val | OPCTRL_INTEN(i4Int&0x1F)); 	   // Eenable interrupt.
	vIO32Write4B((i4Int <= 31)?PDWNC_T8032_EXT_INTEN_0:PDWNC_T8032_EXT_INTEN_1, u4Val);

	UNUSED(pfnCallback);
	#if 0  
    if (pfnCallback)
    {
        //_afnPDWNCGpioCallback[i4Gpio] = pfnCallback;
		_afnPDWNCGpioCallback[_ai1PdwncInt2Callback[_ai1PdwncGpio2Int[i4Gpio]]] = pfnCallback;
    }
	#endif
	
    return 0;
}


INT32 PDWNC_GpioReg(INT32 i4Gpio, GPIO_TYPE eType, PFN_GPIO_CALLBACK pfnCallback)
{
    UINT32 u4Exint2ed;
	UINT32 u4Exintlev;
	UINT32 u4Exintpol;
	UINT32 u4Val;
    INT32 i4Int,i4Idx;

	if(i4Gpio >= 200)
		i4Gpio -= 200;
	
    if ((i4Gpio < 0) || (i4Gpio >= TOTAL_PDWNC_GPIO_NUM))
    {
        return -1;
    }
	
#ifndef CC_FAST_INIT
    if((eType != GPIO_TYPE_NONE) && (pfnCallback == NULL))
    {
        return -1;        
    }
#endif
    if((i4Int = _ai1PdwncGpio2Int[i4Gpio]) < 0)    
    {
        return -1;        
    }
	
	u4Exint2ed = u4IO32Read4B(((i4Int <= 31)? PDWNC_EXINT2ED0 : PDWNC_EXINT2ED1));
	u4Exintlev = u4IO32Read4B(((i4Int <= 31)? PDWNC_EXINTLEV0 : PDWNC_EXINTLEV1));
	u4Exintpol = u4IO32Read4B(((i4Int <= 31)? PDWNC_EXINTPOL0 : PDWNC_EXINTPOL1));
	i4Idx = i4Int & 0x1f;
     /* Set the register and callback function. */
    switch(eType)
    {
        case GPIO_TYPE_INTR_FALL:
            u4Exint2ed &= ~(1U << i4Idx);  // Disable double edge trigger.
            u4Exintlev &= ~(1U << i4Idx);  // Set Edge trigger.
            u4Exintpol &= ~(1U << i4Idx);  // Set Falling Edge interrupt.
            break;
            
        case GPIO_TYPE_INTR_RISE:            
            u4Exint2ed &= ~(1U << i4Idx);  // Disable double edge trigger.
            u4Exintlev &= ~(1U << i4Idx);  // Set Edge trigger.
            u4Exintpol |= (1U << i4Idx);   // Set rasing Edge interrupt.
            break;               
            
        case GPIO_TYPE_INTR_BOTH:            
            u4Exint2ed |= (1U << i4Idx);   // enable double edge trigger.
            u4Exintlev &= ~(1U << i4Idx);  // Set Edge trigger.
            break;         
        case GPIO_TYPE_INTR_LEVEL_LOW:            
            u4Exint2ed &= ~(1U << i4Idx); // Disable double edge trigger.
            u4Exintlev |= (1U << i4Idx);  // Set Level trigger.
            u4Exintpol &= ~(1U << i4Idx); // Set level low interrupt.
            break;  
        case GPIO_TYPE_INTR_LEVEL_HIGH:            
            u4Exint2ed &= ~(1U << i4Idx); // Disable double edge trigger.
            u4Exintlev |= (1U << i4Idx);  // Set level trigger.
            u4Exintpol |= (1U << i4Idx);  // Set level high interrupt.
            break;         
        default:
			u4Val = 0;
			PDWNC_GpioIntrq(i4Int,(INT32 *)&u4Val);// Disable interrupt.
			return 0;
    }
	vIO32Write4B(((i4Int <= 31)? PDWNC_EXINT2ED0 : PDWNC_EXINT2ED1), u4Exint2ed);
	vIO32Write4B(((i4Int <= 31)? PDWNC_EXINTLEV0 : PDWNC_EXINTLEV1), u4Exintlev);
	vIO32Write4B(((i4Int <= 31)? PDWNC_EXINTPOL0 : PDWNC_EXINTPOL1), u4Exintpol);
	
	BSP_PinGpioSel(i4Gpio + OPCTRL(0));//pinmux 

	u4Val = 1;
	PDWNC_GpioIntrq(i4Gpio,(INT32 *)&u4Val);// Enable interrupt.

    if (pfnCallback)
    {
        _afnPDWNCGpioCallback[i4Gpio] = pfnCallback;
    }
	//Printf("Interrupt seting is:0x%8x,0x%8x,0x%8x\n",u4IO32Read4B(PDWNC_EXINT2ED),u4IO32Read4B(PDWNC_EXINTLEV),u4IO32Read4B(PDWNC_EXINTPOL));
    return 0;
}
/*
INT32 PDWNC_GpioResumeINTEN(INT32 i4Gpio, GPIO_TYPE eType)
{
//note: At resume stage, pass NULL will not overwrite _afnPDWNCGpioCallback[] which setup at initial stage    
    return PDWNC_GpioReg(i4Gpio, eType, NULL);
}
*/
#else

INT32 PDWNC_GpioIntrq(INT32 i4Gpio, INT32 *pfgSet)
{
	return -1;
}
INT32 PDWNC_GpioQuery(INT32 i4Gpio, INT32 *pi4Intr, INT32 *pi4Fall, INT32 *pi4Rise)
{
	return -1;
}
INT32 PDWNC_GpioReg(INT32 i4Gpio, GPIO_TYPE eType, PFN_GPIO_CALLBACK pfnCallback)
{
	return -1;
}
/*
INT32 PDWNC_T8032GpioReg(INT32 i4Gpio, GPIO_TYPE eType, PFN_GPIO_CALLBACK pfnCallback)
{
	return -1;
}
*/
INT32 PDWNC_GpioResumeINTEN(INT32 i4Gpio, GPIO_TYPE eType)
{
	return -1;
}

#endif /* CC_MTK_LOADER */

//-----------------------------------------------------------------------------
/** _PdwncSetupGPIO() to setup PDWNC module GPIO wakeup settings
 *  @param pu4PowerDownEn is the power down wakeup enable register value.
 */
//-----------------------------------------------------------------------------
void _PDWNC_SetupGPIO(UINT32 *pu4PowerDownEn)
{
    INT32 i, i4Val;
    UINT32 u4Val, u4Gpio, u4DkBit;
    SGL_GPIO_T   *prSingleGpioKeyList = NULL;
    GREED_KEY_T  *prGreedGpioKeyList = NULL;

    ASSERT(pu4PowerDownEn != NULL);

	// Do the special GPIO wakeup initialization
    if ((DRVCUST_InitQuery(eSpecialWakeupGpio, (UINT32 *)&u4Gpio) == 0) && (DRVCUST_InitQuery(eSpecialWakeupOnVal, (UINT32 *)&u4Val) == 0))
    {
        PDWNC_SetupGpioWakeup(u4Gpio,TRUE,u4Val,FALSE);
    }
	// Do the BT GPIO wakeup initialization
    if ((DRVCUST_InitQuery(eBTWakeupGpio, (UINT32 *)&u4Gpio) == 0) && (DRVCUST_InitQuery(eBTWakeupOnVal, (UINT32 *)&u4Val) == 0))
    {
        PDWNC_SetupGpioWakeup(u4Gpio,TRUE,u4Val,FALSE);
    }

    if (!PDWNC_SetupFpWakeup(NULL))
    {
        LOG(1,"FP wakeup disable\n");
        return;
    }

    u4DkBit = DRVCUST_InitGet(eKeyPadWakeupBit);
    if (DRVCUST_InitQuery(eSingleGpioKeyList, (UINT32 *)(void *)&prSingleGpioKeyList) < 0)
    {
        return;
    }
    if (!((*pu4PowerDownEn) & WAK_IRRX))
    {
        u4DkBit = DKBIT_POWER;
    }

    // Do the GPIO wakeup initialization
    i = 0;
    while ((prSingleGpioKeyList) &&
            (prSingleGpioKeyList[i].u4Key != 0) &&
            (prSingleGpioKeyList[i].u4Key != DKBIT_NONE))
    {
        if (prSingleGpioKeyList[i].u4Key & u4DkBit)
        {
			PDWNC_SetupGpioWakeup(prSingleGpioKeyList[i].u4Gpio,TRUE,prSingleGpioKeyList[i].u4Val,FALSE);
        }
        i++;
    }
    if (DRVCUST_InitQuery(eGreedGpioKeyList, (UINT32 *)(void *)&prGreedGpioKeyList) < 0)
    {
        return;
    }

    // Do the GPIO wakeup initialization
    i = 0;
    while ((prGreedGpioKeyList) &&
            (prGreedGpioKeyList[i].u4Key != 0) &&
            (prGreedGpioKeyList[i].u4Key != DKBIT_NONE))
    {
        if (prGreedGpioKeyList[i].u4Key & u4DkBit)
        {
            u4Gpio = prGreedGpioKeyList[i].u4GpioOut;
            i4Val = (INT32)prGreedGpioKeyList[i].u4Val;
            UNUSED(GPIO_Output((INT32)u4Gpio, &i4Val));
            u4Gpio = prGreedGpioKeyList[i].u4GpioIn - OPCTRL(0);
            if (u4Gpio > TOTAL_PDWNC_DEDICATE_GPI_NUM) //|| _ai1PdwncGpio2Int[u4Gpio] < 0)
            {
                LOG(1, "Error Power button setting\n");
                i++;
                continue;
            }
            u4Val = u4IO32Read4B(PDWNC_PDIO_CFG0);
            if (prGreedGpioKeyList[i].u4Val)
            {
                u4Val |= (GPIOX_POL(u4Gpio));
            }
            else
            {
                u4Val &= ~(GPIOX_POL(u4Gpio));
            }
            vIO32Write4B(PDWNC_PDIO_CFG0, u4Val);
            *pu4PowerDownEn |= GPIO_WAKEN(WAK_GPIOX(u4Gpio));
        }
        i++;
    }
}

//-----------------------------------------------------------------------------
/** PDWNC_SetupGpioWakeup() to setup PDWNC module GPIO wakeup settings
 *  @param fgEnable,fgPolarity,fgStonly is the power down wakeup enable register value.
 */
//-----------------------------------------------------------------------------
INT32 PDWNC_SetupGpioWakeup(INT32 i4GpioNum, BOOL fgEnable, BOOL fgPolarity, BOOL fgStonly)
{
	INT32 i4Int,i4Val;
	UINT32 u4Val;
    if ((i4GpioNum < OPCTRL0) || (i4GpioNum >= (OPCTRL0 + TOTAL_PDWNC_GPIO_NUM)))
    {
    	LOG(0,"gpio num error , OPCTRL .\n");
        return -1;
    }
	if((i4Int = _ai1PdwncGpio2Wak[i4GpioNum - OPCTRL0]) < 0)    
    {
    	LOG(0,"gpio[%d] can't support wakeup.\n",i4GpioNum);
        return -1;        
    }
	// Set to input mode.
    i4Val = 0;
    UNUSED(GPIO_Enable((INT32)i4GpioNum, &i4Val));
	//set enable bit
	u4Val = IO32ReadFldAlign((i4Int<8)?PDWNC_WAKEN:PDWNC_WAKEN1,(i4Int<8)?FLD_EXTIO_WAKEN:FLD_EXTIO_WAKEN1);
	if(fgEnable){
		u4Val |= ((i4Int<8)?GPIO_WAKEN(WAK_GPIOX(i4Int)):GPIO_WAKEN_1(WAK_GPIOX(i4Int-8)));
	}
	else{
		u4Val &= ~((i4Int<8)?GPIO_WAKEN(WAK_GPIOX(i4Int)):GPIO_WAKEN_1(WAK_GPIOX(i4Int-8)));
	}
	vIO32WriteFldAlign((i4Int<=7)?PDWNC_WAKEN:PDWNC_WAKEN1,u4Val,(i4Int<8)?FLD_EXTIO_WAKEN:FLD_EXTIO_WAKEN1);
	//set polarity bit
	u4Val = IO32ReadFldAlign((i4Int<16)?PDWNC_PDIO_CFG0:PDWNC_PDIO_CFG1,(i4Int<16)?FLD_EXTIO_POL0:FLD_EXTIO_POL1);
    if (fgPolarity){
        u4Val |= ((i4Int<16)?(GPIOX_POL(i4Int)):(GPIOX_POL(i4Int-16)));
    }
    else{
        u4Val &= ~((i4Int<16)?(GPIOX_POL(i4Int)):(GPIOX_POL(i4Int-16)));
    }
	vIO32WriteFldAlign((i4Int<16)?PDWNC_PDIO_CFG0:PDWNC_PDIO_CFG1,u4Val,(i4Int<16)?FLD_EXTIO_POL0:FLD_EXTIO_POL1);
	//set stonly bit
	u4Val = IO32ReadFldAlign((i4Int<8)?PDWNC_WAKE_STONLY0:PDWNC_WAKE_STONLY1,(i4Int<8)?FLD_EXTIO_WAK_STON0_EN:FLD_WAK_STON1_EN);
	if(fgStonly){
		u4Val |= ((i4Int<8)?GPIO_WAKEN(WAK_GPIOX(i4Int)):GPIO_WAKEN_1(WAK_GPIOX(i4Int-8)));
	}
	else{
		u4Val &= ~((i4Int<8)?GPIO_WAKEN(WAK_GPIOX(i4Int)):GPIO_WAKEN_1(WAK_GPIOX(i4Int-8)));
	}
	vIO32WriteFldAlign((i4Int<8)?PDWNC_WAKE_STONLY0:PDWNC_WAKE_STONLY1,u4Val,(i4Int<8)?FLD_EXTIO_WAK_STON0_EN:FLD_WAK_STON1_EN);

	return 0;
}

INT32 PDWNC_GetWakeupGpioNum(UINT32 *u4GpioNum)
{
	INT32 i;
	INT32 u4Wak = -1;
	UINT32 u4Val;

	VERIFY(u4GpioNum != NULL);
	u4Val = _PDWNC_ReadWakeupStatus(0); 
	if(u4Val & 0xFF) //OPCTRL0 ~ OPCTRL7.
	{
	
		for(i=0;i<8;i++)
		{
			if(u4Val & 0x01)
			{
				u4Wak = i;
				break;
			}
			u4Val = u4Val >> 1;
		}
	}else{
		u4Val = _PDWNC_ReadWakeupStatus(1);
		if(u4Val & 0x7FFFFFFF)
		{
			for(i=0;i<31;i++) //0~30
			{
				if(u4Val & 0x01)
				{
					u4Wak = i + 8;
					break;
				}
				u4Val = u4Val >> 1;
			}
		}
	}
	if(u4Wak != -1)
	{
		//Printf("GPIO wakeup status, u4Wak = %d.\n",u4Wak);
		for(i=0;i<TOTAL_PDWNC_GPIO_NUM;i++)
		{
			if(_ai1PdwncGpio2Wak[i] == u4Wak)
			{
				//Printf("Trigger wakeup GPIO num: %d .\n",OPCTRL0+i);
				*u4GpioNum = (OPCTRL0+i);
				return 0;
			}
		}
	}
	
	return -1;
}


//-----------------------------------------------------------------------------
/** _PDWNC_RegLedBlinker() Register/Unregister an GPIO as LED blinker
 *  @param i4Gpio the gpio idx to be used as LED blinker.
 *  @param fgEnable enable/disable the gpio as LED blinker. 
 *  @param u1RoundPeriod blinking dark/light cycle time (unit: 0.5 sec).  
 *  @param u1LightPeriod blinking dark time in a cycle time (unit: 0.5 sec).   
 *  @retval blinker gpio idx if success; -1 if fail
 */
//-----------------------------------------------------------------------------
INT32 _PDWNC_RegLedBlinker(INT32 i4Gpio, BOOL fgEnable, UINT8 u1RoundPeriod, UINT8 u1LightPeriod)
{
	#if 0 //todo I don't what is this for 
	UINT32 u4Val;
    INT32 i4Val = 1;
    if(i4Gpio > OPCTRL(5) - OPCTRL(0) )
    {
        return -1;
    }

    if(fgEnable)
    {
        UNUSED(PDWNC_GpioEnable(i4Gpio, &i4Val));
        u4Val =  u4IO32Read4B(PDWNC_PDMISC) | (1U << i4Gpio);
        if(u1RoundPeriod == 4 && u1LightPeriod == 2)
        {
            u4Val |= (0x1 << 8);
        }
        else
        {
            u4Val &= ~(0x1 << 8);            
        }
    }
    else
    {
        u4Val = u4IO32Read4B(PDWNC_PDMISC) & ~(1U << i4Gpio);
    }
      
    vIO32Write4B(PDWNC_PDMISC, u4Val);
	#endif
	return i4Gpio;
}


