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
 * $RCSfile: io_mtfe_tunerhal.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file io_mtfe_tunerhal.c
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "mtfe_tunerhal.h"
#include "io_mtfe_pd_def.h"

#include "tuner_if.h"
#include "cb_low.h"  // For callback function

#include <linux/mtal_ioctl.h>

#include "PD_Def.h"
#include "drvapi_tuner.h"
#include "x_rm_dev_types.h"
#include "io_mtfe_tunerhal.h"
#include "TDMgr.h"
#include "tuner_interface_if.h"

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define USR_SPACE_ACCESS_VALIDATE_1ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(int)))                            \
    {                                                       \
        printk("mtal_ioctl_tunerhal argument error\n");     \
        return -1;                                          \
    }

#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(int)))                        \
    {                                                       \
        printk("mtal_ioctl_tunerhal argument error\n");      \
        return -1;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_tunerhal argument error\n");     \
        return -1;                                          \
    }

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_tunerhal argument error\n");      \
        return -1;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_tunerhal argument error\n");     \
        return -1;                                          \
    }

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_tunerhal argument error\n");      \
        return -1;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_4ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_4ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_auddec argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_4ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_4ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_auddec argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_5ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_5ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_auddec argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_5ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_5ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_auddec argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_6ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_6ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_auddec argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_6ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_6ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_auddec argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_7ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_7ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_auddec argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_7ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_7ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_auddec argument error\n");      \
        return MTR_ERR_INV;                                          \
    }
    
#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        printk("mtal_ioctl_tunerhal argument error\n");     \
        return MTR_ERR_INV;                                          \
    }

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_tunerhal argument error\n");      \
        return MTR_ERR_INV;                                          \
    }
#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_from_user(&ArgKernel,(void __user *)ArgUsr,    \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_tunerhal argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_ARG_K(arg,size)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   size))              \
    {                                                       \
        printk("mtal_ioctl_tunerhal valid argument error\n");     \
        return MTR_ERR_INV;                                          \
    }   

#define COPY_FROM_USER_ARG_K(ArgUsr, ArgKernel,size)              \
    if (copy_from_user(ArgKernel, (void __user *)ArgUsr,    \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_tunerhal argument copy error\n");      \
        return MTR_ERR_INV;                                          \
    } 

#define COPY_TO_USER_ARG_K(ArgUsr, ArgKernel, size)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_tunerhal argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define MTFE_TUNERHAL_RETURN(VALUE)         \
    {                                       \
        eResult = VALUE;                    \
        USR_SPACE_ACCESS_VALIDATE_ARG(peResult,INT32); \
        COPY_TO_USER_ARG(peResult,eResult,INT32);  \
        return eRet; \
    }
//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
/*
 * all variable used in this file should put into pMTPdCtx 
 * exclusive of Exclusive_DigiAnaSema and u1ExclSemaInit
*/
static HANDLE_T    Exclusive_DigiAnaSema=0;
static UINT8       u1ExclSemaInit=0;

//-----------------------------------------------------------------------------
// Extern variables
//-----------------------------------------------------------------------------
extern TUNERHAL_NTY fptIniThreadNfy;
extern TUNERHAL_NTY1 fptTunerProcNfy;

extern BOOL bDetachMW;
extern BOOL TDD_Ready;
extern SEMA_T t_init_sync;

//-----------------------------------------------------------------------------
// Extern function
//-----------------------------------------------------------------------------
extern S32 GetTunerCtx(DRV_COMP_ID_T* pt_comp_id, TUNER_CTX_T** pptTunerCtx,
                       STATE_INFO_T** pptStateInfo);
extern TUNER_MODULATION_T TunerMod(S32 qam_size);

#ifdef CC_SUPPORT_STR
#ifdef CC_APP_IF_MTAL
SEMA_T             t_Resume = (SEMA_T)NULL;
static BOOL  	   u1MtalResumeEnd = TRUE;
extern MT_RESULT_T _MTFE_DVBT_Suspend(VOID);
extern MT_RESULT_T _MTFE_DVBT_Resume(VOID);
extern MT_RESULT_T _MTFE_DVBC_Suspend(VOID);
extern MT_RESULT_T _MTFE_DVBC_Resume(VOID);
extern MT_RESULT_T _MTFE_PAL_Suspend(VOID);
extern MT_RESULT_T _MTFE_PAL_Resume(VOID);
#endif
#endif
extern ITUNER_CTX_T *ITunerGetCtx_Multi(UINT8 u1TunerCellID);

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static function
//-----------------------------------------------------------------------------
static void _CB_PutEventTunerNfy(UINT32 pt_comp_id,
                                 UINT32 e_nfy_cond,
                                 UINT32 pv_tag,
                                 UINT32 ui4_data)
{
    MTAL_FE_NFY_TUNER_CALLBACK_INFO_T 	rCbInfo;

    rCbInfo.pt_comp_id = pt_comp_id;
    rCbInfo.e_nfy_cond = e_nfy_cond;
    rCbInfo.pv_tag = pv_tag;
    rCbInfo.ui4_data = ui4_data;
    
    _CB_PutEvent(CB_MTAL_FE_NFY_TUNER_TRIGGER, sizeof(MTAL_FE_NFY_TUNER_CALLBACK_INFO_T), &rCbInfo);
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTFE_TUNERHAL_Init(VOID) 
{
    INT32 i4Ret;

    i4Ret = _Tuner_Init();

    if (i4Ret)
        return MTR_NOT_OK;
    
    return (MTR_OK);
}

VOID mtfe_pf_nfy (DRV_COMP_ID_T*  pt_comp_id,
                  DRV_COND_T	     e_nfy_cond,
                  VOID*		         pv_tag,
                  UINT32 		       ui4_data)
{ 
    _CB_PutEventTunerNfy((UINT32)pt_comp_id, (UINT32)e_nfy_cond, (UINT32)pv_tag, (UINT32)ui4_data);
}

/** Connect TUNERHAL module.
*@retval MT_OK              Success.
*@retval MT_NOT_OK          Fail.
*/
static MT_RESULT_T _MTFE_TUNERHAL_Connect(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
     
    MTAL_IOCTL_7ARG_T rArg;

    DRV_COMP_ID_T *pt_comp_id;
    DRV_CONN_TYPE_T e_conn_type;
    const VOID *pv_conn_info;
    SIZE_T z_conn_info_len;
    VOID *pv_tag;
    x_rm_nfy_fct pf_nfy;
    INT32 *peResult;
    INT32 eResult;
    
    USR_SPACE_ACCESS_VALIDATE_7ARG(arg);
    COPY_FROM_USER_7ARG(arg, rArg);
    
    pt_comp_id = (DRV_COMP_ID_T*)rArg.ai4Arg[0];
    e_conn_type = (DRV_CONN_TYPE_T)rArg.ai4Arg[1];
    pv_conn_info = (const VOID*)rArg.ai4Arg[2];
    z_conn_info_len = (SIZE_T)rArg.ai4Arg[3];
    pv_tag = (VOID*)rArg.ai4Arg[4];
    pf_nfy = (x_rm_nfy_fct)rArg.ai4Arg[5];
    peResult = (INT32*)rArg.ai4Arg[6];
    
    pf_nfy = (x_rm_nfy_fct)mtfe_pf_nfy;
	
    eResult = TunerConnect(pt_comp_id, e_conn_type, pv_conn_info, z_conn_info_len, pv_tag, pf_nfy);
    
    //pt_comp_id,pv_conn_info leave to _MTFE_TUNERHAL_Connect 
    USR_SPACE_ACCESS_VALIDATE_ARG(peResult,INT32);
    COPY_TO_USER_ARG(peResult,eResult,INT32);
    
    return eRet;
}

/** Disconnet TUNERHAL module.
*@retval MT_OK              Success.
*@retval MT_NOT_OK          Fail.
*/

//static MT_RESULT_T _MTFE_TUNERHAL_DisConnect(VOID)
MT_RESULT_T _MTFE_TUNERHAL_DisConnect(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
       
    MTAL_IOCTL_5ARG_T rArg;
    DRV_COMP_ID_T *pt_comp_id;
    DRV_DISC_TYPE_T e_disc_type;
    const VOID *pv_disc_info;
    SIZE_T z_disc_info_len;
    INT32 *peResult;
    INT32 eResult;

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg); 
    
    pt_comp_id = (DRV_COMP_ID_T*)rArg.ai4Arg[0];
    e_disc_type = (DRV_DISC_TYPE_T)rArg.ai4Arg[1];
    pv_disc_info = (const VOID*)rArg.ai4Arg[2];
    z_disc_info_len = (SIZE_T)rArg.ai4Arg[3];
    peResult = (INT32*)rArg.ai4Arg[4];
    
    eResult = TunerDisconnect(pt_comp_id, e_disc_type, pv_disc_info, z_disc_info_len);

    //pt_comp_id,pv_conn_info leave to _MTFE_TUNERHAL_Connect 
    USR_SPACE_ACCESS_VALIDATE_ARG(peResult,INT32);
    COPY_TO_USER_ARG(peResult,eResult,INT32);
    
    return eRet;
}

/** Get lock status.
*@param	 prStatus           Report demod status.
*@retval MT_OK              Success.
*@retval MT_NOT_OK          Fail.
*/
static MT_RESULT_T _MTFE_TUNERHAL_Get(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    
    MTAL_IOCTL_5ARG_T rArg;
    DRV_COMP_ID_T *pt_comp_id, *usr_pt_comp_id, t_comp_id;
    DRV_GET_TYPE_T  e_get_type;
    VOID *pv_get_info;
    SIZE_T *pz_get_info_len, *pz_size, z_size; 
    INT32 *peResult;
    INT32 eResult;
    TUNER_CTX_T *ptTunerCtx;
    STATE_INFO_T *ptStateInfo;
    
    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

    usr_pt_comp_id = (DRV_COMP_ID_T*)rArg.ai4Arg[0];
    e_get_type = (DRV_GET_TYPE_T)rArg.ai4Arg[1];
    pv_get_info = (VOID*)rArg.ai4Arg[2];
    pz_get_info_len = (SIZE_T*)rArg.ai4Arg[3];
    peResult = (INT32*)rArg.ai4Arg[4];

    pt_comp_id = &t_comp_id;
    pz_size = &z_size;
        
    USR_SPACE_ACCESS_VALIDATE_ARG(usr_pt_comp_id, DRV_COMP_ID_T);
    COPY_FROM_USER_ARG(usr_pt_comp_id, t_comp_id, DRV_COMP_ID_T);

    USR_SPACE_ACCESS_VALIDATE_ARG(pz_get_info_len, SIZE_T);
    COPY_FROM_USER_ARG(pz_get_info_len, z_size, SIZE_T);
        
    MTFE_TUNERHAL_PRINT("[MWCmd]TunerGet()\n");
    if (bDetachMW)
    {
        MTFE_TUNERHAL_PRINT("TunerDriver Detach MW\n");
        MTFE_TUNERHAL_RETURN(RMR_OK);
    }

    // Service will be provided ONLY when tuner is opened successfully.
    if (!TDD_Ready)
    {
        mcSEMA_LOCK(t_init_sync);
        mcSEMA_UNLOCK(t_init_sync);
    }

    // Get corresponding TunerCtx and StateInfo
    if (GetTunerCtx(pt_comp_id, &ptTunerCtx, &ptStateInfo) != DRVAPI_TUNER_OK)
    {
        MTFE_TUNERHAL_RETURN(RMR_DRV_GET_FAILED);
    }
    
    if (ptStateInfo == NULL)
    {
        MTFE_TUNERHAL_RETURN(RMR_DRV_GET_FAILED);
    }

    /* verify the request */
    if ((pt_comp_id->e_type != DRVT_TUNER_CAB_DIG) &&
        (pt_comp_id->e_type != DRVT_TUNER_CAB_DIG_OOB_RX) &&
        (pt_comp_id->e_type != DRVT_TUNER_TER_DIG) &&
        (pt_comp_id->e_type != DRVT_TUNER_TER_ANA) &&
        (pt_comp_id->e_type != DRVT_TUNER_TER_ANA_SCART_OUT) &&
        (pt_comp_id->e_type != DRVT_TUNER_CAB_ANA_SCART_OUT) &&
        (pt_comp_id->e_type != DRVT_TUNER_CAB_ANA) &&
        (pt_comp_id->e_type != DRVT_TUNER_SAT_DIG)&&
        (pt_comp_id->e_type != DRVT_TUNER_SAT_ANA)&&
        (pt_comp_id->e_type != DRVT_TUNER_SAT_ANA_SCART_OUT)
        )
    {
        MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_INV_GET_INFO!\n");
        MTFE_TUNERHAL_RETURN(RMR_DRV_INV_GET_INFO);
    }
    
    /* supported types */
    switch (e_get_type)
    {
        case TUNER_GET_TYPE_DIG_INSTANCE_NUM:
        {
            if (*pz_size < sizeof(TUNER_DIG_INSTANCE_NUM_T)){ /* not enough space */
                *pz_size = sizeof(TUNER_DIG_INSTANCE_NUM_T);
                USR_SPACE_ACCESS_VALIDATE_ARG(pz_get_info_len, SIZE_T);
                COPY_TO_USER_ARG(pz_get_info_len, z_size, SIZE_T);
                
                MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n");
                MTFE_TUNERHAL_RETURN(RMR_DRV_NOT_ENOUGH_SPACE);
            }else{
                TUNER_DIG_INSTANCE_NUM_T argKernel;
                TUNER_DIG_INSTANCE_NUM_T *pt_tuner_info = &argKernel;
                
                ITUNER_CTX_T *pTunerCtx_main = NULL;
                ITUNER_CTX_T *pTunerCtx_sub = NULL;
                
                USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, TUNER_DIG_INSTANCE_NUM_T);
                COPY_FROM_USER_ARG(pv_get_info, argKernel, TUNER_DIG_INSTANCE_NUM_T);
                pTunerCtx_main = ITunerGetCtx_Multi(0);
                pTunerCtx_sub = ITunerGetCtx_Multi(1);
                if((pTunerCtx_main != NULL)&&(pTunerCtx_sub != NULL)){
                    if((pTunerCtx_main->u1TunerCellID == 0)&&(pTunerCtx_sub->u1TunerCellID == 1)){ //Dual tuner
                        pt_tuner_info->ui2_analog_num = 1;
                        pt_tuner_info->ui2_antenna_num = 2;
                        pt_tuner_info->ui2_cable_num = 2;
                        pt_tuner_info->ui2_satellite_num = 2;
                    }else if(pTunerCtx_main->u1TunerCellID == 0){ //Single tuner
                        pt_tuner_info->ui2_analog_num = 1;
                        pt_tuner_info->ui2_antenna_num = 1;
                        pt_tuner_info->ui2_cable_num = 1;
                        pt_tuner_info->ui2_satellite_num = 1;
                    }else{ //Dummy
                        pt_tuner_info->ui2_analog_num = 0;
                        pt_tuner_info->ui2_antenna_num = 0;
                        pt_tuner_info->ui2_cable_num = 0;
                        pt_tuner_info->ui2_satellite_num = 0;
                    }
                    MTFE_TUNERHAL_PRINT("ui2_antenna_num: %d\n", pt_tuner_info->ui2_antenna_num);
                    USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, TUNER_DIG_INSTANCE_NUM_T);
                    COPY_TO_USER_ARG(pv_get_info, argKernel, TUNER_DIG_INSTANCE_NUM_T);
                }else{
                    MTFE_TUNERHAL_RETURN(RMR_DRV_INV_GET_INFO);
                }
            }
            break;
        }
        case TUNER_GET_TYPE_TER_ANA:
        {
            if (ptStateInfo->e_tuner_state == TUNER_STATE_CONNECTED)
            { /* if connected */                
                if (*pz_size < sizeof(TUNER_TER_ANA_TUNE_INFO_T))
                { /* not enough space */
                    *pz_size = sizeof(TUNER_TER_ANA_TUNE_INFO_T);
                    USR_SPACE_ACCESS_VALIDATE_ARG(pz_get_info_len, SIZE_T);
                    COPY_TO_USER_ARG(pz_get_info_len, z_size, SIZE_T);
                    
                    MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n");
                    MTFE_TUNERHAL_RETURN(RMR_DRV_NOT_ENOUGH_SPACE);
                }
                else
                { /* get info */
                    TUNER_TER_ANA_TUNE_INFO_T argKernel;
                    TUNER_TER_ANA_TUNE_INFO_T *pt_tuner_info = &argKernel;
                                      
                    USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, TUNER_TER_ANA_TUNE_INFO_T);
                    COPY_FROM_USER_ARG(pv_get_info, argKernel, TUNER_TER_ANA_TUNE_INFO_T);

                    TunerGetSignal(ptTunerCtx->TunerID, TUNER_CONN_TYPE_TER_ANA,
                                    (SIGNAL*)pt_tuner_info);
                        
                    USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, TUNER_TER_ANA_TUNE_INFO_T);
                    COPY_TO_USER_ARG(pv_get_info, argKernel, TUNER_TER_ANA_TUNE_INFO_T);
                }
            }
            else
            { /* if not connected */
                MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_NO_GET_INFO!\n");
                MTFE_TUNERHAL_RETURN(RMR_DRV_NO_GET_INFO);
            }
            break;
        }
        case TUNER_GET_TYPE_CAB_ANA:
        {
            if (ptStateInfo->e_tuner_state == TUNER_STATE_CONNECTED)
            { /* if connected */
                if (*pz_size < sizeof(TUNER_CAB_ANA_TUNE_INFO_T))
                { /* not enough space */
                    *pz_size = sizeof(TUNER_CAB_ANA_TUNE_INFO_T);
                    USR_SPACE_ACCESS_VALIDATE_ARG(pz_get_info_len, SIZE_T);
                    COPY_TO_USER_ARG(pz_get_info_len, z_size, SIZE_T);
                    
                    MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n");
                    MTFE_TUNERHAL_RETURN(RMR_DRV_NOT_ENOUGH_SPACE);
                }
                else
                { /* get info */
                    TUNER_CAB_ANA_TUNE_INFO_T argKernel;
                    TUNER_CAB_ANA_TUNE_INFO_T *pt_tuner_info = &argKernel;
                    USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, TUNER_CAB_ANA_TUNE_INFO_T);
                    COPY_FROM_USER_ARG(pv_get_info, argKernel, TUNER_CAB_ANA_TUNE_INFO_T);
                    
                    TunerGetSignal(ptTunerCtx->TunerID, TUNER_CONN_TYPE_CAB_ANA, (SIGNAL*)pt_tuner_info);

                    USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, TUNER_CAB_ANA_TUNE_INFO_T);
                    COPY_TO_USER_ARG(pv_get_info, argKernel, TUNER_CAB_ANA_TUNE_INFO_T);
                }
            }
            else
            { /* if not connected */
                MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_NO_GET_INFO!\n");
                MTFE_TUNERHAL_RETURN(RMR_DRV_NO_GET_INFO);
            }
            break;
        }
        case TUNER_GET_TYPE_SAT_ANA:
        {
            if (ptStateInfo->e_tuner_state == TUNER_STATE_CONNECTED)
            { /* if connected */
                if (*pz_size < sizeof(TUNER_SAT_ANA_TUNE_INFO_T))
                { /* not enough space */
                    *pz_size = sizeof(TUNER_SAT_ANA_TUNE_INFO_T);
                    USR_SPACE_ACCESS_VALIDATE_ARG(pz_get_info_len, SIZE_T);
                    COPY_TO_USER_ARG(pz_get_info_len, z_size, SIZE_T);
                    
                    MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n");
                    MTFE_TUNERHAL_RETURN(RMR_DRV_NOT_ENOUGH_SPACE);
                }
                else
                { /* get info */
                    TUNER_SAT_ANA_TUNE_INFO_T argKernel;
                    TUNER_SAT_ANA_TUNE_INFO_T *pt_tuner_info = &argKernel;
                    USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, TUNER_SAT_ANA_TUNE_INFO_T);
                    COPY_FROM_USER_ARG(pv_get_info, argKernel, TUNER_SAT_ANA_TUNE_INFO_T);
                    
                    TunerGetSignal(ptTunerCtx->TunerID, TUNER_CONN_TYPE_SAT_ANA, (SIGNAL*)pt_tuner_info);

                    USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, TUNER_SAT_ANA_TUNE_INFO_T);
                    COPY_TO_USER_ARG(pv_get_info, argKernel, TUNER_SAT_ANA_TUNE_INFO_T);
                }
            }
            else
            { /* if not connected */
                MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_NO_GET_INFO!\n");
                MTFE_TUNERHAL_RETURN(RMR_DRV_NO_GET_INFO);
            }
            break;
        }
        case TUNER_GET_TYPE_TER_DIG:
        {
            if (ptStateInfo->e_tuner_state == TUNER_STATE_CONNECTED)
            { /* if connected */
                if (*pz_size < sizeof(TUNER_TER_DIG_TUNE_INFO_T))
                { /* not enough space */
                    *pz_size = sizeof(TUNER_TER_DIG_TUNE_INFO_T);
                    USR_SPACE_ACCESS_VALIDATE_ARG(pz_get_info_len, SIZE_T);
                    COPY_TO_USER_ARG(pz_get_info_len, z_size, SIZE_T);
                    
                    MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n");
                    MTFE_TUNERHAL_RETURN(RMR_DRV_NOT_ENOUGH_SPACE);
                }
                else
                { /* get info */
                    SIGNAL  t_signal;
                    TUNER_TER_DIG_TUNE_INFO_T argKernel;
                    TUNER_TER_DIG_TUNE_INFO_T *pt_tuner_info = &argKernel;
                    
                    USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, TUNER_TER_DIG_TUNE_INFO_T);
                    COPY_FROM_USER_ARG(pv_get_info, argKernel, TUNER_TER_DIG_TUNE_INFO_T);
                    
                    TunerGetSignal(ptTunerCtx->TunerID, TUNER_CONN_TYPE_TER_DIG, &t_signal);
                    pt_tuner_info->ui4_freq     = t_signal.Frequency * 1000;    /* Hz */
                    pt_tuner_info->e_mod        = TunerMod(t_signal.QAMSize);   /* TUNER_MODULATION_T */
                    pt_tuner_info->e_dvb_t_t2_mode  = t_signal.u1DVBT_T2_indicator;  //add mode get!!
                    pt_tuner_info->e_dvb_t_t2_FecRate= (TUNER_DVB_T_T2_CODRRATE_T) t_signal.u1FecRate;
                    if (pt_tuner_info->e_dvb_t_t2_mode == TUNER_DVB_T2)
                    {
                        pt_tuner_info->ui1_plp_id = t_signal.ui1_plp_id;   //update plp id.
					    pt_tuner_info->ui2_t2_sys_id=t_signal.ui2_t2_sys_id;
					    pt_tuner_info->e_dvb_t2_fft_mode =(TUNER_T2_FFT_TYPE_T)t_signal.u1DVBT_T2_FFT_mode; 
					    pt_tuner_info->e_dvb_t2_guard_int =(TUNER_T2_GUARD_INTERVAL_T)t_signal.u1GI; 
					    pt_tuner_info->e_dvb_t2_Pilot_Pattern=(TUNER_T2_PILOT_PATTERN_TYPE_T)t_signal.u1Piliot_Pattern;
					    MTFE_TUNERHAL_PRINT("[MWInfo] pt_tuner_info->ui2_t2_sys_id = %d\n", pt_tuner_info->ui2_t2_sys_id);
	                    MTFE_TUNERHAL_PRINT("[MWInfo] pt_tuner_info->e_dvb_t2_fft_mode = %d\n", pt_tuner_info->e_dvb_t2_fft_mode);
	                    MTFE_TUNERHAL_PRINT("[MWInfo] pt_tuner_info->e_dvb_t2_guard_int = %d\n", pt_tuner_info->e_dvb_t2_guard_int);
	                    MTFE_TUNERHAL_PRINT("[MWInfo] pt_tuner_info->e_dvb_t2_Pilot_Pattern = %d\n", pt_tuner_info->e_dvb_t2_Pilot_Pattern);
                    }
					else
					{
						pt_tuner_info->e_dvb_t_fft_mode =(TUNER_T_FFT_TYPE_T)t_signal.u1DVBT_T2_FFT_mode; 
						pt_tuner_info->e_dvb_t_guard_int =(TUNER_T_GUARD_INTERVAL_T)t_signal.u1GI; 
						MTFE_TUNERHAL_PRINT("[MWInfo] pt_tuner_info->e_dvb_t_fft_mode = %d\n", pt_tuner_info->e_dvb_t_fft_mode);
						MTFE_TUNERHAL_PRINT("[MWInfo] pt_tuner_info->e_dvb_t_guard_int = %d\n",pt_tuner_info->e_dvb_t_guard_int);
					}
                    MTFE_TUNERHAL_PRINT("[MWInfo] pt_tuner_info->ui4_freq = %d\n", pt_tuner_info->ui4_freq);
                    MTFE_TUNERHAL_PRINT("[MWInfo] pt_tuner_info->e_mod = %d\n", pt_tuner_info->e_mod);
                    MTFE_TUNERHAL_PRINT("[MWInfo] pt_tuner_info->e_dvb_t_t2_mode = %d\n", pt_tuner_info->e_dvb_t_t2_mode);
                    MTFE_TUNERHAL_PRINT("[MWInfo] pt_tuner_info->ui1_plp_id = %d\n", pt_tuner_info->ui1_plp_id);
					MTFE_TUNERHAL_PRINT("[MWInfo] pt_tuner_info->e_dvb_t_t2_FecRate = %d\n", pt_tuner_info->e_dvb_t_t2_FecRate);                 
                      
#ifndef DTMB_MANUAL_DEMOD_DETECT
                    pt_tuner_info->e_bandwidth  = (TUNER_BANDWIDTH_T) t_signal.e_bandwidth;
                    pt_tuner_info->e_hierarchy_priority = (TUNER_HIERARCHY_PRIORITY_T) t_signal.e_hierarchy_priority;
#else
                    pt_tuner_info->e_guard_int = (TUNER_GUARD_INTERVAL_T) t_signal.u1GI;
                    pt_tuner_info->e_timer_int = (TUNER_TIMER_INTERLEAVING_T) t_signal.u1TimDeInterl;
                    pt_tuner_info->e_FecRate = (TUNER_CODRRATE_T) t_signal.u1FecRate;
                    pt_tuner_info->e_CarrierMode = (TUNER_CARRIER_MODE_T) t_signal.u1CarrierMode;
                    pt_tuner_info->e_PnNumber = (TUNER_PN_SEQ_T) t_signal.u1PnNumber;
#endif               
                    MTFE_TUNERHAL_PRINT("[MWInfo] pt_tuner_info->e_hierarchy_priority = %d\n", pt_tuner_info->e_hierarchy_priority);

                    USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, TUNER_TER_DIG_TUNE_INFO_T);
                    COPY_TO_USER_ARG(pv_get_info, argKernel, TUNER_TER_DIG_TUNE_INFO_T);
                }
            }
            else
            { /* if not connected */
                MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_NO_GET_INFO!\n");
                MTFE_TUNERHAL_RETURN(RMR_DRV_NO_GET_INFO);
            }
            break;
        }
        case TUNER_GET_TYPE_CAB_DIG:
        {
            if (ptStateInfo->e_tuner_state == TUNER_STATE_CONNECTED)
            { /* if connected */
                if (*pz_size < sizeof(TUNER_CAB_DIG_TUNE_INFO_T))
                { /* not enough space */
                    *pz_size = sizeof(TUNER_CAB_DIG_TUNE_INFO_T);
                    USR_SPACE_ACCESS_VALIDATE_ARG(pz_get_info_len, SIZE_T);
                    COPY_TO_USER_ARG(pz_get_info_len, z_size, SIZE_T);
                    
                    MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n");
                    MTFE_TUNERHAL_RETURN(RMR_DRV_NOT_ENOUGH_SPACE);
                }
                else
                { /* get info */
                    SIGNAL  t_signal;
                    TUNER_CAB_DIG_TUNE_INFO_T argKernel;
                    TUNER_CAB_DIG_TUNE_INFO_T *pt_tuner_info = &argKernel;
                    
                    USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, TUNER_CAB_DIG_TUNE_INFO_T);
                    COPY_FROM_USER_ARG(pv_get_info, argKernel, TUNER_CAB_DIG_TUNE_INFO_T);
                    
                    TunerGetSignal(ptTunerCtx->TunerID, TUNER_CONN_TYPE_CAB_DIG, &t_signal);
                    pt_tuner_info->ui4_freq     = t_signal.Frequency * 1000;    /* Hz */
                    pt_tuner_info->ui4_sym_rate = t_signal.SymbolRate * 1000;   /* sps */
                    pt_tuner_info->e_mod        = TunerMod(t_signal.QAMSize);   /* TUNER_MODULATION_T */
					pt_tuner_info->e_dvb_c_c2_mode	= t_signal.u1DVBC_C2_indicator;/*mode get*/
					
                    USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, TUNER_CAB_DIG_TUNE_INFO_T);
                    COPY_TO_USER_ARG(pv_get_info, argKernel, TUNER_CAB_DIG_TUNE_INFO_T);
                }
            }
            else
            { /* if not connected */
                MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_NO_GET_INFO!\n");
                MTFE_TUNERHAL_RETURN(RMR_DRV_NO_GET_INFO);
            }
            break;
        }
        case TUNER_GET_TYPE_SAT_DIG:
        {
            if (ptStateInfo->e_tuner_state == TUNER_STATE_CONNECTED)
            { /* if connected */
                if (*pz_size < sizeof(TUNER_SAT_DIG_TUNE_INFO_T))
                { /* not enough space */
                    *pz_size = sizeof(TUNER_SAT_DIG_TUNE_INFO_T);
                    USR_SPACE_ACCESS_VALIDATE_ARG(pz_get_info_len, SIZE_T);
                    COPY_TO_USER_ARG(pz_get_info_len, z_size, SIZE_T);
                    
                    MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n");
                    MTFE_TUNERHAL_RETURN(RMR_DRV_NOT_ENOUGH_SPACE);
                }
                else
                {
#if 0
                { /* get info */
                    SIGNAL  t_signal;
                    TUNER_SAT_DIG_TUNE_INFO_T *pt_tuner_info = (TUNER_SAT_DIG_TUNE_INFO_T *) pv_get_info;

                    TunerGetSignal(ptTunerCtx->TunerID, TUNER_CONN_TYPE_SAT_DIG, &t_signal);
                    pt_tuner_info->ui4_freq     = t_signal.Frequency;    /* Hz */
                    pt_tuner_info->ui4_sym_rate = t_signal.SymbolRate;   /* sps */
                    pt_tuner_info->e_mod        = TunerMod(t_signal.QAMSize);   /* TUNER_MODULATION_T */
                }
#else
                    /*
                     * The maximun pv_get_info buffer structure is TUNER_SAT_DIG_TUNE_INFO_T
                    */
                    TUNER_SAT_DIG_TUNE_INFO_T argKernel;
                    TUNER_SAT_DIG_TUNE_INFO_T *pt_tuner_info = &argKernel;

                    USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, TUNER_SAT_DIG_TUNE_INFO_T);
                    COPY_FROM_USER_ARG(pv_get_info, argKernel, TUNER_SAT_DIG_TUNE_INFO_T);
                    
                    eResult = (TunerGetAttribute2(ptTunerCtx->TunerID, e_get_type, pt_tuner_info, pz_size, TUNER_CONN_TYPE_SAT_DIG));

                    USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, TUNER_SAT_DIG_TUNE_INFO_T);
                    COPY_TO_USER_ARG(pv_get_info, argKernel, TUNER_SAT_DIG_TUNE_INFO_T); 

                    USR_SPACE_ACCESS_VALIDATE_ARG(pz_get_info_len, SIZE_T);
                    COPY_TO_USER_ARG(pz_get_info_len, z_size, SIZE_T);

                    USR_SPACE_ACCESS_VALIDATE_ARG(peResult,INT32);
                    COPY_TO_USER_ARG(peResult,eResult,INT32);
                    return MTR_OK;
#endif
                }
            }
            else
            { /* if not connected */
                MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_NO_GET_INFO!\n");
                MTFE_TUNERHAL_RETURN(RMR_DRV_NO_GET_INFO);
            }
            break;
        }
        case TUNER_GET_TYPE_CAB_DIG_OOB_RX:
        {
            if (ptStateInfo->e_tuner_state == TUNER_STATE_CONNECTED)
            { /* if connected */
                if (*pz_size < sizeof(TUNER_CAB_DIG_OOB_RX_TUNE_INFO_T))
                { /* not enough space */
                    *pz_size = sizeof(TUNER_CAB_DIG_OOB_RX_TUNE_INFO_T);
                    USR_SPACE_ACCESS_VALIDATE_ARG(pz_get_info_len, SIZE_T);
                    COPY_TO_USER_ARG(pz_get_info_len, z_size, SIZE_T);
                    
                    MTFE_TUNERHAL_PRINT("TunerGetOob: RMR_DRV_NOT_ENOUGH_SPACE!\n");
                    MTFE_TUNERHAL_RETURN(RMR_DRV_NOT_ENOUGH_SPACE);
                }
                else
                { /* get info */
                    SIGNAL  t_signal;
                    TUNER_CAB_DIG_OOB_RX_TUNE_INFO_T argKernel;
                    TUNER_CAB_DIG_OOB_RX_TUNE_INFO_T *pt_tuner_info = &argKernel;
                    
                    USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, TUNER_CAB_DIG_OOB_RX_TUNE_INFO_T);
                    COPY_FROM_USER_ARG(pv_get_info, argKernel, TUNER_CAB_DIG_OOB_RX_TUNE_INFO_T);
                    
                    TunerGetSignal(ptTunerCtx->TunerID, TUNER_CONN_TYPE_CAB_DIG_OOB_RX, &t_signal);
                    pt_tuner_info->ui4_freq      = t_signal.Frequency * 1000;    /* Hz */
                    pt_tuner_info->ui4_data_rate = t_signal.SymbolRate * 1000;
                    pt_tuner_info->e_mod         = TunerMod(t_signal.QAMSize);   /* TUNER_MODULATION_T */

                    USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, TUNER_CAB_DIG_OOB_RX_TUNE_INFO_T);
                    COPY_TO_USER_ARG(pv_get_info, argKernel, TUNER_CAB_DIG_OOB_RX_TUNE_INFO_T);
                }
            }
            else
            { /* if not connected */
                MTFE_TUNERHAL_PRINT("TunerGetOob: RMR_DRV_NO_GET_INFO!\n");
                MTFE_TUNERHAL_RETURN(RMR_DRV_NO_GET_INFO);
            }
            break;
        }
        case TUNER_GET_TYPE_SIGNAL_LEVEL:
        {
            if (*pz_size < sizeof(UINT8))
            { /* not enough space */
                *pz_size = sizeof(UINT8);
                USR_SPACE_ACCESS_VALIDATE_ARG(pz_get_info_len, SIZE_T);
                COPY_TO_USER_ARG(pz_get_info_len, z_size, SIZE_T);

                MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n");
                MTFE_TUNERHAL_RETURN(RMR_DRV_NOT_ENOUGH_SPACE);
            }
            else
            { /* get info */
                UINT8 argKernel;
                UINT8 *pt_tuner_info = &argKernel;
                USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, UINT8);
                COPY_FROM_USER_ARG(pv_get_info, argKernel, UINT8);
                
                *(UINT8 *)pt_tuner_info = TunerGetSignalLevel(ptTunerCtx->TunerID, ptStateInfo->e_conn_type);
                MTFE_TUNERHAL_PRINT("%s Signal level: (%d/100)\n", ptStateInfo->comp_name, *(UINT8 *)pt_tuner_info);

                USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, UINT8);
                COPY_TO_USER_ARG(pv_get_info, argKernel, UINT8);
            }
            break;
        }
        case TUNER_GET_TYPE_STATE:
        {
            if (*pz_size < sizeof(TUNER_STATE_INFO_T))
            { /* not enough space */
                *pz_size = sizeof(TUNER_STATE_INFO_T);
                USR_SPACE_ACCESS_VALIDATE_ARG(pz_get_info_len, SIZE_T);
                COPY_TO_USER_ARG(pz_get_info_len, z_size, SIZE_T);
                
                MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n");
                MTFE_TUNERHAL_RETURN(RMR_DRV_NOT_ENOUGH_SPACE);
            }
            else
            { /* get info */
                TUNER_STATE_INFO_T argKernel;
                TUNER_STATE_INFO_T *pt_tuner_state = &argKernel;    
                
                USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, TUNER_STATE_INFO_T);
                COPY_FROM_USER_ARG(pv_get_info, argKernel, TUNER_STATE_INFO_T);
                
                pt_tuner_state->e_state = ptStateInfo->e_tuner_state;
                if ((pt_comp_id->e_type == DRVT_TUNER_CAB_ANA) ||
                    (pt_comp_id->e_type == DRVT_TUNER_TER_ANA) ||
                    (pt_comp_id->e_type == DRVT_TUNER_SAT_ANA))
                {
                    pt_tuner_state->b_dig_signal = (FALSE);
                }
                else
                {
                    pt_tuner_state->b_dig_signal = (TRUE);
                }
                MTFE_TUNERHAL_PRINT("%s Tuner state: (%d)\n",ptStateInfo->comp_name, ptStateInfo->e_tuner_state);
                MTFE_TUNERHAL_PRINT("%s Current HW sync state: (%d)\n",ptStateInfo->comp_name,
                                       TunerGetSync(ptTunerCtx->TunerID, ptStateInfo->e_conn_type));

                USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, TUNER_STATE_INFO_T);
                COPY_TO_USER_ARG(pv_get_info, argKernel, TUNER_STATE_INFO_T);
            }
            break;
        }
        case TUNER_GET_TYPE_NO_DISC_NFY:
        {
            if (*pz_size < sizeof(BOOL))
            { /* not enough space */
                *pz_size = sizeof(BOOL);
                USR_SPACE_ACCESS_VALIDATE_ARG(pz_get_info_len, SIZE_T);
                COPY_TO_USER_ARG(pz_get_info_len, z_size, SIZE_T);
                
                MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n");
                MTFE_TUNERHAL_RETURN(RMR_DRV_NOT_ENOUGH_SPACE);
            }
            else
            { /* get info */
                BOOL argKernel;
                BOOL *pt_tuner_info = &argKernel;
                USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, BOOL);
                COPY_FROM_USER_ARG(pv_get_info, argKernel, BOOL);
                
                *(BOOL *)pt_tuner_info = ptStateInfo->b_disable_nfy;

                USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, BOOL);
                COPY_TO_USER_ARG(pv_get_info, argKernel, BOOL);
            }
            break;
        }
        case TUNER_GET_TYPE_FE_STATUS:
        {
            if (*pz_size < sizeof(UINT8))            
            { /* not enough space */
                *pz_size = sizeof(UINT8);

                USR_SPACE_ACCESS_VALIDATE_ARG(pz_get_info_len, SIZE_T);
                COPY_TO_USER_ARG(pz_get_info_len, z_size, SIZE_T);
                
                MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n");
                MTFE_TUNERHAL_RETURN(RMR_DRV_NOT_ENOUGH_SPACE);
            } 
            else 
            { /* get info */
                UINT8 argKernel;
                UINT8 *pt_tuner_info = &argKernel;
                USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, UINT8);
                COPY_FROM_USER_ARG(pv_get_info, argKernel, UINT8);
                
                #ifdef CC_DUAL_TUNER_SUPPORT
                    *(UINT8 *)pt_tuner_info = fe_status_get(pt_comp_id);
                #else
                    *(UINT8 *)pt_tuner_info = fe_status_get();
                #endif

                USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, UINT8);
                COPY_TO_USER_ARG(pv_get_info, argKernel, UINT8);
            }            
            break;
        }
        case TUNER_GET_TYPE_DBM_SIGNAL_LEVEL:
        {
            if (*pz_size < sizeof(INT16))
            { /* not enough space */
                *pz_size = sizeof(INT16);
                USR_SPACE_ACCESS_VALIDATE_ARG(pz_get_info_len, SIZE_T);
                COPY_TO_USER_ARG(pz_get_info_len, z_size, SIZE_T);
                
                MTFE_TUNERHAL_PRINT("TunerGet: RMR_DRV_NOT_ENOUGH_SPACE!\n");
                MTFE_TUNERHAL_RETURN(RMR_DRV_NOT_ENOUGH_SPACE);
            }
            else
            { /* get info */
                INT16 argKernel;
                INT16 *pt_tuner_info = &argKernel;
                
                USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, INT16);
                COPY_FROM_USER_ARG(pv_get_info, argKernel, INT16);
                
                *(INT16 *)pt_tuner_info = TunerGetSignalLeveldBm(ptTunerCtx->TunerID, ptStateInfo->e_conn_type);
                MTFE_TUNERHAL_PRINT("%s (%d dBm)\n", ptStateInfo->comp_name, *(INT16 *) pt_tuner_info / 10);

                USR_SPACE_ACCESS_VALIDATE_ARG(pv_get_info, INT16);
                COPY_TO_USER_ARG(pv_get_info, argKernel, INT16);
            }
            break;
        }

        case TUNER_GET_TYPE_ANA_ATTRIBUTE:
        case TUNER_GET_TYPE_TV_SYS_GROUP:
        case TUNER_GET_TYPE_TV_SYS_MASK:
        {
            /*
             * The maximun kernel buffer structure is TUNER_ANA_ATTRIBUTE_T.
            */
            TUNER_ANA_ATTRIBUTE_T argKernel;
            TUNER_ANA_ATTRIBUTE_T *pt_tuner_info = &argKernel;

            USR_SPACE_ACCESS_VALIDATE_ARG_K(pv_get_info, sizeof(TUNER_ANA_ATTRIBUTE_T));
            COPY_FROM_USER_ARG_K(pv_get_info, &argKernel, sizeof(TUNER_ANA_ATTRIBUTE_T));
                
            if (pt_comp_id->e_type==DRVT_TUNER_TER_ANA)
            {
                eResult = (TunerGetAttribute2(ptTunerCtx->TunerID, e_get_type, pt_tuner_info, pz_size, TUNER_CONN_TYPE_TER_ANA));
            }
            else if (pt_comp_id->e_type==DRVT_TUNER_CAB_ANA)
            {
                eResult = (TunerGetAttribute2(ptTunerCtx->TunerID, e_get_type, pt_tuner_info, pz_size, TUNER_CONN_TYPE_CAB_ANA));
            }

            USR_SPACE_ACCESS_VALIDATE_ARG_K(pv_get_info, *pz_size);
            COPY_TO_USER_ARG_K(pv_get_info, argKernel, *pz_size);            

            USR_SPACE_ACCESS_VALIDATE_ARG(peResult,INT32);
            COPY_TO_USER_ARG(peResult,eResult,INT32);

            return MTR_OK;
        }

        case TUNER_GET_TYPE_BER:
        case TUNER_GET_TYPE_DIG_ATTRIBUTE:
        case TUNER_GET_TYPE_RAW_SIGNAL_LEVEL:
        case TUNER_GET_TYPE_SIGNAL_LEVEL_TOLERANCE:
        case TUNER_GET_TYPE_BER_TOLERANCE:
        case TUNER_GET_TYPE_EWS_STATUS:
        {
            /*
             * The maximun kernel buffer structure is TUNER_DIG_ATTRIBUTE_T.
            */
            TUNER_DIG_ATTRIBUTE_T argKernel;
            TUNER_DIG_ATTRIBUTE_T *pt_tuner_info = &argKernel;

            USR_SPACE_ACCESS_VALIDATE_ARG_K(pv_get_info, sizeof(TUNER_DIG_ATTRIBUTE_T));
            COPY_FROM_USER_ARG_K(pv_get_info, &argKernel, sizeof(TUNER_DIG_ATTRIBUTE_T));
            
            if ((pt_comp_id->e_type==DRVT_TUNER_TER_ANA) || (pt_comp_id->e_type==DRVT_TUNER_TER_DIG))
            {
                eResult = (TunerGetAttribute2(ptTunerCtx->TunerID, e_get_type, pt_tuner_info, pz_size, TUNER_CONN_TYPE_TER_DIG));
            }
            else if ((pt_comp_id->e_type==DRVT_TUNER_CAB_ANA) || (pt_comp_id->e_type==DRVT_TUNER_CAB_DIG))
            {
                eResult = (TunerGetAttribute2(ptTunerCtx->TunerID, e_get_type, pt_tuner_info, pz_size, TUNER_CONN_TYPE_CAB_DIG));
            }
            else if ((pt_comp_id->e_type==DRVT_TUNER_SAT_ANA) || (pt_comp_id->e_type==DRVT_TUNER_SAT_DIG))
            {
                eResult = (TunerGetAttribute2(ptTunerCtx->TunerID, e_get_type, pt_tuner_info, pz_size, TUNER_CONN_TYPE_SAT_DIG));
            }
            else
            {
                eResult = (TunerGetAttribute(ptTunerCtx->TunerID, e_get_type, pt_tuner_info, pz_size));
            }

            USR_SPACE_ACCESS_VALIDATE_ARG_K(pv_get_info, *pz_size);
            COPY_TO_USER_ARG_K(pv_get_info, argKernel, *pz_size);            

            USR_SPACE_ACCESS_VALIDATE_ARG(peResult,INT32);
            COPY_TO_USER_ARG(peResult,eResult,INT32);

            return MTR_OK;
        }
                
        default:
        {
            /*
             * The maximun kernel buffer structure is TUNER_DVBT2_PLP_LIST_INFO.
            */
            TUNER_DVBT2_PLP_LIST_INFO argKernel;
            TUNER_DVBT2_PLP_LIST_INFO *pt_tuner_info = &argKernel;

            USR_SPACE_ACCESS_VALIDATE_ARG_K(pv_get_info, sizeof(TUNER_DVBT2_PLP_LIST_INFO));
            COPY_FROM_USER_ARG_K(pv_get_info, &argKernel, sizeof(TUNER_DVBT2_PLP_LIST_INFO));
            
            if ((pt_comp_id->e_type==DRVT_TUNER_TER_ANA) || (pt_comp_id->e_type==DRVT_TUNER_TER_DIG))
            {
                eResult = (TunerGetAttribute2(ptTunerCtx->TunerID, e_get_type, pt_tuner_info, pz_size, TUNER_CONN_TYPE_TER_DIG));
            }
            else if ((pt_comp_id->e_type==DRVT_TUNER_CAB_ANA) || (pt_comp_id->e_type==DRVT_TUNER_CAB_DIG))
            {
                eResult = (TunerGetAttribute2(ptTunerCtx->TunerID, e_get_type, pt_tuner_info, pz_size, TUNER_CONN_TYPE_CAB_DIG));
            }
            else if ((pt_comp_id->e_type==DRVT_TUNER_SAT_ANA) || (pt_comp_id->e_type==DRVT_TUNER_SAT_DIG))
            {
                eResult = (TunerGetAttribute2(ptTunerCtx->TunerID, e_get_type, pt_tuner_info, pz_size, TUNER_CONN_TYPE_SAT_DIG));
            }
            else
            {
                eResult = (TunerGetAttribute(ptTunerCtx->TunerID, e_get_type, pt_tuner_info, pz_size));
            }

            USR_SPACE_ACCESS_VALIDATE_ARG_K(pv_get_info, *pz_size);
            COPY_TO_USER_ARG_K(pv_get_info, argKernel, *pz_size);            

            USR_SPACE_ACCESS_VALIDATE_ARG(peResult,INT32);
            COPY_TO_USER_ARG(peResult,eResult,INT32);

            return MTR_OK;            
        }
    }

    eResult = RMR_OK;
    //pt_comp_id,pv_conn_info leave to _MTFE_TUNERHAL_Connect 
    USR_SPACE_ACCESS_VALIDATE_ARG(peResult,INT32);
    COPY_TO_USER_ARG(peResult,eResult,INT32);
    
    return eRet;
}



/**Set visual carrier frequency.
*@param  rParameter             The input paramter (Visual carrier freq...)
*@retval MT_OK                  Success.
*@retval MT_NOT_OK              Fail.
*/

static MT_RESULT_T _MTFE_TUNERHAL_Set(unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    
    MTAL_IOCTL_5ARG_T rArg;
    DRV_COMP_ID_T *pt_comp_id;
    DRV_SET_TYPE_T e_set_type;
    const VOID *pv_set_info;
    SIZE_T z_set_info_len;
    INT32 *peResult;
    INT32 eResult;

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);
    
    pt_comp_id = (DRV_COMP_ID_T*)rArg.ai4Arg[0];
    e_set_type = (DRV_SET_TYPE_T)rArg.ai4Arg[1];
    pv_set_info = (const VOID*)rArg.ai4Arg[2];
    z_set_info_len = (SIZE_T)rArg.ai4Arg[3];
    peResult = (INT32*)rArg.ai4Arg[4];
    
    eResult = TunerSet(pt_comp_id, e_set_type, pv_set_info, z_set_info_len);

    //pt_comp_id,pv_conn_info leave to _MTFE_TUNERHAL_Connect 
    USR_SPACE_ACCESS_VALIDATE_ARG(peResult,INT32);
    COPY_TO_USER_ARG(peResult,eResult,INT32);
    
    return eRet;
}

static int _MTFE_TUNERHAL_IniThreadNfy(UINT16 TunerID, UINT16 DrvType)
{
    MTFE_TUNERHAL_CB_T  rTunerHalCbT;

    rTunerHalCbT.u4Arg1 = TunerID;
    rTunerHalCbT.u4Arg2 = DrvType;
    
    _CB_PutEvent(CB_MTAL_FE_TUNERHAL_INI_THREAD_NFY, sizeof(MTFE_TUNERHAL_CB_T), (void*)&rTunerHalCbT);
    return MTR_OK;
}

static int _MTFE_TUNERHAL_TunerProcNfy(UINT16 TunerID1, UINT16 DrvType1, UINT16 TunerID2, UINT16 DrvType2)
{
    MTFE_TUNERHAL_CB_T  rTunerHalCbT1;

    rTunerHalCbT1.u4Arg1 = TunerID1;
    rTunerHalCbT1.u4Arg2 = DrvType1;    
    rTunerHalCbT1.u4Arg3 = TunerID2;
    rTunerHalCbT1.u4Arg4 = DrvType2;
    
    _CB_PutEvent(CB_MTAL_FE_TUNERHAL_TUNER_PROC_NFY, sizeof(MTFE_TUNERHAL_CB_T), (void*)&rTunerHalCbT1);
    return MTR_OK;
}


/**Register tunerhal module callback function.
*@param  pfCb               Callback function.
*@retval MT_OK              Success.
*@retval MT_NOT_OK          Fail.
*/

static MT_RESULT_T _MTFE_TUNERHAL_RegCb(unsigned long arg)
{
    //add driver implementation code here    
    MTAL_IOCTL_3ARG_T rArg;
    MTFE_TUNERHAL_CB_FUNC_ENUM_T eFuncType;
    UINT32 u4FuncPtr;
    UINT32 u4Arg1;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    
    eFuncType = (MTFE_TUNERHAL_CB_FUNC_ENUM_T)rArg.ai4Arg[0];
    u4FuncPtr = (UINT32)rArg.ai4Arg[1];
    u4Arg1 = (UINT32)rArg.ai4Arg[2];

    printf(" -IO_MTFE_TUNERHAL_RegCb: eFuncType = %d, u4FuncPtr = %ul, u4Arg1 = %ul\n",
        eFuncType, u4FuncPtr, u4Arg1);

    switch(eFuncType)
    {
        case MTFE_TUNERHAL_CB_FUNC_INI_THREAD:
        //TUNERHAL_IniThreadNfy((INT32)_MTFE_TUNERHAL_IniThreadNfy);
        {
            fptIniThreadNfy = _MTFE_TUNERHAL_IniThreadNfy;		
        }
        break;
          
        case MTFE_TUNERHAL_CB_FUNC_TUNER_PROC:
		//TUNERHAL_TunerProcNfy((INT32)_MTFE_TUNERHAL_TunerProcNfy);          
        {
            fptTunerProcNfy = _MTFE_TUNERHAL_TunerProcNfy;
        }
        break;
        
        default:
            return MTR_NOT_OK;
            break;
    }
    return MTR_OK;
}

static MT_RESULT_T _MTFE_TUNERHAL_RetrieveTunerCfg(unsigned long arg)
{
    INT32 rArg;
    TUNER_CONFIG_TBL_T rCfg, *prCfg;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, rArg);

    prCfg = (TUNER_CONFIG_TBL_T*)rArg;

    if(TDIS_RetrieveTunerCfgTbl(&rCfg) != 0)
    {
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG_K(prCfg, sizeof(TUNER_CONFIG_TBL_T));
    COPY_TO_USER_ARG_K(prCfg, rCfg, sizeof(TUNER_CONFIG_TBL_T));
    
    return MTR_OK;
}


static MT_RESULT_T _MTFE_TUNERHAL_GetCompName(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT8 *pu1Buf;
    UINT8 au1Buf[256];
    UINT16 u2TunerID;
    DRV_TYPE_T eDrvType;   
    char *tmp_comp_name = NULL;
    
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);    
    pu1Buf = (UINT8*)rArg.ai4Arg[0];
    u2TunerID = (UINT16)rArg.ai4Arg[1];
    eDrvType = (DRV_TYPE_T)rArg.ai4Arg[2];

    tmp_comp_name = LTDIS_GetCompNameForMW(u2TunerID, eDrvType);
    if (tmp_comp_name) {
        x_memset((void*)au1Buf, 0, 256);
        x_strncpy((char*)au1Buf , tmp_comp_name, 256);
        au1Buf[255] = '\0';
    }
    
    USR_SPACE_ACCESS_VALIDATE_ARG_K(pu1Buf, 256);
    COPY_TO_USER_ARG_K(pu1Buf, au1Buf, 256);

    return MTR_OK;
}

int mtal_ioctl_mtfe_tunerhal(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    //MTFE_TUNERHAL_PRINT("-------tunerhal ioctl cmd-------\r\n");
    if(u1ExclSemaInit==1) {
        mcSEMA_LOCK_MTFE_S(Exclusive_DigiAnaSema,"Ex_DiAna");
    }

    switch (cmd)
    {
        case MTAL_IO_TUNERHAL_INIT:
            eRet = _MTFE_TUNERHAL_Init();
            break;
        case MTAL_IO_TUNERHAL_CONNECT:
            eRet= _MTFE_TUNERHAL_Connect(arg);
            break;
        case MTAL_IO_TUNERHAL_DISCONNECT:
            eRet= _MTFE_TUNERHAL_DisConnect(arg);
            break;
        case MTAL_IO_TUNERHAL_SET:
            eRet= _MTFE_TUNERHAL_Set(arg);
            break;
        case MTAL_IO_TUNERHAL_GET:
            eRet= _MTFE_TUNERHAL_Get(arg);
            break;
        case MTAL_IO_TUNERHAL_REGCB:
            eRet= _MTFE_TUNERHAL_RegCb(arg);
            break;
        case MTAL_IO_TUNERHAL_RETRIEVETUNERCFG:
            eRet = _MTFE_TUNERHAL_RetrieveTunerCfg(arg);
            break;
        case MTAL_IO_TUNERHAL_GETCOMPNAME:
            eRet = _MTFE_TUNERHAL_GetCompName(arg);
            break;
        default:
            break;
    }

    if(u1ExclSemaInit==1) {
        mcSEMA_UNLOCK_MTFE_S(Exclusive_DigiAnaSema, "Ex_DiAna");
    }

    return eRet;
}

/********************************************************************/
//                 MTAL Fast Boot Function,lei140207
/********************************************************************/
#ifdef CC_SUPPORT_STR
#ifdef CC_APP_IF_MTAL
BOOL Mtal_Get_STR_Mode(void)
{
    return u1MtalResumeEnd;
}
void Mtal_Resume_Pro_Mode(void)
{
	mcSEMA_LOCK(t_Resume);
    mcSEMA_UNLOCK(t_Resume);
}
void Mtal_tuner_pm_str_suspend(void)
{
	MT_RESULT_T 		rRet;
	
	u1MtalResumeEnd = FALSE;

	#ifdef CC_FEDTD
	#endif
	
	#ifdef CC_FEISDB
	#endif
	
	#ifdef CC_FEDVBT
	//rRet = _MTFE_DVBT_Suspend();
	#endif
	
	#ifdef CC_FEDVBC
	rRet = _MTFE_DVBC_Suspend();
	#endif
			
	#ifdef CC_FEDTMB      
	#endif
	
	#if defined(ENABLE_SIF_BYPASS) && !defined(CC_FEATD)//common ATD
	rRet = _MTFE_PAL_Suspend();
	#endif

	#if defined(CC_FEATD)
	rRet = _MTFE_PAL_Suspend();
	#endif
	Printf("[DEMOD]TunerClose\n");
	TunerClose();
	
	if (t_Resume)
    {
        mcSEMA_DESTROY(t_Resume);
		Printf("[DEMOD]t_Resume (mcSEMA_DESTROY): Finished!\n");
    }
}
void Mtal_tuner_pm_str_resume(void)
{
	MT_RESULT_T 		rRet;

	if (mcSEMA_CREATE(&t_Resume, X_SEMA_STATE_UNLOCK) != OSR_OK)
    {
        Printf("[DEMOD]t_Resume (mcSEMA_CREATE): ERROR!\n");
        return;   
    }
	Printf("[DEMOD]Resume t_Resume Create\n");
	mcSEMA_LOCK(t_Resume);
	Printf("[DEMOD]Resume t_Resume lock\n");

	#ifdef CC_FEDTD
	#endif
	
	#ifdef CC_FEISDB
	#endif
	
	#ifdef CC_FEDVBT
	//rRet = _MTFE_DVBT_Resume();
	#endif
	
	#ifdef CC_FEDVBC
	rRet = _MTFE_DVBC_Resume();
	#endif
			
	#ifdef CC_FEDTMB	  
	#endif
	
	#if defined(ENABLE_SIF_BYPASS) && !defined(CC_FEATD)//common ATD
	rRet = _MTFE_PAL_Resume();
	#endif

	#if defined(CC_FEATD)
	rRet = _MTFE_PAL_Resume();
	#endif

	u1MtalResumeEnd = TRUE;
    mcSEMA_UNLOCK(t_Resume);
	Printf("[DEMOD]Resume t_Resume unlock\n");
}
#endif
#endif


