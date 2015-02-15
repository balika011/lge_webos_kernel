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
 * $Date  $
 * $RCSfile: drv_pwm.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "drv_pwm.h"
#include "x_hal_5381.h"
#include "x_pinmux.h"
#include "x_assert.h"
#include "nptv_debug.h"
#include "drvcust_if.h"
#include "drv_tdtv_drvif.h"
#include "hw_tcon.h"
#include "hw_mlvds.h"
#include "drv_display.h"
#include "sv_const.h"

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** Brief
 *  @param
 *  @return
 */
//-----------------------------------------------------------------------------
EXTERN UINT8 bSI_DISPLAY_DCLK_TYPE;
UINT8 _bPWMSrc;
extern BOOL fgIsVsyncIsrStart;

#define PWMSCAN_MAX_CH SrcPWM2
typedef struct
{
	UINT8 u1Initialed;	
	UINT32 u4Start;
	UINT32 u4High;
	UINT32 u4Low;
	UINT32 u4High_target;
	UINT32 u4Low_target;	
	UINT32 u4LineTotalPre;
	UINT8 u1CrossZone;	
}SCANPWM_INFO_T;

static SCANPWM_INFO_T PWMSCAN_SETDATA[PWMSCAN_MAX_CH+1];

#define SCANPWM_DEBUGLOG 0

UINT8 fgApplyScanPWMSetData=0;
UINT8 u1ScanPwmCrossFlg=0;
UINT8 fgScanPWMSetpControl=0;
UINT16 u2ScanPWMSetp=2;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** Brief
 * This driver control the PWM0, PWM1 & PWM2 output frequence and duty cycle
 *
 * duty cycle = bDutyON/(bDutyAll+1);
 * PWM freq.(in 5368/96...) = (BUS Clock) / ((PWMP+1)x(PWMRSN+1))
 *
 * @param bSrc      PWM source (SrcPWM0/1/2)
 * @param bTimeBase PWMP, PWM basic unit = (0~0xfffff) * BUS Clock
 * @param bDutyON   PWMH, ON duty width = (0~0xfff) * bTimeBase
 * @param bDutyAll  PWMRSN, Period width = (0~0xffe) * bTimeBase
 * @return void
 *
 * @example  vDrvSetPWM(SrcPWM0, 6, u4DutyON, 254);
 *           	 Duty Cycle = u4DutyON/(254+1)
 */

void vDrvSetPWM(UINT32 u4Src, UINT32 u4TimeBase, UINT32 u4DutyON,
                UINT32 u4DutyAll)
{
    UINT32 u4Reg = REG_RW_PWM1, u4Value;

    if(u4DutyON > (u4DutyAll+1))
    {
        Printf("PWM Error: u4DutyON and u4DutyAll= [0x %x,0x %x], u4DutyON is too large\n", u4DutyON, u4DutyAll);
    }
		switch(u4Src)
		{
			case SrcPWM0:
				u4Reg = REG_RW_PWM0;
	
				if(DRVCUST_InitGet(ePWM0Port)==PAD_PWM0_PWM0)
				{
					UNUSED(BSP_PinSet(PIN_OPWM0, PINMUX_FUNCTION1));
				}
				else if(DRVCUST_InitGet(ePWM0Port)==PAD_PWM0_TCON2)		
				{
					UNUSED(BSP_PinSet(PIN_TCON2, PINMUX_FUNCTION2));
				}
				else if(DRVCUST_InitGet(ePWM0Port)==PAD_PWM0_PDD0)		
				{
					UNUSED(BSP_PinSet(PIN_PDD0, PINMUX_FUNCTION3));
				}
				else if(DRVCUST_InitGet(ePWM0Port)==PAD_PWM0_PARB_)		
				{
					UNUSED(BSP_PinSet(PIN_PARB_, PINMUX_FUNCTION5));
				}
				else if(DRVCUST_InitGet(ePWM0Port)==PAD_PWM0_ADIN2)		
				{
					UNUSED(BSP_PinSet(PIN_ADIN2, PINMUX_FUNCTION2));
				}
				break;
	
			case SrcPWM1:
				u4Reg = REG_RW_PWM1;
	
				if(DRVCUST_InitGet(ePWM1Port)==PAD_PWM1_PWM1)
				{
					UNUSED(BSP_PinSet(PIN_OPWM1, PINMUX_FUNCTION1)); //Tuner 30V Boost-up PWM in BGA
				}
				else if(DRVCUST_InitGet(ePWM1Port)==PAD_PWM1_TCON5)		
				{
					UNUSED(BSP_PinSet(PIN_TCON5, PINMUX_FUNCTION2));
				}
				else if(DRVCUST_InitGet(ePWM1Port)==PAD_PWM1_PDD1)		
				{
					UNUSED(BSP_PinSet(PIN_PDD1, PINMUX_FUNCTION3));
				}
				else if(DRVCUST_InitGet(ePWM1Port)==PAD_PWM1_OPCTRL0)		
				{
					UNUSED(BSP_PinSet(PIN_OPCTRL0, PINMUX_FUNCTION2));
				}
				else if(DRVCUST_InitGet(ePWM1Port)==PAD_PWM1_RF_AGC)		
				{
					UNUSED(BSP_PinSet(PIN_RF_AGC, PINMUX_FUNCTION5));
				}
				else if(DRVCUST_InitGet(ePWM1Port)==PAD_PWM1_GPIO2)		
				{
					UNUSED(BSP_PinSet(PIN_GPIO2, PINMUX_FUNCTION5));
				}
				break;
	
			case SrcPWM2:
				u4Reg = REG_RW_PWM2;
	
				if(DRVCUST_InitGet(ePWM2Port)==PAD_PWM2_PWM2)
				{
					UNUSED(BSP_PinSet(PIN_OPWM2, PINMUX_FUNCTION1)); //LED_CTL in BGA
				}
				else if(DRVCUST_InitGet(ePWM2Port)==PAD_PWM2_TCON8)
				{
					UNUSED(BSP_PinSet(PIN_TCON8, PINMUX_FUNCTION2)); //LED_CTL in BGA
				}
				else if(DRVCUST_InitGet(ePWM2Port)==PAD_PWM2_PDD2)
				{
					UNUSED(BSP_PinSet(PIN_PDD2, PINMUX_FUNCTION3)); //LED_CTL in BGA
				}
				else if(DRVCUST_InitGet(ePWM2Port)==PAD_PWM2_OPCTRL1)
				{
					UNUSED(BSP_PinSet(PIN_OPCTRL1, PINMUX_FUNCTION2)); //LED_CTL in BGA
				}

				break;
	
			default:
				ASSERT(u4Src <= SrcPWM2);
		}
    // workaround for full duty drop issue
    if(u4DutyAll==0xff && u4DutyON==0xff)	//we used 0xff at normal
    {
        u4DutyAll = 0xfe;
    }

    if(u4DutyAll==0xfff && u4DutyON==0xfff)
    {
        u4DutyAll = 0xffe;
    }

    IO_WRITE32MSK(HPDS_BASE, u4Reg+4, u4TimeBase, 0xFFFFF);
    u4Value = (((UINT32)u4DutyAll << 20) |
               ((UINT32)(u4DutyON & 0xFFF) << 4) | 1);
    IO_WRITE32(HPDS_BASE, u4Reg, u4Value);
}

/**
 * Set PWM sync to LVDS output v-sync signal.
 *
 * @param u4Src      PWM source (SrcPWM0/1/2)
 * @param u4LockON   Lock PWM on LVDS v-sync or not.
 */

void vDrvSetLock(UINT32 u4Src, UINT32 u4LockON)
{
    switch(u4Src)
    {
        default:
        case SrcPWM0:
            IO_WRITE32MSK(
                HPDS_BASE,
                REG_RW_PWM0VSYNC,
                u4LockON? 0x80000000 : 0,
                0xC0000000);
            break;

        case SrcPWM1:
            IO_WRITE32MSK(
                HPDS_BASE,
                REG_RW_PWM1VSYNC,
                u4LockON? 0x80000000 : 0,
                0xC0000000);
            break;

        case SrcPWM2:
            IO_WRITE32MSK(
                HPDS_BASE,
                REG_RW_PWM2VSYNC,
                u4LockON? 0x80000000 : 0,
                0xC0000000);
            break;
    }
}

//-----------------------------------------------------------------------------
/** Brief
 * This driver initialize pwm
 */
void vDrvInitSCANPWM(UINT8 u1Src)
{
    vDrvSetScanPWMOnOff(u1Src, SV_OFF);	
    vDrvSetScanPWMOutBL(u1Src, SV_OFF); // SV_OFF: PWM_SCAN, SV_ON: TCON_3DBL
    //vDrvSetScanPWMLatchMode(SCAN_PWM_LATCH_MODE_SETDATA_OFF,SCAN_PWM_LATCH_MODE_OFF,SCAN_PWM_ALIGN_MODE_VSYNC);  // Aligned Vsync
    vDrvSetScanPWMLatchMode(SCAN_PWM_LATCH_MODE_SETDATA,SCAN_PWM_LATCH_MODE_OFF,SCAN_PWM_ALIGN_MODE_VSYNC);  // Set data

    vDrvSetScanBLOnOff(SV_OFF);
    vDrvSetScanPWMPolarity(u1Src, 0); 

    vDrvSetScanStartModeOnOff(u1Src,SV_OFF);
    vDrvSetScanPWMStartPhase(u1Src,0);
    //vDrvSetScanPWM(u1Src,0,0,0);
}

//-----------------------------------------------------------------------------
/** Brief
 * This driver calculates the pwm duty cycle period ratio
 * @return ratio
 */
UINT8 vDrvGetPWMDutyCycleRatio(void)
{
#if defined(CC_MT5398)
    UINT8 u1Ratio, u1FrameRate;
    u1FrameRate = (bSI_DISPLAY_DCLK_TYPE == SV_DCLK_50HZ) ? 50 : 60;
    u1Ratio = (vDrvGetLCDFreq() / u1FrameRate);
    return u1Ratio;
#else
    return 0;
#endif
}


/** Brief
 * This driver control the scanning PWM1 & PWM2
 * duty cycle = u4DutyON/u4DutyAll;
 * @param u1Src  PWM source (ScanPWM1/2/3/4/5/6)
 * @param u4Start  start_period (%) ex: 0 --> 0%
 * @param u4High  high_period (%) ex: 40 --> 40%
 * @param u4Low  low_period (%) ex: 60 --> 60%
 * @u4Start + u4High + u4Low = 100%
 * @return void
 */
static void vDrvUpdataPWMCrossZoneStatus(void)
{
	if ((PWMSCAN_SETDATA[1].u1CrossZone == 2)||(PWMSCAN_SETDATA[2].u1CrossZone == 2))
	{
		u1ScanPwmCrossFlg = 2;
	}	
	else if ((PWMSCAN_SETDATA[1].u1CrossZone == 1)||(PWMSCAN_SETDATA[2].u1CrossZone ==1))
	{
		u1ScanPwmCrossFlg = 1;
	}	
	else
	{
		u1ScanPwmCrossFlg = 0;
	}
}

static void vDrvUpdatePWMCrossZoneStatus2(void)
{
	if ((PWMSCAN_SETDATA[1].u1CrossZone == 0)||(PWMSCAN_SETDATA[2].u1CrossZone == 0))
	{
		u1ScanPwmCrossFlg = 0;
	}	
	else
	{
		u1ScanPwmCrossFlg = 2;
	}	
}

#ifndef CC_MTK_LOADER
static void vDrvSetScanPWMStruct_target(UINT8 u1Src, UINT32 u4High, UINT32 u4Low)
{
	if (u1Src <= PWMSCAN_MAX_CH)
	{
		 PWMSCAN_SETDATA[u1Src].u4High_target = u4High;
		 PWMSCAN_SETDATA[u1Src].u4Low_target = u4Low;	  
	}
}
#endif

UINT8 u1DrvGetScanPWMStruct_Ready(void)
{
	UINT8 u1Ch = 0, u1Ready = 0;;

	for (u1Ch = SrcPWM1; u1Ch <= PWMSCAN_MAX_CH; u1Ch++)
	{
	  if((PWMSCAN_SETDATA[u1Ch].u4High_target == PWMSCAN_SETDATA[u1Ch].u4High)
	  	&& (PWMSCAN_SETDATA[u1Ch].u4Low_target == PWMSCAN_SETDATA[u1Ch].u4Low))
	  {
	   u1Ready |= 1;
	  }
	}
	#if SCANPWM_DEBUGLOG
	Printf("[Nick] u1Ready = %d\n", u1Ready);
	if(!u1Ready)
	{
	 vDrvSetScanPWMSW_SETDATA_Status();	
	}
	#endif
	return(u1Ready);
}

void vDrvSetScanPWMStepControlEn(UINT8 fgEnable)
{
	fgScanPWMSetpControl = fgEnable;
}

void vDrvSetScanPWMStepControl(UINT8 fgEnable, UINT16 u2Step)
{
	vDrvSetScanPWMStepControlEn(fgEnable);
	u2ScanPWMSetp = u2Step;
}

void vDrvSetScanPWMStepTarget(UINT32 u4High, UINT32 u4Low)
{
	PWMSCAN_SETDATA[1].u4High_target= u4High;
	PWMSCAN_SETDATA[2].u4High_target= u4High;	
	PWMSCAN_SETDATA[1].u4Low_target = u4Low;
	PWMSCAN_SETDATA[2].u4Low_target = u4Low;	
	fgApplyScanPWMSetData = 1;
}

void vDrvGetScanPWMStepControlStatus(void)
{
	Printf("fgScanPWMSetpControl=%d\n", fgScanPWMSetpControl);
	Printf("u2ScanPWMSetp=%d\n", u2ScanPWMSetp);	
}

UINT16 u2PWMCrossZoneup = 81; // 36 (sync widthe + VBp) + 45

void vDrvSetCrozzUpLimit(UINT16 u2Value)
{
	if (u2Value >= 10)
		u2PWMCrossZoneup = u2Value-10;
	else
		u2PWMCrossZoneup = 81;
}

UINT16 u2DrvGetCrozzUpLimit(void)
{
	return u2PWMCrossZoneup;
}

static void vDrvSetScanPWMStruct(UINT8 u1Src, UINT32 u4Start, UINT32 u4High, UINT32 u4Low)
{
   UINT8 u1DoubleFrame=0;
   UINT32 u4LineTotalCur = 0,u4LineTotalCur2 = 0;
   UINT16 u2CrossLine = 0, u2CrossLine_top = 0;      

   #if SCANPWM_DEBUGLOG
   Printf("u1Src=%d, u4Start=%d, u4High=%d, u4Low=%d \n",u1Src,u4Start, u4High, u4Low);// nick123
   #endif
   if (u1Src <= PWMSCAN_MAX_CH)
   {
   		if (PWMSCAN_SETDATA[u1Src].u1Initialed != 1)
   		{
			PWMSCAN_SETDATA[u1Src].u4High_target= u4High;
			PWMSCAN_SETDATA[u1Src].u4Low_target = u4Low;	 
   		}
   		PWMSCAN_SETDATA[u1Src].u1Initialed = 1;
   		PWMSCAN_SETDATA[u1Src].u4Start = u4Start;
   		PWMSCAN_SETDATA[u1Src].u4High = u4High;
   		PWMSCAN_SETDATA[u1Src].u4Low = u4Low;    
   
   		if ((u4High+u4Low)<510)
   		{
     		u1DoubleFrame =1;
   		}  

   		if(u1DoubleFrame)
  		{
     		u4LineTotalCur = (u4Start + u4High + u4Low + u4High);
			u4LineTotalCur2 = u4LineTotalCur;
			if(u4LineTotalCur>1000)
			{
				u4LineTotalCur = (u4LineTotalCur - 1000);
			}
			else
			{
				u4LineTotalCur = (u4Start + u4High);
			}
   		}
   		else
   		{
     		u4LineTotalCur = (u4Start + u4High);
			u4LineTotalCur2 = u4LineTotalCur;
			if(u4LineTotalCur>1000)
			{
				u4LineTotalCur = (u4LineTotalCur - 1000);
			}			
   		}   
		u2CrossLine = wDrvGetOutputVTotal()-wDrvGetOutputVActive()-wDrvGetVsyncFp(); // [Apply point1 position] VDE Rising
		u2CrossLine = u2CrossLine * 1000 / wDrvGetOutputVTotal();
		u2CrossLine_top = u2CrossLine + 45;
		vDrvSetCrozzUpLimit(u2CrossLine_top);

		if(fgScanPWMSetpControl)
		{
		// Use apply point 2 instead of apply point 1 to avoid VDE rising set  next step transient
		// if((u4LineTotalCur >= 0) && (u4LineTotalCur <= u2CrossLine_top))  // pointless comparison of unsigned integer with zero
		if(u4LineTotalCur <= u2CrossLine_top)
		{

 			PWMSCAN_SETDATA[u1Src].u1CrossZone = 1;
		}
		else
   		{
     		PWMSCAN_SETDATA[u1Src].u1CrossZone = 0;
   		}   

   		PWMSCAN_SETDATA[u1Src].u4LineTotalPre = u4LineTotalCur2;
   		vDrvUpdataPWMCrossZoneStatus();
		}
		else
		{
			if((u4LineTotalCur < 1000) && (PWMSCAN_SETDATA[u1Src].u4LineTotalPre > 1000))
			  {
			 	PWMSCAN_SETDATA[u1Src].u1CrossZone = 0;
			  }
			  else
			  {
			  	PWMSCAN_SETDATA[u1Src].u1CrossZone = 2;
			  }
			  
	     PWMSCAN_SETDATA[u1Src].u4LineTotalPre = u4LineTotalCur;		
		 vDrvUpdatePWMCrossZoneStatus2(); 
		}
   }
}

void vDrvSetScanPWMSW_StepControl(void)
{
	UINT32 u4HighChg=0, u4LowChg=0;

	UINT8 u1Ch = 0;

	for (u1Ch = SrcPWM1; u1Ch <= PWMSCAN_MAX_CH; u1Ch++)
	{
		if((PWMSCAN_SETDATA[u1Ch].u4High_target != PWMSCAN_SETDATA[u1Ch].u4High)
		|| (PWMSCAN_SETDATA[u1Ch].u4Low_target != PWMSCAN_SETDATA[u1Ch].u4Low))	
		{
			#if SCANPWM_DEBUGLOG
			Printf("High_step=%d, Low_step=%d \n",PWMSCAN_SETDATA[u1Ch].u4High,PWMSCAN_SETDATA[u1Ch].u4Low);
			#endif
			
			if (PWMSCAN_SETDATA[u1Ch].u4High_target >= PWMSCAN_SETDATA[u1Ch].u4High)
			{
				u4HighChg = PWMSCAN_SETDATA[u1Ch].u4High + u2ScanPWMSetp;
				if (u4HighChg > PWMSCAN_SETDATA[u1Ch].u4High_target)
				{
				 u4HighChg = PWMSCAN_SETDATA[u1Ch].u4High_target;	
				} 
			}
			else
			{
				u4HighChg = PWMSCAN_SETDATA[u1Ch].u4High - u2ScanPWMSetp;				
				if (u4HighChg < PWMSCAN_SETDATA[u1Ch].u4High_target)
				{
				 u4HighChg = PWMSCAN_SETDATA[u1Ch].u4High_target;	
				} 				
			}
		
			if (PWMSCAN_SETDATA[u1Ch].u4Low_target >= PWMSCAN_SETDATA[u1Ch].u4Low)
			{
				u4LowChg = PWMSCAN_SETDATA[u1Ch].u4Low + u2ScanPWMSetp;
				if (u4LowChg > PWMSCAN_SETDATA[u1Ch].u4Low_target)
				{
				 u4LowChg = PWMSCAN_SETDATA[u1Ch].u4Low_target;	
				} 				
			}
			else
			{
				u4LowChg = PWMSCAN_SETDATA[u1Ch].u4Low - u2ScanPWMSetp; 			
				if (u4LowChg < PWMSCAN_SETDATA[u1Ch].u4Low_target)
				{
				 u4LowChg = PWMSCAN_SETDATA[u1Ch].u4Low_target;	
				} 								
			}
			#if SCANPWM_DEBUGLOG
			Printf("u4HighChg=%d, u4LowChg=%d \n",u4HighChg,u4LowChg);
			#endif
			
			vDrvSetScanPWMStruct(u1Ch,PWMSCAN_SETDATA[u1Ch].u4Start,u4HighChg,u4LowChg);			
		}
	}
}


void vDrvSetScanPWMSW(UINT8 u1Src, UINT32 u4Start, UINT32 u4High, UINT32 u4Low)
{
#ifndef CC_MTK_LOADER 
    // CRIT_STATE_T csState;
    // csState = x_crit_start();
#endif     

#ifndef CC_MTK_LOADER
	{
	    vDrvSetScanPWMStruct_target(u1Src,u4High,u4Low);
		
		if (!fgScanPWMSetpControl)
		{
 		 vDrvSetScanPWMStruct(u1Src,u4Start,u4High,u4Low);
		}
		
         fgApplyScanPWMSetData = 1;        
		
        //vDrvSetScanPWMLatchMode(SCAN_PWM_LATCH_MODE_OFF);
        vDrvSetScanPWMLatchMode(SCAN_PWM_LATCH_MODE_SETDATA,SCAN_PWM_LATCH_MODE_OFF,SCAN_PWM_ALIGN_MODE_VSYNC);  // Set data
	}	
#else   
    {
    	vDrvSetScanPWM(u1Src, u4Start, u4High, u4Low);
		vDrvSetScanPWMStruct(u1Src,u4Start,u4High,u4Low);
        //vDrvSetScanPWMLatchMode(SCAN_PWM_LATCH_MODE_ON);
        //vDrvSetScanPWMLatchMode(SCAN_PWM_LATCH_MODE_OFF);
        vDrvSetScanPWMLatchMode(SCAN_PWM_LATCH_MODE_SETDATA_OFF,SCAN_PWM_LATCH_MODE_OFF,SCAN_PWM_ALIGN_MODE_ONCE);  // Aligned at once
        vDrvScanPWMDataFire();
    }
#endif



#ifndef CC_MTK_LOADER    
    // x_crit_end(csState);
#endif    

}

void vDrvSetScanPWMSW_SETDATA(void)
{
	if(PWMSCAN_SETDATA[SrcPWM1].u1Initialed)
	{
    	vDrvSetScanPWM(SrcPWM1, PWMSCAN_SETDATA[SrcPWM1].u4Start, PWMSCAN_SETDATA[SrcPWM1].u4High, PWMSCAN_SETDATA[SrcPWM1].u4Low );    
		#if SCANPWM_DEBUGLOG		
		Printf("SrcPWM1 S=%d H=%d L=%d", PWMSCAN_SETDATA[SrcPWM1].u4Start, PWMSCAN_SETDATA[SrcPWM1].u4High, PWMSCAN_SETDATA[SrcPWM1].u4Low);
		#endif		
	}	
	
	if(PWMSCAN_SETDATA[SrcPWM2].u1Initialed)
	{
    	vDrvSetScanPWM(SrcPWM2, PWMSCAN_SETDATA[SrcPWM2].u4Start, PWMSCAN_SETDATA[SrcPWM2].u4High, PWMSCAN_SETDATA[SrcPWM2].u4Low );
		#if SCANPWM_DEBUGLOG
		Printf(", SrcPWM2 S=%d H=%d L=%d\n", PWMSCAN_SETDATA[SrcPWM2].u4Start, PWMSCAN_SETDATA[SrcPWM2].u4High, PWMSCAN_SETDATA[SrcPWM2].u4Low);		
		#endif		
	}
}

void vDrvSetScanPWMSW_SETDATA_Status(void)
{
	UINT8 u1Ch = 0;

	for (u1Ch = SrcPWM1; u1Ch <= PWMSCAN_MAX_CH; u1Ch++)
	{
	 Printf("[SCAN_PWM_%d] u1Initialed=%d, u4Start=%d, u4High=%d,  u4Low=%d , u4High_target=%d,  u4Low_target=%d\n",u1Ch,PWMSCAN_SETDATA[u1Ch].u1Initialed, PWMSCAN_SETDATA[u1Ch].u4Start, PWMSCAN_SETDATA[u1Ch].u4High, PWMSCAN_SETDATA[u1Ch].u4Low, PWMSCAN_SETDATA[u1Ch].u4High_target, PWMSCAN_SETDATA[u1Ch].u4Low_target);	
	 Printf("[SCAN_PWM_%d] u4LineTotalPre=%d, u1CrossZone=%d\n",u1Ch,PWMSCAN_SETDATA[u1Ch].u4LineTotalPre, PWMSCAN_SETDATA[u1Ch].u1CrossZone);		 
	}
	Printf("u2PWMCrossZoneup = %d \n",u2DrvGetCrozzUpLimit());
}

void vDrvSetScanPWM(UINT8 u1Src, UINT32 u4Start, UINT32 u4High, UINT32 u4Low)
{
    UINT16 u2HTotal, u2VTotal;
    UINT32 u4Val;

	vDrvSetScanPWMOnOff(u1Src,SV_ON);
	
	u4Val = u4High + u4Low;
	if (u4Val>100)
	{
		return;
	}
    
    u2HTotal = wDrvGetOutputHTotal();
    u2VTotal = wDrvGetOutputVTotal();
    u4Val = u2HTotal * u2VTotal / 100;

	LOG(3, "_|-|_ vDrvSetScanPWM [SRC:scanPWM%d,%d*%d,start:%d%,high:%d%,low:%d%]\n", u1Src, u2HTotal, u2VTotal, u4Start, u4High, u4Low);

    switch(u1Src)
    {
        case SrcPWM1:
            //UNUSED(BSP_PinSet(PIN_OPWM1, PINMUX_FUNCTION3));
            if (u4High == 100)            
            {			 
	            vIO32WriteFldAlign(PWM_SCAN_06, 1, REG_PWM_SCAN1_FIXED_HIGH);			 
	            vIO32WriteFldAlign(PWM_SCAN_06, 0, REG_PWM_SCAN1_FIXED_LOW);		 			 			 			             
            }			
            else if (u4High == 0)			
            {			 
				vIO32WriteFldAlign(PWM_SCAN_06, 0, REG_PWM_SCAN1_FIXED_HIGH);			 
				vIO32WriteFldAlign(PWM_SCAN_06, 1, REG_PWM_SCAN1_FIXED_LOW);					 			 			 			
			}			
			else			
			{			 
				vIO32WriteFldAlign(PWM_SCAN_06, 0, REG_PWM_SCAN1_FIXED_HIGH);			 
				vIO32WriteFldAlign(PWM_SCAN_06, 0, REG_PWM_SCAN1_FIXED_LOW);			             			
			}
			if (u4Start==0)
			{
				vDrvSetScan1Start(0x1FF);
			}
			else
			{
            	vDrvSetScan1Start(u4Start*u4Val);
            }
            vDrvSetScan1High((u4High*u4Val));
            vDrvSetScan1Low(u4Low*u4Val);
            break;

        case SrcPWM2:
        	//UNUSED(BSP_PinSet(PIN_OPWM2, PINMUX_FUNCTION3));
            if (u4High == 100)            
            {			 
	            vIO32WriteFldAlign(PWM_SCAN_06, 1, REG_PWM_SCAN2_FIXED_HIGH);			 
	            vIO32WriteFldAlign(PWM_SCAN_06, 0, REG_PWM_SCAN2_FIXED_LOW);		 			 			 			             
            }			
            else if (u4High == 0)			
            {			 
				vIO32WriteFldAlign(PWM_SCAN_06, 0, REG_PWM_SCAN2_FIXED_HIGH);			 
				vIO32WriteFldAlign(PWM_SCAN_06, 1, REG_PWM_SCAN2_FIXED_LOW);					 			 			 			
			}			
			else			
			{			 
				vIO32WriteFldAlign(PWM_SCAN_06, 0, REG_PWM_SCAN2_FIXED_HIGH);			 
				vIO32WriteFldAlign(PWM_SCAN_06, 0, REG_PWM_SCAN2_FIXED_LOW);			             			
			}
			if (u4Start==0)
			{
				vDrvSetScan2Start(0x1FF);
			}
			else
			{
            	vDrvSetScan2Start(u4Start*u4Val);
            }
            vDrvSetScan2High((u4High*u4Val));
            vDrvSetScan2Low(u4Low*u4Val);
            break;

        case SrcPWM3:
            // JTMS PINMUX --> FUNCTION4
            if (u4High == 100)            
            {			 
	            vIO32WriteFldAlign(PWM_SCAN_10, 1, REG_PWM_SCAN3_FIXED_HIGH);			 
	            vIO32WriteFldAlign(PWM_SCAN_10, 0, REG_PWM_SCAN3_FIXED_LOW);		 			 			 			             
            }			
            else if (u4High == 0)			
            {			 
				vIO32WriteFldAlign(PWM_SCAN_10, 0, REG_PWM_SCAN3_FIXED_HIGH);			 
				vIO32WriteFldAlign(PWM_SCAN_10, 1, REG_PWM_SCAN3_FIXED_LOW);					 			 			 			
			}			
			else			
			{			 
				vIO32WriteFldAlign(PWM_SCAN_10, 0, REG_PWM_SCAN3_FIXED_HIGH);			 
				vIO32WriteFldAlign(PWM_SCAN_10, 0, REG_PWM_SCAN3_FIXED_LOW);			             			
			}
			if (u4Start==0)
			{
				vDrvSetScan3Start(0x1FF);
			}
			else
			{
            	vDrvSetScan3Start(u4Start*u4Val);
            }
            vDrvSetScan3High((u4High*u4Val));
            vDrvSetScan3Low(u4Low*u4Val);
            break;

        case SrcPWM4:
            // JTDI PINMUX --> FUNCTION4
            if (u4High == 100)            
            {			 
	            vIO32WriteFldAlign(PWM_SCAN_10, 1, REG_PWM_SCAN4_FIXED_HIGH);			 
	            vIO32WriteFldAlign(PWM_SCAN_10, 0, REG_PWM_SCAN4_FIXED_LOW);		 			 			 			             
            }			
            else if (u4High == 0)			
            {			 
				vIO32WriteFldAlign(PWM_SCAN_10, 0, REG_PWM_SCAN4_FIXED_HIGH);			 
				vIO32WriteFldAlign(PWM_SCAN_10, 1, REG_PWM_SCAN4_FIXED_LOW);					 			 			 			
			}			
			else			
			{			 
				vIO32WriteFldAlign(PWM_SCAN_10, 0, REG_PWM_SCAN4_FIXED_HIGH);			 
				vIO32WriteFldAlign(PWM_SCAN_10, 0, REG_PWM_SCAN4_FIXED_LOW);			             			
			}
			if (u4Start==0)
			{
				vDrvSetScan4Start(0x1FF);
			}
			else
			{
            	vDrvSetScan4Start(u4Start*u4Val);
            }
            vDrvSetScan4High((u4High*u4Val));
            vDrvSetScan4Low(u4Low*u4Val);
            break;

        case SrcPWM5:
            // JTRST PINMUX --> FUNCTION4
            if (u4High == 100)            
            {			 
	            vIO32WriteFldAlign(PWM_SCAN_11, 1, REG_PWM_SCAN5_FIXED_HIGH);			 
	            vIO32WriteFldAlign(PWM_SCAN_11, 0, REG_PWM_SCAN5_FIXED_LOW);		 			 			 			             
            }			
            else if (u4High == 0)			
            {			 
				vIO32WriteFldAlign(PWM_SCAN_11, 0, REG_PWM_SCAN5_FIXED_HIGH);			 
				vIO32WriteFldAlign(PWM_SCAN_11, 1, REG_PWM_SCAN5_FIXED_LOW);					 			 			 			
			}			
			else			
			{			 
				vIO32WriteFldAlign(PWM_SCAN_11, 0, REG_PWM_SCAN5_FIXED_HIGH);			 
				vIO32WriteFldAlign(PWM_SCAN_11, 0, REG_PWM_SCAN5_FIXED_LOW);			             			
			}
			if (u4Start==0)
			{
				vDrvSetScan5Start(0x1FF);
			}
			else
			{
            	vDrvSetScan5Start(u4Start*u4Val);
            }
            vDrvSetScan5High((u4High*u4Val));
            vDrvSetScan5Low(u4Low*u4Val);
            break;

		case SrcPWM6:
			// OPWM0/1/2 PINMUX --> FUNCTION1
            if (u4High == 100)            
            {			 
	            vIO32WriteFldAlign(PWM_SCAN_11, 1, REG_PWM_SCAN6_FIXED_HIGH);			 
	            vIO32WriteFldAlign(PWM_SCAN_11, 0, REG_PWM_SCAN6_FIXED_LOW);		 			 			 			             
            }			
            else if (u4High == 0)			
            {			 
				vIO32WriteFldAlign(PWM_SCAN_11, 0, REG_PWM_SCAN6_FIXED_HIGH);			 
				vIO32WriteFldAlign(PWM_SCAN_11, 1, REG_PWM_SCAN6_FIXED_LOW);					 			 			 			
			}			
			else			
			{			 
				vIO32WriteFldAlign(PWM_SCAN_11, 0, REG_PWM_SCAN6_FIXED_HIGH);			 
				vIO32WriteFldAlign(PWM_SCAN_11, 0, REG_PWM_SCAN6_FIXED_LOW);			             			
			}
			if (u4Start==0)
			{
				vDrvSetScan5Start(0x1FF);
			}
			else
			{
            	vDrvSetScan6Start(u4Start*u4Val);
            }
            vDrvSetScan6High((u4High*u4Val));
            vDrvSetScan6Low(u4Low*u4Val);
            break;

        default:
            ASSERT(u1Src <= SrcPWM6);
    }

}

void vDrvSetScanPWMStartPhase(UINT32 u4Src, UINT32 u4Start_Phase)
{
    UINT16 u2HTotal, u2VTotal;
    UINT32 u4Val;

	u2HTotal = wDrvGetOutputHTotal();
    u2VTotal = wDrvGetOutputVTotal();

	u4Val = u2HTotal * u2VTotal / 100;

    switch(u4Src)
    {
        case SrcPWM1:
			vDrvSetScan1Phase(u4Val * u4Start_Phase);
			break;

		case SrcPWM2:
			vDrvSetScan2Phase(u4Val * u4Start_Phase);
			break;

		case SrcPWM3:
			vDrvSetScan3Phase(u4Val * u4Start_Phase);
			break;

		case SrcPWM4:
			vDrvSetScan4Phase(u4Val * u4Start_Phase);
			break;

		case SrcPWM5:
			vDrvSetScan5Phase(u4Val * u4Start_Phase);
			break;

		case SrcPWM6:
			vDrvSetScan6Phase(u4Val * u4Start_Phase);
			break;

		default:
            ASSERT(u4Src <= SrcPWM6);
    }
}

void vDrvSetScanBLOnOff(UINT8 u1OnOff)
{
    vIO32WriteFldAlign(PWM_HS_01, u1OnOff, TCON_BL_EN);
}

/** Brief
 * This driver control the scanning backlight polarify
 * @param bSrc  PWM source (ScanPWM1/2)
 * @param u1blPol  0: Normal/1: Reverse
 * @return void
 */
void vDrvSetScanPWMPolarity(UINT32 u4Src, UINT8 u1blPol)
{
    switch(u4Src)
    {
        case SrcPWM1:
            vIO32WriteFldAlign(PWM_SCAN_06, u1blPol, REG_PWM_SCAN1_POL);
            break;

        case SrcPWM2:
            vIO32WriteFldAlign(PWM_SCAN_06, u1blPol, REG_PWM_SCAN2_POL);
            break;

        case SrcPWM3:
            vIO32WriteFldAlign(PWM_SCAN_10, u1blPol, REG_PWM_SCAN3_POL);
            break;

        case SrcPWM4:
            vIO32WriteFldAlign(PWM_SCAN_10, u1blPol, REG_PWM_SCAN4_POL);
            break;

        case SrcPWM5:
            vIO32WriteFldAlign(PWM_SCAN_11, u1blPol, REG_PWM_SCAN5_POL);
            break;

		case SrcPWM6:
			vIO32WriteFldAlign(PWM_SCAN_11, u1blPol, REG_PWM_SCAN6_POL);
            break;

        default:
            ASSERT(u4Src <= SrcPWM6);
    }
}

/** Brief
 * This driver control the scanning PWM enable
 * @param u1OnOff
 * @return void
 */
void vDrvSetScanPWMOnOff(UINT32 u4Src, UINT8 u1OnOff)
{
	switch(u4Src)    
	{        
		case SrcPWM1:            
			vIO32WriteFldAlign(PWM_SCAN_12, u1OnOff, REG_PWM_SCAN1_EN);            
			break;        
		case SrcPWM2:            
			vIO32WriteFldAlign(PWM_SCAN_12, u1OnOff, REG_PWM_SCAN2_EN);            
			break;        
		case SrcPWM3:            
			vIO32WriteFldAlign(PWM_SCAN_12, u1OnOff, REG_PWM_SCAN3_EN);           
			break;        
		case SrcPWM4:            
			vIO32WriteFldAlign(PWM_SCAN_12, u1OnOff, REG_PWM_SCAN4_EN);            
			break;        
		case SrcPWM5:            
			vIO32WriteFldAlign(PWM_SCAN_12, u1OnOff, REG_PWM_SCAN5_EN);            
			break;		
		case SrcPWM6:			
			vIO32WriteFldAlign(PWM_SCAN_12, u1OnOff, REG_PWM_SCAN6_EN);            
			break;		
		default:		    
			ASSERT(u4Src <= SrcPWM6);    
	}
}

/** Brief
 * This driver control the scanning PWM latch mode
 * @param u1Mode
 * @param 0: Latch until set_data assert
 * @param 1: Update at once
 * @return void
 */
void vDrvSetScanPWMLatchMode(UINT8 u1SetDataMode,UINT8 u1Mode, UINT8 u1AlignMode)
{
#if defined (CC_MT5398) || defined(CC_MT5880)
    vIO32WriteFldAlign(PWM_HS_01, u1Mode, REG_PWM_SCAN_LATCH_OFF);
#elif defined(CC_MT5881)
    vIO32WriteFldAlign(PWM_HS_01, u1SetDataMode, REG_PWM_SCAN_SET_DATA_EN);
    vIO32WriteFldAlign(PWM_HS_01, u1SetDataMode, REG_PWM_SCAN_SET_DATA_EN2);
    vIO32WriteFldAlign(PWM_HS_01, u1Mode, REG_PWM_SCAN_LATCH_OFF);
    vIO32WriteFldAlign(PWM_HS_01, u1Mode, REG_PWM_SCAN_LATCH_OFF2);
	vIO32WriteFldAlign(PWM_HS_01, u1AlignMode, REG_PWM_SCAN_LATCH_SEL);
#endif
}

void vDrvScanPWMDataFire(void)
{
	vIO32WriteFldAlign(PWM_HS_01, 1, REG_PWM_SCAN_LATCH_OFF);
    vIO32WriteFldAlign(PWM_HS_01, 0, REG_PWM_SCAN_LATCH_OFF);
    
    vIO32WriteFldAlign(PWM_HS_01, 1, REG_PWM_SCAN_LATCH_OFF2);
    vIO32WriteFldAlign(PWM_HS_01, 0, REG_PWM_SCAN_LATCH_OFF2);
}

/** Brief
 * This driver control the output mux for scanning pwm
 * @param bSrc  PWM source (ScanPWM1/2)
 * @param u1OutBL
 * @param 0: output SCAN_PWM
 * @param 1: output TCON_3dbl_out which is from Panel_intf
 * @return void
 */
void vDrvSetScanPWMOutBL(UINT32 u4Src, UINT8 u1OutBL)
{
    switch(u4Src)
    {
        case SrcPWM1:
            vIO32WriteFldAlign(PWM_SCAN_06, u1OutBL, REG_TCON_3DBL_OUT_EN1);
            break;

        case SrcPWM2:
            vIO32WriteFldAlign(PWM_SCAN_06, u1OutBL, REG_TCON_3DBL_OUT_EN2);
            break;

        case SrcPWM3:
            vIO32WriteFldAlign(PWM_SCAN_10, u1OutBL, REG_TCON_3DBL_OUT_EN3);
            break;

        case SrcPWM4:
            vIO32WriteFldAlign(PWM_SCAN_10, u1OutBL, REG_TCON_3DBL_OUT_EN4);
            break;

        case SrcPWM5:
            vIO32WriteFldAlign(PWM_SCAN_11, u1OutBL, REG_TCON_3DBL_OUT_EN5);
            break;

		case SrcPWM6:
            vIO32WriteFldAlign(PWM_SCAN_11, u1OutBL, REG_TCON_3DBL_OUT_EN6);
            break;

        default:
            ASSERT(u4Src <= SrcPWM6);
    }

}


void vDrvSetScanStartModeOnOff(UINT32 u4Src, UINT8 u1OnOff)
{
     switch(u4Src)
    {
        case SrcPWM1:
            vIO32WriteFldAlign(PWM_SCAN_12, u1OnOff, REG_PWM_SCAN1_START_MODE);
            break;

        case SrcPWM2:
            vIO32WriteFldAlign(PWM_SCAN_12, u1OnOff, REG_PWM_SCAN2_START_MODE);
            break;

        case SrcPWM3:
            vIO32WriteFldAlign(PWM_SCAN_12, u1OnOff, REG_PWM_SCAN3_START_MODE);
            break;

        case SrcPWM4:
            vIO32WriteFldAlign(PWM_SCAN_12, u1OnOff, REG_PWM_SCAN4_START_MODE);
            break;

        case SrcPWM5:
            vIO32WriteFldAlign(PWM_SCAN_12, u1OnOff, REG_PWM_SCAN5_START_MODE);
            break;

		case SrcPWM6:
			vIO32WriteFldAlign(PWM_SCAN_12, u1OnOff, REG_PWM_SCAN6_START_MODE);
            break;

		default:
            ASSERT(u4Src <= SrcPWM6);
    }

	vIO32WriteFldAlign(PWM_HS_01, 1, REG_PWM_SCAN_LATCH_OFF);
	vIO32WriteFldAlign(PWM_HS_01, 0, REG_PWM_SCAN_LATCH_OFF);	
}


/** Brief
 * This driver control the scanning backlight latch mode
 * @param 0: Latch BL into a whole frame
 * @param 1: Direct use BL info to mask PWM
 * @return void
 */
void vDrvSetScanBLLatchMode(UINT8 u1Mode)
{
    vIO32WriteFldAlign(PWM_SCAN_06, u1Mode, REG_PWM_SCAN_BL_DIS_LATCH);
}



void vDrvSetISOnOff(UINT8 u1Src, UINT8 u1OnOff)
{
#if defined(CC_MT5396) || defined(CC_MT5368)

    switch(u1Src)
    {
        case MINILVDS_IS_HSYNC:
        case MINILVDS_IS_VSYNC:
            vIO32WriteFldAlign(VINTF_P20, u1OnOff, VINTF_INV_SYNC_START);
            break;

        default:
            ASSERT(u1Src <= MINILVDS_IS_VSYNC);
    }

#endif
}

void vDrvSetISMaskNum(UINT8 u1Src, UINT8 u1MaskNum)
{
#if defined(CC_MT5396) || defined(CC_MT5368)

    switch(u1Src)
    {
        case MINILVDS_IS_HSYNC:
            vIO32WriteFldAlign(VINTF_P21, u1MaskNum, VINTF_IS_MASK_LEN_H);
            break;

        case MINILVDS_IS_VSYNC:
            vIO32WriteFldAlign(VINTF_P23, u1MaskNum, VINTF_IS_MASK_LEN_V);
            break;

        default:
            ASSERT(u1Src <= MINILVDS_IS_VSYNC);
    }

#endif
}

void vDrvSetISInputInverse(UINT8 u1Src, UINT8 u1Enable)
{
#if defined(CC_MT5396) || defined(CC_MT5368)

    switch(u1Src)
    {
        case MINILVDS_IS_HSYNC:
            vIO32WriteFldAlign(VINTF_P20, u1Enable, VINTF_IS_POL_H);
            break;

        case MINILVDS_IS_VSYNC:
            vIO32WriteFldAlign(VINTF_P22, u1Enable, VINTF_IS_POL_V);
            break;

        default:
            ASSERT(u1Src <= MINILVDS_IS_VSYNC);
    }

#endif
}

void vDrvSetISDelayOnOff(UINT8 u1Src, UINT8 u1OnOff)
{
#if defined(CC_MT5396) || defined(CC_MT5368)

    switch(u1Src)
    {
        case MINILVDS_IS_HSYNC:
        case MINILVDS_IS_VSYNC:
            vIO32WriteFldAlign(VINTF_P20, u1OnOff, VINTF_IS_FORCE_DELAY);
            break;

        default:
            ASSERT(u1Src <= MINILVDS_IS_VSYNC);
    }

#endif
}

void vDrvSetISDelayNum(UINT8 u1Src, UINT16 u2DelayNum)
{
#if defined(CC_MT5396) || defined(CC_MT5368)

    switch(u1Src)
    {
        case MINILVDS_IS_HSYNC:
            vIO32WriteFldAlign(VINTF_P20, u2DelayNum, VINTF_IS_DELAY_H);
            break;

        case MINILVDS_IS_VSYNC:
            vIO32WriteFldAlign(VINTF_P22, u2DelayNum, VINTF_IS_DELAY_V);
            break;

        default:
            ASSERT(u1Src <= MINILVDS_IS_VSYNC);
    }

#endif
}

void vDrvSetISHoldOnOff(UINT8 u1Src, UINT8 u1OnOff)
{
#if defined(CC_MT5396) || defined(CC_MT5368)

    switch(u1Src)
    {
        case MINILVDS_IS_HSYNC:
            vIO32WriteFldAlign(VINTF_P20, u1OnOff, VINTF_IS_HOLD_EN_H);
            break;

        case MINILVDS_IS_VSYNC:
            vIO32WriteFldAlign(VINTF_P22, u1OnOff, VINTF_IS_HOLD_EN_V);
            break;

        default:
            ASSERT(u1Src <= MINILVDS_IS_VSYNC);
    }

#endif
}

void vDrvSetISRelation(UINT8 u1Src, UINT8 u1SyncCnt, UINT8 u1ISSyncCnt)
{
#if defined(CC_MT5396) || defined(CC_MT5368)
    UINT32 u4Total =0, u4ISTotal=0;
    UINT8 u1Scatter =0;

    if(u1SyncCnt >= 15) u1SyncCnt = 15;

    if(u1ISSyncCnt >= 15) u1ISSyncCnt = 15;

    if(u1SyncCnt == 0) u1SyncCnt = 1;

    if(u1ISSyncCnt == 0) u1ISSyncCnt = 1;

    switch(u1Src)
    {
        case MINILVDS_IS_HSYNC:
            u4Total = wDrvGetOutputHTotal();
            break;

        case MINILVDS_IS_VSYNC:
            u4Total = wDrvGetOutputVTotal();
            break;

        default:
            ASSERT(u1Src <= MINILVDS_IS_VSYNC);
    }

    u4ISTotal = (u4Total * u1SyncCnt) / u1ISSyncCnt;
    u1Scatter = (u4Total * u1SyncCnt) % u1ISSyncCnt;

    switch(u1Src)
    {
        case MINILVDS_IS_HSYNC:
            vIO32WriteFldAlign(VINTF_P20, u1ISSyncCnt, VINTF_IS_DIV_H);
            vIO32WriteFldAlign(VINTF_P20, u1SyncCnt, VINTF_IS_MUL_H);
            vIO32WriteFldAlign(VINTF_P20, u1Scatter, VINTF_SYNC_TOTAL_PLUS_1_H);
            vIO32WriteFldAlign(VINTF_P21, u4ISTotal, VINTF_IS_PWM_SYNC_TOTAL_H);
            break;

        case MINILVDS_IS_VSYNC:
            vIO32WriteFldAlign(VINTF_P22, u1ISSyncCnt, VINTF_IS_DIV_V);
            vIO32WriteFldAlign(VINTF_P22, u1SyncCnt, VINTF_IS_MUL_V);
            vIO32WriteFldAlign(VINTF_P23, u1Scatter, VINTF_IS_LEN_PLUS_1_V);
            vIO32WriteFldAlign(VINTF_P23, u4ISTotal, VINTF_IS_PWM_SYNC_TOTAL_V);
            break;

        default:
            ASSERT(u1Src <= MINILVDS_IS_VSYNC);
    }

#endif
}

#ifdef CC_SUPPORT_STR
//-----------------------------------------------------------------------------
/** PWM_pm_str_suspend
 */
//-----------------------------------------------------------------------------
void PWM_pm_str_suspend(void)
{
	// power off
	CKGEN_WRITE32(0x1CC, CKGEN_READ32(0x1CC) & ~(1U << 10));

}


//-----------------------------------------------------------------------------
/** PWM_pm_str_resume
 */
//-----------------------------------------------------------------------------
void PWM_pm_str_resume(void)
{

	CKGEN_WRITE32(0x1CC, CKGEN_READ32(0x1CC) | (1U << 10));

}
#endif // CC_SUPPORT_STR


