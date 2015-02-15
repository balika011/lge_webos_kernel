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
 * $RCSfile: digiamp_tas5711.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file adac_apogee.c
 *  Brief of file adac_apogee.c.
 *  Details of file adac_apogee.c (optional).
 */

#define AUD_EXTERN_FILE

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#include "x_lint.h"
LINT_EXT_HEADER_BEGIN

#include "digiamp_tas5711.h"
#include "sif_if.h"  // for i2c
#include "aud_if.h"
#include "aud_debug.h"
#include "drvcust_if.h"
#include "x_os.h"
#include "adac_if.h"
#include "x_gpio.h"

LINT_EXT_HEADER_END

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

static void _TAS5711_RegWrite(UINT8 u1Addr, UINT8 u1Data);
static UINT8 _TAS5711_RegRead(UINT8 u1Addr);
static void _TAS5711_Init(void);
static void _TAS5711_Mute(BOOL fgEnable);

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------

static AMP_FUNCTBL_T _rAmpTAS5711FunTbl =
{
    _TAS5711_Init,							//pfnInit
    _TAS5711_Mute,							//pfnMute
    NULL,//_TAS5711_DacFmtCfg,  			   //pfnSetDacInFmt;
    NULL,						 	     		   //pfnSetDacGain;
    _TAS5711_RegWrite, 					//pfnWrite;
    _TAS5711_RegRead					//pfnRead;
};

static AMP_T _rAmpTAS5711 =
{
    "TAS5711",				// szName[32];
    0,							// i2MaxGain
    0,						// i2MaxGain
    0,								// u1DacNum
    &_rAmpTAS5711FunTbl		// rCodecFuncTbl
};

//---------------------------------------------------------------------------
// Static functions
//---------------------------------------------------------------------------
static void ADAC_ResetAmp(VOID)
{
    UINT32 u4AmpResetGPIO;
    u4AmpResetGPIO = DRVCUST_OptGet(eAudioAmpResetGpioNum);
    if (u4AmpResetGPIO != 0xffffffff)
    {
		UINT32 u4AmpenableGPIO;
        UINT32 u4AmpEnablepolarity;
		    DRVCUST_OptQuery(eAudioAmpEnableGpioNum, &u4AmpenableGPIO);
        DRVCUST_OptQuery(eAudioAmpGpioPolarity, &u4AmpEnablepolarity);
        GPIO_SetOut((INT32)u4AmpenableGPIO, (bool)u4AmpEnablepolarity);   
        x_thread_delay(130);
        
        UNUSED(GPIO_SetOut((INT32)u4AmpResetGPIO,0));
        x_thread_delay(3);
        UNUSED(GPIO_SetOut((INT32)u4AmpResetGPIO, 1));
        x_thread_delay(10);
        UNUSED(GPIO_SetOut((INT32)u4AmpResetGPIO,0));
        x_thread_delay(20);
        UNUSED(GPIO_SetOut((INT32)u4AmpResetGPIO, 1));
        x_thread_delay(50);
    }
}

static void _TAS5711_RegWrite(UINT8 u1Addr, UINT8 u1Data)
{
    UINT32 u4WriteCount = 0;
    u4WriteCount = SIF_Write(SIF_CLK_DIV, TAS5711_ADDR, u1Addr, &u1Data, 1);
    LOG( 9,"I2C write (address,data) = (0x %x,0x %x)\n",u1Addr, u1Data);
    if (u4WriteCount < 1)
    {
        Printf("5711 write 1 byte fail=0x%2x, 0x%2x!\n", u1Addr, u1Data );
        LOG(5, "%c WRITE FAIL\n",_rAmpTAS5711.szName);
    }
}

static UINT8 _TAS5711_RegRead(UINT8 u1Addr)
{
    UINT32 u4ReadCount = 0;
    UINT8 u1Data;
    u4ReadCount = SIF_Read(SIF_CLK_DIV, TAS5711_ADDR, u1Addr, &u1Data, 1);
    LOG( 9,"I2C read (address,data) = (0x%x, 0x%x)\n",u1Addr, u1Data);
    if (u4ReadCount > 0)
    {
        return u1Data;
    }
    else
    {
        Printf("5711 read 1 byte fail!\n");
        LOG(5, "%c READ FAIL\n",_rAmpTAS5711.szName);
        return 0;
    }
}

static void _TAS5711_RegWriteN(UINT8 u1WordAddr, UINT8 *pu1Buf, UINT16 u2ByteCnt)
{
    UINT32 u4WriteCount = 0;
    u4WriteCount = SIF_Write(SIF_CLK_DIV, TAS5711_ADDR, u1WordAddr, pu1Buf, u2ByteCnt);
    //LOG( 9,"I2C write (address,data) = (0x %x,0x %x)\n",u1Addr, u1Data);
    if (u4WriteCount < 1)
	{
        Printf("5711 write n byte fail=0x%2x!\n", u1WordAddr );
        LOG(5, "%c WRITE FAIL\n",_rAmpTAS5711.szName);
    }
}

static void _TAS5711_Init(void)
{
    UINT8 u1Array[20];
    //Reset TAS5711
    ADAC_ResetAmp();
    // Make sure I2c is init
    SIF_Init();
    _TAS5711_RegWrite(TAS5711_TRIM, 0x00);
    x_thread_delay(60);

    // DRC energy time
	u1Array[0] = 0x00;
	u1Array[1] = 0x00;
	u1Array[2] = 0x06;
	u1Array[3] = 0xd3;	
	u1Array[4] = 0x00;
	u1Array[5] = 0x7f;
	u1Array[6] = 0xf9;
	u1Array[7] = 0x2b;
    //SIF_Write(SIF_CLK_DIV, TAS5711_ADDR, TAS5711_DRC_ENERGY, u1Array, 8);
    _TAS5711_RegWriteN(TAS5711_DRC_ENERGY, u1Array, 8);
    //DRC attack time(1ms)
    u1Array[0] = 0x00;
    u1Array[1] = 0x02;
    u1Array[2] = 0xa3;
    u1Array[3] = 0x9a;
    u1Array[4] = 0x00;
    u1Array[5] = 0x7d;
    u1Array[6] = 0x2f;
    u1Array[7] = 0xd2;
    //SIF_Write(SIF_CLK_DIV, TAS5711_ADDR, TAS5711_DRC_ATTACK, u1Array, 8);
    _TAS5711_RegWriteN(TAS5711_DRC_ATTACK, u1Array, 8);

    // DRC decay time(100ms)
    u1Array[0] = 0x00;
    u1Array[1] = 0x00;
    u1Array[2] = 0x06;
    u1Array[3] = 0xd3;
    u1Array[4] = 0x00;
    u1Array[5] = 0x7f;
    u1Array[6] = 0xf9;
    u1Array[7] = 0x2b;
    //SIF_Write(SIF_CLK_DIV, TAS5711_ADDR, TAS5711_DRC_DELAY, u1Array, 8);
    _TAS5711_RegWriteN(TAS5711_DRC_DELAY, u1Array, 8);

    //!DRC threshold 1(-14.7dB,9.1W,THD+N=6%,Gain=0dB,PVCC=12V)
//	u1Array[0] = 0xfd;
//	u1Array[1] = 0x77;
//	u1Array[2] = 0x8f;
//	u1Array[3] = 0x45;
    	u1Array[0] = 0xFD;
	u1Array[1] = 0x97;//0xAC;
	u1Array[2] = 0x73;//0xB5;
	u1Array[3] = 0x3d;//0xE3;
    //SIF_Write(SIF_CLK_DIV, TAS5711_ADDR, TAS5711_DRC_THRESHOLD, u1Array, 4);
    _TAS5711_RegWriteN(TAS5711_DRC_THRESHOLD, u1Array, 4);

    //DRC compression ratio£¨100£©
    u1Array[0] = 0x03;
    u1Array[1] = 0x81;
    u1Array[2] = 0x47;
    u1Array[3] = 0xaf;
    //SIF_Write(SIF_CLK_DIV, TAS5708_ADDR, TAS5711_DRC_COMPRESSION_RATIO, u1Array, 4);
    _TAS5711_RegWriteN(TAS5711_DRC_COMPRESSION_RATIO, u1Array, 4);

    //DRC offset£¨0£©
    u1Array[0] = 0x00;
    u1Array[1] = 0x08;
    u1Array[2] = 0x42;
    u1Array[3] = 0x10;
    //SIF_Write(SIF_CLK_DIV, TAS5708_ADDR, TAS5711_DRC_OFFSET, u1Array, 4);
    _TAS5711_RegWriteN(TAS5711_DRC_OFFSET, u1Array, 4);

	// DRC2 energy time
	u1Array[0] = 0x00;
	u1Array[1] = 0x00;
	u1Array[2] = 0x06;
	u1Array[3] = 0xd3;	
	u1Array[4] = 0x00;
	u1Array[5] = 0x7f;
	u1Array[6] = 0xf9;
	u1Array[7] = 0x2b;
    //SIF_Write(SIF_CLK_DIV, TAS5711_ADDR, TAS5711_DRC_ENERGY, u1Array, 8);
    _TAS5711_RegWriteN(TAS5711_DRC2_ENERGY, u1Array, 8);
    //DRC2 attack time(1ms)
    u1Array[0] = 0x00;
    u1Array[1] = 0x02;
    u1Array[2] = 0xa3;
    u1Array[3] = 0x9a;
    u1Array[4] = 0x00;
    u1Array[5] = 0x7d;
    u1Array[6] = 0x2f;
    u1Array[7] = 0xd2;
    //SIF_Write(SIF_CLK_DIV, TAS5711_ADDR, TAS5711_DRC_ATTACK, u1Array, 8);
    _TAS5711_RegWriteN(TAS5711_DRC2_ATTACK, u1Array, 8);

    // DRC2 decay time(100ms)
    u1Array[0] = 0x00;
    u1Array[1] = 0x00;
    u1Array[2] = 0x06;
    u1Array[3] = 0xd3;
    u1Array[4] = 0x00;
    u1Array[5] = 0x7f;
    u1Array[6] = 0xf9;
    u1Array[7] = 0x2b;
    //SIF_Write(SIF_CLK_DIV, TAS5711_ADDR, TAS5711_DRC_DELAY, u1Array, 8);
    _TAS5711_RegWriteN(TAS5711_DRC2_DELAY, u1Array, 8);

    //!DRC2 threshold 1(-14.7dB,9.1W,THD+N=6%,Gain=0dB,PVCC=12V)
//	u1Array[0] = 0xfd;
//	u1Array[1] = 0x77;
//	u1Array[2] = 0x8f;
//	u1Array[3] = 0x45;
    	u1Array[0] = 0xFD;
	u1Array[1] = 0x97;//0xAC;
	u1Array[2] = 0x73;//0xB5;
	u1Array[3] = 0x3d;//0xE3;
    //SIF_Write(SIF_CLK_DIV, TAS5711_ADDR, TAS5711_DRC_THRESHOLD, u1Array, 4);
    _TAS5711_RegWriteN(TAS5711_DRC2_THRESHOLD, u1Array, 4);

    //DRC2 compression ratio£¨100£©
    u1Array[0] = 0x03;
    u1Array[1] = 0x81;
    u1Array[2] = 0x47;
    u1Array[3] = 0xaf;
    //SIF_Write(SIF_CLK_DIV, TAS5708_ADDR, TAS5711_DRC_COMPRESSION_RATIO, u1Array, 4);
    _TAS5711_RegWriteN(TAS5711_DRC2_COMPRESSION_RATIO, u1Array, 4);

    //DRC2 offset£¨0£©
    u1Array[0] = 0x00;
    u1Array[1] = 0x08;
    u1Array[2] = 0x42;
    u1Array[3] = 0x10;
    //SIF_Write(SIF_CLK_DIV, TAS5708_ADDR, TAS5711_DRC_OFFSET, u1Array, 4);
    _TAS5711_RegWriteN(TAS5711_DRC2_OFFSET, u1Array, 4);

    //DRC control
    u1Array[0] = 0x00;
    u1Array[1] = 0x00;
    u1Array[2] = 0x00;
    u1Array[3] = 0x02;//0x00;//20101210 wangxinzhi close drc  0x02;
    //SIF_Write(SIF_CLK_DIV, TAS5711_ADDR, TAS5708_DRC_CONTROL, u1Array, 4);
    _TAS5711_RegWriteN(TAS5711_DRC_CONTROL, u1Array, 4);

	//subch Biquad1 Coeff
    u1Array[0] = 0x00;
    u1Array[1] = 0x00;
    u1Array[2] = 0x05;
    u1Array[3] = 0x83;
	u1Array[4] = 0x00;
	u1Array[5] = 0x00;
	u1Array[6] = 0x0B;
	u1Array[7] = 0x06;
	u1Array[8] = 0x00;
	u1Array[9] = 0x00;
	u1Array[10] = 0x05;
	u1Array[11] = 0x83;
	u1Array[12] = 0x00;
	u1Array[13] = 0xFB;
	u1Array[14] = 0x42;
	u1Array[15] = 0xC1;
	u1Array[16] = 0x0F;
	u1Array[17] = 0x84;
	u1Array[18] = 0xA7;
	u1Array[19] = 0x33;
	
    _TAS5711_RegWriteN(TAS5711_SUBCH_BIQUAD1_COEFF, u1Array, 20);

    //subch Biquad2 Coeff
    u1Array[0] = 0x00;
    u1Array[1] = 0x7F;
    u1Array[2] = 0x77;
    u1Array[3] = 0xC7;
	u1Array[4] = 0x0F;
	u1Array[5] = 0x01;
	u1Array[6] = 0x10;
	u1Array[7] = 0x72;
	u1Array[8] = 0x00;
	u1Array[9] = 0x7F;
	u1Array[10] = 0x77;
	u1Array[11] = 0xC7;
	u1Array[12] = 0x00;
	u1Array[13] = 0xFE;
	u1Array[14] = 0xEE;
	u1Array[15] = 0xFD;
	u1Array[16] = 0x0F;
	u1Array[17] = 0x81;
	u1Array[18] = 0x0F;
	u1Array[19] = 0xE1;
	
    _TAS5711_RegWriteN(TAS5711_SUBCH_BIQUAD2_COEFF, u1Array, 20);

    //Bank switch control £¨no automatic bank switching)
    u1Array[0] = 0x0f;
    u1Array[1] = 0x70;
    u1Array[2] = 0x80;
    u1Array[3] = 0x00;
    //SIF_Write(SIF_CLK_DIV, TAS5711_ADDR, TAS5711_BANK_SWITCH, u1Array, 4);
    _TAS5711_RegWriteN(TAS5711_BANK_SWITCH, u1Array, 4);

	//Input Mux Register(BD mode,opt.)
	u1Array[0] = 0x00;
	u1Array[1] = 0x89;
	u1Array[2] = 0x77;
	u1Array[3] = 0x72;
	//SIF_Write(SIF_CLK_DIV, TAS5711_ADDR, TAS5708_INPUT_MUX, u1Array, 4);
	_TAS5711_RegWriteN(TAS5711_INPUT_MUX, u1Array, 4);

    //ch4 source selectr((R+L)/2)
	u1Array[0] = 0x00;
	u1Array[1] = 0x00;
	u1Array[2] = 0x42;
	u1Array[3] = 0x03;
	//SIF_Write(SIF_CLK_DIV, TAS5711_ADDR, TAS5708_INPUT_MUX, u1Array, 4);
	_TAS5711_RegWriteN(TAS5711_CH4_SOURCE_SELECT, u1Array, 4);

	// PWM Output MUX Register (Note: Writes to this register affect Inter-Channel Delay)
	u1Array[0] = 0x01;
	u1Array[1] = 0x02;//channel2->OUT_A, /channel2->OUT_B   
	u1Array[2] = 0x13;//channel1->OUT_C, /channel1->OUT_D    ////A+,B-,c-,D+  //hjj_1103
	u1Array[3] = 0x45;
	//SIF_Write(SIF_CLK_DIV, TAS5711_ADDR, TAS5711_PWM_OUT_MUX, u1Array, 4);
	_TAS5711_RegWriteN(TAS5711_PWM_OUT_MUX, u1Array, 4);

	//ch1 output mixer:mixer2=0,mixer1=0,mixer0=1
	u1Array[0] = 0x00;
	u1Array[1] = 0x00;
	u1Array[2] = 0x00;
	u1Array[3] = 0x00;	
	u1Array[4] = 0x00;
	u1Array[5] = 0x00;
	u1Array[6] = 0x00;
	u1Array[7] = 0x00;
	u1Array[8] = 0x00;
	u1Array[9] = 0x80;
	u1Array[10] = 0x00;
	u1Array[11] = 0x00;
	_TAS5711_RegWriteN(TAS5711_CH1_OUPUT_MIXER, u1Array, 12);

	//ch2 output mixer:mixer2=0,mixer1=0,mixer0=1
	u1Array[0] = 0x00;
	u1Array[1] = 0x00;
	u1Array[2] = 0x00;
	u1Array[3] = 0x00;	
	u1Array[4] = 0x00;
	u1Array[5] = 0x00;
	u1Array[6] = 0x00;
	u1Array[7] = 0x00;
	u1Array[8] = 0x00;
	u1Array[9] = 0x80;
	u1Array[10] = 0x00;
	u1Array[11] = 0x00;
	_TAS5711_RegWriteN(TAS5711_CH2_OUPUT_MIXER, u1Array, 12);

    //ch4 output mixer:mixer2=0,mixer1=0,mixer0=1
	u1Array[0] = 0x00;
	u1Array[1] = 0x00;
	u1Array[2] = 0x00;
	u1Array[3] = 0x00;	
	u1Array[4] = 0x00;
	u1Array[5] = 0x80;
	u1Array[6] = 0x00;
	u1Array[7] = 0x00;
	_TAS5711_RegWriteN(TAS5711_CH4_OUPUT_MIXER, u1Array, 8);

	_TAS5711_RegWrite(TAS5711_MASTER_VOLUME, 0x1a);
	_TAS5711_RegWrite(TAS5711_CH1_VOLUME, 0x30);
	_TAS5711_RegWrite(TAS5711_CH2_VOLUME, 0x30);
	_TAS5711_RegWrite(TAS5711_CH3_VOLUME, 0x30);
	_TAS5711_RegWrite(TAS5711_VOLUME_CONFIG, 0x91);
	_TAS5711_RegWrite(TAS5711_SOFT_MUTE, 0x00);
	_TAS5711_RegWrite(TAS5711_SYS_CONTROL1, 0xa0);
	_TAS5711_RegWrite(TAS5711_SERIAL_DATA, 0x05);
	_TAS5711_RegWrite(TAS5711_SYS_CONTROL2, 0x08);
	x_thread_delay(20);
	//vKrDelay1ms(20);
	_TAS5711_RegWrite(TAS5711_VOLUME_CONFIG, 0xd1);
	_TAS5711_RegWrite(TAS5711_MODULATION_LIMIT, 0x02);
	_TAS5711_RegWrite(TAS5711_START_STOP_PERIOD, 0x95);
	_TAS5711_RegWrite(TAS5711_BKND_ERR, 0x02);
	_TAS5711_RegWrite(TAS5711_IC_DELAY_CH1, 0xb8);
	_TAS5711_RegWrite(TAS5711_IC_DELAY_CH2, 0x60);
	_TAS5711_RegWrite(TAS5711_IC_DELAY_CH3, 0xa0);
	_TAS5711_RegWrite(TAS5711_IC_DELAY_CH4, 0x48);

}

static void _TAS5711_Mute(BOOL fgEnable)
{
    Printf("TEST: _TAS5711_Mute......%d\n",fgEnable);
    
    if (fgEnable)
    {
        _TAS5711_RegWrite(TAS5711_SOFT_MUTE, 0x03);
    }
    else
    {
        _TAS5711_RegWrite(TAS5711_SOFT_MUTE, 0x00);
    }
}

AMP_T* _AMP_GetAmpTAS5711FunTbl(void)
{
    return &_rAmpTAS5711;
}

/*
void test()
{
    //powr on sequence 1
    vHWADDACRegWriteByte(STA232W_CONFF, 0x5C);
    vHWADDACRegWriteByte(STA232W_MASMUTE, 0x01);
    //initializing
    vHWADDACRegWriteByte(STA232W_CONFA, 0x63); //256fps
    vHWADDACRegWriteByte(STA232W_CONFD, 0x10);
    //vHWADDACRegWrite(STA232W_CONFF, 0xDC); --> overlap, disable
    vHWADDACRegWriteByte(STA232W_MASTER,0x00);
    vHWADDACRegWriteByte(STA232W_MASVOL1,0x44);
    vHWADDACRegWriteByte(STA232W_MASVOL2,0x44);
    vHWADDACRegWriteByte(STA232W_AUTOMODE,0x80);
    vHWADDACRegWriteByte(STA232W_CH1CONFIG,0x00);
    vHWADDACRegWriteByte(STA232W_CH2CONFIG,0x40);
    vHWADDACRegWriteByte(STA232W_LIMITER,0x7F);
    //POWER ON SEQUENCE 2
    vHWADDACRegWriteByte(STA232W_CONFF,0xDC);
    vHWADDACRegWriteByte(STA232W_MASMUTE, 0x00);
}
*/


