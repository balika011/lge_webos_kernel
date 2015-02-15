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

#ifndef _TUNER_MW_C_
#define _TUNER_MW_C_

//#define CC_MW_IF_over_MTAL 

/*-----------------------------------------------------------------------------
                    include files
-----------------------------------------------------------------------------*/

#include "drvapi_tuner.h"
#include "x_rm_dev_types.h"
#include "mtfe_tunerhal.h"
#include "PD_Def.h"

/*-----------------------------------------------------------------------------
                    variables definitions
 ----------------------------------------------------------------------------*/
static x_rm_nfy_fct g_x_rm_nfy_fct;

/*-----------------------------------------------------------------------------
                    function prototypes
 ----------------------------------------------------------------------------*/
static INT32 _TunerConnect(DRV_COMP_ID_T* pt_comp_id,
                           DRV_CONN_TYPE_T  e_conn_type,
                           const VOID*      pv_conn_info,
                           SIZE_T           z_conn_info_len,
                           VOID*            pv_tag,
                           x_rm_nfy_fct     pf_nfy);
     
static INT32 _TunerDisConnect(DRV_COMP_ID_T* pt_comp_id,
                              DRV_DISC_TYPE_T  e_disc_type,
                              const VOID*      pv_disc_info,
                              SIZE_T           z_disc_info_len);
     
static INT32 _TunerGet(DRV_COMP_ID_T* pt_comp_id,
                       DRV_GET_TYPE_T  e_get_type,
                       VOID*           pv_get_info,
                       SIZE_T*         pz_get_info_len);
     
static INT32 _TunerSet(DRV_COMP_ID_T* pt_comp_id,
                       DRV_SET_TYPE_T  e_set_type,
                       const VOID*     pv_set_info,
                       SIZE_T          z_set_info_len);


EXTERN MT_RESULT_T MTFE_TUNERHAL_Connect(INT32 *pt_comp_id,
                              INT32  e_conn_type,
                              const VOID*      pv_conn_info,
                              INT32           z_conn_info_len,
                              VOID*            pv_tag,
                              INT32            *pf_nfy,
                              INT32*           peResult);
    
EXTERN MT_RESULT_T MTFE_TUNERHAL_DisConnect(INT32 *pt_comp_id,
                                 INT32            e_disc_type,
                                 const VOID*      pv_disc_info,
                                 INT32           z_disc_info_len,
                                 INT32*           peResult);
    
EXTERN MT_RESULT_T MTFE_TUNERHAL_Get(INT32 *pt_comp_id,
                          INT32           e_get_type,
                          VOID*           pv_get_info,
                          INT32*         pz_get_info_len,
                          INT32*          peResult);
    
EXTERN MT_RESULT_T MTFE_TUNERHAL_Set(INT32   *pt_comp_id,
                          INT32           e_set_type,
                          const VOID*     pv_set_info,
                          INT32          z_set_info_len,
                          INT32*          peResult);


void _TunerNotifyInitThread(UINT16 TunerID, UINT16 DrvType);
void _TunerNotifyTunerProc(UINT16 TunerID1, UINT16 DrvType1, UINT16 TunerID2, UINT16 DrvType2);

int _TunerNfyCallbackHandler(MTAL_FE_NFY_TUNER_CALLBACK_INFO_T* pArg);
/*-----------------------------------------------------------------------------
                    Structures
 ----------------------------------------------------------------------------*/
DRV_COMP_FCT_TBL_T t_tuner_fct_tbl =
{
    _TunerConnect,
    _TunerDisConnect,
    _TunerGet,
    _TunerSet
};

/*-----------------------------------------------------------------------------
                Global Function definition
----------------------------------------------------------------------------*/
/*
 * This function initializes tuner hardware, create the tuner
 * state machine thread, and registers to resource manager.
 *
 * To release the resources allocated by Tuner_Init (no matter success or failure),
 * please call Tuner_Kill.
 * weile remove register rm thread 2012.7.6
*/
INT32 Tuner_Init(void)
{
    TUNERHAL_MSG("TunerInit START !");
  
    MTFE_TUNERHAL_RegCb(MTFE_TUNERHAL_CB_FUNC_INI_THREAD, (UINT32)_TunerNotifyInitThread, 0);
    TUNERHAL_MSG("TunerMW RegCb: _TunerNotifyInitThread !");
    MTFE_TUNERHAL_RegCb(MTFE_TUNERHAL_CB_FUNC_TUNER_PROC, (UINT32)_TunerNotifyTunerProc, 0);
    TUNERHAL_MSG("TunerMW RegCb: _TunerNotifyTunerProc !");

    MTFE_TUNERHAL_NFY_RegCb((MTFE_NFY_PFN_CB)_TunerNfyCallbackHandler); //register tuner notify callback function.

    if (MTFE_TUNERHAL_Init() != OSR_OK)
    {
       TUNERHAL_MSG("TunerInit failure !");       
    }

    TUNERHAL_MSG("TunerInit END !");    
    return (RMR_OK);
}

/*
 * This function is user space register Tuner component callback.
 * @parameter: UINT16 TunerID,UINT16 DrvType
 * @return: void
 * weile remove rm register thread that cause thread dead lock 2012.7.6
*/
void _TunerNotifyInitThread(UINT16 TunerID, UINT16 DrvType)
{
    INT32	res;
    char ucBuf[256] = {0};
    DRV_COMP_REG_T t_tuner_comp_reg;
    
    if(MTFE_TUNERHAL_GetCompNameForMW(ucBuf, TunerID, DrvType) == MTR_OK)
    {        
        TUNERHAL_MSG("_TunerNotifyInitThread: tuner name:%s, TunerID: %d, DrvType: %d", ucBuf, TunerID, DrvType);

        // Setup component ID 
        t_tuner_comp_reg.e_type = DrvType;
        t_tuner_comp_reg.e_id_type = ID_TYPE_IND;
        t_tuner_comp_reg.u.t_ind.ui2_id = TunerID;
        t_tuner_comp_reg.u.t_ind.ui1_port = ALL_PORTS;
        t_tuner_comp_reg.u.t_ind.pv_tag = NULL;

        res = x_rm_reg_comp(&t_tuner_comp_reg, 1, 1,
                            ucBuf,
                            DRV_FLAG_ASYNC_CONN_OR_DISC,
                            &t_tuner_fct_tbl,
                            (const VOID *)NULL,
                            (SIZE_T) 0);

        TUNERHAL_MSG("x_rm_reg_comp:%d", res);
        
        if (res != RMR_OK)
        {
            printf("[%s:%d] Register Tuner component Fail!\n",__FUNCTION__,__LINE__);
            return;
        }
    }
}

/*
 * This function is user space register Tuner component excl list callback.
 * @parameter: UINT16 TunerID1,UINT16 DrvType1,UINT16 TunerID2,UINT16 DrvType2
 * @return: void
 * weile remove rm register thread that cause thread dead lock 2012.7.6
*/
void _TunerNotifyTunerProc(UINT16 TunerID1, UINT16 DrvType1, UINT16 TunerID2, UINT16 DrvType2)
{
    INT32	res;
    DRV_COMP_REG_T t_tuner_comp_reg1, t_tuner_comp_reg2;
    
    TUNERHAL_MSG("_TunerNotifyTunerProc: TunerID1: %d, DrvType1: %d, TunerID2: %d, DrvType2: %d", TunerID1, DrvType1, TunerID2, DrvType2);

    t_tuner_comp_reg1.e_type = DrvType1;
    t_tuner_comp_reg1.e_id_type = ID_TYPE_IND;
    t_tuner_comp_reg1.u.t_ind.ui2_id = TunerID1;               
    t_tuner_comp_reg1.u.t_ind.ui1_port = ALL_PORTS;
    t_tuner_comp_reg1.u.t_ind.pv_tag = NULL;

    t_tuner_comp_reg2.e_type = DrvType2;
    t_tuner_comp_reg2.e_id_type = ID_TYPE_IND;
    t_tuner_comp_reg2.u.t_ind.ui2_id = TunerID2;
    t_tuner_comp_reg2.u.t_ind.ui1_port = ALL_PORTS;
    t_tuner_comp_reg2.u.t_ind.pv_tag = NULL;

    res = x_rm_reg_comp_excl_list (&t_tuner_comp_reg1,
                                   &t_tuner_comp_reg2,
                                   1);

    if (res != RMR_OK)
    {
        printf("[%s:%d] Register Tuner component excl list Fail!\n",__FUNCTION__,__LINE__);
        return;
    }
}

static INT32 _TunerConnect(DRV_COMP_ID_T* pt_comp_id,
                           DRV_CONN_TYPE_T  e_conn_type,
                           const VOID*      pv_conn_info,
                           SIZE_T           z_conn_info_len,
                           VOID*            pv_tag,
                           x_rm_nfy_fct     pf_nfy)
{
    INT32 eResult;
    eResult = RMR_OK;
    g_x_rm_nfy_fct = pf_nfy;
    MTFE_TUNERHAL_Connect((INT32*)pt_comp_id, (INT32)e_conn_type, pv_conn_info, (INT32)z_conn_info_len, pv_tag, (INT32*)pf_nfy, &eResult);
    return eResult;
}

static INT32 _TunerDisConnect(DRV_COMP_ID_T* pt_comp_id,
                              DRV_DISC_TYPE_T  e_disc_type,
                              const VOID*      pv_disc_info,
                              SIZE_T           z_disc_info_len)
{
    INT32 eResult;
    eResult = RMR_OK;
    MTFE_TUNERHAL_DisConnect((INT32*)pt_comp_id, (INT32)e_disc_type, pv_disc_info, (INT32)z_disc_info_len, &eResult);
    return eResult;
}

/*
 * Function: _TunerGet UserSpace
 * Description: register to RM, Move into User Space
 * Param: DRV_COMP_ID_T* pt_comp_id,
 *         DRV_GET_TYPE_T  e_get_type,
 *         VOID*           pv_get_info,
 *         SIZE_T*         pz_get_info_len
 * Return INT32
 * @weile 2012.1.6
*/
static INT32 _TunerGet(DRV_COMP_ID_T* pt_comp_id,
                       DRV_GET_TYPE_T  e_get_type,
                       VOID*           pv_get_info,
                       SIZE_T*         pz_get_info_len)
{
    INT32 eResult;
    eResult = RMR_OK;
    MTFE_TUNERHAL_Get((INT32*)pt_comp_id, (INT32)e_get_type, pv_get_info, (INT32*)pz_get_info_len, &eResult);
    return eResult;
}
     
static INT32 _TunerSet(DRV_COMP_ID_T* pt_comp_id,
                       DRV_SET_TYPE_T  e_set_type,
                       const VOID*     pv_set_info,
                       SIZE_T          z_set_info_len)
{
    INT32 eResult;
    eResult = RMR_OK;
    MTFE_TUNERHAL_Set((INT32*)pt_comp_id, (INT32)e_set_type, pv_set_info, (INT32)z_set_info_len, &eResult);
    return eResult;
}

/*
 * Function: _TunerNfyCallbackHandler
 * Description: Tuner notify to RM paramters. 
 * Param: MTAL_FE_NFY_TUNER_CALLBACK_INFO_T* pArg
 * Return : int
 * @weile 2012.1.6
*/
int _TunerNfyCallbackHandler(MTAL_FE_NFY_TUNER_CALLBACK_INFO_T* pArg)
{
    MTAL_FE_NFY_TUNER_CALLBACK_INFO_T* prCbInfo = pArg;
    
    DRV_COMP_ID_T*  pt_comp_id;
    DRV_COND_T      e_nfy_cond;          
    VOID*           pv_tag;
    UINT32          ui4_data;

    pt_comp_id = (DRV_COMP_ID_T *)prCbInfo->pt_comp_id;
    e_nfy_cond = (DRV_COND_T)prCbInfo->e_nfy_cond;
    pv_tag     = (VOID *)prCbInfo->pv_tag;
    ui4_data   = (UINT32)prCbInfo->ui4_data;
    
    if (g_x_rm_nfy_fct)
    {
        g_x_rm_nfy_fct(pt_comp_id, e_nfy_cond, pv_tag, ui4_data);
    }
    return 0;
}

#endif	/* _X_TUNER_MW_C_ */

