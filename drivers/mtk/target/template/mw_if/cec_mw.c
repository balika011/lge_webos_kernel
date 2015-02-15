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
/*-----------------------------------------------------------------------------
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: cec_mw.c,v $
 * $Revision: #1 $
 *
 * Description:
 *         CEC middleware driver API
 *---------------------------------------------------------------------------*/

#include "drv_hdmicec.h"

#include "general.h"

#include "x_printf.h"
#include "x_assert.h"
#include "x_debug.h"
#include "x_cec.h"
#include "x_rm.h"
#include "x_rm_dev_types.h"
// /project_x/middleware/res_mngr/drv/x_cec.h

static CEC_NFY_INFO_T  _CECMWNfyInfo;
static UINT8 CEC_MW_Status_Ready = FALSE;

static void _CECMWSetReady()
{
	CEC_MW_Status_Ready = TRUE;
}

UINT8 _CECMWQueryReady()
{
	return CEC_MW_Status_Ready;
}

static INT32 _CECMWWakeCtrl(CEC_WAKE_UP_CTRL_T* wakeup)
{
	#ifdef SUPPORT_WELTREND_UP
	UINT8 data[3];

	if (wakeup->e_cec_fct_ctrl == CEC_FCT_CTRL_OFF)
	{
		vWTCECOnOff(SV_OFF);
	}
	else
	{
		vWTCECOnOff(SV_ON);
	}

	data[0] = (UINT8)wakeup->e_cec_ver;
	vWTSetCECVersion(data);

	data[0] = wakeup->t_dev_vndr_id[0];
	data[1] = wakeup->t_dev_vndr_id[1];
	data[2] = wakeup->t_dev_vndr_id[2];
	vWTSetMenuLang(data);

	data[0] = wakeup->t_menu_lang[0];
	data[1] = wakeup->t_menu_lang[1];
	data[2] = wakeup->t_menu_lang[2];
	vWTSetMenuLang(data);

	vWTSetPhyAddr();
	#endif /* SUPPORT_WELTREND_UP */

	return RMR_OK;
}

static INT32 _CECMWSetNfy(CEC_NFY_INFO_T* nfyinfo)
{
	if (nfyinfo->pf_cec_nfy == NULL)
	{
		return RMR_DRV_INV_SET_INFO;
	}

	_CECMWNfyInfo.pv_nfy_tag = nfyinfo->pv_nfy_tag;
	_CECMWNfyInfo.pf_cec_nfy = nfyinfo->pf_cec_nfy;

	return RMR_OK;
}

void _CECMWNfyDataArrival(CEC_FRAME_DESCRIPTION* frame)
{
	CEC_NFY_T cec_nfy;
	UINT8 i;

	cec_nfy.e_nfy_type = CEC_NFY_TYPE_FRAME;
	cec_nfy.u.t_frame_info.ui1_init_addr = frame->blocks.header.initiator;
	cec_nfy.u.t_frame_info.ui1_dest_addr = frame->blocks.header.destination;
	cec_nfy.u.t_frame_info.ui2_opcode = frame->blocks.opcode;
	cec_nfy.u.t_frame_info.z_operand_size = frame->size-2;
	for (i=0; i<frame->size-2; i++)
	{
		cec_nfy.u.t_frame_info.aui1_operand[i] = frame->blocks.operand[i];
	}

	if (_CECMWNfyInfo.pf_cec_nfy)
	{
		_CECMWNfyInfo.pf_cec_nfy(&cec_nfy, _CECMWNfyInfo.pv_nfy_tag, 0);
	}
}

void _CECMWNfyTxResult(UINT8 result, void* txtag)
{
	CEC_NFY_T cec_nfy;

	cec_nfy.e_nfy_type = CEC_NFY_TYPE_ACK;
	cec_nfy.u.t_ack_info.pv_tag = txtag;
	if (result == 0x00)
	{
		cec_nfy.u.t_ack_info.e_ack_cond = CEC_ACK_COND_OK;
	}
	else if (result == 0x01)
	{
		cec_nfy.u.t_ack_info.e_ack_cond = CEC_ACK_COND_NO_RESPONSE;
	}

	if (_CECMWNfyInfo.pf_cec_nfy)
	{
		_CECMWNfyInfo.pf_cec_nfy(&cec_nfy, _CECMWNfyInfo.pv_nfy_tag, 0);
	}
}

#define LOCAL_CEC_MAX_OPERAND_SIZE 14
static CEC_FRAME_DESCRIPTION cecMwTxMsg;
static INT32 _CECMWSend(CEC_SEND_MSG_T* msg)
{
	INT32 i4Ret;
	UINT8 i;

	// input
	// msg->pv_tag
	// msg->t_frame_info.ui1_init_addr
	// msg->t_frame_info.ui1_dest_addr
	// msg->t_frame_info.ui2_opcode
	// msg->t_frame_info.aui1_operand[]
	// msg->t_frame_info.z_operand_size
	if ((msg->t_frame_info.ui1_init_addr > 0xf) || (msg->t_frame_info.ui1_dest_addr > 0xf) || (msg->t_frame_info.z_operand_size > LOCAL_CEC_MAX_OPERAND_SIZE))
	{
		return RMR_DRV_INV_SET_INFO;
	}
	
	cecMwTxMsg.txtag = msg->pv_tag;
	cecMwTxMsg.blocks.header.initiator = msg->t_frame_info.ui1_init_addr;
	cecMwTxMsg.blocks.header.destination = msg->t_frame_info.ui1_dest_addr;
	cecMwTxMsg.sendidx = 0;
	cecMwTxMsg.reTXcnt = 0;
	if (msg->t_frame_info.ui2_opcode == 0xffff)
	{
		cecMwTxMsg.size = 1;
	}
	else
	{
		cecMwTxMsg.blocks.opcode = msg->t_frame_info.ui2_opcode;
		cecMwTxMsg.size = msg->t_frame_info.z_operand_size + 2;
	}
	for (i=0; i<cecMwTxMsg.size-2; i++)
	{
		cecMwTxMsg.blocks.operand[i] = msg->t_frame_info.aui1_operand[i];
	}
	
	i4Ret = _CEC_TX_Enqueue(&cecMwTxMsg);
	if (i4Ret == 0x01)
	{
		msg->b_enqueue_ok = FALSE;
		return RMR_DRV_INV_SET_INFO;
	}
	else
	{
		msg->b_enqueue_ok = TRUE;
		return RMR_OK;
	}

	// output
	// RMR_OK
	// RMR_NOT_ENOUGH_SPACE
}

static INT32 _CECMWGet(DRV_COMP_ID_T* ptCompId, DRV_GET_TYPE_T eType, VOID* pvData, SIZE_T* pzSize)
{
	INT32 i4Ret = RMR_OK;

	// Check component ID
	if (ptCompId == NULL || ptCompId->e_type != DRVT_HDMI_CEC)
	{
		return RMR_DRV_INV_GET_INFO;
	}

	switch (eType)
	{
		#if 0
		case CEC_GET_TYPE_COMP_ID:
			if (*pzSize < sizeof(CEC_COMP_ID_T))
			{
				*pzSize = sizeof(CEC_COMP_ID_T);
				return RMR_DRV_NOT_ENOUGH_SPACE;
			}
			break;
		#endif
		default:
			i4Ret = RMR_DRV_INV_GET_INFO;
			break;
	}

	return i4Ret;
}

static INT32 _CECMWSet(DRV_COMP_ID_T* ptCompId, DRV_SET_TYPE_T eType, const VOID* pvData, SIZE_T zSize)
{
	INT32 i4Ret = RMR_OK;

	// Check component ID
	if (ptCompId == NULL || ptCompId->e_type != DRVT_HDMI_CEC)
	{
		return RMR_DRV_INV_SET_INFO;
	}

	switch (eType)
	{
		case CEC_SET_TYPE_SEND_MSG:
			if ((pvData == NULL) || zSize < sizeof(CEC_SEND_MSG_T))
			{
				return RMR_DRV_INV_SET_INFO;
			}
			i4Ret = _CECMWSend((CEC_SEND_MSG_T*)pvData);
			break;
		case CEC_SET_TYPE_NFY_FCT:
			if ((pvData == NULL) || zSize < sizeof(CEC_NFY_INFO_T))
			{
				return RMR_DRV_INV_SET_INFO;
			}
			i4Ret = _CECMWSetNfy((CEC_NFY_INFO_T*)pvData);
			break;
		case CEC_SET_TYPE_WAKE_UP_CTRL:
			if ((pvData == NULL) || zSize < sizeof(CEC_WAKE_UP_CTRL_T))
			{
				return RMR_DRV_INV_SET_INFO;
			}
			i4Ret = _CECMWWakeCtrl((CEC_WAKE_UP_CTRL_T*)pvData);
			break;
		case CEC_SET_TYPE_SIGNAL_CEC_READY:
			_CECMWSetReady();
			break;
		default:
			i4Ret = RMR_DRV_INV_SET_INFO;
			break;
	}

	return i4Ret;
}

INT32 _CECMWInit(void)
{
	INT32 i4Ret;
	UINT32 u4Flags;
	DRV_COMP_REG_T tCECCompReg;
	DRV_COMP_FCT_TBL_T tCECFctTbl;

	/* Setup component ID */
	tCECCompReg.e_type = DRVT_HDMI_CEC;
	tCECCompReg.e_id_type = ID_TYPE_IND;
	tCECCompReg.u.t_ind.ui2_id = 0;
	tCECCompReg.u.t_ind.pv_tag = NULL;
	tCECCompReg.u.t_ind.ui1_port = 0;

	/* Registration Flags */
	u4Flags = 0;

	/* Setup component function table */
	tCECFctTbl.pf_rm_connect = NULL;
	tCECFctTbl.pf_rm_disconnect = NULL;
	tCECFctTbl.pf_rm_get = _CECMWGet;
	tCECFctTbl.pf_rm_set = _CECMWSet;

	/* Register component with the Resource Manager database */
	i4Ret = x_rm_reg_comp(&tCECCompReg,
			1,
			1,
			NULL,
			u4Flags,
			&tCECFctTbl,
			NULL,
			0);

	if (i4Ret != RMR_OK)
	{
		LOG(1, "MW_HdmiCECInit(): x_rm_reg_comp failed [%d]\n", (INT32)i4Ret);
	}
	return i4Ret;
}
#if 0
INT32 x_rm_reg_comp (DRV_COMP_REG_T*      pt_comp_id,
                        UINT8                ui1_num_inp_ports,
                        UINT8                ui1_num_out_ports,
                        const CHAR*          ps_comp_name,
                        UINT32               ui4_comp_flags,
                        DRV_COMP_FCT_TBL_T*  pt_comp_fct_tbl,
                        const VOID*          pv_comp_data,
                        SIZE_T               z_comp_data_len)
#endif
