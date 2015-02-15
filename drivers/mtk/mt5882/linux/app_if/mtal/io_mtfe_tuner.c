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
 * $RCSfile: io_mtfe_tuner.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file io_mtfe_tuner.c
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "tuner_if.h"
#include "io_mtfe_pd_def.h"
#include "tuner_interface_if.h"
#include "io_mtfe_tuner.h"
#include <linux/mtal_ioctl.h>


//-----------------------------------------------------------------------------
// debug switch
//-----------------------------------------------------------------------------
//#ifdef DEBUG_TUNER_PERFORMANCE

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define USR_SPACE_ACCESS_VALIDATE_1ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(int)))                            \
    {                                                       \
        printk("mtal_ioctl_tuner argument error\n");     \
        return -1;                                          \
    }

#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(int)))                        \
    {                                                       \
        printk("mtal_ioctl_tuner argument error\n");      \
        return -1;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_tuner argument error\n");     \
        return -1;                                          \
    }

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_tuner argument error\n");      \
        return -1;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_tuner argument error\n");     \
        return -1;                                          \
    }

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_tuner argument error\n");      \
        return -1;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        printk("mtal_ioctl_tuner argument error\n");     \
        return MTR_ERR_INV;                                          \
    }

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_tuner argument error\n");      \
        return MTR_ERR_INV;                                          \
    }
#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_from_user(&ArgKernel,(void __user *)ArgUsr,    \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_tuner argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static MTFE_TUNER_PD_CTX_T *pMTPdCtx;
static BOOL _fgTunerInit=FALSE;
static HANDLE_T    Exclusive_DigiAnaSema=0;
static UINT8       u1ExclSemaInit=0;


//-----------------------------------------------------------------------------
// Static Functions
//-----------------------------------------------------------------------------
extern ITUNER_CTX_T *ITunerGetCtx(VOID);
extern void DigTunerBypassI2C(BOOL bSwitchOn);

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTFE_TUNER_Init(VOID)
{
	MTFE_TUNER_PRINT("In kernel %s\n",__FUNCTION__);
	if(TRUE==_fgTunerInit)
	{
		MTFE_TUNER_PRINT("_fgTunerInit == TRUE, Init done!\n");
		return (MTR_TUNER_ALREADY_INIT);
	}
	pMTPdCtx = (MTFE_TUNER_PD_CTX_T *)x_mem_calloc(1,sizeof(MTFE_TUNER_PD_CTX_T));
	if(NULL == pMTPdCtx)
	{
		MTFE_TUNER_PRINT("MTFE_TUNER_PD_CTX_T malloc fail!\n");
		return (MTR_NOT_OK);
	}
	if (x_sema_create(&(pMTPdCtx->hHalLock), X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        MTFE_TUNER_PRINT(("Tuner x_sema_create(pMTPdCtx->hHalLock) Fail!\n"));
        return (MTR_NOT_OK);
    }
    x_sema_unlock(pMTPdCtx->hHalLock);
	if (x_sema_create(&Exclusive_DigiAnaSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) != OSR_OK)
	{
		MTFE_TUNER_PRINT(("Tuner x_sema_create(Exclusive_DigiAnaSema) Fail!\n"));
		return (MTR_NOT_OK);
	}
	x_sema_unlock(Exclusive_DigiAnaSema);
	u1ExclSemaInit = 1;

	_fgTunerInit = TRUE;

	return MTR_OK;
}

static MT_RESULT_T _MTFE_TUNER_SetFreq(unsigned long arg)
{
	MTFE_TUNER_TUNE_PARAMETER_T *ptTunePara;
	MTFE_TUNER_TUNE_PARAMETER_T tTunePara;
	PARAM_SETFREQ_T SetFreqPara;

    ptTunePara = (MTFE_TUNER_TUNE_PARAMETER_T *)arg;
	MTFE_TUNER_PRINT("In kernel %s\n",__FUNCTION__);
	USR_SPACE_ACCESS_VALIDATE_ARG(ptTunePara,MTFE_TUNER_TUNE_PARAMETER_T);
    COPY_FROM_USER_ARG(ptTunePara, tTunePara,MTFE_TUNER_TUNE_PARAMETER_T);
	MTFE_TUNER_PRINT(" u4FrequencyInKhz = %d\n"
        "eBandWidth = %d\n"
        "e_mod = %d\n"
        "fgAutoSearch = %d\n",
        (INT32)tTunePara.u4FrequencyInKhz,
        (INT32)tTunePara.eBandWidth,
        tTunePara.eMod,
        tTunePara.fgAutoSearch
        );
	pMTPdCtx->tCurTunePara.u4FrequencyInKhz = tTunePara.u4FrequencyInKhz;
	pMTPdCtx->tCurTunePara.eBandWidth = tTunePara.eBandWidth;
	pMTPdCtx->tCurTunePara.eMod = tTunePara.eMod;
	pMTPdCtx->tCurTunePara.fgAutoSearch = tTunePara.fgAutoSearch;

	mcSEMA_LOCK_MTFE(pMTPdCtx->hHalLock); //when access PI Primitive API, must use this lock for protection
	
	ITuner_OP(ITunerGetCtx(), itSetSawBw, tTunePara.eBandWidth, NULL);
	SetFreqPara.Freq = tTunePara.u4FrequencyInKhz;
	SetFreqPara.Modulation = tTunePara.eMod;
	SetFreqPara.fgAutoSearch = tTunePara.fgAutoSearch;
	DigTunerBypassI2C(TRUE); //Need mutex with digital
	if((ITuner_SetFreq(ITunerGetCtx(), &SetFreqPara)) != 0)
	{
		MTFE_TUNER_PRINT("Fail at ITuner_SetFreq!\n");
		return (MTR_NOT_OK);
	}
	DigTunerBypassI2C(FALSE);
	
	mcSEMA_UNLOCK_MTFE(pMTPdCtx->hHalLock);
	
	return MTR_OK;
}

static MT_RESULT_T _MTFE_TUNER_OP(unsigned long arg)
{
	MTFE_OP_PAREMETER_T  tOPPara;
	INT32 *pi4TunerGetValue;
	INT32 i4TunerGetValue;
	MT_RESULT_T  eRet = MTR_OK;
	INT16 i1Ret;
	USR_SPACE_ACCESS_VALIDATE_ARG(arg,MTFE_OP_PAREMETER_T);
    COPY_FROM_USER_ARG(arg, tOPPara,MTFE_OP_PAREMETER_T);
	pi4TunerGetValue=(INT32*)tOPPara.i4GetValue;
	MTFE_TUNER_PRINT(" OPType = %d\n""SetValue = %d\n",(INT32)tOPPara.eOP,(INT32)tOPPara.u1SetValue);
	mcSEMA_LOCK_MTFE(pMTPdCtx->hHalLock);
	i1Ret=ITuner_OP(ITunerGetCtx(), tOPPara.eOP,tOPPara.u1SetValue,&i4TunerGetValue);
	MTFE_TUNER_PRINT("GetValue = %d\n",i4TunerGetValue);
	mcSEMA_UNLOCK_MTFE(pMTPdCtx->hHalLock);
	if(i1Ret==ITUNER_NOT_SUPPORTED)
    {
        eRet=MTR_NOT_SUPPORTED;
    }
    else if(i1Ret==ITUNER_NOT_OK)
    {
        eRet=MTR_NOT_OK;
    }
    else//Ok
    {
		USR_SPACE_ACCESS_VALIDATE_ARG(pi4TunerGetValue,INT32);
		COPY_TO_USER_ARG(pi4TunerGetValue,i4TunerGetValue,INT32);
		eRet=MTR_OK;
	}
	return eRet;
}

int mtal_ioctl_mtfe_tuner(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg)
{
    MT_RESULT_T eRet = MTR_OK;
    MTFE_TUNER_PRINT("-------tuner ioctl cmd-------\r\n");
    if(u1ExclSemaInit==1){
        mcSEMA_LOCK_MTFE_S(Exclusive_DigiAnaSema,"Ex_DiAna");
    }
	switch (cmd)
	{
		case MTAL_IO_TUNER_INIT:
			eRet=_MTFE_TUNER_Init();
			break;
		case MTAL_IO_TUNER_SETFREQ:
			eRet=_MTFE_TUNER_SetFreq(arg);
            break;
		case MTAL_IO_TUNER_OP:
			eRet=_MTFE_TUNER_OP(arg);
			break;
		default:
		    break;
	}
    if(u1ExclSemaInit==1){
        mcSEMA_UNLOCK_MTFE_S(Exclusive_DigiAnaSema, "Ex_DiAna");
    }
	return eRet;
}

