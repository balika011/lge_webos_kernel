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
 * @file SdAVEnc.h
 * @brief Encoder Function Define Header File
 * @author S/W Platform Lab
 * @date 2007/03/16
 * @version		
 */
 
#ifndef _SDAVENC_H_
#define _SDAVENC_H_

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

typedef	enum  {
	SD_AVENC_STATE_POWERDOWN = 0,
	SD_AVENC_STATE_IDLE,
	SD_AVENC_STATE_STOP,
	SD_AVENC_STATE_PAUSE,
	SD_AVENC_STATE_ENCODE,
	SD_AVENC_STATE_MAX
} SdAVEnc_State_k;

typedef struct
{
	int32_t	videoQuality;		///< Video Quality for encoding
} SdAVEnc_Settings_t;

typedef struct
{	
	SdAVEnc_State_k	eEncoderState;	///< Encoder Current State	
} SdAVEnc_Status_t;

/********************************************************************
	EXTERN VARIABLES & FUNCTION PROTOTYPES DECLARATIONS
********************************************************************/

/**
 * @brief Encoder Device�� �ʱ�ȭ�Ͽ� ����ڰ� ����� �� �ֵ��� �Ѵ�. Initialize the Encoder Device.
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
SdResult	SdAVEnc_Init(SdPowerState_k ePowerState);

/**
 * @brief	Encoder path�� �����ϴ� �Լ�. Encoder path connection
 * @remarks	Encoder�� �Է� �ҽ��� �����Ѵ�. Connect between Encoder and Input Source
 * @remarks ������ Disconnect ���¿� ���� �Է¸� ���� �Ǵ� ��/��� ��� �����ؾ� �Ѵ�. Connect Input or Input/Output depending on the previous disconnect status.
 * @param	eAVEncId	�����Ϸ��� Encoder ID. Encoder ID to be connected
 * @param	eInputSrc	Encoder�� �ԷµǴ� �ҽ���. Input Source
 * @return	SdResult SD_OK/SD_NOT_OK
 * @see 
 */
SdResult	SdAVEnc_Connect(SdAVEncID_k eAVEncId, SdSource_k eInputSrc);

/**
 * @brief	Encoder path�� ������ �����ϴ� �Լ�. Release the connection of the Encoder path
 * @remarks Connect�� ���� ����� Encoder Component�� ������ �����Ѵ�. Release the connection of the Encoder Component by the function SdAVEnc_Connect
 * @param eAVEncId	path ������ �����ϰ��� �ϴ� Encoder ID. Encoder ID to be released
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult	SdAVEnc_Disconnect(SdAVEncID_k eAVEncId);

/**
 * @brief	Encoder�� ���� Encoding �۾��� �ϵ��� �Ѵ�. Start Encoding
 * @remarks
 * @param eAVEncId	Encoder ID
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult	SdAVEnc_Start(SdAVEncID_k eAVEncId);

/**
 * @brief	Encoder�� Encoding �۾��� �ߴ��Ѵ�. Stop Encoding
 * @remarks
 * @param eAVEncId	Encoder ID
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult	SdAVEnc_Stop(SdAVEncID_k eAVEncId);

/**
 * @brief	Encoder�� ���� ���� ���¸� ����. Get the Encoder settings
 * @remarks
 * @param eAVEncId	Encoder ID
 * @param pSettings	[out]���� �о�� Encoder ���� �������� ������ ���� ������ �ִ�. 
 video encoding Quality ������ �ִ�.
 [out]The saved Encoder settings. It contains the video encoding Quality information.
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult	SdAVEnc_Get(SdAVEncID_k eAVEncId, SdAVEnc_Settings_t * pSettings);

/**
 * @brief	Encoder ������ ���� �ʿ��� �������� �����ϵ��� �Ѵ�.
 It sets the setting for the Encoder
 * @remarks	�����Ϸ��� ���� SdAVEnc ���������� ����� ���� ���Ͽ� ���� �ٸ� �͸� �����Ѵ�.
 ���� ���ϴ� �͸� �����ϱ� ���ؼ��� SdAVEnc_Get�� �̿��Ͽ� ���� ����� ���� �� �缳���Ϸ��� �͸� ���� �����Ͽ� ȣ���Ѵ�.
 It compares pSettins and the saved settings, and sets only the changed valued.
 It is advised that call SdAVEnc_Get first, change only the required settings and then call SdAVEnc_Set.
 * @param eAVEncId	Encoder ID
 * @param pSettings	Encoder�� �����Ϸ��� ������ ������ ���� ������ �ִ�.
 video encoding Quality ������ �ִ�.
 The value of Encoder settings to be written. It contains the video encoding Quality information.
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult	SdAVEnc_Set(SdAVEncID_k eAVEncId, SdAVEnc_Settings_t * pSettings);

/**
 * @brief	Encoder�� ���¸� ���´�. Get the status of Encoder
 * @remarks	Encoder�� ���õ� ���¸� ������ ������ �� ������ ���������� �ʴ´�.
 mask parameter�� �̿��Ͽ� ���⸦ ���ϴ� ���� ������ �� �ִ�.
 mask�� or�� �̿��Ͽ� ���� ���� ���� �� �ִ�.
 It gets the status and does not set any values.
 Use mask or operation to get multiple status.
 * @param	eAVEncId	Encoder ID
 * @param	mask	���� ������ ��Ÿ���� ����ü���� ���Ϸ��� ������ ��Ÿ���� ����. mask of status information
 * @param	pState	[out]���� �о�� Encoder�� ���·� ������ ���� ������ �ִ�.
 	Encoder�� ���� State ������ �ִ�.
 	[out]The Encoder status. It contains Encoder current status.
 * @param	timeout
 * @return SD_OK/SD_NOT_OK
 * @see 
 */
SdResult	SdAVEnc_GetStatus(SdAVEncID_k eAVEncId, SdMask_t mask, SdAVEnc_Status_t * pState, uint32_t timeout);

/**
 * @brief SdAVEnc API�� Manual Debug �Լ�
 * @remarks SdDebug.h�� ���ǵ� SdDebug()�� ���� ȣ��ȴ�. It is called by SdDebug() in SdDebug.h. 
 * @return 
 * @see 
 */
void SdAVEnc_Debug(void);

#endif //_SDAVENC_H_
