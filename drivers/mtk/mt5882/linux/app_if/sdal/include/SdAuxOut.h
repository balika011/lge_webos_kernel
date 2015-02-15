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
 * @file SdAuxOut.h
 * @brief Auxiliary Audio/Video Output header file
 * @author S/W Platform Lab
 * @date 2006/07/11
 */

#ifndef	_SDAUXOUT_H_
#define	_SDAUXOUT_H_

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
	SD_VBI_DATA_CC = 0, ///< Closed Caption
	SD_VBI_DATA_XDS, ///< Extended Data Service
	SD_VBI_DATA_TTX, ///< Analog SdUserData is Teletext
	SD_VBI_DATA_WSS, ///< Analog SdUserData is WSS	
	SD_VBI_DATA_VPS, ///< Analog SdUserData is VPS - included in NTSC line 21 XDS
	SD_VBI_DATA_PDC, ///< Analog SdUserData is PDC
	SD_VBI_DATA_MAX
} SdAuxOut_VbiDataType_k; ///< VBI Data Type

typedef enum
{
	SD_MACROVISION_AGC = 1,	///< automatic gain control
	SD_MACROVISION_2L,	///< color stripe 2 line
	SD_MACROVISION_4L	///< color stripe 4 line
} SdAuxOut_MacrovisionType_k; ///< Macrovision Type

typedef enum
{
	SD_AUXAUDIO_ANALOG, ///< Aux analog audio output
	SD_AUXAUDIO_SPDIF, ///< Aux SPDIF audio output
	SD_AUXAUDIO_ALL,   ///< All Aux Audio output
} SdAuxOut_Audio_k;

/********************************************************************
	EXTERN VARIABLES & FUNCTION PROTOTYPES DECLARATIONS
********************************************************************/

/**
 * @brief Audio/Video ���� ��°� ���õ� �κ��� �ʱ�ȭ �Ѵ�.
 It initialized the part pertaining to the Audio/Video auxiliary outputs. 
 * @remarks
 * @param ePowerState power off->on���� off->standby ���� standby->on������ �����Ѵ�.
 * @code
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
SdResult SdAuxOut_Init(SdPowerState_k ePowerState);

/**
 * @brief ���� ����� ���� Video/Audio ��θ� �����Ѵ�.
 It connects the AuxOut component with the Input Source.
 * @remarks 
 * @param eAuxOutId
 * @param eSRC input source
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAuxOut_Connect(SdAuxOutID_k eAuxOutId, SdSource_k eSRC);
 
 /**
 * @brief ���� ����� ���� ���� ������ �����Ѵ�.
It releases the auxout connection settings.
 * @remarks 
 * @param eAuxOutId
 * @param eSRC input source
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAuxOut_Disconnect(SdAuxOutID_k eAuxOutId, SdSource_k eSRC);

/**
 * @brief Video ���� ��� (CVBS ���)�� �����Ѵ�.
 It starts the Video auxiliary outputs (CVBS outputs) 
 * @remarks  
 * @param eAuxOutId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAuxOut_VideoStart(SdAuxOutID_k eAuxOutId);

/**
 * @brief Video ���� ��� (CVBS ���)�� �����Ѵ�.
 It stops the Video auxiliary outputs (CVBS output).
 * @remarks 
 * @param eAuxOutId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAuxOut_VideoStop(SdAuxOutID_k eAuxOutId);

/**
 * @brief Audio ���� ��� (Analog Audio, SPDIF ���)�� �����Ѵ�.
 It starts the Audio auxiliary outputs (Analog Audio, SPDIF outputs)
 * @remarks
 * @param eAuxOutId 
 * @return SdResult SD_OK/SD_NOT_OK
 * @see SdAVDec_Settings_t
 */
SdResult SdAuxOut_AudioStart(SdAuxOutID_k eAuxOutId);

/**
 * @brief Audio ���� ��� (Analog Audio, SPDIF ���)�� �����Ѵ�.
 It stops the Audio auxiliary outputs (Analog Audio, SPDIF outputs)
 * @remarks 
 * @param eAuxOutId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAuxOut_AudioStop(SdAuxOutID_k eAuxOutId);

/**
 * @brief Video format.
 It sets the Video auxiliary outputs format(480i or 576i) 
 * @remarks  
 * @param eAuxOutId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAuxOut_VideoFmt(SdAuxOutID_k eAuxOutId, SdAuxOutFmt_k eAuxOutFmt);

/**
 * @brief VBI Data�� �����ϰ� �ִ� �Է� �Ķ���� buffer�κ��� Video ���� ������� VBI Data�� ������.
 It sends the VBI Data to the Video auxiliary outputs from the input parameter buffer which contains the vbi Data. 
 * @remarks Monitoring output�� VBI Data�� insert�Ѵ�.
 It inserts the VBI Data in the monitoring output.
 * @param eAuxOutId
 * @param eType Closed Caption or Extended Data Service or etc.
 * @param pData Data buffer pointer
 * @param length Data Length
 * @code
 		typedef enum
		{
			SD_VBI_DATA_CC = 0, ///< Closed Caption
			SD_VBI_DATA_XDS, ///< Extended Data Service
			SD_VBI_DATA_TTX, ///< Analog SdUserData is Teletext
			SD_VBI_DATA_WSS, ///< Analog SdUserData is WSS
		} SdAuxOut_VbiDataType_k; ///< VBI Data Type
 * @endcode
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAuxOut_SendVbiData(SdAuxOutID_k eAuxOutId, SdAuxOut_VbiDataType_k eType, uint8_t *pData, uint32_t length);

/**
 * @brief VBI Data
 It starts the VBI function of the Video auxiliary outputs. 
 * @remarks Monitoring output VBI 
 * @param eAuxOutId
 * @param eType Closed Caption or Extended Data Service or etc.
 * @code
 		typedef enum
		{
			SD_VBI_DATA_CC = 0, ///< Closed Caption
			SD_VBI_DATA_XDS, ///< Extended Data Service
			SD_VBI_DATA_TTX, ///< Analog SdUserData is Teletext
			SD_VBI_DATA_WSS, ///< Analog SdUserData is WSS
		} SdAuxOut_VbiDataType_k; ///< VBI Data Type
 * @endcode
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAuxOut_VbiStart(SdAuxOutID_k eAuxOutId, SdAuxOut_VbiDataType_k eType);

/**
 * @brief VBI Data
 It stops the VBI function of the Video auxiliary outputs. 
 * @remarks Monitoring output VBI 
 * @param eAuxOutId
 * @param eType Closed Caption or Extended Data Service or etc.
 * @code
 		typedef enum
		{
			SD_VBI_DATA_CC = 0, ///< Closed Caption
			SD_VBI_DATA_XDS, ///< Extended Data Service
			SD_VBI_DATA_TTX, ///< Analog SdUserData is Teletext
			SD_VBI_DATA_WSS, ///< Analog SdUserData is WSS
		} SdAuxOut_VbiDataType_k; ///< VBI Data Type
 * @endcode
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAuxOut_VbiStop(SdAuxOutID_k eAuxOutId, SdAuxOut_VbiDataType_k eType);

/**
 * @brief Video ���� ����� mute ��Ų��.
 It makes the Video auxiliary outputs mute On/Off.
 * @remarks 
 * @param eAuxOutId
 * @param bMuteOn TRUE : Mute On, FALSE : Mute Off
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAuxOut_VideoMute(SdAuxOutID_k eAuxOutId, SdBool_t bMuteOn);

/**
 * @brief Audio ���� ����� mute ��Ų��.
 It makes the Audio auxiliary outpus mute On/Off.
 * @remarks 
 * @param eAuxOutId
 * @param bMuteOn TRUE : Mute On, FALSE : Mute Off
 * @param eAuxAudio Mute Control�� ����� �����Ѵ�. 
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAuxOut_AudioMute(SdAuxOutID_k eAuxOutId, SdBool_t bMuteOn, SdAuxOut_Audio_k eAuxAudio);

/**
 * @brief Video ���� ��¿� Macrovision�� �ɾ��ش�.
 It applies the Macrovision to the video auxiliary output. 
 * @remarks 
 * @param eAuxOutId
 * @param eType Macrovision type
 * @code
 		typedef enum
		{
			SD_MACROVISION_AGC = 1,
			SD_MACROVISION_2L,
			SD_MACROVISION_4L
		} SdAuxOut_MacrovisionType_k;
 * @endcode
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAuxOut_MacrovisionStart(SdAuxOutID_k eAuxOutId, SdAuxOut_MacrovisionType_k eType);

/**
 * @brief Video ���� ����� Macrovision�� �����Ѵ�.
 It inactivates the Macrovision at the video auxiliary output.
 * @remarks 
 * @param eAuxOutId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAuxOut_MacrovisionStop(SdAuxOutID_k eAuxOutId);

/**
 * @brief SdAuxOut API�� Manual Debug �Լ�
 The Manual Debug function of SdAuxOUt API.
 * @remarks SdDebug.h�� ���ǵ� SdDebug()�� ���� ȣ��ȴ�. It is called by SdDebug() in SdDebug.h.  
 * @return 
 * @see 
 */
void SdAuxOut_Debug(void);

#endif

