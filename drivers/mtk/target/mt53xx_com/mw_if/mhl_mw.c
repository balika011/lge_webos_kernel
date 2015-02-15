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
 * $RCSfile: mhl_mw.c,v $
 * $Revision: #1 $
 *
 * Description:
 *         MHL middleware driver API
 *---------------------------------------------------------------------------*/

#include "general.h"

#include "mhl_if.h"
#include "x_printf.h"
#include "x_assert.h"
#include "x_debug.h"
#include "x_cec.h"
#include "x_rm.h"
#include "x_rm_dev_types.h"
#include "x_socket_dev.h"

#ifdef LINUX_TURNKEY_SOLUTION
#include "cb_data_rm.h"
#include "cb_param.h"
#endif

#ifdef SYS_MHL_SUPPORT

#define MHL_HW_PORT_NUM 1
static  SKTDEV_MHL_ST_NFY_T _MHLMWNfyInfo[MHL_HW_PORT_NUM];

void _MHLMWNfyOcp(UINT8 port, MHL_OC_STATUS result)
{
    SKTDEV_MHL_ST_NFY_T *ptMWMHLOCNfyInfo;
    SKTDEV_MHL_STATUS_T e_nfy_st;
	UINT8 uMhlConnect;

    if(port >= MHL_HW_PORT_NUM)
    {
        return;
    }
    
    ptMWMHLOCNfyInfo = (SKTDEV_MHL_ST_NFY_T *)&_MHLMWNfyInfo[port];

    if ((ptMWMHLOCNfyInfo) && (ptMWMHLOCNfyInfo->pf_sktdev_mhl_st_nfy))
    {
        e_nfy_st = (result.MhlOCStatus) ? 
            SKTDEV_MHL_STATUS_OVERCURRENT: SKTDEV_MHL_STATUS_NORMAL;
		uMhlConnect = result.MhlDeviceConnect;
		
		//printf("______result=%d______\n",result);
#ifndef LINUX_TURNKEY_SOLUTION
        ptMWMHLOCNfyInfo->pf_sktdev_mhl_st_nfy(ptMWMHLOCNfyInfo->pv_tag, e_nfy_st, &uMhlConnect);
#else
       //cec_nfy_fct(ptMWMHLOCNfyInfo.pf_cec_nfy, &cec_nfy, ptMWMHLOCNfyInfo.pv_nfy_tag, 0);
#endif

    LOG(3,"#1 driver OCP nfy port:%d, oc status= %d,mhl connect=%d \n", port, result.MhlOCStatus,result.MhlDeviceConnect);
    }

    LOG(3,"#2 driver OCP nfy port:%d, result %d \n", port, result);

}

static INT32 _MHLMWGet(
    DRV_COMP_ID_T  *pt_comp_id,
    DRV_GET_TYPE_T  e_get_type,
    VOID           *pv_get_info,
    SIZE_T         *pz_get_info_len)
{
    INT32 i4Ret = RMR_OK;

    // Check component ID
    if (pt_comp_id == NULL || pt_comp_id->e_type != DRVT_HW_MHL)
    {
        return RMR_DRV_INV_GET_INFO;
    }

    switch (e_get_type)
    {
    default:
        i4Ret = RMR_DRV_INV_GET_INFO;
        break;
    }

    return i4Ret;
}

static INT32 _MHLMWSet(
     DRV_COMP_ID_T  *pt_comp_id,
     DRV_SET_TYPE_T  e_set_type,
     const VOID     *pv_set_info,
     SIZE_T          z_set_info_len)
{
    INT32 i4Ret = RMR_OK;
    SKTDEV_MHL_ST_NFY_T *ptMWMHLOCNfyInfo;
    
    // Check component ID
    if (pt_comp_id == NULL || pt_comp_id->e_type != DRVT_HW_MHL)
    {
        return RMR_DRV_INV_SET_INFO;
    }

    switch (e_set_type)
    {
    case SKTDEV_SET_TYPE_MHL_ST_NFY:
             if ((pv_set_info == NULL) || z_set_info_len < sizeof(SKTDEV_MHL_ST_NFY_T))
             {
                 return RMR_DRV_INV_SET_INFO;
             }

             if(pt_comp_id->ui2_id >= MHL_HW_PORT_NUM)
             {
                 return RMR_DRV_NO_GET_INFO;
             }

             ptMWMHLOCNfyInfo = (SKTDEV_MHL_ST_NFY_T*) pv_set_info;
             
            _MHLMWNfyInfo[pt_comp_id->ui2_id].pv_tag = ptMWMHLOCNfyInfo->pv_tag;
            _MHLMWNfyInfo[pt_comp_id->ui2_id].pf_sktdev_mhl_st_nfy = ptMWMHLOCNfyInfo->pf_sktdev_mhl_st_nfy;

            //printf(" mhl mw set CB %d\n", pt_comp_id->ui2_id);

            i4Ret = RMR_OK;
            
        break;

    default:
        i4Ret = RMR_DRV_INV_SET_INFO;
        break;
    }

    return i4Ret;
}


INT32 MW_MHL_Init(void)
{
    INT32 i4Ret;


    UINT32 u4Flags;
    DRV_COMP_REG_T tMhlCompReg;
    DRV_COMP_FCT_TBL_T tMhlFctTbl;

    /* Setup component ID */
    tMhlCompReg.e_type = DRVT_HW_MHL;
    tMhlCompReg.e_id_type = ID_TYPE_IND;
    tMhlCompReg.u.t_ind.ui2_id = 0;
    tMhlCompReg.u.t_ind.pv_tag = NULL;
    tMhlCompReg.u.t_ind.ui1_port = 0;

    /* Registration Flags */
    u4Flags = 0;

    /* Setup component function table */
    tMhlFctTbl.pf_rm_connect = NULL;
    tMhlFctTbl.pf_rm_disconnect = NULL;
    tMhlFctTbl.pf_rm_get = _MHLMWGet;
    tMhlFctTbl.pf_rm_set = _MHLMWSet;

    /* Register component with the Resource Manager database */
    i4Ret = x_rm_reg_comp(&tMhlCompReg,
                          1,
                          1,
                          NULL,
                          u4Flags,
                          &tMhlFctTbl,
                          NULL,
                          0);

    if (i4Ret != RMR_OK)
    {
        LOG(1, "MW_MHL_Init(): x_rm_reg_comp failed [%d]\n", (INT32)i4Ret);
    }
#ifndef LINUX_TURNKEY_SOLUTION
    MHL_SetOCPCallback(_MHLMWNfyOcp);
#endif

    return i4Ret;
}

#endif

