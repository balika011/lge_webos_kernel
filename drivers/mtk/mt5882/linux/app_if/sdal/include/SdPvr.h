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
 * @file SdPvr.h
 * @brief Pvr Function Define Header File
 * @author S/W Platform Lab
 * @date 2006/08/22
 */

#ifndef	_SDPVR_H_
#define	_SDPVR_H_

/********************************************************************
	INCLUDE FILES
********************************************************************/
#include "SdType.h"
#include "SdMisc.h"

/********************************************************************
	MACRO CONSTANT DEFINITIONS
*********************************************************************/

/********************************************************************
	MACRO FUNCTION DEFINITIONS
********************************************************************/

/********************************************************************
	TYPE DEFINITION
********************************************************************/

/** 
 * @enum SdPVR_Picture_k
 * Catalog�� ���� ���� I/P/B frame ����. I/P/B frame information from Catalog
 */
typedef enum {
	SD_PVR_PICTURE_I, ///< I Picture
	SD_PVR_PICTURE_B, ///< B Picture
	SD_PVR_PICTURE_P, ///< P Picture
	SD_PVR_PICTURE_NULL,
	SD_PVR_PICTURE_SH, ///< Sequence Header
} SdPVR_Picture_k;

/** 
 * @enum SdPVR_CallbackType_k
 * Install�� Callback �Լ��� Type ����. Type of Callback function to be installed
 */
typedef enum
{
	SD_PVR_CALLBACK_CATALOG,
	SD_PVR_CALLBACK_STREAM,
} SdPVR_CallbackType_k;

/** 
 * @enum SdPVR_Table_k
 * Record�߿� ���Խ�ų Table ����. Table information to be inserted while recording
 */
typedef enum
{
	SD_PVR_TABLE_PAT,
	SD_PVR_TABLE_PMT,
	SD_PVR_TABLE_SIT,
} SdPVR_Table_k;

typedef struct
{
	SdVideoFormat_k eVidFormat;	///< PVR Source�� �Ǵ� Stream�� Video Format. Video format of PVR Source Stream
	SdCPKey_t	sEncrytKey;		///< Packet ����� ��ȣȭ ��⿡�� ����ϴ� Key��. Encryption Key value
} SdPVR_RecordSettings_t;

/**
* @brief �������� stream data�� �޾��� �� ȣ��Ǵ� callback �Լ�. callback called when certain amount of stream data is received
* @remarks callback�� �߻��ϴ� stream data�� �������� ���������� ���ǵ�. the amount of stream data is defined internally
* @param pucBuffer  stream data�� ����� DMA buffer �ּ�. DMA buffer address of stream data
* @param uwBufferSize  DMA buffer ũ��. size of DMA buffer
* @param uwParam  ����ڰ� �̸� ������ ����. User predefined parameter
*/
typedef void (*SdPVR_RecordStreamCallback)(uint8_t *pucBuffer, uint32_t uwBufferSize, uint32_t uwParam);


/**
* @brief Catalog data�� �߻����� �� ȣ��Ǵ� callback �Լ�. callback called when Catalog data is received
* @remarks 
* @param uwPacketCount  kPicture�� ���޵Ǵ� I/B/P Frame�� packet ��ġ. Packet position of I/B/P Frame transfered to kPicture
* @param kPicture  I/B/P Frame Information
* @param uwParam  ����ڰ� �̸� ������ ����. User predefined parameter
*/
typedef void (*SdPVR_RecordCatalogCallback)(uint32_t uwPacketCount, SdPVR_Picture_k kPicture, uint32_t uwParam);

/**
 * @brief PVR ���� Device���� �ʱ�ȭ�Ͽ� ����ڰ� ����� �� �ֵ��� �Ѵ�.
 It initialize the PVR-related Devices so that user can use them.
 * @remarks
 * @param ePowerState Power state: power off->on, off->standby, or standby->on. Default is off->on.
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
SdResult SdPVR_Init(SdPowerState_k ePowerState);

/**
* @brief PVR Record�� �ʿ��� ����(Video Format, ��ȣȭ Ű��)�� ������.
It sets the information (Video Format, encryption key value) which is required upon recording PVR.
* @remarks RecordConnect()���� �׻� ���� ȣ��Ǿ�� �ϴ� �Լ��̴�. RecordConnect()�� ���⼭ ������ ������ �����Ͽ� Connect�۾��� �����Ѵ�. 
RecordConnect���Ŀ� ���� ����Ǵ� ��쿡�� �׿� ������ �߰� Action�� �߻��Ѵ�.
It is executed before RecordConnect(). RecordConnect() used information set by this function.
If it is called after RecordConnect(), the corresponding action should be performed.
* @param eRecordId
* @param pSettings
* @return SdResult SD_OK/SD_NOT_OK
* @see 
*/
SdResult SdPVR_RecordSet(SdRecordID_k eRecordId, SdPVR_RecordSettings_t* pSettings);

/**
* @brief PVR Record�� ������ ������ �о��
It gets the information which is set in PVR Record.
* @remarks 
* @param eRecordId
* @param pSettings
* @return SdResult SD_OK/SD_NOT_OK
* @see 
*/
SdResult SdPVR_RecordGet(SdRecordID_k eRecordId, SdPVR_RecordSettings_t* pSettings);

/**
* @brief callback �Լ� ������
It sets the Callback function.
* @remarks 
* @param eRecordId
* @param eCallbackType
* @param Callback  callback �Լ��� �ּ�. callback function
* @param uwParam  callback ȣ�� �� uwParam�� ���޵� ����. parameter of the callback
* @return SdResult SD_OK/SD_NOT_OK
* @see 
*/
SdResult SdPVR_InstallRecordCallback(SdRecordID_k eRecordId, SdPVR_CallbackType_k eCallbackType, void * Callback, uint32_t uwParam);

/**
 * @brief Record Path�� Connect��
 It connects the Record Path.
 * @remarks �׻� Connect������ SdPVR_RecordSet()�� �̿��Ͽ� Video Format�� Encrypt Key�� �̸� �������־�� �Ѵ�.
 SdPVR_RecordSet() should be called before this function to get video format and encrypt key.
 * @param eRecordId
 * @param DmxHndl  �Է� Stream�� �ҽ��� �Ǵ� Demuxer�� handle��. demux handle for input stream
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdPVR_RecordConnect(SdRecordID_k eRecordId, SdAVDec_DmxHandle DmxHndl);

/**
 * @brief ������ Record Path�� Disconnect��
 It disconnects the Record Path.
 * @remarks 
 * @param eRecordId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdPVR_RecordDisconnect(SdRecordID_k eRecordId);

/**
 * @brief PVR ��ȭ�� ������
 It starts the PVR recording.
 * @remarks Digital �ҽ��� ���� Pids ������ SdAVDec����� SdAVDec_StreamOutSetPid() API�� �̿��Ѵ�.
 Use SdAVDec_StreamOutSetPid() in SdAVDec for settings of Pids of digital source 
 * @param eRecordId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdPVR_RecordStart(SdRecordID_k eRecordId);

/**
 * @brief PVR ��ȭ�� ������
 It stops the PVR recording.
 * @remarks 
 * @param eRecordId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdPVR_RecordStop(SdRecordID_k eRecordId);

/**
 * @brief Record�� ����� Table Packet�� ������ �ڿ� �Ҵ�� �ʱ�ȭ ������
 It allots the Table Packet-related resource to be saved upon recording and performs the initialization.
 * @remarks 
 * @param eRecordId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdPVR_RecordTableOpen(SdRecordID_k eRecordId);

/**
 * @brief Record�� ����� Table Packet�� ������ �ڿ� ��ȯ �� ������
 It returns and arranges the Table Packet-related resources to be saved upon Recording.
 * @remarks 
 * @param eRecordId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdPVR_RecordTableClose(SdRecordID_k eRecordId);

/**
 * @brief Record�� ����� Table Packet�� ������
 It sets the Table Packet to be saved upon Recording.
 * @remarks Table Packet�� �����ϴ� ����� HW ������Ŀ� ���� Insert��İ� Replace������� ������.
 Table Packet setting method is defined internally as either Insert method or Replace method depending on the HW support. 
 * @param eRecordId
 * @param pTablePackets ����� Table Packet�� �迭�μ� 188byte�� ��� ũ�⸦ ���´�. Table Replace����� ���� 188byte�� �迭������� �����̴�.
 The pointer of Table Packet array and the size is a multiple of 188 byte. This size is fixed to be 188byte for Replace method.
 * @param NumOfPackets �Էµ� Table Packet�迭������ Packet���̴�. Table Insert����� ��� ���� Table Type�� �Է� Packet ���̰�, Table Replace����� ��쿡�� ������ 1�̴�.
 The number of packets in the Table Packet array. It is fixed to be 1 for Replace method.
 * @param hwParam Table Insert����� ��� Table Packet ���� �ֱⰪ(����:ms)�̰�, Table Replace����� ��� Replace�� Target�� �Ǵ� Pid ���̴�.
 For Insert method it is Table Packet insertion period (unit: ms) and for Replace method it is Pid to be replaced.
 * @param eTableType ����� Table�� Type�� ����. Type of Table to be saved
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdPVR_RecordTableSet(SdRecordID_k eRecordId, uint8_t *pTablePackets, uint32_t NumOfPackets, uint32_t hwParam, SdPVR_Table_k eTableType);

/**
 * @brief �ش� Table Packet�� ���� �۾��� ������
 It stops the work of inserting the concerned Table Packet.
 * @remarks 
 * @param eRecordId
 * @param eTableType ����� Table�� Type�� ����. Type of Table to be saved
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdPVR_RecordTableClear(SdRecordID_k eRecordId, SdPVR_Table_k eTableType);

/**
 * @brief Record Stream�� ������� Table Packet ������ ������
 It starts inserting the Table Packet which has been created in Record Stream.
 * @remarks 
 * @param eRecordId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdPVR_RecordTableStart(SdRecordID_k eRecordId);

/**
 * @brief Record Stream�� ������� Table Packet ������ ������
 It stops the inserting the Table Packet which has been created in Record Stream.
 * @remarks 
 * @param eRecordId
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdPVR_RecordTableStop(SdRecordID_k eRecordId);

/**
 * @brief PVR ��� ���� AVDec��Ⱓ�� ������ ������
 It sets the connection between PVR playback module and AVDec module.
 * @remarks Timestamp ��� ����. ����� ������ Decoder ������ SdAVDec ����� API�� ����Ѵ�. 
 Connection for Timestamp module. Connection for playback decoder is set by SdAVDec.
 * @param ePlayId 
 * @param eAVDecId 
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdPVR_PlayConnect(SdPlayID_k ePlayId, SdAVDecID_k eAVDecId);

/**
 * @brief PVR ��� ���� AVDec��Ⱓ�� ������ ������
 It releases the connection between PVR playback module and AVDec module.
 * @remarks 
 * @param ePlayId 
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdPVR_PlayDisconnect(SdPlayID_k ePlayId);

/**
 * @brief PVR ����� ������
 It starts the PVR playback.
 * @remarks Timestamp ��� ����
 * @param ePlayId 
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdPVR_PlayStart(SdPlayID_k ePlayId);

/**
 * @brief PVR ����� ������
 It stops the PVR playback.
 * @remarks 
 * @param ePlayId 
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdPVR_PlayStop(SdPlayID_k ePlayId);

/**
 * @brief ��밡���� Play Buffer�� ����
 It gets the available Play Buffer.
 * @remarks 
 * @param ePlayId
 * @param ppucBuffer Buffer ������ ����� �ּ�. SdPVR�� ����ϴ� ��⿡�� �Ҵ��ؼ� �����ϴ� ���� �ƴ϶� �Լ��� ȣ���ϸ� ���������� �Ҵ�ǰ� �� �ּҸ� ��ȯ�ȴ�.
 Buffer ������ �����ϱ� ���� ���Ǵ� ������ SdPVR���ο� static array�� SI Buffer ���� ��ŭ �����ϸ� �Ҵ�/������ ������ �ʿ���� ���� �� �ִ�.
 Buffer Address. The buffer is allocated internally in SdPVR and returned.
 
 * @param puwBufferSize Buffer�� ũ��. size of Buffer
 * @param puwNumBuffers ppucBuffer�� ����� Buffer �ּ��� ����. The number of Buffer addresses stored in ppucBuffer
 * @param ppVideoBaseAddr VideoBase ��巹���� ���´�. The address of VideoBase
 * @param TrickState
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdPVR_PlayGetBuffers(SdPlayID_k ePlayId, uint8_t** ppucBuffer, uint32_t* puwBufferSize, uint32_t* puwNumBuffers, uint8_t** ppVideoBaseAddr, uint32_t TrickState);

/**
 * @brief �ش� Play Buffer�� ����Ÿ�� Demux �̵��� ��û��
 It requests to shift the concerned Play Buffer's data to Demux.
 * @remarks 
 * @param ePlayId
 * @param pucBuffer Stream data�� ����� Buffer�� �ּҰ� Array. Array of Buffer addresses storing Stream data.
 * @param uwNumBuffers Stream data�� ����� Buffer�� ����. The number of Buffers storing Stream data
 * @param ppucoffsetBuffer �̵��� SI Offset������ ���´�. Address of SI Offset
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdPVR_PlaySubmit(SdPlayID_k ePlayId, uint8_t *pucBuffer, uint32_t uwNumBuffers, uint8_t** ppucoffsetBuffer);

/**
 * @brief �޸𸮻��� ��ȣȭ�� data�� �ص��Ͽ� �ٸ� �޸𸮿� �����Ѵ�.
 It decrypts the encrypted data in memory and copy to another address.
 * @remarks ���� ������� ����� ���� �۾����� ���ȴ�. This function is generally used in creating a thumbnail.
 * @param pDestination �ص��� data�� ����� �޸� �ּ�. Memory address for decrypted data.
 * @param pSource ��ȣȭ�� data�� ����� �޸� �ּ�. Memory addres for encrypted source data.
 * @param ucBuffersize �ҽ� data�� ����� ������ ũ��(�ص��� data�� ũ��). size of source data to be decrypted
 * @param sDecryptKey ��ȣ�ص��� ���Ǵ� Key��. Key for the decryption
 * @return SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdPVR_DecryptMemdata(uint8_t* pDestination, uint8_t* pSource, uint32_t ucBuffersize, SdCPKey_t sDecryptKey);

/**
 * @brief SdPVR API�� Manual Debug �Լ�
 Manual Debug function of SdPVR API
 * @remarks SdDebug.h�� ���ǵ� SdDebug()�� ���� ȣ��ȴ�. It is called by SdDebug() in SdDebug.h. 
 * @return 
 * @see 
 */
void SdPVR_Debug(void);

#endif	/*_SDPVR_H_*/
