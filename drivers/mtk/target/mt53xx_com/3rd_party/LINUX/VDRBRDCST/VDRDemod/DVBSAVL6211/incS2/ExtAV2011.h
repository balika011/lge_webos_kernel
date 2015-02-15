#ifndef AV2011_h_h
#define AV2011_h_h

#include "II2CRepeater.h"
#include "ITuner.h"

#ifdef AVL_CPLUSPLUS
extern "C" {
#endif

#define AV2011_ENABLE_TIMEOUT
#define AV2011_TIMEOUT 100

#define  tuner_crystal 27 //unit is MHz
#define AV2011_Tuner
	
#define AV2011_R0_PLL_LOCK 0x01

/// AV2011_Setting: Stucture for AV2011 special setting
struct AV2011_Setting
{
	AVL_uchar AutoGain;
	AVL_uchar Single_End;
	AVL_uchar Auto_Scan;
	AVL_uchar RF_Loop;
};

enum AutoGain
{
	AV2011_AutoGain_OFF = 0, //FT_EN 0, fix LNA gain 
	AV2011_AutoGain_ON = 1 //FT_EN 0, LNA gain Auto control
};

enum Single_End
{
	AV2011_Differential = 0,  //IQ Differential mode
	AV2011_SingleEnd = 1    //IQ Single end mode
};

enum Auto_Scan
{
	Auto_Scan_OFF = 0,  //0 = normal manual lock mode
	Auto_Scan_ON = 1 //1 = blindscan search mode
};

enum RF_Loop
{
	RF_Loop_OFF = 0,  //0 = RF loop through off
	RF_Loop_ON = 1 //1 = RF loop through on
};

// time delay function ( minisecond )
AVL_DVBSx_ErrorCode AVL_DVBSx_ExtAV2011_Initialize(struct AVL_Tuner * pTuner);
AVL_DVBSx_ErrorCode AVL_DVBSx_ExtAV2011_GetLockStatus(struct AVL_Tuner * pTuner);
AVL_DVBSx_ErrorCode AVL_DVBSx_ExtAV2011_Lock( struct AVL_Tuner * pTuner);

#ifdef AVL_CPLUSPLUS
}
#endif
#endif
