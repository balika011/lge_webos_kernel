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
/**
 * @file SdSound.h
 * @brief Sound header file\n
 Sound ������Ʈ�� Sound Post Chip�ܿ� Amp�� Speaker ��θ� ����. decoder�� speaker ���̿� �ִ� post-processing unit ����� �ϱ⵵ ������ Ư�� �Ƴ��α��� ��쿡�� MTS ����� �����Ͽ� Decoder�� �Ϻΰ� ���Ե�.
MSP Chip�� Sound�� ���� �� chip�鿡 ���� ������ ����Ѵ�. Ư��Ĩ������ ���� ������ �κ��� SdMisc���� ���� �Ѵ�.
SdSound_Init, SdSound_Connect, SdSound_Get, SdSound_Set, SdSound_GetStatus,SdSound_MuteAmp, SdSound_MuteSpeaker, SdSound_PlayBeeper�� 8�� �Լ��� �����Ǿ� �ִ�. 
AV Decoding�� ���� API�� SdAVDec�� �����Ǿ� �ִ�. Audio Format�� ���� Decorder�� ������ SdAVDec_Set���� Audio Foramt�� ���������� �����ϴ�. 
API ȣ������� SdAVDec_Set() -> SdAVDec_DecorderConnect() �̴�. 
SDAL������ Sound Data�� ��� ��Ʈ�� �������Ĵ� ������� �ʰ�, SDAL������ ���� Source�� ��������, Format�� �������� Ȯ���ؼ� ���ڵ��� �����Ѵ�. 

Sound Component contains not only Sound Post Chip, but also Amp and Speaker.
The function includes post-processing between decoder and speaker, it also includes a part of decoder, MTS detector, for analog signal.
It takes the charge of setting the sound-related chips like MSP Chips etc.  Functions for specific chip are implemented in SdMisc.
It consists of 8 functions such as SdSound_Init, SdSound_Connect, SdSound_Get, SdSound_Set, SdSound_GetStatus, SdSound_MuteAmp, SdSound_MuteSpeaker, SdSound_PlayBeeper.
APIs for AV Decoding is implemented in SdAVDec.
Decoder Audio Format is specified in SdAVDec_Set().
The calling sequence is SdAVDec_Set() -> SdAVDec_DecorderConnect().
SDAL decodes Sound Data using the type of Source and its format. It does not use which port the data comes from.

 * @author S/W Platform Lab
 * @date 2006/07/11
 */

#ifndef	_SDSOUND_H_
#define	_SDSOUND_H_

/********************************************************************
	INCLUDE FILES
********************************************************************/
#include "SdType.h"
#include "SdMisc.h"

/********************************************************************
	MACRO CONSTANT DEFINITIONS
********************************************************************/
#define		MAX_EQUALIZER			(20)
#define		MIN_EQUALIZER			(0)

#define 	MAX_VOLUME 				(100)
#define 	MIN_VOLUME 				(0) 	

#define 	MAX_BALANCE 			(20)
#define 	MIN_BALANCE 			(0) 	

/********************************************************************
	MACRO FUNCTION DEFINITIONS
********************************************************************/

/********************************************************************
	TYPE DEFINITION
********************************************************************/
typedef enum
{
	SD_SOUND_STEP0, 
	SD_SOUND_STEP1, 
	SD_SOUND_STEP2,
	SD_SOUND_STEP3,
	SD_SOUND_STEP4,
}SdSound_Step_k;

typedef enum
{
	SD_MTS_MONO			= 0x0,
	SD_MTS_STEREO		= 0x1,
	SD_MTS_FOREIGN 		= 0x2,
	SD_MTS_SAP 			= 0x2,
	SD_MTS_DUAL			= 0x3,
	SD_MTS_STEREO_SAP	= 0x3,

	SD_MTS_NICAM_STEREO	= 0x11,
	SD_MTS_NICAM_MONO,
	SD_MTS_NICAM_DUAL1,
	SD_MTS_NICAM_DUAL2,
	SD_MTS_INVALID 		= SD_INVALID
} SdSound_MtsMode_k;

typedef enum
{
    SD_AUDIO_ATV_UNKNOWN = 0,
    SD_AUDIO_ATV_BTSC,            ///< TV-Sys: M/N, Audio Modulation: BTSC-Stereo + SAP
    SD_AUDIO_ATV_EIAJ,              ///< TV-Sys: M/N, Audio Modulation: FM-FM (EIAJ)
    SD_AUDIO_ATV_A2_KOREA,  ///< TV-Sys: M/N, Audio Modulation: FM-Stereo
    SD_AUDIO_ATV_BG_A2,         ///< TV-Sys: B/G, Audio Modulation: FM-A2
    SD_AUDIO_ATV_DK1_A2,        ///< TV-Sys: D/K, Audio Modulation: FM-A2 (D/K1) 
    SD_AUDIO_ATV_DK2_A2,        ///< TV-Sys: D/K, Audio Modulation: FM-A2 (D/K2) 
    SD_AUDIO_ATV_DK3_A2,        ///< TV-Sys: D/K, Audio Modulation: FM-A2 (D/K3) 
    SD_AUDIO_ATV_I,                    ///< TV-Sys: I,   Audio Modulation: FM-Mono/NICAM
    SD_AUDIO_ATV_BG,                ///< TV-Sys: B/G, Audio Modulation: FM-Mono/NICAM
    SD_AUDIO_ATV_DK,                ///< TV-Sys: D/K, Audio Modulation: FM-Mono/NICAM (D/K)
    SD_AUDIO_ATV_L,                    ///< TV-Sys: L,   Audio Modulation: AM-Mono/NICAM
    SD_AUDIO_ATV_NUM
}SdSound_SoundSys_k; 

typedef enum
{
	SD_TSXT_OFF		= 0,
	SD_TSXT_STEREO,
	SD_TSXT_3D_MONO,
	SD_TSXT_MAX,
	SD_TSXT_INVALID = SD_INVALID
} SdSound_TsxtMode_k;

typedef enum
{
	SD_AMP_GAIN_0,
	SD_AMP_GAIN_1,
	SD_AMP_GAIN_2,
	SD_AMP_GAIN_3,
	SD_AMP_GAIN_MAX,
	SD_AMP_GAIN_INVALID = SD_INVALID
} SdSound_AmpGainLevel_k;

typedef enum
{
	SD_MELODY_POWER_OFF,
	SD_MELODY_POWER_ON,
	SD_MELODY_POWER_OFF_NOGAIN,
	SD_MELODY_POWER_ON_NOGAIN,
	SD_MELODY_FAST_POWER_ON,
	SD_MELODY_FAST_POWER_ON_NOGAIN
} SdSound_MelodyMode_k;

typedef enum
{
	SD_SOUND_STATUS_ANALOGMTS	= 0x01,
	SD_SOUND_STATUS_GETAMPGAIN 	= 0x02,
	SD_SOUND_STATUS_PLAYAUI		= 0x04,
} SdSound_StatusMask_k;

typedef enum
{
	SD_SUBWOOFER_LOUD_CHANNEL	= 0x00,	///< loudspeaker channel unfiltered
	SD_SUBWOOFER_COMP_HIGHPASS	= 0x01,	///< a complementary high-pass is processed in the loudspeaker output channel
	SD_SUBWOOFER_MICRONAS_BASS	= 0x02,	///< Micronas BASS added onto main channel
	SD_SUBWOOFER_INVALID		= SD_INVALID
} SdSound_SubwooferFilterType_k;

typedef enum
{
	SD_DNSE_NONE,
	SD_DNSE_WIDE_STEREO,
	SD_DNSE_VIRTUAL_SPEAKER,
	SD_DNSE_VIRTUAL_HEADPHONE,
} SdSound_DNSeMode_k;

typedef enum
{
	SD_PLAY_AUI_INIT,
	SD_PLAY_AUI_ING,
	SD_PLAY_AUI_DONE,
} SdSound_PlayAUI_State_k;

typedef enum
{
	SD_MAIN_STEREO_SUB_STEREO,
	SD_MAIN_MIXED_SUB_STEREO,
	SD_MAIN_STEREO_SUB_MIXED,
	SD_MAIN_MIXED_SUB_MIXED,	
} SdSound_AdPath_k; ///< Sound ��� ����� AD-Mixed�� Stereo�� �ΰ� ������� ���� ���, Main/Sub Speaker�� ��θ� ������. (Main - Speaker and Aux,  Sub - headset)

typedef struct
{ 
	uint32_t eqBand100Hz;	///<100Hz loudness(dB)
	uint32_t eqBand300Hz;	///<300Hz loudness(dB)
	uint32_t eqBand1KHz;	///<1KHz loudness(dB)
	uint32_t eqBand3KHz;	///<3KHz loudness(dB)
	uint32_t eqBand10KHz;	///<10KHz loudness(dB)
} SdSound_EqLevel5_t;  ///<Band�� center frequency,  Band�� Frequency���� ������ VD����ο��� ������ SPEC ��. Center Frequence for each Band. The frequencies are specified in VD SPEC.

typedef struct
{ 
	uint32_t eqBand100Hz;	///<100Hz loudness(dB)
	uint32_t eqBand500Hz;	///<500Hz loudness(dB)
	uint32_t eqBand1KHz;	///<1KHz loudness(dB)
	uint32_t eqBand2KHz;	///<2KHz loudness(dB)
	uint32_t eqBand4KHz;	///<4KHz loudness(dB)
	uint32_t eqBand8KHz;	///<8KHz loudness(dB)
	uint32_t eqBand15KHz;	///<15KHz loudness(dB)
} SdSound_EqLevel7_t;  

typedef struct
{
	uint32_t volume;		///<Step100(0~100)���� ������ ���� Gain�� ������ �� �ֵ��� �ؾ� ��. Range of volume is Step100 (0~100)
	uint32_t cornerFreq;	///<50~200 Hz
	SdSound_SubwooferFilterType_k eFilterType;
} SdSound_WooferSet_t;

typedef struct
{
	uint32_t volume;  				///<Step100(0~100)���� ������ ���� Gain�� ������ �� �ֵ��� �ؾ� ��. Range of volume is Step100 (0~100)
	uint32_t balance;				///<Step20(0~20) : 20(left -127dB right 0dB), ... 15(...., right 0dB), ... 10(both 0dB), ... 5(left 0dB, ...), ...0(left 0dB, right -127dB)
									
	uint32_t adVolume;              ///< volume for Audio Description. same step as volume. (only for DVB)
	SdSound_AdPath_k eAdPath;		///< path for Audio Description. (only for DVB)
	
	uint32_t speakerDelay;			///<Speaker Sound delay (ms)
	uint32_t headsetDelay;			///<HeadSet Sound delay (ms)
	uint32_t auxAnalogDelay;		///<Auxout Analog Output(L/R) delay (ms)
	uint32_t auxSpdifDelay;			///<Auxout Spdif Output delay (ms)
	SdBool_t bAutoVolume;			///<True = ON
	SdBool_t bBBE;					///<Used when supported 	
	SdSound_DNSeMode_k eDNSeMode;
	SdSound_MtsMode_k eOutputAnalogMts; ///<���õ� ��(Mono�Է��� �ƴ� ��� �Է� MTS�� ���� ���� ������ ������.)
	SdSound_TsxtMode_k eTsxtMode;
	SdBool_t bSupportEQ7; ///< True : used Eq7, False: used Eq5
	union{
	SdSound_EqLevel5_t sEqLevel5;	///<5Level EQ
	SdSound_EqLevel7_t sEqLevel7;	///<7Level EQ
	}uEQ;
	SdSound_AmpGainLevel_k eAmpGainLevel;
	SdSound_WooferSet_t sWooferSet;
	SdBool_t bExtSpdifIn;			///<External Sound Source flag : True - Spdif Sound Input, False - L/R Sound Input
} SdSound_Settings_t;

typedef struct
{
	SdSound_MtsMode_k eInputAnalogMts; ///<�Է� Analog MTS ��.
	SdSound_AmpGainLevel_k eAmpGainLevel;
	SdSound_PlayAUI_State_k eAUIState; ///< Play AUI Status
} SdSound_Status_t;

typedef void (*SdSound_CallBackCapture)(uint8_t *pBuffer, uint32_t size, uint32_t param);

/********************************************************************
	EXTERN VARIABLES & FUNCTION PROTOTYPES DECLARATIONS
********************************************************************/

/**
 * @brief Sound ���� ����̽� �� �ڿ��� �ʱ�ȭ �Ѵ�.
 It initialize the Sound-related device and resource.
 * @remarks 
 * @param step Sound �ʱ�ȭ Step, ��ǰ ���⸶�� �ٸ� �� �ִ�. Initialization step for Sound Component depending on the sound chip vendor
 * @param ePowerState
 * @code
 	typedef enum
	{
		SD_SOUND_STEP0, 
		SD_SOUND_STEP1,
		SD_SOUND_STEP2,
		SD_SOUND_STEP3,
		SD_SOUND_STEP4,
	}SdSound_Step_k;

	typedef enum
	{
		SD_PWROFF_TO_NORMAL,
		SD_PWROFF_TO_STANDBY,
		SD_STANDBY_TO_NORMAL
	} SdPowerState_k;
 * @endcode
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
*/
SdResult SdSound_Init(SdSound_Step_k step, SdPowerState_k ePowerState);

/**
 * @brief Sound Component�� Sound Component �մܰ��� ������ �����ϴ� �Լ�.Sound �����  Ȯ�强�� ����Ͽ� ���� �� 
 The function to set the connection between Sound Component and the front end of Sound Component.
 * @remarks Sound ����� �ϳ��� �Է¼ҽ����� �������� Sound�� ���ÿ� ������ �� �����Ƿ� Sound ���� �Է� �ҽ��� ã�� �����ϴ� ������� Connect�� �����Ѵ�. 
 Since it is available to connect several sounds at one input source simultaneously, connect it in the way of finding out the input source in the Sound.
 * ���������� �Ʒ��� ����. Sample combination
 * Digital : eSrc, eInput
 * Analog : eSrc, X(Don't Care������, ����ϴ� ExtIn�� ���缭 �����Ѵ�. Don't Care but one defined in ExtIn)
 * @param eSoundId �����ϰ��� �ϴ� Sound Component ID. SD_SOUND0,  SD_SOUND1 �� �ΰ����� ���� ��.
 *    ���� Sound chip�� ������ �� �߰� �ȴٸ� SD_SOUNDn �� ���� �ɰ��̸�, �ϳ��� �ҽ��Է¿� ���� Sound ����� ���ÿ� ������(���������� ��: Speaker, Headphone ��)���� �� ������.
 Sound Component ID to be used. Two IDs are defined: SD_SOUND0, SD_SOUND1 and can be extended to SD_SOUNDn if more sound chips are connected.
 A single source sound can be connected to several output simultaneous (both Speaker and Headphone)
 * @param eSrc �����ϰ��� �ϴ� Sound Source, Sound ��⿡�� Sound�� �ҽ��� �Ǵ��ϱ� ���ؼ� ���.�Ƴ��α��� ��쿡�� eSrc�����ε� ���� �ҽ��� �Ǵ��� �� ���� 
 Sound Source to be connected. For Analog Signal, eSrc is sufficient to find  the input component.
 * @param eInputId Sound Component�� �Է°� ����� Component�� ID. Sound ��⿡�� Sound�� �ҽ��� �Ǵ��ϱ� ���ؼ� ���Ǹ�, 
 *    �������� ��쿡�� eSrc�����δ� ��� Decoder�� ����Ͽ� Sound�� �������� �� �� ���� ������ ��� Decoder�� �˷��ִ� ����
 Input Component to be connected to the Sound Component.
 For Digital signal, Sound Component does not know by eSrc which decoder the signal comes from.
 * @return SdResult SD_OK/SD_NOT_OK
 * @see SdSound_Disconnect
 */
SdResult SdSound_Connect(SdSoundID_k eSoundId, SdSource_k eSrc, SdBackEndInID_k eInputId);

/**
 * @brief Sound Component�� ������ �����ϴ� �Լ� 
 The function of releasing the connection of Sound Component
 * @remarks 
 * @param eSoundId ������ �����ϰ��� �ϴ� Sound Component ID. Sound ID to be disconnected
 * @return SdResult SD_OK/SD_NOT_OK
 * @see SdSound_Connect
 */
SdResult SdSound_Disconnect(SdSoundID_k eSoundId);

/**
 * @brief Speaker�� � Sound�� ������ �Ǵ��� �����Ѵ�. 
  It sets which sound would be connected to Speaker. 
 * @remarks Speaker ����� �ϳ��� Speaker���� �������� Sound�� ���ÿ� ������ �� �����Ƿ� Speaker ���� ���� Sound�� ã�� �����ϴ� ������� Connect�� �����Ѵ�. 
 Since it is available to connect several sounds to one Speaker simultaneously, the connection will be made in the way of finding out the connected Sound in Speaker and connecting it.
 * @param eSpeakerId ���� �ϰ��� �ϴ� Speaker ID. Speaker ID to be connected
 * @param eSoundId Speaker�� ����� Sound ID. Sound ID connected to the speaker
  * @return SdResult SD_OK/SD_NOT_OK
 * @see SdSound_DisconnectSpeaker
 */
SdResult SdSound_SpeakerConnect(SdSpeakerID_k eSpeakerId, SdSoundID_k eSoundId);

/**
 * @brief Speaker�� Sound�� ���� ������ �����Ѵ�.
 It releases the connection settings between Speaker and Sound.
 * @remarks 
 * @param eSpeakerId ������ �����ϰ��� �ϴ� Speaker ID. Speaker ID to be disconnected
 * @return SdResult SD_OK/SD_NOT_OK
 * @see SdSound_ConnectSpeaker
 */
SdResult SdSound_SpeakerDisconnect(SdSpeakerID_k eSpeakerId);

/**
 * @brief Sound ���� �������� �о�´�.
 It reads in the Sound-related setting values.
 * @remarks
 * @param eSoundId
 * @param pSettings ���� setting �Ǿ� �ִ� Sound ������. current Sound setting
 * @code
	typedef struct
	{
		uint32_t volume;
		uint32_t balance;
		uint32_t soundDelay;
		uint32_t auxDelay;
		SdBool_t bAutoVolume;
		SdBool_t bBBE;
		SdSound_DNSeMode_k eDNSeMode;
		SdSound_MtsMode_k eMtsMode;
		SdSound_TsxtMode_k eTsxtMode;
		SdSound_EqLevel_t sEqLevel;
		SdSound_AmpGainLevel_k eAmpGainLevel;
		SdSound_WooferSet_t sWooferSet;
	} SdSound_Settings_t; 

	typedef enum
	{
		SD_MTS_MONO			= 0,
		SD_MTS_STEREO		= 1,
		SD_MTS_FOREIGN 		= 2,
		SD_MTS_SAP 			= 2,
		SD_MTS_DUAL			= 3,
		SD_MTS_STEREO_SAP	= 3,

		SD_MTS_NICAM_STEREO	= 1,
		SD_MTS_NICAM_DUAL,
		SD_MTS_NICAM_DUAL1,
		SD_MTS_NICAM_DUAL2,
		SD_MTS_INVALID 		= SD_INVALID
	} SdSound_MtsMode_k;

	typedef enum
	{
		SD_TSXT_OFF		= 0,
		SD_TSXT_STEREO,
		SD_TSXT_3D_MONO,
		SD_TSXT_MAX,
		SD_TSXT_INVALID = SD_INVALID
	} SdSound_TsxtMode_k;	

	typedef struct
	{
		uint32_t eqBand100Hz;
		uint32_t eqBand300Hz;
		uint32_t eqBand1KHz;
		uint32_t eqBand3KHz;
		uint32_t eqBand10KHz;
	} SdSound_EqLevel_t; 


	typedef enum
	{
		SD_AMP_GAIN_0,
		SD_AMP_GAIN_1,
		SD_AMP_GAIN_2,
		SD_AMP_GAIN_3,
		SD_AMP_GAIN_MAX,
		SD_AMP_GAIN_INVALID = SD_INVALID
	} SdSound_AmpGainLevel_k;

	typedef struct
	{
		uint32_t volume;
		uint32_t cornerFreq;
		SdSound_SubwooferFilterType_k eFilterType;
	} SdSound_WooferSet_t;

	typedef enum
	{
		SD_SUBWOOFER_LOUD_CHANNEL	 = 0x00,  
		SD_SUBWOOFER_COMP_HIGHPASS  = 0x01,  
		SD_SUBWOOFER_MICRONAS_BASS  = 0x02,  
		SD_SUBWOOFER_INVALID		= SD_INVALID
	} SdSound_SubwooferFilterType_k;
 * @endcode
 * @return SdResult SD_OK/SD_NOT_OK
 * @code
 		//////------SAMPLE CODE------//////
		dSound_Settings_t mSoundSettings;
		SdSound_Get(SD_SPEAKER_MAIN, &mSoundSettings);
		mSoundSettings.volume = bVolume;
		mSoundSettings.eAmpGainLevel = SD_AMP_GAIN_3
		......
		SdSound_Set(SD_SPEAKER_MAIN, &mSoundSettings);
 * @endcode
 * @see 
 */
SdResult SdSound_Get(SdSoundID_k eSoundId, SdSound_Settings_t *pSettings);

/**
 * @brief Sound ���� �������� �����Ѵ�.
 It sets the Sound-related setting values.
 * @remarks
 * @param eSoundId
 * @param pSettings setting�� Sound ������. new value of Sound setting
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdSound_Set(SdSoundID_k eSoundId, SdSound_Settings_t *pSettings);

/**
 * @brief Sound ���� Information�� �����´�.
 It takes the Sound-related Information.
 * @remarks 
 * @param eSoundId
  * @param mask
 * @code
	typedef enum
	{
		SD_SOUND_STATUS_ANALOGMTS	= 0x01,
		SD_SOUND_STATUS_GETAMPGAIN 	= 0x02,
	} SdSound_StatusMask_k;
 * @endcode
 * @param pState
 * @code
	typedef struct
	{
		SdSound_MtsMode_k eAnalogMtsMode;
		SdSound_AmpGainLevel_k eAmpGainLevel;
	} SdSound_Status_t;
 * @endcode
 * @return SdResult SD_OK/SD_NOT_OK
 * @code
	SdSound_Status_t SoundState;
	SdSound_GetStatus(SD_SPEAKER_MAIN, SD_SOUND_STATUS_GETAMPGAIN, &SoundState);
	gain = (AMP_GAIN_LEVEL)SoundState.eAmpGainLevel;
 * @endcode
 * @param timeout
 * @see 
 */
SdResult SdSound_GetStatus(SdSoundID_k eSoundId, SdMask_t mask, SdSound_Status_t *pState, uint32_t timeout);

/**
 * @brief Amp�� Mute ��Ų��.
 It makes the Amp muted.
 * @remarks AMP �� �Ƴ��α� ������ ������ ��ȣ�� �Ƴ��α׷� ��ȯ�ؼ� ����Ŀ�� ������ ���� ������ ���ִ� ����̴�.
 AMP converts the digital sound signal to analog, amplifies it and feed to the Speaker.
 * @param eSpeakerId
 * @param bMuteOn TRUE : Mute On, FALSE : Mute Off  
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdSound_AmpMute(SdSpeakerID_k eSpeakerId, SdBool_t bMuteOn);

/**
 * @brief ���� ��´��� Speaker ����� Mute ��Ų��.
 It makes the Speaker output muted.
 * @param eSpeakerId
 * @param bMuteOn TRUE : Mute On, FALSE : Mute Off   
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdSound_SpeakerMute(SdSpeakerID_k eSpeakerId, SdBool_t bMuteOn);

/**
 * @brief Power On/Off�� Beeper�� Melody�� ��½�Ų��.
 It outputs the Melogy of Beeper upon turning Power On/Off.
  Beeper���� SoundChip���� Frequency�� �����ؼ� ���� �Ҹ���, mp3�� PCM ������ ����ؼ� Beeper���� ����� ��쿡�� Decoder�� ������ �ʿ��ϱ� ������ �� API�ʹ� ������ �����Ǿ�� ��. 
  Beeper is produced by controlling sequence of frequencies in SoundChip. mp3 or PCM play requires connection of a audio decoder and should be implemented in another method.
  feeder�� �̿��� clip play�� ������ �Լ���(SdSound_PlayAUI) �����ȴ�.
  SdSound_PlayAUI implements the clip play with feeder.
  
 Beeper is implemented by managing Sound Chip frequencies.
 Beeper can also be implemented using mp3 or PCM clip through sound decoder and it is beyond the scope of this function.
 * @remarks Power On�� Frequency �� Delay : 512Hz 70ms, 656Hz 70ms , 768Hz 70ms , 656Hz 70ms , 768Hz 70ms ,1040Hz 180ms
 * Power Off��  Frequency �� Delay : 1312Hz 70ms, 1040Hz 70ms, 768Hz 70ms , 656Hz 70ms, 512Hz 180ms 
 * @param eSoundId
 * @param eMelodyMode Power On/Off ��Ȳ�� ���� Melody Mode
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdSound_PlayBeeper(SdSoundID_k eSoundId, SdSound_MelodyMode_k eMelodyMode);

/**
 * @brief Audio Data�� �÷����ϴ� UI�μ� ������ �Ǵ� ���������� �÷��̽�ų �� ����Ѵ�.
 It is Audio data playing UI and is used in start up and shut down melodies.
 * @param eSoundId
 * @param eFormat 
 * @param pBuf
 * @param size
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdSound_PlayAUI(SdSoundID_k eSoundId, SdAudioFormat_k eFormat, uint8_t *pBuf, uint32_t size);

SdResult SdSound_SetSifType(SdSoundID_k eSoundId, SdSound_SoundSys_k  eSoundTyp);



/**
 * @brief Speaker�� ����� PCM �����͸� Capture�Ͽ� Callback���� ���� ��⿡ ���� sample ���ݸ��� �÷��ش�.
 * @remarks
 * @param eSoundId
 * @param callback Callback function, uninstall�Ϸ��� callback�� NULL�� �ش�.
 * @param param Callback function �Լ� ȣ�� �� ���� ���޵� ����. parameters for the callback function call
 * @param period PCM sample ��. period ��ŭ�� sample�� Capture �Ǿ��� ������ Callback function�� ȣ��ǰ� �ȴ�.
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdSound_InstallCallbackPCMCapture(SdSoundID_k eSoundId, SdSound_CallBackCapture callback, uint32_t param, uint32_t period);

/**
 * @brief SdSound API�� Manual Debug �Լ�
 Manual Debug function of SdSound API
 * @remarks SdDebug.h�� ���ǵ� SdDebug()�� ���� ȣ��ȴ�. It is called by SdDebug() in SdDebug.h. 
 * @return 
 * @see 
 */
void SdSound_Debug(void);

/**
 * @brief get ATV sound system
 * @remarks should call this function before playing ATV or scanning channel
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdSound_GetSoundSystem(SdSoundID_k eSoundId, SdSound_SoundSys_k eDefaultSys, SdSound_SoundSys_k *peSoundSys);

#endif

