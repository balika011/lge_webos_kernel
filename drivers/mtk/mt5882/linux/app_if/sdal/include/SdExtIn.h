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
 * @file SdExtIn.h
 * @brief Extern input header file
 * @author S/W Platform Lab
 * @date 2006/07/07
 */

#ifndef	_SDEXTIN_H_
#define	_SDEXTIN_H_

/********************************************************************
	INCLUDE FILES
********************************************************************/
#include "SdType.h"
#include "SdUtil.h"

/********************************************************************
	MACRO CONSTANT DEFINITIONS
********************************************************************/

/********************************************************************
	MACRO FUNCTION DEFINITIONS
********************************************************************/

/********************************************************************
	TYPE DEFINITION
********************************************************************/

typedef enum
{
	SD_EXTIN_STATUS_RES			= 0x0001,	///< Resolution
	SD_EXTIN_STATUS_SYNC_LOCK 	= 0x0002,	///< Lock Check
	SD_EXTIN_STATUS_MACRO 		= 0x0004, 	///< Macrovision	
	SD_EXTIN_STATUS_HDMI		= 0x0008, 	///< HDMI ���� 
	SD_EXTIN_STATUS_NOISE 		= 0x0010, 	///< CVBS Noise
	SD_EXTIN_STATUS_HDCP_ERR	= 0x0020, 	///< HDMI HDCP ERR Check
	SD_EXTIN_STATUS_COR_SYS		= 0x0040, 	///< Color System information
} SdExtIn_StatusMask_k;

typedef enum 
{ 
    SD_EXTIN_COR_SYS_AUTO,
    SD_EXTIN_COR_SYS_PAL,
    SD_EXTIN_COR_SYS_PAL60,
    SD_EXTIN_COR_SYS_PAL_M,
    SD_EXTIN_COR_SYS_PAL_N,
    SD_EXTIN_COR_SYS_SECAM,
    SD_EXTIN_COR_SYS_NT358,
    SD_EXTIN_COR_SYS_NT443,
    SD_EXTIN_COR_SYS_MAX
} SdExtIn_ColorSystem_k;

typedef enum 
{ 
	SD_HDMI_INFO_BASIC,
	SD_HDMI_INFO_FRAME_AVI, 
	SD_HDMI_INFO_FRAME_SPD, 
	SD_HDMI_INFO_FRAME_AUDIO, 
	SD_HDMI_INFO_FRAME_MPEG, 
	SD_HDMI_INFO_FRAME_UNREC,             
	SD_HDMI_INFO_FRAME_GAMUT, 
	SD_HDMI_INFO_FRAME_ACP,
} SdExtIn_HdmiInfoType_k;

typedef struct
{ 
	uint8_t generic[40]; 
} SdExtIn_HdmiInfo_t; 

typedef struct
{
	uint8_t		MV_ColourStrip3 	: 1,
				MV_ColourStrip2 	: 1,
				MV_PSP				: 1, ///< pseudo sync pulse
				MV_AGC				: 1, ///< automatic gain control
				MV_Reserved 		: 4;
} SdExtIn_MacrovisionType_t;

typedef struct 
{
	SdBool_t bHdmiEnable;			///< HDMI Enable ����. (DVI or HDMI)
	SdColorFormat_k eHdmiInputFormat; ///< HDMI Video Input Color Format. 
	SdAudioFsRate_k ePcmFsRate; ///< PCM Audio Sampling rate. It is valid as long as eHdmiAudio is SD_AUDIO_FORMAT_PCM.
	SdAudioFormat_k eHdmiAudio;		///< HDMI Audio Format.	
	SdColorDepth_k eHdmiColorDepth;	///< HDMI Color Depth	
} SdExtIn_HDMIStatus_t;

typedef struct
{
	uint32_t hResolution;	///< active horizontal resolution
	uint32_t vResolution;	///< active vertical resolution
	uint32_t hStart;		///< horizontal active resolution start
	uint32_t vStart;		///< vertical active resolution start
	uint32_t hTotal;		///< total horizontal resolution
	uint32_t vTotal;		///< total vertical resolution
	uint32_t hFreq;			///< horizontal frequency
	uint32_t vFreq;			///< vertical frequency (Hz * 1000)
	SdBool_t bProgressScan;	///< progress / interace 
	SdBool_t bHSyncPositive;///< horizontal sync polarity
	SdBool_t bVSyncPositive;///<vertical sync polarity
} SdExtIn_ResolutionInfo_t;

typedef struct
{
	SdBool_t bSyncLock; 				///< sync lock/unlock
	SdExtIn_MacrovisionType_t sMvType; 	///< Macrovision type	
	SdExtIn_HDMIStatus_t sHdmiStatus; 	///< HDMI status information
	uint16_t noiseValue; 				///< CVBS noise value
	SdExtIn_ResolutionInfo_t sResolutionInfo; ///< Resolution information	
	SdBool_t bHdcpError;				///< HDMI HDCP Error flag(True: Error, False: Ok)
	SdExtIn_ColorSystem_k eCorSys;      ///< Color System information(for DVB)
} SdExtIn_Status_t;

typedef struct
{
	uint32_t gainRed;	///< ADC�� Red Gain
	uint32_t gainGreen;	///< ADC�� Green Gain
	uint32_t gainBlue;	///< ADC�� Blue Gain
} SdExtIn_ADCRGBGain_t;

typedef struct
{
	uint32_t VSyncCount; 	///< condition for determining lock
	uint32_t checkTime; 	///< ms
} SdExtIn_LockCheck_t;

typedef struct
{
	SdExtIn_ResolutionInfo_t sResolution; ///< Input Resolution for H/W Setting
	SdBool_t bHDMIEnable; 				///< HDMI Enable�� ���� �ٸ� �κ� Setting. Setting for HDMI Enable
	SdColorFormat_k eHDMIInputFormat;	///< HDMI Video Input Color Format. Its value is usually the same with eHDMIInputFormat, the member of the sHdmiStatus structure.
	SdColorFormat_k eHDMIOutputFormat; 	///< HDMI Video Output Color Format. If you don't want to change HDMI input color format, its value should be always the same with eHDMIInputFormat.
	SdExtIn_ADCRGBGain_t sADCRGBGain; 	///< ADC RGB Gain
	uint32_t lnaLevel; 					///< LNA+(Low Noise Amplifier) Level(0~100)
	SdBool_t bHDMIBlackLow;				///< HDMI Black Level. Adjustment of brightness and contrast of the screen by controlling black level of HDMI RGB444 input signal. (True : Low(bypass), FALSE : Normal(compress))
	SdBool_t bHDMIClkEnable;            ///< HDMI Clk Termination Control (True : Clk Enable, False : Clk Disable)	
	SdExtIn_LockCheck_t sLockCheck; 	///< Lock Check Parameter
	SdBool_t bSetSDCSCTbl;            	///< HDMI PC Timing�� ���� �ܺ�Ĩ SD CSC table set flag
	SdExtIn_ColorSystem_k eCorSys;      ///< Color System information(for DVB)
} SdExtIn_Settings_t;

typedef enum
{
    SD_OUT_FORMAT_Y,
    SD_OUT_FORMAT_CB,
    SD_OUT_FORMAT_CR,
    SD_OUT_FORMAT_R,
    SD_OUT_FORMAT_G,
    SD_OUT_FORMAT_B,
} SdExtIn_OutFormat_k;

typedef enum
{
    SD_CTRL_VALUE_OFFSET,
    SD_CTRL_VALUE_GAIN,
} SdExtIn_ControlValue_k;

typedef struct
{
    uint32_t x;
    uint32_t y;
} SdExtIn_CapturePosition_t;

/********************************************************************
	EXTERN VARIABLES & FUNCTION PROTOTYPES DECLARATIONS
********************************************************************/
/**
 * @brief Initializes the SdExtIn module
 * @remarks 
 * @param ePowerState power off->on���� off->standby ���� standby->on������ �����Ѵ�. Power status (power off->on, off->standby or standby->on)
 * @code
 		typedef enum
		{
			SD_PWROFF_TO_NORMAL,
			SD_PWROFF_TO_STANDBY,
			SD_STANDBY_TO_NORMAL
		} SdPowerState_k;
 * @endcode
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdExtIn_Init(SdPowerState_k ePowerState);

/**
 * @brief �ܺ� �Է¿� ���õ� ������ �����Ѵ�. 
 It sets the contents pertaining to external inputs.
 * @remarks �����Ϸ��� ���� SdExtIn ���������� ����� ���� ���Ͽ� ���� �ٸ� �͸� �����Ѵ�.
 ���� ���ϴ� �͸� �����ϱ� ���ؼ��� SdExtIn_Get�� �̿��Ͽ� ���� ����� ���� �� �缳���Ϸ��� �͸� ���� �����Ͽ� ȣ���Ѵ�.
 As comparing the setting values and the values which are saved inside the SdWxtIn, it sets only the ones with different value. 
 Therefore in order to set the desired one only, by using the SdExtIn_Get, take the currently-saved values and then change and 
 call up only the values you want to re-set.
 * @code
 		SdExtIn_Settings_t sCurSetting;
 	        SdExtIn_Get(SD_WINDOW_MAIN, &sCurSetting);
 	   
 	        sCurSetting.eOutputformat = SD_HDMI_VIDEO_RGB;
 	        sCurSetting.bHDMIEnable = TRUE;
 	        ...
 	        
 	        SdExtIn_Set(SD_WINDOW_MAIN, &sCurSetting); 
 * @endcode
 * @param eExtInId �����Ϸ��� �ϴ� �ܺ� �Է� ID. ExtIn ID
 * @param pSettings �ܺ� �Է� ó�� ����. ExtIn settings
 * @code
 		typedef enum
		{
			SD_COLOR_FORMAT_RGB,
			SD_COLOR_FORMAT_YCbCr420,
			SD_COLOR_FORMAT_YCbCr422,
			SD_COLOR_FORMAT_YCbCr444,
			SD_COLOR_FORMAT_UNKNOWN,
		} SdColorFormat_k;
		
 		typedef struct
		{
			uint32_t gainRed;	///< ADC Red Gain
			uint32_t gainGreen;	///< ADC Green Gain
			uint32_t gainBlue;	///< ADC Blue Gain
		} SdExtIn_ADCRGBGain_t;

		typedef struct
		{
			SdBool_t bHDMIEnable; ///< HDMI Enable�� ���� �ٸ� �κ� Setting. Setting for HDMI Enable
			SdExtIn_HDMIVideoFormat_k eOutputFormat; ///< HDMI Video Output Color Format
			SdExtIn_ADCRGBGain_t sADCRGBGain; ///< ADC RGB Gain
			uint32_t lnaLevel;	///< LNA+(Low Noise Amplifier) Level(0~100)
		} SdExtIn_Settings_t;
 * @endcode
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdExtIn_Set(SdExtInID_k eExtInId, SdExtIn_Settings_t *pSettings);

/**
 * @brief ���� �ܺ� �Է� ó���� ������ ������ ���� �Լ�
 The function to take the values which are set to process the current external inputs.
 * @remarks 
 * @param eExtInId �������� ���Ϸ��� �ܺ� �Է��� ID. ExtIn ID
 * @param pSettings [out]���� �ܺ� �Է� ó�� ����. ExtIn settings 
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdExtIn_Get(SdExtInID_k eExtInId, SdExtIn_Settings_t *pSettings);

/**
 * @brief �ܺ� �Է��� ���¸� ���� �Լ�
 The function to take the status of external inputs.
 * @remarks �ܺ� �Է°� ���õ� ���¸� ������ ������ �� ������ ���������� �ʴ´�.
 mask parameter�� �̿��Ͽ� ���⸦ ���ϴ� ���� ������ �� �ִ�. 
 mask�� or�� �̿��Ͽ� ���� ���� ���� �� �ִ�. 
 ���������� �����ϴ� ����ü �� mask�� �����Ͽ� �� ���� ��ȿ�ϴ�.
 Taking the status pertaining to the external inputs, it does not set these values. 
 By using the mask parameter, it is available to set the ones you want to take. 
 Mask can get several values by using 'or'. Among the structures which save the status information, 
 only the values which are taken as being designated by mask are valid.
 * @param eExtInId ���������� �������� �ϴ� �ܺ� �Է��� ID. ExtIn ID
 * @param mask ���� ������ ��Ÿ���� ����ü���� ���Ϸ��� ������ ��Ÿ���� ����. Mask of status
 * @code
 		typedef enum
		{
			SD_EXTIN_STATUS_RES			= 0x0001,	///< Resolution
			SD_EXTIN_STATUS_SYNC_LOCK 	= 0x0002,	///< Lock Check
			SD_EXTIN_STATUS_MACRO 		= 0x0004, 	///< Macrovision	
			SD_EXTIN_STATUS_HDMI		= 0x0008, 	///< HDMI status
			SD_EXTIN_STATUS_NOISE 		= 0x0010 	///< CVBS Noise
		} SdExtIn_StatusMaskType_k;
 * @endcode 
 * @param pStatus [out]���� �о�� �ܺ� �Է��� ���·� ������ ���� ������ �ִ�.
 sync lock detection ����, Macrovision ���� ����, 
 HDMI ����, CVBS Noise ����, Resolution ������ ���� data�� �ִ�.
 mask�� ���� ������ ������ ��ȿ�ϴ�.
 [out] current status of external input. The following status is available.
 sync lock detection, Macrovision status, HDMI info, CVBS Noise Info, Resolution Info
 
 * @code 
		typedef struct
		{
			uint8_t		MV_ColourStrip3 	: 1,
						MV_ColourStrip2 	: 1,
						MV_PSP				: 1, ///< pseudo sync pulse
						MV_AGC				: 1, ///< automatic gain control
						MV_Reserved 		: 4;
		} SdExtIn_MacrovisionType_t;

		typedef enum
		{
			SD_COLOR_FORMAT_RGB,
			SD_COLOR_FORMAT_YCbCr420,
			SD_COLOR_FORMAT_YCbCr422,
			SD_COLOR_FORMAT_YCbCr444,
			SD_COLOR_FORMAT_UNKNOWN,
		} SdColorFormat_k;

		typedef enum 
		{	
			SD_AUDIO_FS_NONE = 0,	 	
			SD_AUDIO_FS_44KHZ = 1,	 
			SD_AUDIO_FS_48KHZ = 2,
			SD_AUDIO_FS_32KHZ = 3,
			SD_AUDIO_FS_22KHZ = 4,
			SD_AUDIO_FS_24KHZ = 6,
			SD_AUDIO_FS_88KHZ = 8,
			SD_AUDIO_FS_96KHZ = 10,
			SD_AUDIO_FS_176KHZ = 11,
			SD_AUDIO_FS_192KHZ = 14,
		} SdAudioFsRate_k;

		typedef struct 
		{
			SdBool_t bHdmiEnable;			///< HDMI Enable ����. (DVI or HDMI)
			SdColorFormat_k eHdmiInputFormat; ///< HDMI Video Input Color Format. 
			SdAudioFsRate_k ePcmFsRate; ///< PCM Audio Sampling rate. It is valid as long as eHdmiAudio is SD_AUDIO_FORMAT_PCM.
			SdAudioFormat_k eHdmiAudio;		///< HDMI Audio Format.
		} SdExtIn_HDMIStatus_t;

		typedef struct
		{
			uint16_t hResolution;	///< active horizontal resolution
			uint16_t vResolution;	///< active vertical resolution
			uint16_t hStart;	///< horizontal active resolution start
			uint16_t vStart;	///< vertical active resolution start
			uint16_t hTotal;	///< total horizontal resolution
			uint16_t vTotal;	///< total vertical resolution
			uint32_t hFreq;	///< horizontal frequency
			uint32_t vFreq;	///< verticla frequency
			SdBool_t bProgressScan;	///< progress / interace 
			SdBool_t bHSyncPositive;	///< horizontal sync polarity
			SdBool_t bVSyncPositive;	///<vertical sync polarity
		} SdExtIn_ResolutionInfo_t;

		typedef struct
		{
			SdBool_t bSyncLock; 				///< sync lock/unlock
			SdExtIn_MacrovisionType_t sMvType; 	///< Macrovision type	
			SdExtIn_HDMIStatus_t sHdmiStatus; 	///< HDMI status information
			uint16_t noiseValue; 				///< CVBS noise value
			SdExtIn_ResolutionInfo_t sResolutionInfo; ///< Resolution information
		} SdExtIn_Status_t;
 * @endcode
 * @param timeout
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdExtIn_GetStatus(SdExtInID_k eExtInId, SdMask_t mask, SdExtIn_Status_t *pStatus, uint32_t timeout);

/**
 * @brief �ܺ� �Է� path�� �����ϴ� �Լ�
 ExtIn component�� �ٸ� component��� �����Ѵ�. 
 �ܺ� �Է°� ����� �� �ִ� component�δ� ATV�� ���� SdTuner, SdVideo�̰� �� ���� �ܺ��Է� source�� ���� SdVideo�� �ִ�.
 It connects the ExtIn component to the other components. As the components to be connected to the external inputs, 
 it is SdTuner and SdVideo for ATV, and for the rest external input source, it is SdVideo.  
 * @remarks TS ���� �Է��� ������ ATV, CVBS, Component, HDMI, PC �� �ܺ� �Է��� path�� �����Ѵ�.
 It sets the path of the external inputs like ATV, CVBS, Component, HDMI and PC etc. except for the TS-related inputs.
 * @param eExtInId path ������ ���Ͽ� �����ϰ��� �ϴ� �ܺ� �Է��� ID. ExtIn ID
 * @code
		 typedef enum
		{
			SD_EXTIN0,
			SD_EXTIN1,
			SD_EXTIN_MAX
		} SdExtInID_k;
 * @endcode
 * @param eInputSrc �ܺ� �Է� Component�� ����Ǵ� Input Source. Input Source connected to ExtIn
 * @param eOutVideoId �ܺ� �Է� Component�� ��°� ����Ǵ� Video Component ID. SdVideo ID connted to ExtIn
 * @code
		 typedef enum
		{
			SD_VIDEO0, 
			SD_VIDEO1,
			SD_VIDEO_MAX
		} SdVideoID_k;
 * @endcode
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdExtIn_Connect(SdExtInID_k eExtInId, SdSource_k eInputSrc, SdVideoID_k eOutVideoId);

/**
 * @brief �ܺ� �Է� path�� ������ �����ϴ� �Լ�
 The function to release the settings of external input path.
 * @remarks Connect�� ���� ����� �ܺ� �Է� Component�� ������ �����Ѵ�.
 It releases the connection of external input Components which are connected through the Connect.
 * @param eExtInId path ������ �����ϰ��� �ϴ� �ܺ� �Է��� ID
 * @code
		 typedef enum
		{
			SD_EXTIN0,
			SD_EXTIN1,
			SD_EXTIN_MAX
		} SdExtInID_k;
 * @endcode
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdExtIn_Disconnect(SdExtInID_k eExtInId);

/**
 * @brief �ܺ� PC�Է��� �����ϴ� �Լ�
 The function to set the external PC inputs.
 * @remarks �����Ϸ��� ���� SdExtIn ���������� ����� ���� ���Ͽ� ���� �ٸ� �͸� �����Ѵ�.
 ���� ���ϴ� �͸� �����ϱ� ���ؼ��� SdExtIn_PCGet�� �̿��Ͽ� ���� ����� ���� �� �缳���Ϸ��� �͸� ���� �����Ͽ� ȣ���Ѵ�.
 Comparing the values to be set and the values which are saved inside SdWxtIn, it sets only the ones with different value.
 Therefore, in order to set the desired ones only, By using the SdExtIn_PCGet, get the currently-saved values and change 
 and call up only the values you want to re-set.
 * @code
 	      SdExtIn_PCGet(&sPCSetting);
 	      
 	      sPCSetting.frequency = 70;
 	      sPCSetting.sPCPosition.x = 0;
 	      ...
 	      
 	      SdExtIn_PCSet(&sPCSetting);
 * @endcode
 * @param eExtInId PC �Է¿� ���� �ܺ� �Է��� ID. ExtIn ID for PC Input
 * @param pSettings Desired the PC input property settings
 * @code
 		typedef struct
		{
			uint32_t x;
			uint32_t y;
			uint32_t w;
			uint32_t h;
		} SdGeometry_t;

		typedef struct
		{
			uint16_t frequency;
			uint16_t phase;
			SdGeometry_t sPCPosition;
		} SdPCAdjustInfo_t;
 * @endcode
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdExtIn_PCSet(SdExtInID_k eExtInId, SdPCAdjustInfo_t *pSettings);

/**
 * @brief �ܺ� PC�Է� ������ Get�ϴ� �Լ�
 The function to get the external PC input settings.
 * @remarks 
 * @param eExtInId PC �Է¿� ���� �ܺ� �Է��� ID. ExtIn ID for PC Input
 * @param pSettings [out]Current settings of the PC input property
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdExtIn_PCGet(SdExtInID_k eExtInId, SdPCAdjustInfo_t *pSettings);

/**
 * @brief HDMI�� �����Ͽ� HDCP Key�� Write�Ѵ�.
 The function to write a HDCP Key into a HDCP EEPROM.(Optional)
 * @remarks 
 * @param pBuf 
 * @param size 
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdExtIn_WriteHDCPKey(uint8_t *pBuf, uint32_t size);

/**
 * @brief HDMI Info Frame data�� ���´�.
 The function to get HDMI Info Frame data.
 * @remarks 
 * @param eExtInId 
 * @param eInfoType
 * @param pInfo 
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdExtIn_GetHdmiInfo(SdExtInID_k eExtInId, SdExtIn_HdmiInfoType_k eInfoType, SdExtIn_HdmiInfo_t *pInfo);

/**
 * @brief Calibration �˰����� �����ϱ� ���ؼ� �ش� Data�� ���´�.
 * @remarks
 * @param eInputSrc
 * @param eOutputFormat
 * @param sCapturePos
 * @param pData
 * @return SD_OK/SD_NOT_OK
 * @see 
 */      
SdResult SdExtIn_GetFirstCalibRegion(SdSource_k eInputSrc, SdExtIn_OutFormat_k eOutputFormat, SdExtIn_CapturePosition_t sCapturePos, uint16_t* pData);

/**
 * @brief Calibration �˰����� �����ϱ� ���ؼ� �ش簪�� �����Ѵ�.
 * @remarks
 * @param eInputSrc
 * @param eOutputFormat
 * @param eGainOffset
 * @param Data
 * @return SD_OK/SD_NOT_OK
 * @see 
 */ 
SdResult SdExtIn_SetFirstCalibValue(SdSource_k eInputSrc, SdExtIn_OutFormat_k eOutputFormat, SdExtIn_ControlValue_k eGainOffset, uint16_t Data);
        
/**
 * @brief Calibration �˰����� �����ϱ� ���ؼ� �ش� Data�� ���´�.
 * @remarks
 * @param eInputSrc
 * @param eOutputFormat
 * @param sCapturePos
 * @param pData
 * @return SD_OK/SD_NOT_OK
 * @see 
 */ 
SdResult SdExtIn_GetSecondCalibRegion(SdSource_k eInputSrc, SdExtIn_OutFormat_k eOutputFormat, SdExtIn_CapturePosition_t sCapturePos, uint16_t* pData);
       
/**
 * @brief Calibration �˰����� �����ϱ� ���ؼ� �ش簪�� �����Ѵ�.
 * @remarks
 * @param eInputSrc
 * @param eOutputFormat
 * @param eGainOffset
 * @param Data
 * @return SD_OK/SD_NOT_OK
 * @see 
 */ 
SdResult SdExtIn_SetSecondCalibValue(SdSource_k eInputSrc, SdExtIn_OutFormat_k eOutputFormat, SdExtIn_ControlValue_k eGainOffset, uint16_t Data);


        
/**
 * @brief SdExtIn API�� Manual Debug �Լ�
 The Manual Debug function of SdExtIn API
 * @remarks SdDebug.h�� ���ǵ� SdDebug()�� ���� ȣ��ȴ�. It is called by SdDebug() in SdDebug.h. 
 * @return 
 * @see 
 */
void SdExtIn_Debug(void);

#endif

