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
 * @file SdAVDec.h
 * @brief Audio/Video decoder header file
 A/V Decoder Start/Stop ����� �����ϸ�, A/V Lock Status Check ��ɵ� �����Ѵ�.
 TSD(TS Demux)���� Decoder�� ���� Path�� �����Ѵ�.
 This file contains APIs for the following functions:
 A/V Decoder Start/Stop, A/V Lock Status Check, TSD(TS Demux) to Decoder Path Connection.
 \image html SdAVDec_Diagram.jpg "SdAVDec Data Flow & Function Block Diagram"
 * @author S/W Platform Lab
 * @date 2006/07/11
 */

#ifndef	_SDAVDEC_H_
#define	_SDAVDEC_H_

/********************************************************************
	INCLUDE FILES
********************************************************************/
#include "SdType.h"
#include "SdMisc.h"

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
	SD_ASPECT_RATIO_NONE,
	SD_ASPECT_RATIO_1_1, ///< Aspect ration 1:1
	SD_ASPECT_RATIO_4_3, ///< Aspect ration 4:3
	SD_ASPECT_RATIO_16_9, ///< Aspect ration 16:9
	SD_ASPECT_RATIO_221_1, ///< Aspect ration 2.21:1
	SD_ASPECT_RATIO_MAX
} SdAVDec_AspectRatio_k;

typedef enum
{
	SD_AVDEC_STATUS_TSDLOCK		= 0x01, ///< Mask of TSD lock status
	SD_AVDEC_STATUS_AUDIOLOCK 	= 0x02, ///< Mask of audio decoding lock status
	SD_AVDEC_STATUS_VIDEOLOCK	= 0x04, ///< Mask of video decoding lock status
	SD_AVDEC_STATUS_VIDEOINFO	= 0x08, ///< Mask of video information status
	SD_AVDEC_STATUS_UNMUTE		= 0x10, ///< Mask of UNMUTE status		
	SD_AVDEC_STATUS_STC			= 0x20,	///< Mask of STC information status	
	SD_AVDEC_STATUS_AUDIOINFO	= 0x40,	///< Mask of audio information status	
	SD_AVDEC_STATUS_VIDEOUNDERFLOW = 0x080, ///< Mask of Video Decoder Underflow status
    SD_AVDEC_STATUS_SUPPORTTRICK = 0x100, ///< Mask of Video Decoder Trick Support status
    SD_AVDEC_STATUS_ERROR       = 0x200, ///< Mask of Video Decoder Trick Support status
} SdAVDec_StatusMask_k;

typedef enum
{
	SD_DEMUX_TYPE_TS, ///< Demux for Transport Stream
	SD_DEMUX_TYPE_PS, ///< Demux for Program Stream
	SD_DEMUX_TYPE_MAX
} SdAVDec_DemuxType_k;

typedef enum
{
	SD_DEMUX_OUT_DECODER, ///< Decoder(MPEG Decoder, AC3 Decoder, ...)�� Demux ����� ������. Demux to Decoder(MPEG Decoder, AC3 Decoder, ...)
	SD_DEMUX_OUT_TSOUT, ///< TS Generator�� Demux ����� ������. Demux to TS Generator
	SD_DEMUX_OUT_TIMESHIFT, ///< Time Shift Module�� Demux ����� ������. Demux to Time Shift Module
	SD_DEMUX_OUT_RECORD, ///< PVR Recording Module�� Demux ����� ������. Demux to PVR Recording Module
	SD_DEMUX_OUT_MAX
} SdAVDec_DemuxOut_k;

typedef enum
{
	SD_AVDEC_DISCONNECT_ALL, ///<Decoder�� �Է´ܰ� ��´��� ��� Disconnect. Disconnect both Decoder Input and Output
	SD_AVDEC_DISCONNECT_OUT, ///<Decoder�� ��´ܸ��� Disconnect. Disconnect only decoder output	
	SD_AVDEC_DISCONNECT_MAX,	
} SdAVDec_DisconnectOpt_k;

typedef enum 
{
	SD_AVDEC_PLAYMODE_NORMAL,
	SD_AVDEC_PLAYMODE_I_ONLY,
	SD_AVDEC_PLAYMODE_SKIP_B,
	SD_AVDEC_PLAYMODE_PARSE_ONLY,
	SD_AVDEC_PLAYMODE_MAX
} SdAVDec_PlayMode_k;

typedef enum
{
    SD_AUD_CH_MODE_STEREO,
    SD_AUD_CH_MODE_JOINT_STEREO,
    SD_AUD_CH_MODE_DUAL,
    SD_AUD_CH_MODE_SINGLE,
} SdAVDec_AudChMode_k;

typedef enum
{
    SD_AUD_STEREO_MODE_MONO,
    SD_AUD_STEREO_MODE_STEREO,
    SD_AUD_STEREO_MODE_LEFT_TO_BOTH,
    SD_AUD_STEREO_MODE_RIGHT_TO_BOTH,
} SdAVDec_AudStereoMode_k;

typedef enum
{
    SD_AVDEC_ERROR_NONE,
    SD_AVDEC_ERROR_NOT_SUPPORTED_VIDEO,
    SD_AVDEC_ERROR_NOT_SUPPORTED_VIDEO_RESOLUTION,
    SD_AVDEC_ERROR_NOT_SUPPORTED_VIDEO_FRAMERATE,
    SD_AVDEC_ERROR_NOT_SUPPORTED_VIDEO_PROFILE,
    SD_AVDEC_ERROR_NOT_SUPPORTED_AUDIO,
} SdAVDec_Error_k;

typedef struct
{
    SdVideoFormat_k eVidFormat;
    SdAudioFormat_k eAudFormat;
    SdAVDec_Error_k eAVDecError;
} SdAVDec_ErrorInfo_t;

typedef struct
{
	SdFloat32_t			Speed;///< Play Speed
	SdBool_t			bReversePlay; ///< Play ���� ����(FALSE : Forward Direction, TRUE : Reverse Direction). Direction of play
	SdAVDec_PlayMode_k 	ePlaymode; 	///< Play ���, play mode
} SdAVDec_PlaySettings_t;

typedef struct
{
	SdAudioFsRate_k eFsRate; ///< PCM Sampling Rate. invalid value : SD_AUDIO_FS_NONE
	uint32_t bitWidth;	///< bit Width
	uint32_t channels;	///< a number of audio channels
	uint32_t blockAlign;   ///< block Align (ADPCM only)
	SdAudioPCMFormat_k ePCM; ///< PCM Sub Type 
	SdAudioADPCMFormat_k eADPCM; ///< Encode Type (ADPCM only)	
	SdBool_t bLittleEndian;  ///< �Է� Data�� Endian ���� (FALSE : Big Endian, TRUE : Little Endian)
} SdAVDec_PCMSettings_t;

typedef struct
{
    uint16_t formatTag; ///< ������� ����(codec ID) ��:0x161, 0x162, 0x163
    uint16_t channelsNumber; ///< Channel Number ��:1-mono, 2-stereo
    uint32_t sampleRateHz; ///< Sample Rate    
    uint32_t avgBytesPerSecond; ///< Bitrate ����. Asf header�� stream property object�� �Ľ��ϸ� ���´ٰ� ��.
    uint32_t bitsPerPacket; ///< �ϳ��� packet size(BlockAlign�̶�� ��)
    uint16_t validBitsPerSample; ///< Sample �ϳ��� �� bit ����
} SdAVDec_WMASettings_t;

typedef struct
{
	uint8_t   mpegVersion; 	///< (MPEG2 = 2 or MPEG4=4)
	uint8_t   sampleRateIdx;
	uint8_t   profile_ObjectType;
	uint8_t   totalNumCh;
	uint32_t  sampleRateHz; 
	
	//�Ʒ� 5���� AAC LC�� HE-AAC �� �����ϴ� ���� 0���� ����.
	uint32_t  layerLength;
	uint8_t   aacScalefactorDataResilienceFlag;
	uint8_t   aacSectionDataResilienceFlag;
	uint8_t   aacSpectralDataResilienceFlag;
	uint8_t   numOfsubFrame;	
	
	//for SBR
	uint8_t   sbrFlag;
	uint8_t   extSampleRateIndex;
	uint8_t   extSampleRateHz;
} SdAVDec_AACSettings_t;

typedef struct
{
	uint32_t hResolution; ///< Source horizontal active size
	uint32_t vResolution; ///< Source vertical active size
	uint32_t hStart; ///< Source horizontal back porch(optional) 
	uint32_t vStart; ///< Source vertical back porch(optional)
	uint32_t hTotal; ///< Source horizontal raster size(optional)
	uint32_t vTotal; ///< Source vertical raster size(optional)
	SdBool_t bProgressScan; ///< Source scantype - TRUE:Progressive, FALSE:Interlace
	SdAVDec_AspectRatio_k eRatio; ///< Aspect ratio
	uint32_t frameRate; ///< Frame rate (Hz * 1000)
	SdMatrixCoeff_k eMatrix_Coefficients; ///< matrix coefficients used in deriving luminance and chrominance signals from the green, blue, and red primaries								  
} SdAVDec_VideoInfo_t;

typedef struct
{
	SdAVDec_AudChMode_k eAudioChMode; ///< Audio Channel Mode(For DVB dual Audio)
	SdAudioFsRate_k eAudioFsRate; ///< Audio Sampling Rate. invalid value : SD_AUDIO_FS_NONE (For DVB)
} SdAVDec_AudioInfo_t;

typedef struct
{
	SdBool_t bTSDLock; ///< TSD lock status 1:Lock, 0:Unlock
	SdBool_t bAudioLock; ///< Audio decoding lock status 1:Lock, 0:Unlock
	SdBool_t bVideoLock; ///< Video decoding lock status 1:Lock, 0:Unlock
	SdBool_t bUnmuteEnable; ///< Unmute Enable status 1:Enable, 0:Disable
	SdAVDec_VideoInfo_t sVideoInfo; ///< Video information	
	SdAVDec_AudioInfo_t sAudioInfo; ///< Audio information 
	uint64_t stc; ///< stc information
	SdBool_t bDecoderUnderflow; ///< Video decoder Underflow status 0:Normal, 1:Underflow
    SdBool_t bSupportTrickmode; ///< Trick Mode Support status 0:Not Support, 1:Support
    SdAVDec_Error_k eAVDecError; ///< Error status
} SdAVDec_Status_t;

typedef struct
{
	SdVideoFormat_k eVidFormat; ///< Video format
	SdColorFormat_k eColorFormat; ///< Color format
	uint8_t *pBuffer; ///< Buffer pointer
	uint32_t size; ///< Buffer size
	uint32_t imageWidth; ///< Image width, in pixels
	uint32_t imageHeight; ///< Image height, in pixels
	
	uint8_t *pYTopAddr; ///< base address of top field of Y component to be encoded
    uint8_t *pYBotAddr; ///< base address of bottom field of Y component to be encoded
    uint8_t *pCTopAddr; ///< base address of top field of C component to be encoded
    uint8_t *pCBotAddr; ///< base address of bottom field of C component to be encoded
} SdAVDec_CaptureInfo_t;

typedef struct
{
	SdStreamType_k eStreamType;  //Stream Type. SdAVDec_ConnectDecoder ȣ������ �ݵ�� Setting �Ǿ�� ��. Should be set before SdAVDec_ConnectDecoder
	SdVideoFormat_k eVidFormat;  //Video Format. SdAVDec_ConnectDecoder ȣ������ �ݵ�� Setting �Ǿ�� ��. Should be set before SdAVDec_ConnectDecoder
	SdAudioFormat_k eAudFormat;	 //Audio Format. SdAVDec_ConnectDecoder ȣ������ �ݵ�� Setting �Ǿ�� ��. Should be set before SdAVDec_ConnectDecoder
	
	struct
	{
		int32_t pid; ///< Video pid, invalid value:-1
	} video;
	struct
	{
		int32_t pid; ///< Audio pid, invalid value:-1
		int32_t auxpid; ///< Aux audio pid, invalid value:-1 (only for DVB. if auxpid is equal to -1, aux out format is always PCM. )		
	} audio;
	struct
	{
		int32_t pid; ///< PCR pid, invalid value:-1
	} pcr;
	struct
    {
        int32_t pid; ///< Audio Description pid(only for DVB), invalid value:-1
    } audioDescription;	
    
	SdBool_t bAVSyncOn; ///< AV sync - TRUE:Sync, FALSE:ASync(Free running)
	SdBool_t bAdOn; ///< Audio Description - TRUE:Enable, FALSE:Disable
	
	SdAVDec_PlaySettings_t sPlaySettings; ///< Trick Play�� ������ ����. Settings related to Trick Play.
	SdAVDec_PCMSettings_t sPCMSettings; ///< PCM Audio�� ������ ����. Settings related to PCM Audio.
	SdAVDec_WMASettings_t sWMASettings; ///< WMA Audio�� ������ ����. Settings related to WMA Audio.
	SdAVDec_AACSettings_t sAACSettings; ///< AAC Audio�� ������ ����. Settings related to AAC Audio.
	SdAVDec_AudStereoMode_k eAudStereoMode; ///< Audio Stereo Out�� ������ ����. Settings related to Audio Stereo Mode (For DVB dual Audio)
	uint32_t videoUnderflowLevel; ///< underflow�� �ǴܵǴ� video buffer�� data�� �ִ� byte��. Video Decoder Underflow Level.(buffer underflow threshold bytes)
} SdAVDec_Settings_t;

typedef void (*SdAVDec_CallBack)(SdAVDec_Status_t *pStatus, SdAVDec_StatusMask_k eMask, uint32_t param);

typedef void (*SdAVDec_CallBackCapture)(SdAVDec_CaptureInfo_t *pInfo, uint32_t param);

typedef void (*SdAVDec_CallBackError)(SdAVDec_ErrorInfo_t *pInfo, uint32_t param);


/********************************************************************
	EXTERN VARIABLES & FUNCTION PROTOTYPES DECLARATIONS
********************************************************************/

/**
 * @brief Demux, Audio Decoder, Video Decoder ���� Device���� �ʱ�ȭ�Ͽ� ����ڰ� ����� �� �ֵ��� �Ѵ�.
 It initializes the devices pertaining to Demux, Audio Decoder, Video Decoder and allows the user to use them.
 * @remarks
 * @param ePowerState Setting the power state: either power off->on, off->standby or standby->on. default is off->on.
 The following is the Power state list.\n
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
SdResult SdAVDec_Init(SdPowerState_k ePowerState);

/**
 * @brief �Է� �ҽ��� Demux Type(TS or PS) �� Demux ��� ���¿� ���� Demux�� �Ҵ��ϰ� �Ҵ�� Demux�� Handle�� �����ش�.
 It allots the Demux depending on the input source and Demux Type(TS or PS) and the type of outputting Demux and returns the Handle of allotted Demux.
 * @remarks 
 * @param pDmxHndl
 * @param eSrc	Demux�� �Է� �ҽ�, Demux input source
 * @param eDemuxType Demux Type to allocate(TS Demuxer or PS Demuxer) 
 * @param eDemuxOut Demux output type(DECORDER, TSOUT, TIMESHIFT, RECORD)
 * @param bEnableRealloc Demux�� �Ҵ� ����� ����.(Demux�� �����Ͽ� ������� ���� ��쿡�� ������ False) Method to allocate a demux (Set False if demux is not shared)
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_DemuxAllocate(SdAVDec_DmxHandle *pDmxHndl, SdSource_k eSrc, SdAVDec_DemuxType_k eDemuxType, SdAVDec_DemuxOut_k eDemuxOut, SdBool_t bEnableRealloc);

/**
 * @brief �Ҵ�� Demux�� �ݳ��Ѵ�. 
 It returns the allotted Demux.
 * @remarks 
 * @param dmxHndl
 * @param eDemuxOut �ݳ��� Demux�� ��� ���¸� ����. Demux output type for the returning demux
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_DemuxDeallocate(SdAVDec_DmxHandle dmxHndl, SdAVDec_DemuxOut_k eDemuxOut);

/**
 * @brief Demux�� ������ �����Ѵ�.
 It starts the operation of Demux. 
 * @remarks 
 * @param dmxHndl
 * @param eDemuxOut 
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_DemuxStart(SdAVDec_DmxHandle dmxHndl, SdAVDec_DemuxOut_k eDemuxOut);

/**
 * @brief Demux�� ������ �����Ѵ�.
 It stops the operation of Demux.
 * @remarks 
 * @param dmxHndl
 * @param eDemuxOut 
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_DemuxStop(SdAVDec_DmxHandle dmxHndl, SdAVDec_DemuxOut_k eDemuxOut);

/**
 * @brief PVR �Ǵ� 1394�� ����� Stream ������ ���ؼ� Demux�� Pid Filter ������ �Ѵ�.
 Set the Pid Filter at Demux in order to create the Stream to be outputted as PVR or 1394.
 * @remarks 
 * @param dmxHndl
 * @param puwPids ������ PID�� array pointer. Video, Audio, PCR, SI PID���� ���޵Ǿ�� �Ѵ�. Pointer of the array of PIDs to be stored. The PIDs should contain Video, Audio, PCR, SI PID.
 * @param uwNumPids The size of PID array
 * @param eDemuxOut
 * @param bCaOn
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_StreamOutSetPid(SdAVDec_DmxHandle dmxHndl, int32_t *puwPids, uint32_t uwNumPids, SdAVDec_DemuxOut_k eDemuxOut, SdBool_t bCaOn);

/**
 * @brief Audio/Video Decoder�� Demux�� �����ϰ� ��°� ����� Video�� ���� ��� ������ �����Ѵ�.
 It connects the Audio/Video Decoder with Demux and changes the output settings depending on the Video which is connected to the output.
 * @remarks ������ Disconnect ���¿� ���� ��¸� ���� �Ǵ� ��/��� ��� �����ؾ� �Ѵ�.
 Either the output only should be connected depending on the previous Disconnect state or both input/output should be connected.
 * @param eAVDecId
 * @param dmxHndl
 * @param eOutVideoId Decoder�� ����Ǵ� Video ������Ʈ�� ID. ID of SdVideo connected to the decoder
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
//SdResult SdAVDec_DecoderConnect(SdAVDecID_k eAVDecId, SdAVDec_DmxHandle dmxHndl, SdVideoID_k eOutVideoId);

/**
 * @brief It connects the Audio Decoder with HDMI receiver to decode the compressed audio.
 * @remarks  
 * @param eAVDecId
 * @param eExtInId 
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_DecoderConnect(SdAVDecID_k eAVDecId, SdExtInID_k eExtInId);

/**
 * @brief Audio/Video Decoder�� �Է� �� ��´��� ���� ������ �����ϴ� �Լ�
 The function of releasing the connection settings of input and output terminals for Audio/Video Decoder.
 * @remarks 
 * @param eAVDecId
 * @param eOption ���� ������ �Է�, ��´� ��� �� ������ ��´ܸ� �� ������ ���� Disconnect option whether Input/Output or Output only
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_DecoderDisconnect(SdAVDecID_k eAVDecId, SdAVDec_DisconnectOpt_k eOption);

/**
 * @brief Video Decoder�� ���۽�Ų��.
 It starts the operation of the Video Decoder.
 * @remarks 
 * @param eAVDecId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_VideoStart(SdAVDecID_k eAVDecId);

/**
 * @brief Video Decoder�� ������ ������Ų��.
 It stops the operation of Video Decoder.
 * @remarks 
 * @param eAVDecId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_VideoStop(SdAVDecID_k eAVDecId);

/**
 * @brief Audio Decoder�� ���۽�Ų��.
 It starts the operation of  the Audio Decoder. 
 * @remarks 
 * @param eAVDecId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_AudioStart(SdAVDecID_k eAVDecId);

/**
 * @brief Audio Decoder�� ������ ������Ų��.
 It stops the operation of Audio Decoder.
 * @remarks 
 * @param eAVDecId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_AudioStop(SdAVDecID_k eAVDecId);

/**
 * @brief Video Decoder�� Pause��Ų��.
 It pauses the Video Decoder.
 * @remarks 
 * @param eAVDecId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_VideoPause(SdAVDecID_k eAVDecId);

/**
 * @brief Video Decoder�� Resume��Ų��.
 It resumes the operation of Audio Decoder.
 * @remarks 
 * @param eAVDecId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_VideoResume(SdAVDecID_k eAVDecId);

/**
 * @brief Video�� Frame������ �����ش�. Pause���¿��� ����� �������� �����ش�.
 It shows the video screen by frames. It shows frame by frame in pause state.
  * @remarks 
 * @param eAVDecId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_VideoSinglePlay(SdAVDecID_k eAVDecId);

/**
 * @brief Audio Decoder�� Pause��Ų��. 
 It pauses the Audio Decoder.
 * @remarks 
 * @param eAVDecId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_AudioPause(SdAVDecID_k eAVDecId);

/**
 * @brief Audio Decoder�� Resume��Ų��.
 It resumes the operation of Audio Decoder.
 * @remarks 
 * @param eAVDecId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_AudioResume(SdAVDecID_k eAVDecId);

/**
 * @brief �ش� Decoder�� Stream ���� Setting�� �о�´�.
 It reads in the Stream-related settings of the concerned Decoder. 
 * @remarks �� ���õ� audio pid, video pid, pcr pid, AV sync, Stream Type, Video/Audio Format�� �д´�.
 SdAVDec_Set �Լ����� ���õ� ���� �о� ���� �Ǹ� �ʱ⿡�� SdAVDec_Set�� ���� ���� ���¿����� invalid �� ���� ������ �ȴ�.
 The pre-set audio pid, video pid, pcr pid, AV sync, Stream Type, Video/Audio Format will be read in. 
 Reading in the pre-set value from SdAVDec_Set function reads in an invalid value when the SdAVDec_Set is not done in initial time.
 * @param eAVDecId
 * @param pSettings
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_Get(SdAVDecID_k eAVDecId, SdAVDec_Settings_t *pSettings);

/**
 * @brief �ش� Decoder�� Stream ���� Setting�� �����Ѵ�.
 It sets the Stream-related settings of the concerned Decoder.
 * @remarks audio pid, video pid, pcr pid, AV sync, Stream Type, Video/Audio Format�� �����Ѵ�.
 TS demux �� decoder�� �����Ͽ� ���ϴ� AV decoding�� �� �� �ֵ��� ���ش�. 
 pid �� AV sync setting�� �ݵ�� SdAVDec_ConnectDecoder() ȣ�� �� setting �����ϸ�
 SdAVDec_ConnectDecoder() ȣ���� Stream Type, Video/Audio Format�� �ݵ�� Setting�Ǿ� �־�� �Ѵ�.
 It sets the audio pid, video pid, pcr pid, AV sync, Stream Type, Video/Audio Format. 
 It sets both TS demux and Decoder so that AV decoding can be carried out. 
 Both pid and AV sync setting must be set after calling the SdAVDec_ConnectDecoder(), 
 and the Stream Type, Video/Audio Format must be set before calling the SdAVDec_ConnectDecoder(). 
 * @param eAVDecId
 * @param pSettings
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_Set(SdAVDecID_k eAVDecId, SdAVDec_Settings_t *pSettings);

/**
 * @brief Demux Lock, Video Lock, Audio Lock ���� �� Mpeg Decoder�κ��� Video Information�� �����´�.
 It brings the status of Demux Lock, Video Lock, Audio Lock and Video Information from the Mpeg Decoder. 
 * @remarks AV decoder�� ����, TS demux�� ���¸� �о� �´�. ���� decoding�� �Ϸ�� video�� ���� ������ 
 �о� �´�. ���ϴ� ������ �б� ���ؼ��� �ش��ϴ� mask�� |(or) ���Ѽ� ����ϵ��� �ϸ� return���� SD_OK �϶���
 valid�� ������ ���� �� �ִ�. �� ���� return�� �߻��� ��� pState �� ���� ���� ���� �ʴ´�.
 It reads in the AV decoder status and TS demux status. Moreover it reads in the various information of the decoded Video. 
 In order to read in the desired information, make and use the concerned mask |(or) and only when the return value is SD_OK, 
 the valid information can be taken. When the other returns occur, the value of pState will not be guaranteed.
 * @param eAVDecId 
 * @param mask ���� ���� ����ü���� ��ȿ�� ������ ��Ÿ���� ������ 
	�̿� ���� mask�� �Ʒ��� ����. Enum list of status information masks \n
 * @code
		typedef enum
		{
			SD_AVDEC_STATUS_TSDLOCK		= 0x01, ///< Mask of TSD lock status
			SD_AVDEC_STATUS_AUDIOLOCK 	= 0x02, ///< Mask of audio decoding lock status
			SD_AVDEC_STATUS_VIDEOLOCK	= 0x04, ///< Mask of video decoding lock status
			SD_AVDEC_STATUS_VIDEOINFO	= 0x08, ///< Mask of video information status
			SD_AVDEC_STATUS_UNMUTE		= 0x10,  ///< Mask of UNMUTE status	
			SD_AVDEC_STATUS_HDMIAUDIO	= 0x11,  ///< Mask of HDMI AUDIO status	
		} SdAVDec_StatusMask_k;
 * @endcode
 * @param pState ���� ���� ����ü. Demux, AV lock ���� �� video information �� ���� ������
 �������� ����ü�� �Ʒ��� ����. 
 Structures of status information. The following structures show AV lock status and video information.
 * @code
		typedef struct
		{
			SdBool_t bTSDLock;
			SdBool_t bAudioLock;
			SdBool_t bVideoLock;
			SdAVDec_VideoInfo_t sVideoInfo;
		} SdAVDec_Status_t;
		
		typedef struct
		{
			uint16_t hResolution;
			uint16_t vResolution;
			uint16_t hStart;
			uint16_t vStart;
			uint16_t hTotal;
			uint16_t vTotal;
			SdBool_t bProgressScan;
			SdAVDec_AspectRatio_k eRatio;
			uint32_t frameRate;
		} SdAVDec_VideoInfo_t;
 * @endcode
 * @param timeout mask������ SD_AVDEC_STATUS_VIDEOINFO�� ���� Lock ���¿� �������� ������ ������ ��� �ش� ������ �������
 ���� Lock������ True������ ���� Ȯ���ؾ��Ѵ�. timeout�� Lock������ False�� ��� �ش� Lock�� True�� �ɶ������� ������ð��� 
 �ǹ��Ѵ�. mask������ �Ϲ����� Lock�������� ������ ���� �� ���ڴ� �ǹ̸� ���� �ʴ´�.
 A mask such as SD_AVDEC_STATUS_VIDEOINFO is only valid if Lock status is True.
 If Lock status is False, then it waits until Lock is True or timeout is reached.
 timeout has no meaning for Lock status mask.
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_GetStatus(SdAVDecID_k eAVDecId, SdMask_t mask, SdAVDec_Status_t *pState, uint32_t timeout);

/**
 * @brief Demux Lock, Video Lock, Audio Lock ���� �� Video Information�� ���� Callback �Լ��� ����Ѵ�.
 Register the Callback function for the status of Demux Lock, Video Lock, Audio Lock and the Video Information. 
 * @remarks callback �Լ��� ������ִ� �Լ��̴�. mask���� ������ status�� ���� ������(Ȥ�� ���� �ð�����)
 callback �Լ��� ���� �ȴ�. callback �Լ��� ����Ǹ� mask���� ������ status�� ���� �� �ִ�.
 It is the function of registering callback function. Whenever reading the status which was set in mask (Or every a specific time) 
 the callback function will be executed. When callback function is executed, the status which was set in mask can be read.
 * @param eAVDecId
 * @param callback Callback function
 * @param param Callback �Լ� ȣ�� �� ���� ���޵� ����. parameters for the callback function call
 * @param mask ���� ���� ����ü���� ��ȿ�� ������ ��Ÿ���� ������. mask of status information
	mask�� �Ʒ��� ����.
 * @code
		typedef enum
		{
			SD_AVDEC_STATUS_TSDLOCK		= 0x01, ///< Mask of TSD lock status
			SD_AVDEC_STATUS_AUDIOLOCK 	= 0x02, ///< Mask of audio decoding lock status
			SD_AVDEC_STATUS_VIDEOLOCK	= 0x04, ///< Mask of video decoding lock status
			SD_AVDEC_STATUS_VIDEOINFO	= 0x08, ///< Mask of video information status
			SD_AVDEC_STATUS_UNMUTE		= 0x10,  ///< Mask of UNMUTE status	
			SD_AVDEC_STATUS_HDMIAUDIO	= 0x11,  ///< Mask of HDMI AUDIO status	
		} SdAVDec_StatusMask_k;
 * @endcode
 * @param bOneShot �� ���� callback�� ȣ���� ���ΰ�, ��� ȣ���� ���ΰ�? True if the callback function is called only once.
 * @return SdResult SD_OK/SD_NOT_OK
 * @see SdAVDec_CallBack
 */
SdResult SdAVDec_InstallCallback(SdAVDecID_k eAVDecId, SdAVDec_CallBack callback, uint32_t param, SdMask_t mask, SdBool_t bOneShot);

/**
 * @brief decoding�� ���� �����͸� callback�� ����Ѵ�.
 Install/Uninstall the Callback function for capturing the decoded video data
 * @remarks 
 * @param eAVDecId
 * @param callback Callback function, uninstall�Ϸ��� callback�� NULL�� �ش�. Use callback=NULL to uninstall.
 * @param param Callback �Լ� ȣ�� �� ���� ���޵� ����. parameters for the callback function call
 * @param period ������ ms�̸� �� ���� �ֱ�� ���� �����͸� �÷� �ش�. �� 0�� ���� �ѹ��� �����Ѵ�.
 The unit is msec. The callback function is called every period with the captured video data. It captures only once if period is 0.
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_InstallCallbackCapture(SdAVDecID_k eAVDecId, SdAVDec_CallBackCapture callback, uint32_t param, uint32_t period);

/**
 * @brief decoder error�� ȣ�� �� callback �Լ��� ����Ѵ�.
 Install/Uninstall a callback function for reporting error when a decoder error occurs.
 * @remarks 
 * @param eAVDecId
 * @param callback Callback function, uninstall�Ϸ��� callback�� NULL�� �ش�. Use callback=NULL to uninstall.
 * @param param Callback �Լ� ȣ�� �� ���� ���޵� ����. parameters for the callback function call
 * @param bOneShot �� ���� callback�� ȣ���� ���ΰ�, ��� ȣ���� ���ΰ�? True if the callback function is called only once.
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_InstallCallbackError(SdAVDecID_k eAVDecId, SdAVDec_CallBackError callback, uint32_t param, SdBool_t bOneShot);

/**
 * @brief 1394�� ���� Stream ����� �����Ѵ�.
 It starts the Stream output for 1394 in Demux.    
 * @remarks 
 * @param dmxHndl
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_TSOutStart(SdAVDec_DmxHandle dmxHndl);

/**
 * @brief 1394�� ���� Stream ����� �����Ѵ�.
 It stops the Stream output for 1394 in Demux.    
 * @remarks 
 * @param dmxHndl
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdAVDec_TSOutStop(SdAVDec_DmxHandle dmxHndl);

/**
 * @brief SdAVDec API�� Manual Debug �Լ�
 The Manual Debug function of SdAVDec API.
 * @remarks SdDebug.h�� ���ǵ� SdDebug()�� ���� ȣ��ȴ�. It is called by SdDebug() in SdDebug.h. 
 * @return 
 * @see 
 */
void SdAVDec_Debug(void);

#endif

