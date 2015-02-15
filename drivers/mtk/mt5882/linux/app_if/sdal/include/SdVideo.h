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
 * @file SdVideo.h
 * @brief Video header file
 * @author S/W Platform Lab
 * @date 2006/07/10
 */

#ifndef	_SDVIDEO_H_
#define	_SDVIDEO_H_

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
	
typedef struct
{
	uint32_t hResolution;	///< Horizontal Size for Scaling
	uint32_t vResolution;	///< Vertical Size for Scaling
	uint32_t hStart;	///< horizontal active resolution start point
	uint32_t vStart;	///< vertical active resolution start point
	uint32_t hTotal;	///< total horizontal resolution
	uint32_t vTotal;	///< total vertical resolution
	SdBool_t bProgressScan;	///< progress / interace identifier
	uint32_t frameRate;	///< Frame rate (Hz * 1000)
} SdVideo_VideoFormat_t;

typedef struct
{
	SdVideo_VideoFormat_t sCaptureSize;		///<scaler output capture size
	SdGeometry_t sGeometry;		///<geometry values(16:9,4:3,...)
	SdColor_t sBGColor;	///< BackGround Color(�� Plane���� Video Mute�� �������� Color, Color when the plane is mute)
	SdBool_t bVideoOn; 			///< Pip on/off
	SdBool_t bEnable32PullDown; 	///< 480i ��ȣ���� Film mode�� �����ϰ�, 3:2 Pull Down���¸� �����Ͽ� ����ϴ� ��� On/Off(���� IPC ��⿡�� ó����). Film mode on/off in 480i signal. If On, it detects film mode and enables 3:2 Pull Down mode (Generally processed in IPC module)
	SdBool_t bGameModeOn;		///< Game Mode Function On/Off.
	SdBool_t b3DGameModeOn;		///< 3D Game Mode Function On/Off.		
	uint32_t VideoDelay;		///< video output delay(ms)	
	SdColorFormat_k eColorFormat; 	///< scaler Input Color Format.
} SdVideo_Settings_t;

typedef struct
{
	uint64_t FrameSize; 	///<Capture Frame Size(byte)
	uint32_t Height;		///<Capture Frame Height 
	uint32_t Width;			///<Capture Frame Width
	SdColorFormat_k eFormat;///<Capture Frame Video Format 
	uint32_t Pts;			///<Capture Frame PTS
} SdVideo_CaptureInfo_t;

/********************************************************************
	EXTERN VARIABLES & FUNCTION PROTOTYPES DECLARATIONS
********************************************************************/

/**
 * @brief Video ��⿡ ���� ������ �ʱ�ȭ ������ �����Ѵ�. 
 It carries out the first initialization prodecure for the Video module.
 * @remarks
 * @param ePowerState power state
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdVideo_Init(SdPowerState_k ePowerState);

/**
 * @brief Video(Scaler)�� Setting �Լ�. Resolution, VideoSize �� Video�� ���õ� �������� Setting�Ѵ�. 
 It is the function to set the Video(Scaler). It sets the Resolution, VideoSize and Video-related information.
 * @remarks �����Ϸ��� ���� SdVideo�� �̹� �����Ǿ� �ִ� ���� ���Ͽ� ���� �ٸ� �͸� �����Ѵ�.
 ���� ���ϴ� �͸� �����ϱ� ���ؼ��� SdExtIn_Get�� �̿��Ͽ� ���� ����� ���� �� �缳���Ϸ��� �͸� ���� �����Ͽ� ȣ���Ѵ�.
 By comparing the values to be set and the values which have been set in SdVideo, only different value will be set. 
 Therefore, in order to set the desired ones, by using the SdExtIn_Get, get the currently-saved value and then cange and call up only the values which you want to re-set.
 * @param eVideoId Video Component ID
 * @code
		 typedef enum
		{
			SD_VIDEO0, 
			SD_VIDEO1,
			SD_VIDEO_MAX
		} SdVideoID_k;
 * @endcode
 * @param pSettings setting data
 * @code
 		typedef struct
		{
			uint32_t hResolution;	///< Horizontal Size for Scaling
			uint32_t vResolution;	///< Vertical Size for Scaling
			uint32_t hStart;	///< horizontal active resolution start point
			uint32_t vStart;	///< vertical active resolution start point
			uint32_t hTotal;	///< total horizontal resolution
			uint32_t vTotal;	///< total vertical resolution
			SdBool_t bProgressScan;	///< progress / interace identifier
			uint32_t frameRate;	///< Frame rate (Hz * 1000)
		} SdVideo_VideoFormat_t;
		
		typedef struct
		{
			SdVideo_VideoFormat_t sCaptureSize;		///<scaler output capture size
			SdGeometry_t sGeometry;		///<geometry values(16:9,4:3,...)
			SdColor_t sBGColor;			///< BackGround Color (letter box color)
			SdBool_t bVideoOn; 			///< scaler on/off
			SdBool_t bEnable32PullDown; 	///< film mode
		} SdVideo_Settings_t;
 * @endcode 
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdVideo_Set(SdVideoID_k eVideoId, SdVideo_Settings_t* pSettings);

/**
 * @brief Video ����� ������ �����´�. Resolution, VideoSize �� Video�� �����Ͽ� ����� �������� �����´�.
 It takes the information of Video module. It takes the information which is saved in regard with the Resolution, VideoSize and Video.
 * @remarks
 * @param eVideoId Video Component ID
 * @param pSettings [out]setting data
 * @code
		typedef struct
		{
			uint32_t hResolution;	///< Horizontal Size for Scaling
			uint32_t vResolution;	///< Vertical Size for Scaling
			uint32_t hStart;	///< horizontal active resolution start point
			uint32_t vStart;	///< vertical active resolution start point
			uint32_t hTotal;	///< total horizontal resolution
			uint32_t vTotal;	///< total vertical resolution
			SdBool_t bProgressScan;	///< progress / interace identifier
			uint32_t frameRate;	///< Frame rate (Hz * 1000)
		} SdVideo_VideoFormat_t;
 
		typedef struct
		{
			SdVideo_VideoFormat_t sCaptureSize;		///<scaler output capture size
			SdGeometry_t sGeometry;		///<geometry values(16:9,4:3,...)
			SdColor_t sBGColor;			///< BackGround Color (letter box color)
			SdBool_t bVideoOn; 			///< scaler on/off
			SdBool_t bEnable32PullDown; 	///< film mode
		} SdVideo_Settings_t;
 * @endcode 
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdVideo_Get(SdVideoID_k eVideoId, SdVideo_Settings_t* pSettings);

/**
 * @brief Video Component�� AVDec �Ǵ� ExtIn�� �����Ű�� �Լ� 
 The function to connect the Video Component to AVDec or ExtIn.
 * @remarks TS �Է��� ��� AVDec�� ���� Video�� ����ǰ� �������� ExtIn�� ���ļ� Video�� ���´�.
 For the TS input, it is connected to Video through the AVDec and the rest will come in to Video through the ExtIn.
 * @code
		SdVideo_Settings_t sVideoSettings;
		SdExtIn_Status_t sExtInStatus;
		...
		SdExtIn_Connect(SD_EXTIN0, SOURCE_COMP1, SD_VIDEO0);
		SdVideo_Connect(SD_VIDEO0, SD_BE_IN_EXTIN0, SD_WINDOW_MAIN);
 * @endcode
 * @param eVideoId �����ϰ��� �ϴ� Video Component ID. Video Component ID to be connected
 * @code
		 typedef enum
		{
			SD_VIDEO0, 
			SD_VIDEO1,
			SD_VIDEO_MAX
		} SdVideoID_k;
 * @endcode
 * @param eInputId Video Component�� �Է°� ����Ǵ� Component�� ID. Input Component ID to be connected to Video Component
 * @code
 		typedef enum
		{
			SD_VIDEO_IN_AVDEC0,	///< Decoder0
			SD_VIDEO_IN_AVDEC1,	///< Decoder1
			SD_VIDEO_IN_EXTIN0,	///< External Input0
			SD_VIDEO_IN_EXTIN1	///< External input1
		} SdVideoInID_k; 
 * @endcode
 * @param eOutputId ����� Video�� Display�� Window ID. Window ID to be displayed by the Video Component
 * @code
		typedef enum
		{
			SD_WINDOW_MAIN,
			SD_WINDOW_SUB0,
			SD_STREAM_TSOUT,
			SD_STREAM_RECORD0,
			SD_STREAM_RECORD1,	
			SD_WINDOW_MAX,
		} SdWindowID_k;
 * @endcode
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdVideo_Connect(SdVideoID_k eVideoId, SdBackEndInID_k eInputId, SdWindowID_k eOutputId);

/**
 * @brief Video Component�� AVDec �Ǵ� ExtIn���� ������ �����ϴ� �Լ� 
 The function to release the connection between Video Component and either AVDec or ExtIn.
 * @remarks
 * @param eVideoId ������ ���� �ϰ��� �ϴ� Video Component ID. Video Component ID to be disconnected
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
SdResult SdVideo_Disconnect(SdVideoID_k eVideoId);

/**
 * @brief Video�� Mute��Ų��.
 It makes Video muted. 
 * @remarks ȭ�� Mute�� �������� Background Color�� SdVideo_Set�� ���� �̸� �����Ѵ�.
            Background Color(sBGColor) should be set before SdVideo_Mute is called. 
 * @param eVideoId Video Component ID
 * @code
		 typedef enum
		{
			SD_VIDEO0, 
			SD_VIDEO1,
			SD_VIDEO_MAX
		} SdVideoID_k;
		
		typedef struct
		{
			SdVideo_VideoFormat_t sCaptureSize;		///<scaler output capture size
			SdGeometry_t sGeometry;		///<geometry values(16:9,4:3,...)
			SdColor_t sBGColor;	///< BackGround Color(�� Plane���� Video Mute�� �������� Color, Color when the plane is mute)
			SdBool_t bVideoOn; 			///< Pip on/off
			SdBool_t bEnable32PullDown; 	///< 480i ��ȣ���� Film mode�� �����ϰ�, 3:2 Pull Down���¸� �����Ͽ� ����ϴ� ��� On/Off(���� IPC ��⿡�� ó����). Film mode on/off in 480i signal. If On, it detects film mode and enables 3:2 Pull Down mode (Generally processed in IPC module)
			SdBool_t bIpcOn;		///< IPC(Interaced��ȣ�� Progressive��ȣ�� ��ȯ�����ִ� ���) On/Off. Interaced to Progressive Conversion(IPC) On/Off
			uint32_t VideoDelay;		///< video output delay(ms)	
		} SdVideo_Settings_t;
 * @endcode
 * @param bMuteOn mute On/Off. 
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdVideo_Mute(SdVideoID_k eVideoId, SdBool_t bMuteOn);

/**
 * @brief PC AutoAdjust���۽� Frequency(Coarse)�� Detect�Ѵ�.
 It detect coarse, while doing autoadjustment.
 * @remarks 
 * @param eVideoId Video Component ID
 * @param uPll Horizontal Total size
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdVideo_DetectAutoCoarse(SdVideoID_k eVideoId, uint32_t *uPll);

/**
 * @brief PC AutoAdjust���۽� Phase(Fine)�� Detect�Ѵ�.
 It detect Fine, while doing autoadjustment.
 * @remarks
 * @param eVideoId Video Component ID
 * @param uPhase Phase
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdVideo_DetectAutoFine(SdVideoID_k eVideoId, uint32_t *uPhase);

/**
 * @brief PC AutoAdjust���۽� x, y Postion�� ���Ѵ�.
 It get the position of x,y, while dong autoadjustment.
 * @remarks
 * @param eVideoId Video Component ID
 * @param pPostition
 * @return SD_OK/SD_NOT_OK
 * @see 
 */SdResult SdVideo_DetectAutoPostion(SdVideoID_k eVideoId, SdGeometry_t *pPostition);

/**
 * @brief �ܺ� �Է� �� PC input�� Video�� ����� ��� PC�� ȭ���� Video ȭ�鿡 �ڵ����� ���ߵ��� Auto Adjustment�� �����Ѵ�.
 frequency,phase,H/V position�� �����Ѵ�.
 It performs the Auto Adjustment so that the PC screen can be adjusted to the Video screen automatically 
 when a PC input is connected to Video while external input is going on.
 * @remarks ���� AutoAdjustment�� ExtIn�� Video�� ��ȣ �ۿ��� �ϸ鼭 ����Ǵµ�, �� �Լ��� �ϳ��� Video ��⿡�� ��� AutoAdjust��
 �����ϴ� ��� ȣ��ȴ�.
 Generally, AutoAdjust is performed by the interaction between ExtIn and Video components.
 This function is used when only Video Component performs AutoAdjust.
 * @code
		...
		SdVideo_DoAutoAdjust(sPathInfo.eVideoID, &sAutoAdjustInfo);
 * @endcode
 * @param eVideoId Video Component ID
 * @code
		 typedef enum
		{
			SD_VIDEO0, 
			SD_VIDEO1,
			SD_VIDEO_MAX
		} SdVideoID_k;
 * @endcode
 * @param pPcData [out] frequency, phase, geometry: the result of AutoAdjust
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
			uint32_t frequency;
			uint32_t phase;
			SdGeometry_t sPCPosition;
		} SdPCAdjustInfo_t;
 * @endcode 
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdVideo_DoAutoAdjust(SdVideoID_k eVideoId, SdPCAdjustInfo_t* pPcData);

/**
 * @brief Stop doing AutoAdjustment. AutoAdjust �۾��� �����Ѵ�.
 * @remarks
 * @param eVideoId Video Component ID
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdVideo_StopAutoAdjust(SdVideoID_k eVideoId);

/**
 * @brief Still(freeze) a video picture
 * @remarks
 * @param eVideoId Video Component ID
 * @code
		 typedef enum
		{
			SD_VIDEO0, 
			SD_VIDEO1,
			SD_VIDEO_MAX
		} SdVideoID_k;
 * @endcode
 * @param bStillOn still on/off
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdVideo_StillVideo(SdVideoID_k eVideoId, SdBool_t bStillOn);

/**
 * @brief ȭ���� Capture�Ѵ�.
 It captures a Frame Picture.
 * @remarks 
 * @param eVideoId Video Component ID
 * @param pDestination Capture�� Frame�� ������ �޸� �ּ�. address for storing the captured frame
 * @param pCaptureInfo Capture�� Frame�� ���� ����. information of the captured frame
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdVideo_CaptureFrame(SdVideoID_k eVideoId, uint8_t* pDestination, SdVideo_CaptureInfo_t *pCaptureInfo);

/**
 * @brief SdVideo API�� Manual Debug �Լ�
 Manual Debug function of SdVideo API
 * @remarks SdDebug.h�� ���ǵ� SdDebug()�� ���� ȣ��ȴ�. It is called by SdDebug() in SdDebug.h. 
 * @return 
 * @see 
 */
void SdVideo_Debug(void);

#endif //_SDVIDEO_H_

