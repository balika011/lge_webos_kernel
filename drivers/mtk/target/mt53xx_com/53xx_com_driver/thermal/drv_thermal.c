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
 * $RCSfile: drv_thermal.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
#ifdef __KERNEL__
#include <linux/mtk_thermal.h>
#endif
#include "hw_thermal.h"
#include "hw_pdwnc.h"
#include "hw_vdoin.h"
#include "drv_thermal.h"
#include "drv_thermal_if.h"
#include "drv_common.h"
#include "nptv_debug.h"

#ifndef CC_MTK_LOADER
#include "x_os.h"
#include "x_bim.h"
#endif

static int g_curr_temp = 0;
static int g_prev_temp = 0;

// Unit: 0.001 degree C
static unsigned int trip_temp[10] = {120000,110000,100000,90000,80000,70000,65000,60000,55000,50000};

int MA_len=5;
int MA_len_temp=0;
static int proc_write_flag=0;
static int MA_counter=0, MA_first_time=0;
int last_abb_t=0;

// MT5890 & MT5861
#define TS_ADC_INPUT_RANGE 28 // MT5890/5861: 2.8V / MT8135:1.5V
#define TS_BUFFER_GAIN 32 // MT5890/5861:3.2X  / MT8135:1.8x
#define TS_LOW_CRITERIAL 3192 //MT5890/5861: 4096*0.682*3.2/2.8=3192  / MT8135:4096*0.682*1.8/1.5=3352(3350)

static kal_int32 g_adc_ge = 0;  // ADC Gain Error + 512 (Uint: 1/4096) 
static kal_int32 g_adc_oe = 0;  // ADC Offset Error + 512  (Uint: 1/4096)
static kal_int32 g_o_vtsmcu1 = 0; // TSU0 Offset (Uint: 1/4096)
static kal_int32 g_o_vtsmcu2 = 0; // TSU1 Offset (Uint: 1/4096)
static kal_int32 g_o_vtsmcu3 = 0; // TSU2 Offset (Uint: 1/4096)
static kal_int32 g_degc_cali = 0;  // Environment tempature (Unit: degree C) * 2
static kal_int32 g_adc_cali_en = 0; // Gain and Offset calibration
static kal_int32 g_o_slope_sign = 0; // Buffer Gain 3.2 calibration slop (1.xx>=1.65, O_SLOPE_SIGN=0, 1.xx<1.65, O_SLOPE_SIGN=1)
static kal_int32 g_o_slope = 0; // Buffer Gain 3.2 calibration value
static kal_int32 g_id = 0;

static kal_int32 g_ge = 0;  // ADC Real Gain Error  (Uint: 1*10000/4096) 
static kal_int32 g_oe = 0;  // ADC Real Offset Error  (Uint: 1*10000/4096) 
static kal_int32 g_gain = 0;

static kal_int32 g_x_roomt[4] = {0, 0, 0, 0};

static int* tz_last_values[MTK_THERMAL_SENSOR_COUNT] = {NULL};
static int TS_curr_raw[4] = {0, 0, 0, 0};
#define PTP_SRVAD_SW_MODE 0

#define HW_TS_CHANNEL_EN Fld(16,16,AC_FULLW32)//[31:16]
/**
 * @brief vDrvThermal_Reset_and_Initial
 * reset and initial thermal module
 * @param  void
 * @retval void
 */
static void vDrvThermal_Reset_and_Initial(void)
{

	LOG(6,"[Reset and init thermal controller]\n");

	#if PTP_SRVAD_SW_MODE
	vIO32WriteFldAlign(PDWNC_SRVCFG0, 0, FLD_HWEN);
	vIO32WriteFldAlign(PDWNC_SRVCFG0, 1, FLD_SWEN);

	// Thermal Controller 1
	// 1/60MHz=16.67ns
	// 219h x 0CCh x 100h x 16.6ns = 465,535,180.82 ns =0.4 sec
	DRV_WriteReg32(TEMPMONCTL1, 0x000000CC);
	DRV_WriteReg32(TEMPMONCTL2, 0x02190219);
	DRV_WriteReg32(TEMPAHBPOLL,  0x00000001);
	DRV_WriteReg32(TEMPAHBTO,    0x00000333);
	DRV_WriteReg32(TEMPMONIDET0, 0x00000000);
	DRV_WriteReg32(TEMPMONIDET1, 0x00000000);
	DRV_WriteReg32(TEMPMONIDET2, 0x00000000);
	DRV_WriteReg32(TEMPH2NTHRE,  0x000000FF);	
	DRV_WriteReg32(TEMPHTHRE,    0x000000AA);	
	DRV_WriteReg32(TEMPCTHRE,    0x00000133);		
	DRV_WriteReg32(TEMPOFFSETH,  0x000000BB);	
	DRV_WriteReg32(TEMPOFFSETL,  0x00000111);			
	DRV_WriteReg32(TEMPMSRCTL0,  0x000006DB);	

	DRV_WriteReg32(TEMPADCPNP0, 0x172);
	DRV_WriteReg32(TEMPADCPNP1, 0x1B2);
	DRV_WriteReg32(TEMPADCPNP2, 0x1F2);
	DRV_WriteReg32(TEMPADCMUX, 0x11);		
	DRV_WriteReg32(TEMPADCEXT, 0x22);		 
	DRV_WriteReg32(TEMPADCEXT1, 0x33);				
	DRV_WriteReg32(TEMPADCEN, 0x1);
	
	DRV_WriteReg32(TEMPPNPMUXADDR, 0XF0028300);
	DRV_WriteReg32(TEMPADCENADDR, 0XF0028308);
	DRV_WriteReg32(TEMPADCVALIDADDR, 0XF0028308);
	DRV_WriteReg32(TEMPADCVOLTADDR, 0xF0028960);
	
	DRV_WriteReg32(TEMPRDCTRL, 0x1);			 
	DRV_WriteReg32(TEMPADCVALIDMASK, 0x00000000);
	DRV_WriteReg32(TEMPADCVOLTAGESHIFT, 0x0);
	DRV_WriteReg32(TEMPADCWRITECTRL, 0x00000001);
	
	DRV_WriteReg32(TEMPMONINT,		 0x00000000);
	DRV_WriteReg32(TEMPMONCTL0, 	 0x00000007);

	#else
	vIO32WriteFldAlign(PDWNC_SRVCFG1, 0xFFFF, HW_TS_CHANNEL_EN);

	// Thermal Controller 1
	// 1/60MHz=16.67ns
	// 219h x 0CCh x 100h x 16.6ns = 465,535,180.82 ns =0.4 sec	
	DRV_WriteReg32(TEMPMONCTL1, 0x000000CC);
	DRV_WriteReg32(TEMPMONCTL2, 0x02190219);
	// One channel detect is about 30us
	// 32 * 30 = 960us
	// Polling time uint: 16.67 ns (PTP_ck clock selection: 480/8=60MHz)
	// Polling time = 960us/16.67ns = 57588 = 0xE0F4
	// Timing out unit:   20.83 us (PTP_SLOW_ck clock selection: 24MHz/512=48KHz)
	// 0x333*20.83us = 17ms
	DRV_WriteReg32(TEMPAHBPOLL,  0x0000E0F4);
	DRV_WriteReg32(TEMPAHBTO,    0x00000333);
	DRV_WriteReg32(TEMPMONIDET0, 0x00000000);
	DRV_WriteReg32(TEMPMONIDET1, 0x00000000);
	DRV_WriteReg32(TEMPMONIDET2, 0x00000000);
	DRV_WriteReg32(TEMPH2NTHRE,  0x000000FF);	
	DRV_WriteReg32(TEMPHTHRE,    0x000000AA);	
	DRV_WriteReg32(TEMPCTHRE,    0x00000133);		
	DRV_WriteReg32(TEMPOFFSETH,  0x000000BB);	
	DRV_WriteReg32(TEMPOFFSETL,  0x00000111);			
	DRV_WriteReg32(TEMPMSRCTL0,  0x000006DB);	

	DRV_WriteReg32(TEMPADCPNP0, 0x0);
	DRV_WriteReg32(TEMPADCPNP1, 0x1);
	DRV_WriteReg32(TEMPADCPNP2, 0x2);
	DRV_WriteReg32(TEMPADCMUX, 0x11);		
	DRV_WriteReg32(TEMPADCEXT, 0x22);		 
	DRV_WriteReg32(TEMPADCEXT1, 0x33);		
	
	if ((IS_IC_5890_ES1()|| IS_IC_5890_MPS1()))
	{
		DRV_WriteReg32(TEMPADCEN, 0x0);
		
		DRV_WriteReg32(TEMPPNPMUXADDR, 0XF00283FC);		
		DRV_WriteReg32(TEMPADCENADDR, 0XF0028630);
		DRV_WriteReg32(TEMPADCVALIDADDR, 0XF0028630);
		DRV_WriteReg32(TEMPADCVOLTADDR, 0xF0028960);
		DRV_WriteReg32(TEMPRDCTRL, 0x1);			 
		DRV_WriteReg32(TEMPADCVALIDMASK, 0x00000000);
	}
	else
	{
		DRV_WriteReg32(TEMPADCEN, 0x1);
		
		DRV_WriteReg32(TEMPPNPMUXADDR, 0XF00283FC);		
		DRV_WriteReg32(TEMPADCENADDR, 0XF0028300);
		DRV_WriteReg32(TEMPADCVALIDADDR, 0XF0028314);
		DRV_WriteReg32(TEMPADCVOLTADDR, 0xF00283d4);
		DRV_WriteReg32(TEMPRDCTRL, 0x1);	
	 	DRV_WriteReg32(TEMPADCVALIDMASK, 0x00000022);		
	}
	
	DRV_WriteReg32(TEMPADCVOLTAGESHIFT, 0x0);
	DRV_WriteReg32(TEMPADCWRITECTRL, 0x00000001);
	
	DRV_WriteReg32(TEMPMONINT,		 0x00000000);
	DRV_WriteReg32(TEMPMONCTL0, 	 0x00000007);
	#endif
	
//	DRV_WriteReg32(TS_CON0_V, DRV_Reg32(TS_CON0_V) & 0xFFFFFF3F);	//read tsmcu need
	
}

#define SPARE_REPLAC 0xF0028630
void vDrvThermal_Fast_Init(void)
{
	UINT32 temp = 0;

    DRV_WriteReg32(TEMPMONCTL1, 1);
    DRV_WriteReg32(TEMPMONCTL2, 1);
    DRV_WriteReg32(TEMPAHBPOLL, 1);
    DRV_WriteReg32(TEMPAHBTO,    0x000000FF);
    DRV_WriteReg32(TEMPMONIDET0, 0x00000000);
    DRV_WriteReg32(TEMPMONIDET1, 0x00000000);

    DRV_WriteReg32(TEMPMSRCTL0, 0x0000000); 

	DRV_WriteReg32(TEMPADCPNP0, 0x172);
	DRV_WriteReg32(TEMPADCPNP1, 0x1B2);
	DRV_WriteReg32(TEMPADCPNP2, 0x1F2);
	DRV_WriteReg32(TEMPADCMUX, 0x11);		
	DRV_WriteReg32(TEMPADCEXT, 0x22);		 
	DRV_WriteReg32(TEMPADCEXT1, 0x33);				

    temp = 0xCE1; // 40 degree
    DRV_WriteReg32(TEMPADCEN, (0x00001000 + temp));//write temp to spare register	
	
	DRV_WriteReg32(TEMPPNPMUXADDR, SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCENADDR, SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCVALIDADDR,SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCVOLTADDR, SPARE_REPLAC);
	
	DRV_WriteReg32(TEMPRDCTRL, 0x0);			 
	DRV_WriteReg32(TEMPADCVALIDMASK, 0x0000002C);
	DRV_WriteReg32(TEMPADCVOLTAGESHIFT, 0x0);
	DRV_WriteReg32(TEMPADCWRITECTRL, 0x00000000);
	
	DRV_WriteReg32(TEMPMONINT,		 0x00000000);
	DRV_WriteReg32(TEMPMONCTL0, 	 0x00000007);
 
	#if THERMAL_DEBUG_LOG
    temp = DRV_Reg32(TEMPMSR0);
	LOG(6,"thermal_fast_init temp0 =%d \n", temp);
    temp = DRV_Reg32(TEMPMSR1);
	LOG(6,"thermal_fast_init temp1 =%d \n", temp);
	#endif

}

void Get_Thermal_Ctrl_Trigger_Temp(void)
{
	UINT32 curr_raw[4], curr_temp[4];	
	curr_raw[0] = DRV_Reg32(TEMPPROTTA);
	curr_raw[1] = DRV_Reg32(TEMPPROTTB);
	curr_raw[2] = DRV_Reg32(TEMPPROTTC);
	curr_raw[3] = DRV_Reg32(TEMPPROTTC);	

	raw_to_temperature_roomt(curr_raw,curr_temp);

	LOG(6,"Trigger tempature setting (0.1 degree)\n");
	LOG(6,"[LOW]: %5d\n",curr_temp[0]);
	LOG(6,"[MIDDLE]: %5d\n",curr_temp[1]);
	LOG(6,"[HIGH]: %5d\n",curr_temp[2]);	
}

void Set_Thermal_Ctrl_Trigger(int temperature)
{
	int temp = 0;
	int raw_high, raw_middle, raw_low;

	#if THERMAL_DEBUG_LOG
	LOG(6,"[Set_thermal_ctrl_trigger_SPM]: temperature=%d\n", temperature);
	#endif
	
	//temperature to trigger SPM state2
	raw_high = temperature_to_raw_roomt(temperature); // 120 degreeC * 1000 =120000 * 0.001 degre
	raw_middle = temperature_to_raw_roomt(40000);     // 40 degreeC  * 1000 = 40000 * 0.001 degre
	raw_low = temperature_to_raw_roomt(10000);        // 10 degreeC * 1000 = 10000 * 0.001 degre

	temp = DRV_Reg32(TEMPMONINT);
	DRV_WriteReg32(TEMPMONINT, temp & 0x1FFFFFFF);	// disable trigger SPM interrupt
	
	DRV_WriteReg32(TEMPPROTCTL, 0x20000);
	DRV_WriteReg32(TEMPPROTTA, raw_low);
	DRV_WriteReg32(TEMPPROTTB, raw_middle);
	DRV_WriteReg32(TEMPPROTTC, raw_high);
	
	DRV_WriteReg32(TEMPMONINT, temp | 0x80000000);	// only enable trigger SPM high temp interrupt

	#if THERMAL_DEBUG_LOG
	LOG(6,"raw_high=%d,  raw_middle=%d, raw_low = %d \n", raw_high, raw_middle,raw_low);
	#endif	
	
}


/**
 * @brief vDrvThermal_Cal_Prepare
 * Get thermal sensor coefficient from eFuse
 * @param  void
 * @retval void
 */
static void vDrvThermal_Cal_Prepare(void)
{
	kal_uint32 temp0, temp1;

	temp0 = DRV_Reg32(EFUSE_TS1_ADDR);
	temp1 = DRV_Reg32(EFUSE_TS2_ADDR);
	#if THERMAL_DEBUG_LOG
	LOG(6,"[Power/CPU_Thermal] [Thermal calibration] Reg(0xF00086C8)=0x%x, Reg(0xF00086CC)=0x%x\n", temp0, temp1);
	#endif
	
	g_adc_ge = (temp1 & 0xFFC00000)>>22;
	g_adc_oe = (temp1 & 0x003FF000)>>12;
	
	g_o_vtsmcu1 = (temp0 & 0x03FE0000)>>17;
	g_o_vtsmcu2 = (temp0 & 0x0001FF00)>>8;
	g_o_vtsmcu3 = (temp1 & 0x000001FF);
	
	g_degc_cali = (temp0 & 0x0000007E)>>1;
	g_adc_cali_en = (temp0 & 0x00000001);
	g_o_slope_sign = (temp0 & 0x00000080)>>7;
	g_o_slope = (temp0 & 0xFC000000)>>26;
	
	g_id = (temp1 & 0x00000200)>>9;
	if(g_id==0)
	{
		g_o_slope=0;
	}

	if(g_adc_cali_en == 1)
	{
		//thermal_enable = true;        
	}
	else
	{
		g_adc_ge = 512;
		g_adc_oe = 512;
		g_degc_cali = 40;
		g_o_slope = 0;
		g_o_slope_sign = 0;
		g_o_vtsmcu1 = 260;
		g_o_vtsmcu2 = 260;
		g_o_vtsmcu3 = 260;
	}

	#if THERMAL_DEBUG_LOG	
	LOG(6,"== [Power/CPU_Thermal] [Thermal calibration] == \n g_adc_ge = 0x%x ,\n g_adc_oe = 0x%x,\n g_degc_cali = 0x%x,\n g_adc_cali_en = 0x%x,\n", 
		g_adc_ge, g_adc_oe, g_degc_cali, g_adc_cali_en);
	LOG(6,"g_o_slope = 0x%x,\n g_o_slope_sign = 0x%x,\n g_id = 0x%x\n", 
		g_o_slope, g_o_slope_sign, g_id);	
	LOG(6,"[Thermal calibration] g_o_vtsmcu1 = 0x%x,\n g_o_vtsmcu2 = 0x%x,\n g_o_vtsmcu3 = 0x%x \n",
		g_o_vtsmcu1, g_o_vtsmcu2, g_o_vtsmcu3);
	#endif
}

/**
 * @brief vDrvThermal_Cal_Prepare_2
 * Calculate Room temperature
 * @param  void
 * @retval void
 */
 
/*
T1=T0+(C1-C0)*G/SS=-1.65mV/degC .... (0)
TSUV * 3.2X = TSUV_S1.... (1)
TSUV_S1 / 2.8V = Ideal ADC Value / 4096 .... (2)
Ideal ADC Value * ADC_Gain + ADC offset  = ADC Value.... (3)
*
2800mV/3.2x=28/32* (100*10) = 28/32 * 1000.... (A)
1 degreeC = 0.1 degreeC * 10.... (B)
(A)*(B)=1000*10=10000
*/

void vDrvGetEFuse_ThermalSesnorData(void)
{
	#if THERMAL_DEBUG_LOG
	UINT32 u2Temp_num,u2Temp_point;
	#endif
	
	if (g_adc_cali_en == 1)
	{		
		#if THERMAL_DEBUG_LOG
		LOG(6,"==[EFuse_ThermalSesnorData] ==\n");
		if(g_adc_ge < 512)
		{
			u2Temp_num = ((g_adc_ge - 512) * 1000) / 4096;
		  	u2Temp_point = (u2Temp_num * 4096 - ((g_adc_ge - 512) * 1000)) *100 /4096;
		}	
		else
		{
			u2Temp_num = ((g_adc_ge - 512) * 1000) / 4096;
		  	u2Temp_point = (((g_adc_ge - 512) * 1000) - u2Temp_num * 4096) *100 /4096;
		}		
		
		LOG(6,"TSADC_GE = %d.%d mV\n", u2Temp_num,u2Temp_point);
		
		if(g_adc_oe < 512)
		{
			u2Temp_num = ((g_adc_oe - 512) * 1000) / 4096;
		  	u2Temp_point = (u2Temp_num * 4096 - ((g_adc_oe - 512) * 1000)) *100 /4096;
		}	
		else
		{
			u2Temp_num = ((g_adc_oe - 512) * 1000) / 4096;
		  	u2Temp_point = (((g_adc_oe - 512) * 1000) - u2Temp_num * 4096) *100 /4096;
		}				
		LOG(6,"TSADC_OE = %d.%d mV\n", u2Temp_num,u2Temp_point);
		LOG(6,"TSADC_ADCGE = %d\n", g_adc_ge);
		LOG(6,"TSADC_ADCOE = %d\n", g_adc_oe);		
		LOG(6,"TSADC_Y_VTS1 = %d\n", g_o_vtsmcu1 + TS_LOW_CRITERIAL);
		LOG(6,"TSADC_Y_VTS2 = %d\n", g_o_vtsmcu2 + TS_LOW_CRITERIAL);
		LOG(6,"TSADC_Y_VTS3 = %d\n", g_o_vtsmcu3 + TS_LOW_CRITERIAL);
		LOG(6,"TSADC_O_VTSMCU1 = %d\n", g_o_vtsmcu1);
		LOG(6,"TSADC_O_VTSMCU2 = %d\n", g_o_vtsmcu2);
		LOG(6,"TSADC_O_VTSMCU3 = %d\n", g_o_vtsmcu3);
		LOG(6,"Tsensor_00_DUT = %d\n", g_degc_cali/2);		
		LOG(6,"Tsensor_DEGC_Cali = %d\n", g_degc_cali);
		LOG(6,"g_id = %d\n", g_id);
		LOG(6,"g_o_slope_sign = %d\n", g_o_slope_sign);				
		LOG(6,"g_o_slope = %d\n", g_o_slope);
		LOG(6,"EFUSE_TS1_ADDR = 0x%x\n", DRV_Reg32(EFUSE_TS1_ADDR));
		LOG(6,"EFUSE_TS2_ADDR = 0x%x\n", DRV_Reg32(EFUSE_TS2_ADDR));	
		LOG(6,"g_ge = %d\n", g_ge);
		LOG(6,"g_oe = %d\n", g_oe);		
		LOG(6,"g_gain = %d\n", g_gain);		
		#endif
	}
	else
	{
		#if THERMAL_DEBUG_LOG
		LOG(6,"==[without EFuse_ThermalSesnorData] ==\n");
		#endif
	}
}

static void vDrvThermal_Cal_Prepare_2(void)
{
	kal_int32 format_1, format_2, format_3, format_4= 0;

	//  [FT] ADC_GE[9:0] = GE*4096 + 512 (round to integer)-(1)
	//  [FT] ADC_OE[9:0] = OE*4096 + 512 (round to integer)-(2)
	g_ge = ((g_adc_ge - 512) * 10000 ) / 4096; // ge * 10000
	g_oe = (g_adc_oe - 512);
	g_gain = (10000 + g_ge); // gain * 10000

	// [FT] O_VTSMCU1=Y_VTS1-3192
	// Ideal ADC Value * ADC_Gain + ADC offset  = ADC Value.... (3)
	// Ideal ADC Value * ADC_Gain = ADC Value - ADC offset
	// format_X = Ideal ADC Value * ADC_Gain = (g_o_vtsmcu1 + TS_LOW_CRITERIAL) - (g_oe)
	format_1 = (g_o_vtsmcu1 + TS_LOW_CRITERIAL - g_oe);
	format_2 = (g_o_vtsmcu2 + TS_LOW_CRITERIAL - g_oe);
	format_3 = (g_o_vtsmcu3 + TS_LOW_CRITERIAL - g_oe);
	format_4 = (g_o_vtsmcu3 + TS_LOW_CRITERIAL - g_oe);

	// TSUV_S1 / 2.8V = Ideal ADC Value / 4096 .... (2)
	// g_x_roomt[0]=TSUV_S1 / 2.8V = Ideal ADC Value / 4096 = format_X/ADC_Gain/4096
	g_x_roomt[0] = (((format_1 * 10000) / 4096) * 10000) / g_gain; // x_roomt * 10000
	g_x_roomt[1] = (((format_2 * 10000) / 4096) * 10000) / g_gain; // x_roomt * 10000
	g_x_roomt[2] = (((format_3 * 10000) / 4096) * 10000) / g_gain; // x_roomt * 10000
	g_x_roomt[3] = (((format_4 * 10000) / 4096) * 10000) / g_gain; // x_roomt * 10000

	#if THERMAL_DEBUG_LOG
	LOG(6,"[Power/CPU_Thermal] [Thermal calibration] g_ge = %d, g_oe = %d, g_gain = %d, g_x_roomt1 = %d, g_x_roomt2 = %d, g_x_roomt3 = %d, g_x_roomt4 = %d\n",
		g_ge, g_oe, g_gain, g_x_roomt[0], g_x_roomt[1], g_x_roomt[2], g_x_roomt[3]);
	#endif

	// vDrvGetEFuse_ThermalSesnorData();
}

static UINT32 u4raw_to_temperature_roomt_SingleChannel(UINT32 raw, kal_int32 g_x_roomt)
{
	UINT32 temp=0;
	int y_curr = raw;
	int format_1 = 0;
	int format_2 = 0;
	int format_3 = 0;
	int format_4 = 0;

	format_1 = (g_degc_cali / 2);
	        
	// Ideal ADC Value * ADC_Gain = ADC Value - ADC offset
	format_2 = (y_curr - g_oe);
	// TSUV_S1 / 2.8V = Ideal ADC Value / 4096 = format_X/ADC_Gain/4096
	// Delta g_x_roomt[0]= Ideal ADC Value_T1 - Ideal ADC Value_T0
	format_3 = (((((format_2) * 10000) / 4096) * 10000) / g_gain) - g_x_roomt; // 10000 * format3
	// TSUV * 3.2X = TSUV_S1.... (1)
	// format_3[i] = Delta TSUV = delta Ideal ADC value /3.2*2.8
	format_3 = (format_3 * TS_ADC_INPUT_RANGE) / TS_BUFFER_GAIN;
	//T1=T0+(C1-C0)*G/S.... (0) Note: S=-1.65mV/degC 
	// Delta V=G*DeltaC
	// Delta T = Delta V /S
	if(g_o_slope_sign==0)
	{
		format_4 = ((format_3 * 100) / (165+g_o_slope)); // uint = 0.1 deg
	}
	else
	{
		format_4 = ((format_3 * 100) / (165-g_o_slope)); // uint = 0.1 deg
	}
	format_4 = format_4 - (2 * format_4);

	// Current tempature = Environment tempature + Delta T
	if(y_curr == 0)
	{
		temp = 0;
	}
	else
	{
		temp = (format_1 * 10) + format_4; // uint = 0.1 deg
	}
#if THERMAL_DEBUG_LOG
	LOG(6,"format_2 =%d, format_3 =%d, format_4 =%d,temp=%d\n",format_2, format_3,format_4, temp);
#endif
	return(temp);      
}       

static void raw_to_temperature_roomt(UINT32 raw[], UINT32 temp[])
{
	int i = 0;
	int y_curr[4];
	int format_1 = 0;
	int format_2[4];
	int format_3[4];
	int format_4[4];

	for (i=0; i<4; i++)
	{
		y_curr[i] = raw[i];
		format_2[i] = 0;
		format_3[i] = 0;
		format_4[i] = 0;
	}

	#if THERMAL_DEBUG_LOG
	LOG(6,"[Power/CPU_Thermal]: g_ge = %d, g_oe = %d, g_gain = %d, g_x_roomt1 = %d, g_x_roomt2 = %d, g_x_roomt3 = %d, g_x_roomt4 = %d\n",
		g_ge, g_oe, g_gain, g_x_roomt[0], g_x_roomt[1], g_x_roomt[2], g_x_roomt[3]);
	#endif
	
	format_1 = (g_degc_cali / 2);
	#if THERMAL_DEBUG_LOG	
	LOG(6,"[Power/CPU_Thermal]: format_1=%d\n", format_1);
	#endif

	for (i=0; i<4; i++)
	{
		// Ideal ADC Value * ADC_Gain = ADC Value - ADC offset
		format_2[i] = (y_curr[i] - g_oe);
		// TSUV_S1 / 2.8V = Ideal ADC Value / 4096 = format_X/ADC_Gain/4096
		// Delta g_x_roomt[0]= Ideal ADC Value_T1 - Ideal ADC Value_T0
		format_3[i] = (((((format_2[i]) * 10000) / 4096) * 10000) / g_gain) - g_x_roomt[i]; // 10000 * format3
		// TSUV * 3.2X = TSUV_S1.... (1)
		// format_3[i] = Delta TSUV = delta Ideal ADC value /3.2*2.8
		format_3[i] = (format_3[i] * TS_ADC_INPUT_RANGE) / TS_BUFFER_GAIN;
		//T1=T0+(C1-C0)*G/S.... (0) Note: S=-1.65mV/degC 
		// Delta V=G*DeltaC
		// Delta T = Delta V /S
		if(g_o_slope_sign==0)
		{
			format_4[i] = ((format_3[i] * 100) / (165+g_o_slope)); // uint = 0.1 deg
		}
		else
		{
			format_4[i] = ((format_3[i] * 100) / (165-g_o_slope)); // uint = 0.1 deg
		}
		format_4[i] = format_4[i] - (2 * format_4[i]);

		// Current tempature = Environment tempature + Delta T
		if(y_curr[i] == 0)
		{
			temp[i] = 0;
		}
		else
		{
			temp[i] = (format_1 * 10) + format_4[i]; // uint = 0.1 deg
		}
		#if THERMAL_DEBUG_LOG
		if(i==0)
		LOG(6,"=== [ADC Value] [Delta ADC Value] [Delta Tempauure(0.1 deg)] [Tempauure(0.1 deg)]\n");			
		LOG(6,"format_2[%d]=%d, format_3[%d]=%5d, format_4[%d]=%5d, temp[%d]=%5d\n", i, format_2[i], i, format_3[i],i, format_4[i], i, temp[i]);
		#endif
	}

	return;	
}

static kal_int32 temperature_to_raw_roomt(kal_uint32 ret)
{
	// Ycurr = [(Tcurr - DEGC_cali/2)*(-1)*(165+O_slope)*(18/15)*(1/100000)+X_roomtx]*Gain*4096 + OE
	// Tcurr unit is 0.0001 degC

	kal_int32 t_curr = ret;
	kal_int32 format_1 = 0;
	kal_int32 format_2 = 0;
	kal_int32 format_3[4] = {0};
	kal_int32 format_4[4] = {0};
	kal_int32 i, index = 0, temp = 0;

	if(g_o_slope_sign==0)
	{
		format_1 = t_curr-(g_degc_cali*1000/2);
		format_2 = format_1 * (165+g_o_slope) * TS_BUFFER_GAIN/TS_ADC_INPUT_RANGE;
		format_2 = format_2 - 2*format_2;
		for (i=0; i<4; i++)
		{
			format_3[i] = format_2/1000 + g_x_roomt[i]*10;
			format_4[i] = (format_3[i]*4096/10000*g_gain)/100000 + g_oe;
			#if THERMAL_DEBUG_LOG
			LOG(6,"[Temperature_to_raw_roomt][roomt%d] format_1=%d, format_2=%d, format_3=%d, format_4=%d\n", i, format_1, format_2, format_3[i], format_4[i]);
			#endif
		}
	}
	else
	{
		format_1 = t_curr-(g_degc_cali*1000/2);
		format_2 = format_1 * (165-g_o_slope) * TS_BUFFER_GAIN/TS_ADC_INPUT_RANGE;
		format_2 = format_2 - 2*format_2;
		for (i=0; i<4; i++)
		{
			format_3[i] = format_2/1000 + g_x_roomt[i]*10;
			format_4[i] = (format_3[i]*4096/10000*g_gain)/100000 + g_oe;
			#if THERMAL_DEBUG_LOG
			LOG(6,"[Temperature_to_raw_roomt][roomt%d] format_1=%d, format_2=%d, format_3=%d, format_4=%d\n", i, format_1, format_2, format_3[i], format_4[i]);
			#endif
		}
	}

	temp = 0;
	for (i=0; i<4; i++)
	{
		if (temp < format_4[i])
		{
			temp = format_4[i];
			index = i;
		}
	}
	#if THERMAL_DEBUG_LOG
	LOG(6,"[Temperature_to_raw_roomt] temperature=%d, raw[%d]=%d\n", ret, index, format_4[index]);
	#endif
	return format_4[index];
}

static void vDrvThermal_Calibration(void)
{
	if (g_adc_cali_en == 0)
	{
		#if THERMAL_DEBUG_LOG
		LOG(6,"Not Calibration\n");
		#endif
	}
	else
	{
		#if THERMAL_DEBUG_LOG
		LOG(6,"Calibration Data\n");
		#endif		
	}
	vDrvThermal_Cal_Prepare_2();
}

#define TS_SMA_ENABLE 0
#if TS_SMA_ENABLE
#define SMA_NUMBER 6
int TS_Temp[4][SMA_NUMBER+1]; // TS_Temp[][SMA_NUMBER+1]: New TS_Temp[][0]: Oldest
UINT8 fgSMAInit=0;
/**
* @brief simple moving average formula
* @param raw[]: raw value
* @retval temp_ave[]: average value
*/
static void TS_SMA(UINT32 raw[], UINT32 temp_ave[])
{
    UINT8 TS_TSIdx=0, TS_SampleIdx=0;
    UINT32 TS_SUM=0;
    
    if(fgSMAInit)
    {
        // add new value         
    	for (TS_TSIdx=0; TS_TSIdx<4; TS_TSIdx++)
        {
             TS_Temp[TS_TSIdx][SMA_NUMBER]=raw[TS_TSIdx];            
       	     LOG(6,"-->TS_Temp[%d][SMA_NUMBER]=0x%x\n", TS_TSIdx,TS_Temp[TS_TSIdx][SMA_NUMBER]);             
        }                                
    }    
    else
    {
        // initial matrix
    	for (TS_TSIdx=0; TS_TSIdx<4; TS_TSIdx++)
    	{
    	 for (TS_SampleIdx=0; TS_SampleIdx<(SMA_NUMBER+1); TS_SampleIdx++)
    	 {
             TS_Temp[TS_TSIdx][TS_SampleIdx]=raw[TS_TSIdx];
         }  
        }  
        fgSMAInit = 1;              
    }        
                        
	for (TS_TSIdx=0; TS_TSIdx<4; TS_TSIdx++)
	{
     TS_SUM=0;   
	 for (TS_SampleIdx=0; TS_SampleIdx<SMA_NUMBER; TS_SampleIdx++)
	 {
         // remove the old temp + accumulat valid value
         TS_Temp[TS_TSIdx][TS_SampleIdx]=TS_Temp[TS_TSIdx][(TS_SampleIdx+1)];
         TS_SUM += TS_Temp[TS_TSIdx][TS_SampleIdx];
     }
     temp_ave[TS_TSIdx] = TS_SUM/SMA_NUMBER;  
    }  

    // debug use        
    /*
	for (TS_TSIdx=0; TS_TSIdx<4; TS_TSIdx++)
	{
	 for (TS_SampleIdx=0; TS_SampleIdx<(SMA_NUMBER+1); TS_SampleIdx++)
	 {
   	     LOG(6,"0x%x,", TS_Temp[TS_TSIdx][TS_SampleIdx]);              
     }  
   	     LOG(6,"\n");         
    }
	*/
}
#endif

static UINT8 u1PTPValueMode = E_PTP_AVERAGE; // 0: from PTP 1: from SRVADC

void vDrvSetPTPADCMode(UINT8 u1Mode)
{	
 if (u1Mode == 0)
 {
	vIO32WriteFldMulti(TEMPMSRCTL1, P_Fld(0,TEMPMSRCTL1_MSR0IMM)|P_Fld(0,TEMPMSRCTL1_MSR1IMM)|P_Fld(0,TEMPMSRCTL1_MSR2IMM));		 
 	u1PTPValueMode = E_PTP_AVERAGE;
 }	
 else if (u1Mode == 1)
 {
	vIO32WriteFldMulti(TEMPMSRCTL1, P_Fld(1,TEMPMSRCTL1_MSR0IMM)|P_Fld(1,TEMPMSRCTL1_MSR1IMM)|P_Fld(1,TEMPMSRCTL1_MSR2IMM));		 
 	u1PTPValueMode = E_PTP_IMM; 
 }	
 else
 {
	vIO32WriteFldMulti(TEMPMSRCTL1, P_Fld(0,TEMPMSRCTL1_MSR0IMM)|P_Fld(0,TEMPMSRCTL1_MSR1IMM)|P_Fld(0,TEMPMSRCTL1_MSR2IMM)); 
 	u1PTPValueMode = E_SRVAD_IMM;
 }	
}

#define FLD_ADC12BIT Fld(12,0,AC_MSKW10)//[11:0]
static UINT32 _u4GetADCValue(UINT8 u1CH)
{
	UINT32 u4Temp = TS_LOW_CRITERIAL;

	if(u1CH == 0)
	{
		u4Temp = IO32ReadFldAlign(PDWNC_ADOUT17,FLD_ADC12BIT);
	}
	else if (u1CH == 1)
	{
		u4Temp = IO32ReadFldAlign(PDWNC_ADOUT1B,FLD_ADC12BIT);
	}
	else
	{
		u4Temp = IO32ReadFldAlign(PDWNC_ADOUT1F,FLD_ADC12BIT);
	}

	LOG(6,"u4Temp=%d\n",u4Temp);
	return(u4Temp);
}

static void DrvGetSRTemp(UINT8 u1Id)
{
	UINT32 curr_raw;
	if(u1Id == 0)
	{
		curr_raw = DRV_Reg32(TEMPMSR0);		
	}
	else if(u1Id == 1)
	{
		curr_raw = DRV_Reg32(TEMPMSR1);
	}
	else
	{
		curr_raw = DRV_Reg32(TEMPMSR2);
	}

	TS_curr_raw[u1Id] = u4raw_to_temperature_roomt_SingleChannel((curr_raw & 0x0fff),g_x_roomt[u1Id])*100;
	//Printf("TS_curr_raw[u1Id]=%d\n",TS_curr_raw[u1Id]);
	tz_last_values[u1Id] = &TS_curr_raw[u1Id];
}

UINT32 CPU_Temp(void)
{
	UINT32 curr_raw[4], curr_temp[4];
	UINT32 con, index = 0, max_temp;
	#if TS_SMA_ENABLE
	UINT32 curr_average[4];
	#endif

	if ((!fgIsPTPInitial()) && (BSP_GetIcVersion() >= IC_VER_5890_AC))
	{
		max_temp = 25000;
		Printf("Please Initial PTP First\n");
		return max_temp;
	}
	
#if 1
	if(u1PTPValueMode == E_SRVAD_IMM)
	{
		curr_raw[0] = _u4GetADCValue(0);
		curr_raw[1] = _u4GetADCValue(1);
		curr_raw[2] = _u4GetADCValue(2);
		curr_raw[3] = _u4GetADCValue(2); // Let the temp3 same as temp to easy code
	}
	else if (u1PTPValueMode == E_PTP_IMM)
	{
		curr_raw[0] = DRV_Reg32(TEMPIMMD0);
		curr_raw[1] = DRV_Reg32(TEMPIMMD1);
		curr_raw[2] = DRV_Reg32(TEMPIMMD2);
		curr_raw[3] = curr_raw[2]; // Let the temp3 same as temp to easy code
	}	
	else
	{
		curr_raw[0] = DRV_Reg32(TEMPMSR0);
		curr_raw[1] = DRV_Reg32(TEMPMSR1);
		curr_raw[2] = DRV_Reg32(TEMPMSR2);
		curr_raw[3] = curr_raw[2]; // Let the temp3 same as temp to easy code
	}
#else // curr_temp[0]:1200, curr_temp[1]:400, curr_temp[2]:102
	curr_raw[0] = 2679; // read temp0
	curr_raw[1] = 3297; // read temp1
	curr_raw[2] = 3528; // read temp2
   	curr_raw[3] = 3528; // read temp2
#endif

	curr_raw[0] = curr_raw[0] & 0x0fff;
	curr_raw[1] = curr_raw[1] & 0x0fff;
	curr_raw[2] = curr_raw[2] & 0x0fff;
	curr_raw[3] = curr_raw[3] & 0x0fff;
	
	#if TS_SMA_ENABLE
	TS_SMA(curr_raw, curr_average);
	TS_curr_raw[0]= curr_average[0];
	TS_curr_raw[1]= curr_average[1];
	TS_curr_raw[2]= curr_average[2];
	TS_curr_raw[3]= curr_average[3];
	#else
	TS_curr_raw[0]= curr_raw[0];
	TS_curr_raw[1]= curr_raw[1];
	TS_curr_raw[2]= curr_raw[2];
	TS_curr_raw[3]= curr_raw[3];
	#endif

	for (con=0; con<MTK_THERMAL_SENSOR_COUNT; con++)
	{
		tz_last_values[con] = &TS_curr_raw[con];
	}

	#if TS_SMA_ENABLE
	raw_to_temperature_roomt(curr_average, curr_temp);
	#else
	raw_to_temperature_roomt(curr_raw, curr_temp);
	#endif
	curr_temp[0] = curr_temp[0]*100;
	curr_temp[1] = curr_temp[1]*100;
	curr_temp[2] = curr_temp[2]*100;
	curr_temp[3] = curr_temp[3]*100;

	max_temp = 0;
	for (con=0; con<4; con++)
	{
		if (max_temp < curr_temp[con])
		{
			max_temp = curr_temp[con];
			index = con;
		}
	}
	
	#if THERMAL_DEBUG_LOG
	LOG(6,"[CPU_Temp] \ntemp[0]=%6d, raw[0]=%d,\ntemp[1]=%6d, raw[1]=%d,\ntemp[2]=%6d, raw[2]=%d,\ntemp[3]=%6d, raw[3]=%d\n", 
		curr_temp[0], curr_raw[0], curr_temp[1], curr_raw[1], curr_temp[2], curr_raw[2], curr_temp[3], curr_raw[3]);
	LOG(6,"[CPU_Temp] max_temp(0.001 degree)=%d, ts_index=%d\n", max_temp, index);
	#endif
	return max_temp;
}

static int mtktscpu_get_TC_temp(void)
{
//	int curr_raw, curr_temp;
	int /*ret=0,*/ len=0, i=0;
	int t_ret1=0;
	static int abb[60]={0};
	
	if(proc_write_flag==1)
	{
		MA_counter=0, MA_first_time=0;
		MA_len=MA_len_temp;
		proc_write_flag=0;
		#if THERMAL_DEBUG_LOG
		LOG(6,"[mtktscpu_get_hw_temp]:MA_len=%d",MA_len);
		#endif
	}
	
	t_ret1 = CPU_Temp();
	abb[MA_counter] = t_ret1;
//	mtktscpu_dprintk("[mtktscpu_get_temp] temp=%d, raw=%d\n", t_ret1, curr_raw);
	
	if(MA_counter==0 && MA_first_time==0 && MA_len!=1)
	{
		MA_counter++;

		t_ret1 = CPU_Temp();
		abb[MA_counter] = t_ret1;
//		mtktscpu_dprintk("[mtktscpu_get_temp] temp=%d, raw=%d\n", t_ret1, curr_raw);
	}
	MA_counter++;
	if(MA_first_time==0)
		len = MA_counter;
	else
		len = MA_len;

	t_ret1 = 0;
	for(i=0; i<len; i++)
	{
		t_ret1 += abb[i];
	}
	last_abb_t = t_ret1 / len;

	#if THERMAL_DEBUG_LOG
	LOG(6,"[mtktscpu_get_hw_temp] MA_ABB=%d, \n", last_abb_t);
	LOG(6,"[mtktscpu_get_hw_temp] MA_counter=%d, MA_first_time=%d, MA_len=%d\n", MA_counter, MA_first_time, MA_len);
	#endif

	if(MA_counter==MA_len )
	{
		MA_counter=0;
		MA_first_time=1;
	}

	return last_abb_t;
}

UINT8 mtktscpu_get_temp(void)
{
	int curr_temp;
	
	curr_temp = mtktscpu_get_TC_temp();
	//curr_temp += mtktscpu_base_temp;

	if((curr_temp>100000) | (curr_temp<-30000))
	#if THERMAL_DEBUG_LOG		
	LOG(6,"[Power/CPU_Thermal] CPU T=%d\n",curr_temp);
	#endif

	//curr_temp = mtktscpu_get_hw_temp();
	//*t = (unsigned long) curr_temp;



	g_prev_temp = g_curr_temp;
	g_curr_temp = curr_temp;

	CPU_TEMP_STATUS.prev_temp = g_prev_temp;	
	CPU_TEMP_STATUS.curr_temp = g_curr_temp;	
	
	return 0;
}

#if defined(CC_FAST_INIT) || defined(CC_SUPPORT_STR)
void vDrvThermal_pm_suspend(void)	
{
	int temp = 0;
	int cnt = 0;

	#if THERMAL_DEBUG_LOG
	LOG(6,"[mtk_thermal_suspend] \n");
	#endif

	cnt = 0;
	while(cnt < 10) //if bit7 and bit0=0
	{
		temp = DRV_Reg32(TEMPMSRCTL1);
		#if THERMAL_DEBUG_LOG
		LOG(6,"TEMPMSRCTL1 = 0x%x\n", temp);
		#endif
		/*
		TEMPMSRCTL1[7]:Temperature measurement bus status[1]
		TEMPMSRCTL1[0]:Temperature measurement bus status[0]

		00: IDLE                           <=can pause    ,TEMPMSRCTL1[7][0]=0x00
		01: Write transaction				 <=can not pause,TEMPMSRCTL1[7][0]=0x01
		10: Read transaction				 <=can not pause,TEMPMSRCTL1[7][0]=0x10
		11: Waiting for read after Write   <=can pause    ,TEMPMSRCTL1[7][0]=0x11
		*/
		if(((temp & 0x81) == 0x00) || ((temp & 0x81) == 0x81)){
			/*
			Pause periodic temperature measurement for sensing point 0,sensing point 1,sensing point 2
			*/
			DRV_WriteReg32(TEMPMSRCTL1, (temp | 0x0E));//set bit1=bit2=bit3=1 to pause sensing point 0,1,2

			break;
		}
		#if THERMAL_DEBUG_LOG
		LOG(6,"temp=0x%x, cnt=%d\n",temp,cnt);
		#endif
		udelay(10);
		cnt++;
	}
	
	// disable ALL periodoc temperature sensing point
	DRV_WriteReg32(TEMPMONCTL0, 0x00000000);
	vIO32WriteFldAlign(TEMPSPARE0, 0, TEMPSPARE0_INITIAL);
//	DRV_WriteReg32(TS_CON0_V, DRV_Reg32(TS_CON0_V) | 0x000000C0); // turn off the sensor buffer to save power
	
}

void vDrvThermal_pm_resume(void)
{
	#if THERMAL_DEBUG_LOG
	LOG(6,"[mtk_thermal_resume] \n");
	#endif
	vDrvThermalInit();
}
#endif

void vDrvThermalInit(void)
{
    if (BSP_GetIcVersion() >= IC_VER_5890_AC)
    {
		vDrvThermal_Fast_Init(); // fix the system reboot at high temp and stage 3 can't trigger watch dog.

		vDrvThermal_Cal_Prepare();
		vDrvThermal_Calibration();
		
		vDrvThermal_Reset_and_Initial();
		vIO32WriteFldAlign(TEMPSPARE0, 1, TEMPSPARE0_INITIAL);	
		CPU_Temp();

		Set_Thermal_Ctrl_Trigger(trip_temp[0]);
		#ifdef __KERNEL__
	    met_thermal_ext_api.mtk_thermal_get_temp=mtk_thermal_get_temp_driver;
		#endif

		vDrvThermalInitISR();
    }	
	else
	{
		vDrvThermal_Cal_Prepare();
		vDrvThermal_Calibration();
		vIO32WriteFldAlign(PDWNC_SRVCFG1, 0xFFFF, HW_TS_CHANNEL_EN);
		vDrvSetPTPADCMode(E_SRVAD_IMM);
		CPU_Temp();		
	}	
}

#define PTP_CKGEN_REG_TOP_BACKUP4 (0xF000D5D0)
		#define PTP_CKGEN_RESET Fld(1,14,AC_MSKB1)//[14:14]
		
static void vDrvThermal_Control_Reset(void)
{
	UINT8 TSStatus1 = 0, TSStatus2 = 0; 
	LOG(6,"[vDrvThermal_Control_Reset] \n");
	do
	{
		TSStatus1 = IO32ReadFld(TEMPMSRCTL1, TEMPMSRCTL1_MSRSTS0);
		TSStatus2 = IO32ReadFld(TEMPMSRCTL1, TEMPMSRCTL1_MSRSTS1);		
	}while((TSStatus2!=0)||(TSStatus1!=0));
	
	vIO32WriteFldMulti(TEMPMSRCTL1, P_Fld(1,TEMPMSRCTL1_PAUSE0)|P_Fld(1,TEMPMSRCTL1_PAUSE1)|P_Fld(1,TEMPMSRCTL1_PAUSE2));
	vIO32WriteFldAlign(PTP_CKGEN_REG_TOP_BACKUP4, 0, PTP_CKGEN_RESET);
	vIO32WriteFldAlign(PTP_CKGEN_REG_TOP_BACKUP4, 1, PTP_CKGEN_RESET);
	vDrvThermalInit();
}

unsigned int get_thermal(unsigned int *value)
{
	int j = -1;

	value[++j]=mtk_thermal_get_temp_driver(MTK_THERMAL_SENSOR_CPU0); //CPU0
	value[++j]=mtk_thermal_get_temp_driver(MTK_THERMAL_SENSOR_CPU1); //CPU1
	value[++j]=mtk_thermal_get_temp_driver(MTK_THERMAL_SENSOR_CPU2); //CPU2

	return j+1;
}

int mtk_thermal_get_temp_driver(MTK_THERMAL_SENSOR_ID id)
{
    if (id >= MTK_THERMAL_SENSOR_COUNT) // fix compiler warning if (id < 0 || id >= MTK_THERMAL_SENSOR_COUNT)
        return -127000;
    else
    {
		#ifndef CC_MTK_LOADER
        CRIT_STATE_T csState;
        csState = x_crit_start(); // mutex_lock(&MTM_GET_TEMP_LOCK);
		#endif
        if (tz_last_values[id] == NULL)
        {
	        #ifndef CC_MTK_LOADER
    	    x_crit_end(csState); // mutex_unlock(&MTM_GET_TEMP_LOCK);
        	#endif             
            return -127000;
        }
        else
        {
            int ret;        
	        DrvGetSRTemp(id);
            ret = *tz_last_values[id];
			#ifndef CC_MTK_LOADER
			x_crit_end(csState); // mutex_unlock(&MTM_GET_TEMP_LOCK);
			#endif
            return ret;
        }
    }
}

static HANDLE_T _hThermalCtrlQueue = NULL_HANDLE;
static HANDLE_T _hThermalCtrlThread = NULL_HANDLE;

// Thermal queue and thread paramenters
#define THERMAL_QUEUE_NAME                  "THERMAL"
#define THERMAL_QUEUE_SIZE                  1024
#define THERMAL_MSG_PRIORITY_NORMAL         128

static PFN_IMAGE _pfnCallback= NULL;

typedef struct
{
    UINT32 u4ThermalISRCase;
} THERMAL_MSG_T;

static THERMAL_MSG_T rThermal;

void RegThermalCtrlCallback(PFN_IMAGE pfnCallback)
{
    _pfnCallback = pfnCallback;
}
extern void thermal_lisr(void);
void vDrvThermalISR(UINT16 u2Vector)
{			
    if(u2Vector == VECTOR_PTP_THERM)  
    {
		// send msg		
		if (x_msg_q_send(_hThermalCtrlQueue, (VOID*)&rThermal, sizeof (THERMAL_MSG_T),
			THERMAL_MSG_PRIORITY_NORMAL) != OSR_OK)
		{
        	#if THERMAL_DEBUG_LOG
            LOG(6,"Fail to send thermal msg!\n");
			#endif		
		}	
		thermal_lisr();
		// LOG(6,"[ISR]\n");
    }
    VERIFY(BIM_ClearIrq(VECTOR_PTP_THERM));
}

static void _ThermalThread(VOID* pvArg)
{
	INT32 i4Ret;
	UINT16 u2QueueIndex;
	SIZE_T zMsgSize;
	UINT32 u4Value=0;

   // receive msg
    while (1)
    {
        zMsgSize = sizeof (rThermal);    
        i4Ret = x_msg_q_receive(&u2QueueIndex, (VOID*)&rThermal, &zMsgSize, &_hThermalCtrlQueue, 1, X_MSGQ_OPTION_WAIT);

        // send callback
        u4Value=rThermal.u4ThermalISRCase;
		if ( i4Ret==OSR_OK)
		{
			 // send callback
			 u4Value=rThermal.u4ThermalISRCase;
			 if(_pfnCallback!= NULL)
			{
			 _pfnCallback(u4Value);
			}		 
		}		
    }
}


/** Brief
 * Thermal ISR initial
 * @return void
 */
void vDrvThermalInitISR(void)
{
#ifndef CC_MTK_LOADER 
	static BOOL u1ThermalISRInited = SV_FALSE;

	x_os_isr_fct pfnOldIsr=NULL;
	if (!u1ThermalISRInited)
	{

		rThermal.u4ThermalISRCase = 0;
		
        // Create thermal queues       
        if (x_msg_q_create(&_hThermalCtrlQueue, THERMAL_QUEUE_NAME, sizeof (THERMAL_MSG_T), THERMAL_QUEUE_SIZE) != OSR_OK)
        {
        	#if THERMAL_DEBUG_LOG
            LOG(6,"Fail to create thermal queue!\n");
			#endif
      }
		
        // Create thermal thread
        if (x_thread_create(&_hThermalCtrlThread, THERMAL_THREAD_NAME, THERMAL_STACK_SIZE, THERMAL_THREAD_PRIORITY, _ThermalThread, 0,
            NULL) != OSR_OK)
        {
        	#if THERMAL_DEBUG_LOG
            LOG(6,"Fail to create thermal thread!\n");
			#endif
        }

		if (x_reg_isr(VECTOR_PTP_THERM, vDrvThermalISR, &pfnOldIsr) != OSR_OK)
		{
			#if THERMAL_DEBUG_LOG
			LOG(6,"[Thermal] Error: Fail to register Thermal ISR!\n");
			#endif
		}
		else
		{
			#if THERMAL_DEBUG_LOG
			LOG(6,"[Thermal] Success to register Thermal ISR!\n");
			#endif
		}

		u1ThermalISRInited = SV_TRUE;
	}
#endif	
}

#define false SV_FALSE
#define true SV_TRUE

BOOL thermal_real_test = false;
BOOL thermal_intr_flag = false;


static void kal_sleep_task(kal_uint32 time_in_ticks)
{
    kal_uint32 i;
    if (time_in_ticks)
        for (i = 0; i < 1000 * time_in_ticks; i++) {}
}

static void non_volatile_kal_sleep_task(kal_uint32 time_in_ticks)
{
    kal_uint32 i;
    if (time_in_ticks)
        for (i = 0; i < 1000 * time_in_ticks; i++) {}
}


/*******************************************************************************
 * FUNCTION
 *	thermal_exit
 *
 * DESCRIPTION
 *	This function restore the default value for all registers
 *
 * PARAMETERS
 *  None
 *
 * RETURNS
 *	None
 ******************************************************************************/
void thermal_exit(void)
{
   
    LOG(6,"thermal_exit: thermal de-initialized, restore to the default value\n");        
    
    DRV_WriteReg32(TEMPMONCTL0,         0x00000000);
    DRV_WriteReg32(TEMPMONCTL1,         0x00000000);
    DRV_WriteReg32(TEMPMONCTL2,         0x00000000);
    DRV_WriteReg32(TEMPMONINT,          0x00000000);
    DRV_WriteReg32(TEMPMONIDET0,        0x00000000);
    DRV_WriteReg32(TEMPMONIDET1,        0x00000000);
    DRV_WriteReg32(TEMPMONIDET2,        0x00000000);
    DRV_WriteReg32(TEMPH2NTHRE,         0x00000000);
    DRV_WriteReg32(TEMPHTHRE,           0x00000000);
    DRV_WriteReg32(TEMPCTHRE,           0x00000000);
    DRV_WriteReg32(TEMPOFFSETH,         0x00000000);
    DRV_WriteReg32(TEMPOFFSETL,         0x00000000);
    DRV_WriteReg32(TEMPMSRCTL0,         0x00000000);
    DRV_WriteReg32(TEMPMSRCTL1,         0x00000000);
    DRV_WriteReg32(TEMPAHBPOLL,         0x00000000);
    DRV_WriteReg32(TEMPAHBTO,           0xFFFFFFFF);
    DRV_WriteReg32(TEMPADCPNP0,         0x00000000);
    DRV_WriteReg32(TEMPADCPNP1,         0x00000000);
    DRV_WriteReg32(TEMPADCPNP2,         0x00000000);
    DRV_WriteReg32(TEMPADCMUX,          0x00000000);
    DRV_WriteReg32(TEMPADCEXT,          0x00000000);
    DRV_WriteReg32(TEMPADCEXT1,         0x00000000);
    DRV_WriteReg32(TEMPADCEN,           0x00000000);
    DRV_WriteReg32(TEMPPNPMUXADDR,      0x00000000);
    DRV_WriteReg32(TEMPADCMUXADDR,      0x00000000);
    DRV_WriteReg32(TEMPADCEXTADDR,      0x00000000);
    DRV_WriteReg32(TEMPADCEXT1ADDR,     0x00000000);
    DRV_WriteReg32(TEMPADCENADDR,       0x00000000);
    DRV_WriteReg32(TEMPADCVALIDADDR,    0x00000000);
    DRV_WriteReg32(TEMPADCVOLTADDR,     0x00000000);
    DRV_WriteReg32(TEMPRDCTRL,          0x00000000);
    DRV_WriteReg32(TEMPADCVALIDMASK,    0x00000000);
    DRV_WriteReg32(TEMPADCVOLTAGESHIFT, 0x00000000);
    DRV_WriteReg32(TEMPADCWRITECTRL,    0x00000000);
 
    DRV_WriteReg32(TEMPPROTCTL,         0x00000000);
    DRV_WriteReg32(TEMPPROTTA,          0x00000000);
    DRV_WriteReg32(TEMPPROTTB,          0x00000000);
    DRV_WriteReg32(TEMPPROTTC,          0x00000000);
    
    DRV_WriteReg32(TEMPSPARE0,          0x00000000);
    DRV_WriteReg32(TEMPSPARE1,          0x00000000);
    DRV_WriteReg32(TEMPSPARE2,          0x00000000);
    
    LOG(6,"Reset Thermal Controller\n\n");

}

/*******************************************************************************
 * FUNCTION
 *	thermal_lisr
 *
 * DESCRIPTION
 *	This function will handle and identify interrupt trigger type
 *
 * PARAMETERS
 *  None
 *
 * RETURNS
 *	None
 ******************************************************************************/
void thermal_lisr(void)
{
    kal_uint32 temp = 0, ret = 0;
    kal_uint32 temp1 = 0;
    
    ret = DRV_Reg32(TEMPMONINTSTS);    
    // LOG(6,"thermal_lisr: thermal interrupt trigger, status = 0x%x\n", ret);
    rThermal.u4ThermalISRCase = ret;
	
    if(ret & THERMAL_tri_SPM_State0)
    	LOG(6,"thermal_lisr: Thermal state1 to trigger SPM state1(COLD)\n");
    if(ret & THERMAL_tri_SPM_State1)
    	LOG(6,"thermal_lisr: Thermal state2 to trigger SPM state2(NORMAL)\n");	
    if(ret & THERMAL_tri_SPM_State2)
    	LOG(6,"thermal_lisr: Thermal state3 to trigger SPM state3(HOT)\n");
    
    if (ret & THERMAL_MON_CINTSTS0)
    {
        LOG(6,"thermal_lisr: thermal sensor point 0 - cold interrupt trigger\n");
    }
    if (ret & THERMAL_MON_HINTSTS0)
    {	
        LOG(6,"thermal_lisr: thermal sensor point 0 - hot interrupt trigger\n");
    }
    if (ret & THERMAL_MON_LOINTSTS0)
        LOG(6,"thermal_lisr: thermal sensor point 0 - low offset interrupt trigger\n");
    if (ret & THERMAL_MON_HOINTSTS0)
    {	
        LOG(6,"thermal_lisr: thermal sensor point 0 - high offset interrupt trigger\n");
    }
    if (ret & THERMAL_MON_NHINTSTS0)
        LOG(6,"thermal_lisr: thermal sensor point 0 - hot to normal interrupt trigger\n");
    
    if (ret & THERMAL_MON_CINTSTS1)
        LOG(6,"thermal_lisr: thermal sensor point 1 - cold interrupt trigger\n");
    if (ret & THERMAL_MON_HINTSTS1)
    {	
        LOG(6,"thermal_lisr: thermal sensor point 1 - hot interrupt trigger\n");
    }
    if (ret & THERMAL_MON_LOINTSTS1)
        LOG(6,"thermal_lisr: thermal sensor point 1 - low offset interrupt trigger\n");
    if (ret & THERMAL_MON_HOINTSTS1)
    {	
        LOG(6,"thermal_lisr: thermal sensor point 1 - high offset interrupt trigger\n");
    }
    if (ret & THERMAL_MON_NHINTSTS1)
        LOG(6,"thermal_lisr: thermal sensor point 1 - hot to normal interrupt trigger\n");
        
    if (ret & THERMAL_MON_CINTSTS2)
        LOG(6,"thermal_lisr: thermal sensor point 2 - cold interrupt trigger\n");
    if (ret & THERMAL_MON_HINTSTS2)
        LOG(6,"thermal_lisr: thermal sensor point 2 - hot interrupt trigger\n");
    if (ret & THERMAL_MON_LOINTSTS2)
        LOG(6,"thermal_lisr: thermal sensor point 2 - low offset interrupt trigger\n");
    if (ret & THERMAL_MON_HOINTSTS2)
        LOG(6,"thermal_lisr: thermal sensor point 2 - high offset interrupt trigger\n");
    if (ret & THERMAL_MON_NHINTSTS2)
        LOG(6,"thermal_lisr: thermal sensor point 2 - hot to normal interrupt trigger\n");   
        
    if (ret & THERMAL_MON_TOINTSTS)
    {
       	LOG(6,"thermal_lisr: timeout interrupt trigger\n");		    
		if (!(IS_IC_5890_ES1()|| IS_IC_5890_MPS1()))
		{
			vDrvThermal_Control_Reset();
		}
    }	
        
    if (ret & THERMAL_MON_IMMDINTSTS0)
    {
        temp = DRV_Reg32(TEMPMONINT);
        temp1 = temp & 0xFFFEFFFF;
        DRV_WriteReg32(TEMPMONINT, temp1); // disable immediate interrupt for sense point 0
        
        LOG(6,"thermal_lisr: thermal sensor point 0 - immediate sense interrupt trigger\n");
        
        if (!thermal_real_test)
        {
            temp = DRV_Reg32(TEMPIMMD0);
            while ((temp & 0x8000) == 0)
                temp = DRV_Reg32(TEMPIMMD0);
            if ((temp & 0x0FFF) != 0x14) // first filter valid should be equal to 0x14
                LOG(6,"thermal_lisr: fail, read TEMPIMMD0 = 0x%x\n", (temp & 0x0FFF));
            else
                LOG(6,"thermal_lisr: pass, read TEMPIMMD0 = 0x%x\n", (temp & 0x0FFF));
        }

        DRV_WriteReg32(TEMPMONINT, temp); // enable immediate interrupt for sense point 0
    }
    if (ret & THERMAL_MON_IMMDINTSTS1)
    {
        temp = DRV_Reg32(TEMPMONINT);
        temp1 = temp & 0xFFFDFFFF;
        DRV_WriteReg32(TEMPMONINT, temp1); // disable immediate interrupt for sense point 1
        
        LOG(6,"thermal_lisr: thermal sensor point 1 - immediate sense interrupt trigger\n");
        
        if (!thermal_real_test)
        {
            temp = DRV_Reg32(TEMPIMMD1);
            while ((temp & 0x8000) == 0)
                temp = DRV_Reg32(TEMPIMMD1);
            if ((temp & 0x0FFF) != 0x14) // first filter valid should be equal to 0x14
                LOG(6,"thermal_lisr: fail, read TEMPIMMD1 = 0x%x\n", (temp & 0x0FFF));
            else
                LOG(6,"thermal_lisr: pass, read TEMPIMMD1 = 0x%x\n", (temp & 0x0FFF));
        }

        DRV_WriteReg32(TEMPMONINT, temp); // enable immediate interrupt for sense point 1
    }
    if (ret & THERMAL_MON_IMMDINTSTS2)
    {
        temp = DRV_Reg32(TEMPMONINT);
        temp1 = temp & 0xFFFBFFFF;
        DRV_WriteReg32(TEMPMONINT, temp1); // disable immediate interrupt for sense point 2
        
        LOG(6,"thermal_lisr: thermal sensor point 2 - immediate sense interrupt trigger\n");
        
        if (!thermal_real_test)
        {
            temp = DRV_Reg32(TEMPIMMD2);
            while ((temp & 0x8000) == 0)
                temp = DRV_Reg32(TEMPIMMD2);
            if ((temp & 0x0FFF) != 0x14) // first filter valid should be equal to 0x14
                LOG(6,"thermal_lisr: fail, read TEMPIMMD2 = 0x%x\n", (temp & 0x0FFF));
            else
                LOG(6,"thermal_lisr: pass, read TEMPIMMD2 = 0x%x\n", (temp & 0x0FFF));\
        }

        DRV_WriteReg32(TEMPMONINT, temp); // enable immediate interrupt for sense point 2
    }
        
    if (ret & THERMAL_MON_FILTINTSTS0) // just for debug
    {
        LOG(6,"thermal_lisr: thermal sensor point 0 - filter sense interrupt trigger\n");
    }    
    if (ret & THERMAL_MON_FILTINTSTS1) // just for debug
    {
        LOG(6,"thermal_lisr: thermal sensor point 1 - filter sense interrupt trigger\n");
    }
    if (ret & THERMAL_MON_FILTINTSTS2) // just for debug
    {
        LOG(6,"thermal_lisr: thermal sensor point 2 - filter sense interrupt trigger\n");
    }
    
    thermal_intr_flag = true; 
	// LOG(6,"thermal_lisr(): thermal_intr_flag = true\n");
}

int thermal_interrupt_occurrance_test(UINT16 times)
{
    UINT32 temp = 0, i = 0;
    
    thermal_real_test = false;
    
	vDrvThermalInitISR();

	DRV_WriteReg32(TEMPMONCTL1, 0x000000CC); // 3FF);    // counting unit is 1024 / 66M = 15.5us
	DRV_WriteReg32(TEMPMONCTL2, 0x02190219); // 219);	// sensing interval is 537 * 15.5us = 8.3235ms
	DRV_WriteReg32(TEMPAHBPOLL,  0x00000001);
	DRV_WriteReg32(TEMPAHBTO,    0x00000333);
	DRV_WriteReg32(TEMPMONIDET0, times);
	DRV_WriteReg32(TEMPMONIDET1, times);
	DRV_WriteReg32(TEMPMONIDET2, times);
	DRV_WriteReg32(TEMPH2NTHRE,  0x000000FF);	// set hot to normal threshold
	DRV_WriteReg32(TEMPHTHRE,    0x000000AA);	// set hot threshold
	DRV_WriteReg32(TEMPCTHRE,    0x00000133);	// set cold threshold	
	DRV_WriteReg32(TEMPOFFSETH,  0x000000BB);   // set high offset threshold	
	DRV_WriteReg32(TEMPOFFSETL,  0x00000111);	// set low offset threshold		
	DRV_WriteReg32(TEMPMSRCTL0,  0x00000000);	// DRV_WriteReg32(TEMPMSRCTL0,  0x000006DB);	

	DRV_WriteReg32(TEMPADCPNP0, 0x172);
	DRV_WriteReg32(TEMPADCPNP1, 0x1B2);
	DRV_WriteReg32(TEMPADCPNP2, 0x1F2);
	
	DRV_WriteReg32(TEMPADCMUX, 0x800);		
	DRV_WriteReg32(TEMPADCEXT, 0x800);		 
	DRV_WriteReg32(TEMPADCEN, 0x10FF);
	
	DRV_WriteReg32(TEMPPNPMUXADDR, SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCENADDR, SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCVALIDADDR,SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCVOLTADDR, SPARE_REPLAC);
	
	DRV_WriteReg32(TEMPRDCTRL, 0x1);			 
	DRV_WriteReg32(TEMPADCVALIDMASK, 0x0000002C);
	DRV_WriteReg32(TEMPADCVOLTAGESHIFT, 0x0);
	DRV_WriteReg32(TEMPADCWRITECTRL, 0x00000000);
	
	DRV_WriteReg32(TEMPMONINT,		 0x0000FFFF); // 0x00000000); // 0xFFFFFFFF);
	DRV_WriteReg32(TEMPMONCTL0, 	 0x00000007); // 0x0000000F

    temp = 0x14;                                           // set to very hot for pnp0
    DRV_WriteReg32(TEMPADCEN, (0x00001000 + temp));        // set sensor voltage and sensor valid

    while ((temp & 0x0FFF) != 0x14)
    {
        kal_sleep_task(10000);
        temp = DRV_Reg32(TEMPMSR0);
    }
    
    for (i = 0; i < 5; i++)
        kal_sleep_task(10000);

    thermal_intr_flag = false;
        
    while (thermal_intr_flag != true)
    {
        temp = 0x100;                                            // set to normal for pnp0
        DRV_WriteReg32(TEMPADCEN, (0x00001000 + temp));        // set sensor voltage and sensor valid        

        temp = DRV_Reg32(TEMPMSR0);        
        while ((temp & 0x0FFF) != 0x100)
        {
            kal_sleep_task(10000);
            temp = DRV_Reg32(TEMPMSR0);
        }
        
        temp = 0x14;                                           // set to very hot for pnp0
        DRV_WriteReg32(TEMPADCEN, (0x00001000 + temp));        // set sensor voltage and sensor valid

        temp = DRV_Reg32(TEMPMSR0);        
        while ((temp & 0x0FFF) != 0x14)
        {
            kal_sleep_task(10000);
            temp = DRV_Reg32(TEMPMSR0);
        }
    }
    
    thermal_intr_flag = false;
     
    temp = DRV_Reg32(TEMPMSR0);
    while ((temp & 0x0FFF) != 0x14)
    {
        kal_sleep_task(10000);
        temp = DRV_Reg32(TEMPMSR0);
    }
    if ((temp & 0x0FFF) != 0x14) // first filter valid should be equal to 0x14
        LOG(6,"thermal_interrupt_occurrance_test: fail, read TEMPMSR0(0x14) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_interrupt_occurrance_test: pass, read TEMPMSR0(0x14) = 0x%x\n", (temp & 0x0FFF));
    
    temp = DRV_Reg32(TEMPMSR1);
    if ((temp & 0x0FFF) != 0x14) // first filter valid should be equal to 0x14
        LOG(6,"thermal_interrupt_occurrance_test: fail, read TEMPMSR1(0x14) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_interrupt_occurrance_test: pass, read TEMPMSR1(0x14) = 0x%x\n", (temp & 0x0FFF));

    temp = DRV_Reg32(TEMPMSR2);
    if ((temp & 0x0FFF) != 0x14) // first filter valid should be equal to 0x14
        LOG(6,"thermal_interrupt_occurrance_test: fail, read TEMPMSR2(0x14) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_interrupt_occurrance_test: pass, read TEMPMSR2(0x14) = 0x%x\n", (temp & 0x0FFF));

    LOG(6,"\n");
 
    temp = 0x234;                                           // set to very cold for pnp0
    DRV_WriteReg32(TEMPADCEN, (0x00001000 + temp));        // set sensor voltage and sensor valid
    
    while ((temp & 0x0FFF) != 0x234)
    {
        kal_sleep_task(10000);
        temp = DRV_Reg32(TEMPMSR0);
    }
    
    for (i = 0; i < 5; i++)
        kal_sleep_task(10000);

    thermal_intr_flag = false;
        
    while (thermal_intr_flag != true)
    {
        temp = 0x100;                                            // set to normal for pnp0
        DRV_WriteReg32(TEMPADCEN, (0x00001000 + temp));        // set sensor voltage and sensor valid
        
        temp = DRV_Reg32(TEMPMSR0);        
        while ((temp & 0x0FFF) != 0x100)
        {
            kal_sleep_task(10000);
            temp = DRV_Reg32(TEMPMSR0);
        }
        
        temp = 0x234;                                           // set to very cold for pnp0
        DRV_WriteReg32(TEMPADCEN, (0x00001000 + temp));        // set sensor voltage and sensor valid

        temp = DRV_Reg32(TEMPMSR0);        
        while ((temp & 0x0FFF) != 0x234)
        {
            kal_sleep_task(10000);
            temp = DRV_Reg32(TEMPMSR0);
        }
    }
    
    thermal_intr_flag = false;
     
    temp = DRV_Reg32(TEMPMSR0);
    while ((temp & 0x0FFF) != 0x234)
    {
        kal_sleep_task(10000);
        temp = DRV_Reg32(TEMPMSR0);
    }
    if ((temp & 0x0FFF) != 0x234) // first filter valid should be equal to 0x234
        LOG(6,"thermal_interrupt_occurrance_test: fail, read TEMPMSR0(0x234) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_interrupt_occurrance_test: pass, read TEMPMSR0(0x234) = 0x%x\n", (temp & 0x0FFF));
    
    temp = DRV_Reg32(TEMPMSR1);
    if ((temp & 0x0FFF) != 0x234) // first filter valid should be equal to 0x234
        LOG(6,"thermal_interrupt_occurrance_test: fail, read TEMPMSR1(0x234) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_interrupt_occurrance_test: pass, read TEMPMSR1(0x234) = 0x%x\n", (temp & 0x0FFF));

    temp = DRV_Reg32(TEMPMSR2);
    if ((temp & 0x0FFF) != 0x234) // first filter valid should be equal to 0x234
        LOG(6,"thermal_interrupt_occurrance_test: fail, read TEMPMSR2(0x234) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_interrupt_occurrance_test: pass, read TEMPMSR2(0x234) = 0x%x\n", (temp & 0x0FFF));

    LOG(6,"\n");
    return 0;
}


int THERMAL_Interrupt_Occurrance_Check(void)
{
    int result = 1;
    
    LOG(6,"\r\n");
    LOG(6,"THERMAL_Interrupt_Occurrance_Check: 1 times occurrance check\r\n");
    result = thermal_interrupt_occurrance_test(0x00000000); // 1 times occurrance
    if (result < 0)
        goto exit;
    LOG(6,"\r\n");
    
    LOG(6,"\r\n");
    LOG(6,"THERMAL_Interrupt_Occurrance_Check: 2 times occurrance check\r\n");
    result = thermal_interrupt_occurrance_test(0x00000155); // 2 times occurrance
    if (result < 0)
        goto exit;
    LOG(6,"\r\n");
    
    LOG(6,"\r\n");
    LOG(6,"THERMAL_Interrupt_Occurrance_Check: 3 times occurrance check\r\n");
    result = thermal_interrupt_occurrance_test(0x000002AA); // 3 times occurrance
    if (result < 0)
        goto exit;
    LOG(6,"\r\n");
    
    LOG(6,"\r\n");
    LOG(6,"THERMAL_Interrupt_Occurrance_Check: 4 times occurrance check\r\n");
    result = thermal_interrupt_occurrance_test(0x000003FF); // 4 times occurrance
    if (result < 0)
        goto exit;
    LOG(6,"\r\n");
    
exit:
    thermal_exit();
    return result; 
}


int thermal_sensing_filter_option_test(UINT16 option)
{
    UINT32 temp = 0, i = 0, count = 0;
	UINT8 avecnt=0;
    
    thermal_real_test = false;

	if((option&0x00000007)==0) avecnt = 1;
	else if((option&0x00000007)==1) avecnt = 2;	
	else if((option&0x00000007)==2) avecnt = 4;
	else if((option&0x00000007)==3) avecnt = 6;
	else if((option&0x00000007)==4) avecnt = 10;
	else if((option&0x00000007)==5) avecnt = 18;
		
	// vDrvThermalInitISR();

	DRV_WriteReg32(TEMPMONCTL1, 0x000000CC); // 3FF);    // counting unit is 1024 / 66M = 15.5us
	DRV_WriteReg32(TEMPMONCTL2, 0x02190219); // 219);	// sensing interval is 537 * 15.5us = 8.3235ms
	DRV_WriteReg32(TEMPAHBPOLL,  0x00000001);
	DRV_WriteReg32(TEMPAHBTO,    0x00000333);
	
    DRV_WriteReg32(TEMPMONIDET0, 0x00000000);               // times=1 for interrupt occurrance of sensing point 0
    DRV_WriteReg32(TEMPMONIDET1, 0x00000000);               // times=1 for interrupt occurrance of sensing point 1
    DRV_WriteReg32(TEMPMONIDET2, 0x00000000);               // times=1 for interrupt occurrance of sensing point 2
    DRV_WriteReg32(TEMPHTHRE, 0x000000AA);                  // set hot threshold
    DRV_WriteReg32(TEMPOFFSETH, 0x000000BB);                // set high offset threshold
    DRV_WriteReg32(TEMPH2NTHRE, 0x000000FF);                // set hot to normal threshold
    DRV_WriteReg32(TEMPOFFSETL, 0x00000111);                // set low offset threshold
    DRV_WriteReg32(TEMPCTHRE, 0x00000133);                  // set cold threshold
    DRV_WriteReg32(TEMPMSRCTL0, option);                    // !!! temperature measurement sampling control

	DRV_WriteReg32(TEMPADCPNP0, 0x172);
	DRV_WriteReg32(TEMPADCPNP1, 0x1B2);
	DRV_WriteReg32(TEMPADCPNP2, 0x1F2);
	DRV_WriteReg32(TEMPADCMUX, 0x11);		
	DRV_WriteReg32(TEMPADCEXT, 0x22);		 
	DRV_WriteReg32(TEMPADCEXT1, 0x33);				
    temp = 0x14;                                            // set to very hot for pnp0
    DRV_WriteReg32(TEMPADCEN, (0x00001000 + temp));        // set sensor voltage and sensor valid
    
	DRV_WriteReg32(TEMPPNPMUXADDR, SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCENADDR, SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCVALIDADDR,SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCVOLTADDR, SPARE_REPLAC);
	
	DRV_WriteReg32(TEMPRDCTRL, 0x0); // 0x1);			 
	DRV_WriteReg32(TEMPADCVALIDMASK, 0x0000002C);
	DRV_WriteReg32(TEMPADCVOLTAGESHIFT, 0x0);
	DRV_WriteReg32(TEMPADCWRITECTRL, 0x00000000);
	
	DRV_WriteReg32(TEMPMONINT,		 0x0000FFFF);
	DRV_WriteReg32(TEMPMONCTL0, 	 0x00000007);

	// 219h x 0CCh x 100h x 16.6ns = 465,535,180.82 ns =0.4 sec

    for (i = 0; i < 5; i++)
        kal_sleep_task(10000);
    
    count = 10;
    while (thermal_intr_flag != true && count--)
    {
        kal_sleep_task(10000);
    }
    
    temp = DRV_Reg32(TEMPMSR0);
    while ((temp & 0x0FFF) != 0x14)
    {
        kal_sleep_task(10000);
        temp = DRV_Reg32(TEMPMSR0);
    }
		
    for (i = 0; i < (400 * 2 * avecnt * 2); i++)
		HAL_Delay_us(1000);

    if ((temp & 0x0FFF) != 0x14) // first filter valid should be equal to 0x14
        LOG(6,"thermal_sensing_filter_option_test: fail, read TEMPMSR0(0x14) = 0x%x [%d]\n", (temp & 0x0FFF),(temp & 0x80000));
    else
        LOG(6,"thermal_sensing_filter_option_test: pass, read TEMPMSR0(0x14) = 0x%x [%d]\n", (temp & 0x0FFF),(temp & 0x80000));
    
    temp = DRV_Reg32(TEMPMSR1);
    if ((temp & 0x0FFF) != 0x14) // first filter valid should be equal to 0x14
        LOG(6,"thermal_sensing_filter_option_test: fail, read TEMPMSR1(0x14) = 0x%x [%d]\n", (temp & 0x0FFF),(temp & 0x80000));
    else
        LOG(6,"thermal_sensing_filter_option_test: pass, read TEMPMSR1(0x14) = 0x%x [%d]\n", (temp & 0x0FFF),(temp & 0x80000));

    temp = DRV_Reg32(TEMPMSR2);
    if ((temp & 0x0FFF) != 0x14) // first filter valid should be equal to 0x14
        LOG(6,"thermal_sensing_filter_option_test: fail, read TEMPMSR2(0x14) = 0x%x [%d]\n", (temp & 0x0FFF),(temp & 0x80000));
    else
        LOG(6,"thermal_sensing_filter_option_test: pass, read TEMPMSR2(0x14) = 0x%x [%d]\n", (temp & 0x0FFF),(temp & 0x80000));
       
    LOG(6,"\n");
        
    thermal_intr_flag = false;
    

	DRV_WriteReg32(TEMPMONCTL0, 	 0x00000000);
    temp = 0x234;                                           // set to very cold for pnp0
    DRV_WriteReg32(TEMPADCEN, (0x00001000 + temp));        // set sensor voltage and sensor valid
	DRV_WriteReg32(TEMPMONCTL0, 	 0x00000007);
	
    for (i = 0; i < 5; i++)
        kal_sleep_task(10000);
    
    count = 10;
    while (thermal_intr_flag != true && count--)
    {
        kal_sleep_task(10000);
    }  
    
    temp = DRV_Reg32(TEMPMSR0);

    while ((temp & 0x0FFF) != 0x234)
    {
        kal_sleep_task(10000);
        temp = DRV_Reg32(TEMPMSR0);
    }

    for (i = 0; i < (400 * 2 * avecnt * 2); i++)
		HAL_Delay_us(1000);

	
    if ((temp & 0x0FFF) != 0x234) // first filter valid should be equal to 0x234
        LOG(6,"thermal_sensing_filter_option_test: fail, read TEMPMSR0(0x234) = 0x%x [%d]\n", (temp & 0x0FFF),(temp & 0x80000));
    else
        LOG(6,"thermal_sensing_filter_option_test: pass, read TEMPMSR0(0x234) = 0x%x [%d]\n", (temp & 0x0FFF),(temp & 0x80000));
    
    temp = DRV_Reg32(TEMPMSR1);
    if ((temp & 0x0FFF) != 0x234) // first filter valid should be equal to 0x234
        LOG(6,"thermal_sensing_filter_option_test: fail, read TEMPMSR1(0x234) = 0x%x [%d]\n", (temp & 0x0FFF),(temp & 0x80000));
    else
        LOG(6,"thermal_sensing_filter_option_test: pass, read TEMPMSR1(0x234) = 0x%x [%d]\n", (temp & 0x0FFF),(temp & 0x80000));

    temp = DRV_Reg32(TEMPMSR2);
    if ((temp & 0x0FFF) != 0x234) // first filter valid should be equal to 0x234
        LOG(6,"thermal_sensing_filter_option_test: fail, read TEMPMSR2(0x234) = 0x%x [%d]\n", (temp & 0x0FFF),(temp & 0x80000));
    else
        LOG(6,"thermal_sensing_filter_option_test: pass, read TEMPMSR2(0x234) = 0x%x [%d]\n", (temp & 0x0FFF),(temp & 0x80000));
        
    LOG(6,"\n");
    
    thermal_intr_flag = false;

	DRV_WriteReg32(TEMPMONCTL0, 	 0x00000000);    
    temp = 0xB0;                                           // set to very cold for pnp0
    DRV_WriteReg32(TEMPADCEN, (0x00001000 + temp));        // set sensor voltage and sensor valid
	DRV_WriteReg32(TEMPMONCTL0, 	 0x00000007);    
    
    for (i = 0; i < 5; i++)
        kal_sleep_task(10000);
    
    count = 10;
    while (thermal_intr_flag != true && count--)
    {
        kal_sleep_task(10000);
    }  
    
    temp = DRV_Reg32(TEMPMSR0);
 
    while ((temp & 0x0FFF) != 0xB0)
    {
        kal_sleep_task(10000);
        temp = DRV_Reg32(TEMPMSR0);
    }

    for (i = 0; i < (400 * 2 * avecnt * 2); i++)
		HAL_Delay_us(1000);

	
    if ((temp & 0x0FFF) != 0xB0) // first filter valid should be equal to 0xB0
        LOG(6,"thermal_sensing_filter_option_test: fail, read TEMPMSR0(0xB0) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_sensing_filter_option_test: pass, read TEMPMSR0(0xB0) = 0x%x\n", (temp & 0x0FFF));
    
    temp = DRV_Reg32(TEMPMSR1);
    if ((temp & 0x0FFF) != 0xB0) // first filter valid should be equal to 0xB0
        LOG(6,"thermal_sensing_filter_option_test: fail, read TEMPMSR1(0xB0) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_sensing_filter_option_test: pass, read TEMPMSR1(0xB0) = 0x%x\n", (temp & 0x0FFF));

    temp = DRV_Reg32(TEMPMSR2);
    if ((temp & 0x0FFF) != 0xB0) // first filter valid should be equal to 0xB0
        LOG(6,"thermal_sensing_filter_option_test: fail, read TEMPMSR2(0xB0) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_sensing_filter_option_test: pass, read TEMPMSR2(0xB0) = 0x%x\n", (temp & 0x0FFF));
        
    LOG(6,"\n");
    
    thermal_intr_flag = false;
	
    return 0;
}

int THERMAL_Sensing_Filter_Option_Check(void)
{
    int result = 1;
   
    LOG(6,"THERMAL_Sensing_Filter_Option_Check: one sampling check\r\n");
    result = thermal_sensing_filter_option_test(0x00000000);
    if (result < 0)
        goto exit;
    
    LOG(6,"THERMAL_Sensing_Filter_Option_Check: average 2 sampling check\r\n");
    result = thermal_sensing_filter_option_test(0x00000049);
    if (result < 0)
        goto exit;
    
    LOG(6,"THERMAL_Sensing_Filter_Option_Check: 4 sampling check\r\n");
    result = thermal_sensing_filter_option_test(0x00000092);
    if (result < 0)
        goto exit;

    LOG(6,"THERMAL_Sensing_Filter_Option_Check: 6 sampling check\r\n");
    result = thermal_sensing_filter_option_test(0x000000DB);
    if (result < 0)
        goto exit;
    
    LOG(6,"THERMAL_Sensing_Filter_Option_Check: 10 sampling check\r\n");
    result = thermal_sensing_filter_option_test(0x00000124);
    if (result < 0)
        goto exit;
  
    LOG(6,"THERMAL_Sensing_Filter_Option_Check: 18 sampling check\r\n");
    result = thermal_sensing_filter_option_test(0x0000016D);
    if (result < 0)
        goto exit;
    
exit:
    thermal_exit();
    return result; 
}

#define TEST_THERM_CTR_NO1	0
#define PTP_TEST_TEMP 5
UINT16 u2TestTemp[PTP_TEST_TEMP]={0x15, 0x14, 0xcc, 0x120, 0x234};
/*
0xFF   --> 0x15   Hot offset
0x15   --> 0x14   Hot
0x14   --> 0xcc   Nonthing
0xcc   --> 0x120 Low Offset & Hot to Normal
0x120 --> 0x234 Low
*/
int thermal_interrupt_trigger_test(BOOL fgHW)
{
    UINT32 temp = 0, count = 0;
	UINT8 u1testIdx = 0;
	UINT16 u2testTemp = 0;
	
    thermal_real_test = false;

	if(fgHW)
	vDrvThermalInitISR();

	// Thermal Controller 1
	DRV_WriteReg32(TEMPMONCTL1, 0x000000CC); // 3FF);    // counting unit is 1024 / 66M = 15.5us
	DRV_WriteReg32(TEMPMONCTL2, 0x02190219); // 219);	// sensing interval is 537 * 15.5us = 8.3235ms
	DRV_WriteReg32(TEMPAHBPOLL,  0x00000001);
	DRV_WriteReg32(TEMPAHBTO,    0x00000333);
	DRV_WriteReg32(TEMPMONIDET0, 0x00000000);
	DRV_WriteReg32(TEMPMONIDET1, 0x00000000);
	DRV_WriteReg32(TEMPMONIDET2, 0x00000000);
	DRV_WriteReg32(TEMPH2NTHRE,  0x000000FF);	// set hot to normal threshold
	DRV_WriteReg32(TEMPHTHRE,    0x000000AA);	// set hot threshold
	DRV_WriteReg32(TEMPCTHRE,    0x00000133);	// set cold threshold	
	DRV_WriteReg32(TEMPOFFSETH,  0x000000BB);   // set high offset threshold	
	DRV_WriteReg32(TEMPOFFSETL,  0x00000111);	// set low offset threshold		
	DRV_WriteReg32(TEMPMSRCTL0,  0x00000000);	// DRV_WriteReg32(TEMPMSRCTL0,  0x000006DB);	

	DRV_WriteReg32(TEMPADCPNP0, 0x172);
	DRV_WriteReg32(TEMPADCPNP1, 0x1B2);
	DRV_WriteReg32(TEMPADCPNP2, 0x1F2);
	DRV_WriteReg32(TEMPADCMUX, 0x11);		
	DRV_WriteReg32(TEMPADCEXT, 0x22);		 
	DRV_WriteReg32(TEMPADCEXT1, 0x33);				
	DRV_WriteReg32(TEMPADCEN, 0x10FF);
	
	DRV_WriteReg32(TEMPPNPMUXADDR, SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCENADDR, SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCVALIDADDR,SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCVOLTADDR, SPARE_REPLAC);
	
	DRV_WriteReg32(TEMPRDCTRL, 0x1);			 
	DRV_WriteReg32(TEMPADCVALIDMASK, 0x0000002C);
	DRV_WriteReg32(TEMPADCVOLTAGESHIFT, 0x0);
	DRV_WriteReg32(TEMPADCWRITECTRL, 0x00000000);
	
	DRV_WriteReg32(TEMPMONINT,		 0x0000FFFF);
	DRV_WriteReg32(TEMPMONCTL0, 	 0x00000007);

    temp = DRV_Reg32(TEMPMSR0);    
    LOG(6,"thermal_interrupt_trigger_test: (before testing), read TEMPMSR0 = 0x%x\n", (temp & 0x0FFF));
    temp = DRV_Reg32(TEMPMSR1);    
    LOG(6,"thermal_interrupt_trigger_test: (before testing), read TEMPMSR1 = 0x%x\n", (temp & 0x0FFF));
    temp = DRV_Reg32(TEMPMSR2);    
    LOG(6,"thermal_interrupt_trigger_test: (before testing), read TEMPMSR2 = 0x%x\n", (temp & 0x0FFF));


	for(u1testIdx=0; u1testIdx < PTP_TEST_TEMP; u1testIdx++)
	{
		u2testTemp = u2TestTemp[u1testIdx];
		DRV_WriteReg32(TEMPADCEN, (0x00001000 + u2testTemp));
		
	    count = 20;
	    while (thermal_intr_flag != true && count--)
	    {
	        kal_sleep_task(10000);
	    }	    
	    count = 0;
		
	    temp = DRV_Reg32(TEMPMSR0);
		
		while ((temp & 0x0FFF) != u2testTemp)
		{
			// LOG(6,"thermal_interrupt_trigger_test: count(0x%3x) = %d temp = %x \n", u2testTemp,count++, temp);
			kal_sleep_task(10000);
			if(!(fgHW)) thermal_lisr();
			temp = DRV_Reg32(TEMPMSR0);
		}
		
	    if ((temp & 0x0FFF) != u2testTemp)
	    {
	        LOG(6,"thermal_interrupt_trigger_test: fail(0x%3x), read TEMPMSR0 = 0x%x\n", u2testTemp,(temp & 0x0FFF));
	        return -1;
	    }    
	    else
	        LOG(6,"thermal_interrupt_trigger_test: pass(0x%3x), read TEMPMSR0 = 0x%x\n", u2testTemp,(temp & 0x0FFF));
	    
		    
	    temp = DRV_Reg32(TEMPMSR1);
	    if ((temp & 0x0FFF) != u2testTemp)
	    {
	        LOG(6,"thermal_interrupt_trigger_test: fail(0x%3x), read TEMPMSR1 = 0x%x\n", u2testTemp,(temp & 0x0FFF));
	        return -1;
	    }
	    else
	        LOG(6,"thermal_interrupt_trigger_test: pass(0x%3x), read TEMPMSR1 = 0x%x\n", u2testTemp, (temp & 0x0FFF));

	    temp = DRV_Reg32(TEMPMSR2);
	    if ((temp & 0x0FFF) != u2testTemp)
	    {
	        LOG(6,"thermal_interrupt_trigger_test: fail(0x%3x), read TEMPMSR2 = 0x%x\n", u2testTemp, (temp & 0x0FFF));
	        return -1;
	    }
	    else
	        LOG(6,"thermal_interrupt_trigger_test: pass(0x%3x), read TEMPMSR2 = 0x%x\n", u2testTemp, (temp & 0x0FFF));
	        
	    LOG(6,"\n");		

    	thermal_intr_flag = false;

	}

    return 0;
}

int THERMAL_Interrupt_Trigger_Check(BOOL fgHW)
{
    int result = 1;
    
    result = thermal_interrupt_trigger_test(fgHW);
    
    return result; 
}

UINT16 u2TestTemp1[PTP_TEST_TEMP]={0x14, 0x15, 0x13, 0xFF, 0xFF};


static void thermal_ahb_timeout_test_Reset(void)
{
	vIO32WriteFldAlign(PTP_CKGEN_REG_TOP_BACKUP4, 0, PTP_CKGEN_RESET);
	vIO32WriteFldAlign(PTP_CKGEN_REG_TOP_BACKUP4, 1, PTP_CKGEN_RESET);
}

static void thermal_ahb_timeout_test_init(void)
{
    thermal_real_test = false;

	
	DRV_WriteReg32(TEMPMONCTL1, 0x000000CC); // 3FF);    // counting unit is 1024 / 66M = 15.5us
	DRV_WriteReg32(TEMPMONCTL2, 0x02190219); // 219);	// sensing interval is 537 * 15.5us = 8.3235ms
	DRV_WriteReg32(TEMPAHBPOLL,  0x00000001);
	DRV_WriteReg32(TEMPAHBTO,    0x00000333);
    
    DRV_WriteReg32(TEMPMONIDET0, 0x00000000);               // times=1 for interrupt occurrance of sensing point 0
    DRV_WriteReg32(TEMPMONIDET1, 0x00000000);               // times=1 for interrupt occurrance of sensing point 1
    DRV_WriteReg32(TEMPMONIDET2, 0x00000000);               // times=1 for interrupt occurrance of sensing point 2
    DRV_WriteReg32(TEMPHTHRE, 0x000000AA);                  // set hot threshold
    DRV_WriteReg32(TEMPOFFSETH, 0x000000BB);                // set high offset threshold
    DRV_WriteReg32(TEMPH2NTHRE, 0x000000FF);                // set hot to normal threshold
    DRV_WriteReg32(TEMPOFFSETL, 0x00000111);                // set low offset threshold
    DRV_WriteReg32(TEMPCTHRE, 0x00000133);                  // set cold threshold
    DRV_WriteReg32(TEMPMSRCTL0, 0x0000000);                 // temperature measurement sampling control (one sampling)

	DRV_WriteReg32(TEMPADCPNP0, 0x172);
	DRV_WriteReg32(TEMPADCPNP1, 0x1B2);
	DRV_WriteReg32(TEMPADCPNP2, 0x1F2);
	DRV_WriteReg32(TEMPADCMUX, 0x11);		
	DRV_WriteReg32(TEMPADCEXT, 0x22);		 
	DRV_WriteReg32(TEMPADCEXT1, 0x33);				
	DRV_WriteReg32(TEMPADCEN, 0x10FF);
	
	DRV_WriteReg32(TEMPPNPMUXADDR, SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCENADDR, SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCVALIDADDR,SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCVOLTADDR, SPARE_REPLAC);
	
	DRV_WriteReg32(TEMPRDCTRL, 0x1);
	DRV_WriteReg32(TEMPADCVALIDMASK, 0x0000002C);
	DRV_WriteReg32(TEMPADCVOLTAGESHIFT, 0x0);
	DRV_WriteReg32(TEMPADCWRITECTRL, 0x00000000);
	
	DRV_WriteReg32(TEMPMONINT,		 0x0000FFFF);

}

int thermal_ahb_timeout_test(UINT32 u4poling, UINT32 u4timeout)
{
    UINT32 temp = 0, i = 0, count = 0;
	static UINT8 tempIdx=0;
	UINT16 tempIdx_temp=0;
	UINT32 poling, timeout;


	vDrvThermalInitISR();	
	thermal_ahb_timeout_test_init();

	for(tempIdx=0; tempIdx<4; tempIdx++)
	{	
		if (tempIdx == 2) // AHB Timeout, status machine hang up, need reset again
		{
			thermal_ahb_timeout_test_Reset();
			thermal_ahb_timeout_test_init();
		}
		
		DRV_WriteReg32(TEMPMONCTL0, 	 0x00000000);
	
	    tempIdx_temp = u2TestTemp1[tempIdx];
	    DRV_WriteReg32(TEMPADCEN, (0x00001000 + tempIdx_temp));        // set sensor voltage and sensor valid	    

		if ((tempIdx == 0) || (tempIdx == 2))
		{
			poling = 0x00000001;
			timeout = 0x00000333;
		}
		else
		{
			poling = u4poling;
			timeout = u4timeout;
		}

		LOG(6,"poling=%d, timeout=%d\n", poling, timeout);
		DRV_WriteReg32(TEMPAHBPOLL, poling);					  // !!! polling interval to check if temperature sense is ready
		DRV_WriteReg32(TEMPAHBTO, timeout);					  // !!! exceed this polling time, IRQ would be inserted			

		DRV_WriteReg32(TEMPMONCTL0, 	 0x00000007);

	    for (i = 0; i < 1000; i++)
	        kal_sleep_task(10000);    

		count = 0;
		do
		{
		    count++;
			temp = DRV_Reg32(TEMPMSR0);
			LOG(6,"[Stage%d]Count=%d, thermal_intr_flag = %d temp = %x stemp = %x , status= %x \n",tempIdx, count, thermal_intr_flag, temp, tempIdx_temp,DRV_Reg32(TEMPMSRCTL1)&0xFFFFFFFE);
			kal_sleep_task(10000);
			if(temp == tempIdx_temp)
			{
				LOG(6,"temp(%d) == tempIdx_temp(%d)\n",temp, tempIdx_temp);
				break;	
			}	
		}while((!thermal_intr_flag));
	    
	    temp = DRV_Reg32(TEMPMSR0);
		
	    if ((temp & 0x0FFF) != tempIdx_temp) // first filter valid should be equal to 0x14
	    {
	    	if(poling < timeout)
	        LOG(6,"[poling < timeout][%d]thermal_ahb_timeout_test: fail, read TEMPMSR0(0x%3x) = 0x%x\n", tempIdx,tempIdx_temp,(temp & 0x0FFF));
			else		
	        LOG(6,"[poling > timeout][%d]thermal_ahb_timeout_test: pass, read TEMPMSR0(0x%3x) = 0x%x\n", tempIdx,tempIdx_temp,(temp & 0x0FFF));		
	    }
	    else
	    {
	    	if(poling < timeout)    
	        LOG(6,"[poling < timeout][%d]thermal_ahb_timeout_test: pass, read TEMPMSR0(0x%3x) = 0x%x\n", tempIdx,tempIdx_temp,(temp & 0x0FFF));
			else
	        LOG(6,"[poling > timeout][%d]thermal_ahb_timeout_test: fail, read TEMPMSR0(0x%3x) = 0x%x\n", tempIdx,tempIdx_temp,(temp & 0x0FFF));			
	    }	


    	thermal_intr_flag = false;    
	}    
    LOG(6,"\n");        
    
    return 0;
}


int THERMAL_AHB_Timeout_Check(void)
{
    int result = 1;
    	
    LOG(6,"THERMAL_AHB_Timeout_Check: AHB polling time > AHB polling timeout\r\n"); //timeout
    result = thermal_ahb_timeout_test(0x0000FFFF, 0x0000000C); // AHB polling time > AHB polling timeout
    if (result < 0)
        goto exit;
    
exit:
    thermal_exit();
    return result; 
}

int thermal_watchdog_test(void)
{
	vDrvThermal_Fast_Init();
	vDrvThermal_Cal_Prepare();
	vDrvThermal_Calibration();
	vDrvThermal_Reset_and_Initial();
	Set_Thermal_Ctrl_Trigger(trip_temp[6]);

	vDrvThermalInitISR();
		
    return 0;	
}

int Thermal_WatchDog_Check(void)
{
    int result = 1;
    	
    LOG(6,"Thermal_WatchDog_Check\r\n");
    result = thermal_watchdog_test();

    return result; 
}

static void _TSWaitForISR(void)
{
    UINT32 count = 0;

    count = 2000;
    while (thermal_intr_flag != true && count--)
    {
        HAL_Delay_us(1000);
    }  
    // Printf("[A-1] count =%d\n",count);	
	thermal_intr_flag = false;
    count = 2000;	
    while (thermal_intr_flag != true && count--)
    {
        HAL_Delay_us(1000);
    }
    // Printf("[A-2] count =%d\n",count);		
	thermal_intr_flag = false;
    count = 2000;	
    while (thermal_intr_flag != true && count--)
    {
        HAL_Delay_us(1000);
    }	
    // Printf("[A-3] count =%d\n",count);

}

int thermal_different_threshold_test(UINT32 hot_threshold, UINT32 high_offset_threshold, UINT32 h2n_threshold, UINT32 low_offset_threshold, UINT32 cold_threshold)
{
    UINT32 temp = 0, i = 0;
    
    thermal_real_test = false;

	vDrvThermalInitISR();
	   
	DRV_WriteReg32(TEMPMONCTL1, 0x000000CC); // 3FF);    // counting unit is 1024 / 66M = 15.5us
	DRV_WriteReg32(TEMPMONCTL2, 0x02190219); // 219);	// sensing interval is 537 * 15.5us = 8.3235ms
	DRV_WriteReg32(TEMPAHBPOLL,  0x00000001);
	DRV_WriteReg32(TEMPAHBTO,    0x00000333);

    DRV_WriteReg32(TEMPMONIDET0, 0x00000000);               // times=1 for interrupt occurrance of sensing point 0
    DRV_WriteReg32(TEMPMONIDET1, 0x00000000);               // times=1 for interrupt occurrance of sensing point 1
    DRV_WriteReg32(TEMPMONIDET2, 0x00000000);               // times=1 for interrupt occurrance of sensing point 2
    DRV_WriteReg32(TEMPHTHRE, hot_threshold);               // !!! set hot threshold
    DRV_WriteReg32(TEMPOFFSETH, high_offset_threshold);     // !!! set high offset threshold
    DRV_WriteReg32(TEMPH2NTHRE, h2n_threshold);             // !!! set hot to normal threshold
    DRV_WriteReg32(TEMPOFFSETL, low_offset_threshold);      // !!! set low offset threshold
    DRV_WriteReg32(TEMPCTHRE, cold_threshold);              // !!! set cold threshold
    DRV_WriteReg32(TEMPMSRCTL0, 0x0000000);                 // temperature measurement sampling control (one sampling)

	DRV_WriteReg32(TEMPADCPNP0, 0x172);
	DRV_WriteReg32(TEMPADCPNP1, 0x1B2);
	DRV_WriteReg32(TEMPADCPNP2, 0x1F2);
	DRV_WriteReg32(TEMPADCMUX, 0x11);		
	DRV_WriteReg32(TEMPADCEXT, 0x22);		 
	DRV_WriteReg32(TEMPADCEXT1, 0x33);				
    temp = 0x14;                                            // set to very hot for pnp0
    DRV_WriteReg32(TEMPADCEN, (0x00001000 + temp));        // set sensor voltage and sensor valid
	
	DRV_WriteReg32(TEMPPNPMUXADDR, SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCENADDR, SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCVALIDADDR,SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCVOLTADDR, SPARE_REPLAC);
	
	DRV_WriteReg32(TEMPRDCTRL, 0x1);			 
	DRV_WriteReg32(TEMPADCVALIDMASK, 0x0000002C);
	DRV_WriteReg32(TEMPADCVOLTAGESHIFT, 0x0);
	DRV_WriteReg32(TEMPADCWRITECTRL, 0x00000000);
	
	DRV_WriteReg32(TEMPMONINT,		 0x0000FFFF);
	DRV_WriteReg32(TEMPMONCTL0, 	 0x00000007);

    for (i = 0; i < 5; i++)
        kal_sleep_task(10000);
    
	_TSWaitForISR();
	
    temp = DRV_Reg32(TEMPMSR0);
    while ((temp & 0x0FFF) != 0x14)
    {
        kal_sleep_task(10000);
        temp = DRV_Reg32(TEMPMSR0);
    }
    for (i = 0; i < (400 * 2 * 1 * 2); i++)
		HAL_Delay_us(1000);
	
    if ((temp & 0x0FFF) != 0x14) // first filter valid should be equal to 0x14
        LOG(6,"thermal_different_threshold_test: fail, read TEMPMSR0(0x14) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_different_threshold_test: pass, read TEMPMSR0(0x14) = 0x%x\n", (temp & 0x0FFF));
    
    temp = DRV_Reg32(TEMPMSR1);
    if ((temp & 0x0FFF) != 0x14) // first filter valid should be equal to 0x14
        LOG(6,"thermal_different_threshold_test: fail, read TEMPMSR1(0x14) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_different_threshold_test: pass, read TEMPMSR1(0x14) = 0x%x\n", (temp & 0x0FFF));

    temp = DRV_Reg32(TEMPMSR2);
    if ((temp & 0x0FFF) != 0x14) // first filter valid should be equal to 0x14
        LOG(6,"thermal_different_threshold_test: fail, read TEMPMSR2(0x14) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_different_threshold_test: pass, read TEMPMSR2(0x14) = 0x%x\n", (temp & 0x0FFF));
        
    LOG(6,"\n");
        
    thermal_intr_flag = false;
    
    temp = 0x234;                                           // set to very cold for pnp0
    DRV_WriteReg32(TEMPADCEN, (0x00001000 + temp));        // set sensor voltage and sensor valid
    
    for (i = 0; i < 5; i++)
        kal_sleep_task(10000);
    
	_TSWaitForISR();

    temp = DRV_Reg32(TEMPMSR0);
    while ((temp & 0x0FFF) != 0x234)
    {
        kal_sleep_task(10000);
        temp = DRV_Reg32(TEMPMSR0);
    }
    for (i = 0; i < (400 * 2 * 1 * 2); i++)
		HAL_Delay_us(1000);
	
    if ((temp & 0x0FFF) != 0x234)
        LOG(6,"thermal_different_threshold_test: fail, read TEMPMSR0(0x234) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_different_threshold_test: pass, read TEMPMSR0(0x234) = 0x%x\n", (temp & 0x0FFF));
    
    temp = DRV_Reg32(TEMPMSR1);
    if ((temp & 0x0FFF) != 0x234)
        LOG(6,"thermal_different_threshold_test: fail, read TEMPMSR1(0x234) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_different_threshold_test: pass, read TEMPMSR1(0x234) = 0x%x\n", (temp & 0x0FFF));

    temp = DRV_Reg32(TEMPMSR2);
    if ((temp & 0x0FFF) != 0x234)
        LOG(6,"thermal_different_threshold_test: fail, read TEMPMSR2(0x234) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_different_threshold_test: pass, read TEMPMSR2(0x234) = 0x%x\n", (temp & 0x0FFF));
        
    LOG(6,"\n");
    
    thermal_intr_flag = false;
    
    temp = 0xB0;                                           // set to very cold for pnp0
    DRV_WriteReg32(TEMPADCEN, (0x00001000 + temp));        // set sensor voltage and sensor valid
    
    for (i = 0; i < 5; i++)
        kal_sleep_task(10000);
    
	_TSWaitForISR();
	
    temp = DRV_Reg32(TEMPMSR0);
    while ((temp & 0x0FFF) != 0xB0)
    {
        kal_sleep_task(10000);
        temp = DRV_Reg32(TEMPMSR0);
    }
    for (i = 0; i < (400 * 2 * 1 * 2); i++)
		HAL_Delay_us(1000);
	
    if ((temp & 0x0FFF) != 0xB0) // first filter valid should be equal to 0xB0
        LOG(6,"thermal_different_threshold_test: fail, read TEMPMSR0(0xB0) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_different_threshold_test: pass, read TEMPMSR0(0xB0) = 0x%x\n", (temp & 0x0FFF));
    
    temp = DRV_Reg32(TEMPMSR1);
    if ((temp & 0x0FFF) != 0xB0) // first filter valid should be equal to 0xB0
        LOG(6,"thermal_different_threshold_test: fail, read TEMPMSR1(0xB0) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_different_threshold_test: pass, read TEMPMSR1(0xB0) = 0x%x\n", (temp & 0x0FFF));

    temp = DRV_Reg32(TEMPMSR2);
    if ((temp & 0x0FFF) != 0xB0) // first filter valid should be equal to 0xB0
        LOG(6,"thermal_different_threshold_test: fail, read TEMPMSR2(0xB0) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_different_threshold_test: pass, read TEMPMSR2(0xB0) = 0x%x\n", (temp & 0x0FFF));
        
    LOG(6,"\n");

    thermal_intr_flag = false;
    
    return 0;
}


int THERMAL_Different_Threshold_Check(void)
{
    int result = 1;
    
    LOG(6,"THERMAL_Different_Threshold_Check: HOT         HIGHOFFSET  HOT2NORMAL  LOWOFFSET   COLD\r\n");
    LOG(6,"THERMAL_Different_Threshold_Check: 0x000000AA, 0x000000BB, 0x000000FF, 0x00000111, 0x00000133\r\n");
    result = thermal_different_threshold_test(0x000000AA, 0x000000BB, 0x000000FF, 0x00000111, 0x00000133);
    if (result < 0)
        goto exit;
       
    LOG(6,"THERMAL_Different_Threshold_Check: HOT         HIGHOFFSET  HOT2NORMAL  LOWOFFSET   COLD\r\n");
    LOG(6,"THERMAL_Different_Threshold_Check: 0x00000055, 0x00000066, 0x00000088, 0x00000333, 0x00000444\r\n");
    result = thermal_different_threshold_test(0x00000055, 0x00000066, 0x00000088, 0x00000333, 0x00000444);
    if (result < 0)
        goto exit;
        
    LOG(6,"THERMAL_Different_Threshold_Check: HOT         HIGHOFFSET  HOT2NORMAL  LOWOFFSET   COLD\r\n");
    LOG(6,"THERMAL_Different_Threshold_Check: 0x000000111, 0x000000222, 0x00000333, 0x00000666, 0x00000777\r\n");
    result = thermal_different_threshold_test(0x00000111, 0x00000222, 0x00000333, 0x00000666, 0x00000777);

    LOG(6,"THERMAL_Different_Threshold_Check: HOT         HIGHOFFSET  HOT2NORMAL  LOWOFFSET   COLD\r\n");
    LOG(6,"THERMAL_Different_Threshold_Check: 0x000000AA, 0x000000BB, 0x000000FF, 0x00000111, 0x00000133\r\n");
    result = thermal_different_threshold_test(0x000000AA, 0x000000BB, 0x000000FF, 0x00000111, 0x00000133);
    if (result < 0)
        goto exit;
	
exit:
    thermal_exit();
    return result; 
}

int thermal_filter_sample_interval_test(kal_uint32 filt_interval, kal_uint32 sen_interval)
{
    UINT32 temp = 0, i = 0, count = 0;
	UINT32 time_delay;
    
    thermal_real_test = false;
    
    DRV_WriteReg32(TEMPMONCTL1, 0x0000014A);                // counting unit is 330 / 66M = 5us
    
    temp = filt_interval << 16 | sen_interval;
    DRV_WriteReg32(TEMPMONCTL2, temp);                      // !!! sensing interval is sen_interval * 5us

	time_delay = (17 * 0x14A * sen_interval * 2 * 2)/1000;

	DRV_WriteReg32(TEMPAHBPOLL,  0x00000001);
	DRV_WriteReg32(TEMPAHBTO,    0x00000333);

    DRV_WriteReg32(TEMPMONIDET0, 0x00000000);               // times=1 for interrupt occurrance of sensing point 0
    DRV_WriteReg32(TEMPMONIDET1, 0x00000000);               // times=1 for interrupt occurrance of sensing point 1
    DRV_WriteReg32(TEMPMONIDET2, 0x00000000);               // times=1 for interrupt occurrance of sensing point 2
    DRV_WriteReg32(TEMPHTHRE, 0x000000AA);                  // set hot threshold
    DRV_WriteReg32(TEMPOFFSETH, 0x000000BB);                // set high offset threshold
    DRV_WriteReg32(TEMPH2NTHRE, 0x000000FF);                // set hot to normal threshold
    DRV_WriteReg32(TEMPOFFSETL, 0x00000111);                // set low offset threshold
    DRV_WriteReg32(TEMPCTHRE, 0x00000133);                  // set cold threshold
    DRV_WriteReg32(TEMPMSRCTL0, 0x0000000);                 // temperature measurement sampling control

	DRV_WriteReg32(TEMPADCPNP0, 0x172);
	DRV_WriteReg32(TEMPADCPNP1, 0x1B2);
	DRV_WriteReg32(TEMPADCPNP2, 0x1F2);
	DRV_WriteReg32(TEMPADCMUX, 0x11);		
	DRV_WriteReg32(TEMPADCEXT, 0x22);		 
	DRV_WriteReg32(TEMPADCEXT1, 0x33);				
	temp = 0x14;											// set to very hot for pnp0
	DRV_WriteReg32(TEMPADCEN, (0x00001000 + temp)); 	   // set sensor voltage and sensor valid
	
	DRV_WriteReg32(TEMPPNPMUXADDR, SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCENADDR, SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCVALIDADDR,SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCVOLTADDR, SPARE_REPLAC);
	
	DRV_WriteReg32(TEMPRDCTRL, 0x1);			 
	DRV_WriteReg32(TEMPADCVALIDMASK, 0x0000002C);
	DRV_WriteReg32(TEMPADCVOLTAGESHIFT, 0x0);
	DRV_WriteReg32(TEMPADCWRITECTRL, 0x00000000);
	
	DRV_WriteReg32(TEMPMONINT,		 0x0000FFFF);
	DRV_WriteReg32(TEMPMONCTL0, 	 0x00000007);
		
    for (i = 0; i < 5; i++)
        non_volatile_kal_sleep_task(10000);
    
    count = 10;
    while (thermal_intr_flag != true && count--)
    {
        non_volatile_kal_sleep_task(10000);
    }

    count = 0;    
    temp = DRV_Reg32(TEMPMSR0);
    while ((temp & 0x0FFF) != 0x14)
    {
        non_volatile_kal_sleep_task(10000);
        temp = DRV_Reg32(TEMPMSR0);
    }

    for (i = 0; i < time_delay; i++)
		HAL_Delay_us(1000);
	
    if ((temp & 0x0FFF) != 0x14) // first filter valid should be equal to 0x14
        LOG(6,"thermal_filter_sample_interval_test: fail, read TEMPMSR0(0x14) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_filter_sample_interval_test: pass, read TEMPMSR0(0x14) = 0x%x\n", (temp & 0x0FFF));
    
    count = 0;    
    temp = DRV_Reg32(TEMPMSR1);
    while ((temp & 0x0FFF) != 0x14)
    {
        non_volatile_kal_sleep_task(10000);
        temp = DRV_Reg32(TEMPMSR1);
    }
    if ((temp & 0x0FFF) != 0x14) // first filter valid should be equal to 0x14
        LOG(6,"thermal_filter_sample_interval_test: fail, read TEMPMSR1(0x14) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_filter_sample_interval_test: pass, read TEMPMSR1(0x14) = 0x%x\n", (temp & 0x0FFF));

    count = 0;    
    temp = DRV_Reg32(TEMPMSR2);
    while ((temp & 0x0FFF) != 0x14)
    {
        non_volatile_kal_sleep_task(10000);
        temp = DRV_Reg32(TEMPMSR2);
    }
    if ((temp & 0x0FFF) != 0x14) // first filter valid should be equal to 0x14
        LOG(6,"thermal_filter_sample_interval_test: fail, read TEMPMSR2(0x14) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_filter_sample_interval_test: pass, read TEMPMSR2(0x14) = 0x%x\n", (temp & 0x0FFF));
        
    LOG(6,"\n");
        
    thermal_intr_flag = false;
    
    DRV_WriteReg32(TEMPMONCTL0, 	 0x00000000);
    temp = 0x234;                                           // set to very cold for pnp0
    DRV_WriteReg32(TEMPADCEN, (0x00001000 + temp));        // set sensor voltage and sensor valid
    DRV_WriteReg32(TEMPMONCTL0, 	 0x00000007); 
    
    for (i = 0; i < 5; i++)
        non_volatile_kal_sleep_task(10000);
    
    count = 10;
    while (thermal_intr_flag != true && count--)
    {
        non_volatile_kal_sleep_task(10000);
    }  

    count = 0;    
    temp = DRV_Reg32(TEMPMSR0);
    while ((temp & 0x0FFF) != 0x234)
    {
        non_volatile_kal_sleep_task(10000);
        temp = DRV_Reg32(TEMPMSR0);
    }
    for (i = 0; i < time_delay; i++)
		HAL_Delay_us(1000);	
    if ((temp & 0x0FFF) != 0x234)
        LOG(6,"thermal_filter_sample_interval_test: fail, read TEMPMSR0(0x234) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_filter_sample_interval_test: pass, read TEMPMSR0(0x234) = 0x%x\n", (temp & 0x0FFF));
    
    temp = DRV_Reg32(TEMPMSR1);
    if ((temp & 0x0FFF) != 0x234)
        LOG(6,"thermal_filter_sample_interval_test: fail, read TEMPMSR1(0x234) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_filter_sample_interval_test: pass, read TEMPMSR1(0x234) = 0x%x\n", (temp & 0x0FFF));

    temp = DRV_Reg32(TEMPMSR2);
    if ((temp & 0x0FFF) != 0x234)
        LOG(6,"thermal_filter_sample_interval_test: fail, read TEMPMSR2(0x234) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_filter_sample_interval_test: pass, read TEMPMSR2(0x234) = 0x%x\n", (temp & 0x0FFF));
        
    LOG(6,"\n");
    
    thermal_intr_flag = false;
    
    DRV_WriteReg32(TEMPMONCTL0, 	 0x00000000); 
    temp = 0xB0;                                           // set to very cold for pnp0
    DRV_WriteReg32(TEMPADCEN, (0x00001000 + temp));        // set sensor voltage and sensor valid
    DRV_WriteReg32(TEMPMONCTL0, 	 0x00000007); 
    
    for (i = 0; i < 5; i++)
        non_volatile_kal_sleep_task(10000);
    
    count = 10;
    while (thermal_intr_flag != true && count--)
    {
        non_volatile_kal_sleep_task(10000);
    }  

    count = 0;    
    temp = DRV_Reg32(TEMPMSR0);
    while ((temp & 0x0FFF) != 0xB0)
    {
        non_volatile_kal_sleep_task(10000);
        temp = DRV_Reg32(TEMPMSR0);
    }
	
    for (i = 0; i < time_delay; i++)
		HAL_Delay_us(1000);
	
    if ((temp & 0x0FFF) != 0xB0) // first filter valid should be equal to 0xB0
        LOG(6,"thermal_filter_sample_interval_test: fail, read TEMPMSR0(0xB0) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_filter_sample_interval_test: pass, read TEMPMSR0(0xB0) = 0x%x\n", (temp & 0x0FFF));
    
    temp = DRV_Reg32(TEMPMSR1);
    if ((temp & 0x0FFF) != 0xB0) // first filter valid should be equal to 0xB0
        LOG(6,"thermal_filter_sample_interval_test: fail, read TEMPMSR1(0xB0) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_filter_sample_interval_test: pass, read TEMPMSR1(0xB0) = 0x%x\n", (temp & 0x0FFF));

    temp = DRV_Reg32(TEMPMSR2);
    if ((temp & 0x0FFF) != 0xB0) // first filter valid should be equal to 0xB0
        LOG(6,"thermal_filter_sample_interval_test: fail, read TEMPMSR2(0xB0) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_filter_sample_interval_test: pass, read TEMPMSR2(0xB0) = 0x%x\n", (temp & 0x0FFF));
        
    LOG(6,"\n");
    

	DRV_WriteReg32(TEMPMONCTL0, 	 0x00000000); 

    thermal_intr_flag = false;
    
    return 0;
}

int THERMAL_Filter_Sample_Interval_Check(void)
{
    int result = 1;
    
    LOG(6,"THERMAL_Filter_Sample_Interval_Check: set to extra large interval\r\n");
    result = thermal_filter_sample_interval_test(0x000003FF, 0x000003FF);
    if (result < 0)
        goto exit;   
    
    LOG(6,"THERMAL_Filter_Sample_Interval_Check: set to normal interval\r\n");
    result = thermal_filter_sample_interval_test(0x000000FF, 0x000000FF);
    if (result < 0)
        goto exit;
    
    LOG(6,"THERMAL_Filter_Sample_Interval_Check: set to extra small interval\r\n");
    result = thermal_filter_sample_interval_test(0x00000000, 0x0000000F);
    
exit:
    thermal_exit();
    return result; 
}

int thermal_interrupt_mask_test(BOOL fgIsr)
{
    UINT32 temp = 0, i = 0, count = 0;
    
    thermal_real_test = false;

	vDrvThermalInitISR();

	DRV_WriteReg32(TEMPMONCTL1, 0x000000F0); // 3FF);    // counting unit is 1024 / 66M = 15.5us
	DRV_WriteReg32(TEMPMONCTL2, 0x02190219); // 219);	// sensing interval is 537 * 15.5us = 8.3235ms
	DRV_WriteReg32(TEMPAHBPOLL,  0x00000001);
	DRV_WriteReg32(TEMPAHBTO,    0x00000333);
	
    DRV_WriteReg32(TEMPMONIDET0, 0x00000000);               // times=1 for interrupt occurrance of sensing point 0
    DRV_WriteReg32(TEMPMONIDET1, 0x00000000);               // times=1 for interrupt occurrance of sensing point 1
    DRV_WriteReg32(TEMPMONIDET2, 0x00000000);               // times=1 for interrupt occurrance of sensing point 2
    DRV_WriteReg32(TEMPHTHRE, 0x000000AA);                  // set hot threshold
    DRV_WriteReg32(TEMPOFFSETH, 0x000000BB);                // set high offset threshold
    DRV_WriteReg32(TEMPH2NTHRE, 0x000000FF);                // set hot to normal threshold
    DRV_WriteReg32(TEMPOFFSETL, 0x00000111);                // set low offset threshold
    DRV_WriteReg32(TEMPCTHRE, 0x00000133);                  // set cold threshold
    DRV_WriteReg32(TEMPMSRCTL0, 0x0000000);                 // temperature measurement sampling control

	DRV_WriteReg32(TEMPADCPNP0, 0x172);
	DRV_WriteReg32(TEMPADCPNP1, 0x1B2);
	DRV_WriteReg32(TEMPADCPNP2, 0x1F2);
	DRV_WriteReg32(TEMPADCMUX, 0x11);		
	DRV_WriteReg32(TEMPADCEXT, 0x22);		 
	DRV_WriteReg32(TEMPADCEXT1, 0x33);				
    temp = 0x14;                                            // set to very hot for pnp0
    DRV_WriteReg32(TEMPADCEN, (0x00001000 + temp));        // set sensor voltage and sensor valid
    
	DRV_WriteReg32(TEMPPNPMUXADDR, SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCENADDR, SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCVALIDADDR,SPARE_REPLAC);
	DRV_WriteReg32(TEMPADCVOLTADDR, SPARE_REPLAC);
	
	DRV_WriteReg32(TEMPRDCTRL, 0x1);			 
	DRV_WriteReg32(TEMPADCVALIDMASK, 0x0000002C);
	DRV_WriteReg32(TEMPADCVOLTAGESHIFT, 0x0);
	DRV_WriteReg32(TEMPADCWRITECTRL, 0x00000000);	

	if (fgIsr)    DRV_WriteReg32(TEMPMONINT, 0x0000FFFF);
	else     DRV_WriteReg32(TEMPMONINT, 0x00000000);                 // !!! disable all interrupt
    DRV_WriteReg32(TEMPMONCTL0, 0x00000007);                // enable all sensing point (sensing point 0,1,2)
	
    for (i = 0; i < 5; i++)
        kal_sleep_task(10000);	
	
    count = 10;
    while (thermal_intr_flag != true && count--)
    {
        kal_sleep_task(10000);
    }
	
    temp = DRV_Reg32(TEMPMSR0);
    while ((temp & 0x0FFF) != 0x14)
    {
        kal_sleep_task(10000);
        temp = DRV_Reg32(TEMPMSR0);
    }

    for (i = 0; i < (400 * 2 * 1 * 2); i++)
		HAL_Delay_us(1000);
	
    if ((temp & 0x0FFF) != 0x14) // first filter valid should be equal to 0x14
        LOG(6,"thermal_interrupt_mask_test: fail, read TEMPMSR0(0x14) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_interrupt_mask_test: pass, read TEMPMSR0(0x14) = 0x%x\n", (temp & 0x0FFF));
    
    temp = DRV_Reg32(TEMPMSR1);
    if ((temp & 0x0FFF) != 0x14) // first filter valid should be equal to 0x14
        LOG(6,"thermal_interrupt_mask_test: fail, read TEMPMSR1(0x14) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_interrupt_mask_test: pass, read TEMPMSR1(0x14) = 0x%x\n", (temp & 0x0FFF));

    temp = DRV_Reg32(TEMPMSR2);
    if ((temp & 0x0FFF) != 0x14) // first filter valid should be equal to 0x14
        LOG(6,"thermal_interrupt_mask_test: fail, read TEMPMSR2(0x14) = 0x%x\n", (temp & 0x0FFF));
    else
        LOG(6,"thermal_interrupt_mask_test: pass, read TEMPMSR2(0x14) = 0x%x\n", (temp & 0x0FFF));
        
    LOG(6,"\n");
        
    thermal_intr_flag = false;
    
    return 0;
}

int THERMAL_Interrupt_Mask_Check(void)
{
    int result = 1;
    
    LOG(6,"THERMAL_Interrupt_Mask_Check: mask all interrupt\r\n");
    result = thermal_interrupt_mask_test(0);
    if (result < 0)
        goto exit;

    LOG(6,"THERMAL_Interrupt_Mask_Check: enable all interrupt\r\n");
    result = thermal_interrupt_mask_test(1);    
exit:
    thermal_exit();
    return result; 
}

void TS_THERMAL_Module_Test(UINT32 psInput)
{
    int result = 0;
 
    switch (psInput)
    {
        case CHECK_INTERRUPT_TRIGGER_SW:
            LOG(6,"ts_thermal: enter THERMAL_Interrupt_Trigger_Check(0)\r\n");
            result = THERMAL_Interrupt_Trigger_Check(0);
            break;

        case CHECK_INTERRUPT_TRIGGER_HW:
            LOG(6,"ts_thermal: enter THERMAL_Interrupt_Trigger_Check(1)\r\n");
            result = THERMAL_Interrupt_Trigger_Check(1);
            break;
			
        case CHECK_INTERRUPT_OCCURRANCE:
            LOG(6,"ts_thermal: enter THERMAL_Interrupt_Occurrance_Check()\r\n");
            result = THERMAL_Interrupt_Occurrance_Check();
            break;
			
        case CHECK_SENSING_FILTER_OPTION:
            LOG(6,"ts_thermal: enter THERMAL_Sensing_Filter_Option_Check()\r\n");
            result = THERMAL_Sensing_Filter_Option_Check();
            break;
            
			
/*			
        case CHECK_IMMEDIATE_MEASUREMENT:
            LOG(6,"ts_thermal: enter THERMAL_Immediate_Measurement_Check()\r\n");
            result = THERMAL_Immediate_Measurement_Check();
            break;
*/
            
        case CHECK_AHB_TIMEOUT:
            LOG(6,"ts_thermal: enter THERMAL_AHB_Timeout_Check()\r\n");
            result = THERMAL_AHB_Timeout_Check();
            break;


        case CHECK_DIFFERENT_THRESHOLD:
            LOG(6,"ts_thermal: enter THERMAL_Different_Threshold_Check()\r\n");
            result = THERMAL_Different_Threshold_Check();
            break;
									
        case CHECK_FILTER_SAMPLE_INTERVAL:
            LOG(6,"ts_thermal: enter THERMAL_Filter_Sample_Interval_Check()\r\n");
            result = THERMAL_Filter_Sample_Interval_Check();
            break;
        case CHECK_INTERRUPT_MASK:
            LOG(6,"ts_thermal: enter THERMAL_Interrupt_Mask_Check()\r\n");
            result = THERMAL_Interrupt_Mask_Check();
            break;
/*            
        case CHECK_INTERRUPT_Trigger_To_SPM:
        	LOG(6,"ts_thermal: enter THERMAL_Interrupt_Trigger_To_SPM_Check()\r\n");
        	result = Thermal_Interrupt_Trigger_to_SPM_Check();
        	break;
        case CHECK_SLT:
            LOG(6,"ts_thermal: enter THERMAL_SLT_Check()\r\n");
            result = Thermal_SLT_Check();
            break;
        case CHECK_TSENSE:
            LOG(6,"ts_thermal: enter THERMAL_Tsense_Check()\r\n");
            result = Thermal_Tsense_Check();
            break;
*/            

		case CHECK_WATCHDOG:
		result = Thermal_WatchDog_Check();
		break;
		

        default:
            result = -1;
            break;
    }

	 LOG(6,"ts_thermal: enter THERMAL_Interrupt_Trigger_Check()=%d \r\n", result);
}


