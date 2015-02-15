#ifndef _AVL_DVBS2_PORTING_H_
#define _AVL_DVBS2_PORTING_H_
typedef struct CurrnetChannel
{
		UINT32  m_uiFrequency_kHz;		///< The channel carrier frequency in units of kHz. 
		UINT32  m_uiSymbolRate_Hz;
		TUNER_FEC_INNER_T m_fec;
}DVBS2_CHANNEL_T;

INT32 AVL_DVBS2_Demod_Init(VOID);
INT32 AVL_DVBS2_Set_TS_ParallelMode(VOID);
BOOL AVL_DVBS2_CheckTP_IsLocked(VOID);
UINT8 AVL_DVBS2_Get_SignalQualityPercent(VOID);
UINT8 AVL_DVBS2_Get_SignalLevelPercent(VOID);
INT32 AVL_DVBS2_Get_SignalOffSet(INT16 *piRFOff_100Khz);
INT32 AVL_DVBS2_Enable_TS(VOID);
INT32 AVL_DVBS2_Disable_TS(VOID);
INT32 AVL_DVBS2_Select_Tone(UINT8 nAorB);
INT32 AVL_DVBS2_SendDiseqcCmmd(UINT8  *pCmd,UINT8  CmdSize);
INT32 AVL_DVBS2_LcokChannel(UINT32 freq_100kHZ,UINT32 symbolrate_HZ,BOOL fgUnicable);
UINT32 AVL_DVBS2_Get_TSClock(VOID);
INT32 AVL_DVBS2_Get_TSMode(UCHAR * SerialOrParallel, UCHAR * CLK_Polar,UCHAR * TS_Tri);
INT32 AVL_DVBS2_22K_ON(VOID);
INT32 AVL_DVBS2_22K_OFF(VOID);
INT32 AVL_DVBS2_Get_PER(UINT32 *pu4_ret);
INT32 AVL_DVBS2_Get_SNR(UINT32 *pu4_ret);
UINT16 AVL_DVBS2_Get_MaxLoop(VOID);
TUNER_DVB_S_S2_MODE_T AVL_DVBS2_Get_SYSTEM(VOID);
VOID AVL_DVBS2_BlindScanInit(VOID);
INT32 AVL_DVBS2_BlindScanAction(UINT16  nFreqCenter_100KHZ,DVBS2_CHANNEL_T *pChannelList,UCHAR *Find_TP_num);
VOID AVL_DVBS2_Unicable_ODU_Channel_Change(UINT8 *command,TUNER_UNICABLE_CC_T *pUniCabinfo );
VOID AVL_DVBS2_Unicable_ODU_PowerOff(UINT8 *command,UINT8 UserBand);
INT32 AVL_DVBS2_Unicable_BlindScanAction(UINT16  nTunerCenter_100KHZ,UINT16  nFreqCenter_100KHZ,DVBS2_CHANNEL_T *pChannelList,UCHAR *Find_TP_num);
UINT8 AVL_DVBS2_Get_Signal_Info(VOID *SignalInfo);
#endif
