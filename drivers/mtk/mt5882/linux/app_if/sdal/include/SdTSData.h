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
 * @file SdTSData.h
 * @brief TSData header file
 Samsung DAL���� TSData�� ���� API�� ��� ����.
 Section Data�� M/W ���� ������ �ִ� ����� �����Ѵ�.
 Open, Close, StartMonitor, StopMonitor���� �Լ��� �����Ǹ�
 StartMonitorȣ���Ŀ� ��û�Ǵ� Section Data�� callback �Լ��� ������ �ش�.
 Samsung DAL gathers together the API for TSData. 
 It performs the feature of transmitting the Section Data to M/W. 
 It consists of Open, Close, StartMonitor and StopMonitor etc. 
 and transmits the Section Data to be requested after calling the StartMonitor to the callback function.
 \image html SdTSData_Diagram.jpg "SdTSData Data Flow & Function Block Diagram"
 * @author S/W Platform Lab
 * @date 2006/07/10
 */

#ifndef	_SDTSDATA_H_
#define	_SDTSDATA_H_

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

typedef uint32_t	SdTSData_MonHandle;

typedef enum
{
	SD_TSDATA_TYPE_SECTION_TABLEID,
	SD_TSDATA_TYPE_SECTION_FILTERED,
	SD_TSDATA_TYPE_PES,
	SD_TSDATA_TYPE_MAX,	
} SdTSData_Type_k;

typedef struct
{	
	SdTSData_MonHandle monHndl; ///< Monitor Handle. Allocated in SdTSData_StartMonitor
	int32_t pid; ///< Packet ID	
	uint8_t *pData; ///< Section data buffer pointer
	uint32_t length; ///< Section data length	
} SdTSData_SettingsCallBack_t;

typedef struct
{
	int32_t pid; ///< Packet ID
	SdTSData_Type_k	eDataType; ///< TSData Type
	union{		
		struct{
			int16_t _tableId; ///< Section table ID
			SdBool_t _bVersion; ///< Version �� CRC Check ���� ����. Version and CRC Check On/Off
		} sTab;
		struct{
			uint8_t *_pFilterData; ///< Filter data buffer pointer
			uint32_t _Filterlength; ///< Filter data length
			uint8_t *_pMask;
			uint8_t *_pInvert; ///< 0:Positive 1:Negative(Invert)
		} sFil;
	} uParam;
	SdBool_t bHbrMode; ///< High Bitrate Section support
} SdTSData_Settings_t;

/**
 * @brief Section Data ó���� ���� Callback�Լ� Type. Function prototype of callback for processing Section Data
 * @remark Section Data�� �ԷµǾ����� ȣ��ǰ�, SdTSData_StartMonitor()���� ������ SdTSData_Settings_t�� bVersion ���� ������ �޴´�.
  bVersion���� False�� ��� �ش� Section Data�� �����ϸ� �׻� ȣ��, True�� ��� �ش� Section Data�� version�� �ٲ�ų�, CRC���� �޶��� ��쿡�� ȣ��. 
 The callback function is called when Section Data is detected. bVersion in SdTSData_Settings_t, set by SdTSData_StartMonitor(), affects the behavior of callback.
 If bVersion is False, the callback is called every time the corresponding Section Data is detected.
 If True, the callback is called only when the version is updated or CRC is changed.
 * @param pSettings �����ؾ��� Section Data �� �� ����. Information of Section Data
 * @see 
 */
typedef void (*SdTSData_CallBack)(SdTSData_SettingsCallBack_t *pSettings);

/********************************************************************
	EXTERN VARIABLES & FUNCTION PROTOTYPES DECLARATIONS
********************************************************************/

/**
 * @brief TSData ��� ��ü�� initialize �Ѵ�
 It initializes the whole TSData module. 
 * @remarks SdTSData_Init �Լ��� �ѹ��� ȣ�����ָ� �ȴ�
 The SdTSData_Init function will be called up once.
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdTSData_Init(void);

/**
 * @brief TSData ����� callback function�� ����� �ش�
 It registers the callback function of TSData module.
 * @remarks ��ϵ� callback �Լ��� ���ؼ� filtering �� TS data�� �Ѱ� �ش�
 It passes over the filtered TS data through the registered callback function.
 * @param dmxHndl Demux Handle
 * @param callback TS Data�� �Ѱ� ���� callback �Լ�. callback for TS Data
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdTSData_InstallCallback(SdAVDec_DmxHandle dmxHndl, SdTSData_CallBack callback);

/**
 * @brief TSData ����� callback function�� ����� ���� �Ѵ�.
 It releases the registeration of callback function of the TSData module.
 * @remarks
 * @param dmxHndl Demux Handle
 * @param callback TS Data�� �Ѱ� �޵��� ��ϵ� callback �Լ�. the callback installed at SdTSData_InstallCallback()
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdTSData_UnInstallCallback(SdAVDec_DmxHandle dmxHndl, SdTSData_CallBack callback);

/**
 * @brief TS data filtering�� �����Ѵ�
 It starts the TS data filtering. 
 * @remarks ���ϴ� pid, tableid�� �־��ְ� bVersion�� version�� �ٲ� ���� data�� �������� ������ �����Ѵ�.
 SdTSData_Open �Ŀ� SdTSData_InstallCallback�� ���־�� �Ѵ�.
 The desired pid, tableid will be given and the bVersion will decide whether to pass over the data or not only when its version is revised.
 SdTSData_InstallCallback should be performed after carrying out the SdTSData_InstallCallback.
 * @param dmxHndl Demux Handle
 * @param pSettings monitoring setting
 * @return TSData Monitor handle
 * @see SdTSData_CallBack
 */
SdTSData_MonHandle SdTSData_StartMonitor(SdAVDec_DmxHandle dmxHndl, SdTSData_Settings_t *pSettings);

/**
 * @brief TS data filtering ������ ���� �����
 It stops the start of TS data filtering.
 * @remarks SdTSData_StartMonitor �Լ��� ���ؼ� ������ �ش� pid filtering�� stop�Ѵ�. 
 SdTSData_Close �ϱ� ���� SdTSData_Close�� handle�� SdTSData_StopMonitor�� ��� release����� �Ѵ�.
 It stops the concerned pid filtering which has been set through the SdTSData_StartMonitor function. 
 the Handle which has undergone the SdTSData_Close should be all released by using the SdTSData_StopMonitor 
 before performing the SdTSData_Close.
 * @param dmxHndl Demux Handle
 * @param hM TSData Monitor handler (SdTSData_StartMonitor return value)
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult SdTSData_StopMonitor(SdAVDec_DmxHandle dmxHndl, SdTSData_MonHandle hM);

/**
 * @brief SdTSData API�� Manual Debug �Լ�
 Manual Debug function of SdTSData API 
 * @remarks SdDebug.h�� ���ǵ� SdDebug()�� ���� ȣ��ȴ�. It is called by SdDebug() in SdDebug.h. 
 * @return 
 * @see 
 */
void SdTSData_Debug(void);

#endif

