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
 * @file SdFeeder.h
 * @brief Feeder Module header file\n
  Samsung DAL���� �����ü�� �ִ� �����, ���� �����͸� ���ڴ��� �����ϴ� �Լ�.\n
  Live TS, PVR path �ܿ� USB, DLNA path���� ��� ���� media file�� ���� ó���� ����Ѵ�.\n
  MP3, H.264, DivX �� MultiMedia ������ ����� �� �ʿ��� ���� ������ ���� API�� ��� ����.\n
  �� ���Ͽ� �ִ� caption �� MPEG header, TS packet���� �ִ� caption������ �ƴ� asf, avi ���� ����
  �����̳ʿ� �ִ� caption ���� ���� ���̴�.
  Samsung DAL gathers together the APIs for the buffer management which is needed \n
  when playing the MultiMedia file like MP3, H.264, DivX etc.\n 
 * @author S/W Platform Lab
 * @date 2006/08/24
 */

#ifndef	_SDFEEDER_H_
#define	_SDFEEDER_H_

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
	SD_CONTAINER_NONE, ///< none by sean
	SD_CONTAINER_ES, ///< elementary stream
	SD_CONTAINER_PES, ///< packetized elementary stream
	SD_CONTAINER_PS, ///< program stream	
	SD_CONTAINER_TS, ///< transport stream
	SD_CONTAINER_ASF, ///< advanced system format
	SD_CONTAINER_DIVX, ///< Divx
	SD_CONTAINER_VOB, ///< Video Object
	SD_CONTAINER_AVI, ///< AVI
	SD_CONTAINER_WAV, ///< WAV
	SD_CONTAINER_MP3, ///< MP3
	SD_CONTAINER_MP4, ///< MP4, MOV ...
	SD_CONTAINER_VCD,
	SD_CONTAINER_MPG,
	SD_CONTAINER_VRO,
	SD_CONTAINER_MAX
} SdFeeder_ContainerType_k;

typedef enum
{
	SD_FEEDER_STATUS_CUR_POSITION = 0x01, ///< Mask of current position status
	SD_FEEDER_STATUS_BUFFER_SIZE = 0x02, ///< Mask of max buffer size status
	SD_FEEDER_STATUS_BUFFER_ONGOING = 0x04, ///< Mask of buffer avaiable status	
	SD_FEEDER_STATUS_SEEK_MODE = 0x08, ///< Mask of seek mode status
    SD_FEEDER_STATUS_TRICK_MODE = 0x10, ///< Mask of trick mode status	
} SdFeeder_StatusMask_k;

typedef enum
{
	SD_FEEDER_CALLBACK_CAPTION, ///< notification of caption 
	SD_FEEDER_CALLBACK_FILEERROR, ///< notification of fileplay error
	SD_FEEDER_CALLBACK_FILEBEGIN, ///< notification of fileplay begin
	SD_FEEDER_CALLBACK_FILEEND, ///< notification of fileplay end
	SD_FEEDER_CALLBACK_COPYBUFFER, ///< notification of copy buffer
	SD_FEEDER_CALLBACK_MAX
} SdFeeder_CallbackType_k;

typedef enum
{
	SD_FEEDER_DEMUX_ERROR, ///< Demux Error occurence
	SD_FEEDER_DRM_ERROR, ///< DRM Not supported case
	SD_FEEDER_NONINTERLEAVED, ///< non interleaved file case
	SD_FEEDER_VIDEO_ERROR, ///< video error case
	SD_FEEDER_AUDIO_ERROR, ///< audio error case
	SD_FEEDER_FILE_ERROR, ///< file error case
	SD_FEEDER_MAX,	
} SdFeeder_ErrorType_k;

typedef struct
{
	int8_t* pFilePath; ///< file full path. If it is not local file, fill with NULL

	SdFeeder_ContainerType_k eContainterType; ///< container type
	SdVideoFormat_k eVidFormat; ///< video format
	SdAudioFormat_k eAudFormat; ///< audio format

	struct
	{
		uint32_t streamId; ///< video stream ID. Not used in TS streams
	} video;
	struct
	{
		uint32_t streamId; ///< audio stream ID. Not used in TS streams
	} audio;
	struct
	{
		uint32_t streamId; ///< Caption stream ID. Not used in TS streams
	} caption;

	uint32_t speed; ///< play speed
	SdBool_t bReversePlay; ///< play direction (FALSE : Forward Direction, TRUE : Reverse Direction)

	SdAVDec_DmxHandle dmxHandle; ///< demux handle. Only used for TS&PS streams
	SdAVDecID_k eAVDecId;
	uint32_t totalTime; ///< file's total time(millisecond)
	uint64_t totalSize; ///< the total size of file(byte)
} SdFeeder_Settings_t;

typedef struct
{
	uint32_t position; ///< current position time(millisecond)
	uint32_t maxBufferSize; ///< max size of local buffer
	SdBool_t bOnGoing;	///< Is avaiable buffer? true:avaiable, false:Not avaiable	
	SdBool_t bSeek; ///< true : seeking is possible, false : seeking is not possible
    SdBool_t bTrick; ///< true : supported trick mode, false : not supported trick mode
} SdFeeder_Status_t;

typedef struct
{
	struct
	{
	uint32_t captionStreamId; ///< caption stream ID
	uint8_t* pData; ///< caption data buffer pointer
	uint32_t length; ///< caption data length
	} caption;

	struct
	{
		SdBool_t videoSupport; ///< video support
		SdBool_t audioSupport; ///< audio support
		SdFeeder_ErrorType_k eErrorType; ///< error type
	} error;
	
} SdFeeder_CallBackParams_t;

typedef void (*SdFeeder_CallBack)(SdFeeder_CallbackType_k eCallbackType, SdFeeder_CallBackParams_t *pParams, uint32_t param);

typedef void* SdFeeder_Handle;

/********************************************************************
	EXTERN VARIABLES & FUNCTION PROTOTYPES DECLARATIONS
********************************************************************/

/**
 * @brief Feeder ���� �ڿ� �Ҵ�
 It allots the Feeder-related hardware resources. 
 * @remarks 
 * @param pHandle
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdFeeder_Open(SdFeeder_Handle* pHandle);

/**
 * @brief Feeder ���� �Ҵ��� �ڿ��� �����Ѵ�.
 It releases the allotted resources.
 * @remarks 
 * @param handle
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdFeeder_Close(SdFeeder_Handle handle);

/**
 * @brief Feeder�� start�Ѵ�.
 It starts Feeder.
 * @remarks 
 * @param handle
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdFeeder_Start(SdFeeder_Handle handle);

/**
 * @brief Feeder�� stop�Ѵ�.
 It stops Feeder.
 * @remarks 
 * @param handle
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdFeeder_Stop(SdFeeder_Handle handle);

/**
 * @brief Feeder�� pause�Ѵ�.
 It pauses Feeder.
 * @remarks 
 * @param handle
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdFeeder_Pause(SdFeeder_Handle handle);

/**
 * @brief Feeder�� resume�Ѵ�.
 It resumes Feeder.
 * @remarks 
 * @param handle
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdFeeder_Resume(SdFeeder_Handle handle);

/**
 * @brief Feeder buffer�� data�� write�� ũ��� �Բ� write �� ����� �˷� �ش�.
 It informs the size of copied data and the fact of writing to the Feeder buffer.
 * @remarks 
 * @param handle
 * @param size submited size
 * @param bIsLastData If it is last data true, otherwise false.
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdFeeder_Submit(SdFeeder_Handle handle, uint32_t size, SdBool_t bIsLastData);

/**
 * @brief Feeder���� ��� ������ buffer�� pointer�� size�� ��� �´�.
 It gets the avaiable buffer pointer and size. 
 * @remarks 
 * @param handle
 * @param ppBuffer avaiable buffer pointer
 * @param pSize size of avaiable buffer
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdFeeder_GetBuffer(SdFeeder_Handle handle, int32_t** ppBuffer, uint32_t* pSize);

/**
 * @brief �ش� feeder�� Stream ���� Setting�� �о�´�.
 It reads in the Stream-related settings of the concerned feeder 
 * @remarks
 * @param handle
 * @param pSettings
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdFeeder_Get(SdFeeder_Handle handle, SdFeeder_Settings_t* pSettings);

/**
 * @brief �ش� feeder�� Stream ���� Setting�� �����Ѵ�.
 It sets the Stream-related settings of the concerned feeder.
 * @remarks local file play�� ��� pFilePath�� full path�� �־� �ְ� length�� 0���� �����Ѵ�.
 local file�� �ƴѵ� Driver ���� �����̾ parsing�� ���� ��� pData�� �����͸� ����ϰ� ä�� �־ 
 SdFeeder_GetMediaInfo �Լ��� �̿��Ͽ� �ʿ��� ������ ������ �ֵ��� �Ѵ�.
 If it is local file play, set pFilePath with full path of the file and set the length 0.
 If it is not local file play, the file format is supported, and only parsing is needed, fill enough data in pData
 and use SdFeeder_GetMediaInfo function.
 * @param handle
 * @param pSettings
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdFeeder_Set(SdFeeder_Handle handle, SdFeeder_Settings_t* pSettings);

/**
 * @brief current position ���¿� max buffer size ���¸� �����´�.
 It brings the status of current position and max buffer size.
 * @remarks
 * @param handle
 * @param mask ���� ���� ����ü���� ��ȿ�� ������ ��Ÿ���� ������ Masks of status.
	�̿� ���� mask�� �Ʒ��� ����. The masks are explained below.\n
 * @code
		typedef enum
		{
			SD_FEEDER_STATUS_CUR_POSITION = 0x01, ///< Mask of current position status
			SD_FEEDER_STATUS_BUFFER_SIZE = 0x02, ///< Mask of max buffer size status
		} SdFeeder_StatusMask_k;
 * @endcode 
 * @param pState ���� ���� ����ü. The structure of status.
 �������� ����ü�� �Ʒ��� ����. It is explained below.
  * @code
		typedef struct
		{
			uint32_t position; ///< current position time(micro second)
			uint32_t maxBufferSize; ///< max size of local buffer
		} SdFeeder_Status_t;
 * @endcode
 * @param timeout
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdFeeder_GetStatus(SdFeeder_Handle handle, SdMask_t mask, SdFeeder_Status_t *pState, uint32_t timeout);

/**
 * @brief Feeder ����� callback function�� ����� �ش�
 It registers the callback function of Feeder module.
 * @remarks ��ϵ� callback �Լ��� ���ؼ� filtering �� caption data�� �Ѱ� �ش�
 It passes over the filtered caption data through the registered callback function.
 This function is not used for TS&PS streams.
 * @param handle
 * @param eCallbackType
 * @param callback caption data�� �Ѱ� ���� callback �Լ�
 * @param param context parameter
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdFeeder_InstallCallback(SdFeeder_Handle handle, SdFeeder_CallbackType_k eCallbackType, SdFeeder_CallBack callback, uint32_t param);

/**
 * @brief Feeder ����� callback function�� ����� ���� �Ѵ�.
 It releases the registeration of callback function of the Feeder module.
 * @remarks This function is not used for TS&PS streams.
 * @param handle
 * @param eCallbackType
 * @param callback caption data�� �Ѱ� �޵��� ��ϵ� callback �Լ�
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdFeeder_UnInstallCallback(SdFeeder_Handle handle, SdFeeder_CallbackType_k eCallbackType, SdFeeder_CallBack callback);

/**
 * @brief DLNA ���� Setting�� �����Ѵ�.
 It sets the DLNA-related settings. 
 * @remarks
 * @param handle
 * @param packetSize packet�� ũ��
 * @param pKey key�� �����ּ�
 * @param keyLength key�� ����, key ���� ������ �� ������ 0���� �����Ѵ�
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdFeeder_DLNASet(SdFeeder_Handle handle, uint32_t packetSize, int8_t* pKey, uint32_t keyLength);

/**
 * @brief position�� �̵���Ų��.
 It moves the position.
 * @remarks
 * @param handle
 * @param position position time(millisecond)
 * @param pFileOffset getting file offset by position time
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdFeeder_MovePosition(SdFeeder_Handle handle, uint32_t position, uint64_t* pFileOffset);

/**
 * @brief SdFeeder API�� Manual Debug �Լ�
 The Manual Debug function of SdFeeder API
 * @remarks SdDebug.h�� ���ǵ� SdDebug()�� ���� ȣ��ȴ�. 
 * @return 
 * @see 
 */
void SdFeeder_Debug(void);

#endif

