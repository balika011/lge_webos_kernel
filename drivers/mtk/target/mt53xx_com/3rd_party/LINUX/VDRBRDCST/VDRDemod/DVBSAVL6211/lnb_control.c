#include "u_tuner.h"
#include "x_os.h"
#include "x_stl_lib.h"//StrToInt function
#include "tuner_if.h"
#include "TDIS_header.h"
#include "pd_dvbs_glue.h"
#include "tunerDebug.h"
#include "i2c_api.h"
#include "tuner_interface_if.h"
#include "pd_dvbs_if.h"
#include "x_gpio.h"
#include "vdr_PD_Def.h"
#include "x_rm.h"
#include "lnb_control.h"
#include "IDiseqc.h"

//#define LNB_CHIP_8296 1
#define LNB_CHIP_8293 1
//#define LNB_CHIP_23 1
//#define LNB_CHIP_RT5007 1
#define LNB_CHIP_ADDR 0x10
#ifdef LNB_CHIP_8296
static VOID A8296_Control(UCHAR Temp)
{
	 UCHAR uiData = 0x10;

 	 ucI2cReadOnly(LNB_CHIP_ADDR,&uiData,1);
	 switch(Temp)
	 	{
	 	case 0:
			uiData = 0x00;//power off
			break;
		case 1:
			uiData = 0x50; //14V
			break;
		case 2:
			uiData = 0x90;//18.667V
			break;
		case 3:
			uiData = 0x70; //14.333V
			break;
		case 4:
			uiData = 0xb0;//19v
			break;
		default:
			mcDBG_MSG_INFO(("[A8296_Control]  not support!\n"));
			return;
	 	}

	 ucI2cWriteOnly(LNB_CHIP_ADDR,&uiData,1);

}
#endif

#ifdef LNB_CHIP_8293
static VOID A8293_Control(UCHAR Temp)
{
	 UCHAR uiData = 0x10;

	 //ucI2cRead(A9293_addr,uiData,0,0);
 	 ucI2cReadOnly(LNB_CHIP_ADDR,&uiData,1);
	 switch(Temp)
	 	{
	 	case 0:
			uiData = 0x10;
			break;
		case 1:
			uiData = 0x34; //14V
			break;
		case 2:
			uiData = 0x38;//18V
			break;
		case 3:
			uiData = 0x37; //15V
			break;
		case 4:
			uiData = 0x3b;//19v
			break;
		default:
			mcDBG_MSG_INFO(("[A8293_Control]  not support!\n"));
			return;
	 	}

	 ucI2cWriteOnly(LNB_CHIP_ADDR,&uiData,1);

	 uiData = 0x82;
	ucI2cWriteOnly(LNB_CHIP_ADDR,&uiData,1);

}
#endif
#ifdef LNB_CHIP_23
#define LNBH23_SLAVE_ADDR  (0x0a<<1)	//A8293:0x08,LNBH23:0x0A //000 1010 (without W/R bit) //0x16  //0001 01 1(slave) 0(w)

#define BIT_wPCL		0x80
#define BIT_wTTX		0x40
#define BIT_wTEN		0x20
#define BIT_wLLC		0x10
#define BIT_wVSEL		0x08
#define BIT_wEN			0x04
#define BIT_wITEST		0x02
#define BIT_wAUX		0x01

#define BIT_rIMON		0x80
#define BIT_rVMON		0x40
#define BIT_rTMON		0x20
#define BIT_rLLC		0x10
#define BIT_rVSEL		0x08
#define BIT_rEN			0x04
#define BIT_rOTF		0x02
#define BIT_rOLF		0x01


#define DATA_LNBPOWER_OFF 	0x00
#define DATA_13V_OUT		BIT_wEN
#define DATA_18V_OUT		(BIT_wVSEL| BIT_wEN)
#define DATA_14V_OUT		(BIT_wLLC | BIT_wEN)
#define DATA_19V_OUT		(BIT_wLLC | BIT_wVSEL | BIT_wEN)
#define DATA_22V_OUT		(BIT_wEN  | BIT_wAUX)

#define DATA_22K_OFF		BIT_wEN
#define DATA_22K_ON			(BIT_wTTX | BIT_wTEN | BIT_wEN)

#define DATA_VOUT_ON_TONE_OFF (DATA_LNBPOWER_OFF | BIT_wEN)
#define DATA_VOUT_ON_TONE_ON  (BIT_wTTX | BIT_wEN)

#define DATA_PCLS 			(DATA_LNBPOWER_OFF | BIT_wEN)
#define DATA_SCLS 			(BIT_wPCL | BIT_wEN)

static void LNBH23_SetLNBPower(UCHAR LNB_Type)
{

       UCHAR u8Data = DATA_13V_OUT;
	UCHAR u8Data2 = 0x00;
     mcDBG_MSG_INFO(("[LNBH23_SetLNBPower] LNB_Type:%d\n",LNB_Type));
//	 ucI2cReadOnly(LNBH23_SLAVE_ADDR,&u8Data2,1);
//	 mcDBG_MSG_INFO(("[LNBH23_SetLNBPower] u8Data2:%d\n",u8Data2));
      switch(LNB_Type)
      {
            case 0:
                    u8Data= DATA_LNBPOWER_OFF;
                    break;
            case 1:
                    u8Data = DATA_13V_OUT;
                    break;
            case 2:
		      u8Data = DATA_18V_OUT;
                    break;
            case 3:
                    u8Data = DATA_14V_OUT;
                    break;
            case 4:
                    u8Data = DATA_19V_OUT;
                    break;
      }
      u8Data |=   BIT_wTTX;//enable 22K tone
      u8Data &= ~BIT_wTEN;	  
      ucI2cWriteOnly(LNB_CHIP_ADDR,&u8Data,1);

      return;
}
#endif

#ifdef LNB_CHIP_RT5007 



#define CONTROL_REGISTER1_DISENABLE(x) x&0xF7  //1111 0111

//Define the status bit
#define LNBC_A8293_STATUS_DIS 0x01
#define LNBC_A8293_STATUS_OCP 0x04
#define LNBC_A8293_STATUS_PNG 0x10
#define LNBC_A8293_STATUS_TSD 0x40
#define LNBC_A8293_STATUS_VUV 0x80


//Output Voltage Amplitude Selection
enum LNBC_OUT_VOLTAGE
{
	LNBC_OUT_VOLTAGE_12_709 = 0,	    
	LNBC_OUT_VOLTAGE_14,
	LNBC_OUT_VOLTAGE_18,
	LNBC_OUT_VOLTAGE_1433,
	LNBC_OUT_VOLTAGE_19
};


enum LNBC_OUT_TONE
{
	LNBC_OUT_TONE_OFF = 0,	    
	LNBC_OUT_TONE_INTERNAL,
	LNBC_OUT_TONE_EXTERNAL
};


enum LNBC_OUT_VOLTAGE eVoltageType;

//static const UCHAR LNBCOutV[5] = {0x30, 0x31,0x34, 0x38, 0x3B};// enable LNB OUT put
static const UCHAR LNBCOutV[5] = {0x08, 0x0A,0x0C, 0x0B, 0x0D};// enable LNB OUT put

//0:Gate off the tone out
//1:Use internal tone.
//2:Use external tone,it just provides LNB voltage, the DiSEqC tone is provided by Ruby
static const UCHAR LNBCToneMode[3] = {0x80, 0x83, 0x82};

static void LNB_5007SetTone(enum LNBC_OUT_TONE eToneMode)
{
	UCHAR  LNBCRgister2; // second register
	UCHAR uDataIndex = (UCHAR) eToneMode;

	if(uDataIndex > 2) //check in parameter
	{
		return;
	}	
	LNBCRgister2 = LNBCToneMode[uDataIndex];
	ucI2cWriteOnly(LNB_CHIP_ADDR, &LNBCRgister2, 1);

}
UINT8 fgLNBFisrtControl=TRUE;
static void LNBC_RT5007LNBOutPut(enum LNBC_OUT_VOLTAGE eVoltageType)
{
	UCHAR Rt5007_Reg[2];
	UINT8 u1CheckStatus,ii,jj;
	//UCHAR LNBCRgister1; // first register
	UCHAR uDataIndex = (UCHAR) eVoltageType;
													
	if(uDataIndex>4) //check in parameter
	{
		return;
	}
	if(fgLNBFisrtControl)//first time set lnb power, RT5007 need to set 2 times
	{
		jj=2;
		fgLNBFisrtControl=FALSE;
	}
	else
	{
		jj=1;
	}

	Rt5007_Reg[0]=0x00;
	Rt5007_Reg[1] = LNBCOutV[uDataIndex];
	while(jj--)
	{
		for(ii=1;ii<=3;ii++)
		{
			 x_thread_delay(20);
			 ucI2cWriteOnly(LNB_CHIP_ADDR, Rt5007_Reg, 2);
			 
			 x_thread_delay(10);
			 
			 ucI2cWriteOnly(LNB_CHIP_ADDR, Rt5007_Reg, 1);//select the register 0x00
			 ucI2cReadOnly(LNB_CHIP_ADDR,&u1CheckStatus,1);//check the 0x00 status
			 if(u1CheckStatus==0x00)
			 {
			 
			  	mcDBG_MSG_L2(("[RT5007_Control] Set LNB power succesfully at %d/3!\n",ii));
		        break;
			 }
			 else
		 	 {
				 mcDBG_MSG_INFO(("[RT5007_Control] Set LNB power failed at %d/3!\n",ii));

		 	 }
		}
 	}
	 
}
static void LNBOutPut_Disable(void) 
{
	UCHAR Rt5007_Reg[2];
	Rt5007_Reg[0]=0x00;
	Rt5007_Reg[1] = CONTROL_REGISTER1_DISENABLE(LNBCOutV[0]);
	ucI2cWriteOnly(LNB_CHIP_ADDR, Rt5007_Reg, 2);
}

#endif
static VOID LNB_Power_Disable(VOID)
{
	mcDBG_MSG_INFO(("[LNB_Power_Disable] is call!\n"));
#ifdef LNB_CHIP_8296	
	A8296_Control(0);
#endif
#ifdef LNB_CHIP_8293	
	A8293_Control(0);
#endif
#ifdef LNB_CHIP_23	
	LNBH23_SetLNBPower(0);
#endif
#ifdef LNB_CHIP_RT5007
	LNBOutPut_Disable();
#endif


}
static VOID LNB_Power_Enable(VOID)
{
	mcDBG_MSG_INFO(("[LNB_Power_Enable] is call!\n"));
}
static VOID LNB_Power_14V(VOID)
{
	mcDBG_MSG_INFO(("[LNB_Power_14V] is call!\n"));
#ifdef LNB_CHIP_8296	
		A8296_Control(1);
#endif
#ifdef LNB_CHIP_8293	
		A8293_Control(1);
#endif
#ifdef LNB_CHIP_23	
		LNBH23_SetLNBPower(1);
#endif
#ifdef LNB_CHIP_RT5007
		LNBC_RT5007LNBOutPut(LNBC_OUT_VOLTAGE_14);
#endif

}
static VOID LNB_Power_18V(VOID)
{
	mcDBG_MSG_INFO(("[LNB_Power_18V] is call!\n"));
#ifdef LNB_CHIP_8296	
		A8296_Control(2);
#endif
#ifdef LNB_CHIP_8293	
		A8293_Control(2);
#endif
#ifdef LNB_CHIP_23	
		LNBH23_SetLNBPower(2);
#endif
#ifdef LNB_CHIP_RT5007
		LNBC_RT5007LNBOutPut(LNBC_OUT_VOLTAGE_18);
#endif


}
static VOID LNB_Power_15V(VOID)
{
	mcDBG_MSG_INFO(("[LNB_Power_15V] is call!\n"));
#ifdef LNB_CHIP_8296	
		A8296_Control(3);
#endif
#ifdef LNB_CHIP_8293	
		A8293_Control(3);
#endif
#ifdef LNB_CHIP_23	
		LNBH23_SetLNBPower(3);
#endif
#ifdef LNB_CHIP_RT5007
		LNBC_RT5007LNBOutPut(LNBC_OUT_VOLTAGE_1433);
#endif


}
static VOID LNB_Power_19V(VOID)
{
	mcDBG_MSG_INFO(("[LNB_Power_19V] is call!\n"));
#ifdef LNB_CHIP_8296	
		A8296_Control(4);
#endif
#ifdef LNB_CHIP_8293	
		A8293_Control(4);
#endif
#ifdef LNB_CHIP_23	
		LNBH23_SetLNBPower(4);
#endif
#ifdef LNB_CHIP_RT5007
		LNBC_RT5007LNBOutPut(LNBC_OUT_VOLTAGE_19);
#endif


}


VOID AVL_DVBS2_LNB_Power_OnOff(UCHAR nIsOn)
{

	if(nIsOn==0)
		{
			LNB_Power_Disable();
		}
	else
		{
			LNB_Power_Enable();
		}
}
VOID AVL_DVBS2_LNB_Control_14or18V(UCHAR nIs14)
{
	LNB_Power_Enable();
	
	if(nIs14)
		{
			LNB_Power_14V();
		}
	else
		{
			LNB_Power_18V();
		}
}
VOID AVL_DVBS2_LNB_Control_15or19V(UCHAR nIs15)
{
	if(nIs15)
	{
		LNB_Power_15V();
	}
	else
	{
		LNB_Power_19V();
	}
}
///return 0 --AVL_DVBSx_DWM_Normal
///return 1---AVL_DVBSx_DWM_Envelope

UCHAR AVL_Get_22k_Configration_by_LNBType(VOID)
{
enum AVL_DVBSx_Diseqc_WaveFormMode flag=AVL_DVBSx_DWM_Envelope;
#ifdef LNB_CHIP_8296	
			return flag;
#endif
#ifdef LNB_CHIP_8293	
			flag =AVL_DVBSx_DWM_Normal;
            return flag;
#endif
#ifdef LNB_CHIP_23	
			 return flag;
#endif
#ifdef LNB_CHIP_RT5007
			 LNB_5007SetTone(LNBC_OUT_TONE_EXTERNAL);
			 return flag;
#endif

}


