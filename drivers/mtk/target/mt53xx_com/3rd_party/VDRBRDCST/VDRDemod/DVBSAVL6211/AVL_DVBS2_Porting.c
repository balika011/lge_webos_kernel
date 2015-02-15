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
//////////////////////////availink define....
#include "IBSP.h"
#include "avl_dvbsx.h"
#include "IBase.h"
#include "IRx.h"
#include "ITuner.h"
#include "II2C.h"
#include "IDiseqc.h"
#include "IBlindScan.h"
#include "pd_dvbs_glue.h"
#include "AVL_DVBS2_Porting.h"
#include "lnb_control.h"

//#define BS2S7HZ7306 
#define BS2AV2011
#if defined(BS2AV2011)
#include "ExtAV2011.h"
#elif defined(BS2S7HZ7306)
#include "ExtSharpBS2S7HZ6306.h"
#else
#include "ExtSharpBS2S7VZ7801.h"
#endif
#define Unicable_IQ_Spectrum_Invert 

//#define IX2610VA_slave_address     0xC0            //The Tuner slave address. It is the write address of the tuner device. i.e. it is 8 bits address and the last bit is the R/W flag which is zero. Although this data structure support 10 bits I2C address. The AVL chip not supports it. 
#define IX2610VA_I2Cbus_clock      200             //The clock speed of the tuner dedicated I2C bus, in a unit of kHz.
//#define IX2610VA_LPF               340             //The LPF of the tuner,in a unit of 100kHz.
#define Diseqc_Tone_Frequency   22              // The Diseqc bus speed in the unit of kHz. Normally, it should be 22kHz. 
extern AVL_uchar ucPatchData [];			 //Defined in AVL6211_patch.dat.cpp.
#define Sub_abs(a,b) ((a>=b)?(a-b):(b-a))

static AVL_uint16 g_nWaitLoop_Max=0;
struct Signal_Level
{	
	AVL_uint16 SignalLevel;	
	AVL_int16 SignalDBM;
};
static struct AVL_DVBSx_Channel Avl6211_channels[256];
static AVL_uchar g_u16Total_tp = 0;
static 	struct AVL_DVBSx_Channel Channellist_temp[64];


 enum AVL_Demod_ReferenceClock_Select_t
{
   Ref_clock_4MHz=0,
   Ref_clock_4_5MHz=1,
   Ref_clock_10MHz=2,
   Ref_clock_16MHz=3,
   Ref_clock_27MHz=4,
   Ref_clock_Enhance_4MHz=5,
   Ref_clock_Enhance_4_5MHz=6,
   Ref_clock_Enhance_10MHz=7,
   Ref_clock_Enhance_16MHz=8,
   Ref_clock_Enhance_27MHz=9,   
};
struct AVL_Demod_Tuner_Configuration_t
{
	AVL_char nChannelId;
	AVL_uint16 nDemodAddress;
	enum AVL_Demod_ReferenceClock_Select_t   nDemodReferenceClk;
	enum AVL_DVBSx_MpegMode nTSOutPutMode;
	enum AVL_DVBSx_MpegClockPolarity nTSClockPolarity;
	enum AVL_DVBSx_MpegFormat nTSFormat;
	enum AVL_DVBSx_MpegSerialPin nSerDataPin;
	
	AVL_uint16 nTunerAddress;
	AVL_uchar nTunerLPF_AutoSetFlag;
	AVL_uint16  nTunerMaxLPF_100Khz;
	enum AVL_DVBSx_RfagcPola nTunerRFAGC;	
	enum AVL_DVBSx_SpectrumPolarity nTuner_IQ_SpectrumMode;

	AVL_DVBSx_ErrorCode (* m_pInitializeFunc)(struct AVL_Tuner *);	 	///< A pointer to the tuner initialization function.
	AVL_DVBSx_ErrorCode (* m_pLockFunc)(struct AVL_Tuner *);			///< A pointer to the tuner Lock function. 
	AVL_DVBSx_ErrorCode (* m_pGetLockStatusFunc)(struct AVL_Tuner *); 	///< A pointer to the tuner GetLockStatus function.     
};


/*g_DemodTuner_Config Array index defined according to the User HW*/
static AVL_uchar g_nDemodTunerArrayIndex=0;

struct AVL_Demod_Tuner_Configuration_t g_DemodTuner_Config[1]=
{
{
0,
AVL_DVBSx_SA_0,
Ref_clock_27MHz,
AVL_DVBSx_MPM_Parallel,
AVL_DVBSx_MPCP_Rising,
AVL_DVBSx_MPF_TSP,
AVL_DVBSx_MPSP_DATA0,

0xc0,
1,
440,
AVL_DVBSx_RA_Invert,
AVL_DVBSx_Spectrum_Invert,
#if defined(BS2AV2011)
&AVL_DVBSx_ExtAV2011_Initialize,
&AVL_DVBSx_ExtAV2011_Lock,
&AVL_DVBSx_ExtAV2011_GetLockStatus,
#elif defined(BS2S7HZ7306)
&ExtSharpBS2S7HZ6306_Initialize,
&ExtSharpBS2S7HZ6306_Lock,
&ExtSharpBS2S7HZ6306_GetLockStatus,
#else
&ExtSharpBS2S7VZ7801_Initialize,
&ExtSharpBS2S7VZ7801_Lock,
&ExtSharpBS2S7VZ7801_GetLockStatus,
#endif
}
};
#ifdef BS2AV2011
struct Signal_Level  SignalLevel [47]=
{
	{8285,	-92},{10224,	-90},{12538,	-88},{14890,	-86},{17343,	-84},{19767,	-82},{22178,	-80},{24618,	-78},{27006,	-76},{29106,	-74},
	{30853,	-72},{32289,	-70},{33577,	-68},{34625,	-66},{35632,	-64},{36552,	-62},{37467,	-60},{38520,	-58},{39643,	-56},{40972,	-54},
	{42351,	-52},{43659,	-50},{44812,	-48},{45811,	-46},{46703,	-44},{47501,	-42},{48331,	-40},{49116,	-38},{49894,	-36},{50684,	-34},
	{51543,	-32},{52442,	-30},{53407,	-28},{54314,	-26},{55208,	-24},{56000,	-22},{56789,	-20},{57544,	-18},{58253,	-16},{58959,	-14},
	{59657,	-12},{60404,	-10},{61181,	-8},{62008,	-6},{63032,	-4},{65483,	-2},{65535,	-1}
};
#endif
#ifdef BS2S7HZ7306
struct Signal_Level  SignalLevel [47]=
{
	{8285,	-92},{10224,	-90},{12538,	-88},{14890,	-86},{17343,	-84},{19767,	-82},{22178,	-80},{24618,	-78},{27006,	-76},{29106,	-74},
	{30853,	-72},{32289,	-70},{33577,	-68},{34625,	-66},{35632,	-64},{36552,	-62},{37467,	-60},{38520,	-58},{39643,	-56},{40972,	-54},
	{42351,	-52},{43659,	-50},{44812,	-48},{45811,	-46},{46703,	-44},{47501,	-42},{48331,	-40},{49116,	-38},{49894,	-36},{50684,	-34},
	{51543,	-32},{52442,	-30},{53407,	-28},{54314,	-26},{55208,	-24},{56000,	-22},{56789,	-20},{57544,	-18},{58253,	-16},{58959,	-14},
	{59657,	-12},{60404,	-10},{61181,	-8},{62008,	-6},{63032,	-4},{65483,	-2},{65535,	-1}
};
#endif

#ifdef BS2S7HZ7801
struct Signal_Level  AGC_LUT [90]=
{
    {63688,0},  {62626,-1}, {61840,-2}, {61175,-3}, {60626,-4}, {60120,-5}, {59647,-6}, {59187,-7}, {58741,-8}, {58293,-9},
    {57822,-10},{57387,-11},{56913,-12},{56491,-13},{55755,-14},{55266,-15},{54765,-16},{54221,-17},{53710,-18},{53244,-19},
    {52625,-20},{52043,-21},{51468,-22},{50904,-23},{50331,-24},{49772,-25},{49260,-26},{48730,-27},{48285,-28},{47804,-29},
    {47333,-30},{46880,-31},{46460,-32},{46000,-33},{45539,-34},{45066,-35},{44621,-36},{44107,-37},{43611,-38},{43082,-39},
    {42512,-40},{41947,-41},{41284,-42},{40531,-43},{39813,-44},{38978,-45},{38153,-46},{37294,-47},{36498,-48},{35714,-49},
    {35010,-50},{34432,-51},{33814,-52},{33315,-53},{32989,-54},{32504,-55},{32039,-56},{31608,-57},{31141,-58},{30675,-59},
    {30215,-60},{29711,-61},{29218,-62},{28688,-63},{28183,-64},{27593,-65},{26978,-66},{26344,-67},{25680,-68},{24988,-69},
    {24121,-70},{23285,-71},{22460,-72},{21496,-73},{20495,-74},{19320,-75},{18132,-76},{16926,-77},{15564,-78},{14398,-79},
    {12875,-80},{11913,-81},{10514,-82},{ 9070,-83},{ 7588,-84},{ 6044,-85},{ 4613,-86},{ 3177,-87},{ 1614,-88},{  123,-89}
};
#endif
static struct AVL_DVBSx_Chip  AVL_DVBSxChip;
static struct AVL_Tuner Tuner;	

static void Unicable_Cmmond_ChannelChange_Config(UINT8 *command,UINT8 SaTCR_num,UINT8 LNB_num,UINT16 freq_MHZ,UINT16 LNB_MHZ)
{
	UINT16 freq_T=0;
	command[0] = 0xe0;
	command[1] = 0x10;
	command[2] = 0x5A;
	command[3] = ((SaTCR_num << 5) & 0xe0);	 
	command[3] |= ((LNB_num<< 2) & 0x1c);	
	 mcDBG_MSG_L1(("command[3] =0x%x\n",command[3] ));
	freq_T = ((freq_MHZ + LNB_MHZ) / 4) - 350;
       mcDBG_MSG_L1(("freq_T=0x%x\n",freq_T));
	command[3] |= ((freq_T >> 8) & 0x0003);	//T[9:8]
	
	command[4] = (freq_T & 0x00ff);	//T[7:0] 
	mcDBG_MSG_L1(("command[3]=0x%x\n",command[3]));
	mcDBG_MSG_L1(("command[4]=0x%x\n",command[4]));
	return;
}
static AVL_DVBSx_ErrorCode  AV6211_TS_Parallel_Mode(VOID)
{
	AVL_DVBSx_ErrorCode r=AVL_DVBSx_EC_OK;
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;	
	struct AVL_DVBSx_MpegInfo sMpegMode;
	struct AVL_Demod_Tuner_Configuration_t *pConfig=&g_DemodTuner_Config[g_nDemodTunerArrayIndex];
	sMpegMode.m_MpegFormat =pConfig->nTSFormat;// AVL_DVBSx_MPF_TSP;
	sMpegMode.m_MpegMode = pConfig->nTSOutPutMode;//AVL_DVBSx_MPM_Parallel;
	sMpegMode.m_MpegClockPolarity = pConfig->nTSClockPolarity;//AVL_DVBSx_MPCP_Rising;//AVL_DVBSx_MPCP_Falling;
	//This function should be called after demod initialized to set MPEG mode.
	r = AVL_DVBSx_IRx_SetMpegMode(&sMpegMode,pAVLChip );
	if(AVL_DVBSx_EC_OK != r)
	{
		mcDBG_MSG_ERR(("Set MPEG output mode failed !\n"));
	}
     	return r;
}
static AVL_char Get_IsTpLocked(VOID)
{
	AVL_DVBSx_ErrorCode r=AVL_DVBSx_EC_OK;
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;		
	AVL_uint16 uiLockStatus=0;
	
	r = AVL_DVBSx_IRx_GetLockStatus(&uiLockStatus, pAVLChip);
	if( ((AVL_DVBSx_EC_OK == r)&&(1 == uiLockStatus)))
	{
		return 1;
	}
		return 0;
}
static AVL_uchar AVL_Get_Quality_Percent(void)
{
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;
	AVL_DVBSx_ErrorCode r=AVL_DVBSx_EC_OK;
	AVL_uint32 uiSNR;
	AVL_uint16 uiLockStatus=0;
	AVL_uchar SNRrefer = 0;
	AVL_uchar Quality = 0;
	AVL_uchar DVBS_SNR[6] ={12,32,41,52,58,62};
	AVL_uchar DVBS2Qpsk_SNR[8] ={10,24,32,41,47,52,63,65};
	AVL_uchar DVBS28psk_SNR[6] ={57,67,80,95,10,11};	
	struct AVL_DVBSx_SignalInfo SignalInfo;      

	uiLockStatus=Get_IsTpLocked();
	if(uiLockStatus==1)/*只有频点是被锁住的情况下适用，否则返回0*/
	{
		r = AVL_DVBSx_IRx_GetSNR(&uiSNR, pAVLChip);
		r |= AVL_DVBSx_IRx_GetSignalInfo(&SignalInfo, pAVLChip);
	}
	else
	{
		return 0;
	}
	
	if (SignalInfo.m_coderate < RX_DVBS2_1_4)
	{
		SNRrefer = DVBS_SNR[SignalInfo.m_coderate];
	}
	else
	{
		if (SignalInfo.m_modulation == AVL_DVBSx_MM_8PSK)
			SNRrefer = DVBS28psk_SNR[SignalInfo.m_coderate -RX_DVBS2_3_5];
		else
			SNRrefer = DVBS2Qpsk_SNR[SignalInfo.m_coderate -RX_DVBS2_1_2];
	}
/*以下就是大概运算百分比的判断*/
	if ((uiSNR/10) > SNRrefer)
	{
		uiSNR = uiSNR/10 - SNRrefer;
		if(uiSNR>=100)
			Quality = 99;
		else if(uiSNR>=50)  //  >5.0dB
			Quality = 80+ (uiSNR-50)*20/50;
		else if(uiSNR>=25)  //  > 2.5dB
			Quality = 50+ (uiSNR-25)*30/25;
		else if(uiSNR>=10)  //  > 1dB
			Quality = 25+ (uiSNR-10)*25/15;			
		else 
			Quality = 5+ (uiSNR)*20/10;
	}
	else
	{
		Quality = 5;
	}

	return Quality;
}
static AVL_uint32 AVL_Get_Per(VOID) 
{
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;
	AVL_DVBSx_ErrorCode r=AVL_DVBSx_EC_OK;
	AVL_uint32 uiPER=0;
	
	r=AVL_DVBSx_IRx_GetPER(&uiPER, pAVLChip);//return unit is 1e-9;
	if (AVL_DVBSx_EC_OK != r)
	{
		mcDBG_MSG_ERR(("[DVBS_TunerGetSignalPER] --- Fail !\n"));
	}
	else
	{
		uiPER=uiPER/10000;//change to unit is 1e-5;
	}
	return uiPER;
}
static AVL_int16 AVL_Get_Level_Percent(void)
{
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;
	AVL_uint16 Level;
	AVL_int16 i;
	AVL_int16 uiRFSignalDBM;
	AVL_int16 Strength;

	Strength = 0;
	uiRFSignalDBM = 0;
       if(Get_IsTpLocked())
	{
	AVL_DVBSx_IRx_GetSignalLevel(&Level, pAVLChip);
#ifdef BS2S7HZ7801
	for(i=0;i<90;i++)
	{
		if(Level <= AGC_LUT[i].SignalLevel)
		  uiRFSignalDBM = (-1*i);
		else 
		  break;
	}

	if(uiRFSignalDBM> -36)
		Strength = 85+ (uiRFSignalDBM+36)*15/36;
	else if(uiRFSignalDBM> -70)
		Strength = 10+ (uiRFSignalDBM+70)*75/34;
	else
		Strength =(uiRFSignalDBM+90)*10/20;
#endif
#ifdef BS2AV2011
		for(i=0; i<47; i++)
		{
			if(Level<= SignalLevel[i].SignalLevel)   
			{
				//Calculate the corresponding DBM value.
				if((0==i)&&(Level< SignalLevel[i].SignalLevel))
				{
					mcDBG_MSG_ERR(("RFSignalLevel is too weak !"));
					uiRFSignalDBM= -94;
				}
				else
				{
					uiRFSignalDBM=SignalLevel[i].SignalDBM;
				}
				break;
			} 
		}
		
		if(uiRFSignalDBM> -36)
			Strength = 85+ (uiRFSignalDBM+36)*15/36;
		else if(uiRFSignalDBM> -70)
			Strength = 10+ (uiRFSignalDBM+70)*75/34;
		else
			Strength =(uiRFSignalDBM+95)*10/20;		
#endif

#ifdef BS2S7HZ7306
		for(i=0; i<47; i++)
		{
			if(Level<= SignalLevel[i].SignalLevel)   
			{
				//Calculate the corresponding DBM value.
				if((0==i)&&(Level< SignalLevel[i].SignalLevel))
				{
					mcDBG_MSG_ERR(("RFSignalLevel is too weak !"));
					uiRFSignalDBM= -94;
				}
				else
				{
					uiRFSignalDBM=SignalLevel[i].SignalDBM;
				}
				break;
			} 
		}
		if(uiRFSignalDBM> -36)
			Strength = 85+ (uiRFSignalDBM+36)*15/36;
		else if(uiRFSignalDBM> -70)
			Strength = 10+ (uiRFSignalDBM+70)*75/34;
		else
			Strength =(uiRFSignalDBM+95)*10/20;		
#endif
   	}
	return Strength;	
}
static AVL_DVBSx_ErrorCode AVL6211_GetRFOffset(AVL_int16 *piRFOff_100Khz)
{
	AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;
	AVL_int16 iRFOff;
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;
	//This function can be called to get the RF frequency offset after the channel locked.
	AVL_DVBSx_IRx_GetRFOffset(&iRFOff, pAVLChip);
	if (AVL_DVBSx_EC_OK != r)
	{
		mcDBG_MSG_ERR(("Get RFOffset --- Fail !\n"));
		iRFOff = 0;
	}
	*piRFOff_100Khz = iRFOff ;
	return (r);
}
static AVL_DVBSx_ErrorCode AVL_Set_LPF(struct AVL_Tuner * pTuner, AVL_uint32 m_uiSymbolRate_Hz, enum AVL_DVBSx_FunctionalMode enumFunctionalMode)
{
	AVL_DVBSx_ErrorCode r=AVL_DVBSx_EC_OK; 
	struct AVL_Demod_Tuner_Configuration_t *pConfig=&g_DemodTuner_Config[g_nDemodTunerArrayIndex];
	if(enumFunctionalMode == AVL_DVBSx_FunctMode_Demod)
	{	
	       if(pConfig->nTunerLPF_AutoSetFlag)
	       	{
			pTuner->m_uiLPF_100kHz = m_uiSymbolRate_Hz/100000;
	       	}
		else
	   	{
			pTuner->m_uiLPF_100kHz = m_uiSymbolRate_Hz*75/10000000+40;
			
			if(pTuner->m_uiLPF_100kHz > pConfig->nTunerMaxLPF_100Khz)
				pTuner->m_uiLPF_100kHz = pConfig->nTunerMaxLPF_100Khz;
	   	}

	}
	if(enumFunctionalMode == AVL_DVBSx_FunctMode_BlindScan)
	{	
		pTuner->m_uiLPF_100kHz = pConfig->nTunerMaxLPF_100Khz;
	}
	return r;
}
static AVL_DVBSx_ErrorCode AVL6211_22K_Control(AVL_uchar OnOff)
{
	AVL_DVBSx_ErrorCode r=AVL_DVBSx_EC_OK;
	struct  AVL_DVBSx_Chip * pAVLChip  = &AVL_DVBSxChip;  
	if(OnOff)
	{
		r=AVL_DVBSx_IDiseqc_StartContinuous(pAVLChip);
	}else{
		r=AVL_DVBSx_IDiseqc_StopContinuous(pAVLChip);
	}
	return r;
}
static void AV6211_EnableTSOutPut(void)
{
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;	
	mcDBG_MSG_L2(("[AV6211_EnableTSOutPut] is call!\n"));
	AVL_DVBSx_IRx_DriveMpegOutput(pAVLChip); //Enable TS Output
}
static void AV6211_DisableTSOutPut(void)
{
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;	
	mcDBG_MSG_L2(("[AV6211_DisableTSOutPut] is call!\n"));
	AVL_DVBSx_IRx_ReleaseMpegOutput(pAVLChip);//Disable TS Output
}
static void AVL6211_SetToneOut(AVL_uchar ucTone)
{
	AVL_DVBSx_ErrorCode r=AVL_DVBSx_EC_OK;
	struct AVL_DVBSx_Diseqc_TxStatus TxStatus;
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;
	AVL_DVBSx_IDiseqc_SendTone( ucTone, 8, pAVLChip);
	if(r != AVL_DVBSx_EC_OK)
	{
		mcDBG_MSG_ERR(("AVL_DVBSx_IDiseqc_SendTone failed !\n"));
	}
	else
	{
		do
		{
			r= AVL_DVBSx_IDiseqc_GetTxStatus(&TxStatus, pAVLChip);
		}while(TxStatus.m_TxDone != 1);
		if(r ==AVL_DVBSx_EC_OK )
		{

		}
		else
		{
			mcDBG_MSG_ERR(("AVL_DVBSx_IDiseqc_SendTone Err. !\n"));
		}
	}
	
}
static void AVL6211_DiseqcSendCmd(AVL_puchar pCmd,AVL_uchar CmdSize)
{
	AVL_DVBSx_ErrorCode r=AVL_DVBSx_EC_OK;
	struct AVL_DVBSx_Diseqc_TxStatus TxStatus;
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;
	
	r=AVL_DVBSx_IDiseqc_SendModulationData(pCmd, CmdSize, pAVLChip);
	if(r != AVL_DVBSx_EC_OK)
	{
		mcDBG_MSG_ERR(("AVL_DVBSx_IDiseqc_SendModulationData failed !\n"));
	}
	else
	{
		do
		{
			r= AVL_DVBSx_IDiseqc_GetTxStatus(&TxStatus, pAVLChip);
		}while(TxStatus.m_TxDone != 1);
		if(r ==AVL_DVBSx_EC_OK )
		{

		}
		else
		{
			mcDBG_MSG_ERR(("AVL_DVBSx_IDiseqc_SendModulationData Err. !\n"));
		}		
	}
}
static AVL_DVBSx_ErrorCode CPU_Halt(struct AVL_DVBSx_Chip * pAVLChip)
{
     AVL_DVBSx_ErrorCode r;
     AVL_uint16 i= 0;

     r = AVL_DVBSx_IBase_SendRxOP(OP_RX_HALT, pAVLChip );

     if(AVL_DVBSx_EC_OK == r)
     {
          while(i++<20)
          {
              r = AVL_DVBSx_IBase_GetRxOPStatus(pAVLChip);
              if(AVL_DVBSx_EC_OK == r)
              {
                   break;
              }
              else
              {
                   AVL_DVBSx_IBSP_Delay(10);
              }
          }
     }
     return (r);
}
static AVL_DVBSx_ErrorCode  AVL6211_LockChannel(UINT32 freq_100kHZ,UINT32 symbolrate_HZ)//,AVL_uchar lockflag,void *pvArg)
{
	AVL_DVBSx_ErrorCode r=AVL_DVBSx_EC_OK;
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;		
	struct AVL_Tuner * pTuner =&Tuner;	
	struct AVL_DVBSx_Channel Channel;  
	AVL_uint32 uiTimeth; 
	AVL_uint16 uiCounter;
	
	mcDBG_MSG_L2(("[AVL6211_LockChannel]  freq_MHZ:%d ,symbolrate_KHZ:%d!\n",freq_100kHZ/10,symbolrate_HZ/1000));

	r=CPU_Halt(pAVLChip);
	if (AVL_DVBSx_EC_OK != r)
	{
		mcDBG_MSG_ERR(("[AVL6211_LockChannel] cpu halt fail..!\n"));
	}
	pTuner->m_uiFrequency_100kHz = freq_100kHZ; 
	 AVL_Set_LPF(pTuner,symbolrate_HZ,AVL_DVBSx_FunctMode_Demod);
	r = pTuner->m_pLockFunc(pTuner);

	AVL_DVBSx_IBSP_Delay(100);		//Wait a while for tuner to lock in certain frequency.

	Channel.m_uiSymbolRate_Hz = symbolrate_HZ;      //Change the value defined by macro when we want to lock a new channel.
	Channel.m_Flags = (CI_FLAG_IQ_NO_SWAPPED) << CI_FLAG_IQ_BIT;	//Normal IQ
	Channel.m_Flags |= (CI_FLAG_IQ_AUTO_BIT_AUTO) << CI_FLAG_IQ_AUTO_BIT;	//Enable automatic IQ swap detection
	Channel.m_Flags |= (CI_FLAG_DVBS2_UNDEF) << CI_FLAG_DVBS2_BIT;			//Enable automatic standard detection

	//This function should be called after tuner locked to lock the channel.
	r = AVL_DVBSx_IRx_LockChannel(&Channel, pAVLChip);  
	if (AVL_DVBSx_EC_OK != r)
	{
		mcDBG_MSG_ERR(("Lock channel failed: r=%d !\n",r));
		return r;
	}
	
	if(symbolrate_HZ<5000000)
	{
		uiTimeth=2500*2;       //The max waiting time is 5000ms,considering the IQ swapped status the time should be doubled.
	}
	else if(symbolrate_HZ<10000000)
	{
		uiTimeth=600*2;        //The max waiting time is 600ms,considering the IQ swapped status the time should be doubled.
	}
	else
	{
		uiTimeth=250*2;        //The max waiting time is 250ms,considering the IQ swapped status the time should be doubled.
	} 
	
	uiCounter=uiTimeth/10;
	g_nWaitLoop_Max=uiCounter;	
	return r;
}
static UINT32 AVL6211_Calc_TS_CLK(BOOL TS_Type)//0=parallel 1=serial
{
	UINT32 data =0;
#if 0	
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;
	AVL_uint16 uiLockstatus =0;
	struct AVL_DVBSx_SignalInfo SignalInfo;	
	
	AVL_DVBSx_II2C_Read16(pAVLChip, rs_fec_lock_addr, &uiLockstatus);
	if (uiLockstatus == 1)
	{
		//ts data rate = symbolrate*2*coderate	//for QPSK add by availink
		//ts data rate = symbolrate*3*coderate	//for 8PSK add by availink
		AVL_DVBSx_IRx_GetSignalInfo(&SignalInfo, pAVLChip);
		data = (Current_channel.m_uiSymbolRate_Hz/1000)*(SignalInfo.m_modulation+2);
		switch(SignalInfo.m_coderate) 
		{
			case RX_DVBS_1_2:	data = (data*1)/2;  break;
			case RX_DVBS_3_4:	data = (data*3)/4;  break; 							
			case RX_DVBS_5_6:	data = (data*5)/6;  break; 							
			case RX_DVBS_6_7:	data = (data*6)/7;  break; 							
			case RX_DVBS_7_8:	data = (data*7)/8;  break; 							
			case RX_DVBS2_1_4:	data = (data*1)/4;  break; 							
			case RX_DVBS2_1_3:	data = (data*1)/3;  break; 										
			case RX_DVBS2_2_5:	data = (data*2)/5;  break; 							
			case RX_DVBS2_1_2:	data = (data*1)/2;  break; 							
			case RX_DVBS2_3_5:	data = (data*3)/5;  break; 							
			case RX_DVBS2_2_3:	data = (data*2)/3;  break; 							
			case RX_DVBS2_3_4:	data = (data*3)/4;  break; 							
			case RX_DVBS2_4_5:	data = (data*4)/5;  break; 							
			case RX_DVBS2_5_6:	data = (data*5)/6;  break; 							
			case RX_DVBS2_8_9:	data = (data*8)/9;  break; 								
			case RX_DVBS2_9_10: data = (data*9)/10; break; 								
			default:			data = (data*9)/10; break;
		}		
	}
	else  
	{ 
			data = 110000; 
	}	
	
#else
	data = 122000;
#endif
	if(BOOL_FALSE == TS_Type)	//calculate Parallel TS clock
		data = data/8+100;
	else ;
	
	return data;
}
static AVL_DVBSx_ErrorCode AVL6211_GetTSMODE(AVL_uchar * SerialOrParallel, AVL_uchar * CLK_Polar,AVL_uchar * TS_Tri)
{
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;
	AVL_DVBSx_ErrorCode r=AVL_DVBSx_EC_OK;	
	AVL_uint16 Reg_16;
	AVL_uint32 Reg_32;
	
	r = AVL_DVBSx_II2C_Read16(pAVLChip, rc_mpeg_serial_addr, &Reg_16);
	*SerialOrParallel = (AVL_uchar)Reg_16;
	    
	r |= AVL_DVBSx_II2C_Read16(pAVLChip, rc_mpeg_posedge_addr, &Reg_16);
	*CLK_Polar = (AVL_uchar)Reg_16;

	r |= AVL_DVBSx_II2C_Read32(pAVLChip, rc_mpeg_bus_tri_enb, &Reg_32);
	*TS_Tri = (AVL_uchar)Reg_32;

	return r;
}

static void Channels_Filter_and_Adjust(struct AVL_DVBSx_Channel *Ch_list_valid, AVL_uchar *TP_No_valid, struct AVL_DVBSx_Channel *Ch_list_Temp, AVL_uchar TP_No_Temp)
{
	AVL_uchar i,j,flag;
	AVL_uchar Num =*TP_No_valid;
	struct AVL_DVBSx_Channel *pTemp;
	struct AVL_DVBSx_Channel *pValid;
	AVL_uint32 uiSymbolRate_Hz;
	AVL_uint32 ui_SR_offset;

	
	for(i=0;i< TP_No_Temp;i++)
	{
		pTemp = &Ch_list_Temp[i];
		flag =0;
		for(j=0;j<*TP_No_valid;j++)
		{
			pValid = &Ch_list_valid[j];
			if( Sub_abs(pValid->m_uiFrequency_kHz,pTemp->m_uiFrequency_kHz) < AVL_min(pValid->m_uiSymbolRate_Hz,pTemp->m_uiSymbolRate_Hz)/2000)
			{
				flag = 1;
				break;
			}				
		}

		if(0 == flag)
		{
			Ch_list_valid[Num].m_Flags = pTemp->m_Flags;
			Ch_list_valid[Num].m_uiSymbolRate_Hz = pTemp->m_uiSymbolRate_Hz;
			Ch_list_valid[Num].m_uiFrequency_kHz = pTemp->m_uiFrequency_kHz;

			uiSymbolRate_Hz = Ch_list_valid[Num].m_uiSymbolRate_Hz;
			//----------------------------adjust symbolrate offset------------------------------------------------------------
			ui_SR_offset = ((uiSymbolRate_Hz%10000)>5000)?(10000-(uiSymbolRate_Hz%10000)):(uiSymbolRate_Hz%10000);
			if( ((uiSymbolRate_Hz>10000000) && (ui_SR_offset<3500)) || ((uiSymbolRate_Hz>5000000) && (ui_SR_offset<2000))  )
				uiSymbolRate_Hz =  (uiSymbolRate_Hz%10000<5000)?(uiSymbolRate_Hz-ui_SR_offset):(uiSymbolRate_Hz+ui_SR_offset);

			ui_SR_offset = ((uiSymbolRate_Hz%1000)>500)?(1000-(uiSymbolRate_Hz%1000)):(uiSymbolRate_Hz%1000);
			if( (uiSymbolRate_Hz<5000000) && (ui_SR_offset< 500))
				uiSymbolRate_Hz =  (uiSymbolRate_Hz%1000<500)?(uiSymbolRate_Hz-ui_SR_offset):(uiSymbolRate_Hz+ui_SR_offset);
	
			Ch_list_valid[Num].m_uiSymbolRate_Hz =1000*(uiSymbolRate_Hz/1000);
			//----------------------------------------------------------------------------------------------------------------
			Num++;
		}
	}
	
	*TP_No_valid = Num;
}
static AVL_DVBSx_ErrorCode AVL6211_Initialize(struct AVL_DVBSx_Chip * pAVLChip,struct AVL_Tuner * pTuner)
{
	struct AVL_DVBSx_Diseqc_Para sDiseqcPara;
	AVL_DVBSx_ErrorCode r=AVL_DVBSx_EC_OK;
	struct AVL_Demod_Tuner_Configuration_t *pConfig=&g_DemodTuner_Config[g_nDemodTunerArrayIndex];
	
	mcDBG_MSG_L2((" %s \n",__FUNCTION__));

	// This function should be called after bsp initialized to initialize the chip object.
	r = Init_AVL_DVBSx_ChipObject(pAVLChip, ((pConfig->nDemodAddress)<<1));	
	if( AVL_DVBSx_EC_OK !=r ) 
	{
		mcDBG_MSG_ERR(("Chip Object Initialization failed !\n"));
		return (r);
	}
	//This function should be called after chip object initialized to initialize the IBase,using reference clock as 10M. Make sure you pickup the right pll_conf since it may be modified in BSP.
	r = AVL_DVBSx_IBase_Initialize((pll_conf+pConfig->nDemodReferenceClk), ucPatchData, pAVLChip); //d110915_Haibo:For 10M PLL,The parameter is pll_conf+2;27MHz is pll_conf+4
	if( AVL_DVBSx_EC_OK !=r ) 
	{
		mcDBG_MSG_ERR(("IBase Initialization failed !\n"));
		return (r);
	}
	AVL_DVBSx_IBSP_Delay(100);	  //Wait 100 ms to assure that the AVL_DVBSx chip boots up.This function should be implemented by customer.
  
	//This function should be called to verify the AVL_DVBSx chip has completed its initialization procedure.
	r = AVL_DVBSx_IBase_GetStatus(pAVLChip);
	if( AVL_DVBSx_EC_OK != r )		 
	{
		mcDBG_MSG_ERR(("Booted failed !\n"));
		return (r);
	}
	mcDBG_MSG_L2(("Booted !\n"));
	//This function should be called after IBase initialized to initialize the demod.
	r = AVL_DVBSx_IRx_Initialize(pAVLChip);
	if(AVL_DVBSx_EC_OK != r)
	{
		mcDBG_MSG_ERR(("Demod Initialization failed !\n"));
		return (r);
	}
	
	//modify rx_config.aagc_ref
	r |= AVL_DVBSx_II2C_Write16(pAVLChip, rc_aagc_ref_addr, 48);
	r |= AVL_DVBSx_IRx_DriveRFAGC(pAVLChip);


	//This function should be called after demod initialized to set RF AGC polar.
	//User does not need to setup this for Sharp tuner since it is the default value. But for other tuners, user may need to do it here.
	r = AVL_DVBSx_IRx_SetRFAGCPola(pConfig->nTunerRFAGC, pAVLChip);
	if(AVL_DVBSx_EC_OK != r)
	{
		mcDBG_MSG_ERR(("Set RF AGC Polar failed !\n"));
		return (r);
	}

	//Setup MPEG mode parameters.
	r=AV6211_TS_Parallel_Mode();
	if(AVL_DVBSx_EC_OK != r)
	{
		mcDBG_MSG_ERR(("Set MPEG output mode failed !\n"));
		return (r);
	}
	
	//Setup tuner parameters for tuner initialization.
	pTuner->m_uiI2CBusClock_kHz = IX2610VA_I2Cbus_clock;		
	pTuner->m_pParameters = 0;			
	pTuner->m_pAVLChip = pAVLChip;	  
	pTuner->m_uiSlaveAddress = pConfig->nTunerAddress; 
	pTuner->m_pInitializeFunc = pConfig->m_pInitializeFunc;	 
	pTuner->m_pLockFunc = pConfig->m_pLockFunc; 
	pTuner->m_pGetLockStatusFunc = pConfig->m_pGetLockStatusFunc;
	r = pTuner->m_pInitializeFunc(pTuner);
	if(AVL_DVBSx_EC_OK != r)
	{
		mcDBG_MSG_ERR(("Tuner Initialization failed !\n"));
		return (r);
	}

	//Setup Diseqc parameters for Diseqc initialization.
	sDiseqcPara.m_RxTimeout = AVL_DVBSx_DRT_150ms;
	sDiseqcPara.m_RxWaveForm = AVL_Get_22k_Configration_by_LNBType();
	sDiseqcPara.m_ToneFrequency_kHz = Diseqc_Tone_Frequency;		
	sDiseqcPara.m_TXGap = AVL_DVBSx_DTXG_15ms;
	sDiseqcPara.m_TxWaveForm = AVL_Get_22k_Configration_by_LNBType();

	//The Diseqc should be initialized if AVL_DVBSx need to supply power to LNB. This function should be called after IBase initialized to initialize the Diseqc.
	r = AVL_DVBSx_IDiseqc_Initialize(&sDiseqcPara, pAVLChip);
	if(AVL_DVBSx_EC_OK != r)
	{
		mcDBG_MSG_ERR(("Diseqc Initialization failed !\n"));
	}
{
	struct AVL_DVBSx_VerInfo VerInfo;
	
	AVL_DVBSx_IBase_GetVersion( &VerInfo, pAVLChip);
	mcDBG_MSG_L2(("Chip Ver:{%d}.{%d}.{%d}  API Ver:{%d}.{%d}.{%d}  Patch Ver:{%d}.{%d}.{%d} \n", 
						VerInfo.m_Chip.m_Major, VerInfo.m_Chip.m_Minor, VerInfo.m_Chip.m_Build, 
						VerInfo.m_API.m_Major, VerInfo.m_API.m_Minor, VerInfo.m_API.m_Build, 
						VerInfo.m_Patch.m_Major, VerInfo.m_Patch.m_Minor, VerInfo.m_Patch.m_Build));	
}	
	return (r);
}
INT32 AVL_DVBS2_Demod_Init(VOID)
{
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;		
	struct AVL_Tuner * pTuner =&Tuner;	

	AVL6211_Initialize(pAVLChip,pTuner);
	return DRVAPI_TUNER_OK;
}
INT32 AVL_DVBS2_Set_TS_ParallelMode(VOID)
{
	AV6211_TS_Parallel_Mode();
	return DRVAPI_TUNER_OK;
}
BOOL AVL_DVBS2_CheckTP_IsLocked(VOID)
{
	AVL_char ret=0;
        BOOL val=TRUE;
	ret=Get_IsTpLocked();
	if(ret)
	{
		val= TRUE;
	}
	else
	{
		val= FALSE;
	}
	return val;
}
UINT8 AVL_DVBS2_Get_SignalQualityPercent(VOID)
{
	UINT8 val=0;
	val=(UINT8)AVL_Get_Quality_Percent();
	return val;
}
UINT8 AVL_DVBS2_Get_SignalLevelPercent(VOID)
{
	UINT8 val=0;
	val=(UINT8)AVL_Get_Level_Percent();
	return val;
}
INT32 AVL_DVBS2_Get_SignalOffSet(INT16 *piRFOff_100Khz)
{
	if(piRFOff_100Khz)
	{
		AVL6211_GetRFOffset(piRFOff_100Khz);
	}
	return DRVAPI_TUNER_OK;
}
INT32 AVL_DVBS2_Enable_TS(VOID)
{
	AV6211_EnableTSOutPut();
	return DRVAPI_TUNER_OK;
}
INT32 AVL_DVBS2_Disable_TS(VOID)
{
	AV6211_DisableTSOutPut();
	return DRVAPI_TUNER_OK;
}
INT32 AVL_DVBS2_Select_Tone(UINT8 nAorB)
{

	if(nAorB)
		AVL6211_SetToneOut(1);
	else
		AVL6211_SetToneOut(0);
	
	return DRVAPI_TUNER_OK;
}
INT32 AVL_DVBS2_SendDiseqcCmmd(UINT8  *pCmd,UINT8  CmdSize)
{
	AVL6211_DiseqcSendCmd( pCmd, CmdSize);
	return DRVAPI_TUNER_OK;
}
INT32 AVL_DVBS2_LcokChannel(UINT32 freq_100kHZ,UINT32 symbolrate_HZ,BOOL fgUnicable)
{
    AVL_DVBSx_ErrorCode r=AVL_DVBSx_EC_OK;
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;
    struct AVL_Demod_Tuner_Configuration_t *pConfig=&g_DemodTuner_Config[g_nDemodTunerArrayIndex];
	enum AVL_DVBSx_SpectrumPolarity nTuner_IQ_SpectrumMode=pConfig->nTuner_IQ_SpectrumMode;
#ifdef Unicable_IQ_Spectrum_Invert	
	if(TRUE==fgUnicable)
	{
	   if(AVL_DVBSx_Spectrum_Normal==nTuner_IQ_SpectrumMode)nTuner_IQ_SpectrumMode=AVL_DVBSx_Spectrum_Invert;
	   else nTuner_IQ_SpectrumMode=AVL_DVBSx_Spectrum_Normal;
	}
#endif	
    r=AVL_DVBSx_IBase_SetSpectrumPolarity(nTuner_IQ_SpectrumMode, pAVLChip);
	
	if (AVL_DVBSx_EC_OK != r)
	{
		mcDBG_MSG_ERR(("[AVL_DVBSx_IBase_SetSpectrumPolarity]  fail..!\n"));
	}
	
    AVL6211_LockChannel( freq_100kHZ, symbolrate_HZ);
    return DRVAPI_TUNER_OK;
}
UINT32 AVL_DVBS2_Get_TSClock(VOID)
{
	UINT32 val=0;
	val=AVL6211_Calc_TS_CLK(0);
	return val;
}
INT32 AVL_DVBS2_Get_TSMode(UCHAR * SerialOrParallel, UCHAR * CLK_Polar,UCHAR * TS_Tri)
{
	AVL6211_GetTSMODE(SerialOrParallel, CLK_Polar, TS_Tri);
	return DRVAPI_TUNER_OK;
}
INT32 AVL_DVBS2_22K_ON(VOID)
{
	AVL6211_22K_Control(1);
	return DRVAPI_TUNER_OK;
}
INT32 AVL_DVBS2_22K_OFF(VOID)
{
	AVL6211_22K_Control(0);
	return DRVAPI_TUNER_OK;
}
INT32 AVL_DVBS2_Get_PER(UINT32 *pu4_ret)
{
	if(pu4_ret)
		*pu4_ret=(UINT32)AVL_Get_Per();
	return DRVAPI_TUNER_OK;
}
INT32 AVL_DVBS2_Get_SNR(UINT32 *pu4_ret)
{
	AVL_DVBSx_ErrorCode r=AVL_DVBSx_EC_OK;
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;	
	AVL_uint32 uiSNR=0;
	
	r=AVL_DVBSx_IRx_GetSNR(&uiSNR, pAVLChip);//the unit is db*100;exp:27.58db ,uiSNR=2758;
	if (AVL_DVBSx_EC_OK != r)
	{
		mcDBG_MSG_L2(("Get SNR --- Fail !\n")); 
	}
	if(pu4_ret)
	{
		*pu4_ret=uiSNR/100;
	}
	return DRVAPI_TUNER_OK;
}
TUNER_DVB_S_S2_MODE_T AVL_DVBS2_Get_SYSTEM(VOID)
{
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;
	struct AVL_DVBSx_SignalInfo SignalInfo;  
	TUNER_DVB_S_S2_MODE_T u1dvbs_system=TUNER_DVB_AUTO_S_S2;
	AVL_DVBSx_IRx_GetSignalInfo(&SignalInfo, pAVLChip);
	
	mcDBG_MSG_L4(("dvbs code rate is %d!\n",SignalInfo.m_coderate)); 
	if(SignalInfo.m_coderate <= RX_DVBS_7_8)// 0<=fec<=5,dvbs
	{
		u1dvbs_system=TUNER_DVB_S;
	}
	else if(SignalInfo.m_coderate <=RX_DVBS2_9_10)// 6<=fec<=16,dvbs2
	{
		u1dvbs_system=TUNER_DVB_S2;
	}
	else //system unknown
	{
		u1dvbs_system=TUNER_DVB_AUTO_S_S2;
	}
	return (u1dvbs_system);
}

UINT8 AVL_DVBS2_Get_Signal_Info(VOID *SignalInfo)
{
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;
	struct AVL_DVBSx_SignalInfo *pSignalInfo;
    pSignalInfo=(struct AVL_DVBSx_SignalInfo*)SignalInfo;
	
	if(0==(UINT8)AVL_DVBSx_IRx_GetSignalInfo(pSignalInfo, pAVLChip))
		return 0;
	else
		return 1;
}

UINT16 AVL_DVBS2_Get_MaxLoop(VOID)
{
   return g_nWaitLoop_Max;
}

VOID AVL_DVBS2_BlindScanInit(VOID)
{
	g_u16Total_tp=0;
	x_memset(Avl6211_channels,0,256*sizeof(struct AVL_DVBSx_Channel));
	return ;
}
INT32 AVL_DVBS2_BlindScanAction(UINT16  nFreqCenter_100KHZ,DVBS2_CHANNEL_T *pChannelList,UCHAR *Find_TP_num)	
{
	AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;
	struct AVL_Demod_Tuner_Configuration_t *pConfig=&g_DemodTuner_Config[g_nDemodTunerArrayIndex];
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;		
	struct AVL_Tuner * pTuner =&Tuner;	
	struct AVL_DVBSx_BlindScanPara bsPara;
	struct AVL_DVBSx_BlindScanInfo bsInfo;

	UCHAR ChannelNum_temp=0;
	UCHAR  Pre_total_num=0;
	UCHAR i=0,j=0;
	UCHAR bs_min_symbolrate=2	;		//The minimum symbol rate, 1M
	UCHAR bs_max_symbolrate	=45	;		//The maximum symbol rate, 45M
	//UINT8 i=0;
	x_memset(Channellist_temp,0,sizeof(Channellist_temp));
	
	//r |= AVL_DVBSx_IBlindScan_SetSpectrumInversion(pAVLChip, AVL_DVBSx_BS_NORMAL); 
	r|=AVL_DVBSx_IBase_SetSpectrumPolarity(pConfig->nTuner_IQ_SpectrumMode, pAVLChip);
	r |= AVL_DVBSx_IBlindScan_Reset(pAVLChip);

	pTuner->m_uiFrequency_100kHz = nFreqCenter_100KHZ;
       bsPara.m_uiMaxSymRate_kHz = 1000*bs_max_symbolrate;
	bsPara.m_uiMinSymRate_kHz = 1000*bs_min_symbolrate;	
	bsPara.m_uiStartFreq_100kHz =nFreqCenter_100KHZ-320;
	bsPara.m_uiStopFreq_100kHz = nFreqCenter_100KHZ + 320; 

	r |= AVL_Set_LPF(pTuner,0,AVL_DVBSx_FunctMode_BlindScan);
	r |= pTuner->m_pLockFunc(pTuner);   //Lock the tuner.
	
	AVL_DVBSx_IBSP_Delay(100);		//wait a while for tuner to lock in certain frequency.
	
	r|= pTuner->m_pGetLockStatusFunc(pTuner);	 //Check the lock status of the tuner.
	if (AVL_DVBSx_EC_OK != r)		 
	{
		mcDBG_MSG_L2(("Tuner can not lock to %d MHz !!!\n", pTuner->m_uiFrequency_100kHz/10));
		return -1;
	}
	else
		mcDBG_MSG_L2(("Lock to %4d MHz ",pTuner->m_uiFrequency_100kHz/10));
		
	//This function should be called after tuner lock to perform a blind scan.
	
	*Find_TP_num = 0;
	AVL_DVBSx_IBase_SetFunctionalMode(pAVLChip, AVL_DVBSx_FunctMode_BlindScan);
	r |= AVL_DVBSx_IBlindScan_Scan(&bsPara,340, pAVLChip);
	do 
	{
		mcDBG_MSG_L2(("$"));
		 AVL_DVBSx_IBSP_Delay(200); 	//wait 200 milionsecond
		 r = AVL_DVBSx_IBlindScan_GetScanStatus(&bsInfo, pAVLChip);  //Query the internal blind scan procedure information.
		 if(AVL_DVBSx_EC_OK != r)
		 {
			mcDBG_MSG_ERR(("Get scan status failed !\n"));
			break;
		 }
	}	
	while(100 != bsInfo.m_uiProgress);	//100 indicates the blind scan is finished. 
	
	if(bsInfo.m_uiChannelCount>0)
	{
		r = AVL_DVBSx_IBlindScan_ReadChannelInfo(0, &(bsInfo.m_uiChannelCount), Channellist_temp, pAVLChip);
	}
    
	ChannelNum_temp = (AVL_uchar)bsInfo.m_uiChannelCount;
	Pre_total_num=g_u16Total_tp;
	Channels_Filter_and_Adjust(Avl6211_channels, &g_u16Total_tp, Channellist_temp, ChannelNum_temp);
	i=0;
	if(g_u16Total_tp>Pre_total_num)
	{	
		for(j=Pre_total_num;j<g_u16Total_tp;j++)
		{
			pChannelList[i].m_uiFrequency_kHz=Avl6211_channels[j].m_uiFrequency_kHz;
			pChannelList[i].m_uiSymbolRate_Hz=Avl6211_channels[j].m_uiSymbolRate_Hz;
			i++;
			mcDBG_MSG_L2(("[AVL_DVBS2_BlindScanAction] Ch%d: RF: %4d Mhz,SR: %5d Khz\n",j, (Avl6211_channels[j].m_uiFrequency_kHz/1000),(Avl6211_channels[j].m_uiSymbolRate_Hz/1000))); 
		}
	}
	*Find_TP_num=i;
	
	r = AVL_DVBSx_IBase_SetFunctionalMode(pAVLChip, AVL_DVBSx_FunctMode_Demod);
	if(AVL_DVBSx_EC_OK != r)
	{
		mcDBG_MSG_ERR(("AVL_DVBSx_FunctMode_Demod failed !\n"));
	} 	
	
	if(r==AVL_DVBSx_EC_OK )
		return DRVAPI_TUNER_OK;
	else
		return -1;
}

VOID AVL_DVBS2_Unicable_ODU_Channel_Change(UINT8 *command,TUNER_UNICABLE_CC_T *pUniCabinfo )
{
	AVL_uint16 freq_MHZ=AVL_abs(pUniCabinfo->ui4_transponder_freq-pUniCabinfo->ui2_lnb_freq);

	Unicable_Cmmond_ChannelChange_Config( command, pUniCabinfo->e_user_band, pUniCabinfo->e_bank, freq_MHZ, pUniCabinfo->ui2_band_freq);
	return ;
}
VOID AVL_DVBS2_Unicable_ODU_PowerOff(UINT8 *command,UINT8 UserBand)
{
	command[0] = 0xe0;
	command[1] = 0x10;
	command[2] = 0x5A;
	command[3] = 0x00;
	command[3] = UserBand<<5;
	command[4] = 0x00;
}
INT32 AVL_DVBS2_Unicable_BlindScanAction(UINT16  nTunerCenter_100KHZ,UINT16  nFreqCenter_100KHZ,DVBS2_CHANNEL_T *pChannelList,UCHAR *Find_TP_num)	
{
	AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;
//	struct AVL_Demod_Tuner_Configuration_t *pConfig=&g_DemodTuner_Config[g_nDemodTunerArrayIndex];
	struct AVL_DVBSx_Chip * pAVLChip = &AVL_DVBSxChip;		
	struct AVL_Tuner * pTuner =&Tuner;	
	struct AVL_DVBSx_BlindScanPara bsPara;
	struct AVL_DVBSx_BlindScanInfo bsInfo;

	UCHAR ChannelNum_temp=0;
	UCHAR  Pre_total_num=0;
	UCHAR i=0,j=0;
	UCHAR bs_min_symbolrate=2	;		//The    symbol rate, 1M
	UCHAR bs_max_symbolrate	=45	;		//The maximum symbol rate, 45M
	struct AVL_Demod_Tuner_Configuration_t *pConfig=&g_DemodTuner_Config[g_nDemodTunerArrayIndex];
	enum AVL_DVBSx_SpectrumPolarity nTuner_IQ_SpectrumMode=pConfig->nTuner_IQ_SpectrumMode;
	//UINT8 i=0;
	x_memset(Channellist_temp,0,sizeof(Channellist_temp));
	
	//r |= AVL_DVBSx_IBlindScan_SetSpectrumInversion(pAVLChip, AVL_DVBSx_BS_NORMAL); 
#ifdef Unicable_IQ_Spectrum_Invert
	if(AVL_DVBSx_Spectrum_Normal==nTuner_IQ_SpectrumMode)nTuner_IQ_SpectrumMode=AVL_DVBSx_Spectrum_Invert;
	else nTuner_IQ_SpectrumMode=AVL_DVBSx_Spectrum_Normal;
#endif
    r=AVL_DVBSx_IBase_SetSpectrumPolarity(nTuner_IQ_SpectrumMode, pAVLChip);


	r |= AVL_DVBSx_IBlindScan_Reset(pAVLChip);

	pTuner->m_uiFrequency_100kHz = nTunerCenter_100KHZ;
       bsPara.m_uiMaxSymRate_kHz = 1000*bs_max_symbolrate;
	bsPara.m_uiMinSymRate_kHz = 1000*bs_min_symbolrate;	
	bsPara.m_uiStartFreq_100kHz =nFreqCenter_100KHZ-320;
	bsPara.m_uiStopFreq_100kHz = nFreqCenter_100KHZ + 320; 

	r |= AVL_Set_LPF(pTuner,0,AVL_DVBSx_FunctMode_BlindScan);
	r |= pTuner->m_pLockFunc(pTuner);   //Lock the tuner.
	
	AVL_DVBSx_IBSP_Delay(100);		//wait a while for tuner to lock in certain frequency.
	
	r|= pTuner->m_pGetLockStatusFunc(pTuner);	 //Check the lock status of the tuner.
	if (AVL_DVBSx_EC_OK != r)		 
	{
		mcDBG_MSG_L2(("Tuner can not lock to %d MHz !!!\n", pTuner->m_uiFrequency_100kHz/10));
		return -1;
	}
	else
		mcDBG_MSG_L2(("Lock to %4d MHz ",nFreqCenter_100KHZ/10));
		
	//This function should be called after tuner lock to perform a blind scan.
	
	*Find_TP_num = 0;
	AVL_DVBSx_IBase_SetFunctionalMode(pAVLChip, AVL_DVBSx_FunctMode_BlindScan);
	r |= AVL_DVBSx_IBlindScan_Scan(&bsPara,340, pAVLChip);
	do 
	{
		mcDBG_MSG_L2(("$"));
		 AVL_DVBSx_IBSP_Delay(200); 	//wait 200 milionsecond
		 r = AVL_DVBSx_IBlindScan_GetScanStatus(&bsInfo, pAVLChip);  //Query the internal blind scan procedure information.
		 if(AVL_DVBSx_EC_OK != r)
		 {
			mcDBG_MSG_ERR(("Get scan status failed !\n"));
			break;
		 }
	}	
	while(100 != bsInfo.m_uiProgress);	//100 indicates the blind scan is finished. 
	
	if(bsInfo.m_uiChannelCount>0)
	{
		r = AVL_DVBSx_IBlindScan_ReadChannelInfo(0, &(bsInfo.m_uiChannelCount), Channellist_temp, pAVLChip);
	}
    
	ChannelNum_temp = (AVL_uchar)bsInfo.m_uiChannelCount;
	Pre_total_num=g_u16Total_tp;
	Channels_Filter_and_Adjust(Avl6211_channels, &g_u16Total_tp, Channellist_temp, ChannelNum_temp);
	i=0;
	if(g_u16Total_tp>Pre_total_num)
	{	
		for(j=Pre_total_num;j<g_u16Total_tp;j++)
		{
			pChannelList[i].m_uiFrequency_kHz=Avl6211_channels[j].m_uiFrequency_kHz;
			pChannelList[i].m_uiSymbolRate_Hz=Avl6211_channels[j].m_uiSymbolRate_Hz;
			i++;
			mcDBG_MSG_L2(("[AVL_DVBS2_Unicable_BlindScanAction] Ch%d: RF: %4d Mhz,SR: %5d Khz\n",j, (Avl6211_channels[j].m_uiFrequency_kHz/1000),(Avl6211_channels[j].m_uiSymbolRate_Hz/1000))); 
		}
	}
	*Find_TP_num=i;
	
	r = AVL_DVBSx_IBase_SetFunctionalMode(pAVLChip, AVL_DVBSx_FunctMode_Demod);
	if(AVL_DVBSx_EC_OK != r)
	{
		mcDBG_MSG_ERR(("AVL_DVBSx_FunctMode_Demod failed !\n"));
	} 	
	
	if(r==AVL_DVBSx_EC_OK )
		return DRVAPI_TUNER_OK;
	else
		return -1;
}
