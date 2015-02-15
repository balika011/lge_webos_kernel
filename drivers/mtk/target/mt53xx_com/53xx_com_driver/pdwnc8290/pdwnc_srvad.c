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
 * $RCSfile: pdwnc_srvad.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file pdwnc_srvad.c
 *  pdwnc_srvad.c The PDWNC (Power DoWN Controller) interface
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
#include "pdwnc_entry.h"
#include "pdwnc_gpio.h"
#include "pdwnc_t8032.h"
#include "pdwnc_drvif.h"
#include "sif_if.h"
#include "eeprom_if.h"

#include "ir_if.h"
#include "rtc_if.h"

//#undef PDWNC_WRITE32
//#undef PDWNC_READ32

//-----------------------------------------------------------------------------
// Configuration
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define PDWNC_WAKTOP(X) ((X==0)?PDWNC_WAKTOP0:((X==1)?PDWNC_WAKTOP1:((X==2)?PDWNC_WAKTOP2:(X==3)?PDWNC_WAKTOP3:\
						((X==4)?PDWNC_WAKTOP4:((X==5)?PDWNC_WAKTOP5:((X==6)?PDWNC_WAKTOP6:((X==7)?PDWNC_WAKTOP7:\
						((X==8)?PDWNC_WAKTOP8:PDWNC_WAKTOP9))))))))
#define PDWNC_WAKBTM(X) ((X==0)?PDWNC_WAKBTM0:((X==1)?PDWNC_WAKBTM1:((X==2)?PDWNC_WAKBTM2:(X==3)?PDWNC_WAKBTM3:\
						((X==4)?PDWNC_WAKBTM4:((X==5)?PDWNC_WAKBTM5:((X==6)?PDWNC_WAKBTM6:((X==7)?PDWNC_WAKBTM7:\
						((X==8)?PDWNC_WAKBTM8:PDWNC_WAKBTM9))))))))
#define PDWNC_CHCFG(X) ((X==0)?PDWNC_CHCFG0:((X==1)?PDWNC_CHCFG1:((X==2)?PDWNC_CHCFG2:(X==3)?PDWNC_CHCFG3:\
						((X==4)?PDWNC_CHCFG4:((X==5)?PDWNC_CHCFG5:((X==6)?PDWNC_CHCFG6:((X==7)?PDWNC_CHCFG7:\
						((X==8)?PDWNC_CHCFG8:PDWNC_CHCFG9))))))))


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static BOOL _fgServoADCInit = 0;
#if 0//#ifndef CC_MTK_LOADER
static const INT8 _ai1PdwncServoGpio2Int[TOTAL_SERVOADC_NUM] =
{
    -1, -1, 20, 21, 22, 23
};
#endif


//-----------------------------------------------------------------------------
// Prototypes 
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
#ifndef CC_MTK_LOADER
#ifndef CC_ENABLE_HW_SERVO_ADC  
static HANDLE_T _hServoADMutex;
#endif
#endif /* CC_MTK_LOADER */
//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------
void _PDWNC_ReadServoADCWakeupStatus(UINT32* pu4Val)
{
     VERIFY(pu4Val != NULL);
    *pu4Val = (_PDWNC_ReadWakeupStatus(0) & (WAK_ALLSRVAD));
}

void _PDWNC_SetupServoADC(UINT32 *pu4PowerDownEn)
{
    INT32 i, i4LastCh = -1;
    UINT32 u4Val, u4DkBit;
    UINT32 u4Max, u4Min;
    SGL_SERVO_T   *prSingleServoKeyList = NULL;
    
    ASSERT(pu4PowerDownEn != NULL);
    if (!PDWNC_SetupFpWakeup(NULL))
    {
        Printf("FP wakeup disable\n");
        return;
    }
#ifdef CC_NO_ACD_STANDBY_SETTING
    // if ServoADC is enabled, skip this setting, because this is set.
    {        
        Printf("Power down Enable:0x%08x\n", *pu4PowerDownEn);
        return;
    }
#endif
    u4DkBit = DRVCUST_InitGet(eKeyPadWakeupBit);
    i = DRVCUST_InitQuery(eSingleServoKeyList, (UINT32 *)(void *)&prSingleServoKeyList);
    if (i < 0)
    {
        return;
    }

    if (!((*pu4PowerDownEn) & WAK_IRRX))
    {
        u4DkBit = DKBIT_POWER;
    }

    // disable ServoADC working. --- mustang channel b,c,d,e,f for power detect
	vIO32WriteFldAlign(PDWNC_SRVCFG1, u4IO32Read4B(PDWNC_SRVCFG1) & (~0x3FF), FLD_SRVCH_EN);
	// Clear ServoADC output data and channel status.
	vIO32WriteFldAlign(PDWNC_SRVCLR, 0x1, FLD_ADCLR);
	//Printf("The total servoadc num = %d .\n",MAX_SERVOADC_NUM);
	for (i=0; i<MAX_SERVOADC_NUM; i++)//MAX_SERVOADC_NUM
	{
		vIO32WriteFldAlign(PDWNC_WAKTOP(i), 0x0, FLD_CH0TOP);
		vIO32WriteFldAlign(PDWNC_WAKBTM(i), 0x0, FLD_CH0BTM);
		vIO32WriteFldAlign(PDWNC_CHCFG(i), 0x0, FLD_CH0WKC);
	}

    // Do the Servo wakeup initialization
    i = 0;
    u4Max = 0;    
    u4Min = 0xff;//bit [9:2] 
    while ((prSingleServoKeyList) &&
            (prSingleServoKeyList[i].u4Key != 0) &&
            (prSingleServoKeyList[i].u4Key != DKBIT_NONE))
    {
        if (prSingleServoKeyList[i].u4Key & u4DkBit)
        {
            if ((prSingleServoKeyList[i].u4Top > 0x3ff) ||
                (prSingleServoKeyList[i].u4Servo >= (ADIN0 + TOTAL_SERVOADC_NUMBER)) ||
                (prSingleServoKeyList[i].u4Btm > 0x3ff) ||
                (prSingleServoKeyList[i].u4Btm >= prSingleServoKeyList[i].u4Top))
            {
                LOG(1, "Error Power button setting\n");
                i++;
                continue;
            }
            // In case of old style servo ADC numbering
            if(prSingleServoKeyList[i].u4Servo >= ADIN0)
            {
               prSingleServoKeyList[i].u4Servo -= ADIN0; 
            }

            // Make sure enable PAD
            u4Val = PDWNC_ReadServoADCChannelValue(prSingleServoKeyList[i].u4Servo);

			//Get the range
            if(prSingleServoKeyList[i].u4Servo != i4LastCh)
            {//reset init value while setting another channel...
                u4Max = 0;    
                u4Min = 0x3ff;    
            }
            else        
            {            
                u4Max = IO32ReadFldAlign(PDWNC_WAKTOP(prSingleServoKeyList[i].u4Servo), FLD_CH0TOP);
                u4Min = IO32ReadFldAlign(PDWNC_WAKBTM(prSingleServoKeyList[i].u4Servo), FLD_CH0BTM);
            }
            i4LastCh = prSingleServoKeyList[i].u4Servo;                    
            // Set Servo ADC value            
            u4Val = ((u4Max < prSingleServoKeyList[i].u4Top) ? (prSingleServoKeyList[i].u4Top) : u4Max) & 0x3FF;
			vIO32WriteFldAlign(PDWNC_WAKTOP(prSingleServoKeyList[i].u4Servo), u4Val, FLD_CH0TOP);	           
            u4Val = ((u4Min > prSingleServoKeyList[i].u4Btm) ? (prSingleServoKeyList[i].u4Btm) : u4Min) & 0x3FF; 
			vIO32WriteFldAlign(PDWNC_WAKBTM(prSingleServoKeyList[i].u4Servo), u4Val, FLD_CH0BTM);   
			
            if(u4Val)
                u4Val = CH_WAK_MID_EN & CH_MASK;
            else
                u4Val = (CH_WAK_MID_EN | CH_WAK_BTM_EN) & CH_MASK;
			vIO32WriteFldAlign(PDWNC_CHCFG(prSingleServoKeyList[i].u4Servo), u4Val, FLD_CH0WKC);
            u4Val = 0;
            *pu4PowerDownEn |= PDWNC_SRVAD_WAKEN(PDWNC_WAK_SRVADX(prSingleServoKeyList[i].u4Servo));
        }
        i++;
    }
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void PDWNC_ResumeServoADC(void)
{
    // Enable servo ADC H/W mode
	vIO32WriteFldAlign(PDWNC_CLKPDN, 0x0,FLD_SRVPD);
	vIO32WriteFldAlign(PDWNC_SADCCFG0, 0x0, FLD_RG_SRV_MODE_SEL);
	vIO32WriteFldAlign(PDWNC_SADCCFG0, 0x1, FLD_RG_SRV_BIAS_OPTION);
	vIO32WriteFldAlign(PDWNC_SRVCFG0, 0x0, FLD_SWEN);
	vIO32WriteFldAlign(PDWNC_SRVCFG0, 0x1, FLD_HWEN);
}

void PDWNC_InitServoADC(void)
{
    if(_fgServoADCInit == 0)
    {        
    	vIO32WriteFldAlign(PDWNC_CLKPDN, 0x0,FLD_SRVPD);
    	vIO32WriteFldAlign(PDWNC_SADCCFG0, 0x0, FLD_RG_SRV_MODE_SEL);
		vIO32WriteFldAlign(PDWNC_SADCCFG0, 0x1, FLD_RG_SRV_BIAS_OPTION);
    	vIO32WriteFldAlign(PDWNC_SRVCFG0, 0x0, FLD_SWEN);
    	vIO32WriteFldAlign(PDWNC_SRVCFG0, 0x1, FLD_HWEN);
		
    #ifndef CC_MTK_LOADER    
        VERIFY(x_sema_create(&_hServoADMutex, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);    
    #endif    
    	//UNUSED(PDWNC_InitServoGpio());
    	_fgServoADCInit = 1;
    }
}

//-----------------------------------------------------------------------------
/** PDWNC_ReadServoADCChannelValue() Read the ServoADC Channel Value
 *  @param u4Channel the channel number
 *  @retval the channel adc value.
 */
//-----------------------------------------------------------------------------
UINT32 PDWNC_ReadServoADCChannelValue(UINT32 u4Channel)
{
	UINT32 u4Val, u4Ret, i;
	#ifndef CC_MTK_LOADER
	CRIT_STATE_T rCrit;
	#endif
	// support 2 styles of servo adc numbering: 0 ~ 4 and 400 ~ 404
	if(u4Channel >= ADIN0)
	{
		u4Channel -= ADIN0;
	}
	// Maximum is channel 9.(0~9)
	VERIFY(u4Channel < TOTAL_SERVOADC_NUM);
	PDWNC_InitServoADC();
//#ifndef CC_MTK_LOADER
//   VERIFY(x_sema_lock(_hServoADMutex, X_SEMA_OPTION_WAIT) == OSR_OK);
//#endif
	#ifndef CC_MTK_LOADER
	rCrit = x_crit_start();
	#endif /* CC_MTK_LOADER */

	u4Val = IO32ReadFld(PDWNC_SRVCFG1, FLD_SRVCH_EN);
	i = 0;
	if((u4Val & PDWNC_SRVCH_EN_CH(u4Channel)) == 0)
	{
		u4Val |= PDWNC_SRVCH_EN_CH(u4Channel);
		vIO32Write4B(PDWNC_SRVCFG1, u4Val);
		//wait for servoadc update
		HAL_Delay_us(50);
		while(((IO32ReadFldAlign(PDWNC_SRVCST, FLD_SRVCST)) != u4Channel) && (i<3000))
		{
			i++;	//use i for timeout
		}
		if(i >= 3000)
		{
			Printf("Wait Servoadc Channel%d update timeout!!!", u4Channel);
		}
	}
	
	u4Ret = IO32ReadFldAlign(PDWNC_ADOUT0 + (u4Channel * 4), FLD_ADOUT0);	
        	
//#ifndef CC_MTK_LOADER
//	VERIFY(x_sema_unlock(_hServoADMutex) == OSR_OK);
//#endif
	#ifndef CC_MTK_LOADER
	x_crit_end(rCrit);		   
	#endif /* CC_MTK_LOADER */ 

	return (u4Ret & 0x3ff); //Oryx servoADC bit2~11.
}

//-----------------------------------------------------------------------------
/** PDWNC_SRVAD_Read : ABIST version of PDWNC_ReadServoADCChannelValue()
 * function name 'PDWNC_SRVAD_Read' is named by ABIST integrator
 *  @param u4Channel the channel number
 *  @retval the channel adc value.
 */
//-----------------------------------------------------------------------------
#if 0
UINT32 PDWNC_SRVAD_Read(UINT32 u4Channel)
{
	UINT32 u4Val;
	UINT32 u4OldSrvRate0;
    u4OldSrvRate0 = IO32ReadFld(PDWNC_SRVRAT, FLD_SMP_RATE);
	vIO32WriteFldAlign(PDWNC_SRVRAT, 0x0, FLD_SMP_RATE);
	vIO32WriteFldAlign(PDWNC_SRVCFG0, 0x1, FLD_ABIST_MODE);
	u4Val = PDWNC_ReadServoADCChannelValue(u4Channel);
	vIO32WriteFldAlign(PDWNC_SRVRAT, u4OldSrvRate0, FLD_SMP_RATE);
	vIO32WriteFldAlign(PDWNC_SRVCFG0, 0x0, FLD_ABIST_MODE);	
	return u4Val;
}
#endif

//todo: this function is used or not ? and check define ADC2GPIO_CH_ID_MIN  ADC2GPIO_CH_ID_MAX
INT32 PDWNC_ServoGpioRangeCheck(INT32 i4Gpio)
{
    if((i4Gpio >= (ADC2GPIO_CH_ID_MIN + SERVO_0_ALIAS)) && (i4Gpio <= (ADC2GPIO_CH_ID_MAX + SERVO_0_ALIAS)))//only srvad4~7 can be configured as gpio for 5368/96
    {
        return 1;
    }
    else
    {   
        return 0;
    }   
}


