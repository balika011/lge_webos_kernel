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
 * $RCSfile: pi_tuner_teqe3.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
#include "atd_feature.h"
#include "pi_def_atd.h"
#include "drvcust_if.h"
#include "drv_tvd.h"
#include "pi_demod_setsystem_reg.h"
#include "x_gpio.h"

#define cVIF_TARGET_FREQ    -3375  //kHz
#define cPGA_GAIN_IDX_MIN   0x00
#define cPGA_GAIN_IDX_MAX   0x12

#define cPWM_TUNER_30V_FREQ_Hz   800000   //  800Khz
#define cPWM_TUNER_30V_DUTY   0x06      //Duty Cycle = cPWM_TUNER_30V_DUTY / cPWM_TUNER_30V_DUTYALL 
#define cPWM_TUNER_30V_DUTYALL   0x09 
#define cLOW_IF_BOUNDARY 		10000
#if TVD_SUPPORT_SNOW_MODE
#define cTVD_SNOW_INTENSITY  20
#endif

UINT16 g_CPR_delay=0;
UINT16 g_CPR_cnt=0;
UINT16 g_pra_cnt=0;
UINT16 g_CR_BW_AutoConfig_Delay_cnt=3;
#if defined(CC_SUPPORT_EU_US_COMPATIABLE) // iven temp mark
extern UINT8 DynamicSwitch_DTD_DVBT_Mode;/*BRDCST_IF= DTDDVBT_ALL_P_analog for SLT, DTD: DynamicSwitchMode=0,DVBT: DynamicSwitchMode=1(default)*/
#endif
#if ATD_SUPPORT_NCR
//BOOL fg_NCR_Sel = TRUE;  //d20120116_hongji: NCR is 1,OCR is 0. Default is NCR
EXTERN BOOL fg_NCR_Sel;
EXTERN BOOL fgNCN_FLAG ;  
EXTERN BOOL fgLCN_STATUS;
#if ATD_SUPPORT_HW_AUTO_BW
EXTERN BOOL fg_cci_disable_hbw;
#endif
#if defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)                
EXTERN BOOL fg_jump_weighting_status;
#endif
EXTERN UINT8 su1KpUpCnt;
#endif

#if AUDIO_SUPPORT_SNOW_MODE
BOOL fgAudioSnow_Sel=FALSE; // true:avs audio snow; false :video snow as audio snow
#endif

extern UINT8 u1FMRadioDelay;

EXTERN BOOL fgDeScrambleBox;

#ifdef CC_SUPPORT_SIF_BYPASS
UINT8 fgSifBypassEnabled = FALSE;
#endif
UINT32 u4LastRfFreq = 0;
UINT32 u4LastColorSystem=0;
#if ATD_SUPPORT_CHF
STATIC UINT8 g_u1CHFBw=0;//d120206_Haibo:record current chf bw setting
#endif
STATIC UINT16 u1ArrRegisterMap[]=
{
    //module                                 start_addr                                           len
    MODULE_MASK_ADCIF, REG_ATD_ADCIF_CFG,      4,//0x120~0x123
    MODULE_MASK_ADCIF, REG_ATD_SLD_ADCIF_CFG1, 14,//0x280~0x28d
    MODULE_MASK_TVDIF, REG_ATD_TVDIF_CFG0,     4,//0x124~0x127 
    MODULE_MASK_TVDIF, REG_ATD_TVDIF_CFG4,     8,//0x1e0~0x1e7
    MODULE_MASK_DC,    REG_ATD_ATD_DC_RFA2,    4,//0x128~0x12B  
    MODULE_MASK_EQ,    REG_ATD_EQ_CFG,         4,//0x12c~0x12f
    MODULE_MASK_CTL,   REG_ATD_CTL_SYS1,       16,//0x130~0x13F
    MODULE_MASK_DRO,   REG_ATD_DRO_LP_CFG01,   28,//0x164~0x17F
    MODULE_MASK_DRO,   REG_ATD_DRO_OBS2,     32,//0x200~0x21F
    MODULE_MASK_DRO,   REG_ATD_DRO_MAG_NOISE_0,32,//0x230~0x24F
    MODULE_MASK_DRO,   REG_ATD_DRO_VSI_CFG1,   16,//0x270~0x27F
    MODULE_MASK_POA,   REG_ATD_POA_CFG,        16,//0x180~0x18F
    MODULE_MASK_CR,    REG_ATD_ACR_PN_CFG0,    48,//0x190~0x1BF
    MODULE_MASK_CR,    REG_ATD_ADVPS_CFG0,     32,//0x250~0x26F
    MODULE_MASK_AVS,   REG_ATD_AVS_RFA3,       8,//0x1c0~0x1c7
    MODULE_MASK_VOP,   REG_ATD_VOP_FWCPOIH,    8,//0x1c8~0x1cf
    MODULE_MASK_CPR,   REG_ATD_CPR_EXT_FREQ1,  8,//0x1d0~0x1d7
    MODULE_MASK_AUC,   REG_ATD_AUC_CFG1,       8,//0x220~0x227
    MODULE_MASK_DAGC,  REG_ATD_SLD_DAGC_00,    32,//0x290~0x2AF
    MODULE_MASK_PRA,   REG_ATD_SLD_PRA_00 ,    79,//0x2b0~0x2fe
};
STATIC const CHAR * sModule[]=
{
  	"ADCIF",
	"TVDIF",
	"DC",
	"EQ",
	"CTL",
	"DRO",
	"POA",
	"CR",
	"AVS",
	"VOP",
	"CPR",
	"AUC",
	"DAGC",
	"PRA"
};
STATIC UINT16 u2HiddenRegIdx[]=
{
    //Start idx ,   len
    0, 4,
    11, 7,
    21, 1,
    29, 1,
    30, 4,
    35, 3,
    59, 1,
    63, 6,
    81, 1,
    82,8,
    93, 2,
    103,4,
    114,2,
    120,1,
    123,4,
    132,2,
    155,7,
    163,2,
    168,4,
    174,4,
    190,12,
    205,6,	
    222,3,
    228,1,
    232,2,
    235,1,
    237,3,
    250,8,
    258,1,
    263,3,
    270,4,
    278,2,
    284,6,
    308,2,
    312,10,
    329,3,
    354,17,
    372,1,
    374,1,
    376,2,
    392,1,
    400,1
};
UINT8 ATD_ChipInit(ATD_CTX_T *psDemodCtx)
{
    UINT8 ucData;    
#if defined(CC_MT5398)||defined(CC_MT5880) || defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) //d20120629_hongji:only for cobra/viper/python
#ifdef CC_SUPPORT_STR
    //ATD clock 0xF000D1CC[10]:0 off,1 on.
    IO_WRITE32MSK(0xF000D000, 0x1cc, 0x00000400, 0x00000400); //0xD1CC[10]=1 
    mcSHOW_DBG_MSG(("ON ATD clock for power resume \n"));
    //ATD reset 0xF000D1C4[10]:0 reset,1 not reset.
    IO_WRITE32MSK(0xF000D000, 0x1c4, 0x00000400, 0x00000400); //0xD1C4[10]=1     
    mcSHOW_DBG_MSG(("Not reset ATD for power resume \n"));
#endif    
#endif

    // Analog FE setting
#if defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389)
    // ADC Setup
    IO_WRITE32(0xF0061000, 0x500, 0x00000100);//d110804_Haibo:Power on PGA (0xf0061500[2:1]=00)
    IO_WRITE32(0xF0061000, 0x504, 0x0008A694); //d110421_Haibo:change from 0x0008A894 to 0x0008A694 as CC Hou suggested
    IO_WRITE32(0xF0061000, 0x400, 0x00AAA882);//d110804_Haibo:Power on ADC(0xF0061400[0]=0) 
    mcDELAY_MS(1);
    IO_WRITE32(0xF0061000, 0x400, 0x00AAA082);
	IO_WRITE32(0xF0061000, 0x400, 0x00AAA882);
    // Demod Clock
    IO_WRITE32(0xF000D000, 0x204, 0x00000300);     //From xtal(27MHz) // IO_WRITE32(0xF000D000, 0x204, 0x00000100);
    IO_WRITE32(0xF000D000, 0x304, 0x00000001); 
    IO_WRITE32(0xF000D000, 0x218, 0x00000001); 
    IO_WRITE32(0xF000D000, 0x21C, 0x00000002); 
    IO_WRITE32(0xF000D000, 0x300, 0x00000002); 
    IO_WRITE32(0xF000D000, 0x308, 0x00000001); 
    IO_WRITE32(0xF000D000, 0x30C, 0x00000001); 
    // Demod mode
#if defined(CC_MT5396) || defined(CC_MT5368)//d110617_Haibo:Only MT5396/68 need to switch register base 
#if defined(DTD_INCLUDE)//NA mode.Base is 0x2B
#if defined(CC_SUPPORT_EU_US_COMPATIABLE)
			if(DynamicSwitch_DTD_DVBT_Mode == 0)
				{
				IO_WRITE32(0xF002B000, 0x4B4, 0x000F0000);		 
				IO_WRITE32(0xF002B000, 0xE00, 0x000010C2);//lei_code_dB110810 for CAM 10.7M clock issue.
				}
			else
				{
				IO_WRITE32(0xF002A000, 0x4B4, 0x000F0000);  
    			IO_WRITE32(0xF002A000, 0xE00, 0x00001042);//lei_code_dB110810 for CAM 10.7M clock issue.
				}
#else
    IO_WRITE32(0xF002B000, 0x4B4, 0x000F0000);  
    IO_WRITE32(0xF002B000, 0xE00, 0x000010C2);//lei_code_dB110810 for CAM 10.7M clock issue.
#endif
#else//EU mode.Base is 0x2A
    IO_WRITE32(0xF002A000, 0x4B4, 0x000F0000);  	 
    IO_WRITE32(0xF002A000, 0xE00, 0x00001042);//lei_code_dB110810 for CAM 10.7M clock issue.
#endif
#else//MT5389
    //NA ATD mode
    ucData = 0xC0;
    ATD_SetReg(psDemodCtx,REG_ATD_DEMOD_GPM_E00,&ucData,1);
#endif
    //d110907_Haibo:Reset CDC(ana if)
    ATD_GetReg(psDemodCtx,REG_ATD_DEMOD_GPM_E01,&ucData,1);
    ucData&=0xEF;
    ATD_SetReg(psDemodCtx,REG_ATD_DEMOD_GPM_E01,&ucData,1);
    ucData|=0x10;
    ATD_SetReg(psDemodCtx,REG_ATD_DEMOD_GPM_E01,&ucData,1);
    mcSHOW_DBG_MSG(("Reset Demod Ana IF\n"));
#elif defined(CC_MT5398)  || defined(CC_MT5880) || defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)//d20120117_hongji:Analog FE Setting
    //ACD Setup    
#if defined(CC_MT5398)||defined(CC_MT5399)|| defined(CC_MT5882)  
#if defined(CC_MT5399)|| defined(CC_MT5882)
    IO_WRITE32(0xf0061000,0x504,0x0008A894);
#else
    IO_WRITE32(0xf0061000,0x504,0x0008A694); //d20120308_hongji:Follow 96 Setting according ACD DE Advice
#endif
    IO_WRITE32(0xf0061000,0x400,0x00AAA882);
    IO_WRITE32(0xf0061000,0x500,0x00000102);
    IO_WRITE32(0xf0061000,0x500,0x00000100);
#if defined(CC_MT5399)|| defined(CC_MT5882)
    IO_WRITE32(0xf0061000,0x504,0x0008A894);
#else    
    IO_WRITE32(0xf0061000,0x504,0x0008A694); //d20120308_hongji:Follow 96 Setting according ACD DE Advice
#endif
    IO_WRITE32(0xf0061000,0x400,0x00AAA882);
	mcDELAY_MS(1);
    IO_WRITE32(0xF0061000,0x400,0x00AAA082);
	IO_WRITE32(0xF0061000,0x400,0x00AAA882);
#if defined(CC_MT5399)|| defined(CC_MT5882)   
#ifdef CC_SUPPORT_SIF_BYPASS
    if(fgSifBypassEnabled)
    {
        IO_WRITE32MSK(0xF0061000, 0x508, 0x00800000, 0x00800000); //Bit[23]: RG_IFPGA_INPUT_SEL  
    }
#endif
#endif
#elif defined(CC_MT5890) //d20140411_steven:MT5890 use new ADC
    IO_WRITE32(0xf0061000,0x400,0x00041533);  //w 0xf0061400 0x00041533;set RG_DMRX_EN =0
    IO_WRITE32(0xf0061000,0x404,0x33040912);  //w 0xf0061404 0x33040912;set demod input common mode
    IO_WRITE32(0xf0061000,0x408,0x49AAA434);  //w 0xf0061408 0x49AAA434;set LDO voltage
    IO_WRITE32(0xf0061000,0x418,0x881100B0);  //w 0xf0061418 0x881100B0;set RG_CIC_FLT_EN =0
    IO_WRITE32(0xf0061000,0x400,0x40041533);  //w 0xf0061400 0x40041533;set RG_DMRX_en =1
    IO_WRITE32(0xf0061000,0x418,0x883100B0);  //w 0xf0061418 0x883100B0;set RG_CIC_FLT_EN =1   
#ifdef CC_SUPPORT_SIF_BYPASS
    if(fgSifBypassEnabled)
    {
        IO_WRITE32MSK(0xF0061000, 0x404, 0x00000000, 0x00040000); //Bit[18]=0: RG_DMAD_SIFOP_PWD=0;SIF OP power down option : OFF 
        IO_WRITE32MSK(0xF0061000, 0x40C, 0x80000000, 0x80000000); //Bit[31]=1: RG_DMAD_PULLOW_EN=1;SIF mode
        IO_WRITE32MSK(0xF0061000, 0x404, 0x00010000, 0x00030000); //Bit[17:16]=01: RG_CHSEL_DEMOD[1:0]=01;(pga) for SIF application
        IO_WRITE32MSK(0xF0061000, 0x404, 0x00001000, 0x00003800); //Bit[13:11]=x10: RG_DMAD_SIFOP_GAIN[13:11]=x10;x10: 0dB, Rin=7.5K, Vin=1.0Vppd
    }
#endif
#elif defined(CC_MT5880) || defined(CC_MT5881) //d20120306_hongji:MT5880 ACD Base address change
#if defined(CC_MT5881)
    IO_WRITE32(0xf0060000,0x604,0x0008A894); //d20121124_hongji:ACD suggestion for Remy
#else
    IO_WRITE32(0xf0060000,0x604,0x0008A694); //d20120308_hongji:Follow 96 Setting according ACD DE Advice
#endif    
    IO_WRITE32(0xf0060000,0x500,0x00AAA882);
    IO_WRITE32(0xf0060000,0x600,0x00000102);
    IO_WRITE32(0xf0060000,0x600,0x00000100);
#if defined(CC_MT5881)
    IO_WRITE32(0xf0060000,0x604,0x0008A894); //d20121124_hongji:ACD suggestion for Remy
#else
    IO_WRITE32(0xf0060000,0x604,0x0008A694); //d20120308_hongji:Follow 96 Setting according ACD DE Advice
#endif    
    IO_WRITE32(0xf0060000,0x500,0x00AAA882);
	mcDELAY_MS(1);
    IO_WRITE32(0xf0060000,0x500,0x00AAA082);
	IO_WRITE32(0xf0060000,0x500,0x00AAA882);
#endif       
    //Demod Clock Setting    
    IO_WRITE32(0xf000d000,0x3e0,0x00000001);  //c27m_ck (xtal_ck) 27MHz
    IO_WRITE32(0xf000d000,0x308,0x00000001);  //atdad_ck (sawlesspll_d2) 216MHz
    IO_WRITE32(0xf000d000,0x30c,0x00000002);  //clk54m_ck (sawlesspll_d8) 54MHz
    IO_WRITE32(0xf000d000,0x388,0x00000002);  //clk216m_ck (sawlesspll_d2) 216MHz
    IO_WRITE32(0xf000d000,0x38c,0x00000003);  //dvbtci_ck (syspll_d12) 54MHz
    IO_WRITE32(0xf000d000,0x3dc,0x00000200);  //dvbtfe_ck  (=clk54m_ck) 54MHz
#if defined(CC_MT5882)
	IO_WRITE32(0xf000d000,0x460,0x00000004);  //dstx_ck (sawlesspll_d4) 108MHz
#endif
    //Demod Mode    
#if defined(DTD_INCLUDE)//NA mode.Base is 0x2B
#if defined(CC_SUPPORT_EU_US_COMPATIABLE)
			if(DynamicSwitch_DTD_DVBT_Mode == 0)
				{
				IO_WRITE32(0xF002B000, 0x4B4, 0x000F0000);		 
				IO_WRITE32(0xF002B000, 0xE00, 0x000010C2);//lei_code_dB110810 for CAM 10.7M clock issue.
				}
			else
				{
				IO_WRITE32(0xF002A000, 0x4B4, 0x000F0000);  
    			IO_WRITE32(0xF002A000, 0xE00, 0x00001042);//lei_code_dB110810 for CAM 10.7M clock issue.
				}
#else
    IO_WRITE32(0xF002B000, 0x4B4, 0x000F0000);  
    IO_WRITE32(0xF002B000, 0xE00, 0x000010C2);//lei_code_dB110810 for CAM 10.7M clock issue.
#endif
#else//EU mode.Base is 0x2A
    IO_WRITE32(0xF002A000, 0x4B4, 0x000F0000);  	 
    IO_WRITE32(0xF002A000, 0xE00, 0x00001042);//lei_code_dB110810 for CAM 10.7M clock issue.
#endif
    //d20120201_hongji:ATD PGA Setting
    ucData = 0x0E;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG00,&ucData,1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG01,&ucData,1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG02,&ucData,1);
    ucData = 0x01;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG03,&ucData,1);
    ucData = 0xBF;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG04,&ucData,1);
    ucData = 0xFF;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG05,&ucData,1);
    ucData = 0x0A;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG06,&ucData,1);
    ucData = 0x03;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG07,&ucData,1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG08,&ucData,1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG09,&ucData,1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG10,&ucData,1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG11,&ucData,1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG12,&ucData,1);
    ucData = 0x0C;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG13,&ucData,1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG14,&ucData,1);
    ucData = 0x01;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG15,&ucData,1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG15,&ucData,1);
    
    //d110907_Haibo:Reset CDC(ana if)
    ATD_GetReg(psDemodCtx,REG_ATD_DEMOD_GPM_E01,&ucData,1);
    ucData&=0xEF;
    ATD_SetReg(psDemodCtx,REG_ATD_DEMOD_GPM_E01,&ucData,1);
    ucData|=0x10;
    ATD_SetReg(psDemodCtx,REG_ATD_DEMOD_GPM_E01,&ucData,1);
    mcSHOW_DBG_MSG(("Reset Demod Ana IF\n"));
#else
    ucData = 0x82;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG00,&ucData,1);
    ucData = 0xa8;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG01,&ucData,1);
    ucData = 0x5a;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG02,&ucData,1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG03,&ucData,1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG04,&ucData,1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG05,&ucData,1);
    ucData = 0x01;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG06,&ucData,1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG07,&ucData,1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG08,&ucData,1);
    ucData = 0x95;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG09,&ucData,1);
    ucData = 0xa8;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG10,&ucData,1);
    ucData = 0x09;
    ATD_SetReg(psDemodCtx,REG_ATD_AREG11,&ucData,1);
    //PDWNC
    IO_WRITE32MSK(0xF0028000, 0x0DC, 0x0d000084, 0x000000F0); // Xtal driving
    // CKGEN
    IO_WRITE32MSK(0xF000D000, 0x204, 0x00000300, 0x00000300); // C27M Clock, 11: c27m_ck = xtal_ck
    IO_WRITE32(0xF000d000, 0x21C, 0x00000001); //ATD54M Clock	
    IO_WRITE32(0xF000d000, 0x218, 0x00000001); //ATDAD Clock	
#endif

    // pinmux selection    
#ifdef 	CC_MT5395
    //Tuner IFAGC 0xF000D404[6]:1
    IO_WRITE32MSK(0xF000D000, 0x404, 0x00000040, 0x00000040); //0xD404[6] 
    //Tuner RF_AGC 0xF000D404[7]:1
    IO_WRITE32MSK(0xF000D000, 0x404, 0x00000080, 0x00000080); //0xD404[7] 
    //PWM30V PWM0
#if defined(CC_Tuner30V)
    IO_WRITE32MSK(0xF000D000, 0x404, 0x00000003, 0x00000002); //0xD404[1:0]  function2 PWM0
    IO_WRITE32(0xF0008000, 0x230, 0x59300001); // PWM0 = 1.6M 
    mcSHOW_DBG_MSG(("Tuner PWM30V  Enable for MT5395 SAWLess ATD\n"));
#endif
#elif defined(CC_MT5365)//65/66
    if (DRVCUST_InitGet(eFlagDDRQfp)) // for QFP
    {
        if(TRUE == psDemodCtx->pTCtx->fgRFAGC)
        {
            IO_WRITE32MSK(0xF000D000, 0x400, 0x00000100, 0x00000100); //bit8 RF AGC
        }
        if(!psDemodCtx->pTCtx->fgRFTuner) // legacy tuner IF AGC 
        {      
            IO_WRITE32MSK(0xF000D000, 0x400, 0x00001000, 0x00003000); //MX0[13:12]         	
            mcSHOW_DBG_MSG(("Tuner IF AGC Enable for MT5365 LEGACY ATD\n"));
        }	 
#if defined(CC_Tuner30V)
        else     // tuner 30V PWM, use drv API instead of direct reg access
        {
            if (DRVCUST_OptGet(eChinaATVTuner30v))  //liuyuan add//define at mt5365_driver/drv_cust/mtk/config/mt5365_china_atv.h
            {
                IO_WRITE32MSK(0xF000D000, 0x414, 0x01800000, 0x03800000);  //MT5365_P1V1_TUNER_30V use pimx5[25:23],fuction3
            }
            else
            {
                IO_WRITE32MSK(0xF000D000, 0x400, 0x00002000, 0x00003000); //MX0[13:12]   
            }  
                mcSHOW_DBG_MSG(("Tuner PWM30V Enable for MT5365 SAWLess ATD\n"));
                vDrvSetPWM( SrcPWM1,((BSP_GetDomainClock(SRC_BUS_CLK)/((cPWM_TUNER_30V_FREQ_Hz)*(cPWM_TUNER_30V_DUTYALL+1)))-1),
                            cPWM_TUNER_30V_DUTY,cPWM_TUNER_30V_DUTYALL);  //Q_hongji.wan: double confir this setting
        }
#endif
    }
    else  // for BGA
    {
        if(!psDemodCtx->pTCtx->fgRFTuner)       // legacy tuner IF AGC 
        {
            IO_WRITE32MSK(0xF000D000, 0x400, 0x00001000, 0x00003000); //MX0[13:12], IF AGC
            mcSHOW_DBG_MSG(("Tuner IF AGC Enable for MT5366 LEGACY ATD\n"));
        }	
#if defined(CC_Tuner30V)
        else//sawless tuner
        {   // tuner 30V PWM, use drv API instead of direct reg access
            IO_WRITE32MSK(0xF000D000, 0x40C, 0x10180000, 0x07000000); //MX3[26:24]            
            mcSHOW_DBG_MSG(("Tuner PWM30V Enable for MT5366 SAWLess ATD\n"));
            vDrvSetPWM(SrcPWM2,((BSP_GetDomainClock(SRC_BUS_CLK)/((cPWM_TUNER_30V_FREQ_Hz)*(cPWM_TUNER_30V_DUTYALL+1)))-1),
                        cPWM_TUNER_30V_DUTY,cPWM_TUNER_30V_DUTYALL);      
        }
#endif
    }
#elif defined(CC_MT5396) || defined(CC_MT5368) //96
  //Tuner IFAGC 0xF000D408[27]:1
  //Tuner RF_AGC 0xF000D404[19]:1
  // d120323_Haibo:Please refer to mtk_pin_setup.h to get IF/RF AGC pinmux setting
#if defined(CC_Tuner30V) 
    //PWM30V PWM0
    //d110331_Haibo:ToDo:Set PWM frequency
    vDrvSetPWM(SrcPWM0,((BSP_GetDomainClock(SRC_BUS_CLK)/((cPWM_TUNER_30V_FREQ_Hz)*(cPWM_TUNER_30V_DUTYALL+1)))-1),cPWM_TUNER_30V_DUTY,cPWM_TUNER_30V_DUTYALL);
    mcSHOW_DBG_MSG(("tuner30V--PWM0\n"));// PWM0 pin will be use to  TCL F20WT tuner 30V for 5368 not RFAGC switch
#else
    #ifndef CC_COBRA_DEMOD_RESET
    {//d110526_Haibo:Fix build warning:ISO C90 forbids mixed declarations and code
        // RF AGC switch
	 UINT32 u4GpioforRFAGCSwitch;
	 #ifndef CC_PROJECT_FOR_SONY
		 if( 0 == DRVCUST_OptQuery(eTunerRFAGCSwitchGpio,&u4GpioforRFAGCSwitch))
		 {
			 GPIO_SetOut(u4GpioforRFAGCSwitch,0);//d20110503_Haibo:Gpio set low for ATV
			 mcSHOW_DBG_MSG(("GPIO %d set low to switch RF AGC control\n",u4GpioforRFAGCSwitch));
		 }
	 #else
		 if( 0 == DRVCUST_OptQuery(eTunerRFAGCSwitchGpio,&u4GpioforRFAGCSwitch))
		 {
			 GPIO_SetOut(u4GpioforRFAGCSwitch,1);//d20140629_Xiaohong:Gpio set High for ATV(Temporary for Sony)
			 mcSHOW_DBG_MSG(("GPIO %d set high to switch IF AGC control\n",u4GpioforIFAGCSwitch));
		 }
	 #endif
	     }
	     #endif
	 #endif
 #elif defined(CC_MT5389)
    // d110617_Haibo:Please refer to mt5389m1v2_pin_setup.h to get IF/RF AGC pinmux setting
#if defined(CC_Tuner30V)
    vDrvSetPWM(SrcPWM1,((BSP_GetDomainClock(SRC_BUS_CLK)/((cPWM_TUNER_30V_FREQ_Hz)*(cPWM_TUNER_30V_DUTYALL+1)))-1),
                    cPWM_TUNER_30V_DUTY,cPWM_TUNER_30V_DUTYALL);//MT5389 tuner 30V  use PWM1	
    mcSHOW_DBG_MSG(("Set OPWM1 for tuner 30V \n"));
#endif					
#elif defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
    //Tuner IFAGC 0xf0000d604[7]=1	
    //IO_WRITE32MSK(0xF000D000, 0x604, 0x00000080, 0x00000080); //0xD604[7]
    //Tuner 0xf0000d604[11]=1
    //IO_WRITE32MSK(0xF000D000, 0x604, 0x00000800, 0x00000800); //0xD604[11]
#if defined(CC_Tuner30V) 
#if defined(CC_MT5398)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
    vDrvSetPWM(SrcPWM2,((BSP_GetDomainClock(SRC_BUS_CLK)/((cPWM_TUNER_30V_FREQ_Hz)*(cPWM_TUNER_30V_DUTYALL+1)))-1),cPWM_TUNER_30V_DUTY,cPWM_TUNER_30V_DUTYALL);
    mcSHOW_DBG_MSG(("tuner30V--PWM2\n"));// 5398 use OPWM2
#elif defined(CC_MT5881)
    vDrvSetPWM(SrcPWM1,((BSP_GetDomainClock(SRC_BUS_CLK)/((cPWM_TUNER_30V_FREQ_Hz)*(cPWM_TUNER_30V_DUTYALL+1)))-1),cPWM_TUNER_30V_DUTY,cPWM_TUNER_30V_DUTYALL);
    mcSHOW_DBG_MSG(("tuner30V--PWM1\n"));// 5881 use OPWM1
#else  //d20120306_hongji:MT5880 PWM Setting
    vDrvSetPWM(SrcPWM0,((BSP_GetDomainClock(SRC_BUS_CLK)/((cPWM_TUNER_30V_FREQ_Hz)*(cPWM_TUNER_30V_DUTYALL+1)))-1),cPWM_TUNER_30V_DUTY,cPWM_TUNER_30V_DUTYALL);
    mcSHOW_DBG_MSG(("tuner30V--PWM0\n"));// 5880 use OPWM0
#endif
#else
    #ifndef CC_COBRA_DEMOD_RESET
    {
        // RF AGC switch
        UINT32 u4GpioforRFAGCSwitch;
	#ifndef CC_PROJECT_FOR_SONY
		if( 0 == DRVCUST_OptQuery(eTunerRFAGCSwitchGpio,&u4GpioforRFAGCSwitch))
		{
			GPIO_SetOut(u4GpioforRFAGCSwitch,0);//d20110503_Haibo:Gpio set low for ATV
			mcSHOW_DBG_MSG(("GPIO %d set low to switch RF AGC control\n",u4GpioforRFAGCSwitch));
		}
	#else
		if( 0 == DRVCUST_OptQuery(eTunerRFAGCSwitchGpio,&u4GpioforRFAGCSwitch))
		{
			GPIO_SetOut(u4GpioforRFAGCSwitch,1);//d20140629_Xiaohong:Gpio set High for ATV(Temporary for Sony)
			mcSHOW_DBG_MSG(("GPIO %d set high to switch IF AGC control\n",u4GpioforRFAGCSwitch));
		}
        #endif
    }
    #endif
#endif

#endif   
 
    //Power on clk
    //REG_ATD_CTL_TEST, all clk power on
    ucData = 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_CTL_TEST, &ucData, 1);
    
#if defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
    //Set MT5396 RFAGC and IFAGC to normal mode  (0xF62_b3: RF AGC PAD and 0xF62_b2: IF AGC PAD) //d110331_Haibo:RF/IF hi-z register alternation
    ATD_GetReg(psDemodCtx, REG_ATD_SLD_CFG_1, &ucData, 1);
    ucData &= 0xF3;
    ucData |= 0x0C;
    ATD_SetReg(psDemodCtx, REG_ATD_SLD_CFG_1, &ucData, 1);
#else
    //Set MT5365 RFAGC and IFAGC PAD to enable  (0x124_b3: RF AGC PAD and 0x124_b2: IF AGC PAD) //EC_code_dA414 for eIntATD_ExtDTD
    ATD_GetReg(psDemodCtx, REG_ATD_TVDIF_CFG0, &ucData, 1);
    ucData &= 0xF3;
    ucData |= 0x0C;
    ATD_SetReg(psDemodCtx, REG_ATD_TVDIF_CFG0, &ucData, 1);
#endif
    return 0;
}


void ATD_SetIF(ATD_CTX_T *psDemodCtx)
{
    INT32 s4DsmOutFreq;
    INT32 s4IfCtrlWordtFreq;
    INT16 s2IfCtrlWord;
    UINT8 ucData;
#if  ATD_SUPPORT_CHF	
	INT16 i1TargetFreq;
#endif
	if(psDemodCtx->pTCtx->u2IF_Freq_A < cLOW_IF_BOUNDARY)
	{
        s4DsmOutFreq = (INT32)(-psDemodCtx->pTCtx->u2IF_Freq_A);
        ucData = 0xA0;
        ATD_SetReg(psDemodCtx, REG_ATD_SLD_ADCIF_DSM4A, &ucData, 1);
	}
	else
    	s4DsmOutFreq = (INT32)(-psDemodCtx->pTCtx->u2IF_Freq_A) +54000 -13500;  // vedio carrier freq after DSM out

#if  ATD_SUPPORT_CHF
	i1TargetFreq=1250-g_u1CHFBw*1000/2;
    // calculate IF ctrl word
    if (MOD_SECAM_L1 == psDemodCtx->u1SubSysIdx) 
	{
        ucData = 0x98; // spectrum inverse
        s4DsmOutFreq = -1*i1TargetFreq- s4DsmOutFreq;
    }
    else 
	{
	    ucData = 0x80;
		s4DsmOutFreq = i1TargetFreq - s4DsmOutFreq;
    }

#else
    // calculate IF ctrl word
    if (MOD_SECAM_L1 == psDemodCtx->u1SubSysIdx) {
        ucData = 0x98; // spectrum inverse
        s4DsmOutFreq = -cVIF_TARGET_FREQ - s4DsmOutFreq; // freq shift in DC
    }
    else {
        ucData = 0x80;
        s4DsmOutFreq =  cVIF_TARGET_FREQ - s4DsmOutFreq; // freq shift in DC
    }
#endif

	s2IfCtrlWord = (INT16)((s4DsmOutFreq << 10) /27000); 
	
	s4IfCtrlWordtFreq = (INT32)(s2IfCtrlWord*27000);	 //LW;d100926;FM resolution issue
	if (((s4DsmOutFreq*1024)-s4IfCtrlWordtFreq) > 13312) //LW_d100929; The resolution of DC frequency is about 26.367kHz
	{
		  s2IfCtrlWord += 1;
	}
    mcSHOW_DBG_MSG(("IfCtrlWord = %X , IF_Freq = %d\n", s2IfCtrlWord, psDemodCtx->pTCtx->u2IF_Freq_A));

#if ATD_SUPPORT_CHF
    // write IF ctrl word to DC
    ucData |= ((UINT8)(s2IfCtrlWord>>8) & 0x07);
    ATD_SetReg(psDemodCtx,REG_ATD_SLD_DC_CFG2,&ucData,1);
    ucData = (UINT8)(s2IfCtrlWord);
    ATD_SetReg(psDemodCtx,REG_ATD_SLD_DC_CFG3,&ucData,1);
//d120206_Haibo:set CPR according to BW
	if(6==g_u1CHFBw)//BW = 6MHz, CPR: move fv to "0"==>1.75
    {
        //OCPR Setting
     	ucData = 0x00;		
		ATD_SetReg(psDemodCtx,REG_ATD_CPR_EXT_FREQ1,&ucData,1);
		ucData = 0x84;		
		ATD_SetReg(psDemodCtx,REG_ATD_CPR_EXT_FREQ2,&ucData,1);
		ucData = 0xBD;		
		ATD_SetReg(psDemodCtx,REG_ATD_CPR_EXT_FREQ3,&ucData,1);
		ucData = 0xA1;		
		ATD_SetReg(psDemodCtx,REG_ATD_CPR_EXT_FREQ4,&ucData,1);
        //d20120220_hongji:NCRP Setting
        ucData = 0x00;		
		ATD_SetReg(psDemodCtx,REG_ATD_NCPR_EXT_FREQ1,&ucData,1);
        ucData = 0x84;		
		ATD_SetReg(psDemodCtx,REG_ATD_NCPR_EXT_FREQ2,&ucData,1);
        ucData = 0xBD;		
		ATD_SetReg(psDemodCtx,REG_ATD_NCPR_EXT_FREQ3,&ucData,1);
        ucData = 0xA1;		
		ATD_SetReg(psDemodCtx,REG_ATD_NCPR_EXT_FREQ4,&ucData,1);           
    }
	if(7==g_u1CHFBw)//BW = 7MHz ; move fv to "0"==>2.25 
	{
	    //OCPR Setting
	  	ucData = 0x00;		
		ATD_SetReg(psDemodCtx,REG_ATD_CPR_EXT_FREQ1,&ucData,1);
		ucData = 0xAA;		
		ATD_SetReg(psDemodCtx,REG_ATD_CPR_EXT_FREQ2,&ucData,1);
		ucData = 0xAA;		
		ATD_SetReg(psDemodCtx,REG_ATD_CPR_EXT_FREQ3,&ucData,1);
		ucData = 0xAB;		
		ATD_SetReg(psDemodCtx,REG_ATD_CPR_EXT_FREQ4,&ucData,1);
        //d20120220_hongji:NCRP Setting
        ucData = 0x00;		
		ATD_SetReg(psDemodCtx,REG_ATD_NCPR_EXT_FREQ1,&ucData,1);
        ucData = 0xAA;		
		ATD_SetReg(psDemodCtx,REG_ATD_NCPR_EXT_FREQ2,&ucData,1);
        ucData = 0xAA;		
		ATD_SetReg(psDemodCtx,REG_ATD_NCPR_EXT_FREQ3,&ucData,1);
        ucData = 0xAB;		
		ATD_SetReg(psDemodCtx,REG_ATD_NCPR_EXT_FREQ4,&ucData,1);        
	}
    if(8==g_u1CHFBw)//BW = 8MHz; CPR: move fv to "0"==>2.75 
	{
	    //OCPR Setting
	   	ucData = 0x00;		
		ATD_SetReg(psDemodCtx,REG_ATD_CPR_EXT_FREQ1,&ucData,1);
		ucData = 0xD0;		
		ATD_SetReg(psDemodCtx,REG_ATD_CPR_EXT_FREQ2,&ucData,1);
		ucData = 0x97;		
		ATD_SetReg(psDemodCtx,REG_ATD_CPR_EXT_FREQ3,&ucData,1);
		ucData = 0xB4;		
		ATD_SetReg(psDemodCtx,REG_ATD_CPR_EXT_FREQ4,&ucData,1);
        //d20120220_hongji:NCRP Setting
        ucData = 0x00;		
		ATD_SetReg(psDemodCtx,REG_ATD_NCPR_EXT_FREQ1,&ucData,1);
        ucData = 0xD0;		
		ATD_SetReg(psDemodCtx,REG_ATD_NCPR_EXT_FREQ2,&ucData,1);
        ucData = 0x97;		
		ATD_SetReg(psDemodCtx,REG_ATD_NCPR_EXT_FREQ3,&ucData,1);
        ucData = 0xB4;		
		ATD_SetReg(psDemodCtx,REG_ATD_NCPR_EXT_FREQ4,&ucData,1);        
	}
    //Enable External frequency and CR CFO compensation
	ATD_GetReg(psDemodCtx,REG_ATD_CPR_CFO_CFG,&ucData,1);
	ucData |=0x03;
	ATD_SetReg(psDemodCtx,REG_ATD_CPR_CFO_CFG,&ucData,1);
    //Enable External frequency and NCR CFO compensation
	ATD_GetReg(psDemodCtx,REG_ATD_NCPR_CFO_CFG,&ucData,1);
	ucData |=0x03;
	ATD_SetReg(psDemodCtx,REG_ATD_NCPR_CFO_CFG,&ucData,1);
#else
    ucData |= ((UINT8)(s2IfCtrlWord>>8) & 0x07);
    ATD_SetReg(psDemodCtx,REG_ATD_ATD_DC_CFG2,&ucData,1);
    ucData = (UINT8)(s2IfCtrlWord);
    ATD_SetReg(psDemodCtx,REG_ATD_ATD_DC_CFG3,&ucData,1);
#endif

    return; 
}

void ATD_SetPGA(ATD_CTX_T *psDemodCtx)
{
    UINT8 ucData;
    UINT8 ii;

    if(!psDemodCtx->pTCtx->fgRFTuner)//legacy tuner
    {
        // Enable IF AGC
        ucData = 0x11;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_2F,&ucData,1);
        if(TRUE == psDemodCtx->pTCtx->fgRFAGC)
        {   
            // Enable IF and RF AGC
            ucData = 0x31;
            ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_2F,&ucData,1);
        }
    }
    else//SAWLess tuner
    {
        // write PAG gain table into DAGC
        // for PGA gain dec
        ucData = 0x80;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_DAGC_0E,&ucData,1);
        for (ii = cPGA_GAIN_IDX_MIN; ii < cPGA_GAIN_IDX_MAX+1; ii++) 
        {
            // -0.6dB
            ucData = 0xEF;
            ATD_SetReg(psDemodCtx,REG_ATD_SLD_DAGC_0F,&ucData,1);
        }

        // for PGA gain inc
        ucData = 0xA0;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_DAGC_0E,&ucData,1);
        for (ii = cPGA_GAIN_IDX_MIN; ii < cPGA_GAIN_IDX_MAX+1; ii++) 
        {
            // 0.6dB
            ucData = 0x12;
            ATD_SetReg(psDemodCtx,REG_ATD_SLD_DAGC_0F,&ucData,1);
        }

        // set PGA max/min gain index
        ATD_GetReg(psDemodCtx, REG_ATD_SLD_PRA_31, &ucData, 1);
        ucData &= 0xE0;
        ucData |= cPGA_GAIN_IDX_MAX;
        ATD_SetReg(psDemodCtx, REG_ATD_SLD_PRA_31, &ucData, 1);
        ATD_GetReg(psDemodCtx, REG_ATD_SLD_PRA_32, &ucData, 1);
        ucData &= 0xE0;
        ucData |= cPGA_GAIN_IDX_MIN;
        ATD_SetReg(psDemodCtx, REG_ATD_SLD_PRA_32, &ucData, 1);
    }  
}

void ATD_SetAgcSld(ATD_CTX_T *psDemodCtx)
{
    UINT8 ucData;
    BOOL fgNeedAdjustSLD=FALSE;

    if((FALSE == psDemodCtx->pTCtx->fgRFTuner) && (TRUE == psDemodCtx->pTCtx->fgRFAGC))
    {
        // 0x2b7
        ucData = 0x15;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_07,&ucData,1);
        // 0x2b9
        ucData = 0x15;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_09,&ucData,1);
		// 0x2d4
		ucData = 0x10;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_24,&ucData,1);
        // 0x2d5
        ucData = 0xfe;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_25,&ucData,1);
        // 0x2d6
        ucData = 0xfe;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_26,&ucData,1);
        // 0x2d7
        ucData = 0xE0;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_27,&ucData,1);
        // 0x2d8
        ucData = 0xEE;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_28,&ucData,1);
        // 0x2d9
        ucData = 0xEE;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_29,&ucData,1);
        // 0x2da
        ucData = 0x7F;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_2A,&ucData,1);
        // 0x2db
        ucData = 0x7F;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_2B,&ucData,1);
        // 0x2dc
        ucData = 0x80;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_2C,&ucData,1);
        // 0x2cb
        ucData = 0x01;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_1B,&ucData,1);
		// 0x2b1
		ucData = 0x11;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_01,&ucData,1);
		// 0x2b2
		ucData = 0x06;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_02,&ucData,1);
		// 0x2b3
		ucData = 0xC0;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_03,&ucData,1);
    }
	else
    {
        // 0x2d4, RF bias
        ucData = 0x04;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_24,&ucData,1);
        // 0x2d5, IF bias1: 0xE0 --> 0x04
        ucData = 0x04;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_25,&ucData,1);
        // 0x2d6, IF bias2: 0xE0 --> 0x04
        ucData = 0x04;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_26,&ucData,1);
        // 0x2d7, RF slope
        ucData = 0xec;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_27,&ucData,1);
        // 0x2d8, IF slope1: 0xD0 --> 0xF4
        ucData = 0xF4;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_28,&ucData,1);
        // 0x2d9, IF slope2: 0xD0 --> 0xF4
        ucData = 0xF4;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_29,&ucData,1);
        // 0x2da, RF max
        ucData = 0x7f;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_2A,&ucData,1);
        // 0x2db, IF max
        ucData = 0x7f;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_2B,&ucData,1);
        // 0x2dc, RF min
        ucData = 0x80;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_2C,&ucData,1);
    }
    if(psDemodCtx->pTCtx->fgRFTuner)//sawless tuner
	{
        // 0x2ca
        ucData = 0x77;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_1A,&ucData,1);
	}
	else
	{
        // 0x2ca
        ucData = 0x88;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_1A,&ucData,1);
    }
    
#if defined(CC_MT5389)
    fgNeedAdjustSLD=TRUE;
#elif defined(CC_MT5396)
    if(IC_VER_5396_AA==BSP_GetIcVersion())//MT5396 before ECO
    {
        fgNeedAdjustSLD=TRUE;
    }
#endif

	if(fgNeedAdjustSLD)
    {
        if(psDemodCtx->pTCtx->u2IF_Freq_A < cLOW_IF_BOUNDARY)//Low if
        {
            // PRA target
            ucData = 0x10;
            ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_07,&ucData,1);
            // PRA target
            ucData = 0x10;
            ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_09,&ucData,1);
            // DAGC setting
            ucData = 0x08;
            ATD_SetReg(psDemodCtx,REG_ATD_SLD_DAGC_00,&ucData,1);
            ucData = 0x2a;
            ATD_SetReg(psDemodCtx,REG_ATD_SLD_DAGC_04,&ucData,1);
            ucData = 0x2a;
            ATD_SetReg(psDemodCtx,REG_ATD_SLD_DAGC_14,&ucData,1);
            //0x13b
            ucData = 0x80;
            ATD_SetReg(psDemodCtx,REG_ATD_CTL_SYS2,&ucData,1);
            //null region
            ucData = 0x17;
            ATD_SetReg(psDemodCtx,REG_ATD_PE_CFG0,&ucData,1);
        //secam
            if ((MOD_SECAM_L1 == psDemodCtx->u1SubSysIdx) || (MOD_SECAM_L == psDemodCtx->u1SubSysIdx)) 
            {
                // PRA target
                ucData = 0x05;
                ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_07,&ucData,1);
                // PRA target
                ucData = 0x05;
                ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_09,&ucData,1);
                // DAGC setting
                ucData = 0x88;
                ATD_SetReg(psDemodCtx,REG_ATD_SLD_DAGC_00,&ucData,1);
                //0x13b
                ucData = 0x81;
                ATD_SetReg(psDemodCtx,REG_ATD_CTL_SYS2,&ucData,1);
            }
        }
        else//normal IF
        {
            // DAGC setting
            ucData = 0x48;
            ATD_SetReg(psDemodCtx,REG_ATD_SLD_DAGC_00,&ucData,1);
            ucData = 0x38;
            ATD_SetReg(psDemodCtx,REG_ATD_SLD_DAGC_04,&ucData,1);
            ucData = 0x38;
            ATD_SetReg(psDemodCtx,REG_ATD_SLD_DAGC_14,&ucData,1);
            //null region
            ucData = 0x27;
            ATD_SetReg(psDemodCtx,REG_ATD_PE_CFG0,&ucData,1);

            //sawless tuner has different PRA target level.Need hysteresis
            if(psDemodCtx->pTCtx->fgRFTuner)
            {
                // PRA target
                ucData = 0x15;
                ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_07,&ucData,1);
                // PRA target
                ucData = 0x10;
                ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_09,&ucData,1);
            }
            else
            {
                // PRA target
                ucData = 0x15;
                ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_07,&ucData,1);
                // PRA target
                ucData = 0x15;
                ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_09,&ucData,1);
            }
            //secam
            if ((MOD_SECAM_L1 == psDemodCtx->u1SubSysIdx) || (MOD_SECAM_L == psDemodCtx->u1SubSysIdx)) 
            {
                if(psDemodCtx->pTCtx->fgRFTuner)
                {
                    // PRA target
                    ucData = 0x10;
                    ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_07,&ucData,1);
                    // PRA target
                    ucData = 0x0C;
                    ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_09,&ucData,1);
                }
                else
                {
                    // PRA target
                    ucData = 0x09;
                    ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_07,&ucData,1);
                    // PRA target
                    ucData = 0x09;
                    ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_09,&ucData,1);
                }
                // DAGC setting
                ucData = 0xC8;
                ATD_SetReg(psDemodCtx,REG_ATD_SLD_DAGC_00,&ucData,1);
            }
        }
    }
}                                     

void ATD_SetTunerRel(ATD_CTX_T *psDemodCtx)
{
    ATD_TUNER_REL_REG_T * pTunerRelReg;
    UINT8 ucData;
    mcSHOW_DBG_MSG(("ATD_SetTunerRel\n"));
#if defined(CC_MT5890) 
	if (ITUNER_OK == ITuner_OP(psDemodCtx->pTCtx, itSetRSetting, psDemodCtx->u1SubSysIdx, &ucData))
	{
        UINT32 RSetting;
		RSetting = ((UINT32)ucData)<<16;
		mcSHOW_DBG_MSG(("ATD R_Setting Vaule=%d \n",ucData));
	    IO_WRITE32MSK(0xF0061000, 0x418, RSetting, 0x00070000); //RSetting:bit[18:16],RG_DMAD_GAIN=001:
	}
#endif
    if (ITUNER_OK == ITuner_OP(psDemodCtx->pTCtx, itSetIfMin, psDemodCtx->u1SubSysIdx, &ucData))
    {
        // 0x2dd, IF min1
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_2D,&ucData,1);
        // 0x2de, IF min2
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_2E,&ucData,1);
    }
    else
    {
        // 0x2dd, IF min1
        ucData = 0x80;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_2D,&ucData,1);
        // 0x2de, IF min2
        ucData = 0x80;
        ATD_SetReg(psDemodCtx,REG_ATD_SLD_PRA_2E,&ucData,1);
    }

    if (ITUNER_OK == ITuner_OP(psDemodCtx->pTCtx, itGetTunerRelReg, psDemodCtx->u1SubSysIdx, &pTunerRelReg))
    {
        if(pTunerRelReg)
        {
            while(pTunerRelReg->u2RegAddr!=0)
            {
                mcSHOW_DBG_MSG(("RegWrite 0x%04X 0x%02X\n",pTunerRelReg->u2RegAddr,pTunerRelReg->u1RegVal));
                ATD_SetReg(psDemodCtx,pTunerRelReg->u2RegAddr, &pTunerRelReg->u1RegVal,1);
                pTunerRelReg++;
            }
        }
    }
}

#if defined(CC_MT5396)
void ATD_ApplyECOSetting(ATD_CTX_T *psDemodCtx,UINT8 u1SubSysIdx)
{
    UINT16 ii=0;
    UINT16 u2SetSystemEcoRegLen = sizeof(ATD_SetSystem_96ECO_Reg)/sizeof(ATD_REG_T);
    UINT8 u1TvSpec = u1SubSysIdx - MOD_ANA_TYPE_BEGIN;
    mcSHOW_DBG_MSG(("u2SetSystemEcoRegLen = %d, u1TvSpec = %d\n", u2SetSystemEcoRegLen, u1TvSpec));

    for (ii = 0; ii < u2SetSystemEcoRegLen; ii++) 
    {
        ATD_SetReg(psDemodCtx,ATD_SetSystem_96ECO_Reg[ii].Addr,&(ATD_SetSystem_96ECO_Reg[ii].RegVal[u1TvSpec]),1);
    }
}
#endif

#if ATD_SUPPORT_CHF  //d20120115_hongji:Initial Channel Filter Setting
/*
u1ChfBw:
  0:Default Setting according subsysem
  other:CHFBW = u1ChfBw
*/
void ATD_SetCHF(ATD_CTX_T *psDemodCtx,UINT8 u1ChfBw)  
{
    ATD_REG * pCHFReg = NULL; 
    if(0x00 == u1ChfBw)
    {
        if(MOD_NTSC_M == psDemodCtx->u1SubSysIdx)
        {
            pCHFReg = CHF_6M;
			g_u1CHFBw=6;
            mcSHOW_DBG_MSG(("Do 6M Channel Filter Setting for NTSC M \n"));
        }
        else if(MOD_PAL_BG == psDemodCtx->u1SubSysIdx)
        {
            if(psDemodCtx->u4Frequency < 300000)  //d20120131_hongji:PAL B CHF Setting
            {
                pCHFReg = CHF_8M;  //d20120401_hongji:8M CHF for PAL B NICAM
				g_u1CHFBw=8;
                mcSHOW_DBG_MSG(("Do 8M Channel Filter Setting for PAL B\n"));
            }
            else  //d20120131_hongji:PAL G CHF Setting
            {
                pCHFReg = CHF_8M;
				g_u1CHFBw=8;
                mcSHOW_DBG_MSG(("Do 8M Channel Filter Setting for PAL G\n"));
            }
        }
        else if((MOD_PAL_DK == psDemodCtx->u1SubSysIdx)||(MOD_PAL_I== psDemodCtx->u1SubSysIdx)||(MOD_SECAM_L == psDemodCtx->u1SubSysIdx) ||(MOD_SECAM_L1 == psDemodCtx->u1SubSysIdx))
        {
            pCHFReg = CHF_8M;
			g_u1CHFBw=8;
            mcSHOW_DBG_MSG(("Do 8M Channel Filter Setting for PAL DK/I and SECAM L/L1 \n"));
        }
        else
        {
            mcSHOW_DBG_MSG(("Invalidate Subsystem Parameter,Don't do channel filter setting \n"));
        }
    }
    else
    {
	    g_u1CHFBw=u1ChfBw;
        switch(u1ChfBw)
        {
            case 6:
                pCHFReg = CHF_6M;
                mcSHOW_DBG_MSG(("Do 6M Channel Filter Setting \n"));
                break;
            case 7:
                pCHFReg = CHF_7M;
                mcSHOW_DBG_MSG(("Do 7M Channel Filter Setting \n"));
                break;
            case 8:
                pCHFReg = CHF_8M;
                mcSHOW_DBG_MSG(("Do 8M Channel Filter Setting \n"));
                break;
            default:
                mcSHOW_DBG_MSG(("Invalidate CHF BW Parameter \n"));
                break;
        }

    }

    if(pCHFReg != NULL)
    {
        while(pCHFReg->Addr!=0)
        {
            mcSHOW_DBG_MSG4(("RegWrite 0x%04X 0x%02X\n",pCHFReg->Addr,pCHFReg->RegVal));
            ATD_SetReg(psDemodCtx,pCHFReg->Addr, &pCHFReg->RegVal,1);
            pCHFReg++;
        }
    }
    return;
}
#endif
#ifdef CC_SUPPORT_ATV_DESCRAMBLE
void ATD_InitDescramblePara(ATD_CTX_T *psDemodCtx)
{
    UINT16 u2CVBSDelay=0;
	TUNER_DESCRAMBLE_ATTRIBUTE_T * pDesAtt=&(psDemodCtx->pTCtx->sTunerDescramblePara);
    
	if(0==pDesAtt->u2CVBSDelay)//d120312_Haibo:Init default value according to tuner type(if tuner driver does not init the descramble parameter)
	{
	  if(psDemodCtx->pTCtx->fgRFTuner)
		{
			 #if defined(CC_MT5365)
			    u2CVBSDelay=0x7D00;//65.2us
			 #elif defined(CC_MT5389)	
			    u2CVBSDelay=0x0175;//65.2us
             #elif defined(CC_MT5396)	
				u2CVBSDelay=0x0255;//65.2us
			 #elif defined(CC_MT5398)	
			    u2CVBSDelay=0x02C4;//65.2us
			 #elif defined(CC_MT5880)
			 	u2CVBSDelay=0x02c0;//65.2us
			 #elif  defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
                u2CVBSDelay=0x02c0;//d20121024_hongji:waiting calibration for Remy
			 #endif	
		}
		else
		{
			 #if defined(CC_MT5365)
			    u2CVBSDelay=0x7700;//saw-tuner 64us
			 #elif defined(CC_MT5389)	
			    u2CVBSDelay=0x0169;//saw-tuner 64us
             #elif defined(CC_MT5396)	
				u2CVBSDelay=0x0244;//saw-tuner 64us
			 #elif defined(CC_MT5398)	
			    u2CVBSDelay=0x02b4;//saw-tuner 64us
			 #elif defined(CC_MT5880)
				u2CVBSDelay=0x02b0;//saw-tuner 64us
			 #elif  defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
                u2CVBSDelay=0x02c0;//d20121024_hongji:waiting calibration for Remy
			 #endif	
		}
		pDesAtt->u2CVBSDelay=u2CVBSDelay;
	}
}

void ATD_SetDelayLine(ATD_CTX_T *psDemodCtx)
{
	TUNER_DESCRAMBLE_ATTRIBUTE_T * pDesAtt=&(psDemodCtx->pTCtx->sTunerDescramblePara);

    UINT8 ucDelayArr[2],u1DelayLen=2,ucData,ucData1;
	UINT16 u2DelayReg=REG_ATD_DRO_LP_CFG05;
	
    if(pDesAtt->fgDescrambleBoxOn)            
   {
        mcSHOW_DBG_MSG(("ATD_SetDelayLine\n"));
		//set cvbs delay
        #if defined(CC_MT5365)
			u2DelayReg=REG_ATD_FIFO_CFG1;
			u1DelayLen=1;
		#endif	
		if(0 != pDesAtt->u2CVBSDelay)
		{
			ucDelayArr[0]=(UINT8)(pDesAtt->u2CVBSDelay>>8);
			ucDelayArr[1]=(UINT8)(pDesAtt->u2CVBSDelay);
			ATD_SetReg(psDemodCtx,u2DelayReg,ucDelayArr,u1DelayLen);
			mcSHOW_DBG_MSG(("Set Reg 0x%03X(%d) 0x%02X 0x%02x\n",u2DelayReg,u1DelayLen,ucDelayArr[0],ucDelayArr[1]));
		}
		if(0 != pDesAtt->u2ScalingGain) //only set scaling gain if non-zero value
		{
	        //set scaling gain(5:0)
	        ATD_GetReg(psDemodCtx,REG_ATD_DRO_SCAL_0,&ucData,1);
			ucData = ucData&0xC0;
			ucData1 = (pDesAtt->u2ScalingGain)&0x3F;
			ucData = ucData|ucData1;
		   ATD_SetReg(psDemodCtx,REG_ATD_DRO_SCAL_0,&ucData,1);
		}
   }
}
void ATD_GetDelayLine(ATD_CTX_T *psDemodCtx)
{
  	TUNER_DESCRAMBLE_ATTRIBUTE_T * pDesAtt=&(psDemodCtx->pTCtx->sTunerDescramblePara);
    UINT8 ucData;
	
	ATD_GetReg(psDemodCtx,REG_ATD_DRO_SCAL_0,&ucData,1);   
    //update scaling gain 
	pDesAtt->u2ScalingGain=ucData&0x3F;//bit[5:0]
}
void ATD_SetATDSnowmode(ATD_CTX_T *psDemodCtx,BOOL fgSnowOn)//set snow mode for descramble
{
      UINT8 ucData;

    ATD_GetReg(psDemodCtx, REG_ATD_TVDIF_CFG1, &ucData, 1);

    if (fgSnowOn) {
        ucData |= 0x11;
    }
    else {
        ucData &= 0xFE;
    }
    ATD_SetReg(psDemodCtx, REG_ATD_TVDIF_CFG1, &ucData, 1);
}
#endif

void ATD_SetSystem(ATD_CTX_T *psDemodCtx, UINT8 u1SubSysIdx)
{  
#if ATD_SUPPORT_NCR    //d20120116_hongji: Add NCR/OCR Select function
    UINT8 ucData;
#endif
    UINT16 ii;
    UINT8 u1TvSpec;
    UINT16 u2SetSystemRegLen = sizeof(ATD_SetSystem_Reg)/sizeof(ATD_REG_T);
 
	if(MOD_TYPE_UNKNOWN == u1SubSysIdx)//scan mode
	{
	    u1SubSysIdx=MOD_PAL_BG;   
	}
    psDemodCtx->u1SubSysIdx = u1SubSysIdx;
    u1TvSpec = u1SubSysIdx - MOD_ANA_TYPE_BEGIN;

    if ((u1SubSysIdx < MOD_ANA_TYPE_BEGIN) || (u1SubSysIdx > MOD_NTSC_M)) {
        mcSHOW_USER_MSG(("Wrong TvSpec = %d !!!!\n", u1SubSysIdx));
        return;
    }

    mcSHOW_DBG_MSG(("u2SetSystemRegLen = %d, u1TvSpec = %d\n", u2SetSystemRegLen, u1TvSpec));
    for (ii = 0; ii < u2SetSystemRegLen; ii++) 
    {       
        ATD_SetReg(psDemodCtx,ATD_SetSystem_Reg[ii].Addr,&(ATD_SetSystem_Reg[ii].RegVal[u1TvSpec]),1);
    }
#if ATD_SUPPORT_NCR    //d20120116_hongji: Add NCR/OCR Select function
    ATD_GetReg(psDemodCtx, REG_ATD_CR_NCR, &ucData, 1);
    if(TRUE == fg_NCR_Sel)  //d20120116_hongji:NCR Setting
    {
        ucData |= 0x01;
        ATD_SetReg(psDemodCtx, REG_ATD_CR_NCR, &ucData, 1);
    }
    else  //d20120116_hongji:OCR Setting
    {
        ucData &= 0xFE;
        ATD_SetReg(psDemodCtx, REG_ATD_CR_NCR, &ucData, 1);
    }
#endif    

        ATD_SetAgcSld(psDemodCtx);
        ATD_SetPGA(psDemodCtx);      
if(cCTMR_TUNER!=psDemodCtx->u1TunerType)//d120409_Haibo:For driver base application,IF and EQ will be set by AP
{
#if ATD_SUPPORT_CHF  //d20120115_hongji:Initial Channel Filter Setting
        ATD_SetCHF(psDemodCtx,0);  //channel filter setting according subsystem
#endif
       ATD_SetIF(psDemodCtx);
       ATD_SetEQ(psDemodCtx, itGetEqScriptNormal);
}
       ATD_SetNormalMode(psDemodCtx); 
       ATD_SetTunerRel(psDemodCtx); 
#if defined(CC_MT5396)
    if(IC_VER_5396_AA==BSP_GetIcVersion())//MT5396 before ECO
    {
        ATD_ApplyECOSetting(psDemodCtx,u1SubSysIdx);//d110922_Haibo:Load default register setting for MT5396IC before MP
    }
#endif 
	#ifdef CC_SUPPORT_ATV_DESCRAMBLE
	ATD_SetDelayLine(psDemodCtx);  
	#endif
}


UINT8 ATD_StartAcq(ATD_CTX_T *psDemodCtx, UINT8 fgScanMode)
{

    UINT8 ii, ucData,ucData3[3];
    UINT32 u4PilotPower,u4SignalPower;
#if !TVD_SUPPORT_SNOW_MODE	
    UINT32 u4ColorSystem=0;
#endif	
#if ATD_SUPPORT_NCR
    if(FALSE == fg_NCR_Sel)
    {
    ATD_SetPF(psDemodCtx, psDemodCtx->u1PF);
    if(0x00 == (psDemodCtx->u1PF))
    {
      ATD_SetNullRegion(psDemodCtx,1);
      g_CPR_cnt = 18;  //PA advice:70ms is enough for CR loop tracking
	}
    else
    {
       g_CPR_cnt=0;
    }
    }
	su1KpUpCnt=0;
#else
    ATD_SetPF(psDemodCtx, psDemodCtx->u1PF);
    if(0x00 == (psDemodCtx->u1PF))
    {
      ATD_SetNullRegion(psDemodCtx,1);
      g_CPR_cnt = 18;  //PA advice:70ms is enough for CR loop tracking
	}
    else
    {
       g_CPR_cnt=0;
    }
#endif
    if(fgScanMode)
    {
        ATD_SetScanMode(psDemodCtx);
#if !TVD_SUPPORT_SNOW_MODE//d120216_Haibo:Since scan mode will set channel filter again.So ATD need to be reset.And TVD snow mode is supportted,so there is no need to avoid ATD reset
        switch (psDemodCtx->u1SubSysIdx)	  
        {
            case MOD_PAL_BG: 
            case MOD_PAL_DK: 
            case MOD_PAL_I:
              u4ColorSystem=0;//negative modulation
            	 break;
            case MOD_SECAM_L:
            case MOD_SECAM_L1:
             u4ColorSystem=1;//positive modulation
            break;
            default :
            u4ColorSystem=0;//negative modulation
            break;
        }

        // do not reset demod if RF freq is not changed to avoid video flash
        if (u4LastRfFreq == psDemodCtx->pTCtx->u4RF_Freq && u4LastColorSystem == u4ColorSystem)
        {
		    mcSHOW_DBG_MSG3(("Same frequency,same modulation method ! ignore tuner acq, freq = %d\n", u4LastRfFreq));
		    return 0;
        }
        else 
        {
			u4LastRfFreq = psDemodCtx->pTCtx->u4RF_Freq;
			u4LastColorSystem=u4ColorSystem;//diff clor system detect :,pal VS secam
        }
#endif		
    }
#if ATD_SUPPORT_NCR
    if(FALSE == fg_NCR_Sel)
    {
	ATD_SetCrBwAutoCfg(psDemodCtx,FALSE);//d111108_Haibo:Disable CRBW auto config due to it will effect cr tracking speed
    }
#else
    ATD_SetCrBwAutoCfg(psDemodCtx,FALSE);//d111108_Haibo:Disable CRBW auto config due to it will effect cr tracking speed
#endif

    ucData = 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST1, &ucData, 1);
#if defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
    ATD_SetReg(psDemodCtx, REG_ATD_SLD_RST, &ucData, 1);//reset SLD part
#endif

    //Release PRA & CR,....
    ucData = 0xFF;
    ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST1, &ucData, 1);
	ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);    //for Channel scan balck mute CR
#if defined(CC_MT5396)   || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
    ATD_SetReg(psDemodCtx, REG_ATD_SLD_RST, &ucData, 1);
#endif
    
    // PGA gain fix min, move from SetPGA because the register will be default after reset ATD
    if(!psDemodCtx->pTCtx->fgRFTuner)//legacy tuner use PGA gain 0x00     
    {
        ucData = 0x00;
        ATD_SetReg(psDemodCtx, REG_ATD_SLD_PRA_30, &ucData, 1);
    }
	if (ITUNER_OK == ITuner_OP(psDemodCtx->pTCtx,itGetPGAIdx, psDemodCtx->u1SubSysIdx, &ucData))
    {
        ATD_SetReg(psDemodCtx, REG_ATD_SLD_PRA_30, &ucData, 1);
    }
    
    //PRA LOOP
    for (ii = 0; ii < 50; ii ++)
    {
        mcDELAY_MS(10);
        ATD_GetReg(psDemodCtx, REG_ATD_SLD_PRA_34, &ucData, 1);//for 8223 2E4[6:5]=10 means pra tracking state
        if ((ucData & 0x60) == 0x40) 
        {
            if((TRUE == psDemodCtx->pTCtx->fgRFTuner) )//only for sawless tuner ,
			{	
			    ATD_GetRFAGC(psDemodCtx);  //show RFAGC message
                ATDPI_GetIFAGC(psDemodCtx); //show IFAGC message(show PAG Gain for sawless tuner)
  
                if(ii > g_pra_cnt)
                {
                    break;
                }
			}
			else 
			{
                break;
            }
            
            mcSHOW_DBG_MSG(("PRA lock!, cnt = %d\n", ii));
        }
    }
	
    if(50 == ii)
    {
        mcSHOW_USER_MSG(("PRA acquisition fail!\n"));
        //return 1; mark this accorting to A2 project
    }
	
    //Reset CPR
	ucData = 0x10;  //don't reset DRO module
	ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);
	mcSHOW_DBG_MSG(("reset CPR!\n"));

    //Release CRP....
    ucData = 0xFF;
    ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);
#if !ATD_SUPPORT_NCR  //d20120115_hongji:OCR Setting
    //CPR LOOP
    for (ii = 0; ii < 20; ii ++)
    {
        mcDELAY_MS(10);
        ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE0, &ucData, 1);
        if ((ucData & 0x03) == 0x03)
        {
            mcSHOW_DBG_MSG(("ATD acquisition OK!\n"));
        
            Weak_Sinal_Indicator = 0;  // initialize driver patch variables
            Strong_Chroma_Indicator = 0;
            weak_count = 0;             
            NoOVMCounter0 = cNO_OVM_THRESHOLD;
            PNCounter = cPN_CNTR_THRESHOLD;	
            ATD_GetReg(psDemodCtx, REG_ATD_PE_CFG0, &u1NullRegion, 1);      
            u4PilotPower=ATD_GetPilotPower(psDemodCtx);
            ucData = (0x07 << 3); // Select ADVPS_PROB as SignalPower by setting 0x9BB[b5:b3] 0x07
            ATD_SetReg(psDemodCtx, REG_ATD_ADVPS_CFG2 , &ucData, 1);
            ATD_GetReg(psDemodCtx, REG_ATD_ADVPS_PRB0, ucData3, 3);            
            u4SignalPower = mcUNSIGN_2BYTE(ucData3[0], ucData3[1], 8, 8);
            u4SignalPower = (u4SignalPower << 8) + ucData3[2];
            mcSHOW_DBG_MSG(("Signal Power = %08d\n", u4SignalPower));

            ATD_GetCFOKHz(psDemodCtx, TRUE);          

            if((ii>g_CPR_cnt) || (u4PilotPower>150))
            {
                 break;
            }
        }
    }
    if(ii==20)
    {
        mcSHOW_USER_MSG(("CR acquisition fail!\n"));
        return 1;
    }
#else
    if(FALSE == fg_NCR_Sel)
    {
        //CPR LOOP
        for (ii = 0; ii < 20; ii ++)
        {
            mcDELAY_MS(10);
            ATD_GetReg(psDemodCtx, REG_ATD_CR_STATE0, &ucData, 1);
            if ((ucData & 0x03) == 0x03)
            {
                mcSHOW_DBG_MSG(("ATD acquisition OK!\n"));

                Weak_Sinal_Indicator = 0;  // initialize driver patch variables
                Strong_Chroma_Indicator = 0;
                weak_count = 0;             
                NoOVMCounter0 = cNO_OVM_THRESHOLD;
                PNCounter = cPN_CNTR_THRESHOLD;	
                ATD_GetReg(psDemodCtx, REG_ATD_PE_CFG0, &u1NullRegion, 1);      
                u4PilotPower=ATD_GetPilotPower(psDemodCtx);
                ucData = (0x07 << 3); // Select ADVPS_PROB as SignalPower by setting 0x9BB[b5:b3] 0x07
                ATD_SetReg(psDemodCtx, REG_ATD_ADVPS_CFG2 , &ucData, 1);
                ATD_GetReg(psDemodCtx, REG_ATD_ADVPS_PRB0, ucData3, 3);            
                u4SignalPower = mcUNSIGN_2BYTE(ucData3[0], ucData3[1], 8, 8);
                u4SignalPower = (u4SignalPower << 8) + ucData3[2];
                mcSHOW_DBG_MSG(("Signal Power = %08d\n", u4SignalPower));

                ATD_GetCFOKHz(psDemodCtx, TRUE);          

                if((ii>g_CPR_cnt) || (u4PilotPower>150))
                {
                     break;
                }
            }
        }    
        if(ii==20)
        {
            mcSHOW_USER_MSG(("CR acquisition fail!\n"));
            return 1;
        }
    }
    else
    {
        for(ii=0x00;ii<20;ii++)
        {
            mcDELAY_MS(10);
            ATD_GetReg(psDemodCtx, REG_ATD_NCR_FLL_6, &ucData, 1);
            if(0x20 == (ucData & 0x20))
            {
                mcSHOW_DBG_MSG(("NCR FLL Enter tracking mode \n"));
                //ATD_GetReg(psDemodCtx, REG_ATD_NCR_FLL_1, &ucData, 1);
                //ucData &= 0x7F;
                //ucData |= 0x40;
                //ATD_SetReg(psDemodCtx, REG_ATD_NCR_FLL_1, &ucData, 1); //d20120220_hongji:adjust decimating factor to 15 symbols
                Weak_Sinal_Indicator = 0;  // initialize driver patch variables
                Strong_Chroma_Indicator = 0;
                weak_count = 0;             
                NoOVMCounter0 = cNO_OVM_THRESHOLD;
                PNCounter = cPN_CNTR_THRESHOLD; 
                fgNCN_FLAG = TRUE;  //d20120216_hongji:initial NCR Patch
                fgLCN_STATUS = FALSE;
#if defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)                
                fg_jump_weighting_status = FALSE;
#endif
#if ATD_SUPPORT_HW_AUTO_BW
                fg_cci_disable_hbw = FALSE;
#endif
                //ATD_GetPilotStrength(psDemodCtx);
                //ATD_GetInchannelStrength(psDemodCtx);
                ATD_GetCFOKHz(psDemodCtx, TRUE);  
                break;
            }            
        }
        
        //if(20==ii)
        //{
            //mcSHOW_USER_MSG(("NCR FLL Acquisition fail \n"));
            //return 1;//d120504_Haibo:do not return fail since NCR will fail when there is no signal
        //}                        
    }    
#endif
    if(0!=g_CPR_delay)  //d20120115_hongji:CLI for CR Lock Time debug
    {
        mcDELAY_MS(g_CPR_delay);
        mcSHOW_DBG_MSG(("Delay %dms for CR lock!\n",g_CPR_delay));
    }

    return 0;  
}

void ATD_SetNormalMode(ATD_CTX_T *psDemodCtx)
{
#ifdef CC_MT8223
    UINT8 ucData;
    // Reg0x197[b7-b6] set to 00 Turn-off CR auto Re-acq config
    ATD_GetReg(psDemodCtx, REG_ATD_ACR_CFG, &ucData, 1);
    ucData = ucData & 0x3F;
    ATD_SetReg(psDemodCtx, REG_ATD_ACR_CFG, &ucData, 1);
#endif
    mcSHOW_DBG_MSG(("ATD_SetNormalMode(), System = %d\n", psDemodCtx->u1SubSysIdx-MOD_ANA_TYPE_BEGIN));
    
    ATD_SetSystemAudio(psDemodCtx, psDemodCtx->u1SubSysIdx);    
}

//strong after weak patch
void ATD_AVSStrongAfterWeakM(ATD_CTX_T *psDemodCtx)
{
    UINT8 ucData;
    //Center 6.0 BW(3dB)=1.256M/2
    mcSHOW_DBG_MSG(("ATD_AVSStrongAfterWeakM\n"));
    ucData = 0x8E; ATD_SetReg(psDemodCtx, REG_ATD_AVS_AOH, &ucData, 1); //0x1c2
    ucData = 0x64; ATD_SetReg(psDemodCtx, REG_ATD_AVS_AOL, &ucData, 1); //0x1c3
    ucData = 0x14; ATD_SetReg(psDemodCtx, REG_ATD_AVS_RFA0, &ucData, 1);//0x1c4
    ucData = 0x81; ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG1, &ucData, 1);//0x220
    ucData = 0xC7; ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG2, &ucData, 1);//0x221
    //AVS register can not be set real time and need to reset POA
    ATD_GetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);
    ucData &= 0xFB;
    ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);
    ucData |= 0x04;
    ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);
}
void ATD_SetSystemAudio(ATD_CTX_T *psDemodCtx, UINT8 u1SubSysIdx)
{
	UINT8   ucData=0;
	
	ATD_GetReg(psDemodCtx, REG_ATD_AVS_VOL, &ucData, 1);
    ucData = ucData | 0x08;
    ATD_SetReg(psDemodCtx, REG_ATD_AVS_VOL, &ucData, 1);
	
    switch (u1SubSysIdx)
    {
    case MOD_PAL_BG: 
        // set AVS/AUC audio filter,center:5.65M and BW(3dB)=0.9M
        ucData = 0x94; ATD_SetReg(psDemodCtx, REG_ATD_AVS_AOH, &ucData, 1);
        ucData = 0xE8; ATD_SetReg(psDemodCtx, REG_ATD_AVS_AOL, &ucData, 1);
        ucData = 0x94; ATD_SetReg(psDemodCtx, REG_ATD_AVS_RFA0, &ucData, 1);
        ucData = 0x81; ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG1, &ucData, 1);
        ucData = 0xAD; ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG2, &ucData, 1);        
        break;
    
    case MOD_PAL_DK: 
        // set AVS/AUC audio filter,center:6.22M and BW(3dB)=1.256M
        ucData = 0x8A; ATD_SetReg(psDemodCtx, REG_ATD_AVS_AOH, &ucData, 1); 
        ucData = 0x20; ATD_SetReg(psDemodCtx, REG_ATD_AVS_AOL, &ucData, 1); 
        ucData = 0x14; ATD_SetReg(psDemodCtx, REG_ATD_AVS_RFA0, &ucData, 1);
        ucData = 0x81; ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG1, &ucData, 1);
        ucData = 0xD8; ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG2, &ucData, 1);
        break;
    
    case MOD_PAL_I: 
        // set AVS/AUC audio filter,center:6.31M and BW(3dB)=1.256M
        ucData = 0x88; ATD_SetReg(psDemodCtx, REG_ATD_AVS_AOH, &ucData, 1); 
        ucData = 0x60; ATD_SetReg(psDemodCtx, REG_ATD_AVS_AOL, &ucData, 1); 
        ucData = 0x14; ATD_SetReg(psDemodCtx, REG_ATD_AVS_RFA0, &ucData, 1);
        ucData = 0x81; ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG1, &ucData, 1);
        ucData = 0xDF; ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG2, &ucData, 1);
        break;
    
    case MOD_SECAM_L: 
		// set AVS/AUC audio filter,center:6.22M and BW(3dB)=1.256M
        ucData = 0x8A; ATD_SetReg(psDemodCtx, REG_ATD_AVS_AOH, &ucData, 1); 
        ucData = 0x20; ATD_SetReg(psDemodCtx, REG_ATD_AVS_AOL, &ucData, 1); 
        ucData = 0x14; ATD_SetReg(psDemodCtx, REG_ATD_AVS_RFA0, &ucData, 1);
        ucData = 0x81; ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG1, &ucData, 1);
        ucData = 0xD8; ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG2, &ucData, 1);
        break;
    
    case MOD_SECAM_L1: 
		// set AVS/AUC audio filter,center:6.22M and BW(3dB)=1.256M
        ucData = 0x8A; ATD_SetReg(psDemodCtx, REG_ATD_AVS_AOH, &ucData, 1); 
        ucData = 0x20; ATD_SetReg(psDemodCtx, REG_ATD_AVS_AOL, &ucData, 1); 
        ucData = 0x14; ATD_SetReg(psDemodCtx, REG_ATD_AVS_RFA0, &ucData, 1);
        ucData = 0x81; ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG1, &ucData, 1);
        ucData = 0xD8; ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG2, &ucData, 1);
        break;  
    case MOD_NTSC_M:
        // set AVS/AUC audio filter,center:4.5M and BW(3dB)=0.9M
        ucData = 0xAA; ATD_SetReg(psDemodCtx, REG_ATD_AVS_AOH, &ucData, 1); 
        ucData = 0xE8; ATD_SetReg(psDemodCtx, REG_ATD_AVS_AOL, &ucData, 1); 
        ucData = 0x94; ATD_SetReg(psDemodCtx, REG_ATD_AVS_RFA0, &ucData, 1);
        ucData = 0x81; ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG1, &ucData, 1);
        ucData = 0x55; ATD_SetReg(psDemodCtx, REG_ATD_AUC_CFG2, &ucData, 1);
        break;         
    default:
        mcSHOW_DBG_MSG(("Set System = DEFAULT!!\n"));
        break;
    }	
    
    //AVS register can not be set real time and need to reset POA
    ATD_GetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);
    ucData &= 0xFB;
    ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);
    ucData |= 0x04;
    ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);
    
#ifdef CC_SUPPORT_ATV_DESCRAMBLE //For desramble box only
    if(psDemodCtx->pTCtx->sTunerDescramblePara.fgDescrambleBoxOn)
    {
			ATD_GetReg(psDemodCtx, REG_ATD_DRO_SCAL_7, &ucData, 1);
			ucData &= 0x80;
			ATD_SetReg(psDemodCtx, REG_ATD_DRO_SCAL_7, &ucData, 1);
			ucData |= 0x7F;
			ATD_SetReg(psDemodCtx, REG_ATD_DRO_SCAL_7, &ucData, 1);	
    }
#endif
	
	return;
}

void ATD_DisConnect(ATD_CTX_T *psDemodCtx)
{
    UINT8 ucData = 0;

    // reset u4LastRfFreq if ATD disconnected to make sure 0x134/135 would be enabled in StartAcq()
    u4LastRfFreq = 0;

#if defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
    //Set MT5396 RFAGC and IFAGC to high-Z   (0xF62_b3: RF AGC PAD and 0xF62_b2: IF AGC PAD) //d110331_Haibo:RF/IF hi-z register alternation
    ATD_GetReg(psDemodCtx, REG_ATD_SLD_CFG_1, &ucData, 1);//0xF62
    ucData &= 0xF3;
    ATD_SetReg(psDemodCtx, REG_ATD_SLD_CFG_1, &ucData, 1);   
#else
    ATD_GetReg(psDemodCtx, REG_ATD_TVDIF_CFG0, &ucData, 1);
    ucData &= 0xF3;
    ATD_SetReg(psDemodCtx, REG_ATD_TVDIF_CFG0, &ucData, 1);
    // set RF/IF AGC pad to open
#endif
    //Disbale ATD (b4 power down ADCLK)
    ucData = 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST1, &ucData, 1);
    ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);

#if defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
    ATD_SetReg(psDemodCtx, REG_ATD_SLD_RST, &ucData, 1);//d110331_Haibo:power down SLD part
#endif

    mcSHOW_DBG_MSG(("[ATD_DisConnect] Disable ATD Module\n"));
     
    //REG_ATD_CTL_TEST, all clk power down; 
    ucData = 0xFF;
    ATD_SetReg(psDemodCtx,REG_ATD_CTL_TEST,&ucData,1);
    mcSHOW_DBG_MSG(("[ATD_DisConnect] Disable ATD_CTL_TEST CLK\n"));

    //Power down ADC/PAG clock
    // IFADC power down 0x100H[b1_b0]: RG_HPF_PWD_DEMOD=1, RG_ADC_PWD_DEMOD=1
#if defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389)
    IO_WRITE32MSK(0xF0061000, 0x500, 0x00000006,0x00000006);//d110804_Haibo:Power off PGA (0xf0061500[2:1]=11)
    IO_WRITE32MSK(0xF0061000, 0x400, 0x00000001,0x00000001);//d110804_Haibo:Power off ADC(0xF0061400[0]=1) 
#elif  defined(CC_MT5398) || defined(CC_MT5399)|| defined(CC_MT5882)
	//change ADC source from sif to tuner if
#ifdef CC_SUPPORT_SIF_BYPASS
	IO_WRITE32MSK(0xF0061000, 0x508, 0x00000000, 0x00800000); //Bit[23]: RG_IFPGA_INPUT_SEL  
#endif
    //d20120731_hongji:Power down adc
    //RG_ADC_PWD_DEMOD & RG_HPF_PWD_DEMOD
    IO_WRITE32MSK(0xF0061000, 0x400, 0x00000003, 0x00000003);//0xf0061400[1:0]=11'b
    //RG_IFTESTBUF_PWD
    IO_WRITE32MSK(0xF0061000, 0x504, 0x00080000, 0x00080000);//0xf0061504[19]=1'b
    //d20120731_hongji:Power down pga
    //RG_PGAVOCM_PWD & RG_PGA_PWD
    IO_WRITE32MSK(0xF0061000, 0x500, 0x00000006, 0x00000006);//0xf0061500[2:1]=11'b
#elif defined(CC_MT5880) || defined(CC_MT5881)
    //d20120731_hongji:Power down adc
    //RG_ADC_PWD_DEMOD & RG_HPF_PWD_DEMOD
    IO_WRITE32MSK(0xF0060000, 0x500, 0x00000003, 0x00000003);//0xf0060500[1:0]=11'b
    //RG_IFTESTBUF_PWD
    IO_WRITE32MSK(0xF0060000, 0x604, 0x00080000, 0x00080000);//0xf0060604[19]=1'b
    //d20120731_hongji:Power down pga
    //RG_PGAVOCM_PWD & RG_PGA_PWD
    IO_WRITE32MSK(0xF0060000, 0x600, 0x00000006, 0x00000006);//0xf0060600[2:1]=11'b
#elif defined(CC_MT5890)
		//d20140411_steven:Power down adc
		//Set 0xf0061400[30]=1'b0;//set RG_DMRX_EN=0
		IO_WRITE32MSK(0xF0061000, 0x400, 0x00000000, 0x40000000);//0xf0061400[30]=1'b0;
		//Set 0xf0061418[21]=1'b0;//set RG_CIC_FLT_EN=0
		IO_WRITE32MSK(0xF0061000, 0x418, 0x00000000, 0x00200000);//0xf0061418[21]=1'b0;
#else
    ATD_GetReg(psDemodCtx, REG_ATD_AREG00, &ucData, 1);
    ucData |= 0x03;
    ATD_SetReg(psDemodCtx, REG_ATD_AREG00, &ucData, 1);
    // 2b105[2:1]=3. PGA power down
    ATD_GetReg(psDemodCtx, REG_ATD_AREG05, &ucData, 1);
    ucData |= 0x06;
    ATD_SetReg(psDemodCtx, REG_ATD_AREG05, &ucData, 1);
#endif
    mcSHOW_DBG_MSG(("[ATD_DisConnect] Power down IFADC CLK and PGA\n"));
#if defined(CC_MT5398)||defined(CC_MT5880) || defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) //d20120629_hongji:only for cobra/viper/python
#ifdef CC_SUPPORT_STR
    //ATD reset 0xF000D1C4[10]:0 reset,1 not reset.
    IO_WRITE32MSK(0xF000D000, 0x1c4, 0x00000000, 0x00000400); //0xD1C4[10]=0 
    mcSHOW_DBG_MSG(("Reset ATD for power suspend \n"));
    //ATD clock 0xF000D1CC[10]:0 off,1 on.
    IO_WRITE32MSK(0xF000D000, 0x1cc, 0x00000000, 0x00000400); //0xD1CC[10]=0 
    mcSHOW_DBG_MSG(("off ATD clock for power suspend \n"));
#endif    
#endif
    return;
}

//zhilin_20130206:set audio snow mode for audio po-po issue
void ATD_SetAudioSnowMode(ATD_CTX_T *psDemodCtx, UINT8 fgSnowOn)
{
	UINT8 ucData;
#if AUDIO_SUPPORT_SNOW_MODE
	if(fgAudioSnow_Sel)//avs new audio snow mode
	{
		ATD_GetReg(psDemodCtx, REG_ATD_AVS_VOL, &ucData, 1);

		if (fgSnowOn) 
		{
			ucData |= 0x01;
		}
		else 
		{
			ucData &= 0xFE;
		}
		ATD_SetReg(psDemodCtx, REG_ATD_AVS_VOL, &ucData, 1);
		mcSHOW_DBG_MSG(("Set New Audio Snow mode %s\n",fgSnowOn?"On":"Off"));
	}
	else //vedio snow as audio snow
	{
		ATD_GetReg(psDemodCtx, REG_ATD_TVDIF_CFG4, &ucData, 1);
		if (fgSnowOn) 
		{
			ucData |= 0x10;
		}
		else 
		{
			ucData &= 0xEF;
		}
		ATD_SetReg(psDemodCtx, REG_ATD_TVDIF_CFG4, &ucData, 1); 	
		mcSHOW_DBG_MSG(("Set Old Audio Snow mode %s\n",fgSnowOn?"On":"Off"));
	}
#else//vedio snow as audio snow
	{
		ATD_GetReg(psDemodCtx, REG_ATD_TVDIF_CFG4, &ucData, 1);
		if (fgSnowOn) 
		{
			ucData |= 0x10;
		}
		else 
		{
			ucData &= 0xEF;
		}
		ATD_SetReg(psDemodCtx, REG_ATD_TVDIF_CFG4, &ucData, 1); 	
		mcSHOW_DBG_MSG(("Set Old Audio Snow mode %s\n",fgSnowOn?"On":"Off"));
	}
#endif
	return;
}


void ATD_SetSnowMode(ATD_CTX_T *psDemodCtx, UINT8 fgSnowOn)
{
#if TVD_SUPPORT_SNOW_MODE
    mcSHOW_DBG_MSG(("Set Video Snow mode %s\n",fgSnowOn?"On":"Off"));
    _svSnowModeSet(fgSnowOn,cTVD_SNOW_INTENSITY);
#else
    UINT8 ucData;

    ATD_GetReg(psDemodCtx, REG_ATD_TVDIF_CFG1, &ucData, 1);

    if (fgSnowOn) 
    {
        ucData |= 0x11;
    }
    else 
    {
        ucData &= 0xFE;
    }
    ATD_SetReg(psDemodCtx, REG_ATD_TVDIF_CFG1, &ucData, 1);
#endif
#if ATD_SNOWSOUND_MODE
	ATD_SetAudioSnowMode(psDemodCtx,fgSnowOn);//zhilin_20130206:set audio snow mode for audio po-po issue
#endif
    return;
}
void ATD_BypassSIF2Aud(ATD_CTX_T *psDemodCtx, UINT8 u1SubSysIdx)
{
    UINT8 ucData;

    if ((u1SubSysIdx < MOD_ANA_TYPE_BEGIN) || (u1SubSysIdx > MOD_TYPE_UNKNOWN))
    {
        mcSHOW_USER_MSG(("Wrong TvSpec = %d !!!!\n", u1SubSysIdx));
        return;
    }
    mcSHOW_DBG_MSG(("ATD_BypassSIF2Aud %d\n",u1SubSysIdx));
#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) 
    //Set RFAGC and IFAGC to high-Z   (0xF62_b3: RF AGC PAD and 0xF62_b2: IF AGC PAD) 
    ATD_GetReg(psDemodCtx, REG_ATD_SLD_CFG_1, &ucData, 1);//0xF62
    ucData &= 0xF3;
    ATD_SetReg(psDemodCtx, REG_ATD_SLD_CFG_1, &ucData, 1); 
    //DSM4 and DCD Bypass
    ucData = 0xA0;
    ATD_SetReg(psDemodCtx, REG_ATD_SLD_ADCIF_DSM4A, &ucData, 1);
    ucData = 0x87;
    ATD_SetReg(psDemodCtx, REG_ATD_SLD_DC_CFG2, &ucData, 1);
    ucData = 0x80;
    ATD_SetReg(psDemodCtx, REG_ATD_SLD_DC_CFG3, &ucData, 1);
    //D-Saw Bypass
    ucData = 0xC0;
    ATD_SetReg(psDemodCtx, REG_ATD_SLD_CHF_CFG, &ucData, 1);
    ucData = 0xC0;
    ATD_SetReg(psDemodCtx, REG_ATD_GDE_05, &ucData, 1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_GDE_06, &ucData, 1);
    //FEDG Bypass
    ucData = 0xC0;
    ATD_SetReg(psDemodCtx, REG_ATD_FEDG_00, &ucData, 1);
    //DAGC Bypass:
    ucData = 0xC0;
    ATD_SetReg(psDemodCtx, REG_ATD_SLD_DAGC_00, &ucData, 1);
    //NCPR Bypass 
    ucData = 0x01;
    ATD_SetReg(psDemodCtx, REG_ATD_NCPR_EXT_FREQ1, &ucData, 1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_NCPR_EXT_FREQ2, &ucData, 1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_NCPR_EXT_FREQ3, &ucData, 1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_NCPR_EXT_FREQ4, &ucData, 1);
    ucData = 0x02;
    ATD_SetReg(psDemodCtx, REG_ATD_NCPR_CFO_CFG, &ucData, 1);
    //OCPR Bypass 
    ucData = 0x01;
    ATD_SetReg(psDemodCtx, REG_ATD_CPR_EXT_FREQ1, &ucData, 1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_CPR_EXT_FREQ2, &ucData, 1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_CPR_EXT_FREQ3, &ucData, 1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_CPR_EXT_FREQ4, &ucData, 1);
    ucData = 0x02;
    ATD_SetReg(psDemodCtx, REG_ATD_CPR_CFO_CFG, &ucData, 1);
    //NCR Bypass   
    ATD_GetReg(psDemodCtx, REG_ATD_NCR_FSM_4, &ucData, 1);//FLL trk off
    ucData &= 0xF8;
    ATD_SetReg(psDemodCtx, REG_ATD_NCR_FSM_4, &ucData, 1); 
    ATD_GetReg(psDemodCtx, REG_ATD_NCR_FLL_1, &ucData, 1); //FLL acq off
    ucData &= 0xE3;
    ATD_SetReg(psDemodCtx, REG_ATD_NCR_FLL_1, &ucData, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_NCR_FSM_12, &ucData, 1);//PLL kp off
    ucData &= 0x0F;
    ATD_SetReg(psDemodCtx, REG_ATD_NCR_FSM_12, &ucData, 1); 
    ATD_GetReg(psDemodCtx, REG_ATD_NCR_FSM_11, &ucData, 1);//PLL ki off
    ucData &= 0x0F;
    ATD_SetReg(psDemodCtx, REG_ATD_NCR_FSM_11, &ucData, 1); 
    ucData &= 0x80;//FPC off
    ATD_SetReg(psDemodCtx, REG_ATD_NCR_ADV_23, &ucData, 1);
    ATD_GetReg(psDemodCtx, REG_ATD_NCR_PLL_1, &ucData, 1);//Disable BN protect function
    ucData &= 0xFB;
    ATD_SetReg(psDemodCtx, REG_ATD_NCR_PLL_1, &ucData, 1); 
    //OCR Bypass        
    ucData = 0x6D;//0x1B7 Disable FPC
    ATD_SetReg(psDemodCtx, REG_ATD_PJ_CFG, &ucData, 1);    
    ucData = 0xC9;//0x199 Disable FIFO
    ATD_SetReg(psDemodCtx, REG_ATD_PFLL_CFG, &ucData, 1);
#else
    //DC bypass
    //0x282
    ucData = 0xA0;
    ATD_SetReg(psDemodCtx, REG_ATD_SLD_ADCIF_DSM4A, &ucData, 1);
    ucData = 0x80;
    ATD_SetReg(psDemodCtx, REG_ATD_ATD_DC_CFG2, &ucData, 1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_ATD_DC_CFG3, &ucData, 1);
    //Shift -3.375MHz via CPR
    ucData = 0x03;
    ATD_SetReg(psDemodCtx, REG_ATD_CPR_EXT_FREQ1, &ucData, 1);
    ucData = 0x80;
    ATD_SetReg(psDemodCtx, REG_ATD_CPR_EXT_FREQ2, &ucData, 1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_CPR_EXT_FREQ3, &ucData, 1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_CPR_EXT_FREQ4, &ucData, 1);
    //0x1D6 Disable CR CFO compensation,Enable Externel Frequency setting
    ucData = 0x02;
    ATD_SetReg(psDemodCtx, REG_ATD_CPR_CFO_CFG, &ucData, 1);
    //0x1B7 Disable FPC
    ucData = 0x6D;
    ATD_SetReg(psDemodCtx, REG_ATD_PJ_CFG, &ucData, 1);
    //0x199 Disable FIFO
    ucData = 0xC9;
    ATD_SetReg(psDemodCtx, REG_ATD_PFLL_CFG, &ucData, 1);
#endif
    
    if(MOD_TYPE_UNKNOWN == u1SubSysIdx)//scan mode
    {
        mcSHOW_DBG_MSG(("Enable AVS bypass for scan mode\n"));
        ATD_GetReg(psDemodCtx, REG_ATD_AVS_VOL, &ucData, 1);
        ucData &= 0xF7;
        ATD_SetReg(psDemodCtx, REG_ATD_AVS_VOL, &ucData, 1); 
    }
    
    //Reset ATD data path
    ucData = 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST1, &ucData, 1);
    ucData = 0xFF;
    ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST1, &ucData, 1);
    ucData = 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);
    ucData = 0xFF;
    ATD_SetReg(psDemodCtx, REG_ATD_CTL_RST2, &ucData, 1);
    //Fix PGA as minimum    value
    ucData = 0x00;
    ATD_SetReg(psDemodCtx, REG_ATD_SLD_PRA_30, &ucData, 1);
    return;
}
STATIC UINT8 ATD_ShowRegVal(UINT16 u2RegIdx,UINT8 u1StartFindIdx)
{
    UINT8 u1Len=sizeof(u2HiddenRegIdx)/sizeof(u2HiddenRegIdx[0]);

    if(u1StartFindIdx>=u1Len/2)
    {
        return 0;
    }
    else
    {
        if(u2RegIdx==u2HiddenRegIdx[u1StartFindIdx*2])
        {
            return u2HiddenRegIdx[u1StartFindIdx*2+1];//d110629_Haibo:return next jump index
        }
        else
        {
            return 0;
        }
    }    
}

void ATD_DumpRegister(ATD_CTX_T *psDemodCtx, UINT16 u2ModuleMask)
{
    UINT8 u1Len=sizeof(u1ArrRegisterMap)/sizeof(u1ArrRegisterMap[0]);
    UINT8  ii,jj,kk,ucData[80] = {0}; 
    UINT16 u2RegIdx=0;	
	UINT8 u1StartFindIdx=0;
	UINT8 u1RegOft;
	if(0 == (u2ModuleMask&0x7FFF))//info
	{
        for(ii=0;ii<(sizeof(sModule)/sizeof(sModule[0]));ii++)
        {
            mcSHOW_USER_MSG(("Module %s:0x%04X\n",sModule[ii],(UINT16)(1L<<ii))); 			 
        }
        mcSHOW_USER_MSG(("Module All:0x3FFF\n")); 	
        return;			 
	}
	for(kk=0;kk<14;kk++)
	{
        if(u2ModuleMask & (1L<<kk))
        {
            for(ii=0;ii<u1Len;ii+=3)
            {        	  
                if(u1ArrRegisterMap[ii] & (1L<<kk))
                {
                    ATD_GetReg(psDemodCtx,u1ArrRegisterMap[ii+1],ucData,u1ArrRegisterMap[ii+2]);
                    for(jj=1;(jj<=u1ArrRegisterMap[ii+2]) && jj<(sizeof(ucData)/sizeof(ucData[0]));)
                    {
                        if(u2ModuleMask&0x8000)//d110629_Haibo:Show Partial registers
                        {
                            u1RegOft=ATD_ShowRegVal(u2RegIdx,u1StartFindIdx);
                            mcSHOW_DBG_MSG(("u2RegIdx=%d,u1StartFindIdx=%d,u1RegOft=%d,jj=%d\n",u2RegIdx,u1StartFindIdx,u1RegOft,jj));
                            if(u1RegOft)
                            {
                                jj+=u1RegOft;
                                u2RegIdx+=u1RegOft;
                                u1StartFindIdx++;
                                continue;
                            }
                        }
                        mcPRINTF("0x%02X 0x%03X %s \n ",ucData[jj-1],u1ArrRegisterMap[ii+1]+ATD_REG_OFFSET+jj-1,sModule[kk]);
                        u2RegIdx++;
                        jj++;
                    }
                }
            }
        }	    
	}
}

